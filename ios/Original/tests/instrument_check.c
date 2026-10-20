// SPDX-License-Identifier: GPL-3.0-or-later
#include "../NativeInstrument.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(v) do { if (!(v)) { fprintf(stderr,"Instrument check line %d: %s\n",__LINE__,#v); abort(); } } while (0)
static uint32_t read32(FILE *f) { uint8_t b[4]; CHECK(fread(b,1,4,f)==4); return b[0]|(uint32_t)b[1]<<8|(uint32_t)b[2]<<16|(uint32_t)b[3]<<24; }
static void put32(uint8_t *p,uint32_t v) { for (unsigned i=0;i<4;i++) p[i]=(uint8_t)(v>>(i*8)); }
static void rejected(const uint8_t *bank,size_t bytes,uint32_t program,uint32_t key) {
    HGNativeInstrument out,unchanged; memset(&out,0xA5,sizeof(out)); unchanged=out;
    CHECK(HGNativeReadInstrument(bank,bytes,program,key,&out)==HG_INSTRUMENT_INVALID && !memcmp(&out,&unchanged,sizeof(out)));
}
int main(int argc,char **argv) {
    CHECK(argc==2); FILE *f=fopen(argv[1],"rb"); CHECK(f); uint32_t banks=read32(f); uint64_t comparisons=0;
    for (uint32_t i=0;i<banks;i++) {
        uint32_t size=read32(f),programs=read32(f); CHECK(size>=60 && size<16777216 && programs<65536);
        uint8_t *data=malloc(size),*changed=malloc(size); CHECK(data && changed && fread(data,1,size,f)==size);
        for (uint32_t p=0;p<programs;p++) for (uint32_t key=0;key<128;key++) {
            uint8_t want[13]; CHECK(fread(want,1,sizeof(want),f)==sizeof(want));
            HGNativeInstrument out,before; memset(&out,0xA5,sizeof(out)); before=out;
            CHECK(HGNativeReadInstrument(data,size,p,key,&out)==want[0]);
            if (want[0]) CHECK(!memcmp(&out,want+1,sizeof(out)));
            else CHECK(!memcmp(&out,&before,sizeof(out)));
            comparisons++;
        }
        rejected(data,size-1,0,0); rejected(data,size,0,128);
        memcpy(changed,data,size); changed[0]='X'; rejected(changed,size,0,0);
        memcpy(changed,data,size); put32(changed+56,UINT32_MAX); rejected(changed,size,0,0);
        if (programs) {
            memcpy(changed,data,size); put32(changed+60,0xFFFFFF01u); rejected(changed,size,0,0);
            put32(changed+60,0x1001); rejected(changed,size,0,0);
        }
        free(changed); free(data);
    }
    CHECK(fgetc(f)==EOF && !ferror(f)); fclose(f);
    uint8_t sample[96]={0}; memcpy(sample,"SBNK\xFF\xFE\x00\x01",8); put32(sample+8,sizeof(sample));
    sample[12]=16; sample[14]=1; memcpy(sample+16,"DATA",4); put32(sample+20,sizeof(sample)-16); put32(sample+56,1);
    put32(sample+60,(64u<<8)|16); sample[64]=70; sample[65]=60; rejected(sample,sizeof(sample),0,64);
    put32(sample+60,(64u<<8)|17); memset(sample+64,0,32); sample[64]=60; sample[65]=50; rejected(sample,sizeof(sample),0,64);
    memset(sample+64,0,32); sample[64]=127; sample[72]=17; rejected(sample,sizeof(sample),0,64);
    rejected(NULL,0,0,0);
    printf("PASS: %u original banks, %llu exact program/key results; direct, drums, key splits, absent notes and rejected malformed data.\n",banks,(unsigned long long)comparisons);
    return 0;
}
