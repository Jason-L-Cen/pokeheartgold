// SPDX-License-Identifier: GPL-3.0-or-later
#define main PCBoxBootstrapMain
#define HGEngineTrap PCBoxBootstrapTrap
#include "engine_check.c"
#undef HGEngineTrap
#undef main
#include "../../tests/pc_box_qa.h"
#include "constants/items.h"
#include "native_field_choice_ui.h"
#include <nitro/os/native_reset.h>
#include <setjmp.h>

enum { OP_START,OP_ENTRY,OP_MENU,OP_GRID,OP_KEY,OP_WAIT,OP_CHECK,OP_SUMMARY,OP_NAMING,OP_RETURN,OP_FINISH };
typedef struct PCBoxStep { u32 kind,phase,target,bit,party,extra,pidgey,rattata; } PCBoxStep;
#define GRID(phase,target) {OP_GRID,phase,target}
#define KEY(phase,key,handler) {OP_KEY,phase,key,handler}
#define MENU(value) {OP_MENU,0,value}
#define WAIT(phase) {OP_WAIT,phase}
#define CHECKPOINT(phase,bit,party,extra,pidgey,rattata) {OP_CHECK,phase,0,bit,party,extra,pidgey,rattata}
static const PCBoxStep steps[]={
    {OP_START},{OP_ENTRY},MENU(0),MENU(0),
    GRID(HG_PC_BOX_UI_DEPOSIT_INPUT,1),GRID(HG_PC_BOX_UI_DEPOSIT_INPUT,7),GRID(HG_PC_BOX_UI_DEPOSIT_BOX_INPUT,8),
    CHECKPOINT(HG_PC_BOX_UI_DEPOSIT_INPUT,HG_PC_QA_PHYSICAL|HG_PC_QA_DEPOSIT,1,0,2,29),
    GRID(HG_PC_BOX_UI_DEPOSIT_INPUT,0),GRID(HG_PC_BOX_UI_DEPOSIT_INPUT,7),
    CHECKPOINT(HG_PC_BOX_UI_WAIT_BUTTON,HG_PC_QA_LAST,1,0,2,29),
    KEY(HG_PC_BOX_UI_WAIT_BUTTON,2,0),WAIT(HG_PC_BOX_UI_DEPOSIT_INPUT),KEY(HG_PC_BOX_UI_DEPOSIT_INPUT,2,0),
    GRID(HG_PC_BOX_UI_DEPOSIT_INPUT,6),KEY(HG_PC_BOX_UI_HANDLE_YES_NO,1,3),MENU(1),
    GRID(HG_PC_BOX_UI_WITHDRAW_INPUT,0),GRID(HG_PC_BOX_UI_WITHDRAW_INPUT,34),
    CHECKPOINT(HG_PC_BOX_UI_WITHDRAW_INPUT,HG_PC_QA_WITHDRAW,2,HG_PC_QA_PARTY_LOCATION+1,2,29),
    GRID(HG_PC_BOX_UI_WITHDRAW_INPUT,33),KEY(HG_PC_BOX_UI_HANDLE_YES_NO,1,3),MENU(2),
    GRID(HG_PC_BOX_UI_MOVE_MODE_INPUT,2),GRID(HG_PC_BOX_UI_MOVE_MODE_INPUT,36),GRID(HG_PC_BOX_UI_BOX_TRAY_HOLDING_INPUT,14),
    CHECKPOINT(HG_PC_BOX_UI_BOX_TRAY_INPUT,HG_PC_QA_MOVE,2,HG_PC_QA_PARTY_LOCATION+1,14,29),
    KEY(HG_PC_BOX_UI_BOX_TRAY_INPUT,2,0),WAIT(HG_PC_BOX_UI_MOVE_MODE_INPUT),
    GRID(HG_PC_BOX_UI_MOVE_MODE_INPUT,14),GRID(HG_PC_BOX_UI_MOVE_MODE_INPUT,36),
    GRID(HG_PC_BOX_UI_BOX_TRAY_HOLDING_INPUT,29),
    CHECKPOINT(HG_PC_BOX_UI_BOX_TRAY_INPUT,HG_PC_QA_SWAP,2,HG_PC_QA_PARTY_LOCATION+1,29,14),
    KEY(HG_PC_BOX_UI_BOX_TRAY_INPUT,2,0),WAIT(HG_PC_BOX_UI_MOVE_MODE_INPUT),
    GRID(HG_PC_BOX_UI_MOVE_MODE_INPUT,14),GRID(HG_PC_BOX_UI_MOVE_MODE_INPUT,37),{OP_SUMMARY},
    CHECKPOINT(HG_PC_BOX_UI_MOVE_MODE_INPUT,HG_PC_QA_SUMMARY,2,HG_PC_QA_PARTY_LOCATION+1,29,14),
    GRID(HG_PC_BOX_UI_MOVE_MODE_INPUT,40),KEY(HG_PC_BOX_UI_HANDLE_YES_NO,2,1),
    CHECKPOINT(HG_PC_BOX_UI_MOVE_MODE_INPUT,HG_PC_QA_RELEASE_CANCEL,2,HG_PC_QA_PARTY_LOCATION+1,29,14),
    KEY(HG_PC_BOX_UI_MOVE_MODE_INPUT,2,0),GRID(HG_PC_BOX_UI_MOVE_MODE_INPUT,30),GRID(HG_PC_BOX_UI_BOX_MENU_INPUT,10),{OP_NAMING},
    CHECKPOINT(HG_PC_BOX_UI_BOX_MENU_INPUT,HG_PC_QA_NAMING,2,HG_PC_QA_PARTY_LOCATION+1,29,14),
    KEY(HG_PC_BOX_UI_BOX_MENU_INPUT,2,0),GRID(HG_PC_BOX_UI_MOVE_MODE_INPUT,35),KEY(HG_PC_BOX_UI_HANDLE_YES_NO,1,3),MENU(3),
    GRID(HG_PC_BOX_UI_ITEM_MODE_INPUT,34),GRID(HG_PC_BOX_UI_ITEM_MODE_BOX_TRAY_INPUT,29),GRID(HG_PC_BOX_UI_ITEM_TRAY_HOLDING_INPUT,14),
    CHECKPOINT(HG_PC_BOX_UI_ITEM_MODE_BOX_TRAY_INPUT,HG_PC_QA_ITEMS,2,HG_PC_QA_PARTY_LOCATION+1,29,14),
    KEY(HG_PC_BOX_UI_ITEM_MODE_BOX_TRAY_INPUT,2,0),GRID(HG_PC_BOX_UI_ITEM_MODE_INPUT,35),KEY(HG_PC_BOX_UI_HANDLE_YES_NO,1,3),
    MENU(5),{OP_MENU,0,UINT32_MAX},{OP_RETURN},
    CHECKPOINT(0,0,2,HG_PC_QA_PARTY_LOCATION+1,29,14),{OP_FINISH}
};
#undef GRID
#undef KEY
#undef MENU
#undef WAIT
#undef CHECKPOINT

