// SPDX-License-Identifier: GPL-3.0-or-later
#include "NativeMicSampler.h"
#include <stdint.h>
#include <string.h>

void HGNativeMicSamplerInit(HGNativeMicSampler *state) { if (state) memset(state,0,sizeof(*state)); }
MICResult HGNativeMicSamplerStart(HGNativeMicSampler *state,const MICAutoParam *param) {
    if (!state || !param || !param->buffer || (uaddr)param->buffer%32 || !param->size || param->size%32 ||
        (uaddr)param->buffer>UINTPTR_MAX-param->size || (u32)param->type>=MIC_SAMPLING_TYPE_MAX ||
        param->rate<MIC_SAMPLING_RATE_LIMIT || param->rate>=0x4000000) return MIC_RESULT_ILLEGAL_PARAMETER;
    if (state->running) return MIC_RESULT_ILLEGAL_STATUS;
    static const u32 modes[]={0,1,2,3,5,7};
    u32 shift=param->rate<0x10000 ? 0 : param->rate<0x400000 ? 6 : param->rate<0x1000000 ? 8 : 10;
    state->buffer=param->buffer; state->size=param->size; state->period=(param->rate>>shift)<<shift;
    state->flags=modes[param->type]|(param->loop_enable ? 16 : 0);
    state->offset=state->lastOffset=0; state->partial=state->last=0;
    state->published=FALSE; state->running=TRUE; return MIC_RESULT_SUCCESS;
}
static u16 sample(HGNativeMicSampler *state,float input) {
    if (!__builtin_isfinite(input)) return state->flags&4 ? 0xFFFF : state->last;
    if (input< -1) input=-1; if (input>1) input=1;
    s32 value;
    if (state->flags&1) {
        u32 raw=(u32)((input+1)*2048); if (raw>4095) raw=4095; raw<<=4;
        state->sum12+=raw;
        if (++state->count12==256) {
            s32 target=(s32)((state->sum12>>8)&0xFFF0)-0x8000;
            if (state->bias12<target && state->bias12<0xC00) state->bias12+=16;
            else if (target<state->bias12 && state->bias12> -0xC00) state->bias12-=16;
            state->sum12=state->count12=0;
        }
        value=(s32)raw-state->bias12; if (value<0) value=0; if (value>0xFFF0) value=0xFFF0;
        if (state->flags&2) value^=0x8000;
    } else {
        u32 raw=(u32)((input+1)*128); if (raw>255) raw=255;
        state->sum8+=raw;
        if (++state->count8==4096) {
            s32 target=(s32)(state->sum8>>12)-128;
            if (state->bias8<target && state->bias8<12) state->bias8++;
            else if (target<state->bias8 && state->bias8> -12) state->bias8--;
            state->sum8=state->count8=0;
        }
        value=(s32)raw-state->bias8; if (value<0) value=0; if (value>255) value=255;
        if (state->flags&2) value^=0x80;
    }
    return (u16)value;
}
MICResult HGNativeMicSamplerFeed(HGNativeMicSampler *state,const float *input,u32 count,u32 *completed) {
    if (!state || !completed || (count && (!input || (uaddr)input%_Alignof(float) || (u64)count*sizeof(*input)>UINTPTR_MAX-(uaddr)input))) return MIC_RESULT_ILLEGAL_PARAMETER;
    *completed=0;
    if (!state->running) return MIC_RESULT_ILLEGAL_STATUS;
    for (u32 i=0;i<count && state->running;i++) {
        state->last=sample(state,input[i]);
        if (state->flags&1) {
            state->buffer[state->offset]=(u8)state->last;
            state->buffer[state->offset+1]=(u8)(state->last>>8);
            state->lastOffset=state->offset; state->published=TRUE; state->offset+=2;
        } else if (!(state->offset&1)) { state->partial=(u8)state->last; state->offset++; }
        else {
            state->buffer[state->offset-1]=(u8)state->partial;
            state->buffer[state->offset]=(u8)state->last;
            state->lastOffset=state->offset-1; state->published=TRUE; state->offset++;
        }
        if (state->offset==state->size) {
            state->offset=0; (*completed)++;
            if (!(state->flags&16)) state->running=FALSE;
        }
    }
    return MIC_RESULT_SUCCESS;
}
void HGNativeMicSamplerStop(HGNativeMicSampler *state) { if (state) state->running=FALSE; }
void *HGNativeMicSamplerLast(const HGNativeMicSampler *state) { return state && state->published ? state->buffer+state->lastOffset : NULL; }
