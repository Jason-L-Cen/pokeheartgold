// SPDX-License-Identifier: GPL-3.0-or-later
#include <nitro/os/arena.h>
#include <nitro/os/system.h>
#include <nitro/hw/mmap.h>
#ifndef HEARTGOLD_ABI32
#include <nitro/os/common/entropy.h>
#include <pthread.h>
#endif
#include <stdlib.h>

typedef struct NativeArena { u8 *base, *end, *low, *high; } NativeArena;
static NativeArena arenas[OS_ARENA_MAX];
#ifdef HEARTGOLD_ABI32
static BOOL arenasInitialized;
_Alignas(4096) u32 SDK_AUTOLOAD_DTCM_START[HW_DTCM_SIZE/sizeof(u32)];
void *HGNativeSystemWorkAddress(void) {
    _Alignas(32) static u8 systemWork[0x400];
    return systemWork;
}
static OSIntrMode lockArenas(void) { return OS_DisableInterrupts(); }
static void unlockArenas(OSIntrMode mode) { OS_RestoreInterrupts(mode); }
#else
static pthread_once_t arenaOnce = PTHREAD_ONCE_INIT;
static pthread_mutex_t arenaMutex = PTHREAD_MUTEX_INITIALIZER;
static OSIntrMode lockArenas(void) { pthread_mutex_lock(&arenaMutex); return OS_INTRMODE_ENABLE; }
static void unlockArenas(OSIntrMode mode) { (void)mode; pthread_mutex_unlock(&arenaMutex); }

void OS_GetLowEntropyData(u32 buffer[OS_LOW_ENTROPY_DATA_SIZE / sizeof(u32)]) {
    arc4random_buf(buffer, OS_LOW_ENTROPY_DATA_SIZE);
}
#endif

