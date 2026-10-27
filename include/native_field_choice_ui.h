// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef HEARTGOLD_NATIVE_FIELD_CHOICE_UI_H
#define HEARTGOLD_NATIVE_FIELD_CHOICE_UI_H
#include <stdint.h>

enum {
    HG_FIELD_CHOICE_UI_SHOW_YES_NO = 3, HG_FIELD_CHOICE_UI_YES_NO_INPUT = 4, HG_FIELD_CHOICE_UI_YES_NO_END = 5,
    HG_FIELD_CHOICE_UI_SHOW_LIST = 7, HG_FIELD_CHOICE_UI_LIST_INPUT = 8,
    HG_FIELD_CHOICE_UI_LIST_END = 9, HG_FIELD_CHOICE_UI_LIST_WRITE_RESULT = 10
};

typedef struct HGNativeFieldChoiceUIState {
    uint32_t active, ownerGeneration, revision, ready, phase;
    uint32_t rowCount, selectedIndex, cancelEnabled, viewActive;
} HGNativeFieldChoiceUIState;

// Owner thread only. Active includes idle dialogue; YES/NO and list phases own a choice view.
const HGNativeFieldChoiceUIState *HGNativeFieldChoiceUIGetState(void);

#ifdef HEARTGOLD_NATIVE
// Original SysTask lifecycle only; these resident helpers are not shipping exports.
void NativeFieldChoiceUIBegin(const void *owner);
void NativeFieldChoiceUIPublish(const void *owner, const HGNativeFieldChoiceUIState *facts);
void NativeFieldChoiceUIInvalidate(const void *owner);
void NativeFieldChoiceUIRetire(const void *owner);
#endif

static inline uint32_t HGNativeFieldChoiceUIIsListPhase(uint32_t phase) {
    return phase >= HG_FIELD_CHOICE_UI_SHOW_LIST && phase <= HG_FIELD_CHOICE_UI_LIST_WRITE_RESULT;
}

static inline uint32_t HGNativeFieldChoiceUIIsViewPhase(uint32_t phase) {
    return (phase >= HG_FIELD_CHOICE_UI_SHOW_YES_NO && phase <= HG_FIELD_CHOICE_UI_YES_NO_END)
        || HGNativeFieldChoiceUIIsListPhase(phase);
}

static inline uint32_t HGNativeFieldChoiceUIIsYesNoInput(const HGNativeFieldChoiceUIState *state) {
    return state && state->phase == HG_FIELD_CHOICE_UI_YES_NO_INPUT && state->rowCount == 2
        && state->selectedIndex < 2 && state->cancelEnabled == 1;
}

static inline uint32_t HGNativeFieldChoiceUIInputAllowed(const HGNativeFieldChoiceUIState *state,
    uint32_t ownerGeneration, uint32_t revision) {
    return state && state->active == 1 && state->ready == 1 && state->viewActive == 1
        && ownerGeneration && revision && state->ownerGeneration == ownerGeneration && state->revision == revision
        && (HGNativeFieldChoiceUIIsYesNoInput(state)
            || (state->phase == HG_FIELD_CHOICE_UI_LIST_INPUT && state->rowCount >= 2 && state->rowCount <= 8
                && state->selectedIndex < state->rowCount && state->cancelEnabled <= 1));
}

static inline uint32_t HGNativeFieldChoiceUIResolveInput(const HGNativeFieldChoiceUIState *state,
    uint32_t ownerGeneration, uint32_t revision, uint32_t key, uint32_t *keys) {
    if (!keys || !HGNativeFieldChoiceUIInputAllowed(state, ownerGeneration, revision)) return 0;
    if (HGNativeFieldChoiceUIIsYesNoInput(state) && (key == 16 || key == 32)) return 0;
    switch (key) {
    case 2: if (!state->cancelEnabled) return 0; // Original B confirms NO or the cancellable list's final row.
    case 1: case 16: case 32: case 64: case 128: // Original A and D-pad pulses.
        *keys = key;
        return 1;
    default: return 0;
    }
}

#endif
