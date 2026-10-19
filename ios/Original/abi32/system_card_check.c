// SPDX-License-Identifier: GPL-3.0-or-later
#include <nitro.h>
#include "system_card.h"
#include <string.h>
#define CHECK(v) do { if (!(v)) return __LINE__; } while (0)
void HGPrepareRomArchive(u32 code,u32 maker) {
    u8 *header=OS_GetSystemWork()->rom_header;
    memset(header,0,0x160);
    memcpy(header+12,&code,4); u16 id=(u16)maker; memcpy(header+16,&id,2);
    const u32 tables[]={0x1000,0x200,0x3000,0x400}; memcpy(header+0x40,tables,sizeof(tables));
}
u32 CheckNativeRomArchive(void) {
    CHECK(FS_IsAvailable());
    FSArchive *archive=FS_FindArchive("rom",3); CHECK(archive);
    u8 saved[0x160]; memcpy(saved,OS_GetSystemWork()->rom_header,sizeof(saved));
    u32 tables[]={archive->fnt,archive->fnt_size,archive->fat,archive->fat_size};
    const u32 codes[]={0x4A4B5049,0x45475049}; // IPKJ, IPGE
    for (u32 i=0;i<2;i++) {
        HGPrepareRomArchive(codes[i],0x3130);
        SystemCard_InitRomArchive(); SystemCard_InitRomArchive();
        CHECK(archive==FS_FindArchive("rom",3));
        CHECK(archive->fnt==0x1000 && archive->fnt_size==0x200 && archive->fat==0x3000 && archive->fat_size==0x400);
        CHECK(!memcmp(OS_GetSystemWork()->rom_header+12,&codes[i],4));
    }
    memcpy(OS_GetSystemWork()->rom_header,saved,sizeof(saved));
    archive->fnt=tables[0]; archive->fnt_size=tables[1]; archive->fat=tables[2]; archive->fat_size=tables[3];
    return 0;
}
