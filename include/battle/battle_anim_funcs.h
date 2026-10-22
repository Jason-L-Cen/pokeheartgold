#ifndef POKEHEARTGOLD_BATTLE_BATTLE_ANIM_FUNCS_H
#define POKEHEARTGOLD_BATTLE_BATTLE_ANIM_FUNCS_H

// Overlay 7: effect functions of the battle move animation scripts (gBattleAnimFuncTable / sprite function table
// entries) and the tasks they start.

#include "battle/battle_anim_util.h"

#include "bg_window.h"
#include "palette.h"
#include "pokepic.h"
#include "spl.h"
#include "sprite_system.h"
#include "sys_task.h"

typedef struct BattleAnimSpritePopWork BattleAnimSpritePopWork;
typedef struct BattleAnimEmitterMotionWork BattleAnimEmitterMotionWork;

void BattleAnimFunc_OneFrameTask(BattleAnimSystem *system);
void BattleAnimFunc_OneFrameSoundTask(BattleAnimSystem *system);
void BattleAnimFunc_OneFrameUncountedTask(BattleAnimSystem *system);
void BattleAnimSpriteFunc_SpreadSpriteCopies(BattleAnimSystem *system, SpriteSystem *spriteSystem, SpriteManager *spriteManager, ManagedSprite *sprite);
void BattleAnimFunc_ShakeTargets(BattleAnimSystem *system);
void BattleAnimFunc_SlidingBg(BattleAnimSystem *system);
void BattleAnimFunc_SlidingBg2(BattleAnimSystem *system);
void BattleAnimFunc_RotateBattler(BattleAnimSystem *system);
void BattleAnimFunc_CircleBattler(BattleAnimSystem *system);
void BattleAnimSpriteFunc_SpritePops(BattleAnimSystem *system, SpriteSystem *spriteSystem, SpriteManager *spriteManager, ManagedSprite *sprite);
void BattleAnimFunc_FadeBgPalette(BattleAnimSystem *system);
void BattleAnimFunc_BattlerPaletteFade(BattleAnimSystem *system);
void BattleAnimFunc_PulseBattlerSprite(BattleAnimSystem *system);
void BattleAnimFunc_StretchBattler(BattleAnimSystem *system);
void BattleAnimFunc_BlendBattlerSprites(BattleAnimSystem *system);
void BattleAnimFunc_SetBattlerVanished(BattleAnimSystem *system);
void BattleAnimFunc_BlinkBattler(BattleAnimSystem *system);
void BattleAnimFunc_SlideBattler(BattleAnimSystem *system);
void BattleAnimFunc_SlideBattler2(BattleAnimSystem *system);
void BattleAnimFunc_ShakeAndScaleBattler(BattleAnimSystem *system);
void BattleAnimFunc_ShakeAndScaleBattlerXY(BattleAnimSystem *system);
void BattleAnimFunc_ShowBattleBgInObjWindow(BattleAnimSystem *system);
void BattleAnimFunc_MoveBattlerBy(BattleAnimSystem *system);
void BattleAnimFunc_MoveBattlerSin(BattleAnimSystem *system);
void BattleAnimFunc_MoveBattlerLinear(BattleAnimSystem *system);
void BattleAnimFunc_MoveBattlerOffscreen(BattleAnimSystem *system);
void BattleAnimFunc_ResetBattlerPosition(BattleAnimSystem *system);
void BattleAnimFunc_BlendSpritePalette(BattleAnimSystem *system);
void BattleAnimFunc_ChangeBattlerHeight(BattleAnimSystem *system);
void BattleAnimFunc_RevealBattler(BattleAnimSystem *system);
void BattleAnimFunc_ShakeBg(BattleAnimSystem *system);
void BattleAnimFunc_SpriteMosaic(BattleAnimSystem *system);
void BattleAnimFunc_MoveEmitter(BattleAnimSystem *system);
void BattleAnimFunc_MoveEmitterVertical(BattleAnimSystem *system);
void BattleAnimFunc_MoveEmitterArc(BattleAnimSystem *system);
void BattleAnimFunc_EmitterSinMotion(BattleAnimSystem *system);
void BattleAnimSpriteFunc_MoveSpriteAndAnimate(BattleAnimSystem *system, SpriteSystem *spriteSystem, SpriteManager *spriteManager, ManagedSprite *sprite);
void BattleAnimSpriteFunc_SpriteOrbit(BattleAnimSystem *system, SpriteSystem *spriteSystem, SpriteManager *spriteManager, ManagedSprite *sprite);
void BattleAnimSpriteFunc_SpriteAboveDefender(BattleAnimSystem *system, SpriteSystem *spriteSystem, SpriteManager *spriteManager, ManagedSprite *sprite);
void BattleAnimSpriteFunc_SpriteArc(BattleAnimSystem *system, SpriteSystem *spriteSystem, SpriteManager *spriteManager, ManagedSprite *sprite);

// Battle animation system tasks and accessors
SysTask *BattleAnimSystem_CreateTask(BattleAnimSystem *system, SysTaskFunc func, void *data, u32 priority);
SysTask *BattleAnimSystem_CreateTaskDefaultPriority(BattleAnimSystem *system, SysTaskFunc func, void *data);
SysTask *BattleAnimSystem_CreateSoundTask(BattleAnimSystem *system, SysTaskFunc func, void *data, u32 priority);
void BattleAnimSystem_DestroyTask(BattleAnimSystem *system, SysTask *task);
void BattleAnimSystem_DestroySoundTask(BattleAnimSystem *system, SysTask *task);
SPLEmitter *BattleAnimSystem_GetParticleSystem(BattleAnimSystem *system, int index);
SPLEmitter *BattleAnimSystem_GetEmitter(BattleAnimSystem *system, int index);
u32 BattleAnimSystem_GetArg(BattleAnimSystem *system, int index);
u32 BattleAnimSystem_GetBattleBgPaletteMask(BattleAnimSystem *system);
void BattleAnimBackground_LoadBattleBgToLayerDefault(BattleAnimSystem *system, GFBgLayer layer);
void BattleAnimBackground_ResetLayer(BattleAnimSystem *system, GFBgLayer layer);
void BattleAnimSystem_GetSpriteTemplate(ManagedSpriteTemplate *template, BattleAnimSystem *system);
int BattleAnimSystem_GetBattlerHeight(BattleAnimSystem *system, int battler);
BOOL BattleAnimSystem_IsBattlerSemiInvulnerable(BattleAnimSystem *system, int battler);
int BattleAnimSystem_GetBasePriority(BattleAnimSystem *system);
int BattleAnimSystem_GetBgPriority(BattleAnimSystem *system, int kind);
BOOL BattleAnimSystem_ShouldFlipSide(BattleAnimSystem *system, int side);
u32 BattleAnimSystem_GetBackgroundFile(u32 bgId, int file);
void BattleAnimSystem_GetBattlerScreenPos(BattleAnimSystem *system, int battler, s16 *x, s16 *y);
s16 BattleAnimSystem_GetBattlerScreenCoord(BattleAnimSystem *system, int battler, u32 coord);
int BattleAnimUtil_GetBattlerSideSignByMode(BattleAnimSystem *system, int battler);

#endif // POKEHEARTGOLD_BATTLE_BATTLE_ANIM_FUNCS_H
