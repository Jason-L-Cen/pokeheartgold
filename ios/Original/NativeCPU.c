// SPDX-License-Identifier: GPL-3.0-or-later
#include <nitro/os/system.h>
#include <nitro/hw/consts_shared.h>
#include <pthread.h>
#include "NativeEvents.h"

static pthread_mutex_t interruptMutex = PTHREAD_MUTEX_INITIALIZER;
static _Thread_local unsigned interruptState;
static _Thread_local OSProcMode executionMode = OS_PROCMODE_SYS;

static OSIntrMode setInterruptBits(unsigned bits,unsigned value) {
    unsigned previous=interruptState,next=(previous&~bits)|(value&bits);
    if (!previous && next) pthread_mutex_lock(&interruptMutex);
    interruptState=next;
    if (previous && !next) pthread_mutex_unlock(&interruptMutex);
    return (OSIntrMode)(previous&bits);
}
OSIntrMode HGNativeSetInterruptState(OSIntrMode value) {
    return setInterruptBits(OS_INTRMODE_DISABLE_IRQ|OS_INTRMODE_DISABLE_FIQ,value);
}
OSIntrMode HGNativeGetInterruptState(void) { return (OSIntrMode)interruptState; }
OSIntrMode OS_DisableInterrupts(void) { return setInterruptBits(OS_INTRMODE_DISABLE_IRQ,OS_INTRMODE_DISABLE_IRQ); }
OSIntrMode OS_EnableInterrupts(void) {
    OSIntrMode previous=setInterruptBits(OS_INTRMODE_DISABLE_IRQ,0); HGNativeDispatchIrqs(); return previous;
}
OSIntrMode OS_RestoreInterrupts(OSIntrMode mode) {
    OSIntrMode previous=setInterruptBits(OS_INTRMODE_DISABLE_IRQ,mode); HGNativeDispatchIrqs(); return previous;
}
OSIntrMode OS_DisableInterrupts_IrqAndFiq(void) {
    return setInterruptBits(OS_INTRMODE_DISABLE_IRQ|OS_INTRMODE_DISABLE_FIQ,OS_INTRMODE_DISABLE_IRQ|OS_INTRMODE_DISABLE_FIQ);
}
OSIntrMode OS_RestoreInterrupts_IrqAndFiq(OSIntrMode mode) {
    OSIntrMode previous=setInterruptBits(OS_INTRMODE_DISABLE_IRQ|OS_INTRMODE_DISABLE_FIQ,mode); HGNativeDispatchIrqs(); return previous;
}
OSIntrMode_Irq OS_GetCpsrIrq(void) { return (OSIntrMode_Irq)(interruptState&OS_INTRMODE_DISABLE_IRQ); }
OSProcMode OS_GetProcMode(void) { return executionMode; }
OSProcMode HGNativeSetExecutionMode(OSProcMode mode) {
    OSProcMode previous=executionMode; executionMode=mode; return previous;
}
void OS_SpinWait(u32 cycles) {
    // The ARM9 subtract/branch loop always executes once, including when cycles is zero.
    u64 clock=(u64)HW_SYSTEM_CLOCK*2,steps=(u64)cycles/4+1;
    // shortcut: minimum DS-rate delay; calibrate short-wait overhead against real-device frame timing.
    HGNativeWaitNanoseconds((steps*4*1000000000ULL+clock-1)/clock);
}
