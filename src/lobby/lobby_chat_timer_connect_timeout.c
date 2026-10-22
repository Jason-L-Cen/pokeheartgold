#include "global.h"

#include <nitro/code32.h>

#include "lobby_chat_internal.h"

int LobbyChat_TimerConnectTimeout(void *param) {
    LobbyChatManager *manager = gLobbyChatManager;
    (void)param;
    manager->lastError = 3;
    manager->state = LOBBY_CHAT_STATE_ERROR;
    return 0;
}
