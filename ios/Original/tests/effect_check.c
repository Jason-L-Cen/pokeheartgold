// SPDX-License-Identifier: GPL-3.0-or-later
#include "field/field_effect_helpers.h"
#include "heap.h"
#include "system.h"
#include "sys_task_api.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#define CHECK(v) do { if (!(v)) { fprintf(stderr,"Effect check line %d: %s\n",__LINE__,#v); abort(); } } while (0)
typedef struct EffectTrace { int init,update,draw,destroy; BOOL fail,selfDestroy; } EffectTrace;
typedef struct EffectState { u32 ticks; EffectTrace *owner; } EffectState;

static BOOL init(FieldEffect *effect, void *data) {
    EffectState *state=data;
    CHECK(state->ticks==0 && state->owner==NULL);
    state->owner=(EffectTrace *)FieldEffect_GetUserData(effect);
    CHECK(FieldEffect_GetParam(effect)==7);
    VecFx32 position; FieldEffect_GetPosition(effect,&position);
    CHECK(position.x==FX32_ONE && position.y==-FX32_ONE && position.z==2*FX32_ONE);
    state->owner->init++;
    return !state->owner->fail;
}
static void destroy(FieldEffect *effect, void *data) {
    EffectState *state=data;
    CHECK((void *)FieldEffect_GetUserData(effect)==state->owner);
    state->owner->destroy++;
}
static void update(FieldEffect *effect, void *data) {
    EffectState *state=data;
    state->ticks++; state->owner->update++;
    if (state->owner->selfDestroy && state->ticks==2) FieldEffect_Destroy(effect);
}
static void draw(FieldEffect *effect, void *data) {
    EffectState *state=data; state->owner->draw++;
}
void CheckNativeEffects(void) {
    CHECK(gSystem.mainTaskQueue->activeCount==0);
    u32 freeBefore=HeapExp_FndGetTotalFreeSize(HEAP_ID_DEFAULT);
    CHECK(!FieldEffectPool_New(HEAP_ID_DEFAULT,0));
    CHECK(!FieldEffectPool_New(HEAP_ID_DEFAULT,-1));
    CHECK(!FieldEffectPool_New(HEAP_ID_DEFAULT,INT_MAX));
    FieldEffectPool *pool=FieldEffectPool_New(HEAP_ID_DEFAULT,2); CHECK(pool);
    const FieldEffectBasicTemplate template={sizeof(EffectState),init,destroy,update,draw};
    const VecFx32 position={FX32_ONE,-FX32_ONE,2*FX32_ONE};
    EffectTrace first={.selfDestroy=TRUE},second={.fail=TRUE},third={0};
    FieldEffectBasicTemplate oversized=template; oversized.stateSize=129;
    CHECK(!FieldEffectPool_AddBasicEffect(pool,&oversized,&position,7,&first,1));
    CHECK(first.init==0 && gSystem.mainTaskQueue->activeCount==0);
    FieldEffect *a=FieldEffectPool_AddBasicEffect(pool,&template,&position,7,&first,1); CHECK(a);
    CHECK(!FieldEffectPool_AddBasicEffect(pool,&template,&position,7,&second,2));
    CHECK(second.init==1 && second.destroy==0 && gSystem.mainTaskQueue->activeCount==1);
    second.fail=FALSE;
    FieldEffect *b=FieldEffectPool_AddBasicEffect(pool,&template,&position,7,&second,2); CHECK(b && b!=a);
    CHECK(!FieldEffectPool_AddBasicEffect(pool,&template,&position,7,&third,3));
    CHECK(third.init==0 && gSystem.mainTaskQueue->activeCount==2);
    FieldEffectPool_DrawAll(pool); CHECK(first.draw==1 && second.draw==1);
    SysTaskQueue_RunTasks(gSystem.mainTaskQueue); SysTaskQueue_RunTasks(gSystem.mainTaskQueue);
    CHECK(first.update==2 && second.update==2 && first.destroy==1 && gSystem.mainTaskQueue->activeCount==1);
    FieldEffect_Destroy(a); CHECK(first.destroy==1);
    FieldEffect *c=FieldEffectPool_AddBasicEffect(pool,&template,&position,7,&third,3); CHECK(c==a);
    FieldEffectPool_DrawAll(pool); CHECK(first.draw==1 && second.draw==2 && third.draw==1);
    FieldEffectPool_Delete(pool);
    CHECK(second.destroy==1 && third.destroy==1 && gSystem.mainTaskQueue->activeCount==0);
    CHECK(HeapExp_FndGetTotalFreeSize(HEAP_ID_DEFAULT)==freeBefore);
    puts("PASS: original effect pool init failure, capacity, task updates/draw, self-destruction, slot reuse and heap restoration.");
}
