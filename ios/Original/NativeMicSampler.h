// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef HEARTGOLD_NATIVE_MIC_SAMPLER_H
#define HEARTGOLD_NATIVE_MIC_SAMPLER_H
#include <nitro/types.h>
#include <nitro/spi/ARM9/mic.h>
#ifdef __cplusplus
extern "C" {
#endif
typedef struct HGNativeMicSampler {
    u8 *buffer;
    u32 size,offset,period,flags,lastOffset;
    BOOL running,published;
    u16 partial,last;
    s32 bias8,bias12;
    u32 sum8,count8,sum12,count12;
} HGNativeMicSampler;
void HGNativeMicSamplerInit(HGNativeMicSampler *);
MICResult HGNativeMicSamplerStart(HGNativeMicSampler *,const MICAutoParam *);
// Input is normalized mono PCM at the configured microphone rate; NaN/Inf denotes unavailable input.
MICResult HGNativeMicSamplerFeed(HGNativeMicSampler *,const float *,u32,u32 *completedBuffers);
void HGNativeMicSamplerStop(HGNativeMicSampler *);
void *HGNativeMicSamplerLast(const HGNativeMicSampler *);
#ifdef __cplusplus
}
#endif
#endif
