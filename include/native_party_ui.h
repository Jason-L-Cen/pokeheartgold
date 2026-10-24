// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef HEARTGOLD_NATIVE_PARTY_UI_H
#define HEARTGOLD_NATIVE_PARTY_UI_H

#include <stdint.h>

enum { HG_PARTY_UI_ROSTER = 1, HG_PARTY_UI_CONTEXT = 2, HG_PARTY_UI_ITEM_TARGET = 4, HG_PARTY_UI_ITEM_USE = 5,
    HG_PARTY_UI_ITEM_RESULT = 11, HG_PARTY_UI_SUBCONTEXT = 15,
    HG_PARTY_UI_GIVE_ITEM = 16, HG_PARTY_UI_TAKE_RESULT = 17 };
enum { HG_PARTY_UI_USE_CONTEXT = 5, HG_PARTY_UI_GIVE_CONTEXT = 10, HG_PARTY_UI_CONFIRM = 7 };
enum { HG_PARTY_UI_SLOTS = 6, HG_PARTY_UI_ACTIONS = 8 };
enum { HG_PARTY_UI_TOUCH_MON, HG_PARTY_UI_TOUCH_ACTION, HG_PARTY_UI_TOUCH_CANCEL };

typedef struct HGNativePartyUIWindow {
    uint32_t pixelBuffer, tileWidth, tileHeight, clearColor, engine;
    uint16_t palette[16];
} HGNativePartyUIWindow;

typedef struct HGNativePartyUIMon {
    uint32_t species, hp, maxHp, level, heldItem, status, gender;
    uint32_t dontPrintGenderSymbol, isEgg, form, active, hitX, hitY;
    int32_t iconX, iconY;
    uint32_t iconWidth, iconHeight;
    HGNativePartyUIWindow nickname;
} HGNativePartyUIMon;

typedef struct HGNativePartyUIAction {
    uint32_t enabled, hitX, hitY, textId; // textId is zero or 1 + PartyMonContextMenuItem.
    HGNativePartyUIWindow text;
} HGNativePartyUIAction;

typedef struct HGNativePartyUIState {
    uint32_t active, revision, context, panelLayout, phase, ready;
    uint32_t selected, count, slotMask, actionCount, actionSelected, actionEnabledMask;
    uint32_t cancelEnabled, cancelX, cancelY;
    HGNativePartyUIMon mons[HG_PARTY_UI_SLOTS];
    HGNativePartyUIAction actions[HG_PARTY_UI_ACTIONS];
    HGNativePartyUIWindow selectedName, selectedItem, message;
} HGNativePartyUIState;

// Owner thread only: copy metadata and tileWidth * tileHeight * 32 tiled 4bpp bytes
// from each nonzero guest pixelBuffer before advancing the game or freeing windows.
const HGNativePartyUIState *HGNativePartyUIGetState(void);

// Only Potion publishes this context's metadata; borrowed glyphs may be cleared between owner steps.
static inline uint32_t HGNativePartyUIIsPotionUse(const HGNativePartyUIState *state) {
    if (!state || !state->active || state->panelLayout || state->context != HG_PARTY_UI_USE_CONTEXT
        || (state->phase != HG_PARTY_UI_ITEM_TARGET && state->phase != HG_PARTY_UI_ITEM_USE)
        || !state->count || state->count > HG_PARTY_UI_SLOTS || !state->slotMask || (state->slotMask >> HG_PARTY_UI_SLOTS)) return 0;
    uint32_t count = 0;
    for (uint32_t i = 0; i < HG_PARTY_UI_SLOTS; ++i) {
        if (!!state->mons[i].active != !!(state->slotMask & (1u << i))) return 0;
        count += !!state->mons[i].active;
    }
    return count == state->count && (state->selected < HG_PARTY_UI_SLOTS ? state->mons[state->selected].active
        : state->selected == HG_PARTY_UI_CONFIRM || (state->phase == HG_PARTY_UI_ITEM_TARGET && state->selected == HG_PARTY_UI_SLOTS));
}

