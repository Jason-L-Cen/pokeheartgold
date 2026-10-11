#ifndef POKEHEARTGOLD_LIB_INCLUDE_DWC_SB_INTERNAL_H
#define POKEHEARTGOLD_LIB_INCLUDE_DWC_SB_INTERNAL_H

// Server browser of OVY_0 (lib/NitroDWC/src/sb_query_engine.c, sb_server.c, sb_serverlist.c): the server objects, the server
// list and its TCP connection to the master server, the UDP query engine and the browser that owns both.

#include "global.h"

typedef struct SBServer SBServer;
typedef struct SBServerList SBServerList;

// One server known to the browser (0x24 bytes, allocated by ov00_022132C4). Addresses and ports are
// kept in network byte order.
struct SBServer {
    u32 publicIp;    // 0x00
    u16 publicPort;  // 0x04
    u32 privateIp;   // 0x08
    u16 privatePort; // 0x0C
    u32 unk_10;      // 0x10
    u8 state;        // 0x14, flags: 1 basic keys, 2 all keys, 4/8 query sent, 0x40 has keys
    u8 flags;        // 0x15, first byte of the master server's server record
    void *keyValues; // 0x18, hash table of string key / value pairs
    u32 updateTime;  // 0x1C
    SBServer *next;  // 0x20, free list of SBServerList.deadList
};

// Key of the list request (SBServerList.keyList elements)
typedef struct SBKeyInfo {
    char *name;
    int type; // 0: string (or index into SBServerList.popularValues), 1: byte, 2: big-endian u16
} SBKeyInfo;

typedef void (*SBListCallback)(SBServerList *list, int reason, SBServer *server, void *param);
typedef void (*SBStringsCallback)(SBServerList *list, SBServer *server, u32 value, int count, char **strings, void *param);
typedef void (*SBEntryCallback)(SBServerList *list, char *name, u32 ip, u16 port, u32 value, char *string, void *param);

// Values of SBServerList.state
enum {
    SB_LIST_LAN,          // waiting for LAN answers
    SB_LIST_DISCONNECTED, // no master server connection
    SB_LIST_CONNECTED,    // reading messages from the master server
    SB_LIST_LIST,         // reading the server list reply
};

struct SBServerList {
    int state;                 // 0x000
    void *servers;             // 0x004, DArray of SBServer *
    void *keyList;             // 0x008, DArray of SBKeyInfo
    char targetGameName[36];   // 0x00C, also selects the master server host
    char gameName[36];         // 0x030
    char secretKey[32];        // 0x054
    s8 challenge[8];           // 0x074
    u8 *inBuffer;              // 0x07C, 4096 bytes
    int inBufferLen;           // 0x080
    char *popularValues[255];  // 0x084
    int numPopularValues;      // 0x480
    int expectedElements;      // 0x484
    SBListCallback callback;   // 0x488
    SBStringsCallback unk_48C; // 0x48C, message 5
    SBEntryCallback unk_490;   // 0x490, message 6
    void *param;               // 0x494
    const char *sortKey;       // 0x498
    int sortAscending;         // 0x49C
    u32 publicIp;              // 0x4A0, own address as seen by the master server
    u32 unk_4A4;               // 0x4A4
    u16 defaultPort;           // 0x4A8
    char *errorMessage;        // 0x4AC
    int socket;                // 0x4B0
    u32 lanStartTime;          // 0x4B4
    int unk_4B8;               // 0x4B8
    u8 cryptState[0x108];      // 0x4BC, see ov00_02212124 / ov00_022122B8
    u32 queryOptions;          // 0x5C4
    int readStage;             // 0x5C8
    u32 unk_5CC;               // 0x5CC
    SBServer *deadList;        // 0x5D0
};

// Queue of servers (linked through SBServer.next)
typedef struct SBServerQueue {
    SBServer *first;
    SBServer *last;
    int count;
} SBServerQueue;

typedef struct SBQueryEngine SBQueryEngine;
typedef void (*SBEngineCallback)(SBQueryEngine *engine, int reason, SBServer *server, void *param);

// Sends UDP queries to the listed servers, at most maxUpdates at a time
struct SBQueryEngine {
    int queryVersion;          // 0x00, 1: binary queries, else "\\basic\\" / "\\status\\" text queries
    int maxUpdates;            // 0x04
    SBServerQueue querying;    // 0x08
    SBServerQueue pending;     // 0x14
    int socket;                // 0x20
    int unk_24;                // 0x24
    u32 publicIp;              // 0x28, servers behind this address are queried at their private address
    u8 keys[20];               // 0x2C
    int numKeys;               // 0x40
    SBEngineCallback callback; // 0x44
    void *param;               // 0x48
};

