// SPDX-License-Identifier: GPL-3.0-or-later
// This independent case keeps the existing Phone33 observer and its original bodies intact.
#define HGNativePokegearQAGetState PGMapBaseGetState
#define HGNativePokegearQARequest PGMapBaseRequest
#define PokegearQAFieldObserved PGMapBaseFieldObserved
#define PokegearQAParentObserved PGMapBaseParentObserved
#define PokegearQAParentExitObserved PGMapBaseParentExitObserved
#ifdef HG_PG_ELM_EXTENSION
#define PokegearQAPhoneObserved PGElmBasePhoneObserved
#define PokegearQACallSelected PGElmBaseCallSelected
#define PokegearQACallObserved PGElmBaseCallObserved
#define PokegearQACallCompleted PGElmBaseCallCompleted
#endif
#include "pokegear_qa_guest.inc"
#undef HGNativePokegearQAGetState
#undef HGNativePokegearQARequest
#undef PokegearQAFieldObserved
#undef PokegearQAParentObserved
#undef PokegearQAParentExitObserved
#ifdef HG_PG_ELM_EXTENSION
#undef PokegearQAPhoneObserved
#undef PokegearQACallSelected
#undef PokegearQACallObserved
#undef PokegearQACallCompleted
#endif
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
#ifdef HG_PG_ELM_EXTENSION
static uint32_t sPGElmPending,sPGElmLastWait;
static const PokegearPhoneCallContext *sPGElmCall;
static BOOL PGElmRunning(void) {return (sPGQA.coverage&HG_PG_ELM_BEGUN) && !(sPGQA.coverage&HG_PG_ELM_DONE);}
#endif
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
#ifdef HG_PG_ELM_EXTENSION
static BOOL PGElmCallOwner(const PokegearPhoneCallContext *call) {
    return call && sPGMapParent && call->phoneApp && call->phoneApp->pokegear==sPGMapParent
        && sPGMapParent->childAppdata==call->phoneApp && call->saveData && sPGQAField
        && call->saveData==sPGQAField->saveData && call->saveData==sPGMapParent->saveData;
}
static BOOL PGElmCallRootIdentity(const PokegearPhoneCallContext *call) {
    return PGElmCallOwner(call) && call->state.callerID==PHONE_CONTACT_PROF__ELM && call->state.isIncomingCall==1
        && call->state.isScriptedCall==2 && !call->state.predefinedScriptID && !call->state.scriptType
        && call->state.scriptID==PHONE_SCRIPT_002 && call->state.scriptDef
        && call->state.scriptDef->scriptType==PHONESCRIPTTYPE_NONE && !call->state.scriptDef->param0 && !call->state.scriptDef->param1;
}
static void PGElmFieldObserved(void) {
    if(PGElmRunning()) {
        if(sPGMap.mapId==143 && sPGMap.x==4 && sPGMap.z==9)sPGQA.coverage|=HG_PG_ELM_DOOR;
        if(sPGMap.mapId==34 && sPGMap.x==568 && sPGMap.z==295)sPGQA.coverage|=HG_PG_ELM_ARRIVED;
        if(sPGMap.mapId==34 && sPGMap.scriptActive && sPGMap.scriptId==2 && sPGMr.routeScene==2 && sPGQA.panicFlag)
            sPGQA.coverage|=HG_PG_ELM_SCRIPT;
    }
    if(!sPGElmPending || !HGMapStationary(&sPGMap))return;
    uint32_t request=sPGElmPending;sPGElmPending=0;
    if(request==HG_PG_ELM_START){PGMAP_REQUIRE(sPGMr.done && HGMrEndpoint(&sPGMap,&sPGMr) && !sPGQA.coverage && !sPGQA.panicFlag);
        sPGQA.coverage=HG_PG_ELM_BEGUN|HG_PG_ELM_INDOOR;}
    else if(request==HG_PG_ELM_FINISH){sPGMap.phone=sPGQA;PGMAP_REQUIRE(HGElmEndpoint(&sPGMap,&sPGMr));sPGQA.coverage|=HG_PG_ELM_DONE;}
}
void PokegearQAPhoneObserved(const PokegearPhoneAppData *phone,int phase) {
    PGElmBasePhoneObserved(phone,phase);if(!PGElmRunning())return;
    PGMAP_REQUIRE(phone && phone->pokegear==sPGMapParent && phone->pokegear->childAppdata==phone);
    PGQAOwnerFacts(phone->pokegear);
    if(sPGQA.ui.viewActive && sPGQA.ui.viewKind==HG_POKEGEAR_PHONE && sPGQAPhoneGeneration!=sPGQA.ui.ownerGeneration){
        sPGQAPhoneGeneration=sPGQA.ui.ownerGeneration;++sPGQA.phoneEntries;}
    sPGQA.contactCount=phone->numContacts;sPGQA.contactCursor=phone->contactListUI.cursorPos;sPGQA.selectedContact=phone->callerID;
}
void PokegearQACallSelected(const PokegearPhoneCallContext *call) {
    PGElmBaseCallSelected(call);if(!PGElmRunning())return;
    PGMAP_REQUIRE(PGElmCallRootIdentity(call) && !sPGElmCall && !sPGQA.callSelections && (sPGQA.coverage&31)==31);
    sPGElmCall=call;sPGQA.callTarget=call->state.callerID;sPGQA.callEffectType=call->state.scriptDef->scriptType;
    sPGQA.callScriptID=call->state.scriptID;sPGQA.callScriptType=call->state.scriptType;
    sPGQA.callIncoming=call->state.isIncomingCall;sPGQA.callScripted=call->state.isScriptedCall;
    ++sPGQA.callSelections;sPGQA.coverage|=HG_PG_ELM_SELECTED;
}
void PokegearQACallObserved(const PokegearPhoneCallContext *call) {
    PGElmBaseCallObserved(call);if(!PGElmRunning())return;
    PGMAP_REQUIRE(call==sPGElmCall && PGElmCallRootIdentity(call) && sPGQA.callSelections==1 && !sPGQA.callCompletions);
    sPGQA.callMainState=call->state.mainState;sPGQA.callScriptState=call->state.scriptState;
    sPGQA.callTextWait=call->state.mainState==1 && call->state.scriptState==3 ? PGQAPrinterWait(gSystem.printTaskQueue,call->textPrinter,call->phoneCallMsgWindow) : 0;
    if(sPGQA.callTextWait && sPGQA.callTextWait!=sPGElmLastWait)sPGQA.callTextSerial=PGQANext(sPGQA.callTextSerial);
    sPGElmLastWait=sPGQA.callTextWait;sPGQA.callAwaitHangup=call->state.mainState==2;
}
void PokegearQACallCompleted(const PokegearPhoneAppData *phone) {
    PGElmBaseCallCompleted(phone);if(!PGElmRunning())return;
    PGMAP_REQUIRE(phone && phone->pokegear==sPGMapParent && phone->pokegear->childAppdata==phone
        && phone->callerID==PHONE_CONTACT_PROF__ELM && sPGElmCall && sPGQA.callSelections==1 && !sPGQA.callCompletions);
    sPGElmCall=NULL;++sPGQA.callCompletions;sPGQA.callAwaitHangup=0;sPGQA.callTextWait=0;sPGQA.coverage|=HG_PG_ELM_COMPLETED;
}
#endif
#ifdef HG_PG_RIVAL_EXTENSION
#include "constants/trainers.h"
#include "constants/trainer_class.h"
static HGPokegearFirstRivalQAState sPGRival={.version=HG_PG_RIVAL_VERSION,.byteSize=sizeof(HGPokegearFirstRivalQAState),.revision=1};
static const BattleSetup *sPGRivalBattle;
static uint32_t sPGRivalPending;
static BOOL PGRivalRunning(void) {return sPGRival.started && !sPGRival.done;}
void HGNativeFirstRivalQAStatWaitObserved(const void *owner,uint32_t state,uint32_t kind,uint32_t ready) {
    if(!PGRivalRunning() || !sPGRival.battleLive)return;
    uint32_t token=(uint32_t)(uaddr)owner;
    if(!kind){if(token==sPGRival.statOwner){sPGRival.statReady=0;sPGRival.statOwner=0;sPGRival.statState=state;sPGRival.statKind=0;}return;}
    PGMAP_REQUIRE(token && (kind==1 ? state==HG_PG_RIVAL_STAT_DIFF : kind==2 && state==HG_PG_RIVAL_STAT_TRUE));
    if(ready){
        if(!sPGRival.statReady){
            PGMAP_REQUIRE(sPGRival.statWaits<HG_PG_RIVAL_STAT_CAP && kind==sPGRival.statWaits+1);
            sPGRival.statSerial=PGQANext(sPGRival.statSerial);++sPGRival.statWaits;
        } else PGMAP_REQUIRE(sPGRival.statOwner==token && sPGRival.statState==state && sPGRival.statKind==kind);
        sPGRival.statOwner=token;sPGRival.statState=state;sPGRival.statKind=kind;sPGRival.statReady=1;
    } else {
        PGMAP_REQUIRE(HGRivalStatLease(&sPGRival,token,state,kind,sPGRival.statSerial));
        sPGRival.statReady=0;sPGRival.statRetiredSerial=sPGRival.statSerial;++sPGRival.statRetirements;
    }
    sPGRival.revision=PGQANext(sPGRival.revision);
}
static void PGRivalFieldObserved(FieldSystem *field) {
    SaveVarsFlags *flags=Save_VarsFlags_Get(field->saveData);
    sPGRival.revision=PGQANext(sPGRival.revision);
    sPGRival.battleResult=*Save_VarsFlags_GetVarAddr(flags,VAR_BATTLE_RESULT);
    sPGRival.metPasserby=Save_VarsFlags_CheckFlagInArray(flags,FLAG_MET_PASSERBY_BOY);
    sPGRival.rivalHidden=Save_VarsFlags_CheckFlagInArray(flags,FLAG_HIDE_CHERRYGROVE_RIVAL);
    if(PGRivalRunning() && sPGMap.mapId==67 && sPGMap.scene==3 && sPGMap.scriptActive && sPGMap.scriptId==4)sPGRival.scriptSeen=1;
    if(!sPGRivalPending || !HGMapStationary(&sPGMap))return;
    uint32_t request=sPGRivalPending;sPGRivalPending=0;
    if(request==HG_PG_RIVAL_START){PGMAP_REQUIRE(HGElmEndpoint(&sPGMap,&sPGMr) && sPGQA.coverage==255
        && !sPGRival.started && !sPGRival.metPasserby && !sPGRival.rivalHidden);sPGRival.started=1;}
    else if(request==HG_PG_RIVAL_FINISH){PGMAP_REQUIRE(HGRivalEndpoint(&sPGMap,&sPGMr,&sPGRival));sPGRival.done=1;}
}
const HGPokegearFirstRivalQAState *HGNativeFirstRivalQAGetState(void) {return &sPGRival;}
#endif
static uint32_t PGMapTextWait(FieldSystem *field,uint32_t *script,uaddr *textToken) {
    *script=0;*textToken=0;
    if(!field || !field->taskman || field->taskman->func!=Task_RunScripts || !field->bgConfig
        || !FieldSystem_IsFieldMapRunning(field) || FieldSystem_IsChildAppRunning(field) || !IsPaletteFadeFinished())return 0;
    ScriptEnvironment *env=TaskManager_GetEnvironment(field->taskman);
    if(!env || env->check!=Unk80_10_C_MAGIC || env->state!=1 || !env->activeScriptContextCount || env->activeScriptContextCount>3)return 0;
    *script=env->activeScriptNumber;
    if((*script!=2 && *script!=3
#ifdef HG_PG_RIVAL_EXTENSION
        && !(*script==4 && PGRivalRunning() && field->location && field->location->mapId==67 && sPGMap.scene==3)
#endif
        ) || !env->msgWindowOpen || env->activeMovementCounter || !env->window.pixelBuffer
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
#ifdef HG_PG_RIVAL_EXTENSION
        PGMAP_REQUIRE(HGRivalMapAllowed(&sPGMap,&sPGMr,&sPGRival));
#else
        PGMAP_REQUIRE(HGMrMapAllowed(&sPGMap,&sPGMr));
#endif
#else
        PGMAP_REQUIRE((sPGMap.mapId==33 || sPGMap.mapId==67) && sPGMap.cards<=1 && sPGMap.scene<=2);
#endif
        if(sPGMap.phone.ui.active && sPGMap.phone.ui.viewKind==HG_POKEGEAR_MAP && sPGMap.phone.ui.app==2 && sPGMap.phone.ui.ready
            && sPGMap.phone.ui.registeredCards==1 && sPGMap.scene==2)sPGMap.mapReadySeen=1;
    }
#ifdef HG_PG_MR_EXTENSION
    PGMrFieldObserved(field);
#endif
#ifdef HG_PG_ELM_EXTENSION
    PGElmFieldObserved();
#endif
#ifdef HG_PG_RIVAL_EXTENSION
    PGRivalFieldObserved(field);
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
#ifdef HG_PG_ELM_EXTENSION
    if(PGElmRunning()) {
        PGMAP_REQUIRE(owner && sPGQAField && owner->saveData==sPGQAField->saveData && owner->args
            && owner->args->mapID==34 && owner->registeredCards==1 && owner->app==3 && sPGMr.houseScene==1 && sPGQA.panicFlag);
        if(owner!=sPGMapParent){PGMAP_REQUIRE(!sPGMapParent && sPGMap.parentEntries==1 && sPGMap.parentExits==1
            && owner->args->isScriptedLaunch==1 && owner->args->callerId==PHONE_CONTACT_PROF__ELM
            && owner->args->isScriptedCall==2 && !owner->args->callScriptID);sPGMapParent=owner;++sPGMap.parentEntries;}
        PGQAOwnerFacts(owner);return;
    }
#endif
    PGMAP_REQUIRE(owner && sPGQAField && owner->saveData==sPGQAField->saveData && owner->args && !owner->args->isScriptedLaunch
        && owner->args->mapID==67 && owner->registeredCards==1 && (owner->app==3 || owner->app==2));
    if(owner!=sPGMapParent){PGMAP_REQUIRE(!sPGMapParent && !sPGMap.parentEntries);sPGMapParent=owner;++sPGMap.parentEntries;}
    PGQAOwnerFacts(owner);
}
void PokegearQAParentExitObserved(const PokegearAppData *owner) {
    PGMapBaseParentExitObserved(owner);if(!sPGMap.started)return;
#ifdef HG_PG_ELM_EXTENSION
    if(PGElmRunning()) {PGMAP_REQUIRE(owner && owner==sPGMapParent && owner->app==3 && sPGMap.parentEntries==2
        && sPGMap.parentExits==1 && sPGQA.callCompletions==1);PGQAOwnerFacts(owner);sPGMapParent=NULL;++sPGMap.parentExits;return;}
#endif
    PGMAP_REQUIRE(owner && owner==sPGMapParent && owner->app==2 && sPGMap.parentEntries==1 && !sPGMap.parentExits);
    PGQAOwnerFacts(owner);sPGMapParent=NULL;++sPGMap.parentExits;
}
void __real_Battle_LaunchApp(FieldSystem *field,BattleSetup *setup);
void __wrap_Battle_LaunchApp(FieldSystem *field,BattleSetup *setup) {
#ifdef HG_PG_RIVAL_EXTENSION
    if(PGRivalRunning() && setup && setup->battleType) {
        if(!field || field!=sPGQAField || sPGRivalBattle || sPGMapBattle || sPGRival.trainerLaunches || !sPGRival.scriptSeen
            || sPGMap.mapId!=67 || sPGMap.scene!=3 || sPGMap.x!=575 || sPGMap.z!=400
            || setup->battleType!=(BATTLE_TYPE_TRAINER|BATTLE_TYPE_11) || setup->battleSpecial
            || setup->trainerId[1]!=TRAINER_PASSERBY_BOY_2 || setup->trainer[1].data.trainerClass!=TRAINERCLASS_PASSERBY
            || !setup->party[0] || !setup->party[1] || Party_GetCount(setup->party[0])!=1 || Party_GetCount(setup->party[1])!=1) {
            if(!sPGMap.error)sPGMap.error=__LINE__;
        } else {
            Pokemon *player=Party_GetMonByIndex(setup->party[0],0),*opponent=Party_GetMonByIndex(setup->party[1],0);
            sPGRival.playerSpecies=GetMonData(player,MON_DATA_SPECIES,NULL);sPGRival.playerLevel=GetMonData(player,MON_DATA_LEVEL,NULL);
            sPGRival.opponentSpecies=GetMonData(opponent,MON_DATA_SPECIES,NULL);sPGRival.opponentLevel=GetMonData(opponent,MON_DATA_LEVEL,NULL);
            if(sPGRival.playerSpecies!=152 || sPGRival.playerLevel!=5 || sPGRival.opponentSpecies!=155 || sPGRival.opponentLevel!=5) {
                if(!sPGMap.error)sPGMap.error=__LINE__;
            } else {sPGRivalBattle=setup;sPGRival.battleType=setup->battleType;sPGRival.battleSpecial=setup->battleSpecial;
                sPGRival.trainerId=setup->trainerId[1];sPGRival.trainerClass=setup->trainer[1].data.trainerClass;
                sPGRival.battleLive=1;++sPGRival.trainerLaunches;}
        }
        __real_Battle_LaunchApp(field,setup);return;
    }
#endif
    if(sPGMap.started && !sPGMap.done
#ifdef HG_PG_MR_EXTENSION
        || (sPGMap.started && PGMrRunning())
#endif
#ifdef HG_PG_RIVAL_EXTENSION
        || (sPGMap.started && PGRivalRunning())
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
#ifdef HG_PG_RIVAL_EXTENSION
    if(setup && setup==sPGRivalBattle) {
        sPGRival.outcome=setup->winFlag;sPGRivalBattle=NULL;sPGRival.battleLive=0;++sPGRival.trainerReturns;
        if(setup->winFlag!=BATTLE_OUTCOME_WIN && setup->winFlag!=BATTLE_OUTCOME_LOSE && !sPGMap.error)sPGMap.error=__LINE__;
    }
#endif
    if(setup && setup==sPGMapBattle) {
        sPGMap.battleOutcome=setup->winFlag;sPGMapBattle=NULL;sPGMap.battleLive=0;++sPGMap.battleReturns;
        if(setup->winFlag!=BATTLE_OUTCOME_PLAYER_FLED && !sPGMap.error)sPGMap.error=__LINE__;
    }
    __real_BattleSetup_Delete(setup);
}
const HGNativePokegearQAState *HGNativePokegearQAGetState(void) {sPGMap.phone=*PGMapBaseGetState();return &sPGMap.phone;}
uint32_t HGNativePokegearQARequest(uint32_t revision,uint32_t request) {
    if(!revision || revision!=sPGMap.revision || sPGMap.error || sPGMapPending || !HGMapStationary(&sPGMap) || sPGMap.phone.ui.active)return 0;
#ifdef HG_PG_RIVAL_EXTENSION
    if(request==HG_PG_RIVAL_START || request==HG_PG_RIVAL_FINISH){
        if(sPGRivalPending || sPGRival.error || sPGMap.scriptActive || sPGMap.battleLive || sPGRival.battleLive
            || (request==HG_PG_RIVAL_START ? sPGRival.started || sPGQA.coverage!=255 || !HGElmEndpoint(&sPGMap,&sPGMr)
                : !PGRivalRunning() || !HGRivalEndpoint(&sPGMap,&sPGMr,&sPGRival)))return 0;
        sPGRivalPending=request;return 1;
    }
#endif
#ifdef HG_PG_ELM_EXTENSION
    if(request==HG_PG_ELM_START || request==HG_PG_ELM_FINISH){
        if(sPGElmPending || sPGMap.scriptActive || sPGMap.battleLive
            || (request==HG_PG_ELM_START ? sPGQA.coverage || !sPGMr.done || !HGMrEndpoint(&sPGMap,&sPGMr)
                : !PGElmRunning() || !HGElmEndpoint(&sPGMap,&sPGMr)))return 0;
        sPGElmPending=request;return 1;
    }
#endif
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
