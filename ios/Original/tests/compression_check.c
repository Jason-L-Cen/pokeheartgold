// SPDX-License-Identifier: GPL-3.0-or-later
#include <nitro.h>
#include <nitro/hw/native_graphics.h>
#include "../NativeCompression.h"
#include "bg_window.h"
#include <string.h>
#define CHECK(v) do { if (!(v)) return __LINE__; } while (0)

u32 CheckOriginalUncompress(void) {
    static const u8 lz10[]={0x10,9,0,0,0x10,'A','B','C',0x30,2};
    static const u8 short11[]={0x11,4,0,0,0x40,'A',0x20,0};
    static const u8 medium11[]={0x11,18,0,0,0x40,'A',0,0,0};
    static const u8 long11[]={0x11,0x12,1,0,0x40,'A',0x10,0,0,0};
    static const u8 badDistance[]={0x10,3,0,0,0x80,0,0};
    static const u8 badLength[]={0x10,4,0,0,0x40,'A',0xF0,0};
    static const u8 empty[]={0x10,0,0,0},wrong[]={0x12,0,0,0};
    static u8 output[4200],input[4620],snapshot[32];
    memset(output,0xA5,sizeof(output));
    MI_UncompressLZ8(lz10,output+1);
    CHECK(!memcmp(output+1,"ABCABCABC",9) && output[0]==0xA5 && output[10]==0xA5);
    BgConfig background={0}; background.bgs[GF_BG_LYR_MAIN_0].tilemapBuffer=output+1;
    memset(output,0xA5,sizeof(output));
    BG_LoadScreenTilemapData(&background,GF_BG_LYR_MAIN_0,lz10,0);
    CHECK(!memcmp(output+1,"ABCABCABC",9) && output[0]==0xA5 && output[10]==0xA5);
    const u8 *variants[]={short11,medium11,long11};
    const u32 sizes[]={4,18,274};
    for (u32 i=0;i<3;i++) {
        memset(output,0xA5,sizeof(output)); MI_UncompressLZ8(variants[i],output+1);
        for (u32 j=0;j<sizes[i];j++) CHECK(output[1+j]=='A');
        CHECK(output[0]==0xA5 && output[1+sizes[i]]==0xA5);
    }
    CHECK(HGNativeUncompressLZMapped(empty,NULL));
    CHECK(!HGNativeUncompressLZMapped(NULL,output));
    CHECK(!HGNativeUncompressLZMapped(wrong,output));
    memset(output,0xA5,sizeof(output));
    CHECK(!HGNativeUncompressLZMapped(badDistance,output));
    CHECK(!HGNativeUncompressLZMapped(badLength,output));
    for (u32 i=0;i<sizeof(output);i++) CHECK(output[i]==0xA5);
    memcpy(input,lz10,sizeof(lz10));
    CHECK(!HGNativeUncompressLZMapped(input,input+1) && !memcmp(input,lz10,sizeof(lz10)));

    // Largest LZ10 backward distance, with literal flag groups across the 4 KiB boundary.
    input[0]=0x10; input[1]=3; input[2]=16; input[3]=0;
    u32 used=4;
    for (u32 i=0;i<4096;i+=8) {
        input[used++]=0;
        for (u32 j=0;j<8;j++) input[used++]=(u8)(i+j);
    }
    input[used++]=0x80; input[used++]=0x0F; input[used++]=0xFF;
    memset(output,0xA5,sizeof(output)); MI_UncompressLZ8(input,output+1);
    for (u32 i=0;i<4099;i++) CHECK(output[1+i]==(u8)i);
    CHECK(output[0]==0xA5 && output[4100]==0xA5);

    GXVRamBG previous=GX_ResetBankForBG(); GX_SetBankForBG(GX_VRAM_BG_256_AB);
    MI_CpuFill8((void *)0x0601FFF8,0xA5,24);
    MI_UncompressLZ8(lz10,(void *)0x0601FFFC);
    HGNativeSnapshotGraphics(0x0601FFF8,snapshot,24);
    CHECK(!memcmp(snapshot+4,"ABCABCABC",9) && snapshot[3]==0xA5 && snapshot[13]==0xA5);
    background.bgs[GF_BG_LYR_MAIN_0].tilemapBuffer=(void *)0x0601FFFC;
    MI_CpuFill8((void *)0x0601FFF8,0xA5,24);
    BG_LoadScreenTilemapData(&background,GF_BG_LYR_MAIN_0,lz10,0);
    HGNativeSnapshotGraphics(0x0601FFF8,snapshot,24);
    CHECK(!memcmp(snapshot+4,"ABCABCABC",9) && snapshot[3]==0xA5 && snapshot[13]==0xA5);
    MI_CpuCopy8(lz10,(void *)0x0601FFFC,sizeof(lz10));
    memset(output,0xA5,sizeof(output)); MI_UncompressLZ8((void *)0x0601FFFC,output+1);
    CHECK(!memcmp(output+1,"ABCABCABC",9) && output[0]==0xA5 && output[10]==0xA5);
    // An ordinary bank pointer and a physical address may alias despite different numeric addresses.
    MI_CpuCopy8(lz10,(void *)0x06000000,sizeof(lz10));
    CHECK(!HGNativeUncompressLZMapped(HGNativeGraphicsBank(0),(void *)0x06000001));
    HGNativeSnapshotGraphics(0x06000000,snapshot,sizeof(lz10));
    CHECK(!memcmp(snapshot,lz10,sizeof(lz10)));
    MI_CpuFill8((void *)0x050007FC,0xA5,4);
    CHECK(!HGNativeUncompressLZMapped(lz10,(void *)0x050007FC));
    HGNativeSnapshotGraphics(0x050007FC,snapshot,4);
    for (u32 i=0;i<4;i++) CHECK(snapshot[i]==0xA5);
    CHECK(!HGNativeUncompressLZMapped((void *)0x02000000,output));
    GX_SetBankForBG(previous);
    return 0;
}
