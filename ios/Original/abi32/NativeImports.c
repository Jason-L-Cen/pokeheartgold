// SPDX-License-Identifier: GPL-3.0-or-later
#include "NativeImports.h"
#include <nitro/os/system.h>

uint32_t MI_SwapWord(uint32_t value,volatile uint32_t *address);
int32_t OS_GetLockID(void);
void OS_ReleaseLockID(uint16_t id);
void DC_WaitWriteBufferEmpty(void);

uint32_t w2c_env_HGNativeAtomicSwapWord(struct w2c_env *env,uint32_t value,uint32_t address) {
    wasm_rt_memory_t *memory=env ? env->memory : NULL;
    if (!memory || !memory->data || address>memory->size || 4>memory->size-address) wasm_rt_trap(WASM_RT_TRAP_OOB);
    if (address&3) wasm_rt_trap(WASM_RT_TRAP_UNALIGNED);
    return MI_SwapWord(value,(volatile uint32_t *)(memory->data+address));
}

uint32_t w2c_env_HGNativeGetLockID(struct w2c_env *env) {
    if (!env || !env->memory) wasm_rt_trap(WASM_RT_TRAP_OOB);
    return (uint32_t)OS_GetLockID();
}
void w2c_env_HGNativeReleaseLockID(struct w2c_env *env,uint32_t id) {
    if (!env || !env->memory || id>UINT16_MAX) wasm_rt_trap(WASM_RT_TRAP_OOB);
    OS_ReleaseLockID((uint16_t)id);
}
void w2c_env_HGNativeMemoryBarrier(struct w2c_env *env) {
    if (!env || !env->memory) wasm_rt_trap(WASM_RT_TRAP_OOB);
    DC_WaitWriteBufferEmpty();
}
uint32_t w2c_env_HGNativeEnableInterrupts(struct w2c_env *env) {
    if (!env || !env->memory) wasm_rt_trap(WASM_RT_TRAP_OOB);
    return OS_EnableInterrupts();
}
uint32_t w2c_env_HGNativeDisableInterrupts(struct w2c_env *env) {
    if (!env || !env->memory) wasm_rt_trap(WASM_RT_TRAP_OOB);
    return OS_DisableInterrupts();
}
uint32_t w2c_env_HGNativeRestoreInterrupts(struct w2c_env *env,uint32_t state) {
    if (!env || !env->memory || (state&~OS_INTRMODE_DISABLE_IRQ)) wasm_rt_trap(WASM_RT_TRAP_OOB);
    return OS_RestoreInterrupts((OSIntrMode)state);
}
uint32_t w2c_env_HGNativeDisableInterrupts_IrqAndFiq(struct w2c_env *env) {
    if (!env || !env->memory) wasm_rt_trap(WASM_RT_TRAP_OOB);
    return OS_DisableInterrupts_IrqAndFiq();
}
uint32_t w2c_env_HGNativeRestoreInterrupts_IrqAndFiq(struct w2c_env *env,uint32_t state) {
    if (!env || !env->memory || (state&~(OS_INTRMODE_DISABLE_IRQ|OS_INTRMODE_DISABLE_FIQ))) wasm_rt_trap(WASM_RT_TRAP_OOB);
    return OS_RestoreInterrupts_IrqAndFiq((OSIntrMode)state);
}
uint32_t w2c_env_HGNativeGetCpsrIrq(struct w2c_env *env) {
    if (!env || !env->memory) wasm_rt_trap(WASM_RT_TRAP_OOB);
    return OS_GetCpsrIrq();
}
uint32_t w2c_env_HGNativeGetProcMode(struct w2c_env *env) {
    if (!env || !env->memory) wasm_rt_trap(WASM_RT_TRAP_OOB);
    return OS_GetProcMode();
}
void w2c_env_HGNativeSpinWait(struct w2c_env *env,uint32_t cycles) {
    if (!env || !env->memory) wasm_rt_trap(WASM_RT_TRAP_OOB);
    OS_SpinWait(cycles);
}