static jmp_buf pcResetLanding;
static Capture *pcCapture;
static BOOL pcResetExpected,pcPassed;
static const char *pcStatus="not_started";
static u32 pcStep,pcStepFrame,pcSubstep,pcCoverage,pcStaleRejected,pcBusyRejected,pcSnapshots;
static u32 pcPressReads,pcReleaseReads,pcLease,pcMenuSerial,pcCheckpoint,pcPreparationCalls;
static u32 pcOldGeneration,pcOldRevision,pcOldSummaryRevision;
static u32 pcFieldMessageAcks,pcMessageAckLatched;
static const char *pcLeaseOwner;
static HGPCBoxQAState pcFinal;
_Static_assert(sizeof(HGPCBoxQAState)==1416 && offsetof(HGPCBoxQAState,messageAwaitInput)==1368
    && offsetof(HGPCBoxQAState,boot)==1372,"private PC QA version 3 append-only ABI");

void HGEngineTrap(wasm_rt_trap_t code) {
    if (code==WASM_RT_TRAP_UNREACHABLE && pcResetExpected && pcCapture && pcCapture->env->resetRequested) longjmp(pcResetLanding,1);
    PCBoxBootstrapTrap(code);
}
static void pcFinish(const char *status,BOOL passed) {
    pcStatus=status; pcPassed=passed;
    if (pcCapture && pcCapture->env && pcCapture->env->memory) {
        HGNativeSetInput(pcCapture->env,0,FALSE,0,0,FALSE);
        pcResetExpected=TRUE; w2c_env_HGNativeResetControl(pcCapture->env,HG_RESET_REQUEST,0);
    }
    fprintf(stderr,"PC Box failure before reset: %s\n",status); _Exit(1);
}
#define PC_CHECK(value) do { if (!(value)) { fprintf(stderr,"PC Box failure line %d step %u: %s\n",__LINE__,pcStep,#value); pcFinish("assertion_failed",FALSE); } } while (0)
static const void *pcGuest(u32 address,size_t size) {
    PC_CHECK(address && address<=pcCapture->env->memory->size && size<=pcCapture->env->memory->size-address);
    return pcCapture->env->memory->data+address;
}
// Host boot gate begin. The typed values are private source-observer facts, not production exports.
typedef HGYNBootFacts PCBootFacts;
enum { PC_BOOT_TITLE_READY_PHASE=2,PC_BOOT_MAIN_READY_PHASE=5,PC_BOOT_CONTINUE_APP=1,
    PC_BOOT_TERMINAL_PHASE=8,PC_BOOT_TITLE_PROGRESS_REQUIRED=0xF,PC_BOOT_MAIN_PROGRESS_REQUIRED=0xFF,
    PC_BOOT_START_FRAME=450,PC_BOOT_TIMEOUT_FRAMES=2400 };
