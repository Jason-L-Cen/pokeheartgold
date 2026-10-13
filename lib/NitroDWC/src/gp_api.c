#pragma force_active on
#include "global.h"

#include "dwc/gp_internal.h"

// The code below is compiled as ARM (32-bit) code.
#include <nitro/code32.h>

// The end of the platform layer ("nonport.c": socket options / names, ticks, the seeded random generator,
// base64) and the presence connection API ("gp.c": checks around the gpi* functions; with simulation set
// every call succeeds without network traffic).

extern const char ov00_02217E5C[]; // "time"
extern const char ov00_02217E74[]; // "OS_IsTickAvailable() == TRUE"
extern const char ov00_02217E94[]; // "nonport.c"
extern const char ov00_02217EAC[]; // "gpGetBuddyStatus"
extern const char ov00_02217EC0[]; // "Invalid func."
extern const char ov00_02217ED0[]; // "No callback."
extern char ov00_02217EE0[];       // ""
extern const char ov00_02217EE4[]; // "The connection has already been disconnected."
extern const char ov00_02217F14[]; // "Invalid reason."
extern const char ov00_02217F24[]; // "\\addbuddy\\"
extern const char ov00_02217F30[]; // "\\sesskey\\"
extern const char ov00_02217F3C[]; // "\\newprofileid\\"
extern const char ov00_02217F4C[]; // "\\reason\\"
extern const char ov00_02217F58[]; // "\\final\\"
extern const char ov00_02217F60[]; // "Invalid status."
extern const char ov00_02217F70[]; // "Invalid index."
extern const char ov00_02217F80[]; // "buddyStatus"
extern const char ov00_02217F8C[]; // "gp.c"
extern const char ov00_02217F94[]; // "Invalid statusString."
extern const char ov00_02217FAC[]; // "Invalid locationString."
extern const char ov00_02217FC4[]; // "\\status\\"
extern const char ov00_02217FD0[]; // "\\statstring\\"
extern const char ov00_02217FE0[]; // "\\locstring\\"
extern char ov00_02216140[];       // the 3 extra characters (62, 63, padding) of base64 encoding 1
extern char ov00_02216144[];       // ... of encoding 2
extern char ov00_02216148[];       // ... of the standard encoding

typedef struct SocSockAddr {
    u8 len;
    u8 family;
    u8 port[2];
    u8 addr[4];
} SocSockAddr;

typedef struct Ov00SocketState {
    int unk0;
    int lastError;
    char *hName;
    char **hAliases;
    s16 hAddrType;
    s16 hLength;
    char **hAddrList;
    u32 addr;
    u8 pad1C[0x2C - 0x1C];
    char *addrList[2];
} Ov00SocketState;

typedef struct Ov00DwcState {
    int state;
} Ov00DwcState;

typedef void (*GpApiCallback)(GpConnection **connection, void *arg, void *param);

// Status of a buddy as gpGetBuddyStatus returns it.
typedef struct GpBuddyStatusInfo {
    int profile;
    int status;
    char statusString[0x100];
    char locationString[0x100];
    int ip;
    int port;
} GpBuddyStatusInfo;

// Callback arguments given when simulating.
typedef struct GpSimulatedConnectArg {
    int result;
    int profile;
    int unk8;
    int unkC;
    int unk10;
    int unk14;
    int unk18;
    int unk1C;
} GpSimulatedConnectArg;

typedef struct GpSimulatedSearchArg {
    int unk0;
    int unk4;
    int more;
    int unkC;
} GpSimulatedSearchArg;

typedef struct GpSimulatedInfoArg {
    int data[0x81];
} GpSimulatedInfoArg;

extern Ov00SocketState ov00_0221B1AC;
extern u32 ov00_02217E58; // random generator state
extern Ov00DwcState ov00_0221B0D0;

