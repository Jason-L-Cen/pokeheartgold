#ifndef POKEHEARTGOLD_FIELD_WARP_ARROW_H
#define POKEHEARTGOLD_FIELD_WARP_ARROW_H

#include "field/field_effect_manager.h"

#include "player_avatar.h"

typedef struct WarpArrow WarpArrow;

void WarpArrow_Spawn(PlayerAvatar *avatar);

WarpArrow *WarpArrow_New(FieldEffectManager *manager);
void WarpArrow_Free(WarpArrow *arrow);

#endif // POKEHEARTGOLD_FIELD_WARP_ARROW_H
