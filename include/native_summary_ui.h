// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef HEARTGOLD_NATIVE_SUMMARY_UI_H
#define HEARTGOLD_NATIVE_SUMMARY_UI_H

#include "native_party_ui.h"
#include "constants/moves.h"

enum { HG_SUMMARY_UI_INPUT = 2, HG_SUMMARY_UI_MOVE_SELECT = 5, HG_SUMMARY_UI_FORGET_MOVE = 8,
    HG_SUMMARY_UI_FORGET_CONFIRM = 9, HG_SUMMARY_UI_MOVES = 4 };
enum {
    HG_SUMMARY_UI_INPUT_MOVE, HG_SUMMARY_UI_INPUT_BACK,
    HG_SUMMARY_UI_INPUT_PREVIOUS_PAGE, HG_SUMMARY_UI_INPUT_NEXT_PAGE,
    HG_SUMMARY_UI_INPUT_PREVIOUS_MON, HG_SUMMARY_UI_INPUT_NEXT_MON,
    HG_SUMMARY_UI_INPUT_RIBBON_DETAILS, HG_SUMMARY_UI_INPUT_CONFIRM
};

typedef struct HGNativeSummaryUIMove {
    uint32_t moveId, pp, maxPP, enabled, hitX, hitY, cropX, cropY, cropWidth, cropHeight;
    HGNativePartyUIWindow text;
} HGNativeSummaryUIMove;

typedef struct HGNativeSummaryUIInput {
    uint32_t keys, touchPressed, touchX, touchY;
} HGNativeSummaryUIInput;

typedef struct HGNativeSummaryUIState {
    uint32_t active, revision, mode, phase, ready;
    uint32_t page, pageMask, monDataType, partySlot, partyCount, moveToLearn;
    uint32_t species, form, level, gender, noPrintGender, isEgg, shiny, heldItem, type1, type2;
    uint32_t statusIcon, pokerus; // Original cached sprite status ID, including the healthy Pokerus override.
    uint32_t hp, maxHp, attack, defense, spAttack, spDefense, speed, ability, nature, ribbonCount;
    uint32_t moveSlot, moveMask;
    uint32_t previousPageEnabled, previousPage, nextPageEnabled, nextPage;
    uint32_t previousMonEnabled, previousMonSlot, nextMonEnabled, nextMonSlot, backEnabled;
    HGNativeSummaryUIMove moves[HG_SUMMARY_UI_MOVES];
    HGNativePartyUIWindow nickname, levelText, heldItemText, backText, pageTitle;
} HGNativeSummaryUIState;

// Owner thread only; copy each nonzero descriptor's tiled 4bpp bytes before advancing the game.
const HGNativeSummaryUIState *HGNativeSummaryUIGetState(void);

static inline uint32_t HGNativeSummaryUIIsOrdinary(const HGNativeSummaryUIState *state) {
    return state && !state->mode && !state->moveToLearn && state->partyCount && state->partySlot < state->partyCount
        && ((state->monDataType == 1 && state->partyCount <= 6) || (state->monDataType == 2 && state->partyCount <= 30));
}

static inline uint32_t HGNativeSummaryUIIsLearning(const HGNativeSummaryUIState *state) {
    return state && state->mode == 2 && state->page == 1 && (state->pageMask & 2) && !state->isEgg
        && state->moveToLearn && state->moveToLearn <= NUM_MOVES
        && ((state->monDataType == 1 && state->partyCount && state->partyCount <= 6 && state->partySlot < state->partyCount)
            || (state->monDataType == 0 && state->partyCount == 1 && !state->partySlot));
}

