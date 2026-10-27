// SPDX-License-Identifier: GPL-3.0-or-later
#define main PGQABootstrapMain
#define HGEngineTrap PGQABootstrapTrap
#include "engine_check.c"
#undef HGEngineTrap
#undef main
#include "pokegear_qa.h"
#include "constants/maps.h"
#include "native_field_menu_ui.h"
#include <nitro/os/native_reset.h>
#include <setjmp.h>
// All w2c calls below require the real future generated hgengine.h; no replacement prototypes.
enum {PG_BOOT,PG_START,PG_MENU_OPEN,PG_ICON,PG_VIEW,PG_LOCKED,PG_TOUCH,PG_KEY,PG_FOCUS,PG_FOOTER,PG_CONTEXT,PG_CONTACT,PG_CALL_WAIT,PG_CALL_DONE,PG_CLOSE,PG_FIELD,PG_FINISH};
typedef struct PGStep {u32 op,a,b;} PGStep;
static const PGStep pgSteps[]={
    {PG_BOOT},{PG_START},{PG_MENU_OPEN},{PG_ICON},{PG_VIEW,3,0},{PG_LOCKED},{PG_TOUCH,32,176},
    {PG_VIEW,0,0},{PG_TOUCH,128,44},{PG_CONTEXT,0,1},{PG_KEY,1},{PG_VIEW,0,1},
    {PG_FOCUS},{PG_KEY,16},{PG_FOOTER,3},{PG_KEY,1},{PG_VIEW,3,1},
    {PG_CONTACT},{PG_CONTEXT,3,1},{PG_KEY,1},{PG_CALL_WAIT},{PG_KEY,1},{PG_CALL_DONE},
    {PG_TOUCH,32,176},{PG_VIEW,0,1},{PG_CLOSE},{PG_FIELD,1},
    {PG_MENU_OPEN},{PG_ICON},{PG_VIEW,0,1},{PG_CLOSE},{PG_FIELD,2},{PG_FINISH}
};
static Capture *pgCapture;
static jmp_buf pgResetLanding;
static BOOL pgResetExpected,pgPassed;
static const char *pgStatus="not_started";
static HGNativePokegearQAState pgFinal;
static HGNativeFieldMenuUIState pgMenu,pgMenuBefore;
static HGNativePokegearUIState pgCloseBefore;
static u32 pgStep,pgStepFrame,pgLease,pgPressReads,pgReleaseReads,pgKey,pgX,pgY,pgTouch,pgLeaseOwner;
static u32 pgLeaseGeneration,pgLeaseRevision,pgLeaseEngine,pgStaleRejected,pgSnapshots,pgLockedRejected,pgFocusSent,pgCloseRequested;
static u32 pgMenuCloseStage,pgMenuCloseRequests,pgMenuCloses,pgPendingRequest,pgRequestedCheckpoint;
static u32 pgTextContinues,pgLastTextSerial;
static u32 pgTitleSent,pgTitleGeneration,pgTitleExit,pgSwitchSent,pgSwitchGeneration,pgSwitchRevision;
static u32 pgModeSwitchSeen,pgContinueSent,pgContinueGeneration,pgContinueExit;
void HGEngineTrap(wasm_rt_trap_t code) {
    if(code==WASM_RT_TRAP_UNREACHABLE && pgResetExpected && pgCapture && pgCapture->env->resetRequested)longjmp(pgResetLanding,1);
    PGQABootstrapTrap(code);
}
static void pgFinish(const char *status,BOOL passed) {
    pgStatus=status;pgPassed=passed;
    if(pgCapture && pgCapture->env && pgCapture->env->memory) {
        HGNativeSetInput(pgCapture->env,0,FALSE,0,0,FALSE);pgResetExpected=TRUE;
        w2c_env_HGNativeResetControl(pgCapture->env,HG_RESET_REQUEST,0);
    }
    fprintf(stderr,"Pokegear failed before reset: %s\n",status);_Exit(1);
}
#define PG_CHECK(value) do{if(!(value)){fprintf(stderr,"Pokegear line %d step %u: %s\n",__LINE__,pgStep,#value);pgFinish("assertion_failed",FALSE);}}while(0)
static const void *pgGuest(u32 address,size_t size) {
    PG_CHECK(address && address<=pgCapture->env->memory->size && size<=pgCapture->env->memory->size-address);
    return pgCapture->env->memory->data+address;
}
static u32 pgTouchEngine(void) {
    // Existing OriginalApp POWCNT1 witness; the address is SDK MMIO, never a game-data offset.
    u32 address=w2c_hgengine_HGNativeRegisterAddress(pgCapture->game,0x04000304,2);u16 control;
    memcpy(&control,pgGuest(address,sizeof(control)),sizeof(control));return (control>>15)&1;
}
static HGNativeFieldMenuUIState pgReadMenu(void) {
    u32 address=w2c_hgengine_HGNativeFieldMenuUIGetState(pgCapture->game);HGNativeFieldMenuUIState menu;
    memcpy(&menu,pgGuest(address,sizeof(menu)),sizeof(menu));
    PG_CHECK(w2c_hgengine_HGNativeFieldMenuUIGetState(pgCapture->game)==address && !memcmp(&menu,pgGuest(address,sizeof(menu)),sizeof(menu)));return menu;
}
static HGNativePokegearQAState pgRead(void) {
    u32 address=w2c_hgengine_HGNativePokegearQAGetState(pgCapture->game);HGNativePokegearQAState qa;
    memcpy(&qa,pgGuest(address,sizeof(qa)),sizeof(qa));PG_CHECK(qa.version==HG_PG_QA_VERSION && qa.byteSize==sizeof(qa) && !qa.error);
    PG_CHECK(w2c_hgengine_HGNativePokegearQAGetState(pgCapture->game)==address && !memcmp(&qa,pgGuest(address,sizeof(qa)),sizeof(qa)));
    u32 ui=w2c_hgengine_HGNativePokegearUIGetState(pgCapture->game);memcpy(&qa.ui,pgGuest(ui,sizeof(qa.ui)),sizeof(qa.ui));
    PG_CHECK(w2c_hgengine_HGNativePokegearUIGetState(pgCapture->game)==ui && !memcmp(&qa.ui,pgGuest(ui,sizeof(qa.ui)),sizeof(qa.ui)));pgMenu=pgReadMenu();return qa;
}
static BOOL pgMenuRetired(const HGNativeFieldMenuUIState *menu,u32 generation,u32 revision) {
    return menu && generation && revision && menu->ownerGeneration==generation && menu->revision && menu->revision!=revision
        && menu->phase==UINT32_MAX && !menu->active && !menu->ready && !menu->viewActive && !menu->controlActive && !menu->childActive;
}
static BOOL pgView(const HGNativePokegearQAState *qa,u32 app,u32 phase) {
    return qa && qa->ui.ownerKind==HG_POKEGEAR_OWNER_GEAR && qa->ui.app==app && qa->ui.phase==phase
        && qa->viewFactsGeneration==qa->ui.ownerGeneration && HGNativePokegearUIInputAllowed(&qa->ui,qa->ui.ownerGeneration,qa->ui.revision);
}
static BOOL pgTextCanContinue(const HGNativePokegearQAState *qa,u32 serial,u32 count) {
    return pgView(qa,3,6) && qa->callMainState==1 && qa->callScriptState==3 && !qa->callAwaitHangup && count<2
        && qa->callTextSerial==serial+1 && qa->callTextWait==(count ? 3u : 2u);
}
static void pgTrace(const char *event) {
    printf("POKEGEAR input: {\"event\":\"%s\",\"step\":%u,\"op\":%u,\"key\":%u,\"touch\":%u,\"x\":%u,\"y\":%u,\"padReads\":%u,\"frame\":%u,\"owner\":%u,\"generation\":%u,\"revision\":%u,\"liveActive\":%u,\"liveReady\":%u,\"liveGeneration\":%u,\"liveRevision\":%u,\"leaseEngine\":%u,\"touchEngine\":%u,\"bootKind\":%u,\"bootActive\":%u,\"bootGeneration\":%u,\"bootRevision\":%u,\"bootReady\":%u,\"bootPhase\":%u,\"bootMode\":%u,\"bootOption\":%u,\"bootNotice\":%u,\"bootExit\":%u,\"bootProgress\":%u}\n",
        event,pgStep,pgSteps[pgStep].op,pgKey,pgTouch,pgX,pgY,HGNativeInputPadReads(pgCapture->env),OS_GetVBlankCount(),pgLeaseOwner,pgLeaseGeneration,pgLeaseRevision,
        pgLeaseOwner==1 ? pgFinal.ui.active : pgMenu.active,pgLeaseOwner==1 ? pgFinal.ui.ready : pgMenu.ready,
        pgLeaseOwner==1 ? pgFinal.ui.ownerGeneration : pgMenu.ownerGeneration,pgLeaseOwner==1 ? pgFinal.ui.revision : pgMenu.revision,pgLeaseEngine,pgTouchEngine(),
        pgFinal.boot.kind,pgFinal.boot.active,pgFinal.boot.ownerGeneration,pgFinal.boot.revision,pgFinal.boot.ready,pgFinal.boot.phase,pgFinal.boot.inputMode,pgFinal.boot.option,pgFinal.boot.notice,pgFinal.boot.exit,pgFinal.boot.progressFlags);
}
static void pgMenuTrace(const char *event) {
    printf("POKEGEAR menu: {\"event\":\"%s\",\"step\":%u,\"padReads\":%u,\"frame\":%u,\"snapshot\":%u,\"fieldReady\":%u,\"pokegearActive\":%u,\"closeCount\":%u,\"admittedGeneration\":%u,\"admittedRevision\":%u,\"menu\":{\"active\":%u,\"generation\":%u,\"revision\":%u,\"ready\":%u,\"phase\":%u,\"viewActive\":%u,\"controlActive\":%u,\"childActive\":%u,\"inputMode\":%u}}\n",
        event,pgStep,HGNativeInputPadReads(pgCapture->env),OS_GetVBlankCount(),pgSnapshots-1,pgFinal.fieldReady,pgFinal.ui.active,pgMenuCloses,pgMenuBefore.ownerGeneration,pgMenuBefore.revision,
        pgMenu.active,pgMenu.ownerGeneration,pgMenu.revision,pgMenu.ready,pgMenu.phase,pgMenu.viewActive,pgMenu.controlActive,pgMenu.childActive,pgMenu.inputMode);
}
static void pgTextTrace(const char *event) {
    printf("POKEGEAR text: {\"event\":\"%s\",\"step\":%u,\"serial\":%u,\"wait\":%u,\"continues\":%u,\"callTarget\":%u,\"scriptID\":%u,\"effectType\":%u,\"padReads\":%u,\"generation\":%u,\"revision\":%u}\n",
        event,pgStep,pgLastTextSerial,pgFinal.callTextWait,pgTextContinues,pgFinal.callTarget,pgFinal.callScriptID,pgFinal.callEffectType,HGNativeInputPadReads(pgCapture->env),pgLeaseGeneration,pgLeaseRevision);
}
static void pgRawUp(void) {
    PG_CHECK(HGNativeSetInput(pgCapture->env,0,FALSE,0,0,FALSE));pgReleaseReads=HGNativeInputPadReads(pgCapture->env);pgLease=2;pgTrace("released");
    if(pgMenuCloseStage==1)pgMenuTrace("released");
    if(pgSteps[pgStep].op==PG_CALL_WAIT)pgTextTrace("released");
}
static void pgPulse(u32 key,u32 x,u32 y,BOOL touch,u32 owner) {
    PG_CHECK(!pgLease);pgKey=key;pgX=x;pgY=y;pgTouch=touch;pgLeaseOwner=owner;pgLeaseEngine=pgTouchEngine();
    PG_CHECK(owner!=1 || !pgLeaseEngine);
    pgLeaseGeneration=owner==1 ? pgFinal.ui.ownerGeneration : owner==2 ? pgMenu.ownerGeneration : 0;
    pgLeaseRevision=owner==1 ? pgFinal.ui.revision : owner==2 ? pgMenu.revision : 0;
    PG_CHECK(HGNativeSetInput(pgCapture->env,key,FALSE,x,y,touch));pgPressReads=HGNativeInputPadReads(pgCapture->env);pgLease=1;pgTrace("press");
}
static BOOL pgSettle(void) {
    if(!pgLease)return TRUE;u32 reads=HGNativeInputPadReads(pgCapture->env);
    if(pgLease==1) {
        if((pgLeaseOwner && pgTouchEngine()!=pgLeaseEngine) || (pgLeaseOwner==1 && !HGNativePokegearUIInputAllowed(&pgFinal.ui,pgLeaseGeneration,pgLeaseRevision))
            || (pgLeaseOwner==2 && !HGNativeFieldMenuUIInputAllowed(&pgMenu,pgLeaseGeneration,pgLeaseRevision))) {pgRawUp();return FALSE;}
        if(reads-pgPressReads>=4){pgRawUp();return FALSE;}
    }
    if(pgLease!=2 || reads-pgReleaseReads<2)return FALSE;
    pgTrace("settled");if(pgMenuCloseStage==1)pgMenuTrace("settled");if(pgSteps[pgStep].op==PG_CALL_WAIT)pgTextTrace("settled");pgLease=0;return TRUE;
}
static void pgNext(void) {
    ++pgStep;pgStepFrame=OS_GetVBlankCount();pgFocusSent=pgCloseRequested=pgMenuCloseStage=0;
    pgCloseBefore=(HGNativePokegearUIState){0};pgMenuBefore=(HGNativeFieldMenuUIState){0};
}
static void pgConservation(void) {
    PG_CHECK(pgFinal.started && pgFinal.partyUnchanged && pgFinal.pcUnchanged && pgFinal.bagUnchanged && pgFinal.varsFlagsUnchanged
        && pgFinal.easyChatUnchanged && pgFinal.moneyUnchanged && pgFinal.trainerCardUnchanged && pgFinal.statsUnchanged && pgFinal.miscUnchanged && pgFinal.gearMatched);
    PG_CHECK(pgFinal.gearBytes && pgFinal.gearBytes<=HG_PG_QA_GEAR_CAPACITY && pgFinal.statsBytes && pgFinal.statsBytes<=HG_PG_QA_STATS_CAPACITY
        && !memcmp(pgFinal.gearLive,pgFinal.gearExpected,pgFinal.gearBytes) && !memcmp(pgFinal.statsLive,pgFinal.statsExpected,pgFinal.statsBytes)
        && !pgFinal.registeredCards && pgFinal.currentMap==MAP_ROUTE_29 && pgFinal.callSelections<=1 && pgFinal.callCompletions<=1 && pgFinal.currentMoney==pgFinal.baselineMoney);
}
static BOOL pgSnapshot(void) {
    static u8 pixels[2][256*192*4];u32 rendered=0,varied=0,touchEngine=pgTouchEngine();
    if(!pgCapture->buffer)pgCapture->buffer=w2c_hgengine_HGNativeFrameBuffer(pgCapture->game);
    HGNativeDrainDMA();pgGuest(pgCapture->buffer,sizeof(pixels[0]));
    for(u32 engine=0;engine<2;++engine)if(w2c_hgengine_HGNativeRender2D(pgCapture->game,engine,pgCapture->buffer,sizeof(pixels[0]))) {
        rendered|=1u<<engine;memcpy(pixels[engine],pgGuest(pgCapture->buffer,sizeof(pixels[0])),sizeof(pixels[0]));
        for(u32 i=1;i<256*192;++i)if(memcmp(pixels[engine],pixels[engine]+i*4,4)){varied|=1u<<engine;break;}
    }
    u32 required=pgFinal.ui.active || pgMenu.active ? 3 : 1;if((rendered&varied&required)!=required)return FALSE;
    HGNativeFieldMenuUIState before=pgMenu;HGNativePokegearQAState after=pgRead();
    if(after.revision!=pgFinal.revision || after.snapshotSerial!=pgFinal.snapshotSerial || memcmp(&after.ui,&pgFinal.ui,sizeof(after.ui)) || memcmp(&before,&pgMenu,sizeof(before)) || pgTouchEngine()!=touchEngine)return FALSE;
    char path[4096];PG_CHECK(snprintf(path,sizeof(path),"%s/pokegear-%03u-state.bin",pgCapture->folder,pgSnapshots)<sizeof(path));
    FILE *state=fopen(path,"wbx");PG_CHECK(state && fwrite(&pgFinal,1,sizeof(pgFinal),state)==sizeof(pgFinal) && !fclose(state));
    PG_CHECK(snprintf(path,sizeof(path),"%s/pokegear-%03u-menu.bin",pgCapture->folder,pgSnapshots)<sizeof(path));
    FILE *menu=fopen(path,"wbx");PG_CHECK(menu && fwrite(&pgMenu,1,sizeof(pgMenu),menu)==sizeof(pgMenu) && !fclose(menu));
    for(u32 engine=0;engine<2;++engine)if(rendered&(1u<<engine)) {
        PG_CHECK(snprintf(path,sizeof(path),"%s/pokegear-%03u-engine-%u.ppm",pgCapture->folder,pgSnapshots,engine)<sizeof(path));
        FILE *image=fopen(path,"wbx");PG_CHECK(image && fprintf(image,"P6\n256 192\n255\n")>0);
        for(u32 row=0;row<192;++row){u8 rgb[256*3];for(u32 x=0;x<256;++x)memcpy(rgb+x*3,pixels[engine]+(row*256+x)*4,3);PG_CHECK(fwrite(rgb,1,sizeof(rgb),image)==sizeof(rgb));}PG_CHECK(!fclose(image));
    }
    printf("POKEGEAR snapshot: {\"sequence\":%u,\"step\":%u,\"requiredMask\":%u,\"renderedMask\":%u,\"variedMask\":%u,\"frame\":%u,\"packetFrame\":%u,\"generation\":%u,\"revision\":%u,\"touchEngine\":%u,\"callSelections\":%u,\"callCompletions\":%u,\"savedApp\":%u,\"savedSkin\":%u}\n",
        pgSnapshots,pgStep,required,rendered,varied,OS_GetVBlankCount(),pgFinal.frame,pgFinal.ui.ownerGeneration,pgFinal.ui.revision,touchEngine,pgFinal.callSelections,pgFinal.callCompletions,pgFinal.savedApp,pgFinal.savedSkin);
    ++pgSnapshots;return TRUE;
}
static void pgRequest(u32 request) {
    PG_CHECK(!pgPendingRequest && pgFinal.revision);
    PG_CHECK(!w2c_hgengine_HGNativePokegearQARequest(pgCapture->game,pgFinal.revision-1,request));++pgStaleRejected;
    PG_CHECK(w2c_hgengine_HGNativePokegearQARequest(pgCapture->game,pgFinal.revision,request));
    PG_CHECK(!w2c_hgengine_HGNativePokegearQARequest(pgCapture->game,pgFinal.revision,request));pgPendingRequest=request;
}
static void pgKeyAction(u32 key) {
    u32 resolved=0,sentinel=123;PG_CHECK(!HGNativePokegearUIResolveKey(&pgFinal.ui,pgFinal.ui.ownerGeneration,pgFinal.ui.revision-1,key,&sentinel) && sentinel==123);++pgStaleRejected;
    PG_CHECK(HGNativePokegearUIResolveKey(&pgFinal.ui,pgFinal.ui.ownerGeneration,pgFinal.ui.revision,key,&resolved));pgPulse(resolved,0,0,FALSE,1);
}

