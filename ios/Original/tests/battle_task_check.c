// SPDX-License-Identifier: GPL-3.0-or-later
#define SysTask_CreateOnMainQueue BattleTaskCheckCreate
#define SysTask_Destroy BattleTaskCheckDestroy
#include "../../../src/battle/battle_anim_system.c"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(value) do { if (!(value)) return __LINE__; } while (0)
static _Alignas(8) u8 tokens[5][64];
static u32 created,destroyed,lastPriority;
static SysTaskFunc lastCallback;
static void *lastData;
SysTask *BattleTaskCheckCreate(SysTaskFunc callback,void *data,u32 priority) {
    if (created>=5) abort(); lastCallback=callback; lastData=data; lastPriority=priority;
    return (SysTask *)tokens[created++];
}
void BattleTaskCheckDestroy(SysTask *task) {
    if ((uaddr)task<(uaddr)tokens || (uaddr)task-(uaddr)tokens>=sizeof(tokens)) abort(); destroyed++;
}
static void CheckTask(SysTask *task,void *data) { (void)task; (void)data; }

u32 CheckOriginalBattleTasks(void) {
    BattleAnimSystem system; memset(&system,0,sizeof(system));
    int payload=42; created=destroyed=0;
    SysTask *first=BattleAnimSystem_CreateTaskDefaultPriority(&system,CheckTask,&payload);
    CHECK(first==(SysTask *)tokens[0] && system.taskCount==1 && !system.soundTaskCount);
    CHECK(lastCallback==CheckTask && lastData==&payload && lastPriority==1100);
    // Original animation call sites use this returning function-pointer type.
    SysTask *(*factory)(BattleAnimSystem *,SysTaskFunc,void *)=BattleAnimSystem_CreateTaskDefaultPriority;
    SysTask *second=factory(&system,CheckTask,&payload);
    SysTask *third=BattleAnimSystem_CreateTask(&system,CheckTask,&payload,17);
    CHECK(second==(SysTask *)tokens[1] && third==(SysTask *)tokens[2] && system.taskCount==3 && lastPriority==17);
    SysTask *sound=BattleAnimSystem_CreateTaskOfKind(2,&system,CheckTask,&payload,23);
    CHECK(sound==(SysTask *)tokens[3] && system.soundTaskCount==1 && lastPriority==23);
    BattleAnimSystem_CreateSoundTask(&system,CheckTask,&payload,31);
    CHECK(created==5 && system.soundTaskCount==2 && lastPriority==31);
    BattleAnimSystem_DestroyTask(&system,first); BattleAnimSystem_DestroyTask(&system,second);
    BattleAnimSystem_DestroyTask(&system,third); BattleAnimSystem_DestroySoundTask(&system,sound);
    BattleAnimSystem_DestroySoundTask(&system,(SysTask *)tokens[4]);
    CHECK(destroyed==5 && !system.taskCount && !system.soundTaskCount);
    return 0;
}

#ifndef HEARTGOLD_ABI32
void HGNativeAssertFail(const char *condition,const char *file,int line) { fprintf(stderr,"%s:%d: %s\n",file,line,condition); abort(); }
int main(void) {
    u32 line=CheckOriginalBattleTasks();
    if (line) { fprintf(stderr,"Battle task check line %u\n",line); return 1; }
    puts("PASS: original animation task returns, typed factory calls, priorities, counters and destruction.");
    return 0;
}
#endif
