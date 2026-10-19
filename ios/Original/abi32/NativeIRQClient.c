// SPDX-License-Identifier: GPL-3.0-or-later
#include <nitro/os/interrupt.h>
#include <nitro/os/native_irq.h>
BOOL OS_EnableIrq(void) { return HGNativeIrqControl(HG_IRQ_ENABLE,0); }
BOOL OS_DisableIrq(void) { return HGNativeIrqControl(HG_IRQ_DISABLE,0); }
BOOL OS_RestoreIrq(BOOL enabled) { return HGNativeIrqControl(HG_IRQ_RESTORE,enabled); }
OSIrqMask OS_GetIrqMask(void) { return HGNativeIrqControl(HG_IRQ_GET_MASK,0); }
OSIrqMask OS_GetRequestIrqMask(void) { return HGNativeIrqControl(HG_IRQ_GET_REQUEST,0); }
u32 OS_GetIrqCheckFlag(void) { return HGNativeIrqControl(HG_IRQ_GET_CHECK,0); }
void OS_SetIrqCheckFlag(OSIrqMask flags) { HGNativeIrqControl(HG_IRQ_SET_CHECK,flags); }
void OS_ClearIrqCheckFlag(OSIrqMask flags) { HGNativeIrqControl(HG_IRQ_CLEAR_CHECK,flags); }
u32 OS_GetVBlankCount(void) { return HGNativeIrqControl(HG_IRQ_GET_VBLANK,0); }
