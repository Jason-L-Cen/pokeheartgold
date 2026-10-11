#pragma force_active on
#include <string.h>

#include "global.h"

#include "dwc/gp_internal.h"

// The code below is compiled as ARM (32-bit) code.
#include <nitro/code32.h>

// Presence connection set-up and processing ("gpi.c": create / reset / destroy, the server message loop,
// gpiProcess) and buddy messages ("gpiBuddy.c": "\bm\" send / receive, buddy requests and deletion).

extern const char ov00_02217EE4[]; // "The connection has already been disconnected."
extern const char ov00_02217FEC[]; // "Invalid message."
extern const char ov00_02218000[]; // "gpiProcess"
extern const char ov00_0221800C[]; // "\n\n\n\n\n*************\ngpiInitialize\n"
extern const char ov00_02218030[]; // "CM"
extern const char ov00_02218034[]; // "There was an error reading from the server."
extern const char ov00_02218060[]; // "\\final\\"
extern const char ov00_02218068[]; // "CMD: %s\n"
extern const char ov00_02218074[]; // "Out of memory."
extern const char ov00_02218084[]; // "\\id\\"
extern const char ov00_0221808C[]; // "No matching operation found for id %d\n"
extern const char ov00_022180B4[]; // "\\bm\\"
extern const char ov00_022180BC[]; // "\\ka\\"
extern const char ov00_022180C4[]; // "Received an unrecognized, unsolicited message.\n"
extern const char ov00_022180F4[]; // "The server has closed the connection."
extern const char ov00_0221811C[]; // "(iconnection->connectState == GPI_NOT_CONNECTED) || ... || (iconnection->connectState == GPI_DISCONNECTED)"
extern const char ov00_02218214[]; // "gpi.c"
extern const char ov00_0221821C[]; // "0"
extern const char ov00_02218220[]; // "gpiDeleteBuddy"
extern const char ov00_02218230[]; // "\\authadd\\"
extern const char ov00_0221823C[]; // "\\sesskey\\"
extern const char ov00_02218248[]; // "\\fromprofileid\\"
extern const char ov00_02218258[]; // "\\sig\\"
extern const char ov00_02218260[]; // "\\final\\"
extern const char ov00_02218268[]; // "\\bm\\"
extern const char ov00_02218270[]; // "Unexpected data was received from the server."
extern const char ov00_022182A0[]; // "\\f\\"
extern const char ov00_022182A4[]; // "\\date\\"
extern const char ov00_022182AC[]; // "Out of memory."
extern const char ov00_022182BC[]; // "\\msg\\"
extern const char ov00_022182C4[]; // "|signed|"
extern const char ov00_022182D0[]; // "|s|"
extern const char ov00_022182D4[]; // "|ss|"
extern const char ov00_022182DC[]; // "|ls|"
extern const char ov00_022182E4[]; // "|ip|"
extern const char ov00_022182EC[]; // "|p|"
extern const char ov00_022182F0[]; // "|l|"
extern const char ov00_022182F4[]; // "1"
extern const char ov00_022182F8[]; // "\\t\\"
extern const char ov00_022182FC[]; // "Invalid profile."
extern const char ov00_02218310[]; // "\\delbuddy\\"
extern const char ov00_0221831C[]; // "\\delprofileid\\"
extern const char ov00_0221832C[]; // "index >= 0"
extern const char ov00_02218338[]; // "gpiBuddy.c"
extern const char ov00_02218344[]; // "iconnection->profileList.numBuddies >= 0"

extern long atol(const char *);
extern char *strstr(const char *, const char *);
extern int strncmp(const char *, const char *, size_t);
extern void srand(unsigned int);
extern void __msl_assertion_failed(const char *, const char *, const char *, int);
extern void *ov00_021EC3A8(u32 size);
extern void *ov00_021EC3BC(void *p, int size);
extern void ov00_021EC3D8(void *p);
extern void ov00_021FEE8C(void *table);
extern u32 ov00_021FF368(void);
extern void ov00_021FF3C0(int ms);
extern void ov00_021FF3CC(void);
extern char *ov00_021FF3D4(const char *string);
extern int ov00_021FFA70(int);
extern void ov00_02202990(GpConnection **connection, int result, int fatal);
extern int ov00_02202A84(GpConnection **connection, GpCallback callback, void *arg, GpOperation *operation, int type);

