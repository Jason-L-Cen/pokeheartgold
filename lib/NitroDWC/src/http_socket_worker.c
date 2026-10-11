#include "global.h"

#include "dwc/http_socket_internal.h"

// The code below is compiled as ARM (32-bit) code.
#include <nitro/code32.h>

#pragma force_active on

typedef struct HttpSocketPoll {
    int fd;
    short events;
    short revents;
} HttpSocketPoll;

typedef struct HttpDecimalDivisors {
    u32 v[9];
} HttpDecimalDivisors;

extern int SOC_Poll(HttpSocketPoll *fds, int count, int timeout, int unused);

extern int ov38_02226C10[2];
extern char ov38_02226C58[];                    // 1024-byte send/line buffer
extern char ov38_02226C61[];                    // ov38_02226C58 + 9: status code of the response line
extern const HttpDecimalDivisors ov38_02225000; // 1000000000, 100000000, ..., 10

extern const char ov38_02224F8C[]; // "Content-Disposition: form-data; name=\""
extern const char ov38_02224FB4[]; // "Content-Type: application/octet-stream..." of a file part
extern const char ov38_02225EF4[]; // "GET "
extern const char ov38_02225EFC[]; // "POST "
extern const char ov38_02225F04[]; // "HEAD "
extern const char ov38_02225F0C[]; // "/"
extern const char ov38_02225F10[]; // " HTTP/1.1\r\n"
extern const char ov38_02225F1C[]; // "Host: "
extern const char ov38_02225F24[]; // "\r\n"
extern const char ov38_02225F28[]; // ": "
extern const char ov38_02225F2C[]; // "Content-Type: multipart/form-data; boundary="
extern const char ov38_02225F5C[]; // "Content-Type: application/x-www-form-urlencoded\r\n"
extern const char ov38_02225F90[]; // "Content-Length: "
extern const char ov38_02225FA4[]; // "\"\r\n"
extern const char ov38_02225FA8[]; // "--\r\n"
extern const char ov38_02225FB0[]; // "="
extern const char ov38_02225FB4[]; // "&"
extern const char ov38_02225FB8[]; // "HTTP/"
extern const char ov38_02225FC0[]; // "Content-Length"
extern const char ov38_02225FD0[]; // "Connection"
extern const char ov38_02225FDC[]; // "Keep-Alive"
extern const char ov38_02225FE8[]; // "Transfer-Encoding"
extern const char ov38_02225FFC[]; // "chunked"

static inline BOOL IsUpper(int c) {
    BOOL ret = FALSE;
    if (c >= 'A' && c <= 'Z') {
        ret = TRUE;
    }
    return ret;
}

static inline int ToLower(char c) {
    return IsUpper(c) ? c + 0x20 : c;
}

static inline int ToLowerAt(const char *p) {
    int upper = 0;
    if (*p >= 'A' && *p <= 'Z') {
        upper = 1;
    }
    return upper ? *p + 0x20 : *p;
}

// Sends n bytes through the ov38_02226C58 buffer; a socket error completes the job, a closed
// connection retries it on a new one.
#define SEND(p, n)                                           \
    do {                                                     \
        int sent = ov38_02223424(req, socket, &count, p, n); \
        if (sent < 0)                                        \
            goto complete;                                   \
        if (sent == 0)                                       \
            goto retry;                                      \
    } while (0)

