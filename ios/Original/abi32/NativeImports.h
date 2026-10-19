// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef HEARTGOLD_ABI32_NATIVE_IMPORTS_H
#define HEARTGOLD_ABI32_NATIVE_IMPORTS_H
#include "wasm-rt.h"

// The owner keeps this memory buffer alive and fixed while concurrent leaf calls run.
struct w2c_env {
    wasm_rt_memory_t *memory;
    wasm_rt_funcref_table_t *irqTable;
    wasm_rt_func_type_t irqType0,irqType1;
    uint32_t irqDummy;
    wasm_rt_func_type_t rtcType;
    uint32_t pendingRTC;
};
#ifdef __cplusplus
extern "C" {
#endif
uint32_t w2c_env_HGNativeAtomicSwapWord(struct w2c_env *,uint32_t value,uint32_t address);
uint32_t w2c_env_HGNativeGetLockID(struct w2c_env *);
void w2c_env_HGNativeReleaseLockID(struct w2c_env *,uint32_t id);
void w2c_env_HGNativeMemoryBarrier(struct w2c_env *);
uint32_t w2c_env_HGNativeEnableInterrupts(struct w2c_env *);
uint32_t w2c_env_HGNativeDisableInterrupts(struct w2c_env *);
uint32_t w2c_env_HGNativeRestoreInterrupts(struct w2c_env *,uint32_t state);
uint32_t w2c_env_HGNativeDisableInterrupts_IrqAndFiq(struct w2c_env *);
uint32_t w2c_env_HGNativeRestoreInterrupts_IrqAndFiq(struct w2c_env *,uint32_t state);
uint32_t w2c_env_HGNativeGetCpsrIrq(struct w2c_env *);
uint32_t w2c_env_HGNativeGetProcMode(struct w2c_env *);
void w2c_env_HGNativeSpinWait(struct w2c_env *,uint32_t cycles);
void w2c_env_HGNativeIrqInit(struct w2c_env *,uint32_t dummy);
uint32_t w2c_env_HGNativeIrqControl(struct w2c_env *,uint32_t command,uint32_t value);
void w2c_env_HGNativeIrqSet(struct w2c_env *,uint32_t kind,uint32_t target,uint32_t function,uint32_t argument);
uint32_t w2c_env_HGNativeIrqGet(struct w2c_env *,uint32_t mask);
void HGNativeReleaseIrqCallbacks(struct w2c_env *);
uint32_t w2c_env_HGNativeRtcRequest(struct w2c_env *,uint32_t command,uint32_t date,uint32_t time,uint32_t callback,uint32_t argument);
uint64_t w2c_env_HGNativeTickControl(struct w2c_env *,uint32_t command,uint64_t value);
void w2c_env_HGNativeEntropy(struct w2c_env *,uint32_t output);
#ifdef __cplusplus
}
#endif
#endif
