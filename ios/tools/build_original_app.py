# SPDX-License-Identifier: GPL-3.0-or-later
"""Build the original C engine as a native UIKit app, using the verified private content pack."""
import argparse
from concurrent.futures import ThreadPoolExecutor
import hashlib
import json
import plistlib
from pathlib import Path
import re
import shutil
import subprocess
import tempfile
import xml.etree.ElementTree as ET

from check_abi32 import ROOT, BUILD, WABT, native_build_settings
from check_native_engine import generate_engine
from prepare_native_content import verify
from import_original_content import read_boot_header

# Set only after review of the exact actually passed native PC report.
APPROVED_PC_NATIVE_REPORT_SHA256 = 'd9d4cd159c7ca4222ba71bcf5475f5dcf6329835f8d93fb3be3e346a49567eac'


def party_action_names(move_names):
    header=(ROOT/'include/party_menu.h').read_text().split('typedef enum PartyMonContextMenuItem {',1)[1].split('PARTY_MON_CONTEXT_MENU_MAX,',1)[0]
    enums=re.findall(r'(?m)^\s*(PARTY_MON_CONTEXT_MENU_\w+),',header)
    context=(ROOT/'src/party_context_menu.c').read_text()
    messages=re.findall(r'ReadMsgDataIntoString\(partyMenu->msgData, msg_0300_(\d+), partyMenu->contextMenuStrings\[(PARTY_MON_CONTEXT_MENU_\w+)\]\)',context)
    field=(ROOT/'src/party_menu.c').read_text().split('static const u16 sFieldMoves[',1)[1].split('{',1)[1].split('}',1)[0]
    field_moves=re.findall(r'\bMOVE_\w+\b',field)
    move_ids={name:int(value) for name,value in re.findall(r'(?m)^#define\s+(MOVE_\w+)\s+(\d+)\s*$',(ROOT/'include/constants/moves.h').read_text())}
    rows=ET.parse(ROOT/'files/msgdata/msg/msg_0300.gmm').getroot().findall('row')
    labels={int(row.attrib['index']):row.find("language[@name='English']").text or '' for row in rows}
    names=['']+[labels[int(index)] for index,name in messages]+[move_names[move_ids[move]] for move in field_moves]
    if len(messages)!=16 or len(field_moves)!=16 or len(enums)!=32 or enums!=[name for index,name in messages]+[move.replace('MOVE_','PARTY_MON_CONTEXT_MENU_',1) for move in field_moves]:
        raise RuntimeError('Original Party action enum order or labels changed')
    # The original UNUSED action at textId 11 intentionally has an empty message.
    if enums[10]!='PARTY_MON_CONTEXT_MENU_UNUSED' or any(not name.strip() for index,name in enumerate(names[1:],1) if index!=11):
        raise RuntimeError('Only the original UNUSED Party action may have an empty label')
    return names


def bag_names():
    source=(ROOT/'src/application/bag/bag_app.c').read_text()
    display=(ROOT/'src/application/bag/bag_app_display.c').read_text()
    assembly=(ROOT/'asm/bag_app_data.s').read_text()
    constants=(ROOT/'include/constants/items.h').read_text()
    banks=dict(re.findall(r'app->(msgData|itemNamesMsgData) = NewMsgDataFromNarc\([^\n]*NARC_msg_msg_(\d+)_bin',source))
    tables={}
    for field,bank in banks.items():
        rows=ET.parse(ROOT/f'files/msgdata/msg/msg_{bank}.gmm').getroot().findall('row')
        if sorted(int(row.attrib['index']) for row in rows)!=list(range(len(rows))): raise RuntimeError('Original Bag message IDs changed')
        table=['']*len(rows)
        for row in rows: table[int(row.attrib['index'])]=row.find("language[@name='English']").text or ''
        tables[field]=table
    messages=re.findall(r'app->menuStrings\[(\d+)\] = NewString_ReadMsgData\(app->msgData, msg_(\d+)_(\d+)\)',display)
    callbacks=assembly.split('gItemActionFuncs:',1)[1].split('gSubScreenTilemapCommands:',1)[0]
    action_count=len(re.findall(r'(?m)^\s*\.word\s+\S+',callbacks))
    if action_count!=16 or [int(index) for index,bank,message in messages]!=list(range(action_count)) or any(bank!=banks['msgData'] for index,bank,message in messages):
        raise RuntimeError('Original Bag action order or message bank changed')
    actions=[tables['msgData'][int(message)] for index,bank,message in messages]
    # The original action labels 3, 4 and 9 are unused empty English messages.
    if any(not label.strip() for index,label in enumerate(actions) if index not in (3,4,9)): raise RuntimeError('Original Bag action label is missing')
    pocket_count=int(re.search(r'(?m)^#define\s+POCKETS_COUNT\s+(\d+)',constants)[1])
    pocket_ids=[int(value) for value in re.findall(r'(?m)^#define\s+POCKET_\w+\s+(\d+)\s*$',constants)]
    hitboxes=assembly.split('sTouchHitboxes_Main:',1)[1].split('gBgTemplate_Main1:',1)[0]
    coordinates=[int(value.strip(),0) for line in re.findall(r'(?m)^\s*\.byte\s+([^\n]+)',hitboxes) for value in line.split(',')]
    if pocket_count!=8 or pocket_ids!=list(range(pocket_count)) or coordinates[:32]!=[value for index in range(8) for value in (0,31,index*32,index*32+31)]:
        raise RuntimeError('Original Bag pocket IDs or hitbox order changed')
    pocket_source=display.split('void BagApp_ShowPocketDescription(',1)[1].split('\n}',1)[0]
    pocket_base=int(re.search(r'NewString_ReadMsgData\(app->msgData,\s*pocket\s*\+\s*(\d+)\)',pocket_source)[1])
    pockets=[tables['msgData'][pocket_base+index].split(r'\n',1)[0] for index in pocket_ids]
    items=tables['itemNamesMsgData']; item_count=int(re.search(r'(?m)^#define\s+ITEMS_COUNT\s+(\d+)',constants)[1])
    if len(items)!=item_count or not all(label.strip() for label in [*items,*pockets]): raise RuntimeError('Original Bag item or pocket label is missing')
    return {'bag-action-names.plist':actions,'bag-item-names.plist':items,'bag-pocket-names.plist':pockets}


def pokedex_ui_labels():
    # Finite original message references; shared captions match the existing native controls.
    source=json.loads((ROOT/'ios/OriginalApp/pokedex_ui_labels.json').read_text())
    if set(source)!={'controls','families'} or len(source['controls'])!=208 or len(source['families'])!=17 or source['families'][0] is not None:
        raise RuntimeError('Original Pokédex label mapping changed')
    shared=dict(zip(('up','down','left','right','confirm','back','overview'),('↑','↓','←','→','确认','返回','概览')))
    tables={}
    def label(ref):
        if not isinstance(ref,dict) or not isinstance(ref.get('source'),str) or not ref['source']:
            raise RuntimeError('Original Pokédex label provenance is missing')
        if set(ref)=={'shared','source'}:
            if ref['shared'] not in shared: raise RuntimeError('Unknown Pokédex shared caption')
            return shared[ref['shared']]
        if set(ref)!={'bank','message','source'} or type(ref['bank']) is not int or ref['bank'] not in (196,758,802) or type(ref['message']) is not int:
            raise RuntimeError('Invalid original Pokédex message reference')
        bank=ref['bank']
        if bank not in tables:
            rows=ET.parse(ROOT/f'files/msgdata/msg/msg_{bank:04d}.gmm').getroot().findall('row')
            if sorted(int(row.attrib['index']) for row in rows)!=list(range(len(rows))): raise RuntimeError('Original Pokédex message IDs changed')
            tables[bank]={int(row.attrib['index']):row.find("language[@name='English']").text or '' for row in rows}
        text=tables[bank].get(ref['message'],'')
        if not text.strip() or '{' in text or '}' in text or '\\n' in text or '\\r' in text or '\n' in text or '\r' in text:
            raise RuntimeError('Pokédex accessibility captions must be plain original messages')
        return text
    controls={}
    for key,ref in source['controls'].items():
        if not re.fullmatch(r'[1-9]\d*/[1-9]\d*/\d+',key): raise RuntimeError('Invalid Pokédex control label key')
        family,role,index=map(int,key.split('/'))
        if not (1<=family<=16 and 1<=role<=27 and 0<=index<=26): raise RuntimeError('Pokédex control label key is out of range')
        controls[key]=label(ref)
    return {'controls':controls,'families':['']+[label(ref) for ref in source['families'][1:]]}


