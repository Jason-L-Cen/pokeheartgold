// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
#include <nitro/types.h>
#ifdef __cplusplus
extern "C" {
#endif
// Nintendo LZ10/LZ11 with explicit bounds. Discard partial output on FALSE.
BOOL HGNativeUncompressLZ(const void *source, u32 sourceSize, void *destination, u32 destinationSize);
#ifdef __cplusplus
}
#endif
