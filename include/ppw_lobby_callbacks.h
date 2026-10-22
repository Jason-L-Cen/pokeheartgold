#ifndef POKEHEARTGOLD_PPW_LOBBY_CALLBACKS_H
#define POKEHEARTGOLD_PPW_LOBBY_CALLBACKS_H

#include "lobby_chat_callbacks.h"

#ifdef __cplusplus
extern "C" {
#endif

void PPWLobby_OnPlayerJoined(const char *channel, int userId, void *param);
void PPWLobby_OnPlayerLeft(const char *channel, int userId, void *param);
void PPWLobby_OnMessage(const char *channel, int userId, void *message, void *param);
void PPWLobby_OnChannelDataChanged(const char *channel, int userId, void *key, void *value, void *param);
void PPWLobby_OnTopicChanged(const char *channel, void *topic, void *param);
void PPWLobby_OnPlayerMessage(int userId, void *message, void *param);
void PPWLobby_OnExcessFlood(int seconds, void *param);
void PPWLobby_OnEnterMainChannel(int success, int error, const char *channel, u32 param);
void PPWLobby_OnEnterSubChannel(int success, int error, const char *channel, u32 param);
void PPWLobby_OnEnumChannels(int success, u32 count, const char *const *channels, u32 topics, const int *users, u32 param);
void PPWLobby_OnEnumUsers(int success, const char *channel, u32 count, int *ids, u32 param);
void PPWLobby_OnGetChannelKeys(int success, const char *channel, int userId, const char *key, const char *value, u32 param);
void PPWLobby_OnGetChannelMode(int success, const char *channel, u32 mode, u32 param);
void PPWLobby_OnGetChannelTime(int success, const char *channel, int userId, const char *key, const char *value, u32 param);
void PPWLobby_OnGetLobbyData(int success, const char *channel, int userId, const char *key, const char *value, u32 param);

#ifdef __cplusplus
}
#endif

#endif
