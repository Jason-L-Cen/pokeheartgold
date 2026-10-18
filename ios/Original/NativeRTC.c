// SPDX-License-Identifier: GPL-3.0-or-later
#include <nitro/rtc/ARM9/api.h>
#include "NativeEvents.h"
#include <pthread.h>
#include <time.h>

static pthread_once_t initialized = PTHREAD_ONCE_INIT;
static pthread_mutex_t rtcMutex = PTHREAD_MUTEX_INITIALIZER;
static BOOL busy;
static struct {
    RTCDate *date, capturedDate;
    RTCTime *time, capturedTime;
    RTCCallback callback;
    void *argument;
    RTCResult result;
} request;

void RTC_Init(void) { pthread_once(&initialized, tzset); }

static RTCResult capture(RTCDate *date, RTCTime *clock) {
    time_t now = time(NULL);
    struct tm local;
    if (now == (time_t)-1 || !localtime_r(&now, &local)) return RTC_RESULT_FATAL_ERROR;
    if (local.tm_year < 100 || local.tm_year >= 200 || local.tm_sec > 59) return RTC_RESULT_ILLEGAL_STATUS;
    *date = (RTCDate){local.tm_year - 100, local.tm_mon + 1, local.tm_mday, local.tm_wday};
    *clock = (RTCTime){local.tm_hour, local.tm_min, local.tm_sec};
    return RTC_RESULT_SUCCESS;
}

static void completed(void *unused) {
    (void)unused;
    pthread_mutex_lock(&rtcMutex);
    if (request.result == RTC_RESULT_SUCCESS) {
        if (request.date) *request.date = request.capturedDate;
        if (request.time) *request.time = request.capturedTime;
    }
    RTCCallback callback = request.callback;
    void *argument = request.argument;
    RTCResult result = request.result;
    busy = FALSE;
    pthread_mutex_unlock(&rtcMutex);
    if (callback) callback(result, argument);
}

static RTCResult readClock(RTCDate *date, RTCTime *clock, RTCCallback callback, void *argument, BOOL async) {
    RTC_Init();
    pthread_mutex_lock(&rtcMutex);
    if (busy) { pthread_mutex_unlock(&rtcMutex); return RTC_RESULT_BUSY; }
    RTCDate capturedDate;
    RTCTime capturedTime;
    RTCResult result = capture(&capturedDate, &capturedTime);
    if (!async) {
        if (result == RTC_RESULT_SUCCESS) {
            if (date) *date = capturedDate;
            if (clock) *clock = capturedTime;
        }
        pthread_mutex_unlock(&rtcMutex);
        return result;
    }
    request.date = date; request.time = clock;
    if (result == RTC_RESULT_SUCCESS) { request.capturedDate = capturedDate; request.capturedTime = capturedTime; }
    request.callback = callback; request.argument = argument; request.result = result; busy = TRUE;
    pthread_mutex_unlock(&rtcMutex);
    if (HGNativeQueueIrqCallback(OS_IE_SPFIFO_RECV, completed, NULL)) return RTC_RESULT_SUCCESS;
    pthread_mutex_lock(&rtcMutex); busy = FALSE; pthread_mutex_unlock(&rtcMutex);
    return RTC_RESULT_SEND_ERROR;
}

RTCResult RTC_GetDate(RTCDate *date) {
    return date ? readClock(date, NULL, NULL, NULL, FALSE) : RTC_RESULT_ILLEGAL_PARAMETER;
}
RTCResult RTC_GetTime(RTCTime *clock) {
    return clock ? readClock(NULL, clock, NULL, NULL, FALSE) : RTC_RESULT_ILLEGAL_PARAMETER;
}
RTCResult RTC_GetDateTime(RTCDate *date, RTCTime *clock) {
    return date && clock ? readClock(date, clock, NULL, NULL, FALSE) : RTC_RESULT_ILLEGAL_PARAMETER;
}
RTCResult RTC_GetDateAsync(RTCDate *date, RTCCallback callback, void *argument) {
    return date ? readClock(date, NULL, callback, argument, TRUE) : RTC_RESULT_ILLEGAL_PARAMETER;
}
RTCResult RTC_GetTimeAsync(RTCTime *clock, RTCCallback callback, void *argument) {
    return clock ? readClock(NULL, clock, callback, argument, TRUE) : RTC_RESULT_ILLEGAL_PARAMETER;
}
RTCResult RTC_GetDateTimeAsync(RTCDate *date, RTCTime *clock, RTCCallback callback, void *argument) {
    return date && clock ? readClock(date, clock, callback, argument, TRUE) : RTC_RESULT_ILLEGAL_PARAMETER;
}
