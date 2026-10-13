#pragma force_active on
#include "global.h"

#include "dwc/http_internal.h"

// The code below is compiled as ARM (32-bit) code.
#include <nitro/code32.h>

typedef struct HostEnt {
    char *h_name;
    char **h_aliases;
    s16 h_addrtype;
    s16 h_length;
    u8 **h_addr_list;
} HostEnt;

typedef struct SocketAddress {
    u8 len;
    u8 family;
    u16 port; // network byte order
    u32 ip;
} SocketAddress;

// "2147483647": longest accepted Content-Length
typedef struct GHIMaxLengthString {
    char chars[11];
} GHIMaxLengthString;

HostEnt *SOC_GetHostByName(const char *name);
int ov00_021FF8A0(int socket, const SocketAddress *address, int len);

extern char *ov38_02226028; // proxy server address
extern u16 _02226024;       // proxy server port
extern int ov38_02225320;   // socket receive buffer size while throttled
extern const GHIMaxLengthString ov38_02224E6C;
extern char ov38_02225AD4[];
extern char ov38_02225AE0[];
extern char ov38_02225AF0[];
extern char ov38_02225B00[];
extern char ov38_02225B14[];
extern char ov38_02225B2C[];
extern char ov38_02225B48[];
extern char ov38_02225B68[];
extern char ov38_02225B74[];
extern char ov38_02225B84[];
extern char ov38_02225B94[];
extern char ov38_02225B9C[];
extern char ov38_02225BA8[];
extern char ov38_02225BAC[];
extern char ov38_02225BB0[];
extern char ov38_02225BB4[];
extern char ov38_02225BBC[];
extern char ov38_02225BC4[];
extern char ov38_02225BCC[];
extern char ov38_02225BD8[];
extern char ov38_02225BE0[];
extern char ov38_02225BE8[];
extern char ov38_02225BEC[];
extern char ov38_02225BF8[];
extern char ov38_02225C08[];
extern char ov38_02225C14[];
extern char ov38_02225C20[];
extern char ov38_02225C28[];
extern char ov38_02225C2C[];
extern char ov38_02225C3C[];
extern char ov38_02225C4C[];
extern char ov38_02225C78[];
extern char ov38_02225C98[];
extern char ov38_02225CA8[];
extern char ov38_02225CAC[];
extern char ov38_02225CB0[];
extern char ov38_02225CB8[];
extern char ov38_02225CC4[];
extern char ov38_02225CCC[];
extern char ov38_02225CD4[];
extern char ov38_02225CD8[];
extern char ov38_02225CE4[];
extern char ov38_02225CF4[];
extern char ov38_02225D04[];

static inline int IsStatusSpace(int ch) {
    int invalid = 1;
    if (ch >= 0 && ch < 128) {
        invalid = 0;
    }
    return invalid ? 0 : __msl_digit[ch] & 0x100;
}

static inline int IsHeaderSpace(int ch) {
    return (ch < 0 || ch >= 128) ? 0 : __msl_digit[ch] & 0x100;
}

int ghiParseURL(GHIConnection *c) {
    char *p;
    char saved;
    size_t n;
    int secure;
    char *path;
    char *space;
    HTTP_ASSERT(c, ov38_02225B68, ov38_02225B74, ov38_02225AD4, 37);
    if (!c) {
        return 0;
    }
    HTTP_ASSERT(c->URL, ov38_02225B84, ov38_02225B74, ov38_02225AD4, 43);
    p = c->URL;
    if (!p) {
        return 0;
    }
    if (strncmp(p, ov38_02225B94, 7) == 0) {
        p += 7;
        secure = 0;
    } else if (strncmp(p, ov38_02225B9C, 8) == 0) {
        p += 8;
        secure = 1;
    } else {
        return 0;
    }
    n = strspn(p, ov38_02225BA8);
    saved = p[n];
    p[n] = '\0';
    c->serverAddress = ov00_021FF3D4(p);
    if (!c->serverAddress) {
        return 0;
    }
    p[n] = saved;
    p += n;
    if (*p == ':') {
        p++;
        c->serverPort = atol(p);
        if (c->serverPort == 0) {
            return 0;
        }
        while (*++p && *p != '/') {
        }
    } else {
        if (secure) {
            c->serverPort = 443;
        } else {
            c->serverPort = 80;
        }
    }
    if (!*p) {
        p = ov38_02225BAC;
    }
    path = c->requestPath = ov00_021FF3D4(p);
    space = strchr(path, ' ');
    while (space) {
        *space = '+';
        path = c->requestPath;
        space = strchr(path, ' ');
    }
    if (!path) {
        return 0;
    }
    return 1;
}

