#pragma force_active on
#include <stdarg.h>
#include <string.h>

#include "global.h"

#include "dwc/gp_internal.h"
#include "dwc/persist_internal.h"

// The code below is compiled as ARM (32-bit) code.
#include <nitro/code32.h>

// The last two presence connection error setters, the named value buckets ("gbucket.c"), and the
// persistent data client: XOR-scrambled "\final\"-terminated replies to authentication and get / set
// data requests, dispatched to the request callbacks; game and per player / per team values.

extern const char ov00_02219888[]; // "DoFind"
extern const char ov00_02219890[]; // "BucketNew"
extern const char ov00_0221989C[]; // "BucketConcat"
extern const char ov00_022198AC[]; // "set"
extern const char ov00_022198B0[]; // "gbucket.c"
extern const char ov00_022198BC[]; // "pbucket->type == bt_string"
extern char ov00_02219924[];       // reply scrambling key
extern char ov00_02219934[];       // error message scrambling key
extern char ov00_02219944[];       // ""
extern char ov00_02219948[];       // "\\"
extern char ov00_0221994C[];       // "pauthr"
extern char ov00_02219954[];       // "lid"
extern char ov00_02219958[];       // "errmsg"
extern char ov00_02219960[];       // "getpidr"
extern char ov00_02219968[];       // "getpdr"
extern char ov00_02219970[];       // "pid"
extern char ov00_02219974[];       // "mod"
extern char ov00_02219978[];       // "length"
extern char ov00_02219980[];       // "\\data\\"
extern char ov00_02219988[];       // "setpdr"
extern char ov00_02219990[];       // "\\pauthr\\"
extern char ov00_0221999C[];       // "\\getpidr\\"
extern char ov00_022199A8[];       // "\\getpdr\\"
extern char ov00_022199B4[];       // "\\setpdr\\"
extern char ov00_022199C0[];       // "%s_t%d"
extern char ov00_022199C8[];       // "%s_%d"

typedef struct Buf256 {
    char s[0x100];
} Buf256;

typedef struct Buf16 {
    char s[0x10];
} Buf16;

extern Buf16 ov00_02216164;  // scrambled error message
extern Buf256 ov00_022162E7; // "\\"
extern PersistSocket ov00_022198D8;
extern u8 ov00_0221B1D8[24];
extern BucketGlobals ov00_0221B1F0;
extern PersistWork ov00_0221B200;

extern int sprintf(char *, const char *, ...);
extern int strncmp(const char *, const char *, size_t);
extern long atol(const char *);
extern void __msl_assertion_failed(const char *, const char *, const char *, int);
extern void *DWCi_GsMalloc(u32 size);
extern void *DWCi_GsRealloc(void *p, u32 size);
extern void DWCi_GsFree(void *p);
extern void ArrayFree(void *arr);
extern void *ArrayNth(void *arr, int idx);
extern void ArrayDeleteAt(void *arr, int idx);
extern void TableEnter(void *table, void *elem);
extern void *TableLookup(void *table, void *key);
extern char *goastrdup(const char *s);
extern int CanReceiveOnSocket(int s);
extern void gsSocketClose(int s);
extern void gsSocketShutdown(int s, int how);
extern int gsSocketRecv(int s, char *buf, int len, int flags);
extern int ArrayLength(void *arr);

void gpiSetError(GpConnection **connection, int code, const char *message) {
    GpConnection *iconnection = *connection;
    strzcpy(iconnection->errorString, message, 0x100);
    iconnection->errorCode = code;
}

void gpiSetErrorString(GpConnection **connection, const char *message) {
    strzcpy((*connection)->errorString, message, 0x100);
}

void *BucketNew(BucketSet *set, const char *name, int type, void *value) {
    Bucket bucket;
    if (!set) {
        set = ov00_0221B1F0.defaultSet;
    }
    if (!set) {
        __msl_assertion_failed(ov00_022198AC, ov00_022198B0, ov00_02219890, 0x78);
    }
    bucket.name = goastrdup(name);
    bucket.type = type;
    bucket.val.i = 0;
    bucket.count = 1;
    DoSet(&bucket, value);
    TableEnter(set->table, &bucket);
    return DoGet(DoFind(set, name));
}

void *BucketSetValue(BucketSet *set, const char *name, void *value) {
    Bucket *bucket = DoFind(set, name);
    if (!bucket) {
        return 0;
    }
    bucket->count = 0;
    return DoSet(bucket, value);
}

