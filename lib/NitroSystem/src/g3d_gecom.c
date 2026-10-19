#include <nitro.h>
#include <nnsys/g3d/gecom.h>
#include <nnsys/g3d_util_internal.h>
#ifdef HEARTGOLD_NATIVE
#include <nitro/gx/native_geometry.h>
#include "assert.h"
static void nativeSend(const void *words,u32 count) {
    BOOL success=HGNativeGeometryFifo(words,count); GF_ASSERT(success);
}
#endif

#pragma force_active on

// sends the buffered geometry commands
void NNS_G3dGeFlushBuffer(void) {
    if (gGeBufferState.isLocked) {
        NNS_G3dGeWaitSendDL();
    }
    if (gGeBufferState.pBuffer && gGeBufferState.pBuffer[0]) {
#ifdef HEARTGOLD_NATIVE
        GF_ASSERT(gGeBufferState.pBuffer[0]<=G3D_GE_BUFFER_WORDS);
        nativeSend(gGeBufferState.pBuffer+1,gGeBufferState.pBuffer[0]);
#else
        MIi_CpuSend32(gGeBufferState.pBuffer + 1, &reg_G3X_GXFIFO, gGeBufferState.pBuffer[0] * sizeof(u32));
#endif
        gGeBufferState.pBuffer[0] = 0;
    }
}

// waits for the display list DMA
void NNS_G3dGeWaitSendDL(void) {
#ifdef HEARTGOLD_NATIVE
    // shortcut: native submission is synchronous; restore asynchronous DMA ordering when required by callers.
    GF_ASSERT(!gGeBufferState.isLocked);
#else
    while (gGeBufferState.isLocked) {
    }
#endif
}

// DMA completion callback
void simpleUnlock_(void *pLocked) {
    *(u32 *)pLocked = 0;
}

// sends a display list: small ones (or without a DMA channel) through the buffer, others by DMA
void NNS_G3dGeSendDL(const void *pDL, u32 size) {
#ifdef HEARTGOLD_NATIVE
    if (!size) return;
    GF_ASSERT(pDL && (uaddr)pDL%4==0 && size%4==0);
    NNS_G3dGeFlushBuffer(); nativeSend(pDL,size/4);
#else
    if (size < 256 || GXi_DmaId == (u32)-1) {
        NNS_G3dGeBufferOP_N(((const u32 *)pDL)[0], (const u32 *)pDL + 1, (size >> 2) - 1);
        return;
    }
    NNS_G3dGeFlushBuffer();
    gGeBufferState.isLocked = TRUE;
    if (gGeBufferState.useFastDma) {
        MI_SendGXCommandAsyncFast(GXi_DmaId, pDL, size, simpleUnlock_, (void *)&gGeBufferStateIsLocked);
    } else {
        MI_SendGXCommandAsync(GXi_DmaId, pDL, size, simpleUnlock_, (void *)&gGeBufferStateIsLocked);
    }
#endif
}

// queues a geometry command while a display list is being sent, otherwise sends it directly
void NNS_G3dGeBufferOP_N(u32 op, const u32 *args, u32 num) {
    if (gGeBufferState.pBuffer) {
#ifdef HEARTGOLD_NATIVE
        u32 used=gGeBufferState.pBuffer[0]; GF_ASSERT(used<=G3D_GE_BUFFER_WORDS);
        if (gGeBufferState.isLocked && used<G3D_GE_BUFFER_WORDS && num<=G3D_GE_BUFFER_WORDS-used-1) {
#else
        if (gGeBufferState.isLocked && gGeBufferState.pBuffer[0] + 1 + num <= G3D_GE_BUFFER_WORDS) {
#endif
            u32 pos = gGeBufferState.pBuffer[0]++;
            gGeBufferState.pBuffer[pos + 1] = op;
            if (num) {
                MIi_CpuCopyFast(args, gGeBufferState.pBuffer + 1 + gGeBufferState.pBuffer[0], num * sizeof(u32));
                gGeBufferState.pBuffer[0] += num;
            }
            return;
        }
        if (gGeBufferState.pBuffer[0]) {
            NNS_G3dGeFlushBuffer();
        } else if (gGeBufferState.isLocked) {
            NNS_G3dGeWaitSendDL();
        }
    } else if (gGeBufferState.isLocked) {
        NNS_G3dGeWaitSendDL();
    }
#ifdef HEARTGOLD_NATIVE
    BOOL success=HGNativeGeometryPacket(op,args,num); GF_ASSERT(success);
#else
    reg_G3X_GXFIFO = op;
    MIi_CpuSend32(args, &reg_G3X_GXFIFO, num * sizeof(u32));
#endif
}

// reads the current position (pPos) and direction (pDir) matrices
void NNS_G3dGetCurrentMtx(MtxFx43 *pPos, MtxFx33 *pDir) {
    MtxFx44 clip;
    NNS_G3dGeFlushBuffer();
    // clip matrix with an identity projection = position matrix
#ifdef HEARTGOLD_NATIVE
    G3_MtxMode(GX_MTXMODE_PROJECTION); G3_PushMtx(); G3_Identity();
#else
    reg_G3_MTX_MODE = GX_MTXMODE_PROJECTION;
    reg_G3_MTX_PUSH = 0;
    reg_G3_MTX_IDENTITY = 0;
#endif
    if (pPos) {
        while (G3X_GetClipMtx(&clip)) {
        }
        MTX_Copy44To43_(&clip, pPos);
    }
    if (pDir) {
        while (G3X_GetVectorMtx(pDir)) {
        }
    }
#ifdef HEARTGOLD_NATIVE
    G3_PopMtx(1); G3_MtxMode(GX_MTXMODE_POSITION_VECTOR);
#else
    reg_G3_MTX_POP = 1;
    reg_G3_MTX_MODE = GX_MTXMODE_POSITION_VECTOR;
#endif
}