extern void *memset(void *, int, u32);
extern int strcmp(const char *, const char *);
extern void __msl_assertion_failed(const char *cond, const char *file, const char *func, int line);
extern void DWCi_GsFree(void *p);
extern int SOC_GetSockName(int, SocSockAddr *);
extern int SOC_InetAtoN(const char *, u32 *);
extern int gsSocketCheckResult(int ret, int err);

int gsSocketGetSockOpt(int sock, int level, int opt, void *val, int *len);
int gsSocketSetSockOpt(int sock, int level, int opt, const void *val, int len);
int gsSocketGetSockName(int sock, SocSockAddr *addr, int *len);
u32 gsSocketInetAddr(const char *s);
int gsSocketGetLastError(void);
u32 current_time_seconds(u32 *out);
u32 Util_RandNextState(u32 x);
u32 Util_RandNext(void);
void Util_RandSeed(u32 seed);
int Util_RandInt(int lo, int hi);
void TripToQuart(const char *in, char *out, int len);
void B64Encode(const char *in, char *out, int len, int type);
int gpInitialize(GpConnection **connection, int productId, int namespaceId);
void gpDestroy(GpConnection **connection);
int gpProcess(GpConnection **connection);
int gpSetCallback(GpConnection **connection, int func, void *callback, void *param);
int gpConnectPreAuthenticatedA(GpConnection **connection, char *authtoken, char *partnerChallenge, int firewall, int blocking, GpApiCallback callback, void *param);
void gpDisconnect(GpConnection **connection);
int gpProfileSearchA(GpConnection **connection, char *nick, char *uniquenick, char *email, char *firstname, char *lastname, int icquin, int blocking, GpApiCallback callback, void *param);
int gpGetInfo(GpConnection **connection, int profile, int checkCache, int blocking, GpApiCallback callback, void *param);
int gpSetInfosA(GpConnection **connection, int info, const char *value);
int gpSendBuddyRequestA(GpConnection **connection, int profile, const char *reason);
int gpAuthBuddyRequest(GpConnection **connection, int profile);
int gpDenyBuddyRequest(GpConnection **connection, int profileId);
int gpGetNumBuddies(GpConnection **connection, int *numBuddies);
int gpGetBuddyStatus(GpConnection **connection, int index, GpBuddyStatusInfo *status);
int gpGetBuddyIndex(GpConnection **connection, int profileId, int *index);
int gpIsBuddy(GpConnection **connection, int profileId);
int gpDeleteBuddy(GpConnection **connection, int profile);
int gpSetStatusA(GpConnection **connection, int status, const char *statusString, const char *locationString);

// Turns backslashes into slashes (they delimit the "\key\value" messages).
static inline void FixSlashes(char *s) {
    char *p;
    if (*s) {
        p = s;
        do {
            if (*p == '\\') {
                *p = '/';
            }
        } while (*++p);
    }
}

int gsSocketGetSockOpt(int sock, int level, int opt, void *val, int *len) {
    MI_CpuFill8(val, 0, *len);
    return gsSocketCheckResult(0, -1);
}

int gsSocketSetSockOpt(int sock, int level, int opt, const void *val, int len) {
    return gsSocketCheckResult(0, -1);
}

int gsSocketGetSockName(int sock, SocSockAddr *addr, int *len) {
    int ret;
    addr->len = *len;
    ret = SOC_GetSockName(sock, addr);
    *len = addr->len;
    return gsSocketCheckResult(ret, -1);
}

u32 gsSocketInetAddr(const char *s) {
    u32 addr;
    if (SOC_InetAtoN(s, &addr) == 0) {
        return -1;
    }
    return addr;
}

int gsSocketGetLastError(void) {
    return ov00_0221B1AC.lastError;
}

// Seconds since start-up ("time").
u32 current_time_seconds(u32 *out) {
    u32 t;
    if (OS_IsTickAvailable() != TRUE) {
        __msl_assertion_failed(ov00_02217E74, ov00_02217E94, ov00_02217E5C, 0x667);
    }
    t = (u32)((OS_GetTick() * 64) / 33514000);
    if (out) {
        *out = t;
    }
    return t;
}

