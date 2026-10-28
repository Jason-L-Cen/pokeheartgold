// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef HG_PRIVATE_STARTER_QA_H
#define HG_PRIVATE_STARTER_QA_H
#include "npc_yesno_qa.h"
#include "native_starter_ui.h"

// Map events add one to the source script indices 11 (intro) and 12 (machine).
enum { HG_STARTER_QA_VERSION=1, HG_STARTER_QA_MAP=61, HG_STARTER_QA_INTRO=12, HG_STARTER_QA_MACHINE=13 };
typedef struct HGNativeStarterQAState {
    uint32_t version,byteSize,revision,error,frame;
    uint32_t fieldReady,mapId,x,z,facing,betweenSteps,moveState,playerMoveState;
    uint32_t partyCount,species,level,dexSeen,dexCaught;
    uint32_t gotStarter,scene,scriptActive,scriptId,scriptWait,scriptSerial;
    uint32_t starterEntries,starterExits,grants,nicknameNo,choiceReturns;
    uint32_t probeQueries,probeUnchanged,probeGeneration,probeSelection,probeX,probeY,probeValid;
    HGYNBootFacts boot;
    HGNativeStarterUIState ui;
    HGNativeFieldChoiceUIState choice;
} HGNativeStarterQAState;
_Static_assert(sizeof(HGNativeStarterQAState)==252 && sizeof(HGYNBootFacts)==44
    && sizeof(HGNativeStarterUIState)==28 && sizeof(HGNativeFieldChoiceUIState)==36,"private fixed-width ABI");
const HGNativeStarterQAState *HGNativeStarterQAGetState(void);
uint32_t StarterQAProbeWanted(const void *owner);
void StarterQAProbeObserved(const void *owner,uint32_t queries,uint32_t x,uint32_t y,uint32_t found,uint32_t unchanged);
struct ScriptContext;
void StarterQAWaitButtonObserved(struct ScriptContext *ctx);
void StarterQAChoiceObserved(struct ScriptContext *ctx,uint32_t result);

static inline uint32_t HGStarterStationary(const HGNativeStarterQAState *s) {
    return s && s->fieldReady==1 && s->mapId==HG_STARTER_QA_MAP && s->betweenSteps==1
        && s->moveState==0 && (s->playerMoveState==0 || s->playerMoveState==3) && !s->scriptActive && !s->ui.active;
}
static inline uint32_t HGStarterBaseline(const HGNativeStarterQAState *s) {
    return HGStarterStationary(s) && s->x==4 && (s->z==7 || s->z==14) && !s->facing
        && !s->partyCount && !s->gotStarter && !s->scene && !s->starterEntries && !s->grants;
}
static inline uint32_t HGStarterStepValid(uint32_t key,uint32_t x,uint32_t z,uint32_t afterX,uint32_t afterZ) {
    if(key!=16 && key!=32 && key!=64 && key!=128)return 0;
    if(x==afterX && z==afterZ)return 1;
    return key==16 ? x<UINT32_MAX && afterX==x+1 && afterZ==z
        : key==32 ? x && afterX==x-1 && afterZ==z
        : key==64 ? z && afterZ==z-1 && afterX==x
        : key==128 ? z<UINT32_MAX && afterZ==z+1 && afterX==x : 0;
}
static inline uint32_t HGStarterTextAllowed(const HGNativeStarterQAState *s,uint32_t script,uint32_t serial) {
    return s && s->mapId==HG_STARTER_QA_MAP && s->scriptActive==1 && !s->ui.active
        && (script==HG_STARTER_QA_INTRO || script==HG_STARTER_QA_MACHINE) && s->scriptId==script
        && serial && s->scriptSerial==serial && (s->scriptWait==1 || s->scriptWait==2)
        && !(s->choice.active && (s->choice.viewActive || s->choice.ready));
}
#endif
