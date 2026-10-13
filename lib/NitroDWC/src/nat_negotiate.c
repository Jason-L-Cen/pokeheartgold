#pragma force_active on
#include <string.h>

#include "global.h"

// The code below is compiled as ARM (32-bit) code.
#include <nitro/code32.h>

// NAT negotiation: a negotiator sends init packets (from its own socket and optionally the game socket)
// to the two negotiation servers, which pair it with the peer of the same cookie and send both sides
// each other's public address; the peers then ping each other until both have seen the other.
// Starts with the reliable UDP sockets' message size helper.

typedef void (*NatProgressCB)(int state, void *param);
typedef void (*NatCompletedCB)(int result, int socket, void *address, void *param);

typedef enum NatState {
    NAT_STATE_INIT_SENT,  // init packets sent, waiting for the servers' acks
    NAT_STATE_INIT_ACKED, // waiting for the peer's address
    NAT_STATE_CONNECTING, // pinging the peer
    NAT_STATE_CONNECTED,  // both sides have seen each other
    NAT_STATE_FINISHED,   // to be removed
} NatState;

typedef struct NatNegotiator {
    int socket;                 // 0x00
    int gameSocket;             // 0x04, -1: negotiate on socket only
    int cookie;                 // 0x08
    int clientIndex;            // 0x0C
    int state;                  // 0x10, NatState
    int initAcked[3];           // 0x14, per init packet
    int retries;                // 0x20
    int maxRetries;             // 0x24
    u32 deadline;               // 0x28
    u32 remoteIp;               // 0x2C
    u16 remotePort;             // 0x30
    u8 gotRemoteData;           // 0x32
    u8 sentAck;                 // 0x33
    NatProgressCB progressCB;   // 0x34
    NatCompletedCB completedCB; // 0x38
    void *param;                // 0x3C
} NatNegotiator;

typedef struct NatGlobals {
    const char *server2Name; // 0x00, NULL: default host name
    const char *server1Name; // 0x04
    u32 server2Ip;           // 0x08
    u32 server1Ip;           // 0x0C
    void *negotiators;       // 0x10, DArray of NatNegotiator
} NatGlobals;

typedef struct NatSockAddr {
    u8 len;
    u8 family;
    u16 port;
    u32 addr;
} NatSockAddr;

typedef struct NatHostEnt {
    char *name;
    char **aliases;
    s16 addrType;
    s16 length;
    u8 **addrList;
} NatHostEnt;

typedef struct NatHostList {
    u32 unk_00; // 0x0
    u32 unk_04; // 0x4
    u32 unk_08; // 0x8
    u32 **addresses;
} NatHostList;

// Init packet (type 0), 22 bytes + game name.
typedef struct NatInitPacket {
    u8 magic[6];
    u8 version;
    u8 type;
    u32 cookie;
    u8 portType; // which of the three init packets
    u8 clientIndex;
    u8 useGameSocket;
    u8 localIp[4];
    u8 localPort[2];
    char gameName[64];
} NatInitPacket;

// Init ack (type 1) and the address check (type 2) sent back as type 3, 21 bytes.
typedef struct NatAckPacket {
    u8 magic[6];
    u8 version;
    u8 type;
    u8 cookie[4];
    u8 portType;
} NatAckPacket;

// Connect (type 5, from the server) and connect ping (type 7, from the peer), 20 bytes.
typedef struct NatConnectPacket {
    u8 magic[6];
    u8 version;
    u8 type;
    u8 cookie[4];
    u32 remoteIp;
    u16 remotePort;
    u8 gotYourData;
    u8 finished; // connect: 0 = ok, 1 / 2 / other = error
} NatConnectPacket;

typedef struct NatPacket20 {
    u8 bytes[20];
} NatPacket20;

typedef struct NatPacket21 {
    u8 bytes[21];
} NatPacket21;

