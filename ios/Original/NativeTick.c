// SPDX-License-Identifier: GPL-3.0-or-later
#include <nitro/os/tick.h>
#include <nitro/hw/consts_shared.h>
#include <nitro/os/native_clock.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include "NativeEvents.h"

vu64 OSi_TickCounter;
static pthread_mutex_t tickMutex = PTHREAD_MUTEX_INITIALIZER;
static BOOL initialized;
static u64 epoch, baseTick;

static u64 nanoseconds(void) {
    struct timespec value;
    if (clock_gettime(CLOCK_MONOTONIC, &value)) abort();
    return (u64)value.tv_sec * 1000000000 + value.tv_nsec;
}
void OS_InitTick(void) {
    pthread_mutex_lock(&tickMutex);
    if (!initialized) { epoch = nanoseconds(); baseTick = 0; OSi_TickCounter = 0; initialized = TRUE; }
    pthread_mutex_unlock(&tickMutex);
}
u64 HGNativeMonotonicTick(void) {
    return (u64)(((unsigned __int128)nanoseconds()*HW_SYSTEM_CLOCK)/64000000000ULL);
}
BOOL OS_IsTickAvailable(void) {
    pthread_mutex_lock(&tickMutex); BOOL value = initialized; pthread_mutex_unlock(&tickMutex); return value;
}
OSTick OS_GetTick(void) {
    HGNativeDispatchIrqs();
    pthread_mutex_lock(&tickMutex);
    if (!initialized) abort();
    u64 elapsed = nanoseconds() - epoch;
    u64 result = baseTick + (u64)(((unsigned __int128)elapsed * HW_SYSTEM_CLOCK) / 64000000000ULL);
    OSi_TickCounter = result >> 16;
    pthread_mutex_unlock(&tickMutex); return result;
}
u16 OS_GetTickLo(void) { return (u16)OS_GetTick(); }
void OS_SetTick(OSTick tick) {
    OS_InitTick(); pthread_mutex_lock(&tickMutex);
    epoch = nanoseconds(); baseTick = tick; OSi_TickCounter = tick >> 16;
    pthread_mutex_unlock(&tickMutex);
}
