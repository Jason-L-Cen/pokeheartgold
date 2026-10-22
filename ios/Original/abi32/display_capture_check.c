// SPDX-License-Identifier: GPL-3.0-or-later
#include "field/encounter_effect.h"
#include "field/fieldmap.h"
#include "field/field_motion_blur.h"
#include "gf_gfx_planes.h"
#include "legendary_cinematic_internal.h"
#include "system.h"
#include <nitro/gx/g3b.h>
#include <nitro/gx/native_geometry.h>
#include <nitro/hw/native_graphics.h>
#include <nitro/cp/context.h>
#include <nitro/os/thread.h>
#include <nitro/os/native_irq.h>
#include <nnsys/gfd_internal.h>
#include <stdio.h>

#define CHECK(v) do { if (!(v)) return __LINE__; } while (0)
_Static_assert(sizeof(VecFx32)==12 && sizeof(GraphicsModes)==16 && sizeof(BgTemplate)==28 && sizeof(GraphicsBanks)==40,"Original particle constant spans");
_Static_assert(sizeof(EncounterEffectState)==12 && sizeof(HGNativeDisplayCaptureInfo)==12,"Original state and capture wire layout");
_Static_assert(sizeof(FrmTexVramRegion)*5==120 && sizeof(FrmPlttVramManager)==12,"Original VRAM manager spans");
static FieldSystem field;
static void *arenas[3];
static u32 parentFree,kind,requestedWidth,control;
static HGNativeGeometryFrameInfo geometryBefore;
static _Alignas(4) u32 commands[256];
static u8 pixels[512*192*4],reference[512*192*4];
static struct { u32 stage,kind,finishes,mode,stack,cpsr,x,y,expected,actual; } witness;
static CPContext mathBefore;
static OSThread worker;
static _Alignas(16) u8 workerStack[1024];
static u32 workerFailure,cancelled,epochs,unsupportedRequest;
static FieldMotionBlur *fieldBlur;
static LegendaryCinematicCapture *legendaryBlur;
static u32 blurFreeBefore;
void __real_HGNativeRasterStep(u32 line,void *phase);
void __wrap_HGNativeRasterStep(u32 line,void *phase) {
    __real_HGNativeRasterStep(line,phase);
    if (((uaddr)phase&255)==HG_NATIVE_RASTER_FINISH) epochs++;
}
void __real_HGNativeFinishDisplayCapture(void);
void __wrap_HGNativeFinishDisplayCapture(void) {
    volatile u32 marker=0x12345678;
    witness.finishes++; witness.mode=OS_GetProcMode(); witness.stack=(u32)(uaddr)&marker; witness.cpsr=OS_GetCpsrIrq();
    // Invalid-source transaction checks call the same producer on the root after
    // this real epoch; a fatal callback trap must never unwind through Events.
    if (!unsupportedRequest) __real_HGNativeFinishDisplayCapture();
    GF_ASSERT(marker==0x12345678);
    // The real typed guard must restore math state even when this producer changes it.
    reg_CP_DIVCNT=2; reg_CP_DIV_NUMER=~0ULL; reg_CP_DIV_DENOM=3;
    reg_CP_SQRTCNT=1; reg_CP_SQRT_PARAM=~0ULL;
}
extern void VBlankCB_DmaTasksFramecounter(void);

