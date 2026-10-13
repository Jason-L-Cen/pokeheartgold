#pragma force_active on
#include <string.h>

#include "global.h"

#include "dwc/dwc_main_internal.h"
#include "dwc/net_common_internal.h"

// The code below is compiled as ARM (32-bit) code.
#include <nitro/code32.h>

#pragma warn_unusedvar off
#pragma warn_uninitializedvar off

// Main control of the network library: setting up and shutting down the control block, the per-frame
// processing, the log-in / friend list update / matchmaking entry points, the table of GT2 connections
// by aid, and the GT2 / GameSpy callbacks that drive the control state.

// Some u8 fields are read and written through volatile accesses here.
#define VOLATILE_U8(field) (*(volatile u8 *)&(field))

extern char ov00_022179EC[]; // "SCM"
extern char ov00_022179F0[]; // "SCN"
extern char ov00_022179F4[]; // ""
extern char ov00_022179F8[]; // "GPCM"
extern char ov00_02217A00[]; // "MAT"

extern char *strchr(const char *s, int c);
extern unsigned long strtoul(const char *s, char **end, int base);
extern void CloseStatsConnection(void);
extern int gt2CreateSocket(void **socket, u32 addr, u32 sendSize, u32 recvSize, void (*errorCallback)(void *socket));
extern void gt2CloseSocket(void *socket);
extern void gt2Listen(void *socket, void *callback);
extern void gt2CloseConnectionHard(void *connection);
extern void gt2CloseAllConnectionsHard(void *socket);
extern void gt2SetUnrecognizedMessageCallback(void *socket, void *callback);
extern DwcConnectionInfo *gt2GetConnectionData(void *connection);
extern u32 gt2AddressToString(u32 addr, u16 port, int flag);
extern void qr2_send_statechanged(void *reporting);
extern void qr2_shutdown(void *reporting);
extern void ServerBrowserFree(void *list);
extern void GSIStartAvailableCheck(const char *gameName);
extern int ov00_021FE3D8(void);
extern int gpInitialize(GPConnection *connection, int productId, int flag);
extern int gpDestroy(GPConnection *connection);
extern int gpProcess(GPConnection *connection);
extern int gpSetCallback(GPConnection *connection, int type, void *callback, void *param);
extern int gpGetBuddyStatus(GPConnection *connection, int index, FriendBuddyStatus *status);
extern int gpGetBuddyIndex(GPConnection *connection, int profileId, int *index);
extern int gpIsBuddy(GPConnection *connection, int profileId);
extern void DWCi_TransportProcess(void);
extern void DWCi_TransportRecv(void *connection, void *data, int size, int reliable);
extern void DWCi_ResetTransportChannel(u32 aid);
extern void DWCi_ShutdownTransport(void);
extern void DWCi_InitTransport(Transport *transport);

u32 DWCi_WStrLen(const u16 *s) {
    u32 n = 0;
    while (s[n]) {
        n++;
    }
    return n;
}

