#pragma force_active on
#include <string.h>

#include "global.h"

#include "dwc/auth_internal.h"
#include "dwc/net_common_internal.h"

// The code below is compiled as ARM (32-bit) code.
#include <nitro/code32.h>

extern int _021D4670;
extern int ov00_0221A6B4;
extern const u8 ov00_02216650[]; // root CA
extern char ov00_022166B0[];
extern char ov00_022166BC[];
extern char ov00_022166C4[];
extern char ov00_022166CC[];
extern char ov00_022166D0[];
extern char ov00_022166E0[];
extern char ov00_022166F4[];
extern char ov00_022166F8[];
extern char ov00_02216710[];
extern char ov00_02216728[];
extern char ov00_0221677C[];
extern char ov00_0221679C[];
extern char ov00_022167A8[];
extern char ov00_022167AC[];
extern char ov00_022167B4[];
extern char ov00_022167B8[];
extern char ov00_022167CC[];
extern char ov00_022167E0[];
extern char ov00_022167F0[];
extern char ov00_022167F8[];
extern char ov00_02216804[];
extern char ov00_02216808[];
extern char ov00_0221680C[];
extern char ov00_0221682C[];
extern char ov00_0221684C[];
extern char ov00_02216868[];
extern char ov00_02216884[];
extern char ov00_02216888[];
extern char ov00_02216894[];
extern char ov00_02216898[];
extern char ov00_0221689C[];
extern char ov00_022168A0[];

extern long atol(const char *);
extern int strncmp(const char *a, const char *b, u32 n);
extern int CPS_Resolve(char *host);
extern void CPS_SocRegister(NasHttpSocket *socket);
extern void CPS_SslAddRandomSeed(void *, u32);
extern void CPS_SocUse(void);
extern void CPS_SocRelease(void);
extern void CPS_SocUnRegister(void);
extern void CPS_SocFlush(void);
extern void CPS_TcpShutdown(void);
extern void CPS_TcpClose(void);
extern void CPS_SetRootCa(const void *, int);
extern void CPS_SetSsl(int);
extern void CPS_SocBind(int, u16, u32);
extern int CPS_TcpConnect(void);
extern int CPS_SocWrite(const void *, u32);
extern int CPS_SocGetLength(void);
extern void *CPS_SocRead(int *);
extern void CPS_SocConsume(u32);

// Adds "Connection: close" and the Content-Length of the POST body.
int ov00_021E90BC(NasHttp *http) {
    char tmp[8];
    int n;
    if (ov00_021E98C8(http, ov00_022166B0, ov00_022166BC)) {
        return 1;
    }
    n = strlen(strstr(http->request.buffer, ov00_022166C4) + 4);
    if (n) {
        OS_SNPrintf(tmp, 7, ov00_022166CC, n);
        if (ov00_021E98C8(http, ov00_022166D0, tmp)) {
            return 1;
        }
    }
    return 0;
}

// Starts the receive thread.
void ov00_021E9154(NasHttp *http, int priority) {
    http->stop = 0;
    OS_InitMutex(&http->stopMutex);
    OS_InitMutex(&http->recvMutex);
    if (http->params.testServer == 1) {
        ov00_0221A6B4 = 1;
    } else {
        ov00_0221A6B4 = 0;
    }
    if (http->thread.id && !OS_IsThreadTerminated(&http->thread)) {
        return;
    }
    OS_CreateThread(&http->thread, (void (*)(void *))ov00_021E9438, http, http->threadStack + 0x1000, 4096, priority);
    OS_WakeupThreadDirect(&http->thread);
}

// Stops the receive thread and waits for it.
void ov00_021E9210(NasHttp *http) {
    if (http->status != 255) {
        return;
    }
    OS_LockMutex(&http->stopMutex);
    http->stop = 1;
    OS_UnlockMutex(&http->stopMutex);
    if (http->thread.id) {
        OS_JoinThread(&http->thread);
    }
}

