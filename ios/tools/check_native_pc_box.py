# SPDX-License-Identifier: GPL-3.0-or-later
"""Run the original PC Box scenario from one private Route29 save copy and a bound Full receipt."""
import argparse
from concurrent.futures import ThreadPoolExecutor
import hashlib
import json
import os
from pathlib import Path
import re
import shlex
import signal
import subprocess
import sys
import tempfile
import time

sys.dont_write_bytecode = True
ROOT = Path(__file__).resolve().parents[2]
PROTECTED_MAIN = ROOT.parent.parent / 'pokeheartgold/sourceCode'
QA_EXPORTS = ('HGNativePCBoxQAGetState', 'HGNativePCBoxQARequest')
QA_FILES = tuple(ROOT / name for name in ('ios/tests/pc_box_qa.h', 'ios/tests/pc_box_guest.inc',
    'ios/Original/abi32/pc_box_scenario_check.c', 'ios/tools/check_native_pc_box.py',
    'ios/tests/npc_yesno_qa.h', 'ios/tests/npc_yesno_boot.inc', 'ios/tools/npc_yesno_boot_sources.py'))
POST_FULL_PRIVATE_HARNESS = frozenset(str(ROOT / name) for name in ('ios/tests/pc_box_guest.inc',
    'ios/Original/abi32/pc_box_scenario_check.c', 'ios/tools/check_native_pc_box.py'))
REQUIRED_COVERAGE = 0x3FF
QA_VERSION, QA_BYTES = 3, 1416
SHA256 = re.compile(r'[0-9a-f]{64}')


def helpers():
    from audit_original import digest
    from check_abi32 import BUILD, SDK, WABT, native_build_settings
    from check_native_bag_menu import hashes, sha
    from check_native_engine import generate_engine
    from check_native_rainy_field import INCLUDES
    from import_original_content import read_boot_header
    from prepare_native_content import verify
    return dict(digest=digest, BUILD=BUILD, SDK=SDK, WABT=WABT, native_build_settings=native_build_settings,
        hashes=hashes, sha=sha, generate_engine=generate_engine, INCLUDES=INCLUDES, read_boot_header=read_boot_header, verify=verify)


def read_json(path):
    with Path(path).open() as source:
        return json.load(source)


def file_sha(path):
    with Path(path).open('rb') as source:
        return hashlib.file_digest(source, 'sha256').hexdigest()


def wasm_exports(data):
    """Read only the bounded standard export section; no tool or compiler is needed."""
    if data[:8] != b'\0asm\x01\0\0\0':
        raise ValueError('Invalid WebAssembly header')
    def integer(offset, end):
        value = 0
        for shift in range(0, 35, 7):
            if offset >= end:
                raise ValueError('Truncated WebAssembly integer')
            byte = data[offset]; offset += 1
            value |= (byte & 127) << shift
            if not byte & 128:
                if value > 0xFFFFFFFF:
                    raise ValueError('WebAssembly integer exceeds u32')
                return value, offset
        raise ValueError('Oversized WebAssembly integer')
    offset, found = 8, None
    while offset < len(data):
        section = data[offset]; offset += 1
        size, offset = integer(offset, len(data)); end = offset + size
        if end > len(data):
            raise ValueError('Truncated WebAssembly section')
        if section == 7:
            if found is not None:
                raise ValueError('Duplicate WebAssembly export section')
            count, cursor = integer(offset, end); found = []
            if count > size:
                raise ValueError('Invalid WebAssembly export count')
            for _ in range(count):
                length, cursor = integer(cursor, end)
                if length > end - cursor:
                    raise ValueError('Truncated WebAssembly export name')
                name = data[cursor:cursor + length].decode('utf-8'); cursor += length
                if cursor >= end or data[cursor] > 4:
                    raise ValueError('Invalid WebAssembly export kind')
                cursor += 1
                _, cursor = integer(cursor, end)
                if name in found:
                    raise ValueError('Duplicate WebAssembly export name')
                found.append(name)
            if cursor != end:
                raise ValueError('Unexpected WebAssembly export bytes')
        offset = end
    if found is None:
        raise ValueError('Missing WebAssembly export section')
    return sorted(found)


def shipping_exports_passed(exports, actual):
    return isinstance(exports, list) and len(exports) == 30 and all(isinstance(name, str) for name in exports) \
        and len(set(exports)) == 30 and not any('QA' in name for name in exports) \
        and {'HGNativePCBoxUIGetState', 'HGNativeFieldChoiceUIGetState', 'HGNativeTrainerCardUIGetState', 'HGNativeFieldMenuUIGetState'}.issubset(exports) and sorted(exports) == actual


def normal_checks_passed(checks, count):
    return isinstance(checks, list) and bool(checks) and all(isinstance(name, str) for name in checks) \
        and len(checks) == len(set(checks)) == count \
        and {'CheckNativePCBoxUI', 'CheckOriginalSummaryUI', 'CheckNativeFieldChoiceUI', 'CheckNativeTrainerCardUI', 'CheckNativeFieldMenuUI'}.issubset(checks)


def current_input_paths():
    result = subprocess.run(['git', 'ls-files', '-z', '--cached', '--others', '--exclude-standard'], cwd=ROOT,
        env={**os.environ, 'GIT_OPTIONAL_LOCKS': '0'}, capture_output=True, check=True, timeout=15, start_new_session=True)
    paths = {str(ROOT / name) for name in os.fsdecode(result.stdout).split('\0') if name}
    paths |= {str(path) for path in (ROOT / 'ios/build-original-host/include').rglob('*') if path.is_file()}
    paths.add(str(ROOT / 'lib/include/nitro/fx/fx_const.h'))
    return paths


def require_hashes(values, label):
    if not isinstance(values, dict) or not values:
        raise ValueError(label + ' must be a nonempty file-hash bank')
    for name, expected in values.items():
        path = Path(name)
        if not path.is_absolute() or not isinstance(expected, str) or not SHA256.fullmatch(expected):
            raise ValueError('Invalid ' + label + ' row: ' + str(name))
        if not path.is_file() or file_sha(path) != expected:
            raise RuntimeError(label + ' changed: ' + name)


def post_full_private_harness_delta(historical, current):
    if not isinstance(historical, dict) or not historical or not isinstance(current, dict) or set(historical) != set(current):
        raise ValueError('Post-Full harness changes must preserve the exact input inventory')
    for bank in (historical, current):
        if any(not isinstance(name, str) or not Path(name).is_absolute() or not isinstance(value, str)
                or not SHA256.fullmatch(value) for name, value in bank.items()):
            raise ValueError('Invalid post-Full input hash bank')
    delta = {name: dict(before=before, after=current[name]) for name, before in historical.items() if current[name] != before}
    if set(delta) - POST_FULL_PRIVATE_HARNESS:
        raise RuntimeError('Post-Full change outside the three approved private harness files: ' + ', '.join(sorted(set(delta) - POST_FULL_PRIVATE_HARNESS)))
    return delta


