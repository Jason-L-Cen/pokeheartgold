// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef HEARTGOLD_NATIVE_NAMING_UI_H
#define HEARTGOLD_NATIVE_NAMING_UI_H
#include <stdint.h>

typedef struct HGNativeNamingUIState {
    uint32_t active, ownerGeneration, revision, ready;
} HGNativeNamingUIState;

// Owner thread only; UIKit validates copied state before queuing ordinary pad/touch input.
const HGNativeNamingUIState *HGNativeNamingUIGetState(void);

static inline uint32_t HGNativeNamingUIInputAllowed(const HGNativeNamingUIState *state,
    uint32_t ownerGeneration, uint32_t revision) {
    return state && state->active == 1 && state->ready == 1 && ownerGeneration && revision
        && state->ownerGeneration == ownerGeneration && state->revision == revision;
}

#endif
