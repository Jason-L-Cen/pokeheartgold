#ifndef POKEHEARTGOLD_BATTLE_RECORD_H
#define POKEHEARTGOLD_BATTLE_RECORD_H

#include "battle/battle_setup.h"

#include "battle_recording.h"
#include "heap.h"
#include "save.h"

// Battle Video functions; the record type and the recording interface are in battle_recording.h.

// Fields of BattleRecordHeader_GetField
enum BattleRecordHeaderField {
    BATTLE_RECORD_HEADER_SPECIES,
    BATTLE_RECORD_HEADER_FORM,
    BATTLE_RECORD_HEADER_ARG,
    BATTLE_RECORD_HEADER_TYPE,
    BATTLE_RECORD_HEADER_VIDEO_NUMBER,
    BATTLE_RECORD_HEADER_SEALED,
};

u32 BattleRecord_sizeof(void);
void BattleRecord_Init(BattleRecord *record);
void BattleRecord_Delete(void);
BattleRecord *BattleRecord_Get(void);
void *BattleRecord_GetProfile2(void);
BOOL BattleRecord_Load(SaveData *save, enum HeapID heapId, int *out, void *battle, int index);
BOOL BattleRecord_IsSlotValid(SaveData *save, enum HeapID heapId, int *out, int index);
int BattleRecord_WriteSlot(SaveData *save, BattleRecord *record, int index, u16 *state);
int BattleRecord_Save(SaveData *save, int type, int arg, int index, u16 *state, u16 *writeState);
void BattleRecord_GetBattlerAndPartyCount(int type, int *count, int *last);
void BattleRecord_Encrypt(void *data, u32 size, u32 seed);
void BattleRecord_StoreSetup(BattleSetup *setup);
void BattleRecord_SetAiFlags(int battlerId, u32 flags);
BOOL BattleRecord_AreAiFlagsValid(void);
void BattleRecord_RestoreSetup(BattleSetup *setup, SaveData *save);
BattleRecordHeader *BattleRecord_CopyHeader(enum HeapID heapId);
void *BattleRecord_CopyProfile(enum HeapID heapId);
void *BattleRecord_GetProfile(void);
BattleRecordHeader *BattleRecord_GetHeader(void);
void BattleRecord_Import(void *profile, void *summary, void *data, void *battle, SaveData *save);
u64 BattleRecordHeader_GetField(BattleRecordHeader *header, enum BattleRecordHeaderField field, int index);
BattleRecordHeader *BattleRecordHeader_New(enum HeapID heapId);
void BattleRecordHeader_Free(BattleRecordHeader *header);

#endif // POKEHEARTGOLD_BATTLE_RECORD_H