u32 HGCaptureWitness(void) { return (u32)(uaddr)&witness; }
u32 HGCaptureDisplayStatus(void) { return (u32)(uaddr)&reg_GX_DISPSTAT; }
u32 HGCaptureControl(void) { return reg_GX_DISPCAPCNT; }
u32 HGCaptureEpochs(void) { return epochs; }
u32 HGCaptureMath(u32 seed) {
    CPContext now={0};
    if (seed) {
        reg_CP_DIVCNT=2; reg_CP_DIV_NUMER=0x123456789ABCULL; reg_CP_DIV_DENOM=17;
        reg_CP_SQRTCNT=1; reg_CP_SQRT_PARAM=123456789;
        memset(&mathBefore,0,sizeof(mathBefore)); CP_SaveContext(&mathBefore); return 1;
    }
    CP_SaveContext(&now); return !memcmp(&now,&mathBefore,sizeof(now));
}
static void smallWorker(void *argument) {
    (void)argument; volatile u32 canary[8];
    for (u32 i=0;i<8;i++) canary[i]=0x13579BDF+i;
    // This extra backend case rearms the original numeric request after worker entry;
    // an epoch pumped by OS_InitThread/entry on the root cannot satisfy this proof.
    memset((void *)(uaddr)HGNativeGraphicsBank(2),0,0x18000);
    epochs=witness.finishes=0; reg_GX_DISPCAPCNT=control;
    CPContext before={0},after={0};
    reg_CP_DIVCNT=2; reg_CP_DIV_NUMER=0x23456789ABCDULL; reg_CP_DIV_DENOM=19;
    reg_CP_SQRTCNT=1; reg_CP_SQRT_PARAM=987654321; CP_SaveContext(&before);
    while (!witness.finishes) OS_Halt();
    CP_SaveContext(&after);
    if (memcmp(&before,&after,sizeof(before)) || OS_GetProcMode()!=OS_PROCMODE_SYS ||
        OS_GetCpsrIrq()!=OS_INTRMODE_IRQ_ENABLE ||
        ((uaddr)&canary<(uaddr)workerStack || (uaddr)&canary>=(uaddr)(workerStack+sizeof(workerStack))) ||
        (witness.stack>=(uaddr)workerStack && witness.stack<(uaddr)(workerStack+sizeof(workerStack)))) workerFailure=__LINE__;
    for (u32 i=0;i<8;i++) if (canary[i]!=0x13579BDF+i) workerFailure=__LINE__;
}
u32 HGCaptureSmallWorker(void) {
    OS_InitThread();
    OSThread *root=OS_GetCurrentThread(); CHECK(root);
    OS_CreateThread(&worker,smallWorker,NULL,workerStack+sizeof(workerStack),sizeof(workerStack),8);
    OS_WakeupThreadDirect(&worker); OS_JoinThread(&worker);
    CHECK(!workerFailure && OS_GetCurrentThread()==root && OS_IsThreadTerminated(&worker));
    return 0;
}
static void cancelAtLine80(void) { cancelled++; reg_GX_DISPCAPCNT&=~0x80000000u; }
void HGCaptureCancelAtLine80(void) {
    OS_SetIrqFunction(OS_IE_V_COUNT,cancelAtLine80); OS_EnableIrqMask(OS_IE_V_COUNT);
    GX_SetVCountEqVal(80); GX_VCountEqIntr(TRUE);
}
void HGCaptureReset(void) { OS_ResetSystem(1); }

static u16 expected(u32 x,u32 y) {
    u32 index=(y<96 ? 0 : 2)+(x<128 ? 0 : 1);
    if (x>=32 && x<40 && y>=32 && y<40) index=4;
    static const u16 a[]={0x801F,0x83E0,0xFC00,0xFFFF,0x83FF};
    static const u16 absent[]={0x8008,0x8100,0xA000,0xA108,0x8108};
    static const u16 green[]={0x82E8,0x83E0,0xA2E0,0xA3E8,0x83E8};
    static const u16 blue[]={0xDC08,0xDD00,0xFC00,0xFD08,0xDD08};
    static const u16 bright[]={0x9088,0x9104,0xA084,0xA108,0x8108};
    if (kind==3) return 0;
    if (kind==5) return a[index];
    if (kind==6) return 0x83E0;
    if (kind==2) return green[index];
    if (kind==4) return y<64 ? green[index] : blue[index];
    if (kind==7) {
        if (x>=16 && x<48 && y>=16 && y<48 && index!=4) return 0x8000;
        return bright[index];
    }
    return absent[index];
}

