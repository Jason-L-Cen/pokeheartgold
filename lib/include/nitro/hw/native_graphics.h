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
#ifdef __cplusplus
}
#endif
#endif