void ov38_0221EBB0(GHIConnection *c) {
    char *server;
    ghiCallProgressCallback(c, NULL, 0);
    ov00_021FF3CC();
    if (!ghiParseURL(c)) {
        c->completed = 1;
        c->result = 3;
        return;
    }
    if (c->proxyAddress != NULL) {
        server = c->proxyAddress;
    } else if (ov38_02226028 != NULL) {
        server = ov38_02226028;
    } else {
        server = c->serverAddress;
    }
    c->serverIP = gsSocketInetAddr(server);
    if (c->serverIP == -1) {
        HostEnt *he = SOC_GetHostByName(server);
        if (he == NULL) {
            c->completed = 1;
            c->result = 4;
            return;
        }
        c->serverIP = *(int *)he->h_addr_list[0];
    }
    c->state = GHI_STATE_CONNECTING;
    ghiCallProgressCallback(c, NULL, 0);
}

#pragma push
#pragma opt_propagation off

void ov38_0221EC74(GHIConnection *c) {
    SocketAddress address;
    int writable;
    int except;
    BOOL result;
    if (c->socket == -1) {
        c->socket = ov00_021FF804(2, 1, 0);
        if (c->socket == -1) {
            c->completed = 1;
            c->result = 5;
            c->socketError = gsSocketGetLastError(c->socket);
            return;
        }
        if (!ov00_021FF44C(c->socket, 0)) {
            c->completed = 1;
            c->result = 5;
            c->socketError = gsSocketGetLastError(c->socket);
            return;
        }
        if (c->throttle) {
            ov00_021FF48C(c->socket, ov38_02225320);
        }
        {
            SocketAddress *a = &address;
            *(u32 *)a = 0;
            *(u32 *)((u8 *)a + 4) = 0;
        }
        address.family = 2;
        if (c->proxyAddress) {
            int port = c->proxyPort;
            address.port = ((port >> 8) & 0xff) | ((port << 8) & 0xff00);
        } else if (ov38_02226028) {
            int port = _02226024;
            address.port = ((port >> 8) & 0xff) | ((port << 8) & 0xff00);
        } else {
            int port = c->serverPort;
            address.port = ((port >> 8) & 0xff) | ((port << 8) & 0xff00);
        }
        address.ip = c->serverIP;
        if (ov00_021FF8A0(c->socket, &address, 8) == -1) {
            int error = gsSocketGetLastError(c->socket);
            if (error != -6 && error != -26 && error != -76) {
                c->completed = 1;
                c->result = 6;
                c->socketError = error;
                return;
            }
        }
    }
    result = ov00_021FF59C(c->socket, NULL, &writable, &except) > 0;
    if (result == -1 || except) {
        c->completed = 1;
        c->result = 6;
        if (result == 0) {
            c->socketError = gsSocketGetLastError(c->socket);
        }
        return;
    }
    if (writable) {
        c->state = GHI_STATE_SECURING_SESSION;
        ghiCallProgressCallback(c, NULL, 0);
    }
}

#pragma pop

void ghiDoSecuringSession(GHIConnection *c) {
    char buf[1024];
    int len;
    if (!c->encryptor.engine) {
        // https without an encryption engine
        if (!strncmp(c->URL, ov38_02225B9C, 8)) {
            c->completed = 1;
            c->result = 17;
            return;
        }
        c->state = GHI_STATE_SENDING_REQUEST;
        ghiCallProgressCallback(c, NULL, 0);
        return;
    }
    if (c->encryptor.sessionReady) {
        c->state = GHI_STATE_SENDING_REQUEST;
        ghiCallProgressCallback(c, NULL, 0);
        return;
    }
    if (!c->encryptor.initialized) {
        if (c->encryptor.initFunc(c, &c->encryptor) == 3) {
            __msl_assertion_failed(ov38_02225BB0, ov38_02225B74, ov38_02225B14, 0x146);
            return;
        }
    }
    if (c->sendBuffer.pos < c->sendBuffer.len) {
        if (!ov38_0221C748(c)) {
            return;
        }
        if (c->sendBuffer.pos < c->sendBuffer.len) {
            return;
        }
        ghiResetBuffer(&c->sendBuffer);
    }
    len = 1024;
    ov38_0221CB7C(c, buf, &len);
}