static u32 checkBanks(BOOL painted) {
    const u16 *bank=(const u16 *)HGNativeGraphicsBank(2);
    for (u32 y=0;y<192;y++) for (u32 x=0;x<256;x++) {
        u16 want=painted && !x && !y ? 0x801F : expected(x,y);
        if (bank[y*256+x]!=want) {
            witness.x=x; witness.y=y; witness.expected=want; witness.actual=bank[y*256+x];
            return __LINE__;
        }
    }
    for (u32 i=0x18000;i<0x20000;i++) CHECK(((const u8 *)bank)[i]==0xA5);
    const u16 *other=(const u16 *)HGNativeGraphicsBank(3);
    for (u32 i=0;i<0x10000;i++) CHECK(other[i]==0x6C37);
    return 0;
}

static u32 publishGeometry(const u16 colors[4]) {
    GXDLInfo list; G3_BeginMakeDL(&list,commands,sizeof(commands));
    for (u32 i=0;i<4;i++) {
        s16 x=(i&1) ? 0 : -4096,y=(i&2) ? -4096 : 0;
        G3B_PolygonAttr(&list,0,GX_POLYGONMODE_MODULATE,GX_CULL_NONE,0,31,0);
        G3B_Color(&list,colors[i]); G3B_Begin(&list,GX_BEGIN_QUADS);
        G3B_Vtx(&list,x,y,0); G3B_Vtx(&list,x+4096,y,0);
        G3B_Vtx(&list,x+4096,y+4096,0); G3B_Vtx(&list,x,y+4096,0); G3B_End(&list);
    }
    CHECK(HGNativeGeometryFifo(commands,G3_EndMakeDL(&list)/4)); G3_SwapBuffers(0,0);
    CHECK(HGNativeGeometryFrame(1,&geometryBefore)); return 0;
}

static u32 publishFeedback(u32 frame) {
    static const u16 input[]={0x1F,0x7C00,0x7FFF};
    CHECK(frame<3);
    u16 colors[]={input[frame],input[frame],input[frame],0x7FFF};
    return publishGeometry(colors);
}

