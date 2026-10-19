// SPDX-License-Identifier: GPL-3.0-or-later
#include <nitro.h>
#include <stddef.h>
#define CHECK(v) do { if (!(v)) return __LINE__; } while (0)
extern u32 HGLegacyBootInfo(u32 field);
extern u32 HGLegacyRomRegion(u32 table,u32 field);
_Static_assert(sizeof(MBParam)==62 && offsetof(MBParam,parent_bss_desc)==2,"Original download-boot view");
_Static_assert(offsetof(OSSystemWork,wmBootBuf)==0x40,"Original download-boot storage");
_Static_assert(offsetof(OSSystemWork,rom_header)==0x200 && sizeof(((OSSystemWork *)0)->rom_header)==0x160,"Original ROM header storage");
u32 CheckNativeBootInfo(void) {
    u8 *bytes=OS_GetSystemWork()->wmBootBuf;
    CHECK((const void *)MB_GetMultiBootParam()==(void *)bytes);
    CHECK(HGLegacyBootInfo(0)==(u32)(uaddr)bytes && HGLegacyBootInfo(1)==MB_TYPE_NORMAL);
    CHECK(MB_GetMultiBootParam()->boot_type==MB_TYPE_NORMAL && !MB_IsMultiBootChild() && !HGLegacyBootInfo(2));
    bytes[0]=MB_TYPE_MULTIBOOT;
    CHECK(MB_IsMultiBootChild() && HGLegacyBootInfo(2));
    bytes[0]=MB_TYPE_ILLEGAL;
    CHECK(!MB_IsMultiBootChild() && !HGLegacyBootInfo(2));
    bytes[0]=MB_TYPE_NORMAL;
    bytes[60]=0x34; bytes[61]=0x12;
    CHECK(MB_GetMultiBootParam()->parent_bss_desc.cfpMaxDuration==0x1234 && HGLegacyBootInfo(3)==0x1234);
    bytes[60]=bytes[61]=0;
    u32 saved[8],regions[8]={0x1000,0x200,0x3000,0x400,0x5000,0x600,0x7000,0x800};
    u8 *rom=OS_GetSystemWork()->rom_header;
    MI_CpuCopy32(rom+0x40,saved,sizeof(saved)); MI_CpuCopy32(regions,rom+0x40,sizeof(regions));
    const CARDRomRegion *views[]={CARD_GetRomRegionFNT(),CARD_GetRomRegionFAT(),
        CARD_GetRomRegionOVT(MI_PROCESSOR_ARM9),CARD_GetRomRegionOVT(MI_PROCESSOR_ARM7)};
    for (u32 i=0;i<4;i++) {
        CHECK((const void *)views[i]==rom+0x40+i*8 && views[i]->offset==regions[i*2] && views[i]->length==regions[i*2+1]);
        CHECK(HGLegacyRomRegion(i,0)==regions[i*2] && HGLegacyRomRegion(i,1)==regions[i*2+1]);
    }
    MI_CpuCopy32(saved,rom+0x40,sizeof(saved));
    return 0;
}
