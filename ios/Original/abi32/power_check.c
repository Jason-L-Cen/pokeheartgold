// SPDX-License-Identifier: GPL-3.0-or-later
#include <nitro.h>
typedef void (*PmCallback)(u32,void *);
extern u32 PM_SetBackLightAsync(PMLCDTarget,PMBackLightSwitch,PmCallback,void *);
extern u32 PMi_ReadRegister(u32,u16 *);
extern u32 PMi_ReadRegisterAsync(u32,u16 *,PmCallback,void *);
extern u32 PMi_WriteRegister(u32,u32);
extern u32 PM_SendUtilityCommandAsync(u32,PmCallback,void *);
extern void PMi_WaitBusy(void);
extern u32 PMi_SetLED(int);
extern u32 PM_GetLEDPattern(u32 *);
extern u32 PM_SetLCDPower(int);
extern int PM_GetLCDPower(void);
#define CHECK(v) do { if (!(v)) return __LINE__; } while (0)
static u32 calls,lastResult,lastMode,reentered;
static void completed(u32 result,void *argument) {
    if (argument!=&calls) { lastResult=~0u; return; }
    calls++; lastResult=result; lastMode=OS_GetProcMode();
}
static void reentrant(u32 result,void *argument) {
    completed(result,argument);
    if (PM_SetBackLight(PM_LCD_BOTTOM,PM_BACKLIGHT_OFF)) reentered=~0u;
    else reentered++;
}
u32 HGPowerReference(void) { return (u32)(uaddr)completed; }
u32 HGPowerAction(u32 action,u32 value) {
    PM_Init();
    if (!action) {
        PMBackLightSwitch top,bottom;
        return PM_GetBackLight(&top,&bottom) ? ~0u : top|((u32)bottom<<1);
    }
    if (action==1) return PM_SetBackLight((PMLCDTarget)(value&3),(PMBackLightSwitch)(value>>2));
    if (action==2) return PM_SetLCDPower((int)value);
    if (action==3) return PM_GetLCDPower();
    OS_DisableIrq();
    return PM_SetBackLightAsync(PM_LCD_TOP,PM_BACKLIGHT_OFF,completed,&calls);
}
u32 CheckNativePower(u32 beforeIRQ) {
    calls=lastResult=reentered=0;
    PM_Init(); PM_Init();
    if (beforeIRQ) {
        CHECK(PM_SetBackLightAsync(PM_LCD_ALL,PM_BACKLIGHT_OFF,completed,&calls)==2);
        CHECK(PM_SetBackLightAsync(PM_LCD_ALL,PM_BACKLIGHT_OFF,completed,&calls)==2 && !calls);
    }
    OS_InitIrqTable(); OS_EnableIrqMask(OS_IE_SPFIFO_RECV); OS_EnableIrq();
    CHECK(HGPowerAction(0,0)==3 && PM_GetLCDPower()==1);
    CHECK(!PM_SetBackLightAsync(PM_LCD_TOP,PM_BACKLIGHT_OFF,completed,&calls));
    CHECK(calls==1 && !lastResult && lastMode==OS_PROCMODE_IRQ && HGPowerAction(0,0)==2);
    OS_DisableIrq();
    CHECK(!PM_SetBackLightAsync(PM_LCD_TOP,PM_BACKLIGHT_ON,completed,&calls));
    CHECK(PM_SetBackLightAsync(PM_LCD_ALL,PM_BACKLIGHT_OFF,NULL,NULL)==1 && calls==1);
    OSIntrMode previous=OS_DisableInterrupts(); PMi_WaitBusy();
    CHECK(calls==2 && !lastResult && lastMode==OS_PROCMODE_SYS);
    OS_RestoreInterrupts(previous); OS_EnableIrq(); CHECK(calls==2 && HGPowerAction(0,0)==3);
    CHECK(!PM_SetBackLightAsync(PM_LCD_TOP,PM_BACKLIGHT_OFF,reentrant,&calls));
    CHECK(calls==3 && reentered==1 && !lastResult && HGPowerAction(0,0)==0);
    CHECK(!PM_SetBackLight(PM_LCD_ALL,PM_BACKLIGHT_ON));
    CHECK(!PM_SetAmp(PM_AMP_ON) && !PM_SetAmpGain(PM_AMPGAIN_160));
    u16 value=0; CHECK(!PMi_ReadRegister(2,&value) && value==1);
    CHECK(!PMi_ReadRegister(3,&value) && value==3);
    CHECK(!PMi_WriteRegister(4,0x1234) && !PMi_ReadRegister(4,&value) && value==0x34);
    CHECK(PMi_ReadRegisterAsync(5,&value,NULL,NULL)==0xFFFF && PMi_ReadRegisterAsync(0,NULL,NULL,NULL)==0xFFFF);
    CHECK(PMi_WriteRegister(5,1)==0xFFFF && PM_SetBackLight((PMLCDTarget)3,PM_BACKLIGHT_ON)==0xFFFF);
    for (int mode=1;mode<=3;mode++) {
        u32 pattern=0; CHECK(!PMi_SetLED(mode) && !PM_GetLEDPattern(&pattern) && pattern==(u32)mode);
    }
    u32 pattern=0; CHECK(!PMi_SendLEDPatternCommand(0x123) && !PM_GetLEDPattern(&pattern) && pattern==0x23);
    CHECK(!PM_SendUtilityCommandAsync(14,completed,&calls) && lastResult==2);
    CHECK(!PM_GetBackLight(NULL,NULL) && HGPowerAction(0,0)==3);
    return 0;
}
