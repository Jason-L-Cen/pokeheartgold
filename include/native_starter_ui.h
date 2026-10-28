// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef HEARTGOLD_NATIVE_STARTER_UI_H
#define HEARTGOLD_NATIVE_STARTER_UI_H
#include <stdint.h>

typedef struct HGNativeStarterUIState {
    uint32_t active, ownerGeneration, revision, ready, phase, selection, selectionState;
} HGNativeStarterUIState;

const HGNativeStarterUIState *HGNativeStarterUIGetState(void);
#ifdef HEARTGOLD_NATIVE
void NativeStarterUIBegin(const void *owner);
void NativeStarterUIPublish(const void *owner, uint32_t phase, uint32_t selection, uint32_t selectionState, uint32_t ready);
void NativeStarterUIRetire(const void *owner);
#endif

// These are the four original getInput PAD actions, qualified against the displayed owner/frame.
static inline uint32_t HGNativeStarterUIKeyAllowed(const HGNativeStarterUIState *state,
    uint32_t ownerGeneration, uint32_t revision, uint32_t keys) {
    if (!state || state->active != 1 || state->ready != 1 || state->phase != 5 || state->selection >= 3
        || state->selectionState > 2 || !ownerGeneration || !revision
        || state->ownerGeneration != ownerGeneration || state->revision != revision) return 0;
    return keys == 1 || (keys == 2 && state->selectionState == 2)
        || ((keys == 16 || keys == 32) && state->selectionState != 2);
}

static inline uint32_t HGNativeStarterUITouchAllowed(const HGNativeStarterUIState *state,
    uint32_t ownerGeneration, uint32_t revision, uint32_t touchEngine, uint32_t x, uint32_t y) {
    return HGNativeStarterUIKeyAllowed(state, ownerGeneration, revision, 1)
        && touchEngine == 0 && x < 256 && y < 192;
}

#endif
