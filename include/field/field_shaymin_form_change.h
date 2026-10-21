#ifndef POKEHEARTGOLD_FIELD_FIELD_SHAYMIN_FORM_CHANGE_H
#define POKEHEARTGOLD_FIELD_FIELD_SHAYMIN_FORM_CHANGE_H

#include "field/field_effect_manager.h"

typedef struct ShayminFormChange ShayminFormChange;

FieldEffect *ShayminFormChange_Spawn(FieldEffectManager *manager, VecFx32 *position);
BOOL ShayminFormChange_IsDone(FieldEffect *effect);

ShayminFormChange *ShayminFormChange_New(FieldEffectManager *manager);
void ShayminFormChange_Free(ShayminFormChange *formChange);

#endif // POKEHEARTGOLD_FIELD_FIELD_SHAYMIN_FORM_CHANGE_H
