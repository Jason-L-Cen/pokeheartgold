#pragma force_active on
#include "global.h"

#include "dwc/http_internal.h"

// The code below is compiled as ARM (32-bit) code.
#include <nitro/code32.h>

// Multipart body overhead per field, computed once from the boundary string.
typedef struct GHIMultipartLengths {
    int footer;
    int stringField;
    int boundary;
    int fileField;
} GHIMultipartLengths;

typedef struct GHIThrottleSettings {
    int delay; // ms between two receives
    int bytesPerRecv;
} GHIThrottleSettings;

// "%00": url-encoding of one character
typedef struct GHIPercentEscape {
    char chars[4];
} GHIPercentEscape;

extern GHIConnList ov38_0222602C;
extern int ov38_0222603C;
extern char *ov38_02226028; // proxy server address
extern GHIThrottleSettings ov38_0222531C;
extern int ov38_02225320; // socket receive buffer size while throttled
extern GHIMultipartLengths ov38_02226040;
extern GHIPercentEscape _02224E60;
extern char _02225138[];
extern char ov38_02225148[];
extern char ov38_02225158[];
extern char ov38_02225168[];
extern char ov38_02225178[];
extern char ov38_0222518C[];
extern char ov38_022251A4[];
extern char ov38_022251BC[];
extern char ov38_022251D4[];
extern char ov38_022251DC[];
extern char ov38_022251EC[];
extern char ov38_02225200[];
extern char ov38_0222520C[];
extern char ov38_0222521C[];
extern char ov38_02225228[];
extern char ov38_02225234[];
extern char ov38_0222523C[];
extern char ov38_0222524C[];
extern char ov38_02225288[];
extern char ov38_0222528C[];
extern char ov38_02225290[];
extern char ov38_02225294[];
extern char ov38_022252A8[];
extern char ov38_022252B4[];
extern char ov38_022252C8[];
extern char ov38_022252E0[];
extern char ov38_022252FC[];
extern char ov38_02225308[];
extern char ov38_02225324[];
extern char ov38_02225334[];
extern char ov38_02225348[];
extern char ov38_02225360[];
extern char ov38_02225378[];
extern char ov38_022253A0[];
extern char ov38_022253B4[];
extern char ov38_022253C0[];
extern char ov38_022253DC[];
extern char ov38_02225404[];
extern char ov38_02225418[];
extern char ov38_02225428[];
extern char ov38_02225444[];
extern char ov38_0222545C[];
extern char ov38_02225468[];
extern char ov38_02225480[];
extern char ov38_0222548C[];
extern char ov38_02225498[];
extern char ov38_022254D4[];
extern char ov38_022254E4[];
extern char ov38_022254F4[];
extern char ov38_0222550C[];
extern char ov38_02225520[];
extern char ov38_02225534[];
extern char ov38_02225548[];
extern char ov38_0222555C[];
extern char ov38_02225574[];
extern char ov38_0222558C[];
extern char ov38_022255A4[];
extern char ov38_022255C0[];
extern char ov38_022255E0[];
extern char ov38_02225600[];
extern char ov38_02225620[];
extern char ov38_02225640[];
extern char ov38_02225644[];
extern char ov38_02225650[];
extern char ov38_02225694[];
extern char ov38_0222569C[];
extern char ov38_022256A0[];
extern char ov38_022256E4[];
extern char ov38_02225708[];
extern char ov38_02225720[];
extern char ov38_02225748[];
extern char ov38_02225750[];
extern char ov38_02225764[];
extern char ov38_022257B8[];
extern char ov38_022257C8[];
extern char ov38_022257F4[];
extern char ov38_02225804[];
extern char ov38_02225818[];
extern char ov38_02225840[];
extern char ov38_02225878[];
extern char ov38_022258A8[];
extern char ov38_022258C8[];
extern char ov38_022258CC[];
extern char ov38_022258D4[];
extern char ov38_022258FC[];
extern char ov38_02225928[];
extern char ov38_02225958[];
extern char ov38_022259AC[];
extern char ov38_022259D0[];
extern char ov38_022259DC[];
extern char ov38_022259FC[];
extern char ov38_02225A20[];
extern char ov38_02225A70[];
extern char ov38_02225A7C[];
extern char ov38_02225AA8[];

static inline void ClearPostState(void *p) {
    GHIPostState *state = p;
    state->data = NULL;
    state->pos = 0;
    state->file = NULL;
    state->len = 0;
}

int ghiResizeBuffer(GHIBuffer *buffer, int sizeIncrement) {
    char *tmp;
    int newSize;
    HTTP_ASSERT(buffer, ov38_022251D4, ov38_022251DC, ov38_02225168, 0x20);
    HTTP_ASSERT(sizeIncrement > 0, ov38_022251EC, ov38_022251DC, ov38_02225168, 0x21);
    if (!buffer) {
        return 0;
    }
    if (sizeIncrement <= 0) {
        return 0;
    }
    newSize = buffer->size + sizeIncrement;
    tmp = DWCi_GsRealloc(buffer->data, newSize);
    if (!tmp) {
        return 0;
    }
    buffer->data = tmp;
    buffer->size = newSize;
    return 1;
}

