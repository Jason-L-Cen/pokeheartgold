// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef HEARTGOLD_NATIVE_ALARM_H
#define HEARTGOLD_NATIVE_ALARM_H
#include <nitro/types.h>
void HGNativeSetAlarmTimer(void *owner,u32 ticks);
void HGNativeCancelAlarmContext(void *owner);
#endif
