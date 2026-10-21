#ifndef POKEHEARTGOLD_BATTLE_BATTLE_ANIM_BALL_H
#define POKEHEARTGOLD_BATTLE_BATTLE_ANIM_BALL_H

// Poke Ball throw / capture animation (BallThrow), the ball capsule seal and ball burst
// particles, a generic particle emitter manager and the battler pic graphic swap helpers.

#include "battle/battle.h"

#include "filesystem.h"
#include "overlay_07.h"
#include "pokemon.h"
#include "pokepic.h"
#include "seal_case.h"
#include "spl.h"
#include "sys_task_api.h"

// Input of BallSealEffect_New: the particles shown when a Pokemon comes out of its ball
typedef struct BallSealEffectTemplate {
    int battlerType;
    int ball; // used instead of the Pokemon's own ball when nonzero
    Pokemon *mon;
} BallSealEffectTemplate;

// Input of BallBurstEffect_New: the particles of a ball bursting open at a screen position
typedef struct BallBurstEffectTemplate {
    s16 x;
    s16 y;
    int ball;
    enum HeapID heapId;
    int emitter;  // all emitters of the ball's burst (see BallBurstEffect_Start)
    BOOL generic; // the burst of ball 1027 with all its emitters
} BallBurstEffectTemplate;

typedef struct BattleAnimEmitterTemplate {
    int resourceId;  // particle resource (5..26)
    int perspective; // second argument of ParticleSystem_SetPerspectiveType
} BattleAnimEmitterTemplate;

struct BattleAnimBattlerPicParams;

typedef struct BallSealEffect BallSealEffect;
typedef struct BallBurstEffect BallBurstEffect;
typedef struct BattleAnimEmitter BattleAnimEmitter;
typedef struct BattleAnimEmitterManager BattleAnimEmitterManager;

BallSealEffect *BallSealEffect_New(enum HeapID heapId, const BallSealEffectTemplate *template);
void BallSealEffect_LoadResources(BallSealEffect *effect);
BOOL BallSealEffect_IsLoaded(BallSealEffect *effect);
void BallSealEffect_Start(BallSealEffect *effect);
BOOL BallSealEffect_IsActive(BallSealEffect *effect);
void BallSealEffect_Free(BallSealEffect *effect);
BallBurstEffect *BallBurstEffect_New(const BallBurstEffectTemplate *template);
void BallBurstEffect_Start(BallBurstEffect *effect);
BOOL BallThrow_BallAnimThrow(BallThrow *data);
BOOL BallThrow_BallAnimOpen(BallThrow *data);
BOOL BallThrow_BallAnimDeflect(BallThrow *data);
BOOL BallThrow_BallAnimFall(BallThrow *data);
BOOL BallThrow_BallAnimShake(BallThrow *data);
BOOL BallThrow_BallAnimSkip(BallThrow *data);
BOOL BallThrow_BallAnimClick(BallThrow *data);
BOOL BallThrow_BallAnimFade(BallThrow *data);
BOOL BallThrow_CaptureStateIdle(BallThrow *data);
BOOL BallThrow_CaptureStateThrow(BallThrow *data);
BOOL BallThrow_CaptureStateGoToOpen(BallThrow *data);
BOOL BallThrow_CaptureStateOpen(BallThrow *data);
BOOL BallThrow_CaptureStateOpenDone(BallThrow *data);
BOOL BallThrow_CaptureStateDeflect(BallThrow *data);
BOOL BallThrow_CaptureStateDeflectDone(BallThrow *data);
BOOL BallThrow_CaptureStateStartFall(BallThrow *data);
BOOL BallThrow_CaptureStateGoToFlash(BallThrow *data);
BOOL BallThrow_CaptureStateFlash(BallThrow *data);
BOOL BallThrow_CaptureStateGoToDrop(BallThrow *data);
BOOL BallThrow_CaptureStateDrop(BallThrow *data);
BOOL BallThrow_CaptureStateNone(BallThrow *data);
BOOL BallThrow_CaptureStateBounce(BallThrow *data);
BOOL BallThrow_CaptureStateBounceDone(BallThrow *data);
BOOL BallThrow_CaptureStateShakeWait(BallThrow *data);
BOOL BallThrow_CaptureStateGoToShake(BallThrow *data);
BOOL BallThrow_CaptureStateShake(BallThrow *data);
BOOL BallThrow_CaptureStateShakeDone(BallThrow *data);
BOOL BallThrow_CaptureStateGoToSkip(BallThrow *data);
BOOL BallThrow_CaptureStateSkip(BallThrow *data);
BOOL BallThrow_CaptureStateClick(BallThrow *data);
BOOL BallThrow_CaptureStateClickDone(BallThrow *data);
BOOL BallThrow_CaptureStateRestorePalette(BallThrow *data);
BOOL BallThrow_CaptureStateGoToFadeOut(BallThrow *data);
BOOL BallThrow_CaptureStateFadeOut(BallThrow *data);
BOOL BallThrow_CaptureStateGoToEnd(BallThrow *data);
BOOL BallThrow_CaptureStateEnd(BallThrow *data);
BOOL BallThrow_RunCaptureState(BallThrow *data);
BOOL BallThrow_RunThrowArc(BallThrow *data);
BOOL BallThrow_ThrowArcAndOpen(BallThrow *data);
BOOL BallThrow_OpenInPlace(BallThrow *data);
BOOL BallThrow_Wobble(BallThrow *data);
BOOL BallThrow_TrackPosition(BallThrow *data);
BOOL BallThrow_IsActive(BallThrow *data);
BOOL BallThrow_HasOpened(BallThrow *data);
void BallThrow_SetMode(BallThrow *data, int mode);
void BallThrow_SetDrawFlag(BallThrow *data, int flag);
void BallThrow_SetPosition(BallThrow *data, s16 x, s16 y);
void BallThrow_SetAnimateSprite(BallThrow *data, BOOL animate);
void BallThrow_SetDrawPriority(BallThrow *data, u16 priority);
void BallThrow_SetPriority(BallThrow *data, int priority);
void BallThrow_SetDelay(BallThrow *data, int delay);
s32 BallSealEffect_GetDistance(s16 x0, s16 y0, s16 x1, s16 y1);
BattleAnimEmitterManager *BattleAnimEmitterManager_New(enum HeapID heapId, int *unk);
BattleAnimEmitter *BattleAnimEmitterManager_AddEmitter(BattleAnimEmitterManager *manager, BattleAnimEmitterTemplate *template);
void BattleAnimEmitterManager_Free(BattleAnimEmitterManager *manager);
void BattleAnimEmitterManager_StopAll(BattleAnimEmitterManager *manager);
void BattleAnimEmitter_Start(BattleAnimEmitter *emitter);
BOOL BattleAnimEmitter_IsActive(BattleAnimEmitter *emitter);
void BattleAnimUtil_LoadMonIntoPicSameFacing(struct BattleAnimBattlerPicParams *params, enum HeapID heapId);
void BattleAnimUtil_LoadMonIntoPicOppositeFacing(struct BattleAnimBattlerPicParams *params, enum HeapID heapId);
void BattleAnimUtil_RestorePic(struct BattleAnimBattlerPicParams *params, enum HeapID heapId);
void BattleAnimUtil_LoadSubstituteIntoPic(struct BattleAnimBattlerPicParams *params, enum HeapID heapId);
s16 BattleAnimUtil_GetBattlerTypeCoord(u32 battlerType, u32 index);

#endif // POKEHEARTGOLD_BATTLE_BATTLE_ANIM_BALL_H