extern const char ov00_02219D24[]; // ""
extern const u8 ov00_02219D28[6];  // packet magic
extern const char ov00_02219D30[]; // "%s.%s"
extern const char ov00_02219D38[]; // "natneg1.gs.nintendowifi.net"
extern const char ov00_02219D54[]; // "natneg2.gs.nintendowifi.net"
extern int ov00_0221B0D0[];        // [0] == 1: network available
extern char ov00_0221B16C[];       // game name
extern NatGlobals ov00_0221B664[];
extern u8 ov00_0221B678[512]; // receive buffer

#define NAT_HTONS(x) ((((x) >> 8) & 0xff) | (((x) << 8) & 0xff00))
#define NAT_HTONL(x) ((((x) >> 24) & 0xff) | (((x) >> 8) & 0xff00) | (((x) << 8) & 0xff0000) | (((x) << 24) & 0xff000000))

extern int snprintf(char *, unsigned int, const char *, ...);
extern int sprintf(char *, const char *, ...);
extern NatHostEnt *SOC_GetHostByName(const char *name);
extern int ArrayLength(void *);
extern void *ov00_021FE5B4(int, int, void *);
extern void ov00_021FE678(void *);
extern void *ov00_021FE6EC(void *, int);
extern int ov00_021FE754(void *, void *);
extern void ov00_021FE8D8(void *, int);
extern u32 ov00_021FF368(void);
extern int ov00_021FF694(int);
extern NatHostList *ov00_021FF6E4(void);
extern int ov00_021FF770(u32 *);
extern int ov00_021FF804(int, int, int);
extern void ov00_021FF818(int);
extern int ov00_021FF940(int, void *, int, int, NatSockAddr *, int *);
extern int ov00_021FF984(int, void *, int, int, void *, int);
extern int gsSocketGetSockName(int, void *, int *);
extern u32 gsSocketInetAddr(const char *name);

void gti2MessageCheck(char **message, int *len);
NatNegotiator *ov00_0220F9A8(int cookie);
void ov00_0220FA10(NatNegotiator *negotiator);
NatNegotiator *ov00_0220FA40(void);
void ov00_0220FAD8(NatNegotiator *negotiator);
void ov00_0220FB40(void);
int ov00_0220FB6C(const void *packet);
int ov00_0220FB90(int socket, u32 ip, int port, void *buf, int len);
u32 ov00_0220FBE4(void);
u16 ov00_0220FC48(int socket);
void ov00_0220FC7C(NatNegotiator *negotiator);
void ov00_0220FEC4(NatNegotiator *negotiator);
u32 ov00_0220FFE4(const char *name);
u32 ov00_0221001C(const char *name, const char *defaultName);
int ov00_0221005C(void);
int ov00_022100D4(int gameSocket, int cookie, int clientIndex, NatProgressCB progressCB, NatCompletedCB completedCB, void *param);
void ov00_0221019C(int cookie);
void ov00_022101D4(void *arg);
void ov00_022103C4(void);
void ov00_02210408(NatNegotiator *negotiator, NatSockAddr *from);
void ov00_022104D4(NatNegotiator *negotiator, NatConnectPacket *packet, NatSockAddr *from);
void ov00_02210590(NatNegotiator *negotiator, NatConnectPacket *packet, NatSockAddr *from);
void ov00_02210658(NatNegotiator *negotiator, NatAckPacket *packet, NatSockAddr *from);
void ov00_0221075C(const u8 *packet, int size, void *from);

// A NULL message is sent as an empty one; a length of -1 means a string including its terminator.
void gti2MessageCheck(char **message, int *len) {
    char *p = *message;
    if (!p) {
        *message = (char *)ov00_02219D24;
        *len = 0;
        return;
    }
    if (*len == -1) {
        *len = strlen(p) + 1;
    }
}

NatNegotiator *ov00_0220F9A8(int cookie) {
    int i;
    NatNegotiator *negotiator;
    if (!ov00_0221B664->negotiators) {
        return 0;
    }
    for (i = 0; i < ArrayLength(ov00_0221B664->negotiators); i++) {
        negotiator = ov00_021FE6EC(ov00_0221B664->negotiators, i);
        if (negotiator->cookie == cookie) {
            return negotiator;
        }
    }
    return 0;
}