void *BucketAdd(BucketSet *set, const char *name, void *value) {
    Bucket *bucket = DoFind(set, name);
    if (!bucket) {
        return 0;
    }
    if (bucket->type == BUCKET_INT) {
        return DoSet(bucket, bint(*(int *)value + *(int *)DoGet(bucket)));
    }
    if (bucket->type == BUCKET_DOUBLE) {
        return DoSet(bucket, bdouble(*(double *)value + *(double *)DoGet(bucket)));
    }
    return BucketConcat(set, name, value);
}

void *BucketSub(BucketSet *set, const char *name, void *value) {
    Bucket *bucket = DoFind(set, name);
    if (!bucket) {
        return 0;
    }
    if (bucket->type == BUCKET_INT) {
        return DoSet(bucket, bint(*(int *)DoGet(bucket) - *(int *)value));
    }
    if (bucket->type == BUCKET_DOUBLE) {
        return DoSet(bucket, bdouble(*(double *)DoGet(bucket) - *(double *)value));
    }
    return DoGet(bucket);
}

void *BucketMult(BucketSet *set, const char *name, void *value) {
    Bucket *bucket = DoFind(set, name);
    if (!bucket) {
        return 0;
    }
    if (bucket->type == BUCKET_INT) {
        return DoSet(bucket, bint(*(int *)value * *(int *)DoGet(bucket)));
    }
    if (bucket->type == BUCKET_DOUBLE) {
        return DoSet(bucket, bdouble(*(double *)value * *(double *)DoGet(bucket)));
    }
    return DoGet(bucket);
}

void *BucketDiv(BucketSet *set, const char *name, void *value) {
    Bucket *bucket = DoFind(set, name);
    if (!bucket) {
        return 0;
    }
    if (bucket->type == BUCKET_INT) {
        return DoSet(bucket, bint(*(int *)DoGet(bucket) / *(int *)value));
    }
    if (bucket->type == BUCKET_DOUBLE) {
        return DoSet(bucket, bdouble(*(double *)DoGet(bucket) / *(double *)value));
    }
    return DoGet(bucket);
}

void *BucketConcat(BucketSet *set, const char *name, void *value) {
    Bucket *bucket = DoFind(set, name);
    char *old;
    char *buf;
    if (!bucket) {
        return 0;
    }
    if (bucket->type != BUCKET_STRING) {
        __msl_assertion_failed(ov00_022198BC, ov00_022198B0, ov00_0221989C, 0xd1);
    }
    old = DoGet(bucket);
    buf = DWCi_GsMalloc(strlen(old) + strlen(value) + 1);
    strcpy(buf, old);
    strcat(buf, value);
    DoSet(bucket, buf);
    DWCi_GsFree(buf);
    return DoGet(bucket);
}

void *BucketAvg(BucketSet *set, const char *name, void *value) {
    Bucket *bucket = DoFind(set, name);
    int n;
    if (!bucket) {
        return 0;
    }
    if (bucket->type == BUCKET_INT) {
        int *cur = DoGet(bucket);
        n = ++bucket->count;
        return DoSet(bucket, bint((*(int *)value + n * *cur) / n));
    }
    if (bucket->type == BUCKET_DOUBLE) {
        double *cur = DoGet(bucket);
        n = ++bucket->count;
        return DoSet(bucket, bdouble((*(double *)value + n * *cur) / n));
    }
    return DoGet(bucket);
}

int *bint(int value) {
    ov00_0221B1F0.intValue = value;
    return (int *)(ov00_0221B1D8 + 24);
}

double *bdouble(double value) {
    ov00_0221B1F0.doubleValue = value;
    return &ov00_0221B1F0.doubleValue;
}

// Turns the backslashes of s into slashes, so it can't break a "\key\value" string.
char *BucketFixBackslashes(char *s) {
    char *start = s;
    while (*s) {
        if (*s == '\\') {
            *s = '/';
        }
        s++;
    }
    return start;
}

void *DoSet(Bucket *bucket, void *value) {
    if (bucket->type == BUCKET_INT) {
        bucket->val.i = *(int *)value;
    } else if (bucket->type == BUCKET_DOUBLE) {
        bucket->val.d = *(double *)value;
    } else if (bucket->type == BUCKET_STRING) {
        if (bucket->val.s) {
            DWCi_GsFree(bucket->val.s);
        }
        bucket->val.s = value ? BucketFixBackslashes(goastrdup(value)) : 0;
    }
    return DoGet(bucket);
}

void *DoGet(Bucket *bucket) {
    if (!bucket) {
        return 0;
    }
    if (bucket->type == BUCKET_STRING) {
        return bucket->val.s;
    }
    return &bucket->val;
}

