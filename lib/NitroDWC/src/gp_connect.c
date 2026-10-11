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
extern void *ov00_021EC3A8(int size);
extern void ov00_021EC3D8(void *p);
extern void ov00_021FF31C(const char *data, int len, char *md5);
extern void ov00_021FF408(char *string);
extern char *ov00_021FF3D4(const char *string);
extern int ov00_021FF44C(int sock, int blocking);
extern int ov00_021FF804(int domain, int type, int protocol);
extern void ov00_021FF818(int sock);
extern void ov00_021FF82C(int sock, int how);
extern int ov00_021FF840(int sock, SocketAddress *address, int len);
extern int ov00_021FF8A0(int sock, SocketAddress *address, int len);
extern int ov00_021FF8EC(int sock, int backlog);
extern int ov00_021FFA18(int sock, SocketAddress *address, int *len);
extern int ov00_021FFA60(int sock);
extern void ov00_021FFB40(u32 seed);
extern int ov00_021FFB5C(int min, int max);
extern void ov00_021FFC18(const char *src, char *dst, u32 len, int encoding);
extern void ov00_02202990(GpConnection **connection, int result, int fatal);
extern int ov00_02202A84(GpConnection **connection, GpCallback callback, void *arg, GpOperation *operation, int type);

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

int ov00_02203428(GpConnection **connection, GpConnectData *data);
int ov00_022037CC(GpConnection **connection, GpConnectData *data);

#define GP_ERROR(connection, code, message)       \
    {                                             \
        ov00_0220A86C(connection, code, message); \
        ov00_02202990(connection, 3, 1);          \
        return 3;                                 \
    }

// Calls the queued callbacks: with operationId set only that operation's (and the error callbacks),
// otherwise all of them.
int ov00_02202D20(GpConnection **connection, int operationId) {
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
                ov00_02202B28(connection, data);
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
            ov00_02202B28(connection, data);
            data = next;
        }
    }
    return 0;
}

// Random alphanumeric string of count characters.
void ov00_02202E08(char *out, int count) {
    int i;
    for (i = 0; i < count; i++) {
        out[i] = ov00_022186EC[(u32)rand() % 62];
    }
    out[i] = 0;
}

