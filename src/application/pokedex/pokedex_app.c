#include <dsprot.h>

#include "application/pokedex/pokedex_internal.h"

#include "dex_mon_measures.h"
#include "overlay_18.h"
#include "sound_system.h"
#ifdef HEARTGOLD_NATIVE
#include <nitro/gx/native_geometry.h>
#include "pokedex_util.h"
#include "screen_fade.h"
#include <string.h>
#endif

FS_EXTERN_OVERLAY(ds_protect);

void Pokedex_OnEmulatorDetected(void);
void Pokedex_OnFlashcartDetected(void);
void Pokedex_OnDummyCheckFailed(void);

BOOL Pokedex_Init(OverlayManager *man, int *state) {
    PokedexAppData *appData;
#ifdef HEARTGOLD_NATIVE
    (void)HGNativeSetViewWidth(256);
#endif

    Heap_Create(HEAP_ID_3, HEAP_ID_POKEDEX_APP, 0x61000);
    appData = OverlayManager_CreateAndGetData(man, sizeof(PokedexAppData), HEAP_ID_POKEDEX_APP);
    MI_CpuClear8(appData, sizeof(PokedexAppData));
    appData->args = OverlayManager_GetArgs(man);
    appData->nextSeq = 5;
    appData->dexMode = PokedexResumeState_GetDexMode(appData->args->resumeState);
    if (Pokedex_GetNatDexFlag(appData->args->pokedex)) {
        appData->nationalDex = TRUE;
        if (appData->dexMode == 2) {
            appData->dexMode = 1;
        }
    } else {
        appData->nationalDex = FALSE;
        if (appData->dexMode == 2) {
            appData->dexMode = 0;
        }
    }
    if (Pokedex_CheckMonCaughtFlag(appData->args->pokedex, SPECIES_GIRATINA) == TRUE) {
        SetDexBanksByGiratinaForm(Pokedex_GetSeenFormByIdx(appData->args->pokedex, SPECIES_GIRATINA, 0));
    } else {
        SetDexBanksByGiratinaForm(GIRATINA_ALTERED);
    }
    GF_SndHandleSetPlayerVolume(1, 42);
    appData->language = 2;
#ifdef HEARTGOLD_NATIVE
    NativePokedexUIBind(appData);
#endif
    return TRUE;
}

BOOL Pokedex_Main(OverlayManager *man, int *state) {
    PokedexAppData *appData = OverlayManager_GetData(man);

    if (!PokedexApp_RunMainSeq(appData, state)) {
        return TRUE;
    } else {
        return FALSE;
    }
}

BOOL Pokedex_Exit(OverlayManager *man, int *state) {
    PokedexAppData *appData = OverlayManager_GetData(man);
#ifdef HEARTGOLD_NATIVE
    NativePokedexUIBind(NULL);
#endif

    FS_LoadOverlay(MI_PROCESSOR_ARM9, FS_OVERLAY_ID(ds_protect));
    if (DSProt_DetectEmulator(Pokedex_OnEmulatorDetected)) {
        Heap_AllocAtEnd(HEAP_ID_3, 1000);
    }
    PokedexResumeState_Set(appData->args->resumeState, PokedexApp_GetCursorSpecies(appData), appData->dexMode);
    if (DSProt_DetectFlashcart(Pokedex_OnFlashcartDetected)) {
        Heap_AllocAtEnd(HEAP_ID_3, 1000);
    }
    OverlayManager_FreeData(man);
    Heap_Destroy(HEAP_ID_POKEDEX_APP);
    GF_SndHandleSetPlayerVolume(1, 127);
    Sound_ClearSecondaryScene();
    if (!DSProt_DetectNotDummy(Pokedex_OnDummyCheckFailed)) {
        Heap_AllocAtEnd(HEAP_ID_3, 1000);
    }
    FS_UnloadOverlay(MI_PROCESSOR_ARM9, FS_OVERLAY_ID(ds_protect));
    return TRUE;
}

void Pokedex_OnEmulatorDetected(void) {
    Heap_AllocAtEnd(HEAP_ID_3, 1000);
}

void Pokedex_OnFlashcartDetected(void) {
    Heap_AllocAtEnd(HEAP_ID_3, 1000);
}

void Pokedex_OnDummyCheckFailed(void) {
}
#ifdef HEARTGOLD_NATIVE
static PokedexAppData *sNativePokedex;
static HGNativePokedexUIState sNativePokedexUI, sNativePokedexPrevious;
static u32 sNativePokedexPhase, sNativePokedexFrame;
static BOOL sNativePokedexBuilding;