// Argument of the buddy message callback.
typedef struct GpBuddyMessageArg {
    int profile;
    int date;
    char *message;
} GpBuddyMessageArg;

// Argument of the buddy request callback.
typedef struct GpBuddyRequestArg {
    int profile;
    int date;
    char reason[0x401];
} GpBuddyRequestArg;

// Argument of the game invite callback.
typedef struct GpGameInviteArg {
    int profile;
    int productId;
    char location[0x100];
} GpGameInviteArg;

// Sets the "unexpected data" error and disconnects.
#define GP_PARSE_ERROR(connection)                   \
    {                                                \
        ov00_0220A86C(connection, 1, ov00_02218270); \
        ov00_02202990(connection, 3, 1);             \
        return 3;                                    \
    }

#define GP_OUT_OF_MEMORY(connection)              \
    {                                             \
        ov00_0220A890(connection, ov00_022182AC); \
        return 1;                                 \
    }

// Sends a buddy message.
int ov00_02200880(GpConnection **connection, int profile, const char *message) {
    if (!connection || !*connection) {
        return 2;
    }
    if ((*connection)->simulation) {
        return 0;
    }
    if ((*connection)->connectState == GP_DISCONNECTED) {
        ov00_0220A890(connection, ov00_02217EE4);
        return 2;
    }
    if (!message) {
        ov00_0220A890(connection, ov00_02217FEC);
        return 2;
    }
    return ov00_02201BC4(connection, profile, 1, message);
}

// Allocates and initialises a connection ("gpiInitialize").
int ov00_022008F4(GpConnection **connection, int arg1, int arg2) {
    GpConnection *iconnection;
    int i, result;
    *connection = 0;
    iconnection = ov00_021EC3A8(sizeof(GpConnection));
    if (iconnection == 0) {
        return 1;
    }
    memset(iconnection, 0, sizeof(GpConnection));
    iconnection->errorString[0] = 0;
    iconnection->errorCode = 0;
    iconnection->cacheProfileInfo = 1;
    iconnection->unk_104 = 0;
    iconnection->simulation = 0;
    iconnection->firewall = 0;
    iconnection->productId = arg1;
    iconnection->namespaceId = arg2;
    if (ov00_02207B34(&iconnection) == 0) {
        ov00_021EC3D8(iconnection);
        iconnection = 0;
        return 1;
    }
    iconnection->unk_420 = 0;
    for (i = 0; i < GP_CALLBACK_COUNT; i++) {
        iconnection->callbacks[i].callback = 0;
        iconnection->callbacks[i].param = 0;
    }
    iconnection->unk_460 = 0;
    ov00_0220A3BC(&iconnection, ov00_0221800C);
    result = ov00_02200A98(&iconnection);
    if (result) {
        ov00_02200A3C(&iconnection);
        return result;
    }
    ov00_021FF3CC();
    srand(ov00_021FF368());
    *connection = iconnection;
    return 0;
}

void ov00_02200A3C(GpConnection **connection) {
    GpConnection *iconnection = *connection;
    ov00_02204284(connection, 1);
    ov00_021EC3D8(iconnection->unk_460);
    iconnection->unk_460 = 0;
    ov00_021FEE8C(iconnection->profileTable);
    ov00_021EC3D8(iconnection);
    *connection = 0;
}

// Profile table iterator: forgets the buddy data.
int ov00_02200A7C(GpConnection **connection, GpProfile *profile, void *data) {
    profile->buddyStatus = 0;
    profile->authSig = 0;
    profile->requestCount = 0;
    profile->peerSig = 0;
    return 1;
}

