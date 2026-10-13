#pragma force_active on
#include <string.h>

#include "global.h"

#include "dwc/auth_internal.h"
#include "dwc/net_common_internal.h"

// The code below is compiled as ARM (32-bit) code.
#include <nitro/code32.h>

#pragma warn_unusedvar off
#pragma warn_uninitializedvar off

extern signed char *ov00_02216BA8; // encoding alphabet
extern char ov00_02217938[];
extern char ov00_02217960[];
extern char ov00_02217984[];
extern char ov00_022179A4[];
extern const u8 ov00_022179AC[];
extern char ov00_022179B0[];
extern char ov00_022179C4[];
extern char ov00_022179D8[];
extern char ov00_022179E0[];
extern NetErrorState ov00_0221A74C;
extern NetAllocator ov00_0221A754;
extern NetConnectionState ov00_0221A75C;
extern NetAuthWork *ov00_0221A764;
extern NetDownloadWork *ov00_0221A768;
extern int ov00_0221A734;
extern s64 ov00_0221A738;
extern s64 ov00_0221A740;

extern RTCResult RTC_GetDateTime(RTCDate *, RTCTime *);
extern int strncmp(const char *, const char *, u32);
extern char *strchr(const char *, int);
extern int OS_SNPrintf(char *, u32, const char *, ...);
extern void OS_GetMacAddress(u8 *mac);
extern u64 OS_GetTick(void);
extern void CPS_SetSslHandshakePriority(u16);
extern int CPS_GetSslHandshakePriority(void);
extern int WCM_GetLinkLevel(void);
extern int WCM_GetPhase(void);
extern u8 *WCM_GetApMacAddress(void);
extern u8 *WCM_GetApEssid(u16 *);
extern int DWC_AC_Create(NetStartParams *);
extern int DWC_AC_Process(void);
extern s32 DWC_AC_GetStatus(void);
extern int DWC_AC_GetApType(void);
extern int DWC_AC_GetApSpotInfo(void *);
extern int DWC_AC_Destroy(void);
extern int DWCi_AC_SetSpecifiedApEx(void *, void *, u32, void *, int);
extern void DWCi_Beacon_StartApSearch(void *);
extern int DWCi_Beacon_ProcessApSearch(void);
extern void DWCi_Beacon_FreeApSearch(void);
extern int ov38_02220090(int, int, int);
extern int ov38_02220434(void *(*)(u32, int), void (*)(void *), char *, char *, char *, char *, int);
extern void ov38_02220620(void (*)(void));
extern int ov38_02220648(void);
extern int ov38_02220F18(int);
extern int ov38_02220F8C(void *, int, int);
extern int ov38_0222124C(int, int, int);
extern int ov38_02221408(int, int);

// Encodes len bytes in 4-character groups of 6 bits each ('*' pads); returns the length, or with
// dest == NULL the length needed.
int ov00_021EBE08(const u8 *src, u32 len, char *dest, u32 destSize) {
    int count;
    const u8 *next, *end;
    char *nextOut;
    char *out;

    struct {
        u8 first, second, third;
    } in;

    u32 outLen = (len / 3) * 4 + (len % 3 ? 4 : 0);
    if (!dest) {
        return outLen;
    }
    if (destSize < outLen) {
        return -1;
    }
    next = src;
    end = src + len;
    nextOut = dest;
    while (next != end) {
        int rem = end - next;
        int bits = rem * 8;
        count = bits / 6 + (bits % 6 != 0);
        if (rem >= 3) {
            rem = 3;
        }
        MI_CpuFill8((void *)&in, 0, 3);
        out = nextOut;
        MI_CpuCopy8(next, (void *)&in, rem);
        out[0] = ov00_02216BA8[in.first >> 2];
        if (count < 2) {
            out[1] = '*';
        } else {
            int a = in.first, b = in.second;
            out[1] = ov00_02216BA8[((a << 4) & 63) | (b >> 4)];
        }
        if (count < 3) {
            out[2] = '*';
        } else {
            int a = in.second, b = in.third;
            out[2] = ov00_02216BA8[((a << 2) & 63) | (b >> 6)];
        }
        if (count < 4) {
            out[3] = '*';
        } else {
            int a = in.third;
            out[3] = ov00_02216BA8[a & 63];
        }
        next += rem;
        nextOut += 4;
    }
    return nextOut - dest;
}

