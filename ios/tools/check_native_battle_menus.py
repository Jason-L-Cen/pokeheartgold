# SPDX-License-Identifier: GPL-3.0-or-later
"""Run original wild-battle Bag/Party information pages and item flow with a private CHECK guest."""
import argparse
from concurrent.futures import ThreadPoolExecutor
import hashlib
import json
import os
from pathlib import Path
import re
import shlex
import shutil
import subprocess

from audit_original import digest
from check_abi32 import ROOT, BUILD, SDK, WABT, battle_menu_component_data, native_build_settings
from check_native_engine import generate_engine
from check_native_rainy_field import INCLUDES
from import_original_content import read_boot_header
from prepare_native_content import verify

QA_EXPORTS = ('HGNativeBattleMenusQAGetState', 'HGNativeBattleMenusQARequest')
QA_SOURCES = tuple(ROOT / name for name in ('ios/tests/battle_menus_qa.h', 'ios/tests/battle_menus_guest.inc',
                                         'ios/tests/battle_menus_scenario_check.c', 'ios/tools/check_native_battle_menus.py',
                                         'asm/battle_menu_data.s'))


def sha(path):
    with Path(path).open('rb') as stream:
        return hashlib.file_digest(stream, 'sha256').hexdigest()


def hashes(paths):
    return {str(path): sha(path) for path in sorted(set(map(Path, paths)))}


