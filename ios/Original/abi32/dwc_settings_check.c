// SPDX-License-Identifier: GPL-3.0-or-later
#include <dwc/dwc_bm_internal.h>
#include <dwc/native_settings.h>
#define CHECK(v) do { if (!(v)) return __LINE__; } while (0)
_Static_assert(sizeof(DwcWiFiInfo)==20 && _Alignof(DwcWiFiInfo)==4,"Original Wi-Fi identity layout");
_Static_assert(sizeof(DwcBackupWork)==0x700 && sizeof(DwcBackupPage)==256,"Original settings layout");
static DwcBackupWork work;
u64 HGDwcIdentity(void) {
    DwcWiFiInfo info={0}; DWCi_BM_GetWiFiInfo(&info); return info.pseudoUserId;
}
s32 HGDwcInitialize(void) { return DWC_BM_Init(&work); }
u32 CheckNativeDwcSettings(void) {
    u64 local=HGNativeDwcIdentity(); CHECK(local && local<(1ULL<<43));
    CHECK(!DWC_BM_Init(&work) && HGDwcIdentity()==local);
    DwcWiFiInfo info={0}; DWCi_BM_GetWiFiInfo(&info);
    CHECK(!info.userId && info.random<1000 && !DWC_Auth_CheckPseudoWiFiID());
    u32 id[5]; DWC_Auth_GetId(id);
    CHECK(!id[0] && !id[1] && id[2]==(u32)local && id[3]==(u32)(local>>32) && !id[4]);
    CHECK(DWCi_BACKUPlRead(&work));
    for (u32 i=0;i<4;i++) {
        CHECK(work.pages[i].crc==MATH_CalcCRC16(&work.crcTable,&work.pages[i],254));
        if (i<3) CHECK(work.pages[i].state==BACKUP_AP_UNUSED);
    }
    CHECK(!memcmp(work.pages[0].wifiInfo,work.pages[1].wifiInfo,14));
    DwcWiFiInfo maximum={(1ULL<<43)-1,(1ULL<<43)-2,999,65535},decoded={0};
    DWCi_BACKUPlConvWifiInfo(&maximum); DWCi_BM_GetWiFiInfo(&decoded);
    CHECK(!memcmp(&maximum,&decoded,sizeof(maximum)));
    CHECK(!DWC_BM_Init(&work) && HGDwcIdentity()==local);
    u8 corrupt=0xA5;
    CHECK(HGNativeDwcSettings(0,&corrupt,1,TRUE));
    CHECK(!DWC_BM_Init(&work) && HGDwcIdentity()==local);
    u8 sentinel=0xA5;
    CHECK(!readNvram(HG_DWC_SETTINGS_BASE+1024,1,&sentinel) && sentinel==0xA5);
    CHECK(!readNvram(0,1,&sentinel) && sentinel==0xA5);
    CHECK(!NVRAMm_ExecuteCommand(99,0,0,NULL));
    CHECK(!NVRAMm_ExecuteCommand(2,0x20,1,&sentinel));
    CHECK(writeDisable());
    return 0;
}