// Decodes ov00_021EBE08's encoding.
u32 ov00_021EBF90(char *src, u32 len, char *dest, u32 destSize) {
    int bits, bytes, count;
    u32 i;
    char *dst;
    char tmp[4];
    char *q;
    int j;
    if (len & 3) {
        return -1;
    }
    bits = 0;
    for (i = 0; i < len; i++) {
        if (src[i] != '*') {
            bits += 6;
        }
    }
    bytes = bits / 8;
    if (!dest) {
        return bytes;
    }
    if (destSize < (u32)bytes) {
        return -1;
    }
    if (!len) {
        *dest = 0;
        return 0;
    }
    dst = dest;
    do {
        for (j = 0, q = tmp; j < 4; j++, q++) {
            int c = src[j];
            if (c >= 'A' && c <= 'Z') {
                *q = c - 'A';
            } else if (c >= 'a' && c <= 'z') {
                *q = c - 'G';
            } else if (c >= '0' && c <= '9') {
                *q = c + 4;
            } else if (c == '.') {
                *q = 62;
            } else if (c == '-') {
                *q = 63;
            } else {
                *q = 0;
            }
        }
        dst[0] = (tmp[0] << 2) | (tmp[1] >> 4);
        src += 4;
        count = dst + 1 - dest;
        if (count >= bytes) {
            break;
        }
        dst[1] = (tmp[1] << 4) | (tmp[2] >> 2);
        count = dst + 2 - dest;
        if (count >= bytes) {
            break;
        }
        dst[2] = (tmp[2] << 6) | tmp[3];
        dst += 3;
        count = dst - dest;
    } while (count < bytes);
    return count;
}

int ov00_021EC0FC(int *code) {
    if (code) {
        *code = ov00_0221A74C.code;
    }
    return ov00_0221A74C.type;
}

int ov00_021EC11C(int *code, int *category) {
    if (code) {
        *code = ov00_0221A74C.code;
    }
    if (category) {
        switch ((u32)ov00_0221A74C.type) {
        case 2:
        case 3:
        case 4:
        case 5:
        case 8:
            *category = 6;
            break;
        case 6:
            *category = 3;
            break;
        case 7:
            *category = 4;
            break;
        case 10:
        case 11:
        case 12:
        case 13:
            *category = 1;
            break;
        case 1:
        case 9:
            *category = 7;
            break;
        case 14:
            *category = 5;
            break;
        case 15:
        case 17:
        case 19:
            *category = 6;
            break;
        case 16:
        case 18:
            *category = 2;
            break;
        default:
            *category = 0;
            break;
        }
    }
    return ov00_0221A74C.type;
}

void ov00_021EC210(void) {
    if (ov00_0221A74C.type != 9) {
        ov00_0221A74C.type = 0;
        ov00_0221A74C.code = 0;
    }
}

int ov00_021EC230(void) {
    return ov00_0221A74C.type != 0;
}

// Records an error; type 9 is fatal and is never overwritten.
void ov00_021EC24C(int type, int code) {
    if (ov00_0221A74C.type != 9) {
        ov00_0221A74C.type = type;
        ov00_0221A74C.code = code;
    }
}

NetAllocHeader *ov00_021EC264(void *p) {
    return (NetAllocHeader *)((u8 *)p - 32);
}

void *ov00_021EC26C(NetAllocHeader *p, u32 size) {
    p->magic = 0x4457434d;
    p->size = size;
    return (u8 *)p + 32;
}

u32 ov00_021EC284(void *p) {
    return ov00_021EC264(p)->size;
}

void ov00_021EC294(void *(*alloc)(int, u32, int), void (*free)(int, void *, u32)) {
    ov00_0221A754.alloc = alloc;
    ov00_0221A754.free = free;
}

void *ov00_021EC2A8(int heap, u32 size) {
    return ov00_021EC2B8(heap, size, 32);
}

