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
typedef struct { s32 clip[4]; s16 uv[2]; u16 color,reserved; } HGNativeGeometryVertex;
typedef struct { u16 indices[4]; u32 count,attributes,texture,palette,viewport; } HGNativeGeometryPolygon;
typedef struct { u32 vertices,polygons,serial,swapMode,clearColor,clearDepth; } HGNativeGeometryFrameInfo;
// which=0 reads the building bank, which=1 reads the most recently published bank.
BOOL HGNativeGeometryFrame(u32 which,HGNativeGeometryFrameInfo *output);
BOOL HGNativeGeometryReadVertex(u32 which,u32 index,HGNativeGeometryVertex *output);
BOOL HGNativeGeometryReadPolygon(u32 which,u32 index,HGNativeGeometryPolygon *output);
void HGNativeGeometryClear(u32 color,u32 depth);
// Game-owner renderer. FALSE leaves the caller's output unchanged.
BOOL HGNativeRender3D(u32 which,u8 *rgba,u32 size);
#ifdef __cplusplus
}
#endif
#endif
