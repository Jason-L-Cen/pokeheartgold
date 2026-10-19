#include <nitro.h>
#ifdef HEARTGOLD_ABI32
#include <nitro/spi/native_mic.h>
#endif

// The code below is compiled as ARM (32-bit) code.
#include <nitro/code32.h>

// Microphone: auto sampling requests to the ARM7 over the PXI FIFO, synchronous wrappers that wait
// for the reply.

typedef struct MicWork {
    u16 init;
    u16 unk_02;
    volatile BOOL lock;       // 0x04: a request is running
    MICCallback callback;     // 0x08: reply of the running request
    void *callbackArg;        // 0x0C
    MICResult result;         // 0x10: result of the synchronous functions
    MICCallback fullCallback; // 0x14: auto sampling buffer full (MICAutoParam.full_callback)
    void *fullArg;            // 0x18
    u16 *sampleResult;        // 0x1C: single sample (command 0x40)
} MicWork;

#define MIC_LAST_SAMPLING_ADDRESS (*(void **)0x027FFF90)
#define MIC_SAMPLE_DATA           (*(u16 *)0x027FFF94)

#ifdef HEARTGOLD_ABI32
static MicWork gMicWork;
#else
extern MicWork gMicWork;
#endif
extern u32 gMicLock; // label of gMicWork.lock (MicWaitBusy)

MICResult MIC_StartAutoSamplingAsync(const MICAutoParam *param, MICCallback callback, void *arg);
MICResult MIC_StartAutoSampling(const MICAutoParam *param);
MICResult MIC_StopAutoSamplingAsync(MICCallback callback, void *arg);
MICResult MIC_StopAutoSampling(void);
void *MIC_GetLastSamplingAddress(void);
void MicCommonCallback(PXIFifoTag tag, u32 data, BOOL err);
BOOL MicStartAutoSampling(u32 buffer, u32 size, u32 rate, u32 flags);
BOOL MicStopAutoSampling(void);
void MicGetResultCallback(MICResult result, void *arg);
void MicWaitBusy(void);
#ifdef HEARTGOLD_ABI32
static void MicNativeReply(u32 data, void *unused) {
    MicCommonCallback(PXI_FIFO_TAG_MIC, data, FALSE);
}
#endif

#pragma force_active on

void MIC_Init(void) {
    if (gMicWork.init) {
        return;
    }
    gMicWork.init = 1;
    gMicWork.lock = FALSE;
    gMicWork.callback = NULL;
#ifdef HEARTGOLD_ABI32
    HGNativeMicRequest(HG_MIC_INIT, 0, 0, 0, 0, 0);
#else
    PXI_Init();
    while (!PXI_IsCallbackReady(PXI_FIFO_TAG_MIC, PXI_PROC_ARM7)) {}
    MIC_LAST_SAMPLING_ADDRESS = NULL;
    PXI_SetFifoRecvCallback(PXI_FIFO_TAG_MIC, MicCommonCallback);
#endif
}

