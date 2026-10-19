// SPDX-License-Identifier: GPL-3.0-or-later
// Shared by direct-C and generated-C checks; all competing payload accesses are atomic for TSan.
#include <pthread.h>
#include <sched.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>

#define CACHE_CHECK(v) do { if (!(v)) { fprintf(stderr,"Cache ordering check line %d: %s\n",__LINE__,#v); abort(); } } while (0)
enum { CACHE_ROUNDS=9*2048 };
typedef struct {
    atomic_uint round,done[2],slot[2];
    unsigned result[2];
    void (*fence)(void *,unsigned);
    void *context;
} CacheOrdering;
typedef struct { CacheOrdering *state; unsigned index; } CacheWorker;
static void *CacheOrderingWorker(void *arg) {
    CacheWorker *work=arg; CacheOrdering *state=work->state; unsigned index=work->index;
    for (unsigned round=1;round<=CACHE_ROUNDS;round++) {
        while (atomic_load_explicit(&state->round,memory_order_acquire)!=round) sched_yield();
        atomic_store_explicit(&state->slot[index],1,memory_order_relaxed);
        state->fence(state->context,(round-1)%9);
        state->result[index]=atomic_load_explicit(&state->slot[1-index],memory_order_relaxed);
        atomic_store_explicit(&state->done[index],round,memory_order_release);
    }
    return NULL;
}
static void CheckCacheOrdering(void (*fence)(void *,unsigned),void *context) {
    CacheOrdering state={.fence=fence,.context=context};
    CacheWorker work[2]={{&state,0},{&state,1}}; pthread_t threads[2];
    CACHE_CHECK(!pthread_create(&threads[0],NULL,CacheOrderingWorker,&work[0]));
    CACHE_CHECK(!pthread_create(&threads[1],NULL,CacheOrderingWorker,&work[1]));
    for (unsigned round=1;round<=CACHE_ROUNDS;round++) {
        atomic_store_explicit(&state.slot[0],0,memory_order_relaxed);
        atomic_store_explicit(&state.slot[1],0,memory_order_relaxed);
        atomic_store_explicit(&state.round,round,memory_order_release);
        while (atomic_load_explicit(&state.done[0],memory_order_acquire)!=round ||
               atomic_load_explicit(&state.done[1],memory_order_acquire)!=round) sched_yield();
        // Two sequentially consistent fences forbid both loads observing the old zero values.
        CACHE_CHECK(state.result[0] || state.result[1]);
    }
    CACHE_CHECK(!pthread_join(threads[0],NULL) && !pthread_join(threads[1],NULL));
}
#undef CACHE_CHECK
