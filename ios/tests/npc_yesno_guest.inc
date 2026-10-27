// SPDX-License-Identifier: GPL-3.0-or-later
// Included only by a private source copy of resident field_system.c.
#include "npc_yesno_qa.h"
#include "bag.h"
#include "easy_chat.h"
#include "field/draw_map_name.h"
#include "field_system.h"
#include "field_warp_tasks.h"
#include "map_object.h"
#include "native_field_choice_ui.h"
#include "party.h"
#include "player_avatar.h"
#include "player_data.h"
#include "pokemon_storage_system.h"
#include "save_arrays.h"
#include "save_vars_flags.h"
#include "screen_fade.h"
#include "script.h"
#include "system.h"
#include "task.h"
#include "text.h"
#include <stddef.h>
#include <string.h>

_Static_assert(sizeof(HGNativeFieldChoiceUIState)==36, "shipping field choice ABI");
_Static_assert(sizeof(HGNativeNPCYesNoQAState)==276, "private NPC YES/NO QA version 2 ABI");
_Static_assert(sizeof(HGYNBootFacts)==44 && offsetof(HGNativeNPCYesNoQAState,choice)==140
    && offsetof(HGNativeNPCYesNoQAState,boot)==176 && offsetof(HGNativeNPCYesNoQAState,yesBeforeResult)==220,
    "private observer offsets");
_Static_assert(FLAG_UNK_ABA<TEMP_FLAG_BASE && FLAG_UNK_ABA<NUM_FLAGS && EC_WORDS_TOUGH_WORDS_COUNT==32,
    "original persistent ABA flag and trendy bit layout");
static HGNativeNPCYesNoQAState sNPCYN={.version=HG_YN_QA_VERSION,.byteSize=sizeof(HGNativeNPCYesNoQAState),
    .messageId=UINT32_MAX,.specialResult=UINT32_MAX,.yesBeforeResult=UINT32_MAX,.newWordId=UINT32_MAX};
static FieldSystem *sNPCYNField;
static u32 sNPCYNRequest,sNPCYNOrigin[4],sNPCYNStable,sNPCYNCompletionPending,sNPCYNLastPhase;
static const void *sNPCYNBootOwner;
static ScriptContext *sNPCYNWaitContext;
static ScrCmdFunc sNPCYNWaitNative;
static ScriptContext *sNPCYNPrinterContext;
static ScrCmdFunc sNPCYNPrinterNative;
static Party sNPCYNParty;
static PCStorage sNPCYNPC;
static Bag sNPCYNBag;
static SaveVarsFlags sNPCYNVarsFlags;
static SaveEasyChat sNPCYNEasyChat;
static SaveVarsFlags sNPCYNExpectedVarsFlags;
static SaveEasyChat sNPCYNExpectedEasyChat;
static u32 sNPCYNMoney;

#define NPCYN_REQUIRE(value) do { if (!(value)) { if (!sNPCYN.error) sNPCYN.error=__LINE__; return; } } while (0)

static u32 NPCYNNextNonzero(u32 value) {return value==UINT32_MAX ? 1 : value+1;}

static BOOL NPCYNMakeYesLedger(const SaveVarsFlags *flags,const SaveEasyChat *easy,u32 word,
    SaveVarsFlags *expectedFlags,SaveEasyChat *expectedEasy) {
    if (word>=EC_WORDS_TOUGH_WORDS_COUNT || (flags->flags[FLAG_UNK_ABA/8]&(1u<<(FLAG_UNK_ABA%8)))) return FALSE;
    u32 bit=UINT32_C(1)<<word;
    if (easy->trendy&bit) return FALSE;
    memcpy(expectedFlags,flags,sizeof(*expectedFlags)); memcpy(expectedEasy,easy,sizeof(*expectedEasy));
    // Only these offline copies change; the original script owns both live mutations.
    expectedFlags->flags[FLAG_UNK_ABA/8]|=1u<<(FLAG_UNK_ABA%8);
    expectedEasy->trendy|=bit;
    return TRUE;
}

void NPCYNBootPublish(const void *owner,const HGYNBootFacts *facts) {
    if (!owner || !facts) return;
    u32 generation=sNPCYN.boot.ownerGeneration,revision=NPCYNNextNonzero(sNPCYN.boot.revision);
    if (sNPCYNBootOwner!=owner || !sNPCYN.boot.active) generation=NPCYNNextNonzero(generation);
    sNPCYNBootOwner=owner; sNPCYN.boot=*facts;
    sNPCYN.boot.active=1; sNPCYN.boot.ownerGeneration=generation; sNPCYN.boot.revision=revision;
    sNPCYN.revision=NPCYNNextNonzero(sNPCYN.revision);
}

