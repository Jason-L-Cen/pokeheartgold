#include "CARD_request.h"
#include "OS_cache.h"
#include "OS_system.h"
#include "OS_thread.h"
#include "code32.h"
#ifdef HEARTGOLD_ABI32
#include <nitro/card/native_card.h>
#endif

extern CARDiCommon cardi_common;

void CARDi_OnFifoRecv(PXIFifoTag tag, u32 data, BOOL err)
{
#pragma unused (data)
    if ((tag == PXI_FIFO_TAG_FS) && err)
    {
        CARDiCommon *const p = &cardi_common;
        p->flag &= ~CARD_STAT_REQ;
        OS_WakeupThreadDirect(p->cur_th);
    }
}

void CARDi_TaskThread(void *arg)
{
#pragma unused (arg)
    CARDiCommon *const p = &cardi_common;
    while (TRUE)
    {
        OSIntrMode bak_psr = OS_DisableInterrupts();
        while ((p->flag & CARD_STAT_TASK) == 0)
        {
            OS_SleepThread(NULL);
        }
        (void)OS_RestoreInterrupts(bak_psr);
        (*p->task_func)(p);
    }
}


#ifdef HEARTGOLD_ABI32
static void CardNativeReply(u32 result,void *argument) {
    CARDiCommon *p=argument;
    p->cmd->result=(CARDResult)result;
    CARDi_OnFifoRecv(PXI_FIFO_TAG_FS,0,TRUE);
}
BOOL CARDi_NativeRequest(CARDiCommon *p,s32 request,s32 retry,u32 mode) {
    if (retry<1) { p->cmd->result=CARD_RESULT_INVALID_PARAM; return FALSE; }
    do {
        OSIntrMode previous=OS_DisableInterrupts();
        p->command=request; p->flag|=CARD_STAT_REQ;
        if (HGNativeCardRequest((u32)request,(u32)(uaddr)p->cmd,mode,(u32)retry,
                                (u32)(uaddr)CardNativeReply,(u32)(uaddr)p)) {
            while (p->flag&CARD_STAT_REQ) OS_SleepThread(NULL);
        } else {
            p->flag&=~CARD_STAT_REQ; p->cmd->result=CARD_RESULT_ERROR;
        }
        OS_RestoreInterrupts(previous);
    } while (p->cmd->result==CARD_RESULT_TIMEOUT && --retry>0);
    return p->cmd->result==CARD_RESULT_SUCCESS;
}
#endif

BOOL CARDi_Request(CARDiCommon *p, s32 req_type, s32 retry_count)
{
#ifdef HEARTGOLD_ABI32
    if (!(p->flag&CARD_STAT_INIT_CMD)) {
        p->flag|=CARD_STAT_INIT_CMD;
        if (!CARDi_NativeRequest(p,CARD_REQ_INIT,1,CARD_REQUEST_MODE_RECV)) return FALSE;
    }
    return CARDi_NativeRequest(p,req_type,retry_count,req_type==CARD_REQ_READ_BACKUP ? CARD_REQUEST_MODE_RECV : p->req_mode);
#else
    if ((p->flag & CARD_STAT_INIT_CMD) == 0)
    {
        p->flag |= CARD_STAT_INIT_CMD;
        while (!PXI_IsCallbackReady(PXI_FIFO_TAG_FS, PXI_PROC_ARM7))
        {
            OS_SpinWait(100);
        }

        (void)CARDi_Request(p, CARD_REQ_INIT, 1);
    }
    DC_FlushRange(p->cmd, sizeof(*p->cmd));
    DC_WaitWriteBufferEmpty();

    do
    {
        p->command = req_type;
        p->flag |= CARD_STAT_REQ;
        CARDi_SendPxi((u32)req_type);

        switch (req_type)
        {
            case CARD_REQ_INIT:
                CARDi_SendPxi((u32)p->cmd);
                break;
        }
        OSIntrMode bak_psr = OS_DisableInterrupts();
        while ((p->flag & CARD_STAT_REQ) != 0)
        {
            OS_SleepThread(NULL);
        }
        (void)OS_RestoreInterrupts(bak_psr);
        DC_InvalidateRange(p->cmd, sizeof(*p->cmd));
    }
    while ((p->cmd->result == CARD_RESULT_TIMEOUT) && (--retry_count > 0));

    return (p->cmd->result == CARD_RESULT_SUCCESS);
#endif
}
