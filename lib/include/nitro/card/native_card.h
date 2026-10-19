// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef NITRO_CARD_NATIVE_CARD_H
#define NITRO_CARD_NATIVE_CARD_H
#include <nitro/types.h>
#define HG_CARD_CANCEL 0x100u
#ifdef HEARTGOLD_ABI32
struct CARDiCommon;
BOOL CARDi_NativeRequest(struct CARDiCommon *,s32 request,s32 retry,u32 mode);
u32 HGNativeCardRequest(u32 request,u32 command,u32 mode,u32 retry,u32 callback,u32 argument)
    __attribute__((import_module("env"),import_name("HGNativeCardRequest")));
#endif
#endif
