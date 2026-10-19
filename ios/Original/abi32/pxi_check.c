// SPDX-License-Identifier: GPL-3.0-or-later
#include <nitro.h>
#include <stddef.h>
#include <string.h>
#define CHECK(v) do { if (!(v)) return __LINE__; } while (0)
_Static_assert(sizeof(OSSystemWork)==0x400,"Original system work layout");
_Static_assert(offsetof(OSSystemWork,pxiHandleChecker)==0x388,"Original PXI flags");
_Static_assert(offsetof(OSSystemWork,command_area)==0x3FE,"Original system work end");
static u32 callbacks;
static void callback(PXIFifoTag tag,u32 data,BOOL error) { callbacks++; }
u32 HGSystemWorkAddress(void) { return (u32)(uaddr)OS_GetSystemWork(); }
u32 CheckNativePXI(void) {
    OSSystemWork *work=OS_GetSystemWork();
    CHECK(work && !((u32)(uaddr)work&31) && (u32)(uaddr)work!=HW_MAIN_MEM_SYSTEM);
    OSSystemWork saved=*work;
    OSIrqMask mask=OS_GetIrqMask();
    OSIntrMode previous=OS_DisableInterrupts();
    work->resetParameter=0xAABBCCDD; work->pxiHandleChecker[PXI_PROC_ARM7]=0;
    PXI_InitFifo(); CHECK(OS_GetCpsrIrq()==OS_INTRMODE_IRQ_DISABLE);
    CHECK(!work->pxiHandleChecker[PXI_PROC_ARM9] && !work->pxiHandleChecker[PXI_PROC_ARM7]);
    PXI_SetFifoRecvCallback(PXI_FIFO_TAG_USER_0,callback);
    PXI_SetFifoRecvCallback(31,callback);
    CHECK(PXI_IsCallbackReady(PXI_FIFO_TAG_USER_0,PXI_PROC_ARM9) && PXI_IsCallbackReady(31,PXI_PROC_ARM9));
    CHECK(!PXI_IsCallbackReady(PXI_FIFO_TAG_USER_0,PXI_PROC_ARM7));
    PXI_InitFifo(); CHECK(PXI_IsCallbackReady(31,PXI_PROC_ARM9));
    PXI_SetFifoRecvCallback(-1,callback); PXI_SetFifoRecvCallback(32,callback);
    CHECK(!PXI_IsCallbackReady(-1,PXI_PROC_ARM9) && !PXI_IsCallbackReady(32,PXI_PROC_ARM9));
    CHECK(!PXI_IsCallbackReady(31,(PXIProc)2));
    CHECK(PXI_SendWordByFifo(PXI_FIFO_TAG_USER_0,0x3FFFFFF,FALSE)==PXI_FIFO_FAIL_SEND_ERR);
    PXIi_HandlerRecvFifoNotEmpty(); CHECK(!callbacks);
    CHECK(OS_GetIrqMask()==mask && work->resetParameter==0xAABBCCDD);
    PXI_SetFifoRecvCallback(31,NULL); PXI_SetFifoRecvCallback(PXI_FIFO_TAG_USER_0,NULL);
    CHECK(!work->pxiHandleChecker[PXI_PROC_ARM9] && OS_GetCpsrIrq()==OS_INTRMODE_IRQ_DISABLE);
    *work=saved; OS_RestoreInterrupts(previous);
    return 0;
}
