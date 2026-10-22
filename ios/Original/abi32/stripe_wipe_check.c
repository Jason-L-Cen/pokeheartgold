// SPDX-License-Identifier: GPL-3.0-or-later
#include "field/encounter_effect.h"
#include "field/hblank_system.h"
#include "battle/battle_anim_util.h"
#include "frontier/frontier_transition.h"
#include "scanline_effect.h"
#include "system.h"
#include "sys_task.h"
#include <nitro/hw/native_graphics.h>
#include "stripe-wave-golden.h"

#define CHECK(value) do { if (!(value)) return __LINE__; } while (0)
static FieldSystem field;
static FieldMapSystems mapSystems;
static EncounterEffectTask effect;
static StripeWipe *wipe;
static HBlankSystem *hblank;
static void *queueArena;
static u32 freeBefore;
static s32 direction;
static u8 pixels[256*192*4];
static const u16 colors[]={0,31,0x03E0,0x7C00};
static u32 failure[4]; // x, y, expected BGR555, actual RGBA8
static const u32 stoppedScroll=0x00550066;

u32 HGStripeDisplayStatus(void) { return (u32)(uaddr)&reg_GX_DISPSTAT; }
u32 HGStripeFailure(void) { return (u32)(uaddr)failure; }
u32 HGStripePixels(void) { return (u32)(uaddr)pixels; }

u32 HGStripeSetup(s32 sign) {
    CHECK(sign==1 || sign==-1); direction=sign;
    freeBefore=HeapExp_FndGetTotalFreeSize(HEAP_ID_DEFAULT);
    CHECK(Heap_Create(HEAP_ID_DEFAULT,HEAP_ID_FIELD1,64*1024));
    memset(&gSystem,0,sizeof(gSystem));
    queueArena=Heap_Alloc(HEAP_ID_FIELD1,SysTaskQueue_GetArenaSize(8)); CHECK(queueArena);
    gSystem.vwaitTaskQueue=SysTaskQueue_PlacementNew(8,queueArena);
    hblank=HBlankSystem_New(HEAP_ID_FIELD1); CHECK(hblank);
    mapSystems.hBlankSystem=hblank; field.mapSystems=&mapSystems; effect.fieldSystem=&field;

    _Alignas(4) u8 tiles[96];
    _Alignas(4) u16 map[32*32];
    memset(tiles,0x11,32); memset(tiles+32,0x22,32); memset(tiles+64,0x33,32);
    for (u32 y=0;y<32;y++) for (u32 x=0;x<32;x++) map[y*32+x]=x%3;
    GX_SetBankForBG(GX_VRAM_BG_128_A);
    reg_GX_DISPCNT=0x10100; reg_G2_BG0CNT=0x0800;
    reg_G2_BG0OFS=reg_G2_BG1OFS=reg_G2_BG2OFS=reg_G2_BG3OFS=0;
    reg_G2_BLDCNT=reg_G2_MOSAIC=reg_GX_MASTER_BRIGHT=0;
    GX_LoadBGPltt(colors,0,sizeof(colors));
    GX_LoadBG0Char(tiles,0,sizeof(tiles)); GX_LoadBG0Scr(map,0,sizeof(map));

    // Keep the real frame waiter alive after the wipe disables HBlank, and in the missing-raster control.
    GX_VBlankIntr(TRUE); OS_EnableIrqMask(OS_IE_VBLANK);
    HBlankSystem_Start(hblank);
    wipe=StripeWipe_New(); CHECK(wipe);
    // Zero quadratic term: the first two original Update calls produce 8px and 12px.
    StripeWipe_Start(&effect,wipe,8,4,sign*FX32_CONST(8),sign*FX32_CONST(24),sign*FX32_CONST(4));
    SysTaskQueue_RunTasks(gSystem.vwaitTaskQueue); // Begin schedules Update; runNow defers it.
    CHECK(gSystem.vwaitTaskQueue->activeCount==1);
    SysTaskQueue_RunTasks(gSystem.vwaitTaskQueue); // Real Update(frame=0).
    CHECK(reg_G2_WIN0H==247 && reg_G2_WIN1H==9 && !effect.wipeDone);
    return 0;
}

