// SPDX-License-Identifier: GPL-3.0-or-later
#include <nitro/os/systemCall.h>
#include <nitro/os/system.h>
#include <nitro/mi/memory.h>
#include <nitro/math/crc.h>
#include <nitro/hw/native_graphics.h>
#include "assert.h"

void SVC_CpuSet(const void *source, void *destination, u32 control) {
    u32 count=control&0x1FFFFF, width=(control&(1u<<26)) ? 4 : 2;
    if (!count) return;
    if (control&(1u<<24)) {
        if (width==4) {
            u32 value; MI_CpuCopy32(source,&value,4);
            MI_CpuFill32(destination,value,count*4);
        } else {
            u16 value; MI_CpuCopy16(source,&value,2);
            MI_CpuFill16(destination,value,count*2);
        }
    } else if (width==4) MI_CpuCopy32(source,destination,count*4);
    else MI_CpuCopy16(source,destination,count*2);
}

u16 SVC_GetCRC16(u16 seed, const void *data, u32 size) {
    GF_ASSERT((uaddr)data%2==0 && size%2==0);
    MATHCRC16Context context=seed;
    if (!size) return context;
    MATHCRC16Table table;
    MATHi_CRC16InitTableRev(&table,MATH_CRC16_STANDARD_POLY);
    uaddr address=(uaddr)data;
    while (size) {
        u32 count=size;
        const void *span=HGNativeMemorySpan(address,&count);
        GF_ASSERT(span && count && count%2==0);
        MATHi_CRC16UpdateRev(&table,&context,span,count);
        address+=count; size-=count;
    }
    return context;
}

void SVC_WaitByLoop(s32 count) {
    GF_ASSERT(count>0);
    // shortcut: cached ARM9 minimum delay; calibrate BIOS overhead/cache-off timing on real hardware.
    u32 loops=(u32)count;
    while (loops>0x40000000) {
        OS_SpinWait(0xFFFFFFFF); loops-=0x40000000;
    }
    OS_SpinWait(loops*4-1);
}
