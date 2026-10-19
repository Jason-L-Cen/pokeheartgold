// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef NITRO_OS_NATIVE_THREAD_H
#define NITRO_OS_NATIVE_THREAD_H
#include <nitro/types.h>
enum { HG_THREAD_INIT,HG_THREAD_REGISTER,HG_THREAD_RESET,HG_THREAD_SWITCH,
       HG_THREAD_LOAD,HG_THREAD_DROP,HG_THREAD_HALT,HG_THREAD_ROOT_HIGH };
#ifdef HEARTGOLD_ABI32
u32 HGNativeThreadRequest(u32 command,u32 thread,u32 first,u32 second)
    __attribute__((import_module("env"),import_name("HGNativeThreadRequest")));
#endif
#endif