static inline uint32_t HGNativeSummaryUIResolveInput(const HGNativeSummaryUIState *state, uint32_t revision, uint32_t phase,
    uint32_t kind, uint32_t index, HGNativeSummaryUIInput *out) {
    uint32_t learning = HGNativeSummaryUIIsLearning(state);
    if (!state || !out || !state->active || !state->ready || state->revision != revision || state->phase != phase
        || (!learning && !HGNativeSummaryUIIsOrdinary(state)) || state->page > 2
        || !(state->pageMask & (1u << state->page)) || (state->isEgg && state->page)
        || (learning ? ((phase != HG_SUMMARY_UI_FORGET_MOVE && phase != HG_SUMMARY_UI_FORGET_CONFIRM)
                || state->moveSlot > (phase == HG_SUMMARY_UI_FORGET_MOVE ? 4u : 3u))
            : ((phase != HG_SUMMARY_UI_INPUT && phase != HG_SUMMARY_UI_MOVE_SELECT)
                || (phase == HG_SUMMARY_UI_MOVE_SELECT && (state->page != 1 || state->isEgg))))) return 0;
    if (learning && kind != HG_SUMMARY_UI_INPUT_MOVE && kind != HG_SUMMARY_UI_INPUT_BACK && kind != HG_SUMMARY_UI_INPUT_CONFIRM) return 0;
    if (learning && state->moveSlot < HG_SUMMARY_UI_MOVES
        && (!state->moves[state->moveSlot].moveId || state->moves[state->moveSlot].moveId > NUM_MOVES)) return 0;
    HGNativeSummaryUIInput input = { 0, 0, 0, 0 };
    if (kind == HG_SUMMARY_UI_INPUT_MOVE) {
        if (learning && phase != HG_SUMMARY_UI_FORGET_MOVE) return 0;
        if (learning && index == 4) {
            if (!(state->moveMask & 16)) return 0;
            input.touchPressed = 1; input.touchX = 67; input.touchY = 167; // Original Confirm hitbox; selects the fifth move only.
        } else {
            if (state->page != 1 || state->isEgg || index >= HG_SUMMARY_UI_MOVES || !(state->moveMask & (1u << index))
                || !state->moves[index].enabled || !state->moves[index].moveId) return 0;
            if (learning && state->moves[index].moveId > NUM_MOVES) return 0;
            const HGNativeSummaryUIMove *move = &state->moves[index];
            if (!move->cropWidth || move->cropWidth > 256 || !move->cropHeight || move->cropHeight > 192
                || move->cropX > 256 - move->cropWidth || move->cropY > 192 - move->cropHeight
                || move->hitX > 255 || move->hitY > 191
                || move->hitX != move->cropX + move->cropWidth / 2 || move->hitY != move->cropY + move->cropHeight / 2) return 0;
            input.touchPressed = 1; input.touchX = move->hitX; input.touchY = move->hitY;
        }
    } else if (kind == HG_SUMMARY_UI_INPUT_CONFIRM) {
        if (!learning || index || (phase == HG_SUMMARY_UI_FORGET_MOVE ? state->moveSlot != 4 || !(state->moveMask & 16)
                : !state->moves[state->moveSlot].moveId)) return 0;
        input.keys = 1; // Original A: confirms an old move in state 9, or returns cancellation slot 4 in state 8.
    } else if (kind == HG_SUMMARY_UI_INPUT_BACK) {
        if (index || !state->backEnabled) return 0;
        input.keys = 2; // PAD_BUTTON_B closes move details or exits the normal page.
    } else if (kind == HG_SUMMARY_UI_INPUT_PREVIOUS_PAGE || kind == HG_SUMMARY_UI_INPUT_NEXT_PAGE) {
        if (index || phase != HG_SUMMARY_UI_INPUT || state->isEgg) return 0;
        uint32_t previous = kind == HG_SUMMARY_UI_INPUT_PREVIOUS_PAGE;
        uint32_t page = previous ? state->previousPage : state->nextPage;
        if (!(previous ? state->previousPageEnabled : state->nextPageEnabled) || page > 2 || page == state->page
            || !(state->pageMask & (1u << page))) return 0;
        input.keys = previous ? 32 : 16; // PAD_KEY_LEFT / PAD_KEY_RIGHT.
    } else if (kind == HG_SUMMARY_UI_INPUT_PREVIOUS_MON || kind == HG_SUMMARY_UI_INPUT_NEXT_MON) {
        if (index || phase != HG_SUMMARY_UI_INPUT) return 0;
        uint32_t previous = kind == HG_SUMMARY_UI_INPUT_PREVIOUS_MON;
        uint32_t slot = previous ? state->previousMonSlot : state->nextMonSlot;
        if (!(previous ? state->previousMonEnabled : state->nextMonEnabled) || slot >= state->partyCount || slot == state->partySlot
            || (previous ? slot >= state->partySlot : slot <= state->partySlot)) return 0;
        input.keys = previous ? 64 : 128; // PAD_KEY_UP / PAD_KEY_DOWN.
    } else if (kind == HG_SUMMARY_UI_INPUT_RIBBON_DETAILS) {
        if (index || phase != HG_SUMMARY_UI_INPUT || state->page != 2 || state->isEgg || !state->ribbonCount || state->ribbonCount > 80) return 0;
        input.keys = 1; // The original page-2 A branch only; phase 5 can never request Swap.
    } else return 0;
    *out = input;
    return 1;
}

#endif
