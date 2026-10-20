// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef HEARTGOLD_NATIVE_WAVE_H
#define HEARTGOLD_NATIVE_WAVE_H
#include <stdint.h>
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
typedef struct HGNativeWaveInfo {
    uint32_t frames,loopStart;
    uint16_t sampleRate,timer;
    uint8_t format,loop;
} HGNativeWaveInfo;
// Input begins at the original 12-byte SNDWaveParam. Failure leaves output/info untouched.
int HGNativeWaveGetInfo(const void *encoded,size_t bytes,HGNativeWaveInfo *info);
int HGNativeWaveDecode(const void *encoded,size_t bytes,int16_t *output,size_t capacity,HGNativeWaveInfo *info);
#ifdef __cplusplus
}
#endif
#endif
