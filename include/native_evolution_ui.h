// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef HEARTGOLD_NATIVE_EVOLUTION_UI_H
#define HEARTGOLD_NATIVE_EVOLUTION_UI_H

#include "native_party_ui.h"

enum { HG_EVOLUTION_UI_OSCILLATE = 8, HG_EVOLUTION_UI_DELETE_MOVE_MENU = 21,
    HG_EVOLUTION_UI_OPEN_SUMMARY = 22, HG_EVOLUTION_UI_RUN_SUMMARY = 23,
    HG_EVOLUTION_UI_STOP_LEARNING_MENU = 35, HG_EVOLUTION_UI_CANCEL_FINISH = 45 };
enum { HG_EVOLUTION_UI_CAN_CANCEL = 1, HG_EVOLUTION_UI_PRINT_WHAT = 2, HG_EVOLUTION_UI_TRADE = 4 };
enum { HG_EVOLUTION_UI_INPUT_ACK, HG_EVOLUTION_UI_INPUT_CANCEL, HG_EVOLUTION_UI_INPUT_CHOICE };
enum { HG_EVOLUTION_UI_ACTION_ACK = 1, HG_EVOLUTION_UI_ACTION_CANCEL = 2,
    HG_EVOLUTION_UI_ACTION_TOP = 4, HG_EVOLUTION_UI_ACTION_BOTTOM = 8 };

typedef struct HGNativeEvolutionUIState {
    uint32_t active, revision, phase, ready, summaryActive, flags;
    uint32_t species, newSpecies, moveToLearn, actionMask;
    HGNativePartyUIWindow message, choices[2];
} HGNativeEvolutionUIState;

// Owner thread only: copy each present Window and its tiled 4bpp bytes before advancing.
// Active includes busy/fading and child Summary ownership. Ready means at least one legal
// input, not a cinematic-frame fence: original state8 B cancel remains legal during fades.
const HGNativeEvolutionUIState *HGNativeEvolutionUIGetState(void);

static inline uint32_t HGNativeEvolutionUIIsMessagePhase(uint32_t phase) {
    // Original states which test TextPrinterCheckActive before continuing.
    return phase == 3 || phase == 5 || phase == 12 || phase == 15 || phase == 17 || phase == 19
        || phase == 26 || phase == 28 || phase == 30 || phase == 33 || phase == 36 || phase == 37 || phase == 44;
}

static inline uint32_t HGNativeEvolutionUIResolveInput(const HGNativeEvolutionUIState *state, uint32_t revision, uint32_t phase,
    uint32_t kind, uint32_t index, uint32_t *keys, uint32_t *x, uint32_t *y) {
    if (!state || !keys || !x || !y || !state->active || !revision || revision != state->revision || phase != state->phase
        || phase > HG_EVOLUTION_UI_CANCEL_FINISH || state->summaryActive || !state->ready || !state->species || !state->newSpecies
        || (state->flags & ~7u) || !state->actionMask || (state->actionMask & ~15u)) return 0;
    uint32_t inputKeys = 0, touchX = 0, touchY = 0;
    if (kind == HG_EVOLUTION_UI_INPUT_CANCEL) {
        if (index || phase != HG_EVOLUTION_UI_OSCILLATE || !(state->flags & HG_EVOLUTION_UI_CAN_CANCEL)
            || state->actionMask != HG_EVOLUTION_UI_ACTION_CANCEL) return 0;
        inputKeys = 2; // Original PAD_BUTTON_B; no palette/fade condition on the original branch.
    } else if (kind == HG_EVOLUTION_UI_INPUT_ACK) {
        const HGNativePartyUIWindow *message = &state->message;
        if (index || !HGNativeEvolutionUIIsMessagePhase(phase) || state->actionMask != HG_EVOLUTION_UI_ACTION_ACK
            || !message->pixelBuffer || message->pixelBuffer > UINT32_MAX - 27 * 4 * 32
            || message->tileWidth != 27 || message->tileHeight != 4 || message->engine || message->clearColor != 15) return 0;
        inputKeys = 1; // Original PAD_BUTTON_A, accepted only at a real live printer wait.
    } else if (kind == HG_EVOLUTION_UI_INPUT_CHOICE) {
        if (index >= 2 || (phase != HG_EVOLUTION_UI_DELETE_MOVE_MENU && phase != HG_EVOLUTION_UI_STOP_LEARNING_MENU)
            || state->actionMask != (HG_EVOLUTION_UI_ACTION_TOP | HG_EVOLUTION_UI_ACTION_BOTTOM)) return 0;
        for (uint32_t i = 0; i < 2; ++i) {
            const HGNativePartyUIWindow *choice = &state->choices[i];
            if (!choice->pixelBuffer || choice->pixelBuffer > UINT32_MAX - 26 * 2 * 32
                || choice->tileWidth != 26 || choice->tileHeight != 2 || choice->engine != 1 || choice->clearColor) return 0;
        }
        touchX = 127; touchY = index ? 119 : 71; // Original sEvolutionMenuHitboxes centers.
    } else return 0;
    *keys = inputKeys; *x = touchX; *y = touchY;
    return 1;
}

#endif
