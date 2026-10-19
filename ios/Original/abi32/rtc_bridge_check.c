// SPDX-License-Identifier: GPL-3.0-or-later
#include "gf_rtc.h"
#include <nitro/os/system.h>
#include <nitro/rtc/native_api.h>
#include <string.h>
#define CHECK(v) do { if (!(v)) return __LINE__; } while (0)
static struct { u32 before; RTCDate date; RTCTime time; u32 after; } slot;
static u32 calls,lastResult,seenArgument,badMode,repeat;
static void received(RTCResult result,void *argument) {
    if (OS_GetProcMode()!=OS_PROCMODE_IRQ || OS_GetCpsrIrq()!=OS_INTRMODE_IRQ_DISABLE) badMode=1;
    calls++; lastResult=result; seenArgument=(u32)(uaddr)argument;
    if (repeat) {
        repeat--;
        if (RTC_GetTimeAsync(&slot.time,received,argument)!=RTC_RESULT_SUCCESS) badMode=1;
    }
}
static u32 wrongCallback(void) { return 1; }
static void reset(void) {
    memset(&slot,0xA5,sizeof(slot)); slot.before=0x12345678; slot.after=0x87654321;
    calls=badMode=repeat=seenArgument=0; lastResult=~0u;
}
u32 HGRTCAddress(u32 which) {
    return which==0 ? (u32)(uaddr)&slot.date : which==1 ? (u32)(uaddr)&slot.time :
        which==2 ? (u32)(uaddr)received : (u32)(uaddr)wrongCallback;
}
u32 HGRTCRead(u32 field) {
    if (field<4) return ((u32 *)&slot.date)[field];
    if (field<7) return ((u32 *)&slot.time)[field-4];
    switch (field) {
    case 7: return slot.before==0x12345678 && slot.after==0x87654321;
    case 8: return calls;
    case 9: return lastResult;
    case 10: return seenArgument;
    default: return badMode;
    }
}
u32 HGRTCSync(u32 which) {
    return which==HG_RTC_DATE ? RTC_GetDate(&slot.date) : which==HG_RTC_TIME ? RTC_GetTime(&slot.time) : RTC_GetDateTime(&slot.date,&slot.time);
}
u32 HGRTCStart(u32 which,u32 withCallback,u32 repeats) {
    reset(); repeat=repeats;
    RTCCallback cb=withCallback ? received : NULL; void *arg=(void *)(uaddr)0xF00DBAAD;
    return which==HG_RTC_DATE_ASYNC ? RTC_GetDateAsync(&slot.date,cb,arg) :
        which==HG_RTC_TIME_ASYNC ? RTC_GetTimeAsync(&slot.time,cb,arg) : RTC_GetDateTimeAsync(&slot.date,&slot.time,cb,arg);
}
u32 CheckRTCBeforeIRQ(void) {
    RTC_Init(); reset();
    CHECK(RTC_GetDate(NULL)==RTC_RESULT_ILLEGAL_PARAMETER && RTC_GetTime(NULL)==RTC_RESULT_ILLEGAL_PARAMETER);
    CHECK(RTC_GetDateTime(&slot.date,NULL)==RTC_RESULT_ILLEGAL_PARAMETER);
    CHECK(HGRTCStart(HG_RTC_BOTH_ASYNC,1,0)==RTC_RESULT_SEND_ERROR && !calls);
    CHECK(slot.date.year==0xA5A5A5A5 && slot.time.hour==0xA5A5A5A5);
    CHECK(RTC_GetDateTime(&slot.date,&slot.time)==RTC_RESULT_SUCCESS);
    CHECK(slot.date.year<100 && slot.date.month>=1 && slot.date.month<=12 && slot.date.day>=1 && slot.date.day<=31);
    CHECK(slot.time.hour<24 && slot.time.minute<60 && slot.time.second<60 && HGRTCRead(7));
    return 0;
}
u32 CheckOriginalGameRTC(void) {
    GF_InitRTCWork();
    RTCDate date; RTCTime time;
    GF_RTC_CopyDateTime(&date,&time);
    CHECK(date.year<100 && time.hour<24 && time.minute<60 && time.second<60);
    const TIMEOFDAY expected[]={4,4,4,4,0,0,0,0,0,0,1,1,1,1,1,1,1,2,2,2,3,3,3,3};
    for (int hour=0;hour<24;hour++) {
        GF_RTC_SetAndFreezeTime(hour,37);
        CHECK(GF_RTC_GetTimeOfDay()==expected[hour]);
        CHECK(IsNighttime()==(hour<4 || hour>=20));
        CHECK(GF_RTC_GetTimeOfDayWildParam()==(hour<4 || hour>=20 ? TIMEOFDAY_WILD_NITE : hour<10 ? TIMEOFDAY_WILD_MORN : TIMEOFDAY_WILD_DAY));
    }
    GF_RTC_UnfreezeTime(); RTCTime restored; GF_RTC_CopyTime(&restored);
    CHECK(!memcmp(&restored,&time,sizeof(time)));
    CHECK(GF_RTC_DateTimeToSec()==RTC_ConvertDateTimeToSecond(&date,&time));
    return 0;
}
void HGRTCGameFrames(u32 count) { while (count--) GF_RTC_UpdateOnFrame(); }
