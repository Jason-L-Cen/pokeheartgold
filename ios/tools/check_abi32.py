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


def audit(flags, includes):
    destination = BUILD / 'audit'; destination.mkdir(exist_ok=True)
    headers = sorted({p for root in includes for pattern in ('**/*.h','**/*.naix') for p in root.glob(pattern)})
    def header_hash():
        value = hashlib.sha256()
        for path in headers: value.update(str(path.relative_to(ROOT)).encode()); value.update(path.read_bytes())
        return value.hexdigest()
    before = header_hash()
    specification = (ROOT/'main.lsf').read_text()
    sources = sorted([*ROOT.glob('src/**/*.c'), *ROOT.glob('src/**/*.cpp')])
    library_objects = list(dict.fromkeys(re.findall(r'^\s*Object\s+(lib/[^\s]+\.o)',specification,re.MULTILINE)))
    library_sources = []
    for name in library_objects:
        source = next(((ROOT/name).with_suffix(suffix) for suffix in ('.c','.cpp') if (ROOT/name).with_suffix(suffix).is_file()),None)
        if source is not None: library_sources.append(source)
    platform_sources = [ROOT/'ios/Original/NativeGraphicsMemory.c',ROOT/'ios/Original/NativeCompression.c']
    platform_order = [str(source.relative_to(ROOT).with_suffix('.o')) for source in platform_sources]
    sources += library_sources + platform_sources
    def compile_source(source):
        relative = source.relative_to(ROOT)
        target = destination / 'objects' / relative.with_suffix('.o'); target.parent.mkdir(parents=True,exist_ok=True)
        log = destination / 'logs' / relative.with_suffix('.log'); log.parent.mkdir(parents=True,exist_ok=True)
        cpp = source.suffix == '.cpp'
        options = ['-std=gnu++98' if option == '-std=gnu11' and cpp else option for option in flags]
        if relative == Path('src/ov45_02232e60.cpp'):
            from prepare_lobby_parts import build
            code,diagnostics = build(destination / 'lobby-parts',SDK,options,target)
            log.write_text(diagnostics)
            return dict(file=str(relative),sha256=hashlib.sha256(source.read_bytes()).hexdigest(),exitCode=code,
                        log=str(log.relative_to(ROOT)),partitioned=True)
        result = subprocess.run([str(SDK / ('bin/clang++' if cpp else 'bin/clang')), *map(str,options), '-c', str(source), '-o', str(target)],
                                cwd=ROOT,capture_output=True,text=True)
        log.write_text(result.stdout+result.stderr)
        if result.returncode: target.unlink(missing_ok=True)
        return dict(file=str(relative),sha256=hashlib.sha256(source.read_bytes()).hexdigest(),exitCode=result.returncode,
                    log=str(log.relative_to(ROOT)))
    with ThreadPoolExecutor(max_workers=4) as pool: units=list(pool.map(compile_source,sources))
    if header_hash() != before: raise RuntimeError('Headers changed during ABI32 compilation')
    game_units = [unit for unit in units if unit['file'].startswith('src/')]
    sdk_units = [unit for unit in units if unit['file'].startswith('lib/')]
    platform_units = [unit for unit in units if unit['file'].startswith('ios/')]
    compiled = sum(unit['exitCode']==0 for unit in game_units)
    sdk_compiled = sum(unit['exitCode']==0 for unit in sdk_units)
    platform_compiled = sum(unit['exitCode']==0 for unit in platform_units)
    report = dict(scope='ABI32 intermediate objects only; not full iOS linking or gameplay', target='wasm32-wasip1',
                  headersSha256=before, summary=dict(total=len(game_units),compiled=compiled,blocked=len(game_units)-compiled),
                  librarySummary=dict(total=len(sdk_units),compiled=sdk_compiled,blocked=len(sdk_units)-sdk_compiled),
                  platformSummary=dict(total=len(platform_units),compiled=platform_compiled,blocked=len(platform_units)-platform_compiled),units=units)
    (destination / 'report.json').write_text(json.dumps(report,indent=2)+'\n')
    definitions, references, definition_sites = set(), {}, {}
    objects = [destination / 'objects' / Path(unit['file']).with_suffix('.o') for unit in units if not unit['exitCode']]
    inventory_objects = objects
    data_failures = 0
    aggregate = None
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
        from build_original_data import build as build_data
        data = build_data(selected,SDK,flags,destination/'original-data',data_sources)
        data_failures = data['summary']['total']-data['summary']['compiled']
        native_by_name = dict(by_name)
        native_by_name.update({str(Path(unit['file']).with_suffix('.o')):Path(unit['object']) for unit in data['units'] if not unit['exitCode']})
        native_order = list(dict.fromkeys(re.findall(r'^\s*Object\s+([^\s]+\.o)',specification,re.MULTILINE)))
        native_order += platform_order
        native_selected = [native_by_name[name] for name in native_order if name in native_by_name]
        inventory_objects = native_selected
        if header_hash()!=before: raise RuntimeError('Headers changed during native data conversion')
        # The linker can stop checking a symbol after one conflict; inspect every object independently.
        def signatures(path):
            result = subprocess.run([str(WABT / 'bin/wasm-objdump'),'-x',str(path)],
                                    capture_output=True,text=True,check=True)
            types = {int(index):value for index,value in re.findall(r' - type\[(\d+)\] (.*)',result.stdout)}
            functions = {int(index):int(signature) for index,signature in re.findall(r' - func\[(\d+)\] sig=(\d+)',result.stdout)}
            symbols = re.findall(r' - \d+: F <(.+)> func=(\d+) \[([^\]]+)\]',result.stdout)
            if len(symbols) != len(re.findall(r' - \d+: F ',result.stdout)):
                raise RuntimeError(f'Unrecognized function-symbol metadata: {path}')
            entries = []
            for name,index,attributes in symbols:
                if 'binding=local' in attributes: continue
                undefined = 'undefined' in attributes
                if undefined and name.startswith('env.'): name = name[4:]
                entries.append((name,types[functions[int(index)]],'references' if undefined else 'definitions'))
            aliases = {raw:name.removeprefix('env.') for name,raw in re.findall(r' - func\[\d+\] sig=\d+ <(.+)> <- env\.(\S+)',result.stdout)}
            return str(path.relative_to(destination / 'objects')),entries,aliases
        function_types = {}
        raw_names = {}
        with ThreadPoolExecutor(max_workers=4) as pool:
            for path,entries,aliases in pool.map(signatures,selected):
                raw_names.update(aliases)
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
        defined_mismatches = sum(any(sites['definitions'] for sites in types.values()) for types in signature_mismatches.values())
        defined_by_origin = {origin:sum(any(path.startswith(prefix) for sites in types.values() for path in sites['definitions'])
                                       for types in signature_mismatches.values())
                             for origin,prefix in [('game','src/'),('library','lib/'),('platform','ios/')]}
        (destination / 'signature-inventory.json').write_text(json.dumps(dict(
            scope='all nonlocal function definitions/references in selected game/library/platform objects; ABI types only, not runtime validation',
            objects=len(selected),functions=len(function_types),definedMismatchCount=defined_mismatches,
            definedMismatchCountByOrigin=defined_by_origin,
            chatCallbackContracts=chat_contracts,chatCallbackContractErrors=chat_contract_errors,
            externalMismatchCount=len(signature_mismatches)-defined_mismatches,mismatches=signature_mismatches,
            rawNames={raw:name for raw,name in raw_names.items() if raw!=name and name in signature_mismatches}),indent=2)+'\n')
        if chat_contract_errors: raise RuntimeError(f'SDK chat callback contract mismatch: {chat_contract_errors}')
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
                         dataObjects=data['summary']['compiled'],dataFailures=data_failures,linkOrderSha256=hashlib.sha256(specification.encode()).hexdigest(),
                         sourceObjectsOutsideOriginalLinkOrder=sorted(name for name in set(by_name)-set(order) if name.startswith('src/')),
                         pendingManifestObjects=[name for name in re.findall(r'^\s*Object\s+((?!src/)[^\s]+)',specification,re.MULTILINE) if name not in native_by_name],
                         signatureConflicts=[dict(symbol=name,first=first,second=second) for name,first,second in re.findall(
                             r'function signature mismatch: ([^\n]+)\n>>> ([^\n]+)\n>>> ([^\n]+)',result.stderr)])
        (destination / 'game-link.json').write_text(json.dumps(aggregate,indent=2)+'\n')
        missing = {raw_names.get(item['symbol'],item['symbol']) for item in aggregate['signatureConflicts']} - signature_mismatches.keys()
        if missing: raise RuntimeError(f'Function-signature inventory missed linker conflicts: {sorted(missing)}')
        print(f'Independent signature inventory: {defined_mismatches} functions ({defined_by_origin["game"]} game, {defined_by_origin["library"]} library, {defined_by_origin["platform"]} platform) and {len(signature_mismatches)-defined_mismatches} external names have incompatible ABI types.')
        print(f'Original data objects: {data["summary"]["compiled"]} verified, {data_failures} blocked.')
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
    return 1 if compiled != len(game_units) or sdk_compiled != len(sdk_units) or platform_compiled != len(platform_units) or data_failures or signature_mismatches or (aggregate is not None and aggregate['exitCode']) else 0


