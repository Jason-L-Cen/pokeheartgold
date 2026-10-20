// SPDX-License-Identifier: GPL-3.0-or-later
#include "NativeWave.h"
#include <string.h>
#include <limits.h>

static uint16_t read16(const uint8_t *p) { return (uint16_t)(p[0]|(uint16_t)p[1]<<8); }
static uint32_t read32(const uint8_t *p) { return (uint32_t)read16(p)|(uint32_t)read16(p+2)<<16; }
static int signed16(const uint8_t *p) { unsigned n=read16(p); return n>=32768 ? (int)n-65536 : (int)n; }
static const int steps[89]={
    7,8,9,10,11,12,13,14,16,17,19,21,23,25,28,31,34,37,41,45,50,55,60,66,73,80,88,97,107,118,
    130,143,157,173,190,209,230,253,279,307,337,371,408,449,494,544,598,658,724,796,876,963,
    1060,1166,1282,1411,1552,1707,1878,2066,2272,2499,2749,3024,3327,3660,4026,4428,4871,
    5358,5894,6484,7132,7845,8630,9493,10442,11487,12635,13899,15289,16818,18500,20350,
    22385,24623,27086,29794,32767};
static const int indexChange[8]={-1,-1,-1,-1,2,4,6,8};

int HGNativeWaveGetInfo(const void *encoded,size_t bytes,HGNativeWaveInfo *info) {
    if (!encoded || !info || bytes<12) return 0;
    const uint8_t *p=encoded;
    HGNativeWaveInfo value={0,0,read16(p+2),read16(p+4),p[0],p[1]};
    uint32_t loopWords=read16(p+6),tailWords=read32(p+8);
    uint64_t length=((uint64_t)loopWords+tailWords)*4,frames=0,start=0;
    if (value.format>2 || value.loop>1 || length>bytes-12) return 0;
    if (!value.format) { frames=length; start=(uint64_t)loopWords*4; }
    else if (value.format==1) { frames=length/2; start=(uint64_t)loopWords*2; }
    else {
        if (length<4 || (p[14]&127)>88) return 0;
        frames=(length-4)*2; start=loopWords ? ((uint64_t)loopWords-1)*8 : 0;
        if (value.loop && !loopWords) return 0;
    }
    if (frames>UINT32_MAX || start>frames || (value.loop && start>=frames)) return 0;
    value.frames=(uint32_t)frames; value.loopStart=(uint32_t)start;
    *info=value; return 1;
}
int HGNativeWaveDecode(const void *encoded,size_t bytes,int16_t *output,size_t capacity,HGNativeWaveInfo *info) {
    HGNativeWaveInfo value;
    if (!HGNativeWaveGetInfo(encoded,bytes,&value) || !info || capacity<value.frames ||
        (value.frames && (!output || (uintptr_t)output%_Alignof(int16_t)))) return 0;
    size_t size=(size_t)value.frames*sizeof(*output);
    if (value.frames && (size/sizeof(*output)!=value.frames ||
        (uintptr_t)output>UINTPTR_MAX-size || (uintptr_t)encoded>UINTPTR_MAX-bytes ||
        ((uintptr_t)encoded<(uintptr_t)output+size && (uintptr_t)output<(uintptr_t)encoded+bytes))) return 0;
    const uint8_t *data=(const uint8_t *)encoded+12;
    if (!value.format) {
        for (uint32_t i=0;i<value.frames;i++) output[i]=(int16_t)((data[i]>=128 ? (int)data[i]-256 : data[i])*256);
    } else if (value.format==1) {
        for (uint32_t i=0;i<value.frames;i++) output[i]=(int16_t)signed16(data+2*i);
    } else {
        int predictor=signed16(data),index=data[2]&127;
        for (uint32_t i=0;i<value.frames;i++) {
            unsigned nibble=(data[4+i/2]>>((i&1)*4))&15;
            // DS rounding uses one combined product, unlike separately rounded generic IMA shifts.
            int delta=((int)(nibble&7)*2+1)*steps[index]/8;
            predictor+=(nibble&8) ? -delta : delta;
            if (predictor<-32768) predictor=-32768;
            if (predictor>32767) predictor=32767;
            index+=indexChange[nibble&7]; if (index<0) index=0; if (index>88) index=88;
            output[i]=(int16_t)predictor;
        }
    }
    *info=value; return 1;
}
