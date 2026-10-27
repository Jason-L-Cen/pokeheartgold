// SPDX-License-Identifier: GPL-3.0-or-later
#include "native_trainer_card_ui.h"
#include <stddef.h>
#include <string.h>
#ifdef HG_NATIVE_TRAINER_CARD_UI_STATE_CHECK
static void CheckTrainerCardResident(void);
#endif
#define CHECK(value) do { if (!(value)) return __LINE__; } while (0)

uint32_t CheckNativeTrainerCardUI(void) {
    CHECK(sizeof(HGNativeTrainerCardUIState) == 68
        && offsetof(HGNativeTrainerCardUIState, ownerGeneration) == 4
        && offsetof(HGNativeTrainerCardUIState, revision) == 8
        && offsetof(HGNativeTrainerCardUIState, ready) == 12
        && offsetof(HGNativeTrainerCardUIState, viewActive) == 16
        && offsetof(HGNativeTrainerCardUIState, promptButtonHeight) == 64);
    HGNativeTrainerCardUIState local = { .active = 1, .ownerGeneration = 7, .revision = 9,
        .ready = 1, .viewActive = 1, .view = HG_TRAINER_CARD_VIEWER, .phase = 1 }, before = local;
    uint32_t keys = 123;
    CHECK(!HGNativeTrainerCardUIInputAllowed(NULL, 7, 9)
        && !HGNativeTrainerCardUIInputAllowed(&local, 0, 9)
        && !HGNativeTrainerCardUIInputAllowed(&local, 7, 0)
        && !HGNativeTrainerCardUIInputAllowed(&local, 6, 9)
        && !HGNativeTrainerCardUIInputAllowed(&local, 7, 8));
    const uint32_t allowed[] = {2, 16, 32}, rejected[] = {0, 1, 3, 4, 8, 48, 64, 128, 256, 2048, UINT32_MAX};
    for (size_t i = 0; i < sizeof(allowed) / sizeof(allowed[0]); ++i)
        CHECK(HGNativeTrainerCardUIResolveKey(&local, 7, 9, allowed[i], &keys) && keys == allowed[i]);
    keys = 123;
    for (size_t i = 0; i < sizeof(rejected) / sizeof(rejected[0]); ++i)
        CHECK(!HGNativeTrainerCardUIResolveKey(&local, 7, 9, rejected[i], &keys) && keys == 123);
    CHECK(!HGNativeTrainerCardUIResolveKey(&local, 7, 9, 2, NULL) && !memcmp(&local, &before, sizeof(local)));
    const struct { size_t offset; uint32_t value; } invalid[] = {
        {offsetof(HGNativeTrainerCardUIState, active), 0}, {offsetof(HGNativeTrainerCardUIState, active), 2},
        {offsetof(HGNativeTrainerCardUIState, ready), 0}, {offsetof(HGNativeTrainerCardUIState, ready), 2},
        {offsetof(HGNativeTrainerCardUIState, viewActive), 0}, {offsetof(HGNativeTrainerCardUIState, viewActive), 2},
        {offsetof(HGNativeTrainerCardUIState, view), 0}, {offsetof(HGNativeTrainerCardUIState, view), 3},
        {offsetof(HGNativeTrainerCardUIState, phase), 0}, {offsetof(HGNativeTrainerCardUIState, phase), 2},
        {offsetof(HGNativeTrainerCardUIState, phase), 3}, {offsetof(HGNativeTrainerCardUIState, phase), 5},
        {offsetof(HGNativeTrainerCardUIState, showingBack), 2}, {offsetof(HGNativeTrainerCardUIState, isConnected), 2}
    };
    for (size_t i = 0; i < sizeof(invalid) / sizeof(invalid[0]); ++i) {
        local = before;
        memcpy((unsigned char *)&local + invalid[i].offset, &invalid[i].value, sizeof(uint32_t));
        HGNativeTrainerCardUIState invalidBefore = local;
        CHECK(!HGNativeTrainerCardUIInputAllowed(&local, 7, 9) && !memcmp(&local, &invalidBefore, sizeof(local)));
    }
    local = before; local.showingBack = 1;
    CHECK(HGNativeTrainerCardUIResolveKey(&local, 7, 9, 1, &keys));
    local.isConnected = 1; CHECK(!HGNativeTrainerCardUIResolveKey(&local, 7, 9, 1, &keys));
    local = before; local.view = HG_TRAINER_CARD_SIGNATURE; local.subState = 1;
    HGNativeTrainerCardUITouchLease touch = {0};
    CHECK(!HGNativeTrainerCardUIResolveKey(&local, 7, 9, 2, &keys)
        && HGNativeTrainerCardUIResolveTouch(&touch, &local, 7, 9, HG_TRAINER_CARD_TOUCH_DOWN, 32, 72)
        && HGNativeTrainerCardUIResolveTouch(&touch, &local, 7, 9, HG_TRAINER_CARD_TOUCH_MOVE, 255, 191));
    CHECK(!HGNativeTrainerCardUIResolveTouch(&touch, &local, 7, 8, HG_TRAINER_CARD_TOUCH_UP, 0, 0) && touch.down);
    CHECK(HGNativeTrainerCardUIResolveTouch(&touch, &local, 7, 9, HG_TRAINER_CARD_TOUCH_UP, 0, 0) && !touch.down);
    CHECK(!HGNativeTrainerCardUITouchStartAllowed(&local, 7, 9, 10, 180)
        && HGNativeTrainerCardUITouchStartAllowed(&local, 7, 9, 203, 166)
        && !HGNativeTrainerCardUITouchStartAllowed(&local, 7, 9, 255, 166));
    local.subState = 3; local.promptLeft = 200; local.promptTop = 48;
    local.promptWidth = 48; local.promptButtonHeight = 32;
    const uint32_t promptKeys[] = {1, 2, 64, 128};
    for (size_t i = 0; i < sizeof(promptKeys) / sizeof(promptKeys[0]); ++i)
        CHECK(HGNativeTrainerCardUIResolveKey(&local, 7, 9, promptKeys[i], &keys) && keys == promptKeys[i]);
    CHECK(HGNativeTrainerCardUITouchStartAllowed(&local, 7, 9, 247, 111)
        && !HGNativeTrainerCardUITouchStartAllowed(&local, 7, 9, 248, 111));
    local.promptIgnoreTouch = 1; CHECK(!HGNativeTrainerCardUITouchStartAllowed(&local, 7, 9, 200, 48));
    local.promptIgnoreTouch = 0; local.promptCursor = 2; CHECK(!HGNativeTrainerCardUIInputAllowed(&local, 7, 9));
    local.promptCursor = 0; local.view = HG_TRAINER_CARD_VIEWER; local.phase = 4; local.subState = 2; local.promptLeft = 208;
    CHECK(HGNativeTrainerCardUIInputAllowed(&local, 7, 9)
        && HGNativeTrainerCardUITouchStartAllowed(&local, 7, 9, 255, 111));
    local.ownerGeneration = local.revision = UINT32_MAX;
    CHECK(HGNativeTrainerCardUIInputAllowed(&local, UINT32_MAX, UINT32_MAX));
#ifdef HG_NATIVE_TRAINER_CARD_UI_STATE_CHECK
    CheckTrainerCardResident();
#endif
    return 0;
}

