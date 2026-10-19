#include <nitro.h>
#include <nitro/code32.h>
#include <nitro/section.h>

#ifdef HEARTGOLD_ABI32
#include <nitro/mi/native_dma.h>
#include <nitro/hw/native_graphics.h>
#include "assert.h"

static void rangesForAddress(HGNativeDMARange *ranges, u32 *count, uaddr address, u32 size, u32 width, u32 mode) {
    GF_ASSERT(size && size % width == 0 && address % width == 0);
    if (mode == 2) size = width;
    else if (mode == 1) { GF_ASSERT(address >= size - width); address -= size - width; }
    GF_ASSERT(address <= UINT32_MAX - size);
    while (size) {
        u32 part = size;
        void *data = HGNativeMemorySpan(address, &part);
        GF_ASSERT(data && part && part % width == 0 && *count < HG_NATIVE_DMA_MAX_SPANS);
        ranges[(*count)++] = (HGNativeDMARange){address, (u32)data, part};
        address += part; size -= part;
    }
}
static void parameters(u32 number, uaddr source, uaddr destination, u32 control, BOOL wait, BOOL fill) {
    GF_ASSERT(number < 4);
    if (!(control & MI_DMA_ENABLE)) {
        HGNativeDmaRequest(HG_DMA_STOP, number, 0, 0, 0, NULL, 0); return;
    }
    GF_ASSERT(!(control & (MI_DMA_TIMING_MASK | MI_DMA_CONTINUOUS_ON)));
    u32 width = (control & MI_DMA_32BIT_BUS) ? 4 : 2;
    u32 elements = control & 0x1FFFFF;
    if (!elements) elements = 0x200000;
    u32 sourceMode = (control >> 23) & 3, destinationMode = (control >> 21) & 3;
    GF_ASSERT(sourceMode != 3 && (!fill || sourceMode == 2));
    HGNativeDMARange ranges[HG_NATIVE_DMA_MAX_SPANS];
    u32 count = 0;
    OSIntrMode previous = OS_DisableInterrupts();
    if (!fill) rangesForAddress(ranges, &count, source, elements * width, width, sourceMode);
    rangesForAddress(ranges, &count, destination, elements * width, width, destinationMode);
    HGNativeDmaRequest((fill ? HG_DMA_FILL : HG_DMA_TRANSFER) + !!wait, number,
                       source, destination, control, ranges, count);
    OS_RestoreInterrupts(previous);
}
void MIi_DmaSetParams(u32 n, uaddr s, uaddr d, u32 c) { parameters(n,s,d,c,FALSE,FALSE); }
void MIi_DmaSetParams_noInt(u32 n, uaddr s, uaddr d, u32 c) { parameters(n,s,d,c,FALSE,FALSE); }
void MIi_DmaSetParams_wait(u32 n, uaddr s, uaddr d, u32 c) { parameters(n,s,d,c,TRUE,FALSE); }
void MIi_DmaSetParams_wait_noInt(u32 n, uaddr s, uaddr d, u32 c) { parameters(n,s,d,c,TRUE,FALSE); }
void MIi_DmaSetParams_src32(u32 n, u32 v, uaddr d, u32 c) { parameters(n,v,d,c,FALSE,TRUE); }
void MIi_DmaSetParams_wait_src32(u32 n, u32 v, uaddr d, u32 c) { parameters(n,v,d,c,TRUE,TRUE); }
void MI_WaitDma(u32 n) { HGNativeDmaRequest(HG_DMA_WAIT,n,0,0,0,NULL,0); }
void MI_StopDma(u32 n) { HGNativeDmaRequest(HG_DMA_STOP,n,0,0,0,NULL,0); }
void MIi_CheckAnotherAutoDMA(u32 n, u32 timing) { HGNativeDmaRequest(HG_DMA_CHECK_AUTO,n,0,0,timing,NULL,0); }
void MIi_CheckDma0SourceAddress(u32 n, uaddr source, u32 size, u32 direction) {
    GF_ASSERT(n < 4);
    if (!size) return;
    u32 mode = (direction >> 23) & 3, count = 0;
    GF_ASSERT(mode != 3);
    HGNativeDMARange ranges[HG_NATIVE_DMA_MAX_SPANS];
    rangesForAddress(ranges, &count, source, size, 2, mode);
}
void MI_DmaCopy32(u32 n, const void *source, void *destination, u32 size) {
    GF_ASSERT(size % 4 == 0);
    MIi_CheckDma0SourceAddress(n,(uaddr)source,size,MI_DMA_SRC_INC);
    if (!size) return;
    MI_WaitDma(n); parameters(n,(uaddr)source,(uaddr)destination,MI_CNT_COPY32(size),TRUE,FALSE);
}
void MI_DmaCopy16(u32 n, const void *source, void *destination, u32 size) {
    if (!size) return;
    MIi_CheckDma0SourceAddress(n,(uaddr)source,size,MI_DMA_SRC_INC);
    MI_WaitDma(n); parameters(n,(uaddr)source,(uaddr)destination,MI_CNT_COPY16(size),TRUE,FALSE);
}
void MI_DmaFill32(u32 n, void *destination, u32 value, u32 size) {
    GF_ASSERT(size % 4 == 0);
    if (!size) return;
    MI_WaitDma(n); parameters(n,value,(uaddr)destination,MI_CNT_CLEAR32(size),TRUE,TRUE);
}
void MI_DmaCopy32Async(u32 n, const void *source, void *destination, u32 size, MIDmaCallback callback, void *argument) {
    GF_ASSERT(size % 4 == 0);
    MIi_CheckDma0SourceAddress(n,(uaddr)source,size,MI_DMA_SRC_INC);
    if (!size) { MIi_CallCallback(callback,argument); return; }
    MI_WaitDma(n);
    if (callback) OSi_EnterDmaCallback(n,callback,argument);
    parameters(n,(uaddr)source,(uaddr)destination,callback ? MI_CNT_COPY32_IF(size) : MI_CNT_COPY32(size),FALSE,FALSE);
}
void MI_DmaFill32Async(u32 n, void *destination, u32 value, u32 size, MIDmaCallback callback, void *argument) {
    GF_ASSERT(size % 4 == 0);
    if (!size) { MIi_CallCallback(callback,argument); return; }
    MI_WaitDma(n);
    if (callback) OSi_EnterDmaCallback(n,callback,argument);
    parameters(n,value,(uaddr)destination,callback ? MI_CNT_CLEAR32_IF(size) : MI_CNT_CLEAR32(size),FALSE,TRUE);
}
#else
#pragma section ITCM begin
void MIi_DmaSetParams(u32 dmaNo, uaddr src, uaddr dest, u32 ctrl) {
    OSIntrMode lastIntrMode = OS_DisableInterrupts();
    vu32 *p = (vu32 *)((u32)REG_DMA0SAD_ADDR + dmaNo * 12);
    *p = (vu32)src;
    *(p + 1) = (vu32)dest;
    *(p + 2) = (vu32)ctrl;
    (void)OS_RestoreInterrupts(lastIntrMode);
}