void DWC_InitFriendsMatch(DwcControl *control, DwcUserData *userData, int productId, const char *gameName, const char *secretKey, int sendBufferSize, int recvBufferSize, DwcAccData *friends, int friendCount) {
    u32 size;
    ov00_0221A784 = control;
    DWC_ClearError();
    ov00_0221A784->socket = NULL;
    ov00_0221A784->socketCallbacks[0] = (void *)DWCi_GT2ConnectedCallback;
    ov00_0221A784->socketCallbacks[1] = (void *)DWCi_GT2ReceivedCallback;
    ov00_0221A784->socketCallbacks[2] = (void *)DWCi_GT2ClosedCallback;
    ov00_0221A784->socketCallbacks[3] = (void *)DWCi_GT2PingCallback;
    ov00_0221A784->bufferSize[0] = sendBufferSize ? sendBufferSize : 0x2000;
    ov00_0221A784->bufferSize[1] = recvBufferSize ? recvBufferSize : 0x2000;
    ov00_0221A784->connection = NULL;
    ov00_0221A784->userData = userData;
    ov00_0221A784->state = 0;
    ov00_0221A784->prevState = 0;
    ov00_0221A784->aid = 0;
    ov00_0221A784->closing = 0;
    ov00_0221A784->profileId = 0;
    ov00_0221A784->gameName = (const char *)ov00_0221B430;
    ov00_0221A784->secretKey = (const char *)ov00_0221B530;
    ov00_0221A784->loginCallback = NULL;
    ov00_0221A784->loginParam = NULL;
    ov00_0221A784->updateCallback = NULL;
    ov00_0221A784->updateParam = NULL;
    ov00_0221A784->matchCallback = NULL;
    ov00_0221A784->matchParam = NULL;
    ov00_0221A784->friendMatchCallback = NULL;
    ov00_0221A784->friendMatchParam = NULL;
    ov00_0221A784->closedCallback = NULL;
    ov00_0221A784->closedParam = NULL;
    DWCi_ClearGT2ConnectionList();
    DWCi_LoginInit(&ov00_0221A784->login, userData, &ov00_0221A784->connection, productId, userData->gameCode, ov00_0221A784->playerName, DWCi_LoginCallback, NULL);
    DWCi_FriendInit(&ov00_0221A784->friends, &ov00_0221A784->connection, ov00_0221A784->playerName, friends, friendCount);
    DWCi_MatchInit(&ov00_0221A784->match, &ov00_0221A784->connection, &ov00_0221A784->socket, ov00_0221A784->socketCallbacks, (const char *)ov00_0221B430, (const char *)ov00_0221B530, friends, friendCount);
    DWCi_InitTransport(&ov00_0221A784->transport);
    if (strlen(gameName) >= 256) {
        size = 255;
    } else {
        size = strlen(gameName);
    }
    MI_CpuCopy8(gameName, ov00_0221B430, size);
    ov00_0221B430[size] = 0;
    if (strlen(secretKey) >= 256) {
        size = 255;
    } else {
        size = strlen(secretKey);
    }
    MI_CpuCopy8(secretKey, ov00_0221B530, size);
    ov00_0221B530[size] = 0;
}

void DWC_ShutdownFriendsMatch(void) {
    if (!ov00_0221A784) {
        return;
    }
    if (ov00_0221A784->match.reporting) {
        qr2_shutdown(ov00_0221A784->match.reporting);
        ov00_0221A784->match.reporting = NULL;
    }
    ov00_0221A784->match.unk_18 = 0;
    if (ov00_0221A784->match.list) {
        ServerBrowserFree(ov00_0221A784->match.list);
        ov00_0221A784->match.list = NULL;
    }
    DWCi_NNFreeNegotiateList();
    CloseStatsConnection();
    if (ov00_0221A784->connection) {
        gpSetCallback(&ov00_0221A784->connection, 0, NULL, NULL);
        gpSetCallback(&ov00_0221A784->connection, 3, NULL, NULL);
        gpSetCallback(&ov00_0221A784->connection, 1, NULL, NULL);
        gpSetCallback(&ov00_0221A784->connection, 2, NULL, NULL);
        gpProcess(&ov00_0221A784->connection);
        gpDestroy(&ov00_0221A784->connection);
        ov00_0221A784->connection = NULL;
    }
    DWCi_ShutdownLogin();
    DWCi_ShutdownFriend();
    DWCi_ShutdownMatch();
    DWCi_ShutdownTransport();
    if (ov00_0221A784->socket) {
        gt2CloseSocket(ov00_0221A784->socket);
        ov00_0221A784->socket = NULL;
    }
    ov00_0221A784 = NULL;
}

