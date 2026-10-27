// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef HEARTGOLD_NATIVE_POKEGEAR_UI_H
#define HEARTGOLD_NATIVE_POKEGEAR_UI_H
#include <stdint.h>
enum { HG_POKEGEAR_OWNER_GEAR=1, HG_POKEGEAR_OWNER_FLY_MAP=2, HG_POKEGEAR_OWNER_TOWN_MAP=3 };
enum { HG_POKEGEAR_CONFIGURE=0, HG_POKEGEAR_RADIO=1, HG_POKEGEAR_MAP=2, HG_POKEGEAR_PHONE=3,
    HG_POKEGEAR_STANDALONE_MAP=4, HG_POKEGEAR_EASY_CHAT=5, HG_POKEGEAR_NO_VIEW=6 };
typedef struct HGNativePokegearUIState {
    uint32_t active,ownerGeneration,revision,ready;
    uint32_t ownerKind,parentPhase,viewActive,viewKind;
    uint32_t phase,app,registeredCards,scriptedLaunch;
} HGNativePokegearUIState;
// Owner thread only. Copy with original surfaces; Easy Chat owns a separate original screen while viewActive is zero.
const HGNativePokegearUIState *HGNativePokegearUIGetState(void);
#ifdef HEARTGOLD_NATIVE
void NativePokegearUIBegin(const void *owner,uint32_t kind);
void NativePokegearUIBeginView(const void *owner,void *view,uint32_t kind);
void NativePokegearUIParentState(const void *owner,const HGNativePokegearUIState *facts);
void NativePokegearUIPublish(const void *owner,void *view,const HGNativePokegearUIState *facts);
void NativePokegearUIEndView(const void *owner,const void *view);
void NativePokegearUIRetire(const void *owner);
#endif
static inline uint32_t HGNativePokegearUICardAvailable(uint32_t cards,uint32_t app) {
    return cards<=3 && (app==0 || app==3 || (app==1 && (cards&2)) || (app==2 && (cards&1)));
}
static inline uint32_t HGNativePokegearUIIsInputPhase(uint32_t view,uint32_t phase) {
    switch(view) {
    case HG_POKEGEAR_CONFIGURE:return phase==1 || phase==3;
    case HG_POKEGEAR_RADIO:return phase==1;
    case HG_POKEGEAR_MAP:return phase==1 || phase==9;
    case HG_POKEGEAR_PHONE:return phase==1 || phase==6 || phase==7 || phase==8 || phase==9;
    case HG_POKEGEAR_STANDALONE_MAP:return phase==1 || phase==12;
    default:return 0;
    }
}
static inline uint32_t HGNativePokegearUIInputAllowed(const HGNativePokegearUIState *state,uint32_t generation,uint32_t revision) {
    return state && state->active==1 && state->ready==1 && state->viewActive==1 && generation && revision
        && state->ownerGeneration==generation && state->revision==revision && state->registeredCards<=3
        && HGNativePokegearUIIsInputPhase(state->viewKind,state->phase)
        && ((state->ownerKind==HG_POKEGEAR_OWNER_GEAR && state->viewKind==state->app
                && state->parentPhase==(state->app==0 ? 3u : state->app==1 ? 5u : state->app==2 ? 7u : 11u)
                && HGNativePokegearUICardAvailable(state->registeredCards,state->app))
            || (state->ownerKind==HG_POKEGEAR_OWNER_FLY_MAP && state->viewKind==HG_POKEGEAR_STANDALONE_MAP && state->scriptedLaunch==0)
            || (state->ownerKind==HG_POKEGEAR_OWNER_TOWN_MAP && state->viewKind==HG_POKEGEAR_STANDALONE_MAP && state->scriptedLaunch==1));
}
static inline uint32_t HGNativePokegearUIResolveKey(const HGNativePokegearUIState *state,uint32_t generation,uint32_t revision,uint32_t key,uint32_t *keys) {
    if(!keys || !HGNativePokegearUIInputAllowed(state,generation,revision))return 0;
    switch(key){case 1:case 2:case 16:case 32:case 64:case 128:case 1024:case 2048:*keys=key;return 1;default:return 0;}
}
static inline uint32_t HGNativePokegearUITouchHeldAllowed(const HGNativePokegearUIState *state,uint32_t generation,uint32_t revision,uint32_t x,uint32_t y) {
    return x<256 && y<192 && HGNativePokegearUIInputAllowed(state,generation,revision);
}
static inline uint32_t HGNativePokegearUITouchStartAllowed(const HGNativePokegearUIState *state,uint32_t generation,uint32_t revision,uint32_t x,uint32_t y) {
    if(!HGNativePokegearUITouchHeldAllowed(state,generation,revision,x,y))return 0;
    // Only the ordinary input loop delegates this footer to the shared app switcher.
    if(state->ownerKind==HG_POKEGEAR_OWNER_GEAR && state->phase==1 && y>=160) {
        if(x>=56 && x<104 && !(state->registeredCards&2))return 0;
        if(x>=104 && x<152 && !(state->registeredCards&1))return 0;
    }
    return 1;
}
#endif
