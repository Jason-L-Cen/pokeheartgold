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
extern int ov00_021FFA60(int error);
extern void ov00_021F9A4C(void *connection, void *data, int size, int unused);
extern int ov00_021FFD9C(GPConnection *connection);
extern int ov00_021FFE20(GPConnection *connection, const char *token, const char *challenge, int flag, int unused, GPCallback callback, void *param);
extern void ov00_021FFF24(GPConnection *connection);
extern int ov00_02200020(GPConnection *connection, int profileId, int unk_02, int unk_03, GPCallback callback, void *param);
extern int ov00_022000FC(GPConnection *connection, int info, const char *value);

void ov00_021EF1A8(void *connection, void *data, int size, int unused) {
    ov00_021F9A4C(connection, data, size, unused);
}

// GT2 socket error.
void ov00_021EF1B4(void *socket) {
    int error = ov00_021FFA60(gt2GetSocketSOCKET(socket));
    ((int *)&ov00_0221A784)[1] = error;
    ov00_021EC24C(9, -97003);
    ov00_0221A784->socket = NULL;
}

void ov00_021EF1F0(LoginState *login, DwcUserData *userData, GPConnection *connection, int productId, u32 gameCode, u16 *playerName, DwcResultCallback callback, void *param) {
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

void ov00_021EF274(void) {
    ov00_021EF6F4(ov00_021EF640, NULL);
    ov00_0221A90C.state->state = 1;
    ov00_0221A90C.state->pending = 0;
}

void ov00_021EF2AC(void) {
    LoginState *login = ov00_0221A90C.state;
    if (!login) {
        return;
    }
    if (ov00_021EC230()) {
        return;
    }
    login = ov00_0221A90C.state;
    switch (login->state) {
    case 0:
    case 5:
        break;
    case 1:
        ov00_021EF8C0();
        break;
    case 2:
    case 3:
    case 4:
        if (login->connection && *login->connection) {
            ov00_021FFD9C(login->connection);
        }
        if (ov00_0221A90C.state->pending) {
            if (((OS_GetTick() - ov00_0221A90C.state->connectTick) << 6) / 33514 > 60000) {
                ov00_021EF3C0(6, -61070);
                ov00_0221A90C.state->pending = 0;
            }
        }
        break;
    }
}

DwcUserData *ov00_021EF3A4(void) {
    return ov00_0221A90C.state ? ov00_0221A90C.state->userData : NULL;
}

// Log-in failed.
void ov00_021EF3C0(int kind, int code) {
    if (!ov00_0221A90C.state || !kind) {
        return;
    }
    ov00_021EC24C(kind, code);
    if (ov00_0221A90C.state->callback) {
        ov00_0221A90C.state->callback(kind, 0, ov00_0221A90C.state->param);
    }
    ov00_021EF46C();
}

void ov00_021EF410(void) {
    if (ov00_0221A90C.state->auth) {
        ov00_021E7D48();
        ov00_021E7DD4();
        ov00_021EC2EC(0, ov00_0221A90C.state->auth, 0);
        ov00_0221A90C.state->auth = NULL;
    }
    ov00_0221A90C.state = NULL;
}

void ov00_021EF46C(void) {
    if (!ov00_0221A90C.state) {
        return;
    }
    ov00_0221A90C.state->state = 0;
    ov00_0221A90C.state->pending = 0;
}

// Reports a GameSpy presence error during the log-in.
int ov00_021EF494(int error) {
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
    ov00_021EF3C0(kind, code - 61000);
    return error;
}

// Reply to the connect request.
void ov00_021EF508(GPConnection *connection, void *arg, void *param) {
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
                error = ov00_021F04A0(1, ov00_02217A04, NULL);
                if (ov00_021EF494(error)) {
                    return;
                }
                ov00_0221A90C.state->callback(0, result->profileId, ov00_0221A90C.state->param);
                if (ov00_021EE584()) {
                    return;
                }
                if (ov00_021F179C(result->profileId)) {
                    return;
                }
            } else {
                ov00_021EF3C0(6, -60000);
            }
        } else if (ov00_0221A90C.state->state == 3) {
            int error = ov00_02200020(connection, result->profileId, 0, 0, ov00_021EFAC8, NULL);
            if (ov00_021EF494(error)) {
                return;
            }
        }
    } else {
        ov00_021EF494(result->result);
    }
}

void ov00_021EF640(const char *token, const char *challenge, void *param) {
    ov00_021EF658(token, challenge, ov00_021EF508, 2);
}

void ov00_021EF658(const char *token, const char *challenge, GPCallback callback, int state) {
    LoginState *login;
    strcpy(ov00_0221A90C.state->token, token);
    strcpy(ov00_0221A90C.state->challenge, challenge);
    login = ov00_0221A90C.state;
    login->connectTick = OS_GetTick();
    login->pending = 1;
    if (ov00_021EF494(ov00_021FFE20(ov00_0221A90C.state->connection, ov00_0221A90C.state->token, ov00_0221A90C.state->challenge, 1, 0, callback, NULL)) == 0) {
        ov00_0221A90C.state->state = state;
    }
}

