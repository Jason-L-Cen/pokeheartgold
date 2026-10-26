// SPDX-License-Identifier: GPL-3.0-or-later
// Design artifact: normal OVY18 Pokédex only. Capture registration needs its own owner/API.
#ifndef HEARTGOLD_NATIVE_POKEDEX_UI_H
#define HEARTGOLD_NATIVE_POKEDEX_UI_H

#include <stddef.h>
#include "native_party_ui.h"

enum { HG_POKEDEX_UI_VERSION = 1, HG_POKEDEX_UI_CONTROLS = 32,
    HG_POKEDEX_UI_VISUALS = 49, HG_POKEDEX_UI_ENTRIES = 15, HG_POKEDEX_UI_CRITERIA = 10 };
enum { HG_POKEDEX_UI_FAMILY_NONE, HG_POKEDEX_UI_FAMILY_OVERVIEW, HG_POKEDEX_UI_FAMILY_LIST,
    HG_POKEDEX_UI_FAMILY_SEARCH_MENU, HG_POKEDEX_UI_FAMILY_SEARCH_ORDER, HG_POKEDEX_UI_FAMILY_SEARCH_NAME,
    HG_POKEDEX_UI_FAMILY_SEARCH_TYPE, HG_POKEDEX_UI_FAMILY_SEARCH_HEIGHT, HG_POKEDEX_UI_FAMILY_SEARCH_WEIGHT,
    HG_POKEDEX_UI_FAMILY_SEARCH_AREA, HG_POKEDEX_UI_FAMILY_SEARCH_BODY, HG_POKEDEX_UI_FAMILY_SEARCH_RESULTS,
    HG_POKEDEX_UI_FAMILY_AREA, HG_POKEDEX_UI_FAMILY_SIZE, HG_POKEDEX_UI_FAMILY_FORMS,
    HG_POKEDEX_UI_FAMILY_COMPARE, HG_POKEDEX_UI_FAMILY_ENTRY, HG_POKEDEX_UI_FAMILY_COUNT };
enum { HG_POKEDEX_UI_PHASE_OVERVIEW = 6, HG_POKEDEX_UI_PHASE_LIST = 11,
    HG_POKEDEX_UI_PHASE_LIST_DRAG = 15, HG_POKEDEX_UI_PHASE_SEARCH_MENU = 22,
    HG_POKEDEX_UI_PHASE_SEARCH_ORDER = 29, HG_POKEDEX_UI_PHASE_SEARCH_NAME = 32,
    HG_POKEDEX_UI_PHASE_SEARCH_TYPE = 35, HG_POKEDEX_UI_PHASE_SEARCH_HEIGHT = 38,
    HG_POKEDEX_UI_PHASE_HEIGHT_MAX_DRAG = 40, HG_POKEDEX_UI_PHASE_HEIGHT_MIN_DRAG = 41,
    HG_POKEDEX_UI_PHASE_SEARCH_WEIGHT = 45, HG_POKEDEX_UI_PHASE_WEIGHT_MAX_DRAG = 47,
    HG_POKEDEX_UI_PHASE_WEIGHT_MIN_DRAG = 48, HG_POKEDEX_UI_PHASE_SEARCH_AREA = 52,
    HG_POKEDEX_UI_PHASE_SEARCH_BODY = 55, HG_POKEDEX_UI_PHASE_SEARCH_RESULTS = 57,
    HG_POKEDEX_UI_PHASE_RESULTS_DRAG = 60, HG_POKEDEX_UI_PHASE_AREA = 69,
    HG_POKEDEX_UI_PHASE_AREA_DRAG = 73, HG_POKEDEX_UI_PHASE_SIZE = 77,
    HG_POKEDEX_UI_PHASE_FORMS = 80, HG_POKEDEX_UI_PHASE_FORMS_DRAG = 84,
    HG_POKEDEX_UI_PHASE_COMPARE = 87, HG_POKEDEX_UI_PHASE_ENTRY = 90 };