// Next state of the 16807 * x mod (2^31 - 1) generator.
u32 Util_RandNextState(u32 x) {
    u32 lo = (x & 0xffff) * 16807;
    u32 hi = (x >> 16) * 16807;
    lo += (hi & 0x7fff) << 16;
    if (lo > 0x7fffffff) {
        lo &= 0x7fffffff;
        lo++;
    }
    lo += hi >> 15;
    if (lo > 0x7fffffff) {
        lo &= 0x7fffffff;
        lo++;
    }
    return lo;
}

u32 Util_RandNext(void) {
    return ov00_02217E58 = Util_RandNextState(ov00_02217E58);
}

void Util_RandSeed(u32 seed) {
    ov00_02217E58 = seed ? (seed & 0x7fffffff) : 1;
}

// Random number in [lo, hi).
int Util_RandInt(int lo, int hi) {
    int range = hi - lo;
    int r;
    if (range == 0) {
        return lo;
    }
    r = (int)Util_RandNext() % range;
    return r + lo;
}

// Splits up to 3 bytes into 4 6-bit values.
void TripToQuart(const char *in, char *out, int len) {
    u8 tmp[3];
    int i;
    for (i = 0; i < len; i++) {
        tmp[i] = in[i];
    }
    for (; i < 3; i++) {
        tmp[i] = 0;
    }
    out[0] = tmp[0] >> 2;
    out[1] = ((tmp[0] & 3) << 4) | (tmp[1] >> 4);
    out[2] = ((tmp[1] & 0xf) << 2) | (tmp[2] >> 6);
    out[3] = tmp[2] & 0x3f;
}

// Base64-encodes len bytes of in; type selects the characters for 62, 63 and the padding.
void B64Encode(const char *in, char *out, int len, int type) {
    char *tbl;
    char *start = out;
    int remaining = len;
    char *pad;
    int mod;
    int c;
    switch (type) {
    case 1:
        tbl = ov00_02216140;
        break;
    case 2:
        tbl = ov00_02216144;
        break;
    default:
        tbl = ov00_02216148;
        break;
    }
    while (remaining > 0) {
        TripToQuart(in, out, remaining < 3 ? remaining : 3);
        remaining -= 3;
        out += 4;
        in += 3;
    }
    mod = len % 3;
    pad = out;
    if (mod == 1) {
        pad = out - 2;
    } else if (mod == 2) {
        pad = out - 1;
    }
    *out = 0;
    while (out > start) {
        out--;
        if (out >= pad) {
            *out = tbl[2];
        } else {
            c = *out;
            if (c <= 25) {
                *out = c + 'A';
            } else if (c <= 51) {
                *out = c + 'G';
            } else if (c <= 61) {
                *out = c - 4;
            } else if (c == 62) {
                *out = tbl[0];
            } else if (c == 63) {
                *out = tbl[1];
            }
        }
    }
}

// Creates a connection (needs the DWC state to be 1).
int gpInitialize(GpConnection **connection, int productId, int namespaceId) {
    if (ov00_0221B0D0.state != 1) {
        return 2;
    }
    if (connection == 0) {
        return 2;
    }
    return gpiInitialize(connection, productId, namespaceId);
}

void gpDestroy(GpConnection **connection) {
    if (!connection || !*connection) {
        return;
    }
    gpiDestroy(connection);
}

// gpProcess
int gpProcess(GpConnection **connection) {
    if (!connection || !*connection) {
        return 2;
    }
    if ((*connection)->simulation) {
        return 0;
    }
    return gpiProcess(connection, 0);
}

// Sets one of the GP_CALLBACK_* callbacks.
int gpSetCallback(GpConnection **connection, int func, void *callback, void *param) {
    GpConnection *iconnection;
    if (!connection || !(iconnection = *connection)) {
        return 2;
    }
    if (func < 0 || func >= GP_CALLBACK_COUNT) {
        gpiSetErrorString(connection, ov00_02217EC0);
        return 2;
    }
    iconnection->callbacks[func].callback = callback;
    iconnection->callbacks[func].param = param;
    return 0;
}

