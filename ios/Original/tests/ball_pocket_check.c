// SPDX-License-Identifier: GPL-3.0-or-later
#define PaletteData_GetSelectedBuffersBitmask BallPocketFade
#define PlaySE BallPocketSound
#define TouchscreenHitbox_FindRectAtTouchNew BallPocketTouch
#define BattleDpadCursor_HandleInput BallPocketCursorInput
#define BattleDpadCursor_SetActive BallPocketCursorActive
#define BattleDpadCursor_ResetPosition BallPocketCursorReset
#define BattleDpadCursor_GetBattleCursor BallPocketCursor
#define BattleCursor_Disable BallPocketCursorDisable
#define sBattleBagPocketMenuHitboxes BallPocketHitboxes
#include "../../../src/battle/battle_bag_menu.c"
#include <stdio.h>
#include <stdlib.h>
#define CHECK(value) do { if (!(value)) return __LINE__; } while (0)
static u16 fading;
static int touchInput, sounds, touchReads;
// Hit testing is stubbed; this fixture executes the original menu state and feedback path.
const TouchscreenHitbox BallPocketHitboxes[] = {{.rect = {.top = TOUCHSCREEN_RECTLIST_END}}};
u16 BallPocketFade(PaletteData *unused) { return fading; }
BOOL BallPocketSound(u16 seq) { if (seq != SEQ_SE_DP_DECIDE) abort(); sounds++; return TRUE; }
int BallPocketTouch(const TouchscreenHitbox *unused) { touchReads++; return touchInput; }
int BallPocketCursorInput(BattleDpadCursor *unused) { return -1; }
void BallPocketCursorActive(BattleDpadCursor *unused, u8 active) { }
void BallPocketCursorReset(BattleDpadCursor *unused) { }
BattleCursor *BallPocketCursor(const BattleDpadCursor *unused) { return NULL; }
void BallPocketCursorDisable(BattleCursor *unused) { }

u32 CheckOriginalBallPocket(void) {
    BattleBagContext context = {.nativeBallPocket = TRUE};
    BattleBagMenu menu = {.context = &context};
    sounds = touchReads = 0; fading = 1; touchInput = -1;
    CHECK(BattleBagMenu_HandlePocketMenuInput(&menu) == BATTLE_BAG_STATE_POCKET_MENU);
    CHECK(context.nativeBallPocket && !sounds && !touchReads && !menu.buttonAnimActive);
    fading = 0;
    CHECK(BattleBagMenu_HandlePocketMenuInput(&menu) == BATTLE_BAG_STATE_WAIT_BUTTON_ANIM);
    CHECK(!context.nativeBallPocket && !touchReads && sounds == 1);
    CHECK(menu.pocket == BTLPOCKETLIST_BALLS && menu.nextState == BATTLE_BAG_STATE_SHOW_ITEM_LIST);
    CHECK(menu.buttonAnimActive && menu.buttonAnimButton == BTLPOCKETLIST_BALLS);
    CHECK(menu.buttonAnimState == BUTTON_ANIM_STATE_PRESS && !menu.buttonAnimTimer);
    CHECK(BattleBagMenu_WaitButtonAnim(&menu) == BATTLE_BAG_STATE_WAIT_BUTTON_ANIM);
    menu.buttonAnimState = BUTTON_ANIM_STATE_RELEASE;
    CHECK(BattleBagMenu_WaitButtonAnim(&menu) == BATTLE_BAG_STATE_SHOW_ITEM_LIST);
    CHECK(BattleBagMenu_HandlePocketMenuInput(&menu) == BATTLE_BAG_STATE_POCKET_MENU);
    CHECK(touchReads == 1 && sounds == 1); // No second automatic selection.
    touchInput = BTLPOCKETLIST_BATTLE_ITEMS;
    CHECK(BattleBagMenu_HandlePocketMenuInput(&menu) == BATTLE_BAG_STATE_WAIT_BUTTON_ANIM);
    CHECK(menu.pocket == BTLPOCKETLIST_BATTLE_ITEMS && sounds == 2);
    context.nativeBallPocket = TRUE; context.isTutorial = TRUE; touchInput = -1;
    CHECK(BattleBagMenu_HandlePocketMenuInput(&menu) == BATTLE_BAG_STATE_POCKET_MENU);
    CHECK(context.nativeBallPocket && sounds == 2 && touchReads == 3);
    return 0;
}
#ifndef HEARTGOLD_ABI32
int main(void) { u32 line = CheckOriginalBallPocket(); if (line) { fprintf(stderr,"Ball pocket check line %u\n",line); return 1; } puts("PASS: original ball-pocket selection, fade gate, press animation, one-time intent, normal input and tutorial isolation."); return 0; }
#endif
