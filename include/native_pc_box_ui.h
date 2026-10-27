// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef HEARTGOLD_NATIVE_PC_BOX_UI_H
#define HEARTGOLD_NATIVE_PC_BOX_UI_H
#include <stdint.h>

enum {
    HG_PC_BOX_UI_WAIT_BUTTON = 6, HG_PC_BOX_UI_HANDLE_YES_NO = 7,
    HG_PC_BOX_UI_MOVE_MODE_INPUT = 12, HG_PC_BOX_UI_MARKING_MENU_INPUT = 22,
    HG_PC_BOX_UI_MOVE_MODE_PARTY_INPUT = 36, HG_PC_BOX_UI_BOX_TRAY_INPUT = 41,
    HG_PC_BOX_UI_BOX_MENU_INPUT = 61, HG_PC_BOX_UI_WALLPAPER_MENU_INPUT = 66,
    HG_PC_BOX_UI_WITHDRAW_INPUT = 81, HG_PC_BOX_UI_DEPOSIT_INPUT = 91,
    HG_PC_BOX_UI_DEPOSIT_BOX_INPUT = 97, HG_PC_BOX_UI_BOX_TRAY_HOLDING_INPUT = 115,
    HG_PC_BOX_UI_ITEM_MODE_INPUT = 117, HG_PC_BOX_UI_ITEM_MODE_BOX_TRAY_INPUT = 130,
    HG_PC_BOX_UI_ITEM_TRAY_HOLDING_INPUT = 136, HG_PC_BOX_UI_ITEM_MODE_PARTY_INPUT = 139
};

typedef struct HGNativePCBoxUIState {
    uint32_t active, ownerGeneration, revision, ready, phase, mode;
    uint32_t activeBox, selectedSlot, partyPanelOpen, trayOpen, actionMenuOpen, cursorHolding, viewActive;
} HGNativePCBoxUIState;

// Owner thread only. Active owns the app through child overlays; viewActive owns live PC graphics.
const HGNativePCBoxUIState *HGNativePCBoxUIGetState(void);

#ifdef HEARTGOLD_NATIVE
// Original lifecycle only; these resident helpers are not shipping exports.
typedef struct NativePCBoxUISelection {
    uint32_t gridNextInput, gridIsButtons, yesNoCursorPos, yesNoInTouchMode;
    uint32_t markings, trayBox, wallpaperSelection;
} NativePCBoxUISelection;

void NativePCBoxUIBegin(const void *owner);
void NativePCBoxUIBeginView(const void *owner);
void NativePCBoxUIEndView(const void *owner);
void NativePCBoxUIPublish(const void *owner, const HGNativePCBoxUIState *facts, const NativePCBoxUISelection *selection);
void NativePCBoxUIRetire(const void *owner);
#endif

static inline uint32_t HGNativePCBoxUIIsInputPhase(uint32_t phase) {
    switch (phase) {
    case HG_PC_BOX_UI_WAIT_BUTTON: case HG_PC_BOX_UI_HANDLE_YES_NO:
    case HG_PC_BOX_UI_MOVE_MODE_INPUT: case HG_PC_BOX_UI_MARKING_MENU_INPUT:
    case HG_PC_BOX_UI_MOVE_MODE_PARTY_INPUT: case HG_PC_BOX_UI_BOX_TRAY_INPUT:
    case HG_PC_BOX_UI_BOX_MENU_INPUT: case HG_PC_BOX_UI_WALLPAPER_MENU_INPUT:
    case HG_PC_BOX_UI_WITHDRAW_INPUT: case HG_PC_BOX_UI_DEPOSIT_INPUT:
    case HG_PC_BOX_UI_DEPOSIT_BOX_INPUT: case HG_PC_BOX_UI_BOX_TRAY_HOLDING_INPUT:
    case HG_PC_BOX_UI_ITEM_MODE_INPUT: case HG_PC_BOX_UI_ITEM_MODE_BOX_TRAY_INPUT:
    case HG_PC_BOX_UI_ITEM_TRAY_HOLDING_INPUT: case HG_PC_BOX_UI_ITEM_MODE_PARTY_INPUT:
        return 1;
    default: return 0;
    }
}

static inline uint32_t HGNativePCBoxUIResolveInput(const HGNativePCBoxUIState *state,
    uint32_t ownerGeneration, uint32_t revision, uint32_t key, uint32_t *keys) {
    if (!state || !keys || state->active != 1 || state->ready != 1 || state->viewActive != 1
        || !ownerGeneration || !revision || state->ownerGeneration != ownerGeneration || state->revision != revision
        || !HGNativePCBoxUIIsInputPhase(state->phase) || state->mode > 3 || state->activeBox >= 18
        || (state->selectedSlot >= 36 && state->selectedSlot != 255) || state->partyPanelOpen > 1
        || state->trayOpen > 1 || state->actionMenuOpen > 1 || state->cursorHolding > 1) return 0;
    switch (key) {
    case 1: case 2: case 16: case 32: case 64: case 128: case 2048: // Original A/B/D-pad/Y pulses.
        *keys = key;
        return 1;
    default: return 0;
    }
}

#endif
