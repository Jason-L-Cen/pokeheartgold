#pragma force_active on
#include <string.h>

#include "global.h"

#include "dwc/dwc_main_internal.h"
#include "dwc/net_common_internal.h"

// The code below is compiled as ARM (32-bit) code.
#include <nitro/code32.h>

#pragma warn_unusedvar off
#pragma warn_uninitializedvar off

// Log-in: authentication with the Nintendo server (unless the user data holds an authenticated login
// id), connecting to the GameSpy presence server with the token, and checking that the profile is the
// one stored in the user data (a temporary login id is replaced by the real one on the first log-in).

extern char ov00_02217A04[]; // ""

extern int gt2GetSocketSOCKET(void *socket);
extern int gsSocketGetLastError(int error);
extern void DWCi_TransportPingCallback(void *connection, void *data, int size, int unused);
extern int gpProcess(GPConnection *connection);
extern int gpConnectPreAuthenticatedA(GPConnection *connection, const char *token, const char *challenge, int flag, int unused, GPCallback callback, void *param);
extern void gpDisconnect(GPConnection *connection);
extern int gpGetInfo(GPConnection *connection, int profileId, int unk_02, int unk_03, GPCallback callback, void *param);
extern int gpSetInfosA(GPConnection *connection, int info, const char *value);

void DWCi_GT2PingCallback(void *connection, void *data, int size, int unused) {
    DWCi_TransportPingCallback(connection, data, size, unused);
}

// GT2 socket error.
void DWCi_GT2SocketErrorCallback(void *socket) {
    int error = gsSocketGetLastError(gt2GetSocketSOCKET(socket));
    ((int *)&ov00_0221A784)[1] = error;
    DWCi_SetError(9, -97003);
    ov00_0221A784->socket = NULL;
}

void DWCi_LoginInit(LoginState *login, DwcUserData *userData, GPConnection *connection, int productId, u32 gameCode, u16 *playerName, DwcResultCallback callback, void *param) {
    ov00_0221A90C.state = login;
    MI_CpuFill8(login, 0, sizeof(LoginState));
    ov00_0221A90C.state->connection = connection;
    ov00_0221A90C.state->state = 0;
    ov00_0221A90C.state->productId = productId;
    ov00_0221A90C.state->gameCode = gameCode;
    ov00_0221A90C.state->playerName = playerName;
    ov00_0221A90C.state->callback = callback;
    ov00_0221A90C.state->param = param;
    ov00_0221A90C.state->userData = userData;
}

void DWCi_LoginAsync(void) {
    DWCi_StartAuth(DWCi_LoginAuthCallback, NULL);
    ov00_0221A90C.state->state = 1;
    ov00_0221A90C.state->pending = 0;
}

void DWCi_LoginProcess(void) {
    LoginState *login = ov00_0221A90C.state;
    if (!login) {
        return;
    }
    if (DWCi_IsError()) {
        return;
    }
    login = ov00_0221A90C.state;
    switch (login->state) {
    case 0:
    case 5:
        break;
    case 1:
        DWCi_ProcessAuth();
        break;
    case 2:
    case 3:
    case 4:
        if (login->connection && *login->connection) {
            gpProcess(login->connection);
        }
        if (ov00_0221A90C.state->pending) {
            if (((OS_GetTick() - ov00_0221A90C.state->connectTick) << 6) / 33514 > 60000) {
                DWCi_StopLogin(6, -61070);
                ov00_0221A90C.state->pending = 0;
            }
        }
        break;
    }
}

DwcUserData *DWCi_GetUserData(void) {
    return ov00_0221A90C.state ? ov00_0221A90C.state->userData : NULL;
}

// Log-in failed.
void DWCi_StopLogin(int kind, int code) {
    if (!ov00_0221A90C.state || !kind) {
        return;
    }
    DWCi_SetError(kind, code);
    if (ov00_0221A90C.state->callback) {
        ov00_0221A90C.state->callback(kind, 0, ov00_0221A90C.state->param);
    }
    DWCi_ResetLogin();
}

void DWCi_ShutdownLogin(void) {
    if (ov00_0221A90C.state->auth) {
        DWCi_Auth_Abort();
        DWCi_Auth_Destroy();
        DWC_Free(0, ov00_0221A90C.state->auth, 0);
        ov00_0221A90C.state->auth = NULL;
    }
    ov00_0221A90C.state = NULL;
}

void DWCi_ResetLogin(void) {
    if (!ov00_0221A90C.state) {
        return;
    }
    ov00_0221A90C.state->state = 0;
    ov00_0221A90C.state->pending = 0;
}

