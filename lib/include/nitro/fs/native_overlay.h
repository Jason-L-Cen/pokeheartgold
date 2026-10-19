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
    u32 functionCount;
    const u32 *functions;
} HGNativeOverlayUnit;
typedef const HGNativeOverlayUnit *(*HGNativeOverlayGetter)(void);
typedef struct HGNativeOverlayEntry { u32 unitCount; const HGNativeOverlayGetter *units; BOOL ready; } HGNativeOverlayEntry;
extern const HGNativeOverlayEntry HGNativeOverlays[];
extern const u32 HGNativeOverlayCount;
BOOL HGNativeValidateOverlayUnit(const HGNativeOverlayUnit *unit);
BOOL HGNativeRestoreOverlayUnit(const HGNativeOverlayUnit *unit);
struct FSFile;
BOOL HGNativeIsOverlayFile(const struct FSFile *file);
BOOL HGNativeCloseOverlayFile(struct FSFile *file);
#endif
