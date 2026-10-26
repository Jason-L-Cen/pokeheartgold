// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef HEARTGOLD_NATIVE_BATTLE_UI_H
#define HEARTGOLD_NATIVE_BATTLE_UI_H
#include <stdint.h>

enum { HG_BATTLE_UI_NONE, HG_BATTLE_UI_COMMAND, HG_BATTLE_UI_MOVES, HG_BATTLE_UI_TARGETS, HG_BATTLE_UI_NEXT_MON,
    HG_BATTLE_UI_KEEP_FORGET, HG_BATTLE_UI_GIVE_UP, HG_BATTLE_UI_YES_NO };
enum { HG_BATTLE_NAME_WIDTH = 256, HG_BATTLE_NAME_HEIGHT = 16 };
typedef struct HGNativeBattleUIState {
    uint32_t active, revision, mode, ready, enabledMask, cancelRun;
    uint32_t move[4], pp[4], maxPP[4], nameWidth[4];
    uint32_t targetSpecies[4], targetHp[4], targetMaxHp[4], targetMask[4];
    uint8_t namePixels[4][HG_BATTLE_NAME_WIDTH * HG_BATTLE_NAME_HEIGHT * 4];
} HGNativeBattleUIState;

enum { HG_CAPTURE_UI_DISMISS_PHASE = 13 };
typedef struct HGNativeCaptureUIState {
    uint32_t active, ownerGeneration, revision, callerPhase, ready, done, species, form;
} HGNativeCaptureUIState;

#ifdef HEARTGOLD_NATIVE
extern HGNativeBattleUIState gNativeBattleUIState;
extern HGNativeCaptureUIState gNativeCaptureUIState;
void NativeCaptureUIBegin(uint32_t species, uint32_t form);
void NativeCaptureUIPublish(uint32_t callerPhase, uint32_t done);
void NativeCaptureUIInvalidate(void);
#endif

// Call on the engine owner thread; UIKit queues requests instead of accessing game state.
const HGNativeBattleUIState *HGNativeBattleUIGetState(void);
const HGNativeCaptureUIState *HGNativeCaptureUIGetState(void);
uint32_t HGNativeBattleUIChoose(uint32_t revision, uint32_t index);
struct BattleInput;
uint32_t HGNativeBattleUIConsumeBallPocket(struct BattleInput *input);

static inline uint32_t HGNativeCaptureUIResolveInput(const HGNativeCaptureUIState *state,
    uint32_t ownerGeneration, uint32_t revision, uint32_t *keys) {
    if (!state || !keys || state->active != 1 || !ownerGeneration || ownerGeneration != state->ownerGeneration
        || !revision || revision != state->revision || state->callerPhase != HG_CAPTURE_UI_DISMISS_PHASE
        || state->ready != 1 || state->done != 1 || !state->species || state->species > 493) return 0;
    *keys = 1; // The original caller accepts A after the popup's cry finishes.
    return 1;
}
#endif
