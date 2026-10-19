// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef HEARTGOLD_NATIVE_OVERLAY_STATE_H
#define HEARTGOLD_NATIVE_OVERLAY_STATE_H
#include <nitro/types.h>
typedef struct HGNativeOverlaySpan { void *address; const void *initial; u32 size; } HGNativeOverlaySpan;
typedef struct HGNativeOverlayFunction { u32 priority; void (*function)(void); } HGNativeOverlayFunction;
typedef struct HGNativeOverlayUnit {
    u32 spanCount;
    const HGNativeOverlaySpan *spans;
    u32 constructorCount;
    const HGNativeOverlayFunction *constructors;
    u32 destructorCount;
    const HGNativeOverlayFunction *destructors;
} HGNativeOverlayUnit;
BOOL HGNativeRestoreOverlayUnit(const HGNativeOverlayUnit *unit);
#endif
