// SPDX-License-Identifier: GPL-3.0-or-later
#include "NativeImports.h"
#include "../NativeMixer.h"
#include <nitro/snd/native_sound.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct SoundCommand { u32 id,arg[4]; } SoundCommand;
typedef struct SoundBatch { struct SoundBatch *next; u32 count; SoundCommand commands[]; } SoundBatch;
struct HGNativeSoundContext {
    HGNativeMixer mixer;
    SoundBatch *head,*tail;
    u32 count,shared,finished,locked;
    u32 waveStart[16],waveEnd[16];
    int16_t *waves[16];
    u8 players[16][36];
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
static void status(struct w2c_env *env) {
    if (env->sound->shared) store16(span(env,env->sound->shared+8,2),(u16)HGNativeMixerActive(&env->sound->mixer));
}
static void clearVoice(struct HGNativeSoundContext *s,unsigned channel) {
    HGNativeWaveInfo empty={0}; require(HGNativeMixerSetVoice(&s->mixer,channel,NULL,0,&empty));
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
    require(HGNativeWaveDecode(encoded,(size_t)bytes+12,decoded,info.frames,&info)); free(encoded);
    require(HGNativeMixerSetVoice(&s->mixer,channel,decoded,info.frames,&info));
    free(s->waves[channel]); s->waves[channel]=decoded;
    s->waveStart[channel]=arg[1]; s->waveEnd[channel]=arg[1]+(u32)bytes;
    require(HGNativeMixerControl(&s->mixer,1u<<channel,volume,shift,pan));
}
static void driverInfo(struct w2c_env *env,u32 offset) {
    struct HGNativeSoundContext *s=env->sound; u8 *out=span(env,offset,0x11E0); memset(out,0,0x11E0);
    for (unsigned i=0;i<16;i++) {
        HGNativeVoice *v=&s->mixer.voices[i]; u8 *channel=out+i*0x54;
        channel[0]=(u8)i; channel[2]=v->active ? 2 : 3; channel[3]=v->active;
        channel[0x23]=v->pan; store16(channel+0x24,(u16)(v->volume|v->shift<<8));
        store32(out+0x1180+i*4,v->volume|((u32)v->shift<<8)|((u32)v->pan<<16)|((u32)v->active<<31));
        memcpy(out+0x540+i*36,s->players[i],36);
    }
    store32(out+0x11C0,offset); store32(out+0x11C4,s->locked);
}
static void execute(struct w2c_env *env,const SoundCommand *c) {
    struct HGNativeSoundContext *s=env->sound; const u32 *a=c->arg;
    switch (c->id) {
    case 0x1D: require(!(a[0]&3)); (void)span(env,a[0],0x280); s->shared=a[0]; break;
    case 0x01: // Stopping an inactive sequence is valid; sequence preparation remains unsupported.
        require(a[0]<16); s->players[a[0]][0]=0; break;
    case 0x06:
        require(a[0]<16 && (a[3]==1 || a[3]==2 || a[3]==4) && a[1]<=36-a[3]);
        memcpy(s->players[a[0]]+a[1],&a[2],a[3]); break;
    case 0x07:
        require((a[0]&0xFFFFFF)<16 && (a[0]>>24)>=1 && (a[0]>>24)<=4 && a[2]<=64-(a[0]>>24)); break;
    case 0x08: case 0x09: require(a[0]<16 && !(a[1]&~0xFFFFu)); break;
    case 0x0A: require(s->shared && a[0]<16 && a[1]<16); store16(span(env,s->shared+0x20+a[0]*36+a[1]*2,2),(u16)a[2]); break;
    case 0x0B: require(s->shared && a[0]<16); store16(span(env,s->shared+0x260+a[0]*2,2),(u16)a[1]); break;
    case 0x0C: case 0x0D:
        if (a[1] || a[2] || a[3]) unsupported(c->id);
        require(c->id==0x0C ? HGNativeMixerStart(&s->mixer,a[0]) : HGNativeMixerStop(&s->mixer,a[0])); break;
    case 0x0E: setupPCM(env,a); break;
    case 0x13: require(HGNativeMixerTimer(&s->mixer,a[0],a[1])); break;
    case 0x14: case 0x15:
        require(!(a[0]&~0xFFFFu) && a[1]<=127 && (c->id!=0x14 || a[2]<=3));
        for (unsigned i=0;i<16;i++) if (a[0]&(1u<<i)) {
            HGNativeVoice *v=&s->mixer.voices[i];
            require(HGNativeMixerControl(&s->mixer,1u<<i,c->id==0x14 ? a[1] : v->volume,
                c->id==0x14 ? a[2] : v->shift,c->id==0x15 ? a[1] : v->pan));
        } break;
    case 0x17: require(HGNativeMixerMaster(&s->mixer,a[0],s->mixer.masterPan)); break;
    case 0x18: require(HGNativeMixerMaster(&s->mixer,s->mixer.masterVolume,(s32)a[0])); break;
    case 0x19: if (a[0] || a[1] || a[2] || a[3]) unsupported(c->id); break;
    case 0x1A: case 0x1B:
        require(!(a[0]&~0xFFFFu)); if (a[1]) unsupported(c->id);
        if (c->id==0x1A) { s->locked|=a[0]; require(HGNativeMixerStop(&s->mixer,a[0])); }
        else s->locked&=~a[0]; break;
    case 0x1C:
        if (a[1]) unsupported(c->id);
        require(!(a[0]&~0xFFFFu) && HGNativeMixerStop(&s->mixer,a[0]&~s->locked)); break;
    case 0x1E: case 0x1F: case 0x20:
        require(a[0]<=a[1]); (void)span(env,a[0],a[1]-a[0]);
        if (c->id==0x20) for (unsigned i=0;i<16;i++)
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
            require(HGNativeMixerInit(&env->sound->mixer,48000));
            for (unsigned i=0;i<16;i++) memset(env->sound->players[i]+8,255,16);
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
    while (s->head) {
        SoundBatch *batch=s->head;
        for (u32 i=0;i<batch->count;i++) execute(env,&batch->commands[i]);
        require(s->shared!=0); s->finished++; store32(span(env,s->shared,4),s->finished); status(env);
        s->head=batch->next; if (!s->head) s->tail=NULL; s->count--; free(batch);
    }
    return TRUE;
}
int HGNativeSoundRender(struct w2c_env *env,int16_t *output,size_t frames) {
    if (!env || env->resetRequested || !env->sound) return FALSE;
    (void)w2c_env_HGNativeSoundRequest(env,HG_SOUND_PUMP,0);
    int result=HGNativeMixerRender(&env->sound->mixer,output,frames); if (result) status(env); return result;
}
void HGNativeCloseSoundContext(struct w2c_env *env) {
    if (!env->sound) return;
    for (unsigned i=0;i<16;i++) free(env->sound->waves[i]);
    SoundBatch *batch=env->sound->head;
    while (batch) { SoundBatch *next=batch->next; free(batch); batch=next; }
    free(env->sound); env->sound=NULL;
}
