#pragma force_active on
#include <string.h>

#include "global.h"

#include "dwc/sb_internal.h"

// The code below is compiled as ARM (32-bit) code.
#include <nitro/code32.h>

extern void __msl_assertion_failed(const char *expr, const char *file, const char *func, int line);
extern int strnicmp(const char *a, const char *b);
extern int strncmp(const char *a, const char *b, u32 n);
extern double atod(const char *str);
extern int sprintf(char *buf, const char *fmt, ...);
extern int rand(void);
extern void srand(unsigned seed);
extern u8 __lower_mapC[];

extern void *DWCi_GsMalloc(u32 size);
extern void DWCi_GsFree(void *p);
extern void *ArrayNew(int elemSize, int count, void *freeFunc);
extern void ArrayFree(void *array);
extern int ArrayLength(void *array);
extern void *ArrayNth(void *array, int index);
extern void ArrayAppend(void *array, void *elem);
extern void ArrayDeleteAt(void *array, int index);
extern void ArraySort(void *array, void *compare);
extern void ArrayClear(void *array);
extern void TableEnter(void *table, void *elem);
extern void TableRemove(void *table, void *elem);
extern void *TableLookup(void *table, void *elem);
extern u32 current_time(void);
extern void SocketStartUp(void);
extern char *goastrdup(const char *str);
extern int CanReceiveOnSocket(int socket);
extern int gsSocketCreate(int domain, int type, int protocol);
extern int gsSocketClose(int socket);
extern int gsSocketConnect(int socket, void *addr, int len);
extern int gsSocketRecv(int socket, void *buf, int len, int flags);
extern int gsSocketRecvFrom(int socket, void *buf, int len, int flags, void *addr, void *addrLen);
extern int gsSocketSend(int socket, const void *buf, int len, int flags);
extern u32 gsSocketInetAddr(const char *address);

typedef struct SockAddr {
    u8 len;
    u8 family;
    u16 port;
    u32 address;
} SockAddr;

typedef struct HostEnt {
    char *name;
    char **aliases;
    int type;
    u8 **addresses;
} HostEnt;

extern HostEnt *SOC_GetHostByName(const char *name);

extern int ov00_0221B0D0;
extern SBListGlobals ov00_0221B9DC;
extern char *ov00_0221A52C; // "Query Error: "
extern char ov00_0221A530[];
extern char ov00_0221A550[];
extern char ov00_0221A560[];
extern char ov00_0221A574[];
extern char ov00_0221A588[];
extern char ov00_0221A59C[];
extern char ov00_0221A5B8[];
extern char ov00_0221A5BC[];
extern char ov00_0221A5C8[];
extern char ov00_0221A5D8[];
extern char ov00_0221A5E8[];
extern const char ov00_0221A5FC[];
extern char ov00_0221A618[];
extern char ov00_0221A638[];
extern u8 ov00_0221A63C[];
extern char ov00_0221A644[];
extern char ov00_0221A650[];

void SBServerListAppendServer(SBServerList *list, SBServer *server);
void BufferAddNTS(char **cursor, const char *str, int *len);
void BufferAddByte(char **cursor, int value, int *len);
void BufferAddInt(u8 **cursor, u32 value, u32 *len);
void BufferAddData(char **cursor, const void *data, int size, int *len);
void InitCryptKey(SBServerList *list, s8 *data, int count);
int FullRulesPresent(char *data, int len);
int AllKeysPresent(SBServerList *list, u8 *data, int len);
void ParseServerIPPort(SBServerList *list, const u8 *data, int len, u32 *ip, u16 *port);
int ParseServer(SBServerList *list, SBServer *server, const u8 *data, int len, int popular);
int ProcessPushKeysMessage(SBServerList *list, u8 *data, int len);
int ProcessPlayerSearchMessage(SBServerList *list, u8 *data, int len);
int ProcessMaploopMessage(SBServerList *list, u8 *data, int len);
int ProcessDeleteServerMessage(SBServerList *list, u8 *data, int len);
int ProcessPushServerMessage(SBServerList *list, void *data, int len);

int IntKeyCompare(SBServer **a, SBServer **b) {
    SBServer *serverA = *(SBServer *volatile *)a; // volatile keeps the original load order
    SBServerList *list = ov00_0221B9DC.sortList;
    SBServer *serverB = *b;
    int valueA = SBServerGetIntValueA(serverA, list->sortKey, 0);
    int valueB = SBServerGetIntValueA(serverB, ov00_0221B9DC.sortList->sortKey, 0);
    int diff = valueA - valueB;
    if (!ov00_0221B9DC.sortList->sortAscending) {
        diff = -diff;
    }
    return diff;
}

int FloatKeyCompare(SBServer **a, SBServer **b) {
    SBServer *serverB = *b;
    double valueA = SBServerGetFloatValueA(*a, ov00_0221B9DC.sortList->sortKey, 0);
    double diff = valueA - SBServerGetFloatValueA(serverB, ov00_0221B9DC.sortList->sortKey, 0);
    if (!ov00_0221B9DC.sortList->sortAscending) {
        diff = 0 - diff;
    }
    if ((float)diff > 0) {
        return 1;
    }
    return -((float)diff < 0);
}

