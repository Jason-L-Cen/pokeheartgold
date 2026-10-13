#pragma force_active on
#include <string.h>

#include "global.h"

#include "dwc/gt_internal.h"

// The code below is compiled as ARM (32-bit) code.
#include <nitro/code32.h>

// Reliable UDP sockets: the protocol messages (reliable messages with their serial numbers, acks,
// pings, challenge / response), the connection table, socket and connection creation and teardown,
// raw sends, and the "host:port" address conversions.

typedef struct GTInAddr {
    u32 addr;
} GTInAddr;

typedef struct GTHostEnt {
    u8 unk_00[12];
    u32 **addresses; // 0x0C
} GTHostEnt;

typedef struct GTBindAddr {
    u16 port;
    u16 unk_02;
    int length;
    u32 host;
    GTSockAddr address;
} GTBindAddr;

extern const char ov00_02219C94[]; // "gti2EndReliableMessage"
extern const u8 ov00_02219CB4[];   // 0xFE 0xFE, protocol magic
extern const char ov00_02219CB8[]; // "len > 0"
extern const char ov00_02219CC0[]; // "gt2Message.c"
extern const char ov00_02219CD0[]; // "gt2StringToAddress"
extern const char ov00_02219CE4[]; // "%s:%d"
extern const char ov00_02219CEC[]; // "%s"
extern const char ov00_02219CF0[]; // ":%d"
extern const char ov00_02219CF4[]; // "len < GTI2_STACK_HOSTLEN_MAX"
extern const char ov00_02219D14[]; // "gt2Utility.c"
extern int ov00_0221B634;          // which of the two address string buffers is next
extern char ov00_0221B638[];       // two address string buffers of 22 bytes
extern u16 __msl_digit[];

extern int sprintf(char *, const char *, ...);
extern long atol(const char *);
extern char *strchr(const char *, int);
extern void __msl_assertion_failed(const char *, const char *, const char *, int);
extern char *SOC_InetNtoA(GTInAddr addr);
extern GTHostEnt *SOC_GetHostByName(const char *);
extern void *ov00_021EC3A8(u32 size);
extern void ov00_021EC3D8(void *p);
extern int ArrayLength(void *);
extern void *ov00_021FE5B4(int, int, void *);
extern void ov00_021FE678(void *);
extern void *ov00_021FE6EC(void *, int);
extern int ov00_021FE754(void *, void *);
extern void ov00_021FE968(void *, int);
extern void *ov00_021FED4C(int, int, int, void *, void *, void *);
extern void ov00_021FEE8C(void *);
extern void ov00_021FEF74(void *, void *);
extern void ov00_021FF01C(void *, void *);
extern void *ov00_021FF0C0(void *, void *);
extern int ov00_021FF264(void *, void *, void *);
extern u32 ov00_021FF368(void);
extern void ov00_021FF3CC(void);
extern void ov00_021FF3D0(void);
extern int ov00_021FF6BC(int sock);
extern int ov00_021FF804(int, int, int);
extern void ov00_021FF818(int);
extern int ov00_021FF840(int, void *, int);
extern int ov00_021FF984(int sock, const u8 *data, int len, int flags, GTSockAddr *to, int toLen);
extern int ov00_021FFA18(int, void *, int *);
extern u32 ov00_021FFA44(const char *);
extern int ov00_021FFA60(int sock);
extern void gti2MessageCheck(const u8 **data, int *len);

