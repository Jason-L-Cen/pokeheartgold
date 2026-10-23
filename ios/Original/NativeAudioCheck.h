// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef HEARTGOLD_NATIVE_AUDIO_CHECK_H
#define HEARTGOLD_NATIVE_AUDIO_CHECK_H

#include <stdint.h>

enum { HG_AUDIO_CHECK_RECORDS = 32, HG_AUDIO_CHECK_VERSION = 3 };
#define HG_AUDIO_CHECK_LONG_NS UINT64_C(20000000)

enum {
    HG_AUDIO_CHECK_SERVICE = 1, HG_AUDIO_CHECK_SYNTH, HG_AUDIO_CHECK_OUTPUT,
    HG_AUDIO_CHECK_SDK_PUMP, HG_AUDIO_CHECK_FILE, HG_AUDIO_CHECK_WAVE,
    HG_AUDIO_CHECK_APP_RENDER, HG_AUDIO_CHECK_APP_COPY, HG_AUDIO_CHECK_SERVICE_GAP,
    HG_AUDIO_CHECK_KIND_COUNT
};
enum {
    HG_AUDIO_CHECK_SOURCE_HOST = 1, HG_AUDIO_CHECK_SOURCE_SDK_PUMP,
    HG_AUDIO_CHECK_SOURCE_SET_OUTPUT, HG_AUDIO_CHECK_SOURCE_DETACH_OUTPUT,
    HG_AUDIO_CHECK_SOURCE_MODE_CHANGE
};
enum { HG_AUDIO_CHECK_REALTIME = 1, HG_AUDIO_CHECK_OUTPUT_ATTACHED = 2 };

typedef struct HGNativeAudioCheckRecord {
    uint64_t kind, startNs, endNs, serviceSeq, a, b, c;
    // GAP only: CPU covers the prior completed service end through this entry, not startNs..endNs.
    uint64_t cpuWallStartNs, cpuElapsedNs;
} HGNativeAudioCheckRecord;

typedef struct HGNativeAudioCheckCost {
    uint64_t count, totalNs, maxNs;
} HGNativeAudioCheckCost;

typedef struct HGNativeAudioCheckGap {
    HGNativeAudioCheckRecord record; // a=current source, b=current flags, c=previous source.
    uint64_t previousServiceSeq, previousFlags, previousDueFrames, previousOutputFrames;
    uint64_t dueFrames, outputFrames, synthNs, outputNs;
    // Completed-span counter deltas between entries, not interval-clipped coverage; spans can overlap.
    uint64_t spanCount[HG_AUDIO_CHECK_KIND_COUNT], spanNs[HG_AUDIO_CHECK_KIND_COUNT];
} HGNativeAudioCheckGap;

typedef struct HGNativeAudioCheckSnapshot {
    uint64_t version, bootEpoch, bootStartNs, thresholdNs;
    uint64_t sequence, recordCount, nextRecord, overwriteCount;
    uint64_t invalidCalls, clockFailures, otherThreadCalls;
    uint64_t serviceSeq, serviceCount, dueFrames, outputFrames, synthNs, outputNs;
    uint64_t lastServiceStartNs, lastServiceEndNs, lastSource, lastFlags, lastDueFrames, lastOutputFrames;
    HGNativeAudioCheckCost costs[HG_AUDIO_CHECK_KIND_COUNT];
    HGNativeAudioCheckGap firstServiceGap, largestServiceGap; // First/largest >=20ms eligible gaps; ties retain the first.
    HGNativeAudioCheckRecord records[HG_AUDIO_CHECK_RECORDS];
} HGNativeAudioCheckSnapshot;

// CHECK singleton: the owner resets only after the prior boot has stopped; epoch/startNs must be nonzero.
int HGNativeAudioCheckReset(uint64_t bootEpoch, uint64_t startNs);
uint64_t HGNativeAudioCheckNow(void); // CLOCK_MONOTONIC; zero and clockFailures on failure.
// Owner's CLOCK_THREAD_CPUTIME_ID; zero if unbound/wrong owner, or on clock failure (clockFailures).
uint64_t HGNativeAudioCheckThreadCPU(void);

// Span accepts SYNTH..APP_COPY; SERVICE/GAP are produced by the service pair.
// Every completed span updates costs; only spans >=20ms enter the fixed ring.
// SYNTH/OUTPUT a/b/c=source/frames/thread CPU elapsed ns. c=0 is unavailable/below resolution, never proof of zero CPU.
// FILE a/b/c=command/requested READ bytes/raw result; WAVE=encoded bytes/decoded frames/error (zero=success).
// SDK_PUMP a/b/c=initial batches/executed commands/remaining batches; its span includes status updates.
// APP a/b/c=scene/tick/result. Costs may overlap and must not be summed as elapsed time.
int HGNativeAudioCheckSpan(uint64_t kind, uint64_t startNs, uint64_t endNs, uint64_t a, uint64_t b, uint64_t c);

// Begin returns a nonzero serviceSeq. Gaps require both adjacent calls to have REALTIME|OUTPUT_ATTACHED.
// SERVICE records a/b/c=source/dueFrames/frames passed to output callback (not consumer acceptance).
// GAP keeps its start-to-start wall timestamps; CPU starts at cpuWallStartNs=previous service end.
// cpuElapsedNs=0 is unavailable/below resolution, never proof of zero CPU; non-GAP fields stay zero.
// End also retains timing and first/largest gap anchors.
uint64_t HGNativeAudioCheckServiceBegin(uint64_t source, uint64_t flags, uint64_t startNs);
int HGNativeAudioCheckServiceEnd(uint64_t serviceSeq, uint64_t endNs, uint64_t dueFrames, uint64_t outputFrames,
    uint64_t synthNs, uint64_t outputNs);

// Owner only; copy this fixed snapshot before publishing immutable bytes to another thread.
// Ring order starts at (nextRecord + HG_AUDIO_CHECK_RECORDS - recordCount) % HG_AUDIO_CHECK_RECORDS.
int HGNativeAudioCheckGetSnapshot(HGNativeAudioCheckSnapshot *out);

#endif
