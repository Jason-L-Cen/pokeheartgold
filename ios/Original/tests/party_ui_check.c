// SPDX-License-Identifier: GPL-3.0-or-later
// Isolated component: real menu layout/list/string code, explicit platform boundaries.
#include "global.h"
#include <stdio.h>
#include <stdlib.h>

static void PartyCheckBlendBrightness(int plane, int brightness);
static void PartyCheckClear(void *destination, u32 size);
static void PartyCheckCopy16(const void *source, void *destination, u32 size);
#define G2_SetBlendBrightness PartyCheckBlendBrightness
#define MI_CpuClear8 PartyCheckClear
#define MI_CpuCopy16 PartyCheckCopy16
#define Heap_Alloc PartyCheckAlloc
#define Heap_Free PartyCheckFree
#define AddWindow PartyCheckAddWindow
#define RemoveWindow PartyCheckRemoveWindow
#define FillWindowPixelBuffer PartyCheckFillWindow
#define GetWindowWidth PartyCheckWindowWidth
#define ScheduleWindowCopyToVram PartyCheckScheduleWindow
#define LoadRectToBgTilemapRect PartyCheckLoadRect
#define FillBgTilemapRect PartyCheckFillRect
#define BgCommitTilemapBufferToVram PartyCheckCommit
#define FontID_String_GetCenterAlignmentX PartyCheckCenterText
#define AddTextPrinterParameterizedWithColor PartyCheckPrintText
#define Sprite_SetDrawFlag PartyCheckSpriteVisible
#define Sprite_GetPositionXY PartyCheckSpritePosition
#define IsPaletteFadeFinished PartyCheckFadeFinished
#define GetMainBgPlttAddr PartyCheckMainPalette
#define GetSubBgPlttAddr PartyCheckSubPalette
#define GetPartyMenuContextMenuActionFunc PartyCheckActionValue
#define TextPrinterCheckActive PartyCheckTextActive
#include "../../../src/party_context_menu.c"
#include "../../../src/pm_string.c"
#include "../../../src/list_menu_items.c"

#define CHECK(value) do { if (!(value)) return __LINE__; } while (0)
#define BOUNDARY(value) do { if (!(value)) { fprintf(stderr, "Party boundary line %d: %s\n", __LINE__, #value); abort(); } } while (0)
static struct { Window *window; u8 *pixels; u32 size; } checkWindows[16];
static union { u64 alignment; u8 bytes[sizeof(PartyMenuContextMenuCursor)]; } checkCursor;
static u32 checkCursorLive, checkAllocations, checkSpriteReads, checkPrints, checkRemovals;
static BOOL checkFade = TRUE;
static BOOL checkTextActive = TRUE;
static u16 checkPalette[2][256];

