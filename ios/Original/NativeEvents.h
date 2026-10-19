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
OSProcMode HGNativeSetExecutionMode(OSProcMode mode);
#ifdef __cplusplus
}
#endif
#endif