void *ov00_021EC2B8(int heap, u32 size, int align) {
    NetAllocHeader *p = ov00_0221A754.alloc(heap, size + 32, align);
    if (p == 0) {
        return 0;
    }
    return ov00_021EC26C(p, size);
}

void ov00_021EC2EC(int heap, void *p, u32 size) {
    NetAllocHeader *h;
    if (p == 0) {
        return;
    }
    h = ov00_021EC264(p);
    ov00_0221A754.free(heap, h, h->size + 32);
}

void *ov00_021EC328(int heap, void *p, u32 oldSize, u32 size) {
    return ov00_021EC33C(heap, p, oldSize, size, 32);
}

// Reallocation: copies min(old, new) bytes and frees src.
void *ov00_021EC33C(int heap, void *src, u32 oldSize, u32 size, int align) {
    void *buf = ov00_021EC2B8(heap, size, align);
    if (buf == NULL) {
        return NULL;
    }
    if (src != NULL) {
        oldSize = ov00_021EC284(src);
        if (oldSize <= size) {
            size = oldSize;
        }
        MI_CpuCopy8(src, buf, size);
        ov00_021EC2EC(heap, src, oldSize);
    }
    return buf;
}

void *ov00_021EC3A8(u32 size) {
    return ov00_021EC2A8(5, size);
}

void *ov00_021EC3BC(void *p, u32 size) {
    return ov00_021EC328(5, p, size, size);
}

void ov00_021EC3D8(void *p) {
    ov00_021EC2EC(5, p, 0);
}

void ov00_021EC3F0(NetConnection *connection, u16 priority, int unused, u16 sslPriority) {
    if (ov00_0221A75C.connection) {
        return;
    }
    MI_CpuFill8(connection, 0, 100);
    connection->priority = priority;
    connection->one = 1;
    connection->state = 1;
    connection->flag = 0;
    connection->unk_0C = 0;
    connection->phase = 0;
    ov00_0221A75C.connection = connection;
    CPS_SetSslHandshakePriority(sslPriority);
}

// Selects the auth server: 0 test, 1 development, 2 production.
void ov00_021EC454(int server) {
    switch (server) {
    case 0:
        ov00_021E7B60(ov00_02217938);
        break;
    case 1:
        ov00_021E7B60(ov00_02217960);
        break;
    case 2:
        ov00_021E7B60(ov00_02217984);
        break;
    }
}

void ov00_021EC4A4(void) {
    NetStartParams p;
    if (ov00_0221A75C.connection) {
        if (ov00_0221A75C.connection->state != 1) {
            return;
        }
        MI_CpuFill8(&p, 0, 12);
        p.priority = ov00_0221A75C.connection->priority;
        p.one = ov00_0221A75C.connection->one;
        p.alloc = ov00_021EC2A8;
        p.free = ov00_021EC2EC;
        ov00_0221A75C.connection->state = 2;
        if (DWC_AC_Create(&p)) {
            return;
        }
        ov00_021EC24C(9, -50199);
    } else {
        ov00_021EC24C(9, -50199);
    }
}

void ov00_021EC558(void *a, void *b, u32 c, void *d, int e) {
    ov00_021EC4A4();
    if (ov00_0221A75C.connection) {
        ov00_0221A75C.connection->unk_0C = 1;
    }
    if (ov00_021EC724() != 2) {
        return;
    }
    DWCi_AC_SetSpecifiedApEx(a, b, c, d, e);
}

int ov00_021EC5B4(void) {
    if (!ov00_0221A75C.connection) {
        return 0;
    }
    if (ov00_0221A75C.connection->handle) {
        ov00_0221A75C.connection->state = 3;
        ov00_021EC724();
        return ov00_0221A75C.connection->unk_0C != 0;
    }
    return 0;
}

