// SPDX-License-Identifier: GPL-3.0-or-later
#include "../NativeSequence.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(v) do { if (!(v)) { fprintf(stderr,"Sequence check line %d: %s\n",__LINE__,#v); abort(); } } while (0)
typedef struct Trace { HGNativeSequence *engine; unsigned tick,notes,kills,rejectKillAt; uint64_t hash; uint8_t keys[16],steps[16],tracks[16]; int fail; } Trace;
static int event(void *context,const HGSequenceEvent *note) {
    Trace *t=context;
    if (note->kind==HG_SEQUENCE_KILL) { if (t->kills+1==t->rejectKillAt) return 0; t->kills++; }
    if (note->kind!=HG_SEQUENCE_NOTE) return 1;
    if (t->fail) return 0;
    if (t->notes<16) { t->keys[t->notes]=note->key; t->steps[t->notes]=(uint8_t)t->tick; t->tracks[t->notes]=t->engine->tracks[note->track].number; }
    t->notes++;
    t->hash=(t->hash^note->key)*1099511628211ULL; t->hash=(t->hash^note->velocity)*1099511628211ULL;
    t->hash=(t->hash^(uint32_t)note->duration)*1099511628211ULL;
    t->hash=(t->hash^t->engine->tracks[note->track].program)*1099511628211ULL;
    return 1;
}
static void fresh(HGNativeSequence *e,Trace *t) { *t=(Trace){.engine=e}; HGNativeSequenceInit(e,event,t); }
static void step(HGNativeSequence *e,Trace *t,unsigned tick) { t->tick=tick; e->players[0].counter=240; e->players[0].tempo=0; CHECK(HGNativeSequenceTick(e)); }
static void put24(uint8_t *p,unsigned v) { for (unsigned i=0;i<3;i++) p[i]=(uint8_t)(v>>(i*8)); }
static void synthetic(void) {
    HGNativeSequence e; Trace t; fresh(&e,&t);
    uint8_t score[]={0xFE,3,0,0x93,1,0,0,0,0xC1,100,0x81,1,60,100,2,0xD4,2,0x95,0,0,0,0xFC,0xFF,
        0xC0,127,64,80,1,0xFF,0x80,1,62,90,1,0xFD};
    put24(score+5,23); put24(score+18,29);
    CHECK(HGNativeSequencePrepare(&e,0,score,sizeof(score),0,99));
    HGSequencePlayer unchanged; memcpy(&unchanged,&e.players[0],sizeof(unchanged));
    uint8_t badHead[]={0x93,1,255,255,255,0xFF};
    CHECK(!HGNativeSequencePrepare(&e,0,badHead,sizeof(badHead),0,0) && !memcmp(&unchanged,&e.players[0],sizeof(unchanged)));
    CHECK(e.players[0].prepared && e.players[0].bank==99 && e.tracks[e.players[0].tracks[1]].active);
    for (unsigned i=0;i<4;i++) CHECK(HGNativeSequenceTick(&e)); CHECK(!t.notes);
    CHECK(HGNativeSequenceStart(&e,0));
    CHECK(HGNativeSequenceTick(&e) && HGNativeSequenceTick(&e) && !t.notes);
    CHECK(HGNativeSequenceTick(&e) && t.notes==2);
    CHECK(HGNativeSequencePause(&e,0,1)); unsigned before=t.notes,counter=e.players[0].counter;
    for (unsigned i=0;i<10;i++) CHECK(HGNativeSequenceTick(&e));
    CHECK(t.notes==before && e.players[0].counter==counter && HGNativeSequencePause(&e,0,0));
    for (unsigned i=1;i<7;i++) step(&e,&t,i);
    const uint8_t keys[]={60,64,62,62},ticks[]={0,0,3,5},tracks[]={0,1,0,0};
    CHECK(t.notes==4 && !memcmp(t.keys,keys,4) && !memcmp(t.steps,ticks,4) && !memcmp(t.tracks,tracks,4));
    CHECK(HGNativeSequenceEnded(&e,0) && e.tracks[e.players[0].tracks[0]].volume==100 && e.tracks[e.players[0].tracks[1]].pan==63);
    uint8_t vars[]={0xB0,0,255,127,0xB1,0,1,0,0xB5,0,239,255,0xB4,0,0,0,0xB3,0,0,128,
        0xB8,0,0,128,0xA2,0xC1,42,0xB8,0,0,0,0xA2,0xA0,0xC1,90,0,90,0,0xA0,0xC0,64,0,64,0,
        0xB0,16,123,0,0xA1,0xC2,16,0xB0,1,1,0,0xA1,60,100,1,0xA0,61,90,2,0,2,0,0xD7,1,62,100,1,0xD7,0,0xD6,16,0xFF};
    fresh(&e,&t); CHECK(HGNativeSequencePrepare(&e,0,vars,sizeof(vars),0,0) && HGNativeSequenceStart(&e,0));
    for (unsigned i=0;i<5;i++) step(&e,&t,i);
    CHECK(t.notes==2 && t.keys[0]==60 && t.keys[1]==61 && t.steps[0]==0 && t.steps[1]==1);
    CHECK(e.players[0].variables[0]==-32768 && e.globals[0]==123 && e.players[0].volume==123);
    CHECK(e.tracks[e.players[0].tracks[0]].volume==42 && HGNativeSequenceEnded(&e,0));
    uint32_t seed=0x12345678u; seed=seed*1664525u+1013904223u; seed=seed*1664525u+1013904223u;
    CHECK(e.random==seed);
    uint8_t all[]={0xFE,255,255,0xFF};
    CHECK(HGNativeSequencePrepare(&e,0,all,sizeof(all),0,0) && HGNativeSequencePrepare(&e,1,all,sizeof(all),0,0));
    CHECK(!HGNativeSequencePrepare(&e,2,all,sizeof(all),0,0) && e.players[0].active && e.players[1].active);
    t.rejectKillAt=t.kills+2; CHECK(!HGNativeSequenceStop(&e,0));
    CHECK(e.players[0].tracks[0]==255 && e.players[0].trackMask==0xFFFE && e.players[0].active);
    for (unsigned n=1;n<16;n++) CHECK(e.tracks[e.players[0].tracks[n]].allocated && e.tracks[e.players[0].tracks[n]].player==0);
    CHECK(!HGNativeSequencePrepare(&e,2,all,sizeof(all),0,0)); t.rejectKillAt=0;
    CHECK(HGNativeSequenceStop(&e,0) && HGNativeSequencePrepare(&e,2,all,sizeof(all),0,0) && e.globals[0]==123);
    fresh(&e,&t); uint8_t bad[][8]={{0x82},{0x94,255,255,255},{0xFD},{0x95,0,0,0},{0xA1,0xC1,32},{0x80,0xFF,0xFF,0xFF,0xFF,0xFF},{0x94,0,0,0},{0xA0,0xC1,2,0,1,0}};
    const unsigned sizes[]={1,4,1,4,3,6,4,6},errors[]={HG_SEQUENCE_COMMAND,HG_SEQUENCE_BOUNDS,HG_SEQUENCE_STACK,HG_SEQUENCE_STACK,HG_SEQUENCE_VARIABLE,HG_SEQUENCE_BOUNDS,HG_SEQUENCE_LIMIT,HG_SEQUENCE_BOUNDS};
    for (unsigned i=0;i<sizeof(sizes)/sizeof(*sizes);i++) {
        fresh(&e,&t); CHECK(HGNativeSequencePrepare(&e,0,bad[i],sizes[i],0,0) && HGNativeSequenceStart(&e,0));
        e.players[0].counter=240; CHECK(!HGNativeSequenceTick(&e) && e.error.code==errors[i]);
    }
    fresh(&e,&t); uint8_t note[]={60,100,1,0xFF}; t.fail=1;
    CHECK(HGNativeSequencePrepare(&e,0,note,sizeof(note),0,0) && HGNativeSequenceStart(&e,0));
    e.players[0].counter=240; CHECK(!HGNativeSequenceTick(&e) && e.error.code==HG_SEQUENCE_CALLBACK);
    puts("PASS: native sequence timing, multi-track prepare, calls/loops, variables, prefixes/conditionals, mute, pause, shared track limits and bounded failures.");
}
static uint32_t read32(FILE *f) { uint8_t b[4]; CHECK(fread(b,1,4,f)==4); return b[0]|(uint32_t)b[1]<<8|(uint32_t)b[2]<<16|(uint32_t)b[3]<<24; }
int main(int argc,char **argv) {
    synthetic(); CHECK(argc==2); FILE *f=fopen(argv[1],"rb"); CHECK(f); uint32_t count=read32(f); uint64_t total=0,hash=0;
    for (uint32_t i=0;i<count;i++) {
        uint32_t size=read32(f),start=read32(f); CHECK(size && size<16777216 && start<size);
        uint8_t *data=malloc(size); CHECK(data && fread(data,1,size,f)==size);
        HGNativeSequence e; Trace t; fresh(&e,&t);
        CHECK(HGNativeSequencePrepare(&e,0,data,size,start,0) && HGNativeSequenceStart(&e,0));
        for (unsigned tick=0;tick<32768 && !HGNativeSequenceEnded(&e,0);tick++) {
            t.tick=tick;
            if (!HGNativeSequenceTick(&e)) {
                fprintf(stderr,"Original sequence %u failed: code=%u track=%u position=%u command=0x%02X\n",i,e.error.code,e.error.track,e.error.position,e.error.command); return 1;
            }
        }
        total+=t.notes; hash^=t.hash; CHECK(HGNativeSequenceStop(&e,0)); free(data);
    }
    CHECK(fgetc(f)==EOF && !ferror(f)); fclose(f);
    printf("PASS: %u original sequences scheduled for up to 32768 driver clocks each; %llu note events, trace=%016llX. PCM/output not covered.\n",count,(unsigned long long)total,(unsigned long long)hash);
    return 0;
}
