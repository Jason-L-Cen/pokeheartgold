// SPDX-License-Identifier: GPL-3.0-or-later
// Included only in the isolated QA copy of field_system.c, never the shipping module.
#include <stdint.h>
typedef struct BattleQAState {
    uint32_t serial,armed,totalTurns,initialTurn;
    uint32_t hp[4],initialHp[4],maxHp[4];
    uint32_t pp[4][4],initialPP[4][4];
    uint32_t species[4],level[4],partySlot[4],moves[4][4];
    uint32_t selectedMove[4],selectedSlot[4],action[4],target[4],selection[4];
    uint32_t initialMove[4],initialSlot[4],initialAction[4],initialTarget[4];
    uint32_t commandReply[4],moveReply[4],targetReply[4],status[4],status2[4],failures[4];
    uint32_t animationActive,animationStarts,animationEnds,animationBattlers;
    uint32_t playerOTMask,fixtureOwnerRepairs,disobedienceBattlers;
} BattleQAState;

#ifndef HG_BATTLE_QA_STATE_ONLY
#include "field_system.h"
#include "encounter.h"
#include "battle_intro_config.h"
#include "battle/battle_system.h"
#include "battle/battle_controller.h"
#include "battle/battle_util.h"
#include "field/draw_map_name.h"
#include "map_header.h"
#include "overlay_07.h"
#include "trainer_memo.h"
#include "constants/battle_subscript.h"
#include "constants/trainers.h"
#include "constants/species.h"
#include "constants/moves.h"
#include "pokemon.h"

