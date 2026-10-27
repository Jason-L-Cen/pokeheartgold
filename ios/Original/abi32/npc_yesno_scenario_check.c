// SPDX-License-Identifier: GPL-3.0-or-later
#define main NPCYNBootstrapMain
#define HGEngineTrap NPCYNBootstrapTrap
#include "engine_check.c"
#undef HGEngineTrap
#undef main
#include "npc_yesno_qa.h"
#include <nitro/os/native_reset.h>
#include <setjmp.h>

u32 w2c_hgengine_HGNativeNPCYesNoQAGetState(w2c_hgengine *);
u32 w2c_hgengine_HGNativeNPCYesNoQARequest(w2c_hgengine *,u32,u32);
_Static_assert(sizeof(HGNativeNPCYesNoQAState)==276 && offsetof(HGNativeNPCYesNoQAState,choice)==140
    && offsetof(HGNativeNPCYesNoQAState,boot)==176,"private original NPC QA ABI");
enum { YN_BOOT,YN_START,YN_ARRIVE,YN_INTERACT,YN_CHOICE,YN_MESSAGE,YN_COMPLETE,YN_RETURN,YN_FINISH };
static Capture *ynCapture;
static jmp_buf ynResetLanding;
static BOOL ynResetExpected,ynPassed;
static const char *ynStatus="not_started",*ynOwner="none";
static u32 ynStep,ynStepFrame,ynCycle,ynAction,ynLease,ynPressReads,ynReleaseReads,ynHeldKey;
static u32 ynTitleSent,ynSwitchSent,ynContinueSent,ynTitleExit,ynContinueExit,ynModeSwitchSeen;
static u32 ynSwitchGeneration,ynSwitchRevision,ynContinueGeneration;
static u32 ynPhase5Mask,ynPhase6Mask,ynSnapshots,ynStaleRejected,ynOrigin[4];
static u32 ynPrinterSerial;
static HGNativeNPCYesNoQAState ynFinal;
static HGNativeFieldChoiceUIState ynChoice;