Bucket *DoFind(BucketSet *set, const char *name) {
    Bucket key;
    if (!set) {
        set = ov00_0221B1F0.defaultSet;
    }
    if (!set) {
        __msl_assertion_failed(ov00_022198AC, ov00_022198B0, ov00_02219888, 0x153);
    }
    key.name = name;
    return TableLookup(set->table, &key);
}

void CloseStatsConnection(void) {
    if (ov00_022198D8.socket != -1) {
        gsSocketShutdown(ov00_022198D8.socket, 2);
        gsSocketClose(ov00_022198D8.socket);
    }
    ov00_022198D8.socket = -1;
    FailAllRequests();
    if (ov00_0221B200.recvBuf) {
        DWCi_GsFree(ov00_0221B200.recvBuf);
        ov00_0221B200.recvBuf = 0;
        ov00_0221B200.recvCap = 0;
        ov00_0221B200.recvLen = 0;
    }
}

int IsStatsConnected(void) {
    return ov00_022198D8.socket != -1;
}

int PersistThink(void) {
    int n;
    int used;
    if (ov00_022198D8.socket == -1) {
        return 0;
    }
    if (ov00_0221B200.state != 5) {
        return 0;
    }
    while (SocketReadable(ov00_022198D8.socket)) {
        if (ov00_0221B200.recvCap - ov00_0221B200.recvLen < 0x80) {
            if (ov00_0221B200.recvCap < 0x100) {
                ov00_0221B200.recvCap = 0x100;
            } else {
                ov00_0221B200.recvCap = ov00_0221B200.recvCap * 2;
            }
            ov00_0221B200.recvBuf = DWCi_GsRealloc(ov00_0221B200.recvBuf, ov00_0221B200.recvCap + 1);
            if (!ov00_0221B200.recvBuf) {
                return 0;
            }
        }
        n = gsSocketRecv(ov00_022198D8.socket, ov00_0221B200.recvBuf + ov00_0221B200.recvLen, ov00_0221B200.recvCap - ov00_0221B200.recvLen, 0);
        if (n <= 0) {
            CloseStatsConnection();
            return 0;
        }
        ov00_0221B200.recvLen += n;
        ov00_0221B200.recvBuf[ov00_0221B200.recvLen] = 0;
        used = ProcessInBuffer(ov00_0221B200.recvBuf, ov00_0221B200.recvLen);
        if (used == ov00_0221B200.recvLen) {
            ov00_0221B200.recvLen = 0;
        } else {
            memmove(ov00_0221B200.recvBuf, ov00_0221B200.recvBuf + used, ov00_0221B200.recvLen - used);
            ov00_0221B200.recvLen -= used;
        }
    }
    if (ov00_022198D8.socket == -1) {
        return 0;
    }
    return 1;
}

// XOR (un)scrambling with the current key.
void xcode_buf(char *buf, int len) {
    int i;
    const char *k;
    k = ov00_022198D8.key;
    for (i = 0; i < len; i++) {
        buf[i] ^= *k;
        k++;
        if (*k == 0) {
            k = ov00_022198D8.key;
        }
    }
}

// Value of "\key\" in str, in one of two alternating buffers, or NULL.
char *value_for_key(const char *str, const char *key) {
    Buf256 search = ov00_022162E7;
    const char *p;
    char *ret;
    char *dst;
    ov00_0221B200.valueIndex ^= 1;
    strcat(search.s, key);
    strcat(search.s, ov00_02219948);
    p = strstr(str, search.s);
    if (!p) {
        return 0;
    }
    p += strlen(search.s);
    ret = ov00_0221B200.values[ov00_0221B200.valueIndex];
    dst = ret;
    while (*p != 0 && *p != '\\') {
        *dst++ = *p++;
    }
    *dst = 0;
    return ret;
}

char *value_for_key_safe(const char *str, const char *key) {
    char *r = value_for_key(str, key);
    if (!r) {
        r = ov00_02219944;
    }
    return r;
}

int SocketReadable(int socket) {
    return CanReceiveOnSocket(socket);
}

// Finds the "\final\" terminator.
char *FindFinal(char *buf, int len) {
    char *p;
    for (p = buf; p - buf < len - 6; p++) {
        if (p[0] == '\\' && p[1] == 'f' && p[2] == 'i' && p[3] == 'n' && p[4] == 'a' && p[5] == 'l' && p[6] == '\\') {
            return p;
        }
    }
    return 0;
}

