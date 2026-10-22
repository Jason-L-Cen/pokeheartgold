// SPDX-License-Identifier: GPL-3.0-or-later
#include "system.h"
#include "sys_task_api.h"
#include "../NativeEvents.h"
#include <nitro/os/tick.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define CHECK(value) do { if (!(value)) { fprintf(stderr, "Runtime check line %d: %s\n", __LINE__, #value); abort(); } } while (0)
typedef struct Trace { int values[32], count, inserted; SysTask *future, *created; } Trace;
static Trace trace;
static pthread_t owner;
void VBlankCB_DmaTasksFramecounter(void);

static void record(SysTask *task, void *data) {
    Trace *state = data;
    if (SysTask_GetPriority(task) == 22) CHECK(pthread_equal(owner, pthread_self()) && OS_GetProcMode() == OS_PROCMODE_IRQ);
    state->values[state->count++] = (int)SysTask_GetPriority(task);
}
static void mutate(SysTask *task, void *data) {
    Trace *state = data; record(task, data);
    if (!state->inserted) {
        state->inserted = TRUE;
        state->created = SysTaskQueue_InsertTask(task->queue, record, data, 2);
        CHECK(state->created && SysTask_Unlink(state->future));
    }
}
static int argumentCalls, plainCalls;
static u32 irqExits;
static void irqExit(void *argument) { ++*(u32 *)argument; }
static void argumentIRQ(void *data) { CHECK(OS_GetProcMode() == OS_PROCMODE_IRQ); argumentCalls += *(int *)data; }
static void plainIRQ(void) { CHECK(OS_GetProcMode() == OS_PROCMODE_IRQ); plainCalls++; }
static void *postVBlank(void *unused) {
    (void)unused;
    struct timespec delay = {0, 3000000}; nanosleep(&delay, NULL);
    HGNativeSignalVBlank(); return NULL;
}
static SysTaskQueue *queue(u32 capacity) {
    u32 size = SysTaskQueue_GetArenaSize(capacity);
    void *storage = OS_AllocFromArenaLo(OS_ARENA_MAIN, size, 8);
    CHECK(storage); memset(storage, 0xA5, size);
    return SysTaskQueue_PlacementNew(capacity, storage);
}

