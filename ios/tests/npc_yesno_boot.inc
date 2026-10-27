// SPDX-License-Identifier: GPL-3.0-or-later
/* Private source-copy observers. The resident owns active/generation/revision. */
#include "npc_yesno_qa.h"

#if defined(NPCYN_BOOT_TITLE_TU)
enum {
    NPCYN_BOOT_TITLE_WINDOW_LIVE = 1u << 0,
    NPCYN_BOOT_TITLE_BG_LIVE = 1u << 1,
    NPCYN_BOOT_TITLE_FADE_FINISHED = 1u << 2,
    NPCYN_BOOT_TITLE_BG_DRAINED = 1u << 3,
};

static HGYNBootFacts NPCYNBootTitleFacts(const TitleScreenOverlayData *data, int state) {
    HGYNBootFacts facts = {0};
    u32 progressFlags = 0;
    BOOL windowLive = data->animData.window.bgConfig != NULL && data->animData.window.pixelBuffer != NULL
        && data->animData.window.width != 0 && data->animData.window.height != 0;
    BOOL bgLive = data->bgConfig != NULL
        && data->bgConfig->bgs[GF_BG_LYR_MAIN_1].tilemapBuffer != NULL
        && data->bgConfig->bgs[GF_BG_LYR_MAIN_2].tilemapBuffer != NULL
        && data->bgConfig->bgs[GF_BG_LYR_MAIN_3].tilemapBuffer != NULL
        && data->bgConfig->bgs[GF_BG_LYR_SUB_1].tilemapBuffer != NULL
        && data->bgConfig->bgs[GF_BG_LYR_SUB_2].tilemapBuffer != NULL
        && data->bgConfig->bgs[GF_BG_LYR_SUB_3].tilemapBuffer != NULL;
    BOOL fadeFinished = IsPaletteFadeFinished() == TRUE;
    u16 pending = data->bgConfig != NULL
        ? data->bgConfig->scrollScheduled | data->bgConfig->bufferTransferScheduled : 0xFFFF;
    BOOL bgDrained = data->bgConfig != NULL
        && (pending & ((1u << GF_BG_LYR_MAIN_1) | (1u << GF_BG_LYR_MAIN_2) | (1u << GF_BG_LYR_MAIN_3)
                    | (1u << GF_BG_LYR_SUB_1) | (1u << GF_BG_LYR_SUB_2) | (1u << GF_BG_LYR_SUB_3))) == 0;
    if (windowLive) progressFlags |= NPCYN_BOOT_TITLE_WINDOW_LIVE;
    if (bgLive) progressFlags |= NPCYN_BOOT_TITLE_BG_LIVE;
    if (fadeFinished) progressFlags |= NPCYN_BOOT_TITLE_FADE_FINISHED;
    if (bgDrained) progressFlags |= NPCYN_BOOT_TITLE_BG_DRAINED;
    facts.kind = HG_YN_BOOT_TITLE;
    facts.phase = (u32)state;
    facts.notice = data->initialDelay;
    facts.exit = data->exitMode;
    facts.progressFlags = progressFlags;
    facts.ready = state == TITLESCREEN_MAIN_PLAY && data->initialDelay == 0
        && data->exitMode == TITLESCREEN_EXIT_UNSET && windowLive && bgLive && fadeFinished && bgDrained;
    return facts;
}

static void NPCYNBootPublishTitle(const TitleScreenOverlayData *data, int state) {
    HGYNBootFacts facts = NPCYNBootTitleFacts(data, state);
    NPCYNBootPublish(data, &facts);
}

static void NPCYNBootTitleTerminal(const TitleScreenOverlayData *data, int state) {
    HGYNBootFacts facts = NPCYNBootTitleFacts(data, state);
    facts.ready = 0;
    NPCYNBootPublish(data, &facts);
    NPCYNBootRetire(data);
}

#elif defined(NPCYN_BOOT_MAIN_MENU_TU)
enum {
    NPCYN_BOOT_MAIN_SELECTED_WINDOW_LIVE = 1u << 0,
    NPCYN_BOOT_MAIN_BG0_LIVE = 1u << 1,
    NPCYN_BOOT_MAIN_BG1_LIVE = 1u << 2,
    NPCYN_BOOT_MAIN_BG2_LIVE = 1u << 3,
    NPCYN_BOOT_MAIN_FADE_FINISHED = 1u << 4,
    NPCYN_BOOT_MAIN_STABLE_SCROLL = 1u << 5,
    NPCYN_BOOT_MAIN_NOTICE_QUIET = 1u << 6,
    NPCYN_BOOT_MAIN_DRAINED = 1u << 7,
};