static u32 NativePokedexNext(u32 value) { return value + 1 ? value + 1 : 1; }
static u32 NativePokedexFamily(u32 phase);
static HGNativePokedexUIState NativePokedexCurrent(PokedexAppData *app, BOOL ownWindows);

void NativePokedexUIBind(PokedexAppData *app) {
    if (app == sNativePokedex) return;
    sNativePokedex = app;
    u32 generation = NativePokedexUIAdvanceGeneration();
    MI_CpuClear8(&sNativePokedexUI, sizeof(sNativePokedexUI));
    MI_CpuClear8(&sNativePokedexPrevious, sizeof(sNativePokedexPrevious));
    sNativePokedexUI.version = HG_POKEDEX_UI_VERSION;
    sNativePokedexUI.byteSize = sizeof(sNativePokedexUI);
    sNativePokedexUI.ownerGeneration = generation;
    sNativePokedexUI.revision = 1;
    sNativePokedexUI.active = app != NULL;
    sNativePokedexUI.gridCursor = HG_POKEDEX_UI_INDEX_NONE;
    sNativePokedexPhase = 0; sNativePokedexBuilding = FALSE;
}

void NativePokedexUIBegin(PokedexAppData *app, u32 phase) {
    if (!app || app != sNativePokedex) return;
    sNativePokedexBuilding = TRUE;
    sNativePokedexPhase = phase;
    NativePokedexUIPrepareBuild(&sNativePokedexUI, &sNativePokedexPrevious, phase, NativePokedexFamily(phase), NULL);
    // These same-family paths retire an area list or clear reused Compare windows.
    if (sNativePokedexPhase == 75 || sNativePokedexPhase == 88) NativePokedexUIRetire(app);
}

void NativePokedexUIRetire(PokedexAppData *app) {
    if (!app || app != sNativePokedex) return;
    sNativePokedexUI.ownerGeneration = NativePokedexUIAdvanceGeneration();
    sNativePokedexUI.ready = FALSE; sNativePokedexUI.busyReason = HG_POKEDEX_UI_BUSY_RETIRE;
    sNativePokedexUI.controlEnabledMask = sNativePokedexUI.keyEnabledMask = 0;
    sNativePokedexUI.controlCount = sNativePokedexUI.visualCount = 0;
    MI_CpuClear8(&sNativePokedexUI.continuation, sizeof(sNativePokedexUI) - offsetof(HGNativePokedexUIState, continuation));
}

void NativePokedexUIEnd(PokedexAppData *app, u32 phase, BOOL afterDraw) {
    if (!app || app != sNativePokedex) return;
    sNativePokedexPhase = phase;
    if (!afterDraw) {
        NativePokedexUIPrepareBuild(&sNativePokedexUI, &sNativePokedexPrevious, phase, NativePokedexFamily(phase), NULL);
        return;
    }
    HGNativePokedexUIState current = NativePokedexCurrent(app, FALSE);
    NativePokedexUIPrepareBuild(&sNativePokedexUI, &sNativePokedexPrevious, phase, NativePokedexFamily(phase), &current);
    sNativePokedexBuilding = FALSE;
}

u32 NativePokedexUIVisual(HGNativePokedexUIState *state, const HGNativePokedexUIVisual *visual) {
    if (!HGNativePokedexUIVisualIsValid(visual)) return HG_POKEDEX_UI_INDEX_NONE;
    for (u32 i = 0; i < state->visualCount; ++i)
        if (state->visuals[i].role == visual->role && state->visuals[i].index == visual->index && state->visuals[i].kind == visual->kind) return i;
    GF_ASSERT(state->visualCount < HGNativePokedexUIFamilyLimit(state->family, 2));
    if (state->visualCount >= HGNativePokedexUIFamilyLimit(state->family, 2)) {
        state->busyReason = HG_POKEDEX_UI_BUSY_UNSUPPORTED; return HG_POKEDEX_UI_INDEX_NONE;
    }
    state->visuals[state->visualCount] = *visual;
    return state->visualCount++;
}

u32 NativePokedexUIRaster(HGNativePokedexUIState *state, u32 role, u32 index, u32 engine, int x, int y, int width, int height) {
    HGNativePokedexUIVisual v;
    MI_CpuClear8(&v, sizeof(v));
    if (x < 0 || y < 0 || width <= 0 || height <= 0) return HG_POKEDEX_UI_INDEX_NONE;
    v.role = role; v.index = index; v.kind = HG_POKEDEX_UI_VISUAL_RASTER;
    v.source.raster.engine = engine; v.source.raster.x = x; v.source.raster.y = y;
    v.source.raster.width = width; v.source.raster.height = height;
    return NativePokedexUIVisual(state, &v);
}

