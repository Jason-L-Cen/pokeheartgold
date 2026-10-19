// SPDX-License-Identifier: GPL-3.0-or-later
#include <nitro/os/interrupt.h>
#include <nitro/os/irqTable.h>
#include <nitro/os/system.h>
#include <nitro/os/native_irq.h>
#define CHECK(v) do { if (!(v)) return __LINE__; } while (0)
static u32 count,order[16],argument=7,rearm,modeFailure;
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
    }
    return 0;
}
u32 HGIRQAction(u32 command,u32 value) { return HGNativeIrqControl(command,value); }
void HGIRQRegister(u32 kind,u32 target,u32 function,u32 arg) { HGNativeIrqSet(kind,target,function,arg); }
u32 HGIRQGet(u32 mask) { return (u32)(uaddr)OS_GetIrqFunction(mask); }
u32 HGIRQRead(u32 field) { return field==0 ? count : field<=16 ? order[field-1] : modeFailure; }
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
