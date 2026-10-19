// SPDX-License-Identifier: GPL-3.0-or-later
#include "NativeImports.h"
#include "../NativeEvents.h"
#include "../NativeAlarm.h"
#include "../NativeDMA.h"
#include <nitro/os/native_irq.h>
#include <nitro/os/irqTable.h>
#include <string.h>

static wasm_rt_funcref_t callback(struct w2c_env *env,u32 index) {
    if (!env || !env->memory || !env->irqTable || index>=env->irqTable->size) wasm_rt_trap(WASM_RT_TRAP_CALL_INDIRECT);
    wasm_rt_funcref_t ref=env->irqTable->data[index];
    if (!ref.func || !ref.func_type ||
        !((env->irqType0 && !memcmp(ref.func_type,env->irqType0,32)) ||
          (env->irqType1 && !memcmp(ref.func_type,env->irqType1,32)))) wasm_rt_trap(WASM_RT_TRAP_CALL_INDIRECT);
    return ref;
}
static void invoke(void *context,u32 index,u32 argument) {
    struct w2c_env *env=context; wasm_rt_funcref_t ref=callback(env,index);
    if (env->irqType0 && !memcmp(ref.func_type,env->irqType0,32)) ((void (*)(void *))ref.func)(ref.module_instance);
    else ((void (*)(void *,u32))ref.func)(ref.module_instance,argument);
}
void w2c_env_HGNativeIrqInit(struct w2c_env *env,u32 dummy) {
    wasm_rt_funcref_t ref=callback(env,dummy);
    if (memcmp(ref.func_type,env->irqType0,32) || OS_GetProcMode()!=OS_PROCMODE_SYS) wasm_rt_trap(WASM_RT_TRAP_CALL_INDIRECT);
    env->irqDummy=dummy; OS_InitIrqTable();
}
void w2c_env_HGNativeIrqSet(struct w2c_env *env,u32 kind,u32 target,u32 function,u32 argument) {
    if (!env || !env->memory || kind>HG_IRQ_TIMER || (kind!=HG_IRQ_PLAIN && target>=4)) wasm_rt_trap(WASM_RT_TRAP_OOB);
    if (kind==HG_IRQ_PLAIN && !(target&((1u<<OS_IRQ_TABLE_MAX)-1))) return;
    if (function) (void)callback(env,function);
    HGNativeIrqBridge bridge={function ? invoke : NULL,env,function,argument};
    HGNativeInstallIrqBridge(kind,target,&bridge);
}
u32 w2c_env_HGNativeIrqGet(struct w2c_env *env,u32 mask) {
    if (!env || !env->memory) wasm_rt_trap(WASM_RT_TRAP_OOB);
    HGNativeIrqBridge bridge;
    if (HGNativeGetIrqBridge(mask,&bridge)) {
        if (bridge.context!=env || bridge.invoke!=invoke) wasm_rt_trap(WASM_RT_TRAP_CALL_INDIRECT);
        return bridge.function;
    }
    OSIrqFunction native=OS_GetIrqFunction(mask);
    if (!native) return 0;
    if (native==OS_IrqDummy && env->irqDummy) return env->irqDummy;
    wasm_rt_trap(WASM_RT_TRAP_CALL_INDIRECT);
}
u32 w2c_env_HGNativeIrqControl(struct w2c_env *env,u32 command,u32 value) {
    if (!env || !env->memory) wasm_rt_trap(WASM_RT_TRAP_OOB);
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
    case HG_IRQ_WAIT:
    case HG_IRQ_WAIT_CLEAR:
        if (!value || (value&~((1u<<OS_IRQ_TABLE_MAX)-1)) || OS_GetCpsrIrq()==OS_INTRMODE_IRQ_DISABLE) wasm_rt_trap(WASM_RT_TRAP_OOB);
        OS_WaitIrq(command==HG_IRQ_WAIT_CLEAR,value); return 0;
    }
    wasm_rt_trap(WASM_RT_TRAP_OOB);
}
void HGNativeReleaseIrqCallbacks(struct w2c_env *env) {
    if (!env || __atomic_load_n(&env->pendingRTC,__ATOMIC_ACQUIRE) ||
        __atomic_load_n(&env->pendingDMA,__ATOMIC_ACQUIRE)) wasm_rt_trap(WASM_RT_TRAP_OOB);
    // A completed copy can still be posting its IRQ after dropping its memory reference.
    HGNativeDrainDMA();
    if (!HGNativeClearIrqBridgeContext(env)) wasm_rt_trap(WASM_RT_TRAP_OOB);
    HGNativeCloseFileContext(env);
    HGNativeCancelAlarmContext(env);
    env->irqTable=NULL; env->irqType0=env->irqType1=env->rtcType=NULL; env->irqDummy=0;
}