void CheckNativeRuntime(void) {
    SysTaskQueue *tasks = queue(4);
    CHECK(tasks->nextTask == &tasks->headSentinel);
    SysTask *first = SysTaskQueue_InsertTask(tasks, mutate, &trace, 1);
    trace.future = SysTaskQueue_InsertTask(tasks, record, &trace, 3);
    CHECK(first && trace.future);
    SysTaskQueue_RunTasks(tasks);
    CHECK(trace.count == 1 && trace.values[0] == 1 && tasks->activeCount == 2);
    SysTaskQueue_RunTasks(tasks);
    CHECK(trace.count == 3 && trace.values[1] == 1 && trace.values[2] == 2);
    CHECK(SysTask_Unlink(first) && SysTask_Unlink(trace.created) && tasks->activeCount == 0);
    memset(&trace, 0, sizeof(trace));
    gSystem.mainTaskQueue = queue(160); gSystem.vblankTaskQueue = queue(32);
    gSystem.vwaitTaskQueue = queue(32); gSystem.printTaskQueue = queue(4);
    SysTask *main = SysTask_CreateOnMainQueue(record, &trace, 11);
    SysTask *vblank = SysTask_CreateOnVBlankQueue(record, &trace, 22);
    SysTask *print = SysTask_CreateOnPrintQueue(record, &trace, 33);
    SysTask *vwait = SysTask_CreateOnVWaitQueue(record, &trace, 44);
    CHECK(main && vblank && print && vwait);
    owner = pthread_self(); OS_InitIrqTable(); OS_SetIrqFunction(OS_IE_VBLANK, VBlankCB_DmaTasksFramecounter);
    CHECK(OS_GetIrqFunction(OS_IE_VBLANK) == VBlankCB_DmaTasksFramecounter);
    OS_EnableIrqMask(OS_IE_VBLANK); OS_EnableIrq();
    CHECK(HGNativeSetIrqExitHook(irqExit,&irqExits,NULL));
    HGNativeDispatchIrqs(); CHECK(!irqExits);
    SysTaskQueue_RunTasks(gSystem.mainTaskQueue); SysTaskQueue_RunTasks(gSystem.printTaskQueue);
    OSIntrMode disabled = OS_DisableInterrupts();
    HGNativeSignalVBlank(); CHECK(trace.count == 2 && (OS_GetRequestIrqMask() & OS_IE_VBLANK));
    OS_RestoreInterrupts(disabled);
    CHECK(trace.count == 3 && trace.values[2] == 22 && OS_GetProcMode() == OS_PROCMODE_SYS);
    CHECK(irqExits==1 && HGNativeClearIrqExitHook(&irqExits));
    OS_DisableIrqMask(OS_IE_VBLANK); HGNativeSignalVBlank(); CHECK(trace.count == 3);
    OS_EnableIrqMask(OS_IE_VBLANK); CHECK(trace.count == 4);
    pthread_t producer; CHECK(pthread_create(&producer, NULL, postVBlank, NULL) == 0);
    OS_WaitIrq(TRUE, OS_IE_VBLANK); CHECK(pthread_join(producer, NULL) == 0);
    CHECK(trace.count == 5 && OS_GetVBlankCount() == 3 && gSystem.frameCounter == 3);
    CHECK(!pthread_create(&producer,NULL,postVBlank,NULL));
    OS_SpinWait(1340559); CHECK(!pthread_join(producer,NULL));
    CHECK(trace.count==6 && OS_GetVBlankCount()==4 && gSystem.frameCounter==4);
    disabled=OS_DisableInterrupts();
    CHECK(!pthread_create(&producer,NULL,postVBlank,NULL));
    OS_SpinWait(1340559); CHECK(!pthread_join(producer,NULL));
    CHECK(trace.count==6 && (OS_GetRequestIrqMask()&OS_IE_VBLANK));
    OS_RestoreInterrupts(disabled); CHECK(trace.count==7 && gSystem.frameCounter==5);
    SysTaskQueue_RunTasks(gSystem.vwaitTaskQueue); CHECK(trace.values[trace.count - 1] == 44);
    int beforeHalt=trace.count;
    disabled=OS_DisableInterrupts();
    CHECK(!pthread_create(&producer,NULL,postVBlank,NULL));
    OS_Halt(); CHECK(!pthread_join(producer,NULL));
    CHECK(trace.count==beforeHalt && OS_GetCpsrIrq()==OS_INTRMODE_IRQ_DISABLE && (OS_GetRequestIrqMask()&OS_IE_VBLANK));
    OS_Halt(); CHECK(trace.count==beforeHalt);
    OS_RestoreInterrupts(disabled); CHECK(trace.count==beforeHalt+1);
    CHECK(!pthread_create(&producer,NULL,postVBlank,NULL));
    OS_Halt(); CHECK(!pthread_join(producer,NULL)); CHECK(trace.count==beforeHalt+2);
    int value = 7; OSi_EnterDmaCallback(0, argumentIRQ, &value);
    HGNativePostIrq(OS_IE_DMA0);
    CHECK(argumentCalls == 7 && !(OS_GetIrqMask() & OS_IE_DMA0) && (OS_GetIrqCheckFlag() & OS_IE_DMA0));
    OS_SetIrqFunction(OS_IE_TIMER3, plainIRQ); OS_EnableIrqMask(OS_IE_TIMER3);
    HGNativePostIrq(OS_IE_TIMER3); HGNativePostIrq(OS_IE_TIMER3);
    CHECK(plainCalls == 1 && OS_GetIrqFunction(OS_IE_TIMER3) == NULL);
    SysTask_Destroy(main); SysTask_Destroy(vblank); SysTask_Destroy(print); SysTask_Destroy(vwait);
    OS_DisableIrq(); OS_InitIrqTable();

    CHECK(!OS_IsTickAvailable()); OS_InitTick();
    OSTick base = 0x123456780000ULL; OS_SetTick(base);
    OSTick before = OS_GetTick();
    struct timespec start, end, delay = {0, 3000000};
    clock_gettime(CLOCK_MONOTONIC, &start); nanosleep(&delay, NULL);
    OSTick after = OS_GetTick(); clock_gettime(CLOCK_MONOTONIC, &end);
    u64 elapsedNS = (u64)(end.tv_sec - start.tv_sec) * 1000000000 + end.tv_nsec - start.tv_nsec;
    CHECK(before >= base && after > before);
    CHECK(after - before < OS_MilliSecondsToTicks(elapsedNS / 1000000 + 20));
    OS_InitTick(); CHECK(OS_GetTick() >= after && OSi_TickCounter >= (base >> 16));
    printf("PASS: original task insertion/deletion/deferral, four queues, native IRQ masks/context/wait, interruptible CPU delays, typed callbacks and monotonic DS-rate ticks.\n");
}