int ghiInitBuffer(GHIConnection *connection, GHIBuffer *buffer, int initialSize, int sizeIncrement) {
    HTTP_ASSERT(connection, ov38_02225200, ov38_022251DC, ov38_02225148, 0x43);
    HTTP_ASSERT(buffer, ov38_022251D4, ov38_022251DC, ov38_02225148, 0x44);
    HTTP_ASSERT(initialSize > 0, ov38_0222520C, ov38_022251DC, ov38_02225148, 0x45);
    HTTP_ASSERT(sizeIncrement > 0, ov38_022251EC, ov38_022251DC, ov38_02225148, 0x46);
    if (!connection) {
        return 0;
    }
    if (!buffer) {
        return 0;
    }
    if (initialSize <= 0) {
        return 0;
    }
    if (sizeIncrement <= 0) {
        return 0;
    }
    buffer->connection = connection;
    buffer->data = NULL;
    buffer->size = 0;
    buffer->len = 0;
    buffer->pos = 0;
    buffer->sizeIncrement = sizeIncrement;
    buffer->fixed = 0;
    buffer->dontFree = 0;
    buffer->encrypted = 0;
    if (!ghiResizeBuffer(buffer, initialSize)) {
        return 0;
    }
    buffer->data[0] = '\0';
    return 1;
}

int ghiInitFixedBuffer(GHIConnection *connection, GHIBuffer *buffer, char *userBuffer, int size) {
    HTTP_ASSERT(connection, ov38_02225200, ov38_022251DC, ov38_02225178, 0x74);
    HTTP_ASSERT(buffer, ov38_022251D4, ov38_022251DC, ov38_02225178, 0x75);
    HTTP_ASSERT(userBuffer, ov38_0222521C, ov38_022251DC, ov38_02225178, 0x76);
    HTTP_ASSERT(size > 0, ov38_02225228, ov38_022251DC, ov38_02225178, 0x77);
    if (!connection) {
        return 0;
    }
    if (!buffer) {
        return 0;
    }
    if (!userBuffer) {
        return 0;
    }
    if (size <= 0) {
        return 0;
    }
    buffer->connection = connection;
    buffer->data = userBuffer;
    buffer->size = size;
    buffer->len = 0;
    buffer->sizeIncrement = 0;
    buffer->fixed = 1;
    buffer->dontFree = 1;
    buffer->encrypted = 0;
    buffer->data[0] = '\0';
    return 1;
}

void ghiFreeBuffer(GHIBuffer *buffer) {
    HTTP_ASSERT(buffer, ov38_022251D4, ov38_022251DC, _02225138, 0x9b);
    if (!buffer || !buffer->data) {
        return;
    }
    if (!buffer->dontFree) {
        DWCi_GsFree(buffer->data);
    }
    memset(buffer, 0, sizeof(GHIBuffer));
}

int ghiAppendDataToBuffer(GHIBuffer *buffer, const char *data, int len) {
    GHIConnection *connection = buffer->connection;
    int newLen;
    HTTP_ASSERT(buffer, ov38_022251D4, ov38_022251DC, ov38_022251A4, 0xb7);
    HTTP_ASSERT(data, ov38_02225234, ov38_022251DC, ov38_022251A4, 0xb8);
    HTTP_ASSERT(len >= 0, ov38_0222523C, ov38_022251DC, ov38_022251A4, 0xb9);
    if (!buffer) {
        return 0;
    }
    if (!data) {
        return 0;
    }
    if (len < 0) {
        return 0;
    }
    if (len == 0) {
        len = strlen(data);
    }
    if (buffer->encrypted == 1) {
        int result;
        HTTP_ASSERT(connection->encryptor.engine, ov38_0222524C, ov38_022251DC, ov38_022251A4, 0xce);
        do {
            int outLen = buffer->size - buffer->len;
            result = connection->encryptor.encryptFunc(connection, &connection->encryptor, data, &len, buffer->data + buffer->len, &outLen);
            if (result == 2) {
                if (buffer->fixed) {
                    buffer->connection->completed = 1;
                    buffer->connection->result = 2;
                    return 0;
                }
                if (ghiResizeBuffer(buffer, buffer->sizeIncrement)) {
                    buffer->connection->completed = 1;
                    buffer->connection->result = 1;
                    return 0;
                }
            } else {
                buffer->len += outLen;
            }
        } while (result == 2);
    } else {
        newLen = buffer->len + len;
        while (newLen >= buffer->size) {
            if (buffer->fixed) {
                buffer->connection->completed = 1;
                buffer->connection->result = 2;
                return 0;
            }
            if (!ghiResizeBuffer(buffer, buffer->sizeIncrement)) {
                buffer->connection->completed = 1;
                buffer->connection->result = 1;
                return 0;
            }
        }
        memcpy(buffer->data + buffer->len, data, len);
        buffer->len = newLen;
        buffer->data[newLen] = '\0';
    }
    return 1;
}

int ghiAppendHeaderToBuffer(GHIBuffer *buffer, const char *name, const char *value) {
    if (!ghiAppendDataToBuffer(buffer, name, 0)) {
        return 0;
    }
    if (!ghiAppendDataToBuffer(buffer, ov38_02225288, 2)) {
        return 0;
    }
    if (!ghiAppendDataToBuffer(buffer, value, 0)) {
        return 0;
    }
    if (!ghiAppendDataToBuffer(buffer, ov38_0222528C, 2)) {
        return 0;
    }
    return 1;
}

int ghiAppendCharToBuffer(GHIBuffer *buffer, int c) {
    char ch = c;
    HTTP_ASSERT(buffer, ov38_022251D4, ov38_022251DC, ov38_0222518C, 0x131);
    if (!buffer) {
        return 0;
    }
    return ghiAppendDataToBuffer(buffer, &ch, 1);
}

int ghiAppendIntToBuffer(GHIBuffer *buffer, int i) {
    char buf[16];
    sprintf(buf, ov38_02225290, i);
    return ghiAppendDataToBuffer(buffer, buf, 0);
}

void ghiResetBuffer(GHIBuffer *buffer) {
    HTTP_ASSERT(buffer, ov38_022251D4, ov38_022251DC, ov38_02225158, 0x16c);
    buffer->len = 0;
    buffer->pos = 0;
    buffer->data[0] = '\0';
}

