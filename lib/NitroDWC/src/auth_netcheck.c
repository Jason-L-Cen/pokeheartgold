#pragma force_active on
#include <string.h>

#include "global.h"

#include "dwc/auth_internal.h"
#include "dwc/net_common_internal.h"

// The code below is compiled as ARM (32-bit) code.
#include <nitro/code32.h>

extern int errno;
extern char *ov00_022168DC; // connection test URL
extern char ov00_022168A4[];
extern char ov00_022168C0[];
extern char ov00_02216904[];
extern char ov00_02216918[];
extern char ov00_02216928[];
extern char ov00_02216938[];
extern char ov00_02216954[];
extern char ov00_02216974[];
extern char ov00_02216988[];
extern char ov00_022169A4[];
extern char ov00_022169B0[];
extern char ov00_022169C0[];
extern char ov00_022169D0[];
extern char ov00_022169F0[];
extern char ov00_022169FC[];
extern char ov00_02216A1C[];
extern char ov00_02216A24[];
extern char ov00_02216A2C[];
extern char ov00_02216A3C[];
extern char ov00_02216A44[];
extern char ov00_02216A4C[];
extern char ov00_02216A58[];
extern char ov00_02216A5C[];
extern char ov00_02216A64[];
extern char ov00_02216A6C[];
extern char ov00_02216A78[];
extern char ov00_02216A84[];
extern char *ov00_02216AC0[12]; // month names
extern char ov00_02216AF0[];
extern char ov00_02216B10[];
extern char ov00_02216B20[];
extern char ov00_02216B30[];
extern char ov00_02216B50[];
extern char ov00_02216B58[];
extern char ov00_02216B60[];
extern char ov00_02216B64[];
extern char ov00_02216B6C[];
extern char ov00_02216B78[];
extern char ov00_02216B84[];
extern char ov00_02216B8C[];
extern char ov00_02216B9C[];
// int valid; s64 rtc seconds when the server date was read (+4); s64 rtc minus server seconds (+12)
extern char ov00_0221A734[];

extern long atol(const char *);
extern RTCResult RTC_GetDateTime(RTCDate *, RTCTime *);
extern void DWCi_BM_GetWiFiInfo(AuthWiFiInfo *info);

char *DWCi_Http_GetEntry(NasHttp *http, char *label) {
    int i;
    for (i = 0; i < 32; i++) {
        if (!http->entry[i].label) {
            break;
        }
        if (!strcmp(label, http->entry[i].label)) {
            return http->entry[i].value;
        }
    }
    return 0;
}

// Copies the base64-like decoded value of a response field; returns its length.
u32 DWCi_Http_GetBase64Entry(NasHttp *http, char *label, char *out, u32 size) {
    char *v = DWCi_Http_GetEntry(http, label);
    u32 r;
    if (!v) {
        return 0;
    }
    r = DWCi_Base64Decode(v, strlen(v), out, size);
    if (r == 0xffffffff || r >= size) {
        return r;
    }
    out[r] = 0;
    return r;
}

int DWCi_Http_GetStringEntry(NasHttp *http, char *label, char *out, int size) {
    char *v = DWCi_Http_GetEntry(http, label);
    if (!v) {
        return 0;
    }
    if ((int)strlen(v) >= size) {
        return 0;
    }
    strcpy(out, v);
    return 1;
}

void DWCi_Http_FreeEntries(NasHttp *http, NasHttpEntry *entry, int count) {
    NasFreeFunc cb = http->params.free;
    int i;
    char *label, *value;
    i = 0;
    if (count <= 0) {
        return;
    }
    label = ov00_022168A4;
    value = ov00_022168C0;
    do {
        if (entry[i].label) {
            cb(label, entry[i].label, 0);
            entry[i].label = 0;
        }
        if (entry[i].value) {
            cb(value, entry[i].value, 0);
            entry[i].value = 0;
        }
    } while (++i < count);
}

