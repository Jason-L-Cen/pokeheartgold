#pragma force_active on
#include <string.h>

#include "global.h"

#include "dwc/gp_internal.h"

// The code below is compiled as ARM (32-bit) code.
#include <nitro/code32.h>

// Presence connection: delivery of the queued callbacks, connecting and logging in to the connection
// manager ("gpiConnect.c": sockets, "\login\" / "\newuser\" and the "\lc\" replies, disconnect) and the
// start of "gpiInfo.c" (dates, the "\pi\" profile info reply).

extern const char ov00_0221868C[]; // "gpiStartConnect"
extern const char ov00_0221869C[]; // "gpiCheckConnect"
extern const char ov00_022186AC[]; // "gpcm.gs.nintendowifi.net"
extern const char ov00_022186EC[]; // "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"
extern const char ov00_0221872C[]; // "There was an error creating a socket."
extern const char ov00_02218754[]; // "There was an error making a socket non-blocking."
extern const char ov00_02218788[]; // "There was an error binding a socket."
extern const char ov00_022187B0[]; // "There was an error listening on a socket."
extern const char ov00_022187DC[]; // "There was an error getting a socket's addres."
extern const char ov00_0221880C[]; // "Could not resolve connection mananger host name."
extern const char ov00_02218840[]; // "address.sin_addr.s_addr != 0"
extern const char ov00_02218860[]; // "gpiConnect.c"
extern const char ov00_02218870[]; // "There was an error connecting a socket."
extern const char ov00_02218898[]; // "Invalid connection."
extern const char ov00_022188AC[]; // "Invalid firewall."
extern const char ov00_022188C0[]; // "Out of memory."
extern const char ov00_022188D0[]; // "%s@%s"
extern const char ov00_022188D8[]; // "%s%s%s%s%s%s"
extern const char ov00_022188E8[]; // 48 spaces
extern const char ov00_0221891C[]; // "\\login\\"
extern const char ov00_02218924[]; // "\\challenge\\"
extern const char ov00_02218930[]; // "\\authtoken\\"
extern const char ov00_0221893C[]; // "\\uniquenick\\"
extern const char ov00_0221894C[]; // "\\user\\"
extern const char ov00_02218954[]; // "@"
extern const char ov00_02218958[]; // "\\userid\\"
extern const char ov00_02218964[]; // "\\profileid\\"
extern const char ov00_02218970[]; // "\\response\\"
extern const char ov00_0221897C[]; // "\\firewall\\1"
extern const char ov00_02218988[]; // "\\port\\"
extern const char ov00_02218990[]; // "\\productid\\"
extern const char ov00_0221899C[]; // "\\gamename\\"
extern const char ov00_022189A8[]; // "\\namespaceid\\"
extern const char ov00_022189B8[]; // "\\id\\1"
extern const char ov00_022189C0[]; // "\\final\\"
extern const char ov00_022189C8[]; // "\\newuser\\"
extern const char ov00_022189D4[]; // "\\email\\"
extern const char ov00_022189DC[]; // "\\nick\\"
extern const char ov00_022189E4[]; // "\\passwordenc\\"
extern const char ov00_022189F4[]; // "\\cdkeyenc\\"
extern const char ov00_02218A00[]; // "\\pid\\"
extern const char ov00_02218A08[]; // "\\fatal\\"
extern const char ov00_02218A10[]; // "\\lc\\1"
extern const char ov00_02218A18[]; // "Unexpected data was received from the server."
extern const char ov00_02218A48[]; // "\\nur\\"
extern const char ov00_02218A50[]; // "Unexepected data was received from the server."
extern const char ov00_02218A80[]; // "\\lc\\2"
extern const char ov00_02218A88[]; // "\\sesskey\\"
extern const char ov00_02218A94[]; // "\\lt\\"
extern const char ov00_02218A9C[]; // "\\proof\\"
extern const char ov00_02218AA4[]; // "Could not authenticate server."
extern const char ov00_02218AC4[]; // "The server has refused the connection."
extern const char ov00_02218AEC[]; // "state == GPI_CONNECTED"
extern const char ov00_02218B04[]; // "\\logout\\\\sesskey\\"
extern const char ov00_02218B18[]; // "CM"
extern const char ov00_02218B1C[]; // "gpiIntToDate"
extern const char ov00_02218B2C[]; // "gpiProcessGetInfo"
extern const char ov00_02218B40[]; // "gpiInfo.c"
extern const char ov00_02218B4C[]; // "Invalid date."
extern const char ov00_02218B5C[]; // "gpiIsValidDate(d, m, y)"
extern const char ov00_02218B74[]; // "\\pi\\"
extern const char ov00_02218B7C[]; // "Unexpected data was received from the server."
extern const char ov00_02218BAC[]; // "\\profileid\\"
extern const char ov00_02218BB8[]; // "profileid > 0"
extern const char ov00_02218BC8[]; // "\\nick\\"
extern const char ov00_02218BD0[]; // "\\uniquenick\\"
extern const char ov00_02218BE0[]; // "\\email\\"
extern const char ov00_02218BE8[]; // "\\firstname\\"
extern const char ov00_02218BF4[]; // "\\lastname\\"
extern const char ov00_02218C00[]; // "\\icquin\\"
extern const char ov00_02218C0C[]; // "\\homepage\\"
extern const char ov00_02218C18[]; // "\\zipcode\\"
extern const char ov00_02218C24[]; // "\\countrycode\\"
extern const char ov00_02218C34[]; // "\\lon\\"
extern const char ov00_02218C3C[]; // "\\lat\\"
extern const char ov00_02218C44[]; // "\\loc\\"
extern const char ov00_02218C4C[]; // "\\birthday\\"
extern const char ov00_02218C58[]; // "\\sex\\"
extern const char ov00_02218C60[]; // "\\pmask\\"
extern const char ov00_02218C68[]; // "\\aim\\"
extern const char ov00_02218C70[]; // "\\pic\\"
extern const char ov00_02218C78[]; // "\\occ\\"
extern const char ov00_02218C80[]; // "\\ind\\"
extern const char ov00_02218C88[]; // "\\inc\\"
extern const char ov00_02218C90[]; // "\\mar\\"
extern const char ov00_02218C98[]; // "\\chc\\"
extern const char ov00_02218CA0[]; // "\\i1\\"
extern const char ov00_02218CA8[]; // "\\o1\\"
extern const char ov00_02218CB0[]; // "\\conn\\"
extern const char ov00_02218CB8[]; // "\\sig\\"
extern const char ov00_02218CC0[]; // "Out of memory."
extern char ov00_0221B16C[];       // game name

