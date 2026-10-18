// SPDX-License-Identifier: GPL-3.0-or-later
#include <nitro/mi/memory.h>
#include <nitro/hw/native_graphics.h>
#include <string.h>

#define CHECK(value) do { if (!(value)) return __LINE__; } while (0)

u32 CheckNativeMemory(void) {
    u32 source[24], output[24];
    u32 span=4;
    CHECK(!HGNativeMemorySpan(0,&span));
    CHECK(!HGNativeMemorySpan(UINTPTR_MAX-1,&span));
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
    for (u32 size=0;size<=64;size+=2) {
        memset(output,0xA5,sizeof(output));
        MI_CpuCopy16(source,output,size);
        CHECK(!memcmp(source,output,size) && ((u8 *)output)[size]==0xA5);
        MI_CpuFill16(output,0xABCD,size);
        for (u32 i=0;i<size/2;i++) CHECK(((u16 *)output)[i]==0xABCD);
        CHECK(((u8 *)output)[size]==0xA5);
    }
    for (u32 size=0;size<=64;size+=4) {
        memset(output,0xA5,sizeof(output));
        MI_CpuCopy32(source,output,size);
        CHECK(!memcmp(source,output,size) && ((u8 *)output)[size]==0xA5);
        MI_CpuFillFast(output,0x12345678,size);
        for (u32 i=0;i<size/4;i++) CHECK(output[i]==0x12345678);
        CHECK(((u8 *)output)[size]==0xA5);
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
    output[0]=11; output[1]=22; output[2]=33;
    MIi_CpuSend32(source,output+1,20);
    CHECK(output[0]==11 && output[1]==source[4] && output[2]==33);
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
