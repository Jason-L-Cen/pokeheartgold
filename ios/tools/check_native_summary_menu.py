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
from check_abi32 import ROOT, BUILD, SDK, WABT, native_build_settings
from check_native_bag_menu import field_hitboxes, hashes, sha
from check_native_engine import generate_engine
from check_native_rainy_field import INCLUDES
from import_original_content import read_boot_header
from prepare_native_content import verify
from prepare_overlay_state import IDENT, getter_name, instrument_ir


def prepare_tm_guest(destination, run, objects, audit, includes, native_content):
    """Three private source copies observe the unchanged original field owner, printer and TM task."""
    destination.mkdir()
    helper = ROOT / 'ios/tests/summary_learn_guest.inc'
    anchor = 'BOOL finished = FieldSystem_Main(fieldSystem);'
    hook = '\n    SummaryLearnObserve(fieldSystem);'
    flags = ['--target=wasm32-wasip1', '-std=gnu11', '-O2', '-fwrapv', '-fno-strict-aliasing',
             '-fno-builtin-malloc', '-fno-builtin-free', '-fwasm-exceptions', '-fexceptions', '-Xclang', '-disable-red-zone',
             '-fms-extensions', '-Wno-microsoft', '-Wno-unknown-pragmas']
    flags += ['-D' + name for name in ('HEARTGOLD_NATIVE', 'HEARTGOLD_ABI32', 'SDK_ARM9', 'SDK_CW', 'SDK_CODE_ARM',
                                     'SDK_TS', '_NITRO', 'PM_KEEP_ASSERTS', 'PLATFORM_INTRINSIC_FUNCTION_BIT_CLZ32=__builtin_clz')]
    for path in includes:
        flags += ['-I', path]
    if native_content:
        flags += ['-DHG_NATIVE_ROM_TABLES']
    provenance, artifacts = [], []
    for name, define in [('src/field_system.c', 'HG_SUMMARY_LEARN_FIELD_OBSERVER'), ('src/text.c', 'HG_SUMMARY_LEARN_TEXT_OBSERVER'),
                         ('src/application/bag/bag_app.c', 'HG_SUMMARY_LEARN_BAG_OBSERVER')]:
        original = ROOT / name
        code = original.read_bytes()
        inclusion = ('\n#define ' + define + '\n#include "' + str(helper) + '"\n').encode()
        if name == 'src/field_system.c':
            if code.count(anchor.encode()) != 1:
                raise RuntimeError('Original field-owner hook changed')
            body = code.replace(anchor.encode(), (anchor + hook).encode())
            private = inclusion + ('#line 1 "' + name + '"\n').encode() + body
            if body.replace((anchor + hook).encode(), anchor.encode()) != code:
                raise RuntimeError('Private field copy must contain exactly one observation hook')
        else:
            private = code + inclusion
            if private[:len(code)] != code:
                raise RuntimeError('Private observation copy must preserve its exact original prefix')
        source = destination / Path(name).name
        source.write_bytes(private)
        target = source.with_suffix('.o')
        formal = str(audit / 'objects' / Path(name).with_suffix('.o'))
        if objects.count(formal) != 1:
            raise RuntimeError('Expected exactly one formal observation object: ' + name)
        overlay_provenance = None
        if name == 'src/application/bag/bag_app.c':
            unit, = [row for row in json.loads((audit / 'report.json').read_text())['units'] if row['file'] == name]
            formal_state, = unit['overlayStates']
            if unit['exitCode'] or unit['sha256'] != sha(original) or formal_state['key'] != name or formal_state['getter'] != getter_name(name):
                raise RuntimeError('Private Bag copy must retain its audited overlay owner')
            formal_ir = Path(formal).with_suffix('.ll')
            formal_copy = source.with_name(source.stem + '-formal.ll')
            formal_copy.write_bytes(formal_ir.read_bytes())
            raw_ir = source.with_name(source.stem + '-uninstrumented.ll')
            instrumented_ir = source.with_suffix('.ll')
            state_path = source.with_name(source.stem + '-overlay-state.json')
            run([SDK / 'bin/clang', *flags, '-Xclang', '-disable-llvm-passes', '-S', '-emit-llvm', source, '-o', raw_ir], 'private-bag_app-ir.log')
            try:
                transformed, state = instrument_ir(raw_ir.read_text(), name)
            except ValueError as error:
                state_path.write_text(json.dumps(dict(key=name, error=str(error), rawIrSha256=sha(raw_ir)), indent=2) + '\n')
                raise
            pattern = r'^define\b([^\n]*?)(@' + IDENT + r')\('
            formal_functions = {function for attributes, function in re.findall(pattern, formal_copy.read_text(), re.M)
                                if not re.search(r'\bavailable_externally\b', attributes) and function != '@' + formal_state['getter']}
            private_functions = {function for attributes, function in re.findall(pattern, raw_ir.read_text(), re.M)
                                 if not re.search(r'\bavailable_externally\b', attributes)}
            added_functions = private_functions - formal_functions
            if any(state[key] != formal_state[key] for key in ('key', 'getter', 'globals', 'constructors', 'destructors')) \
                or len(formal_functions) != formal_state['functions'] or len(private_functions) != state['functions'] \
                or private_functions - {'@SummaryLearnBagTaskMatches'} != formal_functions or added_functions != {'@SummaryLearnBagTaskMatches'}:
                state_path.write_text(json.dumps(dict(state=state, formalState=formal_state, addedFunctions=sorted(added_functions),
                                                      removedFunctions=sorted(formal_functions - private_functions)), indent=2) + '\n')
                raise RuntimeError('Private Bag overlay state/functions differ beyond the read-only task predicate')
            instrumented_ir.write_text(transformed)
            state_path.write_text(json.dumps(dict(state=state, formalState=formal_state, formalOwnedFunctions=sorted(formal_functions),
                                                  privateOwnedFunctions=sorted(private_functions), addedFunctions=sorted(added_functions),
                                                  removedFunctions=[]), indent=2) + '\n')
            run([SDK / 'bin/clang', *flags, '-c', instrumented_ir, '-o', target], 'private-bag_app-compile.log')
            if sha(formal_ir) != sha(formal_copy):
                raise RuntimeError('Formal Bag overlay IR changed during private compilation')
            artifacts += [formal_copy, raw_ir, instrumented_ir, state_path]
            overlay_provenance = dict(key=name, getter=state['getter'], stateFile=str(state_path), stateSha256=sha(state_path),
                                      formalIrSha256=sha(formal_copy), rawIrSha256=sha(raw_ir), instrumentedIrSha256=sha(instrumented_ir),
                                      addedFunctions=sorted(added_functions), removedFunctions=[], originalOverlayStatePreserved=True)
        else:
            run([SDK / 'bin/clang', *flags, '-c', source, '-o', target], 'private-' + source.stem + '-compile.log')
        objects[objects.index(formal)] = str(target)
        artifacts += [source, target]
        provenance.append(dict(source=name, originalSourceSha256=sha(original), formalObject=formal,
                               formalObjectSha256=sha(Path(formal)), privateSource=str(source), privateSourceSha256=sha(source),
                               privateObject=str(target), privateObjectSha256=sha(target),
                               observerMacro=define, fieldObservationHookCount=1 if name == 'src/field_system.c' else 0,
                               appendOnly=name != 'src/field_system.c',
                               originalPrefixBytes=len(code) if name != 'src/field_system.c' else None,
                               originalPrefixSha256=sha(original) if name != 'src/field_system.c' else None,
                               change='one post-original FieldSystem_Main observation hook' if name == 'src/field_system.c'
                               else 'unchanged original prefix plus read-only ' + ('printer observer' if name == 'src/text.c' else 'TM task pointer predicate'),
                               originalBodyPreserved=True, overlayProvenance=overlay_provenance))
    return artifacts, provenance


