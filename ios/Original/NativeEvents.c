// SPDX-License-Identifier: GPL-3.0-or-later
#include "NativeEvents.h"
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <nitro/os/native_irq.h>

OSIrqFunction OS_IRQTable[OS_IRQ_TABLE_MAX];
OSIrqCallbackInfo OSi_IrqCallbackInfo[OSi_IRQCALLBACK_NUM];
static pthread_mutex_t eventMutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t eventChanged = PTHREAD_COND_INITIALIZER;
static OSIrqMask enabledMask, pendingMask, checkFlags;
static BOOL masterEnabled;
static u32 vblankCount;
static pthread_t eventThread;
static BOOL eventThreadSet;
static _Thread_local BOOL dispatching;
static void (*irqExitHook)(void *);
static void *irqExitContext;
static BOOL argumentCallback[OS_IRQ_TABLE_MAX];
static HGNativeIrqBridge bridges[OS_IRQ_TABLE_MAX];
static _Thread_local HGNativeIrqBridge activeBridge;
static void invokeBridge(void) {
    if (!activeBridge.invoke) abort();
    activeBridge.invoke(activeBridge.context,activeBridge.function,activeBridge.argument);
}
typedef struct NativeIrqJob {
    void (*callback)(void *);
    void *argument;
    struct NativeIrqJob *next;
} NativeIrqJob;
static NativeIrqJob *jobHead[OS_IRQ_TABLE_MAX], *jobTail[OS_IRQ_TABLE_MAX];
static OSIrqMask queuedMask;
void OS_IrqDummy(void) {}

