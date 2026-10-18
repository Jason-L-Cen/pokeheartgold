#ifndef POKEHEARTGOLD_MAP_OBJECT_BEHAVIOR_H
#define POKEHEARTGOLD_MAP_OBJECT_BEHAVIOR_H

#include "script.h"

BOOL TryGetSeenByNpcTrainers(FieldSystem *fieldSystem, BOOL doublesEligible);
SysTask *TrainerApproach_Start(FieldSystem *fieldSystem, LocalMapObject *localMapObject, PlayerAvatar *playerAvatar, int movement, int remaining, int unk10, int unk14, int trainerNum);
BOOL TrainerApproach_IsDone(SysTask *task);
void TrainerApproach_Delete(SysTask *task);

#endif // POKEHEARTGOLD_MAP_OBJECT_BEHAVIOR_H
