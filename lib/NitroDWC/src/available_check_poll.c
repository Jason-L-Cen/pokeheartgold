#include "global.h"
#include "dwc/available_check_internal.h"

// The code below is compiled as ARM (32-bit) code.
#include <nitro/code32.h>

// Polling of the game server availability query ("<game>.available.gs.nintendowifi.net"). The request
// is sent by GSIStartAvailableCheck / gsiAvailableCheckSendPacket and the reply parsed by gsiAvailableCheckParseResponse; this object owns
// the query's .bss (tools/object_aliases.py splits ov00_0221B0D8 with the sidecar alias map).

#ifdef HEARTGOLD_NATIVE
typedef GsiAvailableSockAddr SockAddr;
GsiAvailableCheckState gsiAvailableCheckState;
#define sNatSock (gsiAvailableCheckState.sock)
#define QUERY_SEND_TIME (gsiAvailableCheckState.time)
#define QUERY_RETRIES (gsiAvailableCheckState.retries)
#else
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
#define QUERY_SEND_TIME (ov00_0221B0D8.sendTime)
#define QUERY_RETRIES (ov00_0221B0D8.retries)
#endif

int CanReceiveOnSocket(int sock);
int gsSocketRecvFrom(int sock, void *buf, int size, int flags, void *from, int *fromLen);
int gsiAvailableCheckParseResponse(u8 *buf, int len, SockAddr *from, u32 *result);
int gsSocketClose(int sock);
u32 current_time(void);
void gsiAvailableCheckSendPacket(void);
int GSIAvailableCheckThink(void);

// Returns 0 while waiting for the reply, else the result also stored in ov00_0221B0D0:
// 1 available (or no socket / no reply after one resend), 2 and 3 for the two "not available" bits of the reply.
int GSIAvailableCheckThink(void) {
    SockAddr from;
    u8 buf[0x40];
    int fromLen = 8;
    u32 result;
    int len;

    if (sNatSock == -1) {
        GSI_AVAILABLE_RESULT = 1;
        return 1;
    }
    if (CanReceiveOnSocket(sNatSock)) {
        len = gsSocketRecvFrom(sNatSock, buf, sizeof(buf), 0, &from, &fromLen);
        if (gsiAvailableCheckParseResponse(buf, len, &from, &result) == 0) {
            gsSocketClose(sNatSock);
            if (result & 1) {
                GSI_AVAILABLE_RESULT = 2;
            } else if (result & 2) {
                GSI_AVAILABLE_RESULT = 3;
            } else {
                GSI_AVAILABLE_RESULT = 1;
            }
            return GSI_AVAILABLE_RESULT;
        }
    }
    if (current_time() > QUERY_SEND_TIME + 2000) {
        if (QUERY_RETRIES == 1) {
            gsSocketClose(sNatSock);
            GSI_AVAILABLE_RESULT = 1;
            return 1;
        }
        gsiAvailableCheckSendPacket();
        QUERY_RETRIES++;
    }
    return 0;
}