enum { HG_POKEDEX_UI_CRITERION_ORDER, HG_POKEDEX_UI_CRITERION_LETTER, HG_POKEDEX_UI_CRITERION_TYPE1,
    HG_POKEDEX_UI_CRITERION_TYPE2, HG_POKEDEX_UI_CRITERION_HEIGHT_MIN, HG_POKEDEX_UI_CRITERION_HEIGHT_MAX,
    HG_POKEDEX_UI_CRITERION_WEIGHT_MIN, HG_POKEDEX_UI_CRITERION_WEIGHT_MAX,
    HG_POKEDEX_UI_CRITERION_AREA, HG_POKEDEX_UI_CRITERION_BODY };
enum { HG_POKEDEX_UI_INPUT_CONTROL, HG_POKEDEX_UI_INPUT_DIRECTION, HG_POKEDEX_UI_INPUT_CONFIRM,
    HG_POKEDEX_UI_INPUT_BACK, HG_POKEDEX_UI_INPUT_GESTURE_BEGIN,
    HG_POKEDEX_UI_INPUT_GESTURE_UPDATE, HG_POKEDEX_UI_INPUT_GESTURE_END };
enum { HG_POKEDEX_UI_KEY_UP, HG_POKEDEX_UI_KEY_DOWN, HG_POKEDEX_UI_KEY_LEFT,
    HG_POKEDEX_UI_KEY_RIGHT, HG_POKEDEX_UI_KEY_CONFIRM, HG_POKEDEX_UI_KEY_BACK };
enum { HG_POKEDEX_UI_GRID_NONE, HG_POKEDEX_UI_GRID_TOUCH, HG_POKEDEX_UI_GRID_BUTTONS };
enum { HG_POKEDEX_UI_UNSEEN, HG_POKEDEX_UI_SEEN, HG_POKEDEX_UI_CAUGHT };
enum { HG_POKEDEX_UI_HANDLE_MAX, HG_POKEDEX_UI_HANDLE_MIN };
enum { HG_POKEDEX_UI_TAB_AREA, HG_POKEDEX_UI_TAB_SIZE, HG_POKEDEX_UI_TAB_FORMS, HG_POKEDEX_UI_TAB_BACK };
#define HG_POKEDEX_UI_INDEX_NONE UINT32_MAX
enum { HG_POKEDEX_UI_BUSY_NONE, HG_POKEDEX_UI_BUSY_BUILD, HG_POKEDEX_UI_BUSY_GRAPHICS,
    HG_POKEDEX_UI_BUSY_TRANSITION, HG_POKEDEX_UI_BUSY_RETIRE, HG_POKEDEX_UI_BUSY_UNSUPPORTED,
    HG_POKEDEX_UI_BUSY_DRAG };
enum { HG_POKEDEX_UI_CONTROL_ENABLED = 1, HG_POKEDEX_UI_CONTROL_SELECTED = 2 };
enum { HG_POKEDEX_UI_ROLE_NONE, HG_POKEDEX_UI_ROLE_OPEN_LIST, HG_POKEDEX_UI_ROLE_OPEN_SEARCH,
    HG_POKEDEX_UI_ROLE_EXIT, HG_POKEDEX_UI_ROLE_DEX_MODE, HG_POKEDEX_UI_ROLE_GRID_CELL,
    HG_POKEDEX_UI_ROLE_PAGE, HG_POKEDEX_UI_ROLE_CRY, HG_POKEDEX_UI_ROLE_LANGUAGE,
    HG_POKEDEX_UI_ROLE_ENTRY, HG_POKEDEX_UI_ROLE_CRITERION, HG_POKEDEX_UI_ROLE_RESET,
    HG_POKEDEX_UI_ROLE_SEARCH, HG_POKEDEX_UI_ROLE_OPTION, HG_POKEDEX_UI_ROLE_ACCEPT,
    HG_POKEDEX_UI_ROLE_CANCEL, HG_POKEDEX_UI_ROLE_RANGE_STEP, HG_POKEDEX_UI_ROLE_RANGE_HANDLE,
    HG_POKEDEX_UI_ROLE_SCROLLBAR, HG_POKEDEX_UI_ROLE_REGION, HG_POKEDEX_UI_ROLE_TIME,
    HG_POKEDEX_UI_ROLE_TAB, HG_POKEDEX_UI_ROLE_FORM_STEP, HG_POKEDEX_UI_ROLE_COMPARE,
    HG_POKEDEX_UI_ROLE_FACING, HG_POKEDEX_UI_ROLE_SIDE, HG_POKEDEX_UI_ROLE_REPLAY,
    HG_POKEDEX_UI_ROLE_ENTRY_BACK, HG_POKEDEX_UI_ROLE_COUNT };