void DWC_ProcessFriendsMatch(void) {
    if (DWC_UpdateConnection()) {
        DWCi_DummyProcess();
    }
    if (!ov00_0221A784 || !ov00_0221A784->state) {
        return;
    }
    if (DWCi_IsError()) {
        return;
    }
    switch (ov00_0221A784->state) {
    case 1:
        switch (ov00_021FE3D8()) {
        case 1:
            if (DWCi_MainHandleGPError(gpInitialize(&ov00_0221A784->connection, ov00_0221A784->login.productId, 0))) {
                return;
            }
            if (DWCi_MainHandleGPError(gpSetCallback(&ov00_0221A784->connection, 0, DWCi_GPErrorCallback, NULL))) {
                return;
            }
            if (DWCi_MainHandleGPError(gpSetCallback(&ov00_0221A784->connection, 3, DWCi_GPRecvBuddyMessageCallback, NULL))) {
                return;
            }
            if (DWCi_MainHandleGPError(gpSetCallback(&ov00_0221A784->connection, 1, DWCi_GPRecvBuddyRequestCallback, NULL))) {
                return;
            }
            if (DWCi_MainHandleGPError(gpSetCallback(&ov00_0221A784->connection, 2, DWCi_GPRecvBuddyStatusCallback, NULL))) {
                return;
            }
            DWCi_SetState(2);
            DWCi_LoginAsync();
            break;
        case 2:
            DWCi_StopLogin(3, -20110);
            return;
        case 3:
            DWCi_StopLogin(4, -20101);
            return;
        }
        break;
    case 2:
        DWCi_LoginProcess();
        break;
    case 3:
    case 4:
        DWCi_FriendProcess();
        DWCi_MatchProcess(FALSE);
        break;
    case 5:
        DWCi_MatchProcess(TRUE);
        DWCi_FriendProcess();
        break;
    case 6:
        DWCi_TransportProcess();
        DWCi_FriendProcess();
        if (VOLATILE_U8(ov00_0221A784->match.mode) == 2 || VOLATILE_U8(ov00_0221A784->match.mode) == 3) {
            DWCi_MatchProcess(TRUE);
        } else if (ov00_0221A784->socket) {
            DWCi_MatchProcess(FALSE);
        }
        break;
    }
    if (ov00_0221A784->match.unk_18 == 1) {
        if (ov00_0221A784->match.reporting) {
            qr2_shutdown(ov00_0221A784->match.reporting);
            ov00_0221A784->match.reporting = NULL;
        }
        ov00_0221A784->match.unk_18 = 0;
    }
}

int DWC_LoginAsync(const u16 *playerName, int unused, DwcResultCallback callback, void *param) {
    int length;
    if (!playerName) {
        return 0;
    }
    if (DWCi_IsError() || ov00_0221A784->state) {
        return 0;
    }
    ov00_0221A784->loginCallback = callback;
    ov00_0221A784->loginParam = param;
    if (!playerName || !*playerName) {
        length = 0;
    } else {
        MI_CpuClear16(ov00_0221A784->playerName, sizeof(ov00_0221A784->playerName));
        if (DWCi_WStrLen(playerName) > 25) {
            length = 25;
        } else {
            length = DWCi_WStrLen(playerName);
        }
        MI_CpuCopy16(playerName, ov00_0221A784->playerName, length * 2);
    }
    ov00_0221A784->playerName[length] = 0;
    if (DWC_GetInetStatus() != 4) {
        DWCi_StopLogin(2, -60010);
        return 1;
    }
    DWCi_SetState(1);
    GSIStartAvailableCheck(ov00_0221A784->gameName);
    return 1;
}

int DWC_UpdateServersAsync(int unused, DwcResultCallback callback, void *param, void *statusCallback, void *statusParam, void *deleteCallback, void *deleteParam) {
    if (DWCi_IsError() || ov00_0221A784->state < 3 || ov00_0221A784->state == 4) {
        return 0;
    }
    ov00_0221A784->updateCallback = callback;
    ov00_0221A784->updateParam = param;
    DWCi_SetState(4);
    DWCi_UpdateServersAsync(ov00_0221A784->login.token, ov00_0221A784->login.challenge, (void *)DWCi_UpdateServersCallback, NULL, statusCallback, statusParam, deleteCallback, deleteParam);
    return 1;
}