void MIi_DmaSetParams_wait(u32 dmaNo, uaddr src, uaddr dest, u32 ctrl) {
    OSIntrMode enabled = OS_DisableInterrupts();
    vu32 *p = (vu32 *)((u32)REG_DMA0SAD_ADDR + dmaNo * 12);
    *p = (vu32)src;
    *(p + 1) = (vu32)dest;
    *(p + 2) = (vu32)ctrl;

    // delay cycles
    {
        u32 delay = reg_MI_DMA0SAD;
    }
    {
        u32 delay = reg_MI_DMA0SAD;
    }

    if (!dmaNo) {
        *p = (vu32)0;
        *(p + 1) = (vu32)0;
        *(p + 2) = (vu32)0x81400001;
    }

    (void)OS_RestoreInterrupts(enabled);
}

void MIi_DmaSetParams_noInt(u32 dmaNo, uaddr src, uaddr dest, u32 ctrl) {
    vu32 *p = (vu32 *)((u32)REG_DMA0SAD_ADDR + dmaNo * 12);
    *p = (vu32)src;
    *(p + 1) = (vu32)dest;
    *(p + 2) = (vu32)ctrl;
}

void MIi_DmaSetParams_wait_noInt(u32 dmaNo, uaddr src, uaddr dest, u32 ctrl) {
    vu32 *p = (vu32 *)((u32)REG_DMA0SAD_ADDR + dmaNo * 12);
    *p = (vu32)src;
    *(p + 1) = (vu32)dest;
    *(p + 2) = (vu32)ctrl;

    // delay cycles
    {
        u32 delay = reg_MI_DMA0SAD;
    }
    {
        u32 delay = reg_MI_DMA0SAD;
    }

    if (!dmaNo) {
        *p = (vu32)0;
        *(p + 1) = (vu32)0;
        *(p + 2) = (vu32)0x81400001;
    }

    // delay cycles
    {
        u32 delay = reg_MI_DMA0SAD;
    }
    {
        u32 delay = reg_MI_DMA0SAD;
    }
}
#pragma section ITCM end

void MI_DmaFill32(u32 dmaNo, void *dest, u32 data, u32 size) {
    vu32 *dmaCntp;
    if (!size) {
        return;
    }

    MIi_WAIT_BEFOREDMA(dmaCntp, dmaNo);

    MIi_DmaSetParams_wait_src32(dmaNo, data, (u32)dest, MI_CNT_CLEAR32(size));

    do {
        while (*dmaCntp & 0x80000000) {}
    } while (0);
}

void MI_DmaCopy32(u32 dmaNo, const void *src, void *dest, u32 size) {
    vu32 *dmaCntp;
    MIi_CheckDma0SourceAddress(dmaNo, (u32)src, size, MI_DMA_SRC_INC);

    if (!size) {
        return;
    }

    MIi_WAIT_BEFOREDMA(dmaCntp, dmaNo);

    MIi_DmaSetParams_wait(dmaNo, (u32)src, (u32)dest, MI_CNT_COPY32(size));

    do {
        while (*dmaCntp & 0x80000000) {}
    } while (0);
}

