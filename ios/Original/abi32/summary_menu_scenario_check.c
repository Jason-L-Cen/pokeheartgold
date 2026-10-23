// SPDX-License-Identifier: GPL-3.0-or-later
#define main SummaryBootstrapMain
#define HGEngineTrap SummaryBootstrapTrap
#include "engine_check.c"
#undef HGEngineTrap
#undef main
#include "native_summary_ui.h"
#include "constants/pokemon.h"
#include <nitro/os/native_reset.h>
#include <setjmp.h>

#define QA_CHECK(value) do { if (!(value)) { fprintf(stderr,"Summary menu failure line %d: %s\n",__LINE__,#value); _Exit(1); } } while (0)
enum { OWNER_FIELD, OWNER_PARTY, OWNER_SUMMARY, MON_ATTRIBUTES=26 };
typedef struct Choice { u32 revision,phase,kind,index; } Choice;
static jmp_buf resetLanding;
static struct w2c_env *summaryEnv;
static BOOL resetExpected,casePassed;
static const char *caseStatus="not_started";
static u32 iconCenters[7][2],stage,stageFrame,snapshotStages,selectedSlot;
static u32 leaseActive,leaseReleased,leaseReads,leaseReleaseReads,leaseOwner,leaseRevision,leasePhase;
static u32 starter,partyCount,gotDex,dexEnabled,dexSeen,dexCaught,money,seenPages,seenPhases,viewedMoves,staleRejected,summaryLifetimes,partyLifetimes;
static u32 saveAddress,flagsAddress,partyAddress,dexAddress,bagAddress,profileAddress,bagSize;
static const u32 monAttributes[MON_ATTRIBUTES]={MON_DATA_PERSONALITY,MON_DATA_OT_ID,MON_DATA_SPECIES,MON_DATA_FORM,MON_DATA_IS_EGG,MON_DATA_HELD_ITEM,
    MON_DATA_HP,MON_DATA_MAX_HP,MON_DATA_LEVEL,MON_DATA_STATUS,MON_DATA_MOVE1,MON_DATA_MOVE2,MON_DATA_MOVE3,MON_DATA_MOVE4,
    MON_DATA_MOVE1_PP,MON_DATA_MOVE2_PP,MON_DATA_MOVE3_PP,MON_DATA_MOVE4_PP,MON_DATA_MOVE1_PP_UPS,MON_DATA_MOVE2_PP_UPS,MON_DATA_MOVE3_PP_UPS,MON_DATA_MOVE4_PP_UPS,
    MON_DATA_MOVE1_MAX_PP,MON_DATA_MOVE2_MAX_PP,MON_DATA_MOVE3_MAX_PP,MON_DATA_MOVE4_MAX_PP};
static u32 originalMonData[HG_PARTY_UI_SLOTS][MON_ATTRIBUTES];
static u8 *originalBag;
static Choice oldRoster,oldPartyAction,oldSummary;