void ov38_022236A0(void *arg) {
    int socket = -1;
    int urlLength;
    int length;
    char temp[12];
    HttpSslConnection *ssl;
    int count, parsed, status;
    HttpSocketPoll poll;
    char version[3];
    int invalid;
    int host;
    int lastHost;
    int lastPort;
    int multipart;
    int lastSecure;
    int connected;
    int error;
    int n;
    int offset;
    int remaining;

    ssl = NULL;
    ov38_02226C10[0] = 0;
    ov38_02226C10[1] = 0;
    multipart = 0;
    lastHost = socket;
    lastPort = socket;
    connected = 0;
    lastSecure = 0;
    HttpField *entry;
    HttpRequest *req;
    HttpResponse *response;
    while (!ov38_02226224) {
        int jobId;

        ov38_02222038();
        {
            HttpJob *job = ov38_02222008();
            if (!job) {
                jobId = -1;
            } else {
                jobId = job->id;
                req = job->request;
                ov38_02226220 = job;
            }
        }
        ov38_0222204C();
        if (jobId < 0) {
            ov38_02222100();
            continue;
        }
        response = req->response;
        if (req->cancel) {
            goto complete;
        }
        host = ov38_02222D6C(req);
        if (!host) {
            error = 4;
            goto complete;
        }
        invalid = -1;
        {
            int equal = 0;
            if (host == lastHost && req->port == lastPort && req->secure == lastSecure) {
                equal = 1;
            }
            lastPort = req->port;
            lastSecure = req->secure;
            lastHost = host;
            connected &= equal;
        }
retry:
        error = 0;
        if (socket >= 0) {
            poll.events = 9;
            poll.fd = socket;
            if (SOC_Poll(&poll, 1, 52365, 0) <= 0) {
                connected = 0;
            } else if (poll.revents & 0xe0) {
                connected = 0;
            }
        }
        if (!connected) {
            if (socket >= 0) {
                if (ov38_0222315C(req, socket) < 0) {
                    error = 10;
                }
                ov38_022235A0(socket, &ssl);
                socket = invalid;
                if (error) {
                    goto complete;
                }
            }
            socket = ov38_022230DC(req);
            if (socket < 0) {
                error = 3;
                goto complete;
            }
            if (req->secure) {
                ssl = req->ssl;
                req->ssl = NULL;
            }
            ov38_02222038();
            ov38_02226220->socket = socket;
            ov38_0222204C();
            if (req->cancel) {
                goto complete;
            }
            connected = ov38_022231A8(req, socket, host, req->port) >= 0;
        } else {
            if (req->secure) {
                ov38_02223564(req->ssl);
                req->ssl = NULL;
            }
            ov38_022230BC();
            ov38_02222038();
            ov38_02226220->socket = socket;
            ov38_0222204C();
        }
        if (req->cancel) {
            goto complete;
        }
        if (!connected) {
            error = 5;
            goto complete;
        }
        count = 0;
        connected = 0;
        urlLength = ov38_0222334C(req->url);
        error = 10;
        switch (req->method) {
        case 0:
            SEND(ov38_02225EF4, 4);
            break;
        case 1:
            SEND(ov38_02225EFC, 5);
            break;
        case 2:
            SEND(ov38_02225F04, 5);
            break;
        }
        offset = req->pathStart;
        if (urlLength > offset) {
            n = urlLength - offset;
            if (n) {
                SEND(req->url + offset, n);
            }
        } else {
            SEND(ov38_02225F0C, 1);
        }
        SEND(ov38_02225F10, 11);
        parsed = req->secure ? 8 : 7;
        SEND(ov38_02225F1C, 6);
        offset = parsed;
        n = req->hostEnd - offset;
        if (n) {
            SEND(req->url + offset, n);
        }
        SEND(ov38_02225F24, 2);
        entry = ov38_02221C60(&req->headers);
        while (entry) {
            n = ov38_0222334C(entry->name);
            if (n) {
                SEND(entry->name, n);
            }
            SEND(ov38_02225F28, 2);
            n = ov38_0222334C(entry->value);
            if (n) {
                SEND(entry->value, n);
            }
            SEND(ov38_02225F24, 2);
            ov38_0222621C(entry);
            entry = ov38_02221C60(&req->headers);
        }
        if (req->method == 1) {
            length = 0;
            multipart = 0;
            entry = req->posts;
            {
                HttpField *p = entry;
                while (p) {
                    if (p->isFile) {
                        multipart = 1;
                        break;
                    }
                    if (p == req->posts->prev) {
                        break;
                    }
                    p = p->next;
                }
            }
            if (multipart) {
                while (entry) {
                    length += 22;
                    length += ov38_0222334C(entry->name) + 41;
                    if (entry->isFile) {
                        length += 75;
                    }
                    length = length + 2;
                    length = length + entry->length;
                    length = length + 2;
                    if (entry == req->posts->prev) {
                        break;
                    }
                    entry = entry->next;
                }
                length += 24;
                SEND(ov38_02225F2C, 44);
                SEND(req->boundary + 2, 18);
                SEND(ov38_02225F24, 2);
            } else {
                while (entry) {
                    length += ov38_022249A0(entry->name);
                    length++;
                    length += ov38_022249A0(entry->value);
                    if (entry == req->posts->prev) {
                        break;
                    }
                    entry = entry->next;
                    length++;
                }
                SEND(ov38_02225F5C, 49);
            }
            SEND(ov38_02225F90, 16);
            n = ov38_02224C18(temp, length);
            parsed = n;
            if (n) {
                SEND(temp, n);
            }
            SEND(ov38_02225F24, 2);
        }
        SEND(ov38_02225F24, 2);
        if (req->method == 1) {
            if (multipart) {
                entry = req->posts;
                while (entry) {
                    SEND(req->boundary, 20);
                    SEND(ov38_02225F24, 2);
                    SEND(ov38_02224F8C, 38);
                    n = ov38_0222334C(entry->name);
                    if (n) {
                        SEND(entry->name, n);
                    }
                    SEND(ov38_02225FA4, 3);
                    if (entry->isFile) {
                        SEND(ov38_02224FB4, 75);
                    }
                    SEND(ov38_02225F24, 2);
                    n = entry->length;
                    if (n) {
                        SEND(entry->value, n);
                    }
                    SEND(ov38_02225F24, 2);
                    if (entry == req->posts->prev) {
                        break;
                    }
                    entry = entry->next;
                }
                SEND(req->boundary, 20);
                SEND(ov38_02225FA8, 4);
            } else {
                int index;
                HttpField *form = req->posts;
                while (form) {
                    for (index = 0; form->name[index]; index++) {
                        n = ov38_02224A08(temp, form->name[index]);
                        parsed = n;
                        if (n) {
                            SEND(temp, n);
                        }
                    }
                    SEND(ov38_02225FB0, 1);
                    for (index = 0; form->value[index]; index++) {
                        n = ov38_02224A08(temp, form->value[index]);
                        parsed = n;
                        if (n) {
                            SEND(temp, n);
                        }
                    }
                    if (form == req->posts->prev) {
                        break;
                    }
                    SEND(ov38_02225FB4, 1);
                    form = form->next;
                }
            }
        }
        if (count > 0) {
            n = ov38_02223270(req, socket, ov38_02226C58, count, 0);
            if (n < 0) {
                goto complete;
            }
            if (!n) {
                goto retry;
            }
        }
        response->headerLength = 0;
        temp[0] = 0;
        temp[1] = 0;
        temp[2] = 0;
        temp[3] = 0;
        {
            HttpResponseChunk *chunk = response->chunks;
            error = 7;
            count = 0;
            do {
                if (req->cancel) {
                    goto complete;
                }
                if (count < 1024) {
                    n = ov38_02223224(req, socket, response->bytes + count, 1, 0);
                    temp[count & 3] = response->bytes[count];
                } else {
                    offset = count & 511;
                    if (!offset) {
                        if (chunk) {
                            chunk->next = ov38_02226218(516, 4);
                            chunk = chunk->next;
                        } else {
                            chunk = ov38_02226218(516, 4);
                            response->chunks = chunk;
                        }
                        if (!chunk) {
                            error = 1;
                            goto complete;
                        }
                        chunk->next = NULL;
                    }
                    n = ov38_02223224(req, socket, chunk->bytes + offset, 1, 0);
                    temp[count & 3] = chunk->bytes[offset];
                }
                if (n <= 0) {
                    error = 10;
                    goto complete;
                }
                count += n;
            } while (temp[(count - 4) & 3] != 13 || temp[(count - 3) & 3] != 10 || temp[(count - 2) & 3] != 13 || temp[(count - 1) & 3] != 10);
        }
        response->headerLength = count;
        if (!response->headerLength) {
            goto complete;
        }
        if (!ov38_02222420(response, ov38_02226C58, 0, 14)) {
            goto complete;
        }
        if (ov38_022232E4(ov38_02226C58, ov38_02225FB8, 5) || ov38_02226C58[8] != ' ') {
            goto complete;
        }
        if (ov38_02224B80(ov38_02226C61, 3) < 0) {
            goto complete;
        }
        if (ov38_02222210(response, 12, response->headerLength, &status) < 0) {
            goto complete;
        }
        remaining = ov38_02222E7C(response, ov38_02225FC0, &parsed);
        if (!remaining) {
            error = 0;
            goto complete;
        }
        if (remaining > 1024) {
            goto complete;
        }
        if (remaining > 0) {
            if (!ov38_02222420(response, ov38_02226C58, parsed, remaining)) {
                goto complete;
            }
            remaining = ov38_02224B80(ov38_02226C58, remaining);
            if (remaining < 0) {
                goto complete;
            }
            req->contentLength = remaining;
        } else {
            req->contentLength = -1;
        }
        connected = ov38_02222E7C(response, ov38_02225FD0, &parsed);
        if (!connected) {
            goto complete;
        }
        if (connected < 0) {
            version[0] = ov38_02226C58[5];
            version[1] = ov38_02226C58[7];
            version[2] = 0;
            connected = ov38_02224B80(version, 2) >= 11;
        } else if (connected > 1024) {
            connected = 0;
        } else {
            connected = ov38_02222350(response, parsed, parsed + connected, ov38_02225FDC, 0) == 0;
        }
        n = ov38_02222E7C(response, ov38_02225FE8, &parsed);
        if (!n) {
            goto complete;
        }
        {
            int chunked;
            if (n > 1024) {
                chunked = 0;
            } else if (n > 0) {
                chunked = ov38_02222350(response, parsed, parsed + n, ov38_02225FFC, 59) == 0;
            } else {
                chunked = 0;
            }
            if (req->method == 2) {
                goto complete;
            }
            if (remaining >= 0) {
                while (remaining > 0 && !ov38_0222250C(response, response->bodyLength)) {
                    n = ov38_02222548(req, socket, response->bodyLength, remaining, 0);
                    if (n < 0) {
                        goto complete;
                    }
                    if (!n) {
                        break;
                    }
                    remaining -= n;
                    response->bodyLength += n;
                }
                if (remaining) {
                    error = ov38_0222250C(response, response->bodyLength) ? 6 : 10;
                    goto complete;
                }
                error = 0;
                goto complete;
            }
            error = 10;
            if (chunked) {
                int chunkRemaining;
                for (;;) {
                    temp[0] = 0;
                    temp[1] = 0;
                    count = 0;
                    while (count < 1024) {
                        n = ov38_02223224(req, socket, ov38_02226C58 + count, 1, 0);
                        if (n < 0) {
                            goto complete;
                        }
                        temp[count & 1] = ov38_02226C58[count];
                        if (ov38_02226C58[count] == ';' || (ov38_02226C58[count] == 10 && temp[(count - 1) & 1] == 13)) {
                            if (ov38_02226C58[count] == 10) {
                                parsed = count - 1;
                            } else {
                                parsed = count;
                                if (ov38_022234D8(req, socket) <= 0) {
                                    goto complete;
                                }
                            }
                            if (!parsed) {
                                goto complete;
                            }
                            chunkRemaining = ov38_02224A98(ov38_02226C58, parsed);
                            if (chunkRemaining < 0) {
                                goto complete;
                            }
                            break;
                        }
                        count++;
                    }
                    if (count == 1024) {
                        error = 7;
                        goto complete;
                    }
                    if (chunkRemaining > 0) {
                        while (chunkRemaining > 0) {
                            n = ov38_02222548(req, socket, response->bodyLength, chunkRemaining, 0);
                            if (n <= 0) {
                                goto complete;
                            }
                            response->bodyLength += n;
                            chunkRemaining -= n;
                            if (!chunkRemaining) {
                                if (ov38_02223224(req, socket, ov38_02226C58, 1, 0) <= 0) {
                                    goto complete;
                                }
                                if (ov38_02223224(req, socket, ov38_02226C58, 1, 0) <= 0) {
                                    goto complete;
                                }
                            }
                        }
                    } else {
                        ov38_022234D8(req, socket);
                        error = 0;
                        goto complete;
                    }
                }
            } else {
                if (ov38_0222250C(response, response->bodyLength)) {
                    goto complete;
                }
                do {
                    n = ov38_02222520(req, socket, response->bodyLength, 0);
                    if (n < 0) {
                        goto complete;
                    }
                    if (!n) {
                        error = 0;
                        goto complete;
                    }
                    response->bodyLength += n;
                    if (ov38_0222250C(response, response->bodyLength)) {
                        n = ov38_02223224(req, socket, ov38_02226C58, 1, 0);
                        if (n < 0) {
                            goto complete;
                        }
                        if (n) {
                            error = 6;
                            goto complete;
                        }
                    }
                } while (!ov38_0222250C(response, response->bodyLength));
            }
        }
complete:
        ov38_02222038();
        ov38_0222621C(ov38_02226220);
        ov38_02226220 = NULL;
        ov38_0222204C();
        if (req->cancel) {
            error = 8;
        }
        if (socket >= 0 && (!connected || error)) {
            if (ov38_0222315C(req, socket) < 0) {
                error = 10;
            }
            ov38_022235A0(socket, &ssl);
            socket = -1;
            connected = 0;
        }
        if (!error) {
            response->success = 1;
        } else {
            response->success = 0;
            ov38_02226214 = error;
        }
        {
            void *user = req->param;
            void (*callback)(int, HttpResponse *, void *) = req->callback;
            ov38_02222B98(req);
            callback(error, response, user);
        }
    }
    if (socket >= 0) {
        ov38_0222315C(req, socket);
        ov38_022235A0(socket, &ssl);
    }
    ov38_02223640();
}

