// SPDX-License-Identifier: GPL-3.0-or-later
// Private TM learning check only; the formal guest never includes this file.
#ifndef HG_SUMMARY_LEARN_QA_TYPES
#define HG_SUMMARY_LEARN_QA_TYPES
#include <stdint.h>
#include "native_party_ui.h"
enum { HG_SUMMARY_LEARN_QA_VERSION = 1, HG_SUMMARY_LEARN_BAG = 1, HG_SUMMARY_LEARN_PARTY = 2, HG_SUMMARY_LEARN_SUMMARY = 3 };
typedef struct HGSummaryLearnQAState {
    uint32_t version, byteSize, revision, frame, fieldReady, owner, generation, manager, data, execState, phase;
    uint32_t context, taskState, itemId, moveId, partySlot, selectedAction, selectedMoveSlot;
    uint32_t printerId, printerActive, printerMatches, printerState, printerWaiting, afterTextState;
    uint32_t yesNoActive, yesNoResult, yesNoTimer, yesNoCursor, yesNoX, yesNoY, yesNoWidth, yesNoHeight;
    uint32_t graphicsReady, fadeFinished, hmCalls, hmOwner, hmMoveSlot;
    uint32_t yesNoReady, yesHitX[2], yesHitY[2], partyBusy, tmTaskMatches;
    uint32_t mapSection;
    HGNativePartyUIWindow hmWarning;
} HGSummaryLearnQAState;
#endif

#ifdef HG_SUMMARY_LEARN_TEXT_OBSERVER
// Appended after the unchanged original text.c, where its private task table is visible.
void SummaryLearnReadPrinter(uint32_t id, const Window *window, HGSummaryLearnQAState *out) {
    out->printerId = id;
    if (id >= MAX_TEXT_PRINTERS || sTextPrinterTasks[id] == NULL) return;
    out->printerActive = TRUE;
    const TextPrinter *printer = SysTask_GetData(sTextPrinterTasks[id]);
    if (printer == NULL || printer->template.window != window || printer->id != id) return;
    out->printerMatches = TRUE;
    out->printerState = printer->state;
    out->printerWaiting = printer->active && printer->state == 2 && !printer->callbackResult && !printer->callbackParam;
}
#endif

#ifdef HG_SUMMARY_LEARN_BAG_OBSERVER
// Appended after the unchanged original bag_app.c; its static task remains private.
BOOL SummaryLearnBagTaskMatches(const BagApp *app) { return app->task == BagApp_UseTMHMTask; }
#endif

#ifdef HG_SUMMARY_LEARN_FIELD_OBSERVER
#include "global.h"
#include "application/bag/bag_app.h"
#include "field_system.h"
#include "map_header.h"
#include "overlay_manager.h"
#include "overlay_15.h"
#include "party_menu.h"
#include "palette_vram_addr.h"
#include "pokemon_summary_internal.h"
#include "screen_fade.h"
#include "yes_no_prompt.h"

extern const OverlayManagerTemplate gOverlayTemplate_Bag;
FS_EXTERN_OVERLAY(OVY_15);
// Field/BerryPots launch OVY_15; the already-loaded global Bag template uses NONE.
static const OverlayManagerTemplate sSummaryLearnFieldBagTemplate = { Bag_Init, Bag_Main, Bag_Exit, FS_OVERLAY_ID(OVY_15) };
void SummaryLearnReadPrinter(uint32_t id, const Window *window, HGSummaryLearnQAState *out);
BOOL SummaryLearnBagTaskMatches(const BagApp *app);
static HGSummaryLearnQAState sSummaryLearnQA;
static u32 sSummaryLearnHMCalls, sSummaryLearnHMOwner, sSummaryLearnHMSlot;

void __real_PokemonSummary_PrintCantForgetHMMessage(PokemonSummary *summary);
void __wrap_PokemonSummary_PrintCantForgetHMMessage(PokemonSummary *summary) {
    __real_PokemonSummary_PrintCantForgetHMMessage(summary);
    ++sSummaryLearnHMCalls;
    sSummaryLearnHMOwner = (u32)(uintptr_t)summary;
    sSummaryLearnHMSlot = summary->moveSlot;
}

static BOOL SummaryLearnTemplate(const OverlayManager *manager, const OverlayManagerTemplate *template) {
    return manager->template.init == template->init && manager->template.exec == template->exec
        && manager->template.exit == template->exit && manager->template.ovy_id == template->ovy_id;
}

static void SummaryLearnPrompt(const YesNoPrompt *prompt, HGSummaryLearnQAState *out) {
    if (prompt == NULL) return;
    out->yesNoActive = prompt->buttonsAreInit;
    out->yesNoResult = prompt->result;
    out->yesNoTimer = prompt->confirmSelectionTimer;
    out->yesNoCursor = prompt->cursorPos;
    out->yesNoX = prompt->x; out->yesNoY = prompt->y;
    out->yesNoWidth = prompt->width; out->yesNoHeight = prompt->height;
    out->yesNoReady = prompt->buttonsAreInit == 1 && prompt->touchHitboxController != NULL
        && prompt->result == 3 && !prompt->confirmSelectionTimer && !prompt->ignoreTouch;
    for (u32 i = 0; i < 2; ++i) {
        out->yesHitX[i] = (prompt->hitboxes[i].rect.left + prompt->hitboxes[i].rect.right) / 2;
        out->yesHitY[i] = (prompt->hitboxes[i].rect.top + prompt->hitboxes[i].rect.bottom) / 2;
    }
}

