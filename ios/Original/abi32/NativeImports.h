// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef HEARTGOLD_ABI32_NATIVE_IMPORTS_H
#define HEARTGOLD_ABI32_NATIVE_IMPORTS_H
#include "wasm-rt.h"

// Keep memory fixed/alive through asynchronous transfers. Stop new calls and drain/stop DMA
// before HGNativeReleaseIrqCallbacks; release callbacks before freeing the instance/memory.
// Bind stackPointer to the generated module's stack global and irqGuardType to void(i32,i32,i32,i32)
// before the guest initializes IRQs; stack/table bindings must remain stable while callbacks run.
struct w2c_env {
    wasm_rt_memory_t *memory;
    wasm_rt_funcref_table_t *irqTable;
    wasm_rt_func_type_t irqType0,irqType1;
    uint32_t irqDummy;
    wasm_rt_func_type_t rtcType;
    uint32_t pendingRTC;
    uint32_t pendingDMA;
    uint32_t pendingMIC;
    struct HGNativeMicContext *mic;
    uint32_t pendingPOWER;
    struct HGNativePowerContext *power;
    uint32_t pendingINPUT;
    struct HGNativeInputContext *input;
    uint32_t pendingCARD;
    struct HGNativeCardContext *card;
    struct HGNativeSoundContext *sound;
    uint32_t soundRealtime;
    uint32_t *stackPointer;
    wasm_rt_func_type_t irqGuardType;
    uint32_t irqGuard,irqStackStart,irqStackSize,irqActive;
    uint32_t rootStackHigh;
    uint32_t resetRequested,resetParameter,resetFinished,resetOccurred,bootParameter;
    struct HGNativeThreadContext *threads;
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
void w2c_env_HGNativeIrqInit(struct w2c_env *,uint32_t configuration);
void HGNativeInvokeIrqCallback(struct w2c_env *,uint32_t function,uint32_t first,uint32_t second,uint32_t arity);
uint32_t HGNativeCheckedStackPointer(struct w2c_env *,uint32_t value);
uint32_t HGNativeCheckThreadStack(struct w2c_env *,uint32_t value);
uint32_t w2c_env_HGNativeThreadRequest(struct w2c_env *,uint32_t command,uint32_t thread,uint32_t first,uint32_t second);
void HGNativeCloseThreadContext(struct w2c_env *);
uint32_t w2c_env_HGNativeResetControl(struct w2c_env *,uint32_t command,uint32_t parameter);
// Owner/root thread only after a reset trap; drain replies before releasing module memory.
void HGNativeFinishReset(struct w2c_env *);
uint32_t w2c_env_HGNativeIrqControl(struct w2c_env *,uint32_t command,uint32_t value);
void w2c_env_HGNativeIrqSet(struct w2c_env *,uint32_t kind,uint32_t target,uint32_t function,uint32_t argument);
uint32_t w2c_env_HGNativeIrqGet(struct w2c_env *,uint32_t mask);
void HGNativeReleaseIrqCallbacks(struct w2c_env *);
uint32_t w2c_env_HGNativeRtcRequest(struct w2c_env *,uint32_t command,uint32_t date,uint32_t time,uint32_t callback,uint32_t argument);
uint64_t w2c_env_HGNativeTickControl(struct w2c_env *,uint32_t command,uint64_t value);
void w2c_env_HGNativeEntropy(struct w2c_env *,uint32_t output);
void w2c_env_HGNativeAlarmTimer(struct w2c_env *,uint32_t ticks);
void w2c_env_HGNativeDmaRequest(struct w2c_env *,uint32_t command,uint32_t channel,uint32_t source,
                              uint32_t destination,uint32_t control,uint32_t ranges,uint32_t count);
uint32_t w2c_env_HGNativeFileRequest(struct w2c_env *,uint32_t command,uint32_t file,
                                  uint32_t first,uint32_t second,uint32_t third);
void HGNativeCloseFileContext(struct w2c_env *);
uint32_t w2c_env_HGNativeMicRequest(struct w2c_env *,uint32_t command,uint32_t buffer,uint32_t size,uint32_t rate,uint32_t flags,uint32_t callback);
// Owner thread only, with mono PCM already resampled to the configured MIC timer rate.
uint32_t HGNativeFeedMic(struct w2c_env *,const float *,uint32_t count);
void HGNativeCloseMicContext(struct w2c_env *);
uint32_t w2c_env_HGNativePowerRequest(struct w2c_env *,uint32_t command,uint32_t first,uint32_t second,uint32_t callback);
void HGNativeClosePowerContext(struct w2c_env *);
uint32_t w2c_env_HGNativeInputRequest(struct w2c_env *,uint32_t command,uint32_t first,uint32_t second,uint32_t callback);
// Owner thread only. Coordinates are logical 256x192 pixels; the SDK retains its raw/calibrated conversion.
int HGNativeSetInput(struct w2c_env *,uint32_t keys,int folded,uint32_t x,uint32_t y,int pressed);
// The native input producer supplies samples at this rate/scanline phase; no sampling timer is started here.
uint32_t HGNativeInputSamplingRate(struct w2c_env *,uint32_t *vcount);
int HGNativeSampleInput(struct w2c_env *);
void HGNativeCloseInputContext(struct w2c_env *);
uint32_t w2c_env_HGNativeCardRequest(struct w2c_env *,uint32_t request,uint32_t command,uint32_t mode,uint32_t retry,uint32_t callback,uint32_t argument);
void HGNativeDrainCard(struct w2c_env *);
void HGNativeCloseCardContext(struct w2c_env *);
uint32_t w2c_env_HGNativeSoundRequest(struct w2c_env *,uint32_t command,uint32_t list);
// Owner thread only; output is fixed-rate 48 kHz interleaved signed stereo for the native audio sink.
int HGNativeSoundRender(struct w2c_env *,int16_t *output,size_t frames);
// Exclusive owner-thread mode for headless boot: advance/render elapsed PCM without a device sink.
int HGNativeSoundRealtime(struct w2c_env *,int enabled);
void HGNativeCloseSoundContext(struct w2c_env *);
uint32_t w2c_env_HGNativeDwcSettings(struct w2c_env *,uint32_t offset,uint32_t buffer,uint32_t size,uint32_t write);
uint64_t w2c_env_HGNativeDwcIdentity(struct w2c_env *);
#ifdef __cplusplus
}
#endif
#endif
