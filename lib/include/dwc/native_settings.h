// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef HEARTGOLD_NATIVE_DWC_SETTINGS_H
#define HEARTGOLD_NATIVE_DWC_SETTINGS_H
#include <nitro/types.h>
#define HG_DWC_SETTINGS_BASE 0x1000u
#define HG_DWC_SETTINGS_SIZE 1024u
#ifdef HEARTGOLD_ABI32
extern u32 HGNativeDwcSettings(u32 offset, void *buffer, u32 size, BOOL write)
    __attribute__((import_module("env"),import_name("HGNativeDwcSettings")));
extern u64 HGNativeDwcIdentity(void)
    __attribute__((import_module("env"),import_name("HGNativeDwcIdentity")));
#endif
#endif