typedef struct HostEnt {
    char *name;
    char **aliases;
    int addrType;
    u32 **addrList;
} HostEnt;

typedef union SocketAddress {
    struct {
        u8 len;
        u8 family;
        u16 port;
        u32 addr;
    } in;

    u32 words[2];
} SocketAddress;

extern int sprintf(char *, const char *, ...);
extern long atol(const char *);
extern double atod(const char *);
extern char *strstr(const char *, const char *);
extern int strncmp(const char *, const char *, unsigned long);
extern int rand(void);
extern void __msl_assertion_failed(const char *, const char *, const char *, int);
extern HostEnt *SOC_GetHostByName(const char *name);
extern void *DWCi_GsMalloc(int size);
extern void DWCi_GsFree(void *p);
extern void MD5Digest(const char *data, int len, char *md5);
extern void _strlwr(char *string);
extern char *goastrdup(const char *string);
extern int SetSockBlocking(int sock, int blocking);
extern int gsSocketCreate(int domain, int type, int protocol);
extern void gsSocketClose(int sock);
extern void gsSocketShutdown(int sock, int how);
extern int gsSocketBind(int sock, SocketAddress *address, int len);
extern int gsSocketConnect(int sock, SocketAddress *address, int len);
extern int gsSocketListen(int sock, int backlog);
extern int gsSocketGetSockName(int sock, SocketAddress *address, int *len);
extern int gsSocketGetLastError(int sock);
extern void Util_RandSeed(u32 seed);
extern int Util_RandInt(int min, int max);
extern void B64Encode(const char *src, char *dst, u32 len, int encoding);
extern void gpiCallErrorCallback(GpConnection **connection, int result, int fatal);
extern int gpiAddCallback(GpConnection **connection, GpCallback callback, void *arg, GpOperation *operation, int type);

// Data of the connect operation.
typedef struct GpConnectData {
    char serverChallenge[128];  // 0x000
    char userChallenge[33];     // 0x080
    char response[33];          // 0x0A1
    char authtoken[256];        // 0x0C2
    char partnerChallenge[256]; // 0x1C2
    char cdkey[65];             // 0x2C2
    int newUser;                // 0x304
} GpConnectData;

int sendLogin(GpConnection **connection, GpConnectData *data);
int sendNewuser(GpConnection **connection, GpConnectData *data);

#define GP_ERROR(connection, code, message)       \
    {                                             \
        gpiSetError(connection, code, message); \
        gpiCallErrorCallback(connection, 3, 1);          \
        return 3;                                 \
    }

// Calls the queued callbacks: with operationId set only that operation's (and the error callbacks),
// otherwise all of them.
int gpiProcessCallbacks(GpConnection **connection, int operationId) {
    GpConnection *iconnection = *connection;
    GpCallbackData *head, *tail, *data;
    if (operationId) {
        GpCallbackData *prev;
        head = iconnection->callbacksHead;
        tail = iconnection->callbacksTail;
        prev = 0;
        iconnection->callbacksHead = 0;
        iconnection->callbacksTail = 0;
        data = head;
        while (data) {
            GpCallbackData *next = data->next;
            if (data->operationId == operationId || data->type == 1) {
                if (prev) {
                    prev->next = next;
                } else {
                    head = next;
                }
                if (tail == data) {
                    tail = prev;
                }
                gpiCallCallback(connection, data);
            } else {
                prev = data;
            }
            data = next;
        }
        if (iconnection->callbacksHead) {
            iconnection->callbacksTail->next = head;
        } else {
            iconnection->callbacksHead = head;
        }
        iconnection->callbacksTail = tail;
        return 0;
    }
    while ((data = iconnection->callbacksHead) != 0) {
        iconnection->callbacksHead = 0;
        iconnection->callbacksTail = 0;
        while (data) {
            GpCallbackData *next = data->next;
            gpiCallCallback(connection, data);
            data = next;
        }
    }
    return 0;
}

// Random alphanumeric string of count characters.
void randomString(char *out, int count) {
    int i;
    for (i = 0; i < count; i++) {
        out[i] = ov00_022186EC[(u32)rand() % 62];
    }
    out[i] = 0;
}