def seal_files(seals):
    """Keep every concrete Full file seal, including protected files outside this checkout."""
    result = {}
    def visit(value):
        if not isinstance(value, dict):
            return
        for name, expected in value.items():
            if Path(name).is_absolute() and isinstance(expected, str) and SHA256.fullmatch(expected):
                if name in result and result[name] != expected:
                    raise ValueError('Conflicting Full file seal: ' + name)
                result[name] = expected
            elif isinstance(expected, dict):
                visit(expected)
    visit(seals)
    if not result:
        raise ValueError('Full has no concrete protected file seals')
    return result


def validate_seal_metadata(seals):
    if not isinstance(seals.get('sources'), dict) or not isinstance(seals.get('tools'), dict) \
            or not any(isinstance(seals.get(name), dict) for name in ('protected', 'protected45')):
        raise ValueError('Full source, tool and protected-file seal groups are required')
    links = seals.get('toolLinks')
    if not isinstance(links, dict):
        raise ValueError('Full tool symlink inventory is required')
    for name, target in links.items():
        path = Path(name)
        if not path.is_absolute() or not isinstance(target, str) or not path.is_symlink() or os.readlink(path) != target:
            raise RuntimeError('Full tool symlink changed: ' + str(name))
    expected = seals.get('workingState')
    commands = {
        'wtStatus': (ROOT, ['status', '--porcelain=v1', '-z', '-uall']),
        'wtHead': (ROOT, ['rev-parse', 'HEAD']),
        'mainStatus': (PROTECTED_MAIN, ['status', '--porcelain=v1', '-z', '-uall']),
        'mainDiff': (PROTECTED_MAIN, ['diff', '--binary']),
        'mainHead': (PROTECTED_MAIN, ['rev-parse', 'HEAD']),
    }
    if not isinstance(expected, dict) or set(expected) != set(commands):
        raise ValueError('Full working-state seal must contain the five recorded git stdout hashes')
    for name, (folder, argv) in commands.items():
        output = subprocess.check_output(['git', *argv], cwd=folder, env={**os.environ, 'GIT_OPTIONAL_LOCKS': '0'},
            timeout=15, start_new_session=True)
        if hashlib.sha256(output).hexdigest() != expected[name]:
            raise RuntimeError('Full working-state changed: ' + name)


def validate_full(full_receipt):
    h = helpers(); receipt = Path(full_receipt).resolve(); folder = receipt.parent
    paths = {name: folder / name for name in ('engine.wasm', 'report.json', 'engine-link.json',
        'inputs-before.json', 'inputs-after.json', 'seals-before.json', 'seals-after.json')}
    if receipt.name != 'build-evidence.json' or any(path.is_symlink() or not path.is_file() for path in [receipt, *paths.values()]):
        raise RuntimeError('A complete explicit fresh Full receipt and its actual payloads are required')
    formal = read_json(receipt); audited = read_json(paths['report.json']); link = read_json(paths['engine-link.json'])
    if any(formal.get(key) is not True for key in ('passed', 'freshAuditPassed', 'inputsUnchanged')) or formal.get('firstFailure') is not None:
        raise RuntimeError('Full did not pass with unchanged inputs')
    shipping = formal.get('shipping', {})
    exports = shipping.get('exports', [])
    if not shipping_exports_passed(exports, wasm_exports(paths['engine.wasm'].read_bytes())) \
            or shipping.get('shippingQaExportCount', 0) != 0 \
            or shipping.get('undefinedSymbols', []) != [] or shipping.get('strictLinkExit', 0) != 0 \
            or shipping.get('sha256') != file_sha(paths['engine.wasm']):
        raise RuntimeError('Full shipping module must contain exactly the actual 30 public exports, including Field Menu, and no private QA')
    checks = formal.get('actualNormalChecks', [])
    if not normal_checks_passed(checks, formal.get('actualNormalCheckKinds')):
        raise RuntimeError('Full must include current registered PC Box, Summary and Field Choice checks')
    units = audited.get('units', [])
    source_map = {row['file']: row['sha256'] for row in units}
    if any(Path(name).is_absolute() or '..' in Path(name).parts for name in source_map):
        raise ValueError('Audited source names must remain inside the current checkout')
    source_map_sha = hashlib.sha256(json.dumps(source_map, sort_keys=True, separators=(',', ':')).encode()).hexdigest()
    excluded = audited['nativeTargetSelection']['excludedLibraryUnits']
    selected = sum(not row['exitCode'] for row in units)
    if len(source_map) != len(units) or formal.get('auditedUnits') != len(units) \
            or formal.get('selectedCompiledUnits') != selected or formal.get('excludedAuditUnits') != len(excluded) \
            or len(units) - selected != len(excluded) or audited['nativeTargetSelection']['errors'] \
            or audited['nativeTargetSelection']['enabled'] is not True or audited['nativeDataSelection']['enabled'] is not True \
            or {row['file'] for row in units if row['exitCode']} != {row['file'] for row in excluded} \
            or formal.get('verifiedDataUnits') != audited['nativeDataSelection']['summary']['verified'] \
            or audited['nativeDataSelection']['summary']['blocked'] or audited['nativeDataSelection']['errors'] \
            or formal.get('auditedSources', {}).get('allCurrentSourceHashesMatch') is not True \
            or formal.get('auditedSources', {}).get('sourceMapSha256') != source_map_sha:
        raise RuntimeError('Full source/data unit counts and selected exclusions do not match its actual audit')
    historical_bank = read_json(paths['inputs-before.json'])
    input_paths = current_input_paths()
    if historical_bank != read_json(paths['inputs-after.json']) or set(historical_bank) != input_paths \
            or not set(map(str, QA_FILES)).issubset(historical_bank):
        raise RuntimeError('Current source inventory differs from the Full input bank')
    bank = h['hashes'](map(Path, input_paths))
    private_delta = post_full_private_harness_delta(historical_bank, bank)
    seals = read_json(paths['seals-before.json'])
    if seals != read_json(paths['seals-after.json']):
        raise RuntimeError('Full protected input seals differ before/after')
    validate_seal_metadata(seals)
    protected = seal_files(seals)
    for name, change in private_delta.items():
        if protected.get(name) != change['before']:
            raise RuntimeError('Historical Full seal does not bind the approved private input: ' + name)
        protected[name] = change['after']
    require_hashes(protected, 'Full protected inputs with recorded private harness delta')
    head = subprocess.check_output(['git', 'rev-parse', 'HEAD'], cwd=ROOT, env={**os.environ, 'GIT_OPTIONAL_LOCKS': '0'},
        text=True, timeout=15, start_new_session=True).strip()
    audit = h['BUILD'] / 'audit'
    if link != read_json(audit / 'engine-link.json') or audited != read_json(audit / 'report.json') \
            or link.get('exitCode') or link.get('undefinedSymbols') != [] \
            or link.get('outputSha256') != shipping['sha256'] or file_sha(audit / 'engine.wasm') != shipping['sha256']:
        raise RuntimeError('Canonical strict audit no longer matches the bound actual Full')
    include_paths = [ROOT / name for name in h['INCLUDES']]
    if link.get('nativeContent'):
        include_paths.insert(0, ROOT / link['nativeContent']['include'])
    headers = {path for root in include_paths for pattern in ('**/*.h', '**/*.naix') for path in root.glob(pattern)}
    headers |= set((ROOT / 'lib/MSL_C/src').glob('*.h')) | set((ROOT / 'lib/MSL_C/src').glob('*.hpp'))
    if h['digest'](headers) != audited['headersSha256'] or shipping.get('headersSha256') != audited['headersSha256']:
        raise RuntimeError('Current headers differ from the bound Full')
    for name, expected in source_map.items():
        if file_sha(ROOT / name) != expected:
            raise RuntimeError('Audited source changed: ' + name)
    command = link['command'].copy(); rsp, = [value for value in command if value.startswith('@')]
    objects = shlex.split(Path(rsp[1:]).read_text())
    if not objects or file_sha(rsp[1:]) != link.get('objectListSha256') \
            or '-Wl,--fatal-warnings' not in command or '-Wl,--gc-sections' not in command:
        raise RuntimeError('Strict link command or object list identity changed')
    cached = [Path(name) for name in objects] + [Path(name) for name in command if name.endswith(('.o', '.a'))]
    cached += [audit / 'engine.wasm', audit / 'engine-link.json', audit / 'report.json', Path(rsp[1:]), receipt, *paths.values()]
    for option in command:
        if option.startswith('-Wl,--allow-undefined-file='):
            cached.append(Path(option.split('=', 1)[1]))
    seals_now = dict(protected)
    seals_now.update(bank); seals_now.update(h['hashes'](cached))
    runtime_bank_bytes = (json.dumps(bank, sort_keys=True, indent=2) + '\n').encode()
    return dict(helpers=h, receipt=receipt, formal=formal, audited=audited, link=link, audit=audit, objects=objects,
        rsp=rsp, includes=include_paths, protected=seals_now, sourceBank=bank, headers=headers, head=head,
        pack=h['verify'](link.get('nativeContent')), sourceMapSha256=source_map_sha, seals=seals,
        fullSourceBankCurrent=not private_delta, historicalFullInputBankSha256=file_sha(paths['inputs-before.json']),
        actualRuntimeInputBankSha256=hashlib.sha256(runtime_bank_bytes).hexdigest(), runtimeInputBankBytes=runtime_bank_bytes,
        postFullPrivateHarnessDelta=private_delta, currentQAHashes={str(path): bank[str(path)] for path in QA_FILES})


