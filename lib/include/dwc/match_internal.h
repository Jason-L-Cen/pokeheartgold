#ifndef POKEHEARTGOLD_LIB_INCLUDE_DWC_MATCH_INTERNAL_H
#define POKEHEARTGOLD_LIB_INCLUDE_DWC_MATCH_INTERNAL_H

// Matchmaking state of OVY_0 (lib/NitroDWC/src/match_browse.c, transport.c). The work area is
// registered with ov00_021F954C and fetched with ov00_021F955C.

// Timer for the "all peers ready" exchange, 0x20 bytes.
typedef struct MatchTimer {
    u8 active;
    u8 wanted;  // number of players wanted
    u8 retries; // 0x02
    u8 unk_03;
    u32 timeout; // 0x04, ms
    u32 mask;    // 0x08, aids that answered
    u32 all;     // 0x0C, aids expected
    u64 start;   // 0x10
    u64 last;    // 0x18
} MatchTimer;

// Custom server key, 12 bytes (ov00_0221A948[154], key ids 100..253).
typedef struct MatchKeyEntry {
    u8 id;
    u8 isString;  // 0x01
    u16 unk_02;   // 0x02
    void *buffer; // 0x04, owned by the entry (the key name for keys added with ov00_021F13A4)
    void *value;  // 0x08
} MatchKeyEntry;

typedef struct MatchAddr {
    u8 family;
    u8 unk_01;
    u16 port;    // 0x02
    u32 address; // 0x04
} MatchAddr;

typedef struct MatchRequest {
    u8 flag;
    u8 retries; // 0x01
    u8 unk_02[6];
    int active; // 0x08
} MatchRequest;

typedef struct MatchWork {
    void *unk_00;
    void **peer;    // 0x04
    void *callback; // 0x08
    u8 flag;        // 0x0C
    u8 players;     // 0x0D, connected players besides the local one
    u8 unk_0E;
    u8 unk_0F;
    void *reporting; // 0x10, query/reporting record, see ov00_021F179C
    u8 connections;  // 0x14
    u8 mode;         // 0x15
    u8 limit;        // 0x16
    u8 unk_17;
    u8 unk_18;
    u8 unk_19;
    u16 port;          // 0x1A
    u32 addr;          // 0x1C
    u32 unk_20;        // 0x20
    u32 addresses[32]; // 0x24
    u16 ports[32];     // 0xA4
    void *list;        // 0xE4, server list
    int unk_E8;        // 0xE8
    u64 unk_EC;        // 0xEC
    int profiles[32];  // 0xF4
    u64 pending;       // 0x174
    u8 retryCount;     // 0x17C
    u8 unk_17D;        // 0x17D
    u8 unk_17E[2];
    int retryProfile;     // 0x180
    u64 retryTick;        // 0x184
    u64 waitTick;         // 0x18C
    MatchRequest request; // 0x194
    int state;            // 0x1A0
    u8 initial;           // 0x1A4
    u8 current;           // 0x1A5
    u8 unk_1A6[3];
    u8 unk_1A9;
    u8 unk_1AA; // 0x1AA, retries in state 4
    u8 unk_1AB[2];
    u8 unk_1AD;     // 0x1AD
    u8 stopPending; // 0x1AE, ov00_021F12F0 was called before state 10
    u8 unk_1AF;
    u8 unk_1B0[4];
    u16 remotePort; // 0x1B4
    u16 unk_1B6;
    u32 remoteAddr; // 0x1B8
    int active;     // 0x1BC
    u64 activeTick; // 0x1C0
    u32 delay;      // 0x1C8, ms
    u64 time;       // 0x1CC
    u8 unk_1D4[8];
    u32 unk_1DC;
    u8 unk_1E0[0x10];
    u64 unk_1F0;  // 0x1F0
    u64 unk_1F8;  // 0x1F8
    u32 identity; // 0x200
    int target;   // 0x204
    u8 unk_208[8];
    u32 addr2[32];         // 0x210
    u16 port2[32];         // 0x290
    u8 aids[32];           // 0x2D0
    u32 aidMask;           // 0x2F0
    const char *gameName;  // 0x2F4
    const char *secretKey; // 0x2F8
    void *friends;         // 0x2FC
    u32 unk_300;
    u8 friendIds[64]; // 0x304
    int friendCount;  // 0x344
    u8 unk_348[0x84];
    u8 unk_3CC;                                  // 0x3CC, state of the last command sent
    u8 commandRetries;                           // 0x3CD
    u16 commandPort;                             // 0x3CE
    u32 commandAddr;                             // 0x3D0
    u8 commandData[0x80];                        // 0x3D4
    u32 commandProfile;                          // 0x454
    u32 commandSize;                             // 0x458
    u64 commandTick;                             // 0x45C
    u32 unk_464;                                 // 0x464
    u32 unk_468;                                 // 0x468
    void *unk_46C;                               // 0x46C, callback set by ov00_021F1B24 / ov00_021F1BCC
    void *unk_470;                               // 0x470, its parameter
    int (*evalCallback)(int index, void *param); // 0x474
    void *evalParam;                             // 0x478
    void (*stopCallback)(void *param);           // 0x47C, see ov00_021F12F0
    void *stopParam;                             // 0x480
} MatchWork;

