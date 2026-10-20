// SPDX-License-Identifier: GPL-3.0-or-later
#define _POSIX_C_SOURCE 200809L
#define _DARWIN_C_SOURCE 1
#include "hg32.h"
#include "NativeImports.h"
#include "NativeWASIImports.h"
#include <nitro/os/native_thread.h>
#include <nitro/os/native_reset.h>
#include "wasm-rt-impl.h"
#include "wasm-rt-exceptions.h"
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <pthread.h>
#include <sched.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include "../NativeEvents.h"
#include <nitro/os/native_irq.h>
#include <nitro/rtc/native_api.h>
#include <nitro/rtc/ARM9/api.h>
#include <nitro/spi/ARM9/mic.h>
#include <nitro/spi/native_mic.h>
#include <nitro/spi/native_power.h>
#include <nitro/spi/native_input.h>
#include <nitro/os/native_clock.h>
#include <nitro/hw/consts_shared.h>
#include <nitro/mi/dma.h>
#include <nitro/mi/native_dma.h>
#include <nitro/os/arena.h>
#include <nitro/fs/native_file.h>
#include "../NativeFile.h"
#include "../NativeBackup.h"
#include <nitro/card.h>
#include <nitro/card/native_card.h>
#include <nitro/snd/native_sound.h>

#define CHECK(v) do { if (!(v)) { fprintf(stderr,"ABI32 check line %d: %s\n",__LINE__,#v); abort(); } } while (0)
#include "../tests/cache_threads.h"
static void GeneratedCacheFence(void *context,unsigned operation) {
    w2c_hg32_HGCacheOperation(context,operation,0,0);
}
static u32 read32(wasm_rt_memory_t *memory, u32 offset) {
    CHECK(offset <= memory->size && 4 <= memory->size-offset);
    u32 value; memcpy(&value,memory->data+offset,4); return value;
}