// Allocates the connection test work ("DWCnetcheck") and its connection ("DWChttp"), starts the thread.
int DWCi_NetCheck_Start(NetCheckCallbacks *callbacks) {
    NasAllocFunc alloc = callbacks->alloc;
    if (ov00_0221A6B8.work) {
        return 4;
    }
    ov00_0221A6B8.work = alloc(ov00_02216904, 0x11f4);
    if (!ov00_0221A6B8.work) {
        return 4;
    }
    MI_CpuFill8(ov00_0221A6B8.work, 0, 0x11f4);
    ov00_0221A6B8.work->error = -99999;
    ov00_0221A6B8.work->callbacks = *callbacks;
    if (ov00_0221A6B8.http) {
        return 4;
    }
    ov00_0221A6B8.http = alloc(ov00_02216918, 0x1c14);
    if (!ov00_0221A6B8.http) {
        return 4;
    }
    ov00_0221A6B8.redirected = 0;
    OS_InitMutex(&ov00_0221A6B8.work->mutex);
    DWCi_NetCheck_StartThread();
    return 0;
}

void DWCi_NetCheck_Destroy(void) {
    NasFreeFunc cb = ov00_0221A6B8.work->callbacks.free;
    if (ov00_0221A6B8.http) {
        DWCi_Http_Destroy(ov00_0221A6B8.http);
        cb(ov00_02216928, ov00_0221A6B8.http, 0);
        ov00_0221A6B8.http = 0;
    }
    DWCi_Auth_Destroy();
    if (!ov00_0221A6B8.work) {
        return;
    }
    if (ov00_0221A6B8.work->body302) {
        cb(ov00_02216938, ov00_0221A6B8.work->body302, 0);
        ov00_0221A6B8.work->body302 = 0;
    }
    if (ov00_0221A6B8.work->bodyWayport) {
        cb(ov00_02216954, ov00_0221A6B8.work->bodyWayport, 0);
        ov00_0221A6B8.work->bodyWayport = 0;
    }
    cb(ov00_02216974, ov00_0221A6B8.work, 0);
    ov00_0221A6B8.work = 0;
}

void DWCi_NetCheck_Abort(void) {
    if (!ov00_0221A6B8.work) {
        return;
    }
    if (ov00_0221A6B8.http) {
        DWCi_Http_Abort(ov00_0221A6B8.http);
    }
    DWCi_Auth_Abort();
    if (ov00_0221A6B8.work->thread.id) {
        OS_JoinThread(&ov00_0221A6B8.work->thread);
    }
    ov00_0221A6B8.work->error = -7;
}

int DWCi_NetCheck_GetState(void) {
    int r;
    OS_LockMutex(&ov00_0221A6B8.work->mutex);
    r = ov00_0221A6B8.work->state;
    OS_UnlockMutex(&ov00_0221A6B8.work->mutex);
    return r;
}

int DWCi_NetCheck_GetError(void) {
    return ov00_0221A6B8.work->error;
}

void DWCi_NetCheck_StartThread(void) {
    if (ov00_0221A6B8.work->thread.id && !OS_IsThreadTerminated(&ov00_0221A6B8.work->thread)) {
        return;
    }
    OS_CreateThread(&ov00_0221A6B8.work->thread, DWCi_NetCheck_Thread, ov00_0221A6B8.work, ov00_0221A6B8.work->threadStack + 0x1000, 0x1000, 16);
    OS_WakeupThreadDirect(&ov00_0221A6B8.work->thread);
}

