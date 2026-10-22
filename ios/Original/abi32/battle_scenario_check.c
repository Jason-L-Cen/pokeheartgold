// SPDX-License-Identifier: GPL-3.0-or-later
#include "../NativeEvents.h"
static void BattleQAPresent(void *argument);
// Reuse the real native bootstrap and original Continue inputs; replace only its display observer.
#define HGNativeSetDisplayObserver(owner,callback,argument) HGNativeSetDisplayObserver(owner,BattleQAPresent,argument)
#include "engine_check.c"
#undef HGNativeSetDisplayObserver
#include "native_battle_ui.h"
#include "constants/moves.h"

#define QA_CHECK(value) do { if (!(value)) { fprintf(stderr,"Battle scenario failed at line %d: %s\n",__LINE__,#value); _Exit(1); } } while (0)
static void BattleQAPresent(void *argument) {
    Capture *capture=argument;
    static unsigned phase;
    static u32 revision,selectedRevision;
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
        fputs("PASS: real original doubles launch, target glyph/HP cache, legal masks, cancel to moves, selection and next battler command. No UIKit or battle-settlement claim.\n",stderr);
        _Exit(0); // This isolated process owns all outstanding transfers; reset is checked separately.
    }
}