// Connects with an authentication token.
int gpConnectPreAuthenticatedA(GpConnection **connection, char *authtoken, char *partnerChallenge, int firewall, int blocking, GpApiCallback callback, void *param) {
    GpConnection *iconnection;
    GpSimulatedConnectArg arg;
    if (!connection || !(iconnection = *connection)) {
        return 2;
    }
    if (!authtoken || !*authtoken) {
        return 2;
    }
    if (!partnerChallenge || !*partnerChallenge) {
        return 2;
    }
    if (!callback) {
        gpiSetErrorString(connection, ov00_02217ED0);
        return 2;
    }
    if (iconnection->simulation) {
        memset(&arg, 0, sizeof(arg));
        callback(connection, &arg, param);
        return 0;
    }
    return gpiConnect(connection, ov00_02217EE0, ov00_02217EE0, ov00_02217EE0, ov00_02217EE0, authtoken, partnerChallenge, 0, firewall, 0, blocking, callback, param);
}

// Disconnects.
void gpDisconnect(GpConnection **connection) {
    if (!connection || !*connection) {
        return;
    }
    if ((*connection)->simulation) {
        return;
    }
    gpiDisconnect(connection, 1);
    gpiReset(connection);
}

// Profile search.
int gpProfileSearchA(GpConnection **connection, char *nick, char *uniquenick, char *email, char *firstname, char *lastname, int icquin, int blocking, GpApiCallback callback, void *param) {
    GpConnection *iconnection;
    GpSimulatedSearchArg arg;
    if (!connection || !(iconnection = *connection)) {
        return 2;
    }
    if (!callback) {
        gpiSetErrorString(connection, ov00_02217ED0);
        return 2;
    }
    if (iconnection->simulation) {
        memset(&arg, 0, sizeof(arg));
        arg.more = 0x601;
        callback(connection, &arg, param);
        return 0;
    }
    return gpiProfileSearch(connection, nick, uniquenick, email, firstname, lastname, icquin, 0, blocking, callback, param);
}

// Gets the info of a profile.
int gpGetInfo(GpConnection **connection, int profile, int checkCache, int blocking, GpApiCallback callback, void *param) {
    GpConnection *iconnection;
    if (!connection || !(iconnection = *connection) || !profile) {
        return 2;
    }
    if (!callback) {
        gpiSetErrorString(connection, ov00_02217ED0);
        return 2;
    }
    if (iconnection->simulation) {
        GpSimulatedInfoArg arg = { 0 };
        callback(connection, &arg, param);
        return 0;
    }
    if (iconnection->connectState == GP_DISCONNECTED) {
        gpiSetErrorString(connection, ov00_02217EE4);
        return 2;
    }
    return gpiGetInfo(connection, profile, checkCache, blocking, callback, param);
}

// Sets a profile info field.
int gpSetInfosA(GpConnection **connection, int info, const char *value) {
    GpConnection *iconnection;
    if (!connection || !(iconnection = *connection)) {
        return 2;
    }
    if (iconnection->simulation) {
        return 0;
    }
    if (iconnection->connectState == GP_DISCONNECTED) {
        gpiSetErrorString(connection, ov00_02217EE4);
        return 2;
    }
    return gpiSetInfos(connection, info, value);
}