void HGEngineTrap(wasm_rt_trap_t code) {
    if (code==WASM_RT_TRAP_UNREACHABLE && resetExpected && summaryEnv && summaryEnv->resetRequested) longjmp(resetLanding,1);
    SummaryBootstrapTrap(code);
}
static const void *guest(Capture *capture,u32 address,size_t size) {
    QA_CHECK(address && address<=capture->env->memory->size && size<=capture->env->memory->size-address);
    return capture->env->memory->data+address;
}
static HGNativePartyUIState readParty(Capture *capture) {
    static u32 lastRevision;
    u32 address=w2c_hgengine_HGNativePartyUIGetState(capture->game);
    HGNativePartyUIState state; memcpy(&state,guest(capture,address,sizeof(state)),sizeof(state));
    QA_CHECK(w2c_hgengine_HGNativePartyUIGetState(capture->game)==address && !memcmp(&state,guest(capture,address,sizeof(state)),sizeof(state)));
    QA_CHECK(state.count<=HG_PARTY_UI_SLOTS && state.actionCount<=HG_PARTY_UI_ACTIONS && !(state.slotMask&~63u));
    QA_CHECK(!(state.actionEnabledMask&~((1u<<state.actionCount)-1)) && state.revision>=lastRevision);
    lastRevision=state.revision; return state;
}
static HGNativeSummaryUIState readSummary(Capture *capture) {
    static u32 lastRevision;
    u32 address=w2c_hgengine_HGNativeSummaryUIGetState(capture->game);
    HGNativeSummaryUIState state; memcpy(&state,guest(capture,address,sizeof(state)),sizeof(state));
    QA_CHECK(w2c_hgengine_HGNativeSummaryUIGetState(capture->game)==address && !memcmp(&state,guest(capture,address,sizeof(state)),sizeof(state)));
    QA_CHECK(state.revision>=lastRevision && state.partyCount<=HG_PARTY_UI_SLOTS && !(state.moveMask&~15u));
    lastRevision=state.revision; return state;
}
static void pulse(Capture *capture,u32 owner,u32 revision,u32 phase,HGNativeSummaryUIInput input) {
    QA_CHECK(!leaseActive && input.touchX<256 && input.touchY<192 && !(input.keys&1));
    QA_CHECK(HGNativeSetInput(capture->env,input.keys,FALSE,input.touchX,input.touchY,input.touchPressed));
    leaseActive=TRUE; leaseReleased=FALSE; leaseReads=HGNativeInputPadReads(capture->env);
    leaseOwner=owner; leaseRevision=revision; leasePhase=phase;
    printf("Summary input evidence: {\"stage\":%u,\"event\":\"press\",\"owner\":%u,\"revision\":%u,\"phase\":%u,\"padReads\":%u,\"keys\":%u,\"touch\":[%u,%u,%u]}\n",stage,owner,revision,phase,leaseReads,input.keys,input.touchPressed,input.touchX,input.touchY);
}
static BOOL settleInput(Capture *capture,const HGNativePartyUIState *party,const HGNativeSummaryUIState *summary) {
    if (!leaseActive) return TRUE;
    u32 reads=HGNativeInputPadReads(capture->env);
    BOOL changed=leaseOwner==OWNER_PARTY ? !party->active || party->revision!=leaseRevision || party->phase!=leasePhase
        : leaseOwner==OWNER_SUMMARY ? !summary->active || summary->revision!=leaseRevision || summary->phase!=leasePhase : FALSE;
    if (!leaseReleased && (changed || reads-leaseReads>=4)) {
        QA_CHECK(HGNativeSetInput(capture->env,0,FALSE,0,0,FALSE)); leaseReleased=TRUE; leaseReleaseReads=reads;
        printf("Summary input evidence: {\"stage\":%u,\"event\":\"release\",\"owner\":%u,\"revision\":%u,\"phase\":%u,\"padReads\":%u,\"ownerChanged\":%u}\n",stage,leaseOwner,leaseRevision,leasePhase,reads,changed);
    }
    if (!leaseReleased || reads-leaseReleaseReads<2) return FALSE;
    leaseActive=FALSE; return TRUE;
}
static void chooseParty(Capture *capture,const HGNativePartyUIState *state,u32 kind,u32 index) {
    u32 x,y; HGNativePartyUIState current=readParty(capture);
    QA_CHECK(HGNativePartyUIResolveTouch(&current,state->revision,state->phase,kind,index,&x,&y));
    HGNativeSummaryUIInput input=kind==HG_PARTY_UI_TOUCH_CANCEL ? (HGNativeSummaryUIInput){.keys=2}
        : (HGNativeSummaryUIInput){.touchPressed=1,.touchX=x,.touchY=y};
    printf("Summary action evidence: {\"stage\":%u,\"owner\":\"party\",\"revision\":%u,\"phase\":%u,\"kind\":%u,\"index\":%u,\"textId\":%u}\n",stage,state->revision,state->phase,kind,index,kind==HG_PARTY_UI_TOUCH_ACTION ? state->actions[index].textId : 0);
    pulse(capture,OWNER_PARTY,state->revision,state->phase,input);
}
static void chooseSummary(Capture *capture,const HGNativeSummaryUIState *state,u32 kind,u32 index) {
    HGNativeSummaryUIInput input; HGNativeSummaryUIState current=readSummary(capture);
    QA_CHECK(HGNativeSummaryUIResolveInput(&current,state->revision,state->phase,kind,index,&input));
    printf("Summary action evidence: {\"stage\":%u,\"owner\":\"summary\",\"revision\":%u,\"phase\":%u,\"page\":%u,\"kind\":%u,\"index\":%u}\n",stage,state->revision,state->phase,state->page,kind,index);
    pulse(capture,OWNER_SUMMARY,state->revision,state->phase,input);
}
static void saveBytes(Capture *capture,const char *name,const void *bytes,size_t size) {
    char path[4096]; QA_CHECK(snprintf(path,sizeof(path),"%s/%s",capture->folder,name)<sizeof(path));
    FILE *file=fopen(path,"wb"); QA_CHECK(file && fwrite(bytes,1,size,file)==size && !fclose(file));
}
static void windowEvidence(Capture *capture,u32 phase,const char *role,u32 row,const HGNativePartyUIWindow *window) {
    if (!window->pixelBuffer) return;
    QA_CHECK(window->tileWidth && window->tileWidth<=32 && window->tileHeight && window->tileHeight<=6 && window->clearColor<16 && window->engine<2);
    size_t size=window->tileWidth*window->tileHeight*32;
    char name[128]; QA_CHECK(snprintf(name,sizeof(name),"stage-%02u-%s-%u.4bpp",snapshotStages,role,row)<sizeof(name));
    saveBytes(capture,name,guest(capture,window->pixelBuffer,size),size);
    printf("Summary window evidence: {\"stage\":%u,\"phase\":%u,\"role\":\"%s\",\"row\":%u,\"file\":\"%s\",\"pixelBuffer\":%u,\"tileWidth\":%u,\"tileHeight\":%u,\"clearColor\":%u,\"engine\":%u,\"palette\":[",snapshotStages,phase,role,row,name,window->pixelBuffer,window->tileWidth,window->tileHeight,window->clearColor,window->engine);
    for (u32 i=0;i<16;i++) printf("%s%u",i ? "," : "",window->palette[i]);
    puts("]}");
}
static BOOL renderBoth(Capture *capture,BOOL write,BOOL requireSub) {
    if (!capture->buffer) capture->buffer=w2c_hgengine_HGNativeFrameBuffer(capture->game);
    guest(capture,capture->buffer,256*192*4); HGNativeDrainDMA();
    u32 geometry[8]={0};
    if (w2c_hgengine_HGNativeGeometryFrame(capture->game,1,capture->buffer)) memcpy(geometry,guest(capture,capture->buffer,sizeof(geometry)),sizeof(geometry));
    for (u32 engine=0;engine<2;engine++) {
        if (!w2c_hgengine_HGNativeRender2D(capture->game,engine,capture->buffer,256*192*4)) return FALSE;
        const u8 *pixels=guest(capture,capture->buffer,256*192*4); BOOL varied=FALSE;
        for (u32 i=1;i<256*192;i++) if (memcmp(pixels,pixels+i*4,4)) { varied=TRUE; break; }
        if ((!engine || requireSub) && !varied) return FALSE;
        if (!write) continue;
        char path[4096]; QA_CHECK(snprintf(path,sizeof(path),"%s/stage-%02u-engine-%u.ppm",capture->folder,snapshotStages,engine)<sizeof(path));
        FILE *file=fopen(path,"wb"); QA_CHECK(file && fprintf(file,"P6\n256 192\n255\n")>0);
        for (u32 i=0;i<256*192;i++) QA_CHECK(fwrite(pixels+i*4,1,3,file)==3);
        QA_CHECK(!fclose(file));
        printf("Summary frame evidence: {\"stage\":%u,\"engine\":%u,\"frame\":%u,\"nonuniform\":%u,\"vertices\":%u,\"polygons\":%u,\"geometrySerial\":%u}\n",snapshotStages,engine,OS_GetVBlankCount(),varied,geometry[0],geometry[1],geometry[2]);
    }
    return TRUE;
}
static BOOL snapshot(Capture *capture,HGNativeSummaryUIState state) {
    if (!renderBoth(capture,TRUE,state.active)) return FALSE;
    char name[64]; QA_CHECK(snprintf(name,sizeof(name),"stage-%02u-state.bin",snapshotStages)<sizeof(name));
    saveBytes(capture,name,&state,sizeof(state));
    if (state.active) {
        QA_CHECK(state.ready && state.nickname.pixelBuffer && state.levelText.pixelBuffer && state.heldItemText.pixelBuffer && state.backText.pixelBuffer && state.pageTitle.pixelBuffer);
        const u32 *mon=originalMonData[selectedSlot];
        QA_CHECK(state.species==mon[2] && state.form==mon[3] && state.isEgg==mon[4] && state.heldItem==mon[5]
            && state.hp==mon[6] && state.maxHp==mon[7] && state.level==mon[8]);
        for (u32 i=0;i<HG_SUMMARY_UI_MOVES;i++) {
            const HGNativeSummaryUIMove *move=&state.moves[i];
            QA_CHECK(move->moveId==mon[10+i] && move->pp==mon[14+i] && move->maxPP==mon[22+i]);
            if (state.moveMask&(1u<<i)) {
                QA_CHECK(move->enabled && move->moveId && move->text.pixelBuffer);
                windowEvidence(capture,state.phase,"move",i,&move->text);
            }
        }
        seenPages|=1u<<state.page; seenPhases|=1u<<state.phase;
        if (state.phase==HG_SUMMARY_UI_MOVE_SELECT) viewedMoves|=1u<<state.moveSlot;
    }
    windowEvidence(capture,state.phase,"nickname",0,&state.nickname); windowEvidence(capture,state.phase,"level",0,&state.levelText);
    windowEvidence(capture,state.phase,"held-item",0,&state.heldItemText); windowEvidence(capture,state.phase,"back",0,&state.backText); windowEvidence(capture,state.phase,"page-title",0,&state.pageTitle);
    HGNativeSummaryUIState after=readSummary(capture); QA_CHECK(!memcmp(&state,&after,sizeof(state)));
    printf("Summary snapshot evidence: {\"stage\":%u,\"active\":%u,\"ready\":%u,\"revision\":%u,\"phase\":%u,\"page\":%u,\"pageMask\":%u,\"partySlot\":%u,\"partyCount\":%u,\"moveSlot\":%u,\"moveMask\":%u,\"statusIcon\":%u,\"pokerus\":%u,\"ribbonCount\":%u}\n",snapshotStages,state.active,state.ready,state.revision,state.phase,state.page,state.pageMask,state.partySlot,state.partyCount,state.moveSlot,state.moveMask,state.statusIcon,state.pokerus,state.ribbonCount);
    ++snapshotStages; return TRUE;
}
static void partyEvidence(Capture *capture,const HGNativePartyUIState *state) {
    QA_CHECK(state->active && state->ready && !state->context && !state->panelLayout && state->count==partyCount);
    char name[64]; QA_CHECK(snprintf(name,sizeof(name),"party-stage-%02u-state.bin",stage)<sizeof(name));
    saveBytes(capture,name,state,sizeof(*state));
    printf("Summary action evidence: {\"stage\":%u,\"owner\":\"party_lifetime\",\"lifetime\":%u,\"revision\":%u,\"phase\":%u,\"selected\":%u,\"actionCount\":%u}\n",stage,partyLifetimes,state->revision,state->phase,state->selected,state->actionCount);
}
static void semanticEvidence(Capture *capture,BOOL baseline) {
    u32 count=w2c_hgengine_Party_GetCount(capture->game,partyAddress),current[HG_PARTY_UI_SLOTS][MON_ATTRIBUTES]={{0}};
    QA_CHECK(count<=HG_PARTY_UI_SLOTS);
    for (u32 i=0;i<count;i++) {
        u32 mon=w2c_hgengine_Party_GetMonByIndex(capture->game,partyAddress,i); guest(capture,mon,1);
        for (u32 j=0;j<MON_ATTRIBUTES;j++) current[i][j]=w2c_hgengine_GetMonData(capture->game,mon,monAttributes[j],0);
    }
    u32 currentStarter=w2c_hgengine_CheckGotStarter(capture->game,flagsAddress),currentGotDex=w2c_hgengine_CheckGotPokedex(capture->game,flagsAddress);
    u32 currentEnabled=w2c_hgengine_Pokedex_IsEnabled(capture->game,dexAddress),currentMoney=w2c_hgengine_PlayerProfile_GetMoney(capture->game,profileAddress);
    u32 seen=w2c_hgengine_Pokedex_CountNationalDexSeen(capture->game,dexAddress),caught=w2c_hgengine_Pokedex_CountNationalDexOwned(capture->game,dexAddress);
    if (baseline) {
        starter=currentStarter; partyCount=count; gotDex=currentGotDex; dexEnabled=currentEnabled; dexSeen=seen; dexCaught=caught; money=currentMoney;
        memcpy(originalMonData,current,sizeof(current)); bagSize=w2c_hgengine_Save_Bag_sizeof(capture->game); QA_CHECK(bagSize);
        originalBag=malloc(bagSize); QA_CHECK(originalBag); memcpy(originalBag,guest(capture,bagAddress,bagSize),bagSize);
    } else {
        QA_CHECK(count==partyCount && !memcmp(originalMonData,current,sizeof(current)));
        QA_CHECK(currentStarter==starter && currentGotDex==gotDex && currentEnabled==dexEnabled && seen==dexSeen && caught==dexCaught && currentMoney==money);
        QA_CHECK(bagSize==w2c_hgengine_Save_Bag_sizeof(capture->game) && !memcmp(originalBag,guest(capture,bagAddress,bagSize),bagSize));
    }
    saveBytes(capture,baseline ? "bag-before.bin" : "bag-after.bin",guest(capture,bagAddress,bagSize),bagSize);
    printf("Summary semantic evidence: {\"step\":\"%s\",\"partyCount\":%u,\"flag0x6A\":%u,\"flag0x6B\":%u,\"dexEnabled\":%u,\"dexSeen\":%u,\"dexCaught\":%u,\"money\":%u,\"bagBytes\":%u,\"attributes\":[",baseline ? "before" : "after",count,currentStarter,currentGotDex,currentEnabled,seen,caught,currentMoney,bagSize);
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
static u32 summaryAction(const HGNativePartyUIState *state) {
    for (u32 i=0;i<state->actionCount;i++) if (state->actions[i].textId==2) return i;
    QA_CHECK(FALSE); return 0;
}
static void SummaryPresent(void *argument) {
    Capture *capture=argument; u32 frame=OS_GetVBlankCount();
    if (!stage) {
        present(capture);
        if (!w2c_hgengine_HGNativeFieldMap(capture->game) || !renderBoth(capture,FALSE,FALSE)) return;
        capture->noInput=TRUE; QA_CHECK(HGNativeSetInput(capture->env,0,FALSE,0,0,FALSE));
        saveAddress=w2c_hgengine_SaveData_Get(capture->game); guest(capture,saveAddress,1);
        flagsAddress=w2c_hgengine_Save_VarsFlags_Get(capture->game,saveAddress); partyAddress=w2c_hgengine_SaveArray_Party_Get(capture->game,saveAddress);
        dexAddress=w2c_hgengine_Save_Pokedex_Get(capture->game,saveAddress); bagAddress=w2c_hgengine_Save_Bag_Get(capture->game,saveAddress);
        profileAddress=w2c_hgengine_Save_PlayerData_GetProfile(capture->game,saveAddress);
        guest(capture,flagsAddress,1); guest(capture,partyAddress,1); guest(capture,dexAddress,1); guest(capture,bagAddress,1); guest(capture,profileAddress,1);
        semanticEvidence(capture,TRUE);
        printf("Summary prerequisites: {\"flag0x6A\":%u,\"partyCount\":%u,\"flag0x6B\":%u,\"dexEnabled\":%u}\n",starter,partyCount,gotDex,dexEnabled);
        if (!starter || !partyCount) finish(capture,"scene_prerequisite_blocked",FALSE);
        nextStage(); return;
    }
    QA_CHECK(HGNativeSoundRealtime(capture->env,TRUE));
    if (frame-stageFrame>3600) finish(capture,"scene_or_input_handshake_timeout",FALSE);
    HGNativePartyUIState party=readParty(capture); HGNativeSummaryUIState state=readSummary(capture);
    if (!settleInput(capture,&party,&state)) return;
    u32 ignoredX,ignoredY; HGNativeSummaryUIInput ignored;
    if (stage==1) {
        if (!w2c_hgengine_HGNativeFieldMap(capture->game)) return;
        u32 icons=w2c_hgengine_HGNativeFieldMenuIcons(capture->game);
        for (u32 i=0;i<7;i++) if (((icons>>(i*4))&15)==1) {
            pulse(capture,OWNER_FIELD,0,0,(HGNativeSummaryUIInput){.touchPressed=1,.touchX=iconCenters[i][0],.touchY=iconCenters[i][1]}); nextStage(); return;
        }
        return;
    }
    if ((stage==2 || stage==12 || stage==16) && party.active && party.ready && party.phase==HG_PARTY_UI_ROSTER) {
        ++partyLifetimes; partyEvidence(capture,&party);
        if (stage==2) {
            selectedSlot=HG_PARTY_UI_SLOTS;
            for (u32 i=0;i<party.count;i++) if ((party.slotMask&(1u<<i)) && !party.mons[i].isEgg) { selectedSlot=i; break; }
            if (selectedSlot==HG_PARTY_UI_SLOTS) finish(capture,"scene_no_nonegg_mon",FALSE);
            oldRoster=(Choice){party.revision,party.phase,HG_PARTY_UI_TOUCH_MON,selectedSlot};
        } else {
            QA_CHECK(party.revision>oldRoster.revision && !HGNativePartyUIResolveTouch(&party,oldRoster.revision,oldRoster.phase,oldRoster.kind,oldRoster.index,&ignoredX,&ignoredY)); ++staleRejected;
            HGNativeSummaryUIState cleared={.revision=state.revision}; QA_CHECK(!memcmp(&state,&cleared,sizeof(state)));
            QA_CHECK(!HGNativeSummaryUIResolveInput(&state,oldSummary.revision,oldSummary.phase,oldSummary.kind,oldSummary.index,&ignored)); ++staleRejected;
        }
        if (stage==16) chooseParty(capture,&party,HG_PARTY_UI_TOUCH_CANCEL,0);
        else chooseParty(capture,&party,HG_PARTY_UI_TOUCH_MON,selectedSlot);
        nextStage(); return;
    }
    if ((stage==3 || stage==13) && party.active && party.ready && party.phase==HG_PARTY_UI_CONTEXT) {
        partyEvidence(capture,&party); u32 row=summaryAction(&party); QA_CHECK(party.selected==selectedSlot);
        if (stage==3) oldPartyAction=(Choice){party.revision,party.phase,HG_PARTY_UI_TOUCH_ACTION,row};
        else { QA_CHECK(party.revision>oldPartyAction.revision && !HGNativePartyUIResolveTouch(&party,oldPartyAction.revision,oldPartyAction.phase,oldPartyAction.kind,oldPartyAction.index,&ignoredX,&ignoredY)); ++staleRejected; }
        chooseParty(capture,&party,HG_PARTY_UI_TOUCH_ACTION,row); nextStage(); return;
    }
    if (stage==17) {
        if (party.active || state.active || !w2c_hgengine_HGNativeFieldMap(capture->game) || !renderBoth(capture,FALSE,FALSE)) return;
        HGNativeSummaryUIState cleared={.revision=state.revision}; QA_CHECK(!memcmp(&state,&cleared,sizeof(state)));
        if (!snapshot(capture,state)) finish(capture,"renderer_unsupported",FALSE);
        QA_CHECK(summaryLifetimes==2 && partyLifetimes==3 && seenPages==7 && viewedMoves==3 && staleRejected>=6);
        finish(capture,"passed_original_summary_menu",TRUE); return;
    }
    if (stage==11 || stage==15) {
        // Wait for the original Summary exit and the new Party allocation; no injected application launch.
        if (!state.active) nextStage();
        return;
    }
    if (!state.active || !state.ready) return;
    QA_CHECK(!party.active && !state.mode && state.monDataType==1 && !state.moveToLearn && state.partyCount==partyCount && state.partySlot==selectedSlot && !state.isEgg);
    QA_CHECK(!state.previousMonEnabled && !state.nextMonEnabled);
    if ((stage==4 || stage==14) && state.phase==HG_SUMMARY_UI_INPUT && state.page==0) {
        ++summaryLifetimes;
        if (!snapshot(capture,state)) finish(capture,"renderer_unsupported",FALSE);
        if (stage==4) {
            oldSummary=(Choice){state.revision,state.phase,HG_SUMMARY_UI_INPUT_NEXT_PAGE,0};
            chooseSummary(capture,&state,HG_SUMMARY_UI_INPUT_NEXT_PAGE,0);
        } else {
            QA_CHECK(state.revision>oldSummary.revision && !HGNativeSummaryUIResolveInput(&state,oldSummary.revision,oldSummary.phase,oldSummary.kind,oldSummary.index,&ignored)); ++staleRejected;
            QA_CHECK(!HGNativeSummaryUIResolveInput(&state,state.revision,state.phase,HG_SUMMARY_UI_INPUT_MOVE,HG_SUMMARY_UI_MOVES,&ignored)); ++staleRejected;
            chooseSummary(capture,&state,HG_SUMMARY_UI_INPUT_BACK,0);
        }
        nextStage(); return;
    }
    if ((stage==5 || stage==7 || stage==10) && state.phase==HG_SUMMARY_UI_INPUT && state.page==1) {
        if (!snapshot(capture,state)) finish(capture,"renderer_unsupported",FALSE);
        chooseSummary(capture,&state,stage==5 ? HG_SUMMARY_UI_INPUT_NEXT_PAGE : stage==7 ? HG_SUMMARY_UI_INPUT_MOVE : HG_SUMMARY_UI_INPUT_BACK,0);
        nextStage(); return;
    }
    if (stage==6 && state.phase==HG_SUMMARY_UI_INPUT && state.page==2) {
        if (!snapshot(capture,state)) finish(capture,"renderer_unsupported",FALSE);
        chooseSummary(capture,&state,HG_SUMMARY_UI_INPUT_PREVIOUS_PAGE,0); nextStage(); return;
    }
    if ((stage==8 || stage==9) && state.phase==HG_SUMMARY_UI_MOVE_SELECT && state.page==1 && state.moveSlot==(stage==8 ? 0u : 1u)) {
        QA_CHECK((state.moveMask&3)==3 && !HGNativeSummaryUIResolveInput(&state,state.revision,state.phase,HG_SUMMARY_UI_INPUT_RIBBON_DETAILS,0,&ignored));
        if (!snapshot(capture,state)) finish(capture,"renderer_unsupported",FALSE);
        chooseSummary(capture,&state,stage==8 ? HG_SUMMARY_UI_INPUT_MOVE : HG_SUMMARY_UI_INPUT_BACK,stage==8 ? 1 : 0); nextStage(); return;
    }
}

int main(int argc,char **argv) {
    setvbuf(stdout,NULL,_IONBF,0);
    if (argc!=19 || !HGNativeFSSetRoot(argv[1]) || !HGNativeBackupSetPath(argv[3])) return 2;
    for (u32 i=0;i<14;i++) { char *end; unsigned long value=strtoul(argv[5+i],&end,10); if (!*argv[5+i] || *end || value>=(i%2 ? 192u : 256u)) return 2; iconCenters[i/2][i%2]=(u32)value; }
    u8 header[0x160]; FILE *input=fopen(argv[2],"rb"); QA_CHECK(input);
    QA_CHECK(fread(header,1,sizeof(header),input)==sizeof(header) && fgetc(input)==EOF && !ferror(input)); QA_CHECK(!fclose(input));
    wasm_rt_init(); w2c_hgengine *game=calloc(1,sizeof(*game)); struct w2c_env *env=calloc(1,sizeof(*env));
    struct w2c_wasi__snapshot__preview1 *wasi=calloc(1,sizeof(*wasi)); Capture *capture=calloc(1,sizeof(*capture));
    QA_CHECK(game && env && wasi && capture); for (u32 i=0;i<3;i++) { wasi->fds[i]=dup(i); QA_CHECK(wasi->fds[i]>=0); } summaryEnv=env;
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
        QA_CHECK(HGNativeSetDisplayObserver(env,SummaryPresent,capture)); w2c_hgengine_NitroMain(game); QA_CHECK(FALSE);
    }
    QA_CHECK(resetExpected && env->resetRequested); HGNativeFinishReset(env);
    QA_CHECK(env->resetFinished && !env->pendingDMA && !env->dmaAffineWrites && !env->pendingRTC && !env->pendingMIC && !env->pendingCARD && !env->pendingPOWER && !env->pendingINPUT && !env->irqTable && !env->threads && !env->rasterFunction);
    HGNativeSummaryUIState final=readSummary(capture); HGNativePartyUIState finalParty=readParty(capture);
    if (casePassed) QA_CHECK(!final.active && !final.ready && !finalParty.active && seenPhases==((1u<<2)|(1u<<5)));
    printf("Summary menu evidence: {\"status\":\"%s\",\"flag0x6A\":%u,\"partyCount\":%u,\"flag0x6B\":%u,\"dexEnabled\":%u,\"selectedSlot\":%u,\"seenPages\":%u,\"seenPhases\":%u,\"viewedMoves\":%u,\"summaryLifetimes\":%u,\"partyLifetimes\":%u,\"staleRejected\":%u,\"snapshotStages\":%u,\"activeAfterReturn\":%u,\"resetFinished\":true,\"pendingDMA\":0,\"partyItemsMoneyFlagsUnchanged\":true}\n",caseStatus,starter,partyCount,gotDex,dexEnabled,selectedSlot,seenPages,seenPhases,viewedMoves,summaryLifetimes,partyLifetimes,staleRejected,snapshotStages,final.active);
    wasm2c_hgengine_free(game); for (u32 i=0;i<3;i++) close(wasi->fds[i]); free(originalBag); free(game); free(env); free(wasi); free(capture); summaryEnv=NULL; wasm_rt_free();
    if (casePassed) puts("PASS: original Summary pages and move details, original Party returns, unchanged party/items/money/flags, stale owners, original frames and reset.");
    return casePassed ? 0 : 3;
}
