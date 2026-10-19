// SPDX-License-Identifier: GPL-3.0-or-later
#import <AVFoundation/AVFoundation.h>
#include "NativeMicResampler.h"
#include <math.h>

@interface HGMonoMicConverter : NSObject
@property(nonatomic,strong) AVAudioFormat *sourceFormat;
@property(nonatomic,strong) AVAudioConverter *converter;
@property(nonatomic) uint32_t channels;
@property(nonatomic) BOOL finished;
@end
@implementation HGMonoMicConverter
@end

HGNativeMicResampler *HGNativeMicResamplerCreate(double sourceRate,uint32_t channels,double targetRate) {
    if (!isfinite(sourceRate) || !isfinite(targetRate) || sourceRate<=0 || targetRate<=0 ||
        sourceRate>384000 || targetRate>384000 || !channels || channels>8) return NULL;
    @autoreleasepool { @try {
        HGMonoMicConverter *state=[HGMonoMicConverter new]; state.channels=channels;
        state.sourceFormat=[[AVAudioFormat alloc] initWithCommonFormat:AVAudioPCMFormatFloat32 sampleRate:sourceRate channels:1 interleaved:NO];
        AVAudioFormat *target=[[AVAudioFormat alloc] initWithCommonFormat:AVAudioPCMFormatFloat32 sampleRate:targetRate channels:1 interleaved:NO];
        if (!state.sourceFormat || !target) return NULL;
        state.converter=[[AVAudioConverter alloc] initFromFormat:state.sourceFormat toFormat:target];
        if (!state.converter) return NULL;
        state.converter.primeMethod=AVAudioConverterPrimeMethod_Normal;
        state.converter.sampleRateConverterAlgorithm=AVSampleRateConverterAlgorithm_Mastering;
        state.converter.sampleRateConverterQuality=AVAudioQualityMax;
        return (__bridge_retained HGNativeMicResampler *)state;
    } @catch (NSException *exception) { return NULL; } }
}
static int convert(HGMonoMicConverter *state,AVAudioPCMBuffer *input,BOOL end,HGNativeMicPcmSink sink,void *context) {
    __block BOOL supplied=NO;
    // shortcut: conversion buffers allocate per push; use a pool before sustained realtime capture.
    AVAudioPCMBuffer *output=[[AVAudioPCMBuffer alloc] initWithPCMFormat:state.converter.outputFormat frameCapacity:4096];
    if (!output) return 0;
    for (;;) {
        output.frameLength=0; NSError *error=nil;
        AVAudioConverterOutputStatus status=[state.converter convertToBuffer:output error:&error withInputFromBlock:^AVAudioBuffer *(AVAudioPacketCount packets,AVAudioConverterInputStatus *outStatus) {
            if (input && !supplied) { supplied=YES; *outStatus=AVAudioConverterInputStatus_HaveData; return input; }
            *outStatus=end ? AVAudioConverterInputStatus_EndOfStream : AVAudioConverterInputStatus_NoDataNow;
            return nil;
        }];
        if (error || status==AVAudioConverterOutputStatus_Error) return 0;
        if (output.frameLength) sink(context,output.floatChannelData[0],output.frameLength);
        if (status==AVAudioConverterOutputStatus_InputRanDry || status==AVAudioConverterOutputStatus_EndOfStream) return 1;
        if (!output.frameLength) return 0;
    }
}
int HGNativeMicResamplerPush(HGNativeMicResampler *opaque,const float *const *channels,uint32_t frames,HGNativeMicPcmSink sink,void *context) {
    if (!opaque || !sink || !channels || !frames || frames>1048576) return 0;
    HGMonoMicConverter *state=(__bridge HGMonoMicConverter *)opaque;
    if (state.finished) return 0;
    for (uint32_t channel=0;channel<state.channels;channel++) {
        if (!channels[channel] || (uintptr_t)channels[channel]%_Alignof(float)) return 0;
        for (uint32_t frame=0;frame<frames;frame++) if (!isfinite(channels[channel][frame])) return 0;
    }
    @autoreleasepool { @try {
        AVAudioPCMBuffer *input=[[AVAudioPCMBuffer alloc] initWithPCMFormat:state.sourceFormat frameCapacity:frames];
        if (!input) return 0; input.frameLength=frames;
        for (uint32_t frame=0;frame<frames;frame++) {
            double total=0;
            for (uint32_t channel=0;channel<state.channels;channel++) total+=fminf(1,fmaxf(-1,channels[channel][frame]));
            input.floatChannelData[0][frame]=(float)(total/state.channels);
        }
        return convert(state,input,NO,sink,context);
    } @catch (NSException *exception) { return 0; } }
}
int HGNativeMicResamplerFinish(HGNativeMicResampler *opaque,HGNativeMicPcmSink sink,void *context) {
    if (!opaque || !sink) return 0; HGMonoMicConverter *state=(__bridge HGMonoMicConverter *)opaque;
    if (state.finished) return 0;
    @autoreleasepool { @try { state.finished=YES; return convert(state,nil,YES,sink,context); }
        @catch (NSException *exception) { return 0; } }
}
void HGNativeMicResamplerDestroy(HGNativeMicResampler *opaque) {
    if (opaque) { HGMonoMicConverter *state=(__bridge_transfer HGMonoMicConverter *)opaque; (void)state; }
}
