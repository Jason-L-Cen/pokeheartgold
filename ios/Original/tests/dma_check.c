// SPDX-License-Identifier: GPL-3.0-or-later
#include <nitro/mi/dma.h>
#include "../NativeEvents.h"
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
    printf("PASS: native DMA copy/fill, widths, fixed/reverse/overlap transfers, async IRQ completion, callback reentry and wait.\n");
}
