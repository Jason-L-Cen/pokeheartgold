// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
#include <stdbool.h>
#include <stddef.h>
typedef struct HGNativeFiber HGNativeFiber;
HGNativeFiber *HGNativeFiberRoot(void);
HGNativeFiber *HGNativeFiberCreate(void (*entry)(void *),void *argument);
void HGNativeFiberSwitch(HGNativeFiber *from,HGNativeFiber *to,bool terminate);
// Destroy only an unstarted or terminated fiber, or the currently running borrowed root.
void HGNativeFiberDestroy(HGNativeFiber *fiber);
