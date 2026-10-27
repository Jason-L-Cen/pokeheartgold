// SPDX-License-Identifier: GPL-3.0-or-later
// Generated support changes only the existing Trainer host main name; all original boot/ledger/render helpers stay intact.
#include "trainer_roundtrip_support.inc"
#include "save_roundtrip_qa.h"
#include <limits.h>
static HGSaveRoundtripQAState sr;
static HGSaveRoundtripSnapshot srExpected;
static uint32_t srRole,srStage,srLease,srReads,srReleaseReads,srOwner,srGeneration,srRevision,srKeys,srX,srY,srTouch;
static uint32_t srPromptMask,srMenuCloses,srSnapshotWritten,srCompared,srDone;
static uint32_t srBeforeWritten;
static const char *srSnapshotPath;
static HGSaveRoundtripQAState srRead(void) {
    u32 address=w2c_hgengine_HGNativeTrainerCardQAGetState(tcCapture->game);HGSaveRoundtripQAState state;
    memcpy(&state,tcGuest(address,sizeof(state)),sizeof(state));
    TC_CHECK(state.srVersion==HG_SR_STATE_VERSION && state.srBytes==sizeof(state) && !state.error && state.role==srRole);
    TC_CHECK(state.trainer.version==HG_TC_QA_VERSION && state.trainer.byteSize==sizeof(state.trainer) && !state.trainer.error);
    return state;
}
static void srPulse(u32 owner,u32 key,u32 x,u32 y,BOOL touch,u32 generation,u32 revision) {
    TC_CHECK(!srLease);srOwner=owner;srKeys=key;srX=x;srY=y;srTouch=touch;srGeneration=generation;srRevision=revision;
    TC_CHECK(HGNativeSetInput(tcCapture->env,key,FALSE,x,y,touch));srReads=HGNativeInputPadReads(tcCapture->env);srLease=1;
    printf("SAVE input: {\"event\":\"press\",\"stage\":%u,\"owner\":%u,\"generation\":%u,\"revision\":%u,\"keys\":%u,\"touch\":%u,\"x\":%u,\"y\":%u,\"padReads\":%u}\n",srStage,owner,generation,revision,key,touch,x,y,srReads);
}
static BOOL srSettle(void) {
    if(!srLease)return TRUE;u32 reads=HGNativeInputPadReads(tcCapture->env);
    BOOL valid=srOwner==1 ? HGNativeTrainerCardUIInputAllowed(&tcFinal.ui,srGeneration,srRevision)
        : srOwner==2 ? HGNativeFieldMenuUIInputAllowed(&tcMenu,srGeneration,srRevision)
        : srOwner==3 ? HGSRSaveInputAllowed(&sr,srGeneration,srRevision) : tcFinal.fieldReady;
    if(srLease==1 && (!valid || reads-srReads>=4)) {
        TC_CHECK(HGNativeSetInput(tcCapture->env,0,FALSE,0,0,FALSE));srLease=2;srReleaseReads=reads;
        printf("SAVE input: {\"event\":\"release\",\"stage\":%u,\"padReads\":%u}\n",srStage,reads);return FALSE;
    }
    if(srLease!=2 || reads-srReleaseReads<2)return FALSE;
    srLease=0;printf("SAVE input: {\"event\":\"settled\",\"stage\":%u,\"padReads\":%u}\n",srStage,reads);return TRUE;
}
static void srWriteSnapshot(void) {
    TC_CHECK(HGSRWireValid(&sr.snapshot,sizeof(sr.snapshot)));
    FILE *file=fopen(srSnapshotPath,"wbx");TC_CHECK(file && fwrite(&sr.snapshot,1,sizeof(sr.snapshot),file)==sizeof(sr.snapshot) && !fclose(file));srSnapshotWritten=1;
    printf("SAVE frozen: {\"role\":%u,\"serial\":%u,\"bytes\":%zu,\"lengths\":[%u,%u,%u,%u,%u,%u],\"money\":%u,\"trainerID\":%u}\n",srRole,sr.snapshotSerial,sizeof(sr.snapshot),sr.snapshot.lengths[0],sr.snapshot.lengths[1],sr.snapshot.lengths[2],sr.snapshot.lengths[3],sr.snapshot.lengths[4],sr.snapshot.lengths[5],sr.snapshot.money,sr.snapshot.trainerID);
}
static void srWriteBefore(void) {
    char path[PATH_MAX];TC_CHECK(strlen(srSnapshotPath)<sizeof(path));strcpy(path,srSnapshotPath);
    char *slash=strrchr(path,'/');TC_CHECK(slash);strcpy(slash+1,"before.bin");
    TC_CHECK(HGSRWireValid(&sr.beforeSnapshot,sizeof(sr.beforeSnapshot)));
    FILE *file=fopen(path,"wbx");TC_CHECK(file && fwrite(&sr.beforeSnapshot,1,sizeof(sr.beforeSnapshot),file)==sizeof(sr.beforeSnapshot) && !fclose(file));srBeforeWritten=1;
}
static void SRPresent(void *argument) {
    tcCapture=argument;sr=srRead();
    if(srRole==HG_SR_WRITER && sr.beforeFrozen && !srBeforeWritten)srWriteBefore();
    if(srRole==HG_SR_READER && sr.readerFrozen && !srSnapshotWritten) {
        TC_CHECK(HGSRWireValid(&srExpected,sizeof(srExpected)) && !memcmp(&srExpected,&sr.snapshot,sizeof(srExpected)));
        srCompared=1;srWriteSnapshot();
    }
    if(tcStep==0 || (srRole==HG_SR_WRITER && tcStep<=30)) {TCQAPresent(argument);return;}
    tcFinal=tcRead();sr=srRead();if(!srSettle())return;
    if(srRole==HG_SR_READER) {
        if(!tcFinal.fieldReady)return;
        TC_CHECK(sr.readerFrozen && sr.loadSuccess==1 && srCompared && srSnapshotWritten);
        srDone=1;tcFinish("passed_save_roundtrip_reader",TRUE);
    }
    if(sr.writerFrozen && !srSnapshotWritten)srWriteSnapshot();
    if(srStage==0) {
        if(!HGNativeTrainerCardUIInputAllowed(&tcFinal.ui,tcFinal.ui.ownerGeneration,tcFinal.ui.revision))return;
        TC_CHECK(tcFinal.confirms==1 && tcFinal.editorExits==1 && tcFinal.cachedSignatureMatched);
        u32 key=0;TC_CHECK(HGNativeTrainerCardUIResolveKey(&tcFinal.ui,tcFinal.ui.ownerGeneration,tcFinal.ui.revision,2,&key));
        srStage=1;srPulse(1,key,0,0,FALSE,tcFinal.ui.ownerGeneration,tcFinal.ui.revision);return;
    }
    if(srStage==1) {
        if(tcFinal.ui.active)return;
        if(tcMenu.active) {
            if(!HGNativeFieldMenuUIInputAllowed(&tcMenu,tcMenu.ownerGeneration,tcMenu.revision))return;
            TC_CHECK(!srMenuCloses);++srMenuCloses;srPulse(2,2,0,0,FALSE,tcMenu.ownerGeneration,tcMenu.revision);return;
        }
        if(!tcFinal.fieldReady)return;
        TC_CHECK(srMenuCloses==1 && w2c_hgengine_HGNativeTrainerCardQARequest(tcCapture->game,sr.revision,HG_SR_SEAL_TRAINER));srStage=2;
    }
    if(srStage==2) {if(!tcFinal.fieldReady)return;srStage=3;srPulse(4,1024,0,0,FALSE,0,0);return;}
    if(srStage==3) {
        if(!HGNativeFieldMenuUIInputAllowed(&tcMenu,tcMenu.ownerGeneration,tcMenu.revision))return;
        TC_CHECK(sr.saveSlot<7 && ((w2c_hgengine_HGNativeFieldMenuIcons(tcCapture->game)>>(sr.saveSlot*4))&15)==5);
        TC_CHECK(HGNativeFieldMenuUITouchAllowed(&tcMenu,tcMenu.ownerGeneration,tcMenu.revision,sr.saveX,sr.saveY));
        srStage=4;srPulse(2,0,sr.saveX,sr.saveY,TRUE,tcMenu.ownerGeneration,tcMenu.revision);return;
    }
    if(srStage==4 || srStage==5) {
        u32 phase=srStage==4 ? 4 : 7;if(sr.savePhase!=phase || !HGSRSaveInputAllowed(&sr,sr.saveGeneration,sr.saveRevision))return;
        TC_CHECK(sr.promptCursor==0);srPromptMask|=srStage==4 ? 1 : 2;srStage++;
        srPulse(3,1,0,0,FALSE,sr.saveGeneration,sr.saveRevision);return;
    }
    if(srStage==6) {
        if(!sr.writerFrozen || sr.saveActive)return;
        if(tcMenu.active) {
            if(!HGNativeFieldMenuUIInputAllowed(&tcMenu,tcMenu.ownerGeneration,tcMenu.revision))return;
            TC_CHECK(srMenuCloses==1);++srMenuCloses;srPulse(2,2,0,0,FALSE,tcMenu.ownerGeneration,tcMenu.revision);return;
        }
        if(!tcFinal.fieldReady)return;
        TC_CHECK(srPromptMask==3 && sr.saveCalls==1 && sr.saveSuccess==1 && sr.lastSaveStatus==2 && sr.managerIsStack && !sr.managerLive && sr.flashCompleted && srSnapshotWritten
            && sr.beforeFrozen && sr.preservationChecked==1 && sr.preservationPassed==1 && srBeforeWritten);
        srDone=1;tcFinish("passed_save_roundtrip_writer",TRUE);
    }
}
int main(int argc,char **argv) {
    setvbuf(stdout,NULL,_IONBF,0);if(tcInputContractSelfCheck() || argc!=8)return 2;
    srRole=!strcmp(argv[5],"writer") ? HG_SR_WRITER : !strcmp(argv[5],"reader") ? HG_SR_READER : 0;if(!srRole)return 2;
    char save[PATH_MAX],frames[PATH_MAX],parent[PATH_MAX],expected[PATH_MAX];
    if(!realpath(argv[3],save) || !realpath(argv[4],frames))return 2;
    strcpy(parent,frames);char *slash=strrchr(parent,'/');if(!slash)return 2;*slash=0;
    if(snprintf(expected,sizeof(expected),"%s/scenario.sav",parent)>=sizeof(expected) || strcmp(save,expected))return 2;
    if(snprintf(expected,sizeof(expected),"%s/%s.bin",parent,srRole==HG_SR_WRITER ? "expected" : "actual")>=sizeof(expected) || strcmp(argv[6],expected))return 2;
    srSnapshotPath=argv[6];
    if(srRole==HG_SR_READER) {FILE *file=fopen(argv[7],"rb");if(!file)return 2;BOOL ok=fread(&srExpected,1,sizeof(srExpected),file)==sizeof(srExpected) && fgetc(file)==EOF && !ferror(file);fclose(file);if(!ok || !HGSRWireValid(&srExpected,sizeof(srExpected)))return 2;}
    if(!HGNativeFSSetRoot(argv[1]) || !HGNativeBackupSetPath(argv[3]))return 2;
    u8 header[0x160];FILE *input=fopen(argv[2],"rb");if(!input)return 2;BOOL valid=fread(header,1,sizeof(header),input)==sizeof(header) && fgetc(input)==EOF && !ferror(input);fclose(input);if(!valid)return 2;
    wasm_rt_init();w2c_hgengine *game=calloc(1,sizeof(*game));struct w2c_env *env=calloc(1,sizeof(*env));
    struct w2c_wasi__snapshot__preview1 *wasi=calloc(1,sizeof(*wasi));Capture *capture=calloc(1,sizeof(*capture));if(!game || !env || !wasi || !capture)return 2;
    wasi->fds[0]=dup(0);wasi->fds[1]=dup(1);wasi->fds[2]=dup(2);if(wasi->fds[0]<0 || wasi->fds[1]<0 || wasi->fds[2]<0)return 2;tcCapture=capture;capture->game=game;capture->env=env;
    if(!setjmp(tcResetLanding)) {
        wasm_rt_trap_t trap=wasm_rt_impl_try();TC_CHECK(!trap);wasm2c_hgengine_instantiate(game,env,wasi);env->memory=wasi->memory=w2c_hgengine_memory(game);TC_CHECK(HGNativeSoundRealtime(env,TRUE));
        env->irqTable=&game->w2c_T0;env->stackPointer=&game->w2c_0x5F_stack_pointer;env->rootStackHigh=*env->stackPointer;*env->stackPointer-=16;
        env->irqType0=wasm2c_hgengine_get_func_type(0,0);env->irqType1=wasm2c_hgengine_get_func_type(1,0,WASM_RT_I32);
        env->rtcType=wasm2c_hgengine_get_func_type(2,0,WASM_RT_I32,WASM_RT_I32);env->irqGuardType=wasm2c_hgengine_get_func_type(4,0,WASM_RT_I32,WASM_RT_I32,WASM_RT_I32,WASM_RT_I32);
        u32 system=w2c_hgengine_HGNativeSystemWorkAddress(game);memcpy((void *)tcGuest(system,0x360)+0x200,header,sizeof(header));w2c_hgengine_0x5Finitialize(game);
        u32 qa=w2c_hgengine_HGNativeTrainerCardQAGetState(game);memcpy(&sr,tcGuest(qa,sizeof(sr)),sizeof(sr));
        TC_CHECK(sr.srVersion==HG_SR_STATE_VERSION && sr.srBytes==sizeof(sr) && !sr.role && !sr.error);
        TC_CHECK(w2c_hgengine_HGNativeTrainerCardQARequest(game,sr.revision,srRole==HG_SR_WRITER ? HG_SR_CONFIG_WRITER : HG_SR_CONFIG_READER));sr=srRead();
        u32 status=w2c_hgengine_HGNativeRegisterAddress(game,0x04000004,2);TC_CHECK(HGNativeStartDisplayClock(env,tcGuest(status,2)));*capture=(Capture){.game=game,.env=env,.folder=argv[4],.resume=TRUE,.noInput=TRUE};
        TC_CHECK(HGNativeSetDisplayObserver(env,SRPresent,capture));w2c_hgengine_NitroMain(game);TC_CHECK(FALSE);
    }
    BOOL quiescent=FALSE;if(tcResetExpected && env->resetRequested) {HGNativeFinishReset(env);quiescent=env->resetFinished && !env->pendingDMA && !env->dmaAffineWrites && !env->pendingRTC && !env->pendingMIC && !env->pendingCARD && !env->pendingPOWER && !env->pendingINPUT && !env->irqTable && !env->threads && !env->rasterFunction;}
    printf("SAVE evidence: {\"role\":%u,\"processId\":%u,\"status\":\"%s\",\"done\":%u,\"trainerConfirms\":%u,\"saveCalls\":%u,\"saveSuccess\":%u,\"lastSaveStatus\":%u,\"writerFrozen\":%u,\"beforeFrozen\":%u,\"beforeWritten\":%u,\"preservationChecked\":%u,\"preservationPassed\":%u,\"readerFrozen\":%u,\"loadSuccess\":%u,\"promptMask\":%u,\"managerIsStack\":%u,\"managerLive\":%u,\"flashPolls\":%u,\"flashCompleted\":%u,\"fieldReady\":%u,\"snapshotWritten\":%u,\"readerCompared\":%u,\"resetQuiescent\":%s}\n",srRole,(u32)getpid(),tcStatus,srDone,tcFinal.confirms,sr.saveCalls,sr.saveSuccess,sr.lastSaveStatus,sr.writerFrozen,sr.beforeFrozen,srBeforeWritten,sr.preservationChecked,sr.preservationPassed,sr.readerFrozen,sr.loadSuccess,srPromptMask,sr.managerIsStack,sr.managerLive,sr.flashPolls,sr.flashCompleted,tcFinal.fieldReady,srSnapshotWritten,srCompared,quiescent ? "true" : "false");
    BOOL passed=tcPassed && srDone && quiescent;wasm2c_hgengine_free(game);for(u32 i=0;i<3;i++)close(wasi->fds[i]);free(game);free(env);free(wasi);free(capture);wasm_rt_free();return passed ? 0 : 1;
}
