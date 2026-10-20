// SPDX-License-Identifier: GPL-3.0-or-later
#include "NativeMixer.h"
#include <string.h>

static int valid(HGNativeMixer *m,uint32_t mask) { return m && m->sampleRate && !(mask&~0xFFFFu); }
static int floorDiv(int value,int divisor) { return value<0 ? -((-value+divisor-1)/divisor) : value/divisor; }
static int gain(int value,unsigned volume) { return volume==127 ? value : floorDiv(value*(int)volume,128); }
static int16_t clip(int value) { return (int16_t)(value<-32768 ? -32768 : value>32767 ? 32767 : value); }
int HGNativeMixerInit(HGNativeMixer *m,uint32_t rate) {
    if (!m || !rate) return 0;
    memset(m,0,sizeof(*m)); m->sampleRate=rate; m->masterVolume=127; m->masterPan=-1; return 1;
}
int HGNativeMixerSetVoice(HGNativeMixer *m,uint32_t channel,const int16_t *samples,size_t capacity,const HGNativeWaveInfo *info) {
    if (!valid(m,0) || channel>=16 || !info || capacity<info->frames ||
        (info->frames && (!samples || (uintptr_t)samples%_Alignof(int16_t))) ||
        info->loop>1 || info->loopStart>info->frames || (info->loop && info->loopStart>=info->frames) ||
        (size_t)info->frames>SIZE_MAX/sizeof(*samples) || (uintptr_t)samples>UINTPTR_MAX-(size_t)info->frames*sizeof(*samples)) return 0;
    m->voices[channel]=(HGNativeVoice){.samples=samples,.frames=info->frames,.loopStart=info->loopStart,
        .period=info->timer ? info->timer : 65536,.loop=info->loop,.volume=127,.pan=64};
    return 1;
}
int HGNativeMixerControl(HGNativeMixer *m,uint32_t mask,uint32_t volume,uint32_t shift,uint32_t pan) {
    if (!valid(m,mask) || volume>127 || shift>3 || pan>127) return 0;
    for (unsigned i=0;i<16;i++) if (mask&(1u<<i)) {
        m->voices[i].volume=(uint8_t)volume; m->voices[i].shift=(uint8_t)shift; m->voices[i].pan=(uint8_t)pan;
    }
    return 1;
}
int HGNativeMixerTimer(HGNativeMixer *m,uint32_t mask,uint32_t timer) {
    if (!valid(m,mask) || timer>65535) return 0;
    uint32_t period=timer ? timer : 65536;
    for (unsigned i=0;i<16;i++) if (mask&(1u<<i)) {
        HGNativeVoice *v=&m->voices[i];
        v->remainder=v->period ? v->remainder*period/v->period : 0; v->period=period;
    }
    return 1;
}
int HGNativeMixerMaster(HGNativeMixer *m,uint32_t volume,int32_t pan) {
    if (!valid(m,0) || volume>127 || pan<-1 || pan>127) return 0;
    m->masterVolume=(uint8_t)volume; m->masterPan=(int16_t)pan; return 1;
}
int HGNativeMixerStart(HGNativeMixer *m,uint32_t mask) {
    if (!valid(m,mask)) return 0;
    for (unsigned i=0;i<16;i++) if (mask&(1u<<i)) {
        HGNativeVoice *v=&m->voices[i]; v->position=v->remainder=0; v->startup=0; v->active=v->frames!=0;
    }
    return 1;
}
int HGNativeMixerStop(HGNativeMixer *m,uint32_t mask) {
    if (!valid(m,mask)) return 0;
    for (unsigned i=0;i<16;i++) if (mask&(1u<<i)) m->voices[i].active=0;
    return 1;
}
uint32_t HGNativeMixerActive(const HGNativeMixer *m) {
    uint32_t mask=0; if (m) for (unsigned i=0;i<16;i++) if (m->voices[i].active) mask|=1u<<i; return mask;
}
int HGNativeMixerRender(HGNativeMixer *m,int16_t *out,size_t frames) {
    if (!valid(m,0) || frames>SIZE_MAX/(2*sizeof(*out)) ||
        (frames && (!out || (uintptr_t)out%_Alignof(int16_t)))) return 0;
    size_t bytes=frames*2*sizeof(*out);
    if ((uintptr_t)out>UINTPTR_MAX-bytes) return 0;
    for (unsigned i=0;i<16;i++) {
        const HGNativeVoice *v=&m->voices[i];
        if (v->frames && bytes && (uintptr_t)v->samples<(uintptr_t)out+bytes &&
            (uintptr_t)out<(uintptr_t)v->samples+(size_t)v->frames*sizeof(*v->samples)) return 0;
    }
    static const unsigned shifts[4]={0,1,2,4};
    for (size_t sample=0;sample<frames;sample++) {
        int left=0,right=0;
        for (unsigned i=0;i<16;i++) {
            HGNativeVoice *v=&m->voices[i]; if (!v->active) continue;
            int value=v->startup ? 0 : floorDiv(gain(v->samples[v->position],v->volume),1<<shifts[v->shift]);
            unsigned pan=m->masterPan<0 ? v->pan : (unsigned)m->masterPan;
            left+=gain(value,127-pan); right+=gain(value,pan);
            // Exact rational timer advancement avoids floating-point drift across render blocks.
            uint64_t divisor=(uint64_t)v->period*m->sampleRate,sum=v->remainder+16756991u;
            uint64_t advance=sum/divisor; v->remainder=sum%divisor;
            if (v->startup) {
                if (advance<v->startup) { v->startup-=(uint32_t)advance; advance=0; }
                else { advance-=v->startup; v->startup=0; }
            }
            v->position+=advance;
            if (v->position>=v->frames) {
                if (v->loop) v->position=v->loopStart+(v->position-v->loopStart)%(v->frames-v->loopStart);
                else { v->position=v->frames; v->active=0; }
            }
        }
        out[sample*2]=clip(gain(left,m->masterVolume)); out[sample*2+1]=clip(gain(right,m->masterVolume));
    }
    return 1;
}
