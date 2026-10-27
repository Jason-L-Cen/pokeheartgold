// SPDX-License-Identifier: GPL-3.0-or-later
// Included only by a private copy of resident field_system.c; no live save setter is used.
#define HG_TRAINER_CARD_QA_GUEST 1
#include "trainer_card_qa.h"
#include "trainer_card_ledger.h"
#include "application/trainer_card/overlay_trainer_card_main_internal.h"
#include "bag.h"
#include "easy_chat.h"
#include "field_system.h"
#include "field_overworld_restore.h"
#include "field_overworld_transition.h"
#include "game_stats.h"
#include "heap.h"
#include "launch_application.h"
#include "party.h"
#include "player_data.h"
#include "pokemon_storage_system.h"
#include "save_arrays.h"
#include "save_trainer_card.h"
#include "save_vars_flags.h"
#include "screen_fade.h"
#include "system.h"
#include "task.h"
#include "touchscreen.h"
#include "trainer_card_data.h"
#include <stddef.h>
#include <string.h>
extern const TouchscreenHitbox gTouchscreenMenuIconHitboxes[];
_Static_assert(sizeof(HGNativeTrainerCardUIState)==68 && sizeof(HGYNBootFacts)==44,"actual shipping/typed boot ABI");
_Static_assert(sizeof(HGNativeTrainerCardQAState)==12092 && offsetof(HGNativeTrainerCardQAState,ui)==172
    && offsetof(HGNativeTrainerCardQAState,boot)==240,"private observer packet ABI");