int StrCaseKeyCompare(SBServer **a, SBServer **b) {
    char *valueA = SBServerGetStringValueA(*a, ov00_0221B9DC.sortList->sortKey, ov00_0221A5B8);
    char *valueB = SBServerGetStringValueA(*b, ov00_0221B9DC.sortList->sortKey, ov00_0221A5B8);
    int diff = strcmp(valueA, valueB);
    if (!ov00_0221B9DC.sortList->sortAscending) {
        diff = -diff;
    }
    return diff;
}

int StrNoCaseKeyCompare(SBServer **a, SBServer **b) {
    char *valueA = SBServerGetStringValueA(*a, ov00_0221B9DC.sortList->sortKey, ov00_0221A5B8);
    char *valueB = SBServerGetStringValueA(*b, ov00_0221B9DC.sortList->sortKey, ov00_0221A5B8);
    int diff = strnicmp(valueA, valueB);
    if (!ov00_0221B9DC.sortList->sortAscending) {
        diff = -diff;
    }
    return diff;
}

void SBServerListSort(SBServerList *list, int ascending, const char *key, u32 type) {
    void *compare;
    switch (type) {
    case 0:
        compare = IntKeyCompare;
        break;
    case 1:
        compare = FloatKeyCompare;
        break;
    case 2:
        compare = StrCaseKeyCompare;
        break;
    case 3:
        compare = StrNoCaseKeyCompare;
        break;
    default:
        compare = StrNoCaseKeyCompare;
        break;
    }
    list->sortKey = key;
    list->sortAscending = ascending;
    ov00_0221B9DC.sortList = list;
    ArraySort(list->servers, compare);
}

void SBServerListAppendServer(SBServerList *list, SBServer *server) {
    ArrayAppend(list->servers, &server);
    list->callback(list, 0, server, list->param);
}

int SBServerListFindServer(SBServerList *list, SBServer *server) {
    int count = ArrayLength(list->servers);
    int i;
    for (i = 0; i < count; i++) {
        if (server == *(SBServer **)ArrayNth(list->servers, i)) {
            return i;
        }
    }
    return -1;
}

int SBServerListFindServerByIP(SBServerList *list, u32 ip, u16 port) {
    int count = ArrayLength(list->servers);
    SBServer *server;
    int i;
    for (i = 0; i < count; i++) {
        server = *(SBServer **)ArrayNth(list->servers, i);
        if (ip == SBServerGetPublicInetAddress(server) && port == SBServerGetPublicQueryPortNBO(server)) {
            return i;
        }
    }
    return -1;
}

void SBServerListAddToDeadList(SBServerList *list, SBServer *server) {
    if (!list->deadList) {
        SBServerSetNext(server, NULL);
    } else {
        SBServerSetNext(server, list->deadList);
    }
    list->deadList = server;
}

void SBServerListRemoveAt(SBServerList *list, int index) {
    SBServer *server = *(SBServer **)ArrayNth(list->servers, index);
    list->callback(list, 2, server, list->param);
    ArrayDeleteAt(list->servers, index);
    SBServerListAddToDeadList(list, server);
}

int SBServerListCount(SBServerList *list) {
    return ArrayLength(list->servers);
}

SBServer *SBServerListNth(SBServerList *list, int index) {
    return *(SBServer **)ArrayNth(list->servers, index);
}

void SBServerListFreeDeadList(SBServerList *list) {
    SBServer *server = list->deadList;
    SBServer *current;
    if (!server) {
        return;
    }
    current = server;
    while (server) {
        server = SBServerGetNext(server);
        SBServerFree(&current);
        current = server;
    }
    list->deadList = NULL;
}

void SBServerListClear(SBServerList *list) {
    int i;
    int count = ArrayLength(list->servers);
    for (i = 0; i < count; i++) {
        SBServer **server = ArrayNth(list->servers, i);
        SBServerListAddToDeadList(list, *server);
    }
    ArrayClear(list->servers);
    SBServerListFreeDeadList(list);
}

void SBServerListInitServers(SBServerList *list) {
    list->servers = ArrayNew(4, 100, NULL);
    list->deadList = NULL;
}

// Shared strings: a hash table of reference counted copies (SBRefStrHash)
typedef struct SBRefString {
    char *str;
    int refCount;
} SBRefString;

char *SBRefStr(SBServerList *list, const char *str) {
    SBRefString ref;
    SBRefString *found;
    ref.str = (char *)str;
    found = TableLookup(SBRefStrHash(list), &ref);
    if (found) {
        found->refCount++;
        return found->str;
    }
    ref.str = goastrdup(str);
    ref.refCount = 1;
    TableEnter(SBRefStrHash(list), &ref);
    return ref.str;
}

void SBReleaseStr(SBServerList *list, const char *str) {
    SBRefString ref;
    SBRefString *found;
    ref.str = (char *)str;
    found = TableLookup(SBRefStrHash(list), &ref);
    if (!found) {
        __msl_assertion_failed(ov00_0221A5BC, ov00_0221A5C8, ov00_0221A530, 244);
    }
    if (!found) {
        return;
    }
    if (--found->refCount != 0) {
        return;
    }
    TableRemove(SBRefStrHash(list), &ref);
}

// Length of the NUL-terminated string at str including the NUL, -1 if there is none within len bytes
int NTSLengthSB(const char *str, int len) {
    int i;
    for (i = 0; i < len; i++) {
        if (str[i] == 0) {
            return i + 1;
        }
    }
    return -1;
}

