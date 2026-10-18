// SPDX-License-Identifier: GPL-3.0-or-later
#include <nitro/os/arena.h>
#include <nitro/os/system.h>
#include <nitro/os/spinLock.h>
#include <nitro/os/common/entropy.h>
#include <pthread.h>
#include <stdlib.h>
#include "NativeEvents.h"

typedef struct NativeArena { u8 *base, *end, *low, *high; } NativeArena;
static NativeArena arenas[OS_ARENA_MAX];
static pthread_once_t arenaOnce = PTHREAD_ONCE_INIT;
static pthread_mutex_t arenaMutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t interruptMutex = PTHREAD_MUTEX_INITIALIZER;
static _Thread_local unsigned interruptState;
static _Thread_local OSProcMode executionMode = OS_PROCMODE_SYS;
static pthread_mutex_t lockIdMutex = PTHREAD_MUTEX_INITIALIZER;
static u64 lockIds;

void OS_GetLowEntropyData(u32 buffer[OS_LOW_ENTROPY_DATA_SIZE / sizeof(u32)]) {
    arc4random_buf(buffer, OS_LOW_ENTROPY_DATA_SIZE);
}

s32 OS_GetLockID(void) {
    pthread_mutex_lock(&lockIdMutex);
    for (u32 index = 0; index < 48; index++) if (!(lockIds & (1ULL << index))) {
        lockIds |= 1ULL << index;
        pthread_mutex_unlock(&lockIdMutex); return OS_MAINP_LOCK_ID_START + index;
    }
    pthread_mutex_unlock(&lockIdMutex); return OS_LOCK_ID_ERROR;
}
void OS_ReleaseLockID(u16 id) {
    if (id < OS_MAINP_LOCK_ID_START || id >= OS_MAINP_LOCK_ID_START + 48) abort();
    u64 bit = 1ULL << (id - OS_MAINP_LOCK_ID_START);
    pthread_mutex_lock(&lockIdMutex);
    if (!(lockIds & bit)) abort();
    lockIds &= ~bit; pthread_mutex_unlock(&lockIdMutex);
}

