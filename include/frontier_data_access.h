#ifndef POKEHEARTGOLD_FRONTIER_DATA_ACCESS_H
#define POKEHEARTGOLD_FRONTIER_DATA_ACCESS_H

#include "frontier_data.h"
#include "pokemon_types_def.h"
#include "save.h"
#include "frontier_wifi_upload_record.h"
#include "heap.h"
#include "mail_message.h"
#include "scrcmd_9.h"

struct BattleRoomDownload;

enum BattleTowerChallengeField {
    TOWER_CHALLENGE_MODE,
    TOWER_CHALLENGE_BATTLE_NO,
    TOWER_CHALLENGE_TIMES_FAINTED,
    TOWER_CHALLENGE_TURNS,
    TOWER_CHALLENGE_DAMAGE,
    TOWER_CHALLENGE_PARTY_SLOTS,
    TOWER_CHALLENGE_ALLY_DATA,
    TOWER_CHALLENGE_ALLY_REPLACE_ITEM,
    TOWER_CHALLENGE_TRAINERS,
    TOWER_CHALLENGE_ALLY_ID,
    TOWER_CHALLENGE_RANDOM_STATE,
};

u32 BattleRoomTrainer_sizeof(void);
void BattleTowerChallenge_Clear(u32 challenge);
void FrontierData_Reset(void *data);
void TowerMessages_Init(void *data);
void BattleRoomDownload_Init(void *data);
u32 BattleTowerChallenge_GetField(u32 challenge, u32 field, void *out);
void BattleTowerChallenge_SetField(u32 challenge, u32 field, void *value);
void BattleTowerChallenge_AddWifiScoreCounters(u32 challenge, u8 timesFainted, u32 turns, u32 damage);
u32 BattleTowerChallenge_GetSaved(u32 record);
void BattleTowerChallenge_SetSaved(u32 challenge, u32 value);
u32 FrontierData_WifiStreakAction(FrontierData *frontierData, u32 action);
u32 FrontierData_WifiRankAction(FrontierData *frontierData, u32 action);
void FrontierData_SetWifiTeam(FrontierData *frontierData, u32 index, void *src);
void FrontierData_GetWifiTeam(FrontierData *frontierData, s32 index, FrontierWifiUploadRecord *dst);
u16 FrontierData_CalcWifiScore(FrontierData *frontierData, u32 challenge);
u16 FrontierData_GetWifiScore(FrontierData *frontierData);
u8 FrontierData_GetWifiWinCount(FrontierData *data);
u32 FrontierData_TowerRoundAction(FrontierData *data, u32 towerMode, u32 action);
u32 FrontierData_SetTowerRound(FrontierData *frontierData, u32 towerMode, u16 round);
u32 FrontierData_FlagAction(FrontierData *frontierData, u16 flag, u32 action);
void FrontierData_SetTowerSeed(FrontierData *data, u32 value);
u32 FrontierData_GetTowerSeed(FrontierData *data);
void Save_Frontier_SetTowerMessage(SaveData *saveData, int index, MailMessage *mailMessage);
MailMessage *Save_Frontier_GetTowerMessage(SaveData *save, u32 index);
void BattleRoomDownload_MarkRoom(struct BattleRoomDownload *download, u32 rank, u32 room, u32 *date);
BOOL BattleRoomDownload_IsRoomMarked(struct BattleRoomDownload *download, u32 rank, u32 room, u32 *date);
u32 BattleRoomDownload_HasTrainers(u32 download);
u32 BattleRoomDownload_HasLeaders(u32 download);
void BattleRoomDownload_SetTrainers(struct BattleRoomDownload *download, void *trainers, u32 rank, u32 room);
void BattleRoomDownload_GetTrainersRoom(struct BattleRoomDownload *download, u8 *out);
void BattleRoomDownload_GetTrainer(u32 addr, FrontierTrainer *trainer, int index);
void BattleRoomDownload_SetLeaders(struct BattleRoomDownload *download, void *leaders, u32 rank, u32 room);
void BattleRoomDownload_GetLeadersRoom(u32 addr, u8 *out);
void *BattleRoomDownload_CopyLeaders(u32 addr, enum HeapID heapID);
u32 Save_BattleTowerChallenge_Get(SaveData *saveData);
u32 Save_BattleRoomDownload_Get(SaveData *saveData);

#endif // POKEHEARTGOLD_FRONTIER_DATA_ACCESS_H
