// SPDX-License-Identifier: GPL-3.0-or-later
#include "NativeImports.h"
#include "wasm-rt-impl.h"
#include "wasm-rt-exceptions.h"
#include "../NativeEvents.h"
#include "../NativeAlarm.h"
#include "../NativeDMA.h"
#include <nitro/os/native_irq.h>
#include <nitro/os/irqTable.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static _Noreturn void fail(wasm_rt_trap_t code) {
    if (OS_GetProcMode()==OS_PROCMODE_IRQ) {
        fprintf(stderr,"Original IRQ callback trapped: %s\n",wasm_rt_strerror(code)); abort();
    }
    wasm_rt_trap(code);
}
static wasm_rt_funcref_t callback(struct w2c_env *env,u32 index) {
    if (!env || !env->memory || !env->irqTable || index>=env->irqTable->size) fail(WASM_RT_TRAP_CALL_INDIRECT);
    wasm_rt_funcref_t ref=env->irqTable->data[index];
    if (!ref.func || !ref.func_type ||
        !((env->irqType0 && !memcmp(ref.func_type,env->irqType0,32)) ||
          (env->irqType1 && !memcmp(ref.func_type,env->irqType1,32)))) fail(WASM_RT_TRAP_CALL_INDIRECT);
    return ref;
}
u32 HGNativeCheckedStackPointer(struct w2c_env *env,u32 value) {
    if (!env) fail(WASM_RT_TRAP_OOB);
    // Reserve the pinned WASI libc's 128-byte leaf red zone below every checked frame.
    if (env->irqActive && (value<env->irqStackStart+HG_IRQ_RED_ZONE ||
        value>env->irqStackStart+env->irqStackSize)) fail(WASM_RT_TRAP_EXHAUSTION);
    return env->irqActive ? value : HGNativeCheckThreadStack(env,value);
}
static void invoke(void *context,u32 index,u32 argument) {
    struct w2c_env *env=context; wasm_rt_funcref_t ref=callback(env,index);
    HGNativeInvokeIrqCallback(env,index,argument,0,env->irqType0 && !memcmp(ref.func_type,env->irqType0,32) ? 0 : 1);
}
static wasm_rt_funcref_t guardFor(struct w2c_env *env) {
    if (!env || !env->irqTable || !env->irqGuardType || !env->stackPointer ||
        !env->irqGuard || env->irqGuard>=env->irqTable->size) fail(WASM_RT_TRAP_CALL_INDIRECT);
    wasm_rt_funcref_t guard=env->irqTable->data[env->irqGuard];
    if (!guard.func || !guard.func_type || memcmp(guard.func_type,env->irqGuardType,32)) fail(WASM_RT_TRAP_CALL_INDIRECT);
    return guard;
}
void HGNativeInvokeIrqCallback(struct w2c_env *env,u32 function,u32 first,u32 second,u32 arity) {
    if (env && env->resetRequested) return;
    wasm_rt_funcref_t guard=guardFor(env);
    if (arity>2 || env->irqActive || env->irqStackSize!=HG_IRQ_STACK_SIZE ||
        env->irqStackStart<16 || env->irqStackStart%16 || !env->memory || !env->memory->data ||
        env->irqStackStart>env->memory->size || env->irqStackSize+16>env->memory->size-env->irqStackStart)
        fail(WASM_RT_TRAP_OOB);
    if (function>=env->irqTable->size) fail(WASM_RT_TRAP_CALL_INDIRECT);
    wasm_rt_funcref_t ref=env->irqTable->data[function];
    wasm_rt_func_type_t type=arity==0 ? env->irqType0 : arity==1 ? env->irqType1 : env->rtcType;
    if (!ref.func || !ref.func_type || !type || memcmp(ref.func_type,type,32)) fail(WASM_RT_TRAP_CALL_INDIRECT);
    u8 *bytes=env->memory->data;
    for (u32 i=0;i<16;i++) if (bytes[env->irqStackStart-16+i]!=0xA5 ||
        bytes[env->irqStackStart+env->irqStackSize+i]!=0xA5) fail(WASM_RT_TRAP_OOB);
    u32 previous=*env->stackPointer;
    WASM_RT_UNWIND_TARGET *saved=wasm_rt_get_unwind_target();
    // Runtime traps target the TLS buffer directly; restore its bytes for the outer call.
    wasm_rt_jmp_buf previousTarget;
    memcpy(&previousTarget,&g_wasm_rt_jmp_buf,sizeof(previousTarget));
#if WASM_RT_STACK_DEPTH_COUNT
    u32 savedDepth=wasm_rt_saved_call_stack_depth;
#endif
    env->irqActive=1; *env->stackPointer=env->irqStackStart+env->irqStackSize;
    wasm_rt_trap_t trap=wasm_rt_impl_try();
    if (!trap) ((void (*)(void *,u32,u32,u32,u32))guard.func)(guard.module_instance,function,first,second,arity);
    *env->stackPointer=previous; env->irqActive=0;
    memcpy(&g_wasm_rt_jmp_buf,&previousTarget,sizeof(previousTarget)); wasm_rt_set_unwind_target(saved);
#if WASM_RT_STACK_DEPTH_COUNT
    wasm_rt_saved_call_stack_depth=savedDepth;
#endif
    // Guest traps inside IRQ dispatch are fatal; longjmp must not abandon the native event locks.
    if (trap) { fprintf(stderr,"Original IRQ callback trapped: %s\n",wasm_rt_strerror(trap)); abort(); }
    for (u32 i=0;i<16;i++) if (bytes[env->irqStackStart-16+i]!=0xA5 ||
        bytes[env->irqStackStart+env->irqStackSize+i]!=0xA5) abort();
}
void w2c_env_HGNativeIrqInit(struct w2c_env *env,u32 configuration) {
    if (!env || !env->memory || !env->memory->data || !configuration || configuration%4 ||
        configuration>env->memory->size || 16>env->memory->size-configuration || env->irqActive || !env->stackPointer)
        fail(WASM_RT_TRAP_OOB);
    u32 values[4]; memcpy(values,env->memory->data+configuration,sizeof(values));
    u32 dummy=values[0];
    wasm_rt_funcref_t ref=callback(env,dummy);
    if (memcmp(ref.func_type,env->irqType0,32) || OS_GetProcMode()!=OS_PROCMODE_SYS) fail(WASM_RT_TRAP_CALL_INDIRECT);
    if (!env->irqGuardType || !values[1] || values[1]>=env->irqTable->size) fail(WASM_RT_TRAP_CALL_INDIRECT);
    wasm_rt_funcref_t guard=env->irqTable->data[values[1]];
    if (!guard.func || !guard.func_type || memcmp(guard.func_type,env->irqGuardType,32)) fail(WASM_RT_TRAP_CALL_INDIRECT);
    if (values[2]<16 || values[2]%16 || values[3]!=HG_IRQ_STACK_SIZE || values[2]>env->memory->size ||
        values[3]+16>env->memory->size-values[2]) fail(WASM_RT_TRAP_OOB);
    env->irqDummy=dummy; env->irqGuard=values[1]; env->irqStackStart=values[2]; env->irqStackSize=values[3]; OS_InitIrqTable();
}
void w2c_env_HGNativeIrqSet(struct w2c_env *env,u32 kind,u32 target,u32 function,u32 argument) {
    if (!env || !env->memory || kind>HG_IRQ_TIMER || (kind!=HG_IRQ_PLAIN && target>=4)) fail(WASM_RT_TRAP_OOB);
    if (kind==HG_IRQ_PLAIN && !(target&((1u<<OS_IRQ_TABLE_MAX)-1))) return;
    if (function) (void)callback(env,function);
    HGNativeIrqBridge bridge={function ? invoke : NULL,env,function,argument};
    HGNativeInstallIrqBridge(kind,target,&bridge);
}
u32 w2c_env_HGNativeIrqGet(struct w2c_env *env,u32 mask) {
    if (!env || !env->memory) fail(WASM_RT_TRAP_OOB);
    HGNativeIrqBridge bridge;
    if (HGNativeGetIrqBridge(mask,&bridge)) {
        if (bridge.context!=env || bridge.invoke!=invoke) fail(WASM_RT_TRAP_CALL_INDIRECT);
        return bridge.function;
    }
    OSIrqFunction native=OS_GetIrqFunction(mask);
    if (!native) return 0;
    if (native==OS_IrqDummy && env->irqDummy) return env->irqDummy;
    fail(WASM_RT_TRAP_CALL_INDIRECT);
}
u32 w2c_env_HGNativeIrqControl(struct w2c_env *env,u32 command,u32 value) {
    if (!env || !env->memory) fail(WASM_RT_TRAP_OOB);
    switch (command) {
    case HG_IRQ_ENABLE: return OS_EnableIrq();
    case HG_IRQ_DISABLE: return OS_DisableIrq();
    case HG_IRQ_RESTORE: return OS_RestoreIrq(value!=0);
    case HG_IRQ_GET_MASK: return OS_GetIrqMask();
    case HG_IRQ_SET_MASK: return OS_SetIrqMask(value);
    case HG_IRQ_ENABLE_MASK: return OS_EnableIrqMask(value);
    case HG_IRQ_DISABLE_MASK: return OS_DisableIrqMask(value);
    case HG_IRQ_GET_REQUEST: return OS_GetRequestIrqMask();
    case HG_IRQ_RESET_REQUEST: return OS_ResetRequestIrqMask(value);
    case HG_IRQ_GET_CHECK: return OS_GetIrqCheckFlag();
    case HG_IRQ_SET_CHECK: OS_SetIrqCheckFlag(value); return 0;
    case HG_IRQ_CLEAR_CHECK: OS_ClearIrqCheckFlag(value); return 0;
    case HG_IRQ_GET_VBLANK: return OS_GetVBlankCount();
    case HG_IRQ_HALT:
        if (value || env->irqActive || OS_GetProcMode()!=OS_PROCMODE_SYS) fail(WASM_RT_TRAP_OOB);
        HGNativeHalt(); return 0;
    case HG_IRQ_DISPATCH:
        if (value || env->irqActive || OS_GetProcMode()!=OS_PROCMODE_SYS) fail(WASM_RT_TRAP_OOB);
        HGNativeDispatchIrqs(); return 0;
    case HG_IRQ_WAIT:
    case HG_IRQ_WAIT_CLEAR:
        if (!value || (value&~((1u<<OS_IRQ_TABLE_MAX)-1)) || OS_GetCpsrIrq()==OS_INTRMODE_IRQ_DISABLE) fail(WASM_RT_TRAP_OOB);
        OS_WaitIrq(command==HG_IRQ_WAIT_CLEAR,value); return 0;
    }
    fail(WASM_RT_TRAP_OOB);
}
void HGNativeReleaseIrqCallbacks(struct w2c_env *env) {
    if (!env || env->irqActive || __atomic_load_n(&env->pendingRTC,__ATOMIC_ACQUIRE) ||
        __atomic_load_n(&env->pendingDMA,__ATOMIC_ACQUIRE) || __atomic_load_n(&env->pendingCARD,__ATOMIC_ACQUIRE) ||
        env->pendingMIC || env->pendingPOWER || env->pendingINPUT) fail(WASM_RT_TRAP_OOB);
    // A completed copy can still be posting its IRQ after dropping its memory reference.
    HGNativeDrainDMA();
    HGNativeCloseThreadContext(env);
    if (!HGNativeClearIrqBridgeContext(env)) fail(WASM_RT_TRAP_OOB);
    HGNativeCloseFileContext(env);
    HGNativeCloseMicContext(env);
    HGNativeClosePowerContext(env);
    HGNativeCloseInputContext(env);
    HGNativeCloseCardContext(env);
    HGNativeCancelAlarmContext(env);
    env->irqTable=NULL; env->irqType0=env->irqType1=env->rtcType=NULL; env->irqDummy=0;
    env->irqGuardType=NULL; env->irqGuard=env->irqStackStart=env->irqStackSize=0;
}