static int callbackIndex(unsigned bit) {
    if (bit >= REG_OS_IE_D0_SHIFT && bit <= REG_OS_IE_D3_SHIFT) return (int)bit - REG_OS_IE_D0_SHIFT;
    if (bit >= REG_OS_IE_T0_SHIFT && bit <= REG_OS_IE_T3_SHIFT) return (int)bit - REG_OS_IE_T0_SHIFT + OSi_IRQCALLBACK_NO_TIMER0;
    return -1;
}
void OS_InitIrqTable(void) {
    pthread_mutex_lock(&eventMutex);
    // Reinitializing with pending peripheral replies would strand their callers.
    if (queuedMask) abort();
    queuedMask = 0;
    memset(OS_IRQTable, 0, sizeof(OS_IRQTable)); memset(OSi_IrqCallbackInfo, 0, sizeof(OSi_IrqCallbackInfo));
    memset(argumentCallback, 0, sizeof(argumentCallback));
    memset(bridges,0,sizeof(bridges));
    for (unsigned bit = 0; bit < OS_IRQ_TABLE_MAX; bit++) if (callbackIndex(bit) < 0) OS_IRQTable[bit] = OS_IrqDummy;
    enabledMask = pendingMask = checkFlags = 0; masterEnabled = FALSE; vblankCount = 0;
    eventThread = pthread_self(); eventThreadSet = TRUE;
    pthread_mutex_unlock(&eventMutex);
}
void OS_SetIrqFunction(OSIrqMask mask, OSIrqFunction function) {
    pthread_mutex_lock(&eventMutex);
    for (unsigned bit = 0; bit < OS_IRQ_TABLE_MAX; bit++) if (mask & (1u << bit)) {
        OS_IRQTable[bit] = function; argumentCallback[bit] = FALSE;
        bridges[bit]=(HGNativeIrqBridge){0};
        int index = callbackIndex(bit);
        if (index >= 0) OSi_IrqCallbackInfo[index].enable = TRUE;
    }
    pthread_mutex_unlock(&eventMutex);
}
OSIrqFunction OS_GetIrqFunction(OSIrqMask mask) {
    OSIrqFunction result = NULL;
    pthread_mutex_lock(&eventMutex);
    for (unsigned bit = 0; bit < OS_IRQ_TABLE_MAX; bit++) if (mask & (1u << bit)) {
        int index = callbackIndex(bit);
        result = argumentCallback[bit] ? (OSIrqFunction)OSi_IrqCallbackInfo[index].func : OS_IRQTable[bit]; break;
    }
    pthread_mutex_unlock(&eventMutex); return result;
}
BOOL OS_RestoreIrq(BOOL enabled) {
    pthread_mutex_lock(&eventMutex); BOOL previous = masterEnabled; masterEnabled = enabled != 0;
    pthread_cond_broadcast(&eventChanged); pthread_mutex_unlock(&eventMutex);
    if (enabled) HGNativeDispatchIrqs();
    return previous;
}
BOOL OS_EnableIrq(void) { return OS_RestoreIrq(TRUE); }
BOOL OS_DisableIrq(void) { return OS_RestoreIrq(FALSE); }
static OSIrqMask changeMask(OSIrqMask set, OSIrqMask clear) {
    pthread_mutex_lock(&eventMutex); OSIrqMask previous = enabledMask;
    enabledMask = (enabledMask & ~clear) | set;
    pthread_cond_broadcast(&eventChanged); pthread_mutex_unlock(&eventMutex); HGNativeDispatchIrqs(); return previous;
}
OSIrqMask OS_SetIrqMask(OSIrqMask mask) { return changeMask(mask, UINT32_MAX); }
OSIrqMask OS_EnableIrqMask(OSIrqMask mask) { return changeMask(mask, 0); }
OSIrqMask OS_DisableIrqMask(OSIrqMask mask) { return changeMask(0, mask); }
OSIrqMask OS_GetIrqMask(void) {
    pthread_mutex_lock(&eventMutex); OSIrqMask value = enabledMask; pthread_mutex_unlock(&eventMutex); return value;
}
OSIrqMask OS_GetRequestIrqMask(void) {
    pthread_mutex_lock(&eventMutex); OSIrqMask value = pendingMask | queuedMask; pthread_mutex_unlock(&eventMutex); return value;
}
OSIrqMask OS_ResetRequestIrqMask(OSIrqMask mask) {
    pthread_mutex_lock(&eventMutex); OSIrqMask previous = pendingMask | queuedMask; pendingMask &= ~mask;
    pthread_mutex_unlock(&eventMutex); return previous;
}
void OS_SetIrqCheckFlag(OSIrqMask mask) {
    pthread_mutex_lock(&eventMutex); checkFlags |= mask; pthread_cond_broadcast(&eventChanged); pthread_mutex_unlock(&eventMutex);
}
void OS_ClearIrqCheckFlag(OSIrqMask mask) {
    pthread_mutex_lock(&eventMutex); checkFlags &= ~mask; pthread_mutex_unlock(&eventMutex);
}
u32 OS_GetIrqCheckFlag(void) {
    HGNativeDispatchIrqs();
    pthread_mutex_lock(&eventMutex); u32 value = checkFlags; pthread_mutex_unlock(&eventMutex); return value;
}
void HGNativePostIrq(OSIrqMask mask) {
    if (mask & ~((1u << OS_IRQ_TABLE_MAX) - 1)) abort();
    pthread_mutex_lock(&eventMutex); pendingMask |= mask; pthread_cond_broadcast(&eventChanged); pthread_mutex_unlock(&eventMutex);
    HGNativeDispatchIrqs();
}
BOOL HGNativeQueueIrqCallback(OSIrqMask mask, void (*callback)(void *), void *argument) {
    if (!callback || !mask || (mask & (mask - 1)) || (mask & ~((1u << OS_IRQ_TABLE_MAX) - 1))) return FALSE;
    NativeIrqJob *job = malloc(sizeof(*job));
    if (!job) return FALSE;
    *job = (NativeIrqJob){callback, argument, NULL};
    unsigned bit = (unsigned)__builtin_ctz(mask);
    pthread_mutex_lock(&eventMutex);
    if (!eventThreadSet) { pthread_mutex_unlock(&eventMutex); free(job); return FALSE; }
    if (jobTail[bit]) jobTail[bit]->next = job;
    else jobHead[bit] = job;
    jobTail[bit] = job; queuedMask |= mask;
    pthread_cond_broadcast(&eventChanged); pthread_mutex_unlock(&eventMutex);
    HGNativeDispatchIrqs();
    return TRUE;
}
void HGNativeSignalVBlank(void) {
    pthread_mutex_lock(&eventMutex); vblankCount++; pendingMask |= OS_IE_VBLANK;
    pthread_cond_broadcast(&eventChanged); pthread_mutex_unlock(&eventMutex);
    HGNativeDispatchIrqs();
}
u32 OS_GetVBlankCount(void) {
    pthread_mutex_lock(&eventMutex); u32 value = vblankCount; pthread_mutex_unlock(&eventMutex); return value;
}
static void enterCallback(u32 number, BOOL timer, void (*callback)(void *), void *argument) {
    if (number >= 4) abort();
    unsigned bit = number + (timer ? REG_OS_IE_T0_SHIFT : REG_OS_IE_D0_SHIFT);
    int index = callbackIndex(bit);
    pthread_mutex_lock(&eventMutex);
    OSi_IrqCallbackInfo[index] = (OSIrqCallbackInfo){ callback, timer || (enabledMask & (1u << bit)), argument };
    bridges[bit]=(HGNativeIrqBridge){0};
    argumentCallback[bit] = TRUE; enabledMask |= 1u << bit;
    pthread_cond_broadcast(&eventChanged); pthread_mutex_unlock(&eventMutex); HGNativeDispatchIrqs();
}
void OSi_EnterDmaCallback(u32 number, void (*callback)(void *), void *argument) { enterCallback(number, FALSE, callback, argument); }
void OSi_EnterTimerCallback(u32 number, void (*callback)(void *), void *argument) { enterCallback(number, TRUE, callback, argument); }
void HGNativeDispatchIrqs(void) {
    if (dispatching || OS_GetCpsrIrq() == OS_INTRMODE_IRQ_DISABLE) return;
    pthread_mutex_lock(&eventMutex);
    BOOL owner = eventThreadSet && pthread_equal(eventThread, pthread_self());
    pthread_mutex_unlock(&eventMutex);
    if (!owner) return;
    dispatching = TRUE;
    BOOL interrupted=FALSE;
    for (;;) {
        pthread_mutex_lock(&eventMutex);
        OSIrqMask ready = masterEnabled ? (pendingMask | queuedMask) & enabledMask : 0;
        if (!ready) { pthread_mutex_unlock(&eventMutex); break; }
        interrupted=TRUE;
        unsigned bit = (unsigned)__builtin_ctz(ready); OSIrqMask mask = 1u << bit;
        NativeIrqJob *job = jobHead[bit];
        if (job) {
            jobHead[bit] = job->next;
            if (!job->next) { jobTail[bit] = NULL; queuedMask &= ~mask; }
        } else pendingMask &= ~mask;
        OSIrqFunction plain = job ? NULL : OS_IRQTable[bit];
        HGNativeIrqBridge bridge=plain==invokeBridge ? bridges[bit] : (HGNativeIrqBridge){0};
        int index = job ? -1 : callbackIndex(bit); BOOL hasArgument = !job && argumentCallback[bit];
        if (index >= 0) { OS_IRQTable[bit] = NULL; bridges[bit]=(HGNativeIrqBridge){0}; }
        OSIrqCallbackInfo info = {0};
        if (hasArgument) { info = OSi_IrqCallbackInfo[index]; OSi_IrqCallbackInfo[index].func = NULL; }
        pthread_mutex_unlock(&eventMutex);
        OSIntrMode previousMask = OS_DisableInterrupts();
        OSProcMode previousMode = HGNativeSetExecutionMode(OS_PROCMODE_IRQ);
        HGNativeIrqBridge previousBridge=activeBridge; activeBridge=bridge;
        if (job) { job->callback(job->argument); free(job); OS_SetIrqCheckFlag(mask); }
        else if (hasArgument) { if (info.func) info.func(info.arg); }
        else if (plain) plain();
        activeBridge=previousBridge;
        HGNativeSetExecutionMode(previousMode); OS_RestoreInterrupts(previousMask);
        if (index >= 0) {
            OS_SetIrqCheckFlag(mask);
            pthread_mutex_lock(&eventMutex);
            if (!OSi_IrqCallbackInfo[index].enable) enabledMask &= ~mask;
            pthread_mutex_unlock(&eventMutex);
        }
    }
    dispatching = FALSE;
    pthread_mutex_lock(&eventMutex);
    void (*hook)(void *)=irqExitHook; void *context=irqExitContext;
    pthread_mutex_unlock(&eventMutex);
    if (interrupted && hook) hook(context);
}
BOOL HGNativeSetIrqExitHook(void (*callback)(void *),void *context) {
    pthread_mutex_lock(&eventMutex);
    BOOL allowed=eventThreadSet && pthread_equal(eventThread,pthread_self()) && !dispatching &&
        (!irqExitHook || irqExitContext==context);
    if (allowed) { irqExitHook=callback; irqExitContext=context; }
    pthread_mutex_unlock(&eventMutex); return allowed;
}
BOOL HGNativeClearIrqExitHook(void *context) {
    pthread_mutex_lock(&eventMutex);
    BOOL allowed=!dispatching && (!eventThreadSet || pthread_equal(eventThread,pthread_self()));
    if (allowed && irqExitContext==context) { irqExitHook=NULL; irqExitContext=NULL; }
    pthread_mutex_unlock(&eventMutex); return allowed;
}
void HGNativeHalt(void) {
    // ARM946E-S WFI wakes independently of CPSR.I; callback dispatch still obeys it.
    pthread_mutex_lock(&eventMutex);
    if (!eventThreadSet || !pthread_equal(eventThread,pthread_self())) abort();
    while (!(masterEnabled && ((pendingMask|queuedMask)&enabledMask))) pthread_cond_wait(&eventChanged,&eventMutex);
    pthread_mutex_unlock(&eventMutex); HGNativeDispatchIrqs();
}

