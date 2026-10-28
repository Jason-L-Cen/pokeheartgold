// SPDX-License-Identifier: GPL-3.0-or-later
#import "OriginalAudio.h"
#include <assert.h>
#include <dispatch/dispatch.h>
#include <stdio.h>

static void unavailable(OriginalAudio *audio) {
    NSDictionary<NSString *,NSNumber *> *stats=audio.statistics;
    assert(stats[@"available"] && ![stats[@"available"] boolValue]);
    assert([stats[@"audioDeviceSuppressed"] boolValue]);
    for (NSString *key in @[@"muted",@"queued",@"submitted",@"consumed",@"nonzeroSamples",@"dropped",@"underrunFrames",@"rendered",@"firstShortTimeNs",@"firstShortFrames",@"firstShortAvailable",@"firstShortRead",@"firstShortRendered",@"largestCallback",@"rejectedCapacityFrames",@"rejectedInvalidFrames",@"rejectedConvertFrames",@"firstRejectReason",@"firstRejectFrames",@"firstRejectQueued",@"firstRejectSubmitted",@"firstRejectConsumed",@"firstRejectRendered",@"largestEnqueueBatch",@"lastEnqueueNs",@"largestEnqueueGapConsistent",@"largestEnqueueGapNs",@"largestEnqueueGapEndNs",@"largestEnqueueGapSubmitted",@"largestEnqueueGapConsumed",@"largestEnqueueGapRendered",@"largestEnqueueGapQueued",@"reprimeCount",@"firstReprimeAvailable",@"firstReprimeRead",@"firstReprimeRendered",@"renderedNonzeroSamples"])
        assert(stats[key] && ![stats[key] unsignedLongLongValue]);
    assert(!audio.failure && !audio.firstShortTimeNs);
}

int main(void) { @autoreleasepool {
    OriginalAudio *audio=[OriginalAudio new];
    NSError *error=nil;
    unavailable(audio);
    assert(![audio.statistics[@"opened"] boolValue] && ![audio.statistics[@"closed"] boolValue]);
    assert(![audio resume:&error] && error);
    assert([audio open:&error] && !error && [audio.statistics[@"opened"] boolValue]);
    assert([audio open:NULL]);
    const int16_t pcm[]={INT16_MIN,INT16_MAX,0,1};
    [audio enqueue:pcm frames:2];
    [audio enqueue:NULL frames:0];
    [audio enqueue:(const int16_t *)(uintptr_t)1 frames:SIZE_MAX];
    unavailable(audio);
    [audio pause];
    assert([audio.statistics[@"opened"] boolValue] && [audio.statistics[@"paused"] boolValue]);
    assert([audio open:NULL] && [audio.statistics[@"paused"] boolValue]);
    assert([audio resume:&error] && !error && ![audio.statistics[@"paused"] boolValue]);
    const int16_t *samples=pcm;
    dispatch_apply(16,dispatch_get_global_queue(QOS_CLASS_USER_INITIATED,0),^(size_t index) { @autoreleasepool {
        for (unsigned i=0;i<100;i++) {
            [audio pause];[audio resume:NULL];[audio enqueue:samples frames:2];unavailable(audio);
        }
    }});
    [audio close];[audio close];
    assert(![audio.statistics[@"opened"] boolValue] && [audio.statistics[@"closed"] boolValue]);
    assert(![audio resume:&error] && error);
    assert(![audio open:&error] && error);
    [audio enqueue:pcm frames:2];[audio pause];
    unavailable(audio);
    assert([audio.statistics[@"closed"] boolValue]);
    OriginalAudio *closedBeforeOpen=[OriginalAudio new];
    [closedBeforeOpen close];assert(![closedBeforeOpen open:NULL] && ![closedBeforeOpen resume:NULL]);
    OriginalAudio *reset=[OriginalAudio new];
    assert([reset open:&error] && !error && [reset.statistics[@"opened"] boolValue]);
    unavailable(reset);[reset close];unavailable(reset);
    puts("No-device Foundation lifecycle PASS; audio remains unavailable and untested.");
    return 0;
}}
