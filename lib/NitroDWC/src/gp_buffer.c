#pragma force_active on
#include <string.h>

#include "global.h"

#include "dwc/gp_internal.h"

// The code below is compiled as ARM (32-bit) code.
#include <nitro/code32.h>

// Presence connection buffers ("gpiBuffer.c": growable output / input buffers, buffered socket sends and
// reads, "\msg\" framing) and the queued user callbacks ("gpiCallback.c").

extern const char ov00_02218370[]; // "gpiRecvToBuffer"
extern const char ov00_02218380[]; // "gpiSendFromBuffer"
extern const char ov00_02218394[]; // "gpiSendOrBufferChar"
extern const char ov00_022183A8[]; // "gpiAppendCharToBuffer"
extern const char ov00_022183C0[]; // "gpiSendOrBufferStringLen"
extern const char ov00_022183DC[]; // "gpiAppendStringToBufferLen"
extern const char ov00_022183F8[]; // "outputBuffer != NULL"
extern const char ov00_02218410[]; // "gpiBuffer.c"
extern const char ov00_0221841C[]; // "Out of memory."
extern const char ov00_0221842C[]; // "string != NULL"
extern const char ov00_0221843C[]; // "stringLen >= 0"
extern const char ov00_0221844C[]; // "%d"
extern const char ov00_02218450[]; // "There was an error sending on a socket."
extern const char ov00_02218478[]; // "SENDXXXX(%s): Connection closed\n"
extern const char ov00_0221849C[]; // "peer->outputBuffer.buffer != NULL"
extern const char ov00_022184C0[]; // "PT"
extern const char ov00_022184C4[]; // "sock != INVALID_SOCKET"
extern const char ov00_022184DC[]; // "inputBuffer != NULL"
extern const char ov00_022184F0[]; // "bytesRead != NULL"
extern const char ov00_02218504[]; // "connClosed != NULL"
extern const char ov00_02218518[]; // "There was an error reading from a socket."
extern const char ov00_02218544[]; // "RECVXXXX(%s): Connection closed\n"
extern const char ov00_02218568[]; // "RECVTOTL(%s): %d\n"
extern const char ov00_0221857C[]; // "len >= 0"
extern const char ov00_02218588[]; // "pos >= 0"
extern const char ov00_02218594[]; // "pos <= len"
extern const char ov00_022185A0[]; // "\\msg\\"
extern const char ov00_022185A8[]; // "\\m\\"
extern const char ov00_022185AC[]; // "\\len\\"
extern const char ov00_022185B4[]; // "gpiCallCallback"
extern const char ov00_022185C4[]; // "gpiCallErrorCallback"
extern const char ov00_022185DC[]; // "iconnection != NULL"
extern const char ov00_022185F0[]; // "gpiCallback.c"
extern const char ov00_02218600[]; // "result != GP_NO_ERROR"
extern const char ov00_02218618[]; // "(fatal == GP_FATAL) || (fatal == GP_NON_FATAL)"
extern const char ov00_02218648[]; // "Out of memory."
extern const char ov00_02218658[]; // "data->callback.callback != NULL"
extern const char ov00_02218678[]; // "data->arg != NULL"

extern int sprintf(char *, const char *, ...);
extern int strncmp(const char *, const char *, size_t);
extern long atol(const char *);
extern char *strchr(const char *, int);
extern void __msl_assertion_failed(const char *, const char *, const char *, int);
extern void *DWCi_GsMalloc(u32 size);
extern void *DWCi_GsRealloc(void *p, int size);
extern void DWCi_GsFree(void *p);
extern int ArrayLength(void *);
extern int gsSocketRecv(int, char *, int, int);
extern int gsSocketSend(int, void *, int, int);
extern int gsSocketGetLastError(int);

int gpiSendOrBufferString(GpConnection **connection, GpPeer *peer, char *string);
void gpiCallErrorCallback(GpConnection **connection, int result, int fatal);
int gpiAddCallback(GpConnection **connection, volatile GpCallback callback, void *arg, GpOperation *operation, int type, ...);

int gpiAppendCharToBuffer(GpConnection **connection, GpBuffer *outputBuffer, int c) {
    int len, size;
    char *buffer;
    if (!outputBuffer) {
        __msl_assertion_failed(ov00_022183F8, ov00_02218410, ov00_022183A8, 0x25);
    }
    len = outputBuffer->len;
    buffer = outputBuffer->buffer;
    size = outputBuffer->size;
    if (size == len) {
        size += 2048;
        buffer = DWCi_GsRealloc(buffer, size + 1);
        if (!buffer) {
            gpiSetErrorString(connection, ov00_0221841C);
            return 1;
        }
    }
    buffer[len] = c;
    buffer[len + 1] = 0;
    outputBuffer->len++;
    outputBuffer->size = size;
    outputBuffer->buffer = buffer;
    return 0;
}