static u32 pcBootStart,pcBootLease,pcBootReads,pcBootRelease,pcBootTitleSent,pcBootSwitchSent,pcBootConfirmSent,pcBootExited,pcBootFieldReady;
static u32 pcBootSwitchGeneration,pcBootSwitchRevision,pcBootContinueGeneration;
static u32 pcBootRendered,pcBootVaried;
static const char *pcBootRole;
static BOOL pcBootEligible(const PCBootFacts *facts) {
    if (!facts || facts->active!=1 || !facts->ownerGeneration || !facts->revision || facts->ready!=1) return FALSE;
    if (facts->kind==HG_YN_BOOT_TITLE)
        return facts->phase==PC_BOOT_TITLE_READY_PHASE && !facts->inputMode && !facts->option && !facts->notice && !facts->exit
            && (facts->progressFlags&PC_BOOT_TITLE_PROGRESS_REQUIRED)==PC_BOOT_TITLE_PROGRESS_REQUIRED;
    return facts->kind==HG_YN_BOOT_MAIN_MENU && facts->phase==PC_BOOT_MAIN_READY_PHASE && facts->inputMode<=1
        && facts->option<9 && facts->notice==15 && facts->exit==0
        && (facts->progressFlags&PC_BOOT_MAIN_PROGRESS_REQUIRED)==PC_BOOT_MAIN_PROGRESS_REQUIRED;
}
static BOOL pcBootContinuationAllowed(const PCBootFacts *facts,u32 generation,u32 revision) {
    return pcBootEligible(facts) && facts->kind==HG_YN_BOOT_MAIN_MENU && !facts->inputMode
        && (!generation || (facts->ownerGeneration==generation && facts->revision!=revision));
}
static BOOL pcBootExitWitness(const PCBootFacts *facts,u32 generation) {
    return facts && facts->kind==HG_YN_BOOT_MAIN_MENU && facts->active==0 && facts->ready==0
        && facts->phase==PC_BOOT_TERMINAL_PHASE && generation && facts->ownerGeneration==generation && facts->revision
        && facts->exit==PC_BOOT_CONTINUE_APP;
}
static BOOL pcBootSelfCheck(void) {
#define PC_BOOT_REQUIRE(value) do { if (!(value)) return FALSE; } while (0)
    PCBootFacts title={HG_YN_BOOT_TITLE,1,1,1,1,PC_BOOT_TITLE_READY_PHASE,0,0,0,0,PC_BOOT_TITLE_PROGRESS_REQUIRED};
    PCBootFacts menu={HG_YN_BOOT_MAIN_MENU,1,2,1,1,PC_BOOT_MAIN_READY_PHASE,0,0,15,0,PC_BOOT_MAIN_PROGRESS_REQUIRED};
    PC_BOOT_REQUIRE(pcBootEligible(&title) && pcBootEligible(&menu));
    PC_BOOT_REQUIRE(!pcBootEligible(NULL));
    title.active=0; PC_BOOT_REQUIRE(!pcBootEligible(&title)); title.active=1;
    title.active=2; PC_BOOT_REQUIRE(!pcBootEligible(&title)); title.active=1;
    title.ownerGeneration=0; PC_BOOT_REQUIRE(!pcBootEligible(&title)); title.ownerGeneration=1;
    title.ready=0; PC_BOOT_REQUIRE(!pcBootEligible(&title)); title.ready=1;
    title.ready=2; PC_BOOT_REQUIRE(!pcBootEligible(&title)); title.ready=1;
    title.revision=0; PC_BOOT_REQUIRE(!pcBootEligible(&title)); title.revision=1;
    title.phase=PC_BOOT_TITLE_READY_PHASE+1; PC_BOOT_REQUIRE(!pcBootEligible(&title)); title.phase=PC_BOOT_TITLE_READY_PHASE;
    for (u32 bit=1;bit<=PC_BOOT_TITLE_PROGRESS_REQUIRED;bit<<=1) {
        title.progressFlags=PC_BOOT_TITLE_PROGRESS_REQUIRED&~bit; PC_BOOT_REQUIRE(!pcBootEligible(&title));
    }
    title.progressFlags=PC_BOOT_TITLE_PROGRESS_REQUIRED;
    menu.inputMode=2; PC_BOOT_REQUIRE(!pcBootEligible(&menu)); menu.inputMode=1;
    menu.option=9; PC_BOOT_REQUIRE(!pcBootEligible(&menu)); menu.option=0;
    menu.notice=14; PC_BOOT_REQUIRE(!pcBootEligible(&menu)); menu.notice=15;
    for (u32 bit=1;bit<=PC_BOOT_MAIN_PROGRESS_REQUIRED;bit<<=1) {
        menu.progressFlags=PC_BOOT_MAIN_PROGRESS_REQUIRED&~bit; PC_BOOT_REQUIRE(!pcBootEligible(&menu));
    }
    menu.progressFlags=PC_BOOT_MAIN_PROGRESS_REQUIRED;
    menu.kind=99; PC_BOOT_REQUIRE(!pcBootEligible(&menu)); menu.kind=HG_YN_BOOT_MAIN_MENU;
    menu.inputMode=0;
    PCBootFacts retired=menu; retired.active=0; retired.ready=0; retired.phase=PC_BOOT_TERMINAL_PHASE; retired.exit=PC_BOOT_CONTINUE_APP;
    PC_BOOT_REQUIRE(pcBootContinuationAllowed(&menu,0,0) && pcBootContinuationAllowed(&menu,2,2));
    PC_BOOT_REQUIRE(!pcBootContinuationAllowed(&menu,3,2) && !pcBootContinuationAllowed(&menu,2,1));
    PC_BOOT_REQUIRE(pcBootExitWitness(&retired,2) && !pcBootExitWitness(&retired,3) && !pcBootExitWitness(&retired,0));
    retired.active=1; PC_BOOT_REQUIRE(!pcBootExitWitness(&retired,2));
    retired.active=0; retired.phase=PC_BOOT_TERMINAL_PHASE-1; PC_BOOT_REQUIRE(!pcBootExitWitness(&retired,2)); retired.phase=PC_BOOT_TERMINAL_PHASE;
    retired.phase=PC_BOOT_TERMINAL_PHASE+1; PC_BOOT_REQUIRE(!pcBootExitWitness(&retired,2)); retired.phase=PC_BOOT_TERMINAL_PHASE;
    retired.ready=1; PC_BOOT_REQUIRE(!pcBootExitWitness(&retired,2)); retired.ready=0;
    retired.ownerGeneration=0; PC_BOOT_REQUIRE(!pcBootExitWitness(&retired,2)); retired.ownerGeneration=2;
    retired.revision=0; PC_BOOT_REQUIRE(!pcBootExitWitness(&retired,2)); retired.revision=1;
    retired.exit=0; PC_BOOT_REQUIRE(!pcBootExitWitness(&retired,2));
#undef PC_BOOT_REQUIRE
    return TRUE;
}
// Pure boot predicate end.
static PCBootFacts pcBootObserve(void) {
    return pcFinal.boot;
}
static void pcBootEvidence(const char *event,const PCBootFacts *facts) {
    printf("PC Box boot: {\"event\":\"%s\",\"owner\":\"boot\",\"role\":\"%s\",\"frame\":%u,\"padReads\":%u,\"inputStep\":%u,"
        "\"boot\":{\"kind\":%u,\"active\":%u,\"generation\":%u,\"revision\":%u,\"ready\":%u,\"phase\":%u,"
        "\"inputMode\":%u,\"option\":%u,\"notice\":%u,\"exit\":%u,\"progress\":%u},"
        "\"rendered\":%u,\"varied\":%u}\n",
        event,pcBootRole ? pcBootRole : "waiting",OS_GetVBlankCount(),HGNativeInputPadReads(pcCapture->env),pcCapture->inputStep,
        facts->kind,facts->active,facts->ownerGeneration,facts->revision,facts->ready,facts->phase,facts->inputMode,facts->option,
        facts->notice,facts->exit,facts->progressFlags,pcBootRendered,pcBootVaried);
}
static BOOL pcBootPixels(const PCBootFacts *facts) {
    PC_CHECK(facts && pcBootEligible(facts));
    pcBootRendered=pcBootVaried=0;
    if (!pcCapture->buffer) pcCapture->buffer=w2c_hgengine_HGNativeFrameBuffer(pcCapture->game);
    pcBootRendered=w2c_hgengine_HGNativeRender2D(pcCapture->game,0,pcCapture->buffer,256*192*4) ? 1 : 0;
    if (!pcBootRendered) return FALSE;
    const u8 *pixels=pcGuest(pcCapture->buffer,256*192*4);
    for (u32 i=1;i<256*192;++i) if (memcmp(pixels,pixels+i*4,4)) {pcBootVaried=1; break;}
    if (!pcBootVaried) return FALSE;
    char path[4096]; PC_CHECK(snprintf(path,sizeof(path),"%s/boot-%s-main.ppm",pcCapture->folder,pcBootRole)<sizeof(path));
    FILE *file=fopen(path,"wbx"); PC_CHECK(file && fprintf(file,"P6\n256 192\n255\n")>0);
    for (u32 y=0;y<192;++y) {u8 row[256*3]; for (u32 x=0;x<256;++x) memcpy(row+x*3,pixels+(y*256+x)*4,3); PC_CHECK(fwrite(row,1,sizeof(row),file)==sizeof(row));}
    PC_CHECK(!fclose(file)); return TRUE;
}
static void pcBootTick(void) {
    u32 frame=OS_GetVBlankCount(),reads=HGNativeInputPadReads(pcCapture->env);
    if (!pcBootStart) { if (frame<PC_BOOT_START_FRAME) return; pcBootStart=frame; }
    if (frame-pcBootStart>=PC_BOOT_TIMEOUT_FRAMES) pcFinish("boot_ready_timeout",FALSE);
    PCBootFacts facts=pcBootObserve();
    if (pcBootConfirmSent && !pcBootExited && pcBootExitWitness(&facts,pcBootContinueGeneration)) {
        pcBootExited=1; pcBootEvidence("continue_selected",&facts);
    }
    if (pcBootLease) {
        if (pcBootLease==1 && reads-pcBootReads>=2) {PC_CHECK(HGNativeSetInput(pcCapture->env,0,FALSE,0,0,FALSE)); pcBootRelease=reads; pcBootLease=2; pcBootEvidence("released",&facts);}
        if (pcBootLease==2 && reads-pcBootRelease>=2) {pcBootLease=0; pcBootEvidence("settled",&facts);}
        return;
    }
    if (pcBootConfirmSent || !pcBootEligible(&facts)) return;
    if (facts.kind==HG_YN_BOOT_TITLE) {if (pcBootTitleSent) return; pcBootRole="title";}
    else if (facts.inputMode==1) {if (pcBootSwitchSent) return; pcBootRole="mode_switch";}
    else {PC_CHECK(pcBootContinuationAllowed(&facts,pcBootSwitchGeneration,pcBootSwitchRevision));pcBootRole="continue";}
    if (!pcBootPixels(&facts)) return;
    PCBootFacts live=pcBootObserve();
    PC_CHECK(pcBootEligible(&live) && !memcmp(&live,&facts,sizeof(live)));
    pcBootEvidence("ready",&facts);
    if (facts.kind==HG_YN_BOOT_TITLE) pcBootTitleSent=1;
    else if (facts.inputMode==1) {pcBootSwitchSent=1;pcBootSwitchGeneration=facts.ownerGeneration;pcBootSwitchRevision=facts.revision;}
    else {pcBootConfirmSent=1;pcBootContinueGeneration=facts.ownerGeneration;}
    PC_CHECK(HGNativeSetInput(pcCapture->env,1,FALSE,0,0,FALSE)); pcBootReads=reads; pcBootLease=1; pcBootEvidence("press",&facts);
}
// Host boot gate end.
static HGPCBoxQAState pcRead(void) {
    u32 address=w2c_hgengine_HGNativePCBoxQAGetState(pcCapture->game);
    HGPCBoxQAState state; memcpy(&state,pcGuest(address,sizeof(state)),sizeof(state));
    PC_CHECK(state.version==HG_PC_QA_VERSION && state.byteSize==sizeof(state));
    PC_CHECK(state.messageAwaitInput<=1 && state.boot.active<=1 && state.boot.ready<=1
        && (!state.boot.active || (state.boot.ownerGeneration && state.boot.revision)));
    PC_CHECK(w2c_hgengine_HGNativePCBoxQAGetState(pcCapture->game)==address && !memcmp(&state,pcGuest(address,sizeof(state)),sizeof(state)));
    pcFinal=state;
    if (state.error) { fprintf(stderr,"PC Box guest first error line %u\n",state.error); pcFinish("guest_observer_failed",FALSE); }
    return state;
}
static HGNativeFieldChoiceUIState pcReadFieldChoice(void) {
    u32 address=w2c_hgengine_HGNativeFieldChoiceUIGetState(pcCapture->game);
    HGNativeFieldChoiceUIState state; memcpy(&state,pcGuest(address,sizeof(state)),sizeof(state));
    PC_CHECK(w2c_hgengine_HGNativeFieldChoiceUIGetState(pcCapture->game)==address && !memcmp(&state,pcGuest(address,sizeof(state)),sizeof(state)));
    return state;
}
static BOOL pcMessageAckAllowed(const HGPCBoxQAState *qa,const HGNativeFieldChoiceUIState *choice) {
    return qa && choice && qa->messageAwaitInput==1 && !qa->scriptMenuReady && !qa->pc.active && !qa->summary.active && !qa->naming.active
        && !(choice->active && (choice->ready || choice->viewActive || HGNativeFieldChoiceUIIsListPhase(choice->phase)));
}
static u32 pcFieldChoiceKey(const HGNativeFieldChoiceUIState *choice,u32 target) {
    PC_CHECK(target<choice->rowCount);
    u32 current=choice->selectedIndex;
    if (current==target) return 1;
    // Original 2-4 item menus are vertical; larger menus use two columns.
    if (choice->rowCount<5 || current/2!=target/2) return current>target ? 64 : 128;
    return current>target ? 32 : 16;
}
static void pcNext(void) { ++pcStep; pcSubstep=0; pcStepFrame=OS_GetVBlankCount(); }
static void pcInputEvidence(const char *event,u32 keys,u32 reads) {
    const HGPCBoxQAState *qa=&pcFinal;
    HGNativeFieldChoiceUIState choice=pcReadFieldChoice();
    PC_CHECK(pcLeaseOwner);
    printf("PC Box input: {\"event\":\"%s\",\"step\":%u,\"menuSubstep\":%u,\"keys\":%u,\"padReads\":%u,\"frame\":%u,\"owner\":\"%s\",\"messageAwaitInput\":%u,"
        "\"pc\":{\"phase\":%u,\"mode\":%u,\"selectedSlot\":%u,\"ready\":%u,\"viewActive\":%u,\"ownerGeneration\":%u,\"revision\":%u},"
        "\"summary\":{\"active\":%u,\"phase\":%u,\"partySlot\":%u,\"ready\":%u},"
        "\"naming\":{\"active\":%u,\"ready\":%u,\"ownerGeneration\":%u,\"revision\":%u},"
        "\"fieldChoice\":{\"active\":%u,\"ready\":%u,\"viewActive\":%u,\"phase\":%u,\"ownerGeneration\":%u,\"revision\":%u}}\n",
        event,pcStep,pcSubstep,keys,reads,OS_GetVBlankCount(),pcLeaseOwner,qa->messageAwaitInput,qa->pc.phase,qa->pc.mode,qa->pc.selectedSlot,qa->pc.ready,
        qa->pc.viewActive,qa->pc.ownerGeneration,qa->pc.revision,qa->summary.active,qa->summary.phase,qa->summary.partySlot,
        qa->summary.ready,qa->naming.active,qa->naming.ready,qa->naming.ownerGeneration,qa->naming.revision,
        choice.active,choice.ready,choice.viewActive,choice.phase,choice.ownerGeneration,choice.revision);
}
static void pcPress(u32 keys) {
    PC_CHECK(!pcLease && keys && !(keys&(keys-1)));
    const HGPCBoxQAState *qa=&pcFinal;
    pcLeaseOwner=qa->naming.active ? "naming" : qa->summary.active ? "summary" : qa->pc.active && qa->pc.viewActive ? "pc"
        : qa->messageAwaitInput ? "field_message" : qa->scriptMenuReady ? "field_script" : "field";
    PC_CHECK(HGNativeSetInput(pcCapture->env,keys,FALSE,0,0,FALSE));
    pcPressReads=HGNativeInputPadReads(pcCapture->env); pcLease=1;
    pcInputEvidence("press",keys,pcPressReads);
}
static BOOL pcSettle(void) {
    if (!pcLease) return TRUE;
    u32 reads=HGNativeInputPadReads(pcCapture->env);
    if (pcLease==1 && reads-pcPressReads>=2) {
        PC_CHECK(HGNativeSetInput(pcCapture->env,0,FALSE,0,0,FALSE)); pcReleaseReads=reads; pcLease=2;
        pcInputEvidence("released",0,reads);
    }
    if (pcLease!=2 || reads-pcReleaseReads<2) return FALSE;
    pcLease=0; pcInputEvidence("settled",0,reads); pcLeaseOwner=NULL; return TRUE;
}
static void pcPad(const HGNativePCBoxUIState *state,u32 key) {
    u32 resolved=0; PC_CHECK(HGNativePCBoxUIResolveInput(state,state->ownerGeneration,state->revision,key,&resolved) && resolved==key);
    pcPress(resolved);
}
static u32 pcEdge(const HGPCBoxQAState *qa,u32 from,u32 direction,BOOL immediate) {
    u32 next=qa->neighbors[from][direction];
    if (next&128) next=immediate && qa->gridReturn!=255 ? qa->gridReturn : next&127;
    for (u32 attempts=0;attempts<qa->gridCount;++attempts) {
        if (next>=qa->gridCount) return from;
        if (qa->gridEnabled[next/32]&(1u<<(next%32))) return next;
        u32 following=qa->neighbors[next][direction]&127;
        if (following==next || following==from) return from;
        next=following;
    }
    return from;
}
static u32 pcRoute(const HGPCBoxQAState *qa,u32 target) {
    static const u32 keys[]={64,128,32,16};
    PC_CHECK(qa->gridCount && qa->gridCount<HG_PC_QA_GRID && qa->gridCursor<qa->gridCount && target<qa->gridCount);
    u32 queue[HG_PC_QA_GRID],first[HG_PC_QA_GRID]={0},head=0,tail=1;
    u8 seen[HG_PC_QA_GRID]={0}; queue[0]=qa->gridCursor; seen[queue[0]]=1;
    while (head<tail) {
        u32 from=queue[head++];
        for (u32 d=0;d<4;++d) {
            u32 next=pcEdge(qa,from,d,from==qa->gridCursor);
            if (seen[next]) continue;
            first[next]=from==qa->gridCursor ? keys[d] : first[from];
            if (next==target) return first[next];
            seen[next]=1; queue[tail++]=next;
        }
    }
    PC_CHECK(FALSE); return 0;
}
static BOOL pcSnapshotVisible(u32 required,u32 rendered,u32 varied) {
    return (required==1 || required==3) && !(rendered&~3u) && !(varied&~rendered) && (required&rendered&varied)==required;
}
static void pcSnapshotEvidence(const HGPCBoxQAState *qa,const char *event,u32 required,u32 rendered,u32 varied) {
    printf("PC Box snapshot: {\"event\":\"%s\",\"snapshot\":%u,\"step\":%u,\"requiredMask\":%u,\"renderedMask\":%u,\"variedMask\":%u,\"frame\":%u,\"mapId\":%u,\"ownerGeneration\":%u,\"phase\":%u,\"pcActive\":%u,\"summaryActive\":%u,\"namingActive\":%u,\"scriptMenuReady\":%u}\n",
        event,pcSnapshots,pcStep,required,rendered,varied,OS_GetVBlankCount(),qa->mapId,qa->pc.ownerGeneration,qa->pc.phase,
        qa->pc.active,qa->summary.active,qa->naming.active,qa->scriptMenuReady);
}
static BOOL pcSnapshot(const HGPCBoxQAState *qa) {
    static u8 pixels[2][256*192*4];
    static u32 waitingStep=UINT32_MAX,waitingFrame;
    u32 required=qa->pc.active || qa->summary.active || qa->naming.active || qa->scriptMenuReady ? 3 : 1;
    u32 rendered=0,varied=0,frame=OS_GetVBlankCount();
    if (!pcCapture->buffer) pcCapture->buffer=w2c_hgengine_HGNativeFrameBuffer(pcCapture->game);
    pcGuest(pcCapture->buffer,256*192*4); HGNativeDrainDMA();
    for (u32 engine=0;engine<2;++engine) {
        if (!w2c_hgengine_HGNativeRender2D(pcCapture->game,engine,pcCapture->buffer,256*192*4)) continue;
        rendered|=1u<<engine;
        memcpy(pixels[engine],pcGuest(pcCapture->buffer,sizeof(pixels[engine])),sizeof(pixels[engine]));
        for (u32 i=1;i<256*192;++i) if (memcmp(pixels[engine],pixels[engine]+i*4,4)) { varied|=1u<<engine; break; }
    }
    if (!pcSnapshotVisible(required,rendered,varied)) {
        if (waitingStep!=pcStep || frame-waitingFrame>=60) {
            pcSnapshotEvidence(qa,"waiting",required,rendered,varied); waitingStep=pcStep; waitingFrame=frame;
        }
        return FALSE;
    }
    char path[4096];
    PC_CHECK(snprintf(path,sizeof(path),"%s/snapshot-%03u-step-%03u-state.bin",pcCapture->folder,pcSnapshots,pcStep)<sizeof(path));
    FILE *state=fopen(path,"wb"); PC_CHECK(state && fwrite(qa,1,sizeof(*qa),state)==sizeof(*qa) && !fclose(state));
    for (u32 engine=0;engine<2;++engine) {
        if (!(rendered&(1u<<engine))) continue;
        PC_CHECK(snprintf(path,sizeof(path),"%s/snapshot-%03u-step-%03u-engine-%u.ppm",pcCapture->folder,pcSnapshots,pcStep,engine)<sizeof(path));
        FILE *image=fopen(path,"wb"); PC_CHECK(image && fprintf(image,"P6\n256 192\n255\n")>0);
        for (u32 i=0;i<256*192;++i) PC_CHECK(fwrite(pixels[engine]+i*4,1,3,image)==3);
        PC_CHECK(!fclose(image));
    }
    pcSnapshotEvidence(qa,"captured",required,rendered,varied);
    waitingStep=UINT32_MAX; ++pcSnapshots; return TRUE;
}
static void pcRejectBusy(const HGPCBoxQAState *qa) {
    u32 out=0x12345678;
    if (qa->pc.active && (!qa->pc.ready || !qa->pc.viewActive)) {
        PC_CHECK(!HGNativePCBoxUIResolveInput(&qa->pc,qa->pc.ownerGeneration,qa->pc.revision,1,&out) && out==0x12345678);
        ++pcBusyRejected;
    }
    if (pcOldGeneration && qa->pc.active && qa->pc.ownerGeneration!=pcOldGeneration) {
        PC_CHECK(!HGNativePCBoxUIResolveInput(&qa->pc,pcOldGeneration,pcOldRevision,1,&out)); ++pcStaleRejected;
        pcOldGeneration=0;
    }
}
static void pcSummary(HGPCBoxQAState qa) {
    HGNativeSummaryUIState *summary=&qa.summary; HGNativeSummaryUIInput input;
    if (pcSubstep==0 && summary->ready) {
        PC_CHECK(summary->monDataType==2 && summary->partyCount==30 && summary->partySlot==14 && summary->nextMonEnabled && summary->nextMonSlot==29);
        if (!pcSnapshot(&qa)) return;
        pcOldSummaryRevision=summary->revision;
        PC_CHECK(HGNativeSummaryUIResolveInput(summary,summary->revision,summary->phase,HG_SUMMARY_UI_INPUT_NEXT_MON,0,&input));
        pcPress(input.keys); pcSubstep=1;
    } else if (pcSubstep==1 && summary->ready && summary->partySlot==29) {
        PC_CHECK(!HGNativeSummaryUIResolveInput(summary,pcOldSummaryRevision,summary->phase,HG_SUMMARY_UI_INPUT_BACK,0,&input));
        PC_CHECK(summary->previousMonEnabled && summary->previousMonSlot==14 && !summary->nextMonEnabled);
        if (!pcSnapshot(&qa)) return;
        ++pcStaleRejected;
        PC_CHECK(HGNativeSummaryUIResolveInput(summary,summary->revision,summary->phase,HG_SUMMARY_UI_INPUT_BACK,0,&input));
        pcPress(input.keys); pcSubstep=2;
    } else if (pcSubstep==2 && !summary->active && qa.pc.ready && qa.pc.viewActive) pcNext();
}
static void pcNaming(HGPCBoxQAState qa) {
    if (qa.naming.ready && pcSubstep<2) {
        PC_CHECK(HGNativeNamingUIInputAllowed(&qa.naming,qa.naming.ownerGeneration,qa.naming.revision));
        if (!pcSubstep && !pcSnapshot(&qa)) return;
        pcPress(pcSubstep ? 1 : 8); ++pcSubstep; // Original START selects OK; A accepts the unchanged name.
    } else if (pcSubstep==2 && !qa.naming.active && qa.pc.ready && qa.pc.phase==HG_PC_BOX_UI_BOX_MENU_INPUT) pcNext();
}
static void PCBoxPresent(void *argument) {
    pcCapture=argument; HGPCBoxQAState qa=pcRead(); u32 frame=OS_GetVBlankCount();
    if (!qa.messageAwaitInput) pcMessageAckLatched=FALSE;
    if (!pcStep && !qa.fieldReady) {
        if (pcCapture->inputStep>=1 && !pcCapture->inputFrame) {pcCapture->noInput=TRUE; pcBootTick();}
        present(argument); return;
    }
    if (!pcStep && pcBootConfirmSent && qa.fieldReady && !pcBootFieldReady) {
        PCBootFacts facts=pcBootObserve();
        PC_CHECK(pcBootExited && pcBootExitWitness(&facts,pcBootContinueGeneration)); pcBootFieldReady=1; pcBootEvidence("field_ready",&facts);
    }
    if (!pcStep) PC_CHECK(pcBootConfirmSent && pcBootExited && pcBootFieldReady && !pcBootLease);
    if (!pcStepFrame) { pcStepFrame=frame; pcCapture->noInput=TRUE; PC_CHECK(HGNativeSetInput(pcCapture->env,0,FALSE,0,0,FALSE)); }
    if (frame-pcStepFrame>2400) {
        fprintf(stderr,"PC Box timeout step=%u map=%u phase=%u mode=%u ready=%u cursor=%u menu=%u/%u child=%u/%u\n",
            pcStep,qa.mapId,qa.pc.phase,qa.pc.mode,qa.pc.ready,qa.gridCursor,qa.scriptMenuReady,qa.scriptMenuCount,qa.summary.active,qa.naming.active);
        pcFinish("stage_timeout",FALSE);
    }
    pcRejectBusy(&qa); if (!pcSettle()) return;
    if (qa.baselinePrepared) { if (!pcPreparationCalls) pcPreparationCalls=qa.preparationSetterCalls; PC_CHECK(pcPreparationCalls==qa.preparationSetterCalls); }
    PC_CHECK(pcStep<sizeof(steps)/sizeof(steps[0])); const PCBoxStep *step=&steps[pcStep];
    switch (step->kind) {
    case OP_START:
        PC_CHECK(w2c_hgengine_HGNativePCBoxQARequest(pcCapture->game,qa.revision,HG_PC_QA_START)); pcNext(); break;
    case OP_ENTRY:
        if (qa.stage==3 && qa.fieldReady) {
            PC_CHECK(qa.baselinePrepared && qa.mapId==69 && qa.x==11 && qa.z==13 && !qa.facing && qa.pcTileBehavior==131);
            if (!pcSnapshot(&qa)) break;
            pcPress(1); pcNext();
        }
        break;
    case OP_MENU:
        if (qa.messageAwaitInput) {
            HGNativeFieldChoiceUIState choice=pcReadFieldChoice();
            if (!pcMessageAckAllowed(&qa,&choice)) break;
            PC_CHECK(!qa.scriptMenuReady && !(choice.active && (choice.ready || choice.viewActive || HGNativeFieldChoiceUIIsListPhase(choice.phase))));
            if (!pcMessageAckLatched) {
                if (!pcSnapshot(&qa)) break;
                pcPress(1); pcMessageAckLatched=TRUE; ++pcFieldMessageAcks;
            }
            break;
        }
        if (qa.scriptMenuReady && qa.scriptMenuSerial!=pcMenuSerial) {
            HGNativeFieldChoiceUIState choice=pcReadFieldChoice();
            if (!HGNativeFieldChoiceUIInputAllowed(&choice,choice.ownerGeneration,choice.revision) || choice.rowCount!=qa.scriptMenuCount) break;
            u32 index=0;
            if (step->target==UINT32_MAX) index=qa.scriptMenuCount-1;
            else { while (index<qa.scriptMenuCount && qa.scriptMenuValues[index]!=step->target) ++index; PC_CHECK(index<qa.scriptMenuCount); }
            u32 key=pcFieldChoiceKey(&choice,index),resolved=0;
            PC_CHECK(HGNativeFieldChoiceUIResolveInput(&choice,choice.ownerGeneration,choice.revision,key,&resolved) && resolved==key);
            pcPress(resolved);
            if (key==1) { pcMenuSerial=qa.scriptMenuSerial; pcNext(); } else ++pcSubstep;
        }
        break;
    case OP_GRID:
        if (qa.pc.ready && qa.pc.phase==step->phase && qa.gridCount) {
            if (!qa.gridButtons) pcPad(&qa.pc,64);
            else if (qa.gridCursor!=step->target) pcPad(&qa.pc,pcRoute(&qa,step->target));
            else {
                if (step->target==37 || step->target==10) { pcOldGeneration=qa.pc.ownerGeneration; pcOldRevision=qa.pc.revision; }
                pcPad(&qa.pc,1); pcNext();
            }
        }
        break;
    case OP_KEY:
        if (qa.pc.ready && qa.pc.phase==step->phase) {
            if (step->phase==HG_PC_BOX_UI_HANDLE_YES_NO) PC_CHECK(qa.yesNoHandler==step->bit);
            pcPad(&qa.pc,step->target); pcNext();
        }
        break;
    case OP_WAIT:
        if (qa.pc.ready && qa.pc.phase==step->phase) pcNext();
        break;
    case OP_CHECK:
        if ((!step->phase && qa.stage==5 && qa.fieldReady && qa.mapId==33) || (qa.pc.ready && qa.pc.phase==step->phase && !qa.pc.cursorHolding)) {
            if (!pcSubstep) {
                pcCheckpoint=qa.checkpointSerial;
                PC_CHECK(w2c_hgengine_HGNativePCBoxQARequest(pcCapture->game,qa.revision,HG_PC_QA_CHECKPOINT)); pcSubstep=1;
            } else if (qa.checkpointSerial!=pcCheckpoint) {
                PC_CHECK(qa.partyCount==step->party && qa.boxedCount==HG_PC_QA_MONS-step->party && qa.mons[0].location==HG_PC_QA_PARTY_LOCATION
                    && qa.mons[1].location==step->extra && qa.mons[2].location==step->pidgey && qa.mons[3].location==step->rattata
                    && qa.bagUnchanged && qa.flagsUnchanged && qa.moneyDexUnchanged && qa.boxNameUnchanged);
                if ((pcCoverage|step->bit)&HG_PC_QA_ITEMS) PC_CHECK(!qa.mons[2].heldItem && qa.mons[3].heldItem==ITEM_POTION);
                else PC_CHECK(qa.mons[2].heldItem==ITEM_POTION && !qa.mons[3].heldItem);
                u32 out=123; PC_CHECK(!HGNativePCBoxUIResolveInput(&qa.pc,qa.pc.ownerGeneration,qa.pc.revision-1,1,&out) && out==123);
                if (!pcSnapshot(&qa)) break;
                pcCoverage|=step->bit; ++pcStaleRejected; pcNext();
            }
        }
        break;
    case OP_SUMMARY: pcSummary(qa); break;
    case OP_NAMING: pcNaming(qa); break;
    case OP_RETURN:
        if (qa.fieldReady && !qa.pc.active && qa.mapId==69) { PC_CHECK(w2c_hgengine_HGNativePCBoxQARequest(pcCapture->game,qa.revision,HG_PC_QA_RETURN)); pcNext(); }
        break;
    case OP_FINISH:
        PC_CHECK(pcFieldMessageAcks>=2);
        PC_CHECK(qa.stage==5 && qa.fieldReady && qa.mapId==33 && !qa.pc.active && !qa.summary.active && !qa.naming.active
            && pcCoverage==HG_PC_QA_REQUIRED && qa.conservationChecks>=10 && pcStaleRejected>=3 && pcBusyRejected);
        pcFinish("passed_original_pc_box",TRUE); break;
    default: PC_CHECK(FALSE);
    }
}

