// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef HG_PRIVATE_PC_BOX_QA_H
#define HG_PRIVATE_PC_BOX_QA_H
#include <stdint.h>
#include "native_pc_box_ui.h"
#include "native_summary_ui.h"
#include "native_naming_ui.h"

enum { HG_PC_QA_VERSION=2, HG_PC_QA_MONS=4, HG_PC_QA_GRID=64,
    HG_PC_QA_START=1, HG_PC_QA_CHECKPOINT=2, HG_PC_QA_RETURN=3,
    HG_PC_QA_PARTY_LOCATION=0x10000, HG_PC_QA_REQUIRED=0x3ff };
enum { HG_PC_QA_PHYSICAL=1, HG_PC_QA_DEPOSIT=2, HG_PC_QA_WITHDRAW=4,
    HG_PC_QA_MOVE=8, HG_PC_QA_SWAP=16, HG_PC_QA_LAST=32,
    HG_PC_QA_RELEASE_CANCEL=64, HG_PC_QA_ITEMS=128,
    HG_PC_QA_SUMMARY=256, HG_PC_QA_NAMING=512 };
typedef struct HGPCBoxQAMon {
    uint32_t personality,otId,species,heldItem,location;
} HGPCBoxQAMon;
typedef struct HGPCBoxQAState {
    uint32_t version,byteSize,revision,error,stage,frame;
    uint32_t baselinePrepared,preparationSetterCalls,checkpointSerial,conservationChecks;
    uint32_t mapId,x,z,facing,pcTileBehavior,fieldReady,scriptMenuReady,scriptMenuSerial,scriptMenuCount;
    uint32_t scriptMenuValues[8];
    uint32_t partyCount,boxedCount,bagUnchanged,flagsUnchanged,moneyDexUnchanged,boxNameUnchanged;
    HGPCBoxQAMon mons[HG_PC_QA_MONS];
    HGNativePCBoxUIState pc;
    HGNativeSummaryUIState summary;
    HGNativeNamingUIState naming;
    uint32_t gridCount,gridCursor,gridReturn,gridButtons,gridEnabled[2],yesNoHandler;
    uint8_t neighbors[HG_PC_QA_GRID][4];
    uint32_t messageAwaitInput;
} HGPCBoxQAState;

// Private scenario exports only; requests queue an owner task or a read-only checkpoint.
const HGPCBoxQAState *HGNativePCBoxQAGetState(void);
uint32_t HGNativePCBoxQARequest(uint32_t revision,uint32_t kind);
#endif
