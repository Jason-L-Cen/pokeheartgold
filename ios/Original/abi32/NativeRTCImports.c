// SPDX-License-Identifier: GPL-3.0-or-later
#include "NativeImports.h"
#include <nitro/rtc/ARM9/api.h>
#include <nitro/rtc/native_api.h>
#include <stdlib.h>
#include <string.h>

_Static_assert(sizeof(RTCDate)==16 && sizeof(RTCTime)==12,"RTC value ABI");
static u8 *output(struct w2c_env *env,u32 offset,u32 length) {
    if (!env || !env->memory || !env->memory->data || offset>env->memory->size || length>env->memory->size-offset) wasm_rt_trap(WASM_RT_TRAP_OOB);
    if (offset&3) wasm_rt_trap(WASM_RT_TRAP_UNALIGNED);
    return env->memory->data+offset;
}
static wasm_rt_funcref_t completion(struct w2c_env *env,u32 index) {
    if (!env->irqTable || index>=env->irqTable->size || !env->rtcType) wasm_rt_trap(WASM_RT_TRAP_CALL_INDIRECT);
    wasm_rt_funcref_t ref=env->irqTable->data[index];
    if (!ref.func || !ref.func_type || memcmp(ref.func_type,env->rtcType,32)) wasm_rt_trap(WASM_RT_TRAP_CALL_INDIRECT);
    return ref;
}
typedef struct NativeRtcReply {
    struct w2c_env *env;
    u32 dateOffset,timeOffset,function,argument;
    RTCDate date;
    RTCTime time;
} NativeRtcReply;
static void copyReply(struct w2c_env *env,u32 date,u32 time,const RTCDate *day,const RTCTime *clock) {
    if (date) memcpy(output(env,date,sizeof(*day)),day,sizeof(*day));
    if (time) memcpy(output(env,time,sizeof(*clock)),clock,sizeof(*clock));
}
static void completed(RTCResult result,void *argument) {
    NativeRtcReply *reply=argument; struct w2c_env *env=reply->env;
    if (result==RTC_RESULT_SUCCESS) copyReply(env,reply->dateOffset,reply->timeOffset,&reply->date,&reply->time);
    if (reply->function) {
        wasm_rt_funcref_t ref=completion(env,reply->function);
        ((void (*)(void *,u32,u32))ref.func)(ref.module_instance,result,reply->argument);
    }
    free(reply); __atomic_fetch_sub(&env->pendingRTC,1,__ATOMIC_RELEASE);
}
u32 w2c_env_HGNativeRtcRequest(struct w2c_env *env,u32 command,u32 date,u32 time,u32 function,u32 argument) {
    if (!env || !env->memory) wasm_rt_trap(WASM_RT_TRAP_OOB);
    if (command==HG_RTC_INIT) { RTC_Init(); return RTC_RESULT_SUCCESS; }
    if (command>HG_RTC_BOTH_ASYNC) return RTC_RESULT_INVALID_COMMAND;
    BOOL async=command>=HG_RTC_DATE_ASYNC;
    u32 kind=async ? command-3 : command;
    if ((kind!=HG_RTC_TIME && !date) || (kind!=HG_RTC_DATE && !time)) return RTC_RESULT_ILLEGAL_PARAMETER;
    if (kind==HG_RTC_TIME) date=0;
    if (kind==HG_RTC_DATE) time=0;
    if (date) (void)output(env,date,sizeof(RTCDate));
    if (time) (void)output(env,time,sizeof(RTCTime));
    if (async && function) (void)completion(env,function);
    if (!async) {
        RTCDate day; RTCTime clock;
        RTCResult result=kind==HG_RTC_DATE ? RTC_GetDate(&day) : kind==HG_RTC_TIME ? RTC_GetTime(&clock) : RTC_GetDateTime(&day,&clock);
        if (result==RTC_RESULT_SUCCESS) copyReply(env,date,time,&day,&clock);
        return result;
    }
    NativeRtcReply *reply=calloc(1,sizeof(*reply));
    if (!reply) return RTC_RESULT_SEND_ERROR;
    reply->env=env; reply->dateOffset=date; reply->timeOffset=time; reply->function=function; reply->argument=argument;
    __atomic_fetch_add(&env->pendingRTC,1,__ATOMIC_ACQ_REL);
    RTCResult result=kind==HG_RTC_DATE ? RTC_GetDateAsync(&reply->date,completed,reply) :
        kind==HG_RTC_TIME ? RTC_GetTimeAsync(&reply->time,completed,reply) : RTC_GetDateTimeAsync(&reply->date,&reply->time,completed,reply);
    if (result!=RTC_RESULT_SUCCESS) { free(reply); __atomic_fetch_sub(&env->pendingRTC,1,__ATOMIC_RELEASE); }
    return result;
}