// Connection test thread: GETs the test page; a 302 means a hotspot login page, which is answered
// through the auth server ("HotSpotResponse"/"parse"), then the hotspot's own login is posted.
void DWCi_NetCheck_Thread(void *arg) {
    char *response;
    int status;
    char number[4];
    AuthWiFiInfo wifi;
    AuthResult result;
    AuthFields auth;
    NasAllocFunc allocFunc = ov00_0221A6B8.work->callbacks.alloc;
    NasFreeFunc freeFunc = ov00_0221A6B8.work->callbacks.free;
    char *url = NULL;
    char *post = NULL;
    char *wait = NULL;
    int urlLen, postLen, waitLen;
    int n;
    int waitMs;
    char *copy;
    char *str;

    while (TRUE) {
        ov00_0221A6B8.httpParams.url = ov00_022168DC;
        ov00_0221A6B8.httpParams.method = 1;
        ov00_0221A6B8.httpParams.bufferSize = 0x1000;
        ov00_0221A6B8.httpParams.alloc = allocFunc;
        ov00_0221A6B8.httpParams.free = freeFunc;
        ov00_0221A6B8.httpParams.timeout = 20000;
        ov00_0221A6B8.work->error = -2;
        if (DWCi_Http_Create(ov00_0221A6B8.http, &ov00_0221A6D0)) {
            DWCi_Http_Destroy(ov00_0221A6B8.http);
            DWCi_NetCheck_SetState(1);
            goto done;
        }
        if (DWCi_Http_FinishHeader(ov00_0221A6B8.http)) {
            DWCi_Http_Destroy(ov00_0221A6B8.http);
            DWCi_NetCheck_SetState(1);
            goto done;
        }
        DWCi_Http_StartThread(ov00_0221A6B8.http, OS_GetThreadPriority(OSi_ThreadInfo.current) - 1);
        if (ov00_0221A6B8.http->thread.id != 0) {
            OS_JoinThread(&ov00_0221A6B8.http->thread);
        }
        switch (ov00_0221A6B8.http->result) {
        case 2:
            ov00_0221A6B8.work->error = -1;
        default:
            DWCi_Http_Destroy(ov00_0221A6B8.http);
            DWCi_NetCheck_SetState(3);
            goto done;
        case 8:
            break;
        }
        if (DWCi_Http_ParseResponse(ov00_0221A6B8.http, 0) != 1) {
            DWCi_Http_Destroy(ov00_0221A6B8.http);
            DWCi_NetCheck_SetState(2);
            goto done;
        }
        status = atol(DWCi_Http_GetEntry(ov00_0221A6B8.http, ov00_022169F0));
        if (errno == 34) {
            DWCi_NetCheck_SetState(2);
            goto done;
        }
        switch (status) {
        case 200:
            ov00_0221A6B8.ip = ov00_0221A6B8.http->ip;
            break;
        case 302:
            ov00_0221A6B8.redirected = 1;
            if (ov00_0221A6B8.work->bodyWayport != NULL) {
                ov00_0221A6B8.work->error = -6;
                DWCi_Http_Destroy(ov00_0221A6B8.http);
                ov00_0221A6B8.httpParams.url = ov00_02216420.url;
                ov00_0221A6B8.httpParams.method = 0;
                ov00_0221A6B8.httpParams.bufferSize = 0x200;
                ov00_0221A6B8.httpParams.alloc = allocFunc;
                ov00_0221A6B8.httpParams.free = freeFunc;
                ov00_0221A6B8.httpParams.timeout = 20000;
                if (strcmp(ov00_0221A6B8.httpParams.url, ov00_022169FC) != 0) {
                    ov00_0221A6B8.httpParams.testServer = 1;
                }
                if (DWCi_Http_Create(ov00_0221A6B8.http, &ov00_0221A6D0)) {
                    DWCi_Http_Destroy(ov00_0221A6B8.http);
                    DWCi_NetCheck_SetState(1);
                    goto done;
                }
                if (!DWCi_Auth_FormatFieldsFromBM(&auth) || !DWCi_Auth_AddCommonPostFields(ov00_0221A6B8.http, &auth, 1)) {
                    DWCi_Http_Destroy(ov00_0221A6B8.http);
                    DWCi_NetCheck_SetState(8);
                    goto done;
                }
                if (DWCi_Http_AddPostItem(ov00_0221A6B8.http, ov00_02216A1C, ov00_02216A24, 7)
                    || (str = ov00_0221A6B8.work->bodyWayport, DWCi_Http_AddPostItem(ov00_0221A6B8.http, ov00_02216A2C, str, strlen(str)))) {
                    DWCi_Http_Destroy(ov00_0221A6B8.http);
                    DWCi_NetCheck_SetState(8);
                    goto done;
                }
                freeFunc(ov00_02216954, ov00_0221A6B8.work->bodyWayport, 0);
                ov00_0221A6B8.work->bodyWayport = NULL;
                if (DWCi_Http_FinishHeader(ov00_0221A6B8.http)) {
                    DWCi_Http_Destroy(ov00_0221A6B8.http);
                    DWCi_NetCheck_SetState(1);
                    goto done;
                }
                DWCi_Http_StartThread(ov00_0221A6B8.http, OS_GetThreadPriority(OSi_ThreadInfo.current) - 1);
                if (ov00_0221A6B8.http->thread.id != 0) {
                    OS_JoinThread(&ov00_0221A6B8.http->thread);
                }
                switch (ov00_0221A6B8.http->result) {
                case 2:
                    ov00_0221A6B8.work->error = -1;
                default:
                    DWCi_Http_Destroy(ov00_0221A6B8.http);
                    DWCi_NetCheck_SetState(3);
                    goto done;
                case 8:
                    DWCi_Http_Destroy(ov00_0221A6B8.http);
                    DWCi_NetCheck_SetState(7);
                    goto done;
                }
            } else {
                response = ov00_0221A6B8.http->response.buffer;
                if (response == NULL) {
                    DWCi_Http_Destroy(ov00_0221A6B8.http);
                    DWCi_NetCheck_SetState(2);
                    goto done;
                }
                ov00_0221A6B8.work->body302 = allocFunc(ov00_02216988, strlen(response) + 1);
                copy = ov00_0221A6B8.work->body302;
                if (copy == NULL) {
                    DWCi_Http_Destroy(ov00_0221A6B8.http);
                    DWCi_NetCheck_SetState(4);
                    goto done;
                }
                strncpy(copy, response, strlen(response) + 1);
            }
            break;
        default:
            DWCi_Http_Destroy(ov00_0221A6B8.http);
            DWCi_NetCheck_SetState(10);
            goto done;
        }
        DWCi_Http_Destroy(ov00_0221A6B8.http);
        DWCi_BM_GetWiFiInfo(&wifi);
        if (wifi.userId == 0) {
            ov00_0221A6B8.work->error = -3;
            ov00_0221A6B8.authParams.ingamesn[0] = 0;
            ov00_0221A6B8.authParams.gsbrcd[0] = 0;
            ov00_0221A6B8.authParams.alloc = ov00_0221A6B8.work->callbacks.alloc;
            ov00_0221A6B8.authParams.free = ov00_0221A6B8.work->callbacks.free;
            if (DWCi_Auth_Start(&ov00_0221A6EC, ov00_0221A6B8.http)) {
                DWCi_NetCheck_SetState(5);
                goto done;
            }
            DWCi_Auth_Join();
            if (DWCi_Auth_GetState() != 21) {
                if (DWCi_Auth_GetState() == 9) {
                    ov00_0221A6B8.work->error = -1;
                } else {
                    DWCi_Auth_GetResult(&result);
                    if (ov00_0221A6B8.mode == 1 && (result.error == -23302 || DWCi_Auth_GetState() == 11)) {
                        ov00_0221A6B8.work->error = 0;
                        DWCi_Auth_Destroy();
                        DWCi_NetCheck_SetState(11);
                        goto done;
                    }
                    ov00_0221A6B8.work->error = result.error;
                }
                DWCi_Auth_Destroy();
                DWCi_NetCheck_SetState(6);
                goto done;
            }
            DWCi_Auth_Destroy();
        }
        if (status == 200) {
            ov00_0221A6B8.work->error = 0;
            DWCi_NetCheck_SetState(11);
            goto done;
        }
        ov00_0221A6B8.work->error = -4;
        ov00_0221A6B8.httpParams.url = ov00_02216420.url;
        ov00_0221A6B8.httpParams.method = 0;
        ov00_0221A6B8.httpParams.bufferSize = 0x1000;
        ov00_0221A6B8.httpParams.alloc = allocFunc;
        ov00_0221A6B8.httpParams.free = freeFunc;
        ov00_0221A6B8.httpParams.timeout = 40000;
        if (strcmp(ov00_0221A6B8.httpParams.url, ov00_022169FC) != 0) {
            ov00_0221A6B8.httpParams.testServer = 1;
        }
        if (DWCi_Http_Create(ov00_0221A6B8.http, &ov00_0221A6D0)) {
            DWCi_Http_Destroy(ov00_0221A6B8.http);
            DWCi_NetCheck_SetState(1);
            goto done;
        }
        if (!DWCi_Auth_FormatFieldsFromBM(&auth) || !DWCi_Auth_AddCommonPostFields(ov00_0221A6B8.http, &auth, 1)) {
            DWCi_Http_Destroy(ov00_0221A6B8.http);
            DWCi_NetCheck_SetState(8);
            goto done;
        }
        if (DWCi_Http_AddPostItem(ov00_0221A6B8.http, ov00_02216A1C, ov00_02216A3C, 5)
            || (str = ov00_0221A6B8.work->body302, DWCi_Http_AddPostItem(ov00_0221A6B8.http, ov00_02216A44, str, strlen(str)))) {
            DWCi_Http_Destroy(ov00_0221A6B8.http);
            DWCi_NetCheck_SetState(8);
            goto done;
        }
        freeFunc(ov00_02216938, ov00_0221A6B8.work->body302, 0);
        ov00_0221A6B8.work->body302 = NULL;
        if (DWCi_Http_FinishHeader(ov00_0221A6B8.http)) {
            DWCi_Http_Destroy(ov00_0221A6B8.http);
            DWCi_NetCheck_SetState(1);
            goto done;
        }
        DWCi_Http_StartThread(ov00_0221A6B8.http, OS_GetThreadPriority(OSi_ThreadInfo.current) - 1);
        if (ov00_0221A6B8.http->thread.id != 0) {
            OS_JoinThread(&ov00_0221A6B8.http->thread);
        }
        switch (ov00_0221A6B8.http->result) {
        case 3:
            DWCi_Http_Destroy(ov00_0221A6B8.http);
            if (ov00_0221A6B8.mode == 1) {
                ov00_0221A6B8.work->error = 0;
                DWCi_NetCheck_SetState(11);
            } else {
                DWCi_NetCheck_SetState(3);
            }
            goto done;
        case 2:
            ov00_0221A6B8.work->error = -1;
        default:
            DWCi_Http_Destroy(ov00_0221A6B8.http);
            DWCi_NetCheck_SetState(3);
            goto done;
        case 8:
            break;
        }
        if (DWCi_Http_ParseResponse(ov00_0221A6B8.http, 0) != 1) {
            DWCi_Http_Destroy(ov00_0221A6B8.http);
            DWCi_NetCheck_SetState(2);
            goto done;
        }
        status = atol(DWCi_Http_GetEntry(ov00_0221A6B8.http, ov00_022169F0));
        if (errno == 34) {
            DWCi_Http_Destroy(ov00_0221A6B8.http);
            DWCi_NetCheck_SetState(2);
            goto done;
        }
        if (status != 200) {
            DWCi_Http_Destroy(ov00_0221A6B8.http);
            if (ov00_0221A6B8.mode == 1 && status == 302) {
                ov00_0221A6B8.work->error = 0;
                DWCi_NetCheck_SetState(11);
            } else {
                DWCi_NetCheck_SetState(2);
            }
            goto done;
        }
        if ((int)DWCi_Http_GetBase64Entry(ov00_0221A6B8.http, ov00_02216A4C, number, 4) <= 0) {
            DWCi_Http_Destroy(ov00_0221A6B8.http);
            DWCi_NetCheck_SetState(9);
            goto done;
        }
        n = atol(number);
        if (errno == 34) {
            DWCi_Http_Destroy(ov00_0221A6B8.http);
            DWCi_NetCheck_SetState(9);
            goto done;
        }
        if (ov00_0221A6B8.mode == 1 && n == 114) {
            ov00_0221A6B8.work->error = 0;
            DWCi_NetCheck_SetState(11);
            goto done;
        }
        if (n >= 100) {
            DWCi_Http_Destroy(ov00_0221A6B8.http);
            DWCi_NetCheck_SetState(6);
            goto done;
        }
        urlLen = DWCi_Http_GetBase64Entry(ov00_0221A6B8.http, ov00_02216A58, NULL, 0);
        if (urlLen <= 0) {
            DWCi_Http_Destroy(ov00_0221A6B8.http);
            DWCi_NetCheck_SetState(9);
            goto done;
        }
        postLen = DWCi_Http_GetBase64Entry(ov00_0221A6B8.http, ov00_02216A5C, NULL, 0);
        if (postLen <= 0) {
            DWCi_Http_Destroy(ov00_0221A6B8.http);
            DWCi_NetCheck_SetState(9);
            goto done;
        }
        waitLen = DWCi_Http_GetBase64Entry(ov00_0221A6B8.http, ov00_02216A64, NULL, 0);
        url = allocFunc(ov00_022169A4, urlLen + 1);
        if (url == NULL) {
            DWCi_Http_Destroy(ov00_0221A6B8.http);
            DWCi_NetCheck_SetState(4);
            goto done;
        }
        post = allocFunc(ov00_022169B0, postLen + 1);
        if (post == NULL) {
            DWCi_Http_Destroy(ov00_0221A6B8.http);
            DWCi_NetCheck_SetState(4);
            goto done;
        }
        if (waitLen > 0) {
            wait = allocFunc(ov00_022169C0, waitLen + 1);
            if (wait == NULL) {
                DWCi_Http_Destroy(ov00_0221A6B8.http);
                DWCi_NetCheck_SetState(4);
                goto done;
            }
        }
        n = DWCi_Http_GetBase64Entry(ov00_0221A6B8.http, ov00_02216A58, url, urlLen + 1);
        if (n < 0) {
            DWCi_Http_Destroy(ov00_0221A6B8.http);
            DWCi_NetCheck_SetState(9);
            goto done;
        }
        url[n] = 0;
        n = DWCi_Http_GetBase64Entry(ov00_0221A6B8.http, ov00_02216A5C, post, postLen + 1);
        if (n < 0) {
            DWCi_Http_Destroy(ov00_0221A6B8.http);
            DWCi_NetCheck_SetState(9);
            goto done;
        }
        post[n] = 0;
        waitMs = 0;
        if (waitLen > 0) {
            n = DWCi_Http_GetBase64Entry(ov00_0221A6B8.http, ov00_02216A64, wait, waitLen + 1);
            if (n < 0) {
                DWCi_Http_Destroy(ov00_0221A6B8.http);
                DWCi_NetCheck_SetState(9);
                goto done;
            }
            wait[n] = 0;
            n = atol(wait);
            if (errno == 34) {
                DWCi_Http_Destroy(ov00_0221A6B8.http);
                DWCi_NetCheck_SetState(9);
                goto done;
            }
            waitMs = n * 1000;
            if (waitMs > 180000) {
                waitMs = 180000;
            }
        }
        DWCi_Http_Destroy(ov00_0221A6B8.http);
        ov00_0221A6B8.work->error = -5;
        ov00_0221A6B8.httpParams.url = url;
        ov00_0221A6B8.httpParams.method = 0;
        ov00_0221A6B8.httpParams.bufferSize = 0x1000;
        ov00_0221A6B8.httpParams.alloc = allocFunc;
        ov00_0221A6B8.httpParams.free = freeFunc;
        ov00_0221A6B8.httpParams.timeout = 120000;
        if (DWCi_Http_Create(ov00_0221A6B8.http, &ov00_0221A6D0)) {
            DWCi_Http_Destroy(ov00_0221A6B8.http);
            DWCi_NetCheck_SetState(1);
            goto done;
        }
        if (DWCi_Http_AddString(ov00_0221A6B8.http, post)) {
            DWCi_Http_Destroy(ov00_0221A6B8.http);
            DWCi_NetCheck_SetState(8);
            goto done;
        }
        if (DWCi_Http_FinishHeader(ov00_0221A6B8.http)) {
            DWCi_Http_Destroy(ov00_0221A6B8.http);
            DWCi_NetCheck_SetState(1);
            goto done;
        }
        DWCi_Http_StartThread(ov00_0221A6B8.http, OS_GetThreadPriority(OSi_ThreadInfo.current) - 1);
        if (ov00_0221A6B8.http->thread.id != 0) {
            OS_JoinThread(&ov00_0221A6B8.http->thread);
        }
        switch (ov00_0221A6B8.http->result) {
        case 2:
            ov00_0221A6B8.work->error = -1;
        default:
            DWCi_Http_Destroy(ov00_0221A6B8.http);
            DWCi_NetCheck_SetState(3);
            goto done;
        case 8:
            break;
        }
        if (DWCi_Http_ParseResponse(ov00_0221A6B8.http, 1) != 1) {
            DWCi_Http_Destroy(ov00_0221A6B8.http);
            DWCi_NetCheck_SetState(2);
            goto done;
        }
        response = ov00_0221A6B8.http->response.buffer;
        if (response == NULL) {
            DWCi_Http_Destroy(ov00_0221A6B8.http);
            DWCi_NetCheck_SetState(2);
            goto done;
        }
        ov00_0221A6B8.work->bodyWayport = allocFunc(ov00_022169D0, strlen(response) + 1);
        copy = ov00_0221A6B8.work->bodyWayport;
        if (copy == NULL) {
            DWCi_Http_Destroy(ov00_0221A6B8.http);
            DWCi_NetCheck_SetState(4);
            goto done;
        }
        strncpy(copy, response, strlen(response) + 1);
        DWCi_Http_Destroy(ov00_0221A6B8.http);
        OS_Sleep(waitMs);
    }
done:
    if (url != NULL) {
        freeFunc(ov00_02216A6C, url, 0);
    }
    if (post != NULL) {
        freeFunc(ov00_02216A78, post, 0);
    }
    if (wait != NULL) {
        freeFunc(ov00_02216A84, wait, 0);
    }
}

