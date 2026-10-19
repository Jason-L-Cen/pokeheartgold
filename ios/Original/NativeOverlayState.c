// SPDX-License-Identifier: GPL-3.0-or-later
#include <nitro/fs/native_overlay.h>
#include <string.h>

_Static_assert(sizeof(HGNativeOverlaySpan)==12 && sizeof(HGNativeOverlayFunction)==8 && sizeof(HGNativeOverlayUnit)==32,
    "Native overlay metadata preserves the original pointer ABI");
static BOOL span(const void *pointer,u64 size) {
    u64 address=(uaddr)pointer,limit=(u64)__builtin_wasm_memory_size(0)*65536;
    return pointer && address<=limit && size<=limit-address;
}
BOOL HGNativeValidateOverlayUnit(const HGNativeOverlayUnit *unit) {
    if ((uaddr)unit%4 || !span(unit,sizeof(*unit))) return FALSE;
    if (unit->spanCount && ((uaddr)unit->spans%4 ||
        !span(unit->spans,(u64)unit->spanCount*sizeof(*unit->spans)))) return FALSE;
    if ((unit->constructorCount && ((uaddr)unit->constructors%4 || !span(unit->constructors,(u64)unit->constructorCount*8))) ||
        (unit->destructorCount && ((uaddr)unit->destructors%4 || !span(unit->destructors,(u64)unit->destructorCount*8))) ||
        (unit->functionCount && ((uaddr)unit->functions%4 || !span(unit->functions,(u64)unit->functionCount*4)))) return FALSE;
    for (u32 i=0;i<unit->constructorCount;i++) if (!unit->constructors[i].function) return FALSE;
    for (u32 i=0;i<unit->destructorCount;i++) if (!unit->destructors[i].function) return FALSE;
    // Metadata is compiler generated; validate every range before modifying any overlay state.
    for (u32 i=0;i<unit->spanCount;i++) {
        const HGNativeOverlaySpan *part=&unit->spans[i];
        if (!span(part->address,part->size) || (part->initial && !span(part->initial,part->size))) return FALSE;
    }
    return TRUE;
}
BOOL HGNativeRestoreOverlayUnit(const HGNativeOverlayUnit *unit) {
    if (!HGNativeValidateOverlayUnit(unit)) return FALSE;
    for (u32 i=0;i<unit->spanCount;i++) {
        const HGNativeOverlaySpan *part=&unit->spans[i];
        if (part->initial) memcpy(part->address,part->initial,part->size);
        else memset(part->address,0,part->size);
    }
    return TRUE;
}
