// SPDX-License-Identifier: GPL-3.0-or-later
#include "../../../src/application/pokewalker/pokewalker_event_log.c"

u32 HGAotEventLogSort(void) {
    const u32 values[]={0xFFFFFFFF,0,0x80000000,7};
    const u32 order[]={1,3,2,0};
    PokewalkerIrRecord records[4]={0};
    PokewalkerEventLogSlot slots[4]={0};
    for (u32 i=0;i<4;i++) { records[i].value=values[i]; slots[i].record=&records[i]; slots[i].index=i+17; }
    MATH_QSort(slots,4,sizeof(slots[0]),PokewalkerEventLog_CompareEntryTimes,NULL);
    for (u32 i=0;i<4;i++) {
        if (slots[i].record!=&records[order[i]] || slots[i].index!=order[i]+17 || records[i].value!=values[i]) return FALSE;
    }
    return TRUE;
}