void NPCYNBootRetire(const void *owner) {
    if (!owner || owner!=sNPCYNBootOwner || !sNPCYN.boot.active) return;
    sNPCYN.boot.active=0; sNPCYN.boot.ready=0;
    sNPCYN.boot.revision=NPCYNNextNonzero(sNPCYN.boot.revision); sNPCYN.revision=NPCYNNextNonzero(sNPCYN.revision);
    sNPCYNBootOwner=NULL;
}

static void NPCYNSnapshot(BOOL baseline) {
    SaveData *save=sNPCYNField->saveData;
    Party *party=SaveArray_Party_Get(save); PCStorage *pc=SaveArray_PCStorage_Get(save);
    Bag *bag=Save_Bag_Get(save); SaveVarsFlags *flags=Save_VarsFlags_Get(save);
    SaveEasyChat *easy=Save_EasyChat_Get(save);
    NPCYN_REQUIRE(party && pc && bag && flags && easy
        && SaveArray_Party_sizeof()==sizeof(sNPCYNParty) && PCStorage_sizeof()==sizeof(sNPCYNPC)
        && Save_Bag_sizeof()==sizeof(sNPCYNBag) && Save_VarsFlags_sizeof()==sizeof(sNPCYNVarsFlags)
        && Save_EasyChat_sizeof()==sizeof(sNPCYNEasyChat));
    u32 money=PlayerProfile_GetMoney(Save_PlayerData_GetProfile(save));
    if (baseline) {
        memcpy(&sNPCYNParty,party,SaveArray_Party_sizeof()); memcpy(&sNPCYNPC,pc,PCStorage_sizeof());
        memcpy(&sNPCYNBag,bag,Save_Bag_sizeof()); memcpy(&sNPCYNVarsFlags,flags,Save_VarsFlags_sizeof());
        memcpy(&sNPCYNEasyChat,easy,Save_EasyChat_sizeof()); sNPCYNMoney=money;
    }
    sNPCYN.partyUnchanged=!memcmp(&sNPCYNParty,party,SaveArray_Party_sizeof());
    sNPCYN.pcUnchanged=!memcmp(&sNPCYNPC,pc,PCStorage_sizeof());
    sNPCYN.bagUnchanged=!memcmp(&sNPCYNBag,bag,Save_Bag_sizeof());
    sNPCYN.varsFlagsUnchanged=!memcmp(&sNPCYNVarsFlags,flags,Save_VarsFlags_sizeof());
    sNPCYN.easyChatUnchanged=!memcmp(&sNPCYNEasyChat,easy,Save_EasyChat_sizeof());
    sNPCYN.moneyUnchanged=sNPCYNMoney==money;
    sNPCYN.varsFlagsLedgerMatched=!memcmp(sNPCYN.ledgerPrepared ? &sNPCYNExpectedVarsFlags : &sNPCYNVarsFlags,
        flags,Save_VarsFlags_sizeof());
    sNPCYN.easyChatLedgerMatched=!memcmp(sNPCYN.ledgerPrepared ? &sNPCYNExpectedEasyChat : &sNPCYNEasyChat,
        easy,Save_EasyChat_sizeof());
    NPCYN_REQUIRE(sNPCYN.partyUnchanged && sNPCYN.pcUnchanged && sNPCYN.bagUnchanged
        && sNPCYN.varsFlagsLedgerMatched && sNPCYN.easyChatLedgerMatched && sNPCYN.moneyUnchanged);
    if (!baseline) ++sNPCYN.conservationChecks;
    ++sNPCYN.checkpointSerial;
}

static ScriptEnvironment *NPCYNLiveScript(FieldSystem *field) {
    if (!field || !field->location || field->location->mapId!=HG_YN_QA_MAP
        || !FieldSystem_IsFieldMapLoaded(field) || !FieldSystem_IsFieldMapRunning(field)
        || FieldSystem_IsChildAppRunning(field) || !IsPaletteFadeFinished()
        || !field->taskman || field->taskman->func!=Task_RunScripts) return NULL;
    ScriptEnvironment *env=TaskManager_GetEnvironment(field->taskman);
    if (!env || env->check!=Unk80_10_C_MAGIC || env->state!=1 || env->activeScriptNumber!=HG_YN_QA_SCRIPT
        || !env->activeScriptContextCount || env->activeScriptContextCount>3 || !env->lastInteracted
        || MapObject_GetID(env->lastInteracted)!=1 || MapObject_GetXCoord(env->lastInteracted)!=8
        || MapObject_GetZCoord(env->lastInteracted)!=5) return NULL;
    return env;
}

