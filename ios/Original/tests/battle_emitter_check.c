// SPDX-License-Identifier: GPL-3.0-or-later
#define BattleAnimSystem_GetCurrentParticleSystem EmitterCheckCurrent
#define BattleAnimUtil_GetBattlerType EmitterCheckType
#define BattleAnimSystem_ReadScriptArgs EmitterCheckArgs
#define BattleAnimSystem_GetSideSign EmitterCheckSign
#define BattleAnimUtil_GetBattlerEmitterPositionKind0 EmitterCheckPosition
#define ParticleEmitter_GetConvergencePos EmitterCheckGetConvergence
#define ParticleEmitter_SetConvergencePos EmitterCheckSetConvergence
#define ParticleEmitter_GetMagnetPos EmitterCheckGetMagnet
#define ParticleEmitter_SetMagnetPos EmitterCheckSetMagnet
#include "../../../src/battle/battle_anim_emitter.c"
#include <stdio.h>
#include <stdlib.h>

#define CHECK(value) do { if (!(value)) return __LINE__; } while (0)
static u32 words[28], consumed;
static int types[2], sideSign;
static VecFx32 convergence, magnet;
static BOOL bad;
SPLEmitter *EmitterCheckCurrent(BattleAnimSystem *system) { return NULL; }
int EmitterCheckType(BattleAnimSystem *system, int battler) { return types[battler]; }
int EmitterCheckSign(BattleAnimSystem *system, int a, int b) { bad |= a != 0 || b != 1; return sideSign; }
BOOL EmitterCheckArgs(BattleAnimSystem *system, u32 *out, int count) {
    if (count < 0 || consumed + count > NELEMS(words)) { bad = TRUE; return FALSE; }
    memcpy(out, words + consumed, count * sizeof(u32)); consumed += count; return TRUE;
}
void EmitterCheckPosition(BattleAnimSystem *system, int battler, VecFx32 *pos) { *pos = (VecFx32){100 + battler, 200 + battler, 300 + battler}; }
void EmitterCheckGetConvergence(SPLEmitter *emitter, VecFx32 *pos) { *pos = convergence; }
void EmitterCheckSetConvergence(SPLEmitter *emitter, VecFx32 *pos) { convergence = *pos; }
void EmitterCheckGetMagnet(SPLEmitter *emitter, VecFx32 *pos) { *pos = magnet; }
void EmitterCheckSetMagnet(SPLEmitter *emitter, VecFx32 *pos) { magnet = *pos; }

u32 CheckOriginalBattleEmitter(void) {
    SPLResBase base; memset(&base, 0, sizeof(base)); base.pos = (VecFx32){17,-23,31};
    SPLResource resource; memset(&resource, 0, sizeof(resource)); resource.p_base = &base;
    // Independent axis selection for all six single/double battle positions.
    static const u8 axes[6][6] = {{0,0,0,0,0,0},{0,0,0,0,0,0},{3,3,3,1,3,2},
        {4,4,1,4,4,3},{6,6,6,4,6,5},{5,5,2,6,5,5}};
    for (int a = 0; a < 6; a++) for (int b = 0; b < 6; b++) {
        SPLEmitter emitter; memset(&emitter, 0, sizeof(emitter));
        emitter.p_res = &resource;
        types[0] = a; types[1] = b; sideSign = a & 1 ? -1 : 1;
        consumed = 0; bad = FALSE;
        for (u32 i = 0; i < 21; i++) words[i] = i + 1;
        words[21] = 11; words[22] = 22; words[23] = 33;
        BattleAnimEmitter_SetTransformFromScript(NULL, &emitter, 0, 1, 0, 0);
        CHECK(!bad && consumed == 24);
        CHECK(emitter.emtr_pos.x == 11 * sideSign + 17 && emitter.emtr_pos.y == 22 * sideSign - 23 && emitter.emtr_pos.z == 300 * sideSign + 31);
        CHECK(emitter.axis.x == (s16)((axes[a][b] * 3 + 1) * sideSign));
        CHECK(emitter.axis.y == (s16)((axes[a][b] * 3 + 2) * sideSign));
        CHECK(emitter.axis.z == (s16)((axes[a][b] * 3 + 3) * sideSign));
    }
    types[0] = 1; types[1] = 0;
    for (u32 mode = 0; mode < 3; mode++) for (u32 variant = 0; variant < 4; variant++) {
        SPLEmitter emitter; memset(&emitter, 0, sizeof(emitter));
        emitter.p_res = &resource;
        consumed = 0; bad = FALSE; convergence = magnet = (VecFx32){0,0,0};
        // Zero vector uses the battler position; (0,3200) suppresses mirroring.
        words[21] = variant == 0 || variant == 1 ? 0 : 11;
        words[22] = variant == 0 ? 0 : variant == 1 ? 3200 : 22;
        words[23] = variant == 0 ? 0 : 33;
        sideSign = variant == 3 ? 0xFF : -1;
        words[24] = 5; words[25] = 7; words[26] = 9; words[27] = 7;
        BattleAnimEmitter_SetTransformFromScript(NULL, &emitter, 0, 1, mode, 5);
        int sign = variant == 1 ? 1 : -1;
        VecFx32 expected = variant == 0 ? (VecFx32){100,200,300} : (VecFx32){(s32)words[21]*sign,(s32)words[22]*sign,300*sign};
        CHECK(!bad && consumed == (mode ? 28 : 24));
        CHECK(emitter.emtr_pos.x == expected.x + 17 && emitter.emtr_pos.y == expected.y - 23 && emitter.emtr_pos.z == expected.z + 31);
        if (mode) {
            VecFx32 target = {5*sign-expected.x,7*sign-expected.y,9*sign-expected.z};
            CHECK(!memcmp(mode == 1 ? &convergence : &magnet, &target, sizeof(target)));
        }
    }
    return 0;
}
#ifndef HEARTGOLD_ABI32
void HGNativeAssertFail(const char *condition,const char *file,int line) { fprintf(stderr,"%s:%d: %s\n",file,line,condition); abort(); }
int main(void) { u32 line = CheckOriginalBattleEmitter(); if (line) { fprintf(stderr,"Battle emitter check line %u\n",line); return 1; } puts("PASS: original particle axes, signed mirroring, battler positions and script convergence/magnet targets."); return 0; }
#endif
