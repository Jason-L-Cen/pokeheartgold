// SPDX-License-Identifier: GPL-3.0-or-later
#define main PartyBootstrapMain
#define HGEngineTrap PartyBootstrapTrap
#include "engine_check.c"
#undef HGEngineTrap
#undef main
#include "native_party_ui.h"
#include "constants/pokemon.h"
#include <nitro/os/native_reset.h>
#include <setjmp.h>

#define QA_CHECK(value) do { if (!(value)) { fprintf(stderr,"Party menu failure line %d: %s\n",__LINE__,#value); _Exit(1); } } while (0)
typedef struct PartyChoice { u32 revision,phase,row,kind; } PartyChoice;
static jmp_buf resetLanding;
static struct w2c_env *partyEnv;
static BOOL resetExpected,casePassed;
static const char *caseStatus="not_started";
static u32 iconCenters[7][2],stage,stageFrame,pulseFrame,pulseReleased,selectedSlot;
static u32 starter,partyCount,gotDex,dexEnabled,dexSeen,dexCaught,seenPhases,staleRejected,snapshotStages;
static u32 saveAddress,flagsAddress,partyAddress,dexAddress,bagAddress;
enum { MON_ATTRIBUTES=26 };
static const u32 monAttributes[MON_ATTRIBUTES]={MON_DATA_PERSONALITY,MON_DATA_OT_ID,MON_DATA_SPECIES,MON_DATA_FORM,MON_DATA_IS_EGG,MON_DATA_HELD_ITEM,
    MON_DATA_HP,MON_DATA_MAX_HP,MON_DATA_LEVEL,MON_DATA_STATUS,MON_DATA_MOVE1,MON_DATA_MOVE2,MON_DATA_MOVE3,MON_DATA_MOVE4,
    MON_DATA_MOVE1_PP,MON_DATA_MOVE2_PP,MON_DATA_MOVE3_PP,MON_DATA_MOVE4_PP,MON_DATA_MOVE1_PP_UPS,MON_DATA_MOVE2_PP_UPS,MON_DATA_MOVE3_PP_UPS,MON_DATA_MOVE4_PP_UPS,
    MON_DATA_MOVE1_MAX_PP,MON_DATA_MOVE2_MAX_PP,MON_DATA_MOVE3_MAX_PP,MON_DATA_MOVE4_MAX_PP};
static u32 originalMonData[HG_PARTY_UI_SLOTS][MON_ATTRIBUTES],bagSize;
static u8 *originalBag;
static PartyChoice oldRoster;