enum { HG_POKEDEX_UI_TRANSPORT_NONE, HG_POKEDEX_UI_TRANSPORT_KEY,
    HG_POKEDEX_UI_TRANSPORT_TOUCH, HG_POKEDEX_UI_TRANSPORT_TRACK };
enum { HG_POKEDEX_UI_DRAG_NONE, HG_POKEDEX_UI_DRAG_LIST, HG_POKEDEX_UI_DRAG_RESULTS,
    HG_POKEDEX_UI_DRAG_AREA, HG_POKEDEX_UI_DRAG_FORMS,
    HG_POKEDEX_UI_DRAG_HEIGHT, HG_POKEDEX_UI_DRAG_WEIGHT };
enum { HG_POKEDEX_UI_AXIS_X, HG_POKEDEX_UI_AXIS_Y };
enum { HG_POKEDEX_UI_VISUAL_WINDOW = 1, HG_POKEDEX_UI_VISUAL_RASTER = 2 };
enum { HG_POKEDEX_UI_IMAGE_NONE, HG_POKEDEX_UI_IMAGE_GRID_CELL, HG_POKEDEX_UI_IMAGE_COUNT_LABEL,
    HG_POKEDEX_UI_IMAGE_COUNT_NUMBER, HG_POKEDEX_UI_IMAGE_SELECTED_NAME, HG_POKEDEX_UI_IMAGE_HEADER,
    HG_POKEDEX_UI_IMAGE_CATEGORY, HG_POKEDEX_UI_IMAGE_FLAVOR, HG_POKEDEX_UI_IMAGE_MEASURE_LABEL,
    HG_POKEDEX_UI_IMAGE_MEASURE_VALUE, HG_POKEDEX_UI_IMAGE_PORTRAIT, HG_POKEDEX_UI_IMAGE_TYPE,
    HG_POKEDEX_UI_IMAGE_FOOTPRINT, HG_POKEDEX_UI_IMAGE_CAUGHT, HG_POKEDEX_UI_IMAGE_LANGUAGE,
    HG_POKEDEX_UI_IMAGE_FOOTER, HG_POKEDEX_UI_IMAGE_CRITERION_LABEL, HG_POKEDEX_UI_IMAGE_CRITERION_VALUE,
    HG_POKEDEX_UI_IMAGE_OPTION, HG_POKEDEX_UI_IMAGE_HELP, HG_POKEDEX_UI_IMAGE_BODY_SHAPE,
    HG_POKEDEX_UI_IMAGE_TRACK, HG_POKEDEX_UI_IMAGE_ARROW, HG_POKEDEX_UI_IMAGE_AREA_MAP,
    HG_POKEDEX_UI_IMAGE_AREA_ROW, HG_POKEDEX_UI_IMAGE_TAB, HG_POKEDEX_UI_IMAGE_SIZE_SCENE,
    HG_POKEDEX_UI_IMAGE_FORM_PICTURE, HG_POKEDEX_UI_IMAGE_FORM_ROW, HG_POKEDEX_UI_IMAGE_FORM_ICON,
    HG_POKEDEX_UI_IMAGE_COMPARE_CONTROL, HG_POKEDEX_UI_IMAGE_ENTRY_BODY, HG_POKEDEX_UI_IMAGE_COUNT };

