#include "nitrowifi/wcm_internal.h"

// WCM API: initialisation on the caller's work area and the asynchronous start-up, search, connect,
// disconnect, clean-up and terminate requests.

#pragma force_active on

int WCM_Init(WcmWork *work, u32 size) {
    OSIntrMode enabled = OS_DisableInterrupts();
    if (gWcmWork) {
        OS_RestoreInterrupts(enabled);
        return 1;
    }
    if (!work) {
        OS_RestoreInterrupts(enabled);
        return 1;
    }
    if ((u32)work & 31) {
        OS_RestoreInterrupts(enabled);
        return 1;
    }
    if (size < sizeof(WcmWork)) {
        OS_RestoreInterrupts(enabled);
        return 6;
    }
    gWcmWork = work;
    work->phase = 1;
    gWcmWork->notify = 0;
    gWcmWork->scanPeriod = 0;
    gWcmWork->apListLocked = 0;
    gWcmWork->resetting = 0;
    gWcmWork->aid = 0;
    gWcmWork->reason = 0;
    WcmInitOption();
    WCMi_InitCpsif();
    if (!OS_IsTickAvailable()) {
        OS_InitTick();
    }
    if (!OS_IsAlarmAvailable()) {
        OS_InitAlarm();
    }
    OS_CreateAlarm(&gWcmWork->keepAliveAlarm);
    OS_RestoreInterrupts(enabled);
    return 0;
}

int WCM_Finish(void) {
    OSIntrMode enabled = OS_DisableInterrupts();
    if (!gWcmWork) {
        OS_RestoreInterrupts(enabled);
        return 1;
    }
    if (gWcmWork->phase != 1) {
        OS_RestoreInterrupts(enabled);
        return 1;
    }
    gWcmWork = NULL;
    OS_RestoreInterrupts(enabled);
    return 0;
}

int WCM_StartupAsync(WcmConfig *config, WcmCallback callback) {
    OSIntrMode enabled = OS_DisableInterrupts();
    if (!gWcmWork) {
        OS_RestoreInterrupts(enabled);
        return 1;
    }
    switch (gWcmWork->phase) {
    case 1:
        WcmConfigure(config, callback);
        break;
    case 2:
        OS_RestoreInterrupts(enabled);
        return 2;
    case 3:
        OS_RestoreInterrupts(enabled);
        return 0;
    default:
        OS_RestoreInterrupts(enabled);
        return 1;
    }
    switch (WM_Init(gWcmWork->wmWork, (u16)gWcmWork->dmaNo)) {
    case 0:
        break;
    case 3:
        WcmSetPhase(11);
        OS_RestoreInterrupts(enabled);
        return 7;
    case 4:
        OS_RestoreInterrupts(enabled);
        return 5;
    case 1:
    case 2:
    case 5:
    case 6:
    default:
        WcmSetPhase(11);
        OS_RestoreInterrupts(enabled);
        return 7;
    }
    if (WM_GetAllowedChannel() == 0) {
        if (WM_Finish() != 0) {
            WcmSetPhase(11);
            OS_RestoreInterrupts(enabled);
            return 7;
        }
        OS_RestoreInterrupts(enabled);
        return 5;
    }
    if (WM_SetIndCallback((void (*)(void *))WcmWmcbIndication) != 0) {
        WcmSetPhase(11);
        OS_RestoreInterrupts(enabled);
        return 7;
    }
    switch (WM_Enable((void (*)(void *))WcmWmcbCommon)) {
    case 2:
        WcmSetPhase(2);
        gWcmWork->notify = 1;
        break;
    case 8:
        WcmSetPhase(12);
        OS_RestoreInterrupts(enabled);
        return 1;
    case 3:
    default:
        WcmSetPhase(11);
        OS_RestoreInterrupts(enabled);
        return 7;
    }
    OS_RestoreInterrupts(enabled);
    return 3;
}

int WCM_CleanupAsync(void) {
    OSIntrMode enabled = OS_DisableInterrupts();
    if (!gWcmWork) {
        OS_RestoreInterrupts(enabled);
        return 1;
    }
    switch (gWcmWork->phase) {
    case 4:
        OS_RestoreInterrupts(enabled);
        return 2;
    case 1:
        OS_RestoreInterrupts(enabled);
        return 0;
    default:
        OS_RestoreInterrupts(enabled);
        return 1;
    case 3:
        break;
    }
    switch (WM_PowerOff((void (*)(void *))WcmWmcbCommon)) {
    case 2:
        WcmSetPhase(4);
        gWcmWork->notify = 2;
        break;
    case 8:
        OS_RestoreInterrupts(enabled);
        return 4;
    case 3:
    default:
        WcmSetPhase(11);
        OS_RestoreInterrupts(enabled);
        return 7;
    }
    OS_RestoreInterrupts(enabled);
    return 3;
}