MICResult MIC_StartAutoSamplingAsync(const MICAutoParam *param, MICCallback callback, void *arg) {
    u32 flags;
    OSIntrMode enabled;
#ifdef HEARTGOLD_ABI32
    if (!param || !param->buffer) return MIC_RESULT_ILLEGAL_PARAMETER;
#endif

    if ((u32)param->buffer & 31) {
        return MIC_RESULT_ILLEGAL_PARAMETER;
    }
    if (param->size & 31) {
        return MIC_RESULT_ILLEGAL_PARAMETER;
    }
    if (!param->size) {
        return MIC_RESULT_ILLEGAL_PARAMETER;
    }
    if (param->rate < MIC_SAMPLING_RATE_LIMIT) {
        return MIC_RESULT_ILLEGAL_PARAMETER;
    }
    switch (param->type) {
    case MIC_SAMPLING_TYPE_8BIT:
        flags = 0;
        break;
    case MIC_SAMPLING_TYPE_12BIT:
        flags = 1;
        break;
    case MIC_SAMPLING_TYPE_SIGNED_8BIT:
        flags = 2;
        break;
    case MIC_SAMPLING_TYPE_SIGNED_12BIT:
        flags = 3;
        break;
    case MIC_SAMPLING_TYPE_12BIT_FILTER_OFF:
        flags = 5;
        break;
    case MIC_SAMPLING_TYPE_SIGNED_12BIT_FILTER_OFF:
        flags = 7;
        break;
    default:
        return MIC_RESULT_ILLEGAL_PARAMETER;
    }
    if (param->loop_enable) {
        flags = (u8)(flags | 16);
    } else {
        flags = (u8)flags;
    }
    flags = (u8)flags;
    enabled = OS_DisableInterrupts();
    if (gMicWork.lock) {
        OS_RestoreInterrupts(enabled);
        return MIC_RESULT_BUSY;
    }
    gMicWork.lock = TRUE;
    OS_RestoreInterrupts(enabled);
    gMicWork.callback = callback;
    gMicWork.callbackArg = arg;
    gMicWork.fullCallback = param->full_callback;
    gMicWork.fullArg = param->full_arg;
#ifdef HEARTGOLD_ABI32
    BOOL sent = MicStartAutoSampling((u32)param->buffer, param->size, param->rate, flags);
    if (!sent) { gMicWork.lock = FALSE; gMicWork.callback = NULL; }
    return sent ? MIC_RESULT_SUCCESS : MIC_RESULT_SEND_ERROR;
#else
    return MicStartAutoSampling((u32)param->buffer, param->size, param->rate, flags) ? MIC_RESULT_SUCCESS : MIC_RESULT_SEND_ERROR;
#endif
}

MICResult MIC_StartAutoSampling(const MICAutoParam *param) {
#ifdef HEARTGOLD_ABI32
    // Native IRQ completion can run before the asynchronous wrapper returns.
    gMicWork.result = MIC_RESULT_SUCCESS;
    MICResult result = MIC_StartAutoSamplingAsync(param, MicGetResultCallback, NULL);
    if (result) gMicWork.result = result;
#else
    MICResult result = MIC_StartAutoSamplingAsync(param, MicGetResultCallback, NULL);

    gMicWork.result = result;
#endif
    if (!result) {
        MicWaitBusy();
    }
    return gMicWork.result;
}

MICResult MIC_StopAutoSamplingAsync(MICCallback callback, void *arg) {
    OSIntrMode enabled = OS_DisableInterrupts();

    if (gMicWork.lock) {
        OS_RestoreInterrupts(enabled);
        return MIC_RESULT_BUSY;
    }
    gMicWork.lock = TRUE;
    OS_RestoreInterrupts(enabled);
    gMicWork.callback = callback;
    gMicWork.callbackArg = arg;
#ifdef HEARTGOLD_ABI32
    BOOL sent = MicStopAutoSampling();
    if (!sent) { gMicWork.lock = FALSE; gMicWork.callback = NULL; }
    return sent ? MIC_RESULT_SUCCESS : MIC_RESULT_SEND_ERROR;
#else
    return MicStopAutoSampling() ? MIC_RESULT_SUCCESS : MIC_RESULT_SEND_ERROR;
#endif
}

MICResult MIC_StopAutoSampling(void) {
#ifdef HEARTGOLD_ABI32
    gMicWork.result = MIC_RESULT_SUCCESS;
    MICResult result = MIC_StopAutoSamplingAsync(MicGetResultCallback, NULL);
    if (result) gMicWork.result = result;
#else
    MICResult result = MIC_StopAutoSamplingAsync(MicGetResultCallback, NULL);

    gMicWork.result = result;
#endif
    if (!result) {
        MicWaitBusy();
    }
    return gMicWork.result;
}

void *MIC_GetLastSamplingAddress(void) {
#ifdef HEARTGOLD_ABI32
    return (void *)(uaddr)HGNativeMicRequest(HG_MIC_LAST, 0, 0, 0, 0, 0);
#else
    return MIC_LAST_SAMPLING_ADDRESS;
#endif
}

#pragma push
#pragma opt_propagation off

