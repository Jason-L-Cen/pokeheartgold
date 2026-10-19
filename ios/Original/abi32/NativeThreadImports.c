// SPDX-License-Identifier: GPL-3.0-or-later
#include "NativeImports.h"
#include "wasm-rt-impl.h"
#include "wasm-rt-exceptions.h"
#include "../NativeFiber.h"
#include "../NativeEvents.h"
#include "../NativeAlarm.h"
#include "../NativeDMA.h"
#include <nitro/os/native_thread.h>
#include <nitro/os/native_reset.h>
#include <nitro/os/irqTable.h>
#include <nitro/mi/dma.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct ThreadRecord ThreadRecord;
struct HGNativeThreadContext {
    struct w2c_env *env;
    ThreadRecord *records,*current,*root;
    pthread_t owner;
    u32 irqExit;
    BOOL closing;
};
struct ThreadRecord {
    ThreadRecord *next,*reaper;
    struct HGNativeThreadContext *context;
    HGNativeFiber *fiber;
    u32 address,sp,irq,low,high,depth,savedDepth;
    wasm_rt_jmp_buf trap;
    WASM_RT_UNWIND_TARGET *unwind;
    BOOL started,dead,discard;
};
static u8 *span(struct w2c_env *env,u32 address,u32 size) {
    if (!env || !env->memory || !env->memory->data || !address || address>env->memory->size ||
        size>env->memory->size-address) wasm_rt_trap(WASM_RT_TRAP_OOB);
    return env->memory->data+address;
}
static u32 read32(const u8 *bytes,u32 offset) { u32 result; memcpy(&result,bytes+offset,4); return result; }
static void write32(u8 *bytes,u32 offset,u32 value) { memcpy(bytes+offset,&value,4); }
static wasm_rt_funcref_t function(struct w2c_env *env,u32 index) {
    if (!env->irqTable || index>=env->irqTable->size) wasm_rt_trap(WASM_RT_TRAP_CALL_INDIRECT);
    wasm_rt_funcref_t ref=env->irqTable->data[index];
    if (!ref.func || !ref.func_type ||
        !((env->irqType0 && !memcmp(ref.func_type,env->irqType0,32)) ||
          (env->irqType1 && !memcmp(ref.func_type,env->irqType1,32)))) wasm_rt_trap(WASM_RT_TRAP_CALL_INDIRECT);
    return ref;
}
static void invoke(struct w2c_env *env,u32 index,u32 argument) {
    wasm_rt_funcref_t ref=function(env,index);
    if (!memcmp(ref.func_type,env->irqType0,32)) ((void (*)(void *))ref.func)(ref.module_instance);
    else ((void (*)(void *,u32))ref.func)(ref.module_instance,argument);
}
static ThreadRecord *find(struct HGNativeThreadContext *context,u32 address) {
    for (ThreadRecord *record=context->records;record;record=record->next) if (record->address==address && !record->discard) return record;
    return NULL;
}
static void capture(ThreadRecord *record) {
    struct w2c_env *env=record->context->env;
    record->sp=*env->stackPointer; record->irq=HGNativeGetInterruptState();
    memcpy(&record->trap,&g_wasm_rt_jmp_buf,sizeof(record->trap)); record->unwind=wasm_rt_get_unwind_target();
#if WASM_RT_STACK_DEPTH_COUNT
    record->depth=wasm_rt_call_stack_depth; record->savedDepth=wasm_rt_saved_call_stack_depth;
#endif
    u8 *thread=span(env,record->address,192);
    write32(thread,56,record->sp); write32(thread,0,(read32(thread,0)&~0xC0u)|record->irq);
}
static void restore(ThreadRecord *record) {
    struct w2c_env *env=record->context->env;
    if (!record->started) {
        u8 *thread=span(env,record->address,192);
        record->sp=read32(thread,56); record->irq=read32(thread,0)&0xC0;
    }
    *env->stackPointer=record->sp;
    memcpy(&g_wasm_rt_jmp_buf,&record->trap,sizeof(record->trap)); wasm_rt_set_unwind_target(record->unwind);
#if WASM_RT_STACK_DEPTH_COUNT
    wasm_rt_call_stack_depth=record->depth; wasm_rt_saved_call_stack_depth=record->savedDepth;
#endif
    HGNativeSetInterruptState(record->irq);
}
static void transfer(ThreadRecord *from,ThreadRecord *to,BOOL terminate) {
    struct HGNativeThreadContext *context=from->context;
    if (!terminate) capture(from); else from->dead=TRUE;
    context->current=to; restore(to);
    HGNativeFiberSwitch(from->fiber,to->fiber,terminate);
    if (from->discard) {
        ThreadRecord *reaper=from->reaper;
        context->current=reaper; restore(reaper); from->dead=TRUE;
        HGNativeFiberSwitch(from->fiber,reaper->fiber,true); abort();
    }
    if (context->env->resetRequested && !context->closing) wasm_rt_trap(WASM_RT_TRAP_UNREACHABLE);
}
static void entry(void *argument) {
    ThreadRecord *record=argument; struct w2c_env *env=record->context->env;
    record->started=TRUE;
    wasm_rt_trap_t trap=wasm_rt_impl_try();
    if (trap) { fprintf(stderr,"Original SDK thread trapped: %s\n",wasm_rt_strerror(trap)); abort(); }
    HGNativeDispatchIrqs();
    u8 *thread=span(env,record->address,192);
    invoke(env,read32(thread,64)-4,read32(thread,4));
    invoke(env,read32(thread,60),0); abort();
}
static ThreadRecord *create(struct HGNativeThreadContext *context,u32 address,BOOL root) {
    u8 *thread=span(context->env,address,192);
    u32 low=read32(thread,144),high=read32(thread,148);
    if (address%4 || low<16 || low>=high || high-low<512 || high>context->env->memory->size ||
        low%4 || high%4) wasm_rt_trap(WASM_RT_TRAP_OOB);
    if (!root) { (void)function(context->env,read32(thread,64)-4); if (read32(thread,56)<low+132 ||
        read32(thread,56)>((high-4)&~15u) || read32(thread,56)%16) wasm_rt_trap(WASM_RT_TRAP_OOB); }
    ThreadRecord *record=calloc(1,sizeof(*record));
    if (!record) wasm_rt_trap(WASM_RT_TRAP_EXHAUSTION);
    record->context=context; record->address=address; record->low=low; record->high=high;
    record->fiber=root ? HGNativeFiberRoot() : HGNativeFiberCreate(entry,record);
    if (!record->fiber) { free(record); wasm_rt_trap(WASM_RT_TRAP_EXHAUSTION); }
    record->started=root; record->next=context->records; context->records=record;
    return record;
}
static void removeRecord(struct HGNativeThreadContext *context,ThreadRecord *record) {
    ThreadRecord **link=&context->records;
    while (*link && *link!=record) link=&(*link)->next;
    if (*link) *link=record->next;
    HGNativeFiberDestroy(record->fiber); free(record);
}
static void dispose(struct HGNativeThreadContext *context,ThreadRecord *record) {
    if (record==context->root || record==context->current) wasm_rt_trap(WASM_RT_TRAP_OOB);
    if (record->started && !record->dead) {
        record->discard=TRUE; record->reaper=context->current;
        transfer(context->current,record,FALSE);
    }
    removeRecord(context,record);
}
static void afterIrq(void *argument) {
    struct HGNativeThreadContext *context=argument;
    if (!context->env->resetRequested) invoke(context->env,context->irqExit,0);
}
u32 HGNativeCheckThreadStack(struct w2c_env *env,u32 value) {
    if (env->threads) {
        ThreadRecord *record=env->threads->current;
        if (value<record->low+132 || value>((record->high-4)&~15u)) wasm_rt_trap(WASM_RT_TRAP_EXHAUSTION);
    }
    return value;
}
u32 w2c_env_HGNativeThreadRequest(struct w2c_env *env,u32 command,u32 address,u32 first,u32 second) {
    if (!env || !env->memory || !env->stackPointer || env->irqActive || OS_GetProcMode()!=OS_PROCMODE_SYS ||
        command>HG_THREAD_ROOT_HIGH) wasm_rt_trap(WASM_RT_TRAP_OOB);
    if (command==HG_THREAD_ROOT_HIGH) {
        if (address || first || second || !env->rootStackHigh) wasm_rt_trap(WASM_RT_TRAP_OOB);
        return env->rootStackHigh;
    }
    struct HGNativeThreadContext *context=env->threads;
    if (context && !pthread_equal(context->owner,pthread_self())) wasm_rt_trap(WASM_RT_TRAP_OOB);
    if (command==HG_THREAD_INIT) {
        if (context || second) wasm_rt_trap(WASM_RT_TRAP_OOB);
        wasm_rt_funcref_t hook=function(env,first);
        if (memcmp(hook.func_type,env->irqType0,32)) wasm_rt_trap(WASM_RT_TRAP_CALL_INDIRECT);
        context=calloc(1,sizeof(*context)); if (!context) wasm_rt_trap(WASM_RT_TRAP_EXHAUSTION);
        context->env=env; context->owner=pthread_self(); context->irqExit=first;
        context->root=create(context,address,TRUE); context->current=context->root;
        if (!HGNativeSetIrqExitHook(afterIrq,context)) { removeRecord(context,context->root); free(context); wasm_rt_trap(WASM_RT_TRAP_OOB); }
        env->threads=context; return TRUE;
    }
    if (command==HG_THREAD_RESET && !context) return TRUE;
    if (!context) wasm_rt_trap(WASM_RT_TRAP_OOB);
    ThreadRecord *record=find(context,address);
    switch (command) {
    case HG_THREAD_REGISTER:
        if (first || second || (record && record->started)) wasm_rt_trap(WASM_RT_TRAP_OOB);
        if (record) return TRUE;
        create(context,address,FALSE); return TRUE;
    case HG_THREAD_RESET:
        if (first || second) wasm_rt_trap(WASM_RT_TRAP_OOB);
        if (!record) return TRUE;
        if (record==context->root) wasm_rt_trap(WASM_RT_TRAP_OOB);
        if (record==context->current) { record->discard=TRUE; create(context,address,FALSE); }
        else { dispose(context,record); create(context,address,FALSE); }
        return TRUE;
    case HG_THREAD_DROP:
        if (first || second || !record) wasm_rt_trap(WASM_RT_TRAP_OOB);
        dispose(context,record); return TRUE;
    case HG_THREAD_SWITCH: {
        ThreadRecord *next=find(context,first);
        if (record!=context->current || !next || next==record || second>1 ||
            (record==context->root && second)) wasm_rt_trap(WASM_RT_TRAP_OOB);
        transfer(record,next,second!=0); return TRUE;
    }
    case HG_THREAD_LOAD:
        if (first || second || !record || record==context->current || context->current==context->root) wasm_rt_trap(WASM_RT_TRAP_OOB);
        transfer(context->current,record,TRUE); abort();
    case HG_THREAD_HALT:
        if (address || first || second) wasm_rt_trap(WASM_RT_TRAP_OOB);
        HGNativeHalt(); return TRUE;
    default: wasm_rt_trap(WASM_RT_TRAP_OOB);
    }
}
void HGNativeCloseThreadContext(struct w2c_env *env) {
    struct HGNativeThreadContext *context=env->threads;
    if (!context) return;
    if (context->current!=context->root || !pthread_equal(context->owner,pthread_self()) ||
        !HGNativeClearIrqExitHook(context)) wasm_rt_trap(WASM_RT_TRAP_OOB);
    context->closing=TRUE;
    OSIntrMode previous=HGNativeSetInterruptState(OS_INTRMODE_DISABLE_IRQ|OS_INTRMODE_DISABLE_FIQ);
    for (ThreadRecord *record=context->records,*next;record;record=next) {
        next=record->next; if (record!=context->root) dispose(context,record);
    }
    removeRecord(context,context->root); env->threads=NULL; free(context);
    HGNativeSetInterruptState(previous);
}
u32 w2c_env_HGNativeResetControl(struct w2c_env *env,u32 command,u32 parameter) {
    if (!env || !env->memory || !env->memory->data || env->irqActive || OS_GetProcMode()!=OS_PROCMODE_SYS ||
        command>HG_RESET_REQUEST || env->resetRequested || (command!=HG_RESET_REQUEST && parameter))
        wasm_rt_trap(WASM_RT_TRAP_OOB);
    struct HGNativeThreadContext *context=env->threads;
    if (context && !pthread_equal(context->owner,pthread_self())) wasm_rt_trap(WASM_RT_TRAP_OOB);
    if (command==HG_RESET_INIT) return 0;
    if (command==HG_RESET_OCCURRED) return env->resetOccurred!=0;
    if (command==HG_RESET_PARAMETER) return env->bootParameter;
    env->resetRequested=env->resetOccurred=1; env->resetParameter=parameter;
    OS_DisableIrq();
    for (u32 channel=0;channel<4;channel++) MI_StopDma(channel);
    if (context && context->current!=context->root) {
        transfer(context->current,context->root,TRUE);
        abort();
    }
    wasm_rt_trap(WASM_RT_TRAP_UNREACHABLE);
}
void HGNativeFinishReset(struct w2c_env *env) {
    if (!env || !env->resetRequested || env->irqActive || OS_GetProcMode()!=OS_PROCMODE_SYS)
        wasm_rt_trap(WASM_RT_TRAP_OOB);
    if (env->resetFinished) return;
    if (env->threads && (env->threads->current!=env->threads->root || !pthread_equal(env->threads->owner,pthread_self())))
        wasm_rt_trap(WASM_RT_TRAP_OOB);
    OS_DisableIrq(); HGNativeCancelAlarmContext(env);
    for (u32 channel=0;channel<4;channel++) MI_StopDma(channel);
    HGNativeDrainDMA();
    HGNativeSetInterruptState(OS_INTRMODE_ENABLE);
    if (__atomic_load_n(&env->pendingRTC,__ATOMIC_ACQUIRE) || env->pendingMIC || env->pendingPOWER) {
        OS_SetIrqMask((1u<<OS_IRQ_TABLE_MAX)-1);
        OS_EnableIrq(); // Complete native reply ownership; reset instances suppress all guest callbacks.
    }
    OS_DisableIrq();
    HGNativeReleaseIrqCallbacks(env);
    HGNativeSetInterruptState(OS_INTRMODE_ENABLE);
    env->resetFinished=1;
}