static BOOL NPCYNLiveContext(FieldSystem *field,ScriptContext *ctx,BOOL messageWindow) {
    ScriptEnvironment *env=NPCYNLiveScript(field);
    if (!env || (messageWindow && (!env->msgWindowOpen || !env->window.bgConfig || env->window.bgConfig!=field->bgConfig
        || !env->window.pixelBuffer || !env->window.width || !env->window.height))) return FALSE;
    for (u32 i=0;i<3;++i)
        if (env->scriptContexts[i]==ctx) return ctx && ctx->fieldSystem==field && ctx->taskman==field->taskman;
    return FALSE;
}

static void NPCYNPrepareYesLedger(u32 before,u32 word) {
    sNPCYN.yesBeforeResult=before; sNPCYN.newWordId=word; ++sNPCYN.yesSerial; sNPCYN.revision=NPCYNNextNonzero(sNPCYN.revision);
    NPCYN_REQUIRE(sNPCYN.baselinePrepared && sNPCYN.completedNo==2 && !sNPCYN.completedYes
        && sNPCYN.yesSerial==1 && before==0 && sNPCYN.messageId==1 && sNPCYN.messageSerial==5);
    NPCYN_REQUIRE(NPCYNMakeYesLedger(&sNPCYNVarsFlags,&sNPCYNEasyChat,word,&sNPCYNExpectedVarsFlags,&sNPCYNExpectedEasyChat));
    sNPCYN.ledgerPrepared=1;
    sNPCYN.easyChatLedgerMatched=!memcmp(&sNPCYNExpectedEasyChat,Save_EasyChat_Get(sNPCYNField->saveData),Save_EasyChat_sizeof());
    NPCYN_REQUIRE(sNPCYN.easyChatLedgerMatched
        && !memcmp(&sNPCYNVarsFlags,Save_VarsFlags_Get(sNPCYNField->saveData),Save_VarsFlags_sizeof()));
}

BOOL __real_ScrCmd_UnlockRandomTrendySaying(ScriptContext *ctx);
BOOL __wrap_ScrCmd_UnlockRandomTrendySaying(ScriptContext *ctx) {
    BOOL observe=sNPCYN.stage==HG_YN_QA_READY && ctx->fieldSystem==sNPCYNField && NPCYNLiveContext(sNPCYNField,ctx,TRUE);
    u16 *value=observe ? GetVarPointer(sNPCYNField,VAR_SPECIAL_RESULT) : NULL;
    u32 before=value ? *value : UINT32_MAX;
    BOOL result=__real_ScrCmd_UnlockRandomTrendySaying(ctx);
    if (observe) {
        if (NPCYNLiveContext(sNPCYNField,ctx,TRUE)) {
            value=GetVarPointer(sNPCYNField,VAR_SPECIAL_RESULT);
            NPCYNPrepareYesLedger(before,value ? *value : UINT32_MAX);
        } else if (!sNPCYN.error) sNPCYN.error=__LINE__;
    }
    return result;
}

static BOOL NPCYNExpectedWaitMessage(void) {
    return (sNPCYN.messageId==3 && sNPCYN.noResultObserved)
        || (sNPCYN.messageId==2 && sNPCYN.ledgerPrepared && sNPCYN.yesFlagSet)
        || (sNPCYN.messageId==4 && sNPCYN.completedYes==1 && sNPCYN.dailySeen==1 && sNPCYN.yesFlagSet);
}

