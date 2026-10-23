// SPDX-License-Identifier: GPL-3.0-or-later
#include "NativeAudioCheck.h"

#ifdef HEARTGOLD_ORIGINAL_APP_CHECK
#include <pthread.h>
#include <stdatomic.h>
#include <string.h>
#include <time.h>

_Static_assert(sizeof(HGNativeAudioCheckRecord) == 56, "audio trace record size");
_Static_assert(sizeof(HGNativeAudioCheckSnapshot) == 2776, "audio trace snapshot size");

// shortcut: CHECK trace observes one stopped-then-reset owner/epoch; use per-env collectors if multiple live games are introduced.
static HGNativeAudioCheckSnapshot trace;
static HGNativeAudioCheckRecord service;
static HGNativeAudioCheckCost serviceStartCosts[HG_AUDIO_CHECK_KIND_COUNT];
static pthread_t owner;
static int bound;
static _Atomic uint64_t clockFailures, otherThreadCalls;

static int isOwner(void) {
    if (!bound) return 0;
    if (pthread_equal(owner, pthread_self())) return 1;
    atomic_fetch_add_explicit(&otherThreadCalls, 1, memory_order_relaxed);
    return 0;
}

static int invalid(void) { ++trace.invalidCalls; return 0; }

static void record(HGNativeAudioCheckRecord value) {
    uint64_t elapsed = value.endNs - value.startNs;
    HGNativeAudioCheckCost *cost = &trace.costs[value.kind];
    ++cost->count; cost->totalNs += elapsed;
    if (elapsed > cost->maxNs) cost->maxNs = elapsed;
    if (elapsed < HG_AUDIO_CHECK_LONG_NS) return;
    trace.records[trace.nextRecord] = value;
    trace.nextRecord = (trace.nextRecord + 1) % HG_AUDIO_CHECK_RECORDS;
    if (trace.recordCount == HG_AUDIO_CHECK_RECORDS) ++trace.overwriteCount;
    else ++trace.recordCount;
    ++trace.sequence;
}

int HGNativeAudioCheckReset(uint64_t bootEpoch, uint64_t startNs) {
    if (!bootEpoch || !startNs) {
        if (isOwner()) invalid();
        return 0;
    }
    memset(&trace, 0, sizeof(trace));
    memset(&service, 0, sizeof(service));
    memset(serviceStartCosts, 0, sizeof(serviceStartCosts));
    atomic_store_explicit(&clockFailures, 0, memory_order_relaxed);
    atomic_store_explicit(&otherThreadCalls, 0, memory_order_relaxed);
    owner = pthread_self(); bound = 1;
    trace.version = HG_AUDIO_CHECK_VERSION; trace.bootEpoch = bootEpoch;
    trace.bootStartNs = startNs; trace.thresholdNs = HG_AUDIO_CHECK_LONG_NS;
    return 1;
}

uint64_t HGNativeAudioCheckNow(void) {
    struct timespec stamp;
    if (clock_gettime(CLOCK_MONOTONIC, &stamp) || stamp.tv_sec < 0 || stamp.tv_nsec < 0 || stamp.tv_nsec >= 1000000000
        || (uint64_t)stamp.tv_sec > (UINT64_MAX - (uint64_t)stamp.tv_nsec) / UINT64_C(1000000000)) {
        atomic_fetch_add_explicit(&clockFailures, 1, memory_order_relaxed);
        return 0;
    }
    return (uint64_t)stamp.tv_sec * UINT64_C(1000000000) + (uint64_t)stamp.tv_nsec;
}

int HGNativeAudioCheckSpan(uint64_t kind, uint64_t startNs, uint64_t endNs, uint64_t a, uint64_t b, uint64_t c) {
    if (!isOwner()) return 0;
    if (kind < HG_AUDIO_CHECK_SYNTH || kind > HG_AUDIO_CHECK_APP_COPY || !startNs || endNs < startNs) return invalid();
    record((HGNativeAudioCheckRecord){ kind, startNs, endNs, trace.serviceSeq, a, b, c });
    return 1;
}

