// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef HG_PRIVATE_NPC_YESNO_QA_H
#define HG_PRIVATE_NPC_YESNO_QA_H
#include <stdint.h>
#include "native_field_choice_ui.h"

enum { HG_YN_QA_VERSION=2, HG_YN_QA_START=1, HG_YN_QA_RETURN=2,
    HG_YN_QA_IDLE=0, HG_YN_QA_WARP_OUT=1, HG_YN_QA_ARRIVING=2,
    HG_YN_QA_READY=3, HG_YN_QA_WARP_BACK=4, HG_YN_QA_RETURNING=5, HG_YN_QA_DONE=6,
    HG_YN_QA_MAP=415, HG_YN_QA_SCRIPT=2, HG_YN_QA_X=8, HG_YN_QA_Z=6 };
enum { HG_YN_BOOT_TITLE=1, HG_YN_BOOT_MAIN_MENU=2 };

typedef struct HGYNBootFacts {
    uint32_t kind,active,ownerGeneration,revision,ready,phase,inputMode,option,notice,exit,progressFlags;
} HGYNBootFacts;

typedef struct HGNativeNPCYesNoQAState {
    uint32_t version,byteSize,revision,error,stage,frame;
    uint32_t fieldReady,mapId,x,z,facing,compatibleSave,flagABAClear,trendySayingsRemain;
    uint32_t baselinePrepared,script2Active,messageId,messageSerial,specialResult,noResultObserved;
    uint32_t waitButtonReady,waitButtonSerial,waitButtonMessageSerial,waitButtonAckSerial;
    uint32_t completedNo,checkpointSerial,conservationChecks;
    uint32_t partyUnchanged,pcUnchanged,bagUnchanged,varsFlagsUnchanged,easyChatUnchanged,moneyUnchanged;
    uint32_t phase5Serial,phase6Serial;
    HGNativeFieldChoiceUIState choice;
    HGYNBootFacts boot;
    uint32_t yesBeforeResult,newWordId,yesSerial,yesMessageSerial,yesFlagSet;
    uint32_t varsFlagsLedgerMatched,easyChatLedgerMatched,completedYes,dailySeen,completedDaily;
    uint32_t printerWaitReady,printerWaitSerial,printerWaitMessageSerial,ledgerPrepared;
} HGNativeNPCYesNoQAState;

// Private exports: original tasks own warps; all four interactions use physical input.
const HGNativeNPCYesNoQAState *HGNativeNPCYesNoQAGetState(void);
uint32_t HGNativeNPCYesNoQARequest(uint32_t revision,uint32_t kind);

// Private source-copy hooks only. Resident owns active/generation/revision fields.
void NPCYNBootPublish(const void *owner,const HGYNBootFacts *facts);
void NPCYNBootRetire(const void *owner);
#endif
