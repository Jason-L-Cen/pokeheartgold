#ifndef POKEHEARTGOLD_PPW_LOBBY_STATE_H
#define POKEHEARTGOLD_PPW_LOBBY_STATE_H

// The state of the lobby library (gPPWLobbyWork->state), returned by PPW_LobbyProcess
enum PPWLobbyState {
    PPW_LOBBY_STATE_NOT_INITIALIZED,
    PPW_LOBBY_STATE_CONNECTING,
    PPW_LOBBY_STATE_CONNECTED,
    PPW_LOBBY_STATE_WAIT_SCHEDULE,
    PPW_LOBBY_STATE_ENUM_CHANNELS,
    PPW_LOBBY_STATE_SEARCHING_CHANNEL,
    PPW_LOBBY_STATE_ENTER_MAIN_CHANNEL,
    PPW_LOBBY_STATE_WAIT_MAIN_CHANNEL,
    PPW_LOBBY_STATE_READY,
    PPW_LOBBY_STATE_CLOSED,
    PPW_LOBBY_STATE_ERROR,
};

// PPW_LobbyGetSubChannelState
enum PPWLobbyChannelState {
    PPW_LOBBY_CHANNEL_STATE_NONE,
    PPW_LOBBY_CHANNEL_STATE_JOINING,
    PPW_LOBBY_CHANNEL_STATE_JOINED,
    PPW_LOBBY_CHANNEL_STATE_ERROR,
};

// Results of the PPW_Lobby* calls and of the lobby chat commands they pass through
enum PPWLobbyResult {
    PPW_LOBBY_RESULT_SUCCESS,
    PPW_LOBBY_RESULT_ERROR_ALLOC,
    PPW_LOBBY_RESULT_ERROR_SESSION,
    PPW_LOBBY_RESULT_ERROR_PARAM,
    PPW_LOBBY_RESULT_ERROR_STATE,
    PPW_LOBBY_RESULT_ERROR_CHANNEL,
    PPW_LOBBY_RESULT_ERROR_NODATA,
    PPW_LOBBY_RESULT_ERROR_CONDITION, // the library is in PPW_LOBBY_STATE_ERROR
};

// PPW_LobbyGetLastError
enum PPWLobbyError {
    PPW_LOBBY_ERROR_NONE,
    PPW_LOBBY_ERROR_UNKNOWN,
    PPW_LOBBY_ERROR_ALLOC,
    PPW_LOBBY_ERROR_SESSION,
    PPW_LOBBY_ERROR_SCHEDULE,      // the schedule answer was bad
    PPW_LOBBY_ERROR_VIP,           // the VIP list answer was bad
    PPW_LOBBY_ERROR_CHECK_PROFILE, // the profile check answer was bad
    PPW_LOBBY_ERROR_QUESTIONNAIRE, // the questionnaire answer was bad
};

#ifdef __cplusplus
// The last error (PPW_LobbyGetLastError) a failed lobby chat call leaves behind; PPW_LOBBY_ERROR_NONE when it is not fatal
class PPWLobbyErrorMap {
public:
    int Map(int result) const;
};

inline int PPWLobbyErrorMap::Map(int result) const {
    int error = PPW_LOBBY_ERROR_NONE;
    switch (result) {
    case PPW_LOBBY_RESULT_ERROR_ALLOC:
        error = PPW_LOBBY_ERROR_ALLOC;
        break;
    case PPW_LOBBY_RESULT_ERROR_SESSION:
        error = PPW_LOBBY_ERROR_SESSION;
        break;
    }
    return error;
}
#endif

#endif // POKEHEARTGOLD_PPW_LOBBY_STATE_H
