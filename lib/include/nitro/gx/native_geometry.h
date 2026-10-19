// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef NITRO_GX_NATIVE_GEOMETRY_H
#define NITRO_GX_NATIVE_GEOMETRY_H
#include <nitro/types.h>
#ifdef __cplusplus
extern "C" {
#endif
void HGNativeGeometryReset(void);
BOOL HGNativeGeometryCommand(u32 opcode,const void *parameters,u32 count);
BOOL HGNativeGeometryFifo(const void *words,u32 count);
BOOL HGNativeGeometryMatrix(u32 which,void *output); // 0: clip44, 1: vector33, 2: texture44
u32 HGNativeGeometryStatus(BOOL clearError);
#ifdef __cplusplus
}
#endif
#endif