void SBServerListInit(SBServerList *list, const char *targetGameName, const char *gameName, const char *secretKey, int queryFromVersion, int force, SBListCallback callback, void *param) {
    if (!list) {
        __msl_assertion_failed(ov00_0221A5D8, ov00_0221A5C8, ov00_0221A560, 0x11C);
    }
    if (!force && ov00_0221B0D0 != 1) {
        return;
    }
    list->state = SB_LIST_DISCONNECTED;
    SBServerListInitServers(list);
    SBRefStrHash(list);
    strcpy(list->targetGameName, targetGameName);
    strcpy(list->gameName, gameName);
    strcpy(list->secretKey, secretKey);
    list->callback = callback;
    list->maploopCallback = NULL;
    if (!callback) {
        __msl_assertion_failed(ov00_0221A5E8, ov00_0221A5C8, ov00_0221A560, 0x132);
    }
    list->param = param;
    list->sortKey = ov00_0221A5B8;
    list->publicIp = 0;
    list->socket = -1;
    list->inBuffer = NULL;
    list->inBufferLen = 0;
    list->keyList = NULL;
    list->expectedElements = -1;
    list->numPopularValues = 0;
    list->alternateSourceIp = 0;
    list->queryFromVersion = queryFromVersion;
    SBSetLastListErrorPtr(list, ov00_0221A5B8);
    list->unk_5CC = 0;
    srand(current_time());
    SocketStartUp();
}

void ErrorDisconnect(SBServerList *list) {
    int len = list->inBufferLen;
    if (len > 0) {
        char *prefix = ov00_0221A52C;
        if ((u32)len > strlen(prefix)) {
            char *buffer = (char *)list->inBuffer;
            char *prefix2 = ov00_0221A52C;
            if (strncmp(buffer, prefix2, strlen(prefix2)) == 0) {
                SBSetLastListErrorPtr(list, buffer + strlen(prefix));
                list->callback(list, 5, ov00_0221B9D4, list->param);
            }
        }
    }
    list->callback(list, 4, ov00_0221B9D4, list->param);
    SBServerListDisconnect(list);
}

u32 ServerListStringHash(const char *str, u32 mod) {
    u32 hash = 0;
    for (; *str != 0; str++) {
        int c = *str;
        hash = 0x9CCF9319 * hash + ((c < 0 || c >= 0x80) ? c : __lower_mapC[c]);
    }
    return hash % mod;
}

// Opens the TCP connection to the master server (host selected by a hash of the game name)
int ServerListConnect(SBServerList *list) {
    struct {
        SockAddr address;
        char host[128];
    } f;

    int index = ServerListStringHash(list->targetGameName, 20);
    if (ov00_0221B9DC.masterHost) {
        strcpy(f.host, ov00_0221B9DC.masterHost);
    } else {
        sprintf(f.host, ov00_0221A5FC, list->targetGameName, index);
    }
    f.address.family = 2;
    f.address.port = 0xEE70;
    f.address.address = gsSocketInetAddr(f.host);
    if (f.address.address == (u32)-1) {
        HostEnt *host = SOC_GetHostByName(f.host);
        if (!host) {
            return 2;
        }
        memcpy(&f.address.address, host->addresses[0], 4);
    }
    if (list->socket == -1) {
        list->socket = gsSocketCreate(2, 1, 0);
        if (list->socket == -1) {
            return 1;
        }
    }
    if (gsSocketConnect(list->socket, &f.address, 8) == 0) {
        return 0;
    }
    gsSocketClose(list->socket);
    list->socket = -1;
    return 3;
}

void BufferAddNTS(char **cursor, const char *str, int *len) {
    int size;
    if (!str) {
        str = ov00_0221A5B8;
    }
    size = strlen(str) + 1;
    memcpy(*cursor, str, size);
    *len += size;
    *cursor += size;
}

void BufferAddByte(char **cursor, int value, int *len) {
    **cursor = value;
    (*len)++;
    (*cursor)++;
}

void BufferAddInt(u8 **cursor, u32 value, u32 *len) {
    memcpy(*cursor, &value, 4);
    *len += 4;
    *cursor += 4;
}

void BufferAddData(char **cursor, const void *data, int size, int *len) {
    memcpy(*cursor, data, size);
    *len += size;
    *cursor += size;
}

// Random printable challenge with a parity rule per character
void SetupListChallenge(SBServerList *list) {
    int i;
    int parity;
    list->challenge[0] = rand() % 93 + 33;
    parity = 0;
    i = 1;
    for (; i < 8; i++) {
        parity = ((list->challenge[0] & 1) ^ (parity ^ ((i ^ list->challenge[i - 1]) & 1))) ^ (list->challenge[0] < 0x4F ? 1 : 0) ^ (list->challenge[i - 1] < list->challenge[0] ? 1 : 0);
        list->challenge[i] = rand() % 93 + 33;
        if ((parity != 0 && (list->challenge[i] & 1) == 0) || (parity == 0 && (list->challenge[i] & 1) == 1)) {
            list->challenge[i]++;
        }
    }
}