int ghiSendBufferedData(GHIConnection *c) {
    int writeFlag;
    int exceptFlag;
    int n;
    do {
        if (GSISocketSelect(c->socket, 0, &writeFlag, &exceptFlag) == -1 || exceptFlag) {
            c->completed = 1;
            c->result = 5;
            c->socketError = gsSocketGetLastError(c->socket);
            return 0;
        }
        if (!writeFlag) {
            return 1;
        }
        n = ghiDoSend(c, c->sendBuffer.data + c->sendBuffer.pos, c->sendBuffer.len - c->sendBuffer.pos);
        if (n == -1) {
            return 0;
        }
        c->sendBuffer.pos += n;
    } while (c->sendBuffer.pos < c->sendBuffer.len);
    return 1;
}

int ghiReadDataFromBuffer(GHIBuffer *buffer, char *out, int *len) {
    int n;
    int avail;
    HTTP_ASSERT(buffer, ov38_02225294, ov38_022251DC, ov38_022251BC, 0x1b7);
    HTTP_ASSERT(len, ov38_022252A8, ov38_022251DC, ov38_022251BC, 0x1b8);
    n = *len;
    if (n == 0) {
        return 0;
    }
    avail = buffer->len - buffer->pos;
    if (avail <= 0) {
        return 0;
    }
    if (n >= avail) {
        n = avail;
    }
    memcpy(out, buffer->data + buffer->pos, n);
    out[n] = '\0';
    *len = n;
    buffer->pos += n;
    return 1;
}

void ghiCallCompletedCallback(GHIConnection *c) {
    char *buffer;
    int len;
    int r;
    HTTP_ASSERT(c, ov38_022252FC, ov38_02225308, ov38_022252E0, 0x1b);
    if (!c->completedCallback) {
        return;
    }
    if (c->type) {
        buffer = NULL;
        len = 0;
    } else {
        buffer = c->getFileBuffer.data;
        len = c->fileBytesReceived;
    }
    r = c->completedCallback(c->request, c->result, buffer, len, c->callbackParam);
    if (buffer && !r) {
        c->getFileBuffer.dontFree = 1;
    }
}

void ghiCallProgressCallback(GHIConnection *c, const char *buffer, int len) {
    HTTP_ASSERT(c, ov38_022252FC, ov38_02225308, ov38_022252C8, 0x45);
    if (!c->progressCallback) {
        return;
    }
    c->progressCallback(c->request, c->state, buffer, len, c->fileBytesReceived, c->totalSize, c->callbackParam);
}

void ghiCallPostCallback(GHIConnection *c) {
    HTTP_ASSERT(c, ov38_022252FC, ov38_02225308, ov38_022252B4, 0x5e);
    if (!c->postCallback) {
        return;
    }
    c->postCallback(c->request, c->bytesPosted, c->totalBytes, c->index, ArrayLength(c->states), c->callbackParam);
}

void ghiCreateLock(void) {
}

void ghiFreeLock(void) {
}

void ghiLock(void) {
}

void ghiUnlock(void) {
}

int ghiDecryptReceivedData(GHIConnection *c) {
    int inLen = 0;
    int outLen = 0;
    int result;
    int remain;
    char *inData;
    char *outData;
    do {
        inData = c->recvBuffer.data + c->recvBuffer.pos;
        inLen = c->recvBuffer.len - c->recvBuffer.pos;
        outData = c->encodeBuffer.data + c->encodeBuffer.len;
        outLen = c->encodeBuffer.size - c->encodeBuffer.len;
        result = c->encryptor.decryptFunc(c, &c->encryptor, inData, &inLen, outData, &outLen);
        if (result == 2) {
            if (!ghiResizeBuffer(&c->encodeBuffer, c->encodeBuffer.sizeIncrement)) {
                return 0;
            }
        }
    } while (result == 2 && outLen == 0);
    c->recvBuffer.pos += inLen;
    c->encodeBuffer.len += outLen;
    if (c->recvBuffer.pos > 0xff) {
        remain = c->recvBuffer.len - c->recvBuffer.pos;
        if (remain == 0) {
            ghiResetBuffer(&c->recvBuffer);
        } else {
            memmove(c->recvBuffer.data, c->recvBuffer.data + c->recvBuffer.pos, remain);
            c->recvBuffer.pos = 0;
            c->recvBuffer.len = remain;
        }
    }
    if (result == 3) {
        c->completed = 1;
        c->result = 0x11;
        return 0;
    }
    return 1;
}

int ghiDoReceive(GHIConnection *c, char *buffer, int *bufferLen) {
    int rcode;
    int len = *bufferLen - 1;
    if (c->throttle) {
        u32 now = current_time();
        if (now < c->lastThrottleRecv + ov38_0222531C.delay) {
            return 1;
        }
        c->lastThrottleRecv = now;
        if (len >= ov38_0222531C.bytesPerRecv) {
            len = ov38_0222531C.bytesPerRecv;
        }
    }
    if (c->encodeBuffer.pos < c->encodeBuffer.len) {
        ghiReadDataFromBuffer(&c->encodeBuffer, buffer, bufferLen);
        if (c->encodeBuffer.pos == c->encodeBuffer.len) {
            c->encodeBuffer.len = c->headerLength;
            c->encodeBuffer.pos = c->headerLength;
        }
        return 0;
    }
    rcode = gsSocketRecv(c->socket, buffer, len, 0);
    if (rcode == -1) {
        int err = gsSocketGetLastError(c->socket);
        if (err == -6 || err == -26 || err == -76) {
            return 1;
        }
        c->completed = 1;
        c->result = 5;
        c->socketError = err;
        c->connectionClosed = 1;
        return 3;
    }
    if (rcode == 0) {
        c->connectionClosed = 1;
        return 2;
    }
    if (c->encryptor.engine) {
        if (!ghiAppendDataToBuffer(&c->recvBuffer, buffer, rcode)) {
            return 3;
        }
        if (!ghiDecryptReceivedData(c)) {
            c->completed = 1;
            c->result = 0x11;
            return 3;
        }
        if (c->encodeBuffer.len - c->encodeBuffer.pos <= 0) {
            buffer[0] = '\0';
            *bufferLen = 0;
            return 1;
        }
        rcode = *bufferLen - 1;
        if (!ghiReadDataFromBuffer(&c->encodeBuffer, buffer, &rcode)) {
            return 3;
        }
        if (c->encodeBuffer.pos == c->encodeBuffer.len) {
            c->encodeBuffer.len = c->headerLength;
            c->encodeBuffer.pos = c->headerLength;
        }
        if (rcode <= 0) {
            return 1;
        }
    }
    buffer[rcode] = '\0';
    *bufferLen = rcode;
    if (rcode <= 0) {
        return 1;
    }
    return 0;
}

