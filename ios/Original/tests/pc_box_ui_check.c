// SPDX-License-Identifier: GPL-3.0-or-later
#include "native_pc_box_ui.h"
#include <stddef.h>
#include <string.h>

#ifdef HG_NATIVE_PC_BOX_UI_STATE_CHECK
// Private fixture for the actual resident block; never a shipping export.
extern HGNativePCBoxUIState gNativePCBoxUIState;
#endif
#define CHECK(value) do { if (!(value)) return __LINE__; } while (0)

uint32_t CheckNativePCBoxUI(void) {
    CHECK(sizeof(HGNativePCBoxUIState) == 52 && offsetof(HGNativePCBoxUIState, active) == 0
        && offsetof(HGNativePCBoxUIState, ownerGeneration) == 4 && offsetof(HGNativePCBoxUIState, revision) == 8
        && offsetof(HGNativePCBoxUIState, ready) == 12 && offsetof(HGNativePCBoxUIState, phase) == 16
        && offsetof(HGNativePCBoxUIState, mode) == 20 && offsetof(HGNativePCBoxUIState, activeBox) == 24
        && offsetof(HGNativePCBoxUIState, selectedSlot) == 28 && offsetof(HGNativePCBoxUIState, partyPanelOpen) == 32
        && offsetof(HGNativePCBoxUIState, trayOpen) == 36 && offsetof(HGNativePCBoxUIState, actionMenuOpen) == 40
        && offsetof(HGNativePCBoxUIState, cursorHolding) == 44 && offsetof(HGNativePCBoxUIState, viewActive) == 48);
    const uint32_t phases[] = {6, 7, 12, 22, 36, 41, 61, 66, 81, 91, 97, 115, 117, 130, 136, 139};
    for (uint32_t phase = 0; phase <= 180; ++phase) {
        uint32_t allowed = 0;
        for (size_t i = 0; i < sizeof(phases) / sizeof(phases[0]); ++i) allowed |= phase == phases[i];
        CHECK(HGNativePCBoxUIIsInputPhase(phase) == allowed);
    }
    CHECK(!HGNativePCBoxUIIsInputPhase(UINT32_MAX));
    HGNativePCBoxUIState local = {1, 7, 9, 1, HG_PC_BOX_UI_MOVE_MODE_INPUT, 2, 17, 255, 0, 1, 1, 0, 1};
    HGNativePCBoxUIState before = local;
    const uint32_t keys[] = {1, 2, 16, 32, 64, 128, 2048};
    uint32_t output = 123;
    for (size_t i = 0; i < sizeof(phases) / sizeof(phases[0]); ++i) {
        local.phase = phases[i];
        for (size_t j = 0; j < sizeof(keys) / sizeof(keys[0]); ++j) {
            before = local;
            CHECK(HGNativePCBoxUIResolveInput(&local, 7, 9, keys[j], &output) && output == keys[j]);
            CHECK(!memcmp(&local, &before, sizeof(local)));
        }
    }
    local = before;
    output = 123;
    CHECK(!HGNativePCBoxUIResolveInput(NULL, 7, 9, 1, &output));
    CHECK(!HGNativePCBoxUIResolveInput(&local, 7, 9, 1, NULL));
    CHECK(!HGNativePCBoxUIResolveInput(&local, 0, 9, 1, &output)
        && !HGNativePCBoxUIResolveInput(&local, 7, 0, 1, &output)
        && !HGNativePCBoxUIResolveInput(&local, 6, 9, 1, &output)
        && !HGNativePCBoxUIResolveInput(&local, 7, 8, 1, &output));
    const uint32_t rejectedKeys[] = {0, 3, 4, 8, 48, 256, 512, 1024, 2049, UINT32_MAX};
    for (size_t i = 0; i < sizeof(rejectedKeys) / sizeof(rejectedKeys[0]); ++i)
        CHECK(!HGNativePCBoxUIResolveInput(&local, 7, 9, rejectedKeys[i], &output));
    CHECK(output == 123 && !memcmp(&local, &before, sizeof(local)));
    const struct { size_t offset; uint32_t value; } invalid[] = {
        {offsetof(HGNativePCBoxUIState, active), 0}, {offsetof(HGNativePCBoxUIState, active), 2},
        {offsetof(HGNativePCBoxUIState, ready), 0}, {offsetof(HGNativePCBoxUIState, ready), 2},
        {offsetof(HGNativePCBoxUIState, viewActive), 0}, {offsetof(HGNativePCBoxUIState, viewActive), 2},
        {offsetof(HGNativePCBoxUIState, phase), 0}, {offsetof(HGNativePCBoxUIState, phase), 10},
        {offsetof(HGNativePCBoxUIState, mode), 4}, {offsetof(HGNativePCBoxUIState, activeBox), 18},
        {offsetof(HGNativePCBoxUIState, selectedSlot), 36}, {offsetof(HGNativePCBoxUIState, selectedSlot), 254},
        {offsetof(HGNativePCBoxUIState, partyPanelOpen), 2}, {offsetof(HGNativePCBoxUIState, trayOpen), 2},
        {offsetof(HGNativePCBoxUIState, actionMenuOpen), 2}, {offsetof(HGNativePCBoxUIState, cursorHolding), 2}
    };
    for (size_t i = 0; i < sizeof(invalid) / sizeof(invalid[0]); ++i) {
        local = before;
        memcpy((unsigned char *)&local + invalid[i].offset, &invalid[i].value, sizeof(uint32_t));
        HGNativePCBoxUIState invalidBefore = local;
        CHECK(!HGNativePCBoxUIResolveInput(&local, 7, 9, 1, &output) && output == 123);
        CHECK(!memcmp(&local, &invalidBefore, sizeof(local)));
    }
    local = before;
    local.ownerGeneration = local.revision = UINT32_MAX;
    CHECK(HGNativePCBoxUIResolveInput(&local, UINT32_MAX, UINT32_MAX, 1, &output) && output == 1);

#ifdef HG_NATIVE_PC_BOX_UI_STATE_CHECK
    CHECK(sizeof(NativePCBoxUISelection) == 28 && offsetof(NativePCBoxUISelection, wallpaperSelection) == 24);
    NativePCBoxUISelection selection = {0};
    const HGNativePCBoxUIState *state = HGNativePCBoxUIGetState();
    before = *state;
    NativePCBoxUIBegin(NULL); NativePCBoxUIBeginView(NULL); NativePCBoxUIEndView(NULL); NativePCBoxUIRetire(NULL);
    CHECK(HGNativePCBoxUIGetState() == state && !memcmp(state, &before, sizeof(before)));
    int firstOwner = 1, otherOwner = 2;
    NativePCBoxUIBegin(&firstOwner);
    CHECK(state->active == 1 && !state->ready && !state->viewActive && state->ownerGeneration && state->revision);
    before = *state;
    NativePCBoxUIBegin(&firstOwner); NativePCBoxUIBeginView(&otherOwner); NativePCBoxUIEndView(&otherOwner);
    NativePCBoxUIPublish(&otherOwner, &local, &selection); NativePCBoxUIRetire(&otherOwner);
    CHECK(!memcmp(state, &before, sizeof(before)));
    NativePCBoxUIBeginView(&firstOwner);
    CHECK(state->viewActive == 1 && !state->ready && state->ownerGeneration != before.ownerGeneration && state->revision != before.revision);
    uint32_t generation = state->ownerGeneration, revision = state->revision;
    before = *state;
    NativePCBoxUIBeginView(&firstOwner); NativePCBoxUIPublish(&firstOwner, NULL, &selection);
    NativePCBoxUIPublish(&firstOwner, &local, NULL);
    CHECK(!memcmp(state, &before, sizeof(before)));
    local = (HGNativePCBoxUIState){ .ready = 1, .phase = HG_PC_BOX_UI_MOVE_MODE_INPUT, .mode = 2, .selectedSlot = 255 };
    NativePCBoxUIPublish(&firstOwner, &local, &selection);
    CHECK(state->ready == 1 && state->active == 1 && state->viewActive == 1 && state->revision != revision
        && state->ownerGeneration == generation && HGNativePCBoxUIResolveInput(state, generation, state->revision, 1, &output));
    revision = state->revision; before = *state;
    NativePCBoxUIPublish(&firstOwner, &local, &selection);
    CHECK(!memcmp(state, &before, sizeof(before)));
    const struct { size_t offset; uint32_t value, phase; } choices[] = {
        {offsetof(NativePCBoxUISelection, gridNextInput), 37, HG_PC_BOX_UI_MOVE_MODE_INPUT},
        {offsetof(NativePCBoxUISelection, gridIsButtons), 1, HG_PC_BOX_UI_MOVE_MODE_INPUT},
        {offsetof(NativePCBoxUISelection, yesNoCursorPos), 1, HG_PC_BOX_UI_HANDLE_YES_NO},
        {offsetof(NativePCBoxUISelection, yesNoInTouchMode), 1, HG_PC_BOX_UI_HANDLE_YES_NO},
        {offsetof(NativePCBoxUISelection, markings), 1, HG_PC_BOX_UI_MARKING_MENU_INPUT},
        {offsetof(NativePCBoxUISelection, trayBox), 17, HG_PC_BOX_UI_BOX_MENU_INPUT},
        {offsetof(NativePCBoxUISelection, wallpaperSelection), 23, HG_PC_BOX_UI_WALLPAPER_MENU_INPUT}
    };
    for (size_t i = 0; i < sizeof(choices) / sizeof(choices[0]); ++i) {
        local.phase = choices[i].phase;
        NativePCBoxUIPublish(&firstOwner, &local, &selection);
        before = *state;
        memcpy((unsigned char *)&selection + choices[i].offset, &choices[i].value, sizeof(uint32_t));
        NativePCBoxUIPublish(&firstOwner, &local, &selection);
        CHECK(state->revision != before.revision && state->selectedSlot == 255
            && !HGNativePCBoxUIResolveInput(state, generation, before.revision, 1, &output)
            && HGNativePCBoxUIResolveInput(state, generation, state->revision, 1, &output));
        before.revision = state->revision;
        CHECK(!memcmp(state, &before, sizeof(before))); // Only the lease revision changes for each of the seven facts.
        NativePCBoxUIPublish(&firstOwner, &local, &selection);
        CHECK(!memcmp(state, &before, sizeof(before)));
    }
    revision = state->revision;
    local.selectedSlot = 3; NativePCBoxUIPublish(&firstOwner, &local, &selection);
    CHECK(state->selectedSlot == 3 && state->revision != revision
        && !HGNativePCBoxUIResolveInput(state, generation, revision, 1, &output));
    local.ready = 0; NativePCBoxUIPublish(&firstOwner, &local, &selection);
    CHECK(state->active == 1 && state->viewActive == 1 && !state->ready);
    local.ready = 1; local.phase = 5; NativePCBoxUIPublish(&firstOwner, &local, &selection);
    CHECK(!state->ready);
    local.phase = HG_PC_BOX_UI_MOVE_MODE_INPUT; NativePCBoxUIPublish(&firstOwner, &local, &selection);
    revision = state->revision;
    NativePCBoxUIEndView(&firstOwner);
    CHECK(state->active == 1 && !state->ready && !state->viewActive && state->revision != revision
        && !HGNativePCBoxUIResolveInput(state, generation, revision, 1, &output));
    before = *state;
    NativePCBoxUIEndView(&firstOwner);
    CHECK(!memcmp(state, &before, sizeof(before)));
    NativePCBoxUIPublish(&firstOwner, &local, &selection);
    CHECK(!state->ready && !state->viewActive && state->active == 1);
    NativePCBoxUIBeginView(&firstOwner); // Child Summary/Bag/Naming returned to a rebuilt view.
    CHECK(state->ownerGeneration != generation && !state->ready && state->viewActive == 1);
    NativePCBoxUIPublish(&firstOwner, &local, &selection);
    CHECK(!HGNativePCBoxUIResolveInput(state, generation, state->revision, 1, &output));
    NativePCBoxUIRetire(&firstOwner);
    CHECK(!state->active && !state->ready && !state->viewActive);
    before = *state;
    NativePCBoxUIRetire(&firstOwner); NativePCBoxUIPublish(&firstOwner, &local, &selection); NativePCBoxUIBeginView(&firstOwner);
    CHECK(!memcmp(state, &before, sizeof(before)));
    NativePCBoxUIBegin(&firstOwner); // Reused allocator address starts a fresh app owner.
    CHECK(state->active == 1 && !state->ready && !state->viewActive && state->ownerGeneration != before.ownerGeneration);
    gNativePCBoxUIState.ownerGeneration = gNativePCBoxUIState.revision = UINT32_MAX;
    NativePCBoxUIBeginView(&firstOwner);
    CHECK(state->ownerGeneration == 1 && state->revision == 1);
    gNativePCBoxUIState.revision = UINT32_MAX;
    NativePCBoxUIPublish(&firstOwner, &local, &selection);
    CHECK(state->revision == 1 && state->ready == 1);
    gNativePCBoxUIState.revision = UINT32_MAX; NativePCBoxUIEndView(&firstOwner);
    CHECK(state->revision == 1 && state->active == 1 && !state->viewActive && !state->ready);
    gNativePCBoxUIState.ownerGeneration = gNativePCBoxUIState.revision = UINT32_MAX;
    NativePCBoxUIBegin(&otherOwner);
    CHECK(state->ownerGeneration == 1 && state->revision == 1 && state->active == 1 && !state->viewActive);
    gNativePCBoxUIState.revision = UINT32_MAX; NativePCBoxUIRetire(&otherOwner);
    CHECK(state->revision == 1 && !state->active && !state->viewActive && !state->ready);
#endif
    return 0;
}

#ifdef HG_NATIVE_PC_BOX_UI_CHECK_MAIN
#include <stdio.h>
int main(void) {
    uint32_t line = CheckNativePCBoxUI();
    if (line) { fprintf(stderr, "PC Box UI check line %u\n", line); return 1; }
#ifdef HG_NATIVE_PC_BOX_UI_STATE_CHECK
    puts("PASS: PC Box52 ABI, seven exact selection facts, resident lifetimes, stale input rejection and nonzero counter wrap.");
#else
    puts("PASS: PC Box52 ABI, original input phases and strict/read-only key qualification.");
#endif
    return 0;
}
#endif