// "gpiStartConnect": opens the peer listening socket (unless behind a firewall) and starts connecting to
// the connection manager.
int gpiStartConnect(GpConnection **connection, GpOperation *operation) {
    GpConnection *iconnection = *connection;
    SocketAddress address;
    int len;
    if (iconnection->firewall == 0) {
        iconnection->peerSocket = gsSocketCreate(2, 1, 0);
        if (iconnection->peerSocket == -1) {
            GP_ERROR(connection, 5, ov00_0221872C);
        }
        if (!SetSockBlocking(iconnection->peerSocket, 0)) {
            GP_ERROR(connection, 5, ov00_02218754);
        }
        {
            u32 *p = (u32 *)&address;
            p[0] = 0;
            p[1] = 0;
            address.in.family = 2;
            if (gsSocketBind(iconnection->peerSocket, &address, 8) == -1) {
                GP_ERROR(connection, 5, ov00_02218788);
            }
            if (gsSocketListen(iconnection->peerSocket, 5) == -1) {
                GP_ERROR(connection, 5, ov00_022187B0);
            }
            len = 8;
            if (gsSocketGetSockName(iconnection->peerSocket, &address, &len) == -1) {
                GP_ERROR(connection, 5, ov00_022187DC);
            }
            iconnection->peerPort = address.in.port;
        }
    } else {
        iconnection->peerSocket = -1;
        iconnection->peerPort = 0;
    }
    iconnection->sock = gsSocketCreate(2, 1, 0);
    if (iconnection->sock == -1) {
        GP_ERROR(connection, 5, ov00_0221872C);
    }
    if (!SetSockBlocking(iconnection->sock, 0)) {
        GP_ERROR(connection, 5, ov00_02218754);
    }
    {
        HostEnt *host = SOC_GetHostByName(ov00_022186AC);
        if (!host) {
            GP_ERROR(connection, 5, ov00_0221880C);
        }
        {
            u32 *p = (u32 *)&address;
            p[0] = p[1] = 0;
            address.in.family = 2;
            address.in.addr = **host->addrList;
            if (!address.in.addr) {
                __msl_assertion_failed(ov00_02218840, ov00_02218860, ov00_0221868C, 0x90);
            }
            address.in.port = 0xcc74; // 29900, big-endian
            if (gsSocketConnect(iconnection->sock, &address, 8) == -1) {
                int error = gsSocketGetLastError(iconnection->sock);
                if (error != -6 && error != -26 && error != -76) {
                    GP_ERROR(connection, 5, ov00_02218870);
                }
            }
        }
    }
    operation->state = 1;
    iconnection->connectState = GP_CONNECTING;
    return 0;
}

// Connects and logs in (or creates the user with newUser set).
int gpiConnect(GpConnection **connection, char *nick, char *uniquenick, char *email, char *password, char *authtoken, char *partnerChallenge, char *cdkey, int firewall, int newUser, int blocking, void *callback, void *param) {
    GpConnection *iconnection = *connection;
    GpConnectData *data;
    GpOperation *operation;
    int result;
    if (iconnection->connectState == GP_DISCONNECTED) {
        result = gpiReset(connection);
        if (result) {
            return result;
        }
    }
    if (iconnection->connectState) {
        gpiSetErrorString(connection, ov00_02218898);
        return 2;
    }
    switch (firewall) {
    case 1:
        iconnection->firewall = 1;
        break;
    case 0:
        iconnection->firewall = 0;
        break;
    default:
        gpiSetErrorString(connection, ov00_022188AC);
        return 2;
    }
    iconnection->firewall = 1;
    strzcpy(iconnection->nick, nick, 31);
    strzcpy(iconnection->uniquenick, uniquenick, 21);
    strzcpy(iconnection->email, email, 51);
    strzcpy(iconnection->password, password, 31);
    _strlwr(iconnection->email);
    data = DWCi_GsMalloc(sizeof(GpConnectData));
    if (!data) {
        gpiSetErrorString(connection, ov00_022188C0);
        return 1;
    }
    memset(data, 0, sizeof(GpConnectData));
    data->newUser = newUser;
    if (*authtoken && *partnerChallenge) {
        strzcpy(data->authtoken, authtoken, 256);
        strzcpy(data->partnerChallenge, partnerChallenge, 256);
    }
    if (cdkey) {
        strzcpy(data->cdkey, cdkey, 65);
    }
    result = gpiAddOperation(connection, GP_OPERATION_CONNECT, data, &operation, blocking, callback, param);
    if (result) {
        return result;
    }
    result = gpiStartConnect(connection, operation);
    if (result) {
        operation->result = result;
        gpiFailedOpCallback(connection, operation);
        gpiDisconnect(connection, 0);
        return result;
    }
    if (operation->blocking) {
        result = gpiProcess(connection, operation->id);
        if (result) {
            return result;
        }
    }
    return 0;
}

