// SPDX-License-Identifier: GPL-3.0-or-later
#define main MenusBootstrapMain
#define HGEngineTrap MenusBootstrapTrap
#include "../Original/abi32/engine_check.c"
#undef HGEngineTrap
#undef main
#include "battle_menus_qa.h"
#include "native_battle_ui.h"
#include "native_battle_bag_ui.h"
#include "native_battle_party_ui.h"
#ifdef HG_NATIVE_FIELD_POTION_CHECK
#include "native_bag_ui.h"
#include "constants/heap.h"
#include "constants/pokemon.h"
#include "constants/items.h"
#include "item.h"
_Static_assert(sizeof(HGNativePartyUIState)==1480 && sizeof(HGNativeBagUIState)==1568,"Field Potion UI ABI");
#endif
#include <nitro/os/native_reset.h>
#include <setjmp.h>

#ifdef HG_NATIVE_FIELD_POTION_CHECK
#define NATURAL_DAMAGE_GROWL_LIMIT 12
#else
#define NATURAL_DAMAGE_GROWL_LIMIT 6
#endif

#define QA_CHECK(value) do { if (!(value)) { fprintf(stderr,"Battle menus failure line %d stage %u: %s\n",__LINE__,stage,#value); _Exit(1); } } while (0)
typedef struct BattleMenusCommand { u32 active,revision,mode,ready,enabledMask,cancelRun,move[4],pp[4]; } BattleMenusCommand;
_Static_assert(sizeof(BattleMenusCommand)==offsetof(HGNativeBattleUIState,maxPP),"Battle command prefix");
_Static_assert(sizeof(HGNativeBattleBagUIState)==536,"Battle Bag ABI");
_Static_assert(sizeof(HGNativeBattlePartyUIState)==688 && offsetof(HGNativeBattlePartyUIState,selectedMove)==528
    && offsetof(HGNativeBattlePartyUIState,moves)==532 && offsetof(HGNativeBattlePartyUIState,moveToLearn)==580
    && offsetof(HGNativeBattlePartyUIState,prompt)==584 && offsetof(HGNativeBattlePartyUIState,message)==636,"Battle Party ABI");
extern const u8 gBattlePartySummaryWindows[392],gBattlePartyCheckMovesWindows[88],gBattlePartyMoveDetailWindows[136];
extern const u8 gBattlePartyButtonRects[136];
static jmp_buf resetLanding;
static struct w2c_env *menusEnv;
static u32 stage,stageFrame,pulseFrame,pulseReleased,snapshots,lastBagRevision,lastPartyRevision;
static u32 firstBagRevision,firstTargetRevision,staleRejected,fullReplyCount,baselineTurns,growls,turnBefore,preHealHp,preHealReplies,initialGrowlPP;
static u32 runAttempts,fleeFailures,lastRunRevision;
static u32 infoPreviousRevision,infoStaleRejected,infoPages,infoDescriptionMask;
static BOOL infoComplete,infoBankRoundTrip;
static HGNativeBattleMenusQAState infoBefore;
static HGNativeBattlePartyUIMon infoMons[6];
static HGNativeBattlePartyUIMove infoMoves[4];
static u8 infoDescription[120*80*4];
static const char *blockedStatus;
static BOOL resetExpected,baselineComplete,fullHpNoEffect,targetCancel,shiftRejected,healed,damageMissing,sceneBlocked,busyCaptured,fieldReturned;
static HGNativeBattleMenusQAState finalQA;
#ifdef HG_NATIVE_LEARN_MOVES_CHECK
extern const u8 gBattlePartyLearnMoveWindows[48],gBattlePartyForgetMoveWindows[96];
static BOOL learnFixture,learnExp,learnAuto,learnKeepContinue,learnHM,learnFifth,learnContinueFifth,learnReplace,learnUps,learnGiveUp,learnSaved;
static u32 learnPartyRevision,learnCommandRevision,learnStale,learnInitialExp,learnStatsPulses,learnStateChanges,learnPreviousState[20];
static BOOL learnStateSeen;
static u8 learnHMDescription[120*80*4];
#endif
#ifdef HG_NATIVE_FIELD_POTION_CHECK
static u32 fieldIconCenters[7][2],fieldPartyRevision,fieldBagRevision,fieldTargetRevision,fieldFirstBagRevision,fieldPocketVisited;
static u32 fieldPartyAddress,fieldBagAddress,fieldFlagsAddress,fieldDexAddress,fieldMon26[6][26],fieldFlags[5],fieldBagSize,fieldPotionOffset;
static u32 fieldPreHp,fieldProgress,fieldResultWaitMask,fieldLastDrawHp,fieldUseLastState[10],fieldUseStateCount;
static u8 *fieldOriginalBag,*fieldExpectedBag;
static BOOL fieldTargetCancelled,fieldCancelNewBag,fieldHealed,fieldHealNewBag,fieldFullHp,fieldFullHpNewBag,fieldStale,fieldQuantityExact,fieldSemanticStable;
static const u32 fieldMonAttributes[26]={MON_DATA_PERSONALITY,MON_DATA_OT_ID,MON_DATA_SPECIES,MON_DATA_FORM,MON_DATA_IS_EGG,MON_DATA_HELD_ITEM,
    MON_DATA_HP,MON_DATA_MAX_HP,MON_DATA_LEVEL,MON_DATA_STATUS,MON_DATA_MOVE1,MON_DATA_MOVE2,MON_DATA_MOVE3,MON_DATA_MOVE4,
    MON_DATA_MOVE1_PP,MON_DATA_MOVE2_PP,MON_DATA_MOVE3_PP,MON_DATA_MOVE4_PP,MON_DATA_MOVE1_PP_UPS,MON_DATA_MOVE2_PP_UPS,MON_DATA_MOVE3_PP_UPS,MON_DATA_MOVE4_PP_UPS,
    MON_DATA_MOVE1_MAX_PP,MON_DATA_MOVE2_MAX_PP,MON_DATA_MOVE3_MAX_PP,MON_DATA_MOVE4_MAX_PP};
