// SPDX-License-Identifier: GPL-3.0-or-later
#include "NativeImports.h"
#include "../NativeSynth.h"
#include <time.h>
#include <nitro/snd/native_sound.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct SoundCommand { u32 id,arg[4]; } SoundCommand;
typedef struct SoundBatch { struct SoundBatch *next; u32 count; SoundCommand commands[]; } SoundBatch;
struct HGNativeSoundContext {
    HGNativeSynth synth;
    u64 lastNanos,remainder;
    SoundBatch *head,*tail;
    u32 count,shared,finished,locked;
    u32 waveStart[16],waveEnd[16];
    int16_t *waves[16];
    u8 rawParams[16][12];
};
static u8 *span(struct w2c_env *env,u32 offset,u32 bytes) {
    if ((!offset && bytes) || offset>env->memory->size || bytes>env->memory->size-offset) wasm_rt_trap(WASM_RT_TRAP_OOB);
    return env->memory->data+offset;
}
static u32 read32(const u8 *p) { u32 value; memcpy(&value,p,4); return value; }
static void store32(u8 *p,u32 value) { memcpy(p,&value,4); }
static void store16(u8 *p,u16 value) { memcpy(p,&value,2); }
static void require(int condition) { if (!condition) wasm_rt_trap(WASM_RT_TRAP_OOB); }
static _Noreturn void unsupported(u32 id) {
    fprintf(stderr,"Unimplemented native sound command 0x%02X\n",id); wasm_rt_trap(WASM_RT_TRAP_UNREACHABLE);
}
static u64 now(void) {
    struct timespec stamp; require(!clock_gettime(CLOCK_MONOTONIC,&stamp));
    return (u64)stamp.tv_sec*1000000000ULL+(u64)stamp.tv_nsec;
}
static void render(struct HGNativeSoundContext *s,int16_t *out,size_t frames) {
    if (!HGNativeSynthRender(&s->synth,out,frames)) {
        fprintf(stderr,"Native synthesis failed: resource=%u sequence=%u player=%u track=%u position=%u command=0x%02X\n",
            s->synth.error,s->synth.sequence.error.code,s->synth.sequence.error.player,s->synth.sequence.error.track,
            s->synth.sequence.error.position,s->synth.sequence.error.command);
        wasm_rt_trap(WASM_RT_TRAP_OOB);
    }
}
static void realtime(struct w2c_env *env) {
    struct HGNativeSoundContext *s=env->sound; u64 stamp=now();
    if (!env->soundRealtime) { s->lastNanos=stamp; s->remainder=0; return; }
    u64 elapsed=stamp-s->lastNanos; s->lastNanos=stamp;
    unsigned __int128 scaled=(unsigned __int128)elapsed*48000+s->remainder;
    u64 frames=(u64)(scaled/1000000000); s->remainder=(u64)(scaled%1000000000);
    int16_t buffer[2048];
    while (frames) {
        size_t count=frames>1024 ? 1024 : (size_t)frames;
        render(s,buffer,count);
        if (env->soundOutput) env->soundOutput(env->soundOutputContext,buffer,count);
        frames-=count;
    }
}
int HGNativeSoundSetOutput(struct w2c_env *env,void (*output)(void *,const int16_t *,size_t),void *context) {
    if (!env || !env->memory || env->resetRequested || (!output && context)) return FALSE;
    if (env->sound) realtime(env);
    env->soundOutput=output; env->soundOutputContext=context; return TRUE;
}
int HGNativeSoundRealtime(struct w2c_env *env,int enabled) {
    if (!env || !env->memory || env->resetRequested || (enabled!=0 && enabled!=1)) return FALSE;
    if (env->sound) {
        realtime(env);
        if (env->soundRealtime!=(u32)enabled) { env->sound->lastNanos=now(); env->sound->remainder=0; }
    }
    env->soundRealtime=(u32)enabled; return TRUE;
}
static void status(struct w2c_env *env) {
    struct HGNativeSoundContext *s=env->sound; HGNativeSequence *seq=&s->synth.sequence;
    u32 players=0;
    for (unsigned p=0;p<16;p++) {
        if (seq->players[p].active && HGNativeSequenceEnded(seq,p)) {
            BOOL voices=FALSE;
            for (unsigned c=0;c<16;c++) if (s->synth.voices[c].track>=0 &&
                seq->tracks[(unsigned)s->synth.voices[c].track].player==p) voices=TRUE;
            if (!voices) require(HGNativeSequenceStop(seq,p));
        }
        if (seq->players[p].active) players|=1u<<p;
        if (s->shared) {
            u8 *shared=span(env,s->shared+0x20+p*36,36);
            for (unsigned v=0;v<16;v++) store16(shared+v*2,(u16)seq->players[p].variables[v]);
            store32(shared+32,seq->players[p].ticks);
        }
    }
    for (unsigned c=0;c<16;c++) if (s->synth.voices[c].track>=0 && s->waves[c]) {
        free(s->waves[c]); s->waves[c]=NULL; s->waveStart[c]=s->waveEnd[c]=0;
    }
    if (s->shared) {
        store32(span(env,s->shared+4,4),players);
        store16(span(env,s->shared+8,2),(u16)HGNativeMixerActive(&s->synth.mixer));
        for (unsigned v=0;v<16;v++) store16(span(env,s->shared+0x260+v*2,2),(u16)seq->globals[v]);
    }
}
static void playerWire(const HGSequencePlayer *p,unsigned id,u8 *out) {
    memset(out,0,36); out[0]=(u8)(p->active|p->prepared<<1|p->paused<<2); out[1]=(u8)id;
    out[4]=p->priority; out[5]=p->volume; store16(out+6,(u16)p->extFader); memcpy(out+8,p->tracks,16);
    store16(out+24,p->tempo); store16(out+26,p->ratio); store16(out+28,p->counter); store32(out+32,p->bank);
}
static void playerParam(struct HGNativeSoundContext *s,const u32 *a) {
    require(a[0]<16 && (a[3]==1 || a[3]==2 || a[3]==4));
    require((a[1]>=4 && a[1]<=8 && a[3]<=8-a[1]) || (a[1]>=24 && a[1]<=30 && a[3]<=30-a[1]));
    HGSequencePlayer *p=&s->synth.sequence.players[a[0]]; u8 raw[36]; playerWire(p,a[0],raw); memcpy(raw+a[1],a+2,a[3]);
    p->priority=raw[4]; p->volume=raw[5]; p->extFader=(s16)(raw[6]|raw[7]<<8);
    p->tempo=(u16)(raw[24]|raw[25]<<8); p->ratio=(u16)(raw[26]|raw[27]<<8); p->counter=(u16)(raw[28]|raw[29]<<8);
}
static void trackWire(struct w2c_env *env,unsigned id,u8 *out) {
    HGSequenceTrack *t=&env->sound->synth.sequence.tracks[id]; HGSequencePlayer *p=&env->sound->synth.sequence.players[t->player];
    memset(out,0,64); if (!t->allocated) return;
    out[0]=(u8)(t->active|t->noteWait<<1|t->muted<<2|t->tie<<3|t->portamento<<5|t->comparison<<6|(t->channels!=0xFFFF)<<7);
    out[1]=t->panRange; store16(out+2,t->program); out[4]=t->volume; out[5]=t->expression;
    out[6]=(u8)t->pitchBend; out[7]=t->bendRange; out[8]=(u8)t->pan; out[9]=(u8)t->extPan;
    store16(out+10,(u16)t->extFader); store16(out+12,(u16)t->extPitch);
    out[14]=t->attack; out[15]=t->decay; out[16]=t->sustain; out[17]=t->release;
    out[18]=t->priority; out[19]=(u8)t->transpose; out[20]=t->portaKey; out[21]=t->portaTime; store16(out+22,(u16)t->sweepPitch);
    out[24]=t->modType; out[25]=t->modSpeed; out[26]=t->modDepth; out[27]=t->modRange;
    store16(out+28,t->modDelay); store16(out+30,t->channels); store32(out+32,(u32)t->wait);
    u32 base=(u32)(p->data-env->memory->data); store32(out+36,base); store32(out+40,base+t->position);
    for (unsigned n=0;n<t->depth;n++) store32(out+44+n*4,base+t->stack[n]);
    memcpy(out+56,t->loops,3); out[59]=t->depth;
}
static void trackParam(struct w2c_env *env,const u32 *a) {
    unsigned player=a[0]&0xFFFFFF,size=a[0]>>24; require(player<16 && (size==1 || size==2 || size==4));
    require(!(a[1]&~0xFFFFu) && a[2]>=1 && a[2]<=36 && size<=36-a[2]);
    HGNativeSequence *seq=&env->sound->synth.sequence;
    for (unsigned n=0;n<16;n++) if ((a[1]&(1u<<n)) && seq->players[player].tracks[n]!=255) {
        unsigned id=seq->players[player].tracks[n]; HGSequenceTrack *t=&seq->tracks[id]; u8 raw[64];
        trackWire(env,id,raw); memcpy(raw+a[2],a+3,size);
        t->panRange=raw[1]; t->program=(u16)(raw[2]|raw[3]<<8); t->volume=raw[4]; t->expression=raw[5];
        t->pitchBend=(s8)raw[6]; t->bendRange=raw[7]; t->pan=(s8)raw[8]; t->extPan=(s8)raw[9];
        t->extFader=(s16)(raw[10]|raw[11]<<8); t->extPitch=(s16)(raw[12]|raw[13]<<8);
        t->attack=raw[14]; t->decay=raw[15]; t->sustain=raw[16]; t->release=raw[17]; t->priority=raw[18];
        t->transpose=(s8)raw[19]; t->portaKey=raw[20]; t->portaTime=raw[21]; t->sweepPitch=(s16)(raw[22]|raw[23]<<8);
        t->modType=raw[24]; t->modSpeed=raw[25]; t->modDepth=raw[26]; t->modRange=raw[27];
        t->modDelay=(u16)(raw[28]|raw[29]<<8); t->channels=(u16)(raw[30]|raw[31]<<8); t->wait=(s32)read32(raw+32);
    }
}
static void prepareSequence(struct w2c_env *env,const u32 *a,BOOL start) {
    require(a[0]<16 && a[1]>=28); const u8 *header=span(env,a[1]-28,28);
    // shortcut: this ROM uses SSEQ files; register bounded SSAR regions before supporting archive/custom sequence bases.
    if (memcmp(header,"SSEQ\xFF\xFE\x00\x01",8)) unsupported(0x02);
    u32 bytes=read32(header+8); require(bytes>28 && read32(header+24)==28);
    (void)span(env,a[1]-28,bytes); const u8 *bank=span(env,a[3],60); u32 bankSize=read32(bank+8);
    bank=span(env,a[3],bankSize); HGNativeInstrument ignored;
    require(HGNativeReadInstrument(bank,bankSize,UINT32_MAX,0,&ignored)!=HG_INSTRUMENT_INVALID);
    if (!HGNativeSequencePrepare(&env->sound->synth.sequence,a[0],env->memory->data+a[1],bytes-28,a[2],a[3])) {
        require(env->sound->synth.sequence.error.code==HG_SEQUENCE_LIMIT);
        require(HGNativeSequenceStop(&env->sound->synth.sequence,a[0])); return;
    }
    if (start) require(HGNativeSequenceStart(&env->sound->synth.sequence,a[0]));
}
static void clearVoice(struct HGNativeSoundContext *s,unsigned channel) {
    HGNativeSynthDetach(&s->synth,channel);
    free(s->waves[channel]); s->waves[channel]=NULL; s->waveStart[channel]=s->waveEnd[channel]=0;
}
static void setupPCM(struct w2c_env *env,const u32 *arg) {
    struct HGNativeSoundContext *s=env->sound;
    u32 channel=arg[0]&0xFFFF,timer=arg[0]>>16,format=(arg[3]>>24)&3,loop=(arg[3]>>26)&3;
    u32 start=arg[3]&0xFFFF,tail=arg[2]&0x3FFFFF,volume=arg[2]>>24,shift=(arg[2]>>22)&3,pan=(arg[3]>>16)&255;
    require(channel<16 && format<3 && loop<3 && volume<=127 && pan<=127 && !(arg[3]&0xF0000000));
    uint64_t bytes=((uint64_t)start+tail)*4; require(bytes<=UINT32_MAX-12);
    const u8 *source=span(env,arg[1],(u32)bytes);
    u8 *encoded=malloc((size_t)bytes+12); if (!encoded) wasm_rt_trap(WASM_RT_TRAP_EXHAUSTION);
    memset(encoded,0,12); encoded[0]=(u8)format; encoded[1]=loop==1;
    store16(encoded+4,(u16)timer); store16(encoded+6,(u16)start); store32(encoded+8,tail);
    if (bytes) memcpy(encoded+12,source,(size_t)bytes);
    HGNativeWaveInfo info;
    if (!HGNativeWaveGetInfo(encoded,(size_t)bytes+12,&info)) { free(encoded); wasm_rt_trap(WASM_RT_TRAP_OOB); }
    int16_t *decoded=malloc((size_t)info.frames*2+2);
    if (!decoded) { free(encoded); wasm_rt_trap(WASM_RT_TRAP_EXHAUSTION); }
    require(HGNativeWaveDecode(encoded,(size_t)bytes+12,decoded,info.frames,&info)); memcpy(s->rawParams[channel],encoded,12); free(encoded);
    HGNativeSynthDetach(&s->synth,channel);
    require(HGNativeMixerSetVoice(&s->synth.mixer,channel,decoded,info.frames,&info));
    free(s->waves[channel]); s->waves[channel]=decoded;
    s->waveStart[channel]=arg[1]; s->waveEnd[channel]=arg[1]+(u32)bytes;
    require(HGNativeMixerControl(&s->synth.mixer,1u<<channel,volume,shift,pan));
}
static void driverInfo(struct w2c_env *env,u32 offset) {
    status(env);
    struct HGNativeSoundContext *s=env->sound; u8 *out=span(env,offset,0x11E0); memset(out,0,0x11E0);
    for (unsigned i=0;i<16;i++) {
        HGNativeVoice *v=&s->synth.mixer.voices[i]; u8 *channel=out+i*0x54;
        HGSynthVoice *voice=&s->synth.voices[i];
        channel[0]=(u8)i; channel[2]=v->active ? 2 : 3; channel[3]=v->active;
        if (voice->track>=0) {
            channel[1]=voice->type==2 ? 1 : voice->type==3 ? 2 : 0;
            channel[2]=voice->state<=HG_SYNTH_ATTACK ? 0 : voice->state==HG_SYNTH_DECAY ? 1 : voice->state==HG_SYNTH_SUSTAIN ? 2 : 3;
            channel[4]=127; channel[5]=voice->root; channel[8]=voice->key; channel[10]=(u8)voice->pan;
            store32(channel+16,(u32)voice->amplitude); store32(channel+20,voice->sweepCount); store32(channel+24,voice->sweepLength);
            channel[28]=voice->attack; channel[29]=voice->sustain; store16(channel+30,voice->decay); store16(channel+32,voice->release);
            channel[34]=voice->priority; store16(channel+38,voice->timer); store32(channel+52,(u32)voice->remaining);
            if (voice->address) { memcpy(channel+56,span(env,voice->address,12),12); store32(channel+68,voice->address+12); }
            else channel[56]=3;
        } else { memcpy(channel+56,s->rawParams[i],12); store32(channel+68,s->waveStart[i]); }
        channel[0x23]=v->pan; store16(channel+0x24,(u16)(v->volume|v->shift<<8));
        store32(out+0x1180+i*4,v->volume|((u32)v->shift<<8)|((u32)v->pan<<16)|((u32)v->active<<31));
        playerWire(&s->synth.sequence.players[i],i,out+0x540+i*36);
    }
    for (unsigned t=0;t<32;t++) {
        u8 *track=out+0x780+t*64; trackWire(env,t,track); u32 previous=0;
        for (unsigned c=0;c<16;c++) if (s->synth.voices[c].track==(int)t) {
            u32 address=offset+c*0x54;
            if (previous) store32(out+previous-offset+0x50,address); else store32(track+60,address);
            previous=address;
        }
    }
    store32(out+0x11C0,offset); store32(out+0x11C4,s->locked);
}
static void execute(struct w2c_env *env,const SoundCommand *c) {
    struct HGNativeSoundContext *s=env->sound; const u32 *a=c->arg;
    switch (c->id) {
    case 0x1D: require(!(a[0]&3)); (void)span(env,a[0],0x280); s->shared=a[0]; break;
    case 0x00: case 0x02: prepareSequence(env,a,c->id==0); break;
    case 0x01: require(a[0]<16 && HGNativeSequenceStop(&s->synth.sequence,a[0])); break;
    case 0x03:
        require(a[0]<16); if (s->synth.sequence.players[a[0]].active) require(HGNativeSequenceStart(&s->synth.sequence,a[0])); break;
    case 0x04:
        require(a[0]<16 && a[1]<=1); if (s->synth.sequence.players[a[0]].active) require(HGNativeSynthPause(&s->synth,a[0],(int)a[1])); break;
    case 0x06: playerParam(s,a); break;
    case 0x07: trackParam(env,a); break;
    case 0x08: case 0x09:
        require(a[0]<16 && !(a[1]&~0xFFFFu) && (c->id==0x08 ? a[2]<=3 : !(a[2]&~0xFFFFu)));
        for (unsigned n=0;n<16;n++) if ((a[1]&(1u<<n)) && s->synth.sequence.players[a[0]].tracks[n]!=255) {
            unsigned id=s->synth.sequence.players[a[0]].tracks[n]; HGSequenceTrack *t=&s->synth.sequence.tracks[id];
            if (c->id==0x09) t->channels=(u16)a[2];
            else {
                t->muted=a[2]!=0;
                if (a[2]>=2) for (unsigned ch=0;ch<16;ch++) if (s->synth.voices[ch].track==(int)id) {
                    if (a[2]==3) HGNativeSynthDetach(&s->synth,ch);
                    else { s->synth.voices[ch].state=HG_SYNTH_RELEASE; s->synth.voices[ch].priority=1; s->synth.voices[ch].remaining=-1; }
                }
            }
        }
        break;
    case 0x0A: require(s->shared && a[0]<16 && a[1]<16); s->synth.sequence.players[a[0]].variables[a[1]]=(s16)a[2]; break;
    case 0x0B: require(s->shared && a[0]<16); s->synth.sequence.globals[a[0]]=(s16)a[1]; break;
    case 0x0C: case 0x0D:
        if (a[1] || a[2] || a[3]) unsupported(c->id);
        require(c->id==0x0C ? HGNativeMixerStart(&s->synth.mixer,a[0]) : HGNativeMixerStop(&s->synth.mixer,a[0])); break;
    case 0x0E: setupPCM(env,a); break;
    case 0x13: require(HGNativeMixerTimer(&s->synth.mixer,a[0],a[1])); break;
    case 0x14: case 0x15:
        require(!(a[0]&~0xFFFFu) && a[1]<=127 && (c->id!=0x14 || a[2]<=3));
        for (unsigned i=0;i<16;i++) if (a[0]&(1u<<i)) {
            HGNativeVoice *v=&s->synth.mixer.voices[i];
            require(HGNativeMixerControl(&s->synth.mixer,1u<<i,c->id==0x14 ? a[1] : v->volume,
                c->id==0x14 ? a[2] : v->shift,c->id==0x15 ? a[1] : v->pan));
        } break;
    case 0x17: require(HGNativeMixerMaster(&s->synth.mixer,a[0],s->synth.mixer.masterPan)); break;
    case 0x18: require(HGNativeMixerMaster(&s->synth.mixer,s->synth.mixer.masterVolume,(s32)a[0])); break;
    case 0x19: if (a[0] || a[1] || a[2] || a[3]) unsupported(c->id); break;
    case 0x1A: case 0x1B:
        require(!(a[0]&~0xFFFFu)); if (a[1]) unsupported(c->id);
        if (c->id==0x1A) {
            s->locked|=a[0]; s->synth.locked=s->locked;
            for (unsigned ch=0;ch<16;ch++) if ((a[0]&(1u<<ch)) && s->synth.voices[ch].track>=0) HGNativeSynthDetach(&s->synth,ch);
            require(HGNativeMixerStop(&s->synth.mixer,a[0]));
        }
        else { s->locked&=~a[0]; s->synth.locked=s->locked; } break;
    case 0x1C:
        if (a[1]) unsupported(c->id);
        require(!(a[0]&~0xFFFFu) && HGNativeMixerStop(&s->synth.mixer,a[0]&~s->locked)); break;
    case 0x1E: case 0x1F: case 0x20:
        require(a[0]<=a[1]); (void)span(env,a[0],a[1]-a[0]);
        require(HGNativeSynthInvalidate(&s->synth,a[0],a[1],c->id-0x1E));
        if (c->id==0x20 && a[0]<a[1]) for (unsigned i=0;i<16;i++)
            if (s->waveStart[i]<a[1] && a[0]<s->waveEnd[i]) clearVoice(s,i);
        break;
    case 0x21: driverInfo(env,a[0]); break;
    default: unsupported(c->id);
    }
}
u32 w2c_env_HGNativeSoundRequest(struct w2c_env *env,u32 operation,u32 list) {
    if (!env || !env->memory || !env->memory->data || env->memory->is64 ||
        env->memory->pages!=env->memory->max_pages || env->resetRequested || operation>HG_SOUND_PUMP ||
        (operation!=HG_SOUND_SUBMIT && list)) wasm_rt_trap(WASM_RT_TRAP_OOB);
    if (operation==HG_SOUND_INIT) {
        if (!env->sound) {
            env->sound=calloc(1,sizeof(*env->sound)); if (!env->sound) wasm_rt_trap(WASM_RT_TRAP_EXHAUSTION);
            require(HGNativeSynthInit(&env->sound->synth,48000,env->memory->data,env->memory->size));
            env->sound->lastNanos=now();
        }
        return TRUE;
    }
    if (!env->sound) return FALSE;
    struct HGNativeSoundContext *s=env->sound;
    if (operation==HG_SOUND_SUBMIT) {
        if (s->count>=8) return FALSE;
        SoundCommand commands[256]; u32 seen[256],count=0;
        require(list!=0);
        while (list) {
            require(!(list&3) && count<256); for (u32 i=0;i<count;i++) require(seen[i]!=list);
            const u8 *node=span(env,list,24); seen[count]=list;
            commands[count].id=read32(node+4); require(commands[count].id<=0x21);
            memcpy(commands[count].arg,node+8,16); count++; list=read32(node);
        }
        SoundBatch *batch=malloc(sizeof(*batch)+count*sizeof(*commands)); if (!batch) return FALSE;
        *batch=(SoundBatch){NULL,count}; memcpy(batch->commands,commands,count*sizeof(*commands));
        if (s->tail) s->tail->next=batch; else s->head=batch;
        s->tail=batch; s->count++; return TRUE;
    }
    realtime(env);
    while (s->head) {
        SoundBatch *batch=s->head;
        for (u32 i=0;i<batch->count;i++) execute(env,&batch->commands[i]);
        require(s->shared!=0); s->finished++; store32(span(env,s->shared,4),s->finished); status(env);
        s->head=batch->next; if (!s->head) s->tail=NULL; s->count--; free(batch);
    }
    status(env);
    return TRUE;
}
int HGNativeSoundRender(struct w2c_env *env,int16_t *output,size_t frames) {
    if (!env || env->resetRequested || !env->sound || env->soundRealtime) return FALSE;
    (void)w2c_env_HGNativeSoundRequest(env,HG_SOUND_PUMP,0);
    int result=HGNativeSynthRender(&env->sound->synth,output,frames); if (result) status(env); return result;
}
void HGNativeCloseSoundContext(struct w2c_env *env) {
    env->soundOutput=NULL; env->soundOutputContext=NULL;
    if (!env->sound) return;
    HGNativeSynthClose(&env->sound->synth);
    for (unsigned i=0;i<16;i++) free(env->sound->waves[i]);
    SoundBatch *batch=env->sound->head;
    while (batch) { SoundBatch *next=batch->next; free(batch); batch=next; }
    free(env->sound); env->sound=NULL;
}
