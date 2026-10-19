#ifndef HEARTGOLD_NATIVE_CLOCK_ABI_H
#define HEARTGOLD_NATIVE_CLOCK_ABI_H
#include <nitro/types.h>
enum HGNativeTickCommand { HG_TICK_INIT,HG_TICK_GET,HG_TICK_SET };
#ifdef HEARTGOLD_ABI32
extern u64 HGNativeTickControl(u32 command,u64 value) __attribute__((import_module("env"),import_name("HGNativeTickControl")));
extern void HGNativeEntropy(u32 output) __attribute__((import_module("env"),import_name("HGNativeEntropy")));
#endif
#endif
