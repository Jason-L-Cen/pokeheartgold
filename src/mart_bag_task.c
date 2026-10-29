#include "mart_bag_task.h"

#include "global.h"

#include "field/fieldmap.h"

#include "bag_view.h"
#include "heap.h"
#include "overlay_03.h"
#include "task.h"
#ifdef HEARTGOLD_NATIVE
#include "native_shop_ui.h"
#include <stddef.h>
#include <string.h>
// main.lsf keeps mart_bag_task.o in Static main, outside both shop overlays.
static HGNativeShopUIState sNativeShopUI;
static const void *sNativeShopOwner;
static uint32_t NativeShopNext(uint32_t value) {return value==UINT32_MAX ? 1 : value+1;}
const HGNativeShopUIState *HGNativeShopUIGetState(void) {return &sNativeShopUI;}
void NativeShopUIBegin(const void *owner,uint32_t kind,uint32_t sellOnly) {
    if(!owner)return;
    uint32_t generation=NativeShopNext(sNativeShopUI.ownerGeneration),revision=NativeShopNext(sNativeShopUI.revision);
    MI_CpuClear8(&sNativeShopUI,sizeof(sNativeShopUI));sNativeShopOwner=owner;
    sNativeShopUI.ownerGeneration=generation;sNativeShopUI.revision=revision;
    sNativeShopUI.kind=kind;sNativeShopUI.active=kind==0 && sellOnly==0;
}
void NativeShopUIInvalidate(const void *owner) {
    if(!owner || owner!=sNativeShopOwner)return;
    HGNativeShopUIState next=sNativeShopUI;next.ready=next.viewActive=0;
    MI_CpuClear8(next.items,sizeof(next.items));MI_CpuClear8(next.buttons,sizeof(next.buttons));next.visibleCount=next.buttonCount=0;
    MI_CpuClear8(&next.money,sizeof(next)-offsetof(HGNativeShopUIState,money));
    if(memcmp(&next,&sNativeShopUI,sizeof(next))!=0)next.revision=NativeShopNext(next.revision);
    sNativeShopUI=next;
}
void NativeShopUIPhase(const void *owner,uint32_t phase) {
    if(!owner || owner!=sNativeShopOwner || sNativeShopUI.phase==phase)return;
    NativeShopUIInvalidate(owner);sNativeShopUI.phase=phase;sNativeShopUI.revision=NativeShopNext(sNativeShopUI.revision);
}
void NativeShopUIPublish(const void *owner,const HGNativeShopUIState *facts) {
    if(!owner || owner!=sNativeShopOwner || !facts || !sNativeShopUI.active)return;
    if(facts->kind || facts->visibleCount>HG_SHOP_UI_ITEMS || facts->buttonCount>HG_SHOP_UI_BUTTONS){NativeShopUIInvalidate(owner);return;}
    HGNativeShopUIState next=*facts;next.active=1;next.ownerGeneration=sNativeShopUI.ownerGeneration;next.revision=sNativeShopUI.revision;
    if(memcmp(&next,&sNativeShopUI,sizeof(next))!=0)next.revision=NativeShopNext(next.revision);
    sNativeShopUI=next;
}
void NativeShopUIRetire(const void *owner) {
    if(!owner || owner!=sNativeShopOwner)return;NativeShopUIInvalidate(owner);sNativeShopOwner=NULL;
    sNativeShopUI.active=0;sNativeShopUI.revision=NativeShopNext(sNativeShopUI.revision);
}
#endif

typedef struct MartReturnEnv {
    u8 unk_00[4];
    BagView *bagView;
    u8 unk_08[0x26A];
    u8 state;
    u8 unk_273[0x2D];
    u16 *result;
} MartReturnEnv;

static BOOL MartBag_WaitBagViewExit(FieldSystem *fieldSystem, MartReturnEnv *env);
static void MartBag_ReturnToMart(TaskManager *taskManager);

BOOL Task_MartBag(TaskManager *taskManager) {
    FieldSystem *fieldSystem = TaskManager_GetFieldSystem(taskManager);
    MartReturnEnv *env = TaskManager_GetEnvironment(taskManager);
    switch (env->state) {
    case 24:
        env->state = MartBag_WaitBagViewExit(fieldSystem, env);
        break;
    case 25:
        MartBag_ReturnToMart(taskManager);
        break;
    }
    return FALSE;
}

static BOOL MartBag_WaitBagViewExit(FieldSystem *fieldSystem, MartReturnEnv *env) {
    if (FieldSystem_ApplicationIsRunning(fieldSystem)) {
        return 24;
    }
    if (env->result != NULL) {
        env->result[1] = BagView_GetSellCount(env->bagView);
    }
    Heap_Free(env->bagView);
    FieldSystem_LoadFieldOverlay(fieldSystem);
    return 25;
}

static void MartBag_ReturnToMart(TaskManager *taskManager) {
    FieldSystem *fieldSystem = TaskManager_GetFieldSystem(taskManager);
    MartReturnEnv *env = TaskManager_GetEnvironment(taskManager);
    if (FieldSystem_IsOverworldRestored(fieldSystem)) {
        FieldMap_FadeScreen(1);
        TaskManager_Jump(taskManager, Task_Mart, env);
        env->state = 26;
    }
}