u32 HGStripeAdvance(void) {
    SysTaskQueue_RunTasks(gSystem.vwaitTaskQueue); // Real Update(frame=1), not a copied interpolator.
    CHECK(reg_G2_WIN0H==243 && reg_G2_WIN1H==13 && !effect.wipeDone);
    return 0;
}

static u32 checkPixel(u32 x,u32 y,u16 expected) {
    const u8 *p=pixels+(y*256+x)*4;
    if (p[3]!=255 || (p[0]>>3)!=(expected&31) || (p[1]>>3)!=((expected>>5)&31) || (p[2]>>3)!=((expected>>10)&31)) {
        failure[0]=x; failure[1]=y; failure[2]=expected; memcpy(&failure[3],p,4);
        return __LINE__;
    }
    return 0;
}

u32 HGStripeCheckFrame(u32 frame) {
    CHECK(frame<2);
    CHECK(HGNativeRender2D(0,pixels,sizeof(pixels)));
    s32 magnitude=frame ? 12 : 8;
    for (u32 y=0;y<192;y++) {
        // HBlank on line N affects N+1. Row0 inherits blank line262's even stripe.
        s32 offset=direction*(y && (((y-1)/8)&1) ? -magnitude : magnitude);
        for (u32 x=0;x<256;x++) {
            BOOL visible=offset>=0 ? x<(u32)(255-magnitude) : x>(u32)magnitude;
            u16 expected=visible ? colors[1+((((s32)x+offset)&255)/8)%3] : 0;
            u32 line=checkPixel(x,y,expected); if (line) return line;
        }
    }
    // The last real HBlank (191, odd stripe) left all four BG offsets updated.
    u32 last=(-direction*magnitude)&511;
    CHECK(reg_G2_BG0OFS==last && reg_G2_BG1OFS==last && reg_G2_BG2OFS==last && reg_G2_BG3OFS==last);
    CHECK(reg_G2_WIN0H==(u32)(255-magnitude) && reg_G2_WIN1H==(u32)(magnitude+1));
    return 0;
}

u32 HGStripeStop(void) {
    if (hblank) HBlankSystem_Stop(hblank);
    if (wipe) { StripeWipe_Free(wipe); wipe=NULL; }
    if (hblank) { HBlankSystem_Delete(hblank); hblank=NULL; }
    CHECK(!gSystem.hBlankIntr && !gSystem.hBlankIntrArg);
    CHECK(!(OS_GetIrqMask()&OS_IE_HBLANK));
    CHECK(gSystem.vwaitTaskQueue->activeCount==0 && effect.wipeDone);
    reg_G2_BG0OFS=reg_G2_BG1OFS=reg_G2_BG2OFS=reg_G2_BG3OFS=stoppedScroll;
    reg_G2_WIN0H=0x1234; reg_G2_WIN1H=0x3456;
    GX_SetVisibleWnd(GX_WNDMASK_NONE);
    return 0;
}

u32 HGStripeCheckStopped(void) {
    SysTaskQueue_RunTasks(gSystem.vwaitTaskQueue);
    CHECK(reg_G2_BG0OFS==stoppedScroll && reg_G2_BG1OFS==stoppedScroll && reg_G2_BG2OFS==stoppedScroll && reg_G2_BG3OFS==stoppedScroll);
    CHECK(reg_G2_WIN0H==0x1234 && reg_G2_WIN1H==0x3456 && !(reg_GX_DISPCNT&(7u<<13)));
    CHECK(HGNativeRender2D(0,pixels,sizeof(pixels)));
    for (u32 y=0;y<192;y++) for (u32 x=0;x<256;x++) {
        u32 line=checkPixel(x,y,colors[1+(((x+102)&255)/8)%3]); if (line) return line;
    }
    return 0;
}

u32 HGStripeFinish(void) {
    if (hblank || wipe) { u32 line=HGStripeStop(); if (line) return line; }
    Heap_Free(queueArena); queueArena=NULL; gSystem.vwaitTaskQueue=NULL;
    Heap_Destroy(HEAP_ID_FIELD1);
    CHECK(HeapExp_FndGetTotalFreeSize(HEAP_ID_DEFAULT)==freeBefore);
    return 0;
}

