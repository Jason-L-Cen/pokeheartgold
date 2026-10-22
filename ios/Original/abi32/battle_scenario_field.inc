// SPDX-License-Identifier: GPL-3.0-or-later
// Included only in the isolated QA copy of field_system.c, never the shipping module.
#include <stdint.h>
typedef struct EliteQAState {
    uint32_t caseIndex,trainerId,trainerClass,battleType,effect,music,error;
    uint32_t starts,unloads,launches,stateMask,state,timer,frame,observations;
    uint32_t stateFrames[15],stateVisits[15],requestTimer;
    uint32_t particleLoads,members[2],emitterMask[2],emitterCreates[2],activeEmitters,particleDraws[2],particleCalcs[2];
    uint32_t particleFrees,particleEnds,finishes,spriteLoads,spriteFrees,managerFrees,windowFrees,visibleSprites;
    uint32_t palettes[4],characters[4],cells[4],animations[4];
    uint32_t heapBefore,heapAfter,mainBefore,mainAfter,vwaitBefore,vwaitAfter,flagsAfter,playerGender,stopRequested,fieldReady,readyFrame;
} EliteQAState;
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
    EliteQAState elite;
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
#include "field/encounter_effect.h"
#include "launch_application.h"
#include "particle_system.h"
#include "particle_system_internal.h"
#include "system.h"
#include "screen_fade.h"