#ifdef HG_NATIVE_TRAINER_CARD_UI_STATE_CHECK
#include <assert.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define TRUE 1
#define FALSE 0
#define GF_ASSERT assert
typedef uint16_t u16;
typedef uint32_t u32;
typedef int BOOL;
#include "native_trainer_card_ui.h"
#include "actual_sys_task.h"
#include "actual_sys_task.inc"

static SysTaskQueue *testVWait;
static SysTask *SysTask_CreateOnVWaitQueue(SysTaskFunc func, void *data, u32 priority) {
    return SysTaskQueue_InsertTask(testVWait, func, data, priority);
}
static void SysTask_Destroy(SysTask *task) { assert(SysTask_Unlink(task)); }
#include "actual_resident.inc"

static unsigned uploads, persistentRuns, observerRuns;
static void PersistentTask(SysTask *task, void *data) { ++persistentRuns; }
static void UploadTask(SysTask *task, void *data) { ++uploads; SysTask_Destroy(task); }
static void ObserveFence(SysTask *task, void *view) {
    assert(uploads == 2 && NativeTrainerCardUIPromptReady(view));
    ++observerRuns;
    SysTask_Destroy(task);
}

static HGNativeTrainerCardUIState ViewerFacts(void) {
    return (HGNativeTrainerCardUIState){ .ready = 1, .phase = 1 };
}
static HGNativeTrainerCardUIState SignatureFacts(void) {
    return (HGNativeTrainerCardUIState){ .ready = 1, .phase = 1, .subState = 1 };
}
static HGNativeTrainerCardUIState PromptFacts(void) {
    return (HGNativeTrainerCardUIState){ .ready = 1, .phase = 1, .subState = 3,
        .promptLeft = 200, .promptTop = 48, .promptWidth = 48, .promptButtonHeight = 32 };
}

