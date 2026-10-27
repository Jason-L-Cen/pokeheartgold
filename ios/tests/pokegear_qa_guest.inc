// SPDX-License-Identifier: GPL-3.0-or-later
// Private resident field-system copy only. Expected setters never receive a live save pointer.
#define HG_POKEGEAR_QA_GUEST 1
#include "pokegear_qa.h"
#include "application/pokegear/configure/pokegear_configure_internal.h"
#include "application/pokegear/phone/phone_internal.h"
#include "bag.h"
#include "constants/maps.h"
#include "easy_chat.h"
#include "field_system.h"
#include "game_stats.h"
#include "gear_phone_ring_manager.h"
#include "heap.h"
#include "party.h"
#include "player_data.h"
#include "pokemon_storage_system.h"
#include "save_arrays.h"
#include "save_local_field_data.h"
#include "save_misc_data.h"
#include "save_trainer_card.h"
#include "save_vars_flags.h"
#include "screen_fade.h"
#include "start_menu.h"
#include "sys_flags.h"
#include "system.h"
#include "task.h"
#include "text.h"
#include "touchscreen.h"
#include <stddef.h>
#include <string.h>
extern const TouchscreenHitbox gTouchscreenMenuIconHitboxes[];
_Static_assert(sizeof(HGNativePokegearUIState)==48 && sizeof(HGYNBootFacts)==44,"shipping packet and canonical boot");
_Static_assert(sizeof(SavePokegear)<=HG_PG_QA_GEAR_CAPACITY,"complete original Gear witness capacity");
static HGNativePokegearQAState sPGQA={.version=HG_PG_QA_VERSION,.byteSize=sizeof(HGNativePokegearQAState),.pokegearSlot=UINT32_MAX};
static FieldSystem *sPGQAField;
static const void *sPGQABootOwner;
static const PokegearAppData *sPGQAParent;
static u32 sPGQARequest,sPGQAPhoneGeneration,sPGQAConfigureGeneration,sPGQAMoney,sPGQALastTextWait;
static Party sPGQAParty;
static PCStorage sPGQAPC;
static Bag sPGQABag;
static SaveVarsFlags sPGQAFlags;
static SaveEasyChat sPGQAEasy;
static struct SaveTrainerCard sPGQACard;
static SAVE_MISC_DATA sPGQAMisc;
static SavePokegear sPGQAExpectedGear;
static GameStats *sPGQAExpectedStats;
static inline BOOL PokegearQAExpectConfigureExit(SavePokegear *expected,const SavePokegear *live,u32 skin) {
    if(!expected || !live || expected==live || skin>=6)return FALSE;
    Pokegear_SetSkin(expected,skin);return TRUE;
}
static inline BOOL PokegearQAExpectParentExit(SavePokegear *expected,const SavePokegear *live,u32 app) {
    if(!expected || !live || expected==live || (app!=0 && app!=3))return FALSE;
    SavePokegear_SetLastUsedApp(expected,app);return TRUE;
}
static u32 PGQANext(u32 value){return value==UINT32_MAX ? 1 : value+1;}
static u32 PGQAPrinterWait(SysTaskQueue *queue,u32 id,const Window *window) {
    if(!queue || !window || id>=MAX_TEXT_PRINTERS || !queue->limit || queue->limit>MAX_TEXT_PRINTERS || queue->activeCount>queue->limit)return 0;
    if(TextPrinterIsWaitingForInput(id,window))return 2;
    // This phone-only owner excludes the trade draw task; all other print-queue data is original TextPrinter.
    SysTask *task=queue->headSentinel.next;
    for(u32 i=0;task && task!=&queue->headSentinel && i<queue->limit;++i,task=task->next) {
        if(task->queue!=queue)return 0;
        const TextPrinter *printer=SysTask_GetData(task);
        if(printer && printer->id==id && printer->template.window==window && printer->active && !printer->callback
            && !printer->callbackResult && !printer->callbackParam && printer->state==3)return 3;
    }
    return 0;
}
#define PGQA_REQUIRE(value) do{if(!(value)){if(!sPGQA.error)sPGQA.error=__LINE__;return;}}while(0)
void NPCYNBootPublish(const void *owner,const HGYNBootFacts *facts) {
    if(!owner || !facts)return;
    u32 generation=sPGQA.boot.ownerGeneration,revision=PGQANext(sPGQA.boot.revision);
    if(owner!=sPGQABootOwner || !sPGQA.boot.active)generation=PGQANext(generation);
    sPGQABootOwner=owner;sPGQA.boot=*facts;sPGQA.boot.active=1;sPGQA.boot.ownerGeneration=generation;
    sPGQA.boot.revision=revision;sPGQA.revision=PGQANext(sPGQA.revision);
}
void NPCYNBootRetire(const void *owner) {
    if(!owner || owner!=sPGQABootOwner)return;
    sPGQABootOwner=NULL;sPGQA.boot.active=sPGQA.boot.ready=0;sPGQA.boot.revision=PGQANext(sPGQA.boot.revision);
    sPGQA.revision=PGQANext(sPGQA.revision);
}
static BOOL PGQABaseline(FieldSystem *field) {
    if(!field || !field->saveData || !field->phoneRingManager || GearPhoneRingManager_IsRinging(field->phoneRingManager))return FALSE;
    SaveData *save=field->saveData;SavePokegear *gear=SaveData_Pokegear_Get(save);SaveVarsFlags *flags=Save_VarsFlags_Get(save);
    if(!CheckGotPokegear(flags) || Save_VarsFlags_CheckFlagInArray(flags,FLAG_GOT_ELMS_PANIC_CALL)
        || LocalFieldData_GetCurrentPosition(Save_LocalFieldData_Get(save))->mapId!=MAP_ROUTE_29
        || Pokegear_GetRegisteredCardsArray(gear) || SavePokegear_GetLastUsedApp(gear)!=3 || Pokegear_GetSkin(gear)!=0
        || gear->phoneContacts[0].id!=PHONE_CONTACT_MOTHER || gear->phoneContacts[1].id!=PHONE_CONTACT_PROF__ELM)return FALSE;
    for(u32 i=2;i<NUM_PHONE_CONTACTS;++i)if(gear->phoneContacts[i].id!=PHONE_CONTACT_NONE)return FALSE;
    return TRUE;
}
static void PGQALedger(void) {
    if(!sPGQA.started || sPGQA.stage==HG_PG_QA_DONE || sPGQA.error)return;
    PGQA_REQUIRE(sPGQAField && sPGQAField->saveData && sPGQAExpectedStats);
    SaveData *save=sPGQAField->saveData;SavePokegear *gear=SaveData_Pokegear_Get(save);
    sPGQA.partyUnchanged=!memcmp(&sPGQAParty,SaveArray_Party_Get(save),sizeof(sPGQAParty));
    sPGQA.pcUnchanged=!memcmp(&sPGQAPC,SaveArray_PCStorage_Get(save),sizeof(sPGQAPC));
    sPGQA.bagUnchanged=!memcmp(&sPGQABag,Save_Bag_Get(save),sizeof(sPGQABag));
    sPGQA.varsFlagsUnchanged=!memcmp(&sPGQAFlags,Save_VarsFlags_Get(save),sizeof(sPGQAFlags));
    sPGQA.easyChatUnchanged=!memcmp(&sPGQAEasy,Save_EasyChat_Get(save),sizeof(sPGQAEasy));
    sPGQA.currentMoney=PlayerProfile_GetMoney(Save_PlayerData_GetProfile(save));sPGQA.moneyUnchanged=sPGQAMoney==sPGQA.currentMoney;
    sPGQA.trainerCardUnchanged=!memcmp(&sPGQACard,Save_TrainerCard_Get(save),sizeof(sPGQACard));
    sPGQA.miscUnchanged=!memcmp(&sPGQAMisc,Save_Misc_Get(save),sizeof(sPGQAMisc));
    memcpy(sPGQA.statsLive,Save_GameStats_Get(save),sPGQA.statsBytes);memcpy(sPGQA.statsExpected,sPGQAExpectedStats,sPGQA.statsBytes);
    sPGQA.statsUnchanged=!memcmp(sPGQA.statsLive,sPGQA.statsExpected,sPGQA.statsBytes);
    memcpy(sPGQA.gearLive,gear,sizeof(*gear));memcpy(sPGQA.gearExpected,&sPGQAExpectedGear,sizeof(sPGQAExpectedGear));
    sPGQA.gearMatched=!memcmp(sPGQA.gearLive,sPGQA.gearExpected,sizeof(*gear));
    sPGQA.savedApp=SavePokegear_GetLastUsedApp(gear);sPGQA.savedSkin=Pokegear_GetSkin(gear);
    sPGQA.expectedSkin=Pokegear_GetSkin(&sPGQAExpectedGear);sPGQA.registeredCards=Pokegear_GetRegisteredCardsArray(gear);
    sPGQA.currentMap=LocalFieldData_GetCurrentPosition(Save_LocalFieldData_Get(save))->mapId;
    PGQA_REQUIRE(sPGQA.partyUnchanged && sPGQA.pcUnchanged && sPGQA.bagUnchanged && sPGQA.varsFlagsUnchanged
        && sPGQA.easyChatUnchanged && sPGQA.moneyUnchanged && sPGQA.trainerCardUnchanged && sPGQA.miscUnchanged
        && sPGQA.statsUnchanged && sPGQA.gearMatched && !sPGQA.registeredCards && sPGQA.currentMap==MAP_ROUTE_29);
}
static void PGQABegin(void) {
    PGQA_REQUIRE(sPGQAField && sPGQA.fieldReady && !sPGQA.started && PGQABaseline(sPGQAField));
    SaveData *save=sPGQAField->saveData;
    PGQA_REQUIRE(SaveArray_Party_sizeof()==sizeof(sPGQAParty) && PCStorage_sizeof()==sizeof(sPGQAPC)
        && Save_Bag_sizeof()==sizeof(sPGQABag) && Save_VarsFlags_sizeof()==sizeof(sPGQAFlags)
        && Save_EasyChat_sizeof()==sizeof(sPGQAEasy) && SaveData_Pokegear_sizeof()==sizeof(sPGQAExpectedGear)
        && Save_Misc_sizeof()==sizeof(sPGQAMisc));
    sPGQA.statsBytes=GameStats_sizeof();PGQA_REQUIRE(sPGQA.statsBytes && sPGQA.statsBytes<=HG_PG_QA_STATS_CAPACITY);
    sPGQAExpectedStats=Heap_Alloc(HEAP_ID_FIELD2,sPGQA.statsBytes);PGQA_REQUIRE(sPGQAExpectedStats);
    memcpy(sPGQAExpectedStats,Save_GameStats_Get(save),sPGQA.statsBytes);
    memcpy(&sPGQAParty,SaveArray_Party_Get(save),sizeof(sPGQAParty));memcpy(&sPGQAPC,SaveArray_PCStorage_Get(save),sizeof(sPGQAPC));
    memcpy(&sPGQABag,Save_Bag_Get(save),sizeof(sPGQABag));memcpy(&sPGQAFlags,Save_VarsFlags_Get(save),sizeof(sPGQAFlags));
    memcpy(&sPGQAEasy,Save_EasyChat_Get(save),sizeof(sPGQAEasy));memcpy(&sPGQACard,Save_TrainerCard_Get(save),sizeof(sPGQACard));
    memcpy(&sPGQAMisc,Save_Misc_Get(save),sizeof(sPGQAMisc));memcpy(&sPGQAExpectedGear,SaveData_Pokegear_Get(save),sizeof(sPGQAExpectedGear));
    sPGQAMoney=PlayerProfile_GetMoney(Save_PlayerData_GetProfile(save));sPGQA.baselineMoney=sPGQAMoney;sPGQA.gearBytes=sizeof(sPGQAExpectedGear);sPGQA.miscBytes=sizeof(sPGQAMisc);
    sPGQA.baselineSkin=Pokegear_GetSkin(&sPGQAExpectedGear);sPGQA.started=1;sPGQA.stage=HG_PG_QA_RUNNING;PGQALedger();
}
static void PGQAOwnerFacts(const PokegearAppData *owner) {
    sPGQA.ui=*HGNativePokegearUIGetState();sPGQA.menuInputState=owner->menuInputState;sPGQA.cursorInAppSwitchZone=owner->cursorInAppSwitchZone;
    sPGQA.viewFactsGeneration=sPGQA.ui.ownerGeneration;sPGQA.configuredSkin=owner->skin;
    sPGQA.footerApp=UINT32_MAX;
    if(sPGQA.ui.viewActive && sPGQA.ui.ready && owner->cursorInAppSwitchZone && owner->cursorManager && owner->cursorManager->lastCursor) {
        const PokegearCursor *cursor=owner->cursorManager->lastCursor;
        if(cursor->grid && cursor->cursorPos<cursor->count)sPGQA.footerApp=cursor->grid[cursor->cursorPos].appId;
    }
    ++sPGQA.snapshotSerial;sPGQA.revision=PGQANext(sPGQA.revision);
}
void PokegearQAParentObserved(const PokegearAppData *owner,int phase) {
    if(!sPGQA.started)return;
    PGQA_REQUIRE(owner && sPGQAField && owner->saveData==sPGQAField->saveData && owner->args && !owner->args->isScriptedLaunch
        && owner->args->mapID==MAP_ROUTE_29 && !owner->registeredCards && (owner->app==0 || owner->app==3));
    if(owner!=sPGQAParent){PGQA_REQUIRE(!sPGQAParent && sPGQA.parentEntries<2);sPGQAParent=owner;++sPGQA.parentEntries;}
    PGQAOwnerFacts(owner);PGQALedger();
}
void PokegearQAParentExitObserved(const PokegearAppData *owner) {
    if(!sPGQA.started)return;
    PGQA_REQUIRE(owner && owner==sPGQAParent && owner->app==0 && sPGQA.parentExits<sPGQA.parentEntries);
    PGQA_REQUIRE(PokegearQAExpectParentExit(&sPGQAExpectedGear,owner->savePokegear,owner->app));
    ++sPGQA.parentExits;sPGQA.coverage|=16;sPGQAParent=NULL;PGQAOwnerFacts(owner);PGQALedger();
}
void PokegearQAPhoneObserved(const PokegearPhoneAppData *phone,int phase) {
    if(!sPGQA.started)return;
    PGQA_REQUIRE(phone && phone->pokegear==sPGQAParent && phone->pokegear->childAppdata==phone);
    PGQAOwnerFacts(phone->pokegear);
    if(sPGQA.ui.viewActive && sPGQA.ui.viewKind==HG_POKEGEAR_PHONE && sPGQAPhoneGeneration!=sPGQA.ui.ownerGeneration) {
        sPGQAPhoneGeneration=sPGQA.ui.ownerGeneration;++sPGQA.phoneEntries;sPGQA.coverage|=1;
    }
    sPGQA.contactCount=phone->numContacts;sPGQA.contactCursor=phone->contactListUI.cursorPos;sPGQA.selectedContact=phone->callerID;
    for(u32 i=0;i<6;++i)sPGQA.slotContacts[i]=phone->contactListUI.slotData[i].contactID;
    sPGQA.menuCursor=sPGQA.menuTouchMode=UINT32_MAX;
    if(phase==PHONE_MAIN_STATE_CONTEXT_MENU && phone->touchscreenListMenu) {
        sPGQA.menuCursor=phone->touchscreenListMenu->cursorPos;sPGQA.menuTouchMode=phone->touchscreenListMenu->isTouch;
    }
    PGQALedger();
}
void PokegearQAConfigureObserved(const PokegearConfigureAppData *configure,int phase) {
    if(!sPGQA.started)return;
    PGQA_REQUIRE(configure && configure->pokegear==sPGQAParent && configure->pokegear->childAppdata==configure);
    PGQAOwnerFacts(configure->pokegear);
    if(sPGQA.ui.viewActive && sPGQA.ui.viewKind==HG_POKEGEAR_CONFIGURE && sPGQAConfigureGeneration!=sPGQA.ui.ownerGeneration) {
        sPGQAConfigureGeneration=sPGQA.ui.ownerGeneration;++sPGQA.configureEntries;sPGQA.coverage|=2;
    }
    sPGQA.selectedSkin=configure->selectedSkin;sPGQA.configuredSkin=configure->skin;sPGQA.menuCursor=sPGQA.menuTouchMode=UINT32_MAX;
    if(sPGQA.parentEntries==2 && configure->skin==1)sPGQA.coverage|=32;
    if(phase==PGCONF_MAIN_STATE_CONTEXT_MENU && configure->contextMenu) {
        sPGQA.menuCursor=configure->contextMenu->cursorPos;sPGQA.menuTouchMode=configure->contextMenu->isTouch;
    }
    PGQALedger();
}
void PokegearQAConfigureExitObserved(const PokegearConfigureAppData *configure) {
    if(!sPGQA.started)return;
    PGQA_REQUIRE(configure && configure->pokegear==sPGQAParent && configure->pokegear->childAppdata==configure && configure->skin==1);
    PGQA_REQUIRE(PokegearQAExpectConfigureExit(&sPGQAExpectedGear,configure->pokegear->savePokegear,configure->skin));sPGQA.coverage|=8;PGQALedger();
}
void PokegearQACallSelected(const PokegearPhoneCallContext *call) {
    if(!sPGQA.started)return;
    PGQA_REQUIRE(call && sPGQAParent && call->phoneApp && call->phoneApp->pokegear==sPGQAParent && sPGQAParent->childAppdata==call->phoneApp
        && call->saveData==sPGQAParent->saveData && !sPGQA.callSelections
        && call->state.callerID==PHONE_CONTACT_PROF__ELM && !call->state.isIncomingCall && !call->state.isScriptedCall
        && call->state.scriptType==0 && call->state.scriptID==PHONE_SCRIPT_008 && call->state.scriptDef
        && call->state.scriptDef->scriptType==PHONESCRIPTTYPE_NONE && !call->state.scriptDef->param0 && !call->state.scriptDef->param1);
    sPGQA.callTarget=call->state.callerID;sPGQA.callEffectType=call->state.scriptDef->scriptType;sPGQA.callScriptID=call->state.scriptID;
    sPGQA.callScriptType=call->state.scriptType;sPGQA.callIncoming=call->state.isIncomingCall;sPGQA.callScripted=call->state.isScriptedCall;
    ++sPGQA.callSelections;PGQALedger();
}
void PokegearQACallObserved(const PokegearPhoneCallContext *call) {
    if(!sPGQA.started)return;
    PGQA_REQUIRE(call && sPGQAParent && call->phoneApp && call->phoneApp->pokegear==sPGQAParent && sPGQAParent->childAppdata==call->phoneApp
        && call->saveData==sPGQAParent->saveData && sPGQA.callSelections==1 && !sPGQA.callCompletions);
    sPGQA.callMainState=call->state.mainState;sPGQA.callScriptState=call->state.scriptState;sPGQA.callScriptType=call->state.scriptType;
    sPGQA.callScriptID=call->state.scriptID;sPGQA.callIncoming=call->state.isIncomingCall;sPGQA.callScripted=call->state.isScriptedCall;
    sPGQA.callTextWait=call->state.mainState==1 && call->state.scriptState==3 ? PGQAPrinterWait(gSystem.printTaskQueue,call->textPrinter,call->phoneCallMsgWindow) : 0;
    if(sPGQA.callTextWait && sPGQA.callTextWait!=sPGQALastTextWait)sPGQA.callTextSerial=PGQANext(sPGQA.callTextSerial);
    sPGQALastTextWait=sPGQA.callTextWait;
    sPGQA.callAwaitHangup=call->state.mainState==2;++sPGQA.snapshotSerial;PGQALedger();
}
void PokegearQACallCompleted(const PokegearPhoneAppData *phone) {
    if(!sPGQA.started)return;
    PGQA_REQUIRE(phone && phone->pokegear==sPGQAParent && phone->pokegear->childAppdata==phone && sPGQA.callSelections==1 && !sPGQA.callCompletions
        && phone->callerID==PHONE_CONTACT_PROF__ELM);
    ++sPGQA.callCompletions;sPGQA.callAwaitHangup=0;sPGQA.coverage|=4;PGQALedger();
}
const HGNativePokegearQAState *HGNativePokegearQAGetState(void){return &sPGQA;}
uint32_t HGNativePokegearQARequest(uint32_t revision,uint32_t request) {
    if(!revision || revision!=sPGQA.revision || sPGQA.error || sPGQARequest || !sPGQA.fieldReady || sPGQA.ui.active)return 0;
    if(request==HG_PG_QA_START ? sPGQA.started || !sPGQA.compatibleSave
        : request==HG_PG_QA_CHECKPOINT ? !sPGQA.started || sPGQA.stage!=HG_PG_QA_RUNNING
        : request==HG_PG_QA_FINISH ? !sPGQA.started || sPGQA.stage!=HG_PG_QA_RUNNING || sPGQA.parentEntries!=2 || sPGQA.parentExits!=2
            || sPGQA.phoneEntries!=2 || sPGQA.configureEntries!=3 || sPGQA.callSelections!=1 || sPGQA.callCompletions!=1
            || sPGQA.callTextSerial!=2 || sPGQA.savedApp!=0 || sPGQA.savedSkin!=1 || sPGQA.checkpoints!=2 : 1)return 0;
    sPGQARequest=request;return 1;
}
void PokegearQAFieldObserved(FieldSystem *field) {
    sPGQAField=field;sPGQA.revision=PGQANext(sPGQA.revision);sPGQA.frame=OS_GetVBlankCount();sPGQA.ui=*HGNativePokegearUIGetState();
    sPGQA.fieldReady=field && field->saveData && FieldSystem_IsFieldMapLoaded(field) && FieldSystem_IsFieldMapRunning(field)
        && !FieldSystem_TaskIsRunning(field) && !FieldSystem_IsChildAppRunning(field) && FieldSystem_IsPlayerMovementAllowed(field) && IsPaletteFadeFinished();
    sPGQA.menuIcons=HGNativeFieldMenuIcons();sPGQA.pokegearSlot=UINT32_MAX;
    for(u32 slot=0;slot<7;++slot)if(((sPGQA.menuIcons>>(slot*4))&15)==START_MENU_ICON_POKEGEAR) {
        const TouchscreenHitbox *box=&gTouchscreenMenuIconHitboxes[slot+1];sPGQA.pokegearSlot=slot;
        sPGQA.pokegearX=(box->rect.left+box->rect.right)/2;sPGQA.pokegearY=(box->rect.top+box->rect.bottom)/2;
    }
    if(!sPGQA.started && field && field->saveData) {
        sPGQA.compatibleSave=sPGQA.fieldReady && !sPGQA.ui.active && sPGQA.pokegearSlot<7 && PGQABaseline(field);
        sPGQA.ringActive=field->phoneRingManager && GearPhoneRingManager_IsRinging(field->phoneRingManager);
        sPGQA.panicFlag=Save_VarsFlags_CheckFlagInArray(Save_VarsFlags_Get(field->saveData),FLAG_GOT_ELMS_PANIC_CALL);
    }
    PGQALedger();if(sPGQA.error || !sPGQARequest || !sPGQA.fieldReady)return;
    u32 request=sPGQARequest;sPGQARequest=0;
    if(request==HG_PG_QA_START)PGQABegin();
    else if(request==HG_PG_QA_CHECKPOINT){PGQALedger();++sPGQA.checkpoints;}
    else if(request==HG_PG_QA_FINISH){PGQALedger();if(sPGQA.error)return;Heap_Free(sPGQAExpectedStats);sPGQAExpectedStats=NULL;sPGQA.stage=HG_PG_QA_DONE;}
}