void HGEngineTrap(wasm_rt_trap_t code) {
    if (code==WASM_RT_TRAP_UNREACHABLE && resetExpected && partyEnv && partyEnv->resetRequested) longjmp(resetLanding,1);
    PartyBootstrapTrap(code);
}
static const void *guest(Capture *capture,u32 address,size_t size) {
    QA_CHECK(address && address<=capture->env->memory->size && size<=capture->env->memory->size-address);
    return capture->env->memory->data+address;
}
static HGNativePartyUIState readParty(Capture *capture) {
    static u32 lastRevision;
    u32 address=w2c_hgengine_HGNativePartyUIGetState(capture->game);
    HGNativePartyUIState result; memcpy(&result,guest(capture,address,sizeof(result)),sizeof(result));
    QA_CHECK(w2c_hgengine_HGNativePartyUIGetState(capture->game)==address);
    QA_CHECK(!memcmp(&result,guest(capture,address,sizeof(result)),sizeof(result)));
    QA_CHECK(result.count<=HG_PARTY_UI_SLOTS && result.actionCount<=HG_PARTY_UI_ACTIONS && !(result.slotMask&~63u));
    QA_CHECK(!(result.actionEnabledMask&~((1u<<result.actionCount)-1)) && result.revision>=lastRevision);
    lastRevision=result.revision;
    return result;
}
static void pulse(Capture *capture,u32 keys,u32 x,u32 y,BOOL touch) {
    QA_CHECK(!pulseFrame && x<256 && y<192 && HGNativeSetInput(capture->env,keys,FALSE,x,y,touch));
    pulseFrame=OS_GetVBlankCount(); pulseReleased=0;
}
static void choose(Capture *capture,HGNativePartyUIState state,u32 row,u32 kind) {
    HGNativePartyUIState current=readParty(capture); u32 x,y;
    QA_CHECK(HGNativePartyUIResolveTouch(&current,state.revision,state.phase,kind,row,&x,&y));
    if (kind==HG_PARTY_UI_TOUCH_CANCEL) { pulse(capture,2,0,0,FALSE); return; }
    pulse(capture,0,x,y,TRUE);
}
static void saveBytes(Capture *capture,const char *name,const void *bytes,size_t size) {
    char path[4096]; QA_CHECK(snprintf(path,sizeof(path),"%s/%s",capture->folder,name)<sizeof(path));
    FILE *file=fopen(path,"wb"); QA_CHECK(file && fwrite(bytes,1,size,file)==size && !fclose(file));
}
static void windowEvidence(Capture *capture,u32 phase,const char *role,u32 row,const HGNativePartyUIWindow *window) {
    if (!window->pixelBuffer) return;
    QA_CHECK(window->tileWidth && window->tileWidth<=32 && window->tileHeight && window->tileHeight<=4 && window->clearColor<16 && window->engine<2);
    size_t size=window->tileWidth*window->tileHeight*32;
    char name[128]; QA_CHECK(snprintf(name,sizeof(name),"stage-%02u-%s-%u.4bpp",snapshotStages,role,row)<sizeof(name));
    saveBytes(capture,name,guest(capture,window->pixelBuffer,size),size);
    printf("Party window evidence: {\"stage\":%u,\"phase\":%u,\"role\":\"%s\",\"row\":%u,\"file\":\"%s\",\"pixelBuffer\":%u,\"tileWidth\":%u,\"tileHeight\":%u,\"clearColor\":%u,\"engine\":%u,\"palette\":[",
        snapshotStages,phase,role,row,name,window->pixelBuffer,window->tileWidth,window->tileHeight,window->clearColor,window->engine);
    for (u32 i=0;i<16;i++) printf("%s%u",i ? "," : "",window->palette[i]);
    puts("]}");
}
static BOOL renderBoth(Capture *capture,BOOL write) {
    if (!capture->buffer) capture->buffer=w2c_hgengine_HGNativeFrameBuffer(capture->game);
    guest(capture,capture->buffer,256*192*4); HGNativeDrainDMA();
    u32 geometry[8]={0};
    if (w2c_hgengine_HGNativeGeometryFrame(capture->game,1,capture->buffer)) memcpy(geometry,guest(capture,capture->buffer,sizeof(geometry)),sizeof(geometry));
    for (u32 engine=0;engine<2;engine++) {
        if (!w2c_hgengine_HGNativeRender2D(capture->game,engine,capture->buffer,256*192*4)) return FALSE;
        const u8 *pixels=guest(capture,capture->buffer,256*192*4); BOOL varied=FALSE;
        for (u32 i=1;i<256*192;i++) if (memcmp(pixels,pixels+i*4,4)) { varied=TRUE; break; }
        if (!engine && !varied) return FALSE;
        if (!write) continue;
        char path[4096]; QA_CHECK(snprintf(path,sizeof(path),"%s/stage-%02u-engine-%u.ppm",capture->folder,snapshotStages,engine)<sizeof(path));
        FILE *file=fopen(path,"wb"); QA_CHECK(file && fprintf(file,"P6\n256 192\n255\n")>0);
        for (u32 i=0;i<256*192;i++) QA_CHECK(fwrite(pixels+i*4,1,3,file)==3);
        QA_CHECK(!fclose(file));
        printf("Party frame evidence: {\"stage\":%u,\"engine\":%u,\"frame\":%u,\"nonuniform\":%u,\"vertices\":%u,\"polygons\":%u,\"geometrySerial\":%u}\n",snapshotStages,engine,OS_GetVBlankCount(),varied,geometry[0],geometry[1],geometry[2]);
    }
    return TRUE;
}
static BOOL snapshot(Capture *capture,HGNativePartyUIState state) {
    if (!renderBoth(capture,TRUE)) return FALSE;
    char name[64]; QA_CHECK(snprintf(name,sizeof(name),"stage-%02u-state.bin",snapshotStages)<sizeof(name));
    saveBytes(capture,name,&state,sizeof(state));
    for (u32 i=0;i<HG_PARTY_UI_SLOTS;i++) if (state.slotMask&(1u<<i)) {
        QA_CHECK(state.mons[i].active && state.mons[i].nickname.pixelBuffer);
        windowEvidence(capture,state.phase,"nickname",i,&state.mons[i].nickname);
    }
    for (u32 i=0;i<state.actionCount;i++) {
        QA_CHECK(state.actions[i].text.pixelBuffer);
        printf("Party action evidence: {\"stage\":%u,\"phase\":%u,\"row\":%u,\"textId\":%u,\"enabled\":%u,\"hitX\":%u,\"hitY\":%u}\n",snapshotStages,state.phase,i,state.actions[i].textId,state.actions[i].enabled,state.actions[i].hitX,state.actions[i].hitY);
        windowEvidence(capture,state.phase,"action",i,&state.actions[i].text);
    }
    if (state.actionCount) QA_CHECK(state.selectedName.pixelBuffer && state.selectedItem.pixelBuffer);
    windowEvidence(capture,state.phase,"selected-name",0,&state.selectedName); windowEvidence(capture,state.phase,"selected-item",0,&state.selectedItem); windowEvidence(capture,state.phase,"message",0,&state.message);
    HGNativePartyUIState after=readParty(capture); QA_CHECK(!memcmp(&state,&after,sizeof(state)));
    printf("Party snapshot evidence: {\"stage\":%u,\"active\":%u,\"ready\":%u,\"revision\":%u,\"phase\":%u,\"count\":%u,\"selected\":%u,\"actionCount\":%u,\"actionEnabledMask\":%u}\n",snapshotStages,state.active,state.ready,state.revision,state.phase,state.count,state.selected,state.actionCount,state.actionEnabledMask);
    if (state.active) seenPhases|=1u<<state.phase;
    ++snapshotStages; return TRUE;
}
static void semanticEvidence(Capture *capture,BOOL baseline) {
    u32 count=w2c_hgengine_Party_GetCount(capture->game,partyAddress),current[HG_PARTY_UI_SLOTS][MON_ATTRIBUTES]={{0}};
    QA_CHECK(count<=HG_PARTY_UI_SLOTS);
    for (u32 i=0;i<count;i++) {
        u32 mon=w2c_hgengine_Party_GetMonByIndex(capture->game,partyAddress,i); guest(capture,mon,1);
        for (u32 j=0;j<MON_ATTRIBUTES;j++) current[i][j]=w2c_hgengine_GetMonData(capture->game,mon,monAttributes[j],0);
    }
    u32 currentStarter=w2c_hgengine_CheckGotStarter(capture->game,flagsAddress),currentGotDex=w2c_hgengine_CheckGotPokedex(capture->game,flagsAddress);
    u32 currentEnabled=w2c_hgengine_Pokedex_IsEnabled(capture->game,dexAddress);
    u32 seen=w2c_hgengine_Pokedex_CountNationalDexSeen(capture->game,dexAddress),caught=w2c_hgengine_Pokedex_CountNationalDexOwned(capture->game,dexAddress);
    if (baseline) {
        starter=currentStarter; partyCount=count; gotDex=currentGotDex; dexEnabled=currentEnabled; dexSeen=seen; dexCaught=caught;
        memcpy(originalMonData,current,sizeof(current));
        bagSize=w2c_hgengine_Save_Bag_sizeof(capture->game); QA_CHECK(bagSize);
        const void *bag=guest(capture,bagAddress,bagSize);
        originalBag=malloc(bagSize); QA_CHECK(originalBag); memcpy(originalBag,bag,bagSize);
    } else {
        QA_CHECK(count==partyCount && !memcmp(originalMonData,current,sizeof(current)));
        QA_CHECK(currentStarter==starter && currentGotDex==gotDex && currentEnabled==dexEnabled && seen==dexSeen && caught==dexCaught);
        QA_CHECK(bagSize==w2c_hgengine_Save_Bag_sizeof(capture->game) && !memcmp(originalBag,guest(capture,bagAddress,bagSize),bagSize));
    }
    saveBytes(capture,baseline ? "bag-before.bin" : "bag-after.bin",guest(capture,bagAddress,bagSize),bagSize);
    printf("Party semantic evidence: {\"step\":\"%s\",\"partyCount\":%u,\"flag0x6A\":%u,\"flag0x6B\":%u,\"dexEnabled\":%u,\"dexSeen\":%u,\"dexCaught\":%u,\"bagBytes\":%u,\"attributes\":[",baseline ? "before" : "after",count,currentStarter,currentGotDex,currentEnabled,seen,caught,bagSize);
    for (u32 j=0;j<MON_ATTRIBUTES;j++) printf("%s%u",j ? "," : "",monAttributes[j]);
    fputs("],\"mons\":[",stdout);
    for (u32 i=0;i<count;i++) { printf("%s[",i ? "," : ""); for (u32 j=0;j<MON_ATTRIBUTES;j++) printf("%s%u",j ? "," : "",current[i][j]); putchar(']'); }
    puts("]}");
}
static void finish(Capture *capture,const char *status,BOOL passed) {
    QA_CHECK(HGNativeSetInput(capture->env,0,FALSE,0,0,FALSE));
    if (saveAddress) semanticEvidence(capture,FALSE);
    caseStatus=status; casePassed=passed; resetExpected=TRUE;
    w2c_env_HGNativeResetControl(capture->env,HG_RESET_REQUEST,0); QA_CHECK(FALSE);
}
static void nextStage(void) { ++stage; stageFrame=OS_GetVBlankCount(); }
static u32 actionRow(const HGNativePartyUIState *state,u32 textId) {
    for (u32 i=0;i<state->actionCount;i++) if (state->actions[i].textId==textId) return i;
    QA_CHECK(FALSE); return 0;
}
static BOOL openParty(Capture *capture) {
    u32 icons=w2c_hgengine_HGNativeFieldMenuIcons(capture->game);
    for (u32 i=0;i<7;i++) if (((icons>>(i*4))&15)==1) { pulse(capture,0,iconCenters[i][0],iconCenters[i][1],TRUE); return TRUE; }
    return FALSE;
}
static void PartyPresent(void *argument) {
    Capture *capture=argument; u32 frame=OS_GetVBlankCount();
    if (!stage) {
        present(capture);
        if (!w2c_hgengine_HGNativeFieldMap(capture->game) || !renderBoth(capture,FALSE)) return;
        capture->noInput=TRUE; QA_CHECK(HGNativeSetInput(capture->env,0,FALSE,0,0,FALSE));
        saveAddress=w2c_hgengine_SaveData_Get(capture->game); guest(capture,saveAddress,1);
        flagsAddress=w2c_hgengine_Save_VarsFlags_Get(capture->game,saveAddress); partyAddress=w2c_hgengine_SaveArray_Party_Get(capture->game,saveAddress);
        dexAddress=w2c_hgengine_Save_Pokedex_Get(capture->game,saveAddress); bagAddress=w2c_hgengine_Save_Bag_Get(capture->game,saveAddress);
        guest(capture,flagsAddress,1); guest(capture,partyAddress,1); guest(capture,dexAddress,1); guest(capture,bagAddress,1);
        semanticEvidence(capture,TRUE);
        printf("Party prerequisites: {\"flag0x6A\":%u,\"partyCount\":%u,\"flag0x6B\":%u,\"dexEnabled\":%u}\n",starter,partyCount,gotDex,dexEnabled);
        if (!starter || !partyCount) finish(capture,"scene_prerequisite_blocked",FALSE);
        QA_CHECK(partyCount<=HG_PARTY_UI_SLOTS); nextStage(); return;
    }
    QA_CHECK(HGNativeSoundRealtime(capture->env,TRUE));
    if (frame-stageFrame>3600) finish(capture,"scene_or_input_handshake_timeout",FALSE);
    if (pulseFrame) {
        if (!pulseReleased && frame-pulseFrame>=8) { QA_CHECK(HGNativeSetInput(capture->env,0,FALSE,0,0,FALSE)); pulseReleased=frame; }
        if (!pulseReleased || frame-pulseReleased<2) return;
        pulseFrame=0;
    }
    HGNativePartyUIState state=readParty(capture); u32 ignoredX,ignoredY;
    if (stage==1 || stage==10) { if (w2c_hgengine_HGNativeFieldMap(capture->game) && openParty(capture)) nextStage(); return; }
    if (stage==8) { if (!state.active) nextStage(); return; }
    if (stage==9 || stage==12) {
        if (state.active || !w2c_hgengine_HGNativeFieldMap(capture->game)) return;
        HGNativePartyUIState cleared={.revision=state.revision}; QA_CHECK(!memcmp(&state,&cleared,sizeof(state)));
        if (!renderBoth(capture,FALSE)) return; // Field-map ownership resumes before its fade-in finishes; the stage deadline still applies.
        if (!snapshot(capture,state)) finish(capture,"renderer_unsupported",FALSE);
        if (stage==12) { QA_CHECK(staleRejected>=3); finish(capture,"passed_original_party_menu",TRUE); }
        nextStage(); return;
    }
    if (!state.active || !state.ready) return;
    QA_CHECK(state.context==0 && state.panelLayout==0 && state.count==partyCount);
    if (stage==2 && state.phase==HG_PARTY_UI_ROSTER) {
        selectedSlot=HG_PARTY_UI_SLOTS;
        for (u32 i=0;i<HG_PARTY_UI_SLOTS;i++) if ((state.slotMask&(1u<<i)) && !state.mons[i].isEgg && !state.mons[i].heldItem) { selectedSlot=i; break; }
        if (selectedSlot==HG_PARTY_UI_SLOTS) finish(capture,"scene_no_nonmail_nonegg_mon",FALSE);
        if (!snapshot(capture,state)) finish(capture,"renderer_unsupported",FALSE);
        oldRoster=(PartyChoice){state.revision,state.phase,selectedSlot,HG_PARTY_UI_TOUCH_MON}; choose(capture,state,selectedSlot,HG_PARTY_UI_TOUCH_MON); nextStage();
    } else if (stage==3 && state.phase==HG_PARTY_UI_CONTEXT) {
        QA_CHECK(!HGNativePartyUIResolveTouch(&state,oldRoster.revision,oldRoster.phase,oldRoster.kind,oldRoster.row,&ignoredX,&ignoredY)); ++staleRejected;
        QA_CHECK(state.selected==selectedSlot && state.actionCount>=4);
        if (!snapshot(capture,state)) finish(capture,"renderer_unsupported",FALSE);
        choose(capture,state,actionRow(&state,3),HG_PARTY_UI_TOUCH_ACTION); nextStage();
    } else if (stage==4 && state.phase==HG_PARTY_UI_SUBCONTEXT) {
        QA_CHECK(state.selected==selectedSlot && state.actionCount>=2);
        if (!snapshot(capture,state)) finish(capture,"renderer_unsupported",FALSE);
        choose(capture,state,actionRow(&state,10),HG_PARTY_UI_TOUCH_ACTION); nextStage();
    } else if (stage==5 && state.phase==HG_PARTY_UI_ROSTER) {
        // Original subcontext Quit returns directly to the roster.
        if (!snapshot(capture,state)) finish(capture,"renderer_unsupported",FALSE);
        choose(capture,state,selectedSlot,HG_PARTY_UI_TOUCH_MON); nextStage();
    } else if (stage==6 && state.phase==HG_PARTY_UI_CONTEXT) {
        if (!snapshot(capture,state)) finish(capture,"renderer_unsupported",FALSE);
        choose(capture,state,actionRow(&state,10),HG_PARTY_UI_TOUCH_ACTION); nextStage();
    } else if (stage==7 && state.phase==HG_PARTY_UI_ROSTER) {
        if (!snapshot(capture,state)) finish(capture,"renderer_unsupported",FALSE);
        choose(capture,state,0,HG_PARTY_UI_TOUCH_CANCEL); nextStage();
    } else if (stage==11 && state.phase==HG_PARTY_UI_ROSTER) {
        QA_CHECK(state.revision>oldRoster.revision && !HGNativePartyUIResolveTouch(&state,oldRoster.revision,oldRoster.phase,oldRoster.kind,oldRoster.row,&ignoredX,&ignoredY)); ++staleRejected;
        QA_CHECK(!HGNativePartyUIResolveTouch(&state,state.revision,state.phase,HG_PARTY_UI_TOUCH_MON,HG_PARTY_UI_SLOTS,&ignoredX,&ignoredY)); ++staleRejected;
        if (!snapshot(capture,state)) finish(capture,"renderer_unsupported",FALSE);
        choose(capture,state,0,HG_PARTY_UI_TOUCH_CANCEL); nextStage();
    }
}