// Returns 0 once a stop was requested; otherwise sleeps 10 ms.
int ov00_021E926C(NasHttp *http) {
    if (http->secure == 1) {
        OS_GetLowEntropyData((u32 *)http->seed);
        CPS_SslAddRandomSeed(http->seed, 32);
    }
    OS_LockMutex(&http->stopMutex);
    if (http->stop == 1) {
        OS_UnlockMutex(&http->stopMutex);
        return 0;
    }
    OS_UnlockMutex(&http->stopMutex);
    OS_Sleep(10);
    return 1;
}

void ov00_021E92E4(NasHttp *http) {
    MI_CpuFill8(&http->socket, 0, 100);
    http->socket.recvBufSize = 2920;
    http->socket.recvBuf = http->lowRecvBuf;
    http->socket.sendBufSize = 1514;
    http->socket.sendBuf = http->lowSendBuf;
    CPS_SocRegister(&http->socket);
}

int ov00_021E933C(NasHttp *http) {
    return CPS_Resolve(http->host);
}

// Once the header is complete: counts the body bytes received so far and reads Content-Length.
int ov00_021E9350(NasHttp *http) {
    char *s = http->response.buffer, *header, *tail;
    char saved;
    int n;
    char **buffer = &http->response.buffer;
    if (!strstr(s, ov00_022166C4)) {
        return 0;
    }
    s = strstr(s, ov00_022166C4) + 4;
    OS_LockMutex(&http->recvMutex);
    http->received = buffer[1] - s;
    OS_UnlockMutex(&http->recvMutex);
    header = strstr(buffer[0], ov00_022166E0);
    if (!header) {
        return 1;
    }
    n = strlen(ov00_022166E0);
    tail = strstr(header + n, ov00_022166F4);
    saved = *tail;
    *tail = 0;
    OS_LockMutex(&http->recvMutex);
    http->contentLength = atol(header + n);
    OS_UnlockMutex(&http->recvMutex);
    *tail = saved;
    return 1;
}

// Receive thread: resolves, connects (SSL for https), sends the request and reads the response.
void ov00_021E9438(NasHttp *http) {
    NasHttpSocket *soc = &http->socket;
    NasHttpSsl *ssl = &http->ssl;
    NasHttpBuffer *b = &http->response;
    int timeout = http->params.timeout;
    int parsed = 0;
    u32 ip;
    int length;
    u64 last;
    if (timeout <= 0) {
        timeout = 60000;
    }
    ov00_021E92E4(http);
    ip = ov00_021E933C(http);
    if (!ip) {
        http->result = 2;
        return;
    }
    http->ip = ip;
    CPS_SocUse();
    if (http->secure == 1) {
        MI_CpuFill8(ssl, 0, 0x830);
        ssl->authCallback = ov00_021E9B38;
        ssl->serverName = http->host;
        soc->ssl = ssl;
        CPS_SetRootCa(ov00_02216650, 12);
        CPS_SetSsl(1);
    }
    CPS_SocBind(0, http->port, ip);
    if (CPS_TcpConnect() != 0) {
        http->result = 3;
        CPS_SocRelease();
        CPS_SocUnRegister();
        return;
    }
    {
        char *request = http->request.buffer;
        length = CPS_SocWrite(request, strlen(request));
    }
    if (length <= 0) {
        http->result = 5;
        goto fail;
    }
    CPS_SocFlush();
    if (!ov00_021E926C(http)) {
        http->result = 7;
        goto fail;
    }
    b->pos = b->buffer;
    b->end = b->buffer + b->size;
    last = OS_GetTick();
    for (;;) {
        int take;
        void *data;
        if (!_021D4670) {
            http->result = 5;
            goto fail;
        }
        length = CPS_SocGetLength();
        if (length < 0) {
            break;
        }
        if (length > 0) {
            last = OS_GetTick();
            data = CPS_SocRead(&length);
            if (!data) {
                break;
            }
            take = length;
            if (take >= b->end - 1 - b->pos) {
                take = b->end - 1 - b->pos;
            }
            MI_CpuCopy8(data, b->pos, take);
            b->pos += take;
            *b->pos = 0;
            if (parsed == 1) {
                OS_LockMutex(&http->recvMutex);
                http->received += take;
                OS_UnlockMutex(&http->recvMutex);
            } else {
                parsed = ov00_021E9350(http);
            }
            if ((u32)length > (u32)take) {
                CPS_SocConsume(length);
                break;
            }
            CPS_SocConsume(take);
        }
        if (http->contentLength >= 0 && http->received >= http->contentLength) {
            break;
        }
        if (((OS_GetTick() - last) << 6) / 33514 > (u64)(s64)timeout) {
            http->result = 6;
            goto fail;
        }
        if (!ov00_021E926C(http)) {
            http->result = 7;
            goto fail;
        }
    }
    CPS_TcpShutdown();
    CPS_TcpClose();
    CPS_SocRelease();
    CPS_SocUnRegister();
    http->result = 8;
    return;
fail:
    CPS_TcpShutdown();
    CPS_TcpClose();
    CPS_SocRelease();
    CPS_SocUnRegister();
}

