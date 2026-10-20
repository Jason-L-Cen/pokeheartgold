// SPDX-License-Identifier: GPL-3.0-or-later
#include "timer_3.h"
#define CHECK(v) do { if (!(v)) return __LINE__; } while (0)
u64 HGTimer3Action(u32 action,u64 value) {
    if (!action) { Init_Timer3(); return 0; }
    return action==1 ? GetTimer3Ticks() : Timer3TicksToSeconds(value);
}
u32 CheckNativeGameTimer(void) {
    OSIrqMask mask=OS_GetIrqMask(); OSIntrMode previous=OS_DisableInterrupts();
    Init_Timer3(); u64 first=Timer3_GetTicks(); OS_SpinWait(100000);
    u64 second=GetTimer3Ticks(); CHECK(second>first);
    u64 saved=OS_GetTick(); OS_SetTick(0x8000000000000000ULL);
    u64 third=GetTimer3Ticks(); CHECK(third>=second && third-second<OS_SYSTEM_CLOCK);
    OS_SetTick(saved);
    CHECK(OS_GetIrqMask()==mask && OS_GetCpsrIrq()==OS_INTRMODE_DISABLE_IRQ);
    Init_Timer3(); CHECK(GetTimer3Ticks()<OS_SYSTEM_CLOCK/64);
    CHECK(!Timer3TicksToSeconds(0) && Timer3TicksToSeconds(OS_SYSTEM_CLOCK)==64);
    CHECK(!Timer3TicksToSeconds(OS_SYSTEM_CLOCK/64) && Timer3TicksToSeconds((OS_SYSTEM_CLOCK+63)/64)==1);
    OS_RestoreInterrupts(previous); return 0;
}