static void PartyCheckClear(void *destination, u32 size) { memset(destination, 0, size); }
static void PartyCheckCopy16(const void *source, void *destination, u32 size) { memcpy(destination, source, size); }
static void PartyCheckBlendBrightness(int plane, int brightness) { BOUNDARY(plane == 30 && brightness == 8); }
void *Heap_Alloc(enum HeapID heapID, u32 size) {
    BOUNDARY(heapID == HEAP_ID_PARTY_MENU);
    ++checkAllocations;
    if (size == sizeof(PartyMenuContextMenuCursor)) {
        BOUNDARY(!checkCursorLive); checkCursorLive = TRUE; return checkCursor.bytes;
    }
    void *result = malloc(size); BOUNDARY(result != NULL); return result;
}
void Heap_Free(void *pointer) {
    BOUNDARY(pointer != NULL && checkAllocations != 0); --checkAllocations;
    if (pointer == checkCursor.bytes) {
        BOUNDARY(checkCursorLive); checkCursorLive = FALSE; memset(checkCursor.bytes, 0xFF, sizeof(checkCursor.bytes));
    } else free(pointer);
}
static u32 PartyCheckWindowSlot(Window *window) {
    for (u32 i = 0; i < NELEMS(checkWindows); ++i) if (checkWindows[i].window == window) return i;
    BOUNDARY(FALSE); return 0;
}
void AddWindow(BgConfig *config, Window *window, const WindowTemplate *template) {
    u32 slot = 0; while (slot < NELEMS(checkWindows) && checkWindows[slot].window != NULL) ++slot;
    BOUNDARY(slot < NELEMS(checkWindows));
    u32 size = template->width * template->height * TILE_SIZE_4BPP;
    checkWindows[slot].window = window; checkWindows[slot].size = size;
    checkWindows[slot].pixels = malloc(size); BOUNDARY(checkWindows[slot].pixels != NULL);
    // Synthetic low addresses keep the owner-offset contract testable on a 64-bit host.
    *window = (Window){ .bgConfig = config, .bgId = template->bgId, .tilemapLeft = template->left, .tilemapTop = template->top,
        .width = template->width, .height = template->height, .paletteNum = template->palette, .baseTile = template->baseTile,
        .pixelBuffer = (void *)(uintptr_t)(0x10000 + slot * 4096) };
}
void RemoveWindow(Window *window) {
    u32 slot = PartyCheckWindowSlot(window);
    free(checkWindows[slot].pixels); memset(&checkWindows[slot], 0, sizeof(checkWindows[slot]));
    memset(window, 0, sizeof(*window)); ++checkRemovals;
}
void FillWindowPixelBuffer(Window *window, u8 value) {
    u32 slot = PartyCheckWindowSlot(window);
    BOUNDARY(value < 16 && checkWindows[slot].size == window->width * window->height * TILE_SIZE_4BPP);
    memset(checkWindows[slot].pixels, value | (value << 4), checkWindows[slot].size);
}
u8 GetWindowWidth(Window *window) { return window->width; }
void ScheduleWindowCopyToVram(Window *window) { (void)PartyCheckWindowSlot(window); }
void LoadRectToBgTilemapRect(BgConfig *config, u8 bg, const void *data, u8 x, u8 y, u8 width, u8 height) {
    BOUNDARY(config && bg == GF_BG_LYR_MAIN_0 && data && x < 32 && y < 32 && width && height);
}
void FillBgTilemapRect(BgConfig *config, u8 bg, u16 value, u8 x, u8 y, u8 width, u8 height, u8 mode) {
    (void)value; (void)mode; BOUNDARY(config && bg == GF_BG_LYR_MAIN_0 && x < 32 && y < 32 && width && height);
}
void BgCommitTilemapBufferToVram(BgConfig *config, u8 bg) { BOUNDARY(config && bg == GF_BG_LYR_MAIN_0); }
u32 FontID_String_GetCenterAlignmentX(FontID font, String *text, u32 spacing, u32 width) {
    BOUNDARY(font == 4 && text && spacing == 0 && width); return (width - text->size * 8) / 2;
}
u8 AddTextPrinterParameterizedWithColor(Window *window, FontID font, String *text, u32 x, u32 y, u32 speed, u32 color, PrinterCallback_t callback) {
    u32 slot = PartyCheckWindowSlot(window);
    BOUNDARY(font == 4 && text && x < window->width * 8 && y < window->height * 8 && speed == TEXT_SPEED_NOTRANSFER && !callback);
    checkWindows[slot].pixels[0] = (u8)(color >> 16); ++checkPrints; return 0;
}
void Sprite_SetDrawFlag(Sprite *sprite, BOOL flag) { BOUNDARY(sprite != NULL); sprite->drawFlag = flag; }
void Sprite_GetPositionXY(Sprite *sprite, s16 *x, s16 *y) {
    BOUNDARY(sprite && x && y); *x = sprite->matrix.x / FX32_ONE; *y = sprite->matrix.y / FX32_ONE; ++checkSpriteReads;
}
BOOL IsPaletteFadeFinished(void) { return checkFade; }
u8 TextPrinterCheckActive(u8 printerId) { BOUNDARY(printerId<MAX_TEXT_PRINTERS); return checkTextActive; }
u16 *GetMainBgPlttAddr(void) { return checkPalette[0]; }
u16 *GetSubBgPlttAddr(void) { return checkPalette[1]; }
u32 GetPartyMenuContextMenuActionFunc(int index) { return 0xAB000000u | index; } // Opaque values are never dispatched by this check.

static const TouchscreenHitbox checkRosterHits[7] = {
    { .rect = {0,48,0,128} }, { .rect = {8,56,128,0} }, { .rect = {48,96,0,128} },
    { .rect = {56,104,128,0} }, { .rect = {96,144,0,128} }, { .rect = {104,152,128,0} }, { .rect = {152,192,200,0} },
};
static void PartyCheckClose(PartyMenu *menu) {
    PartyMenu_CloseContextMenu(menu, menu->contextMenuCursor);
    ListMenuItems_Delete(menu->listMenuItems);
}

