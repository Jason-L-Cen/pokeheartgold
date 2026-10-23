// SPDX-License-Identifier: GPL-3.0-or-later
#include "NativeAudioCheck.h"
#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>

static void *OtherThread(void *unused) {
    (void)unused;
    HGNativeAudioCheckSnapshot out;
    memset(&out, 0xA5, sizeof(out));
    assert(!HGNativeAudioCheckSpan(HG_AUDIO_CHECK_FILE, 1, 2, 0, 0, 0));
    assert(!HGNativeAudioCheckServiceBegin(HG_AUDIO_CHECK_SOURCE_HOST, 3, 1));
    assert(!HGNativeAudioCheckServiceEnd(3, 2, 0, 0, 0, 0));
    assert(!HGNativeAudioCheckGetSnapshot(&out));
    for (size_t i = 0; i < sizeof(out); i++) assert(((unsigned char *)&out)[i] == 0xA5);
    return NULL;
}

int main(void) {
    assert(sizeof(HGNativeAudioCheckRecord) == 72 && sizeof(HGNativeAudioCheckSnapshot) == 3320);
    const uint64_t base = UINT64_C(1000000000), threshold = HG_AUDIO_CHECK_LONG_NS;
    HGNativeAudioCheckSnapshot state;
    assert(!HGNativeAudioCheckReset(0, base) && !HGNativeAudioCheckReset(1, 0));
    assert(HGNativeAudioCheckReset(1, base) && HGNativeAudioCheckNow());
    assert(HGNativeAudioCheckGetSnapshot(&state));
    assert(state.version == HG_AUDIO_CHECK_VERSION && state.bootEpoch == 1 && state.bootStartNs == base);
    assert(state.thresholdNs == threshold && !state.recordCount && !state.sequence && !state.invalidCalls && !state.clockFailures);

    assert(HGNativeAudioCheckSpan(HG_AUDIO_CHECK_FILE, base + 1, base + 11, 101, 4096, 4096));
    assert(HGNativeAudioCheckSpan(HG_AUDIO_CHECK_FILE, base + 20, base + 20 + threshold - 1, 102, 8192, 8192));
    assert(HGNativeAudioCheckSpan(HG_AUDIO_CHECK_WAVE, base + 30, base + 30 + threshold, 123, 456, 1));
    assert(HGNativeAudioCheckGetSnapshot(&state));
    assert(state.costs[HG_AUDIO_CHECK_FILE].count == 2 && state.costs[HG_AUDIO_CHECK_FILE].totalNs == threshold + 9);
    assert(state.costs[HG_AUDIO_CHECK_FILE].maxNs == threshold - 1);
    assert(state.costs[HG_AUDIO_CHECK_WAVE].count == 1 && state.costs[HG_AUDIO_CHECK_WAVE].totalNs == threshold && state.costs[HG_AUDIO_CHECK_WAVE].maxNs == threshold);
    assert(state.recordCount == 1 && state.sequence == 1 && state.nextRecord == 1 && !state.overwriteCount);
    assert(state.records[0].kind == HG_AUDIO_CHECK_WAVE && state.records[0].startNs == base + 30 && state.records[0].endNs == base + 30 + threshold);
    assert(state.records[0].a == 123 && state.records[0].b == 456 && state.records[0].c == 1);
    assert(!HGNativeAudioCheckSpan(0, base, base + 1, 0, 0, 0));
    assert(!HGNativeAudioCheckSpan(HG_AUDIO_CHECK_KIND_COUNT, base, base + 1, 0, 0, 0));
    assert(!HGNativeAudioCheckSpan(HG_AUDIO_CHECK_FILE, 0, base, 0, 0, 0));
    assert(!HGNativeAudioCheckSpan(HG_AUDIO_CHECK_FILE, base + 1, base, 0, 0, 0));
    assert(!HGNativeAudioCheckGetSnapshot(NULL));
    assert(!HGNativeAudioCheckServiceBegin(0, 3, base));
    assert(!HGNativeAudioCheckServiceEnd(0, base, 0, 0, 0, 0));
    assert(HGNativeAudioCheckGetSnapshot(&state) && state.invalidCalls == 7 && state.recordCount == 1);

    assert(HGNativeAudioCheckReset(2, base));
    const uint64_t firstStart = base + 1, secondStart = firstStart + threshold, thirdStart = secondStart + threshold * 2;
    uint64_t first = HGNativeAudioCheckServiceBegin(HG_AUDIO_CHECK_SOURCE_HOST, 3, firstStart);
    assert(first == 1);
    assert(!HGNativeAudioCheckServiceBegin(HG_AUDIO_CHECK_SOURCE_HOST, 3, firstStart + 1));
    assert(!HGNativeAudioCheckServiceEnd(first + 1, firstStart + 200, 8, 8, 100, 40));
    assert(!HGNativeAudioCheckServiceEnd(first, firstStart - 1, 8, 8, 100, 40));
    assert(!HGNativeAudioCheckServiceEnd(first, firstStart + 200, 8, 9, 100, 40));
    assert(HGNativeAudioCheckServiceEnd(first, firstStart + 200, 8, 8, 100, 40));
    assert(HGNativeAudioCheckSpan(HG_AUDIO_CHECK_FILE, firstStart + 1000, firstStart + 1100, 1, 2, 3));
    assert(HGNativeAudioCheckSpan(HG_AUDIO_CHECK_WAVE, firstStart + 2000, firstStart + 2200, 4, 5, 6));
    assert(HGNativeAudioCheckSpan(HG_AUDIO_CHECK_APP_RENDER, firstStart + 3000, firstStart + 3300, 7, 8, 9));
    assert(HGNativeAudioCheckSpan(HG_AUDIO_CHECK_APP_COPY, firstStart + 4000, firstStart + 4400, 10, 11, 12));
    uint64_t second = HGNativeAudioCheckServiceBegin(HG_AUDIO_CHECK_SOURCE_SDK_PUMP, 3, secondStart);
    assert(second == 2 && HGNativeAudioCheckServiceEnd(second, secondStart + 500, 960, 960, 110, 90));
    assert(HGNativeAudioCheckGetSnapshot(&state));
    HGNativeAudioCheckGap firstGap = state.firstServiceGap;
    assert(firstGap.record.kind == HG_AUDIO_CHECK_SERVICE_GAP && firstGap.record.startNs == firstStart && firstGap.record.endNs == secondStart && firstGap.record.serviceSeq == second);
    assert(firstGap.record.a == HG_AUDIO_CHECK_SOURCE_SDK_PUMP && firstGap.record.b == 3 && firstGap.record.c == HG_AUDIO_CHECK_SOURCE_HOST);
    assert(firstGap.record.cpuWallStartNs == firstStart + 200);
    assert(!memcmp(&state.records[0], &firstGap.record, sizeof(firstGap.record)));
    assert(firstGap.previousServiceSeq == first && firstGap.previousFlags == 3 && firstGap.previousDueFrames == 8 && firstGap.previousOutputFrames == 8);
    assert(firstGap.dueFrames == 960 && firstGap.outputFrames == 960 && firstGap.synthNs == 110 && firstGap.outputNs == 90);
    const unsigned shortKinds[] = { HG_AUDIO_CHECK_FILE, HG_AUDIO_CHECK_WAVE, HG_AUDIO_CHECK_APP_RENDER, HG_AUDIO_CHECK_APP_COPY };
    for (unsigned i = 0; i < 4; i++) assert(firstGap.spanCount[shortKinds[i]] == 1 && firstGap.spanNs[shortKinds[i]] == (i + 1) * 100);
    assert(state.serviceCount == 2 && state.dueFrames == 968 && state.outputFrames == 968 && state.synthNs == 210 && state.outputNs == 130);
    assert(state.costs[HG_AUDIO_CHECK_SERVICE].count == 2 && state.costs[HG_AUDIO_CHECK_SERVICE].totalNs == 700 && state.costs[HG_AUDIO_CHECK_SERVICE].maxNs == 500);
    assert(state.recordCount == 1 && state.invalidCalls == 4);
    assert(HGNativeAudioCheckSpan(HG_AUDIO_CHECK_FILE, secondStart + 1000, secondStart + 1050, 13, 14, 15));
    uint64_t third = HGNativeAudioCheckServiceBegin(HG_AUDIO_CHECK_SOURCE_HOST, 3, thirdStart);
    assert(third == 3 && HGNativeAudioCheckServiceEnd(third, thirdStart + 600, 1920, 1910, 120, 100));
    assert(HGNativeAudioCheckGetSnapshot(&state));
    HGNativeAudioCheckGap largestGap = state.largestServiceGap;
    assert(!memcmp(&state.firstServiceGap, &firstGap, sizeof(firstGap)));
    assert(largestGap.record.startNs == secondStart && largestGap.record.endNs == thirdStart && largestGap.record.serviceSeq == third);
    assert(largestGap.record.cpuWallStartNs == secondStart + 500);
    assert(!memcmp(&state.records[1], &largestGap.record, sizeof(largestGap.record)));
    assert(largestGap.previousServiceSeq == second && largestGap.previousDueFrames == 960 && largestGap.previousOutputFrames == 960);
    assert(largestGap.dueFrames == 1920 && largestGap.outputFrames == 1910 && largestGap.synthNs == 120 && largestGap.outputNs == 100);
    assert(largestGap.spanCount[HG_AUDIO_CHECK_FILE] == 1 && largestGap.spanNs[HG_AUDIO_CHECK_FILE] == 50 && !largestGap.spanCount[HG_AUDIO_CHECK_WAVE]);
    assert(state.serviceCount == 3 && state.dueFrames == 2888 && state.outputFrames == 2878);
    assert(state.lastServiceStartNs == thirdStart && state.lastServiceEndNs == thirdStart + 600 && state.lastDueFrames == 1920 && state.lastOutputFrames == 1910);

    for (unsigned i = 0; i < 40; i++) {
        uint64_t start = base + threshold * 10 + i * (threshold + 1);
        assert(HGNativeAudioCheckSpan(HG_AUDIO_CHECK_APP_COPY, start, start + threshold + i, i, i + 10, i + 20));
    }
    assert(HGNativeAudioCheckGetSnapshot(&state));
    assert(state.sequence == 42 && state.recordCount == HG_AUDIO_CHECK_RECORDS && state.overwriteCount == 10 && state.nextRecord == 10);
    unsigned oldest = (state.nextRecord + HG_AUDIO_CHECK_RECORDS - state.recordCount) % HG_AUDIO_CHECK_RECORDS;
    for (unsigned i = 0; i < HG_AUDIO_CHECK_RECORDS; i++) {
        const HGNativeAudioCheckRecord *record = &state.records[(oldest + i) % HG_AUDIO_CHECK_RECORDS];
        uint64_t expected = i + 8, start = base + threshold * 10 + expected * (threshold + 1);
        assert(record->kind == HG_AUDIO_CHECK_APP_COPY && record->startNs == start && record->endNs == start + threshold + expected);
        assert(record->a == expected && record->b == expected + 10 && record->c == expected + 20);
        assert(!record->cpuWallStartNs && !record->cpuElapsedNs);
    }
    assert(!memcmp(&state.firstServiceGap, &firstGap, sizeof(firstGap)) && !memcmp(&state.largestServiceGap, &largestGap, sizeof(largestGap)));
    state.records[oldest].a = UINT64_MAX;
    state.firstServiceGap.record.startNs = 0;
    assert(HGNativeAudioCheckGetSnapshot(&state) && state.records[oldest].a == 8 && !memcmp(&state.firstServiceGap, &firstGap, sizeof(firstGap)));
    pthread_t other;
    assert(!pthread_create(&other, NULL, OtherThread, NULL) && !pthread_join(other, NULL));
    assert(HGNativeAudioCheckGetSnapshot(&state) && state.otherThreadCalls == 4 && state.serviceSeq == third && state.recordCount == HG_AUDIO_CHECK_RECORDS);
    assert(state.costs[HG_AUDIO_CHECK_FILE].count == 2 && state.costs[HG_AUDIO_CHECK_FILE].totalNs == 150);
    assert(HGNativeAudioCheckReset(77, base * 2) && HGNativeAudioCheckGetSnapshot(&state));
    assert(state.bootEpoch == 77 && state.bootStartNs == base * 2 && !state.sequence && !state.recordCount && !state.nextRecord && !state.overwriteCount);
    assert(!state.invalidCalls && !state.clockFailures && !state.otherThreadCalls && !state.serviceSeq && !state.serviceCount && !state.dueFrames && !state.outputFrames);
    assert(!state.firstServiceGap.record.kind && !state.largestServiceGap.record.kind);
    for (unsigned i = 0; i < HG_AUDIO_CHECK_KIND_COUNT; i++) assert(!state.costs[i].count && !state.costs[i].totalNs && !state.costs[i].maxNs);
    const unsigned flags[] = { 3, 0, 3, 3 };
    const uint64_t offsets[] = { 0, threshold, threshold * 2, threshold * 5 / 2 };
    for (unsigned i = 0; i < 4; i++) {
        uint64_t start = base * 2 + 1 + offsets[i];
        uint64_t service = HGNativeAudioCheckServiceBegin(HG_AUDIO_CHECK_SOURCE_MODE_CHANGE, flags[i], start);
        assert(service == i + 1 && HGNativeAudioCheckServiceEnd(service, start + 100, 1, 1, 20, 10));
    }
    assert(HGNativeAudioCheckGetSnapshot(&state) && !state.firstServiceGap.record.kind && !state.largestServiceGap.record.kind);
    assert(state.costs[HG_AUDIO_CHECK_SERVICE_GAP].count == 1 && state.costs[HG_AUDIO_CHECK_SERVICE_GAP].totalNs == threshold / 2 && state.costs[HG_AUDIO_CHECK_SERVICE_GAP].maxNs == threshold / 2);
    assert(!state.recordCount && state.serviceCount == 4);
    assert(HGNativeAudioCheckReset(78, base * 3));
    uint64_t cpuStart = HGNativeAudioCheckThreadCPU();
    uint64_t cpuEnd = HGNativeAudioCheckThreadCPU();
    assert(cpuStart && cpuEnd >= cpuStart);
    uint64_t cpuElapsed = cpuEnd > cpuStart ? cpuEnd - cpuStart : 0;
    assert(HGNativeAudioCheckSpan(HG_AUDIO_CHECK_SYNTH, base * 3, base * 3 + threshold, HG_AUDIO_CHECK_SOURCE_HOST, 1024, cpuElapsed));
    assert(HGNativeAudioCheckSpan(HG_AUDIO_CHECK_OUTPUT, base * 3 + threshold, base * 3 + threshold * 2, HG_AUDIO_CHECK_SOURCE_HOST, 512, 123));
    assert(HGNativeAudioCheckGetSnapshot(&state) && state.version == 3 && !state.clockFailures && state.recordCount == 2);
    assert(state.records[0].kind == HG_AUDIO_CHECK_SYNTH && state.records[0].c == cpuElapsed);
    assert(state.records[1].kind == HG_AUDIO_CHECK_OUTPUT && state.records[1].c == 123);
    assert(!state.records[0].cpuWallStartNs && !state.records[0].cpuElapsedNs && !state.records[1].cpuWallStartNs && !state.records[1].cpuElapsedNs);
    assert(HGNativeAudioCheckReset(79, base * 4));
    uint64_t previousEnd = base * 4 + 100;
    uint64_t current = HGNativeAudioCheckServiceBegin(HG_AUDIO_CHECK_SOURCE_HOST, 3, base * 4);
    assert(current && HGNativeAudioCheckServiceEnd(current, previousEnd, 1, 1, 0, 0));
    assert(HGNativeAudioCheckGetSnapshot(&state) && !state.recordCount && !state.firstServiceGap.record.cpuWallStartNs && !state.firstServiceGap.record.cpuElapsedNs);
    for (unsigned i = 0; i < 3; ++i) {
        uint64_t start = previousEnd + threshold * (3 - i);
        current = HGNativeAudioCheckServiceBegin(HG_AUDIO_CHECK_SOURCE_HOST, 3, start);
        assert(current && HGNativeAudioCheckServiceEnd(current, start + 100, 1, 1, 0, 0));
        assert(HGNativeAudioCheckGetSnapshot(&state) && state.recordCount == i + 1);
        assert(state.records[i].kind == HG_AUDIO_CHECK_SERVICE_GAP && state.records[i].cpuWallStartNs == previousEnd);
        previousEnd = start + 100;
    }
    assert(state.firstServiceGap.record.serviceSeq == 2 && state.largestServiceGap.record.serviceSeq == 2);
    assert(state.records[1].serviceSeq == 3 && state.records[2].serviceSeq == 4);
    HGNativeAudioCheckRecord retained = state.records[2];
    state.records[2].cpuWallStartNs = state.records[2].cpuElapsedNs = UINT64_MAX;
    assert(HGNativeAudioCheckGetSnapshot(&state) && !memcmp(&state.records[2], &retained, sizeof(retained)));
    assert(HGNativeAudioCheckReset(80, base * 5) && HGNativeAudioCheckGetSnapshot(&state));
    assert(!state.recordCount && !state.firstServiceGap.record.cpuWallStartNs && !state.largestServiceGap.record.cpuElapsedNs);
    current = HGNativeAudioCheckServiceBegin(HG_AUDIO_CHECK_SOURCE_HOST, 3, base * 5);
    assert(current && HGNativeAudioCheckServiceEnd(current, base * 5 + threshold, 1, 1, 0, 0));
    current = HGNativeAudioCheckServiceBegin(HG_AUDIO_CHECK_SOURCE_HOST, 3, base * 5 + threshold);
    assert(current && HGNativeAudioCheckServiceEnd(current, base * 5 + threshold + 1, 1, 1, 0, 0));
    assert(HGNativeAudioCheckGetSnapshot(&state));
    // An empty end-to-begin wall interval cannot explain CPU work using the longer start-to-start gap.
    assert(state.firstServiceGap.record.cpuWallStartNs == state.firstServiceGap.record.endNs);
    assert(state.firstServiceGap.record.startNs < state.firstServiceGap.record.cpuWallStartNs);
    puts("PASS: audio CHECK span thresholds/costs, invalid calls, service gap anchors/CPU intervals, bounded chronological ring, snapshot copies, owner guard, boot reset and thread CPU clock/record semantics.");
    return 0;
}
