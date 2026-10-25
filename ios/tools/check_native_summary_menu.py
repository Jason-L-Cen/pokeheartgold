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
from import_original_content import read_boot_header, narc
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

def candy_evidence_passed(report):
    """Pure contract for one original Rare Candy route; outer seals/provenance remain caller guards."""
    if not isinstance(report, dict) or report.get('rareCandyLearn') is not True:
        return False
    finals=report.get('candyLearningEvidence',[]);fixtures=report.get('candyFixtureEvidence',[])
    stats=report.get('candyStatsEvidence',[]);expected=report.get('candyExpectedStatsEvidence',[])
    typed=report.get('candyTypedEvidence',[]);inputs=report.get('inputEvidence',[]);states=report.get('candyStateEvidence',[])
    if not all(isinstance(rows,list) for rows in (finals,fixtures,stats,expected,typed,inputs,states)) \
            or len(finals)!=1 or len(fixtures)!=1 or len(stats)!=2 or len(expected)!=1 or not typed or not states:
        return False
    if any(not isinstance(row,dict) for rows in (finals,fixtures,stats,expected,typed,inputs,states) for row in rows) \
            or not isinstance(report.get('capturedFiles'),dict) or any(not isinstance(path,str) for path in report['capturedFiles']):
        return False
    final,fixture,model=finals[0],fixtures[0],expected[0]
    exact=dict(status='passed_original_summary_candy_learning',fixturePrepared=1,summaryLifetimes=2,fifthCancelled=1,
        continuedTrying=1,hmRejected=1,confirmationBack=1,replacement=1,cancelKeptUpgrade=1,statsMask=3,statsStaleRejected=1,
        busyForgedRejected=2,wrongContinuationRejected=1,species=152,level=6,experience=179,finalMove=75,finalPpUps=0,finalPp=25,finalMaxPp=25,
        initialCandyQuantity=2,finalCandyQuantity=1,friendship=75,mood=40,originalIdentityPreserved=True,
        otherBagBytesPreserved=True,flagsMoneyDexPreserved=True,resetFinished=True,peripheralUiNative=True)
    if not all(type(final.get(key)) is type(value) and final[key]==value for key,value in exact.items()):
        return False
    if any(type(final.get(key)) is not int or final[key]<0 for key in ('staleRejected','typedAccepted','typedRejected','typedReleased','typedSettled','partyReturnAcks')) \
            or final['staleRejected']<6 or not final['typedAccepted'] or not final['partyReturnAcks'] \
            or final['typedAccepted']!=final['typedReleased'] or final['typedAccepted']!=final['typedSettled']:
        return False
    if fixture.get('moves')!=[33,45,115,15] or fixture.get('item')!=50 or fixture.get('quantity')!=2 or fixture.get('species')!=152 or fixture.get('partyCount')!=1 \
            or fixture.get('fixtureUsesExistingStarter') is not True or fixture.get('otNameProfileMatched') is not True \
            or fixture.get('preparedBeforeFirstFieldAction') is not True or fixture.get('privateSaveMemoryOnly') is not True \
            or fixture.get('initialFriendship')!=70 or fixture.get('initialMood')!=0 or fixture.get('eggLocation')!=0 \
            or type(fixture.get('otNameLength')) is not int or not 1<=fixture['otNameLength']<=7:
        return False
    semantics_before=report.get('semanticEvidence')
    if not isinstance(semantics_before,list) or any(not isinstance(row,dict) for row in semantics_before):return False
    before=[row for row in semantics_before if row.get('step')=='before']
    if len(before)!=1 or not isinstance(before[0].get('mons'),list) or len(before[0]['mons'])!=1 \
            or not isinstance(before[0]['mons'][0],list) or len(before[0]['mons'][0])!=26 \
            or any(type(value) is not int or value<0 for value in before[0]['mons'][0]):
        return False
    mon=before[0]['mons'][0]
    if mon[:2]!=[fixture.get('originalPersonality'),fixture.get('originalOtId')] or mon[2]!=152 or mon[6:9]!=[20,20,5] or mon[10:14]!=[33,45,115,15]:
        return False
    if model.get('expectedExperience')!=179 or type(model.get('initialExperience')) is not int or not 135<=model['initialExperience']<179:
        return False
    if any(not isinstance(row,list) or len(row)!=6 or any(type(value) is not int or value<=0 for value in row)
           for row in (model.get('initial'),model.get('expected'),final.get('stats'),final.get('initialStats'),final.get('expectedStats'))):
        return False
    if final['stats']!=final['expectedStats'] or final['expectedStats']!=model['expected'] or final['initialStats']!=model['initial']:
        return False
    for i,row in enumerate(stats):
        if any(type(row.get(key)) is not int for key in ('stage','role','revision','qaRevision','generation','owner','context','itemId','phase','taskState','ready','graphicsReady','fadeFinished','bytes','staleDiffRejected')):
            return False
        if (row['stage'],row['role'],row['owner'],row['context'],row['itemId'],row['phase'],row['taskState'],row['ready'],row['graphicsReady'],row['fadeFinished'],row['bytes'],row['staleDiffRejected']) \
                !=(6+i,39+i,2,5,50,5,1+i,1,1,1,128*112*4,i) or row.get('crop')!=[0,0,128,112] \
                or not row['revision'] or not row['qaRevision'] or not row['generation']:
            return False
        display=row.get('displayValues')
        if not isinstance(display,list) or len(display)!=6 or any(type(value) is not int or value<0 for value in display) \
                or display!=([b-a for a,b in zip(model['initial'],model['expected'])] if i==0 else model['expected']):
            return False
        name='candy-stats-'+('diff' if i==0 else 'total')+'.rgba'
        if row.get('file')!=name or len([path for path in report.get('capturedFiles',{}) if path.endswith('/'+name)])!=1:
            return False
    if stats[1]['revision']<=stats[0]['revision'] or stats[1]['qaRevision']<=stats[0]['qaRevision'] or stats[0]['generation']!=stats[1]['generation']:
        return False
    groups=[]
    for event in typed:
        if not isinstance(event,dict) or type(event.get('sequence')) is not int or event['sequence']<=0 \
                or type(event.get('invalidated')) is not int or event['invalidated'] not in (0,1) \
                or not isinstance(event.get('touch'),list) or len(event['touch'])!=3 or any(type(value) is not int or value<0 for value in event['touch']):
            return False
        if not groups or groups[-1][0]['sequence']!=event['sequence']:
            if event['sequence']!=len(groups)+1:return False
            groups.append([])
        groups[-1].append(event)
    accepted=[];rejected=0;presses=[];releases=[];seen=set();last_settled=0
    variable={'event','touch','padReads','invalidated'}
    for group in groups:
        events=[row.get('event') for row in group];queued=group[0]
        if events not in (['queued','rejected'],['queued','accepted','released','settled']) or queued.get('touch')!=[0,0,0] or queued.get('invalidated')!=0:
            return False
        if any({k:v for k,v in row.items() if k not in variable}!={k:v for k,v in queued.items() if k not in variable} for row in group):
            return False
        numeric=('stage','advance','producer','revision','phase','kind','index','role','owner','generation','qaRevision','manager','data','execState','context','itemId','moveId','partySlot','selectedMoveSlot','taskState','printerId','printerActive','printerMatches','printerState','printerWaiting','yesNoActive','yesNoReady','yesNoResult','yesNoTimer','partyBusy','graphicsReady','fadeFinished','keys')
        if any(type(queued.get(key)) is not int or queued[key]<0 for key in numeric) or not queued['revision'] or not queued['qaRevision'] \
                or not queued['generation'] or not queued['manager'] or not queued['data'] or queued['execState']!=2 or queued['graphicsReady']!=1 or queued['fadeFinished']!=1 or queued['keys']:
            return False
        if any(type(row.get('padReads')) is not int or row['padReads']<last_settled for row in group) \
                or [row['padReads'] for row in group]!=sorted(row['padReads'] for row in group):return False
        if events[-1]=='rejected':
            if group[1].get('invalidated')!=1 or group[1].get('touch')!=[0,0,0]:return False
            rejected+=1;continue
        _,a,r,s=group
        if a['invalidated'] or s['invalidated'] or r['invalidated'] not in (0,1) or not isinstance(a.get('touch'),list) or len(a['touch'])!=3 \
                or any(type(v) is not int for v in a['touch']) or a['touch'][0]!=1 or not 0<=a['touch'][1]<256 or not 0<=a['touch'][2]<192 \
                or r['touch']!=[0,0,0] or s['touch']!=[0,0,0] or not 0<=r['padReads']-a['padReads']<=4 \
                or (r['padReads']-a['padReads']<4 and not r['invalidated']) or s['padReads']-r['padReads']<2:
            return False
        producer,phase,stage,kind,index,role=(a[k] for k in ('producer','phase','stage','kind','index','role'))
        valid=False;advance=1;label=None
        if producer==3 and a['owner']==1:
            valid=role==0 and ((phase,stage,kind)==(1,2,0) and index<8 or (phase,stage,kind)==(1,3,1) and index<6 \
                or (phase,stage,kind)==(4,4,2) and index<4 or (phase,stage,kind,index)==(1,20,5,0))
            label={2:'pocket',3:'item',4:'use',20:'cancel'}.get(stage)
        elif producer==1 and a['owner']==2 and not a['partyBusy'] and (a['context'],a['itemId']) in ((5,50),(8,0)) and (a['context']!=8 or a['moveId']==75):
            if phase==4:valid=(stage,kind,index,role,a['context'],a['partySlot'])==(5,0,0,37,5,0);label='target'
            elif phase==5 and role in (39,40):
                valid=(stage,kind,index,a['context'],a['taskState'])==(role-33,1,0,5,role-38);label='stats'+str(role)
                valid=valid and a['touch']==[1,124,168] and any(row['role']==role and row['revision']==a['revision'] and row['qaRevision']==a['qaRevision'] for row in stats)
            elif role==38 and kind==1 and not index and (phase==24 or phase==5 and a['taskState'] in (0,4)):
                valid=a['printerId']<8 and a['printerActive']==a['printerMatches']==a['printerWaiting']==1 and a['printerState']==2 and a['touch']==[1,124,168]
                advance=0;label='returnAck' if a['context']==8 else 'initialAck'
            elif phase==27 and stage in (8,11,12):
                valid=kind==1 and index==(1 if stage==11 else 0) and role==41+index and a['moveId']==75 \
                    and a['context']==(5 if stage==8 else 8) and a['yesNoReady']==a['yesNoActive']==1 and a['yesNoResult']==3 and not a['yesNoTimer'] \
                    and (stage!=11 or a['selectedMoveSlot']==4) and a['touch']==[1,224,96+32*index] \
                    and (a.get('yesNoX'),a.get('yesNoY'),a.get('yesNoWidth'),a.get('yesNoHeight'))==(25,10,6,4) \
                    and a.get('yesHitX')==[224,224] and a.get('yesHitY')==[96,128]
                label={8:'yes',11:'no',12:'retry'}[stage]
        if not valid or a['advance']!=advance:return False
        seen.add(label);accepted.append(a);last_settled=s['padReads']
        presses.append((stage,producer,a['revision'],phase,a['padReads'],0,tuple(a['touch'])))
        releases.append((stage+advance,producer,a['revision'],phase,r['padReads'],r['invalidated']))
    needed={'pocket','item','use','cancel','target','stats39','stats40','initialAck','returnAck','yes','no','retry'}
    if not needed<=seen or len(accepted)!=final['typedAccepted'] or rejected!=final['typedRejected'] \
            or sum(a['producer']==1 and a['stage']>=11 and (a['phase']==24 or a['phase']==5 and a['taskState']==4) for a in accepted)!=final['partyReturnAcks']:
        return False
    peripheral=[row for row in inputs if row.get('owner') in (1,3)]
    try:
        actual_press=[(r['stage'],r['owner'],r['revision'],r['phase'],r['padReads'],r['keys'],tuple(r['touch'])) for r in peripheral if r['event']=='press']
        actual_release=[(r['stage'],r['owner'],r['revision'],r['phase'],r['padReads'],r['ownerChanged']) for r in peripheral if r['event']=='release']
    except (KeyError,TypeError):return False
    if len(peripheral)!=len(actual_press)+len(actual_release) or sorted(actual_press)!=sorted(presses) or sorted(actual_release)!=sorted(releases):
        return False
    if not any(row.get('owner')==2 and row.get('context')==8 and row.get('itemId')==0 and row.get('moveId')==75 for row in states):
        return False
    semantics=report.get('candySemanticEvidence',[])
    if not isinstance(semantics,list) or any(not isinstance(row,dict) for row in semantics):return False
    if not any(row.get('stage')==11 and row.get('upgraded')==1 and row.get('learned')==0 and row.get('level')==6
               and row.get('experience')==179 and row.get('quantity')==1 and row.get('friendship')==75 and row.get('mood')==40
               and row.get('personality')==fixture['originalPersonality'] and row.get('otId')==fixture['originalOtId'] for row in semantics):
        return False
    boots=report.get('candyBootEvidence',[])
    if not isinstance(boots,list) or any(not isinstance(row,dict) for row in boots):return False
    return any(row.get('qaObserved') is True and row.get('qaVersion')==1 and row.get('qaBytes')==232 and row.get('fieldReady')==1 \
        and row.get('owner')==0 and row.get('inputStep')==3 and row.get('inputFrame')==0 for row in boots)

