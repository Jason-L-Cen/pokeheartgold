// SPDX-License-Identifier: GPL-3.0-or-later
#define _POSIX_C_SOURCE 200809L
#include <nitro/gx/native_display.h>
#include <nitro/hw/consts_shared.h>
#include <nitro/os/system.h>
#include <time.h>
#include "assert.h"

static u64 now(void) {
    struct timespec value; int result=clock_gettime(CLOCK_MONOTONIC,&value);
    GF_ASSERT(!result && value.tv_sec>=0 && value.tv_nsec>=0 && value.tv_nsec<1000000000);
    GF_ASSERT((u64)value.tv_sec<=(UINT64_MAX-(u64)value.tv_nsec)/1000000000ULL);
    return (u64)value.tv_sec*1000000000ULL+(u64)value.tv_nsec;
}
static u64 clocksAt(u64 nanos) {
    u64 clocks=(nanos/1000000000ULL)*HW_SYSTEM_CLOCK+((nanos%1000000000ULL)*HW_SYSTEM_CLOCK)/1000000000ULL;
    return clocks;
}
static u32 phase(u64 nanos) { return (u32)(clocksAt(nanos)%HG_DISPLAY_FRAME_CLOCKS); }
u64 HGNativeDisplayVBlanksAt(u64 nanos) {
    u64 clocks=clocksAt(nanos),first=192*HG_DISPLAY_LINE_CLOCKS;
    return clocks<first ? 0 : (clocks-first)/HG_DISPLAY_FRAME_CLOCKS+1;
}
u32 HGNativeDisplayLineAt(u64 nanos) { return phase(nanos)/HG_DISPLAY_LINE_CLOCKS; }
u32 HGNativeDisplayLine(void) { return HGNativeDisplayLineAt(now()); }
u64 HGNativeWaitDisplayLine0(void) {
    for (;;) {
        u64 stamp=now(); u32 current=phase(stamp);
        if (current<HG_DISPLAY_LINE_CLOCKS) return stamp;
        u32 remaining=HG_DISPLAY_FRAME_CLOCKS-current,guard=HW_SYSTEM_CLOCK/1000+1;
        // shortcut: poll the last millisecond; replace with the renderer's frame-boundary event when available.
        if (remaining>guard) OS_SpinWait((remaining-guard)*2);
    }
}