def assert_unchanged(context):
    if set(context['sourceBank']) != current_input_paths():
        raise RuntimeError('Source inventory changed during the private scenario')
    require_hashes(context['protected'], 'Protected source/cache/Full inputs')
    validate_seal_metadata(context['seals'])
    if context['helpers']['digest'](context['headers']) != context['audited']['headersSha256']:
        raise RuntimeError('Header bank changed during the private scenario')


def prepare_pc_box_guest(destination, run, full_receipt, context=None):
    """Build one isolated field-owner observer; reusable by native and future App CHECK runners."""
    destination = Path(destination)
    if destination.exists():
        raise RuntimeError('Private guest output must not exist: ' + str(destination))
    context = validate_full(full_receipt) if context is None else context
    if context['receipt'] != Path(full_receipt).resolve():
        raise RuntimeError('Private preparation context does not bind the requested Full receipt')
    assert_unchanged(context)
    h = context['helpers']; destination.mkdir(parents=True)
    runtime_bank_file = destination / 'runtime-input-bank.json'
    runtime_bank_file.write_bytes(context['runtimeInputBankBytes'])
    if file_sha(runtime_bank_file) != context['actualRuntimeInputBankSha256']:
        raise RuntimeError('Serialized runtime input bank identity changed')
    context['actualRuntimeInputBankFile'] = str(runtime_bank_file)
    context['protected'][str(runtime_bank_file)] = context['actualRuntimeInputBankSha256']
    formal_field = context['audit'] / 'objects/src/field_system.o'
    unit, = [row for row in context['audited']['units'] if row['file'] == 'src/field_system.c']
    objects = context['objects'].copy()
    if unit['exitCode'] or unit['overlayStates'] or objects.count(str(formal_field)) != 1:
        raise RuntimeError('Exactly one audited resident field owner is required')
    original = ROOT / 'src/field_system.c'; code = original.read_text()
    anchor = 'BOOL finished = FieldSystem_Main(fieldSystem);'; hook = '\n    PCBoxQAFieldTick(fieldSystem);'
    if code.count(anchor) != 1:
        raise RuntimeError('Original field-owner observation anchor changed')
    source = destination / 'field-pc-box.c'
    body = code.replace(anchor, anchor + hook)
    if body.replace(anchor + hook, anchor) != code:
        raise RuntimeError('Private copy altered the original field body')
    source.write_text('#include "' + str(ROOT / 'ios/tests/pc_box_guest.inc') + '"\n#line 1 "src/field_system.c"\n' + body)
    flags = ['--target=wasm32-wasip1', '-std=gnu11', '-O2', '-fwrapv', '-fno-strict-aliasing',
        '-fno-builtin-malloc', '-fno-builtin-free', '-fwasm-exceptions', '-fexceptions', '-Xclang', '-disable-red-zone',
        '-fms-extensions', '-Wno-microsoft', '-Wno-unknown-pragmas']
    flags += ['-D' + name for name in ('HEARTGOLD_NATIVE', 'HEARTGOLD_ABI32', 'SDK_ARM9', 'SDK_CW',
        'SDK_CODE_ARM', 'SDK_TS', '_NITRO', 'PM_KEEP_ASSERTS', 'PLATFORM_INTRINSIC_FUNCTION_BIT_CLZ32=__builtin_clz')]
    for path in context['includes']:
        flags += ['-I', path]
    if context['link'].get('nativeContent'):
        flags.append('-DHG_NATIVE_ROM_TABLES')
    flags += ['-I', ROOT / 'ios/tests']
    private_object = source.with_suffix('.o')
    run([h['SDK'] / 'bin/clang', *flags, '-c', source, '-o', private_object], destination / 'field-compile.log')
    objects[objects.index(str(formal_field))] = str(private_object)
    from npc_yesno_boot_sources import build_candidates
    from prepare_overlay_state import compile_unit
    title,menu=build_candidates(ROOT,destination/'source')
    replaced=[str(formal_field)]; private_objects=[str(private_object)]; boot_sources=[]
    for relative,copy in zip(('src/title_screen.c','src/application/main_menu/main_menu.c'),(title,menu)):
        unit,=[row for row in context['audited']['units'] if row['file']==relative]
        old=context['audit']/'objects'/Path(relative).with_suffix('.o'); target=copy.with_suffix('.o')
        if unit['exitCode'] or objects.count(str(old))!=1: raise RuntimeError('Exactly one audited original boot owner is required: '+relative)
        code,diagnostics,state=compile_unit(str(h['SDK']/'bin/clang'),flags,copy,target,relative,ROOT)
        (destination/(Path(relative).stem+'-compile.log')).write_text(diagnostics)
        expected=unit['overlayStates']
        if code or 'error' in state or (state.get('getter') or None)!=(expected[0]['getter'] if expected else None):
            raise RuntimeError('Private boot source-copy compile/ownership failed: '+relative)
        objects[objects.index(str(old))]=str(target);replaced.append(str(old));private_objects.append(str(target));boot_sources.append(copy)
    listing = destination / 'objects.rsp'; listing.write_text('\n'.join('"' + path + '"' for path in objects) + '\n')
    command = context['link']['command'].copy(); command[command.index(context['rsp'])] = '@' + str(listing)
    module = destination / 'engine.wasm'; command[command.index('-o') + 1] = str(module)
    for name in QA_EXPORTS:
        command.insert(command.index('-o'), '-Wl,--export=' + name)
    command.insert(command.index('-o'), '-Wl,--wrap=PCBox_Main')
    run(command, destination / 'strict-link.log')
    expected = sorted([*context['formal']['shipping']['exports'], *QA_EXPORTS])
    if wasm_exports(module.read_bytes()) != expected:
        raise RuntimeError('Private module exports differ beyond the two bounded PC QA getters')
    assert_unchanged(context)
    artifacts = h['hashes']([source, listing, module, runtime_bank_file, *map(Path,private_objects), *boot_sources, *QA_FILES])
    report = dict(passed=True, scope='isolated private PC observer build only; gameplay not executed',
        fullReceipt=str(context['receipt']), fullReceiptSha256=file_sha(context['receipt']), shippingModuleSha256=context['link']['outputSha256'],
        privateModuleSha256=file_sha(module), headersSha256=context['audited']['headersSha256'],
        sourceMapSha256=context['sourceMapSha256'], fullSourceBankCurrent=context['fullSourceBankCurrent'],
        historicalFullInputBankSha256=context['historicalFullInputBankSha256'], actualRuntimeInputBankSha256=context['actualRuntimeInputBankSha256'],
        actualRuntimeInputBankFile=context['actualRuntimeInputBankFile'], postFullPrivateHarnessDelta=context['postFullPrivateHarnessDelta'],
        currentQAHashes=context['currentQAHashes'],
        sourceInputCount=len(context['sourceBank']), auditedUnitCount=len(context['audited']['units']),
        shippingExports=context['formal']['shipping']['exports'], privateQAExports=list(QA_EXPORTS),
        replacedFormalObjects=replaced, privateObjects=private_objects, originalSourceSha256=file_sha(original),
        bootSourceCopies=[dict(original=relative,originalSha256=file_sha(ROOT/relative),source=str(copy),sourceSha256=file_sha(copy))
            for relative,copy in zip(('src/title_screen.c','src/application/main_menu/main_menu.c'),boot_sources)],
        originalBodyPreserved=True, postOriginalFieldHookCount=1, wrappers=['PCBox_Main'], artifactHashes=artifacts,
        protectedInputsUnchanged=True, productionApiAdded=False)
    (destination / 'preparation.json').write_text(json.dumps(report, indent=2) + '\n')
    return module, report