// Resets the connection state (also after a disconnect).
int ov00_02200A98(GpConnection **connection) {
    GpConnection *iconnection = *connection;
    iconnection->nick[0] = 0;
    iconnection->uniquenick[0] = 0;
    iconnection->email[0] = 0;
    iconnection->sock = -1;
    iconnection->connectState = GP_NOT_CONNECTED;
    iconnection->inputBuffer.len = 0;
    iconnection->inputBuffer.pos = 0;
    iconnection->inputBuffer.size = 0;
    ov00_021EC3D8(iconnection->inputBuffer.buffer);
    iconnection->inputBuffer.buffer = 0;
    iconnection->lineBufferSize = 0;
    ov00_021EC3D8(iconnection->lineBuffer);
    iconnection->lineBuffer = 0;
    iconnection->outputBuffer.len = 0;
    iconnection->outputBuffer.pos = 0;
    iconnection->outputBuffer.size = 0;
    ov00_021EC3D8(iconnection->outputBuffer.buffer);
    iconnection->outputBuffer.buffer = 0;
    iconnection->updateproBuffer.len = 0;
    iconnection->updateproBuffer.pos = 0;
    iconnection->updateproBuffer.size = 0;
    ov00_021EC3D8(iconnection->updateproBuffer.buffer);
    iconnection->updateproBuffer.buffer = 0;
    iconnection->updateuiBuffer.len = 0;
    iconnection->updateuiBuffer.pos = 0;
    iconnection->updateuiBuffer.size = 0;
    ov00_021EC3D8(iconnection->updateuiBuffer.buffer);
    iconnection->updateuiBuffer.buffer = 0;
    iconnection->peerSocket = -1;
    iconnection->nextOperationId = 2;
    while (iconnection->operations) {
        ov00_022066D0(connection, iconnection->operations);
    }
    iconnection->operations = 0;
    iconnection->numBuddies = 0;
    ov00_02207F1C(connection, ov00_02200A7C, 0);
    iconnection->userId = 0;
    iconnection->profileId = 0;
    iconnection->sessKey = 0;
    iconnection->numSearches = 0;
    iconnection->fatalError = 0;
    iconnection->peers = 0;
    iconnection->lastStatus = -1;
    iconnection->lastStatusString[0] = 0;
    iconnection->lastLocationString[0] = 0;
    return 0;
}

// Sends what is buffered for the server and handles every complete "...\final\" message received.
int ov00_02200BC0(GpConnection **connection) {
    GpOperation *operation;
    int len;
    int closed = 0;
    int result, more;
    GpConnection *iconnection = *connection;
    char *end, *line;
    do {
        ov00_02205064(connection, &iconnection->outputBuffer);
        result = ov00_0220267C(connection, iconnection->sock, &iconnection->outputBuffer, &closed, 1, ov00_02218030);
        if (result) {
            return result;
        }
        result = ov00_02202454(connection, iconnection->sock, &iconnection->inputBuffer, &len, &closed, ov00_02218030);
        if (result) {
            if (result != 3) {
                return result;
            }
            ov00_0220A86C(connection, 5, ov00_02218034);
            ov00_02202990(connection, 3, 1);
            return 3;
        }
        end = strstr(iconnection->inputBuffer.buffer, ov00_02218060);
        while (end) {
            *end = 0;
            ov00_0220A3BC(connection, ov00_02218068, iconnection->inputBuffer.buffer);
            len = end - iconnection->inputBuffer.buffer;
            if (len > iconnection->lineBufferSize) {
                iconnection->lineBufferSize += len < 2048 ? 2048 : len;
                line = ov00_021EC3BC(iconnection->lineBuffer, iconnection->lineBufferSize + 1);
                if (!line) {
                    ov00_0220A890(connection, ov00_02218074);
                    return 1;
                }
                iconnection->lineBuffer = line;
            }
            memcpy(iconnection->lineBuffer, iconnection->inputBuffer.buffer, len + 1);
            iconnection->inputBuffer.len -= end + 7 - iconnection->inputBuffer.buffer;
            memmove(iconnection->inputBuffer.buffer, end + 7, iconnection->inputBuffer.len + 1);
            line = iconnection->lineBuffer;
            end = strstr(line, ov00_02218084);
            if (end) {
                int id = atol(end + 4);
                if (!ov00_02206720(connection, &operation, id)) {
                    ov00_0220A3BC(connection, ov00_0221808C, id);
                } else {
                    result = ov00_022067A8(connection, operation, iconnection->lineBuffer);
                    if (result) {
                        return result;
                    }
                }
            } else {
                if (ov00_0220A3C8(connection, line, 1)) {
                    return 4;
                }
                line = iconnection->lineBuffer;
                if (!strncmp(line, ov00_022180B4, 4)) {
                    result = ov00_02201134(connection, line);
                    if (result) {
                        return result;
                    }
                } else if (strncmp(line, ov00_022180BC, 10)) {
                    ov00_0220A3BC(connection, ov00_022180C4);
                }
            }
            end = strstr(iconnection->inputBuffer.buffer, ov00_02218060);
        }
        if (closed) {
            ov00_0220A86C(connection, 7, ov00_022180F4);
            ov00_02202990(connection, 3, 1);
            return 0;
        }
        more = ov00_0220676C(connection);
        if (more) {
            ov00_021FF3C0(10);
        }
    } while (more);
    return 0;
}

