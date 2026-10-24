// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef HEARTGOLD_NATIVE_BATTLE_UI_H
#define HEARTGOLD_NATIVE_BATTLE_UI_H
#include <stdint.h>

enum { HG_BATTLE_UI_NONE, HG_BATTLE_UI_COMMAND, HG_BATTLE_UI_MOVES, HG_BATTLE_UI_TARGETS, HG_BATTLE_UI_NEXT_MON };
enum { HG_BATTLE_NAME_WIDTH = 256, HG_BATTLE_NAME_HEIGHT = 16 };
typedef struct HGNativeBattleUIState {
    uint32_t active, revision, mode, ready, enabledMask, cancelRun;
    uint32_t move[4], pp[4], maxPP[4], nameWidth[4];
    uint32_t targetSpecies[4], targetHp[4], targetMaxHp[4], targetMask[4];
    uint8_t namePixels[4][HG_BATTLE_NAME_WIDTH * HG_BATTLE_NAME_HEIGHT * 4];
} HGNativeBattleUIState;

#ifdef HEARTGOLD_NATIVE
extern HGNativeBattleUIState gNativeBattleUIState;
#endif

// Call on the engine owner thread; UIKit queues requests instead of accessing game state.
const HGNativeBattleUIState *HGNativeBattleUIGetState(void);
uint32_t HGNativeBattleUIChoose(uint32_t revision, uint32_t index);
struct BattleInput;
uint32_t HGNativeBattleUIConsumeBallPocket(struct BattleInput *input);
#endif