typedef struct HGNativePokedexUIInput { uint32_t keys, touchPressed, touchX, touchY; } HGNativePokedexUIInput;
typedef struct HGNativePokedexUIEntry { uint32_t listIndex, dexNo, species, caughtState; } HGNativePokedexUIEntry;
// Geometry is copied from the actual original track. Handle0=max and1=min for ranges; otherwise0.
typedef struct HGNativePokedexUITrack {
    uint32_t kind, handle, axis, fixed, minimum, maximum, position;
} HGNativePokedexUITrack;
typedef struct HGNativePokedexUIControl {
    // index is the source semantic cell/option/tab/step, visualId is a visuals[] array index.
    uint32_t role, index, flags, visualId, transportKind;
    union { uint32_t key; struct { uint32_t x, y; } touch; HGNativePokedexUITrack track; } transport;
} HGNativePokedexUIControl;
typedef struct HGNativePokedexUIVisual {
    // WINDOW index is the original Window ID; RASTER index is the role's semantic component index.
    uint32_t role, index, kind;
    union { HGNativePartyUIWindow window;
        struct { uint32_t engine, x, y, width, height; } raster; } source;
} HGNativePokedexUIVisual;
typedef union HGNativePokedexUIFamilyData {
    uint32_t words[8]; // Zero unused words, including the tail of the selected member.
    struct { uint32_t selectedHandle, minIndex, maxIndex, minValue, maxValue; } range;
    struct { uint32_t region, time, listPosition, mapCount; } area;
    struct { uint32_t count, selected, left, right, side, facing; } forms;
} HGNativePokedexUIFamilyData;
typedef struct HGNativePokedexUIState {
    uint32_t version, byteSize, active, ownerGeneration, revision, frameSerial, phase, family, ready, busyReason;
    uint32_t dexMode, nationalDex, species, caughtState, language, entryTab, entryFromSearch;
    uint32_t page, cursor, listLength, resultCount, caughtCount, gridCursor, gridInputMode;
    // gridCursor=INDEX_NONE and gridInputMode=GRID_NONE without an actual gridInput.
    // Otherwise gridInputMode maps original isButtons: false->GRID_TOUCH, true->GRID_BUTTONS.
    uint32_t criteria[HG_POKEDEX_UI_CRITERIA];
    HGNativePokedexUIFamilyData data;
    uint32_t keyEnabledMask, controlCount, controlEnabledMask, visualCount;
    HGNativePokedexUITrack continuation; // Original state only; never a host token/admission flag.
    HGNativePokedexUIEntry entries[HG_POKEDEX_UI_ENTRIES];
    HGNativePokedexUIControl controls[HG_POKEDEX_UI_CONTROLS];
    HGNativePokedexUIVisual visuals[HG_POKEDEX_UI_VISUALS];
} HGNativePokedexUIState;

// Layout candidates to verify on the actual guest/host compilers before implementation release.
_Static_assert(sizeof(HGNativePartyUIWindow) == 52, "existing Window ABI");
_Static_assert(HG_POKEDEX_UI_CONTROLS >= 29 && HG_POKEDEX_UI_CONTROLS <= 32, "control mask capacity");
_Static_assert(HG_POKEDEX_UI_VISUALS >= 49, "curated visual capacity");
_Static_assert(sizeof(HGNativePokedexUIInput) == 16, "input ABI");
_Static_assert(sizeof(HGNativePokedexUIEntry) == 16, "entry ABI");
_Static_assert(sizeof(HGNativePokedexUITrack) == 28, "track ABI");
_Static_assert(sizeof(HGNativePokedexUIControl) == 48, "control ABI");
_Static_assert(sizeof(HGNativePokedexUIVisual) == 64, "visual ABI");
_Static_assert(sizeof(HGNativePokedexUIFamilyData) == 32, "family ABI");
_Static_assert(offsetof(HGNativePokedexUIState, data) == 136, "family offset");
_Static_assert(offsetof(HGNativePokedexUIState, continuation) == 184, "continuation offset");
_Static_assert(offsetof(HGNativePokedexUIState, entries) == 212, "entries offset");
_Static_assert(offsetof(HGNativePokedexUIState, controls) == 452, "controls offset");
_Static_assert(offsetof(HGNativePokedexUIState, visuals) == 1988, "visuals offset");
_Static_assert(sizeof(HGNativePokedexUIState) == 5124, "state ABI");
_Static_assert(_Alignof(HGNativePokedexUIState) == 4, "state alignment");

