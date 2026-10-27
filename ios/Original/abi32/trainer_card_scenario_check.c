// SPDX-License-Identifier: GPL-3.0-or-later
#define main TCQABootstrapMain
#define HGEngineTrap TCQABootstrapTrap
#include "engine_check.c"
#undef HGEngineTrap
#undef main
#include "trainer_card_qa.h"
#include "trainer_card_ledger.h"
#include "native_field_menu_ui.h"
#include <nitro/os/native_reset.h>
#include <setjmp.h>
// Private calls below must come from the future generated hgengine.h. No hand-written ABI prototypes.
enum { TC_BOOT,TC_START,TC_OPEN,TC_VIEW,TC_KEY,TC_PROMPT,TC_STROKE,TC_CANVAS,TC_FIELD,TC_DIRECT,TC_FINISH };
typedef struct TCStep {u32 op,a,b;} TCStep;
static const TCStep tcSteps[]={
    {TC_BOOT},{TC_START},{TC_OPEN},{TC_VIEW,0,0},{TC_KEY,16},{TC_VIEW,1,0},
    {TC_KEY,32},{TC_VIEW,0,0},{TC_KEY,16},{TC_VIEW,1,0},
    {TC_KEY,1},{TC_PROMPT,2,0},{TC_VIEW,1,0},
    {TC_KEY,1},{TC_PROMPT,2,1},{TC_CANVAS,0},{TC_STROKE,0},{TC_STROKE,1},{TC_CANVAS,1},
    {TC_KEY,0},{TC_PROMPT,3,0},{TC_PROMPT,6,0},{TC_CANVAS,2},
    {TC_KEY,0},{TC_PROMPT,3,0},{TC_PROMPT,6,1},{TC_CANVAS,0},{TC_STROKE,2},
    {TC_KEY,0},{TC_PROMPT,3,1},{TC_VIEW,1,1},
    {TC_KEY,1},{TC_PROMPT,2,1},{TC_PROMPT,4,0},{TC_VIEW,1,1},
    {TC_KEY,1},{TC_PROMPT,2,1},{TC_PROMPT,4,1},{TC_CANVAS,0},{TC_STROKE,3},
    {TC_KEY,0},{TC_PROMPT,3,1},{TC_VIEW,1,2},{TC_KEY,2},{TC_FIELD,2},
    {TC_OPEN},{TC_VIEW,0,2},{TC_KEY,2},{TC_FIELD,2},{TC_DIRECT},{TC_CANVAS,0},
    {TC_KEY,0},{TC_PROMPT,3,1},{TC_FIELD,3},{TC_FINISH}
};
static const u16 tcPoints[4][5][2]={
    {{40,80},{60,84},{80,88},{100,92},{120,96}},
    {{48,120},{72,112},{96,104},{120,96},{144,88}},
    {{64,80},{80,88},{96,96},{112,104},{128,112}},
    {{160,80},{144,88},{128,96},{112,104},{96,112}}
};
static Capture *tcCapture;
static jmp_buf tcResetLanding;
static BOOL tcResetExpected,tcPassed;
static const char *tcStatus="not_started";
static HGNativeTrainerCardQAState tcFinal;
static HGNativeFieldMenuUIState tcMenu,tcMenuBefore;
static u32 tcMenuCloseStage,tcMenuCloseRequests,tcMenuCloses,tcDirectFieldWitnesses;
_Static_assert(sizeof(HGNativeFieldMenuUIState)==36,"public field menu witness ABI");
static u32 tcStep,tcStepFrame,tcLease,tcPressReads,tcReleaseReads,tcKey,tcX,tcY,tcTouch,tcPoint,tcStroke;
static u32 tcLeaseGeneration,tcLeaseRevision,tcStaleRejected,tcSnapshots,tcTitleSent,tcSwitchSent,tcContinueSent,tcContinueGeneration;
static u32 tcTitleGeneration,tcTitleExit,tcSwitchGeneration,tcSwitchRevision,tcModeSwitchSeen,tcContinueExit;
static u32 tcPendingRequest,tcRequestedCheckpoint;
static u32 tcInitialScore,tcInitialChecksum,tcInitialWitness,tcFreshOpen,tcLastSnapshotStep=UINT32_MAX;
static HGNativeTrainerCardUITouchLease tcTouchLease;
static TCQATouchPoint tcStrokeTrace[6];
static u32 tcStrokeTraceCount;
static u8 tcRetainedCanvas[6144];
static u8 tcStrokeBeforeCanvas[6144],tcStrokePreviousPayload[1536];
static u32 tcStrokeBeforeSerial,tcStrokeBeforeObserved,tcStrokeWaiting,tcStrokeEffects;
// Small witness regions surround the second source sample of each predefined stroke, in original canvas coordinates.
static const u32 tcStrokeRegions[4][4]={{26,10,8,8},{38,38,8,8},{46,14,8,8},{110,14,8,8}};
static u32 tcStrokeNewInk(u32 stroke,const u8 *before,const u8 *after) {
    if(stroke>=4 || !before || !after)return 0;
    const u32 *region=tcStrokeRegions[stroke];u32 count=0;
    for(u32 y=region[1];y<region[1]+region[3];++y)for(u32 x=region[0];x<region[0]+region[2];++x) {
        u32 offset=((y/8)*24+x/8)*32+(y%8)*4+(x%8)/2,shift=(x&1)*4;
        if(((after[offset]>>shift)&15)==1 && ((before[offset]>>shift)&15)!=1)++count;
    }
    return count;
}
static BOOL tcStrokeEffectValid(u32 stroke,const u8 *before,const u8 *after,const u8 *previousPayload) {
    if(stroke>=4 || !before || !after || !memcmp(before,after,6144) || !tcStrokeNewInk(stroke,before,after))return FALSE;
    u8 packed[1536];TCQAPackCanvas(packed,after);
    return TCQASignatureExists(packed) && (stroke!=3 || (previousPayload && TCQASignatureExists(previousPayload) && memcmp(packed,previousPayload,1536)));
}
void HGEngineTrap(wasm_rt_trap_t code) {
    if (code==WASM_RT_TRAP_UNREACHABLE && tcResetExpected && tcCapture && tcCapture->env->resetRequested)longjmp(tcResetLanding,1);
    TCQABootstrapTrap(code);
}
static void tcFinish(const char *status,BOOL passed) {
    tcStatus=status;tcPassed=passed;
    if (tcCapture && tcCapture->env && tcCapture->env->memory) {
        HGNativeSetInput(tcCapture->env,0,FALSE,0,0,FALSE);tcResetExpected=TRUE;
        w2c_env_HGNativeResetControl(tcCapture->env,HG_RESET_REQUEST,0);
    }
    fprintf(stderr,"Trainer scenario failed before reset: %s\n",status);_Exit(1);
}
#define TC_CHECK(value) do {if (!(value)){fprintf(stderr,"Trainer line %d step %u: %s\n",__LINE__,tcStep,#value);tcFinish("assertion_failed",FALSE);}}while(0)
static const void *tcGuest(u32 address,size_t size) {
    TC_CHECK(address && address<=tcCapture->env->memory->size && size<=tcCapture->env->memory->size-address);
    return tcCapture->env->memory->data+address;
}
static HGNativeFieldMenuUIState tcReadMenu(void) {
    u32 address=w2c_hgengine_HGNativeFieldMenuUIGetState(tcCapture->game);HGNativeFieldMenuUIState menu;
    memcpy(&menu,tcGuest(address,sizeof(menu)),sizeof(menu));
    TC_CHECK(w2c_hgengine_HGNativeFieldMenuUIGetState(tcCapture->game)==address && !memcmp(&menu,tcGuest(address,sizeof(menu)),sizeof(menu)));
    return menu;
}
static HGNativeTrainerCardQAState tcRead(void) {
    u32 address=w2c_hgengine_HGNativeTrainerCardQAGetState(tcCapture->game);
    HGNativeTrainerCardQAState qa;memcpy(&qa,tcGuest(address,sizeof(qa)),sizeof(qa));
    TC_CHECK(qa.version==HG_TC_QA_VERSION && qa.byteSize==sizeof(qa) && !qa.error);
    u32 ui=w2c_hgengine_HGNativeTrainerCardUIGetState(tcCapture->game);
    memcpy(&qa.ui,tcGuest(ui,sizeof(qa.ui)),sizeof(qa.ui));tcMenu=tcReadMenu();return qa;
}
static void tcMenuTrace(const char *event) {
    printf("TRAINER menu: {\"event\":\"%s\",\"step\":%u,\"padReads\":%u,\"frame\":%u,\"snapshot\":%u,\"fieldReady\":%u,\"trainerActive\":%u,\"closeCount\":%u,\"admittedGeneration\":%u,\"admittedRevision\":%u,\"menu\":{\"active\":%u,\"generation\":%u,\"revision\":%u,\"ready\":%u,\"phase\":%u,\"viewActive\":%u,\"controlActive\":%u,\"childActive\":%u,\"inputMode\":%u}}\n",
        event,tcStep,HGNativeInputPadReads(tcCapture->env),OS_GetVBlankCount(),tcSnapshots-1,tcFinal.fieldReady,tcFinal.ui.active,tcMenuCloses,tcMenuBefore.ownerGeneration,tcMenuBefore.revision,
        tcMenu.active,tcMenu.ownerGeneration,tcMenu.revision,tcMenu.ready,tcMenu.phase,tcMenu.viewActive,tcMenu.controlActive,tcMenu.childActive,tcMenu.inputMode);
}
static void tcTrace(const char *event) {
    printf("TRAINER input: {\"event\":\"%s\",\"step\":%u,\"op\":%u,\"key\":%u,\"touch\":%u,\"x\":%u,\"y\":%u,\"point\":%u,\"stroke\":%u,\"padReads\":%u,\"frame\":%u,\"generation\":%u,\"revision\":%u,"
        "\"bootKind\":%u,\"bootActive\":%u,\"bootGeneration\":%u,\"bootRevision\":%u,\"bootReady\":%u,\"bootPhase\":%u,\"bootMode\":%u,\"bootOption\":%u,\"bootNotice\":%u,\"bootExit\":%u,\"bootProgress\":%u}\n",
        event,tcStep,tcSteps[tcStep].op,tcKey,tcTouch,tcX,tcY,tcPoint,tcStroke,HGNativeInputPadReads(tcCapture->env),OS_GetVBlankCount(),tcLeaseGeneration,tcLeaseRevision,
        tcFinal.boot.kind,tcFinal.boot.active,tcFinal.boot.ownerGeneration,tcFinal.boot.revision,tcFinal.boot.ready,tcFinal.boot.phase,tcFinal.boot.inputMode,
        tcFinal.boot.option,tcFinal.boot.notice,tcFinal.boot.exit,tcFinal.boot.progressFlags);
}
static void tcRawUp(void) {
    if(tcSteps[tcStep].op==TC_STROKE) {
        TC_CHECK(tcStrokeTraceCount==5);
        tcStrokeTrace[tcStrokeTraceCount++]=(TCQATouchPoint){tcLeaseGeneration,tcLeaseRevision,tcX,tcY,0,HGNativeInputPadReads(tcCapture->env)};
        TC_CHECK(TCQAStrokeTraceValid(tcStrokeTrace,tcStrokeTraceCount));
    }
    TC_CHECK(HGNativeSetInput(tcCapture->env,0,FALSE,0,0,FALSE));tcKey=tcTouch=0;
    tcTouchLease=(HGNativeTrainerCardUITouchLease){0};tcReleaseReads=HGNativeInputPadReads(tcCapture->env);tcLease=2;tcTrace("released");
    if(tcMenuCloseStage==1)tcMenuTrace("released");
}
static void tcPulse(u32 key,u32 x,u32 y,BOOL touch,BOOL cardOwner) {
    TC_CHECK(!tcLease);tcKey=key;tcX=x;tcY=y;tcTouch=touch;tcPoint=0;
    tcLeaseGeneration=cardOwner ? tcFinal.ui.ownerGeneration : 0;tcLeaseRevision=cardOwner ? tcFinal.ui.revision : 0;
    if (cardOwner && touch)TC_CHECK(HGNativeTrainerCardUIResolveTouch(&tcTouchLease,&tcFinal.ui,tcLeaseGeneration,tcLeaseRevision,HG_TRAINER_CARD_TOUCH_DOWN,x,y));
    TC_CHECK(HGNativeSetInput(tcCapture->env,key,FALSE,x,y,touch));tcPressReads=HGNativeInputPadReads(tcCapture->env);tcLease=1;tcTrace("press");
    if(tcSteps[tcStep].op==TC_STROKE) {tcStrokeTraceCount=1;tcStrokeTrace[0]=(TCQATouchPoint){tcLeaseGeneration,tcLeaseRevision,x,y,1,tcPressReads};}
}
static BOOL tcSettle(void) {
    if (!tcLease)return TRUE;
    u32 reads=HGNativeInputPadReads(tcCapture->env);
    if (tcLease==1) {
        if(tcMenuCloseStage==1 && !HGNativeFieldMenuUIInputAllowed(&tcMenu,tcMenuBefore.ownerGeneration,tcMenuBefore.revision)){tcRawUp();return FALSE;}
        if (tcLeaseGeneration && !HGNativeTrainerCardUIInputAllowed(&tcFinal.ui,tcLeaseGeneration,tcLeaseRevision)) {tcRawUp();return FALSE;}
        if (reads-tcPressReads>=2) {
            if (tcSteps[tcStep].op==TC_STROKE && tcPoint<4) {
                ++tcPoint;tcX=tcPoints[tcStroke][tcPoint][0];tcY=tcPoints[tcStroke][tcPoint][1];
                TC_CHECK(HGNativeTrainerCardUIResolveTouch(&tcTouchLease,&tcFinal.ui,tcLeaseGeneration,tcLeaseRevision,HG_TRAINER_CARD_TOUCH_MOVE,tcX,tcY));
                TC_CHECK(HGNativeSetInput(tcCapture->env,0,FALSE,tcX,tcY,TRUE));tcPressReads=reads;
                TC_CHECK(tcStrokeTraceCount<5);tcStrokeTrace[tcStrokeTraceCount++]=(TCQATouchPoint){tcLeaseGeneration,tcLeaseRevision,tcX,tcY,1,reads};
                tcTrace("move");return FALSE;
            }
            tcRawUp();return FALSE;
        }
    }
    if (tcLease!=2 || reads-tcReleaseReads<2)return FALSE;
    tcTrace("settled");if(tcMenuCloseStage==1)tcMenuTrace("settled");tcLease=0;return TRUE;
}
static void tcNext(void) {++tcStep;tcStepFrame=OS_GetVBlankCount();tcMenuCloseStage=0;tcMenuBefore=(HGNativeFieldMenuUIState){0};}
static void tcConservation(void) {
    TC_CHECK(tcFinal.started && tcFinal.partyUnchanged && tcFinal.pcUnchanged && tcFinal.bagUnchanged
        && tcFinal.varsFlagsUnchanged && tcFinal.easyChatUnchanged && tcFinal.moneyUnchanged
        && tcFinal.prefixUnchanged && tcFinal.statsMatched && tcFinal.saveMatched
        && tcFinal.statsBytes && tcFinal.statsBytes<=HG_TC_QA_STATS_CAPACITY
        && !memcmp(tcFinal.statsLive,tcFinal.statsExpected,tcFinal.statsBytes));
    u32 score=tcFinal.baselineScore;
    if(tcFinal.confirms)score=score>=99999999 || tcFinal.confirms>99999999-score ? 99999999 : score+tcFinal.confirms;
    u32 signedCards=tcFinal.baselineSigned;
    TC_CHECK(signedCards<=9999);signedCards=tcFinal.confirms>9999-signedCards ? 9999 : signedCards+tcFinal.confirms;
    TC_CHECK(tcFinal.currentScore==score && tcFinal.currentSigned==signedCards);
}
static BOOL tcSnapshot(void) {
    if (tcLastSnapshotStep==tcStep && tcSteps[tcStep].op!=TC_BOOT)return TRUE;
    static u8 pixels[2][256*192*4];u32 rendered=0,varied=0;
    if (!tcCapture->buffer)tcCapture->buffer=w2c_hgengine_HGNativeFrameBuffer(tcCapture->game);
    HGNativeDrainDMA();tcGuest(tcCapture->buffer,sizeof(pixels[0]));
    for(u32 engine=0;engine<2;++engine)if(w2c_hgengine_HGNativeRender2D(tcCapture->game,engine,tcCapture->buffer,sizeof(pixels[0]))) {
        rendered|=1u<<engine;memcpy(pixels[engine],tcGuest(tcCapture->buffer,sizeof(pixels[0])),sizeof(pixels[0]));
        for(u32 i=1;i<256*192;++i)if(memcmp(pixels[engine],pixels[engine]+i*4,4)){varied|=1u<<engine;break;}
    }
    u32 required=tcFinal.ui.active || tcMenu.active ? 3 : 1;if((rendered&varied&required)!=required)return FALSE;
    HGNativeFieldMenuUIState after=tcReadMenu();if(memcmp(&tcMenu,&after,sizeof(after)))return FALSE;
    char path[4096];TC_CHECK(snprintf(path,sizeof(path),"%s/trainer-%03u-state.bin",tcCapture->folder,tcSnapshots)<sizeof(path));
    FILE *state=fopen(path,"wbx");TC_CHECK(state && fwrite(&tcFinal,1,sizeof(tcFinal),state)==sizeof(tcFinal) && !fclose(state));
    TC_CHECK(snprintf(path,sizeof(path),"%s/trainer-%03u-menu.bin",tcCapture->folder,tcSnapshots)<sizeof(path));
    FILE *menu=fopen(path,"wbx");TC_CHECK(menu && fwrite(&tcMenu,1,sizeof(tcMenu),menu)==sizeof(tcMenu) && !fclose(menu));
    for(u32 engine=0;engine<2;++engine)if(rendered&(1u<<engine)) {
        TC_CHECK(snprintf(path,sizeof(path),"%s/trainer-%03u-engine-%u.ppm",tcCapture->folder,tcSnapshots,engine)<sizeof(path));
        FILE *image=fopen(path,"wbx");TC_CHECK(image && fprintf(image,"P6\n256 192\n255\n")>0);
        for(u32 row=0;row<192;++row){u8 rgb[256*3];for(u32 x=0;x<256;++x)memcpy(rgb+x*3,pixels[engine]+(row*256+x)*4,3);TC_CHECK(fwrite(rgb,1,sizeof(rgb),image)==sizeof(rgb));}
        TC_CHECK(!fclose(image));
    }
    printf("TRAINER snapshot: {\"sequence\":%u,\"step\":%u,\"requiredMask\":%u,\"renderedMask\":%u,\"variedMask\":%u,\"confirms\":%u,\"canvasSerial\":%u,\"frame\":%u,\"packetFrame\":%u,\"generation\":%u,\"revision\":%u,\"canvasGeneration\":%u,\"viewerGeneration\":%u}\n",
        tcSnapshots,tcStep,required,rendered,varied,tcFinal.confirms,tcFinal.canvasSerial,OS_GetVBlankCount(),tcFinal.frame,
        tcFinal.ui.ownerGeneration,tcFinal.ui.revision,tcFinal.canvasGeneration,tcFinal.viewerGeneration);
    tcLastSnapshotStep=tcStep;++tcSnapshots;return TRUE;
}
static void tcRequest(u32 kind) {
    TC_CHECK(!tcPendingRequest && tcFinal.revision);
    TC_CHECK(!w2c_hgengine_HGNativeTrainerCardQARequest(tcCapture->game,tcFinal.revision-1,kind));
    ++tcStaleRejected;
    TC_CHECK(w2c_hgengine_HGNativeTrainerCardQARequest(tcCapture->game,tcFinal.revision,kind));
    TC_CHECK(!w2c_hgengine_HGNativeTrainerCardQARequest(tcCapture->game,tcFinal.revision,kind));
    tcPendingRequest=kind;
}
static BOOL tcMenuCanClose(const HGNativeFieldMenuUIState *menu,u32 step,u32 trainerActive) {
    return !trainerActive && (step==44 || step==48) && menu && menu->inputMode==0
        && HGNativeFieldMenuUIInputAllowed(menu,menu->ownerGeneration,menu->revision);
}
static BOOL tcMenuRetired(const HGNativeFieldMenuUIState *menu,u32 generation,u32 revision) {
    return menu && generation && revision && menu->ownerGeneration==generation && menu->revision && menu->revision!=revision && menu->phase==UINT32_MAX
        && !menu->active && !menu->ready && !menu->viewActive && !menu->controlActive && !menu->childActive;
}
static BOOL tcBootEligible(const HGYNBootFacts *b) {
    if(!b || b->active!=1 || b->ready!=1 || !b->ownerGeneration || !b->revision)return FALSE;
    if(b->kind==HG_YN_BOOT_TITLE)return b->phase==2 && !b->inputMode && !b->option && !b->notice && !b->exit && (b->progressFlags&15)==15;
    return b->kind==HG_YN_BOOT_MAIN_MENU && b->phase==5 && b->inputMode<=1 && b->option<9 && b->notice==15 && !b->exit && (b->progressFlags&255)==255;
}
static BOOL tcBootSameSwitchedOwner(const HGYNBootFacts *b,u32 generation,u32 revision) {
    return tcBootEligible(b) && b->kind==HG_YN_BOOT_MAIN_MENU && b->inputMode==0 && generation && revision
        && b->ownerGeneration==generation && b->revision!=revision;
}
static BOOL tcBootExitWitness(const HGYNBootFacts *b,u32 generation) {
    return b && b->kind==HG_YN_BOOT_MAIN_MENU && b->active==0 && b->ready==0 && b->phase==8
        && generation && b->ownerGeneration==generation && b->revision && b->exit==1;
}
static BOOL tcBootTitleExitWitness(const HGYNBootFacts *b,u32 generation) {
    return b && b->kind==HG_YN_BOOT_TITLE && b->active==0 && b->ready==0 && b->phase==6
        && generation && b->ownerGeneration==generation && b->revision && b->exit==1;
}
static void TCQAPresent(void *argument) {
    tcCapture=argument;tcFinal=tcRead();u32 frame=OS_GetVBlankCount();
    if(!tcStepFrame)tcStepFrame=frame;if(frame-tcStepFrame>(tcStep==0 ? 12000u : 2400u))tcFinish("stage_timeout",FALSE);
    if(tcTitleSent && tcBootTitleExitWitness(&tcFinal.boot,tcTitleGeneration))tcTitleExit=1;
    if(tcContinueSent && tcBootExitWitness(&tcFinal.boot,tcContinueGeneration))tcContinueExit=1;
    if(tcSwitchSent && tcBootSameSwitchedOwner(&tcFinal.boot,tcSwitchGeneration,tcSwitchRevision))tcModeSwitchSeen=1;
    BOOL settling=tcLease!=0;if(!tcSettle())return;if(settling && tcSteps[tcStep].op!=TC_BOOT){if(tcSteps[tcStep].op==TC_STROKE)tcStrokeWaiting=1;else if(tcSteps[tcStep].op==TC_FIELD){TC_CHECK(tcMenuCloseStage==1);tcMenuCloseStage=2;}else {tcNext();return;}}
    TC_CHECK(tcStep<sizeof(tcSteps)/sizeof(tcSteps[0]));TCStep step=tcSteps[tcStep];
    if(step.op==TC_BOOT) {
        HGYNBootFacts *b=&tcFinal.boot;
        if(tcFinal.fieldReady){TC_CHECK(tcContinueSent && tcContinueExit && tcBootExitWitness(b,tcContinueGeneration) && (!tcTitleSent || tcTitleExit));if(!tcSnapshot())return;tcNext();return;}
        if(!tcBootEligible(b))return;
        if(b->kind==HG_YN_BOOT_TITLE){if(tcTitleSent)return;}
        else if(b->inputMode){if(tcSwitchSent || tcContinueSent)return;}
        else {if(tcContinueSent)return;TC_CHECK(!tcSwitchSent || (tcModeSwitchSeen && tcBootSameSwitchedOwner(b,tcSwitchGeneration,tcSwitchRevision)));}
        if(!tcSnapshot())return;
        if(b->kind==HG_YN_BOOT_TITLE){tcTitleSent=1;tcTitleGeneration=b->ownerGeneration;}
        else if(b->inputMode){tcSwitchSent=1;tcSwitchGeneration=b->ownerGeneration;tcSwitchRevision=b->revision;}
        else {tcContinueSent=1;tcContinueGeneration=b->ownerGeneration;}
        // Boot actions stay on this step until the original terminal witness reaches field.
        tcPulse(1,0,0,FALSE,FALSE);return;
    }
    if(tcFinal.started)tcConservation();
    if(step.op==TC_START){if(!tcFinal.fieldReady || tcFinal.trainerSlot>=7)return;if(!tcPendingRequest){TC_CHECK(tcFinal.compatibleSave);tcRequest(HG_TC_QA_START);return;}TC_CHECK(tcPendingRequest==HG_TC_QA_START);if(!tcFinal.started)return;tcPendingRequest=0;tcNext();return;}
    if(step.op==TC_OPEN){if(!tcFinal.started || !tcFinal.fieldReady || tcFinal.trainerSlot>=7)return;TC_CHECK(((tcFinal.menuIcons>>(tcFinal.trainerSlot*4))&15)==4);if(tcFinal.confirms==2)tcFreshOpen=1;if(!tcSnapshot())return;tcPulse(0,tcFinal.trainerX,tcFinal.trainerY,TRUE,FALSE);return;}
    if(step.op==TC_FIELD){
        if(tcFinal.ui.active)return;
        if(tcMenu.active) {
            TC_CHECK(tcStep==44 || tcStep==48);
            if(!tcMenuCloseStage && tcMenuCanClose(&tcMenu,tcStep,tcFinal.ui.active)) {
                if(!tcSnapshot())return;tcMenuBefore=tcMenu;u32 key=0,sentinel=123;
                TC_CHECK(!HGNativeFieldMenuUIResolveKey(&tcMenu,tcMenu.ownerGeneration,tcMenu.revision-1,2,&sentinel) && sentinel==123);++tcStaleRejected;
                TC_CHECK(HGNativeFieldMenuUIResolveKey(&tcMenu,tcMenu.ownerGeneration,tcMenu.revision,2,&key) && key==2);
                tcMenuCloseStage=1;++tcMenuCloseRequests;tcMenuTrace("admitted");tcPulse(key,0,0,FALSE,FALSE);
            }
            return;
        }
        if(tcMenuCloseStage && !tcMenuRetired(&tcMenu,tcMenuBefore.ownerGeneration,tcMenuBefore.revision))return;
        if(!tcFinal.fieldReady)return;
        if(tcMenuCloseStage==2) {
            tcLastSnapshotStep=UINT32_MAX;if(!tcSnapshot())return;++tcMenuCloses;tcMenuCloseStage=3;tcMenuTrace("closed_field_ready");
        }
        TC_CHECK(tcFinal.confirms==step.a);
        if(step.a==3){TC_CHECK(tcMenuCloseStage==0);for(u32 i=32;i<sizeof(tcFinal.saveCard);++i)TC_CHECK(!tcFinal.saveCard[i]);}
        if(!tcPendingRequest){if(!tcSnapshot())return;if(step.a==3){++tcDirectFieldWitnesses;tcMenuTrace("direct_field_ready");}tcRequestedCheckpoint=tcFinal.checkpoints+1;tcRequest(HG_TC_QA_CHECKPOINT);return;}
        TC_CHECK(tcPendingRequest==HG_TC_QA_CHECKPOINT);if(tcFinal.checkpoints<tcRequestedCheckpoint)return;TC_CHECK(tcFinal.checkpoints==tcRequestedCheckpoint);tcPendingRequest=0;tcNext();return;
    }
    if(step.op==TC_DIRECT){if(!tcPendingRequest){if(!tcFinal.fieldReady)return;tcRequest(HG_TC_QA_DIRECT);return;}TC_CHECK(tcPendingRequest==HG_TC_QA_DIRECT);if(!tcFinal.directLaunches)return;tcPendingRequest=0;tcNext();return;}
    if(step.op==TC_FINISH){if(tcFinal.stage!=HG_TC_QA_DONE){if(tcPendingRequest)return;TC_CHECK(tcFinal.fieldReady && tcFinal.directComplete && tcFinal.confirms==3 && !TCQASignatureExists(tcFinal.saveCard+32));tcRequest(HG_TC_QA_FINISH);return;}TC_CHECK(tcPendingRequest==HG_TC_QA_FINISH);tcPendingRequest=0;tcFinish("passed_original_trainer_card",TRUE);}
    HGNativeTrainerCardUIState *ui=&tcFinal.ui;if(!HGNativeTrainerCardUIInputAllowed(ui,ui->ownerGeneration,ui->revision))return;
    if(step.op==TC_VIEW){if(ui->view!=HG_TRAINER_CARD_VIEWER || ui->phase!=1 || ui->showingBack!=step.a || tcFinal.viewerGeneration!=ui->ownerGeneration)return;TC_CHECK(tcFinal.confirms==step.b && tcFinal.cachedSignatureMatched);if(!tcInitialWitness){tcInitialWitness=1;tcInitialScore=tcFinal.cachedScore;tcInitialChecksum=tcFinal.cachedChecksum;TC_CHECK(tcInitialScore==tcFinal.baselineScore);}if(!tcFreshOpen)TC_CHECK(tcFinal.cachedScore==tcInitialScore && tcFinal.cachedChecksum==tcInitialChecksum);else TC_CHECK(tcFinal.cachedScore==tcFinal.currentScore);if(!tcSnapshot())return;tcNext();return;}
    if(step.op==TC_CANVAS){if(ui->view!=HG_TRAINER_CARD_SIGNATURE || ui->subState!=1 || tcFinal.canvasGeneration!=ui->ownerGeneration)return;TC_CHECK(tcFinal.canvasBytes==6144);u8 packed[1536];TCQAPackCanvas(packed,tcFinal.canvas);if(step.a==0){for(u32 i=0;i<6144;++i)TC_CHECK(tcFinal.canvas[i]==0x22);for(u32 i=0;i<1536;++i)TC_CHECK(!packed[i]);}else if(step.a==1){TC_CHECK(TCQASignatureExists(packed));memcpy(tcRetainedCanvas,tcFinal.canvas,6144);}else TC_CHECK(!memcmp(tcRetainedCanvas,tcFinal.canvas,6144));if(!tcSnapshot())return;tcNext();return;}
    if(step.op==TC_STROKE){
        if(ui->view!=HG_TRAINER_CARD_SIGNATURE || ui->subState!=1 || tcFinal.canvasGeneration!=ui->ownerGeneration)return;
        TC_CHECK(step.a<4 && tcFinal.canvasBytes==6144);
        if(tcStrokeWaiting) {
            TC_CHECK(tcStroke==step.a && tcFinal.canvasGeneration==tcLeaseGeneration);
            if(tcFinal.snapshotSerial==tcStrokeBeforeObserved || tcFinal.canvasSerial==tcStrokeBeforeSerial)return;
            TC_CHECK(tcStrokeEffectValid(tcStroke,tcStrokeBeforeCanvas,tcFinal.canvas,tcStrokePreviousPayload));
            tcLastSnapshotStep=UINT32_MAX;if(!tcSnapshot())return;
            printf("TRAINER stroke: {\"stroke\":%u,\"step\":%u,\"generation\":%u,\"beforeSerial\":%u,\"afterSerial\":%u,\"newInkPixels\":%u,\"nonblank\":1,\"differentFromPrevious\":1,\"snapshot\":%u}\n",
                tcStroke,tcStep,tcFinal.canvasGeneration,tcStrokeBeforeSerial,tcFinal.canvasSerial,tcStrokeNewInk(tcStroke,tcStrokeBeforeCanvas,tcFinal.canvas),tcSnapshots-1);
            ++tcStrokeEffects;tcStrokeWaiting=0;tcNext();return;
        }
        tcStroke=step.a;tcStrokeBeforeSerial=tcFinal.canvasSerial;tcStrokeBeforeObserved=tcFinal.snapshotSerial;
        memcpy(tcStrokeBeforeCanvas,tcFinal.canvas,6144);memcpy(tcStrokePreviousPayload,tcFinal.saveCard+32,1536);
        if(!tcSnapshot())return;tcPulse(0,tcPoints[tcStroke][0][0],tcPoints[tcStroke][0][1],TRUE,TRUE);return;
    }
    if(step.op==TC_PROMPT){if(!HGNativeTrainerCardUIIsPrompt(ui) || ui->subState!=step.a)return;if(!tcSnapshot())return;u32 x=ui->promptLeft+ui->promptWidth/2,y=ui->promptTop+ui->promptButtonHeight/2+(step.b ? 0 : ui->promptButtonHeight);tcPulse(0,x,y,TRUE,TRUE);return;}
    if(step.op==TC_KEY){if(!tcSnapshot())return;if(!step.a){if(ui->view!=HG_TRAINER_CARD_SIGNATURE || ui->subState!=1)return;tcPulse(0,224,175,TRUE,TRUE);}else {u32 key=0,sentinel=123;TC_CHECK(!HGNativeTrainerCardUIResolveKey(ui,ui->ownerGeneration,ui->revision-1,step.a,&sentinel)&&sentinel==123);++tcStaleRejected;TC_CHECK(HGNativeTrainerCardUIResolveKey(ui,ui->ownerGeneration,ui->revision,step.a,&key));tcPulse(key,0,0,FALSE,TRUE);}return;}
}
static u32 tcInputContractSelfCheck(void) {
    HGNativeTrainerCardUIState ui={.active=1,.ownerGeneration=7,.revision=9,.ready=1,.viewActive=1,.view=2,.phase=1,.subState=1};
    HGNativeTrainerCardUITouchLease lease={0};u32 output=123;
    if(HGNativeTrainerCardUIResolveKey(&ui,7,9,2,&output) || output!=123)return __LINE__;
    if(!HGNativeTrainerCardUIResolveTouch(&lease,&ui,7,9,HG_TRAINER_CARD_TOUCH_DOWN,32,72))return __LINE__;
    if(!HGNativeTrainerCardUIResolveTouch(&lease,&ui,7,9,HG_TRAINER_CARD_TOUCH_MOVE,223,135))return __LINE__;
    if(HGNativeTrainerCardUIResolveTouch(&lease,&ui,7,8,HG_TRAINER_CARD_TOUCH_UP,0,0) || !lease.down)return __LINE__;
    ui.ready=0;HGNativeTrainerCardUIFenceTouch(&lease,&ui);if(lease.down)return __LINE__;
    HGYNBootFacts title={.kind=1,.active=1,.ownerGeneration=1,.revision=1,.ready=1,.phase=2,.progressFlags=15};
    if(!tcBootEligible(&title))return __LINE__;title.progressFlags=7;if(tcBootEligible(&title))return __LINE__;
    HGYNBootFacts menu={.kind=2,.active=1,.ownerGeneration=7,.revision=11,.ready=1,.phase=5,.notice=15,.progressFlags=255};
    if(!tcBootSameSwitchedOwner(&menu,7,10) || tcBootSameSwitchedOwner(&menu,8,10) || tcBootSameSwitchedOwner(&menu,7,11))return __LINE__;
    menu.inputMode=1;if(tcBootSameSwitchedOwner(&menu,7,10))return __LINE__;menu.inputMode=0;
    menu.active=menu.ready=0;menu.phase=8;menu.exit=1;
    if(!tcBootExitWitness(&menu,7) || tcBootExitWitness(&menu,8) || tcBootExitWitness(&menu,0))return __LINE__;
    menu.phase=7;if(tcBootExitWitness(&menu,7))return __LINE__;
    title.active=title.ready=0;title.phase=6;title.exit=1;
    if(!tcBootTitleExitWitness(&title,1) || tcBootTitleExitWitness(&title,2))return __LINE__;
    title.active=1;if(tcBootTitleExitWitness(&title,1))return __LINE__;
    HGNativeFieldMenuUIState fieldMenu={.active=1,.ownerGeneration=7,.revision=9,.ready=1,.phase=3,.viewActive=1,.controlActive=1};
    if(!tcMenuCanClose(&fieldMenu,44,0) || !tcMenuCanClose(&fieldMenu,48,0) || tcMenuCanClose(&fieldMenu,53,0) || tcMenuCanClose(&fieldMenu,44,1))return __LINE__;
    fieldMenu.childActive=1;if(tcMenuCanClose(&fieldMenu,44,0))return __LINE__;fieldMenu.childActive=0;
    fieldMenu.inputMode=1;if(tcMenuCanClose(&fieldMenu,44,0))return __LINE__;fieldMenu.inputMode=0;
    fieldMenu.ready=0;if(tcMenuCanClose(&fieldMenu,44,0))return __LINE__;
    fieldMenu=(HGNativeFieldMenuUIState){.ownerGeneration=7,.revision=10,.phase=UINT32_MAX};
    if(!tcMenuRetired(&fieldMenu,7,9) || tcMenuRetired(&fieldMenu,8,9) || tcMenuRetired(&fieldMenu,7,10) || tcMenuRetired(&fieldMenu,0,9))return __LINE__;
    fieldMenu.active=1;if(tcMenuRetired(&fieldMenu,7,9))return __LINE__;fieldMenu.active=0;
    fieldMenu.childActive=1;if(tcMenuRetired(&fieldMenu,7,9))return __LINE__;
    return 0;
}