// Sends "\login\" with the challenge response.
int sendLogin(GpConnection **connection, GpConnectData *data) {
    GpConnection *iconnection = *connection;
    char *password;
    char *user;
    GpProfile *profile;

    struct {
        char hash[33], user[82], text[512];
    } tmp;

    randomString(data->userChallenge, 32);
    password = *data->partnerChallenge ? data->partnerChallenge : iconnection->password;
    MD5Digest(password, strlen(password), data->response);
    if (*data->authtoken) {
        user = data->authtoken;
    } else if (*iconnection->uniquenick) {
        user = iconnection->uniquenick;
    } else {
        sprintf(tmp.user, ov00_022188D0, iconnection->nick, iconnection->email);
        user = tmp.user;
    }
    sprintf(tmp.text, ov00_022188D8, data->response, ov00_022188E8, user, data->userChallenge, data->serverChallenge, data->response);
    MD5Digest(tmp.text, strlen(tmp.text), tmp.hash);
    if (iconnection->cacheProfileInfo) {
        gpiFindProfileByUser(connection, iconnection->nick, iconnection->email, &profile);
        if (profile) {
            iconnection->userId = profile->userId;
            iconnection->profileId = profile->profileId;
        }
    }
    gpiAppendStringToBuffer(connection, &iconnection->outputBuffer, ov00_0221891C);
    gpiAppendStringToBuffer(connection, &iconnection->outputBuffer, ov00_02218924);
    gpiAppendStringToBuffer(connection, &iconnection->outputBuffer, data->userChallenge);
    if (*data->authtoken) {
        gpiAppendStringToBuffer(connection, &iconnection->outputBuffer, ov00_02218930);
        gpiAppendStringToBuffer(connection, &iconnection->outputBuffer, data->authtoken);
    } else if (*iconnection->uniquenick) {
        gpiAppendStringToBuffer(connection, &iconnection->outputBuffer, ov00_0221893C);
        gpiAppendStringToBuffer(connection, &iconnection->outputBuffer, iconnection->uniquenick);
    } else {
        gpiAppendStringToBuffer(connection, &iconnection->outputBuffer, ov00_0221894C);
        gpiAppendStringToBuffer(connection, &iconnection->outputBuffer, iconnection->nick);
        gpiAppendStringToBuffer(connection, &iconnection->outputBuffer, ov00_02218954);
        gpiAppendStringToBuffer(connection, &iconnection->outputBuffer, iconnection->email);
    }
    if (iconnection->userId) {
        gpiAppendStringToBuffer(connection, &iconnection->outputBuffer, ov00_02218958);
        gpiAppendIntToBuffer(connection, &iconnection->outputBuffer, iconnection->userId);
    }
    if (iconnection->profileId) {
        gpiAppendStringToBuffer(connection, &iconnection->outputBuffer, ov00_02218964);
        gpiAppendIntToBuffer(connection, &iconnection->outputBuffer, iconnection->profileId);
    }
    gpiAppendStringToBuffer(connection, &iconnection->outputBuffer, ov00_02218970);
    gpiAppendStringToBuffer(connection, &iconnection->outputBuffer, tmp.hash);
    if (iconnection->firewall == 1) {
        gpiAppendStringToBuffer(connection, &iconnection->outputBuffer, ov00_0221897C);
    }
    gpiAppendStringToBuffer(connection, &iconnection->outputBuffer, ov00_02218988);
    {
        u16 port = iconnection->peerPort;
        port = ((port >> 8) & 255) | ((port << 8) & 0xff00);
        gpiAppendIntToBuffer(connection, &iconnection->outputBuffer, (s16)port);
    }
    gpiAppendStringToBuffer(connection, &iconnection->outputBuffer, ov00_02218990);
    gpiAppendIntToBuffer(connection, &iconnection->outputBuffer, iconnection->productId);
    gpiAppendStringToBuffer(connection, &iconnection->outputBuffer, ov00_0221899C);
    gpiAppendStringToBuffer(connection, &iconnection->outputBuffer, ov00_0221B16C);
    gpiAppendStringToBuffer(connection, &iconnection->outputBuffer, ov00_022189A8);
    gpiAppendIntToBuffer(connection, &iconnection->outputBuffer, iconnection->namespaceId);
    gpiAppendStringToBuffer(connection, &iconnection->outputBuffer, ov00_022189B8);
    gpiAppendStringToBuffer(connection, &iconnection->outputBuffer, ov00_022189C0);
    return 0;
}

// Scrambles src with the fixed-seed random sequence and encodes it into dst.
static inline void ScrambleString(const char *src, char *tmp, char *dst) {
    u32 len = strlen(src);
    u32 i;
    Util_RandSeed(0x79707367);
    for (i = 0; i < len; i++) {
        tmp[i] = src[i] ^ (s8)Util_RandInt(0, 0xff);
    }
    tmp[i] = 0;
    B64Encode(tmp, dst, len, 1);
}

// Sends "\newuser\" (password and cd key scrambled).
int sendNewuser(GpConnection **connection, GpConnectData *data) {
    char cdkey[66];
    char password[31];
    GpConnection *iconnection = *connection;
    char passwordEnc[45];
    char cdkeyEnc[90];
    u32 len;
    char *p;
    u32 i;
    ScrambleString(iconnection->password, password, passwordEnc);
    gpiAppendStringToBuffer(connection, &iconnection->outputBuffer, ov00_022189C8);
    gpiAppendStringToBuffer(connection, &iconnection->outputBuffer, ov00_022189D4);
    gpiAppendStringToBuffer(connection, &iconnection->outputBuffer, iconnection->email);
    gpiAppendStringToBuffer(connection, &iconnection->outputBuffer, ov00_022189DC);
    gpiAppendStringToBuffer(connection, &iconnection->outputBuffer, iconnection->nick);
    gpiAppendStringToBuffer(connection, &iconnection->outputBuffer, ov00_022189E4);
    gpiAppendStringToBuffer(connection, &iconnection->outputBuffer, passwordEnc);
    gpiAppendStringToBuffer(connection, &iconnection->outputBuffer, ov00_02218990);
    gpiAppendIntToBuffer(connection, &iconnection->outputBuffer, iconnection->productId);
    gpiAppendStringToBuffer(connection, &iconnection->outputBuffer, ov00_0221899C);
    gpiAppendStringToBuffer(connection, &iconnection->outputBuffer, ov00_0221B16C);
    gpiAppendStringToBuffer(connection, &iconnection->outputBuffer, ov00_022189A8);
    gpiAppendIntToBuffer(connection, &iconnection->outputBuffer, iconnection->namespaceId);
    gpiAppendStringToBuffer(connection, &iconnection->outputBuffer, ov00_0221893C);
    gpiAppendStringToBuffer(connection, &iconnection->outputBuffer, iconnection->uniquenick);
    if (0 != data->cdkey[0]) {
        len = strlen(data->cdkey);
        Util_RandSeed(0x79707367);
        i = 0;
        if (i < len) {
            p = cdkey;
            do {
                *p = data->cdkey[i] ^ (s8)Util_RandInt(0, 0xff);
                p++;
                i++;
            } while (i < len);
        }
        cdkey[i] = 0;
        B64Encode(cdkey, cdkeyEnc, len, 1);
        gpiAppendStringToBuffer(connection, &iconnection->outputBuffer, ov00_022189F4);
        gpiAppendStringToBuffer(connection, &iconnection->outputBuffer, cdkeyEnc);
    }
    gpiAppendStringToBuffer(connection, &iconnection->outputBuffer, ov00_022189B8);
    gpiAppendStringToBuffer(connection, &iconnection->outputBuffer, ov00_022189C0);
    return 0;
}