int DWC_ConnectToAnybodyAsync(int count, const char *filter, DwcResultCallback callback, void *param, void *evalCallback, void *evalParam) {
    if (DWCi_IsError() || ov00_0221A784->state != 3) {
        return 0;
    }
    DWCi_ClearGT2ConnectionList();
    ov00_0221A784->matchCallback = callback;
    ov00_0221A784->matchParam = param;
    DWCi_SetState(5);
    DWCi_ConnectToAnybodyAsync((count - 1) & 0xFF, filter, (void *)DWCi_MatchedCallback, NULL, evalCallback, evalParam);
    return 1;
}

int DWC_SetupGameServer(int count, DwcMatchedCallback callback, void *param, void *evalCallback, void *evalParam) {
    if (DWCi_IsError() || ov00_0221A784->state != 3) {
        return 0;
    }
    DWCi_ClearGT2ConnectionList();
    ov00_0221A784->friendMatchCallback = callback;
    ov00_0221A784->friendMatchParam = param;
    ov00_0221A784->aid = 0;
    DWCi_SetState(5);
    DWCi_SetupGameServer((count - 1) & 0xFF, (void *)DWCi_MatchedCallback, NULL, evalCallback, evalParam);
    return 1;
}

int DWC_ConnectToGameServerAsync(int index, DwcMatchedCallback callback, void *param, void *newPeerCallback, void *newPeerParam) {
    struct {
        char value[4];
        int buddyIndex;
        FriendBuddyStatus status;
    } local;

    local.buddyIndex = -1;
    int profileId, error;
    u8 a, b;
    if (DWCi_IsError() != 0 || ov00_0221A784->state != 3) {
        return 0;
    }
    DWCi_ClearGT2ConnectionList();
    ov00_0221A784->friendMatchCallback = callback;
    ov00_0221A784->friendMatchParam = param;
    DWCi_SetState(5);
    profileId = DWCi_GetProfileIDFromList(index);
    if (profileId == 0 || gpIsBuddy(&ov00_0221A784->connection, profileId) == 0) {
        error = 11;
        goto fail;
    }
    gpGetBuddyIndex(&ov00_0221A784->connection, profileId, &local.buddyIndex);
    gpGetBuddyStatus(&ov00_0221A784->connection, local.buddyIndex, &local.status);
    if (local.status.status != 6) {
        error = 11;
        goto fail;
    }
    local.value[0] = '0';
    DWC_GetCommonValueString(ov00_022179EC, local.value, local.status.statusString, '/');
    a = strtoul(local.value, NULL, 10);
    DWC_GetCommonValueString(ov00_022179F0, local.value, local.status.statusString, '/');
    b = strtoul(local.value, NULL, 10);
    if (b == a) {
        error = 13;
        goto fail;
    }
    DWCi_ConnectToGameServerAsync(profileId, (void *)DWCi_MatchedCallback, NULL, newPeerCallback, newPeerParam);
    return 1;
fail:
    DWCi_SetError(error, 0);
    ov00_0221A784->friendMatchCallback(error, FALSE, TRUE, FALSE, 0, ov00_0221A784->friendMatchParam);
    if (ov00_0221A784 && ov00_0221A784->state == 5) {
        DWCi_SetState(3);
        DWCi_SetGPStatus(1, ov00_022179F4, NULL);
    }
    return 1;
}

int DWC_SetConnectionClosedCallback(DwcClosedCallback callback, void *param) {
    if (!ov00_0221A784) {
        return 0;
    }
    ov00_0221A784->closedCallback = callback;
    ov00_0221A784->closedParam = param;
    return 1;
}

int DWC_CloseAllConnectionsHard(void) {
    if (!ov00_0221A784 || DWCi_IsError() || (ov00_0221A784->state != 5 && ov00_0221A784->state != 6)) {
        return -1;
    }
    if (ov00_0221A784->match.players == 0) {
        DWCi_SetGPStatus(1, ov00_022179F4, NULL);
        DWCi_NNFreeNegotiateList();
        DWCi_SetState(3);
        return 1;
    }
    ov00_0221A784->closing = 1;
    gt2CloseAllConnectionsHard(ov00_0221A784->socket);
    ov00_0221A784->closing = 0;
    return 0;
}