int gti2AddOutgoingMessage(GTConn *conn, int serial, int len);
int gti2StartReliableMessage(GTConn *conn, int type, int len, int *overflow);
int gti2EndReliableMessage(GTConn *conn);
int gti2SendAppReliable(GTConn *conn, void *msg, int len);
int gti2SendClientChallenge(GTConn *conn, void *challenge);
int gti2SendServerChallenge(GTConn *conn, void *response, void *challenge);
int gti2SendClientResponse(GTConn *conn, void *response, void *msg, int len);
int gti2SendAccept(GTConn *conn);
int gti2SendReject(GTConn *conn, void *msg, int len);
int gti2SendClose(GTConn *conn);
int gti2SendKeepAlive(GTConn *conn);
int gti2SendAppUnreliable(GTConn *conn, u8 *msg, int len);
int gti2SendAck(GTConn *conn);
int gti2SendNack(GTConn *conn, u16 first, u16 last);
int gti2SendPong(GTConn *conn, u8 *msg, int len);
int gti2SendClosed(GTConn *conn);
int gti2SendClosedToAddress(GTSocket *socket, u32 ip, u16 port);
int gti2ResendMessage(GTConn *conn, GTOutMsg *msg);
int gti2Send(GTConn *conn, void *msg, int len, int reliable);
u32 gti2ConnectionHash(GTConn **conn, u32 numBuckets);
int gti2ConnectionCompare(GTConn **a, GTConn **b);
void gti2ConnectionFree(GTConn **conn);
GTConn *gti2SocketFindConnection(GTSocket *socket, u32 ip, int port);
int gti2CreateSocket(GTSocket **out, char *address, int outgoingBufferSize, int incomingBufferSize, void *socketErrorCB);
void gti2CloseSocket(GTSocket *socket);
void gti2Listen(GTSocket *socket, int (*connectAttemptCB)());
GTConn *gti2AllocConnection(void);
int gti2NewSocketConnection(GTSocket *socket, GTConn **out, u32 ip, int port);
void gti2FreeSocketConnection(GTConn *conn);
int gti2SocketSend(GTSocket *socket, u32 ip, u16 port, const u8 *data, int len);
int gti2SocketConnectionsThinkMap(GTConn **conn, u32 *now);
int gti2SocketConnectionsThink(GTSocket *socket);
void gti2FreeClosedConnections(GTSocket *socket);
void gti2SocketError(GTSocket *socket);
char *gt2AddressToString(u32 ip, u16 port, char *buf);
int gt2StringToAddress(const char *text, u32 *ip, u16 *port);

// Remembers where the next reliable message starts in the outgoing buffer.
int gti2AddOutgoingMessage(GTConn *conn, int serial, int len) {
    GTOutMsg msg = { 0 };
    int n;
    msg.start = conn->outgoingBuffer.len;
    msg.len = len;
    msg.serial = serial;
    msg.lastSend = ov00_021FF368();
    n = ArrayLength(conn->outgoingBufferedMessages);
    ov00_021FE754(conn->outgoingBufferedMessages, &msg);
    return n + 1 == ArrayLength(conn->outgoingBufferedMessages);
}

// Writes the header of a reliable message: magic, type, serial number, expected serial number.
int gti2StartReliableMessage(GTConn *conn, int type, int len, int *overflow) {
    if (gti2GetBufferFreeSpace(&conn->outgoingBuffer) < len) {
        if (!gti2ConnectionOutOfMemoryError(conn)) {
            return 0;
        }
        *overflow = 1;
        return 1;
    }
    if (!gti2AddOutgoingMessage(conn, conn->serialNumber, len)) {
        if (!gti2ConnectionOutOfMemoryError(conn)) {
            return 0;
        }
        *overflow = 1;
        return 1;
    }
    gti2BufferWriteData(&conn->outgoingBuffer, ov00_02219CB4, 2);
    gti2BufferWriteByte(&conn->outgoingBuffer, type);
    type = conn->serialNumber;
    conn->serialNumber = type + 1;
    gti2BufferWriteUShort(&conn->outgoingBuffer, type);
    gti2BufferWriteUShort(&conn->outgoingBuffer, conn->expectedSerialNumber);
    *overflow = 0;
    return 1;
}

int gti2EndReliableMessage(GTConn *conn) {
    int n = ArrayLength(conn->outgoingBufferedMessages);
    GTOutMsg *msg;
    if (n <= 0) {
        __msl_assertion_failed(ov00_02219CB8, ov00_02219CC0, ov00_02219C94, 0x475);
    }
    msg = ov00_021FE6EC(conn->outgoingBufferedMessages, n - 1);
    if (gti2ConnectionSendData(conn, conn->outgoingBuffer.buffer + msg->start, msg->len)) {
        conn->pendingAck = 0;
        return 1;
    }
    return 0;
}

// Reliable user message (type 0).
int gti2SendAppReliable(GTConn *conn, void *msg, int len) {
    int overflow;
    if (!gti2StartReliableMessage(conn, 0, len + 7, &overflow)) {
        return 0;
    }
    if (overflow) {
        return 1;
    }
    gti2BufferWriteData(&conn->outgoingBuffer, msg, len);
    if (gti2EndReliableMessage(conn)) {
        return 1;
    }
    return 0;
}

// Challenge (type 1).
int gti2SendClientChallenge(GTConn *conn, void *challenge) {
    int overflow;
    if (!gti2StartReliableMessage(conn, 1, 0x27, &overflow)) {
        return 0;
    }
    if (overflow) {
        return 1;
    }
    gti2BufferWriteData(&conn->outgoingBuffer, challenge, 0x20);
    if (gti2EndReliableMessage(conn)) {
        return 1;
    }
    return 0;
}

