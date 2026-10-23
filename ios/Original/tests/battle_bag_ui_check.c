// SPDX-License-Identifier: GPL-3.0-or-later
// Real producer and Window lifecycle; the original asm tables and resident battle_setup storage link separately.
#include "global.h"
#include <stdio.h>
#include <stdlib.h>
static void BattleBagCheckClear(void *destination, u32 size);
static void BattleBagCheckCopy16(const void *source, void *destination, u32 size);
#define MI_CpuClear8 BattleBagCheckClear
#define MI_CpuCopy16 BattleBagCheckCopy16
#define AllocWindows BattleBagCheckAllocWindows
#define AddWindow BattleBagCheckAddWindow
#define WindowArray_Delete BattleBagCheckDeleteWindows
#define RemoveWindow BattleBagCheckRemoveWindow
#define PaletteData_GetSelectedBuffersBitmask BattleBagCheckFade
#define GetSubBgPlttAddr BattleBagCheckPalette
#define BattleBagMenu_Create BattleBagCheckCreate
#include "../../../src/battle/battle_bag_menu.c"

#define CHECK(value) do { if (!(value)) return __LINE__; } while (0)
#define BOUNDARY(value) do { if (!(value)) { fprintf(stderr,"Battle Bag boundary %d: %s\n",__LINE__,#value); abort(); } } while (0)
static struct { Window *window; void *pixels; } checkWindows[32];
static u16 checkPalette[256], checkFade;
static u32 checkArrays, checkDeleted;
static void BattleBagCheckClear(void *destination, u32 size) { memset(destination,0,size); }
static void BattleBagCheckCopy16(const void *source, void *destination, u32 size) { memcpy(destination,source,size); }
Window *AllocWindows(enum HeapID heap, s32 count) {
    BOUNDARY(heap==HEAP_ID_6 && count>0 && count<=26); ++checkArrays;
    Window *windows=calloc(count,sizeof(*windows)); BOUNDARY(windows); return windows;
}
void AddWindow(BgConfig *bg, Window *window, const WindowTemplate *role) {
    u32 i=0; while (i<NELEMS(checkWindows) && checkWindows[i].window) ++i;
    BOUNDARY(i<NELEMS(checkWindows) && role->width && role->height && role->bgId<8);
    checkWindows[i].window=window; checkWindows[i].pixels=malloc(role->width*role->height*32); BOUNDARY(checkWindows[i].pixels);
    *window=(Window){.bgConfig=bg,.bgId=role->bgId,.tilemapLeft=role->left,.tilemapTop=role->top,.width=role->width,.height=role->height,
        .paletteNum=role->palette,.baseTile=role->baseTile,.pixelBuffer=(void *)(uintptr_t)(0x10000+i*8192)};
}
void RemoveWindow(Window *window) {
    const HGNativeBattleBagUIState *state=HGNativeBattleBagUIGetState();
    BOUNDARY(!state->ready && !state->selectedName.pixelBuffer && !state->selectedQuantityText.pixelBuffer && !state->description.pixelBuffer && !state->message.pixelBuffer);
    u32 i=0; while (i<NELEMS(checkWindows) && checkWindows[i].window!=window) ++i;
    BOUNDARY(i<NELEMS(checkWindows)); free(checkWindows[i].pixels); memset(&checkWindows[i],0,sizeof(checkWindows[i])); memset(window,0,sizeof(*window)); ++checkDeleted;
}
void WindowArray_Delete(Window *windows, s32 count) {
    BOUNDARY(windows && checkArrays && count>0); for (s32 i=0;i<count;++i) RemoveWindow(&windows[i]); --checkArrays; free(windows);
}
u16 PaletteData_GetSelectedBuffersBitmask(PaletteData *palette) { BOUNDARY(palette); return checkFade; }
u16 *GetSubBgPlttAddr(void) { return checkPalette; }

