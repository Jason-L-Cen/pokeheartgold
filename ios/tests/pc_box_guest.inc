// SPDX-License-Identifier: GPL-3.0-or-later
// Included only by an isolated derivative of the resident field_system.c object.
#include "pc_box_qa.h"
#include "application/pc_box/pc_box_internal.h"
#include "bag.h"
#include "constants/items.h"
#include "constants/heap.h"
#include "constants/pokemon.h"
#include "constants/species.h"
#include "constants/std_script.h"
#include "field/draw_map_name.h"
#include "field/script_menu.h"
#include "field_system.h"
#include "field_warp_tasks.h"
#include "field_terrain.h"
#include "overlay_01.h"
#include "native_field_choice_ui.h"
#include "party.h"
#include "player_avatar.h"
#include "player_data.h"
#include "pm_string.h"
#include "pokedex.h"
#include "pokemon.h"
#include "save_arrays.h"
#include "save_vars_flags.h"
#include "screen_fade.h"
#include "script.h"
#include "task.h"
#include "text.h"
#include <string.h>

_Static_assert(sizeof(HGPCBoxQAState)==1372 && offsetof(HGPCBoxQAState,messageAwaitInput)==1368,"private PC QA version 2 append-only ABI");
static HGPCBoxQAState sPCQA={.version=HG_PC_QA_VERSION,.byteSize=sizeof(HGPCBoxQAState)};
static FieldSystem *sPCQAField;
static uint32_t sPCQARequest,sPCQAOrigin[4],sPCQAMenuWasReady;
static Bag sPCQABag;
static SaveVarsFlags sPCQAFlags;
static u32 sPCQAMoney,sPCQASeen,sPCQACaught,sPCQABoxName;
static const int sPCQAAttrs[]={MON_DATA_PERSONALITY,MON_DATA_OT_ID,MON_DATA_SPECIES,MON_DATA_FORM,MON_DATA_IS_EGG,
    MON_DATA_EXPERIENCE,MON_DATA_FRIENDSHIP,MON_DATA_ABILITY,MON_DATA_LANGUAGE,MON_DATA_POKEBALL,
    MON_DATA_OT_GENDER,MON_DATA_MET_GAME,MON_DATA_FATEFUL_ENCOUNTER,MON_DATA_MARKINGS,
    MON_DATA_MOVE1,MON_DATA_MOVE2,MON_DATA_MOVE3,MON_DATA_MOVE4,
    MON_DATA_MOVE1_PP,MON_DATA_MOVE2_PP,MON_DATA_MOVE3_PP,MON_DATA_MOVE4_PP,
    MON_DATA_MOVE1_PP_UPS,MON_DATA_MOVE2_PP_UPS,MON_DATA_MOVE3_PP_UPS,MON_DATA_MOVE4_PP_UPS,
    MON_DATA_HP_IV,MON_DATA_ATK_IV,MON_DATA_DEF_IV,MON_DATA_SPEED_IV,MON_DATA_SPATK_IV,MON_DATA_SPDEF_IV,
    MON_DATA_HP_EV,MON_DATA_ATK_EV,MON_DATA_DEF_EV,MON_DATA_SPEED_EV,MON_DATA_SPATK_EV,MON_DATA_SPDEF_EV};
static u32 sPCQABaseline[HG_PC_QA_MONS][NELEMS(sPCQAAttrs)],sPCQAItems[HG_PC_QA_MONS];
static u16 sPCQANames[HG_PC_QA_MONS][32];