// "gpiProcess": runs the connection; with blocking set, waits until the connect attempt is over.
int ov00_02200ED8(GpConnection **connection, int blocking) {
    GpConnection *iconnection = *connection;
    int result = 0;
    GpOperation *operation;
    if ((u32)iconnection->connectState > GP_DISCONNECTED) {
        __msl_assertion_failed(ov00_0221811C, ov00_02218214, ov00_02218000, 0x1b6);
    }
    if (iconnection->connectState == GP_CONNECTING) {
        int again;
        do {
            result = ov00_0220411C(connection);
            if (result == 0 && blocking && iconnection->connectState == GP_CONNECTING) {
                again = 1;
            } else {
                again = 0;
            }
            if (again) {
                ov00_021FF3C0(10);
            }
        } while (again);
        if (result) {
            if (ov00_02206720(connection, &operation, 1)) {
                operation->result = 4;
            } else {
                __msl_assertion_failed(ov00_0221821C, ov00_02218214, ov00_02218000, 0x1dc);
            }
        }
    }
    if (iconnection->connectState == GP_NEGOTIATING || iconnection->connectState == GP_CONNECTED) {
        if (!result) {
            result = ov00_02200BC0(connection);
        }
        if (!result) {
            result = ov00_022073E8(connection);
        }
    }
    if (!result) {
        result = ov00_02209FCC(connection);
    }
    operation = iconnection->operations;
    while (operation) {
        if (operation->result == 0) {
            operation = operation->next;
        } else {
            ov00_02206230(connection, operation);
            {
                GpOperation *done = operation;
                operation = operation->next;
                ov00_022066D0(connection, done);
            }
        }
    }
    {
        int status = ov00_02202D20(connection, blocking);
        if (status) {
            return status;
        }
    }
    if (iconnection->fatalError) {
        ov00_02204284(connection, 0);
    }
    return result;
}

// Accepts the buddy request of profile ("\authadd\").
int ov00_0220108C(GpConnection **connection, GpProfile *profile) {
    GpConnection *iconnection = *connection;
    ov00_022020EC(connection, &iconnection->outputBuffer, ov00_02218230);
    ov00_022020EC(connection, &iconnection->outputBuffer, ov00_0221823C);
    ov00_0220211C(connection, &iconnection->outputBuffer, iconnection->sessKey);
    ov00_022020EC(connection, &iconnection->outputBuffer, ov00_02218248);
    ov00_0220211C(connection, &iconnection->outputBuffer, profile->profileId);
    ov00_022020EC(connection, &iconnection->outputBuffer, ov00_02218258);
    ov00_022020EC(connection, &iconnection->outputBuffer, profile->authSig);
    ov00_022020EC(connection, &iconnection->outputBuffer, ov00_02218260);
    return 0;
}