u32 HGCaptureSetup(u32 test) {
    CHECK(test<10); kind=witness.kind=test; witness.stage=1;
    requestedWidth=(test==0 || test==2 || test==4 || test>=8) ? 512 : 256;
    parentFree=HeapExp_FndGetTotalFreeSize(HEAP_ID_DEFAULT);
    CHECK(Heap_Create(HEAP_ID_DEFAULT,HEAP_ID_FIELD1,0x20000));
    memset(&field,0,sizeof(field)); memset(&gSystem,0,sizeof(gSystem));
    field.fieldMapUpdateFlags=5; field.bgConfig=BgConfig_Alloc(HEAP_ID_FIELD1); CHECK(field.bgConfig);
    for (u32 i=0;i<3;i++) { arenas[i]=Heap_Alloc(HEAP_ID_FIELD1,SysTaskQueue_GetArenaSize(8)); CHECK(arenas[i]); }
    gSystem.mainTaskQueue=SysTaskQueue_PlacementNew(8,arenas[0]);
    gSystem.vwaitTaskQueue=SysTaskQueue_PlacementNew(8,arenas[1]);
    gSystem.vblankTaskQueue=SysTaskQueue_PlacementNew(8,arenas[2]);
    GX_SetBankForBG(GX_VRAM_BG_128_C); GX_SetBankForTex(GX_VRAM_TEX_01_AB);
    GX_SetBankForOBJ(GX_VRAM_OBJ_64_E);
    Thunk_BgConfig_Init(field.bgConfig);
    CHECK(HGNativeSetViewWidth(requestedWidth)); G3X_Init(); G3X_SetClearColor(0,31,32767,0,FALSE);
    G3X_SetFog(FALSE,0,0,0); G3X_AlphaBlend(TRUE); G3_ViewPort(0,0,255,191);
    const u16 colors[]={31,0x3E0,0x7C00,0x7FFF};
    u32 line=publishGeometry(colors); if (line) return line;
    _Alignas(4) u16 oam[512]={0},palette[16]={0,0x3FF}; _Alignas(4) u8 tile[32]; memset(tile,0x11,sizeof(tile));
    for (u32 i=0;i<128;i++) oam[i*4]=0x200;
    oam[0]=32; oam[1]=32;
    GX_LoadOAM(oam,0,sizeof(oam)); GX_LoadOBJ(tile,0,sizeof(tile)); GX_LoadOBJPltt(palette,0,sizeof(palette));
    const u16 backdrop=0; GX_LoadBGPltt(&backdrop,0,2);
    reg_GX_DISPCNT=0x10108; GfGfx_EngineASetPlanes(GX_PLANEMASK_BG0|GX_PLANEMASK_OBJ);
    reg_G2_BG0CNT=0; reg_G2_BG0OFS=0; reg_G2_BLDCNT=reg_G2_MOSAIC=0;
    reg_GX_MASTER_BRIGHT=test==0 ? 0x4010 : test==1 ? 0x8010 : 0;
    reg_GXS_DB_DISPCNT=0x10000; GX_SetVisibleWnd(GX_WNDMASK_NONE);
    if (kind==7) {
        reg_G2_BLDCNT=(2<<6)|1; reg_G2_BLDY=8;
        reg_G2_WIN0H=(16<<8)|48; reg_G2_WIN0V=(16<<8)|48;
        reg_G2_WININ=16|32; reg_G2_WINOUT=63; GX_SetVisibleWnd(GX_WNDMASK_W0);
    }
    u16 *guard=(u16 *)(uaddr)HGNativeGraphicsBank(3);
    for (u32 i=0;i<0x10000;i++) guard[i]=0x6C37;
    witness.stage=2;
    if (kind>=8) {
        GfGfx_EngineASetPlanes(GX_PLANEMASK_BG0);
        line=publishFeedback(0); if (line) return line;
        if (kind==9) CHECK(Heap_Create(HEAP_ID_DEFAULT,HEAP_ID_153,0x10000));
        blurFreeBefore=HeapExp_FndGetTotalFreeSize(kind==8 ? HEAP_ID_FIELD1 : HEAP_ID_153);
        if (kind==8) { fieldBlur=FieldMotionBlur_Start(3,15); CHECK(fieldBlur && fieldBlur->capture && !fieldBlur->capture->started); }
        else { legendaryBlur=LegendaryCinematic_StartMotionBlur(5,13); CHECK(legendaryBlur && !legendaryBlur->started); }
        CHECK(reg_GX_DISPCAPCNT==0xC0320010);
        CHECK(gSystem.mainTaskQueue->activeCount==1 && gSystem.vwaitTaskQueue->activeCount==1);
        SysTaskQueue_RunTasks(gSystem.mainTaskQueue); CHECK(reg_GX_DISPCAPCNT==0xC0320010);
        SysTaskQueue_RunTasks(gSystem.vwaitTaskQueue);
        CHECK(!gSystem.vwaitTaskQueue->activeCount && gSystem.mainTaskQueue->activeCount==1);
        CHECK(kind==8 ? fieldBlur->capture->started : legendaryBlur->started);
        CHECK(((reg_GX_DISPCNT>>16)&15)==10 && (reg_GX_VRAMCNT_C&0x87)==0x80);
    } else {
        EncounterEffect_StartParticleMode(&field); // The actual original arm, including bank-C clear and both tasks.
        CHECK(reg_GX_DISPCAPCNT==0xC0320C04 && gEncounterEffectState.particles);
        CHECK(gSystem.mainTaskQueue->activeCount==1 && gSystem.vwaitTaskQueue->activeCount==1);
        CHECK(!EncounterEffect_IsParticleSetupDone());
    }
    for (u32 i=0;i<0x20000;i++) CHECK(((const u8 *)HGNativeGraphicsBank(2))[i]==0);
    memset((u8 *)(uaddr)HGNativeGraphicsBank(2)+0x18000,0xA5,0x8000);
    control=kind>=8 ? 0xC0320010 : 0xC0320C04;
    if (kind==1 || kind==2 || kind==4 || kind==6) {
        GX_ResetBankForTex(); GX_SetBankForTex(GX_VRAM_TEX_0_B);
        u16 *b=(u16 *)(uaddr)HGNativeGraphicsBank(0);
        for (u32 i=0;i<0x10000;i++) b[i]=kind==4 && i>=0x4000 ? 0xFC00 : kind==1 ? 0x3E0 : 0x83E0;
    }
    if (kind==3) control=0xC0320000;
    if (kind==4) { control|=1u<<26; reg_GX_DISPCNT=(reg_GX_DISPCNT&~0x30000)|(2u<<16); }
    if (kind==5) control&=~0x60000000u;
    if (kind==6) control=(control&~0x60000000u)|(1u<<29);
    reg_GX_DISPCAPCNT=control;
    OS_SetIrqFunction(OS_IE_VBLANK,VBlankCB_DmaTasksFramecounter);
    OS_EnableIrqMask(OS_IE_VBLANK); OS_EnableIrq(); GX_VBlankIntr(TRUE);
    witness.stage=3; return 0;
}