// Frees everything the connection allocated and clears it.
void ov00_021E9754(NasHttp *http) {
    NasFreeFunc free = http->params.free;
    if (http == NULL) {
        return;
    }
    ov00_021EA2B8(http, http->entry, 32);
    ov00_021E9B9C(http, &http->response);
    ov00_021E9B9C(http, &http->request);
    if (http->lowRecvBuf) {
        free(ov00_022166F8, http->lowRecvBuf, 0);
        http->lowRecvBuf = NULL;
    }
    if (http->lowSendBuf) {
        free(ov00_02216710, http->lowSendBuf, 0);
        http->lowSendBuf = NULL;
    }
    MI_CpuFill8(http, 0, 0x1c14);
}

// Writes the request line: "POST /path HTTP/1.0" (method 0) or "GET /path HTTP/1.0".
int ov00_021E9808(NasHttp *http) {
    NasHttpBuffer *b = &http->request;
    char *format = http->params.method == 0 ? ov00_02216728 : ov00_0221677C;
    int h = strlen(http->host), f = strlen(format), p = strlen(http->path);
    h += f - 4 + p;
    if (ov00_021E9B44(http, &http->request, h + 0x400) != 1) {
        return 1;
    }
    b->pos += OS_SNPrintf(b->pos, b->size, format, http->path, http->host);
    return 0;
}

// Inserts the header line "label: value" in front of the blank line ending the header.
int ov00_021E98C8(NasHttp *http, const char *label, const char *value) {
    NasHttpBuffer *b = &http->request;
    int n = strlen(value), f = strlen(ov00_0221679C), l = strlen(label);
    char saved;
    char *p;
    int written;
    n += (f - 4) + l;
    if (n + 1 > b->end - b->pos) {
        if (!ov00_021E9BDC(http, b, n - (b->end - b->pos) + 1)) {
            return 1;
        }
    }
    p = strstr(b->buffer, ov00_022166C4) + 2;
    saved = *p;
    memmove(p + n, p, strlen(p) + 1);
    written = OS_SNPrintf(p, n + 1, ov00_0221679C, label, value);
    p[written] = saved;
    b->pos += n;
    return 0;
}

// Appends the POST field "label=value" ("&label=value" after the first), value encoded by ov00_021EBE08.
int ov00_021E99A4(NasHttp *http, const char *label, const void *value, int len) {
    int n, f, l, total, remain;
    NasHttpBuffer *b = &http->request;
    char *format = http->postFieldCount == 0 ? ov00_022167A8 : ov00_022167AC;
    http->postFieldCount++;
    n = ov00_021EBE08(value, len, 0, 0);
    f = strlen(format);
    l = strlen(label);
    total = (f - 2) + l + n;
    remain = b->end - b->pos;
    if (total > remain) {
        if (!ov00_021E9BDC(http, b, total - remain + 1)) {
            return 1;
        }
        remain = b->end - b->pos;
    }
    b->pos += OS_SNPrintf(b->pos, remain, format, label);
    if (ov00_021EBE08(value, len, b->pos, b->end - b->pos - 1) < 0) {
        return 1;
    }
    b->pos += n;
    *b->pos = 0;
    return 0;
}

