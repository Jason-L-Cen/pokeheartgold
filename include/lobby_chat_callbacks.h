#ifndef POKEHEARTGOLD_LOBBY_CHAT_CALLBACKS_H
#define POKEHEARTGOLD_LOBBY_CHAT_CALLBACKS_H

#include <nitro/types.h>

#ifdef __cplusplus
extern "C" {
#endif

void LobbyChat_OnRaw(void *chat, char *line, void *param);
void LobbyChat_OnDisconnected(void *chat, char *reason, void *param);
void LobbyChat_OnPrivateMessage(void *chat, char *nick, char *message, int type, void *param);
void LobbyChat_OnInvited(void *chat, char *channel, char *nick, void *param);
void LobbyChat_OnConnected(void *chat, int success, int failureReason, void *param);
void LobbyChat_OnNickError(void *chat, int type, char *nick, int numSuggested, char **suggested, void *param);
void LobbyChat_OnFillInUser(void *chat, u32 ip, char *user, void *param);
void LobbyChat_OnChannelMessage(void *chat, char *channel, char *nick, void *message, int type, void *param);
void LobbyChat_OnKicked(void *chat, char *channel, char *kicker, char *reason, void *param);
void LobbyChat_OnUserJoined(void *chat, const char *channel, const char *nick, int mode, void *param);
void LobbyChat_OnUserParted(void *chat, char *channel, char *nick, int cause, char *reason, char *kicker, void *param);
void LobbyChat_OnUserChangedNick(void *chat, void *channel, char *oldNick, char *newNick, void *param);
void LobbyChat_OnTopicChanged(void *chat, char *channel, void *topic, void *param);
void LobbyChat_OnChannelModeChanged(void *chat, const char *channel, u32 mode, u32 param);
void LobbyChat_OnUserModeChanged(void *chat, char *channel, char *nick, int mode, void *param);
void LobbyChat_OnUserListUpdated(void *chat, const char *channel, void *param);
void LobbyChat_OnBroadcastKeyChanged(void *chat, char *channel, char *nick, void *key, void *value, void *param);
void LobbyChat_OnEnterChannel(void *chat, int success, int error, const char *channel, u32 id);
void LobbyChat_OnEnumChannelsEach(void *chat, int result, u32 index, const char *channel, const char *topic, int users, u32 id);
void LobbyChat_OnEnumChannels(void *chat, int result, u32 count, u32 channels, u32 topics, u32 userCounts, u32 id);
void LobbyChat_OnEnumUsers(void *chat, int result, const char *channel, int count, const char **nicks, u32 modes, u32 id);
void LobbyChat_OnGetChannelKeys(void *chat, int result, const char *channel, const char *user, int count, u32 *keys, u32 *values, u32 id);
void LobbyChat_OnGetChannelMode(void *chat, int result, const char *channel, u32 mode, u32 id);

#ifdef __cplusplus
}
#endif

#endif
