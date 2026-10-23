// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef HEARTGOLD_NATIVE_BATTLE_BAG_UI_H
#define HEARTGOLD_NATIVE_BATTLE_BAG_UI_H

#include "native_party_ui.h"

enum { HG_BATTLE_BAG_UI_POCKETS = 1, HG_BATTLE_BAG_UI_ITEMS = 2, HG_BATTLE_BAG_UI_USE = 3, HG_BATTLE_BAG_UI_ACK = 10 };
enum { HG_BATTLE_BAG_UI_ITEMS_PER_PAGE = 6, HG_BATTLE_BAG_UI_BUTTONS = 9 };

typedef struct HGNativeBattleBagUIButton {
    uint32_t hitX, hitY, cropX, cropY, cropWidth, cropHeight;
} HGNativeBattleBagUIButton;

typedef struct HGNativeBattleBagUIState {
    uint32_t active, revision, phase, page, ready, buttonCount, enabledMask;
    uint32_t pocket, pageIndex, pageCount, itemCount, selectedSlot, selectedItemId, selectedQuantity;
    uint32_t lastUsedItemId, lastUsedPocket;
    uint32_t itemId[HG_BATTLE_BAG_UI_ITEMS_PER_PAGE], quantity[HG_BATTLE_BAG_UI_ITEMS_PER_PAGE];
    HGNativeBattleBagUIButton buttons[HG_BATTLE_BAG_UI_BUTTONS];
    HGNativePartyUIWindow selectedName, selectedQuantityText, description, message;
} HGNativeBattleBagUIState;

// Owner thread only; copy descriptor bytes and original framebuffer crops before advancing the game.
const HGNativeBattleBagUIState *HGNativeBattleBagUIGetState(void);

// Original physical row indexes: pockets 0..3/last-used 4/cancel 5; items 0..5/back 6/prev 7/next 8;
// use 0/back 1; message acknowledgement 0. Input always follows the original touch handler.
static inline uint32_t HGNativeBattleBagUIResolveTouch(const HGNativeBattleBagUIState *state, uint32_t revision,
    uint32_t phase, uint32_t index, uint32_t *x, uint32_t *y) {
    if (!state || !x || !y || !state->active || !state->ready || state->revision != revision || state->phase != phase || state->pocket >= 4) return 0;
    uint32_t count;
    if (phase == HG_BATTLE_BAG_UI_POCKETS && state->page == 0) count = 6;
    else if (phase == HG_BATTLE_BAG_UI_ITEMS && state->page == 1) count = 9;
    else if (phase == HG_BATTLE_BAG_UI_USE && state->page == 2) count = 2;
    else if (phase == HG_BATTLE_BAG_UI_ACK && state->page == 2) count = 1;
    else return 0;
    if (state->buttonCount != count || index >= count || (state->enabledMask & ~((1u << count) - 1))
        || !(state->enabledMask & (1u << index))) return 0;
    if (phase == HG_BATTLE_BAG_UI_POCKETS && index == 4 && (!state->lastUsedItemId || state->lastUsedPocket >= 4)) return 0;
    if (phase == HG_BATTLE_BAG_UI_ITEMS || phase == HG_BATTLE_BAG_UI_USE) {
        if (!state->pageCount || state->pageIndex >= state->pageCount || state->itemCount > 36 || state->selectedSlot >= 6) return 0;
        if (phase == HG_BATTLE_BAG_UI_ITEMS && index < 6 && (!state->itemId[index] || !state->quantity[index])) return 0;
        if (phase == HG_BATTLE_BAG_UI_ITEMS && index >= 7 && state->pageCount <= 1) return 0;
        if (phase == HG_BATTLE_BAG_UI_USE && index == 0 && (!state->selectedItemId || !state->selectedQuantity)) return 0;
    }
    const HGNativeBattleBagUIButton *button = &state->buttons[index];
    if (!button->cropWidth || button->cropWidth > 256 || !button->cropHeight || button->cropHeight > 192
        || button->cropX > 256 - button->cropWidth || button->cropY > 192 - button->cropHeight
        || button->hitX < button->cropX || button->hitX - button->cropX >= button->cropWidth
        || button->hitY < button->cropY || button->hitY - button->cropY >= button->cropHeight) return 0;
    uint32_t edge = phase == HG_BATTLE_BAG_UI_ACK ? 0 : 1;
    if (button->hitX != button->cropX + (button->cropWidth - edge) / 2
        || button->hitY != button->cropY + (button->cropHeight - edge) / 2) return 0;
    *x = button->hitX; *y = button->hitY;
    return 1;
}

#ifdef HEARTGOLD_NATIVE
extern HGNativeBattleBagUIState gNativeBattleBagUIState;
#endif

#endif