void ov00_021EC60C(void) {
    if (!ov00_0221A75C.connection) {
        return;
    }
    switch (ov00_0221A75C.connection->phase) {
    case 1:
        ov00_0221A75C.connection->handle = 0;
        ov00_0221A75C.connection->phase = DWCi_Beacon_ProcessApSearch();
        return;
    case 2:
        DWCi_Beacon_FreeApSearch();
        ov00_021EC558(ov00_0221A75C.connection->unk_14, ov00_0221A75C.connection->unk_34, ov00_0221A75C.connection->unk_54, ov00_0221A75C.connection->unk_58, 8);
        ov00_0221A75C.connection->phase = 0;
        return;
    case 3:
        DWCi_Beacon_FreeApSearch();
        ov00_0221A75C.connection->handle = -1;
        ov00_0221A75C.connection->unk_0C = 1;
        return;
    case 4:
        return;
    default:
        break;
    }
    if (ov00_0221A75C.connection->state == 2) {
        ov00_0221A75C.connection->handle = DWC_AC_Process();
        return;
    }
    if (ov00_0221A75C.connection->state != 4) {
        return;
    }
    if (!ov00_0221A75C.connection->flag) {
        return;
    }
    if (WCM_GetPhase() == 9) {
        return;
    }
    ov00_0221A75C.connection->flag = 0;
    ov00_0221A75C.connection->state = 6;
}

int ov00_021EC724(void) {
    s32 result;
    if (!ov00_0221A75C.connection) {
        return 0;
    }
    switch (ov00_0221A75C.connection->phase) {
    case 1:
        return 2;
    case 2:
        return 2;
    case 3:
        ov00_021EC60C();
        ov00_021EC24C(5, ov00_0221A75C.error);
        ov00_0221A75C.connection->state = 7;
        ov00_0221A75C.connection->phase = 4;
        return 7;
    case 4:
        return 7;
    }
    {
        u16 state = ov00_0221A75C.connection->state;
        if (state == 1) {
            return state;
        }
    }
    result = DWC_AC_GetStatus();
    if (result == 5) {
        ov00_0221A75C.connection->state = 4;
        ov00_0221A75C.connection->flag = 1;
        ov00_0221A75C.connection->unk_0C = 1;
        return 4;
    }
    if (result < 0) {
        if (result >= -10) {
            ov00_021EC24C(9, (s32)0xffff3be9);
            ov00_0221A75C.connection->state = 8;
            return 8;
        }
        if (ov00_0221A75C.connection->unk_0C == 0) {
            NetConnection *connection = ov00_0221A75C.connection;
            u16 priority = connection->priority;
            int handshake = CPS_GetSslHandshakePriority();
            ov00_0221A75C.error = result;
            ov00_021EC8D8();
            ov00_021EC3F0(connection, priority, 1, (u16)handshake);
            DWCi_Beacon_StartApSearch(ov00_0221A75C.connection->unk_14);
            ov00_0221A75C.connection->phase = 1;
            ov00_0221A75C.connection->handle = 0;
            return 2;
        }
        if (ov00_0221A75C.error < result) {
            result = ov00_0221A75C.error;
        }
        ov00_021EC24C(5, result);
        ov00_0221A75C.connection->state = 7;
        return 7;
    }
    return 2;
}

void ov00_021EC8D8(void) {
    if (!ov00_0221A75C.connection) {
        return;
    }
    if (ov00_0221A75C.connection->state == 1) {
        ov00_0221A75C.connection = 0;
        return;
    }
    if (DWC_AC_Destroy() == 0) {
        do {
            OS_Sleep(10);
        } while (DWC_AC_Destroy() == 0);
    }
    ov00_0221A75C.connection = 0;
}

int ov00_021EC938(void) {
    if (!ov00_0221A75C.connection) {
        return 1;
    }
    if (ov00_0221A75C.connection->state == 8) {
        return 0;
    }
    if (ov00_0221A75C.connection->state == 1) {
        ov00_0221A75C.connection = 0;
        return 1;
    }
    ov00_0221A75C.connection->state = 5;
    if (DWC_AC_Destroy() == 0) {
        return 0;
    }
    ov00_0221A75C.connection = 0;
    return 1;
}

int ov00_021EC9A8(void) {
    if (ov00_0221A75C.connection && ov00_0221A75C.connection->state == 6) {
        return 1;
    }
    return 0;
}

int ov00_021EC9D4(void) {
    return WCM_GetLinkLevel();
}

static inline int selectType(int flag, u8 ch) {
    return flag ? ch - 48 : 255;
}