typedef struct ServerBrowser ServerBrowser;
typedef void (*ServerBrowserCallback)(ServerBrowser *browser, int reason, SBServer *server, void *param);

struct ServerBrowser {
    SBQueryEngine engine;           // 0x000
    SBServerList list;              // 0x04C
    int unk_620;                    // 0x620, disconnect from the master server once the list is complete
    int unk_624;                    // 0x624
    u32 triggerIp;                  // 0x628
    u16 triggerPort;                // 0x62C
    ServerBrowserCallback callback; // 0x630
    void *param;                    // 0x634
};

// sb_server.c globals (asm data, overlapping ov00_0221B9D4)
typedef struct SBServerGlobals {
    char *tokenNext; // ov00_02212F60
    SBServer *nullServer;
    void *refStrings; // ov00_02212C08
} SBServerGlobals;

extern SBServerGlobals ov00_0221B9D0;

// sb_serverlist.c globals (asm data): the list being sorted and an optional master server host name
typedef struct SBListGlobals {
    SBServerList *sortList;
    char *masterHost;
} SBListGlobals;

extern SBServer *ov00_0221B9D4; // the "no server" value passed to the list callback

// sb_serverlist.c
int ov00_022139F0(SBServer **a, SBServer **b);
int ov00_02213A4C(SBServer **a, SBServer **b);
int ov00_02213B24(SBServer **a, SBServer **b);
int ov00_02213B8C(SBServer **a, SBServer **b);
void ov00_02213BF4(SBServerList *list, int ascending, const char *key, u32 type);
int ov00_02213CA8(SBServerList *list, SBServer *server);
int ov00_02213CFC(SBServerList *list, u32 ip, u16 port);
void ov00_02213D6C(SBServerList *list, SBServer *server);
void ov00_02213DA0(SBServerList *list, int index);
int ov00_02213DEC(SBServerList *list);
SBServer *ov00_02213DFC(SBServerList *list, int index);
void ov00_02213E10(SBServerList *list);
void ov00_02213E68(SBServerList *list);
void ov00_02213EC0(SBServerList *list);
char *ov00_02213EE8(SBServerList *list, const char *str);
void ov00_02213F58(SBServerList *list, const char *str);
int ov00_02213FD4(const char *str, int len);
void ov00_02214004(SBServerList *list, const char *targetGameName, const char *gameName, const char *secretKey, int unk_4B8, int force, SBListCallback callback, void *param);
void ov00_02214130(SBServerList *list);
u32 ov00_022141EC(const char *str, u32 mod);
int ov00_02214244(SBServerList *list);
void ov00_0221446C(SBServerList *list);
int ov00_02214558(SBServerList *list, const void *data, int size);
int ov00_022145F0(SBServerList *list, const char *keys, const char *filter, u32 options, int unk);
void ov00_02214860(SBServerList *list);
void ov00_022148A4(SBServerList *list);
void ov00_02214908(SBServerList *list);
void ov00_0221496C(SBServerList *list);
int ov00_02214A44(int flags);
int ov00_02214F2C(SBServerList *list, const u8 *data, int len);
void ov00_0221505C(SBServerList *list, char *message);
int ov00_02215064(SBServerList *list);
int ov00_02215A30(SBServerList *list);
int ov00_02215BF4(SBServerList *list);
int ov00_02215CDC(SBServerList *list, u32 ip, u16 port, void *data, int len);
int ov00_02215E04(SBServerList *list, u32 ip, u16 port, u32 value);
int ov00_02215EBC(SBServerList *list);
int ov00_02215FE0(SBServerList *list);

