// SPDX-License-Identifier: GPL-3.0-or-later
#include <nitro.h>
#include <nitro/gx/native_display.h>
#include <nitro/hw/consts_shared.h>
#include <nitro/hw/native_graphics.h>
#define CHECK(v) do { if (!(v)) return __LINE__; } while (0)
void _start_AutoloadDoneCallback(void *[]);
u32 CheckOriginalDisplayClock(void) {
    CHECK(HGNativeDisplayLineAt(0)==0 && HGNativeDisplayLineAt(1000000000ULL)==217);
    CHECK(HGNativeDisplayLineAt(UINT64_MAX)==203);
    for (u32 line=1;line<HG_DISPLAY_LINES;line++) {
        u64 stamp=((u64)line*HG_DISPLAY_LINE_CLOCKS*1000000000ULL+HW_SYSTEM_CLOCK-1)/HW_SYSTEM_CLOCK;
        CHECK(HGNativeDisplayLineAt(stamp)==line && HGNativeDisplayLineAt(stamp-1)==line-1);
    }
    u64 frame=((u64)HG_DISPLAY_FRAME_CLOCKS*1000000000ULL+HW_SYSTEM_CLOCK-1)/HW_SYSTEM_CLOCK;
    CHECK(HGNativeDisplayLineAt(frame)==0 && HGNativeDisplayLineAt(frame-1)==262);
    reg_GX_VCOUNT=0xFFFF; CHECK(GX_GetVCount()<HG_DISPLAY_LINES);
    u32 packed=*(vu32 *)(uaddr)HGNativeRegisterAddress(0x04000004,4); CHECK((packed>>16)<HG_DISPLAY_LINES);
    OS_InitIrqTable(); OSIntrMode saved=OS_DisableInterrupts();
    for (u32 enabled=0;enabled<2;enabled++) {
        OS_RestoreIrq(enabled); OSi_WaitVCount0();
        CHECK(OS_RestoreIrq(FALSE)==enabled && OS_GetCpsrIrq()==OS_INTRMODE_IRQ_DISABLE);
    }
    CHECK(HGNativeDisplayLineAt(HGNativeWaitDisplayLine0())==0);
    OS_RestoreInterrupts(saved);
    void *arguments[]={&frame,&packed}; _start_AutoloadDoneCallback(arguments);
    CHECK(arguments[0]==&frame && arguments[1]==&packed); return 0;
}