// Sends to the master server, reconnecting once if the connection was lost
int SendWithRetry(SBServerList *list, const void *data, int size) {
    int sent, retries = 1, result;
    do {
        retries--;
        sent = gsSocketSend(list->socket, data, size, 0);
        if (sent > 0 || retries < 0) {
            break;
        }
        SBServerListDisconnect(list);
        result = SBServerListConnectAndQuery(list, NULL, NULL, 2, 0);
        if (result) {
            ErrorDisconnect(list);
            return result;
        }
    } while (retries >= 0);
    return sent <= 0 ? 3 : 0;
}

#pragma inlineintrinsics on

// Connects to the master server and sends the list request
int SBServerListConnectAndQuery(SBServerList *list, const char *keys, const char *filter, u32 options, int maxServers) {
    u8 buffer[0x300];
    int len;
    u8 *cursor;
    u16 swapped;
    int result;
    u32 swap;
    if (list->state != SB_LIST_DISCONNECTED) {
        __msl_assertion_failed(ov00_0221A618, ov00_0221A5C8, ov00_0221A59C, 522);
    }
    if (!keys) {
        keys = ov00_0221A5B8;
    }
    if (!filter) {
        filter = ov00_0221A5B8;
    }
    if (strlen(keys) > 256) {
        return 6;
    }
    if (strlen(filter) > 256) {
        return 6;
    }
    result = ServerListConnect(list);
    if (result) {
        return result;
    }
    list->queryOptions = options;
    SetupListChallenge(list);
    len = 2;
    cursor = buffer + 2;
    BufferAddByte((char **)&cursor, 0, &len);
    BufferAddByte((char **)&cursor, 1, &len);
    BufferAddByte((char **)&cursor, 3, &len);
    BufferAddInt(&cursor, list->queryFromVersion, (u32 *)&len);
    BufferAddNTS((char **)&cursor, list->targetGameName, &len);
    BufferAddNTS((char **)&cursor, list->gameName, &len);
    BufferAddData((char **)&cursor, list->challenge, 8, &len);
    BufferAddNTS((char **)&cursor, filter, &len);
    BufferAddNTS((char **)&cursor, keys, &len);
    swap = ((options << 24) & 0xFF000000) | (((options << 8) & 0xFF0000) | (((options >> 24) & 0xFF) | ((options >> 8) & 0xFF00)));
    BufferAddInt(&cursor, swap, (u32 *)&len);
    if (list->queryOptions & 8) {
        BufferAddInt(&cursor, list->alternateSourceIp, (u32 *)&len);
    }
    if (list->queryOptions & 0x80) {
        BufferAddInt(&cursor, maxServers, (u32 *)&len);
    }
    {
        u8 *src = (u8 *)&swapped;
        volatile u8 *dst = buffer;
        swapped = (((u16)len >> 8) & 0xFF) | (((u16)len << 8) & 0xFF00);
        memcpy((u8 *)dst, src, 2);
    }
    result = gsSocketSend(list->socket, buffer, len, 0);
    if (result <= 0) {
        SBServerListDisconnect(list);
        return 3;
    }
    list->state = SB_LIST_LIST;
    list->readStage = 0;
    if (!list->inBuffer) {
        list->inBuffer = DWCi_GsMalloc(4096);
        if (!list->inBuffer) {
            return 5;
        }
        list->inBufferLen = 0;
    }
    return 0;
}

#pragma inlineintrinsics reset

void FreePopularValues(SBServerList *list) {
    int i;
    for (i = 0; i < list->numPopularValues; i++) {
        SBReleaseStr(list, list->popularValues[i]);
    }
    list->numPopularValues = 0;
}

void FreeKeyList(SBServerList *list) {
    int i;
    if (!list->keyList) {
        return;
    }
    for (i = 0; i < ArrayLength(list->keyList); i++) {
        SBReleaseStr(list, *(char **)ArrayNth(list->keyList, i));
    }
    ArrayFree(list->keyList);
    list->keyList = NULL;
}

void SBServerListDisconnect(SBServerList *list) {
    if (list->inBuffer) {
        DWCi_GsFree(list->inBuffer);
    }
    list->inBuffer = NULL;
    list->inBufferLen = 0;
    if (list->socket != -1) {
        gsSocketClose(list->socket);
    }
    list->socket = -1;
    list->state = SB_LIST_DISCONNECTED;
    FreeKeyList(list);
    list->expectedElements = -1;
    FreePopularValues(list);
}

void SBServerListCleanup(SBServerList *list) {
    SBServerListDisconnect(list);
    SBServerListClear(list);
    SBRefStrHashCleanup(list);
    if (list->servers) {
        ArrayFree(list->servers);
    }
    list->servers = NULL;
}

// Mixes the reply's random bytes into the challenge and starts decoding with it
void InitCryptKey(SBServerList *list, s8 *data, int count) {
    int i, len;
    char *key;
    len = strlen(list->secretKey);
    key = list->secretKey;
    for (i = 0; i < count; i++) {
        int first = i % 8;
        int second = (i * (signed char)key[i % len]) % 8;
        signed char value = list->challenge[first] ^ data[i];
        list->challenge[second] = list->challenge[second] ^ value;
    }
    GOACryptInit(list->cryptState, list->challenge, 8);
}