// Response + own challenge (type 2).
int gti2SendServerChallenge(GTConn *conn, void *response, void *challenge) {
    int overflow;
    if (!gti2StartReliableMessage(conn, 2, 0x47, &overflow)) {
        return 0;
    }
    if (overflow) {
        return 1;
    }
    gti2BufferWriteData(&conn->outgoingBuffer, response, 0x20);
    gti2BufferWriteData(&conn->outgoingBuffer, challenge, 0x20);
    if (!gti2EndReliableMessage(conn)) {
        return 0;
    }
    conn->challengeTime = conn->lastSend;
    return 1;
}

// Response + initial message (type 3).
int gti2SendClientResponse(GTConn *conn, void *response, void *msg, int len) {
    int overflow;
    if (!gti2StartReliableMessage(conn, 3, len + 0x27, &overflow)) {
        return 0;
    }
    if (overflow) {
        return 1;
    }
    gti2BufferWriteData(&conn->outgoingBuffer, response, 0x20);
    gti2BufferWriteData(&conn->outgoingBuffer, msg, len);
    if (gti2EndReliableMessage(conn)) {
        return 1;
    }
    return 0;
}

// Accept (type 4).
int gti2SendAccept(GTConn *conn) {
    int overflow;
    if (!gti2StartReliableMessage(conn, 4, 7, &overflow)) {
        return 0;
    }
    if (overflow) {
        return 1;
    }
    if (gti2EndReliableMessage(conn)) {
        return 1;
    }
    return 0;
}

// Reject with a message (type 5).
int gti2SendReject(GTConn *conn, void *msg, int len) {
    int overflow;
    if (!gti2StartReliableMessage(conn, 5, len + 7, &overflow)) {
        return 0;
    }
    if (overflow) {
        return 1;
    }
    gti2BufferWriteData(&conn->outgoingBuffer, msg, len);
    if (gti2EndReliableMessage(conn)) {
        return 1;
    }
    return 0;
}

// Close (type 6).
int gti2SendClose(GTConn *conn) {
    int overflow;
    if (!gti2StartReliableMessage(conn, 6, 7, &overflow)) {
        return 0;
    }
    if (overflow) {
        return 1;
    }
    if (gti2EndReliableMessage(conn)) {
        return 1;
    }
    return 0;
}

// Keep alive (type 7).
int gti2SendKeepAlive(GTConn *conn) {
    int overflow;
    if (!gti2StartReliableMessage(conn, 7, 7, &overflow)) {
        return 0;
    }
    if (overflow) {
        return 1;
    }
    if (gti2EndReliableMessage(conn)) {
        return 1;
    }
    return 0;
}

// Unreliable message; one that starts with the magic gets it doubled so it isn't taken for a protocol
// message.
int gti2SendAppUnreliable(GTConn *conn, u8 *msg, int len) {
    int pos;
    u8 *buffer;
    int n;
    if (len < 2 || memcmp(msg, ov00_02219CB4, 2)) {
        int result = gti2ConnectionSendData(conn, msg, len);
        if (result == 0) {
            return 0;
        }
        return 1;
    }
    n = len + 2;
    if (gti2GetBufferFreeSpace(&conn->outgoingBuffer) < n) {
        return 1;
    }
    buffer = conn->outgoingBuffer.buffer;
    pos = conn->outgoingBuffer.len;
    gti2BufferWriteData(&conn->outgoingBuffer, ov00_02219CB4, 2);
    gti2BufferWriteData(&conn->outgoingBuffer, msg, len);
    if (!gti2ConnectionSendData(conn, buffer + pos, n)) {
        return 0;
    }
    gti2BufferShorten(&conn->outgoingBuffer, -1, n);
    return 1;
}

// Ack (type 100).
int gti2SendAck(GTConn *conn) {
    u8 msg[5];
    memcpy(msg, ov00_02219CB4, 2);
    msg[2] = 100;
    gti2WriteUShort(msg, 3, conn->expectedSerialNumber);
    if (gti2ConnectionSendData(conn, msg, 5)) {
        conn->pendingAck = 0;
        return 1;
    }
    return 0;
}