def candy_evidence_self_check():
    """Pure synthetic trace only; no files/tools/guest, and never gameplay evidence."""
    from copy import deepcopy
    # producer,phase,stage,kind,index,task,role; mirror both original printer owners.
    actions=[(3,1,2,0,0,0,0),(3,1,3,1,0,0,0),(3,4,4,2,0,0,0),
             (1,4,5,0,0,0,37),(1,5,6,1,0,0,38),(1,5,6,1,0,1,39),
             (1,5,7,1,0,2,40),(1,24,8,1,0,4,38),(1,27,8,1,0,4,41),
             (1,24,11,1,0,4,38),(1,27,11,1,1,4,42),(1,24,12,1,0,4,38),
             (1,27,12,1,0,4,41),(1,5,13,1,0,4,38),(1,5,19,1,0,4,38),(3,1,20,5,0,0,0)]
    typed=[];inputs=[];stats=[]
    initial=[20,11,12,11,12,10];expected=[22,12,14,12,14,11]
    for sequence,(producer,phase,stage,kind,index,task,role) in enumerate(actions,1):
        printer=role==38;prompt=role in (41,42);context=5 if stage<=8 else 8
        row=dict(sequence=sequence,stage=stage,advance=int(not printer),producer=producer,revision=sequence,phase=phase,
            kind=kind,index=index,role=role,owner=2 if producer==1 else 1,generation=2,qaRevision=sequence,
            manager=100,data=200,execState=2,context=context if producer==1 else 0,
            itemId=50 if context==5 else 0,moveId=75,partySlot=0,selectedMoveSlot=4 if stage==11 else 0,
            taskState=task,taskMatches=0,printerId=0,printerActive=int(printer),printerMatches=int(printer),
            printerWaiting=int(printer),printerState=2 if printer else 0,yesNoActive=int(prompt),yesNoReady=int(prompt),
            yesNoResult=3,yesNoTimer=0,yesNoX=25,yesNoY=10,yesNoWidth=6,yesNoHeight=4,
            yesHitX=[224,224],yesHitY=[96,128],partyBusy=0,graphicsReady=1,fadeFinished=1,keys=0)
        touch=[1,224,96+32*index] if prompt else [1,124,168] if producer==1 and kind==1 else [1,50,50]
        for event,offset,point in [('queued',0,[0,0,0]),('accepted',1,touch),('released',5,[0,0,0]),('settled',7,[0,0,0])]:
            typed.append(dict(row,event=event,padReads=sequence*8+offset,invalidated=0,touch=point))
        inputs += [dict(stage=stage,event='press',owner=producer,revision=sequence,phase=phase,padReads=sequence*8+1,keys=0,touch=touch),
                   dict(stage=stage+row['advance'],event='release',owner=producer,revision=sequence,phase=phase,padReads=sequence*8+5,ownerChanged=0)]
        if role in (39,40):
            stats.append(dict(stage=stage,role=role,revision=sequence,qaRevision=sequence,generation=2,owner=2,context=5,itemId=50,phase=5,
                taskState=task,ready=1,graphicsReady=1,fadeFinished=1,bytes=57344,staleDiffRejected=role-39,
                crop=[0,0,128,112],file='candy-stats-'+('diff' if role==39 else 'total')+'.rgba',
                displayValues=[b-a for a,b in zip(initial,expected)] if role==39 else expected))
    final=dict(status='passed_original_summary_candy_learning',fixturePrepared=1,summaryLifetimes=2,fifthCancelled=1,continuedTrying=1,
        hmRejected=1,confirmationBack=1,replacement=1,cancelKeptUpgrade=1,statsMask=3,statsStaleRejected=1,busyForgedRejected=2,
        wrongContinuationRejected=1,species=152,level=6,experience=179,finalMove=75,finalPpUps=0,finalPp=25,finalMaxPp=25,
        initialCandyQuantity=2,finalCandyQuantity=1,friendship=75,mood=40,originalIdentityPreserved=True,otherBagBytesPreserved=True,
        flagsMoneyDexPreserved=True,resetFinished=True,peripheralUiNative=True,staleRejected=7,typedAccepted=len(actions),
        typedRejected=0,typedReleased=len(actions),typedSettled=len(actions),partyReturnAcks=4,stats=expected,initialStats=initial,expectedStats=expected)
    mon=[0]*26;mon[:3]=[123,456,152];mon[6:9]=[20,20,5];mon[10:14]=[33,45,115,15]
    fixture=dict(moves=[33,45,115,15],item=50,quantity=2,species=152,partyCount=1,fixtureUsesExistingStarter=True,
        otNameProfileMatched=True,otNameLength=5,originalPersonality=123,originalOtId=456,preparedBeforeFirstFieldAction=True,
        privateSaveMemoryOnly=True,initialFriendship=70,initialMood=0,eggLocation=0)
    valid=dict(rareCandyLearn=True,candyLearningEvidence=[final],candyFixtureEvidence=[fixture],candyStatsEvidence=stats,
        candyExpectedStatsEvidence=[dict(initialExperience=135,expectedExperience=179,initial=initial,expected=expected)],
        candyTypedEvidence=typed,inputEvidence=inputs,candyStateEvidence=[dict(owner=2,context=8,itemId=0,moveId=75)],
        semanticEvidence=[dict(step='before',mons=[mon])],
        candySemanticEvidence=[dict(stage=11,upgraded=1,learned=0,level=6,experience=179,quantity=1,friendship=75,mood=40,personality=123,otId=456)],
        candyBootEvidence=[dict(qaObserved=True,qaVersion=1,qaBytes=232,fieldReady=1,owner=0,inputStep=3,inputFrame=0)],
        capturedFiles={'/synthetic/'+row['file']:'0'*64 for row in stats})
    assert candy_evidence_passed(valid)
    # Predicate mutations change the whole captured request, so snapshot equality cannot mask a weak gate.
    for sequence,key,value in [(4,'context',8),(5,'printerWaiting',0),(6,'taskState',3),(7,'role',43),
                               (9,'yesNoTimer',1),(11,'selectedMoveSlot',0),(13,'itemId',50),(14,'printerMatches',0)]:
        bad=deepcopy(valid)
        for event in bad['candyTypedEvidence']:
            if event['sequence']==sequence:event[key]=value
        assert not candy_evidence_passed(bad),(sequence,key)
    for kind in ('stale','settle','missing-release','extra-input','stats-value','stats-file','double-consume','cancel-level','identity','boot','float'):
        bad=deepcopy(valid)
        if kind=='stale':bad['candyTypedEvidence'][1]['revision']+=1
        elif kind=='settle':bad['candyTypedEvidence'][3]['padReads']-=1
        elif kind=='missing-release':del bad['candyTypedEvidence'][2]
        elif kind=='extra-input':bad['inputEvidence'].append(deepcopy(bad['inputEvidence'][0]))
        elif kind=='stats-value':bad['candyStatsEvidence'][0]['displayValues'][0]+=1
        elif kind=='stats-file':bad['capturedFiles'].clear()
        elif kind=='double-consume':bad['candyLearningEvidence'][0]['finalCandyQuantity']=0
        elif kind=='cancel-level':bad['candySemanticEvidence'][0]['level']=5
        elif kind=='identity':bad['candyFixtureEvidence'][0]['originalPersonality']+=1
        elif kind=='boot':bad['candyBootEvidence'][0]['inputStep']=2
        else:bad['candyLearningEvidence'][0]['level']=6.0
        assert not candy_evidence_passed(bad),kind
    return dict(syntheticPositive=1,targetedNegatives=19,gameplayEvidence=False)