static inline uint32_t HGNativePartyUIIsItemProgress(const HGNativePartyUIState *state) {
    if (HGNativePartyUIIsPotionUse(state)) return state->phase == HG_PARTY_UI_ITEM_USE;
    if (!state || !state->active || state->panelLayout || state->selected >= HG_PARTY_UI_SLOTS
        || !(state->slotMask & (1u << state->selected)) || !state->mons[state->selected].active) return 0;
    return (!state->context && (state->phase == HG_PARTY_UI_ITEM_RESULT || state->phase == HG_PARTY_UI_TAKE_RESULT))
        || (state->context == HG_PARTY_UI_GIVE_CONTEXT && (state->phase == HG_PARTY_UI_GIVE_ITEM || state->phase == HG_PARTY_UI_ITEM_RESULT));
}

static inline uint32_t HGNativePartyUIResolveTouch(const HGNativePartyUIState *state, uint32_t revision, uint32_t phase,
    uint32_t kind, uint32_t index, uint32_t *x, uint32_t *y) {
    if (!state || !x || !y || !state->active || !state->ready || state->panelLayout
        || state->revision != revision || state->phase != phase) return 0;
    if (HGNativePartyUIIsItemProgress(state)) {
        if (phase == HG_PARTY_UI_GIVE_ITEM || kind != HG_PARTY_UI_TOUCH_CANCEL || index || !state->cancelEnabled
            || state->count > HG_PARTY_UI_SLOTS || state->actionCount || state->actionEnabledMask
            || !state->message.pixelBuffer || state->message.tileWidth != 27 || state->message.tileHeight != 4
            || state->message.engine || state->message.clearColor != 15 || state->cancelX != 124 || state->cancelY != 168) return 0;
        *x = state->cancelX; *y = state->cancelY;
        return 1;
    }
    uint32_t target = HGNativePartyUIIsPotionUse(state) && phase == HG_PARTY_UI_ITEM_TARGET;
    if (target) {
        if (state->actionCount || state->actionEnabledMask || !state->message.pixelBuffer
            || state->message.tileWidth != 20 || state->message.tileHeight != 2 || state->message.engine || state->message.clearColor != 15
            || state->cancelX != 228 || state->cancelY != 172) return 0;
    } else if (state->context || (phase != HG_PARTY_UI_ROSTER && phase != HG_PARTY_UI_CONTEXT && phase != HG_PARTY_UI_SUBCONTEXT)) return 0;
    uint32_t touchX, touchY;
    if (kind == HG_PARTY_UI_TOUCH_MON) {
        if ((phase != HG_PARTY_UI_ROSTER && !target) || state->count > HG_PARTY_UI_SLOTS || index >= state->count
            || !(state->slotMask & (1u << index)) || !state->mons[index].active) return 0;
        touchX = state->mons[index].hitX; touchY = state->mons[index].hitY;
    } else if (kind == HG_PARTY_UI_TOUCH_ACTION) {
        if (phase == HG_PARTY_UI_ROSTER || target || state->actionCount > HG_PARTY_UI_ACTIONS || index >= state->actionCount
            || !(state->actionEnabledMask & (1u << index)) || !state->actions[index].enabled) return 0;
        touchX = state->actions[index].hitX; touchY = state->actions[index].hitY;
    } else if (kind == HG_PARTY_UI_TOUCH_CANCEL) {
        if (index || !state->cancelEnabled) return 0;
        touchX = state->cancelX; touchY = state->cancelY;
    } else return 0;
    if (touchX > 255 || touchY > 191) return 0;
    *x = touchX; *y = touchY;
    return 1;
}

#ifdef HEARTGOLD_NATIVE
struct PartyMenu;
union TouchscreenHitbox;
void NativePartyUIBind(struct PartyMenu *partyMenu);
void NativePartyUIBegin(struct PartyMenu *partyMenu);
void NativePartyUIPublish(struct PartyMenu *partyMenu, uint32_t phase, const union TouchscreenHitbox *hitboxes);
#endif

#endif
