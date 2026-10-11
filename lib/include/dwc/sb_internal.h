#ifndef POKEHEARTGOLD_LIB_INCLUDE_DWC_SB_INTERNAL_H
#define POKEHEARTGOLD_LIB_INCLUDE_DWC_SB_INTERNAL_H

// Server browser of OVY_0 (lib/NitroDWC/src/sb_serverlist.c): the server objects, the server list and
// its TCP connection to the master server.

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
void *ov00_02212C08(SBServerList *list);
void ov00_02212C6C(SBServerList *list);
void ov00_02212CAC(SBServer **server);
void ov00_02212CD0(SBServer *server, const char *key, const char *value);
void ov00_02212D10(SBServer *server, const char *key, int value);
char *ov00_02212D48(SBServer *server, const char *key, char *fallback);
int ov00_02212DB4(SBServer *server, const char *key, int fallback);
double ov00_02212E44(SBServer *server, const char *key, double fallback);
u32 ov00_02212E78(SBServer *server);
int ov00_02212EA4(SBServer *server);
void ov00_02212EF0(SBServer *server, SBServer *next);
SBServer *ov00_02212EF8(SBServer *server);
SBServer *ov00_022132C4(SBServerList *list, u32 ip, u16 port);
void ov00_02213370(SBServer *server, u8 flags);
void ov00_02213378(SBServer *server, u32 ip, u16 port);
void ov00_02213384(SBServer *server, u32 ip);
void ov00_0221338C(SBServer *server, u8 state);
u8 ov00_02213394(SBServer *server);
int ov00_0221339C(SBServer *server);

// list request encryption state (ov00_02212124 initialises it with the challenge, ov00_022122B8 decodes)
void ov00_02212124(u8 *state, s8 *key, u32 len);
void ov00_022122B8(u8 *state, u8 *data, int len);

#endif // POKEHEARTGOLD_LIB_INCLUDE_DWC_SB_INTERNAL_H