void ov00_0220FA10(NatNegotiator *negotiator) {
    if (negotiator->socket != -1) {
        ov00_021FF818(negotiator->socket);
    }
    negotiator->socket = -1;
    negotiator->state = NAT_STATE_FINISHED;
}

NatNegotiator *ov00_0220FA40(void) {
    int buf[16] = { 0 };
    if (!ov00_0221B664->negotiators) {
        ov00_0221B664->negotiators = ov00_021FE5B4(sizeof(NatNegotiator), 4, ov00_0220FA10);
    }
    ov00_021FE754(ov00_0221B664->negotiators, buf);
    return ov00_021FE6EC(ov00_0221B664->negotiators, ArrayLength(ov00_0221B664->negotiators) - 1);
}

void ov00_0220FAD8(NatNegotiator *negotiator) {
    int i;
    for (i = 0; i < ArrayLength(ov00_0221B664->negotiators); i++) {
        if (negotiator == ov00_021FE6EC(ov00_0221B664->negotiators, i)) {
            ov00_021FE8D8(ov00_0221B664->negotiators, i);
            return;
        }
    }
}

void ov00_0220FB40(void) {
    if (!ov00_0221B664->negotiators) {
        return;
    }
    ov00_021FE678(ov00_0221B664->negotiators);
    ov00_0221B664->negotiators = 0;
}

int ov00_0220FB6C(const void *packet) {
    return memcmp(packet, ov00_02219D28, 6) == 0;
}

int ov00_0220FB90(int socket, u32 ip, int port, void *buf, int len) {
    NatSockAddr address;
    address.addr = ip;
    address.family = 2;
    address.port = NAT_HTONS(port);
    return ov00_021FF984(socket, buf, len, 0, &address, 8);
}

#pragma opt_propagation off

// The first local address that is not 127.0.0.1 and passes ov00_021FF770.
u32 ov00_0220FBE4(void) {
    int i;
    NatHostList *host;
    u32 result = 0, value;
    host = ov00_021FF6E4();
    if (!host) {
        return result;
    }
    for (i = 0;; i++) {
        u32 *p = host->addresses[i];
        if (!p) {
            break;
        }
        value = *p;
        if (value != 0x0100007f) {
            result = value;
            if (ov00_021FF770(p) != 0) {
                return result;
            }
        }
    }
    return result;
}

#pragma opt_propagation reset

u16 ov00_0220FC48(int socket) {
    NatSockAddr address;
    int len = 8;
    if (gsSocketGetSockName(socket, &address, &len) == -1) {
        return 0;
    }
    return address.port;
}

// Sends the init packets the servers have not acknowledged yet.
void ov00_0220FC7C(NatNegotiator *negotiator) {
    u8 buf[0x58];
    NatInitPacket *packet = (NatInitPacket *)buf;
    u32 a, b, c, d;
    int len;
    u16 port;
    memcpy(packet->magic, ov00_02219D28, 6);
    packet->version = 2;
    packet->type = 0;
    packet->clientIndex = negotiator->clientIndex;
    packet->cookie = NAT_HTONL((u32)negotiator->cookie);
    packet->useGameSocket = negotiator->gameSocket != -1;
    a = ov00_0220FBE4();
    b = ov00_0220FBE4();
    c = ov00_0220FBE4();
    d = ov00_0220FBE4();
    d = ((c >> 24) & 0xff) | ((d >> 8) & 0xff00) | ((b << 8) & 0xff0000) | ((a << 24) & 0xff000000);
    buf[15] = d >> 24;
    buf[16] = d >> 16;
    buf[17] = d >> 8;
    buf[18] = d;
    buf[19] = 0;
    buf[20] = 0;
    strcpy((char *)&buf[21], ov00_0221B16C);
    len = strlen(ov00_0221B16C) + 22;
    if (packet->useGameSocket && negotiator->initAcked[0] == 0) {
        packet->portType = 0;
        ov00_0220FB90(negotiator->gameSocket, ov00_0221B664->server1Ip, 0x6cfd, packet, len);
    }
    if (negotiator->initAcked[1] == 0) {
        packet->portType = 1;
        ov00_0220FB90(negotiator->socket, ov00_0221B664->server1Ip, 0x6cfd, packet, len);
    }
    port = (u16)(((ov00_0220FC48(packet->useGameSocket ? negotiator->gameSocket : negotiator->socket) >> 8) & 0xff) | ((ov00_0220FC48(packet->useGameSocket ? negotiator->gameSocket : negotiator->socket) << 8) & 0xff00));
    buf[20] = port;
    buf[19] = port >> 8;
    if (negotiator->initAcked[2] == 0) {
        packet->portType = 2;
        ov00_0220FB90(negotiator->socket, ov00_0221B664->server2Ip, 0x6cfd, packet, len);
    }
    negotiator->deadline = ov00_021FF368() + 500;
    negotiator->maxRetries = 30;
}

