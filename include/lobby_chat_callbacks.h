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

#ifdef __cplusplus
}
#endif

#endif
