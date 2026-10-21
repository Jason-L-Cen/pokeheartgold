// SPDX-License-Identifier: GPL-3.0-or-later
#include "../../../src/battle/battle_input.c"
#include <stdio.h>
#include <stdlib.h>
#define CHECK(value) do { if (!(value)) return __LINE__; } while (0)

u32 CheckOriginalBattleUI(void) {
    BattleInput input = {0}, other = {0};
    NativeBattleUIBind(&input);
    const HGNativeBattleUIState *state = HGNativeBattleUIGetState();
    CHECK(state->active && !state->mode && !state->ready);
    input.curMenuId = BATTLE_MENU_3; CHECK(NativeBattleUIMode(&input) == HG_BATTLE_UI_COMMAND);
    input.curMenuId = BATTLE_MENU_4; CHECK(NativeBattleUIMode(&input) == HG_BATTLE_UI_COMMAND);
    input.curMenuId = BATTLE_MENU_MAIN;
    CHECK(NativeBattleUIPoll(&input) == -1);
    state = HGNativeBattleUIGetState();
    u32 revision = state->revision;
    CHECK(state->mode == HG_BATTLE_UI_COMMAND && state->ready && state->enabledMask == 15);
    CHECK(!HGNativeBattleUIChoose(revision + 1, 0) && !HGNativeBattleUIChoose(revision, 4));
    CHECK(!HGNativeBattleUIChoose(revision, 0xFFFFFFFF));
    CHECK(HGNativeBattleUIChoose(revision, 2) && !HGNativeBattleUIChoose(revision, 0));
    CHECK(NativeBattleUIPoll(&other) == -1 && NativeBattleUIPoll(&input) == 2);
    CHECK(!HGNativeBattleUIGetState()->ready);
    CHECK(NativeBattleUIPoll(&input) == -1);
    CHECK(HGNativeBattleUIChoose(revision, 1));
    NativeBattleUIInvalidate(&input);
    CHECK(NativeBattleUIPoll(&input) == -1 && !HGNativeBattleUIChoose(revision, 0));

    u8 tiles[128] = {0};
    tiles[0] = 0x87; tiles[32] = 7; tiles[64] = 8;
    input.curMenuId = BATTLE_MENU_FIGHT;
    input.menu.fight = (BattleInputFightMenu){.moveNo = {33, 45, 0, 0}, .pp = {32, 0, 0, 0}, .ppMax = {35, 40, 0, 0}, .battlerType = BATTLER_TYPE_PLAYER_SIDE_SLOT_1};
    input.moveMemory[0].moveDisplay[0] = (BattleInputMoveMemory_DisplayObj){.window = {.width = 2, .height = 2, .pixelBuffer = tiles}, .fontLength = 16};
    NativeBattleUIInvalidate(&input); NativeBattleUIPoll(&input);
    state = HGNativeBattleUIGetState(); revision = state->revision;
    CHECK(state->mode == HG_BATTLE_UI_MOVES && state->enabledMask == 19);
    CHECK(state->move[0] == 33 && state->pp[0] == 32 && state->maxPP[0] == 35 && state->nameWidth[0] == 16);
    CHECK(state->namePixels[0][0] == 250 && state->namePixels[0][3] == 255 && state->namePixels[0][4] == 45);
    CHECK(state->namePixels[0][8 * 4] == 250 && state->namePixels[0][8 * HG_BATTLE_NAME_WIDTH * 4] == 45);
    CHECK(!state->namePixels[0][2 * 4 + 3] && !state->namePixels[1][3]);
    input.moveMemory[0].moveDisplay[0].window.width = 33;
    NativeBattleUIPoll(&input);
    CHECK(!HGNativeBattleUIGetState()->nameWidth[0]);
    input.moveMemory[0].moveDisplay[0].window.width = 2;
    input.moveMemory[0].moveDisplay[0].window.height = 3;
    NativeBattleUIPoll(&input);
    CHECK(!HGNativeBattleUIGetState()->nameWidth[0]);
    input.moveMemory[0].moveDisplay[0].window.height = 2;
    CHECK(!HGNativeBattleUIChoose(revision, 2));
    CHECK(HGNativeBattleUIChoose(revision, 1) && sTouchArrayFightMenu[NativeBattleUIPoll(&input)] == BATTLE_INPUT_MOVE_2); // Original rules report zero PP.
    input.menu.fight.moveNo[2] = 75; input.menu.fight.moveNo[3] = 77;
    for (u32 i = 0; i < 4; i++) {
        NativeBattleUIPoll(&input);
        CHECK(HGNativeBattleUIChoose(revision, i));
        CHECK(sTouchArrayFightMenu[NativeBattleUIPoll(&input)] == BATTLE_INPUT_MOVE_1 + i);
    }
    NativeBattleUIPoll(&input); CHECK(HGNativeBattleUIChoose(revision, 4) && sTouchArrayFightMenu[NativeBattleUIPoll(&input)] == BATTLE_INPUT_CANCEL);
    input.isTouchDisabled = TRUE; NativeBattleUIPoll(&input); CHECK(!HGNativeBattleUIGetState()->ready);
    input.isTouchDisabled = FALSE; input.tutorial.finger = (void *)1;
    NativeBattleUIPoll(&input);
    CHECK(!HGNativeBattleUIGetState()->mode && !HGNativeBattleUIChoose(revision, 0));
    NativeBattleUIBind(NULL); state = HGNativeBattleUIGetState(); CHECK(!state->active && !state->ready);
    return 0;
}
#ifndef HEARTGOLD_ABI32
int main(void) { u32 line = CheckOriginalBattleUI(); if (line) { fprintf(stderr,"Battle UI check line %u\n",line); return 1; } puts("PASS: native battle menu revisions, original choices, PP, glyph tiles, stale/duplicate rejection and lifetime."); return 0; }
#endif