u32 NativePokedexUIFindWindow(const HGNativePokedexUIState *state, u32 window) {
    for (u32 i = 0; i < state->visualCount; ++i)
        if (state->visuals[i].kind == HG_POKEDEX_UI_VISUAL_WINDOW && state->visuals[i].index == window) return i;
    return HG_POKEDEX_UI_INDEX_NONE;
}

static u32 NativePokedexFamily(u32 phase) {
    for (u32 i = 1; i < HG_POKEDEX_UI_FAMILY_COUNT; ++i)
        if (HGNativePokedexUIFamilyLimit(i, 0) == phase) return i;
    switch (phase) {
    case 15: return HG_POKEDEX_UI_FAMILY_LIST;
    case 60: return HG_POKEDEX_UI_FAMILY_SEARCH_RESULTS;
    case 73: return HG_POKEDEX_UI_FAMILY_AREA;
    case 84: return HG_POKEDEX_UI_FAMILY_FORMS;
    case 40: case 41: return HG_POKEDEX_UI_FAMILY_SEARCH_HEIGHT;
    case 47: case 48: return HG_POKEDEX_UI_FAMILY_SEARCH_WEIGHT;
    default: return HG_POKEDEX_UI_FAMILY_NONE;
    }
}

static BOOL NativePokedexGraphics(PokedexAppData *app, u32 family) {
    if (!app->bgConfig || !app->bgCellManager || !app->spriteManager || !app->paletteData
        || ((app->bgConfig->bufferTransferScheduled | app->bgConfig->scrollScheduled) & 255)
        || app->subBgPriorityRequest || PaletteData_GetSelectedBuffersBitmask(app->paletteData)
        || app->paletteData->transparentBit || !IsPaletteFadeFinished()) return FALSE;
    u32 cell = family == HG_POKEDEX_UI_FAMILY_OVERVIEW ? 0 : family == HG_POKEDEX_UI_FAMILY_LIST ? 1
        : family == HG_POKEDEX_UI_FAMILY_SEARCH_MENU ? 6 : family == HG_POKEDEX_UI_FAMILY_SEARCH_RESULTS ? 8
        : family >= HG_POKEDEX_UI_FAMILY_AREA ? 16 : 7;
    if (BgCellManager_IsCellMoving(app->bgCellManager, cell)) return FALSE;
    if ((family == HG_POKEDEX_UI_FAMILY_AREA || family == HG_POKEDEX_UI_FAMILY_FORMS)
        && HGNativePokedexUIFamilyLimit(family, 0) == sNativePokedexPhase)
        for (u32 i = 17; i <= 22; ++i) if (BgCellManager_IsCellMoving(app->bgCellManager, i)) return FALSE;
    return TRUE;
}