#define PCQA_REQUIRE(value) do { if (!(value)) { if (!sPCQA.error) sPCQA.error=__LINE__; return; } } while (0)
static void PCBoxQASnapshot(BOOL baseline) {
    SaveData *save=sPCQAField->saveData;
    Party *party=SaveArray_Party_Get(save); PCStorage *storage=SaveArray_PCStorage_Get(save);
    HGPCBoxQAMon current[HG_PC_QA_MONS]={{0}};
    u32 count=0,seen=0,values[NELEMS(sPCQAAttrs)];
    sPCQA.partyCount=Party_GetCount(party); sPCQA.boxedCount=0;
    PCQA_REQUIRE(sPCQA.partyCount<=6);
    for (u32 location=0;location<6+18*30;++location) {
        BOOL inParty=location<6;
        if (inParty && location>=sPCQA.partyCount) continue;
        BoxPokemon *box=inParty ? Mon_GetBoxMon(Party_GetMonByIndex(party,location))
            : PCStorage_GetMonByIndexPair(storage,(location-6)/30,(location-6)%30);
        if (!GetBoxMonData(box,MON_DATA_SPECIES,NULL)) continue;
        PCQA_REQUIRE(count<HG_PC_QA_MONS);
        for (u32 a=0;a<NELEMS(sPCQAAttrs);++a) values[a]=GetBoxMonData(box,sPCQAAttrs[a],NULL);
        u16 names[32]={0}; GetBoxMonData(box,MON_DATA_NICKNAME,names); GetBoxMonData(box,MON_DATA_OT_NAME,names+16);
        u32 index=count;
        if (!baseline) {
            for (index=0;index<HG_PC_QA_MONS;++index)
                if (values[0]==sPCQABaseline[index][0] && values[1]==sPCQABaseline[index][1]) break;
            PCQA_REQUIRE(index<HG_PC_QA_MONS && !(seen&(1u<<index))
                && !memcmp(values,sPCQABaseline[index],sizeof(values)) && !memcmp(names,sPCQANames[index],sizeof(names)));
        } else {
            memcpy(sPCQABaseline[index],values,sizeof(values)); memcpy(sPCQANames[index],names,sizeof(names));
        }
        if (inParty && index==1) {
            Pokemon *mon=Party_GetMonByIndex(party,location);
            PCQA_REQUIRE(GetMonData(mon,MON_DATA_HP,NULL)==GetMonData(mon,MON_DATA_MAX_HP,NULL) && !GetMonData(mon,MON_DATA_STATUS,NULL));
        }
        seen|=1u<<index;
        u32 item=GetBoxMonData(box,MON_DATA_HELD_ITEM,NULL);
        if (baseline) sPCQAItems[index]=item;
        current[index]=(HGPCBoxQAMon){values[0],values[1],values[2],item,
            inParty ? HG_PC_QA_PARTY_LOCATION+location : location-6};
        if (!inParty) ++sPCQA.boxedCount;
        ++count;
    }
    PCQA_REQUIRE(count==HG_PC_QA_MONS && seen==15);
    for (u32 i=0;i<HG_PC_QA_MONS;++i) {
        u32 before=0,after=0;
        for (u32 j=0;j<HG_PC_QA_MONS;++j) { before+=sPCQAItems[j]==sPCQAItems[i]; after+=current[j].heldItem==sPCQAItems[i]; }
        PCQA_REQUIRE(before==after);
    }
    memcpy(sPCQA.mons,current,sizeof(current));
    String *name=String_New(32,HEAP_ID_FIELD2); PCQA_REQUIRE(name);
    PCStorage_GetBoxName(storage,0,name); const u16 *text=String_cstr(name); u32 nameHash=2166136261u;
    for (u32 i=0;i<32 && text[i]!=0xFFFF;++i) nameHash=(nameHash^text[i])*16777619u;
    String_Delete(name);
    if (baseline) {
        sPCQABoxName=nameHash;
        Save_Bag_Copy(Save_Bag_Get(save),&sPCQABag); sPCQAFlags=*Save_VarsFlags_Get(save);
        sPCQAMoney=PlayerProfile_GetMoney(Save_PlayerData_GetProfile(save));
        sPCQASeen=Pokedex_CountNationalDexSeen(Save_Pokedex_Get(save));
        sPCQACaught=Pokedex_CountNationalDexOwned(Save_Pokedex_Get(save));
    }
    sPCQA.bagUnchanged=!memcmp(&sPCQABag,Save_Bag_Get(save),sizeof(Bag));
    sPCQA.flagsUnchanged=!memcmp(&sPCQAFlags,Save_VarsFlags_Get(save),sizeof(SaveVarsFlags));
    sPCQA.moneyDexUnchanged=sPCQAMoney==PlayerProfile_GetMoney(Save_PlayerData_GetProfile(save))
        && sPCQASeen==Pokedex_CountNationalDexSeen(Save_Pokedex_Get(save))
        && sPCQACaught==Pokedex_CountNationalDexOwned(Save_Pokedex_Get(save));
    sPCQA.boxNameUnchanged=nameHash==sPCQABoxName;
    PCQA_REQUIRE(sPCQA.bagUnchanged && sPCQA.flagsUnchanged && sPCQA.moneyDexUnchanged && sPCQA.boxNameUnchanged);
    ++sPCQA.conservationChecks; ++sPCQA.checkpointSerial;
}

