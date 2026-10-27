// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef HG_PRIVATE_POKEGEAR_QA_H
#define HG_PRIVATE_POKEGEAR_QA_H
#include <stdint.h>
#include "native_pokegear_ui.h"
#include "npc_yesno_qa.h"
enum { HG_PG_QA_VERSION=1,HG_PG_QA_START=1,HG_PG_QA_CHECKPOINT=2,HG_PG_QA_FINISH=3,
    HG_PG_QA_IDLE=0,HG_PG_QA_RUNNING=1,HG_PG_QA_DONE=2,HG_PG_QA_GEAR_CAPACITY=2048,HG_PG_QA_STATS_CAPACITY=512 };
// Private source witness; the host's functions must come from the genuine generated guest header.
typedef struct HGNativePokegearQAState {
    uint32_t version,byteSize,revision,error,stage,frame;
    uint32_t fieldReady,compatibleSave,menuIcons,pokegearSlot,pokegearX,pokegearY;
    uint32_t started,parentEntries,parentExits,phoneEntries,configureEntries,callSelections,callCompletions,coverage;
    uint32_t baselineSkin,expectedSkin,savedApp,savedSkin,registeredCards,gearBytes,snapshotSerial,checkpoints;
    uint32_t statsBytes,miscBytes,currentMap,ringActive,panicFlag,selectedSkin,configuredSkin,callTarget,callEffectType,viewFactsGeneration,baselineMoney,currentMoney;
    uint32_t partyUnchanged,pcUnchanged,bagUnchanged,varsFlagsUnchanged,easyChatUnchanged,moneyUnchanged;
    uint32_t trainerCardUnchanged,statsUnchanged,miscUnchanged,gearMatched;
    uint32_t menuInputState,cursorInAppSwitchZone,footerApp,contactCount,contactCursor,selectedContact,menuCursor,menuTouchMode;
    uint32_t callMainState,callScriptState,callScriptType,callScriptID,callIncoming,callScripted,callAwaitHangup,callTextWait,callTextSerial;
    HGNativePokegearUIState ui;
    HGYNBootFacts boot;
    uint32_t slotContacts[6];
    uint8_t gearLive[HG_PG_QA_GEAR_CAPACITY],gearExpected[HG_PG_QA_GEAR_CAPACITY];
    uint8_t statsLive[HG_PG_QA_STATS_CAPACITY],statsExpected[HG_PG_QA_STATS_CAPACITY];
} HGNativePokegearQAState;
const HGNativePokegearQAState *HGNativePokegearQAGetState(void);
uint32_t HGNativePokegearQARequest(uint32_t revision,uint32_t request);
#ifdef HG_POKEGEAR_QA_GUEST
struct FieldSystem;
struct PokegearAppData;
struct PokegearPhoneAppData;
struct PokegearConfigureAppData;
struct PokegearPhoneCallContext;
void PokegearQAFieldObserved(struct FieldSystem *field);
void PokegearQAParentObserved(const struct PokegearAppData *owner,int phase);
void PokegearQAParentExitObserved(const struct PokegearAppData *owner);
void PokegearQAPhoneObserved(const struct PokegearPhoneAppData *phone,int phase);
void PokegearQACallCompleted(const struct PokegearPhoneAppData *phone);
void PokegearQAConfigureObserved(const struct PokegearConfigureAppData *configure,int phase);
void PokegearQAConfigureExitObserved(const struct PokegearConfigureAppData *configure);
void PokegearQACallSelected(const struct PokegearPhoneCallContext *call);
void PokegearQACallObserved(const struct PokegearPhoneCallContext *call);
#endif
#endif
