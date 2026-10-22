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

FieldEffectPool *FieldEffectPool_New(enum HeapID heapID, int count);
void FieldEffectPool_Delete(FieldEffectPool *pool);
void FieldEffectPool_DrawAll(FieldEffectPool *pool);
FieldEffect *FieldEffectPool_AddBasicEffect(FieldEffectPool *pool, const FieldEffectBasicTemplate *template, const VecFx32 *position, int parameter, void *userData, u32 priority);
void FieldEffect_Destroy(FieldEffect *effect);
void *FieldEffect_GetUserData(FieldEffect *effect);
void FieldEffect_GetPosition(FieldEffect *effect, VecFx32 *position);
void FieldEffect_DefaultDraw(FieldEffect *effect, void *state);
void FieldEffect_DefaultDestroy(FieldEffect *effect, void *state);

#endif // POKEHEARTGOLD_FIELD_EFFECTS_H
