#ifndef POKEHEARTGOLD_FIELD_APRICORN_REGROWTH_H
#define POKEHEARTGOLD_FIELD_APRICORN_REGROWTH_H

#include "field/field_effect_manager.h"

#include "map_object.h"

// The billboard an apricorn tree shows when an apricorn grows back.

typedef struct ApricornRegrowth ApricornRegrowth;

ApricornRegrowth *ApricornRegrowth_New(FieldEffectManager *manager);
void ApricornRegrowth_Free(ApricornRegrowth *regrowth);
void ApricornRegrowth_Spawn(LocalMapObject *mapObject);

#endif // POKEHEARTGOLD_FIELD_APRICORN_REGROWTH_H