uint64_t HGNativeAudioCheckServiceBegin(uint64_t source, uint64_t flags, uint64_t startNs) {
    if (!isOwner()) return 0;
    if (source < HG_AUDIO_CHECK_SOURCE_HOST || source > HG_AUDIO_CHECK_SOURCE_MODE_CHANGE || (flags & ~UINT64_C(3))
        || !startNs || service.serviceSeq || startNs < trace.lastServiceEndNs || startNs < trace.lastServiceStartNs) return invalid();
    uint64_t seq = ++trace.serviceSeq;
    if (trace.lastServiceStartNs && trace.lastFlags == 3 && flags == 3) {
        HGNativeAudioCheckGap gap = { 0 };
        gap.record = (HGNativeAudioCheckRecord){ HG_AUDIO_CHECK_SERVICE_GAP, trace.lastServiceStartNs, startNs, seq, source, flags, trace.lastSource };
        gap.previousServiceSeq = seq - 1; gap.previousFlags = trace.lastFlags;
        gap.previousDueFrames = trace.lastDueFrames; gap.previousOutputFrames = trace.lastOutputFrames;
        for (unsigned i = 0; i < HG_AUDIO_CHECK_KIND_COUNT; ++i) {
            gap.spanCount[i] = trace.costs[i].count - serviceStartCosts[i].count;
            gap.spanNs[i] = trace.costs[i].totalNs - serviceStartCosts[i].totalNs;
        }
        uint64_t elapsed = startNs - trace.lastServiceStartNs;
        if (elapsed >= HG_AUDIO_CHECK_LONG_NS) {
            if (!trace.firstServiceGap.record.endNs) trace.firstServiceGap = gap;
            if (!trace.largestServiceGap.record.endNs
                || elapsed > trace.largestServiceGap.record.endNs - trace.largestServiceGap.record.startNs) trace.largestServiceGap = gap;
        }
        record(gap.record);
    }
    memcpy(serviceStartCosts, trace.costs, sizeof(serviceStartCosts));
    service = (HGNativeAudioCheckRecord){ HG_AUDIO_CHECK_SERVICE, startNs, 0, seq, source, flags, 0 };
    trace.lastServiceStartNs = startNs; trace.lastServiceEndNs = 0;
    trace.lastSource = source; trace.lastFlags = flags; trace.lastDueFrames = trace.lastOutputFrames = 0;
    return seq;
}

int HGNativeAudioCheckServiceEnd(uint64_t serviceSeq, uint64_t endNs, uint64_t dueFrames, uint64_t outputFrames,
    uint64_t synthNs, uint64_t outputNs) {
    if (!isOwner()) return 0;
    if (!serviceSeq || serviceSeq != service.serviceSeq || endNs < service.startNs || outputFrames > dueFrames
        || synthNs > endNs - service.startNs || outputNs > endNs - service.startNs) return invalid();
    record((HGNativeAudioCheckRecord){ HG_AUDIO_CHECK_SERVICE, service.startNs, endNs, serviceSeq, service.a, dueFrames, outputFrames });
    ++trace.serviceCount; trace.dueFrames += dueFrames; trace.outputFrames += outputFrames;
    trace.synthNs += synthNs; trace.outputNs += outputNs;
    trace.lastServiceEndNs = endNs; trace.lastDueFrames = dueFrames; trace.lastOutputFrames = outputFrames;
    HGNativeAudioCheckGap *gaps[] = { &trace.firstServiceGap, &trace.largestServiceGap };
    for (unsigned i = 0; i < 2; ++i) if (gaps[i]->record.serviceSeq == serviceSeq) {
        gaps[i]->dueFrames = dueFrames; gaps[i]->outputFrames = outputFrames;
        gaps[i]->synthNs = synthNs; gaps[i]->outputNs = outputNs;
    }
    memset(&service, 0, sizeof(service));
    return 1;
}

int HGNativeAudioCheckGetSnapshot(HGNativeAudioCheckSnapshot *out) {
    if (!isOwner()) return 0;
    if (!out) return invalid();
    *out = trace;
    out->clockFailures = atomic_load_explicit(&clockFailures, memory_order_relaxed);
    out->otherThreadCalls = atomic_load_explicit(&otherThreadCalls, memory_order_relaxed);
    return 1;
}
#endif
