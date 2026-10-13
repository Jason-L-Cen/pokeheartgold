#pragma force_active on
#include "global.h"

#include "dwc/gt_internal.h"

// The code below is compiled as ARM (32-bit) code.
#include <nitro/code32.h>

// Reliable UDP sockets: the callbacks into the user code (connected, received, closed, ping, filters,
// unrecognized messages), creating / closing sockets and connections, sending, and processing the
// received messages (serial numbers, acknowledgements, buffered out-of-order messages, keep-alives).

extern char ov00_02219B1C[]; // "gti2DumpCallback"
extern char ov00_02219B30[]; // "gti2PingCallback"
extern char ov00_02219B44[]; // "gti2ClosedCallback"
extern char ov00_02219B58[]; // "gti2ReceivedCallback"
extern char ov00_02219B70[]; // "gti2ConnectedCallback"
extern char ov00_02219B88[]; // "gti2SendFilterCallback"
extern char ov00_02219BB8[]; // "gti2ReceiveFilterCallback"
extern char ov00_02219BF0[]; // "gti2UnrecognizedMessageCallback"
extern char ov00_02219C10[]; // "socket"
extern char ov00_02219C18[]; // "gt2Callback.c"
extern char ov00_02219C40[]; // "connection"
extern char ov00_02219C4C[]; // "gt2SetConnectionData"
extern char ov00_02219C64[]; // "gt2GetConnectionData"
extern char ov00_02219C7C[]; // "gt2Main.c"
extern char ov00_02219C88[]; // "connection"
extern char ov00_02219CAC[]; // "time"
extern char ov00_02219CB4[]; // 0xFE 0xFE
extern int ov00_0221B630;    // set while a socket is being closed

extern void __msl_assertion_failed(const char *expr, const char *file, const char *func, int line);
extern void *memcpy(void *, const void *, u32);
extern int memcmp(const void *, const void *, u32);
extern void gti2CloseSocket(GTSocket *socket);
extern void *ov00_021FE6EC(void *array, int n);
extern int ArrayLength(void *array);
extern void ov00_021FE968(void *array, int n);
extern void ov00_021FE678(void *array);
extern void ov00_021FE754(void *array, void *elem);
extern void ov00_021FE858(void *array, void *elem, void *cmp);
extern void ov00_021FF01C(void *table, void *elem);
extern void ov00_021FF1F8(void *table, void *fn, void *user);
extern u32 ov00_021FF368(void);
extern void ov00_021FF3C0(int ms);
extern int ov00_021FF694(int sock);
extern int ov00_021FF940(int sock, void *buf, int len, int flags, void *addr, int *addrlen);
extern int gsSocketGetLastError(int sock);
extern void *ov00_021EC3A8(int size);
extern void ov00_021EC3D8(void *ptr);
extern int gti2NewSocketConnection(GTSocket *socket, GTConn **conn, u32 ip, u16 port);
extern void gti2MessageCheck(u8 **msg, int *len);
extern int gti2ConnectAttemptCallback(GTSocket *socket, GTConn *conn, u32 ip, u16 port, int latency, u8 *msg, int len);
extern int gti2SendClientChallenge(GTConn *conn, u8 *challenge);
extern int gti2SendServerChallenge(GTConn *conn, u8 *response, u8 *challenge);
extern int gti2SendClientResponse(GTConn *conn, u8 *response, u8 *msg, int len);
extern int gti2SendAccept(GTConn *conn);
extern int gti2SendReject(GTConn *conn, u8 *msg, int len);
extern int gti2SendClose(GTConn *conn);
extern int gti2SendKeepAlive(GTConn *conn);
extern int gti2SendAck(GTConn *conn);
extern int gti2SendNack(GTConn *conn, u16 a, u16 b);
extern int gti2SendPong(GTConn *conn, u8 *msg, int len);
extern int gti2SendClosed(GTConn *conn);
extern int gti2SendClosedToAddress(GTSocket *socket, u32 ip, u16 port);
extern int gti2ResendMessage(GTConn *conn, GTOutMsg *msg);
extern int gti2Send(GTConn *conn, u8 *msg, int len, int reliable);
extern GTConn *gti2SocketFindConnection(GTSocket *socket, u32 ip, u16 port);
extern int gti2CreateSocket(void *a, void *b, void *c, void *d, void *e);
extern int gti2Listen(void *a, void *b, void *c, void *d);
extern void gti2FreeSocketConnection(GTConn *conn);
extern int gti2SocketSend(GTSocket *socket, u32 ip, u16 port, u8 *msg, int len);
extern int gti2SocketConnectionsThink(GTSocket *socket);
extern void gti2FreeClosedConnections(GTSocket *socket);
extern void gti2SocketError(GTSocket *socket);
extern int gt2StringToAddress(const char *addr, u32 *ip, u16 *port);

