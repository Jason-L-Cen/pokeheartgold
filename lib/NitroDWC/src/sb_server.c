#pragma force_active on
#include <string.h>

#include "global.h"

#include "dwc/sb_internal.h"

// The code below is compiled as ARM (32-bit) code.
#include <nitro/code32.h>

extern void __msl_assertion_failed(const char *expr, const char *file, const char *func, int line);
extern int strnicmp(const char *a, const char *b);
extern long atol(const char *str);
extern double atod(const char *str);
extern int sprintf(char *buf, const char *fmt, ...);
extern u16 __msl_digit[];
extern u8 __lower_mapC[];

extern void *ov00_021EC3A8(u32 size);
extern void ov00_021EC3D8(void *p);
extern int ArrayLength(void *array);
extern void *ov00_021FED4C(int elemSize, int buckets, int chains, void *hash, void *compare, void *free);
extern void ov00_021FEE8C(void *table);
extern int ov00_021FEF00(void *table);
extern void ov00_021FEF74(void *table, void *elem);
extern void *ov00_021FF0C0(void *table, void *elem);
extern u32 ov00_021FF368(void);
extern void ov00_021FF3C0(int ms);
extern u32 ov00_021FFA44(const char *address);

extern int ov00_0221B0D0;
extern char *ov00_022163F0[2]; // the keys CheckValidKey skips
extern char ov00_0221A4E4[];
extern char ov00_0221A4FC[];
extern char ov00_0221A500[];
extern char ov00_0221A508[];
extern char ov00_0221A514[];
extern char ov00_0221A51C[];
extern const char ov00_0221A520[];
extern char ov00_0221A528[];
extern char *ov00_0221A0B8[]; // query key names

// The query engine part of this module: drops queries that got no answer in 2.5 seconds
void TimeoutOldQueries(SBQueryEngine *engine) {
    u32 now = ov00_021FF368();
    SBServer *server;
    while ((server = engine->querying.first) != NULL) {
        if (now <= server->updateTime + 2500) {
            return;
        }
        server->flags |= 0x10;
        engine->querying.first->updateTime = 2500;
        engine->querying.first->flags &= 0xD3;
        engine->callback(engine, 1, engine->querying.first, engine->param);
        FIFOGetFirst(&engine->querying);
    }
}

void QueueNextQueries(SBQueryEngine *engine) {
    while (engine->querying.count < engine->maxUpdates && engine->pending.count > 0) {
        QEStartQuery(engine, FIFOGetFirst(&engine->pending));
    }
}

void SBQueryEngineThink(SBQueryEngine *engine) {
    if (!engine->querying.count) {
        return;
    }
    ProcessIncomingReplies(engine, 0);
    TimeoutOldQueries(engine);
    if (engine->pending.count > 0) {
        QueueNextQueries(engine);
    }
    if (!engine->querying.count) {
        engine->callback(engine, 2, NULL, engine->param);
    }
}

void SBQueryEngineAddQueryKey(SBQueryEngine *engine, int key) {
    int count = engine->numKeys;
    if (count >= 20) {
        return;
    }
    engine->numKeys = count + 1;
    engine->keys[count] = key;
}

void SBEngineRemoveServerFromFIFOs(SBQueryEngine *engine, SBServer *server) {
    if (FIFORemove(&engine->querying, server)) {
        return;
    }
    FIFORemove(&engine->pending, server);
}

// The shared strings table (see SBRefStr): hashed and compared case-insensitively
u32 RefStringHash(const void *elem, u32 buckets) {
    return StringHash(*(char *const *)elem, buckets);
}

int RefStringCompare(const void *a, const void *b) {
    return strnicmp(*(char *const *)a, *(char *const *)b);
}

void RefStringFree(void *elem) {
    ov00_021EC3D8(*(char **)elem);
}

void *SBRefStrHash(SBServerList *list) {
    if (!ov00_0221B9D0.refStrings) {
        ov00_0221B9D0.refStrings = ov00_021FED4C(8, 100, 2, RefStringHash, RefStringCompare, RefStringFree);
    }
    return ov00_0221B9D0.refStrings;
}