#endif
#ifdef HG_NATIVE_FORCED_SWITCH_CHECK
static u32 forcedGrowls,forcedRevision,forcedCommandRevision,forcedReplyBefore,forcedTurnBefore;
static BOOL forcedKO,forcedTypedCancel,forcedActualCancel,forcedInfo,forcedFaintRejected,forcedEggInfo,forcedEggRejected,forcedReply,forcedActive;
static BOOL forcedNextMonPrompt,forcedNextMonYes;
static u32 forcedNextMonNameWidth[2];
static HGNativeBattlePartyUIMon forcedMons[6]; static void forcedDiagnostic(HGNativeBattleMenusQAState q,BattleMenusCommand cmd,HGNativeBattlePartyUIState party,u32 frame);
#endif
#ifdef HG_BATTLE_MENUS_INFO_NAV
static u32 navPreviousRevision,navVisits,navStaleRejected,navMoveSeen,navNameSeen[2],navNameDistinctPages,navCommandRevision;
static BOOL navComplete,navBrowsingStable;
static HGNativeBattlePartyUIMon navMons[6];
static HGNativeBattlePartyUIMove navMoves[2][4];
static u8 navNames[2][2][72*16*4];
#endif
void HGEngineTrap(wasm_rt_trap_t code) {
    if (code==WASM_RT_TRAP_UNREACHABLE && resetExpected && menusEnv && menusEnv->resetRequested) longjmp(resetLanding,1);
    MenusBootstrapTrap(code);
}
static const void *guest(Capture *capture,u32 address,size_t size) {
    QA_CHECK(address && address<=capture->env->memory->size && size<=capture->env->memory->size-address);
    return capture->env->memory->data+address;
}
static HGNativeBattleMenusQAState readQA(Capture *capture) {
    u32 address=w2c_hgengine_HGNativeBattleMenusQAGetState(capture->game); HGNativeBattleMenusQAState q;
    memcpy(&q,guest(capture,address,sizeof(q)),sizeof(q));
    QA_CHECK(!memcmp(&q,guest(capture,address,sizeof(q)),sizeof(q)) && q.version==HG_BATTLE_MENUS_QA_VERSION && q.byteSize==sizeof(q));
    if (q.errorLine) fprintf(stderr,"Battle menus guest assertion line %u\n",q.errorLine);
    QA_CHECK(!q.errorLine); return q;
}
static BattleMenusCommand readCommand(Capture *capture) {
    u32 address=w2c_hgengine_HGNativeBattleUIGetState(capture->game); BattleMenusCommand state;
    memcpy(&state,guest(capture,address,sizeof(state)),sizeof(state)); QA_CHECK(!memcmp(&state,guest(capture,address,sizeof(state)),sizeof(state))); return state;
}
static HGNativeBattleBagUIState readBag(Capture *capture) {
    u32 address=w2c_hgengine_HGNativeBattleBagUIGetState(capture->game); HGNativeBattleBagUIState state;
    memcpy(&state,guest(capture,address,sizeof(state)),sizeof(state)); QA_CHECK(!memcmp(&state,guest(capture,address,sizeof(state)),sizeof(state)));
    QA_CHECK(state.revision>=lastBagRevision); lastBagRevision=state.revision; return state;
}
static HGNativeBattlePartyUIState readParty(Capture *capture) {
    u32 address=w2c_hgengine_HGNativeBattlePartyUIGetState(capture->game); HGNativeBattlePartyUIState state;
    memcpy(&state,guest(capture,address,sizeof(state)),sizeof(state)); QA_CHECK(!memcmp(&state,guest(capture,address,sizeof(state)),sizeof(state)));
    QA_CHECK(state.revision>=lastPartyRevision); lastPartyRevision=state.revision; return state;
}
static void bytes(Capture *capture,const char *role,const void *data,size_t size) {
    char path[4096]; QA_CHECK(snprintf(path,sizeof(path),"%s/stage-%02u-shot-%02u-%s",capture->folder,stage,snapshots,role)<sizeof(path));
    FILE *file=fopen(path,"wb"); QA_CHECK(file && fwrite(data,1,size,file)==size && !fclose(file));
}
static void window(Capture *capture,const char *role,const HGNativePartyUIWindow *w) {
    if (!w->pixelBuffer) return;
    QA_CHECK(w->tileWidth && w->tileWidth<=32 && w->tileHeight && w->tileHeight<=24 && w->clearColor<16 && w->engine<2);
    bytes(capture,role,guest(capture,w->pixelBuffer,w->tileWidth*w->tileHeight*32),w->tileWidth*w->tileHeight*32);
    printf("Battle menus window: {\"stage\":%u,\"role\":\"%s\",\"width\":%u,\"height\":%u,\"engine\":%u,\"palette\":[",stage,role,w->tileWidth,w->tileHeight,w->engine);
    for (u32 i=0;i<16;i++) printf("%s%u",i?",":"",w->palette[i]); puts("]}");
}
#ifdef HG_BATTLE_MENUS_INFO_NAV
static void navigationName(const HGNativeBattlePartyUIState *party,const u8 *pixels,u32 width,u32 height) {
    QA_CHECK((party->page==2 || party->page==3) && (party->selectedSlot==0 || party->selectedSlot==2) && width==72 && height==16);
    u32 page=party->page-2,mon=party->selectedSlot/2;
    if (navNameSeen[page]&(1u<<mon)) QA_CHECK(!memcmp(navNames[page][mon],pixels,sizeof(navNames[page][mon])));
    else { memcpy(navNames[page][mon],pixels,sizeof(navNames[page][mon])); navNameSeen[page]|=1u<<mon; }
    if (navNameSeen[page]==3) { QA_CHECK(memcmp(navNames[page][0],navNames[page][1],sizeof(navNames[page][0]))); navNameDistinctPages|=1u<<page; }
}
#endif
static void infoCrops(Capture *capture,const HGNativeBattlePartyUIState *party,const u8 *pixels) {
    const u8 *templates; u32 count,stride,bg;
    if (party->phase==HG_BATTLE_PARTY_UI_SUMMARY && party->page==2) {
        templates=gBattlePartySummaryWindows; count=13; stride=22; bg=5;
    } else if (party->phase==HG_BATTLE_PARTY_UI_CHECK_MOVES && party->page==3) {
        templates=gBattlePartyCheckMovesWindows; count=5; stride=6; bg=5;
    } else if (party->phase==HG_BATTLE_PARTY_UI_MOVE_DETAIL && party->page==4) {
        templates=gBattlePartyMoveDetailWindows; count=6; stride=11; bg=4;
    } else return;
    QA_CHECK(party->ready && !party->prompt.pixelBuffer && !party->message.pixelBuffer);
    u8 crop[sizeof(infoDescription)];
    for (u32 i=0;i<count;++i) {
        if (party->page==3 && i && !party->moves[i-1].moveId) continue;
        const u8 *role=templates+i*8,*other=templates+(stride+i)*8;
        // Both original text banks have identical on-screen crops and distinct tile allocations.
        QA_CHECK(!memcmp(role,other,6) && role[0]==bg && role[5]<16 && memcmp(role+6,other+6,2));
        u32 x=role[1]*8,y=role[2]*8,width=role[3]*8,height=role[4]*8;
        QA_CHECK(width && height && x+width<=256 && y+height<=192 && width*height*4<=sizeof(crop));
        BOOL varied=FALSE;
        for (u32 row=0;row<height;++row) {
            memcpy(crop+row*width*4,pixels+((y+row)*256+x)*4,width*4);
            for (u32 col=0;col<width;++col) if (memcmp(crop,crop+(row*width+col)*4,4)) varied=TRUE;
        }
        QA_CHECK(varied);
#ifdef HG_BATTLE_MENUS_INFO_NAV
        if (!i && (party->page==2 || party->page==3)) navigationName(party,crop,width,height);
#endif
        char name[64]; QA_CHECK(snprintf(name,sizeof(name),"info-page-%u-text-%02u.rgba",party->page,i)<sizeof(name));
        bytes(capture,name,crop,width*height*4);
        printf("Battle menus info crop: {\"stage\":%u,\"page\":%u,\"index\":%u,\"bg\":%u,\"x\":%u,\"y\":%u,\"width\":%u,\"height\":%u,\"palette\":%u,\"bankBaseTiles\":[%u,%u]}\n",
            stage,party->page,i,bg,x,y,width,height,role[5],role[6]|role[7]<<8,other[6]|other[7]<<8);
        if (party->page==4 && i==4) {
            QA_CHECK(width==120 && height==80 && party->selectedMove<2);
            if (party->selectedMove==0) {
                if (infoDescriptionMask&1) { QA_CHECK(!memcmp(infoDescription,crop,sizeof(infoDescription))); infoBankRoundTrip=TRUE; }
                else memcpy(infoDescription,crop,sizeof(infoDescription));
                infoDescriptionMask|=1;
            } else {
                QA_CHECK((infoDescriptionMask&1) && memcmp(infoDescription,crop,sizeof(infoDescription)));
                infoDescriptionMask|=2;
            }
        }
    }
}
#ifdef HG_NATIVE_LEARN_MOVES_CHECK
static void learnCrops(Capture *capture,const HGNativeBattlePartyUIState *party,const u8 *pixels) {
    if (!party->active || !party->ready || party->mode!=HG_BATTLE_PARTY_UI_LEARN_MOVE) return;
    const u8 *roles; u32 first,count;
    if (party->phase==HG_BATTLE_PARTY_UI_LEARN_LIST && party->page==6) { roles=gBattlePartyLearnMoveWindows; first=0; count=6; }
    else if (party->phase==HG_BATTLE_PARTY_UI_LEARN_DETAIL && party->page==7) { roles=gBattlePartyForgetMoveWindows; first=8; count=1; }
    else return;
    u8 crop[120*80*4];
    for (u32 i=first;i<first+count;++i) {
        const u8 *role=roles+i*8; u32 x=role[1]*8,y=role[2]*8,width=role[3]*8,height=role[4]*8;
        QA_CHECK(role[0]==(party->page==6 ? 4 : 5) && width && height && x+width<=256 && y+height<=192 && width*height*4<=sizeof(crop));
        BOOL varied=FALSE;
        for (u32 row=0;row<height;++row) {
            memcpy(crop+row*width*4,pixels+((y+row)*256+x)*4,width*4);
            for (u32 col=0;col<width;++col) varied |= memcmp(crop,crop+(row*width+col)*4,4)!=0;
        }
        QA_CHECK(varied);
        char name[64]; QA_CHECK(snprintf(name,sizeof(name),"learn-page-%u-text-%02u.rgba",party->page,i)<sizeof(name));
        bytes(capture,name,crop,width*height*4);
        printf("Battle menus learn moves crop: {\"stage\":%u,\"page\":%u,\"candidate\":%u,\"selectedMove\":%u,\"index\":%u,\"bg\":%u,\"x\":%u,\"y\":%u,\"width\":%u,\"height\":%u}\n",
            stage,party->page,party->moveToLearn,party->selectedMove,i,role[0],x,y,width,height);
        if (party->page==7 && stage==305) memcpy(learnHMDescription,crop,sizeof(learnHMDescription));
        if (party->page==7 && stage==306) QA_CHECK(memcmp(learnHMDescription,crop,sizeof(learnHMDescription)));
    }
}
#endif
static BOOL snapshot(Capture *capture,const char *role,BOOL active) {
    if (!capture->buffer) capture->buffer=w2c_hgengine_HGNativeFrameBuffer(capture->game);
    guest(capture,capture->buffer,256*192*4); HGNativeDrainDMA();
    u32 geometry[8]={0};
    if (w2c_hgengine_HGNativeGeometryFrame(capture->game,1,capture->buffer)) memcpy(geometry,guest(capture,capture->buffer,sizeof(geometry)),sizeof(geometry));
    for (u32 engine=0;engine<2;engine++) {
        if (!w2c_hgengine_HGNativeRender2D(capture->game,engine,capture->buffer,256*192*4)) return FALSE;
        const u8 *pixels=guest(capture,capture->buffer,256*192*4); BOOL varied=FALSE;
        for (u32 i=1;i<256*192;i++) if (memcmp(pixels,pixels+4*i,4)) { varied=TRUE; break; }
        if ((!engine || active) && !varied) return FALSE;
        if (engine) { HGNativeBattlePartyUIState party=readParty(capture); infoCrops(capture,&party,pixels);
#ifdef HG_NATIVE_LEARN_MOVES_CHECK
            learnCrops(capture,&party,pixels);
#endif
        }
        char path[4096]; QA_CHECK(snprintf(path,sizeof(path),"%s/stage-%02u-shot-%02u-%s-engine-%u.ppm",capture->folder,stage,snapshots,role,engine)<sizeof(path));
        FILE *file=fopen(path,"wb"); QA_CHECK(file && fprintf(file,"P6\n256 192\n255\n")>0);
        for (u32 i=0;i<256*192;i++) QA_CHECK(fwrite(pixels+4*i,1,3,file)==3);
        QA_CHECK(!fclose(file));
    }
    HGNativeBattleBagUIState bag=readBag(capture); HGNativeBattlePartyUIState party=readParty(capture); HGNativeBattleMenusQAState q=readQA(capture);
    bytes(capture,"bag-state.bin",&bag,sizeof(bag)); bytes(capture,"party-state.bin",&party,sizeof(party)); bytes(capture,"qa-state.bin",&q,sizeof(q));
    window(capture,"bag-name.4bpp",&bag.selectedName); window(capture,"bag-quantity.4bpp",&bag.selectedQuantityText);
    window(capture,"bag-description.4bpp",&bag.description); window(capture,"bag-message.4bpp",&bag.message);
    window(capture,"party-prompt.4bpp",&party.prompt); window(capture,"party-message.4bpp",&party.message);
    printf("Battle menus geometry: {\"stage\":%u,\"shot\":%u,\"vertices\":%u,\"polygons\":%u,\"serial\":%u}\n",stage,snapshots,geometry[0],geometry[1],geometry[2]);
    printf("Battle menus snapshot: {\"stage\":%u,\"role\":\"%s\",\"frame\":%u,\"turns\":%u,\"hp\":%u,\"potion\":%u,\"bagRevision\":%u,\"bagPhase\":%u,\"bagPage\":%u,\"partyRevision\":%u,\"partyPhase\":%u,\"partyMode\":%u}\n",stage,role,OS_GetVBlankCount(),q.totalTurns,q.playerHp,q.potionQuantity,bag.revision,bag.phase,bag.page,party.revision,party.phase,party.mode);
    ++snapshots; return TRUE;
}
static void next(u32 value) { stage=value; stageFrame=OS_GetVBlankCount(); busyCaptured=FALSE; }
static void pulse(Capture *capture,u32 x,u32 y) {
    QA_CHECK(!pulseFrame && HGNativeSetInput(capture->env,0,FALSE,x,y,TRUE)); pulseFrame=OS_GetVBlankCount(); pulseReleased=FALSE;
}
static void bagChoice(Capture *capture,HGNativeBattleBagUIState previous,u32 row) {
    HGNativeBattleBagUIState state=readBag(capture); u32 x,y;
    QA_CHECK(HGNativeBattleBagUIResolveTouch(&state,previous.revision,previous.phase,row,&x,&y)); pulse(capture,x,y);
}
static void partyChoice(Capture *capture,HGNativeBattlePartyUIState previous,u32 row) {
    HGNativeBattlePartyUIState state=readParty(capture); u32 x,y;
    QA_CHECK(HGNativeBattlePartyUIResolveTouch(&state,previous.revision,previous.phase,row,&x,&y)); pulse(capture,x,y);
}
static void infoUnchanged(HGNativeBattleMenusQAState q) {
    HGNativeBattleMenusQAState before=infoBefore;
    q.revision=before.revision=0; q.fieldFrame=before.fieldFrame=0;
    QA_CHECK(!memcmp(&q,&before,sizeof(q)));
}
static void infoState(HGNativeBattleMenusQAState q,HGNativeBattlePartyUIState party,u32 phase,u32 page,u32 selectedMove) {
    infoUnchanged(q);
    QA_CHECK(party.active && party.ready && party.phase==phase && party.page==page && party.mode==HG_BATTLE_PARTY_UI_SWITCH
        && party.partyCount==1 && !party.selectedSlot && !party.itemId && !party.pocket && !memcmp(party.mons,infoMons,sizeof(infoMons))
        && !party.prompt.pixelBuffer && !party.message.pixelBuffer && party.revision>infoPreviousRevision);
    u32 buttons[8],count,mask;
    if (page==2) { u32 expected[]={12,13,11,6}; memcpy(buttons,expected,sizeof(expected)); count=4; mask=12; }
    else if (page==3) { u32 expected[]={14,15,16,17,12,13,9,6}; memcpy(buttons,expected,sizeof(expected)); count=8; mask=195; }
    else { u32 expected[]={30,31,32,33,6}; memcpy(buttons,expected,sizeof(expected)); count=5; mask=19; QA_CHECK(page==4 && party.selectedMove==selectedMove); }
    QA_CHECK(party.buttonCount==count && party.enabledMask==mask);
    for (u32 i=0;i<4;++i) {
        QA_CHECK(party.moves[i].moveId==q.moves[i] && party.moves[i].pp==q.pp[i]);
        if (q.moves[i]) QA_CHECK(party.moves[i].maxPP && party.moves[i].pp<=party.moves[i].maxPP);
        else QA_CHECK(!party.moves[i].maxPP);
    }
    if (!infoPages) memcpy(infoMoves,party.moves,sizeof(infoMoves));
    else QA_CHECK(!memcmp(infoMoves,party.moves,sizeof(infoMoves)));
    for (u32 i=0;i<count;++i) {
        const HGNativeBattlePartyUIButton *button=&party.buttons[i]; const u8 *rect=gBattlePartyButtonRects+buttons[i]*4;
        u32 x,y; QA_CHECK(button->cropX==rect[0]*8 && button->cropY==rect[1]*8 && button->cropWidth==rect[2]*8 && button->cropHeight==rect[3]*8);
        QA_CHECK(HGNativeBattlePartyUIResolveTouch(&party,party.revision,phase,i,&x,&y)==!!(mask&(1u<<i)));
    }
    u32 x,y,row=page==2 ? 2 : 0;
    QA_CHECK(!HGNativeBattlePartyUIResolveTouch(&party,infoPreviousRevision,phase,row,&x,&y)); ++infoStaleRejected;
    infoPreviousRevision=party.revision; infoPages|=1u<<page;
}
static void commandChoice(Capture *capture,BattleMenusCommand state,u32 row) {
    QA_CHECK(state.active && state.ready && (state.enabledMask&(1u<<row)) && w2c_hgengine_HGNativeBattleUIChoose(capture->game,state.revision,row));
}
static void originalRun(Capture *capture,BattleMenusCommand state) {
    HGNativeBattleMenusQAState q=readQA(capture);
    if (!q.playerHp) {
        blockedStatus="prerequisite_player_not_alive_for_flee"; finalQA=q; resetExpected=TRUE;
        w2c_env_HGNativeResetControl(capture->env,HG_RESET_REQUEST,0); QA_CHECK(FALSE);
    }
    QA_CHECK(runAttempts<6); ++runAttempts; lastRunRevision=state.revision;
    printf("Battle menus flee attempt: {\"attempt\":%u,\"turns\":%u,\"hp\":%u,\"commandRevision\":%u}\n",runAttempts,q.totalTurns,q.playerHp,state.revision);
    commandChoice(capture,state,3);
}
static u32 potionRow(HGNativeBattleBagUIState bag) {
    for (u32 i=0;i<6;i++) if (bag.itemId[i]==17 && bag.quantity[i]==5) return i;
    QA_CHECK(FALSE); return 0;
}
#ifdef HG_NATIVE_FIELD_POTION_CHECK
static HGNativePartyUIState readFieldParty(Capture *capture) {
    u32 address=w2c_hgengine_HGNativePartyUIGetState(capture->game); HGNativePartyUIState state;
    memcpy(&state,guest(capture,address,sizeof(state)),sizeof(state));
    QA_CHECK(!memcmp(&state,guest(capture,address,sizeof(state)),sizeof(state)) && state.revision>=fieldPartyRevision);
    fieldPartyRevision=state.revision; return state;
}
static HGNativeBagUIState readFieldBag(Capture *capture) {
    u32 address=w2c_hgengine_HGNativeBagUIGetState(capture->game); HGNativeBagUIState state;
    memcpy(&state,guest(capture,address,sizeof(state)),sizeof(state));
    QA_CHECK(!memcmp(&state,guest(capture,address,sizeof(state)),sizeof(state)) && state.revision>=fieldBagRevision);
    fieldBagRevision=state.revision; return state;
}
static void fieldSemantic(Capture *capture,const char *step,BOOL baseline,BOOL healed) {
    u32 current[6][26]={{0}},flags[5];
    QA_CHECK(w2c_hgengine_Party_GetCount(capture->game,fieldPartyAddress)==1);
    u32 mon=w2c_hgengine_Party_GetMonByIndex(capture->game,fieldPartyAddress,0); guest(capture,mon,1);
    for (u32 j=0;j<26;++j) current[0][j]=w2c_hgengine_GetMonData(capture->game,mon,fieldMonAttributes[j],0);
    flags[0]=w2c_hgengine_CheckGotStarter(capture->game,fieldFlagsAddress); flags[1]=w2c_hgengine_CheckGotPokedex(capture->game,fieldFlagsAddress);
    flags[2]=w2c_hgengine_Pokedex_IsEnabled(capture->game,fieldDexAddress); flags[3]=w2c_hgengine_Pokedex_CountNationalDexSeen(capture->game,fieldDexAddress);
    flags[4]=w2c_hgengine_Pokedex_CountNationalDexOwned(capture->game,fieldDexAddress);
    u32 size=w2c_hgengine_Save_Bag_sizeof(capture->game),quantity=w2c_hgengine_Bag_GetQuantity(capture->game,fieldBagAddress,17,HEAP_ID_FIELD2);
    const u8 *bag=guest(capture,fieldBagAddress,size);
    if (baseline) {
        QA_CHECK(current[0][2]==152 && !current[0][4] && !current[0][5] && current[0][6] && current[0][6]<20 && current[0][7]==20 && quantity==5);
        fieldPreHp=fieldLastDrawHp=current[0][6]; memcpy(fieldMon26,current,sizeof(current)); memcpy(fieldFlags,flags,sizeof(flags));
        fieldBagSize=size; QA_CHECK(size>=sizeof(ItemSlot)); fieldOriginalBag=malloc(size); fieldExpectedBag=malloc(size); QA_CHECK(fieldOriginalBag && fieldExpectedBag);
        memcpy(fieldOriginalBag,bag,size); memcpy(fieldExpectedBag,bag,size);
        u32 found=0;
        // Ask the original getter for the actual medicine slots; preserve every other Bag byte.
        for (u32 i=0;i<NUM_BAG_MEDICINE;++i) {
            u32 address=w2c_hgengine_Bag_GetPocketSlotN(capture->game,fieldBagAddress,POCKET_MEDICINE,i); ItemSlot slot;
            QA_CHECK(address>=fieldBagAddress && address-fieldBagAddress<=size-sizeof(slot));
            memcpy(&slot,guest(capture,address,sizeof(slot)),sizeof(slot));
            if (slot.id==ITEM_POTION) { QA_CHECK(slot.quantity==5); fieldPotionOffset=address-fieldBagAddress+offsetof(ItemSlot,quantity); ++found; }
        }
        QA_CHECK(found==1); u16 four=4; memcpy(fieldExpectedBag+fieldPotionOffset,&four,2);
    } else {
        QA_CHECK(size==fieldBagSize && quantity==(healed ? 4 : 5) && !memcmp(flags,fieldFlags,sizeof(flags)));
        for (u32 j=0;j<26;++j) QA_CHECK(current[0][j]==(j==6 && healed ? 20 : fieldMon26[0][j]));
        QA_CHECK(!memcmp(bag,healed ? fieldExpectedBag : fieldOriginalBag,size));
        fieldQuantityExact=TRUE; fieldSemanticStable=TRUE;
    }
    bytes(capture,"field-mon26.bin",current,sizeof(current)); bytes(capture,"field-bag.bin",bag,size);
    printf("Battle menus field Potion checkpoint: {\"step\":\"%s\",\"liveHp\":%u,\"maxHp\":%u,\"potion\":%u,\"monAttributes\":26,\"bagBytes\":%u,\"baselineAfterBattle\":true,\"otherAttributesFlagsDexBagUnchanged\":true}\n",step,current[0][6],current[0][7],quantity,size);
}
static void fieldBagChoice(Capture *capture,HGNativeBagUIState previous,u32 kind,u32 row) {
    HGNativeBagUIState current=readFieldBag(capture); u32 x,y;
    QA_CHECK(HGNativeBagUIResolveTouch(&current,previous.revision,previous.phase,kind,row,&x,&y)); pulse(capture,x,y);
}
static void fieldPartyChoice(Capture *capture,HGNativePartyUIState previous,u32 kind,u32 row) {
    HGNativePartyUIState current=readFieldParty(capture); u32 x,y;
    QA_CHECK(HGNativePartyUIResolveTouch(&current,previous.revision,previous.phase,kind,row,&x,&y)); pulse(capture,x,y);
}
static BOOL fieldSnapshot(Capture *capture,const char *role,HGNativePartyUIState party,HGNativeBagUIState bag) {
    if (!snapshot(capture,role,party.active || bag.active)) return FALSE;
    bytes(capture,"field-party-state.bin",&party,sizeof(party)); bytes(capture,"field-bag-state.bin",&bag,sizeof(bag));
    window(capture,"field-party-message.4bpp",&party.message); window(capture,"field-bag-prompt.4bpp",&bag.prompt);
    window(capture,"field-bag-message.4bpp",&bag.message); return TRUE;
}
static BOOL fieldChoosePotion(Capture *capture,HGNativeBagUIState bag) {
    for (u32 i=0;i<bag.visibleCount && i<HG_BAG_UI_ITEMS;++i) if (bag.items[i].itemId==17 && bag.items[i].enabled) {
        fieldPocketVisited=0; fieldBagChoice(capture,bag,HG_BAG_UI_TOUCH_ITEM,i); return TRUE;
    }
    fieldPocketVisited|=1u<<bag.pocketIndex;
    for (u32 i=0;i<HG_BAG_UI_POCKETS;++i) if ((bag.pocketMask&(1u<<i)) && !(fieldPocketVisited&(1u<<i))) {
        fieldBagChoice(capture,bag,HG_BAG_UI_TOUCH_POCKET,i); return FALSE;
    }
    QA_CHECK(FALSE); return FALSE;
}
static void fieldUse(Capture *capture,HGNativeBagUIState bag) {
    QA_CHECK(bag.selectedItemId==17);
    for (u32 i=0;i<bag.actionCount && i<HG_BAG_UI_ACTIONS;++i) if (!bag.actions[i].actionId && bag.actions[i].enabled) {
        fieldBagChoice(capture,bag,HG_BAG_UI_TOUCH_ACTION,i); return;
    }
    QA_CHECK(FALSE);
}
static void fieldObserveUse(HGNativePartyUIState party) {
    u32 x,y;
    u32 continueResolved=party.phase==HG_PARTY_UI_ITEM_USE
        && HGNativePartyUIResolveTouch(&party,party.revision,party.phase,HG_PARTY_UI_TOUCH_CANCEL,0,&x,&y);
    u32 observed[]={stage,party.phase,party.selected,party.ready,party.cancelEnabled,party.revision,party.mons[0].hp,
        party.message.tileWidth,party.message.tileHeight,continueResolved};
    if (!fieldUseStateCount || memcmp(observed,fieldUseLastState,sizeof(observed))) {
        QA_CHECK(fieldUseStateCount<128); ++fieldUseStateCount; memcpy(fieldUseLastState,observed,sizeof(observed));
        printf("Battle menus field Potion state: {\"stage\":%u,\"phase\":%u,\"selected\":%u,\"ready\":%u,\"cancelEnabled\":%u,\"revision\":%u,\"hp\":%u,\"messageWidth\":%u,\"messageHeight\":%u,\"continueResolved\":%u}\n",
            stage,party.phase,party.selected,party.ready,party.cancelEnabled,party.revision,party.mons[0].hp,
            party.message.tileWidth,party.message.tileHeight,continueResolved);
    }
    if ((stage==127 || stage==131) && HGNativePartyUIIsPotionUse(&party) && party.phase==HG_PARTY_UI_ITEM_USE) {
        QA_CHECK(party.selected==(stage==127 ? 0 : HG_PARTY_UI_CONFIRM));
        QA_CHECK(party.revision>fieldTargetRevision
            && !HGNativePartyUIResolveTouch(&party,fieldTargetRevision,HG_PARTY_UI_ITEM_TARGET,HG_PARTY_UI_TOUCH_MON,0,&x,&y)
            && !HGNativePartyUIResolveTouch(&party,fieldTargetRevision,party.phase,HG_PARTY_UI_TOUCH_CANCEL,0,&x,&y)); fieldStale=TRUE;
        QA_CHECK(!HGNativePartyUIResolveTouch(&party,party.revision,party.phase,HG_PARTY_UI_TOUCH_MON,0,&x,&y));
        if (!party.ready) {
            QA_CHECK(!continueResolved);
        } else {
            QA_CHECK(continueResolved); fieldResultWaitMask|=stage==127 ? 1u : 2u;
        }
        if (stage==127 && party.mons[0].hp>fieldPreHp) {
            ++fieldProgress;
            if (party.mons[0].hp!=fieldLastDrawHp) {
                QA_CHECK(party.mons[0].hp>fieldLastDrawHp && party.mons[0].hp<=20);
                printf("Battle menus field Potion draw: {\"previousHp\":%u,\"drawHp\":%u,\"ready\":%u,\"selected\":%u,\"revision\":%u}\n",fieldLastDrawHp,party.mons[0].hp,party.ready,party.selected,party.revision);
                fieldLastDrawHp=party.mons[0].hp;
            }
        }
    }
}
static void fieldPotionPresent(Capture *capture,HGNativeBattleMenusQAState q) {
    HGNativePartyUIState party=readFieldParty(capture); HGNativeBagUIState bag=readFieldBag(capture); u32 x,y;
    BOOL list=bag.active && bag.ready && HGNativeBagUIIsList(&bag) && !bag.mode;
    BOOL menu=bag.active && bag.ready && bag.phase==HG_BAG_UI_ITEM_MENU && !bag.mode;
    BOOL target=HGNativePartyUIIsPotionUse(&party) && party.ready && party.phase==HG_PARTY_UI_ITEM_TARGET;
    if ((stage==127 || stage==131) && HGNativePartyUIIsPotionUse(&party) && party.phase==HG_PARTY_UI_ITEM_USE) {
        if (!party.ready || party.selected!=(stage==127 ? 0 : HG_PARTY_UI_CONFIRM)) return;
        QA_CHECK(party.message.pixelBuffer && party.message.tileWidth==27 && party.message.tileHeight==4 && party.cancelEnabled);
        if (!fieldSnapshot(capture,stage==127 ? "field-potion-healed" : "field-potion-full-no-effect",party,bag)) return;
        fieldSemantic(capture,stage==127 ? "healed" : "full-hp-no-effect",FALSE,TRUE);
        if (stage==127) { QA_CHECK(fieldProgress && (fieldResultWaitMask&1)); fieldHealed=TRUE; } else fieldFullHp=TRUE;
        fieldPartyChoice(capture,party,HG_PARTY_UI_TOUCH_CANCEL,0); next(stage+1); return;
    }
    switch (stage) {
    case 120: {
        u32 icons=w2c_hgengine_HGNativeFieldMenuIcons(capture->game);
        for (u32 i=0;i<7;++i) if (((icons>>(i*4))&15)==2) { pulse(capture,fieldIconCenters[i][0],fieldIconCenters[i][1]); next(121); break; }
        break;
    }
    case 121: case 124: case 128: case 132:
        if (!list) break;
        if (stage==121) fieldFirstBagRevision=bag.revision;
        else {
            QA_CHECK(bag.revision>fieldFirstBagRevision && !party.active);
            QA_CHECK(!HGNativePartyUIResolveTouch(&party,fieldTargetRevision,HG_PARTY_UI_ITEM_TARGET,HG_PARTY_UI_TOUCH_MON,0,&x,&y)); fieldStale=TRUE;
            fieldSemantic(capture,stage==124 ? "cancel-new-bag" : stage==128 ? "heal-new-bag" : "full-hp-new-bag",FALSE,stage!=124);
            if (!fieldSnapshot(capture,stage==124 ? "field-potion-cancel-bag" : stage==128 ? "field-potion-heal-bag" : "field-potion-full-bag",party,bag)) break;
            if (stage==124) fieldCancelNewBag=TRUE; else if (stage==128) fieldHealNewBag=TRUE; else fieldFullHpNewBag=TRUE;
        }
        if (stage==132) { fieldBagChoice(capture,bag,HG_BAG_UI_TOUCH_CANCEL,0); next(133); }
        else if (fieldChoosePotion(capture,bag)) next(stage+1);
        break;
    case 122: case 125: case 129: if (menu) { fieldUse(capture,bag); next(stage+1); } break;
    case 123: case 126: case 130:
        if (!target) break;
        QA_CHECK(party.count==1 && party.mons[0].active && party.mons[0].species==152 && party.mons[0].maxHp==20
            && party.mons[0].hp==(stage==130 ? 20 : fieldPreHp));
        if (stage!=123) { QA_CHECK(party.revision>fieldTargetRevision && !HGNativePartyUIResolveTouch(&party,fieldTargetRevision,party.phase,HG_PARTY_UI_TOUCH_MON,0,&x,&y)); fieldStale=TRUE; }
        if (!fieldSnapshot(capture,stage==123 ? "field-potion-target-cancel" : stage==126 ? "field-potion-target-heal" : "field-potion-target-full",party,bag)) break;
        fieldTargetRevision=party.revision;
        fieldPartyChoice(capture,party,stage==123 ? HG_PARTY_UI_TOUCH_CANCEL : HG_PARTY_UI_TOUCH_MON,0);
        if (stage==123) fieldTargetCancelled=TRUE;
        next(stage+1); break;
    case 127: case 131: break;
    case 133:
        if (party.active || bag.active || w2c_hgengine_HGNativeFieldMap(capture->game)!=33) break;
        fieldSemantic(capture,"field-before-reset",FALSE,TRUE);
        if (!fieldSnapshot(capture,"field-potion-field-return",party,bag)) break;
        fieldReturned=TRUE; finalQA=q; resetExpected=TRUE; w2c_env_HGNativeResetControl(capture->env,HG_RESET_REQUEST,0); QA_CHECK(FALSE); break;
    default: QA_CHECK(FALSE);
    }
}
#endif
#ifdef HG_BATTLE_MENUS_INFO_NAV
static void navigationState(HGNativeBattleMenusQAState q,HGNativeBattlePartyUIState party,u32 page,u32 step) {
    static const u32 slots[]={0,2,0,2,0};
    infoUnchanged(q);
    QA_CHECK(step<5 && party.active && party.ready && party.phase==page+1 && party.page==page
        && party.mode==HG_BATTLE_PARTY_UI_SWITCH && party.partyCount==3 && party.selectedSlot==slots[step]
        && !party.itemId && !party.pocket && !party.prompt.pixelBuffer && !party.message.pixelBuffer
        && !memcmp(party.mons,navMons,sizeof(navMons)) && party.revision>navPreviousRevision);
    u32 mon=party.selectedSlot/2,mask=page==2 ? 15 : 240,count=page==2 ? 4 : 8;
    for (u32 i=0;i<4;++i) {
        const HGNativeBattlePartyUIMove *move=&party.moves[i];
        if (move->moveId) { QA_CHECK(move->maxPP && move->pp<=move->maxPP); if (page==3) mask|=1u<<i; }
        else QA_CHECK(!move->pp && !move->maxPP);
        if (!mon) QA_CHECK(move->moveId==q.moves[i] && move->pp==q.pp[i]);
    }
    if (navMoveSeen&(1u<<mon)) QA_CHECK(!memcmp(navMoves[mon],party.moves,sizeof(party.moves)));
    else { memcpy(navMoves[mon],party.moves,sizeof(party.moves)); navMoveSeen|=1u<<mon; }
    if (navMoveSeen==3) QA_CHECK(memcmp(navMoves[0],navMoves[1],sizeof(navMoves[0])));
    QA_CHECK(party.buttonCount==count && party.enabledMask==mask);
    for (u32 i=0;i<count;++i) {
        u32 x,y; QA_CHECK(HGNativeBattlePartyUIResolveTouch(&party,party.revision,party.phase,i,&x,&y)==!!(mask&(1u<<i)));
    }
    if (step) { u32 x,y; QA_CHECK(!HGNativeBattlePartyUIResolveTouch(&party,navPreviousRevision,party.phase,page==2 ? 0 : 4,&x,&y)); ++navStaleRejected; }
    navPreviousRevision=party.revision; ++navVisits;
    printf("Battle menus navigation checkpoint: {\"stage\":%u,\"page\":%u,\"step\":%u,\"slot\":%u,\"species\":%u,\"revision\":%u,\"enabledMask\":%u,\"semanticsMask\":%u,\"moves\":[%u,%u,%u,%u],\"pp\":[%u,%u,%u,%u],\"maxPP\":[%u,%u,%u,%u]}\n",
        stage,page,step,party.selectedSlot,party.mons[party.selectedSlot].species,party.revision,party.enabledMask,q.semanticsMask,
        party.moves[0].moveId,party.moves[1].moveId,party.moves[2].moveId,party.moves[3].moveId,
        party.moves[0].pp,party.moves[1].pp,party.moves[2].pp,party.moves[3].pp,
        party.moves[0].maxPP,party.moves[1].maxPP,party.moves[2].maxPP,party.moves[3].maxPP);
}
static void navigationPresent(Capture *capture,HGNativeBattleMenusQAState q,BattleMenusCommand cmd,HGNativeBattleBagUIState bag,HGNativeBattlePartyUIState party) {
    BOOL command=cmd.active && cmd.ready && cmd.mode==HG_BATTLE_UI_COMMAND;
    if (stage==1) {
        if (!command || q.semanticsMask!=271) return;
        QA_CHECK(q.battleActive && !q.totalTurns && !q.returned && !q.lifecycleCleared && !q.activePartySlot
            && q.partyCount==3 && q.playerHp==20 && q.playerMaxHp==20 && !q.playerStatus && q.potionQuantity==5);
        infoBefore=q; navCommandRevision=cmd.revision; bytes(capture,"navigation-qa-before.bin",&q,sizeof(q));
        if (!snapshot(capture,"navigation-command-before",TRUE)) return;
        commandChoice(capture,cmd,2); next(90); return;
    }
    QA_CHECK(q.semanticsMask==271 && q.battleActive && !q.totalTurns && !q.returned && !q.lifecycleCleared && !q.activePartySlot);
    if (stage!=112) infoUnchanged(q);
    if (stage==90) {
        if (!party.active || !party.ready || party.phase!=HG_BATTLE_PARTY_UI_ROSTER) return;
        QA_CHECK(party.partyCount==3 && party.mons[0].species==152 && !party.mons[0].isEgg
            && party.mons[1].species==175 && party.mons[1].isEgg && party.mons[2].species==155 && !party.mons[2].isEgg);
        memcpy(navMons,party.mons,sizeof(navMons)); navPreviousRevision=party.revision;
        if (!snapshot(capture,"navigation-three-mon-roster",TRUE)) return;
        partyChoice(capture,party,0); next(91); return;
    }
    if (stage==91) {
        if (!party.active || !party.ready || party.phase!=HG_BATTLE_PARTY_UI_MON_MENU) return;
        QA_CHECK(!party.selectedSlot && party.revision>navPreviousRevision); navPreviousRevision=party.revision;
        partyChoice(capture,party,1); next(100); return;
    }
    if (stage>=100 && stage<=109) {
        u32 page=stage<105 ? 2 : 3,step=(stage-100)%5; static const u32 slots[]={0,2,0,2,0};
        if (!party.active || !party.ready || party.phase!=page+1 || party.page!=page
            || party.selectedSlot!=slots[step] || party.revision==navPreviousRevision) return;
        if (!snapshot(capture,page==2 ? "navigation-summary" : "navigation-moves",TRUE)) return;
        navigationState(q,party,page,step);
        if (step<4) { u32 row=step==0 || step==3 ? (page==2 ? 1 : 5) : (page==2 ? 0 : 4); partyChoice(capture,party,row); next(stage+1); }
        else if (page==2) { partyChoice(capture,party,2); next(105); }
        else { partyChoice(capture,party,7); next(110); }
        return;
    }
    if (stage==110) {
        if (!party.active || !party.ready || party.phase!=HG_BATTLE_PARTY_UI_MON_MENU) return;
        QA_CHECK(!party.selectedSlot && party.revision>navPreviousRevision);
        if (!snapshot(capture,"navigation-return-mon-menu",TRUE)) return;
        navPreviousRevision=party.revision;
        bytes(capture,"navigation-qa-after.bin",&q,sizeof(q)); navBrowsingStable=TRUE;
        partyChoice(capture,party,3); next(111); return;
    }
    if (stage==111) {
        if (!party.active || !party.ready || party.phase!=HG_BATTLE_PARTY_UI_ROSTER) return;
        QA_CHECK(party.revision>navPreviousRevision && !memcmp(party.mons,navMons,sizeof(navMons)));
        if (!snapshot(capture,"navigation-return-roster",TRUE)) return;
        partyChoice(capture,party,6); next(112); return;
    }
    QA_CHECK(stage==112);
    if (!command || party.active || bag.active) return;
    QA_CHECK(cmd.revision>navCommandRevision);
    // Original Party Cancel re-emits command selection once; browsing itself changed no QA field.
    { HGNativeBattleMenusQAState expected=infoBefore,actual=q; ++expected.commandCount;
      expected.revision=actual.revision=0; expected.fieldFrame=actual.fieldFrame=0;
      QA_CHECK(!memcmp(&expected,&actual,sizeof(actual))); }
    HGNativeBattlePartyUIState cleared=party,zero={0}; cleared.revision=0; QA_CHECK(!memcmp(&cleared,&zero,sizeof(zero)));
    QA_CHECK(navBrowsingStable && navVisits==10 && navStaleRejected==8 && navNameDistinctPages==3 && navMoveSeen==3);
    if (!snapshot(capture,"navigation-command-before-reset",TRUE)) return;
    bytes(capture,"navigation-qa-before-reset.bin",&q,sizeof(q)); navComplete=TRUE; finalQA=q;
    resetExpected=TRUE; w2c_env_HGNativeResetControl(capture->env,HG_RESET_REQUEST,0); QA_CHECK(FALSE);
}
#endif
#ifdef HG_NATIVE_FORCED_SWITCH_CHECK
static void forcedStable(HGNativeBattleMenusQAState q,HGNativeBattlePartyUIState party) {
    infoUnchanged(q);
    QA_CHECK(party.active && party.ready && party.mode==HG_BATTLE_PARTY_UI_SWITCH_NO_CANCEL && party.partyCount==3
        && !memcmp(party.mons,forcedMons,sizeof(forcedMons)) && !party.itemId && !party.pocket);
}
static void forcedPresent(Capture *capture,HGNativeBattleMenusQAState q,BattleMenusCommand cmd,HGNativeBattlePartyUIState party) {
    BOOL command=cmd.active && cmd.ready && cmd.mode==HG_BATTLE_UI_COMMAND;
    BOOL roster=party.active && party.ready && party.phase==HG_BATTLE_PARTY_UI_ROSTER;
    BOOL monMenu=party.active && party.ready && party.phase==HG_BATTLE_PARTY_UI_MON_MENU;
    u32 x,y;
    if (q.returned) {
        blockedStatus="prerequisite_forced_switch_battle_ended"; finalQA=q; resetExpected=TRUE;
        w2c_env_HGNativeResetControl(capture->env,HG_RESET_REQUEST,0); QA_CHECK(FALSE);
    }
    if ((stage==1 || stage==201 || stage==202) && cmd.active && cmd.ready && cmd.mode==HG_BATTLE_UI_NEXT_MON) {
        QA_CHECK(!forcedNextMonYes && forcedGrowls && !q.playerHp && q.battleActive && !q.replyCount && cmd.enabledMask==3);
        u32 address=w2c_hgengine_HGNativeBattleUIGetState(capture->game);
        const HGNativeBattleUIState *state=guest(capture,address,sizeof(*state));
        QA_CHECK(state->active && state->ready && state->mode==HG_BATTLE_UI_NEXT_MON && state->enabledMask==3 && state->revision==cmd.revision);
        for (u32 i=0;i<2;++i) {
            forcedNextMonNameWidth[i]=state->nameWidth[i];
            QA_CHECK(state->nameWidth[i] && state->nameWidth[i]<=HG_BATTLE_NAME_WIDTH);
            BOOL visible=FALSE;
            for (u32 pixel=0;pixel<HG_BATTLE_NAME_WIDTH*HG_BATTLE_NAME_HEIGHT;++pixel) visible|=state->namePixels[i][pixel*4+3]!=0;
            QA_CHECK(visible);
        }
        if (!snapshot(capture,"forced-next-mon-original-prompt",TRUE)) return;
        state=guest(capture,address,sizeof(*state));
        QA_CHECK(state->revision==cmd.revision && state->ready && state->mode==HG_BATTLE_UI_NEXT_MON && state->enabledMask==3);
        bytes(capture,"forced-next-mon-upper.rgba",state->namePixels[0],sizeof(state->namePixels[0]));
        bytes(capture,"forced-next-mon-lower.rgba",state->namePixels[1],sizeof(state->namePixels[1]));
        forcedNextMonPrompt=TRUE; commandChoice(capture,cmd,0); forcedNextMonYes=TRUE; next(218); return;
    }
    switch (stage) {
    case 1: case 202: case 218:
        if (roster && party.mode==HG_BATTLE_PARTY_UI_SWITCH_NO_CANCEL) {
            QA_CHECK(forcedNextMonPrompt && forcedNextMonYes && forcedGrowls && party.partyCount==3 && party.mons[0].species==152 && !party.mons[0].hp
                && party.mons[1].species==175 && party.mons[1].isEgg && party.mons[2].species==155 && !party.mons[2].isEgg && party.mons[2].hp
                && q.semanticsMask==527 && !q.replyCount);
            forcedKO=TRUE; infoBefore=q; memcpy(forcedMons,party.mons,sizeof(forcedMons)); forcedRevision=party.revision;
            QA_CHECK(!(party.enabledMask&64) && !HGNativeBattlePartyUIResolveTouch(&party,party.revision,party.phase,6,&x,&y));
            HGNativeBattlePartyUIState forged=party; forged.enabledMask|=64;
            QA_CHECK(!HGNativeBattlePartyUIResolveTouch(&forged,forged.revision,forged.phase,6,&x,&y)); forcedTypedCancel=TRUE;
            if (!snapshot(capture,"forced-natural-ko-roster",TRUE)) break;
            QA_CHECK(party.buttons[6].hitX<256 && party.buttons[6].hitY<192);
            pulse(capture,party.buttons[6].hitX,party.buttons[6].hitY); next(203); break;
        }
        if (stage==218 || !command) break;
        if (stage==202 && (cmd.revision==forcedCommandRevision || q.totalTurns<=forcedTurnBefore)) break;
        QA_CHECK(q.partyCount==3 && q.enemySpecies==19 && q.enemyLevel==20 && !q.activePartySlot && q.playerHp && !q.replyCount);
        if (forcedGrowls>=12) {
            blockedStatus="prerequisite_natural_ko_not_observed"; finalQA=q; resetExpected=TRUE;
            w2c_env_HGNativeResetControl(capture->env,HG_RESET_REQUEST,0); QA_CHECK(FALSE);
        }
        forcedCommandRevision=cmd.revision; forcedTurnBefore=q.totalTurns; commandChoice(capture,cmd,0); next(201); break;
    case 201:
        if (!cmd.active || !cmd.ready || cmd.mode!=HG_BATTLE_UI_MOVES) break;
        QA_CHECK(cmd.move[1]==45 && cmd.pp[1]); ++forcedGrowls; commandChoice(capture,cmd,1); next(202); break;
    case 203:
        if (!roster || party.selectedSlot!=6) break;
        forcedStable(q,party); QA_CHECK(!q.replyCount && party.revision>forcedRevision); forcedActualCancel=TRUE;
        if (!snapshot(capture,"forced-cancel-refused",TRUE)) break;
        partyChoice(capture,party,0); next(204); break;
    case 204:
        if (!monMenu) break;
        forcedStable(q,party); QA_CHECK(!party.selectedSlot);
        partyChoice(capture,party,1); next(205); break;
    case 205: case 207:
        if (!party.active || !party.ready || party.phase!=HG_BATTLE_PARTY_UI_SUMMARY) break;
        forcedStable(q,party); QA_CHECK(!party.selectedSlot && party.page==2 && party.revision>forcedRevision);
        if (!snapshot(capture,stage==205 ? "forced-fainted-summary" : "forced-fainted-summary-return",TRUE)) break;
        forcedRevision=party.revision;
        partyChoice(capture,party,stage==205 ? 2 : 3); next(stage+1); break;
    case 206:
        if (!party.active || !party.ready || party.phase!=HG_BATTLE_PARTY_UI_CHECK_MOVES) break;
        forcedStable(q,party); QA_CHECK(!party.selectedSlot && party.page==3 && party.revision>forcedRevision);
        QA_CHECK(!HGNativeBattlePartyUIResolveTouch(&party,forcedRevision,party.phase,6,&x,&y));
        if (!snapshot(capture,"forced-fainted-moves",TRUE)) break;
        forcedRevision=party.revision; partyChoice(capture,party,6); next(207); break;
    case 208:
        if (!monMenu) break;
        forcedStable(q,party); QA_CHECK(!party.selectedSlot); forcedInfo=TRUE;
        partyChoice(capture,party,0); next(209); break;
    case 209: case 213:
        if (!party.active || !party.ready || party.phase!=HG_BATTLE_PARTY_UI_ACK) break;
        forcedStable(q,party); QA_CHECK(party.selectedSlot==(stage==209 ? 0 : 1) && !q.replyCount);
        if (!snapshot(capture,stage==209 ? "forced-fainted-shift-refused" : "forced-egg-shift-refused",TRUE)) break;
        if (stage==209) forcedFaintRejected=TRUE; else forcedEggRejected=TRUE;
        partyChoice(capture,party,0); next(stage+1); break;
    case 210: case 214:
        if (!monMenu) break;
        forcedStable(q,party); partyChoice(capture,party,3); next(stage+1); break;
    case 211: case 215:
        if (!roster) break;
        forcedStable(q,party); partyChoice(capture,party,stage==211 ? 1 : 2); next(stage+1); break;
    case 212:
        if (!monMenu) break;
        forcedStable(q,party); QA_CHECK(party.selectedSlot==1 && !(party.enabledMask&6)
            && !HGNativeBattlePartyUIResolveTouch(&party,party.revision,party.phase,1,&x,&y)
            && !HGNativeBattlePartyUIResolveTouch(&party,party.revision,party.phase,2,&x,&y));
        if (!snapshot(capture,"forced-egg-actions",TRUE)) break;
        forcedEggInfo=TRUE; partyChoice(capture,party,0); next(213); break;
    case 216:
        if (!monMenu) break;
        forcedStable(q,party); QA_CHECK(party.selectedSlot==2); forcedReplyBefore=q.replyCount;
        if (!snapshot(capture,"forced-reserve-actions",TRUE)) break;
        partyChoice(capture,party,0); next(217); break;
    case 217:
        if (q.replyCount>forcedReplyBefore) {
            QA_CHECK(q.replyCount==forcedReplyBefore+1 && q.replyTarget==3 && !q.replyItem && !q.replyPocket && q.replyHp && q.replyHp<=q.replyMaxHp);
            forcedReply=TRUE;
        }
        if (!forcedReply || !command || party.active) break;
        QA_CHECK(cmd.revision>forcedCommandRevision && q.activePartySlot==2 && q.semanticsMask==575 && q.battleActive && !q.returned && !q.lifecycleCleared);
        if (!snapshot(capture,"forced-reserve-original-command",TRUE)) break;
        forcedActive=TRUE; finalQA=q; resetExpected=TRUE; w2c_env_HGNativeResetControl(capture->env,HG_RESET_REQUEST,0); QA_CHECK(FALSE); break;
    default: QA_CHECK(FALSE);
    }
}
#endif
#ifdef HG_NATIVE_LEARN_MOVES_CHECK
static void learnState(HGNativeBattleMenusQAState q,BattleMenusCommand cmd,HGNativeBattlePartyUIState party,u32 frame) {
    const u32 values[]={stage,q.replyItem,q.replyPocket,q.semanticsMask,cmd.mode,cmd.ready,party.phase,party.ready,party.selectedMove,party.moveToLearn,
        q.replyCount,q.replyTarget,q.moves[0],q.moves[1],q.moves[2],q.moves[3],q.pp[0],q.pp[1],q.pp[2],q.pp[3]};
    if (learnStateSeen && !memcmp(values,learnPreviousState,sizeof(values))) return;
    QA_CHECK(learnStateChanges<128); memcpy(learnPreviousState,values,sizeof(values)); learnStateSeen=TRUE; ++learnStateChanges;
    printf("Battle menus learn moves state: {\"stage\":%u,\"frame\":%u,\"candidate\":%u,\"exp\":%u,\"semanticsMask\":%u,\"commandMode\":%u,\"commandReady\":%u,"
        "\"partyPhase\":%u,\"partyReady\":%u,\"selectedMove\":%u,\"moveToLearn\":%u,\"replyCount\":%u,\"replyTarget\":%u,\"moves\":[%u,%u,%u,%u],\"pp\":[%u,%u,%u,%u]}\n",
        stage,frame,q.replyItem,q.replyPocket,q.semanticsMask,cmd.mode,cmd.ready,party.phase,party.ready,party.selectedMove,party.moveToLearn,
        q.replyCount,q.replyTarget,q.moves[0],q.moves[1],q.moves[2],q.moves[3],q.pp[0],q.pp[1],q.pp[2],q.pp[3]);
}
static BOOL learnPrompt(Capture *capture,HGNativeBattleMenusQAState q,BattleMenusCommand cmd,u32 mode,u32 candidate,u32 choice,const char *name) {
    if (!cmd.active || !cmd.ready || cmd.mode!=mode || q.replyItem!=candidate) return FALSE;
    u32 address=w2c_hgengine_HGNativeBattleUIGetState(capture->game);
    const HGNativeBattleUIState *state=guest(capture,address,sizeof(*state));
    QA_CHECK(state->revision==cmd.revision && state->enabledMask==3 && !state->cancelRun);
    for (u32 i=0;i<4;++i) QA_CHECK(!state->move[i] && !state->pp[i] && !state->maxPP[i]);
    for (u32 i=0;i<2;++i) {
        QA_CHECK(state->nameWidth[i] && state->nameWidth[i]<=HG_BATTLE_NAME_WIDTH);
        BOOL visible=FALSE;
        for (u32 pixel=0;pixel<HG_BATTLE_NAME_WIDTH*HG_BATTLE_NAME_HEIGHT;++pixel) visible |= state->namePixels[i][pixel*4+3]!=0;
        QA_CHECK(visible);
    }
    if (!snapshot(capture,name,TRUE)) return FALSE;
    state=guest(capture,address,sizeof(*state)); QA_CHECK(state->revision==cmd.revision && state->ready && state->mode==mode);
    bytes(capture,"learn-prompt-upper.rgba",state->namePixels[0],sizeof(state->namePixels[0]));
    bytes(capture,"learn-prompt-lower.rgba",state->namePixels[1],sizeof(state->namePixels[1]));
    QA_CHECK(!w2c_hgengine_HGNativeBattleUIChoose(capture->game,cmd.revision,2));
    if (learnCommandRevision) {
        QA_CHECK(cmd.revision>learnCommandRevision && !w2c_hgengine_HGNativeBattleUIChoose(capture->game,learnCommandRevision,choice)); ++learnStale;
    }
    commandChoice(capture,cmd,choice);
    QA_CHECK(!w2c_hgengine_HGNativeBattleUIChoose(capture->game,cmd.revision,choice)); learnCommandRevision=cmd.revision;
    return TRUE;
}
static void learnPartyState(HGNativeBattleMenusQAState q,HGNativeBattlePartyUIState party,u32 phase,u32 selectedMove) {
    QA_CHECK(party.active && party.ready && party.mode==HG_BATTLE_PARTY_UI_LEARN_MOVE && party.phase==phase
        && party.page==(phase==HG_BATTLE_PARTY_UI_LEARN_LIST ? 6 : 7) && party.partyCount==1 && !party.selectedSlot
        && party.mons[0].species==455 && party.mons[0].level==31 && party.mons[0].partyIndex==0 && !party.mons[0].isEgg
        && !party.itemId && !party.pocket && party.moveToLearn==255 && q.replyItem==255 && q.replyPocket==37239);
    QA_CHECK(party.buttonCount==(phase==HG_BATTLE_PARTY_UI_LEARN_LIST ? 7 : 3) && party.enabledMask==(phase==HG_BATTLE_PARTY_UI_LEARN_LIST ? 0x5F : 5));
    for (u32 i=0;i<4;++i) QA_CHECK(party.moves[i].moveId==q.moves[i] && party.moves[i].pp==q.pp[i]);
    if (phase==HG_BATTLE_PARTY_UI_LEARN_DETAIL) QA_CHECK(party.selectedMove==selectedMove && party.message.pixelBuffer
        && party.message.tileWidth==15 && party.message.tileHeight==10 && party.message.engine==1);
}
static void learnPresent(Capture *capture,HGNativeBattleMenusQAState q,BattleMenusCommand cmd,HGNativeBattlePartyUIState party) {
    BOOL list=party.active && party.ready && party.phase==HG_BATTLE_PARTY_UI_LEARN_LIST;
    BOOL detail=party.active && party.ready && party.phase==HG_BATTLE_PARTY_UI_LEARN_DETAIL;
    if (q.returned) {
        blockedStatus="prerequisite_learning_battle_ended"; finalQA=q; resetExpected=TRUE;
        w2c_env_HGNativeResetControl(capture->env,HG_RESET_REQUEST,0); QA_CHECK(FALSE);
    }
    u32 x,y;
    switch (stage) {
    case 1:
        if (!cmd.active || !cmd.ready || cmd.mode!=HG_BATTLE_UI_COMMAND) break;
        QA_CHECK(q.semanticsMask==2063 && q.partyCount==1 && !q.activePartySlot && q.playerHp==q.playerMaxHp && q.playerHp
            && !q.playerStatus && q.potionQuantity==5 && q.enemySpecies==129 && q.enemyLevel==1 && q.enemyHp
            && q.moves[0]==22 && q.moves[1]==15 && q.moves[2]==74 && !q.moves[3] && q.pp[0]==24 && q.replyPocket==37237 && !q.replyCount);
        if (!snapshot(capture,"learn-original-fixture",TRUE)) break;
        learnFixture=TRUE; learnInitialExp=q.replyPocket; commandChoice(capture,cmd,0); next(300); break;
    case 300:
        if (!cmd.active || !cmd.ready || cmd.mode!=HG_BATTLE_UI_MOVES) break;
        QA_CHECK(cmd.move[0]==22 && cmd.pp[0]==24); commandChoice(capture,cmd,0); next(301); break;
    case 301:
        if (q.semanticsMask&0x100) {
            if (cmd.mode!=HG_BATTLE_UI_NONE || party.active) break;
            QA_CHECK(learnStatsPulses<2 && !q.enemyHp);
            QA_CHECK(!pulseFrame && HGNativeSetInput(capture->env,1,FALSE,0,0,FALSE));
            pulseFrame=OS_GetVBlankCount(); pulseReleased=FALSE; ++learnStatsPulses; break;
        }
        if (!cmd.active || !cmd.ready || cmd.mode!=HG_BATTLE_UI_KEEP_FORGET || q.replyItem!=255) break;
        QA_CHECK(learnStatsPulses==2 && q.semanticsMask==2207 && q.replyPocket==37239 && q.moves[3]==254 && q.pp[3]==20 && !q.enemyHp && !q.replyCount);
        if (!learnPrompt(capture,q,cmd,HG_BATTLE_UI_KEEP_FORGET,255,1,"learn-spit-up-keep")) break;
        learnExp=!!(q.semanticsMask&128); learnAuto=!!(q.semanticsMask&16); infoBefore=q; next(302); break;
    case 302:
        infoUnchanged(q);
        if (learnPrompt(capture,q,cmd,HG_BATTLE_UI_GIVE_UP,255,1,"learn-spit-up-continue")) next(303);
        break;
    case 303:
        infoUnchanged(q);
        if (learnPrompt(capture,q,cmd,HG_BATTLE_UI_KEEP_FORGET,255,0,"learn-spit-up-forget")) { learnKeepContinue=TRUE; next(304); }
        break;
    case 304:
        if (!list) break;
        infoUnchanged(q); learnPartyState(q,party,HG_BATTLE_PARTY_UI_LEARN_LIST,0);
        if (!snapshot(capture,"learn-spit-up-list",TRUE)) break;
        learnPartyRevision=party.revision; partyChoice(capture,party,1); next(305); break;
    case 305:
        if (!detail || party.selectedMove!=1 || party.revision==learnPartyRevision) break;
        infoUnchanged(q); learnPartyState(q,party,HG_BATTLE_PARTY_UI_LEARN_DETAIL,1);
        if (!snapshot(capture,"learn-cut-before-hm-rejection",TRUE)) break;
        learnPartyRevision=party.revision; partyChoice(capture,party,0); next(306); break;
    case 306:
        if (!detail || party.selectedMove!=1 || party.revision==learnPartyRevision) break;
        infoUnchanged(q); learnPartyState(q,party,HG_BATTLE_PARTY_UI_LEARN_DETAIL,1);
        QA_CHECK(!q.replyCount && q.moves[1]==15 && !HGNativeBattlePartyUIResolveTouch(&party,learnPartyRevision,party.phase,0,&x,&y)); ++learnStale;
        if (!snapshot(capture,"learn-original-hm-rejection",TRUE)) break;
        learnHM=TRUE; partyChoice(capture,party,2); next(307); break;
    case 307:
        if (!list) break;
        infoUnchanged(q); learnPartyState(q,party,HG_BATTLE_PARTY_UI_LEARN_LIST,0);
        learnPartyRevision=party.revision; partyChoice(capture,party,4); next(308); break;
    case 308:
        if (!detail || party.selectedMove!=4 || party.revision==learnPartyRevision) break;
        infoUnchanged(q); learnPartyState(q,party,HG_BATTLE_PARTY_UI_LEARN_DETAIL,4);
        QA_CHECK(!HGNativeBattlePartyUIResolveTouch(&party,learnPartyRevision,party.phase,0,&x,&y)); ++learnStale;
        if (!snapshot(capture,"learn-spit-up-fifth-cancel",TRUE)) break;
        partyChoice(capture,party,0); next(309); break;
    case 309:
        if (q.replyCount!=1 || q.replyTarget!=255) break;
        if (learnPrompt(capture,q,cmd,HG_BATTLE_UI_GIVE_UP,255,1,"learn-fifth-continue")) { learnFifth=TRUE; next(310); }
        break;
    case 310:
        if (learnPrompt(capture,q,cmd,HG_BATTLE_UI_KEEP_FORGET,255,0,"learn-retry-forget")) { learnContinueFifth=TRUE; next(311); }
        break;
    case 311:
        if (!list) break;
        learnPartyState(q,party,HG_BATTLE_PARTY_UI_LEARN_LIST,0); QA_CHECK(q.replyCount==1 && q.replyTarget==255);
        learnPartyRevision=party.revision; partyChoice(capture,party,0); next(312); break;
    case 312:
        if (!detail || party.selectedMove!=0 || party.revision==learnPartyRevision) break;
        learnPartyState(q,party,HG_BATTLE_PARTY_UI_LEARN_DETAIL,0); QA_CHECK(q.replyCount==1 && q.replyTarget==255 && party.moves[0].maxPP==24);
        if (!snapshot(capture,"learn-replace-vine-whip",TRUE)) break;
        partyChoice(capture,party,0); next(313); break;
    case 313:
        if (!cmd.active || !cmd.ready || cmd.mode!=HG_BATTLE_UI_KEEP_FORGET || q.replyItem!=256) break;
        QA_CHECK(q.semanticsMask==2239 && q.replyCount==2 && q.replyTarget==1 && q.moves[0]==255 && q.pp[0]==10 && q.moves[3]==254 && q.pp[3]==20);
        if (!learnPrompt(capture,q,cmd,HG_BATTLE_UI_KEEP_FORGET,256,1,"learn-swallow-keep")) break;
        learnReplace=TRUE; learnUps=!!(q.semanticsMask&32); next(314); break;
    case 314:
        if (learnPrompt(capture,q,cmd,HG_BATTLE_UI_GIVE_UP,256,0,"learn-swallow-final-give-up")) next(315);
        break;
    case 315:
        if (q.semanticsMask!=2303) break;
        QA_CHECK(q.replyItem==256 && q.replyPocket==37239 && q.replyCount==2 && q.replyTarget==1
            && q.moves[0]==255 && q.moves[1]==15 && q.moves[2]==74 && q.moves[3]==254 && q.pp[0]==10 && q.pp[3]==20
            && q.battleActive && !q.returned && !q.lifecycleCleared);
        if (!snapshot(capture,"learn-complete-before-settlement",TRUE)) break;
        learnGiveUp=!!(q.semanticsMask&64); learnSaved=!!(q.semanticsMask&2); finalQA=q; resetExpected=TRUE;
        w2c_env_HGNativeResetControl(capture->env,HG_RESET_REQUEST,0); QA_CHECK(FALSE); break;
    default: QA_CHECK(FALSE);
    }
}
#endif
static void MenusPresent(void *argument) {
    Capture *capture=argument; u32 frame=OS_GetVBlankCount();
    if (!stage) {
        present(capture); HGNativeBattleMenusQAState q=readQA(capture);
        if (!q.fieldReady || !snapshot(capture,"field-before",FALSE)) return;
        capture->noInput=TRUE; QA_CHECK(HGNativeSetInput(capture->env,0,FALSE,0,0,FALSE));
        QA_CHECK(w2c_hgengine_HGNativeBattleMenusQARequest(capture->game,q.revision));
        QA_CHECK(!w2c_hgengine_HGNativeBattleMenusQARequest(capture->game,q.revision)); next(1); return;
    }
    QA_CHECK(HGNativeSoundRealtime(capture->env,TRUE));
    if (frame-stageFrame>=7200) {
        blockedStatus="timeout_waiting_original_scene"; finalQA=readQA(capture); resetExpected=TRUE;
        w2c_env_HGNativeResetControl(capture->env,HG_RESET_REQUEST,0); QA_CHECK(FALSE);
    }
#ifdef HG_NATIVE_FIELD_POTION_CHECK
    // Observe the original HP draw and printer even during the touch release/cooldown handshake.
    if (stage==127 || stage==131) fieldObserveUse(readFieldParty(capture));
#endif
    if (pulseFrame) {
        if (!pulseReleased && frame-pulseFrame>=8) { QA_CHECK(HGNativeSetInput(capture->env,0,FALSE,0,0,FALSE)); pulseReleased=TRUE; }
        if (frame-pulseFrame<12) return;
        pulseFrame=0;
    }
    HGNativeBattleMenusQAState q=readQA(capture); BattleMenusCommand cmd=readCommand(capture);
    HGNativeBattleBagUIState bag=readBag(capture); HGNativeBattlePartyUIState party=readParty(capture);
#ifdef HG_BATTLE_MENUS_INFO_NAV
    navigationPresent(capture,q,cmd,bag,party); return;
#endif
#ifdef HG_NATIVE_FORCED_SWITCH_CHECK
    forcedDiagnostic(q,cmd,party,frame); forcedPresent(capture,q,cmd,party); return;
#endif
#ifdef HG_NATIVE_LEARN_MOVES_CHECK
    learnState(q,cmd,party,frame); learnPresent(capture,q,cmd,party); return;
#endif
#ifdef HG_NATIVE_FIELD_POTION_CHECK
    if (stage>=120 && stage<=133) { fieldPotionPresent(capture,q); return; }
#endif
    if (q.returned && q.outcome!=5) {
        blockedStatus="prerequisite_original_battle_ended_otherwise"; finalQA=q; resetExpected=TRUE;
        w2c_env_HGNativeResetControl(capture->env,HG_RESET_REQUEST,0); QA_CHECK(FALSE);
    }
    BOOL command=cmd.active && cmd.ready && cmd.mode==HG_BATTLE_UI_COMMAND;
    BOOL pockets=bag.active && bag.ready && bag.phase==HG_BATTLE_BAG_UI_POCKETS;
    BOOL items=bag.active && bag.ready && bag.phase==HG_BATTLE_BAG_UI_ITEMS;
    BOOL use=bag.active && bag.ready && bag.phase==HG_BATTLE_BAG_UI_USE;
    BOOL roster=party.active && party.ready && party.phase==HG_BATTLE_PARTY_UI_ROSTER;
    BOOL monMenu=party.active && party.ready && party.phase==HG_BATTLE_PARTY_UI_MON_MENU;
    BOOL ack=party.active && party.ready && party.phase==HG_BATTLE_PARTY_UI_ACK;
    BOOL summary=party.active && party.ready && party.phase==HG_BATTLE_PARTY_UI_SUMMARY;
    BOOL moves=party.active && party.ready && party.phase==HG_BATTLE_PARTY_UI_CHECK_MOVES;
    BOOL detail=party.active && party.ready && party.phase==HG_BATTLE_PARTY_UI_MOVE_DETAIL;
    if ((stage==8 || stage==17) && party.active && party.phase==HG_BATTLE_PARTY_UI_MESSAGE && !busyCaptured) {
        u32 x,y; QA_CHECK(!HGNativeBattlePartyUIResolveTouch(&party,party.revision,party.phase,0,&x,&y));
        if (snapshot(capture,"read-only-message",TRUE)) busyCaptured=TRUE;
    }
    switch (stage) {
    case 1:
        if (!command) break;
        sceneBlocked=q.partyCount!=1 || q.playerHp!=20 || q.playerMaxHp!=20 || q.playerStatus || q.potionQuantity!=5
            || q.moves[0]!=33 || q.moves[1]!=45 || q.moves[2] || q.moves[3] || q.pp[1]<NATURAL_DAMAGE_GROWL_LIMIT || q.enemySpecies!=10 || q.enemyLevel!=1;
        if (sceneBlocked) { originalRun(capture,cmd); next(30); break; }
        baselineTurns=q.totalTurns; initialGrowlPP=q.pp[1]; QA_CHECK(!baselineTurns); commandChoice(capture,cmd,1); next(2); break;
    case 2:
        if (!pockets || !snapshot(capture,"bag-first",TRUE)) break;
        firstBagRevision=bag.revision; bagChoice(capture,bag,5); next(3); break;
    case 3: if (command) { commandChoice(capture,cmd,1); next(4); } break;
    case 4:
        if (!pockets) break;
        { u32 x,y; QA_CHECK(bag.revision>firstBagRevision && !HGNativeBattleBagUIResolveTouch(&bag,firstBagRevision,bag.phase,0,&x,&y)); ++staleRejected; }
        if (!snapshot(capture,"bag-reopened",TRUE)) break;
        bagChoice(capture,bag,0); next(5); break;
    case 5: if (items && snapshot(capture,"potion-list",TRUE)) { bagChoice(capture,bag,potionRow(bag)); next(6); } break;
    case 6: if (use && snapshot(capture,"potion-use",TRUE)) { bagChoice(capture,bag,0); next(7); } break;
    case 7:
        if (!roster) break;
        QA_CHECK(party.mode==HG_BATTLE_PARTY_UI_USE_ITEM && party.itemId==17 && party.mons[0].hp==20 && party.mons[0].maxHp==20);
        if (!snapshot(capture,"full-hp-target",TRUE)) break;
        firstTargetRevision=party.revision; fullReplyCount=q.replyCount; partyChoice(capture,party,0); next(8); break;
    case 8: if (ack && snapshot(capture,"full-hp-no-effect",TRUE)) { partyChoice(capture,party,0); next(9); } break;
    case 9:
        if (!pockets) break;
        QA_CHECK(q.replyCount==fullReplyCount && q.totalTurns==baselineTurns); fullHpNoEffect=TRUE; bagChoice(capture,bag,0); next(10); break;
    case 10: if (items) { bagChoice(capture,bag,potionRow(bag)); next(11); } break;
    case 11: if (use) { bagChoice(capture,bag,0); next(12); } break;
    case 12:
        if (!roster) break;
        { u32 x,y; QA_CHECK(party.mode==HG_BATTLE_PARTY_UI_USE_ITEM && party.revision>firstTargetRevision && !HGNativeBattlePartyUIResolveTouch(&party,firstTargetRevision,party.phase,0,&x,&y)); ++staleRejected; }
        if (!snapshot(capture,"target-cancel",TRUE)) break;
        partyChoice(capture,party,6); next(13); break;
    case 13:
        if (!pockets) break;
        QA_CHECK(q.replyCount==fullReplyCount && q.totalTurns==baselineTurns); targetCancel=TRUE; bagChoice(capture,bag,5); next(14); break;
    case 14: if (command) { commandChoice(capture,cmd,2); next(15); } break;
    case 15:
        if (!roster) break;
        QA_CHECK(party.mode==HG_BATTLE_PARTY_UI_SWITCH); if (!snapshot(capture,"switch-roster",TRUE)) break;
        partyChoice(capture,party,0); next(16); break;
    case 16:
        if (!monMenu || !snapshot(capture,"switch-actions",TRUE)) break;
        if (!infoComplete) {
            infoBefore=q; memcpy(infoMons,party.mons,sizeof(infoMons)); infoPreviousRevision=party.revision;
            QA_CHECK(party.partyCount==1 && !party.selectedSlot && !party.mons[0].isEgg
                && party.mons[0].hp==q.playerHp && party.mons[0].maxHp==q.playerMaxHp);
            bytes(capture,"info-qa-before.bin",&q,sizeof(q)); partyChoice(capture,party,1); next(40); break;
        }
        partyChoice(capture,party,0); next(17); break;
    case 17: if (ack && snapshot(capture,"shift-rejected",TRUE)) { partyChoice(capture,party,0); next(18); } break;
    case 18:
        if (!monMenu) break;
        QA_CHECK(q.totalTurns==baselineTurns); shiftRejected=TRUE; partyChoice(capture,party,3); next(19); break;
    case 19: if (roster) { partyChoice(capture,party,6); next(20); } break;
    case 20:
        if (!command) break;
        QA_CHECK(fullHpNoEffect && targetCancel && shiftRejected && infoComplete && q.potionQuantity==5 && q.totalTurns==baselineTurns); baselineComplete=TRUE;
        turnBefore=q.totalTurns; commandChoice(capture,cmd,0); next(21); break;
    case 21:
        if (!cmd.active || !cmd.ready || cmd.mode!=HG_BATTLE_UI_MOVES) break;
        QA_CHECK(cmd.move[1]==45 && cmd.pp[1] && growls<NATURAL_DAMAGE_GROWL_LIMIT); ++growls; commandChoice(capture,cmd,1); next(22); break;
    case 22:
        if (!command || q.totalTurns<=turnBefore) break;
        QA_CHECK(q.totalTurns==turnBefore+1 && q.playerHp && q.playerHp<=20 && q.potionQuantity==5);
        if (!snapshot(capture,"natural-turn",TRUE)) break;
        if (q.playerHp<q.playerMaxHp) {
#ifdef HG_NATIVE_FIELD_POTION_CHECK
            originalRun(capture,cmd); next(30);
#else
            commandChoice(capture,cmd,1); next(23);
#endif
        }
        else if (growls<NATURAL_DAMAGE_GROWL_LIMIT) { turnBefore=q.totalTurns; commandChoice(capture,cmd,0); next(21); }
        else { damageMissing=TRUE; originalRun(capture,cmd); next(30); }
        break;
    case 23: if (pockets) { bagChoice(capture,bag,0); next(24); } break;
    case 24: if (items) { bagChoice(capture,bag,potionRow(bag)); next(25); } break;
    case 25: if (use) { bagChoice(capture,bag,0); next(26); } break;
    case 26:
        if (!roster) break;
        QA_CHECK(party.mode==HG_BATTLE_PARTY_UI_USE_ITEM && party.itemId==17 && party.mons[0].hp && party.mons[0].hp<20 && party.mons[0].maxHp==20);
        if (!snapshot(capture,"injured-potion-target",TRUE)) break;
        preHealHp=party.mons[0].hp; preHealReplies=q.replyCount; partyChoice(capture,party,0); next(28); break;
    case 28:
        if (q.replyCount<=preHealReplies) break;
        QA_CHECK(q.replyItem==17 && q.replyPocket==0 && q.replyTarget==1 && q.replyHp==20 && q.replyMaxHp==20 && q.replyPotionQuantity==4 && preHealHp<q.replyHp); healed=TRUE;
        if (!command) break;
        if (!snapshot(capture,"command-after-item-turn",TRUE)) break;
        originalRun(capture,cmd); next(30); break;
    case 30:
        if (!q.returned && command && cmd.revision!=lastRunRevision) {
            ++fleeFailures;
            if (runAttempts>=6) {
                blockedStatus="run_escape_not_observed"; finalQA=q; resetExpected=TRUE;
                w2c_env_HGNativeResetControl(capture->env,HG_RESET_REQUEST,0); QA_CHECK(FALSE);
            }
            originalRun(capture,cmd); stageFrame=frame; break;
        }
        if (!q.returned || !q.fieldReady || cmd.active || bag.active || party.active) break;
        QA_CHECK(q.outcome==5 && q.battleResult==5 && q.lifecycleCleared && !q.battleActive && q.semanticsMask==63);
        if (baselineComplete) QA_CHECK(q.pp[1]<=initialGrowlPP && initialGrowlPP-q.pp[1]==growls);
        { HGNativeBattleBagUIState a=bag; HGNativeBattlePartyUIState b=party; a.revision=b.revision=0; HGNativeBattleBagUIState z={0}; HGNativeBattlePartyUIState w={0}; QA_CHECK(!memcmp(&a,&z,sizeof(a)) && !memcmp(&b,&w,sizeof(b))); }
        if (!snapshot(capture,"field-return",FALSE)) break;
#ifdef HG_NATIVE_FIELD_POTION_CHECK
        if (baselineComplete && !damageMissing && !sceneBlocked) {
            u32 save=w2c_hgengine_SaveData_Get(capture->game); guest(capture,save,1);
            fieldPartyAddress=w2c_hgengine_SaveArray_Party_Get(capture->game,save); fieldBagAddress=w2c_hgengine_Save_Bag_Get(capture->game,save);
            fieldFlagsAddress=w2c_hgengine_Save_VarsFlags_Get(capture->game,save); fieldDexAddress=w2c_hgengine_Save_Pokedex_Get(capture->game,save);
            guest(capture,fieldPartyAddress,1); guest(capture,fieldBagAddress,1); guest(capture,fieldFlagsAddress,1); guest(capture,fieldDexAddress,1);
            u32 mon=w2c_hgengine_Party_GetMonByIndex(capture->game,fieldPartyAddress,0); guest(capture,mon,1);
            u32 hp=w2c_hgengine_GetMonData(capture->game,mon,MON_DATA_HP,0);
            if (!hp || hp>=20) {
                blockedStatus="prerequisite_live_field_damage_not_observed"; finalQA=q; resetExpected=TRUE;
                w2c_env_HGNativeResetControl(capture->env,HG_RESET_REQUEST,0); QA_CHECK(FALSE);
            }
            fieldSemantic(capture,"post-battle-baseline",TRUE,FALSE); next(120); break;
        }
#endif
        fieldReturned=TRUE; finalQA=q; resetExpected=TRUE; w2c_env_HGNativeResetControl(capture->env,HG_RESET_REQUEST,0); QA_CHECK(FALSE); break;
    case 40:
        if (!summary || !snapshot(capture,"party-summary",TRUE)) break;
        infoState(q,party,HG_BATTLE_PARTY_UI_SUMMARY,2,0); partyChoice(capture,party,2); next(41); break;
    case 41:
        if (!moves || !snapshot(capture,"party-check-moves",TRUE)) break;
        infoState(q,party,HG_BATTLE_PARTY_UI_CHECK_MOVES,3,0); partyChoice(capture,party,0); next(42); break;
    case 42:
        if (!detail || !snapshot(capture,"party-detail-tackle",TRUE)) break;
        infoState(q,party,HG_BATTLE_PARTY_UI_MOVE_DETAIL,4,0); partyChoice(capture,party,1); next(43); break;
    case 43:
        if (!detail || party.selectedMove!=1 || party.revision==infoPreviousRevision || !snapshot(capture,"party-detail-growl",TRUE)) break;
        infoState(q,party,HG_BATTLE_PARTY_UI_MOVE_DETAIL,4,1); partyChoice(capture,party,0); next(44); break;
    case 44:
        if (!detail || party.selectedMove!=0 || party.revision==infoPreviousRevision || !snapshot(capture,"party-detail-tackle-return",TRUE)) break;
        infoState(q,party,HG_BATTLE_PARTY_UI_MOVE_DETAIL,4,0);
        QA_CHECK(infoBankRoundTrip && infoDescriptionMask==3); partyChoice(capture,party,4); next(45); break;
    case 45:
        if (!moves || !snapshot(capture,"party-check-moves-return",TRUE)) break;
        infoState(q,party,HG_BATTLE_PARTY_UI_CHECK_MOVES,3,0); partyChoice(capture,party,6); next(46); break;
    case 46:
        if (!summary || !snapshot(capture,"party-summary-return",TRUE)) break;
        infoState(q,party,HG_BATTLE_PARTY_UI_SUMMARY,2,0); partyChoice(capture,party,3); next(47); break;
    case 47:
        if (!monMenu || !snapshot(capture,"party-info-return",TRUE)) break;
        infoUnchanged(q);
        { u32 x,y; QA_CHECK(party.revision>infoPreviousRevision && !HGNativeBattlePartyUIResolveTouch(&party,infoPreviousRevision,party.phase,0,&x,&y)); ++infoStaleRejected; }
        QA_CHECK(party.page==1 && party.mode==HG_BATTLE_PARTY_UI_SWITCH && !party.selectedSlot
            && !memcmp(party.mons,infoMons,sizeof(infoMons)) && infoPages==28 && infoStaleRejected==8);
        bytes(capture,"info-qa-after.bin",&q,sizeof(q)); infoComplete=TRUE; next(16); break;
    default: QA_CHECK(FALSE);
    }
}