static void PCBoxQAPrepare(void) {
    SaveData *save=sPCQAField->saveData; Party *party=SaveArray_Party_Get(save);
    PCStorage *storage=SaveArray_PCStorage_Get(save);
    PCQA_REQUIRE(!sPCQA.baselinePrepared && Party_GetCount(party)==1 && PCStorage_CountEmptySpotsInAllBoxes(storage)==18*30);
    PlayerProfile *profile=Save_PlayerData_GetProfile(save);
    Pokemon mon; u32 ot=PlayerProfile_GetTrainerID(profile); u16 item=ITEM_POTION; u8 gender=PlayerProfile_GetTrainerGender(profile);
    // The only synthetic arrangement: original APIs, before the frozen gameplay baseline.
    CreateMon(&mon,SPECIES_SENTRET,5,20,TRUE,0x12340001,1,ot); ++sPCQA.preparationSetterCalls;
    SetMonData(&mon,MON_DATA_OT_NAME,PlayerProfile_GetNamePtr(profile)); SetMonData(&mon,MON_DATA_OT_GENDER,&gender); sPCQA.preparationSetterCalls+=2;
    Pokedex_SetMonSeenFlag(Save_Pokedex_Get(save),&mon); Pokedex_SetMonCaughtFlag(Save_Pokedex_Get(save),&mon); sPCQA.preparationSetterCalls+=2;
    PCQA_REQUIRE(Party_AddMon(party,&mon)); ++sPCQA.preparationSetterCalls;
    CreateMon(&mon,SPECIES_PIDGEY,5,20,TRUE,0x12340002,1,ot); ++sPCQA.preparationSetterCalls;
    SetMonData(&mon,MON_DATA_OT_NAME,PlayerProfile_GetNamePtr(profile)); SetMonData(&mon,MON_DATA_OT_GENDER,&gender); sPCQA.preparationSetterCalls+=2;
    Pokedex_SetMonSeenFlag(Save_Pokedex_Get(save),&mon); Pokedex_SetMonCaughtFlag(Save_Pokedex_Get(save),&mon); sPCQA.preparationSetterCalls+=2;
    SetMonData(&mon,MON_DATA_HELD_ITEM,&item); ++sPCQA.preparationSetterCalls;
    PCQA_REQUIRE(PCStorage_PlaceMonInBoxByIndexPair(storage,0,2,Mon_GetBoxMon(&mon))); ++sPCQA.preparationSetterCalls;
    CreateMon(&mon,SPECIES_RATTATA,5,20,TRUE,0x12340003,1,ot); ++sPCQA.preparationSetterCalls;
    SetMonData(&mon,MON_DATA_OT_NAME,PlayerProfile_GetNamePtr(profile)); SetMonData(&mon,MON_DATA_OT_GENDER,&gender); sPCQA.preparationSetterCalls+=2;
    Pokedex_SetMonSeenFlag(Save_Pokedex_Get(save),&mon); Pokedex_SetMonCaughtFlag(Save_Pokedex_Get(save),&mon); sPCQA.preparationSetterCalls+=2;
    PCQA_REQUIRE(PCStorage_PlaceMonInBoxByIndexPair(storage,0,29,Mon_GetBoxMon(&mon))); ++sPCQA.preparationSetterCalls;
    PCStorage_SetActiveBox(storage,0); ++sPCQA.preparationSetterCalls;
    PCBoxQASnapshot(TRUE); if (sPCQA.error) return;
    sPCQA.baselinePrepared=1;
}

