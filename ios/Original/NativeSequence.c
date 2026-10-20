// SPDX-License-Identifier: GPL-3.0-or-later
#include "NativeSequence.h"
#include <string.h>
#include <limits.h>

typedef struct Reader { HGNativeSequence *engine; HGSequencePlayer *player; HGSequenceTrack *track; uint32_t command; } Reader;
static int error(Reader *r,unsigned code) {
    r->engine->error.code=code; r->engine->error.player=r->track->player;
    r->engine->error.track=r->track->number; r->engine->error.position=r->track->position;
    r->engine->error.command=r->command; return 0;
}
static int readValue(Reader *r,unsigned bytes,uint32_t *out) {
    uint32_t at=r->track->position;
    if (at>r->player->bytes || bytes>r->player->bytes-at) return error(r,HG_SEQUENCE_BOUNDS);
    uint32_t value=0;
    for (unsigned i=0;i<bytes;i++) value|=(uint32_t)r->player->data[at+i]<<(i*8);
    r->track->position+=bytes; *out=value; return 1;
}
static int variableLength(Reader *r,uint32_t *out) {
    uint32_t value=0,part;
    for (unsigned i=0;i<5;i++) {
        if (!readValue(r,1,&part)) return 0;
        if (value>(UINT32_MAX-(part&127))/128) return error(r,HG_SEQUENCE_BOUNDS);
        value=value*128+(part&127);
        if (!(part&128)) { *out=value; return 1; }
    }
    return error(r,HG_SEQUENCE_BOUNDS);
}
static int16_t *variable(Reader *r,uint32_t index) {
    if (index<16) return &r->player->variables[index];
    if (index<32) return &r->engine->globals[index-16];
    error(r,HG_SEQUENCE_VARIABLE); return NULL;
}
static uint32_t randomValue(HGNativeSequence *e) { e->random=e->random*1664525u+1013904223u; return e->random>>16; }
static int emit(HGNativeSequence *e,unsigned kind,unsigned track,unsigned key,unsigned velocity,int32_t duration,unsigned tie) {
    HGSequenceEvent event={(uint8_t)kind,(uint8_t)track,(uint8_t)key,(uint8_t)velocity,(uint8_t)tie,duration};
    if (e->callback && e->callback(e->context,&event)) return 1;
    e->error.code=HG_SEQUENCE_CALLBACK; e->error.player=e->tracks[track].player;
    e->error.track=e->tracks[track].number; e->error.position=e->tracks[track].position; return 0;
}
static void initTrack(HGSequenceTrack *t,unsigned player,unsigned number) {
    memset(t,0,sizeof(*t)); t->allocated=1; t->player=(uint8_t)player; t->number=(uint8_t)number;
    t->noteWait=t->comparison=1; t->channels=0xFFFF; t->portaKey=60;
    t->volume=t->expression=127; t->bendRange=2; t->priority=64; t->panRange=127;
    t->attack=t->decay=t->sustain=t->release=255;
    t->modRange=1; t->modSpeed=16; t->modDelay=10;
}
static unsigned bits(unsigned value) { unsigned count=0; for (;value;value&=value-1) count++; return count; }
static int reserve(HGNativeSequence *e,unsigned player,uint16_t mask) {
    HGSequencePlayer *p=&e->players[player]; unsigned free=0,missing=mask&~p->trackMask;
    for (unsigned i=0;i<32;i++) free+=!e->tracks[i].allocated;
    if (bits(missing)>free) return 0;
    for (unsigned number=0;number<16;number++) if (missing&(1u<<number)) {
        unsigned id=0; while (e->tracks[id].allocated) id++;
        initTrack(&e->tracks[id],player,number); p->tracks[number]=(uint8_t)id;
    }
    p->trackMask|=mask; return 1;
}
void HGNativeSequenceInit(HGNativeSequence *e,HGSequenceCallback callback,void *context) {
    if (!e) return;
    memset(e,0,sizeof(*e)); e->random=0x12345678; e->callback=callback; e->context=context;
    for (unsigned p=0;p<16;p++) { memset(e->players[p].tracks,255,16); for (unsigned v=0;v<16;v++) e->players[p].variables[v]=-1; }
    for (unsigned v=0;v<16;v++) e->globals[v]=-1;
}
int HGNativeSequenceStop(HGNativeSequence *e,unsigned player) {
    if (!e || player>=16) return 0;
    HGSequencePlayer *p=&e->players[player];
    for (unsigned i=0;i<16;i++) if (p->tracks[i]!=255) {
        unsigned id=p->tracks[i]; if (!emit(e,HG_SEQUENCE_KILL,id,0,0,0,0)) return 0;
        memset(&e->tracks[id],0,sizeof(e->tracks[id]));
        p->tracks[i]=255; p->trackMask&=(uint16_t)~(1u<<i);
    }
    p->active=p->prepared=p->paused=0; p->trackMask=0; p->data=NULL; p->bytes=0; memset(p->tracks,255,16); return 1;
}
static int rejectPrepare(HGNativeSequence *e,unsigned code) { e->error.code=code; return 0; }
int HGNativeSequencePrepare(HGNativeSequence *e,unsigned player,const void *data,size_t bytes,uint32_t start,uint32_t bank) {
    if (!e) return 0;
    if (!e->callback || player>=16 || !data || !bytes || bytes>UINT32_MAX || start>=bytes) return rejectPrepare(e,HG_SEQUENCE_BOUNDS);
    const uint8_t *raw=data; uint16_t mask=1; uint32_t position=start;
    uint32_t starts[16]; for (unsigned i=0;i<16;i++) starts[i]=UINT32_MAX;
    if (raw[start]==0xFE) {
        if (bytes-start<3) return rejectPrepare(e,HG_SEQUENCE_BOUNDS);
        mask|=(uint16_t)(raw[start+1]|(uint16_t)raw[start+2]<<8); position+=3;
        if (position>=bytes) return rejectPrepare(e,HG_SEQUENCE_BOUNDS);
    }
    while (raw[position]==0x93) {
        if (bytes-position<5 || raw[position+1]>=16) return rejectPrepare(e,HG_SEQUENCE_BOUNDS);
        unsigned number=raw[position+1];
        uint32_t target=raw[position+2]|(uint32_t)raw[position+3]<<8|(uint32_t)raw[position+4]<<16;
        if (target>=bytes) return rejectPrepare(e,HG_SEQUENCE_BOUNDS);
        starts[number]=target; mask|=(uint16_t)(1u<<number); position+=5;
        if (position>=bytes) return rejectPrepare(e,HG_SEQUENCE_BOUNDS);
    }
    unsigned available=bits(e->players[player].trackMask);
    for (unsigned i=0;i<32;i++) available+=!e->tracks[i].allocated;
    if (bits(mask)>available) return rejectPrepare(e,HG_SEQUENCE_LIMIT);
    if (!HGNativeSequenceStop(e,player)) return 0;
    HGSequencePlayer *p=&e->players[player]; p->data=raw; p->bytes=bytes; p->bank=bank;
    p->active=p->prepared=1; p->paused=0; p->tempo=120; p->ratio=256; p->counter=0;
    p->priority=64; p->volume=127; p->extFader=0; p->ticks=0;
    for (unsigned v=0;v<16;v++) p->variables[v]=-1;
    if (!reserve(e,player,mask)) return rejectPrepare(e,HG_SEQUENCE_LIMIT);
    for (unsigned n=0;n<16;n++) if (starts[n]!=UINT32_MAX) {
        HGSequenceTrack *t=&e->tracks[p->tracks[n]]; t->active=1; t->position=starts[n];
    }
    HGSequenceTrack *t=&e->tracks[p->tracks[0]]; t->active=1; t->position=position;
    memset(&e->error,0,sizeof(e->error)); return 1;
}
int HGNativeSequenceStart(HGNativeSequence *e,unsigned player) {
    if (!e || player>=16 || !e->players[player].active) return 0;
    e->players[player].prepared=0; return 1;
}
int HGNativeSequencePause(HGNativeSequence *e,unsigned player,int paused) {
    if (!e || player>=16 || !e->players[player].active || (paused!=0 && paused!=1)) return 0;
    e->players[player].paused=(uint8_t)paused; return 1;
}
int HGNativeSequenceEnded(const HGNativeSequence *e,unsigned player) {
    if (!e || player>=16 || !e->players[player].active) return 1;
    for (unsigned n=0;n<16;n++) {
        unsigned id=e->players[player].tracks[n];
        if (id!=255 && e->tracks[id].active && !e->tracks[id].ended) return 0;
    }
    return 1;
}
static int jump(Reader *r,uint32_t destination) {
    if (destination>=r->player->bytes) return error(r,HG_SEQUENCE_BOUNDS);
    r->track->position=destination; return 1;
}
static int runTrack(HGNativeSequence *e,unsigned id) {
    HGSequenceTrack *t=&e->tracks[id]; HGSequencePlayer *p=&e->players[t->player]; Reader r={e,p,t,0};
    if (!emit(e,HG_SEQUENCE_STEP,id,0,0,0,0)) return 0;
    if (t->ended || t->wait<0) return 1;
    if (t->wait && --t->wait) return 1;
    // shortcut: reject zero-time command loops after 65536 commands; raise only for a verified longer score tick.
    for (unsigned operations=0;operations<65536;operations++) {
        uint32_t command,value=0,extra=0; int override=0,execute=1; int32_t argument=0;
        if (!readValue(&r,1,&command)) return 0;
        r.command=command;
        while (command==0xA2) { execute=execute && t->comparison; if (!readValue(&r,1,&command)) return 0; }
        if (command==0xA0 || command==0xA1) {
            uint32_t prefix=command; override=1;
            if (!readValue(&r,1,&command)) return 0;
            r.command=command;
            if ((command<128 || (command>=0xB0 && command<=0xBD)) && !readValue(&r,1,&extra)) return 0;
            if (prefix==0xA0) {
                uint32_t lo,hi; if (!readValue(&r,2,&lo) || !readValue(&r,2,&hi)) return 0;
                int32_t low=(int16_t)lo,high=(int16_t)hi;
                if (low>high) return error(&r,HG_SEQUENCE_BOUNDS);
                if (execute) argument=low+(int32_t)(randomValue(e)%(uint32_t)(high-low+1));
            } else {
                uint32_t index; if (!readValue(&r,1,&index)) return 0;
                int16_t *v=variable(&r,index); if (!v) return 0; argument=*v;
            }
        }
        r.command=command;
        if (override && !(command<128 || command==0x80 || command==0x81 ||
            (command>=0xB0 && command<=0xBD && command!=0xB7) || (command>=0xC0 && command<=0xD7) ||
            command==0xE0 || command==0xE1 || command==0xE3)) return error(&r,HG_SEQUENCE_COMMAND);
        if (command<128 || command==0x80 || command==0x81) {
            if (command<128 && !override && !readValue(&r,1,&extra)) return 0;
            if (!override) { if (!variableLength(&r,&value)) return 0; if (value>INT32_MAX) return error(&r,HG_SEQUENCE_BOUNDS); argument=(int32_t)value; }
        } else if ((command>=0xB0 && command<=0xBD && command!=0xB7) || command==0x93) {
            if (!override && !readValue(&r,1,&extra)) return 0;
            if (!override) { if (!readValue(&r,command==0x93 ? 3 : 2,&value)) return 0; argument=command==0x93 ? (int32_t)value : (int16_t)value; }
        } else if (command==0x94 || command==0x95) {
            if (override || !readValue(&r,3,&value)) return error(&r,HG_SEQUENCE_COMMAND);
            argument=(int32_t)value;
        } else if (command==0xE0 || command==0xE1 || command==0xE3 || command==0xFE) {
            if (!override) { if (!readValue(&r,2,&value)) return 0; argument=command==0xE3 ? (int16_t)value : (int32_t)value; }
        } else if (command>=0xC0 && command<=0xD7) {
            if (!override) { if (!readValue(&r,1,&value)) return 0; argument=(int32_t)value; }
        } else if (command!=0xFC && command!=0xFD && command!=0xFF) return error(&r,HG_SEQUENCE_COMMAND);
        if (!execute) continue;
        if (command<128) {
            int key=(int)command+t->transpose;
            if (extra>127) return error(&r,HG_SEQUENCE_BOUNDS);
            if (t->noteWait) t->wait=argument;
            if (key>=0 && key<=127 && !t->muted && !emit(e,HG_SEQUENCE_NOTE,id,(unsigned)key,extra,argument,t->tie)) return 0;
            t->portaKey=(uint8_t)key;
        } else if (command>=0xB0 && command<=0xBD) {
            int16_t *v=variable(&r,extra); if (!v) return 0; int32_t current=*v,result=current;
            switch (command) {
            case 0xB0: result=argument; break;
            case 0xB1: result=current+argument; break;
            case 0xB2: result=current-argument; break;
            case 0xB3: result=current*argument; break;
            case 0xB4: if (argument) result=current/argument; break;
            case 0xB5:
                if (argument>=0) result=argument>=16 ? 0 : (int32_t)((uint32_t)(uint16_t)current<<argument);
                else { unsigned n=(unsigned)-argument; result=n>=16 ? (current<0 ? -1 : 0) : (current<0 ? -(((-current)+(1<<n)-1)>>n) : current>>n); }
                break;
            case 0xB6: { uint32_t limit=(uint32_t)(argument<0 ? -argument : argument)+1; result=(int32_t)(randomValue(e)%limit); if (argument<0) result=-result; break; }
            case 0xB8: t->comparison=current==argument; break;
            case 0xB9: t->comparison=current>=argument; break;
            case 0xBA: t->comparison=current>argument; break;
            case 0xBB: t->comparison=current<=argument; break;
            case 0xBC: t->comparison=current<argument; break;
            case 0xBD: t->comparison=current!=argument; break;
            }
            if (command<=0xB6) *v=(int16_t)(uint16_t)result;
        } else switch (command) {
        case 0x80: t->wait=argument; break;
        case 0x81: t->program=(uint16_t)argument; break;
        case 0x93: {
            if (extra>=16 || argument<0 || (uint32_t)argument>=p->bytes) return error(&r,HG_SEQUENCE_BOUNDS);
            if (!reserve(e,t->player,(uint16_t)(1u<<extra))) return error(&r,HG_SEQUENCE_LIMIT);
            unsigned target=p->tracks[extra];
            if (!emit(e,HG_SEQUENCE_KILL,target,0,0,0,0)) return 0;
            initTrack(&e->tracks[target],t->player,extra); e->tracks[target].active=1; e->tracks[target].position=(uint32_t)argument; break;
        }
        case 0x94: if (!jump(&r,(uint32_t)argument)) return 0; break;
        case 0x95:
            if (t->depth==3) return error(&r,HG_SEQUENCE_STACK);
            t->stackType[t->depth]=0; t->stack[t->depth++]=t->position;
            if (!jump(&r,(uint32_t)argument)) return 0; break;
        case 0xC0: t->pan=(int8_t)(uint8_t)(argument-64); break;
        case 0xC1: t->volume=(uint8_t)argument; break;
        case 0xC2: p->volume=(uint8_t)argument; break;
        case 0xC3: t->transpose=(int8_t)argument; break;
        case 0xC4: t->pitchBend=(int8_t)argument; break;
        case 0xC5: t->bendRange=(uint8_t)argument; break;
        case 0xC6: t->priority=(uint8_t)argument; break;
        case 0xC7: t->noteWait=argument!=0; break;
        case 0xC8: t->tie=argument!=0; if (!emit(e,HG_SEQUENCE_RELEASE,id,0,0,0,0)) return 0; break;
        case 0xC9: t->portaKey=(uint8_t)(argument+t->transpose); t->portamento=1; break;
        case 0xCA: t->modDepth=(uint8_t)argument; break;
        case 0xCB: t->modSpeed=(uint8_t)argument; break;
        case 0xCC: t->modType=(uint8_t)argument; break;
        case 0xCD: t->modRange=(uint8_t)argument; break;
        case 0xCE: t->portamento=argument!=0; break;
        case 0xCF: t->portaTime=(uint8_t)argument; break;
        case 0xD0: t->attack=(uint8_t)argument; break;
        case 0xD1: t->decay=(uint8_t)argument; break;
        case 0xD2: t->sustain=(uint8_t)argument; break;
        case 0xD3: t->release=(uint8_t)argument; break;
        case 0xD4:
            if (t->depth==3) return error(&r,HG_SEQUENCE_STACK);
            t->stackType[t->depth]=1; t->loops[t->depth]=(uint8_t)argument; t->stack[t->depth++]=t->position; break;
        case 0xD5: t->expression=(uint8_t)argument; break;
        case 0xD6: if (!variable(&r,(uint32_t)argument)) return 0; break;
        case 0xD7:
            if (argument<0 || argument>3) return error(&r,HG_SEQUENCE_BOUNDS);
            t->muted=argument!=0;
            if (argument>=2 && !emit(e,argument==3 ? HG_SEQUENCE_KILL : HG_SEQUENCE_RELEASE,id,0,0,0,0)) return 0;
            break;
        case 0xE0: t->modDelay=(uint16_t)argument; break;
        case 0xE1: p->tempo=(uint16_t)argument; break;
        case 0xE3: t->sweepPitch=(int16_t)argument; break;
        case 0xFC:
            if (!t->depth || !t->stackType[t->depth-1]) return error(&r,HG_SEQUENCE_STACK);
            if (!t->loops[t->depth-1] || --t->loops[t->depth-1]) { if (!jump(&r,t->stack[t->depth-1])) return 0; }
            else t->depth--;
            break;
        case 0xFD:
            if (!t->depth || t->stackType[t->depth-1]) return error(&r,HG_SEQUENCE_STACK);
            if (!jump(&r,t->stack[--t->depth])) return 0; break;
        case 0xFE: if (!reserve(e,t->player,(uint16_t)argument|1u)) return error(&r,HG_SEQUENCE_LIMIT); break;
        case 0xFF: t->ended=1; return 1;
        }
        if (t->wait) return 1;
    }
    return error(&r,HG_SEQUENCE_LIMIT);
}
int HGNativeSequenceTick(HGNativeSequence *e) {
    if (!e) return 0;
    memset(&e->error,0,sizeof(e->error));
    for (unsigned player=0;player<16;player++) {
        HGSequencePlayer *p=&e->players[player]; if (!p->active || p->prepared || p->paused) continue;
        while (p->counter>=240) {
            p->counter-=240;
            p->ticks++;
            for (unsigned number=0;number<16;number++) {
                unsigned id=p->tracks[number]; if (id!=255 && e->tracks[id].active && !runTrack(e,id)) return 0;
            }
        }
        p->counter=(uint16_t)(p->counter+((uint32_t)p->tempo*p->ratio>>8));
    }
    return 1;
}
