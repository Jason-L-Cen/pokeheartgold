// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef HEARTGOLD_NATIVE_BAG_UI_H
#define HEARTGOLD_NATIVE_BAG_UI_H

#include "native_party_ui.h"
#include "constants/items.h"

enum { HG_BAG_UI_INPUT = 1, HG_BAG_UI_ITEM_MENU = 4, HG_BAG_UI_TM_MESSAGE = 13, HG_BAG_UI_GIVE_INPUT = 14, HG_BAG_UI_TM_EXIT_FADE = 37 };
enum { HG_BAG_UI_GIVE_MODE = 1 };
enum { HG_BAG_UI_SELL_MODE=2, HG_BAG_UI_SELL_INPUT=16, HG_BAG_UI_SELL_QUANTITY=18, HG_BAG_UI_SELL_CONFIRM=22, HG_BAG_UI_SELL_MESSAGE=24 };
enum { HG_BAG_UI_ACTION_TM_ACK = 16, HG_BAG_UI_ACTION_TM_YES, HG_BAG_UI_ACTION_TM_NO };
enum { HG_BAG_UI_POCKETS = 8, HG_BAG_UI_ITEMS = 6, HG_BAG_UI_ACTIONS = 4 };
enum { HG_BAG_UI_TOUCH_POCKET, HG_BAG_UI_TOUCH_ITEM, HG_BAG_UI_TOUCH_ACTION, HG_BAG_UI_TOUCH_PREVIOUS, HG_BAG_UI_TOUCH_NEXT, HG_BAG_UI_TOUCH_CANCEL };

typedef struct HGNativeBagUIPocket {
    uint32_t pocketId, enabled, hitX, hitY, cropX, cropY, cropWidth, cropHeight;
} HGNativeBagUIPocket;

typedef struct HGNativeBagUIItem {
    uint32_t itemId, quantity, absoluteIndex, enabled, hitX, hitY, cropX, cropY, cropWidth, cropHeight;
    HGNativePartyUIWindow text;
} HGNativeBagUIItem;

typedef struct HGNativeBagUIAction {
    uint32_t actionId, enabled, hitX, hitY; // Original action 0..15, native TM roles 16..18; 255 is an empty physical row.
    HGNativePartyUIWindow text;
} HGNativeBagUIAction;

typedef struct HGNativeBagUIState {
    uint32_t active, revision, mode, phase, ready;
    uint32_t pocketIndex, pocketId, pocketMask, pocketCount, cursorButton, position, scroll, itemCount, pageIndex, pageCount, visibleCount, itemMask;
    uint32_t selectedItemId, selectedQuantity, menuCursor, actionCount, actionMask;
    uint32_t previousEnabled, previousX, previousY, nextEnabled, nextX, nextY, cancelEnabled, cancelX, cancelY;
    HGNativeBagUIPocket pockets[HG_BAG_UI_POCKETS];
    HGNativeBagUIItem items[HG_BAG_UI_ITEMS];
    HGNativeBagUIAction actions[HG_BAG_UI_ACTIONS];
    HGNativePartyUIWindow description, tmInfo, prompt, message, page, selectedEntry, cancel;
    uint32_t transactionQuantity,transactionMax,unitPrice,currency;
    HGNativePartyUIWindow currencyText,quantityText,quantityOnesText,totalPriceText;
} HGNativeBagUIState;

// Owner thread only; copy each nonzero descriptor's tiled 4bpp bytes before advancing the game.
const HGNativeBagUIState *HGNativeBagUIGetState(void);

static inline uint32_t HGNativeBagUIIsList(const HGNativeBagUIState *state) {
    return state && ((!state->mode && state->phase == HG_BAG_UI_INPUT)
        || (state->mode == HG_BAG_UI_GIVE_MODE && state->phase == HG_BAG_UI_GIVE_INPUT)
        || (state->mode == HG_BAG_UI_SELL_MODE && state->phase == HG_BAG_UI_SELL_INPUT));
}
static inline uint32_t HGNativeBagUIIsSell(const HGNativeBagUIState *state) {
    return state && state->active && state->mode==HG_BAG_UI_SELL_MODE;
}
static inline uint32_t HGNativeBagUIIsSellTransaction(const HGNativeBagUIState *state) {
    return HGNativeBagUIIsSell(state) && (state->phase==HG_BAG_UI_SELL_QUANTITY || state->phase==HG_BAG_UI_SELL_CONFIRM || state->phase==HG_BAG_UI_SELL_MESSAGE);
}
static inline uint32_t HGNativeBagUIResolveSellKey(const HGNativeBagUIState *state,uint32_t revision,uint32_t phase,uint32_t key,uint32_t *output) {
    if(!output || !HGNativeBagUIIsSellTransaction(state) || !state->ready || !revision || state->revision!=revision || state->phase!=phase)return 0;
    if(key!=1 && key!=2 && key!=16 && key!=32 && key!=64 && key!=128)return 0;
    if(phase==HG_BAG_UI_SELL_CONFIRM && key!=1 && key!=2 && key!=64 && key!=128)return 0;
    if(phase==HG_BAG_UI_SELL_MESSAGE && key!=1 && key!=2)return 0;
    *output=key;return 1;
}