// Owner thread: copy the scalar packet and every borrowed asset before advancing/freeing resources.
// The host separately bounds-checks Window.pixelBuffer against current guest memory.
const HGNativePokedexUIState *HGNativePokedexUIGetState(void);

static inline uint32_t HGNativePokedexUIMask(uint32_t count) {
    return count >= 32 ? UINT32_MAX : count ? (1u << count) - 1u : 0;
}

// phase/control/visual ceilings and exact original GridInput targets (zero means no GridInput).
static inline uint32_t HGNativePokedexUIFamilyLimit(uint32_t family, uint32_t column) {
    static const uint8_t limits[HG_POKEDEX_UI_FAMILY_COUNT][4] = {
        { 0, 0, 0, 0 }, { 6, 5, 10, 0 }, { 11, 25, 42, 0 }, { 22, 10, 38, 10 },
        { 29, 8, 28, 8 }, { 32, 29, 49, 29 }, { 35, 20, 41, 20 }, { 38, 8, 29, 4 },
        { 45, 8, 29, 4 }, { 52, 6, 26, 6 }, { 55, 17, 36, 17 }, { 57, 24, 45, 0 },
        { 69, 17, 26, 0 }, { 77, 5, 19, 0 }, { 80, 13, 32, 0 }, { 87, 8, 17, 0 }, { 90, 5, 7, 0 }
    };
    return family < HG_POKEDEX_UI_FAMILY_COUNT && column < 4 ? limits[family][column] : 0;
}

static inline uint32_t HGNativePokedexUIDragKind(uint32_t family) {
    switch (family) {
    case HG_POKEDEX_UI_FAMILY_LIST: return HG_POKEDEX_UI_DRAG_LIST;
    case HG_POKEDEX_UI_FAMILY_SEARCH_RESULTS: return HG_POKEDEX_UI_DRAG_RESULTS;
    case HG_POKEDEX_UI_FAMILY_AREA: return HG_POKEDEX_UI_DRAG_AREA;
    case HG_POKEDEX_UI_FAMILY_FORMS: return HG_POKEDEX_UI_DRAG_FORMS;
    case HG_POKEDEX_UI_FAMILY_SEARCH_HEIGHT: return HG_POKEDEX_UI_DRAG_HEIGHT;
    case HG_POKEDEX_UI_FAMILY_SEARCH_WEIGHT: return HG_POKEDEX_UI_DRAG_WEIGHT;
    default: return HG_POKEDEX_UI_DRAG_NONE;
    }
}

static inline uint32_t HGNativePokedexUITrackPhase(const HGNativePokedexUITrack *track) {
    if (!track) return 0;
    if (track->kind == HG_POKEDEX_UI_DRAG_HEIGHT) return track->handle < 2 ? HG_POKEDEX_UI_PHASE_HEIGHT_MAX_DRAG + track->handle : 0;
    if (track->kind == HG_POKEDEX_UI_DRAG_WEIGHT) return track->handle < 2 ? HG_POKEDEX_UI_PHASE_WEIGHT_MAX_DRAG + track->handle : 0;
    if (track->handle) return 0;
    switch (track->kind) {
    case HG_POKEDEX_UI_DRAG_LIST: return HG_POKEDEX_UI_PHASE_LIST_DRAG;
    case HG_POKEDEX_UI_DRAG_RESULTS: return HG_POKEDEX_UI_PHASE_RESULTS_DRAG;
    case HG_POKEDEX_UI_DRAG_AREA: return HG_POKEDEX_UI_PHASE_AREA_DRAG;
    case HG_POKEDEX_UI_DRAG_FORMS: return HG_POKEDEX_UI_PHASE_FORMS_DRAG;
    default: return 0;
    }
}

