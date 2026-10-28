// SPDX-License-Identifier: GPL-3.0-or-later
// Metadata/query/input component only: no Evolution task, drawing, Pokemon mutation or gameplay is executed.
#include "global.h"
#include <stdio.h>

static void EvolutionCheckClear(void *destination, u32 size);
static void EvolutionCheckCopy16(const void *source, void *destination, u32 size);
static u16 *EvolutionCheckMainPalette(void);
static u16 *EvolutionCheckSubPalette(void);
#define MI_CpuClear8 EvolutionCheckClear
#define MI_CpuCopy16 EvolutionCheckCopy16
#define GetMainBgPlttAddr EvolutionCheckMainPalette
#define GetSubBgPlttAddr EvolutionCheckSubPalette
#include "../../../src/evolution_scene.c"
#include "../../../src/text.c"
#include "../../../src/render_text.c"
#include "../../../src/sys_task.c"
#ifndef HEARTGOLD_ABI32
#include "../../../src/palette.c"
#endif

_Static_assert(sizeof(HGNativeEvolutionUIState) == 196 && offsetof(HGNativeEvolutionUIState, message) == 40
    && offsetof(HGNativeEvolutionUIState, choices) == 92, "Evolution packet ABI");
#define CHECK(value) do { if (!(value)) return __LINE__; } while (0)
static u16 checkPalette[2][256];
static void EvolutionCheckClear(void *destination, u32 size) { memset(destination, 0, size); }
static void EvolutionCheckCopy16(const void *source, void *destination, u32 size) { memcpy(destination, source, size); }
static u16 *EvolutionCheckMainPalette(void) { return checkPalette[0]; }
static u16 *EvolutionCheckSubPalette(void) { return checkPalette[1]; }

static BOOL EvolutionCheckReject(const HGNativeEvolutionUIState *state, u32 revision, u32 phase, u32 kind, u32 index) {
    u32 keys = 0xAA, x = 0xBB, y = 0xCC;
    return !HGNativeEvolutionUIResolveInput(state, revision, phase, kind, index, &keys, &x, &y)
        && keys == 0xAA && x == 0xBB && y == 0xCC;
}