// NULL filters end the search
int WCM_SearchAsync(const u8 *bssid, const u8 *ssid, u32 option) {
    if (!bssid || !ssid) {
        return WCM_EndSearchAsync();
    }
    return WCM_BeginSearchAsync(bssid, ssid, option);
}

int WCM_BeginSearchAsync(const u8 *bssid, const u8 *ssid, u32 option) {
    OSIntrMode enabled = OS_DisableInterrupts();
    if (!gWcmWork) {
        OS_RestoreInterrupts(enabled);
        return 1;
    }
    switch (gWcmWork->phase) {
    case 3:
        break;
    case 5:
        WcmEditScanExParam(bssid, ssid, option);
        OS_RestoreInterrupts(enabled);
        return 2;
    case 6:
        WcmEditScanExParam(bssid, ssid, option);
        OS_RestoreInterrupts(enabled);
        return 0;
    default:
        OS_RestoreInterrupts(enabled);
        return 1;
    }
    WcmEditScanExParam(bssid, ssid, option);
    DC_InvalidateRange(gWcmWork->scanParam.buffer, gWcmWork->scanParam.bufferSize);
    gWcmWork->scanCount++;
    switch (WM_StartScanEx((void (*)(void *))WcmWmcbScanEx, &gWcmWork->scanParam)) {
    case 2:
        WcmSetPhase(5);
        gWcmWork->notify = 3;
        break;
    case 8:
        OS_RestoreInterrupts(enabled);
        return 4;
    case 3:
    default:
        WcmSetPhase(11);
        OS_RestoreInterrupts(enabled);
        return 7;
    }
    OS_RestoreInterrupts(enabled);
    return 3;
}

int WCM_EndSearchAsync(void) {
    OSIntrMode enabled = OS_DisableInterrupts();
    if (!gWcmWork) {
        OS_RestoreInterrupts(enabled);
        return 1;
    }
    switch (gWcmWork->phase) {
    case 6:
        WcmSetPhase(7);
        gWcmWork->notify = 4;
        break;
    case 7:
        OS_RestoreInterrupts(enabled);
        return 2;
    case 3:
        OS_RestoreInterrupts(enabled);
        return 0;
    default:
        OS_RestoreInterrupts(enabled);
        return 1;
    }
    OS_RestoreInterrupts(enabled);
    return 3;
}

int WCM_ConnectAsync(WcmBssDesc *ap, WcmWepKey *key, u32 option) {
    OSIntrMode enabled = OS_DisableInterrupts();
    if (!gWcmWork) {
        OS_RestoreInterrupts(enabled);
        return 1;
    }
    switch (gWcmWork->phase) {
    case 3:
        if (!ap) {
            OS_RestoreInterrupts(enabled);
            return 1;
        }
        if (ap->gameInfoLength) {
            OS_RestoreInterrupts(enabled);
            return 1;
        }
        if (key) {
            if (key->mode >= 4 || key->keyId >= 4) {
                OS_RestoreInterrupts(enabled);
                return 1;
            }
            gWcmWork->wepMode = key->mode;
            gWcmWork->wepKeyId = key->keyId;
            if (gWcmWork->wepMode == 0) {
                MI_CpuFill8(gWcmWork->wepKey, 0, sizeof(gWcmWork->wepKey));
            } else {
                MI_CpuCopy8(key->key, gWcmWork->wepKey, sizeof(gWcmWork->wepKey));
            }
        } else {
            // key, mode and key id
            MI_CpuFill8(gWcmWork->wepKey, 0, 82);
        }
        MI_CpuCopy8(ap, &gWcmWork->ap, sizeof(WcmBssDesc));
        gWcmWork->ap.capability = gWcmWork->ap.unk_2E | 3;
        WCM_UpdateOption(option);
        break;
    case 8:
        OS_RestoreInterrupts(enabled);
        return 2;
    case 9:
        OS_RestoreInterrupts(enabled);
        return 0;
    default:
        OS_RestoreInterrupts(enabled);
        return 1;
    }
    switch (WM_SetLifeTime((void (*)(void *))WcmWmcbCommon, 0xFFFF, 80, 0xFFFF, 0xFFFF)) {
    case 2:
        WcmSetPhase(8);
        gWcmWork->notify = 5;
        break;
    case 8:
        OS_RestoreInterrupts(enabled);
        return 4;
    case 3:
    default:
        WcmSetPhase(11);
        OS_RestoreInterrupts(enabled);
        return 7;
    }
    OS_RestoreInterrupts(enabled);
    return 3;
}

