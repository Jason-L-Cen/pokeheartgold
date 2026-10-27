// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef HG_PRIVATE_TRAINER_CARD_QA_H
#define HG_PRIVATE_TRAINER_CARD_QA_H
#include <stdint.h>
#include "native_trainer_card_ui.h"
#include "npc_yesno_qa.h"
enum { HG_TC_QA_VERSION=1, HG_TC_QA_START=1, HG_TC_QA_CHECKPOINT=2, HG_TC_QA_DIRECT=3, HG_TC_QA_FINISH=4,
    HG_TC_QA_IDLE=0, HG_TC_QA_RUNNING=1, HG_TC_QA_DIRECT_RUNNING=2, HG_TC_QA_DONE=3,
    HG_TC_QA_CANVAS_BYTES=6144, HG_TC_QA_SIGNATURE_BYTES=1536, HG_TC_QA_STATS_CAPACITY=512 };
typedef struct HGNativeTrainerCardQAState {
    uint32_t version,byteSize,revision,error,stage,frame;
    uint32_t fieldReady,compatibleSave,menuIcons,trainerSlot,trainerX,trainerY;
    uint32_t started,confirms,editorEntries,editorExits,canvasSerial,snapshotSerial,checkpoints,coverage;
    uint32_t partyUnchanged,pcUnchanged,bagUnchanged,varsFlagsUnchanged,easyChatUnchanged,moneyUnchanged;
    uint32_t prefixUnchanged,statsMatched,saveMatched,baselineScore,baselineSigned,currentScore,currentSigned;
    uint32_t cachedScore,cachedChecksum,cachedSignatureExists,cachedSignatureMatched,viewerGeneration,directLaunches,directComplete;
    uint32_t statsBytes,canvasBytes,canvasGeneration;
    HGNativeTrainerCardUIState ui;
    HGYNBootFacts boot;
    uint8_t canvas[HG_TC_QA_CANVAS_BYTES],saveCard[0x620],expectedPayload[HG_TC_QA_SIGNATURE_BYTES];
    uint8_t cachedSignature[HG_TC_QA_SIGNATURE_BYTES];
    uint8_t statsLive[HG_TC_QA_STATS_CAPACITY],statsExpected[HG_TC_QA_STATS_CAPACITY];
} HGNativeTrainerCardQAState;
const HGNativeTrainerCardQAState *HGNativeTrainerCardQAGetState(void);
uint32_t HGNativeTrainerCardQARequest(uint32_t revision,uint32_t request);
#ifdef HG_TRAINER_CARD_QA_GUEST
struct Window;
struct TrainerCardMain;
void TrainerCardQACanvasObserved(const struct Window *canvas,int outerState,int innerState);
void TrainerCardQAConfirmObserved(void);
void TrainerCardQAEditorPacked(void);
void TrainerCardQAViewerObserved(const struct TrainerCardMain *viewer);
#endif
#endif
