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
#ifdef HG_EVOLUTION_CHECK
#define HG_SUMMARY_RARE_CANDY_CHECK
#include "native_evolution_ui.h"
enum { OWNER_EVOLUTION=4 };
_Static_assert(sizeof(HGNativeEvolutionUIState)==196,"Evolution native scenario ABI");
static HGNativeEvolutionUIState readEvolution(Capture *capture);
static void evolutionLeaseEvent(const char *event,u32 reads,BOOL changed);
#endif
#ifdef HG_SUMMARY_RARE_CANDY_CHECK
#define HG_SUMMARY_TM_LEARN_CHECK
#endif
#ifdef HG_SUMMARY_TM_LEARN_CHECK
#include "native_bag_ui.h"
#include "constants/items.h"
#include "constants/balls.h"
#include "constants/heap.h"
#include "player_data.h"
#include "../../tests/summary_learn_guest.inc"
_Static_assert(sizeof(HGSummaryLearnQAState) == 232, "Private Summary learning observer ABI");
#endif

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
#ifdef HG_SUMMARY_TM_LEARN_CHECK
enum { OWNER_BAG = 3 };
static u32 tmLeaseQARevision;
static HGNativeBagUIState readTmBag(Capture *capture);
static void tmPeripheralLeaseEvent(Capture *capture,const char *event,u32 reads,BOOL changed);
static const void *guest(Capture *capture,u32 address,size_t size);
static HGSummaryLearnQAState readLearn(Capture *capture) {
    static u32 lastRevision;
    u32 address=w2c_hgengine_HGNativeSummaryLearnQAGetState(capture->game);
    HGSummaryLearnQAState state; memcpy(&state,guest(capture,address,sizeof(state)),sizeof(state));
    QA_CHECK(state.version==HG_SUMMARY_LEARN_QA_VERSION && state.byteSize==sizeof(state) && state.revision>=lastRevision);
    lastRevision=state.revision; return state;
}
#endif

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
    QA_CHECK(state.revision>=lastRevision && state.partyCount<=HG_PARTY_UI_SLOTS);
#ifdef HG_SUMMARY_TM_LEARN_CHECK
    QA_CHECK(!(state.moveMask&~(HGNativeSummaryUIIsLearning(&state) && state.phase==HG_SUMMARY_UI_FORGET_MOVE ? 31u : 15u)));
#else
    QA_CHECK(!(state.moveMask&~15u));
