# SPDX-License-Identifier: GPL-3.0-or-later
"""Check the original field Party/Item/Quit flow using one isolated Continue save."""
import argparse
from concurrent.futures import ThreadPoolExecutor
import hashlib
import json
from pathlib import Path
import re
import shlex
import shutil
import subprocess
import os

from audit_original import digest
from check_abi32 import ROOT, BUILD, WABT, native_build_settings
from check_native_engine import generate_engine
from check_native_rainy_field import INCLUDES
from import_original_content import read_boot_header
from prepare_native_content import verify


def sha(path):
    with path.open('rb') as stream:
        return hashlib.file_digest(stream, 'sha256').hexdigest()


def hashes(paths):
    return {str(path): sha(path) for path in sorted(set(paths))}


def field_hitboxes():
    source = (ROOT / 'asm/touchscreen_menu_data.s').read_text()
    block = source.split('gTouchscreenMenuIconHitboxes:', 1)[1].split('gTouchscreenMenuIcons:', 1)[0]
    values = [int(value.strip(), 0) for line in re.findall(r'(?m)^\s*\.byte\s+([^\n]+)', block) for value in line.split(',')]
    if len(values) != 44 or values[-4:] != [255, 0, 0, 0]:
        raise RuntimeError('Original field menu hitbox table changed')
    boxes = [values[i:i + 4] for i in range(0, 40, 4)]
    if any(not (top < bottom <= 192 and left < right <= 256) for top, bottom, left, right in boxes):
        raise RuntimeError('Invalid original field menu hitboxes')
    return [((left + right) // 2, (top + bottom) // 2) for top, bottom, left, right in boxes[1:8]]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--save', required=True, help='Designated private Route29 fixture; only new copies are opened')
    parser.add_argument('--give-take', action='store_true', help='Original Give/Cancel, Potion Give and Take with full restoration')
    parser.add_argument('--output-name', help='New private archive basename; existing output is never overwritten')
    args = parser.parse_args()
    if args.output_name is None:
        args.output_name = 'native-party-menu-give-take-pass' if args.give_take else 'native-party-menu-pass'
    if not re.fullmatch(r'native-party-menu-[a-z0-9]+(?:-[a-z0-9]+)*', args.output_name):
        parser.error('--output-name must be one lowercase native-party-menu- basename')
    save_source = Path(args.save).resolve()
    if save_source != (ROOT.parent / 'native-ios-route29-save/heartgold.sav').resolve() or not save_source.is_file():
        raise ValueError('Use only the designated private Route29 fixture')
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
        raise RuntimeError('Headers differ from the full audit; never mix old Party objects with the new layout')
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
                 'src/party_menu_items.c', 'src/party_menu_sprites.c', 'src/overlay_94.c']
    if args.give_take:
        consumers += ['src/party.c', 'src/bag.c', 'src/application/bag/bag_app.c', 'src/application/bag/bag_app_display.c']
    for name in consumers:
        if name not in units or units[name]['exitCode']:
            raise RuntimeError('Party layout consumer was not freshly audited: ' + name)
    command = link['command'].copy()
    original_rsp = next(value for value in command if value.startswith('@'))
    objects = shlex.split(Path(original_rsp[1:]).read_text())
    for name in consumers:
        if objects.count(str(audit / 'objects' / Path(name).with_suffix('.o'))) != 1:
            raise RuntimeError('Expected one formal Party layout object: ' + name)
    cached = [Path(path) for path in objects] + [Path(path) for path in command if path.endswith(('.o', '.a'))]
    cached += [audit / 'engine.wasm', Path(original_rsp[1:]), audit / 'engine-link.json', audit / 'report.json', pack / 'manifest.json']
    if link.get('nativeContent'):
        cached.append(ROOT / link['nativeContent']['report'])
    destination = ROOT.parent / args.output_name
    if destination.exists():
        raise RuntimeError('Preserve the existing Party archive before another run: ' + str(destination))
    host_flags, runtime = native_build_settings(destination)
    host_headers = set((ROOT / 'ios/Original').rglob('*.h')) | set(runtime[0].parent.glob('*.h')) | set((WABT / 'include').rglob('*.h'))
    owned = [ROOT / 'ios/tools/check_native_party_menu.py', ROOT / 'ios/Original/abi32/party_menu_scenario_check.c']
    tracked = [*audited_sources, *headers, *host_headers, *runtime, *owned, ROOT / 'ios/Original/abi32/engine_check.c',
               ROOT / 'asm/touchscreen_menu_data.s', *sorted((ROOT / 'ios/tools').glob('*.py'))]
    if args.give_take:
        tracked += [ROOT / 'ios/OriginalApp/main.m', ROOT / 'ios/tests/check_original_app.inc', ROOT / 'asm/bag_app_data.s']
    old_names = list(json.loads((ROOT.parent / 'native-field-maps-batch-01/preparation.json').read_text())['archiveSeals'])
    old_names += ['native-field-maps-batch-01', 'native-field-maps-batch-01-resume']
    old_files = []
    for name in old_names:
        folder = ROOT.parent / name
        if not folder.is_dir():
            raise RuntimeError('Previous evidence archive is missing: ' + str(folder))
        old_files += [path for path in folder.rglob('*') if path.is_file()]
    fixture = sorted(path for path in save_source.parent.glob(save_source.name + '*') if path.is_file())
    if {path.name for path in fixture} != {save_source.name + suffix for suffix in ('', '.console', '.wifi', '.lock')}:
        raise RuntimeError('Expected the original private save and its three companions')
    if args.give_take and sha(save_source) != '61fd40d5247db2c6a9b1c8d4a398038bb79f858733090a5352af1c633713198a':
        raise RuntimeError('The unchanged designated Route29 fixture is required')
    seals = dict(fixture=hashes(fixture), sourcesAndHeaders=hashes(tracked), sharedCache=hashes(cached), previousArchives=hashes(old_files))
    head = subprocess.check_output(['git', 'rev-parse', 'HEAD'], cwd=ROOT, text=True).strip()
    git_status = subprocess.check_output(['git', 'status', '--porcelain'], cwd=ROOT, text=True)
    centers = field_hitboxes()
    destination.mkdir()
    (destination / 'frames').mkdir()
    for path in [*owned, ROOT / 'include/party_menu.h', ROOT / 'include/native_party_ui.h', ROOT / 'asm/touchscreen_menu_data.s']:
        copy = destination / 'sources' / path.relative_to(ROOT)
        copy.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy2(path, copy)
    if args.give_take:
        for path in [ROOT / 'include/native_bag_ui.h', ROOT / 'src/start_menu.c', ROOT / 'src/bag.c',
                     ROOT / 'src/party_menu.c', ROOT / 'src/party_menu_list_items.c', ROOT / 'src/application/bag/bag_app.c']:
            copy = destination / 'sources' / path.relative_to(ROOT)
            copy.parent.mkdir(parents=True, exist_ok=True); shutil.copy2(path, copy)
    report = dict(scope='original Continue and field touchscreen Party roster/context/Item/Quit; original metadata, tiled 4bpp windows and two engines; no Summary, Give, story mutation or UIKit/device claim',
                  complete=False, passed=False, status='building', head=head, gitStatusBefore=git_status,
                  productionModuleSha256=link['outputSha256'], headersSha256=audit_report['headersSha256'],
                  partyLayoutConsumers=consumers, fieldIconCenters=centers, sealsBefore=seals, commands=[])
    if args.give_take:
        report.update(giveTake=True, scope='original field Party Give Cancel, real Potion Give/Take, original printer wait/Continue, restored 26 mon attributes and full Bag; no item injection, unsupported swap/mail/forms or UIKit/device claim')
    report_file = destination / 'report.json'
    report_file.write_text(json.dumps(report, indent=2) + '\n')
    (destination / 'preparation.json').write_text(json.dumps(report, indent=2) + '\n')

    def run(argv, log):
        argv = list(map(str, argv))
        report['commands'].append(argv)
        result = subprocess.run(argv, cwd=ROOT, capture_output=True, text=True, timeout=240)
        (destination / log).write_text(result.stdout + result.stderr)
        if result.returncode:
            raise RuntimeError('Party menu build failed: ' + str(destination / log))

    try:
        # Link the new formal objects unchanged; the private module adds only existing read-only getters.
        listing = destination / 'objects.rsp'
        listing.write_text('\n'.join('"' + path + '"' for path in objects) + '\n')
        command[command.index(original_rsp)] = '@' + str(listing)
        module = destination / 'engine.wasm'
        command[command.index('-o') + 1] = str(module)
        for name in ('HGNativePartyUIGetState', 'SaveData_Get', 'Save_VarsFlags_Get', 'CheckGotStarter', 'CheckGotPokedex',
                     'SaveArray_Party_Get', 'Party_GetCount', 'Party_GetMonByIndex', 'GetMonData',
                     'Save_Pokedex_Get', 'Pokedex_IsEnabled', 'Pokedex_CountNationalDexSeen', 'Pokedex_CountNationalDexOwned',
                     'Save_Bag_Get', 'Save_Bag_sizeof'):
            flag = '-Wl,--export=' + name
            if flag not in command:
                command.insert(command.index('-o'), flag)
        if args.give_take:
            for name in ('HGNativeBagUIGetState', 'Bag_GetQuantity'):
                flag = '-Wl,--export=' + name
                if flag not in command: command.insert(command.index('-o'), flag)
        run(command, 'strict-link.log')
        generated, writes, restores = generate_engine(destination, module, run)
        host_flags += ['-g', '-fsanitize=address,undefined', '-fno-sanitize-recover=all']
        if args.give_take:
            host_flags += ['-DHG_FIELD_GIVE_TAKE_QA']

        def compile_one(item):
            index, path = item
            target = destination / f'host-{index}.o'
            extra = ['-DWASM_RT_TRAP_HANDLER=HGEngineTrap'] if path.name == 'wasm-rt-impl.c' else []
            run(['xcrun', 'clang', *host_flags, *extra, '-c', path, '-o', target], f'compile-{index}.log')
            return target

        with ThreadPoolExecutor(max_workers=4) as pool:
            native = list(pool.map(compile_one, enumerate([*generated, *runtime, owned[1]])))
        binary = destination / 'party-menu-check'
        run(['xcrun', 'clang', '-fsanitize=address,undefined', *native, '-o', binary], 'native-link.log')
        artifacts = [*generated, destination / 'hgengine.h', destination / 'hgengine-impl.h', *native, module, binary, listing]
        artifact_hashes = hashes(artifacts)
        (destination / 'private-build-hashes.json').write_text(json.dumps(artifact_hashes, indent=2) + '\n')
        for name, values in seals.items():
            if any(not Path(path).is_file() or sha(Path(path)) != expected for path, expected in values.items()):
                raise RuntimeError('Protected inputs changed during build: ' + name)
        if digest(headers) != audit_report['headersSha256'] or git_status != subprocess.check_output(['git', 'status', '--porcelain'], cwd=ROOT, text=True):
            raise RuntimeError('Sources or headers changed during build; do not run the mixed result')
        for path in fixture:
            shutil.copy2(path, destination / ('scenario.sav' + path.name[len(save_source.name):]))
        if args.give_take:
            report['privateSaveHashesBefore'] = hashes(path for path in destination.glob('scenario.sav*') if path.is_file())
        boot = destination / 'boot-header.bin'
        boot.write_bytes(read_boot_header(pack))
        argv = list(map(str, [binary, pack / 'files', boot, destination / 'scenario.sav', destination / 'frames',
                              *(coordinate for center in centers for coordinate in center)]))
        report['commands'].append(argv)
        report.update(status='running', scenarioModuleSha256=sha(module), nativeBinarySha256=sha(binary),
                      artifactHashes=artifact_hashes, checkedStackWrites=writes, exceptionDepthRestores=restores)
        report_file.write_text(json.dumps(report, indent=2) + '\n')
        try:
            result = subprocess.run(argv, cwd=ROOT, env={**os.environ, 'HEARTGOLD_TEST_MUTE': '1'}, capture_output=True, text=True, timeout=120)
            exit_code, output = result.returncode, result.stdout + result.stderr
        except subprocess.TimeoutExpired as error:
            exit_code, output = None, (error.stdout or b'').decode() + (error.stderr or b'').decode()
        (destination / 'runtime.log').write_text(output)
        evidence = [json.loads(line.split('Party menu evidence: ', 1)[1]) for line in output.splitlines() if line.startswith('Party menu evidence: ')]
        report.update(complete=exit_code is not None, exitCode=exit_code, evidence=evidence,
                      prerequisites=[json.loads(line.split('Party prerequisites: ', 1)[1]) for line in output.splitlines() if line.startswith('Party prerequisites: ')],
                      semanticEvidence=[json.loads(line.split('Party semantic evidence: ', 1)[1]) for line in output.splitlines() if line.startswith('Party semantic evidence: ')],
                      snapshotEvidence=[json.loads(line.split('Party snapshot evidence: ', 1)[1]) for line in output.splitlines() if line.startswith('Party snapshot evidence: ')],
                      actionEvidence=[json.loads(line.split('Party action evidence: ', 1)[1]) for line in output.splitlines() if line.startswith('Party action evidence: ')],
                      windowEvidence=[json.loads(line.split('Party window evidence: ', 1)[1]) for line in output.splitlines() if line.startswith('Party window evidence: ')],
                      frameEvidence=[json.loads(line.split('Party frame evidence: ', 1)[1]) for line in output.splitlines() if line.startswith('Party frame evidence: ')])
        report['status'] = evidence[0]['status'] if len(evidence) == 1 else 'wall_clock_timeout' if exit_code is None else 'native_assertion' if 'Party menu failure line ' in output else 'native_runtime_failure'
        report['passed'] = exit_code == 0 and len(evidence) == 1 and evidence[0]['status'] == 'passed_original_party_menu' and 'PASS: original Party roster' in output
        if args.give_take:
            report['giveTakeEvidence'] = [json.loads(line.split('Party give-take evidence: ', 1)[1]) for line in output.splitlines() if line.startswith('Party give-take evidence: ')]
            report['giveTakePrerequisites'] = [json.loads(line.split('Party give-take prerequisites: ', 1)[1]) for line in output.splitlines() if line.startswith('Party give-take prerequisites: ')]
            report['giveTakeSemanticEvidence'] = [json.loads(line.split('Party give-take semantic evidence: ', 1)[1]) for line in output.splitlines() if line.startswith('Party give-take semantic evidence: ')]
            report['bagSnapshotEvidence'] = [json.loads(line.split('Party Bag snapshot evidence: ', 1)[1]) for line in output.splitlines() if line.startswith('Party Bag snapshot evidence: ')]
            report['continueEvidence'] = [json.loads(line.split('Party message Continue evidence: ', 1)[1]) for line in output.splitlines() if line.startswith('Party message Continue evidence: ')]
            flow = report['giveTakeEvidence'][0] if len(report['giveTakeEvidence']) == 1 else {}
            report['passed'] = (exit_code == 0 and len(evidence) == 1 and evidence[0]['status'] == 'passed_original_field_give_take'
                                and flow.get('giveCancelled') == 1 and flow.get('givenHeldItem') == 17 and flow.get('givenBagQuantity') == 4
                                and flow.get('restoredHeldItem') == 0 and flow.get('restoredBagQuantity') == 5
                                and flow.get('giveMessageCaptured') == 1 and flow.get('takeMessageCaptured') == 1
                                and flow.get('giveApplied') == 1 and flow.get('takeRestored') == 1
                                and flow.get('giveContinue') == 1 and flow.get('takeContinue') == 1
                                and flow.get('givePrintingBlocked', 0) > 0 and flow.get('takePrintingBlocked', 0) > 0
                                and flow.get('giveWaitPolls', 0) >= 2 and flow.get('takeWaitPolls', 0) >= 2
                                and flow.get('partyNewInstanceRejected', 0) >= 2 and flow.get('bagNewInstanceRejected') == 1
                                and flow.get('fullBaselineRestored') == 1 and flow.get('staleRejected', 0) >= 4
                                and flow.get('partyActiveAfterReturn') == 0 and flow.get('bagActiveAfterReturn') == 0
                                and flow.get('resetFinished') is True and flow.get('pendingDMA') == 0
                                and evidence[0].get('partyItemsFlagsUnchanged') is True
                                and len(report['continueEvidence']) == 2
                                and all(row.get('originalWait') is True and row.get('holdFrames') == 4 for row in report['continueEvidence']))
        report['generatedArtifactsUnchanged'] = hashes(artifacts) == artifact_hashes
        report['passed'] &= report['generatedArtifactsUnchanged']
        report['capturedFiles'] = hashes(path for path in (destination / 'frames').iterdir() if path.is_file())
        report['privateSaveHashesAfter'] = hashes(path for path in destination.glob('scenario.sav*') if path.is_file())
        if args.give_take:
            report['privateCopiesUnchanged'] = len(report['privateSaveHashesAfter']) == 4 and report['privateSaveHashesAfter'] == report['privateSaveHashesBefore']
            report['passed'] &= report['privateCopiesUnchanged']
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