static BOOL pgBootEligible(const HGYNBootFacts *b) {
    if(!b || b->active!=1 || b->ready!=1 || !b->ownerGeneration || !b->revision)return FALSE;
    if(b->kind==HG_YN_BOOT_TITLE)return b->phase==2 && !b->inputMode && !b->option && !b->notice && !b->exit && (b->progressFlags&15)==15;
    return b->kind==HG_YN_BOOT_MAIN_MENU && b->phase==5 && b->inputMode<=1 && b->option<9 && b->notice==15 && !b->exit && (b->progressFlags&255)==255;
}

static BOOL pgBootSameSwitchedOwner(const HGYNBootFacts *b,u32 generation,u32 revision) {
    return pgBootEligible(b) && b->kind==HG_YN_BOOT_MAIN_MENU && b->inputMode==0 && generation && revision
        && b->ownerGeneration==generation && b->revision!=revision;
}

static BOOL pgBootExitWitness(const HGYNBootFacts *b,u32 generation) {
    return b && b->kind==HG_YN_BOOT_MAIN_MENU && b->active==0 && b->ready==0 && b->phase==8
        && generation && b->ownerGeneration==generation && b->revision && b->exit==1;
}

static BOOL pgBootTitleExitWitness(const HGYNBootFacts *b,u32 generation) {
    return b && b->kind==HG_YN_BOOT_TITLE && b->active==0 && b->ready==0 && b->phase==6
        && generation && b->ownerGeneration==generation && b->revision && b->exit==1;
}

