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

#ifdef __cplusplus
}
#endif

#endif
