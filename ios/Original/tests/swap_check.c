// SPDX-License-Identifier: GPL-3.0-or-later
#include <nitro.h>
#ifndef HEARTGOLD_ABI32
#include <pthread.h>
#include <sched.h>
#include <stdatomic.h>
#include <stdlib.h>
#endif

#define CHECK(value) do { if (!(value)) return __LINE__; } while (0)
static struct { u32 before; vu32 value; u32 after; } slot={0x12345678,0,0x89ABCDEF};
u32 HGSwapWordExchange(u32 value) { return MI_SwapWord(value,&slot.value); }
u32 HGSwapWordGuards(void) { return slot.before==0x12345678 && slot.after==0x89ABCDEF; }

#ifndef HEARTGOLD_ABI32
enum { SWAP_COUNT=10000 };
static atomic_int ready,start;
typedef struct { u32 base; u32 *previous; } SwapWork;
static void *SwapWorker(void *arg) {
    SwapWork *work=arg; atomic_fetch_add(&ready,1);
    while (!atomic_load(&start)) sched_yield();
    for (u32 i=0;i<SWAP_COUNT;i++) {
        work->previous[i]=HGSwapWordExchange(work->base+i+1);
        if (!(i&63)) sched_yield();
    }
    return NULL;
}
#endif

u32 CheckOriginalSwapWord(void) {
    HGSwapWordExchange(0);
    CHECK(HGSwapWordExchange(0x80000000)==0);
    CHECK(HGSwapWordExchange(0xFFFFFFFF)==0x80000000);
    CHECK(HGSwapWordExchange(0)==0xFFFFFFFF && HGSwapWordGuards());
#ifndef HEARTGOLD_ABI32
    u32 previous[2][SWAP_COUNT]; u8 seen[2*SWAP_COUNT+1]={0};
    SwapWork work[2]={{0,previous[0]},{SWAP_COUNT,previous[1]}}; pthread_t threads[2];
    atomic_store(&ready,0); atomic_store(&start,0);
    CHECK(!pthread_create(&threads[0],NULL,SwapWorker,&work[0]));
    CHECK(!pthread_create(&threads[1],NULL,SwapWorker,&work[1]));
    while (atomic_load(&ready)!=2) sched_yield(); atomic_store(&start,1);
    CHECK(!pthread_join(threads[0],NULL) && !pthread_join(threads[1],NULL));
    for (u32 thread=0;thread<2;thread++) for (u32 i=0;i<SWAP_COUNT;i++) {
        u32 value=previous[thread][i]; CHECK(value<=2*SWAP_COUNT && !seen[value]); seen[value]=1;
    }
    u32 final=HGSwapWordExchange(0); CHECK(final<=2*SWAP_COUNT && !seen[final]); seen[final]=1;
    for (u32 i=0;i<=2*SWAP_COUNT;i++) CHECK(seen[i]);
    CHECK(HGSwapWordGuards());
#endif
    return 0;
}