static void initializeArenas(void) {
    // shortcut: native arena budgets; calibrate against full-engine peak memory before release.
    static const u32 sizes[OS_ARENA_MAX] = {64 * 1024 * 1024, 1024 * 1024, 16 * 1024 * 1024,
        32768, 16384, 16384, 131072, 131072, 131072};
    for (int i = 0; i < OS_ARENA_MAX; i++) {
        u8 *memory = calloc(1, sizes[i]);
        if (!memory) abort();
        arenas[i] = (NativeArena){ memory, memory + sizes[i], memory, memory + sizes[i] };
    }
}
static NativeArena *arenaFor(OSArenaId id) {
    if ((unsigned)id >= OS_ARENA_MAX) abort();
    pthread_once(&arenaOnce, initializeArenas);
    return &arenas[id];
}
void OS_InitArena(void) { pthread_once(&arenaOnce, initializeArenas); }
void OS_InitArenaHiAndLo(OSArenaId id) {
    NativeArena *arena = arenaFor(id);
    pthread_mutex_lock(&arenaMutex); arena->low = arena->base; arena->high = arena->end; pthread_mutex_unlock(&arenaMutex);
}
void *OS_GetInitArenaLo(OSArenaId id) { return arenaFor(id)->base; }
void *OS_GetInitArenaHi(OSArenaId id) { return arenaFor(id)->end; }
void *OS_GetArenaLo(OSArenaId id) {
    NativeArena *arena = arenaFor(id); pthread_mutex_lock(&arenaMutex);
    void *result = arena->low; pthread_mutex_unlock(&arenaMutex); return result;
}
void *OS_GetArenaHi(OSArenaId id) {
    NativeArena *arena = arenaFor(id); pthread_mutex_lock(&arenaMutex);
    void *result = arena->high; pthread_mutex_unlock(&arenaMutex); return result;
}
void OS_SetArenaLo(OSArenaId id, void *address) {
    NativeArena *arena = arenaFor(id); u8 *value = address;
    pthread_mutex_lock(&arenaMutex);
    if (value && (value < arena->base || value > arena->end || (arena->high && value > arena->high))) abort();
    arena->low = value; pthread_mutex_unlock(&arenaMutex);
}
void OS_SetArenaHi(OSArenaId id, void *address) {
    NativeArena *arena = arenaFor(id); u8 *value = address;
    pthread_mutex_lock(&arenaMutex);
    if (value && (value < arena->base || value > arena->end || (arena->low && value < arena->low))) abort();
    arena->high = value; pthread_mutex_unlock(&arenaMutex);
}
void *OS_AllocFromArenaLo(OSArenaId id, u32 size, u32 align) {
    NativeArena *arena = arenaFor(id);
    if (!align || (align & (align - 1))) return NULL;
    if (align < sizeof(void *)) align = sizeof(void *);
    pthread_mutex_lock(&arenaMutex);
    uintptr_t start = ((uintptr_t)arena->low + align - 1) & ~(uintptr_t)(align - 1);
    void *result = NULL;
    if (arena->low && arena->high && start <= (uintptr_t)arena->high && size <= (uintptr_t)arena->high - start) {
        result = (void *)start; arena->low = (u8 *)((start + size + align - 1) & ~(uintptr_t)(align - 1));
        if (arena->low > arena->high) arena->low = arena->high;
    }
    pthread_mutex_unlock(&arenaMutex); return result;
}
void *OS_AllocFromArenaHi(OSArenaId id, u32 size, u32 align) {
    NativeArena *arena = arenaFor(id);
    if (!align || (align & (align - 1))) return NULL;
    if (align < sizeof(void *)) align = sizeof(void *);
    pthread_mutex_lock(&arenaMutex);
    void *result = NULL;
    if (arena->low && arena->high && size <= (uintptr_t)arena->high - (uintptr_t)arena->low) {
        uintptr_t start = ((uintptr_t)arena->high - size) & ~(uintptr_t)(align - 1);
        if (start >= (uintptr_t)arena->low) { result = (void *)start; arena->high = result; }
    }
    pthread_mutex_unlock(&arenaMutex); return result;
}

static OSIntrMode setInterruptBits(unsigned bits, unsigned value) {
    unsigned previous = interruptState, next = (previous & ~bits) | (value & bits);
    if (!previous && next) pthread_mutex_lock(&interruptMutex);
    interruptState = next;
    if (previous && !next) pthread_mutex_unlock(&interruptMutex);
    return (OSIntrMode)(previous & bits);
}
OSIntrMode OS_DisableInterrupts(void) { return setInterruptBits(OS_INTRMODE_DISABLE_IRQ, OS_INTRMODE_DISABLE_IRQ); }
OSIntrMode OS_EnableInterrupts(void) {
    OSIntrMode previous = setInterruptBits(OS_INTRMODE_DISABLE_IRQ, 0); HGNativeDispatchIrqs(); return previous;
}
OSIntrMode OS_RestoreInterrupts(OSIntrMode mode) {
    OSIntrMode previous = setInterruptBits(OS_INTRMODE_DISABLE_IRQ, mode); HGNativeDispatchIrqs(); return previous;
}
OSIntrMode OS_DisableInterrupts_IrqAndFiq(void) { return setInterruptBits(OS_INTRMODE_DISABLE_IRQ | OS_INTRMODE_DISABLE_FIQ, OS_INTRMODE_DISABLE_IRQ | OS_INTRMODE_DISABLE_FIQ); }
OSIntrMode OS_RestoreInterrupts_IrqAndFiq(OSIntrMode mode) {
    OSIntrMode previous = setInterruptBits(OS_INTRMODE_DISABLE_IRQ | OS_INTRMODE_DISABLE_FIQ, mode); HGNativeDispatchIrqs(); return previous;
}
OSIntrMode_Irq OS_GetCpsrIrq(void) { return (OSIntrMode_Irq)(interruptState & OS_INTRMODE_DISABLE_IRQ); }
OSProcMode OS_GetProcMode(void) { return executionMode; }
OSProcMode HGNativeSetExecutionMode(OSProcMode mode) {
    OSProcMode previous = executionMode; executionMode = mode; return previous;
}