void SBRefStrHashCleanup(SBServerList *list) {
    if (!ov00_0221B9D0.refStrings) {
        return;
    }
    if (ov00_021FEF00(ov00_0221B9D0.refStrings)) {
        return;
    }
    ov00_021FEE8C(ov00_0221B9D0.refStrings);
    ov00_0221B9D0.refStrings = NULL;
}

void SBServerFree(SBServer **server) {
    SBServer *s = *server;
    ov00_021FEE8C(s->keyValues);
    s->keyValues = NULL;
    ov00_021EC3D8(s);
}

void SBServerAddKeyValue(SBServer *server, const char *key, const char *value) {
    char *pair[2];
    pair[0] = SBRefStr(NULL, key);
    pair[1] = SBRefStr(NULL, value);
    ov00_021FEF74(server->keyValues, pair);
}

void SBServerAddIntKeyValue(SBServer *server, const char *key, int value) {
    char buffer[20];
    sprintf(buffer, ov00_0221A4FC, value);
    SBServerAddKeyValue(server, key, buffer);
}

char *SBServerGetStringValueA(SBServer *server, const char *key, char *fallback) {
    struct {
        const char *key;
        u32 unused;
    } elem;

    char **pair;
    if (!server) {
        __msl_assertion_failed(ov00_0221A500, ov00_0221A508, ov00_0221A4E4, 151);
    }
    if (!server) {
        return NULL;
    }
    elem.key = key;
    pair = ov00_021FF0C0(server->keyValues, &elem.key);
    if (pair) {
        fallback = pair[1];
    }
    return fallback;
}

int SBServerGetIntValueA(SBServer *server, const char *key, int fallback) {
    char *value;
    int ch, digit;
    if (!strcmp(key, ov00_0221A514)) {
        return SBServerGetPing(server);
    }
    value = SBServerGetStringValueA(server, key, NULL);
    if (value) {
        ch = (u8)*value;
        if (ch < 0 || ch >= 128) {
            digit = 0;
        } else {
            digit = __msl_digit[ch] & 8;
        }
        if (digit) {
            goto success;
        }
    }
    return fallback;
success:
    return atol(value);
}

double SBServerGetFloatValueA(SBServer *server, const char *key, double fallback) {
    char *value = SBServerGetStringValueA(server, key, NULL);
    if (value) {
        fallback = atod(value);
    }
    return fallback;
}

u32 SBServerGetPublicInetAddress(SBServer *server) {
    return server->publicIp;
}

u16 SBServerGetPublicQueryPort(SBServer *server) {
    u16 port = server->publicPort;
    return ((port >> 8) & 255) | ((port << 8) & 0xFF00);
}

int SBServerGetPublicQueryPortNBO(SBServer *server) {
    return server->publicPort;
}

int SBServerHasPrivateAddress(SBServer *server) {
    return (server->flags & 2) == 2;
}

u32 SBServerGetPrivateInetAddress(SBServer *server) {
    return server->privateIp;
}

u16 SBServerGetPrivateQueryPort(SBServer *server) {
    u16 port = server->privatePort;
    return ((port >> 8) & 255) | ((port << 8) & 0xFF00);
}

void SBServerSetNext(SBServer *server, SBServer *next) {
    server->next = next;
}

SBServer *SBServerGetNext(SBServer *server) {
    return server->next;
}

typedef struct SBSkippedKeys {
    char *keys[2];
} SBSkippedKeys;

int CheckValidKey(const char *key) {
    SBSkippedKeys skipped = *(SBSkippedKeys *)&ov00_022163F0;
    u32 i;
    for (i = 0; i < 2; i++) {
        if (strcmp(key, skipped.keys[i]) == 0) {
            return 0;
        }
    }
    return 1;
}

