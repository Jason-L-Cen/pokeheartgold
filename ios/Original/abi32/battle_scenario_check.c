// SPDX-License-Identifier: GPL-3.0-or-later
#include "../NativeEvents.h"
static void BattleQAPresent(void *argument);
// Reuse the real native bootstrap and original Continue inputs; replace only its display observer.
#define HGNativeSetDisplayObserver(owner,callback,argument) HGNativeSetDisplayObserver(owner,BattleQAPresent,argument)
#define main BattleBootstrapMain
#define HGEngineTrap BattleBootstrapTrap
#include "engine_check.c"
#undef HGEngineTrap
#undef main
#undef HGNativeSetDisplayObserver
#include "native_battle_ui.h"
#include "constants/moves.h"
#include "constants/battle.h"
#define HG_BATTLE_QA_STATE_ONLY
#include "battle_scenario_field.inc"
#undef HG_BATTLE_QA_STATE_ONLY
#include <setjmp.h>
#include <stddef.h>

#define QA_CHECK(value) do { if (!(value)) { fprintf(stderr,"Battle scenario failed at line %d: %s\n",__LINE__,#value); _Exit(1); } } while (0)
static u32 renderedActionFrames,distinctActionFrames,geometryActionFrames,actionRenderErrors;
static u32 lastActionHash,lastGeometrySerial;
static u32 eliteCase,eliteFrames[4],eliteHashes[4],eliteGeometry[4],eliteCaptured;
static jmp_buf eliteResetLanding;
static struct w2c_env *eliteEnv;
static BOOL eliteResetExpected;
static BattleQAState eliteFinal;
void HGEngineTrap(wasm_rt_trap_t code) {
    if (code==WASM_RT_TRAP_UNREACHABLE && eliteResetExpected && eliteEnv && eliteEnv->resetRequested) longjmp(eliteResetLanding,1);
    BattleBootstrapTrap(code);
}
static BattleQAState EliteRead(Capture *capture) {
    u32 address=w2c_hgengine_HGNativeBattleQAGetState(capture->game);
    QA_CHECK(address && address<=capture->env->memory->size && sizeof(BattleQAState)<=capture->env->memory->size-address);
    BattleQAState state; memcpy(&state,capture->env->memory->data+address,sizeof(state));
    if(state.elite.error) fprintf(stderr,"Elite guest assertion line %u: state=%u mask=%x heap=%u/%u main=%u/%u vwait=%u/%u\n",
        state.elite.error,state.elite.state,state.elite.stateMask,state.elite.heapBefore,state.elite.heapAfter,
        state.elite.mainBefore,state.elite.mainAfter,state.elite.vwaitBefore,state.elite.vwaitAfter);
    QA_CHECK(!state.elite.error); return state;
}
static void EliteSetControl(Capture *capture,size_t field,u32 value) {
    u32 address=w2c_hgengine_HGNativeBattleQAGetState(capture->game);
    QA_CHECK(address && address<=capture->env->memory->size && sizeof(BattleQAState)<=capture->env->memory->size-address);
    memcpy(capture->env->memory->data+address+offsetof(BattleQAState,elite)+field,&value,sizeof(value));
}
static BOOL EliteRender(Capture *capture,const EliteQAState *state,u32 stage) {
    static const char *names[]={"field","particles151","particles152","command"};
    if (!capture->buffer) capture->buffer=w2c_hgengine_HGNativeFrameBuffer(capture->game);
    u32 offset=capture->buffer,geometry[8]={0};
    QA_CHECK(stage<4 && offset && offset<=capture->env->memory->size && 256*192*4<=capture->env->memory->size-offset);
    HGNativeDrainDMA();
    BOOL published=w2c_hgengine_HGNativeGeometryFrame(capture->game,1,offset);
    if (published) memcpy(geometry,capture->env->memory->data+offset,sizeof(geometry));
    if(!stage && (!geometry[0] || !geometry[1])) return FALSE;
    if (stage==1 || stage==2) {
        u32 live[8]={0},swap[3]={~0u,~0u,~0u};
        if(w2c_hgengine_HGNativeGeometryFrame(capture->game,0,offset)) memcpy(live,capture->env->memory->data+offset,sizeof(live));
#ifdef HG_ELITE_SWAP_ADDRESS
        QA_CHECK(HG_ELITE_SWAP_ADDRESS<=capture->env->memory->size && sizeof(swap)<=capture->env->memory->size-HG_ELITE_SWAP_ADDRESS);
        memcpy(swap,capture->env->memory->data+HG_ELITE_SWAP_ADDRESS,sizeof(swap));
#endif
        fprintf(stderr,"Elite publication diagnostic: stage=%u state=%u ownerFrame=%u timer=%u published=%u geometry=%u/%u/%u priorSerial=%u live=%u/%u/%u draw=%u/%u active=%u created=%u/%u swap=%u/%u/%u\n",
            stage,state->state,state->frame,state->timer,published,geometry[0],geometry[1],geometry[2],eliteGeometry[stage-1],
            live[0],live[1],live[2],state->particleDraws[0],state->particleDraws[1],state->activeEmitters,state->emitterCreates[0],state->emitterCreates[1],swap[0],swap[1],swap[2]);
        if(!geometry[0] || !geometry[1] || geometry[2]==eliteGeometry[stage-1]) return FALSE;
    }
    BOOL rendered=w2c_hgengine_HGNativeRender2D(capture->game,0,offset,256*192*4);
    if(!stage && !rendered) return FALSE;
    QA_CHECK(rendered);
    const u8 *pixels=capture->env->memory->data+offset; u32 hash=2166136261u,varied=0;
    for (u32 i=0;i<256*192*4;i++) hash=(hash^pixels[i])*16777619u;
    for (u32 i=1;i<256*192;i++) varied|=memcmp(pixels,pixels+i*4,4)!=0;
    if(!varied) return FALSE;
    QA_CHECK(varied && (!stage || hash!=eliteHashes[stage-1]));
    char name[4096]; QA_CHECK(capture->folder && snprintf(name,sizeof(name),"%s/elite-%s.ppm",capture->folder,names[stage])<sizeof(name));
    FILE *file=fopen(name,"wb"); QA_CHECK(file && fprintf(file,"P6\n256 192\n255\n")>0);
    for (u32 i=0;i<256*192;i++) QA_CHECK(fwrite(pixels+i*4,1,3,file)==3);
    QA_CHECK(!fclose(file));
    eliteFrames[stage]=OS_GetVBlankCount(); eliteHashes[stage]=hash; eliteGeometry[stage]=geometry[2]; eliteCaptured|=1u<<stage;
    fprintf(stderr,"Elite frame evidence: {\"stage\":%u,\"state\":%u,\"frame\":%u,\"hash\":%u,\"vertices\":%u,\"polygons\":%u,\"geometrySerial\":%u,\"observedIntroSprites\":%u,\"activeEmitters\":%u}\n",
        stage,state->state,eliteFrames[stage],hash,geometry[0],geometry[1],geometry[2],state->visibleSprites,state->activeEmitters);
    return TRUE;
}
static void ElitePresent(Capture *capture) {
    static u32 phase,entered,lastDiagnostic;
    if (!phase) {
        present(capture);
        if (w2c_hgengine_HGNativeFieldMap(capture->game)) {
            if(!entered) entered=OS_GetVBlankCount();
            QA_CHECK(OS_GetVBlankCount()-entered<7200);
            QA_CHECK(HGNativeSetInput(capture->env,0,FALSE,0,0,FALSE));
            EliteSetControl(capture,offsetof(EliteQAState,caseIndex),eliteCase);
            BattleQAState state=EliteRead(capture);
            if(!state.elite.fieldReady || !EliteRender(capture,&state.elite,0)) return;
            QA_CHECK(state.elite.readyFrame && eliteFrames[0]>=state.elite.readyFrame);
            QA_CHECK(w2c_hgengine_HGNativeBattleQARequest(capture->game)==1);
            entered=OS_GetVBlankCount(); phase=1;
        }
        return;
    }
    QA_CHECK(HGNativeSoundRealtime(capture->env,TRUE)); // Discard sink; no audio device.
    BattleQAState state=EliteRead(capture); const EliteQAState *qa=&state.elite;
    u32 frame=OS_GetVBlankCount(); QA_CHECK(frame-entered<7200);
    if(frame-lastDiagnostic>=300) {
        lastDiagnostic=frame;
        fprintf(stderr,"Elite progress: case=%u frame=%u state=%u mask=%x resources=%u emitters=%u/%u active=%u draw=%u/%u free/end/finish=%u/%u/%u launched=%u\n",
            eliteCase,frame,qa->state,qa->stateMask,qa->particleLoads,qa->emitterCreates[0],qa->emitterCreates[1],qa->activeEmitters,
            qa->particleDraws[0],qa->particleDraws[1],qa->particleFrees,qa->particleEnds,qa->finishes,qa->launches);
    }
    if (!(eliteCaptured&2) && qa->particleLoads==1 && qa->emitterCreates[0]==3 && qa->activeEmitters &&
        qa->particleDraws[0] && qa->visibleSprites>=2 && qa->state>=6 && qa->state<=7) EliteRender(capture,qa,1);
    if (!(eliteCaptured&4) && qa->particleLoads==2 && qa->emitterCreates[1]==4 && qa->activeEmitters &&
        qa->particleDraws[1] && qa->visibleSprites>=2 && qa->state>=11 && qa->state<=12) EliteRender(capture,qa,2);
    QA_CHECK(qa->state<=7 || (eliteCaptured&2));
    QA_CHECK(qa->state<=12 || (eliteCaptured&4));
    u32 address=w2c_hgengine_HGNativeBattleUIGetState(capture->game);
    QA_CHECK(address && address<=capture->env->memory->size && sizeof(HGNativeBattleUIState)<=capture->env->memory->size-address);
    const HGNativeBattleUIState *ui=(const HGNativeBattleUIState *)(capture->env->memory->data+address);
    if(phase==1 && qa->launches && state.serial && ui->ready && ui->mode==HG_BATTLE_UI_COMMAND) {
        static const u32 trainers[]={245,247,418,246,244},classes[]={87,89,112,88,86},nameplates[]={47,51,52,53,54};
        QA_CHECK(eliteCase>=1 && eliteCase<=5 && qa->trainerId==trainers[eliteCase-1] && qa->trainerClass==classes[eliteCase-1]);
        QA_CHECK(qa->battleType==BATTLE_TYPE_TRAINER && qa->effect==28+eliteCase && qa->music && qa->starts==1 && qa->unloads==1 && qa->launches==1);
        QA_CHECK(qa->stateMask==0x7FFF && qa->requestTimer==8 && qa->observations>15 && qa->stateVisits[12]>=(eliteCase==5 ? 9u : 32u));
        QA_CHECK(qa->particleLoads==2 && qa->members[0]==151 && qa->members[1]==152 && qa->emitterMask[0]==7 && qa->emitterMask[1]==15);
        QA_CHECK(qa->emitterCreates[0]==3 && qa->emitterCreates[1]==4 && qa->particleDraws[0] && qa->particleDraws[1] && qa->particleCalcs[0] && qa->particleCalcs[1]);
        QA_CHECK(qa->particleFrees==2 && qa->particleEnds==1 && qa->finishes==1 && qa->spriteLoads==4 && qa->spriteFrees==4 && qa->managerFrees==1 && qa->windowFrees==1);
        QA_CHECK(qa->heapBefore==qa->heapAfter && qa->mainBefore==qa->mainAfter && qa->vwaitBefore==qa->vwaitAfter && (qa->flagsAfter&5)==5);
        QA_CHECK(qa->playerGender<2 && qa->palettes[0]==(qa->playerGender ? 211u : 207u) && qa->palettes[1]==131+4*(eliteCase-1) && qa->palettes[2]==nameplates[eliteCase-1] && qa->palettes[3]==59);
        for(u32 i=0;i<4;i++) {
            u32 base=i==2 ? 47 : qa->palettes[i];
            QA_CHECK(qa->characters[i]==base+1 && qa->cells[i]==base+2 && qa->animations[i]==base+3);
        }
        QA_CHECK(state.species[1] && state.hp[1] && eliteCaptured==7);
        if(!EliteRender(capture,qa,3)) return;
        eliteFinal=state; eliteResetExpected=TRUE; phase=2;
        EliteSetControl(capture,offsetof(EliteQAState,stopRequested),1); // Original reset is executed by the next real battle owner tick.
    }
}
static void BattleQARender(Capture *capture) {
    u32 offset=capture->buffer,geometry[8];
    QA_CHECK(offset && offset<=capture->env->memory->size && 256*192*4<=capture->env->memory->size-offset);
    HGNativeDrainDMA();
    if (w2c_hgengine_HGNativeGeometryFrame(capture->game,1,offset)) {
        memcpy(geometry,capture->env->memory->data+offset,sizeof(geometry));
        if (geometry[0] && geometry[1] && geometry[2]!=lastGeometrySerial) {
            geometryActionFrames++; lastGeometrySerial=geometry[2];
        }
    }
    if (!w2c_hgengine_HGNativeRender2D(capture->game,0,offset,256*192*4)) {
        actionRenderErrors++; return;
    }
    const u8 *pixels=capture->env->memory->data+offset;
    u32 hash=2166136261u;
    for (u32 i=0;i<256*192*4;i++) hash=(hash^pixels[i])*16777619u;
    renderedActionFrames++;
    if (renderedActionFrames>1 && hash==lastActionHash) return;
    lastActionHash=hash; distinctActionFrames++;
    if (distinctActionFrames<=2 && capture->folder) {
        char name[4096];
        QA_CHECK(snprintf(name,sizeof(name),"%s/battle-action-%02u.ppm",capture->folder,distinctActionFrames)<sizeof(name));
        FILE *file=fopen(name,"wb"); QA_CHECK(file);
        QA_CHECK(fprintf(file,"P6\n256 192\n255\n")>0);
        for (u32 y=0;y<192;y++) for (u32 x=0;x<256;x++) QA_CHECK(fwrite(pixels+(y*256+x)*4,1,3,file)==3);
        QA_CHECK(!fclose(file));
    }
}
static void BattleQAPresent(void *argument) {
    Capture *capture=argument;
    if(eliteCase) { ElitePresent(capture); return; }
    static unsigned phase;
    static u32 revision,selectedRevision,diagnosticFrame,actionCaptureFrame;
    if (!phase) {
        present(argument);
        if (w2c_hgengine_HGNativeFieldMap(capture->game)) {
            QA_CHECK(HGNativeSetInput(capture->env,0,FALSE,0,0,FALSE));
            QA_CHECK(w2c_hgengine_HGNativeBattleQARequest(capture->game)==1);
            phase=1; fputs("Battle QA: queued original doubles encounter\n",stderr);
        }
        return;
    }
    QA_CHECK(HGNativeSoundRealtime(capture->env,TRUE));
    u32 address=w2c_hgengine_HGNativeBattleUIGetState(capture->game);
    QA_CHECK(address && address<=capture->env->memory->size && sizeof(HGNativeBattleUIState)<=capture->env->memory->size-address);
    const HGNativeBattleUIState *state=(const HGNativeBattleUIState *)(capture->env->memory->data+address);
    if (phase>=9) {
        u32 snapshot=w2c_hgengine_HGNativeBattleQAGetState(capture->game);
        QA_CHECK(snapshot && snapshot<=capture->env->memory->size && sizeof(BattleQAState)<=capture->env->memory->size-snapshot);
        BattleQAState battle; memcpy(&battle,capture->env->memory->data+snapshot,sizeof(battle));
        if (OS_GetVBlankCount()-diagnosticFrame>=300) {
            diagnosticFrame=OS_GetVBlankCount();
            fprintf(stderr,"Battle QA progress: frame=%u serial=%u armed=%u turn=%u/%u animation=%u starts=%u ends=%u hp=%u,%u,%u,%u pp=%u,%u rendered=%u geometry=%u errors=%u\n",
                diagnosticFrame,battle.serial,battle.armed,battle.initialTurn,battle.totalTurns,battle.animationActive,
                battle.animationStarts,battle.animationEnds,battle.hp[0],battle.hp[1],battle.hp[2],battle.hp[3],
                battle.pp[0][0],battle.pp[2][0],renderedActionFrames,geometryActionFrames,actionRenderErrors);
        }
        // A full ASan software render in every IRQ observer can starve the original owner loop.
        // Capture a bounded pair of action frames, then leave the original animation clock alone.
        if (battle.armed && battle.animationActive && (distinctActionFrames<2 || geometryActionFrames<2)
            && OS_GetVBlankCount()-actionCaptureFrame>=15) {
            actionCaptureFrame=OS_GetVBlankCount(); BattleQARender(capture);
        }
        if (battle.armed && battle.totalTurns>battle.initialTurn && state->ready && state->mode==HG_BATTLE_UI_COMMAND) {
            u32 consumed=0,damaged=0;
            QA_CHECK((battle.playerOTMask&5)==5 && !battle.disobedienceBattlers);
            QA_CHECK(battle.initialMove[0]==MOVE_TACKLE && battle.initialMove[2]==MOVE_TACKLE);
            QA_CHECK(!battle.initialSlot[0] && !battle.initialSlot[2]);
            QA_CHECK(battle.moveReply[0]==1 && battle.moveReply[2]==1);
            for (u32 i=0;i<4;i++) {
                if (battle.hp[i]<battle.initialHp[i]) damaged|=1u<<i;
                for (u32 j=0;j<4;j++) if (battle.pp[i][j]<battle.initialPP[i][j]) consumed|=1u<<i;
                fprintf(stderr,"Battle QA PP battler%u: %u,%u,%u,%u -> %u,%u,%u,%u\n",i,
                    battle.initialPP[i][0],battle.initialPP[i][1],battle.initialPP[i][2],battle.initialPP[i][3],
                    battle.pp[i][0],battle.pp[i][1],battle.pp[i][2],battle.pp[i][3]);
                fprintf(stderr,"Battle QA mon%u: species=%u level=%u partySlot=%u moves=%u,%u,%u,%u selectedSlot=%u move=%u action=%u target=%u reply(command,move,target)=%u,%u,%u status=%x/%x failures=%x\n",
                    i,battle.species[i],battle.level[i],battle.partySlot[i],battle.moves[i][0],battle.moves[i][1],battle.moves[i][2],battle.moves[i][3],
                    battle.initialSlot[i],battle.initialMove[i],battle.initialAction[i],battle.initialTarget[i],
                    battle.commandReply[i],battle.moveReply[i],battle.targetReply[i],battle.status[i],battle.status2[i],battle.failures[i]);
            }
            fprintf(stderr,"Battle QA turn evidence: {\"initialTurn\":%u,\"totalTurns\":%u,\"consumedPpBattlers\":%u,\"damagedBattlers\":%u,\"initialHp\":[%u,%u,%u,%u],\"hp\":[%u,%u,%u,%u],\"initialTacklePp\":[%u,%u],\"tacklePp\":[%u,%u],\"animationStarts\":%u,\"animationEnds\":%u,\"animationBattlers\":%u,\"renderedFrames\":%u,\"distinctActionFrames\":%u,\"geometryFrames\":%u,\"renderErrors\":%u,\"playerOTMask\":%u,\"fixtureOwnerRepairs\":%u,\"disobedienceBattlers\":%u}\n",
                battle.initialTurn,battle.totalTurns,consumed,damaged,
                battle.initialHp[0],battle.initialHp[1],battle.initialHp[2],battle.initialHp[3],
                battle.hp[0],battle.hp[1],battle.hp[2],battle.hp[3],
                battle.initialPP[0][0],battle.initialPP[2][0],battle.pp[0][0],battle.pp[2][0],
                battle.animationStarts,battle.animationEnds,battle.animationBattlers,
                renderedActionFrames,distinctActionFrames,geometryActionFrames,actionRenderErrors,
                battle.playerOTMask,battle.fixtureOwnerRepairs,battle.disobedienceBattlers);
            if (consumed!=15 || !(damaged&5) || !(damaged&10)) {
                fputs("Battle QA branch: a miss, status action, incapacity or early KO did not satisfy the fixed HP case; not a turn/animation pass.\n",stderr);
                _Exit(2);
            }
            QA_CHECK(battle.pp[0][0]+1==battle.initialPP[0][0] && battle.pp[2][0]+1==battle.initialPP[2][0]);
            QA_CHECK(battle.animationStarts && battle.animationStarts==battle.animationEnds && !battle.animationActive);
            QA_CHECK((battle.animationBattlers&5) && (battle.animationBattlers&10));
            QA_CHECK(renderedActionFrames>=2 && distinctActionFrames>=2 && geometryActionFrames>=2 && !actionRenderErrors);
            fputs("PASS: real original doubles animated turn, both sides PP/HP changes, animation lifecycle, rendered geometry and next-turn command. UIKit and settlement unverified.\n",stderr);
            _Exit(0);
        }
        return;
    }
    if (!state->ready || state->revision==revision) return;
    fprintf(stderr,"Battle QA: phase=%u mode=%u revision=%u enabled=%x\n",phase,state->mode,state->revision,state->enabledMask);
    if ((phase==1 && state->mode==HG_BATTLE_UI_COMMAND) || ((phase==2 || phase==4) && state->mode==HG_BATTLE_UI_MOVES)) {
        if (phase!=1) QA_CHECK(state->move[0]==MOVE_TACKLE && state->pp[0]>0);
        revision=state->revision;
        QA_CHECK(w2c_hgengine_HGNativeBattleUIChoose(capture->game,revision,0)); phase++;
    } else if ((phase==3 || phase==5) && state->mode==HG_BATTLE_UI_TARGETS) {
        QA_CHECK((state->enabledMask&3)==3 && __builtin_popcount(state->enabledMask&15)==3 && (state->enabledMask&16));
        for (unsigned i=0;i<4;i++) {
            QA_CHECK(state->targetMaxHp[i] && state->targetHp[i]<=state->targetMaxHp[i]);
            if (!(state->enabledMask&(1u<<i))) {
                QA_CHECK(!state->targetSpecies[i] && !state->nameWidth[i]);
                continue; // The original menu leaves the forbidden self target blank.
            }
            QA_CHECK(state->targetSpecies[i]);
            QA_CHECK(state->nameWidth[i]>0 && state->nameWidth[i]<=HG_BATTLE_NAME_WIDTH);
            unsigned opaque=0;
            for (unsigned p=3;p<sizeof(state->namePixels[i]);p+=4) opaque+=state->namePixels[i][p]!=0;
            QA_CHECK(opaque>0);
            QA_CHECK(__builtin_popcount(state->targetMask[i])==1);
        }
        revision=state->revision;
        if (phase==3) {
            QA_CHECK(w2c_hgengine_HGNativeBattleUIChoose(capture->game,revision,4));
            fputs("Battle QA: original glyphs/HP/legal targets verified; cancelling\n",stderr);
        } else {
            selectedRevision=revision;
            QA_CHECK(w2c_hgengine_HGNativeBattleUIChoose(capture->game,revision,0));
            fputs("Battle QA: selecting original opponent target\n",stderr);
        }
        phase++;
    } else if (phase==6 && state->mode==HG_BATTLE_UI_COMMAND) {
        QA_CHECK(state->revision!=selectedRevision);
        revision=state->revision;
        QA_CHECK(w2c_hgengine_HGNativeBattleUIChoose(capture->game,revision,0)); phase=7;
    } else if (phase==7 && state->mode==HG_BATTLE_UI_MOVES) {
        QA_CHECK(state->move[0]==MOVE_TACKLE && state->pp[0]>0);
        revision=state->revision;
        QA_CHECK(w2c_hgengine_HGNativeBattleUIChoose(capture->game,revision,0)); phase=8;
    } else if (phase==8 && state->mode==HG_BATTLE_UI_TARGETS) {
        QA_CHECK(state->enabledMask&2);
        revision=state->revision;
        QA_CHECK(w2c_hgengine_HGNativeBattleUIChoose(capture->game,revision,1));
        QA_CHECK(w2c_hgengine_HGNativeBattleQAArm(capture->game)); phase=9;
        fputs("Battle QA: both player targets submitted; awaiting original actions and next turn\n",stderr);
    }
}

