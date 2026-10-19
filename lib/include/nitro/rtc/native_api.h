#ifndef HEARTGOLD_NATIVE_RTC_ABI_H
#define HEARTGOLD_NATIVE_RTC_ABI_H
#include <nitro/types.h>
enum HGNativeRtcCommand { HG_RTC_INIT,HG_RTC_DATE,HG_RTC_TIME,HG_RTC_BOTH,HG_RTC_DATE_ASYNC,HG_RTC_TIME_ASYNC,HG_RTC_BOTH_ASYNC };
#ifdef HEARTGOLD_ABI32
extern u32 HGNativeRtcRequest(u32 command,u32 date,u32 time,u32 callback,u32 argument)
    __attribute__((import_module("env"),import_name("HGNativeRtcRequest")));
#endif
#endif
