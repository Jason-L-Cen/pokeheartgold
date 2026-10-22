#ifndef POKEHEARTGOLD_BILLBOARD_INTERNAL_H
#define POKEHEARTGOLD_BILLBOARD_INTERNAL_H

#include "global.h"

#include "billboard_gfx_sequence.h"
#include "gf_3d_render.h"
#include "heap.h"

typedef struct Billboard Billboard;
typedef struct BillboardObject BillboardObject;
typedef void (*BillboardCallback)(BillboardObject *, void *);

// The first and last frame of one of a billboard's sequences; with stopAtEnd set it stays on its last frame
// instead of looping.
typedef struct BillboardSequenceRange {
    s32 start;
    s32 end;
    s32 stopAtEnd;
} BillboardSequenceRange;

// The graphics of a billboard: its model file, the texture file holding every frame's texture and palette, the
// frame ranges of its sequences and which texture/palette each frame shows. With texKey set, the texture is
// already in VRAM under these keys.
typedef struct BillboardResource {
    void *modelFile;
    NNSG3dResTex *texture;
    const BillboardSequenceRange *ranges;
    BillboardGfxSequence gfxSequence;
    NNSG3dTexKey texKey;
    NNSG3dTexKey tex4x4Key;
    NNSG3dPlttKey plttKey;
} BillboardResource;

struct BillboardObject {
    VecFx32 position;
    VecFx32 scale;
    MtxFx33 *rotation;
    void *callbackArg;
    BillboardCallback callback;
    u8 visible;
    u8 padding_25[3];
    Billboard *owner;
    const BillboardSequenceRange *ranges;
    NNSG3dRenderObj renderObj;
    NNSG3dResMdlSet *modelSet;
    NNSG3dResMdl *model;
    NNSG3dResTex *texture;
    NNSG3dResTex *resourceTexture;
    NNSG3dTexKey texKey;
    NNSG3dTexKey tex4x4Key;
    NNSG3dPlttKey paletteKey;
    BillboardGfxSequence gfxSequence;
    void *textureAnimation;
    u8 type;
    u8 padding_B5;
    u16 sequence;
    fx32 frame;
    BillboardObject *next;
    BillboardObject *prev;
};

struct Billboard {
    u8 initialized;
    u8 draw;
    u8 padding_02;
    u8 state;
    BillboardObject *objects;
    int objectCount;
    BillboardObject root;
    BillboardObject **freeObjects;
    u32 usedCount;
    NNSFndAllocator *allocator;
    void *textureAnimationPool;
};

typedef struct BillboardListManager {
    Billboard *list;
    int count;
} BillboardListManager;

typedef struct BillboardCreateParams {
    int count;
    enum HeapID heapId;
} BillboardCreateParams;

typedef struct BillboardObjectCreateParams {
    Billboard *billboard;
    BillboardResource *resource;
    VecFx32 position;
    VecFx32 scale;
} BillboardObjectCreateParams;

Billboard *Billboard_Create(BillboardCreateParams *params);
BOOL Billboard_Delete(Billboard *billboard);
BOOL Billboard_HasPendingFree(Billboard *billboard);
BillboardObject *BillboardObject_Create(BillboardObjectCreateParams *params);
BOOL BillboardObject_Delete(BillboardObject *object);
void BillboardObject_SetPosition(BillboardObject *object, const VecFx32 *position);
VecFx32 *BillboardObject_GetPosition(BillboardObject *object);
void BillboardObject_SetScale(BillboardObject *object, VecFx32 *scale);
VecFx32 *BillboardObject_GetScale(BillboardObject *object);
u32 BillboardObject_IsVisible(BillboardObject *object);
void BillboardObject_SetSequence(BillboardObject *object, u32 sequence);
u32 BillboardObject_GetSequence(BillboardObject *object);
// TRUE once a non-looping sequence has reached its last frame.
BOOL BillboardObject_AdvanceFrame(BillboardObject *object, fx32 delta);
void BillboardObject_SetFrame(BillboardObject *object, fx32 frame);
fx32 BillboardObject_GetFrame(BillboardObject *object);
void BillboardObject_SetSequenceFrame(BillboardObject *object, fx32 frame);
fx32 BillboardObject_GetSequenceFrame(BillboardObject *object);
NNSG3dTexKey BillboardObject_GetTexKey(BillboardObject *object);
NNSG3dPlttKey BillboardObject_GetPlttKey(BillboardObject *object);
void BillboardObject_UpdateTextureAnimation(BillboardObject *object);
NNSG3dResMdl *BillboardObject_GetModelNoCast(BillboardObject *object);
void BillboardObject_SetCallback(BillboardObject *object, BillboardCallback callback, void *callbackArg);
void BillboardObject_SetResource(BillboardObject *object, BillboardResource *resource);
void BillboardResource_InitWithKeys(BillboardResource *resource, void *modelFile, NNSG3dResTex *texture, const BillboardSequenceRange *ranges, BillboardGfxSequence *gfxSequence, NNSG3dTexKey texKey, NNSG3dTexKey tex4x4Key, NNSG3dPlttKey plttKey);
void BillboardResource_Init(BillboardResource *resource, void *modelFile, NNSG3dResTex *texture, const BillboardSequenceRange *ranges, BillboardGfxSequence *gfxSequence);

#endif // POKEHEARTGOLD_BILLBOARD_INTERNAL_H
