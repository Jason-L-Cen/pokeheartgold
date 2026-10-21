#ifndef POKEHEARTGOLD_FIELD_UNION_CHAT_MARKER_H
#define POKEHEARTGOLD_FIELD_UNION_CHAT_MARKER_H

#include "field/field_effect_manager.h"

#include "map_object.h"

// The marker over a Union Room player whose chat log entry is touched.

typedef struct UnionChatMarker UnionChatMarker;

UnionChatMarker *UnionChatMarker_New(FieldEffectManager *manager);
void UnionChatMarker_Free(UnionChatMarker *marker);
FieldEffect *UnionChatMarker_Spawn(LocalMapObject *mapObject);

#endif // POKEHEARTGOLD_FIELD_UNION_CHAT_MARKER_H