void DWCi_NetCheck_SetState(int state) {
    OS_LockMutex(&ov00_0221A6B8.work->mutex);
    ov00_0221A6B8.work->state = state;
    OS_UnlockMutex(&ov00_0221A6B8.work->mutex);
}

// Reads the server's "Date" header ("Fri, 03 Mar 2006 01:28:13 GMT") and stores the RTC offset.
int DWCi_ParseServerDate(char *s) {
    RTCDate date;
    RTCTime time;
    char buf[32];
    s64 now, then;
    int i;
    int n = strlen(s);
    if (n != (int)strlen(ov00_02216AF0) || s[7] != 32 || s[11] != 32 || s[16] != 32 || s[19] != 58 || s[22] != 58 || s[25] != 32) {
        goto fail;
    }
    if (RTC_GetDateTime(&date, &time) != 0) {
        goto fail;
    }
    now = RTC_ConvertDateTimeToSecond(&date, &time);
    if (now == -1) {
        goto fail;
    }
    strcpy(buf, s);
    buf[7] = 0;
    buf[11] = 0;
    buf[16] = 0;
    buf[19] = 0;
    buf[22] = 0;
    buf[25] = 0;
    errno = 0;
    date.year = atol(buf + 12);
    if (errno == 34) {
        goto fail;
    }
    date.year -= 2000;
    date.month = 13;
    for (i = 0; i < 12; i++) {
        if (!strcmp(ov00_02216AC0[i], buf + 8)) {
            date.month = i + 1;
            break;
        }
    }
    if (date.month > 12) {
        goto fail;
    }
    errno = 0;
    date.day = atol(buf + 5);
    if (errno == 34) {
        goto fail;
    }
    errno = 0;
    time.hour = atol(buf + 17);
    if (errno == 34) {
        goto fail;
    }
    errno = 0;
    time.minute = atol(buf + 20);
    if (errno == 34) {
        goto fail;
    }
    errno = 0;
    time.second = atol(buf + 23);
    if (errno == 34) {
        goto fail;
    }
    then = RTC_ConvertDateTimeToSecond(&date, &time);
    if (then == -1) {
        goto fail;
    }
    *(s64 *)(ov00_0221A734 + 12) = now - then;
    *(s64 *)(ov00_0221A734 + 4) = now;
    *(int *)ov00_0221A734 = 1;
    return 1;
fail:
    return 0;
}

