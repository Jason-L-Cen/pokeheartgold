// SPDX-License-Identifier: GPL-3.0-or-later
#include <nitro.h>
#include <stddef.h>
#include <string.h>
#define CHECK(v) do { if (!(v)) return __LINE__; } while (0)
_Static_assert(sizeof(OSLockWord)==8 && offsetof(OSSystemWork,lock_CARD)==0x3E0,"Original card lock layout");
_Static_assert(offsetof(OSSystemWork,lock_CARTRIDGE)==0x3E8 && offsetof(OSSystemWork,lock_INIT)==0x3F0,"Original shared lock layout");
extern u32 HGLegacyCartridgeOwner(void);
static u32 controls;
static void control(void) { controls++; }

u32 CheckOriginalLockLifecycle(void) {
    OSSystemWork *work=OS_GetSystemWork();
    u32 reset=work->resetParameter;
    OSLockWord cartridge=work->lock_CARTRIDGE;
    s32 first=OS_GetLockID(),second=OS_GetLockID();
    CHECK(first>=OS_MAINP_LOCK_ID_START && second>=OS_MAINP_LOCK_ID_START && first!=second);
    work->resetParameter=0xABCDEF01;
    work->lock_CARTRIDGE=(OSLockWord){0x12345678,0x4321,0x55};
    work->lock_INIT.extension=0xBEEF;
    memset((void *)&work->lock_VRAM_C,0xA5,0x28);
    OS_InitLock();
    CHECK(work->lock_INIT.lockFlag==OS_MAINP_SYSTEM_LOCK_ID && work->lock_INIT.ownerID==OS_MAINP_SYSTEM_LOCK_ID);
    CHECK(work->lock_INIT.extension==0xBEEF && work->resetParameter==0xABCDEF01);
    CHECK(work->lockIDFlag_mainp[0]==0xFFFFFFFF && work->lockIDFlag_mainp[1]==0xFFFF0000);
    const u8 *shared=(const void *)&work->lock_VRAM_C;
    for (u32 i=0;i<0x28;i++) CHECK(!shared[i]);
    CHECK(work->lock_CARTRIDGE.lockFlag==0x12345678 && work->lock_CARTRIDGE.ownerID==0x4321 && work->lock_CARTRIDGE.extension==0x55);
    work->lock_CARTRIDGE=(OSLockWord){0}; work->lock_INIT.extension=0;
    OSIntrMode previous=OS_DisableInterrupts_IrqAndFiq();
    CHECK(!OS_LockCard((u16)first) && work->lock_CARD.ownerID==first && work->lock_CARD.lockFlag==first);
    CHECK(OS_UnlockCard((u16)second)==-2 && work->lock_CARD.ownerID==first);
    OS_InitLock(); CHECK(work->lock_CARD.ownerID==first);
    CHECK(OS_TryLockCard((u16)second)==first && work->lock_CARD.ownerID==first);
    CHECK(!OS_UnlockCard((u16)first) && !work->lock_CARD.ownerID && !work->lock_CARD.lockFlag);
    CHECK(!OS_TryLockCard((u16)second) && !OS_UnLockCard((u16)second));
    CHECK(OS_UnlockCard((u16)first)==-2);
    CHECK(!OS_LockCartridge((u16)first) && HGLegacyCartridgeOwner()==first);
    CHECK(OS_UnlockCartridge((u16)second)==-2);
    CHECK(OS_TryLockCartridge((u16)second)==first && work->lock_CARTRIDGE.ownerID==first);
    CHECK(!OS_UnLockCartridge((u16)first) && !HGLegacyCartridgeOwner());
    CHECK(!OS_TryLockCartridge((u16)second) && !OS_UnlockCartridge((u16)second));
    OSLockWord local={0}; controls=0;
    CHECK(!OS_LockByWord((u16)first,&local,control) && controls==1);
    CHECK(OSi_DoTryLockByWord((u16)second,&local,control,TRUE)==first && controls==1);
    CHECK(OS_UnlockByWord((u16)second,&local,control)==-2 && controls==1);
    CHECK(!OS_UnlockByWord((u16)first,&local,control) && controls==2 && !local.ownerID && !local.lockFlag);
    CHECK(OS_DisableInterrupts_IrqAndFiq()==(OS_INTRMODE_DISABLE_IRQ|OS_INTRMODE_DISABLE_FIQ));
    OS_RestoreInterrupts_IrqAndFiq(previous);
    OS_ReleaseLockID((u16)first); OS_ReleaseLockID((u16)second);
    work->resetParameter=reset; work->lock_CARTRIDGE=cartridge;
    return 0;
}
