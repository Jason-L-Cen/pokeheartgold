#ifndef POKEDIAMOND_ARM9_SYSCALL_H
#define POKEDIAMOND_ARM9_SYSCALL_H

#include "consts.h"
#include "MI_dma.h"
#include <nitro/os/systemCall.h>

#define SVC_CpuClear( data, destp, size, bit )                  \
do{                                                             \
    vu##bit tmp = (vu##bit )(data);                             \
    SVC_CpuSet((u8 *)&(tmp), (u8 *)(destp), (                   \
        MI_DMA_SRC_FIX        |                                 \
        MI_DMA_##bit##BIT_BUS | ((size)/((bit)/8) & 0x1fffff)));  \
} while(0)

#endif //POKEDIAMOND_ARM9_SYSCALL_H
