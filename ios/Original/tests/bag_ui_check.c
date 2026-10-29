// The production publisher/dialogue path is checked by the actual ABI32 TM scenario; this entry checks only public packet input guards.
#if defined(HG_NATIVE_BAG_UI_PACKET_CHECK_MAIN) || defined(HEARTGOLD_ABI32)
#include "native_bag_ui.h"
#include <stdio.h>
#include <string.h>
#define CHECK(value) do { if (!(value)) return __LINE__; } while (0)

uint32_t CheckOriginalBagPacketUI(void) {
    HGNativeBagUIState state={.active=1,.revision=7,.phase=HG_BAG_UI_TM_MESSAGE,.ready=1,.pocketId=POCKET_TMHMS,
        .selectedItemId=ITEM_TM17,.selectedQuantity=2,.actionCount=1,.actionMask=1,
        .actions={{.actionId=HG_BAG_UI_ACTION_TM_ACK,.enabled=1,.hitX=124,.hitY=24}},
        .message={.pixelBuffer=0x10000,.engine=1,.tileWidth=27,.tileHeight=4,.clearColor=15}};
    uint32_t x=0,y=0; HGNativeBagUIState copy=state;
    CHECK(sizeof(state)==1792 && HGNativeBagUIIsTMHM(&state));
    CHECK(HGNativeBagUIResolveTouch(&state,7,HG_BAG_UI_TM_MESSAGE,HG_BAG_UI_TOUCH_ACTION,0,&x,&y) && x==124 && y==24);
    CHECK(!memcmp(&state,&copy,sizeof(state)));
    CHECK(!HGNativeBagUIResolveTouch(&state,6,state.phase,HG_BAG_UI_TOUCH_ACTION,0,&x,&y)
        && !HGNativeBagUIResolveTouch(&state,7,HG_BAG_UI_ITEM_MENU,HG_BAG_UI_TOUCH_ACTION,0,&x,&y)
        && !HGNativeBagUIResolveTouch(&state,7,state.phase,HG_BAG_UI_TOUCH_ACTION,1,&x,&y)
        && !HGNativeBagUIResolveTouch(&state,7,state.phase,HG_BAG_UI_TOUCH_CANCEL,0,&x,&y)
        && !HGNativeBagUIResolveTouch(&state,7,state.phase,HG_BAG_UI_TOUCH_ITEM,0,&x,&y)
        && !HGNativeBagUIResolveTouch(NULL,7,state.phase,HG_BAG_UI_TOUCH_ACTION,0,&x,&y)
        && !HGNativeBagUIResolveTouch(&state,7,state.phase,HG_BAG_UI_TOUCH_ACTION,0,NULL,&y));
    copy=state; copy.ready=0; CHECK(HGNativeBagUIIsTMHM(&copy) && !HGNativeBagUIResolveTouch(&copy,7,copy.phase,HG_BAG_UI_TOUCH_ACTION,0,&x,&y));
    copy=state; copy.active=0; CHECK(!HGNativeBagUIIsTMHM(&copy) && !HGNativeBagUIResolveTouch(&copy,7,copy.phase,HG_BAG_UI_TOUCH_ACTION,0,&x,&y));
    copy=state; copy.mode=HG_BAG_UI_GIVE_MODE; CHECK(!HGNativeBagUIIsTMHM(&copy) && !HGNativeBagUIResolveTouch(&copy,7,copy.phase,HG_BAG_UI_TOUCH_ACTION,0,&x,&y));
    copy=state; copy.pocketId=POCKET_MEDICINE; CHECK(!HGNativeBagUIIsTMHM(&copy) && !HGNativeBagUIResolveTouch(&copy,7,copy.phase,HG_BAG_UI_TOUCH_ACTION,0,&x,&y));
    const uint32_t invalidItems[]={ITEM_TM01-1,ITEM_HM08+1};
    for (uint32_t i=0;i<2;++i) { copy=state; copy.selectedItemId=invalidItems[i]; CHECK(!HGNativeBagUIIsTMHM(&copy) && !HGNativeBagUIResolveTouch(&copy,7,copy.phase,HG_BAG_UI_TOUCH_ACTION,0,&x,&y)); }
    copy=state; copy.selectedItemId=ITEM_TM01; CHECK(HGNativeBagUIIsTMHM(&copy)); copy.selectedItemId=ITEM_HM08; CHECK(HGNativeBagUIIsTMHM(&copy));
    copy=state; copy.selectedQuantity=0; CHECK(!HGNativeBagUIResolveTouch(&copy,7,copy.phase,HG_BAG_UI_TOUCH_ACTION,0,&x,&y));
    copy=state; copy.actions[0].enabled=0; CHECK(!HGNativeBagUIResolveTouch(&copy,7,copy.phase,HG_BAG_UI_TOUCH_ACTION,0,&x,&y));
    copy=state; copy.actions[0].hitY++; CHECK(!HGNativeBagUIResolveTouch(&copy,7,copy.phase,HG_BAG_UI_TOUCH_ACTION,0,&x,&y));
    copy=state; copy.actionMask=0; CHECK(!HGNativeBagUIResolveTouch(&copy,7,copy.phase,HG_BAG_UI_TOUCH_ACTION,0,&x,&y));
    copy=state; copy.actionMask=3; CHECK(!HGNativeBagUIResolveTouch(&copy,7,copy.phase,HG_BAG_UI_TOUCH_ACTION,0,&x,&y));
    copy=state; copy.cancelEnabled=1; CHECK(!HGNativeBagUIResolveTouch(&copy,7,copy.phase,HG_BAG_UI_TOUCH_ACTION,0,&x,&y));
    copy=state; copy.message.pixelBuffer=0; CHECK(!HGNativeBagUIResolveTouch(&copy,7,copy.phase,HG_BAG_UI_TOUCH_ACTION,0,&x,&y));
    copy=state; copy.message.engine=0; CHECK(!HGNativeBagUIResolveTouch(&copy,7,copy.phase,HG_BAG_UI_TOUCH_ACTION,0,&x,&y));
    copy=state; copy.message.tileWidth=26; CHECK(!HGNativeBagUIResolveTouch(&copy,7,copy.phase,HG_BAG_UI_TOUCH_ACTION,0,&x,&y));
    copy=state; copy.message.tileHeight=5; CHECK(!HGNativeBagUIResolveTouch(&copy,7,copy.phase,HG_BAG_UI_TOUCH_ACTION,0,&x,&y));
    copy=state; copy.message.clearColor=0; CHECK(!HGNativeBagUIResolveTouch(&copy,7,copy.phase,HG_BAG_UI_TOUCH_ACTION,0,&x,&y));
    copy=state; ++copy.revision;
    CHECK(!HGNativeBagUIResolveTouch(&copy,7,copy.phase,HG_BAG_UI_TOUCH_ACTION,0,&x,&y)
        && HGNativeBagUIResolveTouch(&copy,copy.revision,copy.phase,HG_BAG_UI_TOUCH_ACTION,0,&x,&y));
    state.actionCount=2; state.actionMask=3;
    for (uint32_t i=0;i<2;++i) state.actions[i]=(HGNativeBagUIAction){.actionId=HG_BAG_UI_ACTION_TM_YES+i,.enabled=1,.hitX=224,.hitY=64+32*i};
    CHECK(HGNativeBagUIIsTMHM(&state));
    for (uint32_t i=0;i<2;++i) CHECK(HGNativeBagUIResolveTouch(&state,7,state.phase,HG_BAG_UI_TOUCH_ACTION,i,&x,&y) && x==224 && y==64+32*i);
    copy=state; copy.actions[1].actionId=HG_BAG_UI_ACTION_TM_YES; CHECK(!HGNativeBagUIIsTMHM(&copy) && !HGNativeBagUIResolveTouch(&copy,7,copy.phase,HG_BAG_UI_TOUCH_ACTION,1,&x,&y));
    copy=state; copy.actionMask=1; CHECK(!HGNativeBagUIResolveTouch(&copy,7,copy.phase,HG_BAG_UI_TOUCH_ACTION,0,&x,&y));
    copy=state; copy.actions[1].enabled=0; CHECK(!HGNativeBagUIResolveTouch(&copy,7,copy.phase,HG_BAG_UI_TOUCH_ACTION,1,&x,&y));
    copy=state; copy.actions[1].hitY=64; CHECK(!HGNativeBagUIResolveTouch(&copy,7,copy.phase,HG_BAG_UI_TOUCH_ACTION,1,&x,&y));
    state.phase=HG_BAG_UI_TM_EXIT_FADE; state.actionCount=1; state.actionMask=0; state.ready=0;
    state.actions[0]=(HGNativeBagUIAction){.actionId=HG_BAG_UI_ACTION_TM_ACK,.hitX=124,.hitY=24};
    CHECK(HGNativeBagUIIsTMHM(&state) && !HGNativeBagUIResolveTouch(&state,7,state.phase,HG_BAG_UI_TOUCH_ACTION,0,&x,&y));
    copy=state; copy.ready=1; copy.actionMask=1; copy.actions[0].enabled=1;
    CHECK(HGNativeBagUIIsTMHM(&copy) && !HGNativeBagUIResolveTouch(&copy,7,copy.phase,HG_BAG_UI_TOUCH_ACTION,0,&x,&y));
    state=(HGNativeBagUIState){.active=1,.revision=9,.phase=HG_BAG_UI_INPUT,.ready=1,.visibleCount=1,.itemMask=1,
        .selectedItemId=ITEM_POTION,.selectedQuantity=5,.cancelEnabled=1,.cancelX=223,.cancelY=179,
        .items={{.itemId=ITEM_POTION,.quantity=5,.enabled=1,.hitX=63,.hitY=52,.cropX=0,.cropY=32,.cropWidth=127,.cropHeight=41}}};
    CHECK(HGNativeBagUIIsList(&state) && !HGNativeBagUIIsTMHM(&state)
        && HGNativeBagUIResolveTouch(&state,9,state.phase,HG_BAG_UI_TOUCH_ITEM,0,&x,&y) && x==63 && y==52
        && HGNativeBagUIResolveTouch(&state,9,state.phase,HG_BAG_UI_TOUCH_CANCEL,0,&x,&y) && x==223 && y==179);
    state.phase=HG_BAG_UI_ITEM_MENU; state.actionCount=1; state.actionMask=1;
    state.actions[0]=(HGNativeBagUIAction){.actionId=0,.enabled=1,.hitX=46,.hitY=143};
    CHECK(HGNativeBagUIResolveTouch(&state,9,state.phase,HG_BAG_UI_TOUCH_ACTION,0,&x,&y) && x==46 && y==143);
    state.mode=HG_BAG_UI_GIVE_MODE; CHECK(!HGNativeBagUIResolveTouch(&state,9,state.phase,HG_BAG_UI_TOUCH_ACTION,0,&x,&y));
    state.phase=HG_BAG_UI_GIVE_INPUT;
    CHECK(HGNativeBagUIIsList(&state) && HGNativeBagUIResolveTouch(&state,9,state.phase,HG_BAG_UI_TOUCH_ITEM,0,&x,&y)
        && !HGNativeBagUIResolveTouch(&state,9,state.phase,HG_BAG_UI_TOUCH_ACTION,0,&x,&y));
    state.mode=HG_BAG_UI_SELL_MODE; state.phase=HG_BAG_UI_SELL_INPUT;
    CHECK(HGNativeBagUIIsSell(&state) && HGNativeBagUIIsList(&state) && !HGNativeBagUIIsSellTransaction(&state)
        && HGNativeBagUIResolveTouch(&state,9,state.phase,HG_BAG_UI_TOUCH_ITEM,0,&x,&y)
        && HGNativeBagUIResolveTouch(&state,9,state.phase,HG_BAG_UI_TOUCH_CANCEL,0,&x,&y)
        && !HGNativeBagUIResolveTouch(&state,9,state.phase,HG_BAG_UI_TOUCH_ACTION,0,&x,&y));
    uint32_t output=0, keys[]={1,2,16,32,64,128}, phases[]={HG_BAG_UI_SELL_QUANTITY,HG_BAG_UI_SELL_CONFIRM,HG_BAG_UI_SELL_MESSAGE};
    CHECK(!HGNativeBagUIResolveSellKey(&state,9,state.phase,1,&output));
    CHECK(!HGNativeBagUIResolveTouch(&state,8,state.phase,HG_BAG_UI_TOUCH_ITEM,0,&x,&y));
    copy=state; copy.ready=0; CHECK(!HGNativeBagUIResolveTouch(&copy,9,copy.phase,HG_BAG_UI_TOUCH_ITEM,0,&x,&y));
    copy=state; copy.phase=17; CHECK(!HGNativeBagUIIsList(&copy) && !HGNativeBagUIIsSellTransaction(&copy)
        && !HGNativeBagUIResolveSellKey(&copy,9,copy.phase,1,&output));
    for (uint32_t i=0;i<3;++i) {
        state.phase=phases[i]; copy=state;
        CHECK(HGNativeBagUIIsSellTransaction(&state) && !HGNativeBagUIIsList(&state));
        for (uint32_t j=0;j<6;++j) {
            uint32_t key=keys[j],allowed=i==0 || (i==1 && key!=16 && key!=32) || key==1 || key==2;
            output=0xfeed;
            CHECK(HGNativeBagUIResolveSellKey(&state,9,state.phase,key,&output)==allowed
                && output==(allowed ? key : 0xfeed));
        }
        CHECK(!HGNativeBagUIResolveSellKey(&state,8,state.phase,1,&output)
            && !HGNativeBagUIResolveSellKey(&state,0,state.phase,1,&output)
            && !HGNativeBagUIResolveSellKey(&state,9,HG_BAG_UI_SELL_INPUT,1,&output)
            && !HGNativeBagUIResolveSellKey(&state,9,state.phase,4,&output)
            && !HGNativeBagUIResolveSellKey(&state,9,state.phase,0,&output)
            && !HGNativeBagUIResolveSellKey(&state,9,state.phase,65,&output)
            && !HGNativeBagUIResolveSellKey(&state,9,state.phase,1,NULL)
            && !HGNativeBagUIResolveSellKey(NULL,9,state.phase,1,&output)
            && !HGNativeBagUIResolveTouch(&state,9,state.phase,HG_BAG_UI_TOUCH_ITEM,0,&x,&y)
            && !memcmp(&state,&copy,sizeof(state)));
        copy=state; copy.ready=0; CHECK(!HGNativeBagUIResolveSellKey(&copy,9,copy.phase,1,&output));
        copy=state; copy.active=0; CHECK(!HGNativeBagUIIsSell(&copy) && !HGNativeBagUIResolveSellKey(&copy,9,copy.phase,1,&output));
        copy=state; copy.revision=10; CHECK(!HGNativeBagUIResolveSellKey(&copy,9,copy.phase,1,&output));
        for (uint32_t mode=0;mode<4;++mode) if (mode!=HG_BAG_UI_SELL_MODE) {
            copy=state; copy.mode=mode; CHECK(!HGNativeBagUIIsSell(&copy) && !HGNativeBagUIResolveSellKey(&copy,9,copy.phase,1,&output));
        }
    }
    return 0;
}
#ifdef HG_NATIVE_BAG_UI_PACKET_CHECK_MAIN
int main(void) { uint32_t line=CheckOriginalBagPacketUI(); if (line) { fprintf(stderr,"Bag packet UI check line %u\n",line); return 1; }
    puts("PASS: public Bag TM ACK/YesNo, disabled fade identity, stale guards, ordinary/Give packets and seller list/quantity/confirm/message guards."); return 0; }
