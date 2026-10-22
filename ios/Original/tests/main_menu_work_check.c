// SPDX-License-Identifier: GPL-3.0-or-later
// Standalone component: GPU calls are observers; no display, audio or save file is opened.
#include "../../../src/application/main_menu/main_menu_util.c"
#include "../../../src/application/main_menu/main_menu_shared.c"
#include "../abi32/main_menu_check.c"
#include <stdio.h>
#include <stdlib.h>

static unsigned events[10], eventCount;
static BgConfig *expectedBg;
static fx32 expectedX, expectedY;
static int callbackWorkMatches, rendererMatches;
void GF_RunVramTransferTasks(void) { events[eventCount++] = 2; }
void OamManager_ApplyAndResetBuffers(void) { events[eventCount++] = 3; }
void DoScheduledBgGpuUpdates(BgConfig *bg) { events[eventCount++] = bg == expectedBg ? 4 : 99; }
void OS_SetIrqCheckFlag(OSIrqMask flag) { events[eventCount++] = flag == OS_IE_V_BLANK ? 5 : 99; }
void G2dRenderer_SetSubSurfaceCoords(G2dRenderer *renderer, fx32 x, fx32 y) {
    rendererMatches = renderer == &gMainMenuUtilWork.sprites.renderer && x == expectedX && y == expectedY;
}
static void ObserveWork(MainMenuUtilWork *work) {
    callbackWorkMatches = work == &gMainMenuUtilWork;
    events[eventCount++] = 1;
}

u32 CheckOriginalMainMenuCallbacks(void) {
    CHECK(CheckNativeMainMenuWork() == 0);
    BgConfig bg;
    expectedBg = &bg;
    gMainMenuUtilWork.bgConfig = expectedBg;
    gMainMenuUtilWork.vblankCallback = ObserveWork;
    gMainMenuUtilWork.giftSprite = (Sprite *)&bg;
    expectedX = -FX32_ONE;
    expectedY = FX32_CONST(192);
    MainMenuUtil_SetSubScreenOffset(expectedX, expectedY);
    CHECK(rendererMatches && gMainMenuUtilWork.subScreenY == expectedY);
    CHECK(gMainMenuUtilWork.giftSprite == (Sprite *)&bg);
    GFIntrCB callback = MainMenuUtil_VBlankCB;
    callback(&bg);
    CHECK(callbackWorkMatches && gMainMenuUtilWork.vblankCallback == NULL);
    CHECK(eventCount == 5 && events[0] == 1 && events[1] == 2 && events[2] == 3 && events[3] == 4 && events[4] == 5);
    callback(NULL);
    CHECK(eventCount == 9 && events[5] == 2 && events[6] == 3 && events[7] == 4 && events[8] == 5);
    MainMenuUtil_InitWork(HEAP_ID_MAIN_MENU);
    return 0;
}

#ifndef HEARTGOLD_ABI32
void HGNativeAssertFail(const char *condition, const char *file, int line) {
    fprintf(stderr, "%s:%d: %s\n", file, line, condition);
    abort();
}
int main(void) {
    u32 line = CheckOriginalMainMenuCallbacks();
    if (line) { fprintf(stderr, "Main menu work check line %u\n", line); return 1; }
    puts("PASS: original main-menu work initialization, fades, save states, renderer coordinates and one-shot typed VBlank callback.");
    return 0;
}
#endif
