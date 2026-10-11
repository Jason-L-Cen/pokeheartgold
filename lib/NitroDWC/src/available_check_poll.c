#include "global.h"

// The code below is compiled as ARM (32-bit) code.
#include <nitro/code32.h>

// Polling of the game server availability query ("<game>.available.gs.nintendowifi.net"). The request
// is sent by ov00_021FE228 / ov00_021FE1D8 and the reply parsed by ov00_021FE320; this object owns
// the query's .bss (tools/object_aliases.py splits ov00_0221B0D8 with the sidecar alias map).

typedef struct SockAddr {
    u32 addr;
    u32 port;
} SockAddr;

typedef struct AvailableCheckWork {
    SockAddr server;
    u8 packet[0x40];
    int packetLen;
    u32 sendTime;
    int retries;
} AvailableCheckWork;

int ov00_0221B0D0;
AvailableCheckWork ov00_0221B0D8;
static int sNatSock;

int ov00_021FF694(int sock);
int ov00_021FF940(int sock, u8 *buf, int size, int flags, SockAddr *from, int *fromLen);
int ov00_021FE320(u8 *buf, int len, SockAddr *from, u32 *result);
void ov00_021FF818(int sock);
u32 ov00_021FF368(void);
void ov00_021FE1D8(void);
int ov00_021FE3D8(void);

// Returns 0 while waiting for the reply, else the result also stored in ov00_0221B0D0:
// 1 available (or no socket / no reply after one resend), 2 and 3 for the two "not available" bits of the reply.
int ov00_021FE3D8(void) {
    SockAddr from;
    u8 buf[0x40];
    int fromLen = 8;
    u32 result;
    int len;

    if (sNatSock == -1) {
        ov00_0221B0D0 = 1;
        return 1;
    }
    if (ov00_021FF694(sNatSock)) {
        len = ov00_021FF940(sNatSock, buf, sizeof(buf), 0, &from, &fromLen);
        if (ov00_021FE320(buf, len, &from, &result) == 0) {
            ov00_021FF818(sNatSock);
            if (result & 1) {
                ov00_0221B0D0 = 2;
            } else if (result & 2) {
                ov00_0221B0D0 = 3;
            } else {
                ov00_0221B0D0 = 1;
            }
            return ov00_0221B0D0;
        }
    }
    if (ov00_021FF368() > ov00_0221B0D8.sendTime + 2000) {
        if (ov00_0221B0D8.retries == 1) {
            ov00_021FF818(sNatSock);
            ov00_0221B0D0 = 1;
            return 1;
        }
        ov00_021FE1D8();
        ov00_0221B0D8.retries++;
    }
    return 0;
}
