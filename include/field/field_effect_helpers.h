#ifndef POKEHEARTGOLD_FIELD_FIELD_EFFECT_HELPERS_H
#define POKEHEARTGOLD_FIELD_FIELD_EFFECT_HELPERS_H

#include "field/field_effect_manager.h"

#include "billboard_internal.h"
#include "render_animation.h"

// Helpers the field effect renderers share: model animations, render objects, effect accessors and billboards.

void *FieldEffect_GetState(FieldEffect *effect);
int FieldEffect_GetParam(FieldEffect *effect);
void FieldEffect_SetPosition(FieldEffect *effect, const VecFx32 *position);
void FieldEffect_RunUpdate(FieldEffect *effect);

#endif // POKEHEARTGOLD_FIELD_FIELD_EFFECT_HELPERS_H
