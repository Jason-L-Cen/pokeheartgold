// SPDX-License-Identifier: GPL-3.0-or-later
// Included only in the isolated QA copy of field_system.c, never the shipping module.
#include "field_system.h"
#include "encounter.h"
#include "battle_intro_config.h"
#include "constants/trainers.h"
#include "constants/species.h"
#include "constants/moves.h"
#include "pokemon.h"

static u32 sBattleQARequested;
u32 HGNativeBattleQARequest(void) {
    if (!sBattleQARequested) sBattleQARequested=1;
    return sBattleQARequested;
}
static BOOL BattleQATask(TaskManager *taskManager) {
    u32 *state=TaskManager_GetStatePtr(taskManager);
    if (*state) return TRUE;
    FieldSystem *fieldSystem=TaskManager_GetFieldSystem(taskManager);
    BattleSetup *setup=BattleSetup_New(HEAP_ID_FIELD2,BATTLE_TYPE_TRAINER|BATTLE_TYPE_DOUBLES);
    BattleSetup_InitFromFieldSystem(setup,fieldSystem);
    // Change only the battle-owned party; the loaded player's party and save file remain intact.
    SaveArray_Party_Init(setup->party[BATTLER_PLAYER]);
    Pokemon *mon=AllocMonZeroed(HEAP_ID_FIELD2);
    const u16 species[]={SPECIES_CHIKORITA,SPECIES_CYNDAQUIL};
    for (unsigned i=0;i<2;i++) {
        CreateMon(mon,species[i],15,31,TRUE,i+1,0,0);
        MonSetMoveInSlot(mon,MOVE_TACKLE,0); MonSetMoveInSlot(mon,MOVE_GROWL,1);
        GF_ASSERT(Party_AddMon(setup->party[BATTLER_PLAYER],mon));
    }
    Heap_Free(mon);
    setup->followMonSlot[BATTLER_PLAYER]=0xFF;
    setup->trainerId[BATTLER_ENEMY]=TRAINER_TWINS_AMY_AND_MIMI;
    setup->trainerId[BATTLER_ENEMY2]=TRAINER_TWINS_AMY_AND_MIMI;
    EnemyTrainerSet_Init(setup,fieldSystem->saveData,HEAP_ID_FIELD2);
    GF_ASSERT(Party_GetCount(setup->party[BATTLER_ENEMY])>=2);
    *state=1;
    CallTask_TrainerHouseBattle(taskManager,setup,BattleSetup_GetWildTransitionEffect(setup),BattleSetup_GetWildBattleMusic(setup),NULL);
    return FALSE;
}
static void BattleQAFieldTick(FieldSystem *fieldSystem) {
    if (sBattleQARequested!=1 || fieldSystem->taskman || !FieldSystem_IsFieldMapRunning(fieldSystem) || FieldSystem_IsChildAppRunning(fieldSystem)) return;
    sBattleQARequested=2;
    // FieldSystem_Control clears this task before starting a field event; QA bypasses that input router.
    FieldDrawMapName_Reset(fieldSystem->mapSystems->drawMapNameInfo);
    FieldSystem_CreateTask(fieldSystem,BattleQATask,NULL);
}