int ghiDoSend(GHIConnection *c, const char *buf, int len) {
    int n = gsSocketSend(c->socket, buf, len, 0);
    if (n == -1) {
        int err = gsSocketGetLastError(c->socket);
        if (err == -6 || err == -26 || err == -76) {
            return 0;
        }
        c->completed = 1;
        c->result = 5;
        c->socketError = err;
        return -1;
    }
    if (c->state == 4) {
        c->bytesPosted += n;
    }
    return n;
}

int ghiTrySendThenBuffer(GHIConnection *c, const char *data, int len) {
    int n = 0;
    if (c->sendBuffer.len == 0) {
        n = ghiDoSend(c, data, len);
        if (n == -1) {
            return 0;
        }
        if (n == len) {
            return 1;
        }
    }
    if (!ghiAppendDataToBuffer(&c->sendBuffer, data + n, len - n)) {
        return 0;
    }
    return 2;
}

int ghiFindFreeSlot(void) {
    int i;
    int oldCap;
    int newCap;
    GHIConnection **tmp;
    for (i = 0; i < ov38_0222602C.capacity; i++) {
        if (!ov38_0222602C.list[i]->inUse) {
            return i;
        }
    }
    HTTP_ASSERT(ov38_0222602C.count == ov38_0222602C.capacity, ov38_02225378, ov38_022253A0, ov38_02225324, 0x33);
    oldCap = ov38_0222602C.capacity;
    newCap = ov38_0222602C.capacity + 4;
    tmp = DWCi_GsRealloc(ov38_0222602C.list, newCap * sizeof(GHIConnection *));
    if (!tmp) {
        return -1;
    }
    ov38_0222602C.list = tmp;
    for (i = oldCap; i < newCap; i++) {
        ov38_0222602C.list[i] = DWCi_GsMalloc(sizeof(GHIConnection));
        if (!ov38_0222602C.list[i]) {
            for (i--; i >= oldCap; i--) {
                DWCi_GsFree(ov38_0222602C.list[i]);
            }
            return -1;
        }
        ov38_0222602C.list[i]->inUse = 0;
    }
    ov38_0222602C.capacity = newCap;
    return oldCap;
}

GHIConnection *ghiNewConnection(void) {
    int slot;
    GHIConnection *c;
    int ok;
    ghiLock();
    slot = ghiFindFreeSlot();
    if (slot == -1) {
        ghiUnlock();
        return NULL;
    }
    c = ov38_0222602C.list[slot];
    memset(c, 0, sizeof(GHIConnection));
    c->inUse = 1;
    c->request = slot;
    c->uniqueID = ov38_0222602C.nextUniqueID++;
    c->type = 0;
    c->state = 0;
    c->URL = NULL;
    c->serverAddress = NULL;
    c->serverIP = 0;
    c->serverPort = 0;
    c->requestPath = NULL;
    c->sendHeaders = NULL;
    c->unk_2C = 0;
    c->blocking = 0;
    c->keepAlive = 0;
    c->result = 0;
    c->progressCallback = NULL;
    c->completedCallback = NULL;
    c->callbackParam = NULL;
    c->socket = -1;
    c->socketError = 0;
    c->userBufferSupplied = 0;
    c->statusMajor = 0;
    c->statusMinor = 0;
    c->statusCode = 0;
    c->statusStringIndex = 0;
    c->headerIndex = 0;
    c->headerLength = 0;
    c->completed = 0;
    c->fileBytesReceived = 0;
    c->totalSize = -1;
    c->redirectURL = NULL;
    c->redirectCount = 0;
    c->chunked = 0;
    c->processing = 0;
    c->throttle = 0;
    c->lastThrottleRecv = 0;
    c->post = NULL;
    c->maxRecvTime = 0x1f4;
    c->proxyPort = 0x50;
    c->proxyAddress = NULL;
    c->encryptor.unk_00 = 0;
    ok = ghiInitBuffer(c, &c->sendBuffer, 0x800, 0x1000);
    if (ok) {
        ok = ghiInitBuffer(c, &c->encodeBuffer, 0x800, 0x800);
    }
    if (ok) {
        ok = ghiInitBuffer(c, &c->recvBuffer, 0x800, 0x400);
    }
    if (!ok) {
        ghiFreeConnection(c);
        ghiUnlock();
        return NULL;
    }
    ov38_0222602C.count++;
    ghiUnlock();
    return c;
}