// Size of a server record's fixed part for its flags byte
int ServerSizeForFlags(int flags) {
    int size = 5;
    if (flags & 2) {
        size += 4;
    }
    if (flags & 8) {
        size += 4;
    }
    if (flags & 16) {
        size += 2;
    }
    if (flags & 32) {
        size += 2;
    }
    return size;
}

// Checks that data holds complete key / value string pairs ending with an empty key
int FullRulesPresent(char *data, int len) {
    int size;
    while (len > 0 && *data) {
        size = NTSLengthSB(data, len);
        if (size < 0) {
            return 0;
        }
        len -= size;
        data += size;
        size = NTSLengthSB(data, len);
        if (size < 0) {
            return 0;
        }
        data += size;
        len -= size;
    }
    if (len == 0) {
        return 0;
    }
    return *data == 0;
}

// Checks that data holds complete values for all keys of keyList
int AllKeysPresent(SBServerList *list, u8 *data, int len) {
    int count = ArrayLength(list->keyList);
    int i;
    for (i = 0; i < count; i++) {
        SBKeyInfo *key = ArrayNth(list->keyList, i);
        switch (key->type) {
        case 1:
            data++;
            len--;
            break;
        case 2:
            data += 2;
            len -= 2;
            break;
        case 0:
            if (len < 1) {
                return 0;
            }
            len--;
            if (*data++ == 255) {
                int size = NTSLengthSB((char *)data, len);
                if (size == -1) {
                    return 0;
                }
                data += size;
                len -= size;
            }
            break;
        default:
            __msl_assertion_failed(ov00_0221A638, ov00_0221A5C8, ov00_0221A550, 0x317);
            return 0;
        }
        if (len < 0) {
            return 0;
        }
    }
    return 1;
}

typedef struct {
    u8 data[4];
} Bytes4;

typedef struct {
    u8 data[2];
} Bytes2;

// Public address of a server record
void ParseServerIPPort(SBServerList *list, const u8 *data, int len, u32 *ip, u16 *port) {
    u8 flags;
    if (len < 5) {
        return;
    }
    flags = data[0];
    *(Bytes4 *)ip = *(const Bytes4 *)(data + 1);
    if (flags & 0x10) {
        if (len - 5 < 2) {
            return;
        }
        *(Bytes2 *)port = *(const Bytes2 *)(data + 5);
    } else {
        *port = list->defaultPort;
    }
}

// Parses a server record into server, returns its size
int ParseServer(SBServerList *list, SBServer *server, const u8 *data, int len, int popular) {
    struct {
        int len;
        u16 port, number;
        u32 value;
    } frame;

    int left = len;
    u32 flags;
    flags = *data;
    data += 5;
    frame.len = len;
    SBServerSetFlags(server, (u8)flags);
    left -= 5;
    if (flags & 0x10) {
        data += 2;
        left -= 2;
    }
    if (flags & 2) {
        memcpy(&frame.value, data, 4);
        data += 4;
        left -= 4;
    } else {
        frame.value = 0;
    }
    if (flags & 0x20) {
        memcpy(&frame.port, data, 2);
        data += 2;
        left -= 2;
    } else {
        frame.port = list->defaultPort;
    }
    SBServerSetPrivateAddr(server, frame.value, frame.port);
    if (flags & 8) {
        memcpy(&frame.value, data, 4);
        data += 4;
        left -= 4;
        SBServerSetICMPIP(server, frame.value);
    }
    if (flags & 0x40) {
        int count = ArrayLength(list->keyList);
        int i;
        for (i = 0; i < count; i++) {
            SBKeyInfo *key = ArrayNth(list->keyList, i);
            switch (key->type) {
            case 1:
                SBServerAddIntKeyValue(server, key->name, *data++);
                left--;
                break;
            case 2:
                memcpy(&frame.number, data, 2);
                SBServerAddIntKeyValue(server, key->name, (u16)(((int)frame.number >> 8) & 255 | ((int)frame.number << 8) & 0xFF00));
                data += 2;
                left -= 2;
                break;
            case 0: {
                int code;
                if (popular) {
                    code = *data++;
                    left--;
                } else {
                    code = 255;
                }
                if (code == 255) {
                    int size;
                    SBServerAddKeyValue(server, key->name, (const char *)data);
                    size = strlen((const char *)data) + 1;
                    data += size;
                    left -= size;
                } else {
                    SBServerAddKeyValue(server, key->name, list->popularValues[code]);
                }
                break;
            }
            default:
                break;
            }
        }
        SBServerSetState(server, (u8)(SBServerGetState(server) | 1));
    }
    if (flags & 0x80) {
        while (*(const s8 *)data != 0 && left > 0) {
            const char *name = (const char *)data;
            int size = strlen(name) + 1;
            data += size;
            left -= size;
            SBServerAddKeyValue(server, name, (const char *)data);
            size = strlen((const char *)data) + 1;
            data += size;
            left -= size;
        }
        left--;
        SBServerSetState(server, (u8)(SBServerGetState(server) | 2));
    }
    return frame.len - left;
}