static HGNativePokedexUIState NativePokedexCurrent(PokedexAppData *app, BOOL ownWindows) {
    HGNativePokedexUIState next;
    MI_CpuClear8(&next, sizeof(next));
    next.version = HG_POKEDEX_UI_VERSION; next.byteSize = sizeof(next); next.active = TRUE;
    next.ownerGeneration = sNativePokedexUI.ownerGeneration; next.revision = sNativePokedexUI.revision;
    next.frameSerial = sNativePokedexFrame; next.phase = sNativePokedexPhase;
    next.family = NativePokedexFamily(next.phase); next.busyReason = HG_POKEDEX_UI_BUSY_TRANSITION;
    next.gridCursor = HG_POKEDEX_UI_INDEX_NONE;
    next.dexMode = app->dexMode; next.nationalDex = !!app->nationalDex; next.language = app->language;
    next.entryTab = app->entryTab; next.entryFromSearch = !!app->entryFromSearch;
    next.page = app->listPage; next.cursor = app->listCursor;
    next.resultCount = app->searchResults.count; next.caughtCount = app->searchResults.caughtCount;
    if (app->searchResults.count && app->searchResults.count <= 493)
        next.listLength = PokedexApp_GetListLength(app, next.family != HG_POKEDEX_UI_FAMILY_SEARCH_RESULTS);
    u32 selected = app->listPage * 15u + app->listCursor;
    if (app->listCursor < 15 && selected < 518) {
        next.species = app->listEntries[selected].species; next.caughtState = app->listEntries[selected].caughtState;
    }
    if (next.family >= HG_POKEDEX_UI_FAMILY_AREA) next.species = app->curSpecies;
    for (u32 i = 0; i < 15; ++i) {
        u32 index = app->listPage * 15u + i;
        next.entries[i].listIndex = index < 518 ? index : HG_POKEDEX_UI_INDEX_NONE;
        if (index < 518) {
            next.entries[i].species = app->listEntries[index].species;
            next.entries[i].caughtState = app->listEntries[index].caughtState;
            if (next.entries[i].species) next.entries[i].dexNo = Pokedex_ConvertToCurrentDexNo(app->dexMode, next.entries[i].species);
        }
    }
    MI_CpuCopy8(app->dexSearchCriteria, next.criteria, sizeof(next.criteria));
    if (next.family && HGNativePokedexUIFamilyLimit(next.family, 3) && app->gridInput) {
        next.gridCursor = app->gridInput->nextInput;
        next.gridInputMode = app->gridInput->isButtons ? HG_POKEDEX_UI_GRID_BUTTONS : HG_POKEDEX_UI_GRID_TOUCH;
    }
    if (next.family == HG_POKEDEX_UI_FAMILY_SEARCH_HEIGHT || next.family == HG_POKEDEX_UI_FAMILY_SEARCH_WEIGHT) {
        u32 min = next.family == HG_POKEDEX_UI_FAMILY_SEARCH_HEIGHT ? 4 : 6;
        next.data.range.selectedHandle = next.gridCursor < 2 ? next.gridCursor : HG_POKEDEX_UI_INDEX_NONE;
        next.data.range.minIndex = app->dexSearchCriteria[min]; next.data.range.maxIndex = app->dexSearchCriteria[min + 1];
        if (app->sliderValues && next.data.range.minIndex <= 152 && next.data.range.maxIndex <= 152) {
            next.data.range.minValue = min == 4 ? app->sliderValues[next.data.range.minIndex].height : app->sliderValues[next.data.range.minIndex].weight;
            next.data.range.maxValue = min == 4 ? app->sliderValues[next.data.range.maxIndex].height : app->sliderValues[next.data.range.maxIndex].weight;
        }
    } else if (next.family == HG_POKEDEX_UI_FAMILY_AREA) {
        next.data.area.region = app->areaRegion; next.data.area.time = app->areaTimeOfDay;
        next.data.area.listPosition = app->areaListPos; next.data.area.mapCount = app->areaList.areas.nMaps;
    } else if (next.family == HG_POKEDEX_UI_FAMILY_FORMS || next.family == HG_POKEDEX_UI_FAMILY_COMPARE) {
        next.data.forms.count = app->numSeenForms; next.data.forms.selected = next.data.forms.left = app->selectedForm;
        next.data.forms.right = app->compareForm; next.data.forms.side = app->compareSide; next.data.forms.facing = app->showBackPic;
    }
    if (next.family) {
        next.busyReason = HG_POKEDEX_UI_BUSY_GRAPHICS;
        if (NativePokedexGraphics(app, next.family)) {
            next.busyReason = next.phase == HGNativePokedexUIFamilyLimit(next.family, 0) ? HG_POKEDEX_UI_BUSY_NONE : HG_POKEDEX_UI_BUSY_DRAG;
            NativePokedexUIWindows(app, &next);
            NativePokedexUIInputs(app, &next);
            next.ready = !next.busyReason && !!(next.keyEnabledMask || next.controlEnabledMask);
            if (ownWindows && !NativePokedexUIOwnWindows(&next)) {
                next.ready = next.keyEnabledMask = next.controlCount = next.controlEnabledMask = next.visualCount = 0;
                next.busyReason = HG_POKEDEX_UI_BUSY_UNSUPPORTED;
                memset(&next.continuation, 0, sizeof(next) - offsetof(HGNativePokedexUIState, continuation));
            }
        }
    }
    return next;
}

void NativePokedexUICommit(PokedexAppData *app) {
    if (!app || app != sNativePokedex || sNativePokedexBuilding) return;
    HGNativePokedexUIState next = NativePokedexCurrent(app, TRUE);
    next.frameSerial = ++sNativePokedexFrame;
    HGNativePokedexUIState previous = sNativePokedexPrevious, semantic = next;
    previous.frameSerial = semantic.frameSerial = 0;
    previous.revision = semantic.revision = 0;
    if (memcmp(&previous, &semantic, sizeof(semantic))) next.revision = NativePokedexNext(next.revision);
    sNativePokedexPrevious = next;
    sNativePokedexUI = next;
}

const HGNativePokedexUIState *HGNativePokedexUIGetState(void) { return &sNativePokedexUI; }
#endif