static u32 freeFixture(void) {
    for (u32 i=1;i<4;i++) FreeBgTilemapBuffer(field.bgConfig,i);
    Heap_Free(field.bgConfig); field.bgConfig=NULL;
    for (u32 i=0;i<3;i++) { Heap_Free(arenas[i]); arenas[i]=NULL; }
    gSystem.mainTaskQueue=gSystem.vwaitTaskQueue=gSystem.vblankTaskQueue=NULL;
    Heap_Destroy(HEAP_ID_FIELD1); CHECK(HeapExp_FndGetTotalFreeSize(HEAP_ID_DEFAULT)==parentFree);
    witness.stage=6; return 0;
}
static u32 finishFixture(void) {
    EncounterEffect_EndParticleMode(); CHECK(!gEncounterEffectState.particles && (field.fieldMapUpdateFlags&5)==5);
    return freeFixture();
}

static u16 feedbackExpected(u32 frame,u32 x,u32 y) {
    // Literal independent RGB555A1 oracle; the white quarter saturates on both recurring blends.
    static const u16 fieldColors[]={0x801F,0x981D,0xACDF};
    static const u16 legendaryColors[]={0x801F,0xA819,0xC95E};
    return x>=128 && y>=96 ? 0xFFFF : kind==8 ? fieldColors[frame] : legendaryColors[frame];
}

