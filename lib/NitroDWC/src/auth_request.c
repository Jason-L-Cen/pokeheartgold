#pragma force_active on
#include <string.h>

#include "global.h"

#include "dwc/auth_internal.h"
#include "dwc/net_common_internal.h"

// The code below is compiled as ARM (32-bit) code.
#include <nitro/code32.h>

typedef struct AuthDate {
    int year;
    int month;
    int day;
    int week;
} AuthDate;

typedef struct AuthTime {
    int hour;
    int minute;
    int second;
} AuthTime;

// ov00_021E89AC reads the WiFi info as words.
typedef struct AuthWiFiInfoWords {
    u32 userIdLo;
    u32 userIdHi;
    volatile u32 unk_08;
    volatile u32 unk_0C;
    u16 password;
} AuthWiFiInfoWords;

typedef struct HexChars {
    char byte[2];
} HexChars;

extern int errno;
extern u8 ov00_0221A6A0[];
extern u32 ov00_0221A6BC;
extern char ov00_022164A4[];
extern char ov00_022164B4[];
extern char ov00_022164C0[];
extern char ov00_022164C8[];
extern char ov00_022164D4[];
extern char ov00_022164E0[];
extern char ov00_022164E8[];
extern char ov00_022164F0[];
extern char ov00_022164FC[];
extern char ov00_02216508[];
extern char ov00_02216514[];
extern char ov00_02216520[];
extern char ov00_02216528[];
extern char ov00_02216530[];
extern char ov00_02216538[];
extern char ov00_0221653C[];
extern char ov00_02216544[];
extern char ov00_02216550[];
extern char ov00_02216558[];
extern char ov00_02216560[];
extern char ov00_02216568[];
extern char ov00_02216574[];
extern char ov00_02216590[];
extern char ov00_022165A0[];
extern char ov00_022165AC[];
extern char ov00_022165B4[];
extern char ov00_022165BC[];
extern char ov00_022165C4[];
extern char ov00_022165CC[];
extern char ov00_022165D4[];
extern char ov00_022165DC[];
extern char ov00_022165E4[];
extern char ov00_022165EC[];
extern char ov00_022165F4[];
extern char ov00_022165FC[];
extern char ov00_02216604[];
extern char ov00_0221660C[];
extern char ov00_02216614[];
extern char ov00_0221661C[];
extern char ov00_02216634[];
extern char ov00_02216640[];
extern char ov00_02216680[];
extern char ov00_02216698[];

extern int DWCi_AUTH_UpDateWiFiID(void *, void *);
extern void DWCi_AUTH_RemakeWiFiID(void *);
extern void DWCi_AUTH_MakeWiFiID(void *);
extern void DWCi_BM_GetWiFiInfo(void *);
extern void OS_GetMacAddress(u8 *);
extern void *WCM_GetApMacAddress(void);
extern void *WCM_GetApEssid(u16 *);
extern RTCResult RTC_GetDateTime(RTCDate *, RTCTime *);
extern long atol(const char *s);
extern long strtol(const char *s, char **end, int base);
extern u32 wcslen(const u16 *s);
extern int ov00_021FAA40(void);
extern void ov00_021FAA70(char *);

// Auth thread: waits for the server request, retries up to three times.
void ov00_021E8048(void *arg) {
    int retries = 0;
    for (;;) {
        int restart, status;
        OSTick start;
        if (ov00_0221A694.work->request->thread.id) {
            OS_JoinThread(&ov00_0221A694.work->request->thread);
        }
        if (ov00_0221A694.work->request->result != 8) {
            ov00_0221A694.work->response.error = 20100;
            status = ov00_0221A694.work->request->result;
            if (status == 7) {
                ov00_021E8940(20);
                return;
            }
            if (retries > 2) {
                if (status == 2) {
                    ov00_021E8940(9);
                    return;
                }
                if (status == 3) {
                    ov00_021E8940(11);
                    return;
                }
                ov00_021E8940(13);
                return;
            }
            restart = 1;
            retries++;
        } else {
            status = ov00_021E82FC();
            switch (status) {
            case 21:
                ov00_021E8940(21);
                return;
            case 16:
                if (retries > 2) {
                    ov00_021E8940(16);
                    return;
                }
                restart = 0;
                retries++;
                break;
            case 17:
                ov00_021E8940(status);
                return;
            default:
                if (retries >= 2) {
                    ov00_021E8940(status);
                    return;
                }
                restart = 1;
                retries++;
                break;
            }
        }
        start = OS_GetTick();
        while (((OS_GetTick() - start) << 6) / 33514 < 5000) {
            OS_LockMutex(&ov00_0221A694.work->mutex);
            if (ov00_0221A694.work->cancelled == 1) {
                ov00_0221A694.work->response.error = 20100;
                OS_UnlockMutex(&ov00_0221A694.work->mutex);
                ov00_021E8940(20);
                return;
            }
            OS_UnlockMutex(&ov00_0221A694.work->mutex);
            OS_Sleep(5000);
        }
        ov00_021E9754(ov00_0221A694.work->request);
        OS_LockMutex(&ov00_0221A694.work->mutex);
        ov00_0221A694.work->result = ov00_021E7F38(restart);
        if (ov00_0221A694.work->result) {
            ov00_0221A694.work->response.error = 20100;
            OS_UnlockMutex(&ov00_0221A694.work->mutex);
            return;
        }
        OS_UnlockMutex(&ov00_0221A694.work->mutex);
        continue;
    }
}

