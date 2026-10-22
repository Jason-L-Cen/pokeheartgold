# SPDX-License-Identifier: GPL-3.0-or-later
"""Run the real 2D fixture and raster replay through a shadow ABI32 component.

Run ios/tools/check_abi32.py first to prepare the SDK/probe objects. This check
keeps that cache, shared headers, the engine, Apps and saves unchanged.
"""
from concurrent.futures import ThreadPoolExecutor
import hashlib
import json
from pathlib import Path
import re
import subprocess
import sys

ROOT=Path(__file__).resolve().parents[2]
sys.path.insert(0,str(ROOT/'ios/tools'))
from check_abi32 import BUILD,SDK,WABT,native_build_settings

HOST_MAIN=r'''
#include "hg32.h"
#include "NativeImports.h"
#include "NativeWASIImports.h"
#include "NativeDMA.h"
#include "wasm-rt-impl.h"
#include "wasm-rt-exceptions.h"
#include <stdio.h>
#include <unistd.h>
int main(void) {
 wasm_rt_init(); w2c_hg32 game; struct w2c_env native={0};
 struct w2c_wasi__snapshot__preview1 wasi={NULL,{dup(0),dup(1),dup(2)}};
 wasm2c_hg32_instantiate(&game,&native,&wasi); native.memory=wasi.memory=w2c_hg32_memory(&game);
 native.irqTable=&game.w2c_T0; native.stackPointer=&game.w2c_0x5F_stack_pointer;
 native.rootStackHigh=*native.stackPointer; *native.stackPointer-=16;
 native.irqType0=wasm2c_hg32_get_func_type(0,0);
 native.irqType1=wasm2c_hg32_get_func_type(1,0,WASM_RT_I32);
 native.rtcType=wasm2c_hg32_get_func_type(2,0,WASM_RT_I32,WASM_RT_I32);
 native.irqGuardType=wasm2c_hg32_get_func_type(4,0,WASM_RT_I32,WASM_RT_I32,WASM_RT_I32,WASM_RT_I32);
 wasm_rt_trap_t trap=wasm_rt_impl_try();
 if(!trap) {
  w2c_hg32_HGIRQInit(&game);
#ifndef NEGATIVE_CONTROL
  w2c_hg32_CheckNative2D(&game);
#endif
  w2c_hg32_CheckNativeRaster2D(&game);
 }
 if(trap) fprintf(stderr,"Real render fixture trapped: %s (WASI exit %u)\n",wasm_rt_strerror(trap),wasi.exitCode);
 HGNativeDrainDMA(); HGNativeReleaseIrqCallbacks(&native);
 wasm2c_hg32_free(&game);
 for(unsigned i=0;i<3;i++) if(wasi.fds[i]>=0) close(wasi.fds[i]);
 wasm_rt_free(); return trap ? 23 : 0;
}
'''


