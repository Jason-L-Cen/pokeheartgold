// SPDX-License-Identifier: GPL-3.0-or-later
#include <nitro/mi/dma.h>
#include "NativeEvents.h"
#include "assert.h"
#include <dispatch/dispatch.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdlib.h>
#include <string.h>

typedef struct NativeChannel {
    dispatch_queue_t queue;
    pthread_mutex_t configure;
    atomic_bool cancel;
} NativeChannel;
typedef struct Transfer {
    NativeChannel *channel;
    uintptr_t source, destination;
    u32 size, width, sourceMode, destinationMode, value, number;
    BOOL fill, interrupt, owned, deferInterrupt, deliverInterrupt;
} Transfer;
static NativeChannel channels[4];
static pthread_once_t initializeOnce = PTHREAD_ONCE_INIT;

static void initializeChannels(void) {
    for (unsigned i = 0; i < 4; i++) {
        channels[i].queue = dispatch_queue_create("heartgold.native.dma", DISPATCH_QUEUE_SERIAL);
        if (!channels[i].queue || pthread_mutex_init(&channels[i].configure, NULL)) abort();
    }
}
static NativeChannel *channel(u32 number) {
    GF_ASSERT(number < 4); pthread_once(&initializeOnce, initializeChannels); return &channels[number];
}
static void validateAddress(uintptr_t address, u32 width) {
    // DS graphics/register addresses need the graphics backend, not a truncated host pointer.
    GF_ASSERT(address > UINT32_MAX && address % width == 0);
}
static void fence(void *unused) { (void)unused; }
static void transfer(void *context) {
    Transfer *job = context;
    uintptr_t source = job->source, destination = job->destination;
    u32 position = 0;
    while (position < job->size && !atomic_load(&job->channel->cancel)) {
        if (!job->fill && job->sourceMode == 0 && job->destinationMode == 0 &&
            (source + job->size <= destination || destination + job->size <= source)) {
            u32 count = job->size - position;
            if (count > 32768) count = 32768;
            memcpy((void *)destination, (void *)source, count);
            source += count; destination += count; position += count;
        } else {
            u32 value = job->value;
            if (!job->fill) memcpy(&value, (const void *)source, job->width);
            memcpy((void *)destination, &value, job->width);
            position += job->width;
            if (job->sourceMode == 0) source += job->width;
            else if (job->sourceMode == 1) source -= job->width;
            if (job->destinationMode == 0 || job->destinationMode == 3) destination += job->width;
            else if (job->destinationMode == 1) destination -= job->width;
        }
    }
    if (job->interrupt && !atomic_load(&job->channel->cancel)) {
        if (job->deferInterrupt) job->deliverInterrupt = TRUE;
        else HGNativePostIrq(1u << (REG_OS_IE_D0_SHIFT + job->number));
    }
    if (job->owned) free(job);
}
static void start(Transfer job, BOOL wait, MIDmaCallback callback, void *argument) {
    job.channel = channel(job.number);
    GF_ASSERT(job.width == 2 || job.width == 4);
    GF_ASSERT(job.size % job.width == 0);
    if (!job.size) { if (callback) callback(argument); return; }
    if (!job.fill) validateAddress(job.source, job.width);
    validateAddress(job.destination, job.width);
    OSIntrMode previous = OS_DisableInterrupts();
    pthread_mutex_lock(&job.channel->configure);
    dispatch_sync_f(job.channel->queue, NULL, fence);
    atomic_store(&job.channel->cancel, FALSE);
    if (callback) { OSi_EnterDmaCallback(job.number, callback, argument); job.interrupt = TRUE; }
    if (wait) { job.deferInterrupt = TRUE; dispatch_sync_f(job.channel->queue, &job, transfer); }
    else {
        Transfer *copy = malloc(sizeof(*copy)); GF_ASSERT(copy);
        *copy = job; copy->owned = TRUE; dispatch_async_f(job.channel->queue, copy, transfer);
    }
    pthread_mutex_unlock(&job.channel->configure);
    OS_RestoreInterrupts(previous);
    if (job.deliverInterrupt) HGNativePostIrq(1u << (REG_OS_IE_D0_SHIFT + job.number));
}
void MI_WaitDma(u32 number) {
    NativeChannel *state = channel(number);
    pthread_mutex_lock(&state->configure); dispatch_sync_f(state->queue, NULL, fence); pthread_mutex_unlock(&state->configure);
    HGNativeDispatchIrqs();
}
void MI_StopDma(u32 number) {
    NativeChannel *state = channel(number); atomic_store(&state->cancel, TRUE);
    pthread_mutex_lock(&state->configure); dispatch_sync_f(state->queue, NULL, fence);
    atomic_store(&state->cancel, FALSE); pthread_mutex_unlock(&state->configure);
}
void MI_DmaCopy32(u32 number, const void *src, void *dst, u32 size) {
    start((Transfer){.number=number,.source=(uintptr_t)src,.destination=(uintptr_t)dst,.size=size,.width=4}, TRUE, NULL, NULL);
}
void MI_DmaCopy16(u32 number, const void *src, void *dst, u32 size) {
    start((Transfer){.number=number,.source=(uintptr_t)src,.destination=(uintptr_t)dst,.size=size,.width=2}, TRUE, NULL, NULL);
}
void MI_DmaFill32(u32 number, void *dst, u32 value, u32 size) {
    start((Transfer){.number=number,.destination=(uintptr_t)dst,.size=size,.width=4,.fill=TRUE,.value=value}, TRUE, NULL, NULL);
}
void MI_DmaCopy32Async(u32 number, const void *src, void *dst, u32 size, MIDmaCallback callback, void *arg) {
    start((Transfer){.number=number,.source=(uintptr_t)src,.destination=(uintptr_t)dst,.size=size,.width=4}, FALSE, callback, arg);
}
void MI_DmaFill32Async(u32 number, void *dst, u32 value, u32 size, MIDmaCallback callback, void *arg) {
    start((Transfer){.number=number,.destination=(uintptr_t)dst,.size=size,.width=4,.fill=TRUE,.value=value}, FALSE, callback, arg);
}
static void parameters(u32 number, uaddr src, uaddr dst, u32 control, BOOL wait, BOOL fill, u32 value) {
    if (!(control & MI_DMA_ENABLE)) { MI_StopDma(number); return; }
    // shortcut: timing/repeat modes require the scanline/graphics driver; reject until that driver is wired.
    GF_ASSERT(!(control & (MI_DMA_TIMING_MASK | MI_DMA_CONTINUOUS_ON)));
    u32 width = control & MI_DMA_32BIT_BUS ? 4 : 2;
    u32 count = control & 0x1FFFFF;
    if (!count) count = 0x200000;
    u32 sourceMode = (control >> 23) & 3, destinationMode = (control >> 21) & 3;
    GF_ASSERT(sourceMode != 3);
    start((Transfer){.number=number,.source=src,.destination=dst,.size=count*width,.width=width,
        .sourceMode=sourceMode,.destinationMode=destinationMode,.fill=fill,.value=value,
        .interrupt=(control & MI_DMA_IF_ENABLE) != 0}, wait, NULL, NULL);
}
void MIi_DmaSetParams(u32 number, uaddr src, uaddr dst, u32 control) { parameters(number, src, dst, control, FALSE, FALSE, 0); }
void MIi_DmaSetParams_noInt(u32 number, uaddr src, uaddr dst, u32 control) { parameters(number, src, dst, control, FALSE, FALSE, 0); }
void MIi_DmaSetParams_wait(u32 number, uaddr src, uaddr dst, u32 control) { parameters(number, src, dst, control, TRUE, FALSE, 0); }
void MIi_DmaSetParams_wait_noInt(u32 number, uaddr src, uaddr dst, u32 control) { parameters(number, src, dst, control, TRUE, FALSE, 0); }
void MIi_DmaSetParams_src32(u32 number, u32 value, uaddr dst, u32 control) { parameters(number, 0, dst, control, FALSE, TRUE, value); }
void MIi_DmaSetParams_wait_src32(u32 number, u32 value, uaddr dst, u32 control) { parameters(number, 0, dst, control, TRUE, TRUE, value); }
void MIi_CheckDma0SourceAddress(u32 number, uaddr src, u32 size, u32 direction) {
    (void)direction; channel(number); if (size) validateAddress(src, 2);
}
void MIi_CheckAnotherAutoDMA(u32 number, u32 timing) {
    channel(number); GF_ASSERT(timing == MIi_DMA_TIMING_ANY || timing == 0);
}
