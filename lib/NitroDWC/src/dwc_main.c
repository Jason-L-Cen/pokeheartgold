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
extern void ov00_0220AE60(void);
extern int ov00_0220CF98(void **socket, u32 addr, u32 sendSize, u32 recvSize, void (*errorCallback)(void *socket));
extern void ov00_0220CFAC(void *socket);
extern void ov00_0220CFF4(void *socket, void *callback);
extern void ov00_0220D218(void *connection);
extern void ov00_0220D238(void *socket);
extern void ov00_0220D29C(void *socket, void *callback);
extern DwcConnectionInfo *ov00_0220D2E0(void *connection);
extern u32 ov00_0220F724(u32 addr, u16 port, int flag);
extern void ov00_02210CBC(void *reporting);
extern void ov00_02210D18(void *reporting);
extern void ov00_022136E4(void *list);
extern void ov00_021FE228(const char *gameName);
extern int ov00_021FE3D8(void);
extern int ov00_021FFD50(GPConnection *connection, int productId, int flag);
extern int ov00_021FFD80(GPConnection *connection);
extern int ov00_021FFD9C(GPConnection *connection);
extern int ov00_021FFDD0(GPConnection *connection, int type, void *callback, void *param);
extern int ov00_02200400(GPConnection *connection, int index, FriendBuddyStatus *status);
extern int ov00_02200540(GPConnection *connection, int profileId, int *index);
extern int ov00_022005B0(GPConnection *connection, int profileId);
extern void ov00_021F9A8C(void);
extern void ov00_021F9A10(void *connection, void *data, int size, int reliable);
extern void ov00_021F9C20(u32 aid);
extern void ov00_021F9CA8(void);
extern void ov00_021F99D4(Transport *transport);

u32 ov00_021ED704(const u16 *s) {
    u32 n = 0;
    while (s[n]) {
        n++;
    }
    return n;
}

