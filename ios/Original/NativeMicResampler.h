// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef HEARTGOLD_NATIVE_MIC_RESAMPLER_H
#define HEARTGOLD_NATIVE_MIC_RESAMPLER_H
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
typedef struct HGNativeMicResampler HGNativeMicResampler;
typedef void (*HGNativeMicPcmSink)(void *,const float *,uint32_t);
HGNativeMicResampler *HGNativeMicResamplerCreate(double sourceRate,uint32_t channels,double targetRate);
// Serialized owner calls; planar Float32 chunks contain 1..1048576 frames per channel.
// Sink data is valid only during the synchronous call; discard the stream on failure.
int HGNativeMicResamplerPush(HGNativeMicResampler *,const float *const *,uint32_t,HGNativeMicPcmSink,void *);
int HGNativeMicResamplerFinish(HGNativeMicResampler *,HGNativeMicPcmSink,void *);
void HGNativeMicResamplerDestroy(HGNativeMicResampler *);
#ifdef __cplusplus
}
#endif
#endif
