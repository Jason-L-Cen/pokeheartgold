// SPDX-License-Identifier: GPL-3.0-or-later
#include "NativeImports.h"
#include "wasm-rt-impl.h"
#include "wasm-rt-exceptions.h"
#include "../NativeEvents.h"
#include "../NativeAlarm.h"
#include "../NativeDMA.h"
#include <nitro/os/native_irq.h>
#include <nitro/os/irqTable.h>
#include <nitro/hw/native_graphics.h>
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
static void invokeCallback(struct w2c_env *env,u32 function,u32 first,u32 second,u32 arity,BOOL finish) {
    if (!finish && env && env->resetRequested) return;
    u32 previous=env && env->stackPointer ? *env->stackPointer : 0;
    u32 previousActive=env ? env->irqActive : 0;
    OSIntrMode previousMask=HGNativeGetInterruptState();
    WASM_RT_UNWIND_TARGET *saved=wasm_rt_get_unwind_target();
    // Runtime traps target the TLS buffer directly; restore its bytes for the outer call.
    wasm_rt_jmp_buf previousTarget;
    memcpy(&previousTarget,&g_wasm_rt_jmp_buf,sizeof(previousTarget));
#if WASM_RT_STACK_DEPTH_COUNT
    u32 savedDepth=wasm_rt_saved_call_stack_depth;
#endif
    wasm_rt_trap_t trap=wasm_rt_impl_try();
    if (!trap) {
        wasm_rt_funcref_t guard=guardFor(env);
        if (arity>2 || env->irqActive || env->resetRequested || env->irqStackSize!=HG_IRQ_STACK_SIZE ||
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
        if (finish) {
            if (arity!=2 || OS_GetProcMode()!=OS_PROCMODE_SYS || previous<HG_IRQ_RED_ZONE || previous%16 ||
                previous>env->memory->size || (!env->threads && (!env->rootStackHigh ||
                env->rootStackHigh>env->memory->size || previous>env->rootStackHigh)) ||
                (previous>=env->irqStackStart-16 && previous<=env->irqStackStart+env->irqStackSize+16)) fail(WASM_RT_TRAP_OOB);
            HGNativeCheckThreadStack(env,previous);
        }
        // GPU completion must not consume a small SDK worker stack; irqActive also prohibits yield/reentry in SYS.
        env->irqActive=1; *env->stackPointer=env->irqStackStart+env->irqStackSize;
        // The existing typed guard saves CP state in either execution context.
        ((void (*)(void *,u32,u32,u32,u32))guard.func)(guard.module_instance,function,first,second,arity);
        if (finish && (*env->stackPointer!=env->irqStackStart+env->irqStackSize || env->irqActive!=1 || OS_GetProcMode()!=OS_PROCMODE_SYS)) fail(WASM_RT_TRAP_OOB);
        for (u32 i=0;i<16;i++) if (bytes[env->irqStackStart-16+i]!=0xA5 ||
            bytes[env->irqStackStart+env->irqStackSize+i]!=0xA5) fail(WASM_RT_TRAP_OOB);
    }
    if (env && env->stackPointer) *env->stackPointer=previous;
    if (env) env->irqActive=previousActive;
    if (finish) HGNativeSetInterruptState(previousMask);
    memcpy(&g_wasm_rt_jmp_buf,&previousTarget,sizeof(previousTarget)); wasm_rt_set_unwind_target(saved);
#if WASM_RT_STACK_DEPTH_COUNT
    wasm_rt_saved_call_stack_depth=savedDepth;
#endif
    // Neither IRQ nor SYS completion traps may unwind past the native event epilogue.
    if (trap) { fprintf(stderr,"Original %s callback trapped: %s\n",finish ? "raster FINISH" : "IRQ",wasm_rt_strerror(trap)); abort(); }
}
void HGNativeInvokeIrqCallback(struct w2c_env *env,u32 function,u32 first,u32 second,u32 arity) {
    invokeCallback(env,function,first,second,arity,FALSE);
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
static void rasterStep(void *context,u32 line,u32 phase) {
    struct w2c_env *env=context;
    u32 writes=__atomic_exchange_n(&env->dmaAffineWrites,0,__ATOMIC_ACQ_REL);
    if (writes&~255u) { fprintf(stderr,"Invalid raster affine-write mask: 0x%08X\n",writes); abort(); }
    invokeCallback(env,env->rasterFunction,line,phase|(writes<<HG_NATIVE_RASTER_AFFINE_SHIFT),2,phase==HG_NATIVE_RASTER_FINISH);
}
void w2c_env_HGNativeIrqSet(struct w2c_env *env,u32 kind,u32 target,u32 function,u32 argument) {
    if (kind==HG_IRQ_RASTER) {
        if (!env || !env->memory || !env->irqTable || target || argument) fail(WASM_RT_TRAP_OOB);
        if (function && (function>=env->irqTable->size || !env->rtcType ||
            !env->irqTable->data[function].func || !env->irqTable->data[function].func_type ||
            memcmp(env->irqTable->data[function].func_type,env->rtcType,32))) fail(WASM_RT_TRAP_CALL_INDIRECT);
        if (!HGNativeSetRasterHook(env,function ? rasterStep : NULL,function ? env : NULL)) fail(WASM_RT_TRAP_OOB);
        env->rasterFunction=function;
        return;
    }
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
    HGNativeStopDisplayClock(env);
    HGNativeDrainDMA();
    __atomic_store_n(&env->dmaAffineWrites,0,__ATOMIC_RELEASE);
    HGNativeCloseThreadContext(env);
    if (!HGNativeClearIrqBridgeContext(env)) fail(WASM_RT_TRAP_OOB);
    HGNativeCloseFileContext(env);
    HGNativeCloseMicContext(env);
    HGNativeClosePowerContext(env);
    HGNativeCloseInputContext(env);
    HGNativeCloseCardContext(env);
    HGNativeCloseSoundContext(env);
    HGNativeCancelAlarmContext(env);
    env->irqTable=NULL; env->irqType0=env->irqType1=env->rtcType=NULL; env->irqDummy=0;
    env->irqGuardType=NULL; env->irqGuard=env->irqStackStart=env->irqStackSize=0;
    env->rasterFunction=0;
}
