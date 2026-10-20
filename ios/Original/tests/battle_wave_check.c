// SPDX-License-Identifier: GPL-3.0-or-later
#define Heap_Alloc WaveCheckAlloc
#define Heap_Free WaveCheckFree
#define SysTask_CreateOnMainQueue WaveCheckTask
#define SysTask_CreateOnVWaitQueue WaveCheckTask
#define SysTask_CreateOnVBlankQueue WaveCheckTask
#define SysTask_Destroy WaveCheckDestroyTask
#define MI_StopDma WaveCheckStopDma
#define MI_WaitDma WaveCheckWaitDma
#define MI_HBlankDmaCopy32 WaveCheckDma32
#define MI_HBlankDmaCopy16 WaveCheckDma16
#include "../../../src/battle/battle_anim_util.c"
#include "../../../src/scanline_effect.c"
#include <stdio.h>
#include <stdlib.h>

#define CHECK(value) do { if (!(value)) return __LINE__; } while (0)
#define REQUIRE(value) do { if (!(value)) { fprintf(stderr,"Wave fixture line %u: %s\n",__LINE__,#value); abort(); } } while (0)
static struct { SysTaskFunc callback; void *data; } jobs[3];
static u32 allocated,created,destroyed,transfers;
static const void *dmaSource;
void *WaveCheckAlloc(enum HeapID heap,u32 size) { REQUIRE(heap==HEAP_ID_BATTLE); void *p=calloc(1,size); REQUIRE(p); allocated++; return p; }
void WaveCheckFree(void *p) { if (p) { allocated--; free(p); } }
SysTask *WaveCheckTask(SysTaskFunc callback,void *data,u32 priority) {
    REQUIRE(created<3 && !priority); jobs[created].callback=callback; jobs[created].data=data;
    return (SysTask *)&jobs[created++];
}
void WaveCheckDestroyTask(SysTask *task) { REQUIRE(task); destroyed++; }
void WaveCheckStopDma(u32 channel) { REQUIRE(!channel); }
void WaveCheckWaitDma(u32 channel) { REQUIRE(!channel); }
void WaveCheckDma32(u32 channel,const void *src,void *dst,u32 size) {
    // DMA takes the address-valued constant; ABI32 maps register lvalues separately.
    REQUIRE(!channel && (uaddr)dst==(uaddr)REG_BG2HOFS_ADDR && size==4); transfers++; dmaSource=src;
}
void WaveCheckDma16(u32 channel,const void *src,void *dst,u32 size) { REQUIRE(FALSE); }

u32 CheckOriginalBattleWave(void) {
    const u32 steps[]={341,364,910},samples[]={16,48,96,144,191};
    // Independent sine samples at the original 16-unit angle/table precision, amplitude 32.
    const s16 expected[][5]={{15,32,0,-32,-2},{16,31,-7,-31,11},{31,-28,27,-1,-27}};
    typedef BattleAnimScanlineWave *(*Factory)(u8,u8,u32,fx32,s16,int,u32,u32,enum HeapID);
    Factory factory=BattleAnimScanline_NewWave;
    for (u32 test=0;test<3;test++) {
        CHECK(!allocated); created=destroyed=transfers=0; dmaSource=NULL;
        s16 speed=test==1 ? -100 : 200;
        BattleAnimScanlineWave *wave=factory(8,160,steps[test],FX32_CONST(32),speed,2,0,0x00070003,HEAP_ID_BATTLE);
        CHECK(wave && allocated==3 && created==3 && wave->tasks.active && !wave->tasks.pending);
        ScanlineEffect *effect=wave->effect;
        CHECK(effect->first==8 && effect->last==160 && effect->speed==speed);
        for (u32 i=0;i<5;i++) CHECK(effect->wave[samples[i]]==expected[test][i]);
        jobs[0].callback((SysTask *)&jobs[0],jobs[0].data);
        CHECK(effect->phase==(speed<0 ? 19100 : 200));
        CHECK(effect->buffers[0][7]==0x00070003 && effect->buffers[0][8]==0x00070003);
        CHECK((s16)effect->buffers[0][24]==3+expected[test][0] && effect->buffers[0][24]>>16==7);
        jobs[1].callback((SysTask *)&jobs[1],jobs[1].data);
        CHECK(transfers==1 && dmaSource==effect->buffers[0] && wave->tasks.pending);
        jobs[2].callback((SysTask *)&jobs[2],jobs[2].data);
        CHECK(transfers==2 && !wave->tasks.pending);
        jobs[2].callback((SysTask *)&jobs[2],jobs[2].data); CHECK(transfers==2);
        BattleAnimScanline_DisableWave(wave);
        jobs[1].callback((SysTask *)&jobs[1],jobs[1].data); CHECK(transfers==2);
        BattleAnimScanline_FreeWave(wave); CHECK(!allocated && destroyed==3);
    }
    return 0;
}
#ifndef HEARTGOLD_ABI32
void HGNativeAssertFail(const char *condition,const char *file,int line) { fprintf(stderr,"%s:%d: %s\n",file,line,condition); abort(); }
int main(void) { u32 line=CheckOriginalBattleWave(); if (line) { fprintf(stderr,"Battle wave check line %u\n",line); return 1; } puts("PASS: original wide wave steps, signed phase, buffers, VWait/VBlank callbacks and cleanup."); return 0; }
#endif
