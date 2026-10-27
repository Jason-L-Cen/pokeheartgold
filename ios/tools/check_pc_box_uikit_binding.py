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
import tempfile

QA_NAMES = ('HGNativePCBoxQAGetState', 'HGNativePCBoxQARequest')
QA_VERSION, QA_BYTES = 3, 1416
QA_RELATIVE = ('ios/tests/pc_box_qa.h', 'ios/tests/pc_box_guest.inc',
    'ios/Original/abi32/pc_box_scenario_check.c', 'ios/tools/check_native_pc_box.py',
    'ios/tests/npc_yesno_qa.h','ios/tests/npc_yesno_boot.inc','ios/tools/npc_yesno_boot_sources.py')
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
    allowed = {str(root / relative) for relative in QA_RELATIVE[1:4]}
    require(not set(delta) - allowed, 'Only the three approved private harness files may differ after Full')
    qa_hashes = {str(root / relative): runtime.get(str(root / relative)) for relative in QA_RELATIVE}
    require(all(isinstance(value, str) and SHA256.fullmatch(value) for value in qa_hashes.values()), 'Runtime bank must bind all PC/shared boot QA files')
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
    require(len(data) == QA_BYTES, 'Native checkpoint must use the actual QA3/1416 PC ABI')
    words = struct.unpack(f'<{QA_BYTES//4}I', data)
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
        naming=dict(zip(('active', 'ownerGeneration', 'revision', 'ready'), words[267:271])), messageAwaitInput=words[342],
        boot=dict(zip(('kind','active','generation','revision','ready','phase','inputMode','option','notice','exit','progress'),words[343:354])))


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
    """Original finite roles; retired phase 8 is the last observed exit milestone."""
    require(isinstance(legacy,list) and len(legacy)==1 and isinstance(legacy[0],dict)
        and type(legacy[0].get('ordinal')) is int and legacy[0]['ordinal']==0
        and type(legacy[0].get('keys')) is int and legacy[0]['keys']==8
        and type(legacy[0].get('frame')) is int and legacy[0]['frame']>=450,
        'Exactly one original START at/after its source threshold is required')
    require(isinstance(rows,list) and rows and all(isinstance(row,dict) and row.get('owner')=='boot'
        and isinstance(row.get('boot'),dict) for row in rows),'Typed boot owner/facts are missing')
    for row in rows:
        require(all(type(row.get(name)) is int and 0<=row[name]<=0xFFFFFFFF
            for name in ('frame','padReads','inputStep','rendered','varied'))
            and all(type(row['boot'].get(name)) is int and 0<=row['boot'][name]<=0xFFFFFFFF for name in
                ('kind','active','generation','revision','ready','phase','inputMode','option','notice','exit','progress')),
            'Typed boot facts must be genuine u32 fields')
    require(all(a['frame']<=b['frame'] and a['padReads']<=b['padReads'] for a,b in zip(rows,rows[1:])),
        'Boot frames/input reads regressed')
    exits=[row for row in rows if row.get('event') in ('continue_selected','field_ready')]
    require([row['event'] for row in exits]==['continue_selected','field_ready'] and rows[-1] is exits[-1],
        'Continue selection and field-ready witnesses are required exactly once')
    leases=[row for row in rows if row not in exits]
    require(len(leases)%4==0,'Typed boot leases must have ready/press/released/settled rows')
    roles=[row.get('role') for row in leases[::4]]
    require(roles in (['title','continue'],['title','mode_switch','continue'],['continue'],['mode_switch','continue']),
        'Finite typed boot role ordering changed')
    result=[{**legacy[0],'role':'start'}]
    previous=None
    for index in range(0,len(leases),4):
        ready,press,release,settled=leases[index:index+4]; role=ready['role']; facts=press['boot']
        require([row.get('event') for row in (ready,press,release,settled)]==['ready','press','released','settled']
            and all(row.get('role')==role for row in (ready,press,release,settled)),
            'Typed role lease event order changed')
        require({k:v for k,v in ready.items() if k!='event'}=={k:v for k,v in press.items() if k!='event'},
            'Readiness differs from admitted physical input')
        require(facts['active']==facts['ready']==1 and facts['generation'] and facts['revision']
            and press['rendered']==press['varied']==1 and facts['exit']==0,
            'Boot A requires a live ready owner and measured original source pixels')
        require(press['frame']>result[-1]['frame'] and release['padReads']-press['padReads']>=2
            and settled['padReads']-release['padReads']>=2,'Boot held/released two-read lease changed')
        if role=='title':
            require(facts['kind']==1 and facts['phase']==2 and not any(facts[n] for n in ('inputMode','option','notice'))
                and facts['progress']&15==15,'Original Title is not ready')
        else:
            require(facts['kind']==2 and facts['phase']==5 and facts['option']<9 and facts['notice']==15
                and facts['inputMode']==(1 if role=='mode_switch' else 0) and facts['progress']&255==255,
                'Original Continue or explicit mode-switch is not ready')
            if previous and previous['role']=='mode_switch':
                require(previous['boot']['generation']==facts['generation'] and previous['boot']['revision']!=facts['revision'],
                    'Mode switch needs a fresh BUTTONS publication from the same owner')
        result.append(dict(ordinal=len(result),frame=press['frame'],keys=1,role=role,boot=facts));previous=press
    confirm=leases[-3]
    for row in exits:
        facts=row['boot']
        require(row['role']=='continue' and facts['kind']==2 and facts['active']==facts['ready']==0
            and facts['generation']==confirm['boot']['generation'] and facts['generation'] and facts['revision']
            and facts['phase']==8 and facts['exit']==1 and row['frame']>=confirm['frame'],
            'Genuine retired Continue milestone is missing')
    require(rows.index(exits[0])>rows.index(confirm) and exits[0]['boot']==exits[1]['boot'],'Retired owner facts changed before field ready')
    return result