static u32 NPCYNBootMainMenuProgress(const MainMenuAppData *data) {
    u32 progressFlags = 0;
    BOOL bg0Live = data->bgConfig != NULL && data->bgConfig->bgs[GF_BG_LYR_MAIN_0].tilemapBuffer != NULL;
    BOOL bg1Live = data->bgConfig != NULL && data->bgConfig->bgs[GF_BG_LYR_MAIN_1].tilemapBuffer != NULL;
    BOOL bg2Live = data->bgConfig != NULL && data->bgConfig->bgs[GF_BG_LYR_MAIN_2].tilemapBuffer != NULL;
    BOOL selectedWindowLive = data->currentOption < APPOPTION_COUNT
        && data->optionWindows[data->currentOption].bgConfig != NULL
        && data->optionWindows[data->currentOption].pixelBuffer != NULL
        && data->optionWindows[data->currentOption].width != 0
        && data->optionWindows[data->currentOption].height != 0;
    BOOL bgDrained = data->bgConfig != NULL
        && ((data->bgConfig->scrollScheduled | data->bgConfig->bufferTransferScheduled) & 0x7) == 0;
    if (selectedWindowLive) progressFlags |= NPCYN_BOOT_MAIN_SELECTED_WINDOW_LIVE;
    if (bg0Live) progressFlags |= NPCYN_BOOT_MAIN_BG0_LIVE;
    if (bg1Live) progressFlags |= NPCYN_BOOT_MAIN_BG1_LIVE;
    if (bg2Live) progressFlags |= NPCYN_BOOT_MAIN_BG2_LIVE;
    if (IsPaletteFadeFinished() == TRUE) progressFlags |= NPCYN_BOOT_MAIN_FADE_FINISHED;
    if (data->currentScreenY == data->effectiveScreenY) progressFlags |= NPCYN_BOOT_MAIN_STABLE_SCROLL;
    if (data->noticeState == 15 && data->noticeDelay == 0 && data->noticePending == FALSE) {
        progressFlags |= NPCYN_BOOT_MAIN_NOTICE_QUIET;
    }
    if (bgDrained) progressFlags |= NPCYN_BOOT_MAIN_DRAINED;
    return progressFlags;
}

static HGYNBootFacts NPCYNBootMainMenuFacts(const MainMenuAppData *data, int state) {
    HGYNBootFacts facts = {0};
    BOOL selectedContinue = data->currentOption < APPOPTION_COUNT
        && data->optionIds[data->currentOption] == APPOPTION_CONTINUE;
    BOOL saveDataPresent = data->saveData != NULL && data->dontHaveSavedata == FALSE;
    u32 progressFlags = NPCYNBootMainMenuProgress(data);
    u32 requiredProgress = NPCYN_BOOT_MAIN_SELECTED_WINDOW_LIVE
        | NPCYN_BOOT_MAIN_BG0_LIVE | NPCYN_BOOT_MAIN_BG1_LIVE | NPCYN_BOOT_MAIN_BG2_LIVE
        | NPCYN_BOOT_MAIN_FADE_FINISHED | NPCYN_BOOT_MAIN_STABLE_SCROLL | NPCYN_BOOT_MAIN_NOTICE_QUIET
        | NPCYN_BOOT_MAIN_DRAINED;
    facts.kind = HG_YN_BOOT_MAIN_MENU;
    facts.phase = (u32)state;
    facts.inputMode = (u32)data->menuInputState.state;
    facts.option = data->currentOption;
    facts.notice = data->noticeState;
    facts.exit = data->selectedApp;
    facts.progressFlags = progressFlags;
    facts.ready = state == 5 && selectedContinue && saveDataPresent && data->noticeState == 15
        && data->noticeDelay == 0 && data->noticePending == FALSE && data->redrawOptions == FALSE
        && data->optionChangeTimer == 0 && data->currentScreenY == data->effectiveScreenY
        && data->menuInputState.state <= MENU_INPUT_STATE_TOUCH
        && (progressFlags & requiredProgress) == requiredProgress;
    return facts;
}

static void NPCYNBootPublishMainMenu(const MainMenuAppData *data, int state) {
    // Retirement retains the last observed exit milestone, not the later graphics-free state.
    // If case 8 was never observed, the host's phase-8 exit witness still fails closed.
    if (state == 7) return;
    HGYNBootFacts facts = NPCYNBootMainMenuFacts(data, state);
    NPCYNBootPublish(data, &facts);
}

static void NPCYNBootMainMenuTerminal(const MainMenuAppData *data, int state) {
    (void)state;
    NPCYNBootRetire(data);
}

#else
#error "npc_yesno_boot.inc requires one private source-copy TU selector"
#endif
