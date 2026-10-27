// SPDX-License-Identifier: GPL-3.0-or-later
#include "native_field_menu_ui.h"
#include <stddef.h>
#include <string.h>
#define CHECK(value) do{if(!(value))return __LINE__;}while(0)
#ifdef HG_NATIVE_FIELD_MENU_UI_STATE_CHECK
static void CheckFieldMenuResident(void);
#endif
uint32_t CheckNativeFieldMenuUI(void) {
    CHECK(sizeof(HGNativeFieldMenuUIState)==36 && offsetof(HGNativeFieldMenuUIState,ready)==12
        && offsetof(HGNativeFieldMenuUIState,phase)==16 && offsetof(HGNativeFieldMenuUIState,inputMode)==32);
    HGNativeFieldMenuUIState menu={1,7,9,1,3,1,1,0,0},before=menu;uint32_t keys=123;
    const uint32_t allowed[]={1,2,16,32,64,128,1024},rejected[]={0,3,4,8,256,512,2048,UINT32_MAX};
    for(size_t i=0;i<sizeof(allowed)/sizeof(allowed[0]);++i)CHECK(HGNativeFieldMenuUIResolveKey(&menu,7,9,allowed[i],&keys)&&keys==allowed[i]);
    keys=123;for(size_t i=0;i<sizeof(rejected)/sizeof(rejected[0]);++i)CHECK(!HGNativeFieldMenuUIResolveKey(&menu,7,9,rejected[i],&keys)&&keys==123);
    CHECK(!HGNativeFieldMenuUIInputAllowed(NULL,7,9) && !HGNativeFieldMenuUIInputAllowed(&menu,0,9)
        && !HGNativeFieldMenuUIInputAllowed(&menu,7,0) && !HGNativeFieldMenuUIInputAllowed(&menu,7,8));
    CHECK(HGNativeFieldMenuUITouchAllowed(&menu,7,9,255,191) && !HGNativeFieldMenuUITouchAllowed(&menu,7,9,256,191));
    CHECK(!memcmp(&menu,&before,sizeof(menu)));menu.phase=5;menu.childActive=1;menu.viewActive=menu.controlActive=menu.ready=0;
    CHECK(menu.active && !HGNativeFieldMenuUIInputAllowed(&menu,7,9));
    menu=before;menu.phase=17;menu.ready=menu.controlActive=0;CHECK(menu.viewActive && !HGNativeFieldMenuUIInputAllowed(&menu,7,9));
#ifdef HG_NATIVE_FIELD_MENU_UI_STATE_CHECK
    CheckFieldMenuResident();
#endif
    return 0;
}
#ifdef HG_NATIVE_FIELD_MENU_UI_STATE_CHECK
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "start_menu.h"
#include "field_system.h"
#include "field/bottom_screen_app.h"
#include "overlay_01.h"
#include "screen_fade.h"
#include "sys_task.h"
#include "save_vars_flags.h"
#include "constants/std_script.h"
#undef GF_ASSERT
#define GF_ASSERT assert
#include "actual_sys_task.inc"
static SysTaskQueue *testQueue;
SysTask *SysTask_CreateOnVWaitQueue(SysTaskFunc func,void *data,u32 priority){return SysTaskQueue_InsertTask(testQueue,func,data,priority);}
void SysTask_Destroy(SysTask *task){assert(SysTask_Unlink(task));}
#include "actual_resident.inc"
static u8 testBottomId,testBottomState=1;static BOOL testFade=TRUE,testApplication;static uint32_t testFrees,testJumps,testUnpauses,testScripts;
static BOOL Task_StartMenu(TaskManager *task){return FALSE;}
static BOOL OtherTask(TaskManager *task){return FALSE;}
u8 FieldSystem_GetBottomScreenAppId(FieldSystem *field){return testBottomId;}
u8 FieldSystem_GetBottomScreenAppState(FieldSystem *field){return testBottomState;}
BOOL IsPaletteFadeFinished(void){return testFade;}
BOOL FieldSystem_ApplicationIsRunning(FieldSystem *field){return testApplication;}
void Heap_Free(void *memory){assert(!HGNativeFieldMenuUIGetState()->active);++testFrees;}
void TaskManager_Jump(TaskManager *task,TaskFunc func,void *environment){assert(!HGNativeFieldMenuUIGetState()->active);task->func=func;task->env=environment;++testJumps;}
void MapObjectManager_UnpauseAllMovement(MapObjectManager *manager){++testUnpauses;}
static void StartMenu_ResumeUnionRoomSearch(StartMenuTaskData *menu,FieldSystem *field){}
static void StartMenu_RemoveCursorAndIcons(StartMenuTaskData *menu,FieldSystem *field){assert(!HGNativeFieldMenuUIGetState()->ready);NativeFieldMenuUIEndView(menu);menu->cursorActive=FALSE;}
void ScheduleBgTilemapBufferTransfer(BgConfig *bg,u8 id){}
SaveVarsFlags *Save_VarsFlags_Get(SaveData *save){return NULL;}
BOOL Save_VarsFlags_CheckSafariSysFlag(SaveVarsFlags *flags){return FALSE;}
BOOL Save_VarsFlags_CheckBugContestFlag(SaveVarsFlags *flags){return FALSE;}
void StartScriptFromMenu(TaskManager *task,u16 script,LocalMapObject *lastTalked){assert(!HGNativeFieldMenuUIGetState()->active && script==4);++testScripts;}
static BOOL ChildReturned(TaskManager *task){StartMenuTaskData *menu=task->env;menu->state=START_MENU_STATE_RETURN;return FALSE;}
#include "actual_field_services.inc"
#include "actual_producer.inc"
#include "actual_close_cases.inc"
static void PersistentTask(SysTask *task,void *data){}
static void CheckFieldMenuResident(void) {
    void *arena=calloc(1,SysTaskQueue_GetArenaSize(32));assert(arena);testQueue=SysTaskQueue_PlacementNew(32,arena);
    SysTask *persistent=SysTask_CreateOnVWaitQueue(PersistentTask,NULL,5);assert(persistent);
    StartMenuTaskData menu={0};FieldProcessManager processes={0};BgConfig bg={0};FieldSystem field={0};TaskManager task={0},nested={0};
    void *nonnull=&menu;processes.parent=nonnull;field.processManager=&processes;field.runningFieldMap=TRUE;field.bgConfig=&bg;
    bg.bgs[3].tilemapBuffer=nonnull;field.startMenuState=2;field.menuInputState.state=MENU_INPUT_STATE_BUTTONS;
    menu.state=3;menu.cursorActive=TRUE;menu.spriteList=nonnull;menu.cursorSprite=nonnull;
    task.func=Task_StartMenu;task.env=&menu;task.fieldSystem=&field;field.taskman=&task;
    NativeFieldMenuUIBegin(&menu);NativeFieldMenuUIBeginView(&menu);StartMenu_PublishNativeUI(&field);
    assert(!HGNativeFieldMenuUIGetState()->ready);SysTaskQueue_RunTasks(testQueue);StartMenu_PublishNativeUI(&field);
    HGNativeFieldMenuUIState shown=*HGNativeFieldMenuUIGetState();assert(shown.ready && shown.phase==3 && !FieldSystem_IsPlayerMovementAllowed(&field));
    for(uint32_t i=0;i<1000;++i)StartMenu_PublishNativeUI(&field);assert(!memcmp(&shown,HGNativeFieldMenuUIGetState(),sizeof(shown)));
    field.startMenuState=1;StartMenu_PublishNativeUI(&field);assert(!HGNativeFieldMenuUIGetState()->ready);
    field.startMenuState=2;field.lastTouchMenuInput=4;StartMenu_PublishNativeUI(&field);assert(!HGNativeFieldMenuUIGetState()->ready && HGNativeFieldMenuUIGetState()->controlActive);
    field.lastTouchMenuInput=0;StartMenu_PublishNativeUI(&field);assert(HGNativeFieldMenuUIGetState()->ready);
    processes.isPaused=TRUE;StartMenu_PublishNativeUI(&field);assert(!HGNativeFieldMenuUIGetState()->ready);processes.isPaused=FALSE;
    nested.func=OtherTask;nested.prev=&task;field.taskman=&nested;StartMenu_PublishNativeUI(&field);assert(HGNativeFieldMenuUIGetState()->active && !HGNativeFieldMenuUIGetState()->controlActive);
    field.taskman=&task;NativeFieldMenuUIEndView(&menu);menu.cursorActive=FALSE;menu.state=5;processes.child=nonnull;StartMenu_PublishNativeUI(&field);
    assert(HGNativeFieldMenuUIGetState()->active && HGNativeFieldMenuUIGetState()->childActive && !HGNativeFieldMenuUIGetState()->viewActive);
    processes.child=NULL;StartMenu_PublishNativeUI(&field);assert(HGNativeFieldMenuUIGetState()->active && !HGNativeFieldMenuUIGetState()->childActive && !HGNativeFieldMenuUIGetState()->viewActive);
    task.env=&menu;menu.exitTaskFunc=ChildReturned;field.menuInputState.state=MENU_INPUT_STATE_BUTTONS;Task_StartMenu_WaitApp(&task);assert(menu.state==15);
    menu.state=5;field.menuInputState.state=MENU_INPUT_STATE_TOUCH;Task_StartMenu_WaitApp(&task);assert(menu.state==10);
    field.menuInputState.state=MENU_INPUT_STATE_BUTTONS;menu.state=17;menu.cursorActive=TRUE;NativeFieldMenuUIBeginView(&menu);StartMenu_PublishNativeUI(&field);SysTaskQueue_RunTasks(testQueue);
    assert(HGNativeFieldMenuUIGetState()->viewActive && !HGNativeFieldMenuUIGetState()->controlActive && !HGNativeFieldMenuUIGetState()->ready);
    menu.state=3;StartMenu_PublishNativeUI(&field);shown=*HGNativeFieldMenuUIGetState();assert(shown.ready);
    testBottomId=1;menu.state=6;StartMenu_PublishNativeUI(&field);assert(HGNativeFieldMenuUIGetState()->viewActive && !HGNativeFieldMenuUIGetState()->controlActive);testBottomId=0;
    const uint32_t closing[]={11,13,18,16};
    for(size_t i=0;i<4;++i){NativeFieldMenuUIReset();task.func=Task_StartMenu;task.env=&menu;menu.state=closing[i];menu.exitTaskFunc=OtherTask;menu.cursorActive=FALSE;NativeFieldMenuUIBegin(&menu);uint32_t generation=HGNativeFieldMenuUIGetState()->ownerGeneration;ExecuteActualCloseCase(&task);assert(!HGNativeFieldMenuUIGetState()->active && HGNativeFieldMenuUIGetState()->ownerGeneration==generation);}
    assert(testFrees==4 && testJumps==1 && testUnpauses==3);
    task.func=Task_StartMenu;task.env=&menu;menu.state=3;NativeFieldMenuUIBegin(&menu);
    assert(!Task_StartMenu_HandleSelection_Retire(&task) && testScripts==1 && testFrees==5 && field.startMenuState==0);
    NativeFieldMenuUIReset();sNativeFieldMenuUIState.ownerGeneration=sNativeFieldMenuUIState.revision=UINT32_MAX;NativeFieldMenuUIBegin(&menu);
    assert(HGNativeFieldMenuUIGetState()->ownerGeneration==1 && HGNativeFieldMenuUIGetState()->revision==1);
    NativeFieldMenuUIBeginView(&menu);assert(testQueue->activeCount==2);NativeFieldMenuUIRetire(&menu);assert(testQueue->activeCount==1);SysTaskQueue_RunTasks(testQueue);
    SysTask_Destroy(persistent);assert(!testQueue->activeCount);free(arena);
}
#endif
#ifdef HG_NATIVE_FIELD_MENU_UI_CHECK_MAIN
#include <stdio.h>
int main(void){uint32_t line=CheckNativeFieldMenuUI();if(line){fprintf(stderr,"Field menu UI check line %u\n",line);return 1;}puts("PASS: FieldMenu36 original owner/view/control leases, pending-input fences and return lifecycle.");return 0;}
#endif
