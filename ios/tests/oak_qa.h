// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef HG_PRIVATE_OAK_QA_H
#define HG_PRIVATE_OAK_QA_H
#include "npc_yesno_qa.h"
#include "native_oak_ui.h"
#include "native_naming_ui.h"
enum { HG_OAK_QA_VERSION=1,HG_OAK_QA_MAP=64,HG_OAK_QA_NEW_GAME=2 };
enum { HG_OAK_QA_MAIN_ORDINARY,HG_OAK_QA_MAIN_SELECTED,HG_OAK_QA_MAIN_TERMINAL };
typedef struct HGOakNamingFacts {
    uint32_t phase,pageSwitchState,pageNum,type,maxLen,gender,cursorX,cursorY;
    uint32_t keyboardReady,confirmReady,glyphCount,glyph[2],glyphX[2],glyphY[2],length,name[8];
} HGOakNamingFacts;
typedef struct HGNativeOakQAState {
    uint32_t version,byteSize,revision,error,frame;
    HGYNBootFacts boot;
    HGNativeOakUIState ui;
    HGNativeNamingUIState naming;
    HGOakNamingFacts edit;
    uint32_t mainEmpty,mainSelectedNewGame,mainRouteMask,mainTerminal;
    uint32_t oakEntries,oakExits,namingEntries,namingExits;
    uint32_t fieldReady,mapId,x,z,facing,betweenSteps,moveState,playerMoveState;
    uint32_t partyCount,gotStarter,trainerGender;
    uint32_t resultName[8],profileName[8];
} HGNativeOakQAState;
_Static_assert(sizeof(HGOakNamingFacts)==104 && sizeof(HGNativeOakQAState)==396,"private Oak396 ABI");
const HGNativeOakQAState *HGNativeOakQAGetState(void);
void OakQAObserved(const void *owner,uint32_t parentPhase);
void OakQAMainMenuObserved(uint32_t phase,uint32_t empty,uint32_t selected,uint32_t checkpoint);
void OakQANamingObserved(const void *owner,const HGOakNamingFacts *facts);
void OakQANamingResultObserved(const void *owner,const uint16_t *name,uint32_t length,uint32_t noInput);
static inline uint32_t HGOakTitleReady(const HGYNBootFacts *b) {
    return b && b->kind==HG_YN_BOOT_TITLE && b->active==1 && b->ready==1 && b->ownerGeneration && b->revision
        && b->phase==2 && !b->notice && !b->exit && (b->progressFlags&15)==15;
}
static inline uint32_t HGOakNamingReady(const HGNativeOakQAState *s,uint32_t generation,uint32_t revision) {
    return s && s->ui.active==1 && s->ui.parentPhase==4
        && HGNativeNamingUIInputAllowed(&s->naming,generation,revision) && s->edit.keyboardReady==1;
}
static inline uint32_t HGOakFieldReady(const HGNativeOakQAState *s) {
    return s && s->fieldReady==1 && s->mapId==HG_OAK_QA_MAP && s->betweenSteps==1 && !s->moveState
        && (s->playerMoveState==0 || s->playerMoveState==3) && !s->ui.active && !s->naming.active;
}
#endif
