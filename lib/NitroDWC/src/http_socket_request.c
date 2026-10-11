#include "global.h"

#include "dwc/http_socket_internal.h"

// The code below is compiled as ARM (32-bit) code.
#include <nitro/code32.h>

#pragma force_active on

typedef struct HttpSocketAddress {
    u8 size;
    u8 family;
    u16 port;
    u32 ip;
} HttpSocketAddress;

// Ring of 8 closed sockets whose SSL connection is freed later (ov38_02226C10: write/read index).
typedef struct HttpClosedSocket {
    int socket;
    HttpSslConnection *ssl;
} HttpClosedSocket;

// ov38_02226C18 seen from its .ssl member (label ov38_02226C1C)
typedef struct HttpClosedSsl {
    HttpSslConnection *ssl;
    int nextSocket;
} HttpClosedSsl;

extern u32 STD_GetStringLength(const char *str);
extern int STD_CompareString(const char *a, const char *b);
extern int SOC_InetAtoN(const char *str, u32 *ip);
extern u32 SOCL_Resolve(const char *host);
extern int SOC_Socket(int domain, int type, int protocol);
extern int SOC_Close(int socket);
extern int SOC_Connect(int socket, HttpSocketAddress *address);
extern int SOC_Recv(int socket, void *buf, int size, int flags);
extern int SOC_Send(int socket, const void *buf, int size, int flags);
extern int SOC_Shutdown(int socket, int how);
extern int SOCL_EnableSsl(int socket, HttpSslConnection *ssl);
extern void CPS_SslAddRandomSeed(void *seed, int size);

extern u8 ov38_02224F74[20];       // default boundary "--t9Sf4yfjf1RtvDu3AA"
extern const char ov38_02225ED0[]; // "http://"
extern const char ov38_02225ED8[]; // "https://"
extern char ov38_02225EE4[];       // "HTTPSTATUSCODE"
extern u32 ov38_02226BF0[];        // entropy for the SSL random seed
extern int ov38_02226C10[2];
extern HttpClosedSocket ov38_02226C18[8];
extern HttpClosedSsl ov38_02226C1C[8]; // .ssl of ov38_02226C18
extern char ov38_02226C58[];