static u32 sBattleQARequested;
static BattleQAState sBattleQAState;
static BOOL sBattleQAArmRequested;
void __real_BattleController_ReplyCommandSelection(BattleSystem *,int,u32);
void __real_BattleController_ReplyMoveSelection(BattleSystem *,int,u32);
void __real_BattleController_ReplyTargetSelection(BattleSystem *,int,u32);
void __wrap_BattleController_ReplyCommandSelection(BattleSystem *system,int battler,u32 input) {
    GF_ASSERT((u32)battler<4); sBattleQAState.commandReply[battler]=input;
    __real_BattleController_ReplyCommandSelection(system,battler,input);
}
void __wrap_BattleController_ReplyMoveSelection(BattleSystem *system,int battler,u32 input) {
    GF_ASSERT((u32)battler<4); sBattleQAState.moveReply[battler]=input;
    __real_BattleController_ReplyMoveSelection(system,battler,input);
}
void __wrap_BattleController_ReplyTargetSelection(BattleSystem *system,int battler,u32 input) {
    GF_ASSERT((u32)battler<4); sBattleQAState.targetReply[battler]=input;
    __real_BattleController_ReplyTargetSelection(system,battler,input);
}
const BattleQAState *HGNativeBattleQAGetState(void) { return &sBattleQAState; }
u32 HGNativeBattleQAArm(void) {
    if (!sBattleQAState.serial || sBattleQAArmRequested) return FALSE;
    sBattleQAArmRequested=TRUE;
    sBattleQAState.initialTurn=sBattleQAState.totalTurns;
    for (u32 i=0;i<4;i++) {
        sBattleQAState.initialHp[i]=sBattleQAState.hp[i];
        sBattleQAState.initialMove[i]=sBattleQAState.selectedMove[i];
        sBattleQAState.initialSlot[i]=sBattleQAState.selectedSlot[i];
        sBattleQAState.initialAction[i]=sBattleQAState.action[i];
        sBattleQAState.initialTarget[i]=sBattleQAState.target[i];
        for (u32 j=0;j<4;j++) sBattleQAState.initialPP[i][j]=sBattleQAState.pp[i][j];
    }
    return TRUE;
}
// Called only before the real Battle_BattleMain returns, while its owner/context are alive.
void BattleQAObserve(BattleSystem *battleSystem) {
    BattleContext *ctx=BattleSystem_GetBattleContext(battleSystem);
    sBattleQAState.serial++;
    sBattleQAState.totalTurns=ctx->totalTurns;
    for (u32 i=0;i<4;i++) {
        sBattleQAState.hp[i]=ctx->battleMons[i].hp<0 ? 0 : ctx->battleMons[i].hp;
        sBattleQAState.maxHp[i]=ctx->battleMons[i].maxHp;
        sBattleQAState.species[i]=ctx->battleMons[i].species;
        sBattleQAState.level[i]=ctx->battleMons[i].level;
        sBattleQAState.partySlot[i]=ctx->selectedMonIndex[i];
        sBattleQAState.selectedMove[i]=ctx->moveNoSelected[i];
        sBattleQAState.selectedSlot[i]=ctx->movePos[i];
        sBattleQAState.action[i]=ctx->playerActions[i].command;
        sBattleQAState.target[i]=ctx->playerActions[i].battlerIdTarget;
        sBattleQAState.selection[i]=ctx->selectionState[i];
        sBattleQAState.status[i]=ctx->battleMons[i].status;
        sBattleQAState.status2[i]=ctx->battleMons[i].status2;
        if (sBattleQAState.armed) sBattleQAState.failures[i]|=ctx->moveFail[i].paralysis|
            ctx->moveFail[i].confusion<<1|ctx->moveFail[i].flinch<<2|ctx->moveFail[i].infatuation<<3|
            ctx->moveFail[i].disabled<<4|ctx->moveFail[i].noEffect<<5;
        for (u32 j=0;j<4;j++) {
            sBattleQAState.pp[i][j]=ctx->battleMons[i].movePPCur[j];
            sBattleQAState.moves[i][j]=ctx->battleMons[i].moves[j];
        }
    }
    if (sBattleQAArmRequested && !sBattleQAState.armed && ctx->command!=CONTROLLER_COMMAND_SELECTION_SCREEN_INPUT) {
        for (u32 i=0;i<4;i++) if (BattleSystem_IsPlayerOT(battleSystem,BattleSystem_GetPartyMon(battleSystem,i,ctx->selectedMonIndex[i]))) {
            sBattleQAState.playerOTMask|=1u<<i;
        }
        GF_ASSERT((sBattleQAState.playerOTMask&5)==5);
        sBattleQAState.armed=TRUE;
    }
    if (sBattleQAState.armed) {
        if (ctx->scriptNarcId==NARC_a_0_0_1 && ctx->scriptFileId>=BATTLE_SUBSCRIPT_DISOBEY_WHILE_ASLEEP
            && ctx->scriptFileId<=BATTLE_SUBSCRIPT_DISOBEY_HIT_SELF && (u32)ctx->battlerIdAttacker<4) {
            sBattleQAState.disobedienceBattlers|=1u<<ctx->battlerIdAttacker;
        }
        BOOL active=BattleAnimSystem_IsActive((BattleAnimSystem *)BattleSystem_GetBattleAnimSystem(battleSystem));
        if (active && !sBattleQAState.animationActive) {
            sBattleQAState.animationStarts++;
            if ((u32)ctx->battlerIdAttacker<4) sBattleQAState.animationBattlers|=1u<<ctx->battlerIdAttacker;
        } else if (!active && sBattleQAState.animationActive) sBattleQAState.animationEnds++;
        sBattleQAState.animationActive=active;
    }
}
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
    PlayerProfile *profile=Save_PlayerData_GetProfile(fieldSystem->saveData);
    const u16 species[]={SPECIES_CHIKORITA,SPECIES_CYNDAQUIL};
    for (unsigned i=0;i<2;i++) {
        CreateMon(mon,species[i],15,31,TRUE,i+1,0,0);
        if (GetMonData(mon,MON_DATA_OT_ID,NULL)!=PlayerProfile_GetTrainerID(profile)) sBattleQAState.fixtureOwnerRepairs++;
        // Match the real player OT; foreign Lv15 mons legitimately disobey this early checkpoint.
        MonSetTrainerMemo(mon,profile,0,MapHeader_GetMapSec(fieldSystem->location->mapId),HEAP_ID_FIELD2);
        GF_ASSERT(BoxmonBelongsToPlayer(&mon->box,profile,HEAP_ID_FIELD2));
        MonSetMoveInSlot(mon,MOVE_TACKLE,0); MonSetMoveInSlot(mon,MOVE_GROWL,1);
        GF_ASSERT(Party_AddMon(setup->party[BATTLER_PLAYER],mon));
    }
    Heap_Free(mon);
    setup->followMonSlot[BATTLER_PLAYER]=0xFF;
    setup->trainerId[BATTLER_ENEMY]=TRAINER_TWINS_AMY_AND_MIMI;
    setup->trainerId[BATTLER_ENEMY2]=TRAINER_TWINS_AMY_AND_MIMI;
    EnemyTrainerSet_Init(setup,fieldSystem->saveData,HEAP_ID_FIELD2);
    GF_ASSERT(Party_GetCount(setup->party[BATTLER_ENEMY])>=2);
    setup->randSeed=0x12345678; // Reproduce original hit/AI/critical rolls without changing battle rules.
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
#endif