// These cases use the original producers, queues and DMA APIs; only the pixel oracle is QA code.
static BattleAnimScanlineWave *dmaWave;
static FrontierHBlankDma *frontier;
static void *dmaArenas[3];
static u32 dmaKind,dmaMode;
static u32 finishedEpochs;
static BOOL checkingMasked;
static _Alignas(4) u32 halfwordPackets[2][193];
extern void VBlankCB_DmaTasksFramecounter(void);
void __real_HGNativeRasterStep(u32 line,void *phase);
void __wrap_HGNativeRasterStep(u32 line,void *phase) {
#ifdef STRIPE_DROP_DMA_MASK
    phase=(void *)((uaddr)phase&255); // Negative control at the actual typed guest boundary.
#endif
    __real_HGNativeRasterStep(line,phase);
    if (((uaddr)phase&255)==HG_NATIVE_RASTER_FINISH) finishedEpochs++;
}
u32 HGScanlineEpochs(void) { return finishedEpochs; }

static void textPattern(void) {
    _Alignas(4) u8 tiles[96];
    _Alignas(4) u16 map[1024];
    memset(tiles,0x11,32); memset(tiles+32,0x22,32); memset(tiles+64,0x33,32);
    for (u32 y=0;y<32;y++) for (u32 x=0;x<32;x++) map[y*32+x]=(x+2*y)%3;
    reg_GX_DISPCNT=0x10400; reg_G2_BG2CNT=0x0800;
    GX_LoadBG2Char(tiles,0,sizeof(tiles)); GX_LoadBG2Scr(map,0,sizeof(map));
}

static u16 textColor(u32 x,u32 y) { return colors[1+((x&255)/8+2*((y&255)/8))%3]; }

static u32 waveWord(u32 frame,u32 line) {
    u32 shift=frame ? (dmaMode==1 ? 191 : 2) : 0;
    s32 delta=line>=8 && line<=160 ? qaWave[dmaMode][(shift+line-8)%192] : 0;
    return 7u<<16 | (u16)(3+delta);
}

static void frontierPackets(u32 frame) {
    FrontierScanlineRegs *buffer=FrontierHBlankDma_GetBuffer(frontier);
    // Exchange X/Y advancement across BG2/BG3 so both axes of each BG are exercised.
    MtxFx22 xMatrix={FX32_ONE,FX32_ONE,0,0},yMatrix={FX32_ONE,0,0,FX32_ONE};
    for (u32 i=0;i<192;i++) {
        G2x_SetBGyAffine_((uaddr)&buffer[i],dmaMode<2 ? &xMatrix : &yMatrix,0,0,frame*4,frame*3);
        G2x_SetBGyAffine_((uaddr)&buffer[i]+16,dmaMode<2 ? &yMatrix : &xMatrix,0,0,frame*4,frame*3);
    }
}

