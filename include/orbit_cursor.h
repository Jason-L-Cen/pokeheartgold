#ifndef POKEHEARTGOLD_ORBIT_CURSOR_H
#define POKEHEARTGOLD_ORBIT_CURSOR_H

#include "global.h"

#include "filesystem.h"
#include "heap.h"
#include "palette.h"
#include "sprite_system.h"
#include "sys_task_api.h"

typedef struct OrbitCursorArgs {
    int count;
    enum HeapID heapId;
    int resBase;
    int fixedPalette;
    fx32 yOffset;
} OrbitCursorArgs;

typedef struct OrbitCursorSprite {
    ManagedSprite *sprite;
    s16 x;
    s16 y;
    int orbitPhase;
    int angle;
} OrbitCursorSprite;

typedef struct OrbitCursor {
    NARC *narc;
    SpriteSystem *spriteSys;
    SpriteManager *spriteMan;
    PaletteData *plttData;
    int count;
    enum HeapID heapId;
    int resBase;
    int fixedPalette;
    fx32 yOffset;
    OrbitCursorSprite sprites[12];
    int state;
    int leadSprite;
    int orbitingCount;
    int active;
    SysTask *task;
    int unk_F8;
    int radiusX;
    int radiusY;
    int unk_104;
} OrbitCursor;

OrbitCursor *OrbitCursor_New(OrbitCursorArgs args, SpriteSystem *spriteSys, SpriteManager *spriteMan, PaletteData *plttData);
void OrbitCursor_ReloadPalette(OrbitCursor *p, int palette);
void OrbitCursor_LoadResources(OrbitCursor *p, int palette);
void OrbitCursor_CreateSprites(OrbitCursor *p);
void OrbitCursor_DeleteSprites(OrbitCursor *p);
BOOL OrbitCursor_Start(OrbitCursor *p, int palette);
BOOL OrbitCursor_SetActive(OrbitCursor *p, int active);
BOOL OrbitCursor_SetTarget(OrbitCursor *p, int x, int y);
BOOL OrbitCursor_MoveTarget(OrbitCursor *p, int x, int y);
BOOL OrbitCursor_IsNewTarget(OrbitCursor *p, int x, int y);
BOOL OrbitCursor_SetPosition(OrbitCursor *p, s16 x, s16 y);
BOOL OrbitCursor_Delete(OrbitCursor *p);
BOOL OrbitCursor_SetVisible(OrbitCursor *p, int flag);
void OrbitCursor_SetRadius(OrbitCursor *p, int radiusX, int radiusY);
void OrbitCursor_SetAltAnim(OrbitCursor *p);
void OrbitCursor_SetDefaultAnim(OrbitCursor *p);
extern void OrbitCursor_Task(SysTask *task, void *data);

extern const int gOrbitCursorPaletteMembers[];

#if defined(HEARTGOLD_NATIVE) && !defined(HEARTGOLD_ABI32)
// These are runtime objects allocated with sizeof, not serialized DS resources.
typedef char AssertTouchSpriteSize[(sizeof(OrbitCursorSprite) == 0x18) ? 1 : -1];
typedef char AssertTouchMotionSize[(sizeof(OrbitCursor) == 0x180) ? 1 : -1];
typedef char AssertTouchSprites[(offsetof(OrbitCursor, sprites) == 0x38) ? 1 : -1];
typedef char AssertTouchState[(offsetof(OrbitCursor, state) == 0x158) ? 1 : -1];
typedef char AssertTouchTask[(offsetof(OrbitCursor, task) == 0x168) ? 1 : -1];
#else
typedef char AssertTouchSpriteSize[(sizeof(OrbitCursorSprite) == 0x10) ? 1 : -1];
typedef char AssertTouchMotionSize[(sizeof(OrbitCursor) == 0x108) ? 1 : -1];
typedef char AssertTouchSprites[(offsetof(OrbitCursor, sprites) == 0x24) ? 1 : -1];
typedef char AssertTouchState[(offsetof(OrbitCursor, state) == 0xE4) ? 1 : -1];
typedef char AssertTouchTask[(offsetof(OrbitCursor, task) == 0xF4) ? 1 : -1];
#endif

#endif