static u32 sBattleQARequested;
static BattleQAState sBattleQAState;
static BOOL sBattleQAArmRequested;
#define ELITE_REQUIRE(v) do { if (!(v) && !sBattleQAState.elite.error) sBattleQAState.elite.error=__LINE__; } while (0)
static const u16 sEliteTrainers[]={245,247,418,246,244};
static const u16 sEliteClasses[]={87,89,112,88,86};
static FieldSystem *sEliteField;
static EncounterEffectTask *sEliteEffect;
static SPLEmitter *sEliteSystem;
// Read-only observer injected only into the isolated original leaders TU.
void BattleQAEliteObserve(EncounterEffectTask *effect,Sprite *const *sprites) {
    EliteQAState *qa=&sBattleQAState.elite;
    if (!qa->caseIndex) return;
    ELITE_REQUIRE(effect->state<15);
    if (effect->state>=15) return;
    ELITE_REQUIRE(!qa->observations || effect->state==qa->state || effect->state==qa->state+1);
    if (!qa->stateVisits[effect->state]) qa->stateFrames[effect->state]=gEncounterEffectState.frameCount;
    qa->stateVisits[effect->state]++; qa->stateMask|=1u<<effect->state;
    qa->state=effect->state; qa->timer=effect->timer; qa->frame=gEncounterEffectState.frameCount; qa->observations++;
    sEliteEffect=effect; qa->visibleSprites=0;
    if (sprites) for (u32 i=0;i<4;i++) qa->visibleSprites+=Sprite_GetDrawFlag(sprites[i])!=0;
}
int __real_BattleSetup_GetWildTransitionEffect(BattleSetup *);
int __wrap_BattleSetup_GetWildTransitionEffect(BattleSetup *setup) {
    int effect=__real_BattleSetup_GetWildTransitionEffect(setup); EliteQAState *qa=&sBattleQAState.elite;
    if (qa->caseIndex) {
        qa->trainerId=setup->trainerId[BATTLER_ENEMY]; qa->trainerClass=setup->trainer[BATTLER_ENEMY].data.trainerClass;
        qa->battleType=setup->battleType; qa->effect=effect;
        ELITE_REQUIRE(qa->caseIndex<=5 && qa->trainerId==sEliteTrainers[qa->caseIndex-1] &&
            qa->trainerClass==sEliteClasses[qa->caseIndex-1] && qa->battleType==BATTLE_TYPE_TRAINER && effect==28+qa->caseIndex);
    }
    return effect;
}
int __real_BattleSetup_GetWildBattleMusic(BattleSetup *);
int __wrap_BattleSetup_GetWildBattleMusic(BattleSetup *setup) {
    int music=__real_BattleSetup_GetWildBattleMusic(setup);
    if (sBattleQAState.elite.caseIndex) sBattleQAState.elite.music=music;
    return music;
}
void __real_EncounterEffect_Start(s32,FieldSystem *,BOOL *);
void __wrap_EncounterEffect_Start(s32 effect,FieldSystem *fieldSystem,BOOL *done) {
    EliteQAState *qa=&sBattleQAState.elite;
    if (qa->caseIndex) {
        ELITE_REQUIRE(effect==28+qa->caseIndex && !qa->starts++);
        sEliteField=fieldSystem; qa->heapBefore=HeapExp_FndGetTotalFreeSize(HEAP_ID_FIELD1);
        qa->mainBefore=gSystem.mainTaskQueue->activeCount; qa->vwaitBefore=gSystem.vwaitTaskQueue->activeCount;
        qa->playerGender=PlayerProfile_GetTrainerGender(Save_PlayerData_GetProfile(fieldSystem->saveData));
    }
    __real_EncounterEffect_Start(effect,fieldSystem,done);
}
void __real_EncounterEffect_RequestParticleSetup(void);
void __wrap_EncounterEffect_RequestParticleSetup(void) {
    if (sBattleQAState.elite.caseIndex) {
        ELITE_REQUIRE(sEliteEffect && sEliteEffect->state==3 && sEliteEffect->timer==8);
        if (sEliteEffect) sBattleQAState.elite.requestTimer=sEliteEffect->timer;
    }
    __real_EncounterEffect_RequestParticleSetup();
}
void __real_EncounterEffect_LoadSpriteResources(NARC *,EncounterEffectSpriteManager *,EncounterEffectSpriteResources *,int,int,int,int,int,int);
void __wrap_EncounterEffect_LoadSpriteResources(NARC *narc,EncounterEffectSpriteManager *manager,EncounterEffectSpriteResources *resources,int palette,int count,int character,int cell,int animation,int id) {
    EliteQAState *qa=&sBattleQAState.elite;
    if (qa->caseIndex) {
        u32 i=qa->spriteLoads++; ELITE_REQUIRE(i<4);
        if (i<4) { qa->palettes[i]=palette; qa->characters[i]=character; qa->cells[i]=cell; qa->animations[i]=animation; }
    }
    __real_EncounterEffect_LoadSpriteResources(narc,manager,resources,palette,count,character,cell,animation,id);
}
void __real_EncounterEffect_LoadParticles(NARC *,int);
void __wrap_EncounterEffect_LoadParticles(NARC *narc,int member) {
    EliteQAState *qa=&sBattleQAState.elite;
    if (qa->caseIndex) { u32 i=qa->particleLoads++; ELITE_REQUIRE(i<2 && member==151+i); if(i<2) qa->members[i]=member; }
    __real_EncounterEffect_LoadParticles(narc,member);
}
SPLEmitter *__real_ParticleSystem_CreateEmitter(SPLEmitter *,int,const VecFx32 *);
SPLEmitter *__wrap_ParticleSystem_CreateEmitter(SPLEmitter *system,int id,const VecFx32 *position) {
    EliteQAState *qa=&sBattleQAState.elite;
    if(!qa->caseIndex) return __real_ParticleSystem_CreateEmitter(system,id,position);
    SPLManager *manager=((ParticleSystem *)system)->manager;
    u32 active=manager->act_emtr_list.node_num,inactive=manager->ina_emtr_list.node_num;
    SPLEmitter *previous=manager->act_emtr_list.p_begin;
    SPLResource *resource=(u32)id<manager->res_num ? &manager->res_ary[id] : NULL;
    SPLEmitter *emitter=__real_ParticleSystem_CreateEmitter(system,id,position),*created=manager->act_emtr_list.p_begin;
    // SPL_Create pushes the real node at the head, but intentionally returns NULL for self-destroying resources.
    BOOL inserted=resource && created && created!=previous && created->p_res==resource &&
        manager->act_emtr_list.node_num==active+1 && manager->ina_emtr_list.node_num+1==inactive;
    u32 i=qa->particleLoads-1;
    ELITE_REQUIRE(i<2 && (u32)id<3+i && inserted && (resource->p_base->flag.self_dest ? !emitter : emitter==created));
    if(inserted && i<2 && (u32)id<4) { qa->emitterCreates[i]++; qa->emitterMask[i]|=1u<<id; }
    sEliteSystem=system;
    return emitter;
}
int __real_ParticleSystem_DrawAll(void);
int __wrap_ParticleSystem_DrawAll(void) {
    int count=__real_ParticleSystem_DrawAll(); EliteQAState *qa=&sBattleQAState.elite;
    if (qa->caseIndex && sEliteSystem) { u32 i=qa->particleLoads-1; if(i<2 && count) qa->particleDraws[i]++; qa->activeEmitters=ParticleSystem_GetActiveEmitterCount(sEliteSystem); }
    return count;
}
int __real_ParticleSystem_CalcAll(void);
int __wrap_ParticleSystem_CalcAll(void) {
    int count=__real_ParticleSystem_CalcAll(); EliteQAState *qa=&sBattleQAState.elite;
    if (qa->caseIndex && sEliteSystem) { u32 i=qa->particleLoads-1; if(i<2 && count) qa->particleCalcs[i]++; }
    return count;
}
void __real_EncounterEffect_FreeParticles(void);
void __wrap_EncounterEffect_FreeParticles(void) {
    __real_EncounterEffect_FreeParticles();
    if (sBattleQAState.elite.caseIndex) { sBattleQAState.elite.particleFrees++; sEliteSystem=NULL; sBattleQAState.elite.activeEmitters=0; }
}
void __real_EncounterEffect_FreeSpriteResources(EncounterEffectSpriteManager *,EncounterEffectSpriteResources *);
void __wrap_EncounterEffect_FreeSpriteResources(EncounterEffectSpriteManager *manager,EncounterEffectSpriteResources *resources) {
    __real_EncounterEffect_FreeSpriteResources(manager,resources); if(sBattleQAState.elite.caseIndex) sBattleQAState.elite.spriteFrees++;
}
void __real_EncounterEffect_FreeSprites(EncounterEffectSpriteManager *);
void __wrap_EncounterEffect_FreeSprites(EncounterEffectSpriteManager *manager) {
    __real_EncounterEffect_FreeSprites(manager); if(sBattleQAState.elite.caseIndex) sBattleQAState.elite.managerFrees++;
}
void __real_RemoveWindow(Window *);
void __wrap_RemoveWindow(Window *window) {
    if(sBattleQAState.elite.caseIndex && sEliteEffect && sEliteEffect->state==14) sBattleQAState.elite.windowFrees++;
    __real_RemoveWindow(window);
}
void __real_EncounterEffect_EndParticleMode(void);
void __wrap_EncounterEffect_EndParticleMode(void) {
    __real_EncounterEffect_EndParticleMode(); EliteQAState *qa=&sBattleQAState.elite;
    if(qa->caseIndex) { qa->particleEnds++; qa->flagsAfter=sEliteField->fieldMapUpdateFlags;
        ELITE_REQUIRE(!gEncounterEffectState.particles && !ParticleSystem_CountActive() && (qa->flagsAfter&5)==5); }
}
void __real_EncounterEffect_Finish(EncounterEffectTask *,SysTask *);
void __wrap_EncounterEffect_Finish(EncounterEffectTask *effect,SysTask *task) {
    EliteQAState *qa=&sBattleQAState.elite;
    if(qa->caseIndex) ELITE_REQUIRE(effect->state==14 && effect->done && *effect->done && !reg_G2_BLDCNT);
    __real_EncounterEffect_Finish(effect,task);
    if(qa->caseIndex) {
        qa->finishes++; sEliteEffect=NULL; qa->heapAfter=HeapExp_FndGetTotalFreeSize(HEAP_ID_FIELD1);
        qa->mainAfter=gSystem.mainTaskQueue->activeCount; qa->vwaitAfter=gSystem.vwaitTaskQueue->activeCount;
        ELITE_REQUIRE(qa->heapBefore==qa->heapAfter && qa->mainBefore==qa->mainAfter && qa->vwaitBefore==qa->vwaitAfter);
    }
}
void __real_EncounterEffect_UnloadOverlay(s32);
void __wrap_EncounterEffect_UnloadOverlay(s32 effect) {
    __real_EncounterEffect_UnloadOverlay(effect); if(sBattleQAState.elite.caseIndex) sBattleQAState.elite.unloads++;
}
void __real_Battle_LaunchApp(FieldSystem *,BattleSetup *);
void __wrap_Battle_LaunchApp(FieldSystem *fieldSystem,BattleSetup *setup) {
    EliteQAState *qa=&sBattleQAState.elite;
    if(qa->caseIndex) { qa->launches++; ELITE_REQUIRE(qa->stateMask==0x7FFF && qa->finishes==1 && qa->unloads==1 &&
        qa->particleFrees==2 && qa->particleEnds==1 && qa->spriteFrees==4 && qa->managerFrees==1 && qa->windowFrees==1); }
    __real_Battle_LaunchApp(fieldSystem,setup);
}
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
    // Host requests shutdown only in QA-owned state; the original reset runs on the real battle owner.
    if (sBattleQAState.elite.caseIndex && sBattleQAState.elite.stopRequested) OS_ResetSystem(0);
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
    if (sBattleQAState.elite.caseIndex>5) return FALSE;
    if (!sBattleQARequested) sBattleQARequested=1;
    return sBattleQARequested;
}
static BOOL BattleQATask(TaskManager *taskManager) {
    u32 *state=TaskManager_GetStatePtr(taskManager);
    if (*state) return TRUE;
    FieldSystem *fieldSystem=TaskManager_GetFieldSystem(taskManager);
    if (sBattleQAState.elite.caseIndex) {
        GF_ASSERT(sBattleQAState.elite.caseIndex<=5); *state=1;
        SetupAndStartTrainerBattle(taskManager,sEliteTrainers[sBattleQAState.elite.caseIndex-1],0,0,0,0,HEAP_ID_FIELD2,NULL);
        return FALSE;
    }
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
    if(sBattleQAState.elite.caseIndex) {
        EliteQAState *qa=&sBattleQAState.elite;
        qa->fieldReady=!fieldSystem->taskman && FieldSystem_IsFieldMapRunning(fieldSystem) && !FieldSystem_IsChildAppRunning(fieldSystem) && IsPaletteFadeFinished();
        if(qa->fieldReady && !qa->readyFrame) qa->readyFrame=OS_GetVBlankCount();
    }
    if (sBattleQARequested!=1 || fieldSystem->taskman || !FieldSystem_IsFieldMapRunning(fieldSystem) || FieldSystem_IsChildAppRunning(fieldSystem)) return;
    sBattleQARequested=2;
    // FieldSystem_Control clears this task before starting a field event; QA bypasses that input router.
    FieldDrawMapName_Reset(fieldSystem->mapSystems->drawMapNameInfo);
    FieldSystem_CreateTask(fieldSystem,BattleQATask,NULL);
}
#endif