// Handles a "\bm\" message from the server: 1 buddy message, 2 buddy request, 100 buddy status,
// 101 game invite, 102 ping (answered).
int ov00_02201134(GpConnection **connection, char *input) {
    char buffer[4096], string[256], number[16];
    int profileId, type, date, result;
    GpCallback messageCallback, inviteCallback, statusCallback, requestCallback;
    GpProfile *profile;
    GpBuddyStatus *status;
    GpBuddyMessageArg *arg;
    char *signature;
    GpConnection *iconnection = *connection;
    if (!ov00_0220A4A0(input, ov00_02218268, buffer, 4096)) {
        GP_PARSE_ERROR(connection);
    }
    type = atol(buffer);
    if (!ov00_0220A4A0(input, ov00_022182A0, buffer, 4096)) {
        GP_PARSE_ERROR(connection);
    }
    profileId = atol(buffer);
    if (ov00_0220A4A0(input, ov00_022182A4, buffer, 4096)) {
        date = atol(buffer);
    } else {
        date = ov00_021FFA70(0);
    }
    switch (type) {
    case 1: {
        messageCallback = iconnection->callbacks[GP_CALLBACK_BUDDY_MESSAGE];
        if (!messageCallback.callback) {
            break;
        }
        arg = ov00_021EC3A8(sizeof(GpBuddyMessageArg));
        if (!arg) {
            GP_OUT_OF_MEMORY(connection);
        }
        if (!ov00_0220A4A0(input, ov00_022182BC, buffer, 4096)) {
            GP_PARSE_ERROR(connection);
        }
        arg->message = ov00_021EC3A8(strlen(buffer) + 1);
        if (!arg->message) {
            GP_OUT_OF_MEMORY(connection);
        }
        strcpy(arg->message, buffer);
        arg->profile = profileId;
        arg->date = date;
        result = ov00_02202A84(connection, messageCallback, arg, 0, 2);
        if (result) {
            return result;
        }
        break;
    }
    case 2: {
        GpBuddyRequestArg *request;
        profile = ov00_02207CEC(connection, profileId);
        if (!profile) {
            GP_OUT_OF_MEMORY(connection);
        }
        if (!ov00_0220A4A0(input, ov00_022182BC, buffer, 4096)) {
            GP_PARSE_ERROR(connection);
        }
        signature = strstr(buffer, ov00_022182C4);
        if (!signature) {
            GP_PARSE_ERROR(connection);
        }
        *signature = 0;
        if (strlen(signature + 8) != 32) {
            GP_PARSE_ERROR(connection);
        }
        ov00_021EC3D8(profile->authSig);
        profile->authSig = 0;
        profile->authSig = ov00_021FF3D4(signature + 8);
        profile->requestCount++;
        requestCallback = iconnection->callbacks[GP_CALLBACK_BUDDY_REQUEST];
        if (!requestCallback.callback) {
            break;
        }
        request = ov00_021EC3A8(sizeof(GpBuddyRequestArg));
        if (!request) {
            GP_OUT_OF_MEMORY(connection);
        }
        ov00_0220A348(request->reason, buffer, sizeof(request->reason));
        request->profile = profileId;
        request->date = date;
        result = ov00_02202A84(connection, requestCallback, request, 0, 6);
        if (result) {
            return result;
        }
        break;
    }
    case 100: {
        profile = ov00_02207CEC(connection, profileId);
        if (!profile) {
            GP_OUT_OF_MEMORY(connection);
        }
        if (!profile->buddyStatus) {
            profile->buddyStatus = ov00_021EC3A8(sizeof(GpBuddyStatus));
            if (!profile->buddyStatus) {
                GP_OUT_OF_MEMORY(connection);
            }
            memset(profile->buddyStatus, 0, sizeof(GpBuddyStatus));
            profile->buddyStatus->buddyIndex = iconnection->numBuddies++;
        }
        status = profile->buddyStatus;
        if (!ov00_0220A4A0(input, ov00_022182BC, buffer, 4096)) {
            GP_PARSE_ERROR(connection);
        }
        if (!ov00_0220A4A0(buffer, ov00_022182D0, number, 16)) {
            GP_PARSE_ERROR(connection);
        }
        status->status = atol(number);
        ov00_021EC3D8(status->statusString);
        status->statusString = 0;
        if (!ov00_0220A4A0(buffer, ov00_022182D4, string, 256)) {
            string[0] = 0;
        }
        status->statusString = ov00_021FF3D4(string);
        if (!status->statusString) {
            GP_OUT_OF_MEMORY(connection);
        }
        ov00_021EC3D8(status->locationString);
        status->locationString = 0;
        if (!ov00_0220A4A0(buffer, ov00_022182DC, string, 256)) {
            string[0] = 0;
        }
        status->locationString = ov00_021FF3D4(string);
        if (!status->locationString) {
            GP_OUT_OF_MEMORY(connection);
        }
        if (ov00_0220A4A0(buffer, ov00_022182E4, number, 16)) {
            u32 a = atol(number), b = atol(number), c = atol(number), d = atol(number);
            status->ip = ((c >> 24) & 255) | ((d >> 8) & 0xff00) | ((b << 8) & 0xff0000) | ((a << 24) & 0xff000000);
        } else {
            status->ip = 0;
        }
        if (ov00_0220A4A0(buffer, ov00_022182EC, number, 16)) {
            u16 port = atol(number);
            status->port = (u16)(((port >> 8) & 255) | ((port << 8) & 0xff00));
        } else {
            status->port = 0;
        }
        statusCallback = iconnection->callbacks[GP_CALLBACK_BUDDY_STATUS];
        if (!statusCallback.callback) {
            break;
        }
        arg = ov00_021EC3A8(12);
        if (!arg) {
            GP_OUT_OF_MEMORY(connection);
        }
        arg->profile = profileId;
        arg->message = (char *)status->buddyIndex;
        arg->date = date;
        result = ov00_02202A84(connection, statusCallback, arg, 0, 5);
        if (result) {
            return result;
        }
        break;
    }
    case 101: {
        GpGameInviteArg *invite;
        int productId;
        if (!ov00_0220A4A0(input, ov00_022182BC, buffer, 4096)) {
            GP_PARSE_ERROR(connection);
        }
        signature = strstr(buffer, ov00_022182EC);
        if (!signature) {
            GP_PARSE_ERROR(connection);
        }
        if (!signature[3]) {
            GP_PARSE_ERROR(connection);
        }
        productId = atol(signature + 3);
        signature = strstr(buffer, ov00_022182F0);
        if (!signature) {
            string[0] = 0;
        } else {
            ov00_0220A348(string, signature + 3, 256);
        }
        inviteCallback = iconnection->callbacks[GP_CALLBACK_GAME_INVITE];
        if (!inviteCallback.callback) {
            break;
        }
        invite = ov00_021EC3A8(sizeof(GpGameInviteArg));
        if (!invite) {
            GP_OUT_OF_MEMORY(connection);
        }
        invite->profile = profileId;
        invite->productId = productId;
        strcpy(invite->location, string);
        result = ov00_02202A84(connection, inviteCallback, invite, 0, 0);
        if (result) {
            return result;
        }
        break;
    }
    case 102:
        if (!ov00_0220A4A0(input, ov00_022182BC, buffer, 4096)) {
            GP_PARSE_ERROR(connection);
        }
        ov00_02201BC4(connection, profileId, 0x67, ov00_022182F4);
        break;
    default:
        break;
    }
    return 0;
}

