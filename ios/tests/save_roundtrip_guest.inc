// SPDX-License-Identifier: GPL-3.0-or-later
// Private resident observer: reuse the real Trainer observer and its two private exports.
#define HG_SAVE_ROUNDTRIP_GUEST 1
#include "save_roundtrip_qa.h"
#define HGNativeTrainerCardQAGetState SRBaseTrainerGetState
#define HGNativeTrainerCardQARequest SRBaseTrainerRequest
#include "trainer_card_guest.inc"
#undef HGNativeTrainerCardQAGetState
#undef HGNativeTrainerCardQARequest
#include "sys_task_api.h"
#include "sys_task.h"
static HGSaveRoundtripQAState sSR={.srVersion=HG_SR_STATE_VERSION,.srBytes=sizeof(HGSaveRoundtripQAState),.revision=1,.saveSlot=UINT32_MAX};
static PCStorage sSRExpectedPC;
static const void *sSRSaveOwner;
static SysTask *sSRPromptTask;
static uint32_t sSRPromptReady;
#define SR_REQUIRE(value) do {if(!(value)){if(!sSR.error)sSR.error=__LINE__;return;}}while(0)
static void SRFreeze(SaveData *save,HGSaveRoundtripSnapshot *snapshot) {
    SR_REQUIRE(save && SaveArray_Party_sizeof()==sizeof(Party) && PCStorage_sizeof()==sizeof(PCStorage)
        && Save_Bag_sizeof()==sizeof(Bag) && Save_VarsFlags_sizeof()==sizeof(SaveVarsFlags)
        && Save_TrainerCard_sizeof()==sizeof(struct SaveTrainerCard) && SaveData_Pokegear_sizeof()==sizeof(SavePokegear) && sizeof(SavePokegear)==1624);
    const void *records[]={SaveArray_Party_Get(save),SaveArray_PCStorage_Get(save),Save_Bag_Get(save),Save_VarsFlags_Get(save),Save_TrainerCard_Get(save),SaveData_Pokegear_Get(save)};
    const uint32_t sizes[]={sizeof(Party),sizeof(PCStorage),sizeof(Bag),sizeof(SaveVarsFlags),sizeof(struct SaveTrainerCard),sizeof(SavePokegear)};
    snapshot->version=HG_SR_VERSION;uint32_t offset=0;
    for(uint32_t i=0;i<HG_SR_RECORDS;i++) {SR_REQUIRE(records[i]);snapshot->lengths[i]=sizes[i];memcpy(snapshot->records+offset,records[i],sizes[i]);offset+=sizes[i];}
    PlayerProfile *profile=Save_PlayerData_GetProfile(save);SR_REQUIRE(profile);
    snapshot->money=PlayerProfile_GetMoney(profile);snapshot->trainerID=PlayerProfile_GetTrainerID(profile);
    SR_REQUIRE(offset==HG_SR_RECORD_BYTES);sSR.snapshotSerial=TCQANext(sSR.snapshotSerial);sSR.revision=TCQANext(sSR.revision);
}
static uint32_t SRPreservationValid(void) {
    const HGSaveRoundtripSnapshot *before=&sSR.beforeSnapshot,*after=&sSR.snapshot;
    if(!HGSRWireValid(before,sizeof(*before)) || !HGSRWireValid(after,sizeof(*after))
        || before->money!=after->money || before->trainerID!=after->trainerID)return 0;
    memcpy(&sSRExpectedPC,before->records+sizeof(Party),sizeof(sSRExpectedPC));
    // This is the exact original helper called by Save_ResetPCBoxModifiedFlags; the live PC record is never passed.
    PCStorage_ResetBoxModifiedFlags(&sSRExpectedPC);
    uint32_t offset=0;
    for(uint32_t i=0;i<HG_SR_RECORDS;i++) {
        const void *expected=i==1 ? (const void *)&sSRExpectedPC : before->records+offset;
        if(memcmp(expected,after->records+offset,before->lengths[i]))return 0;
        offset+=before->lengths[i];
    }
    return 1;
}
void SaveRoundtripQANormalEntry(SaveData *save) {
    if(sSR.role!=HG_SR_WRITER)return;
    SR_REQUIRE(!sSR.beforeFrozen && !sSR.writerFrozen && sTCQA.stage==HG_TC_QA_DONE);
    SRFreeze(save,&sSR.beforeSnapshot);if(!sSR.error)sSR.beforeFrozen=1;
}
static void SRPromptCancel(void) {
    sSRPromptReady=0;if(sSRPromptTask)SysTask_Destroy(sSRPromptTask);sSRPromptTask=NULL;
}
static void SRPromptUploaded(SysTask *task,void *owner) {
    if(task==sSRPromptTask && owner==sSRSaveOwner && sSR.saveActive) {sSRPromptTask=NULL;sSRPromptReady=1;}
    SysTask_Destroy(task);
}
void SaveRoundtripQAPromptQueued(const void *owner) {
    if(sSR.role!=HG_SR_WRITER || !owner || owner!=sSRSaveOwner)return;
    SRPromptCancel();sSRPromptTask=SysTask_CreateOnVWaitQueue(SRPromptUploaded,(void *)owner,0x81);
}
void SaveRoundtripQAUI(const void *owner,uint32_t phase,uint32_t ready,uint32_t cursor,uint32_t touch) {
    if(sSR.role!=HG_SR_WRITER || !owner)return;
    if(owner!=sSRSaveOwner) {SRPromptCancel();sSRSaveOwner=owner;sSR.saveGeneration=TCQANext(sSR.saveGeneration);sSR.saveRevision=TCQANext(sSR.saveRevision);}
    ready=ready==1 && sSRPromptReady && (phase==4 || phase==7) && !sSR.managerLive;
    if(!sSR.saveActive || sSR.savePhase!=phase || sSR.saveReady!=ready || sSR.promptCursor!=cursor || sSR.promptTouchMode!=touch) sSR.saveRevision=TCQANext(sSR.saveRevision);
    sSR.saveActive=1;sSR.savePhase=phase;sSR.saveReady=ready;sSR.promptCursor=cursor;sSR.promptTouchMode=touch;
    if(phase!=4 && phase!=7 && phase!=3 && phase!=6)SRPromptCancel();
}
void SaveRoundtripQAUIRetire(const void *owner) {
    if(!owner || owner!=sSRSaveOwner)return;
    SRPromptCancel();sSRSaveOwner=NULL;sSR.saveActive=sSR.saveReady=0;sSR.saveRevision=TCQANext(sSR.saveRevision);
}
void SaveRoundtripQAManager(SaveData *save,const struct AsyncWriteManager *manager,uint32_t live) {
    if(sSR.role!=HG_SR_WRITER)return;
    SR_REQUIRE(save && manager && manager!=&save->asyncWriteMan);
    sSR.managerIsStack=1;sSR.managerLive=live;sSR.managerAddress=live ? (uint32_t)(uaddr)manager : 0;
    sSR.managerWaiting=live && manager->waitingAsync;sSR.saveReady=0;
}
void SaveRoundtripQAFlash(int lock,uint32_t check,uint32_t done,uint32_t success) {
    (void)lock;(void)check;(void)success;if(sSR.role!=HG_SR_WRITER)return;
    ++sSR.flashPolls;if(done)++sSR.flashCompleted;else ++sSR.flashBusyPolls;
}
void SaveRoundtripQANormalResult(SaveData *save,int result) {
    if(sSR.role!=HG_SR_WRITER)return;
    ++sSR.saveCalls;sSR.lastSaveStatus=(uint32_t)result;
    SR_REQUIRE(result==WRITE_STATUS_SUCCESS && result==2 && sSR.managerIsStack && !sSR.managerLive && sSR.flashCompleted && !sSR.writerFrozen);
    SR_REQUIRE(sTCQA.stage==HG_TC_QA_DONE && sTCQA.confirms==1 && sTCQA.editorExits==1 && TrainerCard_SignatureExists(Save_TrainerCard_Get(save)));
    SR_REQUIRE(sSR.beforeFrozen);SRFreeze(save,&sSR.snapshot);if(sSR.error)return;
    sSR.preservationChecked=1;sSR.preservationPassed=SRPreservationValid();SR_REQUIRE(sSR.preservationPassed);
    sSR.writerFrozen=1;++sSR.saveSuccess;
}
void SaveRoundtripQALoaded(SaveData *save) {
    if(sSR.role!=HG_SR_READER)return;
    ++sSR.loadSuccess;SR_REQUIRE(!sSR.readerFrozen && sSR.loadSuccess==1);
    SRFreeze(save,&sSR.snapshot);if(!sSR.error)sSR.readerFrozen=1;
}
void SaveRoundtripQAFieldTick(FieldSystem *field) {
    TrainerCardQAFieldTick(field);sSR.saveSlot=UINT32_MAX;
    uint32_t icons=HGNativeFieldMenuIcons();
    for(uint32_t slot=0;slot<7;slot++)if(((icons>>(slot*4))&15)==5) {
        const TouchscreenHitbox *box=&gTouchscreenMenuIconHitboxes[slot+1];sSR.saveSlot=slot;sSR.saveX=(box->rect.left+box->rect.right)/2;sSR.saveY=(box->rect.top+box->rect.bottom)/2;
    }
}
const HGNativeTrainerCardQAState *HGNativeTrainerCardQAGetState(void) {
    sSR.trainer=*SRBaseTrainerGetState();return &sSR.trainer;
}
uint32_t HGNativeTrainerCardQARequest(uint32_t revision,uint32_t request) {
    if(request==HG_SR_CONFIG_WRITER || request==HG_SR_CONFIG_READER) {
        if(sSR.role || revision!=sSR.revision || sTCQA.frame || sTCQA.boot.active)return 0;
        sSR.role=request==HG_SR_CONFIG_WRITER ? HG_SR_WRITER : HG_SR_READER;sSR.revision=TCQANext(sSR.revision);return 1;
    }
    if(request==HG_SR_SEAL_TRAINER) {
        if(sSR.role!=HG_SR_WRITER || revision!=sSR.revision || sTCQA.stage!=HG_TC_QA_RUNNING || !sTCQA.fieldReady
            || sTCQA.confirms!=1 || sTCQA.editorExits!=1 || !TrainerCard_SignatureExists(&sTCQAExpectedCard))return 0;
        TCQALedger();if(sTCQA.error)return 0;
        Heap_Free(sTCQAExpectedStats);sTCQAExpectedStats=NULL;sTCQA.stage=HG_TC_QA_DONE;sSR.revision=TCQANext(sSR.revision);return 1;
    }
    return SRBaseTrainerRequest(revision,request);
}
