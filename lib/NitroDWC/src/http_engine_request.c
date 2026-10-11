#include "global.h"

#include "dwc/http_socket_internal.h"

// The code below is compiled as ARM (32-bit) code.
#include <nitro/code32.h>

#pragma force_active on

// Requests of the HTTP engine (http_engine_init.c) over the socket HTTP client: every request
// is a POST with the engine's account fields and an "action"; the response is parsed by type.

enum {
    HTTP_ENGINE_REQUEST_COUNT = 1,    // ov38_02220F18
    HTTP_ENGINE_REQUEST_LIST = 2,     // ov38_02220F8C
    HTTP_ENGINE_REQUEST_CONTENTS = 3, // ov38_0222124C
};

typedef void (*HttpEngineCallback)(int type, int result, int value);

typedef struct HttpEngineRequest {
    int jobId;                   // 0x00
    int type;                    // 0x04, HTTP_ENGINE_REQUEST_*
    HttpEngineCallback callback; // 0x08
    int notify;                  // 0x0C
    void *output;                // 0x10
    int size;                    // 0x14
    void *buffer;                // 0x18
    char *offsetParam;           // 0x1C
    char *numParam;              // 0x20
} HttpEngineRequest;

// One line of the file list.
typedef struct HttpEngineFileInfo {
    char name[34];   // 0x00
    u16 unk_22[51];  // 0x22
    char unk_88[11]; // 0x88
    char unk_93[11]; // 0x93
    char unk_9E[11]; // 0x9E
    u8 unk_A9[3];
    int size; // 0xAC
} HttpEngineFileInfo;

typedef struct HttpEngineWiFiInfo {
    u64 userId;
    u8 unk_08[12];
} HttpEngineWiFiInfo;

extern u32 STD_GetStringLength(const char *str);
extern int STD_CompareNString(const char *a, const char *b, u32 n);
extern void DWCi_BM_GetWiFiInfo(HttpEngineWiFiInfo *info);

// fields of ov38_02226050 (Ov38Work, http_engine_init.c)
extern int ov38_02226054;
extern char *ov38_02226058;
extern void *(*ov38_0222605C)(int size, int align);
extern char *ov38_02226060;
extern void (*ov38_02226068)(int type, int result, int value);
extern int ov38_0222606C;
extern void (*ov38_02226070)(void *ptr);
extern int ov38_02226074;
extern void *ov38_02226078[3];

extern int ov38_022261DC;
extern OSMutex ov38_022261E0;
extern OSMutex ov38_022261F8;
extern u64 ov38_02224EF4[];  // 10^16, 10^15, ..., 10
extern char ov38_02224ECC[]; // "list", base64
extern char ov38_02224ED8[]; // "count", base64
extern char ov38_02224EE4[]; // "contents", base64
extern char ov38_02225E08[]; // "Nitro WiFi SDK/%d.%d"
extern char ov38_02225E20[];
extern char ov38_02225E24[]; // "HTTPSTATUSCODE"
extern char ov38_02225E34[]; // "returncd"
extern char ov38_02225E40[]; // "User-Agent"
extern char ov38_02225E4C[]; // "gamecd"
extern char ov38_02225E54[]; // "rhgamecd"
extern char ov38_02225E60[]; // "passwd"
extern char ov38_02225E68[]; // "token"
extern char ov38_02225E70[]; // "userid"
extern char ov38_02225E78[]; // "macadr"
extern char ov38_02225E80[]; // "action"
extern char ov38_02225E88[]; // "attr1"
extern char ov38_02225E90[]; // "attr2"
extern char ov38_02225E98[]; // "attr3"
extern char ov38_02225EA0[]; // "apinfo"
extern char ov38_02225EA8[]; // "offset"
extern char ov38_02225EB0[]; // "num"
extern char ov38_02225EB4[]; // "contents"
extern char ov38_02225EC0[]; // "Content-Length"
extern char ov38_02226084[];
extern char ov38_02226090[];
extern char ov38_022260A4[];
extern char ov38_022260BC[];
extern char ov38_022260D8[];
extern char ov38_02226138[];
extern char ov38_02226144[];
extern char ov38_0222615C[];

