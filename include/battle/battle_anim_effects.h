#ifndef POKEHEARTGOLD_BATTLE_BATTLE_ANIM_EFFECTS_H
#define POKEHEARTGOLD_BATTLE_BATTLE_ANIM_EFFECTS_H

#include "battle/battle_anim_funcs.h"
#include "battle/battle_anim_util.h"

#include "bg_window.h"
#include "palette.h"
#include "pokepic.h"
#include "sprite_system.h"
#include "sys_task.h"

// One sprite of BattleAnimSpriteFunc_StackedSpritesOnDefender, pulsing its x scale
typedef struct BattleAnimScaleSprite {
    ManagedSprite *sprite;
    BattleAnimLinearMotion scale;
    BOOL flip;
    int repeats;
    int start; // start and end scale (x10) swap after each pulse
    int end;
} BattleAnimScaleSprite;

// Parameters of the OBJ window BG scroll effects (BattleAnimObjWindowBg_Start)
typedef struct BattleAnimObjWindowBgParams {
    NarcId narcId;
    int graphic; // row of gBattleAnimObjWindowBgFiles
    int target;  // 0: attacker, otherwise defender
    s16 velocityX;
    s16 velocityY;
    s16 x;
    s16 y;
    u16 eva;
    u16 evb;
} BattleAnimObjWindowBgParams;

typedef struct BattleAnimSkewSlideWork BattleAnimSkewSlideWork;
typedef struct BattleAnimWaveOnBgWork BattleAnimWaveOnBgWork;
typedef struct BattleAnimRisingSpriteStretchWork BattleAnimRisingSpriteStretchWork;
typedef struct BattleAnimZigzagSlashWork BattleAnimZigzagSlashWork;
typedef struct BattleAnimZigzagFlashWork BattleAnimZigzagFlashWork;
typedef struct BattleAnimWaddlePicWork BattleAnimWaddlePicWork;
typedef struct BattleAnimAttackerStretchUpWork BattleAnimAttackerStretchUpWork;
typedef struct BattleAnimPulseSpriteWork BattleAnimPulseSpriteWork;
typedef struct BattleAnimAttackerDipWork BattleAnimAttackerDipWork;
typedef struct BattleAnimFanSpritesWork BattleAnimFanSpritesWork;
typedef struct BattleAnimDarkSpriteJumpWork BattleAnimDarkSpriteJumpWork;
typedef struct BattleAnimScaleSpritesWork BattleAnimScaleSpritesWork;
typedef struct BattleAnimOrbitSpritesWork BattleAnimOrbitSpritesWork;
typedef struct BattleAnimFloatingSpritesWork BattleAnimFloatingSpritesWork;
typedef struct BattleAnimWobbleOnBgWork BattleAnimWobbleOnBgWork;
typedef struct BattleAnimObjWindowBgWork BattleAnimObjWindowBgWork;

