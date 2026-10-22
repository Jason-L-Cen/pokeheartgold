# SPDX-License-Identifier: GPL-3.0-or-later
"""Check original particle capture tasks and the native frame-end producer in a shadow build.

Requires the existing ABI32 probe and verified original-data audit. No shared
headers, audit objects, App, save, or production module are generated or changed.
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
from build_original_data import wasm_object

OLD_COMMIT='0ac27fd974a9e87a2d10968e64bfbd333b93159c'
HOST_MAIN=r'''
#include "capture.h"
#include "NativeImports.h"
#include "NativeWASIImports.h"
#include "NativeEvents.h"
#include "NativeDMA.h"
#include "wasm-rt-impl.h"
#include "wasm-rt-exceptions.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
typedef struct { w2c_capture *game; struct w2c_env *env; u32 phase,line,frames,total; } Capture;
static u32 *witness(Capture *c) {
 u32 address=w2c_capture_HGCaptureWitness(c->game);
 if(address>c->env->memory->size || 40>c->env->memory->size-address) abort();
 return (u32 *)(c->env->memory->data+address);
}
static int start(Capture *c) {
 u32 status=w2c_capture_HGCaptureDisplayStatus(c->game);
 return status && status<=c->env->memory->size && 2<=c->env->memory->size-status &&
   HGNativeStartDisplayClock(c->env,(const vu16 *)(c->env->memory->data+status));
}
static void present(void *argument) {
 Capture *c=argument; c->frames++;
 if(!w2c_capture_HGCaptureMath(c->game,0)) c->line=__LINE__;
 if(c->phase+1==c->total) HGNativeStopDisplayClock(c->env); // The next call frees real task arenas.
 if(!c->line) c->line=w2c_capture_HGCaptureAfterFrame(c->game,c->phase++);
 if(c->phase<c->total) w2c_capture_HGCaptureMath(c->game,1);
}
static void diagnose(Capture *c,u32 kind,u32 mode) {
 u32 *w=witness(c);
 fprintf(stderr,"Capture fixture line %u: kind=%u mode=%u phase=%u stage=%u finishes=%u cpu=%u stack=%x cpsr=%x pixel=%u,%u expected=%x actual=%x\n",
  c->line,kind,mode,c->phase,w[0],w[2],w[3],w[4],w[5],w[6],w[7],w[8],w[9]);
}
// mode0 observer, 1 no observer, 2 CPSR.I masked, 3 real 1KiB SDK worker,
// 4 actual line80 software cancellation, 5 original reset before a pending request completes.
static int runCase(u32 kind,u32 mode,u32 unsupported) {
 w2c_capture game; struct w2c_env env={0};
 struct w2c_wasi__snapshot__preview1 wasi={NULL,{dup(0),dup(1),dup(2)}};
 wasm2c_capture_instantiate(&game,&env,&wasi); env.memory=wasi.memory=w2c_capture_memory(&game);
 env.irqTable=&game.w2c_T0; env.stackPointer=&game.w2c_0x5F_stack_pointer;
 env.rootStackHigh=*env.stackPointer; *env.stackPointer-=16;
 env.irqType0=wasm2c_capture_get_func_type(0,0); env.irqType1=wasm2c_capture_get_func_type(1,0,WASM_RT_I32);
 env.rtcType=wasm2c_capture_get_func_type(2,0,WASM_RT_I32,WASM_RT_I32);
 env.irqGuardType=wasm2c_capture_get_func_type(4,0,WASM_RT_I32,WASM_RT_I32,WASM_RT_I32,WASM_RT_I32);
 Capture c={&game,&env,0,0,0,kind>=8 ? 4u : 3u}; wasm_rt_trap_t trap=wasm_rt_impl_try();
 if(!trap) {
  w2c_capture_0x5Finitialize(&game); c.line=w2c_capture_InitializeOriginalGameHeap(&game);
  if(!c.line) { w2c_capture_HGIRQInit(&game); c.line=w2c_capture_HGCaptureSetup(&game,kind); }
  if(!c.line) {
   if(mode==4) w2c_capture_HGCaptureCancelAtLine80(&game);
   if(unsupported) w2c_capture_HGCaptureUnsupported(&game,unsupported-1);
   w2c_capture_HGCaptureMath(&game,1);
   if(!start(&c)) c.line=__LINE__;
   if(mode==5) { w2c_capture_HGCaptureReset(&game); c.line=__LINE__; }
   else if(!mode) {
    if(!HGNativeSetDisplayObserver(&env,present,&c)) c.line=__LINE__;
    for(u32 i=0;i<4000 && c.phase<c.total && !c.line;i++) HGNativeWaitNanoseconds(1000000);
    if(c.phase!=c.total && !c.line) c.line=__LINE__;
   } else {
    for(u32 frame=0;frame<3 && !c.line;frame++) {
     OSIntrMode previous=mode==2 ? OS_DisableInterrupts() : OS_GetCpsrIrq();
     if(mode==3 && !frame) c.line=w2c_capture_HGCaptureSmallWorker(&game);
     for(u32 i=0;i<2000 && w2c_capture_HGCaptureEpochs(&game)<frame+1 && !c.line;i++) HGNativeWaitNanoseconds(1000000);
     HGNativeStopDisplayClock(&env);
     if(w2c_capture_HGCaptureEpochs(&game)!=frame+1 || !w2c_capture_HGCaptureMath(&game,0)) c.line=__LINE__;
     if(mode==2) { if(OS_GetCpsrIrq()!=OS_INTRMODE_IRQ_DISABLE) c.line=__LINE__; OS_RestoreInterrupts(previous); }
     if(unsupported && !c.line) { w2c_capture_HGCaptureUnsupportedFinish(&game); c.line=__LINE__; }
     if(!c.line) c.line=mode==4 ? w2c_capture_HGCaptureCancelled(&game) : w2c_capture_HGCaptureAfterFrame(&game,frame);
     c.phase=frame+1; if(mode==4) break;
     if(frame<2 && !c.line) { w2c_capture_HGCaptureMath(&game,1); if(!start(&c)) c.line=__LINE__; }
    }
   }
  }
 }
 HGNativeStopDisplayClock(&env);
 if(trap) {
  if(mode==5 && trap==WASM_RT_TRAP_UNREACHABLE && env.resetRequested && env.resetParameter==1) {
   HGNativeFinishReset(&env);
   if(witness(&c)[2] || w2c_capture_HGCaptureUnwritten(&game) || !env.resetFinished || env.irqActive) c.line=__LINE__;
   for(u32 i=0;i<30;i++) HGNativeWaitNanoseconds(1000000);
   if(witness(&c)[2] || w2c_capture_HGCaptureUnwritten(&game)) c.line=__LINE__;
   // Keep the retired instance alive while another real owner completes capture.
   if(!c.line) c.line=runCase(2,1,0);
   if(witness(&c)[2] || w2c_capture_HGCaptureUnwritten(&game)) c.line=__LINE__;
  } else if(unsupported && trap==WASM_RT_TRAP_UNREACHABLE &&
    !w2c_capture_HGCaptureUnwritten(&game) && (w2c_capture_HGCaptureControl(&game)&0x80000000u) && !env.irqActive) {
   printf("PASS: unsupported capture %u traps transactionally before physical writes/E consume.\n",unsupported);
  } else {
   diagnose(&c,kind,mode); fprintf(stderr,"Capture native trap: %s\n",wasm_rt_strerror(trap)); _Exit(23);
  }
 } else if(unsupported || mode==5) c.line=__LINE__;
 if(c.line) diagnose(&c,kind,mode);
 if(__atomic_load_n(&env.pendingDMA,__ATOMIC_ACQUIRE) || env.irqActive) c.line=__LINE__;
 HGNativeDrainDMA(); HGNativeCloseThreadContext(&env); HGNativeReleaseIrqCallbacks(&env);
 wasm2c_capture_free(&game); for(u32 i=0;i<3;i++) if(wasi.fds[i]>=0) close(wasi.fds[i]);
 if(!c.line && !unsupported) printf("PASS: original capture kind=%u mode=%u, real Finish/tasks, exact pixels, CP/stack, cleanup.\n",kind,mode);
 if(!c.line && kind>=8) printf("PASS: original %s motion blur, three C-to-C epochs, frozen prior-C scanout, saturation, real task rearm and armed Stop/next-FINISH consume.\n",kind==8 ? "Field(3,15)" : "Legendary(5,13)");
 return c.line ? 1 : 0;
}
int main(void) {
 wasm_rt_init(); int result=0;
#ifdef NEGATIVE_CONTROL
 result=runCase(0,1,0);
#else
 // Existing compositor/scanline goldens run unchanged before separate fresh capture instances.
 { w2c_capture game; struct w2c_env env={0}; struct w2c_wasi__snapshot__preview1 wasi={NULL,{dup(0),dup(1),dup(2)}};
   wasm2c_capture_instantiate(&game,&env,&wasi); env.memory=wasi.memory=w2c_capture_memory(&game);
   w2c_capture_0x5Finitialize(&game); w2c_capture_CheckNative2D(&game); w2c_capture_CheckNativeRaster2D(&game);
   wasm2c_capture_free(&game); for(u32 i=0;i<3;i++) close(wasi.fds[i]); }
 for(u32 kind=0;kind<10 && !result;kind++) result=runCase(kind,0,0);
 for(u32 mode=1;mode<=5 && !result;mode++) result=runCase(0,mode,0);
 for(u32 source=1;source<=3 && !result;source++) result=runCase(0,1,source);
#endif
 wasm_rt_free(); return result;
}
'''


def function_body(source,name):
    start=source.index('uaddr '+name+'(')
    first=source.index('{',start); depth=1; end=first+1
    while depth:
        depth+=(source[end]=='{')-(source[end]=='}'); end+=1
    return start,end,source[start:end]


def main():
    destination=BUILD/'display-capture-check'; destination.mkdir(exist_ok=True)
    report=destination/'report.json'; report.unlink(missing_ok=True)
    original=json.loads((BUILD/'probe-link-command.json').read_text())
    cached=[Path(a) for a in original if a.endswith(('.o','.a'))]
    if not cached or any(not p.is_file() for p in cached): raise RuntimeError('Run check_abi32.py first')
    seal=cached+[BUILD/'hg32.wasm',BUILD/'audit/engine.wasm',BUILD/'audit/original-data/report.json',
                 BUILD/'audit/original-data/encounter_effect_data.o',BUILD/'audit/original-data/lib/asm/nnsys_data.o']
    cache_hashes={str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in seal}
    commands=[]
    def run(command,log,allow_failure=False):
        command=list(map(str,command)); commands.append(command)
        result=subprocess.run(command,cwd=ROOT,capture_output=True,text=True,timeout=240)
        (destination/log).write_text(result.stdout+result.stderr)
        if result.returncode and not allow_failure: raise RuntimeError(str(destination/log))
        return result

    # Reuse only the actual typed spans, rather than retaining the unrelated 46-entry intro function table.
    audit=json.loads((BUILD/'audit/original-data/report.json').read_text())
    entries=audit if isinstance(audit,list) else audit['units']
    groups={'asm/encounter_effect_data.s':{'gParticleBaseScale':12,'gParticleGraphicsModes':16,'gParticleCaptureBgTemplate':28,
            'gParticleGraphicsBanks':40,'gEncounterEffectState':12},
            'lib/asm/nnsys_data.s':{'gFrmTexVramRegions':120,'gFrmPlttVramManager':12}}
    assembly=[]; evidence={}; data_sources=[]; data_audits=[]; spans={}
    for filename,group in groups.items():
        entry=next(e for e in entries if e.get('file')==filename); data_source=ROOT/filename; data_object=Path(entry['object'])
        if entry['exitCode'] or entry['sha256']!=hashlib.sha256(data_source.read_bytes()).hexdigest() or entry['verifiedObjectSha256']!=cache_hashes[str(data_object)]:
            raise RuntimeError('Original data lacks a current successful byte/relocation audit: '+filename)
        parsed=wasm_object(data_object.read_bytes(),contents=True); data_sources.append(data_source); data_audits.append(entry); spans.update(group)
        for name,size in group.items():
            symbol=next(s for s in parsed['symbols'] if s.get('name')==name and 'segment' in s)
            segment=parsed['segments'][symbol['segment']]; offset=symbol['offset']
            if offset+size>len(segment['bytes']): raise RuntimeError('Truncated '+name)
            start=segment['offset']+offset
            if any(start<=r['offset']<start+size for r in parsed['relocations']): raise RuntimeError('Unexpected relocation within '+name)
            blob=segment['bytes'][offset:offset+size]
            if name in ('gEncounterEffectState','gFrmPlttVramManager') and any(blob): raise RuntimeError('Original state is not zero-initialized')
            assembly+=['.section .data.'+name+',"",@',' .p2align 2','.global '+name,'.type '+name+',@object',name+':',
                       '.byte '+','.join(map(str,blob)),'.size '+name+','+str(size)]
            evidence[name]=dict(file=filename,segment=symbol['segment'],offset=offset,size=size,sha256=hashlib.sha256(blob).hexdigest())
    subset=destination/'particle-data.s'; subset.write_text('\n'.join(assembly)+'\n')
    flags=['--target=wasm32-wasip1','-O2','-fwrapv','-fno-strict-aliasing','-fno-builtin-malloc','-fno-builtin-free',
           '-fwasm-exceptions','-fexceptions','-Xclang','-disable-red-zone','-fms-extensions','-Wno-microsoft','-Wno-unknown-pragmas']
    flags+=['-D'+n for n in ('HEARTGOLD_NATIVE','HEARTGOLD_ABI32','SDK_ARM9','SDK_CW','SDK_CODE_ARM','SDK_TS','_NITRO',
                             'PM_KEEP_ASSERTS','PLATFORM_INTRINSIC_FUNCTION_BIT_CLZ32=__builtin_clz')]
    for p in ('ios/build-original-host/include','include','files','src','lib/include','lib/include/cw','lib/include/nitro',
              'lib/include/nnsys','lib/include/nitro/os','include/library','lib/NitroSDK/include','lib/NitroSDK/include-pokediamond'):
        flags+=['-I',ROOT/p]
    sources=['src/system.c','src/sys_task.c','src/sys_task_api.c','src/field/encounter_effect_particles.c','src/field/fieldmap.c',
             'src/field/field_motion_blur.c','src/application/legendary_cinematic.c','src/systask_environment.c',
             'src/camera.c','lib/NitroSystem/src/gfd_frame_tex_vram_man.c','lib/NitroSystem/src/gfd_frame_pltt_vram_man.c',
             'lib/NitroSystem/src/gfd_linked_list_tex_vram_man.c','lib/NitroSystem/src/gfd_linked_list_pltt_vram_man.c',
             'lib/NitroSystem/src/gfd_linked_list_tex_vram_man_reset.c','lib/NitroSystem/src/gfd_linked_list_pltt_vram_man_reset.c',
             'lib/NitroSystem/src/gfd_linked_list_vram_man.c',
             'src/gf_gfx_planes.c','lib/NitroSystem/src/g3d_glb_state.c','lib/NitroSDK/src/gx/g2.c',
             'lib/NitroSDK/src/mi/mi_dma.c','lib/NitroSDK/src/mi/mi_memory.c',
             'ios/Original/NativeGraphicsMemory.c','ios/Original/NativeGeometry.c','ios/Original/abi32/NativeIRQClient.c',
             'ios/Original/Native2D.cpp','ios/Original/tests/render_check.c','ios/Original/abi32/display_capture_check.c',
             'ios/Original/abi32/arena_heap_check.c']
    host_flags,runtime=native_build_settings(destination)
    tracked=[ROOT/p for p in sources]+runtime+data_sources+[Path(__file__),
        ROOT/'ios/ThirdParty/libntr-g2/libraries/sim/src/sim_g2.cpp',ROOT/'lib/include/nitro/hw/native_graphics.h',
        ROOT/'lib/include/nitro/gx/native_geometry.h',ROOT/'lib/include/nitro/os/native_irq.h',
        ROOT/'ios/Original/NativeEvents.h',ROOT/'ios/Original/abi32/NativeImports.h',ROOT/'ios/Original/NativeDMA.h']
    source_hashes={str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in tracked}
    replacements={}; arena_provenance={}
    for name in sources:
        source=ROOT/name; target=destination/(source.stem+'.o'); cpp=source.suffix=='.cpp'
        if source.name=='arena_heap_check.c':
            actual=source.read_text()
            expanded,count=re.subn(re.escape('Heap_InitSystem(&template,1,128,0);'),'Heap_InitSystem(&template,1,HEAP_ID_MAX,0);',actual)
            if count!=1: raise RuntimeError('Expected exactly one original fixture heap-ID extent')
            source=destination/'shadow-arena_heap_check.c'; source.write_text(expanded)
            arena_provenance=dict(source=str(ROOT/name),sourceSha256=hashlib.sha256(actual.encode()).hexdigest(),
                                  derivedSha256=hashlib.sha256(expanded.encode()).hexdigest(),replacementCount=count)
        extra=['-iquote',ROOT/'ios/Original'] if cpp else []
        run([SDK/('bin/clang++' if cpp else 'bin/clang'),'-std=gnu++17' if cpp else '-std=gnu11',*flags,*extra,'-c',source,'-o',target],source.stem+'-compile.log')
        replacements[str(BUILD/(Path(name).stem+'.o'))]=target
    data_subset=destination/'particle-data.o'
    run([SDK/'bin/clang','--target=wasm32-wasip1','-c',subset,'-o',data_subset],'particle-data-compile.log')
    emitted=wasm_object(data_subset.read_bytes(),contents=True)
    for name,size in spans.items():
        symbol=next(s for s in emitted['symbols'] if s.get('name')==name)
        blob=emitted['segments'][symbol['segment']]['bytes'][symbol['offset']:symbol['offset']+size]
        if hashlib.sha256(blob).hexdigest()!=evidence[name]['sha256']: raise RuntimeError('Subset bytes changed: '+name)

    old=subprocess.run(['git','show',OLD_COMMIT+':ios/Original/NativeGraphicsMemory.c'],cwd=ROOT,capture_output=True,check=True).stdout
    current=(ROOT/'ios/Original/NativeGraphicsMemory.c').read_text()
    begin,end,_=function_body(current,'HGNativeRegisterAddress'); _,_,old_body=function_body(old.decode(),'HGNativeRegisterAddress')
    path=destination/'old-register-Memory.c'; path.write_text(current[:begin]+old_body+current[end:])
    old_register=destination/'old-register-Memory.o'
    run([SDK/'bin/clang','-std=gnu11',*flags,'-iquote',ROOT/'ios/Original','-c',path,'-o',old_register],'old-register-compile.log')
    client=(ROOT/'ios/Original/abi32/NativeIRQClient.c').read_text()
    disabled,count=re.subn(r'\bHGNativeFinishDisplayCapture\(\);','(void)0; /* QA: register admitted, producer not scheduled. */',client)
    if count!=1: raise RuntimeError('Expected exactly one actual hardware FINISH producer call')
    path=destination/'no-producer-client.c'; path.write_text(disabled); no_producer=destination/'no-producer-client.o'
    run([SDK/'bin/clang','-std=gnu11',*flags,'-c',path,'-o',no_producer],'no-producer-compile.log')
    exports=['InitializeOriginalGameHeap','HGIRQInit','CheckNative2D','CheckNativeRaster2D']
    exports+=['HGCapture'+n for n in ('Setup','AfterFrame','Witness','DisplayStatus','Control','Epochs','Math','SmallWorker',
                                    'CancelAtLine80','Cancelled','Reset','Unwritten','Unsupported','UnsupportedFinish')]
    modules={}
    for label in ('current','old-register','no-producer'):
        directory=destination/label; directory.mkdir(exist_ok=True); command=[]; used=set()
        for argument in original[:-2]:
            if argument.startswith('-Wl,--export=') or argument==str(BUILD/'battle_wave_check.o'): continue
            if argument in replacements: used.add(argument); argument=replacements[argument]
            if label=='old-register' and str(argument)==str(destination/'NativeGraphicsMemory.o'): argument=old_register
            if label=='no-producer' and str(argument)==str(destination/'NativeIRQClient.o'): argument=no_producer
            command.append(argument)
        command += [p for key,p in replacements.items() if key not in used]+[data_subset]
        module=directory/'capture.wasm'
        command+=['-Wl,--export='+n for n in exports]+['-Wl,--wrap=HGNativeFinishDisplayCapture','-Wl,--wrap=HGNativeRasterStep','-o',module]
        run(command,label+'-link.log'); run([WABT/'bin/wasm2c',module,'-n','capture','-o',directory/'capture.c'],label+'-generate.log')
        if re.search(r'w2c_\w*bitcast_invalid\w*',(directory/'capture.c').read_text()): raise RuntimeError('Invalid typed callback')
        modules[label]=dict(moduleSha256=hashlib.sha256(module.read_bytes()).hexdigest())
    host=['xcrun','clang',*host_flags,'-I',ROOT/'ios/Original','-O1','-g','-fsanitize=address,undefined','-fno-sanitize-recover=all']
    def compile_runtime(item):
        i,source=item; target=destination/f'host-{i}.o'; run([*host,'-c',source,'-o',target],f'host-{i}.log'); return target
    with ThreadPoolExecutor(max_workers=4) as pool: native=list(pool.map(compile_runtime,enumerate(runtime)))
    main_source=destination/'main.c'; main_source.write_text(HOST_MAIN)
    for label in modules:
        directory=destination/label; binary=directory/'capture-check'; options=[] if label=='current' else ['-DNEGATIVE_CONTROL']
        run([*host,*options,'-I',directory,directory/'capture.c',main_source,*native,'-Wl,-dead_strip','-o',binary],label+'-native-link.log')
        result=run([binary],label+'-runtime.log',True)
        modules[label].update(exitCode=result.returncode,stdout=result.stdout,stderr=result.stderr,nativeSha256=hashlib.sha256(binary.read_bytes()).hexdigest())
        if label=='current':
            if result.returncode or result.stdout.count('PASS: original capture')!=16 or result.stdout.count('PASS: unsupported capture')!=3 or result.stdout.count('motion blur, three C-to-C epochs')!=2:
                raise RuntimeError(str(destination/(label+'-runtime.log')))
        elif label=='old-register':
            if result.returncode==0 or 'Unsupported native graphics register: address=0x04000064 width=4' not in result.stderr or 'stage=2' not in result.stderr:
                raise RuntimeError('Old register negative did not fail the original capture arm')
        elif result.returncode==0 or not re.search(r'stage=3 .*pixel=0,0 expected=8008 actual=0',result.stderr):
            raise RuntimeError('Admitted-register/no-producer control did not fail the actual captured pixel')
    if any(hashlib.sha256(p.read_bytes()).hexdigest()!=cache_hashes[str(p)] for p in seal): raise RuntimeError('Shared production/probe/audit cache changed')
    if any(hashlib.sha256(p.read_bytes()).hexdigest()!=source_hashes[str(p)] for p in tracked): raise RuntimeError('Capture source snapshot changed')
    report.write_text(json.dumps(dict(passed=True,scope='original particle-mode prefix, real Field/Legendary motion-blur helper feedback and controlled compositor cases; not full Elite Four/Champion/Lugia presentation',
        sourceHashes=source_hashes,sharedCacheUnchanged=True,cacheHashes=cache_hashes,originalDataSpans=evidence,
        originalDataAudit=data_audits,shadowArenaFixture=arena_provenance,oldRegisterCommit=OLD_COMMIT,oldRegisterSourceSha256=hashlib.sha256(old).hexdigest(),modules=modules,commands=commands),indent=2)+'\n')
    print('PASS: original capture prefix and three-epoch Field/Legendary C-to-C feedback/Stop, exact pixels, real SYS FINISH/1KiB worker/CP/CPSR lifecycle, both runtime negatives.')


if __name__=='__main__': main()
