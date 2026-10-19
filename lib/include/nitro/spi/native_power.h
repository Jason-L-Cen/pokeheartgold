// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef NITRO_SPI_NATIVE_POWER_H
#define NITRO_SPI_NATIVE_POWER_H
#include <nitro/types.h>
enum { HG_POWER_INIT,HG_POWER_UTILITY,HG_POWER_READ,HG_POWER_WRITE,HG_POWER_SET_LED,HG_POWER_GET_LED,HG_POWER_DRAIN };
#ifdef HEARTGOLD_ABI32
u32 HGNativePowerRequest(u32 command,u32 first,u32 second,u32 callback)
    __attribute__((import_module("env"),import_name("HGNativePowerRequest")));
#endif
#endif
