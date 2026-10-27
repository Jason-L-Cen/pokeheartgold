// SPDX-License-Identifier: GPL-3.0-or-later
#include "native_options_ui.h"
#include <stddef.h>
#include <string.h>

// Private unit fixture, following the capture-state check; this data is not a Wasm export.
#ifdef HG_NATIVE_OPTIONS_UI_STATE_CHECK
extern HGNativeOptionsUIState gNativeOptionsUIState;
#endif
#define CHECK(value) do { if (!(value)) return __LINE__; } while (0)

uint32_t CheckNativeOptionsUI(void) {
    CHECK(sizeof(HGNativeOptionsUIState) == 16 && offsetof(HGNativeOptionsUIState, active) == 0
        && offsetof(HGNativeOptionsUIState, ownerGeneration) == 4
        && offsetof(HGNativeOptionsUIState, revision) == 8 && offsetof(HGNativeOptionsUIState, ready) == 12);
    HGNativeOptionsUIState local = {1, 7, 9, 1}, before = local;
    CHECK(HGNativeOptionsUIInputAllowed(&local, 7, 9) == 1 && !memcmp(&local, &before, sizeof(local)));
    CHECK(!HGNativeOptionsUIInputAllowed(NULL, 7, 9));
    CHECK(!HGNativeOptionsUIInputAllowed(&local, 0, 9) && !HGNativeOptionsUIInputAllowed(&local, 7, 0));
    CHECK(!HGNativeOptionsUIInputAllowed(&local, 6, 9) && !HGNativeOptionsUIInputAllowed(&local, 7, 8));
    CHECK(!memcmp(&local, &before, sizeof(local)));
    HGNativeOptionsUIState invalid[] = {
        {0, 7, 9, 1}, {2, 7, 9, 1}, {UINT32_MAX, 7, 9, 1},
        {1, 7, 9, 0}, {1, 7, 9, 2}, {1, 7, 9, UINT32_MAX},
        {1, 0, 9, 1}, {1, 7, 0, 1}, {0, 0, 0, 0}
    };
    for (size_t i = 0; i < sizeof(invalid) / sizeof(invalid[0]); ++i) {
        before = invalid[i];
        CHECK(!HGNativeOptionsUIInputAllowed(&invalid[i], invalid[i].ownerGeneration, invalid[i].revision));
        CHECK(!memcmp(&invalid[i], &before, sizeof(before)));
    }
    local = (HGNativeOptionsUIState){1, UINT32_MAX, UINT32_MAX, 1}; before = local;
    CHECK(HGNativeOptionsUIInputAllowed(&local, UINT32_MAX, UINT32_MAX) == 1);
    CHECK(!HGNativeOptionsUIInputAllowed(&local, 1, 1) && !memcmp(&local, &before, sizeof(local)));
#ifdef HG_NATIVE_OPTIONS_UI_STATE_CHECK
    const HGNativeOptionsUIState *state = HGNativeOptionsUIGetState();
    before = *state; NativeOptionsUIBegin(NULL);
    CHECK(HGNativeOptionsUIGetState() == state && !memcmp(state, &before, sizeof(before)));
    int firstOwner = 1, otherOwner = 2;
    NativeOptionsUIBegin(&firstOwner);
    CHECK(state->active == 1 && !state->ready && state->ownerGeneration && state->revision);
    uint32_t generation = state->ownerGeneration, revision = state->revision;
    before = *state; NativeOptionsUIBegin(&firstOwner); NativeOptionsUIPublish(&otherOwner, 1); NativeOptionsUIRetire(&otherOwner); NativeOptionsUIRetire(NULL);
    CHECK(!memcmp(state, &before, sizeof(before)));
    NativeOptionsUIPublish(&firstOwner, 1);
    CHECK(state->ready == 1 && state->revision != revision && HGNativeOptionsUIInputAllowed(state, generation, state->revision));
    revision = state->revision; before = *state;
    NativeOptionsUIPublish(&firstOwner, 1); NativeOptionsUIPublish(&firstOwner, 2);
    CHECK(HGNativeOptionsUIGetState() == state && !memcmp(state, &before, sizeof(before)));
    CHECK(!HGNativeOptionsUIInputAllowed(state, generation, revision + 1));
    NativeOptionsUIPublish(&firstOwner, 0);
    CHECK(state->active == 1 && !state->ready && state->revision != revision);
    NativeOptionsUIRetire(&firstOwner);
    CHECK(!state->active && !state->ready && state->ownerGeneration == generation);
    before = *state; NativeOptionsUIRetire(&firstOwner); NativeOptionsUIPublish(&firstOwner, 1);
    CHECK(!memcmp(state, &before, sizeof(before)));
    NativeOptionsUIBegin(&firstOwner); // Same allocator address, new original lifecycle.
    CHECK(state->ownerGeneration && state->ownerGeneration != generation && state->active && !state->ready);
    NativeOptionsUIPublish(&firstOwner, 1);
    CHECK(!HGNativeOptionsUIInputAllowed(state, generation, revision));
    CHECK(HGNativeOptionsUIInputAllowed(state, state->ownerGeneration, state->revision));
    gNativeOptionsUIState.ownerGeneration = gNativeOptionsUIState.revision = UINT32_MAX;
    NativeOptionsUIBegin(&otherOwner);
    CHECK(state->ownerGeneration == 1 && state->revision == 1 && !state->ready);
    gNativeOptionsUIState.revision = UINT32_MAX; NativeOptionsUIPublish(&otherOwner, 1);
    CHECK(state->revision == 1 && HGNativeOptionsUIInputAllowed(state, 1, 1));
    gNativeOptionsUIState.revision = UINT32_MAX; NativeOptionsUIRetire(&otherOwner);
    CHECK(!state->active && !state->ready && state->ownerGeneration == 1 && state->revision == 1);
#endif
    return 0;
}

#ifdef HG_NATIVE_OPTIONS_UI_CHECK_MAIN
#include <stdio.h>
int main(void) {
    uint32_t line = CheckNativeOptionsUI();
    if (line) { fprintf(stderr, "Options UI check line %u\n", line); return 1; }
#ifdef HG_NATIVE_OPTIONS_UI_STATE_CHECK
    puts("PASS: Options16 ABI, resident owner lifetime, strict/read-only qualification and nonzero counter wrap.");
#else
    puts("PASS: Options16 ABI and strict/read-only input qualification.");
#endif
    return 0;
}
#endif