// Pings the peer (connect ping, type 7: address the peer is seen at, got its data, finished).
void ov00_0220FEC4(NatNegotiator *negotiator) {
    u8 buf[20];
    int socket;
    memcpy(buf, ov00_02219D28, 6);
    buf[6] = 2;
    buf[7] = 7;
    *(u32 *)(buf + 8) = NAT_HTONL((u32)negotiator->cookie);
    *(u32 *)(buf + 12) = negotiator->remoteIp;
    *(u16 *)(buf + 16) = NAT_HTONS(negotiator->remotePort);
    buf[18] = negotiator->gotRemoteData;
    buf[19] = (negotiator->state != NAT_STATE_CONNECTING);
    socket = negotiator->gameSocket;
    if (socket == -1) {
        socket = negotiator->socket;
    }
    ov00_0220FB90(socket, negotiator->remoteIp, negotiator->remotePort, buf, 20);
    negotiator->deadline = ov00_021FF368() + 700;
    negotiator->maxRetries = 12;
    if (negotiator->gotRemoteData) {
        negotiator->sentAck = 1;
    }
}

u32 ov00_0220FFE4(const char *name) {
    NatHostEnt *host;
    u32 addr = gsSocketInetAddr(name);
    if (addr != (u32)-1) {
        return addr;
    }
    host = SOC_GetHostByName(name);
    if (host == NULL) {
        return 0;
    }
    return *(u32 *)host->addrList[0];
}

// Resolves name, or "<game name>.<defaultName>" when name is NULL.
u32 ov00_0221001C(const char *name, const char *defaultName) {
    char buf[128];
    if (!name) {
        snprintf(buf, 128, ov00_02219D30, ov00_0221B16C, defaultName);
        name = buf;
    }
    return ov00_0220FFE4(name);
}

int ov00_0221005C(void) {
    if (!ov00_0221B664->server1Ip) {
        ov00_0221B664->server1Ip = ov00_0221001C(ov00_0221B664->server1Name, ov00_02219D38);
    }
    if (!ov00_0221B664->server2Ip) {
        ov00_0221B664->server2Ip = ov00_0221001C(ov00_0221B664->server2Name, ov00_02219D54);
    }
    if (!ov00_0221B664->server1Ip || !ov00_0221B664->server2Ip) {
        return 0;
    }
    return 1;
}

int ov00_022100D4(int gameSocket, int cookie, int clientIndex, NatProgressCB progressCB, NatCompletedCB completedCB, void *param) {
    NatNegotiator *negotiator;
    if (ov00_0221B0D0[0] != 1) {
        return 2;
    }
    if (!ov00_0221005C()) {
        return 3;
    }
    negotiator = ov00_0220FA40();
    if (!negotiator) {
        return 1;
    }
    negotiator->gameSocket = gameSocket;
    negotiator->clientIndex = clientIndex;
    negotiator->cookie = cookie;
    negotiator->progressCB = progressCB;
    negotiator->completedCB = completedCB;
    negotiator->param = param;
    negotiator->socket = ov00_021FF804(2, 2, 0);
    negotiator->retries = 0;
    negotiator->gotRemoteData = 0;
    negotiator->sentAck = 0;
    negotiator->remoteIp = 0;
    negotiator->remotePort = 0;
    negotiator->maxRetries = 0;
    if (negotiator->socket == -1) {
        ov00_0220FAD8(negotiator);
        return 2;
    }
    ov00_0220FC7C(negotiator);
    return 0;
}

