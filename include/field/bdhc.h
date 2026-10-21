#ifndef POKEHEARTGOLD_FIELD_BDHC_H
#define POKEHEARTGOLD_FIELD_BDHC_H

#include "global.h"

#include "filesystem.h"
#include "sys_task.h"

// A map's terrain height data (BDHC): sloped plates over the x/z plane, sorted into strips along z.

// A corner of a plate's bounding rectangle.
typedef struct BdhcPoint {
    fx32 x;
    fx32 z;
} BdhcPoint;

typedef struct BdhcPlate {
    u16 firstPoint; // the two opposite corners of the plate's bounding rectangle
    u16 secondPoint;
    u16 normal;
    u16 constant; // plane: normal . (x, y, z) + constant = 0
} BdhcPlate;

// The plates (access list entries [offset, offset + count)) of the strip that starts at lowerBound along z.
typedef struct BdhcStrip {
    fx32 lowerBound;
    u16 count;
    u16 offset;
} BdhcStrip;

// The arrays point into one buffer the caller provides (at most 0x9000 bytes).
typedef struct Bdhc {
    BdhcPlate *plates;
    fx32 *constants;
    BdhcStrip *strips;
    u16 *accessList;
    BdhcPoint *points;
    VecFx32 *normals;
    BOOL loaded;
    u32 stripsCount;
} Bdhc;

BOOL Bdhc_GetHeight(int mode, fx32 currentY, fx32 x, fx32 z, Bdhc *bdhc, fx32 *y);
Bdhc *Bdhc_New(void);
void Bdhc_Load(NARC *narc, u32 unused, Bdhc *bdhc, u8 *buffer);
void Bdhc_Free(Bdhc *bdhc);
void Bdhc_Reset(Bdhc *bdhc);
// Loads the BDHC once *modelPending is cleared (the map model is loaded); *pending is cleared when done.
SysTask *Bdhc_StartLoadTask(NARC *narc, u32 unused, Bdhc *bdhc, BOOL *pending, u8 **buffer, BOOL *modelPending);
void Bdhc_CancelLoadTask(SysTask *task);
void Bdhc_SetUnloaded(Bdhc *bdhc);

#endif // POKEHEARTGOLD_FIELD_BDHC_H