def run(args, log):
    result = subprocess.run([str(arg) for arg in args], cwd=ROOT, capture_output=True, text=True)
    (BUILD / log).write_text(result.stdout + result.stderr)
    if result.returncode:
        errors = [line for line in result.stderr.splitlines() if any(word in line for word in ['error:','undefined','check line','Layout field'])]
        raise RuntimeError(f'{args[0]} failed; see {BUILD / log}\n' + '\n'.join(errors[:8]))
    return result.stdout


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--audit',action='store_true',help='Also compile all game units for the 32-bit intermediate target')
    args = parser.parse_args()
    if args.audit:
        for name in ('report.json','game-link.json','game-link.log','game-all.o','link-inventory.json','signature-inventory.json','original-data/report.json','native-objects.rsp'):
            (BUILD / 'audit' / name).unlink(missing_ok=True)
    run(['make','-s','-C',ROOT / 'tools/gen_fx_consts'], 'constants-tool.log')
    # The generator derives its include guard from this argument; match the existing build path.
    run([ROOT / 'tools/gen_fx_consts/gen_fx_consts','lib/include/nitro/fx/fx_const.h'], 'constants.log')
    run([sys.executable,ROOT / 'ios/tools/prepare_original_headers.py'], 'headers.log')
    prepare_tools()
    run([sys.executable,ROOT/'ios/tests/check_original_data.py'],'data-check.log')
    includes = [ROOT / 'ios/build-original-host/include', ROOT / 'include', ROOT / 'files', ROOT / 'src',
                ROOT / 'lib/include', ROOT / 'lib/include/cw', ROOT / 'lib/include/nitro',
                ROOT / 'lib/include/nnsys', ROOT / 'lib/include/nitro/os', ROOT / 'include/library', ROOT / 'lib/NitroSDK/include',
                ROOT / 'lib/NitroSDK/include-pokediamond']
    flags = ['--target=wasm32-wasip1', '-std=gnu11', '-O2', '-fwrapv', '-fno-strict-aliasing',
             '-fms-extensions', '-Wno-microsoft', '-Wno-unknown-pragmas', '-ferror-limit=5',
             '-DHEARTGOLD_NATIVE', '-DHEARTGOLD_ABI32', '-DSDK_ARM9', '-DSDK_CW', '-DSDK_CODE_ARM', '-DSDK_TS', '-D_NITRO',
             '-DPM_KEEP_ASSERTS', '-DPLATFORM_INTRINSIC_FUNCTION_BIT_CLZ32=__builtin_clz']
    for include in includes: flags += ['-I', include]
    sources = ['src/math_util.c', 'src/pokemon.c', 'src/wifi_connection.c', 'src/billboard_lists.c', 'src/billboard_gfx_sequence.c', 'src/field_interaction.c', 'src/field_hook_unused.c', 'src/spawn_warp.c', 'src/frontier_state.c',
               'lib/NitroSDK/src/fx/fx_mtx22.c','lib/NitroSystem/src/g2d_char_canvas_util.c','ios/Original/abi32/probe.c']
    sources += ['lib/NitroSDK/src/fx/'+name+'.c' for name in ['fx_mtx33_identity','fx_mtx33_rot_yz','fx_mtx43_identity','fx_mtx43_scale',
                                                           'fx_mtx43_rot','fx_mtx44_identity','fx_mtx44_rot']]
    sources += ['src/particle_system.c','lib/src/spl_manager.c','lib/src/spl_emitter.c','lib/src/spl_list.c']
    sources += ['lib/NitroSDK/src/mi/mi_memory.c','ios/Original/NativeGraphicsMemory.c','ios/Original/tests/memory_check.c']
    sources.append('ios/Original/NativeCompression.c')
    sources.append('src/pokeathlon/pokeathlon_course.c')
    sources.append('lib/NitroDWC/src/transport.c')
    sources += ['lib/NitroWiFi/src/soc.c','lib/NitroWiFi/src/soc_inet.c']
    sources.append('lib/NitroDWC/src/match_join.c')
    sources.append('lib/NitroDWC/src/ap_connect_control.c')
    sources.append('src/communication_command.c')
    sources.append('src/wifi_club_game_entry.c')
    sources.append('src/lobby_mem.c')
    sources += ['lib/src/nitrocrypto_rc4_init.c','lib/src/nitrocrypto_rc4.c','lib/src/nitrocrypto_rc4_fast.c',
                'lib/libCPS/src/cps_rc4.c','ios/Original/tests/rc4_check.c']
    sources += ['lib/NitroSDK/src/dgt_hash.c','lib/NitroSDK/src/dgt_hash2_arm4_small.c',
                'lib/libCPS/src/cps_sha1.c','ios/Original/NativeDgtData.c','ios/Original/tests/digest_check.c']
    sources += ['lib/NitroSDK/src/math/math_qsort.c','ios/Original/tests/qsort_check.c',
                'src/application/pokewalker/pokewalker_event_log.c']
    sources.append('ios/Original/abi32/lobby_string_check.c')
    sources.append('src/ppw_lobby_encode_message.c')
    sources.append('src/lobby_string_vector_erase_at_end.c')
    sources += ['src/chat_callbacks.c','ios/Original/abi32/chat_callback_check.c']
    sources.append('src/lobby_chat_manager.c')
    sources += ['src/lobby_request_map_find.c','src/lobby_requests.c','src/lobby_request_map_insert_node.c']
    sources += ['src/ppw_lobby_recruit.c','src/lobby_timer_map_find.c','src/lobby_channel_map_erase_key.c',
                'src/lobby_channel_map.c','src/lobby_chat_users.c','src/lobby_user_map.c','src/lobby_user_map_insert.c',
                'src/ov45_02242be0.c']
    sources.append('src/lobby_tree_nodes.c')
    sources += ['src/lobby_char_vector_assign_range.c','src/lobby_u32_vector_buffer.c',
                'src/lobby_u32_vector_buffer_deallocate.c','src/lobby_event_deque_map_init_copy.c','src/lobby_vip_vector_reallocate.c']
    sources += ['src/ppw_lobby_player_set_destroy_nodes.c','src/lobby_event_deque_map_recommend_size.c',
                'src/lobby_event_deque_map_buffer.c','src/lobby_string_vector_buffer_allocate.c','src/lobby_char_vector_buffer.c',
                'src/lobby_string_vector_buffer_deallocate.c','src/lobby_char_vector_buffer_deallocate.c',
                'src/ppw_lobby_schedule_init.c','src/ov45_0223e648.c','src/lobby_event_deque_map_length_error.c']
    objects = []
    for source in sources:
        target = BUILD / (Path(source).stem + '.o'); objects.append(target)
        run([SDK / 'bin/clang', *flags, '-c', ROOT / source, '-o', target], Path(source).stem + '.log')
    for source in ('src/ov45_02238f18.cpp','src/lobby_event_deque_copy.cpp','src/lobby_string_find_first_of.cpp',
                   'src/ov45_022384bc.cpp','src/ov45_0224268c.cpp','src/ov45_0223d644.cpp','src/ov45_022383e0.cpp',
                   'src/lobby_string_vector_append_copies_slow.cpp'):
        target = BUILD / (Path(source).stem + '.o'); objects.append(target)
        options = ['-std=gnu++98' if option == '-std=gnu11' else option for option in flags]
        run([SDK / 'bin/clang++',*options,'-c',ROOT / source,'-o',target],Path(source).stem+'.log')
    from build_original_data import prepare_arm,emit,verify,wasm_object
    dgt = prepare_arm(ROOT/'lib/asm/nitro_sdk_data.s',BUILD/'data-check/dgt.arm.o',SDK,flags)
    native_dgt = wasm_object((BUILD/'NativeDgtData.o').read_bytes(),True)
    for name,size in [('gDgtHash1Padding',4),('gDgtHash1MessageOrder',192),
                      ('gDgtHash1Constants',256),('gDgtHmacHash2Params',28)]:
        original = next(s for s in dgt['symbols'] if s['name']==name and s['section'] in dgt['sections'])
        expected_bytes = dgt['sections'][original['section']]['bytes'][original['offset']:original['offset']+size]
        native = next(s for s in native_dgt['symbols'] if s.get('name')==name)
        actual_bytes = native_dgt['segments'][native['segment']]['bytes'][native['offset']:native['offset']+size]
        if len(expected_bytes)!=size or native['size']!=size or actual_bytes!=expected_bytes:
            raise RuntimeError(f'Native DGT table view differs from original SDK data: {name}')
    arm = prepare_arm(ROOT/'asm/spawn_warp_data.s',BUILD/'data-check/spawn.arm.o',SDK,flags)
    assembly,expected,_ = emit(arm,{},set(),{},'spawn_proof')
    source = BUILD/'data-check/spawn.s'; source.write_text(assembly)
    run([SDK/'bin/clang','-c',source,'-o',BUILD/'data-check/spawn.o'],'spawn-data.log')
    verify(BUILD/'data-check/spawn.o',expected)
    objects += [BUILD/'data-check/fixture.o',BUILD/'data-check/spawn.o']
    ctype = prepare_arm(ROOT/'lib/MSL_C/asm/MSL_Common_ctype.s',BUILD/'data-check/ctype.arm.o',SDK,flags)
    assembly,expected,_ = emit(ctype,{},set(),{},'ctype_proof')
    source = BUILD/'data-check/ctype.s'; source.write_text(assembly)
    run([SDK/'bin/clang','-c',source,'-o',BUILD/'data-check/ctype.o'],'ctype-data.log')
    verify(BUILD/'data-check/ctype.o',expected)
    objects.append(BUILD/'data-check/ctype.o')
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
    from prepare_lobby_parts import build
    lobby = BUILD / 'lobby.o'
    code,diagnostics = build(BUILD / 'probe-lobby-parts',SDK,
                            ['-std=gnu++98' if option == '-std=gnu11' else option for option in flags],lobby)
    (BUILD / 'probe-lobby.log').write_text(diagnostics)
    if code: raise RuntimeError(f'Lobby partition/link failed; see {BUILD / "probe-lobby.log"}')
    objects.append(lobby.with_suffix('.a'))
    exports = ['HGAotLayout','HGAotPointerCheck','HGAotString','HGAotIterator','HGAotData','HGAotEncrypt','HGAotDecrypt','HGAotChecksum','HGAotLobbyFind','HGAotDequeCopy','HGAotWifiStatus','HGAotBillboardFrames','HGAotBottomScreenCallbacks','HGAotSequenceIndex','HGAotMovementAllowed',
               'HGAotSeed','HGAotRandom','HGAotMTSeed','HGAotMTRandom','HGAotAsmPointers','HGAotAsmRandom','HGAotSpawnWarp','HGAotFactoryFlags','HGAotSDKMath','HGAotSDKCharIndex','HGAotSDKMatrix','HGAotParticleCreation','CheckNativeMemory','CheckOriginalRC4','CheckOriginalDigests','CheckOriginalQSort','CheckOriginalLobbyStrings','CheckOriginalLobbyComparison','CheckOriginalLobbyCleanup','HGAotEventLogSort','HGAotCompressedResource','HGAotCourseStateDispatch','HGAotTransportCallbacks','HGAotSocketAdapters','HGAotMatchErrorRouting','HGAotSendBufferSpace','HGAotCommunicationModes','HGAotCommandDispatch','HGAotClubDisconnect','HGAotLobbyStringSearch']
    exports.append('CheckOriginalChatDispatch')
    exports.append('CheckOriginalLobbyRequestMap')
    exports.append('CheckOriginalLobbyMapErase')
    exports.append('CheckOriginalLobbyAllocators')
    exports.append('CheckOriginalLobbyGrowth')
    # These exports share a persistent game instance; command-mode wrappers run libc destructors after every call.
    link = [SDK / 'bin/clang', '--target=wasm32-wasip1', '-mexec-model=reactor', '-Wl,--export-memory',
            '-Wl,--gc-sections', '-Wl,--fatal-warnings', '-Wl,--initial-memory=2097152', '-Wl,--max-memory=2097152']
    run([*link, *['-Wl,--export=' + name for name in exports], *objects, '-o', BUILD / 'hg32.wasm'], 'link.log')
    run([WABT / 'bin/wasm2c', BUILD / 'hg32.wasm', '-n', 'hg32', '-o', BUILD / 'hg32.c'], 'wasm2c.log')
    runtime = WABT / 'share/wabt/wasm2c'
    native = ['-std=c11', '-O2', '-fno-optimize-sibling-calls', '-frounding-math',
              '-DWASM_RT_USE_MMAP=0', '-DWASM_RT_MEMCHECK_GUARD_PAGES=0', '-DWASM_RT_MEMCHECK_BOUNDS_CHECK=1',
              '-I', BUILD, '-I', WABT / 'include', '-I', runtime]
    native_sources = [BUILD / 'hg32.c', runtime / 'wasm-rt-impl.c', runtime / 'wasm-rt-mem-impl.c', runtime / 'wasm-rt-exceptions-impl.c']
    run(['xcrun', 'clang', *native, '-fsanitize=address,undefined', '-fno-sanitize-recover=all',
         ROOT / 'ios/Original/abi32/check.c', *native_sources, '-o', BUILD / 'abi32-check'], 'native.log')
    print(run([BUILD / 'abi32-check'], 'check.log').strip())
    device = BUILD / 'iphoneos'; device.mkdir(exist_ok=True)
    device_objects = []
    for source in native_sources:
        target = device / (source.stem + '.o'); device_objects.append(target)
        run(['xcrun', '--sdk', 'iphoneos', 'clang', '-target', 'arm64-apple-ios16.0', *native,
             '-c', source, '-o', target], source.stem + '-iphoneos.log')
    (device / 'libHeartGoldABI32Probe.a').unlink(missing_ok=True)
    run(['xcrun', 'ar', 'rcs', device / 'libHeartGoldABI32Probe.a', *device_objects], 'archive.log')
    print(f'iPhoneOS native C archive: {device / "libHeartGoldABI32Probe.a"}')
    if args.audit: return audit(flags, includes)


if __name__ == '__main__':
    raise SystemExit(main())
