// SPDX-License-Identifier: GPL-3.0-or-later
#import "../OriginalApp/OriginalAudio.m"
#include <assert.h>
#include <stdio.h>
#include <sched.h>

int main(void) { @autoreleasepool {
    AVAudioFormat *format = [[AVAudioFormat alloc] initStandardFormatWithSampleRate:48000 channels:2];
    int16_t pcm[2048];
    assert(!convertPCM(NULL, 1, format) && !convertPCM(pcm, 0, format) && !convertPCM(pcm, 1025, format));
    const int16_t edges[] = {INT16_MIN, INT16_MAX, -1, 0};
    AVAudioPCMBuffer *edgeBuffer = convertPCM(edges, 2, format);
    assert(edgeBuffer.frameLength == 2 && edgeBuffer.floatChannelData[0][0] == -1);
    assert(edgeBuffer.floatChannelData[1][0] == 32767 / 32768.0f);
    assert(edgeBuffer.floatChannelData[0][1] == -1 / 32768.0f && !edgeBuffer.floatChannelData[1][1]);
    for (unsigned i = 0; i < 1024; i++) { pcm[i * 2] = 16384; pcm[i * 2 + 1] = -8192; }
    AVAudioPCMBuffer *buffer = convertPCM(pcm, 1024, format);
    memset(pcm, 0, sizeof(pcm));
    for (unsigned i = 0; i < 1024; i++) assert(buffer.floatChannelData[0][i] == .5f && buffer.floatChannelData[1][i] == -.25f);
    NSError *error;
    OriginalAudio *audio = [OriginalAudio new];
    assert([audio open:&error]);
    AVAudioEngine *stream = [audio valueForKey:@"engine"];
    [stream stop];
    assert([stream enableManualRenderingMode:AVAudioEngineManualRenderingModeOffline format:format maximumFrameCount:1024 error:&error]);
    stream.mainMixerNode.outputVolume = 1;
    assert([stream startAndReturnError:&error]);
    AVAudioPCMBuffer *streamOutput = [[AVAudioPCMBuffer alloc] initWithPCMFormat:format frameCapacity:1024];
    unsigned nonzero = 0;
    for (unsigned cycle = 0; cycle < 32; cycle++) {
        for (unsigned i = 0; i < 1024; i++) { pcm[i * 2] = 16384; pcm[i * 2 + 1] = -8192; }
        for (unsigned i = 0; i < 4; i++) [audio enqueue:pcm frames:1024];
        memset(pcm,0,sizeof(pcm));
        for (unsigned i = 0; i < 4; i++) {
            assert([stream renderOffline:1024 toBuffer:streamOutput error:&error] == AVAudioEngineManualRenderingStatusSuccess);
            for (unsigned j = 0; j < streamOutput.frameLength; j++) {
                float left=streamOutput.floatChannelData[0][j],right=streamOutput.floatChannelData[1][j];
                if (left || right) { assert(left==.5f && right==-.25f); nonzero++; }
            }
        }
    }
    NSDictionary *streamStats = audio.statistics;
    fprintf(stderr,"Offline stream submitted=%llu consumed=%llu queued=%llu dropped=%llu\n",[streamStats[@"submitted"] unsignedLongLongValue],[streamStats[@"consumed"] unsignedLongLongValue],[streamStats[@"queued"] unsignedLongLongValue],[streamStats[@"dropped"] unsignedLongLongValue]);
    assert(![streamStats[@"dropped"] unsignedLongLongValue]);
    assert(nonzero==32*4096 && [streamStats[@"consumed"] unsignedLongLongValue]==nonzero && ![streamStats[@"queued"] unsignedLongLongValue]);
    assert([stream renderOffline:1024 toBuffer:streamOutput error:&error]==AVAudioEngineManualRenderingStatusSuccess);
    NSDictionary *shortage=audio.statistics;
    assert([shortage[@"underrunFrames"] unsignedLongLongValue]==1024);
    assert([shortage[@"firstShortFrames"] unsignedLongLongValue]>0 && [shortage[@"firstShortFrames"] unsignedLongLongValue]<=[shortage[@"largestCallback"] unsignedLongLongValue]);
    assert(![shortage[@"firstShortAvailable"] unsignedLongLongValue] && [shortage[@"firstShortRead"] isEqual:streamStats[@"consumed"]]);
    assert([shortage[@"firstShortRendered"] isEqual:streamStats[@"rendered"]]);
    for (unsigned i = 0; i < 1024; i++) { pcm[i*2]=16384; pcm[i*2+1]=-8192; }
    [audio enqueue:pcm frames:1024]; [audio enqueue:pcm frames:1023];
    assert([stream renderOffline:512 toBuffer:streamOutput error:&error]==AVAudioEngineManualRenderingStatusSuccess);
    NSDictionary *waiting=audio.statistics;
    assert([waiting[@"queued"] unsignedLongLongValue]==2047 && [waiting[@"consumed"] isEqual:shortage[@"consumed"]]);
    assert([waiting[@"underrunFrames"] unsignedLongLongValue]==[shortage[@"underrunFrames"] unsignedLongLongValue]+512);
    for (unsigned i=0;i<streamOutput.frameLength;i++) assert(!streamOutput.floatChannelData[0][i] && !streamOutput.floatChannelData[1][i]);
    [audio enqueue:pcm frames:1];
    for (unsigned i=0;i<4;i++) {
        assert([stream renderOffline:512 toBuffer:streamOutput error:&error]==AVAudioEngineManualRenderingStatusSuccess);
        for (unsigned j=0;j<streamOutput.frameLength;j++) assert(streamOutput.floatChannelData[0][j]==.5f && streamOutput.floatChannelData[1][j]==-.25f);
    }
    NSDictionary *reprimed=audio.statistics;
    assert(![reprimed[@"queued"] unsignedLongLongValue] && [reprimed[@"consumed"] unsignedLongLongValue]==[shortage[@"consumed"] unsignedLongLongValue]+2048);
    assert([reprimed[@"renderedNonzeroSamples"] unsignedLongLongValue]==[shortage[@"renderedNonzeroSamples"] unsignedLongLongValue]+4096);
    assert([reprimed[@"underrunFrames"] isEqual:waiting[@"underrunFrames"]]);
    assert([reprimed[@"firstReprimeAvailable"] unsignedLongLongValue]==2048 && [reprimed[@"firstReprimeRead"] isEqual:shortage[@"consumed"]]);
    assert([reprimed[@"firstReprimeRendered"] isEqual:waiting[@"rendered"]] && [reprimed[@"reprimeCount"] unsignedLongLongValue]==1);
    uint64_t target=[audio.statistics[@"submitted"] unsignedLongLongValue]+256*1024;
    uint64_t renderedNonzero=[audio.statistics[@"renderedNonzeroSamples"] unsignedLongLongValue];
    NSTimeInterval deadline=NSProcessInfo.processInfo.systemUptime+10;
    dispatch_group_t rendering=dispatch_group_create();
    dispatch_group_async(rendering,dispatch_get_global_queue(QOS_CLASS_USER_INITIATED,0),^{ @autoreleasepool {
        NSError *renderError;
        while ([audio.statistics[@"consumed"] unsignedLongLongValue]<target) {
            assert(NSProcessInfo.processInfo.systemUptime<deadline);
            NSDictionary *ready=audio.statistics;
            if ([ready[@"queued"] unsignedLongLongValue]<2048 && [ready[@"submitted"] unsignedLongLongValue]<target) { sched_yield(); continue; }
            assert([stream renderOffline:1024 toBuffer:streamOutput error:&renderError]==AVAudioEngineManualRenderingStatusSuccess);
            for (unsigned i=0;i<streamOutput.frameLength;i++) {
                float left=streamOutput.floatChannelData[0][i],right=streamOutput.floatChannelData[1][i];
                assert((!left && !right) || (left==.5f && right==-.25f));
            }
        }
    }});
    for (unsigned i=0;i<256;i++) {
        while ([audio.statistics[@"queued"] unsignedLongLongValue]>4096) {
            assert(NSProcessInfo.processInfo.systemUptime<deadline); sched_yield();
        }
        [audio enqueue:pcm frames:1024];
    }
    assert(!dispatch_group_wait(rendering,dispatch_time(DISPATCH_TIME_NOW,10*NSEC_PER_SEC)));
    assert([audio.statistics[@"renderedNonzeroSamples"] unsignedLongLongValue]==renderedNonzero+256*1024*2 && ![audio.statistics[@"dropped"] unsignedLongLongValue]);
    assert([audio.statistics[@"submitted"] unsignedLongLongValue]==target && [audio.statistics[@"consumed"] unsignedLongLongValue]==target && ![audio.statistics[@"queued"] unsignedLongLongValue]);
    assert([audio.statistics[@"underrunFrames"] isEqual:reprimed[@"underrunFrames"]]);
    for (unsigned i = 0; i < 4; i++) [audio enqueue:pcm frames:1024];
    assert([stream renderOffline:1024 toBuffer:streamOutput error:&error]==AVAudioEngineManualRenderingStatusSuccess);
    [audio pause]; NSDictionary *paused=audio.statistics;
    assert([paused[@"queued"] unsignedLongLongValue]==3072 && ![paused[@"lastEnqueueNs"] unsignedLongLongValue] && [audio resume:&error]);
    assert(![audio.statistics[@"lastEnqueueNs"] unsignedLongLongValue]);
    for (unsigned i = 0; i < 4; i++) [audio enqueue:pcm frames:1024];
    for (unsigned i = 0; i < 7; i++) {
        assert([stream renderOffline:1024 toBuffer:streamOutput error:&error]==AVAudioEngineManualRenderingStatusSuccess);
        for (unsigned j=0;j<streamOutput.frameLength;j++) assert(streamOutput.floatChannelData[0][j]==.5f && streamOutput.floatChannelData[1][j]==-.25f);
    }
    assert(![audio.statistics[@"queued"] unsignedLongLongValue] && ![audio.statistics[@"dropped"] unsignedLongLongValue]);
    assert([audio.statistics[@"underrunFrames"] isEqual:paused[@"underrunFrames"]]);
    for (unsigned i = 0; i < 9; i++) [audio enqueue:pcm frames:1024];
    assert([audio.statistics[@"queued"] unsignedLongLongValue]==8192 && [audio.statistics[@"dropped"] unsignedLongLongValue]==1024);
    assert([audio.statistics[@"rejectedCapacityFrames"] unsignedLongLongValue]==1024 && ![audio.statistics[@"rejectedInvalidFrames"] unsignedLongLongValue]);
    assert([audio.statistics[@"largestEnqueueBatch"] unsignedLongLongValue]==1024 && [audio.statistics[@"largestEnqueueGapNs"] unsignedLongLongValue]>0);
    assert([audio.statistics[@"largestEnqueueGapSubmitted"] unsignedLongLongValue]-[audio.statistics[@"largestEnqueueGapConsumed"] unsignedLongLongValue]==[audio.statistics[@"largestEnqueueGapQueued"] unsignedLongLongValue]);
    [audio close];

    OriginalAudio *partial=[OriginalAudio new]; assert([partial open:&error]);
    AVAudioEngine *partialStream=[partial valueForKey:@"engine"]; [partialStream stop];
    assert([partialStream enableManualRenderingMode:AVAudioEngineManualRenderingModeOffline format:format maximumFrameCount:1024 error:&error]);
    partialStream.mainMixerNode.outputVolume=1; assert([partialStream startAndReturnError:&error]);
    [partial enqueue:pcm frames:1024]; [partial enqueue:pcm frames:1024]; [partial enqueue:pcm frames:71];
    for (unsigned i=0;i<4;i++) assert([partialStream renderOffline:512 toBuffer:streamOutput error:&error]==AVAudioEngineManualRenderingStatusSuccess);
    NSDictionary *beforePartial=partial.statistics;
    assert([beforePartial[@"queued"] unsignedLongLongValue]==71 && ![beforePartial[@"underrunFrames"] unsignedLongLongValue]);
    assert([partialStream renderOffline:512 toBuffer:streamOutput error:&error]==AVAudioEngineManualRenderingStatusSuccess);
    for (unsigned i=0;i<512;i++) assert(streamOutput.floatChannelData[0][i]==(i<71 ? .5f : 0) && streamOutput.floatChannelData[1][i]==(i<71 ? -.25f : 0));
    NSDictionary *partialShort=partial.statistics;
    assert([partialShort[@"consumed"] unsignedLongLongValue]==[beforePartial[@"consumed"] unsignedLongLongValue]+71 && [partialShort[@"underrunFrames"] unsignedLongLongValue]==441);
    assert([partialShort[@"firstShortFrames"] unsignedLongLongValue]==512 && [partialShort[@"firstShortAvailable"] unsignedLongLongValue]==71);
    assert([partialShort[@"firstShortRead"] isEqual:beforePartial[@"consumed"]] && [partialShort[@"firstShortRendered"] isEqual:beforePartial[@"rendered"]]);
    for (unsigned callback=0;callback<13;callback++) {
        assert([partialStream renderOffline:512 toBuffer:streamOutput error:&error]==AVAudioEngineManualRenderingStatusSuccess);
        for (unsigned i=0;i<512;i++) assert(!streamOutput.floatChannelData[0][i] && !streamOutput.floatChannelData[1][i]);
    }
    NSDictionary *partialSilent=partial.statistics;
    assert([partialSilent[@"underrunFrames"] unsignedLongLongValue]==7097 && [partialSilent[@"consumed"] isEqual:partialShort[@"consumed"]]);
    for (unsigned i=0;i<8;i++) [partial enqueue:pcm frames:1024];
    uint64_t accepted=[partial.statistics[@"submitted"] unsignedLongLongValue];
    [partial enqueue:pcm frames:1024]; [partial enqueue:pcm frames:1024]; [partial enqueue:pcm frames:668];
    NSDictionary *partialFull=partial.statistics;
    assert([partialFull[@"queued"] unsignedLongLongValue]==8192 && [partialFull[@"submitted"] unsignedLongLongValue]==accepted && [partialFull[@"dropped"] unsignedLongLongValue]==2716);
    assert([partialFull[@"rejectedCapacityFrames"] unsignedLongLongValue]==2716 && ![partialFull[@"rejectedInvalidFrames"] unsignedLongLongValue] && ![partialFull[@"rejectedConvertFrames"] unsignedLongLongValue]);
    assert([partialFull[@"firstRejectReason"] unsignedLongLongValue]==AudioRejectCapacity && [partialFull[@"firstRejectFrames"] unsignedLongLongValue]==1024 && [partialFull[@"firstRejectQueued"] unsignedLongLongValue]==8192);
    assert([partialFull[@"firstRejectSubmitted"] unsignedLongLongValue]==accepted && [partialFull[@"firstRejectConsumed"] isEqual:partialSilent[@"consumed"]] && [partialFull[@"firstRejectRendered"] isEqual:partialSilent[@"rendered"]]);
    for (unsigned callback=0;callback<16;callback++) {
        assert([partialStream renderOffline:512 toBuffer:streamOutput error:&error]==AVAudioEngineManualRenderingStatusSuccess);
        for (unsigned i=0;i<512;i++) assert(streamOutput.floatChannelData[0][i]==.5f && streamOutput.floatChannelData[1][i]==-.25f);
    }
    NSDictionary *partialReprimed=partial.statistics;
    assert([partialReprimed[@"firstReprimeAvailable"] unsignedLongLongValue]==8192 && [partialReprimed[@"firstReprimeRead"] isEqual:partialSilent[@"consumed"]] && [partialReprimed[@"firstReprimeRendered"] isEqual:partialSilent[@"rendered"]]);
    assert([partialReprimed[@"reprimeCount"] unsignedLongLongValue]==1 && [partialReprimed[@"underrunFrames"] unsignedLongLongValue]==7097);
    assert([partialReprimed[@"consumed"] unsignedLongLongValue]==accepted && ![partialReprimed[@"queued"] unsignedLongLongValue] && [partialReprimed[@"largestEnqueueBatch"] unsignedLongLongValue]==1024);
    [partial enqueue:NULL frames:71]; [partial enqueue:pcm frames:1025];
    NSDictionary *partialInvalid=partial.statistics;
    assert([partialInvalid[@"submitted"] unsignedLongLongValue]==accepted && [partialInvalid[@"dropped"] unsignedLongLongValue]==2716+71+1025);
    assert([partialInvalid[@"rejectedInvalidFrames"] unsignedLongLongValue]==71+1025 && ![partialInvalid[@"rejectedConvertFrames"] unsignedLongLongValue]);
    NSData *diagnostic=[NSJSONSerialization dataWithJSONObject:@{@"partial":partialShort,@"silence":partialSilent,@"full":partialFull,@"reprimed":partialReprimed,@"invalid":partialInvalid} options:0 error:NULL];
    fprintf(stderr,"Offline partial/reprime diagnostics: %s\n",[[[NSString alloc] initWithData:diagnostic encoding:NSUTF8StringEncoding] UTF8String]);
    [partial close];
    puts("PASS: PCM bounds/extrema, owned stereo samples, exact offline streaming/concurrent ring wraps, 2048-frame reprime boundary, first-shortage diagnostics, pause/resume continuity, partial71/512 with 7097 silence, 2716 controlled capacity rejection and invalid-input diagnostics.");
    return 0;
} }