int gti2ConnectedCallback(GTConn *conn, int result, u8 *msg, int len) {
    if (!conn) {
        __msl_assertion_failed(ov00_02219C40, ov00_02219C18, ov00_02219B70, 0x69);
    }
    if (!conn) {
        return 1;
    }
    conn->connectionResult = result;
    if (!conn->callbacks.connected) {
        return 1;
    }
    if (!len || !msg) {
        msg = NULL;
        len = 0;
    }
    conn->callbackLevel++;
    conn->socket->callbackLevel++;
    ((GTConnectedCB)conn->callbacks.connected)(conn, result, msg, len);
    conn->callbackLevel--;
    conn->socket->callbackLevel--;
    if (conn->socket->close && !conn->socket->callbackLevel) {
        gti2CloseSocket(conn->socket);
        return 0;
    }
    return 1;
}

int gti2ReceivedCallback(GTConn *conn, u8 *msg, int len, int reliable) {
    if (!conn) {
        __msl_assertion_failed(ov00_02219C40, ov00_02219C18, ov00_02219B58, 0x94);
    }
    if (!conn) {
        return 1;
    }
    if (!conn->callbacks.received) {
        return 1;
    }
    if (!len || !msg) {
        msg = NULL;
        len = 0;
    }
    conn->callbackLevel++;
    conn->socket->callbackLevel++;
    ((GTReceivedCB)conn->callbacks.received)(conn, msg, len, reliable);
    conn->callbackLevel--;
    conn->socket->callbackLevel--;
    if (conn->socket->close && !conn->socket->callbackLevel) {
        gti2CloseSocket(conn->socket);
        return 0;
    }
    return 1;
}

int gti2ClosedCallback(GTConn *conn, int reason) {
    if (!conn) {
        __msl_assertion_failed(ov00_02219C40, ov00_02219C18, ov00_02219B44, 0xba);
    }
    if (!conn) {
        return 1;
    }
    if (!conn->callbacks.closed) {
        return 1;
    }
    conn->callbackLevel++;
    conn->socket->callbackLevel++;
    ((GTClosedCB)conn->callbacks.closed)(conn, reason);
    conn->callbackLevel--;
    conn->socket->callbackLevel--;
    if (conn->socket->close && !conn->socket->callbackLevel) {
        gti2CloseSocket(conn->socket);
        return 0;
    }
    return 1;
}

int gti2PingCallback(GTConn *conn, int latency) {
    if (!conn) {
        __msl_assertion_failed(ov00_02219C40, ov00_02219C18, ov00_02219B30, 0xd9);
    }
    if (!conn) {
        return 1;
    }
    if (!conn->callbacks.ping) {
        return 1;
    }
    conn->callbackLevel++;
    conn->socket->callbackLevel++;
    ((GTPingCB)conn->callbacks.ping)(conn, latency);
    conn->callbackLevel--;
    conn->socket->callbackLevel--;
    if (conn->socket->close && !conn->socket->callbackLevel) {
        gti2CloseSocket(conn->socket);
        return 0;
    }
    return 1;
}

int gti2SendFilterCallback(GTConn *conn, int index, u8 *msg, int len, int reliable) {
    GTFilter *filter;
    if (!conn) {
        __msl_assertion_failed(ov00_02219C40, ov00_02219C18, ov00_02219B88, 0x101);
    }
    if (!conn) {
        return 1;
    }
    filter = ov00_021FE6EC(conn->sendFilters, index);
    if (!filter) {
        return 1;
    }
    if (!len || !msg) {
        msg = NULL;
        len = 0;
    }
    conn->callbackLevel++;
    conn->socket->callbackLevel++;
    filter->callback(conn, index, msg, len, reliable);
    conn->callbackLevel--;
    conn->socket->callbackLevel--;
    if (conn->socket->close && !conn->socket->callbackLevel) {
        gti2CloseSocket(conn->socket);
        return 0;
    }
    return 1;
}

int gti2ReceiveFilterCallback(GTConn *conn, int index, u8 *msg, int len, int reliable) {
    GTFilter *filter;
    if (!conn) {
        __msl_assertion_failed(ov00_02219C40, ov00_02219C18, ov00_02219BB8, 0x12d);
    }
    if (!conn) {
        return 1;
    }
    filter = ov00_021FE6EC(conn->receiveFilters, index);
    if (!filter) {
        return 1;
    }
    if (!len || !msg) {
        msg = NULL;
        len = 0;
    }
    conn->callbackLevel++;
    conn->socket->callbackLevel++;
    filter->callback(conn, index, msg, len, reliable);
    conn->callbackLevel--;
    conn->socket->callbackLevel--;
    if (conn->socket->close && !conn->socket->callbackLevel) {
        gti2CloseSocket(conn->socket);
        return 0;
    }
    return 1;
}

int gti2DumpCallback(GTSocket *socket, GTConn *conn, u32 ip, u16 port, int reset, u8 *msg, int len, int fatal) {
    GTSockErrCB cb;
    if (!socket) {
        __msl_assertion_failed(ov00_02219C10, ov00_02219C18, ov00_02219B1C, 0x160);
    }
    if (!socket) {
        return 1;
    }
    if (fatal) {
        cb = socket->unk28;
    } else {
        cb = socket->unk2C;
    }
    if (!cb) {
        return 1;
    }
    if (!len || !msg) {
        len = 0;
        msg = NULL;
    }
    socket->callbackLevel++;
    if (conn) {
        conn->callbackLevel++;
    }
    cb(socket, conn, ip, port, reset, msg, len);
    socket->callbackLevel--;
    if (conn) {
        conn->callbackLevel--;
    }
    if (socket->close && !socket->callbackLevel) {
        gti2CloseSocket(socket);
        return 0;
    }
    return 1;
}

