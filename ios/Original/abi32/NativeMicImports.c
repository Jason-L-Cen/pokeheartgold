// SPDX-License-Identifier: GPL-3.0-or-later
#include "NativeImports.h"
#include "../NativeMicSampler.h"
#include "../NativeEvents.h"
#include <nitro/spi/native_mic.h>
#include <stdlib.h>
#include <string.h>

struct HGNativeMicContext {
    HGNativeMicSampler sampler;
    u32 function,generation;
};
typedef struct MicReply {
    struct w2c_env *env;
    u32 function,data;
} MicReply;
static void completed(void *argument) {
    MicReply *reply=argument; struct w2c_env *env=reply->env;
    HGNativeInvokeIrqCallback(env,reply->function,reply->data,0,2);
    free(reply); env->pendingMIC--;
}
static BOOL post(struct w2c_env *env,u32 function,u32 command,MICResult result) {
    static const u8 replies[]={0,4,2,5,1,3,5};
    MicReply *reply=malloc(sizeof(*reply)); if (!reply) return FALSE;
    *reply=(MicReply){env,function,(command<<8)|replies[result]};
    env->pendingMIC++;
    if (HGNativeQueueIrqCallback(OS_IE_SPFIFO_RECV,completed,reply)) return TRUE;
    env->pendingMIC--; free(reply); return FALSE;
}
static void validateCallback(struct w2c_env *env,u32 function) {
    if (!function || !env->irqTable || !env->rtcType || function>=env->irqTable->size)
        wasm_rt_trap(WASM_RT_TRAP_CALL_INDIRECT);
    wasm_rt_funcref_t ref=env->irqTable->data[function];
    if (!ref.func || !ref.func_type || memcmp(ref.func_type,env->rtcType,32)) wasm_rt_trap(WASM_RT_TRAP_CALL_INDIRECT);
}
u32 w2c_env_HGNativeMicRequest(struct w2c_env *env,u32 command,u32 buffer,u32 size,u32 rate,u32 flags,u32 function) {
    if (!env || !env->memory || !env->memory->data || env->memory->is64 || env->memory->pages!=env->memory->max_pages)
        wasm_rt_trap(WASM_RT_TRAP_OOB);
    if (command>HG_MIC_LAST || (command!=HG_MIC_START && (buffer || size || rate || flags)) ||
        ((command==HG_MIC_INIT || command==HG_MIC_LAST) && function)) wasm_rt_trap(WASM_RT_TRAP_OOB);
    if (command==HG_MIC_INIT) {
        if (!env->mic) { env->mic=calloc(1,sizeof(*env->mic)); if (!env->mic) wasm_rt_trap(WASM_RT_TRAP_EXHAUSTION); }
        return 0;
    }
    if (command==HG_MIC_LAST) {
        void *last=env->mic ? HGNativeMicSamplerLast(&env->mic->sampler) : NULL;
        return last ? (u32)((u8 *)last-env->memory->data) : 0;
    }
    validateCallback(env,function);
    if (!env->mic) return FALSE;
    struct HGNativeMicContext *state=env->mic;
    HGNativeMicSampler previous=state->sampler;
    MICResult result;
    if (command==HG_MIC_START) {
        if (!buffer || buffer%32 || !size || size%32 || buffer>env->memory->size || size>env->memory->size-buffer ||
            flags&~23u || (flags&7)==4 || (flags&7)==6) wasm_rt_trap(WASM_RT_TRAP_OOB);
        static const u8 types[]={0,1,2,3,0,4,0,5};
        MICAutoParam param={types[flags&7],env->memory->data+buffer,size,rate,(flags&16)!=0,NULL,NULL};
        result=HGNativeMicSamplerStart(&state->sampler,&param);
    } else {
        HGNativeMicSamplerStop(&state->sampler); result=MIC_RESULT_SUCCESS;
    }
    u32 previousFunction=state->function,previousGeneration=state->generation;
    if (result==MIC_RESULT_SUCCESS) { state->function=function; state->generation++; }
    if (post(env,function,command==HG_MIC_START ? 0x41 : 0x42,result)) return TRUE;
    state->sampler=previous; state->function=previousFunction; state->generation=previousGeneration; return FALSE;
}
u32 HGNativeFeedMic(struct w2c_env *env,const float *samples,u32 count) {
    if (!env || !env->mic || !env->mic->sampler.running) return MIC_RESULT_ILLEGAL_STATUS;
    if (count>1048576 || (count && (!samples || (uintptr_t)samples%_Alignof(float) ||
        (u64)count*sizeof(*samples)>UINTPTR_MAX-(uintptr_t)samples))) return MIC_RESULT_ILLEGAL_PARAMETER;
    struct HGNativeMicContext *state=env->mic; u32 generation=state->generation;
    while (count && state->sampler.running) {
        u32 frames=(state->sampler.size-state->sampler.offset)/((state->sampler.flags&1) ? 2 : 1);
        if (frames>count) frames=count;
        u32 full; MICResult result=HGNativeMicSamplerFeed(&state->sampler,samples,frames,&full);
        if (result!=MIC_RESULT_SUCCESS) return result;
        samples+=frames; count-=frames;
        if (full && !post(env,state->function,0x51,MIC_RESULT_SUCCESS)) wasm_rt_trap(WASM_RT_TRAP_EXHAUSTION);
        if (state->generation!=generation) break;
    }
    return MIC_RESULT_SUCCESS;
}
void HGNativeCloseMicContext(struct w2c_env *env) {
    if (env->pendingMIC) wasm_rt_trap(WASM_RT_TRAP_OOB);
    free(env->mic); env->mic=NULL;
}