// Reports a GameSpy presence error during the log-in.
int DWCi_LoginHandleGPError(int error) {
    int kind, code;
    if (!error) {
        return 0;
    }
    switch (error) {
    case 1:
        kind = 9;
        code = -1;
        break;
    case 2:
        kind = 9;
        code = -2;
        break;
    case 3:
        kind = 6;
        code = -10;
        break;
    case 4:
        kind = 6;
        code = -20;
        break;
    }
    DWCi_StopLogin(kind, code - 61000);
    return error;
}

// Reply to the connect request.
void DWCi_GPConnectCallback(GPConnection *connection, void *arg, void *param) {
    LoginConnectResult *result = arg;
    ov00_0221A90C.state->pending = 0;
    if (result->result == 0) {
        {
            LoginConnectResult *source = result;
            LoginConnectResult *dest = &ov00_0221A90C.state->response;
            *dest = *source;
        }
        if (ov00_0221A90C.state->state == 2) {
            if (ov00_0221A90C.state->userData->gsProfileId == result->profileId) {
                int error;
                ov00_0221A90C.state->state = 5;
                error = DWCi_SetGPStatus(1, ov00_02217A04, NULL);
                if (DWCi_LoginHandleGPError(error)) {
                    return;
                }
                ov00_0221A90C.state->callback(0, result->profileId, ov00_0221A90C.state->param);
                if (DWCi_CreateGT2Socket()) {
                    return;
                }
                if (DWCi_QR2Startup(result->profileId)) {
                    return;
                }
            } else {
                DWCi_StopLogin(6, -60000);
            }
        } else if (ov00_0221A90C.state->state == 3) {
            int error = gpGetInfo(connection, result->profileId, 0, 0, DWCi_GPGetInfoCallback, NULL);
            if (DWCi_LoginHandleGPError(error)) {
                return;
            }
        }
    } else {
        DWCi_LoginHandleGPError(result->result);
    }
}

void DWCi_LoginAuthCallback(const char *token, const char *challenge, void *param) {
    DWCi_GPConnect(token, challenge, DWCi_GPConnectCallback, 2);
}

void DWCi_GPConnect(const char *token, const char *challenge, GPCallback callback, int state) {
    LoginState *login;
    strcpy(ov00_0221A90C.state->token, token);
    strcpy(ov00_0221A90C.state->challenge, challenge);
    login = ov00_0221A90C.state;
    login->connectTick = OS_GetTick();
    login->pending = 1;
    if (DWCi_LoginHandleGPError(gpConnectPreAuthenticatedA(ov00_0221A90C.state->connection, ov00_0221A90C.state->token, ov00_0221A90C.state->challenge, 1, 0, callback, NULL)) == 0) {
        ov00_0221A90C.state->state = state;
    }
}

// Starts the authentication; callback gets the token and challenge.
void DWCi_StartAuth(void (*callback)(const char *token, const char *challenge, void *param), void *param) {
    AuthParams params;
    NasHttp *auth;
    MI_CpuFill8(&params, 0, sizeof(params));
    ov00_0221A90C.authCallback = callback;
    ov00_0221A90C.authParam = param;
    if (DWCi_Acc_IsAuthentic(ov00_0221A90C.state->userData)) {
        DWCi_Acc_LoginIdToUserName(&ov00_0221A90C.state->userData->loginId, ov00_0221A90C.state->userData->gameCode, ov00_0221A90C.state->userName);
    } else {
        if (!DWCi_Acc_IsValidLoginId(&ov00_0221A90C.state->loginId)) {
            if (DWCi_Acc_CheckConsoleUserId(&ov00_0221A90C.state->userData->tempLoginId)) {
                ov00_0221A90C.state->loginId = ov00_0221A90C.state->userData->tempLoginId;
            } else {
                DWCi_Acc_CreateTempLoginId(&ov00_0221A90C.state->loginId);
            }
        } else {
            DWCi_Acc_SetPlayerId(&ov00_0221A90C.state->loginId, (u32)((OS_GetTick() * 0x5D588B656C078965 + 0x269EC3) >> 32));
        }
        DWCi_Acc_LoginIdToUserName(&ov00_0221A90C.state->loginId, ov00_0221A90C.state->gameCode, ov00_0221A90C.state->userName);
    }
    MI_CpuCopy8(ov00_0221A90C.state->playerName, params.ingamesn, DWCi_WStrLen(ov00_0221A90C.state->playerName) * 2 + 2);
    strcpy(params.gsbrcd, ov00_0221A90C.state->gsbrcd);
    params.alloc = (NasAllocFunc)DWC_Alloc;
    params.free = (NasFreeFunc)DWC_Free;
    auth = DWC_AllocEx(0, sizeof(NasHttp), 4);
    ov00_0221A90C.state->auth = auth;
    ov00_0221A90C.state->authTick = OS_GetTick();
    DWCi_Auth_Start(&params, auth);
}

