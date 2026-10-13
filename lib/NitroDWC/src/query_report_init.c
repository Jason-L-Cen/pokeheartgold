#pragma force_active on
#include "global.h"

int sprintf(char *, const char *, ...);

#pragma force_active on

// The code below is compiled as ARM (32-bit) code.
#include <nitro/code32.h>

// Query / report record creation: game name, secret key, the six user callbacks, a random instance
// key, and the master server address ("<game name>.master..." port 27900 unless overridden).

typedef struct QueryReport {
    int socket;         // 0x000
    char gameName[64];  // 0x004
    char secretKey[64]; // 0x044
    u8 instanceKey[4];  // 0x084, random
    u32 callbacks[6];   // 0x088
    u32 unk_A0;         // 0x0A0
    u32 unk_A4;         // 0x0A4
    u32 unk_A8;         // 0x0A8
    u32 unk_AC;         // 0x0AC
    u32 unk_B0;         // 0x0B0
    u32 unk_B4;         // 0x0B4
    u32 unk_B8;         // 0x0B8
    u32 isPublic;       // 0x0BC, report to the master server
    u32 port;           // 0x0C0
    u32 unk_C4;         // 0x0C4
    u32 unk_C8;         // 0x0C8
    u8 masterAddr[8];   // 0x0CC
    u32 unk_D4;         // 0x0D4
    u32 unk_D8[10];     // 0x0D8
    u32 unk_100;        // 0x100
    u32 unk_104;        // 0x104
    u16 unk_108;        // 0x108
    u32 userData;       // 0x10C
} QueryReport;

extern QueryReport ov00_02219D74;  // the record used when the caller passes no out pointer
extern const char ov00_02219E84[]; // "%s.master.gs.nintendowifi.net"
extern u32 ov00_0221B878;          // master server address initialised
extern char ov00_0221B890[];       // master server host name override

void *DWCi_GsMalloc(u32);
u32 current_time(void);
void get_local_ips(void);
int get_sockaddrin(const char *, u32, void *, int);
int qr2_init_socketA(QueryReport **out, u32 socket, u32 port, const char *gameName, const char *secretKey, u32 isPublic, u32 p6, u32 cb0, u32 cb1, u32 cb2, u32 cb3, u32 cb4, u32 cb5, u32 userData);

#pragma opt_strength_reduction off

int qr2_init_socketA(QueryReport **out, u32 socket, u32 port, const char *gameName, const char *secretKey, u32 isPublic, u32 p6, u32 cb0, u32 cb1, u32 cb2, u32 cb3, u32 cb4, u32 cb5, u32 userData) {
    int i, status;
    QueryReport *qr;
    char buffer[64];
    if (out) {
        qr = *out = DWCi_GsMalloc(sizeof(QueryReport));
    } else {
        qr = &ov00_02219D74;
    }
    srand(current_time());
    strcpy(qr->gameName, gameName);
    strcpy(qr->secretKey, secretKey);
    qr->port = port;
    qr->unk_AC = 0;
    qr->unk_B0 = 0;
    qr->socket = socket;
    qr->unk_B8 = 1;
    qr->userData = userData;
    qr->callbacks[0] = cb0;
    qr->callbacks[1] = cb1;
    qr->callbacks[2] = cb2;
    qr->callbacks[3] = cb3;
    qr->callbacks[4] = cb4;
    qr->callbacks[5] = cb5;
    qr->unk_A0 = 0;
    qr->unk_A4 = 0;
    qr->unk_D4 = 0;
    qr->isPublic = isPublic;
    qr->unk_C4 = 0;
    qr->unk_C8 = p6;
    qr->unk_104 = 0;
    qr->unk_108 = 0;
    qr->unk_A8 = 0;
    qr->unk_B4 = 0;
    for (i = 0; i < 4; i++) {
        qr->instanceKey[i] = rand() % 255;
    }
    for (i = 0; i < 10; i++) {
        qr->unk_D8[i] = -1;
    }
    qr->unk_100 = 0;
    if (!ov00_0221B878) {
        get_local_ips();
    }
    if (isPublic) {
        s8 first = ov00_0221B890[0];
        if (!first) {
            sprintf(buffer, ov00_02219E84, gameName);
        }
        status = get_sockaddrin(first ? ov00_0221B890 : buffer, 0x6cfc, qr->masterAddr, 0);
    } else {
        status = 1;
    }
    if (status == 0) {
        return 3;
    } else {
        return 0;
    }
}