static void PGQAPresent(void *argument) {
    pgCapture=argument;pgFinal=pgRead();u32 frame=OS_GetVBlankCount();
    if(!pgStepFrame)pgStepFrame=frame;if(frame-pgStepFrame>(pgStep==0 ? 12000u : 2400u))pgFinish("stage_timeout",FALSE);
    if(pgTitleSent && pgBootTitleExitWitness(&pgFinal.boot,pgTitleGeneration))pgTitleExit=1;
    if(pgContinueSent && pgBootExitWitness(&pgFinal.boot,pgContinueGeneration))pgContinueExit=1;
    if(pgSwitchSent && pgBootSameSwitchedOwner(&pgFinal.boot,pgSwitchGeneration,pgSwitchRevision))pgModeSwitchSeen=1;
    BOOL settling=pgLease!=0;if(!pgSettle())return;
    if(settling && pgSteps[pgStep].op!=PG_BOOT) {
        if(pgSteps[pgStep].op==PG_FIELD){PG_CHECK(pgMenuCloseStage==1);pgMenuCloseStage=2;}
        else if(pgSteps[pgStep].op!=PG_FOCUS && pgSteps[pgStep].op!=PG_CLOSE && pgSteps[pgStep].op!=PG_CALL_WAIT){pgNext();return;}
    }
    PG_CHECK(pgStep<sizeof(pgSteps)/sizeof(pgSteps[0]));PGStep step=pgSteps[pgStep];
    if(step.op==PG_BOOT) {
        HGYNBootFacts *b=&pgFinal.boot;
        if(pgFinal.fieldReady){PG_CHECK(pgContinueSent && pgContinueExit && pgBootExitWitness(b,pgContinueGeneration) && (!pgTitleSent || pgTitleExit));if(!pgSnapshot())return;pgNext();return;}
        if(!pgBootEligible(b))return;
        if(b->kind==HG_YN_BOOT_TITLE){if(pgTitleSent)return;}
        else if(b->inputMode){if(pgSwitchSent || pgContinueSent)return;}
        else {if(pgContinueSent)return;PG_CHECK(!pgSwitchSent || (pgModeSwitchSeen && pgBootSameSwitchedOwner(b,pgSwitchGeneration,pgSwitchRevision)));}
        if(!pgSnapshot())return;
        if(b->kind==HG_YN_BOOT_TITLE){pgTitleSent=1;pgTitleGeneration=b->ownerGeneration;}
        else if(b->inputMode){pgSwitchSent=1;pgSwitchGeneration=b->ownerGeneration;pgSwitchRevision=b->revision;}
        else {pgContinueSent=1;pgContinueGeneration=b->ownerGeneration;}pgPulse(1,0,0,FALSE,0);return;
    }
    if(pgFinal.started)pgConservation();
    if(step.op==PG_START) {
        if(!pgFinal.fieldReady || pgFinal.pokegearSlot>=7)return;
        if(!pgPendingRequest){PG_CHECK(pgFinal.compatibleSave && !pgFinal.ringActive && !pgFinal.panicFlag);pgRequest(HG_PG_QA_START);return;}
        PG_CHECK(pgPendingRequest==HG_PG_QA_START);if(!pgFinal.started)return;pgPendingRequest=0;pgNext();return;
    }
    if(step.op==PG_MENU_OPEN){if(!pgFinal.fieldReady || pgFinal.ui.active || pgMenu.active)return;if(!pgSnapshot())return;pgPulse(1024,0,0,FALSE,0);return;}
    if(step.op==PG_ICON) {
        if(pgFinal.ui.active || pgFinal.pokegearSlot>=7 || !HGNativeFieldMenuUIInputAllowed(&pgMenu,pgMenu.ownerGeneration,pgMenu.revision))return;
        PG_CHECK(((pgFinal.menuIcons>>(pgFinal.pokegearSlot*4))&15)==3 && HGNativeFieldMenuUITouchAllowed(&pgMenu,pgMenu.ownerGeneration,pgMenu.revision,pgFinal.pokegearX,pgFinal.pokegearY));
        if(!pgSnapshot())return;pgPulse(0,pgFinal.pokegearX,pgFinal.pokegearY,TRUE,2);return;
    }
    if(step.op==PG_FIELD) {
        if(pgFinal.ui.active)return;
        if(pgMenu.active) {
            if(!pgMenuCloseStage && pgMenu.inputMode==0 && HGNativeFieldMenuUIInputAllowed(&pgMenu,pgMenu.ownerGeneration,pgMenu.revision)) {
                if(!pgSnapshot())return;pgMenuBefore=pgMenu;u32 key=0,sentinel=123;
                PG_CHECK(!HGNativeFieldMenuUIResolveKey(&pgMenu,pgMenu.ownerGeneration,pgMenu.revision-1,2,&sentinel) && sentinel==123);++pgStaleRejected;
                PG_CHECK(HGNativeFieldMenuUIResolveKey(&pgMenu,pgMenu.ownerGeneration,pgMenu.revision,2,&key));pgMenuCloseStage=1;++pgMenuCloseRequests;pgMenuTrace("admitted");pgPulse(key,0,0,FALSE,2);
            }
            return;
        }
        if(pgMenuCloseStage!=2 && pgMenuCloseStage!=3)return;
        if(!pgMenuRetired(&pgMenu,pgMenuBefore.ownerGeneration,pgMenuBefore.revision) || !pgFinal.fieldReady)return;
        PG_CHECK(pgFinal.parentExits==step.a && pgFinal.savedApp==0 && pgFinal.savedSkin==1);
        if(pgMenuCloseStage==2){if(!pgSnapshot())return;++pgMenuCloses;pgMenuCloseStage=3;pgMenuTrace("closed_field_ready");}
        if(!pgPendingRequest){pgRequestedCheckpoint=pgFinal.checkpoints+1;pgRequest(HG_PG_QA_CHECKPOINT);return;}
        PG_CHECK(pgPendingRequest==HG_PG_QA_CHECKPOINT);if(pgFinal.checkpoints<pgRequestedCheckpoint)return;
        PG_CHECK(pgFinal.checkpoints==pgRequestedCheckpoint);pgPendingRequest=0;pgNext();return;
    }
    if(step.op==PG_FINISH) {
        if(pgFinal.stage!=HG_PG_QA_DONE){if(pgPendingRequest)return;PG_CHECK(pgFinal.fieldReady && !pgMenu.active && !pgFinal.ui.active && pgMenuCloses==2);pgRequest(HG_PG_QA_FINISH);return;}
        PG_CHECK(pgPendingRequest==HG_PG_QA_FINISH);pgPendingRequest=0;pgFinish("passed_original_pokegear_phone_configure",TRUE);
    }
    if(step.op==PG_CLOSE) {
        if(!pgFinal.ui.active) {
            PG_CHECK(pgCloseRequested && pgFinal.ui.ownerGeneration==pgCloseBefore.ownerGeneration && pgFinal.ui.revision && pgFinal.ui.revision!=pgCloseBefore.revision && !pgFinal.ui.ready && !pgFinal.ui.viewActive);
            pgNext();return;
        }
        if(pgCloseRequested || !pgView(&pgFinal,0,1))return;
        if(!pgFinal.cursorInAppSwitchZone){if(pgFocusSent)return;if(!pgSnapshot())return;pgFocusSent=1;pgKeyAction(2);return;}
        if(!pgSnapshot())return;pgCloseRequested=1;pgCloseBefore=pgFinal.ui;pgKeyAction(2);return;
    }
    if(!HGNativePokegearUIInputAllowed(&pgFinal.ui,pgFinal.ui.ownerGeneration,pgFinal.ui.revision) || pgFinal.viewFactsGeneration!=pgFinal.ui.ownerGeneration)return;
    if(step.op==PG_VIEW){if(!pgView(&pgFinal,step.a,1) || pgFinal.configuredSkin!=step.b)return;if(!pgSnapshot())return;pgNext();return;}
    if(step.op==PG_LOCKED) {
        if(!pgView(&pgFinal,3,1))return;PG_CHECK(!pgFinal.ui.registeredCards);
        PG_CHECK(!HGNativePokegearUITouchStartAllowed(&pgFinal.ui,pgFinal.ui.ownerGeneration,pgFinal.ui.revision,80,176)
            && !HGNativePokegearUITouchStartAllowed(&pgFinal.ui,pgFinal.ui.ownerGeneration,pgFinal.ui.revision,128,176));
        if(!pgSnapshot())return;pgLockedRejected+=2;pgNext();return;
    }
    if(step.op==PG_TOUCH || step.op==PG_CONTACT) {
        u32 x=step.a,y=step.b;
        if(step.op==PG_CONTACT){if(!pgView(&pgFinal,3,1))return;PG_CHECK(pgFinal.contactCount==2 && pgFinal.slotContacts[0]==0 && pgFinal.slotContacts[1]==1);x=116;y=44;}
        PG_CHECK(HGNativePokegearUITouchStartAllowed(&pgFinal.ui,pgFinal.ui.ownerGeneration,pgFinal.ui.revision,x,y));
        if(!pgSnapshot())return;pgPulse(0,x,y,TRUE,1);return;
    }
    if(step.op==PG_CONTEXT) {
        if(!pgView(&pgFinal,step.a,step.a==0 ? 3 : 7))return;PG_CHECK(pgFinal.menuCursor==0);
        PG_CHECK(step.a==0 ? pgFinal.selectedSkin==step.b : pgFinal.selectedContact==step.b);
        if(!pgSnapshot())return;pgNext();return;
    }
    if(step.op==PG_FOCUS){if(!pgView(&pgFinal,0,1))return;if(pgFinal.cursorInAppSwitchZone){pgNext();return;}if(pgFocusSent)return;if(!pgSnapshot())return;pgFocusSent=1;pgKeyAction(2);return;}
    if(step.op==PG_FOOTER){if(!pgFinal.cursorInAppSwitchZone || pgFinal.footerApp!=step.a)return;if(!pgSnapshot())return;pgNext();return;}
    if(step.op==PG_KEY){if(!pgSnapshot())return;pgKeyAction(step.a);return;}
    if(step.op==PG_CALL_WAIT) {
        if(!pgView(&pgFinal,3,6))return;
        PG_CHECK(pgFinal.callSelections==1 && !pgFinal.callCompletions && pgFinal.callTarget==1 && pgFinal.callScriptID==8 && !pgFinal.callScriptType && !pgFinal.callEffectType && !pgFinal.callIncoming && !pgFinal.callScripted);
        if(pgFinal.callAwaitHangup) {PG_CHECK(pgFinal.callMainState==2 && pgTextContinues==2 && pgFinal.callTextSerial==2);if(!pgSnapshot())return;pgNext();return;}
        if(!pgFinal.callTextWait || pgFinal.callTextSerial==pgLastTextSerial)return;
        PG_CHECK(pgTextCanContinue(&pgFinal,pgLastTextSerial,pgTextContinues));
        if(!pgSnapshot())return;pgLastTextSerial=pgFinal.callTextSerial;++pgTextContinues;pgKeyAction(1);pgTextTrace("admitted");return;
    }
    if(step.op==PG_CALL_DONE){if(!pgView(&pgFinal,3,1) || pgFinal.callCompletions!=1)return;PG_CHECK(pgFinal.callSelections==1 && pgFinal.callTarget==1 && !pgFinal.callEffectType);if(!pgSnapshot())return;pgNext();return;}
}

