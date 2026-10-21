#ifndef POKEHEARTGOLD_FRONTIER_BATTLE_TOWER_SETUP_H
#define POKEHEARTGOLD_FRONTIER_BATTLE_TOWER_SETUP_H

#include "battle/battle_setup.h"
#include "frontier/frontier.h"

#include "filesystem.h"
#include "scrcmd_9.h"

typedef struct BattleTowerTrainerSets BattleTowerTrainerSets;

u32 BattleTower_GenerateTrainerMons(FrontierFieldSystem *tower, FrontierTrainer *trainer, u16 trainerId, u32 count, u16 *partySpecies, u16 *partyItems, FrontierMultiBattleAllyData *allyData, enum HeapID heapID);
void BattleTower_GetRoomTrainer(SaveData *saveData, FrontierTrainer *trainer, int index);
BattleSetup *BattleTower_NewBattleSetup(FrontierFieldSystem *tower, FrontierLaunchArgs *args);
void BattleTowerSetup_SetLinkFlag(FrontierFieldSystem *tower, u16 value);
BOOL BattleTowerSetup_IsLinkFlagSetByEither(FrontierFieldSystem *tower, const u16 *remote);
u32 BattleTower_GetTrainerMessageBank(u32 towerMode);

#endif // POKEHEARTGOLD_FRONTIER_BATTLE_TOWER_SETUP_H
