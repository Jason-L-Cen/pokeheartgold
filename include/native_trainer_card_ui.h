// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef HEARTGOLD_NATIVE_TRAINER_CARD_UI_H
#define HEARTGOLD_NATIVE_TRAINER_CARD_UI_H
#include <stdint.h>

enum { HG_TRAINER_CARD_VIEWER = 1, HG_TRAINER_CARD_SIGNATURE = 2 };
enum { HG_TRAINER_CARD_TOUCH_DOWN, HG_TRAINER_CARD_TOUCH_MOVE, HG_TRAINER_CARD_TOUCH_UP };

typedef struct HGNativeTrainerCardUIState {
    uint32_t active, ownerGeneration, revision, ready;
    uint32_t viewActive, view, phase, subState, showingBack, isConnected;
    uint32_t promptCursor, promptTouchMode, promptIgnoreTouch;
    uint32_t promptLeft, promptTop, promptWidth, promptButtonHeight;
} HGNativeTrainerCardUIState;

// Owner thread only; copy beside the original surfaces before queuing original input.
const HGNativeTrainerCardUIState *HGNativeTrainerCardUIGetState(void);

#ifdef HEARTGOLD_NATIVE
// Original lifecycle only. These resident helpers are not shipping exports.
void NativeTrainerCardUIBegin(const void *owner);
void NativeTrainerCardUIBeginView(const void *view, uint32_t kind);
void NativeTrainerCardUIPublish(const void *view, const HGNativeTrainerCardUIState *facts);
void NativeTrainerCardUIPromptQueued(void *view);
uint32_t NativeTrainerCardUIPromptReady(const void *view);
void NativeTrainerCardUIEndView(const void *view);
void NativeTrainerCardUIRetire(const void *owner);
#endif

static inline uint32_t HGNativeTrainerCardUIIsPrompt(const HGNativeTrainerCardUIState *state) {
    return state && ((state->view == HG_TRAINER_CARD_VIEWER && state->phase == 4
        && (state->subState == 2 || state->subState == 4))
        || (state->view == HG_TRAINER_CARD_SIGNATURE && state->phase == 1
        && (state->subState == 3 || state->subState == 6)));
}

static inline uint32_t HGNativeTrainerCardUIInputAllowed(const HGNativeTrainerCardUIState *state,
    uint32_t generation, uint32_t revision) {
    return state && state->active == 1 && state->ready == 1 && state->viewActive == 1
        && generation && revision && state->ownerGeneration == generation && state->revision == revision
        && state->showingBack <= 1 && state->isConnected <= 1
        && ((state->view == HG_TRAINER_CARD_VIEWER && state->phase == 1)
            || (state->view == HG_TRAINER_CARD_SIGNATURE && state->phase == 1 && state->subState == 1)
            || (HGNativeTrainerCardUIIsPrompt(state) && state->promptCursor < 2
                && state->promptTouchMode <= 1 && state->promptIgnoreTouch <= 1
                && state->promptLeft == (state->view == HG_TRAINER_CARD_VIEWER ? 208u : 200u) && state->promptTop == 48
                && state->promptWidth == 48 && state->promptButtonHeight == 32));
}

static inline uint32_t HGNativeTrainerCardUIResolveKey(const HGNativeTrainerCardUIState *state,
    uint32_t generation, uint32_t revision, uint32_t key, uint32_t *keys) {
    if (!keys || !HGNativeTrainerCardUIInputAllowed(state, generation, revision)) return 0;
    if (HGNativeTrainerCardUIIsPrompt(state)) {
        if (key != 1 && key != 2 && key != 64 && key != 128) return 0;
    } else if (state->view != HG_TRAINER_CARD_VIEWER
        || (key != 2 && key != 16 && key != 32 && !(key == 1 && state->showingBack && !state->isConnected))) return 0;
    *keys = key;
    return 1;
}

static inline uint32_t HGNativeTrainerCardUITouchStartAllowed(const HGNativeTrainerCardUIState *state,
    uint32_t generation, uint32_t revision, uint32_t x, uint32_t y) {
    if (x >= 256 || y >= 192 || !HGNativeTrainerCardUIInputAllowed(state, generation, revision)) return 0;
    if (HGNativeTrainerCardUIIsPrompt(state)) return !state->promptIgnoreTouch
        && x >= state->promptLeft && x - state->promptLeft < state->promptWidth
        && y >= state->promptTop && y - state->promptTop < 2 * state->promptButtonHeight;
    if (state->view == HG_TRAINER_CARD_SIGNATURE) return (x >= 32 && x < 224 && y >= 72 && y < 136)
        || (x >= 203 && x < 255 && y >= 166);
    return (x < 255 && y < 168) || (x >= 191 && x < 255 && y >= 168 && y < 191)
        || (state->showingBack && !state->isConnected && x < 112 && y >= 168 && y < 191);
}

typedef struct HGNativeTrainerCardUITouchLease {
    uint32_t ownerGeneration, revision, down;
} HGNativeTrainerCardUITouchLease;

// Call on every owner input read, before consuming queued events. A failed lease must send raw up.
static inline void HGNativeTrainerCardUIFenceTouch(HGNativeTrainerCardUITouchLease *lease,
    const HGNativeTrainerCardUIState *state) {
    if (lease && lease->down && !HGNativeTrainerCardUIInputAllowed(state, lease->ownerGeneration, lease->revision))
        *lease = (HGNativeTrainerCardUITouchLease){0};
}

// Coordinates remain original DS coordinates. Move samples may leave the canvas; original clipping owns them.
static inline uint32_t HGNativeTrainerCardUIResolveTouch(HGNativeTrainerCardUITouchLease *lease,
    const HGNativeTrainerCardUIState *state, uint32_t generation, uint32_t revision,
    uint32_t event, uint32_t x, uint32_t y) {
    if (!lease) return 0;
    uint32_t wasDown = lease->down;
    HGNativeTrainerCardUIFenceTouch(lease, state);
    if (wasDown && !lease->down) return 0;
    if (event == HG_TRAINER_CARD_TOUCH_UP) {
        if (!lease->down || lease->ownerGeneration != generation || lease->revision != revision) return 0;
        *lease = (HGNativeTrainerCardUITouchLease){0};
        return 1;
    }
    if (event == HG_TRAINER_CARD_TOUCH_DOWN) {
        if (lease->down || !HGNativeTrainerCardUITouchStartAllowed(state, generation, revision, x, y)) return 0;
        *lease = (HGNativeTrainerCardUITouchLease){ generation, revision, 1 };
        return 1;
    }
    return event == HG_TRAINER_CARD_TOUCH_MOVE && x < 256 && y < 192 && lease->down
        && lease->ownerGeneration == generation && lease->revision == revision;
}

#endif