void ov38_0221F024(GHIConnection *c) {
    GHIBuffer *buffer;
    char text[16];
    if (c->sendBuffer.len == 0) {
        buffer = &c->sendBuffer;
        ghiAppendDataToBuffer(buffer, c->post ? ov38_02225BB4 : c->type == 3 ? ov38_02225BBC
                                                                             : ov38_02225BC4,
            0);
        if (c->proxyAddress || ov38_02226028) {
            // through a proxy the request line carries the absolute URL
            ghiAppendDataToBuffer(buffer, c->URL, 0);
        } else {
            ghiAppendDataToBuffer(buffer, c->requestPath, 0);
        }
        ghiAppendDataToBuffer(buffer, ov38_02225BCC, 0);
        if (c->serverPort == 80) {
            ov38_0221C5EC(buffer, ov38_02225BD8, c->serverAddress);
        } else {
            ghiAppendDataToBuffer(buffer, ov38_02225BE0, 0);
            ghiAppendDataToBuffer(buffer, c->serverAddress, 0);
            ghiAppendCharToBuffer(buffer, ':');
            ov38_0221C6CC(buffer, c->serverPort);
            ghiAppendDataToBuffer(buffer, ov38_02225BE8, 2);
        }
        if (c->sendHeaders == NULL || strstr(c->sendHeaders, ov38_02225BEC) == NULL) {
            ov38_0221C5EC(buffer, ov38_02225BEC, ov38_02225BF8);
        }
        if (c->keepAlive) {
            ov38_0221C5EC(buffer, ov38_02225C08, ov38_02225C14);
        } else {
            ov38_0221C5EC(buffer, ov38_02225C08, ov38_02225C20);
        }
        if (c->post) {
            sprintf(text, ov38_02225C28, c->totalBytes);
            ov38_0221C5EC(buffer, ov38_02225C2C, text);
            ov38_0221C5EC(buffer, ov38_02225C3C, ghiPostGetContentType(c));
        }
        if (c->sendHeaders) {
            ghiAppendDataToBuffer(buffer, c->sendHeaders, 0);
        }
        ghiAppendDataToBuffer(buffer, ov38_02225BE8, 2);
        if (buffer != &c->sendBuffer) {
            ghiAppendDataToBuffer(&c->sendBuffer, buffer->data, buffer->len);
        }
    }
    if (!ov38_0221C748(c)) {
        return;
    }
    if (c->sendBuffer.pos < c->sendBuffer.len) {
        return;
    }
    ghiResetBuffer(&c->sendBuffer);
    if (c->post) {
        c->state = GHI_STATE_POSTING;
    } else {
        c->state = GHI_STATE_WAITING;
    }
    ghiCallProgressCallback(c, NULL, 0);
}

void ghiDoPosting(GHIConnection *c) {
    int result;
    int bytesPosted = c->bytesPosted;
    result = ghiPostDoPosting(c);
    if (result == 0) {
        HTTP_ASSERT(c->completed && c->result, ov38_02225C4C, ov38_02225B74, ov38_02225AE0, 0x200);
        ov38_0221E0DC(c);
        return;
    }
    if (bytesPosted != c->bytesPosted) {
        ghiCallPostCallback(c);
    }
    if (result != 1) {
        return;
    }
    ov38_0221E0DC(c);
    c->state = GHI_STATE_WAITING;
    ghiCallProgressCallback(c, NULL, 0);
}

void ov38_0221F330(GHIConnection *c) {
    int readable;
    if (ov00_021FF59C(c->socket, &readable, NULL, NULL) == -1) {
        c->completed = 1;
        c->result = 5;
        c->socketError = gsSocketGetLastError(c->socket);
        return;
    }
    if (readable == 0) {
        return;
    }
    c->state = GHI_STATE_RECEIVING_STATUS;
    ghiCallProgressCallback(c, NULL, 0);
}