int ov38_02220784(char *gameCode, char *userId);
int ov38_02220810(char *buf);
void ov38_02220830(int error, HttpResponse *response, HttpEngineRequest *req);
void ov38_02220B60(HttpEngineRequest *req, HttpRequest *request);
int ov38_02220B8C(HttpEngineRequest **outReq, HttpRequest **outRequest, const char *action, void *body, int bodyCapacity);
int ov38_02220EB8(HttpEngineRequest *req, HttpRequest *request);
int ov38_02220F18(int *count);
int ov38_02220F8C(HttpEngineFileInfo *list, int offset, int count);
int ov38_0222124C(HttpEngineFileInfo *info, void *out, u32 capacity);
int ov38_02221408(int *received, int *total);
int ov38_02221414(HttpResponse *response, HttpEngineRequest *req);
int ov38_02221468(HttpResponse *response, HttpEngineRequest *req);
int ov38_02221494(HttpResponse *response, HttpEngineRequest *req);
int ov38_02221718(void);
void ov38_02221730(void);
void ov38_02221734(void);
void ov38_02221748(void);
int ov38_0222175C(void);
void ov38_02221784(void);
int ov38_02221788(void);
void ov38_022217D0(void);
int ov38_022217FC(OSMessageQueue *queue);
int ov38_02221810(char *out);
u32 ov38_02221870(const char *s);
int ov38_0222187C(const char *a, const char *b, u32 n);
void ov38_02221888(void *p, int n);
void ov38_0222189C(void *p, const void *s, int n);
int ov38_022218B4(char *p, int n);
int ov38_022218C0(char *p, int n);
int ov38_022218CC(char *buf, u64 val, int width);
int ov38_022219A4(char *buf, u32 val, int n);

// Copies the Wi-Fi user ID (13 decimal digits) to userId and 4 bytes from 0x027FFE0C to gameCode.
int ov38_02220784(char *gameCode, char *userId) {
    HttpEngineWiFiInfo info;
    char buf[13];
    ov38_02221888(&info, 20);
    DWCi_BM_GetWiFiInfo(&info);
    if (info.userId == 0) {
        return 0;
    }
    if (ov38_022218CC(buf, info.userId, 13) < 0) {
        return 0;
    }
    ov38_022201C8(userId, buf, 13);
    ov38_022201C8(gameCode, (const char *)0x027FFE0C, 4);
    return 1;
}

int ov38_02220810(char *buf) {
    return OS_SNPrintf(buf, 33, ov38_02225E08, 2, 2);
}