void ov00_021ED730(DwcControl *control, DwcUserData *userData, int productId, const char *gameName, const char *secretKey, int sendBufferSize, int recvBufferSize, DwcAccData *friends, int friendCount) {
    u32 size;
    ov00_0221A784 = control;
    ov00_021EC210();
    ov00_0221A784->socket = NULL;
    ov00_0221A784->socketCallbacks[0] = (void *)ov00_021F2BBC;
    ov00_0221A784->socketCallbacks[1] = (void *)ov00_021EEE34;
    ov00_0221A784->socketCallbacks[2] = (void *)ov00_021EEE40;
    ov00_0221A784->socketCallbacks[3] = (void *)ov00_021EF1A8;
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
    ov00_021EE6FC();
    ov00_021EF1F0(&ov00_0221A784->login, userData, &ov00_0221A784->connection, productId, userData->gameCode, ov00_0221A784->playerName, ov00_021EEA74, NULL);
    ov00_021EFF48(&ov00_0221A784->friends, &ov00_0221A784->connection, ov00_0221A784->playerName, friends, friendCount);
    ov00_021F1620(&ov00_0221A784->match, &ov00_0221A784->connection, &ov00_0221A784->socket, ov00_0221A784->socketCallbacks, (const char *)ov00_0221B430, (const char *)ov00_0221B530, friends, friendCount);
    ov00_021F99D4(&ov00_0221A784->transport);
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

void ov00_021ED9B4(void) {
    if (!ov00_0221A784) {
        return;
    }
    if (ov00_0221A784->match.reporting) {
        ov00_02210D18(ov00_0221A784->match.reporting);
        ov00_0221A784->match.reporting = NULL;
    }
    ov00_0221A784->match.unk_18 = 0;
    if (ov00_0221A784->match.list) {
        ov00_022136E4(ov00_0221A784->match.list);
        ov00_0221A784->match.list = NULL;
    }
    ov00_021F125C();
    ov00_0220AE60();
    if (ov00_0221A784->connection) {
        ov00_021FFDD0(&ov00_0221A784->connection, 0, NULL, NULL);
        ov00_021FFDD0(&ov00_0221A784->connection, 3, NULL, NULL);
        ov00_021FFDD0(&ov00_0221A784->connection, 1, NULL, NULL);
        ov00_021FFDD0(&ov00_0221A784->connection, 2, NULL, NULL);
        ov00_021FFD9C(&ov00_0221A784->connection);
        ov00_021FFD80(&ov00_0221A784->connection);
        ov00_0221A784->connection = NULL;
    }
    ov00_021EF410();
    ov00_021F05D8();
    ov00_021F378C();
    ov00_021F9CA8();
    if (ov00_0221A784->socket) {
        ov00_0220CFAC(ov00_0221A784->socket);
        ov00_0221A784->socket = NULL;
    }
    ov00_0221A784 = NULL;
}

void ov00_021EDB1C(void) {
    if (ov00_021ECB40()) {
        ov00_021EE7FC();
    }
    if (!ov00_0221A784 || !ov00_0221A784->state) {
        return;
    }
    if (ov00_021EC230()) {
        return;
    }
    switch (ov00_0221A784->state) {
    case 1:
        switch (ov00_021FE3D8()) {
        case 1:
            if (ov00_021EE8B4(ov00_021FFD50(&ov00_0221A784->connection, ov00_0221A784->login.productId, 0))) {
                return;
            }
            if (ov00_021EE8B4(ov00_021FFDD0(&ov00_0221A784->connection, 0, ov00_021EEC94, NULL))) {
                return;
            }
            if (ov00_021EE8B4(ov00_021FFDD0(&ov00_0221A784->connection, 3, ov00_021EECCC, NULL))) {
                return;
            }
            if (ov00_021EE8B4(ov00_021FFDD0(&ov00_0221A784->connection, 1, ov00_021F0290, NULL))) {
                return;
            }
            if (ov00_021EE8B4(ov00_021FFDD0(&ov00_0221A784->connection, 2, ov00_021F0338, NULL))) {
                return;
            }
            ov00_021EE800(2);
            ov00_021EF274();
            break;
        case 2:
            ov00_021EF3C0(3, -20110);
            return;
        case 3:
            ov00_021EF3C0(4, -20101);
            return;
        }
        break;
    case 2:
        ov00_021EF2AC();
        break;
    case 3:
    case 4:
        ov00_021F0028();
        ov00_021F1DE4(FALSE);
        break;
    case 5:
        ov00_021F1DE4(TRUE);
        ov00_021F0028();
        break;
    case 6:
        ov00_021F9A8C();
        ov00_021F0028();
        if (VOLATILE_U8(ov00_0221A784->match.mode) == 2 || VOLATILE_U8(ov00_0221A784->match.mode) == 3) {
            ov00_021F1DE4(TRUE);
        } else if (ov00_0221A784->socket) {
            ov00_021F1DE4(FALSE);
        }
        break;
    }
    if (ov00_0221A784->match.unk_18 == 1) {
        if (ov00_0221A784->match.reporting) {
            ov00_02210D18(ov00_0221A784->match.reporting);
            ov00_0221A784->match.reporting = NULL;
        }
        ov00_0221A784->match.unk_18 = 0;
    }
}

int ov00_021EDD6C(const u16 *playerName, int unused, DwcResultCallback callback, void *param) {
    int length;
    if (!playerName) {
        return 0;
    }
    if (ov00_021EC230() || ov00_0221A784->state) {
        return 0;
    }
    ov00_0221A784->loginCallback = callback;
    ov00_0221A784->loginParam = param;
    if (!playerName || !*playerName) {
        length = 0;
    } else {
        MI_CpuClear16(ov00_0221A784->playerName, sizeof(ov00_0221A784->playerName));
        if (ov00_021ED704(playerName) > 25) {
            length = 25;
        } else {
            length = ov00_021ED704(playerName);
        }
        MI_CpuCopy16(playerName, ov00_0221A784->playerName, length * 2);
    }
    ov00_0221A784->playerName[length] = 0;
    if (ov00_021EC724() != 4) {
        ov00_021EF3C0(2, -60010);
        return 1;
    }
    ov00_021EE800(1);
    ov00_021FE228(ov00_0221A784->gameName);
    return 1;
}

int ov00_021EDE74(int unused, DwcResultCallback callback, void *param, void *statusCallback, void *statusParam, void *deleteCallback, void *deleteParam) {
    if (ov00_021EC230() || ov00_0221A784->state < 3 || ov00_0221A784->state == 4) {
        return 0;
    }
    ov00_0221A784->updateCallback = callback;
    ov00_0221A784->updateParam = param;
    ov00_021EE800(4);
    ov00_021F018C(ov00_0221A784->login.token, ov00_0221A784->login.challenge, (void *)ov00_021EEAD4, NULL, statusCallback, statusParam, deleteCallback, deleteParam);
    return 1;
}

int ov00_021EDF1C(int count, const char *filter, DwcResultCallback callback, void *param, void *evalCallback, void *evalParam) {
    if (ov00_021EC230() || ov00_0221A784->state != 3) {
        return 0;
    }
    ov00_021EE6FC();
    ov00_0221A784->matchCallback = callback;
    ov00_0221A784->matchParam = param;
    ov00_021EE800(5);
    ov00_021F1938((count - 1) & 0xFF, filter, (void *)ov00_021EEB1C, NULL, evalCallback, evalParam);
    return 1;
}

int ov00_021EDFBC(int count, DwcMatchedCallback callback, void *param, void *evalCallback, void *evalParam) {
    if (ov00_021EC230() || ov00_0221A784->state != 3) {
        return 0;
    }
    ov00_021EE6FC();
    ov00_0221A784->friendMatchCallback = callback;
    ov00_0221A784->friendMatchParam = param;
    ov00_0221A784->aid = 0;
    ov00_021EE800(5);
    ov00_021F1B24((count - 1) & 0xFF, (void *)ov00_021EEB1C, NULL, evalCallback, evalParam);
    return 1;
}

int ov00_021EE054(int index, DwcMatchedCallback callback, void *param, void *newPeerCallback, void *newPeerParam) {
    struct {
        char value[4];
        int buddyIndex;
        FriendBuddyStatus status;
    } local;

    local.buddyIndex = -1;
    int profileId, error;
    u8 a, b;
    if (ov00_021EC230() != 0 || ov00_0221A784->state != 3) {
        return 0;
    }
    ov00_021EE6FC();
    ov00_0221A784->friendMatchCallback = callback;
    ov00_0221A784->friendMatchParam = param;
    ov00_021EE800(5);
    profileId = ov00_021F03BC(index);
    if (profileId == 0 || ov00_022005B0(&ov00_0221A784->connection, profileId) == 0) {
        error = 11;
        goto fail;
    }
    ov00_02200540(&ov00_0221A784->connection, profileId, &local.buddyIndex);
    ov00_02200400(&ov00_0221A784->connection, local.buddyIndex, &local.status);
    if (local.status.status != 6) {
        error = 11;
        goto fail;
    }
    local.value[0] = '0';
    ov00_021ED4E8(ov00_022179EC, local.value, local.status.statusString, '/');
    a = strtoul(local.value, NULL, 10);
    ov00_021ED4E8(ov00_022179F0, local.value, local.status.statusString, '/');
    b = strtoul(local.value, NULL, 10);
    if (b == a) {
        error = 13;
        goto fail;
    }
    ov00_021F1BCC(profileId, (void *)ov00_021EEB1C, NULL, newPeerCallback, newPeerParam);
    return 1;
fail:
    ov00_021EC24C(error, 0);
    ov00_0221A784->friendMatchCallback(error, FALSE, TRUE, FALSE, 0, ov00_0221A784->friendMatchParam);
    if (ov00_0221A784 && ov00_0221A784->state == 5) {
        ov00_021EE800(3);
        ov00_021F04A0(1, ov00_022179F4, NULL);
    }
    return 1;
}

int ov00_021EE24C(DwcClosedCallback callback, void *param) {
    if (!ov00_0221A784) {
        return 0;
    }
    ov00_0221A784->closedCallback = callback;
    ov00_0221A784->closedParam = param;
    return 1;
}

int ov00_021EE278(void) {
    if (!ov00_0221A784 || ov00_021EC230() || (ov00_0221A784->state != 5 && ov00_0221A784->state != 6)) {
        return -1;
    }
    if (ov00_0221A784->match.players == 0) {
        ov00_021F04A0(1, ov00_022179F4, NULL);
        ov00_021F125C();
        ov00_021EE800(3);
        return 1;
    }
    ov00_0221A784->closing = 1;
    ov00_0220D238(ov00_0221A784->socket);
    ov00_0221A784->closing = 0;
    return 0;
}

int ov00_021EE31C(int aid) {
    void *connection;
    if (!ov00_0221A784 || ov00_021EC230() || (ov00_0221A784->state != 5 && ov00_0221A784->state != 6)) {
        return -1;
    }
    if ((connection = ov00_021EE64C(aid)) == NULL) {
        return -2;
    }
    ov00_0220D218(connection);
    return 0;
}

int ov00_021EE384(u32 *aidMask) {
    u8 i;
    if (!ov00_0221A784 || !aidMask || ov00_021EC230() || (ov00_0221A784->state != 5 && ov00_0221A784->state != 6)) {
        return -1;
    }
    for (i = 0; i < 32; i++) {
        if (*aidMask & (1 << i)) {
            if (i == ov00_021EE490()) {
                *aidMask &= ~(1 << i);
            } else if (ov00_021EE31C(i) != 0) {
                *aidMask &= ~(1 << i);
            }
        }
    }
    return *aidMask ? 0 : -2;
}

int ov00_021EE448(void) {
    if (!ov00_0221A784) {
        return 0;
    }
    if (VOLATILE_U8(ov00_0221A784->match.mode) == 2 || VOLATILE_U8(ov00_0221A784->match.mode) == 3) {
        return ov00_021F3570() + 1;
    }
    return ov00_021F3550() + 1;
}

int ov00_021EE490(void) {
    return ov00_0221A784 ? ov00_0221A784->aid : 0;
}

int ov00_021EE4AC(u8 **aids) {
    if (!ov00_0221A784) {
        return 0;
    }
    *aids = ov00_0221A784->match.aids;
    if (VOLATILE_U8(ov00_0221A784->match.mode) == 2 || VOLATILE_U8(ov00_0221A784->match.mode) == 3) {
        return ov00_021F3618(aids);
    }
    return ov00_021F35E4(aids);
}

u32 ov00_021EE4FC(void) {
    u8 *aids;
    int count;
    if (!ov00_0221A784) {
        return 0;
    }
    count = ov00_021EE4AC(&aids);
    return ov00_021EE880(aids, count);
}

int ov00_021EE530(int aid) {
    if (!ov00_0221A784) {
        return 0;
    }
    if (!(ov00_0221A784->match.aidMask & (1 << aid))) {
        return 0;
    }
    return ov00_021EE7B4(aid);
}

int ov00_021EE568(void) {
    return ov00_0221A784 ? ov00_0221A784->state : 0;
}

int ov00_021EE584(void) {
    u16 port;
    u32 addr;
    DwcControl *control;
    int result;
    if (ov00_0221A784->socket) {
        return 0;
    }
    port = (u16)(ov00_021ED5E4(0x4000) + 0xC000);
    control = ov00_0221A784;
    addr = ov00_0220F724(0, port, 0);
    result = ov00_0220CF98(&ov00_0221A784->socket, addr, control->bufferSize[0], control->bufferSize[1], ov00_021EF1B4);
    if (ov00_021EE9D0(result)) {
        return result;
    }
    ov00_0220CFF4(ov00_0221A784->socket, ov00_021F2974);
    ov00_0220D29C(ov00_0221A784->socket, ov00_021F2880);
    return result;
}

void *ov00_021EE64C(int aid) {
    int i;
    if (!ov00_0221A784) {
        return NULL;
    }
    for (i = 0; i < 32; i++) {
        if (ov00_0221A78C[i] && aid == ov00_0220D2E0(ov00_0221A78C[i])->aid) {
            return ov00_0221A78C[i];
        }
    }
    return NULL;
}

int ov00_021EE6B0(void *connection) {
    return ov00_0220D2E0(connection)->aid;
}

int ov00_021EE6C0(void *connection) {
    return ov00_0220D2E0(connection)->index;
}

int ov00_021EE6D0(void) {
    int i;
    for (i = 0; i < 32; i++) {
        if (!ov00_0221A78C[i]) {
            return i;
        }
    }
    return -1;
}

void ov00_021EE6FC(void) {
    MIi_CpuClear32(0, (u32 *)ov00_0221A78C, sizeof(ov00_0221A78C));
    MIi_CpuClear32(0, (u32 *)ov00_0221A80C, sizeof(ov00_0221A80C));
}

void **ov00_021EE72C(int index) {
    return &ov00_0221A78C[index];
}

void **ov00_021EE73C(int profileId, int count) {
    u8 i;
    for (i = 0; i < count; i++) {
        if (profileId == ov00_0221A784->match.profiles[i]) {
            break;
        }
    }
    if (i >= count) {
        return NULL;
    }
    return ov00_021EE72C(ov00_021EE6C0(ov00_021EE64C(ov00_0221A784->match.aids[i])));
}

u32 *ov00_021EE7A4(int index) {
    return ov00_0221A80C[index];
}

BOOL ov00_021EE7B4(int aid) {
    int i;
    for (i = 0; i < 32; i++) {
        if (ov00_0221A78C[i] && aid == ov00_0220D2E0(ov00_0221A78C[i])->aid) {
            return TRUE;
        }
    }
    return FALSE;
}

void ov00_021EE7FC(void) {
}

void ov00_021EE800(int state) {
    ov00_0221A784->prevState = ov00_0221A784->state;
    ov00_0221A784->state = state;
}

// Profile id of the peer with this aid, 0 if there is none.
int ov00_021EE820(int aid) {
    u8 *aids;
    int count = ov00_021F35E4(&aids);
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
    return ov00_021F3398(i, count);
}

u32 ov00_021EE880(const u8 *aids, int count) {
    u32 mask = 0;
    int i;
    for (i = 0; i < count; i++) {
        mask |= 1 << aids[i];
    }
    return mask;
}

// Reports a GameSpy presence error for the current state.
int ov00_021EE8B4(int error) {
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
        ov00_021EF3C0(kind, code);
        break;
    case 2:
        code += -61000;
        if (ov00_0221A784->login.state < 1) {
            ov00_021EF3C0(kind, code);
        }
        break;
    case 5:
        code += -81000;
        ov00_021F2EE4(kind, code);
        break;
    case 4:
        code += -71000;
        break;
    default:
        code += -91000;
        break;
    }
    ov00_021F0238(kind, code);
    return error;
}

