#include "global.h"

#include <nitro/code32.h>
#include <stdarg.h>

#include "ppw_lobby_internal.h"
#include "ppw_lobby_state.h"

int PPW_LobbySendChannelBinaryMessage(int channel, int type, const void *data, int size);
int PPW_LobbySendPlayerBinaryMessage(int userId, int type, const void *data, int size);
int PPW_LobbyGetMyUserId(void);

void PPWLobby_GetChannelName(Str *out, PPWLobbyChannel *channels, int subChannel) {
    volatile Str copy;
    if (!subChannel) {
        PPWLobbyChannel_GetName(out, &channels[0]);
        copy = *out;
        return;
    }
    PPWLobbyChannel_GetName(out, &channels[1]);
    copy = *out;
}

int PPW_LobbySendChannelBinaryMessage(int channel, int type, const void *data, int size) {
    PPWLobbyWork *work = gPPWLobbyWork;
    if (!work) {
        return PPW_LOBBY_RESULT_ERROR_STATE;
    }
    if (work) {
        if (work->state == PPW_LOBBY_STATE_ERROR) {
            return PPW_LOBBY_RESULT_ERROR_CONDITION;
        }
    }
    if (work) {
        if (PPWLobby_GetState() != PPW_LOBBY_STATE_READY) {
            return PPW_LOBBY_RESULT_ERROR_STATE;
        }
    }
    return PPWLobby_SendMessage(-1, channel, 1, 1, type, data, size);
}

int PPW_LobbySendPlayerBinaryMessage(int userId, int type, const void *data, int size) {
    PPWLobbyWork *work = gPPWLobbyWork;
    if (!work) {
        return PPW_LOBBY_RESULT_ERROR_STATE;
    }
    if (work) {
        if (work->state == PPW_LOBBY_STATE_ERROR) {
            return PPW_LOBBY_RESULT_ERROR_CONDITION;
        }
    }
    if (work) {
        if (PPWLobby_GetState() != PPW_LOBBY_STATE_READY) {
            return PPW_LOBBY_RESULT_ERROR_STATE;
        }
    }
    return PPWLobby_SendMessage(userId, 5, 1, 1, type, data, size);
}

int PPW_LobbyGetMyUserId(void) {
    PPWLobbyWork *work = gPPWLobbyWork;
    if (work) {
        return work->userId;
    }
    return -1;
}