void ov38_02220830(int error, HttpResponse *response, HttpEngineRequest *req) {
    HttpEngineCallback callback = req->callback;
    int notify = req->notify;
    int type = req->type;
    int value = -1;
    int result;
    char *status;
    char *body;
    char dec[4];

    switch (error) {
    case 0:
        if (ov38_02222F88(response, ov38_02225E24, &status) != 3) {
            result = 3;
            break;
        } else {
            int code = ov38_022218B4(status, 3);
            if (code == 302) {
                BOOL found = FALSE;
                int len = ov38_0222308C(response, &body);
                int i;
                if (len < 0) {
                    result = 3;
                    break;
                }
                for (i = 0; i < len;) {
                    int start = i;
                    char *p = body + i;
                    int n;
                    int vstart;
                    char *q;
                    int vlen;
                    while (*p != '=' && i < len) {
                        p++;
                        i++;
                    }
                    n = i - start;
                    i++;
                    if (n == 8) {
                        found = ov38_0222187C(body + start, ov38_02225E34, 8) == 0;
                    }
                    vstart = i;
                    q = body + i;
                    while (*q != '&' && *q != '\r' && *q != '\0' && i < len) {
                        q++;
                        i++;
                    }
                    vlen = i - vstart;
                    i++;
                    if (found) {
                        if (vlen == 4) {
                            int m = ov38_022202C0(dec, body + vstart, vlen);
                            dec[m] = '\0';
                            value = ov38_022218B4(dec, m);
                        }
                        break;
                    }
                }
                result = value < 0 ? 3 : 5;
            } else if (code <= 0 || code != 200) {
                result = 3;
            } else {
                switch (type) {
                case 1:
                    result = ov38_02221414(response, req) ? 0 : 3;
                    break;
                case 2:
                    result = ov38_02221494(response, req) ? 0 : 3;
                    break;
                case 3:
                    result = ov38_02221468(response, req) ? 0 : 3;
                    break;
                }
            }
        }
        break;
    case 8:
        result = 6;
        break;
    case 6:
        result = 4;
        break;
    default:
        result = 3;
        break;
    }

    switch (type) {
    case 1:
        break;
    case 2:
        if (req->numParam != NULL) {
            ov38_02226070(req->numParam);
        }
        if (req->offsetParam != NULL) {
            ov38_02226070(req->offsetParam);
        }
        ov38_02226070(req->buffer);
        break;
    case 3:
        ov38_02226070(req->output);
        break;
    }
    ov38_02221734();
    ov38_02226074 = -1;
    ov38_02221748();
    ov38_02222E04(response);
    ov38_02226070(req);
    ov38_022217D0();
    if (result != 0) {
        ov38_0222606C = result;
    }
    if (callback != NULL) {
        callback(type, result, value);
    }
    if (notify != 0) {
        // called without the queue argument
        ((void (*)(void))ov38_022217FC)();
    }
}

void ov38_02220B60(HttpEngineRequest *req, HttpRequest *request) {
    ov38_02222B1C(request);
    ov38_022217D0();
    ov38_02226070(req);
}

int ov38_02220B8C(HttpEngineRequest **outReq, HttpRequest **outRequest, const char *action, void *body, int bodyCapacity) {
    HttpEngineRequest *r;
    HttpRequest *d;
    if (!ov38_02220784(ov38_02226138, ov38_02226144)) {
        ov38_0222606C = 8;
        return 0;
    }
    r = ov38_0222605C(36, 4);
    if (!r) {
        ov38_0222606C = 1;
        return 0;
    }
    r->callback = ov38_02226068;
    r->notify = 0;
    if (!ov38_02221788()) {
        ov38_02226070(r);
        ov38_0222606C = 2;
        return 0;
    }
    d = ov38_022225B8(ov38_02226060, HTTP_SOCKET_METHOD_POST, body, bodyCapacity, (HttpSocketCallback)ov38_02220830, r);
    if (!d) {
        ov38_022217D0();
        ov38_02226070(r);
        ov38_0222606C = 3;
        return 0;
    }
    *outReq = r;
    *outRequest = d;
    if (!ov38_02221E10(d, (int)ov38_02225E20, 1)) {
        goto fail;
    }
    if (!ov38_02221D78(d, ov38_02225E40, ov38_022260D8)) {
        goto fail;
    }
    if (!ov38_02221D9C(d, ov38_02225E4C, ov38_02226084)) {
        goto fail;
    }
    if (!ov38_02221D9C(d, ov38_02225E54, ov38_02226138)) {
        goto fail;
    }
    if (!ov38_02221D9C(d, ov38_02225E60, ov38_022260BC)) {
        goto fail;
    }
    if (!ov38_02221D9C(d, ov38_02225E68, ov38_02226058)) {
        goto fail;
    }
    if (!ov38_02221D9C(d, ov38_02225E70, ov38_02226144)) {
        goto fail;
    }
    if (!ov38_02221D9C(d, ov38_02225E78, ov38_02226090)) {
        goto fail;
    }
    if (!ov38_02221D9C(d, ov38_02225E80, action)) {
        goto fail;
    }
    if (ov38_02226078[0] && !ov38_02221D9C(d, ov38_02225E88, ov38_02226078[0])) {
        goto fail;
    }
    if (ov38_02226078[1] && !ov38_02221D9C(d, ov38_02225E90, ov38_02226078[1])) {
        goto fail;
    }
    if (ov38_02226078[2] && !ov38_02221D9C(d, ov38_02225E98, ov38_02226078[2])) {
        goto fail;
    }
    if (ov38_02226054 && !ov38_02221D9C(d, ov38_02225EA0, ov38_022260A4)) {
        goto fail;
    }
    return 1;
fail:
    ov38_02220B60(r, d);
    ov38_0222606C = 1;
    return 0;
}