u32 HGScanlineSetup(u32 kind,u32 mode) {
    CHECK(kind<3 && mode<(kind==0 ? 3u : kind==1 ? 4u : 1u));
    dmaKind=kind; dmaMode=mode;
    freeBefore=HeapExp_FndGetTotalFreeSize(HEAP_ID_DEFAULT);
    CHECK(Heap_Create(HEAP_ID_DEFAULT,HEAP_ID_BATTLE,64*1024));
    memset(&gSystem,0,sizeof(gSystem));
    for (u32 i=0;i<3;i++) { dmaArenas[i]=Heap_Alloc(HEAP_ID_BATTLE,SysTaskQueue_GetArenaSize(8)); CHECK(dmaArenas[i]); }
    gSystem.mainTaskQueue=SysTaskQueue_PlacementNew(8,dmaArenas[0]);
    gSystem.vwaitTaskQueue=SysTaskQueue_PlacementNew(8,dmaArenas[1]);
    gSystem.vblankTaskQueue=SysTaskQueue_PlacementNew(8,dmaArenas[2]);
    GX_SetBankForBG(GX_VRAM_BG_128_A); GX_LoadBGPltt(colors,0,sizeof(colors));
    reg_G2_BLDCNT=reg_G2_MOSAIC=reg_GX_MASTER_BRIGHT=0;
    GX_SetVisibleWnd(GX_WNDMASK_NONE);
    textPattern(); reg_G2_BG2OFS=0x00070003;
    // Repeat DMA must work without a CPU HBlank IRQ. VBlank still runs the original queue callback.
    GX_HBlankIntr(FALSE); OS_DisableIrqMask(OS_IE_HBLANK);
    OS_SetIrqFunction(OS_IE_VBLANK,VBlankCB_DmaTasksFramecounter);
    OS_EnableIrqMask(OS_IE_VBLANK); GX_VBlankIntr(TRUE); OS_EnableIrq();
    if (!kind) {
        const u32 steps[]={341,364,910};
        dmaWave=BattleAnimScanline_NewWave(8,160,steps[mode],FX32_CONST(32),mode==1 ? -100 : 200,2,0,0x00070003,HEAP_ID_BATTLE);
        CHECK(dmaWave && gSystem.mainTaskQueue->activeCount==1);
        for (u32 i=0;i<192;i++) CHECK(dmaWave->effect->wave[i]==qaWave[mode][i]);
        SysTaskQueue_RunTasks(gSystem.mainTaskQueue);
    } else if (kind==1) {
        _Alignas(4) u8 tile[64],map[256]={0};
        for (u32 y=0;y<8;y++) for (u32 x=0;x<8;x++) tile[y*8+x]=(x+2*y)%3+1;
        reg_GX_DISPCNT=0x10002 | (mode&1 ? 0x800 : 0x400);
        reg_G2_BG2CNT=reg_G2_BG3CNT=0x2800;
        GX_LoadBG2Char(tile,0,sizeof(tile)); GX_LoadBG2Scr(map,0,sizeof(map));
        GX_LoadBG3Char(tile,0,sizeof(tile)); GX_LoadBG3Scr(map,0,sizeof(map));
        const FrontierScanlineRegs initial={{0}};
        frontier=FrontierHBlankDma_New((void *)REG_BG2PA_ADDR,&initial,HEAP_ID_BATTLE); CHECK(frontier);
        frontierPackets(0);
        FrontierScanlineRegs *buffer=FrontierHBlankDma_GetBuffer(frontier);
        // Initial live registers equal packet0; later equal DMA writes must still reload references.
        MI_CpuCopy32(buffer,(void *)(uaddr)&reg_G2_BG2PA,32);
    } else {
        for (u32 frame=0;frame<2;frame++) for (u32 i=0;i<193;i++)
            halfwordPackets[frame][i]=(i==192 ? 0x00110022 : (((i/3)%3*8+frame*8)<<16 | (i%3*8+frame*4)));
        HBlankDma_Start(halfwordPackets[0],(void *)(uaddr)&reg_G2_BG2OFS,4,0);
    }
    if (kind<2) SysTaskQueue_RunTasks(gSystem.vwaitTaskQueue);
    CHECK(!(OS_GetIrqMask()&OS_IE_HBLANK) && !(reg_GX_DISPSTAT&(1u<<4)));
    CHECK(kind ? !frontier || frontier->pending : dmaWave->tasks.pending);
    return 0;
}

u32 HGScanlineAdvance(void) {
    if (!dmaKind) SysTaskQueue_RunTasks(gSystem.mainTaskQueue);
    else if (dmaKind==1) frontierPackets(1);
    else { HBlankDma_Stop(); HBlankDma_Start(halfwordPackets[1],(void *)REG_BG2HOFS_ADDR,4,0); }
    if (dmaKind<2) SysTaskQueue_RunTasks(gSystem.vwaitTaskQueue);
    return 0;
}

