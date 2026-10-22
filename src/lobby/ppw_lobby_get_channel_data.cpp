#include <nitro/types.h>

#include "ppw_lobby_state.h"

#include <nitro/code32.h>

#include "lobby_chat_request.h"
#include "ppw_lobby_work.h"

extern "C" char gLobbyStr_Empty_15[];
extern "C" int LobbyChat_GetChannelKeysAsync(const char *, int, const char *, LobbyGetChannelKeysCallback, u32);
extern "C" int PPW_LobbyGetChannelDataAsync(int, int, const char *, u32);

inline bool IsEmptyChannelName(const Str &name) {
    return LobbyString_CompareCStr(&name, gLobbyStr_Empty_15) == 0;
}

#pragma opt_common_subs off

extern "C" int PPW_LobbyGetChannelDataAsync(int userId, int channel, const char *key, u32 param) {
    PPWLobbyWork *w = gPPWLobbyWork;
    if (w == 0) {
        return PPW_LOBBY_RESULT_ERROR_STATE;
    }
    if (w != 0 && w->state == PPW_LOBBY_STATE_ERROR) {
        return PPW_LOBBY_RESULT_ERROR_CONDITION;
    }
    if (w != 0 && gPPWLobbyWork->state != PPW_LOBBY_STATE_READY) {
        return PPW_LOBBY_RESULT_ERROR_STATE;
    }
    {
        Str name;
        PPWLobby_GetChannelName(&name, &w->mainChannel, channel);
        if (IsEmptyChannelName(name)) {
            LobbyString_Destroy(&name);
            return PPW_LOBBY_RESULT_ERROR_CHANNEL;
        }
        int result = LobbyChat_GetChannelKeysAsync(name.c_str(), userId, key, PPWLobby_OnGetChannelKeys, param);
        {
            PPWLobbyWork *g = gPPWLobbyWork;
            int error = PPWLobbyErrorMap().Map(result);
            if (error != PPW_LOBBY_ERROR_NONE) {
                g->error = error;
                g->state = PPW_LOBBY_STATE_ERROR;
            }
        }
        LobbyString_Destroy(&name);
        return result;
    }
}

#pragma opt_common_subs reset
