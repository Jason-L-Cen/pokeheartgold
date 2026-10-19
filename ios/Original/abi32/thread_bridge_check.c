// SPDX-License-Identifier: GPL-3.0-or-later
#include <nitro.h>
#include <nitro/os/thread.h>
#include <nitro/os/mutex.h>
#include <nitro/os/native_thread.h>
#include <string.h>
#define CHECK(v) do { if (!(v)) return __LINE__; } while (0)
static OSThread threads[3];
static _Alignas(16) u8 stacks[3][16384];
static OSThreadQueue gate;
static OSMutex mutex;
static u32 entries,steps,failures,destructors,destructorArgument,afterWait;
static void destructor(void *argument) { destructors++; destructorArgument=(u32)argument; }
static void alternating(void *argument) {
    u32 seed=(u32)argument; volatile u8 local[384];
    for (u32 i=0;i<sizeof(local);i++) local[i]=(u8)(i+seed);
    reg_CP_DIVCNT=2; reg_CP_DIV_NUMER=seed*101; reg_CP_DIV_DENOM=seed;
    entries++; OS_SleepThread(&gate);
    for (u32 step=0;step<12;step++) {
        for (u32 i=0;i<sizeof(local);i++) if (local[i]!=(u8)(i+seed)) failures++;
        steps++; OS_YieldThread();
    }
}
static void locked(void *argument) {
    entries++; OS_LockMutex(&mutex); steps+=(u32)argument; OS_UnlockMutex(&mutex);
}
static void waiting(void *argument) {
    entries++; OS_SleepThread(&gate); afterWait+=(u32)argument;
}
u32 CheckOriginalThreads(void) {
    OS_InitIrqTable(); OS_InitTick(); OS_InitAlarm(); OS_EnableIrq(); OS_InitThread();
    CHECK(OS_IsThreadAvailable() && OS_GetCurrentThread() && OS_GetThreadPriority(OS_GetCurrentThread())==16);
    OS_InitThreadQueue(&gate); OS_InitMutex(&mutex);
    entries=steps=failures=destructors=destructorArgument=afterWait=0;
    reg_CP_DIVCNT=2; reg_CP_DIV_NUMER=0x123456789ABCULL; reg_CP_DIV_DENOM=17;
    reg_CP_SQRTCNT=1; reg_CP_SQRT_PARAM=123456789;
    CPContext rootMath,afterMath; CP_SaveContext(&rootMath);
    for (u32 i=0;i<2;i++) {
        OS_CreateThread(&threads[i],alternating,(void *)(i+7),stacks[i]+sizeof(stacks[i]),sizeof(stacks[i]),8);
        OS_SetThreadDestructor(&threads[i],destructor);
        OS_WakeupThreadDirect(&threads[i]);
        CHECK(entries==i+1 && !steps);
        CP_SaveContext(&afterMath); CHECK(!memcmp(&rootMath,&afterMath,sizeof(rootMath)));
    }
    OS_WakeupThread(&gate);
    OS_JoinThread(&threads[0]); OS_JoinThread(&threads[1]);
    CHECK(entries==2 && steps==24 && !failures && destructors==2);
    CHECK(OS_IsThreadTerminated(&threads[0]) && OS_IsThreadTerminated(&threads[1]));
    OS_LockMutex(&mutex); OS_LockMutex(&mutex);
    OS_CreateThread(&threads[0],locked,(void *)5,stacks[0]+sizeof(stacks[0]),sizeof(stacks[0]),8);
    OS_WakeupThreadDirect(&threads[0]); CHECK(entries==3 && steps==24);
    OS_UnlockMutex(&mutex); CHECK(steps==24);
    OS_UnlockMutex(&mutex); OS_JoinThread(&threads[0]); CHECK(steps==29);
    OS_CreateThread(&threads[1],waiting,(void *)1,stacks[1]+sizeof(stacks[1]),sizeof(stacks[1]),8);
    OS_SetThreadDestructor(&threads[1],destructor); OS_WakeupThreadDirect(&threads[1]);
    CHECK(entries==4 && !afterWait);
    OS_KillThread(&threads[1],(void *)0xBEEF); OS_JoinThread(&threads[1]);
    CHECK(destructors==3 && destructorArgument==0xBEEF && !afterWait);
    OS_CreateThread(&threads[2],waiting,(void *)1,stacks[2]+sizeof(stacks[2]),sizeof(stacks[2]),8);
    OS_WakeupThreadDirect(&threads[2]); CHECK(entries==5);
    OS_DestroyThread(&threads[2]); CHECK(OS_IsThreadTerminated(&threads[2]) && !afterWait);
    OS_CreateThread(&threads[2],waiting,(void *)1,stacks[2]+sizeof(stacks[2])-4,sizeof(stacks[2])-4,8);
    OS_WakeupThreadDirect(&threads[2]); CHECK(entries==6);
    OS_DestroyThread(&threads[2]); CHECK(OS_IsThreadTerminated(&threads[2]) && !afterWait);
    OSTick before=OS_GetTick(); OS_Sleep(3);
    CHECK(OS_GetTick()>before && OS_GetCurrentThread() && OS_GetProcMode()==OS_PROCMODE_SYS);
    CHECK(OS_GetCpsrIrq()==OS_INTRMODE_IRQ_ENABLE && !failures);
    return 0;
}
