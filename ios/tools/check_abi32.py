# SPDX-License-Identifier: GPL-3.0-or-later
"""Check original 32-bit layouts through build-time C generation, never a runtime VM."""
import os
import re
import argparse
from concurrent.futures import ThreadPoolExecutor
import hashlib
import json
from pathlib import Path
import subprocess
import signal
import sys

ROOT = Path(__file__).resolve().parents[2]
BUILD = ROOT / 'ios/build-original-abi32'
TOOLS = Path(os.environ.get('HG_ABI32_TOOLS', ROOT / 'ios/build-original-abi32/tools'))
SDK = TOOLS / 'wasi-sdk-34.0-arm64-macos'
WABT = TOOLS / 'wabt-1.0.42'
BUILD.mkdir(parents=True, exist_ok=True)


def prepare_tools():
    # shortcut: bootstrap packages target Apple Silicon macOS; add pinned host packages for other build hosts.
    packages = [
        ('wabt-1.0.42-macos-arm64.tar.gz', '3f654779b436c628db3ca4323c51538815f5fbc887a6d5f3bb1c18d36281e240',
         'https://github.com/WebAssembly/wabt/releases/download/1.0.42/', WABT / 'bin/wasm2c'),
        ('wasi-sdk-34.0-arm64-macos.tar.gz', '9c59398106b417f8f14913380fdf0097a8cc0ff4af9eb3ce0065a859e88d49e9',
         'https://github.com/WebAssembly/wasi-sdk/releases/download/wasi-sdk-34/', SDK / 'bin/clang'),
    ]
    TOOLS.mkdir(parents=True, exist_ok=True)
    for name, expected, url, binary in packages:
        archive = TOOLS / name
        if not archive.is_file():
            temporary = archive.with_suffix('.download')
            run(['curl','--fail','--location','--silent','--show-error',url+name,'-o',temporary],name+'.download.log')
            if hashlib.sha256(temporary.read_bytes()).hexdigest() != expected:
                raise RuntimeError(f'Tool download checksum mismatch: {name}')
            temporary.replace(archive)
        if hashlib.sha256(archive.read_bytes()).hexdigest() != expected:
            raise RuntimeError(f'Tool archive checksum mismatch: {archive}')
        if not binary.is_file(): run(['tar','-xzf',archive,'-C',TOOLS],name+'.extract.log')


def link_engine(link,listing,destination,native_content=None):
    target=destination/'engine.wasm'; target.unlink(missing_ok=True)
    command=[*map(str,link),'-Wl,--export=NitroMain','-Wl,--export=HGNativeSystemWorkAddress','-Wl,--export=HGNativeRegisterAddress',
             '-Wl,--export=HGNativeFrameBuffer','-Wl,--export=HGNativeRender2D',
             '-Wl,--export=HGNativeRender2DView','-Wl,--export=HGNativeRenderBattleView','-Wl,--export=HGNativeSetViewWidth','-Wl,--export=HGNativeGeometryFrame',
             '-Wl,--export=HGNativeBattleUIGetState','-Wl,--export=HGNativeBattleUIChoose','-Wl,--export=HGNativeFieldMap','-Wl,--export=HGNativeFieldMenuIcons','-Wl,--error-limit=0',
             '@'+str(listing),'-o',str(target)]
    result=subprocess.run(command,cwd=ROOT,capture_output=True,text=True)
    diagnostics=result.stdout+result.stderr
    (destination/'engine-link.log').write_text(diagnostics)
    if result.returncode: target.unlink(missing_ok=True)
    report=dict(scope='strict final intermediate link rooted at NitroMain; native C/App linking and gameplay unverified',
                exitCode=result.returncode,command=command,
                undefinedSymbols=sorted(set(re.findall(r'undefined symbol: ([^\n]+)',diagnostics))),
                objectListSha256=hashlib.sha256(listing.read_bytes()).hexdigest(),
                outputSha256=hashlib.sha256(target.read_bytes()).hexdigest() if not result.returncode else None,nativeContent=native_content)
    (destination/'engine-link.json').write_text(json.dumps(report,indent=2)+'\n')
    return report


def native_target_selection(units, flags):
    exception_libraries = [str(SDK/'share/wasi-sysroot/lib/wasm32-wasip1/eh'/name) for name in ('libc++.a','libc++abi.a','libunwind.a')]
    alternatives = {
        'lib/NitroSDK/src/os/os_protectionUnit.c': ('b6a182701a0db47ea37df60e8bef4b38c56ae7fac190a9200f03364d1584efe8', 'ARM9 MPU',
            'CP15 protection APIs are unavailable on the native target; native arena/exception paths do not call them.', ['ios/Original/NativeOSMemory.c','ios/Original/NativeException.c']),
        'lib/NitroSDK/src/os/os_protectionRegion.c': ('409a2145300071cfe63f46fe443ab8f154b6594c1268d2810d59030d0aeddaf2', 'ARM9 MPU',
            'Native arenas replace ARM9 region setup; CTRDG_Enable excludes physical-cartridge MPU changes under HEARTGOLD_ABI32.', ['ios/Original/NativeOSMemory.c','lib/NitroSDK/src/ctrdg/CTRDG_common.c']),
        'lib/NitroSDK/src/os/os_reset_boot.c': ('ec171725a2786241fbc4ae45a878bd623d53982e58de3b3b68ea46dce1a7d182', 'ARM/PXI boot',
            'The native reset branch requests owner retirement and instance recreation instead of jumping to ARM boot code.', ['lib/NitroSDK/src/os/os_reset_system.c','ios/Original/abi32/NativeThreadImports.c','ios/OriginalApp/main.m']),
        'lib/NitroSDK/src/os/os_reset_clear32.c': ('4661d19d1948c238b70da6a9e0f96e65e38a8c04fee6aac65b55a6a1411d9e9b', 'ARM/PXI boot',
            'This private clear routine is called only by the excluded ARM boot branch; native reset allocates a fresh instance.', ['lib/NitroSDK/src/os/os_reset_core.c','ios/Original/abi32/NativeThreadImports.c','ios/OriginalApp/main.m']),
        'lib/MSL_C/src/msl_fp_runtime.c': ('4772579d74e6406a595ce01ebbbcc45b1f64bf7b7b79833b16b509f591c1dcaa', 'MWCC numeric runtime',
            'Fourteen integer and seven float C entries cover current explicit callers; Clang lowers ordinary arithmetic. The remaining 63 packed aliases are not ported.', ['ios/Original/NativeIntegerRuntime.c','ios/Original/NativeFloatRuntime.c']),
        'lib/MSL_C/src/msl_unexpected.cpp': ('c9d0058f47233f3e51f16b471b16f459c88ca118470ad24a3a76b5dc319757b3', 'MWCC exception ABI',
            'MWCC exception-magic/encoded RTTI is unavailable; native C++ uses Clang EH and WASI libc++abi/libunwind. Checked specifications and fibre EH remain unvalidated.', [*exception_libraries,'ios/tools/prepare_msl_native.py','ios/tools/prepare_exception_unwind.py']),
        'lib/MSL_C/src/msl_transfer_control.cpp': ('e9a64cf99da32c74f7008b1f95ba3413baa6b3af1604b7f3a5f7f0f7fcc01967', 'MWCC exception ABI',
            'Native C++ unwinding uses Clang EH and WASI libc++abi/libunwind, not MWCC register/stack restoration.', [*exception_libraries,'ios/tools/prepare_msl_native.py','ios/tools/prepare_exception_unwind.py']),
        'lib/MSL_C/src/msl_throw.c': ('d13a46f525a924cced40885315559e491314177e2041c24199acad789bb3cb72', 'MWCC exception ABI',
            'Native throw/rethrow use Clang EH and WASI libc++abi/libunwind; raw legacy throw APIs must not survive the final link.', [*exception_libraries,'ios/tools/prepare_msl_native.py','ios/tools/prepare_exception_unwind.py']),
    }
    options = list(map(str, flags))
    defines = [option for option in options if re.fullmatch(r'-[DU]HEARTGOLD_ABI32(?:=.*)?',option)]
    enabled = '--target=wasm32-wasip1' in options and bool(defines) and defines[-1] in ('-DHEARTGOLD_ABI32','-DHEARTGOLD_ABI32=1')
    errors = [] if enabled else ['Native SDK alternatives require wasm32-wasip1 and the HEARTGOLD_ABI32 target macro.']
    by_name = {unit['file']:unit for unit in units}
    excluded = []
    for name,(expected,category,reason,providers) in alternatives.items():
        unit = by_name.get(name); source = ROOT/name
        actual = hashlib.sha256(source.read_bytes()).hexdigest() if source.is_file() else None
        if unit is None or unit['sha256']!=expected or actual!=expected:
            errors.append(dict(file=name,expectedSha256=expected,compiledSha256=unit['sha256'] if unit else None,sourceSha256=actual))
        elif enabled:
            excluded.append(dict(file=name,sha256=actual,auditedSha256=expected,nativeSelected=False,
                                 category=category,reason=reason,providers=providers))
    omitted = {unit['file'] for unit in excluded}
    summary = {}
    for origin,prefix in [('game','src/'),('library','lib/'),('platform','ios/')]:
        selected = [unit for unit in units if unit['file'].startswith(prefix) and unit['file'] not in omitted]
        compiled = sum(unit['exitCode']==0 for unit in selected)
        summary[origin] = dict(total=len(selected),compiled=compiled,blocked=len(selected)-compiled)
    return dict(target='wasm32-wasip1',requiredDefine='HEARTGOLD_ABI32',enabled=enabled,
                excludedLibraryUnits=excluded,selectedCompileSummary=summary,errors=errors)


def native_retained_entries(module):
    result = subprocess.run([str(WABT/'bin/wasm-objdump'),'-x',str(module)],capture_output=True,text=True,check=True)
    rows = re.findall(r'(?m)^ - func\[(\d+)\] sig=\d+([^\n]*)$',result.stdout)
    defined = re.findall(r'(?m)^Function\[(\d+)\]:$',result.stdout)
    names = set(); imported = 0
    for index,tail in rows:
        entry = re.fullmatch(r' <(.+)>( <- \S+)?',tail)
        if not entry: raise ValueError('Retained function/import has no recognizable name: '+tail)
        names.add(entry[1]); imported += bool(entry[2])
        if entry[2]: names.add(entry[2].removeprefix(' <- '))
    if len(defined)!=1 or not rows or {int(index) for index,tail in rows}!=set(range(len(rows))) or len(rows)!=int(defined[0])+imported:
        raise ValueError('Incomplete retained function/import metadata')
    names.update(re.findall(r'(?m)^ - func\[\d+\][^\n]* -> "([^"\n]+)"$',result.stdout))
    forbidden = []
    for name in names:
        raw = name.removeprefix('env.')
        if re.match(r'^(?:OS_(?:Enable|Disable)ProtectionUnit|OS_SetDPermissionsForProtectionRegion|OS_SetProtectionRegion[0-7]|OSi_DoBoot|OSi_CpuClear32)(?:$|[.(])',raw) or re.match(r'^(?:_Z\d+)?__(?:throw|rethrow|unexpected|TransferControl|ThrowHandler)(?:$|[P.(])',raw):
            forbidden.append(name)
    return dict(checked=True,moduleSha256=hashlib.sha256(module.read_bytes()).hexdigest(),
                functions=len(rows),importedFunctions=imported,forbiddenSymbols=sorted(forbidden))


