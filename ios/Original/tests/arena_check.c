// SPDX-License-Identifier: GPL-3.0-or-later
#include <nitro/os/arena.h>
#include <nitro/os/system.h>
#define CHECK(v) do { if (!(v)) return __LINE__; } while (0)

u32 CheckNativeArenaBounds(void) {
    OSArenaId id=OS_ARENA_DTCM;
    OS_InitArena();
    u8 *base=OS_GetInitArenaLo(id),*end=OS_GetInitArenaHi(id);
    void *savedLow=OS_GetArenaLo(id),*savedHigh=OS_GetArenaHi(id);
    CHECK(base && end);
    base=(u8 *)(((uaddr)base+31)&~(uaddr)31); end=(u8 *)((uaddr)end&~(uaddr)31);
    CHECK((uaddr)end-(uaddr)base>=128);
    OS_SetArenaHi(id,end); OS_SetArenaLo(id,base);
    CHECK(OS_AllocFromArenaLo(id,13,16)==base && OS_GetArenaLo(id)==base+16);
    CHECK(OS_AllocFromArenaHi(id,13,16)==end-16 && OS_GetArenaHi(id)==end-16);
    void *low=OS_GetArenaLo(id),*high=OS_GetArenaHi(id);
    CHECK(!OS_AllocFromArenaLo(id,UINT32_MAX,4) && !OS_AllocFromArenaHi(id,UINT32_MAX,4));
    CHECK(!OS_AllocFromArenaLo(id,1,0) && !OS_AllocFromArenaHi(id,1,3));
    CHECK(OS_GetArenaLo(id)==low && OS_GetArenaHi(id)==high);
    OS_SetArenaHi(id,end); OS_SetArenaLo(id,base);
    OS_SetArenaHi(id,base+31); OS_SetArenaLo(id,base+5);
    CHECK(!OS_AllocFromArenaLo(id,23,8));
    CHECK(OS_GetArenaLo(id)==base+5 && OS_GetArenaHi(id)==base+31);
    CHECK(OS_AllocFromArenaHi(id,1,8)==base+16);
    CHECK(OS_AllocFromArenaLo(id,8,8)==base+8);
    CHECK(OS_GetArenaLo(id)==base+16 && OS_GetArenaHi(id)==base+16);
    CHECK(!OS_AllocFromArenaHi(id,1,8) && !OS_AllocFromArenaLo(id,1,8));
    CHECK(OS_AllocFromArenaLo(id,0,8)==base+16 && OS_AllocFromArenaHi(id,0,8)==base+16);
    OS_SetArenaHi(id,end); OS_SetArenaLo(id,base);
    CHECK(OS_AllocFromArenaLo(id,(u32)(end-base),32)==base);
    OS_InitArena(); CHECK(OS_GetArenaLo(id)==end);
    OS_SetArenaLo(id,NULL); OS_SetArenaHi(id,NULL);
    CHECK(!OS_AllocFromArenaLo(id,1,4) && !OS_AllocFromArenaHi(id,1,4));
    OS_SetArenaHi(id,end); OS_SetArenaLo(id,base);
#ifdef HEARTGOLD_ABI32
    OS_SetArenaHi(id,base+9); OS_SetArenaLo(id,base+1);
    CHECK(OS_AllocFromArenaLo(id,1,1)==base+1 && OS_AllocFromArenaHi(id,1,1)==base+8);
    OS_SetArenaHi(id,end); OS_SetArenaLo(id,base);
#endif
    OS_SetArenaHi(id,savedHigh); OS_SetArenaLo(id,savedLow);
    CHECK(!OS_GetCpsrIrq());
    return 0;
}
