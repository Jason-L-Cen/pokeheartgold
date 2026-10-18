// SPDX-License-Identifier: GPL-3.0-or-later
#include "NativeEvents.h"
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

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
static BOOL argumentCallback[OS_IRQ_TABLE_MAX];
void OS_IrqDummy(void) {}

static int callbackIndex(unsigned bit) {
    if (bit >= REG_OS_IE_D0_SHIFT && bit <= REG_OS_IE_D3_SHIFT) return (int)bit - REG_OS_IE_D0_SHIFT;
    if (bit >= REG_OS_IE_T0_SHIFT && bit <= REG_OS_IE_T3_SHIFT) return (int)bit - REG_OS_IE_T0_SHIFT + OSi_IRQCALLBACK_NO_TIMER0;
    return -1;
}
void OS_InitIrqTable(void) {
    pthread_mutex_lock(&eventMutex);
    memset(OS_IRQTable, 0, sizeof(OS_IRQTable)); memset(OSi_IrqCallbackInfo, 0, sizeof(OSi_IrqCallbackInfo));
    memset(argumentCallback, 0, sizeof(argumentCallback));
    for (unsigned bit = 0; bit < OS_IRQ_TABLE_MAX; bit++) if (callbackIndex(bit) < 0) OS_IRQTable[bit] = OS_IrqDummy;
    enabledMask = pendingMask = checkFlags = 0; masterEnabled = FALSE; vblankCount = 0;
    eventThread = pthread_self(); eventThreadSet = TRUE;
    pthread_mutex_unlock(&eventMutex);
}
void OS_SetIrqFunction(OSIrqMask mask, OSIrqFunction function) {
    pthread_mutex_lock(&eventMutex);
    for (unsigned bit = 0; bit < OS_IRQ_TABLE_MAX; bit++) if (mask & (1u << bit)) {
        OS_IRQTable[bit] = function; argumentCallback[bit] = FALSE;
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
    pthread_mutex_lock(&eventMutex); OSIrqMask value = pendingMask; pthread_mutex_unlock(&eventMutex); return value;
}
OSIrqMask OS_ResetRequestIrqMask(OSIrqMask mask) {
    pthread_mutex_lock(&eventMutex); OSIrqMask previous = pendingMask; pendingMask &= ~mask;
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
    for (;;) {
        pthread_mutex_lock(&eventMutex);
        OSIrqMask ready = masterEnabled ? pendingMask & enabledMask : 0;
        if (!ready) { pthread_mutex_unlock(&eventMutex); break; }
        unsigned bit = (unsigned)__builtin_ctz(ready); OSIrqMask mask = 1u << bit;
        pendingMask &= ~mask;
        OSIrqFunction plain = OS_IRQTable[bit];
        int index = callbackIndex(bit); BOOL hasArgument = argumentCallback[bit];
        if (index >= 0) OS_IRQTable[bit] = NULL;
        OSIrqCallbackInfo info = {0};
        if (hasArgument) { info = OSi_IrqCallbackInfo[index]; OSi_IrqCallbackInfo[index].func = NULL; }
        pthread_mutex_unlock(&eventMutex);
        OSIntrMode previousMask = OS_DisableInterrupts();
        OSProcMode previousMode = HGNativeSetExecutionMode(OS_PROCMODE_IRQ);
        if (hasArgument) { if (info.func) info.func(info.arg); }
        else if (plain) plain();
        HGNativeSetExecutionMode(previousMode); OS_RestoreInterrupts(previousMask);
        if (index >= 0) {
            OS_SetIrqCheckFlag(mask);
            pthread_mutex_lock(&eventMutex);
            if (!OSi_IrqCallbackInfo[index].enable) enabledMask &= ~mask;
            pthread_mutex_unlock(&eventMutex);
        }
    }
    dispatching = FALSE;
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
        while (!(checkFlags & flags) && !(owner && masterEnabled && (pendingMask & enabledMask))) pthread_cond_wait(&eventChanged, &eventMutex);
        pthread_mutex_unlock(&eventMutex);
    }
}
