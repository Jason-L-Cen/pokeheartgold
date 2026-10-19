// SPDX-License-Identifier: GPL-3.0-or-later
#include "NativeAlarm.h"
#include "NativeEvents.h"
#include <nitro/hw/consts_shared.h>
#include <dispatch/dispatch.h>
#include <stdlib.h>
#include <time.h>

static dispatch_once_t once;
static dispatch_queue_t queue;
static dispatch_source_t timer;
static void *owner;
static BOOL armed;
static u64 deadline;
static u64 now(void) {
    struct timespec value; if (clock_gettime(CLOCK_MONOTONIC,&value)) abort();
    return (u64)value.tv_sec*1000000000ULL+value.tv_nsec;
}
static void schedule(u64 delay) {
    dispatch_source_set_timer(timer,dispatch_time(DISPATCH_TIME_NOW,(int64_t)delay),DISPATCH_TIME_FOREVER,0);
}
static void fired(void *unused) {
    (void)unused;
    if (!armed) return;
    u64 current=now();
    if (current<deadline) { schedule(deadline-current); return; }
    armed=FALSE; HGNativePostIrq(OS_IE_TIMER1);
}
static void initialize(void *unused) {
    (void)unused;
    queue=dispatch_queue_create("com.jasoncen.heartgold.alarm",DISPATCH_QUEUE_SERIAL);
    if (!queue) abort();
    timer=dispatch_source_create(DISPATCH_SOURCE_TYPE_TIMER,0,DISPATCH_TIMER_STRICT,queue);
    if (!timer) abort();
    dispatch_source_set_event_handler_f(timer,fired); dispatch_resume(timer);
}
typedef struct AlarmCommand { void *owner; u32 ticks; BOOL onlyOwner; } AlarmCommand;
static void change(void *argument) {
    AlarmCommand *command=argument;
    if (command->onlyOwner && owner!=command->owner) return;
    armed=FALSE; owner=command->onlyOwner ? NULL : command->owner;
    dispatch_source_set_timer(timer,DISPATCH_TIME_FOREVER,DISPATCH_TIME_FOREVER,0);
    if (command->onlyOwner) { OS_ResetRequestIrqMask(OS_IE_TIMER1); return; }
    if (command->ticks) {
        u64 delay=((u64)command->ticks*64000000000ULL+HW_SYSTEM_CLOCK-1)/HW_SYSTEM_CLOCK;
        deadline=now()+delay; armed=TRUE; schedule(delay);
    }
}
void HGNativeSetAlarmTimer(void *context,u32 ticks) {
    if (!context || ticks>0x10000) abort();
    dispatch_once_f(&once,NULL,initialize);
    AlarmCommand command={context,ticks,FALSE}; dispatch_sync_f(queue,&command,change);
}
void HGNativeCancelAlarmContext(void *context) {
    dispatch_once_f(&once,NULL,initialize);
    AlarmCommand command={context,0,TRUE}; dispatch_sync_f(queue,&command,change);
}
