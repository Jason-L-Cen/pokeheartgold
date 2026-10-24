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


def tm_evidence_passed(rows, typed=(), inputs=(), fades=(), raw=()):
    """The new typed contract is intentionally distinct from immutable raw-peripheral evidence."""
    if not isinstance(rows, list) or len(rows) != 1 or not isinstance(rows[0], dict) or not isinstance(raw, list) or raw:
        return False
    row = rows[0]
    exact = dict(status='passed_original_summary_tm_learning', fixturePrepared=1, summaryLifetimes=2,
                 fifthCancelled=1, continuedTrying=1, hmRejected=1, confirmationBack=1, replacement=1,
                 finalMove=182, finalPpUps=0, finalPp=10, finalMaxPp=10, initialTmQuantity=2, finalTmQuantity=1,
                 initialFriendship=70, finalFriendship=71, initialMood=0, finalMood=40,
                 partyFieldsPreserved=True, otherBagBytesPreserved=True, flagsMoneyDexPreserved=True,
                 friendshipMoodPreservedBeforeLearning=True, resetFinished=True, peripheralUiNative=True,
                 exitFadeSeen=1, exitFadeRejected=2)
    if not all(type(row.get(key)) is type(value) and row[key] == value for key, value in exact.items()) \
            or type(row.get('staleRejected')) is not int or row['staleRejected'] < 6:
        return False
    counts = ('typedAccepted', 'typedRejected', 'typedReleased', 'typedSettled', 'partyReturnAcks')
    if any(type(row.get(key)) is not int or row[key] < 0 for key in counts) \
            or not row['typedAccepted'] or not row['partyReturnAcks'] \
            or row['typedAccepted'] != row['typedReleased'] or row['typedAccepted'] != row['typedSettled'] \
            or not isinstance(typed, list) or not typed or not isinstance(inputs, list) or not isinstance(fades, list) or len(fades) != 1:
        return False
    integers = ('sequence', 'stage', 'advance', 'producer', 'revision', 'phase', 'kind', 'index', 'owner', 'generation',
                'qaRevision', 'manager', 'data', 'execState', 'context', 'itemId', 'moveId', 'partySlot', 'selectedMoveSlot',
                'taskState', 'taskMatches', 'printerId', 'printerActive', 'printerMatches', 'printerState', 'printerWaiting',
                'yesNoActive', 'yesNoReady', 'yesNoResult', 'yesNoTimer', 'yesNoX', 'yesNoY', 'yesNoWidth', 'yesNoHeight',
                'partyBusy', 'graphicsReady', 'fadeFinished', 'padReads', 'invalidated', 'keys')
    groups = []
    for event in typed:
        if not isinstance(event, dict) or any(type(event.get(key)) is not int or not 0 <= event[key] <= 0xffffffff for key in integers) \
                or any(not isinstance(event.get(key), list) or len(event[key]) != size
                       or any(type(value) is not int or value < 0 for value in event[key])
                       for key, size in (('touch', 3), ('yesHitX', 2), ('yesHitY', 2))) \
                or not event['sequence'] or not event['revision'] or not event['qaRevision'] \
                or not event['generation'] or not event['manager'] or not event['data'] \
                or event['execState'] != 2 or event['graphicsReady'] != 1 or event['fadeFinished'] != 1 \
                or event['keys'] or event['advance'] not in (0, 1) or event['invalidated'] not in (0, 1):
            return False
        if not groups or groups[-1][0]['sequence'] != event['sequence']:
            if event['sequence'] != len(groups) + 1:
                return False
            groups.append([])
        groups[-1].append(event)
    accepted, rejected, expected_press, expected_release, roles = [], 0, [], [], set()
    last_settled = 0
    identity = (*integers[:-3], 'keys', 'yesHitX', 'yesHitY')  # padReads/invalidated/touch describe each event.
    for group in groups:
        queued = group[0]
        events = [event.get('event') for event in group]
        if events not in (['queued', 'rejected'], ['queued', 'accepted', 'released', 'settled']) \
                or queued['padReads'] < last_settled or queued['invalidated'] or queued['touch'] != [0, 0, 0] \
                or any(any(event[key] != queued[key] for key in identity) for event in group[1:]) \
                or any(group[index]['padReads'] > group[index + 1]['padReads'] for index in range(len(group) - 1)):
            return False
        if events[-1] == 'rejected':
            if group[1]['invalidated'] != 1 or group[1]['touch'] != [0, 0, 0]:
                return False
            rejected += 1
            continue
        _, action, release, settled = group
        if action['invalidated'] or settled['invalidated'] or action['touch'][0] != 1 \
                or not (0 <= action['touch'][1] < 256 and 0 <= action['touch'][2] < 192) \
                or release['touch'] != [0, 0, 0] or settled['touch'] != [0, 0, 0] \
                or not 0 <= release['padReads'] - action['padReads'] <= 4 \
                or (release['padReads'] - action['padReads'] < 4 and not release['invalidated']) \
                or settled['padReads'] - release['padReads'] < 2:
            return False
        producer, phase, kind, index, stage = (action[key] for key in ('producer', 'phase', 'kind', 'index', 'stage'))
        printer = action['printerId'] < 8 and all(action[key] == 1 for key in ('printerActive', 'printerMatches', 'printerWaiting')) \
            and action['printerState'] == 2
        prompt_y = 6 if producer == 3 else 10
        prompt = all(action[key] == 1 for key in ('yesNoActive', 'yesNoReady')) \
            and action['yesNoResult'] == 3 and action['yesNoTimer'] == 0 \
            and (action['yesNoX'], action['yesNoY'], action['yesNoWidth'], action['yesNoHeight']) == (25, prompt_y, 6, 4) \
            and action['yesHitX'] == [224, 224] and action['yesHitY'] == [prompt_y * 8 + 16, prompt_y * 8 + 48]
        role = None
        if producer == 3 and action['owner'] == 1:
            if (phase, stage, kind) in ((1, 2, 0), (1, 3, 1), (4, 4, 2), (1, 20, 5)):
                if (kind == 0 and index < 8) or (kind == 1 and index < 6) or (kind == 2 and index < 4) or (kind == 5 and index == 0):
                    role = {2: 'pocket', 3: 'item', 4: 'use', 20: 'cancel'}[stage]
            elif phase == 13 and kind == 2 and index == 0 and action['taskMatches'] == 1 and action['itemId'] == 344:
                if stage == 5 and action['taskState'] == 1 and action['printerId'] < 8 and not action['printerActive']:
                    role = 'bag-boot-ack'
                elif stage == 6 and action['taskState'] == 2 and printer:
                    role = 'bag-printer'
                elif stage == 6 and action['taskState'] == 3 and prompt:
                    role = 'bag-yes'
        elif producer == 1 and action['owner'] == 2 and not action['partyBusy'] and action['itemId'] == 344 and action['moveId'] == 182:
            if phase == 21 and stage == 7 and kind == 0 and index == 0 and action['context'] == 6 and action['partySlot'] == 0:
                role = 'target'
            elif phase == 24 and stage in (8, 9, 11, 12, 13, 19) and kind == 1 and index == 0 \
                    and action['context'] == (6 if stage in (8, 9) else 7) and printer:
                role = 'party-printer-before' if stage in (8, 9) else 'party-printer-return'
            elif phase == 27 and stage in (8, 11, 12) and kind == 1 and index == (1 if stage == 11 else 0) \
                    and action['context'] == (6 if stage == 8 else 7) and prompt \
                    and (stage != 11 or action['selectedMoveSlot'] == 4):
                role = {8: 'forget-yes', 11: 'stop-no', 12: 'retry-yes'}[stage]
        if role is None or action['advance'] != (0 if 'printer' in role else 1):
            return False
        if role in ('bag-boot-ack', 'bag-printer') and action['touch'] != [1, 124, 24] \
                or role.startswith('party-printer') and action['touch'] != [1, 124, 168] \
                or role in ('bag-yes', 'forget-yes', 'stop-no', 'retry-yes') \
                and action['touch'] != [1, action['yesHitX'][index], action['yesHitY'][index]]:
            return False
        roles.add(role); accepted.append(action); last_settled = settled['padReads']
        expected_press.append((stage, producer, action['revision'], phase, action['padReads'], 0, tuple(action['touch'])))
        expected_release.append((stage + action['advance'], producer, action['revision'], phase, release['padReads'], release['invalidated']))
    required = {'pocket', 'item', 'use', 'cancel', 'bag-boot-ack', 'bag-printer', 'bag-yes', 'target',
                'party-printer-before', 'party-printer-return', 'forget-yes', 'stop-no', 'retry-yes'}
    if not required <= roles or len(accepted) != row['typedAccepted'] or rejected != row['typedRejected'] \
            or sum(action['producer'] == 1 and action['phase'] == 24 and action['stage'] >= 11 for action in accepted) != row['partyReturnAcks']:
        return False
    for event in inputs:
        if not isinstance(event, dict) or event.get('event') not in ('press', 'release') \
                or any(type(event.get(key)) is not int or event[key] < 0 for key in ('stage', 'owner', 'revision', 'phase', 'padReads')) or event['owner'] not in (0, 1, 2, 3):
            return False
        if event['event'] == 'press':
            if type(event.get('keys')) is not int or not isinstance(event.get('touch'), list) or len(event['touch']) != 3 \
                    or any(type(value) is not int or value < 0 for value in event['touch']):
                return False
        elif type(event.get('ownerChanged')) is not int or event['ownerChanged'] not in (0, 1):
            return False
    peripheral = [event for event in inputs if event['owner'] in (1, 3)]
    try:
        presses = [(event['stage'], event['owner'], event['revision'], event['phase'], event['padReads'], event['keys'], tuple(event['touch']))
                   for event in peripheral if event['event'] == 'press']
        releases = [(event['stage'], event['owner'], event['revision'], event['phase'], event['padReads'], event['ownerChanged'])
                    for event in peripheral if event['event'] == 'release']
    except (KeyError, TypeError):
        return False
    if len(peripheral) != len(presses) + len(releases) or sorted(presses) != sorted(expected_press) or sorted(releases) != sorted(expected_release):
        return False
    fade = fades[0]
    if not isinstance(fade, dict) or any(type(fade.get(key)) is not int for key in ('stage', 'owner', 'generation', 'qaRevision', 'revision',
            'phase', 'ready', 'actionCount', 'actionMask', 'enabled', 'oldRevision', 'oldPhase', 'oldKind', 'oldIndex')) \
            or (fade['stage'], fade['owner'], fade['phase'], fade['ready'], fade['actionCount'], fade['actionMask'], fade['enabled'],
                fade['oldPhase'], fade['oldKind'], fade['oldIndex']) != (7, 1, 37, 0, 1, 0, 0, 13, 2, 0) \
            or any(fade.get(key) is not True for key in ('oldChoiceRejected', 'forgedReadyRejected', 'inputUnchanged')):
        return False
    yes = [action for action in accepted if action['producer'] == 3 and action['phase'] == 13 and action['taskState'] == 3]
    target = [action for action in accepted if action['producer'] == 1 and action['phase'] == 21]
    return len(yes) == len(target) == 1 and fade['oldRevision'] == yes[0]['revision'] < fade['revision'] \
        and fade['generation'] == yes[0]['generation'] and fade['qaRevision'] > yes[0]['qaRevision'] \
        and target[0]['generation'] > fade['generation']


