#ifndef POKEHEARTGOLD_FIELD_FIELD_FLASH_DARKNESS_H
#define POKEHEARTGOLD_FIELD_FIELD_FLASH_DARKNESS_H

#include "field/field_effect_manager.h"

#include "map_object.h"

typedef struct FlashDarkness FlashDarkness;

FieldEffect *FlashDarkness_Show(LocalMapObject *mapObject);
void FlashDarkness_SetScale(FieldEffect *effect, float scale);

FlashDarkness *FlashDarkness_New(FieldEffectManager *manager);
void FlashDarkness_Free(FlashDarkness *darkness);

#endif // POKEHEARTGOLD_FIELD_FIELD_FLASH_DARKNESS_H