void MI_DmaCopy16(u32 dmaNo, const void *src, void *dest, u32 size) {
    vu32 *dmaCntp;

    if (!size) {
        return;
    }

    MIi_CheckDma0SourceAddress(dmaNo, (u32)src, size, MI_DMA_SRC_INC);

    MIi_WAIT_BEFOREDMA(dmaCntp, dmaNo);

    MIi_DmaSetParams_wait(dmaNo, (u32)src, (u32)dest, MI_CNT_COPY16(size));

    do {
        while (*dmaCntp & 0x80000000) {}
    } while (0);
}

void MI_DmaFill32Async(u32 dmaNo, void *dest, u32 data, u32 size, MIDmaCallback callback, void *arg) {
    if (!size) {
        MIi_CallCallback(callback, arg);
    } else {
        MI_WaitDma(dmaNo);

        if (callback) {
            OSi_EnterDmaCallback(dmaNo, callback, arg);
            MIi_DmaSetParams_src32(dmaNo, data, (u32)dest, MI_CNT_CLEAR32_IF(size));
        } else {
            MIi_DmaSetParams_src32(dmaNo, data, (u32)dest, MI_CNT_CLEAR32(size));
        }
    }
}

void MI_DmaCopy32Async(u32 dmaNo, const void *src, void *dest, u32 size, MIDmaCallback callback, void *arg) {
    MIi_CheckDma0SourceAddress(dmaNo, (u32)src, size, MI_DMA_SRC_INC);

    if (!size) {
        MIi_CallCallback(callback, arg);
    } else {
        MI_WaitDma(dmaNo);

        if (callback) {
            OSi_EnterDmaCallback(dmaNo, callback, arg);
            MIi_DmaSetParams(dmaNo, (u32)src, (u32)dest, MI_CNT_COPY32_IF(size));
        } else {
            MIi_DmaSetParams(dmaNo, (u32)src, (u32)dest, MI_CNT_COPY32(size));
        }
    }
}

void MI_WaitDma(u32 dmaNo) {
    OSIntrMode lastIntrMode = OS_DisableInterrupts();
    vu32 *dmaCntp;
    MIi_WAIT_BEFOREDMA(dmaCntp, dmaNo);

    if (!dmaNo) {
        vu32 *p = (vu32 *)((u32)REG_DMA0SAD_ADDR + dmaNo * 12);
        *p = (vu32)0;
        *(p + 1) = (vu32)0;
        *(p + 2) = (vu32)0x81400001;
    }

    (void)OS_RestoreInterrupts(lastIntrMode);
}

void MI_StopDma(u32 dmaNo) {
    OSIntrMode lastIntrMode = OS_DisableInterrupts();
    vu16 *dmaCntp = &((vu16 *)REG_DMA0SAD_ADDR)[dmaNo * 6 + 5];

    *dmaCntp &= ~0x3a00;
    *dmaCntp &= ~0x8000;

    // delay cycles
    {
        s32 delay = dmaCntp[0];
    }
    {
        s32 delay = dmaCntp[0];
    }

    if (!dmaNo) {
        vu32 *p = (vu32 *)((u32)REG_DMA0SAD_ADDR + dmaNo * 12);
        *p = (vu32)0;
        *(p + 1) = (vu32)0;
        *(p + 2) = (vu32)0x81400001;
    }

    (void)OS_RestoreInterrupts(lastIntrMode);
}

void MIi_CheckAnotherAutoDMA(u32 dmaNo, u32 dmaType) {
    u32 dmaCnt;
    u32 timing;
    for (int i = 0; i < 3; i++) {
        if (i == dmaNo) {
            continue;
        }

        dmaCnt = *(REGType32v *)(REG_ADDR_DMA0CNT + i * 12);

        if (!(dmaCnt & 0x80000000)) {
            continue;
        }

        timing = dmaCnt & 0x38000000;

        if (timing == dmaType
            || (timing == 0x8000000 && dmaType == MI_DMA_TIMING_H_BLANK)
            || (timing == MI_DMA_TIMING_H_BLANK && dmaType == 0x8000000)) {
            continue;
        }

        if (timing == 0x18000000
            || timing == 0x20000000
            || timing == 0x28000000
            || timing == 0x30000000
            || timing == 0x38000000
            || timing == 0x8000000
            || timing == 0x10000000) {
            OS_Terminate();
        }
    }
}

void MIi_CheckDma0SourceAddress(u32 dmaNo, uaddr src, u32 size, u32 dir) {
    if (!dmaNo) {
        u32 addStart = src & 0xff000000;
        u32 addEnd;

        switch (dir) {
        case 0: // dma_src_inc
            addEnd = src + size;
            break;
        case 0x800000: // dma_src_dec
            addEnd = src - size;
            break;
        default:
            addEnd = src;
            break;
        }
        addEnd &= 0xff000000;

        if (addStart == 0x04000000 || addStart >= 0x08000000 || addEnd == 0x04000000 || addEnd >= 0x08000000) {
            OS_Terminate();
        }
    }
}
#endif