void HGEngineTrap(wasm_rt_trap_t code) {
    if (code==WASM_RT_TRAP_UNREACHABLE && ynResetExpected && ynCapture && ynCapture->env->resetRequested) longjmp(ynResetLanding,1);
    NPCYNBootstrapTrap(code);
}
static void ynFinish(const char *status,BOOL passed) {
    ynStatus=status; ynPassed=passed;
    if (ynCapture && ynCapture->env && ynCapture->env->memory) {
        HGNativeSetInput(ynCapture->env,0,FALSE,0,0,FALSE); ynResetExpected=TRUE;
        w2c_env_HGNativeResetControl(ynCapture->env,HG_RESET_REQUEST,0);
    }
    fprintf(stderr,"NPC YESNO failure before reset: %s\n",status); _Exit(1);
}
#define YN_CHECK(value) do { if (!(value)) {fprintf(stderr,"NPC YESNO line %d step %u: %s\n",__LINE__,ynStep,#value);ynFinish("assertion_failed",FALSE);} } while (0)
static const void *ynGuest(u32 address,size_t size) {
    YN_CHECK(address && address<=ynCapture->env->memory->size && size<=ynCapture->env->memory->size-address);
    return ynCapture->env->memory->data+address;
}
static HGNativeNPCYesNoQAState ynRead(void) {
    u32 address=w2c_hgengine_HGNativeNPCYesNoQAGetState(ynCapture->game);
    HGNativeNPCYesNoQAState qa; memcpy(&qa,ynGuest(address,sizeof(qa)),sizeof(qa));
    YN_CHECK(qa.version==HG_YN_QA_VERSION && qa.byteSize==sizeof(qa) && !qa.error);
    YN_CHECK(w2c_hgengine_HGNativeNPCYesNoQAGetState(ynCapture->game)==address && !memcmp(&qa,ynGuest(address,sizeof(qa)),sizeof(qa)));
    u32 choice=w2c_hgengine_HGNativeFieldChoiceUIGetState(ynCapture->game);
    memcpy(&ynChoice,ynGuest(choice,sizeof(ynChoice)),sizeof(ynChoice));
    YN_CHECK(w2c_hgengine_HGNativeFieldChoiceUIGetState(ynCapture->game)==choice && !memcmp(&ynChoice,ynGuest(choice,sizeof(ynChoice)),sizeof(ynChoice)));
    ynFinal=qa; return qa;
}
static void ynInputEvidence(const char *event,u32 key) {
    printf("NPC YESNO input: {\"event\":\"%s\",\"owner\":\"%s\",\"step\":%u,\"cycle\":%u,\"action\":%u,\"key\":%u,\"frame\":%u,\"padReads\":%u,"
        "\"phase\":%u,\"selectedIndex\":%u,\"ownerGeneration\":%u,\"revision\":%u,\"messageId\":%u,\"messageSerial\":%u,\"result\":%u,\"waitReady\":%u,\"waitSerial\":%u,"
        "\"bootKind\":%u,\"bootActive\":%u,\"bootGeneration\":%u,\"bootRevision\":%u,\"bootReady\":%u,\"bootPhase\":%u,\"bootMode\":%u,\"bootOption\":%u,\"bootNotice\":%u,\"bootExit\":%u,\"bootProgress\":%u,"
        "\"printerReady\":%u,\"printerSerial\":%u,\"yesBeforeResult\":%u,\"newWordId\":%u,\"yesSerial\":%u}\n",
        event,ynOwner,ynStep,ynCycle,ynAction,key,OS_GetVBlankCount(),HGNativeInputPadReads(ynCapture->env),ynChoice.phase,ynChoice.selectedIndex,
        ynChoice.ownerGeneration,ynChoice.revision,ynFinal.messageId,ynFinal.messageSerial,ynFinal.specialResult,ynFinal.waitButtonReady,ynFinal.waitButtonSerial,
        ynFinal.boot.kind,ynFinal.boot.active,ynFinal.boot.ownerGeneration,ynFinal.boot.revision,ynFinal.boot.ready,ynFinal.boot.phase,ynFinal.boot.inputMode,ynFinal.boot.option,ynFinal.boot.notice,ynFinal.boot.exit,ynFinal.boot.progressFlags,
        ynFinal.printerWaitReady,ynFinal.printerWaitSerial,ynFinal.yesBeforeResult,ynFinal.newWordId,ynFinal.yesSerial);
}
static void ynPress(u32 key,const char *owner) {
    YN_CHECK(!ynLease && key && !(key&(key-1))); ynOwner=owner;
    YN_CHECK(HGNativeSetInput(ynCapture->env,key,FALSE,0,0,FALSE));
    ynHeldKey=key; ynPressReads=HGNativeInputPadReads(ynCapture->env); ynLease=1; ynInputEvidence("press",key);
}
static BOOL ynSettle(void) {
    if (!ynLease) return TRUE;
    u32 reads=HGNativeInputPadReads(ynCapture->env);
    if (ynLease==1 && reads-ynPressReads>=2) {
        YN_CHECK(HGNativeSetInput(ynCapture->env,0,FALSE,0,0,FALSE)); ynReleaseReads=reads; ynLease=2; ynInputEvidence("released",0);
    }
    if (ynLease!=2 || reads-ynReleaseReads<2) return FALSE;
    ynInputEvidence("settled",0); ynLease=ynHeldKey=0; return TRUE;
}
static void ynNext(u32 step) {ynStep=step;ynStepFrame=OS_GetVBlankCount();}
static BOOL ynSnapshot(const char *label,u32 required) {
    static u8 pixels[2][256*192*4]; u32 rendered=0,varied=0;
    if (!ynCapture->buffer) ynCapture->buffer=w2c_hgengine_HGNativeFrameBuffer(ynCapture->game);
    HGNativeDrainDMA(); ynGuest(ynCapture->buffer,sizeof(pixels[0]));
    for (u32 engine=0;engine<2;++engine) {
        if (!w2c_hgengine_HGNativeRender2D(ynCapture->game,engine,ynCapture->buffer,sizeof(pixels[0]))) continue;
        rendered|=1u<<engine; memcpy(pixels[engine],ynGuest(ynCapture->buffer,sizeof(pixels[0])),sizeof(pixels[0]));
        for (u32 i=1;i<256*192;++i) if (memcmp(pixels[engine],pixels[engine]+i*4,4)) {varied|=1u<<engine;break;}
    }
    if ((rendered&varied&required)!=required) return FALSE;
    char path[4096];
    YN_CHECK(snprintf(path,sizeof(path),"%s/snapshot-%03u-%s-state.bin",ynCapture->folder,ynSnapshots,label)<sizeof(path));
    FILE *state=fopen(path,"wbx"); YN_CHECK(state && fwrite(&ynFinal,1,sizeof(ynFinal),state)==sizeof(ynFinal) && !fclose(state));
    YN_CHECK(snprintf(path,sizeof(path),"%s/snapshot-%03u-%s-choice.bin",ynCapture->folder,ynSnapshots,label)<sizeof(path));
    FILE *choice=fopen(path,"wbx"); YN_CHECK(choice && fwrite(&ynChoice,1,sizeof(ynChoice),choice)==sizeof(ynChoice) && !fclose(choice));
    for (u32 engine=0;engine<2;++engine) if (rendered&(1u<<engine)) {
        YN_CHECK(snprintf(path,sizeof(path),"%s/snapshot-%03u-%s-engine-%u.ppm",ynCapture->folder,ynSnapshots,label,engine)<sizeof(path));
        FILE *image=fopen(path,"wbx"); YN_CHECK(image && fprintf(image,"P6\n256 192\n255\n")>0);
        for (u32 row=0;row<192;++row) {u8 rgb[256*3];for (u32 x=0;x<256;++x) memcpy(rgb+x*3,pixels[engine]+(row*256+x)*4,3);YN_CHECK(fwrite(rgb,1,sizeof(rgb),image)==sizeof(rgb));}
        YN_CHECK(!fclose(image));
    }
    printf("NPC YESNO snapshot: {\"sequence\":%u,\"label\":\"%s\",\"step\":%u,\"cycle\":%u,\"requiredMask\":%u,\"renderedMask\":%u,\"variedMask\":%u,\"frame\":%u,\"phase\":%u,\"ownerGeneration\":%u,\"revision\":%u}\n",
        ynSnapshots++,label,ynStep,ynCycle,required,rendered,varied,OS_GetVBlankCount(),ynChoice.phase,ynChoice.ownerGeneration,ynChoice.revision); return TRUE;
}
static void ynConservation(const HGNativeNPCYesNoQAState *qa) {
    YN_CHECK(qa->baselinePrepared && qa->partyUnchanged && qa->pcUnchanged && qa->bagUnchanged && qa->moneyUnchanged);
    if (!qa->completedYes) YN_CHECK(qa->varsFlagsUnchanged && qa->easyChatUnchanged);
    else YN_CHECK(qa->ledgerPrepared && qa->varsFlagsLedgerMatched && qa->easyChatLedgerMatched);
}
static BOOL ynBootSameSwitchedOwner(const HGYNBootFacts *boot,u32 generation,u32 revision) {
    return boot && generation && boot->kind==HG_YN_BOOT_MAIN_MENU && boot->active==1 && boot->ready==1
        && boot->ownerGeneration==generation && boot->revision && boot->revision!=revision && boot->inputMode==0;
}
static BOOL ynBootExitWitness(const HGYNBootFacts *boot,u32 generation) {
    return boot && generation && boot->kind==HG_YN_BOOT_MAIN_MENU && boot->active==0 && boot->ready==0
        && boot->ownerGeneration==generation && boot->revision && boot->phase==8 && boot->exit==1;
}
static void NPCYNPresent(void *argument) {
    ynCapture=argument; HGNativeNPCYesNoQAState qa=ynRead(); u32 frame=OS_GetVBlankCount();
    if (!ynStepFrame) ynStepFrame=frame;
    if (frame-ynStepFrame>(ynStep==YN_BOOT ? 12000u : 2400u)) ynFinish("stage_timeout",FALSE);
    if (qa.boot.kind==HG_YN_BOOT_TITLE && qa.boot.exit==1 && qa.boot.phase!=2) ynTitleExit=1;
    if (ynContinueSent && ynBootExitWitness(&qa.boot,ynContinueGeneration)) ynContinueExit=1;
    if (ynSwitchSent && ynBootSameSwitchedOwner(&qa.boot,ynSwitchGeneration,ynSwitchRevision)) ynModeSwitchSeen=1;
    if (qa.script2Active && ynChoice.active && (ynChoice.phase==5 || ynChoice.phase==6)) {
        u32 output=0x12345678; YN_CHECK(!HGNativeFieldChoiceUIResolveInput(&ynChoice,ynChoice.ownerGeneration,ynChoice.revision,1,&output) && output==0x12345678);
        u32 *mask=ynChoice.phase==5 ? &ynPhase5Mask : &ynPhase6Mask,bit=1u<<ynCycle;
        if (!(*mask&bit) && ynSnapshot(ynChoice.phase==5 ? "phase5-blocked" : "phase6-release",ynChoice.phase==5 ? 3 : 1)) *mask|=bit;
    }
    if (ynCycle==3 && qa.script2Active) YN_CHECK(!(ynChoice.active && ynChoice.phase>=3 && ynChoice.phase<=6));
    if (!ynSettle()) return;
    if (ynStep==YN_BOOT) {
        if (qa.fieldReady) {YN_CHECK(ynContinueSent && ynContinueExit && ynBootExitWitness(&qa.boot,ynContinueGeneration) && (!ynTitleSent || ynTitleExit));ynNext(YN_START);return;}
        if (qa.boot.active!=1 || qa.boot.ready!=1 || !qa.boot.ownerGeneration || !qa.boot.revision) return;
        const char *role=NULL;
        if (qa.boot.kind==HG_YN_BOOT_TITLE && !ynTitleSent) {YN_CHECK(qa.boot.phase==2 && !qa.boot.inputMode && !qa.boot.option && !qa.boot.notice && !qa.boot.exit && (qa.boot.progressFlags&0xF)==0xF);role="title";}
        else if (qa.boot.kind==HG_YN_BOOT_MAIN_MENU && !ynContinueSent) {
            YN_CHECK(qa.boot.phase==5 && qa.boot.notice==15 && qa.boot.inputMode<=1 && qa.boot.option<9 && !qa.boot.exit && (qa.boot.progressFlags&0xFF)==0xFF);
            if (qa.boot.inputMode==1) {if (ynSwitchSent) return;role="mode_switch";}
            else {YN_CHECK(!ynSwitchSent || (ynModeSwitchSeen && ynBootSameSwitchedOwner(&qa.boot,ynSwitchGeneration,ynSwitchRevision)));role="continue";}
        }
        if (!role || !ynSnapshot(role,1)) return;
        if (qa.boot.kind==HG_YN_BOOT_TITLE) ynTitleSent=1;
        else if (qa.boot.inputMode) {ynSwitchSent=1;ynSwitchGeneration=qa.boot.ownerGeneration;ynSwitchRevision=qa.boot.revision;}
        else {ynContinueSent=1;ynContinueGeneration=qa.boot.ownerGeneration;}
        ynPress(1,role); return;
    }
    if (ynStep==YN_START) {
        YN_CHECK(qa.fieldReady && qa.compatibleSave && qa.flagABAClear && qa.trendySayingsRemain);
        ynOrigin[0]=qa.mapId;ynOrigin[1]=qa.x;ynOrigin[2]=qa.z;ynOrigin[3]=qa.facing;
        YN_CHECK(w2c_hgengine_HGNativeNPCYesNoQARequest(ynCapture->game,qa.revision,HG_YN_QA_START));ynNext(YN_ARRIVE);return;
    }
    if (ynStep==YN_ARRIVE) {
        if (qa.stage!=HG_YN_QA_READY || !qa.fieldReady) return;
        YN_CHECK(qa.mapId==HG_YN_QA_MAP && qa.x==8 && qa.z==6 && !qa.facing && qa.checkpointSerial==1);
        ynConservation(&qa);if (!ynSnapshot("npc-arrival",1)) return;ynNext(YN_INTERACT);
    }
    if (ynStep==YN_INTERACT) {
        if (!qa.fieldReady) return;
        YN_CHECK(qa.completedNo+qa.completedYes+qa.completedDaily==ynCycle);
        ynAction=0;ynPress(1,"field");ynNext(ynCycle==3 ? YN_MESSAGE : YN_CHOICE);return;
    }
    if (ynStep==YN_CHOICE) {
        if (!HGNativeFieldChoiceUIInputAllowed(&ynChoice,ynChoice.ownerGeneration,ynChoice.revision)) return;
        YN_CHECK(qa.script2Active && qa.messageId==1 && ynChoice.phase==4 && ynChoice.rowCount==2 && ynChoice.cancelEnabled==1);
        const u32 first[]={128,64,2},second[]={128,1},third[]={1}; u32 count=ynCycle==0 ? 3 : ynCycle==1 ? 2 : 1;
        YN_CHECK(ynAction<count && ynChoice.selectedIndex==(ynAction==1 ? 1u : 0u));
        if (!ynSnapshot("yesno-ready",3)) return;
        u32 key=(ynCycle==0 ? first : ynCycle==1 ? second : third)[ynAction],resolved=0,sentinel=0x12345678;
        YN_CHECK(!HGNativeFieldChoiceUIResolveInput(&ynChoice,ynChoice.ownerGeneration,ynChoice.revision-1,key,&sentinel) && sentinel==0x12345678);++ynStaleRejected;
        YN_CHECK(HGNativeFieldChoiceUIResolveInput(&ynChoice,ynChoice.ownerGeneration,ynChoice.revision,key,&resolved) && resolved==key);
        ynPress(resolved,"choice");if (++ynAction==count) ynNext(YN_MESSAGE);return;
    }
    if (ynStep==YN_MESSAGE) {
        if (ynCycle==2 && qa.printerWaitReady) {
            YN_CHECK(qa.script2Active && qa.messageId==2 && qa.printerWaitMessageSerial==qa.messageSerial && !qa.waitButtonReady
                && !(ynChoice.active && ynChoice.phase>=3 && ynChoice.phase<=6));
            if (qa.printerWaitSerial==ynPrinterSerial) return;
            YN_CHECK(qa.printerWaitSerial==ynPrinterSerial+1 && qa.printerWaitSerial<=8);
            if (!ynSnapshot("yes-original-printer-page",1)) return;
            ynPrinterSerial=qa.printerWaitSerial;ynPress(1,"printer");return;
        }
        if (!qa.waitButtonReady) return;
        YN_CHECK(qa.script2Active && qa.messageId==(ynCycle<2 ? 3u : ynCycle==2 ? 2u : 4u)
            && qa.waitButtonSerial==ynCycle+1 && qa.waitButtonMessageSerial==qa.messageSerial);
        if (ynCycle<2) YN_CHECK(qa.specialResult==1 && qa.noResultObserved);
        else YN_CHECK(qa.yesBeforeResult==0 && qa.yesSerial==1 && qa.newWordId<32 && qa.yesFlagSet && qa.ledgerPrepared);
        if (ynCycle==2) YN_CHECK(qa.specialResult==qa.newWordId && qa.yesMessageSerial==6 && qa.messageSerial==6);
        if (!ynSnapshot("original-message-waitbutton",1)) return;
        ynPress(1,"message");ynNext(YN_COMPLETE);return;
    }
    if (ynStep==YN_COMPLETE) {
        if (!qa.fieldReady || qa.completedNo+qa.completedYes+qa.completedDaily!=ynCycle+1) return;
        YN_CHECK(qa.waitButtonAckSerial==ynCycle+1 && qa.conservationChecks==ynCycle+1 && qa.checkpointSerial==ynCycle+2);
        ynConservation(&qa);if (!ynSnapshot("interaction-complete",1)) return;
        if (++ynCycle<4) ynNext(YN_INTERACT); else {
            YN_CHECK(ynPhase5Mask==7 && ynPhase6Mask==7 && qa.phase5Serial>=3 && qa.phase6Serial>=3);
            YN_CHECK(w2c_hgengine_HGNativeNPCYesNoQARequest(ynCapture->game,qa.revision,HG_YN_QA_RETURN));ynNext(YN_RETURN);
        }
        return;
    }
    if (ynStep==YN_RETURN) {
        if (qa.stage!=HG_YN_QA_DONE || !qa.fieldReady) return;
        YN_CHECK(qa.mapId==ynOrigin[0] && qa.x==ynOrigin[1] && qa.z==ynOrigin[2] && qa.facing==ynOrigin[3]);
        ynConservation(&qa);YN_CHECK(qa.conservationChecks==5 && qa.checkpointSerial==6 && qa.completedNo==2 && qa.completedYes==1 && qa.completedDaily==1
            && qa.waitButtonAckSerial==4 && qa.messageSerial==7 && qa.waitButtonMessageSerial==7 && !qa.script2Active);
        if (!ynSnapshot("original-origin-return",1)) return;
        ynNext(YN_FINISH);ynFinish("passed_original_npc_yesno",TRUE);
    }
}