def summary_tm_guest(report_path, audit, link, *, rare_candy=False, evolution=False, cancel_evolution=False, evolution_learn=False):
    """Reuse one sealed, passed private learning module; never prepare a substitute guest."""
    from audit_original import digest
    from check_abi32 import WABT
    from check_native_rainy_field import INCLUDES
    from check_native_summary_menu import candy_evidence_passed, tm_evidence_passed
    def sha(path):
        return hashlib.sha256(Path(path).read_bytes()).hexdigest()
    report_path=Path(report_path).resolve(); folder=report_path.parent
    manifest_path=folder/'archive-seals.json'; manifest=json.loads(manifest_path.read_text())
    report=json.loads(report_path.read_text()); module=folder/'engine.wasm'
    if report_path.name!='report.json' or not manifest or manifest.get('report.json')!=sha(report_path):
        raise RuntimeError('Select the sealed native TM report.json')
    for name,value in manifest.items():
        path=folder/name
        if Path(name).is_absolute() or not path.resolve().is_relative_to(folder) or not path.is_file() or sha(path)!=value:
            raise RuntimeError('Native TM archive seal changed: '+name)
    if evolution_learn:
        from check_native_summary_menu import evolution_learning_evidence_passed
        learning_passed=evolution_learning_evidence_passed(report) and report.get('evolutionCancel') is False
    elif evolution:
        from check_native_summary_menu import evolution_evidence_passed
        learning_passed=evolution_evidence_passed(report) and report.get('evolutionCancel') is cancel_evolution
    else:
        learning_passed=candy_evidence_passed(report) if rare_candy else (report.get('tmLearn') is True and tm_evidence_passed(report.get('tmLearningEvidence',[]),
            report.get('tmTypedEvidence',[]),report.get('inputEvidence',[]),report.get('tmExitFadeEvidence',[]),report.get('tmRawEvidence',[])))
    if not (learning_passed and report.get('passed') is True and report.get('complete') is True
            and type(report.get('exitCode')) is int and report['exitCode']==0
            and report.get('generatedArtifactsUnchanged') is True and report.get('privateSaveUnchanged') is True
            and report.get('headersUnchanged') is True and report.get('gitStateUnchanged') is True
            and report.get('sealsAfter') and all(value is True for value in report['sealsAfter'].values()) and not report.get('changedPaths')):
        raise RuntimeError('Native TM execution and preservation must actually pass first')
    audited=json.loads((audit/'report.json').read_text())
    include_paths=[ROOT/name for name in INCLUDES]
    if link.get('nativeContent'): include_paths.insert(0,ROOT/link['nativeContent']['include'])
    headers={path for root in include_paths for pattern in ('**/*.h','**/*.naix') for path in root.glob(pattern)}
    headers |= set((ROOT/'lib/MSL_C/src').glob('*.h')) | set((ROOT/'lib/MSL_C/src').glob('*.hpp'))
    if report['productionModuleSha256']!=link['outputSha256'] or report['headersSha256']!=audited['headersSha256'] or digest(headers)!=audited['headersSha256']:
        raise RuntimeError('Private TM module differs from the current shipping module or public headers')
    if manifest.get('engine.wasm')!=report['scenarioModuleSha256'] or sha(module)!=report['scenarioModuleSha256'] or sha(module)==link['outputSha256']:
        raise RuntimeError('Private TM module identity is invalid')
    artifacts=report['artifactHashes']
    if not artifacts or any(not Path(name).is_file() or sha(name)!=value for name,value in artifacts.items()):
        raise RuntimeError('Native TM generated artifacts changed')
    provenance=report['privateSourceProvenance']; expected={
        'src/field_system.c':'HG_SUMMARY_LEARN_FIELD_OBSERVER', 'src/text.c':'HG_SUMMARY_LEARN_TEXT_OBSERVER',
        'src/application/bag/bag_app.c':'HG_SUMMARY_LEARN_BAG_OBSERVER'}
    if len(provenance)!=3 or {row['source'] for row in provenance}!=set(expected):
        raise RuntimeError('Expected exactly the three original private observation copies')
    units={row['file']:row for row in audited['units']}
    for row in provenance:
        source=ROOT/row['source']; formal=audit/'objects'/Path(row['source']).with_suffix('.o')
        private=folder/'private-guest'/source.name; obj=private.with_suffix('.o')
        if not (row['observerMacro']==expected[row['source']] and row['originalBodyPreserved'] is True
                and sha(source)==row['originalSourceSha256']==units[row['source']]['sha256']
                and str(formal)==row['formalObject'] and sha(formal)==row['formalObjectSha256']
                and str(private)==row['privateSource'] and sha(private)==row['privateSourceSha256']
                and str(obj)==row['privateObject'] and sha(obj)==row['privateObjectSha256']):
            raise RuntimeError('Private TM source/object provenance changed: '+row['source'])
        if row['source']=='src/application/bag/bag_app.c' and not row['overlayProvenance']['originalOverlayStatePreserved']:
            raise RuntimeError('Private Bag must retain its original overlay owner')
    helper=ROOT/'ios/tests/summary_learn_guest.inc'
    if sha(helper)!=sha(folder/'sources/ios/tests/summary_learn_guest.inc'):
        raise RuntimeError('Private Summary observer source changed')
    fixture,=report['evolutionFixtureEvidence' if evolution else 'candyFixtureEvidence' if rare_candy else 'tmFixtureEvidence']
    baseline,=[row for row in report['semanticEvidence'] if row['step']=='before']
    if (fixture.get('fixtureUsesExistingStarter') is not True or fixture.get('otNameProfileMatched') is not True
            or not 1<=fixture.get('otNameLength',0)<=7 or fixture.get('originalPersonality')!=baseline['mons'][0][0]
            or fixture.get('originalOtId')!=baseline['mons'][0][1]):
        raise RuntimeError('Native TM fixture must retain the valid original starter identity')
    prefix='Summary Candy boot evidence: ' if rare_candy or evolution else 'Summary TM boot evidence: '
    boot=[json.loads(line[len(prefix):]) for line in (folder/'runtime.log').read_text().splitlines() if line.startswith(prefix)]
    if not any(row.get('qaObserved') is True and row.get('qaVersion')==1 and row.get('qaBytes')==232
            and row.get('fieldReady')==1 and row.get('owner')==0 and row.get('inputStep')==3 and row.get('inputFrame')==0 for row in boot):
        raise RuntimeError('Native TM observer must actually publish the 232-byte ready packet')
    listing=subprocess.run([str(WABT/'bin/wasm-objdump'),'-x',str(module)],capture_output=True,text=True,timeout=15)
    exports=set(re.findall(r'-> "([^"\n]+)"',listing.stdout))
    required={'HGNativeSummaryLearnQAGetState','HGNativeSummaryUIGetState','HGNativeEvolutionUIGetState','HGNativeCaptureUIGetState','HGNativePartyUIGetState','HGNativePartyUIGetStats','HGNativeBagUIGetState',
              'MonSetMoveInSlot_ResetPpUp','Bag_AddItem','GetMonData','Bag_GetQuantity',
              'SaveData_Get','SaveArray_Party_Get','Save_PlayerData_GetProfile','PlayerProfile_GetTrainerID','Save_VarsFlags_sizeof'}
    if evolution: required.update(('SetMonData','CalcMonLevelAndStats','Pokedex_CheckMonSeenFlag','Pokedex_CheckMonCaughtFlag'))
    if listing.returncode or not required<=exports or {name for name in exports if name.startswith(('HGNativeBattleMenusQA','HGNativeSummaryLearnQA'))}!={'HGNativeSummaryLearnQAGetState'}:
        raise RuntimeError('Private TM exports are incompatible; Battle QA substitutes are forbidden')
    return module,dict(scope='reuse sealed passed native '+('Evolution learning' if evolution_learn else 'Evolution' if evolution else 'Candy' if rare_candy else 'TM')+' case only',
        **({'evolution':True,'evolutionCancel':False,'evolutionLearn':True} if evolution_learn else {'evolution':True,'evolutionCancel':cancel_evolution} if evolution else {'rareCandyLearn':True} if rare_candy else {'summaryTm':True}),qaObserverBytes=232,
        observer='HGNativeSummaryLearnQAGetState',sourceReport=str(report_path),sourceReportSha256=sha(report_path),
        sourceManifestSha256=sha(manifest_path),privateGuestModule=str(module),privateGuestModuleSha256=sha(module),
        shippingModuleSha256=link['outputSha256'],headersSha256=audited['headersSha256'],privateQAExports=['HGNativeSummaryLearnQAGetState'],
        privateSourceProvenance=provenance)