int ov00_021E82FC(void) {
    NasAllocFunc alloc = ov00_0221A694.work->params.alloc;
    NasFreeFunc free = ov00_0221A694.work->params.free;
    void *block;
    s32 error;
    if (ov00_021E9F2C(ov00_0221A694.work->request, 0) != 1) {
        ov00_0221A694.work->response.error = 20100;
        return 14;
    }
    if (ov00_021E84D4() != 0) {
        return 14;
    }
    error = ov00_0221A694.work->response.error;
    if (error < 20100) {
        if (error == 20002) {
            block = alloc(ov00_022164A4, 0x71f);
            if (!block) {
                ov00_0221A694.work->response.error = 20100;
                return 2;
            }
            if (DWCi_AUTH_UpDateWiFiID(ov00_0221A6A0, (void *)(((u32)block + 31) & ~31u)) != 1) {
                free(ov00_022164B4, block, 0);
                ov00_0221A694.work->response.error = 20100;
                return 15;
            }
            free(ov00_022164B4, block, 0);
        }
        return 21;
    }
    switch (error) {
    case 20104: {
        DWCi_AUTH_RemakeWiFiID(ov00_0221A6A0);
        ov00_0221A694.work->response.error = 20104;
        return 16;
    }
    case 20108: {
        block = alloc(ov00_022164A4, 0x700);
        if (!block) {
            ov00_0221A694.work->response.error = 20108;
            return 17;
        }
        DWCi_AUTH_MakeWiFiID((void *)(((u32)block + 31) & ~31u));
        free(ov00_022164B4, block, 0);
        ov00_0221A694.work->response.error = 20108;
        return 17;
    }
    default:
        return 18;
    }
}

// Reads httpresult, returncd and the auth fields of the response.
int ov00_021E84D4(void) {
    char *end = NULL;
    long code;
    long value;
    AuthWork *work;
    end = ov00_021EA1CC(ov00_0221A694.work->request, ov00_022164C0);
    if (end != NULL) {
        ov00_021EB5B0(end);
    }
    end = NULL;
    errno = 0;
    code = atol(ov00_021EA1CC(ov00_0221A694.work->request, ov00_022164C8));
    if (errno == 0x22) {
        ov00_0221A694.work->response.error = 20101;
        return 12;
    }
    if (code != 200) {
        ov00_0221A694.work->response.error = code + 23000;
        return 18;
    }
    if ((int)ov00_021EA224(ov00_0221A694.work->request, ov00_022164D4, ov00_0221A694.work->response.returncd, 4) <= 0) {
        ov00_0221A694.work->response.error = 20101;
        return 14;
    }
    value = strtol(ov00_0221A694.work->response.returncd, &end, 10);
    work = ov00_0221A694.work;
    if (end != work->response.returncd + strlen(work->response.returncd)) {
        work->response.error = 20101;
        return 12;
    }
    work->response.error = value + 20000;
    if (value < 100) {
        ov00_0221A694.work->response.token[0] = 0;
        ov00_0221A694.work->response.locator[0] = 0;
        ov00_0221A694.work->response.challenge[0] = 0;
        ov00_0221A694.work->response.datetime[0] = 0;
        ov00_0221A694.work->response.cookie[0] = 0;
        ov00_021EA224(ov00_0221A694.work->request, ov00_022164E0, ov00_0221A694.work->response.token, 0x12d);
        ov00_021EA224(ov00_0221A694.work->request, ov00_022164E8, ov00_0221A694.work->response.locator, 0x33);
        ov00_021EA224(ov00_0221A694.work->request, ov00_022164F0, ov00_0221A694.work->response.challenge, 9);
        ov00_021EA224(ov00_0221A694.work->request, ov00_022164FC, ov00_0221A694.work->response.datetime, 0xf);
        ov00_021EA278(ov00_0221A694.work->request, ov00_02216508, ov00_0221A694.work->response.cookie, 0x41);
        ov00_0221A694.work->response.cookie[0x2B] = 0;
        if (value != 40) {
            ov00_0221A694.state = 1;
        } else {
            ov00_0221A694.state = 2;
        }
    }
    return 0;
}

