// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef HEARTGOLD_NATIVE_MIXER_H
#define HEARTGOLD_NATIVE_MIXER_H
#include "NativeWave.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct HGNativeVoice {
    const int16_t *samples;
    uint64_t position,remainder;
    uint32_t frames,loopStart,period,startup;
    uint8_t loop,active,volume,shift,pan;
} HGNativeVoice;
typedef struct HGNativeMixer {
    uint32_t sampleRate;
    int16_t masterPan;
    uint8_t masterVolume;
    HGNativeVoice voices[16];
} HGNativeMixer;
int HGNativeMixerInit(HGNativeMixer *,uint32_t sampleRate);
// Borrows immutable decoded samples while configured, including stopped voices; replace/reset before freeing them.
int HGNativeMixerSetVoice(HGNativeMixer *,uint32_t channel,const int16_t *,size_t capacity,const HGNativeWaveInfo *);
int HGNativeMixerControl(HGNativeMixer *,uint32_t channels,uint32_t volume,uint32_t shift,uint32_t pan);
int HGNativeMixerTimer(HGNativeMixer *,uint32_t channels,uint32_t timer);
int HGNativeMixerMaster(HGNativeMixer *,uint32_t volume,int32_t pan);
int HGNativeMixerStart(HGNativeMixer *,uint32_t channels);
int HGNativeMixerStop(HGNativeMixer *,uint32_t channels);
uint32_t HGNativeMixerActive(const HGNativeMixer *);
// Owner thread only. Writes interleaved signed stereo; no allocations or callbacks during rendering.
int HGNativeMixerRender(HGNativeMixer *,int16_t *stereo,size_t frames);
#ifdef __cplusplus
}
#endif
#endif