int gti2UnrecognizedMessageCallback(GTSocket *socket, u32 ip, u16 port, u8 *msg, int len, int *handled) {
    *handled = 0;
    if (!socket) {
        __msl_assertion_failed(ov00_02219C10, ov00_02219C18, ov00_02219BF0, 0x197);
    }
    if (!socket) {
        return 1;
    }
    if (!socket->unrecognizedCB) {
        return 1;
    }
    if (!len || !msg) {
        msg = NULL;
        len = 0;
    }
    socket->callbackLevel++;
    *handled = socket->unrecognizedCB(socket, ip, port, msg, len);
    socket->callbackLevel--;
    if (socket->close && !socket->callbackLevel) {
        gti2CloseSocket(socket);
        return 0;
    }
    return 1;
}

int gti2NewOutgoingConnection(GTSocket *socket, GTConn **conn, u32 ip, u16 port) {
    int result = gti2NewSocketConnection(socket, conn, ip, port);
    if (result) {
        return result;
    }
    (*conn)->state = 0;
    (*conn)->initiated = 1;
    return 0;
}

int gti2NewIncomingConnection(GTSocket *socket, GTConn **conn, u32 ip, u16 port) {
    int result = gti2NewSocketConnection(socket, conn, ip, port);
    if (result) {
        return result;
    }
    (*conn)->state = 2;
    (*conn)->initiated = 0;
    return 0;
}

int gti2StartConnectionAttempt(GTConn *conn, u8 *msg, int len, GTCallbacks *callbacks) {
    u8 challenge[32];
    gti2MessageCheck(&msg, &len);
    if (len > 0) {
        conn->initialMessage = ov00_021EC3A8(len);
        if (!conn->initialMessage) {
            return 1;
        }
        memcpy(conn->initialMessage, msg, len);
        conn->initialMessageLen = len;
    }
    if (callbacks) {
        conn->callbacks = *callbacks;
    }
    gti2GetChallenge(challenge);
    gti2GetResponse(conn->response, challenge);
    gti2SendClientChallenge(conn, challenge);
    conn->state = 0;
    return 0;
}

int gti2AcceptConnection(GTConn *conn, GTCallbacks *callbacks) {
    if (conn->freeAtAcceptReject) {
        conn->freeAtAcceptReject = 0;
        return 0;
    }
    conn->freeAtAcceptReject = 0;
    if (conn->state != 4) {
        return 0;
    }
    gti2SendAccept(conn);
    conn->state = 5;
    if (callbacks) {
        conn->callbacks = *callbacks;
    }
    return 1;
}

void gti2RejectConnection(GTConn *conn, u8 *msg, int len) {
    conn->freeAtAcceptReject = 0;
    if (conn->state != 4) {
        return;
    }
    gti2MessageCheck(&msg, &len);
    gti2SendReject(conn, msg, len);
    conn->state = 6;
}

int gti2ConnectionSendData(GTConn *conn, u8 *msg, int len) {
    if (!gti2SocketSend(conn->socket, conn->ip, conn->port, msg, len)) {
        return 0;
    }
    conn->lastSend = ov00_021FF368();
    return 1;
}

int gti2CheckConnectionTimeout(GTConn *conn, u32 now) {
    int timedOut;
    if (conn->state < 5) {
        timedOut = 0;
        if (conn->initiated) {
            if (conn->timeout) {
                if (now - conn->startTime > conn->timeout) {
                    timedOut = 1;
                }
            }
        } else if (conn->state < 4) {
            if (now - conn->startTime > 60000) {
                timedOut = 1;
            }
        }
        if (timedOut) {
            gti2SendClosed(conn);
            gti2ConnectionClosed(conn);
            if (!gti2ConnectedCallback(conn, 6, NULL, 0)) {
                return 0;
            }
        }
    }
    return 1;
}

int gti2ResendTimedOutMessages(GTConn *conn, u32 now) {
    int i;
    int n;
    GTOutMsg *msg;
    n = ArrayLength(conn->outgoingBufferedMessages);
    for (i = 0; i < n; i++) {
        msg = ov00_021FE6EC(conn->outgoingBufferedMessages, i);
        if (now - msg->lastSend > 1000) {
            if (!gti2ResendMessage(conn, msg)) {
                return 0;
            }
        }
    }
    return 1;
}

int gti2SendPendingAck(GTConn *conn, u32 now) {
    if (!conn->pendingAck) {
        return 1;
    }
    if (now - conn->pendingAckTime > 100) {
        if (!gti2SendAck(conn)) {
            return 0;
        }
    }
    return 1;
}

int gti2SendKeepAliveIfIdle(GTConn *conn, u32 now) {
    if (now - conn->lastSend > 30000) {
        if (!gti2SendKeepAlive(conn)) {
            return 0;
        }
    }
    return 1;
}