def tm_evidence_passed(rows):
    if len(rows) != 1:
        return False
    row = rows[0]
    if not isinstance(row, dict):
        return False
    exact = dict(status='passed_original_summary_tm_learning', fixturePrepared=1, summaryLifetimes=2,
                 fifthCancelled=1, continuedTrying=1, hmRejected=1, confirmationBack=1, replacement=1,
                 finalMove=182, finalPpUps=0, finalPp=10, finalMaxPp=10, initialTmQuantity=2, finalTmQuantity=1,
                 initialFriendship=70, finalFriendship=71, initialMood=0, finalMood=40,
                 partyFieldsPreserved=True, otherBagBytesPreserved=True, flagsMoneyDexPreserved=True,
                 friendshipMoodPreservedBeforeLearning=True, resetFinished=True, peripheralUiNative=False)
    return all(type(row.get(key)) is type(value) and row[key] == value for key, value in exact.items()) \
        and type(row.get('staleRejected')) is int and row['staleRejected'] >= 4


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--save', required=True, help='Designated private Route29 fixture; only new copies are opened')
    parser.add_argument('--output-name', default='native-summary-menu-pass', help='New private archive basename; existing output is never overwritten')
    parser.add_argument('--tm-learn', action='store_true', help='Private original TM route; only Summary uses native controls, Bag/Party use observed raw input')
    args = parser.parse_args()
    if args.tm_learn and args.output_name == 'native-summary-menu-pass':
        args.output_name = 'native-summary-menu-tm-learn-pass'
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
    if args.tm_learn:
        owned.append(ROOT / 'ios/tests/summary_learn_guest.inc')
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
    if args.tm_learn:
        report.update(scope='private seeded Chikorita/TM17; original Bag/Party/StartMenu route using precisely observed raw peripheral input; native Summary fifth/HM/cancel/replace; original learning result and reset; no complete native TM/HM, UIKit/device or save-writing claim',
                      tmLearn=True, peripheralInput='raw original input, not native peripheral UI', productionApiAdded=False)
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
        # Formal cache objects stay unchanged; TM checks substitute only the three private observation copies.
        private_artifacts = []
        if args.tm_learn:
            private_artifacts, report['privateSourceProvenance'] = prepare_tm_guest(destination / 'private-guest', run, objects,
                                                                                 audit, include_paths, link.get('nativeContent'))
            command.insert(command.index('-o'), '-Wl,--wrap=PokemonSummary_PrintCantForgetHMMessage')
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
        if args.tm_learn:
            extra_exports = ('HGNativeSummaryLearnQAGetState', 'HGNativeBagUIGetState', 'CreateMon', 'MonSetMoveInSlot_ResetPpUp',
                             'Bag_AddItem', 'Bag_GetQuantity', 'Bag_GetPocketSlotN', 'GetMoveMaxPP',
                             'PlayerProfile_GetTrainerID', 'Save_VarsFlags_sizeof')
            for name in extra_exports:
                flag = '-Wl,--export=' + name
                if flag not in command:
                    command.insert(command.index('-o'), flag)
            report['privateAdditionalExports'] = list(extra_exports)
        run(command, 'strict-link.log')
        generated, writes, restores = generate_engine(destination, module, run)
        host_flags += ['-g', '-fsanitize=address,undefined', '-fno-sanitize-recover=all']
        if args.tm_learn:
            host_flags += ['-DHG_SUMMARY_TM_LEARN_CHECK']

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
        artifacts = [*generated, destination / 'hgengine.h', destination / 'hgengine-impl.h', *native, module, binary, listing, *private_artifacts]
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
            result = subprocess.run(argv, cwd=ROOT, env={**os.environ, 'HEARTGOLD_TEST_MUTE': '1'}, capture_output=True, text=True, timeout=240 if args.tm_learn else 180)
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
        if args.tm_learn:
            for key, prefix in [('tmLearningEvidence', 'Summary TM learning evidence: '), ('tmStateEvidence', 'Summary TM state evidence: '),
                                ('tmRawEvidence', 'Summary TM raw evidence: '), ('tmFixtureEvidence', 'Summary TM fixture evidence: ')]:
                report[key] = [json.loads(line[len(prefix):]) for line in output.splitlines() if line.startswith(prefix)]
            report['complete'] = exit_code == 0 and len(evidence) == 1 and evidence[0]['status'] == 'passed_original_summary_tm_learning'
            report['passed'] = report['complete'] and tm_evidence_passed(report['tmLearningEvidence']) and 'PASS: original TM route and native Summary learning' in output
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
