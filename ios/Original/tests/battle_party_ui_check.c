// SPDX-License-Identifier: GPL-3.0-or-later
// Real producer and Window lifecycle; original asm tables, Party getter and resident battle_setup storage link separately.
#include "global.h"
#include <stdio.h>
#include <stdlib.h>
static void BattlePartyCheckClear(void *destination, u32 size);
static void BattlePartyCheckCopy16(const void *source, void *destination, u32 size);
#define MI_CpuClear8 BattlePartyCheckClear
#define MI_CpuCopy16 BattlePartyCheckCopy16
#define AllocWindows BattlePartyCheckAllocWindows
#define AddWindow BattlePartyCheckAddWindow
#define WindowArray_Delete BattlePartyCheckDeleteWindows
#define RemoveWindow BattlePartyCheckRemoveWindow
#define PaletteData_GetSelectedBuffersBitmask BattlePartyCheckFade
#define GetSubBgPlttAddr BattlePartyCheckPalette
#define BattleSystem_GetBattleType BattlePartyCheckBattleType
#include "../../../src/battle/battle_party_menu.c"

#define CHECK(value) do { if (!(value)) return __LINE__; } while (0)
#define BOUNDARY(value) do { if (!(value)) { fprintf(stderr,"Battle Party boundary %d: %s\n",__LINE__,#value); abort(); } } while (0)
static struct { Window *window; void *pixels; } checkWindows[16];
static u16 checkPalette[256], checkFade;
static u32 checkArrays, checkDeleted, checkBattleType;
static void BattlePartyCheckClear(void *destination, u32 size) { memset(destination,0,size); }
static void BattlePartyCheckCopy16(const void *source, void *destination, u32 size) { memcpy(destination,source,size); }
Window *AllocWindows(enum HeapID heap, s32 count) {
    BOUNDARY(heap==HEAP_ID_6 && count>0 && count<=6); ++checkArrays;
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
    const HGNativeBattlePartyUIState *state=HGNativeBattlePartyUIGetState();
    BOUNDARY(!state->ready && !state->prompt.pixelBuffer && !state->message.pixelBuffer);
    u32 i=0; while (i<NELEMS(checkWindows) && checkWindows[i].window!=window) ++i;
    BOUNDARY(i<NELEMS(checkWindows)); free(checkWindows[i].pixels); memset(&checkWindows[i],0,sizeof(checkWindows[i])); memset(window,0,sizeof(*window)); ++checkDeleted;
}
void WindowArray_Delete(Window *windows, s32 count) {
    BOUNDARY(windows && checkArrays && count>0); for (s32 i=0;i<count;++i) RemoveWindow(&windows[i]); --checkArrays; free(windows);
}
u16 PaletteData_GetSelectedBuffersBitmask(PaletteData *palette) { BOUNDARY(palette); return checkFade; }
u16 *GetSubBgPlttAddr(void) { return checkPalette; }
u32 BattleSystem_GetBattleType(BattleSystem *system) { BOUNDARY(system==(BattleSystem *)&checkBattleType); return checkBattleType; }