void MicCommonCallback(PXIFifoTag tag, u32 data, BOOL err) {
    MICResult result;
    int command;
    u32 high, reply;

    if (err) {
        if (gMicWork.lock) {
            gMicWork.lock = FALSE;
        }
        if (gMicWork.callback) {
            MICCallback callback = gMicWork.callback;
            gMicWork.callback = NULL;
            callback(MIC_RESULT_FATAL_ERROR, gMicWork.callbackArg);
        }
    }
    high = data & 0x7F00;
    reply = data & 0xFF;
    command = (u16)(high >> 8);
    switch (reply) {
    case 0:
        result = MIC_RESULT_SUCCESS;
        break;
    case 1:
        result = MIC_RESULT_INVALID_COMMAND;
        break;
    case 2:
        result = MIC_RESULT_ILLEGAL_PARAMETER;
        break;
    case 3:
        result = MIC_RESULT_ILLEGAL_STATUS;
        break;
    case 4:
        result = MIC_RESULT_BUSY;
        break;
    default:
        result = MIC_RESULT_FATAL_ERROR;
        break;
    }
    if (command == 0x51) { // the auto sampling buffer is full
        if (gMicWork.fullCallback) {
            gMicWork.fullCallback(result, gMicWork.fullArg);
        }
        return;
    }
    if (command == 0x40) {
        if (gMicWork.sampleResult) {
            *gMicWork.sampleResult = MIC_SAMPLE_DATA;
        }
    }
    if (gMicWork.lock) {
        gMicWork.lock = FALSE;
    }
    if (gMicWork.callback) {
        MICCallback callback = gMicWork.callback;
        gMicWork.callback = NULL;
        callback(result, gMicWork.callbackArg);
    }
}

#pragma pop

BOOL MicStartAutoSampling(u32 buffer, u32 size, u32 rate, u32 flags) {
#ifdef HEARTGOLD_ABI32
    return HGNativeMicRequest(HG_MIC_START, buffer, size, rate, flags, (u32)(uaddr)MicNativeReply);
#else
    if (PXI_SendWordByFifo(PXI_FIFO_TAG_MIC, flags | 0x4100 | 0x2000000, FALSE) < 0) {
        return FALSE;
    }
    if (PXI_SendWordByFifo(PXI_FIFO_TAG_MIC, (buffer >> 16) | 0x10000, FALSE) < 0) {
        return FALSE;
    }
    if (PXI_SendWordByFifo(PXI_FIFO_TAG_MIC, (buffer & 0xFFFF) | 0x20000, FALSE) < 0) {
        return FALSE;
    }
    if (PXI_SendWordByFifo(PXI_FIFO_TAG_MIC, (size >> 16) | 0x30000, FALSE) < 0) {
        return FALSE;
    }
    if (PXI_SendWordByFifo(PXI_FIFO_TAG_MIC, (size & 0xFFFF) | 0x40000, FALSE) < 0) {
        return FALSE;
    }
    if (PXI_SendWordByFifo(PXI_FIFO_TAG_MIC, (rate >> 16) | 0x50000, FALSE) < 0) {
        return FALSE;
    }
    return PXI_SendWordByFifo(PXI_FIFO_TAG_MIC, 0x01060000 | (rate & 0xFFFF), FALSE) >= 0;
#endif
}

BOOL MicStopAutoSampling(void) {
#ifdef HEARTGOLD_ABI32
    return HGNativeMicRequest(HG_MIC_STOP, 0, 0, 0, 0, (u32)(uaddr)MicNativeReply);
#else
    return PXI_SendWordByFifo(PXI_FIFO_TAG_MIC, 0x03004200, FALSE) >= 0;
#endif
}

void MicGetResultCallback(MICResult result, void *arg) {
    gMicWork.result = result;
}

// clang-format off
#ifdef HEARTGOLD_ABI32
void MicWaitBusy(void) {
    while (gMicWork.lock) OS_SpinWait(1);
}
#else
asm void MicWaitBusy(void) {
    ldr r12, =gMicLock
@wait:
    ldr r0, [r12]
    cmp r0, #1
    beq @wait
    bx lr
}
#endif
// clang-format on