int ov38_02220EB8(HttpEngineRequest *req, HttpRequest *request) {
    int id;
    ov38_02221734();
    id = ov38_02222C10(request);
    req->jobId = id;
    ov38_02226074 = id;
    ov38_02221748();
    if (req->jobId >= 0) {
        return 1;
    }
    ov38_02220B60(req, request);
    ov38_0222606C = 3;
    return 0;
}

int ov38_02220F18(int *count) {
    HttpEngineRequest *req;
    HttpRequest *request;
    if (!ov38_02220B8C(&req, &request, ov38_02224ED8, ov38_0222615C, 128)) {
        return 0;
    }
    req->type = HTTP_ENGINE_REQUEST_COUNT;
    req->output = count;
    return ov38_02220EB8(req, request) != 0;
}

int ov38_02220F8C(HttpEngineFileInfo *list, int offset, int count) {
    int size = count << 8, n;
    char *buffer, *first, *second;
    HttpEngineRequest *r;
    HttpRequest *d;
    char number[12];
    if (size < 128) {
        size = 128;
    }
    buffer = ov38_0222605C(size, 4);
    if (!buffer) {
        ov38_0222606C = 1;
        return 0;
    }
    n = ov38_022218C0(number, offset);
    first = ov38_0222605C(ov38_022201AC(n) + 1, 4);
    if (!first) {
        ov38_02226070(buffer);
        ov38_0222606C = 1;
        return 0;
    }
    first[ov38_022201C8(first, number, n)] = 0;
    n = ov38_022218C0(number, count);
    second = ov38_0222605C(ov38_022201AC(n) + 1, 4);
    if (!second) {
        ov38_02226070(first);
        ov38_02226070(buffer);
        ov38_0222606C = 1;
        return 0;
    }
    second[ov38_022201C8(second, number, n)] = 0;
    if (!ov38_02220B8C(&r, &d, ov38_02224ECC, buffer, size)) {
        ov38_02226070(second);
        ov38_02226070(first);
        ov38_02226070(buffer);
        return 0;
    }
    if (ov38_02221D9C(d, ov38_02225EA8, first) && ov38_02221D9C(d, ov38_02225EB0, second)) {
        r->type = HTTP_ENGINE_REQUEST_LIST;
        r->output = list;
        r->offsetParam = first;
        r->numParam = second;
        r->size = count;
        r->buffer = buffer;
        if (ov38_02220EB8(r, d)) {
            return 1;
        }
        ov38_02226070(second);
        ov38_02226070(first);
        ov38_02226070(buffer);
        return 0;
    }
    ov38_02226070(second);
    ov38_02226070(first);
    ov38_02226070(buffer);
    ov38_02220B60(r, d);
    ov38_0222606C = 1;
    return 0;
}

