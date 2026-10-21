// SPDX-License-Identifier: GPL-3.0-or-later
#import "OriginalAudio.h"
#import <AVFAudio/AVFAudio.h>
#include <stdatomic.h>
#include <TargetConditionals.h>

static BOOL connectPlayer(AVAudioEngine *engine, AVAudioPlayerNode *player, AVAudioFormat *format, NSError **error) {
    if (@available(iOS 27.0, macOS 27.0, *)) return [engine connect:player to:engine.mainMixerNode format:format error:error];
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
    [engine connect:player to:engine.mainMixerNode format:format]; return YES;
#pragma clang diagnostic pop
}
static BOOL playPlayer(AVAudioPlayerNode *player, NSError **error) {
    if (@available(iOS 27.0, macOS 27.0, *)) return [player playAndReturnError:error];
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
    [player play]; return YES;
#pragma clang diagnostic pop
}

static AVAudioPCMBuffer *convertPCM(const int16_t *stereo, size_t frames, AVAudioFormat *format) {
    if (!stereo || !frames || frames > 1024) return nil;
    AVAudioPCMBuffer *buffer = [[AVAudioPCMBuffer alloc] initWithPCMFormat:format frameCapacity:(AVAudioFrameCount)frames];
    if (!buffer || !buffer.floatChannelData) return nil;
    buffer.frameLength = (AVAudioFrameCount)frames;
    for (size_t i = 0; i < frames; i++) {
        buffer.floatChannelData[0][i] = stereo[i * 2] / 32768.0f;
        buffer.floatChannelData[1][i] = stereo[i * 2 + 1] / 32768.0f;
    }
    return buffer;
}

@implementation OriginalAudio {
    AVAudioEngine *_engine;
    AVAudioPlayerNode *_player;
    AVAudioFormat *_format;
    NSString *_failure;
    BOOL _playing, _closed, _tapInstalled, _muted;
    _Atomic(uint64_t) _queued, _submitted, _consumed, _nonzero, _dropped, _rendered, _renderedNonzero;
}
- (BOOL)open:(NSError **)error {
#if TARGET_OS_IPHONE
    AVAudioSession *session = AVAudioSession.sharedInstance;
    if (![session setCategory:AVAudioSessionCategoryAmbient error:error] || ![session setActive:YES error:error]) return NO;
#endif
    _format = [[AVAudioFormat alloc] initStandardFormatWithSampleRate:48000 channels:2];
    _engine = [AVAudioEngine new]; _player = [AVAudioPlayerNode new];
    _muted = [NSProcessInfo.processInfo.environment[@"HEARTGOLD_TEST_MUTE"] boolValue];
#ifdef HEARTGOLD_ORIGINAL_APP_CHECK
    _muted = YES;
#endif
    if (_muted) _engine.mainMixerNode.outputVolume = 0;
    [_engine attachNode:_player];
    if (!connectPlayer(_engine, _player, _format, error)) return NO;
    AVAudioNodeTapBlock tap = ^(AVAudioPCMBuffer *buffer, AVAudioTime *time) {
        atomic_fetch_add(&self->_rendered, buffer.frameLength);
        if (buffer.format.commonFormat == AVAudioPCMFormatFloat32 && buffer.floatChannelData) {
            uint64_t nonzero = 0;
            for (AVAudioChannelCount c = 0; c < buffer.format.channelCount; c++)
                for (AVAudioFrameCount i = 0; i < buffer.frameLength; i++) nonzero += buffer.floatChannelData[c][i] != 0;
            atomic_fetch_add(&self->_renderedNonzero, nonzero);
        }
    };
    // Observe the player's PCM before the final mixer, so silent tests still verify music generation.
    double outputRate = [_player outputFormatForBus:0].sampleRate;
    AVAudioFrameCount tapFrames = (AVAudioFrameCount)((outputRate > 0 ? outputRate : 48000) * 0.1);
    if (@available(iOS 27.0, macOS 27.0, *)) {
        if (![_player installTapOnBus:0 bufferSize:tapFrames format:nil error:error block:tap]) return NO;
    } else {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
        [_player installTapOnBus:0 bufferSize:tapFrames format:nil block:tap];
#pragma clang diagnostic pop
    }
    _tapInstalled = YES;
    return [_engine startAndReturnError:error];
}
- (BOOL)resume:(NSError **)error {
    if (_closed || !_engine) return NO;
#if TARGET_OS_IPHONE
    if (![AVAudioSession.sharedInstance setActive:YES error:error]) return NO;
#endif
    if (!_engine.running && ![_engine startAndReturnError:error]) return NO;
    if (_playing && !playPlayer(_player, error)) return NO;
    return YES;
}
- (void)pause { [_player pause]; [_engine pause]; }
- (void)close {
    if (_closed) return;
    _closed = YES; [_player stop]; [_engine stop];
    if (_tapInstalled) [_player removeTapOnBus:0];
    _player = nil; _engine = nil;
#if TARGET_OS_IPHONE
    [AVAudioSession.sharedInstance setActive:NO withOptions:AVAudioSessionSetActiveOptionNotifyOthersOnDeactivation error:nil];
#endif
}
- (void)enqueue:(const int16_t *)stereo frames:(size_t)frames {
    if (_closed) return;
    if (!_engine.running) {
        NSError *error;
        if (![self resume:&error]) { _failure = error.localizedDescription ?: @"声音输出设备无法恢复。"; return; }
    }
    // shortcut: bound wall-clock/device-clock drift to 8192 queued frames; calibrate long sessions on hardware before adding adaptive resampling.
    if (frames > 8192 || atomic_load(&_queued) > 8192 - frames) { atomic_fetch_add(&_dropped, frames); return; }
    AVAudioPCMBuffer *buffer = convertPCM(stereo, frames, _format);
    if (!buffer) { atomic_fetch_add(&_dropped, frames); return; }
    uint64_t nonzero = 0;
    for (size_t i = 0; i < frames * 2; i++) nonzero += stereo[i] != 0;
    atomic_fetch_add(&_nonzero, nonzero);
    atomic_fetch_add(&_submitted, frames); atomic_fetch_add(&_queued, frames);
    [_player scheduleBuffer:buffer completionCallbackType:AVAudioPlayerNodeCompletionDataConsumed completionHandler:^(AVAudioPlayerNodeCompletionCallbackType type) {
        atomic_fetch_sub(&self->_queued, frames);
        atomic_fetch_add(&self->_consumed, frames);
    }];
    // Keep a short lead so irregular game command batches do not immediately starve the device.
    if (!_playing && atomic_load(&_queued) >= 1024) {
        NSError *error;
        _playing = playPlayer(_player, &error);
        if (!_playing) _failure = error.localizedDescription ?: @"无法开始声音播放。";
    }
}
- (NSString *)failure { return _failure; }
- (NSDictionary<NSString *, NSNumber *> *)statistics {
    return @{ @"muted": @(_muted && (!_engine || _engine.mainMixerNode.outputVolume == 0)),
              @"queued": @(atomic_load(&_queued)), @"submitted": @(atomic_load(&_submitted)),
              @"consumed": @(atomic_load(&_consumed)), @"nonzeroSamples": @(atomic_load(&_nonzero)),
              @"dropped": @(atomic_load(&_dropped)), @"rendered": @(atomic_load(&_rendered)),
              @"renderedNonzeroSamples": @(atomic_load(&_renderedNonzero)) };
}
@end
