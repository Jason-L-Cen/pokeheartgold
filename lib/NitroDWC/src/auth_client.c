#include <string.h>

#include "global.h"

// The code below is compiled as ARM (32-bit) code.
#include <nitro/code32.h>

#pragma force_active on

typedef struct AuthServerConfig {
    const char *url;
    u32 unk_04;
    u32 unk_08;
    u32 unk_0C; // allocator
    u32 unk_10; // deallocator
    u32 unk_14; // set when url is not the production server
    u32 unk_18;
} AuthServerConfig;

typedef struct AuthParams {
    u16 unk_00[26];
    char unk_34[12];
    void *(*alloc)(char *name, int size);
    void (*free)(char *name, void *ptr, int size);
} AuthParams;

// 0x13F4 bytes, allocated by ov00_021E7B70 with the caller's allocator ("ALLOC DWCauth").
typedef struct AuthWork {
    u8 threadStack[0x1000];
    u32 unk_1000;
    int result;
    u8 response[0x1C4]; // starts with the result code
    AuthParams params;
    u8 unk_1214[0x100];
    void *request;
    OSThread thread;
    OSMutex mutex;
    int cancelled;
} AuthWork;

typedef struct AuthState {
    u32 unk_00;
    u32 unk_04;
    AuthWork *work;
} AuthState;

extern AuthServerConfig ov00_02216420;
extern AuthState ov00_0221A694;
extern char ov00_0221A69C[];
extern char ov00_0221A6A0[];
extern char ov00_0221643C[];
extern char ov00_02216464[];
extern char ov00_02216474[];
extern char ov00_02216484[];

extern void ov00_021E8048(void *);
extern u32 ov00_021E879C(void *req, void *a, void *b);
extern int ov00_021E8FA0(void *req, AuthServerConfig *config);
extern int ov00_021E90BC(void *req);
extern void ov00_021E9154(void *req, u32 priority);
extern void ov00_021E9210(void *req);
extern void ov00_021E9754(void *req);
extern void DWCi_BM_GetWiFiInfo(void *info);

void ov00_021E7B60(const char *url);
int ov00_021E7B70(AuthParams *params, void *request);
void ov00_021E7C94(void);
void ov00_021E7D48(void);
void ov00_021E7DD4(void);
void ov00_021E7E2C(void);
int ov00_021E7E5C(void);
void ov00_021E7EA8(void *out);
int ov00_021E7F38(int mode);

void ov00_021E7B60(const char *url) {
    ov00_02216420.url = url;
}

int ov00_021E7B70(AuthParams *params, void *request) {
    AuthWork *work;
    void *(*alloc)(char *, int) = params->alloc;
    if (ov00_0221A694.work) {
        return 2;
    }
    ov00_0221A694.work = alloc(ov00_02216464, 0x13F4);
    if (!ov00_0221A694.work) {
        return 2;
    }
    MI_CpuFill8(ov00_0221A694.work, 0, 0x13F4);
    ov00_0221A694.work->request = request;
    ov00_0221A694.unk_00 = 0;
    MI_CpuFill8(ov00_0221A694.work->response, 0, 0x1C4);
    *(u32 *)ov00_0221A694.work->response = 20100;
    work = ov00_0221A694.work;
    work->params = *params;
    work->params.unk_00[25] = 0;
    ov00_0221A694.work->params.unk_34[11] = 0;
    ov00_0221A694.work->result = ov00_021E7F38(1);
    if (ov00_0221A694.work->result) {
        return ov00_0221A694.work->result;
    }
    ov00_0221A694.unk_04 = 0;
    ov00_021E7C94();
    return 0;
}

void ov00_021E7C94(void) {
    AuthWork *work;
    OS_InitMutex(&ov00_0221A694.work->mutex);
    ov00_0221A694.work->cancelled = 0;
    work = ov00_0221A694.work;
    if (work->thread.id && !OS_IsThreadTerminated(&work->thread)) {
        return;
    }
    work = ov00_0221A694.work;
    OS_CreateThread(&work->thread, ov00_021E8048, ov00_0221A69C, work->threadStack + 0x1000, 0x1000, 16);
    OS_WakeupThreadDirect(&ov00_0221A694.work->thread);
}

void ov00_021E7D48(void) {
    AuthWork *work;
    if (!ov00_0221A694.work) {
        return;
    }
    OS_LockMutex(&ov00_0221A694.work->mutex);
    ov00_0221A694.work->cancelled = 1;
    OS_UnlockMutex(&ov00_0221A694.work->mutex);
    if (ov00_0221A694.work->request) {
        ov00_021E9210(ov00_0221A694.work->request);
    }
    work = ov00_0221A694.work;
    if (work->thread.id) {
        OS_JoinThread(&work->thread);
    }
}

void ov00_021E7DD4(void) {
    AuthWork *work = ov00_0221A694.work;
    void (*free)(char *, void *, int);
    if (!work) {
        return;
    }
    free = work->params.free;
    if (work->request) {
        ov00_021E9754(work->request);
    }
    free(ov00_02216474, ov00_0221A694.work, 0);
    ov00_0221A694.work = NULL;
}

void ov00_021E7E2C(void) {
    AuthWork *work = ov00_0221A694.work;
    if (work->thread.id) {
        OS_JoinThread(&work->thread);
    }
}

int ov00_021E7E5C(void) {
    AuthWork *work;
    int result;
    if (!ov00_0221A694.work) {
        return 22;
    }
    OS_LockMutex(&ov00_0221A694.work->mutex);
    work = ov00_0221A694.work;
    result = work->result;
    OS_UnlockMutex(&work->mutex);
    return result;
}

// Copies the response; result codes outside 20000-29999 become 20998, codes from 20100 on are negated.
void ov00_021E7EA8(void *out) {
    int code;
    if (!ov00_0221A694.work) {
        MI_CpuFill8(out, 0, 0x1c4);
    }
    MI_CpuCopy8(ov00_0221A694.work->response, out, 0x1c4);
    code = *(int *)out;
    if (code < 20000 || code >= 30000) {
        *(int *)out = 20998;
    }
    code = *(int *)out;
    if (code >= 20100) {
        *(int *)out = -code;
    }
}

AuthServerConfig ov00_02216420 = { ov00_0221643C, 0, 0x1000, 0, 0, 0, 20000 };

int ov00_021E7F38(int mode) {
    if (strcmp(ov00_02216420.url, ov00_02216484) != 0) {
        ov00_02216420.unk_14 = 1;
    }
    ov00_02216420.unk_0C = (u32)ov00_0221A694.work->params.alloc;
    ov00_02216420.unk_10 = (u32)ov00_0221A694.work->params.free;
    if (ov00_021E8FA0(ov00_0221A694.work->request, &ov00_02216420) != 0) {
        return 4;
    }
    if (mode == 1) {
        DWCi_BM_GetWiFiInfo(ov00_0221A6A0);
    }
    ov00_0221A694.work->result = ov00_021E879C(ov00_0221A694.work->request, ov00_0221A694.work->params.unk_34, &ov00_0221A694.work->params);
    if (ov00_0221A694.work->result != 0) {
        return 4;
    }
    if (ov00_021E90BC(ov00_0221A694.work->request) != 0) {
        return 4;
    }
    ov00_021E9154(ov00_0221A694.work->request, OS_GetThreadPriority(OSi_ThreadInfo.current) - 1);
    return 0;
}
