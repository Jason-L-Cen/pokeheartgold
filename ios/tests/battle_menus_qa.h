// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef HEARTGOLD_BATTLE_MENUS_QA_H
#define HEARTGOLD_BATTLE_MENUS_QA_H

#include <stdint.h>

enum { HG_BATTLE_MENUS_QA_VERSION = 1, HG_BATTLE_MENUS_QA_BYTES = 176 };

typedef struct HGNativeBattleMenusQAState {
    uint32_t version, byteSize, revision, fieldReady, requested, started, returned, errorLine;
    uint32_t battleActive, battleGeneration, fieldFrame, commandCount, totalTurns, battlerId, activePartySlot;
    uint32_t playerHp, playerMaxHp, playerStatus, enemySpecies, enemyLevel, enemyHp, enemyMaxHp;
    uint32_t potionQuantity, partyCount, moves[4], pp[4];
    uint32_t replyCount, replyItem, replyPocket, replyTarget, replyHp, replyMaxHp, replyPotionQuantity, replyTurns;
    uint32_t outcome, battleResult, semanticsMask, lifecycleCleared;
} HGNativeBattleMenusQAState;

_Static_assert(sizeof(HGNativeBattleMenusQAState) == HG_BATTLE_MENUS_QA_BYTES, "Battle menus QA packet layout");

const HGNativeBattleMenusQAState *HGNativeBattleMenusQAGetState(void);
uint32_t HGNativeBattleMenusQARequest(uint32_t expectedRevision);

#endif