// Nack of a range of serial numbers (type 101).
int gti2SendNack(GTConn *conn, u16 first, u16 last) {
    u8 msg[7];
    int len = 0;
    memcpy(msg, ov00_02219CB4, 2);
    msg[2] = 101;
    gti2WriteUShort(msg, 3, first);
    len += 5;
    if (first != last) {
        gti2WriteUShort(msg, len, last);
        len += 2;
    }
    if (!gti2ConnectionSendData(conn, msg, len)) {
        return 0;
    }
    return 1;
}

// Pong (type 103): the received ping sent back.
int gti2SendPong(GTConn *conn, u8 *msg, int len) {
    msg[2] = 103;
    return gti2ConnectionSendData(conn, msg, len);
}

int gti2SendClosed(GTConn *conn) {
    return gti2SendClosedToAddress(conn->socket, conn->ip, conn->port);
}

// Closed (type 104), sent to an address without a connection.
int gti2SendClosedToAddress(GTSocket *socket, u32 ip, u16 port) {
    u8 msg[3];
    memcpy(msg, ov00_02219CB4, 2);
    msg[2] = 104;
    return gti2SocketSend(socket, ip, port, msg, 3) != 0;
}

// Resends a reliable message with the current expected serial number.
int gti2ResendMessage(GTConn *conn, GTOutMsg *msg) {
    gti2WriteUShort(conn->outgoingBuffer.buffer, msg->start + 5, conn->expectedSerialNumber);
    if (!gti2ConnectionSendData(conn, conn->outgoingBuffer.buffer + msg->start, msg->len)) {
        return 0;
    }
    msg->lastSend = conn->lastSend;
    if (conn->outgoingBuffer.buffer[msg->start + 2] == 2) {
        conn->challengeTime = conn->lastSend;
    }
    return 1;
}

int gti2Send(GTConn *conn, void *msg, int len, int reliable) {
    if (reliable) {
        return gti2SendAppReliable(conn, msg, len);
    }
    return gti2SendAppUnreliable(conn, msg, len);
}

// Connection table hash, compare and free functions.
u32 gti2ConnectionHash(GTConn **conn, u32 numBuckets) {
    GTConn *c = *conn;
    return ((u32)c->ip * (u32)c->port) % numBuckets;
}

int gti2ConnectionCompare(GTConn **a, GTConn **b) {
    GTConn *p = *a, *q = *b;
    if (p->ip != q->ip) {
        return p->ip - q->ip;
    }
    return (s16)(p->port - q->port);
}

void gti2ConnectionFree(GTConn **conn) {
    gti2ConnectionCleanup(*conn);
}

GTConn *gti2SocketFindConnection(GTSocket *socket, u32 ip, int port) {
    GTConn key;
    GTConn *p;
    GTConn **found;
    key.ip = ip;
    key.port = port;
    p = &key;
    found = ov00_021FF0C0(socket->connections, &p);
    return found ? *found : 0;
}

static inline u16 GTSwap16(u16 value) {
    int v = value;
    return ((v >> 8) & 255) | ((v << 8) & 0xff00);
}

int gti2CreateSocket(GTSocket **out, char *address, int outgoingBufferSize, int incomingBufferSize, void *socketErrorCB) {
    GTBindAddr local;
    GTSocket *socket;
    ov00_021FF3CC();
    if (incomingBufferSize == 0) {
        incomingBufferSize = 65536;
    }
    if (outgoingBufferSize == 0) {
        outgoingBufferSize = 65536;
    }
    if (gt2StringToAddress(address, &local.host, &local.port) == 0) {
        return 4;
    }
    socket = ov00_021EC3A8(sizeof(GTSocket));
    if (socket == 0) {
        return 1;
    }
    memset(socket, 0, sizeof(GTSocket));
    socket->handle = -1;
    socket->incomingBufferSize = incomingBufferSize;
    socket->outgoingBufferSize = outgoingBufferSize;
    socket->socketErrorCB = socketErrorCB;
    socket->connections = ov00_021FED4C(4, 32, 2, gti2ConnectionHash, gti2ConnectionCompare, 0);
    if (socket->connections == 0) {
        ov00_021EC3D8(socket);
        return 1;
    }
    socket->closedConnections = ov00_021FE5B4(4, 4, gti2ConnectionFree);
    if (socket->closedConnections == 0) {
        ov00_021FEE8C(socket->connections);
        ov00_021EC3D8(socket);
        return 1;
    }
    socket->handle = ov00_021FF804(2, 2, 0);
    if (socket->handle == -1) {
        ov00_021FEE8C(socket->connections);
        ov00_021FE678(socket->closedConnections);
        ov00_021EC3D8(socket);
        return 3;
    }
    memset(&local.address, 0, 8);
    local.address.family = 2;
    local.address.addr = local.host;
    local.address.port = GTSwap16(local.port);
    if (ov00_021FF840(socket->handle, &local.address, 8) == -1) {
        ov00_021FF818(socket->handle);
        ov00_021FEE8C(socket->connections);
        ov00_021FE678(socket->closedConnections);
        ov00_021EC3D8(socket);
        return 3;
    }
    local.length = 8;
    ov00_021FFA18(socket->handle, &local.address, &local.length);
    socket->ip = local.address.addr;
    socket->port = GTSwap16(local.address.port);
    *out = socket;
    return 0;
}

