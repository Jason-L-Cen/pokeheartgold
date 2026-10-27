// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef HEARTGOLD_NATIVE_OPTIONS_UI_H
#define HEARTGOLD_NATIVE_OPTIONS_UI_H
#include <stdint.h>

typedef struct HGNativeOptionsUIState {
    uint32_t active, ownerGeneration, revision, ready;
} HGNativeOptionsUIState;

// Owner thread only; UIKit qualifies copied state before queuing original pad/touch input.
const HGNativeOptionsUIState *HGNativeOptionsUIGetState(void);

#ifdef HEARTGOLD_NATIVE
// Original overlay lifecycle only; these internal C helpers are not shipping exports.
void NativeOptionsUIBegin(const void *owner);
void NativeOptionsUIPublish(const void *owner, uint32_t ready);
void NativeOptionsUIRetire(const void *owner);
#endif

static inline uint32_t HGNativeOptionsUIInputAllowed(const HGNativeOptionsUIState *state,
    uint32_t ownerGeneration, uint32_t revision) {
    return state && state->active == 1 && state->ready == 1 && ownerGeneration && revision
        && state->ownerGeneration == ownerGeneration && state->revision == revision;
}

#endif