// One server record of the list reply: 0 if incomplete, -1 at the end marker, -2 on allocation failure
int IncomingListParseServer(SBServerList *list, const u8 *data, int len) {
    u8 flags;
    int size, used;
    SBServer *server;
    u32 ip;
    u16 port;
    if (len < 1) {
        return 0;
    }
    flags = *data;
    size = ServerSizeForFlags(flags);
    if (len < size) {
        return 0;
    }
    if ((flags & 0x40) && AllKeysPresent(list, (u8 *)data + size, len - size) == 0) {
        return 0;
    }
    if ((flags & 0x80) && FullRulesPresent((char *)data + size, len - size) == 0) {
        return 0;
    }
    if (memcmp(data + 1, ov00_0221A63C, 4) == 0) {
        return -1;
    }
    ParseServerIPPort(list, data, len, &ip, &port);
    server = SBAllocServer(list, ip, port);
    if (SBIsNullServer(server) != 0) {
        return -2;
    }
    used = ParseServer(list, server, data, len, 1);
    SBServerListAppendServer(list, server);
    return used;
}

void SBSetLastListErrorPtr(SBServerList *list, char *message) {
    list->errorMessage = message;
}

// Reads the list reply: crypt header, own address, keys, popular values, then the server records
int ProcessMainListData(SBServerList *list) {
    struct {
        u8 *ptr;
        int left;
    } cursor = { list->inBuffer, list->inBufferLen };

    SBKeyInfo key;
    switch (list->readStage) {
    case 0: {
        int pos, total, size;
        if (cursor.left < 1) {
            break;
        }
        pos = (*cursor.ptr ^ 0xEC) + 2;
        if (cursor.left < pos) {
            break;
        }
        size = cursor.ptr[pos - 1] ^ 0xEA;
        total = pos + size;
        if (cursor.left < total) {
            break;
        }
        InitCryptKey(list, (s8 *)cursor.ptr + pos, size);
        cursor.ptr += total;
        cursor.left -= total;
        list->readStage = 1;
        GOADecrypt(list->cryptState, cursor.ptr, cursor.left);
    }
    case 1:
        if (cursor.left < 6) {
            break;
        }
        memcpy(&list->publicIp, cursor.ptr, 4);
        list->callback(list, 6, ov00_0221B9D4, list->param);
        memcpy(&list->defaultPort, cursor.ptr + 4, 2);
        if (list->defaultPort == 0xFFFF) {
            if (NTSLengthSB((char *)cursor.ptr + 6, cursor.left - 6) == -1) {
                break;
            }
            SBSetLastListErrorPtr(list, (char *)cursor.ptr + 6);
            list->callback(list, 5, ov00_0221B9D4, list->param);
            if (!list->inBuffer) {
                break;
            }
        }
        cursor.ptr += 6;
        cursor.left -= 6;
        if ((list->queryOptions & 2) || list->defaultPort == 0xFFFF) {
            list->readStage = 5;
            list->state = SB_LIST_CONNECTED;
            break;
        }
        list->readStage = 2;
        list->expectedElements = -1;
    case 2: {
        int size;
        if (list->expectedElements == -1) {
            if (cursor.left < 1) {
                break;
            }
            list->expectedElements = *cursor.ptr;
            list->keyList = ArrayNew(8, list->expectedElements, NULL);
            if (!list->keyList) {
                return 5;
            }
            cursor.ptr++;
            cursor.left--;
        }
        while (list->expectedElements > ArrayLength(list->keyList)) {
            if (cursor.left < 2) {
                break;
            }
            size = NTSLengthSB((char *)cursor.ptr + 1, cursor.left - 1);
            if (size == -1) {
                break;
            }
            key.type = *cursor.ptr;
            key.name = SBRefStr(list, (char *)cursor.ptr + 1);
            ArrayAppend(list->keyList, &key);
            cursor.ptr += size + 1;
            cursor.left -= size + 1;
        }
        if (list->expectedElements > ArrayLength(list->keyList)) {
            break;
        }
        list->readStage = 3;
        list->expectedElements = -1;
    }
    case 3: {
        int size;
        if (list->expectedElements == -1) {
            if (cursor.left < 1) {
                break;
            }
            list->expectedElements = *cursor.ptr++;
            cursor.left--;
            list->numPopularValues = 0;
        }
        while (list->expectedElements > list->numPopularValues) {
            size = NTSLengthSB((char *)cursor.ptr, cursor.left);
            if (size == -1) {
                break;
            }
            list->popularValues[list->numPopularValues++] = SBRefStr(list, (char *)cursor.ptr);
            cursor.ptr += size;
            cursor.left -= size;
        }
        if (list->expectedElements > list->numPopularValues) {
            break;
        }
        list->readStage = 4;
    }
    case 4: {
        int size;
        if (cursor.left < 5) {
            break;
        }
        do {
            size = IncomingListParseServer(list, cursor.ptr, cursor.left);
            if (size == -2) {
                return 5;
            }
            if (size == -1) {
                list->readStage = 5;
                list->state = SB_LIST_CONNECTED;
                cursor.left -= 5;
                cursor.ptr += 5;
                list->callback(list, 3, ov00_0221B9D4, list->param);
                break;
            }
            cursor.ptr += size;
            cursor.left -= size;
            if (!list->inBuffer) {
                size = 0;
            }
        } while (size);
    }
    }
    if (cursor.left < 0) {
        __msl_assertion_failed(ov00_0221A644, ov00_0221A5C8, ov00_0221A588, 0x4AF);
    }
    if (!list->inBuffer) {
        return 0;
    }
    if (cursor.left) {
        memmove(list->inBuffer, cursor.ptr, cursor.left);
    }
    list->inBufferLen = cursor.left;
    return 0;
}