def evolution_evidence_passed(report):
    """Pure native Evolution evidence predicate; no build, file read or UIKit/particle claim."""
    if not isinstance(report,dict) or report.get('evolution') is not True or type(report.get('evolutionCancel')) is not bool:return False
    cancel=report['evolutionCancel'];status='passed_original_evolution_'+('cancel' if cancel else 'success')
    names=('evolutionEvidence','evolutionFixtureEvidence','evolutionSemanticEvidence','evolutionExpectedStatsEvidence','evolutionStateEvidence','evolutionFrameEvidence','evolutionTypedEvidence','candyTypedEvidence','candyStatsEvidence','candyBootEvidence','inputEvidence','semanticEvidence','evidence')
    if any(not isinstance(report.get(name),list) or any(not isinstance(row,dict) for row in report[name]) for name in names):return False
    if any(len(report[name])!=count for name,count in (('evolutionEvidence',1),('evolutionFixtureEvidence',1),('evolutionSemanticEvidence',2),('evolutionExpectedStatsEvidence',1),('candyStatsEvidence',2),('semanticEvidence',1),('evidence',1))):return False
    def exact(row,values):return all(type(row.get(k)) is type(v) and row[k]==v for k,v in values.items())
    def uint(value):return type(value) is int and 0<=value<2**32
    boot=report['candyBootEvidence']
    if not boot or not any(exact(row,dict(map=33,inputStep=3,inputFrame=0,qaObserved=True,qaVersion=1,qaBytes=232,fieldReady=1,owner=0,fadeFinished=1)) for row in boot):return False
    final=report['evolutionEvidence'][0];fixture=report['evolutionFixtureEvidence'][0];model=report['evolutionExpectedStatsEvidence'][0]
    if not exact(final,dict(status=status,cancelCase=cancel,fixturePrepared=1,initialLevel=15,initialExperience=2035,level=16,experience=2535,
        species=152 if cancel else 153,quantity=1,cancelAccepted=int(cancel),ackAccepted=1,rejected=0,initialFieldViewWidth=384,fieldViewWidth=384,statsMask=3,
        outerTypedRejected=0,summaryLifetimes=0,choiceMenus=0,particleContributionSeparated=False,resetFinished=True,privateSaveWrites=False)):return False
    for key in ('queued','accepted','released','settled','geometryFrames','oscillateFrames','firstGeometrySerial','lastGeometrySerial','outerTypedAccepted','outerTypedReleased','outerTypedSettled','personality','otId'):
        if not uint(final.get(key)):return False
    if not final['queued']==final['accepted']==final['released']==final['settled']==1+int(cancel) or final['oscillateFrames']!=2 or final['geometryFrames']<2 \
        or not final['firstGeometrySerial'] or final['firstGeometrySerial']==final['lastGeometrySerial'] \
        or not final['outerTypedAccepted']==final['outerTypedReleased']==final['outerTypedSettled']>=7:return False
    if not exact(fixture,dict(preparedBeforeFirstFieldAction=True,species=152,partyCount=1,initialLevel=15,initialExperience=2035,item=50,quantity=2,
        initialFriendship=70,initialMood=0,eggLocation=0,fixtureUsesExistingStarter=True,otNameProfileMatched=True,privateSaveMemoryOnly=True)) \
        or fixture.get('moves')!=[33,45,0,0] or not uint(fixture.get('otNameLength')) or not 1<=fixture['otNameLength']<=7 \
        or not uint(fixture.get('mapSection')) or not fixture['mapSection'] \
        or not uint(fixture.get('originalPersonality')) or not uint(fixture.get('originalOtId')) \
        or [fixture.get('originalPersonality'),fixture.get('originalOtId')]!=[final['personality'],final['otId']]:return False
    baseline=report['semanticEvidence'][0]
    if baseline.get('step')!='before' or not isinstance(baseline.get('mons'),list) or len(baseline['mons'])!=1:return False
    mon=baseline['mons'][0]
    if not isinstance(mon,list) or len(mon)!=26 or any(not uint(v) for v in mon) or mon[:3]!=[final['personality'],final['otId'],152] \
        or mon[6]!=mon[7] or mon[8]!=15 or mon[10:14]!=[33,45,0,0]:return False
    if not exact(model,dict(initialExperience=2035,expectedExperience=2535)) \
        or any(not isinstance(model.get(key),list) or len(model[key])!=6 or any(not uint(v) or not v for v in model[key]) for key in ('initial','expected')) \
        or model['initial'][0]!=mon[7]:return False
    for checkpoint,semantic in enumerate(report['evolutionSemanticEvidence']):
        if not exact(semantic,dict(species=152 if cancel else 153,level=16,experience=2535,quantity=1,friendship=75,
            mood=40 if not checkpoint or cancel else 0,fieldReturned=bool(checkpoint),
            personality=final['personality'],otId=final['otId'],bayleefSeen=int(not cancel),bayleefCaught=int(not cancel),
            dexSeenDelta=int(not cancel),dexCaughtDelta=int(not cancel),otherBagBytesPreserved=True,flagsMoneyPreserved=True,ivEvPreserved=True)) \
            or not isinstance(semantic.get('stats'),list) or len(semantic['stats'])!=6 or any(not uint(v) or not v for v in semantic['stats']) \
            or semantic.get('hp')!=semantic['stats'][0] or semantic.get('maxHp')!=semantic['stats'][0]:return False
    stable=lambda row:{k:v for k,v in row.items() if k not in ('mood','fieldReturned')}
    if stable(report['evolutionSemanticEvidence'][0])!=stable(report['evolutionSemanticEvidence'][1]):return False
    if not exact(report['evidence'][0],dict(status=status,seenPhases=0,summaryLifetimes=0,activeAfterReturn=0,resetFinished=True,pendingDMA=0,
        partyItemsMoneyFlagsUnchanged=False,expectedPrivateEvolutionMutation=True)):return False
    states=report['evolutionStateEvidence']
    if not states or any(any(not uint(row.get(k)) for k in ('frame','revision','phase','active','ready','summaryActive','flags','species','newSpecies','moveToLearn','actionMask')) \
        or row['active']!=1 or row['ready'] not in (0,1) or row['summaryActive'] or row['species']!=152 or row['newSpecies']!=153 or row['moveToLearn'] \
        or not row['revision'] or row['phase']>45 or row['flags']&~7 or not row['flags']&1 or row['actionMask']&~15 for row in states):return False
    if 8 not in {row['phase'] for row in states} or not (any(41<=row['phase']<=45 for row in states) if cancel else any(row['phase']==12 for row in states)):return False
    if [r['revision'] for r in states]!=sorted(r['revision'] for r in states):return False
    files=report.get('capturedFiles')
    if not isinstance(files,dict) or any(not isinstance(k,str) or not isinstance(v,str) or len(v)!=64 for k,v in files.items()):return False
    def captured(name):
        hits=[v for k,v in files.items() if k.rsplit('/',1)[-1]==name]
        return hits[0] if len(hits)==1 else None
    frames=report['evolutionFrameEvidence'];osc=[row for row in frames if row.get('phase')==8]
    if len(frames)!=final['geometryFrames'] or len(osc)!=2 or osc[0].get('geometrySerial')==osc[1].get('geometrySerial'):return False
    if any(not exact(row,dict(width=256,height=192,bytes=256*192*4,nonuniform=True)) \
        or any(not uint(row.get(k)) or not row[k] for k in ('revision','frame','vertices','polygons','geometrySerial','geometrySerialAfter')) \
        or row['geometrySerial']!=row['geometrySerialAfter'] or not isinstance(row.get('file'),str) or not captured(row['file']) for row in frames):return False
    if captured(osc[0]['file'])==captured(osc[1]['file']) or not captured('evolution-field-before.rgba') or not captured('evolution-field-return.rgba'):return False
    for i,row in enumerate(report['candyStatsEvidence']):
        if not exact(row,dict(stage=6+i,role=39+i,owner=2,context=5,itemId=50,phase=5,taskState=1+i,ready=1,graphicsReady=1,fadeFinished=1,bytes=128*112*4,staleDiffRejected=i)) \
            or row.get('displayValues')!=([b-a for a,b in zip(model['initial'],model['expected'])] if not i else model['expected']) \
            or not captured(row.get('file')):return False
    evolution=report['evolutionTypedEvidence'];expected_kinds=[1,0] if cancel else [0]
    if len(evolution)!=4*len(expected_kinds):return False
    evo_press=[];evo_release=[]
    for i,kind in enumerate(expected_kinds):
        q,a,r,s=evolution[i*4:i*4+4]
        if [row.get('event') for row in (q,a,r,s)]!=['queued','accepted','released','settled'] \
            or any(not exact(row,dict(sequence=i+1,kind=kind,index=0,phase=8 if kind else 44 if cancel else 12)) for row in (q,a,r,s)) \
            or any(not uint(row.get(k)) for row in (q,a,r,s) for k in ('revision','padReads')) \
            or len({row['revision'] for row in (q,a,r,s)})!=1 or not q['revision'] \
            or not q['padReads']<=a['padReads']<=r['padReads'] or s['padReads']-r['padReads']<2 \
            or not uint(r.get('ownerChanged')) or r['ownerChanged'] not in (0,1) or (not r['ownerChanged'] and r['padReads']-a['padReads']<4) \
            or not exact(a,dict(keys=2 if kind else 1,actionMask=2 if kind else 1)) or not uint(a.get('flags')) or not a['flags']&1 \
            or a.get('touch')!=[0,0,0] or q.get('keys')!=a['keys'] or q.get('touch')!=a['touch']:return False
        evo_press.append((4,a['revision'],a['phase'],a['padReads'],a['keys'],tuple(a['touch'])))
        evo_release.append((4,r['revision'],r['phase'],r['padReads']))
    outer=report['candyTypedEvidence'];count=final['outerTypedAccepted']
    if len(outer)!=count*4:return False
    outer_press=[];outer_release=[];seen_roles=set();seen_kinds={1:set(),3:set()}
    for i in range(count):
        q,a,r,s=outer[i*4:i*4+4]
        if [row.get('event') for row in (q,a,r,s)]!=['queued','accepted','released','settled'] or any(not uint(row.get('sequence')) or row['sequence']!=i+1 or not uint(row.get('invalidated')) or row['invalidated'] not in (0,1) for row in (q,a,r,s)):return False
        variable={'event','touch','padReads','invalidated'}
        if any({k:v for k,v in row.items() if k not in variable}!={k:v for k,v in q.items() if k not in variable} for row in (a,r,s)):return False
        fields=('stage','advance','producer','revision','phase','kind','index','role','owner','generation','qaRevision','manager','data','execState','context','itemId','taskState','partySlot','partyBusy','graphicsReady','fadeFinished','keys')
        if any(not uint(q.get(k)) for k in fields) or q['keys'] or q['producer'] not in (1,3) or q['owner']!=(2 if q['producer']==1 else 1) \
            or not q['revision'] or not q['qaRevision'] or not q['generation'] or not q['manager'] or not q['data'] \
            or q['execState']!=2 or q['graphicsReady']!=1 or q['fadeFinished']!=1:return False
        if q['producer']==3:
            if q['role'] or not ((q['phase']==1 and q['kind'] in (0,1,5)) or (q['phase']==4 and q['kind']==2)):return False
        else:
            if q['context']!=5 or q['itemId']!=50 or q['partyBusy'] or q['partySlot'] or q['index']:return False
            if q['kind']==0:
                if q['phase']!=4 or q['role']!=37:return False
            elif q['kind']==1:
                if q['phase']!=5 or q['role'] not in (38,39,40):return False
                if q['role']==38:
                    if q['taskState']!=0 or any(q.get(k)!=1 for k in ('printerActive','printerMatches','printerWaiting')) or q.get('printerState')!=2:return False
                elif q['taskState']!=q['role']-38:return False
                seen_roles.add(q['role'])
            else:return False
        if not all(uint(row.get('padReads')) for row in (q,a,r,s)) or not q['padReads']<=a['padReads']<=r['padReads'] or s['padReads']-r['padReads']<2 \
            or r.get('invalidated') not in (0,1) or (not r['invalidated'] and r['padReads']-a['padReads']<4) \
            or not isinstance(a.get('touch'),list) or len(a['touch'])!=3 or any(not uint(v) for v in a['touch']) \
            or a['touch'][0]!=1 or a['touch'][1]>=256 or a['touch'][2]>=192:return False
        seen_kinds[q['producer']].add(q['kind'])
        outer_press.append((q['producer'],q['revision'],q['phase'],a['padReads'],0,tuple(a['touch'])))
        outer_release.append((q['producer'],q['revision'],q['phase'],r['padReads']))
    if seen_roles!={38,39,40} or seen_kinds!={1:{0,1},3:{0,1,2,5}}:return False
    inputs=report['inputEvidence'];press=[r for r in inputs if r.get('event')=='press'];release=[r for r in inputs if r.get('event')=='release']
    if any(r.get('event') not in ('press','release') for r in inputs) or len(press)!=count+len(expected_kinds)+1 or len(release)!=len(press):return False
    if any(any(not uint(r.get(k)) for k in ('stage','owner','revision','phase','padReads','keys')) or not isinstance(r.get('touch'),list) or len(r['touch'])!=3 or any(not uint(v) for v in r['touch']) for r in press) \
        or any(any(not uint(r.get(k)) for k in ('stage','owner','revision','phase','padReads','ownerChanged')) for r in release):return False
    field=[r for r in press if r.get('owner')==0]
    if len(field)!=1 or field[0].get('keys')!=0 or field[0].get('touch',[0])[0]!=1:return False
    def press_key(r):return (r.get('owner'),r.get('revision'),r.get('phase'),r.get('padReads'),r.get('keys'),tuple(r.get('touch',[])))
    def release_key(r):return (r.get('owner'),r.get('revision'),r.get('phase'),r.get('padReads'))
    if sorted(press_key(r) for r in press if r.get('owner')!=0)!=sorted(evo_press+outer_press) \
        or sorted(release_key(r) for r in release if r.get('owner')!=0)!=sorted(evo_release+outer_release):return False
    return True