static u32 ynInputContractSelfCheck(void) {
#define YN_INPUT_REQUIRE(value) do {if (!(value)) return __LINE__;} while (0)
    HGNativeFieldChoiceUIState state={.active=1,.ownerGeneration=7,.revision=11,.ready=1,.phase=4,
        .rowCount=2,.selectedIndex=0,.cancelEnabled=1,.viewActive=1};
    const u32 allowed[]={1,2,64,128},blocked[]={0,3,8,16,32,2048};
    for (u32 i=0;i<4;++i) {u32 output=0;YN_INPUT_REQUIRE(HGNativeFieldChoiceUIResolveInput(&state,7,11,allowed[i],&output) && output==allowed[i]);}
    for (u32 i=0;i<6;++i) {u32 output=123;YN_INPUT_REQUIRE(!HGNativeFieldChoiceUIResolveInput(&state,7,11,blocked[i],&output) && output==123);}
    u32 output=123;YN_INPUT_REQUIRE(!HGNativeFieldChoiceUIResolveInput(&state,7,10,1,&output) && output==123);
    for (u32 phase=3;phase<=6;++phase) if (phase!=4) {
        state.phase=phase;state.ready=0;
        for (u32 i=0;i<4;++i) {output=123;YN_INPUT_REQUIRE(!HGNativeFieldChoiceUIResolveInput(&state,7,11,allowed[i],&output) && output==123);}
    }
    HGYNBootFacts boot={.kind=HG_YN_BOOT_MAIN_MENU,.active=1,.ownerGeneration=7,.revision=11,.ready=1};
    YN_INPUT_REQUIRE(ynBootSameSwitchedOwner(&boot,7,10));
    YN_INPUT_REQUIRE(!ynBootSameSwitchedOwner(&boot,8,10) && !ynBootSameSwitchedOwner(&boot,7,11));
    boot.active=boot.ready=0;boot.phase=8;boot.exit=1;
    YN_INPUT_REQUIRE(ynBootExitWitness(&boot,7) && !ynBootExitWitness(&boot,8) && !ynBootExitWitness(&boot,0));
    boot.phase=7;YN_INPUT_REQUIRE(!ynBootExitWitness(&boot,7));
#undef YN_INPUT_REQUIRE
    return 0;
}