class Runner:
    def __init__(self, report):
        self.report = report

    def __call__(self, argv, log, timeout=240, check=True):
        argv = list(map(str, argv)); log = Path(log)
        if log.exists():
            raise RuntimeError('Refusing to overwrite a command log: ' + str(log))
        started = time.monotonic(); timed_out = False
        process = subprocess.Popen(argv, cwd=ROOT, env={**os.environ, 'HEARTGOLD_TEST_MUTE': '1', 'PYTHONDONTWRITEBYTECODE': '1'},
            stdout=subprocess.PIPE, stderr=subprocess.STDOUT, start_new_session=True)
        try:
            output, _ = process.communicate(timeout=timeout)
        except subprocess.TimeoutExpired:
            timed_out = True
            try:
                os.killpg(process.pid, signal.SIGTERM)
            except ProcessLookupError:
                pass
            try:
                output, _ = process.communicate(timeout=2)
            except subprocess.TimeoutExpired:
                try:
                    os.killpg(process.pid, signal.SIGKILL)
                except ProcessLookupError:
                    pass
                output, _ = process.communicate()
        group_empty = False
        for _ in range(20):
            try:
                os.killpg(process.pid, 0)
            except ProcessLookupError:
                group_empty = True; break
            os.killpg(process.pid, signal.SIGKILL); time.sleep(0.05)
        log.write_bytes(output)
        row = dict(argv=argv, log=str(log), terminalReturnCode=process.returncode, timedOut=timed_out,
            allChildrenWaited=group_empty, seconds=round(time.monotonic() - started, 3), outputSha256=hashlib.sha256(output).hexdigest())
        self.report['commands'].append(row)
        if check and (process.returncode or timed_out or not group_empty):
            raise RuntimeError('Private PC command failed: ' + str(log))
        return row, output.decode(errors='replace')


def evidence_passed(row):
    return isinstance(row, dict) and row.get('status') == 'passed_original_pc_box' \
        and row.get('qaVersion') == QA_VERSION and row.get('qaBytes') == QA_BYTES \
        and type(row.get('fieldMessageAcks')) is int and row['fieldMessageAcks'] >= 2 \
        and row.get('baselinePrepared') == 1 and row.get('noSettersAfterBaseline') == 1 \
        and type(row.get('conservationChecks')) is int and row['conservationChecks'] >= 10 \
        and row.get('requiredCoverageMask') == REQUIRED_COVERAGE and row.get('coverageMask') == REQUIRED_COVERAGE \
        and all(row.get(key) is True for key in ('resetFinished', 'resetQuiescent', 'physicalEntry')) \
        and row.get('privateSaveWrites') is False and type(row.get('staleRejected')) is int and row['staleRejected'] >= 3 \
        and type(row.get('busyRejected')) is int and row['busyRejected'] >= 1


def action_trace_passed(rows):
    if not rows or any(not isinstance(row, dict) or row.get('event') not in ('press', 'released', 'settled') for row in rows):
        return False
    events = [row['event'] for row in rows]
    return events.count('press') > 0 and events.count('press') == events.count('released') == events.count('settled') \
        and events[-1] == 'settled'