// strtok-like: splits at delim, keeps its position in ov00_0221B9D0.tokenNext
char *mytok(char *input, int delim) {
    int ch;
    char *p, *start;
    if (input) {
        ov00_0221B9D0.tokenNext = input;
    }
    start = ov00_0221B9D0.tokenNext;
    p = start;
    while ((ch = *p) != 0 && ch != delim) {
        p++;
        ov00_0221B9D0.tokenNext = p;
    }
    if (p == start) {
        start = NULL;
    }
    if (ch == 0) {
        return start;
    }
    {
        char *next = ov00_0221B9D0.tokenNext;
        *p = 0;
        ov00_0221B9D0.tokenNext = next + 1;
    }
    return start;
}

// Adds the "\key\value" pairs of a legacy query reply
void SBServerParseKeyVals(SBServer *server, char *str) {
    char *key = mytok(str + 1, '\\');
    if (!key) {
        return;
    }
    do {
        char *value = mytok(NULL, '\\');
        if (!value) {
            value = ov00_0221A51C;
        }
        if (CheckValidKey(key)) {
            SBServerAddKeyValue(server, key, value);
        }
        key = mytok(NULL, '\\');
    } while (key);
}

static inline u32 Swap16(s32 value) {
    return ((value >> 8) & 255) | ((value << 8) & 0xFF00);
}

static inline int ToLower(int c) {
    if (c < 0 || c >= 128) {
        return c;
    }
    return __lower_mapC[c];
}

// Adds the keys of a full query reply: key / value pairs, then two tables (players, teams) whose
// values are stored as "<key><row>"
void SBServerParseQR2FullKeysSingle(SBServer *server, const char *data, int len) {
    char buffer[128];

    union {
        volatile u16 word;
        volatile u8 bytes[2];
    } count;

    const char *names;
    int table;
    while (*data) {
        const char *name;
        int size = NTSLengthSB(data, len);
        if (size < 0) {
            return;
        }
        name = data;
        len -= size;
        data += size;
        size = NTSLengthSB(data, len);
        if (size < 0) {
            return;
        }
        {
            const char *value = data;
            data += size;
            len -= size;
            SBServerAddKeyValue(server, name, value);
        }
    }
    data++;
    len--;
    for (table = 0; table < 2; table++) {
        int fields, row;
        if (len < 2) {
            return;
        }
        memcpy((void *)&count, data, 2);
        data += 2;
        names = data;
        len -= 2;
        count.word = Swap16(count.word);
        fields = 0;
        while (*data) {
            int size = NTSLengthSB(data, len);
            if (size < 0 || size > 100) {
                return;
            }
            data += size;
            fields++;
            len -= size;
        }
        data++;
        len--;
        for (row = 0; row < count.word; row++) {
            int field;
            const char *name;
            for (name = names, field = 0; field < fields; field++) {
                int size = NTSLengthSB(data, len);
                if (size < 0) {
                    return;
                }
                sprintf(buffer, ov00_0221A520, name, row);
                SBServerAddKeyValue(server, buffer, data);
                data += size;
                len -= size;
                name += strlen(name) + 1;
            }
        }
    }
}

u32 StringHash(const char *str, u32 mod) {
    u32 hash = 0;
    while (*str) {
        hash = hash * 0x9CCF9319 + ToLower(*str);
        str++;
    }
    return hash % mod;
}

void KeyValFree(char **pair) {
    SBReleaseStr(NULL, pair[0]);
    SBReleaseStr(NULL, pair[1]);
}

u32 KeyValHashKey(char **pair, u32 mod) {
    return StringHash(*pair, mod);
}

int KeyValCompareKey(char **a, char **b) {
    return strnicmp(*a, *b);
}

u32 SBServerGetPing(SBServer *server) {
    return server->updateTime;
}