// Handles the replies of the connect operation ("\lc\1" challenge, "\nur\" new user, "\lc\2" logged in).
int gpiProcessConnect(GpConnection **connection, GpOperation *operation, char *input) {
    GpConnectData *data;
    GpConnection *iconnection = *connection;

    struct {
        char uniquenick[21], hash[33], user[82], text[512];
    } tmp;

    GpCallback callback;
    if (gpiCheckForError(connection, input, 0)) {
        if (iconnection->errorCode == 0x106 && iconnection->profileId) {
            gpiRemoveProfileByID(connection, iconnection->profileId);
            iconnection->userId = 0;
            iconnection->profileId = 0;
        } else if (iconnection->errorCode == 0x201) {
            if (gpiValueForKey(input, ov00_02218A00, tmp.text, 512)) {
                iconnection->profileId = atol(tmp.text);
            }
        }
        if (strstr(input, ov00_02218A08)) {
            gpiSetError(connection, iconnection->errorCode, iconnection->errorString);
            gpiCallErrorCallback(connection, 4, 1);
            return 4;
        } else {
            gpiSetError(connection, iconnection->errorCode, iconnection->errorString);
            gpiCallErrorCallback(connection, 4, 0);
            return 4;
        }
    }
    data = operation->data;
    switch (operation->state) {
    case 1:
        if (strncmp(input, ov00_02218A10, 5)) {
            GP_ERROR(connection, 1, ov00_02218A18);
        }
        if (!gpiValueForKey(input, ov00_02218924, data->serverChallenge, 128)) {
            GP_ERROR(connection, 1, ov00_02218A18);
        }
        if (data->newUser) {
            int result = sendNewuser(connection, data);
            if (result) {
                return result;
            }
            operation->state = 3;
        } else {
            int result = sendLogin(connection, data);
            if (result) {
                return result;
            }
            operation->state = 2;
        }
        break;
    case 3:
        if (strncmp(input, ov00_02218A48, 5)) {
            GP_ERROR(connection, 1, ov00_02218A18);
        }
        if (!gpiValueForKey(input, ov00_02218958, tmp.text, 512)) {
            GP_ERROR(connection, 1, ov00_02218A50);
        }
        iconnection->userId = atol(tmp.text);
        if (!gpiValueForKey(input, ov00_02218964, tmp.text, 512)) {
            GP_ERROR(connection, 1, ov00_02218A50);
        }
        iconnection->profileId = atol(tmp.text);
        {
            int result = sendLogin(connection, data);
            if (result) {
                return result;
            }
        }
        operation->state = 2;
        break;
    case 2:
        if (strncmp(input, ov00_02218A80, 5)) {
            GP_ERROR(connection, 1, ov00_02218A18);
        }
        if (!gpiValueForKey(input, ov00_02218A88, tmp.text, 512)) {
            GP_ERROR(connection, 1, ov00_02218A50);
        }
        iconnection->sessKey = atol(tmp.text);
        if (!gpiValueForKey(input, ov00_02218958, tmp.text, 512)) {
            GP_ERROR(connection, 1, ov00_02218A50);
        }
        iconnection->userId = atol(tmp.text);
        if (!gpiValueForKey(input, ov00_02218964, tmp.text, 512)) {
            GP_ERROR(connection, 1, ov00_02218A50);
        }
        iconnection->profileId = atol(tmp.text);
        if (!gpiValueForKey(input, ov00_0221893C, tmp.uniquenick, 21)) {
            tmp.uniquenick[0] = 0;
        }
        if (!gpiValueForKey(input, ov00_02218A94, iconnection->loginTicket, 25)) {
            *iconnection->loginTicket = 0;
        }
        {
            char *user;
            if (*data->authtoken) {
                user = data->authtoken;
            } else if (*iconnection->uniquenick) {
                user = iconnection->uniquenick;
            } else {
                sprintf(tmp.user, ov00_022188D0, iconnection->nick, iconnection->email);
                user = tmp.user;
            }
            sprintf(tmp.text, ov00_022188D8, data->response, ov00_022188E8, user, data->serverChallenge, data->userChallenge, data->response);
        }
        MD5Digest(tmp.text, strlen(tmp.text), tmp.hash);
        if (!gpiValueForKey(input, ov00_02218A9C, tmp.text, 512)) {
            GP_ERROR(connection, 1, ov00_02218A50);
        }
        if (memcmp(tmp.hash, tmp.text, 32)) {
            GP_ERROR(connection, 0x108, ov00_02218AA4);
        }
        if (iconnection->cacheProfileInfo) {
            GpProfile *profile = gpiProfileListAdd(connection, iconnection->profileId);
            profile->profileId = iconnection->profileId;
            profile->userId = iconnection->userId;
        }
        iconnection->connectState = GP_CONNECTED;
        callback.callback = operation->callback.callback;
        callback.param = operation->callback.param;
        if (callback.callback) {
            GpConnectArg *arg = DWCi_GsMalloc(sizeof(GpConnectArg));
            int result;
            if (!arg) {
                gpiSetErrorString(connection, ov00_022188C0);
                return 1;
            }
            memset(arg, 0, sizeof(GpConnectArg));
            arg->profile = iconnection->profileId;
            arg->result = 0;
            strzcpy(arg->uniquenick, tmp.uniquenick, 21);
            result = gpiAddCallback(connection, callback, arg, operation, 0);
            if (result) {
                return result;
            }
        }
        gpiRemoveOperation(connection, operation);
        break;
    }
    return 0;
}

