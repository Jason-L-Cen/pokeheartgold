// SPDX-License-Identifier: GPL-3.0-or-later
#include <nitro.h>
#include <nitro/mi/native_dma.h>
#include <nitro/hw/native_graphics.h>
#include <string.h>
#define CHECK(v) do { if (!(v)) return __LINE__; } while (0)

static u32 source[16], destination[17], calls, badMode;
static HGNativeDMARange ranges[2];
static void completed(void *argument) {
    if (OS_GetProcMode()!=OS_PROCMODE_IRQ || argument!=&calls) badMode++;
    MI_WaitDma(3); calls++;
}
static void zeroLength(void *argument) {
    if (OS_GetProcMode()!=OS_PROCMODE_SYS) badMode++;
    (*(u32 *)argument)++;
}
u32 HGDMAReference(u32 field) {
    if (!field) return (u32)source;
    if (field==1) return (u32)destination;
    if (field==2) {
        ranges[0]=(HGNativeDMARange){(u32)source,(u32)source,32};
        ranges[1]=(HGNativeDMARange){(u32)destination,(u32)destination,32};
        return (u32)ranges;
    }
    return calls;
}
u32 CheckOriginalBridgedDMA(void) {
    for (u32 i=0;i<16;i++) source[i]=0x12340000u+i;
    memset(destination,0xA5,sizeof(destination));
    MI_DmaCopy32(0,source,destination,64);
    CHECK(!memcmp(source,destination,64) && destination[16]==0xA5A5A5A5);
    MI_DmaFill32(1,destination,0xCC331177,64);
    for (u32 i=0;i<16;i++) CHECK(destination[i]==0xCC331177);
    MI_DmaCopy16(2,source,destination,32);
    CHECK(!memcmp(source,destination,32) && destination[8]==0xCC331177 && destination[16]==0xA5A5A5A5);
    MIi_DmaSetParams_wait(0,(uaddr)(source+7),(uaddr)(destination+7),
        MI_CNT_COPY32(32)|MI_DMA_SRC_DEC|MI_DMA_DEST_DEC);
    CHECK(!memcmp(source,destination,32));
    MIi_DmaSetParams_wait(0,(uaddr)(source+3),(uaddr)destination,MI_CNT_COPY32(32)|MI_DMA_SRC_FIX);
    for (u32 i=0;i<8;i++) CHECK(destination[i]==source[3]);
    MIi_DmaSetParams_wait(0,(uaddr)source,(uaddr)destination,MI_CNT_COPY32(32)|MI_DMA_DEST_FIX);
    CHECK(destination[0]==source[7] && destination[1]==source[3]);
    MIi_DmaSetParams_wait_src32(2,0xABCD1357,(uaddr)destination,MI_DMA_ENABLE|MI_DMA_SRC_FIX|16);
    for (u32 i=0;i<16;i++) CHECK(((u16 *)destination)[i]==0x1357);
    CHECK(destination[16]==0xA5A5A5A5);
    u32 overlap[8]={1,2,3,4,5,6,7,8};
    MI_DmaCopy32(1,overlap,overlap+1,20);
    for (u32 i=0;i<6;i++) CHECK(overlap[i]==1);
    CHECK(overlap[6]==7 && overlap[7]==8);

    u8 bankA=reg_GX_VRAMCNT_A, bankB=reg_GX_VRAMCNT_B;
    reg_GX_VRAMCNT_A=0x81; reg_GX_VRAMCNT_B=0x89;
    _Alignas(4) u8 saved[32], output[32];
    HGNativeSnapshotGraphics(0x0601FFF0,saved,32);
    MI_DmaCopy32(2,source,(void *)0x0601FFF0,32);
    HGNativeSnapshotGraphics(0x0601FFF0,output,32); CHECK(!memcmp(source,output,32));
    MI_DmaCopy32(2,(const void *)0x0601FFF0,destination,32); CHECK(!memcmp(source,destination,32));
    MI_CpuCopy32(saved,(void *)0x0601FFF0,32);
    reg_GX_VRAMCNT_A=bankA; reg_GX_VRAMCNT_B=bankB;

    OS_InitIrqTable(); OS_DisableIrq(); calls=badMode=0;
    MI_DmaFill32Async(3,destination,42,64,completed,&calls); MI_WaitDma(3);
    CHECK(!calls && (OS_GetRequestIrqMask()&OS_IE_DMA3));
    for (u32 i=0;i<16;i++) CHECK(destination[i]==42);
    OS_EnableIrq(); CHECK(calls==1 && !badMode);
    MI_DmaCopy32Async(3,source,destination,64,completed,&calls); MI_WaitDma(3);
    CHECK(calls==2 && !badMode && !memcmp(source,destination,64));
    u32 zeroCalls=0;
    MI_DmaCopy32Async(3,NULL,NULL,0,zeroLength,&zeroCalls);
    MI_DmaFill32Async(3,NULL,0,0,zeroLength,&zeroCalls);
    CHECK(zeroCalls==2 && !badMode && destination[16]==0xA5A5A5A5);
    MI_StopDma(3); OS_DisableIrq(); OS_InitIrqTable();
    return 0;
}