int main(int argc,char **argv) {
    setvbuf(stdout,NULL,_IONBF,0);
    if (argc!=5 || !HGNativeFSSetRoot(argv[1]) || !HGNativeBackupSetPath(argv[3])) return 2;
    u8 header[0x160];FILE *input=fopen(argv[2],"rb");if (!input) return 2;
    BOOL valid=fread(header,1,sizeof(header),input)==sizeof(header) && fgetc(input)==EOF && !ferror(input);fclose(input);if (!valid) return 2;
    wasm_rt_init();w2c_hgengine *game=calloc(1,sizeof(*game));struct w2c_env *env=calloc(1,sizeof(*env));
    struct w2c_wasi__snapshot__preview1 *wasi=calloc(1,sizeof(*wasi));Capture *capture=calloc(1,sizeof(*capture));
    if (!game || !env || !wasi || !capture) return 2;
    wasi->fds[0]=dup(0);wasi->fds[1]=dup(1);wasi->fds[2]=dup(2);if (wasi->fds[0]<0 || wasi->fds[1]<0 || wasi->fds[2]<0) return 2;
    pgCapture=capture;capture->game=game;capture->env=env;
    if (!setjmp(pgResetLanding)) {
        wasm_rt_trap_t trap=wasm_rt_impl_try();PG_CHECK(!trap);
        wasm2c_hgengine_instantiate(game,env,wasi);env->memory=wasi->memory=w2c_hgengine_memory(game);PG_CHECK(HGNativeSoundRealtime(env,TRUE));
        env->irqTable=&game->w2c_T0;env->stackPointer=&game->w2c_0x5F_stack_pointer;env->rootStackHigh=*env->stackPointer;*env->stackPointer-=16;
        env->irqType0=wasm2c_hgengine_get_func_type(0,0);env->irqType1=wasm2c_hgengine_get_func_type(1,0,WASM_RT_I32);
        env->rtcType=wasm2c_hgengine_get_func_type(2,0,WASM_RT_I32,WASM_RT_I32);env->irqGuardType=wasm2c_hgengine_get_func_type(4,0,WASM_RT_I32,WASM_RT_I32,WASM_RT_I32,WASM_RT_I32);
        u32 system=w2c_hgengine_HGNativeSystemWorkAddress(game);memcpy((void *)pgGuest(system,0x360)+0x200,header,sizeof(header));
        w2c_hgengine_0x5Finitialize(game);u32 status=w2c_hgengine_HGNativeRegisterAddress(game,0x04000004,2);PG_CHECK(HGNativeStartDisplayClock(env,pgGuest(status,2)));
        *capture=(Capture){.game=game,.env=env,.folder=argv[4],.resume=TRUE,.noInput=TRUE};
        PG_CHECK(HGNativeSetDisplayObserver(env,PGQAPresent,capture));w2c_hgengine_NitroMain(game);PG_CHECK(FALSE);
    }
    BOOL quiescent=FALSE;
    if (pgResetExpected && env->resetRequested) {HGNativeFinishReset(env);quiescent=env->resetFinished && !env->pendingDMA && !env->dmaAffineWrites && !env->pendingRTC
        && !env->pendingMIC && !env->pendingCARD && !env->pendingPOWER && !env->pendingINPUT && !env->irqTable && !env->threads && !env->rasterFunction;}
    printf("POKEGEAR evidence: {\"status\":\"%s\",\"qaVersion\":%u,\"qaBytes\":%u,\"step\":%u,\"totalSteps\":%zu,\"parentEntries\":%u,\"parentExits\":%u,\"phoneEntries\":%u,\"configureEntries\":%u,\"callSelections\":%u,\"callCompletions\":%u,\"textContinues\":%u,\"callTextSerial\":%u,\"callTarget\":%u,\"callScriptID\":%u,\"callEffectType\":%u,\"savedApp\":%u,\"savedSkin\":%u,\"cards\":%u,\"checkpoints\":%u,\"staleRejected\":%u,\"lockedRejected\":%u,\"snapshots\":%u,\"titleExit\":%u,\"continueExit\":%u,\"modeSwitchSeen\":%u,\"menuCloseRequests\":%u,\"menuCloses\":%u,\"terminalFieldReady\":%u,\"menuActive\":%u,\"coverage\":%u,\"privateSaveWrites\":false,\"resetQuiescent\":%s}\n",
        pgStatus,pgFinal.version,pgFinal.byteSize,pgStep,sizeof(pgSteps)/sizeof(pgSteps[0]),pgFinal.parentEntries,pgFinal.parentExits,pgFinal.phoneEntries,pgFinal.configureEntries,pgFinal.callSelections,pgFinal.callCompletions,pgTextContinues,pgFinal.callTextSerial,pgFinal.callTarget,pgFinal.callScriptID,pgFinal.callEffectType,pgFinal.savedApp,pgFinal.savedSkin,pgFinal.registeredCards,pgFinal.checkpoints,pgStaleRejected,pgLockedRejected,pgSnapshots,pgTitleExit,pgContinueExit,pgModeSwitchSeen,pgMenuCloseRequests,pgMenuCloses,pgFinal.fieldReady,pgMenu.active,pgFinal.coverage,quiescent ? "true" : "false");
    BOOL passed=pgPassed && quiescent && pgStep+1==sizeof(pgSteps)/sizeof(pgSteps[0]) && pgFinal.parentEntries==2 && pgFinal.parentExits==2
        && pgFinal.phoneEntries==2 && pgFinal.configureEntries==3 && pgFinal.callSelections==1 && pgFinal.callCompletions==1 && pgFinal.callTarget==1
        && pgTextContinues==2 && pgFinal.callTextSerial==2 && pgFinal.callScriptID==8 && !pgFinal.callEffectType && pgFinal.savedApp==0 && pgFinal.savedSkin==1 && !pgFinal.registeredCards
        && pgFinal.coverage==63 && pgFinal.checkpoints==2 && pgMenuCloseRequests==2 && pgMenuCloses==2 && pgFinal.fieldReady && !pgMenu.active && !pgFinal.ui.active;
    wasm2c_hgengine_free(game);for(u32 i=0;i<3;++i)close(wasi->fds[i]);free(game);free(env);free(wasi);free(capture);wasm_rt_free();return passed ? 0 : 1;
}