const HGPCBoxQAState *HGNativePCBoxQAGetState(void) { return &sPCQA; }
u32 HGNativePCBoxQARequest(u32 revision,u32 kind) {
    if (!revision || revision!=sPCQA.revision || sPCQA.error || sPCQARequest) return FALSE;
    if (kind==HG_PC_QA_START ? sPCQA.stage!=0 || !sPCQA.fieldReady
        : kind==HG_PC_QA_RETURN ? sPCQA.stage!=3 || !sPCQA.fieldReady || sPCQA.mapId!=69
        : kind==HG_PC_QA_CHECKPOINT ? !sPCQA.baselinePrepared
            || !(sPCQA.fieldReady || (sPCQA.pc.ready && !sPCQA.pc.cursorHolding)) : TRUE) return FALSE;
    sPCQARequest=kind; return TRUE;
}

static BOOL PCBoxQAWarpTask(TaskManager *task) {
    u32 *state=TaskManager_GetStatePtr(task);
    if (!*state) {
        *state=1;
        if (sPCQA.stage==2) CallTask_ScriptWarp(task,69,-1,11,13,DIR_NORTH);
        else CallTask_ScriptWarp(task,sPCQAOrigin[0],-1,sPCQAOrigin[1],sPCQAOrigin[2],sPCQAOrigin[3]);
        return FALSE;
    }
    sPCQA.stage=sPCQA.stage==2 ? 3 : 5;
    return TRUE;
}

BOOL __real_PCBox_Main(OverlayManager *manager,int *state);
BOOL __wrap_PCBox_Main(OverlayManager *manager,int *state) {
    BOOL result=__real_PCBox_Main(manager,state);
    PCBoxWork *work=OverlayManager_GetData(manager);
    sPCQA.messageAwaitInput=0;
    sPCQA.pc=*HGNativePCBoxUIGetState(); sPCQA.summary=*HGNativeSummaryUIGetState(); sPCQA.naming=*HGNativeNamingUIGetState();
    sPCQA.gridCount=0;
    if (sPCQA.pc.ready && sPCQA.pc.viewActive && work->view && work->view->gridInput) {
        GridInputHandler *grid=work->view->gridInput;
        while (sPCQA.gridCount<HG_PC_QA_GRID && grid->touchscreenHitboxes[sPCQA.gridCount].rect.top!=TOUCHSCREEN_RECTLIST_END) ++sPCQA.gridCount;
        if (sPCQA.gridCount==HG_PC_QA_GRID) { if (!sPCQA.error) sPCQA.error=__LINE__; }
        else {
            sPCQA.gridCursor=grid->nextInput; sPCQA.gridReturn=grid->returnInput; sPCQA.gridButtons=grid->isButtons;
            memcpy(sPCQA.gridEnabled,grid->enabledFlag,sizeof(sPCQA.gridEnabled));
            for (u32 i=0;i<sPCQA.gridCount;++i) for (u32 d=0;d<4;++d)
                sPCQA.neighbors[i][d]=DpadMenuBox_GetNeighborInDirection(grid->dpadPositions,NULL,NULL,NULL,NULL,i,d);
        }
        sPCQA.yesNoHandler=work->view->yesNoHandler;
    }
    if (sPCQARequest==HG_PC_QA_CHECKPOINT && sPCQA.pc.ready && !sPCQA.pc.cursorHolding) {
        sPCQARequest=0; PCBoxQASnapshot(FALSE);
    }
    ++sPCQA.revision;
    return result;
}