static void initializeArenas(void) {
#ifdef HEARTGOLD_ABI32
    // shortcut: data-pool capacities exclude native code/stacks; calibrate reservations against full-engine peaks.
    _Alignas(32) static u8 mainMemory[HW_MAIN_MEM_MAIN_END-HW_MAIN_MEM];
    _Alignas(32) static u8 itcm[HW_ITCM_SIZE];
    _Alignas(32) static u8 shared[HW_SHARED_ARENA_HI_DEFAULT-HW_SHARED_ARENA_LO_DEFAULT], emptyWram[32];
    u8 *memory[OS_ARENA_MAX] = {[OS_ARENA_MAIN]=mainMemory,[OS_ARENA_ITCM]=itcm,
        [OS_ARENA_DTCM]=(u8 *)SDK_AUTOLOAD_DTCM_START,[OS_ARENA_SHARED]=shared,[OS_ARENA_WRAM_MAIN]=emptyWram};
    const u32 sizes[OS_ARENA_MAX] = {[OS_ARENA_MAIN]=sizeof(mainMemory),[OS_ARENA_ITCM]=sizeof(itcm),
        [OS_ARENA_DTCM]=sizeof(SDK_AUTOLOAD_DTCM_START),[OS_ARENA_SHARED]=sizeof(shared)};
    for (int i = 0; i < OS_ARENA_MAX; i++) {
        u8 *end = memory[i] ? memory[i]+sizes[i] : NULL;
        arenas[i] = (NativeArena){memory[i],end,memory[i],end};
    }
#else
    // shortcut: native arena budgets; calibrate against full-engine peak memory before release.
    static const u32 sizes[OS_ARENA_MAX] = {64 * 1024 * 1024, 1024 * 1024, 16 * 1024 * 1024,
        32768, 16384, 16384, 131072, 131072, 131072};
    for (int i = 0; i < OS_ARENA_MAX; i++) {
        u8 *memory = calloc(1, sizes[i]);
        if (!memory) abort();
        arenas[i] = (NativeArena){ memory, memory + sizes[i], memory, memory + sizes[i] };
    }
#endif
}
static void ensureArenas(void) {
#ifdef HEARTGOLD_ABI32
    OSIntrMode mode = lockArenas();
    if (!arenasInitialized) { initializeArenas(); arenasInitialized = TRUE; }
    unlockArenas(mode);
#else
    pthread_once(&arenaOnce, initializeArenas);
#endif
}
static NativeArena *arenaFor(OSArenaId id) {
    if ((unsigned)id >= OS_ARENA_MAX) abort();
    ensureArenas();
    return &arenas[id];
}
void OS_InitArena(void) { ensureArenas(); }
void OS_InitArenaEx(void) { OS_InitArenaHiAndLo(OS_ARENA_MAINEX); }
void OS_InitArenaHiAndLo(OSArenaId id) {
    NativeArena *arena = arenaFor(id);
    OSIntrMode mode = lockArenas(); arena->low = arena->base; arena->high = arena->end; unlockArenas(mode);
}
void *OS_GetInitArenaLo(OSArenaId id) { return arenaFor(id)->base; }
void *OS_GetInitArenaHi(OSArenaId id) { return arenaFor(id)->end; }
void *OS_GetArenaLo(OSArenaId id) {
    NativeArena *arena = arenaFor(id); OSIntrMode mode = lockArenas();
    void *result = arena->low; unlockArenas(mode); return result;
}
void *OS_GetArenaHi(OSArenaId id) {
    NativeArena *arena = arenaFor(id); OSIntrMode mode = lockArenas();
    void *result = arena->high; unlockArenas(mode); return result;
}
void OS_SetArenaLo(OSArenaId id, void *address) {
    NativeArena *arena = arenaFor(id); u8 *value = address;
    OSIntrMode mode = lockArenas();
    if (value && ((uintptr_t)value < (uintptr_t)arena->base || (uintptr_t)value > (uintptr_t)arena->end ||
                  (arena->high && (uintptr_t)value > (uintptr_t)arena->high))) abort();
    arena->low = value; unlockArenas(mode);
}
void OS_SetArenaHi(OSArenaId id, void *address) {
    NativeArena *arena = arenaFor(id); u8 *value = address;
    OSIntrMode mode = lockArenas();
    if (value && ((uintptr_t)value < (uintptr_t)arena->base || (uintptr_t)value > (uintptr_t)arena->end ||
                  (arena->low && (uintptr_t)value < (uintptr_t)arena->low))) abort();
    arena->high = value; unlockArenas(mode);
}
void *OS_AllocFromArenaLo(OSArenaId id, u32 size, u32 align) {
    NativeArena *arena = arenaFor(id);
    if (!align || (align & (align - 1))) return NULL;
#ifndef HEARTGOLD_ABI32
    if (align < sizeof(void *)) align = sizeof(void *);
#endif
    OSIntrMode mode = lockArenas();
    uintptr_t low = (uintptr_t)arena->low, high = (uintptr_t)arena->high;
    void *result = NULL;
    if (low && high && low <= UINTPTR_MAX-(align-1)) {
        uintptr_t start = (low+align-1)&~(uintptr_t)(align-1);
        if (start <= high && size <= high-start && start+size <= UINTPTR_MAX-(align-1)) {
            uintptr_t end = (start+size+align-1)&~(uintptr_t)(align-1);
            if (end <= high) { result = (void *)start; arena->low = (u8 *)end; }
        }
    }
    unlockArenas(mode); return result;
}
void *OS_AllocFromArenaHi(OSArenaId id, u32 size, u32 align) {
    NativeArena *arena = arenaFor(id);
    if (!align || (align & (align - 1))) return NULL;
#ifndef HEARTGOLD_ABI32
    if (align < sizeof(void *)) align = sizeof(void *);
#endif
    OSIntrMode mode = lockArenas();
    void *result = NULL;
    uintptr_t end = (uintptr_t)arena->high&~(uintptr_t)(align-1);
    if (arena->low && arena->high && end >= (uintptr_t)arena->low && size <= end-(uintptr_t)arena->low) {
        uintptr_t start = (end-size)&~(uintptr_t)(align-1);
        if (start >= (uintptr_t)arena->low) { result = (void *)start; arena->high = result; }
    }
    unlockArenas(mode); return result;
}