// The producer publishes these roles only for the original TM/HM task, including its disabled exit fade.
static inline uint32_t HGNativeBagUIIsTMHM(const HGNativeBagUIState *state) {
    return state && state->active && !state->mode && (state->phase == HG_BAG_UI_TM_MESSAGE || state->phase == HG_BAG_UI_TM_EXIT_FADE)
        && state->pocketId == POCKET_TMHMS && state->selectedItemId >= ITEM_TM01 && state->selectedItemId <= ITEM_HM08
        && ((state->actionCount == 1 && state->actions[0].actionId == HG_BAG_UI_ACTION_TM_ACK)
            || (state->phase == HG_BAG_UI_TM_MESSAGE && state->actionCount == 2 && state->actions[0].actionId == HG_BAG_UI_ACTION_TM_YES
                && state->actions[1].actionId == HG_BAG_UI_ACTION_TM_NO));
}

static inline uint32_t HGNativeBagUIResolveTouch(const HGNativeBagUIState *state, uint32_t revision, uint32_t phase,
    uint32_t kind, uint32_t index, uint32_t *x, uint32_t *y) {
    if (!state || !x || !y || !state->active || !state->ready || state->revision != revision || state->phase != phase
        || (!HGNativeBagUIIsList(state) && !HGNativeBagUIIsTMHM(state) && (state->mode || phase != HG_BAG_UI_ITEM_MENU))) return 0;
    if (HGNativeBagUIIsTMHM(state)) {
        if (phase != HG_BAG_UI_TM_MESSAGE || !state->selectedQuantity || kind != HG_BAG_UI_TOUCH_ACTION || index >= state->actionCount || !state->actions[index].enabled
            || state->actionMask != (1u << state->actionCount) - 1 || state->cancelEnabled
            || !state->message.pixelBuffer || state->message.tileWidth != 27 || state->message.tileHeight != 4
            || state->message.engine != 1 || state->message.clearColor != 15) return 0;
        uint32_t hitX = state->actionCount == 1 ? 124 : 224, hitY = state->actionCount == 1 ? 24 : 64 + 32 * index;
        if (state->actions[index].hitX != hitX || state->actions[index].hitY != hitY) return 0;
        *x = hitX; *y = hitY; return 1;
    }
    uint32_t touchX, touchY, cropX = 0, cropY = 0, cropWidth = 0, cropHeight = 0;
    if (kind == HG_BAG_UI_TOUCH_POCKET) {
        if (!HGNativeBagUIIsList(state) || index >= HG_BAG_UI_POCKETS || !(state->pocketMask & (1u << index)) || !state->pockets[index].enabled) return 0;
        const HGNativeBagUIPocket *pocket = &state->pockets[index];
        touchX = pocket->hitX; touchY = pocket->hitY;
        cropX = pocket->cropX; cropY = pocket->cropY; cropWidth = pocket->cropWidth; cropHeight = pocket->cropHeight;
    } else if (kind == HG_BAG_UI_TOUCH_ITEM) {
        if (!HGNativeBagUIIsList(state) || state->visibleCount > HG_BAG_UI_ITEMS || index >= state->visibleCount
            || !(state->itemMask & (1u << index)) || !state->items[index].enabled || !state->items[index].itemId || !state->items[index].quantity) return 0;
        const HGNativeBagUIItem *item = &state->items[index];
        touchX = item->hitX; touchY = item->hitY;
        cropX = item->cropX; cropY = item->cropY; cropWidth = item->cropWidth; cropHeight = item->cropHeight;
    } else if (kind == HG_BAG_UI_TOUCH_ACTION) {
        if (phase != HG_BAG_UI_ITEM_MENU || state->actionCount > HG_BAG_UI_ACTIONS || index >= state->actionCount
            || !(state->actionMask & (1u << index)) || !state->actions[index].enabled || state->actions[index].actionId >= 16) return 0;
        touchX = state->actions[index].hitX; touchY = state->actions[index].hitY;
    } else if (kind == HG_BAG_UI_TOUCH_PREVIOUS || kind == HG_BAG_UI_TOUCH_NEXT) {
        if (!HGNativeBagUIIsList(state) || index || !(kind == HG_BAG_UI_TOUCH_PREVIOUS ? state->previousEnabled : state->nextEnabled)) return 0;
        touchX = kind == HG_BAG_UI_TOUCH_PREVIOUS ? state->previousX : state->nextX;
        touchY = kind == HG_BAG_UI_TOUCH_PREVIOUS ? state->previousY : state->nextY;
    } else if (kind == HG_BAG_UI_TOUCH_CANCEL) {
        if (index || !state->cancelEnabled) return 0;
        touchX = state->cancelX; touchY = state->cancelY;
    } else return 0;
    if (touchX > 255 || touchY > 191) return 0;
    if (kind == HG_BAG_UI_TOUCH_POCKET || kind == HG_BAG_UI_TOUCH_ITEM) {
        if (!cropWidth || cropWidth > 256 || !cropHeight || cropHeight > 192 || cropX > 256 - cropWidth || cropY > 192 - cropHeight
            || touchX != cropX + cropWidth / 2 || touchY != cropY + cropHeight / 2) return 0;
    }
    *x = touchX; *y = touchY;
    return 1;
}

#ifdef HEARTGOLD_NATIVE
struct BagApp;
extern HGNativeBagUIState gNativeBagUIState;
void NativeBagUISetMessage(struct BagApp *app, uint32_t wide);
#endif

#endif