void HGNativeInstallIrqBridge(u32 kind,u32 target,const HGNativeIrqBridge *bridge) {
    if (!bridge || kind>HG_IRQ_TIMER || (kind!=HG_IRQ_PLAIN && target>=4)) abort();
    OSIrqMask mask=kind==HG_IRQ_PLAIN ? target&((1u<<OS_IRQ_TABLE_MAX)-1) :
        1u<<(target+(kind==HG_IRQ_DMA ? REG_OS_IE_D0_SHIFT : REG_OS_IE_T0_SHIFT));
    pthread_mutex_lock(&eventMutex);
    for (unsigned bit=0;bit<OS_IRQ_TABLE_MAX;bit++) if (mask&(1u<<bit)) {
        bridges[bit]=*bridge; argumentCallback[bit]=FALSE;
        OS_IRQTable[bit]=bridge->invoke ? invokeBridge : NULL;
        int index=callbackIndex(bit);
        if (index>=0) OSi_IrqCallbackInfo[index].enable=kind!=HG_IRQ_DMA || (enabledMask&(1u<<bit));
    }
    if (kind!=HG_IRQ_PLAIN) enabledMask|=mask;
    pthread_cond_broadcast(&eventChanged); pthread_mutex_unlock(&eventMutex);
    if (kind!=HG_IRQ_PLAIN) HGNativeDispatchIrqs();
}
BOOL HGNativeGetIrqBridge(OSIrqMask mask,HGNativeIrqBridge *bridge) {
    BOOL found=FALSE;
    pthread_mutex_lock(&eventMutex);
    for (unsigned bit=0;bit<OS_IRQ_TABLE_MAX;bit++) if (mask&(1u<<bit)) {
        if (!argumentCallback[bit] && OS_IRQTable[bit]==invokeBridge) { *bridge=bridges[bit]; found=TRUE; }
        break;
    }
    pthread_mutex_unlock(&eventMutex); return found;
}
BOOL HGNativeClearIrqBridgeContext(void *context) {
    pthread_mutex_lock(&eventMutex);
    if (eventThreadSet && (!pthread_equal(eventThread,pthread_self()) || dispatching)) {
        pthread_mutex_unlock(&eventMutex); return FALSE;
    }
    for (unsigned bit=0;bit<OS_IRQ_TABLE_MAX;bit++) if (bridges[bit].context==context && bridges[bit].invoke) {
        bridges[bit]=(HGNativeIrqBridge){0}; OS_IRQTable[bit]=NULL;
        enabledMask&=~(1u<<bit); pendingMask&=~(1u<<bit);
    }
    pthread_mutex_unlock(&eventMutex); return TRUE;
}
void OS_WaitIrq(BOOL clear, OSIrqMask flags) {
    if (!flags || OS_GetCpsrIrq() == OS_INTRMODE_IRQ_DISABLE) abort();
    if (clear) OS_ClearIrqCheckFlag(flags);
    for (;;) {
        HGNativeDispatchIrqs();
        pthread_mutex_lock(&eventMutex);
        if (!eventThreadSet) abort();
        if (checkFlags & flags) { pthread_mutex_unlock(&eventMutex); return; }
        BOOL owner = pthread_equal(eventThread, pthread_self());
        while (!(checkFlags & flags) && !(owner && masterEnabled && ((pendingMask | queuedMask) & enabledMask))) pthread_cond_wait(&eventChanged, &eventMutex);
        pthread_mutex_unlock(&eventMutex);
    }
}

