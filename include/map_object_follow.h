#ifndef POKEHEARTGOLD_MAP_OBJECT_FOLLOW_H
#define POKEHEARTGOLD_MAP_OBJECT_FOLLOW_H

#include "map_object.h"
#include "field_effects.h"

LocalMapObject *MapObject_FindTrainerPartner(LocalMapObject *object);
void HiddenObjectMovement_SetCoverEffect(LocalMapObject *object, FieldEffect *effect);
FieldEffect *HiddenObjectMovement_GetCoverEffect(LocalMapObject *object);

#endif // POKEHEARTGOLD_MAP_OBJECT_FOLLOW_H
