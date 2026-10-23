# SPDX-License-Identifier: GPL-3.0-or-later
"""Check two original wild encounters, their settlements and OVY12 menu revision lifetime."""
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
from check_abi32 import ROOT, BUILD, SDK, WABT, native_build_settings
from check_native_engine import generate_engine
from check_native_rainy_field import INCLUDES
from import_original_content import read_boot_header
from prepare_native_content import verify


def sha(path):
    with path.open('rb') as stream:
        return hashlib.file_digest(stream, 'sha256').hexdigest()


def hashes(paths):
    return {str(path): sha(path) for path in sorted(set(paths))}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--save', required=True, help='Designated private Route29 fixture; only fresh copies are opened')
    parser.add_argument('--output-name', required=True, help='New native-battle-reload- archive basename; never overwritten')
    args = parser.parse_args()
    if not re.fullmatch(r'native-battle-reload-[a-z0-9]+(?:-[a-z0-9]+)*', args.output_name):
        parser.error('--output-name must be one lowercase native-battle-reload- basename containing only letters, digits and hyphens')
    save_source = Path(args.save).resolve()
    if save_source != (ROOT.parent / 'native-ios-route29-save/heartgold.sav').resolve() or not save_source.is_file():
        raise ValueError('Use only the designated private Route29 fixture')
    if sha(save_source) != '61fd40d5247db2c6a9b1c8d4a398038bb79f858733090a5352af1c633713198a':
        raise RuntimeError('Private Route29 prerequisite fixture changed')
    fixture = sorted(path for path in save_source.parent.glob(save_source.name + '*') if path.is_file())
    if {path.name for path in fixture} != {save_source.name + suffix for suffix in ('', '.console', '.wifi', '.lock')}:
        raise RuntimeError('Expected only the private save and its three original companions')
    audit = BUILD / 'audit'
    link = json.loads((audit / 'engine-link.json').read_text())
    audited = json.loads((audit / 'report.json').read_text())
    if link['exitCode'] or sha(audit / 'engine.wasm') != link['outputSha256']:
        raise RuntimeError('A verified full engine audit is required')
    pack = verify(link.get('nativeContent'))
    include_paths = [ROOT / name for name in INCLUDES]
    if link.get('nativeContent'):
        include_paths.insert(0, ROOT / link['nativeContent']['include'])
    headers = {path for folder in include_paths for pattern in ('**/*.h', '**/*.naix') for path in folder.glob(pattern)}
    headers |= set((ROOT / 'lib/MSL_C/src').glob('*.h')) | set((ROOT / 'lib/MSL_C/src').glob('*.hpp'))
    if digest(headers) != audited['headersSha256']:
        raise RuntimeError('Headers differ from the full audit')
    units = {row['file']: row for row in audited['units']}
    sources = []
    for name, unit in units.items():
        path = ROOT / name
        if not unit['exitCode'] and path.is_file():
            if sha(path) != unit['sha256']:
                raise RuntimeError('Source differs from its full audit: ' + name)
            sources.append(path)
    field_source = ROOT / 'src/field_system.c'
    if units['src/field_system.c']['exitCode'] or units['src/field_system.c']['overlayStates']:
        raise RuntimeError('QA must be injected only into the verified resident field object')
    command = link['command'].copy()
    original_rsp = next(value for value in command if value.startswith('@'))
    objects = shlex.split(Path(original_rsp[1:]).read_text())
    original_field = str(audit / 'objects/src/field_system.o')
    if objects.count(original_field) != 1:
        raise RuntimeError('Expected one formal field object')
    cached = [Path(path) for path in objects] + [Path(path) for path in command if path.endswith(('.o', '.a'))]
    cached += [audit / 'engine.wasm', Path(original_rsp[1:]), audit / 'engine-link.json', audit / 'report.json', pack / 'manifest.json']
    if link.get('nativeContent'):
        cached.append(ROOT / link['nativeContent']['report'])
    destination = ROOT.parent / args.output_name
    if destination.exists():
        raise RuntimeError('Preserve existing evidence: ' + str(destination))
    host_flags, runtime = native_build_settings(destination)
    host_headers = set((ROOT / 'ios/Original').rglob('*.h')) | set(runtime[0].parent.glob('*.h')) | set((WABT / 'include').rglob('*.h'))
    owned = [ROOT / 'ios/tools/check_native_battle_reload.py', ROOT / 'ios/Original/abi32/battle_reload_scenario_check.c']
    tracked = [*sources, *headers, *host_headers, *runtime, *owned, ROOT / 'ios/Original/abi32/engine_check.c',
               *sorted((ROOT / 'ios/tools').glob('*.py'))]
    seals = dict(fixture=hashes(fixture), sourcesAndHeaders=hashes(tracked), sharedCache=hashes(cached))
    head = subprocess.check_output(['git', 'rev-parse', 'HEAD'], cwd=ROOT, text=True).strip()
    status = subprocess.check_output(['git', 'status', '--porcelain'], cwd=ROOT, text=True)
    destination.mkdir()
    (destination / 'frames').mkdir()
    for path in [*owned, field_source, ROOT / 'src/battle/battle_input.c', ROOT / 'src/battle/battle_setup.c', ROOT / 'include/native_battle_ui.h']:
        copy = destination / 'sources' / path.relative_to(ROOT)
        copy.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy2(path, copy)
    report = dict(scope='one native instance/NitroMain; two original Lv1 Caterpie encounters, Run index3, real OVY12 loads/unloads, settlements and field returns; no Pokémon/stat/outcome mutation, emulator, UIKit or audible-output claim',
                  complete=False, passed=False, expectedNegative=False, status='building', head=head, gitStatusBefore=status,
                  productionModuleSha256=link['outputSha256'], headersSha256=audited['headersSha256'], sealsBefore=seals, commands=[])
    report_file = destination / 'report.json'
    report_file.write_text(json.dumps(report, indent=2) + '\n')
    (destination / 'preparation.json').write_text(json.dumps(report, indent=2) + '\n')

    def run(argv, log):
        argv = list(map(str, argv))
        report['commands'].append(argv)
        result = subprocess.run(argv, cwd=ROOT, capture_output=True, text=True, timeout=240)
        (destination / log).write_text(result.stdout + result.stderr)
        if result.returncode:
            raise RuntimeError('Battle reload build failed: ' + str(destination / log))

    try:
        anchor = 'BOOL finished = FieldSystem_Main(fieldSystem);'
        field_code = field_source.read_text()
        if field_code.count(anchor) != 1:
            raise RuntimeError('Original field owner anchor changed')
        field = destination / 'field-scenario.c'
        field.write_text('#define HG_BATTLE_RELOAD_QA_FIELD\n#include "' + str(owned[1]) + '"\n' +
                         field_code.replace(anchor, anchor + '\n    BattleReloadQAFieldTick(fieldSystem);'))
        flags = ['--target=wasm32-wasip1', '-std=gnu11', '-O2', '-fwrapv', '-fno-strict-aliasing',
                 '-fno-builtin-malloc', '-fno-builtin-free', '-fwasm-exceptions', '-fexceptions', '-Xclang', '-disable-red-zone',
                 '-fms-extensions', '-Wno-microsoft', '-Wno-unknown-pragmas']
        flags += ['-D' + name for name in ('HEARTGOLD_NATIVE', 'HEARTGOLD_ABI32', 'SDK_ARM9', 'SDK_CW', 'SDK_CODE_ARM',
                                         'SDK_TS', '_NITRO', 'PM_KEEP_ASSERTS', 'PLATFORM_INTRINSIC_FUNCTION_BIT_CLZ32=__builtin_clz')]
        for path in include_paths:
            flags += ['-I', path]
        if link.get('nativeContent'):
            flags += ['-DHG_NATIVE_ROM_TABLES']
        field_object = destination / 'field-scenario.o'
        run([SDK / 'bin/clang', *flags, '-c', field, '-o', field_object], 'field-compile.log')
        objects[objects.index(original_field)] = str(field_object)
        listing = destination / 'objects.rsp'
        listing.write_text('\n'.join('"' + path + '"' for path in objects) + '\n')
        command[command.index(original_rsp)] = '@' + str(listing)
        module = destination / 'engine.wasm'
        command[command.index('-o') + 1] = str(module)
        for name in ('HGNativeBattleReloadQAGetState', 'HGNativeBattleReloadQARequest'):
            command.insert(command.index('-o'), '-Wl,--export=' + name)
        for name in ('HandleLoadOverlay', 'UnloadOverlayByID', 'Battle_LaunchApp'):
            command.insert(command.index('-o'), '-Wl,--wrap=' + name)
        run(command, 'strict-link.log')
        generated, writes, restores = generate_engine(destination, module, run)
        host_flags += ['-g', '-fsanitize=address,undefined', '-fno-sanitize-recover=all']

        def compile_one(item):
            index, path = item
            target = destination / f'host-{index}.o'
            extra = ['-DWASM_RT_TRAP_HANDLER=HGEngineTrap'] if path.name == 'wasm-rt-impl.c' else []
            run(['xcrun', 'clang', *host_flags, *extra, '-c', path, '-o', target], f'compile-{index}.log')
            return target

        with ThreadPoolExecutor(max_workers=4) as pool:
            native = list(pool.map(compile_one, enumerate([*generated, *runtime, owned[1]])))
        binary = destination / 'battle-reload-check'
        run(['xcrun', 'clang', '-fsanitize=address,undefined', *native, '-o', binary], 'native-link.log')
        artifacts = [field, field_object, listing, module, *generated, destination / 'hgengine.h', destination / 'hgengine-impl.h', *native, binary]
        artifact_hashes = hashes(artifacts)
        (destination / 'private-build-hashes.json').write_text(json.dumps(artifact_hashes, indent=2) + '\n')
        if any(hashes(Path(path) for path in values) != values for values in seals.values()):
            raise RuntimeError('Protected inputs changed during build')
        if digest(headers) != audited['headersSha256'] or status != subprocess.check_output(['git', 'status', '--porcelain'], cwd=ROOT, text=True):
            raise RuntimeError('Sources or headers changed during build; never run a mixed result')
        for path in fixture:
            shutil.copy2(path, destination / ('scenario.sav' + path.name[len(save_source.name):]))
        boot = destination / 'boot-header.bin'
        boot.write_bytes(read_boot_header(pack))
        argv = list(map(str, [binary, pack / 'files', boot, destination / 'scenario.sav', destination / 'frames']))
        report['commands'].append(argv)
        report.update(status='running', scenarioModuleSha256=sha(module), nativeBinarySha256=sha(binary),
                      artifactHashes=artifact_hashes, checkedStackWrites=writes, exceptionDepthRestores=restores)
        report_file.write_text(json.dumps(report, indent=2) + '\n')
        try:
            result = subprocess.run(argv, cwd=ROOT, env={**os.environ, 'HEARTGOLD_TEST_MUTE': '1'}, capture_output=True, text=True, timeout=180)
            exit_code, output = result.returncode, result.stdout + result.stderr
        except subprocess.TimeoutExpired as error:
            exit_code, output = None, (error.stdout or b'').decode() + (error.stderr or b'').decode()
        (destination / 'runtime.log').write_text(output)
        for name, prefix in [('evidence', 'Battle reload evidence: '), ('prerequisites', 'Battle reload prerequisite evidence: '),
                             ('commandsEvidence', 'Battle reload command evidence: '), ('framesEvidence', 'Battle reload frame evidence: ')]:
            report[name] = [json.loads(line[len(prefix):]) for line in output.splitlines() if line.startswith(prefix)]
        evidence = report['evidence']
        report.update(complete=exit_code is not None, exitCode=exit_code)
        report['status'] = evidence[0]['status'] if len(evidence) == 1 else 'wall_clock_timeout' if exit_code is None else 'native_runtime_failure'
        report['passed'] = exit_code == 0 and len(evidence) == 1 and report['status'] == 'passed_original_two_wild_reload'
        report['expectedNegative'] = exit_code == 3 and len(evidence) == 1 and report['status'] == 'expected_negative_revision_reuse'
        report['generatedArtifactsUnchanged'] = hashes(artifacts) == artifact_hashes
        report['capturedFiles'] = hashes(path for path in (destination / 'frames').iterdir() if path.is_file())
        report['privateSaveHashesAfter'] = hashes(path for path in destination.glob('scenario.sav*') if path.is_file())
    except Exception as error:
        report.update(passed=False, expectedNegative=False, status='build_or_integrity_failure', error=str(error))
    finally:
        changed = {name: [path for path, expected in values.items() if not Path(path).is_file() or sha(Path(path)) != expected] for name, values in seals.items()}
        report['sealsAfter'] = {name: not paths for name, paths in changed.items()}
        report['changedPaths'] = {name: paths for name, paths in changed.items() if paths}
        report['headersUnchanged'] = digest(headers) == audited['headersSha256']
        report['gitStateUnchanged'] = head == subprocess.check_output(['git', 'rev-parse', 'HEAD'], cwd=ROOT, text=True).strip() and status == subprocess.check_output(['git', 'status', '--porcelain'], cwd=ROOT, text=True)
        intact = all(report['sealsAfter'].values()) and report['headersUnchanged'] and report['gitStateUnchanged'] and report.get('generatedArtifactsUnchanged', False)
        report['passed'] &= intact
        report['expectedNegative'] &= intact
        report_file.write_text(json.dumps(report, indent=2) + '\n')
        archive = {str(path.relative_to(destination)): sha(path) for path in sorted(destination.rglob('*')) if path.is_file()}
        (destination / 'archive-seals.json').write_text(json.dumps(archive, indent=2) + '\n')
    print(('PASS' if report['passed'] else 'EXPECTED NEGATIVE' if report['expectedNegative'] else 'UNVERIFIED') + ': ' + str(report_file))
    return 0 if report['passed'] else 3 if report['expectedNegative'] else 1


if __name__ == '__main__':
    raise SystemExit(main())
