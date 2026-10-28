// SPDX-License-Identifier: GPL-3.0-or-later
// Private resident observation only; the original scripts create and grant the starter.
#include "starter_qa.h"
#include "constants/flags.h"
#include "constants/vars.h"
#include "field_system.h"
#include "party.h"
#include "player_avatar.h"
#include "player_avatar_movement.h"
#include "pokedex.h"
#include "save_arrays.h"
#include "save_vars_flags.h"
#include "screen_fade.h"
#include "script.h"
#include "script_manager.h"
#include "system.h"
#include "task.h"
#include "text.h"
#include <string.h>
static HGNativeStarterQAState sStarterQA={.version=HG_STARTER_QA_VERSION,.byteSize=sizeof(HGNativeStarterQAState)};
static FieldSystem *sStarterField;
static const void *sStarterBootOwner,*sStarterProbeOwner;
static ScriptContext *sStarterWaitContext;
static BOOL (*sStarterWaitNative)(ScriptContext *);
static uint32_t sStarterLastActive,sStarterLastParty,sStarterPartySampled,sStarterLastWait;
static uaddr sStarterLastTextToken;
static uint32_t StarterQANext(uint32_t value) {return value+1 ? value+1 : 1;}
#define STARTER_REQUIRE(value) do {if(!(value) && !sStarterQA.error)sStarterQA.error=__LINE__;} while(0)
void NPCYNBootPublish(const void *owner,const HGYNBootFacts *facts) {
    if(!owner || !facts)return;
    uint32_t generation=sStarterQA.boot.ownerGeneration,revision=StarterQANext(sStarterQA.boot.revision);
    if(owner!=sStarterBootOwner || !sStarterQA.boot.active)generation=StarterQANext(generation);
    sStarterBootOwner=owner;sStarterQA.boot=*facts;sStarterQA.boot.active=1;
    sStarterQA.boot.ownerGeneration=generation;sStarterQA.boot.revision=revision;sStarterQA.revision=StarterQANext(sStarterQA.revision);
}
void NPCYNBootRetire(const void *owner) {
    if(!owner || owner!=sStarterBootOwner)return;
    sStarterBootOwner=NULL;sStarterQA.boot.active=sStarterQA.boot.ready=0;
    sStarterQA.boot.revision=StarterQANext(sStarterQA.boot.revision);sStarterQA.revision=StarterQANext(sStarterQA.revision);
}
const HGNativeStarterQAState *HGNativeStarterQAGetState(void) {return &sStarterQA;}
uint32_t StarterQAProbeWanted(const void *owner) {
    const HGNativeStarterUIState *s=HGNativeStarterUIGetState();
    return owner && owner!=sStarterProbeOwner && s->active==1 && s->ready==1 && s->phase==5 && !s->selectionState && !s->selection;
}
void StarterQAProbeObserved(const void *owner,uint32_t queries,uint32_t x,uint32_t y,uint32_t found,uint32_t unchanged) {
    const HGNativeStarterUIState *s=HGNativeStarterUIGetState();STARTER_REQUIRE(StarterQAProbeWanted(owner));
    sStarterProbeOwner=owner;sStarterQA.probeQueries=queries;sStarterQA.probeUnchanged=unchanged;
    sStarterQA.probeGeneration=s->ownerGeneration;sStarterQA.probeSelection=s->selection;
    sStarterQA.probeX=x;sStarterQA.probeY=y;sStarterQA.probeValid=found;
    STARTER_REQUIRE(queries && queries<=713 && found==1 && x<256 && y<192 && unchanged==1);
}
static ScriptEnvironment *StarterQAScript(FieldSystem *field) {
    if(!field || !field->location || field->location->mapId!=HG_STARTER_QA_MAP || !field->bgConfig
        || !FieldSystem_IsFieldMapRunning(field) || FieldSystem_IsChildAppRunning(field) || !IsPaletteFadeFinished()
        || !field->taskman || field->taskman->func!=Task_RunScripts)return NULL;
    ScriptEnvironment *env=TaskManager_GetEnvironment(field->taskman);
    if(!env || env->check!=Unk80_10_C_MAGIC || env->state!=1 || !env->activeScriptContextCount || env->activeScriptContextCount>3
        || (env->activeScriptNumber!=HG_STARTER_QA_INTRO && env->activeScriptNumber!=HG_STARTER_QA_MACHINE))return NULL;
    return env;
}
static BOOL StarterQAContext(ScriptContext *ctx) {
    ScriptEnvironment *env=StarterQAScript(sStarterField);
    return env && ctx && env->scriptContexts[0]==ctx && ctx->fieldSystem==sStarterField && ctx->taskman==sStarterField->taskman;
}
BOOL ScrNative_WaitTextPrinter(ScriptContext *ctx);
void StarterQAWaitButtonObserved(ScriptContext *ctx) {
    if(StarterQAContext(ctx) && ctx->mode==SCRIPT_MODE_NATIVE) {sStarterWaitContext=ctx;sStarterWaitNative=ctx->native_ptr;}
}
void StarterQAChoiceObserved(ScriptContext *ctx,uint32_t result) {
    if(StarterQAContext(ctx) && sStarterQA.gotStarter && ctx->data[0]==VAR_SPECIAL_RESULT) {
        ++sStarterQA.choiceReturns;
        sStarterQA.nicknameNo=result==1;STARTER_REQUIRE(sStarterQA.nicknameNo && sStarterQA.choiceReturns==1);
    }
}
static void StarterQAFieldObserved(FieldSystem *field) {
    sStarterField=field;sStarterQA.revision=StarterQANext(sStarterQA.revision);sStarterQA.frame=OS_GetVBlankCount();
    sStarterQA.ui=*HGNativeStarterUIGetState();sStarterQA.choice=*HGNativeFieldChoiceUIGetState();
    if(sStarterQA.ui.active && !sStarterLastActive)++sStarterQA.starterEntries;
    if(!sStarterQA.ui.active && sStarterLastActive)++sStarterQA.starterExits;
    sStarterLastActive=sStarterQA.ui.active;
    sStarterQA.fieldReady=field && field->saveData && FieldSystem_IsFieldMapLoaded(field) && FieldSystem_IsFieldMapRunning(field)
        && !FieldSystem_TaskIsRunning(field) && !FieldSystem_IsChildAppRunning(field) && FieldSystem_IsPlayerMovementAllowed(field) && IsPaletteFadeFinished();
    sStarterQA.scriptActive=sStarterQA.scriptId=sStarterQA.scriptWait=0;
    if(!field || !field->saveData || !field->location || !field->playerAvatar)return;
    sStarterQA.mapId=field->location->mapId;sStarterQA.x=PlayerAvatar_GetXCoord(field->playerAvatar);sStarterQA.z=PlayerAvatar_GetZCoord(field->playerAvatar);
    sStarterQA.facing=PlayerAvatar_GetFacingDirection(field->playerAvatar);sStarterQA.betweenSteps=PlayerAvatar_IsBetweenSteps(field->playerAvatar);
    sStarterQA.moveState=PlayerAvatar_GetMoveState(field->playerAvatar);sStarterQA.playerMoveState=PlayerAvatar_GetPlayerMoveState(field->playerAvatar);
    Party *party=SaveArray_Party_Get(field->saveData);sStarterQA.partyCount=Party_GetCount(party);
    if(sStarterPartySampled && !sStarterLastParty && sStarterQA.partyCount==1) {
        ++sStarterQA.grants;STARTER_REQUIRE(sStarterQA.starterEntries==1 && sStarterQA.starterExits==1 && sStarterQA.grants==1);
    }
    sStarterLastParty=sStarterQA.partyCount;sStarterPartySampled=1;
    sStarterQA.species=sStarterQA.level=sStarterQA.dexSeen=sStarterQA.dexCaught=0;
    if(sStarterQA.partyCount==1) {
        Pokemon *mon=Party_GetMonByIndex(party,0);sStarterQA.species=GetMonData(mon,MON_DATA_SPECIES,NULL);sStarterQA.level=GetMonData(mon,MON_DATA_LEVEL,NULL);
        Pokedex *dex=Save_Pokedex_Get(field->saveData);sStarterQA.dexSeen=Pokedex_CheckMonSeenFlag(dex,sStarterQA.species);sStarterQA.dexCaught=Pokedex_CheckMonCaughtFlag(dex,sStarterQA.species);
    }
    SaveVarsFlags *flags=Save_VarsFlags_Get(field->saveData);sStarterQA.gotStarter=Save_VarsFlags_CheckFlagInArray(flags,FLAG_GOT_STARTER);
    sStarterQA.scene=*Save_VarsFlags_GetVarAddr(flags,VAR_SCENE_ELMS_LAB);
    ScriptEnvironment *env=StarterQAScript(field);uaddr token=0;
    if(env) {
        sStarterQA.scriptActive=1;sStarterQA.scriptId=env->activeScriptNumber;
        ScriptContext *ctx=env->scriptContexts[0];
        if(ctx && StarterQAContext(ctx) && ctx->mode==SCRIPT_MODE_NATIVE && env->msgWindowOpen && !env->activeMovementCounter
            && env->window.bgConfig==field->bgConfig && env->window.pixelBuffer && env->window.width && env->window.height) {
            if(ctx->native_ptr==ScrNative_WaitTextPrinter && TextPrinterIsWaitingForInput(env->textPrinterNum,&env->window))sStarterQA.scriptWait=1;
            else if(ctx==sStarterWaitContext && ctx->native_ptr==sStarterWaitNative)sStarterQA.scriptWait=2;
            token=(uaddr)ctx->script_ptr;
        }
    }
    if(sStarterQA.scriptWait && (sStarterQA.scriptWait!=sStarterLastWait || token!=sStarterLastTextToken))sStarterQA.scriptSerial=StarterQANext(sStarterQA.scriptSerial);
    sStarterLastWait=sStarterQA.scriptWait;sStarterLastTextToken=token;
}