// Sends a "\bm\" message through the server.
int ov00_02201AE8(GpConnection **connection, int profile, int type, const char *message) {
    char buffer[3504];
    GpConnection *iconnection = *connection;
    ov00_0220A348(buffer, message, 3501);
    ov00_022020EC(connection, &iconnection->outputBuffer, ov00_02218268);
    ov00_0220211C(connection, &iconnection->outputBuffer, type);
    ov00_022020EC(connection, &iconnection->outputBuffer, ov00_0221823C);
    ov00_0220211C(connection, &iconnection->outputBuffer, iconnection->sessKey);
    ov00_022020EC(connection, &iconnection->outputBuffer, ov00_022182F8);
    ov00_0220211C(connection, &iconnection->outputBuffer, profile);
    ov00_022020EC(connection, &iconnection->outputBuffer, ov00_022182BC);
    ov00_022020EC(connection, &iconnection->outputBuffer, buffer);
    ov00_022020EC(connection, &iconnection->outputBuffer, ov00_02218260);
    return 0;
}

// Sends a buddy message directly to the peer when one is (or can be) connected, else through the server.
int ov00_02201BC4(GpConnection **connection, int profileId, int type, const char *message) {
    GpProfile *profile;
    GpPeer *peer = ov00_022074D8(connection, profileId);
    int result;
    if (!peer) {
        if (!ov00_02207DD0(connection, profileId, &profile) || !profile->buddyStatus || !profile->buddyStatus->port) {
            return ov00_02201AE8(connection, profileId, type, message);
        }
        peer = ov00_0220752C(connection, profileId, 1);
        if (!peer) {
            return 1;
        }
        if (!profile->peerSig) {
            result = ov00_022075AC(connection, peer);
            if (result) {
                return result;
            }
        } else {
            result = ov00_02207610(connection, peer);
            if (result) {
                return result;
            }
        }
    }
    result = ov00_0220779C(connection, peer, type, message);
    if (!result) {
        return 0;
    }
    return result;
}