def validate_boot_pixels(folder, boot, captures=None):
    for role in (row['role'] for row in boot if row['role']!='start'):
        name=f'frames/boot-{role}-main.ppm'; path=Path(folder)/name
        require(not path.is_symlink() and path.is_file() and (captures is None or name in captures),
            'Actual original boot image is missing: '+name)
        data=path.read_bytes(); header=b'P6\n256 192\n255\n'
        require(data.startswith(header) and len(data)==len(header)+256*192*3,'Invalid original boot PPM dimensions')
        pixels=data[len(header):]
        require(any(pixels[i:i+3]!=pixels[:3] for i in range(3,len(pixels),3)),'Original boot image is uniform')


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
        snapshot_evidence_passed, shipping_exports_passed, normal_checks_passed)
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
        'Native numeric evidence must use exact QA3/u32 values, not Boolean coercions')
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
        and wasm_exports(module.read_bytes()) == sorted([*shipping, *QA_NAMES]), 'Private exports must be exactly current production 30 plus two PC QA exports')
    full_path = Path(report['fullReceipt']); formal = read(full_path); full_folder = full_path.parent
    require(sha(full_path) == report.get('fullReceiptSha256') == preparation.get('fullReceiptSha256')
        and formal.get('passed') is True and formal.get('freshAuditPassed') is True and formal.get('inputsUnchanged') is True
        and type(formal.get('actualNormalCheckKinds')) is int and formal['actualNormalCheckKinds'] == 101
        and normal_checks_passed(formal.get('actualNormalChecks'), formal['actualNormalCheckKinds'])
        and formal.get('shipping', {}).get('sha256') == shipping_sha256
        and shipping_exports_passed(formal.get('shipping', {}).get('exports'), shipping),
        'Full receipt must bind the actual current production30 and 101 normal checks, including FieldMenu')
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
        ('field-pc-box.c', 'field-pc-box.o', 'objects.rsp', 'engine.wasm', 'runtime-input-bank.json',
        'source/src/title_screen.c','source/src/title_screen.o',
        'source/src/application/main_menu/main_menu.c','source/src/application/main_menu/main_menu.o')}
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
        and preparation.get('replacedFormalObjects') == [str(audit / ('objects/'+name)) for name in
            ('src/field_system.o','src/title_screen.o','src/application/main_menu/main_menu.o')]
        and preparation.get('privateObjects') == [str(folder / ('guest/'+name)) for name in
            ('field-pc-box.o','source/src/title_screen.o','source/src/application/main_menu/main_menu.o')],
        'Private guest must replace exactly the unchanged resident and two typed boot source owners')
    from npc_yesno_boot_sources import build_candidates
    with tempfile.TemporaryDirectory(prefix='pc-boot-binding-') as temporary:
        generated=build_candidates(ROOT,Path(temporary)); expected_copies=[]
        for relative,path in zip(('src/title_screen.c','src/application/main_menu/main_menu.c'),generated):
            actual=folder/'guest/source'/relative
            require(actual.read_text()==path.read_text() and sha(ROOT/relative)==cmap[relative],
                'Typed boot copy altered the original source body or declared read-only hook')
            expected_copies.append(dict(original=relative,originalSha256=sha(ROOT/relative),source=str(actual),sourceSha256=sha(actual)))
        require(preparation.get('bootSourceCopies')==expected_copies,'Typed boot source-copy provenance changed')
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
        require(all(row[name] == value for name, value in actual.items()), 'Typed snapshot metadata differs from its actual QA3 state')
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
    validate_boot_pixels(folder,boot,captures)
    runtime_commands = [row['argv'] for row in report['commands'] if row['argv'][0] == str(folder / 'native/pc-box-check')]
    from prepare_native_content import verify
    from import_original_content import read_boot_header
    pack=verify(link.get('nativeContent'))
    require((folder/'boot-header.bin').read_bytes()==read_boot_header(pack),'Actual original boot header changed')
    require(len(runtime_commands) == 1 and runtime_commands[0]==[str(folder/'native/pc-box-check'),
        str(pack/'files'),str(folder/'boot-header.bin'),
        str(folder/'scenario.sav'),str(folder/'frames')], 'Canonical typed-boot runtime argument contract changed')
    resource = dict(version=3, qaVersion=QA_VERSION, qaBytes=QA_BYTES, sourceReportSha256=sha(report_path), privateModuleSha256=sha(module),
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
    runtime = {**historical, **{str(root / name): 'b' * 64 for name in QA_RELATIVE[1:4]}}
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
    for name in QA_RELATIVE[4:]:
        rejected.append(lambda name=name: runtime_bank_provenance(evidence,evidence,historical,
            {**runtime,str(root/name):'e'*64},root,'c'*64,'d'*64))
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
    decoded = decode_qa_snapshot(struct.pack(f'<{QA_BYTES//4}I', *words))
    assert decoded['qaVersion'] == 3 and decoded['qaBytes'] == 1416 and decoded['messageAwaitInput'] == 1
    bad_words = words.copy(); bad_words[342] = 2
    rejected.extend([lambda: decode_qa_snapshot(b'\0' * 1368),
        lambda: decode_qa_snapshot(struct.pack(f'<{QA_BYTES//4}I', *bad_words))])
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
    def lease(role,frame,reads,facts):
        row=dict(event='ready',role=role,owner='boot',frame=frame,padReads=reads,inputStep=1,boot=facts,rendered=1,varied=1)
        return [row,{**row,'event':'press'},{**row,'event':'released','frame':frame+4,'padReads':reads+2},
            {**row,'event':'settled','frame':frame+8,'padReads':reads+4}]
    title=dict(kind=1,active=1,generation=1,revision=2,ready=1,phase=2,inputMode=0,option=0,notice=0,exit=0,progress=15)
    menu=dict(kind=2,active=1,generation=2,revision=5,ready=1,phase=5,inputMode=0,option=0,notice=15,exit=0,progress=255)
    retired={**menu,'active':0,'ready':0,'phase':8,'exit':1,'revision':9}
    exits=[dict(event='continue_selected',role='continue',owner='boot',frame=620,padReads=40,inputStep=1,boot=retired,rendered=1,varied=1),
        dict(event='field_ready',role='continue',owner='boot',frame=640,padReads=50,inputStep=1,boot=retired,rendered=1,varied=1)]
    typed_boot=[*lease('title',500,10,title),*lease('continue',600,30,menu),*exits]
    legacy=[dict(ordinal=0,frame=450,keys=8)]
    assert [row['frame'] for row in approved_boot(legacy,typed_boot)]==[450,500,600]
    assert approved_boot([dict(ordinal=0,frame=451,keys=8)],typed_boot)[0]['frame']==451
    switched=[*lease('title',500,10,title),*lease('mode_switch',560,20,{**menu,'inputMode':1,'revision':4}),
        *lease('continue',600,30,menu),*exits]
    assert [row['role'] for row in approved_boot(legacy,switched)]==['start','title','mode_switch','continue']
    assert [row['role'] for row in approved_boot(legacy,typed_boot[4:])]==['start','continue']
    assert [row['role'] for row in approved_boot(legacy,switched[4:])]==['start','mode_switch','continue']
    rejected_boot=[typed_boot[:-1],typed_boot+exits,typed_boot[:4],[]]
    for index,change in ((5,{'boot':{**menu,'inputMode':1}}),(5,{'boot':{**menu,'active':2}}),
            (5,{'boot':{**menu,'ready':True}}),(5,{'boot':{**menu,'generation':0}}),
            (5,{'boot':{**menu,'revision':0}}),(5,{'boot':{**menu,'option':9}}),
            (5,{'boot':{**menu,'notice':14}}),(5,{'varied':0}),(5,{'owner':'field'}),
            (8,{'boot':{**retired,'phase':7}}),(8,{'boot':{**retired,'active':1}}),
            (8,{'boot':{**retired,'exit':0}}),(6,{'padReads':31})):
        rejected_boot.append([{**row,**change} if i==index else row for i,row in enumerate(typed_boot)])
    for bit in range(4):
        rejected_boot.append([{**row,'boot':{**title,'progress':15^(1<<bit)}} if i<2 else row for i,row in enumerate(typed_boot)])
    for bit in range(8):
        rejected_boot.append([{**row,'boot':{**menu,'progress':255^(1<<bit)}} if 4<=i<6 else row for i,row in enumerate(typed_boot)])
    for field,value in (('generation',3),('revision',4)):
        rejected_boot.append([{**row,'boot':{**menu,field:value}} if 8<=i<10 else row for i,row in enumerate(switched)])
    for invalid in rejected_boot:
        try: approved_boot(legacy,invalid)
        except RuntimeError: pass
        else: raise AssertionError('Invalid typed boot owner/mode/phase/progress/lease accepted')
    for invalid in ([dict(ordinal=0,frame=449,keys=8)],[dict(ordinal=0,frame=True,keys=8)],
            [dict(ordinal=0,frame=500,keys=8)],legacy+legacy):
        try: approved_boot(invalid,typed_boot)
        except RuntimeError: pass
        else: raise AssertionError('Invalid original START threshold/order/count accepted')
    boot_log='\n'.join('PC Box boot: '+json.dumps(row) for row in typed_boot)
    assert typed_boot_trace(dict(typedBootTrace=typed_boot),boot_log)==typed_boot
    assert typed_boot_trace(dict(extra=dict(typedBootTrace=typed_boot)),boot_log)==typed_boot
    for report,log in (({},boot_log),(dict(typedBootTrace=[]),boot_log),(dict(typedBootTrace=typed_boot),'PC Box boot: {')):
        try: typed_boot_trace(report,log)
        except (RuntimeError,json.JSONDecodeError): pass
        else: raise AssertionError('Missing/malformed canonical typed boot log accepted')
    with tempfile.TemporaryDirectory(prefix='pc-boot-pixels-') as temporary:
        folder=Path(temporary);(folder/'frames').mkdir();boot=approved_boot(legacy,switched)
        header=b'P6\n256 192\n255\n';pixels=b'\0\1\2'+bytes(256*192*3-3)
        for role in ('title','mode_switch','continue'): (folder/f'frames/boot-{role}-main.ppm').write_bytes(header+pixels)
        validate_boot_pixels(folder,boot)
        target=folder/'frames/boot-continue-main.ppm'
        for invalid in (header+bytes(256*192*3),header+pixels[:-1],b'P6\n1 1\n255\n'+pixels):
            target.write_bytes(invalid)
            try: validate_boot_pixels(folder,boot)
            except RuntimeError: pass
            else: raise AssertionError('Uniform/truncated/wrong-size boot image accepted')
        target.unlink()
        try: validate_boot_pixels(folder,boot)
        except RuntimeError: pass
        else: raise AssertionError('Missing boot image accepted')
    print('PASS: PC QA3/1416, exact source/QA banks, typed boot lifecycle/finite input/PPM negatives and scoped field-message/action parser; no archive approved or game run.')


if __name__ == '__main__':
    import argparse
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--self-check', action='store_true', required=True)
    parser.parse_args()
    self_check()