int DWC_CloseConnectionHard(int aid) {
    void *connection;
    if (!ov00_0221A784 || DWCi_IsError() || (ov00_0221A784->state != 5 && ov00_0221A784->state != 6)) {
        return -1;
    }
    if ((connection = DWCi_GetGT2Connection(aid)) == NULL) {
        return -2;
    }
    gt2CloseConnectionHard(connection);
    return 0;
}

int DWC_CloseConnectionHardBitmap(u32 *aidMask) {
    u8 i;
    if (!ov00_0221A784 || !aidMask || DWCi_IsError() || (ov00_0221A784->state != 5 && ov00_0221A784->state != 6)) {
        return -1;
    }
    for (i = 0; i < 32; i++) {
        if (*aidMask & (1 << i)) {
            if (i == DWC_GetMyAID()) {
                *aidMask &= ~(1 << i);
            } else if (DWC_CloseConnectionHard(i) != 0) {
                *aidMask &= ~(1 << i);
            }
        }
    }
    return *aidMask ? 0 : -2;
}

int DWC_GetNumConnectionHost(void) {
    if (!ov00_0221A784) {
        return 0;
    }
    if (VOLATILE_U8(ov00_0221A784->match.mode) == 2 || VOLATILE_U8(ov00_0221A784->match.mode) == 3) {
        return DWCi_GetNumValidAids() + 1;
    }
    return DWCi_GetNumConnectedPeers() + 1;
}

int DWC_GetMyAID(void) {
    return ov00_0221A784 ? ov00_0221A784->aid : 0;
}

int DWC_GetAIDList(u8 **aids) {
    if (!ov00_0221A784) {
        return 0;
    }
    *aids = ov00_0221A784->match.aids;
    if (VOLATILE_U8(ov00_0221A784->match.mode) == 2 || VOLATILE_U8(ov00_0221A784->match.mode) == 3) {
        return DWCi_GetValidAIDList(aids);
    }
    return DWCi_GetAIDList(aids);
}

u32 DWC_GetAIDBitmap(void) {
    u8 *aids;
    int count;
    if (!ov00_0221A784) {
        return 0;
    }
    count = DWC_GetAIDList(&aids);
    return DWCi_GetAIDBitmapFromList(aids, count);
}

int DWC_IsValidAID(int aid) {
    if (!ov00_0221A784) {
        return 0;
    }
    if (!(ov00_0221A784->match.aidMask & (1 << aid))) {
        return 0;
    }
    return DWCi_IsConnectedAID(aid);
}

int DWCi_GetState(void) {
    return ov00_0221A784 ? ov00_0221A784->state : 0;
}

int DWCi_CreateGT2Socket(void) {
    u16 port;
    u32 addr;
    DwcControl *control;
    int result;
    if (ov00_0221A784->socket) {
        return 0;
    }
    port = (u16)(DWCi_GetRandom(0x4000) + 0xC000);
    control = ov00_0221A784;
    addr = gt2AddressToString(0, port, 0);
    result = gt2CreateSocket(&ov00_0221A784->socket, addr, control->bufferSize[0], control->bufferSize[1], DWCi_GT2SocketErrorCallback);
    if (DWCi_MainHandleGT2Error(result)) {
        return result;
    }
    gt2Listen(ov00_0221A784->socket, DWCi_GT2ConnectAttemptCallback);
    gt2SetUnrecognizedMessageCallback(ov00_0221A784->socket, DWCi_GT2UnrecognizedMessageCallback);
    return result;
}

void *DWCi_GetGT2Connection(int aid) {
    int i;
    if (!ov00_0221A784) {
        return NULL;
    }
    for (i = 0; i < 32; i++) {
        if (ov00_0221A78C[i] && aid == gt2GetConnectionData(ov00_0221A78C[i])->aid) {
            return ov00_0221A78C[i];
        }
    }
    return NULL;
}

