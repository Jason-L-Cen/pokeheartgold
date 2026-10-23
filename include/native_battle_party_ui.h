// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef HEARTGOLD_NATIVE_BATTLE_PARTY_UI_H
#define HEARTGOLD_NATIVE_BATTLE_PARTY_UI_H

#include "native_party_ui.h"

enum { HG_BATTLE_PARTY_UI_ROSTER = 1, HG_BATTLE_PARTY_UI_MON_MENU = 2, HG_BATTLE_PARTY_UI_SUMMARY = 3,
    HG_BATTLE_PARTY_UI_CHECK_MOVES = 4, HG_BATTLE_PARTY_UI_MOVE_DETAIL = 5, HG_BATTLE_PARTY_UI_MESSAGE = 17, HG_BATTLE_PARTY_UI_ACK = 18 };
enum { HG_BATTLE_PARTY_UI_SWITCH = 0, HG_BATTLE_PARTY_UI_USE_ITEM = 2, HG_BATTLE_PARTY_UI_POTION = 17 };
enum { HG_BATTLE_PARTY_UI_MONS = 6, HG_BATTLE_PARTY_UI_BUTTONS = 8 };

typedef struct HGNativeBattlePartyUIMon {
    uint32_t species, form, level, hp, maxHp, status, gender, hideGender, isEgg, heldItem, partyIndex, slotType;
} HGNativeBattlePartyUIMon;

typedef struct HGNativeBattlePartyUIButton {
    uint32_t hitX, hitY, cropX, cropY, cropWidth, cropHeight;
} HGNativeBattlePartyUIButton;

typedef struct HGNativeBattlePartyUIMove {
    uint32_t moveId, pp, maxPP;
} HGNativeBattlePartyUIMove;

typedef struct HGNativeBattlePartyUIState {
    uint32_t active, revision, phase, page, ready, mode, partyCount, selectedSlot, itemId, pocket, buttonCount, enabledMask;
    HGNativeBattlePartyUIMon mons[HG_BATTLE_PARTY_UI_MONS];
    HGNativeBattlePartyUIButton buttons[HG_BATTLE_PARTY_UI_BUTTONS];
    uint32_t selectedMove;
    HGNativeBattlePartyUIMove moves[4];
    HGNativePartyUIWindow prompt, message;
} HGNativeBattlePartyUIState;

// Owner thread only; copy descriptor bytes and original framebuffer crops before advancing the game.
const HGNativeBattlePartyUIState *HGNativeBattlePartyUIGetState(void);

// Original indexes: roster mons 0..5/back 6; mon menu SHIFT/SUMMARY/CHECK MOVES/back; summary prev/next/moves/back;
// check moves 0..3/prev/next/summary/back; detail moves 0..3/back; ACK 0.
static inline uint32_t HGNativeBattlePartyUIResolveTouch(const HGNativeBattlePartyUIState *state, uint32_t revision,
    uint32_t phase, uint32_t index, uint32_t *x, uint32_t *y) {
    if (!state || !x || !y || !state->active || !state->ready || state->revision != revision || state->phase != phase
        || state->partyCount > HG_BATTLE_PARTY_UI_MONS
        || (state->mode != HG_BATTLE_PARTY_UI_SWITCH && (state->mode != HG_BATTLE_PARTY_UI_USE_ITEM || state->itemId != HG_BATTLE_PARTY_UI_POTION))) return 0;
    uint32_t count;
    if (phase == HG_BATTLE_PARTY_UI_ROSTER && state->page == 0) count = 7;
    else if (phase == HG_BATTLE_PARTY_UI_MON_MENU && state->page == 1 && state->mode == HG_BATTLE_PARTY_UI_SWITCH
        && state->selectedSlot < state->partyCount && state->mons[state->selectedSlot].slotType) count = 4;
    else if (phase == HG_BATTLE_PARTY_UI_ACK && state->page <= 1) count = 1;
    else if (state->mode == HG_BATTLE_PARTY_UI_SWITCH && state->selectedSlot < state->partyCount
        && state->mons[state->selectedSlot].slotType && !state->mons[state->selectedSlot].isEgg) {
        if (phase == HG_BATTLE_PARTY_UI_SUMMARY && state->page == 2) count = 4;
        else if (phase == HG_BATTLE_PARTY_UI_CHECK_MOVES && state->page == 3) count = 8;
        else if (phase == HG_BATTLE_PARTY_UI_MOVE_DETAIL && state->page == 4 && state->selectedMove < 4
            && state->moves[state->selectedMove].moveId) count = 5;
        else return 0;
    }
    else return 0;
    if (state->buttonCount != count || index >= count || (state->enabledMask & ~((1u << count) - 1))
        || !(state->enabledMask & (1u << index))) return 0;
    if (phase == HG_BATTLE_PARTY_UI_ROSTER && index < 6 && !state->mons[index].slotType) return 0;
    if ((phase == HG_BATTLE_PARTY_UI_CHECK_MOVES || phase == HG_BATTLE_PARTY_UI_MOVE_DETAIL) && index < 4 && !state->moves[index].moveId) return 0;
    const HGNativeBattlePartyUIButton *button = &state->buttons[index];
    if (!button->cropWidth || button->cropWidth > 256 || !button->cropHeight || button->cropHeight > 192
        || button->cropX > 256 - button->cropWidth || button->cropY > 192 - button->cropHeight
        || button->hitX < button->cropX || button->hitX - button->cropX >= button->cropWidth
        || button->hitY < button->cropY || button->hitY - button->cropY >= button->cropHeight) return 0;
    // Original exclusive hit bounds put the centre one pixel before these even-sized full-card crops.
    uint32_t edge = phase == HG_BATTLE_PARTY_UI_ACK ? 0 : 1;
    if (button->hitX != button->cropX + (button->cropWidth - edge) / 2
        || button->hitY != button->cropY + (button->cropHeight - edge) / 2) return 0;
    *x = button->hitX; *y = button->hitY;
    return 1;
}

#ifdef HEARTGOLD_NATIVE
extern HGNativeBattlePartyUIState gNativeBattlePartyUIState;
#endif

#endif