int ov38_0222124C(HttpEngineFileInfo *info, void *out, u32 capacity) {
    int len, encoded;
    HttpEngineRequest *p;
    HttpRequest *q;
    char *b;
    if (capacity < (u32)info->size) {
        ov38_0222606C = 7;
        return 0;
    }
    len = ov38_02221870(info->name);
    if (len > 32) {
        ov38_0222606C = 7;
        return 0;
    }
    if (len <= 0) {
        encoded = 0;
    } else {
        encoded = ov38_022201AC(len);
    }
    b = ov38_0222605C(encoded + 1, 4);
    if (!b) {
        ov38_0222606C = 1;
        return 0;
    }
    if (!ov38_02220B8C(&p, &q, ov38_02224EE4, out, info->size)) {
        ov38_02226070(b);
        return 0;
    }
    if (encoded > 0) {
        b[ov38_022201C8(b, info->name, len)] = 0;
        if (ov38_02221D9C(q, ov38_02225EB4, b) == 0) {
            goto failed;
        }
    }
    p->type = HTTP_ENGINE_REQUEST_CONTENTS;
    p->output = b;
    p->size = info->size;
    if (ov38_02220EB8(p, q) != 0) {
        return 1;
    }
    ov38_02226070(b);
    return 0;
failed:
    ov38_02220B60(p, q);
    ov38_02226070(b);
    ov38_0222606C = 1;
    return 0;
}

int ov38_02221408(int *received, int *total) {
    return ov38_02223384(received, total);
}

int ov38_02221414(HttpResponse *response, HttpEngineRequest *req) {
    char *body[1];
    int n = ov38_0222308C(response, body);
    if (n >= 0) {
        *(int *)req->output = ov38_022218B4(body[0], n);
        return *(int *)req->output >= 0;
    }
    return 0;
}

int ov38_02221468(HttpResponse *response, HttpEngineRequest *req) {
    char *body;
    int size = ov38_0222308C(response, &body);
    return size == req->size;
}

// Parses the tab-separated file list (one line per file) into req->output.
int ov38_02221494(HttpResponse *response, HttpEngineRequest *req) {
    int result = 0, len, i, row, offset;
    char *text, *header;
    ov38_02221888(req->output, req->size * sizeof(HttpEngineFileInfo));
    if (ov38_02222F88(response, ov38_02225EC0, &header) > 0 && header[0] == '0' && header[1] == 0) {
        return 1;
    }
    len = ov38_0222308C(response, &text);
    if (len <= 0) {
        goto done;
    }
    i = 0;
    row = 0;
    if (req->size <= 0) {
        goto done;
    }
    offset = 0;
    do {
        int field, start, cr, n, value;
        HttpEngineFileInfo *rec;
        if (i >= len) {
            result = 1;
            goto done;
        }
        rec = (HttpEngineFileInfo *)((u8 *)req->output + offset);
        field = 0;
        start = i;
        cr = 0;
        result = 0;
        while (i < len) {
            int ch = text[i];
            if (ch == 13) {
                cr = 1;
                goto advance;
            }
            if (cr) {
                if (ch == 10) {
                    if (field == 5) {
                        n = i - 1 - start;
                        if (n > 8) {
                            goto done;
                        }
                        value = ov38_022218B4(text + start, n);
                        if (value < 0) {
                            goto done;
                        }
                        rec->size = value;
                    }
                    i++;
                    result = field >= 5;
                    break;
                }
            } else if (ch == 9) {
                switch (field) {
                case 0:
                    n = i - start;
                    if ((u32)n > 32) {
                        goto done;
                    }
                    ov38_0222189C(rec->name, text + start, n);
                    break;
                case 1:
                    n = i - start;
                    if ((u32)ov38_022201A0(n) > 102) {
                        goto done;
                    }
                    ov38_022202C0((char *)rec->unk_22, text + start, n);
                    rec->unk_22[50] = 0;
                    break;
                case 2:
                    n = i - start;
                    if ((u32)n > 10) {
                        goto done;
                    }
                    ov38_0222189C(rec->unk_88, text + start, n);
                    break;
                case 3:
                    n = i - start;
                    if ((u32)n > 10) {
                        goto done;
                    }
                    ov38_0222189C(rec->unk_93, text + start, n);
                    break;
                case 4:
                    n = i - start;
                    if ((u32)n > 10) {
                        goto done;
                    }
                    ov38_0222189C(rec->unk_9E, text + start, n);
                    break;
                case 5:
                    n = i - start;
                    if (n > 8) {
                        goto done;
                    }
                    value = ov38_022218B4(text + start, n);
                    if (value < 0) {
                        goto done;
                    }
                    rec->size = value;
                    break;
                }
                field++;
                start = i + 1;
            }
            cr = 0;
advance:
            i++;
        }
        row++;
        offset += sizeof(HttpEngineFileInfo);
    } while (row < req->size);
done:
    return result;
}