def pokedex_guest(report_path, audit, link):
    """Reuse the approved compiled-game proof; current host changes need their own audit."""
    import os
    from audit_original import digest
    from check_native_rainy_field import INCLUDES
    def sha(path):
        return hashlib.sha256(Path(path).read_bytes()).hexdigest()
    report_path=Path(report_path).resolve(); folder=report_path.parent
    manifest_path=folder/'artifact-hashes.json'; module=folder/'engine.wasm'
    if report_path.name!='report.json' or sha(report_path)!='f14c903a7c3ac2aa76056da49ef6082a5560a45c4e6ffc3279f23fb3a833a8f0' \
            or sha(manifest_path)!='643bcfdc8cd22b7884ca44d4a5c49ea25126f585e14e4f9eb0ba08df3350a127':
        raise RuntimeError('Select the approved sealed native Pokédex report.json')
    manifest=json.loads(manifest_path.read_text()); report=json.loads(report_path.read_text())
    if len(manifest)!=1073 or manifest.get(str(report_path))!=sha(report_path):
        raise RuntimeError('Native Pokédex archive manifest changed')
    for name,value in manifest.items():
        path=Path(name)
        if not path.is_absolute() or path.is_symlink() or not path.resolve().is_relative_to(folder) or not path.is_file() or sha(path)!=value:
            raise RuntimeError('Native Pokédex archive seal changed: '+name)
    if {path.resolve() for path in folder.rglob('*') if path.is_file()}!={Path(name).resolve() for name in manifest}|{manifest_path}:
        raise RuntimeError('Native Pokédex archive contains missing or unreviewed files')
    evidence={'__name__':'_approved_pokedex_evidence','__file__':str(folder/'check_native_pokedex.py')}
    exec(compile((folder/'check_native_pokedex.py').read_bytes(),evidence['__file__'],'exec'),evidence)
    if not (evidence['pokedex_evidence_passed'](report) and report.get('status')=='passed_original_pokedex_normal_flow'
            and all(report.get(key) is True for key in ('complete','passed','gameRuntimeExecuted','runtimeEvidenceParsed',
                'semanticPassed','captureFilesPassed','allChildrenWaited','privateSaveUnchanged'))
            and report.get('sealsAfter') and all(value is True for value in report['sealsAfter'].values())
            and not report.get('evidenceParseErrors') and report.get('children')
            and all(row.get('returnCode')==0 and row.get('timedOut') is False and row.get('allWaited') is True
                and row.get('groupRetired') is True for row in report['children'])):
        raise RuntimeError('Native Pokédex execution and preservation must actually pass first')
    if evidence['verify_capture_files'](report,folder/'frames',sha)!=report['capturedFiles']:
        raise RuntimeError('Native Pokédex capture bytes differ from the passed records')
    private_sha='bb58552bbd419a8b1136bfa96f24a748791d1522e4be370b8abdce5c18623825'
    if report['privateModuleSha256']!=private_sha or manifest.get(str(module))!=private_sha or sha(module)!=private_sha \
            or sha(folder/'runtime.log')!=report['runtimeLogSha256'] \
            or report.get('actualReceipt')!={'status':report['status'],'passed':True,'privateModuleSha256':private_sha,'runtimeLogSha256':report['runtimeLogSha256']}:
        raise RuntimeError('Native Pokédex actual receipt or module identity changed')
    if not report['artifactHashes'] or any(manifest.get(name)!=value for name,value in report['artifactHashes'].items()):
        raise RuntimeError('Native Pokédex generated artifacts changed')
    audited=json.loads((audit/'report.json').read_text())
    include_paths=[ROOT/name for name in INCLUDES]
    if link.get('nativeContent'): include_paths.insert(0,ROOT/link['nativeContent']['include'])
    headers={path for root in include_paths for pattern in ('**/*.h','**/*.naix') for path in root.glob(pattern)}
    headers |= set((ROOT/'lib/MSL_C/src').glob('*.h')) | set((ROOT/'lib/MSL_C/src').glob('*.hpp'))
    if report['productionModuleSha256']!=link['outputSha256'] or link['outputSha256']!=evidence['MODULE_SHA'] \
            or report['headersSha256']!=audited['headersSha256'] or audited['headersSha256']!=evidence['HEADERS_SHA'] \
            or digest(headers)!=audited['headersSha256'] or sha(module)==link['outputSha256']:
        raise RuntimeError('Native Pokédex game module or public headers differ from the current formal build')
    # These five host inputs are independently re-audited; all original game/support inputs must still match.
    host_changes={ROOT/name for name in ('ios/OriginalApp/main.m','ios/OriginalApp/pokedex_ui.inc',
        'ios/tests/check_original_app.inc','ios/tests/pokedex_check.inc','ios/tools/build_original_app.py')}
    source_bank=report['sealsBefore']['reviewedSourceBank']
    if len(source_bank)!=18841 or any(sha(name)!=value for name,value in source_bank.items() if Path(name) not in host_changes):
        raise RuntimeError('Original game/support input changed since the passed native Pokédex case')
    names=subprocess.run(['git','ls-files','-z','--cached','--others','--exclude-standard'],cwd=ROOT,
        env=dict(os.environ,GIT_OPTIONAL_LOCKS='0'),capture_output=True,check=True,timeout=15).stdout
    current_inputs={ROOT/name for name in os.fsdecode(names).split('\0') if name}|set(map(Path,source_bank))
    current_inputs.update(path for path in (ROOT/'ios/build-original-host/include').rglob('*') if path.is_file())
    if current_inputs!=set(map(Path,source_bank))|{ROOT/'ios/tests/pokedex_check.inc'}:
        raise RuntimeError('Current inputs must be exactly the native source bank plus the new Pokédex CHECK include')
    expected={'src/field_system.c':'HG_SUMMARY_LEARN_FIELD_OBSERVER','src/text.c':'HG_SUMMARY_LEARN_TEXT_OBSERVER',
        'src/application/bag/bag_app.c':'HG_SUMMARY_LEARN_BAG_OBSERVER','src/application/pokedex/pokedex_app.c':'HEARTGOLD_NATIVE'}
    provenance=report['privateSourceProvenance']; units={row['file']:row for row in audited['units']}
    if [row['source'] for row in provenance]!=list(expected):
        raise RuntimeError('Expected the original three observation copies and one committed-state getter copy')
    for row in provenance:
        source=ROOT/row['source']; formal=audit/'objects'/Path(row['source']).with_suffix('.o')
        private=folder/'private-guest'/source.name; obj=private.with_suffix('.o')
        if not (row['observerMacro']==expected[row['source']] and row['originalBodyPreserved'] is True
                and sha(source)==row['originalSourceSha256']==units[row['source']]['sha256'] and not units[row['source']]['exitCode']
                and str(formal)==row['formalObject'] and sha(formal)==row['formalObjectSha256']
                and str(private)==row['privateSource'] and sha(private)==row['privateSourceSha256']
                and str(obj)==row['privateObject'] and sha(obj)==row['privateObjectSha256']):
            raise RuntimeError('Private Pokédex source/object provenance changed: '+row['source'])
        overlay=row['overlayProvenance']
        if overlay:
            stem=source.stem; state=private.with_name(stem+'-overlay-state.json')
            added=['@HGNativePokedexUIQAGetCommittedState'] if stem=='pokedex_app' else ['@SummaryLearnBagTaskMatches']
            if not (overlay['originalOverlayStatePreserved'] is True and overlay['key']==row['source']
                    and overlay['addedFunctions']==added and overlay['removedFunctions']==[]
                    and str(state)==overlay['stateFile'] and sha(state)==overlay['stateSha256']
                    and sha(formal.with_suffix('.ll'))==overlay['formalIrSha256']==sha(private.with_name(stem+'-formal.ll'))
                    and sha(private.with_name(stem+'-uninstrumented.ll'))==overlay['rawIrSha256']
                    and sha(private.with_suffix('.ll'))==overlay['instrumentedIrSha256']):
                raise RuntimeError('Private Pokédex original overlay state changed: '+row['source'])
        if source.name=='pokedex_app.c':
            suffix=b'\n#ifdef HEARTGOLD_NATIVE\nconst HGNativePokedexUIState *HGNativePokedexUIQAGetCommittedState(void){ return &sNativePokedexPrevious; }\n#endif\n'
            if not (row['appendOnly'] is True and row['originalPrefixBytes']==source.stat().st_size
                    and row['originalPrefixSha256']==sha(source) and private.read_bytes()==source.read_bytes()+suffix):
                raise RuntimeError('Private committed-state getter must retain the exact original source prefix')
    if sha(ROOT/'ios/tests/summary_learn_guest.inc')!=sha(folder/'summary_learn_guest.inc'):
        raise RuntimeError('Private field/Bag 232-byte observer changed')
    private_exports=report['privateAdditionalExports']
    if len(private_exports)!=36 or len(set(private_exports))!=36 or 'CreateMon' in private_exports \
            or private_exports!=json.loads((folder/'reviewed-notes.json').read_text())['privateAdditionalExports']:
        raise RuntimeError('Require the exact reviewed 36 private Pokédex exports')
    listing=subprocess.run([str(WABT/'bin/wasm-objdump'),'-x',str(module)],capture_output=True,text=True,timeout=15)
    exports=set(re.findall(r'-> "([^"\n]+)"',listing.stdout))
    required=set(private_exports)|{'HGNativePokedexUIGetState','HGNativeBagUIGetState','HGNativePartyUIGetState',
        'HGNativePartyUIGetStats','HGNativeSummaryUIGetState','HGNativeEvolutionUIGetState','HGNativeCaptureUIGetState'}
    qa={'HGNativeSummaryLearnQAGetState','HGNativePokedexUIQAGetCommittedState'}
    if listing.returncode or not required<=exports or {name for name in exports if name.startswith('HGNative') and 'QA' in name}!=qa:
        raise RuntimeError('Private Pokédex exports differ from the approved observer/getter; no Battle QA or TM substitutes')
    return module,dict(scope='reuse sealed passed compiled-game normal Pokédex case; current host audited separately',
        normalPokedex=True,qaObserverBytes=232,observer='HGNativeSummaryLearnQAGetState',
        sourceReport=str(report_path),sourceReportSha256=sha(report_path),sourceManifestSha256=sha(manifest_path),
        privateGuestModule=str(module),privateGuestModuleSha256=private_sha,shippingModuleSha256=link['outputSha256'],
        headersSha256=audited['headersSha256'],privateQAExports=sorted(qa),privateAdditionalExports=private_exports,
        privateSourceProvenance=provenance,nativeSourceBankSha256=evidence['SOURCE_BANK_SHA'],nativeSourceInputCount=18841,
        currentHostMatchesNativeSourceBank=False)