#endif
#else
// SPDX-License-Identifier: GPL-3.0-or-later
// Isolated check: include the real producers, window lifecycle and menu-label drawing.
// bag_ui_check_data.h is generated byte-for-byte from asm/bag_app_data.s in the private check directory.
#include "global.h"
#include <stdio.h>
#include <stdlib.h>
static void BagCheckClear(void *destination, u32 size);
static void BagCheckCopy16(const void *source, void *destination, u32 size);
#define MI_CpuClear8 BagCheckClear
#define MI_CpuCopy16 BagCheckCopy16
#define Heap_Alloc BagCheckAlloc
#define Heap_Free BagCheckFree
#define AddWindowParameterized BagCheckAddWindow
#define RemoveWindow BagCheckRemoveWindow
#define FillWindowPixelBuffer BagCheckFillWindow
#define ClearWindowTilemapAndScheduleTransfer BagCheckClearTilemap
#define ScheduleWindowCopyToVram BagCheckScheduleWindow
#define GetWindowWidth BagCheckWindowWidth
#define FontID_String_GetWidth BagCheckTextWidth
#define AddTextPrinterParameterizedWithColor BagCheckPrintText
#define ManagedSprite_SetPositionXYWithSubscreenOffset BagCheckSpritePosition
#define ManagedSprite_SetAnim BagCheckSpriteAnim
#define ManagedSprite_SetPaletteOverride BagCheckSpritePalette
#define ManagedSprite_SetDrawFlag BagCheckSpriteVisible
#define IsPaletteFadeFinished BagCheckFadeFinished
#define GetMainBgPlttAddr BagCheckMainPalette
#define GetSubBgPlttAddr BagCheckSubPalette
#define PlayerProfile_GetMoney BagCheckMoney
#define Save_Bag_Get BagCheckSaveBag
#define Bag_GetQuantity BagCheckQuantity
#define TextPrinterCheckActive BagCheckPrinterActive
#include "../../../src/application/bag/bag_app.c"
#include "../../../src/application/bag/bag_app_display.c"
#include "../../../src/pm_string.c"
#include "bag_ui_check_data.h"

