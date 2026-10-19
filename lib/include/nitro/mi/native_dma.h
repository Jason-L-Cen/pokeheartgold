// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef NITRO_MI_NATIVE_DMA_H
#define NITRO_MI_NATIVE_DMA_H
#include <nitro/types.h>

#define HG_NATIVE_DMA_MAX_SPANS 18
enum {
    HG_DMA_TRANSFER, HG_DMA_TRANSFER_WAIT, HG_DMA_FILL, HG_DMA_FILL_WAIT,
    HG_DMA_WAIT, HG_DMA_STOP, HG_DMA_CHECK_AUTO
};
typedef struct HGNativeDMARange {
    u32 address, offset, size;
} HGNativeDMARange;
#ifdef HEARTGOLD_ABI32
_Static_assert(sizeof(HGNativeDMARange)==12,"DMA range wire layout");
void HGNativeDmaRequest(u32 command, u32 channel, u32 source, u32 destination, u32 control,
                       const HGNativeDMARange *ranges, u32 count)
    __attribute__((import_module("env"), import_name("HGNativeDmaRequest")));
#endif
#endif
