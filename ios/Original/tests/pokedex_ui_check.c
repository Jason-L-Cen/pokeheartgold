// SPDX-License-Identifier: GPL-3.0-or-later
// Standalone: packet/resolver only. ABI32 also checks the real resident owned-window/build helpers.
// Original producer timing and host accepted-BEGIN leases still need separate runtime checks.
// Frozen native_pokedex_ui.h SHA-256: 8dd6d9409bc7b6e83df4e01db12555a48442c7a934a5938999efd7b69f675bc6.
#include "native_pokedex_ui.h"
#include <stdio.h>
#include <string.h>
#ifdef HEARTGOLD_ABI32
#include "application/pokedex/pokedex_internal.h"
_Static_assert(sizeof(void *) == 4, "Owned-window checks require actual guest pointers");
#endif

#define CHECK(value) do { if (!(value)) return __LINE__; } while (0)
#define COUNT(array) (sizeof(array) / sizeof((array)[0]))

typedef struct PokedexCheckFamily {
    uint32_t family, phase, controls, visuals, grid, drag;
} PokedexCheckFamily;

// Fixed contract expectations, not values obtained from the implementation being checked.
static const PokedexCheckFamily pokedexFamilies[] = {
    {HG_POKEDEX_UI_FAMILY_OVERVIEW,       6,  5, 10,  0, HG_POKEDEX_UI_DRAG_NONE},
    {HG_POKEDEX_UI_FAMILY_LIST,          11, 25, 42,  0, HG_POKEDEX_UI_DRAG_LIST},
    {HG_POKEDEX_UI_FAMILY_SEARCH_MENU,   22, 10, 38, 10, HG_POKEDEX_UI_DRAG_NONE},
    {HG_POKEDEX_UI_FAMILY_SEARCH_ORDER,  29,  8, 28,  8, HG_POKEDEX_UI_DRAG_NONE},
    {HG_POKEDEX_UI_FAMILY_SEARCH_NAME,   32, 29, 49, 29, HG_POKEDEX_UI_DRAG_NONE},
    {HG_POKEDEX_UI_FAMILY_SEARCH_TYPE,   35, 20, 41, 20, HG_POKEDEX_UI_DRAG_NONE},
    {HG_POKEDEX_UI_FAMILY_SEARCH_HEIGHT, 38,  8, 29,  4, HG_POKEDEX_UI_DRAG_HEIGHT},
    {HG_POKEDEX_UI_FAMILY_SEARCH_WEIGHT, 45,  8, 29,  4, HG_POKEDEX_UI_DRAG_WEIGHT},
    {HG_POKEDEX_UI_FAMILY_SEARCH_AREA,   52,  6, 26,  6, HG_POKEDEX_UI_DRAG_NONE},
    {HG_POKEDEX_UI_FAMILY_SEARCH_BODY,   55, 17, 36, 17, HG_POKEDEX_UI_DRAG_NONE},
    {HG_POKEDEX_UI_FAMILY_SEARCH_RESULTS,57, 24, 45,  0, HG_POKEDEX_UI_DRAG_RESULTS},
    {HG_POKEDEX_UI_FAMILY_AREA,          69, 17, 26,  0, HG_POKEDEX_UI_DRAG_AREA},
    {HG_POKEDEX_UI_FAMILY_SIZE,          77,  5, 19,  0, HG_POKEDEX_UI_DRAG_NONE},
    {HG_POKEDEX_UI_FAMILY_FORMS,         80, 13, 32,  0, HG_POKEDEX_UI_DRAG_FORMS},
    {HG_POKEDEX_UI_FAMILY_COMPARE,       87,  8, 17,  0, HG_POKEDEX_UI_DRAG_NONE},
    {HG_POKEDEX_UI_FAMILY_ENTRY,         90,  5,  7,  0, HG_POKEDEX_UI_DRAG_NONE}
};

static void PokedexCheckReady(HGNativePokedexUIState *state, const PokedexCheckFamily *family) {
    memset(state, 0, sizeof(*state));
    state->version = HG_POKEDEX_UI_VERSION;
    state->byteSize = sizeof(*state);
    state->active = state->ready = 1;
    state->ownerGeneration = 7;
    state->revision = 11;
    state->phase = family->phase;
    state->family = family->family;
    state->gridCursor = family->grid ? family->grid - 1 : HG_POKEDEX_UI_INDEX_NONE;
    state->gridInputMode = family->grid ? HG_POKEDEX_UI_GRID_TOUCH : HG_POKEDEX_UI_GRID_NONE;
    state->keyEnabledMask = 63;
    state->controlCount = family->controls;
    state->visualCount = family->visuals;
    // Opaque valid transports/assets test packet limits, not original semantic table production or pixels.
    for (uint32_t i = 0; i < family->visuals; ++i) {
        state->visuals[i] = (HGNativePokedexUIVisual){.role = HG_POKEDEX_UI_IMAGE_OPTION,
            .index = i, .kind = HG_POKEDEX_UI_VISUAL_RASTER,
            .source.raster = {.engine = 1, .x = 16, .y = 24, .width = 32, .height = 16}};
    }
    for (uint32_t i = 0; i < family->controls; ++i) {
        state->controls[i] = (HGNativePokedexUIControl){.role = HG_POKEDEX_UI_ROLE_OPTION,
            .index = i, .flags = HG_POKEDEX_UI_CONTROL_ENABLED, .visualId = i,
            .transportKind = HG_POKEDEX_UI_TRANSPORT_TOUCH, .transport.touch = {.x = 47, .y = 24}};
        state->controlEnabledMask |= 1u << i;
    }
}

static int PokedexCheckRejected(const HGNativePokedexUIState *state, uint32_t generation,
    uint32_t revision, uint32_t phase, uint32_t kind, uint32_t index, uint32_t value) {
    HGNativePokedexUIInput out = {0xAA, 0xBB, 0xCC, 0xDD};
    return !HGNativePokedexUIResolveInput(state, generation, revision, phase, kind, index, value, &out)
        && !out.keys && !out.touchPressed && !out.touchX && !out.touchY;
}

