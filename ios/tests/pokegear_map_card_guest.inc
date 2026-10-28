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
#ifdef HG_PG_MR_EXTENSION
#include "bag.h"
#include "party.h"
#include "pokedex.h"
#include "pokemon.h"
#include "constants/items.h"
#include "constants/heap.h"
#include "constants/phone_contacts.h"
static HGPokegearMrPokemonQAState sPGMr={.version=HG_PG_MR_VERSION,.byteSize=sizeof(HGPokegearMrPokemonQAState),.revision=1};
static uint32_t sPGMrPending;
static BOOL PGMrRunning(void) {return sPGMr.started && !sPGMr.done;}
static void PGMrFieldObserved(FieldSystem *field) {
    if(!sPGMrPending && !sPGMr.started)return;
    SaveData *save=field->saveData;SaveVarsFlags *flags=Save_VarsFlags_Get(save);SavePokegear *gear=SaveData_Pokegear_Get(save);
    sPGMr.revision=PGQANext(sPGMr.revision);
    sPGMr.routeScene=*Save_VarsFlags_GetVarAddr(flags,VAR_SCENE_ROUTE_30_OW);
    sPGMr.houseScene=*Save_VarsFlags_GetVarAddr(flags,VAR_SCENE_MR_POKEMONS_HOUSE);
    sPGMr.embeddedTower=*Save_VarsFlags_GetVarAddr(flags,VAR_SCENE_EMBEDDED_TOWER);
    sPGMr.apricornBox=Bag_GetQuantity(Save_Bag_Get(save),ITEM_APRICORN_BOX,HEAP_ID_FIELD2);
    sPGMr.mysteryEgg=Bag_GetQuantity(Save_Bag_Get(save),ITEM_MYSTERY_EGG,HEAP_ID_FIELD2);
    sPGMr.pokedexEnabled=Pokedex_IsEnabled(Save_Pokedex_Get(save));
    sPGMr.oakRegistered=SavePokegear_IsNumberRegistered(gear,PHONE_CONTACT_PROF__OAK)!=0xFF;
    Party *party=SaveArray_Party_Get(save);int count=Party_GetCount(party);sPGMr.partyHealed=count>0 && count<=6;
    for(int i=0;i<count && i<6;i++){Pokemon *mon=Party_GetMonByIndex(party,i);
        if(!GetMonData(mon,MON_DATA_MAX_HP,NULL) || GetMonData(mon,MON_DATA_HP,NULL)!=GetMonData(mon,MON_DATA_MAX_HP,NULL)
            || GetMonData(mon,MON_DATA_STATUS,NULL))sPGMr.partyHealed=0;}
    if(sPGMr.started){
        if(sPGMap.mapId==34 && sPGMap.scriptActive && sPGMap.scriptId==3)sPGMr.apricornScriptSeen=1;
        if(sPGMap.mapId==34 && sPGMr.routeScene==1 && sPGMr.apricornBox==1)sPGMr.apricornCompletedSeen=1;
        if(sPGMap.mapId==143 && sPGMap.x==4 && sPGMap.z==9)sPGMr.doorAnchorSeen=1;
        if(sPGMap.mapId==143 && sPGMap.scriptActive && sPGMap.scriptId==2)sPGMr.houseScriptSeen=1;
    }
    if(!sPGMrPending || !HGMapStationary(&sPGMap))return;
    uint32_t request=sPGMrPending;sPGMrPending=0;
    if(request==HG_PG_MR_START){PGMAP_REQUIRE(sPGMap.done && sPGMap.mapId==67 && sPGMap.x==550 && sPGMap.z==385
        && sPGMap.scene==2 && !sPGMr.routeScene && !sPGMr.houseScene && sPGMr.embeddedTower!=1
        && !sPGMr.apricornBox && !sPGMr.mysteryEgg && !sPGMr.pokedexEnabled && !sPGMr.oakRegistered);sPGMr.started=1;}
    else if(request==HG_PG_MR_FINISH){PGMAP_REQUIRE(HGMrEndpoint(&sPGMap,&sPGMr));sPGMr.done=1;}
}
const HGPokegearMrPokemonQAState *HGNativeMrPokemonQAGetState(void) {return &sPGMr;}
static ScriptContext *PGMrTextContext(FieldSystem *field,ScriptEnvironment *env,ScriptContext *parent) {
    if(!parent || parent!=env->scriptContexts[0] || parent->id || parent->fieldSystem!=field || parent->taskman!=field->taskman
        || parent->mode!=SCRIPT_MODE_NATIVE || !parent->native_ptr)return NULL;
    if(env->activeScriptContextCount==1 && !env->stdScriptWaitFlags && !env->scriptContexts[1] && !env->scriptContexts[2])return parent;
    if(!sPGMr.started || env->activeScriptContextCount!=2 || env->stdScriptWaitFlags!=1 || env->scriptContexts[2] || !field->location)return NULL;
    u32 map=field->location->mapId;
    if(!((map==34 && env->activeScriptNumber==3 && !sPGMr.routeScene && !sPGMr.houseScene)
        || (map==143 && env->activeScriptNumber==2 && !sPGMr.houseScene && sPGMr.apricornCompletedSeen && sPGMr.doorAnchorSeen)))return NULL;
    ScriptContext *child=env->scriptContexts[1];
    return child && child!=parent && child->id==1 && child->fieldSystem==field && child->taskman==field->taskman
        && child->mode==SCRIPT_MODE_NATIVE && child->native_ptr ? child : NULL;
}
#endif
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
#ifdef HG_PG_MR_EXTENSION
    ctx=PGMrTextContext(field,env,ctx);
#endif
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
#ifdef HG_PG_MR_EXTENSION
        PGMAP_REQUIRE(HGMrMapAllowed(&sPGMap,&sPGMr));
#else
        PGMAP_REQUIRE((sPGMap.mapId==33 || sPGMap.mapId==67) && sPGMap.cards<=1 && sPGMap.scene<=2);
#endif
        if(sPGMap.phone.ui.active && sPGMap.phone.ui.viewKind==HG_POKEGEAR_MAP && sPGMap.phone.ui.app==2 && sPGMap.phone.ui.ready
            && sPGMap.phone.ui.registeredCards==1 && sPGMap.scene==2)sPGMap.mapReadySeen=1;
    }
#ifdef HG_PG_MR_EXTENSION
    PGMrFieldObserved(field);
#endif
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
    if(sPGMap.started && !sPGMap.done
#ifdef HG_PG_MR_EXTENSION
        || (sPGMap.started && PGMrRunning())
#endif
        ) {
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
#ifdef HG_PG_MR_EXTENSION
    if(request==HG_PG_MR_START || request==HG_PG_MR_FINISH){
        if(sPGMrPending || sPGMr.error || !sPGMap.done || sPGMap.scriptActive || sPGMap.battleLive
            || (request==HG_PG_MR_START ? sPGMr.started || sPGMap.mapId!=67 || sPGMap.x!=550 || sPGMap.z!=385 || sPGMap.scene!=2
                : !sPGMr.started || sPGMr.done || !HGMrEndpoint(&sPGMap,&sPGMr)))return 0;
        sPGMrPending=request;return 1;
    }
#endif
    if(request==HG_PG_MAP_START ? sPGMap.started || !sPGMap.phone.compatibleSave
        : request==HG_PG_MAP_FINISH ? !sPGMap.started || sPGMap.done || !sPGMap.mapReadySeen : 1)return 0;
    sPGMapPending=request;return 1;
}