# Bind Naming CHECK to the corrected original-game icon and keyboard evidence.
NAMING_APPROVED = dict(reportSha256='8cda1c390d6206ec8c3b9f376e7090e0fbf8c7d6fec836e857678987616742f3',manifestSha256='05d2f2f36d123c24321704cb44ef9a969eb86d94e596226e5f17f6ad1431d350',payloadCount=438,privateModuleSha256='e4284602f2b3e0fabee5ee16401e05d35c71ef02655e5e9c4a5c4c0a4a1484d4',
    shippingModuleSha256='3ec308d4a1e71620650f9bb4cb047d9f5a0e74ec26d15ef1fd39fb51a96ef37c',headersSha256='2a08a4a2c031de59b9aec46c7e67baeaefbd0e7af95d21e99390f125b6abbc55',sourceInputBankSha256='6226abeeb3c07588019e94e492e0b9fbae8270ebf377cb9b77a0016ac9d0a26c',sourceMapSha256='70bdfb90c7eebcc02b0f9b2c6118f7482f601f77c1ad0457d5951075f2083b06',
    fullManifestSha256='541cf12f6c2c5e2d65e71fdb781eae9c6641aa3f12b2048fef64b87aab3c4cea',fullReceiptSha256='ce70e365ac636561b7815fe5856f8aab6c9e630595a8b90deae31d154dd79138',fullPayloadCount=571)


