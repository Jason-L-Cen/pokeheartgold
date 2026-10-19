// SPDX-License-Identifier: GPL-3.0-or-later
#include "../NativeMicResampler.h"
#include "../NativeMicSampler.h"
#include <string.h>
#include <stdio.h>
#define CHECK(v) do { if (!(v)) return __LINE__; } while (0)
static float input[48000],inverse[48000],one[40000],split[40000];
typedef struct Output { float *samples; u32 count; BOOL failed; HGNativeMicSampler *sampler; } Output;
static void receive(void *context,const float *samples,uint32_t count) {
    Output *out=context;
    if (count>40000-out->count) { out->failed=TRUE; return; }
    memcpy(out->samples+out->count,samples,count*sizeof(float)); out->count+=count;
    if (out->sampler && out->sampler->running) { u32 completed; if (HGNativeMicSamplerFeed(out->sampler,samples,count,&completed)!=MIC_RESULT_SUCCESS) out->failed=TRUE; }
}
u32 CheckNativeMicResampler(void) {
    CHECK(!HGNativeMicResamplerCreate(0,1,2000) && !HGNativeMicResamplerCreate(48000,0,2000));
    HGNativeMicResampler *invalid=HGNativeMicResamplerCreate(48000,2,2000); CHECK(invalid);
    const float bad[]={__builtin_nanf("")},good[]={0}; const float *badChannels[]={good,bad};
    Output rejected={one,0,FALSE,NULL};
    CHECK(!HGNativeMicResamplerPush(invalid,badChannels,1,receive,&rejected) && !rejected.count);
    HGNativeMicResamplerDestroy(invalid);
    const u32 rates[]={48000,44100};
    const u32 targets[]={2000,8000,32000};
    for (u32 source=0;source<2;source++) for (u32 target=0;target<3;target++) {
        u32 frames=rates[source]; for (u32 i=0;i<frames;i++) { input[i]=0.25f; inverse[i]=-0.25f; }
        HGNativeMicResampler *a=HGNativeMicResamplerCreate(frames,1,targets[target]),*b=HGNativeMicResamplerCreate(frames,1,targets[target]); CHECK(a && b);
        Output first={one,0,FALSE,NULL},second={split,0,FALSE,NULL}; const float *whole[]={input};
        CHECK(HGNativeMicResamplerPush(a,whole,frames,receive,&first));
        CHECK(HGNativeMicResamplerFinish(a,receive,&first) && !first.failed);
        for (u32 start=0;start<frames;) { u32 count=frames-start; if (count>997) count=997; const float *part[]={input+start}; CHECK(HGNativeMicResamplerPush(b,part,count,receive,&second)); start+=count; }
        CHECK(HGNativeMicResamplerFinish(b,receive,&second) && !second.failed && second.count==first.count);
        CHECK(first.count+1>=targets[target] && first.count<=targets[target]+1);
        for (u32 i=0;i<first.count;i++) {
            if (!(__builtin_fabsf(one[i]-split[i])<0.0001f) || (i>64 && i+64<first.count && !(__builtin_fabsf(one[i]-0.25f)<0.002f))) {
                fprintf(stderr,"Resampler %u Hz frame %u/%u: whole %.9g split %.9g\n",frames,i,first.count,one[i],split[i]);
                return __LINE__;
            }
        }
        CHECK(!HGNativeMicResamplerPush(a,whole,1,receive,&first)); HGNativeMicResamplerDestroy(a); HGNativeMicResamplerDestroy(b);
    }
    for (u32 i=0;i<48000;i++) input[i]=(float)i/96000;
    HGNativeMicResampler *rampA=HGNativeMicResamplerCreate(48000,1,8000),*rampB=HGNativeMicResamplerCreate(48000,1,8000); CHECK(rampA && rampB);
    Output firstRamp={one,0,FALSE,NULL},splitRamp={split,0,FALSE,NULL}; const float *wholeRamp[]={input};
    CHECK(HGNativeMicResamplerPush(rampA,wholeRamp,48000,receive,&firstRamp) && HGNativeMicResamplerFinish(rampA,receive,&firstRamp));
    for (u32 i=0;i<48000;i+=1000) { const float *part[]={input+i}; CHECK(HGNativeMicResamplerPush(rampB,part,1000,receive,&splitRamp)); }
    CHECK(HGNativeMicResamplerFinish(rampB,receive,&splitRamp) && firstRamp.count==splitRamp.count);
    for (u32 i=0;i<firstRamp.count;i++) CHECK(__builtin_fabsf(one[i]-split[i])<0.0001f);
    HGNativeMicResamplerDestroy(rampA); HGNativeMicResamplerDestroy(rampB);
    for (u32 i=0;i<48000;i++) { input[i]=0.25f; inverse[i]=-0.25f; }
    HGNativeMicResampler *stereo=HGNativeMicResamplerCreate(48000,2,8000); CHECK(stereo);
    const float *channels[]={input,inverse}; Output mono={one,0,FALSE,NULL};
    CHECK(HGNativeMicResamplerPush(stereo,channels,44100,receive,&mono) && HGNativeMicResamplerFinish(stereo,receive,&mono));
    for (u32 i=0;i<mono.count;i++) CHECK(__builtin_fabsf(one[i])<0.0001f);
    HGNativeMicResamplerDestroy(stereo);
    static _Alignas(32) u8 recorded[1984]; HGNativeMicSampler sampler; HGNativeMicSamplerInit(&sampler);
    MICAutoParam p={MIC_SAMPLING_TYPE_SIGNED_8BIT,recorded,sizeof(recorded),HW_CPU_CLOCK_ARM7/2000,FALSE,NULL,NULL};
    CHECK(HGNativeMicSamplerStart(&sampler,&p)==MIC_RESULT_SUCCESS);
    HGNativeMicResampler *recording=HGNativeMicResamplerCreate(48000,1,(double)HW_CPU_CLOCK_ARM7/sampler.period); CHECK(recording);
    for (u32 i=0;i<48000;i++) input[i]=0;
    Output recordedOut={one,0,FALSE,&sampler}; const float *silence[]={input};
    CHECK(HGNativeMicResamplerPush(recording,silence,48000,receive,&recordedOut) && HGNativeMicResamplerFinish(recording,receive,&recordedOut));
    CHECK(!recordedOut.failed && !sampler.running && HGNativeMicSamplerLast(&sampler)==recorded+sizeof(recorded)-2);
    for (u32 i=0;i<sizeof(recorded);i++) CHECK(!recorded[i]);
    HGNativeMicResamplerDestroy(recording); return 0;
}
