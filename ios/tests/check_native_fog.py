# SPDX-License-Identifier: GPL-3.0-or-later
"""Run original fog setters/weather fade and real native epochs in an isolated shadow component."""
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

OLD_COMMIT='9e70c2d9fd1281ce6251dd23b33f811ef9b68e9e'
OLD_SHA='95af74ff010b3d98f3b1ee483424f2b16bfd2505a15203a6c8ec1ee6ee1af0e8'
HOST_MAIN=r'''
#include "fog.h"
#include "NativeImports.h"
#include "NativeWASIImports.h"
#include "NativeDMA.h"
#include "NativeEvents.h"
#include "wasm-rt-impl.h"
#include "wasm-rt-exceptions.h"
#include <stdio.h>
#include <unistd.h>
typedef struct { w2c_fog *game; u32 phase,line,frames; } Capture;
static void present(void *argument) {
 Capture *c=argument; c->frames++;
 c->line=w2c_fog_HGFogClockCheck(c->game,c->phase);
 if(!c->line) c->line=c->phase ? w2c_fog_HGFogClockStop(c->game) : w2c_fog_HGFogClockAdvance(c->game);
 c->phase++;
}
int main(void) {
 wasm_rt_init(); w2c_fog game; struct w2c_env env={0};
 struct w2c_wasi__snapshot__preview1 wasi={NULL,{dup(0),dup(1),dup(2)}};
 wasm2c_fog_instantiate(&game,&env,&wasi); env.memory=wasi.memory=w2c_fog_memory(&game);
 env.irqTable=&game.w2c_T0; env.stackPointer=&game.w2c_0x5F_stack_pointer;
 env.rootStackHigh=*env.stackPointer; *env.stackPointer-=16;
 env.irqType0=wasm2c_fog_get_func_type(0,0); env.irqType1=wasm2c_fog_get_func_type(1,0,WASM_RT_I32);
 env.rtcType=wasm2c_fog_get_func_type(2,0,WASM_RT_I32,WASM_RT_I32);
 env.irqGuardType=wasm2c_fog_get_func_type(4,0,WASM_RT_I32,WASM_RT_I32,WASM_RT_I32,WASM_RT_I32);
 Capture c={&game,0,0,0}; wasm_rt_trap_t trap=wasm_rt_impl_try();
 if(!trap) {
  w2c_fog_0x5Finitialize(&game); w2c_fog_HGIRQInit(&game);
#ifdef NEGATIVE_CONTROL
  c.line=w2c_fog_CheckOriginalFog3D(&game);
  if(c.line) fprintf(stderr,"Fog fixture stage %u line %u\n",w2c_fog_HGFogAcceptanceStage(&game),c.line);
#else
  c.line=w2c_fog_CheckOriginalRaster3D(&game);
  if(c.line) fprintf(stderr,"Raster/Fog fixture line %u\n",c.line);
  if(!c.line) c.line=w2c_fog_CheckOriginalAlpha3D(&game);
  if(!c.line) c.line=w2c_fog_HGFogClockSetup(&game);
  if(!c.line) {
   u32 status=w2c_fog_HGFogClockStatus(&game);
   if(!status || status>env.memory->size || 2>env.memory->size-status ||
      !HGNativeStartDisplayClock(&env,(const vu16 *)(env.memory->data+status)) ||
      !HGNativeSetDisplayObserver(&env,present,&c)) c.line=__LINE__;
   else while(c.phase<2 && !c.line) HGNativeHalt();
  }
  if(c.line) fprintf(stderr,"Fog clock/alpha fixture line %u phase %u\n",c.line,c.phase);
#endif
 }
 if(trap) { fprintf(stderr,"Fog native trap: %s\n",wasm_rt_strerror(trap)); _Exit(23); }
 HGNativeStopDisplayClock(&env); w2c_fog_HGFogClockStop(&game);
 if(__atomic_load_n(&env.pendingDMA,__ATOMIC_ACQUIRE)) c.line=__LINE__;
 HGNativeDrainDMA(); HGNativeReleaseIrqCallbacks(&env); wasm2c_fog_free(&game);
 for(u32 i=0;i<3;i++) if(wasi.fds[i]>=0) close(wasi.fds[i]); wasm_rt_free();
 if(!c.line) puts("PASS: unchanged raster/alpha goldens, original fog/weather pixels, 40B snapshots, two real no-SWAP epochs and packed DMA masks.");
 return c.line ? 1 : 0;
}
'''


