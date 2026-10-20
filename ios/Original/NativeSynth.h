// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef HEARTGOLD_NATIVE_SYNTH_H
#define HEARTGOLD_NATIVE_SYNTH_H
#include "NativeSequence.h"
#include "NativeMixer.h"
#include "NativeInstrument.h"
#ifdef __cplusplus
extern "C" {
#endif
enum { HG_SYNTH_FREE,HG_SYNTH_START,HG_SYNTH_ATTACK,HG_SYNTH_DECAY,HG_SYNTH_SUSTAIN,HG_SYNTH_RELEASE };
typedef struct HGSynthVoice {
    struct HGDecodedWave *wave;
    int32_t amplitude,remaining,sweep;
    uint32_t sweepLength,sweepCount;
    uint16_t timer,decay,release,modDelayCount,modCounter;
    int16_t velocity;
    int8_t track,pan;
    uint8_t state,priority,key,root,attack,sustain,manualSweep,type;
} HGSynthVoice;
typedef struct HGNativeSynth {
    HGNativeSequence sequence;
    HGNativeMixer mixer;
    const uint8_t *memory;
    size_t memoryBytes;
    struct HGDecodedWave *cache;
    HGSynthVoice voices[16];
    int16_t noise[32767],duty[8][8];
    uint64_t phase;
    uint32_t locked,error;
} HGNativeSynth;
// Initialize fresh/closed state. Borrowed engine memory must fit the tagged 31-bit wave address space.
int HGNativeSynthInit(HGNativeSynth *,uint32_t rate,const void *memory,size_t bytes);
void HGNativeSynthClose(HGNativeSynth *);
// Detach a sequence voice before configuring the same mixer slot directly.
void HGNativeSynthDetach(HGNativeSynth *,unsigned channel);
int HGNativeSynthPause(HGNativeSynth *,unsigned player,int paused);
int HGNativeSynthInvalidate(HGNativeSynth *,uint32_t first,uint32_t end,unsigned kind);
// Owner thread only; first-use decoding allocates. Buffer PCM before an audio device callback.
int HGNativeSynthRender(HGNativeSynth *,int16_t *stereo,size_t frames);
uint16_t HGNativeSynthTimer(uint16_t base,int32_t pitch);
#ifdef __cplusplus
}
#endif
#endif