int main(int argc,char **argv) {
    setvbuf(stdout,NULL,_IONBF,0);
    if (tcInputContractSelfCheck()) return 2;
    if (argc!=5 || !HGNativeFSSetRoot(argv[1]) || !HGNativeBackupSetPath(argv[3])) return 2;
    u8 header[0x160];FILE *input=fopen(argv[2],"rb");if (!input) return 2;
    BOOL valid=fread(header,1,sizeof(header),input)==sizeof(header) && fgetc(input)==EOF && !ferror(input);fclose(input);if (!valid) return 2;
    wasm_rt_init();w2c_hgengine *game=calloc(1,sizeof(*game));struct w2c_env *env=calloc(1,sizeof(*env));
    struct w2c_wasi__snapshot__preview1 *wasi=calloc(1,sizeof(*wasi));Capture *capture=calloc(1,sizeof(*capture));
    if (!game || !env || !wasi || !capture) return 2;
    wasi->fds[0]=dup(0);wasi->fds[1]=dup(1);wasi->fds[2]=dup(2);if (wasi->fds[0]<0 || wasi->fds[1]<0 || wasi->fds[2]<0) return 2;
    tcCapture=capture;capture->game=game;capture->env=env;
    if (!setjmp(tcResetLanding)) {
        wasm_rt_trap_t trap=wasm_rt_impl_try();TC_CHECK(!trap);
        wasm2c_hgengine_instantiate(game,env,wasi);env->memory=wasi->memory=w2c_hgengine_memory(game);TC_CHECK(HGNativeSoundRealtime(env,TRUE));
        env->irqTable=&game->w2c_T0;env->stackPointer=&game->w2c_0x5F_stack_pointer;env->rootStackHigh=*env->stackPointer;*env->stackPointer-=16;
        env->irqType0=wasm2c_hgengine_get_func_type(0,0);env->irqType1=wasm2c_hgengine_get_func_type(1,0,WASM_RT_I32);
        env->rtcType=wasm2c_hgengine_get_func_type(2,0,WASM_RT_I32,WASM_RT_I32);env->irqGuardType=wasm2c_hgengine_get_func_type(4,0,WASM_RT_I32,WASM_RT_I32,WASM_RT_I32,WASM_RT_I32);
        u32 system=w2c_hgengine_HGNativeSystemWorkAddress(game);memcpy((void *)tcGuest(system,0x360)+0x200,header,sizeof(header));
        w2c_hgengine_0x5Finitialize(game);u32 status=w2c_hgengine_HGNativeRegisterAddress(game,0x04000004,2);TC_CHECK(HGNativeStartDisplayClock(env,tcGuest(status,2)));
        *capture=(Capture){.game=game,.env=env,.folder=argv[4],.resume=TRUE,.noInput=TRUE};
        TC_CHECK(HGNativeSetDisplayObserver(env,TCQAPresent,capture));w2c_hgengine_NitroMain(game);TC_CHECK(FALSE);
    }
    BOOL quiescent=FALSE;
    if (tcResetExpected && env->resetRequested) {HGNativeFinishReset(env);quiescent=env->resetFinished && !env->pendingDMA && !env->dmaAffineWrites && !env->pendingRTC
        && !env->pendingMIC && !env->pendingCARD && !env->pendingPOWER && !env->pendingINPUT && !env->irqTable && !env->threads && !env->rasterFunction;}
    printf("TRAINER evidence: {\"status\":\"%s\",\"qaVersion\":%u,\"qaBytes\":%u,\"step\":%u,\"totalSteps\":%zu,\"confirms\":%u,\"editorEntries\":%u,\"editorExits\":%u,\"directComplete\":%u,\"checkpoints\":%u,\"staleRejected\":%u,\"snapshots\":%u,\"strokeEffects\":%u,\"titleExit\":%u,\"continueExit\":%u,\"modeSwitchSeen\":%u,\"menuCloseRequests\":%u,\"menuCloses\":%u,\"directFieldWitnesses\":%u,\"terminalFieldReady\":%u,\"menuActive\":%u,\"menuReady\":%u,\"menuPhase\":%u,\"menuViewActive\":%u,\"menuControlActive\":%u,\"menuChildActive\":%u,\"menuGeneration\":%u,\"menuRevision\":%u,\"privateSaveWrites\":false,\"resetQuiescent\":%s}\n",
        tcStatus,tcFinal.version,tcFinal.byteSize,tcStep,sizeof(tcSteps)/sizeof(tcSteps[0]),tcFinal.confirms,tcFinal.editorEntries,tcFinal.editorExits,
        tcFinal.directComplete,tcFinal.checkpoints,tcStaleRejected,tcSnapshots,tcStrokeEffects,tcTitleExit,tcContinueExit,tcModeSwitchSeen,
        tcMenuCloseRequests,tcMenuCloses,tcDirectFieldWitnesses,tcFinal.fieldReady,tcMenu.active,tcMenu.ready,tcMenu.phase,tcMenu.viewActive,tcMenu.controlActive,tcMenu.childActive,tcMenu.ownerGeneration,tcMenu.revision,quiescent ? "true" : "false");
    BOOL passed=tcPassed && quiescent && tcStep+1==sizeof(tcSteps)/sizeof(tcSteps[0]) && tcFinal.confirms==3
        && tcFinal.editorEntries==3 && tcFinal.editorExits==3 && tcFinal.directComplete && tcStrokeEffects==4
        && tcMenuCloseRequests==2 && tcMenuCloses==2 && tcDirectFieldWitnesses==1 && tcFinal.fieldReady && !tcMenu.active;
    wasm2c_hgengine_free(game);for(u32 i=0;i<3;++i)close(wasi->fds[i]);free(game);free(env);free(wasi);free(capture);wasm_rt_free();
    return passed ? 0 : 1;
}
