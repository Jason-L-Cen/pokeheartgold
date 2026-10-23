// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef HEARTGOLD_NATIVE_PARTY_UI_H
#define HEARTGOLD_NATIVE_PARTY_UI_H

#include <stdint.h>

enum { HG_PARTY_UI_ROSTER = 1, HG_PARTY_UI_CONTEXT = 2, HG_PARTY_UI_SUBCONTEXT = 15 };
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

static inline uint32_t HGNativePartyUIResolveTouch(const HGNativePartyUIState *state, uint32_t revision, uint32_t phase,
    uint32_t kind, uint32_t index, uint32_t *x, uint32_t *y) {
    if (!state || !x || !y || !state->active || !state->ready || state->context || state->panelLayout
        || state->revision != revision || state->phase != phase
        || (phase != HG_PARTY_UI_ROSTER && phase != HG_PARTY_UI_CONTEXT && phase != HG_PARTY_UI_SUBCONTEXT)) return 0;
    uint32_t touchX, touchY;
    if (kind == HG_PARTY_UI_TOUCH_MON) {
        if (phase != HG_PARTY_UI_ROSTER || state->count > HG_PARTY_UI_SLOTS || index >= state->count
            || !(state->slotMask & (1u << index)) || !state->mons[index].active) return 0;
        touchX = state->mons[index].hitX; touchY = state->mons[index].hitY;
    } else if (kind == HG_PARTY_UI_TOUCH_ACTION) {
        if (phase == HG_PARTY_UI_ROSTER || state->actionCount > HG_PARTY_UI_ACTIONS || index >= state->actionCount
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