def capture_guest(report_path, audit, link, *, naming=False):
    """Reuse only the approved R4 capture guest; never synthesize another fixture."""
    import os
    from audit_original import digest
    from check_native_rainy_field import INCLUDES
    def sha(path):
        return hashlib.sha256(Path(path).read_bytes()).hexdigest()
    report_sha='ed35823f25952f8725c1d3fa7220e69dbf148d9e8a1883672dee88aca676cd3f'
    manifest_sha='466716f07249bc8d9e2b212279ccc7bc4afd0e5feb285c0993db7bc22adba49d'
    private_sha='8e997d582bd5b05d32dcbd7465a1d8d3809cd300810554a93ef046a2af1359dd'
    shipping_sha='20ba9057be475a1244f8f90a662bf844c8788e46dfedcef1de410760a61ca997'
    headers_sha='a21da26e9940f4fa8e8c7c66b933f574df2b148bbfb46708afea2167f115e6fc'
    source_sha='0d6f11cff81a8ad118276d799b75ae81e103988413ea5d9be63cb6555e50cffe'
    source_map_sha='723f151d339054c9140c2d8a888e3d53de3e0f99f671d4c004a515860bd26576'
    qa_sha='8f569ac5dc722f07544c7d67d92d4b42f5ffde58d8ddd162cbfe6e602af712f1'
    if naming:
        if not all(NAMING_APPROVED.values()):
            raise RuntimeError('Naming CHECK is locked: bind the corrected actual Full and native Naming proof first')
        for key,value in NAMING_APPROVED.items():
            if key.endswith('Sha256') and (not isinstance(value,str) or not re.fullmatch(r'[0-9a-f]{64}',value)):
                raise RuntimeError('Invalid approved Naming identity: '+key)
        if not all(isinstance(NAMING_APPROVED[key],int) and NAMING_APPROVED[key]>0 for key in ('payloadCount','fullPayloadCount')):
            raise RuntimeError('Approved Naming manifest counts must be actual positive integers')
        report_sha=NAMING_APPROVED['reportSha256']; manifest_sha=NAMING_APPROVED['manifestSha256']
        private_sha=NAMING_APPROVED['privateModuleSha256']; shipping_sha=NAMING_APPROVED['shippingModuleSha256']
        headers_sha=NAMING_APPROVED['headersSha256']; source_sha=NAMING_APPROVED['sourceInputBankSha256']
        source_map_sha=NAMING_APPROVED['sourceMapSha256']
        qa_sha='50a6f0d02b9369a77c999ed169c9809a12790e0eda6c2a73c7dc15080c925ab4'
    expected_payloads=NAMING_APPROVED['payloadCount'] if naming else 304
    expected_variants=[3,4] if naming else [1,2]
    expected_checks=96 if naming else 95; expected_exports=25 if naming else 24
    expected_sources=18843 if naming else 18841
    qa=['HGNativeCaptureQAGetState','HGNativeCaptureQARequest']
    observers=['PokedexEntryPopup_Create','PokedexEntryPopup_Delete']
    report_path=Path(report_path).resolve(); folder=report_path.parent
    manifest_path=folder/'artifact-hashes.json'; module=folder/'guest/engine.wasm'
    if report_path.name!='report.json' or sha(report_path)!=report_sha or sha(manifest_path)!=manifest_sha:
        raise RuntimeError('Select the approved sealed native Capture R4 report.json')
    manifest=json.loads(manifest_path.read_text()); report=json.loads(report_path.read_text())
    if len(manifest)!=expected_payloads or manifest.get('report.json')!=report_sha:
        raise RuntimeError('Native Capture R4 archive manifest changed')
    for name,value in manifest.items():
        relative=Path(name); path=folder/relative
        if relative.is_absolute() or '..' in relative.parts or path.is_symlink() or not path.resolve().is_relative_to(folder) or not path.is_file() or sha(path)!=value:
            raise RuntimeError('Native Capture archive seal changed: '+name)
    if {path.resolve() for path in folder.rglob('*') if path.is_file()}!={folder/name for name in manifest}|{manifest_path}:
        raise RuntimeError('Native Capture archive has missing or unreviewed files')
    if not all(report.get(key) is True for key in ('passed','complete','gameRuntimeExecuted','captureRegistrationCovered',
            'popupLifetimeCovered','privateSave4Unchanged','protectedInputsUnchanged','allChildrenWaited')) or report.get('firstFailure') is not None:
        raise RuntimeError('Native Capture execution and preservation must actually pass first')
    if not report['commands'] or any(row['terminalReturnCode']!=0 or row['timedOut'] or not row['allChildrenWaited'] for row in report['commands']):
        raise RuntimeError('Native Capture children must all terminate and be waited')
    cases=report['cases']
    if len(cases)!=2 or [case['variant'] for case in cases]!=expected_variants:
        raise RuntimeError('Require both original party and full-party PC captures')
    for case in cases:
        preserved=('passed','popupLifecycleChecked','resetQuiescent','oldPartyPreserved','oldPcPreserved','bagExact','flagsPreserved','statsExact')
        settled=all(case.get(key) in (True,1) for key in preserved) and case['status']==('passed_original_naming_flow' if naming else 'passed_original_capture_registration') \
            and case['outcome']==case['battleResult']==4 and case['targetPID']==case['storedPID'] \
            and case['targetSpecies']==case['storedSpecies']==10 and case['storedLocation']==(1 if case['variant']==expected_variants[0] else 2) \
            and case['storedBall']==case['expectedBall'] and case['ownedBefore']==0 and case['ownedAfter']==1 \
            and case['ballBefore']==1 and case['ballAfter']==0 and not case['prematureOwned'] \
            and case['maxHeldReads']<=4 and case['staleRejected']>=4 and case['snapshots']>=8 \
            and all(case[key]>0 for key in ('popupWaitFrames','popupReadyFrames','popupFadeFrames'))
        counts=(case['partyBefore']==1 and case['partyAfter']==2 and case['pcAfter']==case['pcBefore']) if case['variant']==expected_variants[0] \
            else (case['partyBefore']==case['partyAfter']==6 and case['pcAfter']==case['pcBefore']+1)
        if not settled or not counts:
            raise RuntimeError('Original Capture identity, input, lifecycle or settlement failed')
    if naming:
        if report.get('namingCovered') is not True or report.get('namingPublicStateBytes')!=16 or report.get('independentNamingFreeObserved') is not False:
            raise RuntimeError('Require actual original Naming keyboard coverage')
        for case in cases:
            observed=case['naming']; stored=case['storedNaming']; live=case['namingLive']
            if not all(observed.get(key) is True for key in ('initBusy','ready','postOKBusy','publicInactive')) \
                    or live.get('allocationObserved') is not True or not live.get('manager') or not live.get('data') \
                    or live.get('execState')!=1 or live.get('procState')!=1 \
                    or stored.get('liveAllocationObserved') is not True or stored.get('fadeStateObserved') is not True or stored.get('publicInactiveObserved') is not True:
                raise RuntimeError('Original Naming manager/data, readiness and exit observations must pass')
            if case['variant']==3:
                if not observed['pageBusy'] or not observed['maxIgnoreBusy'] or observed['pageSelectInputs']!=3 or observed['AInputsBeforeExtra']!=10 \
                        or stored['nameCodes']!=[299]*10+[65535] or stored['hasNickname']!=1 or stored['expectedDelta']!=1 \
                        or stored['nicknameAfter']!=stored['nicknameBefore']+1:
                    raise RuntimeError('Actual named party result must be ten CHAR_A plus EOS, nickname flag and stat+1')
            elif not stored['defaultNoInputSemantics'] or stored['hasNickname'] or stored['expectedDelta'] \
                    or stored['nicknameAfter']!=stored['nicknameBefore']:
                raise RuntimeError('Actual empty-name PC result must preserve original default/noInput semantics')
    guest=report['privateGuest']; notes_path=folder/'guest/sources/notes.json'
    notes=json.loads(notes_path.read_text()); full=notes['actualFull']; full_dir=Path(full['directory'])
    if full.get('manifestName','archive-seals.json' if naming else None)!='archive-seals.json' or full['payloadCount']!=(NAMING_APPROVED['fullPayloadCount'] if naming else 570) or full['passed'] is not True \
            or sha(full_dir/'archive-seals.json')!=full['manifestSha256'] or full['manifestSha256']!=(NAMING_APPROVED['fullManifestSha256'] if naming else '13f9fddbe78bdc026ead37aa30c180b716491ffa7e36fd5148a0deaa1cf0a8c8') \
            or sha(full_dir/'build-evidence.json')!=full['receiptSha256'] or full['receiptSha256']!=(NAMING_APPROVED['fullReceiptSha256'] if naming else 'c97cbcb3c362da5a7247abfeb45bf725a7fdfc73bdf4a26dba44de87395f0672'):
        raise RuntimeError('Capture must bind the actual BLDCNT Full570 proof')
    formal=json.loads((full_dir/'build-evidence.json').read_text())
    if not formal['passed'] or not formal['freshAuditPassed'] or formal['actualNormalCheckKinds']!=expected_checks \
            or formal['shipping']['shippingQaExportCount'] or formal['shipping']['undefinedSymbols'] \
            or len(formal['shipping']['exports'])!=expected_exports or formal['shipping']['exports'].count('HGNativeCaptureUIGetState')!=1:
        raise RuntimeError('Capture formal shipping export or normal-check proof changed')
    if naming and formal['shipping']['exports'].count('HGNativeNamingUIGetState')!=1:
        raise RuntimeError('Naming formal proof must retain the current public getter once')
    audited=json.loads((audit/'report.json').read_text())
    include_paths=[ROOT/name for name in INCLUDES]
    if link.get('nativeContent'): include_paths.insert(0,ROOT/link['nativeContent']['include'])
    headers={path for root in include_paths for pattern in ('**/*.h','**/*.naix') for path in root.glob(pattern)}
    headers |= set((ROOT/'lib/MSL_C/src').glob('*.h')) | set((ROOT/'lib/MSL_C/src').glob('*.hpp'))
    source_map={row['file']:row['sha256'] for row in audited['units']}
    if report['productionModuleSha256']!=shipping_sha or link['outputSha256']!=shipping_sha or sha(audit/'engine.wasm')!=shipping_sha \
            or report['headersSha256']!=headers_sha or audited['headersSha256']!=headers_sha or digest(headers)!=headers_sha \
            or hashlib.sha256(json.dumps(source_map,sort_keys=True,separators=(',',':')).encode()).hexdigest()!=source_map_sha \
            or report['sourceInputBankSha256']!=source_sha or formal['auditedSources']['sourceMapSha256']!=source_map_sha:
        raise RuntimeError('Capture formal game, headers or audited source identity changed')
    source_bank_path=Path(notes['sourceInputBank'])
    if sha(source_bank_path)!=source_sha:
        raise RuntimeError('Capture source bank identity changed')
    source_bank=json.loads(source_bank_path.read_text())
    # Only these three host inputs are changed for this integration; game/support inputs stay exact.
    host_changes={ROOT/name for name in ('ios/OriginalApp/main.m','ios/tests/check_original_app.inc','ios/tools/build_original_app.py')}
    if len(source_bank)!=expected_sources or any(sha(name)!=value for name,value in source_bank.items() if Path(name) not in host_changes):
        raise RuntimeError('Original game/support input changed since the approved Capture case')
    names=subprocess.run(['git','ls-files','-z','--cached','--others','--exclude-standard'],cwd=ROOT,
        env=dict(os.environ,GIT_OPTIONAL_LOCKS='0'),capture_output=True,check=True,timeout=15).stdout
    current_inputs={ROOT/name for name in os.fsdecode(names).split('\0') if name}|set(map(Path,source_bank))
    current_inputs.update(path for path in (ROOT/'ios/build-original-host/include').rglob('*') if path.is_file())
    if current_inputs!=set(map(Path,source_bank)):
        raise RuntimeError('Capture integration must not add another WT source/header input')
    if report['privateGuestModuleSha256']!=private_sha or guest['privateGuestModuleSha256']!=private_sha \
            or manifest.get('guest/engine.wasm')!=private_sha or sha(module)!=private_sha \
            or report['qaABIBytes']!=244 or report['privateQAExports']!=qa or guest['addedPrivateQAExports']!=qa \
            or guest['shippingModuleSha256']!=shipping_sha or guest['headersSha256']!=headers_sha:
        raise RuntimeError('Capture private module or exact two-export QA244 ABI changed')
    if guest['gameplayWrappers'] or report['gameplayWrappers'] or guest['lifecycleObservers']!=observers or report['lifecycleObservers']!=observers:
        raise RuntimeError('Only the two read-only popup lifecycle observers are allowed')
    lifetime=guest['popupLifetimeProvenance']
    if lifetime!=report['popupLifetimeProvenance'] or lifetime['originalCallsPerWrapper']!=1 \
            or lifetime['expectedCreatesPerCase']!=1 or lifetime['expectedDeletesPerCase']!=1 or lifetime['qaBytesUnchanged']!=244 \
            or lifetime['gameplayDataWritten'] is not False or not all(lifetime[key] is True for key in (
                'undefinedCallSitesVerified','wrapperForwardsOriginalArguments','originalCreateResultPreserved','countersRemainPrivate',
                'fieldTickRequiresCaptureActiveWhileRealPopupAlive','deleteRequiresPriorInvalidation','sameCallDeleteExcludedFromFieldTickCheck')):
        raise RuntimeError('Independent original popup lifetime provenance changed')
    if sha(ROOT/lifetime['callerSource'])!=lifetime['callerSourceSha256'] or sha(ROOT/lifetime['header'])!=lifetime['headerSha256'] \
            or sha(folder/'popup-lifecycle-symbols.log')!=lifetime['symbolLogSha256']:
        raise RuntimeError('Original popup caller/header or undefined-symbol witness changed')
    for name,value in guest['privateSourceHashes'].items():
        path=Path(name)
        if path.is_symlink() or not path.resolve().is_relative_to(folder) or manifest.get(str(path.relative_to(folder)))!=value or sha(path)!=value:
            raise RuntimeError('Capture private source provenance changed: '+name)
    if naming:
        provenance=guest['namingSourceProvenance']
        if provenance['namingSourceReplaced'] is not False or provenance['namingWrappersAdded'] \
                or provenance['intermediateBufferReadsAdded'] is not False \
                or provenance['charCodes']!={'CHAR_A':299,'CHAR_B':300,'EOS':65535} \
                or provenance['originalTouchPoints']!={'CHAR_A':[36,97],'CHAR_B':[52,97],'OK':[213,71]} \
                or sha(ROOT/provenance['source'])!=provenance['sourceSha256']:
            raise RuntimeError('Require original keyboard source, ordinary touch points and read-only Naming observations')
    qa_header=folder/'guest/sources/capture_qa.h'
    if sha(qa_header)!=qa_sha or guest['privateSourceHashes'].get(str(qa_header))!=qa_sha:
        raise RuntimeError('Use the single sealed private Capture QA244 header')
    listing=subprocess.run([str(WABT/'bin/wasm-objdump'),'-x',str(module)],capture_output=True,text=True,timeout=15)
    exports=set(re.findall(r'-> "([^"\n]+)"',listing.stdout))
    if listing.returncode or exports!=set(formal['shipping']['exports'])|set(qa):
        raise RuntimeError('Capture module must contain public shipping getters and exactly the two private QA exports')
    return module,dict(scope='reuse corrected approved original Naming module; UIKit host checked separately' if naming else 'reuse approved native R4 capture module; UIKit host checked separately',
        sourceReport=str(report_path),sourceReportSha256=report_sha,sourceManifest=str(manifest_path),
        sourceManifestSha256=manifest_sha,sourceManifestPayloadCount=expected_payloads,privateGuestModuleSha256=private_sha,
        shippingModuleSha256=shipping_sha,headersSha256=headers_sha,sourceInputBankSha256=source_sha,sourceMapSha256=source_map_sha,
        privateQAExports=qa,qaHeader=str(qa_header),qaHeaderSha256=qa_sha,qaABIBytes=244,
        lifecycleObservers=observers,gameplayWrappers=[],popupLifetimeProvenance=lifetime,naming=naming,
        namingPublicStateBytes=16 if naming else None,namingSourceProvenance=guest.get('namingSourceProvenance') if naming else None)


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--sdk',choices=('iphonesimulator','iphoneos'),default='iphonesimulator')
    parser.add_argument('--check',action='store_true',help='Run UIKit input, frame, pause/resume and reset checks after launching')
    parser.add_argument('--check-case',choices=('field-potion','forced-switch','learn-moves','summary-tm','summary-candy','evolution','evolution-cancel','evolution-learn','pokedex','capture-registration','naming','pc-box'),help='Select an independent private UIKit case; requires --check')
    parser.add_argument('--pc-box-native-report',type=Path,help='Separately approved passed native PC report; only --check-case pc-box')
    parser.add_argument('--summary-tm-report',type=Path,help='Sealed passed native TM report.json; only --check-case summary-tm')
    parser.add_argument('--summary-candy-report',type=Path,help='Sealed passed native Candy report.json; only --check-case summary-candy')
    parser.add_argument('--evolution-report',type=Path,help='Sealed passed native success/cancel/learning report.json; only matching evolution CHECK case')
    parser.add_argument('--pokedex-report',type=Path,help='Approved sealed passed compiled-game Pokédex report.json; only --check-case pokedex')
    parser.add_argument('--capture-report',type=Path,help='Approved sealed passed native Capture R4 report.json; only --check-case capture-registration')
    parser.add_argument('--naming-report',type=Path,help='Corrected approved native Naming report.json; only --check-case naming')
    args=parser.parse_args()
    if args.check_case and not args.check:
        parser.error('--check-case requires --check; production builds cannot select a private case')
    if (args.check_case=='summary-tm')!=bool(args.summary_tm_report):
        parser.error('--summary-tm-report is required only with --check --check-case summary-tm')
    if (args.check_case=='summary-candy')!=bool(args.summary_candy_report):
        parser.error('--summary-candy-report is required only with --check --check-case summary-candy')
    if (args.check_case in ('evolution','evolution-cancel','evolution-learn'))!=bool(args.evolution_report):
        parser.error('--evolution-report is required only with --check --check-case evolution/evolution-cancel/evolution-learn')
    if (args.check_case=='pokedex')!=bool(args.pokedex_report):
        parser.error('--pokedex-report is required only with --check --check-case pokedex')
    if (args.check_case=='capture-registration')!=bool(args.capture_report):
        parser.error('--capture-report is required only with --check --check-case capture-registration')
    if (args.check_case=='naming')!=bool(args.naming_report):
        parser.error('--naming-report is required only with --check --check-case naming')
    if (args.check_case=='pc-box')!=bool(args.pc_box_native_report):
        parser.error('--pc-box-native-report is required only with --check --check-case pc-box')
    audit=BUILD/'audit'; module=audit/'engine.wasm'
    link=json.loads((audit/'engine-link.json').read_text())
    if link['exitCode'] or hashlib.sha256(module.read_bytes()).hexdigest()!=link['outputSha256']:
        raise RuntimeError('Run check_abi32.py --audit first; strict engine link is unavailable')
    listing=subprocess.run([str(WABT/'bin/wasm-objdump'),'-x',str(module)],capture_output=True,text=True,timeout=15)
    shipping_exports=set(re.findall(r'-> "([^"\n]+)"',listing.stdout))
    if listing.returncode or not {'HGNativePokedexUIGetState','HGNativeFieldChoiceUIGetState','HGNativeTrainerCardUIGetState','HGNativeFieldMenuUIGetState','HGNativePokegearUIGetState'}<=shipping_exports:
        raise RuntimeError('Shipping engine is missing an original public UI getter; run the current Full first')
    pack=verify(link.get('nativeContent')); header=read_boot_header(pack)
    destination=BUILD/f'app-{args.sdk}'; destination.mkdir(exist_ok=True)
    def run(command,log):
        argv=list(map(str,command)); journal=destination/(log+'.command.json')
        journal.write_text(json.dumps(dict(argv=argv,cwd=str(ROOT),returnCode=None),indent=2)+'\n')
        result=subprocess.run(argv,cwd=ROOT,capture_output=True,text=True)
        journal.write_text(json.dumps(dict(argv=argv,cwd=str(ROOT),returnCode=result.returncode),indent=2)+'\n')
        (destination/log).write_text(result.stdout+result.stderr)
        if result.returncode: raise RuntimeError(f'Original app build failed: {destination/log}')
    shipping_module_sha256=link['outputSha256']
    private_guest=None; pc_box_replay=None; app_source_bank=None
    if args.check_case=='pc-box':
        from check_pc_box_uikit_binding import QA_NAMES, pc_box_guest
        module,private_guest,pc_box_replay=pc_box_guest(args.pc_box_native_report,audit,link,APPROVED_PC_NATIVE_REPORT_SHA256)
        from check_native_pc_box import current_input_paths
        app_source_bank={name:hashlib.sha256(Path(name).read_bytes()).hexdigest() for name in sorted(current_input_paths())}
        app_source_bank_bytes=(json.dumps(app_source_bank,sort_keys=True,separators=(',',':'))+'\n').encode()
        (destination/'app-input-bank.json').write_bytes(app_source_bank_bytes)
    elif args.check_case=='naming':
        module,private_guest=capture_guest(args.naming_report,audit,link,naming=True)
    elif args.check_case=='capture-registration':
        module,private_guest=capture_guest(args.capture_report,audit,link)
    elif args.check_case=='pokedex':
        module,private_guest=pokedex_guest(args.pokedex_report,audit,link)
    elif args.check_case in ('evolution','evolution-cancel','evolution-learn'):
        module,private_guest=summary_tm_guest(args.evolution_report,audit,link,evolution=True,cancel_evolution=args.check_case=='evolution-cancel',evolution_learn=args.check_case=='evolution-learn')
    elif args.check_case in ('summary-tm','summary-candy'):
        module,private_guest=summary_tm_guest(args.summary_candy_report or args.summary_tm_report,audit,link,rare_candy=args.check_case=='summary-candy')
    elif args.check:
        from check_native_battle_menus import QA_EXPORTS, prepare_battle_menus_guest
        guest_destination=Path(tempfile.mkdtemp(prefix='battle-menus-guest-',dir=destination))
        if args.check_case:
            module,private_guest=prepare_battle_menus_guest(guest_destination,run,
                field_potion=args.check_case=='field-potion',forced_switch=args.check_case=='forced-switch',learn_moves=args.check_case=='learn-moves')
        else:
            def default_check_run(command,log):
                if log=='guest-strict-link.log':
                    command=list(command)
                    command.insert(command.index('-o'),'-Wl,--export=IsPaletteFadeFinished')
                run(command,log)
            module,private_guest=prepare_battle_menus_guest(guest_destination,default_check_run)
    selected_listing=subprocess.run([str(WABT/'bin/wasm-objdump'),'-x',str(module)],capture_output=True,text=True,timeout=15)
    (destination/'selected-module-exports.log').write_text(selected_listing.stdout+selected_listing.stderr)
    selected_exports=set(re.findall(r'-> "([^"\n]+)"',selected_listing.stdout))
    if selected_listing.returncode or not {'HGNativePokedexUIGetState','HGNativeCaptureUIGetState','HGNativeNamingUIGetState','HGNativeOptionsUIGetState','HGNativeStarterUIGetState','HGNativePCBoxUIGetState','HGNativeFieldChoiceUIGetState','HGNativeTrainerCardUIGetState','HGNativeFieldMenuUIGetState','HGNativePokegearUIGetState'}<=selected_exports:
        raise RuntimeError('Selected module lacks a current public UI getter; require a matching actual native proof, not rewritten approval hashes')
    if args.check_case=='pc-box' and selected_exports!=shipping_exports|set(QA_NAMES):
        raise RuntimeError('PC CHECK requires exactly the current public exports plus the two approved private PC QA exports')
    if args.check and not args.check_case and selected_exports!=shipping_exports|set(QA_EXPORTS)|{'IsPaletteFadeFinished'}:
        raise RuntimeError('Default CHECK requires the shipping exports, existing private QA exports and one read-only palette-fade export')
    generated,writes,restores=generate_engine(destination,module,run)
    flags,sources=native_build_settings(destination)
    check_flags=['-DHEARTGOLD_ORIGINAL_APP_CHECK'] if args.check else []
    if args.check and not args.check_case: check_flags.append('-DHEARTGOLD_ORIGINAL_DEFAULT_CHECK')
    if args.check_case=='pc-box':
        check_flags += ['-DHEARTGOLD_ORIGINAL_PC_BOX_CHECK',
            f'-DHG_PC_BOX_NATIVE_REPORT_SHA256="{private_guest["sourceReportSha256"]}"',
            f'-DHG_PC_BOX_NATIVE_MODULE_SHA256="{private_guest["privateGuestModuleSha256"]}"',
            f'-DHG_PC_BOX_REPLAY_SHA256="{private_guest["replayResourceSha256"]}"']
    if args.check_case in ('summary-tm','summary-candy','evolution','evolution-cancel','evolution-learn','pokedex'): check_flags.append('-DHEARTGOLD_ORIGINAL_SUMMARY_TM_CHECK')
    if args.check_case in ('evolution','evolution-cancel'): check_flags.append('-DHEARTGOLD_ORIGINAL_EVOLUTION_CHECK')
    if args.check_case=='evolution-learn': check_flags.append('-DHEARTGOLD_ORIGINAL_EVOLUTION_LEARN_CHECK')
    if args.check_case=='naming':
        check_flags += ['-DHEARTGOLD_ORIGINAL_CAPTURE_REGISTRATION_CHECK','-DHEARTGOLD_ORIGINAL_NAMING_CHECK',
            f'-DHG_NAMING_NATIVE_REPORT_SHA256="{private_guest["sourceReportSha256"]}"',
            f'-DHG_NAMING_NATIVE_MODULE_SHA256="{private_guest["privateGuestModuleSha256"]}"']
    if args.check_case=='capture-registration':
        check_flags += ['-DHEARTGOLD_ORIGINAL_CAPTURE_REGISTRATION_CHECK',
            f'-DHG_CAPTURE_NATIVE_REPORT_SHA256="{private_guest["sourceReportSha256"]}"',
            f'-DHG_CAPTURE_NATIVE_MODULE_SHA256="{private_guest["privateGuestModuleSha256"]}"']
    if args.check_case=='pokedex':
        check_flags += ['-DHEARTGOLD_ORIGINAL_POKEDEX_CHECK',
            f'-DHG_POKEDEX_NATIVE_REPORT_SHA256="{private_guest["sourceReportSha256"]}"',
            f'-DHG_POKEDEX_NATIVE_MODULE_SHA256="{private_guest["privateGuestModuleSha256"]}"']
    if args.check: sources.append(ROOT/'ios/Original/NativeAudioCheck.c')
    target='arm64-apple-ios16.0'+('-simulator' if args.sdk=='iphonesimulator' else '')
    def compile_one(item):
        index,source=item; output=destination/f'unit-{index}.o'
        trap_flags=['-DWASM_RT_TRAP_HANDLER=HGAppTrap'] if source.name=='wasm-rt-impl.c' else []
        run(['xcrun','--sdk',args.sdk,'clang','-target',target,*flags,*check_flags,*trap_flags,'-c',source,'-o',output],f'compile-{index}.log')
        return output
    with ThreadPoolExecutor(max_workers=4) as pool:
        objects=list(pool.map(compile_one,enumerate([*generated,*sources])))
    app=destination/'HeartGoldOriginal.app'
    if app.exists(): shutil.rmtree(app)
    app.mkdir()
    entry=destination/'main.o'; audio=destination/'audio.o'
    entry_flags=['-include',private_guest['qaHeader']] if args.check_case in ('capture-registration','naming') else []
    run(['xcrun','--sdk',args.sdk,'clang','-target',target,*flags,*check_flags,*entry_flags,'-fobjc-arc',
         '-c',ROOT/'ios/OriginalApp/main.m','-o',entry],'entry.log')
    run(['xcrun','--sdk',args.sdk,'clang','-target',target,'-std=c11','-O2',*check_flags,'-fobjc-arc',
         '-c',ROOT/'ios/OriginalApp/OriginalAudio.m','-o',audio],'audio.log')
    run(['xcrun','--sdk',args.sdk,'clang','-target',target,*objects,entry,audio,
         '-framework','UIKit','-framework','QuartzCore','-framework','CoreGraphics','-framework','Foundation','-framework','AVFAudio',
         '-o',app/'HeartGoldOriginal'],'link.log')
    plist=plistlib.loads((ROOT/'ios/Info.plist').read_bytes())
    plist['UIApplicationSceneManifest']={'UIApplicationSupportsMultipleScenes':False,
        'UISceneConfigurations':{'UIWindowSceneSessionRoleApplication':[{
            'UISceneConfigurationName':'Original Game','UISceneClassName':'UIWindowScene',
            'UISceneDelegateClassName':'OriginalSceneDelegate'}]}}
    plist.update(CFBundleExecutable='HeartGoldOriginal',CFBundleIdentifier='com.jasoncen.heartgold.original',
                 CFBundleName='HeartGoldOriginal',CFBundleDisplayName='心金 · 原版引擎',UIDeviceFamily=[1,2],MinimumOSVersion='16.0')
    if args.check:
        plist['HGPrivateCheckCase']=args.check_case or 'default'
    if args.check_case=='pc-box':
        plist['HGPrivatePCBoxNativeReportSha256']=private_guest['sourceReportSha256']
        plist['HGPrivatePCBoxNativeModuleSha256']=private_guest['privateGuestModuleSha256']
        plist['HGPrivatePCBoxReplaySha256']=private_guest['replayResourceSha256']
        (app/'pc-box-approved-trace.json').write_bytes(pc_box_replay)
    if args.check_case=='naming':
        plist['HGPrivateNamingNativeReportSha256']=private_guest['sourceReportSha256']
        plist['HGPrivateNamingNativeModuleSha256']=private_guest['privateGuestModuleSha256']
    if args.check_case=='capture-registration':
        plist['HGPrivateCaptureNativeReportSha256']=private_guest['sourceReportSha256']
        plist['HGPrivateCaptureNativeModuleSha256']=private_guest['privateGuestModuleSha256']
    if args.check_case=='pokedex':
        plist['HGPrivatePokedexNativeReportSha256']=private_guest['sourceReportSha256']
        plist['HGPrivatePokedexNativeModuleSha256']=private_guest['privateGuestModuleSha256']
    (app/'Info.plist').write_bytes(plistlib.dumps(plist))
    rows=ET.parse(ROOT/'files/msgdata/msg/msg_0750.gmm').getroot().findall('row')
    move_names=['']*len(rows)
    for row in rows: move_names[int(row.attrib['index'])]=row.find("language[@name='English']").text or ''
    (app/'battle-move-names.plist').write_bytes(plistlib.dumps(move_names))
    (app/'party-action-names.plist').write_bytes(plistlib.dumps(party_action_names(move_names)))
    for name,labels in bag_names().items(): (app/name).write_bytes(plistlib.dumps(labels))
    rows=ET.parse(ROOT/'files/msgdata/msg/msg_0237.gmm').getroot().findall('row')
    species_names=['']*len(rows)
    for row in rows: species_names[int(row.attrib['index'])]=row.find("language[@name='English']").text or ''
    (app/'battle-species-names.plist').write_bytes(plistlib.dumps(species_names))
    (app/'pokedex-ui-labels.plist').write_bytes(plistlib.dumps(pokedex_ui_labels()))
    menu_data=(ROOT/'asm/touchscreen_menu_data.s').read_text()
    records=menu_data.split('gTouchscreenMenuIcons:',1)[1].split('gTouchscreenMenuLayouts:',1)[0]
    message_ids=[int(value,16) for value in re.findall(r'\.short\s+0x[0-9A-Fa-f]+,\s+0x([0-9A-Fa-f]+)',records)]
    rows=ET.parse(ROOT/'files/msgdata/msg/msg_0196.gmm').getroot().findall('row')
    menu_names={int(row.attrib['index']):row.find("language[@name='English']").text or '' for row in rows}
    def menu_hitboxes(start,end):
        block=menu_data.split(start+':',1)[1].split(end+':',1)[0]
        values=[int(value.strip(),0) for line in re.findall(r'(?m)^\s*\.byte\s+([^\n]+)',block) for value in line.split(',')]
        if len(values)%4 or values[-4:]!=[255,0,0,0]: raise RuntimeError('Original menu hitbox layout changed')
        boxes=[values[i:i+4] for i in range(0,len(values)-4,4)]
        if any(not (top<bottom<=192 and left<right<=256) for top,bottom,left,right in boxes): raise RuntimeError('Invalid original menu hitbox')
        return [[left,top,right-left,bottom-top] for top,bottom,left,right in boxes]
    icons=menu_hitboxes('gTouchscreenMenuIconHitboxes','gTouchscreenMenuIcons')
    buttons=menu_hitboxes('gTouchscreenMenuButtonHitboxes','gTouchscreenMenuActionMsgIds')
    if len(message_ids)!=13 or len(icons)!=10 or len(buttons)!=2: raise RuntimeError('Original menu metadata changed')
    (app/'field-menu.plist').write_bytes(plistlib.dumps(dict(names=[menu_names[index] for index in message_ids],hitboxes=icons[1:]+buttons[::-1])))
    content=app/'OriginalContent'; content.mkdir()
    shutil.copytree(pack/'files',content/'files')
    (content/'boot-header.bin').write_bytes(header)
    run(['codesign','--force','--sign','-',app],'sign.log')
    report=dict(scope='native UIKit original-engine app build; gameplay validation separate',sdk=args.sdk,selfCheckEnabled=args.check,
                checkCase=(args.check_case or 'default') if args.check else None,
                learnMoves=args.check_case=='learn-moves',
                intermediateSha256=hashlib.sha256(module.read_bytes()).hexdigest(),shippingModuleSha256=shipping_module_sha256,
                privateGuestModuleSha256=private_guest['privateGuestModuleSha256'] if private_guest else None,
                privateGuest=private_guest,nativeContent=link.get('nativeContent'),
                checkedStackWrites=writes,exceptionDepthRestores=restores,
                executableSha256=hashlib.sha256((app/'HeartGoldOriginal').read_bytes()).hexdigest(),
                entryCommandJournal=str(destination/'entry.log.command.json'),
                entryCommandJournalSha256=hashlib.sha256((destination/'entry.log.command.json').read_bytes()).hexdigest())
    if args.check and not args.check_case: report['privateReadOnlyExports']=['IsPaletteFadeFinished']
    if args.check_case=='summary-tm': report['summaryTm']=True
    if args.check_case=='summary-candy': report['rareCandyLearn']=True
    if args.check_case in ('evolution','evolution-cancel'): report.update(evolution=True,evolutionCancel=args.check_case=='evolution-cancel')
    if args.check_case=='evolution-learn': report.update(evolution=True,evolutionCancel=False,evolutionLearn=True)
    if args.check_case=='naming': report['naming']=True
    if args.check_case=='pc-box': report['pcBox']=True
    if args.check_case=='pc-box':
        if app_source_bank!={name:hashlib.sha256(Path(name).read_bytes()).hexdigest() for name in sorted(current_input_paths())}:
            raise RuntimeError('Actual App source inventory or bytes changed during the PC CHECK build')
        report.update(appInputBankFile=str(destination/'app-input-bank.json'),appInputBankSha256=hashlib.sha256(app_source_bank_bytes).hexdigest(),
            appInputCount=len(app_source_bank),appInputsUnchanged=True,
            nativeWholeBankScope='historical native-run provenance; current App source bank is pinned separately')
    if args.check_case=='capture-registration': report['captureRegistration']=True
    if args.check_case=='pokedex': report['normalPokedex']=True
    (destination/'report.json').write_text(json.dumps(report,indent=2)+'\n')
    print(app)


if __name__=='__main__': main()