int ghiParseStatus(GHIConnection *c) {
    int major;
    int minor;
    int status;
    int end;
    int count;
    int i;
    HTTP_ASSERT(c, ov38_02225B68, ov38_02225B74, ov38_02225AF0, 590);
    HTTP_ASSERT(c->encodeBuffer.len > 0, ov38_02225C78, ov38_02225B74, ov38_02225AF0, 591);
    count = sscanf(c->encodeBuffer.data, ov38_02225C98, &major, &minor, &status, &end);
    i = end;
    while (c->encodeBuffer.data[i] && IsStatusSpace((signed char)c->encodeBuffer.data[i])) {
        i++;
        end = i;
    }
    if (count != 3 || major < 1 || status < 100 || status >= 600) {
        c->completed = 1;
        c->result = 7;
        return 0;
    }
    c->statusMajor = major;
    c->statusMinor = minor;
    c->statusCode = status;
    c->statusStringIndex = end;
    return 1;
}

void ov38_0221F4FC(GHIConnection *c) {
    char buf[1024];
    char *end;
    int len = 1024;
    int result = ov38_0221CB7C(c, buf, &len);
    if (result == 3) {
        return;
    }
    if (result == 1 && c->encodeBuffer.pos == c->encodeBuffer.len) {
        return;
    }
    if (result == 0) {
        if (!ghiAppendDataToBuffer(&c->encodeBuffer, buf, len)) {
            return;
        }
    }
    end = strstr(c->encodeBuffer.data, ov38_02225BE8);
    if (end) {
        *end = '\0';
        result = end - c->encodeBuffer.data;
        c->headerLength = result + 1;
        if (!ghiParseStatus(c)) {
            return;
        }
        c->encodeBuffer.pos = result + 2;
        c->state = GHI_STATE_RECEIVING_HEADERS;
        ghiCallProgressCallback(c, NULL, 0);
        return;
    }
    if (result != 2) {
        return;
    }
    c->completed = 1;
    c->result = 7;
    c->socketError = gsSocketGetLastError(c->socket);
}

int ov38_0221F624(GHIConnection *c, const char *data, int len) {
    const char *outData = NULL;
    int outLen = 0;
    c->fileBytesReceived += len;
    if (c->fileBytesReceived == c->totalSize || c->connectionClosed) {
        c->completed = 1;
    }
    if (c->type == 0) {
        if (!ghiAppendDataToBuffer(&c->getFileBuffer, data, len)) {
            return 0;
        }
        outData = c->getFileBuffer.data;
        outLen = c->getFileBuffer.len;
    } else if (c->type == 1) {
        if (len != 0) {
            c->completed = 1;
            c->result = 13;
            return 0;
        }
        outData = data;
        outLen = len;
    } else if (c->type == 2) {
        outData = data;
        outLen = len;
    }
    ghiCallProgressCallback(c, outData, outLen);
    return 1;
}

int ghiParseChunkSize(GHIConnection *c) {
    int size;
    HTTP_ASSERT(c->chunkHeaderLen, ov38_02225CA8, ov38_02225B74, ov38_02225B00, 0x321);
    if (sscanf(c->chunkHeader, ov38_02225CAC, &size) != 1) {
        return -1;
    }
    return size;
}

void ghiAppendToChunkHeaderBuffer(GHIConnection *c, const char *data, int len) {
    int n;
    HTTP_ASSERT(c, ov38_02225B68, ov38_02225B74, ov38_02225B48, 0x333);
    HTTP_ASSERT(data, ov38_02225CB0, ov38_02225B74, ov38_02225B48, 0x334);
    HTTP_ASSERT(len >= 0, ov38_02225CB8, ov38_02225B74, ov38_02225B48, 0x335);
    if (!len) {
        return;
    }
    if (c->chunkHeaderLen >= 10) {
        return;
    }
    n = 10 - c->chunkHeaderLen;
    if (n >= len) {
        n = len;
    }
    memcpy(c->chunkHeader + c->chunkHeaderLen, data, n);
    c->chunkHeaderLen += n;
    c->chunkHeader[c->chunkHeaderLen] = '\0';
}

