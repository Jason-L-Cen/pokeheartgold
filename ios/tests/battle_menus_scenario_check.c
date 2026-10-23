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
#include <nitro/os/native_reset.h>
#include <setjmp.h>

#define QA_CHECK(value) do { if (!(value)) { fprintf(stderr,"Battle menus failure line %d stage %u: %s\n",__LINE__,stage,#value); _Exit(1); } } while (0)
typedef struct BattleMenusCommand { u32 active,revision,mode,ready,enabledMask,cancelRun,move[4],pp[4]; } BattleMenusCommand;
_Static_assert(sizeof(BattleMenusCommand)==offsetof(HGNativeBattleUIState,maxPP),"Battle command prefix");
_Static_assert(sizeof(HGNativeBattleBagUIState)==536,"Battle Bag ABI");
_Static_assert(sizeof(HGNativeBattlePartyUIState)==684 && offsetof(HGNativeBattlePartyUIState,selectedMove)==528
    && offsetof(HGNativeBattlePartyUIState,moves)==532 && offsetof(HGNativeBattlePartyUIState,prompt)==580
    && offsetof(HGNativeBattlePartyUIState,message)==632,"Battle Party ABI");
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
        if (engine) { HGNativeBattlePartyUIState party=readParty(capture); infoCrops(capture,&party,pixels); }
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
    if (pulseFrame) {
        if (!pulseReleased && frame-pulseFrame>=8) { QA_CHECK(HGNativeSetInput(capture->env,0,FALSE,0,0,FALSE)); pulseReleased=TRUE; }
        if (frame-pulseFrame<12) return;
        pulseFrame=0;
    }
    HGNativeBattleMenusQAState q=readQA(capture); BattleMenusCommand cmd=readCommand(capture);
    HGNativeBattleBagUIState bag=readBag(capture); HGNativeBattlePartyUIState party=readParty(capture);
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
            || q.moves[0]!=33 || q.moves[1]!=45 || q.moves[2] || q.moves[3] || q.pp[1]<6 || q.enemySpecies!=10 || q.enemyLevel!=1;
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
        QA_CHECK(cmd.move[1]==45 && cmd.pp[1] && growls<6); ++growls; commandChoice(capture,cmd,1); next(22); break;
    case 22:
        if (!command || q.totalTurns<=turnBefore) break;
        QA_CHECK(q.totalTurns==turnBefore+1 && q.playerHp && q.playerHp<=20 && q.potionQuantity==5);
        if (!snapshot(capture,"natural-turn",TRUE)) break;
        if (q.playerHp<q.playerMaxHp) { commandChoice(capture,cmd,1); next(23); }
        else if (growls<6) { turnBefore=q.totalTurns; commandChoice(capture,cmd,0); next(21); }
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
    if (argc!=5 || !HGNativeFSSetRoot(argv[1]) || !HGNativeBackupSetPath(argv[3])) return 2;
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
    BOOL passed=baselineComplete && infoComplete && infoBankRoundTrip && infoStaleRejected==8 && healed && fieldReturned
        && !sceneBlocked && !damageMissing && !blockedStatus && staleRejected>=2;
    const char *status=blockedStatus ? blockedStatus : sceneBlocked ? "scene_prerequisite_blocked" : damageMissing ? "prerequisite_damage_not_observed" : passed ? "passed_original_battle_menus_and_heal" : "actual_assertion_failed";
    printf("Battle menus flee evidence: {\"attempts\":%u,\"failures\":%u,\"actualOutcome\":%u,\"returned\":%u}\n",runAttempts,fleeFailures,finalQA.outcome,finalQA.returned);
    printf("Battle menus info evidence: {\"complete\":%u,\"pagesMask\":%u,\"availableMoveMask\":%u,\"staleRejected\":%u,\"descriptionBankRoundTrip\":%u,\"qaBytes\":%zu,\"realFixtureMonCount\":%u,\"multiMonOrEggNavigationCovered\":false}\n",
        infoComplete,infoPages,infoDescriptionMask,infoStaleRejected,infoBankRoundTrip,sizeof(infoBefore),infoBefore.partyCount);
    printf("Battle menus evidence: {\"status\":\"%s\",\"baselineCancel\":%u,\"fullHpNoEffect\":%u,\"targetCancel\":%u,\"shiftRejected\":%u,\"staleRejected\":%u,\"growlTurns\":%u,\"preHealHp\":%u,\"healGate\":%u,\"replyHp\":%u,\"replyMaxHp\":%u,\"replyPotionQuantity\":%u,\"lastCommandHp\":%u,\"totalTurns\":%u,\"returned\":%u,\"outcome\":%u,\"semanticsMask\":%u,\"lifecycleCleared\":%u,\"snapshots\":%u,\"flowComplete\":%u,\"resetFinished\":true,\"pendingDMA\":0}\n",status,baselineComplete,fullHpNoEffect,targetCancel,shiftRejected,staleRejected,growls,preHealHp,healed,finalQA.replyHp,finalQA.replyMaxHp,finalQA.replyPotionQuantity,finalQA.playerHp,finalQA.totalTurns,finalQA.returned,finalQA.outcome,finalQA.semanticsMask,finalQA.lifecycleCleared,snapshots,fieldReturned);
    bytes(capture,"final-qa.bin",&finalQA,sizeof(finalQA));
    wasm2c_hgengine_free(game); for (u32 i=0;i<3;i++) close(wasi->fds[i]); free(game); free(env); free(wasi); free(capture); menusEnv=NULL; wasm_rt_free();
    return passed ? 0 : sceneBlocked || damageMissing || blockedStatus ? 3 : 1;
}
