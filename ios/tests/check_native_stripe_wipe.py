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
import struct
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

    cached_objects=[Path(arg) for arg in original if arg.endswith(('.o','.a'))]
    if not cached_objects or any(not p.is_file() for p in cached_objects):
        raise RuntimeError('Run ios/tools/check_abi32.py to populate the actual probe object cache first')
    cached_hashes={str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in cached_objects}
    old_client=destination/'no-raster-client.o'
    shutil.copyfile(BUILD/'NativeIRQClient.o',old_client)
    # Independent integer oracle from the original data, also checked against the selected real object.
    sine_source=ROOT/'lib/NitroSDK/asm/fx_sincos.s'
    values=[int(value.strip(),0) for row in re.findall(r'^\s*\.short ([^\n]+)',sine_source.read_text(),re.M) for value in row.split(',')]
    if len(values)!=8192: raise RuntimeError('Unexpected original sine/cosine table size')
    raw=struct.pack('<8192H',*values)
    table=wasm_object((BUILD/'data-check/sincos.o').read_bytes(),contents=True)
    symbol=next(s for s in table['symbols'] if s.get('name')=='FX_SinCosTable_' and 'segment' in s)
    if table['segments'][symbol['segment']]['bytes'][symbol['offset']:symbol['offset']+len(raw)]!=raw:
        raise RuntimeError('Selected sine object differs from the original table')
    waves=[]
    for step in (341,364,910):
        sine=[values[(((i*step)&65535)>>4)*2] for i in range(192)]
        waves.append([((((s if s<32768 else s-65536)*32*4096+2048)>>12)>>12) for s in sine])
    if [[wave[i] for i in (16,48,96,144,191)] for wave in waves]!=[[15,32,0,-32,-2],[16,31,-7,-31,11],[31,-28,27,-1,-27]]:
        raise RuntimeError('Original sine precision/golden samples changed')
    (destination/'stripe-wave-golden.h').write_text('static const s16 qaWave[3][192]={\n'+',\n'.join('{'+','.join(map(str,wave))+'}' for wave in waves)+'\n};\n')
    flags=['--target=wasm32-wasip1','-O2','-fwrapv','-fno-strict-aliasing',
           '-fno-builtin-malloc','-fno-builtin-free','-fwasm-exceptions','-fexceptions',
           '-Xclang','-disable-red-zone','-fms-extensions','-Wno-microsoft','-Wno-unknown-pragmas']
    flags+=['-D'+name for name in ['HEARTGOLD_NATIVE','HEARTGOLD_ABI32','SDK_ARM9','SDK_CW',
        'SDK_CODE_ARM','SDK_TS','_NITRO','PM_KEEP_ASSERTS','PLATFORM_INTRINSIC_FUNCTION_BIT_CLZ32=__builtin_clz']]
    flags+=['-I',destination]
    for path in ['ios/build-original-host/include','include','files','src','lib/include','lib/include/cw',
                 'lib/include/nitro','lib/include/nnsys','lib/include/nitro/os','include/library',
                 'lib/NitroSDK/include','lib/NitroSDK/include-pokediamond']:
        flags+=['-I',ROOT/path]
    sources=['src/system.c','src/sys_task.c','src/sys_task_api.c',
             'src/field/encounter_effect.c','src/field/encounter_effect_wipe.c','src/field/hblank_system.c',
             'src/battle/battle_anim_util.c','src/scanline_effect.c','src/frontier/frontier_transition.c',
             'lib/NitroSDK/src/mi/mi_dma.c','lib/NitroSDK/src/mi/mi_dma_hblank.c','lib/NitroSDK/src/mi/mi_memory.c',
             'lib/NitroSDK/src/fx/fx_cp.c','lib/NitroSDK/src/gx/g2.c',
             'ios/Original/NativeGraphicsMemory.c','ios/Original/NativeGeometry.c',
             'ios/Original/abi32/NativeIRQClient.c','ios/Original/Native2D.cpp',
             'ios/Original/abi32/stripe_wipe_check.c']
    host_flags,runtime=native_build_settings(destination)
    tracked=[ROOT/name for name in sources]+runtime+[
        ROOT/'ios/Original/tests/stripe_wipe_host_check.c',Path(__file__),
        ROOT/'ios/ThirdParty/libntr-g2/libraries/sim/src/sim_g2.cpp',
        ROOT/'lib/include/nitro/hw/native_graphics.h',ROOT/'lib/include/nitro/os/native_irq.h',
        ROOT/'ios/Original/NativeEvents.h',ROOT/'ios/Original/abi32/NativeImports.h',
        ROOT/'ios/Original/NativeDMA.h',ROOT/'lib/include/nitro/mi/native_dma.h',sine_source]
    source_hashes={str(p.relative_to(ROOT)) if p.is_relative_to(ROOT) else str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in tracked}
    replacements={}
    for name in sources:
        source=ROOT/name; target=destination/(source.stem+'.o')
        cpp=source.suffix=='.cpp'
        options=['-std=gnu++17' if cpp else '-std=gnu11',*flags]
        if cpp: options+=['-iquote',ROOT/'ios/Original']
        run([SDK/('bin/clang++' if cpp else 'bin/clang'),*options,'-c',source,'-o',target],source.stem+'-compile.log')
        replacements[str(BUILD/(source.stem+'.o'))]=target
    drop_mask=destination/'no-dma-affine-mask.o'
    run([SDK/'bin/clang','-std=gnu11',*flags,'-DSTRIPE_DROP_DMA_MASK','-c',ROOT/'ios/Original/abi32/stripe_wipe_check.c','-o',drop_mask],
        'no-dma-affine-mask-compile.log')

    negative='cached pre-Stage1 client without automatic kind3 registration'
    if any(s.get('name')=='HGNativeRasterStep' for s in wasm_object(old_client.read_bytes())['symbols']):
        # Reproduce precisely the missing old binding if the cache has already been refreshed.
        source=(ROOT/'ios/Original/abi32/NativeIRQClient.c').read_text()
        removed,count=re.subn(r'^\s*HGNativeIrqSet\(HG_IRQ_RASTER,0,\(u32\)\(uaddr\)(?:HGNativeRasterStep|rasterStep),0\);\n',
                              '\n',source,flags=re.M)
        if count!=1: raise RuntimeError('Cannot identify the single automatic raster registration for the negative control')
        path=destination/'no-raster-client.c'; path.write_text(removed)
        run([SDK/'bin/clang','-std=gnu11',*flags,'-c',path,'-o',old_client],'no-raster-client-compile.log')
        negative='current original client copy with only automatic kind3 registration removed'

    exports=['InitializeOriginalGameHeap','HGIRQInit','HGStripeSetup','HGStripeAdvance','HGStripeCheckFrame',
             'HGStripeStop','HGStripeCheckStopped','HGStripeFinish','HGStripeDisplayStatus','HGStripeFailure','HGStripePixels']
    exports+=['HGScanline'+name for name in ('Setup','Advance','CheckFrame','Stop','CheckStopped','Finish','Epochs','CheckMasked')]
    modules={}
    for label in ['current','no-auto-raster','no-dma-affine-mask']:
        directory=destination/label; directory.mkdir(exist_ok=True)
        command=[]; used=set()
        for argument in original[:-2]:
            if argument.startswith('-Wl,--export='): continue
            if argument==str(BUILD/'battle_wave_check.o'): continue # This unit includes mock versions of both real source TUs.
            if argument in replacements:
                used.add(argument); argument=replacements[argument]
            command.append(argument)
        command += [p for key,p in replacements.items() if key not in used]
        if label=='no-auto-raster':
            command=[old_client if str(arg)==str(destination/'NativeIRQClient.o') else arg for arg in command]
        if label=='no-dma-affine-mask':
            command=[drop_mask if str(arg)==str(destination/'stripe_wipe_check.o') else arg for arg in command]
        module=directory/'stripe.wasm'
        command += ['-Wl,--export='+name for name in exports]+['-Wl,--wrap=HGNativeRasterStep','-o',module]
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
        result=run([binary],label+'-runtime.log',allow_failure=True,timeout=60)
        modules[label].update(exitCode=result.returncode,nativeSha256=hashlib.sha256(binary.read_bytes()).hexdigest(),
                              stdout=result.stdout,stderr=result.stderr)
        if label=='current':
            if result.returncode or result.stdout.count('PASS: original StripeWipe')!=2 or result.stdout.count('PASS: original HDMA')!=9:
                raise RuntimeError('Real StripeWipe current component failed: '+str(destination/(label+'-runtime.log')))
        elif label=='no-auto-raster':
            if result.returncode==0 or not re.search(r'Stripe fixture line \d+: sign=1 phase=0 pixel=0,0 expected=3e0 actualRGBA=ff0000ff',result.stderr):
                raise RuntimeError('Missing-raster negative control did not fail the actual pixel fixture')
        elif result.returncode==0 or not re.search(r'HDMA fixture line \d+: kind=2 mode=0 masked=0 phase=0 pixel=0,1 expected=1f actualRGBA=ff00ff00',result.stderr):
            raise RuntimeError('Missing DMA affine notification did not fail the real Frontier pixel fixture')
    if any(hashlib.sha256(p.read_bytes()).hexdigest()!=cached_hashes[str(p)] for p in cached_objects):
        raise RuntimeError('The shared probe cache changed during this shadow check')
    if any(hashlib.sha256(p.read_bytes()).hexdigest()!=source_hashes[str(p.relative_to(ROOT)) if p.is_relative_to(ROOT) else str(p)] for p in tracked):
        raise RuntimeError('StripeWipe source/header snapshot changed during the check')
    report.write_text(json.dumps(dict(passed=True,scope='original StripeWipe and real Wave/Frontier HBlank DMA, tasks, clock, per-row pixels, masked CPU delivery, cleanup; no App/full-battle claim',
        sourceHashes=source_hashes,negativeControl=negative,modules=modules,commands=commands,
        sineSha256=hashlib.sha256(raw).hexdigest(),sharedCacheUnchanged=True),indent=2)+'\n')
    print('PASS: original StripeWipe plus two-frame Wave/Frontier 16/32-bit HDMA pixels, repeated affine reload, CPSR-masked hardware, queues/cleanup; missing kind3 and DMA reload notifications fail at runtime.')


if __name__=='__main__': main()
