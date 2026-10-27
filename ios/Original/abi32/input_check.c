// SPDX-License-Identifier: GPL-3.0-or-later
#include <nitro.h>
#include <string.h>
extern void TP_RequestAutoSamplingStartAsync(u16,u16,TPData *,u16);
extern void TP_RequestAutoSamplingStopAsync(void);
extern void TP_WaitBusy(u32);
extern u32 TP_CheckBusy(u32);
extern u32 TP_GetLatestIndexInAuto(void);
extern int TP_CalcCalibrateParam(TPCalibrateParam *,u16,u16,u16,u16,u16,u16,u16,u16);
#define CHECK(v) do { if (!(v)) return __LINE__; } while (0)
static TPData raw,calibrated,ring[9];
static void wire(u32 data,u32 point) { (void)data; (void)point; }
u32 HGInputReference(u32 field) {
    return field==0 ? (u32)(uaddr)&raw : field==1 ? (u32)(uaddr)&calibrated :
        field==2 ? (u32)(uaddr)ring : (u32)(uaddr)wire;
}
u32 HGInputAction(u32 action,u32 value) {
    switch (action) {
    case 0: return PAD_Read();
    case 1: return PAD_DetectFold();
    case 2:
        if (TP_RequestRawSampling(&raw)) return 1;
        TP_GetCalibratedPoint(&calibrated,&raw); return 0;
    case 3:
        TP_RequestAutoSamplingStartAsync(17,(u16)value,ring,9); TP_WaitBusy(2); return TP_CheckBusy(2);
    case 4:
        TP_RequestAutoSamplingStopAsync(); TP_WaitBusy(4); return TP_CheckBusy(4);
    case 5: return TP_GetLatestIndexInAuto();
    case 6:
        TP_GetLatestRawPointInAuto(&raw); TP_GetCalibratedPoint(&calibrated,&raw); return 0;
    case 7: TP_RequestSamplingAsync(); return TP_CheckBusy(1);
    case 8:
        if (TP_WaitRawResult(&raw)) return 1;
        TP_GetCalibratedPoint(&calibrated,&raw); return 0;
    case 9:
        // Original GF clients calibrate every exposed ring entry in place.
        for (u32 i=0;i<9;i++) {TPData point;TP_GetCalibratedPoint(&point,&ring[i]);ring[i]=point;}
        return 0;
    }
    return ~0u;
}
u32 CheckNativeTouch(u32 beforeIRQ) {
    TP_Init(); TP_Init();
    CHECK(!TP_GetUserInfo(NULL));
    TPCalibrateParam param;
    CHECK(TP_GetUserInfo(&param) && !param.x0 && !param.y0 && param.xDotSize==4096 && param.yDotSize==4096);
    TP_SetCalibrateParam(&param);
    if (beforeIRQ) {
        TP_RequestSamplingAsync(); CHECK(TP_WaitRawResult(&raw)==1);
        TP_RequestSamplingAsync(); CHECK(TP_WaitRawResult(&raw)==1);
    }
    OS_InitIrqTable(); OS_EnableIrqMask(OS_IE_SPFIFO_RECV); OS_EnableIrq();
    CHECK(!PAD_Read() && !PAD_DetectFold() && !HGInputAction(2,0));
    CHECK(!raw.touch && raw.validity==TP_VALIDITY_INVALID_XY && !calibrated.x && !calibrated.y);
    TPCalibrateParam calculated;
    CHECK(!TP_CalcCalibrateParam(&calculated,0,0,0,0,4080,3056,255,191));
    CHECK(!memcmp(&param,&calculated,sizeof(param)));
    CHECK(!TP_CalcCalibrateParam(&calculated,4080,3056,255,191,0,0,0,0));
    CHECK(!memcmp(&param,&calculated,sizeof(param)));
    CHECK(TP_CalcCalibrateParam(&calculated,4096,0,0,0,4080,3056,255,191)==1);
    CHECK(TP_CalcCalibrateParam(&calculated,0,0,1,0,4080,3056,1,191)==1);
    const TPData points[]={{0,0,1,0},{4080,3056,1,0},{4095,4095,1,3},{100,200,0,0}};
    const u16 expected[][2]={{0,0},{255,191},{255,191},{0,0}};
    for (u32 i=0;i<4;i++) {
        TP_GetCalibratedPoint(&calibrated,&points[i]);
        CHECK(calibrated.x==expected[i][0] && calibrated.y==expected[i][1] && calibrated.touch==points[i].touch && calibrated.validity==points[i].validity);
    }
    TP_SetCalibrateParam(NULL); TP_GetCalibratedPoint(&calibrated,&points[1]);
    CHECK(!memcmp(&calibrated,&points[1],sizeof(calibrated))); TP_SetCalibrateParam(&param);
    TP_RequestAutoSamplingStartAsync(0,0,ring,9); CHECK(TP_CheckBusy(2));
    TP_RequestAutoSamplingStartAsync(0,1,NULL,9); CHECK(TP_CheckBusy(2));
    TP_RequestAutoSamplingStartAsync(263,1,ring,9); CHECK(TP_CheckBusy(2));
    CHECK(!HGInputAction(3,1) && !HGInputAction(4,0));
    return 0;
}
