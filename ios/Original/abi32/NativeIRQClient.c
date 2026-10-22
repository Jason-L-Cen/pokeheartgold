// SPDX-License-Identifier: GPL-3.0-or-later
#include <nitro/os/interrupt.h>
#include <nitro/os/native_irq.h>
#include <nitro/cp/context.h>
#include <nitro/os/system.h>
#include <nitro/hw/native_graphics.h>
#include "assert.h"

static _Alignas(16) u8 irqStack[HG_IRQ_STACK_SIZE+32];
static void guardedCallback(u32 function,u32 first,u32 second,u32 arity) {
    CPContext saved;
    CP_SaveContext(&saved);
    if (!arity) ((void (*)(void))(uaddr)function)();
    else if (arity==1) ((void (*)(void *))(uaddr)function)((void *)(uaddr)first);
    else ((void (*)(u32,void *))(uaddr)function)(first,(void *)(uaddr)second);
    CPi_RestoreContext(&saved);
}
void HGNativeSetIrqStackChecker(void) {
    GF_ASSERT(OS_GetProcMode()==OS_PROCMODE_SYS);
    for (u32 i=0;i<16;i++) irqStack[i]=irqStack[sizeof(irqStack)-16+i]=0xA5;
}
void HGNativeInitIrqClient(u32 dummy) {
    HGNativeSetIrqStackChecker();
    const u32 configuration[]={dummy,(u32)(uaddr)guardedCallback,(u32)(irqStack+16),HG_IRQ_STACK_SIZE};
    HGNativeIrqInit((u32)configuration);
    HGNativeIrqSet(HG_IRQ_RASTER,0,(u32)(uaddr)HGNativeRasterStep,0);
}
BOOL OS_EnableIrq(void) { return HGNativeIrqControl(HG_IRQ_ENABLE,0); }
BOOL OS_DisableIrq(void) { return HGNativeIrqControl(HG_IRQ_DISABLE,0); }
BOOL OS_RestoreIrq(BOOL enabled) { return HGNativeIrqControl(HG_IRQ_RESTORE,enabled); }
OSIrqMask OS_GetIrqMask(void) { return HGNativeIrqControl(HG_IRQ_GET_MASK,0); }
OSIrqMask OS_GetRequestIrqMask(void) { return HGNativeIrqControl(HG_IRQ_GET_REQUEST,0); }
u32 OS_GetIrqCheckFlag(void) { return HGNativeIrqControl(HG_IRQ_GET_CHECK,0); }
void OS_SetIrqCheckFlag(OSIrqMask flags) { HGNativeIrqControl(HG_IRQ_SET_CHECK,flags); }
void OS_ClearIrqCheckFlag(OSIrqMask flags) { HGNativeIrqControl(HG_IRQ_CLEAR_CHECK,flags); }
u32 OS_GetVBlankCount(void) { return HGNativeIrqControl(HG_IRQ_GET_VBLANK,0); }
