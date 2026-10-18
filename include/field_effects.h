#ifndef POKEHEARTGOLD_FIELD_EFFECTS_H
#define POKEHEARTGOLD_FIELD_EFFECTS_H

#include "field_types_def.h"
#include "constants/heap.h"

typedef struct FieldEffect FieldEffect;
typedef struct FieldEffectPool FieldEffectPool;
typedef BOOL (*FieldEffectInitFunc)(FieldEffect *effect, void *state);
typedef void (*FieldEffectFunc)(FieldEffect *effect, void *state);
typedef struct FieldEffectBasicTemplate {
    u32 stateSize;
    FieldEffectInitFunc init;
    FieldEffectFunc destroy;
    FieldEffectFunc update;
    FieldEffectFunc draw;
} FieldEffectBasicTemplate;

typedef struct {
    int unk0;
    u8 unk4[0x8];
    LocalMapObject *unkC;
    int unk10;
} FieldEffectUserDataHeader;

typedef struct {
    FieldEffectUserDataHeader unk0;
} FieldEffectUserData;

FieldEffectPool *FieldEffectPool_New(enum HeapID heapID, int count);
void FieldEffectPool_Delete(FieldEffectPool *pool);
void FieldEffectPool_DrawAll(FieldEffectPool *pool);
FieldEffect *FieldEffectPool_AddBasicEffect(FieldEffectPool *pool, const FieldEffectBasicTemplate *template, const VecFx32 *position, int parameter, void *userData, u32 priority);
void FieldEffect_Destroy(FieldEffect *effect);
FieldEffectUserData *FieldEffect_GetUserData(void *data);
void FieldEffect_GetPosition(void *data, VecFx32 *position);

#endif // POKEHEARTGOLD_FIELD_EFFECTS_H