HttpRequest *ov38_022225B8(const char *url, int method, void *body, int bodyCapacity, HttpSocketCallback callback, void *param) {
    int len;
    int prefix;
    int i;
    int j;
    const char *host;
    int pct;
    int esc;
    BOOL slash;
    char *p;
    HttpRequest *request = NULL;

    if (method != 0 && method != 1 && method != 2) {
        ov38_02226214 = 11;
        goto fail;
    }
    if (bodyCapacity == 0) {
        ov38_02226214 = 11;
        goto fail;
    }
    request = ov38_02226218(sizeof(HttpRequest), 4);
    if (request == NULL) {
        ov38_02226214 = 1;
        goto fail;
    }
    ov38_02223358(request, sizeof(HttpRequest));
    request->response = ov38_02226218(sizeof(HttpResponse), 4);
    if (request->response == NULL) {
        ov38_02226214 = 1;
        goto fail;
    }
    ov38_02223358(request->response, sizeof(HttpResponse));
    request->response->body = body;
    request->response->bodyCapacity = bodyCapacity;
    len = ov38_0222334C(url);
    if (len <= 7) {
        ov38_02226214 = 4;
        goto fail;
    }
    prefix = 7;
    request->port = 80;
    if (ov38_022232E4(url, ov38_02225ED0, prefix) != 0) {
        if (ov38_022232E4(url, ov38_02225ED8, 8) != 0) {
            ov38_02226214 = 4;
            goto fail;
        }
        request->secure = 1;
        request->port = 443;
        prefix = 8;
    }
    len -= prefix;
    host = url + prefix;
    if (len <= 0) {
        ov38_02226214 = 4;
        goto fail;
    }

    i = 0;
    pct = 0;
    esc = 0;
    while (i < len && host[i] != '/') {
        if (esc == 2) {
            esc--;
        } else if (esc == 1) {
            char c = ov38_02224A98(host + (i - 1), 2);
            esc--;
            if (c < 0) {
                ov38_02226214 = 4;
                goto fail;
            }
            if (c == '/') {
                pct--;
                break;
            }
        } else if (host[i] == '%') {
            esc = 2;
            pct++;
        }
        i++;
    }
    if (esc != 0) {
        ov38_02226214 = 4;
        goto fail;
    }

    request->url = ov38_02226218(prefix + len - pct * 2 + 1, 4);
    if (request->url == NULL) {
        ov38_02226214 = 1;
        goto fail;
    }
    ov38_0222336C(request->url, url, prefix);

    i = 0;
    j = 0;
    esc = 0;
    slash = FALSE;
    for (; i < len; i++) {
        if (esc == 2) {
            esc--;
        } else if (esc == 1) {
            char c = ov38_02224A98(host + (i - 1), 2);
            esc--;
            request->url[prefix + j - 1] = c;
            if (c == '/') {
                slash = TRUE;
            }
        } else {
            char c = host[i];
            if (c == '/') {
                slash = TRUE;
            }
            if (!slash && c == '%') {
                esc = 2;
            } else {
                request->url[prefix + j] = c;
            }
            j++;
        }
    }
    request->url[prefix + j] = '\0';

    p = request->url + prefix;
    for (i = 0; i < j; i++) {
        if (p[i] == '/' || p[i] == ':') {
            request->hostEnd = i + prefix;
            break;
        }
    }
    if (i == j) {
        request->pathStart = request->hostEnd = i + prefix;
    } else if (p[i] == '/') {
        request->pathStart = request->hostEnd;
    } else if (p[i] == ':') {
        for (; i < j; i++) {
            if (p[i] == '/') {
                request->pathStart = i + prefix;
                break;
            }
        }
        if (i == j) {
            request->pathStart = i + prefix;
        } else {
            int start = request->hostEnd + 1;
            int port = ov38_02224D58(request->url + start, request->pathStart - start);
            if (port < 0) {
                port = request->port;
            } else if (port > 0xFFFF) {
                ov38_02226214 = 4;
                goto fail;
            }
            request->port = (u16)port;
        }
    }

    if (request->secure) {
        int n = request->hostEnd - prefix;
        request->ssl = ov38_02226218(sizeof(HttpSslConnection), 4);
        if (request->ssl == NULL) {
            ov38_02226214 = 1;
            goto fail;
        }
        ov38_02223358(request->ssl, sizeof(HttpSslConnection));
        request->ssl->host = ov38_02226218(n + 1, 4);
        if (request->ssl->host == NULL) {
            ov38_02226214 = 1;
            goto fail;
        }
        ov38_0222336C(request->ssl->host, request->url + prefix, n);
        request->ssl->host[n] = '\0';
    }
    ov38_0222336C(request->boundary, ov38_02224F74, sizeof(request->boundary));
    request->param = param;
    request->method = method;
    request->callback = callback;
    return request;

fail:
    if (request != NULL) {
        if (request->ssl != NULL) {
            if (request->ssl->host != NULL) {
                ov38_0222621C(request->ssl->host);
            }
            ov38_0222621C(request->ssl);
        }
        if (request->url != NULL) {
            ov38_0222621C(request->url);
        }
        if (request->response != NULL) {
            ov38_0222621C(request->response);
        }
        ov38_0222621C(request);
    }
    return NULL;
}

void ov38_02222B1C(HttpRequest *request) {
    ov38_0222621C(request->response);
    ov38_02222B98(request);
}

void ov38_02222B44(HttpField *list) {
    while (list) {
        HttpField *last = list->prev;
        if (list != last) {
            HttpField *prev = last->prev;
            ov38_0222621C(last);
            list->prev = prev;
        } else {
            ov38_0222621C(list);
            list = NULL;
        }
    }
}