int main(int argc,char **argv) {
    if(argc!=7) return BattleBootstrapMain(argc,argv);
    QA_CHECK(!strcmp(argv[5],"--continue") && strlen(argv[6])==1 && argv[6][0]>='1' && argv[6][0]<='5');
    eliteCase=argv[6][0]-'0'; QA_CHECK(HGNativeFSSetRoot(argv[1]) && HGNativeBackupSetPath(argv[3]));
    u8 header[0x160]; FILE *input=fopen(argv[2],"rb"); QA_CHECK(input);
    QA_CHECK(fread(header,1,sizeof(header),input)==sizeof(header) && fgetc(input)==EOF && !ferror(input)); QA_CHECK(!fclose(input));
    wasm_rt_init(); w2c_hgengine *game=calloc(1,sizeof(*game)); struct w2c_env *env=calloc(1,sizeof(*env));
    struct w2c_wasi__snapshot__preview1 *wasi=calloc(1,sizeof(*wasi)); Capture *capture=calloc(1,sizeof(*capture));
    QA_CHECK(game && env && wasi && capture); for(u32 i=0;i<3;i++) { wasi->fds[i]=dup(i); QA_CHECK(wasi->fds[i]>=0); }
    eliteEnv=env;
    if(!setjmp(eliteResetLanding)) {
        wasm_rt_trap_t trap=wasm_rt_impl_try(); QA_CHECK(!trap);
        wasm2c_hgengine_instantiate(game,env,wasi); env->memory=wasi->memory=w2c_hgengine_memory(game);
        QA_CHECK(HGNativeSoundRealtime(env,TRUE));
        env->irqTable=&game->w2c_T0; env->stackPointer=&game->w2c_0x5F_stack_pointer;
        env->rootStackHigh=*env->stackPointer; *env->stackPointer-=16;
        env->irqType0=wasm2c_hgengine_get_func_type(0,0); env->irqType1=wasm2c_hgengine_get_func_type(1,0,WASM_RT_I32);
        env->rtcType=wasm2c_hgengine_get_func_type(2,0,WASM_RT_I32,WASM_RT_I32);
        env->irqGuardType=wasm2c_hgengine_get_func_type(4,0,WASM_RT_I32,WASM_RT_I32,WASM_RT_I32,WASM_RT_I32);
        u32 work=w2c_hgengine_HGNativeSystemWorkAddress(game); QA_CHECK(work && work<=env->memory->size && 0x360<=env->memory->size-work);
        memcpy(env->memory->data+work+0x200,header,sizeof(header)); w2c_hgengine_0x5Finitialize(game);
        u32 status=w2c_hgengine_HGNativeRegisterAddress(game,0x04000004,2);
        QA_CHECK(status && !(status&1) && status<=env->memory->size && 2<=env->memory->size-status);
        QA_CHECK(HGNativeStartDisplayClock(env,(const vu16 *)(env->memory->data+status)));
        *capture=(Capture){.game=game,.env=env,.folder=argv[4],.resume=TRUE};
        QA_CHECK(HGNativeSetDisplayObserver(env,BattleQAPresent,capture)); w2c_hgengine_NitroMain(game); QA_CHECK(FALSE);
    }
    QA_CHECK(eliteResetExpected && env->resetRequested); HGNativeFinishReset(env);
    QA_CHECK(env->resetFinished && !env->pendingDMA && !env->dmaAffineWrites && !env->pendingRTC && !env->pendingCARD &&
        !env->pendingMIC && !env->pendingPOWER && !env->pendingINPUT && !env->irqTable && !env->threads && !env->rasterFunction);
    const EliteQAState *qa=&eliteFinal.elite;
    printf("Elite QA evidence: {\"case\":%u,\"trainer\":%u,\"class\":%u,\"effect\":%u,\"stateMask\":%u,\"requestTimer\":%u,\"observations\":%u,\"particleLoads\":%u,\"emitterCreates\":[%u,%u],\"particleFrees\":%u,\"particleEnds\":%u,\"finished\":%u,\"launched\":%u,\"heapBefore\":%u,\"heapAfter\":%u,\"playerGender\":%u,\"renderedMask\":%u,\"ownerReadyFrame\":%u,\"firstVisibleFrame\":%u,\"resetFinished\":true,\"pendingDMA\":0,\"stateFrames\":[",
        eliteCase,qa->trainerId,qa->trainerClass,qa->effect,qa->stateMask,qa->requestTimer,qa->observations,qa->particleLoads,
        qa->emitterCreates[0],qa->emitterCreates[1],qa->particleFrees,qa->particleEnds,qa->finishes,qa->launches,qa->heapBefore,qa->heapAfter,qa->playerGender,eliteCaptured,qa->readyFrame,eliteFrames[0]);
    for(u32 i=0;i<15;i++) printf("%s%u",i ? "," : "",qa->stateFrames[i]);
    fputs("],\"stateVisits\":[",stdout);
    for(u32 i=0;i<15;i++) printf("%s%u",i ? "," : "",qa->stateVisits[i]);
    fputs("],\"spriteMembers\":[",stdout);
    for(u32 i=0;i<4;i++) printf("%s[%u,%u,%u,%u]",i ? "," : "",qa->palettes[i],qa->characters[i],qa->cells[i],qa->animations[i]);
    puts("]}");
    wasm2c_hgengine_free(game); for(u32 i=0;i<3;i++) close(wasi->fds[i]); free(game); free(env); free(wasi); free(capture); eliteEnv=NULL; wasm_rt_free();
    puts("PASS: full original Elite Four/Champion intro, ROM sprites/particles, real timers/cleanup and battle command, native frames and reset; league story/device fidelity unverified."); return 0;
}