def field_message_evidence_passed(trace, count):
    if type(count) is not int or count < 2 or not isinstance(trace, list):
        return False
    indices = [index for index, row in enumerate(trace) if isinstance(row, dict) and row.get('owner') == 'field_message']
    if len(indices) != count * 3:
        return False
    for offset in range(0, len(indices), 3):
        group = indices[offset:offset + 3]
        if group != list(range(group[0], group[0] + 3)):
            return False
        rows = [trace[index] for index in group]
        if [row.get('event') for row in rows] != ['press', 'released', 'settled']:
            return False
        for row in rows:
            if any(type(row.get(key)) is not int or not 0 <= row[key] <= 0xFFFFFFFF for key in ('step', 'menuSubstep', 'keys')) \
                    or type(row.get('messageAwaitInput')) is not int or row['messageAwaitInput'] not in (0, 1):
                return False
            choice = row.get('fieldChoice')
            if not isinstance(choice, dict) or any(type(choice.get(key)) is not int or choice[key] not in (0, 1)
                    for key in ('active', 'ready', 'viewActive')) \
                    or any(type(choice.get(key)) is not int or not 0 <= choice[key] <= 0xFFFFFFFF
                    for key in ('phase', 'ownerGeneration', 'revision')):
                return False
        press = rows[0]; choice = press['fieldChoice']
        if press['keys'] != 1 or press['messageAwaitInput'] != 1 \
                or (choice['active'] and (choice['ready'] or choice['viewActive'] or 7 <= choice['phase'] <= 10)) \
                or any(row['keys'] or row['step'] != press['step'] or row['menuSubstep'] != press['menuSubstep'] for row in rows[1:]):
            return False
    return True


def snapshot_evidence_passed(rows):
    if not rows:
        return False
    captured = 0
    flags = ('pcActive', 'summaryActive', 'namingActive', 'scriptMenuReady')
    numbers = ('snapshot', 'step', 'frame', 'mapId', 'ownerGeneration', 'phase', 'requiredMask', 'renderedMask', 'variedMask')
    for row in rows:
        if not isinstance(row, dict) or row.get('event') not in ('waiting', 'captured') \
                or any(type(row.get(key)) is not int or not 0 <= row[key] <= 0xFFFFFFFF for key in numbers) \
                or any(type(row.get(key)) is not int or row[key] not in (0, 1) for key in flags):
            return False
        required = 3 if any(row[key] for key in flags) else 1
        rendered, varied = row['renderedMask'], row['variedMask']
        if row['snapshot'] != captured or row['requiredMask'] != required or rendered > 3 or varied & ~rendered:
            return False
        visible = required & rendered & varied == required
        if (row['event'] == 'captured') != visible:
            return False
        captured += row['event'] == 'captured'
    return captured > 0 and rows[-1]['event'] == 'captured'


def source_plumbing_check(header, guest, host):
    def region(source, opening):
        depth = 1
        for index in range(opening + 1, len(source)):
            depth += (source[index] == '{') - (source[index] == '}')
            if not depth:
                return source[opening + 1:index], opening + 1, index
        raise AssertionError('Unterminated source body')
    def function(source, name):
        definitions = list(re.finditer(r'\b' + name + r'\s*\([^;{}]*\)\s*\{', source))
        assert len(definitions) == 1, name + ' must have exactly one definition'
        return region(source, definitions[0].end() - 1)
    for name, source in [('header', header), ('guest', guest), ('host', host)]:
        exports = set(re.findall(r'\b(?:w2c_hgengine_)?(HGNativePCBoxQA\w+)\s*\(', source))
        assert exports == set(QA_EXPORTS), name + ' private export contract'
    wrapper, _, _ = function(guest, '__wrap_PCBox_Main')
    original_call = r'\b__real_PCBox_Main\s*\(\s*manager\s*,\s*state\s*\)'
    assert len(re.findall(original_call, guest)) == 1 and len(re.findall(original_call, wrapper)) == 1
    assert re.search(r'\bBOOL\s+result\s*=\s*' + original_call, wrapper) and re.search(r'\breturn\s+result\s*;', wrapper)
    prepare, start, end = function(guest, 'PCBoxQAPrepare')
    snapshots = list(re.finditer(r'\bPCBoxQASnapshot\s*\(\s*TRUE\s*\)\s*;', prepare))
    baselines = list(re.finditer(r'\bsPCQA\.baselinePrepared\s*=\s*1\s*;', prepare))
    assert len(snapshots) == len(baselines) == 1 and snapshots[0].start() < baselines[0].start()
    mutators = r'\b(CreateMon|SetMonData|Party_AddMon|PCStorage_Place\w*|PCStorage_SetActiveBox|Pokedex_SetMonSeenFlag|Pokedex_SetMonCaughtFlag)\s*\('
    calls = list(re.finditer(mutators, guest))
    assert {call[1] for call in calls} == {'CreateMon', 'SetMonData', 'Party_AddMon', 'PCStorage_PlaceMonInBoxByIndexPair',
        'PCStorage_SetActiveBox', 'Pokedex_SetMonSeenFlag', 'Pokedex_SetMonCaughtFlag'}
    assert all(start <= call.start() < start + snapshots[0].start() < end for call in calls), 'Fixture setters escaped the preparation phase'
    assert not re.search(mutators, header + host), 'Fixture setters must remain private to the guest preparation'
    steps = re.search(r'\bsteps\s*\[\s*\]\s*=\s*\{', host)
    assert steps, 'Missing original-input scenario steps'
    body, _, _ = region(host, steps.end() - 1)
    coverage = ('PHYSICAL', 'DEPOSIT', 'WITHDRAW', 'MOVE', 'SWAP', 'LAST', 'RELEASE_CANCEL', 'ITEMS', 'SUMMARY', 'NAMING')
    for name in coverage:
        assert re.search(r'\bHG_PC_QA_' + name + r'\b', body), 'Missing scenario coverage: ' + name
    for name in ('START', 'CHECKPOINT', 'RETURN'):
        assert re.search(r'\b(?:w2c_hgengine_)?HGNativePCBoxQARequest\s*\([^;]*\bHG_PC_QA_' + name + r'\b', host), 'Missing owner request: ' + name
    assert re.search(r'\bHG_PC_QA_REQUIRED\s*=\s*0x3ff\b', header, re.I), 'Required coverage mask drifted'
    assert re.search(r'\bHG_PC_QA_VERSION\s*=\s*3\b', header), 'Private PC protocol must be version 3'
    state = re.search(r'\btypedef\s+struct\s+HGPCBoxQAState\s*\{', header)
    assert state, 'Missing private PC state'
    state_body, _, _ = region(header, state.end() - 1)
    assert re.search(r'\buint32_t\s+messageAwaitInput\s*;\s*HGYNBootFacts\s+boot\s*;\s*$', state_body), 'Typed boot facts must append to the unchanged message prefix'
    assert re.search(r'\bsizeof\s*\(\s*HGPCBoxQAState\s*\)\s*==\s*1416\b', guest + host) \
        and re.search(r'\boffsetof\s*\(\s*HGPCBoxQAState\s*,\s*messageAwaitInput\s*\)\s*==\s*1368\b', guest + host) \
        and re.search(r'\boffsetof\s*\(\s*HGPCBoxQAState\s*,\s*boot\s*\)\s*==\s*1372\b', guest+host), 'Private PC protocol size/append offsets must remain pinned'
    assert 'NPCYNBootPublish' in guest and 'pcBootEligible' in host and 'pcBootExitWitness' in host and 'PC_BOOT_TOKEN' not in host, 'Typed boot owner/exit plumbing changed'
    assert re.search(r'\bHGNativeFieldChoiceUIState\b', host) \
        and re.search(r'\bw2c_hgengine_HGNativeFieldChoiceUIGetState\s*\(', host), 'Host must read the existing public Field Choice state'
    assert re.search(r'\bmessageAwaitInput\s*=', guest), 'Guest must publish the original message wait fact'
    assert '"field_message"' in host and 'fieldMessageAcks' in host, 'Original field message acknowledgments must be scoped and counted'
    assert not re.search(r'\b(?:PCBox_LaunchApp|PC_LaunchApp)\s*\(', header + guest + host), 'Physical entry bypassed by a direct app launch'
    assert re.search(r'\bBOOL\s+pcSnapshot\s*\(', host), 'Snapshots must report temporal readiness'
    assert not re.search(r'(?<!!)\bpcSnapshot\s*\(\s*&qa\s*\)', host), 'Snapshot caller ignored temporal readiness'


