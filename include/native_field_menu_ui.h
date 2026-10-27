// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef HEARTGOLD_NATIVE_FIELD_MENU_UI_H
#define HEARTGOLD_NATIVE_FIELD_MENU_UI_H
#include <stdint.h>
typedef struct HGNativeFieldMenuUIState {
    uint32_t active,ownerGeneration,revision,ready;
    uint32_t phase,viewActive,controlActive,childActive,inputMode;
} HGNativeFieldMenuUIState;
// Owner thread only; raw StartMenu lifetime includes child overlays and return/fade gaps.
const HGNativeFieldMenuUIState *HGNativeFieldMenuUIGetState(void);
#ifdef HEARTGOLD_NATIVE
void NativeFieldMenuUIBegin(const void *owner);
void NativeFieldMenuUIBeginView(const void *owner);
void NativeFieldMenuUIEndView(const void *owner);
void NativeFieldMenuUIPublish(const void *owner,const HGNativeFieldMenuUIState *facts);
void NativeFieldMenuUIRetire(const void *owner);
void NativeFieldMenuUIReset(void);
#endif
static inline uint32_t HGNativeFieldMenuUIInputAllowed(const HGNativeFieldMenuUIState *state,uint32_t generation,uint32_t revision) {
    return state && state->active==1 && state->ready==1 && state->phase==3 && state->viewActive==1
        && state->controlActive==1 && state->childActive==0 && state->inputMode<=1
        && generation && revision && state->ownerGeneration==generation && state->revision==revision;
}
static inline uint32_t HGNativeFieldMenuUIResolveKey(const HGNativeFieldMenuUIState *state,uint32_t generation,uint32_t revision,uint32_t key,uint32_t *keys) {
    if(!keys || !HGNativeFieldMenuUIInputAllowed(state,generation,revision))return 0;
    switch(key){case 1:case 2:case 16:case 32:case 64:case 128:case 1024:*keys=key;return 1;default:return 0;}
}
static inline uint32_t HGNativeFieldMenuUITouchAllowed(const HGNativeFieldMenuUIState *state,uint32_t generation,uint32_t revision,uint32_t x,uint32_t y) {
    return x<256 && y<192 && HGNativeFieldMenuUIInputAllowed(state,generation,revision);
}
#endif