int ghiFreeConnection(GHIConnection *c) {
    HTTP_ASSERT(c, ov38_022253B4, ov38_022253A0, ov38_02225334, 0xb3);
    HTTP_ASSERT(c->request >= 0, ov38_022253C0, ov38_022253A0, ov38_02225334, 0xb4);
    HTTP_ASSERT(c->request < ov38_0222602C.capacity, ov38_022253DC, ov38_022253A0, ov38_02225334, 0xb5);
    HTTP_ASSERT(c->inUse, ov38_02225404, ov38_022253A0, ov38_02225334, 0xb6);
    if (!c) {
        return 0;
    }
    if (!c->inUse) {
        return 0;
    }
    if (c->request < 0) {
        return 0;
    }
    if (c->request >= ov38_0222602C.capacity) {
        return 0;
    }
    ghiLock();
    DWCi_GsFree(c->URL);
    DWCi_GsFree(c->serverAddress);
    DWCi_GsFree(c->requestPath);
    DWCi_GsFree(c->sendHeaders);
    DWCi_GsFree(c->redirectURL);
    DWCi_GsFree(c->proxyAddress);
    if (c->socket != -1) {
        gsSocketShutdown(c->socket, 2);
        gsSocketClose(c->socket);
    }
    ghiFreeBuffer(&c->sendBuffer);
    ghiFreeBuffer(&c->encodeBuffer);
    ghiFreeBuffer(&c->recvBuffer);
    ghiFreeBuffer(&c->getFileBuffer);
    if (c->states) {
        ghiPostCleanupState(c);
    }
    if (c->post) {
        if (ghiIsPostAutoFree(c->post)) {
            ghiFreePost(c->post);
            c->post = NULL;
        }
    }
    if (c->encryptor.initialized) {
        if (c->encryptor.cleanupFunc) {
            c->encryptor.cleanupFunc(c, &c->encryptor);
        }
        c->encryptor.initialized = 0;
    }
    c->inUse = 0;
    ov38_0222602C.count--;
    ghiUnlock();
    return 1;
}

GHIConnection *ghiRequestToConnection(int request) {
    GHIConnection *c;
    HTTP_ASSERT(request >= 0, ov38_02225418, ov38_022253A0, ov38_02225360, 0x101);
    HTTP_ASSERT(request < ov38_0222602C.capacity, ov38_02225428, ov38_022253A0, ov38_02225360, 0x102);
    ghiLock();
    if (request < 0 || request >= ov38_0222602C.capacity) {
        ghiUnlock();
        return NULL;
    }
    c = ov38_0222602C.list[request];
    if (!c->inUse) {
        c = NULL;
    }
    ghiUnlock();
    return c;
}

void ghiEnumConnections(void (*func)(GHIConnection *)) {
    int i;
    if (ov38_0222602C.count <= 0) {
        return;
    }
    ghiLock();
    for (i = 0; i < ov38_0222602C.capacity; i++) {
        GHIConnection *c = ov38_0222602C.list[i];
        if (c->inUse) {
            func(c);
        }
    }
    ghiUnlock();
}

void ghiRedirectConnection(GHIConnection *c) {
    HTTP_ASSERT(c, ov38_022253B4, ov38_022253A0, ov38_02225348, 0x132);
    HTTP_ASSERT(c->redirectURL, ov38_02225444, ov38_022253A0, ov38_02225348, 0x133);
    c->state = 0;
    DWCi_GsFree(c->URL);
    c->URL = c->redirectURL;
    c->redirectURL = NULL;
    DWCi_GsFree(c->serverAddress);
    c->serverAddress = NULL;
    c->serverIP = 0;
    c->serverPort = 0;
    DWCi_GsFree(c->requestPath);
    c->requestPath = NULL;
    gsSocketShutdown(c->socket, 2);
    gsSocketClose(c->socket);
    c->socket = -1;
    ghiResetBuffer(&c->sendBuffer);
    ghiResetBuffer(&c->encodeBuffer);
    ghiResetBuffer(&c->recvBuffer);
    c->statusMajor = 0;
    c->statusMinor = 0;
    c->statusCode = 0;
    c->statusStringIndex = 0;
    c->headerIndex = 0;
    c->headerLength = 0;
    c->connectionClosed = 0;
    c->redirectCount++;
}

void ghiCleanupConnections(void) {
    int i;
    if (!ov38_0222602C.list) {
        return;
    }
    ghiEnumConnections((void (*)(GHIConnection *))ghiFreeConnection);
    for (i = 0; i < ov38_0222602C.capacity; i++) {
        DWCi_GsFree(ov38_0222602C.list[i]);
    }
    DWCi_GsFree(ov38_0222602C.list);
    ov38_0222602C.list = NULL;
    ov38_0222602C.capacity = 0;
    ov38_0222602C.count = 0;
}

void ghiHandleStatus(GHIConnection *c) {
    switch (c->statusCode / 100) {
    case 1:
    case 2:
    case 3:
        return;
    case 4:
        switch (c->statusCode) {
        case 401:
            c->result = 9;
            return;
        case 403:
            c->result = 10;
            return;
        case 404:
        case 410:
            c->result = 11;
            return;
        default:
            c->result = 8;
            return;
        }
    case 5:
        c->result = 12;
        return;
    }
}

int ghiProcessConnection(GHIConnection *c) {
    int completed;
    HTTP_ASSERT(c, ov38_02225480, ov38_0222548C, ov38_02225468, 0x5b);
    HTTP_ASSERT(ghiRequestToConnection(c->request) == c, ov38_02225498, ov38_0222548C, ov38_02225468, 0x5c);
    if (c->processing) {
        return 0;
    }
    c->processing = 1;
    if (c->state == 0) {
        ghiDoHostLookup(c);
    }
    if (c->state == 1) {
        ghiDoConnecting(c);
    }
    if (c->state == 2) {
        ghiDoSecuringSession(c);
    }
    if (c->state == 3) {
        ghiDoSendingRequest(c);
    }
    if (c->state == 4) {
        ghiDoPosting(c);
    }
    if (c->state == 5) {
        ghiDoWaiting(c);
    }
    if (c->state == 6) {
        ghiDoReceivingStatus(c);
    }
    if (c->state == 7) {
        ghiDoReceivingHeaders(c);
    }
    if (c->state == 8) {
        ghiDoReceivingFile(c);
    }
    if (c->redirectURL) {
        ghiRedirectConnection(c);
    }
    completed = c->completed;
    if (completed) {
        ghiHandleStatus(c);
        ghiCallCompletedCallback(c);
        ghiFreeConnection(c);
    } else {
        c->processing = 0;
    }
    return completed;
}

