// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef HG_PRIVATE_SAVE_ROUNDTRIP_QA_H
#define HG_PRIVATE_SAVE_ROUNDTRIP_QA_H
#include <stdint.h>
#include <string.h>
#include "trainer_card_qa.h"
#include "save.h"
#include "party.h"
#include "pokemon_storage_system.h"
#include "bag.h"
#include "save_vars_flags.h"
#include "save_trainer_card.h"
#include "save_pokegear.h"
enum { HG_SR_VERSION=2,HG_SR_STATE_VERSION=3,HG_SR_RECORDS=6,HG_SR_WRITER=1,HG_SR_READER=2,
    HG_SR_CONFIG_WRITER=0x100,HG_SR_CONFIG_READER=0x101,HG_SR_SEAL_TRAINER=0x102,
    HG_SR_RECORD_BYTES=sizeof(Party)+sizeof(PCStorage)+sizeof(Bag)+sizeof(SaveVarsFlags)+sizeof(struct SaveTrainerCard)+sizeof(SavePokegear) };
typedef struct HGSaveRoundtripSnapshot {
    uint32_t version,lengths[HG_SR_RECORDS];
    uint32_t money,trainerID;
    uint8_t records[HG_SR_RECORD_BYTES];
} HGSaveRoundtripSnapshot;
typedef struct HGSaveRoundtripQAState {
    // Keep the existing private Trainer observer as the prefix; only its original two exports are used.
    HGNativeTrainerCardQAState trainer;
    uint32_t srVersion,srBytes,role,error,revision;
    uint32_t saveSlot,saveX,saveY,saveActive,saveGeneration,saveRevision,savePhase,saveReady,promptCursor,promptTouchMode;
    uint32_t saveCalls,saveSuccess,lastSaveStatus,writerFrozen,readerFrozen,snapshotSerial,loadSuccess;
    uint32_t managerLive,managerAddress,managerIsStack,managerWaiting,flashPolls,flashBusyPolls,flashCompleted;
    uint32_t beforeFrozen,preservationChecked,preservationPassed;
    HGSaveRoundtripSnapshot beforeSnapshot;
    HGSaveRoundtripSnapshot snapshot;
} HGSaveRoundtripQAState;
static inline uint32_t HGSRSaveInputAllowed(const HGSaveRoundtripQAState *state,uint32_t generation,uint32_t revision) {
    return state && state->srVersion==HG_SR_STATE_VERSION && state->srBytes==sizeof(*state) && !state->error && state->role==HG_SR_WRITER
        && state->saveActive==1 && state->saveReady==1 && (state->savePhase==4 || state->savePhase==7)
        && !state->managerLive && state->promptCursor<2 && state->promptTouchMode<=1 && generation && revision
        && generation==state->saveGeneration && revision==state->saveRevision;
}
static inline uint32_t HGSRWireValid(const void *bytes,uint32_t size) {
    if(!bytes || size!=sizeof(HGSaveRoundtripSnapshot))return 0;
    uint32_t header[9];memcpy(header,bytes,sizeof(header));
    return header[0]==HG_SR_VERSION && header[1]==sizeof(Party) && header[2]==sizeof(PCStorage)
        && header[3]==sizeof(Bag) && header[4]==sizeof(SaveVarsFlags) && header[5]==sizeof(struct SaveTrainerCard) && header[6]==sizeof(SavePokegear);
}
#ifdef HG_SAVE_ROUNDTRIP_GUEST
struct FieldSystem;
struct SaveData;
struct AsyncWriteManager;
void SaveRoundtripQAFieldTick(struct FieldSystem *field);
void SaveRoundtripQAPromptQueued(const void *owner);
void SaveRoundtripQAUI(const void *owner,uint32_t phase,uint32_t ready,uint32_t cursor,uint32_t touch);
void SaveRoundtripQAUIRetire(const void *owner);
void SaveRoundtripQANormalResult(struct SaveData *save,int result);
void SaveRoundtripQANormalEntry(struct SaveData *save);
void SaveRoundtripQALoaded(struct SaveData *save);
void SaveRoundtripQAManager(struct SaveData *save,const struct AsyncWriteManager *manager,uint32_t live);
void SaveRoundtripQAFlash(int lock,uint32_t check,uint32_t done,uint32_t success);
#endif
#endif