// Message 1: new key list
int ProcessPushKeysMessage(SBServerList *list, u8 *data, int len) {
    int i;
    int count = *data++;
    SBKeyInfo key;
    len--;
    if (list->keyList) {
        FreeKeyList(list);
    }
    list->keyList = ArrayNew(8, count, NULL);
    if (!list->keyList) {
        return 5;
    }
    for (i = 0; i < count; i++) {
        int size;
        if (len < 2) {
            return 4;
        }
        size = NTSLengthSB((char *)data + 1, len - 1);
        if (size == -1) {
            return 4;
        }
        key.type = data[0];
        key.name = SBRefStr(list, (char *)data + 1);
        ArrayAppend(list->keyList, &key);
        data += size + 1;
        len -= size + 1;
    }
    return 0;
}

// Message 6: named entries (name, address, port, value, string) for the playerSearchCallback callback
int ProcessPlayerSearchMessage(SBServerList *list, u8 *data, int len) {
    u32 count, last;
    int i;
    u32 ip, value;
    u16 port;
    char *name;
    if (len < 2) {
        return 4;
    }
    count = data[1];
    last = data[0];
    data += 2;
    len -= 2;
    for (i = 0; i < (int)count; i++) {
        int size;
        name = (char *)data;
        size = NTSLengthSB((char *)data, len);
        if (size == -1) {
            return 4;
        }
        data += size;
        len -= size;
        if (len < 11) {
            return 4;
        }
        memcpy(&ip, data, 4);
        memcpy(&port, data + 4, 2);
        memcpy(&value, data + 6, 4);
        data += 10;
        len -= 10;
        value = ((value >> 24) & 255) | ((value >> 8) & 0xFF00) | ((value << 8) & 0xFF0000) | ((value << 24) & 0xFF000000);
        size = NTSLengthSB((char *)data, len);
        if (size == -1) {
            return 4;
        }
        list->playerSearchCallback(list, name, ip, port, value, (char *)data, list->param);
        data += size;
        len -= size;
    }
    if (last) {
        list->playerSearchCallback(list, NULL, 0, 0, 0, NULL, list->param);
    }
    return 0;
}

// Message 5: strings for a known server, for the maploopCallback callback
int ProcessMaploopMessage(SBServerList *list, u8 *data, int len) {
    u32 value;
    u16 port;
    u32 ip;
    char *strings[16];
    int i, count;
    SBServer *server;
    if (len < 11) {
        return 4;
    }
    memcpy(&ip, data, 4);
    memcpy(&port, data + 4, 2);
    i = SBServerListFindServerByIP(list, ip, port);
    if (i == -1) {
        return 0;
    }
    server = SBServerListNth(list, i);
    memcpy(&value, data + 6, 4);
    value = ((value >> 24) & 255) | ((value >> 8) & 0xFF00) | ((value << 8) & 0xFF0000) | ((value << 24) & 0xFF000000);
    count = data[10];
    data += 11;
    len -= 11;
    for (i = 0; i < count && i < 16; i++) {
        int size;
        if (len < 1) {
            break;
        }
        size = NTSLengthSB((char *)data, len);
        if (size == -1) {
            return 4;
        }
        strings[i] = (char *)data;
        data += size;
        len -= size;
    }
    if (!list->maploopCallback) {
        return 0;
    }
    list->maploopCallback(list, server, value, i, strings, list->param);
    return 0;
}

// Message 4: a server was removed
int ProcessDeleteServerMessage(SBServerList *list, u8 *data, int len) {
    u32 ip;
    u16 port;
    int i;
    if (len < 6) {
        return 4;
    }
    memcpy(&ip, data, 4);
    memcpy(&port, data + 4, 2);
    i = SBServerListFindServerByIP(list, ip, port);
    if (i == -1) {
        return 0;
    }
    SBServerListRemoveAt(list, i);
    return 0;
}

// Message 2: a server record (new or updated server)
int ProcessPushServerMessage(SBServerList *list, void *data, int len) {
    u32 ip;
    u16 port;
    SBServer *server;
    int i;
    if (len < 5) {
        return 4;
    }
    ParseServerIPPort(list, data, len, &ip, &port);
    i = SBServerListFindServerByIP(list, ip, port);
    if (i == -1) {
        server = SBAllocServer(list, ip, port);
        if (SBIsNullServer(server)) {
            return 5;
        }
    } else {
        server = SBServerListNth(list, i);
    }
    if (ParseServer(list, server, data, len, 0) < 0) {
        return 4;
    }
    if (i == -1) {
        SBServerListAppendServer(list, server);
    }
    list->callback(list, 1, server, list->param);
    return 0;
}

