// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef HEARTGOLD_NATIVE_MIC_ABI_H
#define HEARTGOLD_NATIVE_MIC_ABI_H
#include <nitro/types.h>
enum HGNativeMicCommand { HG_MIC_INIT,HG_MIC_START,HG_MIC_STOP,HG_MIC_LAST };
#ifdef HEARTGOLD_ABI32
extern u32 HGNativeMicRequest(u32 command,u32 buffer,u32 size,u32 rate,u32 flags,u32 callback)
    __attribute__((import_module("env"),import_name("HGNativeMicRequest")));
#endif
#endif