static void CheckWasiHost(struct w2c_wasi__snapshot__preview1 *env,u32 offset) {
    CHECK(w2c_wasi__snapshot__preview1_clock_time_get(NULL,0,1,offset)==WASI_FAULT);
    CHECK(w2c_wasi__snapshot__preview1_fd_write(NULL,1,0,0,offset)==WASI_BADF);
    struct w2c_wasi__snapshot__preview1 unbound={NULL,{-1,-1,-1}};
    CHECK(w2c_wasi__snapshot__preview1_clock_time_get(&unbound,0,1,offset)==WASI_FAULT);
    wasm_rt_memory_t *memory=env->memory;
    memset(memory->data+offset,0xA5,8);
    CHECK(w2c_wasi__snapshot__preview1_environ_sizes_get(env,offset,(u32)memory->size-1)==WASI_FAULT && read32(memory,offset)==0xA5A5A5A5);
    CHECK(!w2c_wasi__snapshot__preview1_environ_sizes_get(env,offset,offset+4) && !read32(memory,offset) && !read32(memory,offset+4));
    CHECK(!w2c_wasi__snapshot__preview1_environ_get(env,0,(u32)memory->size));
    CHECK(w2c_wasi__snapshot__preview1_environ_get(env,UINT32_MAX,0)==WASI_FAULT);
    CHECK(w2c_wasi__snapshot__preview1_environ_sizes_get(&unbound,0,4)==WASI_FAULT);
    char path[]="ios/build-original-abi32/io-check-XXXXXX";
    int file=mkstemp(path), saved=env->fds[1]; CHECK(file>=0 && !unlink(path)); env->fds[1]=file;
    u32 vectors[]={offset+96,2,offset+112,2}; memcpy(memory->data+offset,vectors,sizeof(vectors));
    memcpy(memory->data+offset+96,"ab",2); memcpy(memory->data+offset+112,"cd",2);
    CHECK(!w2c_wasi__snapshot__preview1_fd_write(env,1,offset,2,offset+40) && read32(memory,offset+40)==4);
    CHECK(!w2c_wasi__snapshot__preview1_fd_seek(env,1,0,0,offset+32));
    char bytes[4]; CHECK(read(file,bytes,4)==4 && !memcmp(bytes,"abcd",4));
    CHECK(!w2c_wasi__snapshot__preview1_fd_seek(env,1,0,0,offset+32));
    memset(memory->data+offset+96,0,2); memset(memory->data+offset+112,0,2);
    CHECK(!w2c_wasi__snapshot__preview1_fd_read(env,1,offset,2,offset+40) && read32(memory,offset+40)==4);
    CHECK(!memcmp(memory->data+offset+96,"ab",2) && !memcmp(memory->data+offset+112,"cd",2));
    CHECK(!w2c_wasi__snapshot__preview1_fd_fdstat_get(env,1,offset+48) && memory->data[offset+48]==4);
    CHECK(!w2c_wasi__snapshot__preview1_clock_time_get(env,0,1,offset+80));
    u64 time; memcpy(&time,memory->data+offset+80,8); CHECK(time>0);
    CHECK(w2c_wasi__snapshot__preview1_clock_time_get(env,99,1,offset+80)==WASI_INVAL);
    CHECK(w2c_wasi__snapshot__preview1_clock_time_get(env,0,1,memory->size-3)==WASI_FAULT);
    CHECK(w2c_wasi__snapshot__preview1_fd_fdstat_get(env,1,memory->size-3)==WASI_FAULT);
    CHECK(w2c_wasi__snapshot__preview1_fd_write(env,1,memory->size-3,2,offset+40)==WASI_FAULT);
    CHECK(w2c_wasi__snapshot__preview1_fd_write(env,1,offset,UINT32_MAX,offset+40)==WASI_FAULT);
    vectors[2]=memory->size-1; memcpy(memory->data+offset,vectors,sizeof(vectors));
    CHECK(w2c_wasi__snapshot__preview1_fd_write(env,1,offset,2,offset+40)==WASI_FAULT && lseek(file,0,SEEK_CUR)==4);
    CHECK(w2c_wasi__snapshot__preview1_fd_read(env,1,offset,2,offset+40)==WASI_FAULT && lseek(file,0,SEEK_CUR)==4);
    CHECK(read32(memory,offset+40)==4);
    CHECK(w2c_wasi__snapshot__preview1_fd_write(env,3,offset,0,offset+40)==WASI_BADF);
    CHECK(w2c_wasi__snapshot__preview1_fd_seek(env,1,0,3,offset+32)==WASI_INVAL);
    CHECK(w2c_wasi__snapshot__preview1_fd_seek(env,1,UINT64_MAX,0,offset+32)==WASI_INVAL);
    CHECK(!w2c_wasi__snapshot__preview1_fd_close(env,1));
    CHECK(w2c_wasi__snapshot__preview1_fd_close(env,1)==WASI_BADF);
    env->fds[1]=saved;
}
static void CheckConsoleHost(w2c_hg32 *game,struct w2c_wasi__snapshot__preview1 *env) {
    char inPath[]="ios/build-original-abi32/console-in-XXXXXX",outPath[]="ios/build-original-abi32/console-out-XXXXXX";
    int input=mkstemp(inPath),output=mkstemp(outPath); CHECK(input>=0 && output>=0 && !unlink(inPath) && !unlink(outPath));
    const u8 data[]={'a','b','\r','\n','c','d','\n',0,255}; CHECK(write(input,data,sizeof(data))==sizeof(data) && !lseek(input,0,SEEK_SET));
    int savedIn=env->fds[0],savedOut=env->fds[1]; env->fds[0]=input; env->fds[1]=output;
    u32 buffer=w2c_hg32_HGConsoleReference(game,0),length=w2c_hg32_HGConsoleReference(game,1);
    CHECK(!w2c_hg32_HGConsoleAction(game,0,32) && read32(env->memory,length)==3 && !memcmp(env->memory->data+buffer,"ab\r",3));
    CHECK(!w2c_hg32_HGConsoleAction(game,0,32) && read32(env->memory,length)==1 && env->memory->data[buffer]=='\n');
    CHECK(!w2c_hg32_HGConsoleAction(game,0,32) && read32(env->memory,length)==3 && !memcmp(env->memory->data+buffer,"cd\n",3));
    CHECK(!w2c_hg32_HGConsoleAction(game,3,0) && w2c_hg32_HGConsoleAction(game,3,0)==255);
    CHECK(w2c_hg32_HGConsoleAction(game,3,0)==UINT32_MAX);
    CHECK(w2c_hg32_HGConsoleAction(game,0,32)==2 && !read32(env->memory,length));
    env->fds[0]=-1; CHECK(w2c_hg32_HGConsoleAction(game,0,1)==1 && !read32(env->memory,length));
    const u8 written[]={'x',0,'y',255,'x'}; memcpy(env->memory->data+buffer,written,4);
    CHECK(!w2c_hg32_HGConsoleAction(game,1,4) && read32(env->memory,length)==4);
    CHECK(!w2c_hg32_HGConsoleAction(game,2,0) && !lseek(output,0,SEEK_SET));
    u8 actual[5]; CHECK(read(output,actual,5)==5 && !memcmp(actual,written,5));
    env->fds[1]=-1; CHECK(w2c_hg32_HGConsoleAction(game,1,4)==1 && !read32(env->memory,length));
    CHECK(lseek(output,0,SEEK_CUR)==5);
    env->fds[0]=savedIn; env->fds[1]=savedOut; CHECK(!close(input) && !close(output));
}
enum { SWAP_COUNT=10000 };
static atomic_int swapReady,swapStart;
typedef struct { w2c_hg32 *game; u32 base; u32 *previous; } SwapWork;
static void *SwapWorker(void *arg) {
    SwapWork *work=arg; atomic_fetch_add(&swapReady,1);
    while (!atomic_load(&swapStart)) sched_yield();
    for (u32 i=0;i<SWAP_COUNT;i++) {
        work->previous[i]=w2c_hg32_HGSwapWordExchange(work->game,work->base+i+1);
        if (!(i&63)) sched_yield();
    }
    return NULL;
}
static void CheckAtomicSwap(w2c_hg32 *game,struct w2c_env *env) {
    CHECK(!w2c_hg32_CheckOriginalSwapWord(game));
    u32 previous[2][SWAP_COUNT]; u8 seen[2*SWAP_COUNT+1]={0}; pthread_t threads[2];
    SwapWork work[2]={{game,0,previous[0]},{game,SWAP_COUNT,previous[1]}};
    atomic_store(&swapReady,0); atomic_store(&swapStart,0);
    CHECK(!pthread_create(&threads[0],NULL,SwapWorker,&work[0]));
    CHECK(!pthread_create(&threads[1],NULL,SwapWorker,&work[1]));
    while (atomic_load(&swapReady)!=2) sched_yield(); atomic_store(&swapStart,1);
    CHECK(!pthread_join(threads[0],NULL) && !pthread_join(threads[1],NULL));
    for (u32 thread=0;thread<2;thread++) for (u32 i=0;i<SWAP_COUNT;i++) {
        u32 value=previous[thread][i]; CHECK(value<=2*SWAP_COUNT && !seen[value]); seen[value]=1;
    }
    u32 final=w2c_hg32_HGSwapWordExchange(game,0); CHECK(final<=2*SWAP_COUNT && !seen[final]); seen[final]=1;
    for (u32 i=0;i<=2*SWAP_COUNT;i++) CHECK(seen[i]);
    CHECK(w2c_hg32_HGSwapWordGuards(game));
    WASM_RT_UNWIND_TARGET *previousTarget=wasm_rt_get_unwind_target();
    wasm_rt_trap_t trap=wasm_rt_impl_try();
    if (!trap) { w2c_env_HGNativeAtomicSwapWord(env,1,(u32)env->memory->size-2); CHECK(0); }
    CHECK(trap==WASM_RT_TRAP_OOB);
    trap=wasm_rt_impl_try();
    if (!trap) { w2c_env_HGNativeAtomicSwapWord(env,1,1); CHECK(0); }
    CHECK(trap==WASM_RT_TRAP_UNALIGNED);
    trap=wasm_rt_impl_try();
    if (!trap) { w2c_env_HGNativeAtomicSwapWord(NULL,1,0); CHECK(0); }
    CHECK(trap==WASM_RT_TRAP_OOB);
    trap=wasm_rt_impl_try();
    if (!trap) { w2c_env_HGNativeMemoryBarrier(NULL); CHECK(0); }
    CHECK(trap==WASM_RT_TRAP_OOB);
    wasm_rt_set_unwind_target(previousTarget);
}
static atomic_int lockReady,lockStart;
static atomic_uint_least64_t claimedIDs;
static void *LockWorker(void *arg) {
    w2c_hg32 *game=arg; atomic_fetch_add(&lockReady,1);
    while (!atomic_load(&lockStart)) sched_yield();
    for (u32 i=0;i<10000;i++) {
        int32_t id=(int32_t)w2c_hg32_HGGetLockID(game); CHECK(id>=0x40 && id<0x70);
        uint64_t bit=1ULL<<(id-0x40); CHECK(!(atomic_fetch_or(&claimedIDs,bit)&bit));
        if (!(i&15)) sched_yield();
        CHECK(atomic_fetch_and(&claimedIDs,~bit)&bit);
        w2c_hg32_HGReleaseLockID(game,(u32)id);
    }
    return NULL;
}
static void CheckLockIDs(w2c_hg32 *first,w2c_hg32 *second) {
    CHECK(!w2c_hg32_CheckOriginalLockIDs(first));
    u32 a=w2c_hg32_HGGetLockID(first),b=w2c_hg32_HGGetLockID(second);
    CHECK(a==0x40 && b==0x41);
    w2c_hg32_HGReleaseLockID(first,a); w2c_hg32_HGReleaseLockID(second,b);
    pthread_t threads[2]; atomic_store(&lockReady,0); atomic_store(&lockStart,0); atomic_store(&claimedIDs,0);
    CHECK(!pthread_create(&threads[0],NULL,LockWorker,first));
    CHECK(!pthread_create(&threads[1],NULL,LockWorker,second));
    while (atomic_load(&lockReady)!=2) sched_yield(); atomic_store(&lockStart,1);
    CHECK(!pthread_join(threads[0],NULL) && !pthread_join(threads[1],NULL));
    CHECK(!atomic_load(&claimedIDs) && !w2c_hg32_CheckOriginalLockIDs(first));
}
static w2c_hg32 *cpuGame;
static unsigned cpuIrqCalls;
static void CpuIrqCallback(void) {
    CHECK(w2c_hg32_HGCPUAction(cpuGame,5,0)==OS_INTRMODE_DISABLE_IRQ);
    CHECK(w2c_hg32_HGCPUAction(cpuGame,6,0)==OS_PROCMODE_IRQ);
    cpuIrqCalls++;
}
static void *CpuWorker(void *arg) {
    CHECK(w2c_hg32_HGCPUIncrement(arg,10000)==10000);
    CHECK(w2c_hg32_HGCPUAction(arg,5,0)==0 && w2c_hg32_HGCPUAction(arg,6,0)==OS_PROCMODE_SYS);
    return NULL;
}
static void *CpuPostIrq(void *unused) {
    (void)unused;
    struct timespec delay={0,2000000}; nanosleep(&delay,NULL);
    HGNativePostIrq(OS_IE_VBLANK); return NULL;
}
static u64 CpuNow(void) {
    struct timespec value; CHECK(!clock_gettime(CLOCK_MONOTONIC,&value));
    return (u64)value.tv_sec*1000000000ULL+value.tv_nsec;
}
static void CheckCPU(w2c_hg32 *game,struct w2c_env *env) {
    CHECK(!w2c_hg32_CheckOriginalCPU(game));
    u64 svcStart=CpuNow(); w2c_hg32_HGCPUAction(game,8,335140);
    CHECK(CpuNow()-svcStart>=20000000);
    pthread_t threads[2];
    CHECK(!pthread_create(&threads[0],NULL,CpuWorker,game));
    CHECK(!pthread_create(&threads[1],NULL,CpuWorker,game));
    CHECK(!pthread_join(threads[0],NULL) && !pthread_join(threads[1],NULL));
    CHECK(w2c_hg32_HGCPUCounter(game,0)==20000 && w2c_hg32_HGCPUCounter(game,1));
    cpuGame=game; cpuIrqCalls=0; OS_InitIrqTable(); OS_SetIrqFunction(OS_IE_VBLANK,CpuIrqCallback);
    OS_SetIrqMask(OS_IE_VBLANK); OS_EnableIrq();
    CHECK(w2c_hg32_HGCPUAction(game,1,0)==0);
    HGNativePostIrq(OS_IE_VBLANK); CHECK(!cpuIrqCalls);
    CHECK(w2c_hg32_HGCPUAction(game,2,0)==OS_INTRMODE_DISABLE_IRQ && cpuIrqCalls==1);
    CHECK(!pthread_create(&threads[0],NULL,CpuPostIrq,NULL));
    u64 start=CpuNow(); w2c_hg32_HGCPUAction(game,7,1340559); u64 elapsed=CpuNow()-start;
    CHECK(!pthread_join(threads[0],NULL));
    CHECK(elapsed>=20000000 && cpuIrqCalls==2);
    CHECK(w2c_hg32_HGCPUAction(game,5,0)==0 && w2c_hg32_HGCPUAction(game,6,0)==OS_PROCMODE_SYS);
    CHECK(w2c_hg32_HGCPUAction(game,1,0)==0);
    CHECK(!pthread_create(&threads[0],NULL,CpuPostIrq,NULL));
    w2c_hg32_HGCPUAction(game,7,1340559); CHECK(!pthread_join(threads[0],NULL));
    CHECK(cpuIrqCalls==2 && (OS_GetRequestIrqMask()&OS_IE_VBLANK));
    CHECK(w2c_hg32_HGCPUAction(game,2,0)==OS_INTRMODE_DISABLE_IRQ && cpuIrqCalls==3);
    OS_DisableIrq(); OS_InitIrqTable(); cpuGame=NULL;
    WASM_RT_UNWIND_TARGET *saved=wasm_rt_get_unwind_target();
    wasm_rt_trap_t trap=wasm_rt_impl_try();
    if (!trap) { w2c_env_HGNativeRestoreInterrupts(env,1); CHECK(0); }
    CHECK(trap==WASM_RT_TRAP_OOB && w2c_hg32_HGCPUAction(game,5,0)==0);
    trap=wasm_rt_impl_try();
    if (!trap) { w2c_env_HGNativeRestoreInterrupts_IrqAndFiq(env,0x100); CHECK(0); }
    CHECK(trap==WASM_RT_TRAP_OOB && w2c_hg32_HGCPUAction(game,5,0)==0);
    trap=wasm_rt_impl_try();
    if (!trap) { w2c_env_HGNativeGetProcMode(NULL); CHECK(0); }
    CHECK(trap==WASM_RT_TRAP_OOB); wasm_rt_set_unwind_target(saved);
}
static void BindIRQ(w2c_hg32 *game,struct w2c_env *env) {
    env->irqTable=&game->w2c_T0; env->stackPointer=&game->w2c_0x5F_stack_pointer;
    if (!env->rootStackHigh) { env->rootStackHigh=*env->stackPointer; *env->stackPointer-=16; }
    env->irqType0=wasm2c_hg32_get_func_type(0,0);
    env->irqType1=wasm2c_hg32_get_func_type(1,0,WASM_RT_I32);
    env->rtcType=wasm2c_hg32_get_func_type(2,0,WASM_RT_I32,WASM_RT_I32);
    env->irqGuardType=wasm2c_hg32_get_func_type(4,0,WASM_RT_I32,WASM_RT_I32,WASM_RT_I32,WASM_RT_I32);
}
static void CheckPowerBoundaries(w2c_hg32 *game,struct w2c_env *env) {
    u32 function=w2c_hg32_HGPowerReference(game);
    const u32 invalid[][4]={{99,0,0,0},{HG_POWER_INIT,1,0,0},{HG_POWER_READ,5,0,function},
        {HG_POWER_WRITE,5,1,function},{HG_POWER_WRITE,0,65536,function},
        {HG_POWER_SET_LED,256,0,function},{HG_POWER_GET_LED,1,0,function},{HG_POWER_DRAIN,0,0,0}};
    WASM_RT_UNWIND_TARGET *saved=wasm_rt_get_unwind_target();
    for (u32 i=0;i<sizeof(invalid)/sizeof(invalid[0]);i++) {
        wasm_rt_trap_t trap=wasm_rt_impl_try();
        if (!trap) { w2c_env_HGNativePowerRequest(env,invalid[i][0],invalid[i][1],invalid[i][2],invalid[i][3]); CHECK(0); }
        CHECK(trap==WASM_RT_TRAP_OOB && !env->pendingPOWER);
    }
    wasm_rt_trap_t trap=wasm_rt_impl_try();
    if (!trap) { w2c_env_HGNativePowerRequest(env,HG_POWER_READ,0,0,w2c_hg32_HGIRQReference(game,0)); CHECK(0); }
    CHECK(trap==WASM_RT_TRAP_CALL_INDIRECT && !env->pendingPOWER);
    CHECK(w2c_hg32_HGPowerAction(game,0,0)==3 && !w2c_hg32_HGPowerAction(game,4,0) && env->pendingPOWER==1);
    trap=wasm_rt_impl_try();
    if (!trap) { HGNativeReleaseIrqCallbacks(env); CHECK(0); }
    CHECK(trap==WASM_RT_TRAP_OOB && env->pendingPOWER==1);
    w2c_hg32_HGIRQAction(game,HG_IRQ_ENABLE,0);
    CHECK(!env->pendingPOWER && w2c_hg32_HGPowerAction(game,0,0)==2);
    CHECK(w2c_hg32_HGPowerAction(game,2,0)==1 && !w2c_hg32_HGPowerAction(game,3,0));
    CHECK(!w2c_hg32_HGPowerAction(game,2,1));
    for (u32 i=0;i<7;i++) HGNativeSignalVBlank();
    CHECK(!w2c_hg32_HGPowerAction(game,2,1)); HGNativeSignalVBlank();
    CHECK(w2c_hg32_HGPowerAction(game,2,1)==1 && w2c_hg32_HGPowerAction(game,3,0)==1);
    wasm_rt_set_unwind_target(saved);
}
static void CheckInputs(w2c_hg32 *game,w2c_hg32 *other,struct w2c_env *env,struct w2c_env *otherEnv) {
    BindIRQ(game,env); u32 line=w2c_hg32_CheckNativeTouch(game,0);
    if (line) fprintf(stderr,"Original touch check line %u\n",line);
    CHECK(!line && !env->pendingINPUT);
    BindIRQ(other,otherEnv); line=w2c_hg32_CheckNativeTouch(other,0);
    if (line) fprintf(stderr,"Second original touch check line %u\n",line);
    CHECK(!line && !otherEnv->pendingINPUT);
    BindIRQ(game,env);
    CHECK(HGNativeSetInput(env,0x401,FALSE,80,100,TRUE));
    CHECK(w2c_hg32_HGInputAction(game,0,0)==0x401 && w2c_hg32_HGLegacyPad(game,0)==0x401);
    CHECK(!w2c_hg32_HGInputAction(other,0,0) && !w2c_hg32_HGLegacyPad(other,0));
    CHECK(HGNativeSetInput(env,0x401,TRUE,80,100,TRUE));
    CHECK(w2c_hg32_HGInputAction(game,1,0) && w2c_hg32_HGLegacyPad(game,1));
    CHECK(HGNativeSetInput(env,0x401,FALSE,80,100,TRUE) && !w2c_hg32_HGInputAction(game,2,0));
    u32 raw=w2c_hg32_HGInputReference(game,0),point=w2c_hg32_HGInputReference(game,1);
    CHECK(read32(env->memory,raw)==(1600u<<16|1280) && read32(env->memory,raw+4)==1);
    CHECK(read32(env->memory,point)==(100u<<16|80) && read32(env->memory,point+4)==1);
    w2c_hg32_HGIRQAction(game,HG_IRQ_DISABLE,0);
    CHECK(HGNativeSetInput(env,0,FALSE,42,23,TRUE) && !w2c_hg32_HGInputAction(game,7,0) && env->pendingINPUT==1);
    CHECK(HGNativeSetInput(env,0,FALSE,230,170,TRUE));
    w2c_hg32_HGCPUAction(game,1,0); CHECK(!w2c_hg32_HGInputAction(game,8,0));
    CHECK(read32(env->memory,point)==(23u<<16|42) && env->pendingINPUT==1);
    w2c_hg32_HGCPUAction(game,0,0); w2c_hg32_HGIRQAction(game,HG_IRQ_ENABLE,0); CHECK(!env->pendingINPUT);
    CHECK(!w2c_hg32_HGInputAction(game,3,2)); u32 phase=~0u;
    CHECK(HGNativeInputSamplingRate(env,&phase)==2 && phase==17);
    CHECK(HGNativeSetInput(env,0,FALSE,20,30,TRUE) && HGNativeSampleInput(env));
    CHECK(HGNativeSetInput(env,0,FALSE,40,50,TRUE) && HGNativeSampleInput(env));
    CHECK(w2c_hg32_HGInputAction(game,5,0)==2 && !w2c_hg32_HGInputAction(game,6,0));
    CHECK(read32(env->memory,point)==(30u<<16|20)); // Original filter selects the previous valid sample.
    CHECK(HGNativeSetInput(env,0,FALSE,0,0,FALSE) && HGNativeSampleInput(env));
    CHECK(!w2c_hg32_HGInputAction(game,6,0) && !read32(env->memory,point+4));
    CHECK(HGNativeSetInput(env,0,FALSE,55,60,TRUE));
    for (u32 i=0;i<16;i++) CHECK(HGNativeSampleInput(env));
    CHECK(w2c_hg32_HGInputAction(game,5,0)==1);
    CHECK(!HGNativeSetInput(env,0x1000,FALSE,55,60,TRUE) && !HGNativeSetInput(env,0,2,55,60,TRUE));
    CHECK(!HGNativeSetInput(env,0,FALSE,256,60,TRUE) && !HGNativeSetInput(env,0,FALSE,55,192,TRUE));
    CHECK(!HGNativeSetInput(env,0,FALSE,55,60,2));
    u32 function=w2c_hg32_HGInputReference(game,3);
    const u32 invalid[][4]={{99,0,0,0},{HG_INPUT_INIT,1,0,0},{HG_INPUT_PAD,0,1,0},
        {HG_INPUT_START,263,2,function},{HG_INPUT_START,0,0,function},{HG_INPUT_START,0,256,function},
        {HG_INPUT_SAMPLE,0,1,function},{HG_INPUT_DRAIN,0,0,0}};
    WASM_RT_UNWIND_TARGET *saved=wasm_rt_get_unwind_target();
    for (u32 i=0;i<sizeof(invalid)/sizeof(invalid[0]);i++) {
        wasm_rt_trap_t trap=wasm_rt_impl_try();
        if (!trap) { w2c_env_HGNativeInputRequest(env,invalid[i][0],invalid[i][1],invalid[i][2],invalid[i][3]); CHECK(0); }
        CHECK(trap==WASM_RT_TRAP_OOB && !env->pendingINPUT);
    }
    wasm_rt_trap_t trap=wasm_rt_impl_try();
    if (!trap) { w2c_env_HGNativeInputRequest(env,HG_INPUT_SAMPLE,0,0,w2c_hg32_HGIRQReference(game,0)); CHECK(0); }
    CHECK(trap==WASM_RT_TRAP_CALL_INDIRECT && !env->pendingINPUT);
    w2c_hg32_HGIRQAction(game,HG_IRQ_DISABLE,0);
    for (u32 i=0;i<256;i++) CHECK(HGNativeSampleInput(env));
    CHECK(env->pendingINPUT==256 && !HGNativeSampleInput(env));
    trap=wasm_rt_impl_try();
    if (!trap) { HGNativeReleaseIrqCallbacks(env); CHECK(0); }
    CHECK(trap==WASM_RT_TRAP_OOB && env->pendingINPUT==256);
    CHECK(w2c_hg32_HGInputAction(game,4,0) && HGNativeInputSamplingRate(env,NULL)==2);
    w2c_hg32_HGIRQAction(game,HG_IRQ_ENABLE,0); CHECK(!env->pendingINPUT);
    CHECK(w2c_hg32_HGInputAction(game,5,0)==5 && !w2c_hg32_HGInputAction(game,4,0));
    CHECK(!HGNativeInputSamplingRate(env,&phase) && !phase && !HGNativeSampleInput(env));
    CHECK(!HGNativeSetInput(NULL,0,FALSE,0,0,FALSE));
    wasm_rt_set_unwind_target(saved);
    HGNativeReleaseIrqCallbacks(env); HGNativeReleaseIrqCallbacks(otherEnv);
    puts("PASS: native pad/lid state, original touch calibration/filter/ring, sample snapshots, masked waits, queue bounds and independent inputs.");
}
static void CheckSounds(w2c_hg32 *game,w2c_hg32 *other,struct w2c_env *env,struct w2c_env *otherEnv) {
    u32 line=w2c_hg32_CheckNativeSoundQueue(game);
    if (line) fprintf(stderr,"Original sound queue check line %u\n",line);
    CHECK(!line); line=w2c_hg32_CheckNativeSoundQueue(other);
    if (line) fprintf(stderr,"Second sound queue check line %u\n",line);
    CHECK(!line);
    u32 shared=w2c_hg32_HGSoundReference(game,0),raw=w2c_hg32_HGSoundReference(game,4);
    w2c_hg32_HGSoundAction(game,1,0); CHECK(w2c_hg32_HGSoundAction(game,0,0)==1 && !w2c_hg32_HGSoundAction(other,0,0));
    s16 output[64]; CHECK(HGNativeSoundRender(env,output,8));
    const s16 expected[]={1000,1000,-1000,3000,3000,-3000,5000,5000};
    for (u32 i=0;i<8;i++) CHECK(output[i*2]==expected[i] && !output[i*2+1]);
    w2c_hg32_HGSoundAction(game,2,0); CHECK(HGNativeSoundRender(env,output,8));
    for (u32 i=0;i<8;i++) CHECK(!output[i*2]);
    CHECK(output[1]!=0);
    w2c_hg32_HGSoundAction(game,5,0); u32 info=w2c_hg32_HGSoundReference(game,3);
    CHECK(read32(env->memory,info+0x1180)==0x807F013F && read32(env->memory,info+0x11C0)==info);
    w2c_hg32_HGSoundAction(game,6,0xFFFE); CHECK((read32(env->memory,shared+0x26)&65535)==65534);
    w2c_hg32_HGSoundAction(game,7,0x1234); CHECK((read32(env->memory,shared+0x268)&65535)==0x1234);
    w2c_hg32_HGSoundAction(game,3,0); CHECK(!w2c_hg32_HGSoundAction(game,0,0) && HGNativeSoundRender(env,output,8));
    for (u32 i=0;i<16;i++) CHECK(!output[i]);
    w2c_hg32_HGSoundAction(game,1,0); w2c_hg32_HGSoundAction(game,4,0);
    CHECK(!w2c_hg32_HGSoundAction(game,0,0));
    WASM_RT_UNWIND_TARGET *saved=wasm_rt_get_unwind_target();
    const u32 invalid[][2]={{99,0},{HG_SOUND_INIT,raw},{HG_SOUND_SUBMIT,0},{HG_SOUND_SUBMIT,raw+1}};
    for (u32 i=0;i<sizeof(invalid)/sizeof(invalid[0]);i++) {
        wasm_rt_trap_t trap=wasm_rt_impl_try();
        if (!trap) { w2c_env_HGNativeSoundRequest(env,invalid[i][0],invalid[i][1]); CHECK(0); }
        CHECK(trap==WASM_RT_TRAP_OOB);
    }
    w2c_hg32_HGSoundRawCommand(game,0x17,127,0); memcpy(env->memory->data+raw,&raw,4);
    wasm_rt_trap_t trap=wasm_rt_impl_try();
    if (!trap) { w2c_env_HGNativeSoundRequest(env,HG_SOUND_SUBMIT,raw); CHECK(0); }
    CHECK(trap==WASM_RT_TRAP_OOB); u32 zero=0; memcpy(env->memory->data+raw,&zero,4);
    w2c_hg32_HGSoundRawCommand(game,0x22,0,0); trap=wasm_rt_impl_try();
    if (!trap) { w2c_env_HGNativeSoundRequest(env,HG_SOUND_SUBMIT,raw); CHECK(0); }
    CHECK(trap==WASM_RT_TRAP_OOB);
    u32 before=read32(env->memory,shared);
    w2c_hg32_HGSoundRawCommand(game,2,0,0); CHECK(w2c_env_HGNativeSoundRequest(env,HG_SOUND_SUBMIT,raw));
    trap=wasm_rt_impl_try();
    if (!trap) { w2c_env_HGNativeSoundRequest(env,HG_SOUND_PUMP,0); CHECK(0); }
    CHECK(trap==WASM_RT_TRAP_UNREACHABLE && read32(env->memory,shared)==before);
    wasm_rt_set_unwind_target(saved);
    HGNativeCloseSoundContext(env); HGNativeCloseSoundContext(otherEnv);
    puts("PASS: original sound pool/batches/tags, native PCM command rendering, shared status/variables, driver snapshots, invalidation and rejected/unsupported commands.");
}
static void CheckThreadBoundaries(struct w2c_env *env) {
    u32 previous=*env->stackPointer; void *context=env->threads;
    const u32 commands[]={UINT32_MAX,HG_THREAD_REGISTER,HG_THREAD_SWITCH,HG_THREAD_LOAD,HG_THREAD_DROP};
    WASM_RT_UNWIND_TARGET *saved=wasm_rt_get_unwind_target();
    for (u32 i=0;i<5;i++) {
        wasm_rt_trap_t trap=wasm_rt_impl_try();
        if (!trap) { w2c_env_HGNativeThreadRequest(env,commands[i],0,0,0); CHECK(0); }
        CHECK(trap==WASM_RT_TRAP_OOB && env->threads==context && *env->stackPointer==previous);
    }
    wasm_rt_trap_t trap=wasm_rt_impl_try();
    if (!trap) { HGNativeCheckThreadStack(env,0); CHECK(0); }
    CHECK(trap==WASM_RT_TRAP_EXHAUSTION && *env->stackPointer==previous);
    CHECK(HGNativeCheckThreadStack(env,previous)==previous);
    wasm_rt_set_unwind_target(saved);
}
static void RejectIrqInit(struct w2c_env *env,u32 configuration,wasm_rt_trap_t expected) {
    WASM_RT_UNWIND_TARGET *saved=wasm_rt_get_unwind_target();
    wasm_rt_trap_t trap=wasm_rt_impl_try();
    if (!trap) { w2c_env_HGNativeIrqInit(env,configuration); CHECK(0); }
    CHECK(trap==expected); wasm_rt_set_unwind_target(saved);
}
static void CheckIRQ(w2c_hg32 *game,struct w2c_env *env) {
    u32 legacyLine=w2c_hg32_CheckLegacyIRQState(game);
    if (legacyLine) fprintf(stderr,"Legacy IRQ check line %u\n",legacyLine);
    CHECK(!legacyLine);
    CHECK(!w2c_hg32_CheckOriginalIRQ(game));
    u32 stack=env->irqStackStart,end=stack+env->irqStackSize,dtcm=w2c_hg32_HGStartupData(game,0);
    u8 savedDTCM[16384]; memcpy(savedDTCM,env->memory->data+dtcm,sizeof(savedDTCM));
    u8 low=env->memory->data[stack],high=env->memory->data[end-1];
    u32 savedMask=w2c_hg32_HGCPUAction(game,1,0);
    memset(env->memory->data+stack-16,0,16); memset(env->memory->data+end,0,16);
    env->memory->data[stack]=0x11; env->memory->data[end-1]=0x22;
    w2c_hg32_HGIRQResetStack(game);
    for (u32 i=0;i<16;i++) CHECK(env->memory->data[stack-16+i]==0xA5 && env->memory->data[end+i]==0xA5);
    CHECK(env->memory->data[stack]==0x11 && env->memory->data[end-1]==0x22);
    CHECK(!memcmp(savedDTCM,env->memory->data+dtcm,sizeof(savedDTCM)));
    env->memory->data[stack]=low; env->memory->data[end-1]=high;
    w2c_hg32_HGCPUAction(game,2,savedMask);
    CHECK(w2c_hg32_HGLegacyIRQAction(game,3,0)==(OS_IE_VBLANK|OS_IE_HBLANK));
    u32 plain=w2c_hg32_HGIRQReference(game,0),arg=w2c_hg32_HGIRQReference(game,2),argument=w2c_hg32_HGIRQReference(game,4);
    HGNativePostIrq(OS_IE_VBLANK|OS_IE_HBLANK); CHECK(!w2c_hg32_HGIRQRead(game,0));
    CHECK(!w2c_hg32_HGLegacyIRQAction(game,1,0));
    CHECK(w2c_hg32_HGIRQRead(game,0)==2 && w2c_hg32_HGIRQRead(game,1)==1 && w2c_hg32_HGIRQRead(game,2)==2);
    wasm_rt_func_type_t savedType=env->irqType1; env->irqType1=NULL;
    w2c_hg32_HGIRQRegister(game,HG_IRQ_PLAIN,OS_IE_HBLANK,w2c_hg32_HGIRQReference(game,1),0);
    env->irqType1=savedType;
    CHECK(w2c_hg32_HGIRQAction(game,HG_IRQ_GET_CHECK,0)==(OS_IE_VBLANK|OS_IE_HBLANK));
    CHECK(w2c_hg32_HGCPUAction(game,1,0)==0);
    HGNativeSignalVBlank(); CHECK(w2c_hg32_HGIRQRead(game,0)==2);
    w2c_hg32_HGIRQDispatch(game); CHECK(w2c_hg32_HGIRQRead(game,0)==2);
    w2c_hg32_HGCPUAction(game,2,0); CHECK(w2c_hg32_HGIRQRead(game,0)==3);
    w2c_hg32_HGIRQDispatch(game); CHECK(w2c_hg32_HGIRQRead(game,0)==3);
    CHECK(w2c_hg32_HGIRQAction(game,HG_IRQ_GET_VBLANK,0)==1);
    w2c_hg32_HGIRQAction(game,HG_IRQ_DISABLE_MASK,OS_IE_HBLANK);
    HGNativePostIrq(OS_IE_HBLANK); CHECK(w2c_hg32_HGIRQRead(game,0)==3);
    CHECK(w2c_hg32_HGIRQAction(game,HG_IRQ_RESET_REQUEST,OS_IE_HBLANK)&OS_IE_HBLANK);
    w2c_hg32_HGIRQAction(game,HG_IRQ_ENABLE_MASK,OS_IE_HBLANK);
    pthread_t producer; CHECK(!pthread_create(&producer,NULL,CpuPostIrq,NULL));
    w2c_hg32_HGIRQAction(game,HG_IRQ_WAIT_CLEAR,OS_IE_VBLANK); CHECK(!pthread_join(producer,NULL));
    CHECK(w2c_hg32_HGIRQRead(game,0)==4);
    w2c_hg32_HGIRQRegister(game,HG_IRQ_DMA,0,arg,argument);
    CHECK(w2c_hg32_HGIRQGet(game,OS_IE_DMA0)==arg);
    HGNativePostIrq(OS_IE_DMA0);
    CHECK(w2c_hg32_HGIRQRead(game,0)==5 && w2c_hg32_HGIRQRead(game,5)==7);
    CHECK(!w2c_hg32_HGIRQGet(game,OS_IE_DMA0) && !(w2c_hg32_HGIRQAction(game,HG_IRQ_GET_MASK,0)&OS_IE_DMA0));
    w2c_hg32_HGIRQAction(game,HG_IRQ_ENABLE_MASK,OS_IE_DMA0);
    w2c_hg32_HGIRQRegister(game,HG_IRQ_DMA,0,arg,argument); HGNativePostIrq(OS_IE_DMA0);
    CHECK(w2c_hg32_HGIRQRead(game,0)==6 && (w2c_hg32_HGIRQAction(game,HG_IRQ_GET_MASK,0)&OS_IE_DMA0));
    w2c_hg32_HGIRQArm(game,1); HGNativePostIrq(OS_IE_TIMER0);
    CHECK(w2c_hg32_HGIRQRead(game,0)==7 && w2c_hg32_HGIRQGet(game,OS_IE_TIMER0)==arg);
    HGNativePostIrq(OS_IE_TIMER0);
    CHECK(w2c_hg32_HGIRQRead(game,0)==8 && !w2c_hg32_HGIRQGet(game,OS_IE_TIMER0));
    CHECK(w2c_hg32_HGIRQAction(game,HG_IRQ_GET_MASK,0)&OS_IE_TIMER0);
    w2c_hg32_HGIRQRegister(game,HG_IRQ_TIMER,1,plain,0); HGNativePostIrq(OS_IE_TIMER1);
    CHECK(w2c_hg32_HGIRQRead(game,0)==9 && w2c_hg32_HGIRQRead(game,9)==1 && !w2c_hg32_HGIRQRead(game,17));
    w2c_hg32_HGIRQAction(game,HG_IRQ_DISABLE_MASK,OS_IE_VBLANK);
    w2c_hg32_HGIRQRegister(game,HG_IRQ_PLAIN,OS_IE_VBLANK,0,0); CHECK(!w2c_hg32_HGIRQGet(game,OS_IE_VBLANK));
    w2c_hg32_HGIRQRegister(game,HG_IRQ_PLAIN,0,UINT32_MAX,0);
    u32 previous=w2c_hg32_HGIRQGet(game,OS_IE_HBLANK);
    WASM_RT_UNWIND_TARGET *saved=wasm_rt_get_unwind_target();
    wasm_rt_trap_t trap=wasm_rt_impl_try();
    if (!trap) { w2c_env_HGNativeIrqSet(env,HG_IRQ_PLAIN,OS_IE_HBLANK,w2c_hg32_HGIRQReference(game,3),0); CHECK(0); }
    CHECK(trap==WASM_RT_TRAP_CALL_INDIRECT && w2c_hg32_HGIRQGet(game,OS_IE_HBLANK)==previous);
    trap=wasm_rt_impl_try();
    if (!trap) { w2c_env_HGNativeIrqSet(env,HG_IRQ_PLAIN,OS_IE_HBLANK,UINT32_MAX,0); CHECK(0); }
    CHECK(trap==WASM_RT_TRAP_CALL_INDIRECT);
    trap=wasm_rt_impl_try();
    if (!trap) { w2c_env_HGNativeIrqSet(env,HG_IRQ_TIMER,4,plain,0); CHECK(0); }
    CHECK(trap==WASM_RT_TRAP_OOB);
    wasm_rt_set_unwind_target(saved);
    u32 configuration=w2c_hg32_HGIRQReference(game,7);
    const u32 original[]={env->irqDummy,env->irqGuard,env->irqStackStart,env->irqStackSize};
    RejectIrqInit(NULL,configuration,WASM_RT_TRAP_OOB);
    RejectIrqInit(env,0,WASM_RT_TRAP_OOB);
    RejectIrqInit(env,(u32)env->memory->size-4,WASM_RT_TRAP_OOB);
    const u32 fields[]={0,1,2,2,3},values[]={arg,UINT32_MAX,1,(u32)env->memory->size-4,UINT32_MAX};
    for (u32 i=0;i<5;i++) {
        u32 words[4]; memcpy(words,original,sizeof(words)); words[fields[i]]=values[i];
        memcpy(env->memory->data+configuration,words,sizeof(words));
        RejectIrqInit(env,configuration,i<2 ? WASM_RT_TRAP_CALL_INDIRECT : WASM_RT_TRAP_OOB);
        CHECK(env->irqDummy==original[0] && env->irqGuard==original[1] &&
              env->irqStackStart==original[2] && env->irqStackSize==original[3]);
    }
    CHECK(w2c_hg32_HGIRQMathState(game,1));
    u32 small=w2c_hg32_HGIRQReference(game,6),previousStack=*env->stackPointer;
    memset(env->memory->data+small,0x5A,1024);
    w2c_hg32_HGIRQRegister(game,HG_IRQ_PLAIN,OS_IE_HBLANK,w2c_hg32_HGIRQReference(game,5),0);
    wasm_rt_jmp_buf preservedTarget; memcpy(&preservedTarget,&g_wasm_rt_jmp_buf,sizeof(preservedTarget));
#if WASM_RT_STACK_DEPTH_COUNT
    u32 previousSavedDepth=wasm_rt_saved_call_stack_depth; wasm_rt_saved_call_stack_depth=123;
#endif
    *env->stackPointer=small+1024;
    HGNativePostIrq(OS_IE_HBLANK);
    CHECK(*env->stackPointer==small+1024 && !env->irqActive);
    *env->stackPointer=previousStack;
    CHECK(!memcmp(&preservedTarget,&g_wasm_rt_jmp_buf,sizeof(preservedTarget)));
#if WASM_RT_STACK_DEPTH_COUNT
    CHECK(wasm_rt_saved_call_stack_depth==123); wasm_rt_saved_call_stack_depth=previousSavedDepth;
#endif
    for (u32 i=0;i<1024;i++) CHECK(env->memory->data[small+i]==0x5A);
    u32 used=w2c_hg32_HGIRQRead(game,18);
    CHECK(used>=env->irqStackStart && used+8192<=env->irqStackStart+env->irqStackSize);
    CHECK(!w2c_hg32_HGIRQRead(game,19) && w2c_hg32_HGIRQMathState(game,0));
    env->irqActive=1; saved=wasm_rt_get_unwind_target(); trap=wasm_rt_impl_try();
    if (!trap) { HGNativeCheckedStackPointer(env,env->irqStackStart+HG_IRQ_RED_ZONE-1); CHECK(0); }
    CHECK(trap==WASM_RT_TRAP_EXHAUSTION && *env->stackPointer==previousStack);
    CHECK(HGNativeCheckedStackPointer(env,env->irqStackStart+HG_IRQ_RED_ZONE)==env->irqStackStart+HG_IRQ_RED_ZONE);
    CHECK(HGNativeCheckedStackPointer(env,env->irqStackStart+env->irqStackSize)==env->irqStackStart+env->irqStackSize);
    trap=wasm_rt_impl_try();
    if (!trap) { HGNativeCheckedStackPointer(env,env->irqStackStart+env->irqStackSize+1); CHECK(0); }
    CHECK(trap==WASM_RT_TRAP_EXHAUSTION); env->irqActive=0; wasm_rt_set_unwind_target(saved);
    w2c_hg32_HGIRQRegister(game,HG_IRQ_PLAIN,OS_IE_HBLANK,w2c_hg32_HGIRQReference(game,1),0);
    w2c_hg32_HGIRQRegister(game,HG_IRQ_PLAIN,OS_IE_VBLANK,plain,0);
    w2c_hg32_HGIRQAction(game,HG_IRQ_ENABLE_MASK,OS_IE_VBLANK);
    CHECK(!w2c_hg32_HGCPUAction(game,1,0));
    CHECK(!pthread_create(&producer,NULL,CpuPostIrq,NULL));
    w2c_hg32_HGIRQHalt(game); CHECK(!pthread_join(producer,NULL));
    CHECK(w2c_hg32_HGIRQRead(game,0)==9 && (OS_GetRequestIrqMask()&OS_IE_VBLANK));
    w2c_hg32_HGIRQHalt(game); CHECK(w2c_hg32_HGIRQRead(game,0)==9);
    w2c_hg32_HGCPUAction(game,2,0); CHECK(w2c_hg32_HGIRQRead(game,0)==10);
    CHECK(!pthread_create(&producer,NULL,CpuPostIrq,NULL));
    w2c_hg32_HGIRQHalt(game); CHECK(!pthread_join(producer,NULL));
    CHECK(w2c_hg32_HGIRQRead(game,0)==11);
    const u32 irqControls[]={HG_IRQ_HALT,HG_IRQ_DISPATCH};
    for (u32 i=0;i<2;i++) {
        saved=wasm_rt_get_unwind_target(); trap=wasm_rt_impl_try();
        if (!trap) { w2c_env_HGNativeIrqControl(env,irqControls[i],1); CHECK(0); }
        CHECK(trap==WASM_RT_TRAP_OOB); wasm_rt_set_unwind_target(saved);
        env->irqActive=1; saved=wasm_rt_get_unwind_target(); trap=wasm_rt_impl_try();
        if (!trap) { w2c_env_HGNativeIrqControl(env,irqControls[i],0); CHECK(0); }
        CHECK(trap==WASM_RT_TRAP_OOB); env->irqActive=0; wasm_rt_set_unwind_target(saved);
    }
    w2c_hg32_HGIRQAction(game,HG_IRQ_DISABLE,0); HGNativePostIrq(OS_IE_VBLANK);
    HGNativeReleaseIrqCallbacks(env); OS_EnableIrq(); HGNativePostIrq(OS_IE_VBLANK|OS_IE_HBLANK);
    CHECK(w2c_hg32_HGIRQRead(game,0)==11 && !(OS_GetIrqMask()&(OS_IE_VBLANK|OS_IE_HBLANK)));
    OS_DisableIrq(); OS_InitIrqTable();
}
static void *RtcWorker(void *game) {
    CHECK(w2c_hg32_HGRTCStart(game,HG_RTC_TIME_ASYNC,1,0)==RTC_RESULT_SUCCESS);
    return NULL;
}
static void CheckRTC(w2c_hg32 *game,struct w2c_env *env) {
    w2c_hg32_HGIRQInit(game); OS_EnableIrqMask(OS_IE_SPFIFO_RECV); OS_EnableIrq();
    time_t before=time(NULL); CHECK(w2c_hg32_HGRTCSync(game,HG_RTC_BOTH)==RTC_RESULT_SUCCESS); time_t after=time(NULL);
    BOOL matches=FALSE;
    for (int dst=0;dst<=1;dst++) {
        struct tm local={.tm_year=(int)w2c_hg32_HGRTCRead(game,0)+100,.tm_mon=(int)w2c_hg32_HGRTCRead(game,1)-1,
            .tm_mday=(int)w2c_hg32_HGRTCRead(game,2),.tm_hour=(int)w2c_hg32_HGRTCRead(game,4),
            .tm_min=(int)w2c_hg32_HGRTCRead(game,5),.tm_sec=(int)w2c_hg32_HGRTCRead(game,6),.tm_isdst=dst};
        time_t captured=mktime(&local);
        if (captured>=before && captured<=after && local.tm_wday==(int)w2c_hg32_HGRTCRead(game,3)) matches=TRUE;
    }
    CHECK(matches && w2c_hg32_HGRTCRead(game,7));
    OS_DisableIrq(); CHECK(w2c_hg32_HGRTCStart(game,HG_RTC_BOTH_ASYNC,1,0)==RTC_RESULT_SUCCESS);
    CHECK(env->pendingRTC==1 && !w2c_hg32_HGRTCRead(game,8));
    CHECK(w2c_hg32_HGRTCRead(game,0)==0xA5A5A5A5 && w2c_hg32_HGRTCRead(game,4)==0xA5A5A5A5);
    CHECK(w2c_hg32_HGRTCSync(game,HG_RTC_TIME)==RTC_RESULT_BUSY);
    CHECK(w2c_env_HGNativeRtcRequest(env,HG_RTC_TIME_ASYNC,0,w2c_hg32_HGRTCAddress(game,1),0,0)==RTC_RESULT_BUSY);
    CHECK(env->pendingRTC==1);
    WASM_RT_UNWIND_TARGET *saved=wasm_rt_get_unwind_target(); wasm_rt_trap_t trap=wasm_rt_impl_try();
    if (!trap) { HGNativeReleaseIrqCallbacks(env); CHECK(0); }
    CHECK(trap==WASM_RT_TRAP_OOB && env->irqTable && env->pendingRTC==1); wasm_rt_set_unwind_target(saved);
    OS_EnableIrq();
    CHECK(!env->pendingRTC && w2c_hg32_HGRTCRead(game,8)==1 && w2c_hg32_HGRTCRead(game,9)==RTC_RESULT_SUCCESS);
    CHECK(w2c_hg32_HGRTCRead(game,10)==0xF00DBAAD && !w2c_hg32_HGRTCRead(game,11) && w2c_hg32_HGRTCRead(game,7));
    OS_DisableIrqMask(OS_IE_SPFIFO_RECV);
    CHECK(w2c_hg32_HGRTCStart(game,HG_RTC_DATE_ASYNC,1,0)==RTC_RESULT_SUCCESS && !w2c_hg32_HGRTCRead(game,8));
    OS_EnableIrqMask(OS_IE_SPFIFO_RECV); CHECK(w2c_hg32_HGRTCRead(game,8)==1 && !env->pendingRTC);
    CHECK(w2c_hg32_HGRTCRead(game,4)==0xA5A5A5A5);
    u32 year=w2c_hg32_HGRTCRead(game,0);
    CHECK(w2c_hg32_HGRTCSync(game,HG_RTC_TIME)==RTC_RESULT_SUCCESS && w2c_hg32_HGRTCRead(game,4)<24);
    CHECK(w2c_hg32_HGRTCRead(game,0)==year);
    CHECK(w2c_hg32_HGRTCStart(game,HG_RTC_TIME_ASYNC,1,1)==RTC_RESULT_SUCCESS);
    CHECK(w2c_hg32_HGRTCRead(game,8)==2 && !env->pendingRTC && !w2c_hg32_HGRTCRead(game,11));
    CHECK(w2c_hg32_HGRTCRead(game,0)==0xA5A5A5A5);
    u32 hour=w2c_hg32_HGRTCRead(game,4);
    CHECK(w2c_hg32_HGRTCSync(game,HG_RTC_DATE)==RTC_RESULT_SUCCESS && w2c_hg32_HGRTCRead(game,0)<100);
    CHECK(w2c_hg32_HGRTCRead(game,4)==hour);
    CHECK(w2c_hg32_HGRTCStart(game,HG_RTC_BOTH_ASYNC,0,0)==RTC_RESULT_SUCCESS && !env->pendingRTC);
    CHECK(!w2c_hg32_HGRTCRead(game,8) && w2c_hg32_HGRTCRead(game,0)<100 && w2c_hg32_HGRTCRead(game,7));
    pthread_t worker; CHECK(!pthread_create(&worker,NULL,RtcWorker,game) && !pthread_join(worker,NULL));
    CHECK(env->pendingRTC==1 && !w2c_hg32_HGRTCRead(game,8));
    OS_WaitIrq(TRUE,OS_IE_SPFIFO_RECV);
    CHECK(!env->pendingRTC && w2c_hg32_HGRTCRead(game,8)==1 && !w2c_hg32_HGRTCRead(game,11));
    CHECK(w2c_env_HGNativeRtcRequest(env,UINT32_MAX,0,0,0,0)==RTC_RESULT_INVALID_COMMAND);
    CHECK(!w2c_hg32_CheckOriginalGameRTC(game) && !env->pendingRTC);
    OS_DisableIrqMask(OS_IE_SPFIFO_RECV); w2c_hg32_HGRTCGameFrames(game,10); CHECK(!env->pendingRTC);
    w2c_hg32_HGRTCGameFrames(game,1); CHECK(env->pendingRTC==1);
    OS_EnableIrqMask(OS_IE_SPFIFO_RECV); CHECK(!env->pendingRTC);
    saved=wasm_rt_get_unwind_target(); trap=wasm_rt_impl_try();
    if (!trap) { w2c_env_HGNativeRtcRequest(env,HG_RTC_DATE,(u32)env->memory->size-4,0,0,0); CHECK(0); }
    CHECK(trap==WASM_RT_TRAP_OOB);
    trap=wasm_rt_impl_try();
    if (!trap) { w2c_env_HGNativeRtcRequest(env,HG_RTC_DATE,1,0,0,0); CHECK(0); }
    CHECK(trap==WASM_RT_TRAP_UNALIGNED);
    trap=wasm_rt_impl_try();
    if (!trap) { w2c_env_HGNativeRtcRequest(env,HG_RTC_DATE_ASYNC,w2c_hg32_HGRTCAddress(game,0),0,w2c_hg32_HGRTCAddress(game,3),0); CHECK(0); }
    CHECK(trap==WASM_RT_TRAP_CALL_INDIRECT && !env->pendingRTC); wasm_rt_set_unwind_target(saved);
    OS_DisableIrq(); OS_InitIrqTable();
}
static void CheckMIC(w2c_hg32 *game,w2c_hg32 *other,struct w2c_env *env,struct w2c_env *otherEnv) {
    w2c_hg32_HGIRQInit(game); OS_EnableIrqMask(OS_IE_SPFIFO_RECV); OS_EnableIrq();
    w2c_hg32_HGMicAction(game,0,0,0); w2c_hg32_HGMicAction(other,0,0,0);
    CHECK(w2c_hg32_HGMicAction(game,7,0,0)==MIC_RESULT_ILLEGAL_PARAMETER);
    CHECK(!w2c_hg32_HGMicAction(game,5,0,0) && !w2c_hg32_HGMicAction(other,5,0,0));
    float samples[6000]={0}; u32 buffer=w2c_hg32_HGMicAddress(game,0);
    CHECK(w2c_hg32_HGMicAction(game,1,MIC_SAMPLING_TYPE_SIGNED_8BIT,0)==MIC_RESULT_SUCCESS);
    CHECK(w2c_hg32_HGMicAction(game,1,MIC_SAMPLING_TYPE_SIGNED_8BIT,0)==MIC_RESULT_ILLEGAL_STATUS);
    CHECK(HGNativeFeedMic(env,samples,1)==MIC_RESULT_SUCCESS && !w2c_hg32_HGMicAction(game,5,0,0));
    CHECK(env->memory->data[buffer]==0xA5);
    CHECK(HGNativeFeedMic(env,NULL,1)==MIC_RESULT_ILLEGAL_PARAMETER);
    CHECK(HGNativeFeedMic(env,samples,1)==MIC_RESULT_SUCCESS && w2c_hg32_HGMicAction(game,5,0,0)==buffer);
    CHECK(!env->memory->data[buffer] && !env->memory->data[buffer+1]);
    CHECK(HGNativeFeedMic(env,samples,1982)==MIC_RESULT_SUCCESS);
    CHECK(w2c_hg32_HGMicAction(game,5,0,0)==buffer+1982 && w2c_hg32_HGMicRead(game,1)==1);
    CHECK(w2c_hg32_HGMicRead(game,3)==0x1234ABCD && !w2c_hg32_HGMicRead(game,4));
    CHECK(HGNativeFeedMic(env,samples,1)==MIC_RESULT_ILLEGAL_STATUS);
    CHECK(!w2c_hg32_HGMicAction(other,5,0,0) && HGNativeFeedMic(otherEnv,samples,1)==MIC_RESULT_ILLEGAL_STATUS);
    for (u32 type=0;type<MIC_SAMPLING_TYPE_MAX;type++) {
        w2c_hg32_HGMicAction(game,0,0,0);
        CHECK(w2c_hg32_HGMicAction(game,1,type,0)==MIC_RESULT_SUCCESS);
        CHECK(HGNativeFeedMic(env,samples,1984)==MIC_RESULT_SUCCESS && !env->pendingMIC);
        CHECK(w2c_hg32_HGMicRead(game,1)==1 && w2c_hg32_HGMicRead(game,5));
        u8 first=type==MIC_SAMPLING_TYPE_8BIT ? 0x80 : 0;
        u8 second=type==MIC_SAMPLING_TYPE_8BIT || type==MIC_SAMPLING_TYPE_12BIT || type==MIC_SAMPLING_TYPE_12BIT_FILTER_OFF ? 0x80 : 0;
        for (u32 i=0;i<1984;i++) CHECK(env->memory->data[buffer+i]==((i&1) ? second : first));
    }
    w2c_hg32_HGMicAction(game,0,0,0); OS_DisableIrq();
    CHECK(w2c_hg32_HGMicAction(game,2,MIC_SAMPLING_TYPE_SIGNED_8BIT,1)==MIC_RESULT_SUCCESS && env->pendingMIC==1);
    CHECK(!w2c_hg32_HGMicRead(game,0));
    CHECK(w2c_hg32_HGMicAction(game,2,MIC_SAMPLING_TYPE_SIGNED_8BIT,1)==MIC_RESULT_BUSY);
    WASM_RT_UNWIND_TARGET *saved=wasm_rt_get_unwind_target(); wasm_rt_trap_t trap=wasm_rt_impl_try();
    if (!trap) { HGNativeReleaseIrqCallbacks(env); CHECK(0); }
    CHECK(trap==WASM_RT_TRAP_OOB && env->pendingMIC==1); wasm_rt_set_unwind_target(saved);
    OS_EnableIrq();
    CHECK(!env->pendingMIC && w2c_hg32_HGMicRead(game,0)==1 && w2c_hg32_HGMicRead(game,2)==MIC_RESULT_SUCCESS);
    CHECK(w2c_hg32_HGMicRead(game,3)==0x9876ABCD && !w2c_hg32_HGMicRead(game,4));
    OS_DisableIrqMask(OS_IE_SPFIFO_RECV);
    CHECK(HGNativeFeedMic(env,samples,6000)==MIC_RESULT_SUCCESS && env->pendingMIC==3);
    CHECK(!w2c_hg32_HGMicRead(game,1)); OS_EnableIrqMask(OS_IE_SPFIFO_RECV);
    CHECK(!env->pendingMIC && w2c_hg32_HGMicRead(game,1)==3 && w2c_hg32_HGMicAction(game,5,0,0)==buffer+46);
    CHECK(w2c_hg32_HGMicAction(game,3,0,0)==MIC_RESULT_SUCCESS);
    CHECK(HGNativeFeedMic(env,samples,1)==MIC_RESULT_ILLEGAL_STATUS);
    w2c_hg32_HGMicAction(game,0,0,0); w2c_hg32_HGMicAction(game,6,0,0);
    CHECK(w2c_hg32_HGMicAction(game,1,MIC_SAMPLING_TYPE_SIGNED_8BIT,1)==MIC_RESULT_SUCCESS);
    CHECK(HGNativeFeedMic(env,samples,6000)==MIC_RESULT_SUCCESS);
    CHECK(!env->pendingMIC && w2c_hg32_HGMicRead(game,1)==1 && w2c_hg32_HGMicRead(game,0)==1 && !w2c_hg32_HGMicRead(game,4));
    CHECK(HGNativeFeedMic(env,samples,1)==MIC_RESULT_ILLEGAL_STATUS && w2c_hg32_HGMicRead(game,5));
    trap=wasm_rt_impl_try();
    if (!trap) { w2c_env_HGNativeMicRequest(env,HG_MIC_STOP,0,0,0,0,w2c_hg32_HGMicAddress(game,2)); CHECK(0); }
    CHECK(trap==WASM_RT_TRAP_CALL_INDIRECT && !env->pendingMIC); wasm_rt_set_unwind_target(saved);
    const u32 badBuffers[]={buffer+1,(u32)env->memory->size-32};
    for (u32 i=0;i<2;i++) {
        trap=wasm_rt_impl_try();
        if (!trap) { w2c_env_HGNativeMicRequest(env,HG_MIC_START,badBuffers[i],64,16000,2,w2c_hg32_HGMicAddress(game,3)); CHECK(0); }
        CHECK(trap==WASM_RT_TRAP_OOB && !env->pendingMIC); wasm_rt_set_unwind_target(saved);
    }
    HGNativeCloseMicContext(env); HGNativeCloseMicContext(otherEnv);
    CHECK(!env->mic && !otherEnv->mic);
    OS_DisableIrq(); OS_InitIrqTable();
    puts("PASS: original MIC request locks/results, six PCM formats, halfword publication, loop/full IRQ callbacks, reentrant stop, bounds and per-instance lifetime.");
}
static void *TickWorker(void *game) {
    u64 previous=w2c_hg32_HGTickRead(game,0);
    for (int i=0;i<2000;i++) {
        u64 tick=w2c_hg32_HGTickRead(game,0); CHECK(tick>=previous); previous=tick;
    }
    return NULL;
}
static u64 TickUpperBound(u64 nanoseconds) {
    return (u64)(((unsigned __int128)nanoseconds*HW_SYSTEM_CLOCK)/64000000000ULL)+2;
}
static void CheckClock(w2c_hg32 *game,w2c_hg32 *other,struct w2c_env *env) {
    CHECK(!w2c_hg32_CheckOriginalClock(game) && !w2c_hg32_HGTickRead(other,3));
    u64 start=CpuNow(),before=w2c_hg32_HGTickRead(game,0);
    struct timespec delay={0,4000000}; nanosleep(&delay,NULL);
    u64 after=w2c_hg32_HGTickRead(game,0),elapsed=CpuNow()-start;
    CHECK(after>before && after-before<=TickUpperBound(elapsed));
    start=CpuNow(); u64 low=w2c_hg32_HGTickRead(game,1),next=w2c_hg32_HGTickRead(game,0); elapsed=CpuNow()-start;
    CHECK(low<65536 && (u16)((u16)next-(u16)low)<=TickUpperBound(elapsed));
    pthread_t threads[2]; CHECK(!pthread_create(&threads[0],NULL,TickWorker,game));
    CHECK(!pthread_create(&threads[1],NULL,TickWorker,game));
    CHECK(!pthread_join(threads[0],NULL) && !pthread_join(threads[1],NULL));
    u64 counter=w2c_hg32_HGTickRead(game,2); CHECK(counter<=w2c_hg32_HGTickRead(game,0)>>16);
    start=CpuNow(); w2c_hg32_HGTickSet(game,UINT64_MAX-100); nanosleep(&delay,NULL);
    u64 wrapped=w2c_hg32_HGTickRead(game,0); elapsed=CpuNow()-start;
    CHECK(wrapped<=TickUpperBound(elapsed));
    start=CpuNow(); w2c_hg32_HGTickSet(game,0); u64 reset=w2c_hg32_HGTickRead(game,0); elapsed=CpuNow()-start;
    CHECK(reset<=TickUpperBound(elapsed));
    w2c_hg32_HGTickInit(game); CHECK(w2c_hg32_HGTickRead(game,0)>=reset);
    WASM_RT_UNWIND_TARGET *saved=wasm_rt_get_unwind_target(); wasm_rt_trap_t trap=wasm_rt_impl_try();
    if (!trap) { w2c_env_HGNativeEntropy(env,(u32)env->memory->size-4); CHECK(0); }
    CHECK(trap==WASM_RT_TRAP_OOB);
    trap=wasm_rt_impl_try();
    if (!trap) { w2c_env_HGNativeEntropy(env,1); CHECK(0); }
    CHECK(trap==WASM_RT_TRAP_UNALIGNED);
    trap=wasm_rt_impl_try();
    if (!trap) { w2c_env_HGNativeTickControl(env,UINT32_MAX,0); CHECK(0); }
    CHECK(trap==WASM_RT_TRAP_OOB); wasm_rt_set_unwind_target(saved);
}
static void WaitAlarms(w2c_hg32 *game,u32 count) {
    u64 end=CpuNow()+2000000000ULL;
    while (w2c_hg32_HGAlarmRead(game,0)<count && CpuNow()<end) w2c_hg32_HGCPUAction(game,7,670280);
    CHECK(w2c_hg32_HGAlarmRead(game,0)==count && !w2c_hg32_HGAlarmRead(game,17));
}
static void CheckAlarm(w2c_hg32 *game,struct w2c_env *env) {
    // The clock check follows IRQ detachment; restore the table binding before registering timers.
    BindIRQ(game,env);
    CHECK(!w2c_hg32_CheckOriginalIRQ(game));
    CHECK(!w2c_hg32_CheckOriginalAlarmQueue(game));
    w2c_hg32_HGAlarmStart(game,0); w2c_hg32_HGIRQAction(game,HG_IRQ_ENABLE,0);
    WaitAlarms(game,2);
    CHECK(w2c_hg32_HGAlarmRead(game,1)==1 && w2c_hg32_HGAlarmRead(game,2)==2 && w2c_hg32_HGAlarmRead(game,18));
    w2c_hg32_HGIRQAction(game,HG_IRQ_DISABLE,0); w2c_hg32_HGAlarmStart(game,1);
    w2c_hg32_HGIRQAction(game,HG_IRQ_ENABLE,0); w2c_hg32_HGCPUAction(game,7,3351398);
    CHECK(!w2c_hg32_HGAlarmRead(game,0) && w2c_hg32_HGAlarmRead(game,18));
    w2c_hg32_HGIRQAction(game,HG_IRQ_DISABLE,0); w2c_hg32_HGAlarmStart(game,2);
    CHECK(!w2c_hg32_HGCPUAction(game,5,0));
    w2c_hg32_HGIRQAction(game,HG_IRQ_ENABLE,0); WaitAlarms(game,3);
    CHECK(w2c_hg32_HGAlarmRead(game,1)==4 && w2c_hg32_HGAlarmRead(game,3)==4 && w2c_hg32_HGAlarmRead(game,18));
    w2c_hg32_HGCPUAction(game,7,1340559); CHECK(w2c_hg32_HGAlarmRead(game,0)==3);
    w2c_hg32_HGIRQAction(game,HG_IRQ_DISABLE,0); w2c_hg32_HGAlarmStart(game,3);
    w2c_hg32_HGCPUAction(game,7,1340559); CHECK(!w2c_hg32_HGAlarmRead(game,0));
    w2c_hg32_HGIRQAction(game,HG_IRQ_ENABLE,0); WaitAlarms(game,1);
    CHECK(w2c_hg32_HGAlarmRead(game,1)==3);
    w2c_hg32_HGAlarmCancelAll(game);
    w2c_env_HGNativeAlarmTimer(env,65536); HGNativeReleaseIrqCallbacks(env);
    OS_EnableIrq(); struct timespec wait={0,160000000}; nanosleep(&wait,NULL);
    CHECK(!(OS_GetRequestIrqMask()&OS_IE_TIMER1)); OS_DisableIrq(); OS_InitIrqTable();
}
static void RejectDMA(struct w2c_env *env,u32 command,u32 channel,u32 source,u32 destination,u32 control,u32 table,u32 count) {
    u8 unchanged[32];
    BOOL hasDestination=env && destination && destination<=env->memory->size && sizeof(unchanged)<=env->memory->size-destination;
    if (hasDestination) memcpy(unchanged,env->memory->data+destination,sizeof(unchanged));
    WASM_RT_UNWIND_TARGET *saved=wasm_rt_get_unwind_target();
    wasm_rt_trap_t trap=wasm_rt_impl_try();
    if (!trap) { w2c_env_HGNativeDmaRequest(env,command,channel,source,destination,control,table,count); CHECK(0); }
    CHECK(trap==WASM_RT_TRAP_OOB && !OS_GetCpsrIrq());
    CHECK(!env || !__atomic_load_n(&env->pendingDMA,__ATOMIC_ACQUIRE));
    CHECK(!hasDestination || !memcmp(unchanged,env->memory->data+destination,sizeof(unchanged)));
    wasm_rt_set_unwind_target(saved);
}
static void CheckDMA(w2c_hg32 *game,struct w2c_env *env) {
    BindIRQ(game,env);
    u32 line=w2c_hg32_CheckOriginalBridgedDMA(game);
    if (line) fprintf(stderr,"Bridged DMA check line %u\n",line);
    CHECK(!line && !__atomic_load_n(&env->pendingDMA,__ATOMIC_ACQUIRE));
    u32 source=w2c_hg32_HGDMAReference(game,0),destination=w2c_hg32_HGDMAReference(game,1);
    u32 table=w2c_hg32_HGDMAReference(game,2),control=MI_CNT_COPY32(32);
    w2c_env_HGNativeDmaRequest(env,HG_DMA_TRANSFER_WAIT,0,source,destination,control,table,2);
    CHECK(!memcmp(env->memory->data+source,env->memory->data+destination,32) && !env->pendingDMA);
    memset(env->memory->data+destination,0xCD,32);
    RejectDMA(NULL,HG_DMA_TRANSFER_WAIT,0,source,destination,control,table,2);
    RejectDMA(env,HG_DMA_TRANSFER_WAIT,4,source,destination,control,table,2);
    RejectDMA(env,99,0,0,0,0,0,0);
    RejectDMA(env,HG_DMA_TRANSFER_WAIT,0,source,destination,control,0,2);
    RejectDMA(env,HG_DMA_TRANSFER_WAIT,0,source,destination,control,table+1,2);
    RejectDMA(env,HG_DMA_TRANSFER_WAIT,0,source,destination,control,table,0);
    RejectDMA(env,HG_DMA_TRANSFER_WAIT,0,source,destination,control,table,HG_NATIVE_DMA_MAX_SPANS+1);
    RejectDMA(env,HG_DMA_TRANSFER_WAIT,0,source,destination,control,(u32)env->memory->size-4,2);
    RejectDMA(env,HG_DMA_TRANSFER_WAIT,0,source+1,destination,control,table,2);
    RejectDMA(env,HG_DMA_TRANSFER_WAIT,0,source,destination+1,control,table,2);
    RejectDMA(env,HG_DMA_TRANSFER_WAIT,0,source,destination,control|MI_DMA_TIMING_H_BLANK,table,2);
    RejectDMA(env,HG_DMA_TRANSFER_WAIT,0,source,destination,control|MI_DMA_CONTINUOUS_ON,table,2);
    RejectDMA(env,HG_DMA_TRANSFER_WAIT,0,source,destination,control|(3u<<23),table,2);
    RejectDMA(env,HG_DMA_FILL_WAIT,0,42,destination,control,table,2);
    RejectDMA(env,HG_DMA_WAIT,0,source,0,0,0,0);
    RejectDMA(env,HG_DMA_STOP,0,0,0,1,0,0);
    u64 maximum=env->memory->max_pages; env->memory->max_pages++;
    RejectDMA(env,HG_DMA_TRANSFER_WAIT,0,source,destination,control,table,2);
    env->memory->max_pages=maximum;
    u32 record[6]; memcpy(record,env->memory->data+table,sizeof(record));
    u32 bad=(u32)env->memory->size-4; memcpy(env->memory->data+table+4,&bad,4);
    RejectDMA(env,HG_DMA_TRANSFER_WAIT,0,source,destination,control,table,2);
    memcpy(env->memory->data+table,record,sizeof(record)); bad=6;
    memcpy(env->memory->data+table+8,&bad,4);
    RejectDMA(env,HG_DMA_TRANSFER_WAIT,0,source,destination,control,table,2);
    memcpy(env->memory->data+table,record,sizeof(record));
    // Isolate the retained-reference guard without depending on transfer/thread scheduling speed.
    __atomic_store_n(&env->pendingDMA,1,__ATOMIC_RELEASE);
    WASM_RT_UNWIND_TARGET *saved=wasm_rt_get_unwind_target();
    wasm_rt_trap_t trap=wasm_rt_impl_try();
    if (!trap) { HGNativeReleaseIrqCallbacks(env); CHECK(0); }
    CHECK(trap==WASM_RT_TRAP_OOB && env->irqTable==&game->w2c_T0);
    __atomic_store_n(&env->pendingDMA,0,__ATOMIC_RELEASE); wasm_rt_set_unwind_target(saved);
    w2c_env_HGNativeDmaRequest(env,HG_DMA_TRANSFER,0,source,destination,control|MI_DMA_IF_ENABLE,table,2);
    w2c_env_HGNativeDmaRequest(env,HG_DMA_STOP,0,0,0,0,0,0);
    CHECK(!__atomic_load_n(&env->pendingDMA,__ATOMIC_ACQUIRE));
    HGNativeReleaseIrqCallbacks(env);
    OS_EnableIrq(); CHECK(!(OS_GetRequestIrqMask()&(OS_IE_DMA0|OS_IE_DMA1|OS_IE_DMA2|OS_IE_DMA3)));
    CHECK(w2c_hg32_HGDMAReference(game,3)==2);
    OS_DisableIrq(); OS_InitIrqTable();
}
static u8 *FileCheckRead(const char *path,u32 *size) {
    FILE *file=fopen(path,"rb"); CHECK(file && !fseek(file,0,SEEK_END));
    long length=ftell(file); CHECK(length>16 && length<1048576 && !fseek(file,0,SEEK_SET));
    u8 *bytes=malloc((size_t)length); CHECK(bytes && fread(bytes,1,(size_t)length,file)==(size_t)length);
    CHECK(!fclose(file)); *size=(u32)length; return bytes;
}
static u32 FileCheckLE(const u8 *bytes,u32 size) {
    u32 result=0; for (u32 i=0;i<size;i++) result|=(u32)bytes[i]<<(i*8); return result;
}
static void FileCheckWrite(const char *path,const void *bytes,u32 size) {
    FILE *file=fopen(path,"wb"); CHECK(file && fwrite(bytes,1,size,file)==size && !fclose(file));
}
static void RejectFile(struct w2c_env *env,u32 command,u32 file,u32 first,u32 second,u32 third) {
    WASM_RT_UNWIND_TARGET *saved=wasm_rt_get_unwind_target();
    wasm_rt_trap_t trap=wasm_rt_impl_try();
    if (!trap) { (void)w2c_env_HGNativeFileRequest(env,command,file,first,second,third); CHECK(0); }
    CHECK(trap==WASM_RT_TRAP_OOB && !OS_GetCpsrIrq()); wasm_rt_set_unwind_target(saved);
}
static void CheckFiles(w2c_hg32 *game,w2c_hg32 *other,struct w2c_env *env,struct w2c_env *otherEnv) {
    char root[]="/tmp/heartgold-files-XXXXXX"; CHECK(mkdtemp(root));
    char path[512]; const char *directories[]={"sub","a","a/0","a/0/0","a/empty"};
    for (u32 i=0;i<5;i++) { snprintf(path,sizeof(path),"%s/%s",root,directories[i]); CHECK(!mkdir(path,0700)); }
    u8 pattern[64]; for (u32 i=0;i<64;i++) pattern[i]=(u8)(i*3+7);
    snprintf(path,sizeof(path),"%s/sub/binary.dat",root); FileCheckWrite(path,pattern,64);
    u32 archiveSize; u8 *archive=FileCheckRead("ios/OriginalContent/files/a/0/0/0",&archiveSize);
    CHECK(!memcmp(archive,"NARC",4));
    snprintf(path,sizeof(path),"%s/a/0/0/0",root); FileCheckWrite(path,archive,archiveSize);
    snprintf(path,sizeof(path),"%s/escape.dat",root); CHECK(!symlink("/etc/hosts",path));
    CHECK(HGNativeFSSetRoot(root));
    HGNativeDirEntry *rootEntries=NULL; u32 rootCount=0;
    CHECK(HGNativeFSReadDirectory("rom:/",&rootEntries,&rootCount) && rootCount==2);
    CHECK(rootEntries[0].directory && !strcmp(rootEntries[0].name,"a"));
    CHECK(rootEntries[1].directory && !strcmp(rootEntries[1].name,"sub"));
    CHECK(!HGNativeFSSetRoot(root)); HGNativeFSFreeDirectory(rootEntries);
    CHECK(HGNativeFSSetRoot(root));
    char longName[129]; memset(longName,'x',128); longName[128]=0;
    char longPath[512]; snprintf(longPath,sizeof(longPath),"%s/sub/%s",root,longName);
    FileCheckWrite(longPath,pattern,64);
    HGNativeDirEntry *rejected=NULL; u32 rejectedCount=42;
    CHECK(!HGNativeFSReadDirectory("sub",&rejected,&rejectedCount) && !rejected && rejectedCount==42);
    CHECK(!unlink(longPath) && HGNativeFSSetRoot(root));
    u32 line=w2c_hg32_CheckOriginalBridgedFiles(game);
    if (line) fprintf(stderr,"File bridge check line %u\n",line);
    CHECK(!line && !w2c_hg32_CheckOriginalBridgedFiles(other));
    u32 btaf=FileCheckLE(archive+12,2); CHECK(btaf+12<=archiveSize);
    u32 count=FileCheckLE(archive+btaf+8,2),btnf=btaf+FileCheckLE(archive+btaf+4,4);
    CHECK(count && btaf+12+count*8<=archiveSize && btnf+8<=archiveSize);
    u32 gmif=btnf+FileCheckLE(archive+btnf+4,4); CHECK(gmif+8<=archiveSize);
    CHECK(w2c_hg32_HGFileReadSample(game,0,0)==count);
    const u32 members[]={0,count/2,count-1};
    u32 output=w2c_hg32_HGFileReference(game,1);
    for (u32 i=0;i<3;i++) {
        u32 first=FileCheckLE(archive+btaf+12+members[i]*8,4),last=FileCheckLE(archive+btaf+16+members[i]*8,4);
        CHECK(first<=last && gmif+8+last<=archiveSize);
        CHECK(w2c_hg32_HGFileReadSample(game,members[i],1)==last-first);
        CHECK(!memcmp(env->memory->data+output,archive+gmif+8+first,last-first));
    }
    free(archive);
    u32 file=w2c_hg32_HGFileReference(game,0),text=w2c_hg32_HGFileReference(game,2);
    RejectFile(NULL,HG_FILE_INIT,file,0,0,0);
    RejectFile(env,99,file,0,0,0);
    RejectFile(env,HG_FILE_INIT,0,0,0,0);
    RejectFile(env,HG_FILE_INIT,file+1,0,0,0);
    RejectFile(env,HG_FILE_INIT,(u32)env->memory->size-4,0,0,0);
    RejectFile(env,HG_FILE_RESOLVE,0,0,0,0);
    u8 tail[4]; memcpy(tail,env->memory->data+env->memory->size-4,4);
    memset(env->memory->data+env->memory->size-4,'X',4);
    RejectFile(env,HG_FILE_RESOLVE,0,(u32)env->memory->size-4,0,0);
    memcpy(env->memory->data+env->memory->size-4,tail,4);
    RejectFile(env,HG_FILE_READ,file,(u32)env->memory->size-4,64,0);
    RejectFile(env,HG_FILE_READ,file,output,UINT32_MAX,0);
    RejectFile(env,HG_FILE_SEEK,file,0,99,0);
    RejectFile(env,HG_FILE_READ_DIR,file,output+1,0,0);
    RejectFile(env,HG_FILE_READ_DIR,file,(u32)env->memory->size-4,0,0);
    RejectFile(env,HG_FILE_READ_DIR,file,output,2,0);
    RejectFile(env,HG_FILE_GET_PATH,file,(u32)env->memory->size-4,64,0);
    RejectFile(env,HG_FILE_SEEK_DIR,file,0,UINT32_MAX,0);
    RejectFile(env,HG_FILE_RESOLVE_AT,0,text,0,2);
    CHECK(w2c_hg32_HGFileOpenHeld(game));
    RejectFile(env,HG_FILE_INIT,file,0,0,0);
    CHECK(!HGNativeFSSetRoot(root));
    CHECK(w2c_hg32_HGFileOpenHeld(other));
    CHECK(w2c_env_HGNativeFileRequest(env,HG_FILE_READ,file,output,8,0)==8);
    CHECK(read32(env->memory,file+44)==8);
    u32 otherFile=w2c_hg32_HGFileReference(other,0);
    CHECK(read32(otherEnv->memory,otherFile+44)==0);
    u32 directory=w2c_hg32_HGFileReference(game,3),relative=w2c_hg32_HGFileReference(game,4);
    CHECK(w2c_env_HGNativeFileRequest(env,HG_FILE_CHANGE_DIR,0,directory,0,0));
    CHECK(w2c_env_HGNativeFileRequest(env,HG_FILE_RESOLVE,0,relative,0,0));
    CHECK(!w2c_env_HGNativeFileRequest(otherEnv,HG_FILE_RESOLVE,0,w2c_hg32_HGFileReference(other,4),0,0));
    HGNativeCloseFileContext(otherEnv); CHECK(!HGNativeFSSetRoot(root));
    HGNativeReleaseIrqCallbacks(env); CHECK(HGNativeFSSetRoot("ios/OriginalContent/files"));
    CHECK(!w2c_env_HGNativeFileRequest(env,HG_FILE_WAIT,file,0,0,0));
    CHECK(text && !unlink(path));
    snprintf(path,sizeof(path),"%s/sub/binary.dat",root); CHECK(!unlink(path));
    snprintf(path,sizeof(path),"%s/a/0/0/0",root); CHECK(!unlink(path));
    for (int i=4;i>=0;i--) { snprintf(path,sizeof(path),"%s/%s",root,directories[i]); CHECK(!rmdir(path)); }
    CHECK(!rmdir(root));
}
typedef struct ResetInstance { w2c_hg32 game; struct w2c_env native; struct w2c_wasi__snapshot__preview1 wasi; } ResetInstance;
static ResetInstance *ResetCreate(u32 parameter) {
    ResetInstance *instance=calloc(1,sizeof(*instance)); CHECK(instance);
    instance->native.bootParameter=parameter;
    for (u32 i=0;i<3;i++) { instance->wasi.fds[i]=dup((int)i); CHECK(instance->wasi.fds[i]>=0); }
    wasm2c_hg32_instantiate(&instance->game,&instance->native,&instance->wasi);
    instance->wasi.memory=w2c_hg32_memory(&instance->game); instance->native.memory=instance->wasi.memory;
    BindIRQ(&instance->game,&instance->native); w2c_hg32_0x5Finitialize(&instance->game);
    return instance;
}
static void ResetRelease(ResetInstance *instance) {
    wasm2c_hg32_free(&instance->game);
    for (u32 i=0;i<3;i++) if (instance->wasi.fds[i]>=0) close(instance->wasi.fds[i]);
    free(instance);
}
static void WaitCard(ResetInstance *instance) {
    struct timespec start,now; CHECK(!clock_gettime(CLOCK_MONOTONIC,&start));
    while (!w2c_hg32_HGCardAction(&instance->game,4,0)) {
        CHECK(!clock_gettime(CLOCK_MONOTONIC,&now) && now.tv_sec-start.tv_sec<10);
    }
}
static int CheckNativeCards(void) {
    CHECK(HGNativeBackupSetPath(NULL) && !HGNativeBackupTryLock(0x7F));
    ResetInstance *missing=ResetCreate(0); CHECK(w2c_hg32_HGCardSetup(&missing->game));
    CHECK(!w2c_hg32_HGCardAction(&missing->game,0,CARD_BACKUP_TYPE_FLASH_4MBITS_EX));
    CHECK((s32)w2c_hg32_HGDwcInitialize(&missing->game)==-10001);
    HGNativeReleaseIrqCallbacks(&missing->native); ResetRelease(missing);
    char folder[]="ios/build-original-abi32/card-storage-XXXXXX"; CHECK(mkdtemp(folder));
    char path[512],sidecar[528]; snprintf(path,sizeof(path),"%s/game.sav",folder);
    CHECK(HGNativeBackupSetPath(path)); u64 console=HGNativeBackupConsoleId(); CHECK(console);
    ResetInstance *instance=ResetCreate(0); struct w2c_env *env=&instance->native;
    u32 dwcLine=w2c_hg32_CheckNativeDwcSettings(&instance->game);
    if (dwcLine) fprintf(stderr,"Original DWC settings check line %u\n",dwcLine);
    CHECK(!dwcLine);
    CHECK(w2c_hg32_HGCardSetup(&instance->game));
    CHECK(w2c_hg32_HGCardAction(&instance->game,0,CARD_BACKUP_TYPE_FLASH_4MBITS_EX));
    CHECK(w2c_hg32_HGCardAction(&instance->game,9,0)==0x10000 && !HGNativeBackupSetPath(path));
    u32 input=w2c_hg32_HGCardBuffer(&instance->game,0),output=w2c_hg32_HGCardBuffer(&instance->game,1);
    CHECK(w2c_hg32_HGCardAction(&instance->game,1,0x1000) && w2c_hg32_HGCardAction(&instance->game,2,0x1000));
    CHECK(!memcmp(env->memory->data+input,env->memory->data+output,1024));
    CHECK(!w2c_hg32_HGCardAction(&instance->game,10,0x1000));
    env->memory->data[input]^=0xFF;
    CHECK(w2c_hg32_HGCardAction(&instance->game,10,0x1000)==CARD_RESULT_FAILURE);
    env->memory->data[input]^=0xFF;
    CHECK(w2c_hg32_HGCardAction(&instance->game,1,0x7FC00) && w2c_hg32_HGCardAction(&instance->game,2,0x7FC00));
    CHECK(!memcmp(env->memory->data+input,env->memory->data+output,1024));
    memset(env->memory->data+output,0xA5,1024);
    CHECK(w2c_hg32_HGCardAction(&instance->game,6,0)==CARD_RESULT_INVALID_PARAM);
    for (u32 i=0;i<1024;i++) CHECK(env->memory->data[output+i]==0xA5);
    CHECK(w2c_hg32_HGCardAction(&instance->game,0,CARD_BACKUP_TYPE_FLASH_4MBITS));
    CHECK(w2c_hg32_HGCardAction(&instance->game,3,0x1000)); WaitCard(instance);
    CHECK(w2c_hg32_HGCardMode(&instance->game,0)==1 && w2c_hg32_HGCardMode(&instance->game,1)==OS_PROCMODE_SYS);
    CHECK(w2c_hg32_HGCardMode(&instance->game,2) && !w2c_hg32_HGCardMode(&instance->game,3));
    CHECK(!w2c_hg32_HGCardAction(&instance->game,5,0) && !env->pendingCARD);
    CHECK(!memcmp(env->memory->data+input,env->memory->data+output,1024));
    u32 command=w2c_hg32_HGCardReference(&instance->game,0),function=w2c_hg32_HGCardReference(&instance->game,2);
    WASM_RT_UNWIND_TARGET *boundaryTarget=wasm_rt_get_unwind_target();
    const u32 invalid[][6]={{CARD_REQ_INIT,0,0,1,function,0},{CARD_REQ_INIT,command+1,0,1,function,0},
        {CARD_REQ_INIT,command,4,1,function,0},{CARD_REQ_INIT,command,0,0,function,0},
        {HG_CARD_CANCEL,1,0,0,0,0}};
    for (u32 i=0;i<sizeof(invalid)/sizeof(invalid[0]);i++) {
        wasm_rt_trap_t bad=wasm_rt_impl_try();
        if (!bad) { w2c_env_HGNativeCardRequest(env,invalid[i][0],invalid[i][1],invalid[i][2],invalid[i][3],invalid[i][4],invalid[i][5]); CHECK(0); }
        CHECK(bad==WASM_RT_TRAP_OOB && !env->pendingCARD);
    }
    wasm_rt_trap_t bad=wasm_rt_impl_try();
    if (!bad) { w2c_env_HGNativeCardRequest(env,CARD_REQ_INIT,command,0,1,0,0); CHECK(0); }
    CHECK(bad==WASM_RT_TRAP_CALL_INDIRECT && !env->pendingCARD); wasm_rt_set_unwind_target(boundaryTarget);
    u8 oldBytes[1024],changed[1024]; memcpy(oldBytes,env->memory->data+input,sizeof(oldBytes));
    for (u32 i=0;i<1024;i++) changed[i]=oldBytes[i]^0x33;
    memcpy(env->memory->data+input,changed,sizeof(changed));
    CHECK(w2c_hg32_HGCardAction(&instance->game,3,0x1000)); w2c_hg32_HGCardAction(&instance->game,7,0); WaitCard(instance);
    u32 cancelled=w2c_hg32_HGCardAction(&instance->game,5,0);
    CHECK((cancelled==CARD_RESULT_SUCCESS || cancelled==CARD_RESULT_CANCELED) && w2c_hg32_HGCardMode(&instance->game,0)==1);
    u8 settingByte;
    CHECK(HGNativeBackupSettings(0,&settingByte,1,FALSE) && HGNativeBackupSettings(0,&settingByte,1,TRUE));
    CHECK(w2c_hg32_HGCardAction(&instance->game,2,0x1000));
    CHECK(!memcmp(env->memory->data+output,oldBytes,1024) || !memcmp(env->memory->data+output,changed,1024));
    u8 previous[1024],replacement[1024]; memcpy(previous,env->memory->data+output,sizeof(previous));
    for (u32 i=0;i<1024;i++) replacement[i]=env->memory->data[input+i]^0x5A;
    memcpy(env->memory->data+input,replacement,sizeof(replacement));
    u32 state=w2c_hg32_HGCardReference(&instance->game,1);
    WASM_RT_UNWIND_TARGET *saved=wasm_rt_get_unwind_target(); wasm_rt_trap_t trap=wasm_rt_impl_try();
    if (!trap) { w2c_hg32_HGCardAction(&instance->game,8,0x12345678); CHECK(0); }
    CHECK(trap==WASM_RT_TRAP_UNREACHABLE && env->resetRequested && env->pendingCARD==1);
    trap=wasm_rt_impl_try();
    if (!trap) { HGNativeReleaseIrqCallbacks(env); CHECK(0); }
    CHECK(trap==WASM_RT_TRAP_OOB && env->pendingCARD==1);
    wasm_rt_set_unwind_target(saved); HGNativeFinishReset(env);
    CHECK(!env->pendingCARD && !env->card && !read32(env->memory,state)); ResetRelease(instance);
    CHECK(HGNativeBackupSetPath(NULL) && HGNativeBackupSetPath(path) && HGNativeBackupConsoleId()==console);
    ResetInstance *reopened=ResetCreate(0x12345678); CHECK(w2c_hg32_HGCardSetup(&reopened->game));
    CHECK(!w2c_hg32_HGDwcInitialize(&reopened->game) && w2c_hg32_HGDwcIdentity(&reopened->game)==console);
    CHECK(w2c_hg32_HGCardAction(&reopened->game,0,CARD_BACKUP_TYPE_FLASH_4MBITS_EX));
    CHECK(w2c_hg32_HGCardAction(&reopened->game,2,0x1000));
    output=w2c_hg32_HGCardBuffer(&reopened->game,1);
    CHECK(!memcmp(reopened->native.memory->data+output,previous,1024) || !memcmp(reopened->native.memory->data+output,replacement,1024));
    CHECK(w2c_hg32_HGCardAction(&reopened->game,2,0x7FC00));
    CHECK(!memcmp(reopened->native.memory->data+output,oldBytes,1024));
    HGNativeReleaseIrqCallbacks(&reopened->native); ResetRelease(reopened);
    u8 settings[1024],unchanged[1024]; CHECK(HGNativeBackupSettings(0,settings,sizeof(settings),FALSE));
    memcpy(unchanged,settings,sizeof(settings));
    CHECK(!HGNativeBackupSettings(1024,settings,1,TRUE) && !HGNativeBackupSettings(UINT32_MAX,settings,1,FALSE));
    CHECK(HGNativeBackupSettings(0,settings,sizeof(settings),FALSE) && !memcmp(settings,unchanged,sizeof(settings)));
    snprintf(sidecar,sizeof(sidecar),"%s.wifi",path);
    CHECK(!truncate(sidecar,1023)); memset(settings,0xA5,sizeof(settings));
    CHECK(!HGNativeBackupSettings(0,settings,sizeof(settings),FALSE) && !HGNativeBackupSettings(0,unchanged,1,TRUE));
    for (u32 i=0;i<sizeof(settings);i++) CHECK(settings[i]==0xA5);
    struct stat settingsInfo; CHECK(!stat(sidecar,&settingsInfo) && settingsInfo.st_size==1023 && !unlink(sidecar));
    CHECK(!symlink(path,sidecar) && !HGNativeBackupSettings(0,settings,sizeof(settings),TRUE) && !unlink(sidecar));
    CHECK(HGNativeBackupSetPath(NULL) && !unlink(path));
    snprintf(sidecar,sizeof(sidecar),"%s.lock",path); CHECK(!unlink(sidecar));
    snprintf(sidecar,sizeof(sidecar),"%s.console",path); CHECK(!unlink(sidecar) && !rmdir(folder));
    puts("PASS: original SDK backup I/O, callbacks, reset, plus durable original DWC settings, CRC repair, identity, reopening and malformed-file protection.");
    return 0;
}
static int CheckResets(void) {
    char directory[]="ios/build-original-abi32/reset-storage-XXXXXX"; CHECK(mkdtemp(directory));
    char path[256]; snprintf(path,sizeof(path),"%s/progress.sav",directory);
    int file=open(path,O_CREAT|O_EXCL|O_WRONLY,0600); CHECK(file>=0 && write(file,"progress",8)==8 && !close(file));
    CHECK(HGNativeFSSetRoot(directory));
    const u32 parameters[]={0x89ABCDEF,0,0x13579BDF}; u32 boot=0;
    for (u32 scenario=0;scenario<3;scenario++) {
        ResetInstance *instance=ResetCreate(boot); struct w2c_env *env=&instance->native;
        CHECK(w2c_hg32_HGResetBootInfo(&instance->game)==boot);
        CHECK(!w2c_hg32_HGResetReadFile(&instance->game));
        u32 address=w2c_hg32_HGResetStateAddress(&instance->game);
        WASM_RT_UNWIND_TARGET *saved=wasm_rt_get_unwind_target(); wasm_rt_trap_t trap=wasm_rt_impl_try();
        if (!trap) { w2c_hg32_HGResetScenario(&instance->game,scenario,parameters[scenario]); CHECK(0); }
        CHECK(trap==WASM_RT_TRAP_UNREACHABLE && env->resetRequested && env->resetOccurred && env->resetParameter==parameters[scenario] && !env->resetFinished);
        wasm_rt_set_unwind_target(saved);
        CHECK(read32(env->memory,address)==(scenario==0 ? 1u : scenario==1 ? 2u : 4u));
        CHECK(!read32(env->memory,address+4) && !read32(env->memory,address+8) && !read32(env->memory,address+12));
        CHECK(read32(env->memory,address+16)==boot && !read32(env->memory,address+20));
        CHECK(env->pendingRTC==1 && env->pendingMIC==1 && env->pendingPOWER==1 && env->pendingINPUT==1);
        const float sample=0; CHECK(HGNativeFeedMic(env,&sample,1)==MIC_RESULT_ILLEGAL_STATUS);
        HGNativeFinishReset(env);
        CHECK(env->resetFinished && !env->pendingRTC && !env->pendingMIC && !env->pendingPOWER && !env->power && !env->pendingINPUT && !env->input && !env->pendingDMA && !env->threads && !env->mic && !env->irqTable);
        CHECK(!read32(env->memory,address+4) && !read32(env->memory,address+8) && OS_GetProcMode()==OS_PROCMODE_SYS && !OS_GetCpsrIrq());
        HGNativeFinishReset(env); boot=env->resetParameter; ResetRelease(instance);
    }
    ResetInstance *fresh=ResetCreate(boot);
    CHECK(w2c_hg32_HGResetBootInfo(&fresh->game)==boot && !w2c_hg32_HGResetReadFile(&fresh->game));
    HGNativeReleaseIrqCallbacks(&fresh->native); ResetRelease(fresh);
    CHECK(HGNativeFSSetRoot("ios/OriginalContent/files") && !unlink(path) && !rmdir(directory));
    puts("PASS: native reset from root/worker, suspended-thread teardown, queued RTC/MIC/power/input/DMA suppression, fresh boot parameters and persistent file preservation.");
    return 0;
}
int main(int argc,char **argv) {
    wasm_rt_init();
    if (argc==2 && !strcmp(argv[1],"--native-card")) { int result=CheckNativeCards(); wasm_rt_free(); return result; }
    if (argc==2 && !strcmp(argv[1],"--native-reset")) { int result=CheckResets(); wasm_rt_free(); return result; }
    w2c_hg32 first, second;
    struct w2c_wasi__snapshot__preview1 firstEnv={NULL,{dup(0),dup(1),dup(2)}}, secondEnv={NULL,{dup(0),dup(1),dup(2)}};
    struct w2c_env firstNative={NULL},secondNative={NULL};
    wasm2c_hg32_instantiate(&first,&firstNative,&firstEnv); wasm2c_hg32_instantiate(&second,&secondNative,&secondEnv);
    firstEnv.memory=w2c_hg32_memory(&first); secondEnv.memory=w2c_hg32_memory(&second);
    firstNative.memory=firstEnv.memory; secondNative.memory=secondEnv.memory;
    BindIRQ(&first,&firstNative); BindIRQ(&second,&secondNative);
    w2c_hg32_0x5Finitialize(&first); w2c_hg32_0x5Finitialize(&second);
    if (argc==2 && (!strcmp(argv[1],"--power-before-irq") || !strcmp(argv[1],"--input-before-irq"))) {
        u32 line=!strcmp(argv[1],"--input-before-irq") ? w2c_hg32_CheckNativeTouch(&first,1) : w2c_hg32_CheckNativePower(&first,1);
        if (line) fprintf(stderr,"Native peripheral pre-IRQ check line %u\n",line);
        CHECK(!line && !firstNative.pendingPOWER && !firstNative.pendingINPUT);
        HGNativeReleaseIrqCallbacks(&firstNative);
        wasm2c_hg32_free(&first); wasm2c_hg32_free(&second); wasm_rt_free();
        for (u32 i=0;i<3;i++) { close(firstEnv.fds[i]); close(secondEnv.fds[i]); }
        puts("PASS: power/input send failure before IRQ initialization preserves retryable SDK state.");
        return 0;
    }
    if (argc==2 && (!strcmp(argv[1],"--native-assert") || !strcmp(argv[1],"--console-exit") || !strcmp(argv[1],"--native-exception"))) {
        wasm_rt_trap_t trap=wasm_rt_impl_try();
        if (!trap) {
            if (!strcmp(argv[1],"--native-assert")) w2c_hg32_HGAotNativeAssert(&first);
            else if (!strcmp(argv[1],"--native-exception")) w2c_hg32_OSi_ExceptionHandler(&first);
            else w2c_hg32_HGConsoleAction(&first,4,0);
        }
        if (trap==WASM_RT_TRAP_UNREACHABLE) abort();
        return 2;
    }
    if (argc==2 && (!strcmp(argv[1],"--irq-overflow") || !strcmp(argv[1],"--irq-stack-reset"))) {
        w2c_hg32_HGIRQInit(&first);
        w2c_hg32_HGIRQRegister(&first,HG_IRQ_PLAIN,OS_IE_HBLANK,w2c_hg32_HGIRQReference(&first,!strcmp(argv[1],"--irq-overflow") ? 8 : 9),0);
        OS_EnableIrqMask(OS_IE_HBLANK); OS_EnableIrq(); HGNativePostIrq(OS_IE_HBLANK);
        return 2;
    }
    u32 overlayStateLine=w2c_hg32_CheckNativeOverlayState(&first);
    if (overlayStateLine) fprintf(stderr,"Native overlay state check line %u\n",overlayStateLine);
    CHECK(!overlayStateLine && !w2c_hg32_CheckNativeOverlayState(&second));
    puts("PASS: compiled C/assembly overlay images, BSS, function statics, relocated data/function pointers, constructors and original RTC/Pokewalker state restoration.");
    u32 lifecycleLine=w2c_hg32_CheckNativeOverlayLifecycle(&first);
    if (lifecycleLine) fprintf(stderr,"Native overlay lifecycle check line %u\n",lifecycleLine);
    CHECK(!lifecycleLine && !w2c_hg32_CheckNativeOverlayLifecycle(&second));
    puts("PASS: original overlay loader normal/no-init/async modes, native reloads, cross-unit priorities, legacy destructors, reentrant registration and unavailable-module rejection.");
    u32 dsProtectLine=w2c_hg32_CheckNativeDSProtect(&first);
    if (dsProtectLine) fprintf(stderr,"Native DS Protect contract check line %u\n",dsProtectLine);
    CHECK(!dsProtectLine && !w2c_hg32_CheckNativeDSProtect(&second));
    puts("PASS: native DS Protect platform classification, six original callback contracts, interrupt-state preservation and original module load/unload cycles.");
    u32 compressionLine=w2c_hg32_CheckOriginalUncompress(&first);
    if (compressionLine) fprintf(stderr,"Generated SDK decompression check line %u\n",compressionLine);
    CHECK(!compressionLine && !w2c_hg32_CheckOriginalUncompress(&second));
    u32 mathUnitLine=w2c_hg32_CheckOriginalMathUnit(&first);
    if (mathUnitLine) fprintf(stderr,"Generated SDK math unit check line %u\n",mathUnitLine);
    CHECK(!mathUnitLine && !w2c_hg32_CheckOriginalMathUnit(&second));
    u32 geometryLine=w2c_hg32_CheckOriginalGeometry(&first);
    if (geometryLine) fprintf(stderr,"Generated geometry check line %u\n",geometryLine);
    CHECK(!geometryLine && !w2c_hg32_CheckOriginalGeometry(&second));
    u32 primitiveLine=w2c_hg32_CheckOriginalPrimitives(&first);
    if (primitiveLine) fprintf(stderr,"Generated primitive check line %u\n",primitiveLine);
    CHECK(!primitiveLine && !w2c_hg32_CheckOriginalPrimitives(&second));
    u32 raster3DLine=w2c_hg32_CheckOriginalRaster3D(&first);
    if (raster3DLine) fprintf(stderr,"Generated 3D raster check line %u\n",raster3DLine);
    CHECK(!raster3DLine && !w2c_hg32_CheckOriginalRaster3D(&second));
    u32 textureLine=w2c_hg32_CheckOriginalTextures(&first);
    if (textureLine) fprintf(stderr,"Generated texture check line %u\n",textureLine);
    CHECK(!textureLine && !w2c_hg32_CheckOriginalTextures(&second));
    u32 alpha3DLine=w2c_hg32_CheckOriginalAlpha3D(&first);
    if (alpha3DLine) fprintf(stderr,"Generated 3D alpha check line %u\n",alpha3DLine);
    CHECK(!alpha3DLine && !w2c_hg32_CheckOriginalAlpha3D(&second));
    u32 lightingLine=w2c_hg32_CheckOriginalLighting(&first);
    if (lightingLine) fprintf(stderr,"Generated lighting check line %u\n",lightingLine);
    CHECK(!lightingLine && !w2c_hg32_CheckOriginalLighting(&second));
    u32 gecomLine=w2c_hg32_CheckOriginalGecom(&first);
    if (gecomLine) fprintf(stderr,"Generated geometry transport check line %u\n",gecomLine);
    CHECK(!gecomLine && !w2c_hg32_CheckOriginalGecom(&second));
    u32 softspriteLine=w2c_hg32_CheckOriginalSoftsprite(&first);
    if (softspriteLine) fprintf(stderr,"Generated software sprite check line %u\n",softspriteLine);
    CHECK(!softspriteLine && !w2c_hg32_CheckOriginalSoftsprite(&second));
    u32 heapLine=w2c_hg32_InitializeOriginalGameHeap(&first);
    if (heapLine) fprintf(stderr,"First game heap initialization check line %u\n",heapLine);
    CHECK(!heapLine);
    heapLine=w2c_hg32_InitializeOriginalGameHeap(&second);
    if (heapLine) fprintf(stderr,"Second game heap initialization check line %u\n",heapLine);
    CHECK(!heapLine);
    u32 otherLow=w2c_hg32_HGArenaReference(&second,OS_ARENA_MAIN,2);
    u32 arenaAllocation=w2c_hg32_HGArenaTake(&first,8);
    CHECK(arenaAllocation && w2c_hg32_HGArenaReference(&first,OS_ARENA_MAIN,2)==arenaAllocation+8);
    CHECK(w2c_hg32_HGArenaReference(&second,OS_ARENA_MAIN,2)==otherLow);
    u8 otherBytes[8]; memcpy(otherBytes,secondNative.memory->data+arenaAllocation,sizeof(otherBytes));
    memset(firstNative.memory->data+arenaAllocation,0x5A,sizeof(otherBytes));
    CHECK(!memcmp(otherBytes,secondNative.memory->data+arenaAllocation,sizeof(otherBytes)));
    heapLine=w2c_hg32_CheckOriginalGameHeaps(&first);
    if (heapLine) fprintf(stderr,"Game heap lifecycle check line %u\n",heapLine);
    CHECK(!heapLine && !w2c_hg32_CheckOriginalGameHeaps(&second));
    u32 startupLine=w2c_hg32_CheckOriginalStartupData(&first);
    if (startupLine) fprintf(stderr,"Original startup data check line %u\n",startupLine);
    CHECK(!startupLine && !w2c_hg32_CheckOriginalStartupData(&second));
    CHECK(!w2c_hg32_CheckNativeCartridgeEnable(&first) && !w2c_hg32_CheckNativeCartridgeEnable(&second));
    u32 work=w2c_hg32_HGSystemWorkAddress(&first),otherWork=w2c_hg32_HGSystemWorkAddress(&second);
    CHECK(work==w2c_hg32_HGLegacySystemWorkAddress(&first) && otherWork==w2c_hg32_HGLegacySystemWorkAddress(&second));
    CHECK(!w2c_hg32_CheckNativePXI(&first) && !w2c_hg32_CheckNativePXI(&second));
    firstNative.memory->data[work+0x3FF]=0xA5;
    CHECK(!secondNative.memory->data[otherWork+0x3FF]); firstNative.memory->data[work+0x3FF]=0;
    puts("PASS: native system-work layout/views/isolation and PXI callback lifecycle without hardware register changes or ARM7 acceptance.");
    u32 lockLine=w2c_hg32_CheckOriginalLockLifecycle(&first);
    if (lockLine) fprintf(stderr,"Original lock lifecycle check line %u\n",lockLine);
    CHECK(!lockLine && !w2c_hg32_CheckOriginalLockLifecycle(&second));
    firstNative.memory->data[work+0x3E0]=0x5A;
    CHECK(!secondNative.memory->data[otherWork+0x3E0]); firstNative.memory->data[work+0x3E0]=0;
    puts("PASS: original card/cartridge/word locks, owner errors, repeat initialization, interrupt flags, shared ID ownership and per-instance lock words.");
    CHECK(!w2c_hg32_CheckNativeBootInfo(&first) && !w2c_hg32_CheckNativeBootInfo(&second));
    firstNative.memory->data[work+0x40]=2;
    CHECK(secondNative.memory->data[otherWork+0x40]==1); firstNative.memory->data[work+0x40]=1;
    puts("PASS: original normal/download boot views and SDK-family agreement in independent native system-work storage.");
    u32 exceptionWork=w2c_hg32_HGExceptionAddress(&first,0),exceptionVector=w2c_hg32_HGExceptionAddress(&first,1);
    u32 exceptionLine=w2c_hg32_CheckOriginalExceptionInit(&first);
    if (exceptionLine) fprintf(stderr,"Original exception initialization check line %u\n",exceptionLine);
    CHECK(!exceptionLine && !read32(secondNative.memory,exceptionVector));
    for (u32 i=0;i<44;i++) CHECK(!secondNative.memory->data[exceptionWork+i]);
    CHECK(!w2c_hg32_CheckOriginalExceptionInit(&second));
    CHECK(read32(firstNative.memory,exceptionVector)==w2c_hg32_HGExceptionAddress(&first,3));
    puts("PASS: original exception initialization, 44-byte work layout, debugger/vector branches and independent native instance storage.");
    u32 startup=w2c_hg32_HGStartupData(&first,1),otherStartup=w2c_hg32_HGStartupData(&second,1);
    memset(firstNative.memory->data+startup,0x5A,28);
    for (u32 i=0;i<28;i++) CHECK(!secondNative.memory->data[otherStartup+i]);
    memset(firstNative.memory->data+startup,0,28);
    puts("PASS: native DTCM base/storage/alignment and original cartridge-state callback, offsets and per-instance ownership.");
    u32 pokewalkerLine=w2c_hg32_CheckOriginalPokewalkerState(&first);
    if (pokewalkerLine) fprintf(stderr,"Original Pokewalker state check line %u\n",pokewalkerLine);
    CHECK(!pokewalkerLine && !w2c_hg32_CheckOriginalPokewalkerState(&second));
    u32 sharedPtrLine=w2c_hg32_CheckOriginalSharedPtr(&first);
    if (sharedPtrLine) fprintf(stderr,"Original shared pointer check line %u\n",sharedPtrLine);
    CHECK(!sharedPtrLine && !w2c_hg32_CheckOriginalSharedPtr(&second));
    struct w2c_env unusedNative={0}; HGNativeReleaseIrqCallbacks(&unusedNative);
    w2c_env_HGNativeIrqSet(&firstNative,HG_IRQ_PLAIN,OS_IE_VBLANK,w2c_hg32_HGIRQReference(&first,0),0);
    HGNativeReleaseIrqCallbacks(&firstNative);
    HGNativeIrqBridge coldBridge; CHECK(!HGNativeGetIrqBridge(OS_IE_VBLANK,&coldBridge));
    BindIRQ(&first,&firstNative);
    w2c_hg32_HGMicAction(&first,0,0,0);
    CHECK(w2c_hg32_HGMicAction(&first,1,MIC_SAMPLING_TYPE_SIGNED_8BIT,0)==MIC_RESULT_SEND_ERROR && !firstNative.pendingMIC);
    CHECK(w2c_hg32_HGMicAction(&first,1,MIC_SAMPLING_TYPE_SIGNED_8BIT,0)==MIC_RESULT_SEND_ERROR && !firstNative.pendingMIC);
    const float noMicInput=0;
    CHECK(HGNativeFeedMic(&firstNative,&noMicInput,1)==MIC_RESULT_ILLEGAL_STATUS);
    CHECK(!w2c_hg32_CheckRTCBeforeIRQ(&first) && !firstNative.pendingRTC);
    CheckAtomicSwap(&first,&firstNative);
    CHECK(!w2c_hg32_HGSwapWordExchange(&second,0x87654321));
    CHECK(!w2c_hg32_HGSwapWordExchange(&first,0) && w2c_hg32_HGSwapWordExchange(&second,0)==0x87654321);
    CheckLockIDs(&first,&second);
    u32 cartridgeLine=w2c_hg32_CheckNativeCartridgeInit(&first);
    if (cartridgeLine) fprintf(stderr,"Native cartridge initialization check line %u\n",cartridgeLine);
    CHECK(!cartridgeLine && !w2c_hg32_CheckNativeCartridgeInit(&second));
    CHECK(w2c_hg32_HGCartridgeLockID(&first)!=w2c_hg32_HGCartridgeLockID(&second));
    puts("PASS: aligned original cartridge work, absent-slot module state, public queries, repeat initialization, IRQ state and independent instance IDs.");
    CheckCPU(&first,&firstNative);
    u32 svcLine=w2c_hg32_CheckOriginalSystemCalls(&first);
    if (svcLine) fprintf(stderr,"System call check line %u\n",svcLine);
    CHECK(!svcLine);
    u32 bindingLine=w2c_hg32_CheckOriginalLinkerBindings(&first);
    if (bindingLine) fprintf(stderr,"Linker binding check line %u\n",bindingLine);
    CHECK(!bindingLine);
    CheckRTC(&first,&firstNative);
    CheckMIC(&first,&second,&firstNative,&secondNative);
    CheckIRQ(&first,&firstNative);
    BindIRQ(&first,&firstNative);
    u32 displayLine=w2c_hg32_CheckOriginalDisplayClock(&first);
    if (displayLine) fprintf(stderr,"Generated display clock check line %u\n",displayLine);
    BindIRQ(&second,&secondNative);
    CHECK(!displayLine && !w2c_hg32_CheckOriginalDisplayClock(&second));
    CheckClock(&first,&second,&firstNative);
    CheckAlarm(&first,&firstNative);
    CheckDMA(&first,&firstNative);
    CheckFiles(&first,&second,&firstNative,&secondNative);
    CHECK(!w2c_hg32_CheckNativeRomArchive(&first) && !w2c_hg32_CheckNativeRomArchive(&second));
    u8 savedRomHeader[0x160]; memcpy(savedRomHeader,firstNative.memory->data+work+0x200,sizeof(savedRomHeader));
    WASM_RT_UNWIND_TARGET *romTarget=wasm_rt_get_unwind_target();
    u32 romStack=*firstNative.stackPointer;
    for (u32 i=0;i<2;i++) {
        w2c_hg32_HGPrepareRomArchive(&first,i ? 0x4A4B5049 : 0x4A414441,i ? 0 : 0x3130);
        wasm_rt_trap_t trap=wasm_rt_impl_try();
        if (!trap) { w2c_hg32_SystemCard_InitRomArchive(&first); CHECK(0); }
        CHECK(trap==WASM_RT_TRAP_UNREACHABLE);
        *firstNative.stackPointer=romStack;
    }
    memcpy(firstNative.memory->data+work+0x200,savedRomHeader,sizeof(savedRomHeader)); wasm_rt_set_unwind_target(romTarget);
    puts("PASS: original ROM archive setup from native HG/SS metadata, repeated setup and rejection of wrong game/maker codes.");
    u32 pokepicLine=w2c_hg32_CheckOriginalPokepic(&first);
    if (pokepicLine) fprintf(stderr,"Original ROM Pokepic draw check line %u\n",pokepicLine);
    CHECK(!pokepicLine && !w2c_hg32_CheckOriginalPokepic(&second));
    puts("PASS: original ROM Pokepic decode/upload/draw, two species front/back, both frames matching source PNG pixels, animation offsets and heap cleanup.");
    CHECK(!w2c_hg32_CheckOriginalCache(&first));
    CheckCacheOrdering(GeneratedCacheFence,&first);
    CHECK(!w2c_hg32_CheckOriginalIntegerRuntime(&first));
    u32 floatLine=w2c_hg32_CheckOriginalFloatRuntime(&first);
    if (floatLine) fprintf(stderr,"Float runtime check line %u\n",floatLine);
    CHECK(!floatLine);
    u32 formatLine=w2c_hg32_CheckOriginalNarrowFormat(&first);
    if (formatLine) fprintf(stderr,"Narrow formatter check line %u\n",formatLine);
    CHECK(!formatLine);
    u32 availabilityLine=w2c_hg32_CheckOriginalAvailability(&first);
    if (availabilityLine) fprintf(stderr,"Availability check line %u\n",availabilityLine);
    CHECK(!availabilityLine);
    formatLine=w2c_hg32_CheckOriginalWideFormat(&first);
    if (formatLine) fprintf(stderr,"Wide formatter check line %u\n",formatLine);
    CHECK(!formatLine);
    const u32 sizes[] = {4,136,236,12,4,8,0x48,0xE4,0x10,0x108,16,2,0x18,0x1C,0x14,0x28,20};
    for (u32 i=0;i<sizeof(sizes)/sizeof(sizes[0]);i++) {
        u32 actual = w2c_hg32_HGAotLayout(&first,i);
        if (actual != sizes[i]) fprintf(stderr,"Layout field %u: actual %u, expected %u\n",i,actual,sizes[i]);
        CHECK(actual==sizes[i]);
    }
    wasm_rt_memory_t *memory=w2c_hg32_memory(&first), *other=w2c_hg32_memory(&second);
    CHECK(memory->data != other->data);
    u32 offset=w2c_hg32_HGAotData(&first);
    CHECK(offset <= memory->size && 256 <= memory->size-offset);
    CHECK(w2c_hg32_HGAotPointerCheck(&first,offset,17));
    CHECK(!w2c_hg32_HGAotPointerCheck(&first,offset,33));
    CHECK(!w2c_hg32_HGAotPointerCheck(&first,offset+256,1));
    CheckWasiHost(&firstEnv,offset);
    CheckConsoleHost(&first,&firstEnv);
    u32 string=w2c_hg32_HGAotString(&first), iterator=w2c_hg32_HGAotIterator(&first);
    CHECK(read32(memory,string+4)==17 && read32(memory,string+8)==offset && read32(memory,iterator)==string);
    const u16 expected[] = {0,0xE97E,0x5271,0x31B0,0x8E42,0xE2CC,0xAFC5,0x67DB};
    w2c_hg32_HGAotSeed(&first,0); w2c_hg32_HGAotSeed(&second,0);
    for (u32 i=0;i<8;i++) CHECK(w2c_hg32_HGAotRandom(&first)==expected[i]);
    CHECK(w2c_hg32_HGAotRandom(&second)==expected[0]);
    w2c_hg32_HGAotMTSeed(&first,5489);
    CHECK(w2c_hg32_HGAotMTRandom(&first)==3499211612u);
    CHECK(w2c_hg32_HGAotMTRandom(&first)==581869302u);
    CHECK(w2c_hg32_HGAotMTRandom(&first)==3890346734u);
    memset(memory->data+offset,0,256); CHECK(w2c_hg32_HGAotEncrypt(&first,16,0));
    CHECK(memcmp(memory->data+offset,expected,16)==0);
    CHECK(w2c_hg32_HGAotDecrypt(&first,16,0));
    for (u32 i=0;i<16;i++) CHECK(memory->data[offset+i]==0);
    u8 original[256]; for (u32 i=0;i<256;i++) original[i]=(u8)(i*7+11);
    for (u32 length=0;length<=256;length+=2) {
        memcpy(memory->data+offset,original,256);
        u32 checksum=w2c_hg32_HGAotChecksum(&first,length);
        CHECK(w2c_hg32_HGAotEncrypt(&first,length,0x12345678^length));
        CHECK(w2c_hg32_HGAotDecrypt(&first,length,0x12345678^length));
        CHECK(memcmp(memory->data+offset,original,256)==0);
        CHECK(w2c_hg32_HGAotChecksum(&first,length)==checksum);
    }
    CHECK(!w2c_hg32_HGAotEncrypt(&first,258,0) && !w2c_hg32_HGAotDecrypt(&first,1,0));
    CHECK(w2c_hg32_HGAotChecksum(&first,258)==UINT32_MAX && memcmp(memory->data+offset,original,256)==0);
    memcpy(memory->data+offset,"alpha beta beta",15); memcpy(memory->data+offset+128,"beta",4);
    CHECK(w2c_hg32_HGAotLobbyFind(&first,15,4,0,0)==6);
    CHECK(w2c_hg32_HGAotLobbyFind(&first,15,4,7,0)==11);
    CHECK(w2c_hg32_HGAotLobbyFind(&first,15,4,12,0)==UINT32_MAX);
    CHECK(w2c_hg32_HGAotLobbyFind(&first,15,0,15,0)==15);
    CHECK(w2c_hg32_HGAotLobbyFind(&first,15,0,16,0)==UINT32_MAX);
    memcpy(memory->data+offset,"abcabc",6); memcpy(memory->data+offset+128,"bc",2);
    CHECK(w2c_hg32_HGAotLobbyFind(&first,6,2,2,1)==4);
    CHECK(w2c_hg32_HGAotLobbyFind(&first,12,2,0,1)==UINT32_MAX);
    CHECK(w2c_hg32_HGAotDequeCopy(&first));
    CHECK(w2c_hg32_HGAotWifiStatus(&first,0));
    CHECK(w2c_hg32_HGAotWifiStatus(&first,1));
    CHECK(w2c_hg32_HGAotBillboardFrames(&first));
    CHECK(w2c_hg32_HGAotBottomScreenCallbacks(&first,0));
    CHECK(w2c_hg32_HGAotBottomScreenCallbacks(&first,1));
    CHECK(!w2c_hg32_HGAotBottomScreenCallbacks(&first,2));
    CHECK(w2c_hg32_HGAotSequenceIndex(&first,0)==0xA010);
    CHECK(w2c_hg32_HGAotSequenceIndex(&first,4)==0xA010);
    CHECK(w2c_hg32_HGAotSequenceIndex(&first,5)==0xB020);
    CHECK(w2c_hg32_HGAotSequenceIndex(&first,14)==0xB020);
    CHECK(w2c_hg32_HGAotSequenceIndex(&first,15)==0xC030);
    CHECK(w2c_hg32_HGAotSequenceIndex(&first,65535)==0xC030);
    CHECK(w2c_hg32_HGAotSequenceIndex(&first,65536)==UINT32_MAX);
    CHECK(w2c_hg32_HGAotMovementAllowed(&first,0)==0);
    CHECK(w2c_hg32_HGAotMovementAllowed(&first,1)==1);
    CHECK(w2c_hg32_HGAotMovementAllowed(&first,0xA5)==0xA5);
    CHECK(w2c_hg32_HGAotMovementAllowed(&first,256)==UINT32_MAX);
    CHECK(w2c_hg32_HGAotAsmPointers(&first));
    CHECK(w2c_hg32_HGAotFactoryFlags(&first));
    CHECK(w2c_hg32_HGAotSDKMath(&first));
    CHECK(w2c_hg32_HGAotCommunicationModes(&first));
    CHECK(w2c_hg32_HGAotCommandDispatch(&first));
    CHECK(w2c_hg32_HGAotClubDisconnect(&first));
    CHECK(w2c_hg32_HGAotLobbyStringSearch(&first));
    CHECK(w2c_hg32_HGAotSendBufferSpace(&first));
    CHECK(w2c_hg32_HGAotMatchErrorRouting(&first));
    CHECK(w2c_hg32_HGAotSocketAdapters(&first));
    CHECK(w2c_hg32_HGAotTransportCallbacks(&first));
    CHECK(w2c_hg32_HGAotCourseStateDispatch(&first));
    CHECK(w2c_hg32_HGAotCompressedResource(&first,0,10,9));
    CHECK(w2c_hg32_HGAotCompressedResource(&first,1,8,4));
    for (u32 size=0;size<10;size++) CHECK(!w2c_hg32_HGAotCompressedResource(&first,0,size,9));
    for (u32 size=0;size<8;size++) CHECK(!w2c_hg32_HGAotCompressedResource(&first,1,size,4));
    CHECK(!w2c_hg32_HGAotCompressedResource(&first,0,10,8));
    CHECK(!w2c_hg32_HGAotCompressedResource(&first,2,7,3));
    u32 memoryLine=w2c_hg32_CheckNativeMemory(&first);
    if (memoryLine) fprintf(stderr,"Memory check line %u\n",memoryLine);
    CHECK(!memoryLine);
    u32 rc4Line=w2c_hg32_CheckOriginalRC4(&first);
    if (rc4Line) fprintf(stderr,"RC4 check line %u\n",rc4Line);
    CHECK(!rc4Line);
    u32 digestLine=w2c_hg32_CheckOriginalDigests(&first);
    if (digestLine) fprintf(stderr,"Digest check line %u\n",digestLine);
    CHECK(!digestLine);
    u32 sortLine=w2c_hg32_CheckOriginalQSort(&first);
    if (sortLine) fprintf(stderr,"Sort check line %u\n",sortLine);
    CHECK(!sortLine);
    u32 bigLine=w2c_hg32_CheckOriginalBignum(&first);
    if (bigLine) fprintf(stderr,"Big-number check line %u\n",bigLine);
    CHECK(!bigLine);
    u32 wcmLine=w2c_hg32_CheckOriginalWcm(&first);
    if (wcmLine) fprintf(stderr,"WCM check line %u\n",wcmLine);
    CHECK(!wcmLine);
    u32 stringLine=w2c_hg32_CheckOriginalLobbyStrings(&first);
    if (stringLine) fprintf(stderr,"Lobby string check line %u\n",stringLine);
    CHECK(!stringLine);
    u32 callbackLine=w2c_hg32_CheckOriginalChatDispatch(&first);
    if (callbackLine) fprintf(stderr,"Chat dispatch check line %u\n",callbackLine);
    CHECK(!callbackLine);
    u32 requestLine=w2c_hg32_CheckOriginalLobbyRequestMap(&first);
    if (requestLine) fprintf(stderr,"Request map check line %u\n",requestLine);
    CHECK(!requestLine);
    requestLine=w2c_hg32_CheckOriginalLobbyGrowth(&first);
    if (requestLine) fprintf(stderr,"Growth check line %u\n",requestLine);
    CHECK(!requestLine);
    requestLine=w2c_hg32_CheckOriginalLobbyAllocators(&first);
    if (requestLine) fprintf(stderr,"Allocator check line %u\n",requestLine);
    CHECK(!requestLine);
    requestLine=w2c_hg32_CheckOriginalLobbyMapErase(&first);
    if (requestLine) fprintf(stderr,"Map erase check line %u\n",requestLine);
    CHECK(!requestLine);
    requestLine=w2c_hg32_CheckOriginalLobbyUserRetention(&first);
    if (requestLine) fprintf(stderr,"User retention check line %u\n",requestLine);
    CHECK(!requestLine);
    requestLine=w2c_hg32_CheckOriginalLobbySchedule(&first);
    if (requestLine) fprintf(stderr,"Schedule check line %u\n",requestLine);
    CHECK(!requestLine);
    requestLine=w2c_hg32_CheckOriginalPPWTimers(&first);
    if (requestLine) fprintf(stderr,"PPW timer check line %u\n",requestLine);
    CHECK(!requestLine);
    requestLine=w2c_hg32_CheckOriginalLobbyEventForwarding(&first);
    if (requestLine) fprintf(stderr,"Event forwarding check line %u\n",requestLine);
    CHECK(!requestLine);
    requestLine=w2c_hg32_CheckOriginalWifiSetup(&first);
    if (requestLine) fprintf(stderr,"Wi-Fi setup check line %u\n",requestLine);
    CHECK(!requestLine);
    requestLine=w2c_hg32_CheckOriginalWifiFiles(&first);
    if (requestLine) fprintf(stderr,"Wi-Fi file check line %u\n",requestLine);
    CHECK(!requestLine);
    requestLine=w2c_hg32_CheckOriginalWifiPlaza(&first);
    if (requestLine) fprintf(stderr,"Wi-Fi plaza check line %u\n",requestLine);
    CHECK(!requestLine);
    requestLine=w2c_hg32_CheckOriginalSceneInterfaces(&first);
    if (requestLine) fprintf(stderr,"Scene interface check line %u\n",requestLine);
    CHECK(!requestLine);
    stringLine=w2c_hg32_CheckOriginalLobbyComparison(&first);
    if (stringLine) fprintf(stderr,"Lobby comparison check line %u\n",stringLine);
    CHECK(!stringLine);
    stringLine=w2c_hg32_CheckOriginalOwnedDeque(&first);
    if (stringLine) fprintf(stderr,"Owned deque check line %u\n",stringLine);
    CHECK(!stringLine);
    stringLine=w2c_hg32_CheckOriginalLobbyProfile(&first);
    if (stringLine) fprintf(stderr,"Profile cleanup check line %u\n",stringLine);
    CHECK(!stringLine);
    stringLine=w2c_hg32_CheckOriginalLobbyCleanup(&first);
    if (stringLine) fprintf(stderr,"Lobby cleanup check line %u\n",stringLine);
    CHECK(!stringLine);
    CHECK(w2c_hg32_HGAotEventLogSort(&first));
    CHECK(w2c_hg32_HGAotParticleCreation(&first));
    const struct { u32 count; s32 values[16]; } matrices[] = {
        {9,{4096,0,0,0,4096,0,0,0,4096}},
        {9,{4096,0,-2048,0,4096,0,2048,0,4096}},
        {9,{4096,2048,0,-2048,4096,0,0,0,4096}},
        {12,{4096,0,0,0,4096,0,0,0,4096,0,0,0}},
        {12,{2048,0,0,0,4096,0,0,0,1234,0,0,0}},
        {12,{4096,0,0,0,4096,2048,0,-2048,4096,0,0,0}},
        {12,{4096,0,-2048,0,4096,0,2048,0,4096,0,0,0}},
        {16,{4096,0,0,0,0,4096,0,0,0,0,4096,0,0,0,0,4096}},
        {16,{4096,0,0,0,0,4096,2048,0,0,-2048,4096,0,0,0,0,4096}},
        {16,{4096,0,-2048,0,0,4096,0,0,2048,0,4096,0,0,0,0,4096}},
        {16,{4096,2048,0,0,-2048,4096,0,0,0,0,4096,0,0,0,0,4096}},
        {16,{1,2,3,0,4,5,6,0,7,8,9,0,10,11,12,4096}},
        {12,{1,2,3,5,6,7,9,10,11,13,14,15}},
        {12,{1,2,3,5,6,7,9,10,11,13,14,15}},
    };
    for (u32 operation=0;operation<sizeof(matrices)/sizeof(matrices[0]);operation++) {
        u32 result=w2c_hg32_HGAotSDKMatrix(&first,operation,2048,4096);
        CHECK(result);
        for (u32 i=0;i<matrices[operation].count;i++) CHECK(read32(memory,result+4*i)==(u32)matrices[operation].values[i]);
        if (operation<11) {
            result=w2c_hg32_HGAotSDKMatrix(&first,operation,0x80000000u,4096);
            for (u32 i=0;i<matrices[operation].count;i++) {
                s32 expected=matrices[operation].values[i];
                CHECK(read32(memory,result+4*i)==(expected==2048 || expected==-2048 ? 0x80000000u : (u32)expected));
            }
        }
    }
    CHECK(!w2c_hg32_HGAotSDKMatrix(&first,14,0,0));
    const u32 grid[]={0,1,2,3,8,4,5,6,7,9,10,11,12,13,14};
    for (u32 y=0;y<3;y++) for (u32 x=0;x<5;x++) CHECK(w2c_hg32_HGAotSDKCharIndex(&first,5,3,x,y)==grid[y*5+x]);
    for (u32 h=1;h<=16;h++) for (u32 w=1;w<=16;w++) {
        u8 seen[256]={0};
        for (u32 y=0;y<h;y++) for (u32 x=0;x<w;x++) {
            u32 index=w2c_hg32_HGAotSDKCharIndex(&first,w,h,x,y);
            CHECK(index<w*h && !seen[index]); seen[index]=1;
        }
    }
    CHECK(w2c_hg32_HGAotSDKCharIndex(&first,0,1,0,0)==UINT32_MAX);
    w2c_hg32_HGAotSeed(&first,0);
    for (u32 i=0;i<8;i++) CHECK(w2c_hg32_HGAotAsmRandom(&first)==expected[i]);
    CHECK(w2c_hg32_HGAotSpawnWarp(&first,1,0,0)==63);
    CHECK(w2c_hg32_HGAotSpawnWarp(&first,1,0,2)==6 && w2c_hg32_HGAotSpawnWarp(&first,1,0,3)==8);
    CHECK(w2c_hg32_HGAotSpawnWarp(&first,1,1,0)==60);
    CHECK(w2c_hg32_HGAotSpawnWarp(&first,1,1,2)==695 && w2c_hg32_HGAotSpawnWarp(&first,1,1,3)==397);
    for (u32 id=1;id<=30;id++) for (u32 kind=0;kind<3;kind++) {
        CHECK(w2c_hg32_HGAotSpawnWarp(&first,id,kind,1)==UINT32_MAX);
        CHECK(w2c_hg32_HGAotSpawnWarp(&first,id,kind,4)==(kind==0 ? 0 : 1));
    }
    BindIRQ(&first,&firstNative);
    WASM_RT_UNWIND_TARGET *threadTarget=wasm_rt_get_unwind_target();
    wasm_rt_jmp_buf threadPrevious; memcpy(&threadPrevious,&g_wasm_rt_jmp_buf,sizeof(threadPrevious));
    wasm_rt_trap_t threadTrap=wasm_rt_impl_try();
    if (!threadTrap) {
        u32 line=w2c_hg32_CheckOriginalThreads(&first);
        if (line) fprintf(stderr,"Original thread check line %u\n",line);
        CHECK(!line);
    } else fprintf(stderr,"Original thread check trap: %s\n",wasm_rt_strerror(threadTrap));
    CHECK(!threadTrap);
    CheckThreadBoundaries(&firstNative);
    HGNativeReleaseIrqCallbacks(&firstNative); CHECK(!firstNative.threads);
    BindIRQ(&second,&secondNative);
    threadTrap=wasm_rt_impl_try();
    if (!threadTrap) {
        u32 line=w2c_hg32_CheckOriginalThreads(&second);
        if (line) fprintf(stderr,"Second original thread check line %u\n",line);
        CHECK(!line);
    }
    CHECK(!threadTrap);
    CheckThreadBoundaries(&secondNative);
    HGNativeReleaseIrqCallbacks(&secondNative); CHECK(!secondNative.threads);
    memcpy(&g_wasm_rt_jmp_buf,&threadPrevious,sizeof(threadPrevious)); wasm_rt_set_unwind_target(threadTarget);
    BindIRQ(&first,&firstNative);
    u32 powerLine=w2c_hg32_CheckNativePower(&first,0);
    if (powerLine) fprintf(stderr,"Original power check line %u\n",powerLine);
    CHECK(!powerLine && !firstNative.pendingPOWER);
    CheckPowerBoundaries(&first,&firstNative);
    CHECK(!w2c_hg32_HGPowerAction(&first,1,0) && w2c_hg32_HGPowerAction(&first,0,0)==2);
    BindIRQ(&second,&secondNative); powerLine=w2c_hg32_CheckNativePower(&second,0);
    if (powerLine) fprintf(stderr,"Second original power check line %u\n",powerLine);
    CHECK(!powerLine && !secondNative.pendingPOWER && w2c_hg32_HGPowerAction(&second,0,0)==3);
    CHECK(w2c_hg32_HGPowerAction(&first,0,0)==2);
    HGNativeReleaseIrqCallbacks(&firstNative); HGNativeReleaseIrqCallbacks(&secondNative);
    puts("PASS: original power callbacks, masked synchronous polling, reentrancy, logical backlights/amp/gain/LEDs and separate native states.");
    CheckInputs(&first,&second,&firstNative,&secondNative);
    CheckSounds(&first,&second,&firstNative,&secondNative);
    CHECK(!w2c_hg32_CheckNativeGameTimer(&first) && !w2c_hg32_CheckNativeGameTimer(&second));
    w2c_hg32_HGTimer3Action(&first,0,0); w2c_hg32_HGCPUAction(&first,7,100000);
    w2c_hg32_HGTimer3Action(&second,0,0);
    u64 firstTime=w2c_hg32_HGTimer3Action(&first,1,0),secondTime=w2c_hg32_HGTimer3Action(&second,1,0);
    CHECK(firstTime>secondTime);
    const u64 durations[]={0,1,33514000,0xFFFFFFFFFFFFFFFFULL,0x8000000000000000ULL};
    for (u32 i=0;i<sizeof(durations)/sizeof(durations[0]);i++)
        CHECK(w2c_hg32_HGTimer3Action(&first,2,durations[i])==(u64)(((unsigned __int128)durations[i]*64)/33514000));
    puts("PASS: native game timer epochs, independent system-tick adjustment, IRQ preservation and full-width seconds conversion.");
    w2c_hg32_HGReleaseLockID(&first,w2c_hg32_HGCartridgeLockID(&first));
    w2c_hg32_HGReleaseLockID(&second,w2c_hg32_HGCartridgeLockID(&second));
    wasm2c_hg32_free(&first); wasm2c_hg32_free(&second); wasm_rt_free();
    for (u32 i=0;i<3;i++) { if (firstEnv.fds[i]>=0) close(firstEnv.fds[i]); if (secondEnv.fds[i]>=0) close(secondEnv.fds[i]); }
    puts("PASS: native generated C preserves original layouts, RNG/crypto, lobby ABI, Wi-Fi returns, animation sequences and field callbacks.");
    return 0;
}