BOOL __real_ScrCmd_NPCMsg(ScriptContext *ctx);
BOOL __wrap_ScrCmd_NPCMsg(ScriptContext *ctx) {
    // ScriptReadByte consumes exactly this u8; observation must not advance it.
    u32 message=ctx->script_ptr ? *ctx->script_ptr : UINT32_MAX;
    BOOL result=__real_ScrCmd_NPCMsg(ctx);
    if (sNPCYN.stage==HG_YN_QA_READY && ctx->fieldSystem==sNPCYNField && NPCYNLiveContext(sNPCYNField,ctx,TRUE)) {
        sNPCYN.messageId=message; ++sNPCYN.messageSerial; sNPCYN.revision=NPCYNNextNonzero(sNPCYN.revision);
        sNPCYN.waitButtonReady=0; sNPCYNWaitContext=NULL; sNPCYNWaitNative=NULL;
        sNPCYN.printerWaitReady=0; sNPCYNPrinterContext=NULL; sNPCYNPrinterNative=NULL;
        sNPCYN.noResultObserved=0; sNPCYN.specialResult=UINT32_MAX;
        if (message==2 || message==3) {
            u16 *value=GetVarPointer(sNPCYNField,VAR_SPECIAL_RESULT);
            if (value) { sNPCYN.specialResult=*value; sNPCYN.noResultObserved=message==3 && *value==1; }
        }
        BOOL valid=FALSE;
        if (message==1) valid=!sNPCYN.yesSerial && sNPCYN.completedNo<=2 && sNPCYN.messageSerial==sNPCYN.completedNo*2+1;
        if (message==3) valid=!sNPCYN.yesSerial && sNPCYN.completedNo<2
            && sNPCYN.messageSerial==sNPCYN.completedNo*2+2 && sNPCYN.noResultObserved;
        if (message==2) {
            sNPCYN.yesMessageSerial=sNPCYN.messageSerial;
            sNPCYN.yesFlagSet=Save_VarsFlags_CheckFlagInArray(Save_VarsFlags_Get(sNPCYNField->saveData),FLAG_UNK_ABA);
            valid=sNPCYN.completedNo==2 && sNPCYN.yesSerial==1 && !sNPCYN.completedYes && sNPCYN.messageSerial==6
                && sNPCYN.ledgerPrepared && sNPCYN.yesFlagSet && sNPCYN.specialResult==sNPCYN.newWordId;
            if (ctx->mode==SCRIPT_MODE_NATIVE && ctx->native_ptr) {
                sNPCYNPrinterContext=ctx; sNPCYNPrinterNative=ctx->native_ptr;
            } else valid=FALSE;
        }
        if (message==4) {
            ++sNPCYN.dailySeen;
            valid=sNPCYN.completedYes==1 && sNPCYN.dailySeen==1 && !sNPCYN.completedDaily && sNPCYN.messageSerial==7
                && Save_VarsFlags_CheckFlagInArray(Save_VarsFlags_Get(sNPCYNField->saveData),FLAG_UNK_ABA);
        }
        if (!valid && !sNPCYN.error) sNPCYN.error=__LINE__;
    }
    return result;
}

BOOL __real_ScrCmd_WaitButton(ScriptContext *ctx);
BOOL __wrap_ScrCmd_WaitButton(ScriptContext *ctx) {
    BOOL result=__real_ScrCmd_WaitButton(ctx);
    if (sNPCYN.stage==HG_YN_QA_READY && ctx->fieldSystem==sNPCYNField && NPCYNLiveContext(sNPCYNField,ctx,TRUE)
        && NPCYNExpectedWaitMessage() && ctx->mode==SCRIPT_MODE_NATIVE && ctx->native_ptr) {
        sNPCYNWaitContext=ctx; sNPCYNWaitNative=ctx->native_ptr;
        sNPCYN.waitButtonMessageSerial=sNPCYN.messageSerial;
    }
    return result;
}

const HGNativeNPCYesNoQAState *HGNativeNPCYesNoQAGetState(void) { return &sNPCYN; }
u32 HGNativeNPCYesNoQARequest(u32 revision,u32 kind) {
    if (!revision || revision!=sNPCYN.revision || sNPCYN.error || sNPCYNRequest || !sNPCYN.fieldReady) return FALSE;
    if (kind==HG_YN_QA_START ? sNPCYN.stage!=HG_YN_QA_IDLE || !sNPCYN.compatibleSave
        : kind==HG_YN_QA_RETURN ? sNPCYN.stage!=HG_YN_QA_READY || sNPCYN.mapId!=HG_YN_QA_MAP
            || sNPCYN.completedNo!=2 || sNPCYN.completedYes!=1 || sNPCYN.completedDaily!=1
            || sNPCYN.conservationChecks!=4 : TRUE) return FALSE;
    sNPCYNRequest=kind; return TRUE;
}