// "gpiCheckConnect"
int gpiCheckConnect(GpConnection **connection) {
    GpConnection *iconnection = *connection;
    int state;
    int result = gpiCheckSocketConnect(connection, iconnection->sock, &state);
    if (result) {
        return result;
    }
    if (state == 4) {
        gpiSetError(connection, 0x107, ov00_02218AC4);
        gpiCallErrorCallback(connection, 4, 1);
        return 4;
    }
    if (state == 0) {
        return 0;
    }
    if (state != 3) {
        __msl_assertion_failed(ov00_02218AEC, ov00_02218860, ov00_0221869C, 0x2ef);
    }
    iconnection->connectState = GP_NEGOTIATING;
    return 0;
}

// Profile table iterator for the disconnect: frees the buddy data; returns 0 after removing the profile.
int gpiDisconnectCleanupProfile(GpConnection **connection, GpProfile *profile, void *data) {
    GpConnection *iconnection = *connection;
    if (profile->buddyStatus && !iconnection->unk_104) {
        DWCi_GsFree(profile->buddyStatus->statusString);
        profile->buddyStatus->statusString = 0;
        DWCi_GsFree(profile->buddyStatus->locationString);
        profile->buddyStatus->locationString = 0;
        DWCi_GsFree(profile->buddyStatus);
        profile->buddyStatus = 0;
    }
    DWCi_GsFree(profile->authSig);
    profile->authSig = 0;
    DWCi_GsFree(profile->peerSig);
    profile->peerSig = 0;
    profile->requestCount = 0;
    if (!profile->cache || (iconnection->unk_104 == 1 && !profile->buddyStatus)) {
        gpiRemoveProfile(connection, profile);
        return 0;
    }
    return 1;
}

// Disconnects ("\logout\" when tearDown is set and logged in), closes the sockets and frees the
// operations, peers and buddy data.
void gpiDisconnect(GpConnection **connection, int tearDown) {
    GpConnection *iconnection = *connection;
    GpPeer *peer;
    int closed;
    if (iconnection->connectState == GP_DISCONNECTED) {
        return;
    }
    if (iconnection->connectState != GP_NOT_CONNECTED) {
        if (tearDown && iconnection->connectState == GP_CONNECTED) {
            gpiAppendStringToBuffer(connection, &iconnection->outputBuffer, ov00_02218B04);
            gpiAppendIntToBuffer(connection, &iconnection->outputBuffer, iconnection->sessKey);
            gpiAppendStringToBuffer(connection, &iconnection->outputBuffer, ov00_022189C0);
        }
        gpiSendFromBuffer(connection, iconnection->sock, &iconnection->outputBuffer, &closed, 1, ov00_02218B18);
        if (iconnection->sock != -1) {
            gsSocketShutdown(iconnection->sock, 2);
            gsSocketClose(iconnection->sock);
            iconnection->sock = -1;
        }
        if (iconnection->peerSocket != -1) {
            gsSocketShutdown(iconnection->peerSocket, 2);
            gsSocketClose(iconnection->peerSocket);
            iconnection->peerSocket = -1;
        }
        iconnection->connectState = GP_DISCONNECTED;
        iconnection->userId = 0;
        iconnection->profileId = 0;
    }
    DWCi_GsFree(iconnection->inputBuffer.buffer);
    iconnection->inputBuffer.buffer = 0;
    DWCi_GsFree(iconnection->lineBuffer);
    iconnection->lineBuffer = 0;
    DWCi_GsFree(iconnection->outputBuffer.buffer);
    iconnection->outputBuffer.buffer = 0;
    DWCi_GsFree(iconnection->updateproBuffer.buffer);
    iconnection->updateproBuffer.buffer = 0;
    DWCi_GsFree(iconnection->updateuiBuffer.buffer);
    iconnection->updateuiBuffer.buffer = 0;
    while (iconnection->operations) {
        gpiRemoveOperation(connection, iconnection->operations);
    }
    iconnection->operations = 0;
    peer = iconnection->peers;
    while (peer) {
        GpPeer *current = peer;
        peer = peer->next;
        gpiDestroyPeer(connection, current);
    }
    iconnection->peers = 0;
    while (!gpiProfileMap(connection, gpiDisconnectCleanupProfile, 0)) {}
}

