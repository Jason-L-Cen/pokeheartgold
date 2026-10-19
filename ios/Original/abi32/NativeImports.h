// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef HEARTGOLD_ABI32_NATIVE_IMPORTS_H
#define HEARTGOLD_ABI32_NATIVE_IMPORTS_H
#include "wasm-rt.h"

// The owner keeps this memory buffer alive and fixed while concurrent leaf calls run.
struct w2c_env { wasm_rt_memory_t *memory; };
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
#ifdef __cplusplus
}
#endif
#endif