int WCM_DisconnectAsync(void) {
    OSIntrMode enabled = OS_DisableInterrupts();
    if (!gWcmWork) {
        OS_RestoreInterrupts(enabled);
        return 1;
    }
    switch (gWcmWork->phase) {
    case 10:
        OS_RestoreInterrupts(enabled);
        return 2;
    case 3:
        OS_RestoreInterrupts(enabled);
        return 0;
    default:
        OS_RestoreInterrupts(enabled);
        return 1;
    case 9:
        break;
    }
    if (gWcmWork->resetting == 1) {
        WcmSetPhase(10);
        gWcmWork->notify = 6;
    } else {
        switch (WM_EndDCF((void (*)(void *))WcmWmcbEndDCF)) {
        case 2:
            WcmSetPhase(10);
            gWcmWork->notify = 6;
            break;
        case 8:
            OS_RestoreInterrupts(enabled);
            return 4;
        case 3:
        default:
            WcmSetPhase(11);
            OS_RestoreInterrupts(enabled);
            return 7;
        }
    }
    OS_RestoreInterrupts(enabled);
    return 3;
}

// Stops whatever WM is doing (by its state) and finishes it.
int WCM_TerminateAsync(void) {
    OSIntrMode enabled = OS_DisableInterrupts();
    int result;
    u16 *status;
    if (gWcmWork == NULL) {
        OS_RestoreInterrupts(enabled);
        return 1;
    }
    switch (gWcmWork->phase) {
    case 13:
        OS_RestoreInterrupts(enabled);
        return 2;
    case 1:
        OS_RestoreInterrupts(enabled);
        return 0;
    case 6:
        WcmSetPhase(13);
        gWcmWork->notify = 9;
        OS_RestoreInterrupts(enabled);
        return 3;
    case 3:
    case 9:
    case 12:
        break;
    case 0:
    case 2:
    case 4:
    case 5:
    case 7:
    case 8:
    case 10:
    case 11:
    default:
        OS_RestoreInterrupts(enabled);
        return 1;
    }
    if (gWcmWork->resetting == 1) {
        WcmSetPhase(13);
        gWcmWork->notify = 9;
        goto accepted;
    }
    status = WMi_GetStatusAddress();
    DC_InvalidateRange(status, 2);
    switch (*status) {
    case 0:
        result = WM_Finish();
        if (result == 0) {
            WcmSetPhase(1);
            gWcmWork->notify = 0;
            OS_RestoreInterrupts(enabled);
            return 0;
        }
        break;
    case 1:
        result = WM_Disable((void (*)(void *))WcmWmcbCommon);
        break;
    case 2:
        result = WM_PowerOff((void (*)(void *))WcmWmcbCommon);
        break;
    default:
        gWcmWork->resetting = 1;
        result = WM_Reset((void (*)(void *))WcmWmcbReset);
        break;
    }
    switch (result) {
    case 2:
        WcmSetPhase(13);
        gWcmWork->notify = 9;
        break;
    case 8:
        OS_RestoreInterrupts(enabled);
        return 4;
    case 3:
    default:
        WcmSetPhase(11);
        OS_RestoreInterrupts(enabled);
        return 7;
    }
accepted:
    OS_RestoreInterrupts(enabled);
    return 3;
}

int WCM_GetPhase(void) {
    OSIntrMode enabled = OS_DisableInterrupts();
    int phase = 0;
    if (gWcmWork) {
        phase = gWcmWork->phase;
    }
    OS_RestoreInterrupts(enabled);
    return phase;
}

// Sets the option fields given in option (bit 15: channel mask 0x3FFE, 17: 0x10000, 19: 0x40000,
// 21: 0x100000, 23: 0x400000) and returns the old options.
u32 WCM_UpdateOption(u32 option) {
    OSIntrMode enabled = OS_DisableInterrupts();
    u32 mask = 0;
    WcmWork *work = gWcmWork;
    u32 old = work->option;
    if (!work) {
        OS_RestoreInterrupts(enabled);
        return 0;
    }
    if (option & 0x8000) {
        mask |= 0x3FFE;
        if (!(option & 0x3FFE)) {
            option |= 0xA082;
        }
    }
    if (option & 0x20000) {
        mask |= 0x10000;
    }
    if (option & 0x80000) {
        mask |= 0x40000;
    }
    if (option & 0x200000) {
        mask |= 0x100000;
    }
    if (option & 0x800000) {
        mask |= 0x400000;
    }
    work->option = option | (old & ~mask);
    OS_RestoreInterrupts(enabled);
    return old;
}

WcmWork *WCMi_GetSystemWork(void) {
    return gWcmWork;
}
