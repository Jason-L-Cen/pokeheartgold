# SPDX-License-Identifier: GPL-3.0-or-later
"""Shadow-link the real StripeWipe/IRQ/raster chain from an existing ABI32 probe cache.

Run check_abi32.py first to prepare the SDK objects. This test never regenerates
shared headers or changes that cache, the audited engine, an App, or a save.
"""
from concurrent.futures import ThreadPoolExecutor
import hashlib
import json
from pathlib import Path
import re
import shutil
import subprocess
import sys

ROOT=Path(__file__).resolve().parents[2]
sys.path.insert(0,str(ROOT/'ios/tools'))
from check_abi32 import BUILD,SDK,WABT,native_build_settings
from build_original_data import wasm_object


def main():
    destination=BUILD/'stripe-wipe-check'; destination.mkdir(exist_ok=True)
    report=destination/'report.json'; report.unlink(missing_ok=True)
    original=json.loads((BUILD/'probe-link-command.json').read_text())
    commands=[]
    def run(command,log,allow_failure=False,timeout=None):
        command=list(map(str,command)); commands.append(command)
        result=subprocess.run(command,cwd=ROOT,capture_output=True,text=True,timeout=timeout)
        (destination/log).write_text(result.stdout+result.stderr)
        if result.returncode and not allow_failure: raise RuntimeError(str(destination/log))
        return result

    cached_objects=[Path(arg) for arg in original if arg.endswith('.o')]
    if not cached_objects or any(not p.is_file() for p in cached_objects):
        raise RuntimeError('Run ios/tools/check_abi32.py to populate the actual probe object cache first')
    cached_hashes={str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in cached_objects}
    old_client=destination/'no-raster-client.o'
    shutil.copyfile(BUILD/'NativeIRQClient.o',old_client)
    flags=['--target=wasm32-wasip1','-O2','-fwrapv','-fno-strict-aliasing',
           '-fno-builtin-malloc','-fno-builtin-free','-fwasm-exceptions','-fexceptions',
           '-Xclang','-disable-red-zone','-fms-extensions','-Wno-microsoft','-Wno-unknown-pragmas']
    flags+=['-D'+name for name in ['HEARTGOLD_NATIVE','HEARTGOLD_ABI32','SDK_ARM9','SDK_CW',
        'SDK_CODE_ARM','SDK_TS','_NITRO','PM_KEEP_ASSERTS','PLATFORM_INTRINSIC_FUNCTION_BIT_CLZ32=__builtin_clz']]
    for path in ['ios/build-original-host/include','include','files','src','lib/include','lib/include/cw',
                 'lib/include/nitro','lib/include/nnsys','lib/include/nitro/os','include/library',
                 'lib/NitroSDK/include','lib/NitroSDK/include-pokediamond']:
        flags+=['-I',ROOT/path]
    sources=['src/system.c','src/sys_task.c','src/sys_task_api.c',
             'src/field/encounter_effect.c','src/field/encounter_effect_wipe.c','src/field/hblank_system.c',
             'lib/NitroSDK/src/fx/fx_cp.c','lib/NitroSDK/src/gx/g2.c',
             'ios/Original/NativeGraphicsMemory.c','ios/Original/NativeGeometry.c',
             'ios/Original/abi32/NativeIRQClient.c','ios/Original/Native2D.cpp',
             'ios/Original/abi32/stripe_wipe_check.c']
    host_flags,runtime=native_build_settings(destination)
    tracked=[ROOT/name for name in sources]+runtime+[
        ROOT/'ios/Original/tests/stripe_wipe_host_check.c',Path(__file__),
        ROOT/'ios/ThirdParty/libntr-g2/libraries/sim/src/sim_g2.cpp',
        ROOT/'lib/include/nitro/hw/native_graphics.h',ROOT/'lib/include/nitro/os/native_irq.h',
        ROOT/'ios/Original/NativeEvents.h',ROOT/'ios/Original/abi32/NativeImports.h']
    source_hashes={str(p.relative_to(ROOT)) if p.is_relative_to(ROOT) else str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in tracked}
    replacements={}
    for name in sources:
        source=ROOT/name; target=destination/(source.stem+'.o')
        cpp=source.suffix=='.cpp'
        options=['-std=gnu++17' if cpp else '-std=gnu11',*flags]
        if cpp: options+=['-iquote',ROOT/'ios/Original']
        run([SDK/('bin/clang++' if cpp else 'bin/clang'),*options,'-c',source,'-o',target],source.stem+'-compile.log')
        replacements[str(BUILD/(source.stem+'.o'))]=target

    negative='cached pre-Stage1 client without automatic kind3 registration'
    if any(s.get('name')=='HGNativeRasterStep' for s in wasm_object(old_client.read_bytes())['symbols']):
        # Reproduce precisely the missing old binding if the cache has already been refreshed.
        source=(ROOT/'ios/Original/abi32/NativeIRQClient.c').read_text()
        removed,count=re.subn(r'^\s*HGNativeIrqSet\(HG_IRQ_RASTER,0,\(u32\)\(uaddr\)HGNativeRasterStep,0\);\n',
                              '\n',source,flags=re.M)
        if count!=1: raise RuntimeError('Cannot identify the single automatic raster registration for the negative control')
        path=destination/'no-raster-client.c'; path.write_text(removed)
        run([SDK/'bin/clang','-std=gnu11',*flags,'-c',path,'-o',old_client],'no-raster-client-compile.log')
        negative='current original client copy with only automatic kind3 registration removed'

    exports=['InitializeOriginalGameHeap','HGIRQInit','HGStripeSetup','HGStripeAdvance','HGStripeCheckFrame',
             'HGStripeStop','HGStripeCheckStopped','HGStripeFinish','HGStripeDisplayStatus','HGStripeFailure','HGStripePixels']
    modules={}
    for label in ['current','no-auto-raster']:
        directory=destination/label; directory.mkdir(exist_ok=True)
        command=[]; used=set()
        for argument in original[:-2]:
            if argument.startswith('-Wl,--export='): continue
            if argument in replacements:
                used.add(argument); argument=replacements[argument]
            command.append(argument)
        command += [p for key,p in replacements.items() if key not in used]
        if label=='no-auto-raster':
            command=[old_client if str(arg)==str(destination/'NativeIRQClient.o') else arg for arg in command]
        module=directory/'stripe.wasm'
        command += ['-Wl,--export='+name for name in exports]+['-o',module]
        run(command,label+'-link.log')
        run([WABT/'bin/wasm2c',module,'-n','stripe','-o',directory/'stripe.c'],label+'-generate.log')
        if re.search(r'w2c_\w*bitcast_invalid\w*',(directory/'stripe.c').read_text()):
            raise RuntimeError('Invalid typed indirect call in StripeWipe component')
        modules[label]=dict(moduleSha256=hashlib.sha256(module.read_bytes()).hexdigest())

    host=['xcrun','clang',*host_flags,'-I',ROOT/'ios/Original','-O1','-g',
          '-fsanitize=address,undefined','-fno-sanitize-recover=all']
    def compile_runtime(item):
        index,source=item; target=destination/f'host-{index}.o'
        run([*host,'-c',source,'-o',target],f'host-{index}.log'); return target
    with ThreadPoolExecutor(max_workers=4) as pool:
        native_objects=list(pool.map(compile_runtime,enumerate(runtime)))
    for label in modules:
        directory=destination/label; binary=directory/'stripe-check'
        run([*host,'-I',directory,directory/'stripe.c',ROOT/'ios/Original/tests/stripe_wipe_host_check.c',
             *native_objects,'-Wl,-dead_strip','-o',binary],label+'-native-link.log')
        result=run([binary],label+'-runtime.log',allow_failure=True,timeout=30)
        modules[label].update(exitCode=result.returncode,nativeSha256=hashlib.sha256(binary.read_bytes()).hexdigest(),
                              stdout=result.stdout,stderr=result.stderr)
        if label=='current':
            if result.returncode or result.stdout.count('PASS: original StripeWipe')!=2:
                raise RuntimeError('Real StripeWipe current component failed: '+str(destination/(label+'-runtime.log')))
        elif result.returncode==0 or not re.search(r'Stripe fixture line \d+: sign=1 phase=0 pixel=0,0 expected=3e0 actualRGBA=ff0000ff',result.stderr):
            raise RuntimeError('Missing-raster negative control did not fail the actual pixel fixture')
    if any(hashlib.sha256(p.read_bytes()).hexdigest()!=cached_hashes[str(p)] for p in cached_objects):
        raise RuntimeError('The shared probe cache changed during this shadow check')
    if any(hashlib.sha256(p.read_bytes()).hexdigest()!=source_hashes[str(p.relative_to(ROOT)) if p.is_relative_to(ROOT) else str(p)] for p in tracked):
        raise RuntimeError('StripeWipe source/header snapshot changed during the check')
    report.write_text(json.dumps(dict(passed=True,scope='original StripeWipe tasks, Main HBlank callback, automatic kind3, real Events, per-row BG/window pixels and cleanup; no HDMA/App/battle claim',
        sourceHashes=source_hashes,negativeControl=negative,modules=modules,commands=commands,
        sharedCacheUnchanged=True),indent=2)+'\n')
    print('PASS: real StripeWipe positive/negative directions, two original Update frames, exact row9/17/window/interpolation pixels, no writes after disable; missing kind3 fails at runtime.')


if __name__=='__main__': main()