// sb_server.c
void ov00_02212A58(SBQueryEngine *engine);
void ov00_02212AE8(SBQueryEngine *engine);
void ov00_02212B28(SBQueryEngine *engine);
void ov00_02212B88(SBQueryEngine *engine, int key);
void ov00_02212BA8(SBQueryEngine *engine, SBServer *server);
u32 ov00_02212BD4(const void *elem, u32 buckets);
int ov00_02212BE4(const void *a, const void *b);
void ov00_02212BF8(void *elem);
void *ov00_02212C08(SBServerList *list);
void ov00_02212C6C(SBServerList *list);
void ov00_02212CAC(SBServer **server);
void ov00_02212CD0(SBServer *server, const char *key, const char *value);
void ov00_02212D10(SBServer *server, const char *key, int value);
char *ov00_02212D48(SBServer *server, const char *key, char *fallback);
int ov00_02212DB4(SBServer *server, const char *key, int fallback);
double ov00_02212E44(SBServer *server, const char *key, double fallback);
u32 ov00_02212E78(SBServer *server);
u16 ov00_02212E80(SBServer *server);
int ov00_02212EA4(SBServer *server);
int ov00_02212EAC(SBServer *server);
u32 ov00_02212EC4(SBServer *server);
u16 ov00_02212ECC(SBServer *server);
void ov00_02212EF0(SBServer *server, SBServer *next);
SBServer *ov00_02212EF8(SBServer *server);
int ov00_02212F00(const char *key);
char *ov00_02212F60(char *input, int delim);
void ov00_02212FC4(SBServer *server, char *str);
void ov00_02213044(SBServer *server, const char *data, int len);
u32 ov00_0221321C(const char *str, u32 mod);
void ov00_02213274(char **pair);
u32 ov00_02213298(char **pair, u32 mod);
int ov00_022132A8(char **a, char **b);
u32 ov00_022132BC(SBServer *server);
SBServer *ov00_022132C4(SBServerList *list, u32 ip, u16 port);
void ov00_02213370(SBServer *server, u8 flags);
void ov00_02213378(SBServer *server, u32 ip, u16 port);
void ov00_02213384(SBServer *server, u32 ip);
void ov00_0221338C(SBServer *server, u8 state);
u8 ov00_02213394(SBServer *server);
int ov00_0221339C(SBServer *server);
void ov00_022133B8(SBServerList *list, int reason, SBServer *server, ServerBrowser *browser);
void ov00_02213570(SBQueryEngine *engine, int reason, SBServer *server, ServerBrowser *browser);
ServerBrowser *ov00_02213610(const char *targetGameName, const char *gameName, const char *secretKey, int unk_4B8, int maxUpdates, int queryVersion, int force, ServerBrowserCallback callback, void *param);
void ov00_022136E4(ServerBrowser *browser);
int ov00_02213708(ServerBrowser *browser, int async, int unk_620, u8 *keys, int numKeys, const char *filter, u32 options, int unk);
int ov00_02213838(ServerBrowser *browser, int async, int unk_620, u8 *keys, int numKeys, const char *filter, int unk);
int ov00_0221386C(ServerBrowser *browser, const char *address, int port, void *data, int len);
int ov00_022138C4(ServerBrowser *browser, const char *address, int port, u32 value);
void ov00_0221390C(ServerBrowser *browser, SBServer *server);
int ov00_02213938(ServerBrowser *browser);
void ov00_02213950(ServerBrowser *browser);
void ov00_0221396C(ServerBrowser *browser);
int ov00_02213984(ServerBrowser *browser);
SBServer *ov00_022139B8(ServerBrowser *browser, int index);
int ov00_022139C8(ServerBrowser *browser);
void ov00_022139D8(ServerBrowser *browser, int ascending, const char *key, u32 type);
u32 ov00_022139E8(ServerBrowser *browser);

// sb_query_engine.c
u8 ov00_02212028(u8 *state, u32 max, u8 *key, u32 len, u8 *value, u32 *pos);
void ov00_022120DC(u8 *state);
u8 ov00_022121EC(u8 *state, u8 value);
void ov00_022122F4(SBServerQueue *queue, SBServer *server);
void ov00_02212328(SBServerQueue *queue, SBServer *server);
SBServer *ov00_02212350(SBServerQueue *queue);
int ov00_02212384(SBServerQueue *queue, SBServer *server);
void ov00_022123F0(SBServerQueue *queue);
void ov00_02212404(SBQueryEngine *engine, SBServer *server);
void ov00_022125E4(SBQueryEngine *engine, int maxUpdates, int queryVersion, int force, SBEngineCallback callback, void *param);
void ov00_02212658(SBQueryEngine *engine, u32 publicIp);
void ov00_02212660(SBQueryEngine *engine);
void ov00_0221267C(SBQueryEngine *engine);
void ov00_022126A8(SBQueryEngine *engine, SBServer *server, int first, int mode);
void ov00_02212724(SBQueryEngine *engine, SBServer *server, s8 *data, int len);
void ov00_02212824(SBQueryEngine *engine, SBServer *server, char *data, int len);
int ov00_022128BC(SBQueryEngine *engine, SBServer *server, char *data, int len);
void ov00_022128C4(SBQueryEngine *engine, int second);

// list request encryption state (ov00_02212124 initialises it with the challenge, ov00_022122B8 decodes)
void ov00_02212124(u8 *state, s8 *key, u32 len);
void ov00_022122B8(u8 *state, u8 *data, int len);

#endif // POKEHEARTGOLD_LIB_INCLUDE_DWC_SB_INTERNAL_H