def tm_evidence_self_check():
    """Pure in-memory positive trace and targeted negatives; no files, tools, guest or device."""
    from copy import deepcopy
    stages = [(3, 1, 2, 0, 0, 0), (3, 1, 3, 1, 0, 0), (3, 4, 4, 2, 0, 0),
              (3, 13, 5, 2, 0, 1), (3, 13, 6, 2, 0, 2), (3, 13, 6, 2, 0, 3),
              (1, 21, 7, 0, 0, 0), (1, 24, 8, 1, 0, 0), (1, 27, 8, 1, 0, 0), (1, 24, 9, 1, 0, 0),
              (1, 24, 11, 1, 0, 0), (1, 27, 11, 1, 1, 0), (1, 24, 12, 1, 0, 0),
              (1, 27, 12, 1, 0, 0), (1, 24, 13, 1, 0, 0), (1, 24, 19, 1, 0, 0), (3, 1, 20, 5, 0, 0)]
    typed, inputs = [], []
    for sequence, (producer, phase, stage, kind, index, task) in enumerate(stages, 1):
        data = dict(sequence=sequence, stage=stage, advance=int(not (phase == 24 or (phase == 13 and task == 2))),
                    producer=producer, revision=sequence, phase=phase, kind=kind, index=index, owner=1 if producer == 3 else 2,
                    generation=1 if producer == 3 and stage < 7 else 5 if producer == 3 else 2, qaRevision=sequence,
                    manager=100, data=200, execState=2, context=6 if stage in (7, 8, 9) else 7 if producer == 1 else 0,
                    itemId=344, moveId=182, partySlot=0, selectedMoveSlot=4 if stage == 11 else 0,
                    taskState=task, taskMatches=int(producer == 3 and phase == 13), printerId=0,
                    printerActive=int(phase == 24 or task == 2), printerMatches=int(phase == 24 or task == 2),
                    printerState=2 if phase == 24 or task == 2 else 0, printerWaiting=int(phase == 24 or task == 2),
                    yesNoActive=int(phase == 27 or task == 3), yesNoReady=int(phase == 27 or task == 3), yesNoResult=3,
                    yesNoTimer=0, yesNoX=25, yesNoY=6 if producer == 3 else 10, yesNoWidth=6, yesNoHeight=4,
                    yesHitX=[224, 224], yesHitY=[64, 96] if producer == 3 else [96, 128],
                    partyBusy=0, graphicsReady=1, fadeFinished=1, keys=0)
        touch = [1, 124, 168 if producer == 1 else 24] if phase == 24 or (phase == 13 and task in (1, 2)) \
            else [1, 224, data['yesHitY'][index]] if phase == 27 or task == 3 else [1, 50, 50]
        reads = sequence * 8
        for event, offset, point in [('queued', 0, [0, 0, 0]), ('accepted', 1, touch), ('released', 5, [0, 0, 0]), ('settled', 7, [0, 0, 0])]:
            typed.append(dict(data, event=event, padReads=reads + offset, invalidated=0, touch=point))
        inputs += [dict(stage=stage, event='press', owner=producer, revision=sequence, phase=phase, padReads=reads + 1, keys=0, touch=touch),
                   dict(stage=stage + data['advance'], event='release', owner=producer, revision=sequence, phase=phase, padReads=reads + 5, ownerChanged=0)]
    final = dict(status='passed_original_summary_tm_learning', fixturePrepared=1, summaryLifetimes=2, fifthCancelled=1, continuedTrying=1,
                 hmRejected=1, confirmationBack=1, replacement=1, finalMove=182, finalPpUps=0, finalPp=10, finalMaxPp=10,
                 initialTmQuantity=2, finalTmQuantity=1, initialFriendship=70, finalFriendship=71, initialMood=0, finalMood=40,
                 partyFieldsPreserved=True, otherBagBytesPreserved=True, flagsMoneyDexPreserved=True,
                 friendshipMoodPreservedBeforeLearning=True, resetFinished=True, peripheralUiNative=True, staleRejected=7,
                 typedAccepted=len(stages), typedRejected=0, typedReleased=len(stages), typedSettled=len(stages), partyReturnAcks=4,
                 exitFadeSeen=1, exitFadeRejected=2)
    fade = dict(stage=7, owner=1, generation=1, qaRevision=7, revision=7, phase=37, ready=0, actionCount=1, actionMask=0, enabled=0,
                oldRevision=6, oldPhase=13, oldKind=2, oldIndex=0, oldChoiceRejected=True, forgedReadyRejected=True, inputUnchanged=True)
    valid = [[final], typed, inputs, [fade], []]
    assert tm_evidence_passed(*valid)
    stale = deepcopy(valid)
    queued = deepcopy(stale[1][-4]); rejected = dict(queued, event='rejected', padReads=queued['padReads'] + 1, invalidated=1)
    for event in stale[1][-4:]:
        event['sequence'] += 1; event['revision'] += 1; event['qaRevision'] += 1; event['padReads'] += 2
    stale[1][-4:-4] = [queued, rejected]
    for event in stale[2][-2:]:
        event['revision'] += 1; event['padReads'] += 2
    stale[0][0]['typedRejected'] = 1
    assert tm_evidence_passed(*stale)
    # Mutate every copy of a cached request to test its predicate, independently of snapshot equality.
    mutations = [(4, 'printerActive', 1), (5, 'printerWaiting', 0), (6, 'yesNoTimer', 1), (7, 'partyBusy', 1),
                 (8, 'printerMatches', 0), (9, 'yesNoReady', 0), (12, 'index', 0), (14, 'context', 6)]
    for sequence, key, value in mutations:
        bad = deepcopy(valid)
        for event in bad[1]:
            if event['sequence'] == sequence:
                event[key] = value
        assert not tm_evidence_passed(*bad), (sequence, key)
    for kind in ('stale', 'release', 'settled', 'extra-input', 'raw', 'fade', 'legacy'):
        bad = deepcopy(valid)
        if kind == 'stale': bad[1][1]['revision'] += 1
        elif kind == 'release': del bad[1][2]
        elif kind == 'settled': bad[1][3]['padReads'] -= 1
        elif kind == 'extra-input': bad[2].append(deepcopy(bad[2][0]))
        elif kind == 'raw': bad[4].append({'reason': 'raw'})
        elif kind == 'fade': bad[3][0]['ready'] = 1
        else: bad[0][0]['peripheralUiNative'] = False
        assert not tm_evidence_passed(*bad), kind
    return True

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--save', required=True, help='Designated private Route29 fixture; only new copies are opened')
    parser.add_argument('--output-name', default='native-summary-menu-pass', help='New private archive basename; existing output is never overwritten')
    parser.add_argument('--tm-learn', action='store_true', help='Private original TM route; native typed Bag/Party and Summary controls')
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
        report.update(scope='private valid-starter/TM17; original field menu and revision-bound typed Bag/Party route; native Summary fifth/HM/cancel/replace; disabled Bag exit fade and original learning result/reset; no other TM/HM branches, UIKit/device or save-writing claim',
                      tmLearn=True, peripheralInput='revision-bound native typed Bag/Party queues through original resolvers', productionApiAdded=False)
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
                                ('tmRawEvidence', 'Summary TM raw evidence: '), ('tmFixtureEvidence', 'Summary TM fixture evidence: '),
                                ('tmTypedEvidence', 'Summary TM typed evidence: '), ('tmExitFadeEvidence', 'Summary TM exit fade evidence: ')]:
                report[key] = [json.loads(line[len(prefix):]) for line in output.splitlines() if line.startswith(prefix)]
            report['complete'] = exit_code == 0 and len(evidence) == 1 and evidence[0]['status'] == 'passed_original_summary_tm_learning'
            report['passed'] = report['complete'] and tm_evidence_passed(report['tmLearningEvidence'], report['tmTypedEvidence'], report['inputEvidence'],
                                                                                report['tmExitFadeEvidence'], report['tmRawEvidence']) and 'PASS: original TM route and native Summary learning' in output
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