int FindRequest(int type, int localId, int profileId) {
    int i;
    if (!ov00_0221B200.requests) {
        return -1;
    }
    for (i = 0; i < ArrayLength(ov00_0221B200.requests); i++) {
        PersistRequest *request = ArrayNth(ov00_0221B200.requests, i);
        if (request->type == type && request->localId == localId && request->profileId == profileId) {
            return i;
        }
    }
    return -1;
}

// "\pauthr\" reply
void ProcessPlayerAuth(char *buf, int len) {
    int profileId;
    int localId;
    int index;
    char *errmsg;
    profileId = atol(value_for_key_safe(buf, ov00_0221994C));
    localId = atol(value_for_key_safe(buf, ov00_02219954));
    errmsg = value_for_key_safe(buf, ov00_02219958);
    index = FindRequest(0, localId, 0);
    if (index == -1) {
        return;
    }
    ((PersistRequest *)ArrayNth(ov00_0221B200.requests, index))->profileId = profileId;
    CallReqCallback(index, profileId > 0, 0, errmsg, 0);
}

// "\getpidr\" reply
void ProcessGetPid(char *buf, int len) {
    int profileId = atol(value_for_key_safe(buf, ov00_02219960));
    int localId = atol(value_for_key_safe(buf, ov00_02219954));
    int index = FindRequest(3, localId, 0);
    PersistRequest *request;
    if (index == -1) {
        return;
    }
    request = ArrayNth(ov00_0221B200.requests, index);
    request->profileId = profileId;
    CallReqCallback(index, profileId > 0, 0, 0, 0);
}

// "\getpdr\" reply
void ProcessGetData(char *buf, int len) {
    int index;
    int length;
    int success;
    int modified;
    int profileId;
    int localId;
    char *p;
    char *data;
    success = atol(value_for_key_safe(buf, ov00_02219968));
    localId = atol(value_for_key_safe(buf, ov00_02219954));
    profileId = atol(value_for_key_safe(buf, ov00_02219970));
    modified = atol(value_for_key_safe(buf, ov00_02219974));
    index = FindRequest(1, localId, profileId);
    if (index != -1) {
        length = atol(value_for_key_safe(buf, ov00_02219978));
        p = strstr(buf, ov00_02219980);
        if (!p) {
            data = ov00_02219944;
            length = 0;
        } else {
            data = p + 6;
        }
        CallReqCallback(index, success, modified, data, length);
    }
}

// "\setpdr\" reply
void ProcessSetData(char *buf, int len) {
    int success = atol(value_for_key_safe(buf, ov00_02219988));
    int profileId = atol(value_for_key_safe(buf, ov00_02219970));
    int localId = atol(value_for_key_safe(buf, ov00_02219954));
    int modified = atol(value_for_key_safe(buf, ov00_02219974));
    int index = FindRequest(2, localId, profileId);
    if (index == -1) {
        return;
    }
    CallReqCallback(index, success, modified, 0, 0);
}

void ProcessStatement(char *buf, int len) {
    buf[len] = 0;
    if (strncmp(buf, ov00_02219990, 8) == 0) {
        ProcessPlayerAuth(buf, len);
    } else if (strncmp(buf, ov00_0221999C, 9) == 0) {
        ProcessGetPid(buf, len);
    } else if (strncmp(buf, ov00_0221999C, 9) == 0) {
        ProcessGetPid(buf, len);
    } else if (strncmp(buf, ov00_022199A8, 8) == 0) {
        ProcessGetData(buf, len);
    } else if (strncmp(buf, ov00_022199B4, 8) == 0) {
        ProcessSetData(buf, len);
    }
}

// Handles every complete reply in buf; returns the number of bytes used.
int ProcessInBuffer(char *buf, int len) {
    char *p;
    int orig;
    int n;
    orig = len;
    p = FindFinal(buf, len);
    while (len > 0 && p) {
        n = p - buf;
        ov00_022198D8.key = ov00_02219924;
        xcode_buf(buf, n);
        ProcessStatement(buf, n);
        len -= n + 7;
        buf = p + 7;
        if (len > 0) {
            p = FindFinal(buf, len);
        }
    }
    return orig - len;
}

// Calls the callback of request index and removes the request.
void CallReqCallback(int index, int success, int modified, char *data, int len) {
    PersistRequest *request;
    if (index < 0) {
        return;
    }
    if (index >= ArrayLength(ov00_0221B200.requests)) {
        return;
    }
    request = ArrayNth(ov00_0221B200.requests, index);
    if (request->callback) {
        switch (request->type) {
        case 0:
            ((void (*)(int, int, int, char *, int))request->callback)(request->localId, request->profileId, success, data, request->param);
            break;
        case 1:
            ((void (*)(int, int, int, int, int, int, char *, int, int))request->callback)(request->localId, request->profileId, request->persistType, request->index, success, modified, data, len, request->param);
            break;
        case 2:
            ((void (*)(int, int, int, int, int, int, int))request->callback)(request->localId, request->profileId, request->persistType, request->index, success, modified, request->param);
            break;
        case 3:
            ((void (*)(int, int, int, int))request->callback)(request->localId, request->profileId, success, request->param);
            break;
        }
    }
    ArrayDeleteAt(ov00_0221B200.requests, index);
}