int ov38_02222B98(HttpRequest *request) {
    ov38_02222B44(request->headers);
    ov38_02222B44(request->posts);
    if (request->ssl) {
        if (request->ssl->host) {
            ov38_0222621C(request->ssl->host);
        }
        ov38_0222621C(request->ssl);
    }
    ov38_0222621C(request->url);
    ov38_0222621C(request);
    return 1;
}

int ov38_02222C10(HttpRequest *request) {
    int id;
    if (request->queued) {
        ov38_02226214 = 11;
        return -1;
    }
    if (request->method == HTTP_SOCKET_METHOD_POST && !request->posts) {
        ov38_02226214 = 11;
        return -1;
    }
    ov38_02222038();
    id = ov38_02221E2C(request);
    if (id >= 0) {
        request->queued = 1;
        ov38_0222211C();
    } else {
        ov38_02226214 = 1;
    }
    ov38_0222204C();
    return id;
}

int ov38_02222C9C(int id) {
    int ok = 0;
    ov38_02222038();
    if (ov38_02226220 && ov38_02226220->id == id && ov38_02226220->request->cancel == 0) {
        ov38_02226220->request->cancel = 1;
        ov38_022232BC(ov38_02226220->request, ov38_02226220->socket);
        ok = 1;
    }
    if (!ok) {
        ok = ov38_02221F38(id);
    }
    ov38_0222204C();
    return ok;
}

void ov38_02222D18(void) {
    ov38_02222038();
    if (ov38_02226220 && !ov38_02226220->request->cancel) {
        ov38_02226220->request->cancel = 1;
        ov38_022232BC(ov38_02226220->request, ov38_02226220->socket);
    }
    ov38_02221FD8();
    ov38_0222204C();
}

// IP address of the request's host, 0 if it cannot be resolved.
u32 ov38_02222D6C(HttpRequest *request) {
    char old = request->url[request->hostEnd];
    char *url;
    int skip;

    struct {
        u32 ip;
    } addr;

    u32 ip;
    request->url[request->hostEnd] = 0;
    url = request->url;
    skip = request->secure ? 8 : 7;
    if (SOC_InetAtoN(url + skip, &addr.ip)) {
        ip = addr.ip;
    } else {
        ip = SOCL_Resolve(url + skip);
        if (!ip) {
            return 0;
        }
        ip = (ip >> 24 & 255) | (ip >> 8 & 0xff00) | (ip << 8 & 0xff0000) | (ip << 24 & 0xff000000);
    }
    request->url[request->hostEnd] = old;
    return ip;
}

void ov38_02222E04(HttpResponse *response) {
    HttpResponseChunk *chunk = response->chunks;
    while (chunk) {
        HttpResponseChunk *next = chunk->next;
        ov38_0222621C(chunk);
        response->chunks = next;
        chunk = response->chunks;
    }
    if (response->unk_10) {
        ov38_0222621C(response->unk_10);
    }
    if (response->headerValue) {
        ov38_0222621C(response->headerValue);
    }
    ov38_0222621C(response);
}

// Finds the header line `name: value`; returns the value's length and its start in *valueStart,
// 0 if the line has no value and -1 if there is no such header.
int ov38_02222E7C(HttpResponse *response, const char *name, int *valueStart) {
    int colon, current, next, end;
    current = ov38_02222210(response, 12, response->headerLength, &colon);
    end = 0;
    while (current > 0) {
        next = ov38_02222210(response, current, response->headerLength, &colon);
        if (colon > 0 && ov38_02222350(response, current, colon, name, 0) == 0) {
            if (colon + 1 < response->headerLength) {
                end = ov38_02222210(response, colon + 1, response->headerLength, NULL);
                if (end <= 0) {
                    end = response->headerLength;
                } else {
                    if (end < 2) {
                        return -1;
                    }
                    end -= 2;
                }
                {
                    int start = ov38_022222E0(response, colon + 1, end);
                    if (start < 0) {
                        start = end;
                    }
                    *valueStart = start;
                    return end - start;
                }
            }
            return 0;
        }
        current = next;
    }
    return -1;
}