int gpiAppendStringToBufferLen(GpConnection **connection, GpBuffer *outputBuffer, const char *string, int stringLen) {
    int len, size;
    char *buffer;
    if (!string) {
        __msl_assertion_failed(ov00_0221842C, ov00_02218410, ov00_022183DC, 0x51);
    }
    if (stringLen < 0) {
        __msl_assertion_failed(ov00_0221843C, ov00_02218410, ov00_022183DC, 0x52);
    }
    if (!outputBuffer) {
        __msl_assertion_failed(ov00_022183F8, ov00_02218410, ov00_022183DC, 0x53);
    }
    if (!string) {
        return 0;
    }
    len = outputBuffer->len;
    size = outputBuffer->size;
    buffer = outputBuffer->buffer;
    if (size - len < stringLen) {
        size += stringLen < 2048 ? 2048 : stringLen;
        buffer = DWCi_GsRealloc(buffer, size + 1);
        if (!buffer) {
            gpiSetErrorString(connection, ov00_0221841C);
            return 1;
        }
    }
    memcpy(buffer + len, string, stringLen);
    buffer[len + stringLen] = 0;
    outputBuffer->len += stringLen;
    outputBuffer->size = size;
    outputBuffer->buffer = buffer;
    return 0;
}

int gpiAppendStringToBuffer(GpConnection **connection, GpBuffer *outputBuffer, const char *string) {
    return gpiAppendStringToBufferLen(connection, outputBuffer, string, strlen(string));
}

int gpiAppendIntToBuffer(GpConnection **connection, GpBuffer *outputBuffer, int n) {
    char buf[16];
    sprintf(buf, ov00_0221844C, n);
    return gpiAppendStringToBuffer(connection, outputBuffer, buf);
}

#pragma optimize_for_size on

// Sends what the socket takes; "PR"-tagged sockets fail silently.
int gpiSendData(GpConnection **connection, int sock, void *data, int len, int *connClosed, int *bytesSent, const char *tag) {
    u32 n = gsSocketSend(sock, data, len, 0);
    if (n == -1) {
        int err = gsSocketGetLastError(sock);
        if (err != -6 && err != -26 && err != -76) {
            if (tag[0] == 'P' && tag[1] == 'R') {
                return 3;
            }
            gpiSetError(connection, 5, ov00_02218450);
            gpiCallErrorCallback(connection, 3, 0);
            return 3;
        }
        *bytesSent = 0;
        *connClosed = 0;
    } else {
        if (n > 0) {
            *bytesSent = n;
            *connClosed = 0;
            goto finish;
        }
        gpiDebug(connection, ov00_02218478, tag);
        *bytesSent = 0;
        *connClosed = 1;
    }
finish:
    return 0;
}

#pragma optimize_for_size reset

// gpiSendData was compiled for size in its own file; keep it out of line.
#pragma dont_inline on

int gpiSendOrBufferChar(GpConnection **connection, GpPeer *peer, char c, ...) {
    int closed, sent, result;
    if (peer->outputBuffer.buffer == 0) {
        __msl_assertion_failed(ov00_0221849C, ov00_02218410, ov00_02218394, 222);
    }
    if (peer->outputBuffer.len - peer->outputBuffer.pos == 0 && ArrayLength(peer->messages) == 0) {
        result = gpiSendData(connection, peer->sock, &c, 1, &closed, &sent, ov00_022184C0);
        if (result) {
            return result;
        }
        if (sent) {
            return 0;
        }
    }
    return gpiAppendCharToBuffer(connection, &peer->outputBuffer, *(s8 *)(&c));
}

int gpiSendOrBufferStringLen(GpConnection **connection, GpPeer *peer, char *string, int stringLen) {
    int total, result, closed, sent;
    if (!peer->outputBuffer.buffer) {
        __msl_assertion_failed(ov00_0221849C, ov00_02218410, ov00_022183C0, 0xfb);
    }
    total = 0;
    if (!stringLen) {
        return total;
    }
    if (peer->outputBuffer.len - peer->outputBuffer.pos == 0 && ArrayLength(peer->messages) == 0) {
        do {
            result = gpiSendData(connection, peer->sock, string + total, stringLen, &closed, &sent, ov00_022184C0);
            if (result) {
                return result;
            }
            if (sent) {
                stringLen -= sent;
                total += sent;
            }
        } while (sent && stringLen);
    }
    if (stringLen) {
        result = gpiAppendStringToBufferLen(connection, &peer->outputBuffer, string + total, stringLen);
        if (result) {
            return result;
        }
    }
    return 0;
}

int gpiSendOrBufferString(GpConnection **connection, GpPeer *peer, char *string) {
    return gpiSendOrBufferStringLen(connection, peer, string, strlen(string));
}

#pragma opt_lifetimes off

