// SPDX-License-Identifier: GPL-3.0-or-later
#include <CTRDG_init.h>
#include <CTRDG_common.h>
#include <nitro/hw/native_graphics.h>
#include <OS_arena.h>
#include <OS_tcm.h>
#include <mmap.h>
#include <nitro/OS_systemWork_shared.h>
#include <stddef.h>
#include <string.h>
#define CHECK(v) do { if (!(v)) return __LINE__; } while (0)
static BOOL pulledOut(void) { return FALSE; }
_Static_assert(sizeof(OSSystemWork)==0x400,"Original legacy system work layout");
_Static_assert(offsetof(OSSystemWork,pxiHandleChecker)==0x388,"Original legacy PXI flags");
u32 HGLegacySystemWorkAddress(void) { return (u32)(uaddr)OS_GetSystemWork(); }
u32 HGStartupData(u32 field) { return field ? (u32)&gCtrdgInitWork : OS_GetDTCMAddress(); }
u32 CheckOriginalStartupData(void) {
    u8 *base=OS_GetInitArenaLo(OS_ARENA_DTCM),*end=OS_GetInitArenaHi(OS_ARENA_DTCM);
    void *low=OS_GetArenaLo(OS_ARENA_DTCM),*high=OS_GetArenaHi(OS_ARENA_DTCM);
    CHECK((u32)base==OS_GetDTCMAddress() && base==(u8 *)SDK_AUTOLOAD_DTCM_START);
    CHECK(!((u32)base&4095) && end-base==HW_DTCM_SIZE);
    OS_SetArenaLo(OS_ARENA_DTCM,base); OS_SetArenaHi(OS_ARENA_DTCM,end);
    CHECK(OS_AllocFromArenaLo(OS_ARENA_DTCM,128,32)==base);
    CHECK(OS_GetDTCMAddress()==(u32)base);
    u8 first=base[0],last=end[-1]; base[0]=0x5A; end[-1]=0xA5;
    CHECK(((u8 *)(uaddr)OS_GetDTCMAddress())[0]==0x5A && ((u8 *)SDK_AUTOLOAD_DTCM_START)[HW_DTCM_SIZE-1]==0xA5);
    base[0]=first; end[-1]=last;
    OS_SetArenaLo(OS_ARENA_DTCM,base); OS_SetArenaHi(OS_ARENA_DTCM,high); OS_SetArenaLo(OS_ARENA_DTCM,low);
    CHECK(sizeof(gCtrdgInitWork)==28 && offsetof(CTRDGiInitWork,settingPhi)==4);
    CHECK(offsetof(CTRDGiInitWork,initialized)==8 && offsetof(CTRDGiInitWork,pulledOut)==12 && offsetof(CTRDGiInitWork,pulledOutCallback)==24);
    CTRDGiInitWork saved=gCtrdgInitWork;
    const CTRDGiInitWork initial={TRUE,TRUE,TRUE,FALSE,0x12345678,0x87654321,pulledOut};
    gCtrdgInitWork=initial;
    CTRDGi_CallbackForSetPhi(PXI_FIFO_TAG_CTRDG,0,FALSE);
    CTRDGiInitWork expected=initial; expected.settingPhi=FALSE;
    CHECK(!memcmp(&gCtrdgInitWork,&expected,sizeof(expected)));
    gCtrdgInitWork=saved; return 0;
}
u32 CheckNativeCartridgeEnable(void) {
    BOOL enabled=CTRDG_IsEnabled();
    OSIntrMode previous=OS_DisableInterrupts();
    CTRDG_Enable(TRUE); CHECK(CTRDG_IsEnabled() && OS_GetCpsrIrq()==OS_INTRMODE_DISABLE_IRQ);
    u32 size=4; CHECK(!HGNativeMemorySpan(0x08000000,&size));
    CTRDG_Enable(FALSE); CHECK(!CTRDG_IsEnabled() && OS_GetCpsrIrq()==OS_INTRMODE_DISABLE_IRQ);
    OS_RestoreInterrupts(previous);
    CTRDG_Enable(enabled); CHECK(OS_GetCpsrIrq()==previous);
    return 0;
}