int main(int argc,char **argv) {
    setvbuf(stdout,NULL,_IONBF,0);
    if (argc!=5 || !HGNativeFSSetRoot(argv[1]) || !HGNativeBackupSetPath(argv[3])) return 2;
    if (!pcBootSelfCheck()) { fputs("PC Box boot predicate self-check failed\n",stderr); return 2; }
    u8 header[0x160]; FILE *input=fopen(argv[2],"rb"); if (!input) return 2;
    BOOL valid=fread(header,1,sizeof(header),input)==sizeof(header) && fgetc(input)==EOF && !ferror(input); fclose(input); if (!valid) return 2;
    wasm_rt_init();
    w2c_hgengine *game=calloc(1,sizeof(*game)); struct w2c_env *env=calloc(1,sizeof(*env));
    struct w2c_wasi__snapshot__preview1 *wasi=calloc(1,sizeof(*wasi)); Capture *capture=calloc(1,sizeof(*capture));
    if (!game || !env || !wasi || !capture) return 2;
    wasi->fds[0]=dup(0); wasi->fds[1]=dup(1); wasi->fds[2]=dup(2);
    if (wasi->fds[0]<0 || wasi->fds[1]<0 || wasi->fds[2]<0) return 2;
    pcCapture=capture; capture->game=game; capture->env=env;
    if (!setjmp(pcResetLanding)) {
        wasm_rt_trap_t trap=wasm_rt_impl_try(); PC_CHECK(!trap);
        wasm2c_hgengine_instantiate(game,env,wasi); env->memory=wasi->memory=w2c_hgengine_memory(game); PC_CHECK(HGNativeSoundRealtime(env,TRUE));
        env->irqTable=&game->w2c_T0; env->stackPointer=&game->w2c_0x5F_stack_pointer; env->rootStackHigh=*env->stackPointer; *env->stackPointer-=16;
        env->irqType0=wasm2c_hgengine_get_func_type(0,0); env->irqType1=wasm2c_hgengine_get_func_type(1,0,WASM_RT_I32);
        env->rtcType=wasm2c_hgengine_get_func_type(2,0,WASM_RT_I32,WASM_RT_I32);
        env->irqGuardType=wasm2c_hgengine_get_func_type(4,0,WASM_RT_I32,WASM_RT_I32,WASM_RT_I32,WASM_RT_I32);
        u32 work=w2c_hgengine_HGNativeSystemWorkAddress(game); pcGuest(work,0x360); memcpy(env->memory->data+work+0x200,header,sizeof(header));
        w2c_hgengine_0x5Finitialize(game); u32 status=w2c_hgengine_HGNativeRegisterAddress(game,0x04000004,2); pcGuest(status,2);
        PC_CHECK(HGNativeStartDisplayClock(env,(const vu16 *)(env->memory->data+status)));
        *capture=(Capture){.game=game,.env=env,.folder=argv[4],.resume=TRUE};
        PC_CHECK(HGNativeSetDisplayObserver(env,PCBoxPresent,capture)); w2c_hgengine_NitroMain(game); PC_CHECK(FALSE);
    }
    BOOL resetQuiescent=FALSE;
    if (pcResetExpected && env->resetRequested) {
        HGNativeFinishReset(env);
        resetQuiescent=env->resetFinished && !env->pendingDMA && !env->dmaAffineWrites && !env->pendingRTC && !env->pendingMIC
            && !env->pendingCARD && !env->pendingPOWER && !env->pendingINPUT && !env->irqTable && !env->threads && !env->rasterFunction;
    }
    printf("PC Box evidence: {\"status\":\"%s\",\"step\":%u,\"guestError\":%u,\"qaVersion\":%u,\"qaBytes\":%u,\"fieldMessageAcks\":%u,\"baselinePrepared\":%u,\"preparationSetterCalls\":%u,\"noSettersAfterBaseline\":%u,\"conservationChecks\":%u,\"coverageMask\":%u,\"requiredCoverageMask\":%u,\"staleRejected\":%u,\"busyRejected\":%u,\"snapshots\":%u,\"physicalEntry\":%s,\"resetFinished\":%s,\"resetQuiescent\":%s,\"privateSaveWrites\":false,\"fixtureScope\":\"original APIs before baseline: Sentret party; Pidgey slot2/Potion; Rattata slot29; original Dex seen/caught; no capture claim\",\"namingOutcome\":\"original START/A accepts unchanged name\"}\n",
        pcStatus,pcStep,pcFinal.error,pcFinal.version,pcFinal.byteSize,pcFieldMessageAcks,pcFinal.baselinePrepared,pcFinal.preparationSetterCalls,
        pcPreparationCalls && pcPreparationCalls==pcFinal.preparationSetterCalls,pcFinal.conservationChecks,pcCoverage,HG_PC_QA_REQUIRED,
        pcStaleRejected,pcBusyRejected,pcSnapshots,pcCoverage&HG_PC_QA_PHYSICAL ? "true" : "false",env->resetFinished ? "true" : "false",resetQuiescent ? "true" : "false");
    BOOL passed=pcPassed && resetQuiescent;
    wasm2c_hgengine_free(game); for (u32 i=0;i<3;++i) close(wasi->fds[i]); free(game); free(env); free(wasi); free(capture); wasm_rt_free();
    if (passed) puts("PASS: original physical PC entry, deposit/withdraw/move/swap/refusal/cancel/items, child Summary/naming, conservation and reset.");
    return passed ? 0 : 1;
}