// Service locator ("SVCLOC"): work block "intwork".
int DWCi_Svl_Init(NasAllocFunc alloc, NasFreeFunc free) {
    if (ov00_0221A748) {
        DWCi_SetError(9, -24100);
        return 0;
    }
    ov00_0221A748 = alloc(ov00_02216B10, 0x1c20);
    if (!ov00_0221A748) {
        DWCi_SetError(9, -24100);
        return 0;
    }
    MI_CpuFill8(ov00_0221A748, 0, 0x1c20);
    ov00_0221A748->alloc = alloc;
    ov00_0221A748->free = free;
    return 1;
}

void DWCi_Svl_Cleanup(void) {
    if (!ov00_0221A748) {
        return;
    }
    DWCi_Http_Destroy(&ov00_0221A748->http);
    ov00_0221A748->free(ov00_02216B20, ov00_0221A748, 0);
    ov00_0221A748 = 0;
}

// Starts a service locator request for service svc (4 characters).
int DWCi_Svl_GetTokenAsync(const char *svc, SvcLocResult *output) {
    AuthFields auth;
    NasHttpParams param;
    register SvcLocWork *work;
    ov00_0221A748->output = output;
    MI_CpuFill8(output, 0, 0x174);
    if (ov00_0221A748->http.thread.id && !OS_IsThreadTerminated(&ov00_0221A748->http.thread)) {
        OS_Terminate();
    }
    param.url = ov00_02216420.url;
    param.method = 0;
    param.bufferSize = 0x1000;
    work = ov00_0221A748;
    param.alloc = work->alloc;
    param.free = work->free;
    if (strcmp(param.url, ov00_02216B30) != 0) {
        param.testServer = 1;
    } else {
        param.testServer = 0;
    }
    param.timeout = 20000;
    if (DWCi_Http_Create(&work->http, &param)) {
        DWCi_SetError(9, -24100);
        return 0;
    }
    if (!DWCi_Auth_FormatFieldsFromBM(&auth) || !DWCi_Auth_AddCommonPostFields(&ov00_0221A748->http, &auth, 0)) {
        DWCi_Http_Destroy(&ov00_0221A748->http);
        DWCi_SetError(9, -24100);
        return 0;
    }
    if (DWCi_Http_AddPostItem(&ov00_0221A748->http, ov00_02216B50, ov00_02216B58, 6) || DWCi_Http_AddPostItem(&ov00_0221A748->http, ov00_02216B60, svc, 4)) {
        DWCi_Http_Destroy(&ov00_0221A748->http);
        DWCi_SetError(9, -24100);
        return 0;
    }
    if (DWCi_Http_FinishHeader(&ov00_0221A748->http)) {
        DWCi_Http_Destroy(&ov00_0221A748->http);
        DWCi_SetError(9, -24100);
        return 0;
    }
    DWCi_Http_StartThread(&ov00_0221A748->http, OS_GetThreadPriority(OS_GetCurrentThread()) - 1);
    if (ov00_0221A748->http.thread.id) {
        return 1;
    }
    DWCi_Http_Destroy(&ov00_0221A748->http);
    DWCi_SetError(9, -24100);
    return 0;
}