int ov00_021E9AB0(NasHttp *http, const char *str) {
    int n;
    NasHttpBuffer *b = &http->request;
    n = strlen(str);
    int avail = b->end - b->pos;
    int result;
    if (n > avail) {
        if (!ov00_021E9BDC(http, b, n - avail + 1)) {
            return 1;
        }
        avail = b->end - b->pos;
    }
    result = OS_SNPrintf(b->pos, avail, ov00_022167B4, str);
    if (result != n) {
        return 1;
    }
    b->pos += result;
    return 0;
}

// SSL certificate check callback: ignores the 0x8000 flag.
int ov00_021E9B38(int code) {
    if (code & 0x8000) {
        code &= ~0x8000;
    }
    return code;
}

int ov00_021E9B44(NasHttp *http, NasHttpBuffer *buf, u32 size) {
    NasAllocFunc alloc = http->params.alloc;
    if (size == 0) {
        return 0;
    }
    buf->buffer = alloc(ov00_022167B8, size);
    if (buf->buffer == NULL) {
        return 0;
    }
    buf->pos = buf->buffer;
    buf->size = size;
    buf->end = buf->buffer + buf->size;
    return 1;
}

void ov00_021E9B9C(NasHttp *http, NasHttpBuffer *buf) {
    NasFreeFunc f = http->params.free;
    if (buf->buffer) {
        f(ov00_022167CC, buf->buffer, 0);
    }
    MI_CpuFill8(buf, 0, 16);
}

// Grows buf by grow bytes.
int ov00_021E9BDC(NasHttp *http, NasHttpBuffer *buf, int grow) {
    NasFreeFunc f = http->params.free;
    NasAllocFunc a = http->params.alloc;
    char *s;
    if (grow <= 0) {
        return 0;
    }
    s = a(ov00_022167E0, buf->size + grow);
    if (!s) {
        return 0;
    }
    MI_CpuCopy8(buf->buffer, s, buf->size);
    f(ov00_022167CC, buf->buffer, 0);
    if (!s) {
        return 0;
    }
    buf->pos = buf->pos + (s - buf->buffer);
    buf->size += grow;
    buf->buffer = s;
    buf->end = s + buf->size;
    return 1;
}

// Splits url ("http://" or "https://", host, optional ":port", "/path") into the connection.
int ov00_021E9C84(NasHttp *http, const char *url) {
    char *port = 0, *p;
    if (strlen(url) >= 256) {
        return 0;
    }
    strncpy(http->url, url, 256);
    if (strlen(url) != strlen(http->url)) {
        return 0;
    }
    if (strstr(http->url, ov00_022167F0)) {
        http->host = http->url + 7;
        http->secure = 0;
        http->port = 80;
    } else {
        p = strstr(http->url, ov00_022167F8);
        if (!p) {
            return 0;
        }
        http->host = p + 8;
        http->secure = 1;
        http->port = 443;
    }
    p = strstr(http->host, ov00_02216804);
    if (p) {
        *p = 0;
        port = p + 1;
    }
    p = strstr(http->host, ov00_02216808);
    if (!p) {
        http->path = 0;
    } else {
        *p = 0;
        http->path = p + 1;
    }
    if (port) {
        http->port = atol(port);
    }
    return 1;
}

