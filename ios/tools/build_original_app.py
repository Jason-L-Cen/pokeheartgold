# SPDX-License-Identifier: GPL-3.0-or-later
"""Build the original C engine as a native UIKit app, using the verified private content pack."""
import argparse
from concurrent.futures import ThreadPoolExecutor
import hashlib
import json
import plistlib
import shutil
import subprocess
import xml.etree.ElementTree as ET

from check_abi32 import ROOT, BUILD, native_build_settings
from check_native_engine import generate_engine
from prepare_native_content import verify
from import_original_content import read_boot_header


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