static u64 monotonicNanoseconds(void) {
    struct timespec value;
    if (clock_gettime(CLOCK_MONOTONIC,&value)) abort();
    return (u64)value.tv_sec*1000000000ULL+value.tv_nsec;
}
void HGNativeWaitNanoseconds(u64 duration) {
    u64 start=monotonicNanoseconds();
    for (;;) {
        HGNativeDispatchIrqs();
        pthread_mutex_lock(&eventMutex);
        u64 elapsed=monotonicNanoseconds()-start;
        if (elapsed>=duration) { pthread_mutex_unlock(&eventMutex); return; }
        BOOL owner=eventThreadSet && pthread_equal(eventThread,pthread_self());
        BOOL ready=!dispatching && owner && masterEnabled && OS_GetCpsrIrq()==OS_INTRMODE_IRQ_ENABLE &&
                   ((pendingMask|queuedMask)&enabledMask);
        if (ready) { pthread_mutex_unlock(&eventMutex); continue; }
        u64 remaining=duration-elapsed;
        struct timespec delay={(time_t)(remaining/1000000000ULL),(long)(remaining%1000000000ULL)};
        int status=pthread_cond_timedwait_relative_np(&eventChanged,&eventMutex,&delay);
        pthread_mutex_unlock(&eventMutex);
        if (status && status!=ETIMEDOUT) abort();
    }
}