u32 CheckOriginalBattlePartyUI(void) {
    CHECK(sizeof(HGNativeBattlePartyUIState)==608 && offsetof(HGNativeBattlePartyUIState,mons)==48
        && offsetof(HGNativeBattlePartyUIState,buttons)==336 && offsetof(HGNativeBattlePartyUIState,prompt)==504);
    const HGNativeBattlePartyUIState *state=HGNativeBattlePartyUIGetState(); CHECK(state==&gNativeBattlePartyUIState && !state->active);
    Party party={0}; party.core.maxCount=6; party.core.curCount=2;
    BattlePartyContext context={.party=&party,.heapID=HEAP_ID_6,.battleSystem=(BattleSystem *)&checkBattleType,.item=0xBEEF,.pocket=0xA5};
    memset(context.partyOrder,0xA5,sizeof(context.partyOrder)); context.partyOrder[0]=0; context.partyOrder[1]=1;
    BgConfig bg={0}; PaletteData palette={0}; BattlePartyMenu menu={.context=&context,.bgConfig=&bg,.paletteData=&palette,.state=1};
    menu.mons[0]=(BattlePartyMenuMon){.species=SPECIES_CHIKORITA,.level=5,.hp=20,.maxHp=20,.status=7};
    menu.mons[1]=(BattlePartyMenuMon){.species=SPECIES_CHIKORITA,.level=1,.hp=1,.maxHp=1,.isEgg=1,.status=7};
    memset(&menu.mons[2],0xA5,4*sizeof(menu.mons[0]));
    bg.bgs[GF_BG_LYR_SUB_0].colorMode=bg.bgs[GF_BG_LYR_SUB_1].colorMode=GX_BG_COLORMODE_16;
    for (u32 i=0;i<256;++i) checkPalette[i]=0x100+i;
    NativeBattlePartyUIBind(&menu); BattlePartyMenu_AddWindows(&menu); NativeBattlePartyUIPublish(&menu);
    CHECK(state->ready && state->buttonCount==7 && state->enabledMask==67 && state->partyCount==2 && !state->itemId && !state->pocket);
    CHECK(context.item==0xBEEF && context.pocket==0xA5 && state->mons[0].slotType==BATTLE_PARTY_SLOT_ACTIVE && state->mons[1].slotType==BATTLE_PARTY_SLOT_OTHER);
    HGNativeBattlePartyUIMon empty={0}; for (u32 i=2;i<6;++i) CHECK(!memcmp(&state->mons[i],&empty,sizeof(empty)));
    u32 x=0,y=0,revision=state->revision;
    CHECK(HGNativeBattlePartyUIResolveTouch(state,revision,1,0,&x,&y) && x==63 && y==23 && state->buttons[0].cropWidth==128 && state->buttons[0].cropHeight==48);
    CHECK(HGNativeBattlePartyUIResolveTouch(state,revision,1,1,&x,&y) && x==191 && y==31 && state->buttons[1].cropX==128 && state->buttons[1].cropY==8);
    CHECK(!HGNativeBattlePartyUIResolveTouch(state,revision,1,2,&x,&y) && HGNativeBattlePartyUIResolveTouch(state,revision,1,6,&x,&y));
    HGNativeBattlePartyUIState copy=*state;
    CHECK(HGNativeBattlePartyUIGetState()==state && !memcmp(state,&copy,sizeof(copy)));
    ++copy.buttons[0].hitX; CHECK(!HGNativeBattlePartyUIResolveTouch(&copy,copy.revision,1,0,&x,&y));
    NativeBattlePartyUIBegin(&menu); CHECK(!state->ready && !state->prompt.pixelBuffer && !state->message.pixelBuffer);
    NativeBattlePartyUIPublish(&menu); CHECK(state->ready && state->revision==revision && state->prompt.clearColor==15 && !state->message.pixelBuffer);
    context.partyOrder[5]^=1; menu.mons[5].species^=1; NativeBattlePartyUIPublish(&menu); CHECK(state->revision==revision);
    checkFade=1; NativeBattlePartyUIBegin(&menu); NativeBattlePartyUIPublish(&menu); CHECK(!state->ready && state->revision==revision);
    checkFade=0; NativeBattlePartyUIPublish(&menu); CHECK(state->ready && state->revision==revision);
    context.mode=BATTLE_PARTY_MODE_SWITCH_NO_CANCEL; NativeBattlePartyUIPublish(&menu); CHECK(!state->ready);
    context.mode=BATTLE_PARTY_MODE_LEARN_MOVE; NativeBattlePartyUIPublish(&menu); CHECK(!state->ready);
    context.mode=BATTLE_PARTY_MODE_USE_ITEM; context.item=ITEM_SUPER_POTION; NativeBattlePartyUIPublish(&menu); CHECK(!state->ready);
    context.item=ITEM_POTION; context.pocket=0; NativeBattlePartyUIPublish(&menu);
    CHECK(state->ready && state->itemId==ITEM_POTION && HGNativeBattlePartyUIResolveTouch(state,state->revision,1,0,&x,&y)
        && HGNativeBattlePartyUIResolveTouch(state,state->revision,1,1,&x,&y));
    revision=state->revision; context.selectedMon=6; menu.state=17; NativeBattlePartyUIPublish(&menu);
    CHECK(!state->ready && !state->buttonCount && !state->enabledMask && !state->prompt.pixelBuffer && state->message.pixelBuffer
        && state->buttons[0].cropWidth==128 && state->buttons[1].cropY==8);
    CHECK(!HGNativeBattlePartyUIResolveTouch(state,state->revision,17,0,&x,&y));
    menu.state=18; NativeBattlePartyUIPublish(&menu);
    CHECK(state->ready && state->buttonCount==1 && state->enabledMask==1 && !state->prompt.pixelBuffer && state->message.clearColor==15);
    CHECK(HGNativeBattlePartyUIResolveTouch(state,state->revision,18,0,&x,&y) && x==124 && y==168
        && !HGNativeBattlePartyUIResolveTouch(state,revision,1,0,&x,&y));
    HGNativePartyUIWindow descriptor; Window bad=menu.windows[1]; bad.colorMode=GF_BG_CLR_8BPP;
    CHECK(!NativeBattlePartyUIWindow(&descriptor,&menu,&bad,&gBattlePartyWindowTemplates[1],15) && !descriptor.pixelBuffer);
    bad=menu.windows[1]; bad.pixelBuffer=(void *)(uintptr_t)UINT32_MAX;
    CHECK(!NativeBattlePartyUIWindow(&descriptor,&menu,&bad,&gBattlePartyWindowTemplates[1],15));
    BattlePartyMenu_RemovePageWindows(&menu); menu.page=1; menu.state=2; context.mode=BATTLE_PARTY_MODE_SWITCH; context.selectedMon=0;
    BattlePartyMenu_AddPageWindows(&menu,1); NativeBattlePartyUIPublish(&menu);
    CHECK(state->ready && state->buttonCount==4 && state->enabledMask==15 && !state->prompt.pixelBuffer && !state->message.pixelBuffer);
    CHECK(HGNativeBattlePartyUIResolveTouch(state,state->revision,2,0,&x,&y) && x==127 && y==75
        && state->buttons[0].cropWidth==240 && state->buttons[0].cropHeight==136);
    revision=state->revision; NativeBattlePartyUIBegin(&menu); NativeBattlePartyUIPublish(&menu); CHECK(state->ready && state->revision==revision);
    context.selectedMon=1; NativeBattlePartyUIPublish(&menu); CHECK(state->ready && state->enabledMask==9);
    CHECK(HGNativeBattlePartyUIResolveTouch(state,state->revision,2,0,&x,&y) && !HGNativeBattlePartyUIResolveTouch(state,state->revision,2,1,&x,&y)
        && !HGNativeBattlePartyUIResolveTouch(state,state->revision,2,2,&x,&y) && HGNativeBattlePartyUIResolveTouch(state,state->revision,2,3,&x,&y));
    copy=*state; copy.buttons[0].cropWidth=257; CHECK(!HGNativeBattlePartyUIResolveTouch(&copy,copy.revision,2,0,&x,&y));
    CHECK(!HGNativeBattlePartyUIResolveTouch(state,state->revision,2,4,&x,&y) && !HGNativeBattlePartyUIResolveTouch(state,state->revision,2,0,NULL,&y));
    revision=state->revision; NativeBattlePartyUIBind(NULL);
    HGNativeBattlePartyUIState cleared={.revision=state->revision}; CHECK(state->revision>revision && !memcmp(state,&cleared,sizeof(cleared)));
    BattlePartyMenu_RemoveWindows(&menu); copy=*state; sNativeBattlePartyOwner=NULL; sNativeBattlePartyWindowsLive=FALSE;
    CHECK(!memcmp(state,&copy,sizeof(copy)) && HGNativeBattlePartyUIGetState()==&gNativeBattlePartyUIState);
    menu.page=0; menu.state=1; context.selectedMon=0; NativeBattlePartyUIBind(&menu); BattlePartyMenu_AddWindows(&menu); NativeBattlePartyUIPublish(&menu);
    CHECK(state->ready && state->revision>revision && !HGNativeBattlePartyUIResolveTouch(state,revision,1,6,&x,&y));
    revision=state->revision; NativeBattlePartyUIBind(&menu); CHECK(state->revision>revision && !state->ready);
    NativeBattlePartyUIBind(NULL); BattlePartyMenu_RemoveWindows(&menu);
    CHECK(!checkArrays && checkDeleted); for (u32 i=0;i<NELEMS(checkWindows);++i) CHECK(!checkWindows[i].window);
    return 0;
}

#ifdef HG_NATIVE_BATTLE_PARTY_UI_CHECK_MAIN
int main(void) { u32 line=CheckOriginalBattlePartyUI(); if (line) { fprintf(stderr,"Battle Party UI check line %u\n",line); return 1; }
    puts("PASS: real battle Party producer, original card geometry, slot masks, messages, window lifetime and resident revisions."); return 0; }
#endif
