// SPDX-License-Identifier: GPL-3.0-or-later
// Included only by the private, resident field_system.c copy.
#include "battle_menus_qa.h"
#include "battle/battle.h"
#include "battle/battle_controller.h"
#include "battle/battle_system.h"
#include "battle/battle_022378C0.h"
#include "encounter.h"
#include "field/draw_map_name.h"
#include "field_system.h"
#include "pokemon.h"
#include "screen_fade.h"
#include "sys_flags.h"
#include "system.h"
#include "constants/items.h"
#include "constants/pokemon.h"
#include "constants/species.h"
#include "constants/vars.h"

static HGNativeBattleMenusQAState sMenusQA = { .version = HG_BATTLE_MENUS_QA_VERSION, .byteSize = HG_BATTLE_MENUS_QA_BYTES, .revision = 1 };
static BattleSystem *sMenusLiveBattle;
static u32 sMenusOutcome, sMenusPotionUses, sMenusInitialCount, sMenusInitialMons[6][26];
static Bag sMenusInitialBag, sMenusExpectedBag;
static u32 sMenusMoney, sMenusStarter, sMenusGotDex, sMenusDexEnabled, sMenusDexCaught, sMenusWild;
static const u32 sMenusAttributes[26] = {
    MON_DATA_PERSONALITY,MON_DATA_OT_ID,MON_DATA_SPECIES,MON_DATA_FORM,MON_DATA_IS_EGG,MON_DATA_HELD_ITEM,
    MON_DATA_HP,MON_DATA_MAX_HP,MON_DATA_LEVEL,MON_DATA_STATUS,MON_DATA_MOVE1,MON_DATA_MOVE2,MON_DATA_MOVE3,MON_DATA_MOVE4,
    MON_DATA_MOVE1_PP,MON_DATA_MOVE2_PP,MON_DATA_MOVE3_PP,MON_DATA_MOVE4_PP,
    MON_DATA_MOVE1_PP_UPS,MON_DATA_MOVE2_PP_UPS,MON_DATA_MOVE3_PP_UPS,MON_DATA_MOVE4_PP_UPS,
    MON_DATA_MOVE1_MAX_PP,MON_DATA_MOVE2_MAX_PP,MON_DATA_MOVE3_MAX_PP,MON_DATA_MOVE4_MAX_PP
};
#define MENUS_REQUIRE(value) do { if (!(value) && !sMenusQA.errorLine) sMenusQA.errorLine = __LINE__; } while (0)

