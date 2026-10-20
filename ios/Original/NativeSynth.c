// SPDX-License-Identifier: GPL-3.0-or-later
#include "NativeSynth.h"
#include <nitro/snd/native_sound.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "NativeSoundTables.inc"

typedef struct HGDecodedWave {
    struct HGDecodedWave *next;
    uint32_t address,bytes;
    HGNativeWaveInfo info;
    int16_t samples[];
} HGDecodedWave;
static uint16_t read16(const uint8_t *p) { return (uint16_t)(p[0]|(uint16_t)p[1]<<8); }
static uint32_t read32(const uint8_t *p) { return read16(p)|(uint32_t)read16(p+2)<<16; }
static int floorDiv(int64_t x,int divisor) { return (int)(x<0 ? -((-x+divisor-1)/divisor) : x/divisor); }
static int clamp(int x,int low,int high) { return x<low ? low : x>high ? high : x; }
static const uint8_t *span(HGNativeSynth *s,uint32_t offset,size_t length) {
    if ((!offset && length) || offset>s->memoryBytes || length>s->memoryBytes-offset) { s->error=1; return NULL; }
    return s->memory+offset;
}
static unsigned attack(unsigned n) {
    static const uint8_t table[]={0,1,5,14,26,38,51,63,73,84,92,100,109,116,123,127,132,137,143};
    if (n>127) n=0;
    return n>=109 ? table[127-n] : 255-n;
}
static unsigned fall(unsigned n) {
    if (n>127) n=0;
    return n==127 ? 65535 : n==126 ? 15360 : n<50 ? n*2+1 : 7680/(126-n);
}
static int squared(unsigned n) { return squareTable[n<128 ? n : 0]; }
uint16_t HGNativeSynthTimer(uint16_t base,int32_t pitch) {
    if (!pitch) return base;
    int64_t inverse=-(int64_t)pitch,octave=inverse/768;
    int remainder=(int)(inverse%768); if (remainder<0) { octave--; remainder+=768; }
    uint64_t value=(uint64_t)(base ? base : 65536)*(65536u+pitchTable[remainder]);
    int64_t shift=octave-16;
    if (shift<0) value=-shift>=64 ? 0 : value>>-shift;
    else return 65535;
    return (uint16_t)(value<16 ? 16 : value>65535 ? 65535 : value);
}
static HGDecodedWave *wave(HGNativeSynth *s,uint32_t address,uint32_t limit) {
    const uint8_t *p=span(s,address,12); if (!p) return NULL;
    uint64_t length=12+((uint64_t)read16(p+6)+read32(p+8))*4;
    if (length>UINT32_MAX || length>limit || !(p=span(s,address,(size_t)length))) { s->error=1; return NULL; }
    // shortcut: the cache is a linear list of loaded waves; index it if profiling shows note lookup costs matter.
    for (HGDecodedWave *w=s->cache;w;w=w->next) if (w->address==address && w->bytes==length) return w;
    HGNativeWaveInfo info; if (!HGNativeWaveGetInfo(p,(size_t)length,&info)) { s->error=1; return NULL; }
    uint64_t allocation=sizeof(HGDecodedWave)+(uint64_t)info.frames*2;
    if (allocation>SIZE_MAX) { s->error=1; return NULL; }
    HGDecodedWave *w=malloc((size_t)allocation);
    if (!w) { s->error=2; return NULL; }
    *w=(HGDecodedWave){s->cache,address,(uint32_t)length,info};
    if (!HGNativeWaveDecode(p,(size_t)length,w->samples,info.frames,&w->info)) { free(w); s->error=1; return NULL; }
    s->cache=w; return w;
}
static HGDecodedWave *resolveWave(HGNativeSynth *s,uint32_t bank,const HGNativeInstrument *inst) {
    if (inst->type==4) return wave(s,(uint32_t)inst->wave|(uint32_t)inst->archive<<16,UINT32_MAX);
    if (inst->archive>=4) { s->error=1; return NULL; }
    const uint8_t *b=span(s,bank+24+inst->archive*8,4); if (!b) return NULL;
    uint32_t arc=read32(b); const uint8_t *header=span(s,arc,60); if (!header) return NULL;
    if (memcmp(header,"SWAR\xFF\xFE\x00\x01",8) || inst->wave>=read32(header+56)) { s->error=1; return NULL; }
    uint32_t count=read32(header+56),size=read32(header+8);
    if (count>(UINT32_MAX-60)/4 || size<60+count*4 || !span(s,arc,60+(size_t)count*4)) { s->error=1; return NULL; }
    uint32_t entry=read32(header+60+(uint32_t)inst->wave*4),address=entry&~HG_SOUND_WAVE_ADDRESS_FLAG;
    if (!entry) { s->error=1; return NULL; }
    if (entry&HG_SOUND_WAVE_ADDRESS_FLAG) return wave(s,address,UINT32_MAX);
    if (entry<60+count*4 || entry>=size || arc>UINT32_MAX-entry) { s->error=1; return NULL; }
    return wave(s,arc+entry,size-entry);
}
void HGNativeSynthDetach(HGNativeSynth *s,unsigned channel) {
    if (!s || channel>=16) return;
    HGNativeWaveInfo empty={0}; (void)HGNativeMixerSetVoice(&s->mixer,channel,NULL,0,&empty);
    memset(&s->voices[channel],0,sizeof(s->voices[channel])); s->voices[channel].track=-1;
}
static void releaseVoice(HGSynthVoice *v) { v->state=HG_SYNTH_RELEASE; v->remaining=-1; v->priority=1; }
static int allocate(HGNativeSynth *s,const HGSequenceTrack *t,unsigned type,unsigned priority) {
    static const uint8_t order[]={4,5,6,7,2,0,3,1,8,9,10,11,14,12,15,13};
    int selected=-1; unsigned lowest=UINT_MAX,quietest=UINT_MAX;
    for (unsigned n=0;n<16;n++) {
        unsigned id=order[n]; if ((s->locked&(1u<<id)) || !(t->channels&(1u<<id))) continue;
        if ((type==2 && (id<8 || id>13)) || (type==3 && id<14)) continue;
        HGSynthVoice *v=&s->voices[id]; HGNativeVoice *m=&s->mixer.voices[id];
        unsigned p=v->state ? v->priority : 0,volume=m->active ? (unsigned)m->volume>>(m->shift==3 ? 4 : m->shift) : 0;
        if (p<lowest || (p==lowest && volume<quietest)) { selected=(int)id; lowest=p; quietest=volume; }
    }
    return selected<0 || priority<lowest ? -1 : selected;
}
static void setSweep(HGSynthVoice *v,const HGSequenceTrack *t,unsigned key,int32_t duration) {
    v->sweepCount=0; v->sweep=t->sweepPitch; v->sweepLength=0; v->manualSweep=0;
    if (!t->portamento) return;
    v->sweep=(int16_t)(uint16_t)(v->sweep+((int)t->portaKey-(int)key)*64);
    if (!t->portaTime) { v->manualSweep=1; v->sweepLength=duration>0 ? (uint32_t)duration : 0; }
    else { uint64_t amount=(uint64_t)(v->sweep<0 ? -v->sweep : v->sweep); v->sweepLength=(uint32_t)(amount*t->portaTime*t->portaTime/2048); }
}
static int sequenceEvent(void *context,const HGSequenceEvent *event) {
    HGNativeSynth *s=context; HGSequenceTrack *t=&s->sequence.tracks[event->track]; HGSequencePlayer *p=&s->sequence.players[t->player];
    if (event->kind!=HG_SEQUENCE_NOTE) {
        for (unsigned i=0;i<16;i++) {
            HGSynthVoice *v=&s->voices[i]; if (v->track!=(int)event->track) continue;
            if (event->kind==HG_SEQUENCE_KILL) HGNativeSynthDetach(s,i);
            else if (event->kind==HG_SEQUENCE_RELEASE) releaseVoice(v);
            else if (v->state>HG_SYNTH_START) {
                if (v->remaining>0 && !--v->remaining) releaseVoice(v);
                if (v->manualSweep && v->sweepCount<v->sweepLength) v->sweepCount++;
            }
        }
        return 1;
    }
    unsigned priority=(unsigned)clamp(p->priority+t->priority,0,255);
    if (event->tie) for (unsigned i=0;i<16;i++) {
        HGSynthVoice *v=&s->voices[i];
        if (v->track==(int)event->track && v->state && v->state!=HG_SYNTH_RELEASE) {
            v->key=event->key; v->velocity=(int16_t)squared(event->velocity); v->priority=(uint8_t)priority;
            v->remaining=-1; v->modCounter=v->modDelayCount=0; setSweep(v,t,event->key,event->duration); return 1;
        }
    }
    const uint8_t *bank=span(s,p->bank,60); if (!bank) return 0;
    uint32_t size=read32(bank+8); if (!span(s,p->bank,size)) return 0;
    HGNativeInstrument inst; int found=HGNativeReadInstrument(bank,size,t->program,event->key,&inst);
    if (found==HG_INSTRUMENT_MISSING || (found==HG_INSTRUMENT_FOUND && inst.type==5)) return 1;
    if (found!=HG_INSTRUMENT_FOUND) { s->error=1; return 0; }
    if (inst.rootKey>127 || inst.pan>127) { s->error=1; return 0; }
    int id=allocate(s,t,inst.type,priority); if (id<0) return 1;
    HGDecodedWave *data=NULL; HGNativeWaveInfo info={.frames=8,.timer=0x1F46,.loop=1}; const int16_t *samples=NULL;
    if (inst.type==1 || inst.type==4) { data=resolveWave(s,p->bank,&inst); if (!data) return 0; info=data->info; samples=data->samples; }
    else if (inst.type==2) samples=s->duty[inst.wave&7];
    else if (inst.type==3) { info.frames=32767; samples=s->noise; }
    else { s->error=1; return 0; }
    HGNativeSynthDetach(s,(unsigned)id);
    if (!HGNativeMixerSetVoice(&s->mixer,(unsigned)id,samples,info.frames,&info)) return 0;
    HGSynthVoice *v=&s->voices[id];
    *v=(HGSynthVoice){.wave=data,.remaining=event->tie ? -1 : event->duration,.timer=info.timer,
        .velocity=(int16_t)squared(event->velocity),.track=(int8_t)event->track,.pan=(int8_t)((int)inst.pan-64),
        .state=HG_SYNTH_START,.priority=(uint8_t)priority,.key=event->key,.root=inst.rootKey,
        .attack=(uint8_t)attack(t->attack==255 ? inst.attack : t->attack),.sustain=t->sustain==255 ? inst.sustain : t->sustain,
        .decay=(uint16_t)fall(t->decay==255 ? inst.decay : t->decay),.release=(uint16_t)fall(t->release==255 ? inst.release : t->release),.type=inst.type};
    setSweep(v,t,event->key,event->duration); return 1;
}
int HGNativeSynthInit(HGNativeSynth *s,uint32_t rate,const void *memory,size_t bytes) {
    if (!s || !memory || !bytes || bytes>HG_SOUND_WAVE_ADDRESS_FLAG || (uintptr_t)memory>UINTPTR_MAX-bytes || !rate) return 0;
    memset(s,0,sizeof(*s)); s->memory=memory; s->memoryBytes=bytes;
    if (!HGNativeMixerInit(&s->mixer,rate)) return 0;
    HGNativeSequenceInit(&s->sequence,sequenceEvent,s);
    for (unsigned i=0;i<16;i++) s->voices[i].track=-1;
    for (unsigned d=0;d<8;d++) for (unsigned i=0;i<8;i++) s->duty[d][i]=(int16_t)(d<7 && i>=7-d ? 32767 : -32767);
    unsigned state=0x7FFF;
    for (unsigned i=0;i<32767;i++) { unsigned bit=state&1; s->noise[i]=(int16_t)(bit ? -32767 : 32767); state=(state>>1)^(bit ? 0x6000 : 0); }
    return 1;
}
int HGNativeSynthPause(HGNativeSynth *s,unsigned player,int paused) {
    if (!s || !HGNativeSequencePause(&s->sequence,player,paused)) return 0;
    if (paused) for (unsigned i=0;i<16;i++) if (s->voices[i].track>=0 && s->sequence.tracks[(unsigned)s->voices[i].track].player==player) releaseVoice(&s->voices[i]);
    return 1;
}
static int sine(unsigned phase) {
    static const uint8_t quarter[]={0,6,12,19,25,31,37,43,49,54,60,65,71,76,81,85,90,94,98,102,106,109,112,115,117,120,122,123,125,126,126,127,127};
    return phase<32 ? quarter[phase] : phase<64 ? quarter[64-phase] : phase<96 ? -(int)quarter[phase-64] : -(int)quarter[128-phase];
}
static int clockSynth(HGNativeSynth *s) {
    for (unsigned i=0;i<16;i++) {
        HGSynthVoice *v=&s->voices[i]; if (v->track<0) continue;
        if (v->state>HG_SYNTH_START && !s->mixer.voices[i].active) { HGNativeSynthDetach(s,i); continue; }
        HGSequenceTrack *t=&s->sequence.tracks[(unsigned)v->track]; HGSequencePlayer *p=&s->sequence.players[t->player];
        unsigned starting=v->state==HG_SYNTH_START;
        if (starting) { v->amplitude=-723*128; v->state=HG_SYNTH_ATTACK; }
        if (v->state==HG_SYNTH_ATTACK) {
            int before=floorDiv(v->amplitude,128);
            do { v->amplitude=v->amplitude*v->attack/256; } while (v->amplitude && floorDiv(v->amplitude,128)==before);
            if (!v->amplitude) v->state=HG_SYNTH_DECAY;
        } else if (v->state==HG_SYNTH_DECAY) {
            v->amplitude-=v->decay; int sustain=squared(v->sustain)*128;
            if (v->amplitude<=sustain) { v->amplitude=sustain; v->state=HG_SYNTH_SUSTAIN; }
        } else if (v->state==HG_SYNTH_RELEASE) {
            v->amplitude-=v->release;
            if (v->amplitude<=-723*128) { HGNativeSynthDetach(s,i); continue; }
        }
        int modulation=0;
        if (t->modDepth) {
            if (v->modDelayCount<t->modDelay) v->modDelayCount++;
            else {
                int amount=sine(v->modCounter>>8)*t->modRange*t->modDepth;
                modulation=t->modType==0 ? floorDiv((int64_t)amount*60,16384) : floorDiv(amount,256);
                v->modCounter=(uint16_t)((v->modCounter+(unsigned)t->modSpeed*64)&32767);
            }
        }
        int pitch=((int)v->key-v->root)*64+floorDiv((int)t->pitchBend*t->bendRange,2)+t->extPitch;
        if (t->modType==0) pitch+=modulation;
        if (v->sweepLength && v->sweepCount<=v->sweepLength) {
            pitch+=(int)((int64_t)v->sweep*(v->sweepLength-v->sweepCount)/v->sweepLength);
            if (!v->manualSweep) v->sweepCount++;
        }
        uint16_t timer=HGNativeSynthTimer(v->timer,pitch);
        int db=floorDiv(v->amplitude,128)+v->velocity+squared(p->volume)+squared(t->volume)+squared(t->expression)+p->extFader+t->extFader;
        if (t->modType==1) db+=modulation;
        db=clamp(db,-723,0); unsigned shift=db<-240 ? 3 : db<-120 ? 2 : db<-60 ? 1 : 0;
        int pan=(v->pan+t->pan+t->extPan)*(int)t->panRange/127+64;
        if (t->modType==2) pan+=modulation;
        if (!HGNativeMixerTimer(&s->mixer,1u<<i,timer) || !HGNativeMixerControl(&s->mixer,1u<<i,volumeTable[db+723],shift,(unsigned)clamp(pan,0,127))) return 0;
        if (starting) {
            if (!HGNativeMixerStart(&s->mixer,1u<<i)) return 0;
            s->mixer.voices[i].startup=v->type==1 || v->type==4 ? 3 : 1;
        }
    }
    return HGNativeSequenceTick(&s->sequence);
}
int HGNativeSynthRender(HGNativeSynth *s,int16_t *out,size_t frames) {
    if (!s || !s->mixer.sampleRate || frames>SIZE_MAX/4 || (frames && (!out || (uintptr_t)out%_Alignof(int16_t))) ||
        (uintptr_t)out>UINTPTR_MAX-frames*4) return 0;
    if (frames && (uintptr_t)out<(uintptr_t)s->memory+s->memoryBytes && (uintptr_t)s->memory<(uintptr_t)out+frames*4) return 0;
    uint64_t period=(uint64_t)s->mixer.sampleRate*HG_SEQUENCE_TICK_CLOCKS;
    while (frames) {
        uint64_t until=(period-s->phase+HG_SEQUENCE_CLOCK-1)/HG_SEQUENCE_CLOCK;
        size_t count=frames<until ? frames : (size_t)until;
        if (!HGNativeMixerRender(&s->mixer,out,count)) return 0;
        s->phase+=(uint64_t)count*HG_SEQUENCE_CLOCK; frames-=count; out+=count*2;
        while (s->phase>=period) { s->phase-=period; if (!clockSynth(s)) return 0; }
    }
    return 1;
}
int HGNativeSynthInvalidate(HGNativeSynth *s,uint32_t first,uint32_t end,unsigned kind) {
    if (!s || first>end || end>s->memoryBytes || kind>2) return 0;
    if (first==end) return 1;
    if (kind<2) for (unsigned p=0;p<16;p++) {
        HGSequencePlayer *player=&s->sequence.players[p];
        if (!player->active) continue;
        uintptr_t data=(uintptr_t)player->data,base=(uintptr_t)s->memory;
        if (!kind && (data<base || data-base>s->memoryBytes)) continue;
        uint32_t address=kind ? player->bank : (uint32_t)(data-base);
        if (address<end && first<=address && !HGNativeSequenceStop(&s->sequence,p)) return 0;
    }
    if (kind==2) {
        HGDecodedWave **link=&s->cache;
        while (*link) {
            HGDecodedWave *w=*link;
            if (w->address<end && first<w->address+w->bytes) {
                for (unsigned i=0;i<16;i++) if (s->voices[i].wave==w) HGNativeSynthDetach(s,i);
                *link=w->next; free(w);
            } else link=&w->next;
        }
    }
    return 1;
}
void HGNativeSynthClose(HGNativeSynth *s) {
    if (!s) return;
    for (unsigned i=0;i<16;i++) HGNativeSynthDetach(s,i);
    HGDecodedWave *w=s->cache; while (w) { HGDecodedWave *next=w->next; free(w); w=next; }
    memset(s,0,sizeof(*s));
}