#endif
    lastRevision=state.revision; return state;
}
static void pulse(Capture *capture,u32 owner,u32 revision,u32 phase,HGNativeSummaryUIInput input) {
    QA_CHECK(!leaseActive && input.touchX<256 && input.touchY<192);
#ifdef HG_EVOLUTION_CHECK
    QA_CHECK(!(input.keys&1) || owner==OWNER_SUMMARY || owner==OWNER_EVOLUTION);
#elif defined(HG_SUMMARY_TM_LEARN_CHECK)
    QA_CHECK(!(input.keys&1) || owner==OWNER_SUMMARY);
#else
    QA_CHECK(!(input.keys&1));
#endif
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
#ifdef HG_SUMMARY_TM_LEARN_CHECK
    if (leaseOwner==OWNER_BAG) {
        HGNativeBagUIState bag=readTmBag(capture);
        changed=!bag.active || !bag.ready || bag.revision!=leaseRevision || bag.phase!=leasePhase;
    }
    if (leaseOwner==OWNER_PARTY && !party->ready) changed=TRUE;
    if (tmLeaseQARevision && (leaseOwner==OWNER_BAG || leaseOwner==OWNER_PARTY))
        changed=changed || readLearn(capture).revision!=tmLeaseQARevision;
#endif
#ifdef HG_EVOLUTION_CHECK
    if (leaseOwner==OWNER_EVOLUTION) {
        HGNativeEvolutionUIState evolution=readEvolution(capture);
        changed=!evolution.active || evolution.summaryActive || evolution.revision!=leaseRevision || evolution.phase!=leasePhase;
    }
#endif
    if (!leaseReleased && (changed || reads-leaseReads>=4)) {
        QA_CHECK(HGNativeSetInput(capture->env,0,FALSE,0,0,FALSE)); leaseReleased=TRUE; leaseReleaseReads=reads;
        printf("Summary input evidence: {\"stage\":%u,\"event\":\"release\",\"owner\":%u,\"revision\":%u,\"phase\":%u,\"padReads\":%u,\"ownerChanged\":%u}\n",stage,leaseOwner,leaseRevision,leasePhase,reads,changed);
#ifdef HG_SUMMARY_TM_LEARN_CHECK
        if (tmLeaseQARevision) tmPeripheralLeaseEvent(capture,"released",reads,changed);
#ifdef HG_EVOLUTION_CHECK
        if (leaseOwner==OWNER_EVOLUTION) evolutionLeaseEvent("released",reads,changed);
#endif
#endif
    }
    if (!leaseReleased || reads-leaseReleaseReads<2) return FALSE;
#ifdef HG_SUMMARY_TM_LEARN_CHECK
    if (tmLeaseQARevision) {
        tmPeripheralLeaseEvent(capture,"settled",reads,FALSE);
        tmLeaseQARevision=0;
    }
#endif
#ifdef HG_EVOLUTION_CHECK
    if (leaseOwner==OWNER_EVOLUTION) evolutionLeaseEvent("settled",reads,FALSE);
#endif
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
    BOOL sizeValid=window->tileWidth && window->tileWidth<=32 && window->tileHeight && window->tileHeight<=6;
#ifdef HG_SUMMARY_TM_LEARN_CHECK
    if (!strcmp(role,"original-hm-warning")) sizeValid=window->tileWidth==15 && window->tileHeight==10 && window->engine==1 && !window->clearColor;
#endif
    QA_CHECK(sizeValid && window->clearColor<16 && window->engine<2);
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
#ifdef HG_SUMMARY_TM_LEARN_CHECK
enum { TM_OPEN_BAG=1, TM_POCKET, TM_ITEM, TM_USE, TM_BOOT_ACK, TM_BAG_YES, TM_TARGET, TM_FORGET_YES,
    TM_FIRST_SUMMARY, TM_FIFTH_CONFIRM, TM_STOP_NO, TM_RETRY_YES, TM_SECOND_SUMMARY, TM_HM_RESULT,
    TM_OLD_MOVE, TM_CONFIRM_BACK, TM_OLD_AGAIN, TM_CONFIRM_REPLACE, TM_RETURN_BAG, TM_CLOSE_BAG, TM_FIELD };
static u32 tmFixturePrepared,tmFifthCancelled,tmContinuedTrying,tmHmRejected,tmConfirmationBack,tmReplacement;
static u32 tmFlagsSize,tmSlotOffset,tmInitialQuantity,tmFinalQuantity,tmFinalMove,tmFinalPpUps,tmFinalPp,tmFinalMaxPp;
static u32 tmFirstGeneration,tmHmCallsBefore;
static u32 tmInitialFriendship,tmInitialMood,tmFinalFriendship,tmFinalMood,tmMapSection;
static u8 *tmOriginalFlags;
static Choice tmFirstChoice,tmOldChoice;

typedef struct TMRequest {
    Choice choice;
    HGSummaryLearnQAState owner;
    u32 producer,sequence,stage,advance;
#ifdef HG_SUMMARY_RARE_CANDY_CHECK
    u32 role;
#endif
} TMRequest;
static TMRequest tmPending,tmLeaseRequest;
static u32 tmSequence,tmTypedAccepted,tmTypedRejected,tmTypedReleased,tmTypedSettled,tmPartyReturnAcks;
static u32 tmExitFadeSeen,tmExitFadeRejected;
static Choice tmBagYesChoice;

static HGNativeBagUIState readTmBag(Capture *capture) {
    static u32 lastRevision;
    u32 address=w2c_hgengine_HGNativeBagUIGetState(capture->game);
    HGNativeBagUIState state; memcpy(&state,guest(capture,address,sizeof(state)),sizeof(state));
    QA_CHECK(w2c_hgengine_HGNativeBagUIGetState(capture->game)==address && !memcmp(&state,guest(capture,address,sizeof(state)),sizeof(state)));
    QA_CHECK(state.visibleCount<=HG_BAG_UI_ITEMS && state.actionCount<=HG_BAG_UI_ACTIONS
        && !(state.actionMask&~((1u<<state.actionCount)-1)) && state.revision>=lastRevision);
    lastRevision=state.revision; return state;
}
static BOOL tmPromptReady(const HGSummaryLearnQAState *qa,u32 row) {
    return row<2 && qa->yesNoReady && qa->yesNoActive==1 && qa->yesNoResult==3 && !qa->yesNoTimer
        && qa->yesNoX==25 && qa->yesNoWidth==6 && qa->yesNoHeight==4
        && qa->yesNoY==(qa->owner==HG_SUMMARY_LEARN_BAG ? 6u : 10u)
        && qa->yesHitX[row]==224 && qa->yesHitY[row]==qa->yesNoY*8+16+row*32;
}
static BOOL tmPrinterReady(const HGSummaryLearnQAState *qa) {
    return qa->printerId<8 && qa->printerActive && qa->printerMatches && qa->printerState==2 && qa->printerWaiting;
}
#ifdef HG_SUMMARY_RARE_CANDY_CHECK
static void candyStatsEvidence(Capture *capture,const HGSummaryLearnQAState *qa,const HGNativePartyUIState *party);
static BOOL candyPeripheralReady(const HGSummaryLearnQAState *qa,const HGNativeBagUIState *bag,
    const HGNativePartyUIState *party,u32 producer,u32 kind,u32 index) {
    if (qa->execState!=2 || !qa->graphicsReady || !qa->fadeFinished) return FALSE;
    if (producer==OWNER_BAG) return qa->owner==HG_SUMMARY_LEARN_BAG && bag->active && bag->ready
        && qa->phase==bag->phase && (HGNativeBagUIIsList(bag) || bag->phase==HG_BAG_UI_ITEM_MENU);
    BOOL owner=(qa->context==5 && qa->itemId==ITEM_RARE_CANDY)
        || (qa->context==8 && qa->itemId==ITEM_NONE && qa->moveId==MOVE_RAZOR_LEAF);
    if (producer!=OWNER_PARTY || qa->owner!=HG_SUMMARY_LEARN_PARTY || !owner || qa->partyBusy
        || !party->active || !party->ready || qa->phase!=party->phase || qa->context!=party->context
        || !HGNativePartyUIIsRareCandy(party)) return FALSE;
    if (party->phase==HG_PARTY_UI_ITEM_TARGET) return qa->context==5 && !qa->partySlot
        && ((kind==HG_PARTY_UI_TOUCH_MON && !index) || (kind==HG_PARTY_UI_TOUCH_CANCEL && !index));
    if (kind!=HG_PARTY_UI_TOUCH_ACTION || index>=party->actionCount) return FALSE;
    if (party->phase==HG_PARTY_UI_TM_YES_NO) return qa->moveId==MOVE_RAZOR_LEAF && tmPromptReady(qa,index);
    if (index || party->actionCount!=1) return FALSE;
    u32 role=party->actions[0].textId;
    if (party->phase==HG_PARTY_UI_ITEM_USE && qa->context==5
        && ((qa->taskState==1 && role==HG_PARTY_UI_TEXT_CANDY_STATS_DIFF)
            || (qa->taskState==2 && role==HG_PARTY_UI_TEXT_CANDY_STATS_TOTAL))) return TRUE;
    return role==HG_PARTY_UI_TEXT_CANDY_ACK && tmPrinterReady(qa)
        && (party->phase==HG_PARTY_UI_TM_MESSAGE || (party->phase==HG_PARTY_UI_ITEM_USE && (qa->taskState==0 || qa->taskState==4)));
}
#endif
static BOOL tmPeripheralReady(const HGSummaryLearnQAState *qa,const HGNativeBagUIState *bag,
    const HGNativePartyUIState *party,u32 producer,u32 kind,u32 index) {
#ifdef HG_SUMMARY_RARE_CANDY_CHECK
    return candyPeripheralReady(qa,bag,party,producer,kind,index);
#else
    if (qa->execState!=2 || !qa->graphicsReady || !qa->fadeFinished) return FALSE;
    if (producer==OWNER_BAG) {
        if (qa->owner!=HG_SUMMARY_LEARN_BAG || !bag->active || !bag->ready || qa->phase!=bag->phase) return FALSE;
        if (HGNativeBagUIIsList(bag) || bag->phase==HG_BAG_UI_ITEM_MENU) return TRUE;
        if (!HGNativeBagUIIsTMHM(bag) || bag->phase!=HG_BAG_UI_TM_MESSAGE || !qa->tmTaskMatches
            || qa->itemId!=ITEM_TM17 || bag->selectedItemId!=ITEM_TM17 || kind!=HG_BAG_UI_TOUCH_ACTION) return FALSE;
        if (qa->taskState==3) return tmPromptReady(qa,index);
        return !index && ((qa->taskState==1 && qa->printerId<8 && !qa->printerActive)
            || (qa->taskState==2 && tmPrinterReady(qa)));
    }
    if (producer!=OWNER_PARTY || qa->owner!=HG_SUMMARY_LEARN_PARTY || !party->active || !party->ready
        || qa->phase!=party->phase || qa->context!=party->context || !HGNativePartyUIIsTMHM(party)
        || (qa->context!=6 && qa->context!=7) || qa->partyBusy || qa->itemId!=ITEM_TM17 || qa->moveId!=MOVE_PROTECT) return FALSE;
    if (party->phase==HG_PARTY_UI_TM_TARGET)
        return qa->context==6 && !qa->partyBusy && !qa->partySlot
            && ((kind==HG_PARTY_UI_TOUCH_MON && !index) || (kind==HG_PARTY_UI_TOUCH_CANCEL && !index));
    if (kind!=HG_PARTY_UI_TOUCH_ACTION) return FALSE;
    if (party->phase==HG_PARTY_UI_TM_MESSAGE) return !index && tmPrinterReady(qa);
    return party->phase==HG_PARTY_UI_TM_YES_NO && tmPromptReady(qa,index);
#endif
}
static void tmTypedEvidence(Capture *capture,const TMRequest *request,const char *event,u32 reads,BOOL changed,u32 x,u32 y) {
    const HGSummaryLearnQAState *qa=&request->owner;
    printf(
#ifdef HG_SUMMARY_RARE_CANDY_CHECK
        "Summary Candy typed evidence: "
#else
        "Summary TM typed evidence: "
#endif
        "{\"event\":\"%s\",\"sequence\":%u,\"stage\":%u,\"advance\":%u,\"producer\":%u,\"revision\":%u,\"phase\":%u,\"kind\":%u,\"index\":%u,\"owner\":%u,\"generation\":%u,\"qaRevision\":%u,\"manager\":%u,\"data\":%u,\"execState\":%u,\"context\":%u,\"itemId\":%u,\"moveId\":%u,\"partySlot\":%u,\"selectedMoveSlot\":%u,\"taskState\":%u,\"taskMatches\":%u,\"printerId\":%u,\"printerActive\":%u,\"printerMatches\":%u,\"printerState\":%u,\"printerWaiting\":%u,\"yesNoActive\":%u,\"yesNoReady\":%u,\"yesNoResult\":%u,\"yesNoTimer\":%u,\"yesNoX\":%u,\"yesNoY\":%u,\"yesNoWidth\":%u,\"yesNoHeight\":%u,\"yesHitX\":[%u,%u],\"yesHitY\":[%u,%u],\"partyBusy\":%u,\"graphicsReady\":%u,\"fadeFinished\":%u,\"padReads\":%u,\"invalidated\":%u,\"keys\":0,\"touch\":[%u,%u,%u]"
#ifdef HG_SUMMARY_RARE_CANDY_CHECK
        ",\"role\":%u"
#endif
        "}\n",
        event,request->sequence,request->stage,request->advance,request->producer,request->choice.revision,request->choice.phase,request->choice.kind,request->choice.index,
        qa->owner,qa->generation,qa->revision,qa->manager,qa->data,qa->execState,qa->context,qa->itemId,qa->moveId,qa->partySlot,qa->selectedMoveSlot,qa->taskState,qa->tmTaskMatches,qa->printerId,
        qa->printerActive,qa->printerMatches,qa->printerState,qa->printerWaiting,qa->yesNoActive,qa->yesNoReady,qa->yesNoResult,qa->yesNoTimer,
        qa->yesNoX,qa->yesNoY,qa->yesNoWidth,qa->yesNoHeight,qa->yesHitX[0],qa->yesHitX[1],qa->yesHitY[0],qa->yesHitY[1],
        qa->partyBusy,qa->graphicsReady,qa->fadeFinished,reads,changed,!strcmp(event,"accepted"),x,y
#ifdef HG_SUMMARY_RARE_CANDY_CHECK
        ,request->role
#endif
        );
    (void)capture;
}
static void tmPeripheralLeaseEvent(Capture *capture,const char *event,u32 reads,BOOL changed) {
    QA_CHECK(tmLeaseRequest.sequence && (leaseOwner==OWNER_BAG || leaseOwner==OWNER_PARTY));
    tmTypedEvidence(capture,&tmLeaseRequest,event,reads,changed,0,0);
    if (!strcmp(event,"released")) ++tmTypedReleased;
    else { QA_CHECK(!strcmp(event,"settled") && reads-leaseReleaseReads>=2); ++tmTypedSettled; }
}
static void tmQueue(Capture *capture,const HGSummaryLearnQAState *qa,const HGNativeBagUIState *bag,
    const HGNativePartyUIState *party,u32 producer,u32 kind,u32 index,BOOL advance) {
    u32 x,y;
    QA_CHECK(!leaseActive && !tmPending.sequence && tmPeripheralReady(qa,bag,party,producer,kind,index));
    Choice choice=producer==OWNER_BAG ? (Choice){bag->revision,bag->phase,kind,index} : (Choice){party->revision,party->phase,kind,index};
    QA_CHECK(producer==OWNER_BAG ? HGNativeBagUIResolveTouch(bag,choice.revision,choice.phase,kind,index,&x,&y)
        : HGNativePartyUIResolveTouch(party,choice.revision,choice.phase,kind,index,&x,&y));
    tmPending=(TMRequest){.choice=choice,.owner=*qa,.producer=producer,.sequence=++tmSequence,.stage=stage,.advance=advance};
#ifdef HG_SUMMARY_RARE_CANDY_CHECK
    tmPending.role=producer==OWNER_PARTY ? party->actions[kind==HG_PARTY_UI_TOUCH_ACTION ? index : 0].textId
        : kind==HG_BAG_UI_TOUCH_ACTION ? bag->actions[index].actionId : 0;
#endif
    tmTypedEvidence(capture,&tmPending,"queued",HGNativeInputPadReads(capture->env),FALSE,0,0);
}
static BOOL tmTakeChoice(Capture *capture,const HGSummaryLearnQAState *qa,const HGNativeBagUIState *bag,const HGNativePartyUIState *party) {
    if (!tmPending.sequence) return FALSE;
    u32 x,y; TMRequest request=tmPending; tmPending.sequence=0;
    BOOL current=qa->revision==request.owner.revision && qa->owner==request.owner.owner && qa->generation==request.owner.generation
        && qa->manager==request.owner.manager && qa->data==request.owner.data && stage==request.stage
        && tmPeripheralReady(qa,bag,party,request.producer,request.choice.kind,request.choice.index);
    BOOL resolved=current && (request.producer==OWNER_BAG
        ? HGNativeBagUIResolveTouch(bag,request.choice.revision,request.choice.phase,request.choice.kind,request.choice.index,&x,&y)
        : HGNativePartyUIResolveTouch(party,request.choice.revision,request.choice.phase,request.choice.kind,request.choice.index,&x,&y));
    if (!resolved) {
        tmTypedEvidence(capture,&request,"rejected",HGNativeInputPadReads(capture->env),TRUE,0,0); ++tmTypedRejected; return TRUE;
    }
#ifdef HG_SUMMARY_RARE_CANDY_CHECK
    if (request.producer==OWNER_PARTY && (request.role==HG_PARTY_UI_TEXT_CANDY_STATS_DIFF || request.role==HG_PARTY_UI_TEXT_CANDY_STATS_TOTAL))
        candyStatsEvidence(capture,qa,party);
#endif
    tmLeaseRequest=request; tmLeaseQARevision=qa->revision;
    pulse(capture,request.producer,request.choice.revision,request.choice.phase,(HGNativeSummaryUIInput){.touchPressed=1,.touchX=x,.touchY=y});
    tmTypedEvidence(capture,&request,"accepted",leaseReads,FALSE,x,y); ++tmTypedAccepted;
    if (request.producer==OWNER_BAG && bag->phase==HG_BAG_UI_TM_MESSAGE && qa->taskState==3 && !request.choice.index)
        tmBagYesChoice=request.choice;
#ifdef HG_SUMMARY_RARE_CANDY_CHECK
    if (request.producer==OWNER_PARTY && stage>=TM_STOP_NO
        && (party->phase==HG_PARTY_UI_TM_MESSAGE || (party->phase==HG_PARTY_UI_ITEM_USE && qa->taskState==4))) ++tmPartyReturnAcks;
#else
    if (request.producer==OWNER_PARTY && party->phase==HG_PARTY_UI_TM_MESSAGE && stage>=TM_STOP_NO) ++tmPartyReturnAcks;
#endif
    if (request.advance) nextStage();
    return TRUE;
}
static void tmChooseBag(Capture *capture,const HGSummaryLearnQAState *qa,const HGNativeBagUIState *bag,
    const HGNativePartyUIState *party,u32 kind,u32 index,BOOL advance) {
    if (tmPeripheralReady(qa,bag,party,OWNER_BAG,kind,index))
        tmQueue(capture,qa,bag,party,OWNER_BAG,kind,index,advance);
}
static void tmChooseParty(Capture *capture,const HGSummaryLearnQAState *qa,const HGNativeBagUIState *bag,
    const HGNativePartyUIState *party,u32 kind,u32 index,BOOL advance) {
    if (tmPeripheralReady(qa,bag,party,OWNER_PARTY,kind,index))
        tmQueue(capture,qa,bag,party,OWNER_PARTY,kind,index,advance);
}
static void tmObserveExitFade(Capture *capture,const HGSummaryLearnQAState *qa,const HGNativeBagUIState *bag) {
    if (tmExitFadeSeen || !bag->active || bag->phase!=HG_BAG_UI_TM_EXIT_FADE
        || qa->owner!=HG_SUMMARY_LEARN_BAG || qa->execState!=2 || qa->phase!=HG_BAG_UI_TM_EXIT_FADE) return;
    u32 x,y,reads=HGNativeInputPadReads(capture->env);
    QA_CHECK(stage==TM_TARGET && tmBagYesChoice.revision && qa->owner==HG_SUMMARY_LEARN_BAG && qa->execState==2
        && qa->phase==HG_BAG_UI_TM_EXIT_FADE && HGNativeBagUIIsTMHM(bag) && !bag->ready
        && bag->selectedItemId==ITEM_TM17 && bag->actionCount==1 && !bag->actionMask && !bag->actions[0].enabled && !bag->cancelEnabled);
    QA_CHECK(!HGNativeBagUIResolveTouch(bag,tmBagYesChoice.revision,tmBagYesChoice.phase,tmBagYesChoice.kind,tmBagYesChoice.index,&x,&y));
    HGNativeBagUIState forged=*bag; forged.ready=TRUE;
    QA_CHECK(!HGNativeBagUIResolveTouch(&forged,forged.revision,forged.phase,HG_BAG_UI_TOUCH_ACTION,0,&x,&y));
    HGNativeBagUIState after=readTmBag(capture);
    QA_CHECK(!memcmp(bag,&after,sizeof(after)) && HGNativeInputPadReads(capture->env)==reads);
    tmExitFadeSeen=1; tmExitFadeRejected=2; staleRejected+=2;
    printf("Summary TM exit fade evidence: {\"stage\":%u,\"owner\":%u,\"generation\":%u,\"qaRevision\":%u,\"revision\":%u,\"phase\":%u,\"ready\":%u,\"actionCount\":%u,\"actionMask\":%u,\"enabled\":%u,\"oldChoiceRejected\":true,\"forgedReadyRejected\":true,\"inputUnchanged\":true,\"oldRevision\":%u,\"oldPhase\":%u,\"oldKind\":%u,\"oldIndex\":%u}\n",
        stage,qa->owner,qa->generation,qa->revision,bag->revision,bag->phase,bag->ready,bag->actionCount,bag->actionMask,bag->actions[0].enabled,tmBagYesChoice.revision,tmBagYesChoice.phase,tmBagYesChoice.kind,tmBagYesChoice.index);
}
static void tmSemantics(Capture *capture,BOOL learned) {
    u32 target=w2c_hgengine_Party_GetMonByIndex(capture->game,partyAddress,0);
    u32 friendship=w2c_hgengine_GetMonData(capture->game,target,MON_DATA_FRIENDSHIP,0);
    u32 mood=w2c_hgengine_GetMonData(capture->game,target,MON_DATA_MOOD,0);
    printf("Summary TM semantic boundary evidence: {\"stage\":%u,\"frame\":%u,\"learned\":%u,\"personality\":%u,\"originalPersonality\":%u,\"otId\":%u,\"originalOtId\":%u,\"friendship\":%u,\"expectedFriendship\":%u,\"mood\":%u,\"expectedMood\":%u}\n",
        stage,OS_GetVBlankCount(),learned,w2c_hgengine_GetMonData(capture->game,target,MON_DATA_PERSONALITY,0),originalMonData[0][0],
        w2c_hgengine_GetMonData(capture->game,target,MON_DATA_OT_ID,0),originalMonData[0][1],friendship,tmInitialFriendship+(learned ? 1u : 0u),mood,tmInitialMood+(learned ? 40u : 0u));
    QA_CHECK(w2c_hgengine_Party_GetCount(capture->game,partyAddress)==partyCount);
    for (u32 i=0;i<partyCount;++i) {
        u32 mon=w2c_hgengine_Party_GetMonByIndex(capture->game,partyAddress,i);
        for (u32 j=0;j<MON_ATTRIBUTES;++j) {
            u32 expected=originalMonData[i][j];
            if (learned && !i) {
                if (j==10) expected=MOVE_PROTECT;
                else if (j==14 || j==22) expected=w2c_hgengine_GetMoveMaxPP(capture->game,MOVE_PROTECT,0);
                else if (j==18) expected=0;
            }
            QA_CHECK(w2c_hgengine_GetMonData(capture->game,mon,monAttributes[j],0)==expected);
        }
    }
    QA_CHECK(w2c_hgengine_CheckGotStarter(capture->game,flagsAddress)==starter
        && w2c_hgengine_CheckGotPokedex(capture->game,flagsAddress)==gotDex
        && w2c_hgengine_Pokedex_IsEnabled(capture->game,dexAddress)==dexEnabled
        && w2c_hgengine_Pokedex_CountNationalDexSeen(capture->game,dexAddress)==dexSeen
        && w2c_hgengine_Pokedex_CountNationalDexOwned(capture->game,dexAddress)==dexCaught
        && w2c_hgengine_PlayerProfile_GetMoney(capture->game,profileAddress)==money);
    QA_CHECK(!memcmp(tmOriginalFlags,guest(capture,flagsAddress,tmFlagsSize),tmFlagsSize));
    QA_CHECK(friendship==tmInitialFriendship+(learned ? 1u : 0u) && mood==tmInitialMood+(learned ? 40u : 0u));
    u8 *expectedBag=malloc(bagSize); QA_CHECK(expectedBag); memcpy(expectedBag,originalBag,bagSize);
    if (learned) { u16 quantity=1; memcpy(expectedBag+tmSlotOffset+2,&quantity,sizeof(quantity)); }
    QA_CHECK(!memcmp(expectedBag,guest(capture,bagAddress,bagSize),bagSize)); free(expectedBag);
    QA_CHECK(w2c_hgengine_Bag_GetQuantity(capture->game,bagAddress,ITEM_TM17,HEAP_ID_FIELD2)==(learned ? 1u : 2u));
}
static void tmFinish(Capture *capture,const char *status,BOOL passed) {
    QA_CHECK(HGNativeSetInput(capture->env,0,FALSE,0,0,FALSE));
    if (passed) tmSemantics(capture,TRUE);
    else if (tmFixturePrepared) saveBytes(capture,"failure-bag.bin",guest(capture,bagAddress,bagSize),bagSize);
    caseStatus=status; casePassed=passed; resetExpected=TRUE;
    w2c_env_HGNativeResetControl(capture->env,HG_RESET_REQUEST,0); QA_CHECK(FALSE);
}
static void tmPrepare(Capture *capture) {
    saveAddress=w2c_hgengine_SaveData_Get(capture->game);
    flagsAddress=w2c_hgengine_Save_VarsFlags_Get(capture->game,saveAddress);
    partyAddress=w2c_hgengine_SaveArray_Party_Get(capture->game,saveAddress);
    dexAddress=w2c_hgengine_Save_Pokedex_Get(capture->game,saveAddress);
    bagAddress=w2c_hgengine_Save_Bag_Get(capture->game,saveAddress);
    profileAddress=w2c_hgengine_Save_PlayerData_GetProfile(capture->game,saveAddress);
    QA_CHECK(w2c_hgengine_CheckGotStarter(capture->game,flagsAddress) && w2c_hgengine_Party_GetCount(capture->game,partyAddress)==1);
    u32 mon=w2c_hgengine_Party_GetMonByIndex(capture->game,partyAddress,0); guest(capture,mon,1);
    QA_CHECK(w2c_hgengine_GetMonData(capture->game,mon,MON_DATA_SPECIES,0)==152
        && w2c_hgengine_GetMonData(capture->game,mon,MON_DATA_LEVEL,0)==5
        && w2c_hgengine_GetMonData(capture->game,mon,MON_DATA_HP,0)==20
        && w2c_hgengine_GetMonData(capture->game,mon,MON_DATA_MAX_HP,0)==20
        && !w2c_hgengine_GetMonData(capture->game,mon,MON_DATA_HELD_ITEM,0)
        && w2c_hgengine_GetMonData(capture->game,mon,MON_DATA_OT_ID,0)==w2c_hgengine_PlayerProfile_GetTrainerID(capture->game,profileAddress)
        && !w2c_hgengine_Bag_GetQuantity(capture->game,bagAddress,ITEM_TM17,HEAP_ID_FIELD2));
    tmMapSection=readLearn(capture).mapSection; QA_CHECK(tmMapSection);
    u32 originalPersonality=w2c_hgengine_GetMonData(capture->game,mon,MON_DATA_PERSONALITY,0);
    u32 originalOtId=w2c_hgengine_GetMonData(capture->game,mon,MON_DATA_OT_ID,0);
    u16 profileName[PLAYER_NAME_LENGTH+1],otName[PLAYER_NAME_LENGTH+1]; u32 nameLength=0;
    memcpy(profileName,guest(capture,profileAddress+offsetof(PlayerProfile,name),sizeof(profileName)),sizeof(profileName));
    while (nameLength<PLAYER_NAME_LENGTH+1 && profileName[nameLength]!=EOS) ++nameLength;
    QA_CHECK(nameLength && nameLength<=PLAYER_NAME_LENGTH);
    guest(capture,capture->buffer,sizeof(otName));
    w2c_hgengine_GetMonData(capture->game,mon,MON_DATA_OT_NAME,capture->buffer);
    memcpy(otName,guest(capture,capture->buffer,sizeof(otName)),sizeof(otName));
    QA_CHECK(!memcmp(otName,profileName,(nameLength+1)*sizeof(*otName)));
    const u16 moves[4]={MOVE_TACKLE,MOVE_GROWL,MOVE_RAZOR_LEAF,MOVE_CUT};
    for (u32 i=0;i<4;++i) w2c_hgengine_MonSetMoveInSlot_ResetPpUp(capture->game,mon,moves[i],i);
    QA_CHECK(w2c_hgengine_Bag_AddItem(capture->game,bagAddress,ITEM_TM17,2,HEAP_ID_FIELD2));
    semanticEvidence(capture,TRUE); selectedSlot=0;
    QA_CHECK(originalMonData[0][0]==originalPersonality && originalMonData[0][1]==originalOtId);
    tmInitialFriendship=w2c_hgengine_GetMonData(capture->game,mon,MON_DATA_FRIENDSHIP,0);
    tmInitialMood=w2c_hgengine_GetMonData(capture->game,mon,MON_DATA_MOOD,0);
    // Keep the real starter's trainer/follower identity; seed only its four moves and two TMs.
    // Its zero egg location differs from Route29, so the original TM event adds exactly 1 and mood adds 40.
    QA_CHECK(tmInitialFriendship==70 && !tmInitialMood && tmMapSection
        && !w2c_hgengine_GetMonData(capture->game,mon,MON_DATA_HELD_ITEM,0)
        && w2c_hgengine_GetMonData(capture->game,mon,MON_DATA_DP_POKEBALL,0)!=BALL_LUXURY
        && !w2c_hgengine_GetMonData(capture->game,mon,MON_DATA_EGG_LOCATION,0));
    tmFlagsSize=w2c_hgengine_Save_VarsFlags_sizeof(capture->game); QA_CHECK(tmFlagsSize && tmFlagsSize<65536);
    tmOriginalFlags=malloc(tmFlagsSize); QA_CHECK(tmOriginalFlags); memcpy(tmOriginalFlags,guest(capture,flagsAddress,tmFlagsSize),tmFlagsSize);
    tmSlotOffset=bagSize;
    for (u32 i=0;i<NUM_BAG_TMS_HMS;++i) {
        u32 slot=w2c_hgengine_Bag_GetPocketSlotN(capture->game,bagAddress,POCKET_TMHMS,i); u16 values[2];
        memcpy(values,guest(capture,slot,sizeof(values)),sizeof(values));
        if (values[0]==ITEM_TM17) { QA_CHECK(values[1]==2 && slot>=bagAddress && slot-bagAddress<=bagSize-4); tmSlotOffset=slot-bagAddress; break; }
    }
    QA_CHECK(tmSlotOffset<bagSize); tmInitialQuantity=2; tmFixturePrepared=1;
    tmSemantics(capture,FALSE);
    printf("Summary TM fixture evidence: {\"preparedBeforeFirstFieldAction\":true,\"species\":152,\"partyCount\":1,\"moves\":[33,45,75,15],\"item\":344,\"quantity\":2,\"tmSlotOffset\":%u,\"flagsBytes\":%u,\"initialFriendship\":%u,\"initialMood\":%u,\"mapSection\":%u,\"eggLocation\":0,\"otNameProfileMatched\":true,\"otNameLength\":%u,\"fixtureUsesExistingStarter\":true,\"originalPersonality\":%u,\"originalOtId\":%u,\"privateSaveMemoryOnly\":true}\n",tmSlotOffset,tmFlagsSize,tmInitialFriendship,tmInitialMood,tmMapSection,nameLength,originalPersonality,originalOtId);
}
static BOOL tmContinuePrinter(Capture *capture,const HGSummaryLearnQAState *qa,const HGNativeBagUIState *bag,const HGNativePartyUIState *party) {
    BOOL bagWait=qa->owner==HG_SUMMARY_LEARN_BAG && qa->phase==HG_BAG_UI_TM_MESSAGE && qa->taskState==2 && qa->tmTaskMatches;
    BOOL partyWait=qa->owner==HG_SUMMARY_LEARN_PARTY && (qa->context==6 || qa->context==7) && qa->phase==HG_PARTY_UI_TM_MESSAGE;
    if (!(bagWait || partyWait) || !qa->graphicsReady || !qa->fadeFinished || !tmPrinterReady(qa)) return FALSE;
    u32 producer=bagWait ? OWNER_BAG : OWNER_PARTY,kind=bagWait ? HG_BAG_UI_TOUCH_ACTION : HG_PARTY_UI_TOUCH_ACTION;
    if (!tmPeripheralReady(qa,bag,party,producer,kind,0)) return FALSE;
    tmQueue(capture,qa,bag,party,producer,kind,0,FALSE); return TRUE;
}
static BOOL tmBootReady(u32 map,u32 ready,u32 owner,u32 inputStep,u32 inputFrame) {
    return map && ready==TRUE && !owner && inputStep==3 && !inputFrame;
}
static void TMLearnPresent(Capture *capture) {
    u32 frame=OS_GetVBlankCount();
    if (!stage) {
        static BOOL started,logged;
        static u32 startFrame,lastFrame,lastMap,lastRevision,lastInputStep,lastInputFrame;
        if (!started) {
            QA_CHECK(tmBootReady(33,TRUE,0,3,0) && !tmBootReady(0,TRUE,0,3,0)
                && !tmBootReady(33,FALSE,0,3,0) && !tmBootReady(33,TRUE,1,3,0)
                && !tmBootReady(33,TRUE,0,2,0) && !tmBootReady(33,TRUE,0,3,1320));
            started=TRUE; startFrame=frame;
        }
        present(capture);
        u32 map=w2c_hgengine_HGNativeFieldMap(capture->game);
        HGSummaryLearnQAState qa;
        // The original field owner publishes this map only after the private observation hook.
        if (map) qa=readLearn(capture);
        if (!logged || map!=lastMap || (map && qa.revision!=lastRevision)
            || capture->inputStep!=lastInputStep || capture->inputFrame!=lastInputFrame || frame-lastFrame>=300) {
            printf("Summary TM boot evidence: {\"hostFrame\":%u,\"elapsedFrames\":%u,\"map\":%u,\"inputStep\":%u,\"inputFrame\":%u,\"qaObserved\":%s",
                frame,frame-startFrame,map,capture->inputStep,capture->inputFrame,map ? "true" : "false");
            if (map) printf(",\"qaVersion\":%u,\"qaBytes\":%u,\"qaFrame\":%u,\"revision\":%u,\"generation\":%u,\"fieldReady\":%u,\"owner\":%u,\"manager\":%u,\"execState\":%u,\"phase\":%u,\"fadeFinished\":%u,\"mapSection\":%u",
                qa.version,qa.byteSize,qa.frame,qa.revision,qa.generation,qa.fieldReady,qa.owner,qa.manager,qa.execState,qa.phase,qa.fadeFinished,qa.mapSection);
            puts("}");
            logged=TRUE; lastFrame=frame; lastMap=map; lastInputStep=capture->inputStep; lastInputFrame=capture->inputFrame;
            if (map) lastRevision=qa.revision;
        }
        if (frame-startFrame>=3600) { tmFinish(capture,"tm_boot_prerequisite_timeout",FALSE); return; }
        if (!map || !tmBootReady(map,qa.fieldReady,qa.owner,capture->inputStep,capture->inputFrame)) return;
        if (!renderBoth(capture,FALSE,FALSE)) return;
        capture->noInput=TRUE; QA_CHECK(HGNativeSetInput(capture->env,0,FALSE,0,0,FALSE));
        tmPrepare(capture); nextStage(); return;
    }
    QA_CHECK(HGNativeSoundRealtime(capture->env,TRUE));
    HGSummaryLearnQAState qa=readLearn(capture);
    static u32 loggedRevision;
    if (qa.revision!=loggedRevision) {
        printf("Summary TM state evidence: {\"stage\":%u,\"frame\":%u,\"revision\":%u,\"owner\":%u,\"generation\":%u,\"execState\":%u,\"phase\":%u,\"context\":%u,\"taskState\":%u,\"taskMatches\":%u,\"printerId\":%u,\"printerActive\":%u,\"printerMatches\":%u,\"printerState\":%u,\"printerWaiting\":%u,\"afterTextState\":%u,\"yesNoActive\":%u,\"yesNoResult\":%u,\"yesNoTimer\":%u,\"yesNoReady\":%u,\"selectedAction\":%u,\"selectedMoveSlot\":%u,\"hmCalls\":%u,\"graphicsReady\":%u,\"fadeFinished\":%u,\"partyBusy\":%u}\n",
            stage,qa.frame,qa.revision,qa.owner,qa.generation,qa.execState,qa.phase,qa.context,qa.taskState,qa.tmTaskMatches,
            qa.printerId,qa.printerActive,qa.printerMatches,qa.printerState,qa.printerWaiting,qa.afterTextState,
            qa.yesNoActive,qa.yesNoResult,qa.yesNoTimer,qa.yesNoReady,qa.selectedAction,qa.selectedMoveSlot,qa.hmCalls,qa.graphicsReady,qa.fadeFinished,qa.partyBusy);
        loggedRevision=qa.revision;
    }
    if (frame-stageFrame>3600) { tmFinish(capture,"tm_scene_or_input_handshake_timeout",FALSE); return; }
    HGNativePartyUIState party=readParty(capture); HGNativeSummaryUIState summary=readSummary(capture);
    HGNativeBagUIState bag=readTmBag(capture);
    // Observe the original disabled exit-fade owner before a changed lease is settled.
    tmObserveExitFade(capture,&qa,&bag);
    if (!settleInput(capture,&party,&summary)) return;
    if (tmTakeChoice(capture,&qa,&bag,&party)) return;
    if (tmContinuePrinter(capture,&qa,&bag,&party)) return;
    HGNativeSummaryUIInput ignored;
    if (stage==TM_OPEN_BAG) {
        if (!qa.fieldReady || w2c_hgengine_HGNativeFieldMap(capture->game)!=33) return;
        u32 icons=w2c_hgengine_HGNativeFieldMenuIcons(capture->game);
        for (u32 i=0;i<7;++i) if (((icons>>(i*4))&15)==2) {
            pulse(capture,OWNER_FIELD,0,0,(HGNativeSummaryUIInput){.touchPressed=1,.touchX=iconCenters[i][0],.touchY=iconCenters[i][1]}); nextStage(); return;
        }
    } else if (stage==TM_POCKET && bag.active && bag.ready && HGNativeBagUIIsList(&bag)) {
        u32 pocket=HG_BAG_UI_POCKETS;
        for (u32 i=0;i<HG_BAG_UI_POCKETS;++i) if (bag.pockets[i].enabled && bag.pockets[i].pocketId==POCKET_TMHMS) pocket=i;
        QA_CHECK(pocket<HG_BAG_UI_POCKETS); tmChooseBag(capture,&qa,&bag,&party,HG_BAG_UI_TOUCH_POCKET,pocket,TRUE);
    } else if (stage==TM_ITEM && bag.active && bag.ready && HGNativeBagUIIsList(&bag) && bag.pocketId==POCKET_TMHMS) {
        u32 row=HG_BAG_UI_ITEMS;
        for (u32 i=0;i<bag.visibleCount;++i) if (bag.items[i].enabled && bag.items[i].itemId==ITEM_TM17) row=i;
        QA_CHECK(row<HG_BAG_UI_ITEMS && bag.items[row].quantity==2); tmChooseBag(capture,&qa,&bag,&party,HG_BAG_UI_TOUCH_ITEM,row,TRUE);
    } else if (stage==TM_USE && bag.active && bag.ready && bag.phase==HG_BAG_UI_ITEM_MENU) {
        u32 row=HG_BAG_UI_ACTIONS;
        for (u32 i=0;i<bag.actionCount;++i) if (bag.actions[i].enabled && bag.actions[i].actionId==0) row=i;
        QA_CHECK(row<HG_BAG_UI_ACTIONS && bag.selectedItemId==ITEM_TM17); tmChooseBag(capture,&qa,&bag,&party,HG_BAG_UI_TOUCH_ACTION,row,TRUE);
    } else if (stage==TM_BOOT_ACK && qa.owner==HG_SUMMARY_LEARN_BAG && qa.execState==2 && qa.phase==13
        && qa.tmTaskMatches && qa.taskState==1 && qa.printerId<8 && !qa.printerActive && qa.graphicsReady && qa.fadeFinished
        && bag.active && bag.ready && HGNativeBagUIIsTMHM(&bag)) {
        tmChooseBag(capture,&qa,&bag,&party,HG_BAG_UI_TOUCH_ACTION,0,TRUE);
    } else if (stage==TM_BAG_YES && qa.owner==HG_SUMMARY_LEARN_BAG && qa.phase==13 && qa.tmTaskMatches && qa.taskState==3
        && qa.yesNoReady && qa.graphicsReady && qa.fadeFinished && bag.active && bag.ready && HGNativeBagUIIsTMHM(&bag)) {
        tmChooseBag(capture,&qa,&bag,&party,HG_BAG_UI_TOUCH_ACTION,0,TRUE);
    } else if (stage==TM_TARGET && qa.owner==HG_SUMMARY_LEARN_PARTY && qa.context==6 && qa.phase==21
        && qa.execState==2 && !qa.partyBusy && qa.graphicsReady && qa.fadeFinished
        && party.active && party.ready && HGNativePartyUIIsTMHM(&party)) {
        QA_CHECK(tmExitFadeSeen && tmExitFadeRejected==2 && qa.partySlot==0 && qa.itemId==ITEM_TM17 && qa.moveId==MOVE_PROTECT);
        tmChooseParty(capture,&qa,&bag,&party,HG_PARTY_UI_TOUCH_MON,0,TRUE);
    } else if ((stage==TM_FORGET_YES || stage==TM_STOP_NO || stage==TM_RETRY_YES) && qa.owner==HG_SUMMARY_LEARN_PARTY
        && qa.phase==27 && qa.execState==2 && qa.yesNoReady && qa.graphicsReady && qa.fadeFinished
        && party.active && party.ready && HGNativePartyUIIsTMHM(&party)) {
        QA_CHECK(qa.context==(stage==TM_FORGET_YES ? 6u : 7u) && qa.itemId==ITEM_TM17 && qa.moveId==MOVE_PROTECT);
        if (stage==TM_STOP_NO) { QA_CHECK(qa.selectedMoveSlot==4); tmFifthCancelled=1; }
        tmChooseParty(capture,&qa,&bag,&party,HG_PARTY_UI_TOUCH_ACTION,stage==TM_STOP_NO ? 1 : 0,TRUE);
    } else if ((stage==TM_FIRST_SUMMARY || stage==TM_SECOND_SUMMARY) && summary.active && summary.ready
        && summary.phase==HG_SUMMARY_UI_FORGET_MOVE && qa.owner==HG_SUMMARY_LEARN_SUMMARY && qa.execState==2) {
        QA_CHECK(HGNativeSummaryUIIsLearning(&summary) && summary.monDataType==1 && summary.partySlot==0
            && summary.partyCount==1 && summary.moveToLearn==MOVE_PROTECT && summary.moveMask==31);
        tmSemantics(capture,FALSE); QA_CHECK(snapshot(capture,summary)); ++summaryLifetimes;
        if (stage==TM_FIRST_SUMMARY) {
            tmFirstGeneration=qa.generation; tmFirstChoice=(Choice){summary.revision,summary.phase,HG_SUMMARY_UI_INPUT_MOVE,4};
            QA_CHECK(!HGNativeSummaryUIResolveInput(&summary,summary.revision,summary.phase,HG_SUMMARY_UI_INPUT_MOVE,5,&ignored)); ++staleRejected;
            chooseSummary(capture,&summary,HG_SUMMARY_UI_INPUT_MOVE,4);
        } else {
            QA_CHECK(qa.generation>tmFirstGeneration && tmFifthCancelled && summaryLifetimes==2);
            QA_CHECK(!HGNativeSummaryUIResolveInput(&summary,tmFirstChoice.revision,tmFirstChoice.phase,tmFirstChoice.kind,tmFirstChoice.index,&ignored)); ++staleRejected;
            tmContinuedTrying=1; tmHmCallsBefore=qa.hmCalls; chooseSummary(capture,&summary,HG_SUMMARY_UI_INPUT_MOVE,3);
        }
        nextStage();
    } else if (stage==TM_FIFTH_CONFIRM && summary.active && summary.ready && summary.phase==HG_SUMMARY_UI_FORGET_MOVE && summary.moveSlot==4) {
        QA_CHECK(!HGNativeSummaryUIResolveInput(&summary,tmFirstChoice.revision,tmFirstChoice.phase,tmFirstChoice.kind,tmFirstChoice.index,&ignored)); ++staleRejected;
        QA_CHECK(snapshot(capture,summary)); chooseSummary(capture,&summary,HG_SUMMARY_UI_INPUT_CONFIRM,0); nextStage();
    } else if (stage==TM_HM_RESULT && summary.active && summary.ready && summary.phase==HG_SUMMARY_UI_FORGET_MOVE
        && summary.moveSlot==3 && qa.hmCalls==tmHmCallsBefore+1) {
        QA_CHECK(qa.hmOwner==qa.data && qa.hmMoveSlot==3 && summary.moves[3].moveId==MOVE_CUT && qa.hmWarning.pixelBuffer);
        tmSemantics(capture,FALSE); windowEvidence(capture,summary.phase,"original-hm-warning",0,&qa.hmWarning);
        QA_CHECK(snapshot(capture,summary)); tmHmRejected=1; nextStage();
    } else if ((stage==TM_OLD_MOVE || stage==TM_OLD_AGAIN) && summary.active && summary.ready && summary.phase==HG_SUMMARY_UI_FORGET_MOVE) {
        if (stage==TM_OLD_AGAIN) tmConfirmationBack=1;
        tmOldChoice=(Choice){summary.revision,summary.phase,HG_SUMMARY_UI_INPUT_MOVE,0};
        chooseSummary(capture,&summary,HG_SUMMARY_UI_INPUT_MOVE,0); nextStage();
    } else if ((stage==TM_CONFIRM_BACK || stage==TM_CONFIRM_REPLACE) && summary.active && summary.ready && summary.phase==HG_SUMMARY_UI_FORGET_CONFIRM) {
        QA_CHECK(summary.moveSlot==0 && !HGNativeSummaryUIResolveInput(&summary,tmOldChoice.revision,tmOldChoice.phase,tmOldChoice.kind,tmOldChoice.index,&ignored)); ++staleRejected;
        tmSemantics(capture,FALSE); QA_CHECK(snapshot(capture,summary));
        chooseSummary(capture,&summary,stage==TM_CONFIRM_BACK ? HG_SUMMARY_UI_INPUT_BACK : HG_SUMMARY_UI_INPUT_CONFIRM,0); nextStage();
    } else if (stage==TM_RETURN_BAG && bag.active && bag.ready && HGNativeBagUIIsList(&bag)) {
        QA_CHECK(!summary.active && !party.active && tmHmRejected);
        tmReplacement=1; tmSemantics(capture,TRUE);
        u32 mon=w2c_hgengine_Party_GetMonByIndex(capture->game,partyAddress,0);
        tmFinalMove=w2c_hgengine_GetMonData(capture->game,mon,MON_DATA_MOVE1,0);
        tmFinalPpUps=w2c_hgengine_GetMonData(capture->game,mon,MON_DATA_MOVE1_PP_UPS,0);
        tmFinalPp=w2c_hgengine_GetMonData(capture->game,mon,MON_DATA_MOVE1_PP,0);
        tmFinalMaxPp=w2c_hgengine_GetMonData(capture->game,mon,MON_DATA_MOVE1_MAX_PP,0);
        tmFinalFriendship=w2c_hgengine_GetMonData(capture->game,mon,MON_DATA_FRIENDSHIP,0);
        tmFinalMood=w2c_hgengine_GetMonData(capture->game,mon,MON_DATA_MOOD,0);
        tmFinalQuantity=w2c_hgengine_Bag_GetQuantity(capture->game,bagAddress,ITEM_TM17,HEAP_ID_FIELD2);
        QA_CHECK(snapshot(capture,summary)); nextStage();
    } else if (stage==TM_CLOSE_BAG && bag.active && bag.ready && HGNativeBagUIIsList(&bag)) {
        tmChooseBag(capture,&qa,&bag,&party,HG_BAG_UI_TOUCH_CANCEL,0,TRUE);
    } else if (stage==TM_FIELD && qa.fieldReady && !bag.active && !party.active && !summary.active) {
        QA_CHECK(tmFixturePrepared && tmFifthCancelled && tmContinuedTrying && tmHmRejected && tmConfirmationBack && tmReplacement
            && summaryLifetimes==2 && staleRejected>=4 && seenPhases==((1u<<8)|(1u<<9))
            && !tmPending.sequence && !tmLeaseQARevision && tmTypedAccepted && tmTypedAccepted==tmTypedReleased
            && tmTypedAccepted==tmTypedSettled && tmPartyReturnAcks && tmExitFadeSeen && tmExitFadeRejected==2);
        QA_CHECK(snapshot(capture,summary)); tmFinish(capture,"passed_original_summary_tm_learning",TRUE);
    }
}
#ifdef HG_SUMMARY_RARE_CANDY_CHECK
#ifdef HG_EVOLUTION_LEARN_CHECK
#define CANDY_START_LEVEL 31
#define CANDY_END_LEVEL 32
#define CANDY_END_EXP 27021u
#elif defined(HG_EVOLUTION_CHECK)
#define CANDY_START_LEVEL 15
#define CANDY_END_LEVEL 16
#define CANDY_END_EXP 2535u
#else
#define CANDY_START_LEVEL 5
#define CANDY_END_LEVEL 6
#define CANDY_END_EXP 179u
#endif
static u32 candyInitialStats[6],candyExpectedStats[6],candyInitialExp,candyStatsMask,candyDiffRevision,candyStatsStale,candyWrongContext,candyBusyRejected,candyCancelKeptUpgrade;
static u32 candyOriginal[MON_ATTRIBUTES],candyFinalLevel,candyFinalExperience,candyFinalStats[6],candyIV[6],candyEV[6],candyFinalSpecies;
static const u32 candyStatAttrs[6]={MON_DATA_MAX_HP,MON_DATA_ATK,MON_DATA_DEF,MON_DATA_SP_ATK,MON_DATA_SP_DEF,MON_DATA_SPEED};
static void candyPrepareStats(Capture *capture,u32 mon) {
    // Chikorita's original personal.json base data; same integer formula as CalcMonStats.
    const u32 base[6]={45,49,65,49,65,45};
    const u32 iv[6]={MON_DATA_HP_IV,MON_DATA_ATK_IV,MON_DATA_DEF_IV,MON_DATA_SPATK_IV,MON_DATA_SPDEF_IV,MON_DATA_SPEED_IV};
    const u32 ev[6]={MON_DATA_HP_EV,MON_DATA_ATK_EV,MON_DATA_DEF_EV,MON_DATA_SPATK_EV,MON_DATA_SPDEF_EV,MON_DATA_SPEED_EV};
    const u32 natureIndex[6]={0,0,1,3,4,2}; u32 nature=originalMonData[0][0]%25;
    memcpy(candyOriginal,originalMonData[0],sizeof(candyOriginal));
    candyInitialExp=w2c_hgengine_GetMonData(capture->game,mon,MON_DATA_EXPERIENCE,0);
#ifdef HG_EVOLUTION_CHECK
#ifdef HG_EVOLUTION_LEARN_CHECK
    QA_CHECK(candyInitialExp==24294);
#else
    QA_CHECK(candyInitialExp==2035);
#endif
#else
    QA_CHECK(candyInitialExp>=135 && candyInitialExp<179);
#endif
    for (u32 i=0;i<6;++i) for (u32 level=CANDY_START_LEVEL;level<=CANDY_END_LEVEL;++level) {
        u32 value=(2*base[i]+w2c_hgengine_GetMonData(capture->game,mon,iv[i],0)
            +w2c_hgengine_GetMonData(capture->game,mon,ev[i],0)/4)*level/100+(i ? 5 : level+10);
        if (i && nature/5!=nature%5) {
            if (natureIndex[i]==nature/5) value=(u16)(value*110)/100;
            else if (natureIndex[i]==nature%5) value=(u16)(value*90)/100;
        }
        if (level==CANDY_START_LEVEL) { candyIV[i]=w2c_hgengine_GetMonData(capture->game,mon,iv[i],0);candyEV[i]=w2c_hgengine_GetMonData(capture->game,mon,ev[i],0);
            QA_CHECK(candyIV[i]<=31 && candyEV[i]<=255);candyInitialStats[i]=w2c_hgengine_GetMonData(capture->game,mon,candyStatAttrs[i],0); QA_CHECK(value==candyInitialStats[i]); }
        else candyExpectedStats[i]=value;
    }
#ifdef HG_EVOLUTION_CHECK
#ifdef HG_EVOLUTION_LEARN_CHECK
    printf("Evolution expected stats evidence: {\"initialExperience\":%u,\"expectedExperience\":27021,\"initial\":[",candyInitialExp);
#else
    printf("Evolution expected stats evidence: {\"initialExperience\":%u,\"expectedExperience\":2535,\"initial\":[",candyInitialExp);
#endif
#else
    printf("Summary Candy expected stats evidence: {\"initialExperience\":%u,\"expectedExperience\":179,\"initial\":[",candyInitialExp);
#endif
    for (u32 i=0;i<6;++i) printf("%s%u",i ? "," : "",candyInitialStats[i]);
    fputs("],\"expected\":[",stdout);for (u32 i=0;i<6;++i) printf("%s%u",i ? "," : "",candyExpectedStats[i]);puts("]}");
}
static void candySemantics(Capture *capture,BOOL upgraded,BOOL learned) {
    QA_CHECK(tmFixturePrepared && (!learned || upgraded) && w2c_hgengine_Party_GetCount(capture->game,partyAddress)==1);
    u32 mon=w2c_hgengine_Party_GetMonByIndex(capture->game,partyAddress,0);
    for (u32 j=0;j<MON_ATTRIBUTES;++j) {
        u32 value=candyOriginal[j];
        if (upgraded) { if (j==6 || j==7) value=candyExpectedStats[0]; else if (j==8) value=CANDY_END_LEVEL; }
        if (learned) { if (j==10) value=MOVE_RAZOR_LEAF; else if (j==14 || j==22) value=w2c_hgengine_GetMoveMaxPP(capture->game,MOVE_RAZOR_LEAF,0); else if (j==18) value=0; }
        QA_CHECK(w2c_hgengine_GetMonData(capture->game,mon,monAttributes[j],0)==value);
    }
    const u32 order[6]={0,1,2,4,5,3};
    for (u32 i=0;i<6;++i) {
        QA_CHECK(w2c_hgengine_GetMonData(capture->game,mon,candyStatAttrs[i],0)==(upgraded ? candyExpectedStats[i] : candyInitialStats[i]));
        QA_CHECK(w2c_hgengine_GetMonData(capture->game,mon,MON_DATA_HP_IV+order[i],0)==candyIV[i]
            && w2c_hgengine_GetMonData(capture->game,mon,MON_DATA_HP_EV+order[i],0)==candyEV[i]);
    }
    QA_CHECK(w2c_hgengine_GetMonData(capture->game,mon,MON_DATA_EXPERIENCE,0)==(upgraded ? CANDY_END_EXP : candyInitialExp));
    QA_CHECK(w2c_hgengine_GetMonData(capture->game,mon,MON_DATA_FRIENDSHIP,0)==(upgraded ? 75u : 70u)
        && w2c_hgengine_GetMonData(capture->game,mon,MON_DATA_MOOD,0)==(upgraded ? 40u : 0u));
    QA_CHECK(!memcmp(tmOriginalFlags,guest(capture,flagsAddress,tmFlagsSize),tmFlagsSize)
        && w2c_hgengine_CheckGotStarter(capture->game,flagsAddress)==starter && w2c_hgengine_CheckGotPokedex(capture->game,flagsAddress)==gotDex
        && w2c_hgengine_Pokedex_IsEnabled(capture->game,dexAddress)==dexEnabled
        && w2c_hgengine_Pokedex_CountNationalDexSeen(capture->game,dexAddress)==dexSeen
        && w2c_hgengine_Pokedex_CountNationalDexOwned(capture->game,dexAddress)==dexCaught
        && w2c_hgengine_PlayerProfile_GetMoney(capture->game,profileAddress)==money);
    u8 *expected=malloc(bagSize);QA_CHECK(expected);memcpy(expected,originalBag,bagSize);
    if (upgraded) { u16 quantity=1;memcpy(expected+tmSlotOffset+2,&quantity,2); }
    QA_CHECK(!memcmp(expected,guest(capture,bagAddress,bagSize),bagSize));free(expected);
    QA_CHECK(w2c_hgengine_Bag_GetQuantity(capture->game,bagAddress,ITEM_RARE_CANDY,HEAP_ID_FIELD2)==(upgraded ? 1u : 2u));
    if (upgraded) { originalMonData[0][6]=originalMonData[0][7]=candyExpectedStats[0];originalMonData[0][8]=CANDY_END_LEVEL; }
    printf("Summary Candy semantic boundary evidence: {\"stage\":%u,\"upgraded\":%u,\"learned\":%u,\"level\":%u,\"experience\":%u,\"quantity\":%u,\"friendship\":%u,\"mood\":%u,\"personality\":%u,\"otId\":%u}\n",
        stage,upgraded,learned,w2c_hgengine_GetMonData(capture->game,mon,MON_DATA_LEVEL,0),w2c_hgengine_GetMonData(capture->game,mon,MON_DATA_EXPERIENCE,0),
        w2c_hgengine_Bag_GetQuantity(capture->game,bagAddress,ITEM_RARE_CANDY,HEAP_ID_FIELD2),
        w2c_hgengine_GetMonData(capture->game,mon,MON_DATA_FRIENDSHIP,0),w2c_hgengine_GetMonData(capture->game,mon,MON_DATA_MOOD,0),
        w2c_hgengine_GetMonData(capture->game,mon,MON_DATA_PERSONALITY,0),w2c_hgengine_GetMonData(capture->game,mon,MON_DATA_OT_ID,0));
}
static void candyFinish(Capture *capture,const char *status,BOOL passed) {
    QA_CHECK(HGNativeSetInput(capture->env,0,FALSE,0,0,FALSE));
    if (passed) candySemantics(capture,TRUE,TRUE);
    else if (tmFixturePrepared) saveBytes(capture,"candy-failure-bag.bin",guest(capture,bagAddress,bagSize),bagSize);
    caseStatus=status;casePassed=passed;resetExpected=TRUE;w2c_env_HGNativeResetControl(capture->env,HG_RESET_REQUEST,0);QA_CHECK(FALSE);
}
static void candyStatsEvidence(Capture *capture,const HGSummaryLearnQAState *qa,const HGNativePartyUIState *party) {
    u32 expected=stage==6 ? HG_PARTY_UI_TEXT_CANDY_STATS_DIFF : HG_PARTY_UI_TEXT_CANDY_STATS_TOTAL,x,y;
    QA_CHECK((stage==6 || stage==7) && qa->owner==HG_SUMMARY_LEARN_PARTY && qa->context==5 && qa->itemId==ITEM_RARE_CANDY
        && qa->taskState==stage-5 && qa->phase==HG_PARTY_UI_ITEM_USE && qa->graphicsReady && qa->fadeFinished
        && party->ready && party->actions[0].textId==expected);
    candySemantics(capture,TRUE,FALSE);
    u32 displayAddress=w2c_hgengine_HGNativePartyUIGetStats(capture->game,party->revision);
    QA_CHECK(displayAddress);const u16 *display=(const u16 *)guest(capture,displayAddress,6*sizeof(u16));
    u16 displayValues[6];memcpy(displayValues,display,sizeof(displayValues));
    for (u32 i=0;i<6;++i) QA_CHECK(displayValues[i]==(stage==6 ? candyExpectedStats[i]-candyInitialStats[i] : candyExpectedStats[i]));
    if (stage==6) { QA_CHECK(!candyStatsMask);candyDiffRevision=party->revision; }
    else { QA_CHECK(candyStatsMask==1 && !w2c_hgengine_HGNativePartyUIGetStats(capture->game,candyDiffRevision)
        && !HGNativePartyUIResolveTouch(party,candyDiffRevision,party->phase,HG_PARTY_UI_TOUCH_ACTION,0,&x,&y));++staleRejected;++candyStatsStale; }
    HGNativePartyUIState busy=*party;busy.actions[0].textId=HG_PARTY_UI_TEXT_CANDY_BUSY;busy.ready=1;busy.actionEnabledMask=1;busy.actions[0].enabled=1;
    QA_CHECK(!HGNativePartyUIResolveTouch(&busy,busy.revision,busy.phase,HG_PARTY_UI_TOUCH_ACTION,0,&x,&y));++candyBusyRejected;
    QA_CHECK(w2c_hgengine_HGNativeRender2D(capture->game,0,capture->buffer,256*192*4));
    const u8 *frame=guest(capture,capture->buffer,256*192*4);u8 pixels[128*112*4];
    for (u32 y=0;y<112;++y) memcpy(pixels+y*128*4,frame+y*256*4,128*4);
    const char *name=stage==6 ? "candy-stats-diff.rgba" : "candy-stats-total.rgba";saveBytes(capture,name,pixels,sizeof(pixels));
    candyStatsMask|=1u<<(stage-6);
    printf("Summary Candy stats evidence: {\"stage\":%u,\"role\":%u,\"revision\":%u,\"qaRevision\":%u,\"generation\":%u,\"owner\":%u,\"context\":%u,\"itemId\":%u,\"phase\":%u,\"taskState\":%u,\"ready\":%u,\"graphicsReady\":%u,\"fadeFinished\":%u,\"file\":\"%s\",\"crop\":[0,0,128,112],\"bytes\":%zu,\"staleDiffRejected\":%u,\"displayValues\":[%u,%u,%u,%u,%u,%u]}\n",
        stage,expected,party->revision,qa->revision,qa->generation,qa->owner,qa->context,qa->itemId,qa->phase,qa->taskState,party->ready,qa->graphicsReady,qa->fadeFinished,name,sizeof(pixels),candyStatsStale,
        displayValues[0],displayValues[1],displayValues[2],displayValues[3],displayValues[4],displayValues[5]);
}
static void candyPrepare(Capture *capture) {
    saveAddress=w2c_hgengine_SaveData_Get(capture->game);
    flagsAddress=w2c_hgengine_Save_VarsFlags_Get(capture->game,saveAddress);
    partyAddress=w2c_hgengine_SaveArray_Party_Get(capture->game,saveAddress);
    dexAddress=w2c_hgengine_Save_Pokedex_Get(capture->game,saveAddress);
    bagAddress=w2c_hgengine_Save_Bag_Get(capture->game,saveAddress);
    profileAddress=w2c_hgengine_Save_PlayerData_GetProfile(capture->game,saveAddress);
    QA_CHECK(w2c_hgengine_CheckGotStarter(capture->game,flagsAddress) && w2c_hgengine_Party_GetCount(capture->game,partyAddress)==1);
    u32 mon=w2c_hgengine_Party_GetMonByIndex(capture->game,partyAddress,0); guest(capture,mon,1);
    QA_CHECK(w2c_hgengine_GetMonData(capture->game,mon,MON_DATA_SPECIES,0)==152
        && w2c_hgengine_GetMonData(capture->game,mon,MON_DATA_LEVEL,0)==5
        && w2c_hgengine_GetMonData(capture->game,mon,MON_DATA_HP,0)==20
        && w2c_hgengine_GetMonData(capture->game,mon,MON_DATA_MAX_HP,0)==20
        && !w2c_hgengine_GetMonData(capture->game,mon,MON_DATA_HELD_ITEM,0)
        && w2c_hgengine_GetMonData(capture->game,mon,MON_DATA_OT_ID,0)==w2c_hgengine_PlayerProfile_GetTrainerID(capture->game,profileAddress)
        && !w2c_hgengine_Bag_GetQuantity(capture->game,bagAddress,ITEM_RARE_CANDY,HEAP_ID_FIELD2));
    tmMapSection=readLearn(capture).mapSection; QA_CHECK(tmMapSection);
    u32 originalPersonality=w2c_hgengine_GetMonData(capture->game,mon,MON_DATA_PERSONALITY,0);
    u32 originalOtId=w2c_hgengine_GetMonData(capture->game,mon,MON_DATA_OT_ID,0);
    u16 profileName[PLAYER_NAME_LENGTH+1],otName[PLAYER_NAME_LENGTH+1]; u32 nameLength=0;
    memcpy(profileName,guest(capture,profileAddress+offsetof(PlayerProfile,name),sizeof(profileName)),sizeof(profileName));
    while (nameLength<PLAYER_NAME_LENGTH+1 && profileName[nameLength]!=EOS) ++nameLength;
    QA_CHECK(nameLength && nameLength<=PLAYER_NAME_LENGTH);
    guest(capture,capture->buffer,sizeof(otName));
    w2c_hgengine_GetMonData(capture->game,mon,MON_DATA_OT_NAME,capture->buffer);
    memcpy(otName,guest(capture,capture->buffer,sizeof(otName)),sizeof(otName));
    QA_CHECK(!memcmp(otName,profileName,(nameLength+1)*sizeof(*otName)));
#ifdef HG_EVOLUTION_CHECK
    QA_CHECK(w2c_hgengine_GetMonData(capture->game,mon,MON_DATA_MOVE1,0)==MOVE_TACKLE
        && w2c_hgengine_GetMonData(capture->game,mon,MON_DATA_MOVE2,0)==MOVE_GROWL
        && !w2c_hgengine_GetMonData(capture->game,mon,MON_DATA_MOVE3,0) && !w2c_hgengine_GetMonData(capture->game,mon,MON_DATA_MOVE4,0));
#ifdef HG_EVOLUTION_LEARN_CHECK
    u32 experience=24294;
#else
    u32 experience=2035;
#endif
    memcpy((void *)guest(capture,capture->buffer,sizeof(experience)),&experience,sizeof(experience));
    w2c_hgengine_SetMonData(capture->game,mon,MON_DATA_EXPERIENCE,capture->buffer);
    w2c_hgengine_CalcMonLevelAndStats(capture->game,mon);
    QA_CHECK(w2c_hgengine_GetMonData(capture->game,mon,MON_DATA_LEVEL,0)==CANDY_START_LEVEL
        && w2c_hgengine_GetMonData(capture->game,mon,MON_DATA_HP,0)==w2c_hgengine_GetMonData(capture->game,mon,MON_DATA_MAX_HP,0));
#ifdef HG_EVOLUTION_LEARN_CHECK
    const u16 moves[4]={MOVE_TACKLE,MOVE_GROWL,MOVE_REFLECT,MOVE_CUT};
    for (u32 i=0;i<4;++i) w2c_hgengine_MonSetMoveInSlot_ResetPpUp(capture->game,mon,moves[i],i);
#endif
#else
    const u16 moves[4]={MOVE_TACKLE,MOVE_GROWL,MOVE_REFLECT,MOVE_CUT};
    for (u32 i=0;i<4;++i) w2c_hgengine_MonSetMoveInSlot_ResetPpUp(capture->game,mon,moves[i],i);
#endif
    QA_CHECK(w2c_hgengine_Bag_AddItem(capture->game,bagAddress,ITEM_RARE_CANDY,2,HEAP_ID_FIELD2));
    semanticEvidence(capture,TRUE); selectedSlot=0;
    QA_CHECK(originalMonData[0][0]==originalPersonality && originalMonData[0][1]==originalOtId);
    tmInitialFriendship=w2c_hgengine_GetMonData(capture->game,mon,MON_DATA_FRIENDSHIP,0);
    tmInitialMood=w2c_hgengine_GetMonData(capture->game,mon,MON_DATA_MOOD,0);
    // Original item50 adds5 friendship/40mood here; ctx8/item0 does not add a TM event.
    QA_CHECK(tmInitialFriendship==70 && !tmInitialMood && tmMapSection
        && !w2c_hgengine_GetMonData(capture->game,mon,MON_DATA_HELD_ITEM,0)
        && w2c_hgengine_GetMonData(capture->game,mon,MON_DATA_DP_POKEBALL,0)!=BALL_LUXURY
        && !w2c_hgengine_GetMonData(capture->game,mon,MON_DATA_EGG_LOCATION,0));
    tmFlagsSize=w2c_hgengine_Save_VarsFlags_sizeof(capture->game); QA_CHECK(tmFlagsSize && tmFlagsSize<65536);
    tmOriginalFlags=malloc(tmFlagsSize); QA_CHECK(tmOriginalFlags); memcpy(tmOriginalFlags,guest(capture,flagsAddress,tmFlagsSize),tmFlagsSize);
    tmSlotOffset=bagSize;
    for (u32 i=0;i<NUM_BAG_MEDICINE;++i) {
        u32 slot=w2c_hgengine_Bag_GetPocketSlotN(capture->game,bagAddress,POCKET_MEDICINE,i); u16 values[2];
        memcpy(values,guest(capture,slot,sizeof(values)),sizeof(values));
        if (values[0]==ITEM_RARE_CANDY) { QA_CHECK(values[1]==2 && slot>=bagAddress && slot-bagAddress<=bagSize-4); tmSlotOffset=slot-bagAddress; break; }
    }
    QA_CHECK(tmSlotOffset<bagSize); tmInitialQuantity=2; tmFixturePrepared=1;
    candyPrepareStats(capture,mon);candySemantics(capture,FALSE,FALSE);
#ifdef HG_EVOLUTION_CHECK
#ifdef HG_EVOLUTION_LEARN_CHECK
    printf("Evolution fixture evidence: {\"preparedBeforeFirstFieldAction\":true,\"species\":152,\"partyCount\":1,\"moves\":[33,45,115,15],\"initialLevel\":31,\"initialExperience\":24294,\"item\":50,\"quantity\":2,\"tmSlotOffset\":%u,\"flagsBytes\":%u,\"initialFriendship\":%u,\"initialMood\":%u,\"mapSection\":%u,\"eggLocation\":0,\"otNameProfileMatched\":true,\"otNameLength\":%u,\"fixtureUsesExistingStarter\":true,\"originalPersonality\":%u,\"originalOtId\":%u,\"privateSaveMemoryOnly\":true}\n",tmSlotOffset,tmFlagsSize,tmInitialFriendship,tmInitialMood,tmMapSection,nameLength,originalPersonality,originalOtId);
#else
    printf("Evolution fixture evidence: {\"preparedBeforeFirstFieldAction\":true,\"species\":152,\"partyCount\":1,\"moves\":[33,45,0,0],\"initialLevel\":15,\"initialExperience\":2035,\"item\":50,\"quantity\":2,\"tmSlotOffset\":%u,\"flagsBytes\":%u,\"initialFriendship\":%u,\"initialMood\":%u,\"mapSection\":%u,\"eggLocation\":0,\"otNameProfileMatched\":true,\"otNameLength\":%u,\"fixtureUsesExistingStarter\":true,\"originalPersonality\":%u,\"originalOtId\":%u,\"privateSaveMemoryOnly\":true}\n",tmSlotOffset,tmFlagsSize,tmInitialFriendship,tmInitialMood,tmMapSection,nameLength,originalPersonality,originalOtId);
#endif
#else
    printf("Summary Candy fixture evidence: {\"preparedBeforeFirstFieldAction\":true,\"species\":152,\"partyCount\":1,\"moves\":[33,45,115,15],\"item\":50,\"quantity\":2,\"tmSlotOffset\":%u,\"flagsBytes\":%u,\"initialFriendship\":%u,\"initialMood\":%u,\"mapSection\":%u,\"eggLocation\":0,\"otNameProfileMatched\":true,\"otNameLength\":%u,\"fixtureUsesExistingStarter\":true,\"originalPersonality\":%u,\"originalOtId\":%u,\"privateSaveMemoryOnly\":true}\n",tmSlotOffset,tmFlagsSize,tmInitialFriendship,tmInitialMood,tmMapSection,nameLength,originalPersonality,originalOtId);
#endif
}
static BOOL candyContinuePrinter(Capture *capture,const HGSummaryLearnQAState *qa,const HGNativeBagUIState *bag,const HGNativePartyUIState *party) {
    if (qa->owner!=HG_SUMMARY_LEARN_PARTY || !tmPrinterReady(qa)
        || !candyPeripheralReady(qa,bag,party,OWNER_PARTY,HG_PARTY_UI_TOUCH_ACTION,0)) return FALSE;
    if (party->phase!=HG_PARTY_UI_TM_MESSAGE && (party->phase!=HG_PARTY_UI_ITEM_USE
        || (qa->taskState!=0 && qa->taskState!=4) || party->actions[0].textId!=HG_PARTY_UI_TEXT_CANDY_ACK)) return FALSE;
    tmQueue(capture,qa,bag,party,OWNER_PARTY,HG_PARTY_UI_TOUCH_ACTION,0,FALSE);return TRUE;
}
static void CandyLearnPresent(Capture *capture) {
    u32 frame=OS_GetVBlankCount();
    if (!stage) {
        static BOOL started,logged;
        static u32 startFrame,lastFrame,lastMap,lastRevision,lastInputStep,lastInputFrame;
        if (!started) {
            QA_CHECK(tmBootReady(33,TRUE,0,3,0) && !tmBootReady(0,TRUE,0,3,0)
                && !tmBootReady(33,FALSE,0,3,0) && !tmBootReady(33,TRUE,1,3,0)
                && !tmBootReady(33,TRUE,0,2,0) && !tmBootReady(33,TRUE,0,3,1320));
            started=TRUE; startFrame=frame;
        }
        present(capture);
        u32 map=w2c_hgengine_HGNativeFieldMap(capture->game);
        HGSummaryLearnQAState qa;
        // The original field owner publishes this map only after the private observation hook.
        if (map) qa=readLearn(capture);
        if (!logged || map!=lastMap || (map && qa.revision!=lastRevision)
            || capture->inputStep!=lastInputStep || capture->inputFrame!=lastInputFrame || frame-lastFrame>=300) {
            printf("Summary Candy boot evidence: {\"hostFrame\":%u,\"elapsedFrames\":%u,\"map\":%u,\"inputStep\":%u,\"inputFrame\":%u,\"qaObserved\":%s",
                frame,frame-startFrame,map,capture->inputStep,capture->inputFrame,map ? "true" : "false");
            if (map) printf(",\"qaVersion\":%u,\"qaBytes\":%u,\"qaFrame\":%u,\"revision\":%u,\"generation\":%u,\"fieldReady\":%u,\"owner\":%u,\"manager\":%u,\"execState\":%u,\"phase\":%u,\"fadeFinished\":%u,\"mapSection\":%u",
                qa.version,qa.byteSize,qa.frame,qa.revision,qa.generation,qa.fieldReady,qa.owner,qa.manager,qa.execState,qa.phase,qa.fadeFinished,qa.mapSection);
            puts("}");
            logged=TRUE; lastFrame=frame; lastMap=map; lastInputStep=capture->inputStep; lastInputFrame=capture->inputFrame;
            if (map) lastRevision=qa.revision;
        }
        if (frame-startFrame>=3600) { candyFinish(capture,"candy_boot_prerequisite_timeout",FALSE); return; }
        if (!map || !tmBootReady(map,qa.fieldReady,qa.owner,capture->inputStep,capture->inputFrame)) return;
        if (!renderBoth(capture,FALSE,FALSE)) return;
        capture->noInput=TRUE; QA_CHECK(HGNativeSetInput(capture->env,0,FALSE,0,0,FALSE));
        candyPrepare(capture); nextStage(); return;
    }
    QA_CHECK(HGNativeSoundRealtime(capture->env,TRUE));
    HGSummaryLearnQAState qa=readLearn(capture);
    static u32 loggedRevision;
    if (qa.revision!=loggedRevision) {
        printf("Summary Candy state evidence: {\"stage\":%u,\"frame\":%u,\"revision\":%u,\"owner\":%u,\"generation\":%u,\"execState\":%u,\"phase\":%u,\"context\":%u,\"itemId\":%u,\"moveId\":%u,\"partySlot\":%u,\"taskState\":%u,\"taskMatches\":%u,\"printerId\":%u,\"printerActive\":%u,\"printerMatches\":%u,\"printerState\":%u,\"printerWaiting\":%u,\"afterTextState\":%u,\"yesNoActive\":%u,\"yesNoResult\":%u,\"yesNoTimer\":%u,\"yesNoReady\":%u,\"selectedAction\":%u,\"selectedMoveSlot\":%u,\"hmCalls\":%u,\"graphicsReady\":%u,\"fadeFinished\":%u,\"partyBusy\":%u}\n",
            stage,qa.frame,qa.revision,qa.owner,qa.generation,qa.execState,qa.phase,qa.context,qa.itemId,qa.moveId,qa.partySlot,qa.taskState,qa.tmTaskMatches,
            qa.printerId,qa.printerActive,qa.printerMatches,qa.printerState,qa.printerWaiting,qa.afterTextState,
            qa.yesNoActive,qa.yesNoResult,qa.yesNoTimer,qa.yesNoReady,qa.selectedAction,qa.selectedMoveSlot,qa.hmCalls,qa.graphicsReady,qa.fadeFinished,qa.partyBusy);
        loggedRevision=qa.revision;
    }
    if (frame-stageFrame>3600) { candyFinish(capture,"candy_scene_or_input_handshake_timeout",FALSE); return; }
    HGNativePartyUIState party=readParty(capture); HGNativeSummaryUIState summary=readSummary(capture);
    HGNativeBagUIState bag=readTmBag(capture);
    if (!settleInput(capture,&party,&summary)) return;
    if (tmTakeChoice(capture,&qa,&bag,&party)) return;
    if (candyContinuePrinter(capture,&qa,&bag,&party)) return;
    HGNativeSummaryUIInput ignored;
    if (stage==TM_OPEN_BAG) {
        if (!qa.fieldReady || w2c_hgengine_HGNativeFieldMap(capture->game)!=33) return;
        u32 icons=w2c_hgengine_HGNativeFieldMenuIcons(capture->game);
        for (u32 i=0;i<7;++i) if (((icons>>(i*4))&15)==2) {
            pulse(capture,OWNER_FIELD,0,0,(HGNativeSummaryUIInput){.touchPressed=1,.touchX=iconCenters[i][0],.touchY=iconCenters[i][1]}); nextStage(); return;
        }
    } else if (stage==TM_POCKET && bag.active && bag.ready && HGNativeBagUIIsList(&bag)) {
        u32 pocket=HG_BAG_UI_POCKETS;
        for (u32 i=0;i<HG_BAG_UI_POCKETS;++i) if (bag.pockets[i].enabled && bag.pockets[i].pocketId==POCKET_MEDICINE) pocket=i;
        QA_CHECK(pocket<HG_BAG_UI_POCKETS); tmChooseBag(capture,&qa,&bag,&party,HG_BAG_UI_TOUCH_POCKET,pocket,TRUE);
    } else if (stage==TM_ITEM && bag.active && bag.ready && HGNativeBagUIIsList(&bag) && bag.pocketId==POCKET_MEDICINE) {
        u32 row=HG_BAG_UI_ITEMS;
        for (u32 i=0;i<bag.visibleCount;++i) if (bag.items[i].enabled && bag.items[i].itemId==ITEM_RARE_CANDY) row=i;
        QA_CHECK(row<HG_BAG_UI_ITEMS && bag.items[row].quantity==2); tmChooseBag(capture,&qa,&bag,&party,HG_BAG_UI_TOUCH_ITEM,row,TRUE);
    } else if (stage==TM_USE && bag.active && bag.ready && bag.phase==HG_BAG_UI_ITEM_MENU) {
        u32 row=HG_BAG_UI_ACTIONS;
        for (u32 i=0;i<bag.actionCount;++i) if (bag.actions[i].enabled && bag.actions[i].actionId==0) row=i;
        QA_CHECK(row<HG_BAG_UI_ACTIONS && bag.selectedItemId==ITEM_RARE_CANDY); tmChooseBag(capture,&qa,&bag,&party,HG_BAG_UI_TOUCH_ACTION,row,TRUE);
    } else if (stage==5 && qa.owner==HG_SUMMARY_LEARN_PARTY && qa.context==5 && qa.itemId==ITEM_RARE_CANDY
        && qa.phase==HG_PARTY_UI_ITEM_TARGET && party.active && party.ready && !qa.partyBusy && !qa.partySlot
        && HGNativePartyUIIsRareCandy(&party)) {
        candySemantics(capture,FALSE,FALSE);
        tmChooseParty(capture,&qa,&bag,&party,HG_PARTY_UI_TOUCH_MON,0,TRUE);
    } else if ((stage==6 || stage==7) && party.active && party.ready && HGNativePartyUIIsRareCandy(&party)
        && qa.owner==HG_SUMMARY_LEARN_PARTY && qa.context==5 && qa.itemId==ITEM_RARE_CANDY
        && qa.phase==HG_PARTY_UI_ITEM_USE && qa.taskState==stage-5 && qa.graphicsReady && qa.fadeFinished
        && party.actions[0].textId==(stage==6 ? HG_PARTY_UI_TEXT_CANDY_STATS_DIFF : HG_PARTY_UI_TEXT_CANDY_STATS_TOTAL)) {
        tmChooseParty(capture,&qa,&bag,&party,HG_PARTY_UI_TOUCH_ACTION,0,TRUE);
    } else if ((stage==TM_FORGET_YES || stage==TM_STOP_NO || stage==TM_RETRY_YES) && qa.owner==HG_SUMMARY_LEARN_PARTY
        && qa.phase==27 && qa.execState==2 && qa.yesNoReady && qa.graphicsReady && qa.fadeFinished
        && party.active && party.ready && HGNativePartyUIIsRareCandy(&party)) {
        QA_CHECK(qa.context==(stage==TM_FORGET_YES ? 5u : 8u) && qa.itemId==(stage==TM_FORGET_YES ? ITEM_RARE_CANDY : ITEM_NONE) && qa.moveId==MOVE_RAZOR_LEAF);
        if (stage==TM_STOP_NO) {
            QA_CHECK(qa.selectedMoveSlot==4);candySemantics(capture,TRUE,FALSE);tmFifthCancelled=1;candyCancelKeptUpgrade=1;
            if (!candyWrongContext) {
                HGSummaryLearnQAState wrong=qa;wrong.itemId=ITEM_RARE_CANDY;
                QA_CHECK(!candyPeripheralReady(&wrong,&bag,&party,OWNER_PARTY,HG_PARTY_UI_TOUCH_ACTION,1));candyWrongContext=1;
            }
        }
        tmChooseParty(capture,&qa,&bag,&party,HG_PARTY_UI_TOUCH_ACTION,stage==TM_STOP_NO ? 1 : 0,TRUE);
    } else if ((stage==TM_FIRST_SUMMARY || stage==TM_SECOND_SUMMARY) && summary.active && summary.ready
        && summary.phase==HG_SUMMARY_UI_FORGET_MOVE && qa.owner==HG_SUMMARY_LEARN_SUMMARY && qa.execState==2) {
        QA_CHECK(HGNativeSummaryUIIsLearning(&summary) && summary.monDataType==1 && summary.partySlot==0
            && summary.partyCount==1 && summary.moveToLearn==MOVE_RAZOR_LEAF && summary.moveMask==31);
        candySemantics(capture,TRUE,FALSE); QA_CHECK(snapshot(capture,summary)); ++summaryLifetimes;
        if (stage==TM_FIRST_SUMMARY) {
            tmFirstGeneration=qa.generation; tmFirstChoice=(Choice){summary.revision,summary.phase,HG_SUMMARY_UI_INPUT_MOVE,4};
            QA_CHECK(!HGNativeSummaryUIResolveInput(&summary,summary.revision,summary.phase,HG_SUMMARY_UI_INPUT_MOVE,5,&ignored)); ++staleRejected;
            chooseSummary(capture,&summary,HG_SUMMARY_UI_INPUT_MOVE,4);
        } else {
            QA_CHECK(qa.generation>tmFirstGeneration && tmFifthCancelled && summaryLifetimes==2);
            QA_CHECK(!HGNativeSummaryUIResolveInput(&summary,tmFirstChoice.revision,tmFirstChoice.phase,tmFirstChoice.kind,tmFirstChoice.index,&ignored)); ++staleRejected;
            tmContinuedTrying=1; tmHmCallsBefore=qa.hmCalls; chooseSummary(capture,&summary,HG_SUMMARY_UI_INPUT_MOVE,3);
        }
        nextStage();
    } else if (stage==TM_FIFTH_CONFIRM && summary.active && summary.ready && summary.phase==HG_SUMMARY_UI_FORGET_MOVE && summary.moveSlot==4) {
        QA_CHECK(!HGNativeSummaryUIResolveInput(&summary,tmFirstChoice.revision,tmFirstChoice.phase,tmFirstChoice.kind,tmFirstChoice.index,&ignored)); ++staleRejected;
        QA_CHECK(snapshot(capture,summary)); chooseSummary(capture,&summary,HG_SUMMARY_UI_INPUT_CONFIRM,0); nextStage();
    } else if (stage==TM_HM_RESULT && summary.active && summary.ready && summary.phase==HG_SUMMARY_UI_FORGET_MOVE
        && summary.moveSlot==3 && qa.hmCalls==tmHmCallsBefore+1) {
        QA_CHECK(qa.hmOwner==qa.data && qa.hmMoveSlot==3 && summary.moves[3].moveId==MOVE_CUT && qa.hmWarning.pixelBuffer);
        candySemantics(capture,TRUE,FALSE); windowEvidence(capture,summary.phase,"original-hm-warning",0,&qa.hmWarning);
        QA_CHECK(snapshot(capture,summary)); tmHmRejected=1; nextStage();
    } else if ((stage==TM_OLD_MOVE || stage==TM_OLD_AGAIN) && summary.active && summary.ready && summary.phase==HG_SUMMARY_UI_FORGET_MOVE) {
        if (stage==TM_OLD_AGAIN) tmConfirmationBack=1;
        tmOldChoice=(Choice){summary.revision,summary.phase,HG_SUMMARY_UI_INPUT_MOVE,0};
        chooseSummary(capture,&summary,HG_SUMMARY_UI_INPUT_MOVE,0); nextStage();
    } else if ((stage==TM_CONFIRM_BACK || stage==TM_CONFIRM_REPLACE) && summary.active && summary.ready && summary.phase==HG_SUMMARY_UI_FORGET_CONFIRM) {
        QA_CHECK(summary.moveSlot==0 && !HGNativeSummaryUIResolveInput(&summary,tmOldChoice.revision,tmOldChoice.phase,tmOldChoice.kind,tmOldChoice.index,&ignored)); ++staleRejected;
        candySemantics(capture,TRUE,FALSE); QA_CHECK(snapshot(capture,summary));
        chooseSummary(capture,&summary,stage==TM_CONFIRM_BACK ? HG_SUMMARY_UI_INPUT_BACK : HG_SUMMARY_UI_INPUT_CONFIRM,0); nextStage();
    } else if (stage==TM_RETURN_BAG && bag.active && bag.ready && HGNativeBagUIIsList(&bag)) {
        QA_CHECK(!summary.active && !party.active && tmHmRejected);
        tmReplacement=1; candySemantics(capture,TRUE,TRUE);
        u32 mon=w2c_hgengine_Party_GetMonByIndex(capture->game,partyAddress,0);
        tmFinalMove=w2c_hgengine_GetMonData(capture->game,mon,MON_DATA_MOVE1,0);
        tmFinalPpUps=w2c_hgengine_GetMonData(capture->game,mon,MON_DATA_MOVE1_PP_UPS,0);
        tmFinalPp=w2c_hgengine_GetMonData(capture->game,mon,MON_DATA_MOVE1_PP,0);
        tmFinalMaxPp=w2c_hgengine_GetMonData(capture->game,mon,MON_DATA_MOVE1_MAX_PP,0);
        tmFinalFriendship=w2c_hgengine_GetMonData(capture->game,mon,MON_DATA_FRIENDSHIP,0);
        tmFinalMood=w2c_hgengine_GetMonData(capture->game,mon,MON_DATA_MOOD,0);
        tmFinalQuantity=w2c_hgengine_Bag_GetQuantity(capture->game,bagAddress,ITEM_RARE_CANDY,HEAP_ID_FIELD2);
        candyFinalSpecies=w2c_hgengine_GetMonData(capture->game,mon,MON_DATA_SPECIES,0);
        candyFinalLevel=w2c_hgengine_GetMonData(capture->game,mon,MON_DATA_LEVEL,0);
        candyFinalExperience=w2c_hgengine_GetMonData(capture->game,mon,MON_DATA_EXPERIENCE,0);
        for (u32 i=0;i<6;++i) candyFinalStats[i]=w2c_hgengine_GetMonData(capture->game,mon,candyStatAttrs[i],0);
        QA_CHECK(snapshot(capture,summary)); nextStage();
    } else if (stage==TM_CLOSE_BAG && bag.active && bag.ready && HGNativeBagUIIsList(&bag)) {
        tmChooseBag(capture,&qa,&bag,&party,HG_BAG_UI_TOUCH_CANCEL,0,TRUE);
    } else if (stage==TM_FIELD && qa.fieldReady && !bag.active && !party.active && !summary.active) {
        QA_CHECK(tmFixturePrepared && tmFifthCancelled && tmContinuedTrying && tmHmRejected && tmConfirmationBack && tmReplacement
            && summaryLifetimes==2 && staleRejected>=4 && seenPhases==((1u<<8)|(1u<<9))
            && !tmPending.sequence && !tmLeaseQARevision && tmTypedAccepted && tmTypedAccepted==tmTypedReleased
            && tmTypedAccepted==tmTypedSettled && tmPartyReturnAcks && candyStatsMask==3 && candyStatsStale==1
            && candyBusyRejected==2 && candyWrongContext==1 && candyCancelKeptUpgrade);
        QA_CHECK(snapshot(capture,summary)); candyFinish(capture,"passed_original_summary_candy_learning",TRUE);
    }
}
static void candyPrintEvidence(void) {
    const char *truth=casePassed ? "true" : "false";
    printf("Summary Candy learning evidence: {\"status\":\"%s\",\"fixturePrepared\":%u,\"summaryLifetimes\":%u,\"fifthCancelled\":%u,\"continuedTrying\":%u,\"hmRejected\":%u,\"confirmationBack\":%u,\"replacement\":%u,\"cancelKeptUpgrade\":%u,\"statsMask\":%u,\"statsStaleRejected\":%u,\"busyForgedRejected\":%u,\"wrongContinuationRejected\":%u,\"species\":%u,\"level\":%u,\"experience\":%u,\"finalMove\":%u,\"finalPpUps\":%u,\"finalPp\":%u,\"finalMaxPp\":%u,\"initialCandyQuantity\":%u,\"finalCandyQuantity\":%u,\"friendship\":%u,\"mood\":%u,\"staleRejected\":%u,\"typedAccepted\":%u,\"typedRejected\":%u,\"typedReleased\":%u,\"typedSettled\":%u,\"partyReturnAcks\":%u,\"originalIdentityPreserved\":%s,\"otherBagBytesPreserved\":%s,\"flagsMoneyDexPreserved\":%s,\"resetFinished\":true,\"peripheralUiNative\":true,\"stats\":[",
        caseStatus,tmFixturePrepared,summaryLifetimes,tmFifthCancelled,tmContinuedTrying,tmHmRejected,tmConfirmationBack,tmReplacement,
        candyCancelKeptUpgrade,candyStatsMask,candyStatsStale,candyBusyRejected,candyWrongContext,candyFinalSpecies,candyFinalLevel,candyFinalExperience,
        tmFinalMove,tmFinalPpUps,tmFinalPp,tmFinalMaxPp,tmInitialQuantity,tmFinalQuantity,tmFinalFriendship,tmFinalMood,staleRejected,
        tmTypedAccepted,tmTypedRejected,tmTypedReleased,tmTypedSettled,tmPartyReturnAcks,truth,truth,truth);
    for (u32 i=0;i<6;++i) printf("%s%u",i ? "," : "",candyFinalStats[i]);
    fputs("],\"initialStats\":[",stdout);for (u32 i=0;i<6;++i) printf("%s%u",i ? "," : "",candyInitialStats[i]);
    fputs("],\"expectedStats\":[",stdout);for (u32 i=0;i<6;++i) printf("%s%u",i ? "," : "",candyExpectedStats[i]);puts("]}");
}
#endif
#endif

#ifdef HG_EVOLUTION_CHECK
static u32 evoSeen,evoCancelAccepted,evoAckAccepted,evoQueued,evoAccepted,evoRejected,evoReleased,evoSettled;
static u32 evoPendingSequence,evoLeaseSequence,evoSequence,evoFrameCount,evoGeometryFrames,evoOscFrames,evoFirstSerial,evoLastSerial,evoFieldWidth,evoInitialFieldWidth;
static u32 evoSeenBefore,evoCaughtBefore,evoFinalSpecies,evoFinalStats[6],evoFixturePersonality,evoFixtureOtId,evoFinalLevel,evoFinalExperience,evoFinalQuantity;
static u64 evoPhaseMask;
#ifdef HG_EVOLUTION_LEARN_CHECK
static u32 evoLearnAdvance,evoLearnReplaced,evoLearnMenus,evoLearnHmCalls,evoLearnFifth,evoLearnBack,evoLearnHm,evoLearnResume;
#endif
static u8 evoFirstOscPixels[256*192*4];
static Choice evoPendingChoice,evoLeaseChoice;
static HGNativeEvolutionUIState readEvolution(Capture *capture) {
    static u32 previous;
    u32 address=w2c_hgengine_HGNativeEvolutionUIGetState(capture->game);HGNativeEvolutionUIState state;
    memcpy(&state,guest(capture,address,sizeof(state)),sizeof(state));
    QA_CHECK(state.revision>=previous && state.phase<=HG_EVOLUTION_UI_CANCEL_FINISH && !(state.actionMask&~15u));
    QA_CHECK(w2c_hgengine_HGNativeEvolutionUIGetState(capture->game)==address && !memcmp(&state,guest(capture,address,sizeof(state)),sizeof(state)));
    previous=state.revision;return state;
}
static void evolutionLeaseEvent(const char *event,u32 reads,BOOL changed) {
    printf("Evolution typed evidence: {\"event\":\"%s\",\"sequence\":%u,\"revision\":%u,\"phase\":%u,\"kind\":%u,\"index\":%u,\"padReads\":%u,\"ownerChanged\":%u}\n",
        event,evoLeaseSequence,evoLeaseChoice.revision,evoLeaseChoice.phase,evoLeaseChoice.kind,evoLeaseChoice.index,reads,changed);
    if (!strcmp(event,"released")) ++evoReleased;
    else { QA_CHECK(!strcmp(event,"settled"));++evoSettled;evoLeaseSequence=0; }
}
static void evolutionChoose(Capture *capture,const HGNativeEvolutionUIState *state,u32 kind,u32 index) {
    u32 keys,x,y;QA_CHECK(!evoPendingSequence && !leaseActive && HGNativeEvolutionUIResolveInput(state,state->revision,state->phase,kind,index,&keys,&x,&y));
    evoPendingChoice=(Choice){state->revision,state->phase,kind,index};evoPendingSequence=++evoSequence;++evoQueued;
    printf("Evolution typed evidence: {\"event\":\"queued\",\"sequence\":%u,\"revision\":%u,\"phase\":%u,\"kind\":%u,\"index\":%u,\"padReads\":%u,\"keys\":%u,\"touch\":[%u,%u,%u]}\n",
        evoPendingSequence,state->revision,state->phase,kind,index,HGNativeInputPadReads(capture->env),keys,kind==HG_EVOLUTION_UI_INPUT_CHOICE,x,y);
}
static BOOL evolutionTakeChoice(Capture *capture,const HGNativeEvolutionUIState *state) {
    if (!evoPendingSequence) return FALSE;
    if (!state->active || state->summaryActive || state->revision!=evoPendingChoice.revision || state->phase!=evoPendingChoice.phase) {
        printf("Evolution typed evidence: {\"event\":\"rejected\",\"sequence\":%u}\n",evoPendingSequence);++evoRejected;evoPendingSequence=0;return TRUE;
    }
    if (!state->ready) return TRUE; // Begin temporarily retires borrowed Window leases.
    u32 keys,x,y;QA_CHECK(HGNativeEvolutionUIResolveInput(state,evoPendingChoice.revision,evoPendingChoice.phase,evoPendingChoice.kind,evoPendingChoice.index,&keys,&x,&y));
    evoLeaseChoice=evoPendingChoice;evoLeaseSequence=evoPendingSequence;evoPendingSequence=0;
    pulse(capture,OWNER_EVOLUTION,state->revision,state->phase,(HGNativeSummaryUIInput){.keys=keys,.touchPressed=evoLeaseChoice.kind==HG_EVOLUTION_UI_INPUT_CHOICE,.touchX=x,.touchY=y});
    printf("Evolution typed evidence: {\"event\":\"accepted\",\"sequence\":%u,\"revision\":%u,\"phase\":%u,\"kind\":%u,\"index\":%u,\"padReads\":%u,\"keys\":%u,\"touch\":[%u,%u,%u],\"actionMask\":%u,\"flags\":%u}\n",
        evoLeaseSequence,state->revision,state->phase,evoLeaseChoice.kind,evoLeaseChoice.index,leaseReads,keys,evoLeaseChoice.kind==HG_EVOLUTION_UI_INPUT_CHOICE,x,y,state->actionMask,state->flags);
    ++evoAccepted;
    if (evoLeaseChoice.kind==HG_EVOLUTION_UI_INPUT_CANCEL) ++evoCancelAccepted;
    if (evoLeaseChoice.kind==HG_EVOLUTION_UI_INPUT_ACK) ++evoAckAccepted;
    return TRUE;
}
static u32 evolutionRegister(Capture *capture,u32 registerAddress) {
    u32 address=w2c_hgengine_HGNativeRegisterAddress(capture->game,registerAddress,2);u16 value;
    memcpy(&value,guest(capture,address,2),2);return value;
}
static BOOL evolutionFrame(Capture *capture,const HGNativeEvolutionUIState *state) {
    u32 geometry[8],after[8];if (!w2c_hgengine_HGNativeGeometryFrame(capture->game,1,capture->buffer)) return FALSE;
    memcpy(geometry,guest(capture,capture->buffer,sizeof(geometry)),sizeof(geometry));
    if (!geometry[0] || !geometry[1] || !geometry[2] || !w2c_hgengine_HGNativeRender2D(capture->game,0,capture->buffer,256*192*4)) return FALSE;
    const u8 *pixels=guest(capture,capture->buffer,256*192*4);BOOL varied=FALSE;
    for (u32 i=1;i<256*192;++i) if (memcmp(pixels,pixels+i*4,4)) { varied=TRUE;break; }
    if (!varied) return FALSE;
    if (state->phase==HG_EVOLUTION_UI_OSCILLATE) {
        if (evoOscFrames && (geometry[2]==evoLastSerial || !memcmp(evoFirstOscPixels,pixels,sizeof(evoFirstOscPixels)))) return FALSE;
        if (!evoOscFrames) memcpy(evoFirstOscPixels,pixels,sizeof(evoFirstOscPixels));
    }
    char name[96];QA_CHECK(snprintf(name,sizeof(name),"evolution-%02u-phase-%u.rgba",evoFrameCount,state->phase)<sizeof(name));
    saveBytes(capture,name,pixels,256*192*4);
    QA_CHECK(w2c_hgengine_HGNativeGeometryFrame(capture->game,1,capture->buffer));memcpy(after,guest(capture,capture->buffer,sizeof(after)),sizeof(after));
    QA_CHECK(geometry[2]==after[2]);++evoFrameCount;++evoGeometryFrames;
    if (!evoFirstSerial) evoFirstSerial=geometry[2];evoLastSerial=geometry[2];
    if (state->phase==HG_EVOLUTION_UI_OSCILLATE) ++evoOscFrames;
    windowEvidence(capture,state->phase,"evolution-message",evoFrameCount,&state->message);
    printf("Evolution frame evidence: {\"file\":\"%s\",\"width\":256,\"height\":192,\"bytes\":%u,\"phase\":%u,\"revision\":%u,\"frame\":%u,\"vertices\":%u,\"polygons\":%u,\"geometrySerial\":%u,\"geometrySerialAfter\":%u,\"nonuniform\":true,\"blendControl\":%u,\"bg0Control\":%u}\n",
        name,256*192*4,state->phase,state->revision,OS_GetVBlankCount(),geometry[0],geometry[1],geometry[2],after[2],
        evolutionRegister(capture,0x04000050),evolutionRegister(capture,0x04000008));
    return TRUE;
}
static void evolutionStats(Capture *capture,u32 mon,u32 species,u32 level,u32 *out) {
    const u32 bases[2][6]={{45,49,65,49,65,45},{60,62,80,63,80,60}},natureIndex[6]={0,0,1,3,4,2};
    QA_CHECK(species==152 || species==153);u32 nature=candyOriginal[0]%25;
    for (u32 i=0;i<6;++i) {
        u32 value=(2*bases[species-152][i]+candyIV[i]+candyEV[i]/4)*level/100+(i ? 5 : level+10);
        if (i && nature/5!=nature%5) {
            if (natureIndex[i]==nature/5) value=(u16)(value*110)/100;
            else if (natureIndex[i]==nature%5) value=(u16)(value*90)/100;
        }
        QA_CHECK(w2c_hgengine_GetMonData(capture->game,mon,candyStatAttrs[i],0)==value);out[i]=value;
    }
}
static void evolutionSemantics(Capture *capture,BOOL fieldReturned) {
    u32 mon=w2c_hgengine_Party_GetMonByIndex(capture->game,partyAddress,0);
#ifdef HG_EVOLUTION_CANCEL_CHECK
    u32 species=152;
#else
    u32 species=153;
#endif
    u32 experience=w2c_hgengine_GetMonData(capture->game,mon,MON_DATA_EXPERIENCE,0);
    u32 friendship=w2c_hgengine_GetMonData(capture->game,mon,MON_DATA_FRIENDSHIP,0),mood=w2c_hgengine_GetMonData(capture->game,mon,MON_DATA_MOOD,0);
    // Original FollowMonMood_SetMon resets mood when the evolved species resumes following.
    u32 expectedMood=fieldReturned && species==153 ? 0 : 40;
    printf("Evolution outcome observation: {\"fieldReturned\":%s,\"experience\":%u,\"friendship\":%u,\"mood\":%u,\"expectedMood\":%u}\n",fieldReturned ? "true" : "false",experience,friendship,mood,expectedMood);
    QA_CHECK(w2c_hgengine_Party_GetCount(capture->game,partyAddress)==1);
#ifdef HG_EVOLUTION_LEARN_CHECK
    evolutionStats(capture,mon,species,32,evoFinalStats);
#else
    evolutionStats(capture,mon,species,16,evoFinalStats);
#endif
    for (u32 i=0;i<MON_ATTRIBUTES;++i) {
#ifdef HG_EVOLUTION_LEARN_CHECK
        u32 expected=i==2 ? species : i==6 || i==7 ? evoFinalStats[0] : i==8 ? 32 : candyOriginal[i];
        if (evoLearnReplaced) { if (i==10) expected=230;else if (i==14 || i==22) expected=w2c_hgengine_GetMoveMaxPP(capture->game,230,0);else if (i==18) expected=0; }
#else
        u32 expected=i==2 ? species : i==6 || i==7 ? evoFinalStats[0] : i==8 ? 16 : candyOriginal[i];
#endif
        QA_CHECK(w2c_hgengine_GetMonData(capture->game,mon,monAttributes[i],0)==expected);
    }
    const u32 order[6]={0,1,2,4,5,3};
    for (u32 i=0;i<6;++i) QA_CHECK(w2c_hgengine_GetMonData(capture->game,mon,MON_DATA_HP_IV+order[i],0)==candyIV[i]
        && w2c_hgengine_GetMonData(capture->game,mon,MON_DATA_HP_EV+order[i],0)==candyEV[i]);
#ifdef HG_EVOLUTION_LEARN_CHECK
    QA_CHECK(experience==27021 && friendship==75 && mood==expectedMood);
    tmFinalMove=w2c_hgengine_GetMonData(capture->game,mon,MON_DATA_MOVE1,0);
    tmFinalPp=w2c_hgengine_GetMonData(capture->game,mon,MON_DATA_MOVE1_PP,0);
    tmFinalMaxPp=w2c_hgengine_GetMonData(capture->game,mon,MON_DATA_MOVE1_MAX_PP,0);
    tmFinalPpUps=w2c_hgengine_GetMonData(capture->game,mon,MON_DATA_MOVE1_PP_UPS,0);
#else
    QA_CHECK(experience==2535 && friendship==75 && mood==expectedMood);
#endif
    QA_CHECK(!memcmp(tmOriginalFlags,guest(capture,flagsAddress,tmFlagsSize),tmFlagsSize)
        && w2c_hgengine_PlayerProfile_GetMoney(capture->game,profileAddress)==money && w2c_hgengine_Pokedex_IsEnabled(capture->game,dexAddress)==dexEnabled);
    u8 *expected=malloc(bagSize);QA_CHECK(expected);memcpy(expected,originalBag,bagSize);u16 one=1;memcpy(expected+tmSlotOffset+2,&one,2);
    QA_CHECK(!memcmp(expected,guest(capture,bagAddress,bagSize),bagSize));free(expected);
    QA_CHECK(w2c_hgengine_Bag_GetQuantity(capture->game,bagAddress,ITEM_RARE_CANDY,HEAP_ID_FIELD2)==1);
    u32 seen=w2c_hgengine_Pokedex_CheckMonSeenFlag(capture->game,dexAddress,153),caught=w2c_hgengine_Pokedex_CheckMonCaughtFlag(capture->game,dexAddress,153);
    QA_CHECK(seen==(species==153) && caught==(species==153)
        && w2c_hgengine_Pokedex_CountNationalDexSeen(capture->game,dexAddress)==dexSeen+(species==153)
        && w2c_hgengine_Pokedex_CountNationalDexOwned(capture->game,dexAddress)==dexCaught+(species==153));
    evoFinalSpecies=species;
#ifdef HG_EVOLUTION_LEARN_CHECK
    printf("Evolution semantic evidence: {\"species\":%u,\"level\":32,\"experience\":27021,\"quantity\":1,\"friendship\":75,\"mood\":%u,\"fieldReturned\":%s,\"personality\":%u,\"otId\":%u,\"hp\":%u,\"maxHp\":%u,\"bayleefSeen\":%u,\"bayleefCaught\":%u,\"dexSeenDelta\":%u,\"dexCaughtDelta\":%u,\"otherBagBytesPreserved\":true,\"flagsMoneyPreserved\":true,\"ivEvPreserved\":true,\"stats\":[%u,%u,%u,%u,%u,%u]}\n",
#else
    printf("Evolution semantic evidence: {\"species\":%u,\"level\":16,\"experience\":2535,\"quantity\":1,\"friendship\":75,\"mood\":%u,\"fieldReturned\":%s,\"personality\":%u,\"otId\":%u,\"hp\":%u,\"maxHp\":%u,\"bayleefSeen\":%u,\"bayleefCaught\":%u,\"dexSeenDelta\":%u,\"dexCaughtDelta\":%u,\"otherBagBytesPreserved\":true,\"flagsMoneyPreserved\":true,\"ivEvPreserved\":true,\"stats\":[%u,%u,%u,%u,%u,%u]}\n",
#endif
        species,mood,fieldReturned ? "true" : "false",w2c_hgengine_GetMonData(capture->game,mon,MON_DATA_PERSONALITY,0),w2c_hgengine_GetMonData(capture->game,mon,MON_DATA_OT_ID,0),evoFinalStats[0],evoFinalStats[0],
        seen,caught,species==153,species==153,evoFinalStats[0],evoFinalStats[1],evoFinalStats[2],evoFinalStats[3],evoFinalStats[4],evoFinalStats[5]);
}
static void evolutionFinish(Capture *capture,const char *status,BOOL passed) {
    QA_CHECK(HGNativeSetInput(capture->env,0,FALSE,0,0,FALSE));
    if (passed) evolutionSemantics(capture,TRUE);
    if (tmFixturePrepared) {
        u32 mon=w2c_hgengine_Party_GetMonByIndex(capture->game,partyAddress,0);
        evoFinalSpecies=w2c_hgengine_GetMonData(capture->game,mon,MON_DATA_SPECIES,0);
        evoFinalLevel=w2c_hgengine_GetMonData(capture->game,mon,MON_DATA_LEVEL,0);
        evoFinalExperience=w2c_hgengine_GetMonData(capture->game,mon,MON_DATA_EXPERIENCE,0);
        evoFinalQuantity=w2c_hgengine_Bag_GetQuantity(capture->game,bagAddress,ITEM_RARE_CANDY,HEAP_ID_FIELD2);
    }
    caseStatus=status;casePassed=passed;resetExpected=TRUE;w2c_env_HGNativeResetControl(capture->env,HG_RESET_REQUEST,0);QA_CHECK(FALSE);
}
static void EvolutionPresent(Capture *capture) {
    if (!stage) {
        CandyLearnPresent(capture);
        if (stage==1) {
            evoFixturePersonality=candyOriginal[0];evoFixtureOtId=candyOriginal[1];
            evoSeenBefore=w2c_hgengine_Pokedex_CheckMonSeenFlag(capture->game,dexAddress,153);
            evoCaughtBefore=w2c_hgengine_Pokedex_CheckMonCaughtFlag(capture->game,dexAddress,153);
#ifdef HG_EVOLUTION_LEARN_CHECK
            QA_CHECK(!evoSeenBefore && !evoCaughtBefore && candyOriginal[8]==31 && candyInitialExp==24294
                && candyOriginal[10]==MOVE_TACKLE && candyOriginal[11]==MOVE_GROWL && candyOriginal[12]==MOVE_REFLECT && candyOriginal[13]==MOVE_CUT);
#else
            QA_CHECK(!evoSeenBefore && !evoCaughtBefore && candyOriginal[8]==15 && candyInitialExp==2035
                && candyOriginal[10]==MOVE_TACKLE && candyOriginal[11]==MOVE_GROWL && !candyOriginal[12] && !candyOriginal[13]);
#endif
        }
        return;
    }
    u32 frame=OS_GetVBlankCount();
    if (frame-stageFrame>(stage==8 ? 7200u : 3600u)) { evolutionFinish(capture,"evolution_scene_timeout",FALSE);return; }
    QA_CHECK(HGNativeSoundRealtime(capture->env,TRUE));
    HGSummaryLearnQAState qa=readLearn(capture);HGNativePartyUIState party=readParty(capture);
    HGNativeSummaryUIState summary=readSummary(capture);HGNativeBagUIState bag=readTmBag(capture);HGNativeEvolutionUIState evo=readEvolution(capture);
    BOOL inputIdle=settleInput(capture,&party,&summary);
    // This original Lv16 learnset has no move. Do not synthesize a Summary/menu branch.
    QA_CHECK(!summary.active && !evo.summaryActive && !evo.moveToLearn);
    if (evo.active && (!evo.species || !evo.newSpecies)) { QA_CHECK(!evo.ready && !evo.actionMask);return; }
    if (evo.active) {
        static u32 lastRevision,lastPhase=UINT32_MAX;
        QA_CHECK(evo.species==152 && evo.newSpecies==153 && (evo.flags&HG_EVOLUTION_UI_CAN_CANCEL));
        evoSeen=1;evoPhaseMask|=(u64)1<<evo.phase;
        if (evo.revision!=lastRevision || evo.phase!=lastPhase) {
            printf("Evolution state evidence: {\"frame\":%u,\"revision\":%u,\"phase\":%u,\"active\":%u,\"ready\":%u,\"summaryActive\":%u,\"flags\":%u,\"species\":%u,\"newSpecies\":%u,\"moveToLearn\":%u,\"actionMask\":%u}\n",
                frame,evo.revision,evo.phase,evo.active,evo.ready,evo.summaryActive,evo.flags,evo.species,evo.newSpecies,evo.moveToLearn,evo.actionMask);
            lastRevision=evo.revision;lastPhase=evo.phase;
        }
        if (evo.phase==HG_EVOLUTION_UI_OSCILLATE && evoOscFrames<2 && (!evoOscFrames || frame-stageFrame>30)) (void)evolutionFrame(capture,&evo);
        else if ((evo.phase==12 || evo.phase==44) && evoFrameCount<4 && evo.ready) (void)evolutionFrame(capture,&evo);
    }
    if (!inputIdle) return;
    if (evolutionTakeChoice(capture,&evo) || tmTakeChoice(capture,&qa,&bag,&party)) return;
    if (evo.active) {
        u32 k,x,y;
#ifdef HG_EVOLUTION_CANCEL_CHECK
        if (!evoCancelAccepted && evo.phase==HG_EVOLUTION_UI_OSCILLATE && evoOscFrames==2
            && HGNativeEvolutionUIResolveInput(&evo,evo.revision,evo.phase,HG_EVOLUTION_UI_INPUT_CANCEL,0,&k,&x,&y)) {
            QA_CHECK(k==2 && !x && !y);
            QA_CHECK(!HGNativeEvolutionUIResolveInput(&evo,evo.revision-1,evo.phase,HG_EVOLUTION_UI_INPUT_CANCEL,0,&k,&x,&y));++staleRejected;
            HGNativeEvolutionUIState forged=evo;forged.flags&=~HG_EVOLUTION_UI_CAN_CANCEL;
            QA_CHECK(!HGNativeEvolutionUIResolveInput(&forged,forged.revision,forged.phase,HG_EVOLUTION_UI_INPUT_CANCEL,0,&k,&x,&y));
            evolutionChoose(capture,&evo,HG_EVOLUTION_UI_INPUT_CANCEL,0);return;
        }
#endif
        if (HGNativeEvolutionUIResolveInput(&evo,evo.revision,evo.phase,HG_EVOLUTION_UI_INPUT_ACK,0,&k,&x,&y)) {
            QA_CHECK(k==1 && !x && !y);evolutionChoose(capture,&evo,HG_EVOLUTION_UI_INPUT_ACK,0);return;
        }
        return;
    }
    if (candyContinuePrinter(capture,&qa,&bag,&party)) return;
    if (stage==1 && qa.fieldReady) {
        if (!evoInitialFieldWidth) {
            QA_CHECK(w2c_hgengine_HGNativeSetViewWidth(capture->game,384));
            if (!w2c_hgengine_HGNativeRender2DView(capture->game,0,384,capture->buffer,384*192*4)) return;
            const u8 *pixels=guest(capture,capture->buffer,384*192*4);BOOL varied=FALSE;
            for (u32 i=1;i<384*192;++i) if (memcmp(pixels,pixels+i*4,4)) {varied=TRUE;break;}if (!varied) return;
            saveBytes(capture,"evolution-field-before.rgba",pixels,384*192*4);evoInitialFieldWidth=384;
        }
        QA_CHECK(w2c_hgengine_HGNativeSetViewWidth(capture->game,256));
        u32 icons=w2c_hgengine_HGNativeFieldMenuIcons(capture->game);
        for (u32 i=0;i<7;++i) if (((icons>>(i*4))&15)==2) {
            pulse(capture,OWNER_FIELD,0,0,(HGNativeSummaryUIInput){.touchPressed=1,.touchX=iconCenters[i][0],.touchY=iconCenters[i][1]});nextStage();return;
        }
    } else if (stage==2 && bag.active && bag.ready && HGNativeBagUIIsList(&bag)) {
        for (u32 i=0;i<HG_BAG_UI_POCKETS;++i) if (bag.pockets[i].enabled && bag.pockets[i].pocketId==POCKET_MEDICINE) {tmChooseBag(capture,&qa,&bag,&party,HG_BAG_UI_TOUCH_POCKET,i,TRUE);return;}
    } else if (stage==3 && bag.active && bag.ready && HGNativeBagUIIsList(&bag) && bag.pocketId==POCKET_MEDICINE) {
        for (u32 i=0;i<bag.visibleCount;++i) if (bag.items[i].enabled && bag.items[i].itemId==ITEM_RARE_CANDY && bag.items[i].quantity==2) {tmChooseBag(capture,&qa,&bag,&party,HG_BAG_UI_TOUCH_ITEM,i,TRUE);return;}
    } else if (stage==4 && bag.active && bag.ready && bag.phase==HG_BAG_UI_ITEM_MENU) {
        for (u32 i=0;i<bag.actionCount;++i) if (bag.actions[i].enabled && !bag.actions[i].actionId) {tmChooseBag(capture,&qa,&bag,&party,HG_BAG_UI_TOUCH_ACTION,i,TRUE);return;}
    } else if (stage==5 && qa.owner==HG_SUMMARY_LEARN_PARTY && qa.context==5 && qa.itemId==ITEM_RARE_CANDY && qa.phase==HG_PARTY_UI_ITEM_TARGET && !qa.partyBusy) {
        tmChooseParty(capture,&qa,&bag,&party,HG_PARTY_UI_TOUCH_MON,0,TRUE);
    } else if ((stage==6 || stage==7) && party.active && party.ready && HGNativePartyUIIsRareCandy(&party)
        && qa.context==5 && qa.phase==HG_PARTY_UI_ITEM_USE && qa.taskState==stage-5
        && party.actions[0].textId==(stage==6 ? HG_PARTY_UI_TEXT_CANDY_STATS_DIFF : HG_PARTY_UI_TEXT_CANDY_STATS_TOTAL)) {
        tmChooseParty(capture,&qa,&bag,&party,HG_PARTY_UI_TOUCH_ACTION,0,TRUE);
    } else if (stage==8 && evoSeen && bag.active && bag.ready && HGNativeBagUIIsList(&bag)) {
        QA_CHECK(candyStatsMask==3 && evoOscFrames && evoAckAccepted==1 && !evoPendingSequence && !evoLeaseSequence
            && evoQueued==evoAccepted && evoAccepted==evoReleased && evoAccepted==evoSettled && !evoRejected);
#ifdef HG_EVOLUTION_CANCEL_CHECK
        QA_CHECK(evoCancelAccepted==1 && (evoPhaseMask&((u64)31<<41)));
#else
        QA_CHECK(!evoCancelAccepted && evoOscFrames==2 && evoFirstSerial!=evoLastSerial && (evoPhaseMask&((u64)1<<12)));
#endif
        evolutionSemantics(capture,FALSE);nextStage();
    } else if (stage==9 && bag.active && bag.ready && HGNativeBagUIIsList(&bag)) {
        tmChooseBag(capture,&qa,&bag,&party,HG_BAG_UI_TOUCH_CANCEL,0,TRUE);
    } else if (stage==10 && qa.fieldReady && !qa.owner && !bag.active && !party.active) {
        QA_CHECK(w2c_hgengine_HGNativeSetViewWidth(capture->game,384));
        if (!w2c_hgengine_HGNativeRender2DView(capture->game,0,384,capture->buffer,384*192*4)) return;
        const u8 *pixels=guest(capture,capture->buffer,384*192*4);BOOL varied=FALSE;
        for (u32 i=1;i<384*192;++i) if (memcmp(pixels,pixels+i*4,4)) {varied=TRUE;break;}if (!varied) return;
        saveBytes(capture,"evolution-field-return.rgba",pixels,384*192*4);evoFieldWidth=384;QA_CHECK(evoInitialFieldWidth==384);
        QA_CHECK(!leaseActive && !tmPending.sequence && !tmLeaseQARevision && tmTypedAccepted==tmTypedReleased && tmTypedAccepted==tmTypedSettled && !tmTypedRejected);
#ifdef HG_EVOLUTION_CANCEL_CHECK
        evolutionFinish(capture,"passed_original_evolution_cancel",TRUE);
#else
        evolutionFinish(capture,"passed_original_evolution_success",TRUE);
#endif
    }
}
static void evolutionPrintEvidence(void) {
#ifdef HG_EVOLUTION_CANCEL_CHECK
    const char *cancel="true";
#else
    const char *cancel="false";
#endif
    printf("Evolution result evidence: {\"status\":\"%s\",\"cancelCase\":%s,\"fixturePrepared\":%u,\"initialLevel\":%u,\"initialExperience\":%u,\"level\":%u,\"experience\":%u,\"species\":%u,\"quantity\":%u,\"personality\":%u,\"otId\":%u,\"cancelAccepted\":%u,\"ackAccepted\":%u,\"queued\":%u,\"accepted\":%u,\"rejected\":%u,\"released\":%u,\"settled\":%u,\"geometryFrames\":%u,\"oscillateFrames\":%u,\"firstGeometrySerial\":%u,\"lastGeometrySerial\":%u,\"initialFieldViewWidth\":%u,\"fieldViewWidth\":%u,\"outerTypedAccepted\":%u,\"outerTypedReleased\":%u,\"outerTypedSettled\":%u,\"outerTypedRejected\":%u,\"statsMask\":%u,\"summaryLifetimes\":0,\"choiceMenus\":0,\"particleContributionSeparated\":false,\"resetFinished\":true,\"privateSaveWrites\":false}\n",
        caseStatus,cancel,tmFixturePrepared,candyOriginal[8],candyInitialExp,evoFinalLevel,evoFinalExperience,evoFinalSpecies,evoFinalQuantity,evoFixturePersonality,evoFixtureOtId,evoCancelAccepted,evoAckAccepted,evoQueued,evoAccepted,evoRejected,evoReleased,evoSettled,
        evoGeometryFrames,evoOscFrames,evoFirstSerial,evoLastSerial,evoInitialFieldWidth,evoFieldWidth,tmTypedAccepted,tmTypedReleased,tmTypedSettled,tmTypedRejected,candyStatsMask);
}
#endif


#ifdef HG_EVOLUTION_LEARN_CHECK
static Choice evoLearnOldMenu,evoLearnOldSummary;
static void evolutionLearningContext(Capture *capture,const HGNativeEvolutionUIState *evo,const char *event) {
    BOOL rendered=w2c_hgengine_HGNativeRender2D(capture->game,0,capture->buffer,256*192*4);
    u32 geometry[8]={0};BOOL has=w2c_hgengine_HGNativeGeometryFrame(capture->game,1,capture->buffer);
    if (has) memcpy(geometry,guest(capture,capture->buffer,sizeof(geometry)),sizeof(geometry));
    printf("Evolution resume evidence: {\"event\":\"%s\",\"stage\":%u,\"frame\":%u,\"phase\":%u,\"revision\":%u,\"summaryActive\":%u,\"renderWidth\":256,\"rendered\":%u,\"geometryAvailable\":%u,\"vertices\":%u,\"polygons\":%u,\"geometrySerial\":%u,\"blendControl\":%u,\"bg0Control\":%u}\n",
        event,stage,OS_GetVBlankCount(),evo->phase,evo->revision,evo->summaryActive,rendered,has,geometry[0],geometry[1],geometry[2],
        evolutionRegister(capture,0x04000050),evolutionRegister(capture,0x04000008));
}
static BOOL evolutionLearningSummary(Capture *capture,const HGNativeEvolutionUIState *evo,const HGSummaryLearnQAState *qa,
    const HGNativeSummaryUIState *state,const char *event) {
    QA_CHECK(evo->active && evo->phase==HG_EVOLUTION_UI_RUN_SUMMARY && evo->summaryActive && !evo->ready && !evo->actionMask
        && !evo->message.pixelBuffer && !evo->choices[0].pixelBuffer && !evo->choices[1].pixelBuffer
        && !qa->owner && !qa->hmWarning.pixelBuffer);
    QA_CHECK(state->active && state->ready && HGNativeSummaryUIIsLearning(state) && !state->monDataType
        && state->partyCount==1 && !state->partySlot && state->species==153 && state->level==32 && state->moveToLearn==230
        && state->page==1 && (state->phase==8 || state->phase==9) && state->moveMask==(state->phase==8 ? 31u : 0u));
    u32 mon=w2c_hgengine_Party_GetMonByIndex(capture->game,partyAddress,0);
    for (u32 i=0;i<4;++i) QA_CHECK(state->moves[i].moveId==candyOriginal[10+i]
        && state->moves[i].pp==w2c_hgengine_GetMonData(capture->game,mon,MON_DATA_MOVE1_PP+i,0)
        && state->moves[i].maxPP==w2c_hgengine_GetMonData(capture->game,mon,MON_DATA_MOVE1_MAX_PP+i,0));
    if (!renderBoth(capture,TRUE,TRUE)) return FALSE;
    char file[80];QA_CHECK(snprintf(file,sizeof(file),"evolution-summary-%02u.bin",snapshotStages)<sizeof(file));
    saveBytes(capture,file,state,sizeof(*state));
    windowEvidence(capture,state->phase,"evolution-summary-back",snapshotStages,&state->backText);
    for (u32 i=0;i<4;++i) windowEvidence(capture,state->phase,"evolution-summary-move",i,&state->moves[i].text);
    HGNativeSummaryUIState after=readSummary(capture);QA_CHECK(!memcmp(state,&after,sizeof(after)));
    seenPhases|=1u<<state->phase;
    printf("Evolution Summary evidence: {\"event\":\"%s\",\"stage\":%u,\"snapshot\":%u,\"file\":\"%s\",\"frame\":%u,\"revision\":%u,\"phase\":%u,\"moveSlot\":%u,\"moveMask\":%u,\"mode\":%u,\"monDataType\":%u,\"partyCount\":%u,\"partySlot\":%u,\"species\":%u,\"level\":%u,\"candidate\":%u,\"parentPhase\":%u,\"parentRevision\":%u,\"parentSummaryActive\":%u,\"qaOwner\":%u,\"hmCalls\":%u,\"hmMoveSlot\":%u,\"hmWarningDescriptor\":%u}\n",
        event,stage,snapshotStages,file,OS_GetVBlankCount(),state->revision,state->phase,state->moveSlot,state->moveMask,state->mode,state->monDataType,state->partyCount,state->partySlot,
        state->species,state->level,state->moveToLearn,evo->phase,evo->revision,evo->summaryActive,qa->owner,qa->hmCalls,qa->hmMoveSlot,qa->hmWarning.pixelBuffer);
    ++snapshotStages;return TRUE;
}
static void evolutionLearningMenu(Capture *capture,const HGNativeEvolutionUIState *evo,u32 index) {
    u32 keys,x,y;QA_CHECK(evo->ready && evo->moveToLearn==230
        && HGNativeEvolutionUIResolveInput(evo,evo->revision,evo->phase,HG_EVOLUTION_UI_INPUT_CHOICE,index,&keys,&x,&y));
    QA_CHECK(!keys && x==127 && y==(index ? 119u : 71u));
    if (!evoLearnMenus) evoLearnOldMenu=(Choice){evo->revision,evo->phase,HG_EVOLUTION_UI_INPUT_CHOICE,index};
    else if (evo->phase==evoLearnOldMenu.phase) {
        QA_CHECK(!HGNativeEvolutionUIResolveInput(evo,evoLearnOldMenu.revision,evoLearnOldMenu.phase,evoLearnOldMenu.kind,evoLearnOldMenu.index,&keys,&x,&y));++staleRejected;
    }
    windowEvidence(capture,evo->phase,"evolution-choice-top",evoLearnMenus,&evo->choices[0]);
    windowEvidence(capture,evo->phase,"evolution-choice-bottom",evoLearnMenus,&evo->choices[1]);
    evolutionLearningContext(capture,evo,"menu");
    ++evoLearnMenus;evolutionChoose(capture,evo,HG_EVOLUTION_UI_INPUT_CHOICE,index);evoLearnAdvance=1;
}
static void EvolutionLearningPresent(Capture *capture) {
    if (stage<8) { EvolutionPresent(capture);return; }
    u32 frame=OS_GetVBlankCount();
    if (frame-stageFrame>7200u) { evolutionFinish(capture,"evolution_learning_timeout",FALSE);return; }
    QA_CHECK(HGNativeSoundRealtime(capture->env,TRUE));
    HGSummaryLearnQAState qa=readLearn(capture);HGNativePartyUIState party=readParty(capture);
    HGNativeSummaryUIState summary=readSummary(capture);HGNativeBagUIState bag=readTmBag(capture);
    HGNativeEvolutionUIState evo=readEvolution(capture);
    BOOL idle=settleInput(capture,&party,&summary);
    if (evo.active && evo.species && evo.newSpecies) {
        static u32 lastRevision,lastPhase=UINT32_MAX;
        QA_CHECK(evo.species==152 && evo.newSpecies==153 && (evo.flags&HG_EVOLUTION_UI_CAN_CANCEL));
        evoSeen=1;evoPhaseMask|=(u64)1<<evo.phase;
        if (evo.revision!=lastRevision || evo.phase!=lastPhase) {
            printf("Evolution state evidence: {\"frame\":%u,\"revision\":%u,\"phase\":%u,\"active\":%u,\"ready\":%u,\"summaryActive\":%u,\"flags\":%u,\"species\":%u,\"newSpecies\":%u,\"moveToLearn\":%u,\"actionMask\":%u}\n",
                frame,evo.revision,evo.phase,evo.active,evo.ready,evo.summaryActive,evo.flags,evo.species,evo.newSpecies,evo.moveToLearn,evo.actionMask);
            lastRevision=evo.revision;lastPhase=evo.phase;
        }
        if (evo.phase==8 && evoOscFrames<2) (void)evolutionFrame(capture,&evo);
    }
    if (!idle) return;
    if (evoPendingSequence) {
        u32 accepted=evoAccepted;BOOL handled=evolutionTakeChoice(capture,&evo);
        if (evoLearnAdvance && evoAccepted!=accepted) { evoLearnAdvance=0;nextStage(); }
        if (handled) return;
    }
    if (tmTakeChoice(capture,&qa,&bag,&party)) return;
    if (evo.active && !evo.summaryActive) {
        u32 keys,x,y;
        if (HGNativeEvolutionUIResolveInput(&evo,evo.revision,evo.phase,HG_EVOLUTION_UI_INPUT_ACK,0,&keys,&x,&y)) {
            if (stage==20 && evo.phase==37) {
                if (!evoLearnReplaced) evolutionLearningContext(capture,&evo,"after-summary");
                evoLearnReplaced=1;evolutionSemantics(capture,FALSE);
                QA_CHECK(evoLearnFifth && evoLearnBack && evoLearnHm && summaryLifetimes==2);
            }
            evolutionChoose(capture,&evo,HG_EVOLUTION_UI_INPUT_ACK,0);return;
        }
        if ((stage==8 || stage==10 || stage==14) && evo.phase==21 && evo.ready)
            evolutionLearningMenu(capture,&evo,stage==8 ? 1 : 0);
        else if ((stage==9 || stage==13) && evo.phase==35 && evo.ready) {
            if (stage==13) { evoLearnFifth=1;QA_CHECK(!summary.active); }
            evolutionLearningMenu(capture,&evo,1);
        }
        return;
    }
    if (summary.active && summary.ready && evo.summaryActive) {
        if (stage==11 && summary.phase==8) {
            QA_CHECK(!summaryLifetimes);++summaryLifetimes;
            if (!evolutionLearningSummary(capture,&evo,&qa,&summary,"first-list")) {--summaryLifetimes;return;}
            evoLearnOldSummary=(Choice){summary.revision,summary.phase,HG_SUMMARY_UI_INPUT_MOVE,4};
            evolutionLearningContext(capture,&evo,"summary-first");
            chooseSummary(capture,&summary,HG_SUMMARY_UI_INPUT_MOVE,4);nextStage();
        } else if (stage==12 && summary.phase==8 && summary.moveSlot==4) {
            if (!evolutionLearningSummary(capture,&evo,&qa,&summary,"fifth-cancel")) return;
            chooseSummary(capture,&summary,HG_SUMMARY_UI_INPUT_CONFIRM,0);nextStage();
        } else if (stage==15 && summary.phase==8) {
            HGNativeSummaryUIInput out;QA_CHECK(summaryLifetimes==1
                && !HGNativeSummaryUIResolveInput(&summary,evoLearnOldSummary.revision,evoLearnOldSummary.phase,evoLearnOldSummary.kind,evoLearnOldSummary.index,&out));
            ++staleRejected;++summaryLifetimes;
            if (!evolutionLearningSummary(capture,&evo,&qa,&summary,"second-list")) {--summaryLifetimes;return;}
            evolutionLearningContext(capture,&evo,"summary-second");evoLearnHmCalls=qa.hmCalls;
            chooseSummary(capture,&summary,HG_SUMMARY_UI_INPUT_MOVE,3);nextStage();
        } else if (stage==16 && summary.phase==8 && summary.moveSlot==3 && qa.hmCalls==evoLearnHmCalls+1) {
            QA_CHECK(qa.hmMoveSlot==3 && !qa.hmWarning.pixelBuffer);
            if (!evolutionLearningSummary(capture,&evo,&qa,&summary,"hm-rejection")) return;
            evoLearnHm=1;chooseSummary(capture,&summary,HG_SUMMARY_UI_INPUT_MOVE,0);nextStage();
        } else if (stage==17 && summary.phase==9 && summary.moveSlot==0) {
            if (!evolutionLearningSummary(capture,&evo,&qa,&summary,"confirmation-back")) return;
            chooseSummary(capture,&summary,HG_SUMMARY_UI_INPUT_BACK,0);nextStage();
        } else if (stage==18 && summary.phase==8 && summary.moveSlot==0) {
            if (!evolutionLearningSummary(capture,&evo,&qa,&summary,"reopen-old-slot")) return;
            evoLearnBack=1;chooseSummary(capture,&summary,HG_SUMMARY_UI_INPUT_MOVE,0);nextStage();
        } else if (stage==19 && summary.phase==9 && summary.moveSlot==0) {
            if (!evolutionLearningSummary(capture,&evo,&qa,&summary,"replace-confirm")) return;
            chooseSummary(capture,&summary,HG_SUMMARY_UI_INPUT_CONFIRM,0);nextStage();
        }
        return;
    }
    if (stage==20 && !evo.active && !summary.active && bag.active && bag.ready && HGNativeBagUIIsList(&bag)) {
        QA_CHECK(evoLearnReplaced && evoLearnMenus==5 && !evoPendingSequence && !evoLeaseSequence && !evoRejected
            && evoQueued==evoAccepted && evoAccepted==evoReleased && evoAccepted==evoSettled
            && evoOscFrames==2 && candyStatsMask==3 && !evoCancelAccepted);
        evolutionSemantics(capture,FALSE);evolutionLearningContext(capture,&evo,"resumed-bag");evoLearnResume=1;nextStage();
    } else if (stage==21 && bag.active && bag.ready && HGNativeBagUIIsList(&bag))
        tmChooseBag(capture,&qa,&bag,&party,HG_BAG_UI_TOUCH_CANCEL,0,TRUE);
    else if (stage==22 && qa.fieldReady && !qa.owner && !bag.active && !party.active && !summary.active && !evo.active) {
        QA_CHECK(w2c_hgengine_HGNativeSetViewWidth(capture->game,384));
        if (!w2c_hgengine_HGNativeRender2DView(capture->game,0,384,capture->buffer,384*192*4)) return;
        const u8 *pixels=guest(capture,capture->buffer,384*192*4);BOOL varied=FALSE;
        for (u32 i=1;i<384*192;++i) if (memcmp(pixels,pixels+i*4,4)) {varied=TRUE;break;}if (!varied) return;
        saveBytes(capture,"evolution-field-return.rgba",pixels,384*192*4);evoFieldWidth=384;
        QA_CHECK(evoInitialFieldWidth==384 && evoLearnResume && !leaseActive && !tmPending.sequence && !tmLeaseQARevision
            && tmTypedAccepted==tmTypedReleased && tmTypedAccepted==tmTypedSettled && !tmTypedRejected);
        evolutionFinish(capture,"passed_original_evolution_learning",TRUE);
    }
}
static void evolutionLearningPrintEvidence(void) {
    printf("Evolution learning evidence: {\"status\":\"%s\",\"fixturePrepared\":%u,\"initialLevel\":31,\"initialExperience\":24294,\"level\":%u,\"experience\":%u,\"species\":%u,\"candidate\":230,\"quantity\":%u,\"personality\":%u,\"otId\":%u,\"summaryLifetimes\":%u,\"choiceMenus\":%u,\"fifthCancelled\":%u,\"confirmationBack\":%u,\"hmRejected\":%u,\"replacement\":%u,\"resumed\":%u,\"queued\":%u,\"accepted\":%u,\"rejected\":%u,\"released\":%u,\"settled\":%u,\"ackAccepted\":%u,\"cancelAccepted\":%u,\"outerTypedAccepted\":%u,\"outerTypedReleased\":%u,\"outerTypedSettled\":%u,\"outerTypedRejected\":%u,\"statsMask\":%u,\"oscillateFrames\":%u,\"geometryFrames\":%u,\"initialFieldViewWidth\":%u,\"fieldViewWidth\":%u,\"finalMove\":%u,\"finalPp\":%u,\"finalMaxPp\":%u,\"finalPpUps\":%u,\"particleContributionSeparated\":false,\"resetFinished\":true,\"privateSaveWrites\":false}\n",
        caseStatus,tmFixturePrepared,evoFinalLevel,evoFinalExperience,evoFinalSpecies,evoFinalQuantity,evoFixturePersonality,evoFixtureOtId,summaryLifetimes,evoLearnMenus,
        evoLearnFifth,evoLearnBack,evoLearnHm,evoLearnReplaced,evoLearnResume,evoQueued,evoAccepted,evoRejected,evoReleased,evoSettled,evoAckAccepted,evoCancelAccepted,
        tmTypedAccepted,tmTypedReleased,tmTypedSettled,tmTypedRejected,candyStatsMask,evoOscFrames,evoGeometryFrames,evoInitialFieldWidth,evoFieldWidth,
        tmFinalMove,tmFinalPp,tmFinalMaxPp,tmFinalPpUps);
}
#endif

static void SummaryPresent(void *argument) {
    Capture *capture=argument; u32 frame=OS_GetVBlankCount();
#ifdef HG_EVOLUTION_LEARN_CHECK
    EvolutionLearningPresent(capture);return;
#elif defined(HG_EVOLUTION_CHECK)
    EvolutionPresent(capture);return;
#elif defined(HG_SUMMARY_RARE_CANDY_CHECK)
    CandyLearnPresent(capture);return;
#elif defined(HG_SUMMARY_TM_LEARN_CHECK)
    TMLearnPresent(capture); return;
#endif
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
    if (casePassed) QA_CHECK(!final.active && !final.ready && !finalParty.active);
#ifdef HG_SUMMARY_TM_LEARN_CHECK
#ifndef HG_EVOLUTION_CHECK
    if (casePassed) QA_CHECK(seenPhases==((1u<<8)|(1u<<9)));
#endif
#ifdef HG_EVOLUTION_CHECK
    HGNativeEvolutionUIState finalEvolution=readEvolution(capture);if (casePassed) QA_CHECK(!finalEvolution.active && !finalEvolution.ready);
#ifdef HG_EVOLUTION_LEARN_CHECK
    evolutionLearningPrintEvidence();
#else
    evolutionPrintEvidence();
#endif
#elif defined(HG_SUMMARY_RARE_CANDY_CHECK)
    candyPrintEvidence();
#else
    const char *preserved=casePassed ? "true" : "false";
    printf("Summary TM learning evidence: {\"status\":\"%s\",\"fixturePrepared\":%u,\"summaryLifetimes\":%u,\"fifthCancelled\":%u,\"continuedTrying\":%u,\"hmRejected\":%u,\"confirmationBack\":%u,\"replacement\":%u,\"finalMove\":%u,\"finalPpUps\":%u,\"finalPp\":%u,\"finalMaxPp\":%u,\"initialTmQuantity\":%u,\"finalTmQuantity\":%u,\"staleRejected\":%u,\"initialFriendship\":%u,\"finalFriendship\":%u,\"initialMood\":%u,\"finalMood\":%u,\"partyFieldsPreserved\":%s,\"otherBagBytesPreserved\":%s,\"flagsMoneyDexPreserved\":%s,\"friendshipMoodPreservedBeforeLearning\":%s,\"resetFinished\":true,\"peripheralUiNative\":true,\"typedAccepted\":%u,\"typedRejected\":%u,\"typedReleased\":%u,\"typedSettled\":%u,\"partyReturnAcks\":%u,\"exitFadeSeen\":%u,\"exitFadeRejected\":%u}\n",
        caseStatus,tmFixturePrepared,summaryLifetimes,tmFifthCancelled,tmContinuedTrying,tmHmRejected,tmConfirmationBack,tmReplacement,
        tmFinalMove,tmFinalPpUps,tmFinalPp,tmFinalMaxPp,tmInitialQuantity,tmFinalQuantity,staleRejected,
        tmInitialFriendship,tmFinalFriendship,tmInitialMood,tmFinalMood,preserved,preserved,preserved,preserved,
        tmTypedAccepted,tmTypedRejected,tmTypedReleased,tmTypedSettled,tmPartyReturnAcks,tmExitFadeSeen,tmExitFadeRejected);
#endif
#else
    if (casePassed) QA_CHECK(seenPhases==((1u<<2)|(1u<<5)));
#endif
#ifdef HG_SUMMARY_TM_LEARN_CHECK
#ifdef HG_EVOLUTION_CHECK
#ifdef HG_EVOLUTION_LEARN_CHECK
    printf("Summary menu evidence: {\"status\":\"%s\",\"seenPhases\":%u,\"summaryLifetimes\":%u,\"staleRejected\":%u,\"snapshotStages\":%u,\"activeAfterReturn\":%u,\"resetFinished\":true,\"pendingDMA\":0,\"partyItemsMoneyFlagsUnchanged\":false,\"expectedPrivateEvolutionMutation\":true}\n",caseStatus,seenPhases,summaryLifetimes,staleRejected,snapshotStages,final.active);
#else
    printf("Summary menu evidence: {\"status\":\"%s\",\"seenPhases\":0,\"summaryLifetimes\":0,\"staleRejected\":%u,\"snapshotStages\":%u,\"activeAfterReturn\":%u,\"resetFinished\":true,\"pendingDMA\":0,\"partyItemsMoneyFlagsUnchanged\":false,\"expectedPrivateEvolutionMutation\":true}\n",caseStatus,staleRejected,snapshotStages,final.active);
#endif
#elif defined(HG_SUMMARY_RARE_CANDY_CHECK)
    printf("Summary menu evidence: {\"status\":\"%s\",\"seenPhases\":%u,\"summaryLifetimes\":%u,\"staleRejected\":%u,\"snapshotStages\":%u,\"activeAfterReturn\":%u,\"resetFinished\":true,\"pendingDMA\":0,\"partyItemsMoneyFlagsUnchanged\":false,\"expectedPrivateCandyMutation\":true}\n",
        caseStatus,seenPhases,summaryLifetimes,staleRejected,snapshotStages,final.active);
#else
    printf("Summary menu evidence: {\"status\":\"%s\",\"seenPhases\":%u,\"summaryLifetimes\":%u,\"staleRejected\":%u,\"snapshotStages\":%u,\"activeAfterReturn\":%u,\"resetFinished\":true,\"pendingDMA\":0,\"partyItemsMoneyFlagsUnchanged\":false,\"expectedPrivateTmMutation\":true}\n",
        caseStatus,seenPhases,summaryLifetimes,staleRejected,snapshotStages,final.active);
#endif
#else
    printf("Summary menu evidence: {\"status\":\"%s\",\"flag0x6A\":%u,\"partyCount\":%u,\"flag0x6B\":%u,\"dexEnabled\":%u,\"selectedSlot\":%u,\"seenPages\":%u,\"seenPhases\":%u,\"viewedMoves\":%u,\"summaryLifetimes\":%u,\"partyLifetimes\":%u,\"staleRejected\":%u,\"snapshotStages\":%u,\"activeAfterReturn\":%u,\"resetFinished\":true,\"pendingDMA\":0,\"partyItemsMoneyFlagsUnchanged\":true}\n",caseStatus,starter,partyCount,gotDex,dexEnabled,selectedSlot,seenPages,seenPhases,viewedMoves,summaryLifetimes,partyLifetimes,staleRejected,snapshotStages,final.active);
#endif
    wasm2c_hgengine_free(game); for (u32 i=0;i<3;i++) close(wasi->fds[i]); free(originalBag); free(game); free(env); free(wasi); free(capture); summaryEnv=NULL; wasm_rt_free();
#ifdef HG_SUMMARY_TM_LEARN_CHECK
    free(tmOriginalFlags);
#ifdef HG_EVOLUTION_CHECK
#ifdef HG_EVOLUTION_LEARN_CHECK
    if (casePassed) puts("PASS: original evolution learning route, nested Summary fifth/HM/back/replace, original outcome and field/reset.");
#else
    if (casePassed) puts("PASS: original field Rare Candy evolution route, canonical main geometry, owner-typed input, original outcome and field/reset.");
#endif
#elif defined(HG_SUMMARY_RARE_CANDY_CHECK)
    if (casePassed) puts("PASS: original Rare Candy route and native Summary learning, two original stats views, cancel keeps upgrade, HM refusal/back/replacement and no double consumption.");
#else
    if (casePassed) puts("PASS: original TM route and native Summary learning, original HM refusal/cancel/replace, original item/PP result and reset; Bag/Party used revision-bound native typed queues.");
#endif
#else
    if (casePassed) puts("PASS: original Summary pages and move details, original Party returns, unchanged party/items/money/flags, stale owners, original frames and reset.");
#endif
    return casePassed ? 0 : 3;
}