int ov00_021EC9E0(NetApInfo *info) {
    struct {
        u16 len;
        u8 id[10];
    } temp;

    OSIntrMode irq;
    u8 *p;
    BOOL digit;
    u8 ch;
    MI_CpuFill8(info, 0, 60);
    info->spotType = 255;
    if (ov00_021EC724() != 4) {
        return 0;
    }
    info->apType = DWC_AC_GetApType();
    if (info->apType == 255 || info->apType > 99) {
        info->apType = 99;
        return 0;
    }
    irq = OS_DisableInterrupts();
    p = WCM_GetApMacAddress();
    DC_InvalidateRange(p, 6);
    if (!p) {
        OS_RestoreInterrupts(irq);
        return 0;
    }
    MI_CpuCopy8(p, info->bssid, 6);
    if (info->apType >= 0 && info->apType < 3) {
        p = WCM_GetApEssid(&temp.len);
        DC_InvalidateRange(p, 32);
        if (!p) {
            OS_RestoreInterrupts(irq);
            return 0;
        }
        MI_CpuCopy8(p, info->essid, temp.len);
    }
    OS_RestoreInterrupts(irq);
    if (DWC_AC_GetApSpotInfo(temp.id)) {
        digit = FALSE;
        ch = temp.id[0];
        if (ch < 48) {
            goto selected;
        }
        if (ch <= 57) {
            digit = TRUE;
        }
selected:
        info->spotType = selectType(digit, ch);
        MI_CpuCopy8(temp.id + 1, info->spotInfo, 9);
    } else {
        info->spotType = 255;
    }
    return 1;
}

int ov00_021ECB40(void) {
    unsigned n;
    ov00_021EC60C();
    if (ov00_021EC9A8()) {
        n = DWC_AC_GetApType();
        if (n == 255 || n > 99) {
            n = 99;
        }
        ov00_021EC24C(8, -54000 - n);
        return 1;
    }
    return 0;
}

// Current date and time, corrected by the server date read by ov00_021EB5B0.
int ov00_021ECB94(RTCDate *date, RTCTime *time) {
    s64 n;
    if (!ov00_0221A734) {
        return 0;
    }
    if (RTC_GetDateTime(date, time) != 0) {
        return 0;
    }
    n = RTC_ConvertDateTimeToSecond(date, time);
    if (n == -1) {
        return 0;
    }
    if (n < ov00_0221A738) {
        n += 3155760000LL;
    }
    n -= ov00_0221A740;
    if (n < 0 || n > 3155759999LL) {
        return 0;
    }
    RTC_ConvertSecondToDateTime(date, time, n);
    return 1;
}

int ov00_021ECC6C(const char *svc, SvcLocResult *output) {
    if (ov00_021EB7CC((NasAllocFunc)ov00_021EC2A8, (NasFreeFunc)ov00_021EC2EC) == 0) {
        return 0;
    }
    {
        int ret;
        if (strlen(svc) == 0) {
            ret = ov00_021EB8B0(ov00_022179A4, output);
        } else {
            ret = ov00_021EB8B0(svc, output);
        }
        if (ret) {
            return 1;
        }
    }
    ov00_021EB864();
    return 0;
}

int ov00_021ECCE0(void) {
    int n = ov00_021EBB18();
    if ((unsigned)(n - 3) <= 2) {
        ov00_021EB864();
    }
    return n;
}

int ov00_021ECD04(void) {
    void *allocated;
    if (ov00_0221A764) {
        OS_Terminate();
    }
    allocated = ov00_021EC2A8(4, 7712);
    ov00_0221A764 = allocated;
    if (!allocated) {
        ov00_021EC24C(9, -20100);
        return 0;
    }
    MI_CpuFill8(allocated, 0, 7712);
    MI_CpuCopy8(ov00_022179AC, ov00_0221A764->params.gsbrcd, 9);
    ov00_0221A764->params.alloc = (NasAllocFunc)ov00_021EC2A8;
    ov00_0221A764->params.free = (NasFreeFunc)ov00_021EC2EC;
    ov00_021E7B70(&ov00_0221A764->params, &ov00_0221A764->http);
    return 1;
}

