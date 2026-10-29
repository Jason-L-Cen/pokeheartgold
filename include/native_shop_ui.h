// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef HEARTGOLD_NATIVE_SHOP_UI_H
#define HEARTGOLD_NATIVE_SHOP_UI_H
#include "native_party_ui.h"

enum { HG_SHOP_UI_LIST=3, HG_SHOP_UI_QUANTITY=7, HG_SHOP_UI_CONFIRM=11,
    HG_SHOP_UI_PURCHASE_PRINT=12, HG_SHOP_UI_MESSAGE=13, HG_SHOP_UI_ERROR=14, HG_SHOP_UI_BONUS=15 };
enum { HG_SHOP_UI_ITEMS=6, HG_SHOP_UI_BUTTONS=6 };
enum { HG_SHOP_UI_TOUCH_ITEM, HG_SHOP_UI_TOUCH_BUTTON };
typedef struct HGNativeShopUIItem {
    uint32_t itemId,price,owned,enabled,hitX,hitY,cropX,cropY,cropWidth,cropHeight;
    HGNativePartyUIWindow text;
} HGNativeShopUIItem;
typedef struct HGNativeShopUIButton { uint32_t enabled,hitX,hitY,cropX,cropY,cropWidth,cropHeight; } HGNativeShopUIButton;
typedef struct HGNativeShopUIState {
    uint32_t active,ownerGeneration,revision,ready,viewActive,kind,phase;
    uint32_t pageStart,itemCount,visibleCount,cursor,selectedItemId,currency,unitPrice,quantity,maxQuantity,ownedQuantity,totalPrice,confirmCursor;
    uint32_t buttonCount;
    HGNativeShopUIItem items[HG_SHOP_UI_ITEMS];
    HGNativeShopUIButton buttons[HG_SHOP_UI_BUTTONS];
    HGNativePartyUIWindow money,page,description,selected,owned,quantityTens,quantityOnes,totalPriceText,prompt;
} HGNativeShopUIState;

// Resident copy; owner thread copies original Window pixels before advancing the game.
const HGNativeShopUIState *HGNativeShopUIGetState(void);
static inline uint32_t HGNativeShopUIInputAllowed(const HGNativeShopUIState *s,uint32_t generation,uint32_t revision) {
    return s && s->active==1 && s->viewActive==1 && s->ready==1 && !s->kind && generation && revision
        && s->ownerGeneration==generation && s->revision==revision
        && (s->phase==HG_SHOP_UI_LIST || s->phase==HG_SHOP_UI_QUANTITY || s->phase==HG_SHOP_UI_CONFIRM
            || s->phase==HG_SHOP_UI_PURCHASE_PRINT || s->phase==HG_SHOP_UI_MESSAGE || s->phase==HG_SHOP_UI_ERROR || s->phase==HG_SHOP_UI_BONUS);
}
static inline uint32_t HGNativeShopUIResolveKey(const HGNativeShopUIState *s,uint32_t generation,uint32_t revision,uint32_t key,uint32_t *output) {
    if(!output || !HGNativeShopUIInputAllowed(s,generation,revision))return 0;
    if(key!=1 && key!=2 && key!=16 && key!=32 && key!=64 && key!=128)return 0;
    if(s->phase==HG_SHOP_UI_CONFIRM && key!=1 && key!=2 && key!=64 && key!=128)return 0;
    if((s->phase==HG_SHOP_UI_PURCHASE_PRINT || s->phase>=HG_SHOP_UI_MESSAGE) && key!=1 && key!=2)return 0;
    *output=key;return 1;
}
static inline uint32_t HGNativeShopUIResolveTouch(const HGNativeShopUIState *s,uint32_t generation,uint32_t revision,uint32_t kind,uint32_t index,uint32_t *x,uint32_t *y) {
    if(!x || !y || !HGNativeShopUIInputAllowed(s,generation,revision))return 0;
    if(s->phase==HG_SHOP_UI_PURCHASE_PRINT)return 0;
    uint32_t px,py,left,top,width,height;
    if(kind==HG_SHOP_UI_TOUCH_ITEM){
        if(s->phase!=HG_SHOP_UI_LIST || s->visibleCount>HG_SHOP_UI_ITEMS || index>=s->visibleCount || !s->items[index].enabled || !s->items[index].itemId)return 0;
        px=s->items[index].hitX;py=s->items[index].hitY;left=s->items[index].cropX;top=s->items[index].cropY;width=s->items[index].cropWidth;height=s->items[index].cropHeight;
    }else if(kind==HG_SHOP_UI_TOUCH_BUTTON){
        if(s->buttonCount>HG_SHOP_UI_BUTTONS || index>=s->buttonCount || !s->buttons[index].enabled)return 0;
        px=s->buttons[index].hitX;py=s->buttons[index].hitY;left=s->buttons[index].cropX;top=s->buttons[index].cropY;width=s->buttons[index].cropWidth;height=s->buttons[index].cropHeight;
    }else return 0;
    if(!width || width>256 || !height || height>192 || left>256-width || top>192-height || px!=left+width/2 || py!=top+height/2)return 0;
    *x=px;*y=py;return 1;
}

#ifdef HEARTGOLD_NATIVE
// Producers only; these functions are not shipping exports or game-state setters.
void NativeShopUIBegin(const void *owner,uint32_t kind,uint32_t sellOnly);
void NativeShopUIPublish(const void *owner,const HGNativeShopUIState *facts);
void NativeShopUIPhase(const void *owner,uint32_t phase);
void NativeShopUIInvalidate(const void *owner);
void NativeShopUIRetire(const void *owner);
#endif
#endif