// Polls the authentication.
void DWCi_ProcessAuth(void) {
    AuthResult success, failure;
    AuthParams params;
    if (DWCi_Auth_GetState() == 21) {
        DWCi_Auth_GetResult(&success);
        strcpy(ov00_0221A90C.state->token, success.token);
        strcpy(ov00_0221A90C.state->challenge, success.challenge);
        DWCi_Auth_Destroy();
        DWC_Free(0, ov00_0221A90C.state->auth, 0);
        ov00_0221A90C.state->auth = NULL;
        if (DWCi_Acc_IsAuthentic(ov00_0221A90C.state->userData)) {
            ov00_0221A90C.authCallback(ov00_0221A90C.state->token, ov00_0221A90C.state->challenge, ov00_0221A90C.authParam);
            return;
        }
        DWCi_GPConnect(ov00_0221A90C.state->token, ov00_0221A90C.state->challenge, DWCi_GPConnectCallback, 3);
        return;
    }
    if (DWCi_Auth_GetState() == 0) {
        return;
    }
    if (((OS_GetTick() - ov00_0221A90C.state->authTick) << 6) / 33514 > 10000) {
        DWCi_Auth_GetResult(&failure);
        DWCi_Auth_Destroy();
        DWC_Free(0, ov00_0221A90C.state->auth, 0);
        ov00_0221A90C.state->auth = NULL;
        DWCi_StopLogin(2, failure.error);
        return;
    }
    // retry
    DWCi_Auth_Destroy();
    MI_CpuFill8(&params, 0, sizeof(params));
    MI_CpuCopy8(ov00_0221A90C.state->playerName, params.ingamesn, DWCi_WStrLen(ov00_0221A90C.state->playerName) * 2 + 2);
    strcpy(params.gsbrcd, ov00_0221A90C.state->gsbrcd);
    params.alloc = (NasAllocFunc)DWC_Alloc;
    params.free = (NasFreeFunc)DWC_Free;
    DWCi_Auth_Start(&params, ov00_0221A90C.state->auth);
}

// Profile information of the logged-in profile: checks its user name against the login id.
void DWCi_GPGetInfoCallback(GPConnection *connection, void *arg, void *param) {
    FriendProfileInfo *info = arg;
    int error;
    char tempUserName[21], userName[21], unused[21];
    if (info->result) {
        return;
    }
    if (ov00_0221A90C.state->state == 3) {
        if (info->userName[0] == 0) {
            DWCi_Acc_LoginIdToUserName(&ov00_0221A90C.state->userData->tempLoginId, ov00_0221A90C.state->gameCode, tempUserName);
            error = gpSetInfosA(connection, 0x705, tempUserName);
            if (DWCi_LoginHandleGPError(error)) {
                return;
            }
            ov00_0221A90C.state->state = 4;
            error = gpGetInfo(connection, info->profileId, 0, 0, DWCi_GPGetInfoCallback, NULL);
            if (DWCi_LoginHandleGPError(error)) {
                return;
            }
        } else {
            gpDisconnect(connection);
            DWCi_StartAuth(DWCi_LoginAuthCallback, NULL);
            ov00_0221A90C.state->state = 1;
        }
    } else if (ov00_0221A90C.state->state == 4) {
        DWCi_Acc_LoginIdToUserName(&ov00_0221A90C.state->userData->tempLoginId, ov00_0221A90C.state->gameCode, userName);
        if (strcmp(info->userName, userName) == 0) {
            DWCi_Acc_LoginIdToUserName(&ov00_0221A90C.state->loginId, ov00_0221A90C.state->gameCode, unused);
            DWCi_Acc_SetLoginIdToUserData(ov00_0221A90C.state->userData, &ov00_0221A90C.state->loginId, info->profileId);
            ov00_0221A90C.state->state = 2;
            DWCi_GPConnectCallback(connection, &ov00_0221A90C.state->response, NULL);
        } else {
            error = gpGetInfo(connection, info->profileId, 0, 0, DWCi_GPGetInfoCallback, NULL);
            if (DWCi_LoginHandleGPError(error)) {
                return;
            }
        }
    }
}