void ov00_0221019C(int cookie) {
    NatNegotiator *negotiator = ov00_0220F9A8(cookie);
    if (!negotiator) {
        return;
    }
    if (negotiator->socket != -1) {
        ov00_021FF818(negotiator->socket);
    }
    negotiator->socket = -1;
    negotiator->state = NAT_STATE_FINISHED;
}

void ov00_022101D4(void *arg) {
    NatNegotiator *negotiator = arg;
    NatSockAddr remote;
    int size = 8;
    NatSockAddr address;
    if (negotiator->state == NAT_STATE_FINISHED) {
        ov00_0220FAD8(negotiator);
        return;
    }
    if (negotiator->socket != size - 9) {
        for (;;) {
            if (ov00_021FF694(negotiator->socket) == 0) {
                break;
            }
            int got = ov00_021FF940(negotiator->socket, ov00_0221B678, 512, 0, &remote, &size);
            if (got == -1) {
                break;
            }
            ov00_0221075C(ov00_0221B678, got, &remote);
            if (negotiator->state == NAT_STATE_FINISHED || negotiator->socket == -1) {
                break;
            }
        }
    }
    if ((negotiator->state == NAT_STATE_INIT_SENT || negotiator->state == NAT_STATE_CONNECTING) && ov00_021FF368() > negotiator->deadline) {
        if (negotiator->retries > negotiator->maxRetries) {
            negotiator->completedCB(2, -1, 0, negotiator->param);
            ov00_0221019C(negotiator->cookie);
        } else {
            negotiator->retries++;
            if (negotiator->state == NAT_STATE_INIT_SENT) {
                ov00_0220FC7C(negotiator);
            } else {
                ov00_0220FEC4(negotiator);
            }
        }
    }
    if (negotiator->state == NAT_STATE_CONNECTED && ov00_021FF368() > negotiator->deadline) {
        if (negotiator->gameSocket == -1) {
            address.family = 2;
            address.port = (u16)((u8)(negotiator->remotePort >> 8) | ((negotiator->remotePort << 8) & 0xff00));
            address.addr = negotiator->remoteIp;
            negotiator->completedCB(0, negotiator->socket, &address, negotiator->param);
            negotiator->socket = -1;
        }
        ov00_0221019C(negotiator->cookie);
    }
    if (negotiator->state == NAT_STATE_INIT_ACKED && ov00_021FF368() > negotiator->deadline) {
        negotiator->completedCB(1, -1, 0, negotiator->param);
        ov00_0221019C(negotiator->cookie);
    }
}

void ov00_022103C4(void) {
    int i;
    if (!ov00_0221B664->negotiators) {
        return;
    }
    for (i = ArrayLength(ov00_0221B664->negotiators) - 1; i >= 0; i--) {
        ov00_022101D4(ov00_021FE6EC(ov00_0221B664->negotiators, i));
    }
}

// Acknowledges the server's connect packet (type 6, with the client index).
void ov00_02210408(NatNegotiator *negotiator, NatSockAddr *from) {
    u8 buf[21];
    memcpy(buf, ov00_02219D28, 6);
    buf[6] = 2;
    buf[7] = 6;
    buf[13] = negotiator->clientIndex;
    *(u32 *)(buf + 8) = NAT_HTONL((u32)negotiator->cookie);
    ov00_0220FB90(negotiator->socket, from->addr, (u16)NAT_HTONS(from->port), buf, 21);
}

