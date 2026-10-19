// SPDX-License-Identifier: GPL-3.0-or-later
#define _POSIX_C_SOURCE 200809L
#define _DARWIN_C_SOURCE 1
#include "hg32.h"
#include "NativeImports.h"
#include <nitro/os/native_thread.h>
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
#include <sys/uio.h>
#include <time.h>
#include <unistd.h>
#include "../NativeEvents.h"
#include <nitro/os/native_irq.h>
#include <nitro/rtc/native_api.h>
#include <nitro/rtc/ARM9/api.h>
#include <nitro/os/native_clock.h>
#include <nitro/hw/consts_shared.h>
#include <nitro/mi/dma.h>
#include <nitro/mi/native_dma.h>
#include <nitro/os/arena.h>
#include <nitro/fs/native_file.h>
#include "../NativeFile.h"

#define CHECK(v) do { if (!(v)) { fprintf(stderr,"ABI32 check line %d: %s\n",__LINE__,#v); abort(); } } while (0)
#include "../tests/cache_threads.h"
static void GeneratedCacheFence(void *context,unsigned operation) {
    w2c_hg32_HGCacheOperation(context,operation,0,0);
}
static u32 read32(wasm_rt_memory_t *memory, u32 offset) {
    CHECK(offset <= memory->size && 4 <= memory->size-offset);
    u32 value; memcpy(&value,memory->data+offset,4); return value;
}

