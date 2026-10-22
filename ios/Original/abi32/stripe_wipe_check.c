// SPDX-License-Identifier: GPL-3.0-or-later
#include "field/encounter_effect.h"
#include "field/hblank_system.h"
#include "system.h"
#include "sys_task.h"
#include <nitro/hw/native_graphics.h>

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