int main(int argc,char **argv) {
    if (argc!=19 || !HGNativeFSSetRoot(argv[1]) || !HGNativeBackupSetPath(argv[3])) return 2;
    for (u32 i=0;i<14;i++) { char *end; unsigned long value=strtoul(argv[5+i],&end,10); if (!*argv[5+i] || *end || value>=(i%2 ? 192u : 256u)) return 2; iconCenters[i/2][i%2]=(u32)value; }
    u8 header[0x160]; FILE *input=fopen(argv[2],"rb"); QA_CHECK(input);
    QA_CHECK(fread(header,1,sizeof(header),input)==sizeof(header) && fgetc(input)==EOF && !ferror(input)); QA_CHECK(!fclose(input));
    wasm_rt_init(); w2c_hgengine *game=calloc(1,sizeof(*game)); struct w2c_env *env=calloc(1,sizeof(*env));
    struct w2c_wasi__snapshot__preview1 *wasi=calloc(1,sizeof(*wasi)); Capture *capture=calloc(1,sizeof(*capture));
    QA_CHECK(game && env && wasi && capture); for (u32 i=0;i<3;i++) { wasi->fds[i]=dup(i); QA_CHECK(wasi->fds[i]>=0); } partyEnv=env;
    if (!setjmp(resetLanding)) {
        wasm_rt_trap_t trap=wasm_rt_impl_try(); QA_CHECK(!trap);
        wasm2c_hgengine_instantiate(game,env,wasi); env->memory=wasi->memory=w2c_hgengine_memory(game); QA_CHECK(HGNativeSoundRealtime(env,TRUE));
        env->irqTable=&game->w2c_T0; env->stackPointer=&game->w2c_0x5F_stack_pointer; env->rootStackHigh=*env->stackPointer; *env->stackPointer-=16;
        env->irqType0=wasm2c_hgengine_get_func_type(0,0); env->irqType1=wasm2c_hgengine_get_func_type(1,0,WASM_RT_I32);
        env->rtcType=wasm2c_hgengine_get_func_type(2,0,WASM_RT_I32,WASM_RT_I32);
        env->irqGuardType=wasm2c_hgengine_get_func_type(4,0,WASM_RT_I32,WASM_RT_I32,WASM_RT_I32,WASM_RT_I32);
        u32 work=w2c_hgengine_HGNativeSystemWorkAddress(game); guest(&(Capture){.env=env},work,0x360);
        memcpy(env->memory->data+work+0x200,header,sizeof(header)); w2c_hgengine_0x5Finitialize(game);
        u32 status=w2c_hgengine_HGNativeRegisterAddress(game,0x04000004,2); QA_CHECK(!(status&1)); guest(&(Capture){.env=env},status,2);
        QA_CHECK(HGNativeStartDisplayClock(env,(const vu16 *)(env->memory->data+status)));
        *capture=(Capture){.game=game,.env=env,.folder=argv[4],.resume=TRUE};
        QA_CHECK(HGNativeSetDisplayObserver(env,PartyPresent,capture)); w2c_hgengine_NitroMain(game); QA_CHECK(FALSE);
    }
    QA_CHECK(resetExpected && env->resetRequested); HGNativeFinishReset(env);
    QA_CHECK(env->resetFinished && !env->pendingDMA && !env->dmaAffineWrites && !env->pendingRTC && !env->pendingMIC && !env->pendingCARD && !env->pendingPOWER && !env->pendingINPUT && !env->irqTable && !env->threads && !env->rasterFunction);
    HGNativePartyUIState final=readParty(capture);
    if (casePassed) QA_CHECK(!final.active && !final.ready && !final.count && !final.actionCount && seenPhases==((1u<<1)|(1u<<2)|(1u<<15)));
    printf("Party menu evidence: {\"status\":\"%s\",\"flag0x6A\":%u,\"partyCount\":%u,\"flag0x6B\":%u,\"dexEnabled\":%u,\"selectedSlot\":%u,\"seenPhases\":%u,\"staleRejected\":%u,\"snapshotStages\":%u,\"activeAfterReturn\":%u,\"resetFinished\":true,\"pendingDMA\":0,\"partyItemsFlagsUnchanged\":true}\n",caseStatus,starter,partyCount,gotDex,dexEnabled,selectedSlot,seenPhases,staleRejected,snapshotStages,final.active);
    wasm2c_hgengine_free(game); for (u32 i=0;i<3;i++) close(wasi->fds[i]); free(originalBag); free(game); free(env); free(wasi); free(capture); partyEnv=NULL; wasm_rt_free();
    if (casePassed) puts("PASS: original Party roster, context and Item/Quit through real touch input, immutable save state, stale owner rejection, original frames and reset.");
    return casePassed ? 0 : 3;
}
