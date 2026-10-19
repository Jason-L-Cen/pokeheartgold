// SPDX-License-Identifier: GPL-3.0-or-later
#include <nitro/mi/dma.h>
#include <nitro/hw/native_graphics.h>
#include "NativeEvents.h"
#include "NativeDMA.h"
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
    const HGNativeDMASpan *spans;
    u32 spanCount;
    void (*dispose)(void *);
    void *context;
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
static void *span(const Transfer *job, uintptr_t address, u32 *size) {
    if (!job->spanCount) return HGNativeMemorySpan(address, size);
    for (u32 i = 0; i < job->spanCount; i++) {
        const HGNativeDMASpan *entry = &job->spans[i];
        if (address >= entry->address && address - entry->address < entry->size) {
            u32 offset = (u32)(address - entry->address), remaining = entry->size - offset;
            if (*size > remaining) *size = remaining;
            return (u8 *)entry->data + offset;
        }
    }
    return NULL;
}
static BOOL validAddress(const Transfer *job, uintptr_t address, u32 width, u32 size, u32 mode) {
    if (address % width) return FALSE;
    if (mode == 2) size = width;
    if (mode == 1) { if (address < size - width) return FALSE; address -= size - width; }
    if (address > UINTPTR_MAX - size) return FALSE;
    while (size) {
        u32 count = size;
        if (!span(job, address, &count) || !count || count % width) return FALSE;
        address += count; size -= count;
    }
    return TRUE;
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
            const void *from = span(job, source, &count);
            void *to = span(job, destination, &count); GF_ASSERT(from && to);
            // Physical aliases may overlap even when their DS addresses differ.
            if ((uintptr_t)from + count > (uintptr_t)to && (uintptr_t)to + count > (uintptr_t)from) count = job->width;
            if (to != from) memcpy(to, from, count);
            source += count; destination += count; position += count;
        } else {
            u32 value = job->value;
            u32 width = job->width;
            if (!job->fill) {
                const void *from = span(job, source, &width); GF_ASSERT(from && width == job->width);
                memcpy(&value, from, width);
            }
            void *to = span(job, destination, &width); GF_ASSERT(to && width == job->width);
            memcpy(to, &value, width);
            position += job->width;
            if (job->sourceMode == 0) source += job->width;
            else if (job->sourceMode == 1) source -= job->width;
            if (job->destinationMode == 0 || job->destinationMode == 3) destination += job->width;
            else if (job->destinationMode == 1) destination -= job->width;
        }
    }
    if (job->dispose) job->dispose(job->context);
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
    if (!job.fill) GF_ASSERT(validAddress(&job, job.source, job.width, job.size, job.sourceMode));
    GF_ASSERT(validAddress(&job, job.destination, job.width, job.size, job.destinationMode));
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
void HGNativeDrainDMA(void) {
    for (u32 number = 0; number < 4; number++) {
        NativeChannel *state = channel(number);
        pthread_mutex_lock(&state->configure);
        dispatch_sync_f(state->queue, NULL, fence);
        pthread_mutex_unlock(&state->configure);
    }
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
static Transfer parameterJob(u32 number, uaddr src, uaddr dst, u32 control, BOOL fill, u32 value) {
    u32 width = control & MI_DMA_32BIT_BUS ? 4 : 2;
    u32 count = control & 0x1FFFFF;
    if (!count) count = 0x200000;
    u32 sourceMode = (control >> 23) & 3, destinationMode = (control >> 21) & 3;
    return (Transfer){.number=number,.source=src,.destination=dst,.size=count*width,.width=width,
        .sourceMode=sourceMode,.destinationMode=destinationMode,.fill=fill,.value=value,
        .interrupt=(control & MI_DMA_IF_ENABLE) != 0};
}
static Transfer mappedJob(const HGNativeDMARequest *request) {
    Transfer job = parameterJob(request->channel, request->source, request->destination,
                                request->control, request->fill, request->value);
    job.spans = request->spans; job.spanCount = request->spanCount;
    job.dispose = request->dispose; job.context = request->context;
    return job;
}
BOOL HGNativeValidateDMA(const HGNativeDMARequest *request) {
    if (!request || request->channel >= 4 || !(request->control & MI_DMA_ENABLE) ||
        (request->control & (MI_DMA_TIMING_MASK | MI_DMA_CONTINUOUS_ON)) ||
        !request->spans || !request->spanCount || request->spanCount > HG_NATIVE_DMA_MAX_SPANS) return FALSE;
    Transfer job = mappedJob(request);
    if (job.sourceMode == 3 || (job.fill && job.sourceMode != 2)) return FALSE;
    for (u32 i = 0; i < job.spanCount; i++) {
        const HGNativeDMASpan *entry = &job.spans[i];
        if (!entry->data || !entry->size || entry->address % job.width || entry->size % job.width ||
            (uintptr_t)entry->data % job.width || entry->address > UINTPTR_MAX - entry->size ||
            (uintptr_t)entry->data > UINTPTR_MAX - entry->size) return FALSE;
        for (u32 j = 0; j < i; j++) {
            const HGNativeDMASpan *other = &job.spans[j];
            uintptr_t first = entry->address > other->address ? entry->address : other->address;
            if (first < entry->address + entry->size && first < other->address + other->size &&
                (uintptr_t)entry->data + (first-entry->address) !=
                (uintptr_t)other->data + (first-other->address)) return FALSE;
        }
    }
    return (job.fill || validAddress(&job, job.source, job.width, job.size, job.sourceMode)) &&
           validAddress(&job, job.destination, job.width, job.size, job.destinationMode);
}
void HGNativeSubmitDMA(const HGNativeDMARequest *request) {
    GF_ASSERT(HGNativeValidateDMA(request));
    start(mappedJob(request), request->wait, NULL, NULL);
}
static void parameters(u32 number, uaddr src, uaddr dst, u32 control, BOOL wait, BOOL fill, u32 value) {
    if (!(control & MI_DMA_ENABLE)) { MI_StopDma(number); return; }
    // shortcut: timing/repeat modes require the scanline/graphics driver; reject until that driver is wired.
    GF_ASSERT(!(control & (MI_DMA_TIMING_MASK | MI_DMA_CONTINUOUS_ON)));
    Transfer job = parameterJob(number, src, dst, control, fill, value);
    GF_ASSERT(job.sourceMode != 3);
    start(job, wait, NULL, NULL);
}
void MIi_DmaSetParams(u32 number, uaddr src, uaddr dst, u32 control) { parameters(number, src, dst, control, FALSE, FALSE, 0); }
void MIi_DmaSetParams_noInt(u32 number, uaddr src, uaddr dst, u32 control) { parameters(number, src, dst, control, FALSE, FALSE, 0); }
void MIi_DmaSetParams_wait(u32 number, uaddr src, uaddr dst, u32 control) { parameters(number, src, dst, control, TRUE, FALSE, 0); }
void MIi_DmaSetParams_wait_noInt(u32 number, uaddr src, uaddr dst, u32 control) { parameters(number, src, dst, control, TRUE, FALSE, 0); }
void MIi_DmaSetParams_src32(u32 number, u32 value, uaddr dst, u32 control) { parameters(number, 0, dst, control, FALSE, TRUE, value); }
void MIi_DmaSetParams_wait_src32(u32 number, u32 value, uaddr dst, u32 control) { parameters(number, 0, dst, control, TRUE, TRUE, value); }
void MIi_CheckDma0SourceAddress(u32 number, uaddr src, u32 size, u32 direction) {
    channel(number); if (size) GF_ASSERT(validAddress(&(Transfer){0}, src, 2, size, (direction >> 23) & 3));
}
void MIi_CheckAnotherAutoDMA(u32 number, u32 timing) {
    channel(number); GF_ASSERT(timing == MIi_DMA_TIMING_ANY || timing == 0);
}
