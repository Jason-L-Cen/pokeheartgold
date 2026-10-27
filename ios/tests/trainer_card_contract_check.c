#ifdef HG_TC_DIRECT_TASK_CHECK
// Actual SDK/game header types and task/helper bodies; only external effects are controlled.
#include "global.h"
#include "field_system.h"
#include "task.h"
#include "screen_fade.h"
#include "launch_application.h"
#include "field_overworld_restore.h"
#include "field_overworld_transition.h"
#include <stdio.h>
#include <stdlib.h>

#define CHECK(value) do {if(!(value)){fprintf(stderr,"Direct task check line %d: %s\n",__LINE__,#value);abort();}}while(0)
static FieldSystem field;
static FieldProcessManager process;
static BOOL fadeFinished;
static unsigned fades,launches,restores,resetWindows,eventCount;
static unsigned events[5];
static void event(unsigned value){CHECK(eventCount<5);events[eventCount++]=value;}
void HGNativeAssertFail(const char *condition,const char *file,int line){fprintf(stderr,"Original assertion %s %s:%d\n",condition,file,line);abort();}
void *Heap_AllocAtEnd(enum HeapID heap,u32 bytes){CHECK(heap==32);void *p=calloc(1,bytes);CHECK(p);return p;}
void Heap_Free(void *p){free(p);}
void BeginNormalPaletteFade(enum FadeMode mode,enum FadeType main,enum FadeType sub,u16 color,int steps,int frames,enum HeapID heap) {
    CHECK(mode==FADE_BOTH_SCREENS && color==RGB_BLACK && steps==6 && frames==1 && heap==HEAP_ID_FIELD1);
    CHECK(fades<2 && main==(fades ? FADE_TYPE_BRIGHTNESS_IN : FADE_TYPE_BRIGHTNESS_OUT) && sub==main);
    ++fades;fadeFinished=FALSE;event(fades==1 ? 1 : 4);
}
BOOL IsPaletteFadeFinished(void){return fadeFinished;}
void ResetVisibleHardwareWindows(PMLCDTarget screen){CHECK(screen==(resetWindows%2 ? PM_LCD_BOTTOM : PM_LCD_TOP));++resetWindows;}
void TrainerCardSignature_LaunchApp(FieldSystem *f) {
    CHECK(f==&field && fadeFinished && fades==1 && resetWindows==2 && !launches);
    ++launches;f->runningFieldMap=FALSE;process.child=(OverlayManager *)(uintptr_t)2;event(2);
}
void FieldSystem_LoadFieldOverlayInternal(FieldSystem *f) {
    CHECK(f==&field && !process.parent && !process.child && !restores);
    ++restores;process.parent=(OverlayManager *)(uintptr_t)3;process.isPaused=FALSE;field.runningFieldMap=FALSE;event(3);
}

#include "actual_direct_task_bodies.inc"