int gpiRecvToBuffer(GpConnection **connection, int sock, GpBuffer *inputBuffer, int *bytesRead, int *connClosed, const char *tag) {
    char *buffer;
    int len, size, read, total, closed;

    struct {
        int a, b, c;
    } errors;

    if (sock == -1) {
        __msl_assertion_failed(ov00_022184C4, ov00_02218410, ov00_02218370, 0x150);
    }
    if (!inputBuffer) {
        __msl_assertion_failed(ov00_022184DC, ov00_02218410, ov00_02218370, 0x151);
    }
    if (!bytesRead) {
        __msl_assertion_failed(ov00_022184F0, ov00_02218410, ov00_02218370, 0x152);
    }
    if (!connClosed) {
        __msl_assertion_failed(ov00_02218504, ov00_02218410, ov00_02218370, 0x153);
    }
    len = inputBuffer->len;
    buffer = inputBuffer->buffer;
    size = inputBuffer->size;
    errors.c = -76;
    errors.b = -26;
    errors.a = -6;
    total = 0;
    closed = 0;
    do {
        if (len + 2048 > size) {
            size = len + 2048;
            buffer = DWCi_GsRealloc(buffer, size + 1);
            if (!buffer) {
                gpiSetErrorString(connection, ov00_0221841C);
                return 1;
            }
        }
        read = gsSocketRecv(sock, buffer + len, size - len, 0);
        if (read == -1) {
            int error = gsSocketGetLastError(sock);
            if (error != errors.a && error != errors.b && error != errors.c) {
                gpiSetErrorString(connection, ov00_02218518);
                return 3;
            }
        } else if (read) {
            len += read;
            total += read;
        } else {
            closed = 1;
            gpiDebug(connection, ov00_02218544, tag);
        }
        buffer[len] = 0;
    } while (read != -1 && !closed && total < 0x20000);
    if (total) {
        gpiDebug(connection, ov00_02218568, tag, total);
    }
    inputBuffer->buffer = buffer;
    inputBuffer->len = len;
    inputBuffer->size = size;
    *bytesRead = total;
    *connClosed = closed;
    return 0;
}

#pragma opt_lifetimes reset

int gpiSendFromBuffer(GpConnection **connection, int sock, GpBuffer *outputBuffer, int *connClosed, int clear, const char *tag) {
    if (!outputBuffer) {
        __msl_assertion_failed(ov00_022183F8, ov00_02218410, ov00_02218380, 441);
    }
    int total = 0, left;
    char *buffer;
    int pos, len;
    len = outputBuffer->len;
    pos = outputBuffer->pos;
    buffer = outputBuffer->buffer;
    left = len - pos;
    int result, closed, sent;
    if (left == 0) {
        return total;
    }
    do {
        result = gpiSendData(connection, sock, buffer + (pos + total), left, &closed, &sent, tag);
        if (result) {
            return result;
        }
        if (sent) {
            left -= sent;
            total += sent;
        }
    } while (sent && left);
    if (clear) {
        if (total > 0) {
            memmove(buffer, buffer + total, left + 1);
            len -= total;
        }
    } else {
        pos += total;
    }
    if (len < 0) {
        __msl_assertion_failed(ov00_0221857C, ov00_02218410, ov00_02218380, 478);
    }
    if (pos < 0) {
        __msl_assertion_failed(ov00_02218588, ov00_02218410, ov00_02218380, 479);
    }
    if (pos > len) {
        __msl_assertion_failed(ov00_02218594, ov00_02218410, ov00_02218380, 480);
    }
    outputBuffer->len = len;
    outputBuffer->pos = pos;
    if (connClosed) {
        *connClosed = closed;
    }
    return 0;
}

#pragma dont_inline reset

// Takes the next complete "...\msg\...\m\<type>\len\<n>\n<n bytes>" message out of inputBuffer, if any.
int gpiReadMessage(GpConnection **connection, GpBuffer *inputBuffer, char **message, int *type, int *len) {
    char tmp[16], *end;
    int n;
    *message = 0;
    if (inputBuffer->len < 5) {
        return 0;
    }
    end = strchr(inputBuffer->buffer, 10);
    if (end) {
        if (strncmp(end - 5, ov00_022185A0, 5)) {
            return 3;
        }
        *end = 0;
        if (!gpiValueForKey(inputBuffer->buffer, ov00_022185A8, tmp, 16)) {
            return 3;
        }
        *type = atol(tmp);
        if (!gpiValueForKey(inputBuffer->buffer, ov00_022185AC, tmp, 16)) {
            return 3;
        }
        n = atol(tmp);
        if (inputBuffer->len > (n + 1) + (end - inputBuffer->buffer)) {
            if (end[n + 1]) {
                return 3;
            }
            *message = end + 1;
            *len = n;
            inputBuffer->pos = (n + 1) + (end - inputBuffer->buffer) + 1;
        } else {
            *end = 10;
        }
    }
    return 0;
}