const HGNativeBattleMenusQAState *HGNativeBattleMenusQAGetState(void) { return &sMenusQA; }
u32 HGNativeBattleMenusQARequest(u32 expectedRevision) {
    if (expectedRevision != sMenusQA.revision || !sMenusQA.fieldReady || sMenusQA.requested || sMenusQA.started) return FALSE;
    sMenusQA.requested = 1; ++sMenusQA.revision;
    return TRUE;
}
static void BattleMenusObserve(BattleSystem *system, BattleContext *ctx, int battlerId, int partySlot) {
    if (!sMenusQA.started || battlerId != BATTLER_PLAYER) return;
    sMenusLiveBattle = system;
    sMenusQA.totalTurns = ctx->totalTurns; sMenusQA.battlerId = battlerId; sMenusQA.activePartySlot = partySlot;
    sMenusQA.playerHp = ctx->battleMons[BATTLER_PLAYER].hp > 0 ? ctx->battleMons[BATTLER_PLAYER].hp : 0;
    sMenusQA.playerMaxHp = ctx->battleMons[BATTLER_PLAYER].maxHp; sMenusQA.playerStatus = ctx->battleMons[BATTLER_PLAYER].status;
    sMenusQA.enemySpecies = ctx->battleMons[BATTLER_ENEMY].species; sMenusQA.enemyLevel = ctx->battleMons[BATTLER_ENEMY].level;
    sMenusQA.enemyHp = ctx->battleMons[BATTLER_ENEMY].hp > 0 ? ctx->battleMons[BATTLER_ENEMY].hp : 0;
    sMenusQA.enemyMaxHp = ctx->battleMons[BATTLER_ENEMY].maxHp;
    sMenusQA.potionQuantity = Bag_GetQuantity(system->bag, ITEM_POTION, HEAP_ID_BATTLE);
    sMenusQA.partyCount = BattleSystem_GetPartySize(system, BATTLER_PLAYER);
    for (u32 i = 0; i < 4; ++i) { sMenusQA.moves[i] = ctx->battleMons[BATTLER_PLAYER].moves[i]; sMenusQA.pp[i] = ctx->battleMons[BATTLER_PLAYER].movePPCur[i]; }
}
void __real_BattleController_EmitSelectCommand(BattleSystem *, BattleContext *, int, int);
void __wrap_BattleController_EmitSelectCommand(BattleSystem *system, BattleContext *ctx, int battlerId, int partySlot) {
    if (sMenusQA.started && battlerId == BATTLER_PLAYER) { BattleMenusObserve(system, ctx, battlerId, partySlot); ++sMenusQA.commandCount; }
    __real_BattleController_EmitSelectCommand(system, ctx, battlerId, partySlot);
}
void __real_BattleController_ReplyItemSelection(BattleSystem *, int, BattleItemInput);
void __wrap_BattleController_ReplyItemSelection(BattleSystem *system, int battlerId, BattleItemInput input) {
    if (sMenusQA.started && battlerId == BATTLER_PLAYER) {
        ++sMenusQA.replyCount; sMenusQA.replyItem = input.item;
        sMenusQA.replyPocket = sMenusQA.replyTarget = sMenusQA.replyHp = sMenusQA.replyMaxHp = 0;
        sMenusQA.replyTurns = system->ctx->totalTurns;
        sMenusQA.replyPotionQuantity = Bag_GetQuantity(system->bag, ITEM_POTION, HEAP_ID_BATTLE);
        // Cancel leaves the original pocket/target bytes unspecified.
        if (input.item != 0xFF) {
            sMenusQA.replyPocket = input.pocket; sMenusQA.replyTarget = input.target;
            MENUS_REQUIRE(input.item == ITEM_POTION && input.pocket == 0 && input.target > 0 && input.target <= BattleSystem_GetPartySize(system, battlerId));
            if (input.target > 0 && input.target <= BattleSystem_GetPartySize(system, battlerId)) {
                Pokemon *mon = BattleSystem_GetPartyMon(system, battlerId, input.target - 1);
                sMenusQA.replyHp = GetMonData(mon, MON_DATA_HP, NULL); sMenusQA.replyMaxHp = GetMonData(mon, MON_DATA_MAX_HP, NULL);
            }
            if (input.item == ITEM_POTION) ++sMenusPotionUses;
        }
    }
    __real_BattleController_ReplyItemSelection(system, battlerId, input);
}
void __real_Battle_BattleExit(OverlayManager *);
void __wrap_Battle_BattleExit(OverlayManager *manager) {
    if (sMenusQA.started && !sMenusQA.returned) {
        sMenusLiveBattle = NULL; sMenusQA.battleActive = FALSE; sMenusQA.lifecycleCleared = TRUE; ++sMenusQA.revision;
    }
    __real_Battle_BattleExit(manager);
}
static void BattleMenusSemantics(FieldSystem *field, BOOL after) {
    SaveData *save = field->saveData; Party *party = SaveArray_Party_Get(save); Bag *bag = Save_Bag_Get(save);
    SaveVarsFlags *flags = Save_VarsFlags_Get(save); Pokedex *dex = Save_Pokedex_Get(save);
    u32 count = Party_GetCount(party), money = PlayerProfile_GetMoney(Save_PlayerData_GetProfile(save));
    MENUS_REQUIRE(count <= 6);
    if (!after) {
        sMenusInitialCount = count;
        for (u32 i = 0; i < count && i < 6; ++i) for (u32 j = 0; j < 26; ++j) sMenusInitialMons[i][j] = GetMonData(Party_GetMonByIndex(party, i), sMenusAttributes[j], NULL);
        Save_Bag_Copy(bag, &sMenusInitialBag);
        sMenusMoney = money; sMenusStarter = CheckGotStarter(flags); sMenusGotDex = CheckGotPokedex(flags);
        sMenusDexEnabled = Pokedex_IsEnabled(dex); sMenusDexCaught = Pokedex_CountNationalDexOwned(dex);
        sMenusWild = GameStats_GetCapped(Save_GameStats_Get(save), GAME_STAT_WILD_ENCOUNTERS);
        sMenusQA.semanticsMask = 1;
        return;
    }
    BOOL identity = count == sMenusInitialCount, hp = TRUE, pp = TRUE, items = sMenusPotionUses <= 1;
    for (u32 i = 0; i < count && i < 6; ++i) for (u32 j = 0; j < 26; ++j) {
        u32 value = GetMonData(Party_GetMonByIndex(party, i), sMenusAttributes[j], NULL), initial = sMenusInitialMons[i][j];
        if (j == 6) hp &= value > 0 && value <= sMenusInitialMons[i][7];
        else if (i == 0 && j == 15) pp &= value <= initial && initial - value <= 6;
        else identity &= value == initial;
    }
    Save_Bag_Copy(&sMenusInitialBag, &sMenusExpectedBag);
    BOOL potionFound = FALSE;
    for (u32 i = 0; i < NUM_BAG_MEDICINE; ++i) if (sMenusExpectedBag.medicine[i].id == ITEM_POTION) {
        potionFound = TRUE;
        items &= sMenusExpectedBag.medicine[i].quantity >= sMenusPotionUses;
        if (items) sMenusExpectedBag.medicine[i].quantity -= sMenusPotionUses;
    }
    items &= (!sMenusPotionUses || potionFound) && !memcmp(&sMenusExpectedBag, bag, sizeof(Bag));
    BOOL unlocks = money == sMenusMoney && CheckGotStarter(flags) == sMenusStarter && CheckGotPokedex(flags) == sMenusGotDex && Pokedex_IsEnabled(dex) == sMenusDexEnabled && Pokedex_CountNationalDexOwned(dex) == sMenusDexCaught;
    BOOL settlement = GameStats_GetCapped(Save_GameStats_Get(save), GAME_STAT_WILD_ENCOUNTERS) == sMenusWild + 1;
    sMenusQA.semanticsMask = 1 | (identity && pp ? 2 : 0) | (hp ? 4 : 0) | (items ? 8 : 0) | (unlocks ? 16 : 0) | (settlement ? 32 : 0);
    MENUS_REQUIRE(sMenusQA.semanticsMask == 63);
}
static BOOL BattleMenusQATask(TaskManager *task) {
    u32 *state = TaskManager_GetStatePtr(task);
    if (!*state) {
        sMenusQA.started = sMenusQA.battleActive = TRUE; ++sMenusQA.battleGeneration; ++sMenusQA.revision; *state = 1;
        SetupAndStartWildBattle(task, SPECIES_CATERPIE, 1, &sMenusOutcome, FALSE, FALSE);
        return FALSE;
    }
    FieldSystem *field = TaskManager_GetFieldSystem(task);
    sMenusQA.outcome = sMenusOutcome; sMenusQA.battleResult = FieldSystem_VarGet(field, VAR_BATTLE_RESULT);
    MENUS_REQUIRE(!sMenusLiveBattle && sMenusQA.lifecycleCleared && sMenusQA.battleResult == sMenusOutcome);
    if (sMenusOutcome == BATTLE_OUTCOME_PLAYER_FLED) BattleMenusSemantics(field, TRUE);
    sMenusQA.returned = TRUE; ++sMenusQA.revision;
    return TRUE;
}
static void BattleMenusQAFieldTick(FieldSystem *field) {
    // The original child app ran first; refresh real read-only battle values without replying or advancing a command.
    if (sMenusLiveBattle) BattleMenusObserve(sMenusLiveBattle, sMenusLiveBattle->ctx, BATTLER_PLAYER, sMenusQA.activePartySlot);
    BOOL ready = !field->taskman && FieldSystem_IsFieldMapRunning(field) && !FieldSystem_IsChildAppRunning(field) && IsPaletteFadeFinished() && field->location->mapId == 33;
    if (sMenusQA.fieldReady != ready) { sMenusQA.fieldReady = ready; ++sMenusQA.revision; }
    sMenusQA.fieldFrame = OS_GetVBlankCount();
    if (!ready || !sMenusQA.requested || sMenusQA.started) return;
    BattleMenusSemantics(field, FALSE); FieldDrawMapName_Reset(field->mapSystems->drawMapNameInfo);
    FieldSystem_CreateTask(field, BattleMenusQATask, NULL);
}