u32 CheckOriginalBattleBagUI(void) {
    CHECK(sizeof(HGNativeBattleBagUIState)==536 && offsetof(HGNativeBattleBagUIState,buttons)==112 && offsetof(HGNativeBattleBagUIState,selectedName)==328);
    const HGNativeBattleBagUIState *state=HGNativeBattleBagUIGetState(); CHECK(state==&gNativeBattleBagUIState && !state->active);
    BattleBagContext context={.heapID=HEAP_ID_6,.lastUsedItem=ITEM_POTION,.lastUsedPocket=0,.selectedItem=999};
    BgConfig bg={0}; PaletteData palette={0}; BattleBagMenu menu={.context=&context,.bgConfig=&bg,.paletteData=&palette,.state=1};
    bg.bgs[GF_BG_LYR_SUB_0].colorMode=bg.bgs[GF_BG_LYR_SUB_1].colorMode=GX_BG_COLORMODE_16;
    for (u32 i=0;i<256;++i) checkPalette[i]=0x100+i;
    menu.itemCounts[0]=7; menu.lastPages[0]=1;
    for (u32 i=0;i<7;++i) menu.pocketItems[0][i]=(ItemSlot){.id=ITEM_POTION+i,.quantity=i+1};
    NativeBattleBagUIBind(&menu); BattleBagMenu_AddWindows(&menu); NativeBattleBagUIPublish(&menu);
    CHECK(state->active && state->ready && state->buttonCount==6 && state->enabledMask==63 && state->selectedItemId==ITEM_POTION && context.selectedItem==999);
    u32 x=0,y=0,revision=state->revision; HGNativeBattleBagUIState copy=*state;
    CHECK(HGNativeBattleBagUIResolveTouch(state,revision,1,0,&x,&y) && x==63 && y==43);
    CHECK(HGNativeBattleBagUIGetState()==state && !memcmp(state,&copy,sizeof(copy)));
    NativeBattleBagUIBegin(&menu); CHECK(!state->ready && !state->selectedName.pixelBuffer);
    NativeBattleBagUIPublish(&menu); CHECK(state->ready && state->revision==revision);
    checkFade=1; NativeBattleBagUIBegin(&menu); NativeBattleBagUIPublish(&menu); CHECK(!state->ready && state->revision==revision);
    checkFade=0; NativeBattleBagUIPublish(&menu); CHECK(state->ready && state->revision==revision);
    context.lastUsedPocket=4; NativeBattleBagUIPublish(&menu);
    CHECK(state->ready && !(state->enabledMask&16) && !HGNativeBattleBagUIResolveTouch(state,state->revision,1,4,&x,&y)
        && HGNativeBattleBagUIResolveTouch(state,state->revision,1,5,&x,&y));
    menu.pocket=4; NativeBattleBagUIPublish(&menu); CHECK(!state->ready && !HGNativeBattleBagUIResolveTouch(state,state->revision,1,5,&x,&y));
    menu.pocket=0; context.lastUsedPocket=0;
    BattleBagMenu_RemovePageWindows(&menu); menu.page=1; menu.state=2; BattleBagMenu_AddPageWindows(&menu,1); menu.itemListWindowBank=1;
    NativeBattleBagUIPublish(&menu);
    CHECK(state->ready && state->buttonCount==9 && state->enabledMask==511 && state->pageCount==2);
    CHECK(state->selectedName.pixelBuffer==(u32)(uintptr_t)menu.pageWindows[0].pixelBuffer
        && state->selectedQuantityText.pixelBuffer==(u32)(uintptr_t)menu.pageWindows[1].pixelBuffer);
    CHECK(state->selectedName.tileWidth==14 && state->selectedName.tileHeight==3 && state->selectedName.palette[3]==checkPalette[3]
        && state->selectedQuantityText.palette[3]==checkPalette[4*16+3]);
    CHECK(HGNativeBattleBagUIResolveTouch(state,state->revision,2,0,&x,&y) && x==63 && y==31);
    copy=*state; ++copy.buttons[0].hitX; CHECK(!HGNativeBattleBagUIResolveTouch(&copy,copy.revision,2,0,&x,&y));
    revision=state->revision; NativeBattleBagUIBegin(&menu); NativeBattleBagUIPublish(&menu); CHECK(state->ready && state->revision==revision);
    menu.itemListWindowBank=0; checkPalette[1]^=1; NativeBattleBagUIPublish(&menu);
    CHECK(state->ready && state->revision==revision && state->selectedName.pixelBuffer==(u32)(uintptr_t)menu.pageWindows[12].pixelBuffer);
    context.pocketPages[0]=1; NativeBattleBagUIPublish(&menu);
    CHECK(state->ready && state->enabledMask==449 && state->selectedItemId==ITEM_POTION+6 && state->selectedQuantity==7);
    CHECK(!HGNativeBattleBagUIResolveTouch(state,state->revision,2,1,&x,&y)
        && HGNativeBattleBagUIResolveTouch(state,state->revision,2,7,&x,&y) && HGNativeBattleBagUIResolveTouch(state,state->revision,2,8,&x,&y));
    BattleBagMenu_RemovePageWindows(&menu); menu.page=2; menu.state=3; BattleBagMenu_AddPageWindows(&menu,2); NativeBattleBagUIPublish(&menu);
    CHECK(state->ready && state->enabledMask==3 && state->selectedItemId==ITEM_POTION+6 && context.selectedItem==999
        && state->selectedName.pixelBuffer==(u32)(uintptr_t)menu.pageWindows[0].pixelBuffer && state->description.tileHeight==6);
    CHECK(HGNativeBattleBagUIResolveTouch(state,state->revision,3,0,&x,&y) && x==103 && y==171);
    revision=state->revision; menu.state=10; NativeBattleBagUIPublish(&menu);
    CHECK(state->ready && state->buttonCount==1 && state->enabledMask==1 && state->message.clearColor==15 && state->message.palette[15]==checkPalette[15*16+15]);
    CHECK(HGNativeBattleBagUIResolveTouch(state,state->revision,10,0,&x,&y) && x==124 && y==168
        && !HGNativeBattleBagUIResolveTouch(state,revision,3,0,&x,&y));
    copy=*state; ++copy.buttons[0].hitY; CHECK(!HGNativeBattleBagUIResolveTouch(&copy,copy.revision,10,0,&x,&y));
    for (u32 page=0;page<=2;++page) {
        BattleBagMenu_RemovePageWindows(&menu); menu.page=page; menu.state=page+1; BattleBagMenu_AddPageWindows(&menu,page);
        u32 button=page==0 ? 5 : page==1 ? 6 : 1;
        for (u32 bank=0;bank<2;++bank) {
            menu.itemListWindowBank=bank; NativeBattleBagUIPublish(&menu); CHECK(state->ready);
            u32 pendingRevision=state->revision;
            for (u32 transfer=0;transfer<2;++transfer) for (u32 layer=0;layer<8;++layer) {
                u16 *pending=transfer ? &bg.scrollScheduled : &bg.bufferTransferScheduled;
                *pending=1u<<layer; NativeBattleBagUIBegin(&menu); NativeBattleBagUIPublish(&menu);
                CHECK(state->ready==(layer<GF_BG_LYR_SUB_0)
                    && HGNativeBattleBagUIResolveTouch(state,pendingRevision,menu.state,button,&x,&y)==(layer<GF_BG_LYR_SUB_0));
                *pending=0; NativeBattleBagUIPublish(&menu);
                CHECK(state->ready && state->revision==pendingRevision && HGNativeBattleBagUIResolveTouch(state,pendingRevision,menu.state,button,&x,&y));
            }
        }
    }
    menu.state=HG_BATTLE_BAG_UI_ACK; NativeBattleBagUIPublish(&menu);
    bg.scrollScheduled=1u<<GF_BG_LYR_SUB_2; NativeBattleBagUIPublish(&menu);
    CHECK(!state->ready && !HGNativeBattleBagUIResolveTouch(state,state->revision,HG_BATTLE_BAG_UI_ACK,0,&x,&y));
    bg.scrollScheduled=0; NativeBattleBagUIPublish(&menu); CHECK(state->ready);
    HGNativePartyUIWindow descriptor; Window bad=menu.pageWindows[2]; bad.colorMode=GF_BG_CLR_8BPP;
    CHECK(!NativeBattleBagUIWindow(&descriptor,&menu,&bad,&gBattleBagUseItemWindows[2],0) && !descriptor.pixelBuffer);
    bad=menu.pageWindows[2]; bad.pixelBuffer=(void *)(uintptr_t)UINT32_MAX;
    CHECK(!NativeBattleBagUIWindow(&descriptor,&menu,&bad,&gBattleBagUseItemWindows[2],0));
    context.isTutorial=1; NativeBattleBagUIPublish(&menu); CHECK(!state->ready); context.isTutorial=0;
    NativeBattleBagUIPublish(&menu); copy=*state; copy.buttons[0].hitX=256;
    CHECK(!HGNativeBattleBagUIResolveTouch(&copy,copy.revision,10,0,&x,&y)
        && !HGNativeBattleBagUIResolveTouch(state,state->revision,10,1,&x,&y)
        && !HGNativeBattleBagUIResolveTouch(state,state->revision,10,0,NULL,&y));
    revision=state->revision; NativeBattleBagUIBind(NULL); CHECK(!state->active && state->revision>revision);
    HGNativeBattleBagUIState cleared={.revision=state->revision}; CHECK(!memcmp(state,&cleared,sizeof(cleared)));
    BattleBagMenu_RemoveWindows(&menu); copy=*state;
    sNativeBattleBagOwner=NULL; sNativeBattleBagWindowsLive=FALSE;
    CHECK(!memcmp(state,&copy,sizeof(copy)) && HGNativeBattleBagUIGetState()==&gNativeBattleBagUIState);
    menu.page=0; menu.state=1; NativeBattleBagUIBind(&menu); BattleBagMenu_AddWindows(&menu); NativeBattleBagUIPublish(&menu);
    CHECK(state->ready && state->revision>revision && !HGNativeBattleBagUIResolveTouch(state,revision,1,5,&x,&y));
    revision=state->revision; NativeBattleBagUIBind(&menu); CHECK(state->revision>revision && !state->ready);
    NativeBattleBagUIBind(NULL); BattleBagMenu_RemoveWindows(&menu);
    CHECK(!checkArrays && checkDeleted); for (u32 i=0;i<NELEMS(checkWindows);++i) CHECK(!checkWindows[i].window);
    return 0;
}

#ifdef HG_NATIVE_BATTLE_BAG_UI_CHECK_MAIN
int main(void) { u32 line=CheckOriginalBattleBagUI(); if (line) { fprintf(stderr,"Battle Bag UI check line %u\n",line); return 1; }
    puts("PASS: real battle Bag producer, original layouts, visible banks, touch masks, borrowed windows and resident lifetime revisions."); return 0; }
#endif