#undef SEND

int ov38_022249A0(const char *s) {
    int n = 0, c = *s++;
    while (c) {
        if ((c >= 48 && c <= 57) || (c >= 65 && c <= 90) || (c >= 97 && c <= 122) || c == 32) {
            n++;
        } else {
            n += 3;
        }
        c = *s++;
    }
    return n;
}

int ov38_02224A08(char *p, int c) {
    int hi, lo;
    if (c == 32) {
        *p = 43;
        return 1;
    }
    if ((c >= 48 && c <= 57) || (c >= 65 && c <= 90) || (c >= 97 && c <= 122)) {
        *p = c;
        return 1;
    }
    hi = (c >> 4) & 15;
    *p = 37;
    if (hi < 10) {
        p[1] = hi + 48;
    } else {
        p[1] = hi + 55;
    }
    lo = c & 15;
    if (lo < 10) {
        p[2] = lo + 48;
    } else {
        p[2] = lo + 55;
    }
    return 3;
}

int ov38_02224A98(const char *s, int n) {
    int i;
    int val;
    BOOL started;
    if (n > 8) {
        return -1;
    }
    if (n == 8 && s[0] > '7') {
        return -1;
    }
    i = 0;
    val = 0;
    started = FALSE;
    for (; i < n; i++) {
        char c = s[i];
        c = IsUpper(c) ? c + 0x20 : c;
        if (c >= '0' && c <= '9') {
            started = TRUE;
            val = val * 16 + c - '0';
        } else if (c >= 'a' && c <= 'f') {
            started = TRUE;
            val = val * 16 + c - 'a' + 10;
        } else {
            if (started && (c == ' ' || c == '\0')) {
                break;
            }
            if (!started && c == ' ') {
                continue;
            }
            return -1;
        }
    }
    return val;
}