u32 CheckOriginalPartyUI(void) {
    PartyMenu menu = {0}; PartyMenuArgs args = {0}; BgConfig bg = {0}; Sprite sprites[10] = {0};
    menu.args = &args; menu.bgConfig = &bg;
    for (u32 i = 0; i < NELEMS(sprites); ++i) menu.sprites[i] = &sprites[i];
    for (u32 i = 0; i < 256; ++i) { checkPalette[0][i] = 0x100 + i; checkPalette[1][i] = 0x300 + i; }
    for (u32 i = 0; i < 16; ++i) { menu.contextMenuStrings[i] = String_New(2, HEAP_ID_PARTY_MENU); String_AddChar(menu.contextMenuStrings[i], i + 1); }
    for (u32 i = 0; i < 2; ++i) {
        menu.monsDrawState[i] = (PartyMenuMonsDrawState){ .species = 152 + i, .hp = 18, .maxHp = 20, .level = 5, .active = 1, .iconSprite = &sprites[i] };
        sprites[i].matrix.x = (30 + i * 128) * FX32_ONE; sprites[i].matrix.y = (16 + i * 8) * FX32_ONE;
        AddWindow(&bg, &menu.windows[i * PARTY_MENU_WINDOWS_PER_MON], &sMainWindowTemplates[i * 5]);
    }
    AddWindow(&bg, &menu.windows[PARTY_MENU_WINDOW_ID_37], &sMainWindowTemplates[45]);
    AddWindow(&bg, &menu.windows[PARTY_MENU_WINDOW_ID_39], &sMainWindowTemplates[47]);
    AddWindow(&bg, &menu.windows[PARTY_MENU_WINDOW_ID_32], &sAdditionalWindowTemplates[2]);
    NativePartyUIBind(&menu); NativePartyUIPublish(&menu, HG_PARTY_UI_ROSTER, checkRosterHits);
    const HGNativePartyUIState *state = HGNativePartyUIGetState();
    CHECK(sizeof(*state) == 1480 && state->ready && state->count == 2 && state->slotMask == 3 && state->mons[0].nickname.engine == 0);
    u32 revision = state->revision, x = 0, y = 0, reads = checkSpriteReads;
    HGNativePartyUIState copy = *state;
    CHECK(HGNativePartyUIGetState() == state && !memcmp(state, &copy, sizeof(copy)) && reads == checkSpriteReads);
    for (u32 transfer=0;transfer<2;++transfer) for (u32 layer=0;layer<8;++layer) {
        u16 *pending=transfer ? &bg.scrollScheduled : &bg.bufferTransferScheduled;
        *pending=1u<<layer; NativePartyUIBegin(&menu); NativePartyUIPublish(&menu,HG_PARTY_UI_ROSTER,checkRosterHits);
        CHECK(!state->ready && !state->mons[0].nickname.pixelBuffer && !state->mons[0].iconWidth
            && !HGNativePartyUIResolveTouch(state,revision,HG_PARTY_UI_ROSTER,HG_PARTY_UI_TOUCH_MON,0,&x,&y));
        *pending=0; NativePartyUIPublish(&menu,HG_PARTY_UI_ROSTER,checkRosterHits);
        CHECK(state->ready && state->revision==revision && state->mons[0].nickname.pixelBuffer && state->mons[0].iconWidth==32
            && HGNativePartyUIResolveTouch(state,revision,HG_PARTY_UI_ROSTER,HG_PARTY_UI_TOUCH_MON,0,&x,&y));
    }
    CHECK(HGNativePartyUIResolveTouch(state, revision, HG_PARTY_UI_ROSTER, HG_PARTY_UI_TOUCH_MON, 1, &x, &y) && x == 192 && y == 32);
    CHECK(HGNativePartyUIResolveTouch(state, revision, HG_PARTY_UI_ROSTER, HG_PARTY_UI_TOUCH_CANCEL, 0, &x, &y) && x == 228 && y == 172);
    CHECK(!HGNativePartyUIResolveTouch(state, revision + 1, 1, 0, 0, &x, &y) && !HGNativePartyUIResolveTouch(state, revision, 2, 0, 0, &x, &y));
    CHECK(!HGNativePartyUIResolveTouch(state, revision, 1, 0, UINT32_MAX, &x, &y) && !HGNativePartyUIResolveTouch(state, revision, 1, 1, 0, &x, &y));
    CHECK(!HGNativePartyUIResolveTouch(NULL, revision, 1, 0, 0, &x, &y) && !HGNativePartyUIResolveTouch(state, revision, 1, 0, 0, NULL, &y));
    NativePartyUIBegin(&menu); CHECK(!state->ready && state->revision == revision && !state->message.pixelBuffer && !state->mons[0].nickname.pixelBuffer);
    CHECK(!HGNativePartyUIResolveTouch(state, revision, 1, 0, 0, &x, &y));
    NativePartyUIPublish(&menu, 1, checkRosterHits); CHECK(state->ready && state->revision == revision);
    sprites[0].matrix.y -= 3 * FX32_ONE; checkPalette[0][15] ^= 1;
    NativePartyUIBegin(&menu); NativePartyUIPublish(&menu, 1, checkRosterHits);
    CHECK(state->revision == revision && state->mons[0].iconY == -3 && state->mons[0].iconWidth == 32 && state->mons[0].nickname.palette[15] == checkPalette[0][15]);
    checkFade = FALSE; NativePartyUIBegin(&menu); NativePartyUIPublish(&menu, 1, checkRosterHits); CHECK(!state->ready && state->revision == revision);
    checkFade = TRUE; menu.partyMonIndex = 1; NativePartyUIBegin(&menu); NativePartyUIPublish(&menu, 1, checkRosterHits); CHECK(state->revision > revision);
    menu.partyMonIndex = 0;

    u8 topIds[] = {PARTY_MON_CONTEXT_MENU_SUMMARY, PARTY_MON_CONTEXT_MENU_SWITCH, PARTY_MON_CONTEXT_MENU_ITEM, PARTY_MON_CONTEXT_MENU_QUIT};
    NativePartyUIBegin(&menu); PartyMenu_OpenContextMenu(&menu, topIds, NELEMS(topIds)); NativePartyUIPublish(&menu, 2, checkRosterHits);
    revision = state->revision;
    CHECK(state->ready && state->actionCount == 4 && state->actionEnabledMask == 15 && state->actions[2].textId == PARTY_MON_CONTEXT_MENU_ITEM + 1);
    CHECK(state->actions[3].textId == PARTY_MON_CONTEXT_MENU_QUIT + 1 && state->selectedName.engine == 1 && state->selectedItem.tileHeight == 3);
    CHECK(state->selectedName.palette[15] == checkPalette[1][2 * 16 + 15] && state->actions[0].text.palette[15] == checkPalette[0][2 * 16 + 15]);
    CHECK(state->actions[0].text.clearColor == 11 && state->actions[1].text.clearColor == 4);
    CHECK(HGNativePartyUIResolveTouch(state, revision, 2, 1, 3, &x, &y) && x == 228 && y == 172);
    copy = *state; copy.actions[0].enabled = 0; CHECK(!HGNativePartyUIResolveTouch(&copy, revision, 2, 1, 0, &x, &y));
    copy = *state; copy.actionEnabledMask = 0; CHECK(!HGNativePartyUIResolveTouch(&copy, revision, 2, 1, 0, &x, &y));
    NativePartyUIBegin(&menu); CHECK(!state->ready && !state->actions[0].text.pixelBuffer && !state->selectedItem.pixelBuffer);
    NativePartyUIPublish(&menu, 2, checkRosterHits); CHECK(state->ready && state->revision == revision);
    menu.contextMenuButtonAnim.active=TRUE; NativePartyUIBegin(&menu); NativePartyUIPublish(&menu,HG_PARTY_UI_CONTEXT,checkRosterHits);
    CHECK(!state->ready && state->mons[0].nickname.pixelBuffer && state->mons[0].iconWidth==32);
    menu.contextMenuButtonAnim.active=FALSE; NativePartyUIPublish(&menu,HG_PARTY_UI_CONTEXT,checkRosterHits); CHECK(state->ready);
    PartyMenuContextMenuCursor *oldCursor = menu.contextMenuCursor;
    PartyCheckClose(&menu); CHECK(!state->ready && !state->actions[0].text.pixelBuffer);
    menu.contextMenuCursor = (void *)1; NativePartyUIPublish(&menu, 2, checkRosterHits); CHECK(!state->ready && !state->actionCount);

    for (u32 count = 2; count <= 5; ++count) {
        u16 canary[NELEMS(menu.leadPanelTilemap)]; for (u32 i = 0; i < NELEMS(canary); ++i) canary[i] = 0x7000 + i;
        memcpy(menu.leadPanelTilemap, canary, sizeof(canary));
        menu.listMenuItems = ListMenuItems_New(count, HEAP_ID_PARTY_MENU);
        for (u32 i = 0; i < count; ++i) ListMenuItems_AddItem(menu.listMenuItems, menu.contextMenuStrings[i == count - 1 ? PARTY_MON_CONTEXT_MENU_QUIT : PARTY_MON_CONTEXT_MENU_GIVE + i], 0xABCD);
        PartyMenuContextMenu template = {.items = menu.listMenuItems, .numItems = count};
        NativePartyUIBegin(&menu); menu.contextMenuCursor = PartyMenu_CreateContextMenuCursor(&menu, &template, 0, HEAP_ID_PARTY_MENU, 1);
        CHECK(!memcmp(menu.leadPanelTilemap, canary, sizeof(canary))); // The original Window[8] declaration fails this canary.
        CHECK(menu.contextMenuCursor == oldCursor && menu.listMenuItems[0].text != menu.contextMenuStrings[PARTY_MON_CONTEXT_MENU_GIVE]);
        NativePartyUIPublish(&menu, 15, checkRosterHits); CHECK(state->ready && state->revision > revision && state->actions[0].textId == PARTY_MON_CONTEXT_MENU_GIVE + 1);
        revision = state->revision;
        CHECK(state->actions[count - 1].textId == PARTY_MON_CONTEXT_MENU_QUIT + 1);
        CHECK(HGNativePartyUIResolveTouch(state, revision, 15, 1, count - 1, &x, &y) && x == 228 && y == 172);
        if (count == 3) CHECK(state->actions[0].hitX == 192 && state->actions[0].hitY == 32 && state->actions[1].hitY == 64);
        NativePartyUIBegin(&menu); NativePartyUIPublish(&menu, 15, checkRosterHits); CHECK(state->revision == revision);
        PartyCheckClose(&menu); CHECK(!memcmp(menu.leadPanelTilemap, canary, sizeof(canary)));
    }

    HGNativePartyUIWindow descriptor;
    Window valid = menu.windows[0], bad = valid;
    CHECK(NativePartyUIWindow(&descriptor, &menu, &valid, GF_BG_LYR_MAIN_1, 9, 2, 0));
    bad.colorMode = GF_BG_CLR_8BPP; CHECK(!NativePartyUIWindow(&descriptor, &menu, &bad, GF_BG_LYR_MAIN_1, 9, 2, 0) && !descriptor.pixelBuffer);
    bad = valid; bad.width = 8; CHECK(!NativePartyUIWindow(&descriptor, &menu, &bad, GF_BG_LYR_MAIN_1, 9, 2, 0));
    bad = valid; bad.height = 3; CHECK(!NativePartyUIWindow(&descriptor, &menu, &bad, GF_BG_LYR_MAIN_1, 9, 2, 0));
    bad = valid; bad.paletteNum = 16; CHECK(!NativePartyUIWindow(&descriptor, &menu, &bad, GF_BG_LYR_MAIN_1, 9, 2, 0));
    bad = valid; bad.bgConfig = NULL; CHECK(!NativePartyUIWindow(&descriptor, &menu, &bad, GF_BG_LYR_MAIN_1, 9, 2, 0));
    bad = valid; bad.pixelBuffer = NULL; CHECK(!NativePartyUIWindow(&descriptor, &menu, &bad, GF_BG_LYR_MAIN_1, 9, 2, 0));
    bad = valid; bad.pixelBuffer = (void *)(uintptr_t)UINT32_MAX; CHECK(!NativePartyUIWindow(&descriptor, &menu, &bad, GF_BG_LYR_MAIN_1, 9, 2, 0));
    reads = checkSpriteReads; revision = state->revision;
    NativePartyUIBegin(&menu); NativePartyUIPublish(&menu, PARTY_MENU_STATE_WAIT_TEXT_PRINTER, checkRosterHits);
    CHECK(!state->ready && state->message.pixelBuffer && state->mons[0].nickname.pixelBuffer && checkSpriteReads==reads+2 && state->revision > revision);
    reads=checkSpriteReads;
    args.context = PARTY_MENU_CONTEXT_1; NativePartyUIPublish(&menu, 1, checkRosterHits); CHECK(!state->ready && reads == checkSpriteReads);
    args.context = PARTY_MENU_CONTEXT_0; args.panelLayout = 2; NativePartyUIPublish(&menu, 1, checkRosterHits); CHECK(!state->ready && reads == checkSpriteReads);
    args.panelLayout = 0;
    CHECK(HG_PARTY_UI_GIVE_CONTEXT==PARTY_MENU_CONTEXT_10 && HG_PARTY_UI_GIVE_ITEM==PARTY_MENU_STATE_GIVE_ITEM_TO_MON
        && HG_PARTY_UI_ITEM_RESULT==PARTY_MENU_STATE_PRINT_ITEM_SWAP_MESSAGE && HG_PARTY_UI_TAKE_RESULT==PARTY_MENU_STATE_PRINT_TAKE_ITEM_MESSAGE);
    AddWindow(&bg, &menu.windows[PARTY_MENU_WINDOW_ID_34], &sAdditionalWindowTemplates[4]);
    revision=state->revision; NativePartyUIBind(NULL); args.context=HG_PARTY_UI_GIVE_CONTEXT; menu.partyMonIndex=0;
    NativePartyUIBind(&menu); NativePartyUIPublish(&menu,HG_PARTY_UI_GIVE_ITEM,checkRosterHits);
    CHECK(sizeof(*state)==1480 && state->revision>revision && HGNativePartyUIIsItemProgress(state) && !state->ready
        && state->count==2 && state->slotMask==3 && !state->actionCount && !state->actionEnabledMask && !state->cancelEnabled
        && state->mons[0].nickname.pixelBuffer && !state->message.pixelBuffer);
    CHECK(!HGNativePartyUIResolveTouch(state,state->revision,HG_PARTY_UI_GIVE_ITEM,HG_PARTY_UI_TOUCH_CANCEL,0,&x,&y));
    NativePartyUISetMessage(&menu,&menu.windows[PARTY_MENU_WINDOW_ID_34]);
    NativePartyUIPublish(&menu,HG_PARTY_UI_ITEM_RESULT,checkRosterHits); revision=state->revision;
    CHECK(HGNativePartyUIIsItemProgress(state) && !state->ready && !state->actionCount && state->message.pixelBuffer
        && state->message.tileWidth==27 && state->message.tileHeight==4 && !state->message.engine && state->message.clearColor==15);
    for (u32 transfer=0;transfer<2;++transfer) for (u32 layer=0;layer<8;++layer) {
        u16 *pending=transfer ? &bg.scrollScheduled : &bg.bufferTransferScheduled;
        *pending=1u<<layer; NativePartyUIBegin(&menu); NativePartyUIPublish(&menu,HG_PARTY_UI_ITEM_RESULT,checkRosterHits);
        CHECK(!state->ready && state->message.pixelBuffer && !state->mons[0].nickname.pixelBuffer && !state->mons[0].iconWidth);
        *pending=0; NativePartyUIPublish(&menu,HG_PARTY_UI_ITEM_RESULT,checkRosterHits);
        CHECK(!state->ready && state->revision==revision && state->message.pixelBuffer && state->mons[0].nickname.pixelBuffer && state->mons[0].iconWidth==32);
    }
    checkFade=FALSE; NativePartyUIPublish(&menu,HG_PARTY_UI_ITEM_RESULT,checkRosterHits);
    CHECK(!state->ready && !state->mons[0].nickname.pixelBuffer && !state->mons[0].iconWidth && state->message.pixelBuffer);
    checkFade=TRUE; NativePartyUIPublish(&menu,HG_PARTY_UI_ITEM_RESULT,checkRosterHits);
    CHECK(!state->ready && state->mons[0].nickname.pixelBuffer && state->mons[0].iconWidth==32);
    for (u32 kind=HG_PARTY_UI_TOUCH_MON;kind<=HG_PARTY_UI_TOUCH_CANCEL;++kind) {
        CHECK(!HGNativePartyUIResolveTouch(state,revision,HG_PARTY_UI_ITEM_RESULT,kind,0,&x,&y));
        copy=*state; copy.ready=TRUE; CHECK(!HGNativePartyUIResolveTouch(&copy,revision,HG_PARTY_UI_ITEM_RESULT,kind,0,&x,&y));
    }
    NativePartyUIBegin(&menu); CHECK(!state->message.pixelBuffer && !state->mons[0].nickname.pixelBuffer);
    NativePartyUIPublish(&menu,HG_PARTY_UI_ITEM_RESULT,checkRosterHits); CHECK(!state->ready && state->revision==revision && state->message.pixelBuffer);
    menu.textPrinterId=3;
    TextPrinter printer={.template={.window=&menu.windows[PARTY_MENU_WINDOW_ID_34]},.active=TRUE,.id=3};
    for (u32 printerState=0;printerState<9;++printerState) {
        printer.state=printerState; NativePartyUIPrinterUpdated(&printer.template,0);
        NativePartyUIPublish(&menu,HG_PARTY_UI_ITEM_RESULT,checkRosterHits);
        CHECK(state->ready==(printerState==2) && state->cancelEnabled==(printerState==2) && !state->actionCount);
    }
    printer.state=2; printer.active=FALSE; NativePartyUIPrinterUpdated(&printer.template,0);
    NativePartyUIPublish(&menu,HG_PARTY_UI_ITEM_RESULT,checkRosterHits); CHECK(!state->ready); printer.active=TRUE;
    printer.callbackResult=1; NativePartyUIPrinterUpdated(&printer.template,0);
    NativePartyUIPublish(&menu,HG_PARTY_UI_ITEM_RESULT,checkRosterHits); CHECK(!state->ready); printer.callbackResult=0;
    printer.callbackParam=1; NativePartyUIPrinterUpdated(&printer.template,0);
    NativePartyUIPublish(&menu,HG_PARTY_UI_ITEM_RESULT,checkRosterHits); CHECK(!state->ready); printer.callbackParam=0;
    NativePartyUIPrinterUpdated(&printer.template,1); NativePartyUIPublish(&menu,HG_PARTY_UI_ITEM_RESULT,checkRosterHits); CHECK(!state->ready);
    printer.id=2; NativePartyUIPrinterUpdated(&printer.template,0);
    NativePartyUIPublish(&menu,HG_PARTY_UI_ITEM_RESULT,checkRosterHits); CHECK(!state->ready); printer.id=3;
    printer.template.window=&menu.windows[PARTY_MENU_WINDOW_ID_32]; NativePartyUIPrinterUpdated(&printer.template,0);
    NativePartyUIPublish(&menu,HG_PARTY_UI_ITEM_RESULT,checkRosterHits); CHECK(!state->ready); printer.template.window=&menu.windows[PARTY_MENU_WINDOW_ID_34];
    revision=state->revision; NativePartyUIPrinterUpdated(&printer.template,0); NativePartyUIPublish(&menu,HG_PARTY_UI_ITEM_RESULT,checkRosterHits);
    CHECK(state->ready && state->cancelEnabled && state->cancelX==124 && state->cancelY==168 && state->revision>revision && !state->actionCount);
    CHECK(!HGNativePartyUIResolveTouch(state,revision,HG_PARTY_UI_ITEM_RESULT,HG_PARTY_UI_TOUCH_CANCEL,0,&x,&y));
    for (u32 transfer=0;transfer<2;++transfer) for (u32 layer=0;layer<8;++layer) {
        u16 *pending=transfer ? &bg.scrollScheduled : &bg.bufferTransferScheduled;
        *pending=1u<<layer; NativePartyUIBegin(&menu); NativePartyUIPublish(&menu,HG_PARTY_UI_ITEM_RESULT,checkRosterHits);
        CHECK(!state->ready && !state->cancelEnabled && !state->mons[0].nickname.pixelBuffer && !state->mons[0].iconWidth && state->message.pixelBuffer
            && !HGNativePartyUIResolveTouch(state,state->revision,HG_PARTY_UI_ITEM_RESULT,HG_PARTY_UI_TOUCH_CANCEL,0,&x,&y));
        *pending=0; NativePartyUIPublish(&menu,HG_PARTY_UI_ITEM_RESULT,checkRosterHits);
        CHECK(state->ready && state->cancelEnabled && state->mons[0].nickname.pixelBuffer && state->mons[0].iconWidth==32
            && HGNativePartyUIResolveTouch(state,state->revision,HG_PARTY_UI_ITEM_RESULT,HG_PARTY_UI_TOUCH_CANCEL,0,&x,&y));
    }
    revision=state->revision;
    CHECK(HGNativePartyUIResolveTouch(state,revision,HG_PARTY_UI_ITEM_RESULT,HG_PARTY_UI_TOUCH_CANCEL,0,&x,&y) && x==124 && y==168);
    CHECK(!HGNativePartyUIResolveTouch(state,revision,HG_PARTY_UI_ITEM_RESULT,HG_PARTY_UI_TOUCH_MON,0,&x,&y)
        && !HGNativePartyUIResolveTouch(state,revision,HG_PARTY_UI_ITEM_RESULT,HG_PARTY_UI_TOUCH_ACTION,0,&x,&y)
        && !HGNativePartyUIResolveTouch(state,revision,HG_PARTY_UI_ITEM_RESULT,HG_PARTY_UI_TOUCH_CANCEL,1,&x,&y));
    copy=*state; copy.message.pixelBuffer=0; CHECK(!HGNativePartyUIResolveTouch(&copy,revision,copy.phase,HG_PARTY_UI_TOUCH_CANCEL,0,&x,&y));
    copy=*state; copy.message.tileWidth=26; CHECK(!HGNativePartyUIResolveTouch(&copy,revision,copy.phase,HG_PARTY_UI_TOUCH_CANCEL,0,&x,&y));
    copy=*state; copy.message.tileHeight=5; CHECK(!HGNativePartyUIResolveTouch(&copy,revision,copy.phase,HG_PARTY_UI_TOUCH_CANCEL,0,&x,&y));
    copy=*state; copy.message.engine=1; CHECK(!HGNativePartyUIResolveTouch(&copy,revision,copy.phase,HG_PARTY_UI_TOUCH_CANCEL,0,&x,&y));
    copy=*state; copy.message.clearColor=0; CHECK(!HGNativePartyUIResolveTouch(&copy,revision,copy.phase,HG_PARTY_UI_TOUCH_CANCEL,0,&x,&y));
    copy=*state; ++copy.cancelX; CHECK(!HGNativePartyUIResolveTouch(&copy,revision,copy.phase,HG_PARTY_UI_TOUCH_CANCEL,0,&x,&y));
    copy=*state; ++copy.cancelY; CHECK(!HGNativePartyUIResolveTouch(&copy,revision,copy.phase,HG_PARTY_UI_TOUCH_CANCEL,0,&x,&y));
    copy=*state; copy.actionCount=1; CHECK(!HGNativePartyUIResolveTouch(&copy,revision,copy.phase,HG_PARTY_UI_TOUCH_CANCEL,0,&x,&y));
    copy=*state; copy.actionEnabledMask=1; CHECK(!HGNativePartyUIResolveTouch(&copy,revision,copy.phase,HG_PARTY_UI_TOUCH_CANCEL,0,&x,&y));
    copy=*state; copy.context=PARTY_MENU_CONTEXT_9; CHECK(!HGNativePartyUIResolveTouch(&copy,revision,copy.phase,HG_PARTY_UI_TOUCH_CANCEL,0,&x,&y));
    copy=*state; copy.phase=HG_PARTY_UI_GIVE_ITEM; CHECK(!HGNativePartyUIResolveTouch(&copy,revision,copy.phase,HG_PARTY_UI_TOUCH_CANCEL,0,&x,&y));
    copy=*state; copy.phase=HG_PARTY_UI_TAKE_RESULT; CHECK(!HGNativePartyUIResolveTouch(&copy,revision,copy.phase,HG_PARTY_UI_TOUCH_CANCEL,0,&x,&y));
    NativePartyUIPublish(&menu,HG_PARTY_UI_GIVE_ITEM,checkRosterHits); CHECK(!state->ready && !state->cancelEnabled);
    NativePartyUIPublish(&menu,HG_PARTY_UI_ITEM_RESULT,checkRosterHits); CHECK(state->ready);
    checkTextActive=FALSE; NativePartyUIPublish(&menu,HG_PARTY_UI_ITEM_RESULT,checkRosterHits); CHECK(!state->ready && !state->cancelEnabled); checkTextActive=TRUE;
    checkFade=FALSE; NativePartyUIPublish(&menu,HG_PARTY_UI_ITEM_RESULT,checkRosterHits); CHECK(!state->ready && !state->cancelEnabled); checkFade=TRUE;
    NativePartyUIPublish(&menu,HG_PARTY_UI_ITEM_RESULT,checkRosterHits); CHECK(state->ready); revision=state->revision;
    NativePartyUIBegin(&menu); CHECK(!state->ready && !state->message.pixelBuffer);
    NativePartyUIPublish(&menu,HG_PARTY_UI_ITEM_RESULT,checkRosterHits); CHECK(state->ready && state->revision==revision);
    // Original Continue changes RenderText state 2 to 0 before the next callback.
    printer.state=0; NativePartyUIPrinterUpdated(&printer.template,0); NativePartyUIPublish(&menu,HG_PARTY_UI_ITEM_RESULT,checkRosterHits);
    CHECK(!state->ready && !state->cancelEnabled && state->revision>revision
        && !HGNativePartyUIResolveTouch(state,revision,HG_PARTY_UI_ITEM_RESULT,HG_PARTY_UI_TOUCH_CANCEL,0,&x,&y));
    bad=menu.windows[PARTY_MENU_WINDOW_ID_34]; menu.windows[PARTY_MENU_WINDOW_ID_34].width=26;
    NativePartyUIPublish(&menu,HG_PARTY_UI_ITEM_RESULT,checkRosterHits); CHECK(!state->message.pixelBuffer);
    menu.windows[PARTY_MENU_WINDOW_ID_34]=bad;
    for (u32 phase=9;phase<=17;++phase) {
        NativePartyUIPublish(&menu,phase,checkRosterHits);
        CHECK(!state->ready && !state->actionCount && HGNativePartyUIIsItemProgress(state)==(phase==11 || phase==16));
    }
    menu.partyMonIndex=HG_PARTY_UI_SLOTS; NativePartyUIPublish(&menu,HG_PARTY_UI_ITEM_RESULT,checkRosterHits);
    CHECK(!HGNativePartyUIIsItemProgress(state) && !state->count && !state->message.pixelBuffer);
    menu.partyMonIndex=0; menu.monsDrawState[0].active=FALSE; NativePartyUIPublish(&menu,HG_PARTY_UI_ITEM_RESULT,checkRosterHits);
    CHECK(!HGNativePartyUIIsItemProgress(state) && !state->count); menu.monsDrawState[0].active=TRUE;
    args.panelLayout=1; NativePartyUIPublish(&menu,HG_PARTY_UI_ITEM_RESULT,checkRosterHits); CHECK(!HGNativePartyUIIsItemProgress(state)); args.panelLayout=0;
    args.context=PARTY_MENU_CONTEXT_9; NativePartyUIPublish(&menu,HG_PARTY_UI_ITEM_RESULT,checkRosterHits); CHECK(!HGNativePartyUIIsItemProgress(state));
    args.context=PARTY_MENU_CONTEXT_0;
    for (u32 phase=11;phase<=17;++phase) {
        if (phase==HG_PARTY_UI_SUBCONTEXT) continue;
        NativePartyUIPublish(&menu,phase,checkRosterHits);
        CHECK(!state->ready && !state->actionCount && HGNativePartyUIIsItemProgress(state)==(phase==11 || phase==17));
    }
    menu.monsDrawState[0].heldItem=17; NativePartyUIPublish(&menu,HG_PARTY_UI_TAKE_RESULT,checkRosterHits); revision=state->revision;
    menu.monsDrawState[0].heldItem=0; NativePartyUIPublish(&menu,HG_PARTY_UI_TAKE_RESULT,checkRosterHits);
    CHECK(HGNativePartyUIIsItemProgress(state) && !state->ready && !state->mons[0].heldItem && state->message.pixelBuffer && state->revision>revision);
    copy=*state; copy.ready=TRUE; CHECK(!HGNativePartyUIResolveTouch(&copy,copy.revision,HG_PARTY_UI_TAKE_RESULT,HG_PARTY_UI_TOUCH_CANCEL,0,&x,&y));
    copy=*state; copy.active=FALSE; CHECK(!HGNativePartyUIIsItemProgress(&copy));
    copy=*state; copy.slotMask=0; CHECK(!HGNativePartyUIIsItemProgress(&copy));
    copy=*state; copy.selected=HG_PARTY_UI_SLOTS; CHECK(!HGNativePartyUIIsItemProgress(&copy));
    printer.state=2; NativePartyUIPrinterUpdated(&printer.template,0); NativePartyUIPublish(&menu,HG_PARTY_UI_TAKE_RESULT,checkRosterHits);
    CHECK(state->ready && HGNativePartyUIResolveTouch(state,state->revision,HG_PARTY_UI_TAKE_RESULT,HG_PARTY_UI_TOUCH_CANCEL,0,&x,&y) && x==124 && y==168);
    bg.bufferTransferScheduled=0xFF; NativePartyUIBegin(&menu); NativePartyUIPublish(&menu,HG_PARTY_UI_TAKE_RESULT,checkRosterHits);
    CHECK(!state->ready && !state->cancelEnabled && !state->mons[0].nickname.pixelBuffer && !state->mons[0].iconWidth && state->message.pixelBuffer);
    bg.bufferTransferScheduled=0; NativePartyUIPublish(&menu,HG_PARTY_UI_TAKE_RESULT,checkRosterHits); CHECK(state->ready && state->mons[0].nickname.pixelBuffer);
    NativePartyUISetMessage(&menu,&menu.windows[PARTY_MENU_WINDOW_ID_34]); NativePartyUIPublish(&menu,HG_PARTY_UI_TAKE_RESULT,checkRosterHits);
    CHECK(!state->ready && !state->cancelEnabled);
    NativePartyUIPrinterUpdated(&printer.template,0); NativePartyUIPublish(&menu,HG_PARTY_UI_TAKE_RESULT,checkRosterHits); CHECK(state->ready);
    NativePartyUIBegin(&menu); CHECK(!state->message.pixelBuffer && !state->mons[0].nickname.pixelBuffer);
    RemoveWindow(&menu.windows[PARTY_MENU_WINDOW_ID_34]);
    revision=state->revision; NativePartyUIBind(NULL); NativePartyUIBind(&menu);
    CHECK(!sNativePartyMessageWaiting);
    NativePartyUIPublish(&menu, 1, checkRosterHits);
    CHECK(state->ready && !HGNativePartyUIIsItemProgress(state) && state->message.pixelBuffer && state->revision>revision
        && !HGNativePartyUIResolveTouch(state,revision,HG_PARTY_UI_TAKE_RESULT,HG_PARTY_UI_TOUCH_CANCEL,0,&x,&y));
    revision = state->revision;
    copy = *state; copy.context = 1; CHECK(!HGNativePartyUIResolveTouch(&copy, revision, 1, 0, 0, &x, &y));
    copy = *state; copy.panelLayout = 1; CHECK(!HGNativePartyUIResolveTouch(&copy, revision, 1, 0, 0, &x, &y));
    copy = *state; copy.mons[0].hitX = 256; CHECK(!HGNativePartyUIResolveTouch(&copy, revision, 1, 0, 0, &x, &y));
    copy = *state; copy.mons[0].hitY = 192; CHECK(!HGNativePartyUIResolveTouch(&copy, revision, 1, 0, 0, &x, &y));
    copy = *state; copy.slotMask = 0; CHECK(!HGNativePartyUIResolveTouch(&copy, revision, 1, 0, 0, &x, &y));
    copy = *state; copy.cancelEnabled = 0; CHECK(!HGNativePartyUIResolveTouch(&copy, revision, 1, 2, 0, &x, &y));
    CHECK(!HGNativePartyUIResolveTouch(state, revision, 1, 2, 1, &x, &y));
    NativePartyUIBind(NULL); CHECK(!state->active && !state->ready && !state->message.pixelBuffer && !state->mons[0].nickname.pixelBuffer && state->revision > revision);
    NativePartyUIBind(&menu); NativePartyUIPublish(&menu, 1, checkRosterHits); CHECK(!HGNativePartyUIResolveTouch(state, revision, 1, 0, 0, &x, &y));
    NativePartyUIBind(NULL);
    for (u32 i = 0; i < NELEMS(checkWindows); ++i) if (checkWindows[i].window) RemoveWindow(checkWindows[i].window);
    for (u32 i = 0; i < 16; ++i) String_Delete(menu.contextMenuStrings[i]);
    CHECK(!checkAllocations && !checkCursorLive && checkPrints > 0 && checkRemovals > 0);
    return 0;
}

#ifdef HG_NATIVE_PARTY_UI_CHECK_MAIN
int main(void) {
    u32 line = CheckOriginalPartyUI();
    if (line) { fprintf(stderr, "Party UI check line %u\n", line); return 1; }
    puts("PASS: original Party context windows, labels, touch guard, revisions, cooperative boundaries and lifetime.");
    return 0;
}
#endif