// "gpiIsValidDate" (0/0/0 = no date).
int gpiIsValidDate(int day, int month, int year) {
    if (day == 0 && month == 0 && year == 0) {
        return 1;
    }
    if (day < 0 || month < 0 || year < 0) {
        return 0;
    }
    switch (month) {
    case 0:
        if (day != 0) {
            return 0;
        }
        break;
    case 1:
    case 3:
    case 5:
    case 7:
    case 8:
    case 10:
    case 12:
        if (day > 31) {
            return 0;
        }
        break;
    case 4:
    case 6:
    case 9:
    case 11:
        if (day > 30) {
            return 0;
        }
        break;
    case 2:
        if ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0) {
            if (day > 29) {
                return 0;
            }
        } else if (day > 28) {
            return 0;
        }
        break;
    default:
        return 0;
    }
    if (year < 1900) {
        return 0;
    }
    if (year > 2079) {
        return 0;
    }
    if (year == 2079) {
        if (month > 6) {
            return 0;
        }
        if (month == 6 && day > 6) {
            return 0;
        }
    }
    return 1;
}

// "gpiIntToDate": date packed as day << 24 | month << 16 | year.
int gpiIntToDate(GpConnection **connection, int date, int *day, int *month, int *year) {
    int d = (date >> 24) & 255, m = (date >> 16) & 255, y = (u16)date;
    if (!gpiIsValidDate(d, m, y)) {
        __msl_assertion_failed(ov00_02218B5C, ov00_02218B40, ov00_02218B1C, 0xb7);
    }
    if (!gpiIsValidDate(d, m, y)) {
        gpiSetErrorString(connection, ov00_02218B4C);
        return 2;
    }
    *day = d;
    *month = m;
    *year = y;
    return 0;
}

// Copies the parsed profile info into the callback argument.
void gpiInfoCacheToArg(GpInfo *info, GpInfoArg *arg) {
    if (!info->nick) {
        arg->nick[0] = 0;
    } else {
        strzcpy(arg->nick, info->nick, 31);
    }
    if (!info->uniquenick) {
        arg->uniquenick[0] = 0;
    } else {
        strzcpy(arg->uniquenick, info->uniquenick, 21);
    }
    if (!info->email) {
        arg->email[0] = 0;
    } else {
        strzcpy(arg->email, info->email, 51);
    }
    if (!info->firstname) {
        arg->firstname[0] = 0;
    } else {
        strzcpy(arg->firstname, info->firstname, 31);
    }
    if (!info->lastname) {
        arg->lastname[0] = 0;
    } else {
        strzcpy(arg->lastname, info->lastname, 31);
    }
    if (!info->homepage) {
        arg->homepage[0] = 0;
    } else {
        strzcpy(arg->homepage, info->homepage, 76);
    }
    arg->icquin = info->icquin;
    strzcpy(arg->zipcode, info->zipcode, 11);
    strzcpy(arg->countrycode, info->countrycode, 3);
    arg->longitude = info->longitude;
    arg->latitude = info->latitude;
    if (!info->place) {
        arg->place[0] = 0;
    } else {
        strzcpy(arg->place, info->place, 128);
    }
    arg->birthday = info->birthday;
    arg->birthmonth = info->birthmonth;
    arg->birthyear = info->birthyear;
    arg->sex = info->sex;
    arg->publicmask = info->publicmask;
    if (!info->aimname) {
        arg->aimname[0] = 0;
    } else {
        strzcpy(arg->aimname, info->aimname, 51);
    }
    arg->icquin = info->icquin;
    arg->longitude = info->longitude;
    arg->latitude = info->latitude;
    arg->birthday = info->birthday;
    arg->birthmonth = info->birthmonth;
    arg->birthyear = info->birthyear;
    arg->sex = info->sex;
    arg->publicmask = info->publicmask;
    arg->pic = info->pic;
    arg->occupationid = info->occupationid;
    arg->industryid = info->industryid;
    arg->incomeid = info->incomeid;
    arg->marriedid = info->marriedid;
    arg->childcount = info->childcount;
    arg->interests1 = info->interests1;
    arg->ownership1 = info->ownership1;
    arg->conntypeid = info->conntypeid;
}

#pragma inlineintrinsics on

