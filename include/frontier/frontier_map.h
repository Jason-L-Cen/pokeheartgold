#ifndef POKEHEARTGOLD_FRONTIER_MAP_H
#define POKEHEARTGOLD_FRONTIER_MAP_H

#include "bg_window.h"
#include "gf_3d_vramman.h"
#include "overlay_42.h"
#include "palette.h"
#include "sprite_system.h"
#include "sys_task.h"

#define FRONTIER_MAP_SPRITE_MAX     8
#define FRONTIER_MAP_OBJECT_MAX     32
#define FRONTIER_MAP_OBJECT_GFX_MAX 24

#define FRONTIER_MAP_NO_ID 0xFFFF

// The frontier application's work (frontier_runtime.c)
typedef struct FrontierWork FrontierWork;

// Object placement as the frontier scripts and the save copy store it (15 halfwords)
typedef struct FrontierMapObjectTemplate {
    u16 spriteId;
    u16 action;
    u16 id;
    s16 x;
    s16 y;
    u8 direction;
    u8 visible;
    u8 unk_0C;
    u8 filler_0D[0x11];
} FrontierMapObjectTemplate;

// One of the 32 map objects kept in the frontier app's work (survives a map rebuild)
// Scripted movement of a map object (type 0: none)
typedef struct FrontierMapObjectMovement {
    u8 type;
    u8 state;
    s16 data[7];
} FrontierMapObjectMovement;

typedef struct FrontierMapObject {
    Field2DObject *object;
    Field2DObjectSprite *objectSprite;
    FrontierMapObjectTemplate template;
    FrontierMapObjectMovement movement;
    u8 unk_36[2];
    void *movementTask;
} FrontierMapObject;

// Object graphics loaded into the map's object graphics manager (24 slots)
typedef struct FrontierMapObjectGfx {
    u16 id;
    u8 animSet;
    u8 unk_03;
} FrontierMapObjectGfx;

typedef struct FrontierMapSpriteRecord {
    s16 x;
    s16 y;
    u8 resourceId;
    u8 anim;
    u16 frame : 13;
    u16 animActive : 1;
    u16 draw : 1;
    u16 active : 1;
} FrontierMapSpriteRecord;

// Sprite state saved while the map is torn down for another application
typedef struct FrontierMapSpriteState {
    u16 resourceIds[FRONTIER_MAP_SPRITE_MAX];
    FrontierMapSpriteRecord sprites[FRONTIER_MAP_SPRITE_MAX];
} FrontierMapSpriteState;

// Extra sprites the scripts create on the map (resources from NARC a/1/8/4)
typedef struct FrontierMapSpriteSet {
    ManagedSprite *sprites[FRONTIER_MAP_SPRITE_MAX];
    u16 resourceIds[FRONTIER_MAP_SPRITE_MAX];
    u32 animActiveMask;
    u16 loadedResourceIds[FRONTIER_MAP_SPRITE_MAX];
} FrontierMapSpriteSet;

// Map shake (FrtCmd_ShakeMap): the offsets are added to the scroll and flip sign every interval + 1 frames, count times.
typedef struct FrontierMapShake {
    SysTask *task;
    s16 x;
    s16 y;
    s8 frame;
    s8 interval;
    s8 count;
    u8 unk_0B;
} FrontierMapShake;

// Main screen window 0 set after a delay (FrtCmd_SetWindowDelayed).
typedef struct FrontierMapWindowTask {
    SysTask *task;
    u8 left;
    u8 top;
    u8 right;
    u8 bottom;
    s16 delay;
    s8 enable;
    u8 unk_0B;
} FrontierMapWindowTask;