static BOOL PCBoxQAMessageAwaitInput(FieldSystem *field) {
    if (!FieldSystem_IsFieldMapLoaded(field) || !FieldSystem_IsFieldMapRunning(field)
        || FieldSystem_IsChildAppRunning(field) || !IsPaletteFadeFinished()
        || !field->taskman || field->taskman->func!=Task_RunScripts) return FALSE;
    const HGNativeFieldChoiceUIState *choice=HGNativeFieldChoiceUIGetState();
    if (choice->active && (choice->ready || choice->viewActive || HGNativeFieldChoiceUIIsListPhase(choice->phase))) return FALSE;
    ScriptEnvironment *env=TaskManager_GetEnvironment(field->taskman);
    if (!env || env->check!=Unk80_10_C_MAGIC || env->state!=1 || env->activeScriptNumber!=std_pokecenter_pc
        || !env->msgWindowOpen || !env->activeScriptContextCount || env->activeScriptContextCount>3
        || env->window.bgConfig!=field->bgConfig) return FALSE;
    for (u32 i=0;i<3;++i) {
        const ScriptContext *ctx=env->scriptContexts[i];
        if (ctx && ctx->fieldSystem==field && ctx->mode==SCRIPT_MODE_NATIVE && ctx->native_ptr==ScrNative_WaitTextPrinter)
            return TextPrinterIsWaitingForInput(env->textPrinterNum,&env->window);
    }
    return FALSE;
}

static void PCBoxQAFieldTick(FieldSystem *field) {
    sPCQAField=field; ++sPCQA.revision; sPCQA.frame=OS_GetVBlankCount();
    sPCQA.messageAwaitInput=0;
    sPCQA.mapId=field->location ? field->location->mapId : UINT32_MAX;
    sPCQA.fieldReady=FieldSystem_IsFieldMapLoaded(field) && FieldSystem_IsFieldMapRunning(field)
        && !FieldSystem_TaskIsRunning(field) && !FieldSystem_IsChildAppRunning(field)
        && FieldSystem_IsPlayerMovementAllowed(field) && IsPaletteFadeFinished();
    sPCQA.scriptMenuReady=0;
    if (FieldSystem_IsFieldMapRunning(field) && !FieldSystem_IsChildAppRunning(field) && field->bottomScreenTask) {
        if (field->playerAvatar) {
            sPCQA.x=PlayerAvatar_GetXCoord(field->playerAvatar); sPCQA.z=PlayerAvatar_GetZCoord(field->playerAvatar);
            sPCQA.facing=PlayerAvatar_GetFacingDirection(field->playerAvatar);
            if (sPCQA.mapId==69) sPCQA.pcTileBehavior=GetMetatileBehavior(field,11,12);
        }
        sPCQA.pc=*HGNativePCBoxUIGetState();
        if (FieldSystem_GetBottomScreenAppId(field)==3 && FieldSystem_GetTouchscreenChoiceMenuState(field)==8 && IsPaletteFadeFinished()) {
            ScriptMenu *menu=*FieldSystem_GetTouchscreenChoiceMenuList(field);
            if (menu) {
                sPCQA.scriptMenuCount=ScriptMenu_GetCount(menu);
                if (sPCQA.scriptMenuCount>8) { if (!sPCQA.error) sPCQA.error=__LINE__; return; }
                ListMenuItem *items=ScriptMenu_GetListItems(menu);
                for (u32 i=0;i<sPCQA.scriptMenuCount;++i) sPCQA.scriptMenuValues[i]=items[i].value;
                sPCQA.scriptMenuReady=1;
            }
        }
    }
    if (sPCQA.scriptMenuReady && !sPCQAMenuWasReady) ++sPCQA.scriptMenuSerial;
    sPCQAMenuWasReady=sPCQA.scriptMenuReady;
    sPCQA.messageAwaitInput=PCBoxQAMessageAwaitInput(field);
    if (!sPCQARequest || !sPCQA.fieldReady) return;
    u32 request=sPCQARequest; sPCQARequest=0;
    if (request==HG_PC_QA_CHECKPOINT) { PCBoxQASnapshot(FALSE); return; }
    if (request==HG_PC_QA_START) {
        PCQA_REQUIRE(sPCQA.mapId==33);
        sPCQAOrigin[0]=sPCQA.mapId; sPCQAOrigin[1]=sPCQA.x; sPCQAOrigin[2]=sPCQA.z; sPCQAOrigin[3]=sPCQA.facing;
        PCBoxQAPrepare(); if (sPCQA.error) return; sPCQA.stage=2;
    } else sPCQA.stage=4;
    FieldDrawMapName_Reset(field->mapSystems->drawMapNameInfo);
    FieldSystem_CreateTask(field,PCBoxQAWarpTask,NULL);
}
