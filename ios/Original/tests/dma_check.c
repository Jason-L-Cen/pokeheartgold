// SPDX-License-Identifier: GPL-3.0-or-later
#include <nitro/mi/dma.h>
#include "../NativeEvents.h"
#include "../NativeDMA.h"
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(value) do { if (!(value)) { fprintf(stderr, "DMA check line %d: %s\n", __LINE__, #value); abort(); } } while (0)
typedef struct Completion { int count; u32 channel; } Completion;
static void completed(void *data) {
    Completion *state = data;
    CHECK(OS_GetProcMode() == OS_PROCMODE_IRQ);
    MI_WaitDma(state->channel); state->count++;
}
static void zeroLength(void *data) { CHECK(OS_GetProcMode() == OS_PROCMODE_SYS); (*(int *)data)++; }
static void disposed(void *data) { atomic_fetch_add((atomic_uint *)data, 1); }
typedef struct MappedCompletion { Completion irq; atomic_uint *disposals; unsigned expected; } MappedCompletion;
static void mappedCompleted(void *data) {
    MappedCompletion *state = data;
    CHECK(atomic_load(state->disposals) == state->expected);
    completed(&state->irq);
}

static void checkMappedDMA(void) {
    u32 source[9], destination[9];
    for (u32 i = 0; i < 9; i++) source[i] = 0x12340000u + i;
    memset(destination, 0xA5, sizeof(destination));
    HGNativeDMASpan spans[] = {
        {0x1000, source, 16}, {0x1010, source + 4, 16},
        {0x2000, destination, 8}, {0x2008, destination + 2, 24}
    };
    atomic_uint disposals = 0;
    unsigned expectedDisposals = 0;
    HGNativeDMARequest request = {.channel=2,.control=MI_CNT_COPY32(32),.source=0x1000,
        .destination=0x2000,.wait=TRUE,.spans=spans,.spanCount=4,.dispose=disposed,.context=&disposals};
    CHECK(HGNativeValidateDMA(&request)); HGNativeSubmitDMA(&request);
    CHECK(atomic_load(&disposals) == ++expectedDisposals && !memcmp(source, destination, 32));
    CHECK(destination[8] == 0xA5A5A5A5);
    request.source = 0x101C; request.destination = 0x201C;
    request.control |= MI_DMA_SRC_DEC | MI_DMA_DEST_DEC;
    memset(destination, 0xA5, sizeof(destination)); HGNativeSubmitDMA(&request);
    CHECK(atomic_load(&disposals) == ++expectedDisposals && !memcmp(source, destination, 32));
    CHECK(destination[8] == 0xA5A5A5A5);
    request.source = 0x100C; request.destination = 0x2000;
    request.control = MI_CNT_COPY32(32) | MI_DMA_SRC_FIX;
    HGNativeSubmitDMA(&request); CHECK(atomic_load(&disposals) == ++expectedDisposals);
    for (u32 i = 0; i < 8; i++) CHECK(destination[i] == source[3]);
    request.source = 0x1000; request.control = MI_CNT_COPY32(32) | MI_DMA_DEST_FIX;
    HGNativeSubmitDMA(&request); CHECK(atomic_load(&disposals) == ++expectedDisposals);
    CHECK(destination[0] == source[7] && destination[1] == source[3]);
    request.fill = TRUE; request.value = 0xABCD1357; request.control = MI_CNT_CLEAR32(32);
    HGNativeSubmitDMA(&request); CHECK(atomic_load(&disposals) == ++expectedDisposals);
    for (u32 i = 0; i < 8; i++) CHECK(destination[i] == 0xABCD1357);
    request.control = MI_DMA_ENABLE | MI_DMA_SRC_FIX | 16;
    HGNativeSubmitDMA(&request); CHECK(atomic_load(&disposals) == ++expectedDisposals);
    for (u32 i = 0; i < 16; i++) CHECK(((u16 *)destination)[i] == 0x1357);
    CHECK(destination[8] == 0xA5A5A5A5);

    u32 overlap[8] = {1,2,3,4,5,6,7,8};
    HGNativeDMASpan aliases[] = {{0x3000, overlap, 20}, {0x4000, overlap + 1, 20}};
    request = (HGNativeDMARequest){.channel=1,.control=MI_CNT_COPY32(20),.source=0x3000,
        .destination=0x4000,.wait=TRUE,.spans=aliases,.spanCount=2,.dispose=disposed,.context=&disposals};
    HGNativeSubmitDMA(&request); CHECK(atomic_load(&disposals) == ++expectedDisposals);
    for (u32 i = 0; i < 6; i++) CHECK(overlap[i] == 1);
    CHECK(overlap[6] == 7 && overlap[7] == 8);
    for (u32 i = 0; i < 8; i++) overlap[i] = i + 1;
    aliases[1].address = 0x3004; request.destination = 0x3004;
    CHECK(HGNativeValidateDMA(&request)); HGNativeSubmitDMA(&request);
    CHECK(atomic_load(&disposals) == ++expectedDisposals);
    for (u32 i = 0; i < 6; i++) CHECK(overlap[i] == 1);
    CHECK(overlap[6] == 7 && overlap[7] == 8);
    aliases[1].address = 0x4000; request.destination = 0x4000;

    HGNativeDMARequest valid = request;
    CHECK(!HGNativeValidateDMA(NULL));
    request.channel = 4; CHECK(!HGNativeValidateDMA(&request)); request = valid;
    request.spanCount = 0; CHECK(!HGNativeValidateDMA(&request)); request = valid;
    request.spanCount = HG_NATIVE_DMA_MAX_SPANS + 1; CHECK(!HGNativeValidateDMA(&request)); request = valid;
    request.control = 0; CHECK(!HGNativeValidateDMA(&request)); request = valid;
    request.control |= MI_DMA_TIMING_H_BLANK; CHECK(!HGNativeValidateDMA(&request)); request = valid;
    request.control |= MI_DMA_CONTINUOUS_ON; CHECK(!HGNativeValidateDMA(&request)); request = valid;
    request.control |= 3u << 23; CHECK(!HGNativeValidateDMA(&request)); request = valid;
    request.fill = TRUE; CHECK(!HGNativeValidateDMA(&request)); request = valid;
    request.source++; CHECK(!HGNativeValidateDMA(&request)); request = valid;
    aliases[0].size = 16; CHECK(!HGNativeValidateDMA(&request)); aliases[0].size = 20;
    aliases[0].size = 18; CHECK(!HGNativeValidateDMA(&request)); aliases[0].size = 20;
    aliases[0].data = (u8 *)overlap + 1; CHECK(!HGNativeValidateDMA(&request)); aliases[0].data = overlap;
    aliases[0].data = NULL; CHECK(!HGNativeValidateDMA(&request)); aliases[0].data = overlap;
    aliases[0].address = UINTPTR_MAX - 3; CHECK(!HGNativeValidateDMA(&request)); aliases[0].address = 0x3000;
    aliases[0].data = (void *)(UINTPTR_MAX - 3); CHECK(!HGNativeValidateDMA(&request)); aliases[0].data = overlap;
    aliases[1].address = 0x3000; CHECK(!HGNativeValidateDMA(&request)); aliases[1].address = 0x4000;
    CHECK(atomic_load(&disposals) == expectedDisposals && overlap[6] == 7 && overlap[7] == 8);

    request = (HGNativeDMARequest){.channel=3,.control=MI_CNT_COPY32(32),.source=0x1000,
        .destination=0x2000,.spans=spans,.spanCount=4,.dispose=disposed,.context=&disposals};
    HGNativeSubmitDMA(&request); MI_WaitDma(3);
    CHECK(atomic_load(&disposals) == ++expectedDisposals && !memcmp(source, destination, 32));
    HGNativeSubmitDMA(&request); MI_StopDma(3);
    CHECK(atomic_load(&disposals) == ++expectedDisposals);

    OS_InitIrqTable(); OS_DisableIrq();
    MappedCompletion completion = {{0,3}, &disposals, expectedDisposals + 1};
    OSi_EnterDmaCallback(3, mappedCompleted, &completion);
    request.control |= MI_DMA_IF_ENABLE;
    HGNativeSubmitDMA(&request); HGNativeDrainDMA();
    CHECK(atomic_load(&disposals) == ++expectedDisposals && completion.irq.count == 0);
    CHECK(OS_GetRequestIrqMask() & OS_IE_DMA3);
    OS_EnableIrq(); CHECK(completion.irq.count == 1);
    CHECK(!memcmp(source, destination, 32) && destination[8] == 0xA5A5A5A5);
    OS_DisableIrq(); OS_InitIrqTable();
}

void CheckNativeDMA(void) {
    u32 source[32], destination[32];
    for (u32 i = 0; i < 32; i++) source[i] = i + 10;
    memset(destination, 0, sizeof(destination));
    MI_DmaCopy32(0, source, destination, sizeof(source)); CHECK(memcmp(source, destination, sizeof(source)) == 0);
    MI_DmaFill32(1, destination, 0x12345678, sizeof(destination));
    for (u32 i = 0; i < 32; i++) CHECK(destination[i] == 0x12345678);
    MI_DmaCopy16(2, source, destination, 64); CHECK(memcmp(source, destination, 64) == 0 && destination[16] == 0x12345678);
    MIi_DmaSetParams_wait(0, (uaddr)&source[7], (uaddr)&destination[7], MI_DMA_ENABLE | MI_DMA_32BIT_BUS | MI_DMA_SRC_DEC | MI_DMA_DEST_DEC | 8);
    CHECK(memcmp(source, destination, 32) == 0);
    MIi_DmaSetParams_wait(0, (uaddr)&source[3], (uaddr)destination, MI_DMA_ENABLE | MI_DMA_32BIT_BUS | MI_DMA_SRC_FIX | 8);
    for (unsigned i = 0; i < 8; i++) CHECK(destination[i] == source[3]);
    MIi_DmaSetParams_wait(0, (uaddr)source, (uaddr)destination, MI_DMA_ENABLE | MI_DMA_32BIT_BUS | MI_DMA_DEST_FIX | 8);
    CHECK(destination[0] == source[7]);
    u32 overlap[8] = {1,2,3,4,5,6,7,8};
    MI_DmaCopy32(1, overlap, overlap + 1, 20);
    for (unsigned i = 0; i < 6; i++) CHECK(overlap[i] == 1);
    MIi_DmaSetParams_wait_src32(2, 99, (uaddr)destination, MI_CNT_CLEAR32(32));
    for (unsigned i = 0; i < 8; i++) CHECK(destination[i] == 99);
    OS_InitIrqTable(); OS_EnableIrq();
    Completion completion = {0, 3};
    MI_DmaCopy32Async(3, source, destination, sizeof(source), completed, &completion);
    MI_WaitDma(3);
    CHECK(completion.count == 1 && memcmp(source, destination, sizeof(source)) == 0);
    OS_DisableIrq();
    MI_DmaFill32Async(3, destination, 42, sizeof(destination), completed, &completion); MI_WaitDma(3);
    CHECK(completion.count == 1 && (OS_GetRequestIrqMask() & OS_IE_DMA3));
    OS_EnableIrq(); CHECK(completion.count == 2);
    for (unsigned i = 0; i < 32; i++) CHECK(destination[i] == 42);
    int zeroCalls = 0; MI_DmaCopy32Async(3, NULL, NULL, 0, zeroLength, &zeroCalls); CHECK(zeroCalls == 1);
    MI_StopDma(3); OS_DisableIrq(); OS_InitIrqTable();
    checkMappedDMA();
    printf("PASS: native DMA copy/fill, widths, fixed/reverse/overlap transfers, async IRQ completion, callback reentry and wait.\n");
}