void ghttpStartup(void) {
    ghiLock();
    if (++ov38_0222603C == 1) {
        ghiCreateLock();
        ov38_02225320 = 0x7d;
        ov38_0222531C.delay = 0xfa;
        return;
    }
    ghiUnlock();
}

void ghttpCleanup(void) {
    ghiLock();
    if (--ov38_0222603C == 0) {
        ghiCleanupConnections();
        if (ov38_02226028) {
            DWCi_GsFree(ov38_02226028);
            ov38_02226028 = NULL;
        }
        ghiUnlock();
        ghiFreeLock();
        return;
    }
    ghiUnlock();
}

int ghttpGetA(const char *url, int blocking, CompletedCallback completed, void *param) {
    return ghttpGetExA(url, NULL, NULL, 0, NULL, 0, blocking, NULL, completed, param);
}

int ghttpGetExA(const char *url, const char *headers, char *buffer, int bufferSize, GHIPost *post, int throttle, int blocking, ProgressCallback progress, CompletedCallback completed, void *param) {
    GHIConnection *c;
    int ok;
    HTTP_ASSERT(url && *url, ov38_022254D4, ov38_0222548C, ov38_0222545C, 0x128);
    HTTP_ASSERT(bufferSize >= 0, ov38_022254E4, ov38_0222548C, ov38_0222545C, 0x129);
    HTTP_ASSERT(!buffer || bufferSize, ov38_022254F4, ov38_0222548C, ov38_0222545C, 0x12a);
    if (!url || !*url) {
        return -1;
    }
    if (bufferSize < 0) {
        return -1;
    }
    if (buffer && !bufferSize) {
        return -1;
    }
    if (!ov38_0222603C) {
        ghttpStartup();
    }
    c = ghiNewConnection();
    if (!c) {
        return -1;
    }
    c->type = 0;
    c->URL = goastrdup(url);
    if (!c->URL) {
        ghiFreeConnection(c);
        return -1;
    }
    if (headers && *headers) {
        c->sendHeaders = goastrdup(headers);
        if (!c->sendHeaders) {
            ghiFreeConnection(c);
            return -1;
        }
    }
    c->post = post;
    c->blocking = blocking;
    c->progressCallback = progress;
    c->completedCallback = completed;
    c->callbackParam = param;
    c->throttle = throttle;
    c->userBufferSupplied = buffer != NULL;
    if (c->userBufferSupplied) {
        ok = ghiInitFixedBuffer(c, &c->getFileBuffer, buffer, bufferSize);
    } else {
        ok = ghiInitBuffer(c, &c->getFileBuffer, 0x800, 0x800);
    }
    if (!ok) {
        ghiFreeConnection(c);
        return -1;
    }
    if (post) {
        if (!ghiPostInitState(c)) {
            ghiFreeConnection(c);
            return -1;
        }
    }
    if (blocking) {
        while (!ghiProcessConnection(c)) {
            msleep(10);
        }
        return 0;
    }
    return c->request;
}

void ghttpThink(void) {
    ghiEnumConnections((void (*)(GHIConnection *))ghiProcessConnection);
}

int ghiIsPostAutoFree(GHIPost *post) {
    return post->autoFree;
}

void ghiFreePost(GHIPost *post) {
    ArrayFree(post->data);
    DWCi_GsFree(post);
}

char *ghiPostGetContentType(GHIConnection *c) {
    GHIPost *post = c->post;
    HTTP_ASSERT(post, ov38_02225694, ov38_02225644, ov38_02225574, 0x192);
    if (!post) {
        return ov38_0222569C;
    }
    if (post->hasFiles) {
        return ov38_022256A0;
    }
    return ov38_022256E4;
}

int ghiPostGetNoFilesContentLength(GHIConnection *c) {
    GHIPost *post = c->post;
    GHIPostData *d;
    int i;
    int num;
    int total = 0;
    num = ArrayLength(post->data);
    if (!num) {
        return 0;
    }
    for (i = 0; i < num; i++) {
        d = ArrayNth(post->data, i);
        HTTP_ASSERT(d->type == 0, ov38_02225708, ov38_02225644, ov38_022255E0, 0x1b0);
        total += strlen(d->name);
        total += d->u.string.len;
        total += d->u.string.numEscaped * 2;
        total++;
    }
    total += num - 1;
    return total;
}

int ghiPostGetHasFilesContentLength(GHIConnection *c) {
    GHIPost *post = c->post;
    GHIPostData *d;
    int i;
    int num;
    int total = 0;
    GHIPostState *st;
    if (!ov38_02226040.boundary) {
        ov38_02226040.boundary = strlen(ov38_02225720);
        ov38_02226040.stringField = ov38_02226040.boundary + 0x2f;
        ov38_02226040.fileField = ov38_02226040.boundary + 0x4c;
        ov38_02226040.footer = ov38_02226040.boundary + 4;
    }
    num = ArrayLength(post->data);
    for (i = 0; i < num; i++) {
        d = ArrayNth(post->data, i);
        if (d->type == 0) {
            total += ov38_02226040.stringField;
            total += strlen(d->name);
            total += d->u.string.len;
        } else if (d->type == 1) {
            total += ov38_02226040.fileField;
            total += strlen(d->name);
            total += strlen(d->u.disk.fileName);
            total += strlen(d->u.disk.contentType);
            st = ArrayNth(c->states, i);
            HTTP_ASSERT(st, ov38_02225748, ov38_02225644, ov38_02225620, 0x1ec);
            total += st->len;
        } else if (d->type == 2) {
            total += ov38_02226040.fileField;
            total += strlen(d->name);
            total += strlen(d->u.memory.fileName);
            total += strlen(d->u.memory.contentType);
            total += d->u.memory.len;
        } else {
            HTTP_ASSERT(0, ov38_02225640, ov38_02225644, ov38_02225620, 0x1f9);
            return 0;
        }
    }
    return total + ov38_02226040.footer;
}