static int PokedexCheckAccepted(const HGNativePokedexUIState *state, uint32_t kind,
    uint32_t index, uint32_t value, HGNativePokedexUIInput expected) {
    HGNativePokedexUIInput out = {0xAA, 0xBB, 0xCC, 0xDD};
    return HGNativePokedexUIResolveInput(state, 7, 11, state->phase, kind, index, value, &out) == 1
        && !memcmp(&out, &expected, sizeof(out));
}

typedef struct PokedexCheckDrag {
    uint32_t family, phase;
    HGNativePokedexUITrack track;
    HGNativePokedexUIInput begin, low, middle, high;
} PokedexCheckDrag;

// Fixed X/Y interpolation vectors include the physical 0/255 and 0/191 endpoints.
static const PokedexCheckDrag pokedexDrags[] = {
    {HG_POKEDEX_UI_FAMILY_LIST, 15, {HG_POKEDEX_UI_DRAG_LIST, 0, HG_POKEDEX_UI_AXIS_Y, 242, 5, 146, 44},
        {0,1,242,44}, {0,1,242,5}, {0,1,242,76}, {0,1,242,146}},
    {HG_POKEDEX_UI_FAMILY_SEARCH_RESULTS, 60, {HG_POKEDEX_UI_DRAG_RESULTS, 0, HG_POKEDEX_UI_AXIS_Y, 240, 0, 191, 95},
        {0,1,240,95}, {0,1,240,0}, {0,1,240,96}, {0,1,240,191}},
    {HG_POKEDEX_UI_FAMILY_AREA, 73, {HG_POKEDEX_UI_DRAG_AREA, 0, HG_POKEDEX_UI_AXIS_Y, 242, 44, 148, 96},
        {0,1,242,96}, {0,1,242,44}, {0,1,242,96}, {0,1,242,148}},
    {HG_POKEDEX_UI_FAMILY_FORMS, 84, {HG_POKEDEX_UI_DRAG_FORMS, 0, HG_POKEDEX_UI_AXIS_Y, 242, 12, 116, 64},
        {0,1,242,64}, {0,1,242,12}, {0,1,242,64}, {0,1,242,116}},
    {HG_POKEDEX_UI_FAMILY_SEARCH_HEIGHT, 40, {HG_POKEDEX_UI_DRAG_HEIGHT, 0, HG_POKEDEX_UI_AXIS_X, 60, 26, 229, 52},
        {0,1,52,60}, {0,1,26,60}, {0,1,128,60}, {0,1,229,60}},
    {HG_POKEDEX_UI_FAMILY_SEARCH_HEIGHT, 41, {HG_POKEDEX_UI_DRAG_HEIGHT, 1, HG_POKEDEX_UI_AXIS_X, 60, 0, 255, 80},
        {0,1,80,60}, {0,1,0,60}, {0,1,128,60}, {0,1,255,60}},
    {HG_POKEDEX_UI_FAMILY_SEARCH_WEIGHT, 47, {HG_POKEDEX_UI_DRAG_WEIGHT, 0, HG_POKEDEX_UI_AXIS_X, 132, 26, 229, 200},
        {0,1,200,132}, {0,1,26,132}, {0,1,128,132}, {0,1,229,132}},
    {HG_POKEDEX_UI_FAMILY_SEARCH_WEIGHT, 48, {HG_POKEDEX_UI_DRAG_WEIGHT, 1, HG_POKEDEX_UI_AXIS_X, 132, 0, 255, 128},
        {0,1,128,132}, {0,1,0,132}, {0,1,128,132}, {0,1,255,132}}
};