// Handles the complete messages in inBuffer (big-endian u16 size, type byte, payload)
int ProcessAdHocData(SBServerList *list) {
    u16 size;
    int result = 0;
    while (list->inBufferLen >= 3) {
        memcpy(&size, list->inBuffer, 2);
        size = ((size >> 8) & 255) | ((size << 8) & 0xFF00);
        if (size > 4096) {
            result = 4;
            break;
        }
        if (list->inBufferLen < size) {
            return 0;
        }
        switch (*(s8 *)(list->inBuffer + 2)) {
        case 1:
            result = ProcessPushKeysMessage(list, list->inBuffer + 3, size - 3);
            break;
        case 2:
            result = ProcessPushServerMessage(list, list->inBuffer + 3, size - 3);
            break;
        case 3:
            if (gsSocketSend(list->socket, list->inBuffer, size, 0) <= 0) {
                return 3;
            }
            break;
        case 4:
            result = ProcessDeleteServerMessage(list, list->inBuffer + 3, size - 3);
            break;
        case 5:
            result = ProcessMaploopMessage(list, list->inBuffer + 3, size - 3);
            break;
        case 6:
            result = ProcessPlayerSearchMessage(list, list->inBuffer + 3, size - 3);
            break;
        }
        list->inBufferLen -= size;
        if (list->inBufferLen < 0) {
            __msl_assertion_failed(ov00_0221A650, ov00_0221A5C8, ov00_0221A574, 1460);
        }
        if (list->inBufferLen && list->inBuffer) {
            memmove(list->inBuffer, list->inBuffer + size, list->inBufferLen);
        }
        if (result) {
            break;
        }
    }
    if (result) {
        ErrorDisconnect(list);
    }
    return result;
}

// Receives from the master server connection
int ProcessIncomingData(SBServerList *list) {
    int used, received, error;
    if (!CanReceiveOnSocket(list->socket)) {
        return 0;
    }
    used = list->inBufferLen;
    received = gsSocketRecv(list->socket, list->inBuffer + used, 0x1000 - used, 0);
    if ((u32)(received + 1) <= 1) {
        ErrorDisconnect(list);
        return 3;
    }
    list->inBufferLen += received;
    error = 0;
    if (list->state == SB_LIST_CONNECTED || list->readStage > 0) {
        GOADecrypt(list->cryptState, list->inBuffer + used, list->inBufferLen - used);
    }
    if (list->state == SB_LIST_LIST) {
        error = ProcessMainListData(list);
    }
    if (error) {
        return error;
    }
    if (list->state == SB_LIST_CONNECTED && list->inBufferLen > 0) {
        return ProcessAdHocData(list);
    }
    return 0;
}

// Sends data to a server through the master server (message 2)
int SBSendMessageToServer(SBServerList *list, u32 ip, u16 port, void *data, int len) {
    u8 packet[16];
    u16 size;
    int value;
    if (list->state == SB_LIST_DISCONNECTED) {
        SBServerListConnectAndQuery(list, NULL, NULL, 2, 0);
    }
    if (list->state == SB_LIST_DISCONNECTED) {
        return 3;
    }
    value = (u16)(len + 9);
    size = ((value >> 8) & 255) | ((value << 8) & 0xFF00);
    memcpy(packet, &size, 2);
    packet[2] = 2;
    memcpy(packet + 3, &ip, 4);
    memcpy(packet + 7, &port, 2);
    {
        int result = SendWithRetry(list, packet, 9);
        if (result) {
            return result;
        }
    }
    return gsSocketSend(list->socket, data, len, 0) < 0 ? 3 : 0;
}

// Sends the 6-byte NAT negotiation magic plus value to a server
int SBSendNatNegotiateCookieToServer(SBServerList *list, u32 ip, u16 port, u32 value) {
    u8 packet[10];
    value = ((value >> 24) & 255) | ((value >> 8) & 0xFF00) | ((value << 8) & 0xFF0000) | ((value << 24) & 0xFF000000);
    packet[0] = 0xFD;
    packet[1] = 0xFC;
    packet[2] = 0x1E;
    packet[3] = 0x66;
    packet[4] = 0x6A;
    packet[5] = 0xB2;
    memcpy(packet + 6, &value, 4);
    return SBSendMessageToServer(list, ip, port, packet, 10);
}

// LAN mode: adds every server that answers, gives up after two seconds
int ProcessLanData(SBServerList *list) {
    SockAddr from;
    int fromLen = 8;
    u8 buffer[1500];
    if (CanReceiveOnSocket(list->socket)) {
        do {
            int i;
            SBServer *server;
            if (gsSocketRecvFrom(list->socket, buffer, 1499, 0, &from, &fromLen) != -1) {
                i = SBServerListFindServerByIP(list, from.address, from.port);
                if (i == -1) {
                    server = SBAllocServer(list, from.address, from.port);
                    if (SBIsNullServer(server)) {
                        return 5;
                    }
                    SBServerSetFlags(server, 0x11);
                    SBServerListAppendServer(list, server);
                }
            }
        } while (CanReceiveOnSocket(list->socket));
    }
    if (current_time() - list->lanStartTime > 2000) {
        gsSocketClose(list->socket);
        list->socket = -1;
        list->state = SB_LIST_DISCONNECTED;
        list->callback(list, 3, ov00_0221B9D4, list->param);
    }
    return 0;
}

int SBListThink(SBServerList *list) {
    SBServerListFreeDeadList(list);
    switch (list->state) {
    case SB_LIST_CONNECTED:
    case SB_LIST_LIST:
        return ProcessIncomingData(list);
    case SB_LIST_LAN:
        return ProcessLanData(list);
    case SB_LIST_DISCONNECTED:
    default:
        return 0;
    }
}
