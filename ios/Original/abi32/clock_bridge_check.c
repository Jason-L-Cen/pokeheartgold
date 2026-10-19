// SPDX-License-Identifier: GPL-3.0-or-later
#include <nitro/os/tick.h>
#include <nitro/os/system.h>
#include <nitro/os/common/entropy.h>
#include <string.h>
#define CHECK(v) do { if (!(v)) return __LINE__; } while (0)
static struct { u32 before,words[8],after; } entropy;
u64 HGTickRead(u32 kind) {
    if (kind==0) return OS_GetTick();
    if (kind==1) return OS_GetTickLo();
    if (kind==3) return OS_IsTickAvailable();
    OSIntrMode old=OS_DisableInterrupts(); u64 counter=OSi_TickCounter; OS_RestoreInterrupts(old); return counter;
}
void HGTickSet(u64 value) { OS_SetTick(value); }
void HGTickInit(void) { OS_InitTick(); }
u32 HGEntropyAddress(void) { return (u32)(uaddr)entropy.words; }
u32 CheckOriginalClock(void) {
    CHECK(!OS_IsTickAvailable()); OS_InitTick(); CHECK(OS_IsTickAvailable());
    OS_SetTick(0x123456780000ULL);
    u64 before=OS_GetTick(); CHECK(before>=0x123456780000ULL && OSi_TickCounter==before>>16);
    OS_InitTick(); CHECK(OS_GetTick()>=before);
    memset(&entropy,0xA5,sizeof(entropy)); entropy.before=0x12345678; entropy.after=0x87654321;
    u32 unchanged[8]; memcpy(unchanged,entropy.words,sizeof(unchanged));
    OS_GetLowEntropyData(entropy.words);
    CHECK(entropy.before==0x12345678 && entropy.after==0x87654321);
    CHECK(memcmp(unchanged,entropy.words,sizeof(unchanged))!=0);
    return 0;
}
