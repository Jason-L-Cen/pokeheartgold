#ifndef POKEHEARTGOLD_COMM_PLAYER_MANAGER_H
#define POKEHEARTGOLD_COMM_PLAYER_MANAGER_H

#include "script.h"

void CommPlayerManager_RemoveAllAvatars(void);
void CommPlayerManager_FieldUpdate(FieldSystem *fieldSystem, int value);
int CommPlayerManager_IsLocalPlayerConnected(void);
void CommPlayerManager_SetLocalFacing(int val);
void CommPlayerManager_ReceivePosition(int netId, int size, void *data, void *context);
u32 CommPlayerManager_GetPositionSize(void);
void CommPlayerManager_ReceiveHostPosition(int netId, int size, void *data, void *context);
void CommPlayerManager_ReceiveRemovePlayer(int netId, int size, void *data, void *context);
void CommPlayerManager_ReceiveMoveEnabled(int netId, int size, void *data, void *context);
void CommPlayerManager_RecvLockOnSpot(int netId, int size, void *data, void *context);
void CommPlayerManager_ForcePosition();

#endif // POKEHEARTGOLD_COMM_PLAYER_MANAGER_H
