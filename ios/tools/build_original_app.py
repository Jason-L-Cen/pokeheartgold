# SPDX-License-Identifier: GPL-3.0-or-later
"""Build the original C engine as a native UIKit app, using the verified private content pack."""
import argparse
from concurrent.futures import ThreadPoolExecutor
import hashlib
import json
import plistlib
import re
import shutil
import subprocess
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


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--sdk',choices=('iphonesimulator','iphoneos'),default='iphonesimulator')
    parser.add_argument('--check',action='store_true',help='Run UIKit input, frame, pause/resume and reset checks after launching')
    args=parser.parse_args()
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
    generated,writes,restores=generate_engine(destination,module,run)
    flags,sources=native_build_settings(destination)
    target='arm64-apple-ios16.0'+('-simulator' if args.sdk=='iphonesimulator' else '')
    def compile_one(item):
        index,source=item; output=destination/f'unit-{index}.o'
        trap_flags=['-DWASM_RT_TRAP_HANDLER=HGAppTrap'] if source.name=='wasm-rt-impl.c' else []
        run(['xcrun','--sdk',args.sdk,'clang','-target',target,*flags,*trap_flags,'-c',source,'-o',output],f'compile-{index}.log')
        return output
    with ThreadPoolExecutor(max_workers=4) as pool:
        objects=list(pool.map(compile_one,enumerate([*generated,*sources])))
    app=destination/'HeartGoldOriginal.app'
    if app.exists(): shutil.rmtree(app)
    app.mkdir()
    entry=destination/'main.o'; audio=destination/'audio.o'
    check_flags=['-DHEARTGOLD_ORIGINAL_APP_CHECK'] if args.check else []
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
    (app/'Info.plist').write_bytes(plistlib.dumps(plist))
    rows=ET.parse(ROOT/'files/msgdata/msg/msg_0750.gmm').getroot().findall('row')
    move_names=['']*len(rows)
    for row in rows: move_names[int(row.attrib['index'])]=row.find("language[@name='English']").text or ''
    (app/'battle-move-names.plist').write_bytes(plistlib.dumps(move_names))
    (app/'party-action-names.plist').write_bytes(plistlib.dumps(party_action_names(move_names)))
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
                intermediateSha256=link['outputSha256'],nativeContent=link.get('nativeContent'),
                checkedStackWrites=writes,exceptionDepthRestores=restores,
                executableSha256=hashlib.sha256((app/'HeartGoldOriginal').read_bytes()).hexdigest())
    (destination/'report.json').write_text(json.dumps(report,indent=2)+'\n')
    print(app)


if __name__=='__main__': main()
