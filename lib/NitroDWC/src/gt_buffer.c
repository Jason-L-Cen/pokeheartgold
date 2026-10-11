#pragma force_active on
#include <string.h>

#include "global.h"

#include "dwc/gt_internal.h"

// The code below is compiled as ARM (32-bit) code.
#include <nitro/code32.h>

// Reliable UDP sockets: the connection challenge / response, the growable message buffers, and the
// socket error and connect attempt callbacks into the user code.

extern const char ov00_022199D0[]; // "gti2GetChallenge"
extern const char ov00_022199E4[]; // "3b8dd8995f7c40a9a5c5b7dd5b481341"
extern const char ov00_02219A08[]; // "buffer"
extern const char ov00_02219A10[]; // "gt2Auth.c"
extern const char ov00_02219A1C[]; // "gti2BufferShorten"
extern const char ov00_02219A30[]; // "gti2BufferWriteData"
extern const char ov00_02219A44[]; // "gti2BufferWriteByte"
extern const char ov00_02219A58[]; // "gti2BufferWriteUShort"
extern const char ov00_02219A70[]; // "buffer->len < buffer->size"
extern const char ov00_02219A8C[]; // "gt2Buffer.c"
extern const char ov00_02219A98[]; // "(buffer->len + 2) <= buffer->size"
extern const char ov00_02219ABC[]; // "(buffer->len + len) <= buffer->size"
extern const char ov00_02219AE0[]; // "start <= buffer->len"
extern const char ov00_02219AF8[]; // "shortenBy <= (buffer->len - start)"
extern const char ov00_02219BA0[]; // "gti2SocketErrorCallback"
extern const char ov00_02219BD4[]; // "gti2ConnectAttemptCallback"
extern const char ov00_02219C10[]; // "socket"
extern const char ov00_02219C18[]; // "gt2Callback.c"
extern const char ov00_02219C28[]; // "socket && connection"

extern void __msl_assertion_failed(const char *expr, const char *file, const char *func, int line);
extern int abs(int x);
extern int rand(void);
extern void srand(unsigned int seed);
extern void *ov00_021EC3A8(u32 size);
extern u32 ov00_021FF368(void);
extern void ov00_0220F144(GTSocket *socket);

int ov00_0220C200(GTSocket *socket, GTConn *connection, int ip, int port, int latency, int msg, int len);

BOOL ov00_0220BC50(const u8 *buf) {
    int parity;
    int i;
    parity = 0;
    i = 1;
    for (; i < 32; i++) {
        parity = ((buf[0] & 1) ^ (parity ^ ((i ^ buf[i - 1]) & 1))) ^ (buf[0] < 0x4f ? 1 : 0) ^ (buf[i - 1] < buf[0] ? 1 : 0);
        if ((parity != 0 && (buf[i] & 1) == 0) || (parity == 0 && (buf[i] & 1) == 1)) {
            return FALSE;
        }
    }
    return TRUE;
}

u8 *ov00_0220BCEC(u8 *buf) {
    int i;
    int parity;
    if (buf == NULL) {
        __msl_assertion_failed(ov00_02219A08, ov00_02219A10, ov00_022199D0, 0x2b);
    }
    srand(ov00_021FF368());
    buf[0] = rand() % 93 + 33;
    parity = 0;
    i = 1;
    for (; i < 32; i++) {
        parity = ((buf[0] & 1) ^ (parity ^ ((i ^ buf[i - 1]) & 1))) ^ (buf[0] < 0x4f ? 1 : 0) ^ (buf[i - 1] < buf[0] ? 1 : 0);
        buf[i] = rand() % 93 + 33;
        if ((parity != 0 && (buf[i] & 1) == 0) || (parity == 0 && (buf[i] & 1) == 1)) {
            buf[i]++;
        }
    }
    return buf;
}