#ifdef HEARTGOLD_ABI32
static uint32_t PokedexCheckSnapshots(void) {
    // Metadata-only fixtures exercise the production copy helpers, not a game outcome or renderer.
    static uint8_t source[24 * 1024 + 32], expected[24 * 1024];
    static HGNativePokedexUIState state, committed, live, invalid, unchanged, current;
    const uint8_t *owned[4];
    for (uint32_t i = 0; i < sizeof(expected); ++i) source[i] = expected[i] = (uint8_t)(17 + 37 * i);
    memset(source + sizeof(expected), 0x3C, 32);
    PokedexCheckReady(&state, &pokedexFamilies[1]);
    state.frameSerial = 41;
    state.controlCount = state.controlEnabledMask = 0;
    state.visualCount = 4;
    for (uint32_t i = 0; i < state.visualCount; ++i) {
        state.visuals[i] = (HGNativePokedexUIVisual){.role = HG_POKEDEX_UI_IMAGE_HEADER,
            .index = i, .kind = HG_POKEDEX_UI_VISUAL_WINDOW,
            .source.window = {.pixelBuffer = (uint32_t)(uintptr_t)(source + i * 6144),
                .tileWidth = 32, .tileHeight = 6, .clearColor = i, .engine = i & 1}};
        for (uint32_t color = 0; color < 16; ++color) state.visuals[i].source.window.palette[color] = (uint16_t)(32 * i + color);
    }
    unchanged = state;
    CHECK(NativePokedexUIOwnWindows(&state));
    for (uint32_t i = 0; i < state.visualCount; ++i) {
        owned[i] = (const uint8_t *)(uintptr_t)state.visuals[i].source.window.pixelBuffer;
        CHECK(owned[i] && owned[i] != source + i * 6144);
        CHECK(!memcmp(owned[i], expected + i * 6144, 6144));
        CHECK(!memcmp(&state.visuals[i].source.window.tileWidth, &unchanged.visuals[i].source.window.tileWidth,
            sizeof(HGNativePartyUIWindow) - offsetof(HGNativePartyUIWindow, tileWidth)));
    }
    committed = state;
    memset(source, 0xA5, sizeof(source));
    for (uint32_t i = 0; i < 4; ++i) CHECK(!memcmp(owned[i], expected + i * 6144, 6144));

    // A late invalid/overflowing window must not partially overwrite the preceding committed image.
    invalid = unchanged; invalid.visualCount = 5;
    invalid.visuals[4] = (HGNativePokedexUIVisual){.role = HG_POKEDEX_UI_IMAGE_HEADER,
        .index = 4, .kind = HG_POKEDEX_UI_VISUAL_WINDOW,
        .source.window = {.pixelBuffer = (uint32_t)(uintptr_t)(source + 24 * 1024), .tileWidth = 1, .tileHeight = 1}};
    unchanged = invalid;
    CHECK(!NativePokedexUIOwnWindows(&invalid) && !memcmp(&invalid, &unchanged, sizeof(invalid)));
    for (uint32_t i = 0; i < 4; ++i) CHECK(!memcmp(owned[i], expected + i * 6144, 6144));
    invalid = unchanged; invalid.visualCount = 4;
    invalid.visuals[3].source.window.pixelBuffer = 0;
    unchanged = invalid;
    CHECK(!NativePokedexUIOwnWindows(&invalid) && !memcmp(&invalid, &unchanged, sizeof(invalid)));
    for (uint32_t i = 0; i < 4; ++i) CHECK(!memcmp(owned[i], expected + i * 6144, 6144));

    // AfterDraw may restore only an independently rebuilt, identical current capability.
    memcpy(source, expected, sizeof(expected));
    committed.controlCount = committed.controlEnabledMask = 1;
    committed.controls[0] = (HGNativePokedexUIControl){.role = HG_POKEDEX_UI_ROLE_GRID_CELL,
        .index = 0, .flags = HG_POKEDEX_UI_CONTROL_ENABLED, .visualId = 0,
        .transportKind = HG_POKEDEX_UI_TRANSPORT_TOUCH, .transport.touch = {.x = 47, .y = 24}};
    current = committed;
    for (uint32_t i = 0; i < 4; ++i) current.visuals[i].source.window.pixelBuffer = (uint32_t)(uintptr_t)(source + i * 6144);
    live = committed;
    NativePokedexUIPrepareBuild(&live, &committed, 11, HG_POKEDEX_UI_FAMILY_LIST, NULL);
    unchanged = live;
    NativePokedexUIPrepareBuild(&live, &committed, 11, HG_POKEDEX_UI_FAMILY_LIST, &current);
    CHECK(!memcmp(&live, &committed, sizeof(live)));
    CHECK(PokedexCheckAccepted(&live, HG_POKEDEX_UI_INPUT_CONTROL, 0, 0, (HGNativePokedexUIInput){0,1,47,24}));
    CHECK(PokedexCheckAccepted(&live, HG_POKEDEX_UI_INPUT_BACK, 0, 0, (HGNativePokedexUIInput){2,0,0,0}));
    invalid = current; invalid.revision++;
    live = unchanged;
    NativePokedexUIPrepareBuild(&live, &committed, 11, HG_POKEDEX_UI_FAMILY_LIST, &invalid);
    CHECK(!memcmp(&live, &committed, sizeof(live))); // Revalidation does not invent a new revision/frame.
    for (uint32_t mismatch = 0; mismatch < 10; ++mismatch) {
        invalid = current;
        if (mismatch == 0) invalid.species++;
        if (mismatch == 1) invalid.cursor++;
        if (mismatch == 2) invalid.controls[0].transport.touch.x++;
        if (mismatch == 3) invalid.visuals[0].source.window.palette[3] ^= 1;
        if (mismatch == 4) source[37] ^= 1;
        if (mismatch == 5) invalid.ownerGeneration++;
        if (mismatch == 6) invalid.phase = 15;
        if (mismatch == 7) invalid.frameSerial++;
        if (mismatch == 8) invalid.visuals[0].source.window.pixelBuffer = committed.visuals[0].source.window.pixelBuffer;
        if (mismatch == 9) { invalid.ready = 0; invalid.busyReason = HG_POKEDEX_UI_BUSY_GRAPHICS; }
        live = unchanged;
        NativePokedexUIPrepareBuild(&live, &committed, 11, HG_POKEDEX_UI_FAMILY_LIST, &invalid);
        CHECK(!memcmp(&live, &unchanged, sizeof(live)));
        CHECK(!live.ready && !live.controlCount && !live.controlEnabledMask && !live.keyEnabledMask && live.visualCount == 4);
        CHECK(PokedexCheckRejected(&live, 7, 11, 11, HG_POKEDEX_UI_INPUT_CONTROL, 0, 0));
        CHECK(PokedexCheckRejected(&live, 7, 11, 11, HG_POKEDEX_UI_INPUT_BACK, 0, 0));
        if (mismatch == 4) source[37] ^= 1;
        for (uint32_t i = 0; i < 4; ++i) CHECK(!memcmp(owned[i], expected + i * 6144, 6144));
    }
    committed = state;

    // First stable-list -> drag BUILD keeps owned presentation without inventing a continuation.
    live = committed;
    NativePokedexUIPrepareBuild(&live, &committed, 15, HG_POKEDEX_UI_FAMILY_LIST, NULL);
    CHECK(live.active == 1 && !live.ready && live.busyReason == HG_POKEDEX_UI_BUSY_BUILD && live.phase == 15);
    CHECK(live.ownerGeneration == 7 && live.revision == 11 && live.family == HG_POKEDEX_UI_FAMILY_LIST && live.frameSerial == 41);
    CHECK(live.visualCount == 4 && !memcmp(live.visuals, committed.visuals, 4 * sizeof(*live.visuals)));
    CHECK(!live.controlCount && !live.controlEnabledMask && !live.keyEnabledMask && !live.continuation.kind);
    CHECK(PokedexCheckRejected(&live, 7, 11, 15, HG_POKEDEX_UI_INPUT_CONTROL, 0, 0));
    CHECK(PokedexCheckRejected(&live, 7, 11, 15, HG_POKEDEX_UI_INPUT_GESTURE_UPDATE, 0, 0));

    committed.phase = 15; committed.ready = 0; committed.busyReason = HG_POKEDEX_UI_BUSY_DRAG;
    committed.keyEnabledMask = 0; committed.continuation = pokedexDrags[0].track;
    live = committed;
    NativePokedexUIPrepareBuild(&live, &committed, 15, HG_POKEDEX_UI_FAMILY_LIST, NULL);
    CHECK(!live.ready && live.busyReason == HG_POKEDEX_UI_BUSY_DRAG && live.visualCount == 4);
    CHECK(!live.controlCount && !live.controlEnabledMask && !live.keyEnabledMask);
    CHECK(!memcmp(&live.continuation, &committed.continuation, sizeof(live.continuation)));
    CHECK(PokedexCheckAccepted(&live, HG_POKEDEX_UI_INPUT_GESTURE_UPDATE, 0, 65535, pokedexDrags[0].high));
    CHECK(PokedexCheckRejected(&live, 7, 11, 15, HG_POKEDEX_UI_INPUT_GESTURE_BEGIN, 0, 0));
    live = committed;
    NativePokedexUIPrepareBuild(&live, &committed, 11, HG_POKEDEX_UI_FAMILY_LIST, NULL);
    CHECK(live.visualCount == 4 && !live.continuation.kind && !live.ready && !live.controlCount && !live.keyEnabledMask && !live.controlEnabledMask);
    CHECK(PokedexCheckRejected(&live, 7, 11, 11, HG_POKEDEX_UI_INPUT_GESTURE_UPDATE, 0, 0));

    for (uint32_t mismatch = 0; mismatch < 5; ++mismatch) {
        live = committed;
        if (mismatch == 0) live.ownerGeneration++;
        if (mismatch == 1) live.frameSerial++;
        if (mismatch == 2) live.active = 0;
        if (mismatch == 3) { live.ownerGeneration++; live.busyReason = HG_POKEDEX_UI_BUSY_RETIRE; }
        HGNativePokedexUIState previous = committed;
        if (mismatch == 4) previous.frameSerial = 0;
        NativePokedexUIPrepareBuild(&live, &previous, 15, HG_POKEDEX_UI_FAMILY_LIST, NULL);
        CHECK(!live.visualCount && !live.continuation.kind && !live.ready && !live.controlCount && !live.controlEnabledMask && !live.keyEnabledMask);
    }
    live = committed;
    NativePokedexUIPrepareBuild(&live, &committed, 57, HG_POKEDEX_UI_FAMILY_SEARCH_RESULTS, NULL);
    CHECK(live.family == HG_POKEDEX_UI_FAMILY_SEARCH_RESULTS && !live.visualCount && !live.continuation.kind && !live.ready);
    live = committed;
    NativePokedexUIPrepareBuild(&live, &committed, 16, HG_POKEDEX_UI_FAMILY_NONE, NULL);
    CHECK(live.phase == 16 && !live.family && !live.visualCount && !live.continuation.kind && !live.ready);
    for (uint32_t i = 0; i < 4; ++i) CHECK(!memcmp(owned[i], expected + i * 6144, 6144));

    // The original no-results wait has no family/assets/input, but retains real scalar entries.
    PokedexCheckReady(&committed, &pokedexFamilies[1]);
    committed.frameSerial = 51; committed.phase = 23; committed.family = HG_POKEDEX_UI_FAMILY_NONE;
    committed.ready = committed.keyEnabledMask = committed.controlCount = committed.controlEnabledMask = committed.visualCount = 0;
    committed.busyReason = HG_POKEDEX_UI_BUSY_TRANSITION;
    memset(&committed.continuation, 0, sizeof(committed) - offsetof(HGNativePokedexUIState, continuation));
    committed.entries[0].species = 152;
    current = live = committed;
    NativePokedexUIPrepareBuild(&live, &committed, 23, HG_POKEDEX_UI_FAMILY_NONE, NULL);
    CHECK(live.busyReason == HG_POKEDEX_UI_BUSY_BUILD && !live.ready && !live.visualCount);
    unchanged = live;
    NativePokedexUIPrepareBuild(&live, &committed, 23, HG_POKEDEX_UI_FAMILY_NONE, &current);
    CHECK(!memcmp(&live, &committed, sizeof(live)) && live.entries[0].species == 152);
    CHECK(!live.ready && !live.keyEnabledMask && !live.controlCount && !live.controlEnabledMask && !live.visualCount);
    for (uint32_t kind = HG_POKEDEX_UI_INPUT_CONTROL; kind <= HG_POKEDEX_UI_INPUT_GESTURE_END; ++kind)
        CHECK(PokedexCheckRejected(&live, 7, 11, 23, kind, 0, 0));
    for (uint32_t difference = 0; difference < 21; ++difference) {
        invalid = current; live = state = unchanged;
        uint32_t actualPhase = 23, actualFamily = HG_POKEDEX_UI_FAMILY_NONE;
        if (difference == 0) invalid.entries[0].species++;
        if (difference == 1) invalid.species++;
        if (difference == 2) invalid.phase = 22;
        if (difference == 3) invalid.family = HG_POKEDEX_UI_FAMILY_LIST;
        if (difference == 4) invalid.ownerGeneration++;
        if (difference == 5) invalid.frameSerial++;
        if (difference == 6) invalid.ready = 1;
        if (difference == 7) invalid.keyEnabledMask = 1;
        if (difference == 8) invalid.controlCount = 1;
        if (difference == 9) invalid.controlEnabledMask = 1;
        if (difference == 10) invalid.visualCount = 1;
        if (difference == 11) invalid.continuation = pokedexDrags[0].track;
        if (difference == 12) invalid.controls[0].role = HG_POKEDEX_UI_ROLE_OPTION;
        if (difference == 13) invalid.visuals[0].role = HG_POKEDEX_UI_IMAGE_HEADER;
        if (difference == 14) invalid.busyReason = HG_POKEDEX_UI_BUSY_UNSUPPORTED;
        if (difference == 15) actualPhase = 22;
        if (difference == 16) actualFamily = HG_POKEDEX_UI_FAMILY_LIST;
        if (difference == 17) live.ownerGeneration++;
        if (difference == 18) live.frameSerial++;
        if (difference == 19) live.revision++;
        if (difference == 20) live.active = 0;
        state = live; state.phase = actualPhase; state.family = actualFamily;
        NativePokedexUIPrepareBuild(&live, &committed, actualPhase, actualFamily, &invalid);
        CHECK(!memcmp(&live, &state, sizeof(live)) && live.busyReason == HG_POKEDEX_UI_BUSY_BUILD);
        for (uint32_t kind = HG_POKEDEX_UI_INPUT_CONTROL; kind <= HG_POKEDEX_UI_INPUT_GESTURE_END; ++kind)
            CHECK(PokedexCheckRejected(&live, live.ownerGeneration, live.revision, live.phase, kind, 0, 0));
    }
    // Matching nonempty models must still fail each no-input/no-resource condition.
    for (uint32_t nonempty = 0; nonempty < 8; ++nonempty) {
        invalid = committed;
        if (nonempty == 0) invalid.ready = 1;
        if (nonempty == 1) invalid.keyEnabledMask = 1;
        if (nonempty == 2) invalid.controlCount = 1;
        if (nonempty == 3) invalid.controlEnabledMask = 1;
        if (nonempty == 4) invalid.visualCount = 1;
        if (nonempty == 5) invalid.continuation = pokedexDrags[0].track;
        if (nonempty == 6) invalid.controls[0].role = HG_POKEDEX_UI_ROLE_OPTION;
        if (nonempty == 7) invalid.visuals[0].role = HG_POKEDEX_UI_IMAGE_HEADER;
        current = live = invalid;
        NativePokedexUIPrepareBuild(&live, &invalid, 23, HG_POKEDEX_UI_FAMILY_NONE, &current);
        CHECK(!memcmp(&live, &unchanged, sizeof(live)) && live.busyReason == HG_POKEDEX_UI_BUSY_BUILD);
    }
    for (uint32_t i = 0; i < 4; ++i) CHECK(!memcmp(owned[i], expected + i * 6144, 6144));
    return 0;
}
#endif

