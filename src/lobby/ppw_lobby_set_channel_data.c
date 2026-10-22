#include "global.h"

#include <nitro/code32.h>
#include <stdarg.h>

#include "ppw_lobby_internal.h"
#include "ppw_lobby_state.h"

int PPW_LobbySetChannelData(int userId, int channel, const char *key, const void *data, u32 size);

int PPW_LobbySetChannelData(int userId, int channel, const char *key, const void *data, u32 size) {
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
    return PPWLobby_SetChannelData(userId, channel, key, data, size);
}