SBServer *SBAllocServer(SBServerList *list, u32 ip, u16 port) {
    SBServer *server = ov00_021EC3A8(sizeof(SBServer));
    if (!server) {
        return NULL;
    }
    server->keyValues = ov00_021FED4C(8, 8, 4, KeyValHashKey, KeyValCompareKey, KeyValFree);
    if (!server->keyValues) {
        ov00_021EC3D8(server);
        return NULL;
    }
    server->state = 0;
    server->flags = 0;
    server->next = NULL;
    server->updateTime = 0;
    server->unk_10 = 0;
    server->publicIp = ip;
    server->publicPort = port;
    server->privateIp = 0;
    server->privatePort = 0;
    return server;
}

void SBServerSetFlags(SBServer *server, u8 flags) {
    server->flags = flags;
}

void SBServerSetPrivateAddr(SBServer *server, u32 ip, u16 port) {
    server->privateIp = ip;
    server->privatePort = port;
}

void SBServerSetICMPIP(SBServer *server, u32 ip) {
    server->unk_10 = ip;
}

void SBServerSetState(SBServer *server, u8 state) {
    server->state = state;
}

u8 SBServerGetState(SBServer *server) {
    return server->state;
}

int SBIsNullServer(SBServer *server) {
    return server == ov00_0221B9D0.nullServer;
}

// Server list callback of the browser
void ListCallback(SBServerList *list, int reason, SBServer *server, ServerBrowser *browser) {
    switch (reason) {
    case 0:
        browser->callback(browser, 0, server, browser->param);
        if ((server->state & 3) && (server->state & 64)) {
            break;
        }
        if ((server->state & 44) == 0 && browser->unk_624 == 0) {
            int mode;
            if (server->flags & 1) {
                if (browser->list.state == SB_LIST_LAN || browser->engine.numKeys == 0) {
                    mode = 1;
                } else {
                    mode = 0;
                }
            } else {
                mode = 2;
            }
            SBQueryEngineUpdateServer(&browser->engine, server, 0, mode);
        }
        break;
    case 1:
        if ((server->state & 67) == 0) {
            browser->callback(browser, 2, server, browser->param);
        } else {
            browser->callback(browser, 1, server, browser->param);
        }
        break;
    case 2:
        if (server->state & 44) {
            SBEngineRemoveServerFromFIFOs(&browser->engine, server);
        }
        browser->callback(browser, 3, server, browser->param);
        break;
    case 3:
        if (browser->unk_620) {
            SBServerListDisconnect(list);
        }
        if (ArrayLength(list->servers) != 0 && browser->engine.querying.count != 0) {
            break;
        }
        browser->callback(browser, 4, NULL, browser->param);
        break;
    case 5:
        browser->callback(browser, 5, NULL, browser->param);
        break;
    case 6:
        SBQueryEngineSetPublicIP(&browser->engine, browser->list.publicIp);
        break;
    }
    if (server && server->publicIp == browser->triggerIp && server->publicPort == browser->triggerPort) {
        browser->triggerIp = 0;
    }
}

// Query engine callback of the browser
void EngineCallback(SBQueryEngine *engine, int reason, SBServer *server, ServerBrowser *browser) {
    switch (reason) {
    case 1:
        browser->callback(browser, 2, server, browser->param);
        break;
    case 0:
        browser->callback(browser, 1, server, browser->param);
        break;
    case 2:
        browser->callback(browser, 4, server, browser->param);
        break;
    }
    if (server && server->publicIp == browser->triggerIp && server->publicPort == browser->triggerPort) {
        browser->triggerIp = 0;
    }
}

ServerBrowser *ServerBrowserNewA(const char *targetGameName, const char *gameName, const char *secretKey, int unk_4B8, int maxUpdates, int queryVersion, int force, ServerBrowserCallback callback, void *param) {
    ServerBrowser *browser;
    if (!force && ov00_0221B0D0 != 1) {
        return NULL;
    }
    browser = ov00_021EC3A8(sizeof(ServerBrowser));
    if (!browser) {
        return NULL;
    }
    browser->callback = callback;
    browser->param = param;
    browser->unk_624 = 0;
    SBServerListInit(&browser->list, targetGameName, gameName, secretKey, unk_4B8, force, (SBListCallback)ListCallback, browser);
    SBQueryEngineInit(&browser->engine, maxUpdates, queryVersion, force, (SBEngineCallback)EngineCallback, browser);
    return browser;
}