int gti2ConnectionThink(GTConn *conn, u32 now) {
    if (!gti2CheckConnectionTimeout(conn, now)) {
        return 0;
    }
    if (!gti2SendKeepAliveIfIdle(conn, now)) {
        return 0;
    }
    if (!gti2ResendTimedOutMessages(conn, now)) {
        return 0;
    }
    if (!gti2SendPendingAck(conn, now)) {
        return 0;
    }
    return 1;
}

void gti2CloseConnection(GTConn *conn, int hard) {
    if (hard) {
        if (conn->state >= 7) {
            return;
        }
        gti2ConnectionClosed(conn);
        gti2SendClosed(conn);
        gti2ClosedCallback(conn, 0);
        gti2FreeSocketConnection(conn);
    } else {
        conn->state = 6;
        gti2SendClose(conn);
    }
}

void gti2ConnectionClosed(GTConn *conn) {
    if (conn->state == 7) {
        return;
    }
    conn->state = 7;
    ov00_021FF01C(conn->socket->connections, &conn);
    ov00_021FE754(conn->socket->closedConnections, &conn);
}

void gti2ConnectionCleanup(GTConn *conn) {
    if (conn->initialMessage) {
        ov00_021EC3D8(conn->initialMessage);
    }
    if (conn->incomingBuffer.buffer) {
        ov00_021EC3D8(conn->incomingBuffer.buffer);
    }
    if (conn->outgoingBuffer.buffer) {
        ov00_021EC3D8(conn->outgoingBuffer.buffer);
    }
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

int gt2CreateSocket(void *a, void *b, void *c, void *d, void *e) {
    return gti2CreateSocket(a, b, c, d, e);
}

void gt2CloseSocket(GTSocket *socket) {
    gt2CloseAllConnectionsHard(socket);
    gti2CloseSocket(socket);
}

void gt2Think(GTSocket *socket) {
    if (!gti2ReceiveMessages(socket)) {
        return;
    }
    if (!gti2SocketConnectionsThink(socket)) {
        return;
    }
    gti2FreeClosedConnections(socket);
}

int gt2Listen(void *a, void *b, void *c, void *d) {
    return gti2Listen(a, b, c, d);
}

int gt2Accept(GTConn *conn, GTCallbacks *callbacks) {
    return gti2AcceptConnection(conn, callbacks);
}

void gt2Reject(GTConn *conn, u8 *msg, int len) {
    gti2RejectConnection(conn, msg, len);
}

int gt2Connect(GTSocket *socket, GTConn **connp, const char *address, u8 *msg, int len, u32 timeout, GTCallbacks *callbacks, int blocking) {
    GTConn *conn;
    u32 ip;
    u16 port;
    int result;
    int done;

    if (!gt2StringToAddress(address, &ip, &port) || !ip || !port) {
        return 4;
    }
    if (((((ip >> 24) & 0xFF) | ((ip >> 8) & 0xFF00) | ((ip << 8) & 0xFF0000) | ((ip << 24) & 0xFF000000)) & 0xE0000000) == 0xE0000000) {
        return 4;
    }
    result = gti2NewOutgoingConnection(socket, &conn, ip, port);
    if (result) {
        return result;
    }
    conn->timeout = timeout;
    result = gti2StartConnectionAttempt(conn, msg, len, callbacks);
    if (result) {
        gti2FreeSocketConnection(conn);
        return result;
    }
    if (!blocking) {
        if (connp) {
            *connp = conn;
        }
        return 0;
    }
    conn->callbackLevel++;
    do {
        gt2Think(socket);
        done = conn->state >= 5;
        if (!done) {
            ov00_021FF3C0(1);
        }
    } while (!done);
    conn->callbackLevel--;
    if (conn->state == 5) {
        *connp = conn;
    }
    return conn->connectionResult;
}

void gt2Send(GTConn *conn, u8 *msg, int len, int reliable) {
    if (conn->state != 5) {
        return;
    }
    gti2MessageCheck(&msg, &len);
    if (ArrayLength(conn->sendFilters)) {
        gti2SendFilterCallback(conn, 0, msg, len, reliable);
        return;
    }
    gti2Send(conn, msg, len, reliable);
}

void gt2CloseConnectionHard(GTConn *conn) {
    gti2CloseConnection(conn, 1);
}

void gti2CloseConnectionHardMap(GTConn **conn) {
    gt2CloseConnectionHard(*conn);
}

void gt2CloseAllConnectionsHard(GTSocket *socket) {
    if (ov00_0221B630 == 1) {
        return;
    }
    ov00_0221B630 = 1;
    ov00_021FF1F8(socket->connections, gti2CloseConnectionHardMap, NULL);
    ov00_0221B630 = 0;
}

u16 gt2GetLocalPort(GTSocket *socket) {
    return socket->port;
}

int gt2GetOutgoingBufferFreeSpace(GTConn *conn) {
    return conn->outgoingBuffer.size - conn->outgoingBuffer.len;
}

u32 gt2GetSocketSOCKET(GTConn *conn) {
    return conn->ip;
}

void gt2SetUnrecognizedMessageCallback(GTSocket *socket, GTUnrecCB cb) {
    socket->unrecognizedCB = cb;
}

void gt2SetConnectionData(GTConn *conn, void *data) {
    if (!conn) {
        __msl_assertion_failed(ov00_02219C88, ov00_02219C7C, ov00_02219C4C, 0x1a3);
    }
    conn->data = data;
}

void *gt2GetConnectionData(GTConn *conn) {
    if (!conn) {
        __msl_assertion_failed(ov00_02219C88, ov00_02219C7C, ov00_02219C64, 0x1aa);
    }
    return conn->data;
}

u16 gti2ReadUShort(u8 *buf, int off) {
    u16 v = (u16)((buf[off] << 8) & 0xFF00);
    v |= buf[off + 1];
    return v;
}

void gti2WriteUShort(u8 *buf, int off, u16 val) {
    buf[off] = val >> 8;
    buf[off + 1] = val;
}

s16 gti2SerialNumberDiff(u16 a, u16 b) {
    return (s16)(a - b);
}

int gti2ConnectionError(GTConn *conn, int result, GTCloseReason reason) {
    if (conn->state < 5) {
        if (conn->initiated) {
            gti2ConnectionClosed(conn);
            if (!gti2ConnectedCallback(conn, result, NULL, 0)) {
                return 0;
            }
        } else {
            if (conn->state == 4) {
                conn->freeAtAcceptReject = 1;
            }
            gti2ConnectionClosed(conn);
        }
    } else if (conn->state != 7) {
        gti2ConnectionClosed(conn);
        if (!gti2ClosedCallback(conn, reason)) {
            return 0;
        }
    }
    return 1;
}

int gti2ConnectionCommunicationError(GTConn *conn) {
    return gti2ConnectionError(conn, 7, GT_CLOSE_ERROR);
}

int gti2ConnectionOutOfMemoryError(GTConn *conn) {
    if (!gti2SendClosed(conn)) {
        return 0;
    }
    return gti2ConnectionError(conn, 1, GT_CLOSE_FATAL_ERROR);
}

int gti2RemoveAckedMessages(GTConn *conn, u16 ack) {
    int n;
    int i;
    int first;
    GTOutMsg *msg;

    n = ArrayLength(conn->outgoingBufferedMessages);
    if (!n) {
        return 1;
    }
    for (i = 0; i < n; i++) {
        msg = ov00_021FE6EC(conn->outgoingBufferedMessages, i);
        if (gti2SerialNumberDiff(msg->serial, ack) >= 0) {
            break;
        }
    }
    if (i == 0) {
        return 1;
    }
    while (i--) {
        ov00_021FE968(conn->outgoingBufferedMessages, i);
    }
    n = ArrayLength(conn->outgoingBufferedMessages);
    if (!n) {
        conn->outgoingBuffer.len = 0;
        return 1;
    }
    msg = ov00_021FE6EC(conn->outgoingBufferedMessages, 0);
    first = msg->start;
    for (i = 0; i < n; i++) {
        msg = ov00_021FE6EC(conn->outgoingBufferedMessages, i);
        msg->start -= first;
    }
    gti2BufferShorten(&conn->outgoingBuffer, 0, first);
    return 1;
}

int gti2HandleAppUnreliable(GTConn *conn, u8 *msg, int len) {
    if (conn->state != 5 && conn->state != 6) {
        return 1;
    }
    if (ArrayLength(conn->receiveFilters)) {
        if (!gti2ReceiveFilterCallback(conn, 0, msg, len, 0)) {
            return 0;
        }
        return 1;
    }
    if (!gti2ReceivedCallback(conn, msg, len, 0)) {
        return 0;
    }
    return 1;
}

int gti2HandleAppReliable(GTConn *conn, u8 *msg, int len) {
    if (conn->state != 5 && conn->state != 6) {
        if (!gti2ConnectionCommunicationError(conn)) {
            return 0;
        }
    } else {
        if (ArrayLength(conn->receiveFilters)) {
            if (!gti2ReceiveFilterCallback(conn, 0, msg, len, 1)) {
                return 0;
            }
            return 1;
        }
        if (!gti2ReceivedCallback(conn, msg, len, 1)) {
            return 0;
        }
    }
    return 1;
}

int gti2HandleClientChallenge(GTConn *conn, u8 *msg, int len) {
    u8 response[32];
    u8 challenge[32];
    if (conn->state != 2) {
        if (!gti2ConnectionCommunicationError(conn)) {
            return 0;
        }
        return 1;
    }
    if (len < 0x20) {
        if (!gti2ConnectionCommunicationError(conn)) {
            return 0;
        }
        return 1;
    }
    gti2GetResponse(response, msg);
    gti2GetChallenge(challenge);
    gti2GetResponse(conn->response, challenge);
    if (!gti2SendServerChallenge(conn, response, challenge)) {
        return 0;
    }
    conn->state = 3;
    return 1;
}

int gti2HandleServerChallenge(GTConn *conn, u8 *msg, int len) {
    u8 response[32];
    if (conn->state != 0) {
        if (!gti2ConnectionCommunicationError(conn)) {
            return 0;
        }
        return 1;
    }
    if (len < 0x40) {
        if (!gti2ConnectionCommunicationError(conn)) {
            return 0;
        }
        return 1;
    }
    if (!gti2CheckResponse(msg, conn->response)) {
        if (!gti2ConnectionCommunicationError(conn)) {
            return 0;
        }
        return 1;
    }
    gti2GetResponse(response, msg + 0x20);
    if (!gti2SendClientResponse(conn, response, conn->initialMessage, conn->initialMessageLen)) {
        return 0;
    }
    if (conn->initialMessage) {
        ov00_021EC3D8(conn->initialMessage);
        conn->initialMessage = NULL;
    }
    conn->state = 1;
    return 1;
}

int gti2HandleClientResponse(GTConn *conn, u8 *msg, int len) {
    if (conn->state != 3) {
        if (!gti2ConnectionCommunicationError(conn)) {
            return 0;
        }
        return 1;
    }
    if (len < 0x20) {
        if (!gti2ConnectionCommunicationError(conn)) {
            return 0;
        }
        return 1;
    }
    if (!gti2CheckResponse(msg, conn->response)) {
        if (!gti2ConnectionCommunicationError(conn)) {
            return 0;
        }
        return 1;
    }
    if (!conn->socket->connectAttemptCB) {
        if (!gti2SendClosed(conn)) {
            return 0;
        }
        gti2ConnectionClosed(conn);
        return 1;
    }
    conn->state = 4;
    if (!gti2ConnectAttemptCallback(conn->socket, conn, conn->ip, conn->port, ov00_021FF368() - conn->challengeTime, msg + 0x20, len - 0x20)) {
        return 0;
    }
    return 1;
}

int gti2HandleAccept(GTConn *conn) {
    if (conn->state != 1) {
        if (!gti2ConnectionCommunicationError(conn)) {
            return 0;
        }
        return 1;
    }
    conn->state = 5;
    if (!gti2ConnectedCallback(conn, 0, NULL, 0)) {
        return 0;
    }
    return 1;
}

int gti2HandleReject(GTConn *conn, u8 *msg, int len) {
    if (conn->state != 1) {
        if (!gti2ConnectionCommunicationError(conn)) {
            return 0;
        }
        return 1;
    }
    gti2ConnectionClosed(conn);
    if (!gti2SendClosed(conn)) {
        return 0;
    }
    if (!gti2ConnectedCallback(conn, 2, msg, len)) {
        return 0;
    }
    return 1;
}

int gti2HandleClose(GTConn *conn) {
    int r;
    if (gti2SendClosed(conn) == 0) {
        return 0;
    }
    r = gti2ConnectionError(conn, 2, conn->state == 6 ? GT_CLOSE_LOCAL : GT_CLOSE_REMOTE);
    if (r == 0) {
        return 0;
    }
    return 1;
}

int gti2DeliverReliableMessage(GTConn *conn, int type, u8 *msg, int len) {
    conn->expectedSerialNumber++;
    if (type == 0) {
        if (!gti2HandleAppReliable(conn, msg, len)) {
            return 0;
        }
    } else if (type == 1) {
        if (!gti2HandleClientChallenge(conn, msg, len)) {
            return 0;
        }
    } else if (type == 2) {
        if (!gti2HandleServerChallenge(conn, msg, len)) {
            return 0;
        }
    } else if (type == 3) {
        if (!gti2HandleClientResponse(conn, msg, len)) {
            return 0;
        }
    } else if (type == 4) {
        if (!gti2HandleAccept(conn)) {
            return 0;
        }
    } else if (type == 5) {
        if (!gti2HandleReject(conn, msg, len)) {
            return 0;
        }
    } else if (type == 6) {
        if (!gti2HandleClose(conn)) {
            return 0;
        }
    }
    return 1;
}

s16 gti2IncomingMessageCompare(GTInMsg *a, GTInMsg *b) {
    return gti2SerialNumberDiff(a->serial, b->serial);
}

int gti2BufferIncomingMessage(GTConn *conn, int type, u16 serial, u8 *msg, int len, int *overflow) {
    int n;
    int i;
    GTInMsg *m;
    GTInMsg newMsg;

    n = ArrayLength(conn->incomingBufferedMessages);
    for (i = 0; i < n; i++) {
        m = ov00_021FE6EC(conn->incomingBufferedMessages, i);
        if (m->serial == serial) {
            *overflow = 0;
            return 1;
        }
        if (gti2SerialNumberDiff(m->serial, serial) > 0) {
            break;
        }
    }
    if (gti2GetBufferFreeSpace(&conn->incomingBuffer) < len) {
        *overflow = 1;
        return 1;
    }
    newMsg.start = conn->incomingBuffer.len;
    newMsg.len = len;
    newMsg.type = type;
    newMsg.serial = serial;
    ov00_021FE858(conn->incomingBufferedMessages, &newMsg, gti2IncomingMessageCompare);
    if (ArrayLength(conn->incomingBufferedMessages) != n + 1) {
        *overflow = 1;
        return 1;
    }
    gti2BufferWriteData(&conn->incomingBuffer, msg, len);
    if (n == 0) {
        if (!gti2SendNack(conn, conn->expectedSerialNumber, serial - 1)) {
            return 0;
        }
    } else {
        m = ov00_021FE6EC(conn->incomingBufferedMessages, n);
        if (m->serial == serial) {
            m = ov00_021FE6EC(conn->incomingBufferedMessages, n - 1);
            if ((u16)gti2SerialNumberDiff(serial, m->serial) > 1) {
                if (!gti2SendNack(conn, m->serial + 1, serial - 1)) {
                    return 0;
                }
            }
        }
    }
    *overflow = 0;
    return 1;
}

void gti2RemoveIncomingMessage(GTConn *conn, GTInMsg *msg, int index) {
    int start = msg->start;
    int len = msg->len;
    int n;
    int i;
    int max = 0;
    int end;
    GTInMsg *m;

    ov00_021FE968(conn->incomingBufferedMessages, index);
    n = ArrayLength(conn->incomingBufferedMessages);
    for (i = 0; i < n; i++) {
        m = ov00_021FE6EC(conn->incomingBufferedMessages, i);
        if (m->start > start) {
            m->start -= len;
            end = m->start + m->len;
            if (max <= end) {
                max = end;
            }
        }
    }
    gti2BufferShorten(&conn->incomingBuffer, start, len);
}

int gti2DeliverBufferedMessages(GTConn *conn) {
    int i;
    GTInMsg *m;
again:
    for (i = ArrayLength(conn->incomingBufferedMessages) - 1; i >= 0; i--) {
        m = ov00_021FE6EC(conn->incomingBufferedMessages, i);
        if (m->serial == conn->expectedSerialNumber) {
            if (!gti2DeliverReliableMessage(conn, m->type, conn->incomingBuffer.buffer + m->start, m->len)) {
                return 0;
            }
            gti2RemoveIncomingMessage(conn, m, i);
            goto again;
        }
    }
    return 1;
}

void gti2QueueAck(GTConn *conn) {
    if (conn->pendingAck) {
        return;
    }
    conn->pendingAck = 1;
    conn->pendingAckTime = ov00_021FF368();
}

int gti2HandleReliableMessage(GTConn *conn, int type, u8 *msg, int len) {
    u16 serial;
    int overflow;
    if (len < 7) {
        if (!gti2ConnectionCommunicationError(conn)) {
            return 0;
        }
        return 1;
    }
    serial = gti2ReadUShort(msg, 3);
    if (!gti2RemoveAckedMessages(conn, gti2ReadUShort(msg, 5))) {
        return 0;
    }
    if (serial == conn->expectedSerialNumber) {
        gti2QueueAck(conn);
        if (!gti2DeliverReliableMessage(conn, type, msg + 7, len - 7)) {
            return 0;
        }
        if (!gti2DeliverBufferedMessages(conn)) {
            return 0;
        }
        return 1;
    }
    if (gti2SerialNumberDiff(serial, conn->expectedSerialNumber) < 0) {
        gti2QueueAck(conn);
        return 1;
    }
    if (!gti2BufferIncomingMessage(conn, type, serial, msg + 7, len - 7, &overflow)) {
        return 0;
    }
    if (overflow) {
        if (!gti2ConnectionOutOfMemoryError(conn)) {
            return 0;
        }
    }
    return 1;
}

int gti2HandleAck(GTConn *conn, u8 *msg, int len) {
    if (len != 2) {
        if (!gti2ConnectionCommunicationError(conn)) {
            return 0;
        }
        return 1;
    }
    if (!gti2RemoveAckedMessages(conn, gti2ReadUShort(msg, 0))) {
        return 0;
    }
    return 1;
}

int gti2HandleNack(GTConn *conn, u8 *msg, int len) {
    u16 start;
    u16 end;
    int n;
    int i;
    GTOutMsg *m;

    start = gti2ReadUShort(msg, 0);
    if (len == 2) {
        end = start;
    } else if (len == 4) {
        end = gti2ReadUShort(msg, 2);
    } else {
        if (!gti2ConnectionCommunicationError(conn)) {
            return 0;
        }
        return 1;
    }
    n = ArrayLength(conn->outgoingBufferedMessages);
    for (i = 0; i < n; i++) {
        m = ov00_021FE6EC(conn->outgoingBufferedMessages, i);
        if (gti2SerialNumberDiff(m->serial, start) >= 0 && gti2SerialNumberDiff(m->serial, end) <= 0) {
            if (!gti2ResendMessage(conn, m)) {
                return 0;
            }
        }
    }
    return 1;
}

int gti2HandlePing(GTConn *conn, u8 *msg, int len) {
    return gti2SendPong(conn, msg, len);
}

int gti2HandlePong(GTConn *conn, u8 *msg, int len) {
    u32 time;
    if (!conn->callbacks.ping) {
        return 1;
    }
    if (len != 8) {
        return 1;
    }
    if (memcmp(msg, ov00_02219CAC, 4)) {
        return 1;
    }
    memcpy(&time, msg + 4, 4);
    if (!gti2PingCallback(conn, ov00_021FF368() - time)) {
        return 0;
    }
    return 1;
}

int gti2HandleClosed(GTConn *conn) {
    int r;
    if (conn->state == 7) {
        return 1;
    }
    r = gti2ConnectionError(conn, 2, conn->state == 6 ? GT_CLOSE_LOCAL : GT_CLOSE_REMOTE);
    if (r == 0) {
        return 0;
    }
    return 1;
}

int gti2HandleUnreliableMessage(GTConn *conn, int type, u8 *msg, int len) {
    int dataLen = len - 3;
    if (type == 0x64) {
        if (!gti2HandleAck(conn, msg + 3, dataLen)) {
            return 0;
        }
    } else if (type == 0x65) {
        if (!gti2HandleNack(conn, msg + 3, dataLen)) {
            return 0;
        }
    } else if (type == 0x66) {
        if (!gti2HandlePing(conn, msg, len)) {
            return 0;
        }
    } else if (type == 0x67) {
        if (!gti2HandlePong(conn, msg + 3, dataLen)) {
            return 0;
        }
    } else if (type == 0x68) {
        if (!gti2HandleClosed(conn)) {
            return 0;
        }
    }
    return 1;
}

int gti2HandleMessage(GTSocket *socket, u8 *msg, int len, u32 ip, u16 port) {
    GTConn *conn;
    int handled;
    int isGT2;
    int result;
    int type;

    conn = gti2SocketFindConnection(socket, ip, port);
    if (socket->unk2C) {
        if (!gti2DumpCallback(socket, conn, ip, port, 0, msg, len, 0)) {
            return 0;
        }
    }
    if (len > 2 && !memcmp(msg, ov00_02219CB4, 2)) {
        isGT2 = 1;
    } else {
        isGT2 = 0;
    }
    if (!conn) {
        if (!gti2UnrecognizedMessageCallback(socket, ip, port, msg, len, &handled)) {
            return 0;
        }
        if (handled) {
            return 1;
        }
        if (!isGT2 || msg[2] != 1) {
            if (!isGT2 || msg[2] != 0x68) {
                if (!gti2SendClosedToAddress(socket, ip, port)) {
                    return 0;
                }
            }
            return 1;
        }
        if (!socket->connectAttemptCB) {
            return 1;
        }
        result = gti2NewIncomingConnection(socket, &conn, ip, port);
        if (result) {
            if (result != 5) {
                if (!gti2SendClosedToAddress(socket, ip, port)) {
                    return 0;
                }
            }
            return 1;
        }
    }
    if (conn->state == 7) {
        if (!isGT2 || msg[2] != 0x68) {
            if (!gti2SendClosed(conn)) {
                return 0;
            }
        }
        return 1;
    }
    if (isGT2 && len >= 4 && !memcmp(msg + 2, ov00_02219CB4, 2)) {
        msg += 2;
        len -= 2;
        isGT2 = 0;
    }
    if (!isGT2) {
        if (!gti2HandleAppUnreliable(conn, msg, len)) {
            return 0;
        }
        return 1;
    }
    type = msg[2];
    if (type < 0) {
        if (!gti2ConnectionCommunicationError(conn)) {
            return 0;
        }
        return 1;
    }
    if (type < 8) {
        if (!gti2HandleReliableMessage(conn, type, msg, len)) {
            return 0;
        }
        return 1;
    }
    if (!gti2HandleUnreliableMessage(conn, type, msg, len)) {
        return 0;
    }
    return 1;
}

int gti2HandleConnectionReset(GTSocket *socket, u32 ip, u16 port) {
    GTConn *conn = gti2SocketFindConnection(socket, ip, port);
    if (socket->unk2C) {
        if (!gti2DumpCallback(socket, conn, ip, port, 1, NULL, 0, 0)) {
            return 0;
        }
    }
    if (!conn) {
        return 1;
    }
    if (conn->state == 0) {
        if (!conn->timeout || ov00_021FF368() - conn->startTime < conn->timeout) {
            return 1;
        }
        if (!gti2ConnectionError(conn, 6, GT_CLOSE_REMOTE)) {
            return 0;
        }
    } else {
        if (!gti2ConnectionError(conn, 2, GT_CLOSE_REMOTE)) {
            return 0;
        }
    }
    return 1;
}

int gti2ReceiveMessages(GTSocket *socket) {
    GTSockAddr addr;
    int addrlen;
    u8 buf[1500];
    int rc;
    int err;

    while (ov00_021FF694(socket->handle)) {
        addrlen = 8;
        rc = ov00_021FF940(socket->handle, buf, 1500, 0, &addr, &addrlen);
        if (rc == -1) {
            err = gsSocketGetLastError(socket->handle);
            if (err == -15) {
                if (!gti2HandleConnectionReset(socket, addr.addr, (u16)(((addr.port >> 8) & 0xFF) | ((addr.port << 8) & 0xFF00)))) {
                    return 0;
                }
            } else if (err != -35) {
                gti2SocketError(socket);
                return 0;
            }
        } else {
            if (!gti2HandleMessage(socket, buf, rc, addr.addr, (u16)(((addr.port >> 8) & 0xFF) | ((addr.port << 8) & 0xFF00)))) {
                return 0;
            }
        }
    }
    return 1;
}
