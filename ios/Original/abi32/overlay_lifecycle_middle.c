// SPDX-License-Identifier: GPL-3.0-or-later
#include <nitro/types.h>
extern void HGOverlayLifecycleObserve(u32 value);
__attribute__((constructor(150))) static void middle(void) { HGOverlayLifecycleObserve(2); }
__attribute__((destructor(150))) static void endMiddle(void) { HGOverlayLifecycleObserve(8); }
