# SPDX-License-Identifier: GPL-3.0-or-later
"""Reuse one separately approved passed native PC guest and its exact input trace.

This deliberately does not call validate_full(): the native input bank predates later
host-only CHECK edits. It verifies that bank as historical native provenance,
then verifies current compiled C sources, public headers, QA sources and module.
"""
import hashlib
import json
from pathlib import Path
import re
import struct

QA_NAMES = ('HGNativePCBoxQAGetState', 'HGNativePCBoxQARequest')
QA_VERSION, QA_BYTES = 2, 1372
QA_RELATIVE = ('ios/tests/pc_box_qa.h', 'ios/tests/pc_box_guest.inc',
    'ios/Original/abi32/pc_box_scenario_check.c', 'ios/tools/check_native_pc_box.py')
BITS = dict(PHYSICAL=1, DEPOSIT=2, WITHDRAW=4, MOVE=8, SWAP=16, LAST=32,
    RELEASE_CANCEL=64, ITEMS=128, SUMMARY=256, NAMING=512)
SHA256 = re.compile('[0-9a-f]{64}')


def sha(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest()


def read(path):
    return json.loads(Path(path).read_text())


def require(value, message):
    if not value:
        raise RuntimeError(message)


def digest_map(value):
    return hashlib.sha256(json.dumps(value, sort_keys=True, separators=(',', ':')).encode()).hexdigest()


def runtime_bank_provenance(report, preparation, historical, runtime, root, historical_sha, runtime_sha):
    """Validate frozen native provenance without treating later host edits as that run's inputs."""
    require(isinstance(historical, dict) and historical and isinstance(runtime, dict) and set(historical) == set(runtime),
        'Historical Full and actual native runtime inventories must match exactly')
    for bank in (historical, runtime):
        require(all(isinstance(name, str) and Path(name).is_absolute() and isinstance(value, str)
            and SHA256.fullmatch(value) for name, value in bank.items()), 'Invalid historical/runtime input hash bank')
    delta = {name: dict(before=before, after=runtime[name]) for name, before in historical.items() if runtime[name] != before}
    allowed = {str(root / relative) for relative in QA_RELATIVE[1:]}
    require(not set(delta) - allowed, 'Only the three approved private harness files may differ after Full')
    qa_hashes = {str(root / relative): runtime.get(str(root / relative)) for relative in QA_RELATIVE}
    require(all(isinstance(value, str) and SHA256.fullmatch(value) for value in qa_hashes.values()), 'Runtime bank must bind all four QA files')
    for evidence in (report, preparation):
        require(type(evidence.get('fullSourceBankCurrent')) is bool and evidence['fullSourceBankCurrent'] is (not delta)
            and evidence.get('historicalFullInputBankSha256') == historical_sha
            and evidence.get('actualRuntimeInputBankSha256') == runtime_sha
            and evidence.get('postFullPrivateHarnessDelta') == delta
            and evidence.get('currentQAHashes') == qa_hashes
            and type(evidence.get('sourceInputCount')) is int and evidence['sourceInputCount'] == len(runtime),
            'Native historical/runtime bank or private harness delta metadata changed')
    return delta, qa_hashes


def decode_qa_snapshot(data):
    require(len(data) == QA_BYTES, 'Native checkpoint must use the actual QA2/1372 PC ABI')
    words = struct.unpack('<343I', data)
    require(words[0] == QA_VERSION and words[1] == QA_BYTES and not words[3] and words[342] in (0, 1),
        'Invalid native checkpoint ABI/error/message-wait fact')
    return dict(qaVersion=words[0], qaBytes=words[1], stage=words[4], baselinePrepared=words[6], preparationSetterCalls=words[7],
        checkpointSerial=words[8], conservationChecks=words[9], mapId=words[10], x=words[11], z=words[12],
        facing=words[13], pcTileBehavior=words[14], fieldReady=words[15], scriptMenuReady=words[16], partyCount=words[27], boxedCount=words[28],
        bagUnchanged=words[29], flagsUnchanged=words[30], moneyDexUnchanged=words[31], boxNameUnchanged=words[32],
        mons=[list(words[33 + index * 5:38 + index * 5]) for index in range(4)],
        pc=dict(zip(('active', 'ownerGeneration', 'revision', 'ready', 'phase', 'mode', 'activeBox', 'selectedSlot',
            'partyPanelOpen', 'trayOpen', 'actionMenuOpen', 'cursorHolding', 'viewActive'), words[53:66])),
        summary=dict(active=words[66], revision=words[67], mode=words[68], phase=words[69], ready=words[70],
            monDataType=words[73], partySlot=words[74], partyCount=words[75], previousMonEnabled=words[105],
            previousMonSlot=words[106], nextMonEnabled=words[107], nextMonSlot=words[108]),
        naming=dict(zip(('active', 'ownerGeneration', 'revision', 'ready'), words[267:271])), messageAwaitInput=words[342])


def qa_snapshot(path):
    return decode_qa_snapshot(Path(path).read_bytes())


def native_steps(source):
    """Read step ordinals from the approved C table; no routing/gameplay policy is recreated."""
    match = re.search(r'static const PCBoxStep steps\[\]\s*=\s*\{(.*?)\n\};', source, re.S)
    require(match is not None, 'Approved native step table changed')
    rows = []; start = depth = 0
    for index, char in enumerate(match[1]):
        depth += (char in '({') - (char in ')}')
        require(depth >= 0, 'Malformed native step table')
        if char == ',' and not depth:
            row = match[1][start:index].strip()
            if row:
                rows.append(row)
            start = index + 1
    require(not depth, 'Unconsumed native step table')
    if match[1][start:].strip():
        rows.append(match[1][start:].strip())
    return rows


def approved_boot(legacy, rows):
    """Bind the approved finite Title/Continue presses to their original readiness evidence."""
    require(isinstance(legacy, list) and len(legacy) == 1 and isinstance(legacy[0], dict)
        and type(legacy[0].get('ordinal')) is int and legacy[0]['ordinal'] == 0
        and type(legacy[0].get('keys')) is int and legacy[0]['keys'] == 8
        and type(legacy[0].get('frame')) is int and legacy[0]['frame'] >= 450,
        'Only the original START skip may use its source frame threshold')
    sequence = [('ready', 'title'), ('press', 'title'), ('released', 'title'), ('settled', 'title'),
        ('ready', 'continue'), ('press', 'continue'), ('continue_selected', 'continue'), ('released', 'continue'), ('settled', 'continue')]
    require(isinstance(rows, list) and len(rows) == len(sequence)
        and all(isinstance(row, dict) and (row.get('event'), row.get('role')) == expected
            and row.get('owner') == 'boot' for row, expected in zip(rows, sequence)), 'Typed boot event/role ordering changed')
    require(all(type(row.get(name)) is int and 0 <= row[name] <= 0xFFFFFFFF
        for row in rows for name in ('frame', 'padReads', 'kind', 'outer', 'manager', 'exec', 'proc', 'data', 'option', 'optionId',
            'selectedApp', 'notice', 'noticeDelay', 'noSave', 'inputMode', 'optionTimer', 'titleDelay', 'fade', 'pending',
            'requiredMask', 'renderedMask', 'variedMask')), 'Boot facts must be actual u32 values')
    require([row['frame'] for row in rows] == sorted(row['frame'] for row in rows), 'Boot frame ordering changed')
    result = list(legacy)
    for ready, press, release, settled in ((rows[0], rows[1], rows[2], rows[3]), (rows[4], rows[5], rows[7], rows[8])):
        require({key: value for key, value in ready.items() if key != 'event'}
            == {key: value for key, value in press.items() if key != 'event'}, 'Boot readiness differs from the admitted press')
        require(press['manager'] and press['data'] and not ((press['manager'] | press['data']) & 3)
            and press['exec'] == 2 and press['fade'] == 1 and press['pending'] == 0
            and press['requiredMask'] == press['renderedMask'] == press['variedMask'] == 1,
            'Boot press requires an original ready owner and visible source pixels')
        require(press['frame'] > result[-1]['frame'] and release['frame'] >= press['frame'] and settled['frame'] >= release['frame']
            and release['padReads'] - press['padReads'] == settled['padReads'] - release['padReads'] == 2,
            'Boot input/release lease changed')
        if press['role'] == 'title':
            require(press['kind'] == 1 and press['outer'] == 60 and press['proc'] == 2 and not press['titleDelay'], 'Title was not ready')
        else:
            require(press['kind'] == 2 and press['outer'] == 74 and press['proc'] == 5 and press['option'] < 9
                and press['optionId'] == 1 and press['notice'] == 15 and not any(press[name]
                    for name in ('noticeDelay', 'noSave', 'inputMode', 'optionTimer')), 'Continue was not ready in button mode')
        result.append(dict(ordinal=len(result), frame=press['frame'], keys=1))
    selected = rows[6]
    require(selected['manager'] == rows[5]['manager'] and selected['outer'] == 74 and selected['exec'] == 2
        and selected['proc'] == 8 and selected['selectedApp'] == 1
        and rows[5]['frame'] <= selected['frame'] <= rows[7]['frame'], 'Original Continue exit was not observed')
    return result


def typed_boot_trace(report, runtime):
    rows = [json.loads(line.removeprefix('PC Box boot: ')) for line in runtime.splitlines() if line.startswith('PC Box boot: ')]
    require(rows == report.get('typedBootTrace', report.get('extra', {}).get('typedBootTrace')),
        'Typed boot evidence is missing or differs from the actual runtime log')
    return rows


def approved_actions(rows):
    require(isinstance(rows, list) and rows and len(rows) % 3 == 0, 'Approved trace must contain complete action triplets')
    result = []
    previous_step = -1
    for ordinal in range(0, len(rows), 3):
        press, release, settle = rows[ordinal:ordinal + 3]
        require(all(isinstance(row, dict) for row in (press, release, settle)), 'Native input rows must be objects')
        require([row.get('event') for row in (press, release, settle)] == ['press', 'released', 'settled'], 'Native input ordering changed')
        require(press.get('owner') == release.get('owner') == settle.get('owner'), 'Native lease owner must remain scoped through release')
        require(all(type(row.get(name)) is int and 0 <= row[name] <= 0xFFFFFFFF
            for row in (press, release, settle) for name in ('step', 'menuSubstep', 'padReads', 'frame'))
            and previous_step <= press['step'] <= release['step'] <= settle['step'],
            'Native action steps must be monotonic')
        # pcNext() may advance the native step before release/settle; only the press identifies the action.
        require(type(press.get('keys')) is int and press['keys'] in (1, 2, 8, 16, 32, 64, 128, 2048)
            and type(release.get('keys')) is int and type(settle.get('keys')) is int and release['keys'] == settle['keys'] == 0
            and release['padReads'] - press['padReads'] >= 2
            and settle['padReads'] - release['padReads'] >= 2, 'Invalid native key/release evidence')
        require(press.get('owner') in ('field', 'field_script', 'field_message', 'pc', 'summary', 'naming')
            and all(isinstance(press.get(key), dict) for key in ('pc', 'summary', 'naming', 'fieldChoice')), 'Native owner evidence is missing')
        require(all(type(row.get('messageAwaitInput')) is int and row['messageAwaitInput'] in (0, 1)
            for row in (press, release, settle)), 'Native message-wait facts must be exact zero/one u32 values')
        for packet, names in dict(pc=('phase', 'mode', 'selectedSlot', 'ready', 'viewActive', 'ownerGeneration', 'revision'),
                summary=('active', 'phase', 'partySlot', 'ready'), naming=('active', 'ready', 'ownerGeneration', 'revision'),
                fieldChoice=('active', 'ready', 'viewActive', 'phase', 'ownerGeneration', 'revision')).items():
            require(all(type(press[packet].get(name)) is int and 0 <= press[packet][name] <= 0xFFFFFFFF for name in names),
                'Native semantic packet fields must be actual u32 values, not legacy Boolean coercions')
        require(all(press[packet][name] in (0, 1) for packet, names in
            dict(pc=('ready', 'viewActive'), summary=('active', 'ready'), naming=('active', 'ready'),
                fieldChoice=('active', 'ready', 'viewActive')).items() for name in names), 'Native scene flags must be zero or one')
        keys_by_owner = dict(pc=(1, 2, 16, 32, 64, 128, 2048), summary=(2, 64, 128), naming=(1, 8),
            field=(1, 2, 8, 16, 32, 64, 128), field_script=(1, 2, 16, 32, 64, 128), field_message=(1,))
        require(press['keys'] in keys_by_owner[press['owner']], 'Approved action has no existing attached UIKit control mapping')
        if press['owner'] == 'pc':
            require(press['pc']['ready'] == press['pc']['viewActive'] == 1 and press['pc']['mode'] <= 3,
                'Native PC press was not made against a ready original view')
        elif press['owner'] in ('summary', 'naming'):
            require(press[press['owner']]['active'] == press[press['owner']]['ready'] == 1,
                'Native child press was not made against a ready original child')
        elif press['owner'] == 'field_message':
            choice = press['fieldChoice']
            require(press['messageAwaitInput'] == 1 and not press['summary']['active'] and not press['naming']['active']
                and not press['pc']['ready'] and not press['pc']['viewActive']
                and not (choice['active'] and (choice['ready'] or choice['viewActive'] or 7 <= choice['phase'] <= 10))
                and all(row.get('owner') == 'field_message' and row['step'] == press['step']
                    and row['menuSubstep'] == press['menuSubstep'] for row in (release, settle)),
                'Original field-message A cannot overlap a choice view or another action scope')
        result.append(dict(ordinal=len(result), nativeStep=press['step'], keys=press['keys'], owner=press['owner'],
            menuSubstep=press['menuSubstep'], messageAwaitInput=press['messageAwaitInput'],
            pc=press['pc'], summary=press['summary'], naming=press['naming'], fieldChoice=press['fieldChoice']))
        previous_step = settle['step']
    return result


def pc_box_guest(report_path, audit, link, approved_report_sha256):
    """Return the exact passed private module + a replay resource; never build another guest.

    approved_report_sha256 must be pinned by review after the native case passes.
    Current failed native-pc-box-pass/report.json is intentionally inadmissible.
    """
    from audit_original import digest
    from check_native_pc_box import (evidence_passed, wasm_exports, field_message_evidence_passed,
        snapshot_evidence_passed, shipping_exports_passed, normal_checks_passed, boot_argument)
    from check_native_rainy_field import INCLUDES
    from check_abi32 import ROOT
    requested = Path(report_path)
    require(not requested.is_symlink(), 'Select a concrete native report, not a symlink')
    report_path = requested.resolve(); folder = report_path.parent; audit = Path(audit)
    require(re.fullmatch('[0-9a-f]{64}', approved_report_sha256 or '') is not None,
        'A separately reviewed exact passed native report hash must be pinned')
    require(report_path.name == 'report.json' and not report_path.is_symlink() and sha(report_path) == approved_report_sha256,
        'Native report differs from the separately approved immutable report')
    report = read(report_path); preparation = report.get('privateGuest', {})
    require(report.get('passed') is True and report.get('complete') is True and report.get('status') == 'passed_original_pc_box'
        and report.get('firstFailure') is None and report.get('gameRuntimeExecuted') is True
        and report.get('allChildrenWaited') is True and report.get('protectedInputsUnchanged') is True,
        'The actual native PC scenario must pass first; no retry/substitute/rewritten approval')
    evidence = report.get('evidence', [])
    require(len(evidence) == 1 and evidence_passed(evidence[0]), 'Native conservation/coverage/reset evidence failed')
    require(all(type(evidence[0].get(name)) is int and 0 <= evidence[0][name] <= 0xFFFFFFFF for name in
        ('step', 'guestError', 'qaVersion', 'qaBytes', 'fieldMessageAcks', 'baselinePrepared', 'preparationSetterCalls', 'noSettersAfterBaseline', 'conservationChecks',
            'coverageMask', 'requiredCoverageMask', 'staleRejected', 'busyRejected', 'snapshots'))
        and evidence[0]['guestError'] == 0 and evidence[0]['qaVersion'] == QA_VERSION and evidence[0]['qaBytes'] == QA_BYTES,
        'Native numeric evidence must use exact QA2/u32 values, not Boolean coercions')
    require(report.get('noAutomaticRetry') is True and report.get('commands')
        and all(type(row.get('terminalReturnCode')) is int and row['terminalReturnCode'] == 0
            and row.get('timedOut') is False and row.get('allChildrenWaited') is True
            for row in report['commands']), 'Native command completion/preservation evidence failed')
    for command in report['commands']:
        log = Path(command['log'])
        require(log.resolve().is_relative_to(folder) and not log.is_symlink() and sha(log) == command.get('outputSha256'),
            'Native command log identity changed')
    require(sha(folder / 'native/pc-box-check') == report.get('nativeBinarySha256'), 'Actual passed native host binary changed')
    runtime = (folder / 'runtime.log').read_text()
    observed_trace = [json.loads(line.removeprefix('PC Box input: ')) for line in runtime.splitlines() if line.startswith('PC Box input: ')]
    observed_evidence = [json.loads(line.removeprefix('PC Box evidence: ')) for line in runtime.splitlines() if line.startswith('PC Box evidence: ')]
    observed_snapshots = [json.loads(line.removeprefix('PC Box snapshot: ')) for line in runtime.splitlines() if line.startswith('PC Box snapshot: ')]
    messages = [row for row in observed_trace if row.get('owner') == 'field_message']
    require(observed_trace == report.get('actionTrace') and observed_evidence == evidence
        and observed_snapshots == report.get('typedSnapshotEvidence') and messages == report.get('typedFieldMessageEvidence')
        and field_message_evidence_passed(observed_trace, evidence[0]['fieldMessageAcks'])
        and snapshot_evidence_passed(observed_snapshots)
        and evidence[0]['snapshots'] == sum(row['event'] == 'captured' for row in observed_snapshots),
        'Report action/conservation evidence differs from the actual native runtime log')
    require(preparation == read(folder / 'guest/preparation.json') and preparation.get('passed') is True
        and preparation.get('originalBodyPreserved') is True and preparation.get('postOriginalFieldHookCount') == 1
        and preparation.get('productionApiAdded') is False and preparation.get('wrappers') == ['PCBox_Main']
        and preparation.get('privateQAExports') == list(QA_NAMES), 'Private observer provenance is invalid')
    module = folder / 'guest/engine.wasm'
    shipping_sha256 = sha(audit / 'engine.wasm')
    require(not module.is_symlink() and sha(module) == report.get('privateModuleSha256') == preparation.get('privateModuleSha256')
        and sha(module) != shipping_sha256, 'The selected module is not the actual passed private guest')
    require(type(link.get('exitCode')) is int and link['exitCode'] == 0 and link.get('undefinedSymbols') == []
        and link.get('outputSha256') == report.get('shippingModuleSha256') == preparation.get('shippingModuleSha256') == shipping_sha256,
        'Native proof differs from the actual current canonical production module')
    shipping = wasm_exports((audit / 'engine.wasm').read_bytes())
    require(shipping_exports_passed(preparation.get('shippingExports'), shipping)
        and wasm_exports(module.read_bytes()) == sorted([*shipping, *QA_NAMES]), 'Private exports must be exactly current production 28 plus two PC QA exports')
    full_path = Path(report['fullReceipt']); formal = read(full_path); full_folder = full_path.parent
    require(sha(full_path) == report.get('fullReceiptSha256') == preparation.get('fullReceiptSha256')
        and formal.get('passed') is True and formal.get('freshAuditPassed') is True and formal.get('inputsUnchanged') is True
        and type(formal.get('actualNormalCheckKinds')) is int and formal['actualNormalCheckKinds'] == 99
        and normal_checks_passed(formal.get('actualNormalChecks'), formal['actualNormalCheckKinds'])
        and formal.get('shipping', {}).get('sha256') == shipping_sha256
        and shipping_exports_passed(formal.get('shipping', {}).get('exports'), shipping),
        'Historical Full receipt must bind the actual current production28 and 99 normal checks, including FieldChoice')
    seals = read(full_folder / 'seals-before.json')
    require(seals == read(full_folder / 'seals-after.json') and isinstance(seals.get('protected'), dict)
        and len(seals['protected']) == 45 and all(Path(name).is_absolute() and Path(name).is_file()
            and not Path(name).is_symlink() and sha(name) == expected for name, expected in seals['protected'].items()),
        'Historical Full protected forty-five files changed')
    before = read(full_folder / 'inputs-before.json')
    require(before == read(full_folder / 'inputs-after.json'), 'Historical Full input bank changed')
    runtime_bank_file = folder / 'guest/runtime-input-bank.json'
    require(not runtime_bank_file.is_symlink() and report.get('actualRuntimeInputBankFile') == str(runtime_bank_file)
        and preparation.get('actualRuntimeInputBankFile') == str(runtime_bank_file), 'Native runtime bank path changed')
    delta, qa_hashes = runtime_bank_provenance(report, preparation, before, read(runtime_bank_file), ROOT,
        sha(full_folder / 'inputs-before.json'), sha(runtime_bank_file))
    audited = read(audit / 'report.json'); native_audited = read(full_folder / 'report.json')
    cmap = {row['file']: row['sha256'] for row in native_audited['units']}
    require(len(cmap) == len(native_audited['units']) == len(audited['units']) == formal.get('auditedUnits')
        and digest_map(cmap) == formal.get('auditedSources', {}).get('sourceMapSha256')
        == report.get('sourceMapSha256') == preparation.get('sourceMapSha256')
        and cmap == {row['file']: row['sha256'] for row in audited['units']}, 'Current native C source map differs from the passed native map')
    for relative, expected in cmap.items():
        require(not Path(relative).is_absolute() and '..' not in Path(relative).parts and sha(ROOT / relative) == expected,
            'Current compiled C source changed: ' + relative)
    includes = [ROOT / name for name in INCLUDES]
    if link.get('nativeContent'):
        includes.insert(0, ROOT / link['nativeContent']['include'])
    headers = {path for root in includes for pattern in ('**/*.h', '**/*.naix') for path in root.glob(pattern)}
    headers |= set((ROOT / 'lib/MSL_C/src').glob('*.h')) | set((ROOT / 'lib/MSL_C/src').glob('*.hpp'))
    require(digest(headers) == report.get('headersSha256') == preparation.get('headersSha256') == audited.get('headersSha256')
        == native_audited.get('headersSha256') == formal.get('shipping', {}).get('headersSha256'),
        'Current public headers differ from the actual passed native guest')
    artifacts = preparation.get('artifactHashes', {})
    guest_files = {str(folder / 'guest' / name) for name in
        ('field-pc-box.c', 'field-pc-box.o', 'objects.rsp', 'engine.wasm', 'runtime-input-bank.json')}
    require(isinstance(artifacts, dict) and set(artifacts) == guest_files | set(qa_hashes)
        and all(Path(name).is_file() and not Path(name).is_symlink() and sha(name) == expected for name, expected in artifacts.items()),
        'Private native source/object/module artifact changed')
    for relative in QA_RELATIVE:
        current = ROOT / relative
        require(artifacts.get(str(current)) == qa_hashes[str(current)] == sha(current), 'Current private QA source changed: ' + relative)
    original = ROOT / 'src/field_system.c'
    body = original.read_text(); anchor = 'BOOL finished = FieldSystem_Main(fieldSystem);'
    require(body.count(anchor) == 1, 'Original field observation anchor changed')
    private_source = '#include "' + str(ROOT / 'ios/tests/pc_box_guest.inc') + '"\n#line 1 "src/field_system.c"\n' \
        + body.replace(anchor, anchor + '\n    PCBoxQAFieldTick(fieldSystem);')
    require((folder / 'guest/field-pc-box.c').read_text() == private_source
        and preparation.get('originalSourceSha256') == sha(original) == cmap['src/field_system.c']
        and preparation.get('replacedFormalObjects') == [str(audit / 'objects/src/field_system.o')]
        and preparation.get('privateObjects') == [str(folder / 'guest/field-pc-box.o')],
        'Private guest must replace exactly the unchanged original resident field owner')
    fixture = ROOT.parent / 'native-ios-route29-save/heartgold.sav'
    originals = {str(Path(str(fixture) + suffix)) for suffix in ('', '.console', '.wifi', '.lock')}
    copies = {str(folder / ('scenario.sav' + suffix)) for suffix in ('', '.console', '.wifi', '.lock')}
    require(set(report.get('fixtureHashes', {})) == originals and set(report.get('privateSaveHashesBefore', {})) == copies
        and all(not Path(name).is_symlink() for name in originals | copies), 'Private fixture must be the designated concrete four-file set')
    require(report.get('privateSaveHashesBefore') == report.get('privateSaveHashesAfter')
        and report.get('fixtureHashes') and all(sha(name) == expected for name, expected in report['fixtureHashes'].items())
        and all(sha(name) == expected for name, expected in report['privateSaveHashesAfter'].items()),
        'The designated private four-file save or approved native copies changed')
    captures = report.get('capturedFiles', {})
    require(isinstance(captures, dict) and captures, 'Actual native frame/state captures are required')
    for relative, expected in captures.items():
        require(not Path(relative).is_absolute() and '..' not in Path(relative).parts and sha(folder / relative) == expected,
            'Approved native frame/state artifact changed: ' + relative)
    actions = approved_actions(report['actionTrace'])
    require(len(actions) <= 4096, 'Approved action trace exceeds the bounded replay resource')
    steps = native_steps((ROOT / QA_RELATIVE[2]).read_text())
    snapshots = []
    for row in observed_snapshots:
        if row['event'] != 'captured':
            continue
        prefix = f"frames/snapshot-{row['snapshot']:03d}-step-{row['step']:03d}"
        state_path = prefix + '-state.bin'
        require(state_path in captures, 'Actual typed snapshot state is not archived: ' + state_path)
        snapshot = qa_snapshot(folder / state_path)
        actual = dict(mapId=snapshot['mapId'], ownerGeneration=snapshot['pc']['ownerGeneration'], phase=snapshot['pc']['phase'],
            pcActive=snapshot['pc']['active'], summaryActive=snapshot['summary']['active'], namingActive=snapshot['naming']['active'],
            scriptMenuReady=snapshot['scriptMenuReady'])
        require(all(row[name] == value for name, value in actual.items()), 'Typed snapshot metadata differs from its actual QA2 state')
        for engine in range(2):
            if row['renderedMask'] & (1 << engine):
                require(prefix + f'-engine-{engine}.ppm' in captures, 'Actual typed snapshot engine pixels are not archived')
        snapshot.update(nativeStep=row['step'], snapshotSequence=row['snapshot'], requiredMask=row['requiredMask'],
            renderedMask=row['renderedMask'], variedMask=row['variedMask'], stateFile=state_path, stateSha256=captures[state_path])
        snapshots.append(snapshot)
    checkpoints = []
    for step, row in enumerate(steps):
        if row.startswith('CHECKPOINT('):
            matches = [snapshot for snapshot in snapshots if snapshot['nativeStep'] == step]
            require(len(matches) == 1, 'Each approved native checkpoint must have exactly one typed capture')
            snapshot = dict(matches[0])
            snapshot.update(nativeStep=step, coverage=sum(value for name, value in BITS.items() if 'HG_PC_QA_' + name in row))
            checkpoints.append(snapshot)
    require(len(checkpoints) >= 10 and sum(1 for row in checkpoints if row['mapId'] == 33) == 1,
        'Approved native read-only checkpoints/final Route29 state are missing')
    coverage = 0
    for checkpoint in checkpoints:
        coverage |= checkpoint['coverage']
    require(coverage == 1023, 'Native checkpoint table does not cover the approved scenario')
    entry_step, = [step for step, row in enumerate(steps) if row == '{OP_ENTRY}']
    entries = [snapshot for snapshot in snapshots if snapshot['nativeStep'] == entry_step]
    require(len(entries) == 1, 'Approved physical entry must have exactly one typed capture')
    entry = entries[0]
    legacy_boot = [dict(ordinal=int(a), frame=int(b), keys=int(c)) for a, b, c in re.findall(
        r'Native test input: step=(\d+) frame=(\d+) keys=(\d+) touch=0,0/0', runtime)]
    typed_boot = typed_boot_trace(report, runtime)
    boot = approved_boot(legacy_boot, typed_boot)
    token = boot_argument(sha(module), (ROOT / QA_RELATIVE[2]).read_text())
    require({'frames/boot-title-main.ppm', 'frames/boot-continue-main.ppm'} <= set(captures), 'Original boot source witnesses are missing')
    runtime_commands = [row['argv'] for row in report['commands'] if row['argv'][0] == str(folder / 'native/pc-box-check')]
    require(len(runtime_commands) == 1 and len(runtime_commands[0]) == 6 and runtime_commands[0][-1] == token
        and runtime_commands[0].count(token) == 1, 'Canonical runtime boot token is missing or duplicated')
    boot_binding = report.get('extra', {}).get('nativeObjectReuse', {}).get('bootBinding')
    if boot_binding is not None:
        require(isinstance(boot_binding, dict) and sha(boot_binding['path']) == boot_binding['sha256'] == token.removeprefix('--pc-boot-binding=')
            and read(boot_binding['path']) == boot_binding['manifest']
            and boot_binding['manifest']['privateModuleSha256'] == sha(module)
            and boot_binding['manifest']['shippingModuleSha256'] == shipping_sha256, 'Optional boot layout provenance changed')
        layout_full = Path(boot_binding.get('layoutFullReceipt', full_path))
        layout_proof = read(layout_full)
        require(sha(layout_full) == boot_binding['manifest']['fullReceiptSha256'] and layout_proof.get('passed') is True
            and layout_proof.get('freshAuditPassed') is True and layout_proof.get('inputsUnchanged') is True
            and layout_proof['shipping']['sha256'] == shipping_sha256
            and layout_proof['shipping']['headersSha256'] == audited['headersSha256']
            and layout_proof['auditedSources']['sourceMapSha256'] == report['sourceMapSha256'], 'Optional historical boot layout Full changed')
        require(all(sha(ROOT / name) == value for name, value in boot_binding['manifest']['sourceHashes'].items()),
            'Optional original boot layout source changed')
    resource = dict(version=2, qaVersion=QA_VERSION, qaBytes=QA_BYTES, sourceReportSha256=sha(report_path), privateModuleSha256=sha(module),
        shippingModuleSha256=shipping_sha256, actions=actions, checkpoints=checkpoints, snapshots=snapshots,
        typedSnapshotEvidence=observed_snapshots, typedBootEvidence=typed_boot, entry=entry, boot=boot,
        scope='same approved native input trace through attached UIKit controls; physicalGestures=false')
    payload = (json.dumps(resource, sort_keys=True, separators=(',', ':')) + '\n').encode()
    metadata = dict(pcBox=True, qaObserverVersion=QA_VERSION, qaObserverBytes=QA_BYTES, privateQAExports=list(QA_NAMES), sourceReport=str(report_path),
        sourceReportSha256=sha(report_path), privateGuestModule=str(module), privateGuestModuleSha256=sha(module),
        shippingModuleSha256=shipping_sha256, headersSha256=audited['headersSha256'], sourceMapSha256=report['sourceMapSha256'],
        historicalFullReceipt=str(full_path), historicalFullReceiptSha256=sha(full_path),
        nativeInputBankCurrentAtNativeRun=report['fullSourceBankCurrent'], historicalFullInputBankSha256=report['historicalFullInputBankSha256'],
        actualRuntimeInputBankSha256=report['actualRuntimeInputBankSha256'], actualRuntimeInputBankFile=str(runtime_bank_file),
        postFullPrivateHarnessDelta=delta, currentQAHashes=qa_hashes,
        historicalSourceBankScope='frozen native provenance only; current App whole-bank identity is independently recorded by its builder',
        replayResourceSha256=hashlib.sha256(payload).hexdigest(), productionApiAdded=False)
    return module, metadata, payload


def self_check():
    """Pure helper checks only: no source inventory, archive approval, build, or game run."""
    root = Path('/tmp/pc-box-binding-self-check')
    historical = {str(root / name): 'a' * 64 for name in (*QA_RELATIVE, 'ios/OriginalApp/main.m')}
    runtime = {**historical, **{str(root / name): 'b' * 64 for name in QA_RELATIVE[1:]}}
    delta = {name: dict(before=value, after=runtime[name]) for name, value in historical.items() if runtime[name] != value}
    qa_hashes = {str(root / name): runtime[str(root / name)] for name in QA_RELATIVE}
    evidence = dict(fullSourceBankCurrent=False, historicalFullInputBankSha256='c' * 64, actualRuntimeInputBankSha256='d' * 64,
        postFullPrivateHarnessDelta=delta, currentQAHashes=qa_hashes, sourceInputCount=len(runtime))
    assert runtime_bank_provenance(evidence, evidence, historical, runtime, root, 'c' * 64, 'd' * 64) == (delta, qa_hashes)
    unchanged_qa = {str(root / name): historical[str(root / name)] for name in QA_RELATIVE}
    unchanged = {**evidence, 'fullSourceBankCurrent': True, 'postFullPrivateHarnessDelta': {}, 'currentQAHashes': unchanged_qa}
    assert runtime_bank_provenance(unchanged, unchanged, historical, historical, root, 'c' * 64, 'd' * 64) == ({}, unchanged_qa)
    bad_bank = {**runtime, str(root / 'ios/OriginalApp/main.m'): 'e' * 64}
    rejected = [lambda: runtime_bank_provenance(evidence, evidence, historical, bad_bank, root, 'c' * 64, 'd' * 64),
        lambda: runtime_bank_provenance(evidence, evidence, historical, {**runtime, '/tmp/extra': 'a' * 64}, root, 'c' * 64, 'd' * 64)]
    for key, value in [('fullSourceBankCurrent', 0), ('historicalFullInputBankSha256', 'e' * 64),
            ('actualRuntimeInputBankSha256', 'e' * 64), ('postFullPrivateHarnessDelta', {}), ('currentQAHashes', {}), ('sourceInputCount', True)]:
        rejected.append(lambda key=key, value=value: runtime_bank_provenance({**evidence, key: value}, evidence,
            historical, runtime, root, 'c' * 64, 'd' * 64))
    pc = dict(phase=12, mode=2, selectedSlot=14, ready=1, viewActive=1, ownerGeneration=7, revision=11)
    press = dict(event='press', step=4, menuSubstep=0, keys=1, padReads=20, frame=40, owner='pc', messageAwaitInput=0, pc=pc,
        summary=dict(active=0, phase=0, partySlot=0, ready=0), naming=dict(active=0, ready=0, ownerGeneration=0, revision=0),
        fieldChoice=dict(active=0, ready=0, viewActive=0, phase=0, ownerGeneration=0, revision=0))
    trace = [press, dict(event='released', step=5, menuSubstep=0, keys=0, padReads=22, frame=42, owner='pc', messageAwaitInput=0),
        dict(event='settled', step=5, menuSubstep=0, keys=0, padReads=24, frame=44, owner='pc', messageAwaitInput=0)]
    action, = approved_actions(trace)
    assert action['ordinal'] == 0 and action['nativeStep'] == 4 and action['pc'] == pc
    for malformed in ([], trace[:2], [press, {**trace[1], 'keys': False}, trace[2]],
            [{**press, 'pc': {**pc, 'ready': True}}, *trace[1:]],
            [{**press, 'keys': 8}, *trace[1:]], [press, {**trace[1], 'padReads': 20}, trace[2]],
            [press, trace[1], {**trace[2], 'padReads': 23}], [{**press, 'step': -1}, *trace[1:]]):
        rejected.append(lambda malformed=malformed: approved_actions(malformed))
    message_press = {**press, 'step': 6, 'owner': 'field_message', 'messageAwaitInput': 1,
        'pc': dict(phase=0, mode=0, selectedSlot=0, ready=0, viewActive=0, ownerGeneration=0, revision=0),
        'fieldChoice': dict(active=1, ready=0, viewActive=0, phase=2, ownerGeneration=17, revision=29)}
    message = [message_press, {**trace[1], 'step': 6, 'owner': 'field_message'}, {**trace[2], 'step': 6, 'owner': 'field_message'}]
    message_action, = approved_actions(message)
    assert message_action['owner'] == 'field_message' and message_action['menuSubstep'] == 0 and message_action['messageAwaitInput'] == 1
    for malformed in ([{**message_press, 'keys': 2}, *message[1:]], [{**message_press, 'messageAwaitInput': 0}, *message[1:]],
            [{**message_press, 'fieldChoice': {**message_press['fieldChoice'], 'phase': 8}}, *message[1:]],
            [message_press, {**message[1], 'menuSubstep': 1}, message[2]], [message_press, {**message[1], 'owner': 'field'}, message[2]],
            [{**message_press, 'messageAwaitInput': True}, *message[1:]]):
        rejected.append(lambda malformed=malformed: approved_actions(malformed))
    words = [0] * (QA_BYTES // 4); words[0] = QA_VERSION; words[1] = QA_BYTES; words[342] = 1
    decoded = decode_qa_snapshot(struct.pack('<343I', *words))
    assert decoded['qaVersion'] == 2 and decoded['qaBytes'] == 1372 and decoded['messageAwaitInput'] == 1
    bad_words = words.copy(); bad_words[342] = 2
    rejected.extend([lambda: decode_qa_snapshot(b'\0' * 1368),
        lambda: decode_qa_snapshot(struct.pack('<343I', *bad_words))])
    for operation in rejected:
        try:
            operation()
        except RuntimeError:
            pass
        else:
            raise AssertionError('Malformed native binding evidence was accepted')
    assert native_steps('static const PCBoxStep steps[]={\n{OP_ENTRY},CHECKPOINT(0,0,2,1,29,14),\n};') == \
        ['{OP_ENTRY}', 'CHECKPOINT(0,0,2,1,29,14)']
    assert native_steps('static const PCBoxStep steps[]={\n{OP_ENTRY},{OP_FINISH}\n};') == ['{OP_ENTRY}', '{OP_FINISH}']
    boot_names = ('frame', 'padReads', 'kind', 'outer', 'manager', 'exec', 'proc', 'data', 'option', 'optionId', 'selectedApp',
        'notice', 'noticeDelay', 'noSave', 'inputMode', 'optionTimer', 'titleDelay', 'fade', 'pending', 'requiredMask', 'renderedMask', 'variedMask')
    title = dict.fromkeys(boot_names, 0)
    title.update(event='ready', role='title', owner='boot', frame=500, padReads=10, kind=1, outer=60, manager=4, data=8,
        exec=2, proc=2, fade=1, requiredMask=1, renderedMask=1, variedMask=1)
    continuing = {**title, 'role': 'continue', 'frame': 600, 'padReads': 20, 'kind': 2, 'outer': 74, 'proc': 5, 'optionId': 1, 'notice': 15}
    typed_boot = [title, {**title, 'event': 'press'}, {**title, 'event': 'released', 'frame': 504, 'padReads': 12},
        {**title, 'event': 'settled', 'frame': 508, 'padReads': 14}, continuing, {**continuing, 'event': 'press'},
        {**continuing, 'event': 'continue_selected', 'frame': 602, 'padReads': 21, 'proc': 8, 'selectedApp': 1},
        {**continuing, 'event': 'released', 'frame': 604, 'padReads': 22}, {**continuing, 'event': 'settled', 'frame': 608, 'padReads': 24}]
    legacy = [dict(ordinal=0, frame=450, keys=8)]
    assert [row['frame'] for row in approved_boot(legacy, typed_boot)] == [450, 500, 600]
    assert approved_boot([dict(ordinal=0, frame=451, keys=8)], typed_boot)[0]['frame'] == 451
    for invalid_legacy in ([dict(ordinal=0, frame=449, keys=8)], [dict(ordinal=0, frame=450.0, keys=8)],
            [dict(ordinal=0, frame=500, keys=8)], legacy + legacy):
        try:
            approved_boot(invalid_legacy, typed_boot)
        except RuntimeError:
            pass
        else:
            raise AssertionError('An early, mistyped, out-of-order or duplicate START skip was accepted')
    boot_log = '\n'.join('PC Box boot: ' + json.dumps(row) for row in typed_boot)
    assert typed_boot_trace(dict(typedBootTrace=typed_boot), boot_log) == typed_boot
    assert typed_boot_trace(dict(extra=dict(typedBootTrace=typed_boot)), boot_log) == typed_boot
    for invalid_report, invalid_log in (({}, boot_log), (dict(typedBootTrace=[]), boot_log),
            (dict(typedBootTrace=typed_boot), 'PC Box boot: {')):
        try:
            typed_boot_trace(invalid_report, invalid_log)
        except (RuntimeError, json.JSONDecodeError):
            pass
        else:
            raise AssertionError('Missing or malformed canonical boot trace was accepted')
    for index, change in ((5, {'inputMode': 1}), (5, {'fade': 0}), (5, {'requiredMask': True}),
            (6, {'selectedApp': 0}), (7, {'padReads': 21})):
        invalid = [dict(row) for row in typed_boot]; invalid[index].update(change)
        try:
            approved_boot(legacy, invalid)
        except RuntimeError:
            pass
        else:
            raise AssertionError('Invalid typed boot evidence was accepted')
    print('PASS: pure PC binding unchanged/delta banks, QA2/1372, exact scoped field-message/action trace, and step parser; no archive approved or game run.')


if __name__ == '__main__':
    import argparse
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--self-check', action='store_true', required=True)
    parser.parse_args()
    self_check()
