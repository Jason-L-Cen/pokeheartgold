// SPDX-License-Identifier: GPL-3.0-or-later
#define main StarterBootstrapMain
#define HGEngineTrap StarterBootstrapTrap
#include "engine_check.c"
#undef HGEngineTrap
#undef main
#include "starter_qa.h"
#include <nitro/os/native_reset.h>
#include <setjmp.h>
static Capture *sqCapture;
static HGNativeStarterQAState sqFinal;
static jmp_buf sqLanding;
static BOOL sqResetExpected,sqPassed;
static const char *sqStatus="not_started";
static u32 sqStage,sqStageFrame,sqLease,sqReads,sqReleaseReads,sqOwner,sqGen,sqRev,sqLCD,sqKey;
static u32 sqScript,sqSerial,sqMoveX,sqMoveZ,sqMovePending,sqBlocked,sqSteps,sqSnapshots,sqStale;
static u32 sqTitleSent,sqTitleGen,sqTitleExit,sqContinueSent,sqContinueGen,sqContinueExit,sqSwitchSent,sqSwitchGen,sqSwitchRev;
static u32 sqLastTextScript,sqLastTextSerial,sqDesired,sqInitialSelection,sqNicknameSent,sqPhaseMask,sqPhaseCaptured;
static u32 sqOpenFrame,sqOpenSnapshots,sqTouchDone,sqLeftDone,sqRightDone,sqBackDone,sqConfirmDone;
static void sqFinish(const char *status,BOOL passed) {
    sqStatus=status;sqPassed=passed;
    if(sqCapture && sqCapture->env && sqCapture->env->memory) {
        HGNativeSetInput(sqCapture->env,0,FALSE,0,0,FALSE);sqResetExpected=TRUE;
        w2c_env_HGNativeResetControl(sqCapture->env,HG_RESET_REQUEST,0);
    }
    fprintf(stderr,"Starter failed before reset: %s\n",status);_Exit(1);
}
#define SQ_CHECK(value) do {if(!(value))sqFinish("assert:" #value,FALSE);} while(0)
void HGEngineTrap(wasm_rt_trap_t code) {
    if(code==WASM_RT_TRAP_UNREACHABLE && sqResetExpected && sqCapture && sqCapture->env->resetRequested)longjmp(sqLanding,1);
    StarterBootstrapTrap(code);
}
static const void *sqGuest(u32 address,size_t size) {
    SQ_CHECK(address && address<=sqCapture->env->memory->size && size<=sqCapture->env->memory->size-address);
    return sqCapture->env->memory->data+address;
}
static HGNativeStarterQAState sqRead(void) {
    u32 address=w2c_hgengine_HGNativeStarterQAGetState(sqCapture->game);HGNativeStarterQAState state;
    memcpy(&state,sqGuest(address,sizeof(state)),sizeof(state));
    SQ_CHECK(state.version==HG_STARTER_QA_VERSION && state.byteSize==sizeof(state) && !state.error);
    memcpy(&state.ui,sqGuest(w2c_hgengine_HGNativeStarterUIGetState(sqCapture->game),sizeof(state.ui)),sizeof(state.ui));
    memcpy(&state.choice,sqGuest(w2c_hgengine_HGNativeFieldChoiceUIGetState(sqCapture->game),sizeof(state.choice)),sizeof(state.choice));
    return state;
}
static u32 sqTouchEngine(void) {
    u32 address=w2c_hgengine_HGNativeRegisterAddress(sqCapture->game,0x04000304,2);u16 control;
    memcpy(&control,sqGuest(address,sizeof(control)),sizeof(control));return (control>>15)&1;
}
static BOOL sqSnapshot(const char *label,BOOL blankAllowed) {
    HGNativeStarterQAState before=sqRead();static u8 pixels[2][256*192*4];u32 rendered=0,varied=0;
    if(!sqCapture->buffer)sqCapture->buffer=w2c_hgengine_HGNativeFrameBuffer(sqCapture->game);
    HGNativeDrainDMA();sqGuest(sqCapture->buffer,sizeof(pixels[0]));
    for(u32 engine=0;engine<2;++engine)if(w2c_hgengine_HGNativeRender2D(sqCapture->game,engine,sqCapture->buffer,sizeof(pixels[0]))) {
        rendered|=1u<<engine;memcpy(pixels[engine],sqGuest(sqCapture->buffer,sizeof(pixels[0])),sizeof(pixels[0]));
        for(u32 i=1;i<256*192;++i)if(memcmp(pixels[engine],pixels[engine]+i*4,4)){varied|=1u<<engine;break;}
    }
    u32 required=before.ui.active || before.choice.viewActive ? 3 : 1;
    if((rendered&required)!=required || (!blankAllowed && (varied&required)!=required))return FALSE;
    HGNativeStarterQAState after=sqRead();if(memcmp(&before,&after,sizeof(before)))return FALSE;
    char path[4096];SQ_CHECK(snprintf(path,sizeof(path),"%s/starter-%03u-state.bin",sqCapture->folder,sqSnapshots)<sizeof(path));
    FILE *file=fopen(path,"wbx");SQ_CHECK(file && fwrite(&after,1,sizeof(after),file)==sizeof(after) && !fclose(file));
    for(u32 engine=0;engine<2;++engine)if(rendered&(1u<<engine)) {
        SQ_CHECK(snprintf(path,sizeof(path),"%s/starter-%03u-engine-%u.ppm",sqCapture->folder,sqSnapshots,engine)<sizeof(path));
        FILE *image=fopen(path,"wbx");SQ_CHECK(image && fprintf(image,"P6\n256 192\n255\n")>0);
        for(u32 row=0;row<192;++row) {u8 rgb[256*3];for(u32 x=0;x<256;++x)memcpy(rgb+x*3,pixels[engine]+(row*256+x)*4,3);SQ_CHECK(fwrite(rgb,1,sizeof(rgb),image)==sizeof(rgb));}
        SQ_CHECK(!fclose(image));
    }
    printf("STARTER snapshot: {\"sequence\":%u,\"stage\":%u,\"label\":\"%s\",\"phase\":%u,\"selection\":%u,\"selectionState\":%u,\"generation\":%u,\"revision\":%u,\"frame\":%u,\"renderedMask\":%u,\"variedMask\":%u,\"blankAllowed\":%s}\n",
        sqSnapshots,sqStage,label,after.ui.phase,after.ui.selection,after.ui.selectionState,after.ui.ownerGeneration,after.ui.revision,OS_GetVBlankCount(),rendered,varied,blankAllowed ? "true" : "false");
    ++sqSnapshots;return TRUE;
}
static BOOL sqBootReady(const HGYNBootFacts *b) {
    if(!b || b->active!=1 || b->ready!=1 || !b->ownerGeneration || !b->revision)return FALSE;
    return b->kind==HG_YN_BOOT_TITLE ? b->phase==2 && !b->inputMode && !b->option && !b->notice && !b->exit && (b->progressFlags&15)==15
        : b->kind==HG_YN_BOOT_MAIN_MENU && b->phase==5 && b->inputMode<=1 && b->option<9 && b->notice==15 && !b->exit && (b->progressFlags&255)==255;
}
static void sqTrace(const char *event) {
    printf("STARTER input: {\"event\":\"%s\",\"stage\":%u,\"owner\":%u,\"generation\":%u,\"revision\":%u,\"key\":%u,\"padReads\":%u,\"script\":%u,\"serial\":%u,\"map\":%u,\"x\":%u,\"z\":%u}\n",
        event,sqStage,sqOwner,sqGen,sqRev,sqKey,HGNativeInputPadReads(sqCapture->env),sqScript,sqSerial,sqFinal.mapId,sqFinal.x,sqFinal.z);
}
static void sqPulse(u32 key,u32 x,u32 y,BOOL touch,u32 owner) {
    SQ_CHECK(!sqLease);sqOwner=owner;sqKey=key;sqLCD=sqTouchEngine();
    sqGen=owner==1 ? sqFinal.boot.ownerGeneration : owner==2 ? sqFinal.ui.ownerGeneration : owner==3 ? sqFinal.choice.ownerGeneration : 0;
    sqRev=owner==1 ? sqFinal.boot.revision : owner==2 ? sqFinal.ui.revision : owner==3 ? sqFinal.choice.revision : 0;
    sqScript=sqFinal.scriptId;sqSerial=sqFinal.scriptSerial;
    SQ_CHECK(owner!=2 || sqLCD==0);
    SQ_CHECK(HGNativeSetInput(sqCapture->env,key,FALSE,x,y,touch));sqReads=HGNativeInputPadReads(sqCapture->env);sqLease=1;sqTrace("press");
}
static BOOL sqSettle(void) {
    if(!sqLease)return TRUE;u32 reads=HGNativeInputPadReads(sqCapture->env);
    BOOL valid=sqOwner==1 ? sqBootReady(&sqFinal.boot) && sqFinal.boot.ownerGeneration==sqGen && sqFinal.boot.revision==sqRev
        : sqOwner==2 ? HGNativeStarterUIKeyAllowed(&sqFinal.ui,sqGen,sqRev,1)
        : sqOwner==3 ? HGNativeFieldChoiceUIInputAllowed(&sqFinal.choice,sqGen,sqRev)
        : sqOwner==4 ? HGStarterTextAllowed(&sqFinal,sqScript,sqSerial)
        : HGStarterStationary(&sqFinal) && (!sqMovePending || (sqFinal.x==sqMoveX && sqFinal.z==sqMoveZ));
    if(sqOwner && sqTouchEngine()!=sqLCD)valid=FALSE;
    if(sqLease==1 && (!valid || reads-sqReads>=4)) {
        SQ_CHECK(HGNativeSetInput(sqCapture->env,0,FALSE,0,0,FALSE));sqReleaseReads=reads;sqLease=2;sqTrace("release");return FALSE;
    }
    if(sqLease!=2 || reads-sqReleaseReads<2)return FALSE;
    sqLease=0;sqTrace("settled");return TRUE;
}
static void sqNext(void) {++sqStage;sqStageFrame=OS_GetVBlankCount();}
static BOOL sqReady(u32 selection,u32 selectionState) {
    return HGNativeStarterUIKeyAllowed(&sqFinal.ui,sqFinal.ui.ownerGeneration,sqFinal.ui.revision,1)
        && sqFinal.ui.selection==selection && sqFinal.ui.selectionState==selectionState;
}
static void sqStarterKey(u32 key) {
    HGNativeStarterUIState *s=&sqFinal.ui;
    SQ_CHECK(!HGNativeStarterUIKeyAllowed(s,s->ownerGeneration,s->revision-1,key));++sqStale;
    SQ_CHECK(!HGNativeStarterUIKeyAllowed(s,s->ownerGeneration+1,s->revision,key));++sqStale;
    SQ_CHECK(HGNativeStarterUIKeyAllowed(s,s->ownerGeneration,s->revision,key));sqPulse(key,0,0,FALSE,2);
}
static BOOL sqAdvanceText(void) {
    if(!HGStarterTextAllowed(&sqFinal,sqFinal.scriptId,sqFinal.scriptSerial)
        || (sqLastTextScript==sqFinal.scriptId && sqLastTextSerial==sqFinal.scriptSerial))return FALSE;
    if(!sqSnapshot("script-wait",FALSE))return TRUE;
    sqLastTextScript=sqFinal.scriptId;sqLastTextSerial=sqFinal.scriptSerial;sqPulse(1,0,0,FALSE,4);return TRUE;
}
static void sqNavigate(void) {
    if(!HGStarterStationary(&sqFinal))return;
    if(sqMovePending) {
        SQ_CHECK(HGStarterStepValid(sqKey,sqMoveX,sqMoveZ,sqFinal.x,sqFinal.z));
        if(sqFinal.x==sqMoveX && sqFinal.z==sqMoveZ)SQ_CHECK(++sqBlocked<=16);else {sqBlocked=0;++sqSteps;}
        sqMovePending=0;
    }
    SQ_CHECK(sqFinal.scene==0 && !sqFinal.gotStarter && !sqFinal.partyCount && sqFinal.x>=4 && sqFinal.x<=8 && sqFinal.z>=5 && sqFinal.z<=14);
    u32 key=sqFinal.z>7 ? 64 : sqFinal.x<8 ? 16 : sqFinal.z>5 ? 64 : 0;
    if(!key) {SQ_CHECK(sqFinal.x==8 && sqFinal.z==5 && !sqFinal.facing);if(!sqSnapshot("machine",FALSE))return;sqPulse(1,0,0,FALSE,0);sqNext();return;}
    sqMoveX=sqFinal.x;sqMoveZ=sqFinal.z;sqMovePending=1;sqPulse(key,0,0,FALSE,0);
}
static void StarterPresent(void *argument) {
    sqCapture=argument;sqFinal=sqRead();u32 frame=OS_GetVBlankCount();
    if(!sqStageFrame)sqStageFrame=frame;if(frame-sqStageFrame>(sqStage ? 3600u : 12000u))sqFinish("stage_timeout",FALSE);
    HGYNBootFacts *b=&sqFinal.boot;
    if(sqTitleSent && b->kind==HG_YN_BOOT_TITLE && !b->active && !b->ready && b->phase==6 && b->ownerGeneration==sqTitleGen && b->exit==1)sqTitleExit=1;
    if(sqContinueSent && b->kind==HG_YN_BOOT_MAIN_MENU && !b->active && !b->ready && b->phase==8 && b->ownerGeneration==sqContinueGen && b->exit==1)sqContinueExit=1;
    if(sqFinal.ui.active) {
        SQ_CHECK(sqFinal.ui.phase<=12);u32 bit=1u<<sqFinal.ui.phase;sqPhaseMask|=bit;
        if(!(sqPhaseCaptured&bit) && sqSnapshot("scene-phase",TRUE))sqPhaseCaptured|=bit;
        if(sqFinal.ui.phase==11) {
            if(!sqOpenFrame)sqOpenFrame=frame;
            if(sqOpenSnapshots<3 && frame-sqOpenFrame>=sqOpenSnapshots*4 && sqSnapshot("ball-open",TRUE))++sqOpenSnapshots;
        }
    }
    if(!sqSettle())return;
    if(sqStage==0) {
        if(sqFinal.fieldReady) {SQ_CHECK(sqContinueSent && sqContinueExit && (!sqTitleSent || sqTitleExit) && HGStarterBaseline(&sqFinal));if(!sqSnapshot("loaded-baseline",FALSE))return;sqNext();return;}
        if(!sqBootReady(b))return;
        if(b->kind==HG_YN_BOOT_TITLE) {if(sqTitleSent)return;}
        else if(b->inputMode) {if(sqSwitchSent || sqContinueSent)return;}
        else {if(sqContinueSent)return;SQ_CHECK(!sqSwitchSent || (b->ownerGeneration==sqSwitchGen && b->revision!=sqSwitchRev));}
        if(!sqSnapshot("boot",FALSE))return;
        if(b->kind==HG_YN_BOOT_TITLE) {sqTitleSent=1;sqTitleGen=b->ownerGeneration;}
        else if(b->inputMode) {sqSwitchSent=1;sqSwitchGen=b->ownerGeneration;sqSwitchRev=b->revision;}
        else {sqContinueSent=1;sqContinueGen=b->ownerGeneration;}
        sqPulse(1,0,0,FALSE,1);return;
    }
    if(sqStage<=2 && sqAdvanceText())return;
    if(sqStage==1) {sqNavigate();return;}
    if(sqStage==2) {
        if(!sqReady(0,0))return;
        SQ_CHECK(sqFinal.probeValid==1 && sqFinal.probeUnchanged==1 && sqFinal.probeQueries && sqFinal.probeQueries<=713
            && sqFinal.probeGeneration==sqFinal.ui.ownerGeneration && !sqFinal.probeSelection);
        SQ_CHECK(!HGNativeStarterUITouchAllowed(&sqFinal.ui,sqFinal.ui.ownerGeneration,sqFinal.ui.revision-1,0,sqFinal.probeX,sqFinal.probeY));++sqStale;
        SQ_CHECK(!HGNativeStarterUITouchAllowed(&sqFinal.ui,sqFinal.ui.ownerGeneration,sqFinal.ui.revision,1,sqFinal.probeX,sqFinal.probeY));++sqStale;
        SQ_CHECK(HGNativeStarterUITouchAllowed(&sqFinal.ui,sqFinal.ui.ownerGeneration,sqFinal.ui.revision,0,sqFinal.probeX,sqFinal.probeY));
        if(!sqSnapshot("touch-before",FALSE))return;sqPulse(0,sqFinal.probeX,sqFinal.probeY,TRUE,2);sqNext();return;
    }
    if(sqStage==3) {if(!sqReady(0,1))return;sqTouchDone=1;sqInitialSelection=0;if(!sqSnapshot("touch-inspect",FALSE))return;sqStarterKey(16);sqNext();return;}
    if(sqStage==4) {if(!sqReady(2,1))return;sqRightDone=1;if(!sqSnapshot("right",FALSE))return;sqStarterKey(32);sqNext();return;}
    if(sqStage==5) {if(!sqReady(sqInitialSelection,1))return;sqLeftDone=1;sqStarterKey(1);sqNext();return;}
    if(sqStage==6) {if(!sqReady(0,2))return;if(!sqSnapshot("confirm-before-back",FALSE))return;sqStarterKey(2);sqNext();return;}
    if(sqStage==7) {if(!sqReady(0,0))return;sqBackDone=1;if(!sqSnapshot("back",FALSE))return;sqStarterKey(1);sqNext();return;}
    if(sqStage==8) {
        if(!HGNativeStarterUIKeyAllowed(&sqFinal.ui,sqFinal.ui.ownerGeneration,sqFinal.ui.revision,1) || sqFinal.ui.selectionState!=1)return;
        if(sqFinal.ui.selection!=sqDesired) {sqStarterKey(32);return;}
        if(!sqSnapshot("final-inspect",FALSE))return;sqStarterKey(1);sqNext();return;
    }
    if(sqStage==9) {if(!sqReady(sqDesired,2))return;if(!sqSnapshot("final-confirm",FALSE))return;sqConfirmDone=1;sqStarterKey(1);sqNext();return;}
    if(sqStage==10) {
        if(sqFinal.ui.active)return;
        if(sqFinal.choice.active && HGNativeFieldChoiceUIIsYesNoInput(&sqFinal.choice) && HGNativeFieldChoiceUIInputAllowed(&sqFinal.choice,sqFinal.choice.ownerGeneration,sqFinal.choice.revision)) {
            SQ_CHECK(sqFinal.scriptId==HG_STARTER_QA_MACHINE && sqFinal.gotStarter && sqFinal.partyCount==1 && !sqNicknameSent);
            if(!sqSnapshot("nickname-no",FALSE))return;u32 key=0;
            SQ_CHECK(!HGNativeFieldChoiceUIResolveInput(&sqFinal.choice,sqFinal.choice.ownerGeneration,sqFinal.choice.revision-1,2,&key) && !key);++sqStale;
            SQ_CHECK(HGNativeFieldChoiceUIResolveInput(&sqFinal.choice,sqFinal.choice.ownerGeneration,sqFinal.choice.revision,2,&key) && key==2);
            sqNicknameSent=1;sqPulse(key,0,0,FALSE,3);return;
        }
        if(sqAdvanceText())return;
        if(!HGStarterStationary(&sqFinal) || sqFinal.scriptActive)return;
        static const u32 species[3]={152,155,158};
        SQ_CHECK(sqFinal.partyCount==1 && sqFinal.species==species[sqDesired] && sqFinal.level==5 && sqFinal.dexSeen && sqFinal.dexCaught
            && sqFinal.gotStarter && sqFinal.scene==1 && sqFinal.starterEntries==1 && sqFinal.starterExits==1 && sqFinal.grants==1
            && sqFinal.nicknameNo && sqFinal.choiceReturns==1 && sqNicknameSent && sqTouchDone && sqLeftDone && sqRightDone && sqBackDone && sqConfirmDone
            && (sqPhaseMask&((1u<<6)|(1u<<7)|(1u<<8)|(1u<<9)|(1u<<10)|(1u<<11)|(1u<<12)))==((1u<<6)|(1u<<7)|(1u<<8)|(1u<<9)|(1u<<10)|(1u<<11)|(1u<<12)) && sqOpenSnapshots==3);
        if(!sqSnapshot("granted-and-returned",FALSE))return;sqFinish("passed_original_starter",TRUE);
    }
}
int main(int argc,char **argv) {
    setvbuf(stdout,NULL,_IONBF,0);
    if(argc!=6 || strlen(argv[5])!=1 || argv[5][0]<'0' || argv[5][0]>'2' || !HGNativeFSSetRoot(argv[1]) || !HGNativeBackupSetPath(argv[3]))return 2;
    sqDesired=(u32)(argv[5][0]-'0');u8 header[0x160];FILE *input=fopen(argv[2],"rb");if(!input)return 2;
    BOOL valid=fread(header,1,sizeof(header),input)==sizeof(header) && fgetc(input)==EOF && !ferror(input);fclose(input);if(!valid)return 2;
    wasm_rt_init();w2c_hgengine *game=calloc(1,sizeof(*game));struct w2c_env *env=calloc(1,sizeof(*env));
    struct w2c_wasi__snapshot__preview1 *wasi=calloc(1,sizeof(*wasi));Capture *capture=calloc(1,sizeof(*capture));
    if(!game || !env || !wasi || !capture)return 2;
    wasi->fds[0]=dup(0);wasi->fds[1]=dup(1);wasi->fds[2]=dup(2);if(wasi->fds[0]<0 || wasi->fds[1]<0 || wasi->fds[2]<0)return 2;
    sqCapture=capture;capture->game=game;capture->env=env;
    if(!setjmp(sqLanding)) {
        wasm_rt_trap_t trap=wasm_rt_impl_try();SQ_CHECK(!trap);
        wasm2c_hgengine_instantiate(game,env,wasi);env->memory=wasi->memory=w2c_hgengine_memory(game);SQ_CHECK(HGNativeSoundRealtime(env,TRUE));
        env->irqTable=&game->w2c_T0;env->stackPointer=&game->w2c_0x5F_stack_pointer;env->rootStackHigh=*env->stackPointer;*env->stackPointer-=16;
        env->irqType0=wasm2c_hgengine_get_func_type(0,0);env->irqType1=wasm2c_hgengine_get_func_type(1,0,WASM_RT_I32);
        env->rtcType=wasm2c_hgengine_get_func_type(2,0,WASM_RT_I32,WASM_RT_I32);env->irqGuardType=wasm2c_hgengine_get_func_type(4,0,WASM_RT_I32,WASM_RT_I32,WASM_RT_I32,WASM_RT_I32);
        u32 system=w2c_hgengine_HGNativeSystemWorkAddress(game);memcpy((void *)sqGuest(system,0x360)+0x200,header,sizeof(header));
        w2c_hgengine_0x5Finitialize(game);u32 status=w2c_hgengine_HGNativeRegisterAddress(game,0x04000004,2);SQ_CHECK(HGNativeStartDisplayClock(env,sqGuest(status,2)));
        *capture=(Capture){.game=game,.env=env,.folder=argv[4],.resume=TRUE,.noInput=TRUE};
        SQ_CHECK(HGNativeSetDisplayObserver(env,StarterPresent,capture));w2c_hgengine_NitroMain(game);SQ_CHECK(FALSE);
    }
    BOOL quiescent=FALSE;
    if(sqResetExpected && env->resetRequested) {HGNativeFinishReset(env);quiescent=env->resetFinished && !env->pendingDMA && !env->dmaAffineWrites && !env->pendingRTC
        && !env->pendingMIC && !env->pendingCARD && !env->pendingPOWER && !env->pendingINPUT && !env->irqTable && !env->threads && !env->rasterFunction;}
    printf("STARTER evidence: {\"status\":\"%s\",\"starterIndex\":%u,\"partyCount\":%u,\"species\":%u,\"level\":%u,\"dexSeen\":%u,\"dexCaught\":%u,\"gotStarter\":%u,\"scene\":%u,\"entries\":%u,\"exits\":%u,\"grants\":%u,\"nicknameNo\":%u,\"choiceReturns\":%u,\"touchInspect\":%u,\"left\":%u,\"right\":%u,\"back\":%u,\"finalConfirm\":%u,\"phaseMask\":%u,\"openSnapshots\":%u,\"snapshots\":%u,\"steps\":%u,\"staleRejected\":%u,\"probeQueries\":%u,\"probeUnchanged\":%u,\"fieldReady\":%u,\"privateSaveWrites\":false,\"resetQuiescent\":%s}\n",
        sqStatus,sqDesired,sqFinal.partyCount,sqFinal.species,sqFinal.level,sqFinal.dexSeen,sqFinal.dexCaught,sqFinal.gotStarter,sqFinal.scene,sqFinal.starterEntries,sqFinal.starterExits,sqFinal.grants,sqFinal.nicknameNo,sqFinal.choiceReturns,
        sqTouchDone,sqLeftDone,sqRightDone,sqBackDone,sqConfirmDone,sqPhaseMask,sqOpenSnapshots,sqSnapshots,sqSteps,sqStale,sqFinal.probeQueries,sqFinal.probeUnchanged,sqFinal.fieldReady,quiescent ? "true" : "false");
    BOOL passed=sqPassed && quiescent;wasm2c_hgengine_free(game);for(u32 i=0;i<3;++i)close(wasi->fds[i]);free(game);free(env);free(wasi);free(capture);wasm_rt_free();return passed ? 0 : 1;
}