int main(int argc,char **argv) {
    QA_CHECK(!setvbuf(stdout,NULL,_IOLBF,0));
#ifdef HG_NATIVE_FIELD_POTION_CHECK
    if (argc!=19 || !HGNativeFSSetRoot(argv[1]) || !HGNativeBackupSetPath(argv[3])) return 2;
    for (u32 i=0;i<14;++i) { char *end; unsigned long value=strtoul(argv[5+i],&end,10); if (!*argv[5+i] || *end || value>=(i%2 ? 192u : 256u)) return 2; fieldIconCenters[i/2][i%2]=(u32)value; }
#else
    if (argc!=5 || !HGNativeFSSetRoot(argv[1]) || !HGNativeBackupSetPath(argv[3])) return 2;
#endif
    u8 header[0x160]; FILE *input=fopen(argv[2],"rb"); QA_CHECK(input);
    QA_CHECK(fread(header,1,sizeof(header),input)==sizeof(header) && fgetc(input)==EOF && !ferror(input)); QA_CHECK(!fclose(input));
    wasm_rt_init(); w2c_hgengine *game=calloc(1,sizeof(*game)); struct w2c_env *env=calloc(1,sizeof(*env));
    struct w2c_wasi__snapshot__preview1 *wasi=calloc(1,sizeof(*wasi)); Capture *capture=calloc(1,sizeof(*capture));
    QA_CHECK(game && env && wasi && capture); for (u32 i=0;i<3;i++) { wasi->fds[i]=dup(i); QA_CHECK(wasi->fds[i]>=0); } menusEnv=env;
    if (!setjmp(resetLanding)) {
        wasm_rt_trap_t trap=wasm_rt_impl_try(); QA_CHECK(!trap);
        wasm2c_hgengine_instantiate(game,env,wasi); env->memory=wasi->memory=w2c_hgengine_memory(game); QA_CHECK(HGNativeSoundRealtime(env,TRUE));
        env->irqTable=&game->w2c_T0; env->stackPointer=&game->w2c_0x5F_stack_pointer; env->rootStackHigh=*env->stackPointer; *env->stackPointer-=16;
        env->irqType0=wasm2c_hgengine_get_func_type(0,0); env->irqType1=wasm2c_hgengine_get_func_type(1,0,WASM_RT_I32);
        env->rtcType=wasm2c_hgengine_get_func_type(2,0,WASM_RT_I32,WASM_RT_I32);
        env->irqGuardType=wasm2c_hgengine_get_func_type(4,0,WASM_RT_I32,WASM_RT_I32,WASM_RT_I32,WASM_RT_I32);
        u32 work=w2c_hgengine_HGNativeSystemWorkAddress(game); guest(&(Capture){.env=env},work,0x360);
        memcpy(env->memory->data+work+0x200,header,sizeof(header)); w2c_hgengine_0x5Finitialize(game);
        u32 display=w2c_hgengine_HGNativeRegisterAddress(game,0x04000004,2); QA_CHECK(!(display&1)); guest(&(Capture){.env=env},display,2);
        QA_CHECK(HGNativeStartDisplayClock(env,(const vu16 *)(env->memory->data+display)));
        *capture=(Capture){.game=game,.env=env,.folder=argv[4],.resume=TRUE};
        QA_CHECK(HGNativeSetDisplayObserver(env,MenusPresent,capture)); w2c_hgengine_NitroMain(game); QA_CHECK(FALSE);
    }
    QA_CHECK(resetExpected && env->resetRequested); HGNativeFinishReset(env);
    QA_CHECK(env->resetFinished && !env->pendingDMA && !env->dmaAffineWrites && !env->pendingRTC && !env->pendingMIC && !env->pendingCARD && !env->pendingPOWER && !env->pendingINPUT && !env->irqTable && !env->threads && !env->rasterFunction);
#if defined(HG_NATIVE_LEARN_MOVES_CHECK)
    BOOL passed=learnFixture && learnExp && learnAuto && learnKeepContinue && learnHM && learnFifth && learnContinueFifth
        && learnReplace && learnUps && learnGiveUp && learnSaved && learnStale>=2 && !blockedStatus;
    const char *status=blockedStatus ? blockedStatus : passed ? "passed_private_original_learn_moves" : "actual_assertion_failed";
    printf("Battle menus learn moves evidence: {\"fixtureReady\":%u,\"originalExpLevelUp\":%u,\"autoStockpile\":%u,\"keepContinue\":%u,\"hmRejected\":%u,\"fifthCancelled\":%u,"
        "\"continueAfterFifth\":%u,\"replacementCommitted\":%u,\"ppUpsReset\":%u,\"finalGiveUp\":%u,\"savedPartyBagUnchanged\":%u,\"staleRejected\":%u,\"staleRejections\":%u,"
        "\"semanticsMask\":%u,\"initialEXP\":%u,\"finalEXP\":%u,\"partyReplies\":%u,\"finalReply\":%u,\"finalCandidate\":%u,\"battleActiveBeforeReset\":%u,\"returnedBeforeReset\":%u,\"lifecycleClearedBeforeReset\":%u,"
        "\"stateChanges\":%u,\"statAdvancePulses\":%u,\"qaBytes\":%zu,\"partyBytes\":%zu,\"normalBattleTeardown\":false,\"resultsCopiedToSave\":false,\"resetFinished\":true,\"pendingDMA\":0}\n",
        learnFixture,learnExp,learnAuto,learnKeepContinue,learnHM,learnFifth,learnContinueFifth,learnReplace,learnUps,learnGiveUp,learnSaved,learnStale>=2,learnStale,
        finalQA.semanticsMask,learnInitialExp,finalQA.replyPocket,finalQA.replyCount,finalQA.replyTarget,finalQA.replyItem,finalQA.battleActive,finalQA.returned,finalQA.lifecycleCleared,
        learnStateChanges,learnStatsPulses,sizeof(finalQA),sizeof(HGNativeBattlePartyUIState));
    printf("Battle menus evidence: {\"status\":\"%s\",\"flowComplete\":%u,\"snapshots\":%u,\"resetFinished\":true,\"pendingDMA\":0}\n",status,passed,snapshots);
#elif defined(HG_NATIVE_FIELD_POTION_CHECK)
    BOOL passed=baselineComplete && infoComplete && infoBankRoundTrip && infoStaleRejected==8 && fieldTargetCancelled && fieldCancelNewBag
        && fieldHealed && fieldHealNewBag && fieldFullHp && fieldFullHpNewBag && fieldStale && fieldQuantityExact && fieldSemanticStable
        && fieldProgress && fieldResultWaitMask==3 && fieldReturned && !sceneBlocked && !damageMissing && !blockedStatus;
    const char *status=blockedStatus ? blockedStatus : sceneBlocked ? "scene_prerequisite_blocked" : damageMissing ? "prerequisite_damage_not_observed" : passed ? "passed_original_field_potion" : "actual_assertion_failed";
    printf("Battle menus field Potion evidence: {\"targetCancel\":%u,\"cancelNewBag\":%u,\"heal\":%u,\"healNewBag\":%u,\"fullHpNoEffect\":%u,\"fullHpNewBag\":%u,\"quantityExact\":%u,\"staleRejected\":%u,\"fieldReturned\":%u,\"otherAttributesFlagsDexBagUnchanged\":%u,\"progressObserved\":%u,\"resultWaitObserved\":%u,\"resultWaitMask\":%u,\"resultStateChanges\":%u,\"preHealHp\":%u,\"battleOutcome\":%u,\"semanticsMask\":%u,\"baselineAfterBattle\":%s,\"resetFinished\":true,\"pendingDMA\":0}\n",
        fieldTargetCancelled,fieldCancelNewBag,fieldHealed,fieldHealNewBag,fieldFullHp,fieldFullHpNewBag,fieldQuantityExact,fieldStale,fieldReturned,fieldSemanticStable,!!fieldProgress,fieldResultWaitMask==3,fieldResultWaitMask,fieldUseStateCount,fieldPreHp,finalQA.outcome,finalQA.semanticsMask,
        fieldOriginalBag && fieldExpectedBag && fieldBagSize && fieldPreHp && fieldPreHp<20 ? "true" : "false");
    printf("Battle menus evidence: {\"status\":\"%s\",\"flowComplete\":%u,\"snapshots\":%u,\"resetFinished\":true,\"pendingDMA\":0}\n",status,passed,snapshots);
#elif defined(HG_NATIVE_FORCED_SWITCH_CHECK)
    BOOL passed=forcedNextMonPrompt && forcedNextMonYes && forcedKO && forcedTypedCancel && forcedActualCancel && forcedInfo && forcedFaintRejected && forcedEggInfo && forcedEggRejected && forcedReply && forcedActive && !blockedStatus;
    const char *status=blockedStatus ? blockedStatus : passed ? "passed_private_original_forced_switch" : "actual_assertion_failed";
    printf("Battle menus forced switch evidence: {\"nextMonPromptObserved\":%u,\"nextMonTypedYes\":%u,\"nextMonNameWidths\":[%u,%u],\"fleeBranchExercised\":false,\"naturalKO\":%u,\"typedCancelRejected\":%u,\"actualCancelRejected\":%u,\"faintedInfoRoundTrip\":%u,\"faintedShiftRejected\":%u,\"eggInfoDisabled\":%u,\"eggShiftRejected\":%u,\"reserveReplyObserved\":%u,\"reserveActiveObserved\":%u,\"semanticsMask\":%u,\"activeSlot\":%u,\"replyTarget\":%u,\"growlChoices\":%u,\"normalBattleTeardown\":false,\"resultsCopiedToSave\":false,\"resetFinished\":true,\"pendingDMA\":0}\n",
        forcedNextMonPrompt,forcedNextMonYes,forcedNextMonNameWidth[0],forcedNextMonNameWidth[1],forcedKO,forcedTypedCancel,forcedActualCancel,forcedInfo,forcedFaintRejected,forcedEggInfo,forcedEggRejected,forcedReply,forcedActive,finalQA.semanticsMask,finalQA.activePartySlot,finalQA.replyTarget,forcedGrowls);
    printf("Battle menus evidence: {\"status\":\"%s\",\"flowComplete\":%u,\"snapshots\":%u,\"resetFinished\":true,\"pendingDMA\":0}\n",status,passed,snapshots);
#elif defined(HG_BATTLE_MENUS_INFO_NAV)
    BOOL passed=navComplete && navBrowsingStable && navVisits==10 && navStaleRejected==8 && navNameDistinctPages==3 && navMoveSeen==3 && !blockedStatus;
    const char *status=blockedStatus ? blockedStatus : passed ? "passed_private_battle_party_navigation" : "actual_assertion_failed";
    printf("Battle menus navigation evidence: {\"complete\":%u,\"visits\":%u,\"samePhaseStaleRejected\":%u,\"nameDistinctPages\":%u,\"browsingQAUnchanged\":%u,\"commandReopenIncrement\":%u,\"semanticsMask\":%u,\"syntheticBattlePartyCount\":%u,\"loadedSavePartyCount\":1,\"battleActiveBeforeReset\":%u,\"turnsBeforeReset\":%u,\"returnedBeforeReset\":%u,\"lifecycleClearedBeforeReset\":%u,\"normalBattleTeardown\":false,\"resultsCopiedToSave\":false,\"qaBytes\":%zu}\n",
        navComplete,navVisits,navStaleRejected,navNameDistinctPages,navBrowsingStable,finalQA.commandCount-infoBefore.commandCount,finalQA.semanticsMask,finalQA.partyCount,finalQA.battleActive,finalQA.totalTurns,finalQA.returned,finalQA.lifecycleCleared,sizeof(finalQA));
    printf("Battle menus evidence: {\"status\":\"%s\",\"flowComplete\":%u,\"snapshots\":%u,\"resetFinished\":true,\"pendingDMA\":0}\n",status,navComplete,snapshots);
#else
    BOOL passed=baselineComplete && infoComplete && infoBankRoundTrip && infoStaleRejected==8 && healed && fieldReturned
        && !sceneBlocked && !damageMissing && !blockedStatus && staleRejected>=2;
    const char *status=blockedStatus ? blockedStatus : sceneBlocked ? "scene_prerequisite_blocked" : damageMissing ? "prerequisite_damage_not_observed" : passed ? "passed_original_battle_menus_and_heal" : "actual_assertion_failed";
    printf("Battle menus flee evidence: {\"attempts\":%u,\"failures\":%u,\"actualOutcome\":%u,\"returned\":%u}\n",runAttempts,fleeFailures,finalQA.outcome,finalQA.returned);
    printf("Battle menus info evidence: {\"complete\":%u,\"pagesMask\":%u,\"availableMoveMask\":%u,\"staleRejected\":%u,\"descriptionBankRoundTrip\":%u,\"qaBytes\":%zu,\"realFixtureMonCount\":%u,\"multiMonOrEggNavigationCovered\":false}\n",
        infoComplete,infoPages,infoDescriptionMask,infoStaleRejected,infoBankRoundTrip,sizeof(infoBefore),infoBefore.partyCount);
    printf("Battle menus evidence: {\"status\":\"%s\",\"baselineCancel\":%u,\"fullHpNoEffect\":%u,\"targetCancel\":%u,\"shiftRejected\":%u,\"staleRejected\":%u,\"growlTurns\":%u,\"preHealHp\":%u,\"healGate\":%u,\"replyHp\":%u,\"replyMaxHp\":%u,\"replyPotionQuantity\":%u,\"lastCommandHp\":%u,\"totalTurns\":%u,\"returned\":%u,\"outcome\":%u,\"semanticsMask\":%u,\"lifecycleCleared\":%u,\"snapshots\":%u,\"flowComplete\":%u,\"resetFinished\":true,\"pendingDMA\":0}\n",status,baselineComplete,fullHpNoEffect,targetCancel,shiftRejected,staleRejected,growls,preHealHp,healed,finalQA.replyHp,finalQA.replyMaxHp,finalQA.replyPotionQuantity,finalQA.playerHp,finalQA.totalTurns,finalQA.returned,finalQA.outcome,finalQA.semanticsMask,finalQA.lifecycleCleared,snapshots,fieldReturned);
#endif
    bytes(capture,"final-qa.bin",&finalQA,sizeof(finalQA));
#ifdef HG_NATIVE_FIELD_POTION_CHECK
    free(fieldOriginalBag); free(fieldExpectedBag);
#endif
    wasm2c_hgengine_free(game); for (u32 i=0;i<3;i++) close(wasi->fds[i]); free(game); free(env); free(wasi); free(capture); menusEnv=NULL; wasm_rt_free();
    return passed ? 0 : sceneBlocked || damageMissing || blockedStatus ? 3 : 1;
}