// Reports a GT2 socket error.
int ov00_021EE9D0(int error) {
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
        ov00_021EF3C0(kind, code - 67000);
    }
    return error;
}

// Log-in finished.
void ov00_021EEA74(int error, int profileId, void *param) {
    if (error == 0) {
        ov00_0221A784->profileId = profileId;
        ov00_021EE800(3);
        ov00_021F0468();
    } else {
        ov00_021EE800(0);
    }
    if (ov00_0221A784->loginCallback) {
        ov00_0221A784->loginCallback(error, profileId, ov00_0221A784->loginParam);
    }
}

// Friend list update finished.
void ov00_021EEAD4(int error, BOOL changed, void *param) {
    if (ov00_0221A784->prevState != 4) {
        ov00_021EE800(ov00_0221A784->prevState);
    }
    ov00_0221A784->updateCallback(error, changed, ov00_0221A784->updateParam);
}

// Matchmaking finished.
void ov00_021EEB1C(int error, BOOL cancelled, BOOL self, BOOL isServer, int index, void *param) {
    if (!error && cancelled) {
        if (!ov00_0221A784->match.state) {
            ov00_021F2FA0();
            ov00_021EE800(3);
        }
    } else if (!error) {
        DwcControl *control;
        int i;
        int profileId;
        ov00_021EE800(6);
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
    ov00_0221A784->match.aidMask = ov00_021EE880(ov00_0221A784->match.aids, VOLATILE_U8(ov00_0221A784->match.players) + 1);
    ov00_021F3590();
    if (VOLATILE_U8(ov00_0221A784->match.mode) == 2 || VOLATILE_U8(ov00_0221A784->match.mode) == 3) {
        ov00_0221A784->friendMatchCallback(error, cancelled, self, isServer, index, ov00_0221A784->friendMatchParam);
    } else {
        ov00_0221A784->matchCallback(error, cancelled, ov00_0221A784->matchParam);
    }
    if (error && ov00_0221A784 && ov00_0221A784->state == 5) {
        ov00_021EE800(3);
    }
}

// GameSpy presence error.
void ov00_021EEC94(GPConnection *connection, void *arg, void *param) {
    u32 *error = arg;
    if (error[1] == 0x603 || error[1] == 0x901 || error[1] == 0xB01) {
        return;
    }
    ov00_021EE8B4(3);
}

// Buddy message: "GPCM<version>vMAT..." messages are matchmaking commands.
void ov00_021EECCC(GPConnection *connection, void *arg, void *param) {
    GpBuddyMessage *message = arg;
    char version[12] = { 0 };
    char *text = message->message;
    u32 len;
    if (ov00_021F02DC(connection, message, param) != 0) {
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
    ov00_021F2E40((int)connection, (void *)message->profileId, (u8 *)text + strlen(ov00_02217A00));
}

void ov00_021EEE34(void *connection, void *data, int size, int reliable) {
    ov00_021F9A10(connection, data, size, reliable);
}

// A GT2 connection was closed.
void ov00_021EEE40(void *connection, int reason) {
    u32 aid;
    int profileId = 0;
    int code;
    DwcConnectionInfo *info;
    BOOL known;
    BOOL isServer = FALSE;
    int error;
    if (ov00_021F3804()) {
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
        info = ov00_0220D2E0(connection);
        if (info == NULL) {
            return;
        }
        aid = info->aid;
        known = TRUE;
        if (!(ov00_0221A784->match.aidMask & (1 << aid))) {
            known = FALSE;
        }
        ov00_021F9C20(aid);
        if ((VOLATILE_U8(ov00_0221A784->match.mode) == 2 && reason == 0) || (VOLATILE_U8(ov00_0221A784->match.mode) == 3 && aid == 0)) {
            isServer = TRUE;
        }
        profileId = ov00_021EE820(aid);
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
            ov00_021F36BC();
            ov00_021F3310(profileId);
        }
        return;
    }
    // ov00_021F3194 ignores the profile id
    if (((int (*)(int, int, int))ov00_021F3194)(error, code, profileId)) {
        return;
    }
    if (error) {
        ov00_021EC24C(error, code - 80000);
        return;
    }
    if (!ov00_0221A784->closing && (VOLATILE_U8(ov00_0221A784->match.mode) == 2 || VOLATILE_U8(ov00_0221A784->match.mode) == 3)) {
        int n = ov00_0221A784->match.players;
        if (ov00_0221A784->match.profiles[n + 2] != 0) {
            ov00_0221A784->match.aids[n + 1] = ov00_0221A784->match.aids[n + 2];
            ov00_021F3398(ov00_0221A784->match.players + 1, ov00_0221A784->match.players + 3);
        }
    }
    if (VOLATILE_U8(ov00_0221A784->match.mode) == 2) {
        if (!ov00_0221A784->closing) {
            ov00_021F36BC();
        } else if (!ov00_0221A784->match.players) {
            ov00_021F04A0(1, ov00_022179F4, NULL);
        }
    } else if (!ov00_0221A784->match.players) {
        ov00_021F04A0(1, ov00_022179F4, NULL);
    }
    if (VOLATILE_U8(ov00_0221A784->match.mode) == 0 || VOLATILE_U8(ov00_0221A784->match.mode) == 1) {
        ov00_0221A784->match.limit = ov00_0221A784->match.connections;
        ov00_02210CBC(ov00_0221A784->match.reporting);
    }
    {
        DwcControl *control = ov00_0221A784;
        if (control->closedCallback && known) {
            ov00_0221A784->closedCallback(error, reason == 0, isServer, aid, ov00_021F0404(profileId), control->closedParam);
        }
    }
    if (!ov00_0221A784->closing && VOLATILE_U8(ov00_0221A784->match.mode) == 2) {
        return;
    }
    if (ov00_0221A784->match.players) {
        return;
    }
    ov00_021F125C();
    ov00_021F2FA0();
    ov00_021EE800(3);
}