void ServerBrowserFree(ServerBrowser *browser) {
    SBServerListCleanup(&browser->list);
    SBEngineCleanup(&browser->engine);
    ov00_021EC3D8(browser);
}

// Requests the list with the given query keys; waits for it unless async
int ServerBrowserBeginUpdate2(ServerBrowser *browser, int async, int unk_620, u8 *keys, int numKeys, const char *filter, u32 options, int unk) {
    struct {
        char data[256];
    } buffer = { 0 };

    int len = 0, i;
    int result;
    browser->unk_620 = unk_620;
    browser->engine.numKeys = 0;
    for (i = 0; i < numKeys; i++) {
        char *name = ov00_0221A0B8[keys[i]];
        if (len + (int)strlen(name) + 1 >= 256) {
            break;
        }
        len += sprintf(buffer.data + len, ov00_0221A528, name);
        SBQueryEngineAddQueryKey(&browser->engine, keys[i]);
    }
    result = SBServerListConnectAndQuery(&browser->list, buffer.data, filter, options, unk);
    if (result) {
        return result;
    }
    if (async) {
        return result;
    }
    while (browser->list.state == SB_LIST_LIST || (browser->engine.querying.count > 0 && !result)) {
        ov00_021FF3C0(10);
        result = ServerBrowserThink(browser);
    }
    return result;
}

int ServerBrowserLimitUpdateA(ServerBrowser *browser, int async, int unk_620, u8 *keys, int numKeys, const char *filter, int unk) {
    return ServerBrowserBeginUpdate2(browser, async, unk_620, keys, numKeys, filter, 128, unk);
}

int ServerBrowserSendMessageToServerA(ServerBrowser *browser, const char *address, int port, void *data, int len) {
    u32 ip = ov00_021FFA44(address);
    return SBSendMessageToServer(&browser->list, ip, (u16)(((port >> 8) & 255) | ((port << 8) & 0xFF00)), data, len);
}

int ServerBrowserSendNatNegotiateCookieToServerA(ServerBrowser *browser, const char *address, int port, u32 value) {
    u32 ip = ov00_021FFA44(address);
    return SBSendNatNegotiateCookieToServer(&browser->list, ip, (u16)(((port >> 8) & 255) | ((port << 8) & 0xFF00)), value);
}

void ServerBrowserRemoveServer(ServerBrowser *browser, SBServer *server) {
    int i = SBServerListFindServer(&browser->list, server);
    if (i == -1) {
        return;
    }
    SBServerListRemoveAt(&browser->list, i);
}

int ServerBrowserThink(ServerBrowser *browser) {
    SBQueryEngineThink(&browser->engine);
    return SBListThink(&browser->list);
}

void ServerBrowserHalt(ServerBrowser *browser) {
    SBServerListDisconnect(&browser->list);
    SBEngineHaltUpdates(&browser->engine);
}

void ServerBrowserClear(ServerBrowser *browser) {
    ServerBrowserHalt(browser);
    SBServerListClear(&browser->list);
}

int ServerBrowserState(ServerBrowser *browser) {
    int state;
    if (browser->engine.querying.count > 0) {
        return 2;
    }
    state = browser->list.state;
    if (state == SB_LIST_LIST || state == SB_LIST_LAN) {
        return 1;
    }
    return state == SB_LIST_DISCONNECTED ? 0 : 3;
}

SBServer *ServerBrowserGetServer(ServerBrowser *browser, int index) {
    return SBServerListNth(&browser->list, index);
}

int ServerBrowserCount(ServerBrowser *browser) {
    return SBServerListCount(&browser->list);
}

void ServerBrowserSortA(ServerBrowser *browser, int ascending, const char *key, u32 type) {
    SBServerListSort(&browser->list, ascending, key, type);
}

u32 ServerBrowserGetMyPublicIP(ServerBrowser *browser) {
    return browser->list.publicIp;
}