// Sends a buddy request ("\addbuddy\").
int gpSendBuddyRequestA(GpConnection **connection, int profile, const char *reason) {
    GpConnection *iconnection;
    char buf[0x401];
    if (!connection || !(iconnection = *connection)) {
        return 2;
    }
    if (iconnection->simulation) {
        return 0;
    }
    if (iconnection->connectState == GP_DISCONNECTED) {
        gpiSetErrorString(connection, ov00_02217EE4);
        return 2;
    }
    if (!reason) {
        gpiSetErrorString(connection, ov00_02217F14);
        return 2;
    }
    strzcpy(buf, reason, sizeof(buf));
    FixSlashes(buf);
    gpiAppendStringToBuffer(connection, &iconnection->outputBuffer, ov00_02217F24);
    gpiAppendStringToBuffer(connection, &iconnection->outputBuffer, ov00_02217F30);
    gpiAppendIntToBuffer(connection, &iconnection->outputBuffer, iconnection->sessKey);
    gpiAppendStringToBuffer(connection, &iconnection->outputBuffer, ov00_02217F3C);
    gpiAppendIntToBuffer(connection, &iconnection->outputBuffer, profile);
    gpiAppendStringToBuffer(connection, &iconnection->outputBuffer, ov00_02217F4C);
    gpiAppendStringToBuffer(connection, &iconnection->outputBuffer, buf);
    gpiAppendStringToBuffer(connection, &iconnection->outputBuffer, ov00_02217F58);
    return 0;
}

// Accepts a buddy request.
int gpAuthBuddyRequest(GpConnection **connection, int profile) {
    GpConnection *iconnection;
    if (!connection || !(iconnection = *connection)) {
        return 2;
    }
    if (iconnection->simulation) {
        return 0;
    }
    if (iconnection->connectState == GP_DISCONNECTED) {
        gpiSetErrorString(connection, ov00_02217EE4);
        return 2;
    }
    return gpiAuthBuddyRequest(connection, profile);
}

// Denies a buddy request.
int gpDenyBuddyRequest(GpConnection **connection, int profileId) {
    GpConnection *iconnection;
    GpProfile *profile;
    if (!connection || !(iconnection = *connection)) {
        return 2;
    }
    if (iconnection->simulation) {
        return 0;
    }
    if (iconnection->connectState == GP_DISCONNECTED) {
        gpiSetErrorString(connection, ov00_02217EE4);
        return 2;
    }
    if (!gpiGetProfile(connection, profileId, &profile)) {
        return 0;
    }
    profile->requestCount--;
    if (!iconnection->cacheProfileInfo && profile->requestCount <= 0) {
        DWCi_GsFree(profile->authSig);
        profile->authSig = NULL;
        if (gpiCanFreeProfile(profile)) {
            gpiRemoveProfile(connection, profile);
        }
    }
    return 0;
}

int gpGetNumBuddies(GpConnection **connection, int *numBuddies) {
    GpConnection *iconnection;
    if (!connection || !(iconnection = *connection)) {
        return 2;
    }
    if (iconnection->simulation) {
        *numBuddies = 0;
        return 0;
    }
    *numBuddies = iconnection->numBuddies;
    return 0;
}

// "gpGetBuddyStatus"
int gpGetBuddyStatus(GpConnection **connection, int index, GpBuddyStatusInfo *status) {
    GpConnection *iconnection;
    GpProfile *profile;
    GpBuddyStatus *buddyStatus;
    int num;
    if (!connection || !(iconnection = *connection)) {
        return 2;
    }
    if (iconnection->simulation) {
        memset(status, 0, sizeof(GpBuddyStatusInfo));
        return 0;
    }
    if (!status) {
        gpiSetErrorString(connection, ov00_02217F60);
        return 2;
    }
    num = iconnection->numBuddies;
    if (index < 0 || index >= num) {
        gpiSetErrorString(connection, ov00_02217F70);
        return 2;
    }
    profile = gpiFindBuddy(connection, index);
    if (!profile) {
        gpiSetErrorString(connection, ov00_02217F70);
        return 2;
    }
    buddyStatus = profile->buddyStatus;
    if (!buddyStatus) {
        __msl_assertion_failed(ov00_02217F80, ov00_02217F8C, ov00_02217EAC, 0x5CF);
    }
    status->profile = profile->profileId;
    status->status = buddyStatus->status;
    if (!buddyStatus->statusString) {
        buddyStatus->statusString[0] = 0;
    } else {
        strzcpy(status->statusString, buddyStatus->statusString, 0x100);
    }
    if (!buddyStatus->locationString) {
        buddyStatus->locationString[0] = 0;
    } else {
        strzcpy(status->locationString, buddyStatus->locationString, 0x100);
    }
    status->ip = buddyStatus->ip;
    status->port = buddyStatus->port;
    return 0;
}

