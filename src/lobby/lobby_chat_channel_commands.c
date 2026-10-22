#include "global.h"

#include "dwc/net_common_internal.h"

#include <nitro/code32.h>

#include "lobby_chat_internal.h"

u32 LobbyRequest_GetId(LobbyRequest *request) {
    return request->id;
}

int LobbyChat_LeaveChannel(const char *channel) {
    Str channelName;
    Str fullName;
    Str removedName;
    Str removedFullName;
    LobbyChatManager *manager = gLobbyChatManager;
    CTX_CHECK(manager);
    LobbyString_InitFromCStr(&channelName, channel);
    LobbyChat_GetFullChannelName(&fullName, gLobbyChatManager, &channelName);
    chatLeaveChannelA(gLobbyChatManager->connection, StrData(&fullName), 0);
    LobbyString_Destroy(&fullName);
    LobbyString_Destroy(&channelName);
    LobbyString_InitFromCStr(&removedName, channel);
    LobbyChat_GetFullChannelName(&removedFullName, gLobbyChatManager, &removedName);
    LobbyChat_RemoveChannel(gLobbyChatManager, &removedFullName);
    LobbyString_Destroy(&removedFullName);
    LobbyString_Destroy(&removedName);
    return PPW_LOBBY_RESULT_SUCCESS;
}

int LobbyChat_SendChannelMessage(const char *channel, const char *message) {
    Str channelName;
    Str fullName;
    LobbyChatManager *manager = gLobbyChatManager;
    CTX_CHECK(manager);
    if (strlen(message) + 1 > 400) {
        return PPW_LOBBY_RESULT_ERROR_PARAM;
    }
    LobbyString_InitFromCStr(&channelName, channel);
    LobbyChat_GetFullChannelName(&fullName, gLobbyChatManager, &channelName);
    chatSendChannelMessageA(gLobbyChatManager->connection, StrData(&fullName), message, 3);
    LobbyString_Destroy(&fullName);
    LobbyString_Destroy(&channelName);
    return PPW_LOBBY_RESULT_SUCCESS;
}

int LobbyChat_SendUserMessage(int userId, const char *message) {
    Str nick;
    LobbyChatManager *manager = gLobbyChatManager;
    CTX_CHECK(manager);
    if (strlen(message) + 1 > 400) {
        return PPW_LOBBY_RESULT_ERROR_PARAM;
    }
    LobbyChat_GetUserNick(&nick, manager, userId);
    if (StrEqC(&nick, gLobbyStr_Empty_17)) {
        LobbyString_Destroy(&nick);
        return PPW_LOBBY_RESULT_ERROR_PARAM;
    }
    chatSendUserMessageA(gLobbyChatManager->connection, StrData(&nick), message, 3);
    LobbyString_Destroy(&nick);
    return PPW_LOBBY_RESULT_SUCCESS;
}

// userId -1 sets a channel key instead of a key of one user in the channel.
int LobbyChat_SetChannelKey(const char *channel, int userId, const char *key, const char *value) {
    Str nick;
    Str userNick;
    Str channelName;
    Str fullName;
    LobbyChatManager *manager = gLobbyChatManager;
    CTX_CHECK(manager);
    if (strlen(value) + 1 > 400 || strlen(key) + 1 > 20) {
        return PPW_LOBBY_RESULT_ERROR_PARAM;
    }
    LobbyString_InitFromCStr(&nick, gLobbyStr_Empty_17);
    if (userId == -1) {
        LobbyString_AssignCStr(&nick, gLobbyStr_Empty_17);
    } else {
        LobbyChat_GetUserNick(&userNick, gLobbyChatManager, userId);
        LobbyString_Assign(&nick, &userNick);
        LobbyString_Destroy(&userNick);
        if (StrEqC(&nick, gLobbyStr_Empty_17)) {
            LobbyString_Destroy(&nick);
            return PPW_LOBBY_RESULT_ERROR_PARAM;
        }
    }
    LobbyString_InitFromCStr(&channelName, channel);
    LobbyChat_GetFullChannelName(&fullName, gLobbyChatManager, &channelName);
    chatSetChannelKeysA(gLobbyChatManager->connection, StrData(&fullName), StrData(&nick), 1, &key, &value);
    LobbyString_Destroy(&fullName);
    LobbyString_Destroy(&channelName);
    LobbyString_Destroy(&nick);
    return PPW_LOBBY_RESULT_SUCCESS;
}
