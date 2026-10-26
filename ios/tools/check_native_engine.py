# SPDX-License-Identifier: GPL-3.0-or-later
"""Compile the strict full-engine intermediate to native C and attempt NitroMain."""
import hashlib
import argparse
import json
from pathlib import Path
import re
import subprocess
from concurrent.futures import ThreadPoolExecutor

from check_abi32 import ROOT,BUILD,WABT,native_build_settings,restore_arm_division
from prepare_exception_unwind import restore_exception_depth
from import_original_content import read_boot_header
from prepare_native_content import verify


def generate_engine(destination, module, run):
    for path in destination.glob('hgengine*'): path.unlink()
    run([WABT/'bin/wasm2c',module,'-n','hgengine','--num-outputs=16','-o',destination/'hgengine.c'],'generate.log')
    header=destination/'hgengine-impl.h'
    content=header.read_text()
    if content.count('#include "hgengine.h"')!=1: raise RuntimeError('Generated implementation header changed')
    content=content.replace('#include "hgengine.h"','#include "hgengine.h"\n#include "NativeImports.h"')
    header.write_text(restore_arm_division(content))
    generated=sorted(destination.glob('hgengine_*.c'))
    if len(generated)!=16: raise RuntimeError('Expected sixteen generated native C units')
    invalid_calls=set()
    for path in generated:
        invalid_calls.update(re.findall(r'w2c_hgengine_[A-Za-z0-9_]*bitcast_invalid[A-Za-z0-9_]*',path.read_text()))
    if invalid_calls:
        raise RuntimeError('Incompatible original function calls remain: '+', '.join(sorted(invalid_calls)))
    writes=restores=0
    for path in generated:
        source,count=restore_exception_depth(path.read_text()); restores+=count
        expected=len(re.findall(r'w2c_0x5F_stack_pointer\s*=',source))
        source,count=re.subn(r'(\w+)->w2c_0x5F_stack_pointer = ([^;\n]+);',
            r'\1->w2c_0x5F_stack_pointer = HGNativeCheckedStackPointer(\1->w2c_env_instance, \2);',source)
        if count!=expected: raise RuntimeError('Generated stack-pointer layout changed')
        writes+=count; path.write_text(source)
    if not writes or not restores: raise RuntimeError('Full engine stack/exception checks missing')
    return generated,writes,restores


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    mode=parser.add_mutually_exclusive_group()
    mode.add_argument('--no-input',action='store_true',help='Exercise the unskipped original opening without synthetic input')
    mode.add_argument('--continue-load',action='store_true',help='Select Continue from the original main menu using the existing bootstrap save')
    mode.add_argument('--continue-walk-out',action='store_true',help='Continue the original home checkpoint, then walk through its exit')
    mode.add_argument('--continue-starter',action='store_true',help='Continue the home checkpoint, walk to the lab and select the original starter')
    parser.add_argument('--host-only',action='store_true',help='Skip the iPhoneOS archive while diagnosing native bootstrap failures')
    args=parser.parse_args()
    audit=BUILD/'audit'; module=audit/'engine.wasm'
    link=json.loads((audit/'engine-link.json').read_text())
    if link['exitCode'] or not module.is_file() or hashlib.sha256(module.read_bytes()).hexdigest()!=link['outputSha256']:
        raise RuntimeError('Run check_abi32.py --audit first; strict engine link is unavailable')
    pack=verify(link.get('nativeContent')); boot_header=read_boot_header(pack)
    destination=BUILD/'engine-native'; destination.mkdir(exist_ok=True)
    boot_header_file=destination/'boot-header.bin'; boot_header_file.write_bytes(boot_header)
    report=destination/'report.json'; report.unlink(missing_ok=True)
    binary=destination/'engine-check'; binary.unlink(missing_ok=True)
    (destination/'bootstrap.log').unlink(missing_ok=True)
    frames=destination/'frames'; frames.mkdir(exist_ok=True)
    for pattern in ('frame-*.ppm','frame-*.png'):
        for path in frames.glob(pattern): path.unlink()
    (destination/'iphoneos/libHeartGoldNativeEngine.a').unlink(missing_ok=True)
    def run(command,log):
        result=subprocess.run(list(map(str,command)),cwd=ROOT,capture_output=True,text=True)
        (destination/log).write_text(result.stdout+result.stderr)
        if result.returncode: raise RuntimeError(f'Native engine build failed: {destination/log}')
    generated,writes,restores=generate_engine(destination,module,run)
    flags,sources=native_build_settings(destination)
    flags+=['-g','-fsanitize=address,undefined','-fno-sanitize-recover=all']
    sources=[*generated,*sources,ROOT/'ios/Original/abi32/engine_check.c']
    def compile_one(item):
        index,source=item; target=destination/f'host-{index}.o'
        trap_flags=['-DWASM_RT_TRAP_HANDLER=HGEngineTrap'] if source.name=='wasm-rt-impl.c' else []
        run(['xcrun','clang',*flags,*trap_flags,'-c',source,'-o',target],f'compile-{index}.log')
        return target
    with ThreadPoolExecutor(max_workers=4) as pool: objects=list(pool.map(compile_one,enumerate(sources)))
    run(['xcrun','clang','-fsanitize=address,undefined',*objects,'-o',binary],'link.log')
    try:
        command=[str(binary),str(pack/'files'),str(boot_header_file),str(destination/'bootstrap.sav'),str(frames)]
        if args.no_input: command.append('--no-input')
        if args.continue_load: command.append('--continue')
        if args.continue_walk_out: command.append('--continue-walk-out')
        if args.continue_starter: command.append('--continue-starter')
        result=subprocess.run(command,cwd=ROOT,capture_output=True,text=True,timeout=260 if args.continue_starter else 50 if args.continue_load else 70 if args.no_input or args.continue_walk_out else 235)
        status='returned' if result.returncode==0 else 'failed'
        output=result.stdout+result.stderr; exit_code=result.returncode
    except subprocess.TimeoutExpired as error:
        status='timeout'; exit_code=None
        output=(error.stdout or b'').decode()+(error.stderr or b'').decode()
    (destination/'bootstrap.log').write_text(output)
    result_report=dict(scope='native host/iPhoneOS compilation and bounded original NitroMain bootstrap; no App/gameplay verification',
        intermediateSha256=link['outputSha256'],nativeSha256=hashlib.sha256(binary.read_bytes()).hexdigest(),
        nativeContent=link.get('nativeContent'),
        bootHeaderSha256=hashlib.sha256(boot_header).hexdigest(),
        generatedUnits=len(generated),checkedStackWrites=writes,exceptionDepthRestores=restores,
        bootstrapStatus=status,bootstrapExitCode=exit_code,bootstrapInput='continue-starter' if args.continue_starter else 'continue-walk-out' if args.continue_walk_out else 'continue' if args.continue_load else 'none' if args.no_input else 'original-playthrough',deviceArchiveSha256=None,
        renderedFrames=[dict(path=str(p.relative_to(ROOT)),sha256=hashlib.sha256(p.read_bytes()).hexdigest()) for p in sorted(frames.glob('frame-*.ppm'))])
    report.write_text(json.dumps(result_report,indent=2)+'\n')
    if args.host_only:
        print(f'Native host built; bootstrap {status}; see {destination/"bootstrap.log"}')
        return 1
    device=destination/'iphoneos'; device.mkdir(exist_ok=True)
    device_flags,device_sources=native_build_settings(destination)
    def compile_device(item):
        index,source=item; target=device/f'unit-{index}.o'
        run(['xcrun','--sdk','iphoneos','clang','-target','arm64-apple-ios16.0',*device_flags,
             '-c',source,'-o',target],f'device-{index}.log')
        return target
    with ThreadPoolExecutor(max_workers=4) as pool:
        device_objects=list(pool.map(compile_device,enumerate([*generated,*device_sources])))
    archive=device/'libHeartGoldNativeEngine.a'; archive.unlink(missing_ok=True)
    run(['xcrun','ar','rcs',archive,*device_objects],'device-archive.log')
    result_report['deviceArchiveSha256']=hashlib.sha256(archive.read_bytes()).hexdigest()
    report.write_text(json.dumps(result_report,indent=2)+'\n')
    print(f'Native engine built; bootstrap {status}; see {destination/"bootstrap.log"}')
    print(f'iPhoneOS native engine archive: {archive}')
    return 1  # NitroMain must reach verified frames before bootstrap can pass.


if __name__=='__main__':
    raise SystemExit(main())