def audit(flags, includes, engine_link):
    destination = BUILD / 'audit'; destination.mkdir(exist_ok=True)
    for name in ('engine.wasm','engine-link.json','engine-link.log'): (destination/name).unlink(missing_ok=True)
    from prepare_native_content import prepare
    native_content=prepare(destination/'native-content',SDK/'bin/clang',flags)
    flags=list(flags); includes=list(includes)
    if native_content:
        content_include=ROOT/native_content['include']; includes.insert(0,content_include)
        flags=['-I',content_include,'-DHG_NATIVE_ROM_TABLES',*flags]
    headers = sorted({p for root in includes for pattern in ('**/*.h','**/*.naix') for p in root.glob(pattern)})
    headers=sorted(set(headers)|set((ROOT/'lib/MSL_C/src').glob('*.h'))|set((ROOT/'lib/MSL_C/src').glob('*.hpp')))
    def header_hash():
        value = hashlib.sha256()
        for path in headers: value.update(str(path.relative_to(ROOT)).encode()); value.update(path.read_bytes())
        return value.hexdigest()
    before = header_hash()
    from prepare_msl_native import legacy_flags
    specification = (ROOT/'main.lsf').read_text()
    from prepare_overlay_state import overlay_objects,compile_unit,catalog_source,NATIVE_OVERLAY_REPLACEMENTS
    overlay_registry=overlay_objects(specification)
    overlay_owners={obj:row['id'] for row in overlay_registry for obj in row['objects']}
    for row in overlay_registry:
        overlay_owners.update({obj:row['id'] for obj in NATIVE_OVERLAY_REPLACEMENTS.get(row['name'],[])})
    sources = sorted([*ROOT.glob('src/**/*.c'), *ROOT.glob('src/**/*.cpp')])
    library_objects = list(dict.fromkeys(re.findall(r'^\s*Object\s+(lib/[^\s]+\.o)',specification,re.MULTILINE)))
    library_sources = []
    for name in library_objects:
        source = next(((ROOT/name).with_suffix(suffix) for suffix in ('.c','.cpp') if (ROOT/name).with_suffix(suffix).is_file()),None)
        if source is not None: library_sources.append(source)
    platform_sources = [ROOT/'ios/Original/NativeGraphicsMemory.c',ROOT/'ios/Original/NativeCompression.c',ROOT/'ios/Original/NativeIntegerRuntime.c',ROOT/'ios/Original/NativeFloatRuntime.c',ROOT/'ios/Original/abi32/NativeIRQClient.c']
    platform_sources.append(ROOT/'ios/Original/NativeSystemCalls.c')
    platform_sources.append(ROOT/'ios/Original/NativeOSMemory.c')
    platform_sources.append(ROOT/'ios/Original/NativeCtrdgData.c')
    platform_sources.append(ROOT/'ios/Original/NativeException.c')
    platform_sources.append(ROOT/'ios/Original/NativeOverlayState.c')
    platform_sources.append(ROOT/'ios/Original/NativeOverlayLoader.c')
    platform_sources.append(ROOT/'ios/Original/NativeDSProtect.c')
    platform_sources.append(ROOT/'ios/Original/NativeMemory.c')
    platform_sources.append(ROOT/'ios/Original/NativeDisplayClock.c')
    platform_sources.append(ROOT/'ios/Original/NativeGeometry.c')
    platform_sources.append(ROOT/'ios/Original/Native3D.c')
    platform_sources.append(ROOT/'ios/Original/NativeTexture.c')
    platform_sources.append(ROOT/'ios/Original/Native2D.cpp')
    platform_order = [str(source.relative_to(ROOT).with_suffix('.o')) for source in platform_sources]
    sources += library_sources + platform_sources
    def compile_source(source):
        relative = source.relative_to(ROOT)
        target = destination / 'objects' / relative.with_suffix('.o'); target.parent.mkdir(parents=True,exist_ok=True)
        log = destination / 'logs' / relative.with_suffix('.log'); log.parent.mkdir(parents=True,exist_ok=True)
        cpp = source.suffix == '.cpp'
        owned=str(relative.with_suffix('.o')) in overlay_owners
        options = ['-std=gnu++98' if option == '-std=gnu11' and cpp else option for option in flags]
        if relative==Path('ios/Original/Native2D.cpp'): options+=['-std=gnu++17','-iquote',ROOT/'ios/Original']
        options+=legacy_flags(relative)
        if relative == Path('src/lobby/ppw_lobby.cpp'):
            from prepare_lobby_parts import build
            code,diagnostics = build(destination / 'lobby-parts',SDK,options,target,str(relative) if owned else None)
            log.write_text(diagnostics)
            states=json.loads((destination/'lobby-parts/overlay-state.json').read_text()) if owned and not code else []
        elif owned:
            code,diagnostics,state=compile_unit(SDK/('bin/clang++' if cpp else 'bin/clang'),options,source,target,str(relative),ROOT)
            states=[state]; log.write_text(diagnostics)
        else:
            result = subprocess.run([str(SDK / ('bin/clang++' if cpp else 'bin/clang')), *map(str,options), '-c', str(source), '-o', str(target)],cwd=ROOT,capture_output=True,text=True)
            code,diagnostics,states=result.returncode,result.stdout+result.stderr,[]; log.write_text(diagnostics)
        if code: target.unlink(missing_ok=True)
        return dict(file=str(relative),sha256=hashlib.sha256(source.read_bytes()).hexdigest(),exitCode=code,
                    log=str(log.relative_to(ROOT)),overlayStates=states)
    with ThreadPoolExecutor(max_workers=4) as pool: units=list(pool.map(compile_source,sources))
    if header_hash() != before: raise RuntimeError('Headers changed during ABI32 compilation')
    game_units = [unit for unit in units if unit['file'].startswith('src/')]
    sdk_units = [unit for unit in units if unit['file'].startswith('lib/')]
    platform_units = [unit for unit in units if unit['file'].startswith('ios/')]
    compiled = sum(unit['exitCode']==0 for unit in game_units)
    sdk_compiled = sum(unit['exitCode']==0 for unit in sdk_units)
    platform_compiled = sum(unit['exitCode']==0 for unit in platform_units)
    selection = native_target_selection(units,flags)
    selection['retainedModule'] = dict(checked=False,reason='Final engine link and retained-entry audit have not completed.')
    report = dict(scope='ABI32 intermediate objects only; not full iOS linking or gameplay', target='wasm32-wasip1',
                  headersSha256=before, summary=dict(total=len(game_units),compiled=compiled,blocked=len(game_units)-compiled),
                  librarySummary=dict(total=len(sdk_units),compiled=sdk_compiled,blocked=len(sdk_units)-sdk_compiled),
                  platformSummary=dict(total=len(platform_units),compiled=platform_compiled,blocked=len(platform_units)-platform_compiled),
                  nativeTargetSelection=selection,units=units)
    (destination / 'report.json').write_text(json.dumps(report,indent=2)+'\n')
    definitions, references, definition_sites = set(), {}, {}
    excluded_sources = {unit['file'] for unit in selection['excludedLibraryUnits']}
    objects = [destination / 'objects' / Path(unit['file']).with_suffix('.o') for unit in units if not unit['exitCode'] and unit['file'] not in excluded_sources]
    inventory_objects = objects
    data_failures = 0
    data_selection = None
    overlay_state_failures = 0
    catalog_failures = 0
    aggregate = None
    engine = None
    signature_mismatches = {}
    if compiled == len(game_units):
        order = re.findall(r'^\s*Object\s+(src/[^\s]+\.o)',specification,re.MULTILINE)
        if not order or len(set(order)) != len(order): raise RuntimeError('Original game link order is missing or ambiguous')
        by_name = {str(path.relative_to(destination / 'objects')):path for path in objects}
        if any(name not in by_name for name in order): raise RuntimeError('Original link order requires missing source objects')
        game_selected = [by_name[name] for name in order]
        code_order = list(dict.fromkeys(re.findall(r'^\s*Object\s+((?:src|lib)/[^\s]+\.o)',specification,re.MULTILINE)))
        selected = [by_name[name] for name in code_order if name in by_name]
        selected += [by_name[name] for name in platform_order if name in by_name]
        data_sources = [(ROOT/name).with_suffix('.s') for name in dict.fromkeys(re.findall(r'^\s*Object\s+([^\s]+\.o)',specification,re.MULTILINE))
                        if (ROOT/name).with_suffix('.s').is_file()]
        from build_original_data import build as build_data,native_data_selection
        data = build_data(selected,SDK,flags,destination/'original-data',data_sources)
        data_selection = native_data_selection(data,flags,SDK)
        report['nativeDataSelection'] = data_selection
        data_failures = data['summary']['total']-data['summary']['compiled']
        overlay_state_failures = data['summary']['overlayStateFailures']
        native_by_name = dict(by_name)
        native_by_name.update({str(Path(unit['file']).with_suffix('.o')):Path(unit['object'])
                               for unit in data_selection['selectedObjects']})
        native_order = list(dict.fromkeys(re.findall(r'^\s*Object\s+([^\s]+\.o)',specification,re.MULTILINE)))
        native_order += platform_order
        native_selected = [native_by_name[name] for name in native_order if name in native_by_name]
        state_objects={str(Path(unit['file']).with_suffix('.o')):dict(getters=[state['getter'] for state in unit['overlayStates']])
                       for unit in units if not unit['exitCode'] and unit.get('overlayStates') and all('getter'in state for state in unit['overlayStates'])}
        state_objects.update({str(Path(unit['file']).with_suffix('.o')):dict(getters=[unit['overlayStateGetter']])
                              for unit in data['units'] if not unit['exitCode'] and 'overlayStateGetter'in unit})
        catalog,coverage=catalog_source(overlay_registry,state_objects,NATIVE_OVERLAY_REPLACEMENTS)
        (destination/'overlay-catalog.c').write_text(catalog)
        (destination/'overlay-catalog.json').write_text(json.dumps(coverage,indent=2)+'\n')
        catalog_object=destination/'overlay-catalog.o'
        run([SDK/'bin/clang',*flags,'-c',destination/'overlay-catalog.c','-o',catalog_object],'overlay-catalog.log')
        native_selected.append(catalog_object)
        catalog_failures=sum(not row['ready'] for row in coverage)
        inventory_objects = native_selected
        if header_hash()!=before: raise RuntimeError('Headers changed during native data conversion')
        setup_task_sources = {}
        for source in (ROOT/'src').rglob('wifi_setup*.c'):
            for match in re.finditer(
                r'\bWifiSetupTask_(?:Create(?:Ex)?|SetFunc)\s*\([^,\n]+,\s*(?:\(WifiSetupTaskFunc\)\s*)?(WifiSetup\w+)',
                source.read_text()):
                if match[1]!='WifiSetupTaskFunc':
                    setup_task_sources.setdefault(match[1],set()).add(str(source.relative_to(ROOT).with_suffix('.o')))
        setup_task_names = set(setup_task_sources)
        if len(setup_task_names)<74: raise RuntimeError('Original Wi-Fi setup task registrations are missing')
        # The linker can stop checking a symbol after one conflict; inspect every object independently.
        def signatures(path):
            result = subprocess.run([str(WABT / 'bin/wasm-objdump'),'-x',str(path)],
                                    capture_output=True,text=True,check=True)
            types = {int(index):value for index,value in re.findall(r' - type\[(\d+)\] (.*)',result.stdout)}
            functions = {int(index):int(signature) for index,signature in re.findall(r' - func\[(\d+)\] sig=(\d+)',result.stdout)}
            symbols = re.findall(r' - \d+: F <(.+)> func=(\d+) \[([^\]]+)\]',result.stdout)
            if len(symbols) != len(re.findall(r' - \d+: F ',result.stdout)):
                raise RuntimeError(f'Unrecognized function-symbol metadata: {path}')
            entries = []; task_entries = []
            for name,index,attributes in symbols:
                signature=types[functions[int(index)]]
                if name in setup_task_names and 'undefined' not in attributes:
                    task_entries.append((name,signature,'binding=local' in attributes))
                if 'binding=local' in attributes: continue
                undefined = 'undefined' in attributes
                if undefined and name.startswith('env.'): name = name[4:]
                entries.append((name,signature,'references' if undefined else 'definitions'))
            aliases = {raw:name.removeprefix('env.') for name,raw in re.findall(r' - func\[\d+\] sig=\d+ <(.+)> <- env\.(\S+)',result.stdout)}
            return str(path.relative_to(destination / 'objects')),entries,aliases,task_entries
        function_types = {}
        raw_names = {}
        setup_task_definitions = {}
        with ThreadPoolExecutor(max_workers=4) as pool:
            for path,entries,aliases,tasks in pool.map(signatures,selected):
                raw_names.update(aliases)
                for name,signature,local in tasks:
                    setup_task_definitions.setdefault(name,[]).append(dict(object=path,signature=signature,local=local))
                for name,signature,kind in entries:
                    sites = function_types.setdefault(name,{}).setdefault(signature,dict(definitions=[],references=[]))
                    sites[kind].append(path)
        signature_mismatches = {name:types for name,types in sorted(function_types.items()) if len(types)>1}
        # SDK dispatcher and direct connect/fill-in-user callback contracts.
        chat_contracts = dict(LobbyChat_OnRaw=3,LobbyChat_OnDisconnected=3,LobbyChat_OnPrivateMessage=5,
                              LobbyChat_OnInvited=4,LobbyChat_OnConnected=4,LobbyChat_OnNickError=6,LobbyChat_OnFillInUser=4)
        chat_contracts.update(LobbyChat_OnChannelMessage=6,LobbyChat_OnKicked=5,LobbyChat_OnUserJoined=5,
                              LobbyChat_OnUserParted=7,LobbyChat_OnUserChangedNick=5,LobbyChat_OnTopicChanged=4,
                              LobbyChat_OnChannelModeChanged=4,LobbyChat_OnUserModeChanged=5,LobbyChat_OnUserListUpdated=3,
                              LobbyChat_OnBroadcastKeyChanged=6,LobbyChat_OnEnterChannel=5)
        chat_contracts.update(LobbyChat_OnEnumChannelsEach=7,LobbyChat_OnEnumChannels=7,LobbyChat_OnEnumUsers=7,
                              LobbyChat_OnGetChannelKeys=8,LobbyChat_OnGetChannelMode=5)
        chat_contract_errors = {}
        for name,count in chat_contracts.items():
            expected = '('+', '.join(['i32']*count)+') -> nil'
            defined = {signature for signature,sites in function_types.get(name,{}).items() if sites['definitions']}
            if defined!={expected}: chat_contract_errors[name] = dict(expected=expected,defined=sorted(defined))
        ppw_timer_contracts = {name:'(i32) -> i32' for name in (
            'PPWLobby_TimerUploadUserProfile','PPWLobby_TimerUploadSystemProfile','PPWLobby_TimerSendRecruitInfo',
            'PPWLobby_TimerPollChannelMode','PPWLobby_TimerRequestChannelTime','PPWLobby_TimerRequestLobbyData')}
        ppw_timer_errors = {}
        for name,expected in ppw_timer_contracts.items():
            defined = {signature for signature,sites in function_types.get(name,{}).items() if sites['definitions']}
            if defined!={expected}: ppw_timer_errors[name] = dict(expected=expected,defined=sorted(defined))
        ppw_callback_contracts = dict(PPWLobby_OnPlayerJoined=3,PPWLobby_OnPlayerLeft=3,PPWLobby_OnMessage=4,
            PPWLobby_OnChannelDataChanged=5,PPWLobby_OnTopicChanged=3,PPWLobby_OnPlayerMessage=3,PPWLobby_OnExcessFlood=2,
            PPWLobby_OnEnterMainChannel=4,PPWLobby_OnEnterSubChannel=4,PPWLobby_OnEnumChannels=6,PPWLobby_OnEnumUsers=5,
            PPWLobby_OnGetChannelKeys=6,PPWLobby_OnGetChannelMode=4,PPWLobby_OnGetChannelTime=6,PPWLobby_OnGetLobbyData=6)
        ppw_callback_errors = {}
        for name,count in ppw_callback_contracts.items():
            expected = '('+', '.join(['i32']*count)+') -> nil'
            defined = {signature for signature,sites in function_types.get(name,{}).items() if sites['definitions']}
            if defined!={expected}: ppw_callback_errors[name] = dict(expected=expected,defined=sorted(defined))
        setup_task_contracts = {name:'(i32, i32) -> nil' for name in sorted(setup_task_names)}
        setup_task_errors = {}
        for name,expected in setup_task_contracts.items():
            task_definitions=setup_task_definitions.get(name,[])
            for source in sorted(setup_task_sources[name]):
                # A local callback belongs to its registration TU; another TU cannot supply it.
                local=[row for row in task_definitions if row['local'] and row['object']==source]
                defined={row['signature'] for row in local or [row for row in task_definitions if not row['local']]}
                if defined!={expected}:
                    setup_task_errors.setdefault(name,dict(expected=expected,registrations={}))["registrations"][source]=sorted(defined)
        defined_mismatches = sum(any(sites['definitions'] for sites in types.values()) for types in signature_mismatches.values())
        defined_by_origin = {origin:sum(any(path.startswith(prefix) for sites in types.values() for path in sites['definitions'])
                                       for types in signature_mismatches.values())
                             for origin,prefix in [('game','src/'),('library','lib/'),('platform','ios/')]}
        (destination / 'signature-inventory.json').write_text(json.dumps(dict(
            scope='all nonlocal function definitions/references in selected game/library/platform objects; ABI types only, not runtime validation',
            objects=len(selected),functions=len(function_types),definedMismatchCount=defined_mismatches,
            definedMismatchCountByOrigin=defined_by_origin,
            chatCallbackContracts=chat_contracts,chatCallbackContractErrors=chat_contract_errors,
            ppwTimerContracts=ppw_timer_contracts,ppwTimerContractErrors=ppw_timer_errors,
            ppwCallbackContracts=ppw_callback_contracts,ppwCallbackContractErrors=ppw_callback_errors,
            wifiSetupTaskContracts=setup_task_contracts,wifiSetupTaskContractErrors=setup_task_errors,
            wifiSetupTaskRegistrations={name:sorted(sites) for name,sites in sorted(setup_task_sources.items())},
            wifiSetupTaskDefinitions=setup_task_definitions,
            externalMismatchCount=len(signature_mismatches)-defined_mismatches,mismatches=signature_mismatches,
            rawNames={raw:name for raw,name in raw_names.items() if raw!=name and name in signature_mismatches}),indent=2)+'\n')
        if chat_contract_errors: raise RuntimeError(f'SDK chat callback contract mismatch: {chat_contract_errors}')
        if ppw_timer_errors: raise RuntimeError(f'PPW timer callback contract mismatch: {ppw_timer_errors}')
        if ppw_callback_errors: raise RuntimeError(f'PPW application callback contract mismatch: {ppw_callback_errors}')
        if setup_task_errors: raise RuntimeError(f'Wi-Fi setup task callback contract mismatch: {setup_task_errors}')
        listing = destination / 'game-objects.rsp'
        listing.write_text('\n'.join(json.dumps(str(path)) for path in game_selected)+'\n')
        listing = destination / 'native-objects.rsp'
        listing.write_text('\n'.join(json.dumps(str(path)) for path in native_selected)+'\n')
        target = destination / 'game-all.o'; target.unlink(missing_ok=True)
        result = subprocess.run([str(SDK / 'bin/wasm-ld'),'--relocatable','--fatal-warnings','--error-limit=0','@'+str(listing),'-o',str(target)],
                                cwd=ROOT,capture_output=True,text=True)
        (destination / 'game-link.log').write_text(result.stdout+result.stderr)
        if result.returncode: target.unlink(missing_ok=True)
        aggregate = dict(scope='relocatable game-object merge; SDK/data imports and final iOS linking are not resolved',
                         exitCode=result.returncode,objects=len(native_selected),gameSourceObjects=len(game_selected),librarySourceObjects=sdk_compiled,
                         platformSourceObjects=platform_compiled,
                         dataObjects=data['summary']['objects'],completeDataObjects=data['summary']['compiled'],
                         partialDataObjects=data['summary']['constantObjects'],dataFailures=data_failures,
                         nativeDataSelectionSummary=data_selection['summary'],nativeDataSelectionErrors=data_selection['errors'],
                         overlayStateObjects=data['summary']['overlayStateObjects'],overlayStateFailures=overlay_state_failures,
                         overlayCatalogReady=len(coverage)-catalog_failures,overlayCatalogFailures=catalog_failures,
                         linkOrderSha256=hashlib.sha256(specification.encode()).hexdigest(),
                         sourceObjectsOutsideOriginalLinkOrder=sorted(name for name in set(by_name)-set(order) if name.startswith('src/')),
                         pendingManifestObjects=[name for name in re.findall(r'^\s*Object\s+((?!src/)[^\s]+)',specification,re.MULTILINE) if name not in native_by_name],
                         signatureConflicts=[dict(symbol=name,first=first,second=second) for name,first,second in re.findall(
                             r'function signature mismatch: ([^\n]+)\n>>> ([^\n]+)\n>>> ([^\n]+)',result.stderr)])
        (destination / 'game-link.json').write_text(json.dumps(aggregate,indent=2)+'\n')
        if not result.returncode: engine=link_engine(engine_link,listing,destination,native_content)
        missing = {raw_names.get(item['symbol'],item['symbol']) for item in aggregate['signatureConflicts']} - signature_mismatches.keys()
        if missing: raise RuntimeError(f'Function-signature inventory missed linker conflicts: {sorted(missing)}')
        print(f'Independent signature inventory: {defined_mismatches} functions ({defined_by_origin["game"]} game, {defined_by_origin["library"]} library, {defined_by_origin["platform"]} platform) and {len(signature_mismatches)-defined_mismatches} external names have incompatible ABI types.')
        print(f'Original data objects: {data["summary"]["compiled"]} fully verified, {data["summary"]["constantObjects"]} constants-only, {data_failures} incomplete files.')
        print(f'Original overlay data state: {data["summary"]["overlayStateObjects"]} verified descriptors, {overlay_state_failures} blocked descriptors.')
        print(f'Native overlay catalog: {len(coverage)-catalog_failures}/{len(coverage)} complete, {catalog_failures} unavailable.')
    for first in range(0,len(inventory_objects),64):
        result = subprocess.run([str(SDK / 'bin/llvm-nm'),'--extern-only','--format=posix',*map(str,inventory_objects[first:first+64])],
                                cwd=ROOT,capture_output=True,text=True,check=True)
        current = None
        for line in result.stdout.splitlines():
            if line.endswith(':'):
                current = str(Path(line[:-1]).relative_to(destination)); continue
            fields = line.split()
            if len(fields) < 2: continue
            if fields[1]=='U': references.setdefault(fields[0],set()).add(current)
            else:
                definitions.add(fields[0])
                definition_sites.setdefault(fields[0],set()).add(current)
    alarm_prefix = {'OSi_UseAlarm','OSi_AlarmQueue','OSi_SetTimer','OS_InitAlarm','OS_IsAlarmAvailable','OS_CreateAlarm',
                    'OSi_InsertAlarm','OS_SetAlarm','OS_SetPeriodicAlarm','OS_CancelAlarm'}
    for stem,expected in [('before_handler',alarm_prefix),('after_handler',{'OSi_ArrangeTimer'}),('irq_handler',{'OSi_AlarmHandler'})]:
        path = f'objects/lib/NitroSDK/src/os/os_alarm_{stem}.o'
        actual = {name for name,sites in definition_sites.items() if path in sites}
        if actual != expected: raise RuntimeError(f'Original alarm partition changed: {stem}: {sorted(actual)}')
    unresolved = {name:sorted(path for path in callers if path) for name,callers in sorted(references.items()) if name not in definitions}
    (destination/'link-inventory.json').write_text(json.dumps(dict(
        scope='undefined symbols after selected game/library/platform code and verified original data; SDK imports and final linking remain',
        compiledUnits=compiled,compiledLibraryUnits=sdk_compiled,compiledPlatformUnits=platform_compiled,
        objects=len(inventory_objects),uncompiledUnits=[unit['file'] for unit in units if unit['exitCode']],
        definedSymbols=len(definitions),unresolvedSymbols=unresolved),indent=2)+'\n')
    print(f'ABI32 game object audit: {compiled} compiled, {len(game_units)-compiled} blocked, {len(game_units)} total.')
    print(f'ABI32 library object audit: {sdk_compiled} compiled, {len(sdk_units)-sdk_compiled} blocked, {len(sdk_units)} total.')
    print(f'ABI32 platform object audit: {platform_compiled} compiled, {len(platform_units)-platform_compiled} blocked, {len(platform_units)} total.')
    print(f'Compiled-game symbol inventory: {len(unresolved)} symbols require SDK/data or remaining source resolution.')
    if aggregate is not None: print(f'Relocatable game-object merge exit: {aggregate["exitCode"]}; see {destination / "game-link.log"}')
    if engine is not None: print(f'Final engine intermediate link exit: {engine["exitCode"]}; {len(engine["undefinedSymbols"])} undefined names; see {destination / "engine-link.json"}')
    retained = dict(checked=False,reason='Strict final engine link is unavailable or has undefined symbols.')
    if engine is not None and not engine['exitCode'] and not engine['undefinedSymbols']:
        try: retained = native_retained_entries(destination/'engine.wasm')
        except (ValueError,subprocess.CalledProcessError) as error: retained = dict(checked=False,error=str(error))
        if retained['checked'] and retained['moduleSha256']!=engine['outputSha256']:
            retained = dict(checked=False,error='Retained module differs from the strict engine link output.')
    selection['retainedModule'] = retained
    (destination/'report.json').write_text(json.dumps(report,indent=2)+'\n')
    native_library = selection['selectedCompileSummary']['library']
    print(f'Native selected library audit: {native_library["compiled"]}/{native_library["total"]} compiled; {len(selection["excludedLibraryUnits"])} target-specific exclusions, {len(selection["errors"])} selection errors.')
    if data_selection is not None:
        print(f'Native ordinary data: {data_selection["summary"]["verified"]}/{data_selection["summary"]["total"]} verified; {data_selection["summary"]["excludedMetadataOnly"]} ARM-only metadata inputs excluded; {len(data_selection["errors"])} selection errors.')
    return 1 if compiled != len(game_units) or native_library['blocked'] or selection['errors'] or platform_compiled != len(platform_units) or data_selection is None or data_selection['summary']['blocked'] or data_selection['errors'] or overlay_state_failures or catalog_failures or signature_mismatches or (aggregate is not None and aggregate['exitCode']) or engine is None or engine['exitCode'] or engine['undefinedSymbols'] or not retained['checked'] or retained['forbiddenSymbols'] else 0


