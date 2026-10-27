// SPDX-License-Identifier: GPL-3.0-or-later
#include "native_field_choice_ui.h"
#include <stddef.h>
#include <string.h>

#ifdef HG_NATIVE_FIELD_CHOICE_UI_STATE_CHECK
// Private fixture for the actual resident block; never a shipping export.
extern HGNativeFieldChoiceUIState gNativeFieldChoiceUIState;
#endif
#define CHECK(value) do { if (!(value)) return __LINE__; } while (0)

uint32_t CheckNativeFieldChoiceUI(void) {
    CHECK(sizeof(HGNativeFieldChoiceUIState) == 36 && offsetof(HGNativeFieldChoiceUIState, active) == 0
        && offsetof(HGNativeFieldChoiceUIState, ownerGeneration) == 4 && offsetof(HGNativeFieldChoiceUIState, revision) == 8
        && offsetof(HGNativeFieldChoiceUIState, ready) == 12 && offsetof(HGNativeFieldChoiceUIState, phase) == 16
        && offsetof(HGNativeFieldChoiceUIState, rowCount) == 20 && offsetof(HGNativeFieldChoiceUIState, selectedIndex) == 24
        && offsetof(HGNativeFieldChoiceUIState, cancelEnabled) == 28 && offsetof(HGNativeFieldChoiceUIState, viewActive) == 32);
    for (uint32_t phase = 0; phase <= 12; ++phase)
        CHECK(HGNativeFieldChoiceUIIsListPhase(phase) == (phase >= 7 && phase <= 10));
    CHECK(!HGNativeFieldChoiceUIIsListPhase(UINT32_MAX));
    HGNativeFieldChoiceUIState local = {1, 7, 9, 1, HG_FIELD_CHOICE_UI_LIST_INPUT, 8, 7, 1, 1}, before = local;
    uint32_t output = 123;
    const uint32_t keys[] = {1, 2, 16, 32, 64, 128};
    for (size_t i = 0; i < sizeof(keys) / sizeof(keys[0]); ++i) {
        CHECK(HGNativeFieldChoiceUIResolveInput(&local, 7, 9, keys[i], &output) && output == keys[i]);
        CHECK(!memcmp(&local, &before, sizeof(local)));
    }
    output = 123;
    CHECK(!HGNativeFieldChoiceUIResolveInput(NULL, 7, 9, 1, &output)
        && !HGNativeFieldChoiceUIResolveInput(&local, 7, 9, 1, NULL)
        && !HGNativeFieldChoiceUIResolveInput(&local, 0, 9, 1, &output)
        && !HGNativeFieldChoiceUIResolveInput(&local, 7, 0, 1, &output)
        && !HGNativeFieldChoiceUIResolveInput(&local, 6, 9, 1, &output)
        && !HGNativeFieldChoiceUIResolveInput(&local, 7, 8, 1, &output));
    const uint32_t rejectedKeys[] = {0, 3, 4, 8, 48, 256, 512, 1024, 2048, UINT32_MAX};
    for (size_t i = 0; i < sizeof(rejectedKeys) / sizeof(rejectedKeys[0]); ++i)
        CHECK(!HGNativeFieldChoiceUIResolveInput(&local, 7, 9, rejectedKeys[i], &output));
    CHECK(output == 123 && !memcmp(&local, &before, sizeof(local)));
    const struct { size_t offset; uint32_t value; } invalid[] = {
        {offsetof(HGNativeFieldChoiceUIState, active), 0}, {offsetof(HGNativeFieldChoiceUIState, active), 2},
        {offsetof(HGNativeFieldChoiceUIState, ready), 0}, {offsetof(HGNativeFieldChoiceUIState, ready), 2},
        {offsetof(HGNativeFieldChoiceUIState, viewActive), 0}, {offsetof(HGNativeFieldChoiceUIState, viewActive), 2},
        {offsetof(HGNativeFieldChoiceUIState, phase), 1}, {offsetof(HGNativeFieldChoiceUIState, phase), 4},
        {offsetof(HGNativeFieldChoiceUIState, phase), 7}, {offsetof(HGNativeFieldChoiceUIState, phase), 9},
        {offsetof(HGNativeFieldChoiceUIState, phase), 10}, {offsetof(HGNativeFieldChoiceUIState, phase), UINT32_MAX},
        {offsetof(HGNativeFieldChoiceUIState, rowCount), 0}, {offsetof(HGNativeFieldChoiceUIState, rowCount), 1},
        {offsetof(HGNativeFieldChoiceUIState, rowCount), 9}, {offsetof(HGNativeFieldChoiceUIState, rowCount), UINT32_MAX},
        {offsetof(HGNativeFieldChoiceUIState, selectedIndex), 8}, {offsetof(HGNativeFieldChoiceUIState, selectedIndex), UINT32_MAX},
        {offsetof(HGNativeFieldChoiceUIState, cancelEnabled), 2}
    };
    for (size_t i = 0; i < sizeof(invalid) / sizeof(invalid[0]); ++i) {
        local = before;
        memcpy((unsigned char *)&local + invalid[i].offset, &invalid[i].value, sizeof(uint32_t));
        HGNativeFieldChoiceUIState invalidBefore = local;
        CHECK(!HGNativeFieldChoiceUIResolveInput(&local, 7, 9, 1, &output) && output == 123);
        CHECK(!memcmp(&local, &invalidBefore, sizeof(local)));
    }
    local = before; local.cancelEnabled = 0;
    CHECK(!HGNativeFieldChoiceUIResolveInput(&local, 7, 9, 2, &output) && output == 123);
    CHECK(HGNativeFieldChoiceUIResolveInput(&local, 7, 9, 1, &output) && output == 1);
    local.rowCount = 2; local.selectedIndex = 1;
    CHECK(HGNativeFieldChoiceUIResolveInput(&local, 7, 9, 64, &output) && output == 64);
    local.ownerGeneration = local.revision = UINT32_MAX;
    CHECK(HGNativeFieldChoiceUIResolveInput(&local, UINT32_MAX, UINT32_MAX, 1, &output) && output == 1);

#ifdef HG_NATIVE_FIELD_CHOICE_UI_STATE_CHECK
    const HGNativeFieldChoiceUIState *state = HGNativeFieldChoiceUIGetState();
    before = *state;
    NativeFieldChoiceUIBegin(NULL); NativeFieldChoiceUIInvalidate(NULL); NativeFieldChoiceUIRetire(NULL);
    CHECK(HGNativeFieldChoiceUIGetState() == state && !memcmp(state, &before, sizeof(before)));
    int owner = 1, otherOwner = 2;
    NativeFieldChoiceUIBegin(&owner);
    CHECK(state->active == 1 && !state->ready && !state->viewActive && state->ownerGeneration && state->revision);
    before = *state;
    NativeFieldChoiceUIBegin(&owner); NativeFieldChoiceUIPublish(&owner, NULL);
    NativeFieldChoiceUIPublish(&otherOwner, &local); NativeFieldChoiceUIInvalidate(&otherOwner); NativeFieldChoiceUIRetire(&otherOwner);
    CHECK(!memcmp(state, &before, sizeof(before)));
    uint32_t generation = state->ownerGeneration, revision = state->revision;
    local = (HGNativeFieldChoiceUIState){ .ready = 1, .phase = HG_FIELD_CHOICE_UI_LIST_INPUT,
        .rowCount = 4, .selectedIndex = 1, .cancelEnabled = 1, .viewActive = 1 };
    NativeFieldChoiceUIPublish(&owner, &local);
    CHECK(state->active == 1 && state->viewActive == 1 && state->ready == 1 && state->revision != revision
        && HGNativeFieldChoiceUIResolveInput(state, generation, state->revision, 1, &output));
    before = *state; NativeFieldChoiceUIPublish(&owner, &local);
    CHECK(!memcmp(state, &before, sizeof(before)));
    const struct { size_t offset; uint32_t value; } changes[] = {
        {offsetof(HGNativeFieldChoiceUIState, selectedIndex), 2},
        {offsetof(HGNativeFieldChoiceUIState, cancelEnabled), 0},
        {offsetof(HGNativeFieldChoiceUIState, rowCount), 8}
    };
    for (size_t i = 0; i < sizeof(changes) / sizeof(changes[0]); ++i) {
        before = *state;
        memcpy((unsigned char *)&local + changes[i].offset, &changes[i].value, sizeof(uint32_t));
        NativeFieldChoiceUIPublish(&owner, &local);
        CHECK(state->phase == HG_FIELD_CHOICE_UI_LIST_INPUT && state->ready == 1 && state->revision != before.revision
            && !HGNativeFieldChoiceUIResolveInput(state, generation, before.revision, 1, &output)
            && HGNativeFieldChoiceUIResolveInput(state, generation, state->revision, 1, &output));
        before = *state; NativeFieldChoiceUIPublish(&owner, &local);
        CHECK(!memcmp(state, &before, sizeof(before)));
    }
    CHECK(!HGNativeFieldChoiceUIResolveInput(state, generation, state->revision, 2, &output));
    local.selectedIndex = 8; NativeFieldChoiceUIPublish(&owner, &local);
    CHECK(!state->ready && state->viewActive == 1);
    local.selectedIndex = 7; local.cancelEnabled = 2; NativeFieldChoiceUIPublish(&owner, &local);
    CHECK(!state->ready);
    local.cancelEnabled = 1; local.phase = HG_FIELD_CHOICE_UI_LIST_END; NativeFieldChoiceUIPublish(&owner, &local);
    CHECK(!state->ready && state->viewActive == 1);
    local.phase = 4; NativeFieldChoiceUIPublish(&owner, &local);
    CHECK(!state->ready && !state->viewActive && state->active == 1);
    local.phase = 1; NativeFieldChoiceUIPublish(&owner, &local);
    CHECK(!state->ready && !state->viewActive && state->active == 1); // Idle ownership must leave dialogue to the field.
    local.phase = HG_FIELD_CHOICE_UI_LIST_INPUT; NativeFieldChoiceUIPublish(&owner, &local);
    revision = state->revision;
    NativeFieldChoiceUIInvalidate(&owner);
    CHECK(state->active == 1 && !state->ready && !state->viewActive && state->revision != revision
        && !HGNativeFieldChoiceUIResolveInput(state, generation, revision, 1, &output));
    before = *state; NativeFieldChoiceUIInvalidate(&owner);
    CHECK(state->revision != before.revision && !state->ready && !state->viewActive);
    NativeFieldChoiceUIPublish(&owner, &local); // Same phase/list after a synchronous manager/state invalidation.
    CHECK(state->ready == 1 && !HGNativeFieldChoiceUIResolveInput(state, generation, revision, 1, &output));
    NativeFieldChoiceUIRetire(&owner);
    CHECK(!state->active && !state->ready && !state->viewActive && state->ownerGeneration == generation);
    before = *state;
    NativeFieldChoiceUIRetire(&owner); NativeFieldChoiceUIInvalidate(&owner); NativeFieldChoiceUIPublish(&owner, &local);
    CHECK(!memcmp(state, &before, sizeof(before)));
    NativeFieldChoiceUIBegin(&owner); // Same allocator address after destruction is a new owner.
    CHECK(state->active == 1 && !state->viewActive && !state->ready && state->ownerGeneration != generation);
    NativeFieldChoiceUIPublish(&owner, &local);
    CHECK(!HGNativeFieldChoiceUIResolveInput(state, generation, state->revision, 1, &output));
    gNativeFieldChoiceUIState.ownerGeneration = gNativeFieldChoiceUIState.revision = UINT32_MAX;
    NativeFieldChoiceUIBegin(&otherOwner);
    CHECK(state->ownerGeneration == 1 && state->revision == 1 && !state->ready && !state->viewActive);
    gNativeFieldChoiceUIState.revision = UINT32_MAX; NativeFieldChoiceUIPublish(&otherOwner, &local);
    CHECK(state->revision == 1 && state->ready == 1);
    gNativeFieldChoiceUIState.revision = UINT32_MAX; NativeFieldChoiceUIInvalidate(&otherOwner);
    CHECK(state->revision == 1 && !state->ready && !state->viewActive && state->active == 1);
    gNativeFieldChoiceUIState.revision = UINT32_MAX; NativeFieldChoiceUIRetire(&otherOwner);
    CHECK(state->revision == 1 && !state->active && !state->ready && !state->viewActive);
#endif
    return 0;
}

#ifdef HG_NATIVE_FIELD_CHOICE_UI_CHECK_MAIN
#include <stdio.h>
int main(void) {
    uint32_t line = CheckNativeFieldChoiceUI();
    if (line) { fprintf(stderr, "Field choice UI check line %u\n", line); return 1; }
#ifdef HG_NATIVE_FIELD_CHOICE_UI_STATE_CHECK
    puts("PASS: field choice36 ABI, resident lifetimes, same-phase selection and synchronous invalidation, nonzero counter wrap.");
#else
    puts("PASS: field choice36 ABI, list-only ownership and strict/read-only original key qualification.");
#endif
    return 0;
}
#endif
