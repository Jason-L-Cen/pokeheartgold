// SPDX-License-Identifier: GPL-3.0-or-later
#include "../NativeMicSampler.h"
#include <string.h>
#define CHECK(v) do { if (!(v)) return __LINE__; } while (0)
static _Alignas(32) u8 buffer[8192];
static float input[4096];
static MICAutoParam param(u32 type,u32 size,BOOL loop) {
    MICAutoParam p={(MICSamplingType)type,buffer,size,0x1040,loop,NULL,NULL}; return p;
}
u32 CheckNativeMicSampler(void) {
    HGNativeMicSampler state; HGNativeMicSamplerInit(&state); u32 full;
    const u8 eight[2][4]={{0,128,255,192},{128,0,127,64}};
    const u16 twelve[2][4]={{0,0x8000,0xFFF0,0xC000},{0x8000,0,0x7FF0,0x4000}};
    const float values[]={-1,0,1,0.5f};
    for (u32 type=0;type<MIC_SAMPLING_TYPE_MAX;type++) {
        HGNativeMicSamplerInit(&state); MICAutoParam p=param(type,32,FALSE);
        memset(buffer,0xA5,sizeof(buffer)); CHECK(HGNativeMicSamplerStart(&state,&p)==MIC_RESULT_SUCCESS);
        CHECK(!HGNativeMicSamplerLast(&state));
        CHECK(HGNativeMicSamplerFeed(&state,values,1,&full)==MIC_RESULT_SUCCESS && !full);
        BOOL wide=type!=0 && type!=2,sign=type==2 || type==3 || type==5;
        if (!wide) CHECK(!HGNativeMicSamplerLast(&state) && buffer[0]==0xA5);
        CHECK(HGNativeMicSamplerFeed(&state,values+1,3,&full)==MIC_RESULT_SUCCESS && !full);
        for (u32 i=0;i<4;i++) {
            if (wide) CHECK((u16)(buffer[i*2]|((u16)buffer[i*2+1]<<8))==twelve[sign][i]);
            else CHECK(buffer[i]==eight[sign][i]);
        }
        CHECK(HGNativeMicSamplerLast(&state)==buffer+(wide ? 6 : 2));
        HGNativeMicSamplerStop(&state);
    }
    HGNativeMicSamplerInit(&state); MICAutoParam p=param(0,32,TRUE);
    CHECK(HGNativeMicSamplerStart(&state,&p)==MIC_RESULT_SUCCESS);
    for (u32 i=0;i<96;i++) input[i]=0;
    CHECK(HGNativeMicSamplerFeed(&state,input,65,&full)==MIC_RESULT_SUCCESS && full==2 && state.running);
    CHECK(state.offset==1 && HGNativeMicSamplerLast(&state)==buffer+30);
    HGNativeMicSamplerStop(&state); p.loop_enable=FALSE;
    CHECK(HGNativeMicSamplerStart(&state,&p)==MIC_RESULT_SUCCESS);
    CHECK(HGNativeMicSamplerFeed(&state,input,65,&full)==MIC_RESULT_SUCCESS && full==1 && !state.running);
    CHECK(HGNativeMicSamplerFeed(&state,input,1,&full)==MIC_RESULT_ILLEGAL_STATUS && !full);
    HGNativeMicSamplerInit(&state); p=param(0,4096,FALSE);
    CHECK(HGNativeMicSamplerStart(&state,&p)==MIC_RESULT_SUCCESS);
    for (u32 i=0;i<4096;i++) input[i]=0.5f;
    CHECK(HGNativeMicSamplerFeed(&state,input,4096,&full)==MIC_RESULT_SUCCESS && full==1 && state.bias8==1);
    CHECK(buffer[4094]==192 && buffer[4095]==191);
    CHECK(HGNativeMicSamplerStart(&state,&p)==MIC_RESULT_SUCCESS && state.bias8==1);
    HGNativeMicSamplerStop(&state); HGNativeMicSamplerInit(&state); p=param(1,512,FALSE);
    CHECK(HGNativeMicSamplerStart(&state,&p)==MIC_RESULT_SUCCESS);
    CHECK(HGNativeMicSamplerFeed(&state,input,256,&full)==MIC_RESULT_SUCCESS && full==1 && state.bias12==16);
    CHECK((u16)(buffer[510]|((u16)buffer[511]<<8))==0xBFF0);
    for (u32 type=1;type<=5;type+=3) {
        HGNativeMicSamplerInit(&state); p=param(type,32,FALSE); CHECK(HGNativeMicSamplerStart(&state,&p)==MIC_RESULT_SUCCESS);
        const float gaps[]={0,__builtin_nanf(""),__builtin_inff()}; CHECK(HGNativeMicSamplerFeed(&state,gaps,3,&full)==MIC_RESULT_SUCCESS);
        CHECK((u16)(buffer[2]|((u16)buffer[3]<<8))==(type==4 ? 0xFFFF : 0x8000));
        CHECK(state.count12==1); HGNativeMicSamplerStop(&state);
    }
    HGNativeMicSamplerInit(&state); p=param(0,32,FALSE);
    const u32 periods[]={1024,65535,65537,0x400001,0x1000001,0x3FFFFFF};
    const u32 actual[]={1024,65535,65536,0x400000,0x1000000,0x3FFFC00};
    for (u32 i=0;i<6;i++) { p.rate=periods[i]; CHECK(HGNativeMicSamplerStart(&state,&p)==MIC_RESULT_SUCCESS && state.period==actual[i]); HGNativeMicSamplerStop(&state); }
    HGNativeMicSampler before=state;
    for (u32 bad=0;bad<6;bad++) {
        p=param(0,32,FALSE);
        if (!bad) p.buffer=NULL; if (bad==1) p.buffer=buffer+1; if (bad==2) p.size=31;
        if (bad==3) p.rate=1023; if (bad==4) p.rate=0x4000000; if (bad==5) p.type=MIC_SAMPLING_TYPE_MAX;
        CHECK(HGNativeMicSamplerStart(&state,&p)==MIC_RESULT_ILLEGAL_PARAMETER && !memcmp(&state,&before,sizeof(state)));
    }
    p=param(0,32,FALSE); CHECK(HGNativeMicSamplerStart(&state,&p)==MIC_RESULT_SUCCESS); before=state;
    CHECK(HGNativeMicSamplerStart(&state,&p)==MIC_RESULT_ILLEGAL_STATUS && !memcmp(&state,&before,sizeof(state)));
    full=0x12345678;
    CHECK(HGNativeMicSamplerFeed(&state,NULL,1,&full)==MIC_RESULT_ILLEGAL_PARAMETER && full==0x12345678 && !memcmp(&state,&before,sizeof(state)));
    CHECK(HGNativeMicSamplerFeed(&state,(const float *)((const u8 *)input+1),1,&full)==MIC_RESULT_ILLEGAL_PARAMETER);
    return 0;
}