// Adds the POST fields of an "acctcreate" or "login" request.
int ov00_021E879C(NasHttp *http, const char *gsbrcd, const u16 *ingamesn) {
    AuthFields fields;
    if (ov00_0221A694.userId == 0) {
        if (ov00_021E99A4(http, ov00_02216520, ov00_02216514, strlen(ov00_02216514)) != 0) {
            return 8;
        }
    } else {
        if (ov00_021E99A4(http, ov00_02216520, ov00_02216528, strlen(ov00_02216528)) != 0) {
            return 8;
        }
        if (ov00_021E99A4(http, ov00_02216530, gsbrcd, strlen(gsbrcd)) != 0) {
            return 8;
        }
    }
    if (!ov00_021E89AC(&fields, (AuthWiFiInfo *)ov00_0221A6A0)) {
        return 5;
    }
    if (!ov00_021E8C5C(http, &fields, ov00_0221A6BC)) {
        return 8;
    }
    if (ov00_0221A694.mode == 1) {
        if (ov00_021E99A4(http, ov00_0221653C, ov00_02216538, strlen(ov00_02216538)) != 0) {
            return 8;
        }
    }
    if (wcslen(ingamesn) != 0) {
        if (ov00_021E99A4(http, ov00_02216544, ingamesn, wcslen(ingamesn) * 2) != 0) {
            return 8;
        }
    }
    return 0;
}

void ov00_021E8940(int result) {
    OS_LockMutex(&ov00_0221A694.work->mutex);
    ov00_0221A694.work->result = result;
    OS_UnlockMutex(&ov00_0221A694.work->mutex);
}

int ov00_021E8984(AuthFields *fields) {
    char info[20];
    DWCi_BM_GetWiFiInfo(info);
    return ov00_021E89AC(fields, (AuthWiFiInfo *)info);
}

static inline void MacString(char *dest, u8 *src) {
    int index;
    HexChars *output = (HexChars *)dest;
    for (index = 0; index < 6; index++, output++) {
        OS_SPrintf(output->byte, ov00_02216560, *src++);
    }
}

// Formats the request fields from the WiFi info, the console and the access point.
int ov00_021E89AC(AuthFields *fields, AuthWiFiInfo *wifiInfo) {
    char owner[84];
    AuthDate date;
    AuthTime time;
    u8 mac[6];
    u16 essidLen;
    int i;
    s32 source;
    OSIntrMode irq;
    u8 *ap;
    char *essid, *cursor;
    u32 a, b;
    char *out = (char *)fields;
    AuthWiFiInfoWords *info = (AuthWiFiInfoWords *)wifiInfo;
    MI_CpuFill8(out, 0, 0x94);
    a = info->userIdLo;
    b = info->userIdHi;
    if (b || a) {
        OS_SNPrintf(out, 14, ov00_02216550, a, b);
    } else {
        a = info->unk_08;
        b = info->unk_0C;
        OS_SNPrintf(out, 14, ov00_02216550, a, b);
    }
    OS_SNPrintf(out + 14, 7, ov00_02216558, info->password);
    if (!*(volatile u8 *)0x27ffe0c) {
        OS_Terminate();
    }
    MI_CpuCopy8((void *)0x27ffe0c, out + 21, 4);
    if (!*(volatile u8 *)0x27ffe10) {
        OS_Terminate();
    }
    MI_CpuCopy8((void *)0x27ffe10, out + 26, 2);
    out[29] = 48;
    OS_GetMacAddress(mac);
    source = (s32)mac;
    cursor = out + 31;
    for (i = 0; i < 6; i++, cursor += 2, source++) {
        OS_SPrintf(cursor, ov00_02216560, *(u8 *)source);
    }
    OS_GetOwnerInfo((OSOwnerInfo *)owner);
    if ((u8)owner[0] >= 8) {
        owner[0] = 1;
    }
    OS_SNPrintf(out + 44, 3, ov00_02216560, (u8)owner[0]);
    MI_CpuCopy8(owner + 4, out + 126, 20);
    OS_SNPrintf(out + 47, 5, ov00_02216568, (u8)owner[2], (u8)owner[3]);
    if (RTC_GetDateTime((RTCDate *)&date, (RTCTime *)&time) != 0) {
        return 0;
    }
    OS_SNPrintf(out + 52, 13, ov00_02216574, date.year, date.month, date.day, time.hour, time.minute, time.second);
    irq = OS_DisableInterrupts();
    ap = WCM_GetApMacAddress();
    DC_InvalidateRange(ap, 6);
    if (!ap) {
        OS_RestoreInterrupts(irq);
        return 0;
    }
    for (i = 0, cursor = out + 65; i < 6; i++, cursor += 2) {
        OS_SPrintf(cursor, ov00_02216560, ap[i]);
    }
    {
        unsigned strength = ov00_021FAA40();
        if (strength == 255 || strength > 99) {
            strength = 99;
        }
        OS_SNPrintf(out + 111, 14, ov00_02216590, strength);
    }
    essid = WCM_GetApEssid(&essidLen);
    DC_InvalidateRange(essid, 32);
    if (!essid) {
        OS_RestoreInterrupts(irq);
        return 0;
    }
    MI_CpuCopy8(essid, out + 78, 32);
    ov00_021FAA70(out + 114);
    OS_RestoreInterrupts(irq);
    return 1;
}