u32 CheckOriginalEvolutionUI(void) {
    EvolutionTaskData data = {0}, foreign = {0};
    BgConfig bg = {0}, foreignBg = {0}; PaletteData palette = {0};
    Window message = {.bgConfig = &bg, .bgId = GF_BG_LYR_MAIN_1, .tilemapLeft = 2, .tilemapTop = 19,
        .width = 27, .height = 4, .paletteNum = 11, .baseTile = 31, .pixelBuffer = (void *)(uintptr_t)0x10000};
    ManagedSprite cursor = {0}; OverlayManager child = {0};
    // As in Party UI checks, low addresses are guest-offset tokens, never host-dereferenced or claimed as rendered glyphs.
    data.bgConfig = &bg; data.paletteData = &palette; data.window = &message; data.species = 152; data.newSpecies = 153;
    data.flags = EVO_FLAG_CAN_CANCEL; data.state = EVO_STATE_OSCILLATE; data.newMove = MOVE_RAZOR_LEAF;
    data.spritesReady = TRUE; data.cursorSprite = &cursor; data.menuResult = EVO_MENU_TOP;
    for (u32 i = 0; i < 2; ++i) data.menuWindows[i] = (Window){.bgConfig = &bg, .bgId = GF_BG_LYR_SUB_2,
        .tilemapLeft = 3, .tilemapTop = 8 + 6 * i, .width = 26, .height = 2, .paletteNum = 15,
        .baseTile = 80 + 52 * i, .pixelBuffer = (void *)(uintptr_t)(0x11000 + 0x1000 * i)};
    for (u32 i = 0; i < 256; ++i) { checkPalette[0][i] = 0x100 + i; checkPalette[1][i] = 0x300 + i; }
    NativeEvolutionUIBind(&data); NativeEvolutionUIBgs(&bg, TRUE); NativeEvolutionUIPublish(&data);
    const HGNativeEvolutionUIState *state = HGNativeEvolutionUIGetState();
    CHECK(state->active && state->ready && state->phase == 8 && state->actionMask == HG_EVOLUTION_UI_ACTION_CANCEL);
    u32 revision = state->revision, keys, x, y;
    CHECK(HGNativeEvolutionUIResolveInput(state, revision, 8, HG_EVOLUTION_UI_INPUT_CANCEL, 0, &keys, &x, &y) && keys == 2 && !x && !y);
    HGNativeEvolutionUIState copy = *state;
    data.species = 155; CHECK(HGNativeEvolutionUIGetState() == state && !memcmp(state, &copy, sizeof(copy))); data.species = 152;
    CHECK(EvolutionCheckReject(state, revision + 1, 8, 1, 0) && EvolutionCheckReject(state, revision, 9, 1, 0));
    CHECK(EvolutionCheckReject(state, revision, 8, 1, 1) && EvolutionCheckReject(state, revision, 8, 1, UINT32_MAX));
    CHECK(EvolutionCheckReject(state, revision, 8, 0, 0) && EvolutionCheckReject(state, revision, 8, 2, 0));
    CHECK(EvolutionCheckReject(NULL, revision, 8, 1, 0));
    CHECK(!HGNativeEvolutionUIResolveInput(state, revision, 8, 1, 0, NULL, &x, &y));
    copy = *state; copy.active = 0; CHECK(EvolutionCheckReject(&copy, revision, 8, 1, 0));
    copy = *state; copy.ready = 0; CHECK(EvolutionCheckReject(&copy, revision, 8, 1, 0));
    copy = *state; copy.summaryActive = 1; CHECK(EvolutionCheckReject(&copy, revision, 8, 1, 0));
    copy = *state; copy.flags = 0; CHECK(EvolutionCheckReject(&copy, revision, 8, 1, 0));
    copy = *state; copy.flags |= 8; CHECK(EvolutionCheckReject(&copy, revision, 8, 1, 0));
    copy = *state; copy.actionMask |= 1; CHECK(EvolutionCheckReject(&copy, revision, 8, 1, 0));
    copy = *state; copy.actionMask = 16; CHECK(EvolutionCheckReject(&copy, revision, 8, 1, 0));
    copy = *state; copy.species = 0; CHECK(EvolutionCheckReject(&copy, revision, 8, 1, 0));
    for (u32 transfer = 0; transfer < 2; ++transfer) for (u32 layer = 0; layer < 8; ++layer) {
        u16 *pending = transfer ? &bg.scrollScheduled : &bg.bufferTransferScheduled; *pending = 1u << layer;
        palette.selectedBuffer = palette.transparentBit = 1;
        NativeEvolutionUIBegin(&data); NativeEvolutionUIPublish(&data);
        CHECK(state->ready && state->revision == revision && state->actionMask == HG_EVOLUTION_UI_ACTION_CANCEL && !state->message.pixelBuffer);
        CHECK(HGNativeEvolutionUIResolveInput(state, revision, 8, 1, 0, &keys, &x, &y) && keys == 2 && !x && !y);
        *pending = 0; palette.selectedBuffer = palette.transparentBit = 0;
    }
    data.flags = 0; NativeEvolutionUIPublish(&data); CHECK(!state->ready && EvolutionCheckReject(state, state->revision, 8, 1, 0));
    data.flags = EVO_FLAG_CAN_CANCEL;

    TextPrinter printer = {.template = {.window = &message}, .id = 3, .active = TRUE, .state = 2};
    SysTask printerTask = {.data = &printer}; sTextPrinterTasks[3] = &printerTask;
    data.printerId = 3; data.state = EVO_STATE_WAIT_ASK_DELETE_MOVE;
    NativeEvolutionUIPublish(&data); revision = state->revision;
    CHECK(state->ready && state->actionMask == HG_EVOLUTION_UI_ACTION_ACK && state->message.engine == 0 && state->message.clearColor == 15);
    CHECK(state->message.palette[15] == checkPalette[0][11 * 16 + 15]);
    CHECK(HGNativeEvolutionUIResolveInput(state, revision, 19, 0, 0, &keys, &x, &y) && keys == 1 && !x && !y);
    for (u32 printerState = 0; printerState < 9; ++printerState) {
        printer.state = printerState; NativeEvolutionUIPublish(&data);
        BOOL canAck = printerState == 2 || printerState == 3 || printerState == 7 || printerState == 8;
        CHECK(state->ready == canAck && state->actionMask == (canAck ? HG_EVOLUTION_UI_ACTION_ACK : 0));
        CHECK(HGNativeEvolutionUIResolveInput(state, state->revision, 19, 0, 0, &keys, &x, &y) == canAck);
    }
    printer.state = 3; TextFlags_SetAutoScrollParam(1); NativeEvolutionUIPublish(&data);
    CHECK(!state->ready && !state->actionMask && EvolutionCheckReject(state, state->revision, 19, 0, 0));
    TextFlags_SetAutoScrollParam(3); NativeEvolutionUIPublish(&data);
    CHECK(state->ready && HGNativeEvolutionUIResolveInput(state, state->revision, 19, 0, 0, &keys, &x, &y) && keys == 1);
    TextFlags_SetAutoScrollParam(0); printer.state = 2; NativeEvolutionUIPublish(&data); revision = state->revision;
    NativeEvolutionUIBegin(&data);
    CHECK(!state->ready && state->revision == revision && !state->message.pixelBuffer && EvolutionCheckReject(state, revision, 19, 0, 0));
    NativeEvolutionUIPublish(&data); CHECK(state->ready && state->revision == revision);
    for (u32 invalid = 0; invalid < 7; ++invalid) {
        printer.state = invalid == 0 ? 0 : 2; printer.active = invalid != 1; printer.id = invalid == 2 ? 4 : 3;
        printer.callbackResult = invalid == 3; printer.callbackParam = invalid == 4;
        printer.template.window = invalid == 5 ? &data.menuWindows[0] : &message;
        data.printerId = invalid == 6 ? MAX_TEXT_PRINTERS : 3;
        NativeEvolutionUIBegin(&data); NativeEvolutionUIPublish(&data);
        CHECK(!state->ready && !state->actionMask && EvolutionCheckReject(state, state->revision, 19, 0, 0));
    }
    printer.state = 2; printer.active = TRUE; printer.id = data.printerId = 3;
    printer.callbackResult = printer.callbackParam = 0; printer.template.window = &message;
    sTextPrinterTasks[3] = NULL; NativeEvolutionUIPublish(&data); CHECK(!state->ready); sTextPrinterTasks[3] = &printerTask;
    for (u32 transfer = 0; transfer < 2; ++transfer) for (u32 layer = 0; layer < 8; ++layer) {
        u16 *pending = transfer ? &bg.scrollScheduled : &bg.bufferTransferScheduled; *pending = 1u << layer;
        NativeEvolutionUIBegin(&data); NativeEvolutionUIPublish(&data); revision = state->revision;
        CHECK(!state->ready && !state->message.pixelBuffer && EvolutionCheckReject(state, revision, 19, 0, 0));
        *pending = 0; CHECK(!state->ready && EvolutionCheckReject(state, revision, 19, 0, 0));
        NativeEvolutionUIPublish(&data); CHECK(state->ready && state->revision > revision);
    }
    for (u32 fade = 0; fade < 2; ++fade) {
        palette.selectedBuffer = fade == 0; palette.transparentBit = fade == 1;
        NativeEvolutionUIPublish(&data); CHECK(!state->ready && !state->message.pixelBuffer);
    }
    palette.selectedBuffer = palette.transparentBit = 0; NativeEvolutionUIPublish(&data); revision = state->revision;
    Window saved = message; message.height = 3; NativeEvolutionUIPublish(&data); CHECK(!state->ready && !state->message.pixelBuffer);
    message = saved; message.bgConfig = &foreignBg; NativeEvolutionUIPublish(&data); CHECK(!state->ready);
    message = saved; message.pixelBuffer = (void *)(uintptr_t)(UINT32_MAX - 27 * 4 * 32 + 1); NativeEvolutionUIPublish(&data); CHECK(!state->ready);
    message = saved; NativeEvolutionUIPublish(&data);
    copy = *state; copy.message.tileHeight = 5; CHECK(EvolutionCheckReject(&copy, copy.revision, 19, 0, 0));
    copy = *state; copy.message.engine = 1; CHECK(EvolutionCheckReject(&copy, copy.revision, 19, 0, 0));
    copy = *state; copy.message.pixelBuffer = UINT32_MAX; CHECK(EvolutionCheckReject(&copy, copy.revision, 19, 0, 0));
    CHECK(EvolutionCheckReject(state, state->revision, 19, 0, 1) && EvolutionCheckReject(state, state->revision, 19, 3, 0));

    const u32 menus[] = {EVO_STATE_DELETE_MOVE_MENU, EVO_STATE_STOP_LEARNING_MENU};
    for (u32 menu = 0; menu < NELEMS(menus); ++menu) {
        data.state = menus[menu]; data.menuState = EVO_MENU_STATE_INPUT;
        NativeEvolutionUIPublish(&data); revision = state->revision;
        CHECK(state->ready && state->actionMask == (HG_EVOLUTION_UI_ACTION_TOP | HG_EVOLUTION_UI_ACTION_BOTTOM));
        CHECK(state->choices[0].engine == 1 && state->choices[0].tileWidth == 26 && state->choices[0].tileHeight == 2 && !state->choices[0].clearColor);
        CHECK(state->choices[1].palette[15] == checkPalette[1][15 * 16 + 15]);
        for (u32 index = 0; index < 2; ++index)
            CHECK(HGNativeEvolutionUIResolveInput(state, revision, menus[menu], 2, index, &keys, &x, &y) && !keys && x == 127 && y == (index ? 119 : 71));
        CHECK(EvolutionCheckReject(state, revision, menus[menu], 2, 2) && EvolutionCheckReject(state, revision, menus[menu], 0, 0));
        copy = *state; copy.choices[0].clearColor = 1; CHECK(EvolutionCheckReject(&copy, revision, menus[menu], 2, 0));
        copy = *state; copy.choices[1].tileWidth = 25; CHECK(EvolutionCheckReject(&copy, revision, menus[menu], 2, 0));
        NativeEvolutionUIBegin(&data); CHECK(!state->ready && !state->choices[0].pixelBuffer && state->revision == revision);
        NativeEvolutionUIPublish(&data); CHECK(state->ready && state->revision == revision);
        data.menuState = EVO_MENU_STATE_WAIT_CURSOR; NativeEvolutionUIPublish(&data);
        CHECK(!state->ready && !state->actionMask && !state->choices[0].pixelBuffer && state->revision > revision);
        CHECK(EvolutionCheckReject(state, revision, menus[menu], 2, 0));
        copy = *state; copy.ready = TRUE; copy.actionMask = HG_EVOLUTION_UI_ACTION_TOP | HG_EVOLUTION_UI_ACTION_BOTTOM;
        CHECK(EvolutionCheckReject(&copy, copy.revision, menus[menu], 2, 0));
        data.menuState = EVO_MENU_STATE_INPUT; data.spritesReady = FALSE; NativeEvolutionUIPublish(&data); CHECK(!state->ready);
        data.spritesReady = TRUE; data.menuWindows[1].height = 3; NativeEvolutionUIPublish(&data); CHECK(!state->ready && !state->choices[0].pixelBuffer);
        data.menuWindows[1].height = 2; NativeEvolutionUIPublish(&data);
    }
    copy = *state; NativeEvolutionUIBegin(&foreign); NativeEvolutionUIPublish(&foreign); NativeEvolutionUIBgs(&foreignBg, FALSE);
    CHECK(!memcmp(state, &copy, sizeof(copy)));
    revision = state->revision; NativeEvolutionUIBgs(&bg, FALSE);
    CHECK(state->active && !state->ready && state->revision > revision && !state->message.pixelBuffer && !state->choices[0].pixelBuffer);
    message.pixelBuffer = NULL; NativeEvolutionUIPublish(&data); CHECK(!state->ready && !state->choices[1].pixelBuffer);
    data.summaryApp = &child; data.state = EVO_STATE_RUN_SUMMARY; NativeEvolutionUIPublish(&data);
    CHECK(state->active && state->summaryActive && !state->ready && !state->actionMask && !state->message.pixelBuffer);
    CHECK(EvolutionCheckReject(state, state->revision, 23, 2, 0));
    message = saved; data.summaryApp = NULL; data.state = EVO_STATE_STOP_LEARNING_MENU;
    NativeEvolutionUIBgs(&bg, TRUE); NativeEvolutionUIPublish(&data);
    CHECK(state->active && !state->summaryActive && state->ready && state->revision > revision && state->choices[0].pixelBuffer);
    CHECK(EvolutionCheckReject(state, revision, 35, 2, 0));
    revision = state->revision; data.done = TRUE; NativeEvolutionUIPublish(&data);
    CHECK(state->active && !state->ready && !state->actionMask && !state->message.pixelBuffer && !state->choices[0].pixelBuffer);
    // This is the real native invalidation called first by Delete, not a fabricated run of its resource cleanup.
    NativeEvolutionUIBind(NULL); CHECK(!state->active && !state->ready && state->revision > revision);
    CHECK(EvolutionCheckReject(state, revision, 35, 2, 0));
    copy = *state; NativeEvolutionUIBind(NULL); CHECK(!memcmp(state, &copy, sizeof(copy)));
    data.done = FALSE; NativeEvolutionUIBind(&data); NativeEvolutionUIBgs(&bg, TRUE); NativeEvolutionUIPublish(&data);
    CHECK(state->active && state->ready && state->revision > copy.revision);
    NativeEvolutionUIBind(&foreign); CHECK(state->active && !state->ready && !state->message.pixelBuffer && !state->choices[0].pixelBuffer);
    NativeEvolutionUIPublish(&data); CHECK(!state->ready);
    NativeEvolutionUIBind(NULL); sTextPrinterTasks[3] = NULL;
    return 0;
}

#ifdef HG_NATIVE_EVOLUTION_UI_CHECK_MAIN
int main(void) {
    u32 line = CheckOriginalEvolutionUI();
    if (line) { fprintf(stderr, "Evolution UI metadata check line %u\n", line); return 1; }
    puts("PASS: Evolution metadata, real printer/palette queries, typed input and native lifetime guards; no gameplay executed.");
    return 0;
}
#endif