int ov00_021E9DD8(NasHttp *http, NasHttpEntryList *list, char *label, char *value) {
    NasAllocFunc alloc = http->params.alloc;
    NasFreeFunc free = http->params.free;
    if (list->count > list->max) {
        return 0;
    }
    list->entry[list->count].label = alloc(ov00_0221680C, strlen(label) + 1);
    if (list->entry[list->count].label) {
        list->entry[list->count].value = alloc(ov00_0221682C, strlen(value) + 1);
        if (list->entry[list->count].value) {
            strcpy(list->entry[list->count].label, label);
            strcpy(list->entry[list->count].value, value);
            list->count++;
            return 1;
        }
    }
    if (list->entry[list->count].label) {
        free(ov00_0221684C, list->entry[list->count].label, 0);
        list->entry[list->count].label = 0;
    }
    if (list->entry[list->count].value) {
        free(ov00_02216868, list->entry[list->count].value, 0);
        list->entry[list->count].value = 0;
    }
    return 0;
}

// Parses the response: status code as "httpresult", then (only for 200 and !headerOnly) the header
// lines "label: value" and the body fields "label=value&...".
int ov00_021E9F2C(NasHttp *http, int headerOnly) {
    char *vp1;
    char *vp7;
    char vc0;
    char vc1;
    NasHttpEntryList list;
    char vc2;
    int vi0;
    char *vp3;
    char *vp4;
    char *vp5;
    char *vp0;
    char *vp2;
    char *vp6;
    list.entry = http->entry;
    list.max = 0x20;
    list.count = 0;
    MI_CpuFill8(http->entry, 0, 0x100);
    vp3 = http->response.buffer;
    vp7 = strstr(vp3, ov00_022166C4);
    if (vp7 == NULL) {
        return 0;
    }
    vp6 = vp7 + 4 + strlen(vp7 + 4);
    vp5 = strstr(vp3, ov00_02216884);
    if (vp5 == NULL) {
        return 0;
    }
    vc1 = vp5[4];
    vp5[4] = 0;
    vi0 = ov00_021E9DD8(http, &list, ov00_02216888, vp5 + 1);
    if (vi0 != 1) {
        vp5[4] = vc1;
        return 0;
    }
    vp5[4] = vc1;
    if (headerOnly == 1 || strncmp(vp5 + 1, ov00_02216894, 3) != 0) {
        return 1;
    }
    vp2 = strstr(vp5 + 5, ov00_022166F4);
    if (vp2 == NULL) {
        return 0;
    }
    vp2 += 2;
    while (vp2[0] != '\r' && vp2[1] != '\n') {
        vp4 = strstr(vp2, ov00_02216898);
        if (vp4 == NULL) {
            break;
        }
        vc0 = *vp4;
        *vp4 = 0;
        vp5 = vp4 + 2;
        vp1 = strstr(vp5, ov00_022166F4);
        if (vp1 == NULL) {
            *vp4 = vc0;
            break;
        }
        vc2 = *vp1;
        *vp1 = 0;
        if (ov00_021E9DD8(http, &list, vp2, vp5) != 1) {
            *vp4 = vc0;
            *vp1 = vc2;
            return 0;
        }
        vp2 = vp5 + strlen(vp5);
        *vp4 = vc0;
        *vp1 = vc2;
        vp2 += 2;
    }
    vp2 = vp7 + 4;
    if (vp2 < vp6) {
        do {
            vp0 = strstr(vp2, ov00_0221689C);
            if (vp0 == NULL) {
                break;
            }
            vc0 = *vp0;
            *vp0 = 0;
            vp3 = vp0 + 1;
            vp1 = strstr(vp3, ov00_022168A0);
            if (vp1 == NULL) {
                vp1 = strstr(vp3, ov00_022166F4);
            }
            if (vp1 != NULL) {
                vc2 = *vp1;
                *vp1 = 0;
            }
            if (ov00_021E9DD8(http, &list, vp2, vp3) != 1) {
                *vp0 = vc0;
                if (vp1 != NULL) {
                    *vp1 = vc2;
                }
                return 0;
            }
            vp2 = vp3 + strlen(vp3);
            *vp0 = vc0;
            vp2++;
            if (vp1 != NULL) {
                *vp1 = vc2;
            }
        } while (vp2 < vp6);
    }
    return 1;
}
