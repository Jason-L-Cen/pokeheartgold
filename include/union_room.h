#ifndef POKEHEARTGOLD_UNION_ROOM_H
#define POKEHEARTGOLD_UNION_ROOM_H

#include "script.h"

void Colosseum_ShowOpponentTrainerCard(FieldSystem *fieldSystem);
struct UnionRoom *UnionRoom_Start(FieldSystem *fieldSystem);
void UnionRoom_Stop(FieldSystem *fieldSystem);
int UnionRoom_GetPlayerActivityType(struct UnionRoom *fieldSystem_unk80, int objId);
int UnionRoom_RequestJoinPlayer(struct UnionRoom *fieldSystem_unk80, int objId, u16 kind);
int UnionRoom_GetJoinStatus(struct UnionRoom *fieldSystem_unk80);
u32 UnionRoom_GetTalkChoice(struct UnionRoom *fieldSystem_unk80);
void UnionRoom_SendTalkResponse(struct UnionRoom *fieldSystem_unk80, int mode, int value);
int UnionRoom_GetTalkReply(struct UnionRoom *fieldSystem_unk80);
int UnionRoom_TrySendTalkCancel(struct UnionRoom *fieldSystem_unk80, int ret);
void UnionRoom_RecvUnusedProfile(int netId, int size, void *data, void *context);
void UnionRoom_RecvTalkReply(int netId, int size, void *data, void *context);
void UnionRoom_RecvUnused(int netId, int size, void *data, void *context);
void UnionRoom_RecvRestartSearch(int netId, int size, void *data, void *context);
void Colosseum_RecvMixBattleMons(int netId, int size, void *data, void *context);
u32 Colosseum_GetMixBattleMonsSize(void);
void *Colosseum_GetMixBattleMonsBuffer(int netId, void *context, int size);
void Colosseum_RecvMixBattleChoice(int netId, int size, void *data, void *context);

#endif // POKEHEARTGOLD_UNION_ROOM_H
