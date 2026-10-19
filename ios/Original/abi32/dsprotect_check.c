// SPDX-License-Identifier: GPL-3.0-or-later
#include "poke_overlay.h"
#include <dsprot.h>
#define CHECK(v) do { if (!(v)) return __LINE__; } while (0)
static u32 calls,failed,expectedMode,expectedIrq;
static void observed(void) {
    calls++;
    if (OS_GetProcMode()!=expectedMode || OS_GetCpsrIrq()!=expectedIrq || DSProt_DetectNotDummy(NULL)!=1) failed=1;
}
u32 CheckNativeDSProtect(void) {
    u32 (*rules[])(void *)={DSProt_DetectFlashcart,DSProt_DetectNotFlashcart,DSProt_DetectEmulator,
        DSProt_DetectNotEmulator,DSProt_DetectDummy,DSProt_DetectNotDummy};
    for (u32 cycle=0;cycle<2;cycle++) {
        CHECK(HandleLoadOverlay(FS_OVERLAY_ID(ds_protect),cycle ? OVY_LOAD_ASYNC : OVY_LOAD_NORMAL));
        for (u32 masked=0;masked<2;masked++) {
            OSIntrMode previous=masked ? OS_DisableInterrupts() : OS_GetCpsrIrq();
            expectedMode=OS_GetProcMode(); expectedIrq=OS_GetCpsrIrq();
            for (u32 i=0;i<6;i++) {
                calls=failed=0;
                CHECK(rules[i](observed)==(i&1) && calls==(i&1) && !failed);
                CHECK(rules[i](NULL)==(i&1) && calls==(i&1));
                if (!(i&1)) CHECK(!rules[i]((void *)(uaddr)0xFFFFFFFF) && !calls);
                CHECK(OS_GetProcMode()==expectedMode && OS_GetCpsrIrq()==expectedIrq);
            }
            if (masked) OS_RestoreInterrupts(previous);
        }
        UnloadOverlayByID(FS_OVERLAY_ID(ds_protect));
        CHECK(!FS_UnloadOverlay(MI_PROCESSOR_ARM9,FS_OVERLAY_ID(ds_protect)));
    }
    return 0;
}