int ov38_02224B80(const char *s, int n) {
    int i;
    int val;
    BOOL started;
    if (n > 10) {
        return -1;
    }
    i = 0;
    val = 0;
    started = FALSE;
    for (; i < n; i++) {
        char c = s[i];
        if (started && (c == ' ' || c == '\0')) {
            break;
        }
        if (!started && c == ' ') {
            continue;
        }
        if (c < '0' || c > '9') {
            return -1;
        }
        started = TRUE;
        {
            int prev = val;
            val = val * 10 + c - '0';
            if (prev > val) {
                return -1;
            }
        }
    }
    return val;
}

int ov38_02224C18(char *out, u32 val) {
    HttpDecimalDivisors d = ov38_02225000;
    int i;
    int len = 0;
    BOOL started;
    i = 0;
    started = FALSE;
    for (; i < 9; i++) {
        u32 p = d.v[i];
        if (val >= p) {
            u32 q = val / p;
            started = TRUE;
            val -= q * p;
            if (out != NULL) {
                out[len] = q + '0';
            }
            len++;
        } else if (started) {
            if (out != NULL) {
                out[len] = '0';
            }
            len++;
        }
    }
    if (out != NULL) {
        out[len] = val + '0';
    }
    return len + 1;
}

int ov38_02224CD4(const char *a, const char *b) {
    int lb, la;
    while (lb = ToLower(*b), la = ToLowerAt(a), la == lb) {
        if (*a == '\0' || *a == ' ') {
            return 0;
        }
        a++;
        b++;
    }
    return -1;
}

int ov38_02224D58(const char *p, int n) {
    int count = 0, total = 0;
    while (n--) {
        int c = *p;
        if (c != 32 && c >= 48 && c <= 57) {
            total = total * 10 + c - 48;
            if (++count > 9) {
                return -1;
            }
        }
        p++;
    }
    if (!count) {
        total = -1;
    }
    return total;
}

int ov38_02224DC8(const char *hay, int hlen, const char *needle, int nlen) {
    int i;
    int j;
    if (hlen < nlen) {
        return -1;
    }
    for (i = 0; i < hlen - nlen + 1; i++) {
        if (needle[0] == hay[i]) {
            const char *p = hay + i;
            for (j = 1; j < nlen; j++) {
                if (p[j] != needle[j]) {
                    break;
                }
            }
            if (j == nlen) {
                return 0;
            }
        }
    }
    return -1;
}