// Drops the message taken by gpiReadMessage.
int gpiClipBufferToPosition(GpConnection **connection, GpBuffer *inputBuffer) {
    if (!inputBuffer || !inputBuffer->buffer || !inputBuffer->pos) {
        return 0;
    }
    inputBuffer->len -= inputBuffer->pos;
    if (inputBuffer->len) {
        memmove(inputBuffer->buffer, inputBuffer->buffer + inputBuffer->pos, inputBuffer->len);
    }
    inputBuffer->buffer[inputBuffer->len] = 0;
    inputBuffer->pos = 0;
    return 0;
}

void gpiCallErrorCallback(GpConnection **connection, int result, int fatal) {
    GpConnection *iconnection = *connection;
    GpCallback callback;
    GpErrorArg *arg;
    if (!iconnection) {
        __msl_assertion_failed(ov00_022185DC, ov00_022185F0, ov00_022185C4, 35);
    }
    if (!result) {
        __msl_assertion_failed(ov00_02218600, ov00_022185F0, ov00_022185C4, 36);
    }
    if (fatal != 1 && fatal != 0) {
        __msl_assertion_failed(ov00_02218618, ov00_022185F0, ov00_022185C4, 37);
    }
    if (fatal == 1) {
        iconnection->fatalError = 1;
    }
    callback.callback = iconnection->callbacks[GP_CALLBACK_ERROR].callback;
    callback.param = iconnection->callbacks[GP_CALLBACK_ERROR].param;
    if (callback.callback == 0) {
        return;
    }
    arg = DWCi_GsMalloc(16);
    if (arg) {
        arg->result = result;
        arg->fatal = fatal;
        arg->errorCode = iconnection->errorCode;
        arg->errorString = iconnection->errorString;
    }
    gpiAddCallback(connection, callback, arg, 0, 1);
}

// Queues a callback; gpiCallCallback calls it later.
int gpiAddCallback(GpConnection **connection, volatile GpCallback callback, void *arg, GpOperation *operation, int type, ...) {
    GpConnection *iconnection = *connection;
    GpCallbackData *data = DWCi_GsMalloc(24);
    if (!data) {
        gpiSetErrorString(connection, ov00_02218648);
        return 1;
    }
    data->callback = callback.callback;
    data->param = callback.param;
    data->arg = arg;
    if (operation) {
        data->operationId = operation->id;
    } else {
        data->operationId = 0;
    }
    data->type = type;
    data->next = 0;
    if (!iconnection->callbacksHead) {
        iconnection->callbacksHead = data;
    }
    if (iconnection->callbacksTail) {
        iconnection->callbacksTail->next = data;
    }
    iconnection->callbacksTail = data;
    return 0;
}

// Calls a queued callback and frees its argument (with the strings / lists the argument type owns).
void gpiCallCallback(GpConnection **connection, GpCallbackData *data) {
    if (!data->callback) {
        __msl_assertion_failed(ov00_02218658, ov00_022185F0, ov00_022185B4, 110);
    }
    if (!data->arg) {
        __msl_assertion_failed(ov00_02218678, ov00_022185F0, ov00_022185B4, 111);
    }
    ((void (*)(GpConnection **, void *, void *))data->callback)(connection, data->arg, data->param);
    if (data->type == 2) {
        void **p = data->arg;
        DWCi_GsFree(p[2]);
        ((void **)data->arg)[2] = 0;
    } else if (data->type == 3) {
        int i;
        GpListArg *list = data->arg;
        for (i = 0; i < list->count; i++) {
            DWCi_GsFree(list->a[i]);
            list->a[i] = 0;
            DWCi_GsFree(list->b[i]);
            list->b[i] = 0;
        }
        DWCi_GsFree(list->a);
        list->a = 0;
        DWCi_GsFree(list->b);
        list->b = 0;
    } else if (data->type == 4) {
        void **p = data->arg;
        DWCi_GsFree(p[3]);
        p[3] = 0;
    } else if (data->type == 7) {
        void **p = data->arg;
        if (p[4]) {
            DWCi_GsFree(p[4]);
            p[4] = 0;
        }
    } else if (data->type == 8) {
        void **p = data->arg;
        if (p[2]) {
            DWCi_GsFree(p[2]);
            p[2] = 0;
        }
    } else if (data->type == 9) {
        int i;

        struct {
            u32 x;
            int count;
            void **p;
        } *p = data->arg;

        for (i = 0; i < p->count; i++) {
            DWCi_GsFree(p->p[i]);
            p->p[i] = 0;
        }
        DWCi_GsFree(p->p);
        p->p = 0;
    }
    DWCi_GsFree(data->arg);
    data->arg = 0;
    DWCi_GsFree(data);
}
