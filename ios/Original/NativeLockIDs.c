// SPDX-License-Identifier: GPL-3.0-or-later
#include <nitro/types.h>
#include <nitro/os/spinLock.h>
#include <pthread.h>
#include <stdlib.h>

// Device-wide, shared by the direct core and generated-code imports.
static pthread_mutex_t lockIdMutex = PTHREAD_MUTEX_INITIALIZER;
static u64 lockIds;

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