// "gpiStartConnect": opens the peer listening socket (unless behind a firewall) and starts connecting to
// the connection manager.
int ov00_02202E70(GpConnection **connection, GpOperation *operation) {
    GpConnection *iconnection = *connection;
    SocketAddress address;
    int len;
    if (iconnection->firewall == 0) {
        iconnection->peerSocket = ov00_021FF804(2, 1, 0);
        if (iconnection->peerSocket == -1) {
            GP_ERROR(connection, 5, ov00_0221872C);
        }
        if (!ov00_021FF44C(iconnection->peerSocket, 0)) {
            GP_ERROR(connection, 5, ov00_02218754);
        }
        {
            u32 *p = (u32 *)&address;
            p[0] = 0;
            p[1] = 0;
            address.in.family = 2;
            if (ov00_021FF840(iconnection->peerSocket, &address, 8) == -1) {
                GP_ERROR(connection, 5, ov00_02218788);
            }
            if (ov00_021FF8EC(iconnection->peerSocket, 5) == -1) {
                GP_ERROR(connection, 5, ov00_022187B0);
            }
            len = 8;
            if (ov00_021FFA18(iconnection->peerSocket, &address, &len) == -1) {
                GP_ERROR(connection, 5, ov00_022187DC);
            }
            iconnection->peerPort = address.in.port;
        }
    } else {
        iconnection->peerSocket = -1;
        iconnection->peerPort = 0;
    }
    iconnection->sock = ov00_021FF804(2, 1, 0);
    if (iconnection->sock == -1) {
        GP_ERROR(connection, 5, ov00_0221872C);
    }
    if (!ov00_021FF44C(iconnection->sock, 0)) {
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
            if (ov00_021FF8A0(iconnection->sock, &address, 8) == -1) {
                int error = ov00_021FFA60(iconnection->sock);
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
int ov00_022031E4(GpConnection **connection, char *nick, char *uniquenick, char *email, char *password, char *authtoken, char *partnerChallenge, char *cdkey, int firewall, int newUser, int blocking, void *callback, void *param) {
    GpConnection *iconnection = *connection;
    GpConnectData *data;
    GpOperation *operation;
    int result;
    if (iconnection->connectState == GP_DISCONNECTED) {
        result = ov00_02200A98(connection);
        if (result) {
            return result;
        }
    }
    if (iconnection->connectState) {
        ov00_0220A890(connection, ov00_02218898);
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
        ov00_0220A890(connection, ov00_022188AC);
        return 2;
    }
    iconnection->firewall = 1;
    ov00_0220A348(iconnection->nick, nick, 31);
    ov00_0220A348(iconnection->uniquenick, uniquenick, 21);
    ov00_0220A348(iconnection->email, email, 51);
    ov00_0220A348(iconnection->password, password, 31);
    ov00_021FF408(iconnection->email);
    data = ov00_021EC3A8(sizeof(GpConnectData));
    if (!data) {
        ov00_0220A890(connection, ov00_022188C0);
        return 1;
    }
    memset(data, 0, sizeof(GpConnectData));
    data->newUser = newUser;
    if (*authtoken && *partnerChallenge) {
        ov00_0220A348(data->authtoken, authtoken, 256);
        ov00_0220A348(data->partnerChallenge, partnerChallenge, 256);
    }
    if (cdkey) {
        ov00_0220A348(data->cdkey, cdkey, 65);
    }
    result = ov00_0220657C(connection, GP_OPERATION_CONNECT, data, &operation, blocking, callback, param);
    if (result) {
        return result;
    }
    result = ov00_02202E70(connection, operation);
    if (result) {
        operation->result = result;
        ov00_02206230(connection, operation);
        ov00_02204284(connection, 0);
        return result;
    }
    if (operation->blocking) {
        result = ov00_02200ED8(connection, operation->id);
        if (result) {
            return result;
        }
    }
    return 0;
}

// Sends "\login\" with the challenge response.
int ov00_02203428(GpConnection **connection, GpConnectData *data) {
    GpConnection *iconnection = *connection;
    char *password;
    char *user;
    GpProfile *profile;

    struct {
        char hash[33], user[82], text[512];
    } tmp;

    ov00_02202E08(data->userChallenge, 32);
    password = *data->partnerChallenge ? data->partnerChallenge : iconnection->password;
    ov00_021FF31C(password, strlen(password), data->response);
    if (*data->authtoken) {
        user = data->authtoken;
    } else if (*iconnection->uniquenick) {
        user = iconnection->uniquenick;
    } else {
        sprintf(tmp.user, ov00_022188D0, iconnection->nick, iconnection->email);
        user = tmp.user;
    }
    sprintf(tmp.text, ov00_022188D8, data->response, ov00_022188E8, user, data->userChallenge, data->serverChallenge, data->response);
    ov00_021FF31C(tmp.text, strlen(tmp.text), tmp.hash);
    if (iconnection->cacheProfileInfo) {
        ov00_02207EB4(connection, iconnection->nick, iconnection->email, &profile);
        if (profile) {
            iconnection->userId = profile->userId;
            iconnection->profileId = profile->profileId;
        }
    }
    ov00_022020EC(connection, &iconnection->outputBuffer, ov00_0221891C);
    ov00_022020EC(connection, &iconnection->outputBuffer, ov00_02218924);
    ov00_022020EC(connection, &iconnection->outputBuffer, data->userChallenge);
    if (*data->authtoken) {
        ov00_022020EC(connection, &iconnection->outputBuffer, ov00_02218930);
        ov00_022020EC(connection, &iconnection->outputBuffer, data->authtoken);
    } else if (*iconnection->uniquenick) {
        ov00_022020EC(connection, &iconnection->outputBuffer, ov00_0221893C);
        ov00_022020EC(connection, &iconnection->outputBuffer, iconnection->uniquenick);
    } else {
        ov00_022020EC(connection, &iconnection->outputBuffer, ov00_0221894C);
        ov00_022020EC(connection, &iconnection->outputBuffer, iconnection->nick);
        ov00_022020EC(connection, &iconnection->outputBuffer, ov00_02218954);
        ov00_022020EC(connection, &iconnection->outputBuffer, iconnection->email);
    }
    if (iconnection->userId) {
        ov00_022020EC(connection, &iconnection->outputBuffer, ov00_02218958);
        ov00_0220211C(connection, &iconnection->outputBuffer, iconnection->userId);
    }
    if (iconnection->profileId) {
        ov00_022020EC(connection, &iconnection->outputBuffer, ov00_02218964);
        ov00_0220211C(connection, &iconnection->outputBuffer, iconnection->profileId);
    }
    ov00_022020EC(connection, &iconnection->outputBuffer, ov00_02218970);
    ov00_022020EC(connection, &iconnection->outputBuffer, tmp.hash);
    if (iconnection->firewall == 1) {
        ov00_022020EC(connection, &iconnection->outputBuffer, ov00_0221897C);
    }
    ov00_022020EC(connection, &iconnection->outputBuffer, ov00_02218988);
    {
        u16 port = iconnection->peerPort;
        port = ((port >> 8) & 255) | ((port << 8) & 0xff00);
        ov00_0220211C(connection, &iconnection->outputBuffer, (s16)port);
    }
    ov00_022020EC(connection, &iconnection->outputBuffer, ov00_02218990);
    ov00_0220211C(connection, &iconnection->outputBuffer, iconnection->productId);
    ov00_022020EC(connection, &iconnection->outputBuffer, ov00_0221899C);
    ov00_022020EC(connection, &iconnection->outputBuffer, ov00_0221B16C);
    ov00_022020EC(connection, &iconnection->outputBuffer, ov00_022189A8);
    ov00_0220211C(connection, &iconnection->outputBuffer, iconnection->namespaceId);
    ov00_022020EC(connection, &iconnection->outputBuffer, ov00_022189B8);
    ov00_022020EC(connection, &iconnection->outputBuffer, ov00_022189C0);
    return 0;
}

// Scrambles src with the fixed-seed random sequence and encodes it into dst.
static inline void ScrambleString(const char *src, char *tmp, char *dst) {
    u32 len = strlen(src);
    u32 i;
    ov00_021FFB40(0x79707367);
    for (i = 0; i < len; i++) {
        tmp[i] = src[i] ^ (s8)ov00_021FFB5C(0, 0xff);
    }
    tmp[i] = 0;
    ov00_021FFC18(tmp, dst, len, 1);
}

// Sends "\newuser\" (password and cd key scrambled).
int ov00_022037CC(GpConnection **connection, GpConnectData *data) {
    char cdkey[66];
    char password[31];
    GpConnection *iconnection = *connection;
    char passwordEnc[45];
    char cdkeyEnc[90];
    u32 len;
    char *p;
    u32 i;
    ScrambleString(iconnection->password, password, passwordEnc);
    ov00_022020EC(connection, &iconnection->outputBuffer, ov00_022189C8);
    ov00_022020EC(connection, &iconnection->outputBuffer, ov00_022189D4);
    ov00_022020EC(connection, &iconnection->outputBuffer, iconnection->email);
    ov00_022020EC(connection, &iconnection->outputBuffer, ov00_022189DC);
    ov00_022020EC(connection, &iconnection->outputBuffer, iconnection->nick);
    ov00_022020EC(connection, &iconnection->outputBuffer, ov00_022189E4);
    ov00_022020EC(connection, &iconnection->outputBuffer, passwordEnc);
    ov00_022020EC(connection, &iconnection->outputBuffer, ov00_02218990);
    ov00_0220211C(connection, &iconnection->outputBuffer, iconnection->productId);
    ov00_022020EC(connection, &iconnection->outputBuffer, ov00_0221899C);
    ov00_022020EC(connection, &iconnection->outputBuffer, ov00_0221B16C);
    ov00_022020EC(connection, &iconnection->outputBuffer, ov00_022189A8);
    ov00_0220211C(connection, &iconnection->outputBuffer, iconnection->namespaceId);
    ov00_022020EC(connection, &iconnection->outputBuffer, ov00_0221893C);
    ov00_022020EC(connection, &iconnection->outputBuffer, iconnection->uniquenick);
    if (0 != data->cdkey[0]) {
        len = strlen(data->cdkey);
        ov00_021FFB40(0x79707367);
        i = 0;
        if (i < len) {
            p = cdkey;
            do {
                *p = data->cdkey[i] ^ (s8)ov00_021FFB5C(0, 0xff);
                p++;
                i++;
            } while (i < len);
        }
        cdkey[i] = 0;
        ov00_021FFC18(cdkey, cdkeyEnc, len, 1);
        ov00_022020EC(connection, &iconnection->outputBuffer, ov00_022189F4);
        ov00_022020EC(connection, &iconnection->outputBuffer, cdkeyEnc);
    }
    ov00_022020EC(connection, &iconnection->outputBuffer, ov00_022189B8);
    ov00_022020EC(connection, &iconnection->outputBuffer, ov00_022189C0);
    return 0;
}

// Handles the replies of the connect operation ("\lc\1" challenge, "\nur\" new user, "\lc\2" logged in).
int ov00_02203A58(GpConnection **connection, GpOperation *operation, char *input) {
    GpConnectData *data;
    GpConnection *iconnection = *connection;

    struct {
        char uniquenick[21], hash[33], user[82], text[512];
    } tmp;

    GpCallback callback;
    if (ov00_0220A3C8(connection, input, 0)) {
        if (iconnection->errorCode == 0x106 && iconnection->profileId) {
            ov00_02207E0C(connection, iconnection->profileId);
            iconnection->userId = 0;
            iconnection->profileId = 0;
        } else if (iconnection->errorCode == 0x201) {
            if (ov00_0220A4A0(input, ov00_02218A00, tmp.text, 512)) {
                iconnection->profileId = atol(tmp.text);
            }
        }
        if (strstr(input, ov00_02218A08)) {
            ov00_0220A86C(connection, iconnection->errorCode, iconnection->errorString);
            ov00_02202990(connection, 4, 1);
            return 4;
        } else {
            ov00_0220A86C(connection, iconnection->errorCode, iconnection->errorString);
            ov00_02202990(connection, 4, 0);
            return 4;
        }
    }
    data = operation->data;
    switch (operation->state) {
    case 1:
        if (strncmp(input, ov00_02218A10, 5)) {
            GP_ERROR(connection, 1, ov00_02218A18);
        }
        if (!ov00_0220A4A0(input, ov00_02218924, data->serverChallenge, 128)) {
            GP_ERROR(connection, 1, ov00_02218A18);
        }
        if (data->newUser) {
            int result = ov00_022037CC(connection, data);
            if (result) {
                return result;
            }
            operation->state = 3;
        } else {
            int result = ov00_02203428(connection, data);
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
        if (!ov00_0220A4A0(input, ov00_02218958, tmp.text, 512)) {
            GP_ERROR(connection, 1, ov00_02218A50);
        }
        iconnection->userId = atol(tmp.text);
        if (!ov00_0220A4A0(input, ov00_02218964, tmp.text, 512)) {
            GP_ERROR(connection, 1, ov00_02218A50);
        }
        iconnection->profileId = atol(tmp.text);
        {
            int result = ov00_02203428(connection, data);
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
        if (!ov00_0220A4A0(input, ov00_02218A88, tmp.text, 512)) {
            GP_ERROR(connection, 1, ov00_02218A50);
        }
        iconnection->sessKey = atol(tmp.text);
        if (!ov00_0220A4A0(input, ov00_02218958, tmp.text, 512)) {
            GP_ERROR(connection, 1, ov00_02218A50);
        }
        iconnection->userId = atol(tmp.text);
        if (!ov00_0220A4A0(input, ov00_02218964, tmp.text, 512)) {
            GP_ERROR(connection, 1, ov00_02218A50);
        }
        iconnection->profileId = atol(tmp.text);
        if (!ov00_0220A4A0(input, ov00_0221893C, tmp.uniquenick, 21)) {
            tmp.uniquenick[0] = 0;
        }
        if (!ov00_0220A4A0(input, ov00_02218A94, iconnection->loginTicket, 25)) {
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
        ov00_021FF31C(tmp.text, strlen(tmp.text), tmp.hash);
        if (!ov00_0220A4A0(input, ov00_02218A9C, tmp.text, 512)) {
            GP_ERROR(connection, 1, ov00_02218A50);
        }
        if (memcmp(tmp.hash, tmp.text, 32)) {
            GP_ERROR(connection, 0x108, ov00_02218AA4);
        }
        if (iconnection->cacheProfileInfo) {
            GpProfile *profile = ov00_02207CEC(connection, iconnection->profileId);
            profile->profileId = iconnection->profileId;
            profile->userId = iconnection->userId;
        }
        iconnection->connectState = GP_CONNECTED;
        callback.callback = operation->callback.callback;
        callback.param = operation->callback.param;
        if (callback.callback) {
            GpConnectArg *arg = ov00_021EC3A8(sizeof(GpConnectArg));
            int result;
            if (!arg) {
                ov00_0220A890(connection, ov00_022188C0);
                return 1;
            }
            memset(arg, 0, sizeof(GpConnectArg));
            arg->profile = iconnection->profileId;
            arg->result = 0;
            ov00_0220A348(arg->uniquenick, tmp.uniquenick, 21);
            result = ov00_02202A84(connection, callback, arg, operation, 0);
            if (result) {
                return result;
            }
        }
        ov00_022066D0(connection, operation);
        break;
    }
    return 0;
}

// "gpiCheckConnect"
int ov00_0220411C(GpConnection **connection) {
    GpConnection *iconnection = *connection;
    int state;
    int result = ov00_0220A5A8(connection, iconnection->sock, &state);
    if (result) {
        return result;
    }
    if (state == 4) {
        ov00_0220A86C(connection, 0x107, ov00_02218AC4);
        ov00_02202990(connection, 4, 1);
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
int ov00_022041C0(GpConnection **connection, GpProfile *profile, void *data) {
    GpConnection *iconnection = *connection;
    if (profile->buddyStatus && !iconnection->unk_104) {
        ov00_021EC3D8(profile->buddyStatus->statusString);
        profile->buddyStatus->statusString = 0;
        ov00_021EC3D8(profile->buddyStatus->locationString);
        profile->buddyStatus->locationString = 0;
        ov00_021EC3D8(profile->buddyStatus);
        profile->buddyStatus = 0;
    }
    ov00_021EC3D8(profile->authSig);
    profile->authSig = 0;
    ov00_021EC3D8(profile->peerSig);
    profile->peerSig = 0;
    profile->requestCount = 0;
    if (!profile->cache || (iconnection->unk_104 == 1 && !profile->buddyStatus)) {
        ov00_02207E40(connection, profile);
        return 0;
    }
    return 1;
}

// Disconnects ("\logout\" when tearDown is set and logged in), closes the sockets and frees the
// operations, peers and buddy data.
void ov00_02204284(GpConnection **connection, int tearDown) {
    GpConnection *iconnection = *connection;
    GpPeer *peer;
    int closed;
    if (iconnection->connectState == GP_DISCONNECTED) {
        return;
    }
    if (iconnection->connectState != GP_NOT_CONNECTED) {
        if (tearDown && iconnection->connectState == GP_CONNECTED) {
            ov00_022020EC(connection, &iconnection->outputBuffer, ov00_02218B04);
            ov00_0220211C(connection, &iconnection->outputBuffer, iconnection->sessKey);
            ov00_022020EC(connection, &iconnection->outputBuffer, ov00_022189C0);
        }
        ov00_0220267C(connection, iconnection->sock, &iconnection->outputBuffer, &closed, 1, ov00_02218B18);
        if (iconnection->sock != -1) {
            ov00_021FF82C(iconnection->sock, 2);
            ov00_021FF818(iconnection->sock);
            iconnection->sock = -1;
        }
        if (iconnection->peerSocket != -1) {
            ov00_021FF82C(iconnection->peerSocket, 2);
            ov00_021FF818(iconnection->peerSocket);
            iconnection->peerSocket = -1;
        }
        iconnection->connectState = GP_DISCONNECTED;
        iconnection->userId = 0;
        iconnection->profileId = 0;
    }
    ov00_021EC3D8(iconnection->inputBuffer.buffer);
    iconnection->inputBuffer.buffer = 0;
    ov00_021EC3D8(iconnection->lineBuffer);
    iconnection->lineBuffer = 0;
    ov00_021EC3D8(iconnection->outputBuffer.buffer);
    iconnection->outputBuffer.buffer = 0;
    ov00_021EC3D8(iconnection->updateproBuffer.buffer);
    iconnection->updateproBuffer.buffer = 0;
    ov00_021EC3D8(iconnection->updateuiBuffer.buffer);
    iconnection->updateuiBuffer.buffer = 0;
    while (iconnection->operations) {
        ov00_022066D0(connection, iconnection->operations);
    }
    iconnection->operations = 0;
    peer = iconnection->peers;
    while (peer) {
        GpPeer *current = peer;
        peer = peer->next;
        ov00_022071F4(connection, current);
    }
    iconnection->peers = 0;
    while (!ov00_02207F1C(connection, ov00_022041C0, 0)) {}
}

// "gpiIsValidDate" (0/0/0 = no date).
int ov00_0220444C(int day, int month, int year) {
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
int ov00_022045AC(GpConnection **connection, int date, int *day, int *month, int *year) {
    int d = (date >> 24) & 255, m = (date >> 16) & 255, y = (u16)date;
    if (!ov00_0220444C(d, m, y)) {
        __msl_assertion_failed(ov00_02218B5C, ov00_02218B40, ov00_02218B1C, 0xb7);
    }
    if (!ov00_0220444C(d, m, y)) {
        ov00_0220A890(connection, ov00_02218B4C);
        return 2;
    }
    *day = d;
    *month = m;
    *year = y;
    return 0;
}

// Copies the parsed profile info into the callback argument.
void ov00_02204654(GpInfo *info, GpInfoArg *arg) {
    if (!info->nick) {
        arg->nick[0] = 0;
    } else {
        ov00_0220A348(arg->nick, info->nick, 31);
    }
    if (!info->uniquenick) {
        arg->uniquenick[0] = 0;
    } else {
        ov00_0220A348(arg->uniquenick, info->uniquenick, 21);
    }
    if (!info->email) {
        arg->email[0] = 0;
    } else {
        ov00_0220A348(arg->email, info->email, 51);
    }
    if (!info->firstname) {
        arg->firstname[0] = 0;
    } else {
        ov00_0220A348(arg->firstname, info->firstname, 31);
    }
    if (!info->lastname) {
        arg->lastname[0] = 0;
    } else {
        ov00_0220A348(arg->lastname, info->lastname, 31);
    }
    if (!info->homepage) {
        arg->homepage[0] = 0;
    } else {
        ov00_0220A348(arg->homepage, info->homepage, 76);
    }
    arg->icquin = info->icquin;
    ov00_0220A348(arg->zipcode, info->zipcode, 11);
    ov00_0220A348(arg->countrycode, info->countrycode, 3);
    arg->longitude = info->longitude;
    arg->latitude = info->latitude;
    if (!info->place) {
        arg->place[0] = 0;
    } else {
        ov00_0220A348(arg->place, info->place, 128);
    }
    arg->birthday = info->birthday;
    arg->birthmonth = info->birthmonth;
    arg->birthyear = info->birthyear;
    arg->sex = info->sex;
    arg->publicmask = info->publicmask;
    if (!info->aimname) {
        arg->aimname[0] = 0;
    } else {
        ov00_0220A348(arg->aimname, info->aimname, 51);
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
int ov00_0220484C(GpConnection **connection, GpOperation *operation, char *input) {
    GpInfo info;
    char homepage[76], buffer[64], nick[31], uniquenick[21], email[51], firstname[31], lastname[31], aimname[51];
    GpProfile *profile;
    GpConnection *iconnection = *connection;
    GpPeer *peer;
    int cache, result, profileId;
    GpCallback callback;
    GpInfoArg *arg;
    if (ov00_0220A3C8(connection, input, 1)) {
        return 4;
    }
    if (strncmp(input, ov00_02218B74, 4)) {
        GP_ERROR(connection, 1, ov00_02218B7C);
    }
    if (!ov00_0220A4A0(input, ov00_02218BAC, buffer, 64)) {
        GP_ERROR(connection, 1, ov00_02218B7C);
    }
    profileId = atol(buffer);
    if (profileId <= 0) {
        __msl_assertion_failed(ov00_02218BB8, ov00_02218B40, ov00_02218B2C, 346);
    }
    ov00_02207DD0(connection, profileId, &profile);
    memset(&info, 0, sizeof(info));
    info.nick = nick;
    info.uniquenick = uniquenick;
    info.email = email;
    info.firstname = firstname;
    info.lastname = lastname;
    info.homepage = homepage;
    info.aimname = aimname;
    if (!ov00_0220A4A0(input, ov00_02218BC8, info.nick, 31)) {
        info.nick[0] = 0;
    }
    if (!ov00_0220A4A0(input, ov00_02218BD0, info.uniquenick, 21)) {
        info.uniquenick[0] = 0;
    }
    if (!ov00_0220A4A0(input, ov00_02218BE0, info.email, 51)) {
        info.email[0] = 0;
    }
    if (!ov00_0220A4A0(input, ov00_02218BE8, info.firstname, 31)) {
        info.firstname[0] = 0;
    }
    if (!ov00_0220A4A0(input, ov00_02218BF4, info.lastname, 31)) {
        info.lastname[0] = 0;
    }
    if (!ov00_0220A4A0(input, ov00_02218C00, buffer, 64)) {
        info.icquin = -1;
    } else {
        info.icquin = atol(buffer);
    }
    if (!ov00_0220A4A0(input, ov00_02218C0C, info.homepage, 76)) {
        info.homepage[0] = 0;
    }
    if (!ov00_0220A4A0(input, ov00_02218C18, info.zipcode, 11)) {
        info.zipcode[0] = 0;
    }
    if (!ov00_0220A4A0(input, ov00_02218C24, info.countrycode, 3)) {
        info.countrycode[0] = 0;
    }
    if (!ov00_0220A4A0(input, ov00_02218C34, buffer, 64)) {
        info.longitude = 0;
    } else {
        info.longitude = atod(buffer);
    }
    if (!ov00_0220A4A0(input, ov00_02218C3C, buffer, 64)) {
        info.latitude = 0;
    } else {
        info.latitude = atod(buffer);
    }
    if (!ov00_0220A4A0(input, ov00_02218C44, info.place, 128)) {
        info.place[0] = 0;
    }
    if (!ov00_0220A4A0(input, ov00_02218C4C, buffer, 64)) {
        info.birthday = 0;
        info.birthmonth = 0;
        info.birthyear = 0;
    } else {
        result = ov00_022045AC(connection, atol(buffer), &info.birthday, &info.birthmonth, &info.birthyear);
        if (result) {
            return result;
        }
    }
    if (!ov00_0220A4A0(input, ov00_02218C58, buffer, 64)) {
        info.sex = 1282;
    } else if (buffer[0] == '0') {
        info.sex = 1280;
    } else if (buffer[0] == '1') {
        info.sex = 1281;
    } else {
        info.sex = 1282;
    }
    if (!ov00_0220A4A0(input, ov00_02218C60, buffer, 64)) {
        info.publicmask = -1;
    } else {
        info.publicmask = atol(buffer);
    }
    if (!ov00_0220A4A0(input, ov00_02218C68, info.aimname, 51)) {
        info.aimname[0] = 0;
    }
    if (!ov00_0220A4A0(input, ov00_02218C70, buffer, 64)) {
        info.pic = 0;
    } else {
        info.pic = atol(buffer);
    }
    if (!ov00_0220A4A0(input, ov00_02218C78, buffer, 64)) {
        info.occupationid = 0;
    } else {
        info.occupationid = atol(buffer);
    }
    if (!ov00_0220A4A0(input, ov00_02218C80, buffer, 64)) {
        info.industryid = 0;
    } else {
        info.industryid = atol(buffer);
    }
    if (!ov00_0220A4A0(input, ov00_02218C88, buffer, 64)) {
        info.incomeid = 0;
    } else {
        info.incomeid = atol(buffer);
    }
    if (!ov00_0220A4A0(input, ov00_02218C90, buffer, 64)) {
        info.marriedid = 0;
    } else {
        info.marriedid = atol(buffer);
    }
    if (!ov00_0220A4A0(input, ov00_02218C98, buffer, 64)) {
        info.childcount = 0;
    } else {
        info.childcount = atol(buffer);
    }
    if (!ov00_0220A4A0(input, ov00_02218CA0, buffer, 64)) {
        info.interests1 = 0;
    } else {
        info.interests1 = atol(buffer);
    }
    if (!ov00_0220A4A0(input, ov00_02218CA8, buffer, 64)) {
        info.ownership1 = 0;
    } else {
        info.ownership1 = atol(buffer);
    }
    if (!ov00_0220A4A0(input, ov00_02218CB0, buffer, 64)) {
        info.conntypeid = 0;
    } else {
        info.conntypeid = atol(buffer);
    }
    if (!ov00_0220A4A0(input, ov00_02218CB8, buffer, 64)) {
        GP_ERROR(connection, 1, ov00_02218B7C);
    }
    peer = iconnection->peers;
    cache = iconnection->cacheProfileInfo;
    while (peer) {
        if (peer->profile == profileId && peer->state == 101) {
            if (!profile) {
                profile = ov00_02207CEC(connection, profileId);
            }
            peer->state = 102;
            cache = 1;
        }
        peer = peer->next;
    }
    if (!profile && iconnection->cacheProfileInfo) {
        profile = ov00_02207CEC(connection, profileId);
    }
    if (cache) {
        ov00_021EC3D8(profile->peerSig);
        profile->peerSig = 0;
        profile->peerSig = ov00_021FF3D4(buffer);
    }
    if (iconnection->cacheProfileInfo) {
        ov00_0220608C(connection, profile, &info);
    }
    callback.callback = operation->callback.callback;
    callback.param = operation->callback.param;
    if (callback.callback) {
        arg = ov00_021EC3A8(sizeof(GpInfoArg));
        if (!arg) {
            ov00_0220A890(connection, ov00_02218CC0);
            return 1;
        }
        ov00_02204654(&info, arg);
        arg->result = 0;
        arg->profile = profileId;
        result = ov00_02202A84(connection, callback, arg, operation, 0);
        if (result) {
            return result;
        }
    }
    ov00_022066D0(connection, operation);
    return 0;
}
