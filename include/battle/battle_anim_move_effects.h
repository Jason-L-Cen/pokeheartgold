#ifndef POKEHEARTGOLD_BATTLE_BATTLE_ANIM_MOVE_EFFECTS_H
#define POKEHEARTGOLD_BATTLE_BATTLE_ANIM_MOVE_EFFECTS_H

#include "battle/battle_anim_funcs.h"
#include "battle/battle_anim_internal.h"

#include "bg_window.h"
#include "palette.h"
#include "pokepic.h"
#include "sprite_system.h"
#include "sys_task.h"

// A sprite circling a point: the angle step of its orbit oscillates (speed), then it runs a second circle
typedef struct BattleAnimMotionSprite {
    ManagedSprite *sprite;
    BattleAnimSinMotion orbit;
    BattleAnimCosMotion speed; // added to baseAngleStep
    s16 baseAngleStep;
    s16 x; // center of the orbit
    s16 y;
    u16 unk_42;
    int state;
    BOOL active;
} BattleAnimMotionSprite;

typedef struct BattleAnimShakeDefenderWithOverlayWork BattleAnimShakeDefenderWithOverlayWork;
typedef struct BattleAnimDropSpriteStepwiseWork BattleAnimDropSpriteStepwiseWork;

void BattleAnimFunc_SetBgPaletteGrayscale(BattleAnimSystem *system);
void BattleAnimFunc_DropSpriteStepwise(BattleAnimSystem *system);
void BattleAnimFunc_AnimateSprites(BattleAnimSystem *system);
void BattleAnimFunc_BattleBgWave(BattleAnimSystem *system);
void BattleAnimSpriteFunc_BlinkScaleFadeSprites(BattleAnimSystem *system, SpriteSystem *spriteSystem, SpriteManager *spriteManager, ManagedSprite *sprite);
void BattleAnimFunc_AttackerGrowFlashRed(BattleAnimSystem *system);
void BattleAnimFunc_AttackerSquashStretch(BattleAnimSystem *system);
void BattleAnimFunc_AttackerAfterimages(BattleAnimSystem *system);
void BattleAnimFunc_AttackerPulseWhite(BattleAnimSystem *system);
void BattleAnimFunc_AttackerStretchSteps(BattleAnimSystem *system);
void BattleAnimFunc_AttackerLeapOffscreen(BattleAnimSystem *system);
void BattleAnimFunc_AttackerSilhouette(BattleAnimSystem *system);
void BattleAnimFunc_AttackerHop(BattleAnimSystem *system);
void BattleAnimFunc_ShowAttackerObjWindowSprites(BattleAnimSystem *system);
void BattleAnimFunc_AttackerSquashAfterimages(BattleAnimSystem *system);
void BattleAnimFunc_ShakeBattlefieldFlashing(BattleAnimSystem *system);
void BattleAnimFunc_SpriteDriftFade(BattleAnimSystem *system);
void BattleAnimFunc_RockOrShakeAttacker(BattleAnimSystem *system);
void BattleAnimFunc_ShakeBattlefieldByPower(BattleAnimSystem *system);
void BattleAnimFunc_ShakeSpritePair(BattleAnimSystem *system);
void BattleAnimSpriteFunc_ThrowSpriteArc(BattleAnimSystem *system, SpriteSystem *spriteSystem, SpriteManager *spriteManager, ManagedSprite *sprite);
void BattleAnimFunc_StretchFadeSprite(BattleAnimSystem *system);
void BattleAnimSpriteFunc_CrossingSprites(BattleAnimSystem *system, SpriteSystem *spriteSystem, SpriteManager *spriteManager, ManagedSprite *sprite);
void BattleAnimSpriteFunc_BlinkSpritesCenter(BattleAnimSystem *system, SpriteSystem *spriteSystem, SpriteManager *spriteManager, ManagedSprite *sprite);
void BattleAnimSpriteFunc_SlidingSpritePairs(BattleAnimSystem *system, SpriteSystem *spriteSystem, SpriteManager *spriteManager, ManagedSprite *sprite);
void BattleAnimSpriteFunc_PopAndScatterSprites(BattleAnimSystem *system, SpriteSystem *spriteSystem, SpriteManager *spriteManager, ManagedSprite *sprite);
void BattleAnimFunc_WaveAttackerOnBg(BattleAnimSystem *system);
void BattleAnimSpriteFunc_SpriteQuad(BattleAnimSystem *system, SpriteSystem *spriteSystem, SpriteManager *spriteManager, ManagedSprite *sprite);
void BattleAnimSpriteFunc_FlashingSpritesAroundAttacker(BattleAnimSystem *system, SpriteSystem *spriteSystem, SpriteManager *spriteManager, ManagedSprite *sprite);
void BattleAnimFunc_ShrinkSpriteOnDefender(BattleAnimSystem *system);
void BattleAnimFunc_AttackerDashAfterimages(BattleAnimSystem *system);
void BattleAnimSpriteFunc_SpriteTrailToDefender(BattleAnimSystem *system, SpriteSystem *spriteSystem, SpriteManager *spriteManager, ManagedSprite *sprite);
void BattleAnimFunc_SquashSprite(BattleAnimSystem *system);
void BattleAnimFunc_SlideOutAndTransform(BattleAnimSystem *system);
void BattleAnimFunc_WipeOutDefender(BattleAnimSystem *system);
void BattleAnimFunc_PicOrbit(BattleAnimSystem *system);
void BattleAnimFunc_AttackerRecoilTilt(BattleAnimSystem *system);
void BattleAnimSpriteFunc_OrbitingSpriteTrail(BattleAnimSystem *system, SpriteSystem *spriteSystem, SpriteManager *spriteManager, ManagedSprite *sprite);
void BattleAnimFunc_ShakeDefenderWithOverlay(BattleAnimSystem *system);

s16 BattleAnimSystem_GetBattlerScreenCoord(BattleAnimSystem *system, int battler, u32 coord);
int BattleAnimUtil_GetBattlerSideSign(BattleAnimSystem *system, int battler);

#endif // POKEHEARTGOLD_BATTLE_BATTLE_ANIM_MOVE_EFFECTS_H
