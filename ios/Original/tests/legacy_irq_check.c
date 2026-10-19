// SPDX-License-Identifier: GPL-3.0-or-later
// Include the legacy SDK header first; it must share the native state used by SDK functions.
#include "nitro/OS_interrupt_shared.h"
#include "OS_interrupt.h"
#include <nitro/os/system.h>
#define CHECK(v) do { if (!(v)) return __LINE__; } while (0)

u32 HGLegacyIRQAction(u32 operation,u32 value) {
    switch (operation) {
    case 0: return OS_DisableIrq();
    case 1: return OS_EnableIrq();
    case 2: return OS_RestoreIrq(value);
    case 3: return OS_GetIrqMask();
    case 4: return OS_SetIrqMask(value);
    }
    return ~0u;
}
u32 CheckLegacyIRQState(void) {
    OS_InitIrqTable(); OS_DisableIrq();
    OSIrqMask mask=OS_IE_TIMER0|OS_IE_DMA1;
    CHECK(OS_SetIrqMask(mask)==0 && OS_GetIrqMask()==mask);
    CHECK(OS_DisableIrqMask(OS_IE_DMA1)==mask && OS_GetIrqMask()==OS_IE_TIMER0);
    CHECK(OS_EnableIrqMask(OS_IE_HBLANK)==OS_IE_TIMER0);
    CHECK(OS_GetIrqMask()==(OS_IE_TIMER0|OS_IE_HBLANK));
    CHECK(!OS_EnableIrq() && OS_DisableIrq());
    CHECK(!OS_RestoreIrq(TRUE) && OS_RestoreIrq(FALSE));
    CHECK(!OS_GetCpsrIrq() && OS_GetProcMode()==OS_PROCMODE_SYS);
    OS_SetIrqMask(0); OS_InitIrqTable();
    return 0;
}