def main():
    destination=BUILD/'fog-check'; destination.mkdir(exist_ok=True)
    report=destination/'report.json'; report.unlink(missing_ok=True)
    original=json.loads((BUILD/'probe-link-command.json').read_text())
    cached=[Path(a) for a in original if a.endswith(('.o','.a'))]
    if not cached or any(not p.is_file() for p in cached):
        raise RuntimeError('Run check_abi32.py first to populate the actual probe cache')
    if any(p.name in ('weather_manager.o','field_fog.o') for p in cached):
        raise RuntimeError('Fog fixture includes these actual TUs; duplicate selected providers need explicit migration')
    cache_hashes={str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in cached}
    commands=[]
    def run(command,log,allow_failure=False):
        command=list(map(str,command)); commands.append(command)
        result=subprocess.run(command,cwd=ROOT,capture_output=True,text=True,timeout=180)
        (destination/log).write_text(result.stdout+result.stderr)
        if result.returncode and not allow_failure: raise RuntimeError(str(destination/log))
        return result
    old_source=subprocess.run(['git','show',OLD_COMMIT+':ios/Original/Native3D.c'],cwd=ROOT,capture_output=True,check=True).stdout
    if hashlib.sha256(old_source).hexdigest()!=OLD_SHA: raise RuntimeError('Pinned original renderer changed')
    old=destination/'old-Native3D.c'; old.write_bytes(old_source)
    flags=['--target=wasm32-wasip1','-O2','-fwrapv','-fno-strict-aliasing','-fno-builtin-malloc',
           '-fno-builtin-free','-fwasm-exceptions','-fexceptions','-Xclang','-disable-red-zone',
           '-fms-extensions','-Wno-microsoft','-Wno-unknown-pragmas']
    flags+=['-D'+n for n in ('HEARTGOLD_NATIVE','HEARTGOLD_ABI32','SDK_ARM9','SDK_CW','SDK_CODE_ARM',
              'SDK_TS','_NITRO','PM_KEEP_ASSERTS','PLATFORM_INTRINSIC_FUNCTION_BIT_CLZ32=__builtin_clz')]
    for p in ('ios/build-original-host/include','include','files','src','lib/include','lib/include/cw',
              'lib/include/nitro','lib/include/nnsys','lib/include/nitro/os','include/library',
              'lib/NitroSDK/include','lib/NitroSDK/include-pokediamond'):
        flags+=['-I',ROOT/p]
    sources=['ios/Original/Native3D.c','ios/Original/NativeGeometry.c','ios/Original/NativeGraphicsMemory.c',
             'ios/Original/abi32/NativeIRQClient.c','ios/Original/Native2D.cpp',
             'ios/Original/tests/raster3d_check.c','ios/Original/tests/alpha3d_check.c',
             'lib/NitroSDK/src/gx/g3x.c','lib/NitroSDK/src/mi/mi_dma.c','lib/NitroSDK/src/mi/mi_dma_hblank.c',
             'lib/NitroSDK/src/mi/mi_memory.c']
    host_flags,runtime=native_build_settings(destination)
    tracked=[ROOT/name for name in sources]+runtime+[Path(__file__),
        ROOT/'src/field/weather_manager.c',ROOT/'src/field/field_fog.c',ROOT/'include/field/weather_manager.h',
        ROOT/'lib/include/nitro/gx/native_geometry.h',ROOT/'lib/include/nitro/hw/native_graphics.h',
        ROOT/'ios/Original/abi32/NativeImports.h',ROOT/'ios/Original/NativeDMA.h',
        ROOT/'ios/Original/NativeEvents.h',ROOT/'ios/ThirdParty/libntr-g2/libraries/sim/src/sim_g2.cpp']
    source_hashes={str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in tracked}
    replacements={}
    for name in sources:
        p=ROOT/name; target=destination/(p.stem+'.o'); cpp=p.suffix=='.cpp'
        extra=['-iquote',ROOT/'ios/Original'] if cpp else []
        run([SDK/('bin/clang++' if cpp else 'bin/clang'),'-std=gnu++17' if cpp else '-std=gnu11',
             *flags,*extra,'-c',p,'-o',target],p.stem+'-compile.log')
        replacements[str(BUILD/(p.stem+'.o'))]=target
    old_object=destination/'old-Native3D.o'
    run([SDK/'bin/clang','-std=gnu11',*flags,'-c',old,'-o',old_object],'old-compile.log')
    exports=['HGIRQInit','CheckOriginalRaster3D','CheckOriginalAlpha3D','CheckOriginalFog3D',
             'HGFogAcceptanceStage','HGFogClockSetup','HGFogClockStatus','HGFogClockCheck','HGFogClockAdvance','HGFogClockStop']
    modules={}
    for label in ('current','old-renderer'):
        directory=destination/label; directory.mkdir(exist_ok=True); command=[]; used=set()
        for argument in original[:-2]:
            if argument.startswith('-Wl,--export='): continue
            if argument in replacements: used.add(argument); argument=replacements[argument]
            if label=='old-renderer' and str(argument)==str(destination/'Native3D.o'): argument=old_object
            command.append(argument)
        command += [p for key,p in replacements.items() if key not in used]
        module=directory/'fog.wasm'; command+=['-Wl,--export='+n for n in exports]+['-o',module]
        run(command,label+'-link.log'); run([WABT/'bin/wasm2c',module,'-n','fog','-o',directory/'fog.c'],label+'-generate.log')
        if re.search(r'w2c_\w*bitcast_invalid\w*',(directory/'fog.c').read_text()): raise RuntimeError('Invalid typed fog callback')
        modules[label]=dict(moduleSha256=hashlib.sha256(module.read_bytes()).hexdigest())
    host=['xcrun','clang',*host_flags,'-I',ROOT/'ios/Original','-O1','-g',
          '-fsanitize=address,undefined','-fno-sanitize-recover=all']
    def compile_runtime(item):
        i,p=item; target=destination/f'host-{i}.o'
        run([*host,'-c',p,'-o',target],f'host-{i}.log'); return target
    with ThreadPoolExecutor(max_workers=4) as pool: native=list(pool.map(compile_runtime,enumerate(runtime)))
    main_source=destination/'main.c'; main_source.write_text(HOST_MAIN)
    for label in modules:
        directory=destination/label; binary=directory/'fog-check'
        options=['-DNEGATIVE_CONTROL'] if label=='old-renderer' else []
        run([*host,*options,'-I',directory,directory/'fog.c',main_source,*native,'-Wl,-dead_strip','-o',binary],label+'-native-link.log')
        result=run([binary],label+'-runtime.log',True)
        modules[label].update(exitCode=result.returncode,stdout=result.stdout,stderr=result.stderr,
                              nativeSha256=hashlib.sha256(binary.read_bytes()).hexdigest())
        if label=='current':
            if result.returncode or 'PASS: unchanged raster/alpha goldens' not in result.stdout: raise RuntimeError(str(destination/(label+'-runtime.log')))
        elif result.returncode==0 or not re.search(r'Fog fixture stage 1 line \d+',result.stderr):
            raise RuntimeError('Old renderer must fail the actual enabled eligible fog acceptance')
    if any(hashlib.sha256(p.read_bytes()).hexdigest()!=cache_hashes[str(p)] for p in cached): raise RuntimeError('Probe cache changed')
    if any(hashlib.sha256(p.read_bytes()).hexdigest()!=source_hashes[str(p)] for p in tracked): raise RuntimeError('Fog source/header snapshot changed')
    report.write_text(json.dumps(dict(passed=True,scope='native original fog setters/weather fade, final depth/eligibility, immutable fog snapshots, real no-SWAP epochs; no full weather map/App claim',
        sourceHashes=source_hashes,oldRendererCommit=OLD_COMMIT,oldRendererSha256=OLD_SHA,
        sharedCacheUnchanged=True,modules=modules,commands=commands),indent=2)+'\n')
    print('PASS: real fog/WeatherFade pixels, retained raster/alpha goldens, real no-SWAP epoch refresh and packed DMA masks; old renderer fails enabled fog.')


if __name__=='__main__': main()