int DWCi_GetConnectionAID(void *connection) {
    return gt2GetConnectionData(connection)->aid;
}

int DWCi_GetConnectionIndex(void *connection) {
    return gt2GetConnectionData(connection)->index;
}

int DWCi_GetFreeConnectionIndex(void) {
    int i;
    for (i = 0; i < 32; i++) {
        if (!ov00_0221A78C[i]) {
            return i;
        }
    }
    return -1;
}

void DWCi_ClearGT2ConnectionList(void) {
    MIi_CpuClear32(0, (u32 *)ov00_0221A78C, sizeof(ov00_0221A78C));
    MIi_CpuClear32(0, (u32 *)ov00_0221A80C, sizeof(ov00_0221A80C));
}

void **DWCi_GetGT2ConnectionByIdx(int index) {
    return &ov00_0221A78C[index];
}

void **DWCi_GetGT2ConnectionByProfileID(int profileId, int count) {
    u8 i;
    for (i = 0; i < count; i++) {
        if (profileId == ov00_0221A784->match.profiles[i]) {
            break;
        }
    }
    if (i >= count) {
        return NULL;
    }
    return DWCi_GetGT2ConnectionByIdx(DWCi_GetConnectionIndex(DWCi_GetGT2Connection(ov00_0221A784->match.aids[i])));
}

u32 *DWCi_GetConnectionInfoByIdx(int index) {
    return ov00_0221A80C[index];
}

BOOL DWCi_IsConnectedAID(int aid) {
    int i;
    for (i = 0; i < 32; i++) {
        if (ov00_0221A78C[i] && aid == gt2GetConnectionData(ov00_0221A78C[i])->aid) {
            return TRUE;
        }
    }
    return FALSE;
}

void DWCi_DummyProcess(void) {
}

void DWCi_SetState(int state) {
    ov00_0221A784->prevState = ov00_0221A784->state;
    ov00_0221A784->state = state;
}

// Profile id of the peer with this aid, 0 if there is none.
int DWCi_GetProfileIDFromAID(int aid) {
    u8 *aids;
    int count = DWCi_GetAIDList(&aids);
    int i = 0;
    if (count > 0) {
        u8 *cursor = aids;
        do {
            if (aid == *cursor) {
                break;
            }
            i++;
            cursor++;
        } while (i < count);
    }
    if (i == count) {
        return 0;
    }
    return DWCi_DeleteHostByIndex(i, count);
}

u32 DWCi_GetAIDBitmapFromList(const u8 *aids, int count) {
    u32 mask = 0;
    int i;
    for (i = 0; i < count; i++) {
        mask |= 1 << aids[i];
    }
    return mask;
}

// Reports a GameSpy presence error for the current state.
int DWCi_MainHandleGPError(int error) {
    int code, kind;
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
    switch (ov00_0221A784->state) {
    case 1:
        code += -61000;
        DWCi_StopLogin(kind, code);
        break;
    case 2:
        code += -61000;
        if (ov00_0221A784->login.state < 1) {
            DWCi_StopLogin(kind, code);
        }
        break;
    case 5:
        code += -81000;
        DWCi_StopMatching(kind, code);
        break;
    case 4:
        code += -71000;
        break;
    default:
        code += -91000;
        break;
    }
    DWCi_StopFriendProcess(kind, code);
    return error;
}

// Reports a GT2 socket error.
int DWCi_MainHandleGT2Error(int error) {
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
    case 5:
        kind = 0;
        code = 0;
        error = 0;
        break;
    case 3:
        kind = 6;
        code = -10;
        break;
    case 4:
        kind = 6;
        code = -30;
        break;
    case 6:
        kind = 6;
        code = -70;
        break;
    case 7:
        kind = 6;
        code = -80;
        break;
    }
    if (kind) {
        DWCi_StopLogin(kind, code - 67000);
    }
    return error;
}