// Fails every pending request with the "disconnected" message and frees the list.
void FailAllRequests(void) {
    int i;
    Buf16 message;
    Buf16 scrambled;
    if (!ov00_0221B200.requests) {
        return;
    }
    i = ArrayLength(ov00_0221B200.requests) - 1;
    if (i >= 0) {
        scrambled = ov00_02216164;
        do {
            message = scrambled;
            ov00_022198D8.key = ov00_02219934;
            xcode_buf(message.s, 15);
            CallReqCallback(i, 0, 0, message.s, 0);
        } while (--i >= 0);
    }
    ArrayFree(ov00_0221B200.requests);
    ov00_0221B200.requests = 0;
}

int GetTeamNumber(StatsGame *game, int index) {
    if (!game) {
        game = ov00_0221B200.currentGame;
    }
    if (!game) {
        return index;
    }
    return *(int *)ArrayNth(game->teams, index);
}

int GetPlayerNumber(StatsGame *game, int index) {
    if (!game) {
        game = ov00_0221B200.currentGame;
    }
    if (!game) {
        return index;
    }
    return *(int *)ArrayNth(game->players, index);
}

int BucketIntOp(StatsGame *game, const char *name, BucketGetter get, int value, ...) {
    void *r;
    if (!game) {
        game = ov00_0221B200.currentGame;
    }
    if (!game) {
        r = &value;
    } else {
        r = get(game->buckets, name, &value);
        if (!r) {
            r = BucketNew(game->buckets, name, BUCKET_INT, &value);
        }
    }
    return *(int *)r;
}

double BucketDoubleOp(StatsGame *game, const char *name, BucketGetter get, double value, ...) {
    void *r;
    if (!game) {
        game = ov00_0221B200.currentGame;
    }
    if (!game) {
        r = &value;
    } else {
        r = get(game->buckets, name, &value);
        if (!r) {
            r = BucketNew(game->buckets, name, BUCKET_DOUBLE, &value);
        }
    }
    return *(double *)r;
}

char *BucketStringOp(StatsGame *game, const char *name, BucketStringGetter get, char *value, int unused) {
    char *r;
    if (!game) {
        game = ov00_0221B200.currentGame;
    }
    if (!game) {
        return value;
    }
    r = get(game->buckets, name, value);
    if (r) {
        return r;
    }
    return BucketNew(game->buckets, name, BUCKET_STRING, value);
}

int TeamBucketIntOp(StatsGame *game, const char *name, BucketGetter get, int value, int index) {
    char buf[0x40];
    sprintf(buf, ov00_022199C0, name, GetTeamNumber(game, index));
    return BucketIntOp(game, buf, get, value, index);
}

double TeamBucketDoubleOp(StatsGame *game, const char *name, BucketGetter get, double value, int index) {
    char buf[0x40];
    sprintf(buf, ov00_022199C0, name, GetTeamNumber(game, index));
    return BucketDoubleOp(game, buf, get, value, index);
}

char *TeamBucketStringOp(StatsGame *game, const char *name, BucketStringGetter get, char *value, int index) {
    char buf[0x40];
    sprintf(buf, ov00_022199C0, name, GetTeamNumber(game, index));
    return BucketStringOp(game, buf, get, value, index);
}

int PlayerBucketIntOp(StatsGame *game, const char *name, BucketGetter get, int value, int index) {
    char buf[0x40];
    sprintf(buf, ov00_022199C8, name, GetPlayerNumber(game, index));
    return BucketIntOp(game, buf, get, value, index);
}

double PlayerBucketDoubleOp(StatsGame *game, const char *name, BucketGetter get, double value, int index) {
    char buf[0x40];
    sprintf(buf, ov00_022199C8, name, GetPlayerNumber(game, index));
    return BucketDoubleOp(game, buf, get, value, index);
}

char *PlayerBucketStringOp(StatsGame *game, const char *name, BucketStringGetter get, char *value, int index) {
    char buf[0x40];
    sprintf(buf, ov00_022199C8, name, GetPlayerNumber(game, index));
    return BucketStringOp(game, buf, get, value, index);
}
