// SPDX-License-Identifier: GPL-3.0-or-later
#define BattleController_ReplyItemSelection ItemReplyCapture
#include "../../../src/battle/battle_controller_opponent.c"
#include <stdio.h>
#include <string.h>
#define CHECK(value) do { if (!(value)) return __LINE__; } while (0)
static BattleSystem *capturedSystem;
static int capturedBattler;
static u32 capturedWord;
void ItemReplyCapture(BattleSystem *system, int battler, u32 input) {
    capturedSystem = system; capturedBattler = battler; capturedWord = input;
}
u32 CheckOriginalItemReply(void) {
    u32 marker = 0;
    BattleSystem *system = (BattleSystem *)&marker;
    BattleItemInput input = {.item = 0x1234, .pocket = 0x56, .target = 0x78};
    const u8 bytes[] = {0x34, 0x12, 0x56, 0x78};
    REPLY_ITEM_SELECTION(system, 3, input);
    CHECK(capturedSystem == system && capturedBattler == 3 && !memcmp(&capturedWord, bytes, 4));
    input = (BattleItemInput){.item = ITEM_NONE, .pocket = 4};
    REPLY_ITEM_SELECTION(system, 0, input);
    CHECK((capturedWord & 0xFFFF) == ITEM_NONE && capturedWord >> 16 == 4);
    struct { BattleItemInput input; } recorded = {.input = {.item = 0xFFFF, .pocket = 0xFF, .target = 0xFF}};
    REPLY_ITEM_SELECTION(system, 2, recorded.input);
    CHECK(capturedWord == UINT32_MAX && capturedBattler == 2);
    return 0;
}
#ifndef HEARTGOLD_ABI32
int main(void) { u32 line = CheckOriginalItemReply(); if (line) { fprintf(stderr,"Item reply check line %u\n",line); return 1; } puts("PASS: original item reply bytes, cancellation and recorded aggregate callers."); return 0; }
#endif
