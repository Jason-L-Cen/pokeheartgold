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
// New TM waits use the real printer table/query; old Give/Take active-printer boundary stays scoped.
#undef AddTextPrinterParameterizedWithColor
#undef TextPrinterCheckActive
#include "../../../src/text.c"
#include "../../../src/sys_task.c"
#include "../../../src/item.c"
#define AddTextPrinterParameterizedWithColor PartyCheckPrintText
#define TextPrinterCheckActive PartyCheckTextActive

_Static_assert(sizeof(HGNativePartyUIState)==1480, "Public Party packet ABI");
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

    CHECK(HG_PARTY_UI_USE_CONTEXT==PARTY_MENU_CONTEXT_USE_ITEM && HG_PARTY_UI_ITEM_TARGET==PARTY_MENU_STATE_USE_ITEM_SELECT_MON
        && HG_PARTY_UI_ITEM_USE==PARTY_MENU_STATE_ITEM_USE_CB && HG_PARTY_UI_CONFIRM==PARTY_MON_SELECTION_CONFIRM
        && HG_PARTY_UI_SLOTS==PARTY_MON_SELECTION_CANCEL);
    revision=state->revision; NativePartyUIBind(NULL); args.context=HG_PARTY_UI_USE_CONTEXT; args.itemId=ITEM_POTION; menu.partyMonIndex=0;
    NativePartyUIBind(&menu); CHECK(sNativePartyMessage==&menu.windows[PARTY_MENU_WINDOW_ID_32] && !sNativePartyMessageWaiting);
    NativePartyUIPublish(&menu,HG_PARTY_UI_ITEM_TARGET,checkRosterHits);
    CHECK(sizeof(*state)==1480 && state->ready && state->revision>revision && HGNativePartyUIIsPotionUse(state)
        && !HGNativePartyUIIsItemProgress(state) && state->count==2 && state->slotMask==3 && !state->actionCount && !state->actionEnabledMask
        && state->message.pixelBuffer && state->message.tileWidth==20 && state->message.tileHeight==2 && !state->message.engine
        && !state->selectedName.pixelBuffer && !state->selectedItem.pixelBuffer);
    revision=state->revision;
    CHECK(HGNativePartyUIResolveTouch(state,revision,HG_PARTY_UI_ITEM_TARGET,HG_PARTY_UI_TOUCH_MON,0,&x,&y) && x==64 && y==24);
    CHECK(HGNativePartyUIResolveTouch(state,revision,HG_PARTY_UI_ITEM_TARGET,HG_PARTY_UI_TOUCH_CANCEL,0,&x,&y) && x==228 && y==172);
    CHECK(!HGNativePartyUIResolveTouch(state,revision,HG_PARTY_UI_ITEM_TARGET,HG_PARTY_UI_TOUCH_ACTION,0,&x,&y)
        && !HGNativePartyUIResolveTouch(state,revision,HG_PARTY_UI_ITEM_TARGET,HG_PARTY_UI_TOUCH_MON,HG_PARTY_UI_SLOTS,&x,&y)
        && !HGNativePartyUIResolveTouch(state,revision,HG_PARTY_UI_ITEM_TARGET,HG_PARTY_UI_TOUCH_CANCEL,1,&x,&y));
    copy=*state; copy.mons[0].hp=copy.mons[0].maxHp; copy.mons[0].isEgg=TRUE;
    CHECK(HGNativePartyUIResolveTouch(&copy,revision,copy.phase,HG_PARTY_UI_TOUCH_MON,0,&x,&y)); // Original code decides applicability/Egg rejection.
    copy=*state; copy.count=0; CHECK(!HGNativePartyUIIsPotionUse(&copy));
    copy=*state; copy.slotMask=1; CHECK(!HGNativePartyUIIsPotionUse(&copy));
    copy=*state; copy.selected=UINT32_MAX; CHECK(!HGNativePartyUIIsPotionUse(&copy));
    copy=*state; copy.message.tileWidth=27; CHECK(!HGNativePartyUIResolveTouch(&copy,revision,copy.phase,HG_PARTY_UI_TOUCH_MON,0,&x,&y));
    copy=*state; copy.message.engine=1; CHECK(!HGNativePartyUIResolveTouch(&copy,revision,copy.phase,HG_PARTY_UI_TOUCH_CANCEL,0,&x,&y));
    copy=*state; copy.actionCount=1; CHECK(!HGNativePartyUIResolveTouch(&copy,revision,copy.phase,HG_PARTY_UI_TOUCH_MON,0,&x,&y));
    copy=*state; copy.cancelX=124; CHECK(!HGNativePartyUIResolveTouch(&copy,revision,copy.phase,HG_PARTY_UI_TOUCH_CANCEL,0,&x,&y));
    NativePartyUIBegin(&menu); CHECK(HGNativePartyUIIsPotionUse(state) && !state->ready && !state->message.pixelBuffer && !state->mons[0].nickname.pixelBuffer);
    CHECK(!HGNativePartyUIResolveTouch(state,revision,HG_PARTY_UI_ITEM_TARGET,HG_PARTY_UI_TOUCH_MON,0,&x,&y));
    NativePartyUIPublish(&menu,HG_PARTY_UI_ITEM_TARGET,checkRosterHits); CHECK(state->ready && state->revision==revision);
    for (u32 transfer=0;transfer<2;++transfer) for (u32 layer=0;layer<8;++layer) {
        u16 *pending=transfer ? &bg.scrollScheduled : &bg.bufferTransferScheduled;
        *pending=1u<<layer; NativePartyUIBegin(&menu); NativePartyUIPublish(&menu,HG_PARTY_UI_ITEM_TARGET,checkRosterHits);
        CHECK(HGNativePartyUIIsPotionUse(state) && !state->ready && !state->mons[0].nickname.pixelBuffer && !state->mons[0].iconWidth);
        *pending=0; CHECK(!state->ready && !state->mons[0].nickname.pixelBuffer); // VBlank must republish after completing transfers.
        NativePartyUIPublish(&menu,HG_PARTY_UI_ITEM_TARGET,checkRosterHits);
        CHECK(state->ready && state->revision==revision && state->mons[0].nickname.pixelBuffer);
    }
    bad=menu.windows[PARTY_MENU_WINDOW_ID_32]; menu.windows[PARTY_MENU_WINDOW_ID_32].tilemapTop=19;
    NativePartyUIPublish(&menu,HG_PARTY_UI_ITEM_TARGET,checkRosterHits); CHECK(!state->ready);
    menu.windows[PARTY_MENU_WINDOW_ID_32]=bad;
    menu.partyMonIndex=HG_PARTY_UI_CONFIRM; menu.contextMenuButtonAnim.active=TRUE;
    NativePartyUIBegin(&menu); NativePartyUIPublish(&menu,HG_PARTY_UI_ITEM_TARGET,checkRosterHits);
    CHECK(HGNativePartyUIIsPotionUse(state) && !state->ready && state->selected==HG_PARTY_UI_CONFIRM
        && state->mons[0].nickname.pixelBuffer && state->revision>revision
        && !HGNativePartyUIResolveTouch(state,state->revision,state->phase,HG_PARTY_UI_TOUCH_MON,0,&x,&y)
        && !HGNativePartyUIResolveTouch(state,state->revision,state->phase,HG_PARTY_UI_TOUCH_CANCEL,0,&x,&y));
    reads=checkSpriteReads; menu.contextMenuButtonAnim.active=FALSE;
    NativePartyUIPublish(&menu,PARTY_MENU_STATE_BEGIN_EXIT,checkRosterHits);
    CHECK(!state->ready && !HGNativePartyUIIsPotionUse(state) && !state->count && checkSpriteReads==reads);
    NativePartyUIBind(NULL); menu.partyMonIndex=0; NativePartyUIBind(&menu);
    menu.swapMonsData.active=TRUE; NativePartyUIPublish(&menu,HG_PARTY_UI_ITEM_TARGET,checkRosterHits); CHECK(!state->ready && state->mons[0].nickname.pixelBuffer);
    menu.swapMonsData.active=FALSE; checkFade=FALSE; NativePartyUIPublish(&menu,HG_PARTY_UI_ITEM_TARGET,checkRosterHits);
    CHECK(!state->ready && !state->mons[0].nickname.pixelBuffer && state->message.pixelBuffer);
    checkFade=TRUE; NativePartyUIPublish(&menu,HG_PARTY_UI_ITEM_TARGET,checkRosterHits); CHECK(state->ready);
    u32 targetRevision=state->revision;
    NativePartyUIBegin(&menu); NativePartyUIPublish(&menu,HG_PARTY_UI_ITEM_USE,checkRosterHits);
    CHECK(HGNativePartyUIIsPotionUse(state) && HGNativePartyUIIsItemProgress(state) && !state->ready && !state->cancelEnabled && state->revision>targetRevision);
    revision=state->revision; menu.monsDrawState[0].hp=19; bg.bufferTransferScheduled=1u<<GF_BG_LYR_MAIN_1;
    NativePartyUIBegin(&menu); NativePartyUIPublish(&menu,HG_PARTY_UI_ITEM_USE,checkRosterHits);
    CHECK(state->mons[0].hp==19 && state->revision>revision && !state->ready && !state->mons[0].nickname.pixelBuffer);
    bg.bufferTransferScheduled=0; CHECK(!state->mons[0].nickname.pixelBuffer);
    NativePartyUIPublish(&menu,HG_PARTY_UI_ITEM_USE,checkRosterHits);
    CHECK(state->mons[0].hp==19 && state->mons[0].nickname.pixelBuffer && !state->ready && !state->cancelEnabled);
    NativePartyUISetMessage(&menu,&menu.windows[PARTY_MENU_WINDOW_ID_34]); printer.state=0; NativePartyUIPrinterUpdated(&printer.template,0);
    NativePartyUIPublish(&menu,HG_PARTY_UI_ITEM_USE,checkRosterHits); CHECK(!state->ready && state->message.tileWidth==27);
    revision=state->revision; printer.state=2; NativePartyUIPrinterUpdated(&printer.template,0); NativePartyUIPublish(&menu,HG_PARTY_UI_ITEM_USE,checkRosterHits);
    CHECK(state->ready && state->cancelEnabled && state->revision>revision
        && HGNativePartyUIResolveTouch(state,state->revision,state->phase,HG_PARTY_UI_TOUCH_CANCEL,0,&x,&y) && x==124 && y==168
        && !HGNativePartyUIResolveTouch(state,state->revision,state->phase,HG_PARTY_UI_TOUCH_MON,0,&x,&y)
        && !HGNativePartyUIResolveTouch(state,state->revision,state->phase,HG_PARTY_UI_TOUCH_ACTION,0,&x,&y)
        && !HGNativePartyUIResolveTouch(state,targetRevision,HG_PARTY_UI_ITEM_TARGET,HG_PARTY_UI_TOUCH_MON,0,&x,&y));
    for (u32 transfer=0;transfer<2;++transfer) for (u32 layer=0;layer<8;++layer) {
        u16 *pending=transfer ? &bg.scrollScheduled : &bg.bufferTransferScheduled;
        *pending=1u<<layer; NativePartyUIBegin(&menu); NativePartyUIPublish(&menu,HG_PARTY_UI_ITEM_USE,checkRosterHits);
        CHECK(!state->ready && !state->cancelEnabled && !state->mons[0].nickname.pixelBuffer && state->message.pixelBuffer);
        *pending=0; NativePartyUIPublish(&menu,HG_PARTY_UI_ITEM_USE,checkRosterHits);
        CHECK(state->ready && state->mons[0].nickname.pixelBuffer);
    }
    menu.partyMonIndex=HG_PARTY_UI_CONFIRM; NativePartyUISetMessage(&menu,&menu.windows[PARTY_MENU_WINDOW_ID_34]);
    NativePartyUIPublish(&menu,HG_PARTY_UI_ITEM_USE,checkRosterHits); CHECK(HGNativePartyUIIsItemProgress(state) && !state->ready && state->count==2);
    NativePartyUIPrinterUpdated(&printer.template,0); NativePartyUIPublish(&menu,HG_PARTY_UI_ITEM_USE,checkRosterHits);
    CHECK(state->ready && state->selected==HG_PARTY_UI_CONFIRM
        && HGNativePartyUIResolveTouch(state,state->revision,state->phase,HG_PARTY_UI_TOUCH_CANCEL,0,&x,&y)
        && !HGNativePartyUIResolveTouch(state,state->revision,state->phase,HG_PARTY_UI_TOUCH_MON,0,&x,&y));
    revision=state->revision; printer.state=0; NativePartyUIPrinterUpdated(&printer.template,0); NativePartyUIPublish(&menu,HG_PARTY_UI_ITEM_USE,checkRosterHits);
    CHECK(!state->ready && !state->cancelEnabled && state->revision>revision);
    menu.partyMonIndex=HG_PARTY_UI_SLOTS; NativePartyUIPublish(&menu,HG_PARTY_UI_ITEM_USE,checkRosterHits);
    CHECK(!HGNativePartyUIIsPotionUse(state) && !state->count && !state->ready);
    menu.partyMonIndex=0; reads=checkSpriteReads; args.itemId=ITEM_ANTIDOTE;
    NativePartyUIBind(NULL); NativePartyUIBind(&menu); NativePartyUIPublish(&menu,HG_PARTY_UI_ITEM_TARGET,checkRosterHits);
    CHECK(!state->ready && !state->count && !state->message.pixelBuffer && checkSpriteReads==reads);
    args.itemId=ITEM_POTION; args.panelLayout=1; NativePartyUIPublish(&menu,HG_PARTY_UI_ITEM_TARGET,checkRosterHits);
    CHECK(!state->ready && !state->count && checkSpriteReads==reads); args.panelLayout=0;
    NativePartyUIBind(NULL); NativePartyUIBind(&menu);
    const u32 unsupportedPotionPhases[]={PARTY_MENU_STATE_INIT,PARTY_MENU_STATE_3,PARTY_MENU_STATE_SELECT_MOVE,PARTY_MENU_STATE_SACRED_ASH,
        PARTY_MENU_STATE_GIVE_ITEM_SELECT_MON,PARTY_MENU_STATE_HANDLE_SUBCONTEXT_MENU_INPUT,PARTY_MENU_STATE_BEGIN_EXIT,PARTY_MENU_STATE_WAIT_EXIT_FADE_OUT};
    for (u32 i=0;i<NELEMS(unsupportedPotionPhases);++i) {
        NativePartyUIPublish(&menu,unsupportedPotionPhases[i],checkRosterHits);
        CHECK(!HGNativePartyUIIsPotionUse(state) && !state->ready && !state->count && checkSpriteReads==reads);
    }
    NativePartyUIPublish(&menu,HG_PARTY_UI_ITEM_TARGET,checkRosterHits); CHECK(state->ready && !sNativePartyMessageWaiting);
    // Reuse the existing real windows and original TM mapping; never dispatch item-use callbacks.
    args.context=PARTY_MENU_CONTEXT_TM_HM; args.itemId=ITEM_TM17; args.moveId=TMHMGetMove(ITEM_TM17);
    menu.partyMonIndex=0; menu.cancelDisabled=FALSE; NativePartyUIBind(NULL); NativePartyUIBind(&menu);
    NativePartyUIPublish(&menu,HG_PARTY_UI_TM_TARGET,checkRosterHits);
    CHECK(sizeof(*state)==1480 && HGNativePartyUIIsTMHM(state) && state->ready && state->actionCount==1
        && state->actions[0].textId==HG_PARTY_UI_TEXT_TM_TARGET && !state->actionEnabledMask && state->message.tileWidth==20 && state->message.tileHeight==2);
    CHECK(HGNativePartyUIResolveTouch(state,state->revision,state->phase,HG_PARTY_UI_TOUCH_MON,0,&x,&y) && x==64 && y==24);
    CHECK(HGNativePartyUIResolveTouch(state,state->revision,state->phase,HG_PARTY_UI_TOUCH_CANCEL,0,&x,&y) && x==228 && y==172
        && !HGNativePartyUIResolveTouch(state,state->revision,state->phase,HG_PARTY_UI_TOUCH_ACTION,0,&x,&y));
    revision=state->revision; menu.partyMonIndex=HG_PARTY_UI_CONFIRM; NativePartyUIPublish(&menu,HG_PARTY_UI_TM_TARGET,checkRosterHits);
    CHECK(HGNativePartyUIIsTMHM(state) && !state->ready && state->revision>revision);
    menu.partyMonIndex=HG_PARTY_UI_SLOTS; NativePartyUIPublish(&menu,HG_PARTY_UI_TM_TARGET,checkRosterHits);
    CHECK(state->ready && HGNativePartyUIResolveTouch(state,state->revision,state->phase,HG_PARTY_UI_TOUCH_CANCEL,0,&x,&y));
    menu.partyMonIndex=0; menu.cancelDisabled=TRUE; NativePartyUIPublish(&menu,HG_PARTY_UI_TM_TARGET,checkRosterHits);
    CHECK(state->ready && !HGNativePartyUIResolveTouch(state,state->revision,state->phase,HG_PARTY_UI_TOUCH_CANCEL,0,&x,&y)); menu.cancelDisabled=FALSE;
    SysTask tmTask={.data=&printer}; sTextPrinterTasks[3]=&tmTask;
    printer=(TextPrinter){.template={.window=&menu.windows[PARTY_MENU_WINDOW_ID_34]},.active=TRUE,.id=3,.state=2};
    menu.textPrinterId=3; NativePartyUISetMessage(&menu,&menu.windows[PARTY_MENU_WINDOW_ID_34]);
    NativePartyUIPublish(&menu,HG_PARTY_UI_TM_MESSAGE,checkRosterHits); revision=state->revision;
    CHECK(HGNativePartyUIIsTMHM(state) && state->ready && state->actionEnabledMask==1 && !state->cancelEnabled
        && HGNativePartyUIResolveTouch(state,revision,state->phase,HG_PARTY_UI_TOUCH_ACTION,0,&x,&y) && x==124 && y==168);
    CHECK(!HGNativePartyUIResolveTouch(state,revision,state->phase,HG_PARTY_UI_TOUCH_MON,0,&x,&y)
        && !HGNativePartyUIResolveTouch(state,revision,state->phase,HG_PARTY_UI_TOUCH_CANCEL,0,&x,&y));
    printer.callbackParam=1; NativePartyUIPublish(&menu,HG_PARTY_UI_TM_MESSAGE,checkRosterHits);
    CHECK(!state->ready && state->revision>revision && !HGNativePartyUIResolveTouch(state,revision,HG_PARTY_UI_TM_MESSAGE,HG_PARTY_UI_TOUCH_ACTION,0,&x,&y));
    printer.callbackParam=0; printer.template.window=&menu.windows[PARTY_MENU_WINDOW_ID_32]; NativePartyUIPublish(&menu,HG_PARTY_UI_TM_MESSAGE,checkRosterHits); CHECK(!state->ready);
    printer.template.window=&menu.windows[PARTY_MENU_WINDOW_ID_34];
    for (u32 transfer=0;transfer<2;++transfer) for (u32 layer=0;layer<8;++layer) {
        u16 *pending=transfer ? &bg.scrollScheduled : &bg.bufferTransferScheduled;
        *pending=1u<<layer; NativePartyUIBegin(&menu); NativePartyUIPublish(&menu,HG_PARTY_UI_TM_MESSAGE,checkRosterHits);
        CHECK(HGNativePartyUIIsTMHM(state) && !state->ready && !state->actionEnabledMask && !state->mons[0].nickname.pixelBuffer && state->message.pixelBuffer);
        *pending=0; CHECK(!state->ready); NativePartyUIPublish(&menu,HG_PARTY_UI_TM_MESSAGE,checkRosterHits); CHECK(state->ready && state->mons[0].nickname.pixelBuffer);
    }
    YesNoPrompt tmYesNo={.bgConfig=&bg,.bgId=GF_BG_LYR_MAIN_0,.x=25,.y=10,.width=6,.height=4,
        .buttonsAreInit=1,.touchHitboxController=(TouchHitboxController *)(uintptr_t)1,.result=3};
    for (u32 i=0;i<2;++i) {
        tmYesNo.hitboxes[i]=(TouchscreenHitbox){.rect={80+32*i,112+32*i,200,248}};
        tmYesNo.buttons[i].bgConfig=&bg; tmYesNo.buttons[i].bgId=GF_BG_LYR_MAIN_0;
    }
    menu.yesNoPrompt=&tmYesNo;
    for (u32 context=PARTY_MENU_CONTEXT_TM_HM;context<=PARTY_MENU_CONTEXT_REPLACE_MOVE_TMHM;++context) {
        args.context=context; NativePartyUIBind(NULL); NativePartyUIBind(&menu); NativePartyUISetMessage(&menu,&menu.windows[PARTY_MENU_WINDOW_ID_34]);
        NativePartyUIPublish(&menu,HG_PARTY_UI_TM_YES_NO,checkRosterHits);
        CHECK(HGNativePartyUIIsTMHM(state) && state->ready && state->actionCount==2 && state->actionEnabledMask==3
            && !state->actions[0].text.pixelBuffer && !state->actions[1].text.pixelBuffer);
        for (u32 i=0;i<2;++i) CHECK(HGNativePartyUIResolveTouch(state,state->revision,state->phase,HG_PARTY_UI_TOUCH_ACTION,i,&x,&y) && x==224 && y==96+32*i);
        revision=state->revision; tmYesNo.confirmSelectionTimer=1; NativePartyUIPublish(&menu,HG_PARTY_UI_TM_YES_NO,checkRosterHits);
        CHECK(!state->ready && !state->actionEnabledMask && state->revision>revision); tmYesNo.confirmSelectionTimer=0;
        tmYesNo.ignoreTouch=TRUE; NativePartyUIPublish(&menu,HG_PARTY_UI_TM_YES_NO,checkRosterHits); CHECK(!state->ready); tmYesNo.ignoreTouch=FALSE;
        tmYesNo.buttons[1].bgId=GF_BG_LYR_SUB_0; NativePartyUIPublish(&menu,HG_PARTY_UI_TM_YES_NO,checkRosterHits); CHECK(!state->ready); tmYesNo.buttons[1].bgId=GF_BG_LYR_MAIN_0;
        const u32 tmBusy[]={0,3,22,25,26,32,33};
        for (u32 i=0;i<NELEMS(tmBusy);++i) {
            NativePartyUIPublish(&menu,tmBusy[i],checkRosterHits);
            CHECK(HGNativePartyUIIsTMHM(state) && !state->ready && state->actionCount==1 && !state->actionEnabledMask);
            copy=*state; copy.ready=TRUE; copy.actionEnabledMask=1; copy.actions[0].enabled=TRUE;
            CHECK(!HGNativePartyUIResolveTouch(&copy,copy.revision,copy.phase,HG_PARTY_UI_TOUCH_ACTION,0,&x,&y));
        }
        NativePartyUIPublish(&menu,HG_PARTY_UI_TM_MESSAGE,checkRosterHits); CHECK(state->ready);
    }
    checkFade=FALSE; NativePartyUIPublish(&menu,HG_PARTY_UI_TM_MESSAGE,checkRosterHits); CHECK(!state->ready && !state->mons[0].nickname.pixelBuffer); checkFade=TRUE;
    args.context=PARTY_MENU_CONTEXT_REPLACE_MOVE_LEVELUP; NativePartyUIPublish(&menu,HG_PARTY_UI_TM_MESSAGE,checkRosterHits);
    CHECK(!HGNativePartyUIIsTMHM(state) && !state->ready && !state->count);
    args.context=PARTY_MENU_CONTEXT_TM_HM; args.moveId=MOVE_TACKLE; NativePartyUIPublish(&menu,HG_PARTY_UI_TM_TARGET,checkRosterHits);
    CHECK(!HGNativePartyUIIsTMHM(state) && !state->ready && !state->count);
    args.moveId=TMHMGetMove(args.itemId); args.panelLayout=1; NativePartyUIPublish(&menu,HG_PARTY_UI_TM_TARGET,checkRosterHits);
    CHECK(!HGNativePartyUIIsTMHM(state) && !state->ready && !state->count); args.panelLayout=0;
    revision=state->revision; NativePartyUIBind(NULL); CHECK(!HGNativePartyUIIsTMHM(state) && !state->message.pixelBuffer);
    CHECK(!HGNativePartyUIResolveTouch(state,revision,HG_PARTY_UI_TM_MESSAGE,HG_PARTY_UI_TOUCH_ACTION,0,&x,&y));
    sTextPrinterTasks[3]=NULL; menu.yesNoPrompt=NULL; menu.partyMonIndex=0; args.moveId=MOVE_NONE;
    NativePartyUIBind(&menu);
    // Candy bridge metadata only: no original item-use/learning callback or SaveData is invoked.
    args.context=PARTY_MENU_CONTEXT_USE_ITEM; args.itemId=ITEM_RARE_CANDY; args.moveId=MOVE_NONE;
    menu.partyMonIndex=0; menu.levelUpLearnMovesLoopState=0; NativePartyUIBind(NULL); NativePartyUIBind(&menu);
    CHECK(HGNativePartyUIIsRareCandy(state) && !state->ready && state->actions[0].textId==HG_PARTY_UI_TEXT_CANDY_BUSY);
    NativePartyUIPublish(&menu,HG_PARTY_UI_ITEM_TARGET,checkRosterHits); revision=state->revision;
    CHECK(state->ready && HGNativePartyUIIsRareCandy(state) && !HGNativePartyUIIsPotionUse(state) && !HGNativePartyUIIsItemProgress(state)
        && state->actions[0].textId==HG_PARTY_UI_TEXT_CANDY_TARGET && !state->actionEnabledMask
        && HGNativePartyUIResolveTouch(state,revision,state->phase,HG_PARTY_UI_TOUCH_MON,0,&x,&y) && x==64 && y==24
        && HGNativePartyUIResolveTouch(state,revision,state->phase,HG_PARTY_UI_TOUCH_CANCEL,0,&x,&y) && x==228 && y==172
        && !HGNativePartyUIResolveTouch(state,revision,state->phase,HG_PARTY_UI_TOUCH_ACTION,0,&x,&y));
    copy=*state; copy.count=1; CHECK(!HGNativePartyUIIsRareCandy(&copy) && !HGNativePartyUIResolveTouch(&copy,revision,copy.phase,HG_PARTY_UI_TOUCH_MON,0,&x,&y));
    copy=*state; copy.slotMask^=2; CHECK(!HGNativePartyUIIsRareCandy(&copy) && !HGNativePartyUIResolveTouch(&copy,revision,copy.phase,HG_PARTY_UI_TOUCH_MON,0,&x,&y));
    copy=*state; copy.mons[1].active=0; CHECK(!HGNativePartyUIIsRareCandy(&copy) && !HGNativePartyUIResolveTouch(&copy,revision,copy.phase,HG_PARTY_UI_TOUCH_MON,0,&x,&y));
    copy=*state; copy.slotMask|=1u<<HG_PARTY_UI_SLOTS; CHECK(!HGNativePartyUIIsRareCandy(&copy));
    copy=*state; copy.context=PARTY_MENU_CONTEXT_REPLACE_MOVE_LEVELUP; CHECK(!HGNativePartyUIResolveTouch(&copy,revision,copy.phase,HG_PARTY_UI_TOUCH_MON,0,&x,&y));
    menu.partyMonIndex=HG_PARTY_UI_CONFIRM; NativePartyUIPublish(&menu,HG_PARTY_UI_ITEM_TARGET,checkRosterHits); CHECK(!state->ready);
    menu.partyMonIndex=0; NativePartyUIPublish(&menu,HG_PARTY_UI_ITEM_USE,checkRosterHits);
    CHECK(!state->ready && state->actions[0].textId==HG_PARTY_UI_TEXT_CANDY_BUSY);
    printer=(TextPrinter){.template={.window=&menu.windows[PARTY_MENU_WINDOW_ID_34]},.active=TRUE,.id=3,.state=2};
    SysTask candyTask={.data=&printer}; sTextPrinterTasks[3]=&candyTask; menu.textPrinterId=3;
    NativePartyUISetMessage(&menu,&menu.windows[PARTY_MENU_WINDOW_ID_34]); NativePartyUIPublish(&menu,HG_PARTY_UI_ITEM_USE,checkRosterHits);
    CHECK(state->ready && state->actions[0].textId==HG_PARTY_UI_TEXT_CANDY_ACK && state->actionEnabledMask==1 && !state->cancelEnabled
        && HGNativePartyUIResolveTouch(state,state->revision,state->phase,HG_PARTY_UI_TOUCH_ACTION,0,&x,&y) && x==124 && y==168
        && !HGNativePartyUIResolveTouch(state,revision,HG_PARTY_UI_ITEM_TARGET,HG_PARTY_UI_TOUCH_MON,0,&x,&y)
        && !HGNativePartyUIResolveTouch(state,state->revision,state->phase,HG_PARTY_UI_TOUCH_CANCEL,0,&x,&y));
    printer.callbackResult=1; NativePartyUIPublish(&menu,HG_PARTY_UI_ITEM_USE,checkRosterHits); CHECK(!state->ready); printer.callbackResult=0;
    printer.callbackParam=1; NativePartyUIPublish(&menu,HG_PARTY_UI_ITEM_USE,checkRosterHits); CHECK(!state->ready); printer.callbackParam=0;
    printer.state=0; NativePartyUIPublish(&menu,HG_PARTY_UI_ITEM_USE,checkRosterHits); CHECK(!state->ready); printer.state=2;
    NativePartyUIPublish(&menu,HG_PARTY_UI_ITEM_USE,checkRosterHits); revision=state->revision;
    copy=*state; copy.message.tileWidth=20; CHECK(!HGNativePartyUIResolveTouch(&copy,revision,copy.phase,HG_PARTY_UI_TOUCH_ACTION,0,&x,&y));
    copy=*state; copy.message.tileHeight=12; CHECK(!HGNativePartyUIResolveTouch(&copy,revision,copy.phase,HG_PARTY_UI_TOUCH_ACTION,0,&x,&y));
    copy=*state; copy.actions[0].hitY++; CHECK(!HGNativePartyUIResolveTouch(&copy,revision,copy.phase,HG_PARTY_UI_TOUCH_ACTION,0,&x,&y));
    copy=*state; copy.actionCount=2; CHECK(!HGNativePartyUIIsRareCandy(&copy) && !HGNativePartyUIResolveTouch(&copy,revision,copy.phase,HG_PARTY_UI_TOUCH_ACTION,0,&x,&y));
    const WindowTemplate candyStats={.bgId=GF_BG_LYR_MAIN_0,.left=1,.top=1,.width=14,.height=12,.palette=0,.baseTile=0x260};
    AddWindow(&bg,&menu.levelUpStatsWindow[0],&candyStats); sTextPrinterTasks[3]=NULL;
    // Owned synthetic numbers check only the real cache formula/getter lifetime, not a level-up business outcome.
    const u16 statsBefore[6]={20,10,11,12,13,14},statsAfter[6]={22,11,13,15,17,19},statsDiff[6]={2,1,2,3,4,5};
    memcpy(menu.levelUpStatsTmp,statsBefore,sizeof(statsBefore));
    CHECK(HGNativePartyUIGetStats(state->revision)==NULL);
    NativePartyUICacheStats(&menu,HG_PARTY_UI_TEXT_CANDY_STATS_DIFF,statsAfter);
    CHECK(!memcmp(menu.levelUpStatsTmp,statsBefore,sizeof(statsBefore)));
    menu.levelUpLearnMovesLoopState=1; NativePartyUIPublish(&menu,HG_PARTY_UI_ITEM_USE,checkRosterHits); revision=state->revision;
    CHECK(state->ready && state->actions[0].textId==HG_PARTY_UI_TEXT_CANDY_STATS_DIFF && !state->actions[0].text.pixelBuffer
        && state->message.tileWidth==27 && state->message.tileHeight==4
        && HGNativePartyUIResolveTouch(state,revision,state->phase,HG_PARTY_UI_TOUCH_ACTION,0,&x,&y));
    const u16 *displayed=HGNativePartyUIGetStats(revision);
    CHECK(displayed && sizeof(statsDiff)==12 && !memcmp(displayed,statsDiff,sizeof(statsDiff)) && HGNativePartyUIGetStats(revision-1)==NULL);
    NativePartyUIBegin(&menu); CHECK(HGNativePartyUIGetStats(revision)==NULL && !memcmp(sNativePartyStats,statsDiff,sizeof(statsDiff)));
    NativePartyUIPublish(&menu,HG_PARTY_UI_ITEM_USE,checkRosterHits); CHECK(HGNativePartyUIGetStats(revision)==displayed);
    NativePartyUIBegin(&menu); memcpy(menu.levelUpStatsTmp,statsAfter,sizeof(statsAfter));
    NativePartyUICacheStats(&menu,HG_PARTY_UI_TEXT_CANDY_STATS_TOTAL,menu.levelUpStatsTmp);
    menu.levelUpLearnMovesLoopState=2; NativePartyUIPublish(&menu,HG_PARTY_UI_ITEM_USE,checkRosterHits);
    CHECK(state->ready && state->actions[0].textId==HG_PARTY_UI_TEXT_CANDY_STATS_TOTAL && state->revision>revision
        && !HGNativePartyUIResolveTouch(state,revision,state->phase,HG_PARTY_UI_TOUCH_ACTION,0,&x,&y));
    CHECK(HGNativePartyUIGetStats(revision)==NULL && HGNativePartyUIGetStats(state->revision)
        && !memcmp(HGNativePartyUIGetStats(state->revision),statsAfter,sizeof(statsAfter)));
    menu.levelUpLearnMovesLoopState=1; CHECK(HGNativePartyUIGetStats(state->revision)==NULL); menu.levelUpLearnMovesLoopState=2;
    checkFade=FALSE; CHECK(HGNativePartyUIGetStats(state->revision)==NULL); checkFade=TRUE;
    for (u32 transfer=0;transfer<2;++transfer) for (u32 layer=0;layer<8;++layer) {
        u16 *pending=transfer ? &bg.scrollScheduled : &bg.bufferTransferScheduled; *pending=1u<<layer;
        CHECK(HGNativePartyUIGetStats(state->revision)==NULL);
        NativePartyUIBegin(&menu); NativePartyUIPublish(&menu,HG_PARTY_UI_ITEM_USE,checkRosterHits);
        CHECK(!state->ready && !state->actionEnabledMask && !state->mons[0].nickname.pixelBuffer);
        *pending=0; CHECK(!state->ready && HGNativePartyUIGetStats(state->revision)==NULL);
        NativePartyUIPublish(&menu,HG_PARTY_UI_ITEM_USE,checkRosterHits);
        CHECK(state->ready && HGNativePartyUIGetStats(state->revision) && !memcmp(HGNativePartyUIGetStats(state->revision),statsAfter,sizeof(statsAfter)));
    }
    menu.levelUpStatsWindow[0].height=11; CHECK(HGNativePartyUIGetStats(state->revision)==NULL);
    NativePartyUIPublish(&menu,HG_PARTY_UI_ITEM_USE,checkRosterHits); CHECK(!state->ready); menu.levelUpStatsWindow[0].height=12;
    NativePartyUIPublish(&menu,HG_PARTY_UI_ITEM_USE,checkRosterHits); revision=state->revision;
    PartyMenu foreignStats=menu; NativePartyUICacheStats(&foreignStats,HG_PARTY_UI_TEXT_CANDY_STATS_DIFF,statsAfter);
    CHECK(HGNativePartyUIGetStats(revision) && !memcmp(HGNativePartyUIGetStats(revision),statsAfter,sizeof(statsAfter)));
    NativePartyUIBind(&foreignStats); CHECK(HGNativePartyUIGetStats(revision)==NULL && !sNativePartyStatsRole);
    NativePartyUIBind(&menu); CHECK(HGNativePartyUIGetStats(state->revision)==NULL);
    NativePartyUISetMessage(&menu,&menu.windows[PARTY_MENU_WINDOW_ID_34]);
    NativePartyUICacheStats(&menu,HG_PARTY_UI_TEXT_CANDY_STATS_TOTAL,menu.levelUpStatsTmp); NativePartyUIPublish(&menu,HG_PARTY_UI_ITEM_USE,checkRosterHits);
    CHECK(HGNativePartyUIGetStats(state->revision)!=NULL);
    NativePartyUIInvalidateStats(&menu); CHECK(HGNativePartyUIGetStats(state->revision)==NULL && !sNativePartyStatsRole);
    NativePartyUIBegin(&menu); RemoveWindow(&menu.levelUpStatsWindow[0]);
    const u32 candyBusy[]={3,5,6};
    for (u32 i=0;i<NELEMS(candyBusy);++i) {
        menu.levelUpLearnMovesLoopState=candyBusy[i]; NativePartyUIPublish(&menu,HG_PARTY_UI_ITEM_USE,checkRosterHits);
        CHECK(HGNativePartyUIIsRareCandy(state) && !state->ready && state->actions[0].textId==HG_PARTY_UI_TEXT_CANDY_BUSY);
        copy=*state; copy.ready=TRUE; copy.actionEnabledMask=1; copy.actions[0].enabled=TRUE;
        CHECK(!HGNativePartyUIResolveTouch(&copy,copy.revision,copy.phase,HG_PARTY_UI_TOUCH_ACTION,0,&x,&y));
    }
    CHECK(HGNativePartyUIGetStats(state->revision)==NULL);
    menu.partyMonIndex=HG_PARTY_UI_CONFIRM; menu.levelUpLearnMovesLoopState=0; sTextPrinterTasks[3]=&candyTask;
    NativePartyUIPublish(&menu,HG_PARTY_UI_ITEM_USE,checkRosterHits); revision=state->revision;
    CHECK(state->ready && state->selected==HG_PARTY_UI_CONFIRM && state->actions[0].textId==HG_PARTY_UI_TEXT_CANDY_ACK
        && HGNativePartyUIResolveTouch(state,revision,state->phase,HG_PARTY_UI_TOUCH_ACTION,0,&x,&y)
        && !HGNativePartyUIResolveTouch(state,revision,state->phase,HG_PARTY_UI_TOUCH_MON,0,&x,&y));
    copy=*state; copy.context=PARTY_MENU_CONTEXT_REPLACE_MOVE_LEVELUP; CHECK(!HGNativePartyUIResolveTouch(&copy,revision,copy.phase,HG_PARTY_UI_TOUCH_ACTION,0,&x,&y));
    args.context=PARTY_MENU_CONTEXT_REPLACE_MOVE_LEVELUP; args.itemId=ITEM_RARE_CANDY; args.moveId=MOVE_RAZOR_LEAF;
    NativePartyUIPublish(&menu,HG_PARTY_UI_ITEM_USE,checkRosterHits); CHECK(!HGNativePartyUIIsRareCandy(state) && !state->ready && !state->count);
    args.itemId=ITEM_NONE; menu.partyMonIndex=0; menu.levelUpLearnMovesLoopState=4; NativePartyUIBind(NULL); NativePartyUIBind(&menu);
    NativePartyUISetMessage(&menu,&menu.windows[PARTY_MENU_WINDOW_ID_34]); NativePartyUIPublish(&menu,HG_PARTY_UI_ITEM_USE,checkRosterHits);
    CHECK(state->ready && HGNativePartyUIIsRareCandy(state) && !HGNativePartyUIIsPotionUse(state)
        && !HGNativePartyUIResolveTouch(state,revision,state->phase,HG_PARTY_UI_TOUCH_ACTION,0,&x,&y));
    NativePartyUIPublish(&menu,HG_PARTY_UI_TM_MESSAGE,checkRosterHits);
    CHECK(state->ready && state->actions[0].textId==HG_PARTY_UI_TEXT_CANDY_ACK
        && HGNativePartyUIResolveTouch(state,state->revision,state->phase,HG_PARTY_UI_TOUCH_ACTION,0,&x,&y));
    const u32 candyBusyPhases[]={0,3,25,26,32,33};
    for (u32 i=0;i<NELEMS(candyBusyPhases);++i) {
        NativePartyUIPublish(&menu,candyBusyPhases[i],checkRosterHits);
        CHECK(HGNativePartyUIIsRareCandy(state) && !state->ready && state->actions[0].textId==HG_PARTY_UI_TEXT_CANDY_BUSY);
        copy=*state; copy.ready=TRUE; copy.actionEnabledMask=1; copy.actions[0].enabled=TRUE;
        CHECK(!HGNativePartyUIResolveTouch(&copy,copy.revision,copy.phase,HG_PARTY_UI_TOUCH_ACTION,0,&x,&y));
    }
    menu.yesNoPrompt=&tmYesNo; NativePartyUIPublish(&menu,HG_PARTY_UI_TM_YES_NO,checkRosterHits); revision=state->revision;
    CHECK(state->ready && state->actions[0].textId==HG_PARTY_UI_TEXT_CANDY_YES && state->actions[1].textId==HG_PARTY_UI_TEXT_CANDY_NO);
    for (u32 i=0;i<2;++i) CHECK(HGNativePartyUIResolveTouch(state,revision,state->phase,HG_PARTY_UI_TOUCH_ACTION,i,&x,&y) && x==224 && y==96+32*i);
    copy=*state; copy.actionEnabledMask=1; CHECK(!HGNativePartyUIResolveTouch(&copy,revision,copy.phase,HG_PARTY_UI_TOUCH_ACTION,0,&x,&y));
    copy=*state; copy.actions[1].hitY--; CHECK(!HGNativePartyUIResolveTouch(&copy,revision,copy.phase,HG_PARTY_UI_TOUCH_ACTION,1,&x,&y));
    args.moveId=MOVE_NONE; NativePartyUIPublish(&menu,HG_PARTY_UI_TM_MESSAGE,checkRosterHits); CHECK(!HGNativePartyUIIsRareCandy(state) && !state->ready);
    args.context=PARTY_MENU_CONTEXT_USE_ITEM; args.itemId=ITEM_POTION; menu.levelUpLearnMovesLoopState=0; args.moveId=MOVE_NONE;
    NativePartyUIBind(NULL); NativePartyUIBind(&menu); NativePartyUIPublish(&menu,HG_PARTY_UI_ITEM_TARGET,checkRosterHits); revision=state->revision;
    CHECK(state->ready && HGNativePartyUIIsPotionUse(state) && !HGNativePartyUIIsRareCandy(state) && !state->actionCount);
    args.itemId=ITEM_RARE_CANDY; NativePartyUIPublish(&menu,HG_PARTY_UI_ITEM_TARGET,checkRosterHits);
    CHECK(HGNativePartyUIIsRareCandy(state) && !HGNativePartyUIIsPotionUse(state) && state->revision>revision
        && !HGNativePartyUIResolveTouch(state,revision,state->phase,HG_PARTY_UI_TOUCH_MON,0,&x,&y));
    sTextPrinterTasks[3]=NULL; menu.yesNoPrompt=NULL;
    args.context=PARTY_MENU_CONTEXT_0; args.itemId=ITEM_NONE; menu.monsDrawState[0].hp=18;
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
