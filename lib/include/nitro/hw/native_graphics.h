// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef HEARTGOLD_NATIVE_GRAPHICS_H
#define HEARTGOLD_NATIVE_GRAPHICS_H

#include <nitro/types.h>

#ifdef __cplusplus
extern "C" {
#endif
uaddr HGNativeRegisterAddress(u32 address, u32 width);
// Returns the contiguous native span, shortening size at a graphics bank boundary.
// Unmapped/unsupported DS addresses return NULL; native pointers pass through.
void *HGNativeMemorySpan(uaddr address, u32 *size);
void HGNativeSnapshotGraphics(u32 address, void *destination, u32 size);
const void *HGNativeGraphicsBank(u32 bank);
// GPU texture/palette views; unmapped bytes read zero, overlapping banks are ORed.
BOOL HGNativeReadTextureMemory(BOOL palette,u32 address,void *output,u32 size);
// Game-thread renderer. FALSE leaves output untouched for invalid/unsupported modes.
BOOL HGNativeRender2D(u32 engine, u8 *rgba, u32 size);
#ifdef __cplusplus
}
#endif
#endif
