#ifndef POKEHEARTGOLD_PLAYER_AVATAR_MOVEMENT_H
#define POKEHEARTGOLD_PLAYER_AVATAR_MOVEMENT_H

#include "player_avatar.h"

// PlayerAvatar_CheckCollision result bits
enum PlayerAvatarCollision {
    PLAYER_COLLISION_BLOCKED = 1 << 0,
    PLAYER_COLLISION_LINK_OCCUPIED = 1 << 1, // another link player stands there
    PLAYER_COLLISION_LEDGE = 1 << 2,
    PLAYER_COLLISION_WARP = 1 << 3, // blocked at a warp or door: no bump sound
    PLAYER_COLLISION_WATER = 1 << 5,
};

void PlayerAvatar_GetCoordsInFront(PlayerAvatar *playerAvatar, int *x, int *z);
void PlayerAvatar_UpdateMovement(PlayerAvatar *playerAvatar);
void PlayerAvatar_MoveControl(PlayerAvatar *playerAvatar, MapLoadManager *mapLoadManager, int direction, u16 keys, u16 heldKeys, int allowTurn);
int PlayerAvatar_CheckReadyToMove(PlayerAvatar *, int);
void PlayerAvatar_ResetMovement(PlayerAvatar *playerAvatar);
BOOL PlayerAvatar_IsBetweenSteps(PlayerAvatar *playerAvatar);
void PlayerAvatar_StopAndFace(PlayerAvatar *playerAvatar, int direction);
u32 PlayerAvatar_CheckCollision(PlayerAvatar *, LocalMapObject *, int);
int PlayerAvatar_GetInputDirection(PlayerAvatar *avatar, u16 newKeys, u16 heldKeys);
BOOL PlayerAvatar_IsReadyOrBumpingWall(PlayerAvatar *);
BOOL PlayerAvatar_IsRunning(PlayerAvatar *playerAvatar);
int PlayerAvatar_GetMoveStateForDirection(PlayerAvatar *, int);
u32 PlayerAvatar_GetMovementFromInput(PlayerAvatar *, int, int, u32, int, int);
BOOL PlayerAvatar_IsReadyForMovement(PlayerAvatar *);
void PlayerAvatar_SetMovement(PlayerAvatar *, int);
int PlayerAvatar_GetMetatileBehaviorInDirection(PlayerAvatar *, int);

#endif // POKEHEARTGOLD_PLAYER_AVATAR_MOVEMENT_H
