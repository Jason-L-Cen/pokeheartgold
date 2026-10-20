// SPDX-License-Identifier: GPL-3.0-or-later
#include "../NativeWave.h"
#include "../NativeMixer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(v) do { if (!(v)) { fprintf(stderr,"Native wave check line %d: %s\n",__LINE__,#v); abort(); } } while (0)
static uint32_t read32(FILE *file) {
    uint8_t b[4]; CHECK(fread(b,1,4,file)==4);
    return b[0]|(uint32_t)b[1]<<8|(uint32_t)b[2]<<16|(uint32_t)b[3]<<24;
}
static void header(uint8_t *p,unsigned format,unsigned loop,unsigned start,unsigned tail) {
    memset(p,0,32); p[0]=(uint8_t)format; p[1]=(uint8_t)loop; p[2]=0x80; p[3]=0x3E;
    p[4]=0x17; p[5]=4; p[6]=(uint8_t)start; p[8]=(uint8_t)tail;
}
static void unitChecks(void) {
    uint8_t wave[32]; int16_t out[16]; HGNativeWaveInfo info;
    header(wave,0,1,1,1);
    const uint8_t pcm8[]={0,1,127,128,129,255,42,214}; memcpy(wave+12,pcm8,8);
    const int16_t expected8[]={0,256,32512,-32768,-32512,-256,10752,-10752};
    CHECK(HGNativeWaveDecode(wave,20,out,16,&info) && info.frames==8 && info.loopStart==4);
    CHECK(!memcmp(out,expected8,sizeof(expected8)));
    for (size_t n=0;n<20;n++) {
        HGNativeWaveInfo before; memset(&before,0xA5,sizeof(before)); info=before;
        for (unsigned i=0;i<16;i++) out[i]=1234;
        CHECK(!HGNativeWaveDecode(wave,n,out,16,&info) && !memcmp(&info,&before,sizeof(info)));
        for (unsigned i=0;i<16;i++) CHECK(out[i]==1234);
    }
    CHECK(!HGNativeWaveDecode(wave,20,out,7,&info));
    uint8_t saved[32]; memcpy(saved,wave,32);
    CHECK(!HGNativeWaveDecode(wave,20,(int16_t *)(void *)(wave+12),8,&info) && !memcmp(saved,wave,32));
    header(wave,1,0,0,2);
    const uint8_t pcm16[]={0,128,133,255,0,0,255,127}; memcpy(wave+12,pcm16,8);
    const int16_t expected16[]={-32768,-123,0,32767};
    CHECK(HGNativeWaveDecode(wave,20,out,4,&info) && info.frames==4 && !memcmp(out,expected16,sizeof(expected16)));
    header(wave,2,1,1,1);
    const uint8_t adpcm[]={0x18,0xFC,0,0xA5,0x11,0x99,0x77,0xFF}; memcpy(wave+12,adpcm,8);
    const int16_t expectedADPCM[]={-998,-996,-998,-1000,-987,-957,-1020,-1156};
    CHECK(HGNativeWaveDecode(wave,20,out,8,&info) && info.frames==8 && !info.loopStart);
    CHECK(!memcmp(out,expectedADPCM,sizeof(expectedADPCM)));
    wave[14]=89; CHECK(!HGNativeWaveGetInfo(wave,20,&info)); wave[14]=128;
    CHECK(HGNativeWaveDecode(wave,20,out,8,&info) && !memcmp(out,expectedADPCM,sizeof(expectedADPCM)));
    wave[0]=3; CHECK(!HGNativeWaveGetInfo(wave,20,&info)); wave[0]=2;
    wave[6]=0; CHECK(!HGNativeWaveGetInfo(wave,20,&info));
    header(wave,0,0,0,0); CHECK(HGNativeWaveDecode(wave,12,NULL,0,&info) && !info.frames);
    CHECK(!HGNativeWaveGetInfo(NULL,12,&info) && !HGNativeWaveGetInfo(wave,12,NULL));
}
static void mixerChecks(void) {
    HGNativeMixer a,b; CHECK(!HGNativeMixerInit(&a,0) && HGNativeMixerInit(&a,16756991));
    const int16_t pcm[]={1000,-1000,32767,-32768};
    HGNativeWaveInfo info={.frames=4,.loopStart=1,.timer=1,.loop=0};
    CHECK(HGNativeMixerSetVoice(&a,0,pcm,4,&info) && !HGNativeMixerActive(&a));
    CHECK(HGNativeMixerControl(&a,1,127,0,0) && HGNativeMixerStart(&a,1));
    int16_t out[2048],split[2048]; CHECK(HGNativeMixerRender(&a,out,6));
    for (unsigned i=0;i<6;i++) CHECK(out[i*2]==(i<4 ? pcm[i] : 0) && !out[i*2+1]);
    CHECK(!HGNativeMixerActive(&a)); info.loop=1;
    CHECK(HGNativeMixerSetVoice(&a,0,pcm,4,&info) && HGNativeMixerControl(&a,1,127,0,0) && HGNativeMixerStart(&a,1));
    CHECK(HGNativeMixerRender(&a,out,10));
    for (unsigned i=0;i<10;i++) CHECK(out[i*2]==pcm[i<4 ? i : 1+(i-4)%3] && !out[i*2+1]);
    CHECK(HGNativeMixerInit(&a,48000) && HGNativeMixerInit(&b,48000)); info.timer=512;
    CHECK(HGNativeMixerSetVoice(&a,0,pcm,4,&info) && HGNativeMixerSetVoice(&b,0,pcm,4,&info));
    CHECK(HGNativeMixerStart(&a,1) && HGNativeMixerStart(&b,1) && HGNativeMixerRender(&a,out,1024));
    CHECK(HGNativeMixerRender(&b,split,1) && HGNativeMixerRender(&b,split+2,17) && HGNativeMixerRender(&b,split+36,1006));
    CHECK(!memcmp(out,split,sizeof(out)) && a.voices[0].remainder==b.voices[0].remainder);
    uint64_t ticks=(uint64_t)1024*16756991,divisor=(uint64_t)512*48000;
    CHECK(a.voices[0].remainder==ticks%divisor && a.voices[0].position==1+((ticks/divisor)-1)%3);
    CHECK(HGNativeMixerTimer(&a,1,0) && a.voices[0].period==65536);
    HGNativeVoice before=a.voices[0];
    CHECK(!HGNativeMixerSetVoice(&a,16,pcm,4,&info) && !HGNativeMixerSetVoice(&a,0,pcm,3,&info));
    CHECK(!HGNativeMixerControl(&a,1,128,0,0) && !HGNativeMixerControl(&a,1,127,4,0));
    CHECK(!HGNativeMixerTimer(&a,0x10000,1) && !HGNativeMixerTimer(&a,1,65536));
    CHECK(!HGNativeMixerMaster(&a,128,-1) && !HGNativeMixerMaster(&a,127,-2));
    CHECK(a.voices[0].position==before.position && a.voices[0].remainder==before.remainder);
    CHECK(!HGNativeMixerRender(&a,(int16_t *)(void *)pcm,1) && a.voices[0].position==before.position);
    CHECK(HGNativeMixerStop(&a,1) && !HGNativeMixerActive(&a));
    CHECK(!HGNativeMixerRender(&a,(int16_t *)(void *)pcm,1));
    const int16_t positive[]={32767},negative[]={-32768},tiny[]={-1};
    info=(HGNativeWaveInfo){.frames=1,.timer=1};
    CHECK(HGNativeMixerInit(&a,16756991));
    for (unsigned i=0;i<16;i++) CHECK(HGNativeMixerSetVoice(&a,i,positive,1,&info));
    CHECK(HGNativeMixerControl(&a,0xFFFF,127,0,0) && HGNativeMixerStart(&a,0xFFFF) && HGNativeMixerRender(&a,out,1));
    CHECK(out[0]==32767 && !out[1]);
    for (unsigned i=0;i<16;i++) CHECK(HGNativeMixerSetVoice(&a,i,negative,1,&info));
    CHECK(HGNativeMixerControl(&a,0xFFFF,127,0,127) && HGNativeMixerStart(&a,0xFFFF) && HGNativeMixerRender(&a,out,1));
    CHECK(!out[0] && out[1]==-32768);
    CHECK(HGNativeMixerSetVoice(&a,0,tiny,1,&info) && HGNativeMixerControl(&a,1,1,0,0) && HGNativeMixerStart(&a,1));
    CHECK(HGNativeMixerRender(&a,out,1) && out[0]==-1);
    CHECK(HGNativeMixerSetVoice(&a,0,positive,1,&info) && HGNativeMixerControl(&a,1,127,3,0) && HGNativeMixerStart(&a,1));
    CHECK(HGNativeMixerRender(&a,out,1) && out[0]==2047);
    CHECK(HGNativeMixerControl(&a,1,127,0,0) && HGNativeMixerMaster(&a,127,127) && HGNativeMixerStart(&a,1));
    CHECK(HGNativeMixerRender(&a,out,1) && !out[0] && out[1]==32767);
    CHECK(HGNativeMixerMaster(&a,0,-1) && HGNativeMixerStart(&a,1) && HGNativeMixerRender(&a,out,1));
    CHECK(!out[0] && !out[1] && HGNativeMixerRender(&a,NULL,0));
}
int main(int argc,char **argv) {
    CHECK(argc==2); unitChecks(); mixerChecks();
    FILE *file=fopen(argv[1],"rb"); CHECK(file);
    uint32_t count=read32(file); CHECK(count && count<100000);
    uint64_t total=0;
    for (uint32_t i=0;i<count;i++) {
        uint32_t size=read32(file),frames=read32(file),loop=read32(file);
        CHECK(size>=12 && size<=16777216 && frames<=16777216);
        uint8_t *encoded=malloc(size); int16_t *decoded=malloc((size_t)frames*2+2),*expected=malloc((size_t)frames*2+2);
        CHECK(encoded && decoded && expected && fread(encoded,1,size,file)==size);
        CHECK(fread(expected,2,frames,file)==frames);
        HGNativeWaveInfo info; CHECK(HGNativeWaveDecode(encoded,size,decoded,frames,&info));
        CHECK(info.frames==frames && info.loopStart==loop && !memcmp(decoded,expected,(size_t)frames*2));
        total+=frames; free(encoded); free(decoded); free(expected);
    }
    CHECK(fgetc(file)==EOF && !ferror(file)); fclose(file);
    printf("PASS: %u original waveforms, %llu exact PCM samples; signed PCM/ADPCM, DS rounding, loops and rejected malformed/overlapping output.\n",count,(unsigned long long)total);
    puts("PASS: native 16-voice stereo mixing, timer/rational split-block consistency, loop/end state, volume/pan, clipping and rejected controls.");
    return 0;
}
