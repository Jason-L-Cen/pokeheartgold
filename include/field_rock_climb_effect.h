#ifndef POKEHEARTGOLD_FIELD_ROCK_CLIMB_EFFECT_H
#define POKEHEARTGOLD_FIELD_ROCK_CLIMB_EFFECT_H

#include "field/field_effect_manager.h"

#include "map_object.h"

// The rock the player climbs on with Rock Climb.

typedef struct RockClimb RockClimb;

RockClimb *RockClimb_New(FieldEffectManager *manager);
void RockClimb_Free(RockClimb *rockClimb);
FieldEffect *RockClimbEffect_Create(LocalMapObject *mapObject, int x, int z, int direction, BOOL follow);
void RockClimbEffect_SetFollow(FieldEffect *effect, BOOL follow);

#endif // POKEHEARTGOLD_FIELD_ROCK_CLIMB_EFFECT_H
