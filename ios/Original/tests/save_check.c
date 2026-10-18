// SPDX-License-Identifier: GPL-3.0-or-later
#include "save.h"
#include "math_util.h"
#include "player_data.h"
#include "bag.h"
#include "party.h"
#include "pokemon.h"
#include "save_arrays.h"
#include "pokemon_storage_system.h"
#include "friend_roster.h"
#include <dwc/dwc_account_internal.h>
#include "constants/items.h"
#include "constants/species.h"
#include "../NativeBackup.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define CHECK(value) do { if (!(value)) { fprintf(stderr,"Save check line %d: %s\n",__LINE__,#value); abort(); } } while (0)
_Static_assert(sizeof(SaveData)==0x2330C && offsetof(SaveData,arrayHeaders)==0x23014,"Original save work layout");

static void checkContents(SaveData *save, u32 money) {
    PlayerProfile *profile=Save_PlayerData_GetProfile(save);
    CHECK(PlayerProfile_GetMoney(profile)==money && PlayerProfile_GetTrainerID(profile)==0x12345678);
    CHECK(PlayerProfile_TestBadgeFlag(profile,3));
    CHECK(Bag_GetQuantity(Save_Bag_Get(save),ITEM_POTION,HEAP_ID_3)==7);
    Party *party=SaveArray_Party_Get(save);
    CHECK(Party_GetCount(party)==1 && GetMonData(Party_GetMonByIndex(party,0),MON_DATA_SPECIES,NULL)==SPECIES_CHIKORITA);
    CHECK(PCStorage_GetMonDataByIndexPair(SaveArray_PCStorage_Get(save),0,0,MON_DATA_SPECIES,NULL)==SPECIES_CHIKORITA);
}
static void damageNormalFooter(SaveData *save) {
    const struct SaveSlotSpec *spec=&save->saveSlotSpecs[0];
    u32 address=save->lastGoodSector*0x40000+spec->offset+spec->size-sizeof(struct SaveChunkFooter)+offsetof(struct SaveChunkFooter,crc);
    s32 lock=OS_GetLockID(); CHECK(lock!=OS_LOCK_ID_ERROR); CARD_LockBackup(lock);
    u8 value; CHECK(CARD_ReadBackup(address,&value,1)); value^=1;
    CHECK(CARD_WriteAndVerifyBackup(address,&value,1));
    CARD_UnlockBackup(lock); OS_ReleaseLockID(lock);
}
void CheckNativeSave(void) {
    // Golden sizes from the original callbacks compiled for ABI32, independent of native pointer width.
    const u32 sizes[]={92,44,1456,1948,1100,128,832,480,2176,736,5120,32,2100,1120,264,1568,448,368,1004,
        5672,14,104,248,3016,3744,2240,4088,5760,1672,40,8,64,8,8,1624,1532,4756,2944,128,308,3840,74492};
    const u32 extraSizes[]={10928,2976,7504,7504,7504,7504};
    CHECK(gNumSaveChunkHeaders==sizeof(sizes)/sizeof(sizes[0]) && gNumExtraSaveChunkHeaders==sizeof(extraSizes)/sizeof(extraSizes[0]));
    for (int i=0;i<gNumSaveChunkHeaders;i++) CHECK(gSaveChunkHeaders[i].sizeFunc()==sizes[i]);
    for (int i=0;i<gNumExtraSaveChunkHeaders;i++) CHECK(gExtraSaveChunkHeaders[i].sizeFunc()==extraSizes[i]);
    char folder[]="/tmp/heartgold-save-XXXXXX"; CHECK(mkdtemp(folder));
    char path[256], lock[256]; snprintf(path,sizeof(path),"%s/game.sav",folder); snprintf(lock,sizeof(lock),"%s.lock",path);
    CHECK(HGNativeBackupSetPath(path));
    GF_CRC16Init(HEAP_ID_DEFAULT);
    SaveData *save=SaveData_New();
    CHECK(save && Save_FlashChipIsDetected(save) && !Save_FileExists(save));
    DwcUserData *account=FriendRoster_GetUserData(Save_FriendRoster_Get(save));
    CHECK(DWC_CheckUserData(account) && DWCi_Acc_GetUserId(&account->tempLoginId)==HGNativeBackupConsoleId());
    PlayerProfile *profile=Save_PlayerData_GetProfile(save);
    PlayerProfile_SetMoney(profile,12345); PlayerProfile_SetTrainerID(profile,0x12345678); PlayerProfile_SetBadgeFlag(profile,3);
    CHECK(Bag_AddItem(Save_Bag_Get(save),ITEM_POTION,7,HEAP_ID_3));
    Pokemon mon; CreateMon(&mon,SPECIES_CHIKORITA,5,31,TRUE,0,1,12345);
    CHECK(Party_AddMon(SaveArray_Party_Get(save),&mon));
    CHECK(PCStorage_PlaceMonInBoxByIndexPair(SaveArray_PCStorage_Get(save),0,0,&mon.box));
    CHECK(SaveGameNormal(save)==WRITE_STATUS_SUCCESS && Save_FileExists(save));
    checkContents(save,12345);
    Heap_Free(save); CHECK(HGNativeBackupSetPath(NULL) && HGNativeBackupSetPath(path));
    save=SaveData_New(); CHECK(Save_FileExists(save)); checkContents(save,12345);
    PlayerProfile_SetMoney(Save_PlayerData_GetProfile(save),54321);
    CHECK(SaveGameNormal(save)==WRITE_STATUS_SUCCESS);
    PlayerProfile_SetMoney(Save_PlayerData_GetProfile(save),1);
    CHECK(SaveData_TryLoadOnContinue(save)); checkContents(save,54321);
    damageNormalFooter(save);
    Heap_Free(save); CHECK(HGNativeBackupSetPath(NULL) && HGNativeBackupSetPath(path));
    save=SaveData_New(); CHECK(Save_FileExists(save) && (Save_GetStatusFlags(save)&1)); checkContents(save,12345);
    damageNormalFooter(save);
    Heap_Free(save); CHECK(HGNativeBackupSetPath(NULL) && HGNativeBackupSetPath(path));
    save=SaveData_New(); CHECK(!Save_FileExists(save) && (Save_GetStatusFlags(save)&2));
    Heap_Free(save);
    CHECK(HGNativeBackupSetPath(NULL) && !unlink(path) && !unlink(lock));
    snprintf(lock,sizeof(lock),"%s.console",path); CHECK(!unlink(lock) && !rmdir(folder));
    puts("PASS: original SaveData initialization, trainer/bag/party/PC round trips, dual-slot recovery and corrupt-save rejection.");
}
