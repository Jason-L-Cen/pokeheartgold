// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef HEARTGOLD_NATIVE_EVENTS_H
#define HEARTGOLD_NATIVE_EVENTS_H
#include <nitro/os/interrupt.h>
#include <nitro/os/system.h>

#ifdef __cplusplus
extern "C" {
#endif
void HGNativePostIrq(OSIrqMask flags);
BOOL HGNativeQueueIrqCallback(OSIrqMask flag, void (*callback)(void *), void *argument);
void HGNativeSignalVBlank(void);
// Call from the game owner thread; status must stay alive until Stop completes.
BOOL HGNativeStartDisplayClock(void *context,const vu16 *displayStatus);
void HGNativeStopDisplayClock(void *context);
// Owner-thread callback after a display event and its IRQ scheduling; stop the clock before freeing its context.
BOOL HGNativeSetDisplayObserver(void *owner,void (*callback)(void *),void *argument);
// Register on the IRQ owner, before or after starting its display clock; no guest call occurs here.
BOOL HGNativeSetRasterHook(void *owner,void (*callback)(void *,u32 line,u32 phase),void *argument);
void HGNativeDispatchIrqs(void);
void HGNativeWaitNanoseconds(u64 duration);
typedef struct HGNativeIrqBridge {
    void (*invoke)(void *context,u32 function,u32 argument);
    void *context;
    u32 function,argument;
} HGNativeIrqBridge;
void HGNativeInstallIrqBridge(u32 kind,u32 target,const HGNativeIrqBridge *bridge);
BOOL HGNativeGetIrqBridge(OSIrqMask mask,HGNativeIrqBridge *bridge);
BOOL HGNativeClearIrqBridgeContext(void *context);
OSProcMode HGNativeSetExecutionMode(OSProcMode mode);
OSIntrMode HGNativeSetInterruptState(OSIntrMode value);
OSIntrMode HGNativeGetInterruptState(void);
BOOL HGNativeSetIrqExitHook(void (*callback)(void *),void *context);
BOOL HGNativeClearIrqExitHook(void *context);
void HGNativeHalt(void);
#ifdef __cplusplus
}
#endif
#endif
