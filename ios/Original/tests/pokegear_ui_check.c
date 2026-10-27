// SPDX-License-Identifier: GPL-3.0-or-later
#include "native_pokegear_ui.h"
#include <stddef.h>
#include <string.h>
#define CHECK(value) do{if(!(value))return __LINE__;}while(0)
#ifdef HG_NATIVE_POKEGEAR_UI_STATE_CHECK
static void CheckPokegearResident(void);
#endif
uint32_t CheckNativePokegearUI(void) {
    CHECK(sizeof(HGNativePokegearUIState)==48 && offsetof(HGNativePokegearUIState,ownerGeneration)==4
        && offsetof(HGNativePokegearUIState,ready)==12 && offsetof(HGNativePokegearUIState,scriptedLaunch)==44);
    for(uint32_t cards=0;cards<4;++cards) {
        CHECK(HGNativePokegearUICardAvailable(cards,0) && HGNativePokegearUICardAvailable(cards,3));
        CHECK(HGNativePokegearUICardAvailable(cards,1)==!!(cards&2) && HGNativePokegearUICardAvailable(cards,2)==!!(cards&1));
    }
    HGNativePokegearUIState state={.active=1,.ownerGeneration=7,.revision=9,.ready=1,.viewActive=1,
        .ownerKind=1,.parentPhase=3,.viewKind=0,.phase=1,.app=0,.registeredCards=0},before=state;
    uint32_t keys=123;
    CHECK(HGNativePokegearUIResolveKey(&state,7,9,1,&keys) && keys==1);
    CHECK(!HGNativePokegearUIResolveKey(&state,7,8,1,&keys) && !HGNativePokegearUIResolveKey(&state,0,9,1,&keys));
    CHECK(!HGNativePokegearUIResolveKey(&state,7,9,3,&keys) && !HGNativePokegearUIResolveKey(&state,7,9,8,&keys));
    CHECK(!HGNativePokegearUITouchStartAllowed(&state,7,9,80,175) && !HGNativePokegearUITouchStartAllowed(&state,7,9,120,175));
    CHECK(HGNativePokegearUITouchStartAllowed(&state,7,9,24,40) && !HGNativePokegearUITouchStartAllowed(&state,7,9,256,40));
    CHECK(HGNativePokegearUITouchHeldAllowed(&state,7,9,80,175) && !HGNativePokegearUITouchHeldAllowed(&state,7,8,80,175));
    CHECK(!memcmp(&state,&before,sizeof(state)));
    state.viewKind=HG_POKEGEAR_EASY_CHAT;state.ready=state.viewActive=0;state.parentPhase=9;
    CHECK(!HGNativePokegearUIInputAllowed(&state,7,9));
    state=before;state.ownerKind=2;state.viewKind=4;state.scriptedLaunch=0;CHECK(HGNativePokegearUIInputAllowed(&state,7,9));
    state.ownerKind=3;CHECK(!HGNativePokegearUIInputAllowed(&state,7,9));state.scriptedLaunch=1;CHECK(HGNativePokegearUIInputAllowed(&state,7,9));
#ifdef HG_NATIVE_POKEGEAR_UI_STATE_CHECK
    CheckPokegearResident();
#endif
    return 0;
}
#ifdef HG_NATIVE_POKEGEAR_UI_STATE_CHECK
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
typedef uint16_t u16;typedef uint32_t u32;typedef int BOOL;
#define TRUE 1
#define FALSE 0
#define GF_ASSERT assert
#include "actual_sys_task.h"
#include "actual_sys_task.inc"
static SysTaskQueue *testQueue;
static SysTask *SysTask_CreateOnVWaitQueue(SysTaskFunc func,void *data,u32 priority){return SysTaskQueue_InsertTask(testQueue,func,data,priority);}
static void SysTask_Destroy(SysTask *task){assert(SysTask_Unlink(task));}
#include "actual_resident.inc"
static uint32_t uploads,persistentRuns;
static void persistent(SysTask *task,void *data){++persistentRuns;}
static void upload(SysTask *task,void *data){assert(!sNativePokegearUploaded);++uploads;SysTask_Destroy(task);}
static void CheckPokegearResident(void) {
    void *arena=calloc(1,SysTaskQueue_GetArenaSize(32));assert(arena);testQueue=SysTaskQueue_PlacementNew(32,arena);
    int parent,other,view;HGNativePokegearUIState facts={.ready=1,.parentPhase=7,.phase=1,.app=2,.registeredCards=1};
    NativePokegearUIBegin(&parent,1);NativePokegearUIParentState(&parent,&facts);NativePokegearUIBeginView(&parent,&view,2);
    SysTask *alive=SysTask_CreateOnVWaitQueue(persistent,NULL,5);assert(alive);
    assert(SysTask_CreateOnVWaitQueue(upload,NULL,128));assert(SysTask_CreateOnVWaitQueue(upload,NULL,128));
    NativePokegearUIPublish(&parent,&view,&facts);assert(!HGNativePokegearUIGetState()->ready);
    SysTaskQueue_RunTasks(testQueue);assert(uploads==2 && persistentRuns==1 && testQueue->activeCount==1);
    NativePokegearUIPublish(&parent,&view,&facts);HGNativePokegearUIState shown=*HGNativePokegearUIGetState();assert(shown.ready);
    for(uint32_t i=0;i<1000;++i)NativePokegearUIPublish(&parent,&view,&facts);
    assert(!memcmp(&shown,HGNativePokegearUIGetState(),sizeof(shown)));
    facts.phase=9;NativePokegearUIPublish(&parent,&view,&facts);assert(!HGNativePokegearUIGetState()->ready);
    NativePokegearUIEndView(&parent,&view);assert(testQueue->activeCount==1 && !HGNativePokegearUIGetState()->viewActive);
    facts.parentPhase=9;facts.app=4;NativePokegearUIParentState(&parent,&facts);
    assert(HGNativePokegearUIGetState()->active && HGNativePokegearUIGetState()->viewKind==5 && !HGNativePokegearUIGetState()->ready);
    NativePokegearUIBeginView(&parent,&view,2);SysTaskQueue_RunTasks(testQueue);assert(!HGNativePokegearUIGetState()->ready);
    facts.parentPhase=7;facts.phase=1;facts.app=2;NativePokegearUIParentState(&parent,&facts);NativePokegearUIPublish(&parent,&view,&facts);SysTaskQueue_RunTasks(testQueue);NativePokegearUIPublish(&parent,&view,&facts);
    assert(HGNativePokegearUIGetState()->ready && !HGNativePokegearUIInputAllowed(HGNativePokegearUIGetState(),shown.ownerGeneration,shown.revision));
    NativePokegearUIRetire(&other);assert(HGNativePokegearUIGetState()->active);NativePokegearUIRetire(&parent);
    sNativePokegearUIState.ownerGeneration=sNativePokegearUIState.revision=UINT32_MAX;
    NativePokegearUIBegin(&other,2);assert(HGNativePokegearUIGetState()->ownerGeneration==1 && HGNativePokegearUIGetState()->revision==1);
    NativePokegearUIBeginView(&other,&view,4);facts=(HGNativePokegearUIState){.ready=1,.phase=1,.scriptedLaunch=0};
    NativePokegearUIPublish(&other,&view,&facts);SysTaskQueue_RunTasks(testQueue);NativePokegearUIPublish(&other,&view,&facts);assert(HGNativePokegearUIGetState()->ready);
    NativePokegearUIEndView(&other,&view);NativePokegearUIRetire(&other);SysTask_Destroy(alive);assert(!testQueue->activeCount);free(arena);
}
#endif
#ifdef HG_NATIVE_POKEGEAR_UI_CHECK_MAIN
#include <stdio.h>
int main(void){uint32_t line=CheckNativePokegearUI();if(line){fprintf(stderr,"Pokegear UI check line %u\n",line);return 1;}puts("PASS: Pokegear48 original card gates, raw input leases and resident lifecycle/upload fence.");return 0;}
#endif
