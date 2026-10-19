// SPDX-License-Identifier: GPL-3.0-or-later
#include <nitro.h>
#include <stddef.h>
#include <string.h>
#define CHECK(v) do { if (!(v)) return __LINE__; } while (0)
extern struct OSiAlarmQueue OSi_AlarmQueue;
static OSAlarm alarms[4];
static u32 ids[]={1,2,3,4},calls,order[16],failure,periodicLimit;
static void received(void *argument) {
    if (OS_GetProcMode()!=OS_PROCMODE_IRQ || OS_GetCpsrIrq()!=OS_INTRMODE_IRQ_DISABLE || calls>=16) { failure=1; return; }
    u32 id=*(u32 *)argument; order[calls++]=id;
    if (id==4 && calls>=periodicLimit) OS_CancelAlarm(&alarms[3]);
}
static void reset(void) {
    for (u32 i=0;i<4;i++) { OS_CancelAlarm(&alarms[i]); memset(&alarms[i],0,sizeof(alarms[i])); OS_CreateAlarm(&alarms[i]); }
    calls=failure=0; memset(order,0,sizeof(order));
}
u32 CheckOriginalAlarmQueue(void) {
    OS_InitTick(); CHECK(!OS_IsAlarmAvailable()); OS_InitAlarm(); CHECK(OS_IsAlarmAvailable());
    reset();
    OS_SetAlarm(&alarms[0],OS_MilliSecondsToTicks(3000),received,&ids[0]);
    OS_SetAlarm(&alarms[1],OS_MilliSecondsToTicks(4000),received,&ids[1]);
    OS_SetAlarm(&alarms[2],OS_MilliSecondsToTicks(5000),received,&ids[2]);
    CHECK(OSi_AlarmQueue.head==&alarms[0] && OSi_AlarmQueue.tail==&alarms[2]);
    OS_CancelAlarm(&alarms[1]); CHECK(alarms[0].next==&alarms[2] && alarms[2].prev==&alarms[0]);
    OS_CancelAlarm(&alarms[0]); CHECK(OSi_AlarmQueue.head==&alarms[2] && !alarms[2].prev);
    OS_CancelAlarm(&alarms[2]); CHECK(!OSi_AlarmQueue.head && !OSi_AlarmQueue.tail && !calls);
    OS_CancelAlarm(&alarms[2]); CHECK(OS_GetCpsrIrq()==OS_INTRMODE_IRQ_ENABLE);
    return 0;
}
void HGAlarmStart(u32 kind) {
    reset();
    if (kind==0) {
        OSTick fire=OS_GetTick()+OS_MilliSecondsToTicks(10);
        for (u32 i=0;i<2;i++) { alarms[i].handler=received; alarms[i].arg=&ids[i]; alarms[i].period=0; OSi_InsertAlarm(&alarms[i],fire); }
    } else if (kind==1) {
        OS_SetAlarm(&alarms[0],OS_MilliSecondsToTicks(20),received,&ids[0]);
        OS_CancelAlarm(&alarms[0]);
    } else if (kind==2) {
        periodicLimit=3;
        OS_SetPeriodicAlarm(&alarms[3],OS_GetTick()+OS_MilliSecondsToTicks(5),OS_MilliSecondsToTicks(5),received,&ids[3]);
    } else {
        OS_SetAlarm(&alarms[2],OS_MilliSecondsToTicks(10),received,&ids[2]);
    }
}
u32 HGAlarmRead(u32 field) { return field==0 ? calls : field==17 ? failure : field<=16 ? order[field-1] : OSi_AlarmQueue.head==NULL && OSi_AlarmQueue.tail==NULL; }
void HGAlarmCancelAll(void) { for (u32 i=0;i<4;i++) OS_CancelAlarm(&alarms[i]); }
