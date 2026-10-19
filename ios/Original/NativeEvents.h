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
#ifdef __cplusplus
}
#endif
#endif