void gti2CloseSocket(GTSocket *socket) {
    if (socket->callbackLevel) {
        socket->close = 1;
        return;
    }
    ov00_021FF818(socket->handle);
    ov00_021FEE8C(socket->connections);
    ov00_021FE678(socket->closedConnections);
    ov00_021EC3D8(socket);
    ov00_021FF3D0();
}

void gti2Listen(GTSocket *socket, int (*connectAttemptCB)()) {
    socket->connectAttemptCB = connectAttemptCB;
}

GTConn *gti2AllocConnection(void) {
    return ov00_021EC3A8(sizeof(GTConn));
}

int gti2NewSocketConnection(GTSocket *socket, GTConn **out, u32 ip, int port) {
    GTConn *conn = 0;
    if (gti2SocketFindConnection(socket, ip, port)) {
        return 5;
    }
    conn = gti2AllocConnection();
    if (conn) {
        memset(conn, 0, sizeof(GTConn));
        conn->ip = ip;
        conn->port = port;
        conn->socket = socket;
        conn->startTime = ov00_021FF368();
        conn->lastSend = conn->startTime;
        conn->serialNumber = 0;
        conn->expectedSerialNumber = 0;
        if (!gti2AllocateBuffer(&conn->incomingBuffer, socket->incomingBufferSize)) {
            goto fail;
        }
        if (!gti2AllocateBuffer(&conn->outgoingBuffer, socket->outgoingBufferSize)) {
            goto fail;
        }
        conn->incomingBufferedMessages = ov00_021FE5B4(0x10, 0x40, 0);
        if (!conn->incomingBufferedMessages) {
            goto fail;
        }
        conn->outgoingBufferedMessages = ov00_021FE5B4(0x10, 0x40, 0);
        if (!conn->outgoingBufferedMessages) {
            goto fail;
        }
        conn->sendFilters = ov00_021FE5B4(4, 2, 0);
        if (!conn->sendFilters) {
            goto fail;
        }
        conn->receiveFilters = ov00_021FE5B4(4, 2, 0);
        if (!conn->receiveFilters) {
            goto fail;
        }
        ov00_021FEF74(socket->connections, &conn);
        *out = gti2SocketFindConnection(socket, ip, port);
        if (*out) {
            return 0;
        }
    }
fail:
    if (conn) {
        ov00_021EC3D8(conn->incomingBuffer.buffer);
        ov00_021EC3D8(conn->outgoingBuffer.buffer);
        if (conn->incomingBufferedMessages) {
            ov00_021FE678(conn->incomingBufferedMessages);
        }
        if (conn->outgoingBufferedMessages) {
            ov00_021FE678(conn->outgoingBufferedMessages);
        }
        if (conn->sendFilters) {
            ov00_021FE678(conn->sendFilters);
        }
        if (conn->receiveFilters) {
            ov00_021FE678(conn->receiveFilters);
        }
        ov00_021EC3D8(conn);
    }
    return 1;
}

// Removes a connection from its socket's tables (unless a callback is running on it).
void gti2FreeSocketConnection(GTConn *conn) {
    int n, i;
    if (conn->freeAtAcceptReject || conn->callbackLevel) {
        return;
    }
    if (conn->state == 7) {
        n = ArrayLength(conn->socket->closedConnections);
        for (i = 0; i < n; i++) {
            if (conn == *(GTConn **)ov00_021FE6EC(conn->socket->closedConnections, i)) {
                ov00_021FE968(conn->socket->closedConnections, i);
                return;
            }
        }
    } else {
        ov00_021FF01C(conn->socket->connections, &conn);
    }
}

