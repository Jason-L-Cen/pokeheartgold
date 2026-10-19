// SPDX-License-Identifier: GPL-3.0-or-later
#include <nitro.h>
#include <string.h>
static struct { u32 marker,callbacks,afterWait,failure,bootParameter,resetOccurred; } state;
static OSThread worker;
static _Alignas(16) u8 stack[16384];
static OSThreadQueue gate;
static OSAlarm alarm;
static RTCTime clock;
static _Alignas(32) u8 microphone[32];
static _Alignas(32) u32 source[64],destination[64];
static void rtcDone(RTCResult result,void *argument) { (void)result; (void)argument; state.callbacks++; }
static void micDone(MICResult result,void *argument) { (void)result; (void)argument; state.callbacks++; }
static void powerDone(u32 result,void *argument) { (void)result; (void)argument; state.callbacks++; }
extern u32 PM_SetBackLightAsync(PMLCDTarget,PMBackLightSwitch,void (*)(u32,void *),void *);
static void done(void *argument) { (void)argument; state.callbacks++; }
static void resetting(void *argument) { state.marker=2; OS_ResetSystem((u32)(uaddr)argument); state.afterWait++; }
static void waiting(void *argument) { (void)argument; state.marker=3; OS_SleepThread(&gate); state.afterWait++; }
u32 HGResetStateAddress(void) { return (u32)(uaddr)&state; }
u64 HGResetBootInfo(void) {
    OS_InitReset(); OS_InitReset();
    return ((u64)OS_IsResetOccurred()<<32)|OS_GetResetParameter();
}
void HGResetScenario(u32 scenario,u32 parameter) {
    memset(&state,0,sizeof(state)); state.bootParameter=OS_GetResetParameter(); state.resetOccurred=OS_IsResetOccurred();
    OS_InitIrqTable(); OS_InitTick(); OS_InitAlarm(); OS_InitThread(); OS_InitThreadQueue(&gate);
    OS_DisableIrq(); OS_EnableIrqMask(OS_IE_SPFIFO_RECV|OS_IE_DMA0|OS_IE_TIMER1);
    RTC_Init();
    if (RTC_GetTimeAsync(&clock,rtcDone,NULL)!=RTC_RESULT_SUCCESS) state.failure=1;
    MIC_Init();
    MICAutoParam mic={MIC_SAMPLING_TYPE_SIGNED_8BIT,microphone,sizeof(microphone),MIC_SAMPLING_RATE_8K,TRUE,micDone,NULL};
    if (MIC_StartAutoSamplingAsync(&mic,micDone,NULL)!=MIC_RESULT_SUCCESS) state.failure=2;
    PM_Init();
    if (PM_SetBackLightAsync(PM_LCD_TOP,PM_BACKLIGHT_OFF,powerDone,NULL)) state.failure=4;
    MI_DmaCopy32Async(0,source,destination,sizeof(source),done,NULL);
    OS_CreateAlarm(&alarm); OS_SetAlarm(&alarm,OS_MilliSecondsToTicks(60000),done,NULL);
    state.marker=1;
    if (scenario) {
        OS_CreateThread(&worker,scenario==1 ? resetting : waiting,(void *)(uaddr)parameter,stack+sizeof(stack),sizeof(stack),4);
        OS_WakeupThreadDirect(&worker);
        if (scenario==2) state.marker=4;
        else state.failure=3;
    }
    OS_ResetSystem(parameter); state.afterWait++;
}
u32 HGResetReadFile(void) {
    FSFile file; u8 bytes[8]={0};
    FS_Init(FS_DMA_NOT_USE); FS_InitFile(&file);
    if (!FS_OpenFile(&file,"progress.sav")) return 1;
    int count=FS_ReadFile(&file,bytes,sizeof(bytes)); BOOL closed=FS_CloseFile(&file);
    return count==8 && closed && !memcmp(bytes,"progress",8) ? 0 : 2;
}
