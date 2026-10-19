// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef HEARTGOLD_NATIVE_RESET_H
#define HEARTGOLD_NATIVE_RESET_H
#include <nitro/types.h>
enum { HG_RESET_INIT,HG_RESET_OCCURRED,HG_RESET_PARAMETER,HG_RESET_REQUEST };
#ifdef HEARTGOLD_ABI32
u32 HGNativeResetControl(u32 command,u32 parameter)
    __attribute__((import_module("env"),import_name("HGNativeResetControl")));
#endif
#endif