// 0: no request, 1: idle, 2: running, 3: done, 4: failed, 5: cancelled.
int DWCi_Svl_Process(void) {
    if (!ov00_0221A748) {
        return 0;
    }
    if (!ov00_0221A748->http.thread.id) {
        return 1;
    }
    if (OS_IsThreadTerminated(&ov00_0221A748->http.thread) == 1) {
        switch (ov00_0221A748->http.result) {
        case 8:
            if (!DWCi_Svl_ReadResponse()) {
                DWCi_Http_Destroy(&ov00_0221A748->http);
                return 4;
            }
            DWCi_Http_Destroy(&ov00_0221A748->http);
            return 3;
        case 7:
            DWCi_Http_Destroy(&ov00_0221A748->http);
            return 5;
        default:
            DWCi_Http_Destroy(&ov00_0221A748->http);
            DWCi_SetError(17, -24101);
            return 4;
        }
    }
    return 2;
}

int DWCi_Svl_ReadResponse(void) {
    SvcLocWork *p = ov00_0221A748;
    char b[4];
    char *v;
    int n;
    if (!DWCi_Http_ParseResponse(&p->http, 0)) {
        DWCi_SetError(16, -25101);
        return 0;
    }
    v = DWCi_Http_GetEntry(&p->http, ov00_02216B64);
    if (v) {
        DWCi_ParseServerDate(v);
    }
    v = DWCi_Http_GetEntry(&p->http, ov00_02216B6C);
    if (!v) {
        DWCi_SetError(16, -25101);
        return 0;
    }
    n = atol(v);
    if (errno == 34) {
        DWCi_SetError(16, -25101);
        return 0;
    }
    if (n != 200) {
        DWCi_SetError(16, -25000 - n);
        return 0;
    }
    if ((int)DWCi_Http_GetBase64Entry(&p->http, ov00_02216B78, b, 4) <= 0) {
        DWCi_SetError(16, -25101);
        return 0;
    }
    n = atol(b);
    if (errno == 34) {
        DWCi_SetError(16, -25101);
        return 0;
    }
    if (n >= 100) {
        DWCi_SetError(16, -24000 - n);
        return 0;
    }
    DWCi_Http_GetBase64Entry(&p->http, ov00_02216B84, ov00_0221A748->output->svchost, 64);
    DWCi_Http_GetBase64Entry(&p->http, ov00_02216B8C, ov00_0221A748->output->servicetoken, 300);
    DWCi_Http_GetBase64Entry(&p->http, ov00_02216B9C, b, 1);
    if (b[0] == 'Y') {
        ov00_0221A748->output->statusdata = 1;
    } else {
        ov00_0221A748->output->statusdata = 0;
    }
    return 1;
}