// Value of the response header name ("HTTPSTATUSCODE": the status code), copied to headerValue.
int ov38_02222F88(HttpResponse *response, const char *name, char **value) {
    int start[1], length;
    if (!response->success) {
        return -1;
    }
    if (response->headerValue) {
        ov38_0222621C(response->headerValue);
        response->headerValue = NULL;
    }
    length = ov38_02222E7C(response, name, start);
    if (length >= 0) {
        response->headerValue = ov38_02226218(length + 1, 4);
        response->headerValue[length] = 0;
        ov38_02222420(response, response->headerValue, start[0], length);
        *value = response->headerValue;
        return length;
    }
    if (ov38_022232D8(ov38_02225EE4, name) != 0) {
        return -1;
    }
    response->headerValue = ov38_02226218(4, 4);
    response->headerValue[3] = 0;
    ov38_02222420(response, response->headerValue, 9, 3);
    *value = response->headerValue;
    return 3;
}

int ov38_0222308C(HttpResponse *response, char **body) {
    if (!response->success || !response->bodyLength) {
        return -1;
    }
    *body = response->body;
    return response->bodyLength;
}

int ov38_022230B0(int result) {
    if (result & 0x8000) {
        result &= ~0x8000;
    }
    return result;
}

void ov38_022230BC(void) {
    OS_GetLowEntropyData(ov38_02226BF0);
    CPS_SslAddRandomSeed(ov38_02226BF0, 32);
}

int ov38_022230DC(HttpRequest *request) {
    int socket = SOC_Socket(2, 1, 0);
    if (socket >= 0 && request->secure) {
        ov38_022230BC();
        request->ssl->unk_814 = request->unk_1C;
        request->ssl->unk_818 = request->unk_18;
        request->ssl->unk_810 = ov38_022230B0;
        if (SOCL_EnableSsl(socket, request->ssl) < 0) {
            SOC_Close(socket);
            socket = -1;
        }
    }
    return socket;
}

int ov38_0222315C(HttpRequest *request, int socket) {
    return SOC_Close(socket);
}

// Closes the socket, waiting while the stack reports it busy (-26).
int ov38_0222316C(HttpRequest *request, int socket) {
    int result = SOC_Close(socket);
    if (result != -26) {
        return result;
    }
    do {
        OS_Sleep(500);
        result = SOC_Close(socket);
    } while (result == -26);
    return result;
}

int ov38_022231A8(HttpRequest *request, int socket, int ip, int port) {
    HttpSocketAddress address;

    address.size = 8;
    address.family = 2;
    address.port = ((u16)port >> 8 & 255) | ((u16)port << 8 & 0xff00);
    address.ip = ip;
    if (SOC_Connect(socket, &address) < 0) {
        if (request->cancel) {
            return -1002;
        }
        return -1001;
    }
    return 0;
}

// The socket wrappers return -1002 when the request was cancelled, -1001 on an error and 0 when
// the operation would block (-56).
int ov38_02223224(HttpRequest *request, int socket, void *buf, int size, int flags) {
    int result = SOC_Recv(socket, buf, size, flags);
    if (result >= 0) {
        return result;
    }
    if (request->cancel) {
        return -1002;
    }
    if (result == -56) {
        return 0;
    }
    return -1001;
}

int ov38_02223270(HttpRequest *request, int socket, const void *buf, int size, int flags) {
    int result = SOC_Send(socket, buf, size, flags);
    if (result >= 0) {
        return result;
    }
    if (request->cancel) {
        return -1002;
    }
    if (result == -56) {
        return 0;
    }
    return -1001;
}

void ov38_022232BC(HttpRequest *request, int socket) {
    if (socket < 0) {
        return;
    }
    SOC_Shutdown(socket, 2);
}

int ov38_022232D8(const char *a, const char *b) {
    return STD_CompareString(a, b);
}

