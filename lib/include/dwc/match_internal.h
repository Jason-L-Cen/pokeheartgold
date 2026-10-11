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

typedef struct MatchWork {
    void *unk_00;
    void **peer;    // 0x04
    void *callback; // 0x08
    u8 flag;        // 0x0C
    u8 players;     // 0x0D, connected players besides the local one
    u8 unk_0E[6];
    u8 connections; // 0x14
    u8 mode;        // 0x15
    u8 limit;       // 0x16
    u8 unk_17[3];
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
    int retryProfile; // 0x180
    u64 retryTick;    // 0x184
    u64 waitTick;     // 0x18C
    u8 unk_194[12];
    int state;  // 0x1A0
    u8 initial; // 0x1A4
    u8 current; // 0x1A5
    u8 unk_1A6[7];
    u8 unk_1AD; // 0x1AD
    u8 unk_1AE[6];
    u16 remotePort; // 0x1B4
    u16 unk_1B6;
    u32 remoteAddr; // 0x1B8
    int active;     // 0x1BC
    u8 unk_1C0[8];
    int delay; // 0x1C8
    u64 time;  // 0x1CC
    u8 unk_1D4[0x24];
    u64 unk_1F8;  // 0x1F8
    u32 identity; // 0x200
    int target;   // 0x204
    u8 unk_208[8];
    u32 addr2[32]; // 0x210
    u16 port2[32]; // 0x290
    u8 aids[32];   // 0x2D0
    u32 aidMask;   // 0x2F0
    u8 unk_2F4[8];
    void *friends; // 0x2FC
    u32 unk_300;
    u8 friendIds[64]; // 0x304
    int friendCount;  // 0x344
    u8 unk_348[0x84];
    u8 unk_3CC; // 0x3CC
    u8 unk_3CD[0xA7];
    int (*evalCallback)(int index, void *param); // 0x474
    void *evalParam;                             // 0x478
} MatchWork;

typedef struct MatchGlobal {
    MatchTimer *timer;
    MatchWork *work; // 0x04
    u32 unk_08[4];
    int busy; // 0x18, callback nesting depth
} MatchGlobal;

extern MatchGlobal ov00_0221A928;

int ov00_021F94DC(int flag);
void ov00_021F954C(MatchWork *work);
MatchWork *ov00_021F955C(void);
void ov00_021F956C(int state);

#endif // POKEHEARTGOLD_LIB_INCLUDE_DWC_MATCH_INTERNAL_H
