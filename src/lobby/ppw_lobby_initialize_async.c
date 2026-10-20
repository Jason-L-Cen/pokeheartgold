#include "global.h"

#include "dwc/dwc_account_internal.h"

#include "ppw_lobby_state.h"

#include <nitro/code32.h>

#include "ppw_lobby_work.h"

int PPW_LobbyInitializeAsync(const char *gameName, const char *secretKey, int prefix, const PPWLobbyCallbacks *callbacks, const DwcUserData *userData, const void *playerData, u32 playerDataSize);

int PPW_LobbyInitializeAsync(const char *gameName, const char *secretKey, int prefix, const PPWLobbyCallbacks *callbacks, const DwcUserData *userData, const void *playerData, u32 playerDataSize) {
    if (!DWC_CheckUserData(userData) || !DWC_CheckHasProfile(userData)) {
        return PPW_LOBBY_RESULT_ERROR_PARAM;
    }
    return PPWLobby_Initialize(gameName, secretKey, prefix, callbacks, userData->gsProfileId, playerData, playerDataSize, TRUE);
}
