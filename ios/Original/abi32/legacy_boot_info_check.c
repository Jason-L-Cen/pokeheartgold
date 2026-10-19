// SPDX-License-Identifier: GPL-3.0-or-later
#include <MB_mb.h>
#include <CARD_rom.h>
#include <stddef.h>
_Static_assert(sizeof(MBParam)==62 && offsetof(MBParam,parent_bss_desc)==2,"Original legacy download-boot view");
u32 HGLegacyBootInfo(u32 field) {
    if (!field) return (u32)(uaddr)MB_GetMultiBootParam();
    if (field==1) return MB_GetMultiBootParam()->boot_type;
    if (field==2) return MB_IsMultiBootChild();
    return MB_GetMultiBootParam()->parent_bss_desc.cfpMaxDuration;
}
u32 HGLegacyRomRegion(u32 table,u32 field) {
    const CARDRomRegion *view=table==0 ? CARD_GetRomRegionFNT() : table==1 ? CARD_GetRomRegionFAT() :
        CARD_GetRomRegionOVT(table==2 ? MI_PROCESSOR_ARM9 : MI_PROCESSOR_ARM7);
    return field ? view->length : view->offset;
}