static u32 feedbackAfterFrame(u32 frame) {
    CHECK(frame<4 && witness.finishes==frame+1 && witness.mode==OS_PROCMODE_SYS);
    CHECK(!(reg_GX_DISPCAPCNT&0x80000000u) && HGNativeViewWidth()==requestedWidth);
    const u16 *bank=(const u16 *)HGNativeGraphicsBank(2);
    for (u32 y=0;y<192;y++) for (u32 x=0;x<256;x++) {
        u16 want=frame<3 ? feedbackExpected(frame,x,y) : 0;
        if (bank[y*256+x]!=want) { witness.x=x; witness.y=y; witness.expected=want; witness.actual=bank[y*256+x]; return __LINE__; }
    }
    for (u32 i=0x18000;i<0x20000;i++) CHECK(((const u8 *)bank)[i]==(frame<3 ? 0xA5 : 0));
    for (u32 i=0;i<0x10000;i++) CHECK(((const u16 *)HGNativeGraphicsBank(3))[i]==0x6C37);
    if (frame==3) {
        CHECK(!gSystem.mainTaskQueue->activeCount && !gSystem.vwaitTaskQueue->activeCount);
        SysTaskQueue_RunTasks(gSystem.mainTaskQueue); CHECK(!(reg_GX_DISPCAPCNT&0x80000000u));
        for (u32 i=0;i<0x10000;i++) CHECK(bank[i]==0);
        if (kind==9) Heap_Destroy(HEAP_ID_153);
        return freeFixture();
    }
    CHECK(((reg_GX_DISPCNT>>16)&15)==10 && (reg_GX_VRAMCNT_C&0x87)==0x80);
    HGNativeGeometryFrameInfo now; CHECK(HGNativeGeometryFrame(1,&now) && !memcmp(&now,&geometryBefore,sizeof(now)));
    CHECK(HGNativeRender2D(0,pixels,sizeof(pixels)));
    for (u32 y=0;y<192;y++) for (u32 x=0;x<256;x++) {
        u16 want=frame ? feedbackExpected(frame-1,x,y) : 0;
        const u8 *p=pixels+(y*256+x)*4;
        if ((p[0]>>3)!=(want&31) || (p[1]>>3)!=((want>>5)&31) || (p[2]>>3)!=((want>>10)&31) || p[3]!=255) {
            witness.x=x; witness.y=y; witness.expected=want; memcpy(&witness.actual,p,4); return __LINE__;
        }
    }
    memcpy(reference,pixels,256*192*4);
    CHECK(HGNativeRender2D(1,pixels,sizeof(pixels)) && HGNativeRender2D(0,pixels,sizeof(pixels)));
    CHECK(!memcmp(reference,pixels,256*192*4));
    memset(pixels,0xA7,sizeof(pixels)); CHECK(!HGNativeRender2DView(0,512,pixels,sizeof(pixels)));
    for (u32 i=0;i<sizeof(pixels);i++) CHECK(pixels[i]==0xA7);
    CHECK(!(reg_GX_DISPCAPCNT&0x80000000u) && HGNativeViewWidth()==requestedWidth);
    for (u32 y=0;y<192;y++) for (u32 x=0;x<256;x++) CHECK(bank[y*256+x]==feedbackExpected(frame,x,y));
    SysTaskQueue_RunTasks(gSystem.mainTaskQueue);
    control=kind==8 ? 0xC0320F03 : 0xC0320D05;
    CHECK(reg_GX_DISPCAPCNT==control && gSystem.mainTaskQueue->activeCount==1);
    if (frame<2) { witness.stage=7+frame; return publishFeedback(frame+1); }
    if (kind==8) FieldMotionBlur_Stop(&fieldBlur);
    else LegendaryCinematic_StopMotionBlur(legendaryBlur);
    // Public Stop intentionally leaves the pending E bit armed; the next FINISH must only consume it.
    CHECK(reg_GX_DISPCAPCNT==control && ((reg_GX_DISPCNT>>16)&3)==1 && (reg_GX_VRAMCNT_C&0x87)==0x81);
    CHECK(!gSystem.mainTaskQueue->activeCount && !gSystem.vwaitTaskQueue->activeCount);
    CHECK(HeapExp_FndGetTotalFreeSize(kind==8 ? HEAP_ID_FIELD1 : HEAP_ID_153)==blurFreeBefore);
    for (u32 i=0;i<0x10000;i++) CHECK(bank[i]==0);
    witness.stage=9; return 0;
}
u32 HGCaptureCancelled(void) {
    CHECK(cancelled==1 && witness.finishes==1 && !(reg_GX_DISPCAPCNT&0x80000000u));
    for (u32 i=0;i<0x18000;i++) CHECK(((const u8 *)HGNativeGraphicsBank(2))[i]==0);
    for (u32 i=0x18000;i<0x20000;i++) CHECK(((const u8 *)HGNativeGraphicsBank(2))[i]==0xA5);
    EncounterEffect_RequestParticleSetup(); SysTaskQueue_RunTasks(gSystem.vwaitTaskQueue);
    SysTaskQueue_RunTasks(gSystem.vwaitTaskQueue); SysTaskQueue_RunTasks(gSystem.mainTaskQueue);
    CHECK(EncounterEffect_IsParticleSetupDone()); return finishFixture();
}
u32 HGCaptureUnwritten(void) {
    for (u32 i=0;i<0x18000;i++) CHECK(((const u8 *)HGNativeGraphicsBank(2))[i]==0);
    for (u32 i=0x18000;i<0x20000;i++) CHECK(((const u8 *)HGNativeGraphicsBank(2))[i]==0xA5);
    for (u32 i=0;i<0x10000;i++) CHECK(((const u16 *)HGNativeGraphicsBank(3))[i]==0x6C37);
    return 0;
}
void HGCaptureUnsupported(u32 test) {
    GF_ASSERT(test<3);
    unsupportedRequest=1;
    if (!test) reg_GX_DISPCAPCNT|=1u<<24;
    else if (test==1) reg_GX_DISPCAPCNT|=1u<<25;
    else reg_GX_DISPCNT|=0x80;
}
void HGCaptureUnsupportedFinish(void) { __real_HGNativeFinishDisplayCapture(); }

