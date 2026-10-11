#include <string.h>

#include "global.h"

#include "dwc/auth_internal.h"
#include "dwc/net_common_internal.h"

// The code below is compiled as ARM (32-bit) code.
#include <nitro/code32.h>

#pragma force_active on

extern char ov00_0221A69C[];
extern AuthWiFiInfo ov00_0221A6A0;
extern char ov00_0221643C[];
extern char ov00_02216464[];
extern char ov00_02216474[];
extern char ov00_02216484[];

extern void DWCi_BM_GetWiFiInfo(AuthWiFiInfo *info);

void ov00_021E7B60(const char *url) {
    ov00_02216420.url = url;
}

int ov00_021E7B70(AuthParams *params, NasHttp *request) {
    AuthWork *work;
    NasAllocFunc alloc = params->alloc;
    if (ov00_0221A694.work) {
        return 2;
    }
    ov00_0221A694.work = alloc(ov00_02216464, 0x13F4);
    if (!ov00_0221A694.work) {
        return 2;
    }
    MI_CpuFill8(ov00_0221A694.work, 0, 0x13F4);
    ov00_0221A694.work->request = request;
    ov00_0221A694.state = 0;
    MI_CpuFill8(&ov00_0221A694.work->response, 0, 0x1C4);
    ov00_0221A694.work->response.error = 20100;
    work = ov00_0221A694.work;
    work->params = *params;
    work->params.ingamesn[25] = 0;
    ov00_0221A694.work->params.gsbrcd[11] = 0;
    ov00_0221A694.work->result = ov00_021E7F38(1);
    if (ov00_0221A694.work->result) {
        return ov00_0221A694.work->result;
    }
    ov00_0221A694.mode = 0;
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
    NasFreeFunc free;
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
void ov00_021E7EA8(AuthResult *out) {
    int code;
    if (!ov00_0221A694.work) {
        MI_CpuFill8(out, 0, 0x1c4);
    }
    MI_CpuCopy8(&ov00_0221A694.work->response, out, 0x1c4);
    code = out->error;
    if (code < 20000 || code >= 30000) {
        out->error = 20998;
    }
    code = out->error;
    if (code >= 20100) {
        out->error = -code;
    }
}

NasHttpParams ov00_02216420 = { ov00_0221643C, 0, 0x1000, NULL, NULL, 0, 20000 };

int ov00_021E7F38(int mode) {
    if (strcmp(ov00_02216420.url, ov00_02216484) != 0) {
        ov00_02216420.testServer = 1;
    }
    ov00_02216420.alloc = ov00_0221A694.work->params.alloc;
    ov00_02216420.free = ov00_0221A694.work->params.free;
    if (ov00_021E8FA0(ov00_0221A694.work->request, &ov00_02216420) != 0) {
        return 4;
    }
    if (mode == 1) {
        DWCi_BM_GetWiFiInfo(&ov00_0221A6A0);
    }
    ov00_0221A694.work->result = ov00_021E879C(ov00_0221A694.work->request, ov00_0221A694.work->params.gsbrcd, ov00_0221A694.work->params.ingamesn);
    if (ov00_0221A694.work->result != 0) {
        return 4;
    }
    if (ov00_021E90BC(ov00_0221A694.work->request) != 0) {
        return 4;
    }
    ov00_021E9154(ov00_0221A694.work->request, OS_GetThreadPriority(OSi_ThreadInfo.current) - 1);
    return 0;
}
