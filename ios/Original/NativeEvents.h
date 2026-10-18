// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef HEARTGOLD_NATIVE_EVENTS_H
#define HEARTGOLD_NATIVE_EVENTS_H
#include <nitro/os/interrupt.h>
#include <nitro/os/system.h>

#ifdef __cplusplus
extern "C" {
#endif
void HGNativePostIrq(OSIrqMask flags);
void HGNativeSignalVBlank(void);
void HGNativeDispatchIrqs(void);
OSProcMode HGNativeSetExecutionMode(OSProcMode mode);
#ifdef __cplusplus
}
#endif
#endif