// Log-in finished.
void DWCi_LoginCallback(int error, int profileId, void *param) {
    if (error == 0) {
        ov00_0221A784->profileId = profileId;
        DWCi_SetState(3);
        DWCi_ResetFriendPoll();
    } else {
        DWCi_SetState(0);
    }
    if (ov00_0221A784->loginCallback) {
        ov00_0221A784->loginCallback(error, profileId, ov00_0221A784->loginParam);
    }
}

// Friend list update finished.
void DWCi_UpdateServersCallback(int error, BOOL changed, void *param) {
    if (ov00_0221A784->prevState != 4) {
        DWCi_SetState(ov00_0221A784->prevState);
    }
    ov00_0221A784->updateCallback(error, changed, ov00_0221A784->updateParam);
}

// Matchmaking finished.
void DWCi_MatchedCallback(int error, BOOL cancelled, BOOL self, BOOL isServer, int index, void *param) {
    if (!error && cancelled) {
        if (!ov00_0221A784->match.state) {
            DWCi_ResetReportedPlayerCount();
            DWCi_SetState(3);
        }
    } else if (!error) {
        DwcControl *control;
        int i;
        int profileId;
        DWCi_SetState(6);
        control = ov00_0221A784;
        i = 0;
        if (i <= VOLATILE_U8(control->match.players)) {
            profileId = control->profileId;
            do {
                if (profileId == control->match.profiles[i]) {
                    VOLATILE_U8(ov00_0221A784->aid) = VOLATILE_U8(ov00_0221A784->match.aids[i]);
                    break;
                }
            } while (++i <= VOLATILE_U8(control->match.players));
        }
    }
    ov00_0221A784->match.aidMask = DWCi_GetAIDBitmapFromList(ov00_0221A784->match.aids, VOLATILE_U8(ov00_0221A784->match.players) + 1);
    DWCi_UpdateValidAidCount();
    if (VOLATILE_U8(ov00_0221A784->match.mode) == 2 || VOLATILE_U8(ov00_0221A784->match.mode) == 3) {
        ov00_0221A784->friendMatchCallback(error, cancelled, self, isServer, index, ov00_0221A784->friendMatchParam);
    } else {
        ov00_0221A784->matchCallback(error, cancelled, ov00_0221A784->matchParam);
    }
    if (error && ov00_0221A784 && ov00_0221A784->state == 5) {
        DWCi_SetState(3);
    }
}

// GameSpy presence error.
void DWCi_GPErrorCallback(GPConnection *connection, void *arg, void *param) {
    u32 *error = arg;
    if (error[1] == 0x603 || error[1] == 0x901 || error[1] == 0xB01) {
        return;
    }
    DWCi_MainHandleGPError(3);
}

// Buddy message: "GPCM<version>vMAT..." messages are matchmaking commands.
void DWCi_GPRecvBuddyMessageCallback(GPConnection *connection, void *arg, void *param) {
    GpBuddyMessage *message = arg;
    char version[12] = { 0 };
    char *text = message->message;
    u32 len;
    if (DWCi_GPRecvBuddyAuthCallback(connection, message, param) != 0) {
        return;
    }
    if (memcmp(text, ov00_022179F8, strlen(ov00_022179F8)) != 0) {
        return;
    }
    text += strlen(ov00_022179F8);
    len = strchr(text, 'v') - text;
    strncpy(version, text, len);
    if (len > 10) {
        return;
    }
    if (strtoul(version, NULL, 10) != 3) {
        return;
    }
    text += len + 1;
    if (memcmp(text, ov00_02217A00, strlen(ov00_02217A00)) != 0) {
        return;
    }
    switch (ov00_0221A784->state) {
    case 5:
        break;
    case 6:
        if (VOLATILE_U8(ov00_0221A784->match.mode) == 2 || VOLATILE_U8(ov00_0221A784->match.mode) == 3) {
            break;
        }
    default:
        return;
    }
    DWCi_ProcessGPMatchCommand((int)connection, (void *)message->profileId, (u8 *)text + strlen(ov00_02217A00));
}

void DWCi_GT2ReceivedCallback(void *connection, void *data, int size, int reliable) {
    DWCi_TransportRecv(connection, data, size, reliable);
}