def run(args, log, timeout=None):
    result = subprocess.run([str(arg) for arg in args], cwd=ROOT, capture_output=True, text=True,timeout=timeout)
    (BUILD / log).write_text(result.stdout + result.stderr)
    if result.returncode:
        errors = [line for line in result.stderr.splitlines() if any(word in line for word in ['error:','undefined','check line','Layout field'])]
        raise RuntimeError(f'{args[0]} failed; see {BUILD / log}\n' + '\n'.join(errors[:8]))
    return result.stdout


def native_build_settings(destination=BUILD):
    runtime = WABT / 'share/wabt/wasm2c'
    flags = ['-std=c11', '-O2', '-fno-optimize-sibling-calls', '-frounding-math',
             '-DHEARTGOLD_NATIVE', '-DSDK_ARM9', '-DPM_KEEP_ASSERTS', '-iquote', ROOT/'include', '-idirafter', ROOT/'lib/include',
             '-idirafter', ROOT/'ios/build-original-host/include',
             '-DWASM_RT_USE_MMAP=0', '-DWASM_RT_MEMCHECK_GUARD_PAGES=0', '-DWASM_RT_MEMCHECK_BOUNDS_CHECK=1',
             '-I', destination, '-I', ROOT/'ios/Original/abi32', '-I', WABT/'include', '-I', runtime]
    sources = [runtime/name for name in ('wasm-rt-impl.c','wasm-rt-mem-impl.c','wasm-rt-exceptions-impl.c')]
    sources += [ROOT/name for name in (
        'ios/Original/abi32/NativeImports.c','lib/NitroSDK/src/mi/mi_swap.c',
        'ios/Original/abi32/NativeWASIImports.c',
        'ios/Original/NativeLockIDs.c','lib/NitroSDK/src/os/os_cache.c',
        'ios/Original/NativeCPU.c','ios/Original/NativeEvents.c','ios/Original/abi32/NativeIRQImports.c',
        'ios/Original/NativeRTC.c','ios/Original/abi32/NativeRTCImports.c',
        'ios/Original/NativeTick.c','ios/Original/NativeOSMemory.c','ios/Original/abi32/NativeClockImports.c',
        'ios/Original/NativeAlarm.c','ios/Original/NativeDMA.c',
        'ios/Original/NativeGraphicsMemory.c','ios/Original/NativeMemory.c','ios/Original/NativeDisplayClock.c',
        'ios/Original/abi32/NativeDMAImports.c',
        'ios/Original/NativeFile.c','ios/Original/abi32/NativeFileImports.c',
        'ios/Original/NativeMicSampler.c','ios/Original/abi32/NativeMicImports.c',
        'ios/Original/abi32/NativePowerImports.c',
        'ios/Original/abi32/NativeInputImports.c',
        'ios/Original/NativeBackup.c','ios/Original/abi32/NativeCardImports.c',
        'ios/Original/NativeWave.c','ios/Original/NativeMixer.c','ios/Original/NativeInstrument.c','ios/Original/NativeSequence.c','ios/Original/NativeSynth.c',
        'ios/Original/abi32/NativeSoundImports.c',
        'ios/Original/abi32/NativeDwcImports.c',
        'ios/Original/NativeFiber.c','ios/Original/NativeFiberStack.S','ios/Original/abi32/NativeThreadImports.c')]
    return flags,sources


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--audit',action='store_true',help='Also compile all game units for the 32-bit intermediate target')
    args = parser.parse_args()
    if args.audit:
        for name in ('report.json','game-link.json','game-link.log','game-all.o','link-inventory.json','signature-inventory.json','original-data/report.json','native-objects.rsp','overlay-catalog.c','overlay-catalog.json','overlay-catalog.o'):
            (BUILD / 'audit' / name).unlink(missing_ok=True)
    run(['make','-s','-C',ROOT / 'tools/gen_fx_consts'], 'constants-tool.log')
    # The generator derives its include guard from this argument; match the existing build path.
    run([ROOT / 'tools/gen_fx_consts/gen_fx_consts','lib/include/nitro/fx/fx_const.h'], 'constants.log')
    run([sys.executable,ROOT / 'ios/tools/prepare_original_headers.py'], 'headers.log')
    run([sys.executable,ROOT / 'ios/tools/prepare_bignum_checks.py'], 'bignum-goldens.log')
    run([sys.executable,ROOT / 'ios/tools/prepare_float_checks.py'], 'float-goldens.log')
    prepare_tools()
    run([sys.executable,ROOT/'ios/tests/check_lobby_tag_abi.py'],'lobby-tag-abi.log')
    run([sys.executable,ROOT/'ios/tests/check_lobby_views.py'],'lobby-views.log')
    run([sys.executable,ROOT/'ios/tests/check_native_target_selection.py'],'native-target-selection.log')
    run([sys.executable,ROOT/'ios/tests/check_original_data.py'],'data-check.log')
    print(run([sys.executable,ROOT/'ios/tests/check_overlay_state.py'],'overlay-state-parser.log').strip())
    includes = [ROOT / 'ios/build-original-host/include', ROOT / 'include', ROOT / 'files', ROOT / 'src',
                ROOT / 'lib/include', ROOT / 'lib/include/cw', ROOT / 'lib/include/nitro',
                ROOT / 'lib/include/nnsys', ROOT / 'lib/include/nitro/os', ROOT / 'include/library', ROOT / 'lib/NitroSDK/include',
                ROOT / 'lib/NitroSDK/include-pokediamond']
    flags = ['--target=wasm32-wasip1', '-std=gnu11', '-O2', '-fwrapv', '-fno-strict-aliasing',
             '-fno-builtin-malloc','-fno-builtin-free',
             '-fwasm-exceptions','-fexceptions',
             '-Xclang', '-disable-red-zone',
             '-fms-extensions', '-Wno-microsoft', '-Wno-unknown-pragmas', '-ferror-limit=5',
             '-DHEARTGOLD_NATIVE', '-DHEARTGOLD_ABI32', '-DSDK_ARM9', '-DSDK_CW', '-DSDK_CODE_ARM', '-DSDK_TS', '-D_NITRO',
             '-DPM_KEEP_ASSERTS', '-DPLATFORM_INTRINSIC_FUNCTION_BIT_CLZ32=__builtin_clz']
    for include in includes: flags += ['-I', include]
    sources = ['src/math_util.c', 'src/pokemon.c', 'src/wifi_connection.c', 'src/billboard_lists.c', 'src/billboard_gfx_sequence.c', 'src/field_interaction.c', 'src/field/field_hook_unused.c', 'src/spawn_warp.c', 'src/frontier_state.c',
               'lib/NitroSDK/src/fx/fx_mtx22.c','lib/NitroSystem/src/g2d_char_canvas_util.c','ios/Original/abi32/probe.c']
    sources += ['lib/NitroSDK/src/fx/'+name+'.c' for name in ['fx_mtx33_identity','fx_mtx33_rot_yz','fx_mtx43_identity','fx_mtx43_scale',
                                                           'fx_mtx43_rot','fx_mtx44_identity','fx_mtx44_rot']]
    sources += ['src/particle_system.c','lib/src/spl_manager.c','lib/src/spl_emitter.c','lib/src/spl_list.c','lib/src/spl_draw_ptcl.c']
    sources.append('ios/Original/abi32/save_wait_check.c')
    sources.append('ios/Original/tests/area_animation_check.c')
    sources.append('ios/Original/tests/pokepic_motion_check.c')
    sources.append('ios/Original/tests/battle_background_check.c')
    sources.append('ios/Original/tests/battle_task_check.c')
    sources.append('ios/Original/tests/battle_wave_check.c')
    sources.append('ios/Original/tests/battle_emitter_check.c')
    sources.append('ios/Original/tests/battle_ui_check.c')
    sources.append('ios/Original/tests/ball_pocket_check.c')
    sources.append('ios/Original/tests/item_reply_check.c')
    sources += ['lib/NitroSDK/src/mi/mi_memory.c','ios/Original/NativeGraphicsMemory.c','ios/Original/tests/memory_check.c']
    sources += ['lib/NitroSDK/src/mi/mi_swap.c','ios/Original/tests/swap_check.c']
    sources += ['lib/NitroSDK/src/mi/mi_dma.c','ios/Original/abi32/dma_bridge_check.c']
    sources.append('lib/NitroSDK/src/mi/mi_init.c')
    sources += ['lib/NitroSDK/src/fs/FS_file.c','lib/NitroSDK/src/fs/FS_rom.c','lib/NitroSDK/src/fs/FS_command_default.c',
                'lib/NitroSDK/src/fs/FS_archive_suffix.c','lib/NitroSDK/src/fs/FS_archive_prefix.c',
                'src/filesystem.c','ios/Original/abi32/file_bridge_check.c']
    sources += ['ios/Original/NativeOSMemory.c','src/heap.c','ios/Original/NativeFndData.c',
                'lib/NitroSystem/src/fnd_expheap.c','lib/NitroSystem/src/fnd_heapcommon.c',
                'lib/NitroSystem/src/fnd_list.c','lib/NitroSystem/src/fnd_allocator.c',
                'ios/Original/tests/arena_check.c','ios/Original/abi32/arena_heap_check.c']
    sources.append('ios/Original/NativeMemory.c')
    sources += ['lib/MSL_C/src/msl_console_io.c','lib/MSL_C/src/msl_semihosting.c','ios/Original/abi32/console_check.c']
    sources.append('lib/MSL_C/src/msl_alloc.c')
    sources += ['lib/MSL_C/src/'+name+'.c' for name in ('msl_shared_ptr_create','msl_shared_ptr_destroy','msl_shared_ptr_release_weak')]
    sources += ['ios/Original/NativeCtrdgData.c','lib/NitroSDK/src/os/os_tcm.c',
                'lib/NitroSDK/src/ctrdg/ctrdg_module_info.c','ios/Original/abi32/startup_data_check.c']
    sources.append('lib/NitroSDK/src/ctrdg/CTRDG_common.c')
    sources.append('lib/NitroSDK/src/ctrdg/CTRDG_init.c')
    sources += ['lib/NitroSDK/src/pxi/PXI_fifo.c','lib/NitroSDK/src/pxi/PXI_init.c','ios/Original/abi32/pxi_check.c']
    sources += ['lib/NitroSDK/src/os/'+name+'.c' for name in
                ('os_spinLock','os_spin_lock_word','os_spin_lock_card','os_spin_lock_cartridge')]
    sources.append('ios/Original/abi32/lock_check.c')
    sources += ['ios/Original/abi32/boot_info_check.c','ios/Original/abi32/legacy_boot_info_check.c']
    sources += ['lib/NitroSDK/src/pm.c','ios/Original/abi32/power_check.c']
    sources += ['lib/NitroSDK/src/tp.c','ios/Original/abi32/input_check.c','ios/Original/abi32/legacy_input_check.c']
    sources += ['src/system_card.c','ios/Original/abi32/system_card_check.c']
    sources += ['lib/NitroSDK/src/card/'+name+'.c' for name in
                ('CARD_common','CARD_request','CARD_backup','card_identify_backup','card_request_stream_command')]
    sources.append('ios/Original/abi32/card_check.c')
    sources += ['lib/NitroSDK/src/snd/'+name+'.c' for name in
                ('SND_command','SND_main','SND_work','SND_alarm','snd_flush_command','snd_interface','snd_master_volume','snd_read_driver_info')]
    sources.append('ios/Original/abi32/sound_check.c')
    sources.append('ios/Original/tests/render_check.c')
    sources += ['lib/NitroSDK/src/gx/g2.c','lib/NitroSDK/src/gx/gx_bgcnt.c']
    sources += ['lib/NitroSDK/src/snd/SND_bank.c','lib/NitroSDK/src/snd/snd_destroy_bank.c','ios/Original/abi32/bank_check.c']
    sources += ['src/timer3.c','ios/Original/abi32/timer3_check.c']
    sources += ['lib/NitroDWC/src/'+name+'.c' for name in ('dwc_bm','dwc_bm_init','dwc_nvram','dwc_auth_wifi_id')]
    sources.append('ios/Original/abi32/dwc_settings_check.c')
    sources += ['ios/Original/NativeException.c','lib/NitroSDK/src/os/os_exception_init.c','ios/Original/abi32/exception_check.c']
    sources += ['src/application/pokewalker/'+name+'.c' for name in
                ('pokewalker_ir','pokewalker_ir_requests','pokewalker_ir_status')]
    sources += ['lib/NitroSDK/src/os/os_message.c','lib/NitroSDK/src/mi/mi_compress.c',
                'lib/NitroSDK/src/mi/mi_compress_search.c','ios/Original/abi32/pokewalker_state_check.c']
    sources += ['lib/NitroSDK/src/os/os_cache.c','ios/Original/tests/cache_check.c']
    sources.append('lib/NitroSDK/src/os/os_terminate_proc.c')
    sources += ['ios/Original/NativeDisplayClock.c','lib/NitroSDK/src/os/os_init.c','ios/Original/tests/display_clock_check.c']
    sources += ['lib/NitroSDK/src/os/os_system.c','ios/Original/tests/cpu_check.c']
    sources.append('ios/Original/tests/legacy_irq_check.c')
    sources += ['ios/Original/NativeSystemCalls.c','lib/NitroSDK/src/math/MATH_crc.c','ios/Original/tests/system_call_check.c']
    sources += ['lib/NitroSDK/src/snd/SND_util.c',
                'lib/NitroSystem/src/snd_fader.c','ios/Original/tests/linker_binding_check.c']
    sources += ['lib/NitroSDK/src/os/os_interrupt.c','lib/NitroSDK/src/os/os_irqTable.c','lib/NitroSDK/src/os/os_wait_irq.c',
                'ios/Original/abi32/NativeIRQClient.c','ios/Original/abi32/irq_check.c']
    sources.append('lib/NitroSDK/src/os/os_irqHandler.c')
    sources += ['lib/NitroSDK/src/rtc/rtc_api.c','src/gf_rtc.c','ios/Original/abi32/rtc_bridge_check.c']
    sources += ['lib/NitroSDK/src/mic.c','ios/Original/abi32/mic_bridge_check.c']
    sources += ['lib/NitroSDK/src/os/os_tick.c','lib/NitroSDK/src/os/os_timer.c','lib/NitroSDK/src/os/os_entropy.c','ios/Original/abi32/clock_bridge_check.c']
    sources += ['lib/NitroSDK/src/os/os_reset_core.c','lib/NitroSDK/src/os/os_reset_system.c','ios/Original/abi32/reset_check.c']
    sources += ['lib/NitroSDK/src/os/os_alarm_before_handler.c','lib/NitroSDK/src/os/os_alarm_after_handler.c',
                'lib/NitroSDK/src/os/os_alarm_irq_handler.c','ios/Original/abi32/alarm_bridge_check.c']
    sources += ['lib/NitroSDK/src/rtc/'+name+'.c' for name in ('rtc_convert_pre','rtc_convert_post','rtc_convert_date_time_to_second')]
    sources += ['lib/NitroSDK/src/os/os_spin_lock_id.c','ios/Original/abi32/lock_id_check.c']
    sources += ['ios/Original/NativeCompression.c','lib/NitroSDK/src/mi/mi_uncompress.c',
                'lib/NitroSDK/src/gx/gxstate.c','lib/NitroSDK/src/gx/gx_vramcnt.c',
                'src/bg_window.c','ios/Original/tests/compression_check.c']
    sources += ['lib/NitroSDK/src/cp/CP_context.c','ios/Original/tests/math_unit_check.c']
    sources += ['ios/Original/NativeGeometry.c','ios/Original/tests/geometry_check.c',
                'lib/NitroSDK/src/fx/fx_mtx44_concat.c','lib/NitroSDK/src/gx/g3imm.c','lib/NitroSDK/src/gx/g3x.c',
                'lib/NitroSDK/src/gx/gxasm.c','lib/NitroSDK/src/gx/g3x_nop_clear_fifo.c']
    sources += ['lib/NitroSDK/src/gx/g3.c','lib/NitroSDK/src/gx/g3b.c','ios/Original/tests/primitive_check.c']
    sources += ['ios/Original/Native3D.c','ios/Original/tests/raster3d_check.c']
    sources += ['ios/Original/NativeTexture.c','ios/Original/tests/texture_check.c',
                'lib/NitroSDK/src/gx/gx_load3d.c','lib/NitroSDK/src/gx/gx.c']
    sources.append('ios/Original/tests/alpha3d_check.c')
    sources.append('ios/Original/tests/lighting_check.c')
    sources += ['lib/NitroSystem/src/g3d_gecom.c','ios/Original/tests/gecom_check.c']
    sources += ['lib/NitroSystem/src/g3d_util.c','lib/NitroSystem/src/g3d_glb.c']
    sources += ['lib/NitroSystem/src/g3d_sbc.c','ios/Original/abi32/sbc_shape_check.c']
    sources += ['src/application/main_menu/main_menu_util.c','src/application/main_menu/main_menu_shared.c','ios/Original/abi32/main_menu_check.c']
    sources += ['lib/NitroSystem/src/g2d_softsprite.c','ios/Original/tests/softsprite_check.c']
    sources += ['src/pokepic.c','src/palette.c','lib/NitroSystem/src/g2d_load.c',
                'lib/NitroSystem/src/g2d_image.c','lib/NitroSDK/src/gx/gx_load2d.c',
                'ios/Original/abi32/pokepic_check.c']
    sources += ['lib/NitroSDK/src/gx/g3_util.c','lib/NitroSDK/src/fx/fx_mtx33_scale_rot_x.c',
                'lib/NitroSDK/src/fx/fx_mtx43_inverse_concat.c']
    sources += ['lib/NitroSDK/src/os/os_thread.c','lib/NitroSDK/src/os/os_thread_available.c','lib/NitroSDK/src/os/os_context.c',
                'lib/NitroSDK/src/os/os_mutex.c','ios/Original/abi32/thread_bridge_check.c']
    sources += ['lib/NitroSDK/src/fx/'+name+'.c' for name in
                ('fx_cp','fx_vec','fx_vec_normalize','fx_vec_mult_add','fx_mtx44_perspective','fx_mtx44_ortho')]
    sources += ['ios/Original/NativeIntegerRuntime.c','ios/Original/tests/integer_check.c']
    sources += ['ios/Original/NativeFloatRuntime.c','ios/Original/tests/float_check.c']
    sources += ['ios/Original/abi32/format_narrow_check.c','ios/Original/abi32/format_wide_check.c']
    sources += ['lib/MSL_C/src/'+name+'.c' for name in (
        'msl_fp_ansi_num2dec','msl_fp_ansi_num2dec_internal','msl_fp_ansi_rounddec','msl_fp_ansi_two_exp',
        'msl_fp_ansi_timesdec','msl_fp_ansi_str2dec','msl_math','msl_s_frexp','msl_s_ldexp','msl_arm_math',
        'msl_mbstring','msl_mbstring_wcstombs','msl_mbstring_mbstowcs',
        'msl_wstring_wcscpy','msl_wstring_wcschr','msl_wstring_wcslen','msl_wmem')]
    sources.append('src/pokeathlon/pokeathlon_course.c')
    sources.append('lib/NitroDWC/src/transport.c')
    sources += ['lib/NitroWiFi/src/soc.c','lib/NitroWiFi/src/soc_inet.c']
    sources.append('lib/NitroDWC/src/match_join.c')
    sources.append('lib/NitroDWC/src/ap_connect_control.c')
    sources.append('ios/Original/abi32/availability_check.c')
    sources.append('ios/Original/abi32/communication_command_check.c')
    sources.append('src/wifi_club/wifi_club_game_entry.c')
    sources += ['lib/src/nitrocrypto_rc4_init.c','lib/src/nitrocrypto_rc4.c','lib/src/nitrocrypto_rc4_fast.c',
                'lib/libCPS/src/cps_rc4.c','ios/Original/tests/rc4_check.c']
    sources += ['lib/NitroSDK/src/dgt_hash.c','lib/NitroSDK/src/dgt_hash2_arm4_small.c',
                'lib/libCPS/src/cps_sha1.c','ios/Original/NativeDgtData.c','ios/Original/tests/digest_check.c']
    sources += ['lib/NitroSDK/src/math/math_qsort.c','ios/Original/tests/qsort_check.c',
                'ios/Original/abi32/event_log_check.c']
    sources += ['lib/libCPS/src/cps_bignum.c','ios/Original/tests/bignum_check.c']
    sources.append('ios/Original/tests/wcm_check.c')
    sources.append('ios/Original/abi32/lobby_string_check.c')
    sources += ['src/chat/chat_callbacks.c','ios/Original/abi32/chat_callback_check.c']
    # Upstream regrouped the lobby units; use its canonical source set, as the game audit does.
    sources += [str(path.relative_to(ROOT)) for path in sorted((ROOT/'src/lobby').glob('*.c'))]
    sources += ['src/wifi_setup/wifi_setup_crypto.c','src/wifi_setup/wifi_setup_share.c',
                'ios/Original/abi32/wifi_setup_check.c']
    sources += ['src/wifi_plaza/wifi_plaza_lobby_client.c','src/wifi_plaza/wifi_plaza.c',
                'ios/Original/abi32/wifi_plaza_check.c']
    sources += ['src/frontier/battle_hall_data.c','src/pokeathlon/pokeathlon_course_sprites.c',
                'src/pokeathlon/pokeathlon_course_mon_gfx.c','ios/Original/abi32/scene_interface_check.c']
    sources += ['ios/Original/NativeOverlayState.c','ios/Original/abi32/overlay_state_check.c']
    sources += ['ios/Original/NativeOverlayLoader.c','lib/NitroSDK/src/fs/FS_overlay.c','src/poke_overlay.c',
                'ios/Original/abi32/overlay_lifecycle_unit.c','ios/Original/abi32/overlay_lifecycle_middle.c','ios/Original/abi32/overlay_lifecycle_check.c']
    sources += ['ios/Original/NativeDSProtect.c','ios/Original/abi32/dsprotect_check.c']
    from prepare_overlay_state import instrument_ir,getter_name,overlay_objects,NATIVE_OVERLAY_REPLACEMENTS
    overlay_units={'src/gf_rtc.c','ios/Original/abi32/overlay_state_check.c','ios/Original/abi32/overlay_lifecycle_unit.c','ios/Original/abi32/overlay_lifecycle_middle.c','ios/Original/NativeDSProtect.c'}
    overlay_reports=[]
    objects = []
    from prepare_msl_native import legacy_flags
    for source in sources:
        target = BUILD / (Path(source).stem + '.o'); objects.append(target)
        observer_flags = ['-DOS_Sleep=SocketCheck_Sleep'] if source=='lib/NitroWiFi/src/soc_inet.c' else []
        observer_flags+=legacy_flags(source)
        if source=='src/heap.c':
            observer_flags += ['-DCommState_Exists=HeapCheck_CommStateExists',
                               '-DPrintErrorMessageAndReset=HeapCheck_PrintErrorMessageAndReset']
        if source=='ios/Original/tests/linker_binding_check.c':
            observer_flags += ['-DHandleLoadOverlay=BindingCheckHandleLoadOverlay','-DUnloadOverlayByID=BindingCheckUnloadOverlayByID']
        if source in ['lib/MSL_C/src/'+name+'.c' for name in ('msl_shared_ptr_create','msl_shared_ptr_destroy','msl_shared_ptr_release_weak')]:
            observer_flags += ['-D_Znwm=SharedPtrCheckNew','-D_ZdlPv=SharedPtrCheckDelete','-D_ZdaPv=SharedPtrCheckArrayDelete',
                               '-D_02111540=SharedPtrCheckBaseVTable','-D_02111558=SharedPtrCheckVTable']
        if source=='ios/Original/abi32/overlay_state_check.c':
            observer_flags += ['-DHGTestOverlayUnit='+getter_name(source),'-DHGTestRtcUnit='+getter_name('src/gf_rtc.c'),
                               '-DHGTestAsmOverlayUnit='+getter_name('ios/Original/abi32/data_fixture.s'),
                               '-DHGTestPokewalkerOverlayUnit='+getter_name('asm/pokewalker_ir_bss.s')]
        if source in overlay_units:
            intermediate=target.with_suffix('.ll')
            # Instrument before optimization so escaped state remains visible to alias analysis.
            run([SDK/'bin/clang',*flags,*observer_flags,'-Xclang','-disable-llvm-passes','-S','-emit-llvm',ROOT/source,'-o',intermediate],Path(source).stem+'-overlay-ir.log')
            transformed,report=instrument_ir(intermediate.read_text(),source)
            intermediate.write_text(transformed); overlay_reports.append(report)
            run([SDK/'bin/clang',*flags,'-c',intermediate,'-o',target],Path(source).stem+'.log')
        else:
            run([SDK / 'bin/clang', *flags, *observer_flags, '-c', ROOT / source, '-o', target], Path(source).stem + '.log')
    (BUILD/'overlay-state-probe.json').write_text(json.dumps(overlay_reports,indent=2)+'\n')
    from prepare_overlay_state import catalog_source
    fixture_registry=[dict(id=i,name='fixture'+str(i),objects=(['unit.o','middle.o'] if i==0 else ['rtc.o','pokewalker.o'] if i==1 else ['unavailable.o'])) for i in range(129)]
    original_registry=overlay_objects((ROOT/'main.lsf').read_text())
    for original in original_registry:
        if original['name'] in NATIVE_OVERLAY_REPLACEMENTS: fixture_registry[original['id']]=original
    fixture_objects={name:dict(getters=[getter_name(key)]) for name,key in [('unit.o','ios/Original/abi32/overlay_lifecycle_unit.c'),
        ('middle.o','ios/Original/abi32/overlay_lifecycle_middle.c'),('rtc.o','src/gf_rtc.c'),('pokewalker.o','asm/pokewalker_ir_bss.s')]}
    fixture_objects['ios/Original/NativeDSProtect.o']=dict(getters=[getter_name('ios/Original/NativeDSProtect.c')])
    fixture_catalog,_=catalog_source(fixture_registry,fixture_objects,NATIVE_OVERLAY_REPLACEMENTS)
    (BUILD/'overlay-catalog-probe.c').write_text(fixture_catalog)
    run([SDK/'bin/clang',*flags,'-c',BUILD/'overlay-catalog-probe.c','-o',BUILD/'overlay-catalog-probe.o'],'overlay-catalog-probe.log')
    objects.append(BUILD/'overlay-catalog-probe.o')
    # The deque/timer fixtures include these two original units to inspect their private state.
    cpp_sources = [str(path.relative_to(ROOT)) for path in sorted((ROOT/'src/lobby').glob('*.cpp'))
                   if path.name not in ('ppw_lobby.cpp','lobby_event_deque_add_back_capacity.cpp','ppw_lobby_process.cpp')]
    cpp_sources += ['ios/Original/abi32/lobby_deque_check.cpp','ios/Original/abi32/ppw_timer_check.cpp',
                    'src/to91_msl_shared_ptr_char_d1.cpp','lib/MSL_C/src/msl_shared_ptr_release.cpp',
                    'ios/Original/abi32/shared_ptr_check.cpp','ios/Original/Native2D.cpp']
    for source in cpp_sources:
        target = BUILD / (Path(source).stem + '.o'); objects.append(target)
        options = ['-std=gnu++98' if option == '-std=gnu11' else option for option in flags]
        if source=='ios/Original/Native2D.cpp': options+=['-std=gnu++17','-iquote',ROOT/'ios/Original']
        run([SDK / 'bin/clang++',*options,'-c',ROOT / source,'-o',target],Path(source).stem+'.log')
    from build_original_data import prepare_arm,emit,verify,wasm_object
    # Private C constants replaced ASM exports; pin every byte to asm/wifi_setup_data.s at
    # 06e9f0ab4a8f6f3537829c367341e6e2f1484337 (sizes below, SHA256 of original ARM bytes).
    setup_tables = {
        'sWifiSetupClientVersion': (7,'f0ac51f92ccaae6e76d633c9b3be005c58ed4716e5a2354fdffe0e21f8ee6151'),
        'sWifiSetupAesRcon': (40,'e47588a34ad373c5b8486c13d0a9b42dae24acb6ea58dae259a3ef2a68c49106'),
        'sWifiSetupAesTe0': (1024,'783fb0a44be7670eb6ec170cf7f16bb2b6f23f4964b87b60c4eb506c7c6a286d'),
        'sWifiSetupAesTe1': (1024,'15a0e7628178b3fe6546e9b60f35e2b1c4864e020b16376866f38f72c74b45a9'),
        'sWifiSetupAesTe2': (1024,'e8058399671d13082c35b6b3ce7f9e282507edd458ccfdd3bad9f35d90016f62'),
        'sWifiSetupAesTe3': (1024,'09fdc69aa887ac8d36e0c8284c7b1d53e580e4880b72a67ff80d7e38317115d9'),
        'sWifiSetupAesTe4': (1024,'00f9e0a8d0a09aee3e3f87f20f296ac0c5e0ae4cdb655f7daf05446f35971047'),
        'sWifiSetupAesTd0': (1024,'0270bff41cb170c33c20788c368cb1b5a66b0fd0b98d638a827b783537583821'),
        'sWifiSetupAesTd1': (1024,'4cf8dc711cd3210d673d86981355e2609233025ed22e4dc7a1f9b8e75ade35f4'),
        'sWifiSetupAesTd2': (1024,'c40a9a77ee05e10af16da9d1781a2053159ea8570ada03637bb5fe3d076c9f89'),
        'sWifiSetupAesTd3': (1024,'0f4764044bf2e91efefaf3d4f1e81c10a2b7f0f45305489925cfc01198cf45b5'),
        'sWifiSetupAesTd4': (1024,'a9bec6bc6dbfe7e4b631f5f2f839a22ca0740e0221a0cb46511ae92d10b7381d'),
    }
    native_setup = wasm_object((BUILD/'wifi_setup_crypto.o').read_bytes(),True)
    # The packet fixture includes the actual private wifi_setup_packet.c translation unit.
    native_packet = wasm_object((BUILD/'wifi_setup_check.o').read_bytes(),True)
    for name,(size,digest) in setup_tables.items():
        native = native_packet if name=='sWifiSetupClientVersion' else native_setup
        symbols = [s for s in native['symbols'] if s.get('name')==name and 'segment' in s]
        if len(symbols)!=1 or symbols[0]['size']!=size:
            raise RuntimeError(f'Missing, ambiguous or resized native setup constant: {name}')
        symbol = symbols[0]
        actual = native['segments'][symbol['segment']]['bytes'][symbol['offset']:symbol['offset']+size]
        if len(actual)!=size or hashlib.sha256(actual).hexdigest()!=digest:
            raise RuntimeError(f'Native setup constant differs from original ARM data: {name}')
    dgt = prepare_arm(ROOT/'lib/asm/nitro_sdk_data.s',BUILD/'data-check/dgt.arm.o',SDK,flags)
    available = prepare_arm(ROOT/'lib/NitroDWC/asm/dwc_lib_data.s',BUILD/'data-check/available.arm.o',SDK,flags)
    declarations=[]
    for name,want in [('gsiAvailableCheckHostFormat',b'%s.available.gs.nintendowifi.net\0'),
                      ('gsiAvailableCheckResponseMagic',b'\xFE\xFD\x09\0')]:
        symbol=next(s for s in available['symbols'] if s['name']==name and s['section'] in available['sections'])
        content=available['sections'][symbol['section']]['bytes'][symbol['offset']:symbol['offset']+len(want)]
        if content!=want: raise RuntimeError(f'Original availability-query constant changed: {name}')
        declarations.append('char '+name+'[]={'+','.join(map(str,content))+'};')
    source=BUILD/'data-check/available-tables.c'; source.write_text('\n'.join(declarations)+'\n')
    target=BUILD/'data-check/available-tables.o'
    run([SDK/'bin/clang',*flags,'-c',source,'-o',target],'available-tables.log'); objects.append(target)
    native_dgt = wasm_object((BUILD/'NativeDgtData.o').read_bytes(),True)
    for name,size in [('gDgtHash1Padding',4),('gDgtHash1MessageOrder',192),
                      ('gDgtHash1Constants',256),('gDgtHmacHash2Params',28)]:
        original = next(s for s in dgt['symbols'] if s['name']==name and s['section'] in dgt['sections'])
        expected_bytes = dgt['sections'][original['section']]['bytes'][original['offset']:original['offset']+size]
        native = next(s for s in native_dgt['symbols'] if s.get('name')==name)
        actual_bytes = native_dgt['segments'][native['segment']]['bytes'][native['offset']:native['offset']+size]
        if len(expected_bytes)!=size or native['size']!=size or actual_bytes!=expected_bytes:
            raise RuntimeError(f'Native DGT table view differs from original SDK data: {name}')
    # These former ASM constants are also private C data. Original 06e9f0ab bytes came from
    # spawn_warp_data.s (with its original maps.h) and system_card_data.s, respectively.
    for owner,name,size,digest in [
        ('spawn_warp.o','sSpawnPoints',540,'b18077032dc20253f99b7972a9123557544102240b61d3cf9c5fe5ef42ec6dfd'),
        ('system_card.o','sRomArchiveName',4,'dfcf7454187a828f09768591fbc2a4602fa29c53655fe55093291ec362f9e41d')]:
        native = wasm_object((BUILD/owner).read_bytes(),True)
        symbols = [s for s in native['symbols'] if s.get('name')==name and 'segment' in s]
        if len(symbols)!=1 or symbols[0]['size']!=size:
            raise RuntimeError(f'Missing, ambiguous or resized native original constant: {name}')
        symbol = symbols[0]
        actual = native['segments'][symbol['segment']]['bytes'][symbol['offset']:symbol['offset']+size]
        if len(actual)!=size or hashlib.sha256(actual).hexdigest()!=digest:
            raise RuntimeError(f'Native constant differs from original ARM data: {name}')
    objects.append(BUILD/'data-check/fixture.o')
    arm=prepare_arm(ROOT/'lib/NitroDWC/asm/dwc_bm_data.s',BUILD/'data-check/dwc-bm.arm.o',SDK,flags)
    assembly,expected,_=emit(arm,{},set(),{},'dwc_bm_proof')
    source=BUILD/'data-check/dwc-bm.s'; source.write_text(assembly)
    target=BUILD/'data-check/dwc-bm.o'
    run([SDK/'bin/clang','-c',source,'-o',target],'dwc-bm-data.log')
    verify(target,expected); objects.append(target)
    arm=prepare_arm(ROOT/'asm/middleware.s',BUILD/'data-check/middleware.arm.o',SDK,flags)
    assembly,expected,_=emit(arm,{},set(),{},'middleware_proof')
    source=BUILD/'data-check/middleware.s'; source.write_text(assembly)
    target=BUILD/'data-check/middleware.o'
    run([SDK/'bin/clang','-c',source,'-o',target],'middleware-data.log')
    verify(target,expected); objects.append(target)
    arm=prepare_arm(ROOT/'asm/pokewalker_ir_bss.s',BUILD/'data-check/pokewalker-ir.arm.o',SDK,flags)
    assembly,expected,_=emit(arm,{},set(),{},'pokewalker_ir_proof','asm/pokewalker_ir_bss.s')
    source=BUILD/'data-check/pokewalker-ir.s'; source.write_text(assembly)
    target=BUILD/'data-check/pokewalker-ir.o'
    run([SDK/'bin/clang','-c',source,'-o',target],'pokewalker-ir-data.log')
    verify(target,expected); objects.append(target)
    ctype = prepare_arm(ROOT/'lib/MSL_C/asm/MSL_Common_ctype.s',BUILD/'data-check/ctype.arm.o',SDK,flags)
    assembly,expected,_ = emit(ctype,{},set(),{},'ctype_proof')
    source = BUILD/'data-check/ctype.s'; source.write_text(assembly)
    run([SDK/'bin/clang','-c',source,'-o',BUILD/'data-check/ctype.o'],'ctype-data.log')
    verify(BUILD/'data-check/ctype.o',expected)
    objects.append(BUILD/'data-check/ctype.o')
    sine = prepare_arm(ROOT/'lib/NitroSDK/asm/fx_sincos.s',BUILD/'data-check/sincos.arm.o',SDK,flags)
    assembly,expected,_ = emit(sine,{},set(),{},'sincos_proof')
    source = BUILD/'data-check/sincos.s'; source.write_text(assembly)
    target = BUILD/'data-check/sincos.o'
    run([SDK/'bin/clang','-c',source,'-o',target],'sincos-data.log')
    verify(target,expected); objects.append(target)
    # Use the same constant partitioner as the full audit; exception support remains incomplete.
    from build_original_data import function_registry
    functions,variables=function_registry(objects)
    format_data=[]
    for name in ('msl_printf_data','msl_wprintf_data','msl_fp_ansi_data','msl_locale_data','MSL_Common_wctype'):
        original=ROOT/'lib/MSL_C/asm'/(name+'.s')
        variables.update(re.findall(r'^\s*([\w.$?]+):',original.read_text(),re.M))
        format_data.append((name,original))
    for name,source in format_data:
        excluded=[]
        arm=prepare_arm(source,BUILD/'data-check'/(name+'.arm.o'),SDK,flags,excluded)
        assembly,expected,_=emit(arm,functions,variables,{},'format_'+name)
        native=BUILD/'data-check'/(name+'.s'); native.write_text(assembly)
        target=BUILD/'data-check'/(name+'.o')
        run([SDK/'bin/clang','-c',native,'-o',target],name+'-data.log')
        verify(target,expected); objects.append(target)
    # Use original SDK table bytes for the isolated character-index checks, without unrelated renderers.
    canvas = prepare_arm(ROOT/'lib/asm/nnsys_data.s',BUILD/'data-check/canvas.arm.o',SDK,flags)
    declarations = []
    for name,size,ctype,shape in [('gCharShiftTable',32,'unsigned char','[4][4][2]'),('gObjShapeSizeAttrs',64,'unsigned int','[4][4]')]:
        symbol = next(s for s in canvas['symbols'] if s['name']==name and s['section'] in canvas['sections'])
        content = canvas['sections'][symbol['section']]['bytes'][symbol['offset']:symbol['offset']+size]
        if len(content)!=size: raise RuntimeError(f'Truncated original SDK table: {name}')
        values = list(content) if size==32 else [int.from_bytes(content[i:i+4],'little') for i in range(0,size,4)]
        declarations.append(f'const {ctype} {name}{shape}={{'+','.join(map(str,values))+'};')
    source = BUILD/'data-check/canvas-tables.c'; source.write_text('\n'.join(declarations)+'\n')
    run([SDK/'bin/clang',*flags,'-c',source,'-o',BUILD/'data-check/canvas-tables.o'],'canvas-tables.log')
    objects.append(BUILD/'data-check/canvas-tables.o')
    # Keep only the original lobby storage used here; the other lobby data have fixture owners.
    # ARM labels are unsized, so check each span through its next original label, including padding.
    for relative,spans in [
        ('lib/NitroDWC/asm/dwc_lobby_data.s', [('gPPWLobbyWork',4)]),
        ('lib/NitroDWC/asm/dwc_lobby_chat_data.s', [('gLobbyStr_DataNeNULL',16),('gLobbyStr_ChatCallbacksC',16),
            ('gChatStr_ciFreeCallbackData',20),('gChatStr_DataTypeGe0',16),
            ('gChatStr_DataTypeLtCALLBACK_NUM',28),('gChatStr_DataCallbackNeNULL',24),
            ('gChatStr_DataCallbackParamsNeNULL',32),('gChatStr_DataID_Ge0',16),
            ('gChatStr_0_8',4),('gChatStr_ChatNeNULL',16),('gChatStr_ciCallCallback',16),
            ('gLobbyStr_TreeInsertLengthError',28)])]:
        original=ROOT/relative; owner=original.stem
        arm=prepare_arm(original,BUILD/'data-check'/(owner+'.arm.o'),SDK,flags)
        assembly=[]; expected={}
        for name,size in spans:
            matches=[s for s in arm['symbols'] if s['name']==name and s['section'] in arm['sections']]
            if len(matches)!=1: raise RuntimeError(f'Missing or ambiguous original lobby data: {name}')
            symbol=matches[0]; section=arm['sections'][symbol['section']]; begin=symbol['offset']
            end=min([len(section['bytes']),*[s['offset'] for s in arm['symbols']
                                            if s['section']==symbol['section'] and s['offset']>begin]])
            if end-begin!=size or any(begin<=r['offset']<end for r in section['relocations']):
                raise RuntimeError(f'Original lobby data span or relocations changed: {name}')
            selected=dict(sections={symbol['section']:dict(section,bytes=section['bytes'][begin:end],relocations=[])},
                          symbols=[dict(symbol,offset=0,size=size)])
            part,want,_=emit(selected,{},set(),{},'lobby_'+name)
            assembly.append(part); expected.update(want)
        source=BUILD/'data-check'/(owner+'.s'); source.write_text('\n'.join(assembly))
        target=BUILD/'data-check'/(owner+'.o')
        run([SDK/'bin/clang','-c',source,'-o',target],owner+'-data.log')
        verify(target,expected); objects.append(target)
    from prepare_lobby_parts import build
    lobby = BUILD / 'lobby.o'
    code,diagnostics = build(BUILD / 'probe-lobby-parts',SDK,
                            ['-std=gnu++98' if option == '-std=gnu11' else option for option in flags],lobby)
    (BUILD / 'probe-lobby.log').write_text(diagnostics)
    if code: raise RuntimeError(f'Lobby native preparation/link failed; see {BUILD / "probe-lobby.log"}')
    objects.append(lobby.with_suffix('.a'))
    exports = ['HGAotLayout','HGAotPointerCheck','HGAotString','HGAotIterator','HGAotData','HGAotEncrypt','HGAotDecrypt','HGAotChecksum','HGAotLobbyFind','HGAotDequeCopy','HGAotWifiStatus','HGAotBillboardFrames','HGAotBottomScreenCallbacks','HGAotSequenceIndex','HGAotMovementAllowed',
               'HGAotSeed','HGAotRandom','HGAotMTSeed','HGAotMTRandom','HGAotAsmPointers','HGAotAsmRandom','HGAotSpawnWarp','HGAotFactoryFlags','HGAotSDKMath','HGAotSDKCharIndex','HGAotSDKMatrix','HGAotParticleCreation','CheckNativeMemory','CheckOriginalRC4','CheckOriginalDigests','CheckOriginalQSort','CheckOriginalLobbyStrings','CheckOriginalLobbyComparison','CheckOriginalLobbyCleanup','HGAotEventLogSort','HGAotCompressedResource','HGAotCourseStateDispatch','HGAotTransportCallbacks','HGAotSocketAdapters','HGAotMatchErrorRouting','HGAotSendBufferSpace','HGAotCommunicationModes','HGAotCommandDispatch','HGAotClubDisconnect','HGAotLobbyStringSearch']
    exports.append('CheckOriginalChatDispatch')
    exports.append('CheckOriginalLobbyRequestMap')
    exports.append('CheckOriginalLobbyMapErase')
    exports.append('CheckOriginalLobbyAllocators')
    exports.append('CheckOriginalLobbyGrowth')
    exports += ['CheckOriginalLobbyUserRetention','CheckOriginalLobbySchedule']
    exports.append('CheckOriginalPPWTimers')
    exports.append('CheckOriginalLobbyEventForwarding')
    exports.append('CheckOriginalWifiSetup')
    exports.append('CheckOriginalWifiFiles')
    exports.append('CheckOriginalWifiPlaza')
    exports.append('CheckOriginalSceneInterfaces')
    exports.append('CheckOriginalBignum')
    exports.append('CheckOriginalWcm')
    exports += ['CheckOriginalSwapWord','HGSwapWordExchange','HGSwapWordGuards']
    exports += ['CheckOriginalLockIDs','HGGetLockID','HGReleaseLockID']
    exports += ['CheckOriginalCache','HGCacheOperation']
    exports += ['CheckOriginalCPU','HGCPUAction','HGCPUIncrement','HGCPUCounter']
    exports += ['CheckOriginalIRQ','HGIRQReference','HGIRQAction','HGIRQRegister','HGIRQGet','HGIRQRead','HGIRQArm']
    exports += ['HGIRQInit','HGIRQMathState']
    exports.append('HGIRQHalt')
    exports.append('HGIRQDispatch')
    exports.append('HGIRQResetStack')
    exports.append('HGAotNativeAssert')
    exports += ['HGConsoleReference','HGConsoleAction']
    exports.append('CheckOriginalSharedPtr')
    exports.append('CheckOriginalThreads')
    exports.append('CheckOriginalGeometry')
    exports.append('CheckOriginalDisplayClock')
    exports.append('CheckOriginalPrimitives')
    exports += ['HGSaveWaitReset','HGSaveWaitWritten','HGSaveWaitPoll','HGSaveWaitCallback']
    exports.append('CheckOriginalRaster3D')
    exports.append('CheckOriginalTextures')
    exports.append('CheckOriginalAlpha3D')
    exports.append('CheckOriginalLighting')
    exports.append('CheckOriginalGecom')
    exports.append('CheckOriginalSoftsprite')
    exports.append('CheckOriginalPokepic')
    exports += ['CheckRTCBeforeIRQ','CheckOriginalGameRTC','HGRTCAddress','HGRTCRead','HGRTCSync','HGRTCStart','HGRTCGameFrames']
    exports += ['HGMicAddress','HGMicAction','HGMicRead']
    exports += ['CheckOriginalClock','HGTickRead','HGTickSet','HGTickInit','HGEntropyAddress']
    exports += ['CheckOriginalAlarmQueue','HGAlarmStart','HGAlarmRead','HGAlarmCancelAll']
    exports.append('CheckOriginalIntegerRuntime')
    exports.append('CheckOriginalFloatRuntime')
    exports.append('CheckOriginalSystemCalls')
    exports.append('CheckOriginalLinkerBindings')
    exports.append('CheckOriginalLobbyProfile')
    exports.append('CheckOriginalOwnedDeque')
    exports += ['CheckOriginalBridgedDMA','HGDMAReference']
    exports += ['CheckLegacyIRQState','HGLegacyIRQAction']
    exports += ['CheckOriginalBridgedFiles','HGFileReference','HGFileOpenHeld','HGFileReadSample','CheckOriginalUncompress','CheckOriginalMathUnit']
    exports += ['InitializeOriginalGameHeap','CheckOriginalGameHeaps','HGArenaReference','HGArenaTake']
    exports += ['CheckOriginalStartupData','HGStartupData']
    exports.append('CheckNativeCartridgeEnable')
    exports += ['CheckNativeCartridgeInit','HGCartridgeLockID']
    exports += ['CheckNativePXI','HGSystemWorkAddress','HGLegacySystemWorkAddress']
    exports.append('CheckOriginalLockLifecycle')
    exports.append('CheckNativeBootInfo')
    exports += ['CheckNativePower','HGPowerAction','HGPowerReference']
    exports += ['CheckNativeTouch','HGInputAction','HGInputReference','HGLegacyPad']
    exports += ['CheckNativeRomArchive','HGPrepareRomArchive','SystemCard_InitRomArchive']
    exports += ['HGCardSetup','HGCardAction','HGCardReference','HGCardBuffer','HGCardMode']
    exports += ['CheckNativeSoundQueue','HGSoundAction','HGSoundReference','HGSoundRawCommand']
    exports += ['CheckNative2D','HGNativeFrameBuffer']
    exports.append('CheckNativeShapeCallback')
    exports.append('CheckNativeNodeMix')
    exports.append('CheckNativeMainMenuWork')
    exports.append('CheckOriginalAreaAnimation')
    exports.append('CheckOriginalPokepicMotion')
    exports.append('CheckOriginalBattleBackground')
    exports.append('CheckOriginalBattleTasks')
    exports.append('CheckOriginalBattleWave')
    exports.append('CheckOriginalBattleEmitter')
    exports.append('CheckOriginalBattleUI')
    exports.append('CheckOriginalBallPocket')
    exports.append('CheckOriginalItemReply')
    exports.append('HGSoundMusicAction')
    exports.append('CheckNativeSoundBank')
    exports += ['CheckNativeGameTimer','HGTimer3Action']
    exports += ['CheckNativeDwcSettings','HGDwcIdentity','HGDwcInitialize']
    exports += ['CheckOriginalExceptionInit','HGExceptionAddress','OSi_ExceptionHandler']
    exports.append('CheckNativeOverlayState')
    exports.append('CheckNativeOverlayLifecycle')
    exports.append('CheckNativeDSProtect')
    exports += ['HGResetStateAddress','HGResetBootInfo','HGResetScenario','HGResetReadFile']
    exports.append('CheckOriginalPokewalkerState')
    exports += ['CheckOriginalNarrowFormat','CheckOriginalWideFormat']
    exports.append('CheckOriginalAvailability')
    # These exports share a persistent game instance; command-mode wrappers run libc destructors after every call.
    native_imports = BUILD/'native-imports.txt'
    native_imports.write_text('HGNativeAtomicSwapWord\nHGNativeGetLockID\nHGNativeReleaseLockID\nHGNativeMemoryBarrier\n'
                             'HGNativeEnableInterrupts\nHGNativeDisableInterrupts\nHGNativeRestoreInterrupts\n'
                             'HGNativeDisableInterrupts_IrqAndFiq\nHGNativeRestoreInterrupts_IrqAndFiq\n'
                             'HGNativeGetCpsrIrq\nHGNativeGetProcMode\nHGNativeSpinWait\n'
                             'HGNativeIrqInit\nHGNativeIrqControl\nHGNativeIrqSet\nHGNativeIrqGet\nHGNativeRtcRequest\n'
                             'HGNativeTickControl\nHGNativeEntropy\nHGNativeAlarmTimer\nHGNativeDmaRequest\nHGNativeFileRequest\nHGNativeThreadRequest\nHGNativeMicRequest\nHGNativePowerRequest\nHGNativeInputRequest\nHGNativeCardRequest\nHGNativeSoundRequest\nHGNativeDwcSettings\nHGNativeDwcIdentity\nHGNativeResetControl\n')
    link = [SDK / 'bin/clang', '--target=wasm32-wasip1', '-mexec-model=reactor', '-Wl,--export-memory',
            '-Wl,--wrap=malloc','-Wl,--wrap=free',
            '-Wl,--allow-undefined-file='+str(native_imports),
            '-Wl,--gc-sections', '-Wl,--fatal-warnings','-Wl,--error-limit=0','-Wl,--stack-first','-Wl,-z,stack-size=65536',
            '-Wl,--initial-memory=16777216', '-Wl,--max-memory=16777216']
    link+=['-fwasm-exceptions','-lc++','-lc++abi','-lunwind']
    link_command = [*link, *['-Wl,--export=' + name for name in exports], *objects, '-o', BUILD / 'hg32.wasm']
    (BUILD/'probe-link-command.json').write_text(json.dumps(list(map(str,link_command)),indent=2)+'\n')
    run(link_command, 'link.log')
    run([WABT / 'bin/wasm2c', BUILD / 'hg32.wasm', '-n', 'hg32', '-o', BUILD / 'hg32.c'], 'wasm2c.log')
    generated = (BUILD / 'hg32.c').read_text()
    if generated.count('#include "hg32.h"') != 1:
        raise RuntimeError('Pinned generated C header layout changed')
    generated = generated.replace('#include "hg32.h"', '#include "hg32.h"\n#include "NativeImports.h"')
    from prepare_exception_unwind import restore_exception_depth
    generated,_=restore_exception_depth(generated)
    expected_stack_writes = len(re.findall(r'w2c_0x5F_stack_pointer\s*=', generated))
    generated, stack_writes = re.subn(r'(\w+)->w2c_0x5F_stack_pointer = ([^;\n]+);',
        r'\1->w2c_0x5F_stack_pointer = HGNativeCheckedStackPointer(\1->w2c_env_instance, \2);', generated)
    if not stack_writes or stack_writes != expected_stack_writes:
        raise RuntimeError('Pinned generated C stack-pointer layout changed')
    (BUILD / 'hg32.c').write_text(generated)
    native,native_sources = native_build_settings()
    native_sources = [BUILD/'hg32.c',*native_sources]
    run(['xcrun', 'clang', *native, '-fsanitize=address,undefined', '-fno-sanitize-recover=all',
         ROOT / 'ios/Original/abi32/check.c', *native_sources, '-o', BUILD / 'abi32-check'], 'native.log')
    print(run([BUILD / 'abi32-check'], 'check.log').strip())
    print(run([BUILD/'abi32-check','--save-wait'],'save-wait-check.log',timeout=15).strip())
    print(run([BUILD/'abi32-check','--native-reset'],'reset-check.log').strip())
    print(run([BUILD/'abi32-check','--power-before-irq'],'power-before-irq.log').strip())
    print(run([BUILD/'abi32-check','--input-before-irq'],'input-before-irq.log').strip())
    print(run([BUILD/'abi32-check','--native-card'],'card-check.log',timeout=45).strip())
    print(run([BUILD/'abi32-check','--display-clock'],'display-clock-check.log',timeout=15).strip())
    print(run([BUILD/'abi32-check','--raster'],'raster-check.log',timeout=15).strip())
    assertion=subprocess.run([str(BUILD/'abi32-check'),'--native-assert'],cwd=ROOT,capture_output=True,text=True)
    (BUILD/'native-assert.log').write_text(assertion.stdout+assertion.stderr)
    if assertion.returncode!=-signal.SIGABRT or 'original game assertion:' not in assertion.stderr or 'native assertion self-check' not in assertion.stderr:
        raise RuntimeError('Native assertion did not report its condition and stop; see native-assert.log')
    exception=subprocess.run([str(BUILD/'abi32-check'),'--native-exception'],cwd=ROOT,capture_output=True,text=True)
    (BUILD/'native-exception.log').write_text(exception.stdout+exception.stderr)
    if exception.returncode!=-signal.SIGABRT or 'original game assertion:' not in exception.stderr or 'Original DS exception vector reached native code' not in exception.stderr:
        raise RuntimeError('Native exception-vector entry did not diagnose and stop; see native-exception.log')
    console=subprocess.run([str(BUILD/'abi32-check'),'--console-exit'],cwd=ROOT,capture_output=True,text=True)
    (BUILD/'console-exit.log').write_text(console.stdout+console.stderr)
    if console.returncode!=-signal.SIGABRT or 'Original runtime requested termination.' not in console.stdout:
        raise RuntimeError('Native debugger-console exit did not stop; see console-exit.log')
    overflow = subprocess.run([str(BUILD/'abi32-check'),'--irq-overflow'],cwd=ROOT,capture_output=True,text=True)
    (BUILD/'irq-overflow.log').write_text(overflow.stdout+overflow.stderr)
    if overflow.returncode != -signal.SIGABRT or 'Original IRQ callback trapped: Call stack exhausted' not in overflow.stderr:
        raise RuntimeError('IRQ stack overflow did not fail in the checked prologue; see irq-overflow.log')
    reset=subprocess.run([str(BUILD/'abi32-check'),'--irq-stack-reset'],cwd=ROOT,capture_output=True,text=True)
    (BUILD/'irq-stack-reset.log').write_text(reset.stdout+reset.stderr)
    if reset.returncode!=-signal.SIGABRT or 'OS_GetProcMode()==OS_PROCMODE_SYS' not in reset.stderr or 'Original IRQ callback trapped:' not in reset.stderr:
        raise RuntimeError('Active IRQ stack-checker reset was not rejected; see irq-stack-reset.log')
    run(['xcrun','clang',*native,'-fsanitize=thread',ROOT/'ios/Original/abi32/check.c',
         *native_sources,'-o',BUILD/'abi32-thread-check'],'thread-native.log')
    print(run([BUILD/'abi32-thread-check'],'thread-check.log').strip())
    print(run([BUILD/'abi32-thread-check','--native-reset'],'reset-thread-check.log').strip())
    print(run([BUILD/'abi32-thread-check','--power-before-irq'],'power-before-irq-thread.log').strip())
    print(run([BUILD/'abi32-thread-check','--input-before-irq'],'input-before-irq-thread.log').strip())
    print(run([BUILD/'abi32-thread-check','--native-card'],'card-thread-check.log',timeout=45).strip())
    print(run([BUILD/'abi32-thread-check','--display-clock'],'display-clock-thread-check.log',timeout=15).strip())
    print(run([BUILD/'abi32-thread-check','--save-wait'],'save-wait-thread-check.log',timeout=15).strip())
    device = BUILD / 'iphoneos'; device.mkdir(exist_ok=True)
    device_objects = []
    for source in native_sources:
        target = device / (source.stem + '.o'); device_objects.append(target)
        run(['xcrun', '--sdk', 'iphoneos', 'clang', '-target', 'arm64-apple-ios16.0', *native,
             '-c', source, '-o', target], source.stem + '-iphoneos.log')
    (device / 'libHeartGoldABI32Probe.a').unlink(missing_ok=True)
    run(['xcrun', 'ar', 'rcs', device / 'libHeartGoldABI32Probe.a', *device_objects], 'archive.log')
    print(f'iPhoneOS native C archive: {device / "libHeartGoldABI32Probe.a"}')
    print(run([sys.executable,ROOT/'ios/tests/check_cpp_exception_runtime.py'],'cpp-exception-check.log').strip())
    if args.audit: return audit(flags, includes, link)


if __name__ == '__main__':
    raise SystemExit(main())