// "gpiProcessGetInfo": handles a "\pi\" reply.
int gpiProcessGetInfo(GpConnection **connection, GpOperation *operation, char *input) {
    GpInfo info;
    char homepage[76], buffer[64], nick[31], uniquenick[21], email[51], firstname[31], lastname[31], aimname[51];
    GpProfile *profile;
    GpConnection *iconnection = *connection;
    GpPeer *peer;
    int cache, result, profileId;
    GpCallback callback;
    GpInfoArg *arg;
    if (gpiCheckForError(connection, input, 1)) {
        return 4;
    }
    if (strncmp(input, ov00_02218B74, 4)) {
        GP_ERROR(connection, 1, ov00_02218B7C);
    }
    if (!gpiValueForKey(input, ov00_02218BAC, buffer, 64)) {
        GP_ERROR(connection, 1, ov00_02218B7C);
    }
    profileId = atol(buffer);
    if (profileId <= 0) {
        __msl_assertion_failed(ov00_02218BB8, ov00_02218B40, ov00_02218B2C, 346);
    }
    gpiGetProfile(connection, profileId, &profile);
    memset(&info, 0, sizeof(info));
    info.nick = nick;
    info.uniquenick = uniquenick;
    info.email = email;
    info.firstname = firstname;
    info.lastname = lastname;
    info.homepage = homepage;
    info.aimname = aimname;
    if (!gpiValueForKey(input, ov00_02218BC8, info.nick, 31)) {
        info.nick[0] = 0;
    }
    if (!gpiValueForKey(input, ov00_02218BD0, info.uniquenick, 21)) {
        info.uniquenick[0] = 0;
    }
    if (!gpiValueForKey(input, ov00_02218BE0, info.email, 51)) {
        info.email[0] = 0;
    }
    if (!gpiValueForKey(input, ov00_02218BE8, info.firstname, 31)) {
        info.firstname[0] = 0;
    }
    if (!gpiValueForKey(input, ov00_02218BF4, info.lastname, 31)) {
        info.lastname[0] = 0;
    }
    if (!gpiValueForKey(input, ov00_02218C00, buffer, 64)) {
        info.icquin = -1;
    } else {
        info.icquin = atol(buffer);
    }
    if (!gpiValueForKey(input, ov00_02218C0C, info.homepage, 76)) {
        info.homepage[0] = 0;
    }
    if (!gpiValueForKey(input, ov00_02218C18, info.zipcode, 11)) {
        info.zipcode[0] = 0;
    }
    if (!gpiValueForKey(input, ov00_02218C24, info.countrycode, 3)) {
        info.countrycode[0] = 0;
    }
    if (!gpiValueForKey(input, ov00_02218C34, buffer, 64)) {
        info.longitude = 0;
    } else {
        info.longitude = atod(buffer);
    }
    if (!gpiValueForKey(input, ov00_02218C3C, buffer, 64)) {
        info.latitude = 0;
    } else {
        info.latitude = atod(buffer);
    }
    if (!gpiValueForKey(input, ov00_02218C44, info.place, 128)) {
        info.place[0] = 0;
    }
    if (!gpiValueForKey(input, ov00_02218C4C, buffer, 64)) {
        info.birthday = 0;
        info.birthmonth = 0;
        info.birthyear = 0;
    } else {
        result = gpiIntToDate(connection, atol(buffer), &info.birthday, &info.birthmonth, &info.birthyear);
        if (result) {
            return result;
        }
    }
    if (!gpiValueForKey(input, ov00_02218C58, buffer, 64)) {
        info.sex = 1282;
    } else if (buffer[0] == '0') {
        info.sex = 1280;
    } else if (buffer[0] == '1') {
        info.sex = 1281;
    } else {
        info.sex = 1282;
    }
    if (!gpiValueForKey(input, ov00_02218C60, buffer, 64)) {
        info.publicmask = -1;
    } else {
        info.publicmask = atol(buffer);
    }
    if (!gpiValueForKey(input, ov00_02218C68, info.aimname, 51)) {
        info.aimname[0] = 0;
    }
    if (!gpiValueForKey(input, ov00_02218C70, buffer, 64)) {
        info.pic = 0;
    } else {
        info.pic = atol(buffer);
    }
    if (!gpiValueForKey(input, ov00_02218C78, buffer, 64)) {
        info.occupationid = 0;
    } else {
        info.occupationid = atol(buffer);
    }
    if (!gpiValueForKey(input, ov00_02218C80, buffer, 64)) {
        info.industryid = 0;
    } else {
        info.industryid = atol(buffer);
    }
    if (!gpiValueForKey(input, ov00_02218C88, buffer, 64)) {
        info.incomeid = 0;
    } else {
        info.incomeid = atol(buffer);
    }
    if (!gpiValueForKey(input, ov00_02218C90, buffer, 64)) {
        info.marriedid = 0;
    } else {
        info.marriedid = atol(buffer);
    }
    if (!gpiValueForKey(input, ov00_02218C98, buffer, 64)) {
        info.childcount = 0;
    } else {
        info.childcount = atol(buffer);
    }
    if (!gpiValueForKey(input, ov00_02218CA0, buffer, 64)) {
        info.interests1 = 0;
    } else {
        info.interests1 = atol(buffer);
    }
    if (!gpiValueForKey(input, ov00_02218CA8, buffer, 64)) {
        info.ownership1 = 0;
    } else {
        info.ownership1 = atol(buffer);
    }
    if (!gpiValueForKey(input, ov00_02218CB0, buffer, 64)) {
        info.conntypeid = 0;
    } else {
        info.conntypeid = atol(buffer);
    }
    if (!gpiValueForKey(input, ov00_02218CB8, buffer, 64)) {
        GP_ERROR(connection, 1, ov00_02218B7C);
    }
    peer = iconnection->peers;
    cache = iconnection->cacheProfileInfo;
    while (peer) {
        if (peer->profile == profileId && peer->state == 101) {
            if (!profile) {
                profile = gpiProfileListAdd(connection, profileId);
            }
            peer->state = 102;
            cache = 1;
        }
        peer = peer->next;
    }
    if (!profile && iconnection->cacheProfileInfo) {
        profile = gpiProfileListAdd(connection, profileId);
    }
    if (cache) {
        DWCi_GsFree(profile->peerSig);
        profile->peerSig = 0;
        profile->peerSig = goastrdup(buffer);
    }
    if (iconnection->cacheProfileInfo) {
        gpiSetInfoCache(connection, profile, &info);
    }
    callback.callback = operation->callback.callback;
    callback.param = operation->callback.param;
    if (callback.callback) {
        arg = DWCi_GsMalloc(sizeof(GpInfoArg));
        if (!arg) {
            gpiSetErrorString(connection, ov00_02218CC0);
            return 1;
        }
        gpiInfoCacheToArg(&info, arg);
        arg->result = 0;
        arg->profile = profileId;
        result = gpiAddCallback(connection, callback, arg, operation, 0);
        if (result) {
            return result;
        }
    }
    gpiRemoveOperation(connection, operation);
    return 0;
}