int gti2SocketSend(GTSocket *socket, u32 ip, u16 port, const u8 *data, int len) {
    int err;
    gti2MessageCheck(&data, &len);
    if (!ov00_021FF6BC(socket->handle)) {
        return 1;
    }
    {
        GTSockAddr to = { 0 };
        to.family = 2;
        to.port = ((port >> 8) & 0xff) | ((port << 8) & 0xff00);
        to.addr = ip;
        err = ov00_021FF984(socket->handle, data, len, 0, &to, sizeof(to));
    }
    if (err == -1) {
        err = ov00_021FFA60(socket->handle);
        if (err == -15) {
            if (gti2HandleConnectionReset(socket, ip, port) == 0) {
                return 0;
            }
        } else {
            if (err == -42 || err == -6) {
                return 1;
            }
            if (err != -35) {
                gti2SocketError(socket);
                return 0;
            }
        }
    } else if (socket->unk28 != 0) {
        if (gti2DumpCallback(socket, gti2SocketFindConnection(socket, ip, port), ip, port, 0, (u8 *)data, len, 1) == 0) {
            return 0;
        }
    }
    return 1;
}

int gti2SocketConnectionsThinkMap(GTConn **connp, u32 *now) {
    GTConn *conn = *connp;
    u32 time = *now;
    if (conn->state != 7 && !gti2ConnectionThink(conn, time)) {
        return 0;
    }
    if (conn->state == 7 && !conn->freeAtAcceptReject && !conn->callbackLevel) {
        gti2FreeSocketConnection(conn);
    }
    return 1;
}

int gti2SocketConnectionsThink(GTSocket *socket) {
    u32 now = ov00_021FF368();
    return !ov00_021FF264(socket->connections, gti2SocketConnectionsThinkMap, &now);
}

void gti2FreeClosedConnections(GTSocket *socket) {
    int i;
    for (i = ArrayLength(socket->closedConnections) - 1; i >= 0; i--) {
        gti2FreeSocketConnection(*(GTConn **)ov00_021FE6EC(socket->closedConnections, i));
    }
}

void gti2SocketError(GTSocket *socket) {
    if (socket->error) {
        return;
    }
    socket->error = 1;
    gt2CloseAllConnectionsHard(socket);
    if (gti2SocketErrorCallback(socket)) {
        gti2CloseSocket(socket);
    }
}

static inline int GTIsDigit(int c) {
    return (c < 0 || c >= 128) ? 0 : (__msl_digit[c] & 8);
}

// "a.b.c.d:port" in buf, or in one of two alternating static buffers when buf is NULL.
char *gt2AddressToString(u32 ip, u16 port, char *buf) {
    if (buf == 0) {
        ov00_0221B634 ^= 1;
        buf = ov00_0221B638 + ov00_0221B634 * 22;
    }
    if (ip) {
        GTInAddr addr;
        addr.addr = ip;
        if (port) {
            sprintf(buf, ov00_02219CE4, SOC_InetNtoA(addr), port);
        } else {
            sprintf(buf, ov00_02219CEC, SOC_InetNtoA(addr));
        }
    } else {
        if (port) {
            sprintf(buf, ov00_02219CF0, port);
        } else {
            *buf = 0;
        }
    }
    return buf;
}

// Parses "host[:port]" (host name or dotted address, either part may be missing).
int gt2StringToAddress(const char *text, u32 *ip, u16 *port) {
    char local[260];
    u32 address;
    u16 portValue;
    char *colon;
    if (!text || !*text) {
        address = 0;
        portValue = 0;
    } else {
        colon = strchr(text, ':');
        if (!colon) {
            portValue = 0;
        } else {
            if (colon == text) {
                text = 0;
                address = 0;
            } else {
                int length = colon - text;
                if (length >= 256) {
                    __msl_assertion_failed(ov00_02219CF4, ov00_02219D14, ov00_02219CD0, 129);
                }
                memcpy(local, text, length);
                local[length] = 0;
                text = local;
            }
            {
                char *p = colon + 1;
                while (*p) {
                    if (!GTIsDigit(*p)) {
                        return 0;
                    }
                    p++;
                }
            }
            {
                long value = atol(colon + 1);
                if (value < 0 || value > 65535) {
                    return 0;
                }
                portValue = (u16)value;
            }
        }
        if (text) {
            address = ov00_021FFA44(text);
            if (address == 0xffffffff) {
                GTHostEnt *host = SOC_GetHostByName(text);
                if (!host) {
                    return 0;
                }
                address = *host->addresses[0];
            }
        }
    }
    if (ip) {
        *ip = address;
    }
    if (port) {
        *port = portValue;
    }
    return 1;
}
