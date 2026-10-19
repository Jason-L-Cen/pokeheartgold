// SPDX-License-Identifier: GPL-3.0-or-later
#include <nitro/os/interrupt.h>
#include <nitro/os/irqTable.h>
#include <nitro/os/irqHandler.h>
#include <nitro/os/system.h>
#include <nitro/os/native_irq.h>
#include <nitro/fx/fx_cp.h>
#include <nitro/hw/io_reg.h>
#include <nitro/cp/context.h>
#include <string.h>
#define CHECK(v) do { if (!(v)) return __LINE__; } while (0)
static u32 count,order[16],argument=7,rearm,modeFailure;
static u32 stackAddress,stackFailure;
static _Alignas(16) u8 smallStack[1024];
static u32 configuration[4];
static CPContext mathBefore;
static void stackProbe(void) {
    volatile u8 local[8192]; stackAddress=(u32)(uaddr)local;
    for (u32 i=0;i<sizeof(local);i++) local[i]=(u8)(i*7);
    if (FX_Div(6*4096,-2*4096)!=-3*4096) stackFailure=1;
    reg_CP_SQRTCNT=1; reg_CP_SQRT_PARAM=~0ULL;
    if (reg_CP_SQRT_RESULT!=0xFFFFFFFFu) stackFailure=1;
    for (u32 i=0;i<sizeof(local);i++) if (local[i]!=(u8)(i*7)) stackFailure=1;
    OS_SetIrqCheckFlag(OS_IE_HBLANK);
}
static void overflowProbe(void) {
    volatile u8 local[HG_IRQ_STACK_SIZE+256];
    // If the checked prologue is missing, the body fails with a different trap before any stores.
    HGNativeIrqControl(~0u,0);
    for (u32 i=0;i<sizeof(local);i++) local[i]=(u8)i;
}
static void resetStackFromIrq(void) { OS_SetIrqStackChecker(); }
u32 HGIRQMathState(u32 set) {
    if (set) {
        reg_CP_DIVCNT=2; reg_CP_DIV_NUMER=0x123456789ABCULL; reg_CP_DIV_DENOM=17;
        reg_CP_SQRTCNT=1; reg_CP_SQRT_PARAM=123456789;
        CP_SaveContext(&mathBefore); return 1;
    }
    CPContext after; CP_SaveContext(&after);
    return !memcmp(&after,&mathBefore,sizeof(after));
}
void HGIRQInit(void) { OS_InitIrqTable(); }
void HGIRQHalt(void) { OS_Halt(); }
void HGIRQDispatch(void) { OS_IrqHandler(0); }
void HGIRQResetStack(void) { OS_SetIrqStackChecker(); }
static void record(u32 value) {
    if (OS_GetProcMode()!=OS_PROCMODE_IRQ || OS_GetCpsrIrq()!=OS_INTRMODE_IRQ_DISABLE || count>=16) { modeFailure=1; return; }
    order[count++]=value;
}
static void plain0(void) { record(1); OS_SetIrqCheckFlag(OS_IE_VBLANK); }
static void plain1(void) { record(2); OS_SetIrqCheckFlag(OS_IE_HBLANK); }
static void withArgument(void *value) {
    record(*(u32 *)value);
    if (rearm) { rearm--; OSi_EnterTimerCallback(0,withArgument,value); }
}
static u32 wrongType(void) { return 99; }
u32 HGIRQReference(u32 kind) {
    switch (kind) {
    case 0: return (u32)(uaddr)plain0;
    case 1: return (u32)(uaddr)plain1;
    case 2: return (u32)(uaddr)withArgument;
    case 3: return (u32)(uaddr)wrongType;
    case 4: return (u32)(uaddr)&argument;
    case 5: return (u32)(uaddr)stackProbe;
    case 6: return (u32)(uaddr)smallStack;
    case 7: return (u32)(uaddr)configuration;
    case 8: return (u32)(uaddr)overflowProbe;
    case 9: return (u32)(uaddr)resetStackFromIrq;
    }
    return 0;
}
u32 HGIRQAction(u32 command,u32 value) { return HGNativeIrqControl(command,value); }
void HGIRQRegister(u32 kind,u32 target,u32 function,u32 arg) { HGNativeIrqSet(kind,target,function,arg); }
u32 HGIRQGet(u32 mask) { return (u32)(uaddr)OS_GetIrqFunction(mask); }
u32 HGIRQRead(u32 field) { return field==0 ? count : field<=16 ? order[field-1] : field==17 ? modeFailure : field==18 ? stackAddress : stackFailure; }
void HGIRQArm(u32 repeats) { rearm=repeats; OSi_EnterTimerCallback(0,withArgument,&argument); }
u32 CheckOriginalIRQ(void) {
    OS_InitIrqTable(); count=modeFailure=rearm=0;
    CHECK(OS_GetIrqFunction(OS_IE_VBLANK)==OS_IrqDummy && OS_GetIrqFunction(OS_IE_TIMER0)==NULL);
    CHECK(OS_GetIrqFunction(0)==NULL && OS_GetIrqFunction(0x80000000)==NULL);
    CHECK(!OS_DisableIrq() && !OS_GetIrqMask());
    OS_SetIrqFunction(OS_IE_VBLANK,plain0); OS_SetIrqFunction(OS_IE_HBLANK,plain1);
    CHECK(OS_GetIrqFunction(OS_IE_VBLANK|OS_IE_HBLANK)==plain0);
    CHECK(!OS_SetIrqMask(OS_IE_VBLANK|OS_IE_HBLANK));
    CHECK(OS_DisableIrqMask(OS_IE_HBLANK)==(OS_IE_VBLANK|OS_IE_HBLANK));
    CHECK(OS_EnableIrqMask(OS_IE_HBLANK)==OS_IE_VBLANK);
    OS_SetIrqCheckFlag(OS_IE_VBLANK|OS_IE_HBLANK); OS_ClearIrqCheckFlag(OS_IE_VBLANK);
    CHECK(OS_GetIrqCheckFlag()==OS_IE_HBLANK); OS_ClearIrqCheckFlag(~0u);
    CHECK(!OS_GetIrqCheckFlag() && !OS_GetVBlankCount());
    return 0;
}
