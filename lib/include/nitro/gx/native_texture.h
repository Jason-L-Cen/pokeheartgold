// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef NITRO_GX_NATIVE_TEXTURE_H
#define NITRO_GX_NATIVE_TEXTURE_H
#include <nitro/types.h>
#ifdef __cplusplus
extern "C" {
#endif
// Coordinates are signed 12.4 texel units; outputs are RGB555 and 5-bit alpha.
BOOL HGNativeSampleTexture(u32 texture,u32 palette,s32 s,s32 t,u16 *color,u16 *alpha);
#ifdef __cplusplus
}
#endif
#endif
