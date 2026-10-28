// SPDX-License-Identifier: GPL-3.0-or-later
#include "native_starter_ui.h"
#include <stddef.h>
#include <string.h>

#define CHECK(value) do { if (!(value)) return __LINE__; } while (0)
#ifdef HG_NATIVE_STARTER_UI_STATE_CHECK
extern HGNativeStarterUIState gNativeStarterUIState;
#endif

uint32_t CheckNativeStarterUI(void) {
    CHECK(sizeof(HGNativeStarterUIState) == 28 && offsetof(HGNativeStarterUIState, ownerGeneration) == 4
        && offsetof(HGNativeStarterUIState, revision) == 8 && offsetof(HGNativeStarterUIState, ready) == 12
        && offsetof(HGNativeStarterUIState, phase) == 16 && offsetof(HGNativeStarterUIState, selection) == 20
        && offsetof(HGNativeStarterUIState, selectionState) == 24);
    const uint32_t keys[] = {0, 1, 2, 3, 8, 16, 32, 48, 64, UINT32_MAX};
    CHECK(!HGNativeStarterUIKeyAllowed(NULL, 7, 9, 1));
    for (uint32_t phase = 0; phase < 14; phase++) for (uint32_t selected = 0; selected < 4; selected++)
    for (uint32_t mode = 0; mode < 4; mode++) for (uint32_t ready = 0; ready < 3; ready++)
    for (size_t i = 0; i < sizeof(keys) / sizeof(*keys); i++) {
        HGNativeStarterUIState state = {1, 7, 9, ready, phase, selected, mode}, before = state;
        uint32_t expected = phase == 5 && selected < 3 && mode < 3 && ready == 1
            && (keys[i] == 1 || (keys[i] == 2 && mode == 2) || ((keys[i] == 16 || keys[i] == 32) && mode != 2));
        CHECK(HGNativeStarterUIKeyAllowed(&state, 7, 9, keys[i]) == expected && !memcmp(&before, &state, sizeof(state)));
        CHECK(!HGNativeStarterUIKeyAllowed(&state, 0, 9, keys[i]) && !HGNativeStarterUIKeyAllowed(&state, 7, 0, keys[i]));
        CHECK(!HGNativeStarterUIKeyAllowed(&state, 6, 9, keys[i]) && !HGNativeStarterUIKeyAllowed(&state, 7, 8, keys[i]));
        uint32_t touch = phase == 5 && selected < 3 && mode < 3 && ready == 1;
        CHECK(HGNativeStarterUITouchAllowed(&state, 7, 9, 0, 255, 191) == touch);
        CHECK(!HGNativeStarterUITouchAllowed(&state, 7, 9, 1, 0, 0)
            && !HGNativeStarterUITouchAllowed(&state, 7, 9, UINT32_MAX, 0, 0)
            && !HGNativeStarterUITouchAllowed(&state, 7, 9, 0, 256, 0)
            && !HGNativeStarterUITouchAllowed(&state, 7, 9, 0, 0, 192));
        CHECK(!memcmp(&before, &state, sizeof(state)));
    }
#ifdef HG_NATIVE_STARTER_UI_STATE_CHECK
    int first, second;
    const HGNativeStarterUIState *state = HGNativeStarterUIGetState();
    HGNativeStarterUIState before = *state;
    NativeStarterUIBegin(NULL); NativeStarterUIPublish(NULL, 5, 0, 0, 1); NativeStarterUIRetire(NULL);
    CHECK(!memcmp(state, &before, sizeof(before)));
    NativeStarterUIBegin(&first);
    CHECK(state->active == 1 && state->ownerGeneration && state->revision && !state->ready);
    uint32_t generation = state->ownerGeneration, revision = state->revision;
    NativeStarterUIPublish(&first, 5, 1, 1, 1);
    CHECK(state->ready == 1 && state->phase == 5 && state->selection == 1 && state->selectionState == 1
        && state->revision != revision && HGNativeStarterUIKeyAllowed(state, generation, state->revision, 1));
    before = *state; NativeStarterUIBegin(&first); NativeStarterUIPublish(&second, 5, 2, 2, 1); NativeStarterUIRetire(&second);
    NativeStarterUIPublish(&first, 13, 1, 1, 1); NativeStarterUIPublish(&first, 5, 3, 1, 1);
    NativeStarterUIPublish(&first, 5, 1, 3, 1); NativeStarterUIPublish(&first, 5, 1, 1, 2);
    NativeStarterUIPublish(&first, 4, 1, 1, 1); NativeStarterUIPublish(&first, 5, 1, 1, 1);
    CHECK(!memcmp(state, &before, sizeof(before)));
    NativeStarterUIPublish(&first, 5, 1, 1, 0);
    CHECK(!state->ready && !HGNativeStarterUIKeyAllowed(state, generation, state->revision, 1));
    NativeStarterUIPublish(&first, 5, 1, 2, 1);
    CHECK(HGNativeStarterUIKeyAllowed(state, generation, state->revision, 2)
        && !HGNativeStarterUIKeyAllowed(state, generation, state->revision, 16));
    NativeStarterUIRetire(&first);
    CHECK(!state->active && !state->ready && state->ownerGeneration == generation);
    before = *state; NativeStarterUIPublish(&first, 5, 0, 0, 1); NativeStarterUIRetire(&first);
    CHECK(!memcmp(state, &before, sizeof(before)));
    NativeStarterUIBegin(&first);
    CHECK(state->active == 1 && state->ownerGeneration != generation && !state->ready);
    gNativeStarterUIState.ownerGeneration = gNativeStarterUIState.revision = UINT32_MAX;
    NativeStarterUIBegin(&second);
    CHECK(state->ownerGeneration == 1 && state->revision == 1 && !state->ready);
    gNativeStarterUIState.revision = UINT32_MAX; NativeStarterUIPublish(&second, 5, 0, 0, 1);
    CHECK(state->revision == 1 && HGNativeStarterUIKeyAllowed(state, 1, 1, 1));
    gNativeStarterUIState.revision = UINT32_MAX; NativeStarterUIRetire(&second);
    CHECK(state->revision == 1 && !state->active && !state->ready);
#endif
    return 0;
}

#ifdef HG_NATIVE_STARTER_UI_CHECK_MAIN
#include <stdio.h>
int main(void) {
    uint32_t line = CheckNativeStarterUI();
    if (line) { fprintf(stderr, "Starter UI check line %u\n", line); return 1; }
    puts("PASS: Starter28 ABI, readonly phase/action qualification and resident lifecycle.");
    return 0;
}
#endif