int ov00_021ECDC8(void) {
    AuthResult result;
    if (!ov00_0221A764) {
        return 0;
    }
    switch (ov00_021E7E5C()) {
    case 0:
        return 2;
    case 21:
        ov00_021E7DD4();
        ov00_021EC2EC(4, ov00_0221A764, 7712);
        ov00_0221A764 = 0;
        return 3;
    case 20:
        ov00_021E7DD4();
        ov00_021EC2EC(4, ov00_0221A764, 7712);
        ov00_0221A764 = 0;
        return 5;
    default:
        ov00_021E7EA8(&result);
        ov00_021E7DD4();
        ov00_021EC2EC(4, ov00_0221A764, 7712);
        ov00_0221A764 = 0;
        ov00_021EC24C(2, result.error);
        return 4;
    }
}

void ov00_021ECEC0(void) {
    ov00_021E7D48();
}

void *ov00_021ECECC(u32 size, int align) {
    return ov00_021EC2B8(6, size, align);
}

void ov00_021ECEE4(void *p) {
    ov00_021EC2EC(6, p, 0);
}

void ov00_021ECEFC(int a, unsigned result, int c) {
    if (result == 0) {
        ov00_0221A768->callback(a, result, c);
        return;
    }
    switch (result) {
    case 1:
        ov00_021EC24C(9, -31001);
        break;
    case 2:
        OS_Terminate();
        break;
    case 3:
        ov00_021EC24C(15, -31020);
        break;
    case 4:
        ov00_021EC24C(14, -31030);
        break;
    case 5:
        ov00_021EC24C(14, -31000 - c);
        break;
    case 6:
        break;
    case 7:
        OS_Terminate();
        break;
    case 8:
        ov00_021EC24C(9, -31009);
        break;
    default:
        OS_Terminate();
        break;
    }
    ov00_0221A768->callback(a, result, c);
}

void ov00_021ECFF4(void) {
    void (*end)(void);
    OS_JoinThread(&ov00_0221A768->thread);
    end = ov00_0221A768->end;
    ov00_021ECEE4(ov00_0221A768);
    ov00_0221A768 = 0;
    if (end) {
        end();
    }
}

// Download thread: waits for the service locator, then connects to "https://<svchost>/download".
void ov00_021ED038(void *arg) {
    struct Frame {
        int value;
        char token[400], url[256];
    } f;

    int status;
    NetDownloadWork *s;
    for (;;) {
        status = ov00_021ECCE0();
        if (status == 3) {
            s = ov00_0221A768;
            if (strlen(s->svcloc.svchost) + strlen(ov00_022179B0) > 255) {
                ov00_021ECEFC(0, 8, -1);
                return;
            }
            OS_SNPrintf(f.url, 256, ov00_022179C4, s->svcloc.svchost);
            s = ov00_0221A768;
            if (ov00_021EBE08((u8 *)s->svcloc.servicetoken, strlen(s->svcloc.servicetoken), f.token, 400) < 0) {
                ov00_021ECEFC(0, 8, -1);
                return;
            }
            s = ov00_0221A768;
            if (!ov38_02220434(ov00_021ECECC, ov00_021ECEE4, f.url, f.token, s->user, s->pass, (int)ov00_021ECEFC)) {
                ov00_021ECEFC(0, 8, -1);
                return;
            }
            ov00_0221A768->active = 1;
            ov00_021ECEFC(0, 0, -1);
            return;
        } else if (status == 4) {
            ov00_021EC0FC(&f.value);
            ov00_021EC24C(14, f.value);
            ov00_0221A768->callback(0, 3, -1);
            return;
        } else if (status == 5) {
            ov00_021ECEFC(0, 6, -1);
        }
        OS_Sleep(100);
    }
}

int ov00_021ED1F0(NetDownloadCallback callback, char *user, char *pass) {
    if (ov00_021EC230() != 0) {
        return 0;
    }
    ov00_0221A768 = ov00_021ECECC(2688, 32);
    ov00_0221A768->active = 0;
    MI_CpuFill8(ov00_0221A768, 0, 2688);
    ov00_0221A768->callback = callback;
    strncpy(ov00_0221A768->user, user, 31);
    strncpy(ov00_0221A768->pass, pass, 31);
    if (ov00_021ECC6C(ov00_022179D8, &ov00_0221A768->svcloc) == 0) {
        ov00_021ECEE4(ov00_0221A768);
        ov00_0221A768 = 0;
        return 0;
    }
    OS_CreateThread(&ov00_0221A768->thread, ov00_021ED038, 0, &ov00_0221A768->thread, 2048, 16);
    OS_WakeupThreadDirect(&ov00_0221A768->thread);
    return 1;
}

