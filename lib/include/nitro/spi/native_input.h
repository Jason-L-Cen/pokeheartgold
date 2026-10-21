// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef NITRO_SPI_NATIVE_INPUT_H
#define NITRO_SPI_NATIVE_INPUT_H
#include <nitro/types.h>
enum { HG_INPUT_INIT,HG_INPUT_PAD,HG_INPUT_SAMPLE,HG_INPUT_START,HG_INPUT_STOP,HG_INPUT_DRAIN };
// PAD first=1 marks a game key read; first=0 queries state without consuming a press (e.g. lid checks).
#ifdef HEARTGOLD_ABI32
u32 HGNativeInputRequest(u32 command,u32 first,u32 second,u32 callback)
    __attribute__((import_module("env"),import_name("HGNativeInputRequest")));
#endif
#endif
