// SPDX-License-Identifier: GPL-3.0-or-later
#include <nitro/mi/memory.h>
#include <nitro/hw/native_graphics.h>
#include <nitro/hw/mmap.h>
#include <string.h>

#define CHECK(value) do { if (!(value)) return __LINE__; } while (0)

u32 CheckNativeMemory(void) {
    u32 source[24], output[24];
    u32 span=4;
    CHECK(!HGNativeMemorySpan(0,&span));
    CHECK(!HGNativeMemorySpan(UINTPTR_MAX-1,&span));
    span=4;
    CHECK((uaddr)HGNativeMemorySpan(0x05000000,&span)==HW_PLTT && HW_PLTT_SIZE==0x400);
    u16 saved,color=0x6B5A,readback=0;
    MI_CpuCopy16((const void *)0x0500006C,&saved,2);
    ((vu16 *)HW_PLTT)[54]=color;
    MI_CpuCopy16((const void *)0x0500006C,&readback,2);
    CHECK(readback==color && HW_PLTT_END-HW_PLTT==0x400);
    ((vu16 *)HW_PLTT)[54]=saved;
    // Original battle banks: A=BG, H=sub-BG, E=OBJ, I=sub-OBJ; B/C/F/G remain textures/palettes.
    const u32 controls[]={0x240,0x241,0x242,0x243,0x244,0x245,0x246,0x248,0x249};
    const u8 battle[]={0x81,0x83,0x8B,0,0x82,0x83,0x8B,0x81,0x82};
    const u32 sizes[]={0x20000,0x20000,0x20000,0x20000,0x10000,0x4000,0x4000,0x8000,0x4000};
    u8 previous[9];
    for (u32 bank=0;bank<9;bank++) {
        vu8 *control=(vu8 *)HGNativeRegisterAddress(0x04000000+controls[bank],1);
        previous[bank]=*control; *control=battle[bank];
        memset((void *)HGNativeGraphicsBank(bank),0xA5,sizes[bank]);
    }
    MI_CpuClear32((void *)0x06000000,0x80000);
    MI_CpuClear32((void *)0x06200000,0x20000);
    MI_CpuClear32((void *)0x06400000,0x40000);
    MI_CpuClear32((void *)0x06600000,0x20000);
    for (u32 bank=0;bank<9;bank++) {
        const u8 *bytes=HGNativeGraphicsBank(bank);
        u8 expected=(bank==0 || bank==4 || bank==7 || bank==8) ? 0 : 0xA5;
        for (u32 i=0;i<sizes[bank];i++) CHECK(bytes[i]==expected);
    }
    // A mapped bank after a hole must still receive writes, including a partial final bank.
    *(vu8 *)HGNativeRegisterAddress(0x04000240,1)=0;
    *(vu8 *)HGNativeRegisterAddress(0x04000241,1)=0x91;
    MI_CpuFill16((void *)0x06020000,0x1234,0x20004);
    const u16 *later=HGNativeGraphicsBank(1);
    CHECK(later[0]==0x1234 && later[1]==0x1234 && later[2]==0xA5A5);
    *(vu8 *)HGNativeRegisterAddress(0x04000240,1)=0x91;
    MI_CpuFill32((void *)0x06040000,0x76543210,4);
    CHECK(*(const u32 *)HGNativeGraphicsBank(0)==0x76543210 && *(const u32 *)later==0x76543210);
    CHECK(!HGNativeFillVRAM(0x0607FFFC,0,8,4) && !HGNativeFillVRAM(0x04000000,0,4,4));
    CHECK(!HGNativeFillVRAM(0x06000001,0,4,4) && !HGNativeFillVRAM(UINTPTR_MAX,0,4,4));
    for (u32 bank=0;bank<9;bank++) *(vu8 *)HGNativeRegisterAddress(0x04000000+controls[bank],1)=previous[bank];
#ifdef HEARTGOLD_ABI32
    uaddr end=__builtin_wasm_memory_size(0)*65536;
    CHECK(!HGNativeMemorySpan(end-2,&span));
#endif
    for (u32 i=0;i<24;i++) source[i]=0x10203000u+i;
    for (u32 alignment=0;alignment<4;alignment++) for (u32 size=0;size<=65;size++) {
        u8 *bytes=(u8 *)output;
        memset(output,0xA5,sizeof(output));
        MI_CpuCopy8((const u8 *)source+alignment,bytes+4+alignment,size);
        CHECK(!memcmp((const u8 *)source+alignment,bytes+4+alignment,size));
        CHECK(bytes[3+alignment]==0xA5 && bytes[4+alignment+size]==0xA5);
        MI_CpuFill8(bytes+4+alignment,0x72,size);
        for (u32 i=0;i<size;i++) CHECK(bytes[4+alignment+i]==0x72);
        CHECK(bytes[3+alignment]==0xA5 && bytes[4+alignment+size]==0xA5);
    }
    for (u32 size=0;size<=65;size++) {
        u32 transferred=(size+1)&~1u;
        memset(output,0xA5,sizeof(output));
        MI_CpuCopy16(source,output,size);
        CHECK(!memcmp(source,output,transferred) && ((u8 *)output)[transferred]==0xA5);
        MI_CpuFill16(output,0xABCD,size);
        for (u32 i=0;i<transferred/2;i++) CHECK(((u16 *)output)[i]==0xABCD);
        CHECK(((u8 *)output)[transferred]==0xA5);
    }
    for (u32 size=0;size<=65;size++) {
        u32 transferred=(size+3)&~3u;
        memset(output,0xA5,sizeof(output));
        MI_CpuCopy32(source,output,size);
        CHECK(!memcmp(source,output,transferred) && ((u8 *)output)[transferred]==0xA5);
        MI_CpuFillFast(output,0x12345678,size);
        for (u32 i=0;i<transferred/4;i++) CHECK(output[i]==0x12345678);
        CHECK(((u8 *)output)[transferred]==0xA5);
        MI_CpuCopyFast(source,output,size);
        CHECK(!memcmp(source,output,transferred) && ((u8 *)output)[transferred]==0xA5);
        MI_CpuFill32(output,0x87654321,size);
        for (u32 i=0;i<transferred/4;i++) CHECK(output[i]==0x87654321);
        CHECK(((u8 *)output)[transferred]==0xA5);
        output[0]=11; output[1]=22; output[2]=33;
        MIi_CpuSend32(source,output+1,size);
        CHECK(output[0]==11 && output[1]==(transferred ? source[transferred/4-1] : 22) && output[2]==33);
    }
    const struct { void (*copy)(const void *,void *); u32 size; } fixed[] = {
        {MI_Copy32B,32},{MI_Copy36B,36},{MI_Copy48B,48},{MI_Copy64B,64}
    };
    // Golden words from the original 3-register LDM/STM groups, and the final 4-register group in Copy64B.
    const u32 overlap[]={0,0,1,2,2,4,5,5,7,8,8,10,11,11,13,14,15};
    for (u32 operation=0;operation<4;operation++) {
        memset(output,0xA5,sizeof(output));
        fixed[operation].copy(source,output);
        CHECK(!memcmp(source,output,fixed[operation].size) && ((u8 *)output)[fixed[operation].size]==0xA5);
        for (u32 i=0;i<24;i++) output[i]=i;
        fixed[operation].copy(output,output+1);
        CHECK(!memcmp(output,overlap,fixed[operation].size+4));
        CHECK(output[fixed[operation].size/4+1]==fixed[operation].size/4+1);
    }
    for (u32 i=0;i<24;i++) output[i]=i;
    MI_CpuCopyFast(output,output+1,40);
    const u32 fast[]={0,0,1,2,3,4,5,6,7,7,7};
    CHECK(!memcmp(output,fast,sizeof(fast)) && output[11]==11);
    for (u32 i=0;i<24;i++) output[i]=i;
    MI_CpuCopy32(output,output+1,20);
    for (u32 i=0;i<6;i++) CHECK(output[i]==0);
    CHECK(output[6]==6);
    MI_Zero36B(output+1);
    for (u32 i=1;i<=9;i++) CHECK(output[i]==0);
    CHECK(output[10]==10);
    MIi_CpuSend32(NULL,NULL,0);
    MI_CpuCopyFast(NULL,NULL,0);
    volatile u8 *controlA=(volatile u8 *)HGNativeRegisterAddress(0x04000240,1);
    volatile u8 *controlB=(volatile u8 *)HGNativeRegisterAddress(0x04000241,1);
    volatile u8 *controlC=(volatile u8 *)HGNativeRegisterAddress(0x04000242,1);
    u8 oldA=*controlA, oldB=*controlB, oldC=*controlC;
    *controlA=0x81; *controlB=0; *controlC=0x89;
    for (u32 i=0;i<24;i++) output[i]=i;
    MI_CpuCopy32(output,(void *)0x0601FFF0,80);
    MI_Copy64B((const void *)0x0601FFF0,(void *)0x0601FFF4);
    HGNativeSnapshotGraphics(0x0601FFF0,output,68);
    CHECK(!memcmp(output,overlap,sizeof(overlap)));
    *controlA=oldA; *controlB=oldB; *controlC=oldC;
    return 0;
}