// A GT2 connection was closed.
void DWCi_GT2ClosedCallback(void *connection, int reason) {
    u32 aid;
    int profileId = 0;
    int code;
    DwcConnectionInfo *info;
    BOOL known;
    BOOL isServer = FALSE;
    int error;
    if (DWCi_IsShutdownMatch()) {
        return;
    }
    switch (reason) {
    case 0:
    case 1:
        error = 0;
        break;
    case 2:
    case 3:
        error = 6;
        code = -7600;
        break;
    case 4:
        code = -7601;
        error = 9;
        break;
    }
    if (error == 0) {
        info = gt2GetConnectionData(connection);
        if (info == NULL) {
            return;
        }
        aid = info->aid;
        known = TRUE;
        if (!(ov00_0221A784->match.aidMask & (1 << aid))) {
            known = FALSE;
        }
        DWCi_ResetTransportChannel(aid);
        if ((VOLATILE_U8(ov00_0221A784->match.mode) == 2 && reason == 0) || (VOLATILE_U8(ov00_0221A784->match.mode) == 3 && aid == 0)) {
            isServer = TRUE;
        }
        profileId = DWCi_GetProfileIDFromAID(aid);
        ov00_0221A78C[info->index] = NULL;
        if (ov00_0221A784->match.players) {
            ov00_0221A784->match.players--;
        }
        if (ov00_0221A784->match.connections) {
            ov00_0221A784->match.connections--;
        }
    }
    if (ov00_0221A784->closing == 0 && ov00_0221A784->state == 6 && known == FALSE) {
        if (VOLATILE_U8(ov00_0221A784->match.mode) == 2 && error == 0) {
            DWCi_UpdateGameServerStatus();
            DWCi_HandlePeerConnectionClosed(profileId);
        }
        return;
    }
    // DWCi_HandleServerConnectionClosed ignores the profile id
    if (((int (*)(int, int, int))DWCi_HandleServerConnectionClosed)(error, code, profileId)) {
        return;
    }
    if (error) {
        DWCi_SetError(error, code - 80000);
        return;
    }
    if (!ov00_0221A784->closing && (VOLATILE_U8(ov00_0221A784->match.mode) == 2 || VOLATILE_U8(ov00_0221A784->match.mode) == 3)) {
        int n = ov00_0221A784->match.players;
        if (ov00_0221A784->match.profiles[n + 2] != 0) {
            ov00_0221A784->match.aids[n + 1] = ov00_0221A784->match.aids[n + 2];
            DWCi_DeleteHostByIndex(ov00_0221A784->match.players + 1, ov00_0221A784->match.players + 3);
        }
    }
    if (VOLATILE_U8(ov00_0221A784->match.mode) == 2) {
        if (!ov00_0221A784->closing) {
            DWCi_UpdateGameServerStatus();
        } else if (!ov00_0221A784->match.players) {
            DWCi_SetGPStatus(1, ov00_022179F4, NULL);
        }
    } else if (!ov00_0221A784->match.players) {
        DWCi_SetGPStatus(1, ov00_022179F4, NULL);
    }
    if (VOLATILE_U8(ov00_0221A784->match.mode) == 0 || VOLATILE_U8(ov00_0221A784->match.mode) == 1) {
        ov00_0221A784->match.limit = ov00_0221A784->match.connections;
        qr2_send_statechanged(ov00_0221A784->match.reporting);
    }
    {
        DwcControl *control = ov00_0221A784;
        if (control->closedCallback && known) {
            ov00_0221A784->closedCallback(error, reason == 0, isServer, aid, DWCi_GetFriendListIndex(profileId), control->closedParam);
        }
    }
    if (!ov00_0221A784->closing && VOLATILE_U8(ov00_0221A784->match.mode) == 2) {
        return;
    }
    if (ov00_0221A784->match.players) {
        return;
    }
    DWCi_NNFreeNegotiateList();
    DWCi_ResetReportedPlayerCount();
    DWCi_SetState(3);
}