char *ov00_0220BE08(char *out, const char *key) {
    int i;
    int keyLen[1];
    BOOL valid;
    keyLen[0] = strlen(ov00_022199E4);
    valid = ov00_0220BC50((const u8 *)key);
    for (i = 0; i < 32; i++) {
        if (!valid || i == 0 || i == 13) {
            out[i] = rand() % 93 + 33;
        } else {
            int prev = ((i == 1 || i == 14) ? key[i] : key[i - 1]);
            out[i] = abs(((u8 *)key)[(i * (u8)key[i] + ov00_022199E4[(i + (u8)key[i]) % keyLen[0]]) % 32] ^ ov00_022199E4[(prev * (i * 0x4647)) % keyLen[0]]) % 93 + 33;
        }
    }
    return out;
}

int ov00_0220BF10(u8 *a, u8 *b) {
    int i;
    for (i = 0; i < 32; i++) {
        if (i != 0 && i != 13 && a[i] != b[i]) {
            return 0;
        }
    }
    return 1;
}

int ov00_0220BF44(GTBuffer *buffer, int size) {
    buffer->buffer = ov00_021EC3A8(size);
    if (!buffer->buffer) {
        return 0;
    }
    buffer->size = size;
    return 1;
}

int ov00_0220BF70(GTBuffer *buffer) {
    return buffer->size - buffer->len;
}

void ov00_0220BF80(GTBuffer *buffer, u8 c) {
    if (!(buffer->len < buffer->size)) {
        __msl_assertion_failed(ov00_02219A70, ov00_02219A8C, ov00_02219A44, 0x23);
    }
    buffer->buffer[buffer->len++] = c;
}

void ov00_0220BFD0(GTBuffer *buffer, int v) {
    if (!(buffer->len + 2 <= buffer->size)) {
        __msl_assertion_failed(ov00_02219A98, ov00_02219A8C, ov00_02219A58, 0x2e);
    }
    buffer->buffer[buffer->len++] = v >> 8;
    buffer->buffer[buffer->len++] = v;
}

void ov00_0220C03C(GTBuffer *buffer, const void *src, int n) {
    if (!src || !n) {
        return;
    }
    if (n == -1) {
        n = strlen(src);
    }
    if (!(buffer->len + n <= buffer->size)) {
        __msl_assertion_failed(ov00_02219ABC, ov00_02219A8C, ov00_02219A30, 0x40);
    }
    memcpy(buffer->buffer + buffer->len, src, n);
    buffer->len += n;
}

void ov00_0220C0C4(GTBuffer *buffer, int pos, int n) {
    if (pos == -1) {
        pos = buffer->len - n;
    }
    if (!(pos <= buffer->len)) {
        __msl_assertion_failed(ov00_02219AE0, ov00_02219A8C, ov00_02219A1C, 0x4f);
    }
    if (!(n <= buffer->len - pos)) {
        __msl_assertion_failed(ov00_02219AF8, ov00_02219A8C, ov00_02219A1C, 0x50);
    }
    memmove(buffer->buffer + pos, buffer->buffer + pos + n, buffer->len - pos - n);
    buffer->len -= n;
}

int ov00_0220C164(GTSocket *socket) {
    if (!socket) {
        __msl_assertion_failed(ov00_02219C10, ov00_02219C18, ov00_02219BA0, 0x1b);
    }
    if (!socket) {
        return 1;
    }
    if (!socket->socketErrorCB) {
        return 1;
    }
    socket->callbackLevel++;
    socket->socketErrorCB(socket);
    socket->callbackLevel--;
    if (socket->close && socket->callbackLevel == 0) {
        ov00_0220F144(socket);
        return 0;
    }
    return 1;
}

int ov00_0220C200(GTSocket *socket, GTConn *connection, int ip, int port, int latency, int msg, int len) {
    if (!(socket && connection)) {
        __msl_assertion_failed(ov00_02219C28, ov00_02219C18, ov00_02219BD4, 0x3d);
    }
    if (!socket || !connection) {
        return 1;
    }
    if (!socket->connectAttemptCB) {
        return 1;
    }
    if (!len || !msg) {
        msg = 0;
        len = 0;
    }
    socket->callbackLevel++;
    connection->callbackLevel++;
    socket->connectAttemptCB(socket, connection, ip, port, latency, msg, len);
    socket->callbackLevel--;
    connection->callbackLevel--;
    if (socket->close && socket->callbackLevel == 0) {
        ov00_0220F144(socket);
        return 0;
    }
    return 1;
}