def prepare_battle_menus_guest(destination, run):
    """The same private guest is used by native CQA and build_original_app --check."""
    destination = Path(destination)
    destination.mkdir(parents=True, exist_ok=True)
    if any(destination.iterdir()):
        raise RuntimeError('Private guest directory must be empty: ' + str(destination))
    audit = BUILD / 'audit'
    link = json.loads((audit / 'engine-link.json').read_text())
    audited = json.loads((audit / 'report.json').read_text())
    if link['exitCode'] or sha(audit / 'engine.wasm') != link['outputSha256']:
        raise RuntimeError('A fresh, strict full audit is required')
    if any(name in ' '.join(map(str, link['command'])) for name in QA_EXPORTS):
        raise RuntimeError('Shipping link must not contain private QA exports')
    def inspect_exports(module, log):
        result = subprocess.run([str(WABT / 'bin/wasm-objdump'), '-x', str(module)], capture_output=True, text=True, timeout=15)
        (destination / log).write_text(result.stdout + result.stderr)
        if result.returncode:
            raise RuntimeError('Unable to inspect guest exports')
        return sorted(re.findall(r'-> "(HGNativeBattleMenusQA[^\"]+)"', result.stdout))
    shipping_exports = inspect_exports(audit / 'engine.wasm', 'shipping-exports.log')
    if shipping_exports:
        raise RuntimeError('Shipping module contains private QA exports')
    pack = verify(link.get('nativeContent'))
    includes = [ROOT / name for name in INCLUDES]
    if link.get('nativeContent'):
        includes.insert(0, ROOT / link['nativeContent']['include'])
    headers = {path for folder in includes for pattern in ('**/*.h', '**/*.naix') for path in folder.glob(pattern)}
    headers |= set((ROOT / 'lib/MSL_C/src').glob('*.h')) | set((ROOT / 'lib/MSL_C/src').glob('*.hpp'))
    if digest(headers) != audited['headersSha256']:
        raise RuntimeError('Current headers differ from the full audit')
    units = {row['file']: row for row in audited['units']}
    sources = []
    for name, unit in units.items():
        path = ROOT / name
        if not unit['exitCode'] and path.is_file():
            if sha(path) != unit['sha256']:
                raise RuntimeError('Current source differs from its full audit: ' + name)
            sources.append(path)
    field_source = ROOT / 'src/field_system.c'
    if units['src/field_system.c']['exitCode'] or units['src/field_system.c']['overlayStates']:
        raise RuntimeError('The private task must reside in the verified resident field object')
    command = link['command'].copy()
    original_rsp = next(value for value in command if value.startswith('@'))
    objects = shlex.split(Path(original_rsp[1:]).read_text())
    formal_field = str(audit / 'objects/src/field_system.o')
    if objects.count(formal_field) != 1:
        raise RuntimeError('Expected exactly one formal field object')
    cached = [Path(path) for path in objects] + [Path(path) for path in command if path.endswith(('.o', '.a'))]
    cached += [audit / 'engine.wasm', Path(original_rsp[1:]), audit / 'engine-link.json', audit / 'report.json', pack / 'manifest.json']
    if link.get('nativeContent'):
        cached.append(ROOT / link['nativeContent']['report'])
    seals = dict(sourcesAndHeaders=hashes([*sources, *headers, *QA_SOURCES]), sharedCache=hashes(cached))
    copied = []
    for path in [*QA_SOURCES, field_source, ROOT / 'ios/Original/abi32/engine_check.c',
                 ROOT / 'src/battle/battle_controller.c', ROOT / 'src/battle/battle_022378C0.c',
                 ROOT / 'src/battle/battle_main.c', ROOT / 'src/battle/battle_setup.c',
                 ROOT / 'src/battle/battle_bag_menu.c', ROOT / 'src/battle/battle_party_menu.c',
                 ROOT / 'include/native_battle_bag_ui.h', ROOT / 'include/native_battle_party_ui.h']:
        target = destination / 'sources' / path.relative_to(ROOT)
        target.parent.mkdir(parents=True, exist_ok=True); shutil.copy2(path, target); copied.append(target)
    # Battle_BattleExit is called across translation units; --wrap must intercept the real teardown.
    owner = audit / 'objects/src/battle/battle_022378C0.o'
    symbol = subprocess.run([str(SDK / 'bin/llvm-nm'), '--undefined-only', str(owner)], capture_output=True, text=True, timeout=15)
    (destination / 'lifecycle-symbol.log').write_text(symbol.stdout + symbol.stderr)
    if symbol.returncode or not re.search(r'\bU\s+Battle_BattleExit\b', symbol.stdout):
        raise RuntimeError('Formal Battle_BattleExit caller is not an undefined external reference')
    anchor = 'BOOL finished = FieldSystem_Main(fieldSystem);'
    code = field_source.read_text()
    if code.count(anchor) != 1:
        raise RuntimeError('Original field-owner hook changed')
    private_source = destination / 'field-menus.c'
    private_source.write_text('#include "' + str(ROOT / 'ios/tests/battle_menus_guest.inc') + '"\n#line 1 "src/field_system.c"\n' +
                              code.replace(anchor, anchor + '\n    BattleMenusQAFieldTick(fieldSystem);'))
    flags = ['--target=wasm32-wasip1', '-std=gnu11', '-O2', '-fwrapv', '-fno-strict-aliasing',
             '-fno-builtin-malloc', '-fno-builtin-free', '-fwasm-exceptions', '-fexceptions', '-Xclang', '-disable-red-zone',
             '-fms-extensions', '-Wno-microsoft', '-Wno-unknown-pragmas']
    flags += ['-D' + name for name in ('HEARTGOLD_NATIVE', 'HEARTGOLD_ABI32', 'SDK_ARM9', 'SDK_CW', 'SDK_CODE_ARM',
                                     'SDK_TS', '_NITRO', 'PM_KEEP_ASSERTS', 'PLATFORM_INTRINSIC_FUNCTION_BIT_CLZ32=__builtin_clz')]
    for path in [*includes, ROOT / 'ios/tests']:
        flags += ['-I', path]
    if link.get('nativeContent'):
        flags += ['-DHG_NATIVE_ROM_TABLES']
    private_object = destination / 'field-menus.o'
    run([SDK / 'bin/clang', *flags, '-c', private_source, '-o', private_object], 'guest-field-compile.log')
    index = objects.index(formal_field)
    objects[index] = str(private_object)
    listing = destination / 'objects.rsp'
    listing.write_text('\n'.join('"' + path + '"' for path in objects) + '\n')
    command[command.index(original_rsp)] = '@' + str(listing)
    module = destination / 'engine.wasm'
    command[command.index('-o') + 1] = str(module)
    for name in QA_EXPORTS:
        command.insert(command.index('-o'), '-Wl,--export=' + name)
    for name in ('BattleController_EmitSelectCommand', 'BattleController_ReplyItemSelection', 'Battle_BattleExit'):
        command.insert(command.index('-o'), '-Wl,--wrap=' + name)
    run(command, 'guest-strict-link.log')
    private_exports = inspect_exports(module, 'private-exports.log')
    if private_exports != sorted(QA_EXPORTS):
        raise RuntimeError('Private guest QA export set differs from its fixed ABI')
    changed = {name: [path for path, expected in rows.items() if not Path(path).is_file() or sha(path) != expected]
               for name, rows in seals.items()}
    if any(changed.values()) or digest(headers) != audited['headersSha256']:
        raise RuntimeError('Protected audit inputs changed during private preparation')
    artifacts = hashes([private_source, private_object, listing, module, *QA_SOURCES, *copied])
    report = dict(scope='private CHECK guest only; no game or UIKit execution', shippingModuleSha256=link['outputSha256'],
                  privateGuestModuleSha256=sha(module), headersSha256=audited['headersSha256'], shippingQAExportCount=len(shipping_exports),
                  privateQAExports=private_exports, privateGuestDirectory=str(destination), privateGuestModule=str(module),
                  replacedObjectIndices=[index], replacedFormalObjects=[formal_field],
                  privateObjects=[str(private_object)], artifactHashes=artifacts, sealsBefore=seals, sealsAfter={key: True for key in seals},
                  sourceHook='original FieldSystem_Main then owner tick; original single wild task', passed=True)
    (destination / 'preparation.json').write_text(json.dumps(report, indent=2) + '\n')
    return module, report


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--save', required=True)
    parser.add_argument('--output-name', default='native-battle-menus-pass')
    args = parser.parse_args()
    if not re.fullmatch(r'native-battle-menus-[a-z0-9]+(?:-[a-z0-9]+)*', args.output_name):
        parser.error('--output-name must be one lowercase native-battle-menus- basename')
    source = Path(args.save).resolve()
    if source != (ROOT.parent / 'native-ios-route29-save/heartgold.sav').resolve() or sha(source) != '61fd40d5247db2c6a9b1c8d4a398038bb79f858733090a5352af1c633713198a':
        raise RuntimeError('Only the unchanged designated Route29 fixture is allowed')
    fixture = sorted(path for path in source.parent.glob(source.name + '*') if path.is_file())
    if {path.name for path in fixture} != {source.name + suffix for suffix in ('', '.console', '.wifi', '.lock')}:
        raise RuntimeError('Expected the original four private fixture files')
    destination = ROOT.parent / args.output_name
    if destination.exists():
        raise RuntimeError('Preserve existing evidence: ' + str(destination))
    destination.mkdir(); (destination / 'frames').mkdir()
    report = dict(complete=False, passed=False, status='building', commands=[], fixtureBefore=hashes(fixture))
    report_file = destination / 'report.json'
    report_file.write_text(json.dumps(report, indent=2) + '\n')
    def run(argv, log):
        argv = list(map(str, argv)); report['commands'].append(argv)
        result = subprocess.run(argv, cwd=ROOT, capture_output=True, text=True, timeout=240)
        (destination / log).write_text(result.stdout + result.stderr)
        if result.returncode:
            raise RuntimeError('Private battle menus build failed: ' + str(destination / log))
    artifacts = {}; preparation = {}
    try:
        module, preparation = prepare_battle_menus_guest(destination / 'guest', run)
        report.update(preparation=preparation, shippingModuleSha256=preparation['shippingModuleSha256'], privateGuestModuleSha256=preparation['privateGuestModuleSha256'])
        generated, writes, restores = generate_engine(destination, module, run)
        original_tables = battle_menu_component_data(destination)
        flags, runtime = native_build_settings(destination)
        git_file = ROOT / '.git'
        git_directory = Path(git_file.read_text().strip().removeprefix('gitdir: '))
        if not git_directory.is_absolute(): git_directory = ROOT / git_directory
        common = (git_directory / (git_directory / 'commondir').read_text().strip()).resolve()
        metadata = [git_file, *[path for path in git_directory.rglob('*') if path.is_file()],
                    *[path for path in common.iterdir() if path.is_file()]]
        metadata += [path for folder in ('refs', 'logs') for path in (common / folder).rglob('*') if path.is_file()]
        report['gitMetadataBefore'] = hashes(metadata)
        report['hostInputsBefore'] = hashes([*runtime, *sorted((ROOT / 'ios/Original').rglob('*.h')),
                                            *sorted((WABT / 'include').rglob('*.h')),
                                            *sorted((ROOT / 'ios/tools').glob('*.py')),
                                            ROOT / 'ios/Original/abi32/engine_check.c'])
        flags += ['-g', '-fsanitize=address,undefined', '-fno-sanitize-recover=all']
        def compile_one(item):
            index, path = item; target = destination / f'host-{index}.o'
            extra = ['-DWASM_RT_TRAP_HANDLER=HGEngineTrap'] if path.name == 'wasm-rt-impl.c' else []
            run(['xcrun', 'clang', *flags, *extra, '-c', path, '-o', target], f'compile-{index}.log')
            return target
        with ThreadPoolExecutor(max_workers=4) as pool:
            objects = list(pool.map(compile_one, enumerate([*generated, *runtime, original_tables, ROOT / 'ios/tests/battle_menus_scenario_check.c'])))
        binary = destination / 'battle-menus-check'
        run(['xcrun', 'clang', '-fsanitize=address,undefined', *objects, '-o', binary], 'native-link.log')
        artifacts = hashes([*preparation['artifactHashes'], *generated, original_tables, destination / 'hgengine.h', destination / 'hgengine-impl.h', *objects, binary])
        link = json.loads((BUILD / 'audit/engine-link.json').read_text()); pack = verify(link.get('nativeContent'))
        for path in fixture: shutil.copy2(path, destination / ('scenario.sav' + path.name[len(source.name):]))
        boot = destination / 'boot-header.bin'; boot.write_bytes(read_boot_header(pack))
        if any(hashes(rows) != rows for rows in preparation['sealsBefore'].values()) or hashes(artifacts) != artifacts:
            raise RuntimeError('Protected guest inputs or generated artifacts changed; never run a mixed result')
        if hashes(report['hostInputsBefore']) != report['hostInputsBefore'] or hashes(report['gitMetadataBefore']) != report['gitMetadataBefore']:
            raise RuntimeError('Host inputs or Git metadata changed; never run a mixed result')
        argv = list(map(str, [binary, pack / 'files', boot, destination / 'scenario.sav', destination / 'frames']))
        report.update(status='running', checkedStackWrites=writes, exceptionDepthRestores=restores, artifactHashes=artifacts)
        report['commands'].append(argv); report_file.write_text(json.dumps(report, indent=2) + '\n')
        try:
            result = subprocess.run(argv, cwd=ROOT, env={**os.environ, 'HEARTGOLD_TEST_MUTE': '1'}, capture_output=True, text=True, timeout=180)
            code, output = result.returncode, result.stdout + result.stderr
        except subprocess.TimeoutExpired as error:
            code, output = None, (error.stdout or b'').decode() + (error.stderr or b'').decode()
        (destination / 'runtime.log').write_text(output)
        report['evidence'] = [json.loads(line.removeprefix('Battle menus evidence: ')) for line in output.splitlines() if line.startswith('Battle menus evidence: ')]
        report['infoEvidence'] = [json.loads(line.removeprefix('Battle menus info evidence: ')) for line in output.splitlines() if line.startswith('Battle menus info evidence: ')]
        report['infoCrops'] = [json.loads(line.removeprefix('Battle menus info crop: ')) for line in output.splitlines() if line.startswith('Battle menus info crop: ')]
        report.update(runtimeComplete=code is not None, complete=code is not None and len(report['evidence']) == 1 and bool(report['evidence'][0]['flowComplete']),
                      exitCode=code, status=report['evidence'][0]['status'] if len(report['evidence']) == 1 else 'wall_clock_timeout' if code is None else 'native_runtime_failure')
        report['passed'] = (code == 0 and len(report['evidence']) == 1 and report['status'] == 'passed_original_battle_menus_and_heal'
                            and len(report['infoEvidence']) == 1 and report['infoEvidence'][0]['complete'] == 1
                            and report['infoEvidence'][0]['pagesMask'] == 28 and report['infoEvidence'][0]['availableMoveMask'] == 3
                            and report['infoEvidence'][0]['staleRejected'] == 8 and report['infoEvidence'][0]['descriptionBankRoundTrip'] == 1)
    except Exception as error:
        report.update(passed=False, status='build_or_integrity_failure', error=str(error))
    finally:
        report['fixtureUnchanged'] = hashes(fixture) == report['fixtureBefore']
        report['privateSaveHashesAfter'] = hashes(path for path in destination.glob('scenario.sav*') if path.is_file())
        report['artifactHashesUnchanged'] = hashes(artifacts) == artifacts
        report['hostInputsUnchanged'] = 'hostInputsBefore' in report and hashes(report['hostInputsBefore']) == report['hostInputsBefore']
        report['gitMetadataUnchanged'] = 'gitMetadataBefore' in report and hashes(report['gitMetadataBefore']) == report['gitMetadataBefore']
        report['sealsAfter'] = {name: hashes(rows) == rows for name, rows in preparation.get('sealsBefore', {}).items()}
        report['passed'] &= report['fixtureUnchanged'] and report['artifactHashesUnchanged'] and report['hostInputsUnchanged'] and report['gitMetadataUnchanged'] and all(report['sealsAfter'].values())
        report['capturedFiles'] = hashes(path for path in (destination / 'frames').iterdir() if path.is_file())
        report_file.write_text(json.dumps(report, indent=2) + '\n')
        (destination / 'archive-seals.json').write_text(json.dumps({str(path.relative_to(destination)): sha(path) for path in sorted(destination.rglob('*')) if path.is_file()}, indent=2) + '\n')
    print(('PASS: ' if report['passed'] else 'UNVERIFIED: ') + str(report_file))
    return 0 if report['passed'] else 1


if __name__ == '__main__':
    raise SystemExit(main())