def evolution_validator_self_check():
    from copy import deepcopy
    def model(cancel):
        status='passed_original_evolution_'+('cancel' if cancel else 'success')
        initial=[40,23,28,23,28,20];expected=[42,24,30,24,30,22];finalstats=expected if cancel else [47,28,35,29,35,26]
        fixture=dict(preparedBeforeFirstFieldAction=True,species=152,partyCount=1,moves=[33,45,0,0],initialLevel=15,initialExperience=2035,item=50,quantity=2,
            initialFriendship=70,initialMood=0,eggLocation=0,fixtureUsesExistingStarter=True,otNameProfileMatched=True,otNameLength=5,privateSaveMemoryOnly=True,mapSection=177,originalPersonality=123,originalOtId=456)
        mon=[123,456,152,0,0,0,40,40,15,0,33,45,0,0,35,40,0,0,0,0,0,0,35,40,0,0]
        final=dict(status=status,cancelCase=cancel,fixturePrepared=1,initialLevel=15,initialExperience=2035,level=16,experience=2535,species=152 if cancel else 153,quantity=1,
            personality=123,otId=456,cancelAccepted=int(cancel),ackAccepted=1,queued=1+int(cancel),accepted=1+int(cancel),rejected=0,released=1+int(cancel),settled=1+int(cancel),
            geometryFrames=3,oscillateFrames=2,firstGeometrySerial=10,lastGeometrySerial=12,initialFieldViewWidth=384,fieldViewWidth=384,outerTypedAccepted=8,outerTypedReleased=8,outerTypedSettled=8,
            outerTypedRejected=0,statsMask=3,summaryLifetimes=0,choiceMenus=0,particleContributionSeparated=False,resetFinished=True,privateSaveWrites=False)
        semantic=dict(species=final['species'],level=16,experience=2535,quantity=1,friendship=75,mood=40,fieldReturned=False,personality=123,otId=456,hp=finalstats[0],maxHp=finalstats[0],
            bayleefSeen=int(not cancel),bayleefCaught=int(not cancel),dexSeenDelta=int(not cancel),dexCaughtDelta=int(not cancel),otherBagBytesPreserved=True,flagsMoneyPreserved=True,ivEvPreserved=True,stats=finalstats)
        states=[dict(frame=1+i,revision=1+i,phase=phase,active=1,ready=1,summaryActive=0,flags=1,species=152,newSpecies=153,moveToLearn=0,actionMask=2 if phase==8 else 1) for i,phase in enumerate((8,44 if cancel else 12))]
        frames=[dict(file='evolution-%02u.rgba'%i,width=256,height=192,bytes=256*192*4,phase=8 if i<2 else 44 if cancel else 12,revision=i+1,frame=i+1,
            vertices=4,polygons=1,geometrySerial=10+i,geometrySerialAfter=10+i,nonuniform=True) for i in range(3)]
        files={'/frames/'+row['file']:str(i+1)*64 for i,row in enumerate(frames)}
        files.update({'/frames/evolution-field-before.rgba':'a'*64,'/frames/evolution-field-return.rgba':'b'*64,'/frames/candy-stats-diff.rgba':'c'*64,'/frames/candy-stats-total.rgba':'d'*64})
        stats=[dict(stage=6+i,role=39+i,owner=2,context=5,itemId=50,phase=5,taskState=1+i,ready=1,graphicsReady=1,fadeFinished=1,bytes=128*112*4,staleDiffRejected=i,
            displayValues=[b-a for a,b in zip(initial,expected)] if not i else expected,file='candy-stats-'+('diff' if not i else 'total')+'.rgba') for i in range(2)]
        outer=[];inputs=[dict(stage=1,event='press',owner=0,revision=0,phase=0,padReads=1,keys=0,touch=[1,100,100]),dict(stage=1,event='release',owner=0,revision=0,phase=0,padReads=5,ownerChanged=0)]
        plans=[(3,1,2,0,0),(3,1,3,1,0),(3,4,4,2,0),(1,4,5,0,37),(1,5,6,1,38),(1,5,6,1,39),(1,5,7,1,40),(3,1,9,5,0)]
        for i,(producer,phase,stage,kind,role) in enumerate(plans,1):
            reads=400 if i==8 else i*12
            q=dict(event='queued',sequence=i,stage=stage,advance=int(role!=38),producer=producer,revision=i,phase=phase,kind=kind,index=0,role=role,
                owner=2 if producer==1 else 1,generation=i,qaRevision=i,manager=100,data=200,execState=2,context=5 if producer==1 else 0,
                itemId=50 if producer==1 else 0,taskState=role-38 if role>=38 else 0,partySlot=0,partyBusy=0,graphicsReady=1,fadeFinished=1,keys=0,padReads=reads,
                invalidated=0,touch=[0,0,0],printerActive=1,printerMatches=1,printerWaiting=1,printerState=2)
            a={**q,'event':'accepted','padReads':reads+1,'touch':[1,64,24]};r={**q,'event':'released','padReads':reads+5};s={**q,'event':'settled','padReads':reads+7};outer.extend((q,a,r,s))
            inputs.extend((dict(stage=stage,event='press',owner=producer,revision=i,phase=phase,padReads=reads+1,keys=0,touch=a['touch']),
                dict(stage=stage,event='release',owner=producer,revision=i,phase=phase,padReads=reads+5,ownerChanged=0)))
        evo=[]
        for i,kind in enumerate(([1,0] if cancel else [0]),1):
            phase=8 if kind else 44 if cancel else 12;reads=200+i*12;keys=2 if kind else 1
            q=dict(event='queued',sequence=i,revision=100+i,phase=phase,kind=kind,index=0,padReads=reads,keys=keys,touch=[0,0,0])
            a={**q,'event':'accepted','padReads':reads+1,'actionMask':keys,'flags':1}
            r=dict(event='released',sequence=i,revision=100+i,phase=phase,kind=kind,index=0,padReads=reads+5,ownerChanged=0)
            s={**r,'event':'settled','padReads':reads+7};evo.extend((q,a,r,s))
            inputs.extend((dict(stage=8,event='press',owner=4,revision=100+i,phase=phase,padReads=reads+1,keys=keys,touch=[0,0,0]),
                dict(stage=8,event='release',owner=4,revision=100+i,phase=phase,padReads=reads+5,ownerChanged=0)))
        inputs.sort(key=lambda r:r['padReads'])
        returned={**deepcopy(semantic),'mood':40 if cancel else 0,'fieldReturned':True}
        return dict(evolution=True,evolutionCancel=cancel,evolutionEvidence=[final],evolutionFixtureEvidence=[fixture],evolutionSemanticEvidence=[semantic,returned],
            evolutionExpectedStatsEvidence=[dict(initialExperience=2035,expectedExperience=2535,initial=initial,expected=expected)],evolutionStateEvidence=states,evolutionFrameEvidence=frames,
            evolutionTypedEvidence=evo,candyTypedEvidence=outer,candyStatsEvidence=stats,candyBootEvidence=[dict(map=33,inputStep=3,inputFrame=0,qaObserved=True,qaVersion=1,qaBytes=232,fieldReady=1,owner=0,fadeFinished=1)],inputEvidence=inputs,capturedFiles=files,semanticEvidence=[dict(step='before',mons=[mon])],
            evidence=[dict(status=status,seenPhases=0,summaryLifetimes=0,activeAfterReturn=0,resetFinished=True,pendingDMA=0,partyItemsMoneyFlagsUnchanged=False,expectedPrivateEvolutionMutation=True)])
    assert evolution_evidence_passed(model(False)) and evolution_evidence_passed(model(True))
    cases=('wrong-status','wrong-species','exp','dex','identity','outer-owner','busy-role','raw-key','missing-release','missing-outer-release','bad-mask','bad-b-flag','bad-b-phase','short-release',
        'no-geometry','serial','same-pixels','field-file','stats-value','field-mood','cancel-field-mood','bag-mood','missing-checkpoint','swapped-checkpoint','field-stats','float-counter')
    for name in cases:
        bad=model(name in ('bad-b-flag','bad-b-phase','cancel-field-mood'));final=bad['evolutionEvidence'][0]
        if name=='wrong-status':final['status']='passed_original_summary_menu'
        elif name=='wrong-species':bad['evolutionSemanticEvidence'][0]['species']=152
        elif name=='exp':final['experience']=179
        elif name=='dex':bad['evolutionSemanticEvidence'][0]['dexCaughtDelta']=0
        elif name=='identity':bad['evolutionFixtureEvidence'][0]['originalPersonality']=999
        elif name=='outer-owner':bad['candyTypedEvidence'][0]['owner']=2
        elif name=='busy-role':bad['candyTypedEvidence'][5*4]['role']=43
        elif name=='raw-key':bad['inputEvidence'][0]['keys']=1
        elif name=='missing-release':del bad['evolutionTypedEvidence'][2]
        elif name=='missing-outer-release':del bad['candyTypedEvidence'][2]
        elif name=='bad-mask':bad['evolutionTypedEvidence'][1]['actionMask']=2
        elif name=='bad-b-flag':bad['evolutionTypedEvidence'][1]['flags']=0
        elif name=='bad-b-phase':bad['evolutionTypedEvidence'][1]['phase']=7
        elif name=='short-release':bad['evolutionTypedEvidence'][3]['padReads']=bad['evolutionTypedEvidence'][2]['padReads']+1
        elif name=='no-geometry':bad['evolutionFrameEvidence'][0]['polygons']=0
        elif name=='serial':bad['evolutionFrameEvidence'][0]['geometrySerialAfter']=99
        elif name=='same-pixels':bad['capturedFiles']['/frames/evolution-01.rgba']=bad['capturedFiles']['/frames/evolution-00.rgba']
        elif name=='field-file':del bad['capturedFiles']['/frames/evolution-field-return.rgba']
        elif name=='stats-value':bad['candyStatsEvidence'][1]['displayValues'][0]+=1
        elif name=='field-mood':bad['evolutionSemanticEvidence'][1]['mood']=40
        elif name=='cancel-field-mood':bad['evolutionSemanticEvidence'][1]['mood']=0
        elif name=='bag-mood':bad['evolutionSemanticEvidence'][0]['mood']=0
        elif name=='missing-checkpoint':del bad['evolutionSemanticEvidence'][1]['fieldReturned']
        elif name=='swapped-checkpoint':bad['evolutionSemanticEvidence'].reverse()
        elif name=='field-stats':
            bad['evolutionSemanticEvidence'][1]['stats'][0]+=1
            bad['evolutionSemanticEvidence'][1]['hp']=bad['evolutionSemanticEvidence'][1]['maxHp']=bad['evolutionSemanticEvidence'][1]['stats'][0]
        else:final['accepted']=1.0
        assert not evolution_evidence_passed(bad),name
    return dict(syntheticPositive=2,targetedNegatives=len(cases),gameplayEvidence=False)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--save', required=True, help='Designated private Route29 fixture; only new copies are opened')
    parser.add_argument('--output-name', default='native-summary-menu-pass', help='New private archive basename; existing output is never overwritten')
    learning=parser.add_mutually_exclusive_group()
    learning.add_argument('--tm-learn', action='store_true', help='Private original TM route; native typed Bag/Party and Summary controls')
    learning.add_argument('--rare-candy', action='store_true', help='Private original Candy upgrade/stats/level-up learning route')
    learning.add_argument('--evolution', action='store_true', help='Private original Chikorita15-to16 Candy evolution success route')
    learning.add_argument('--cancel-evolution', action='store_true', help='Private original state8 B-cancel route, preserving the Candy upgrade')
    args = parser.parse_args()
    evolution=args.evolution or args.cancel_evolution
    if evolution and args.output_name=='native-summary-menu-pass':
        args.output_name='native-summary-menu-evolution-'+('cancel' if args.cancel_evolution else 'success')
    if args.rare_candy and args.output_name == 'native-summary-menu-pass':
        args.output_name='native-summary-menu-rare-candy-pass'
    private_learning=args.tm_learn or args.rare_candy or evolution
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
    if evolution: consumers.append('src/evolution_scene.c')
    for name in consumers:
        if name not in units or units[name]['exitCode']:
            raise RuntimeError('Party/Summary consumer was not freshly audited: ' + name)
    command = link['command'].copy()
    if evolution and '-Wl,--export=HGNativeEvolutionUIGetState' not in command:
        raise RuntimeError('Require the fresh formal Evolution public getter; no private fallback export')
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
    if private_learning:
        owned.append(ROOT / 'ios/tests/summary_learn_guest.inc')
    if evolution:
        owned += [ROOT / 'include/native_evolution_ui.h', ROOT / 'files/poketool/personal/personal.json',
                  ROOT / 'files/poketool/personal/evo.json', ROOT / 'files/poketool/personal/growtbl.csv']
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
    if args.rare_candy:
        report.update(scope='Private valid-starter Candy50x2, originalLv5to6/stats/learnset and typed native Bag/Party/Summary; cancel retainsupgrade, HM/back/retry/replacement; no whole-game/UIKit/device/save-writing claim',
                      rareCandyLearn=True,peripheralInput='revision-bound native typed Bag/Party queues through original resolvers',productionApiAdded=True,
                      productionApiScope='One revision-guarded read-only six displayed stats getter; Party1480/QA232 unchanged')
    if evolution:
        learnset=pack/'files/a/0/3/3';members=narc(learnset);level16={}
        for species in (152,153):
            data=members[species]
            if len(data)%2: raise RuntimeError('Malformed original level-up learnset')
            words=[int.from_bytes(data[i:i+2],'little') for i in range(0,len(data),2)]
            level16[str(species)]=[word&511 for word in words if word!=65535 and word>>9==16 and word&511]
        if any(level16.values()): raise RuntimeError('This private fixture expects no natural Lv16 Summary/menu; do not force one')
        report.update(evolution=True,evolutionCancel=args.cancel_evolution,privateObserverBytes=232,evolutionPacketBytes=196,
            productionApiAdded=True,productionApiScope='One read-only revision-bound public Evolution196 state getter; existing Summary232 observer',
            evolutionLearnsetPrerequisite=dict(narcSha256=sha(learnset),level16Moves=level16,summaryAndChoiceMenusExercised=False),
            fixturePolicy='Original starter/PID/OT/two moves preserved; pre-flow EXP2035 with original SetMonData/CalcMonLevelAndStats plus Candy50x2 only',
            particleContributionSeparated=False,scope='Original field Candy Lv15to16 evolution success or legal state8 B cancel, canonical MAIN geometry/pixels, original ACK and Bag/field/reset. No separate SPL-pixel attribution, learning-menu/Summary, UIKit/device/audio/full-game claim.')
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
        if private_learning:
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
        if private_learning:
            extra_exports = ('HGNativeSummaryLearnQAGetState', 'HGNativeBagUIGetState', 'CreateMon', 'MonSetMoveInSlot_ResetPpUp',
                             'Bag_AddItem', 'Bag_GetQuantity', 'Bag_GetPocketSlotN', 'GetMoveMaxPP',
                             'PlayerProfile_GetTrainerID', 'Save_VarsFlags_sizeof')
            for name in extra_exports:
                flag = '-Wl,--export=' + name
                if flag not in command:
                    command.insert(command.index('-o'), flag)
            if evolution:
                extra_exports=tuple(name for name in extra_exports if name!='CreateMon')+('SetMonData','CalcMonLevelAndStats','Pokedex_CheckMonSeenFlag','Pokedex_CheckMonCaughtFlag')
                # These two setters are fixture-only before any field action; none is a shipping export or gameplay bypass.
                for name in ('SetMonData','CalcMonLevelAndStats','Pokedex_CheckMonSeenFlag','Pokedex_CheckMonCaughtFlag'):
                    flag='-Wl,--export='+name
                    if flag not in command: command.insert(command.index('-o'),flag)
                old='-Wl,--export=CreateMon'
                if old in command: command.remove(old)
            report['privateAdditionalExports'] = list(extra_exports)
        run(command, 'strict-link.log')
        generated, writes, restores = generate_engine(destination, module, run)
        host_flags += ['-g', '-fsanitize=address,undefined', '-fno-sanitize-recover=all']
        if evolution:
            host_flags += ['-DHG_EVOLUTION_CHECK']
            if args.cancel_evolution: host_flags += ['-DHG_EVOLUTION_CANCEL_CHECK']
        elif args.tm_learn:
            host_flags += ['-DHG_SUMMARY_TM_LEARN_CHECK']
        elif args.rare_candy:
            host_flags += ['-DHG_SUMMARY_RARE_CANDY_CHECK']

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
            result = subprocess.run(argv, cwd=ROOT, env={**os.environ, 'HEARTGOLD_TEST_MUTE': '1'}, capture_output=True, text=True, timeout=360 if evolution else 240 if private_learning else 180)
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
        if args.rare_candy:
            for key,label in (('candyLearningEvidence','learning'),('candyTypedEvidence','typed'),('candyStateEvidence','state'),
                              ('candyStatsEvidence','stats'),('candyFixtureEvidence','fixture'),('candyBootEvidence','boot'),
                              ('candyExpectedStatsEvidence','expected stats'),('candySemanticEvidence','semantic boundary')):
                prefix='Summary Candy '+label+' evidence: '
                report[key]=[json.loads(line[len(prefix):]) for line in output.splitlines() if line.startswith(prefix)]
            report['complete']=exit_code==0 and len(evidence)==1 and evidence[0]['status']=='passed_original_summary_candy_learning'
            report['passed']=report['complete'] and 'PASS: original Rare Candy route' in output
        if evolution:
            for key,label in (('evolutionEvidence','result'),('evolutionFixtureEvidence','fixture'),('evolutionSemanticEvidence','semantic'),
                              ('evolutionExpectedStatsEvidence','expected stats'),('evolutionStateEvidence','state'),('evolutionFrameEvidence','frame'),('evolutionTypedEvidence','typed')):
                prefix='Evolution '+label+' evidence: '
                report[key]=[json.loads(line[len(prefix):]) for line in output.splitlines() if line.startswith(prefix)]
            for key,label in (('candyTypedEvidence','typed'),('candyStatsEvidence','stats'),('candyBootEvidence','boot')):
                prefix='Summary Candy '+label+' evidence: '
                report[key]=[json.loads(line[len(prefix):]) for line in output.splitlines() if line.startswith(prefix)]
            expected_status='passed_original_evolution_'+('cancel' if args.cancel_evolution else 'success')
            report['complete']=exit_code==0 and len(evidence)==1 and evidence[0]['status']==expected_status
            report['passed']=report['complete'] and 'PASS: original field Rare Candy evolution route' in output
        report['generatedArtifactsUnchanged'] = hashes(artifacts) == artifact_hashes
        report['capturedFiles'] = hashes(path for path in (destination / 'frames').iterdir() if path.is_file())
        if args.rare_candy:
            report['passed'] &= candy_evidence_passed(report)
        if evolution:
            for row in report['evolutionFrameEvidence']:
                name=row['file'];path=destination/'frames'/name
                if Path(name).name!=name or path.stat().st_size!=row['bytes']:
                    raise RuntimeError('Evolution canonical frame size/path mismatch')
            for name in ('evolution-field-before.rgba','evolution-field-return.rgba'):
                if (destination/'frames'/name).stat().st_size!=384*192*4: raise RuntimeError('Evolution field frame size mismatch')
            report['passed'] &= evolution_evidence_passed(report)
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
