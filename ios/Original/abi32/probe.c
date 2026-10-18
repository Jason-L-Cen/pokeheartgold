// SPDX-License-Identifier: GPL-3.0-or-later
#include "pokemon.h"
#include "math_util.h"
#include "overlay45_string_search.h"
#include "orbit_cursor.h"
#include "wifi_setup_ui.h"
#include "../../../lib/NitroDWC/src/tree-iterator4.h"
#include <stddef.h>

u32 CalcMonChecksum(void *data, u32 size);
static u16 data[128];
static StringSearch string;
static Iterator iterator;

u32 HGAotLayout(u32 field) {
    switch (field) {
    case 0: return sizeof(void *);
    case 1: return sizeof(BoxPokemon);
    case 2: return sizeof(Pokemon);
    case 3: return sizeof(StringSearch);
    case 4: return sizeof(Iterator);
    case 5: return offsetof(StringSearch, raw.longForm.data);
    case 6: return sizeof(FSFile);
    case 7: return offsetof(WifiSetupArchiveState, lockId);
    case 8: return sizeof(OrbitCursorSprite);
    case 9: return sizeof(OrbitCursor);
    default: return 0;
    }
}
u32 HGAotPointerCheck(u32 offset, u32 size) {
    u32 start = (uaddr)data, end = start + sizeof(data);
    if (offset < start || offset > end || size > 32 || size > end - offset) return FALSE;
    string.raw.longForm.flag = 1; string.raw.longForm.capacity = 32;
    string.raw.longForm.size = size; string.raw.longForm.data = (s8 *)(uaddr)offset;
    iterator.current = &string;
    // Exercise the real 32-bit byte offsets, rather than weakening their layout assertions.
    return *(s8 **)((u8 *)iterator.current + 8) == (s8 *)(uaddr)offset && string.raw.longForm.size == size;
}
u32 HGAotString(void) { return (uaddr)&string; }
u32 HGAotIterator(void) { return (uaddr)&iterator; }
u32 HGAotData(void) { return (uaddr)data; }
static BOOL valid_size(u32 size) { return size <= sizeof(data) && !(size & 1); }
u32 HGAotEncrypt(u32 size, u32 seed) {
    if (!valid_size(size)) return FALSE;
    _MonEncryptSegment(data, size, seed); return TRUE;
}
u32 HGAotDecrypt(u32 size, u32 seed) {
    if (!valid_size(size)) return FALSE;
    _MonDecryptSegment(data, size, seed); return TRUE;
}
u32 HGAotChecksum(u32 size) { return valid_size(size) ? CalcMonChecksum(data, size) : UINT32_MAX; }
void HGAotSeed(u32 seed) { SetLCRNGSeed(seed); }
u32 HGAotRandom(void) { return LCRandom(); }
void HGAotMTSeed(u32 seed) { SetMTRNGSeed(seed); }
u32 HGAotMTRandom(void) { return MTRandom(); }
