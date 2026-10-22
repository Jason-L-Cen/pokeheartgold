// SPDX-License-Identifier: GPL-3.0-or-later
#include "../NativeEvents.h"
static void BattleQAPresent(void *argument);
// Reuse the real native bootstrap and original Continue inputs; replace only its display observer.
#define HGNativeSetDisplayObserver(owner,callback,argument) HGNativeSetDisplayObserver(owner,BattleQAPresent,argument)
#include "engine_check.c"
#undef HGNativeSetDisplayObserver
#include "native_battle_ui.h"
#include "constants/moves.h"
#define HG_BATTLE_QA_STATE_ONLY
#include "battle_scenario_field.inc"
#undef HG_BATTLE_QA_STATE_ONLY

#define QA_CHECK(value) do { if (!(value)) { fprintf(stderr,"Battle scenario failed at line %d: %s\n",__LINE__,#value); _Exit(1); } } while (0)
static u32 renderedActionFrames,distinctActionFrames,geometryActionFrames,actionRenderErrors;
static u32 lastActionHash,lastGeometrySerial;
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