int ghiPostGetContentLength(GHIConnection *c) {
    GHIPost *post = c->post;
    HTTP_ASSERT(post, ov38_02225694, ov38_02225644, ov38_0222558C, 0x20c);
    if (!post) {
        return 0;
    }
    if (post->hasFiles) {
        return ghiPostGetHasFilesContentLength(c);
    }
    return ghiPostGetNoFilesContentLength(c);
}

int ghiPostStateInit(GHIPostState *s) {
    int type = s->data->type;
    s->pos = -1;
    if (type == 0) {
    } else if (type == 1) {
        if (!s->file) {
            return 0;
        }
        if (fseek(s->file, 0, 2)) {
            return 0;
        }
        s->len = ftell(s->file);
        if (s->len == -1) {
            return 0;
        }
        rewind(s->file);
    } else if (type != 2) {
        HTTP_ASSERT(0, ov38_02225640, ov38_02225644, ov38_02225534, 0x244);
        return 0;
    }
    return 1;
}

void ghiPostStateCleanup(GHIPostState *s) {
    int type = s->data->type;
    if (type == 0) {
    } else if (type == 1) {
        if (s->file) {
            fclose(s->file);
        }
        s->file = NULL;
    } else if (type != 2) {
        HTTP_ASSERT(0, ov38_02225640, ov38_02225644, ov38_02225548, 0x269);
    }
}

int ghiPostInitState(GHIConnection *c) {
    int i;
    int num;
    GHIPostState state;
    GHIPostData *d;
    HTTP_ASSERT(c->post, ov38_02225750, ov38_02225644, ov38_0222550C, 0x278);
    if (!c->post) {
        return 0;
    }
    c->index = 0;
    c->bytesPosted = 0;
    c->totalBytes = 0;
    c->postCallback = c->post->callback;
    c->postParam = c->post->param;
    num = ArrayLength(c->post->data);
    c->states = ArrayNew(sizeof(GHIPostState), num, 0);
    if (!c->states) {
        return 0;
    }
    for (i = 0; i < num; i++) {
        d = ArrayNth(c->post->data, i);
        ClearPostState(&state);
        state.data = d;
        if (!ghiPostStateInit(&state)) {
            for (i--; i >= 0; i--) {
                ghiPostStateCleanup(ArrayNth(c->states, i));
            }
            ArrayFree(c->states);
            c->states = NULL;
            return 0;
        }
        ArrayAppend(c->states, &state);
    }
    HTTP_ASSERT(ArrayLength(c->post->data) == ArrayLength(c->states), ov38_02225764, ov38_02225644, ov38_0222550C, 0x2b1);
    c->totalBytes = ghiPostGetContentLength(c);
    return 1;
}

void ghiPostCleanupState(GHIConnection *c) {
    int i;
    int num;
    if (c->states) {
        num = ArrayLength(c->states);
        for (i = 0; i < num; i++) {
            ghiPostStateCleanup(ArrayNth(c->states, i));
        }
        ArrayFree(c->states);
        c->states = NULL;
    }
    if (c->post && c->post->autoFree) {
        ghiFreePost(c->post);
        c->post = NULL;
    }
}

int ghiPostStringStateDoPosting(GHIPostState *state, GHIConnection *c) {
    HTTP_ASSERT(state->pos >= 0, ov38_022257B8, ov38_02225644, ov38_022255A4, 743);
    if (state->data->u.string.len == 0) {
        return 1;
    }
    HTTP_ASSERT(state->pos < state->data->u.string.len, ov38_022257C8, ov38_02225644, ov38_022255A4, 750);
    if (!c->post->hasFiles && state->data->u.string.encode) {
        int i;
        int ch;
        char *input;
        GHIPercentEscape escape;
        input = state->data->u.string.data;
        escape = _02224E60;
        i = 0;
        ch = input[0];
        while (ch) {
            if (strchr(ov38_02225650, ch)) {
                ghiAppendCharToBuffer(&c->sendBuffer, ch);
            } else if (ch == ' ') {
                ghiAppendCharToBuffer(&c->sendBuffer, '+');
            } else {
                int high = ch / 16;
                HTTP_ASSERT(high < 16, ov38_022257F4, ov38_02225644, ov38_022255A4, 781);
                escape.chars[1] = ov38_02225804[high];
                escape.chars[2] = ov38_02225804[ch % 16];
                ghiAppendDataToBuffer(&c->sendBuffer, escape.chars, 3);
            }
            ch = input[++i];
        }
        return 1;
    } else {
        int left = state->data->u.string.len - state->pos;
        int sent = ghiDoSend(c, state->data->u.string.data, left);
        if (sent == -1) {
            return 0;
        }
        state->pos += sent;
        return sent == left ? 1 : 2;
    }
}

