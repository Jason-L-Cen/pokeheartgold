// SPDX-License-Identifier: GPL-3.0-or-later
#import "OriginalAudio.h"
#import <AVFAudio/AVFAudio.h>
#include <stdatomic.h>
#include <TargetConditionals.h>
#ifdef HEARTGOLD_ORIGINAL_APP_CHECK
#include <time.h>
enum { AudioRejectCapacity = 1, AudioRejectInvalid, AudioRejectConvert };
static uint64_t checkMonotonicNs(void) {
    struct timespec now;
    return clock_gettime(CLOCK_MONOTONIC, &now) ? 0 : (uint64_t)now.tv_sec * 1000000000 + now.tv_nsec;
}
#endif

static BOOL connectNode(AVAudioEngine *engine, AVAudioNode *node, AVAudioFormat *format, NSError **error) {
    if (@available(iOS 27.0, macOS 27.0, *)) return [engine connect:node to:engine.mainMixerNode format:format error:error];
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
    [engine connect:node to:engine.mainMixerNode format:format]; return YES;
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
    AVAudioSourceNode *_source;
    AVAudioFormat *_format;
    NSString *_failure;
    BOOL _primed, _started, _closed, _muted;
    float _pcm[8192 * 2];
    _Atomic(BOOL) _invalidOutput;
    _Atomic(uint64_t) _submitted, _consumed, _nonzero, _dropped, _underrun, _rendered, _renderedNonzero;
#ifdef HEARTGOLD_ORIGINAL_APP_CHECK
    _Atomic(uint64_t) _firstShortFrames, _firstShortAvailable, _firstShortRead, _firstShortRendered, _largestCallback;
    _Atomic(uint64_t) _rejectedCapacityFrames, _rejectedInvalidFrames, _rejectedConvertFrames, _largestEnqueueBatch;
    _Atomic(uint64_t) _firstRejectReason, _firstRejectFrames, _firstRejectQueued, _firstRejectSubmitted, _firstRejectConsumed, _firstRejectRendered;
    _Atomic(uint64_t) _lastEnqueueNs, _largestEnqueueGapGeneration, _largestEnqueueGapNs, _largestEnqueueGapEndNs, _largestEnqueueGapSubmitted, _largestEnqueueGapConsumed, _largestEnqueueGapRendered, _largestEnqueueGapQueued;
    _Atomic(uint64_t) _reprimeCount, _firstReprimeAvailable, _firstReprimeRead, _firstReprimeRendered;
#endif
}
- (BOOL)open:(NSError **)error {
#ifdef HEARTGOLD_ORIGINAL_APP_CHECK
    atomic_store(&_lastEnqueueNs, 0);
#endif
#if TARGET_OS_IPHONE
    AVAudioSession *session = AVAudioSession.sharedInstance;
    if (![session setCategory:AVAudioSessionCategoryAmbient error:error] || ![session setActive:YES error:error]) return NO;
#endif
    _format = [[AVAudioFormat alloc] initStandardFormatWithSampleRate:48000 channels:2];
    _engine = [AVAudioEngine new];
    _muted = [NSProcessInfo.processInfo.environment[@"HEARTGOLD_TEST_MUTE"] boolValue];
#ifdef HEARTGOLD_ORIGINAL_APP_CHECK
    _muted = YES;
#endif
    if (_muted) _engine.mainMixerNode.outputVolume = 0;
    _source = [[AVAudioSourceNode alloc] initWithFormat:_format renderBlock:^OSStatus(BOOL *isSilence, const AudioTimeStamp *time, AVAudioFrameCount frames, AudioBufferList *output) {
        (void)time;
        if (!frames) { *isSilence = YES; return noErr; }
        if (output->mNumberBuffers != 2 || output->mBuffers[0].mNumberChannels != 1 || output->mBuffers[1].mNumberChannels != 1 ||
            !output->mBuffers[0].mData || !output->mBuffers[1].mData ||
            output->mBuffers[0].mDataByteSize / sizeof(float) < frames || output->mBuffers[1].mDataByteSize / sizeof(float) < frames) {
            atomic_store(&self->_invalidOutput, YES); *isSilence = YES; return kAudio_ParamError;
        }
        uint64_t read = atomic_load_explicit(&self->_consumed, memory_order_relaxed);
        uint64_t available = atomic_load_explicit(&self->_submitted, memory_order_acquire) - read;
#ifdef HEARTGOLD_ORIGINAL_APP_CHECK
        if (frames>atomic_load(&self->_largestCallback)) atomic_store(&self->_largestCallback,frames);
#endif
        if (!self->_primed && available >= 2048) {
#ifdef HEARTGOLD_ORIGINAL_APP_CHECK
            if (self->_started) {
                atomic_fetch_add(&self->_reprimeCount, 1);
                if (!atomic_load(&self->_firstReprimeAvailable)) {
                    atomic_store(&self->_firstReprimeRead, read);
                    atomic_store(&self->_firstReprimeRendered, atomic_load(&self->_rendered));
                    atomic_store(&self->_firstReprimeAvailable, available);
                }
            }
#endif
            self->_started = self->_primed = YES;
        }
        AVAudioFrameCount count = self->_primed ? (AVAudioFrameCount)MIN(available, frames) : 0;
        uint64_t nonzero = 0;
        for (unsigned c = 0; c < 2; c++) {
            float *channel = output->mBuffers[c].mData;
            for (AVAudioFrameCount i = 0; i < frames; i++) {
                float sample = i < count ? self->_pcm[((read + i) & 8191) * 2 + c] : 0;
                channel[i] = sample; nonzero += sample != 0;
            }
        }
        atomic_store_explicit(&self->_consumed, read + count, memory_order_release);
        if (self->_started) {
#ifdef HEARTGOLD_ORIGINAL_APP_CHECK
            if (count<frames && !atomic_load(&self->_underrun)) {
                atomic_store(&self->_firstShortFrames,frames); atomic_store(&self->_firstShortAvailable,available);
                atomic_store(&self->_firstShortRead,read); atomic_store(&self->_firstShortRendered,atomic_load(&self->_rendered));
            }
#endif
            atomic_fetch_add(&self->_underrun, frames - count);
        }
        if (count < frames) self->_primed = NO;
        atomic_fetch_add(&self->_rendered, frames); atomic_fetch_add(&self->_renderedNonzero, nonzero);
        *isSilence = !nonzero; return noErr;
    }];
    [_engine attachNode:_source];
    if (!connectNode(_engine, _source, _format, error)) return NO;
    return [_engine startAndReturnError:error];
}
- (BOOL)resume:(NSError **)error {
#ifdef HEARTGOLD_ORIGINAL_APP_CHECK
    atomic_store(&_lastEnqueueNs, 0);
#endif
    if (_closed || !_engine) return NO;
#if TARGET_OS_IPHONE
    if (![AVAudioSession.sharedInstance setActive:YES error:error]) return NO;
#endif
    if (!_engine.running && ![_engine startAndReturnError:error]) return NO;
    return YES;
}
- (void)pause {
#ifdef HEARTGOLD_ORIGINAL_APP_CHECK
    atomic_store(&_lastEnqueueNs, 0);
#endif
    [_engine pause];
}
- (void)close {
    if (_closed) return;
    _closed = YES; [_engine stop];
    _source = nil; _engine = nil;
#if TARGET_OS_IPHONE
    [AVAudioSession.sharedInstance setActive:NO withOptions:AVAudioSessionSetActiveOptionNotifyOthersOnDeactivation error:nil];
#endif
}
#ifdef HEARTGOLD_ORIGINAL_APP_CHECK
- (void)checkReject:(uint64_t)reason frames:(uint64_t)frames submitted:(uint64_t)submitted consumed:(uint64_t)consumed {
    if (reason == AudioRejectCapacity) atomic_fetch_add(&_rejectedCapacityFrames, frames);
    else if (reason == AudioRejectInvalid) atomic_fetch_add(&_rejectedInvalidFrames, frames);
    else atomic_fetch_add(&_rejectedConvertFrames, frames);
    if (!atomic_load(&_firstRejectReason)) {
        atomic_store(&_firstRejectFrames, frames); atomic_store(&_firstRejectQueued, submitted - consumed);
        atomic_store(&_firstRejectSubmitted, submitted); atomic_store(&_firstRejectConsumed, consumed);
        atomic_store(&_firstRejectRendered, atomic_load(&_rendered));
        atomic_store(&_firstRejectReason, reason);
    }
}
#endif
- (void)enqueue:(const int16_t *)stereo frames:(size_t)frames {
    if (_closed) return;
    if (!_engine.running) {
        NSError *error;
        if (![self resume:&error]) { _failure = error.localizedDescription ?: @"声音输出设备无法恢复。"; return; }
    }
    uint64_t submitted = atomic_load_explicit(&_submitted, memory_order_relaxed);
    uint64_t queued = submitted - atomic_load_explicit(&_consumed, memory_order_acquire);
#ifdef HEARTGOLD_ORIGINAL_APP_CHECK
    if (frames > atomic_load(&_largestEnqueueBatch)) atomic_store(&_largestEnqueueBatch, frames);
    uint64_t now = checkMonotonicNs(), previous = atomic_exchange(&_lastEnqueueNs, now);
    if (previous && now >= previous && now - previous > atomic_load(&_largestEnqueueGapNs)) {
        atomic_fetch_add(&_largestEnqueueGapGeneration, 1);
        atomic_store(&_largestEnqueueGapSubmitted, submitted); atomic_store(&_largestEnqueueGapConsumed, submitted - queued);
        atomic_store(&_largestEnqueueGapRendered, atomic_load(&_rendered)); atomic_store(&_largestEnqueueGapQueued, queued);
        atomic_store(&_largestEnqueueGapEndNs, now);
        atomic_store(&_largestEnqueueGapNs, now - previous);
        atomic_fetch_add(&_largestEnqueueGapGeneration, 1);
    }
#endif
    // shortcut: bound device-clock drift to 8192 actual frames; calibrate long hardware sessions before adaptive resampling.
    if (frames > 8192 || queued > 8192 - frames) {
#ifdef HEARTGOLD_ORIGINAL_APP_CHECK
        [self checkReject:AudioRejectCapacity frames:frames submitted:submitted consumed:submitted - queued];
#endif
        atomic_fetch_add(&_dropped, frames); return;
    }
    AVAudioPCMBuffer *buffer = convertPCM(stereo, frames, _format);
    if (!buffer) {
#ifdef HEARTGOLD_ORIGINAL_APP_CHECK
        [self checkReject:((!stereo || !frames || frames > 1024) ? AudioRejectInvalid : AudioRejectConvert) frames:frames submitted:submitted consumed:submitted - queued];
#endif
        atomic_fetch_add(&_dropped, frames); return;
    }
    uint64_t nonzero = 0;
    for (size_t i = 0; i < frames; i++) for (unsigned c = 0; c < 2; c++) {
        float sample = buffer.floatChannelData[c][i];
        _pcm[((submitted + i) & 8191) * 2 + c] = sample; nonzero += sample != 0;
    }
    atomic_fetch_add(&_nonzero, nonzero);
    atomic_store_explicit(&_submitted, submitted + frames, memory_order_release);
}
- (NSString *)failure { return atomic_load(&_invalidOutput) ? @"声音输出缓冲区格式异常。" : _failure; }
- (NSDictionary<NSString *, NSNumber *> *)statistics {
    uint64_t submitted = atomic_load(&_submitted), consumed = atomic_load(&_consumed);
#ifdef HEARTGOLD_ORIGINAL_APP_CHECK
    uint64_t firstRejectReason = atomic_load(&_firstRejectReason), firstReprimeAvailable = atomic_load(&_firstReprimeAvailable);
    uint64_t gapNs=0, gapEndNs=0, gapSubmitted=0, gapConsumed=0, gapRendered=0, gapQueued=0;
    BOOL gapConsistent=NO;
    // A bounded read never blocks a checkpoint on a preempted producer.
    for (unsigned attempt=0;attempt<8;attempt++) {
        uint64_t generation=atomic_load(&_largestEnqueueGapGeneration);
        if (generation&1) continue;
        gapNs=atomic_load(&_largestEnqueueGapNs); gapEndNs=atomic_load(&_largestEnqueueGapEndNs);
        gapSubmitted=atomic_load(&_largestEnqueueGapSubmitted); gapConsumed=atomic_load(&_largestEnqueueGapConsumed);
        gapRendered=atomic_load(&_largestEnqueueGapRendered); gapQueued=atomic_load(&_largestEnqueueGapQueued);
        if (generation==atomic_load(&_largestEnqueueGapGeneration)) { gapConsistent=YES; break; }
    }
#endif
    return @{ @"muted": @(_muted && (!_engine || _engine.mainMixerNode.outputVolume == 0)),
              @"queued": @(submitted > consumed ? submitted - consumed : 0), @"submitted": @(submitted),
              @"consumed": @(consumed), @"nonzeroSamples": @(atomic_load(&_nonzero)),
              @"dropped": @(atomic_load(&_dropped)), @"underrunFrames": @(atomic_load(&_underrun)), @"rendered": @(atomic_load(&_rendered)),
#ifdef HEARTGOLD_ORIGINAL_APP_CHECK
              @"firstShortFrames":@(atomic_load(&_firstShortFrames)), @"firstShortAvailable":@(atomic_load(&_firstShortAvailable)),
              @"firstShortRead":@(atomic_load(&_firstShortRead)), @"firstShortRendered":@(atomic_load(&_firstShortRendered)), @"largestCallback":@(atomic_load(&_largestCallback)),
              @"rejectedCapacityFrames":@(atomic_load(&_rejectedCapacityFrames)), @"rejectedInvalidFrames":@(atomic_load(&_rejectedInvalidFrames)), @"rejectedConvertFrames":@(atomic_load(&_rejectedConvertFrames)),
              @"firstRejectReason":@(firstRejectReason), @"firstRejectFrames":@(firstRejectReason ? atomic_load(&_firstRejectFrames) : 0),
              @"firstRejectQueued":@(firstRejectReason ? atomic_load(&_firstRejectQueued) : 0), @"firstRejectSubmitted":@(firstRejectReason ? atomic_load(&_firstRejectSubmitted) : 0),
              @"firstRejectConsumed":@(firstRejectReason ? atomic_load(&_firstRejectConsumed) : 0), @"firstRejectRendered":@(firstRejectReason ? atomic_load(&_firstRejectRendered) : 0),
              @"largestEnqueueBatch":@(atomic_load(&_largestEnqueueBatch)), @"lastEnqueueNs":@(atomic_load(&_lastEnqueueNs)),
              @"largestEnqueueGapConsistent":@(gapConsistent), @"largestEnqueueGapNs":@(gapNs), @"largestEnqueueGapEndNs":@(gapEndNs), @"largestEnqueueGapSubmitted":@(gapSubmitted),
              @"largestEnqueueGapConsumed":@(gapConsumed), @"largestEnqueueGapRendered":@(gapRendered), @"largestEnqueueGapQueued":@(gapQueued),
              @"reprimeCount":@(atomic_load(&_reprimeCount)), @"firstReprimeAvailable":@(firstReprimeAvailable),
              @"firstReprimeRead":@(firstReprimeAvailable ? atomic_load(&_firstReprimeRead) : 0), @"firstReprimeRendered":@(firstReprimeAvailable ? atomic_load(&_firstReprimeRendered) : 0),
#endif
              @"renderedNonzeroSamples": @(atomic_load(&_renderedNonzero)) };
}
@end