#define CHECK(value) do { if (!(value)) return __LINE__; } while (0)
#define BOUNDARY(value) do { if (!(value)) { fprintf(stderr,"Bag boundary line %d: %s\n",__LINE__,#value); abort(); } } while (0)
static struct { Window *window; u8 *pixels; u32 size; } checkWindows[32];
static u16 checkPalettes[2][256];
static u32 checkAllocations, checkPrints, checkFrees;
static BOOL checkFade = TRUE;
static PlayerProfile checkProfile;
static Bag checkBag;
static SaveData *const checkSave=(SaveData *)(uintptr_t)0x12340;
static u32 checkMoney=12345;
static u16 checkOwnedQuantity=2;
static u8 checkPrinterActive;
static void BagCheckClear(void *destination, u32 size) { memset(destination,0,size); }
static void BagCheckCopy16(const void *source, void *destination, u32 size) { memcpy(destination,source,size); }
void *Heap_Alloc(enum HeapID heap, u32 size) { BOUNDARY(heap==HEAP_ID_6); void *p=malloc(size); BOUNDARY(p); ++checkAllocations; return p; }
void Heap_Free(void *pointer) { BOUNDARY(pointer && checkAllocations); --checkAllocations; free(pointer); }
static u32 BagCheckWindowSlot(Window *window) {
    for (u32 i=0;i<NELEMS(checkWindows);++i) if (checkWindows[i].window==window) return i;
    BOUNDARY(FALSE); return 0;
}
void AddWindowParameterized(BgConfig *bg,Window *window,u8 layer,u8 x,u8 y,u8 width,u8 height,u8 palette,u16 tile) {
    u32 i=0; while (i<NELEMS(checkWindows) && checkWindows[i].window) ++i;
    BOUNDARY(i<NELEMS(checkWindows) && layer<8 && width && height);
    checkWindows[i].window=window; checkWindows[i].size=width*height*32;
    checkWindows[i].pixels=malloc(checkWindows[i].size); BOUNDARY(checkWindows[i].pixels);
    // Low guest-offset tokens preserve production validation on a 64-bit host; pixels remain owned by this boundary.
    *window=(Window){.bgConfig=bg,.bgId=layer,.tilemapLeft=x,.tilemapTop=y,.width=width,.height=height,.paletteNum=palette,
        .baseTile=tile,.pixelBuffer=(void *)(uintptr_t)(0x10000+i*8192)};
}
void RemoveWindow(Window *window) {
    const HGNativeBagUIState *state=HGNativeBagUIGetState(); BOUNDARY(!state->ready);
    for (u32 i=0;i<HG_BAG_UI_ITEMS;i++) BOUNDARY(!state->items[i].text.pixelBuffer);
    for (u32 i=0;i<HG_BAG_UI_ACTIONS;i++) BOUNDARY(!state->actions[i].text.pixelBuffer);
    BOUNDARY(!state->currencyText.pixelBuffer && !state->quantityText.pixelBuffer && !state->quantityOnesText.pixelBuffer && !state->totalPriceText.pixelBuffer);
    u32 i=BagCheckWindowSlot(window); free(checkWindows[i].pixels); memset(&checkWindows[i],0,sizeof(checkWindows[i]));
    memset(window,0,sizeof(*window)); ++checkFrees;
}
void FillWindowPixelBuffer(Window *window,u8 value) {
    u32 i=BagCheckWindowSlot(window); value&=15; memset(checkWindows[i].pixels,value|(value<<4),checkWindows[i].size);
}
void ClearWindowTilemapAndScheduleTransfer(Window *window) { (void)BagCheckWindowSlot(window); }
void ScheduleWindowCopyToVram(Window *window) { (void)BagCheckWindowSlot(window); }
u8 GetWindowWidth(Window *window) { return window->width; }
u32 FontID_String_GetWidth(u32 font,String *string,u32 spacing) { BOUNDARY(!font && string && !spacing); return string->size*8; }
u8 AddTextPrinterParameterizedWithColor(Window *window,FontID font,String *string,u32 x,u32 y,u32 speed,u32 color,PrinterCallback_t callback) {
    u32 i=BagCheckWindowSlot(window); BOUNDARY(!font && string && x<window->width*8 && y<window->height*8 && speed==TEXT_SPEED_NOTRANSFER && !callback);
    checkWindows[i].pixels[0]=(color>>16)&15; ++checkPrints; return 0;
}
void ManagedSprite_SetPositionXYWithSubscreenOffset(ManagedSprite *sprite,s16 x,s16 y,fx32 offset) { BOUNDARY(sprite && x>=0 && y>=0 && offset==FX32_CONST(256)); }
void ManagedSprite_SetAnim(ManagedSprite *sprite,int animation) { BOUNDARY(sprite && animation>=0); }
void ManagedSprite_SetPaletteOverride(ManagedSprite *sprite,int palette) { BOUNDARY(sprite && palette>=0); }
void ManagedSprite_SetDrawFlag(ManagedSprite *sprite,int visible) { BOUNDARY(sprite && (visible==0 || visible==1)); }
BOOL IsPaletteFadeFinished(void) { return checkFade; }
u16 *GetMainBgPlttAddr(void) { return checkPalettes[0]; }
u16 *GetSubBgPlttAddr(void) { return checkPalettes[1]; }
u32 PlayerProfile_GetMoney(PlayerProfile *profile) { BOUNDARY(profile==&checkProfile); return checkMoney; }
Bag *Save_Bag_Get(SaveData *save) { BOUNDARY(save==checkSave); return &checkBag; }
u16 Bag_GetQuantity(Bag *bag,u16 itemId,enum HeapID heap) { BOUNDARY(bag==&checkBag && itemId==1 && heap==HEAP_ID_6); return checkOwnedQuantity; }
u8 TextPrinterCheckActive(u8 printerId) { (void)printerId; return checkPrinterActive; }