void BattleAnimFunc_SkewSlideAttackerBg(BattleAnimSystem *system);
void BattleAnimFunc_FadeInShrinkSprite(BattleAnimSystem *system);
void BattleAnimFunc_ShakeDefenderBlackFlash(BattleAnimSystem *system);
void BattleAnimFunc_WaveDefenderOnBg(BattleAnimSystem *system);
void BattleAnimSpriteFunc_GrowRockShrinkSprite(BattleAnimSystem *system, SpriteSystem *spriteSystem, SpriteManager *spriteManager, ManagedSprite *sprite);
void BattleAnimSpriteFunc_StackedSpritesOnDefender(BattleAnimSystem *system, SpriteSystem *spriteSystem, SpriteManager *spriteManager, ManagedSprite *sprite);
void BattleAnimSpriteFunc_BoomerangSprite(BattleAnimSystem *system, SpriteSystem *spriteSystem, SpriteManager *spriteManager, ManagedSprite *sprite);
void BattleAnimFunc_AttackerCircleFade(BattleAnimSystem *system);
void BattleAnimSpriteFunc_RisingSpriteStretchAttacker(BattleAnimSystem *system, SpriteSystem *spriteSystem, SpriteManager *spriteManager, ManagedSprite *sprite);
void BattleAnimSpriteFunc_ZigzagSlash(BattleAnimSystem *system, SpriteSystem *spriteSystem, SpriteManager *spriteManager, ManagedSprite *sprite);
void BattleAnimSpriteFunc_ZigzagFlash(BattleAnimSystem *system, SpriteSystem *spriteSystem, SpriteManager *spriteManager, ManagedSprite *sprite);
void BattleAnimFunc_WaddlePic(BattleAnimSystem *system);
void BattleAnimFunc_AttackerStretchUp(BattleAnimSystem *system);
void BattleAnimSpriteFunc_PulseSpriteBrightnessFlash(BattleAnimSystem *system, SpriteSystem *spriteSystem, SpriteManager *spriteManager, ManagedSprite *sprite);
void BattleAnimFunc_AttackerDip(BattleAnimSystem *system);
void BattleAnimFunc_AttackerLungeKnockback(BattleAnimSystem *system);
void BattleAnimSpriteFunc_FanSpritesAroundAttacker(BattleAnimSystem *system, SpriteSystem *spriteSystem, SpriteManager *spriteManager, ManagedSprite *sprite);
void BattleAnimSpriteFunc_ShrinkAttackerIntoSprite(BattleAnimSystem *system, SpriteSystem *spriteSystem, SpriteManager *spriteManager, ManagedSprite *sprite);
void BattleAnimFunc_DarkSpriteJump(BattleAnimSystem *system);
void BattleAnimSpriteFunc_ScaleSpritesOverDefender(BattleAnimSystem *system, SpriteSystem *spriteSystem, SpriteManager *spriteManager, ManagedSprite *sprite);
void BattleAnimSpriteFunc_SpritesOrbitAttackerBobbing(BattleAnimSystem *system, SpriteSystem *spriteSystem, SpriteManager *spriteManager, ManagedSprite *sprite);
void BattleAnimSpriteFunc_FloatingSpriteStream(BattleAnimSystem *system, SpriteSystem *spriteSystem, SpriteManager *spriteManager, ManagedSprite *sprite);
void BattleAnimFunc_WobbleDefenderOnBg(BattleAnimSystem *system);
void BattleAnimFunc_WindowSweepFlashWhite(BattleAnimSystem *system);
void BattleAnimSpriteFunc_FadeSpriteAnim(BattleAnimSystem *system, SpriteSystem *spriteSystem, SpriteManager *spriteManager, ManagedSprite *sprite);
void BattleAnimFunc_StretchShakeDefender(BattleAnimSystem *system);
void BattleAnimFunc_ShootSpriteFromAttacker(BattleAnimSystem *system);
void BattleAnimFunc_AttackerShakeLunge(BattleAnimSystem *system);
void BattleAnimFunc_DefenderKnockback(BattleAnimSystem *system);
void BattleAnimSpriteFunc_RisingShrinkSprites(BattleAnimSystem *system, SpriteSystem *spriteSystem, SpriteManager *spriteManager, ManagedSprite *sprite);
void BattleAnimSpriteFunc_PaletteCycleSprite(BattleAnimSystem *system, SpriteSystem *spriteSystem, SpriteManager *spriteManager, ManagedSprite *sprite);
void BattleAnimSpriteFunc_SpriteRowSequence(BattleAnimSystem *system, SpriteSystem *spriteSystem, SpriteManager *spriteManager, ManagedSprite *sprite);
void BattleAnimSpriteFunc_SpritesOrbitAttacker(BattleAnimSystem *system, SpriteSystem *spriteSystem, SpriteManager *spriteManager, ManagedSprite *sprite);
void BattleAnimObjWindowBg_Start(BattleAnimSystem *system, const BattleAnimObjWindowBgParams *params);
void BattleAnimFunc_ObjWindowBgScrollUp(BattleAnimSystem *system);
void BattleAnimFunc_ObjWindowBgScrollDown(BattleAnimSystem *system);
void BattleAnimFunc_ObjWindowBgScrollUpAlt(BattleAnimSystem *system);
void BattleAnimFunc_ObjWindowBgScrollDownFast(BattleAnimSystem *system);

// Battle animation system accessors
ManagedSprite *BattleAnimSystem_GetSprite(BattleAnimSystem *system, int index);
u32 BattleAnimSystem_GetBgLayer(BattleAnimSystem *system, int index);
u8 BattleAnimUtil_GetSpritePaletteSlot(ManagedSprite *sprite);
int BattleAnimUtil_GetBattlerSideSign(BattleAnimSystem *system, int battler);

#endif // POKEHEARTGOLD_BATTLE_BATTLE_ANIM_EFFECTS_H