int ghiProcessIncomingFileData(GHIConnection *c, char *data, int len) {
    char *lineEnd;
    int amount;
    HTTP_ASSERT(c, ov38_02225B68, ov38_02225B74, ov38_02225B2C, 0x35d);
    HTTP_ASSERT(data, ov38_02225CB0, ov38_02225B74, ov38_02225B2C, 0x35e);
    HTTP_ASSERT(len > 0, ov38_02225CC4, ov38_02225B74, ov38_02225B2C, 0x35f);
    if (c->chunked) {
        while (len > 0) {
            if (c->chunkReadingState == GHI_CHUNK_HEADER) {
                lineEnd = strchr(data, '\n');
                if (lineEnd) {
                    ghiAppendToChunkHeaderBuffer(c, data, lineEnd - data);
                    amount = lineEnd + 1 - data;
                    len -= amount;
                    data = lineEnd + 1;
                    c->chunkBytesLeft = ghiParseChunkSize(c);
                    if (c->chunkBytesLeft == -1) {
                        c->completed = 1;
                        c->result = 7;
                        return 0;
                    }
                    if (c->chunkBytesLeft == 0) {
                        c->chunkReadingState = GHI_CHUNK_DONE;
                    } else {
                        c->chunkReadingState = GHI_CHUNK_DATA;
                    }
                } else {
                    ghiAppendToChunkHeaderBuffer(c, data, len);
                    return 1;
                }
            } else if (c->chunkReadingState == GHI_CHUNK_DATA) {
                amount = c->chunkBytesLeft;
                if (amount >= len) {
                    amount = len;
                }
                if (!ov38_0221F624(c, data, amount)) {
                    return 0;
                }
                c->chunkBytesLeft -= amount;
                data += amount;
                len -= amount;
                if (c->chunkBytesLeft == 0) {
                    c->chunkReadingState = GHI_CHUNK_END;
                }
            } else if (c->chunkReadingState == GHI_CHUNK_END) {
                lineEnd = strchr(data, '\n');
                if (!lineEnd) {
                    return 1;
                }
                c->chunkHeader[0] = '\0';
                c->chunkHeaderLen = 0;
                c->chunkBytesLeft = 0;
                c->chunkReadingState = GHI_CHUNK_HEADER;
                amount = lineEnd + 1 - data;
                data = lineEnd + 1;
                len -= amount;
            } else if (c->chunkReadingState == GHI_CHUNK_DONE) {
                c->completed = 1;
                return 1;
            } else {
                __msl_assertion_failed(ov38_02225BB0, ov38_02225B74, ov38_02225B2C, 0x3e3);
                return 0;
            }
        }
        return 1;
    }
    return ov38_0221F624(c, data, len);
}