#ifdef HG_NATIVE_FORCED_SWITCH_CHECK
// HG_FORCED_DIAGNOSTIC_BEGIN
static u32 forcedDiagnosticCount,forcedDiagnosticOverflow,forcedDiagnosticPrevious[25];
static BOOL forcedDiagnosticSeen;
static void forcedDiagnostic(HGNativeBattleMenusQAState q,BattleMenusCommand cmd,HGNativeBattlePartyUIState party,u32 frame) {
    const u32 values[]={stage,cmd.active,cmd.ready,cmd.mode,party.active,party.ready,party.mode,party.page,party.phase,party.enabledMask,
        q.started,q.requested,q.battleActive,q.playerHp,q.totalTurns,q.semanticsMask,q.errorLine,q.fieldReady,q.commandCount,q.partyCount,
        forcedGrowls,q.enemySpecies,q.enemyLevel,cmd.enabledMask,cmd.cancelRun};
    if (forcedDiagnosticSeen && !memcmp(values,forcedDiagnosticPrevious,sizeof(values))) return;
    memcpy(forcedDiagnosticPrevious,values,sizeof(values)); forcedDiagnosticSeen=TRUE;
    if (forcedDiagnosticCount==128) {
        ++forcedDiagnosticOverflow;
        if (forcedDiagnosticOverflow==1) {
            puts("Forced diagnostic overflow: {\"limit\":128,\"droppedChangesAtLeast\":1,\"affectsVerdict\":false}"); fflush(stdout);
        }
        return;
    }
    printf("Forced diagnostic: {\"sample\":%u,\"overflow\":%u,\"frame\":%u,\"stage\":%u,\"growls\":%u,"
        "\"cmd\":{\"active\":%u,\"ready\":%u,\"mode\":%u,\"enabledMask\":%u,\"cancelRun\":%u},\"party\":{\"active\":%u,\"ready\":%u,\"mode\":%u,\"page\":%u,\"phase\":%u,\"mask\":%u},"
        "\"q\":{\"started\":%u,\"requested\":%u,\"battleActive\":%u,\"playerHp\":%u,\"totalTurns\":%u,\"semanticsMask\":%u,\"errorLine\":%u,"
        "\"fieldReady\":%u,\"commandCount\":%u,\"partyCount\":%u,\"enemySpecies\":%u,\"enemyLevel\":%u}}\n",
        ++forcedDiagnosticCount,forcedDiagnosticOverflow,frame,stage,forcedGrowls,cmd.active,cmd.ready,cmd.mode,cmd.enabledMask,cmd.cancelRun,
        party.active,party.ready,party.mode,party.page,party.phase,party.enabledMask,q.started,q.requested,q.battleActive,q.playerHp,q.totalTurns,
        q.semanticsMask,q.errorLine,q.fieldReady,q.commandCount,q.partyCount,q.enemySpecies,q.enemyLevel);
    fflush(stdout);
}
// HG_FORCED_DIAGNOSTIC_END
#endif