def main():
    destination=BUILD/'raster-2d-check'; destination.mkdir(exist_ok=True)
    report=destination/'report.json'; report.unlink(missing_ok=True)
    original=json.loads((BUILD/'probe-link-command.json').read_text())
    cached=[Path(arg) for arg in original if arg.endswith(('.o','.a'))]
    if not cached or any(not p.is_file() for p in cached):
        raise RuntimeError('Run ios/tools/check_abi32.py to populate the actual probe object cache first')
    cached_hashes={str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in cached}
    commands=[]
    def run(command,log,allow_failure=False):
        command=list(map(str,command)); commands.append(command)
        result=subprocess.run(command,cwd=ROOT,capture_output=True,text=True,timeout=180)
        (destination/log).write_text(result.stdout+result.stderr)
        if result.returncode and not allow_failure: raise RuntimeError(str(destination/log))
        return result

    flags=['--target=wasm32-wasip1','-O2','-fwrapv','-fno-strict-aliasing','-fno-builtin-malloc',
           '-fno-builtin-free','-fwasm-exceptions','-fexceptions','-Xclang','-disable-red-zone',
           '-fms-extensions','-Wno-microsoft','-Wno-unknown-pragmas']
    flags+=['-D'+name for name in ['HEARTGOLD_NATIVE','HEARTGOLD_ABI32','SDK_ARM9','SDK_CW',
        'SDK_CODE_ARM','SDK_TS','_NITRO','PM_KEEP_ASSERTS','PLATFORM_INTRINSIC_FUNCTION_BIT_CLZ32=__builtin_clz']]
    for path in ['ios/build-original-host/include','include','files','src','lib/include','lib/include/cw',
                 'lib/include/nitro','lib/include/nnsys','lib/include/nitro/os','include/library',
                 'lib/NitroSDK/include','lib/NitroSDK/include-pokediamond']:
        flags+=['-I',ROOT/path]
    sources=['ios/Original/Native2D.cpp','ios/Original/tests/render_check.c',
             'ios/Original/NativeGraphicsMemory.c','ios/Original/NativeGeometry.c','lib/NitroSDK/src/gx/g2.c']
    host_flags,runtime=native_build_settings(destination)
    tracked=[ROOT/name for name in sources]+runtime+[Path(__file__),
        ROOT/'ios/ThirdParty/libntr-g2/libraries/sim/src/sim_g2.cpp',
        ROOT/'lib/include/nitro/hw/native_graphics.h',ROOT/'ios/Original/Native2DRaster.h',
        ROOT/'ios/Original/abi32/NativeImports.h',ROOT/'ios/build-original-host/include/hg_native_registers.h']
    source_hashes={str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in tracked}
    replacements={}
    for name in sources:
        source=ROOT/name; target=destination/(source.stem+'.o'); cpp=source.suffix=='.cpp'
        run([SDK/('bin/clang++' if cpp else 'bin/clang'),'-std=gnu++17' if cpp else '-std=gnu11',
             *flags,'-iquote',ROOT/'ios/Original','-c',source,'-o',target],source.stem+'-compile.log')
        replacements[str(BUILD/(source.stem+'.o'))]=target

    renderer=(ROOT/sources[0]).read_text()
    draw='G2SIM_DrawBGLine(bg,bgMode,0,engine,y);'
    if renderer.count(draw)!=1: raise RuntimeError('Cannot identify the single row dispatch for the negative control')
    negative_source=destination/'whole-frame-dispatch.cpp'
    negative_source.write_text(renderer.replace(draw,'G2SIM_DrawBG(bg,bgMode,0,engine);'))
    negative_object=destination/'whole-frame-dispatch.o'
    run([SDK/'bin/clang++','-std=gnu++17',*flags,'-iquote',ROOT/'ios/Original','-c',negative_source,
         '-o',negative_object],'negative-compile.log')
    include='#include "../ThirdParty/libntr-g2/libraries/sim/src/sim_g2.cpp"'
    if renderer.count(include)!=1: raise RuntimeError('Cannot identify affine samplers for the negative control')
    no_reload_source=destination/'no-affine-reload.cpp'
    no_reload_source.write_text(renderer.replace(include,'#define HGNativeRasterAffineWrites(engine,line) 0u\n'+include))
    no_reload_object=destination/'no-affine-reload.o'
    run([SDK/'bin/clang++','-std=gnu++17',*flags,'-iquote',ROOT/'ios/Original','-c',no_reload_source,
         '-o',no_reload_object],'no-affine-reload-compile.log')
    modules={}
    for label,renderer_object in [('current',destination/'Native2D.o'),
                                  ('whole-frame-dispatch',negative_object),('no-affine-reload',no_reload_object)]:
        directory=destination/label; directory.mkdir(exist_ok=True); command=[]; used=set()
        for argument in original[:-2]:
            if argument.startswith('-Wl,--export='): continue
            if argument in replacements:
                used.add(argument); argument=replacements[argument]
            if str(argument)==str(destination/'Native2D.o'): argument=renderer_object
            command.append(argument)
        command += [p for key,p in replacements.items() if key not in used]
        module=directory/'hg32.wasm'
        command += ['-Wl,--export='+name for name in ['HGIRQInit','CheckNative2D','CheckNativeRaster2D']]+['-o',module]
        run(command,label+'-link.log')
        run([WABT/'bin/wasm2c',module,'-n','hg32','-o',directory/'hg32.c'],label+'-generate.log')
        if re.search(r'w2c_\w*bitcast_invalid\w*',(directory/'hg32.c').read_text()):
            raise RuntimeError('Invalid typed indirect call in raster component')
        modules[label]=dict(moduleSha256=hashlib.sha256(module.read_bytes()).hexdigest())
    host=['xcrun','clang',*host_flags,'-I',ROOT/'ios/Original','-O1','-g',
          '-fsanitize=address,undefined','-fno-sanitize-recover=all']
    def compile_runtime(item):
        index,source=item; target=destination/f'host-{index}.o'
        run([*host,'-c',source,'-o',target],f'host-{index}.log'); return target
    with ThreadPoolExecutor(max_workers=4) as pool:
        native_objects=list(pool.map(compile_runtime,enumerate(runtime)))
    host_main=destination/'main.c'; host_main.write_text(HOST_MAIN)
    for label in modules:
        directory=destination/label; binary=directory/'render-check'
        options=[] if label=='current' else ['-DNEGATIVE_CONTROL']
        run([*host,*options,'-I',directory,directory/'hg32.c',host_main,*native_objects,
             '-Wl,-dead_strip','-o',binary],label+'-native.log')
        result=run([binary],label+'-runtime.log',allow_failure=True)
        modules[label].update(exitCode=result.returncode,nativeSha256=hashlib.sha256(binary.read_bytes()).hexdigest())
        if label=='current':
            if result.returncode or 'PASS: committed per-row' not in result.stdout:
                raise RuntimeError('Actual raster fixture failed: '+str(destination/(label+'-runtime.log')))
        elif result.returncode==0 or not re.search(r'Render check line \d+:',result.stderr):
            raise RuntimeError(label+' negative control did not fail the actual pixel fixture')
    if any(hashlib.sha256(p.read_bytes()).hexdigest()!=cached_hashes[str(p)] for p in cached):
        raise RuntimeError('The shared probe cache changed during the shadow check')
    if any(hashlib.sha256(p.read_bytes()).hexdigest()!=source_hashes[str(p)] for p in tracked):
        raise RuntimeError('Raster source/header snapshot changed during the check')
    report.write_text(json.dumps(dict(passed=True,scope='actual CheckNative2D plus committed scanline register replay, mosaic, main/sub, widths, affine reloads and incomplete rejection; no HDMA/App claim',
        sourceHashes=source_hashes,sharedCacheUnchanged=True,modules=modules,commands=commands),indent=2)+'\n')
    print('PASS: actual renderer checks and row replay under ASan/UBSan; whole-frame dispatch and omitted affine reloads fail the same pixel fixture.')


if __name__=='__main__': main()
