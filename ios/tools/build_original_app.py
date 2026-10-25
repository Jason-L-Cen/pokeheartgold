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

from check_abi32 import ROOT, BUILD, native_build_settings
from check_native_engine import generate_engine
from prepare_native_content import verify
from import_original_content import read_boot_header


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


def summary_tm_guest(report_path, audit, link, *, rare_candy=False):
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
    fixture,=report['candyFixtureEvidence' if rare_candy else 'tmFixtureEvidence']
    baseline,=[row for row in report['semanticEvidence'] if row['step']=='before']
    if (fixture.get('fixtureUsesExistingStarter') is not True or fixture.get('otNameProfileMatched') is not True
            or not 1<=fixture.get('otNameLength',0)<=7 or fixture.get('originalPersonality')!=baseline['mons'][0][0]
            or fixture.get('originalOtId')!=baseline['mons'][0][1]):
        raise RuntimeError('Native TM fixture must retain the valid original starter identity')
    prefix='Summary Candy boot evidence: ' if rare_candy else 'Summary TM boot evidence: '
    boot=[json.loads(line[len(prefix):]) for line in (folder/'runtime.log').read_text().splitlines() if line.startswith(prefix)]
    if not any(row.get('qaObserved') is True and row.get('qaVersion')==1 and row.get('qaBytes')==232
            and row.get('fieldReady')==1 and row.get('owner')==0 and row.get('inputStep')==3 and row.get('inputFrame')==0 for row in boot):
        raise RuntimeError('Native TM observer must actually publish the 232-byte ready packet')
    listing=subprocess.run([str(WABT/'bin/wasm-objdump'),'-x',str(module)],capture_output=True,text=True,timeout=15)
    exports=set(re.findall(r'-> "([^"\n]+)"',listing.stdout))
    required={'HGNativeSummaryLearnQAGetState','HGNativeSummaryUIGetState','HGNativePartyUIGetState','HGNativePartyUIGetStats','HGNativeBagUIGetState',
              'MonSetMoveInSlot_ResetPpUp','Bag_AddItem','GetMonData','Bag_GetQuantity',
              'SaveData_Get','SaveArray_Party_Get','Save_PlayerData_GetProfile','PlayerProfile_GetTrainerID','Save_VarsFlags_sizeof'}
    if listing.returncode or not required<=exports or {name for name in exports if name.startswith(('HGNativeBattleMenusQA','HGNativeSummaryLearnQA'))}!={'HGNativeSummaryLearnQAGetState'}:
        raise RuntimeError('Private TM exports are incompatible; Battle QA substitutes are forbidden')
    return module,dict(scope='reuse sealed passed native '+('Candy' if rare_candy else 'TM')+' case only',
        **({'rareCandyLearn':True} if rare_candy else {'summaryTm':True}),qaObserverBytes=232,
        observer='HGNativeSummaryLearnQAGetState',sourceReport=str(report_path),sourceReportSha256=sha(report_path),
        sourceManifestSha256=sha(manifest_path),privateGuestModule=str(module),privateGuestModuleSha256=sha(module),
        shippingModuleSha256=link['outputSha256'],headersSha256=audited['headersSha256'],privateQAExports=['HGNativeSummaryLearnQAGetState'],
        privateSourceProvenance=provenance)


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--sdk',choices=('iphonesimulator','iphoneos'),default='iphonesimulator')
    parser.add_argument('--check',action='store_true',help='Run UIKit input, frame, pause/resume and reset checks after launching')
    parser.add_argument('--check-case',choices=('field-potion','forced-switch','learn-moves','summary-tm','summary-candy'),help='Select an independent private UIKit case; requires --check')
    parser.add_argument('--summary-tm-report',type=Path,help='Sealed passed native TM report.json; only --check-case summary-tm')
    parser.add_argument('--summary-candy-report',type=Path,help='Sealed passed native Candy report.json; only --check-case summary-candy')
    args=parser.parse_args()
    if args.check_case and not args.check:
        parser.error('--check-case requires --check; production builds cannot select a private case')
    if (args.check_case=='summary-tm')!=bool(args.summary_tm_report):
        parser.error('--summary-tm-report is required only with --check --check-case summary-tm')
    if (args.check_case=='summary-candy')!=bool(args.summary_candy_report):
        parser.error('--summary-candy-report is required only with --check --check-case summary-candy')
    audit=BUILD/'audit'; module=audit/'engine.wasm'
    link=json.loads((audit/'engine-link.json').read_text())
    if link['exitCode'] or hashlib.sha256(module.read_bytes()).hexdigest()!=link['outputSha256']:
        raise RuntimeError('Run check_abi32.py --audit first; strict engine link is unavailable')
    pack=verify(link.get('nativeContent')); header=read_boot_header(pack)
    destination=BUILD/f'app-{args.sdk}'; destination.mkdir(exist_ok=True)
    def run(command,log):
        result=subprocess.run(list(map(str,command)),cwd=ROOT,capture_output=True,text=True)
        (destination/log).write_text(result.stdout+result.stderr)
        if result.returncode: raise RuntimeError(f'Original app build failed: {destination/log}')
    shipping_module_sha256=link['outputSha256']
    private_guest=None
    if args.check_case in ('summary-tm','summary-candy'):
        module,private_guest=summary_tm_guest(args.summary_candy_report or args.summary_tm_report,audit,link,rare_candy=args.check_case=='summary-candy')
    elif args.check:
        from check_native_battle_menus import prepare_battle_menus_guest
        guest_destination=Path(tempfile.mkdtemp(prefix='battle-menus-guest-',dir=destination))
        if args.check_case:
            module,private_guest=prepare_battle_menus_guest(guest_destination,run,
                field_potion=args.check_case=='field-potion',forced_switch=args.check_case=='forced-switch',learn_moves=args.check_case=='learn-moves')
        else:
            module,private_guest=prepare_battle_menus_guest(guest_destination,run)
    generated,writes,restores=generate_engine(destination,module,run)
    flags,sources=native_build_settings(destination)
    check_flags=['-DHEARTGOLD_ORIGINAL_APP_CHECK'] if args.check else []
    if args.check_case in ('summary-tm','summary-candy'): check_flags.append('-DHEARTGOLD_ORIGINAL_SUMMARY_TM_CHECK')
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
    run(['xcrun','--sdk',args.sdk,'clang','-target',target,*flags,*check_flags,'-fobjc-arc',
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
                executableSha256=hashlib.sha256((app/'HeartGoldOriginal').read_bytes()).hexdigest())
    if args.check_case=='summary-tm': report['summaryTm']=True
    if args.check_case=='summary-candy': report['rareCandyLearn']=True
    (destination/'report.json').write_text(json.dumps(report,indent=2)+'\n')
    print(app)


if __name__=='__main__': main()
