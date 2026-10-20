// SPDX-License-Identifier: GPL-3.0-or-later
#include "../NativeSynth.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(v) do { if (!(v)) { fprintf(stderr,"Synth check line %d: %s\n",__LINE__,#v); abort(); } } while (0)
static void put16(uint8_t *p,unsigned v) { p[0]=(uint8_t)v; p[1]=(uint8_t)(v>>8); }
static void put32(uint8_t *p,uint32_t v) { for (unsigned i=0;i<4;i++) p[i]=(uint8_t)(v>>(i*8)); }
static uint32_t get32(FILE *f) { uint8_t b[4]; CHECK(fread(b,1,4,f)==4); return b[0]|(uint32_t)b[1]<<8|(uint32_t)b[2]<<16|(uint32_t)b[3]<<24; }
static void header(uint8_t *p,const char *magic,unsigned size) {
    memcpy(p,magic,4); p[4]=255; p[5]=254; p[7]=1; put32(p+8,size); p[12]=16; p[14]=1;
    memcpy(p+16,"DATA",4); put32(p+20,size-16);
}
static void fixture(uint8_t *memory,unsigned type,unsigned relocated) {
    memset(memory,0,1024); uint8_t *bank=memory+64,*arc=memory+256,*wave=memory+320,*note=bank+64;
    header(bank,"SBNK",80); put32(bank+24,256); put32(bank+56,1); put32(bank+60,64*256+type);
    note[4]=60; note[5]=note[6]=note[7]=note[8]=127; note[9]=64;
    header(arc,"SWAR",84); put32(arc+56,1); put32(arc+60,64);
    wave[0]=1; wave[1]=1; put16(wave+2,32000); put16(wave+4,512); put32(wave+8,2);
    const int16_t pcm[]={1000,-1000,2000,-2000}; memcpy(wave+12,pcm,8);
    if (relocated) { memcpy(memory+512,wave,20); put32(arc+60,0x80000200u); }
    const uint8_t seq[]={60,127,2,0x80,1,0xFF}; memcpy(memory+768,seq,sizeof(seq));
}
static unsigned framesAt(unsigned ticks) { return (unsigned)(((uint64_t)ticks*48000*HG_SEQUENCE_TICK_CLOCKS+HG_SEQUENCE_CLOCK-1)/HG_SEQUENCE_CLOCK); }
static void prepare(HGNativeSynth *s,uint8_t *memory) {
    CHECK(HGNativeSynthInit(s,48000,memory,1024));
    CHECK(HGNativeSequencePrepare(&s->sequence,0,memory+768,6,0,64) && HGNativeSequenceStart(&s->sequence,0));
}
static void synthetic(void) {
    uint8_t memory[1024]; fixture(memory,1,0);
    HGNativeSynth *s=calloc(1,sizeof(*s)),*other=calloc(1,sizeof(*other)); CHECK(s && other); prepare(s,memory); prepare(other,memory);
    unsigned n=framesAt(4); int16_t *out=calloc(n*2+8000,2),*split=calloc(n*2+8000,2); CHECK(out && split);
    CHECK(HGNativeSynthRender(s,out,n)); for (unsigned i=0;i<n*2;i++) CHECK(!out[i]);
    unsigned latency=(3u*512*48000+16756991u-1)/16756991u;
    CHECK(HGNativeSynthRender(s,out,latency)); for (unsigned i=0;i<latency*2;i++) CHECK(!out[i]);
    CHECK(HGNativeSynthRender(s,out,1) && out[0]==492 && out[1]==500);
    CHECK(s->voices[4].state==HG_SYNTH_DECAY && s->voices[4].amplitude==0 && s->voices[4].track>=0);
    HGNativeSynthClose(s); prepare(s,memory);
    CHECK(HGNativeSynthRender(s,out,n+1000));
    for (unsigned i=0;i<n+1000;i+=7) { unsigned count=n+1000-i; if (count>7) count=7; CHECK(HGNativeSynthRender(other,split+i*2,count)); }
    CHECK(!memcmp(out,split,(n+1000)*4) && s->phase==other->phase);
    CHECK(HGNativeSynthRender(s,out,2000) && !HGNativeMixerActive(&s->mixer));
    HGNativeSynthClose(s); HGNativeSynthClose(other);
    fixture(memory,1,1); prepare(s,memory); CHECK(HGNativeSynthRender(s,out,n+latency+1) && out[(n+latency)*2]==492);
    CHECK(HGNativeSynthInvalidate(s,520,520,2) && HGNativeMixerActive(&s->mixer) && s->cache);
    CHECK(HGNativeSynthInvalidate(s,512,532,2) && !HGNativeMixerActive(&s->mixer) && !s->cache);
    CHECK(HGNativeSynthRender(s,out,20)); for (unsigned i=0;i<40;i++) CHECK(!out[i]);
    HGNativeSynthClose(s); fixture(memory,1,0); memory[64+64+5]=0; prepare(s,memory);
    CHECK(HGNativeSynthRender(s,out,n) && s->voices[4].state==HG_SYNTH_ATTACK && s->voices[4].amplitude==-92182);
    HGNativeSynthClose(s); fixture(memory,1,0); memory[64+64+5]=126; prepare(s,memory);
    CHECK(HGNativeSynthRender(s,out,n) && s->voices[4].amplitude==-361);
    HGNativeSynthClose(s); fixture(memory,1,0); prepare(s,memory); s->locked=1u<<4;
    CHECK(HGNativeSynthRender(s,out,n) && s->voices[4].track<0 && s->voices[5].track>=0);
    CHECK(HGNativeSynthPause(s,0,1)); CHECK(HGNativeSynthRender(s,out,1000) && !HGNativeMixerActive(&s->mixer));
    HGNativeSynthClose(s);
    for (unsigned type=2;type<=3;type++) {
        fixture(memory,type,0); prepare(s,memory); CHECK(HGNativeSynthRender(s,out,n+100));
        unsigned mask=HGNativeMixerActive(&s->mixer); CHECK(type==2 ? (mask&0x3F00)!=0 : (mask&0xC000)!=0);
        CHECK(type==2 ? !(mask&~0x3F00u) : !(mask&~0xC000u)); HGNativeSynthClose(s);
    }
    fixture(memory,1,0); prepare(s,memory); put32(memory+256+60,UINT32_MAX);
    CHECK(!HGNativeSynthRender(s,out,n) && s->error==1); HGNativeSynthClose(s);
    fixture(memory,1,0); prepare(s,memory); CHECK(!HGNativeSynthRender(s,(int16_t *)(memory+768),10));
    CHECK(HGNativeSynthInvalidate(s,0,1024,0) && !s->sequence.players[0].active); HGNativeSynthClose(s);
    CHECK(HGNativeSynthTimer(512,0)==512 && HGNativeSynthTimer(512,768)==256 && HGNativeSynthTimer(512,-768)==1024);
    CHECK(HGNativeSynthTimer(512,INT32_MAX)==16 && HGNativeSynthTimer(512,INT32_MIN)==65535);
    free(out); free(split); free(s); free(other);
    puts("PASS: native sequence-to-PCM, exact gain/timing, split-block rendering, ADSR boundaries, wave relocation/invalidation, locks, pause, PSG/noise and rejected resources.");
}
int main(int argc,char **argv) {
    synthetic(); CHECK(argc==2); FILE *f=fopen(argv[1],"rb"); CHECK(f); unsigned records=get32(f),audible=0; uint64_t samples=0;
    int16_t *pcm=malloc(6000*4); HGNativeSynth *s=malloc(sizeof(*s)); CHECK(pcm && s);
    for (unsigned i=0;i<records;i++) {
        uint32_t bytes=get32(f),seq=get32(f),length=get32(f),bank=get32(f),volume=get32(f),priority=get32(f); CHECK(bytes && bytes<16777216 && seq<bytes && length<=bytes-seq && priority<=255);
        uint8_t *memory=malloc(bytes); CHECK(memory && fread(memory,1,bytes,f)==bytes);
        CHECK(HGNativeSynthInit(s,48000,memory,bytes) && HGNativeSequencePrepare(&s->sequence,0,memory+seq,length,0,bank) && HGNativeSequenceStart(&s->sequence,0));
        s->sequence.players[0].extFader=(int16_t)volume; s->sequence.players[0].priority=(uint8_t)priority;
        if (!HGNativeSynthRender(s,pcm,6000)) {
            fprintf(stderr,"Original sound %u failed: synth=%u seq=%u track=%u position=%u cmd=%u\n",i,s->error,s->sequence.error.code,s->sequence.error.track,s->sequence.error.position,s->sequence.error.command); return 1;
        }
        int nonzero=0; for (unsigned n=0;n<12000;n++) nonzero|=pcm[n]!=0; audible+=(unsigned)nonzero;
        samples+=6000; HGNativeSynthClose(s); free(memory);
    }
    unsigned math=get32(f);
    for (unsigned i=0;i<math;i++) {
        unsigned base=get32(f); int32_t pitch=(int32_t)get32(f); unsigned want=get32(f);
        CHECK(HGNativeSynthTimer((uint16_t)base,pitch)==want);
    }
    CHECK(audible>records/2 && fgetc(f)==EOF && !ferror(f)); fclose(f); free(pcm); free(s);
    printf("PASS: %u original sequence/bank/wave sets rendered to %llu stereo frames; %u non-silent starts. Complete-song/device output unverified.\n",records,(unsigned long long)samples,audible);
    printf("PASS: %u original-table pitch timer cases, including extreme inputs.\n",math);
    return 0;
}
