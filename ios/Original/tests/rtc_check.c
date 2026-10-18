// SPDX-License-Identifier: GPL-3.0-or-later
#include "gf_rtc.h"
#include "../NativeEvents.h"
#include <nitro/rtc/ARM9/convert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define CHECK(value) do { if (!(value)) { fprintf(stderr,"RTC check line %d: %s\n",__LINE__,#value); abort(); } } while (0)
static pthread_t owner;
static int order[8], count, plainCount;
static void plain(void) { CHECK(OS_GetProcMode()==OS_PROCMODE_IRQ); plainCount++; }
static void queued(void *argument) {
    CHECK(pthread_equal(owner,pthread_self()) && OS_GetProcMode()==OS_PROCMODE_IRQ);
    order[count++]=*(int *)argument;
}
typedef struct Reply { int count; BOOL repeat; RTCTime time; } Reply;
static void received(RTCResult result, void *argument) {
    Reply *reply=argument;
    CHECK(result==RTC_RESULT_SUCCESS && pthread_equal(owner,pthread_self()) && OS_GetProcMode()==OS_PROCMODE_IRQ);
    reply->count++;
    if (reply->repeat && reply->count==1) CHECK(RTC_GetTimeAsync(&reply->time,received,reply)==RTC_RESULT_SUCCESS);
}
typedef struct ThreadRead { Reply reply; RTCResult result; } ThreadRead;
static void *readOnWorker(void *argument) {
    ThreadRead *read=argument;
    read->result=RTC_GetTimeAsync(&read->reply.time,received,&read->reply);
    return NULL;
}

void CheckNativeRTCBeforeIRQ(void) {
    RTCDate date; RTCTime clock;
    CHECK(RTC_GetDateTimeAsync(&date,&clock,NULL,NULL)==RTC_RESULT_SEND_ERROR);
    CHECK(RTC_GetDateTime(&date,&clock)==RTC_RESULT_SUCCESS);
    CHECK(!HGNativeQueueIrqCallback(OS_IE_SPFIFO_RECV,queued,NULL));
}

