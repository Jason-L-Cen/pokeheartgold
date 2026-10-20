// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef NITRO_SND_NATIVE_SOUND_H
#define NITRO_SND_NATIVE_SOUND_H
#include <nitro/types.h>
enum { HG_SOUND_INIT,HG_SOUND_SUBMIT,HG_SOUND_PUMP };
#ifdef HEARTGOLD_ABI32
u32 HGNativeSoundRequest(u32 command,u32 list)
    __attribute__((import_module("env"),import_name("HGNativeSoundRequest")));
#endif
#endif
