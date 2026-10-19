#ifndef POKEHEARTGOLD_DWC_AVAILABLE_CHECK_INTERNAL_H
#define POKEHEARTGOLD_DWC_AVAILABLE_CHECK_INTERNAL_H
#include <nitro/types.h>

#ifdef HEARTGOLD_NATIVE
typedef struct GsiAvailableSockAddr {
    u8 len,family;
    u16 port;
    u32 addr;
} GsiAvailableSockAddr;

// Original ov00_0221B0D0..0221B12B, including the socket and interior assembly aliases.
typedef struct GsiAvailableCheckState {
    int state,sock;
    GsiAvailableSockAddr addr; // ov00_0221B0D8; address word at ov00_0221B0DC
    u8 buf[0x40];             // ov00_0221B0E0; game name starts at ov00_0221B0E5
    int len;
    u32 time;
    int retries;
} GsiAvailableCheckState;
extern GsiAvailableCheckState gsiAvailableCheckState;
#define GSI_AVAILABLE_RESULT (gsiAvailableCheckState.state)
#else
extern int ov00_0221B0D0;
#define GSI_AVAILABLE_RESULT ov00_0221B0D0
#endif
#endif
