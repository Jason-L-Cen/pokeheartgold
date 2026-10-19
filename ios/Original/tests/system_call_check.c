// SPDX-License-Identifier: GPL-3.0-or-later
#include <nitro/os/systemCall.h>
#include <string.h>
#define CHECK(v) do { if (!(v)) return __LINE__; } while (0)

u32 CheckOriginalSystemCalls(void) {
    _Alignas(4) u8 source[96], output[96];
    for (u32 i=0;i<sizeof(source);i++) source[i]=(u8)i;
    for (u32 width=2;width<=4;width+=2) {
        u32 mode=width==4 ? 1u<<26 : 0;
        for (u32 count=0;count<=20;count++) {
            memset(output,0xA5,sizeof(output));
            SVC_CpuSet(source,output+4,mode|count|0xE8E00000);
            CHECK(!memcmp(source,output+4,count*width));
            CHECK(output[3]==0xA5 && output[4+count*width]==0xA5);
            SVC_CpuSet(source,output+4,mode|(1u<<24)|count);
            for (u32 i=0;i<count*width;i++) CHECK(output[4+i]==source[i%width]);
            CHECK(output[3]==0xA5 && output[4+count*width]==0xA5);
        }
        memcpy(output,source,sizeof(source));
        SVC_CpuSet(output,output+width,mode|12);
        for (u32 i=0;i<13*width;i++) CHECK(output[i]==source[i%width]);
        CHECK(output[13*width]==source[13*width]);
        memcpy(output,source,sizeof(source));
        SVC_CpuSet(output+width,output,mode|12);
        CHECK(!memcmp(output,source+width,12*width));
        CHECK(output[12*width]==source[12*width]);
        memcpy(output,source,sizeof(source));
        SVC_CpuSet(output+width,output,mode|(1u<<24)|12);
        for (u32 i=0;i<12*width;i++) CHECK(output[i]==source[width+i%width]);
        SVC_CpuSet(output,output,mode|12);
        for (u32 i=0;i<12*width;i++) CHECK(output[i]==source[width+i%width]);
    }
    SVC_CpuSet(NULL,NULL,0);
    SVC_CpuSet(NULL,NULL,(1u<<26)|(1u<<24));
    _Alignas(2) const u8 digits[]="123456789";
    const u16 seeds[]={0,0xFFFF,0x1234,0x8001};
    // Independent bit-at-a-time polynomial 0xA001 reference values; includes the terminating zero.
    const u16 digitCrc[]={0xD17A,0xD60A,0x5BEF,0xD24A};
    const u16 rangeCrc[]={0x2799,0x08D9,0x3406,0x37D9};
    for (u32 i=0;i<4;i++) {
        CHECK(SVC_GetCRC16(seeds[i],digits,sizeof(digits))==digitCrc[i]);
        CHECK(SVC_GetCRC16(seeds[i],source,64)==rangeCrc[i]);
        CHECK(SVC_GetCRC16(seeds[i],NULL,0)==seeds[i]);
        u16 split=SVC_GetCRC16(seeds[i],source,22);
        CHECK(SVC_GetCRC16(split,source+22,42)==rangeCrc[i]);
    }
    for (u32 i=0;i<sizeof(source);i++) CHECK(source[i]==i);
    SVC_WaitByLoop(1); SVC_WaitByLoop(1024);
    return 0;
}
