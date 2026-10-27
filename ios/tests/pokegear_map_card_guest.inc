// SPDX-License-Identifier: GPL-3.0-or-later
// This independent case keeps the existing Phone33 observer and its original bodies intact.
#define HGNativePokegearQAGetState PGMapBaseGetState
#define HGNativePokegearQARequest PGMapBaseRequest
#define PokegearQAFieldObserved PGMapBaseFieldObserved
#define PokegearQAParentObserved PGMapBaseParentObserved
#define PokegearQAParentExitObserved PGMapBaseParentExitObserved
#include "pokegear_qa_guest.inc"
#undef HGNativePokegearQAGetState
#undef HGNativePokegearQARequest
#undef PokegearQAFieldObserved
#undef PokegearQAParentObserved
#undef PokegearQAParentExitObserved
#include "pokegear_map_card_qa.h"
#include "battle/battle_setup.h"
#include "constants/battle.h"
#include "constants/player_avatar.h"
#include "launch_application.h"
#include "map_object.h"
#include "overlay_01.h"
#include "player_avatar.h"
#include "player_avatar_movement.h"
#include "script.h"
#include "script_manager.h"
static HGPokegearMapCardQAState sPGMap={.version=HG_PG_MAP_VERSION,.byteSize=sizeof(HGPokegearMapCardQAState),.revision=1};
static const PokegearAppData *sPGMapParent;
static const BattleSetup *sPGMapBattle;
static uint32_t sPGMapPending,sPGMapLastWait;
static uaddr sPGMapLastTextToken;
#define PGMAP_REQUIRE(value) do{if(!(value)){if(!sPGMap.error)sPGMap.error=__LINE__;return;}}while(0)
static uint32_t PGMapTextWait(FieldSystem *field,uint32_t *script,uaddr *textToken) {
    *script=0;*textToken=0;
    if(!field || !field->taskman || field->taskman->func!=Task_RunScripts || !field->bgConfig
        || !FieldSystem_IsFieldMapRunning(field) || FieldSystem_IsChildAppRunning(field) || !IsPaletteFadeFinished())return 0;
    ScriptEnvironment *env=TaskManager_GetEnvironment(field->taskman);
    if(!env || env->check!=Unk80_10_C_MAGIC || env->state!=1 || !env->activeScriptContextCount || env->activeScriptContextCount>3)return 0;
    *script=env->activeScriptNumber;
    if((*script!=2 && *script!=3) || !env->msgWindowOpen || env->activeMovementCounter || !env->window.pixelBuffer
        || env->window.bgConfig!=field->bgConfig || !env->window.width || !env->window.height)return 0;
    ScriptContext *ctx=env->scriptContexts[0];
    if(!ctx || ctx->fieldSystem!=field || ctx->taskman!=field->taskman || ctx->mode!=SCRIPT_MODE_NATIVE
        || ctx->native_ptr!=ScrNative_WaitTextPrinter)return 0;
    *textToken=(uaddr)ctx->script_ptr;
    return PGQAPrinterWait(gSystem.printTaskQueue,env->textPrinterNum,&env->window);
}
void PokegearQAFieldObserved(FieldSystem *field) {
    PGMapBaseFieldObserved(field);sPGMap.phone=*PGMapBaseGetState();sPGMap.revision=PGQANext(sPGMap.revision);
    if(!field || !field->saveData || !field->location || !field->playerAvatar)return;
    sPGMap.mapId=field->location->mapId;sPGMap.x=PlayerAvatar_GetXCoord(field->playerAvatar);sPGMap.z=PlayerAvatar_GetZCoord(field->playerAvatar);
    sPGMap.facing=PlayerAvatar_GetFacingDirection(field->playerAvatar);sPGMap.betweenSteps=PlayerAvatar_IsBetweenSteps(field->playerAvatar);
    sPGMap.moveState=PlayerAvatar_GetMoveState(field->playerAvatar);sPGMap.playerMoveState=PlayerAvatar_GetPlayerMoveState(field->playerAvatar);
    sPGMap.scene=*Save_VarsFlags_GetVarAddr(Save_VarsFlags_Get(field->saveData),VAR_SCENE_CHERRYGROVE_CITY_OW);
    SavePokegear *gear=SaveData_Pokegear_Get(field->saveData);sPGMap.cards=Pokegear_GetRegisteredCardsArray(gear);sPGMap.lastApp=SavePokegear_GetLastUsedApp(gear);
    sPGMap.scriptActive=field->taskman && field->taskman->func==Task_RunScripts;
    uaddr textToken=0;sPGMap.scriptWait=PGMapTextWait(field,&sPGMap.scriptId,&textToken);
    if(sPGMap.scriptWait && (sPGMap.scriptWait!=sPGMapLastWait || textToken!=sPGMapLastTextToken))sPGMap.scriptSerial=PGQANext(sPGMap.scriptSerial);
    sPGMapLastWait=sPGMap.scriptWait;sPGMapLastTextToken=textToken;
    if(sPGMap.started) {
        PGMAP_REQUIRE((sPGMap.mapId==33 || sPGMap.mapId==67) && sPGMap.cards<=1 && sPGMap.scene<=2);
        if(sPGMap.phone.ui.active && sPGMap.phone.ui.viewKind==HG_POKEGEAR_MAP && sPGMap.phone.ui.app==2 && sPGMap.phone.ui.ready
            && sPGMap.phone.ui.registeredCards==1 && sPGMap.scene==2)sPGMap.mapReadySeen=1;
    }
    if(!sPGMapPending || !HGMapStationary(&sPGMap))return;
    uint32_t request=sPGMapPending;sPGMapPending=0;
    if(request==HG_PG_MAP_START){PGMAP_REQUIRE(sPGMap.phone.compatibleSave && sPGMap.mapId==33 && sPGMap.x==668 && sPGMap.z==397
        && !sPGMap.scene && !sPGMap.cards && !sPGMap.phone.ringActive && !sPGMap.phone.panicFlag);sPGMap.started=1;}
    else if(request==HG_PG_MAP_FINISH){PGMAP_REQUIRE(sPGMap.started && sPGMap.cards==1 && sPGMap.scene==2 && sPGMap.mapReadySeen
        && sPGMap.parentEntries==1 && sPGMap.parentExits==1 && sPGMap.lastApp==2 && !sPGMap.battleLive
        && sPGMap.battleLaunches==sPGMap.battleReturns && !sPGMap.phone.ui.active);sPGMap.done=1;}
}
void PokegearQAParentObserved(const PokegearAppData *owner,int phase) {
    PGMapBaseParentObserved(owner,phase);if(!sPGMap.started)return;
    PGMAP_REQUIRE(owner && sPGQAField && owner->saveData==sPGQAField->saveData && owner->args && !owner->args->isScriptedLaunch
        && owner->args->mapID==67 && owner->registeredCards==1 && (owner->app==3 || owner->app==2));
    if(owner!=sPGMapParent){PGMAP_REQUIRE(!sPGMapParent && !sPGMap.parentEntries);sPGMapParent=owner;++sPGMap.parentEntries;}
    PGQAOwnerFacts(owner);
}
void PokegearQAParentExitObserved(const PokegearAppData *owner) {
    PGMapBaseParentExitObserved(owner);if(!sPGMap.started)return;
    PGMAP_REQUIRE(owner && owner==sPGMapParent && owner->app==2 && sPGMap.parentEntries==1 && !sPGMap.parentExits);
    PGQAOwnerFacts(owner);sPGMapParent=NULL;++sPGMap.parentExits;
}
void __real_Battle_LaunchApp(FieldSystem *field,BattleSetup *setup);
void __wrap_Battle_LaunchApp(FieldSystem *field,BattleSetup *setup) {
    if(sPGMap.started && !sPGMap.done) {
        if(!field || field!=sPGQAField || !setup || sPGMapBattle || setup->battleType || setup->battleSpecial) {
            if(!sPGMap.error)sPGMap.error=__LINE__;
        } else {sPGMapBattle=setup;sPGMap.battleType=setup->battleType;sPGMap.battleSpecial=setup->battleSpecial;
            sPGMap.battleLive=1;++sPGMap.battleLaunches;}
    }
    __real_Battle_LaunchApp(field,setup);
}
void __real_BattleSetup_Delete(BattleSetup *setup);
void __wrap_BattleSetup_Delete(BattleSetup *setup) {
    if(setup && setup==sPGMapBattle) {
        sPGMap.battleOutcome=setup->winFlag;sPGMapBattle=NULL;sPGMap.battleLive=0;++sPGMap.battleReturns;
        if(setup->winFlag!=BATTLE_OUTCOME_PLAYER_FLED && !sPGMap.error)sPGMap.error=__LINE__;
    }
    __real_BattleSetup_Delete(setup);
}
const HGNativePokegearQAState *HGNativePokegearQAGetState(void) {sPGMap.phone=*PGMapBaseGetState();return &sPGMap.phone;}
uint32_t HGNativePokegearQARequest(uint32_t revision,uint32_t request) {
    if(!revision || revision!=sPGMap.revision || sPGMap.error || sPGMapPending || !HGMapStationary(&sPGMap) || sPGMap.phone.ui.active)return 0;
    if(request==HG_PG_MAP_START ? sPGMap.started || !sPGMap.phone.compatibleSave
        : request==HG_PG_MAP_FINISH ? !sPGMap.started || sPGMap.done || !sPGMap.mapReadySeen : 1)return 0;
    sPGMapPending=request;return 1;
}