static void CheckTrainerCardResident(void) {
    _Static_assert(sizeof(HGNativeTrainerCardUIState) == 68, "Trainer Card public packet ABI");
    void *arena = calloc(1, SysTaskQueue_GetArenaSize(32));
    assert(arena);
    testVWait = SysTaskQueue_PlacementNew(32, arena);
    int parent, viewer, signature, unrelated;
    uint32_t keys = 0x12345678;
    HGNativeTrainerCardUITouchLease touch = {0};
    HGNativeTrainerCardUIState facts = ViewerFacts();
    NativeTrainerCardUIBegin(&parent);
    NativeTrainerCardUIBeginView(&viewer, HG_TRAINER_CARD_VIEWER);
    NativeTrainerCardUIPublish(&viewer, &facts);
    HGNativeTrainerCardUIState shown = *HGNativeTrainerCardUIGetState();
    assert(shown.active && shown.viewActive && shown.ready && shown.ownerGeneration && shown.revision);
    assert(HGNativeTrainerCardUIResolveKey(&shown, shown.ownerGeneration, shown.revision, 16, &keys) && keys == 16);
    keys = 0x12345678;
    assert(!HGNativeTrainerCardUIResolveKey(&shown, shown.ownerGeneration, shown.revision, 1, &keys) && keys == 0x12345678);
    assert(!HGNativeTrainerCardUIResolveKey(&shown, shown.ownerGeneration, shown.revision, 3, &keys));
    assert(!HGNativeTrainerCardUIInputAllowed(&shown, 0, shown.revision));
    assert(!HGNativeTrainerCardUIInputAllowed(&shown, shown.ownerGeneration, shown.revision - 1));
    assert(HGNativeTrainerCardUITouchStartAllowed(&shown, shown.ownerGeneration, shown.revision, 0, 0));
    assert(!HGNativeTrainerCardUITouchStartAllowed(&shown, shown.ownerGeneration, shown.revision, 255, 0));
    assert(!HGNativeTrainerCardUITouchStartAllowed(&shown, shown.ownerGeneration, shown.revision, 32, 180));
    facts.showingBack = 1;
    NativeTrainerCardUIPublish(&viewer, &facts);
    shown = *HGNativeTrainerCardUIGetState();
    assert(HGNativeTrainerCardUIResolveKey(&shown, shown.ownerGeneration, shown.revision, 1, &keys));
    assert(HGNativeTrainerCardUITouchStartAllowed(&shown, shown.ownerGeneration, shown.revision, 111, 190));
    facts.isConnected = 1;
    NativeTrainerCardUIPublish(&viewer, &facts);
    shown = *HGNativeTrainerCardUIGetState();
    assert(!HGNativeTrainerCardUIResolveKey(&shown, shown.ownerGeneration, shown.revision, 1, &keys));
    assert(!HGNativeTrainerCardUITouchStartAllowed(&shown, shown.ownerGeneration, shown.revision, 111, 190));
    facts = ViewerFacts(); facts.phase = 3;
    NativeTrainerCardUIPublish(&viewer, &facts);
    assert(!HGNativeTrainerCardUIGetState()->ready);
    NativeTrainerCardUIEndView(&unrelated);
    assert(HGNativeTrainerCardUIGetState()->viewActive);
    NativeTrainerCardUIEndView(&viewer);
    assert(HGNativeTrainerCardUIGetState()->active && !HGNativeTrainerCardUIGetState()->viewActive);
    NativeTrainerCardUIBeginView(&signature, HG_TRAINER_CARD_SIGNATURE);
    facts = SignatureFacts();
    NativeTrainerCardUIPublish(&signature, &facts);
    shown = *HGNativeTrainerCardUIGetState();
    assert(!HGNativeTrainerCardUIResolveKey(&shown, shown.ownerGeneration, shown.revision, 2, &keys));
    assert(HGNativeTrainerCardUIResolveTouch(&touch, &shown, shown.ownerGeneration, shown.revision,
        HG_TRAINER_CARD_TOUCH_DOWN, 32, 72));
    for (unsigned i = 0; i < 1000; ++i) {
        NativeTrainerCardUIPublish(&signature, &facts);
        assert(HGNativeTrainerCardUIGetState()->revision == shown.revision);
        assert(HGNativeTrainerCardUIResolveTouch(&touch, HGNativeTrainerCardUIGetState(), shown.ownerGeneration,
            shown.revision, HG_TRAINER_CARD_TOUCH_MOVE, i % 256, i % 192));
    }
    assert(!HGNativeTrainerCardUIResolveTouch(&touch, &shown, shown.ownerGeneration, shown.revision - 1,
        HG_TRAINER_CARD_TOUCH_UP, 0, 0) && touch.down);
    assert(HGNativeTrainerCardUIResolveTouch(&touch, &shown, shown.ownerGeneration, shown.revision,
        HG_TRAINER_CARD_TOUCH_UP, 0, 0) && !touch.down);
    assert(HGNativeTrainerCardUITouchStartAllowed(&shown, shown.ownerGeneration, shown.revision, 203, 166));
    assert(!HGNativeTrainerCardUITouchStartAllowed(&shown, shown.ownerGeneration, shown.revision, 255, 166));
    assert(!HGNativeTrainerCardUITouchStartAllowed(&shown, shown.ownerGeneration, shown.revision, 10, 180));

    facts = PromptFacts();
    NativeTrainerCardUIPublish(&signature, &facts);
    assert(!HGNativeTrainerCardUIGetState()->ready);
    SysTask *persistent = SysTask_CreateOnVWaitQueue(PersistentTask, NULL, 5);
    assert(persistent);
    assert(SysTask_CreateOnVWaitQueue(UploadTask, NULL, 0x80));
    assert(SysTask_CreateOnVWaitQueue(UploadTask, NULL, 0x80));
    NativeTrainerCardUIPromptQueued(&signature);
    assert(!NativeTrainerCardUIPromptReady(&signature));
    assert(SysTask_CreateOnVWaitQueue(ObserveFence, &signature, 0x82));
    SysTaskQueue_RunTasks(testVWait);
    assert(uploads == 2 && persistentRuns == 1 && observerRuns == 1 && testVWait->activeCount == 1);
    NativeTrainerCardUIPublish(&signature, &facts);
    shown = *HGNativeTrainerCardUIGetState();
    assert(shown.ready && HGNativeTrainerCardUIIsPrompt(&shown));
    assert(HGNativeTrainerCardUIResolveKey(&shown, shown.ownerGeneration, shown.revision, 1, &keys));
    assert(HGNativeTrainerCardUIResolveKey(&shown, shown.ownerGeneration, shown.revision, 128, &keys));
    assert(!HGNativeTrainerCardUIResolveKey(&shown, shown.ownerGeneration, shown.revision, 16, &keys));
    assert(HGNativeTrainerCardUITouchStartAllowed(&shown, shown.ownerGeneration, shown.revision, 247, 111));
    assert(!HGNativeTrainerCardUITouchStartAllowed(&shown, shown.ownerGeneration, shown.revision, 248, 111));
    assert(!HGNativeTrainerCardUITouchStartAllowed(&shown, shown.ownerGeneration, shown.revision, 247, 112));
    facts.promptTouchMode = 1;
    NativeTrainerCardUIPublish(&signature, &facts);
    assert(HGNativeTrainerCardUIGetState()->revision != shown.revision);
    facts.ready = 0;
    NativeTrainerCardUIPublish(&signature, &facts);
    uint32_t confirming = HGNativeTrainerCardUIGetState()->revision;
    for (unsigned i = 0; i < 8; ++i) {
        NativeTrainerCardUIPublish(&signature, &facts);
        assert(!HGNativeTrainerCardUIGetState()->ready && HGNativeTrainerCardUIGetState()->revision == confirming);
    }

    facts = SignatureFacts();
    NativeTrainerCardUIPublish(&signature, &facts);
    shown = *HGNativeTrainerCardUIGetState();
    assert(HGNativeTrainerCardUIResolveTouch(&touch, &shown, shown.ownerGeneration, shown.revision,
        HG_TRAINER_CARD_TOUCH_DOWN, 32, 72));
    NativeTrainerCardUIPromptQueued(&signature);
    assert(testVWait->activeCount == 2);
    NativeTrainerCardUIEndView(&signature);
    assert(testVWait->activeCount == 1 && !NativeTrainerCardUIPromptReady(&signature));
    assert(!HGNativeTrainerCardUIResolveTouch(&touch, HGNativeTrainerCardUIGetState(), shown.ownerGeneration,
        shown.revision, HG_TRAINER_CARD_TOUCH_DOWN, 32, 72));
    assert(!touch.down && HGNativeTrainerCardUIGetState()->active);
    NativeTrainerCardUIBeginView(&signature, HG_TRAINER_CARD_SIGNATURE); // Reuse the same owner address.
    SysTaskQueue_RunTasks(testVWait);
    assert(!NativeTrainerCardUIPromptReady(&signature));
    assert(!HGNativeTrainerCardUIInputAllowed(HGNativeTrainerCardUIGetState(), shown.ownerGeneration, shown.revision));
    NativeTrainerCardUIPublish(&viewer, &facts);
    assert(!HGNativeTrainerCardUIGetState()->ready);
    NativeTrainerCardUIPromptQueued(&viewer);
    assert(testVWait->activeCount == 1);
    NativeTrainerCardUIPromptQueued(&signature);
    SysTaskQueue_RunTasks(testVWait);
    assert(NativeTrainerCardUIPromptReady(&signature));
    NativeTrainerCardUIEndView(&signature);
    NativeTrainerCardUIBeginView(&viewer, HG_TRAINER_CARD_VIEWER);
    facts = ViewerFacts(); facts.showingBack = 1;
    NativeTrainerCardUIPublish(&viewer, &facts);
    assert(HGNativeTrainerCardUIGetState()->active && HGNativeTrainerCardUIGetState()->ready
        && HGNativeTrainerCardUIGetState()->view == HG_TRAINER_CARD_VIEWER
        && HGNativeTrainerCardUIGetState()->ownerGeneration != shown.ownerGeneration);
    NativeTrainerCardUIEndView(&viewer);
    NativeTrainerCardUIRetire(&parent);
    assert(!HGNativeTrainerCardUIGetState()->active);
    NativeTrainerCardUIBeginView(&signature, HG_TRAINER_CARD_SIGNATURE);
    assert(HGNativeTrainerCardUIGetState()->active);
    NativeTrainerCardUIEndView(&signature);
    assert(!HGNativeTrainerCardUIGetState()->active);
    sNativeTrainerCardUIState.ownerGeneration = UINT32_MAX;
    sNativeTrainerCardUIState.revision = UINT32_MAX;
    NativeTrainerCardUIBegin(&parent);
    assert(HGNativeTrainerCardUIGetState()->ownerGeneration == 1 && HGNativeTrainerCardUIGetState()->revision == 1);
    sNativeTrainerCardUIState.ownerGeneration = UINT32_MAX;
    sNativeTrainerCardUIState.revision = UINT32_MAX;
    NativeTrainerCardUIBeginView(&viewer, HG_TRAINER_CARD_VIEWER);
    assert(HGNativeTrainerCardUIGetState()->ownerGeneration == 1 && HGNativeTrainerCardUIGetState()->revision == 1);
    facts = ViewerFacts();
    sNativeTrainerCardUIState.revision = UINT32_MAX;
    NativeTrainerCardUIPublish(&viewer, &facts);
    assert(HGNativeTrainerCardUIGetState()->revision == 1);
    NativeTrainerCardUIRetire(&parent);
    assert(!HGNativeTrainerCardUIGetState()->active);
    SysTask_Destroy(persistent);
    assert(testVWait->activeCount == 0);
    free(arena);
}

#endif

#ifdef HG_NATIVE_TRAINER_CARD_UI_CHECK_MAIN
#include <stdio.h>
int main(void) {
    uint32_t line = CheckNativeTrainerCardUI();
    if (line) { fprintf(stderr, "Trainer Card UI check line %u\n", line); return 1; }
#ifdef HG_NATIVE_TRAINER_CARD_UI_STATE_CHECK
    puts("PASS: Trainer Card68 qualification, resident lifecycle, original SysTask upload/fence order, persistent VWait, cancel/reuse, wrap and stable stroke leases.");
#else
    puts("PASS: Trainer Card68 ABI, original key/touch qualification and stale/release fences.");
#endif
    return 0;
}
#endif
