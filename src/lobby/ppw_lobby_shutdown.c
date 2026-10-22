#include "global.h"

#include "ppw_lobby_state.h"

#include <nitro/code32.h>

#include "ppw_lobby_work.h"

extern int LobbyChat_Shutdown(void);
int PPW_LobbyShutdownAsync(void);

int PPW_LobbyShutdownAsync(void) {
    int result, error;
    volatile u8 unused;
    PPWLobbyWork *work;
    if (!gPPWLobbyWork) {
        return PPW_LOBBY_RESULT_SUCCESS;
    }
    if (gPPWLobbyWork->state == PPW_LOBBY_STATE_CLOSED) {
        return PPW_LOBBY_RESULT_SUCCESS;
    }
    LobbyTimerMap_Clear(&gPPWLobbyWork->timers);
    gPPWLobbyWork->state = PPW_LOBBY_STATE_CLOSED;
    result = LobbyChat_Shutdown();
    work = gPPWLobbyWork;
    unused = 0;
    if (result != PPW_LOBBY_RESULT_ERROR_ALLOC) {
        if (result == PPW_LOBBY_RESULT_ERROR_SESSION) {
            error = PPW_LOBBY_ERROR_SESSION;
        } else {
            error = PPW_LOBBY_ERROR_NONE;
        }
    } else {
        error = PPW_LOBBY_ERROR_ALLOC;
    }
    if (error) {
        work->error = error;
        work->state = PPW_LOBBY_STATE_ERROR;
    }
    return result;
}
