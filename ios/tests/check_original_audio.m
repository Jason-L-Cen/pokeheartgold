// SPDX-License-Identifier: GPL-3.0-or-later
#import "../OriginalApp/OriginalAudio.m"
#include <assert.h>
#include <stdio.h>

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
    AVAudioEngine *engine = [AVAudioEngine new]; AVAudioPlayerNode *player = [AVAudioPlayerNode new];
    NSError *error;
    [engine attachNode:player]; assert(connectPlayer(engine, player, format, &error));
    if (![engine enableManualRenderingMode:AVAudioEngineManualRenderingModeOffline format:format maximumFrameCount:512 error:&error] || ![engine startAndReturnError:&error]) {
        fprintf(stderr, "Offline audio setup failed: %s\n", error.localizedDescription.UTF8String); return 1;
    }
    [player scheduleBuffer:buffer completionHandler:nil]; assert(playPlayer(player, &error));
    AVAudioPCMBuffer *output = [[AVAudioPCMBuffer alloc] initWithPCMFormat:format frameCapacity:512];
    unsigned nonzero = 0;
    for (unsigned n = 0; n < 8; n++) {
        assert([engine renderOffline:512 toBuffer:output error:&error] == AVAudioEngineManualRenderingStatusSuccess);
        for (unsigned i = 0; i < output.frameLength; i++) {
            float left = output.floatChannelData[0][i], right = output.floatChannelData[1][i];
            if (left || right) { assert(left == .5f && right == -.25f); nonzero++; }
        }
    }
    assert(nonzero == 1024); [player stop]; [engine stop];
    puts("PASS: PCM bounds, signed extrema, independent stereo copies, and exact 48 kHz AVAudioEngine offline output.");
    return 0;
} }