// Accepts a pending buddy request.
int ov00_02201CAC(GpConnection **connection, int profileId) {
    GpConnection *iconnection = *connection;
    GpProfile *profile;
    int result;
    if (!ov00_02207DD0(connection, profileId, &profile)) {
        ov00_0220A890(connection, ov00_022182FC);
        return 2;
    }
    if (!profile->authSig) {
        ov00_0220A890(connection, ov00_022182FC);
        return 2;
    }
    result = ov00_0220108C(connection, profile);
    if (result) {
        return result;
    }
    profile->requestCount--;
    if (!iconnection->cacheProfileInfo && profile->requestCount <= 0) {
        ov00_021EC3D8(profile->authSig);
        profile->authSig = 0;
        if (ov00_02207FB8(profile)) {
            ov00_02207E40(connection, profile);
        }
    }
    return 0;
}

// Profile table iterator: moves the buddies after a deleted one down by one index.
int ov00_02201D78(GpConnection **connection, GpProfile *profile, void *index) {
    GpBuddyStatus *status = profile->buddyStatus;
    if (status) {
        if (status->buddyIndex > (int)index) {
            status->buddyIndex--;
        }
    }
    return 1;
}

// "gpiDeleteBuddy"
int ov00_02201D9C(GpConnection **connection, int profileId) {
    GpConnection *iconnection = *connection;
    GpProfile *profile;
    if (!ov00_02207DD0(connection, profileId, &profile)) {
        ov00_0220A890(connection, ov00_022182FC);
        return 2;
    }
    ov00_022020EC(connection, &iconnection->outputBuffer, ov00_02218310);
    ov00_022020EC(connection, &iconnection->outputBuffer, ov00_0221823C);
    ov00_0220211C(connection, &iconnection->outputBuffer, iconnection->sessKey);
    ov00_022020EC(connection, &iconnection->outputBuffer, ov00_0221831C);
    ov00_0220211C(connection, &iconnection->outputBuffer, profile->profileId);
    ov00_022020EC(connection, &iconnection->outputBuffer, ov00_02218260);
    if (profile->buddyStatus) {
        int index = profile->buddyStatus->buddyIndex;
        if (index < 0) {
            __msl_assertion_failed(ov00_0221832C, ov00_02218338, ov00_02218220, 0x1fd);
        }
        ov00_021EC3D8(profile->buddyStatus->statusString);
        profile->buddyStatus->statusString = 0;
        ov00_021EC3D8(profile->buddyStatus->locationString);
        profile->buddyStatus->locationString = 0;
        ov00_021EC3D8(profile->buddyStatus);
        profile->buddyStatus = 0;
        if (ov00_02207FB8(profile)) {
            ov00_02207E40(connection, profile);
        }
        if (--iconnection->numBuddies < 0) {
            __msl_assertion_failed(ov00_02218344, ov00_02218338, ov00_02218220, 0x204);
        }
        ov00_02207F1C(connection, ov00_02201D78, (void *)index);
    }
    return 0;
}
