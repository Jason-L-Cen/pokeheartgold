// SPDX-License-Identifier: GPL-3.0-or-later
// Generated support only renames the unused Phone33 main; its helpers and body are retained.
#include "pokegear_map_phone_support.inc"
#include "pokegear_map_card_qa.h"
#include "native_battle_ui.h"
#include "constants/battle.h"
enum {MAP_START,MAP_GUIDE_ROUTE,MAP_TOUR,MAP_CARD_ROUTE,MAP_GIFT,MAP_MENU,MAP_ICON,MAP_PHONE,MAP_VIEW,MAP_CLOSE,MAP_FIELD,MAP_FINISH};
static HGPokegearMapCardQAState mapFinal;
static u32 mapStage,mapStageFrame,mapSegment,mapPending,mapTextSerial,mapTextId,mapRunLaunch,mapRunRevision,mapRunAttempts,mapRuns,mapMenuCloses,mapClosePresses;
static u32 mapLease,mapReads,mapReleaseReads,mapOwner,mapGen,mapRev,mapLCD,mapLeaseScript,mapLeaseSerial;
static u32 mapMovePending,mapMoveKey,mapMoveX,mapMoveZ,mapBlocked,mapSteps;
static HGNativePokegearUIState mapCloseBefore;
static HGNativeFieldMenuUIState mapMenuBefore;
static BOOL mapStartCanRequest(const HGPokegearMapCardQAState *state) {
    return HGMapStationary(state) && state->revision && state->phone.compatibleSave==1 && state->phone.pokegearSlot<7;
}
static HGPokegearMapCardQAState mapRead(void) {
    u32 address=w2c_hgengine_HGNativePokegearQAGetState(pgCapture->game);HGPokegearMapCardQAState state;
    memcpy(&state,pgGuest(address,sizeof(state)),sizeof(state));
    PG_CHECK(state.version==HG_PG_MAP_VERSION && state.byteSize==sizeof(state) && !state.error);
    PG_CHECK(w2c_hgengine_HGNativePokegearQAGetState(pgCapture->game)==address && !memcmp(&state,pgGuest(address,sizeof(state)),sizeof(state)));
    pgFinal=pgRead();state.phone=pgFinal;return state;
}
static BOOL mapSnapshot(void) {
    HGPokegearMapCardQAState before=mapRead();if(!pgSnapshot())return FALSE;
    HGPokegearMapCardQAState after=mapRead();if(memcmp(&before,&after,sizeof(before)))return FALSE;
    char path[4096];PG_CHECK(snprintf(path,sizeof(path),"%s/map-%03u-state.bin",pgCapture->folder,pgSnapshots-1)<sizeof(path));
    FILE *file=fopen(path,"wbx");PG_CHECK(file && fwrite(&after,1,sizeof(after),file)==sizeof(after) && !fclose(file));
    printf("MAP snapshot: {\"sequence\":%u,\"stage\":%u,\"cards\":%u,\"scene\":%u,\"script\":%u,\"wait\":%u,\"serial\":%u}\n",pgSnapshots-1,mapStage,after.cards,after.scene,after.scriptId,after.scriptWait,after.scriptSerial);return TRUE;
}
static void mapNext(void) {++mapStage;mapStageFrame=OS_GetVBlankCount();mapPending=0;}
static void mapTrace(const char *event,u32 key,u32 touch,u32 x,u32 y) {
    printf("MAP input: {\"event\":\"%s\",\"stage\":%u,\"segment\":%u,\"key\":%u,\"touch\":%u,\"x\":%u,\"y\":%u,\"padReads\":%u,\"map\":%u,\"worldX\":%u,\"worldZ\":%u,\"scene\":%u,\"cards\":%u,\"script\":%u,\"wait\":%u,\"serial\":%u,\"owner\":%u,\"generation\":%u,\"revision\":%u}\n",
        event,mapStage,mapSegment,key,touch,x,y,HGNativeInputPadReads(pgCapture->env),mapFinal.mapId,mapFinal.x,mapFinal.z,mapFinal.scene,mapFinal.cards,mapFinal.scriptId,mapFinal.scriptWait,mapFinal.scriptSerial,mapOwner,mapGen,mapRev);
}
static void mapPulse(u32 key,u32 x,u32 y,BOOL touch,u32 owner) {
    PG_CHECK(!mapLease);mapOwner=owner;mapLCD=pgTouchEngine();mapGen=owner==1 ? pgFinal.ui.ownerGeneration : owner==2 ? pgMenu.ownerGeneration : 0;
    mapRev=owner==1 ? pgFinal.ui.revision : owner==2 ? pgMenu.revision : 0;
    mapLeaseScript=mapFinal.scriptId;mapLeaseSerial=mapFinal.scriptSerial;
    PG_CHECK(owner!=1 || mapLCD==0);PG_CHECK(HGNativeSetInput(pgCapture->env,key,FALSE,x,y,touch));
    mapReads=HGNativeInputPadReads(pgCapture->env);mapLease=1;mapTrace("press",key,touch,x,y);
}
static BOOL mapSettle(void) {
    if(!mapLease)return TRUE;u32 reads=HGNativeInputPadReads(pgCapture->env);
    BOOL valid=mapOwner==1 ? HGNativePokegearUIInputAllowed(&pgFinal.ui,mapGen,mapRev)
        : mapOwner==2 ? HGNativeFieldMenuUIInputAllowed(&pgMenu,mapGen,mapRev)
        : mapOwner==3 ? HGMapTextAllowed(&mapFinal,mapLeaseScript,mapLeaseSerial)
        : pgFinal.fieldReady && !mapFinal.scriptActive && !mapFinal.battleLive;
    if(mapOwner && pgTouchEngine()!=mapLCD)valid=FALSE;
    if(!mapOwner && mapMovePending && (mapFinal.x!=mapMoveX || mapFinal.z!=mapMoveZ))valid=FALSE;
    if(mapLease==1 && (!valid || reads-mapReads>=4)) {
        PG_CHECK(HGNativeSetInput(pgCapture->env,0,FALSE,0,0,FALSE));mapReleaseReads=reads;mapLease=2;mapTrace("release",0,0,0,0);return FALSE;
    }
    if(mapLease!=2 || reads-mapReleaseReads<2)return FALSE;
    mapLease=0;mapTrace("settled",0,0,0,0);return TRUE;
}
static BOOL mapHandleBattle(void) {
    u32 address=w2c_hgengine_HGNativeBattleUIGetState(pgCapture->game);HGNativeBattleUIState ui;
    memcpy(&ui,pgGuest(address,sizeof(ui)),sizeof(ui));
    PG_CHECK(w2c_hgengine_HGNativeBattleUIGetState(pgCapture->game)==address && !memcmp(&ui,pgGuest(address,sizeof(ui)),sizeof(ui)));
    if(!mapFinal.battleLive && !ui.active)return FALSE;
    PG_CHECK(mapFinal.battleLive && !mapFinal.battleType && !mapFinal.battleSpecial && mapFinal.battleLaunches==mapFinal.battleReturns+1);
    if(mapRunLaunch!=mapFinal.battleLaunches){mapRunLaunch=mapFinal.battleLaunches;mapRunAttempts=0;mapRunRevision=0;}
    if(ui.ready) {
        if(ui.mode!=HG_BATTLE_UI_COMMAND)pgFinish("unknown_battle_input",FALSE);
        if(mapRunAttempts && ui.revision==mapRunRevision)return TRUE;
        if(mapRunAttempts>=HG_PG_MAP_RUN_ATTEMPT_CAP)pgFinish("run_attempt_cap",FALSE);
        PG_CHECK(HGMapRunReady(&ui,mapRunAttempts,mapRunRevision));
        PG_CHECK(ui.enabledMask&8);if(!mapSnapshot())return TRUE;
        if(mapRunAttempts)PG_CHECK(!w2c_hgengine_HGNativeBattleUIChoose(pgCapture->game,mapRunRevision,3));
        PG_CHECK(!w2c_hgengine_HGNativeBattleUIChoose(pgCapture->game,ui.revision-1,3));
        PG_CHECK(w2c_hgengine_HGNativeBattleUIChoose(pgCapture->game,ui.revision,3));
        PG_CHECK(!w2c_hgengine_HGNativeBattleUIChoose(pgCapture->game,ui.revision,3));
        mapRunRevision=ui.revision;++mapRunAttempts;++mapRuns;
        printf("MAP run: {\"launch\":%u,\"attempt\":%u,\"totalAttempts\":%u,\"revision\":%u,\"battleType\":%u,\"battleSpecial\":%u,\"index\":3}\n",mapRunLaunch,mapRunAttempts,mapRuns,ui.revision,mapFinal.battleType,mapFinal.battleSpecial);
    }
    return TRUE; // Original success and failure messages both advance their thirty-tick wait without A/B.
}
static void mapText(u32 script) {
    if(!HGMapTextAllowed(&mapFinal,script,mapFinal.scriptSerial) || mapFinal.scriptSerial==mapTextSerial)return;
    if(!mapSnapshot())return;mapTextSerial=mapFinal.scriptSerial;mapTextId=script;mapPulse(1,0,0,FALSE,3);
}
static void mapNavigate(const HGPokegearMapRouteSegment *route,u32 count) {
    if(!HGMapStationary(&mapFinal) || mapFinal.scriptActive)return;
    if(mapMovePending) {
        PG_CHECK(HGMapStepValid(mapMoveKey,mapMoveX,mapMoveZ,mapFinal.x,mapFinal.z));
        if(mapFinal.x==mapMoveX && mapFinal.z==mapMoveZ)PG_CHECK(++mapBlocked<=16);
        else {mapBlocked=0;++mapSteps;}
        mapMovePending=0;
    }
    while(mapSegment<count && mapFinal.x==route[mapSegment].x1 && mapFinal.z==route[mapSegment].z1)++mapSegment;
    if(mapSegment==count){mapSegment=0;mapNext();return;}
    u32 key=HGMapRouteKey(&route[mapSegment],mapFinal.x,mapFinal.z);PG_CHECK(key);
    mapMovePending=1;mapMoveKey=key;mapMoveX=mapFinal.x;mapMoveZ=mapFinal.z;mapPulse(key,0,0,FALSE,0);
}
static void MapCardPresent(void *argument) {
    pgCapture=argument;if(!pgStep){PGQAPresent(argument);return;}
    mapFinal=mapRead();u32 frame=OS_GetVBlankCount();if(!mapStageFrame)mapStageFrame=frame;
    if(frame-mapStageFrame>12000u)pgFinish("map_stage_timeout",FALSE);
    if(!mapSettle())return;
    if(mapHandleBattle())return;
    if(mapFinal.battleReturns){PG_CHECK(mapFinal.battleOutcome==BATTLE_OUTCOME_PLAYER_FLED && mapRuns>=mapFinal.battleLaunches);}
    if(mapStage==MAP_START) {
        if(!HGMapStationary(&mapFinal))return;
        if(!mapPending){if(!mapStartCanRequest(&mapFinal))return;
            PG_CHECK(!mapFinal.scene && !mapFinal.cards && mapFinal.x==668 && mapFinal.z==397 && mapFinal.mapId==33);
            printf("MAP baseline: {\"map\":%u,\"x\":%u,\"z\":%u,\"scene\":%u,\"cards\":%u}\n",mapFinal.mapId,mapFinal.x,mapFinal.z,mapFinal.scene,mapFinal.cards);
            PG_CHECK(w2c_hgengine_HGNativePokegearQARequest(pgCapture->game,mapFinal.revision,HG_PG_MAP_START));mapPending=1;return;}
        if(!mapFinal.started)return;if(!mapSnapshot())return;mapNext();return;
    }
    if(mapStage==MAP_GUIDE_ROUTE) {
        if(mapFinal.scriptId==2){PG_CHECK(mapFinal.mapId==67 && !mapFinal.scene && mapSegment==24 && mapFinal.x==566 && mapFinal.z==400);mapMovePending=0;mapSegment=0;mapNext();return;}
        mapNavigate(hgMapGuideRoute,sizeof(hgMapGuideRoute)/sizeof(*hgMapGuideRoute));return;
    }
    if(mapStage==MAP_TOUR) {
        if(mapFinal.scene==1 && HGMapStationary(&mapFinal)){PG_CHECK(mapFinal.mapId==67 && mapFinal.x==557 && mapFinal.z==404 && !mapFinal.cards);if(!mapSnapshot())return;mapNext();return;}
        mapText(2);return;
    }
    if(mapStage==MAP_CARD_ROUTE) {
        if(mapFinal.scriptId==3){PG_CHECK(mapFinal.mapId==67 && mapFinal.scene==1 && mapSegment==3 && mapFinal.x==550 && mapFinal.z==385);mapMovePending=0;mapSegment=0;mapNext();return;}
        mapNavigate(hgMapCardRoute,sizeof(hgMapCardRoute)/sizeof(*hgMapCardRoute));return;
    }
    if(mapStage==MAP_GIFT) {
        if(mapFinal.scene==2 && HGMapStationary(&mapFinal)){PG_CHECK(mapFinal.mapId==67 && mapFinal.cards==1);if(!mapSnapshot())return;mapNext();return;}
        mapText(3);return;
    }
    if(mapStage==MAP_MENU) {if(!HGMapStationary(&mapFinal) || pgMenu.active)return;mapNext();mapPulse(1024,0,0,FALSE,0);return;}
    if(mapStage==MAP_ICON) {
        if(!HGNativeFieldMenuUIInputAllowed(&pgMenu,pgMenu.ownerGeneration,pgMenu.revision))return;
        PG_CHECK(pgFinal.pokegearSlot<7 && ((pgFinal.menuIcons>>(pgFinal.pokegearSlot*4))&15)==3
            && HGNativeFieldMenuUITouchAllowed(&pgMenu,pgMenu.ownerGeneration,pgMenu.revision,pgFinal.pokegearX,pgFinal.pokegearY));
        if(!mapSnapshot())return;mapNext();mapPulse(0,pgFinal.pokegearX,pgFinal.pokegearY,TRUE,2);return;
    }
    if(mapStage==MAP_PHONE) {
        if(!pgView(&pgFinal,3,1))return;PG_CHECK(pgFinal.ui.registeredCards==1 && !HGNativePokegearUICardAvailable(1,1));
        PG_CHECK(!HGNativePokegearUITouchStartAllowed(&pgFinal.ui,pgFinal.ui.ownerGeneration,pgFinal.ui.revision,80,176));
        PG_CHECK(HGNativePokegearUITouchStartAllowed(&pgFinal.ui,pgFinal.ui.ownerGeneration,pgFinal.ui.revision,128,176));
        if(!mapSnapshot())return;mapNext();mapPulse(0,128,176,TRUE,1);return;
    }
    if(mapStage==MAP_VIEW) {if(!pgView(&pgFinal,2,1))return;PG_CHECK(mapFinal.scene==2 && mapFinal.cards==1 && pgFinal.ui.registeredCards==1);if(!mapSnapshot())return;mapNext();return;}
    if(mapStage==MAP_CLOSE) {
        if(!pgFinal.ui.active){PG_CHECK(mapClosePresses && pgFinal.ui.ownerGeneration==mapCloseBefore.ownerGeneration && pgFinal.ui.revision!=mapCloseBefore.revision && !pgFinal.ui.ready && !pgFinal.ui.viewActive);mapNext();return;}
        if(!pgView(&pgFinal,2,1))return;PG_CHECK(mapClosePresses<3);
        // Map may switch to BUTTONS and focus the footer on the same B; the next original B cancels.
        PG_CHECK(mapClosePresses==0 || pgFinal.menuInputState==0);
        if(mapClosePresses==2)PG_CHECK(pgFinal.cursorInAppSwitchZone);
        mapCloseBefore=pgFinal.ui;++mapClosePresses;mapPulse(2,0,0,FALSE,1);return;
    }
    if(mapStage==MAP_FIELD) {
        if(pgFinal.ui.active)return;
        if(pgMenu.active){if(!HGNativeFieldMenuUIInputAllowed(&pgMenu,pgMenu.ownerGeneration,pgMenu.revision))return;
            PG_CHECK(!mapMenuCloses);mapMenuBefore=pgMenu;++mapMenuCloses;mapPulse(2,0,0,FALSE,2);return;}
        if(!HGMapStationary(&mapFinal))return;PG_CHECK(mapMenuCloses==1 && pgMenuRetired(&pgMenu,mapMenuBefore.ownerGeneration,mapMenuBefore.revision));
        if(!mapSnapshot())return;mapNext();return;
    }
    if(mapStage==MAP_FINISH) {
        if(!HGMapStationary(&mapFinal))return;
        if(!mapPending){PG_CHECK(w2c_hgengine_HGNativePokegearQARequest(pgCapture->game,mapFinal.revision,HG_PG_MAP_FINISH));mapPending=1;return;}
        if(!mapFinal.done)return;PG_CHECK(mapFinal.cards==1 && mapFinal.scene==2 && mapFinal.mapReadySeen && mapFinal.parentEntries==1 && mapFinal.parentExits==1);
        pgFinish("passed_original_map_card",TRUE);
    }
}
int main(int argc,char **argv) {
    setvbuf(stdout,NULL,_IONBF,0);if(argc!=5 || !HGNativeFSSetRoot(argv[1]) || !HGNativeBackupSetPath(argv[3]))return 2;
    u8 header[0x160];FILE *input=fopen(argv[2],"rb");if(!input)return 2;
    BOOL valid=fread(header,1,sizeof(header),input)==sizeof(header) && fgetc(input)==EOF && !ferror(input);fclose(input);if(!valid)return 2;
    wasm_rt_init();w2c_hgengine *game=calloc(1,sizeof(*game));struct w2c_env *env=calloc(1,sizeof(*env));
    struct w2c_wasi__snapshot__preview1 *wasi=calloc(1,sizeof(*wasi));Capture *capture=calloc(1,sizeof(*capture));if(!game || !env || !wasi || !capture)return 2;
    wasi->fds[0]=dup(0);wasi->fds[1]=dup(1);wasi->fds[2]=dup(2);if(wasi->fds[0]<0 || wasi->fds[1]<0 || wasi->fds[2]<0)return 2;
    pgCapture=capture;capture->game=game;capture->env=env;
    if(!setjmp(pgResetLanding)) {
        wasm_rt_trap_t trap=wasm_rt_impl_try();PG_CHECK(!trap);wasm2c_hgengine_instantiate(game,env,wasi);env->memory=wasi->memory=w2c_hgengine_memory(game);PG_CHECK(HGNativeSoundRealtime(env,TRUE));
        env->irqTable=&game->w2c_T0;env->stackPointer=&game->w2c_0x5F_stack_pointer;env->rootStackHigh=*env->stackPointer;*env->stackPointer-=16;
        env->irqType0=wasm2c_hgengine_get_func_type(0,0);env->irqType1=wasm2c_hgengine_get_func_type(1,0,WASM_RT_I32);
        env->rtcType=wasm2c_hgengine_get_func_type(2,0,WASM_RT_I32,WASM_RT_I32);env->irqGuardType=wasm2c_hgengine_get_func_type(4,0,WASM_RT_I32,WASM_RT_I32,WASM_RT_I32,WASM_RT_I32);
        u32 system=w2c_hgengine_HGNativeSystemWorkAddress(game);memcpy((void *)pgGuest(system,0x360)+0x200,header,sizeof(header));w2c_hgengine_0x5Finitialize(game);
        u32 status=w2c_hgengine_HGNativeRegisterAddress(game,0x04000004,2);PG_CHECK(HGNativeStartDisplayClock(env,pgGuest(status,2)));
        *capture=(Capture){.game=game,.env=env,.folder=argv[4],.resume=TRUE,.noInput=TRUE};PG_CHECK(HGNativeSetDisplayObserver(env,MapCardPresent,capture));w2c_hgengine_NitroMain(game);PG_CHECK(FALSE);
    }
    BOOL quiescent=FALSE;if(pgResetExpected && env->resetRequested){HGNativeFinishReset(env);quiescent=env->resetFinished && !env->pendingDMA && !env->dmaAffineWrites && !env->pendingRTC && !env->pendingMIC && !env->pendingCARD && !env->pendingPOWER && !env->pendingINPUT && !env->irqTable && !env->threads && !env->rasterFunction;}
    printf("MAP evidence: {\"status\":\"%s\",\"stage\":%u,\"done\":%u,\"qaBytes\":%zu,\"cards\":%u,\"scene\":%u,\"steps\":%u,\"runs\":%u,\"runAttempts\":%u,\"runAttemptCap\":%u,\"battleLaunches\":%u,\"battleReturns\":%u,\"outcome\":%u,\"mapReadySeen\":%u,\"parentEntries\":%u,\"parentExits\":%u,\"lastApp\":%u,\"menuCloses\":%u,\"fieldReady\":%u,\"menuActive\":%u,\"privateSaveWrites\":false,\"resetQuiescent\":%s}\n",pgStatus,mapStage,mapFinal.done,sizeof(mapFinal),mapFinal.cards,mapFinal.scene,mapSteps,mapRuns,mapRuns,HG_PG_MAP_RUN_ATTEMPT_CAP,mapFinal.battleLaunches,mapFinal.battleReturns,mapFinal.battleOutcome,mapFinal.mapReadySeen,mapFinal.parentEntries,mapFinal.parentExits,mapFinal.lastApp,mapMenuCloses,pgFinal.fieldReady,pgMenu.active,quiescent ? "true" : "false");
    BOOL passed=pgPassed && quiescent && mapFinal.done && mapFinal.cards==1 && mapFinal.scene==2 && mapFinal.lastApp==2 && mapRuns>=mapFinal.battleLaunches && mapFinal.battleLaunches==mapFinal.battleReturns && mapMenuCloses==1 && !pgMenu.active && pgFinal.fieldReady;
    wasm2c_hgengine_free(game);for(u32 i=0;i<3;++i)close(wasi->fds[i]);free(game);free(env);free(wasi);free(capture);wasm_rt_free();return passed ? 0 : 1;
}