static inline uint32_t HGNativePokedexUITrackIsValid(const HGNativePokedexUITrack *track, uint32_t family) {
    return track && track->kind && track->kind == HGNativePokedexUIDragKind(family)
        && HGNativePokedexUITrackPhase(track) && track->axis <= HG_POKEDEX_UI_AXIS_Y
        && track->minimum <= track->position && track->position <= track->maximum
        && track->minimum < track->maximum
        && track->maximum <= (track->axis == HG_POKEDEX_UI_AXIS_X ? 255u : 191u)
        && track->fixed <= (track->axis == HG_POKEDEX_UI_AXIS_X ? 191u : 255u);
}

static inline uint32_t HGNativePokedexUIVisualIsValid(const HGNativePokedexUIVisual *visual) {
    if (!visual || !visual->role || visual->role >= HG_POKEDEX_UI_IMAGE_COUNT) return 0;
    if (visual->kind == HG_POKEDEX_UI_VISUAL_WINDOW) {
        const HGNativePartyUIWindow *w = &visual->source.window;
        return w->tileWidth && w->tileWidth <= 32 && w->tileHeight && w->tileHeight <= 6
            && w->engine <= 1 && w->clearColor <= 15 && w->pixelBuffer
            && w->pixelBuffer <= UINT32_MAX - w->tileWidth * w->tileHeight * 32u;
    }
    if (visual->kind != HG_POKEDEX_UI_VISUAL_RASTER) return 0;
    return visual->source.raster.engine <= 1 && visual->source.raster.x < 256 && visual->source.raster.y < 192
        && visual->source.raster.width && visual->source.raster.width <= 256 - visual->source.raster.x
        && visual->source.raster.height && visual->source.raster.height <= 192 - visual->source.raster.y;
}