// Connect packet from the server: the peer's address, or an error.
void ov00_022104D4(NatNegotiator *negotiator, NatConnectPacket *packet, NatSockAddr *from) {
    int err;
    if (packet->finished == 0) {
        ov00_02210408(negotiator, from);
    }
    if (negotiator->state >= NAT_STATE_CONNECTING) {
        return;
    }
    if (packet->finished != 0) {
        err = 3;
        if (packet->finished == 1) {
            err = 1;
        } else if (packet->finished == 2) {
            err = 2;
        }
        ((void (*)(int, int, int, void *))negotiator->completedCB)(err, -1, 0, negotiator->param);
        ov00_0221019C(negotiator->cookie);
        return;
    }
    negotiator->remoteIp = packet->remoteIp;
    negotiator->remotePort = NAT_HTONS(packet->remotePort);
    negotiator->retries = 0;
    negotiator->state = NAT_STATE_CONNECTING;
    negotiator->progressCB(negotiator->state, negotiator->param);
    ov00_0220FEC4(negotiator);
}

// Ping from the peer.
void ov00_02210590(NatNegotiator *negotiator, NatConnectPacket *packet, NatSockAddr *from) {
    if (negotiator->state < NAT_STATE_CONNECTING) {
        return;
    }
    negotiator->remoteIp = from->addr;
    negotiator->remotePort = NAT_HTONS(from->port);
    negotiator->gotRemoteData = 1;
    if (!packet->gotYourData) {
        ov00_0220FEC4(negotiator);
        return;
    }
    if (negotiator->state == NAT_STATE_CONNECTING) {
        if (!negotiator->sentAck) {
            ov00_0220FEC4(negotiator);
        }
        negotiator->state = NAT_STATE_CONNECTED;
        negotiator->deadline = ov00_021FF368() + 5000;
        if (negotiator->gameSocket == -1) {
            return;
        }
        negotiator->completedCB(0, negotiator->gameSocket, from, negotiator->param);
        return;
    }
    if (!packet->finished) {
        ov00_0220FEC4(negotiator);
    }
}

// Init ack (type 1) and address check (type 2) packets from the servers.
void ov00_02210658(NatNegotiator *negotiator, NatAckPacket *packet, NatSockAddr *from) {
    switch (packet->type) {
    case 1:
        if (packet->portType > 2) {
            return;
        }
        negotiator->initAcked[packet->portType] = 1;
        if (negotiator->state != NAT_STATE_INIT_SENT) {
            return;
        }
        if (negotiator->initAcked[1] == 0 || negotiator->initAcked[2] == 0) {
            return;
        }
        if (negotiator->gameSocket != -1 && negotiator->initAcked[0] == 0) {
            return;
        }
        negotiator->state = NAT_STATE_INIT_ACKED;
        negotiator->deadline = ov00_021FF368() + 10000;
        negotiator->progressCB(negotiator->state, negotiator->param);
        break;
    case 2:
        packet->type = 3;
        ov00_0220FB90(negotiator->socket, from->addr, (u16)NAT_HTONS(from->port), packet, 0x15);
        break;
    }
}

// Handles a received negotiation packet.
void ov00_0221075C(const u8 *packet, int size, void *from) {
    struct {
        NatPacket21 large;
        u8 padding[3];
        NatPacket20 small;
    } locals;

    int type;
    NatNegotiator *negotiator;
    if (!ov00_0220FB6C(packet)) {
        return;
    }
    type = packet[7];
    if (type == 5 || type == 7) {
        if (size < 20) {
            return;
        }
        locals.small = *(const NatPacket20 *)packet;
        negotiator = ov00_0220F9A8(NAT_HTONL(*(u32 *)(locals.small.bytes + 8)));
        if (!negotiator) {
            return;
        }
        if (type == 5) {
            ov00_022104D4(negotiator, (NatConnectPacket *)&locals.small, from);
        } else {
            ov00_02210590(negotiator, (NatConnectPacket *)&locals.small, from);
        }
    } else {
        if (size < 21) {
            return;
        }
        locals.large = *(const NatPacket21 *)packet;
        negotiator = ov00_0220F9A8(NAT_HTONL(*(u32 *)(locals.large.bytes + 8)));
        if (!negotiator) {
            return;
        }
        ov00_02210658(negotiator, (NatAckPacket *)&locals.large, from);
    }
}
