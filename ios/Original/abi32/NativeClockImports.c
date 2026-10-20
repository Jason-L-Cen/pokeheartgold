// SPDX-License-Identifier: GPL-3.0-or-later
#include "NativeImports.h"
#include <nitro/os/tick.h>
#include <nitro/os/common/entropy.h>
#include <nitro/os/native_clock.h>
#include "../NativeAlarm.h"

u64 w2c_env_HGNativeTickControl(struct w2c_env *env,u32 command,u64 value) {
    if (!env || !env->memory) wasm_rt_trap(WASM_RT_TRAP_OOB);
    switch (command) {
    case HG_TICK_INIT: OS_InitTick(); return 0;
    case HG_TICK_GET:
        if (!OS_IsTickAvailable()) wasm_rt_trap(WASM_RT_TRAP_OOB);
        return OS_GetTick();
    case HG_TICK_SET: OS_SetTick(value); return 0;
    case HG_TICK_MONOTONIC:
        if (value) wasm_rt_trap(WASM_RT_TRAP_OOB);
        return HGNativeMonotonicTick();
    }
    wasm_rt_trap(WASM_RT_TRAP_OOB);
}
void w2c_env_HGNativeEntropy(struct w2c_env *env,u32 address) {
    if (!env || !env->memory || !env->memory->data || !address || address>env->memory->size ||
        OS_LOW_ENTROPY_DATA_SIZE>env->memory->size-address) wasm_rt_trap(WASM_RT_TRAP_OOB);
    if (address&3) wasm_rt_trap(WASM_RT_TRAP_UNALIGNED);
    OS_GetLowEntropyData((u32 *)(env->memory->data+address));
}
void w2c_env_HGNativeAlarmTimer(struct w2c_env *env,u32 ticks) {
    if (!env || !env->memory || ticks>0x10000) wasm_rt_trap(WASM_RT_TRAP_OOB);
    HGNativeSetAlarmTimer(env,ticks);
}