void CheckNativeRTC(void) {
    owner=pthread_self();
    OS_InitIrqTable(); OS_EnableIrqMask(OS_IE_SPFIFO_RECV); OS_EnableIrq();
    OS_SetIrqFunction(OS_IE_SPFIFO_RECV,plain);
    int first=11, second=22;
    CHECK(!HGNativeQueueIrqCallback(0,queued,&first));
    CHECK(!HGNativeQueueIrqCallback(OS_IE_SPFIFO_RECV|OS_IE_VBLANK,queued,&first));
    CHECK(!HGNativeQueueIrqCallback(OS_IE_SPFIFO_RECV,NULL,&first));
    OS_DisableIrq();
    CHECK(HGNativeQueueIrqCallback(OS_IE_SPFIFO_RECV,queued,&first));
    CHECK(HGNativeQueueIrqCallback(OS_IE_SPFIFO_RECV,queued,&second));
    HGNativePostIrq(OS_IE_SPFIFO_RECV);
    CHECK(!count && !plainCount && (OS_GetRequestIrqMask()&OS_IE_SPFIFO_RECV));
    CHECK(OS_ResetRequestIrqMask(OS_IE_SPFIFO_RECV)&OS_IE_SPFIFO_RECV);
    CHECK(OS_GetRequestIrqMask()&OS_IE_SPFIFO_RECV);
    HGNativePostIrq(OS_IE_SPFIFO_RECV);
    OS_EnableIrq();
    CHECK(count==2 && order[0]==11 && order[1]==22 && plainCount==1);
    CHECK(OS_GetIrqFunction(OS_IE_SPFIFO_RECV)==plain && !(OS_GetRequestIrqMask()&OS_IE_SPFIFO_RECV));
    OSIntrMode prior=OS_DisableInterrupts();
    CHECK(HGNativeQueueIrqCallback(OS_IE_SPFIFO_RECV,queued,&first) && count==2);
    OS_RestoreInterrupts(prior); CHECK(count==3);

    RTCDate date; RTCTime clock;
    time_t before=time(NULL);
    CHECK(RTC_GetDateTime(&date,&clock)==RTC_RESULT_SUCCESS);
    time_t after=time(NULL);
    BOOL matches=FALSE;
    // Check both sides of a repeated DST hour; the DS date/time has no DST flag.
    for (int dst=0;dst<=1;dst++) {
        struct tm local={.tm_year=(int)date.year+100,.tm_mon=(int)date.month-1,.tm_mday=(int)date.day,
            .tm_hour=(int)clock.hour,.tm_min=(int)clock.minute,.tm_sec=(int)clock.second,.tm_isdst=dst};
        time_t captured=mktime(&local), low=before<after ? before : after, high=before>after ? before : after;
        if (captured>=low && captured<=high && date.week==local.tm_wday && local.tm_year==(int)date.year+100 &&
            local.tm_mon==(int)date.month-1 && local.tm_mday==date.day && local.tm_hour==clock.hour &&
            local.tm_min==clock.minute && local.tm_sec==clock.second) matches=TRUE;
    }
    CHECK(matches);
    CHECK(RTC_GetDate(NULL)==RTC_RESULT_ILLEGAL_PARAMETER && RTC_GetTime(NULL)==RTC_RESULT_ILLEGAL_PARAMETER);
    CHECK(RTC_GetDateTime(&date,NULL)==RTC_RESULT_ILLEGAL_PARAMETER);
    CHECK(RTC_GetDateTimeAsync(NULL,&clock,NULL,NULL)==RTC_RESULT_ILLEGAL_PARAMETER);
    CHECK(RTC_GetDate(&date)==RTC_RESULT_SUCCESS && RTC_GetTime(&clock)==RTC_RESULT_SUCCESS);

    Reply reply={0};
    OS_DisableIrq(); memset(&date,0xA5,sizeof(date)); memset(&clock,0xA5,sizeof(clock));
    CHECK(RTC_GetDateTimeAsync(&date,&clock,received,&reply)==RTC_RESULT_SUCCESS);
    CHECK(!reply.count && date.year==0xA5A5A5A5 && clock.hour==0xA5A5A5A5);
    CHECK(RTC_GetTime(&reply.time)==RTC_RESULT_BUSY && RTC_GetTimeAsync(&reply.time,NULL,NULL)==RTC_RESULT_BUSY);
    RTC_Init(); OS_EnableIrq();
    CHECK(reply.count==1 && date.year<100 && clock.hour<24 && plainCount==1);
    OS_DisableIrqMask(OS_IE_SPFIFO_RECV);
    CHECK(RTC_GetDateAsync(&date,received,&reply)==RTC_RESULT_SUCCESS && reply.count==1);
    OS_EnableIrqMask(OS_IE_SPFIFO_RECV); CHECK(reply.count==2);
    reply=(Reply){.repeat=TRUE};
    CHECK(RTC_GetTimeAsync(&reply.time,received,&reply)==RTC_RESULT_SUCCESS && reply.count==2);
    CHECK(RTC_GetDateAsync(&date,NULL,NULL)==RTC_RESULT_SUCCESS && date.month>=1 && date.month<=12);
    ThreadRead worker={0}; pthread_t thread;
    CHECK(!pthread_create(&thread,NULL,readOnWorker,&worker) && !pthread_join(thread,NULL));
    CHECK(worker.result==RTC_RESULT_SUCCESS && !worker.reply.count);
    OS_WaitIrq(TRUE,OS_IE_SPFIFO_RECV);
    CHECK(worker.reply.count==1);

    GF_InitRTCWork(); GF_RTC_CopyDateTime(&date,&clock);
    CHECK(date.year<100 && clock.hour<24);
    const TIMEOFDAY expected[]={4,4,4,4,0,0,0,0,0,0,1,1,1,1,1,1,1,2,2,2,3,3,3,3};
    for (int hour=0;hour<24;hour++) {
        GF_RTC_SetAndFreezeTime(hour,37);
        CHECK(GF_RTC_GetTimeOfDay()==expected[hour] && GF_RTC_TimeToSec()==hour*3600+37*60);
        CHECK(IsNighttime()==(hour<4 || hour>=20));
        CHECK(GF_RTC_GetTimeOfDayWildParam()==(hour<4 || hour>=20 ? TIMEOFDAY_WILD_NITE : hour<10 ? TIMEOFDAY_WILD_MORN : TIMEOFDAY_WILD_DAY));
    }
    GF_RTC_UnfreezeTime(); RTCTime restored; GF_RTC_CopyTime(&restored);
    CHECK(!memcmp(&restored,&clock,sizeof(clock)));
    OS_DisableIrqMask(OS_IE_SPFIFO_RECV);
    for (int frame=0;frame<10;frame++) GF_RTC_UpdateOnFrame();
    CHECK(RTC_GetTime(&restored)==RTC_RESULT_SUCCESS);
    GF_RTC_UpdateOnFrame();
    CHECK(RTC_GetTime(&restored)==RTC_RESULT_BUSY);
    for (int frame=0;frame<22;frame++) GF_RTC_UpdateOnFrame();
    OS_EnableIrqMask(OS_IE_SPFIFO_RECV);
    GF_RTC_CopyDateTime(&date,&clock);
    CHECK(GF_RTC_DateTimeToSec()==RTC_ConvertDateTimeToSecond(&date,&clock));
    RTCDate epoch={0,1,1,RTC_WEEK_SATURDAY}, leap={0,2,29,RTC_WEEK_TUESDAY};
    RTCTime midnight={0};
    CHECK(RTC_ConvertDateTimeToSecond(&epoch,&midnight)==0);
    CHECK(RTC_ConvertDateTimeToSecond(&leap,&midnight)==59*86400);
    CHECK(GF_RTC_GetDayOfYear(&leap)==60 && GF_RTC_TimeDelta(3155759994LL,10)==15);
    RTC_ConvertSecondToDateTime(&date,&clock,3155759999LL);
    CHECK(date.year==99 && date.month==12 && date.day==31 && clock.hour==23 && clock.minute==59 && clock.second==59);
    CHECK(RTC_ConvertDateTimeToSecond(&date,&clock)==3155759999LL);
    puts("PASS: device local RTC, IRQ masks/owner/FIFO/reentry, original GF RTC refresh, all 24 day/night/encounter periods and calendar boundaries.");
}
