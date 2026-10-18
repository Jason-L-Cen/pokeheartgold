#ifndef POKEHEARTGOLD_FIELD_EMOTE_MARK_H
#define POKEHEARTGOLD_FIELD_EMOTE_MARK_H

#include "player_avatar.h"
#include "field_effects.h"

FieldEffect *EmoteMark_Spawn(LocalMapObject *obj, int kind, int playSound);
BOOL EmoteMark_IsDone(FieldEffect *effect);
void EmoteMark_Hide(FieldEffect *effect);

#endif // POKEHEARTGOLD_FIELD_EMOTE_MARK_H