int ov38_02221718(void) {
    OS_InitMutex(&ov38_022261F8);
    return 1;
}

void ov38_02221730(void) {
}

void ov38_02221734(void) {
    OS_LockMutex(&ov38_022261F8);
}

void ov38_02221748(void) {
    OS_UnlockMutex(&ov38_022261F8);
}

int ov38_0222175C(void) {
    OS_InitMutex(&ov38_022261E0);
    ov38_022261DC = 0;
    return 1;
}

void ov38_02221784(void) {
}

int ov38_02221788(void) {
    int ok;
    OS_LockMutex(&ov38_022261E0);
    ok = ov38_022261DC == 0;
    if (ok) {
        ov38_022261DC = 1;
    }
    OS_UnlockMutex(&ov38_022261E0);
    return ok;
}

void ov38_022217D0(void) {
    OS_LockMutex(&ov38_022261E0);
    ov38_022261DC = 0;
    OS_UnlockMutex(&ov38_022261E0);
}

int ov38_022217FC(OSMessageQueue *queue) {
    return OS_SendMessage(queue, 0, 0);
}

// Writes the MAC address as 12 hex digits.
int ov38_02221810(char *out) {
    u8 mac[6];
    char b[12];
    int i;
    OS_GetMacAddress(mac);
    for (i = 0; i < 6; i++) {
        ov38_022219A4(b + i * 2, mac[i], 2);
    }
    return ov38_022201C8(out, b, 12);
}

u32 ov38_02221870(const char *s) {
    return STD_GetStringLength(s);
}

int ov38_0222187C(const char *a, const char *b, u32 n) {
    return STD_CompareNString(a, b, n);
}

void ov38_02221888(void *p, int n) {
    MI_CpuFill8(p, 0, n);
}

void ov38_0222189C(void *p, const void *s, int n) {
    MI_CpuCopy8(s, p, n);
}

int ov38_022218B4(char *p, int n) {
    return ov38_02224B80(p, n);
}

int ov38_022218C0(char *p, int n) {
    return ov38_02224C18(p, n);
}

// Writes val in decimal, zero-padded to width digits; returns the length, -1 if val >= 10^17.
int ov38_022218CC(char *buf, u64 val, int width) {
    int i;
    int len = 0;
    BOOL started;
    if (val >= 100000000000000000ULL) {
        return len - 1;
    }
    i = 0;
    started = FALSE;
    for (; i < 16; i++) {
        u64 p = ov38_02224EF4[i];
        if (val >= p) {
            u32 q = val / p;
            val -= q * p;
            buf[len] = q + '0';
            started = TRUE;
            len++;
        } else if (started || 17 - i <= width) {
            buf[len] = '0';
            len++;
        }
    }
    buf[len] = val + '0';
    return len + 1;
}

// Writes the low n hex digits of val (lowercase).
int ov38_022219A4(char *buf, u32 val, int n) {
    int len = 0;
    int i;
    if (n > 8) {
        return -1;
    }
    for (i = 1; i <= n; i++) {
        u32 d = (val >> ((n - i) * 4)) & 0xF;
        if (d < 10) {
            buf[len++] = d + '0';
        } else {
            buf[len++] = d + 'a' - 10;
        }
    }
    return len;
}