uint32_t CheckPokedexUI(void) {
    HGNativePokedexUIState state, base, before;
    static const uint32_t directionKeys[] = {64, 128, 32, 16};
    const HGNativePokedexUIInput zero = {0};
#define REJECT(kind, index, value) PokedexCheckRejected(&state, 7, 11, state.phase, kind, index, value)
#define BAD(field, value) do { state = base; state.field = (value); CHECK(REJECT(HG_POKEDEX_UI_INPUT_CONTROL, 0, 0)); } while (0)

    CHECK(COUNT(pokedexFamilies) == 16 && COUNT(pokedexDrags) == 8);
    CHECK(HGNativePokedexUIMask(0) == 0 && HGNativePokedexUIMask(1) == 1);
    CHECK(HGNativePokedexUIMask(31) == 0x7FFFFFFFu && HGNativePokedexUIMask(32) == UINT32_MAX);
    CHECK(HGNativePokedexUIMask(UINT32_MAX) == UINT32_MAX);
    CHECK(!HGNativePokedexUIFamilyLimit(HG_POKEDEX_UI_FAMILY_COUNT, 0));
    CHECK(!HGNativePokedexUIFamilyLimit(UINT32_MAX, 0) && !HGNativePokedexUIFamilyLimit(1, 4));
    CHECK(!HGNativePokedexUITrackPhase(NULL) && !HGNativePokedexUIVisualIsValid(NULL));
    CHECK(!HGNativePokedexUITrackIsValid(NULL, HG_POKEDEX_UI_FAMILY_LIST));
    for (uint32_t i = 0; i < COUNT(pokedexFamilies); ++i) {
        const PokedexCheckFamily *family = &pokedexFamilies[i];
        CHECK(HGNativePokedexUIFamilyLimit(family->family, 0) == family->phase);
        CHECK(HGNativePokedexUIFamilyLimit(family->family, 1) == family->controls);
        CHECK(HGNativePokedexUIFamilyLimit(family->family, 2) == family->visuals);
        CHECK(HGNativePokedexUIFamilyLimit(family->family, 3) == family->grid);
        CHECK(HGNativePokedexUIDragKind(family->family) == family->drag);
        PokedexCheckReady(&state, family); before = state;
        CHECK(PokedexCheckAccepted(&state, HG_POKEDEX_UI_INPUT_CONTROL, family->controls - 1, 0,
            (HGNativePokedexUIInput){0,1,47,24}));
        for (uint32_t direction = 0; direction < 4; ++direction)
            CHECK(PokedexCheckAccepted(&state, HG_POKEDEX_UI_INPUT_DIRECTION, direction, 0,
                (HGNativePokedexUIInput){directionKeys[direction],0,0,0}));
        CHECK(PokedexCheckAccepted(&state, HG_POKEDEX_UI_INPUT_CONFIRM, 0, 0, (HGNativePokedexUIInput){1,0,0,0}));
        CHECK(PokedexCheckAccepted(&state, HG_POKEDEX_UI_INPUT_BACK, 0, 0, (HGNativePokedexUIInput){2,0,0,0}));
        CHECK(!memcmp(&state, &before, sizeof(state)));
        CHECK(REJECT(HG_POKEDEX_UI_INPUT_CONTROL, family->controls, 0));
        CHECK(REJECT(HG_POKEDEX_UI_INPUT_CONTROL, UINT32_MAX, 0));
        CHECK(REJECT(HG_POKEDEX_UI_INPUT_CONTROL, 0, 1));
        CHECK(REJECT(HG_POKEDEX_UI_INPUT_GESTURE_BEGIN, 0, 0));
        state.controlCount++; CHECK(REJECT(HG_POKEDEX_UI_INPUT_CONTROL, 0, 0)); state = before;
        state.visualCount++; CHECK(REJECT(HG_POKEDEX_UI_INPUT_CONTROL, 0, 0)); state = before;
        state.controlEnabledMask |= 1u << family->controls;
        CHECK(REJECT(HG_POKEDEX_UI_INPUT_CONTROL, 0, 0)); state = before;
        state.gridInputMode = 3; CHECK(REJECT(HG_POKEDEX_UI_INPUT_CONTROL, 0, 0)); state = before;
        if (family->grid) {
            state.gridInputMode = HG_POKEDEX_UI_GRID_BUTTONS;
            CHECK(PokedexCheckAccepted(&state, HG_POKEDEX_UI_INPUT_CONTROL, 0, 0, (HGNativePokedexUIInput){0,1,47,24}));
            state.gridCursor = family->grid; CHECK(REJECT(HG_POKEDEX_UI_INPUT_CONTROL, 0, 0)); state = before;
            state.gridCursor = HG_POKEDEX_UI_INDEX_NONE; CHECK(REJECT(HG_POKEDEX_UI_INPUT_CONTROL, 0, 0)); state = before;
            state.gridInputMode = HG_POKEDEX_UI_GRID_NONE; CHECK(REJECT(HG_POKEDEX_UI_INPUT_CONTROL, 0, 0));
        } else {
            state.gridCursor = 0; CHECK(REJECT(HG_POKEDEX_UI_INPUT_CONTROL, 0, 0)); state = before;
            state.gridInputMode = HG_POKEDEX_UI_GRID_TOUCH; CHECK(REJECT(HG_POKEDEX_UI_INPUT_CONTROL, 0, 0));
        }
    }

    PokedexCheckReady(&base, &pokedexFamilies[1]); state = base;
    CHECK(PokedexCheckRejected(NULL, 7, 11, 11, HG_POKEDEX_UI_INPUT_CONTROL, 0, 0));
    CHECK(!HGNativePokedexUIResolveInput(&state, 7, 11, 11, HG_POKEDEX_UI_INPUT_CONTROL, 0, 0, NULL));
    CHECK(PokedexCheckRejected(&state, 0, 11, 11, HG_POKEDEX_UI_INPUT_CONTROL, 0, 0));
    CHECK(PokedexCheckRejected(&state, 8, 11, 11, HG_POKEDEX_UI_INPUT_CONTROL, 0, 0));
    CHECK(PokedexCheckRejected(&state, 7, 0, 11, HG_POKEDEX_UI_INPUT_CONTROL, 0, 0));
    CHECK(PokedexCheckRejected(&state, 7, 12, 11, HG_POKEDEX_UI_INPUT_CONTROL, 0, 0));
    CHECK(PokedexCheckRejected(&state, 7, 11, 12, HG_POKEDEX_UI_INPUT_CONTROL, 0, 0));
    BAD(version, 0); BAD(byteSize, 5120); BAD(active, 0); BAD(active, 2);
    BAD(ownerGeneration, 0); BAD(revision, 0); BAD(family, HG_POKEDEX_UI_FAMILY_NONE);
    BAD(family, HG_POKEDEX_UI_FAMILY_COUNT); BAD(family, UINT32_MAX); BAD(phase, 12);
    BAD(ready, 0); BAD(ready, 2); BAD(busyReason, HG_POKEDEX_UI_BUSY_GRAPHICS); BAD(busyReason, UINT32_MAX);
    BAD(keyEnabledMask, 64); BAD(controlCount, UINT32_MAX); BAD(visualCount, UINT32_MAX); BAD(visualCount, 0);
    BAD(controls[0].role, HG_POKEDEX_UI_ROLE_NONE); BAD(controls[0].role, HG_POKEDEX_UI_ROLE_COUNT);
    BAD(controls[24].flags, 4); BAD(controls[24].visualId, 42);
    BAD(controls[0].flags, HG_POKEDEX_UI_CONTROL_SELECTED);
    BAD(controls[0].transportKind, HG_POKEDEX_UI_TRANSPORT_NONE);
    BAD(controls[0].transportKind, UINT32_MAX);
    BAD(controls[0].transport.touch.x, 256); BAD(controls[0].transport.touch.y, 192);
    BAD(continuation.kind, HG_POKEDEX_UI_DRAG_LIST);
    state = base; state.controls[0].flags = 3;
    CHECK(PokedexCheckAccepted(&state, HG_POKEDEX_UI_INPUT_CONTROL, 0, 0, (HGNativePokedexUIInput){0,1,47,24}));
    state = base; state.controls[0].flags = 0; state.controlEnabledMask &= ~1u;
    CHECK(REJECT(HG_POKEDEX_UI_INPUT_CONTROL, 0, 0));
    CHECK(PokedexCheckAccepted(&state, HG_POKEDEX_UI_INPUT_CONTROL, 1, 0, (HGNativePokedexUIInput){0,1,47,24}));
    state = base; state.controls[0].transport.touch.x = 255; state.controls[0].transport.touch.y = 191;
    CHECK(PokedexCheckAccepted(&state, HG_POKEDEX_UI_INPUT_CONTROL, 0, 0, (HGNativePokedexUIInput){0,1,255,191}));
    state = base; state.keyEnabledMask = 0;
    CHECK(REJECT(HG_POKEDEX_UI_INPUT_DIRECTION, 0, 0));
    CHECK(REJECT(HG_POKEDEX_UI_INPUT_CONFIRM, 0, 0)); CHECK(REJECT(HG_POKEDEX_UI_INPUT_BACK, 0, 0));
    state = base; state.controlCount = state.controlEnabledMask = state.visualCount = 0;
    state.keyEnabledMask = 32;
    CHECK(PokedexCheckAccepted(&state, HG_POKEDEX_UI_INPUT_BACK, 0, 0, (HGNativePokedexUIInput){2,0,0,0}));
    CHECK(REJECT(HG_POKEDEX_UI_INPUT_CONTROL, 0, 0));
    state = base;
    CHECK(REJECT(HG_POKEDEX_UI_INPUT_DIRECTION, 4, 0)); CHECK(REJECT(HG_POKEDEX_UI_INPUT_DIRECTION, UINT32_MAX, 0));
    CHECK(REJECT(HG_POKEDEX_UI_INPUT_CONFIRM, 1, 0)); CHECK(REJECT(HG_POKEDEX_UI_INPUT_BACK, 1, 0));
    CHECK(REJECT(HG_POKEDEX_UI_INPUT_BACK, 0, 65536)); CHECK(REJECT(UINT32_MAX, 0, 0));
    state.controls[0].transportKind = HG_POKEDEX_UI_TRANSPORT_KEY;
    for (uint32_t bit = 0; bit < 12; ++bit) {
        state.controls[0].transport.key = 1u << bit;
        CHECK(PokedexCheckAccepted(&state, HG_POKEDEX_UI_INPUT_CONTROL, 0, 0, (HGNativePokedexUIInput){1u << bit,0,0,0}));
    }
    static const uint32_t badKeys[] = {0, 3, 4096, UINT32_MAX};
    for (uint32_t i = 0; i < COUNT(badKeys); ++i) {
        state.controls[0].transport.key = badKeys[i]; CHECK(REJECT(HG_POKEDEX_UI_INPUT_CONTROL, 0, 0));
    }

    base.visuals[0] = (HGNativePokedexUIVisual){.role = HG_POKEDEX_UI_IMAGE_HEADER,
        .index = 7, .kind = HG_POKEDEX_UI_VISUAL_WINDOW,
        .source.window = {.pixelBuffer = 0x1000, .tileWidth = 32, .tileHeight = 6, .clearColor = 15, .engine = 1}};
    state = base; before = state;
    CHECK(PokedexCheckAccepted(&state, HG_POKEDEX_UI_INPUT_CONTROL, 0, 0, (HGNativePokedexUIInput){0,1,47,24}));
    CHECK(!memcmp(&state, &before, sizeof(state)));
    state.visuals[0].source.window.pixelBuffer = 0xFFFFE7FFu;
    CHECK(PokedexCheckAccepted(&state, HG_POKEDEX_UI_INPUT_CONTROL, 0, 0, (HGNativePokedexUIInput){0,1,47,24}));
    BAD(visuals[0].source.window.pixelBuffer, 0xFFFFE800u);
    BAD(visuals[0].source.window.pixelBuffer, 0); BAD(visuals[0].source.window.pixelBuffer, UINT32_MAX);
    BAD(visuals[0].source.window.tileWidth, 0); BAD(visuals[0].source.window.tileWidth, 33);
    BAD(visuals[0].source.window.tileHeight, 0); BAD(visuals[0].source.window.tileHeight, 7);
    BAD(visuals[0].source.window.clearColor, 16); BAD(visuals[0].source.window.engine, 2);
    BAD(visuals[0].role, HG_POKEDEX_UI_IMAGE_NONE); BAD(visuals[0].role, HG_POKEDEX_UI_IMAGE_COUNT);
    BAD(visuals[0].kind, 0); BAD(visuals[0].kind, UINT32_MAX);
    PokedexCheckReady(&base, &pokedexFamilies[1]);
    base.visuals[0] = (HGNativePokedexUIVisual){.role = HG_POKEDEX_UI_IMAGE_OPTION,
        .kind = HG_POKEDEX_UI_VISUAL_RASTER,
        .source.raster = {.engine = 1, .x = 255, .y = 191, .width = 1, .height = 1}};
    state = base;
    CHECK(PokedexCheckAccepted(&state, HG_POKEDEX_UI_INPUT_CONTROL, 0, 0, (HGNativePokedexUIInput){0,1,47,24}));
    BAD(visuals[0].source.raster.width, 2); BAD(visuals[0].source.raster.height, 2);
    BAD(visuals[0].source.raster.width, UINT32_MAX); BAD(visuals[0].source.raster.height, UINT32_MAX);
    BAD(visuals[0].source.raster.x, 256); BAD(visuals[0].source.raster.y, 192);
    BAD(visuals[0].source.raster.width, 0); BAD(visuals[0].source.raster.height, 0);
    BAD(visuals[41].source.raster.engine, 2);
    base.visuals[0] = (HGNativePokedexUIVisual){.role = HG_POKEDEX_UI_IMAGE_OPTION,
        .kind = HG_POKEDEX_UI_VISUAL_RASTER,
        .source.raster = {.engine = 0, .x = 0, .y = 0, .width = 256, .height = 192}};
    state = base;
    CHECK(PokedexCheckAccepted(&state, HG_POKEDEX_UI_INPUT_CONTROL, 0, 0, (HGNativePokedexUIInput){0,1,47,24}));

    for (uint32_t i = 0; i < COUNT(pokedexDrags); ++i) {
        const PokedexCheckDrag *drag = &pokedexDrags[i];
        PokedexCheckReady(&state, &pokedexFamilies[drag->family - 1]);
        CHECK(HGNativePokedexUIDragKind(drag->family) == drag->track.kind);
        CHECK(HGNativePokedexUITrackPhase(&drag->track) == drag->phase);
        CHECK(HGNativePokedexUITrackIsValid(&drag->track, drag->family));
        CHECK(!HGNativePokedexUITrackIsValid(&drag->track, HG_POKEDEX_UI_FAMILY_OVERVIEW));
        state.controls[0].transportKind = HG_POKEDEX_UI_TRANSPORT_TRACK;
        state.controls[0].transport.track = drag->track; before = state;
        CHECK(PokedexCheckAccepted(&state, HG_POKEDEX_UI_INPUT_GESTURE_BEGIN, 0, 0, drag->begin));
        CHECK(!memcmp(&state, &before, sizeof(state)));
        state.controls[0].transport.track.position = drag->track.minimum;
        CHECK(PokedexCheckAccepted(&state, HG_POKEDEX_UI_INPUT_GESTURE_BEGIN, 0, 0, drag->low));
        state.controls[0].transport.track.position = drag->track.maximum;
        CHECK(PokedexCheckAccepted(&state, HG_POKEDEX_UI_INPUT_GESTURE_BEGIN, 0, 0, drag->high)); state = before;
        CHECK(REJECT(HG_POKEDEX_UI_INPUT_CONTROL, 0, 0));
        state.controls[0].transport.track.handle = 2;
        CHECK(REJECT(HG_POKEDEX_UI_INPUT_GESTURE_BEGIN, 0, 0)); state = before;
        state.controls[0].transport.track.maximum = 256;
        CHECK(REJECT(HG_POKEDEX_UI_INPUT_GESTURE_BEGIN, 0, 0)); state = before;
        CHECK(REJECT(HG_POKEDEX_UI_INPUT_GESTURE_UPDATE, drag->track.handle, 0));
        CHECK(REJECT(HG_POKEDEX_UI_INPUT_GESTURE_END, drag->track.handle, 0));
        state.ready = 0; state.busyReason = HG_POKEDEX_UI_BUSY_DRAG; state.phase = drag->phase;
        state.keyEnabledMask = state.controlEnabledMask = 0;
        for (uint32_t control = 0; control < state.controlCount; ++control) state.controls[control].flags = 0;
        state.continuation = drag->track; base = before = state;
        // No admission history is supplied: these checks prove only guest continuation geometry, not a host lease.
        CHECK(PokedexCheckAccepted(&state, HG_POKEDEX_UI_INPUT_GESTURE_UPDATE, drag->track.handle, 0, drag->low));
        CHECK(PokedexCheckAccepted(&state, HG_POKEDEX_UI_INPUT_GESTURE_UPDATE, drag->track.handle, 32768, drag->middle));
        CHECK(PokedexCheckAccepted(&state, HG_POKEDEX_UI_INPUT_GESTURE_UPDATE, drag->track.handle, 65535, drag->high));
        CHECK(PokedexCheckAccepted(&state, HG_POKEDEX_UI_INPUT_GESTURE_END, drag->track.handle, 0, zero));
        CHECK(!memcmp(&state, &before, sizeof(state)));
        CHECK(REJECT(HG_POKEDEX_UI_INPUT_CONTROL, 0, 0)); CHECK(REJECT(HG_POKEDEX_UI_INPUT_GESTURE_BEGIN, 0, 0));
        CHECK(REJECT(HG_POKEDEX_UI_INPUT_GESTURE_UPDATE, drag->track.handle, 65536));
        CHECK(REJECT(HG_POKEDEX_UI_INPUT_GESTURE_END, drag->track.handle, 1));
        CHECK(REJECT(HG_POKEDEX_UI_INPUT_GESTURE_UPDATE, drag->track.handle ^ 1u, 0));
        CHECK(REJECT(HG_POKEDEX_UI_INPUT_GESTURE_UPDATE, 2, 0));
        CHECK(PokedexCheckRejected(&state, 8, 11, drag->phase, HG_POKEDEX_UI_INPUT_GESTURE_UPDATE, drag->track.handle, 0));
        CHECK(PokedexCheckRejected(&state, 7, 12, drag->phase, HG_POKEDEX_UI_INPUT_GESTURE_END, drag->track.handle, 0));
#define BAD_TRACK(field, value) do { state = base; state.field = (value); CHECK(REJECT(HG_POKEDEX_UI_INPUT_GESTURE_UPDATE, drag->track.handle, 0)); } while (0)
        BAD_TRACK(phase, 1); BAD_TRACK(ready, 1); BAD_TRACK(busyReason, HG_POKEDEX_UI_BUSY_NONE);
        BAD_TRACK(keyEnabledMask, 1); BAD_TRACK(active, 0);
        BAD_TRACK(continuation.kind, HG_POKEDEX_UI_DRAG_NONE); BAD_TRACK(continuation.kind, UINT32_MAX);
        BAD_TRACK(continuation.handle, 2); BAD_TRACK(continuation.axis, 2);
        BAD_TRACK(continuation.fixed, 256); BAD_TRACK(continuation.maximum, 256);
        BAD_TRACK(continuation.minimum, UINT32_MAX);
        BAD_TRACK(continuation.minimum, drag->track.maximum);
        BAD_TRACK(continuation.maximum, drag->track.minimum);
        BAD_TRACK(continuation.position, UINT32_MAX);
        if (drag->track.minimum) { BAD_TRACK(continuation.position, drag->track.minimum - 1); }
        HGNativePokedexUITrack edge = drag->track;
        if (drag->track.axis == HG_POKEDEX_UI_AXIS_X) {
            BAD_TRACK(continuation.fixed, 192); edge.fixed = 191;
        } else {
            BAD_TRACK(continuation.maximum, 192); edge.fixed = 255;
        }
        CHECK(HGNativePokedexUITrackIsValid(&edge, drag->family));
        state = base; state.continuation.minimum = state.continuation.maximum = state.continuation.position;
        CHECK(REJECT(HG_POKEDEX_UI_INPUT_GESTURE_UPDATE, drag->track.handle, 0));
        state = base; state.controlEnabledMask = 1; state.controls[0].flags = HG_POKEDEX_UI_CONTROL_ENABLED;
        CHECK(REJECT(HG_POKEDEX_UI_INPUT_GESTURE_UPDATE, drag->track.handle, 0));
        state = base; state.controlCount = state.visualCount = 0;
        CHECK(PokedexCheckAccepted(&state, HG_POKEDEX_UI_INPUT_GESTURE_END, drag->track.handle, 0, zero));
#undef BAD_TRACK
    }
#undef BAD
#undef REJECT
#ifdef HEARTGOLD_ABI32
    uint32_t snapshotLine = PokedexCheckSnapshots();
    if (snapshotLine) return snapshotLine;
#endif
    return 0;
}

#ifdef HG_NATIVE_POKEDEX_UI_CHECK_MAIN
// Standalone later: cc -std=c11 -Iinclude -DHG_NATIVE_POKEDEX_UI_CHECK_MAIN ios/Original/tests/pokedex_ui_check.c -o /tmp/pokedex-ui-check
int main(void) {
    uint32_t line = CheckPokedexUI();
    if (line) { fprintf(stderr, "Pokedex UI packet check line %u\n", (unsigned)line); return 1; }
    puts("PASS: Pokedex packet/resolver bounds; no original producer, host lease, rendering or gameplay executed.");
    return 0;
}
#endif