// Case-insensitive compare of at most n characters; returns the count left (0: equal).
int ov38_022232E4(const char *a, const char *b, int n) {
    while (n > 0) {
        int y = *b++, x = *a++;
        if (!x || !y) {
            if (!x && !y) {
                n = 0;
            }
            break;
        }
        if (y >= 'A' && y <= 'Z') {
            y += 32;
        }
        if (x >= 'A' && x <= 'Z') {
            x += 32;
        }
        if (x != y) {
            break;
        }
        n--;
    }
    return n;
}

u32 ov38_0222334C(const char *str) {
    return STD_GetStringLength(str);
}

void ov38_02223358(void *dst, int size) {
    MI_CpuFill8(dst, 0, size);
}

void ov38_0222336C(void *dst, const void *src, int size) {
    MI_CpuCopy8(src, dst, size);
}

// Progress of the current job: body bytes received and Content-Length (0 if unknown). Returns
// FALSE when no job is running or queued.
int ov38_02223384(int *received, int *total) {
    int ok;
    *received = 0;
    *total = 0;
    ov38_02222038();
    if (ov38_02226220) {
        if (ov38_02226220->request->contentLength) {
            *received = ov38_02226220->request->response->bodyLength;
            *total = ov38_02226220->request->contentLength == -1 ? 0 : ov38_02226220->request->contentLength;
        }
        ok = 1;
    } else {
        ok = ov38_02226230 != NULL;
    }
    ov38_0222204C();
    return ok;
}

// Appends size bytes to the 1024-byte send buffer ov38_02226C58, sending it whenever it is full.
int ov38_02223424(HttpRequest *request, int socket, int *used, const char *src, int size) {
    int left = size;
    while (left > 0) {
        int count;
        if (request->cancel) {
            return -1;
        }
        count = 1024 - *used;
        if (left <= count) {
            count = left;
        }
        ov38_0222336C(ov38_02226C58 + *used, src, count);
        *used += count;
        src += count;
        left -= count;
        if (*used == 1024) {
            int result = ov38_02223270(request, socket, ov38_02226C58, 1024, 0);
            if (result <= 0) {
                return result;
            }
            *used -= result;
        }
    }
    return size;
}

// Skips the rest of a line (up to CR LF); returns the bytes read.
int ov38_022234D8(HttpRequest *request, int socket) {
    char buf[2];
    int n;
    int i;
    int total = 0;
    i = 0;
    buf[0] = 0;
    buf[1] = 0;
    for (; buf[i & 1] != 13 || buf[(i - 1) & 1] != 10; i++) {
        n = ov38_02223224(request, socket, buf + (i & 1), 1, 0);
        if (n <= 0) {
            return n;
        }
        total += n;
    }
    return total;
}

void ov38_02223564(HttpSslConnection *ssl) {
    if (!ssl) {
        return;
    }
    if (ssl->host) {
        ov38_0222621C(ssl->host);
    }
    ov38_0222621C(ssl);
}

void ov38_022235A0(int socket, HttpSslConnection **ssl) {
    int next = (ov38_02226C10[0] + 1) & 7;
    if (next == ov38_02226C10[1]) {
        ov38_0222316C(NULL, ov38_02226C18[ov38_02226C10[1]].socket);
        ov38_02223564(ov38_02226C1C[ov38_02226C10[1]].ssl);
        ov38_02226C10[1] = (ov38_02226C10[1] + 1) & 7;
    }
    if (!*ssl) {
        return;
    }
    ov38_02226C18[ov38_02226C10[0]].socket = socket;
    ov38_02226C1C[ov38_02226C10[0]].ssl = *ssl;
    *ssl = NULL;
    ov38_02226C10[0] = next;
}

void ov38_02223640(void) {
    while (ov38_02226C10[0] != ov38_02226C10[1]) {
        ov38_0222316C(NULL, ov38_02226C18[ov38_02226C10[1]].socket);
        ov38_02223564(ov38_02226C18[ov38_02226C10[1]].ssl);
        ov38_02226C10[1] = (ov38_02226C10[1] + 1) & 7;
    }
}