// Host-check bindings only: each instance owns copies of its standard descriptors.
struct w2c_wasi__snapshot__preview1 { wasm_rt_memory_t *memory; int fds[3]; };
enum { WASI_AGAIN=6,WASI_BADF=8,WASI_FAULT=21,WASI_INTR=27,WASI_INVAL=28,WASI_IO=29,WASI_OVERFLOW=61,WASI_SPIPE=70 };
static u32 wasiError(void) {
    switch (errno) {
    case EAGAIN: return WASI_AGAIN;
    case EBADF: return WASI_BADF;
    case EFAULT: return WASI_FAULT;
    case EINTR: return WASI_INTR;
    case EINVAL: return WASI_INVAL;
    case EOVERFLOW: return WASI_OVERFLOW;
    case ESPIPE: return WASI_SPIPE;
    default: return WASI_IO;
    }
}
static u8 *wasiSpan(struct w2c_wasi__snapshot__preview1 *env,u32 offset,u64 size) {
    if (!env->memory || offset>env->memory->size || size>env->memory->size-offset) return NULL;
    return env->memory->data+offset;
}
static int wasiFd(struct w2c_wasi__snapshot__preview1 *env,u32 fd) { return fd<3 ? env->fds[fd] : -1; }
u32 w2c_wasi__snapshot__preview1_clock_time_get(struct w2c_wasi__snapshot__preview1 *env,u32 id,u64 precision,u32 output) {
    (void)precision;
    const clockid_t clocks[]={CLOCK_REALTIME,CLOCK_MONOTONIC,CLOCK_PROCESS_CPUTIME_ID,CLOCK_THREAD_CPUTIME_ID};
    if (id>=4) return WASI_INVAL;
    u8 *dest=wasiSpan(env,output,8);
    if (!dest) return WASI_FAULT;
    struct timespec value;
    if (clock_gettime(clocks[id],&value)) return wasiError();
    if (value.tv_sec<0 || (u64)value.tv_sec>(UINT64_MAX-(u64)value.tv_nsec)/1000000000) return WASI_OVERFLOW;
    u64 nanos=(u64)value.tv_sec*1000000000+(u64)value.tv_nsec;
    memcpy(dest,&nanos,8);
    return 0;
}
u32 w2c_wasi__snapshot__preview1_fd_close(struct w2c_wasi__snapshot__preview1 *env,u32 fd) {
    int host=wasiFd(env,fd);
    if (host<0) return WASI_BADF;
    env->fds[fd]=-1;
    return close(host) ? wasiError() : 0;
}
u32 w2c_wasi__snapshot__preview1_fd_fdstat_get(struct w2c_wasi__snapshot__preview1 *env,u32 fd,u32 output) {
    int host=wasiFd(env,fd);
    if (host<0) return WASI_BADF;
    u8 *dest=wasiSpan(env,output,24);
    if (!dest) return WASI_FAULT;
    struct stat info;
    if (fstat(host,&info)) return wasiError();
    int flags=fcntl(host,F_GETFL);
    if (flags<0) return wasiError();
    u16 fdflags=((flags&O_APPEND) ? 1 : 0)|((flags&O_NONBLOCK) ? 4 : 0)|((flags&O_SYNC) ? 16 : 0);
    u64 rights=0;
    if ((flags&O_ACCMODE)!=O_WRONLY) rights|=1ULL<<1;
    if ((flags&O_ACCMODE)!=O_RDONLY) rights|=1ULL<<6;
    if (S_ISREG(info.st_mode)) rights|=(1ULL<<2)|(1ULL<<5);
    memset(dest,0,24);
    dest[0]=S_ISREG(info.st_mode) ? 4 : S_ISCHR(info.st_mode) ? 2 : S_ISDIR(info.st_mode) ? 3 : 0;
    memcpy(dest+2,&fdflags,2); memcpy(dest+8,&rights,8);
    return 0;
}
u32 w2c_wasi__snapshot__preview1_fd_seek(struct w2c_wasi__snapshot__preview1 *env,u32 fd,u64 offset,u32 whence,u32 output) {
    int host=wasiFd(env,fd);
    if (host<0) return WASI_BADF;
    u8 *dest=wasiSpan(env,output,8);
    if (!dest) return WASI_FAULT;
    if (whence>2) return WASI_INVAL;
    int64_t signedOffset; memcpy(&signedOffset,&offset,8);
    const int origins[]={SEEK_SET,SEEK_CUR,SEEK_END};
    off_t result=lseek(host,(off_t)signedOffset,origins[whence]);
    if (result<0) return wasiError();
    u64 position=(u64)result; memcpy(dest,&position,8);
    return 0;
}
u32 w2c_wasi__snapshot__preview1_fd_write(struct w2c_wasi__snapshot__preview1 *env,u32 fd,u32 vectors,u32 count,u32 output) {
    int host=wasiFd(env,fd);
    if (host<0) return WASI_BADF;
    u8 *dest=wasiSpan(env,output,4), *table=wasiSpan(env,vectors,(u64)count*8);
    if (!dest || !table) return WASI_FAULT;
    if (count>IOV_MAX) return WASI_INVAL;
    struct iovec entries[IOV_MAX];
    u64 total=0;
    for (u32 i=0;i<count;i++) {
        u32 address,length; memcpy(&address,table+i*8,4); memcpy(&length,table+i*8+4,4);
        entries[i].iov_base=wasiSpan(env,address,length); entries[i].iov_len=length;
        if (!entries[i].iov_base) return WASI_FAULT;
        total+=length;
    }
    if (total>UINT32_MAX) return WASI_OVERFLOW;
    ssize_t result=writev(host,entries,(int)count);
    if (result<0) return wasiError();
    u32 written=(u32)result; memcpy(dest,&written,4);
    return 0;
}
static void CheckWasiHost(struct w2c_wasi__snapshot__preview1 *env,u32 offset) {
    wasm_rt_memory_t *memory=env->memory;
    char path[]="ios/build-original-abi32/io-check-XXXXXX";
    int file=mkstemp(path), saved=env->fds[1]; CHECK(file>=0 && !unlink(path)); env->fds[1]=file;
    u32 vectors[]={offset+96,2,offset+112,2}; memcpy(memory->data+offset,vectors,sizeof(vectors));
    memcpy(memory->data+offset+96,"ab",2); memcpy(memory->data+offset+112,"cd",2);
    CHECK(!w2c_wasi__snapshot__preview1_fd_write(env,1,offset,2,offset+40) && read32(memory,offset+40)==4);
    CHECK(!w2c_wasi__snapshot__preview1_fd_seek(env,1,0,0,offset+32));
    char bytes[4]; CHECK(read(file,bytes,4)==4 && !memcmp(bytes,"abcd",4));
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
    CHECK(read32(memory,offset+40)==4);
    CHECK(w2c_wasi__snapshot__preview1_fd_write(env,3,offset,0,offset+40)==WASI_BADF);
    CHECK(w2c_wasi__snapshot__preview1_fd_seek(env,1,0,3,offset+32)==WASI_INVAL);
    CHECK(w2c_wasi__snapshot__preview1_fd_seek(env,1,UINT64_MAX,0,offset+32)==WASI_INVAL);
    CHECK(!w2c_wasi__snapshot__preview1_fd_close(env,1));
    CHECK(w2c_wasi__snapshot__preview1_fd_close(env,1)==WASI_BADF);
    env->fds[1]=saved;
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
    w2c_hg32_HGCPUAction(game,2,0); CHECK(w2c_hg32_HGIRQRead(game,0)==3);
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
    w2c_hg32_HGIRQAction(game,HG_IRQ_DISABLE,0); HGNativePostIrq(OS_IE_VBLANK);
    HGNativeReleaseIrqCallbacks(env); OS_EnableIrq(); HGNativePostIrq(OS_IE_VBLANK|OS_IE_HBLANK);
    CHECK(w2c_hg32_HGIRQRead(game,0)==9 && !(OS_GetIrqMask()&(OS_IE_VBLANK|OS_IE_HBLANK)));
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
int main(int argc,char **argv) {
    wasm_rt_init(); w2c_hg32 first, second;
    struct w2c_wasi__snapshot__preview1 firstEnv={NULL,{dup(0),dup(1),dup(2)}}, secondEnv={NULL,{dup(0),dup(1),dup(2)}};
    struct w2c_env firstNative={NULL},secondNative={NULL};
    wasm2c_hg32_instantiate(&first,&firstNative,&firstEnv); wasm2c_hg32_instantiate(&second,&secondNative,&secondEnv);
    firstEnv.memory=w2c_hg32_memory(&first); secondEnv.memory=w2c_hg32_memory(&second);
    firstNative.memory=firstEnv.memory; secondNative.memory=secondEnv.memory;
    BindIRQ(&first,&firstNative); BindIRQ(&second,&secondNative);
    w2c_hg32_0x5Finitialize(&first); w2c_hg32_0x5Finitialize(&second);
    if (argc==2 && !strcmp(argv[1],"--irq-overflow")) {
        w2c_hg32_HGIRQInit(&first);
        w2c_hg32_HGIRQRegister(&first,HG_IRQ_PLAIN,OS_IE_HBLANK,w2c_hg32_HGIRQReference(&first,8),0);
        OS_EnableIrqMask(OS_IE_HBLANK); OS_EnableIrq(); HGNativePostIrq(OS_IE_HBLANK);
        return 2;
    }
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
    struct w2c_env unusedNative={0}; HGNativeReleaseIrqCallbacks(&unusedNative);
    w2c_env_HGNativeIrqSet(&firstNative,HG_IRQ_PLAIN,OS_IE_VBLANK,w2c_hg32_HGIRQReference(&first,0),0);
    HGNativeReleaseIrqCallbacks(&firstNative);
    HGNativeIrqBridge coldBridge; CHECK(!HGNativeGetIrqBridge(OS_IE_VBLANK,&coldBridge));
    BindIRQ(&first,&firstNative);
    CHECK(!w2c_hg32_CheckRTCBeforeIRQ(&first) && !firstNative.pendingRTC);
    CheckAtomicSwap(&first,&firstNative);
    CHECK(!w2c_hg32_HGSwapWordExchange(&second,0x87654321));
    CHECK(!w2c_hg32_HGSwapWordExchange(&first,0) && w2c_hg32_HGSwapWordExchange(&second,0)==0x87654321);
    CheckLockIDs(&first,&second);
    CheckCPU(&first,&firstNative);
    u32 svcLine=w2c_hg32_CheckOriginalSystemCalls(&first);
    if (svcLine) fprintf(stderr,"System call check line %u\n",svcLine);
    CHECK(!svcLine);
    u32 bindingLine=w2c_hg32_CheckOriginalLinkerBindings(&first);
    if (bindingLine) fprintf(stderr,"Linker binding check line %u\n",bindingLine);
    CHECK(!bindingLine);
    CheckRTC(&first,&firstNative);
    CheckIRQ(&first,&firstNative);
    CheckClock(&first,&second,&firstNative);
    CheckAlarm(&first,&firstNative);
    CheckDMA(&first,&firstNative);
    CheckFiles(&first,&second,&firstNative,&secondNative);
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
    wasm2c_hg32_free(&first); wasm2c_hg32_free(&second); wasm_rt_free();
    for (u32 i=0;i<3;i++) { if (firstEnv.fds[i]>=0) close(firstEnv.fds[i]); if (secondEnv.fds[i]>=0) close(secondEnv.fds[i]); }
    puts("PASS: native generated C preserves original layouts, RNG/crypto, lobby ABI, Wi-Fi returns, animation sequences and field callbacks.");
    return 0;
}
