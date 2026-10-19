// SPDX-License-Identifier: GPL-3.0-or-later
#include "NativeImports.h"
#include "../NativeEvents.h"
#include <nitro/spi/native_power.h>
#include <stdlib.h>
#include <string.h>

typedef struct PowerReply {
    struct w2c_env *env;
    u32 function,data;
    BOOL delivered;
} PowerReply;
struct HGNativePowerContext { u8 registers[5],led; PowerReply *active; };

static wasm_rt_funcref_t callback(struct w2c_env *env,u32 function) {
    if (!function || !env->irqTable || !env->rtcType || function>=env->irqTable->size)
        wasm_rt_trap(WASM_RT_TRAP_CALL_INDIRECT);
    wasm_rt_funcref_t ref=env->irqTable->data[function];
    if (!ref.func || !ref.func_type || memcmp(ref.func_type,env->rtcType,32)) wasm_rt_trap(WASM_RT_TRAP_CALL_INDIRECT);
    return ref;
}
static void completed(void *argument) {
    PowerReply *reply=argument; struct w2c_env *env=reply->env;
    if (!reply->delivered) {
        reply->delivered=TRUE;
        if (env->power->active==reply) env->power->active=NULL;
        if (!env->resetRequested) HGNativeInvokeIrqCallback(env,reply->function,reply->data,0,2);
    }
    free(reply); env->pendingPOWER--;
}
static BOOL post(struct w2c_env *env,u32 function,u32 data) {
    PowerReply *reply=calloc(1,sizeof(*reply)); if (!reply) return FALSE;
    *reply=(PowerReply){env,function,data,FALSE}; env->power->active=reply; env->pendingPOWER++;
    if (HGNativeQueueIrqCallback(OS_IE_SPFIFO_RECV,completed,reply)) return TRUE;
    env->power->active=NULL; env->pendingPOWER--; free(reply); return FALSE;
}
u32 w2c_env_HGNativePowerRequest(struct w2c_env *env,u32 command,u32 first,u32 second,u32 function) {
    if (!env || !env->memory || !env->memory->data || env->memory->is64 || env->memory->pages!=env->memory->max_pages ||
        command>HG_POWER_DRAIN || env->resetRequested ||
        ((command==HG_POWER_INIT || command==HG_POWER_DRAIN) && (first || second || function)) ||
        (command!=HG_POWER_WRITE && second)) wasm_rt_trap(WASM_RT_TRAP_OOB);
    if (command==HG_POWER_INIT) {
        if (!env->power) {
            env->power=calloc(1,sizeof(*env->power)); if (!env->power) wasm_rt_trap(WASM_RT_TRAP_EXHAUSTION);
            env->power->registers[0]=12; env->power->registers[3]=1;
        }
        return TRUE;
    }
    if (!env->power) return FALSE;
    if (command==HG_POWER_DRAIN) {
        if (OS_GetCpsrIrq()!=OS_INTRMODE_IRQ_DISABLE) wasm_rt_trap(WASM_RT_TRAP_OOB);
        PowerReply *reply=env->power->active;
        if (reply) {
            wasm_rt_funcref_t ref=callback(env,reply->function);
            reply->delivered=TRUE; env->power->active=NULL;
            // The SDK polls replies on its current SYS/IRQ stack when CPU IRQs are masked.
            ((void (*)(void *,u32,u32))ref.func)(ref.module_instance,reply->data,0);
        }
        return TRUE;
    }
    (void)callback(env,function);
    if (env->power->active) return FALSE;
    struct HGNativePowerContext previous=*env->power;
    u32 result=0,data=0;
    switch (command) {
    case HG_POWER_READ:
        if (first>=5) wasm_rt_trap(WASM_RT_TRAP_OOB);
        data=((0x70+first)<<8)|env->power->registers[first]; break;
    case HG_POWER_WRITE:
        if (first>=5 || second>0xFFFF) wasm_rt_trap(WASM_RT_TRAP_OOB);
        env->power->registers[first]=(u8)second; data=0x6400; break;
    case HG_POWER_UTILITY:
        if (first>=1 && first<=3) env->power->led=first==1 ? 1 : first==2 ? 3 : 2;
        else if (first>=4 && first<=9) {
            u8 mask=first<6 ? 4 : first<8 ? 8 : 12;
            env->power->registers[0]=(env->power->registers[0]&~mask)|((first&1) ? 0 : mask);
        } else result=2; // Shutdown/sleep need the native application lifecycle; never acknowledge them as complete.
        data=0x6300|result; break;
    case HG_POWER_SET_LED:
        if (first>255) wasm_rt_trap(WASM_RT_TRAP_OOB);
        env->power->led=(u8)first; data=0x6600; break;
    case HG_POWER_GET_LED:
        if (first) wasm_rt_trap(WASM_RT_TRAP_OOB);
        data=0x6700|env->power->led; break;
    default: wasm_rt_trap(WASM_RT_TRAP_OOB);
    }
    if (post(env,function,data)) return TRUE;
    *env->power=previous; return FALSE;
}
void HGNativeClosePowerContext(struct w2c_env *env) {
    if (env->pendingPOWER || (env->power && env->power->active)) wasm_rt_trap(WASM_RT_TRAP_OOB);
    free(env->power); env->power=NULL;
}