void ov38_0221FA38(GHIConnection *c) {
    char *receive;
    int size;
    int status;
    char *base;
    int position;
    char *p;
    char *body;
    int remaining;
    int group;
    char *lengthHeader;
    GHIMaxLengthString maximum;
    char *value;
    char *end;
    int n;
    receive = ov00_021EC3A8(4096);
    if (!receive) {
        c->completed = 1;
        c->result = 1;
        return;
    }
    size = 4096;
    status = ov38_0221CB7C(c, receive, &size);
    if (status == 3 || (status == 1 && c->encodeBuffer.pos == c->encodeBuffer.len)) {
        goto finish;
    }
    if (status == 0 && !ghiAppendDataToBuffer(&c->encodeBuffer, receive, size)) {
        goto finish;
    }
    position = c->encodeBuffer.pos;
    base = c->encodeBuffer.data;
    c->headerIndex = position;
    p = strstr(base + position, ov38_02225CCC);
    if (!p) {
        p = strstr(base + position, ov38_02225CD4);
    }
    if (p) {
        int oldLength;
        char *headerBase;
        char *headerEnd = p + 2;
        headerEnd[0] = '\0';
        headerBase = c->encodeBuffer.data;
        oldLength = c->encodeBuffer.len;
        c->encodeBuffer.len = headerEnd - headerBase;
        body = p + 4;
        remaining = oldLength - (body - headerBase);
        c->headerLength = headerEnd - c->encodeBuffer.data;
        c->encodeBuffer.pos = c->headerLength;
        group = c->statusCode / 100;
        if (group == 1) {
            if (remaining) {
                memmove(c->encodeBuffer.data, body, remaining + 1);
                c->encodeBuffer.len = remaining;
                c->encodeBuffer.pos = 0;
            } else {
                ghiResetBuffer(&c->encodeBuffer);
            }
            c->state = GHI_STATE_RECEIVING_STATUS;
            ghiCallProgressCallback(c, NULL, 0);
            goto finish;
        }
        if (group == 3) {
            if (c->redirectCount > 10) {
                c->completed = 1;
                c->result = 11;
                goto finish;
            }
            p = strstr(base + position, ov38_02225CD8);
            if (p) {
                value = p + 9;
                while (IsHeaderSpace(*value)) {
                    ++value;
                }
                end = value;
                while (*end && !IsHeaderSpace(*end)) {
                    ++end;
                }
                *end = '\0';
                if (*value == '/') {
                    n = strlen(c->serverAddress);
                    n = (n + 14) + strlen(value);
                    c->redirectURL = ov00_021EC3A8(n);
                    if (!c->redirectURL) {
                        c->completed = 1;
                        c->result = 1;
                    }
                    sprintf(c->redirectURL, ov38_02225CE4, c->serverAddress, c->serverPort, value);
                } else {
                    c->redirectURL = ov00_021FF3D4(value);
                    if (!c->redirectURL) {
                        c->completed = 1;
                        c->result = 1;
                    }
                }
                goto finish;
            }
        }
        lengthHeader = strstr(base + position, ov38_02225CF4);
        if (lengthHeader) {
            maximum = ov38_02224E6C;
            value = lengthHeader + 16;
            end = value;
            n = strlen(maximum.chars);
            while (end && *end && *end != '\n' && *end != '\r' && *end != ' ') {
                ++end;
            }
            {
                int tokenLength = end - value;
                if (tokenLength > n) {
                    c->completed = 1;
                    c->result = 16;
                    goto finish;
                }
                if (n == tokenLength && strncmp(value, maximum.chars, tokenLength) >= 0) {
                    c->completed = 1;
                    c->result = 16;
                    goto finish;
                }
            }
            c->totalSize = atol(value);
        }
        c->chunked = strstr(base + position, ov38_02225D04) != 0;
        if (c->chunked) {
            c->chunkHeader[0] = '\0';
            c->chunkHeaderLen = 0;
            c->chunkBytesLeft = 0;
            c->chunkReadingState = GHI_CHUNK_HEADER;
        }
        if (c->type == 3 || c->type == 4) {
            c->completed = 1;
            goto finish;
        }
        c->state = GHI_STATE_RECEIVING_FILE;
        if (lengthHeader && c->totalSize == 0) {
            c->completed = 1;
            goto finish;
        }
        if (remaining > 0) {
            ghiProcessIncomingFileData(c, body, remaining);
        }
    } else if (status == 2) {
        c->completed = 1;
        c->result = 7;
        c->socketError = gsSocketGetLastError(c->socket);
    }
finish:
    ov00_021EC3D8(receive);
}

void ov38_0221FEA8(GHIConnection *c) {
    void *buffer;
    u32 start = ov00_021FF368();
    u32 elapsed = 0;
    int length;
    int result;
    buffer = ov00_021EC3A8(8192);
    if (!buffer) {
        c->completed = 1;
        c->result = 1;
        return;
    }
    while (!c->completed && elapsed < c->maxRecvTime) {
        length = 8192;
        result = ov38_0221CB7C(c, buffer, &length);
        if (result == 3 || result == 1) {
            break;
        }
        if (result == 2) {
            c->completed = 1;
            if (c->totalSize > 0 && c->fileBytesReceived < c->totalSize) {
                c->result = 15;
            }
            break;
        }
        if (!ghiProcessIncomingFileData(c, buffer, length)) {
            break;
        }
        elapsed = ov00_021FF368() - start;
    }
    ov00_021EC3D8(buffer);
}
