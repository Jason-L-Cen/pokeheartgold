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
    assert([paused[@"queued"] unsignedLongLongValue]==3072 && [audio resume:&error]);
    for (unsigned i = 0; i < 4; i++) [audio enqueue:pcm frames:1024];
    for (unsigned i = 0; i < 7; i++) {
        assert([stream renderOffline:1024 toBuffer:streamOutput error:&error]==AVAudioEngineManualRenderingStatusSuccess);
        for (unsigned j=0;j<streamOutput.frameLength;j++) assert(streamOutput.floatChannelData[0][j]==.5f && streamOutput.floatChannelData[1][j]==-.25f);
    }
    assert(![audio.statistics[@"queued"] unsignedLongLongValue] && ![audio.statistics[@"dropped"] unsignedLongLongValue]);
    assert([audio.statistics[@"underrunFrames"] isEqual:paused[@"underrunFrames"]]);
    for (unsigned i = 0; i < 9; i++) [audio enqueue:pcm frames:1024];
    assert([audio.statistics[@"queued"] unsignedLongLongValue]==8192 && [audio.statistics[@"dropped"] unsignedLongLongValue]==1024);
    [audio close];
    puts("PASS: PCM bounds/extrema, owned stereo samples, exact offline streaming/concurrent ring wraps, 2048-frame reprime boundary, first-shortage diagnostics, pause/resume continuity, and real overflow bounds.");
    return 0;
} }