int main(int argc,char **argv) {
    setvbuf(stdout,NULL,_IONBF,0);
    if (ynInputContractSelfCheck()) return 2;
    if (argc!=5 || !HGNativeFSSetRoot(argv[1]) || !HGNativeBackupSetPath(argv[3])) return 2;
    u8 header[0x160];FILE *input=fopen(argv[2],"rb");if (!input) return 2;
    BOOL valid=fread(header,1,sizeof(header),input)==sizeof(header) && fgetc(input)==EOF && !ferror(input);fclose(input);if (!valid) return 2;
    wasm_rt_init();w2c_hgengine *game=calloc(1,sizeof(*game));struct w2c_env *env=calloc(1,sizeof(*env));
    struct w2c_wasi__snapshot__preview1 *wasi=calloc(1,sizeof(*wasi));Capture *capture=calloc(1,sizeof(*capture));
    if (!game || !env || !wasi || !capture) return 2;
    wasi->fds[0]=dup(0);wasi->fds[1]=dup(1);wasi->fds[2]=dup(2);if (wasi->fds[0]<0 || wasi->fds[1]<0 || wasi->fds[2]<0) return 2;
    ynCapture=capture;capture->game=game;capture->env=env;
    if (!setjmp(ynResetLanding)) {
        wasm_rt_trap_t trap=wasm_rt_impl_try();YN_CHECK(!trap);
        wasm2c_hgengine_instantiate(game,env,wasi);env->memory=wasi->memory=w2c_hgengine_memory(game);YN_CHECK(HGNativeSoundRealtime(env,TRUE));
        env->irqTable=&game->w2c_T0;env->stackPointer=&game->w2c_0x5F_stack_pointer;env->rootStackHigh=*env->stackPointer;*env->stackPointer-=16;
        env->irqType0=wasm2c_hgengine_get_func_type(0,0);env->irqType1=wasm2c_hgengine_get_func_type(1,0,WASM_RT_I32);
        env->rtcType=wasm2c_hgengine_get_func_type(2,0,WASM_RT_I32,WASM_RT_I32);env->irqGuardType=wasm2c_hgengine_get_func_type(4,0,WASM_RT_I32,WASM_RT_I32,WASM_RT_I32,WASM_RT_I32);
        u32 system=w2c_hgengine_HGNativeSystemWorkAddress(game);memcpy((void *)ynGuest(system,0x360)+0x200,header,sizeof(header));
        w2c_hgengine_0x5Finitialize(game);u32 status=w2c_hgengine_HGNativeRegisterAddress(game,0x04000004,2);YN_CHECK(HGNativeStartDisplayClock(env,ynGuest(status,2)));
        *capture=(Capture){.game=game,.env=env,.folder=argv[4],.resume=TRUE,.noInput=TRUE};
        YN_CHECK(HGNativeSetDisplayObserver(env,NPCYNPresent,capture));w2c_hgengine_NitroMain(game);YN_CHECK(FALSE);
    }
    BOOL quiescent=FALSE;
    if (ynResetExpected && env->resetRequested) {HGNativeFinishReset(env);quiescent=env->resetFinished && !env->pendingDMA && !env->dmaAffineWrites && !env->pendingRTC
        && !env->pendingMIC && !env->pendingCARD && !env->pendingPOWER && !env->pendingINPUT && !env->irqTable && !env->threads && !env->rasterFunction;}
    printf("NPC YESNO evidence: {\"status\":\"%s\",\"qaVersion\":%u,\"qaBytes\":%u,\"step\":%u,\"completedNo\":%u,\"completedYes\":%u,\"completedDaily\":%u,\"conservationChecks\":%u,\"checkpoints\":%u,\"messageSerial\":%u,\"waitSerial\":%u,\"waitAckSerial\":%u,\"phase5Mask\":%u,\"phase6Mask\":%u,\"staleRejected\":%u,\"snapshots\":%u,\"titleExit\":%u,\"continueExit\":%u,\"yesBeforeResult\":%u,\"newWordId\":%u,\"yesSerial\":%u,\"yesFlagSet\":%u,\"varsLedger\":%u,\"easyChatLedger\":%u,\"privateSaveWrites\":false,\"resetQuiescent\":%s}\n",
        ynStatus,ynFinal.version,ynFinal.byteSize,ynStep,ynFinal.completedNo,ynFinal.completedYes,ynFinal.completedDaily,ynFinal.conservationChecks,ynFinal.checkpointSerial,ynFinal.messageSerial,
        ynFinal.waitButtonSerial,ynFinal.waitButtonAckSerial,ynPhase5Mask,ynPhase6Mask,ynStaleRejected,ynSnapshots,ynTitleExit,ynContinueExit,
        ynFinal.yesBeforeResult,ynFinal.newWordId,ynFinal.yesSerial,ynFinal.yesFlagSet,ynFinal.varsFlagsLedgerMatched,ynFinal.easyChatLedgerMatched,quiescent ? "true" : "false");
    BOOL passed=ynPassed && quiescent;wasm2c_hgengine_free(game);for (u32 i=0;i<3;++i) close(wasi->fds[i]);free(game);free(env);free(wasi);free(capture);wasm_rt_free();
    if (passed) puts("PASS: original NPC No/No/Yes/daily paths, original printer/WaitButton input, exact mutation ledger, origin return and reset.");
    return passed ? 0 : 1;
}