static BOOL tick(void){return FieldSystem_RunTaskFrame(&field);}
int main(void) {
    process.parent=(OverlayManager *)(uintptr_t)1;process.isPaused=FALSE;
    field.processManager=&process;field.runningFieldMap=TRUE;
    TaskManager *root=Task_New(&field,TCQADirectTask,NULL);field.taskman=root;
    CHECK(!tick() && fades==1 && resetWindows==2 && !launches && root->state==1 && field.taskman!=root);
    CHECK(!tick() && !launches && root->state==1); // Fade-out wait must block the parent.
    fadeFinished=TRUE;CHECK(!tick() && launches==1 && root->state==2 && field.taskman==root);
    CHECK(!tick() && !restores); // Both original applications are still live.
    process.child=NULL;CHECK(!tick() && !restores && root->state==2); // Parent-only must block.
    process.parent=NULL;process.child=(OverlayManager *)(uintptr_t)2;
    CHECK(!tick() && !restores && root->state==2); // Child-only must block.
    process.child=NULL;CHECK(!tick() && restores==1 && root->state==3 && field.taskman!=root && fades==1);
    CHECK(!tick() && fades==1 && root->state==3); // The real nested restore helper must block.
    field.runningFieldMap=TRUE;CHECK(!tick() && fades==2 && resetWindows==4 && root->state==4 && field.taskman!=root);
    CHECK(!tick() && field.taskman!=NULL); // Fade-in wait must block completion.
    fadeFinished=TRUE;CHECK(tick() && !field.taskman);event(5);
    CHECK(eventCount==5 && events[0]==1 && events[1]==2 && events[2]==3 && events[3]==4 && events[4]==5);
    CHECK(FieldSystem_IsFieldMapLoaded(&field) && FieldSystem_IsFieldMapRunning(&field) && !FieldSystem_TaskIsRunning(&field) && !FieldSystem_IsChildAppRunning(&field));
    puts("PASS actual direct task + original nested task scheduler/helpers: exact fade6/1/black, both parent/child waits, restore wait, fade-in wait, final loaded/running/no-task");
    return 0;
}
#else
// SPDX-License-Identifier: GPL-3.0-or-later
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
typedef uint8_t u8;typedef uint16_t u16;typedef uint32_t u32;typedef int BOOL;
enum {FALSE=0,TRUE=1};
#define GF_ASSERT assert
#include "constants/game_stats.h"
#include "trainer_card_qa.h"
#include "trainer_card_ledger.h"
typedef struct GameStats GameStats;
static void MI_CpuClear32(void *ptr,u32 size){memset(ptr,0,size);}
static u32 OS_GetVBlankCount(void){return 0x1234;}
#include "actual_math.inc"
#include "actual_game_stats.inc"
#include "actual_card.inc"
#include "actual_stroke_effect.inc"
#include "actual_input_callers.inc"
static void check_stroke_effects(void) {
    u8 before[6144],after[6144],previous[1536],packed[1536];memset(before,0x22,sizeof(before));memset(previous,0,sizeof(previous));
    for(u32 stroke=0;stroke<4;++stroke) {
        if(stroke>=2)memset(before,0x22,sizeof(before));
        memcpy(after,before,sizeof(after));
        u32 x=tcPoints[stroke][1][0]-32+1,y=tcPoints[stroke][1][1]-72+1;
        u32 offset=((y/8)*24+x/8)*32+(y%8)*4+(x%8)/2,shift=(x&1)*4;
        after[offset]=(after[offset]&~(15u<<shift))|(1u<<shift);
        assert(tcStrokeEffectValid(stroke,before,after,previous));
        assert(!tcStrokeEffectValid(stroke,before,before,previous)); // Drop this entire stroke, including replacement.
        u8 elsewhere[6144];memcpy(elsewhere,before,sizeof(elsewhere));elsewhere[0]=0x11;
        assert(!tcStrokeEffectValid(stroke,before,elsewhere,previous));
        TCQAPackCanvas(packed,after);
        if(stroke==3)assert(!tcStrokeEffectValid(stroke,before,after,packed)); // Saving the previous signature is not replacement.
        if(stroke==2)memcpy(previous,packed,sizeof(previous));
        memcpy(before,after,sizeof(before));
    }
    puts("PASS observed stroke effects: each original sample-specific region must gain ink; dropping any complete stroke/blank or previous replacement is rejected");
}
int main(void) {
    _Static_assert(sizeof(HGNativeTrainerCardUIState)==68 && sizeof(HGYNBootFacts)==44,"actual input/boot packet");
    _Static_assert(sizeof(HGNativeTrainerCardQAState)==12092 && offsetof(HGNativeTrainerCardQAState,ui)==172
        && offsetof(HGNativeTrainerCardQAState,boot)==240,"private observer packet ABI");
    _Static_assert(sizeof(struct SaveTrainerCard)==0x620 && offsetof(struct SaveTrainerCard,signaturePointData)==0x20,"actual save prefix");
    assert(sizeof(GameStats)==GameStats_sizeof() && sizeof(GameStats)<=HG_TC_QA_STATS_CAPACITY);
    GameStats live;GameStats_Init(&live);
    GameStats_SetCapped(&live,GAME_STAT_SCORE,99999998);GameStats_SetCapped(&live,GAME_STAT_TRAINER_CARDS_SIGNED,9998);
    GameStats before=live,expected=live;
    GameStats_AddScore(&expected,SCORE_EVENT_TRAINER_CARD_SIGNED);GameStats_Inc(&expected,GAME_STAT_TRAINER_CARDS_SIGNED);
    assert(!memcmp(&live,&before,sizeof(live)) && memcmp(&expected,&before,sizeof(live)));
    assert(GameStats_GetScore(&expected)==99999999 && GameStats_GetCapped(&expected,GAME_STAT_TRAINER_CARDS_SIGNED)==9999);
    for(u32 i=0;i<3;++i) {
        GameStats_AddScore(&live,SCORE_EVENT_TRAINER_CARD_SIGNED);GameStats_Inc(&live,GAME_STAT_TRAINER_CARDS_SIGNED);
        assert(!memcmp(&live,&expected,sizeof(live)));
        GameStats_AddScore(&expected,SCORE_EVENT_TRAINER_CARD_SIGNED);GameStats_Inc(&expected,GAME_STAT_TRAINER_CARDS_SIGNED);
    }
    u8 canvas[6144],reference[1536],actual[1536];memset(canvas,2,sizeof(canvas));
    for(u32 pass=0;pass<4;++pass) {
        if(pass)for(u32 i=0;i<sizeof(canvas);i+=pass*7)canvas[i]=(i%3==0 ? 0x11 : i%3==1 ? 0x21 : 0x12);
        TCQAPackCanvas(reference,canvas);TrainerCardSignature_PackCanvas(actual,canvas);assert(!memcmp(reference,actual,1536));
        struct SaveTrainerCard card;memset(&card,0,sizeof(card));memcpy(card.signaturePointData,actual,1536);
        assert(TCQASignatureExists(actual)==TrainerCard_SignatureExists(&card));
    }
    memset(actual,0,sizeof(actual));u32 words[2]={1,UINT32_MAX};memcpy(actual,words,sizeof(words));
    struct SaveTrainerCard cancellation;memset(&cancellation,0,sizeof(cancellation));memcpy(cancellation.signaturePointData,actual,1536);
    assert(actual[0] && !TCQASignatureExists(actual) && !TrainerCard_SignatureExists(&cancellation));
    TCQATouchPoint trace[6];for(u32 i=0;i<6;++i)trace[i]=(TCQATouchPoint){7,9,32+i,72+i,i<5,10+i*2};
    assert(TCQAStrokeTraceValid(trace,6));trace[2].revision=8;assert(!TCQAStrokeTraceValid(trace,6));trace[2].revision=9;
    trace[3].padReads=trace[2].padReads+1;assert(!TCQAStrokeTraceValid(trace,6));trace[3].padReads=16;
    trace[5].down=1;assert(!TCQAStrokeTraceValid(trace,6));
    assert(TCQANext(UINT32_MAX)==1 && TCQANext(0)==1);
    check_actual_input_callers();check_stroke_effects();
    printf("PASS: actual encrypted GameStats clone isolation/caps, original packing/signature sum, finite continuous strokes; QA bytes=%zu\n",sizeof(HGNativeTrainerCardQAState));
    printf("TCQA_LAYOUT {\"bytes\":%zu,\"canvas\":%zu,\"saveCard\":%zu,\"expectedPayload\":%zu,\"statsLive\":%zu,\"statsExpected\":%zu,\"statsBytes\":%zu,\"canvasBytes\":%zu,\"confirms\":%zu,\"editorExits\":%zu,\"ledgerFlags\":%zu,\"statsTypeBytes\":%zu,\"started\":%zu,\"canvasSerial\":%zu,\"canvasGeneration\":%zu,\"fieldReady\":%zu,\"ui\":%zu}\n",
        sizeof(HGNativeTrainerCardQAState),offsetof(HGNativeTrainerCardQAState,canvas),offsetof(HGNativeTrainerCardQAState,saveCard),
        offsetof(HGNativeTrainerCardQAState,expectedPayload),offsetof(HGNativeTrainerCardQAState,statsLive),offsetof(HGNativeTrainerCardQAState,statsExpected),
        offsetof(HGNativeTrainerCardQAState,statsBytes),offsetof(HGNativeTrainerCardQAState,canvasBytes),offsetof(HGNativeTrainerCardQAState,confirms),
        offsetof(HGNativeTrainerCardQAState,editorExits),offsetof(HGNativeTrainerCardQAState,partyUnchanged),sizeof(GameStats),offsetof(HGNativeTrainerCardQAState,started),
        offsetof(HGNativeTrainerCardQAState,canvasSerial),offsetof(HGNativeTrainerCardQAState,canvasGeneration),offsetof(HGNativeTrainerCardQAState,fieldReady),offsetof(HGNativeTrainerCardQAState,ui));
}
#endif