typedef struct FrontierMap {
    BgConfig *bgConfig;
    PaletteData *paletteData;
    FrontierWork *frontier;
    GF3DVramMan *vramMan;
    struct FrontierParticles *particles; // frontier/frontier_particles.h
    Field2DObjectManager *objectManager;
    Field2DMap *grid;
    Field2DPoint camera;
    Field2DObjectSpriteManager *objectGfx;
    Field2DBgLayer *bg3Layer;
    Field2DBgLayer *bg2Layer;
    Field2DCommandQueue *commandQueue;
    Field2DMoveQueue *moveQueue;
    SpriteSystem *spriteSystem;
    SpriteManager *spriteManager;
    FrontierMapSpriteSet spriteSet;
    ManagedSprite *monSprites[4];
    void *mapEffect; // set up by the map's init callback
    SysTask *objectTask;
    SysTask *eventTask;
    SysTask *updateTask;
    SysTask *vblankTask;
    FrontierMapShake shake;
    FrontierMapWindowTask windowTask;
    u8 unk_BC[5];
    u8 mapId;
    u8 unk_C2[2];
} FrontierMap;

FrontierMap *FrontierMap_Init(FrontierWork *frontier);
void FrontierMap_Free(FrontierMap *map);
void FrontierMap_SaveObjects(FrontierMap *map);
void FrontierMap_RestoreObjects(FrontierMap *map);
void FrontierMap_AddObjectGfx(FrontierMap *map, const FrontierMapObjectGfx *gfx);
void FrontierMap_RemoveObjectGfx(FrontierMap *map, u16 id);
Field2DObject *FrontierMap_CreateObject(FrontierMap *map, const FrontierMapObjectTemplate *template, int index);
void FrontierMap_DeleteObject(FrontierMap *map, Field2DObject *object);
void FrontierMap_GetObjectById(FrontierMap *map, u16 id, Field2DObject **object, Field2DObjectSprite **objectSprite);
void FrontierMap_AddLoadedResource(FrontierMap *map, u16 resourceId);
void FrontierMap_RemoveLoadedResource(FrontierMap *map, u16 resourceId);
void FrontierMap_DeleteSprite(FrontierMap *map, u32 index);
ManagedSprite *FrontierMap_GetSprite(FrontierMap *map, u32 index);
u32 FrontierMap_IsSpriteAnimActive(FrontierMap *map, u16 index);
void FrontierMap_GetCameraPos(FrontierMap *map, u16 *x, u16 *y);
FrontierMapObject *FrontierMap_FindObject(FrontierWork *frontier, u32 id);

typedef struct FrontierMapTileBlink FrontierMapTileBlink;
typedef struct FrontierMapBgAnim FrontierMapBgAnim;

FrontierMapTileBlink *FrontierMapTileBlink_Create(BgConfig *bgConfig);
void FrontierMapTileBlink_Free(FrontierMapTileBlink *blink);
u32 FrontierLinkLobby_GetWinStreakStat(u32 value);
u32 FrontierLinkLobby_GetStreakActiveStat(u32 value);

void FrontierMapSprite_LoadResources(SpriteSystem *spriteSystem, SpriteManager *spriteManager, NARC *narc, PaletteData *paletteData, u16 kind);
void FrontierMapSprite_UnloadResources(SpriteManager *spriteManager, u32 kind);
ManagedSprite *FrontierMapSprite_Create(SpriteSystem *spriteSystem, SpriteManager *spriteManager, u32 kind);
void FrontierMapSprite_Delete(ManagedSprite *sprite);

void FrontierMap_SetObjectMovement(FrontierWork *frontier, u32 id, u32 type, const s16 *params, s32 count);
void FrontierMap_RunObjectMovements(FrontierWork *frontier);

FrontierMapBgAnim *FrontierMapBgAnim_Create(BgConfig *bgConfig, PaletteData *paletteData);
void FrontierMapBgAnim_Free(FrontierMapBgAnim *anim);

// The map parts of the frontier app's work (frontier_runtime.c)
FrontierMap *Frontier_GetMap(FrontierWork *frontier);
void Frontier_RequestExit(FrontierWork *frontier);
void Frontier_ChangeMode(FrontierWork *frontier, u16 mode, u16 taskId);
FrontierMapObjectGfx *Frontier_GetMapObjectGfx(FrontierWork *frontier);
FrontierMapObject *Frontier_GetMapObjects(FrontierWork *frontier);
FrontierMapObject *Frontier_GetMapObject(FrontierWork *frontier, int index);
FrontierMapSpriteState *Frontier_GetMapSpriteState(FrontierWork *frontier);
void Frontier_ResetMapSpriteState(FrontierWork *frontier);

#endif