// The buddy index of a profile, -1 if it is no buddy.
int gpGetBuddyIndex(GpConnection **connection, int profileId, int *index) {
    GpConnection *iconnection;
    GpProfile *profile;
    if (!connection || !(iconnection = *connection)) {
        return 2;
    }
    if (iconnection->simulation) {
        *index = 0;
        return 0;
    }
    if (gpiGetProfile(connection, profileId, &profile) && profile->buddyStatus) {
        *index = profile->buddyStatus->buddyIndex;
    } else {
        *index = -1;
    }
    return 0;
}

// Whether the profile is a buddy.
int gpIsBuddy(GpConnection **connection, int profileId) {
    GpConnection *iconnection;
    GpProfile *profile;
    if (!connection || !(iconnection = *connection)) {
        return 0;
    }
    if (iconnection->simulation) {
        return 0;
    }
    if (gpiGetProfile(connection, profileId, &profile) && profile->buddyStatus) {
        return 1;
    }
    return 0;
}

// Deletes a buddy.
int gpDeleteBuddy(GpConnection **connection, int profile) {
    GpConnection *iconnection;
    int result;
    if (!connection || !(iconnection = *connection)) {
        return 2;
    }
    if (iconnection->simulation) {
        return 0;
    }
    if (iconnection->connectState == GP_DISCONNECTED) {
        gpiSetErrorString(connection, ov00_02217EE4);
        return 2;
    }
    result = gpiDeleteBuddy(connection, profile);
    if (result) {
        return result;
    }
    return 0;
}

// Sets the own status ("\status\"), unless it did not change.
int gpSetStatusA(GpConnection **connection, int status, const char *statusString, const char *locationString) {
    GpConnection *iconnection;
    char statBuf[0x100];
    char locBuf[0x100];
    if (!connection || !(iconnection = *connection)) {
        return 2;
    }
    if (iconnection->simulation) {
        return 0;
    }
    if (iconnection->connectState == GP_DISCONNECTED) {
        gpiSetErrorString(connection, ov00_02217EE4);
        return 2;
    }
    if (!statusString) {
        gpiSetErrorString(connection, ov00_02217F94);
        return 2;
    }
    if (!locationString) {
        gpiSetErrorString(connection, ov00_02217FAC);
        return 2;
    }
    strzcpy(statBuf, statusString, 0x100);
    FixSlashes(statBuf);
    strzcpy(locBuf, locationString, 0x100);
    FixSlashes(locBuf);
    if (status == iconnection->lastStatus && !strcmp(statBuf, iconnection->lastStatusString) && !strcmp(locBuf, iconnection->lastLocationString)) {
        return 0;
    }
    iconnection->lastStatus = status;
    strzcpy(iconnection->lastStatusString, statBuf, 0x100);
    strzcpy(iconnection->lastLocationString, locBuf, 0x100);
    gpiAppendStringToBuffer(connection, &iconnection->outputBuffer, ov00_02217FC4);
    gpiAppendIntToBuffer(connection, &iconnection->outputBuffer, status);
    gpiAppendStringToBuffer(connection, &iconnection->outputBuffer, ov00_02217F30);
    gpiAppendIntToBuffer(connection, &iconnection->outputBuffer, iconnection->sessKey);
    gpiAppendStringToBuffer(connection, &iconnection->outputBuffer, ov00_02217FD0);
    gpiAppendStringToBuffer(connection, &iconnection->outputBuffer, statBuf);
    gpiAppendStringToBuffer(connection, &iconnection->outputBuffer, ov00_02217FE0);
    gpiAppendStringToBuffer(connection, &iconnection->outputBuffer, locBuf);
    gpiAppendStringToBuffer(connection, &iconnection->outputBuffer, ov00_02217F58);
    return 0;
}