// No original handler/callback or mutation is called here. Transports must be produced by the
// original-table resolver, then re-read/re-resolved on the owner thread before PAD/TP sampling.
// UPDATE/END additionally require the host-private accepted-BEGIN lease; this pure function
// cannot prove that history. Cancellation always releases host input without retired pointers.
static inline uint32_t HGNativePokedexUIResolveInput(const HGNativePokedexUIState *state,
    uint32_t generation, uint32_t revision, uint32_t phase, uint32_t kind, uint32_t index,
    uint32_t value, HGNativePokedexUIInput *out) {
    if (!out) return 0;
    out->keys = out->touchPressed = out->touchX = out->touchY = 0;
    if (!state || state->version != HG_POKEDEX_UI_VERSION || state->byteSize != sizeof(*state)
        || state->active != 1 || !generation || generation != state->ownerGeneration
        || !revision || revision != state->revision || phase != state->phase
        || !state->family || state->family >= HG_POKEDEX_UI_FAMILY_COUNT
        || state->ready > 1 || state->busyReason > HG_POKEDEX_UI_BUSY_DRAG
        || state->gridInputMode > HG_POKEDEX_UI_GRID_BUTTONS
        || (HGNativePokedexUIFamilyLimit(state->family, 3)
            ? (!state->gridInputMode || state->gridCursor >= HGNativePokedexUIFamilyLimit(state->family, 3))
            : (state->gridInputMode != HG_POKEDEX_UI_GRID_NONE || state->gridCursor != HG_POKEDEX_UI_INDEX_NONE))
        || state->controlCount > HGNativePokedexUIFamilyLimit(state->family, 1)
        || state->visualCount > HGNativePokedexUIFamilyLimit(state->family, 2)
        || (state->controlEnabledMask & ~HGNativePokedexUIMask(state->controlCount))
        || (state->keyEnabledMask & ~63u)) return 0;
    for (uint32_t i = 0; i < state->visualCount; ++i)
        if (!HGNativePokedexUIVisualIsValid(&state->visuals[i])) return 0;
    for (uint32_t i = 0; i < state->controlCount; ++i) {
        const HGNativePokedexUIControl *c = &state->controls[i];
        if (!c->role || c->role >= HG_POKEDEX_UI_ROLE_COUNT || (c->flags & ~3u)
            || c->visualId >= state->visualCount
            || !!(c->flags & HG_POKEDEX_UI_CONTROL_ENABLED) != !!(state->controlEnabledMask & (1u << i))) return 0;
    }
    if (kind == HG_POKEDEX_UI_INPUT_GESTURE_UPDATE || kind == HG_POKEDEX_UI_INPUT_GESTURE_END) {
        const HGNativePokedexUITrack *t = &state->continuation;
        if (state->ready || state->busyReason != HG_POKEDEX_UI_BUSY_DRAG || state->keyEnabledMask
            || state->controlEnabledMask || !HGNativePokedexUITrackIsValid(t, state->family)
            || phase != HGNativePokedexUITrackPhase(t) || index != t->handle || value > 65535
            || (kind == HG_POKEDEX_UI_INPUT_GESTURE_END && value)) return 0;
        if (kind == HG_POKEDEX_UI_INPUT_GESTURE_END) return 1;
        uint32_t position = t->minimum + ((t->maximum - t->minimum) * value + 32767u) / 65535u;
        out->touchPressed = 1;
        out->touchX = t->axis == HG_POKEDEX_UI_AXIS_X ? position : t->fixed;
        out->touchY = t->axis == HG_POKEDEX_UI_AXIS_Y ? position : t->fixed;
        return 1;
    }
    if (!state->ready || state->busyReason || state->continuation.kind
        || phase != HGNativePokedexUIFamilyLimit(state->family, 0) || value) return 0;
    if (kind == HG_POKEDEX_UI_INPUT_DIRECTION || kind == HG_POKEDEX_UI_INPUT_CONFIRM || kind == HG_POKEDEX_UI_INPUT_BACK) {
        uint32_t key;
        if (kind == HG_POKEDEX_UI_INPUT_DIRECTION) { if (index >= 4) return 0; key = index; }
        else { if (index) return 0; key = kind == HG_POKEDEX_UI_INPUT_CONFIRM ? 4 : 5; }
        if (!(state->keyEnabledMask & (1u << key))) return 0;
        static const uint32_t keys[6] = { 64, 128, 32, 16, 1, 2 }; // Original up/down/left/right/A/B.
        out->keys = keys[key]; return 1;
    }
    if ((kind != HG_POKEDEX_UI_INPUT_CONTROL && kind != HG_POKEDEX_UI_INPUT_GESTURE_BEGIN)
        || index >= state->controlCount || !(state->controlEnabledMask & (1u << index))) return 0;
    const HGNativePokedexUIControl *c = &state->controls[index];
    if (kind == HG_POKEDEX_UI_INPUT_GESTURE_BEGIN) {
        const HGNativePokedexUITrack *t = &c->transport.track;
        if (c->transportKind != HG_POKEDEX_UI_TRANSPORT_TRACK || !HGNativePokedexUITrackIsValid(t, state->family)) return 0;
        out->touchPressed = 1;
        out->touchX = t->axis == HG_POKEDEX_UI_AXIS_X ? t->position : t->fixed;
        out->touchY = t->axis == HG_POKEDEX_UI_AXIS_Y ? t->position : t->fixed;
        return 1;
    }
    if (c->transportKind == HG_POKEDEX_UI_TRANSPORT_KEY) {
        uint32_t key = c->transport.key;
        if (!key || (key & ~4095u) || (key & (key - 1u))) return 0;
        out->keys = key; return 1;
    }
    if (c->transportKind != HG_POKEDEX_UI_TRANSPORT_TOUCH || c->transport.touch.x > 255 || c->transport.touch.y > 191) return 0;
    out->touchPressed = 1; out->touchX = c->transport.touch.x; out->touchY = c->transport.touch.y;
    return 1;
}

#endif