u32 HGScanlineCheckFrame(u32 frame) {
    CHECK(frame<2 && (checkingMasked ? !gSystem.frameCounter : gSystem.frameCounter>=frame+1));
    CHECK(!(OS_GetIrqMask()&OS_IE_HBLANK) && !(reg_GX_DISPSTAT&(1u<<4)));
    if (!dmaKind) {
        CHECK(dmaWave->tasks.pending==checkingMasked && dmaWave->effect->doubleBuffer->current==(frame ? 0 : 1));
        CHECK(dmaWave->effect->phase==(dmaMode==1 ? 19200-(frame+1)*100 : (frame+1)*200));
        const u32 *buffer=DoubleBuffer_GetOther(dmaWave->effect->doubleBuffer);
        for (u32 i=0;i<192;i++) CHECK(buffer[i]==waveWord(frame,i));
        CHECK(reg_G2_BG2OFS==waveWord(frame,191));
    } else if (dmaKind==1) CHECK(!frontier->pending && frontier->buffers->current==(frame ? 0 : 1));
    else CHECK(reg_G2_BG2OFS==halfwordPackets[frame][191]);
    CHECK(HGNativeRender2D(0,pixels,sizeof(pixels)));
    for (u32 y=0;y<192;y++) for (u32 x=0;x<256;x++) {
        u16 expected;
        if (dmaKind==1) {
            BOOL horizontal=dmaMode==0 || dmaMode==3;
            u32 next=frame && y;
            // Same X/Y packet every row: no hidden PB/PD accumulation is allowed.
            u32 sourceX=x+next*(horizontal ? 7 : 4),sourceY=next*(horizontal ? 0 : 3);
            expected=colors[1+((sourceX&7)+2*(sourceY&7))%3];
        } else {
            u32 packet=!y ? (frame && dmaKind==2 ? halfwordPackets[0][191] : 0x00070003) :
                dmaKind==2 ? halfwordPackets[frame][y-1] : waveWord(frame,y-1);
            expected=textColor(x+(packet&0xffff),y+(packet>>16));
        }
        u32 line=checkPixel(x,y,expected); if (line) return line;
    }
    return 0;
}

u32 HGScanlineCheckMasked(void) {
    CHECK(finishedEpochs==1 && !dmaKind && dmaWave);
    checkingMasked=TRUE;
    u32 line=HGScanlineCheckFrame(0);
    checkingMasked=FALSE;
    return line;
}

u32 HGScanlineStop(void) {
    if (dmaWave) {
        BattleAnimScanline_DisableWave(dmaWave); SysTaskQueue_RunTasks(gSystem.vwaitTaskQueue);
        BattleAnimScanline_FreeWave(dmaWave); dmaWave=NULL;
    }
    if (frontier) { FrontierHBlankDma_Delete(frontier); frontier=NULL; }
    HBlankDma_Stop();
    CHECK(!gSystem.mainTaskQueue->activeCount && !gSystem.vwaitTaskQueue->activeCount && !gSystem.vblankTaskQueue->activeCount);
    textPattern(); reg_G2_BG2OFS=0x00070003;
    reg_G2_BG2X=0x123456; reg_G2_BG2Y=0x234567; reg_G2_BG3X=0x345678; reg_G2_BG3Y=0x456789;
    return 0;
}

u32 HGScanlineCheckStopped(void) {
    CHECK(gSystem.frameCounter>0);
    CHECK(reg_G2_BG2OFS==0x00070003 && reg_G2_BG2X==0x123456 && reg_G2_BG2Y==0x234567 && reg_G2_BG3X==0x345678 && reg_G2_BG3Y==0x456789);
    CHECK(!gSystem.mainTaskQueue->activeCount && !gSystem.vwaitTaskQueue->activeCount && !gSystem.vblankTaskQueue->activeCount);
    CHECK(HGNativeRender2D(0,pixels,sizeof(pixels)));
    for (u32 y=0;y<192;y++) for (u32 x=0;x<256;x++) { u32 line=checkPixel(x,y,textColor(x+3,y+7)); if (line) return line; }
    return 0;
}

u32 HGScanlineFinish(void) {
    if (dmaWave || frontier) { u32 line=HGScanlineStop(); if (line) return line; }
    HBlankDma_Stop();
    for (u32 i=0;i<3;i++) { Heap_Free(dmaArenas[i]); dmaArenas[i]=NULL; }
    gSystem.mainTaskQueue=gSystem.vwaitTaskQueue=gSystem.vblankTaskQueue=NULL;
    Heap_Destroy(HEAP_ID_BATTLE);
    CHECK(HeapExp_FndGetTotalFreeSize(HEAP_ID_DEFAULT)==freeBefore);
    return 0;
}