int ov00_021ED308(void (*end)(void)) {
    if (!ov00_0221A768) {
        return 0;
    }
    ov00_0221A768->end = end;
    if (ov00_0221A768->active == 0) {
        ov00_021ECFF4();
    } else {
        ov38_02220620(ov00_021ECFF4);
    }
    return 1;
}

int ov00_021ED354(int a, int b, int c) {
    if (ov00_021EC230()) {
        return 0;
    }
    return ov38_02220090(a, b, c);
}

int ov00_021ED388(int a) {
    if (ov00_021EC230()) {
        return 0;
    }
    return ov38_02220F18(a);
}

int ov00_021ED3AC(void *a, int b, int c) {
    if (ov00_021EC230()) {
        return 0;
    }
    MI_CpuFill8(a, 0, c * 176);
    return ov38_02220F8C(a, b, c);
}

int ov00_021ED3F4(int a, int b, int c) {
    if (ov00_021EC230()) {
        return 0;
    }
    return ov38_0222124C(a, b, c);
}

int ov00_021ED428(void) {
    if (ov00_021EC230()) {
        return 0;
    }
    return ov38_02220648();
}

int ov00_021ED444(int a, int b) {
    if (ov00_021EC230()) {
        return 0;
    }
    return ov38_02221408(a, b);
}

u32 ov00_021ED470(const char *a, const char *b, char *dest, int separator) {
    OS_SNPrintf(dest, 4096, ov00_022179E0, separator, a, separator, b);
    return strlen(dest);
}

u32 ov00_021ED4A8(const char *a, const char *b, char *dest, int separator) {
    ov00_021ED470(a, b, strchr(dest, 0), separator);
    return strlen(dest);
}

// Looks up key in "<sep>key<sep>value<sep>key<sep>value..." and copies its value to out.
int ov00_021ED4E8(char *key, char *out, char *input, int separator) {
    char *p, *q;
    int length;
    if (out == 0) {
        return -1;
    }
    p = strchr(input, separator);
    if (p == 0) {
        return -1;
    }
    for (;;) {
        if (strncmp(p + 1, key, strlen(key)) == 0 && *((char *)strlen(key) + (u32)p + 1) == separator) {
            break;
        }
        q = strchr(p + 1, separator);
        if (q == 0) {
            return -1;
        }
        p = strchr(q + 1, separator);
        if (p == 0) {
            return -1;
        }
    }
    p = strchr(p + 1, separator);
    if (p == 0) {
        return -1;
    }
    q = strchr(p + 1, separator);
    if (q != 0) {
        length = q - (p + 1);
    } else {
        length = strlen(p + 1);
    }
    strncpy(out, p + 1, length);
    out[length] = 0;
    return length;
}

NetRandom ov00_0221A76C;

// Linear congruential random number in [0, max) (max == 0: the full upper word), seeded from the
// MAC address and the tick count.
u32 ov00_021ED5E4(u32 max) {
    if (ov00_0221A76C.x == 0 && ov00_0221A76C.mul == 0 && ov00_0221A76C.add == 0) {
        u64 seed;
        OS_GetMacAddress((u8 *)&seed);
        seed = ((seed >> 24) & 0xffffff) | (OS_GetTick() << 24);
        ov00_0221A76C.x = seed;
        ov00_0221A76C.mul = 0x5D588B656C078965ULL;
        ov00_0221A76C.add = 0x269EC3;
    }
    ov00_0221A76C.x = ov00_0221A76C.mul * ov00_0221A76C.x + ov00_0221A76C.add;
    if (max == 0) {
        return (u32)(ov00_0221A76C.x >> 32);
    }
    return (u32)(((ov00_0221A76C.x >> 32) * max) >> 32);
}
