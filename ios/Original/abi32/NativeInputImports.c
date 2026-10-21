// SPDX-License-Identifier: GPL-3.0-or-later
#include "NativeImports.h"
#include "../NativeEvents.h"
#include <nitro/spi/native_input.h>
#include <nitro/gx/native_display.h>
#include <nitro/hw/consts_shared.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct InputReply {
    struct w2c_env *env;
    struct InputReply *next;
    u32 function,data,point;
    BOOL delivered;
} InputReply;
struct HGNativeInputContext {
    u32 pad,point,frequency,vcount,function,padReads,generation;
    u64 sample;
    InputReply *head,*tail;
};
static u64 sampleClock(const struct HGNativeInputContext *state) {
    struct timespec stamp; if (clock_gettime(CLOCK_MONOTONIC,&stamp)) abort();
    u64 nanoseconds=(u64)stamp.tv_sec*1000000000ULL+stamp.tv_nsec;
    u64 clocks=(unsigned __int128)nanoseconds*HW_SYSTEM_CLOCK/1000000000ULL;
    u64 phase=(u64)state->vcount*HG_DISPLAY_LINE_CLOCKS;
    return clocks<phase ? 0 : (unsigned __int128)(clocks-phase)*state->frequency/HG_DISPLAY_FRAME_CLOCKS;
}
static void sampleDue(struct w2c_env *env) {
    struct HGNativeInputContext *state=env->input;
    if (!state->frequency) return;
    u64 sample=sampleClock(state),count=sample-state->sample;
    u32 generation=state->generation;
    state->sample=sample;
    // shortcut: deliver on the original game input loop, bounded to 256 after stalls; use scanline wakeups for intra-frame clients.
    if (count>256) count=256;
    while (count-- && state->generation==generation && HGNativeSampleInput(env)) { }
}
static wasm_rt_funcref_t callback(struct w2c_env *env,u32 function) {
    if (!function || !env->irqTable || !env->rtcType || function>=env->irqTable->size)
        wasm_rt_trap(WASM_RT_TRAP_CALL_INDIRECT);
    wasm_rt_funcref_t ref=env->irqTable->data[function];
    if (!ref.func || !ref.func_type || memcmp(ref.func_type,env->rtcType,32)) wasm_rt_trap(WASM_RT_TRAP_CALL_INDIRECT);
    return ref;
}
static BOOL usable(struct w2c_env *env) {
    return env && env->memory && env->memory->data && !env->memory->is64 &&
        env->memory->pages==env->memory->max_pages && !env->resetRequested;
}
static void take(InputReply *reply) {
    struct HGNativeInputContext *state=reply->env->input;
    if (state->head!=reply) wasm_rt_trap(WASM_RT_TRAP_OOB);
    state->head=reply->next; if (!state->head) state->tail=NULL;
    reply->delivered=TRUE;
}
static void completed(void *argument) {
    InputReply *reply=argument; struct w2c_env *env=reply->env;
    if (!reply->delivered) {
        take(reply);
        if (!env->resetRequested) HGNativeInvokeIrqCallback(env,reply->function,reply->data,reply->point,2);
    }
    free(reply); env->pendingINPUT--;
}
static BOOL post(struct w2c_env *env,u32 function,u32 data) {
    // Bound queued samples while IRQ delivery is paused; the producer can retry after dispatch.
    if (env->pendingINPUT>=256) return FALSE;
    InputReply *reply=calloc(1,sizeof(*reply)); if (!reply) return FALSE;
    *reply=(InputReply){env,NULL,function,data,env->input->point,FALSE};
    struct HGNativeInputContext *state=env->input;
    InputReply *previous=state->tail;
    if (previous) previous->next=reply; else state->head=reply;
    state->tail=reply; env->pendingINPUT++;
    if (HGNativeQueueIrqCallback(OS_IE_SPFIFO_RECV,completed,reply)) return TRUE;
    if (previous) previous->next=NULL; else state->head=NULL;
    state->tail=previous; env->pendingINPUT--; free(reply); return FALSE;
}
u32 w2c_env_HGNativeInputRequest(struct w2c_env *env,u32 command,u32 first,u32 second,u32 function) {
    if (!usable(env) || command>HG_INPUT_DRAIN ||
        ((command==HG_INPUT_INIT || command==HG_INPUT_DRAIN) && (first || second || function)) ||
        (command==HG_INPUT_PAD && (first>1 || second || function)) ||
        ((command==HG_INPUT_SAMPLE || command==HG_INPUT_STOP) && (first || second))) wasm_rt_trap(WASM_RT_TRAP_OOB);
    if (command==HG_INPUT_INIT) {
        if (!env->input) { env->input=calloc(1,sizeof(*env->input)); if (!env->input) wasm_rt_trap(WASM_RT_TRAP_EXHAUSTION); }
        return TRUE;
    }
    if (command==HG_INPUT_PAD) {
        if (!env->input) return 0;
        u32 pad=env->input->pad;
        if (first) { env->input->padReads++; sampleDue(env); }
        return pad;
    }
    if (!env->input) return FALSE;
    if (command==HG_INPUT_DRAIN) {
        // shortcut: masked polling drains this service; use shared FIFO polling for cross-service callback ordering.
        if (OS_GetCpsrIrq()!=OS_INTRMODE_IRQ_DISABLE) wasm_rt_trap(WASM_RT_TRAP_OOB);
        InputReply *reply=env->input->head;
        if (reply) {
            wasm_rt_funcref_t ref=callback(env,reply->function); take(reply);
            ((void (*)(void *,u32,u32))ref.func)(ref.module_instance,reply->data,reply->point);
        }
        return TRUE;
    }
    (void)callback(env,function);
    struct HGNativeInputContext *state=env->input;
    u32 frequency=state->frequency,vcount=state->vcount,previousFunction=state->function;
    u32 generation=state->generation;
    u64 sample=state->sample;
    u32 reply=0x01000000;
    if (command==HG_INPUT_SAMPLE) {
        if (state->frequency) reply|=3;
    } else if (command==HG_INPUT_START) {
        if (first>=263 || !second || second>255) wasm_rt_trap(WASM_RT_TRAP_OOB);
        reply|=0x100;
        if (state->frequency) reply|=3;
        else { state->frequency=second; state->vcount=first; state->function=function; state->sample=sampleClock(state); state->generation++; }
    } else {
        reply|=0x200; state->frequency=0; state->generation++;
    }
    if (post(env,function,reply)) return TRUE;
    state->frequency=frequency; state->vcount=vcount; state->function=previousFunction;
    state->sample=sample; state->generation=generation; return FALSE;
}
u32 HGNativeInputPadReads(struct w2c_env *env) { return usable(env) && env->input ? env->input->padReads : 0; }
BOOL HGNativeSetInput(struct w2c_env *env,u32 keys,BOOL folded,u32 x,u32 y,BOOL pressed) {
    if (!usable(env) || !env->irqTable || (keys&~0x2FFFu) || (u32)folded>1 || x>255 || y>191 || (u32)pressed>1) return FALSE;
    if (!env->input) (void)w2c_env_HGNativeInputRequest(env,HG_INPUT_INIT,0,0,0);
    env->input->pad=keys|((u32)folded<<15);
    env->input->point=(x<<4)|(y<<16)|((u32)pressed<<24);
    return TRUE;
}
u32 HGNativeInputSamplingRate(struct w2c_env *env,u32 *vcount) {
    u32 rate=usable(env) && env->input ? env->input->frequency : 0;
    if (vcount) *vcount=rate ? env->input->vcount : 0;
    return rate;
}
BOOL HGNativeSampleInput(struct w2c_env *env) {
    if (!usable(env) || !env->input || !env->input->frequency) return FALSE;
    return post(env,env->input->function,0x1000);
}
void HGNativeCloseInputContext(struct w2c_env *env) {
    if (env->pendingINPUT || (env->input && env->input->head)) wasm_rt_trap(WASM_RT_TRAP_OOB);
    free(env->input); env->input=NULL;
}