int ghiPostFileDiskStateDoPosting(GHIPostState *state, GHIConnection *c) {
    char buf[0x1000];
    int n;
    int r;
    HTTP_ASSERT(state->pos >= 0, ov38_022257B8, ov38_02225644, ov38_022255C0, 0x336);
    HTTP_ASSERT(state->pos < state->len, ov38_02225818, ov38_02225644, ov38_022255C0, 0x337);
    HTTP_ASSERT(state->pos == ftell(state->file), ov38_02225840, ov38_02225644, ov38_022255C0, 0x338);
    do {
        n = fread(buf, 1, 0x1000, state->file);
        if (n <= 0) {
            c->completed = 1;
            c->result = 0xe;
            return 0;
        }
        state->pos += n;
        if (state->pos > state->len) {
            c->completed = 1;
            c->result = 0xe;
            return 0;
        }
        r = ghiTrySendThenBuffer(c, buf, n);
        if (!r) {
            return 0;
        }
        if (state->pos == state->len) {
            return 1;
        }
    } while (r == 1);
    return 2;
}

int ghiPostFileMemoryStateDoPosting(GHIPostState *state, GHIConnection *c) {
    HTTP_ASSERT(state->pos >= 0, ov38_022257B8, ov38_02225644, ov38_02225600, 0x36e);
    if (state->data->u.memory.len == 0) {
        return 1;
    }
    HTTP_ASSERT(state->pos < state->data->u.memory.len, ov38_02225878, ov38_02225644, ov38_02225600, 0x375);
    for (;;) {
        GHIPostData *data = state->data;
        int pos = state->pos;
        int len = data->u.memory.len - pos;
        int sent;
        if (len >= 0x8000) {
            len = 0x8000;
        }
        sent = ghiDoSend(c, data->u.memory.data + pos, len);
        if (sent == -1) {
            return 0;
        }
        state->pos += sent;
        if (state->data->u.memory.len == state->pos) {
            return 1;
        }
        if (sent == 0) {
            return 2;
        }
    }
}

int ghiPostStateDoPosting(GHIPostState *state, GHIConnection *c, int first) {
    char buffer[2048];
    int status;
    if (state->pos == -1) {
        state->pos = 0;
        if (c->post->hasFiles == 0) {
            HTTP_ASSERT(state->data->type == GHI_POST_DATA_STRING, ov38_022258A8, ov38_02225644, ov38_0222555C, 937);
            if (first) {
                sprintf(buffer, ov38_022258C8, state->data->name);
            } else {
                sprintf(buffer, ov38_022258CC, state->data->name);
            }
        } else if (state->data->type == GHI_POST_DATA_STRING) {
            sprintf(buffer, ov38_02225928, first ? ov38_022258D4 : ov38_022258FC, state->data->name);
        } else if (state->data->type == GHI_POST_DATA_FILE_DISK || state->data->type == GHI_POST_DATA_FILE_MEMORY) {
            char *fileName;
            char *contentType;
            if (state->data->type == GHI_POST_DATA_FILE_DISK) {
                fileName = state->data->u.disk.fileName;
                contentType = state->data->u.disk.contentType;
            } else {
                fileName = state->data->u.memory.fileName;
                contentType = state->data->u.memory.contentType;
            }
            sprintf(buffer, ov38_02225958, first ? ov38_022258D4 : ov38_022258FC, state->data->name, fileName, contentType);
        } else {
            __msl_assertion_failed(ov38_02225640, ov38_02225644, ov38_0222555C, 988);
        }
        status = ghiTrySendThenBuffer(c, buffer, strlen(buffer));
        if (status == 0) {
            return 0;
        }
        if (status == 2) {
            return 2;
        }
    }
    if (state->data->type == GHI_POST_DATA_STRING) {
        return ghiPostStringStateDoPosting(state, c);
    }
    if (state->data->type == GHI_POST_DATA_FILE_DISK) {
        return ghiPostFileDiskStateDoPosting(state, c);
    }
    HTTP_ASSERT(state->data->type == GHI_POST_DATA_FILE_MEMORY, ov38_022259AC, ov38_02225644, ov38_0222555C, 1013);
    return ghiPostFileMemoryStateDoPosting(state, c);
}

int ghiPostDoPosting(GHIConnection *c) {
    int count;
    int status;
    HTTP_ASSERT(c, ov38_022259D0, ov38_02225644, ov38_02225520, 1028);
    HTTP_ASSERT(c->post, ov38_02225750, ov38_02225644, ov38_02225520, 1029);
    HTTP_ASSERT(c->states, ov38_022259DC, ov38_02225644, ov38_02225520, 1030);
    count = ArrayLength(c->post->data);
    HTTP_ASSERT(count == ArrayLength(c->states), ov38_02225764, ov38_02225644, ov38_02225520, 1031);
    HTTP_ASSERT(c->index >= 0, ov38_022259FC, ov38_02225644, ov38_02225520, 1032);
    HTTP_ASSERT(c->index <= ArrayLength(c->states), ov38_02225A20, ov38_02225644, ov38_02225520, 1033);
    count = ArrayLength(c->states);
    if (c->sendBuffer.len) {
        if (!ghiSendBufferedData(c)) {
            return 0;
        }
        if (c->sendBuffer.pos < c->sendBuffer.len) {
            return 2;
        }
        ghiResetBuffer(&c->sendBuffer);
        if (c->index == count) {
            return 1;
        }
    }
    for (; c->index < count; c->index++) {
        GHIPostState *state = ArrayNth(c->states, c->index);
        HTTP_ASSERT(state, ov38_02225A70, ov38_02225644, ov38_02225520, 1071);
        status = ghiPostStateDoPosting(state, c, c->index == 0);
        if (status == 0) {
            HTTP_ASSERT(c->completed && c->result, ov38_02225A7C, ov38_02225644, ov38_02225520, 1083);
            return 0;
        }
        if (status == 2) {
            return 2;
        }
    }
    if (c->post->hasFiles) {
        int length = strlen(ov38_02225AA8);
        if (!ghiTrySendThenBuffer(c, ov38_02225AA8, length)) {
            return 0;
        }
    }
    return c->sendBuffer.len ? 2 : 1;
}