static void SummaryLearnWindow(const Window *window, HGNativePartyUIWindow *out) {
    if (window == NULL || window->bgConfig == NULL || window->pixelBuffer == NULL || window->bgId >= 8
        || window->colorMode != GF_BG_CLR_4BPP || window->paletteNum >= 16
        || window->width != 15 || window->height != 10 || window->bgId != GF_BG_LYR_SUB_0) return;
    out->pixelBuffer = (u32)(uintptr_t)window->pixelBuffer;
    out->tileWidth = window->width; out->tileHeight = window->height;
    out->engine = window->bgId >= GF_BG_LYR_SUB_0;
    const u16 *palette = out->engine ? GetSubBgPlttAddr() : GetMainBgPlttAddr();
    MI_CpuCopy16(palette + window->paletteNum * 16, out->palette, sizeof(out->palette));
}

static void SummaryLearnObserve(FieldSystem *field) {
    HGSummaryLearnQAState next = { .version = HG_SUMMARY_LEARN_QA_VERSION, .byteSize = sizeof(next),
        .revision = sSummaryLearnQA.revision, .generation = sSummaryLearnQA.generation };
    next.fieldReady = !field->taskman && FieldSystem_IsFieldMapRunning(field) && !FieldSystem_IsChildAppRunning(field)
        && IsPaletteFadeFinished();
    next.fadeFinished = IsPaletteFadeFinished();
    if (field->location) next.mapSection = MapHeader_GetMapSec(field->location->mapId);
    next.hmCalls = sSummaryLearnHMCalls; next.hmOwner = sSummaryLearnHMOwner; next.hmMoveSlot = sSummaryLearnHMSlot;
    OverlayManager *child = field->processManager->child;
    if (child != NULL) {
        next.manager = (u32)(uintptr_t)child; next.execState = child->exec_state; next.phase = child->proc_state;
        if (SummaryLearnTemplate(child, &gOverlayTemplate_Bag) || SummaryLearnTemplate(child, &sSummaryLearnFieldBagTemplate)) next.owner = HG_SUMMARY_LEARN_BAG;
        else if (SummaryLearnTemplate(child, &gOverlayTemplate_PartyMenu)) next.owner = HG_SUMMARY_LEARN_PARTY;
        else if (SummaryLearnTemplate(child, &gOverlayTemplate_PokemonSummary)) next.owner = HG_SUMMARY_LEARN_SUMMARY;
        if (next.owner && child->exec_state == 2 && child->data) {
            next.data = (u32)(uintptr_t)child->data;
            BgConfig *bg = NULL;
            if (next.owner == HG_SUMMARY_LEARN_BAG) {
                BagApp *app = child->data; bg = app->bgConfig;
                next.taskState = app->taskState; next.itemId = app->bagView->itemId;
                // Only the real TM task owns this phase, item pocket and message window.
                if (next.phase == 13 && app->bagView->mode == 0
                    && app->bagView->pocketIndex < 8
                    && app->bagView->pockets[app->bagView->pocketIndex].pocketId == POCKET_TMHMS
                    && SummaryLearnBagTaskMatches(app)) {
                    next.tmTaskMatches = TRUE;
                    SummaryLearnReadPrinter(app->printerId, &app->windows[3], &next);
                    if (app->taskState == 3) SummaryLearnPrompt(app->yesNoPrompt, &next);
                }
            } else if (next.owner == HG_SUMMARY_LEARN_PARTY) {
                PartyMenu *menu = child->data; bg = menu->bgConfig;
                next.context = menu->args->context; next.itemId = menu->args->itemId; next.moveId = menu->args->moveId;
                next.partySlot = menu->partyMonIndex; next.selectedAction = menu->args->selectedAction;
                next.selectedMoveSlot = menu->args->selectedMoveIdx; next.afterTextState = menu->afterTextPrinterState;
                next.partyBusy = menu->contextMenuButtonAnim.active || menu->swapMonsData.active;
                if (next.phase == PARTY_MENU_STATE_WAIT_TEXT_PRINTER)
                    SummaryLearnReadPrinter(menu->textPrinterId, &menu->windows[PARTY_MENU_WINDOW_ID_34], &next);
                if (next.phase == PARTY_MENU_STATE_YES_NO_HANDLE_INPUT) SummaryLearnPrompt(menu->yesNoPrompt, &next);
            } else {
                PokemonSummary *summary = child->data; bg = summary->bgConfig;
                next.context = summary->args->mode; next.moveId = summary->args->moveToLearn;
                next.partySlot = summary->args->partySlot; next.selectedMoveSlot = summary->args->selectedMoveSlot;
                if (summary->page == 1 && summary->pageWindows && summary->numPageWindows > 15)
                    SummaryLearnWindow(&summary->pageWindows[15], &next.hmWarning);
            }
            next.graphicsReady = bg != NULL && !((bg->bufferTransferScheduled | bg->scrollScheduled) & 0xFF);
        }
    }
    if (next.owner != sSummaryLearnQA.owner || next.manager != sSummaryLearnQA.manager || next.data != sSummaryLearnQA.data)
        ++next.generation;
    // Frame changes are diagnostic only; an input lease changes only with its real owner/state.
    if (memcmp(&next.fieldReady, &sSummaryLearnQA.fieldReady, sizeof(next) - offsetof(HGSummaryLearnQAState, fieldReady))) ++next.revision;
    next.frame = OS_GetVBlankCount();
    sSummaryLearnQA = next;
}

const HGSummaryLearnQAState *HGNativeSummaryLearnQAGetState(void) { return &sSummaryLearnQA; }
#endif