typedef struct MatchGlobal {
    MatchTimer *timer;
    MatchWork *work; // 0x04
    char *filter;    // 0x08, extra server list filter, see ov00_021F1938
    int deferred;    // 0x0C, ov00_0220FB40 requested while updating
    int updating;    // 0x10, set while ov00_022103C4 runs
    int unk_14;      // 0x14
    int busy;        // 0x18, callback nesting depth
    u8 unk_1C;
    u8 unk_1D;
    u16 unk_1E;
} MatchGlobal;

extern MatchGlobal ov00_0221A928;

int ov00_021F94DC(int flag);
void ov00_021F954C(MatchWork *work);
MatchWork *ov00_021F955C(void);
void ov00_021F956C(int state);

extern MatchKeyEntry ov00_0221A948[];

// match_setup.c
void ov00_021F125C(void);
int ov00_021F1284(void);
int ov00_021F12A4(void);
int ov00_021F12F0(void (*callback)(void *param), void *param);
u32 ov00_021F13A4(u32 id, const char *name, void *value);
int ov00_021F14D4(int option, MatchTimer *param);
void ov00_021F1620(MatchWork *work, void *unk_00, void **peer, void *callback, const char *gameName, const char *secretKey, void *friends, u32 unk_300);
int ov00_021F179C(u32 identity);
void ov00_021F1938(int count, const char *filter, void *arg2, void *arg3, int (*evalCallback)(int index, void *param), void *evalParam);
void ov00_021F1B24(int count, void *arg1, void *arg2, void *callback, void *param);
void ov00_021F1BCC(int profile, void *arg1, void *arg2, void *callback, void *param);
void ov00_021F1D34(void *reporting);
void ov00_021F1DE4(BOOL active);

// match_browse.c
u8 ov00_021F7A98(void);
void ov00_021F7AD8(void);
void ov00_021F7DDC(int flag);
void ov00_021F7E4C(void);
int ov00_021F8278(unsigned error);
int ov00_021F82EC(unsigned error);
int ov00_021F8380(int error);
void ov00_021F85B8(void *list, int event, void *arg);
void ov00_021F8D18(int id, void *out);
void ov00_021F8E60(void);
void ov00_021F8E64(void);
void ov00_021F8E68(int state, void *p);
int ov00_021F8F0C(void);
int ov00_021F8F14(unsigned error);
void ov00_021F8F20(u32 addr, u16 port);
void ov00_021F8F40(int profile);
void ov00_021F8FF4(void *src, u32 size);
void ov00_021F9104(int result, int unused, MatchAddr *addr, MatchRequest *req);

#endif // POKEHEARTGOLD_LIB_INCLUDE_DWC_MATCH_INTERNAL_H
