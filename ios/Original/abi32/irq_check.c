// SPDX-License-Identifier: GPL-3.0-or-later
#include <nitro/os/interrupt.h>
#include <nitro/os/irqTable.h>
#include <nitro/os/irqHandler.h>
#include <nitro/os/system.h>
#include <nitro/os/thread.h>
#include <nitro/os/native_irq.h>
#include <nitro/fx/fx_cp.h>
#include <nitro/hw/io_reg.h>
#include <nitro/cp/context.h>
#include <nitro/hw/native_graphics.h>
#include <nitro/mi/dma.h>
#include <string.h>
#define CHECK(v) do { if (!(v)) return __LINE__; } while (0)
static u32 count,order[16],argument=7,rearm,modeFailure;
static u32 stackAddress,stackFailure;
static _Alignas(16) u8 smallStack[1024];
static u32 configuration[4];
static CPContext mathBefore;
// FINISH count/local/packed/mode/I, short local/line/phase, HBlank/VCount counts, deferred-IRQ proof/failure, worker state.
static u32 rasterProbeData[13];
static const u32 affineProbeValue=0x12345678;
static OSThread rasterWorkerThread;
static _Alignas(16) u8 rasterWorkerStack[1024];
static void rasterHBlank(void) {
    volatile u32 local=0;
    if (OS_GetProcMode()!=OS_PROCMODE_IRQ || OS_GetCpsrIrq()!=OS_INTRMODE_IRQ_DISABLE) rasterProbeData[11]=1;
    rasterProbeData[8]++;
    if (rasterProbeData[6]==191 && rasterProbeData[7]==HG_NATIVE_RASTER_HBLANK) {
        MI_DmaCopy32(2,&affineProbeValue,(void *)&reg_G2_BG2X,4);
        MI_DmaCopy32(2,&affineProbeValue,(void *)&reg_G2_BG3X,4);
        MI_DmaCopy32(2,&affineProbeValue,(void *)&reg_G2S_DB_BG2Y,4);
        MI_DmaCopy32(2,&affineProbeValue,(void *)&reg_G2S_DB_BG3Y,4);
    }
    OS_SetIrqCheckFlag(OS_IE_HBLANK);
    local++; (void)local;
}
static void rasterVCount(void) {
    if (OS_GetProcMode()!=OS_PROCMODE_IRQ || OS_GetCpsrIrq()!=OS_INTRMODE_IRQ_DISABLE) rasterProbeData[11]=1;
    rasterProbeData[9]++;
}
static void rasterProbe(u32 line,void *packedPointer) {
    volatile u8 local[8192];
    u32 packed=(u32)(uaddr)packedPointer,phase=packed&255;
    HGNativeRasterStep(line,packedPointer);
    if (phase==HG_NATIVE_RASTER_FINISH) {
        rasterProbeData[0]++; rasterProbeData[1]=(u32)(uaddr)local; rasterProbeData[2]=packed;
        rasterProbeData[3]=OS_GetProcMode(); rasterProbeData[4]=OS_GetCpsrIrq();
        if (OS_GetProcMode()!=OS_PROCMODE_SYS) { rasterProbeData[11]=1; return; }
        for (u32 i=0;i<sizeof(local);i++) local[i]=(u8)(i*7);
        if (FX_Div(6*4096,-2*4096)!=-3*4096) rasterProbeData[11]=1;
        reg_CP_SQRTCNT=1; reg_CP_SQRT_PARAM=~0ULL;
        if (reg_CP_SQRT_RESULT!=0xFFFFFFFFu) rasterProbeData[11]=1;
        u32 before=rasterProbeData[9],frames=rasterProbeData[0];
        OS_EnableIrqMask(OS_IE_V_COUNT);
        OS_GetIrqCheckFlag();
        rasterProbeData[10]=rasterProbeData[9]==before;
        if (rasterProbeData[0]!=frames) rasterProbeData[11]=1;
        for (u32 i=0;i<sizeof(local);i++) if (local[i]!=(u8)(i*7)) rasterProbeData[11]=1;
    } else {
        rasterProbeData[5]=(u32)(uaddr)local; rasterProbeData[6]=line; rasterProbeData[7]=phase;
        if (OS_GetProcMode()!=OS_PROCMODE_IRQ || OS_GetCpsrIrq()!=OS_INTRMODE_IRQ_DISABLE) rasterProbeData[11]=1;
    }
}
static void rasterWorker(void *unused) {
    (void)unused;
    volatile u8 local[128]; CPContext before,after;
    for (u32 i=0;i<sizeof(local);i++) local[i]=(u8)(i*13);
    reg_CP_DIVCNT=2; reg_CP_DIV_NUMER=0x987654321ULL; reg_CP_DIV_DENOM=23;
    reg_CP_SQRTCNT=1; reg_CP_SQRT_PARAM=987654321;
    CP_SaveContext(&before);
    u32 started=rasterProbeData[0];
    rasterProbeData[12]=1;
    while (rasterProbeData[0]-started<2) OS_Halt();
    // Pending presentation must not spin either wait path or prevent later hardware epochs.
    OS_SpinWait(1340559);
    // Exercise the native wait without the SDK wait's intentional switch back to root.
    HGNativeIrqControl(HG_IRQ_WAIT_CLEAR,OS_IE_HBLANK);
    CP_SaveContext(&after);
    if (memcmp(&before,&after,sizeof(before))) rasterProbeData[11]=1;
    for (u32 i=0;i<sizeof(local);i++) if (local[i]!=(u8)(i*13)) rasterProbeData[11]=1;
    rasterProbeData[12]=2;
}
static void runRasterWorker(void) {
    OS_CreateThread(&rasterWorkerThread,rasterWorker,NULL,rasterWorkerStack+sizeof(rasterWorkerStack),sizeof(rasterWorkerStack),8);
    u32 first=*(u32 *)rasterWorkerStack,last=*(u32 *)(rasterWorkerStack+sizeof(rasterWorkerStack)-4);
    OS_WakeupThreadDirect(&rasterWorkerThread); OS_JoinThread(&rasterWorkerThread);
    if (!OS_IsThreadTerminated(&rasterWorkerThread) || *(u32 *)rasterWorkerStack!=first ||
        *(u32 *)(rasterWorkerStack+sizeof(rasterWorkerStack)-4)!=last) rasterProbeData[11]=1;
}
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
    case 10: return (u32)(uaddr)rasterProbe;
    case 11: return (u32)(uaddr)rasterProbeData;
    case 12: return (u32)(uaddr)rasterHBlank;
    case 13: return (u32)(uaddr)rasterVCount;
    case 14: return (u32)(uaddr)&reg_GX_DISPSTAT;
    case 15: return (u32)(uaddr)OS_InitThread;
    case 16: return (u32)(uaddr)runRasterWorker;
    case 17: return (u32)(uaddr)rasterWorkerStack;
    case 18: return (u32)(uaddr)HGNativeRender2D;
    }
    return 0;
}
u32 HGIRQAction(u32 command,u32 value) { return HGNativeIrqControl(command,value); }
void HGIRQRegister(u32 kind,u32 target,u32 function,u32 arg) { HGNativeIrqSet(kind,target,function,arg); }
u32 HGIRQGet(u32 mask) { return (u32)(uaddr)OS_GetIrqFunction(mask); }
u32 HGIRQRead(u32 field) { return field>=20 && field<33 ? rasterProbeData[field-20] : field==0 ? count : field<=16 ? order[field-1] : field==17 ? modeFailure : field==18 ? stackAddress : stackFailure; }
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
