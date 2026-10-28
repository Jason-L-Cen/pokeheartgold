// SPDX-License-Identifier: GPL-3.0-or-later
#define main OakBootstrapMain
#define HGEngineTrap OakBootstrapTrap
#include "engine_check.c"
#undef HGEngineTrap
#undef main
#include "oak_qa.h"
#include <nitro/os/native_reset.h>
#include <setjmp.h>
#include <errno.h>
static Capture *oqCapture;
static HGNativeOakQAState oqFinal;
static jmp_buf oqLanding;
static BOOL oqResetExpected,oqPassed;
static const char *oqStatus="not_started";
static u32 oqStage,oqStageFrame,oqLease,oqReads,oqReleaseReads,oqOwner,oqGen,oqRev,oqLCD,oqKey,oqX,oqY;
static BOOL oqTouch;
static u32 oqSnapshots,oqStale,oqTitleSent,oqTitleGen,oqTitleExit,oqControlDone,oqAdventureDone,oqGenderNo,oqNameNo;
static u32 oqExpected[2],oqPhases[4],oqCaptured[4],oqBallFrame,oqBallSnapshots;
static void oqFinish(const char *status,BOOL passed){oqStatus=status;oqPassed=passed;if(oqCapture && oqCapture->env && oqCapture->env->memory){HGNativeSetInput(oqCapture->env,0,FALSE,0,0,FALSE);oqResetExpected=TRUE;w2c_env_HGNativeResetControl(oqCapture->env,HG_RESET_REQUEST,0);}fprintf(stderr,"Oak failed before reset: %s\n",status);_Exit(1);}
#define OQ_CHECK(v) do{if(!(v))oqFinish("assert:" #v,FALSE);}while(0)
void HGEngineTrap(wasm_rt_trap_t code){if(code==WASM_RT_TRAP_UNREACHABLE && oqResetExpected && oqCapture && oqCapture->env->resetRequested)longjmp(oqLanding,1);OakBootstrapTrap(code);}
static const void *oqGuest(u32 address,size_t size){OQ_CHECK(address && address<=oqCapture->env->memory->size && size<=oqCapture->env->memory->size-address);return oqCapture->env->memory->data+address;}
static HGNativeOakQAState oqRead(void){HGNativeOakQAState s;memcpy(&s,oqGuest(w2c_hgengine_HGNativeOakQAGetState(oqCapture->game),sizeof(s)),sizeof(s));
/* OAK_DIAGNOSTIC_BEGIN */
if (!(s.version==HG_OAK_QA_VERSION && s.byteSize==sizeof(s) && !s.error)) {
    fprintf(stderr,"OAK bad-QA: {\"version\":%u,\"byteSize\":%u,\"error\":%u,\"stage\":%u,\"mainEmpty\":%u,\"mainSelectedNewGame\":%u,\"mainRouteMask\":%u,\"mainTerminal\":%u,\"bootKind\":%u,\"bootActive\":%u,\"bootPhase\":%u,\"bootExit\":%u,\"bootReady\":%u,\"bootOwnerGeneration\":%u,\"bootRevision\":%u}\n",s.version,s.byteSize,s.error,oqStage,s.mainEmpty,s.mainSelectedNewGame,s.mainRouteMask,s.mainTerminal,s.boot.kind,s.boot.active,s.boot.phase,s.boot.exit,s.boot.ready,s.boot.ownerGeneration,s.boot.revision);
    char path[4096];int n=snprintf(path,sizeof(path),"%s/failed-qa-state.bin",oqCapture->folder);
    if(n>0 && n<sizeof(path)) {
        FILE *f=fopen(path,"wbx");
        if(f){size_t bytes=fwrite(&s,1,sizeof(s),f);int closed=fclose(f);fprintf(stderr,"OAK diagnostic-dump: {\"bytes\":%zu,\"closeResult\":%d}\n",bytes,closed);}
        else fprintf(stderr,"OAK diagnostic-dump-open-error: %d\n",errno);
    }
}
/* OAK_DIAGNOSTIC_END */
OQ_CHECK(s.version==HG_OAK_QA_VERSION && s.byteSize==sizeof(s) && !s.error);memcpy(&s.ui,oqGuest(w2c_hgengine_HGNativeOakUIGetState(oqCapture->game),sizeof(s.ui)),sizeof(s.ui));memcpy(&s.naming,oqGuest(w2c_hgengine_HGNativeNamingUIGetState(oqCapture->game),sizeof(s.naming)),sizeof(s.naming));return s;}
static u32 oqTouchEngine(void){u16 control;memcpy(&control,oqGuest(w2c_hgengine_HGNativeRegisterAddress(oqCapture->game,0x04000304,2),2),2);return (control>>15)&1;}
static BOOL oqSnapshot(const char *label,BOOL blankAllowed){
    HGNativeOakQAState before=oqRead();static u8 pixels[2][256*192*4];u32 rendered=0,varied=0;
    if(!oqCapture->buffer)oqCapture->buffer=w2c_hgengine_HGNativeFrameBuffer(oqCapture->game);HGNativeDrainDMA();
    for(u32 engine=0;engine<2;engine++)if(w2c_hgengine_HGNativeRender2D(oqCapture->game,engine,oqCapture->buffer,sizeof(pixels[0]))){rendered|=1u<<engine;memcpy(pixels[engine],oqGuest(oqCapture->buffer,sizeof(pixels[0])),sizeof(pixels[0]));for(u32 i=1;i<256*192;i++)if(memcmp(pixels[engine],pixels[engine]+i*4,4)){varied|=1u<<engine;break;}}
    u32 needed=before.ui.active || before.naming.active ? 3 : 1;if((rendered&needed)!=needed || (!blankAllowed && (varied&needed)!=needed))return FALSE;
    HGNativeOakQAState after=oqRead();if(memcmp(&before,&after,sizeof(before)))return FALSE;
    char path[4096];OQ_CHECK(snprintf(path,sizeof(path),"%s/oak-%03u-state.bin",oqCapture->folder,oqSnapshots)<sizeof(path));FILE *f=fopen(path,"wbx");OQ_CHECK(f && fwrite(&after,1,sizeof(after),f)==sizeof(after) && !fclose(f));
    for(u32 engine=0;engine<2;engine++)if(rendered&(1u<<engine)){OQ_CHECK(snprintf(path,sizeof(path),"%s/oak-%03u-engine-%u.ppm",oqCapture->folder,oqSnapshots,engine)<sizeof(path));f=fopen(path,"wbx");OQ_CHECK(f && fprintf(f,"P6\n256 192\n255\n")>0);for(u32 row=0;row<192;row++){u8 rgb[256*3];for(u32 x=0;x<256;x++)memcpy(rgb+x*3,pixels[engine]+(row*256+x)*4,3);OQ_CHECK(fwrite(rgb,1,sizeof(rgb),f)==sizeof(rgb));}OQ_CHECK(!fclose(f));}
    printf("OAK snapshot: {\"sequence\":%u,\"stage\":%u,\"label\":\"%s\",\"phase\":%u,\"parentPhase\":%u,\"frame\":%u,\"renderedMask\":%u,\"variedMask\":%u,\"blankAllowed\":%s}\n",oqSnapshots,oqStage,label,after.ui.phase,after.ui.parentPhase,OS_GetVBlankCount(),rendered,varied,blankAllowed ? "true" : "false");++oqSnapshots;return TRUE;
}
static BOOL oqOwnerValid(void){
    if(oqOwner==1)return HGOakTitleReady(&oqFinal.boot) && oqFinal.boot.ownerGeneration==oqGen && oqFinal.boot.revision==oqRev;
    if(oqOwner==2)return HGNativeOakUIOwnsInput(&oqFinal.ui,oqFinal.naming.active) && (oqTouch ? HGNativeOakUITouchAllowed(&oqFinal.ui,oqGen,oqRev,oqLCD,oqX,oqY) : HGNativeOakUIKeyAllowed(&oqFinal.ui,oqGen,oqRev,oqKey));
    return oqOwner==3 && HGOakNamingReady(&oqFinal,oqGen,oqRev);
}
static void oqPulse(u32 owner,u32 key,u32 x,u32 y,BOOL touch){
    OQ_CHECK(!oqLease);oqOwner=owner;oqKey=key;oqX=x;oqY=y;oqTouch=touch;oqLCD=oqTouchEngine();
    oqGen=owner==1 ? oqFinal.boot.ownerGeneration : owner==2 ? oqFinal.ui.ownerGeneration : oqFinal.naming.ownerGeneration;
    oqRev=owner==1 ? oqFinal.boot.revision : owner==2 ? oqFinal.ui.revision : oqFinal.naming.revision;
    OQ_CHECK(oqOwnerValid());if(touch)OQ_CHECK(oqLCD==(owner==2 ? 1u : 0u));
    OQ_CHECK(HGNativeSetInput(oqCapture->env,key,FALSE,x,y,touch));oqReads=HGNativeInputPadReads(oqCapture->env);oqLease=1;
    printf("OAK input: {\"stage\":%u,\"owner\":%u,\"key\":%u,\"x\":%u,\"y\":%u,\"touch\":%u,\"reads\":%u,\"generation\":%u,\"revision\":%u}\n",oqStage,owner,key,x,y,touch,oqReads,oqGen,oqRev);
}
static BOOL oqSettle(void){
    if(!oqLease)return TRUE;u32 reads=HGNativeInputPadReads(oqCapture->env);
    if(oqLease==1 && (!oqOwnerValid() || oqTouchEngine()!=oqLCD || reads-oqReads>=4)){
        OQ_CHECK(HGNativeSetInput(oqCapture->env,0,FALSE,0,0,FALSE));oqReleaseReads=reads;oqLease=2;return FALSE;
    }
    if(oqLease!=2 || reads-oqReleaseReads<2)return FALSE;oqLease=0;return TRUE;
}
static void oqNext(void){++oqStage;oqStageFrame=OS_GetVBlankCount();}
static BOOL oqOakChoice(u32 phase,u32 index,const char *label){u32 x=0,y=0;HGNativeOakUIState *s=&oqFinal.ui;if(s->phase!=phase || !HGNativeOakUIOwnsInput(s,oqFinal.naming.active) || !HGNativeOakUIResolveChoice(s,s->ownerGeneration,s->revision,oqTouchEngine(),index,&x,&y))return FALSE;OQ_CHECK(!HGNativeOakUIResolveChoice(s,s->ownerGeneration,s->revision-1,oqTouchEngine(),index,&x,&y));++oqStale;OQ_CHECK(HGNativeOakUIResolveChoice(s,s->ownerGeneration,s->revision,oqTouchEngine(),index,&x,&y));if(!oqSnapshot(label,FALSE))return FALSE;oqPulse(2,0,x,y,TRUE);return TRUE;}
static BOOL oqAdvance(void){HGNativeOakUIState *s=&oqFinal.ui;if(!HGNativeOakUIOwnsInput(s,oqFinal.naming.active) || (s->mode!=HG_OAK_DIALOG && s->mode!=HG_OAK_PAGE) || !HGNativeOakUIKeyAllowed(s,s->ownerGeneration,s->revision,1))return FALSE;oqPulse(2,1,0,0,FALSE);return TRUE;}
static BOOL oqNameReady(u32 entry){return oqFinal.namingEntries==entry && HGOakNamingReady(&oqFinal,oqFinal.naming.ownerGeneration,oqFinal.naming.revision);}
static void OakPresent(void *argument){
    oqCapture=argument;oqFinal=oqRead();u32 frame=OS_GetVBlankCount();if(!oqStageFrame)oqStageFrame=frame;if(frame-oqStageFrame>18000)oqFinish("stage_timeout",FALSE);
    if(oqTitleSent && oqFinal.boot.kind==HG_YN_BOOT_TITLE && !oqFinal.boot.active && oqFinal.boot.phase==6 && oqFinal.boot.ownerGeneration==oqTitleGen && oqFinal.boot.exit==1)oqTitleExit=1;
    if(oqFinal.ui.active && oqFinal.ui.phase<128){u32 p=oqFinal.ui.phase;oqPhases[p/32]|=1u<<(p%32);if(!(oqCaptured[p/32]&(1u<<(p%32))) && oqSnapshot("phase",TRUE))oqCaptured[p/32]|=1u<<(p%32);if(p==52){if(!oqBallFrame)oqBallFrame=frame;if(oqBallSnapshots<3 && frame-oqBallFrame>=oqBallSnapshots*8 && oqSnapshot("ball",TRUE))++oqBallSnapshots;}}
    if(!oqSettle())return;
    if(oqStage==0){
        if(oqFinal.ui.active && oqFinal.ui.phase==3 && oqFinal.ui.ready){OQ_CHECK(oqTitleSent && oqTitleExit && oqFinal.mainEmpty && oqFinal.mainSelectedNewGame && oqFinal.mainTerminal && (oqFinal.mainRouteMask&392)==392);oqNext();return;}
        if(!oqTitleSent && HGOakTitleReady(&oqFinal.boot)){if(!oqSnapshot("title",FALSE))return;oqTitleSent=1;oqTitleGen=oqFinal.boot.ownerGeneration;oqPulse(1,1,0,0,FALSE);}return;
    }
    if(oqStage==1){if(oqOakChoice(3,0,"controls"))oqNext();return;}
    if(oqStage==2){if(oqOakChoice(24,0,"controls-understood"))oqNext();else oqAdvance();return;}
    if(oqStage==3){if(oqFinal.ui.phase==3 && oqFinal.ui.ready){for(u32 p=11;p<=24;p++)if(p!=10 && p!=25 && p!=26)OQ_CHECK(oqPhases[p/32]&(1u<<(p%32)));oqControlDone=1;if(oqOakChoice(3,1,"adventure"))oqNext();}return;}
    if(oqStage==4){if(oqFinal.ui.phase==3 && oqFinal.ui.ready){for(u32 p=36;p<=41;p++)OQ_CHECK(oqPhases[p/32]&(1u<<(p%32)));oqAdventureDone=1;if(oqOakChoice(3,2,"start-adventure"))oqNext();}else oqAdvance();return;}
    if(oqStage==5){if(oqOakChoice(65,0,"boy-first"))oqNext();else oqAdvance();return;}
    if(oqStage==6){if(oqOakChoice(69,1,"gender-no")){oqGenderNo=1;oqNext();}else oqAdvance();return;}
    if(oqStage==7){if(oqOakChoice(65,0,"boy-retry")){OQ_CHECK((oqPhases[2]&((1u<<7)|(1u<<8)))==((1u<<7)|(1u<<8)));oqNext();}else oqAdvance();return;}
    if(oqStage==8 || oqStage==14){if(oqOakChoice(69,0,"gender-yes"))oqNext();else oqAdvance();return;}
    if(oqStage==9 || oqStage==15){u32 entry=oqStage==9 ? 1 : 2,index=entry-1;if(oqNameReady(entry) && oqFinal.edit.glyphCount>=2 && !oqFinal.edit.length){OQ_CHECK(oqFinal.edit.glyph[0] && oqFinal.edit.glyph[0]!=oqFinal.edit.glyph[1]);oqExpected[index]=oqFinal.edit.glyph[index];if(!oqSnapshot("keyboard-glyph",FALSE))return;oqPulse(3,0,oqFinal.edit.glyphX[index],oqFinal.edit.glyphY[index],TRUE);oqNext();}return;}
    if(oqStage==10 || oqStage==16){u32 entry=oqStage==10 ? 1 : 2;if(oqNameReady(entry) && oqFinal.edit.length==1 && oqFinal.edit.name[0]==oqExpected[entry-1]){if(!oqSnapshot("entered-glyph",FALSE))return;oqPulse(3,8,0,0,FALSE);oqNext();}return;}
    if(oqStage==11 || oqStage==17){u32 entry=oqStage==11 ? 1 : 2;if(oqNameReady(entry) && oqFinal.edit.confirmReady && oqFinal.edit.length==1){if(!oqSnapshot("naming-ok",FALSE))return;oqPulse(3,1,0,0,FALSE);oqNext();}return;}
    if(oqStage==12 || oqStage==18){u32 entry=oqStage==12 ? 1 : 2;if(oqFinal.namingExits==entry && !oqFinal.naming.active){OQ_CHECK(oqFinal.resultName[0]==oqExpected[entry-1] && oqFinal.resultName[1]==0xFFFF);if(oqOakChoice(98,entry==1 ? 1 : 0,entry==1 ? "name-no" : "name-yes")){if(entry==1)oqNameNo=1;oqNext();}else oqAdvance();}return;}
    if(oqStage==13){OQ_CHECK(oqFinal.namingEntries==1);if(oqOakChoice(65,0,"boy-after-name-no"))oqNext();else oqAdvance();return;}
    if(oqStage==19){if(oqAdvance())return;if(!HGOakFieldReady(&oqFinal))return;OQ_CHECK(oqFinal.x==6 && oqFinal.z==6 && oqFinal.facing==1 && !oqFinal.partyCount && !oqFinal.gotStarter && !oqFinal.trainerGender && oqFinal.profileName[0]==oqExpected[1] && oqFinal.profileName[1]==0xFFFF && oqFinal.oakEntries==1 && oqFinal.oakExits==1 && oqFinal.namingEntries==2 && oqFinal.namingExits==2 && oqControlDone && oqAdventureDone && oqGenderNo && oqNameNo && oqBallSnapshots==3);if(!oqSnapshot("newgame-field",FALSE))return;oqFinish("passed_original_oak_newgame",TRUE);}
}
static void oqPlatformBaseline(const char *save,const char *folder){
    const char *suffixes[]={"",".console",".wifi",".lock"};char source[4096],target[4096];
    for(unsigned i=0;i<4;i++){OQ_CHECK(snprintf(source,sizeof(source),"%s%s",save,suffixes[i])<sizeof(source));FILE *f=fopen(source,"rb");if(!f){OQ_CHECK(i && errno==ENOENT);continue;}OQ_CHECK(snprintf(target,sizeof(target),"%s/before%s",folder,suffixes[i])<sizeof(target));FILE *out=fopen(target,"wbx");OQ_CHECK(out);size_t count=0;int c;while((c=fgetc(f))!=EOF){if(!i)OQ_CHECK(c==255);OQ_CHECK(fputc(c,out)!=EOF);count++;}OQ_CHECK(!ferror(f) && !fclose(f) && !fclose(out));if(!i)OQ_CHECK(count==524288);printf("OAK platform: {\"suffix\":\"%s\",\"bytes\":%zu}\n",suffixes[i],count);}
}
int main(int argc,char **argv){
    setvbuf(stdout,NULL,_IONBF,0);if(argc!=6 || access(argv[3],F_OK)==0 || !HGNativeFSSetRoot(argv[1]) || !HGNativeBackupSetPath(argv[3]))return 2;oqPlatformBaseline(argv[3],argv[5]);
    u8 header[0x160];FILE *input=fopen(argv[2],"rb");if(!input)return 2;BOOL valid=fread(header,1,sizeof(header),input)==sizeof(header) && fgetc(input)==EOF && !ferror(input);fclose(input);if(!valid)return 2;
    wasm_rt_init();w2c_hgengine *game=calloc(1,sizeof(*game));struct w2c_env *env=calloc(1,sizeof(*env));struct w2c_wasi__snapshot__preview1 *wasi=calloc(1,sizeof(*wasi));Capture *capture=calloc(1,sizeof(*capture));if(!game || !env || !wasi || !capture)return 2;
    wasi->fds[0]=dup(0);wasi->fds[1]=dup(1);wasi->fds[2]=dup(2);if(wasi->fds[0]<0 || wasi->fds[1]<0 || wasi->fds[2]<0)return 2;oqCapture=capture;capture->game=game;capture->env=env;
    if(!setjmp(oqLanding)){wasm_rt_trap_t trap=wasm_rt_impl_try();OQ_CHECK(!trap);wasm2c_hgengine_instantiate(game,env,wasi);env->memory=wasi->memory=w2c_hgengine_memory(game);OQ_CHECK(HGNativeSoundRealtime(env,TRUE));env->irqTable=&game->w2c_T0;env->stackPointer=&game->w2c_0x5F_stack_pointer;env->rootStackHigh=*env->stackPointer;*env->stackPointer-=16;env->irqType0=wasm2c_hgengine_get_func_type(0,0);env->irqType1=wasm2c_hgengine_get_func_type(1,0,WASM_RT_I32);env->rtcType=wasm2c_hgengine_get_func_type(2,0,WASM_RT_I32,WASM_RT_I32);env->irqGuardType=wasm2c_hgengine_get_func_type(4,0,WASM_RT_I32,WASM_RT_I32,WASM_RT_I32,WASM_RT_I32);u32 system=w2c_hgengine_HGNativeSystemWorkAddress(game);memcpy((void *)oqGuest(system,0x360)+0x200,header,sizeof(header));w2c_hgengine_0x5Finitialize(game);u32 status=w2c_hgengine_HGNativeRegisterAddress(game,0x04000004,2);OQ_CHECK(HGNativeStartDisplayClock(env,oqGuest(status,2)));*capture=(Capture){.game=game,.env=env,.folder=argv[4],.resume=TRUE,.noInput=TRUE};OQ_CHECK(HGNativeSetDisplayObserver(env,OakPresent,capture));puts("OAK game-started: true");w2c_hgengine_NitroMain(game);OQ_CHECK(FALSE);}
    BOOL quiescent=FALSE;if(oqResetExpected && env->resetRequested){HGNativeFinishReset(env);quiescent=env->resetFinished && !env->pendingDMA && !env->dmaAffineWrites && !env->pendingRTC && !env->pendingMIC && !env->pendingCARD && !env->pendingPOWER && !env->pendingINPUT && !env->irqTable && !env->threads && !env->rasterFunction;}
    printf("OAK evidence: {\"status\":\"%s\",\"stage\":%u,\"controls\":%u,\"adventure\":%u,\"genderNo\":%u,\"nameNo\":%u,\"oakEntries\":%u,\"oakExits\":%u,\"namingEntries\":%u,\"namingExits\":%u,\"finalName\":%u,\"expectedName\":%u,\"gender\":%u,\"party\":%u,\"gotStarter\":%u,\"map\":%u,\"x\":%u,\"z\":%u,\"fieldReady\":%u,\"ballSnapshots\":%u,\"snapshots\":%u,\"resetQuiescent\":%s}\n",oqStatus,oqStage,oqControlDone,oqAdventureDone,oqGenderNo,oqNameNo,oqFinal.oakEntries,oqFinal.oakExits,oqFinal.namingEntries,oqFinal.namingExits,oqFinal.profileName[0],oqExpected[1],oqFinal.trainerGender,oqFinal.partyCount,oqFinal.gotStarter,oqFinal.mapId,oqFinal.x,oqFinal.z,oqFinal.fieldReady,oqBallSnapshots,oqSnapshots,quiescent ? "true" : "false");
    BOOL passed=oqPassed && quiescent;wasm2c_hgengine_free(game);for(u32 i=0;i<3;i++)close(wasi->fds[i]);free(game);free(env);free(wasi);free(capture);wasm_rt_free();return passed ? 0 : 1;
}
