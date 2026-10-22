#ifndef POKEHEARTGOLD_MAP_OBJECT_MOVEMENT_H
#define POKEHEARTGOLD_MAP_OBJECT_MOVEMENT_H

#include "map_object.h"

void RandomLookMovement_InitAnyDirection(LocalMapObject *object);
void RandomLookMovement_InitUpLeft(LocalMapObject *object);
void RandomLookMovement_InitUpRight(LocalMapObject *object);
void RandomLookMovement_InitDownLeft(LocalMapObject *object);
void RandomLookMovement_InitDownRight(LocalMapObject *object);
void RandomLookMovement_InitUpDownLeft(LocalMapObject *object);
void RandomLookMovement_InitUpDownRight(LocalMapObject *object);
void RandomLookMovement_InitUpLeftRight(LocalMapObject *object);
void RandomLookMovement_InitDownLeftRight(LocalMapObject *object);
void RandomLookMovement_InitUpDown(LocalMapObject *object);
void RandomLookMovement_InitLeftRight(LocalMapObject *object);
void RandomLookMovement_Update(LocalMapObject *object);
void RandomLookMovement_Delete(LocalMapObject *object);
void RandomWalkMovement_InitAnyDirection(LocalMapObject *object);
void RandomWalkMovement_InitUpDown(LocalMapObject *object);
void RandomWalkMovement_InitLeftRight(LocalMapObject *object);
void RandomWalkMovement_Update(LocalMapObject *object);
void FaceDirectionMovement_Update(LocalMapObject *object);
void FaceDirectionMovement_InitUp(LocalMapObject *object);
void FaceDirectionMovement_InitDown(LocalMapObject *object);
void FaceDirectionMovement_InitLeft(LocalMapObject *object);
void FaceDirectionMovement_InitRight(LocalMapObject *object);
void SpinMovement_InitCounterclockwise(LocalMapObject *object);
void SpinMovement_InitClockwise(LocalMapObject *object);
void SpinMovement_Update(LocalMapObject *object);
void AlternatingSpinMovement_Init(LocalMapObject *object);
void AlternatingSpinMovement_Update(LocalMapObject *object);
void BackAndForthMovement_Init(LocalMapObject *object);
void BackAndForthMovement_Update(LocalMapObject *object);
void PathMovement_InitNorthEastWestSouth(LocalMapObject *object);
void PathMovement_InitEastWestSouthNorth(LocalMapObject *object);
void PathMovement_InitSouthNorthEastWest(LocalMapObject *object);
void PathMovement_InitWestSouthNorthEast(LocalMapObject *object);
void PathMovement_InitWestEastSouthNorth(LocalMapObject *object);
void PathMovement_InitWestEastSouthNorth2(LocalMapObject *object);
void PathMovement_InitSouthNorthWestEast(LocalMapObject *object);
void PathMovement_InitEastSouthNorthWest(LocalMapObject *object);
void PathMovement_InitWestNorthSouthEast(LocalMapObject *object);
void PathMovement_InitNorthSouthEastWest(LocalMapObject *object);
void PathMovement_InitEastWestNorthSouth(LocalMapObject *object);
void PathMovement_InitSouthEastWestNorth(LocalMapObject *object);
void PathMovement_InitEastNorthSouthWest(LocalMapObject *object);
void PathMovement_InitNorthSouthWestEast(LocalMapObject *object);
void PathMovement_InitWestEastNorthSouth(LocalMapObject *object);
void PathMovement_InitSouthWestEastNorth(LocalMapObject *object);
void PathMovement_Update(LocalMapObject *object);
void PathMovement_InitNorthWestSouthEast(LocalMapObject *object);
void PathMovement_InitSouthEastNorthWest(LocalMapObject *object);
void PathMovement_InitWestSouthEastNorth(LocalMapObject *object);
void PathMovement_InitEastNorthWestSouth(LocalMapObject *object);
void PathMovement_InitNorthEastSouthWest(LocalMapObject *object);
void PathMovement_InitSouthWestNorthEast(LocalMapObject *object);
void PathMovement_InitWestNorthEastSouth(LocalMapObject *object);
void PathMovement_InitEastSouthWestNorth(LocalMapObject *object);

#endif // POKEHEARTGOLD_MAP_OBJECT_MOVEMENT_H