int ov00_021E8C5C(NasHttp *http, AuthFields *fields, int withSsid) {
    char temp[33];
    OS_SNPrintf(temp, 33, ov00_022165A0, 2, 2);
    if (ov00_021E99A4(http, ov00_022165AC, temp, strlen(temp))) {
        return 0;
    }
    if (ov00_021E99A4(http, ov00_022165B4, fields->userid, strlen(fields->userid))) {
        return 0;
    }
    if (ov00_021E99A4(http, ov00_022165BC, fields->passwd, strlen(fields->passwd))) {
        return 0;
    }
    if (ov00_021E99A4(http, ov00_022165C4, fields->bssid, strlen(fields->bssid))) {
        return 0;
    }
    if (ov00_021E99A4(http, ov00_022165CC, fields->apinfo, strlen(fields->apinfo))) {
        return 0;
    }
    if (ov00_021E99A4(http, ov00_022165D4, fields->gamecd, strlen(fields->gamecd))) {
        return 0;
    }
    if (ov00_021E99A4(http, ov00_022165DC, fields->makercd, strlen(fields->makercd))) {
        return 0;
    }
    if (ov00_021E99A4(http, ov00_022165E4, fields->unitcd, strlen(fields->unitcd))) {
        return 0;
    }
    if (ov00_021E99A4(http, ov00_022165EC, fields->macadr, strlen(fields->macadr))) {
        return 0;
    }
    if (ov00_021E99A4(http, ov00_022165F4, fields->lang, strlen(fields->lang))) {
        return 0;
    }
    if (ov00_021E99A4(http, ov00_022165FC, fields->birth, strlen(fields->birth))) {
        return 0;
    }
    if (ov00_021E99A4(http, ov00_02216604, fields->devtime, strlen(fields->devtime))) {
        return 0;
    }
    if (ov00_021E99A4(http, ov00_0221660C, fields->devname, wcslen(fields->devname) * 2)) {
        return 0;
    }
    if (withSsid == 1) {
        if (ov00_021E99A4(http, ov00_02216614, fields->ssid, strlen(fields->ssid))) {
            return 0;
        }
    }
    OS_SNPrintf(temp, 33, ov00_0221661C, 2, 2);
    if (ov00_021E98C8(http, ov00_02216634, temp)) {
        return 0;
    }
    return ov00_021E98C8(http, ov00_02216640, fields->gamecd) == 0;
}

// Sets up a connection: receive/send buffers, response buffer, URL, request line.
int ov00_021E8FA0(NasHttp *http, NasHttpParams *params) {
    NasAllocFunc alloc = params->alloc;
    MI_CpuFill8(http, 0, 0x1C14);
    http->contentLength = -1;
    http->received = -1;
    http->params = *params;
    http->lowRecvBuf = alloc(ov00_02216680, 0xB68);
    if (!http->lowRecvBuf) {
        http->result = 1;
        return 1;
    }
    http->lowSendBuf = alloc(ov00_02216698, 0x5EA);
    if (!http->lowSendBuf) {
        http->result = 1;
        return 1;
    }
    if (!ov00_021E9B44(http, &http->response, http->params.bufferSize)) {
        http->result = 1;
        return 1;
    }
    if (!ov00_021E9C84(http, params->url)) {
        http->result = 1;
        return 1;
    }
    http->result = ov00_021E9808(http);
    if (!http->result) {
        http->status = 255;
    }
    return http->result;
}