// Starts the authentication; callback gets the token and challenge.
void ov00_021EF6F4(void (*callback)(const char *token, const char *challenge, void *param), void *param) {
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
    MI_CpuCopy8(ov00_0221A90C.state->playerName, params.ingamesn, ov00_021ED704(ov00_0221A90C.state->playerName) * 2 + 2);
    strcpy(params.gsbrcd, ov00_0221A90C.state->gsbrcd);
    params.alloc = (NasAllocFunc)ov00_021EC2A8;
    params.free = (NasFreeFunc)ov00_021EC2EC;
    auth = ov00_021EC2B8(0, sizeof(NasHttp), 4);
    ov00_0221A90C.state->auth = auth;
    ov00_0221A90C.state->authTick = OS_GetTick();
    ov00_021E7B70(&params, auth);
}

// Polls the authentication.
void ov00_021EF8C0(void) {
    AuthResult success, failure;
    AuthParams params;
    if (ov00_021E7E5C() == 21) {
        ov00_021E7EA8(&success);
        strcpy(ov00_0221A90C.state->token, success.token);
        strcpy(ov00_0221A90C.state->challenge, success.challenge);
        ov00_021E7DD4();
        ov00_021EC2EC(0, ov00_0221A90C.state->auth, 0);
        ov00_0221A90C.state->auth = NULL;
        if (DWCi_Acc_IsAuthentic(ov00_0221A90C.state->userData)) {
            ov00_0221A90C.authCallback(ov00_0221A90C.state->token, ov00_0221A90C.state->challenge, ov00_0221A90C.authParam);
            return;
        }
        ov00_021EF658(ov00_0221A90C.state->token, ov00_0221A90C.state->challenge, ov00_021EF508, 3);
        return;
    }
    if (ov00_021E7E5C() == 0) {
        return;
    }
    if (((OS_GetTick() - ov00_0221A90C.state->authTick) << 6) / 33514 > 10000) {
        ov00_021E7EA8(&failure);
        ov00_021E7DD4();
        ov00_021EC2EC(0, ov00_0221A90C.state->auth, 0);
        ov00_0221A90C.state->auth = NULL;
        ov00_021EF3C0(2, failure.error);
        return;
    }
    // retry
    ov00_021E7DD4();
    MI_CpuFill8(&params, 0, sizeof(params));
    MI_CpuCopy8(ov00_0221A90C.state->playerName, params.ingamesn, ov00_021ED704(ov00_0221A90C.state->playerName) * 2 + 2);
    strcpy(params.gsbrcd, ov00_0221A90C.state->gsbrcd);
    params.alloc = (NasAllocFunc)ov00_021EC2A8;
    params.free = (NasFreeFunc)ov00_021EC2EC;
    ov00_021E7B70(&params, ov00_0221A90C.state->auth);
}

// Profile information of the logged-in profile: checks its user name against the login id.
void ov00_021EFAC8(GPConnection *connection, void *arg, void *param) {
    FriendProfileInfo *info = arg;
    int error;
    char tempUserName[21], userName[21], unused[21];
    if (info->result) {
        return;
    }
    if (ov00_0221A90C.state->state == 3) {
        if (info->userName[0] == 0) {
            DWCi_Acc_LoginIdToUserName(&ov00_0221A90C.state->userData->tempLoginId, ov00_0221A90C.state->gameCode, tempUserName);
            error = ov00_022000FC(connection, 0x705, tempUserName);
            if (ov00_021EF494(error)) {
                return;
            }
            ov00_0221A90C.state->state = 4;
            error = ov00_02200020(connection, info->profileId, 0, 0, ov00_021EFAC8, NULL);
            if (ov00_021EF494(error)) {
                return;
            }
        } else {
            ov00_021FFF24(connection);
            ov00_021EF6F4(ov00_021EF640, NULL);
            ov00_0221A90C.state->state = 1;
        }
    } else if (ov00_0221A90C.state->state == 4) {
        DWCi_Acc_LoginIdToUserName(&ov00_0221A90C.state->userData->tempLoginId, ov00_0221A90C.state->gameCode, userName);
        if (strcmp(info->userName, userName) == 0) {
            DWCi_Acc_LoginIdToUserName(&ov00_0221A90C.state->loginId, ov00_0221A90C.state->gameCode, unused);
            DWCi_Acc_SetLoginIdToUserData(ov00_0221A90C.state->userData, &ov00_0221A90C.state->loginId, info->profileId);
            ov00_0221A90C.state->state = 2;
            ov00_021EF508(connection, &ov00_0221A90C.state->response, NULL);
        } else {
            error = ov00_02200020(connection, info->profileId, 0, 0, ov00_021EFAC8, NULL);
            if (ov00_021EF494(error)) {
                return;
            }
        }
    }
}