static BOOL NPCYNWarpTask(TaskManager *task) {
    u32 *state=TaskManager_GetStatePtr(task);
    if (!*state) {
        *state=1;
        if (sNPCYN.stage==HG_YN_QA_WARP_OUT) CallTask_ScriptWarp(task,HG_YN_QA_MAP,-1,HG_YN_QA_X,HG_YN_QA_Z,DIR_NORTH);
        else CallTask_ScriptWarp(task,sNPCYNOrigin[0],-1,sNPCYNOrigin[1],sNPCYNOrigin[2],sNPCYNOrigin[3]);
        return FALSE;
    }
    sNPCYN.stage=sNPCYN.stage==HG_YN_QA_WARP_OUT ? HG_YN_QA_ARRIVING : HG_YN_QA_RETURNING;
    sNPCYNStable=0;
    return TRUE;
}

static void NPCYNFieldTick(FieldSystem *field) {
    sNPCYNField=field; sNPCYN.revision=NPCYNNextNonzero(sNPCYN.revision); sNPCYN.frame=OS_GetVBlankCount();
    sNPCYN.mapId=field->location ? field->location->mapId : UINT32_MAX;
    sNPCYN.fieldReady=field->location && field->saveData && FieldSystem_IsFieldMapLoaded(field) && FieldSystem_IsFieldMapRunning(field)
        && !FieldSystem_TaskIsRunning(field) && !FieldSystem_IsChildAppRunning(field)
        && FieldSystem_IsPlayerMovementAllowed(field) && IsPaletteFadeFinished() && field->playerAvatar && field->mapSystems;
    if (field->playerAvatar) {
        sNPCYN.x=PlayerAvatar_GetXCoord(field->playerAvatar); sNPCYN.z=PlayerAvatar_GetZCoord(field->playerAvatar);
        sNPCYN.facing=PlayerAvatar_GetFacingDirection(field->playerAvatar);
    }
    sNPCYN.flagABAClear=field->saveData && !Save_VarsFlags_CheckFlagInArray(Save_VarsFlags_Get(field->saveData),FLAG_UNK_ABA);
    sNPCYN.trendySayingsRemain=field->saveData && !Save_EasyChat_TrendySayingsUnlockedAllCheck(Save_EasyChat_Get(field->saveData));
    if (!sNPCYN.baselinePrepared) sNPCYN.compatibleSave=sNPCYN.flagABAClear && sNPCYN.trendySayingsRemain;
    sNPCYN.script2Active=NPCYNLiveScript(field)!=NULL;
    sNPCYN.choice=*HGNativeFieldChoiceUIGetState();
    u32 phase=sNPCYN.script2Active && sNPCYN.choice.active ? sNPCYN.choice.phase : UINT32_MAX;
    if (phase!=sNPCYNLastPhase) {
        if (phase==5) ++sNPCYN.phase5Serial;
        if (phase==6) ++sNPCYN.phase6Serial;
    }
    sNPCYNLastPhase=phase;
    BOOL choiceOwnsInput=sNPCYN.choice.active && (sNPCYN.choice.ready || sNPCYN.choice.viewActive
        || sNPCYN.choice.phase==5 || sNPCYN.choice.phase==6 || HGNativeFieldChoiceUIIsListPhase(sNPCYN.choice.phase));
    u32 printerReady=FALSE;
    if (sNPCYNPrinterContext && NPCYNLiveContext(field,sNPCYNPrinterContext,TRUE)
        && sNPCYNPrinterContext->mode==SCRIPT_MODE_NATIVE && sNPCYNPrinterContext->native_ptr==sNPCYNPrinterNative
        && sNPCYN.messageId==2 && sNPCYN.yesMessageSerial==sNPCYN.messageSerial && !choiceOwnsInput) {
        ScriptEnvironment *env=NPCYNLiveScript(field);
        printerReady=TextPrinterIsWaitingForInput(env->textPrinterNum,&env->window);
    }
    if (printerReady && !sNPCYN.printerWaitReady) {
        ++sNPCYN.printerWaitSerial; sNPCYN.printerWaitMessageSerial=sNPCYN.messageSerial;
    }
    sNPCYN.printerWaitReady=printerReady;
    if (sNPCYN.dailySeen && !sNPCYN.completedDaily && choiceOwnsInput && !sNPCYN.error) sNPCYN.error=__LINE__;
    u32 liveWait=sNPCYNWaitContext && NPCYNLiveContext(field,sNPCYNWaitContext,TRUE)
        && sNPCYNWaitNative && sNPCYNWaitContext->native_ptr==sNPCYNWaitNative
        && NPCYNExpectedWaitMessage() && sNPCYN.waitButtonMessageSerial==sNPCYN.messageSerial;
    u32 waitReady=liveWait && sNPCYNWaitContext->mode==SCRIPT_MODE_NATIVE;
    if (waitReady && !sNPCYN.waitButtonReady) ++sNPCYN.waitButtonSerial;
    // RunScriptCommand leaves the original successful native callback in bytecode mode for this frame.
    if (liveWait && sNPCYN.waitButtonReady && sNPCYNWaitContext->mode==SCRIPT_MODE_BYTECODE) {
        if (!(gSystem.newKeys&PAD_BUTTON_A)) { if (!sNPCYN.error) sNPCYN.error=__LINE__; }
        else {
            ++sNPCYN.waitButtonAckSerial;
            sNPCYNCompletionPending=sNPCYN.messageId==3 ? 1 : sNPCYN.messageId==2 ? 2 : 3;
        }
    }
    sNPCYN.waitButtonReady=waitReady;
    if (sNPCYN.error) return;
    if (sNPCYN.stage==HG_YN_QA_ARRIVING || sNPCYN.stage==HG_YN_QA_RETURNING) {
        if (!sNPCYN.fieldReady) { sNPCYNStable=0; return; }
        if (!sNPCYNStable++) return;
        if (sNPCYN.stage==HG_YN_QA_ARRIVING) {
            NPCYN_REQUIRE(sNPCYN.mapId==HG_YN_QA_MAP && sNPCYN.x==HG_YN_QA_X && sNPCYN.z==HG_YN_QA_Z
                && sNPCYN.facing==DIR_NORTH && sNPCYN.compatibleSave);
            NPCYNSnapshot(TRUE); if (sNPCYN.error) return;
            sNPCYN.baselinePrepared=1; sNPCYN.stage=HG_YN_QA_READY;
        } else {
            NPCYN_REQUIRE(sNPCYN.mapId==sNPCYNOrigin[0] && sNPCYN.x==sNPCYNOrigin[1]
                && sNPCYN.z==sNPCYNOrigin[2] && sNPCYN.facing==sNPCYNOrigin[3]);
            NPCYNSnapshot(FALSE); if (sNPCYN.error) return;
            sNPCYN.stage=HG_YN_QA_DONE;
        }
        return;
    }
    if (sNPCYN.stage==HG_YN_QA_READY && sNPCYNCompletionPending && sNPCYN.fieldReady) {
        NPCYN_REQUIRE(sNPCYN.baselinePrepared && sNPCYN.mapId==HG_YN_QA_MAP);
        NPCYN_REQUIRE(sNPCYNCompletionPending==1 ? sNPCYN.completedNo<2 && !sNPCYN.yesSerial
            : sNPCYNCompletionPending==2 ? sNPCYN.completedNo==2 && sNPCYN.yesSerial==1 && !sNPCYN.completedYes
            : sNPCYNCompletionPending==3 && sNPCYN.completedYes==1 && sNPCYN.dailySeen==1 && !sNPCYN.completedDaily);
        NPCYNSnapshot(FALSE); if (sNPCYN.error) return;
        if (sNPCYNCompletionPending==1) ++sNPCYN.completedNo;
        else if (sNPCYNCompletionPending==2) ++sNPCYN.completedYes;
        else ++sNPCYN.completedDaily;
        sNPCYNCompletionPending=0; sNPCYNWaitContext=NULL; sNPCYNWaitNative=NULL;
        sNPCYNPrinterContext=NULL; sNPCYNPrinterNative=NULL;
    }
    if (!sNPCYNRequest || !sNPCYN.fieldReady) return;
    u32 request=sNPCYNRequest; sNPCYNRequest=0;
    if (request==HG_YN_QA_START) {
        NPCYN_REQUIRE(sNPCYN.stage==HG_YN_QA_IDLE && sNPCYN.compatibleSave);
        sNPCYNOrigin[0]=sNPCYN.mapId; sNPCYNOrigin[1]=sNPCYN.x; sNPCYNOrigin[2]=sNPCYN.z; sNPCYNOrigin[3]=sNPCYN.facing;
        sNPCYN.stage=HG_YN_QA_WARP_OUT;
    } else {
        NPCYN_REQUIRE(request==HG_YN_QA_RETURN && sNPCYN.stage==HG_YN_QA_READY && sNPCYN.completedNo==2
            && sNPCYN.completedYes==1 && sNPCYN.completedDaily==1 && sNPCYN.conservationChecks==4);
        sNPCYN.stage=HG_YN_QA_WARP_BACK;
    }
    FieldDrawMapName_Reset(field->mapSystems->drawMapNameInfo);
    FieldSystem_CreateTask(field,NPCYNWarpTask,NULL);
}
