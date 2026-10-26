// SPDX-License-Identifier: GPL-3.0-or-later
#include "native_naming_ui.h"
#include <stddef.h>
#include <string.h>

#define CHECK(value) do { if (!(value)) return __LINE__; } while (0)

uint32_t CheckNativeNamingUI(void) {
    CHECK(sizeof(HGNativeNamingUIState) == 16 && offsetof(HGNativeNamingUIState, active) == 0
        && offsetof(HGNativeNamingUIState, ownerGeneration) == 4
        && offsetof(HGNativeNamingUIState, revision) == 8 && offsetof(HGNativeNamingUIState, ready) == 12);
    HGNativeNamingUIState state = {1, 7, 9, 1}, before = state;
    CHECK(HGNativeNamingUIInputAllowed(&state, 7, 9) == 1 && !memcmp(&state, &before, sizeof(state)));
    CHECK(!HGNativeNamingUIInputAllowed(NULL, 7, 9));
    CHECK(!HGNativeNamingUIInputAllowed(&state, 0, 9) && !HGNativeNamingUIInputAllowed(&state, 7, 0));
    CHECK(!HGNativeNamingUIInputAllowed(&state, 6, 9) && !HGNativeNamingUIInputAllowed(&state, 7, 8));
    CHECK(!memcmp(&state, &before, sizeof(state)));
    HGNativeNamingUIState invalid[] = {
        {0, 7, 9, 1}, {2, 7, 9, 1}, {UINT32_MAX, 7, 9, 1},
        {1, 7, 9, 0}, {1, 7, 9, 2}, {1, 7, 9, UINT32_MAX},
        {1, 0, 9, 1}, {1, 7, 0, 1}, {0, 0, 0, 0}
    };
    for (size_t i = 0; i < sizeof(invalid) / sizeof(invalid[0]); ++i) {
        before = invalid[i];
        CHECK(!HGNativeNamingUIInputAllowed(&invalid[i], invalid[i].ownerGeneration, invalid[i].revision));
        CHECK(!memcmp(&invalid[i], &before, sizeof(before)));
    }
    state = (HGNativeNamingUIState){1, UINT32_MAX, UINT32_MAX, 1}; before = state;
    CHECK(HGNativeNamingUIInputAllowed(&state, UINT32_MAX, UINT32_MAX) == 1);
    CHECK(!HGNativeNamingUIInputAllowed(&state, 1, 1) && !memcmp(&state, &before, sizeof(state)));
    return 0;
}

#ifdef HG_NATIVE_NAMING_UI_CHECK_MAIN
#include <stdio.h>
int main(void) {
    uint32_t line = CheckNativeNamingUI();
    if (line) { fprintf(stderr, "Naming UI check line %u\n", line); return 1; }
    puts("PASS: naming UI ABI, strict readiness, owner identity and read-only qualification.");
    return 0;
}
#endif