def self_check():
    def module(names):
        def leb(value):
            result = bytearray()
            while value >= 128:
                result.append((value & 127) | 128); value >>= 7
            return bytes(result + bytes([value]))
        body = leb(len(names)) + b''.join(leb(len(name)) + name.encode() + b'\0\0' for name in names)
        return b'\0asm\x01\0\0\0\x07' + leb(len(body)) + body
    assert wasm_exports(module(['memory', 'NitroMain', *QA_EXPORTS])) == sorted(['memory', 'NitroMain', *QA_EXPORTS])
    shipping = ['HGNativePCBoxUIGetState', 'HGNativeFieldChoiceUIGetState', 'HGNativeTrainerCardUIGetState', 'HGNativeFieldMenuUIGetState', *('public' + str(index) for index in range(26))]
    assert shipping_exports_passed(shipping, sorted(shipping))
    for invalid in (shipping[:-1], [*shipping[:-1], shipping[0]], [name.replace('HGNativeFieldChoiceUIGetState', 'missingChoice') for name in shipping],
            [name.replace('HGNativeTrainerCardUIGetState', 'missingTrainerCard') for name in shipping],
            [name.replace('HGNativeFieldMenuUIGetState', 'missingFieldMenu') for name in shipping],
            [*shipping[:-1], QA_EXPORTS[0]]):
        assert not shipping_exports_passed(invalid, sorted(invalid))
    assert not shipping_exports_passed(shipping, sorted([*shipping, 'extra']))
    checks = ['CheckNativePCBoxUI', 'CheckOriginalSummaryUI', 'CheckNativeFieldChoiceUI', 'CheckNativeTrainerCardUI', 'CheckNativeFieldMenuUI']
    assert normal_checks_passed(checks, len(checks)) and not normal_checks_passed(checks[:-1], len(checks) - 1)
    assert not normal_checks_passed(checks, len(checks) + 1)
    for data in (b'', module(['x', 'x']), module(['x'])[:-1], b'\0asm\x01\0\0\0\x07\xff\xff\xff\xff\xff'):
        try:
            wasm_exports(data)
        except (ValueError, UnicodeDecodeError):
            pass
        else:
            raise AssertionError('Malformed module accepted')
    valid = dict(status='passed_original_pc_box', baselinePrepared=1, noSettersAfterBaseline=1, conservationChecks=10,
        qaVersion=QA_VERSION, qaBytes=QA_BYTES, fieldMessageAcks=2,
        requiredCoverageMask=REQUIRED_COVERAGE, coverageMask=REQUIRED_COVERAGE, resetFinished=True, resetQuiescent=True,
        physicalEntry=True, privateSaveWrites=False, staleRejected=3, busyRejected=1)
    assert evidence_passed(valid)
    for key, value in [('status', 'failed'), ('coverageMask', 0x1FF), ('requiredCoverageMask', 0), ('conservationChecks', 9),
            ('noSettersAfterBaseline', 0), ('resetQuiescent', False), ('privateSaveWrites', True), ('staleRejected', 2), ('busyRejected', 0),
            ('qaVersion', 1), ('qaBytes', 1368), ('fieldMessageAcks', 1)]:
        assert not evidence_passed({**valid, key: value}), key
    for key in valid:
        assert not evidence_passed({name: value for name, value in valid.items() if name != key}), key
    assert not evidence_passed(None)
    source_hash = 'a' * 64
    old_bank = {name: source_hash for name in POST_FULL_PRIVATE_HARNESS}
    new_bank = {name: 'b' * 64 for name in old_bank}
    assert post_full_private_harness_delta(old_bank, old_bank) == {}
    assert post_full_private_harness_delta(old_bank, new_bank) == {name: dict(before=source_hash, after='b' * 64) for name in old_bank}
    rejected_banks = [(old_bank, {name: value for name, value in new_bank.items() if name != str(QA_FILES[1])}),
        (old_bank, {**new_bank, str(ROOT / 'ios/tests/new_private_case.c'): source_hash}),
        ({name: value for name, value in old_bank.items() if name != str(QA_FILES[1])}, new_bank)]
    for path in (QA_FILES[0], *QA_FILES[4:], ROOT / 'src/application/pc_box/pc_box.c', ROOT / 'include/native_pc_box_ui.h',
            ROOT / 'ios/OriginalApp/main.m', ROOT / 'ios/tools/build_original_app.py', ROOT.parent / 'abi32-tools/compiler'):
        rejected_banks.append(({**old_bank, str(path): source_hash}, {**new_bank, str(path): 'b' * 64}))
    for historical, current in rejected_banks:
        try:
            post_full_private_harness_delta(historical, current)
        except (ValueError, RuntimeError):
            pass
        else:
            raise AssertionError('Unauthorized post-Full input change accepted')
    assert seal_files({'sources': {'/tmp/pc-box-self-check-input': source_hash}, 'workingState': {'wtHead': 'b' * 64}}) \
        == {'/tmp/pc-box-self-check-input': source_hash}
    trace = [dict(event=event, step=0, key=1, padReads=index, frame=index, owner='field', pc={}, summary={}, naming={})
        for index, event in enumerate(('press', 'released', 'settled'))]
    assert action_trace_passed(trace) and not action_trace_passed(trace[:-1]) and not action_trace_passed(trace + trace[:1])
    assert not action_trace_passed([]) and not action_trace_passed([dict(event='unknown')])
    choice = dict(active=1, ready=0, viewActive=0, phase=4, ownerGeneration=1, revision=2)
    message = [dict(event=event, owner='field_message', step=2, menuSubstep=0, keys=1 if index == 0 else 0,
        messageAwaitInput=1 if index == 0 else 0, fieldChoice=dict(choice)) for index, event in enumerate(('press', 'released', 'settled'))]
    messages = [*message, *message]
    assert field_message_evidence_passed(messages, 2)
    resumed = [*message, *message]
    resumed[1] = {**resumed[1], 'fieldChoice': {**choice, 'phase': 8, 'ready': 1, 'viewActive': 1}}
    assert field_message_evidence_passed(resumed, 2)  # Original dialogue may create its next list after A.
    assert not field_message_evidence_passed(messages, 1) and not field_message_evidence_passed(message, 2)
    for change in ({'keys': 2}, {'messageAwaitInput': 0}, {'messageAwaitInput': 2}, {'owner': 'field'}, {'event': 'released'},
            {'fieldChoice': {**choice, 'ready': 1}}, {'fieldChoice': {**choice, 'viewActive': 1}},
            *({'fieldChoice': {**choice, 'phase': phase}} for phase in range(7, 11))):
        assert not field_message_evidence_passed([{**messages[0], **change}, *messages[1:]], 2), change
    for index, change in ((1, {'step': 3}), (2, {'menuSubstep': 1}), (1, {'owner': 'field'}), (2, {'event': 'released'})):
        invalid = messages.copy(); invalid[index] = {**invalid[index], **change}
        assert not field_message_evidence_passed(invalid, 2), change
    assert not field_message_evidence_passed([messages[0], trace[0], *messages[1:]], 2)
    field_snapshot = dict(event='captured', snapshot=0, step=1, requiredMask=1, renderedMask=1, variedMask=1,
        frame=1483, mapId=69, ownerGeneration=0, phase=0, pcActive=0, summaryActive=0, namingActive=0, scriptMenuReady=0)
    assert snapshot_evidence_passed([field_snapshot])
    assert snapshot_evidence_passed([{**field_snapshot, 'renderedMask': 3}])  # Actual uniform field SUB is optional.
    both = {**field_snapshot, 'pcActive': 1, 'requiredMask': 3, 'renderedMask': 3, 'variedMask': 3}
    assert snapshot_evidence_passed([both])
    assert snapshot_evidence_passed([{**both, 'event': 'waiting', 'variedMask': 1}, both])
    for invalid in ({**both, 'variedMask': 1}, {**both, 'renderedMask': 2, 'variedMask': 2},
            {**both, 'requiredMask': 1}, {**field_snapshot, 'requiredMask': 0}, {**field_snapshot, 'renderedMask': 0},
            {**field_snapshot, 'renderedMask': 4, 'variedMask': 4}, {**field_snapshot, 'variedMask': 3},
            {**field_snapshot, 'event': 'waiting'}, {**field_snapshot, 'snapshot': 1}):
        assert not snapshot_evidence_passed([invalid]), invalid
    assert not snapshot_evidence_passed([]) and not snapshot_evidence_passed([field_snapshot, field_snapshot])
    for flag in ('summaryActive', 'namingActive', 'scriptMenuReady'):
        assert not snapshot_evidence_passed([{**field_snapshot, flag: 1}])
    header, guest, host = (path.read_text() for path in QA_FILES[:3])
    source_plumbing_check(header, guest, host)
    for broken_header, broken_guest, broken_host in [(re.sub(r'\b(HG_PC_QA_VERSION\s*=\s*)3\b', r'\g<1>2', header, count=1), guest, host),
            (header.replace('uint32_t messageAwaitInput;', 'uint32_t missingMessageFact;'), guest, host),
            (header, re.sub(r'\b1416\b', '1372', guest), re.sub(r'\b1416\b', '1372', host)),
            (header, guest, host.replace('w2c_hgengine_HGNativeFieldChoiceUIGetState', 'missingPublicFieldChoiceGetter')),
            (header, guest + '\nSetMonData(&mon,0,0);', host),
            (header, guest.replace('return result;', 'return TRUE;'), host),
            (header, guest, host.replace('HG_PC_QA_NAMING', '0')),
            (header, guest, host.replace('if (!pcSnapshot(&qa)) break;', 'pcSnapshot(&qa);')),
            (header, guest, host + '\nPCBox_LaunchApp(field,args);')]:
        try:
            source_plumbing_check(broken_header, broken_guest, broken_host)
        except AssertionError:
            pass
        else:
            raise AssertionError('Broken scenario plumbing accepted')
    from check_pc_box_uikit_binding import self_check as binding_self_check
    binding_self_check()
    block=host[host.index('typedef HGYNBootFacts PCBootFacts;'):host.index('// Pure boot predicate end.')]
    with tempfile.TemporaryDirectory(prefix='pc-boot-contract-') as directory:
        temporary=Path(directory); source=temporary/'check.c'; binary=temporary/'check'
        source.write_text('#include <stddef.h>\n#include <stdint.h>\n#include "npc_yesno_qa.h"\ntypedef uint32_t u32;\ntypedef int BOOL;\nenum {FALSE=0,TRUE=1};\n'
            +block+'\nint main(void){return !pcBootSelfCheck();}\n')
        subprocess.run(['xcrun','clang','-std=c11','-O2','-fsanitize=address,undefined','-fno-sanitize-recover=all',
            '-iquote',str(ROOT/'ios/tests'),'-iquote',str(ROOT/'include'),str(source),'-o',str(binary)],check=True,timeout=30)
        subprocess.run([str(binary)],check=True,timeout=10)
    print('PASS: PC formal30/FieldMenu gates, QA3/1416, typed boot lifecycle, scoped message leases and exact fixture/ledger checks; no build or game run.')

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--self-check', action='store_true')
    parser.add_argument('--full-receipt', type=Path)
    parser.add_argument('--save', type=Path)
    parser.add_argument('--output-name', default='native-pc-box-pass')
    args = parser.parse_args()
    if args.self_check:
        self_check(); return 0
    if not args.full_receipt or not args.save:
        parser.error('--full-receipt and --save are required for execution')
    if not re.fullmatch(r'[A-Za-z0-9][A-Za-z0-9_-]*', args.output_name):
        parser.error('--output-name must be one new private archive basename')
    destination = ROOT.parent / args.output_name
    if destination.exists():
        raise RuntimeError('Preserve the existing output; select a new name: ' + str(destination))
    allowed = (ROOT.parent / 'native-ios-route29-save/heartgold.sav').resolve()
    source = args.save.resolve()
    fixture = [Path(str(source) + suffix) for suffix in ('', '.console', '.wifi', '.lock')]
    if source != allowed or any(path.is_symlink() or not path.is_file() for path in fixture) \
            or set(fixture) != set(source.parent.glob(source.name + '*')):
        raise RuntimeError('Only the designated private Route29 fixture and its exact three companions are allowed')
    destination.mkdir(); frames = destination / 'frames'; frames.mkdir()
    report = dict(scope='original PC entry, storage/menu operations and child return from an explicitly prepared private fixture; no capture, UIKit/device or save-writing claim',
        fixturePolicy='Original CreateMon/Party_AddMon/PCStorage APIs prepare only the loaded private copy before its frozen baseline; later gameplay changes use original input',
        passed=False, complete=False, status='preparing', firstFailure=None, noAutomaticRetry=True, gameRuntimeExecuted=False,
        allChildrenWaited=False, protectedInputsUnchanged=False, commands=[], actionTrace=[], typedBootTrace=[], typedFieldMessageEvidence=[], typedSnapshotEvidence=[], evidence=[])
    runner = Runner(report); context = None
    try:
        fixture_hashes = {str(path): file_sha(path) for path in fixture}
        context = validate_full(args.full_receipt)
        report.update(fullReceipt=str(context['receipt']), fullReceiptSha256=file_sha(context['receipt']),
            shippingModuleSha256=context['link']['outputSha256'], headersSha256=context['audited']['headersSha256'],
            fullSourceBankCurrent=context['fullSourceBankCurrent'], historicalFullInputBankSha256=context['historicalFullInputBankSha256'],
            actualRuntimeInputBankSha256=context['actualRuntimeInputBankSha256'], actualRuntimeInputBankFile=None,
            postFullPrivateHarnessDelta=context['postFullPrivateHarnessDelta'], currentQAHashes=context['currentQAHashes'], sourceInputCount=len(context['sourceBank']),
            sourceMapSha256=context['sourceMapSha256'], fixtureHashes=fixture_hashes, head=context['head'])
        module, preparation = prepare_pc_box_guest(destination / 'guest', runner, args.full_receipt, context=context)
        report['privateGuest'] = preparation
        h = context['helpers']; native = destination / 'native'; native.mkdir()
        generated, writes, restores = h['generate_engine'](native, module, lambda argv, log: runner(argv, native / log))
        flags, runtime = h['native_build_settings'](native)
        flags += ['-g', '-fsanitize=address,undefined', '-fno-sanitize-recover=all']
        def compile_one(item):
            index, path = item; target = native / ('host-' + str(index) + '.o')
            extra = ['-DWASM_RT_TRAP_HANDLER=HGEngineTrap'] if path.name == 'wasm-rt-impl.c' else []
            runner(['xcrun', 'clang', *flags, *extra, '-c', path, '-o', target], native / ('compile-' + str(index) + '.log'))
            return target
        with ThreadPoolExecutor(max_workers=4) as pool:
            objects = list(pool.map(compile_one, enumerate([*generated, *runtime, QA_FILES[2]])))
        binary = native / 'pc-box-check'
        runner(['xcrun', 'clang', '-fsanitize=address,undefined', *objects, '-o', binary], native / 'native-link.log')
        boot = destination / 'boot-header.bin'; boot.write_bytes(h['read_boot_header'](context['pack']))
        copies = []
        for path in fixture:
            copy = destination / ('scenario.sav' + path.name[len(source.name):]); shutil_copy(path, copy); copies.append(copy)
        report['privateSaveHashesBefore'] = h['hashes'](copies)
        assert_unchanged(context); require_hashes(fixture_hashes, 'Original private fixture')
        report.update(status='running', gameRuntimeExecuted=True, privateModuleSha256=file_sha(module),
            checkedStackWrites=writes, exceptionDepthRestores=restores, nativeBinarySha256=file_sha(binary))
        row, output = runner([binary, context['pack'] / 'files', boot, destination / 'scenario.sav', frames],
            destination / 'runtime.log', timeout=300, check=False)
        for line in output.splitlines():
            if line.startswith('PC Box boot: '):
                report['typedBootTrace'].append(json.loads(line.removeprefix('PC Box boot: ')))
            elif line.startswith('PC Box input: '):
                action = json.loads(line.removeprefix('PC Box input: '))
                report['actionTrace'].append(action)
                if action.get('owner') == 'field_message':
                    report['typedFieldMessageEvidence'].append(action)
            elif line.startswith('PC Box snapshot: '):
                report['typedSnapshotEvidence'].append(json.loads(line.removeprefix('PC Box snapshot: ')))
            elif line.startswith('PC Box evidence: '):
                report['evidence'].append(json.loads(line.removeprefix('PC Box evidence: ')))
        evidence = report['evidence']
        report['privateSaveHashesAfter'] = h['hashes'](copies)
        from check_pc_box_uikit_binding import approved_boot, validate_boot_pixels
        legacy_boot=[dict(ordinal=int(a),frame=int(b),keys=int(c)) for a,b,c in re.findall(
            r'Native test input: step=(\d+) frame=(\d+) keys=(\d+) touch=0,0/0',output)]
        validated_boot=approved_boot(legacy_boot,report['typedBootTrace'])
        validate_boot_pixels(destination,validated_boot)
        if row['terminalReturnCode'] or row['timedOut'] or not row['allChildrenWaited'] or len(evidence) != 1 \
                or not evidence_passed(evidence[0]) or not action_trace_passed(report['actionTrace']) \
                or not field_message_evidence_passed(report['actionTrace'], evidence[0].get('fieldMessageAcks')) \
                or not snapshot_evidence_passed(report['typedSnapshotEvidence']) \
                or evidence[0].get('snapshots') != sum(row['event'] == 'captured' for row in report['typedSnapshotEvidence']):
            raise RuntimeError('Original PC scenario failed; retain the first runtime log and all output')
        if report['privateSaveHashesAfter'] != report['privateSaveHashesBefore']:
            raise RuntimeError('The private run unexpectedly wrote its save files')
        report.update(passed=True, complete=True, status='passed_original_pc_box')
    except Exception as error:
        report.update(status='failed', firstFailure=dict(type=type(error).__name__, message=str(error)))
    finally:
        try:
            if context:
                report['actualRuntimeInputBankFile'] = context.get('actualRuntimeInputBankFile')
                assert_unchanged(context)
            if 'fixture_hashes' in locals():
                require_hashes(fixture_hashes, 'Original private fixture')
            report['protectedInputsUnchanged'] = context is not None
        except Exception as error:
            report['passed'] = False; report['status'] = 'protected_input_changed'
            report['preservationFailure'] = dict(type=type(error).__name__, message=str(error))
        report['allChildrenWaited'] = bool(report['commands']) and all(row['allChildrenWaited'] for row in report['commands'])
        report['capturedFiles'] = {str(path.relative_to(destination)): file_sha(path) for path in frames.rglob('*') if path.is_file()}
        (destination / 'report.json').write_text(json.dumps(report, indent=2) + '\n')
    print(('PASS' if report['passed'] else 'FAIL') + ': ' + str(destination / 'report.json'))
    return 0 if report['passed'] else 1


def shutil_copy(source, destination):
    import shutil
    if source.resolve() == destination.resolve() or destination.exists():
        raise RuntimeError('Private save copy aliases an existing file')
    shutil.copy2(source, destination)


if __name__ == '__main__':
    raise SystemExit(main())
