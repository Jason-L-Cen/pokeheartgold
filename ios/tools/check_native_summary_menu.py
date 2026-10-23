# SPDX-License-Identifier: GPL-3.0-or-later
"""Check original field Party/Summary browsing without changing the private Continue save."""
import argparse
from concurrent.futures import ThreadPoolExecutor
import json
from pathlib import Path
import re
import shlex
import shutil
import subprocess
import os

from audit_original import digest
from check_abi32 import ROOT, BUILD, WABT, native_build_settings
from check_native_bag_menu import field_hitboxes, hashes, sha
from check_native_engine import generate_engine
from check_native_rainy_field import INCLUDES
from import_original_content import read_boot_header
from prepare_native_content import verify


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--save', required=True, help='Designated private Route29 fixture; only new copies are opened')
    parser.add_argument('--output-name', default='native-summary-menu-pass', help='New private archive basename; existing output is never overwritten')
    args = parser.parse_args()
    if not re.fullmatch(r'native-summary-menu-[a-z0-9]+(?:-[a-z0-9]+)*', args.output_name):
        parser.error('--output-name must be one lowercase native-summary-menu- basename containing only letters, digits and hyphens')
    save_source = Path(args.save).resolve()
    if save_source != (ROOT.parent / 'native-ios-route29-save/heartgold.sav').resolve() or not save_source.is_file():
        raise ValueError('Use only the designated private Route29 fixture')
    if sha(save_source) != '61fd40d5247db2c6a9b1c8d4a398038bb79f858733090a5352af1c633713198a':
        raise RuntimeError('The designated private fixture changed')
    destination = ROOT.parent / args.output_name
    if destination.exists():
        raise RuntimeError('Preserve the existing Summary archive before another run: ' + str(destination))
    audit = BUILD / 'audit'
    link = json.loads((audit / 'engine-link.json').read_text())
    audit_report = json.loads((audit / 'report.json').read_text())
    if link['exitCode'] or sha(audit / 'engine.wasm') != link['outputSha256']:
        raise RuntimeError('A fresh strict full-engine audit is required')
    pack = verify(link.get('nativeContent'))
    include_paths = [ROOT / path for path in INCLUDES]
    if link.get('nativeContent'):
        include_paths.insert(0, ROOT / link['nativeContent']['include'])
    headers = {path for folder in include_paths for pattern in ('**/*.h', '**/*.naix') for path in folder.glob(pattern)}
    headers |= set((ROOT / 'lib/MSL_C/src').glob('*.h')) | set((ROOT / 'lib/MSL_C/src').glob('*.hpp'))
    if digest(headers) != audit_report['headersSha256']:
        raise RuntimeError('Headers differ from the full audit; never mix old Party/Summary objects with new headers')
    if not re.search(r'Window\s+contextMenuButtonWindows\[12\]', (ROOT / 'include/party_menu.h').read_text()):
        raise RuntimeError('All twelve original context window slots are required')
    units = {row['file']: row for row in audit_report['units']}
    audited_sources = []
    for name, unit in units.items():
        path = ROOT / name
        if not unit['exitCode'] and path.is_file():
            if sha(path) != unit['sha256']:
                raise RuntimeError('Source differs from its full audit: ' + name)
            audited_sources.append(path)
    consumers = ['src/party_menu.c', 'src/party_context_menu.c', 'src/party_menu_list_items.c',
                 'src/party_menu_items.c', 'src/party_menu_sprites.c', 'src/overlay_94.c', 'src/pokemon_summary.c']
    for name in consumers:
        if name not in units or units[name]['exitCode']:
            raise RuntimeError('Party/Summary consumer was not freshly audited: ' + name)
    command = link['command'].copy()
    original_rsp = next(value for value in command if value.startswith('@'))
    objects = shlex.split(Path(original_rsp[1:]).read_text())
    for name in consumers:
        if objects.count(str(audit / 'objects' / Path(name).with_suffix('.o'))) != 1:
            raise RuntimeError('Expected one formal Party/Summary object: ' + name)
    cached = [Path(path) for path in objects] + [Path(path) for path in command if path.endswith(('.o', '.a'))]
    cached += [audit / 'engine.wasm', Path(original_rsp[1:]), audit / 'engine-link.json', audit / 'report.json', pack / 'manifest.json']
    if link.get('nativeContent'):
        cached.append(ROOT / link['nativeContent']['report'])
    host_flags, runtime = native_build_settings(destination)
    host_headers = set((ROOT / 'ios/Original').rglob('*.h')) | set(runtime[0].parent.glob('*.h')) | set((WABT / 'include').rglob('*.h'))
    owned = [ROOT / 'ios/tools/check_native_summary_menu.py', ROOT / 'ios/Original/abi32/summary_menu_scenario_check.c',
             ROOT / 'ios/Original/tests/summary_ui_check.c']
    tracked = [*audited_sources, *headers, *host_headers, *runtime, *owned, ROOT / 'ios/Original/abi32/engine_check.c',
               ROOT / 'ios/OriginalApp/main.m', ROOT / 'ios/tests/check_original_app.inc',
               ROOT / 'asm/touchscreen_menu_data.s', *sorted((ROOT / 'ios/tools').glob('*.py'))]
    previous_names = ['native-party-menu-pass', 'native-bag-menu-pass', 'native-bag-reload-root-cause',
                      'native-bag-menu-resident-cache-pass', 'native-bag-uikit-canvas-fixed-run',
                      'native-bag-uikit-audio-diagnostics-run', 'native-battle-reload-current-negative',
                      'native-battle-reload-current-negative-execution', 'native-battle-reload-resident-cache-pass',
                      'native-battle-resident-cache-app', 'native-battle-resident-cache-iphoneos-app']
    old_files = []
    for name in previous_names:
        folder = ROOT.parent / name
        manifest = folder / 'archive-seals.json'
        values = json.loads(manifest.read_text())
        if any(not (folder / path).is_file() or sha(folder / path) != expected for path, expected in values.items()):
            raise RuntimeError('Previous immutable evidence seal changed: ' + name)
        old_files += [folder / path for path in values] + [manifest]
    fixture = [Path(str(save_source) + suffix) for suffix in ('', '.console', '.wifi', '.lock')]
    if any(not path.is_file() for path in fixture) or set(fixture) != set(save_source.parent.glob(save_source.name + '*')):
        raise RuntimeError('Expected the original private save and its three companions')
    seals = dict(fixture=hashes(fixture), sourcesAndHeaders=hashes(tracked), sharedCache=hashes(cached), previousArchives=hashes(old_files))
    head = subprocess.check_output(['git', 'rev-parse', 'HEAD'], cwd=ROOT, text=True).strip()
    git_status = subprocess.check_output(['git', 'status', '--porcelain'], cwd=ROOT, text=True)
    centers = field_hitboxes()
    destination.mkdir()
    (destination / 'frames').mkdir()
    for path in [*owned, ROOT / 'include/party_menu.h', ROOT / 'include/native_party_ui.h', ROOT / 'include/native_summary_ui.h',
                 ROOT / 'include/pokemon_summary.h', ROOT / 'include/pokemon_summary_internal.h', ROOT / 'asm/touchscreen_menu_data.s']:
        copy = destination / 'sources' / path.relative_to(ROOT)
        copy.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy2(path, copy)
    report = dict(scope='original Continue, field Party Summary action, pages 0/1/2, two move details, Back, new Party/Summary owners and stale rejection; original windows/two engines; no move swap, save mutation or UIKit/device claim',
                  complete=False, passed=False, status='building', head=head, gitStatusBefore=git_status,
                  productionModuleSha256=link['outputSha256'], headersSha256=audit_report['headersSha256'],
                  layoutConsumers=consumers, fieldIconCenters=centers, sealsBefore=seals, commands=[])
    report_file = destination / 'report.json'
    report_file.write_text(json.dumps(report, indent=2) + '\n')
    (destination / 'preparation.json').write_text(json.dumps(report, indent=2) + '\n')

    def run(argv, log):
        argv = list(map(str, argv))
        report['commands'].append(argv)
        result = subprocess.run(argv, cwd=ROOT, capture_output=True, text=True, timeout=240)
        (destination / log).write_text(result.stdout + result.stderr)
        if result.returncode:
            raise RuntimeError('Summary menu build failed: ' + str(destination / log))

    try:
        # Formal objects stay unchanged; the private module adds only existing read-only getters.
        listing = destination / 'objects.rsp'
        listing.write_text('\n'.join('"' + path + '"' for path in objects) + '\n')
        command[command.index(original_rsp)] = '@' + str(listing)
        module = destination / 'engine.wasm'
        command[command.index('-o') + 1] = str(module)
        for name in ('HGNativeSummaryUIGetState', 'HGNativePartyUIGetState', 'SaveData_Get', 'Save_VarsFlags_Get',
                     'CheckGotStarter', 'CheckGotPokedex', 'SaveArray_Party_Get', 'Party_GetCount', 'Party_GetMonByIndex', 'GetMonData',
                     'Save_Pokedex_Get', 'Pokedex_IsEnabled', 'Pokedex_CountNationalDexSeen', 'Pokedex_CountNationalDexOwned',
                     'Save_Bag_Get', 'Save_Bag_sizeof', 'Save_PlayerData_GetProfile', 'PlayerProfile_GetMoney'):
            flag = '-Wl,--export=' + name
            if flag not in command:
                command.insert(command.index('-o'), flag)
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
        binary = destination / 'summary-menu-check'
        run(['xcrun', 'clang', '-fsanitize=address,undefined', *native, '-o', binary], 'native-link.log')
        artifacts = [*generated, destination / 'hgengine.h', destination / 'hgengine-impl.h', *native, module, binary, listing]
        artifact_hashes = hashes(artifacts)
        (destination / 'private-build-hashes.json').write_text(json.dumps(artifact_hashes, indent=2) + '\n')
        for name, values in seals.items():
            if any(not Path(path).is_file() or sha(Path(path)) != expected for path, expected in values.items()):
                raise RuntimeError('Protected inputs changed during build: ' + name)
        if digest(headers) != audit_report['headersSha256'] or git_status != subprocess.check_output(['git', 'status', '--porcelain'], cwd=ROOT, text=True):
            raise RuntimeError('Sources or headers changed during build; do not run the mixed result')
        private_inputs = []
        for path in fixture:
            copy = destination / ('scenario.sav' + path.name[len(save_source.name):])
            shutil.copy2(path, copy)
            private_inputs.append(copy)
        report['privateSaveHashesBefore'] = hashes(private_inputs)
        boot = destination / 'boot-header.bin'
        boot.write_bytes(read_boot_header(pack))
        argv = list(map(str, [binary, pack / 'files', boot, destination / 'scenario.sav', destination / 'frames',
                              *(coordinate for center in centers for coordinate in center)]))
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
        prefixes = dict(evidence='menu', prerequisites='prerequisites', semanticEvidence='semantic evidence',
                        snapshotEvidence='snapshot evidence', actionEvidence='action evidence',
                        windowEvidence='window evidence', frameEvidence='frame evidence', inputEvidence='input evidence')
        for key, label in prefixes.items():
            prefix = 'Summary ' + label + (': ' if label != 'menu' else ' evidence: ')
            report[key] = [json.loads(line[len(prefix):]) for line in output.splitlines() if line.startswith(prefix)]
        evidence = report['evidence']
        report['exitCode'] = exit_code
        report['status'] = evidence[0]['status'] if len(evidence) == 1 else 'wall_clock_timeout' if exit_code is None else 'native_assertion' if 'Summary menu failure line ' in output else 'native_runtime_failure'
        report['complete'] = exit_code == 0 and len(evidence) == 1 and evidence[0]['status'] == 'passed_original_summary_menu'
        report['passed'] = report['complete'] and 'PASS: original Summary pages' in output
        report['generatedArtifactsUnchanged'] = hashes(artifacts) == artifact_hashes
        report['capturedFiles'] = hashes(path for path in (destination / 'frames').iterdir() if path.is_file())
        report['privateSaveHashesAfter'] = hashes(private_inputs)
        report['privateSaveUnchanged'] = report['privateSaveHashesBefore'] == report['privateSaveHashesAfter']
        report['passed'] &= report['generatedArtifactsUnchanged'] and report['privateSaveUnchanged']
    except Exception as error:
        report.update(passed=False, status='build_or_integrity_failure', error=str(error))
    finally:
        changed = {name: [path for path, expected in values.items() if not Path(path).is_file() or sha(Path(path)) != expected]
                   for name, values in seals.items()}
        report['sealsAfter'] = {name: not paths for name, paths in changed.items()}
        report['changedPaths'] = {name: paths for name, paths in changed.items() if paths}
        report['headersUnchanged'] = digest(headers) == audit_report['headersSha256']
        report['gitStateUnchanged'] = head == subprocess.check_output(['git', 'rev-parse', 'HEAD'], cwd=ROOT, text=True).strip() and git_status == subprocess.check_output(['git', 'status', '--porcelain'], cwd=ROOT, text=True)
        report['passed'] &= all(report['sealsAfter'].values()) and report['headersUnchanged'] and report['gitStateUnchanged']
        report_file.write_text(json.dumps(report, indent=2) + '\n')
    print(('PASS' if report['passed'] else 'UNVERIFIED') + ': ' + str(report_file))
    return 0 if report['passed'] else 1


if __name__ == '__main__':
    raise SystemExit(main())