u32 CheckOriginalBagUI(void) {
    CHECK(HGNativeBagUIGetState()==&gNativeBagUIState && !gNativeBagUIState.active && !gNativeBagUIState.revision);
    BagApp app={0}; BagView view={0}; BgConfig bg={0}; ItemSlot slots[8][7]={0}; ManagedSprite sprites[39]={0};
    app.bagView=&view; app.bgConfig=&bg; app.cursorButton=8;
    for (u32 i=0;i<8;i++) { view.pockets[i].slots=slots[i]; view.pockets[i].pocketId=i; }
    for (u32 i=0;i<7;i++) slots[0][i]=(ItemSlot){.id=i+1,.quantity=i+2};
    view.pockets[0].itemCount=7;
    for (u32 i=0;i<NELEMS(sprites);i++) app.sprites[i]=&sprites[i];
    for (u32 i=0;i<256;i++) { checkPalettes[0][i]=0x100+i; checkPalettes[1][i]=0x300+i; }
    for (u32 i=0;i<16;i++) { app.menuStrings[i]=String_New(2,HEAP_ID_6); String_AddChar(app.menuStrings[i],i+1); }
    BagApp_AddWindows(&app); BagApp_AddItemListWindows(&app); app.listBank=1;
    NativeBagUIBind(&app); NativeBagUIPublish(&app,HG_BAG_UI_INPUT);
    const HGNativeBagUIState *state=HGNativeBagUIGetState();
    CHECK(sizeof(*state)==1792 && state->ready && state->pocketMask==255 && state->pocketCount==8 && state->visibleCount==6 && state->itemMask==63);
    CHECK(state->items[0].text.pixelBuffer==(u32)(uintptr_t)app.dynWindows[0].pixelBuffer && state->description.tileHeight==6);
    CHECK(state->previousEnabled && state->nextEnabled && state->pageCount==2 && state->description.palette[15]==checkPalettes[0][4*16+15]);
    u32 revision=state->revision,x=0,y=0; HGNativeBagUIState copy=*state;
    CHECK(HGNativeBagUIGetState()==state && !memcmp(&copy,state,sizeof(copy)));
    for (u32 transfer=0;transfer<2;++transfer) for (u32 layer=0;layer<8;++layer) {
        u16 *pending=transfer ? &bg.scrollScheduled : &bg.bufferTransferScheduled;
        *pending=1u<<layer; NativeBagUIBegin(&app); NativeBagUIPublish(&app,HG_BAG_UI_INPUT);
        CHECK(!state->ready && !state->items[0].text.pixelBuffer
            && !HGNativeBagUIResolveTouch(state,revision,HG_BAG_UI_INPUT,HG_BAG_UI_TOUCH_ITEM,0,&x,&y));
        *pending=0; NativeBagUIPublish(&app,HG_BAG_UI_INPUT);
        CHECK(state->ready && state->revision==revision && state->items[0].text.pixelBuffer
            && HGNativeBagUIResolveTouch(state,revision,HG_BAG_UI_INPUT,HG_BAG_UI_TOUCH_ITEM,0,&x,&y));
    }
    CHECK(HGNativeBagUIResolveTouch(state,revision,1,HG_BAG_UI_TOUCH_ITEM,0,&x,&y) && x==63 && y==52);
    CHECK(HGNativeBagUIResolveTouch(state,revision,1,HG_BAG_UI_TOUCH_POCKET,7,&x,&y) && x==239 && y==15);
    CHECK(HGNativeBagUIResolveTouch(state,revision,1,HG_BAG_UI_TOUCH_PREVIOUS,0,&x,&y) && x==19 && y==179);
    CHECK(HGNativeBagUIResolveTouch(state,revision,1,HG_BAG_UI_TOUCH_NEXT,0,&x,&y) && x==59 && y==179);
    CHECK(!HGNativeBagUIResolveTouch(state,revision+1,1,HG_BAG_UI_TOUCH_ITEM,0,&x,&y));
    CHECK(!HGNativeBagUIResolveTouch(state,revision,1,HG_BAG_UI_TOUCH_ACTION,0,&x,&y));
    NativeBagUIBegin(&app); CHECK(!state->ready && !state->description.pixelBuffer && !state->items[0].text.pixelBuffer && state->revision==revision);
    NativeBagUIPublish(&app,1); CHECK(state->ready && state->revision==revision);
    app.listBank=0; checkPalettes[1][11*16+15]^=1; NativeBagUIPublish(&app,1);
    CHECK(state->revision==revision && state->items[0].text.pixelBuffer==(u32)(uintptr_t)app.dynWindows[6].pixelBuffer);
    view.pockets[0].scroll=6; NativeBagUIPublish(&app,1);
    CHECK(state->ready && state->visibleCount==1 && state->items[0].absoluteIndex==6 && state->items[0].itemId==7 && state->previousEnabled && state->nextEnabled && state->revision>revision);
    view.pockets[0].scroll=0; view.pocketIndex=1; NativeBagUIPublish(&app,1);
    CHECK(state->ready && !state->visibleCount && state->pageCount==1 && !state->itemMask && !state->previousEnabled && !state->nextEnabled);
    CHECK(HGNativeBagUIResolveTouch(state,state->revision,1,HG_BAG_UI_TOUCH_CANCEL,0,&x,&y) && x==223 && y==179);
    view.pocketIndex=0; NativeBagUIPublish(&app,1); revision=state->revision;
    BagApp_RemoveItemListWindows(&app); CHECK(!state->ready && !state->items[0].text.pixelBuffer);
    BagApp_AddItemActionWindows(&app); view.itemId=slots[0][0].id;
    u8 actions[5]={0,6,255,12,11};
    BagApp_DrawItemMenu(&app,actions); NativeBagUIPublish(&app,4);
    CHECK(state->ready && state->actionCount==4 && state->actionMask==0 && state->cancelEnabled && state->selectedItemId==1);
    CHECK(state->actions[0].actionId==0 && state->actions[1].actionId==6 && state->actions[2].actionId==255 && state->actions[3].actionId==12);
    CHECK(!state->items[0].text.pixelBuffer && state->selectedEntry.pixelBuffer && state->prompt.pixelBuffer && state->actions[0].text.engine==1);
    CHECK(HGNativeBagUIResolveTouch(state,state->revision,4,HG_BAG_UI_TOUCH_CANCEL,0,&x,&y) && x==223 && y==179);
    bg.bufferTransferScheduled=(1u<<GF_BG_LYR_MAIN_1)|(1u<<GF_BG_LYR_SUB_0); NativeBagUIBegin(&app); NativeBagUIPublish(&app,HG_BAG_UI_ITEM_MENU);
    CHECK(!state->ready && !state->actions[0].text.pixelBuffer && !HGNativeBagUIResolveTouch(state,state->revision,4,HG_BAG_UI_TOUCH_CANCEL,0,&x,&y));
    bg.bufferTransferScheduled=0; NativeBagUIPublish(&app,HG_BAG_UI_ITEM_MENU); CHECK(state->ready);
    CHECK(!HGNativeBagUIResolveTouch(state,state->revision,4,HG_BAG_UI_TOUCH_ACTION,0,&x,&y));
    copy=*state; copy.actions[0].enabled=1; copy.actionMask=1;
    CHECK(HGNativeBagUIResolveTouch(&copy,copy.revision,4,HG_BAG_UI_TOUCH_ACTION,0,&x,&y) && x==46 && y==143);
    revision=state->revision; NativeBagUIBegin(&app); NativeBagUIPublish(&app,4); CHECK(state->ready && state->revision==revision);
    BagApp_RemoveItemActionWindows(&app); NativeBagUIPublish(&app,4); CHECK(!state->ready && !state->actionCount);
    BagApp_AddItemActionWindows(&app); BagApp_DrawItemMenu(&app,actions); NativeBagUIPublish(&app,4);
    CHECK(state->ready && state->revision>revision && !HGNativeBagUIResolveTouch(state,revision,4,HG_BAG_UI_TOUCH_CANCEL,0,&x,&y));
    BagApp_RemoveItemActionWindows(&app); BagApp_AddItemListWindows(&app); NativeBagUIPublish(&app,1);
    HGNativePartyUIWindow descriptor; Window bad=app.windows[0];
    CHECK(NativeBagUIWindow(&descriptor,&app,&bad,GF_BG_LYR_MAIN_1,32,6,0));
    bad.colorMode=GF_BG_CLR_8BPP; CHECK(!NativeBagUIWindow(&descriptor,&app,&bad,GF_BG_LYR_MAIN_1,32,6,0) && !descriptor.pixelBuffer);
    bad=app.windows[0]; bad.height=7; CHECK(!NativeBagUIWindow(&descriptor,&app,&bad,GF_BG_LYR_MAIN_1,32,6,0));
    bad=app.windows[0]; bad.bgConfig=NULL; CHECK(!NativeBagUIWindow(&descriptor,&app,&bad,GF_BG_LYR_MAIN_1,32,6,0));
    bad=app.windows[0]; bad.pixelBuffer=(void *)(uintptr_t)UINT32_MAX; CHECK(!NativeBagUIWindow(&descriptor,&app,&bad,GF_BG_LYR_MAIN_1,32,6,0));
    revision=state->revision; view.mode=HG_BAG_UI_GIVE_MODE; NativeBagUIPublish(&app,HG_BAG_UI_GIVE_INPUT);
    CHECK(sizeof(*state)==1792 && state->ready && HGNativeBagUIIsList(state) && state->revision>revision
        && state->pocketMask==255 && state->itemMask==63 && state->visibleCount==6 && !state->actionCount && !state->actionMask
        && state->description.pixelBuffer && state->page.pixelBuffer && state->cancel.pixelBuffer
        && !state->prompt.pixelBuffer && !state->selectedEntry.pixelBuffer && !state->tmInfo.pixelBuffer);
    CHECK(!HGNativeBagUIResolveTouch(state,revision,HG_BAG_UI_INPUT,HG_BAG_UI_TOUCH_ITEM,0,&x,&y));
    revision=state->revision;
    CHECK(HGNativeBagUIResolveTouch(state,revision,HG_BAG_UI_GIVE_INPUT,HG_BAG_UI_TOUCH_ITEM,0,&x,&y) && x==63 && y==52);
    CHECK(HGNativeBagUIResolveTouch(state,revision,HG_BAG_UI_GIVE_INPUT,HG_BAG_UI_TOUCH_POCKET,7,&x,&y) && x==239 && y==15);
    CHECK(HGNativeBagUIResolveTouch(state,revision,HG_BAG_UI_GIVE_INPUT,HG_BAG_UI_TOUCH_PREVIOUS,0,&x,&y) && x==19 && y==179);
    CHECK(HGNativeBagUIResolveTouch(state,revision,HG_BAG_UI_GIVE_INPUT,HG_BAG_UI_TOUCH_NEXT,0,&x,&y) && x==59 && y==179);
    CHECK(HGNativeBagUIResolveTouch(state,revision,HG_BAG_UI_GIVE_INPUT,HG_BAG_UI_TOUCH_CANCEL,0,&x,&y) && x==223 && y==179);
    CHECK(!HGNativeBagUIResolveTouch(state,revision,HG_BAG_UI_GIVE_INPUT,HG_BAG_UI_TOUCH_ACTION,0,&x,&y));
    for (u32 bank=0;bank<2;++bank) {
        app.listBank=bank; NativeBagUIBegin(&app);
        CHECK(!state->ready && !state->description.pixelBuffer && !state->items[0].text.pixelBuffer);
        NativeBagUIPublish(&app,HG_BAG_UI_GIVE_INPUT);
        CHECK(state->ready && state->revision==revision && state->items[0].text.pixelBuffer==(u32)(uintptr_t)app.dynWindows[(bank^1)*6].pixelBuffer);
        for (u32 transfer=0;transfer<2;++transfer) for (u32 layer=0;layer<8;++layer) {
            u16 *pending=transfer ? &bg.scrollScheduled : &bg.bufferTransferScheduled;
            *pending=1u<<layer; NativeBagUIBegin(&app); NativeBagUIPublish(&app,HG_BAG_UI_GIVE_INPUT);
            CHECK(!state->ready && !state->items[0].text.pixelBuffer
                && !HGNativeBagUIResolveTouch(state,revision,HG_BAG_UI_GIVE_INPUT,HG_BAG_UI_TOUCH_ITEM,0,&x,&y));
            *pending=0; NativeBagUIPublish(&app,HG_BAG_UI_GIVE_INPUT);
            CHECK(state->ready && state->revision==revision && state->items[0].text.pixelBuffer==(u32)(uintptr_t)app.dynWindows[(bank^1)*6].pixelBuffer
                && HGNativeBagUIResolveTouch(state,revision,HG_BAG_UI_GIVE_INPUT,HG_BAG_UI_TOUCH_ITEM,0,&x,&y));
        }
        Window *window=&app.dynWindows[(bank^1)*6]; bad=*window; window->colorMode=GF_BG_CLR_8BPP;
        NativeBagUIPublish(&app,HG_BAG_UI_GIVE_INPUT); CHECK(!state->ready && !state->items[0].text.pixelBuffer);
        *window=bad; NativeBagUIPublish(&app,HG_BAG_UI_GIVE_INPUT); CHECK(state->ready);
    }
    copy=*state; ++copy.items[0].hitX; CHECK(!HGNativeBagUIResolveTouch(&copy,revision,HG_BAG_UI_GIVE_INPUT,HG_BAG_UI_TOUCH_ITEM,0,&x,&y));
    copy=*state; copy.items[0].quantity=0; CHECK(!HGNativeBagUIResolveTouch(&copy,revision,HG_BAG_UI_GIVE_INPUT,HG_BAG_UI_TOUCH_ITEM,0,&x,&y));
    for (u32 mode=0;mode<3;++mode) for (u32 phase=1;phase<=15;++phase) {
        copy=*state; copy.mode=mode; copy.phase=phase;
        BOOL list=(mode==0 && phase==HG_BAG_UI_INPUT) || (mode==HG_BAG_UI_GIVE_MODE && phase==HG_BAG_UI_GIVE_INPUT);
        CHECK(HGNativeBagUIIsList(&copy)==list);
        CHECK(HGNativeBagUIResolveTouch(&copy,revision,phase,HG_BAG_UI_TOUCH_ITEM,0,&x,&y)==list);
        if (mode!=0 || phase!=HG_BAG_UI_ITEM_MENU) CHECK(!HGNativeBagUIResolveTouch(&copy,revision,phase,HG_BAG_UI_TOUCH_ACTION,0,&x,&y));
    }
    app.isMovingItem=TRUE; NativeBagUIPublish(&app,HG_BAG_UI_GIVE_INPUT); CHECK(!state->ready); app.isMovingItem=FALSE;
    checkFade=FALSE; NativeBagUIPublish(&app,HG_BAG_UI_GIVE_INPUT); CHECK(!state->ready); checkFade=TRUE;
    NativeBagUIPublish(&app,HG_BAG_UI_ITEM_MENU); CHECK(!state->ready && !state->actionCount);
    NativeBagUIPublish(&app,15); CHECK(!state->ready && !state->message.pixelBuffer);
    NativeBagUIPublish(&app,HG_BAG_UI_GIVE_INPUT); CHECK(state->ready); revision=state->revision;
    NativeBagUIBind(NULL); CHECK(!state->active && !state->ready && !state->items[0].text.pixelBuffer);
    NativeBagUIBind(&app); NativeBagUIPublish(&app,HG_BAG_UI_GIVE_INPUT);
    CHECK(state->ready && state->revision>revision && !HGNativeBagUIResolveTouch(state,revision,HG_BAG_UI_GIVE_INPUT,HG_BAG_UI_TOUCH_CANCEL,0,&x,&y));
    view.mode=HG_BAG_UI_SELL_MODE; view.saveData=checkSave; app.profile=&checkProfile;
    app.quantity=1; app.maxQuantity=2; app.sellPrice=50; view.itemId=1;
    NativeBagUIBegin(&app); NativeBagUIPublish(&app,HG_BAG_UI_SELL_INPUT);
    CHECK(state->ready && HGNativeBagUIIsSell(state) && HGNativeBagUIIsList(state) && state->selectedQuantity==2
        && state->transactionQuantity==1 && state->transactionMax==2 && state->unitPrice==50 && state->currency==checkMoney
        && HGNativeBagUIResolveTouch(state,state->revision,state->phase,HG_BAG_UI_TOUCH_ITEM,0,&x,&y));
    BagApp_RemoveItemListWindows(&app); BagApp_AddItemActionWindows(&app);
    NativeBagUIPublish(&app,HG_BAG_UI_SELL_QUANTITY); revision=state->revision;
    CHECK(state->ready && state->selectedQuantity==checkOwnedQuantity && state->transactionQuantity==1
        && state->currencyText.pixelBuffer==(u32)(uintptr_t)app.dynWindows[22].pixelBuffer
        && state->quantityText.pixelBuffer==(u32)(uintptr_t)app.dynWindows[17].pixelBuffer
        && state->quantityOnesText.pixelBuffer==(u32)(uintptr_t)app.dynWindows[18].pixelBuffer
        && state->quantityText.tileWidth==2 && state->quantityText.tileHeight==3
        && state->quantityOnesText.tileWidth==2 && state->quantityOnesText.tileHeight==3
        && state->totalPriceText.pixelBuffer==(u32)(uintptr_t)app.dynWindows[23].pixelBuffer);
    copy=*state;
    for (u32 repeat=0;repeat<3;++repeat) {
        NativeBagUIBegin(&app);
        CHECK(!state->ready && !state->currencyText.pixelBuffer && !state->quantityText.pixelBuffer
            && !state->quantityOnesText.pixelBuffer && !state->totalPriceText.pixelBuffer
            && state->transactionQuantity==1 && state->transactionMax==2 && state->unitPrice==50 && state->currency==checkMoney);
        NativeBagUIPublish(&app,HG_BAG_UI_SELL_QUANTITY);
        CHECK(state->ready && state->revision==revision && !memcmp(state,&copy,sizeof(copy)));
    }
    app.quantity=2; NativeBagUIBegin(&app); NativeBagUIPublish(&app,HG_BAG_UI_SELL_QUANTITY);
    u32 key=0;
    CHECK(state->ready && state->revision>revision && state->transactionQuantity==2
        && !HGNativeBagUIResolveSellKey(state,revision,state->phase,1,&key)
        && HGNativeBagUIResolveSellKey(state,state->revision,state->phase,1,&key) && key==1);
    revision=state->revision; checkFade=FALSE; NativeBagUIBegin(&app); NativeBagUIPublish(&app,HG_BAG_UI_SELL_QUANTITY);
    CHECK(!state->ready && !HGNativeBagUIResolveSellKey(state,revision,state->phase,1,&key)); checkFade=TRUE;
    Window digit=app.dynWindows[18]; app.dynWindows[18].height=4; NativeBagUIPublish(&app,HG_BAG_UI_SELL_QUANTITY);
    CHECK(!state->ready && !state->quantityOnesText.pixelBuffer); app.dynWindows[18]=digit;
    NativeBagUIPublish(&app,HG_BAG_UI_SELL_QUANTITY); CHECK(state->ready && state->revision==revision);
    YesNoPrompt yes={.buttonsAreInit=1,.result=3}; app.yesNoPrompt=&yes;
    NativeBagUIPublish(&app,HG_BAG_UI_SELL_CONFIRM);
    CHECK(state->ready && !state->quantityText.pixelBuffer && !state->quantityOnesText.pixelBuffer
        && HGNativeBagUIResolveSellKey(state,state->revision,state->phase,64,&key)
        && !HGNativeBagUIResolveSellKey(state,state->revision,state->phase,16,&key));
    yes.confirmSelectionTimer=1; NativeBagUIBegin(&app); NativeBagUIPublish(&app,HG_BAG_UI_SELL_CONFIRM);
    CHECK(!state->ready); yes.confirmSelectionTimer=0; yes.result=0; NativeBagUIPublish(&app,HG_BAG_UI_SELL_CONFIRM); CHECK(!state->ready);
    yes.result=3; NativeBagUIPublish(&app,HG_BAG_UI_SELL_CONFIRM); CHECK(state->ready); app.yesNoPrompt=NULL;
    checkPrinterActive=1; NativeBagUIBegin(&app); NativeBagUIPublish(&app,HG_BAG_UI_SELL_MESSAGE); CHECK(!state->ready);
    checkPrinterActive=0; NativeBagUIPublish(&app,HG_BAG_UI_SELL_MESSAGE);
    CHECK(state->ready && state->message.pixelBuffer && HGNativeBagUIResolveSellKey(state,state->revision,state->phase,2,&key)
        && !HGNativeBagUIResolveSellKey(state,state->revision,state->phase,64,&key));
    BagApp_RemoveItemActionWindows(&app); BagApp_AddItemListWindows(&app);
    view.mode=0; NativeBagUIPublish(&app,HG_BAG_UI_INPUT);
    revision=state->revision; checkFade=FALSE; NativeBagUIBegin(&app); NativeBagUIPublish(&app,1); CHECK(!state->ready && state->revision==revision);
    checkFade=TRUE; view.mode=1; NativeBagUIPublish(&app,1); CHECK(!state->ready); view.mode=0;
    NativeBagUISetMessage(&app,FALSE); NativeBagUIPublish(&app,BAG_STATE_WAIT_ITEM_MESSAGE); CHECK(!state->ready && state->message.pixelBuffer);
    NativeBagUIPublish(&app,1); revision=state->revision;
    copy=*state; copy.items[0].cropWidth=0; CHECK(!HGNativeBagUIResolveTouch(&copy,revision,1,HG_BAG_UI_TOUCH_ITEM,0,&x,&y));
    copy=*state; copy.items[0].hitX++; CHECK(!HGNativeBagUIResolveTouch(&copy,revision,1,HG_BAG_UI_TOUCH_ITEM,0,&x,&y));
    NativeBagUIBind(NULL); CHECK(!state->active && !state->ready && !state->description.pixelBuffer && state->revision>revision);
    copy=*state;
    // Mirror only OVY15's transient BSS reset; the separately linked resident Bag cache is outside those spans.
    sNativeBagApp=NULL; sNativeBagMessage=0; sNativeBagMenuLive=FALSE; memset(sNativeBagActions,0,sizeof(sNativeBagActions));
    CHECK(HGNativeBagUIGetState()==&gNativeBagUIState && !memcmp(state,&copy,sizeof(copy)));
    NativeBagUIBind(&app); NativeBagUIPublish(&app,1); CHECK(!HGNativeBagUIResolveTouch(state,revision,1,HG_BAG_UI_TOUCH_CANCEL,0,&x,&y)); NativeBagUIBind(NULL);
    BagApp_RemoveItemListWindows(&app);
    for (u32 i=0;i<8;i++) RemoveWindow(&app.windows[i]);
    for (u32 i=0;i<16;i++) String_Delete(app.menuStrings[i]);
    CHECK(!checkAllocations && checkPrints && checkFrees);
    return 0;
}

#ifdef HG_NATIVE_BAG_UI_CHECK_MAIN
int main(void) { u32 line=CheckOriginalBagUI(); if (line) { fprintf(stderr,"Bag UI check line %u\n",line); return 1; }
    puts("PASS: original Bag layouts, action labels, window lifetime, bank selection, typed touch guard and stable revisions."); return 0; }
#endif

#endif // Header-only public packet check or legacy producer component.