u32 HGCaptureAfterFrame(u32 frame) {
    if (kind>=8) return feedbackAfterFrame(frame);
    u32 line=checkBanks(frame!=0); if (line) return line;
    CHECK(frame<3 && witness.finishes>=frame+1 && witness.mode==OS_PROCMODE_SYS);
    CHECK(!(reg_GX_DISPCAPCNT&0x80000000u) && HGNativeViewWidth()==requestedWidth);
    HGNativeGeometryFrameInfo now; CHECK(HGNativeGeometryFrame(1,&now) && !memcmp(&now,&geometryBefore,sizeof(now)));
    if (!frame) {
        witness.stage=4;
        // Rearming in an already consumed epoch and ordinary renders must not write/clear capture.
        ((u16 *)(uaddr)HGNativeGraphicsBank(2))[0]=0x801F; reg_GX_DISPCAPCNT=control;
        CHECK(HGNativeRender2D(0,pixels,sizeof(pixels))); memcpy(reference,pixels,256*192*4);
        CHECK(HGNativeRender2D(1,pixels,sizeof(pixels)) && HGNativeRender2D(0,pixels,sizeof(pixels)));
        CHECK(!memcmp(reference,pixels,256*192*4));
        if (requestedWidth==512 && kind!=4) {
            CHECK(HGNativeRender2DView(0,512,pixels,sizeof(pixels)));
            for (u32 y=0;y<192;y++) CHECK(!memcmp(pixels+(y*512+128)*4,reference+y*256*4,256*4));
        }
        CHECK(reg_GX_DISPCAPCNT==control && ((const u16 *)HGNativeGraphicsBank(2))[0]==0x801F);
        reg_GX_DISPCAPCNT=control&~0x80000000u;
        EncounterEffect_RequestParticleSetup();
        SysTaskQueue_RunTasks(gSystem.mainTaskQueue); SysTaskQueue_RunTasks(gSystem.vwaitTaskQueue);
        CHECK(!EncounterEffect_IsParticleSetupDone()); // Original WAIT_FRAME cannot be skipped by an early request.
    } else if (frame==1) {
        reg_GX_MASTER_BRIGHT=0; reg_G2_BLDCNT=0; GX_SetVisibleWnd(GX_WNDMASK_NONE);
        SysTaskQueue_RunTasks(gSystem.vwaitTaskQueue); SysTaskQueue_RunTasks(gSystem.mainTaskQueue);
        CHECK(EncounterEffect_IsParticleSetupDone() && !(field.fieldMapUpdateFlags&5));
        CHECK(!gSystem.mainTaskQueue->activeCount && !gSystem.vwaitTaskQueue->activeCount);
        CHECK((reg_GX_DISPCNT&7)==3 && (reg_G2_BG3CNT&0xFFFC)==0x4884);
        CHECK(reg_G2_BG3PA==256 && !reg_G2_BG3PB && !reg_G2_BG3PC && reg_G2_BG3PD==256 && !reg_G2_BG3X && !reg_G2_BG3Y);
        witness.stage=5;
    } else {
        CHECK(HGNativeRender2D(0,pixels,sizeof(pixels)));
        for (u32 y=0;y<192;y++) for (u32 x=0;x<256;x++) {
            u16 value=!x && !y ? 0x801F : expected(x,y); if (!(value&0x8000)) value=0;
            const u8 *p=pixels+(y*256+x)*4;
            if ((p[0]>>3)!=(value&31) || (p[1]>>3)!=((value>>5)&31) || (p[2]>>3)!=((value>>10)&31) || p[3]!=255) {
                witness.x=x; witness.y=y; witness.expected=value; memcpy(&witness.actual,p,4); return __LINE__;
            }
        }
        return finishFixture();
    }
    return 0;
}