_Static_assert(sizeof(TrainerCardData)==0x66c,"original complete cached card ABI");
_Static_assert(sizeof(struct SaveTrainerCard)==0x620 && offsetof(struct SaveTrainerCard,signaturePointData)==0x20,"original save witness ABI");
static HGNativeTrainerCardQAState sTCQA={.version=HG_TC_QA_VERSION,.byteSize=sizeof(HGNativeTrainerCardQAState),.trainerSlot=UINT32_MAX};
static FieldSystem *sTCQAField;
static const void *sTCQABootOwner;
static uint32_t sTCQARequest,sTCQACanvasGeneration;
static Party sTCQAParty;
static PCStorage sTCQAPC;
static Bag sTCQABag;
static SaveVarsFlags sTCQAFlags;
static SaveEasyChat sTCQAEasy;
static struct SaveTrainerCard sTCQAExpectedCard;
static GameStats *sTCQAExpectedStats;
static uint32_t sTCQAMoney;
#define TCQA_REQUIRE(value) do {if (!(value)) {if (!sTCQA.error)sTCQA.error=__LINE__;return;}} while (0)
void NPCYNBootPublish(const void *owner,const HGYNBootFacts *facts) {
    if (!owner || !facts) return;
    uint32_t generation=sTCQA.boot.ownerGeneration,revision=TCQANext(sTCQA.boot.revision);
    if (owner!=sTCQABootOwner || !sTCQA.boot.active) generation=TCQANext(generation);
    sTCQABootOwner=owner;sTCQA.boot=*facts;sTCQA.boot.active=1;
    sTCQA.boot.ownerGeneration=generation;sTCQA.boot.revision=revision;sTCQA.revision=TCQANext(sTCQA.revision);
}
void NPCYNBootRetire(const void *owner) {
    if (!owner || owner!=sTCQABootOwner) return;
    sTCQABootOwner=NULL;sTCQA.boot.active=sTCQA.boot.ready=0;
    sTCQA.boot.revision=TCQANext(sTCQA.boot.revision);sTCQA.revision=TCQANext(sTCQA.revision);
}
static void TCQALedger(void) {
    if (!sTCQA.started || sTCQA.stage==HG_TC_QA_DONE || !sTCQAField || !sTCQAField->saveData) return;
    SaveData *save=sTCQAField->saveData;
    sTCQA.partyUnchanged=!memcmp(&sTCQAParty,SaveArray_Party_Get(save),sizeof(sTCQAParty));
    sTCQA.pcUnchanged=!memcmp(&sTCQAPC,SaveArray_PCStorage_Get(save),sizeof(sTCQAPC));
    sTCQA.bagUnchanged=!memcmp(&sTCQABag,Save_Bag_Get(save),sizeof(sTCQABag));
    sTCQA.varsFlagsUnchanged=!memcmp(&sTCQAFlags,Save_VarsFlags_Get(save),sizeof(sTCQAFlags));
    sTCQA.easyChatUnchanged=!memcmp(&sTCQAEasy,Save_EasyChat_Get(save),sizeof(sTCQAEasy));
    sTCQA.moneyUnchanged=sTCQAMoney==PlayerProfile_GetMoney(Save_PlayerData_GetProfile(save));
    memcpy(sTCQA.saveCard,Save_TrainerCard_Get(save),sizeof(sTCQAExpectedCard));
    sTCQA.prefixUnchanged=!memcmp(&sTCQAExpectedCard,sTCQA.saveCard,0x20);
    sTCQA.saveMatched=!memcmp(&sTCQAExpectedCard,sTCQA.saveCard,sizeof(sTCQAExpectedCard));
    memcpy(sTCQA.statsLive,Save_GameStats_Get(save),sTCQA.statsBytes);
    memcpy(sTCQA.statsExpected,sTCQAExpectedStats,sTCQA.statsBytes);
    sTCQA.statsMatched=!memcmp(sTCQA.statsLive,sTCQA.statsExpected,sTCQA.statsBytes);
    sTCQA.currentScore=GameStats_GetScore(sTCQAExpectedStats);
    sTCQA.currentSigned=GameStats_GetCapped(sTCQAExpectedStats,GAME_STAT_TRAINER_CARDS_SIGNED);
    TCQA_REQUIRE(sTCQA.partyUnchanged && sTCQA.pcUnchanged && sTCQA.bagUnchanged && sTCQA.varsFlagsUnchanged
        && sTCQA.easyChatUnchanged && sTCQA.moneyUnchanged && sTCQA.prefixUnchanged && sTCQA.statsMatched && sTCQA.saveMatched);
}
static void TCQABegin(void) {
    SaveData *save=sTCQAField->saveData;
    TCQA_REQUIRE(!sTCQA.started && !TrainerCard_SignatureExists(Save_TrainerCard_Get(save))
        && SaveArray_Party_sizeof()==sizeof(sTCQAParty) && PCStorage_sizeof()==sizeof(sTCQAPC)
        && Save_Bag_sizeof()==sizeof(sTCQABag) && Save_VarsFlags_sizeof()==sizeof(sTCQAFlags)
        && Save_EasyChat_sizeof()==sizeof(sTCQAEasy));
    sTCQA.statsBytes=GameStats_sizeof();TCQA_REQUIRE(sTCQA.statsBytes && sTCQA.statsBytes<=HG_TC_QA_STATS_CAPACITY);
    sTCQAExpectedStats=Heap_Alloc(HEAP_ID_FIELD2,sTCQA.statsBytes);TCQA_REQUIRE(sTCQAExpectedStats);
    memcpy(sTCQAExpectedStats,Save_GameStats_Get(save),sTCQA.statsBytes);
    memcpy(&sTCQAParty,SaveArray_Party_Get(save),sizeof(sTCQAParty));memcpy(&sTCQAPC,SaveArray_PCStorage_Get(save),sizeof(sTCQAPC));
    memcpy(&sTCQABag,Save_Bag_Get(save),sizeof(sTCQABag));memcpy(&sTCQAFlags,Save_VarsFlags_Get(save),sizeof(sTCQAFlags));
    memcpy(&sTCQAEasy,Save_EasyChat_Get(save),sizeof(sTCQAEasy));memcpy(&sTCQAExpectedCard,Save_TrainerCard_Get(save),sizeof(sTCQAExpectedCard));
    sTCQAMoney=PlayerProfile_GetMoney(Save_PlayerData_GetProfile(save));sTCQA.baselineScore=GameStats_GetScore(sTCQAExpectedStats);
    sTCQA.baselineSigned=GameStats_GetCapped(sTCQAExpectedStats,GAME_STAT_TRAINER_CARDS_SIGNED);
    sTCQA.started=1;sTCQA.stage=HG_TC_QA_RUNNING;TCQALedger();
}
void TrainerCardQACanvasObserved(const Window *canvas,int outerState,int innerState) {
    if (!sTCQA.started) return;
    TCQA_REQUIRE(canvas && canvas->bgConfig && canvas->pixelBuffer && canvas->bgId==1
        && canvas->tilemapLeft==4 && canvas->tilemapTop==9 && canvas->width==24 && canvas->height==8);
    sTCQA.ui=*HGNativeTrainerCardUIGetState();
    if (sTCQACanvasGeneration!=sTCQA.ui.ownerGeneration) {++sTCQA.editorEntries;sTCQACanvasGeneration=sTCQA.ui.ownerGeneration;}
    if (sTCQA.canvasBytes!=HG_TC_QA_CANVAS_BYTES || memcmp(sTCQA.canvas,canvas->pixelBuffer,HG_TC_QA_CANVAS_BYTES)) ++sTCQA.canvasSerial;
    memcpy(sTCQA.canvas,canvas->pixelBuffer,HG_TC_QA_CANVAS_BYTES);sTCQA.canvasBytes=HG_TC_QA_CANVAS_BYTES;
    sTCQA.canvasGeneration=sTCQA.ui.ownerGeneration;
    TCQAPackCanvas(sTCQA.expectedPayload,sTCQA.canvas);++sTCQA.snapshotSerial;
}
void TrainerCardQAConfirmObserved(void) {
    if (!sTCQA.started) return;
    TCQA_REQUIRE(sTCQAExpectedStats && sTCQA.canvasBytes==HG_TC_QA_CANVAS_BYTES);
    // Only this complete offline clone changes; the original Confirm handler owns both live calls.
    GameStats_AddScore(sTCQAExpectedStats,SCORE_EVENT_TRAINER_CARD_SIGNED);
    GameStats_Inc(sTCQAExpectedStats,GAME_STAT_TRAINER_CARDS_SIGNED);++sTCQA.confirms;TCQALedger();
}
void TrainerCardQAEditorPacked(void) {
    if (!sTCQA.started) return;
    TCQA_REQUIRE(sTCQA.canvasBytes==HG_TC_QA_CANVAS_BYTES);
    memcpy(sTCQAExpectedCard.signaturePointData,sTCQA.expectedPayload,sizeof(sTCQAExpectedCard.signaturePointData));
    ++sTCQA.editorExits;if (sTCQA.stage==HG_TC_QA_DIRECT_RUNNING)sTCQA.directComplete=1;TCQALedger();
}
void TrainerCardQAViewerObserved(const TrainerCardMain *viewer) {
    if (!sTCQA.started) return;
    TCQA_REQUIRE(viewer && viewer->card && viewer->args);
    sTCQA.ui=*HGNativeTrainerCardUIGetState();sTCQA.cachedScore=viewer->card->score;
    // The original card payload carries checksum after signature; no packed-offset alias is used here.
    TrainerCardData card;memcpy(&card,viewer->args,sizeof(card));
    sTCQA.cachedChecksum=card.checksum;sTCQA.cachedSignatureExists=viewer->card->signatureExists;
    memcpy(sTCQA.cachedSignature,viewer->card->signature,sizeof(sTCQA.cachedSignature));
    sTCQA.cachedSignatureMatched=!memcmp(sTCQA.cachedSignature,sTCQAExpectedCard.signaturePointData,sizeof(sTCQA.cachedSignature));
    sTCQA.viewerGeneration=sTCQA.ui.ownerGeneration;TCQALedger();
}
const HGNativeTrainerCardQAState *HGNativeTrainerCardQAGetState(void) {return &sTCQA;}
uint32_t HGNativeTrainerCardQARequest(uint32_t revision,uint32_t request) {
    if (!revision || revision!=sTCQA.revision || sTCQA.error || sTCQARequest || !sTCQA.fieldReady) return 0;
    if (request==HG_TC_QA_START ? sTCQA.started || !sTCQA.compatibleSave
        : request==HG_TC_QA_CHECKPOINT ? !sTCQA.started || sTCQA.stage!=HG_TC_QA_RUNNING
        : request==HG_TC_QA_DIRECT ? !sTCQA.started || sTCQA.stage!=HG_TC_QA_RUNNING || sTCQA.confirms!=2 || sTCQA.directLaunches
        : request==HG_TC_QA_FINISH ? !sTCQA.directComplete || sTCQA.confirms!=3 : 1) return 0;
    sTCQARequest=request;return 1;
}
static BOOL TCQADirectTask(TaskManager *task) {
    uint32_t *state=TaskManager_GetStatePtr(task);
    FieldSystem *field=TaskManager_GetFieldSystem(task);
    // Original scr_seq_0004 _0EF6: FadeScreen 6,1,0,BLACK; app wait; RestoreOverworld; FadeScreen 6,1,1,BLACK.
    switch (*state) {
    case 0:
        PaletteFadeUntilFinished(task);
        ResetVisibleHardwareWindows(PM_LCD_TOP);ResetVisibleHardwareWindows(PM_LCD_BOTTOM);
        *state=1;break;
    case 1:
        TrainerCardSignature_LaunchApp(field);*state=2;break;
    case 2:
        if (FieldSystem_ApplicationIsRunning(field)) break;
        CallTask_RestoreOverworld(task);*state=3;break;
    case 3:
        CallTask_FadeFromBlack(task);
        ResetVisibleHardwareWindows(PM_LCD_TOP);ResetVisibleHardwareWindows(PM_LCD_BOTTOM);
        *state=4;break;
    case 4:
        return TRUE;
    }
    return FALSE;
}
static void TrainerCardQAFieldTick(FieldSystem *field) {
    sTCQAField=field;sTCQA.revision=TCQANext(sTCQA.revision);sTCQA.frame=OS_GetVBlankCount();
    sTCQA.ui=*HGNativeTrainerCardUIGetState();sTCQA.menuIcons=HGNativeFieldMenuIcons();sTCQA.trainerSlot=UINT32_MAX;
    sTCQA.fieldReady=field && field->saveData && FieldSystem_IsFieldMapLoaded(field) && FieldSystem_IsFieldMapRunning(field)
        && !FieldSystem_TaskIsRunning(field) && !FieldSystem_IsChildAppRunning(field) && FieldSystem_IsPlayerMovementAllowed(field) && IsPaletteFadeFinished();
    for (uint32_t slot=0;slot<7;++slot) if (((sTCQA.menuIcons>>(slot*4))&15)==4) {
        const TouchscreenHitbox *box=&gTouchscreenMenuIconHitboxes[slot+1];
        sTCQA.trainerSlot=slot;sTCQA.trainerX=(box->rect.left+box->rect.right)/2;sTCQA.trainerY=(box->rect.top+box->rect.bottom)/2;
    }
    if (!sTCQA.started && field && field->saveData) sTCQA.compatibleSave=!TrainerCard_SignatureExists(Save_TrainerCard_Get(field->saveData)) && sTCQA.trainerSlot<7;
    if (sTCQA.started) TCQALedger();
    if (sTCQA.stage==HG_TC_QA_DIRECT_RUNNING && sTCQA.directComplete && sTCQA.fieldReady)sTCQA.stage=HG_TC_QA_RUNNING;
    if (sTCQA.error || !sTCQARequest || !sTCQA.fieldReady) return;
    uint32_t request=sTCQARequest;sTCQARequest=0;
    if (request==HG_TC_QA_START) TCQABegin();
    else if (request==HG_TC_QA_CHECKPOINT) {TCQALedger();++sTCQA.checkpoints;}
    else if (request==HG_TC_QA_DIRECT) {sTCQA.stage=HG_TC_QA_DIRECT_RUNNING;++sTCQA.directLaunches;FieldSystem_CreateTask(field,TCQADirectTask,NULL);}
    else if (request==HG_TC_QA_FINISH) {TCQALedger();if (sTCQA.error)return;Heap_Free(sTCQAExpectedStats);sTCQAExpectedStats=NULL;sTCQA.stage=HG_TC_QA_DONE;}
}
