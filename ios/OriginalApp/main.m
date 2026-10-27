// SPDX-License-Identifier: GPL-3.0-or-later
#import <UIKit/UIKit.h>
#import <AVFAudio/AVFAudio.h>
#import "OriginalAudio.h"
#include "hgengine.h"
#include "hgengine-impl.h"
#include "NativeImports.h"
#include "NativeWASIImports.h"
// Nitro uses a 32-bit BOOL; keep that ABI distinct from Objective-C's bool.
#define BOOL NitroBOOL
#include "../Original/NativeFile.h"
#include "../Original/NativeBackup.h"
#include "../Original/NativeEvents.h"
#include "../Original/NativeDMA.h"
#include <nitro/os/native_reset.h>
#undef BOOL
#include "wasm-rt-impl.h"
#include <pthread.h>
#include <execinfo.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include "native_battle_ui.h"
#include "native_battle_bag_ui.h"
#include "native_battle_party_ui.h"
#include "native_party_ui.h"
#include "native_bag_ui.h"
#include "native_summary_ui.h"
#include "native_evolution_ui.h"
#include "native_pokedex_ui.h"
#include "native_naming_ui.h"
#include "native_options_ui.h"
#include "native_trainer_card_ui.h"
#include "native_pc_box_ui.h"
#include "native_field_choice_ui.h"
#include "native_field_menu_ui.h"

static BOOL OriginalStartMenuHasView(const HGNativeFieldMenuUIState *state) {
    return state && state->active==1 && !state->childActive && state->viewActive==1;
}
_Static_assert(sizeof(HGNativeFieldMenuUIState)==36,"original StartMenu public ABI");
static BOOL OriginalStartMenuFallback(const HGNativeFieldMenuUIState *state) {
    return OriginalStartMenuHasView(state) && (state->phase==6 || state->phase==7 || state->phase==19 || state->phase==20);
}
static BOOL OriginalStartMenuBlocksInput(const HGNativeFieldMenuUIState *state) {
    return state && state->active==1 && !state->childActive && !OriginalStartMenuFallback(state);
}
static BOOL OriginalStartMenuHandoff(const HGNativeFieldMenuUIState *old,const HGNativeFieldMenuUIState *next,BOOL was,BOOL scene,BOOL changed) {
    return was!=scene || (scene && changed) || (((old && old->active) || (next && next->active))
        && (!old || !next || old->active!=next->active || old->ownerGeneration!=next->ownerGeneration
            || old->childActive!=next->childActive || old->viewActive!=next->viewActive));
}

enum { OriginalBattleMenuBag=1, OriginalBattleMenuParty=2 };
static NSString *OriginalPartyStatusName(uint32_t status) {
    static NSString *const names[]={@"",@"麻痹",@"冰冻",@"睡眠",@"中毒",@"灼伤",@"濒死",@""};
    return status<sizeof(names)/sizeof(*names) ? names[status] : @"";
}
_Static_assert(sizeof(HGNativeBattleBagUIState)==536, "Battle Bag UI ABI");
_Static_assert(sizeof(HGNativeBattlePartyUIState)==688, "Battle Party UI ABI");
_Static_assert(offsetof(HGNativeBattlePartyUIState,selectedMove)==528 && offsetof(HGNativeBattlePartyUIState,moves)==532
    && offsetof(HGNativeBattlePartyUIState,moveToLearn)==580 && offsetof(HGNativeBattlePartyUIState,prompt)==584
    && offsetof(HGNativeBattlePartyUIState,message)==636, "Battle Party info ABI");
_Static_assert(sizeof(HGNativeSummaryUIState)==804 && sizeof(HGNativeSummaryUIMove)==92
    && sizeof(HGNativeSummaryUIInput)==16 && offsetof(HGNativeSummaryUIState,moves)==176, "Summary UI ABI");
_Static_assert(sizeof(HGNativeEvolutionUIState)==196 && offsetof(HGNativeEvolutionUIState,message)==40
    && offsetof(HGNativeEvolutionUIState,choices)==92, "Evolution UI ABI");
_Static_assert(sizeof(HGNativeCaptureUIState)==32 && offsetof(HGNativeCaptureUIState,callerPhase)==12
    && offsetof(HGNativeCaptureUIState,species)==24, "Capture UI ABI");
_Static_assert(sizeof(HGNativeNamingUIState)==16 && offsetof(HGNativeNamingUIState,ownerGeneration)==4
    && offsetof(HGNativeNamingUIState,revision)==8 && offsetof(HGNativeNamingUIState,ready)==12, "Naming UI ABI");
_Static_assert(sizeof(HGNativeOptionsUIState)==16 && offsetof(HGNativeOptionsUIState,ownerGeneration)==4
    && offsetof(HGNativeOptionsUIState,revision)==8 && offsetof(HGNativeOptionsUIState,ready)==12, "Options UI ABI");
_Static_assert(sizeof(HGNativePCBoxUIState)==52 && offsetof(HGNativePCBoxUIState,viewActive)==48, "PC Box UI ABI");
_Static_assert(sizeof(HGNativeFieldChoiceUIState)==36 && offsetof(HGNativeFieldChoiceUIState,viewActive)==32, "Field Choice UI ABI");
_Static_assert(sizeof(HGNativePokedexUIState)==5124 && offsetof(HGNativePokedexUIState,controls)==452
    && offsetof(HGNativePokedexUIState,visuals)==1988, "Pokedex UI ABI");
static BOOL OriginalBattleChoiceMode(uint32_t mode) {
    return mode==HG_BATTLE_UI_NEXT_MON || mode==HG_BATTLE_UI_KEEP_FORGET || mode==HG_BATTLE_UI_GIVE_UP || mode==HG_BATTLE_UI_YES_NO;
}
static BOOL OriginalBattlePartyInfoPhase(uint32_t phase) {
    return (phase>=HG_BATTLE_PARTY_UI_SUMMARY && phase<=HG_BATTLE_PARTY_UI_MOVE_DETAIL)
        || phase==HG_BATTLE_PARTY_UI_LEARN_LIST || phase==HG_BATTLE_PARTY_UI_LEARN_DETAIL;
}
static uint32_t OriginalBattleMenuKind(const HGNativeBattleBagUIState *bag, const HGNativeBattlePartyUIState *party) {
    if (bag && bag->active && ((bag->phase==HG_BATTLE_BAG_UI_POCKETS && bag->page==0)
        || (bag->phase==HG_BATTLE_BAG_UI_ITEMS && bag->page==1)
        || ((bag->phase==HG_BATTLE_BAG_UI_USE || bag->phase==HG_BATTLE_BAG_UI_ACK) && bag->page==2))) return OriginalBattleMenuBag;
    if (party && party->active && party->partyCount<=HG_BATTLE_PARTY_UI_MONS && party->mode==HG_BATTLE_PARTY_UI_LEARN_MOVE
        && party->selectedSlot<party->partyCount && party->mons[party->selectedSlot].slotType && !party->mons[party->selectedSlot].isEgg
        && party->moveToLearn && party->moveToLearn<=NUM_MOVES && party->selectedMove<=4
        && ((party->phase==HG_BATTLE_PARTY_UI_LEARN_LIST && party->page==6)
            || (party->phase==HG_BATTLE_PARTY_UI_LEARN_DETAIL && party->page==7))) {
        for (NSUInteger i=0;i<4;i++) if (!party->moves[i].moveId || party->moves[i].moveId>NUM_MOVES) return 0;
        return OriginalBattleMenuParty;
    }
    if (party && party->active && party->partyCount<=HG_BATTLE_PARTY_UI_MONS
        && (party->mode==HG_BATTLE_PARTY_UI_SWITCH || party->mode==HG_BATTLE_PARTY_UI_SWITCH_NO_CANCEL
            || (party->mode==HG_BATTLE_PARTY_UI_USE_ITEM && party->itemId==HG_BATTLE_PARTY_UI_POTION))
        && ((party->phase==HG_BATTLE_PARTY_UI_ROSTER && party->page==0)
            || (party->phase==HG_BATTLE_PARTY_UI_MON_MENU && party->page==1
                && (party->mode==HG_BATTLE_PARTY_UI_SWITCH || party->mode==HG_BATTLE_PARTY_UI_SWITCH_NO_CANCEL))
            || ((party->mode==HG_BATTLE_PARTY_UI_SWITCH || party->mode==HG_BATTLE_PARTY_UI_SWITCH_NO_CANCEL)
                && party->selectedSlot<HG_BATTLE_PARTY_UI_MONS && party->selectedSlot<party->partyCount
                && party->mons[party->selectedSlot].slotType && !party->mons[party->selectedSlot].isEgg
                && ((party->phase==HG_BATTLE_PARTY_UI_SUMMARY && party->page==2)
                    || (party->phase==HG_BATTLE_PARTY_UI_CHECK_MOVES && party->page==3)
                    || (party->phase==HG_BATTLE_PARTY_UI_MOVE_DETAIL && party->page==4 && party->selectedMove<4
                        && party->moves[party->selectedMove].moveId)))
            || ((party->phase==HG_BATTLE_PARTY_UI_MESSAGE || party->phase==HG_BATTLE_PARTY_UI_ACK) && party->page<=1))) return OriginalBattleMenuParty;
    return 0;
}

#ifdef HEARTGOLD_ORIGINAL_APP_CHECK
#include "../Original/NativeAudioCheck.h"
#if !defined(HEARTGOLD_ORIGINAL_CAPTURE_REGISTRATION_CHECK) && !defined(HEARTGOLD_ORIGINAL_PC_BOX_CHECK)
#include "../tests/battle_menus_qa.h"
#endif
#ifdef HEARTGOLD_ORIGINAL_PC_BOX_CHECK
#include "../tests/pc_box_qa.h"
#endif
#ifdef HEARTGOLD_ORIGINAL_SUMMARY_TM_CHECK
#include "../tests/summary_learn_guest.inc"
#include "constants/balls.h"
#include "constants/charcode.h"
#include "constants/heap.h"
#include "constants/items.h"
#include "constants/maps.h"
#include "constants/pokemon.h"
#define BOOL NitroBOOL
#include "player_data.h"
#undef BOOL
_Static_assert(sizeof(HGSummaryLearnQAState)==232 && offsetof(HGSummaryLearnQAState,hmWarning)==180,"Private Summary TM observer ABI");
static const uint32_t SummaryTMAttributes[]={MON_DATA_PERSONALITY,MON_DATA_OT_ID,MON_DATA_SPECIES,MON_DATA_FORM,MON_DATA_IS_EGG,MON_DATA_HELD_ITEM,
    MON_DATA_HP,MON_DATA_MAX_HP,MON_DATA_LEVEL,MON_DATA_STATUS,MON_DATA_MOVE1,MON_DATA_MOVE2,MON_DATA_MOVE3,MON_DATA_MOVE4,
    MON_DATA_MOVE1_PP,MON_DATA_MOVE2_PP,MON_DATA_MOVE3_PP,MON_DATA_MOVE4_PP,MON_DATA_MOVE1_PP_UPS,MON_DATA_MOVE2_PP_UPS,MON_DATA_MOVE3_PP_UPS,MON_DATA_MOVE4_PP_UPS,
    MON_DATA_MOVE1_MAX_PP,MON_DATA_MOVE2_MAX_PP,MON_DATA_MOVE3_MAX_PP,MON_DATA_MOVE4_MAX_PP};
#endif
#include <pthread.h>
typedef struct CheckAudioAppSpan {
    uint64_t kind, startNs, scene, tick;
} CheckAudioAppSpan;
static void CheckAudioAppStage(CheckAudioAppSpan *span, uint64_t kind, uint64_t result) {
    uint64_t now=HGNativeAudioCheckNow();
    if (span->kind) HGNativeAudioCheckSpan(span->kind,span->startNs,now,span->scene,span->tick,result);
    span->kind=kind; span->startNs=now;
}
static NSString *CheckAudioKindName(uint64_t kind) {
    static NSString *const names[]={@"none",@"service",@"synth",@"output",@"sdkPump",@"file",@"wave",@"appRender",@"appOther",@"serviceGap"};
    _Static_assert(sizeof(names)/sizeof(names[0])==HG_AUDIO_CHECK_KIND_COUNT,"audio trace kind labels");
    return kind<HG_AUDIO_CHECK_KIND_COUNT ? names[kind] : @"unknown";
}
static NSDictionary *CheckAudioRecord(const HGNativeAudioCheckRecord *record) {
    return @{@"kind":@(record->kind),@"name":CheckAudioKindName(record->kind),@"startNs":@(record->startNs),
        @"endNs":@(record->endNs),@"serviceSeq":@(record->serviceSeq),@"a":@(record->a),@"b":@(record->b),@"c":@(record->c),
        @"cpuWallStartNs":@(record->cpuWallStartNs),@"cpuElapsedNs":@(record->cpuElapsedNs)};
}
static NSDictionary *CheckAudioGap(const HGNativeAudioCheckGap *gap) {
    if (!gap->record.kind) return @{};
    NSMutableDictionary *spans=[NSMutableDictionary new];
    for (unsigned i=1;i<HG_AUDIO_CHECK_KIND_COUNT;i++) spans[CheckAudioKindName(i)]=@{@"count":@(gap->spanCount[i]),@"totalNs":@(gap->spanNs[i])};
    return @{@"record":CheckAudioRecord(&gap->record),@"previousServiceSeq":@(gap->previousServiceSeq),
        @"previousFlags":@(gap->previousFlags),@"previousDueFrames":@(gap->previousDueFrames),@"previousOutputFrames":@(gap->previousOutputFrames),
        @"dueFrames":@(gap->dueFrames),@"outputFrames":@(gap->outputFrames),@"synthNs":@(gap->synthNs),@"outputNs":@(gap->outputNs),
        @"completedSpanDeltas":spans};
}
static NSDictionary *CheckAudioTrace(NSData *data, uint64_t capturedAtNs) {
    if (data.length!=sizeof(HGNativeAudioCheckSnapshot)) return @{@"available":@NO};
    const HGNativeAudioCheckSnapshot *state=data.bytes;
    NSMutableArray *records=[NSMutableArray new]; NSMutableDictionary *costs=[NSMutableDictionary new];
    if (state->version!=HG_AUDIO_CHECK_VERSION || state->recordCount>HG_AUDIO_CHECK_RECORDS || state->nextRecord>=HG_AUDIO_CHECK_RECORDS) return @{@"available":@NO};
    for (uint64_t i=0;i<state->recordCount;i++) {
        uint64_t index=(state->nextRecord+HG_AUDIO_CHECK_RECORDS-state->recordCount+i)%HG_AUDIO_CHECK_RECORDS;
        [records addObject:CheckAudioRecord(&state->records[index])];
    }
    for (unsigned i=1;i<HG_AUDIO_CHECK_KIND_COUNT;i++) costs[CheckAudioKindName(i)]=@{@"count":@(state->costs[i].count),
        @"totalNs":@(state->costs[i].totalNs),@"maxNs":@(state->costs[i].maxNs)};
    return @{@"available":@YES,@"version":@(state->version),@"bootEpoch":@(state->bootEpoch),@"bootStartNs":@(state->bootStartNs),
        @"capturedAtNs":@(capturedAtNs),@"thresholdNs":@(state->thresholdNs),@"sequence":@(state->sequence),
        @"recordCount":@(state->recordCount),@"overwriteCount":@(state->overwriteCount),@"invalidCalls":@(state->invalidCalls),
        @"clockFailures":@(state->clockFailures),@"otherThreadCalls":@(state->otherThreadCalls),
        @"serviceSeq":@(state->serviceSeq),@"serviceCount":@(state->serviceCount),@"dueFrames":@(state->dueFrames),
        @"outputFrames":@(state->outputFrames),@"synthNs":@(state->synthNs),@"outputNs":@(state->outputNs),
        @"lastServiceStartNs":@(state->lastServiceStartNs),@"lastServiceEndNs":@(state->lastServiceEndNs),@"lastSource":@(state->lastSource),
        @"lastFlags":@(state->lastFlags),@"lastDueFrames":@(state->lastDueFrames),@"lastOutputFrames":@(state->lastOutputFrames),
        @"costs":costs,@"firstServiceGap":CheckAudioGap(&state->firstServiceGap),@"largestServiceGap":CheckAudioGap(&state->largestServiceGap),@"records":records};
}
#endif

WASM_RT_THREAD_LOCAL wasm_rt_jmp_buf g_wasm_rt_jmp_buf;
void HGAppTrap(wasm_rt_trap_t code) {
    void *frames[48];
    fprintf(stderr,"Original iOS engine trap: %s\n",wasm_rt_strerror(code));
    backtrace_symbols_fd(frames,backtrace(frames,48),STDERR_FILENO);
    WASM_RT_LONGJMP(g_wasm_rt_jmp_buf,code);
}

@interface OriginalKeyButton : UIButton
@property(nonatomic) NSUInteger inputGeneration;
#ifdef HEARTGOLD_ORIGINAL_APP_CHECK
@property(nonatomic,strong) NSMutableArray *heldInputCompletions;
#endif
@end
@implementation OriginalKeyButton
- (BOOL)accessibilityActivate {
    [self sendActionsForControlEvents:UIControlEventTouchDown];
    NSUInteger generation=self.inputGeneration;
    dispatch_block_t release=^{ if (generation==self.inputGeneration) [self sendActionsForControlEvents:UIControlEventTouchUpInside]; };
#ifdef HEARTGOLD_ORIGINAL_APP_CHECK
    if (_heldInputCompletions) [_heldInputCompletions addObject:release]; else
#endif
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW,120*NSEC_PER_MSEC),dispatch_get_main_queue(),release);
    return YES;
}
@end

@interface OriginalMenuElement : UIAccessibilityElement
@property(nonatomic,copy) BOOL (^activate)(void);
@end
@implementation OriginalMenuElement
- (BOOL)accessibilityActivate { return self.activate ? self.activate() : NO; }
@end

static UIImage *OriginalPixelImage(NSData *frame,uint32_t width,uint32_t height) {
    if (!width || !height || frame.length != (uint64_t)width*height*4) return nil;
    CGDataProviderRef provider=CGDataProviderCreateWithCFData((__bridge CFDataRef)frame);
    CGColorSpaceRef color=CGColorSpaceCreateDeviceRGB();
    CGImageRef image=CGImageCreate(width,height,8,32,width*4,color,kCGBitmapByteOrder32Big | kCGImageAlphaLast,provider,NULL,NO,kCGRenderingIntentDefault);
    UIImage *result=image ? [UIImage imageWithCGImage:image] : nil;
    if (image) CGImageRelease(image);
    CGColorSpaceRelease(color); CGDataProviderRelease(provider); return result;
}

static UIImage *OriginalWindowImage(const HGNativePartyUIWindow *window,const uint8_t *memory,uint64_t memorySize) {
    uint32_t width=window->tileWidth*8,height=window->tileHeight*8;
    uint64_t bytes=(uint64_t)window->tileWidth*window->tileHeight*32;
    if (!memory || !window->pixelBuffer || !window->tileWidth || window->tileWidth>32 || !window->tileHeight || window->tileHeight>6
        || window->clearColor>15 || window->engine>1 || window->pixelBuffer>memorySize || bytes>memorySize-window->pixelBuffer) return nil;
    const uint8_t *tiles=memory+window->pixelBuffer;
    NSMutableData *pixels=[NSMutableData dataWithLength:width*height*4]; uint8_t *out=pixels.mutableBytes;
    for (uint32_t y=0;y<height;y++) for (uint32_t x=0;x<width;x++) {
        uint32_t offset=((y/8)*window->tileWidth+x/8)*32+(y%8)*4+(x%8)/2;
        uint32_t index=(tiles[offset]>>((x&1)*4))&15; uint16_t color=window->palette[index];
        uint8_t *pixel=out+(y*width+x)*4;
        for (unsigned c=0;c<3;c++) pixel[c]=(((color>>(c*5))&31)*527+23)>>6;
        pixel[3]=index==window->clearColor ? 0 : 255;
    }
    return OriginalPixelImage(pixels,width,height);
}

static UIImage *OriginalFrameRegion(NSData *frame,uint32_t left,uint32_t top,uint32_t width,uint32_t height) {
    if (frame.length!=256*192*4 || !width || width>256 || !height || height>192 || left>256-width || top>192-height) return nil;
    NSMutableData *pixels=[NSMutableData dataWithLength:width*height*4];
    for (uint32_t y=0;y<height;y++) memcpy((uint8_t *)pixels.mutableBytes+y*width*4,(const uint8_t *)frame.bytes+((top+y)*256+left)*4,width*4);
    return OriginalPixelImage(pixels,width,height);
}

static UIImage *OriginalPartyCard(NSData *frame,const HGNativePartyUIMon *mon) {
    // Layout 0's original 128x48 panels include the status and held-item sprites.
    if (mon->hitX<64 || mon->hitY<24) return nil;
    return OriginalFrameRegion(frame,mon->hitX-64,mon->hitY-24,128,48);
}

typedef struct { uint32_t generation,revision,event,x,y; } OriginalTrainerTouchEvent;
_Static_assert(sizeof(HGNativeTrainerCardUIState)==68 && sizeof(OriginalTrainerTouchEvent)==20,"Trainer Card UI/input ABI");

static void OriginalSetButtonContent(UIButton *button,UIButtonConfiguration *configuration,UIImage *image,NSString *fallbackTitle) {
    image=[image isKindOfClass:UIImage.class] ? image : nil;
    if (!configuration && [[button titleForState:UIControlStateNormal] isEqualToString:fallbackTitle]) return;
    [UIView performWithoutAnimation:^{
        if (configuration) {
            [button setTitle:nil forState:UIControlStateNormal];
            configuration.image=image;
            configuration.title=image ? @"" : fallbackTitle;
            button.configuration=configuration;
        } else [button setTitle:fallbackTitle forState:UIControlStateNormal];
        [button layoutIfNeeded];
    }];
}

@interface OriginalJoystick : UIControl
@property(nonatomic,readonly) CGPoint location;
@property(nonatomic,readonly) BOOL pressed;
@property(nonatomic,readonly) BOOL cancelled;
#ifdef HEARTGOLD_ORIGINAL_PC_BOX_CHECK
- (void)checkPCBoxEventAt:(CGPoint)point pressed:(BOOL)pressed;
#endif
@end
@implementation OriginalJoystick
#ifdef HEARTGOLD_ORIGINAL_PC_BOX_CHECK
- (void)checkPCBoxEventAt:(CGPoint)point pressed:(BOOL)pressed {
    _location=point; _pressed=pressed; _cancelled=NO;
    [self sendActionsForControlEvents:UIControlEventValueChanged];
}
#endif
- (BOOL)beginTrackingWithTouch:(UITouch *)touch withEvent:(UIEvent *)event {
    _location=[touch locationInView:self]; _pressed=YES; _cancelled=NO;
    [self sendActionsForControlEvents:UIControlEventValueChanged]; return YES;
}
- (BOOL)continueTrackingWithTouch:(UITouch *)touch withEvent:(UIEvent *)event {
    _location=[touch locationInView:self];
    [self sendActionsForControlEvents:UIControlEventValueChanged]; return YES;
}
- (void)endTrackingWithTouch:(UITouch *)touch withEvent:(UIEvent *)event {
    _pressed=NO; [self sendActionsForControlEvents:UIControlEventValueChanged];
}
- (void)cancelTrackingWithEvent:(UIEvent *)event {
    _pressed=NO; _cancelled=YES; [self sendActionsForControlEvents:UIControlEventValueChanged];
}
@end

@interface OriginalController : UIViewController {
    NSCondition *_condition;
    BOOL _paused, _stopping, _inactive, _interrupted, _interruptionEnded;
    OriginalAudio *_audio;
    uint32_t _keys, _pendingKeys, _joystickKeys, _pendingJoystickKeys, _touchX, _touchY, _selectedEngine;
    uint32_t _submittedKeys, _submittedJoystickKeys, _lastPadReadCount;
    BOOL _joystickNeedsReset;
    BOOL _touchPressed;
    NSUInteger _inputGeneration;
    NSData *_frame;
    NSString *_failure;
    UIImageView *_canvas;
    UILabel *_status;
    CADisplayLink *_displayLink;
    NSMutableDictionary<NSNumber *, UIButton *> *_buttons;
    UIButton *_menuButton;
    UIView *_fieldMenuShade;
    UIImageView *_fieldMenu;
    NSArray<OriginalMenuElement *> *_fieldMenuElements;
    NSArray<NSArray<NSNumber *> *> *_fieldMenuHitboxes;
    NSArray<NSString *> *_fieldMenuNames;
    NSData *_fieldMenuFrame;
    uint32_t _fieldMenuIcons;
    BOOL _fieldMenuOpen, _automaticMenuEngine, _battleScene;
    NSData *_startMenuUIState,*_startMenuRawUIState,*_displayedStartMenuUIState,*_presentedStartMenuUIState,*_startMenuMainFrame,*_startMenuSubFrame;
    BOOL _startMenuScene,_startMenuBlocksInput,_startMenuOwnedInputs,_startMenuChoicePending,_startMenuInputActive,_startMenuInputCooling,_startMenuReleasePending;
    BOOL _startMenuChoiceTouch,_startMenuInputTouch;
    HGNativeFieldMenuUIState _startMenuChoice,_startMenuInput;
    uint32_t _startMenuChoiceKeys,_startMenuChoiceX,_startMenuChoiceY,_startMenuChoiceEngine;
    uint32_t _startMenuInputKeys,_startMenuInputX,_startMenuInputY,_startMenuInputEngine,_startMenuInputReads,_startMenuReleaseReads;
    uint32_t _startMenuFrameEngine,_displayedStartMenuEngine,_startMenuRawEngine;
    uint32_t _touchEngine;
    OriginalJoystick *_joystick;
    UIView *_joystickThumb;
    UIView *_battleBar;
    NSArray<UIButton *> *_battleButtons;
    UIButton *_battleBack;
    UIButton *_battleBall;
    NSArray<NSString *> *_moveNames, *_speciesNames;
    NSData *_battleUIState, *_displayedBattleUIState;
    uint32_t _battleMode, _battleChoiceRevision, _battleChoiceIndex;
    BOOL _battleChoicePending;
    UIView *_battleMenuPanel;
    UILabel *_battleMenuTitle;
    UIImageView *_battleMenuPreview, *_battleMenuSelected, *_battleMenuQuantity, *_battleMenuDescription, *_battleMenuMessage;
    NSArray<UIButton *> *_battleMenuButtons;
    NSArray<UIImageView *> *_battleMenuImages;
    NSData *_battleBagUIState, *_battlePartyUIState, *_displayedBattleBagUIState, *_displayedBattlePartyUIState;
    NSDictionary *_battleMenuPictures, *_battleMenuOwnerPictures;
    BOOL _battleMenuScene, _battleMenuChoicePending, _battleMenuTouchActive, _battleMenuTouchCooling;
    uint32_t _battleMenuKind, _battleMenuOwnerKind, _battleMenuOwnerRevision, _battleMenuOwnerPhase;
    uint32_t _battleMenuChoiceKind, _battleMenuChoiceRevision, _battleMenuChoicePhase, _battleMenuChoiceIndex;
    uint32_t _battleMenuTouchKind, _battleMenuTouchRevision, _battleMenuTouchReads, _battleMenuTouchX, _battleMenuTouchY;
    UIView *_partyPanel;
    UIScrollView *_partyActionScroll;
    UIButton *_partyBack;
    UILabel *_partyTitle;
    NSArray<UIButton *> *_partyMons, *_partyActions;
    NSArray<UIImageView *> *_partyCards;
    UIImageView *_partySelectedName, *_partySelectedItem, *_partyMessage, *_partyBody;
    NSArray<NSString *> *_partyActionNames;
    NSData *_partyUIState, *_displayedPartyUIState;
    NSDictionary *_partyPictures, *_partyOwnerPictures;
    BOOL _partyScene, _partyChoicePending, _partyTouchActive;
    uint32_t _partyChoiceRevision, _partyChoicePhase, _partyChoiceKind, _partyChoiceIndex;
    uint32_t _partyTouchRevision, _partyTouchReads, _partyTouchX, _partyTouchY;
    UIView *_bagPanel;
    UIImageView *_bagPreview, *_bagSelectedEntry, *_bagPrompt, *_bagPage;
    NSArray<UIButton *> *_bagPockets, *_bagItems, *_bagActions;
    NSArray<UIImageView *> *_bagPocketImages, *_bagItemImages;
    NSArray<UIButton *> *_bagNavigation;
    NSArray<NSString *> *_bagActionNames, *_itemNames, *_pocketNames;
    NSData *_bagUIState, *_displayedBagUIState;
    NSDictionary *_bagPictures, *_bagOwnerPictures;
    BOOL _bagScene, _bagChoicePending, _bagTouchActive;
    uint32_t _bagChoiceRevision, _bagChoicePhase, _bagChoiceKind, _bagChoiceIndex;
    uint32_t _bagTouchRevision, _bagTouchReads, _bagTouchX, _bagTouchY;
    UIView *_summaryPanel;
    UIImageView *_summaryPreview, *_summaryContent, *_summaryTitle;
    NSArray<UIButton *> *_summaryMoves, *_summaryNavigation;
    NSArray<UIImageView *> *_summaryMoveImages;
    NSData *_summaryUIState, *_displayedSummaryUIState;
    NSDictionary *_summaryPictures, *_summaryOwnerPictures;
    BOOL _summaryScene, _summaryMovePanel, _summaryMoveDetail, _summaryChoicePending, _summaryInputActive;
    uint32_t _summaryChoiceRevision, _summaryChoicePhase, _summaryChoiceKind, _summaryChoiceIndex;
    uint32_t _summaryInputRevision, _summaryInputPhase, _summaryInputReads, _summaryInputKeys, _summaryTouchX, _summaryTouchY;
    BOOL _summaryTouchPressed;
    NSData *_evolutionUIState, *_displayedEvolutionUIState;
    NSDictionary *_evolutionPictures;
    BOOL _evolutionScene, _evolutionChoicePending, _evolutionInputActive, _evolutionInputCooling;
    uint32_t _evolutionChoiceRevision, _evolutionChoicePhase, _evolutionChoiceKind, _evolutionChoiceIndex;
    uint32_t _evolutionInputRevision, _evolutionInputPhase, _evolutionInputKind, _evolutionInputIndex, _evolutionInputReads, _evolutionReleaseReads, _evolutionInputKeys, _evolutionTouchX, _evolutionTouchY;
    NSData *_optionsUIState, *_displayedOptionsUIState;
    UIImageView *_optionsPreview;
    UIImage *_optionsPreviewImage;
    HGNativeOptionsUIState _optionsChoice, _optionsInput;
    BOOL _optionsScene, _optionsChoicePending, _optionsInputActive, _optionsInputCooling;
    uint32_t _optionsOwnerGeneration, _optionsChoiceKeys, _optionsInputKeys, _optionsInputReads, _optionsReleaseReads;
    UIImageView *_fieldChoicePreview;
    UIImage *_fieldChoicePreviewImage, *_fieldChoiceOwnerPreview;
    NSData *_fieldChoiceUIState, *_displayedFieldChoiceUIState, *_fieldChoiceOwnerFrame;
    HGNativeFieldChoiceUIState _fieldChoiceChoice, _fieldChoiceInput;
    BOOL _fieldChoiceScene, _fieldChoiceChoicePending, _fieldChoiceInputActive, _fieldChoiceInputCooling;
    uint32_t _fieldChoiceOwnerGeneration, _fieldChoiceChoiceKeys, _fieldChoiceInputKeys, _fieldChoiceInputReads, _fieldChoiceReleaseReads, _fieldChoiceFrameGeneration;
    NSData *_namingUIState, *_displayedNamingUIState;
    HGNativeNamingUIState _namingChoice, _namingInput;
    BOOL _namingScene, _namingChoicePending, _namingChoiceTouch, _namingInputActive, _namingInputCooling, _namingInputTouch;
    uint32_t _namingOwnerGeneration, _namingChoiceKeys, _namingChoiceX, _namingChoiceY;
    uint32_t _namingInputKeys, _namingInputX, _namingInputY, _namingInputReads, _namingReleaseReads;

    UIView *_trainerCardPanel;
    UIImageView *_trainerCardBody, *_trainerCardFooter, *_trainerCardBadges;
    OriginalJoystick *_trainerCardSurface;
    OriginalKeyButton *_trainerCardDone;
    NSData *_trainerCardUIState, *_displayedTrainerCardUIState, *_trainerCardRawUIState;
    NSDictionary *_trainerCardPictures;
    BOOL _trainerCardScene, _trainerCardChoicePending, _trainerCardInputActive, _trainerCardInputCooling;
    BOOL _trainerCardTracking, _trainerCardRawDown, _trainerCardReleasePending, _trainerCardTouchCooling, _trainerCardSurfaceDown;
    id _trainerCardTouchSource;
    uint32_t _trainerCardOwnerGeneration, _trainerCardChoiceKeys, _trainerCardInputKeys, _trainerCardInputReads, _trainerCardReleaseReads;
    uint32_t _trainerCardTouchX, _trainerCardTouchY, _trainerCardTouchReleaseReads, _trainerCardTouchStartReads;
    HGNativeTrainerCardUIState _trainerCardChoice, _trainerCardInput, _trainerCardGesture;
    HGNativeTrainerCardUITouchLease _trainerCardTouchLease;
    NSMutableArray<NSData *> *_trainerCardTouches;
    UIView *_pcBoxPanel;
    UIImageView *_pcBoxWorkspace, *_pcBoxFooter, *_pcBoxInfo;
    NSArray<UIImageView *> *_pcBoxActions;
    NSData *_pcBoxUIState, *_displayedPCBoxUIState;
    NSDictionary *_pcBoxPictures, *_pcBoxOwnerPictures;
    HGNativePCBoxUIState _pcBoxChoice, _pcBoxInput;
    BOOL _pcBoxScene, _pcBoxChoicePending, _pcBoxInputActive, _pcBoxInputCooling;
    uint32_t _pcBoxOwnerGeneration, _pcBoxChoiceKeys, _pcBoxInputKeys, _pcBoxInputReads, _pcBoxReleaseReads;
    NSData *_captureUIState, *_displayedCaptureUIState;
    BOOL _captureScene, _captureChoicePending, _captureInputActive, _captureInputCooling;
    uint32_t _captureChoiceGeneration, _captureChoiceRevision, _captureInputGeneration, _captureInputRevision, _captureInputReads, _captureReleaseReads;
    UIView *_pokedexPanel;
    UIScrollView *_pokedexContent, *_pokedexDetails;
    NSArray<UIImageView *> *_pokedexImages;
    NSArray<UIButton *> *_pokedexButtons;
    NSArray<UIPanGestureRecognizer *> *_pokedexPans;
    NSDictionary *_pokedexLabels, *_pokedexPictures, *_pokedexOwnerPictures, *_pokedexDisplayPictures;
    NSData *_pokedexUIState, *_displayedPokedexUIState;
    BOOL _pokedexScene, _pokedexChoicePending, _pokedexInputActive, _pokedexInputCooling, _pokedexGestureLive, _pokedexGestureAdmitted, _pokedexGestureContinued;
    uint32_t _pokedexChoiceGeneration, _pokedexChoiceRevision, _pokedexChoicePhase, _pokedexChoiceKind, _pokedexChoiceIndex;
    uint32_t _pokedexInputGeneration, _pokedexInputRevision, _pokedexInputPhase, _pokedexInputFamily, _pokedexInputKind, _pokedexInputIndex, _pokedexInputReads, _pokedexReleaseReads;
    uint32_t _pokedexGestureValue, _pokedexGestureStartValue, _pokedexGesturePulse, _pokedexSampledGesturePulse, _pokedexGestureCommitReads;
    NSInteger _pokedexGestureControl;
    HGNativePokedexUIInput _pokedexInput;
    HGNativePokedexUITrack _pokedexGestureTrack;
    w2c_hgengine *_game;
    struct w2c_env *_env;
    struct w2c_wasi__snapshot__preview1 *_wasi;
    const vu16 *_displayStatus, *_displayControl;
    uint32_t _frameBuffer, _lastFrame, _preferredViewWidth, _preferredBattleWidth, _frameWidth, _frameEngine, _displayedWidth, _displayedEngine, _fieldMapId;
#ifdef HEARTGOLD_ORIGINAL_APP_CHECK
    uint32_t _publishedFrames, _bootCount, _bootFrameBase, _checkedBootParameter;
#ifdef HEARTGOLD_ORIGINAL_PC_BOX_CHECK
    NSData *_pcBoxQAState,*_displayedPCBoxQAState,*_pcBoxQARequestExpected;
    uint32_t _pcBoxQAPadReads,_displayedPCBoxQAPadReads,_pcBoxQAOwnerFrame,_displayedPCBoxQAOwnerFrame;
    uint32_t _pcBoxQAPublishedFrame,_pcBoxQADisplayedPublishedFrame;
    uint32_t _pcBoxQARequestKind,_pcBoxQARequestSerial,_pcBoxQARequestCompleted;
    BOOL _pcBoxQARequestPending,_pcBoxQARequestAccepted;
    uint32_t _pcBoxQAPulseStage,_pcBoxQAPulseKey,_pcBoxQAPulseReads,_pcBoxQAPulseReleaseReads,_pcBoxQAPulseHeld,_pcBoxQAPulseError;
    BOOL _pcBoxQAPulseGeneric, _pcBoxQAPulseMessage;
    NSMutableDictionary *_pcBoxCheckRuntime;
#endif
    BOOL _suspended, _finished, _checkResetPending, _checkedResetOccurred, _resetQuiesced, _restartClean, _audioChanging;
    NSData *_geometryState;
    NSData *_audioProducerTrace;
    uint64_t _audioProducerCapturedAtNs;
    NSData *_audioFirstShortTrace;
    uint64_t _audioFirstShortTimeNs, _audioFirstShortCapturedAtNs;
    NSMutableArray *_checkInputCompletions;
    NSData *_battleMenusQAState;
#ifdef HEARTGOLD_ORIGINAL_DEFAULT_CHECK
    NSData *_displayedBattleMenusQAState;
    BOOL _paletteFadeFinished, _displayedPaletteFadeFinished;
#endif
    NSArray *_displayedBagItemGlyphs;
    BOOL _checkBattleMenusRequestPending, _checkBattleMenusRequestAccepted;
    uint32_t _checkBattleMenusRequestRevision;
#ifdef HEARTGOLD_ORIGINAL_CAPTURE_REGISTRATION_CHECK
    NSData *_captureQAState,*_displayedCaptureQAState;
    BOOL _captureQARequestPending,_captureQARequestInFlight,_captureQARequestAccepted;
    uint32_t _captureQARequestRevision,_captureQARequestVariant,_captureQABootStep,_captureQABootFrame;
    uint32_t _captureQAPadReads,_displayedCaptureQAPadReads,_captureQAOwnerRequests;
    uint32_t _captureQAInputPresses,_captureQAInputReleases,_captureQALastReleaseReads,_captureQAMaxHeldReads;
#ifdef HEARTGOLD_ORIGINAL_NAMING_CHECK
    uint32_t _namingQAInputPresses,_namingQAInputReleases,_namingQALastReleaseReads,_namingQAMaxHeldReads;
    NSMutableArray *_namingQAInputs;
    BOOL _namingQAOverflow;
#endif
#endif
#ifdef HEARTGOLD_ORIGINAL_SUMMARY_TM_CHECK
    NSData *_summaryTMQAState, *_summaryTMBagBaseline, *_summaryTMFlagsBaseline;
    NSDictionary *_summaryTMFacts; UIImage *_summaryTMHMWarningImage; NSString *_summaryTMFixtureError;
    BOOL _summaryTMEnabled, _summaryTMObserverSeen, _summaryTMFixturePreparing, _summaryTMFixturePrepared;
    BOOL _summaryTMRequestPending, _summaryTMRequestAccepted, _summaryTMRawPending, _summaryTMRawActive, _summaryTMRawCooling, _summaryTMRawIdle, _summaryTMRawAccepted;
    uint32_t _summaryTMRequestRevision, _summaryTMPadReads, _summaryTMBootStep, _summaryTMBootFrame, _summaryTMBootInputStep, _summaryTMBootInputFrame;
    uint32_t _summaryTMRawKeys, _summaryTMRawX, _summaryTMRawY, _summaryTMRawReads, _summaryTMRawPresses, _summaryTMRawReleases, _summaryTMRawRejected, _summaryTMRawBagRevision;
    BOOL _summaryTMRawPressed; HGSummaryLearnQAState _summaryTMRawExpected;
    uint32_t _summaryTMMon, _summaryTMBag, _summaryTMFlags, _summaryTMProfile, _summaryTMDex, _summaryTMTmOffset, _summaryTMMonBaseline[33];
    uint32_t _summaryTMMoney, _summaryTMDexEnabled, _summaryTMDexSeen, _summaryTMDexOwned, _summaryTMNameLength, _summaryTMMapSection, _summaryTMOriginalPersonality, _summaryTMOriginalOtId;
#endif
#endif
}
- (void)engineFrame;
- (void)clearOptionsInputLocked;
- (void)queueOptionsKey:(OriginalKeyButton *)button;
- (void)takeOptionsInputLocked:(const HGNativeOptionsUIState *)state reads:(uint32_t)reads keys:(uint32_t *)keys;
- (void)updateOptionsControls:(NSData *)data preview:(UIImage *)preview paused:(BOOL)paused;
- (void)clearFieldChoiceInputLocked;
- (void)queueFieldChoiceKey:(OriginalKeyButton *)button;
- (void)takeFieldChoiceInputLocked:(const HGNativeFieldChoiceUIState *)state reads:(uint32_t)reads keys:(uint32_t *)keys;
- (void)updateFieldChoiceControls:(NSData *)data preview:(UIImage *)preview blocked:(BOOL)blocked paused:(BOOL)paused;
- (void)layoutFieldChoiceControls:(CGRect)area;
- (void)clearNamingInputLocked;
- (void)queueNamingInputKeys:(uint32_t)keys touch:(BOOL)touch point:(CGPoint)point;
- (void)takeNamingInputLocked:(const HGNativeNamingUIState *)state reads:(uint32_t)reads
    keys:(uint32_t *)keys x:(uint32_t *)x y:(uint32_t *)y pressed:(BOOL *)pressed;
- (void)updateNamingControls:(NSData *)data paused:(BOOL)paused;
- (void)setupTrainerCardControls;
- (void)layoutTrainerCardControls:(CGRect)area;
- (void)clearTrainerCardInputLocked;
- (void)queueTrainerCardKey:(OriginalKeyButton *)button;
- (void)queueTrainerCardTouch:(uint32_t)event point:(CGPoint)point source:(id)source;
- (void)takeTrainerCardInputLocked:(const HGNativeTrainerCardUIState *)state reads:(uint32_t)reads keys:(uint32_t *)keys x:(uint32_t *)x y:(uint32_t *)y pressed:(BOOL *)pressed;
- (NSDictionary *)copyTrainerCardPictures:(const HGNativeTrainerCardUIState *)state main:(NSData *)main sub:(NSData *)sub;
- (void)updateTrainerCardControls:(NSData *)data pictures:(NSDictionary *)pictures paused:(BOOL)paused;
- (void)clearStartMenuInputLocked;
- (void)queueStartMenuInputKeys:(uint32_t)keys touch:(BOOL)touch point:(CGPoint)point;
- (void)takeStartMenuInputLocked:(const HGNativeFieldMenuUIState *)state blocked:(BOOL)blocked reads:(uint32_t)reads keys:(uint32_t *)keys x:(uint32_t *)x y:(uint32_t *)y pressed:(BOOL *)pressed;
- (void)updateStartMenuControls:(NSData *)data main:(NSData *)main sub:(NSData *)sub engine:(uint32_t)engine blocked:(BOOL)blocked paused:(BOOL)paused;
- (void)setupPCBoxControls;
- (void)layoutPCBoxControls:(CGRect)area;
- (void)clearPCBoxInputLocked;
- (void)queuePCBoxKey:(OriginalKeyButton *)button;
- (void)takePCBoxInputLocked:(const HGNativePCBoxUIState *)state reads:(uint32_t)reads keys:(uint32_t *)keys;
- (NSDictionary *)copyPCBoxPictures:(const HGNativePCBoxUIState *)state main:(NSData *)main sub:(NSData *)sub;
- (void)updatePCBoxControls:(NSData *)data pictures:(NSDictionary *)pictures blocked:(BOOL)blocked paused:(BOOL)paused;
#ifdef HEARTGOLD_ORIGINAL_SUMMARY_TM_CHECK
- (BOOL)queueSummaryTMFixture:(uint32_t)revision;
- (BOOL)queueSummaryTMRaw:(const HGSummaryLearnQAState *)expected keys:(uint32_t)keys x:(uint32_t)x y:(uint32_t)y pressed:(BOOL)pressed;
#endif
- (void)stop;
- (void)tapAt:(CGPoint)point;
@end
@interface OriginalController (PokedexUI)
- (void)setupPokedexControls;
- (void)layoutPokedexControls:(CGRect)area;
- (void)clearPokedexInputLocked;
- (void)queuePokedexInput:(uint32_t)kind index:(uint32_t)index;
- (NSDictionary *)copyPokedexPictures:(const HGNativePokedexUIState *)state main:(NSData *)main sub:(NSData *)sub;
- (void)updatePokedexControls:(NSData *)data pictures:(NSDictionary *)pictures blocked:(BOOL)blocked paused:(BOOL)paused;
- (void)takePokedexInputLocked:(const HGNativePokedexUIState *)state blocked:(BOOL)blocked reads:(uint32_t)reads
    keys:(uint32_t *)keys x:(uint32_t *)x y:(uint32_t *)y pressed:(BOOL *)pressed;
- (void)pokedexControlPressed:(UIButton *)button;
- (void)pokedexGesture:(UIPanGestureRecognizer *)gesture;
- (NSString *)pokedexLabelForRole:(uint32_t)role index:(uint32_t)index state:(const HGNativePokedexUIState *)state visual:(BOOL)visual;
@end
#ifdef HEARTGOLD_ORIGINAL_PC_BOX_CHECK
@interface OriginalController (PCBoxUIKitCheck)
- (BOOL)readPCBoxQAOwner:(HGPCBoxQAState *)qa;
- (BOOL)queuePCBoxQARequest:(uint32_t)kind;
- (void)processPCBoxQARequestOwnerLocked:(const HGPCBoxQAState *)qa;
- (void)observePCBoxCheckPulseOwnerLocked:(const HGPCBoxQAState *)qa reads:(uint32_t)reads keys:(uint32_t *)keys pressed:(BOOL)pressed;
- (void)clearPCBoxCheckTransientLocked:(BOOL)interrupted;
- (void)beginPCBoxUIKitCheck;
- (void)checkPCBoxUIKitDisplay;
- (void)finishPCBoxUIKitCheck:(NSString *)status;
@end
#endif
#if defined(HEARTGOLD_ORIGINAL_APP_CHECK) && !defined(HEARTGOLD_ORIGINAL_PC_BOX_CHECK)
@interface OriginalController (Checks)
#ifdef HEARTGOLD_ORIGINAL_CAPTURE_REGISTRATION_CHECK
- (BOOL)captureRegistrationInputIdleLocked;
- (BOOL)readCaptureRegistrationQA:(HGNativeCaptureQAState *)state;
- (BOOL)queueCaptureQARequest:(uint32_t)revision variant:(uint32_t)variant;
- (void)processCaptureRegistrationRequest:(uint32_t)revision variant:(uint32_t)variant idle:(BOOL)idle;
- (void)takeCaptureRegistrationBootLocked:(const HGNativeCaptureQAState *)q keys:(uint32_t *)keys x:(uint32_t *)x y:(uint32_t *)y pressed:(BOOL *)pressed;
- (void)checkCaptureRegistrationDisplay:(uint32_t)frame;
- (void)checkCaptureRegistrationCase;
- (BOOL)captureRegistrationPrivateFilesEqual:(NSDictionary *)before;
- (BOOL)captureRegistrationSettlement:(const HGNativeCaptureQAState *)q variant:(uint32_t)variant;
#else
- (void)checkControls;
#ifdef HEARTGOLD_ORIGINAL_DEFAULT_CHECK
- (NSDictionary *)checkPCBoxHostPrimitives;
- (NSDictionary *)checkTrainerCardHostPrimitives;
- (NSDictionary *)checkStartMenuHostPrimitives;
#endif
#ifdef HEARTGOLD_ORIGINAL_POKEDEX_CHECK
- (void)checkPokedexCase;
- (void)checkPokedexDisplay:(uint32_t)frame;
- (BOOL)preparePokedexFixture:(const HGSummaryLearnQAState *)state;
- (NSDictionary *)pokedexCurrentFacts;
- (void)resetPokedexCheckOwner;
- (BOOL)queuePokedexFixture:(uint32_t)variant restoring:(BOOL)restoring revision:(uint32_t)revision;
- (BOOL)pokedexFixtureAdmission:(const HGSummaryLearnQAState *)state bag:(const HGNativeBagUIState *)bag
    dex:(const HGNativePokedexUIState *)dex map:(uint32_t)map idle:(BOOL)idle;
#endif
#ifdef HEARTGOLD_ORIGINAL_EVOLUTION_CHECK
- (void)checkEvolutionCase;
- (BOOL)prepareEvolutionFixture:(const HGSummaryLearnQAState *)state;
- (NSDictionary *)evolutionCurrentFacts;
#endif
#ifdef HEARTGOLD_ORIGINAL_EVOLUTION_LEARN_CHECK
- (BOOL)prepareEvolutionLearningFixture:(const HGSummaryLearnQAState *)state;
- (NSDictionary *)evolutionLearningCurrentFacts;
- (void)checkEvolutionLearningCase;
#endif
- (void)checkPrivatePartyPublished:(NSData *)data pictures:(NSDictionary *)pictures frame:(uint32_t)frame;
- (void)checkPrivateCaseDisplay:(uint32_t)frame;
#endif
- (BOOL)checkPrivatePixels:(UIImage *)image;
- (BOOL)checkPrivateCapture:(NSString *)name kind:(NSString *)kind image:(UIImage *)source;
@end
#endif

static void present(void *context) { @autoreleasepool { [(__bridge OriginalController *)context engineFrame]; } }
static void outputPCM(void *context,const int16_t *stereo,size_t frames) {
    @autoreleasepool { [(__bridge OriginalAudio *)context enqueue:stereo frames:frames]; }
}

@implementation OriginalController
- (BOOL)prefersStatusBarHidden { return YES; }
- (UIInterfaceOrientationMask)supportedInterfaceOrientations { return UIInterfaceOrientationMaskLandscape; }
- (UIButton *)button:(NSString *)title key:(uint32_t)key {
    UIButton *button = [[OriginalKeyButton alloc] initWithFrame:CGRectZero];
    [button setTitle:title forState:UIControlStateNormal];
    button.titleLabel.font = [UIFont boldSystemFontOfSize:18];
    button.backgroundColor = [UIColor colorWithWhite:0.13 alpha:0.94];
    button.tintColor = UIColor.whiteColor;
    button.layer.cornerRadius = 14;
    button.tag = key;
    button.accessibilityLabel = title;
    [button addTarget:self action:@selector(keyDown:) forControlEvents:UIControlEventTouchDown];
    [button addTarget:self action:@selector(keyUp:) forControlEvents:UIControlEventTouchUpInside];
    [button addTarget:self action:@selector(keyCancel:) forControlEvents:UIControlEventTouchUpOutside | UIControlEventTouchCancel];
    [self.view addSubview:button];
    _buttons[@(key)] = button;
    return button;
}
- (void)viewDidLoad {
    [super viewDidLoad];
#ifdef HEARTGOLD_ORIGINAL_SUMMARY_TM_CHECK
    NSString *summaryCase=[NSBundle.mainBundle objectForInfoDictionaryKey:@"HGPrivateCheckCase"];
    _summaryTMEnabled=([summaryCase isEqual:@"summary-tm"] || [summaryCase isEqual:@"summary-candy"]
        || [summaryCase isEqual:@"evolution"] || [summaryCase isEqual:@"evolution-cancel"])
        && [NSProcessInfo.processInfo.environment[@"HEARTGOLD_CHECK_CASE"] isEqual:summaryCase]
        && [NSProcessInfo.processInfo.environment[@"HEARTGOLD_TEST_MUTE"] isEqual:@"1"];
#ifdef HEARTGOLD_ORIGINAL_EVOLUTION_LEARN_CHECK
    if ([summaryCase isEqual:@"evolution-learn"]) _summaryTMEnabled=[NSProcessInfo.processInfo.environment[@"HEARTGOLD_CHECK_CASE"] isEqual:summaryCase] && [NSProcessInfo.processInfo.environment[@"HEARTGOLD_TEST_MUTE"] isEqual:@"1"];
#endif
#ifdef HEARTGOLD_ORIGINAL_POKEDEX_CHECK
    _summaryTMEnabled=[summaryCase isEqual:@"pokedex"] && [NSProcessInfo.processInfo.environment[@"HEARTGOLD_CHECK_CASE"] isEqual:summaryCase]
        && [NSProcessInfo.processInfo.environment[@"HEARTGOLD_TEST_MUTE"] isEqual:@"1"];
#endif
    _summaryTMRawIdle=YES;
#endif
    self.view.backgroundColor = [UIColor colorWithRed:0.035 green:0.06 blue:0.07 alpha:1];
    _condition = [NSCondition new]; _buttons = [NSMutableDictionary new]; _audio = [OriginalAudio new];
    _touchEngine=1;
    _canvas = [UIImageView new]; _canvas.contentMode = UIViewContentModeScaleAspectFit;
    _canvas.accessibilityLabel = @"原版游戏画面";
    _canvas.layer.magnificationFilter = kCAFilterNearest;
    _canvas.userInteractionEnabled = YES; [self.view addSubview:_canvas];
    _optionsPreview=[UIImageView new]; _optionsPreview.hidden=YES; _optionsPreview.userInteractionEnabled=NO;
    _optionsPreview.contentMode=UIViewContentModeScaleAspectFit; _optionsPreview.layer.magnificationFilter=kCAFilterNearest;
    _optionsPreview.isAccessibilityElement=YES; _optionsPreview.accessibilityLabel=@"原版文字速度和边框预览";
    _optionsPreview.accessibilityHint=@"只读预览，由原版菜单同步更新"; [self.view addSubview:_optionsPreview];
    _fieldChoicePreview=[UIImageView new]; _fieldChoicePreview.hidden=YES;
    _fieldChoicePreview.contentMode=UIViewContentModeScaleAspectFit; _fieldChoicePreview.layer.magnificationFilter=kCAFilterNearest;
    _fieldChoicePreview.isAccessibilityElement=YES; _fieldChoicePreview.accessibilityLabel=@"原版对话与选项说明";
    [self.view addSubview:_fieldChoicePreview];
    UIPanGestureRecognizer *touch = [[UIPanGestureRecognizer alloc] initWithTarget:self action:@selector(canvasTouch:)];
    touch.minimumNumberOfTouches = touch.maximumNumberOfTouches = 1;
    [_canvas addGestureRecognizer:touch];
    UITapGestureRecognizer *tap = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(canvasTap:)];
    [_canvas addGestureRecognizer:tap];
    [self button:@"↑" key:0x40]; [self button:@"↓" key:0x80];
    [self button:@"←" key:0x20]; [self button:@"→" key:0x10];
    [self button:@"确认" key:1]; [self button:@"返回" key:2];
    [self button:@"开始" key:8]; [self button:@"X" key:0x400];
    _joystick = [OriginalJoystick new]; _joystick.hidden = YES;
    _joystick.backgroundColor = [UIColor colorWithWhite:0 alpha:0.35];
    _joystick.layer.borderWidth = 2; _joystick.layer.borderColor = [UIColor colorWithWhite:1 alpha:0.5].CGColor;
    _joystick.isAccessibilityElement = YES; _joystick.accessibilityLabel = @"移动摇杆";
    _joystick.accessibilityHint = @"拖动移动，或使用四个方向操作。";
    _joystickThumb = [[UIView alloc] initWithFrame:CGRectMake(0,0,44,44)];
    _joystickThumb.backgroundColor = [UIColor colorWithWhite:1 alpha:0.65];
    _joystickThumb.layer.cornerRadius = 22; _joystickThumb.userInteractionEnabled = NO;
    [_joystick addSubview:_joystickThumb]; [self.view addSubview:_joystick];
    [_joystick addTarget:self action:@selector(joystickChanged:) forControlEvents:UIControlEventValueChanged];
    NSArray *directions = @[@0x40,@0x80,@0x20,@0x10], *names = @[@"向上移动",@"向下移动",@"向左移动",@"向右移动"];
    NSMutableArray *actions = [NSMutableArray new]; __weak OriginalController *weakSelf = self;
    for (NSUInteger i=0;i<directions.count;i++) {
        NSNumber *key = directions[i];
        [actions addObject:[[UIAccessibilityCustomAction alloc] initWithName:names[i] actionHandler:^BOOL(UIAccessibilityCustomAction *action) {
            OriginalController *owner = weakSelf;
            return owner && [owner->_buttons[key] accessibilityActivate];
        }]];
    }
    _joystick.accessibilityCustomActions = actions;
    _fieldMenuShade=[UIView new]; _fieldMenuShade.backgroundColor=[UIColor colorWithWhite:0 alpha:0.45];
    _fieldMenuShade.hidden=YES; [self.view addSubview:_fieldMenuShade];
    [_fieldMenuShade addGestureRecognizer:[[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(toggleMenu)]];
    _fieldMenu=[UIImageView new]; _fieldMenu.userInteractionEnabled=YES; _fieldMenu.hidden=YES;
    _fieldMenu.layer.magnificationFilter=kCAFilterNearest; _fieldMenu.layer.cornerRadius=12; _fieldMenu.clipsToBounds=YES;
    _fieldMenu.layer.borderWidth=2; _fieldMenu.layer.borderColor=[UIColor colorWithWhite:1 alpha:0.7].CGColor;
    [self.view addSubview:_fieldMenu];
    UIPanGestureRecognizer *menuTouch=[[UIPanGestureRecognizer alloc] initWithTarget:self action:@selector(canvasTouch:)];
    menuTouch.minimumNumberOfTouches=menuTouch.maximumNumberOfTouches=1; [_fieldMenu addGestureRecognizer:menuTouch];
    [_fieldMenu addGestureRecognizer:[[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(canvasTap:)]];
    NSDictionary *menuInfo=[NSDictionary dictionaryWithContentsOfFile:[NSBundle.mainBundle.resourcePath stringByAppendingPathComponent:@"field-menu.plist"]];
    _fieldMenuHitboxes=menuInfo[@"hitboxes"]; _fieldMenuNames=menuInfo[@"names"]; _fieldMenuIcons=0x0DDDDDDD;
    NSMutableArray *menuElements=[NSMutableArray new];
    for (NSUInteger i=0;i<_fieldMenuHitboxes.count;i++) {
        OriginalMenuElement *element=[[OriginalMenuElement alloc] initWithAccessibilityContainer:_fieldMenu];
        element.accessibilityTraits=UIAccessibilityTraitButton;
        element.activate=^BOOL {
            OriginalController *owner=weakSelf;
            if (!owner || owner->_fieldMenu.hidden || !owner->_fieldMenu.userInteractionEnabled) return NO;
            CGRect rect=owner->_fieldMenuElements[i].accessibilityFrameInContainerSpace;
            [owner tapAt:CGPointMake(CGRectGetMidX(rect),CGRectGetMidY(rect))]; return YES;
        };
        [menuElements addObject:element];
    }
    _fieldMenuElements=menuElements;
    _menuButton = [UIButton buttonWithType:UIButtonTypeSystem];
    [_menuButton setTitle:@"触摸菜单" forState:UIControlStateNormal];
    _menuButton.tintColor = UIColor.whiteColor;
    _menuButton.backgroundColor = [UIColor colorWithWhite:0.13 alpha:0.94];
    _menuButton.layer.cornerRadius = 12;
    [_menuButton addTarget:self action:@selector(toggleMenu) forControlEvents:UIControlEventTouchUpInside];
    [self.view addSubview:_menuButton];
    _battleBar = [UIView new];
    _battleBar.backgroundColor = [UIColor colorWithRed:0.09 green:0.12 blue:0.14 alpha:1];
    _battleBar.hidden = YES; [self.view addSubview:_battleBar];
    NSMutableArray *battleButtons = [NSMutableArray new];
    for (NSUInteger i = 0; i < 6; i++) {
        UIButton *button = [UIButton buttonWithType:UIButtonTypeSystem];
        button.tag = i; button.layer.cornerRadius = i == 5 ? 36 : 8; button.layer.borderWidth = 2;
        button.layer.borderColor = [UIColor colorWithWhite:0.8 alpha:1].CGColor;
        button.imageView.layer.magnificationFilter = kCAFilterNearest;
        [button addTarget:self action:@selector(battlePressed:) forControlEvents:UIControlEventTouchUpInside];
        [_battleBar addSubview:button];
        if (i < 4) [battleButtons addObject:button]; else if (i == 4) _battleBack = button; else _battleBall = button;
    }
    _battleButtons = battleButtons;
    _moveNames = [NSArray arrayWithContentsOfFile:[NSBundle.mainBundle.resourcePath stringByAppendingPathComponent:@"battle-move-names.plist"]];
    _speciesNames = [NSArray arrayWithContentsOfFile:[NSBundle.mainBundle.resourcePath stringByAppendingPathComponent:@"battle-species-names.plist"]];
    _partyActionNames = [NSArray arrayWithContentsOfFile:[NSBundle.mainBundle.resourcePath stringByAppendingPathComponent:@"party-action-names.plist"]];
    _partyPanel=[UIView new]; _partyPanel.hidden=YES;
    _partyPanel.backgroundColor=[UIColor colorWithRed:0.84 green:0.91 blue:0.95 alpha:1];
    _partyPanel.layer.cornerRadius=12; [self.view addSubview:_partyPanel];
    _partyTitle=[UILabel new]; _partyTitle.text=@"宝可梦"; _partyTitle.font=[UIFont boldSystemFontOfSize:20];
    _partyTitle.textColor=[UIColor colorWithRed:0.12 green:0.24 blue:0.34 alpha:1]; [_partyPanel addSubview:_partyTitle];
    NSMutableArray *partyMons=[NSMutableArray new],*partyCards=[NSMutableArray new];
    for (NSUInteger i=0;i<HG_PARTY_UI_SLOTS;i++) {
        UIButton *button=[UIButton buttonWithType:UIButtonTypeCustom]; button.tag=i;
        button.backgroundColor=UIColor.whiteColor; button.layer.cornerRadius=8; button.layer.borderWidth=2;
        [button addTarget:self action:@selector(partyMonPressed:) forControlEvents:UIControlEventTouchUpInside];
        UIImageView *image=[UIImageView new]; image.contentMode=UIViewContentModeScaleAspectFit; image.layer.magnificationFilter=kCAFilterNearest;
        [button addSubview:image]; [_partyPanel addSubview:button]; [partyMons addObject:button]; [partyCards addObject:image];
    }
    _partyMons=partyMons; _partyCards=partyCards;
    _partySelectedName=[UIImageView new]; _partySelectedItem=[UIImageView new]; _partyMessage=[UIImageView new];
    for (UIImageView *image in @[_partySelectedName,_partySelectedItem,_partyMessage]) {
        image.contentMode=UIViewContentModeScaleAspectFit; image.layer.magnificationFilter=kCAFilterNearest; [_partyPanel addSubview:image];
        image.clipsToBounds=YES;
        if (image!=_partyMessage) { image.backgroundColor=_partyTitle.textColor; image.layer.cornerRadius=6; }
    }
    _partyBody=[UIImageView new]; _partyBody.hidden=YES; _partyBody.clipsToBounds=YES;
    _partyBody.contentMode=UIViewContentModeScaleAspectFit; _partyBody.layer.magnificationFilter=kCAFilterNearest;
    _partyBody.isAccessibilityElement=YES; _partyBody.accessibilityTraits=UIAccessibilityTraitImage;
    [_partyPanel addSubview:_partyBody];
    _partyActionScroll=[UIScrollView new]; [_partyPanel addSubview:_partyActionScroll];
    NSMutableArray *partyActions=[NSMutableArray new];
    for (NSUInteger i=0;i<HG_PARTY_UI_ACTIONS;i++) {
        UIButton *button=[UIButton buttonWithType:UIButtonTypeSystem]; button.tag=i; button.layer.cornerRadius=8;
        button.imageView.layer.magnificationFilter=kCAFilterNearest;
        [button addTarget:self action:@selector(partyActionPressed:) forControlEvents:UIControlEventTouchUpInside];
        [_partyActionScroll addSubview:button]; [partyActions addObject:button];
    }
    _partyActions=partyActions;
    _partyBack=[UIButton buttonWithType:UIButtonTypeSystem]; [_partyBack setTitle:@"返回" forState:UIControlStateNormal];
    _partyBack.backgroundColor=[UIColor colorWithWhite:0.13 alpha:0.94]; _partyBack.tintColor=UIColor.whiteColor;
    _partyBack.layer.cornerRadius=12; _partyBack.hidden=YES;
    [_partyBack addTarget:self action:@selector(partyBackPressed:) forControlEvents:UIControlEventTouchUpInside]; [self.view addSubview:_partyBack];
    _bagActionNames=[NSArray arrayWithContentsOfFile:[NSBundle.mainBundle.resourcePath stringByAppendingPathComponent:@"bag-action-names.plist"]];
    _itemNames=[NSArray arrayWithContentsOfFile:[NSBundle.mainBundle.resourcePath stringByAppendingPathComponent:@"bag-item-names.plist"]];
    _pocketNames=[NSArray arrayWithContentsOfFile:[NSBundle.mainBundle.resourcePath stringByAppendingPathComponent:@"bag-pocket-names.plist"]];
    _bagPanel=[UIView new]; _bagPanel.hidden=YES; _bagPanel.backgroundColor=_partyPanel.backgroundColor;
    _bagPanel.layer.cornerRadius=12; [self.view addSubview:_bagPanel];
    _bagPreview=[UIImageView new]; _bagSelectedEntry=[UIImageView new]; _bagPrompt=[UIImageView new]; _bagPage=[UIImageView new];
    for (UIImageView *image in @[_bagPreview,_bagSelectedEntry,_bagPrompt]) {
        image.contentMode=UIViewContentModeScaleAspectFit; image.clipsToBounds=YES; image.layer.magnificationFilter=kCAFilterNearest; [_bagPanel addSubview:image];
    }
    _bagPage.contentMode=UIViewContentModeScaleAspectFit; _bagPage.layer.magnificationFilter=kCAFilterNearest; _bagPage.clipsToBounds=YES;
    _bagPage.hidden=YES; [self.view addSubview:_bagPage];
    for (NSUInteger group=0;group<3;group++) {
        NSMutableArray *buttons=[NSMutableArray new],*images=[NSMutableArray new];
        NSUInteger count=group==0 ? HG_BAG_UI_POCKETS : group==1 ? HG_BAG_UI_ITEMS : HG_BAG_UI_ACTIONS;
        for (NSUInteger i=0;i<count;i++) {
            UIButton *button=[UIButton buttonWithType:group==2 ? UIButtonTypeSystem : UIButtonTypeCustom];
            button.tag=(group<<8)|i; button.layer.cornerRadius=8; button.layer.borderWidth=2;
            button.backgroundColor=UIColor.whiteColor; button.imageView.layer.magnificationFilter=kCAFilterNearest;
            [button addTarget:self action:@selector(bagPressed:) forControlEvents:UIControlEventTouchUpInside]; [_bagPanel addSubview:button];
            if (group<2) {
                UIImageView *image=[UIImageView new]; image.contentMode=UIViewContentModeScaleAspectFit; image.layer.magnificationFilter=kCAFilterNearest;
                [button addSubview:image]; [images addObject:image];
            }
            [buttons addObject:button];
        }
        if (!group) { _bagPockets=buttons; _bagPocketImages=images; }
        else if (group==1) { _bagItems=buttons; _bagItemImages=images; }
        else _bagActions=buttons;
    }
    NSMutableArray *bagNavigation=[NSMutableArray new];
    for (NSString *name in @[@"返回",@"上一页",@"下一页"]) {
        UIButton *button=[UIButton buttonWithType:UIButtonTypeSystem]; [button setTitle:name forState:UIControlStateNormal]; button.tag=bagNavigation.count;
        button.backgroundColor=_partyBack.backgroundColor; button.tintColor=UIColor.whiteColor; button.layer.cornerRadius=12; button.hidden=YES;
        [button addTarget:self action:@selector(bagNavigate:) forControlEvents:UIControlEventTouchUpInside]; [self.view addSubview:button]; [bagNavigation addObject:button];
    }
    _bagNavigation=bagNavigation;
    _summaryPanel=[UIView new]; _summaryPanel.hidden=YES; _summaryPanel.backgroundColor=_partyPanel.backgroundColor;
    _summaryPanel.layer.cornerRadius=12; [self.view addSubview:_summaryPanel];
    _summaryPreview=[UIImageView new]; _summaryContent=[UIImageView new]; _summaryTitle=[UIImageView new];
    for (UIImageView *image in @[_summaryPreview,_summaryContent,_summaryTitle]) {
        image.contentMode=UIViewContentModeScaleAspectFit; image.clipsToBounds=YES; image.layer.magnificationFilter=kCAFilterNearest;
        [_summaryPanel addSubview:image];
    }
    _summaryTitle.backgroundColor=_partyTitle.textColor; _summaryTitle.layer.cornerRadius=6;
    NSMutableArray *summaryMoves=[NSMutableArray new],*summaryMoveImages=[NSMutableArray new];
    for (NSUInteger i=0;i<4;i++) {
        UIButton *button=[UIButton buttonWithType:UIButtonTypeCustom]; button.tag=i;
        button.backgroundColor=UIColor.whiteColor; button.layer.cornerRadius=8; button.layer.borderWidth=2;
        [button addTarget:self action:@selector(summaryMovePressed:) forControlEvents:UIControlEventTouchUpInside];
        UIImageView *image=[UIImageView new]; image.contentMode=UIViewContentModeScaleAspectFit;
        image.clipsToBounds=YES; image.layer.magnificationFilter=kCAFilterNearest;
        [button addSubview:image]; [_summaryPanel addSubview:button]; [summaryMoves addObject:button]; [summaryMoveImages addObject:image];
    }
    _summaryMoves=summaryMoves; _summaryMoveImages=summaryMoveImages;
    NSMutableArray *summaryNavigation=[NSMutableArray new];
    for (NSString *name in @[@"返回",@"上一页",@"下一页",@"上一只",@"下一只",@"查看缎带"]) {
        UIButton *button=[UIButton buttonWithType:UIButtonTypeSystem]; button.tag=summaryNavigation.count;
        [button setTitle:name forState:UIControlStateNormal]; button.accessibilityLabel=name;
        button.backgroundColor=_partyBack.backgroundColor; button.tintColor=UIColor.whiteColor; button.layer.cornerRadius=12; button.hidden=YES;
        [button addTarget:self action:@selector(summaryNavigate:) forControlEvents:UIControlEventTouchUpInside];
        [self.view addSubview:button]; [summaryNavigation addObject:button];
    }
    _summaryNavigation=summaryNavigation;
    _battleMenuPanel=[UIView new]; _battleMenuPanel.hidden=YES;
    _battleMenuPanel.backgroundColor=_partyPanel.backgroundColor; _battleMenuPanel.layer.cornerRadius=12; [self.view addSubview:_battleMenuPanel];
    _battleMenuTitle=[UILabel new]; _battleMenuTitle.font=_partyTitle.font; _battleMenuTitle.textColor=_partyTitle.textColor; [_battleMenuPanel addSubview:_battleMenuTitle];
    _battleMenuPreview=[UIImageView new]; _battleMenuSelected=[UIImageView new]; _battleMenuQuantity=[UIImageView new];
    _battleMenuDescription=[UIImageView new]; _battleMenuMessage=[UIImageView new];
    for (UIImageView *image in @[_battleMenuPreview,_battleMenuSelected,_battleMenuQuantity,_battleMenuDescription,_battleMenuMessage]) {
        image.contentMode=UIViewContentModeScaleAspectFit; image.clipsToBounds=YES; image.layer.magnificationFilter=kCAFilterNearest;
        [_battleMenuPanel addSubview:image];
    }
    _battleMenuPreview.contentMode=UIViewContentModeScaleAspectFill;
    _battleMenuMessage.isAccessibilityElement=YES; _battleMenuMessage.accessibilityLabel=@"原版战斗提示";
    NSMutableArray *battleMenuButtons=[NSMutableArray new],*battleMenuImages=[NSMutableArray new];
    for (NSUInteger i=0;i<HG_BATTLE_BAG_UI_BUTTONS;i++) {
        UIButton *button=[UIButton buttonWithType:UIButtonTypeCustom]; button.tag=i; button.hidden=YES;
        button.backgroundColor=UIColor.whiteColor; button.layer.cornerRadius=8; button.layer.borderWidth=2; button.clipsToBounds=YES;
        button.titleLabel.font=[UIFont systemFontOfSize:16 weight:UIFontWeightSemibold]; button.titleLabel.adjustsFontSizeToFitWidth=YES;
        [button setTitleColor:UIColor.blackColor forState:UIControlStateNormal];
        [button addTarget:self action:@selector(battleMenuPressed:) forControlEvents:UIControlEventTouchUpInside];
        UIImageView *image=[UIImageView new]; image.contentMode=UIViewContentModeScaleAspectFit;
        image.clipsToBounds=YES; image.layer.magnificationFilter=kCAFilterNearest; [button addSubview:image];
        [_battleMenuPanel addSubview:button]; [battleMenuButtons addObject:button]; [battleMenuImages addObject:image];
    }
    _battleMenuButtons=battleMenuButtons; _battleMenuImages=battleMenuImages;
    [self setupPokedexControls];
    [self setupTrainerCardControls];
    [self setupPCBoxControls];
    [self.view bringSubviewToFront:_buttons[@1]]; [self.view bringSubviewToFront:_buttons[@2]];
    _status = [UILabel new]; _status.text = @"正在启动原版引擎…";
    _status.textColor = UIColor.whiteColor; _status.textAlignment = NSTextAlignmentCenter;
    _status.numberOfLines = 0; [self.view addSubview:_status];
    _displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(displayFrame)];
    _displayLink.preferredFramesPerSecond = 30;
    [_displayLink addToRunLoop:NSRunLoop.mainRunLoop forMode:NSRunLoopCommonModes];
    NSNotificationCenter *notifications = NSNotificationCenter.defaultCenter;
    [notifications addObserver:self selector:@selector(pause) name:UIApplicationWillResignActiveNotification object:nil];
    [notifications addObserver:self selector:@selector(resume) name:UIApplicationDidBecomeActiveNotification object:nil];
    [notifications addObserver:self selector:@selector(audioInterruption:) name:AVAudioSessionInterruptionNotification object:nil];
    [notifications addObserver:self selector:@selector(audioRouteChanged:) name:AVAudioSessionRouteChangeNotification object:nil];
#ifdef HEARTGOLD_ORIGINAL_APP_CHECK
#if defined(HEARTGOLD_ORIGINAL_PC_BOX_CHECK)
    [self beginPCBoxUIKitCheck];
#elif defined(HEARTGOLD_ORIGINAL_CAPTURE_REGISTRATION_CHECK)
    [self checkCaptureRegistrationCase];
#else
#ifdef HEARTGOLD_ORIGINAL_POKEDEX_CHECK
    [self checkPokedexCase];
#else
#ifdef HEARTGOLD_ORIGINAL_EVOLUTION_LEARN_CHECK
    [self checkEvolutionLearningCase];
#else
#ifdef HEARTGOLD_ORIGINAL_EVOLUTION_CHECK
    [self checkEvolutionCase];
#else
    [self checkControls];
#endif
#endif
#endif
#endif
#endif
    NSThread *engineThread=[[NSThread alloc] initWithTarget:self selector:@selector(runEngine) object:nil];
    engineThread.qualityOfService=NSQualityOfServiceUserInteractive;
    [engineThread start];
}
- (void)viewDidLayoutSubviews {
    [super viewDidLayoutSubviews];
    CGRect area = UIEdgeInsetsInsetRect(self.view.bounds, self.view.safeAreaInsets);
    CGFloat side = MIN(150, area.size.width * 0.19), h = area.size.height;
    uint32_t preferred=h>0 ? ((uint32_t)MIN(512,MAX(256,area.size.width*192/h))&~1u) : 256;
    CGFloat barHeight=MIN(136,MAX(122,h*0.34)),sceneHeight=MAX(1,h-barHeight-8);
    uint32_t battleWidth=(uint32_t)MIN(1024,MAX(256,area.size.width*192/sceneHeight))&~1u;
    [_condition lock]; _preferredViewWidth=preferred; _preferredBattleWidth=battleWidth; [_condition unlock];
    CGFloat width = MIN(area.size.width - 2 * side, h * 4 / 3);
    _canvas.frame = CGRectMake(CGRectGetMidX(area) - width / 2, area.origin.y + (h - width * 0.75) / 2, width, width * 0.75);
    if (_displayedWidth>256 && !_battleMode && !_battleScene) _canvas.frame=area;
    CGFloat size = MIN(56, (side - 12) / 2), left = area.origin.x + 6, bottom = CGRectGetMaxY(area) - size - 12;
    _buttons[@0x40].frame = CGRectMake(left + size / 2, bottom - size * 2 - 8, size, size);
    _buttons[@0x80].frame = CGRectMake(left + size / 2, bottom, size, size);
    _buttons[@0x20].frame = CGRectMake(left, bottom - size - 4, size, size);
    _buttons[@0x10].frame = CGRectMake(left + size + 4, bottom - size - 4, size, size);
    CGFloat stickSize=MIN(140,MAX(96,h*0.38));
    BOOL resized=!CGSizeEqualToSize(_joystick.bounds.size,CGSizeMake(stickSize,stickSize));
    _joystick.frame=CGRectMake(area.origin.x+12,CGRectGetMaxY(area)-stickSize-12,stickSize,stickSize);
    _joystick.layer.cornerRadius=stickSize/2;
    if (resized) [self resetJoystick];
    CGFloat menuWidth=MIN(420,MIN(area.size.width*0.55,MAX(1,h-72)*4/3));
    _fieldMenuShade.frame=area;
    _fieldMenu.frame=CGRectMake(CGRectGetMaxX(area)-(_startMenuScene ? side : 0)-menuWidth-12,area.origin.y+56,menuWidth,menuWidth*0.75);
    for (NSUInteger i=0;i<_fieldMenuElements.count;i++) {
        NSArray *rect=_fieldMenuHitboxes[i]; CGFloat scale=menuWidth/256;
        CGRect frame=CGRectMake([rect[0] doubleValue]*scale,[rect[1] doubleValue]*scale,[rect[2] doubleValue]*scale,[rect[3] doubleValue]*scale);
        _fieldMenuElements[i].accessibilityFrameInContainerSpace=CGRectInset(frame,-MAX(0,44-frame.size.width)/2,-MAX(0,44-frame.size.height)/2);
    }
    CGFloat right = CGRectGetMaxX(area) - side + 6;
    _buttons[@1].frame = CGRectMake(right, bottom - size - 8, side - 12, size);
    _buttons[@2].frame = CGRectMake(right, bottom, side - 12, size);
    _buttons[@8].frame = CGRectMake(left, area.origin.y + 8, side - 12, 44);
    _buttons[@0x400].frame = CGRectMake(right, bottom - 2 * size - 16, side - 12, 44);
    _menuButton.frame = CGRectMake(right, area.origin.y + 8, side - 12, 44);
    _status.frame = CGRectInset(_canvas.frame, 10, 10);
    if (_battleScene) {
        _battleBar.frame = CGRectMake(area.origin.x, CGRectGetMaxY(area) - barHeight, area.size.width, barHeight);
        BOOL selection=_battleMode==HG_BATTLE_UI_MOVES || _battleMode==HG_BATTLE_UI_TARGETS;
        CGFloat inset = selection ? 96 : 12;
        BOOL choice=OriginalBattleChoiceMode(_battleMode);
        CGFloat ballSpace = selection || choice ? 0 : 96;
        CGFloat gap = 10, buttonWidth = (_battleBar.bounds.size.width - inset - 12 - gap - ballSpace) / 2;
        CGFloat buttonHeight = (barHeight - 24 - gap) / 2;
        for (NSUInteger i = 0; i < 4; i++) {
            _battleButtons[i].frame = CGRectMake(inset + (i % 2) * (buttonWidth + gap), choice ? 12 : 12 + (i / 2) * (buttonHeight + gap), buttonWidth, choice ? barHeight - 24 : buttonHeight);
        }
        _battleBack.frame = CGRectMake(12, 12, 72, barHeight - 24);
        _battleBall.frame = CGRectMake(_battleBar.bounds.size.width - 84, (barHeight - 72) / 2, 72, 72);
        _canvas.frame = CGRectMake(area.origin.x, area.origin.y, area.size.width, sceneHeight);
        _status.frame = CGRectInset(_canvas.frame, 10, 10);
    }
    if (_partyScene && !_partyPanel.hidden) {
        _partyPanel.frame=CGRectMake(area.origin.x,area.origin.y,area.size.width,MAX(1,h-60));
        CGFloat pw=_partyPanel.bounds.size.width,ph=_partyPanel.bounds.size.height,leftWidth=pw*0.55,gap=8;
        _partyTitle.frame=CGRectMake(12,4,leftWidth-24,32);
        CGFloat cardW=(leftWidth-3*gap)/2,cardH=(ph-44-2*gap)/3;
        for (NSUInteger i=0;i<HG_PARTY_UI_SLOTS;i++) {
            _partyMons[i].frame=CGRectMake(gap+(i%2)*(cardW+gap),40+(i/2)*(cardH+gap),cardW,cardH);
            _partyCards[i].frame=CGRectInset(_partyMons[i].bounds,3,3);
        }
        CGFloat rx=leftWidth+8,rw=pw-rx-12;
        _partySelectedName.frame=CGRectMake(rx,12,rw,28); _partySelectedItem.frame=CGRectMake(rx,44,rw,24);
        _partyMessage.frame=CGRectMake(rx,ph-56,rw,48);
        const HGNativePartyUIState *state=_displayedPartyUIState.bytes;
        BOOL candy=_displayedPartyUIState.length==sizeof(*state) && HGNativePartyUIIsRareCandy(state);
        BOOL stats=candy && state->actionCount && (state->actions[0].textId==HG_PARTY_UI_TEXT_CANDY_STATS_DIFF || state->actions[0].textId==HG_PARTY_UI_TEXT_CANDY_STATS_TOTAL);
        _partyBody.frame=CGRectMake(rx,12,rw,MAX(1,MIN(rw*112.0/128.0,ph-72)));
        _partyActionScroll.frame=stats ? CGRectMake(rx,CGRectGetMaxY(_partyBody.frame)+8,rw,44) : CGRectMake(rx,72,rw,MAX(44,ph-116));
        NSUInteger count=_displayedPartyUIState.length==sizeof(*state) ? MIN(HG_PARTY_UI_ACTIONS,state->actionCount) : 0;
        for (NSUInteger i=0;i<HG_PARTY_UI_ACTIONS;i++) _partyActions[i].frame=stats ? CGRectMake(0,0,rw,44) : CGRectMake((i%2)*(rw+gap)/2,(i/2)*52,(rw-gap)/2,44);
        _partyActionScroll.contentSize=CGSizeMake(rw,stats ? 44 : ((count+1)/2)*52);
        _partyBack.frame=CGRectMake(area.origin.x,CGRectGetMaxY(area)-52,120,44);
        _buttons[@2].frame=_partyBack.frame;
        _buttons[@1].frame=CGRectMake(CGRectGetMaxX(area)-120,CGRectGetMaxY(area)-52,120,44);
        _canvas.frame=CGRectMake(area.origin.x,area.origin.y,area.size.width,MAX(1,h-60));
        _status.frame=CGRectInset(_canvas.frame,10,10);
    }
    if (_bagScene && !_bagPanel.hidden) {
        _bagPanel.frame=CGRectMake(area.origin.x,area.origin.y,area.size.width,MAX(1,h-60));
        CGFloat pw=_bagPanel.bounds.size.width,ph=_bagPanel.bounds.size.height,gap=8,leftWidth=MIN(280,pw*0.38),rx=leftWidth+gap,rw=pw-rx-gap;
        const HGNativeBagUIState *state=_displayedBagUIState.bytes;
        BOOL tm=_displayedBagUIState.length==sizeof(*state) && HGNativeBagUIIsTMHM(state);
        CGFloat pocketWidth=(pw-9*gap)/8;
        for (NSUInteger i=0;i<HG_BAG_UI_POCKETS;i++) {
            _bagPockets[i].frame=CGRectMake(gap+i*(pocketWidth+gap),4,pocketWidth,44); _bagPocketImages[i].frame=CGRectInset(_bagPockets[i].bounds,3,3);
        }
        _bagPreview.frame=CGRectMake(8,56,leftWidth-16,ph-64);
        _bagSelectedEntry.frame=CGRectMake(rx,54,rw,40);
        CGFloat cardWidth=(rw-gap)/2,cardHeight=(ph-106)/3;
        for (NSUInteger i=0;i<HG_BAG_UI_ITEMS;i++) {
            _bagItems[i].frame=CGRectMake(rx+(i%2)*(cardWidth+gap),56+(i/2)*(cardHeight+gap),cardWidth,cardHeight);
            _bagItemImages[i].frame=CGRectInset(_bagItems[i].bounds,3,3);
        }
        CGFloat actionHeight=MAX(44,MIN(64,(ph-140)/2));
        for (NSUInteger i=0;i<HG_BAG_UI_ACTIONS;i++) _bagActions[i].frame=CGRectMake(rx+(i%2)*(cardWidth+gap),104+(i/2)*(actionHeight+gap),cardWidth,actionHeight);
        _bagPrompt.frame=CGRectMake(rx,ph-30,rw,26);
        if (tm) {
            _bagSelectedEntry.frame=CGRectMake(rx,4,rw,40);
            _bagPrompt.frame=CGRectMake(rx,52,rw,MAX(32,MIN(64,ph-112)));
            for (NSUInteger i=0;i<HG_BAG_UI_ACTIONS;i++) _bagActions[i].frame=CGRectMake(rx+(i%2)*(cardWidth+gap),CGRectGetMaxY(_bagPrompt.frame)+8+(i/2)*52,cardWidth,44);
        }
        CGFloat navY=CGRectGetMaxY(area)-52;
        _bagNavigation[0].frame=CGRectMake(area.origin.x,navY,100,44);
        _bagNavigation[1].frame=CGRectMake(CGRectGetMidX(area)-122,navY,84,44);
        _bagPage.frame=CGRectMake(CGRectGetMidX(area)-34,navY,68,44);
        _bagNavigation[2].frame=CGRectMake(CGRectGetMidX(area)+38,navY,84,44);
        _buttons[@1].frame=CGRectMake(CGRectGetMaxX(area)-100,navY,100,44);
        _status.frame=CGRectInset(_bagPanel.frame,10,10);
    }
    if (_summaryScene && !_summaryPanel.hidden) {
        _summaryPanel.frame=CGRectMake(area.origin.x,area.origin.y,area.size.width,MAX(1,h-60));
        CGFloat pw=_summaryPanel.bounds.size.width,ph=_summaryPanel.bounds.size.height,gap=8,leftWidth=pw*0.48,rx=leftWidth+gap,rw=pw-rx-gap;
        _summaryTitle.frame=CGRectMake(12,4,pw-24,24);
        _summaryPreview.frame=CGRectMake(8,32,leftWidth-16,ph-40);
        _summaryContent.frame=CGRectMake(rx,32,rw,ph-40);
        CGFloat cardsWidth=_summaryMoveDetail ? (rw-gap)*136/256 : rw;
        if (_summaryMoveDetail) _summaryContent.frame=CGRectMake(rx+cardsWidth+gap,32,rw-cardsWidth-gap,ph-40);
        CGFloat cardGap=6,cardHeight=(ph-40-3*cardGap)/4;
        for (NSUInteger i=0;i<4;i++) {
            _summaryMoves[i].frame=CGRectMake(rx,32+i*(cardHeight+cardGap),cardsWidth,cardHeight);
            _summaryMoveImages[i].frame=CGRectInset(_summaryMoves[i].bounds,3,3);
        }
        CGFloat navWidth=(area.size.width-5*gap)/6,navY=CGRectGetMaxY(area)-52;
        for (NSUInteger i=0;i<_summaryNavigation.count;i++) _summaryNavigation[i].frame=CGRectMake(area.origin.x+i*(navWidth+gap),navY,navWidth,44);
        const HGNativeSummaryUIState *state=_displayedSummaryUIState.bytes;
        if (HGNativeSummaryUIIsLearning(state)) {
            navWidth=MIN(128,(area.size.width-2*gap)/3);
            CGFloat confirmWidth=MIN(220,area.size.width-2*navWidth-2*gap);
            _summaryNavigation[0].frame=CGRectMake(area.origin.x,navY,navWidth,44);
            _summaryNavigation[1].frame=CGRectMake(CGRectGetMidX(area)-confirmWidth/2,navY,confirmWidth,44);
            _summaryNavigation[5].frame=CGRectMake(CGRectGetMaxX(area)-navWidth,navY,navWidth,44);
        }
        _status.frame=CGRectInset(_summaryPanel.frame,10,10);
    }
    if (_battleMenuKind) {
        _battleMenuPanel.frame=area;
        CGFloat pw=area.size.width,ph=area.size.height,gap=8,lw=MIN(280,pw*0.36),rx=lw+gap,rw=pw-rx-gap;
        CGFloat previewHeight=MIN((lw-16)*0.75,ph*0.5);
        _battleMenuTitle.frame=CGRectMake(12,4,lw-24,28);
        _battleMenuPreview.frame=CGRectMake(8,38,lw-16,previewHeight);
        _battleMenuSelected.frame=CGRectMake(8,46+previewHeight,lw-16,24);
        _battleMenuQuantity.frame=CGRectMake(8,72+previewHeight,lw-16,20);
        _battleMenuDescription.frame=CGRectMake(8,96+previewHeight,lw-16,MAX(40,ph-previewHeight-104));
        _battleMenuMessage.frame=CGRectMake(rx,40,rw,MAX(44,ph-52));
        const HGNativeBattleBagUIState *bag=_displayedBattleBagUIState.bytes;
        const HGNativeBattlePartyUIState *party=_displayedBattlePartyUIState.bytes;
        uint32_t phase=_battleMenuKind==OriginalBattleMenuBag ? bag->phase : party->phase;
        BOOL list=(_battleMenuKind==OriginalBattleMenuBag && phase==HG_BATTLE_BAG_UI_ITEMS)
            || (_battleMenuKind==OriginalBattleMenuParty && phase==HG_BATTLE_PARTY_UI_ROSTER);
        CGFloat cardWidth=(rw-gap)/2,cardHeight=(ph-(list ? 104 : 48)-2*gap)/3;
        for (NSUInteger i=0;i<_battleMenuButtons.count;i++) _battleMenuButtons[i].frame=CGRectMake(rx+(i%2)*(cardWidth+gap),40+(i/2)*(cardHeight+gap),cardWidth,cardHeight);
        if (list) {
            NSUInteger count=_battleMenuKind==OriginalBattleMenuBag ? 3 : 1;
            CGFloat navWidth=count==3 ? (rw-2*gap)/3 : MIN(160,rw);
            for (NSUInteger i=0;i<count;i++) _battleMenuButtons[6+i].frame=CGRectMake(rx+i*(navWidth+gap),ph-52,navWidth,44);
        } else if (_battleMenuKind==OriginalBattleMenuBag && phase==HG_BATTLE_BAG_UI_USE) {
            CGFloat ch=MAX(44,MIN(112,ph*0.35));
            for (NSUInteger i=0;i<2;i++) _battleMenuButtons[i].frame=CGRectMake(rx+i*(cardWidth+gap),(ph-ch)/2,cardWidth,ch);
        } else if (_battleMenuKind==OriginalBattleMenuParty && phase==HG_BATTLE_PARTY_UI_MON_MENU) {
            CGFloat ch=(ph-56-gap)/2;
            for (NSUInteger i=0;i<4;i++) _battleMenuButtons[i].frame=CGRectMake(rx+(i%2)*(cardWidth+gap),40+(i/2)*(ch+gap),cardWidth,ch);
        } else if (_battleMenuKind==OriginalBattleMenuParty && phase==HG_BATTLE_PARTY_UI_LEARN_LIST) {
            CGFloat cw=(pw-3*gap)/2,ch=MAX(44,(ph-76-2*gap)/3);
            for (NSUInteger i=0;i<5;i++) _battleMenuButtons[i].frame=CGRectMake(gap+(i%2)*(cw+gap),8+(i/2)*(ch+gap),cw,ch);
            _battleMenuButtons[6].frame=CGRectMake(gap,ph-52,MIN(160,pw-2*gap),44);
        } else if (_battleMenuKind==OriginalBattleMenuParty && phase==HG_BATTLE_PARTY_UI_LEARN_DETAIL) {
            CGFloat cw=(pw-3*gap)/2;
            _battleMenuButtons[0].frame=CGRectMake(gap,ph-52,cw,44);
            _battleMenuButtons[2].frame=CGRectMake(2*gap+cw,ph-52,cw,44);
            _battleMenuDescription.frame=CGRectMake(gap,8,pw-2*gap,MAX(44,ph-68));
        } else if (_battleMenuKind==OriginalBattleMenuParty && OriginalBattlePartyInfoPhase(phase)) {
            BOOL moves=phase==HG_BATTLE_PARTY_UI_CHECK_MOVES;
            NSUInteger first=moves ? 4 : 0,count=phase==HG_BATTLE_PARTY_UI_MOVE_DETAIL ? 5 : 4;
            CGFloat navWidth=(pw-(count+1)*gap)/count;
            for (NSUInteger i=0;i<count;i++) _battleMenuButtons[first+i].frame=CGRectMake(gap+i*(navWidth+gap),ph-52,navWidth,44);
            _battleMenuDescription.frame=CGRectMake(gap,8,pw-2*gap,MAX(44,ph-68));
            _battleMenuSelected.frame=CGRectMake(gap,8,pw-2*gap,24);
            if (moves) {
                CGFloat cw=(pw-3*gap)/2,ch=(ph-108)/2;
                for (NSUInteger i=0;i<4;i++) _battleMenuButtons[i].frame=CGRectMake(gap+(i%2)*(cw+gap),40+(i/2)*(ch+gap),cw,ch);
            }
        } else if ((_battleMenuKind==OriginalBattleMenuBag && phase==HG_BATTLE_BAG_UI_ACK)
            || (_battleMenuKind==OriginalBattleMenuParty && phase==HG_BATTLE_PARTY_UI_ACK)) _battleMenuButtons[0].frame=_battleMenuMessage.frame;
        for (NSUInteger i=0;i<_battleMenuImages.count;i++) _battleMenuImages[i].frame=CGRectInset(_battleMenuButtons[i].bounds,3,3);
        _status.frame=CGRectInset(_battleMenuPanel.frame,10,10);
    }
    if (_evolutionScene || _captureScene) {
        CGFloat controlWidth=MIN(232,MAX(44,area.size.width-128));
        CGFloat sceneWidth=MIN(MAX(1,area.size.width-controlWidth-12),h*4/3),sceneHeight=sceneWidth*0.75;
        _canvas.frame=CGRectMake(area.origin.x,area.origin.y+(h-sceneHeight)/2,sceneWidth,sceneHeight);
        CGFloat x=CGRectGetMaxX(area)-controlWidth,y=CGRectGetMidY(area)-(_captureScene ? 32 : 70);
        _buttons[@1].frame=CGRectMake(x,y,controlWidth,64);
        _buttons[@2].frame=CGRectMake(x,y+76,controlWidth,64);
        _status.frame=CGRectInset(_canvas.frame,10,10);
    }
    [self layoutPokedexControls:area];
    if (_namingScene) {
        // The image and hit surface share this exact 4:3 rectangle; no letterbox/crop inverse mapping is needed.
        _canvas.frame=CGRectMake(CGRectGetMidX(area)-width/2,area.origin.y+(h-width*0.75)/2,width,width*0.75);
        _buttons[@1].frame=CGRectMake(right,bottom-size-8,side-12,size);
        _buttons[@2].frame=CGRectMake(right,bottom,side-12,size);
        _buttons[@8].frame=CGRectMake(left,area.origin.y+8,side-12,44);
        _buttons[@0x400].frame=CGRectMake(right,bottom-2*size-16,side-12,44);
        _status.frame=CGRectInset(_canvas.frame,10,10);
    }
    if (_optionsScene) {
        CGFloat previewGap=8,columnWidth=MIN(area.size.width-2*side,MAX(1,h-previewGap)*256.0/240.0);
        CGFloat menuHeight=columnWidth*192/256,previewHeight=columnWidth*48/256;
        CGFloat x=CGRectGetMidX(area)-columnWidth/2,y=CGRectGetMidY(area)-(menuHeight+previewGap+previewHeight)/2;
        _canvas.frame=CGRectMake(x,y,columnWidth,menuHeight);
        _optionsPreview.frame=CGRectMake(x,y+menuHeight+previewGap,columnWidth,previewHeight);
        _buttons[@1].frame=CGRectMake(right,bottom-size-8,side-12,size);
        _buttons[@2].frame=CGRectMake(right,bottom,side-12,size);
        _status.frame=CGRectInset(_canvas.frame,10,10);
    }
    [self layoutPCBoxControls:area];
    [self layoutFieldChoiceControls:area];
    [self layoutTrainerCardControls:area];
}
- (void)battlePressed:(UIButton *)button {
    const HGNativeBattleUIState *state = _displayedBattleUIState.bytes;
    if (_displayedBattleUIState.length != sizeof(*state) || !state->ready || button.tag >= 6 || !(state->enabledMask & (1u << button.tag))) return;
    [_condition lock];
    if (!_paused && !_battleChoicePending) {
        _battleChoiceRevision = state->revision; _battleChoiceIndex = (uint32_t)button.tag; _battleChoicePending = YES;
        _automaticMenuEngine = YES;
        _selectedEngine = state->mode == HG_BATTLE_UI_COMMAND && (button.tag == 1 || button.tag == 5) ? 1 : 0;
        _touchPressed = NO;
    }
    uint32_t engine = _selectedEngine;
    [_condition unlock];
    OriginalSetButtonContent(_menuButton,nil,nil,engine ? @"返回场景" : @"触摸菜单");
    for (UIButton *control in _battleButtons) control.enabled = NO;
    _battleBack.enabled = NO;
    _battleBall.enabled = NO;
}
- (NSDictionary *)copyBattleMenuPictures:(const HGNativeBattleBagUIState *)bag party:(const HGNativeBattlePartyUIState *)party frame:(NSData *)frame subFrame:(NSData *)subFrame {
    uint32_t kind=OriginalBattleMenuKind(bag,party);
    if (!kind) { _battleMenuOwnerPictures=nil; _battleMenuOwnerKind=0; return nil; }
    if (subFrame.length!=256*192*4 || !frame.length || frame.length%(192*4)) return nil;
    uint32_t revision=kind==OriginalBattleMenuBag ? bag->revision : party->revision;
    uint32_t phase=kind==OriginalBattleMenuBag ? bag->phase : party->phase;
    BOOL info=kind==OriginalBattleMenuParty && OriginalBattlePartyInfoPhase(phase);
    BOOL ready=kind==OriginalBattleMenuBag ? bag->ready : party->ready;
    BOOL keep=_battleMenuOwnerPictures && _battleMenuOwnerKind==kind && _battleMenuOwnerRevision==revision && _battleMenuOwnerPhase==phase;
    NSMutableDictionary *pictures=keep ? [_battleMenuOwnerPictures mutableCopy] : [NSMutableDictionary new];
    if (info && ready) {
        [pictures removeObjectForKey:@"preview"];
        if (phase==HG_BATTLE_PARTY_UI_CHECK_MOVES) {
            UIImage *name=OriginalFrameRegion(subFrame,40,8,72,16); if (!name) return nil;
            pictures[@"selected"]=name;
        } else if (phase!=HG_BATTLE_PARTY_UI_LEARN_LIST) {
            UIImage *content=OriginalFrameRegion(subFrame,0,0,256,152); if (!content) return nil;
            pictures[@"content"]=content;
        }
    } else if (!info) {
        UIImage *preview=OriginalPixelImage(frame,(uint32_t)(frame.length/(192*4)),192);
        if (!preview) return nil;
        pictures[@"preview"]=preview;
    }
    NSArray *old=pictures[@"buttons"]; NSMutableArray *images=[NSMutableArray new];
    uint32_t count=kind==OriginalBattleMenuBag ? MIN(bag->buttonCount,HG_BATTLE_BAG_UI_BUTTONS) : MIN(party->buttonCount,HG_BATTLE_PARTY_UI_BUTTONS);
    for (NSUInteger i=0;i<HG_BATTLE_BAG_UI_BUTTONS;i++) {
        UIImage *image=nil;
        if (ready && i<count && kind==OriginalBattleMenuBag) {
            const HGNativeBattleBagUIButton *button=&bag->buttons[i];
            image=OriginalFrameRegion(subFrame,button->cropX,button->cropY,button->cropWidth,button->cropHeight);
        } else if (ready && i<count) {
            const HGNativeBattlePartyUIButton *button=&party->buttons[i];
            image=OriginalFrameRegion(subFrame,button->cropX,button->cropY,button->cropWidth,button->cropHeight);
        }
        [images addObject:image ?: (i<old.count ? old[i] : NSNull.null)];
    }
    const HGNativePartyUIWindow *windows[4]={0}; NSArray *names;
    if (kind==OriginalBattleMenuBag) {
        windows[0]=&bag->selectedName; windows[1]=&bag->selectedQuantityText; windows[2]=&bag->description; windows[3]=&bag->message;
        names=@[@"selected",@"quantity",@"description",@"message"];
    } else if (!info) { windows[0]=&party->prompt; windows[1]=&party->message; names=@[@"prompt",@"message"]; }
    else names=@[];
    for (NSUInteger i=0;i<names.count;i++) {
        UIImage *image=OriginalWindowImage(windows[i],_env->memory->data,_env->memory->size);
        if (image) pictures[names[i]]=image; else if (ready) [pictures removeObjectForKey:names[i]];
    }
    BOOL ack=(kind==OriginalBattleMenuBag && phase==HG_BATTLE_BAG_UI_ACK) || (kind==OriginalBattleMenuParty && phase==HG_BATTLE_PARTY_UI_ACK);
    if (ack && pictures[@"message"]) images[0]=pictures[@"message"];
    pictures[@"buttons"]=[images copy];
    _battleMenuOwnerKind=kind; _battleMenuOwnerRevision=revision; _battleMenuOwnerPhase=phase;
    _battleMenuOwnerPictures=[pictures copy]; return _battleMenuOwnerPictures;
}
- (void)battleMenuPressed:(UIButton *)button {
    if (button.tag<0 || button.tag>=HG_BATTLE_BAG_UI_BUTTONS) return;
    const HGNativeBattleBagUIState *bag=_displayedBattleBagUIState.length==sizeof(*bag) ? _displayedBattleBagUIState.bytes : NULL;
    const HGNativeBattlePartyUIState *party=_displayedBattlePartyUIState.length==sizeof(*party) ? _displayedBattlePartyUIState.bytes : NULL;
    uint32_t kind=OriginalBattleMenuKind(bag,party),revision,phase,x,y,index=(uint32_t)button.tag;
    if (kind==OriginalBattleMenuBag) {
        revision=bag->revision; phase=bag->phase;
        if (!HGNativeBattleBagUIResolveTouch(bag,revision,phase,index,&x,&y)) return;
    } else if (kind==OriginalBattleMenuParty) {
        revision=party->revision; phase=party->phase;
        if (!HGNativeBattlePartyUIResolveTouch(party,revision,phase,index,&x,&y)) return;
    } else return;
    [_condition lock];
    if (!_paused && !_stopping && !_battleMenuChoicePending && !_battleMenuTouchActive && !_battleMenuTouchCooling) {
        _battleMenuChoiceKind=kind; _battleMenuChoiceRevision=revision; _battleMenuChoicePhase=phase; _battleMenuChoiceIndex=index; _battleMenuChoicePending=YES;
    }
    [_condition unlock];
}
- (void)takeBattleMenuTouchLocked:(const HGNativeBattleBagUIState *)bag party:(const HGNativeBattlePartyUIState *)party reads:(uint32_t)reads x:(uint32_t *)x y:(uint32_t *)y pressed:(BOOL *)pressed {
    uint32_t kind=OriginalBattleMenuKind(bag,party);
    uint32_t revision=kind==OriginalBattleMenuBag ? bag->revision : kind==OriginalBattleMenuParty ? party->revision : 0;
    uint32_t phase=kind==OriginalBattleMenuBag ? bag->phase : kind==OriginalBattleMenuParty ? party->phase : 0;
    BOOL ready=kind==OriginalBattleMenuBag ? bag->ready : kind==OriginalBattleMenuParty ? party->ready : NO;
    if (_battleMenuTouchCooling && reads-_battleMenuTouchReads>=2) _battleMenuTouchCooling=NO;
    if (_battleMenuTouchActive && (kind!=_battleMenuTouchKind || revision!=_battleMenuTouchRevision || reads-_battleMenuTouchReads>=4)) {
        _battleMenuTouchActive=NO; _battleMenuTouchCooling=YES; _battleMenuTouchReads=reads;
    }
    if (_battleMenuTouchCooling) { *pressed=NO; return; }
    if (_battleMenuChoicePending) {
        if (kind!=_battleMenuChoiceKind || revision!=_battleMenuChoiceRevision || phase!=_battleMenuChoicePhase) _battleMenuChoicePending=NO;
        else if (ready) {
            BOOL valid=kind==OriginalBattleMenuBag
                ? HGNativeBattleBagUIResolveTouch(bag,revision,phase,_battleMenuChoiceIndex,&_battleMenuTouchX,&_battleMenuTouchY)
                : HGNativeBattlePartyUIResolveTouch(party,revision,phase,_battleMenuChoiceIndex,&_battleMenuTouchX,&_battleMenuTouchY);
            if (valid) { _battleMenuTouchActive=YES; _battleMenuTouchReads=reads; _battleMenuTouchKind=kind; _battleMenuTouchRevision=revision; }
            _battleMenuChoicePending=NO;
        }
    }
    if (_battleMenuTouchActive) { *x=_battleMenuTouchX; *y=_battleMenuTouchY; *pressed=YES; }
}
- (void)updateBattleMenuControls:(NSData *)bagData party:(NSData *)partyData pictures:(NSDictionary *)pictures paused:(BOOL)paused engine:(uint32_t)engine {
    const HGNativeBattleBagUIState *bag=bagData.length==sizeof(*bag) ? bagData.bytes : NULL;
    const HGNativeBattlePartyUIState *party=partyData.length==sizeof(*party) ? partyData.bytes : NULL;
    uint32_t kind=OriginalBattleMenuKind(bag,party),phase=kind==OriginalBattleMenuBag ? bag->phase : kind==OriginalBattleMenuParty ? party->phase : 0;
    BOOL info=kind==OriginalBattleMenuParty && OriginalBattlePartyInfoPhase(phase);
    BOOL scene=(bag && bag->active) || (party && party->active);
    BOOL changed=kind!=_battleMenuKind || scene!=_battleMenuScene
        || (kind==OriginalBattleMenuBag && (_displayedBattleBagUIState.length!=sizeof(*bag) || ((const HGNativeBattleBagUIState *)_displayedBattleBagUIState.bytes)->revision!=bag->revision))
        || (kind==OriginalBattleMenuParty && (_displayedBattlePartyUIState.length!=sizeof(*party) || ((const HGNativeBattlePartyUIState *)_displayedBattlePartyUIState.bytes)->revision!=party->revision));
    if (scene && !_battleMenuScene) {
        [_condition lock]; _keys=_pendingKeys=_submittedKeys=0; _touchPressed=_battleChoicePending=NO; [_condition unlock]; [self resetJoystick];
    }
    _battleMenuScene=scene; _battleMenuKind=kind; _displayedBattleBagUIState=bagData; _displayedBattlePartyUIState=partyData;
    _battleMenuPanel.hidden=!kind;
    if (scene) { _battleMode=0; _battleScene=!engine; _battleBar.hidden=YES; }
    if (changed) [self.view setNeedsLayout];
    [_condition lock]; BOOL waiting=_battleMenuChoicePending || _battleMenuTouchActive || _battleMenuTouchCooling; [_condition unlock];
    BOOL ready=!paused && !waiting && (kind==OriginalBattleMenuBag ? bag->ready : kind==OriginalBattleMenuParty ? party->ready : NO);
    uint32_t count=kind==OriginalBattleMenuBag ? MIN(bag->buttonCount,HG_BATTLE_BAG_UI_BUTTONS) : kind==OriginalBattleMenuParty ? MIN(party->buttonCount,HG_BATTLE_PARTY_UI_BUTTONS) : 0;
    uint32_t mask=kind==OriginalBattleMenuBag ? bag->enabledMask : kind==OriginalBattleMenuParty ? party->enabledMask : 0;
    NSArray *images=pictures[@"buttons"],*pockets=@[@"恢复 HP／PP",@"恢复异常状态",@"精灵球",@"战斗道具",@"上次使用的道具",@"返回"];
    NSArray *monActions=@[@"换上场",@"查看属性",@"查看招式",@"返回"];
    UIColor *border=[UIColor colorWithRed:0.45 green:0.62 blue:0.72 alpha:1],*selected=[UIColor colorWithRed:0.91 green:0.49 blue:0.12 alpha:1];
    for (NSUInteger i=0;i<_battleMenuButtons.count;i++) {
        UIButton *button=_battleMenuButtons[i]; UIImage *image=i<images.count && [images[i] isKindOfClass:UIImage.class] ? images[i] : nil;
        BOOL hidden=i>=count,highlight=NO; NSString *label=@"继续",*title=nil;
        if (kind==OriginalBattleMenuBag) {
            if (phase==HG_BATTLE_BAG_UI_POCKETS && i<pockets.count) label=pockets[i];
            else if (phase==HG_BATTLE_BAG_UI_ITEMS) {
                if (i<6) {
                    uint32_t item=bag->itemId[i]; hidden=hidden || !item;
                    label=[NSString stringWithFormat:@"%@，数量 %u",item<_itemNames.count ? _itemNames[item] : @"道具",bag->quantity[i]];
                    highlight=i==bag->selectedSlot;
                } else if (i<9) label=(@[@"返回",@"上一页",@"下一页"])[i-6];
            } else if (phase==HG_BATTLE_BAG_UI_USE && i<2) label=i ? @"返回" : [NSString stringWithFormat:@"使用%@",bag->selectedItemId<_itemNames.count ? _itemNames[bag->selectedItemId] : @"道具"];
        } else if (kind==OriginalBattleMenuParty) {
            if (phase==HG_BATTLE_PARTY_UI_ROSTER && i<6) {
                const HGNativeBattlePartyUIMon *mon=&party->mons[i]; hidden=hidden || !mon->slotType;
                NSString *name=mon->species<_speciesNames.count ? _speciesNames[mon->species] : @"宝可梦";
                NSString *status=OriginalPartyStatusName(mon->status);
                label=mon->isEgg ? @"蛋" : [NSString stringWithFormat:@"%@，Lv.%u，HP %u／%u%@%@",name,mon->level,mon->hp,mon->maxHp,status.length ? [@"，" stringByAppendingString:status] : @"",mon->heldItem ? @"，携带道具" : @""];
                highlight=i==party->selectedSlot;
            } else if (phase==HG_BATTLE_PARTY_UI_ROSTER && i==6) label=@"返回";
            else if (phase==HG_BATTLE_PARTY_UI_MON_MENU && i<monActions.count) label=monActions[i];
            else if (phase==HG_BATTLE_PARTY_UI_LEARN_LIST) {
                hidden=hidden || i==5;
                if (i<5) {
                    uint32_t move=i<4 ? party->moves[i].moveId : party->moveToLearn;
                    NSString *name=move<_moveNames.count ? _moveNames[move] : @"招式";
                    label=i<4 ? [NSString stringWithFormat:@"%@，PP %u／%u",name,party->moves[i].pp,party->moves[i].maxPP]
                        : [@"要学习的招式，" stringByAppendingString:name];
                    highlight=i==party->selectedMove;
                } else if (i==6) label=@"返回";
            } else if (phase==HG_BATTLE_PARTY_UI_LEARN_DETAIL) {
                hidden=hidden || i==1;
                if (i==0) label=@"确认";
                else if (i==2) label=@"返回";
            }
            else if (phase==HG_BATTLE_PARTY_UI_SUMMARY && i<4) label=(@[@"上一只宝可梦",@"下一只宝可梦",@"查看招式",@"返回"])[i];
            else if ((phase==HG_BATTLE_PARTY_UI_CHECK_MOVES || phase==HG_BATTLE_PARTY_UI_MOVE_DETAIL) && i<4) {
                const HGNativeBattlePartyUIMove *move=&party->moves[i];
                NSString *name=move->moveId<_moveNames.count ? _moveNames[move->moveId] : @"招式";
                label=move->moveId ? [NSString stringWithFormat:@"%@，PP %u／%u",name,move->pp,move->maxPP] : @"无招式";
                highlight=phase==HG_BATTLE_PARTY_UI_MOVE_DETAIL && i==party->selectedMove;
                if (phase==HG_BATTLE_PARTY_UI_MOVE_DETAIL && move->moveId) title=name;
            } else if (phase==HG_BATTLE_PARTY_UI_CHECK_MOVES && i<8) label=(@[@"上一只宝可梦",@"下一只宝可梦",@"查看属性",@"返回"])[i-4];
            else if (phase==HG_BATTLE_PARTY_UI_MOVE_DETAIL && i==4) label=@"返回";
        }
        button.hidden=hidden; button.enabled=ready && !hidden && (mask&(1u<<i)) && image!=nil;
        button.layer.borderColor=(highlight ? selected : border).CGColor;
        button.accessibilityLabel=label; button.accessibilityTraits=UIAccessibilityTraitButton | (highlight ? UIAccessibilityTraitSelected : 0) | (button.enabled ? 0 : UIAccessibilityTraitNotEnabled);
        _battleMenuImages[i].image=image;
        [button setTitle:title forState:UIControlStateNormal]; if (title) [button bringSubviewToFront:button.titleLabel];
    }
    _battleMenuTitle.text=kind==OriginalBattleMenuBag ? @"战斗背包" : kind==OriginalBattleMenuParty ? (party->mode==HG_BATTLE_PARTY_UI_USE_ITEM ? @"使用道具" : party->mode==HG_BATTLE_PARTY_UI_LEARN_MOVE ? @"学习招式" : @"宝可梦") : nil;
    _battleMenuPreview.image=pictures[@"preview"]; _battleMenuSelected.image=pictures[@"selected"]; _battleMenuQuantity.image=pictures[@"quantity"];
    _battleMenuDescription.image=info ? pictures[@"content"] : pictures[@"description"] ?: pictures[@"prompt"]; _battleMenuMessage.image=pictures[@"message"];
    _battleMenuTitle.hidden=_battleMenuPreview.hidden=_battleMenuQuantity.hidden=info;
    _battleMenuSelected.hidden=info && phase!=HG_BATTLE_PARTY_UI_CHECK_MOVES;
    BOOL moveList=phase==HG_BATTLE_PARTY_UI_CHECK_MOVES || phase==HG_BATTLE_PARTY_UI_LEARN_LIST;
    _battleMenuDescription.hidden=info && moveList;
    _battleMenuDescription.isAccessibilityElement=info && !moveList;
    _battleMenuSelected.isAccessibilityElement=info && phase==HG_BATTLE_PARTY_UI_CHECK_MOVES;
    NSString *selectedName=info && party->mons[party->selectedSlot].species<_speciesNames.count ? _speciesNames[party->mons[party->selectedSlot].species] : @"宝可梦";
    _battleMenuSelected.accessibilityLabel=selectedName;
    if (phase==HG_BATTLE_PARTY_UI_LEARN_DETAIL) {
        uint32_t move=party->selectedMove<4 ? party->moves[party->selectedMove].moveId : party->moveToLearn;
        NSString *name=move<_moveNames.count ? _moveNames[move] : @"招式";
        _battleMenuDescription.accessibilityLabel=[NSString stringWithFormat:@"%@，%@，招式详情与原版提示",selectedName,name];
    } else _battleMenuDescription.accessibilityLabel=phase==HG_BATTLE_PARTY_UI_MOVE_DETAIL ? [NSString stringWithFormat:@"%@，%@",selectedName,_battleMenuButtons[party->selectedMove].accessibilityLabel] : [selectedName stringByAppendingString:@"，属性"];
    _battleMenuMessage.hidden=!(kind==OriginalBattleMenuParty && phase==HG_BATTLE_PARTY_UI_MESSAGE);
}
- (NSDictionary *)copyPartyPictures:(const HGNativePartyUIState *)state frame:(NSData *)frame {
    BOOL tm=HGNativePartyUIIsTMHM(state),candy=HGNativePartyUIIsRareCandy(state);
    uint32_t role=candy && state->actionCount ? state->actions[0].textId : 0;
    BOOL stats=candy && (role==HG_PARTY_UI_TEXT_CANDY_STATS_DIFF || role==HG_PARTY_UI_TEXT_CANDY_STATS_TOTAL);
    if (!state->active || state->panelLayout || (state->context && !HGNativePartyUIIsItemProgress(state) && !HGNativePartyUIIsPotionUse(state) && !tm && !candy)) { _partyOwnerPictures=nil; return nil; }
    NSMutableDictionary *pictures=_partyOwnerPictures ? [_partyOwnerPictures mutableCopy] : [NSMutableDictionary new];
    BOOL sameView=[pictures[@"messageRevision"] unsignedIntValue]==state->revision
        && [pictures[@"messageContext"] unsignedIntValue]==state->context && [pictures[@"messagePhase"] unsignedIntValue]==state->phase;
    BOOL sameMessage=(HGNativePartyUIIsItemProgress(state) || HGNativePartyUIIsPotionUse(state) || tm || candy) && sameView;
    for (NSString *key in @[@"cards",@"actions"]) {
        NSUInteger count=[key isEqual:@"actions"] ? HG_PARTY_UI_ACTIONS : HG_PARTY_UI_SLOTS;
        NSArray *old=pictures[key]; NSMutableArray *images=[NSMutableArray new];
        for (NSUInteger i=0;i<count;i++) {
            UIImage *image=nil;
            if ([key isEqual:@"actions"]) {
                if ((tm || candy) && state->phase==HG_PARTY_UI_TM_YES_NO && state->ready && i<state->actionCount) image=OriginalFrameRegion(frame,200,80+32*i,48,32);
                else image=OriginalWindowImage(&state->actions[i].text,_env->memory->data,_env->memory->size);
            } else if (!stats && state->mons[i].active && state->mons[i].nickname.pixelBuffer) image=OriginalPartyCard(frame,&state->mons[i]);
            [images addObject:image ?: (sameView && i<old.count ? old[i] : NSNull.null)];
        }
        pictures[key]=[images copy];
    }
    const HGNativePartyUIWindow *windows[]={&state->selectedName,&state->selectedItem,&state->message};
    NSArray *keys=@[@"selected",@"item",@"message"];
    for (NSUInteger i=0;i<3;i++) {
        UIImage *image=stats && i==2 ? nil : OriginalWindowImage(windows[i],_env->memory->data,_env->memory->size);
        if (image || i!=2 || !sameMessage || stats) pictures[keys[i]]=image ?: NSNull.null;
    }
    // Bind advances revision for each owner; the role separates the two phase-5 statistics pages.
    BOOL sameBody=stats && [pictures[@"bodyRevision"] unsignedIntValue]==state->revision && [pictures[@"bodyContext"] unsignedIntValue]==state->context
        && [pictures[@"bodyPhase"] unsignedIntValue]==state->phase && [pictures[@"bodyRole"] unsignedIntValue]==role;
    NSData *statsValues=nil;
    if (stats && state->ready) {
        uint32_t address=w2c_hgengine_HGNativePartyUIGetStats(_game,state->revision);
        if (address && address<=_env->memory->size && 12<=_env->memory->size-address)
            statsValues=[NSData dataWithBytes:_env->memory->data+address length:12];
    }
    UIImage *body=statsValues ? OriginalFrameRegion(frame,0,0,128,112) : nil;
    pictures[@"body"]=body ?: (sameBody ? pictures[@"body"] : NSNull.null);
    pictures[@"stats"]=statsValues ?: (sameBody ? pictures[@"stats"] : NSNull.null);
    pictures[@"bodyRevision"]=@(state->revision); pictures[@"bodyContext"]=@(state->context); pictures[@"bodyPhase"]=@(state->phase); pictures[@"bodyRole"]=@(role);
    pictures[@"messageRevision"]=@(state->revision); pictures[@"messageContext"]=@(state->context); pictures[@"messagePhase"]=@(state->phase);
    _partyOwnerPictures=[pictures copy]; return _partyOwnerPictures;
}
- (void)queuePartyTouch:(uint32_t)kind index:(uint32_t)index {
    const HGNativePartyUIState *state=_displayedPartyUIState.bytes; uint32_t x,y;
    if (_displayedPartyUIState.length!=sizeof(*state) || !HGNativePartyUIResolveTouch(state,state->revision,state->phase,kind,index,&x,&y)) return;
    [_condition lock];
    if (!_paused && !_stopping && !_partyChoicePending && !_partyTouchActive) {
        _partyChoiceRevision=state->revision; _partyChoicePhase=state->phase; _partyChoiceKind=kind; _partyChoiceIndex=index; _partyChoicePending=YES;
    }
    [_condition unlock];
}
- (void)partyMonPressed:(UIButton *)button { [self queuePartyTouch:HG_PARTY_UI_TOUCH_MON index:(uint32_t)button.tag]; }
- (void)partyActionPressed:(UIButton *)button { [self queuePartyTouch:HG_PARTY_UI_TOUCH_ACTION index:(uint32_t)button.tag]; }
- (void)partyBackPressed:(UIButton *)button { [self queuePartyTouch:HG_PARTY_UI_TOUCH_CANCEL index:0]; }
- (void)takePartyTouchLocked:(const HGNativePartyUIState *)state reads:(uint32_t)reads keys:(uint32_t *)keys x:(uint32_t *)x y:(uint32_t *)y pressed:(BOOL *)pressed {
    if (HGNativePartyUIIsItemProgress(state) || HGNativePartyUIIsPotionUse(state) || HGNativePartyUIIsTMHM(state) || HGNativePartyUIIsRareCandy(state)) {
        _keys&=~3u; _pendingKeys&=~3u; _submittedKeys&=~3u; *keys&=~3u;
        _touchPressed=NO; *pressed=NO;
    }
    if (_partyChoicePending) {
        if (!state->active || state->revision!=_partyChoiceRevision || state->phase!=_partyChoicePhase) _partyChoicePending=NO;
        else if (state->ready) {
            if (HGNativePartyUIResolveTouch(state,_partyChoiceRevision,_partyChoicePhase,_partyChoiceKind,_partyChoiceIndex,&_partyTouchX,&_partyTouchY)) {
                _partyTouchActive=YES; _partyTouchReads=reads; _partyTouchRevision=state->revision;
            }
            _partyChoicePending=NO;
        }
    }
    if (_partyTouchActive) {
        // Keep a tap through original PAD/TP sampling, then release it before another menu action.
        BOOL release=!state->active || state->revision!=_partyTouchRevision || reads-_partyTouchReads>=4;
        *x=_partyTouchX; *y=_partyTouchY; *pressed=!release;
        if (release) _partyTouchActive=NO;
    }
}
- (void)updatePartyControls:(NSData *)data pictures:(NSDictionary *)pictures paused:(BOOL)paused {
    const HGNativePartyUIState *state=data.bytes;
    BOOL candy=data.length==sizeof(*state) && HGNativePartyUIIsRareCandy(state),candyTarget=candy && state->phase==HG_PARTY_UI_ITEM_TARGET;
    uint32_t role=candy && state->actionCount ? state->actions[0].textId : 0;
    BOOL stats=candy && (role==HG_PARTY_UI_TEXT_CANDY_STATS_DIFF || role==HG_PARTY_UI_TEXT_CANDY_STATS_TOTAL);
    BOOL progress=data.length==sizeof(*state) && !candy && HGNativePartyUIIsItemProgress(state);
    BOOL potion=data.length==sizeof(*state) && !candy && HGNativePartyUIIsPotionUse(state);
    BOOL tm=data.length==sizeof(*state) && HGNativePartyUIIsTMHM(state),tmTarget=tm && state->phase==HG_PARTY_UI_TM_TARGET;
    BOOL scene=data.length==sizeof(*state) && state->active && !state->panelLayout && (!state->context || progress || potion || tm || candy);
    BOOL panel=scene && (candy || tm || potion || progress || state->phase==HG_PARTY_UI_ROSTER || state->phase==HG_PARTY_UI_CONTEXT || state->phase==HG_PARTY_UI_SUBCONTEXT);
    BOOL changed=_partyScene!=scene || !_displayedPartyUIState || data.length!=_displayedPartyUIState.length
        || (scene && memcmp(data.bytes,_displayedPartyUIState.bytes,offsetof(HGNativePartyUIState,mons)));
    if (scene && !_partyScene) {
        [_condition lock]; _keys=_pendingKeys=_submittedKeys=0; _touchPressed=NO; [_condition unlock]; [self resetJoystick];
    }
    _partyScene=scene; _displayedPartyUIState=data;
    _partyPanel.hidden=!panel; _partyBack.hidden=!panel || (tm && !tmTarget) || (candy && !candyTarget);
    _partyBody.image=stats && [pictures[@"body"] isKindOfClass:UIImage.class] ? pictures[@"body"] : nil;
    _partyBody.hidden=!panel || !stats || !_partyBody.image;
    _partyBody.accessibilityLabel=stats ? (role==HG_PARTY_UI_TEXT_CANDY_STATS_DIFF ? @"能力变化" : @"当前能力值") : nil;
    _partyBody.accessibilityValue=nil;
    NSData *statsData=[pictures[@"stats"] isKindOfClass:NSData.class] ? pictures[@"stats"] : nil;
    if (stats && statsData.length==12) {
        uint16_t values[6]; memcpy(values,statsData.bytes,sizeof(values));
        NSArray *labels=@[@"HP",@"攻击",@"防御",@"特攻",@"特防",@"速度"];
        NSString *sign=role==HG_PARTY_UI_TEXT_CANDY_STATS_DIFF ? @"+" : @"";
        NSMutableArray *description=[NSMutableArray new];
        for (NSUInteger i=0;i<6;i++) [description addObject:[NSString stringWithFormat:@"%@ %@%u",labels[i],sign,(unsigned)values[i]]];
        _partyBody.accessibilityValue=[description componentsJoinedByString:@"，"];
    }
    OriginalSetButtonContent(_partyBack,nil,nil,progress ? @"继续" : @"返回");
    _partyBack.accessibilityLabel=_partyBack.currentTitle;
    if (changed) [self.view setNeedsLayout];
    if (!panel) return;
    [_condition lock]; BOOL waiting=_partyChoicePending || _partyTouchActive; [_condition unlock];
    BOOL ready=state->ready && !paused && !waiting && (!stats || _partyBody.image);
    uint32_t touchX,touchY;
    _partyBack.enabled=ready && HGNativePartyUIResolveTouch(state,state->revision,state->phase,HG_PARTY_UI_TOUCH_CANCEL,0,&touchX,&touchY);
    NSArray *cards=pictures[@"cards"],*actions=pictures[@"actions"];
    for (NSUInteger i=0;i<HG_PARTY_UI_SLOTS;i++) {
        const HGNativePartyUIMon *mon=&state->mons[i]; UIButton *button=_partyMons[i];
        button.hidden=stats || !(state->slotMask&(1u<<i)) || !mon->active;
        button.enabled=ready && HGNativePartyUIResolveTouch(state,state->revision,state->phase,HG_PARTY_UI_TOUCH_MON,(uint32_t)i,&touchX,&touchY);
        button.layer.borderColor=(i==state->selected ? [UIColor colorWithRed:0.91 green:0.49 blue:0.12 alpha:1] : [UIColor colorWithRed:0.45 green:0.62 blue:0.72 alpha:1]).CGColor;
        _partyCards[i].image=i<cards.count && [cards[i] isKindOfClass:UIImage.class] ? cards[i] : nil;
        NSString *name=mon->species<_speciesNames.count ? _speciesNames[mon->species] : @"宝可梦";
        NSString *stats=mon->isEgg ? @"蛋" : [NSString stringWithFormat:@"Lv.%u  %u/%u",mon->level,mon->hp,mon->maxHp];
        NSString *status=OriginalPartyStatusName(mon->status);
        button.accessibilityLabel=mon->isEgg ? @"蛋" : [NSString stringWithFormat:@"%@，%@，%@%@%@",name,stats,status,mon->heldItem ? @"，携带道具" : @"",i==state->selected ? @"，已选择" : @""];
        button.accessibilityHint=state->phase==HG_PARTY_UI_ROSTER ? @"打开原版宝可梦操作菜单" : (potion && !progress) || tmTarget || candyTarget ? @"选择原版道具使用目标" : nil;
        button.accessibilityTraits=UIAccessibilityTraitButton | (i==state->selected ? UIAccessibilityTraitSelected : 0);
    }
    for (NSUInteger i=0;i<HG_PARTY_UI_ACTIONS;i++) {
        UIButton *button=_partyActions[i]; button.hidden=progress || potion || tmTarget || candyTarget || i>=state->actionCount;
        button.enabled=ready && !progress && !potion && i<state->actionCount && (state->actionEnabledMask&(1u<<i))
            && (!(tm || candy) || HGNativePartyUIResolveTouch(state,state->revision,state->phase,HG_PARTY_UI_TOUCH_ACTION,(uint32_t)i,&touchX,&touchY));
        uint32_t textId=state->actions[i].textId;
        NSString *name=candy && (textId==HG_PARTY_UI_TEXT_CANDY_ACK || textId==HG_PARTY_UI_TEXT_CANDY_BUSY || textId==HG_PARTY_UI_TEXT_CANDY_STATS_DIFF || textId==HG_PARTY_UI_TEXT_CANDY_STATS_TOTAL) ? @"继续"
            : candy && textId==HG_PARTY_UI_TEXT_CANDY_YES ? @"是" : candy && textId==HG_PARTY_UI_TEXT_CANDY_NO ? @"否"
            : tm && textId==HG_PARTY_UI_TEXT_TM_ACK ? @"继续" : tm && textId==HG_PARTY_UI_TEXT_TM_YES ? @"是" : tm && textId==HG_PARTY_UI_TEXT_TM_NO ? @"否"
            : textId<_partyActionNames.count && [_partyActionNames[textId] length] ? _partyActionNames[textId] : @"操作";
        UIButtonConfiguration *config=UIButtonConfiguration.filledButtonConfiguration;
        config.contentInsets=NSDirectionalEdgeInsetsMake(2,4,2,4);
        config.baseBackgroundColor=i==state->actionSelected ? [UIColor colorWithRed:1 green:0.81 blue:0.48 alpha:1] : UIColor.whiteColor;
        config.baseForegroundColor=_partyTitle.textColor;
        UIImage *image=i<actions.count && [actions[i] isKindOfClass:UIImage.class] ? actions[i] : nil;
        if ((tm || candy) && state->phase==HG_PARTY_UI_TM_YES_NO && !image) button.enabled=NO;
        OriginalSetButtonContent(button,config,image,name); button.accessibilityLabel=name;
    }
    _partySelectedName.image=[pictures[@"selected"] isKindOfClass:UIImage.class] ? pictures[@"selected"] : nil;
    _partySelectedItem.image=[pictures[@"item"] isKindOfClass:UIImage.class] ? pictures[@"item"] : nil;
    _partyMessage.image=(progress || potion || tm || candy) && !stats && [pictures[@"message"] isKindOfClass:UIImage.class] ? pictures[@"message"] : nil;
    _partyMessage.hidden=!(progress || potion || tm || candy) || stats || !_partyMessage.image;
    _partySelectedName.hidden=progress || potion || tm || candy || state->phase==HG_PARTY_UI_ROSTER || !_partySelectedName.image;
    _partySelectedItem.hidden=progress || potion || tm || candy || state->phase==HG_PARTY_UI_ROSTER || !_partySelectedItem.image;
}
- (NSDictionary *)copyBagPictures:(const HGNativeBagUIState *)state frame:(NSData *)frame subFrame:(NSData *)subFrame {
    if (!state->active || (state->mode && !HGNativeBagUIIsList(state))) { _bagOwnerPictures=nil; return nil; }
    BOOL tm=HGNativeBagUIIsTMHM(state);
    BOOL sameView=[_bagOwnerPictures[@"revision"] unsignedIntValue]==state->revision
        && [_bagOwnerPictures[@"mode"] unsignedIntValue]==state->mode && [_bagOwnerPictures[@"phase"] unsignedIntValue]==state->phase;
    if (!state->ready && !tm) { if (!sameView) _bagOwnerPictures=nil; return _bagOwnerPictures; }
    NSMutableDictionary *pictures=sameView && _bagOwnerPictures ? [_bagOwnerPictures mutableCopy] : [NSMutableDictionary new];
    if (state->ready) pictures[@"preview"]=OriginalPixelImage(frame,256,192) ?: NSNull.null;
    for (NSString *key in @[@"pockets",@"items",@"actions"]) {
        NSUInteger count=[key isEqual:@"pockets"] ? HG_BAG_UI_POCKETS : [key isEqual:@"items"] ? HG_BAG_UI_ITEMS : HG_BAG_UI_ACTIONS;
        NSArray *old=pictures[key]; NSMutableArray *images=[NSMutableArray new];
        for (NSUInteger i=0;i<count;i++) {
            UIImage *image=nil;
            if ([key isEqual:@"actions"]) {
                if (tm && state->ready && state->actionCount==2 && i<state->actionCount) image=OriginalFrameRegion(subFrame,200,48+32*i,48,32);
                else image=OriginalWindowImage(&state->actions[i].text,_env->memory->data,_env->memory->size);
            } else if (HGNativeBagUIIsList(state) && state->ready) {
                if ([key isEqual:@"pockets"]) {
                    const HGNativeBagUIPocket *pocket=&state->pockets[i];
                    if (pocket->enabled) image=OriginalFrameRegion(subFrame,pocket->cropX,pocket->cropY,pocket->cropWidth,pocket->cropHeight);
                } else {
                    const HGNativeBagUIItem *item=&state->items[i];
                    if (item->enabled) image=OriginalFrameRegion(subFrame,item->cropX,item->cropY,item->cropWidth,item->cropHeight);
                }
            }
            [images addObject:image ?: (i<old.count ? old[i] : NSNull.null)];
        }
        pictures[key]=[images copy];
    }
#ifdef HEARTGOLD_ORIGINAL_APP_CHECK
    NSMutableArray *glyphs=[NSMutableArray new];
    for (NSUInteger i=0;i<HG_BAG_UI_ITEMS;i++) {
        UIImage *glyph=HGNativeBagUIIsList(state) ? OriginalWindowImage(&state->items[i].text,_env->memory->data,_env->memory->size) : nil;
        [glyphs addObject:glyph ?: NSNull.null];
    }
    pictures[@"itemGlyphs"]=[glyphs copy];
#endif
    const HGNativePartyUIWindow *windows[]={&state->prompt,&state->message,&state->page,&state->selectedEntry};
    NSArray *keys=@[@"prompt",@"message",@"page",@"selected"];
    for (NSUInteger i=0;i<4;i++) {
        UIImage *image=OriginalWindowImage(windows[i],_env->memory->data,_env->memory->size);
        if (image) pictures[keys[i]]=image;
    }
    pictures[@"revision"]=@(state->revision); pictures[@"mode"]=@(state->mode); pictures[@"phase"]=@(state->phase);
    _bagOwnerPictures=[pictures copy]; return _bagOwnerPictures;
}
- (void)queueBagTouch:(uint32_t)kind index:(uint32_t)index {
    const HGNativeBagUIState *state=_displayedBagUIState.bytes; uint32_t x,y;
    if (_displayedBagUIState.length!=sizeof(*state) || !HGNativeBagUIResolveTouch(state,state->revision,state->phase,kind,index,&x,&y)) return;
    [_condition lock];
    if (!_paused && !_stopping && !_bagChoicePending && !_bagTouchActive) {
        _bagChoiceRevision=state->revision; _bagChoicePhase=state->phase; _bagChoiceKind=kind; _bagChoiceIndex=index; _bagChoicePending=YES;
    }
    [_condition unlock];
}
- (void)bagPressed:(UIButton *)button {
    uint32_t group=(uint32_t)button.tag>>8;
    if (group<=2) [self queueBagTouch:group==0 ? HG_BAG_UI_TOUCH_POCKET : group==1 ? HG_BAG_UI_TOUCH_ITEM : HG_BAG_UI_TOUCH_ACTION index:(uint32_t)button.tag&255];
}
- (void)bagNavigate:(UIButton *)button {
    if (button.tag>=0 && button.tag<3) [self queueBagTouch:button.tag==0 ? HG_BAG_UI_TOUCH_CANCEL : button.tag==1 ? HG_BAG_UI_TOUCH_PREVIOUS : HG_BAG_UI_TOUCH_NEXT index:0];
}
- (void)takeBagTouchLocked:(const HGNativeBagUIState *)state reads:(uint32_t)reads x:(uint32_t *)x y:(uint32_t *)y pressed:(BOOL *)pressed {
    if (_bagChoicePending) {
        if (!state->active || state->revision!=_bagChoiceRevision || state->phase!=_bagChoicePhase) _bagChoicePending=NO;
        else if (state->ready) {
            if (HGNativeBagUIResolveTouch(state,_bagChoiceRevision,_bagChoicePhase,_bagChoiceKind,_bagChoiceIndex,&_bagTouchX,&_bagTouchY)) {
                _bagTouchActive=YES; _bagTouchReads=reads; _bagTouchRevision=state->revision;
            }
            _bagChoicePending=NO;
        }
    }
    if (_bagTouchActive) {
        BOOL release=!state->active || state->revision!=_bagTouchRevision || reads-_bagTouchReads>=4;
        *x=_bagTouchX; *y=_bagTouchY; *pressed=!release;
        if (release) _bagTouchActive=NO;
    }
}
- (void)updateBagControls:(NSData *)data pictures:(NSDictionary *)pictures paused:(BOOL)paused {
    const HGNativeBagUIState *state=data.bytes;
    BOOL list=data.length==sizeof(*state) && HGNativeBagUIIsList(state),tm=data.length==sizeof(*state) && HGNativeBagUIIsTMHM(state);
    BOOL scene=data.length==sizeof(*state) && state->active && (!state->mode || list);
    BOOL panel=scene && (list || tm || state->phase==HG_BAG_UI_ITEM_MENU);
    BOOL changed=_bagScene!=scene || !_displayedBagUIState || data.length!=_displayedBagUIState.length
        || (scene && memcmp(data.bytes,_displayedBagUIState.bytes,offsetof(HGNativeBagUIState,pockets)));
    if (scene && !_bagScene) { [_condition lock]; _keys=_pendingKeys=_submittedKeys=0; _touchPressed=NO; [_condition unlock]; [self resetJoystick]; }
    _bagScene=scene; _displayedBagUIState=data; _bagPanel.hidden=!panel;
#ifdef HEARTGOLD_ORIGINAL_APP_CHECK
    _displayedBagItemGlyphs=[pictures[@"itemGlyphs"] isKindOfClass:NSArray.class] ? pictures[@"itemGlyphs"] : nil;
#endif
    for (UIButton *button in _bagNavigation) button.hidden=!panel;
    _bagPage.hidden=!panel || !list;
    if (changed) [self.view setNeedsLayout];
    if (!panel) return;
    [_condition lock]; BOOL waiting=_bagChoicePending || _bagTouchActive; [_condition unlock];
    BOOL ready=state->ready && !paused && !waiting;
    UIColor *selected=[UIColor colorWithRed:0.91 green:0.49 blue:0.12 alpha:1],*border=[UIColor colorWithRed:0.45 green:0.62 blue:0.72 alpha:1];
    NSArray *pockets=pictures[@"pockets"],*items=pictures[@"items"],*actions=pictures[@"actions"];
    for (NSUInteger i=0;i<HG_BAG_UI_POCKETS;i++) {
        UIButton *button=_bagPockets[i]; const HGNativeBagUIPocket *pocket=&state->pockets[i];
        button.hidden=tm || !(state->pocketMask&(1u<<i)); button.enabled=ready && list && pocket->enabled;
        button.layer.borderColor=(i==state->pocketIndex ? selected : border).CGColor;
        button.accessibilityLabel=pocket->pocketId<_pocketNames.count ? _pocketNames[pocket->pocketId] : @"口袋";
        button.accessibilityTraits=UIAccessibilityTraitButton | (i==state->pocketIndex ? UIAccessibilityTraitSelected : 0);
        _bagPocketImages[i].image=i<pockets.count && [pockets[i] isKindOfClass:UIImage.class] ? pockets[i] : nil;
    }
    for (NSUInteger i=0;i<HG_BAG_UI_ITEMS;i++) {
        UIButton *button=_bagItems[i]; const HGNativeBagUIItem *item=&state->items[i];
        button.hidden=!list || !(state->itemMask&(1u<<i)); button.enabled=ready && item->enabled;
        button.layer.borderColor=(state->cursorButton==8+i ? selected : border).CGColor;
        button.accessibilityTraits=UIAccessibilityTraitButton | (state->cursorButton==8+i ? UIAccessibilityTraitSelected : 0);
        _bagItemImages[i].image=i<items.count && [items[i] isKindOfClass:UIImage.class] ? items[i] : nil;
        NSString *name=item->itemId<_itemNames.count ? _itemNames[item->itemId] : @"道具";
        button.accessibilityLabel=[NSString stringWithFormat:@"%@，数量 %u",name,item->quantity];
    }
    for (NSUInteger i=0;i<HG_BAG_UI_ACTIONS;i++) {
        UIButton *button=_bagActions[i]; const HGNativeBagUIAction *action=&state->actions[i];
        button.hidden=list || i>=state->actionCount || (!tm && !(state->actionMask&(1u<<i)));
        uint32_t x,y; button.enabled=ready && action->enabled && (!tm || HGNativeBagUIResolveTouch(state,state->revision,state->phase,HG_BAG_UI_TOUCH_ACTION,(uint32_t)i,&x,&y));
        NSString *name=tm && action->actionId==HG_BAG_UI_ACTION_TM_ACK ? @"继续" : tm && action->actionId==HG_BAG_UI_ACTION_TM_YES ? @"是" : tm && action->actionId==HG_BAG_UI_ACTION_TM_NO ? @"否"
            : action->actionId<_bagActionNames.count && [_bagActionNames[action->actionId] length] ? _bagActionNames[action->actionId] : @"操作";
        UIButtonConfiguration *config=UIButtonConfiguration.filledButtonConfiguration;
        config.baseBackgroundColor=i==state->menuCursor ? [UIColor colorWithRed:1 green:0.81 blue:0.48 alpha:1] : UIColor.whiteColor;
        config.baseForegroundColor=_partyTitle.textColor; config.contentInsets=NSDirectionalEdgeInsetsMake(4,4,4,4);
        UIImage *image=i<actions.count && [actions[i] isKindOfClass:UIImage.class] ? actions[i] : nil;
        if (tm && action->actionId!=HG_BAG_UI_ACTION_TM_ACK && !image) button.enabled=NO;
        OriginalSetButtonContent(button,config,image,name); button.accessibilityLabel=name;
    }
    _bagNavigation[0].hidden=!panel || tm;
    _bagNavigation[0].enabled=ready && state->cancelEnabled;
    _bagNavigation[1].hidden=_bagNavigation[2].hidden=!list;
    _bagNavigation[1].enabled=ready && state->previousEnabled; _bagNavigation[2].enabled=ready && state->nextEnabled;
    _bagPreview.image=[pictures[@"preview"] isKindOfClass:UIImage.class] ? pictures[@"preview"] : nil;
    _bagSelectedEntry.hidden=list; _bagSelectedEntry.image=pictures[@"selected"];
    _bagPrompt.hidden=list; _bagPrompt.image=list ? nil : tm ? pictures[@"message"] : (pictures[@"prompt"] ?: pictures[@"message"]); _bagPage.image=pictures[@"page"];
}
- (NSDictionary *)copySummaryPictures:(const HGNativeSummaryUIState *)state frame:(NSData *)frame subFrame:(NSData *)subFrame {
    BOOL learning=state->active && HGNativeSummaryUIIsLearning(state);
    if (!state->active || (!learning && !HGNativeSummaryUIIsOrdinary(state))) { _summaryOwnerPictures=nil; return nil; }
    BOOL sameView=[_summaryOwnerPictures[@"revision"] unsignedIntValue]==state->revision
        && [_summaryOwnerPictures[@"phase"] unsignedIntValue]==state->phase && [_summaryOwnerPictures[@"page"] unsignedIntValue]==state->page
        && [_summaryOwnerPictures[@"mode"] unsignedIntValue]==state->mode;
    BOOL inputPhase=learning ? state->phase==HG_SUMMARY_UI_FORGET_MOVE || state->phase==HG_SUMMARY_UI_FORGET_CONFIRM
        : state->phase==HG_SUMMARY_UI_INPUT || (state->phase==HG_SUMMARY_UI_MOVE_SELECT && state->page==1);
    if (!state->ready || state->page>2 || !inputPhase) {
        if (!sameView) _summaryOwnerPictures=nil;
        return _summaryOwnerPictures;
    }
    NSMutableDictionary *pictures=[NSMutableDictionary new];
    pictures[@"preview"]=OriginalPixelImage(frame,256,192) ?: NSNull.null;
    BOOL detail=learning || state->phase==HG_SUMMARY_UI_MOVE_SELECT;
    if (detail) pictures[@"content"]=OriginalFrameRegion(subFrame,136,0,120,160) ?: NSNull.null;
    else if (state->page!=1) pictures[@"content"]=OriginalFrameRegion(subFrame,0,0,144,160) ?: NSNull.null;
    NSMutableArray *moves=[NSMutableArray new];
    for (NSUInteger i=0;i<HG_SUMMARY_UI_MOVES;i++) {
        const HGNativeSummaryUIMove *move=&state->moves[i];
        UIImage *image=state->page==1 && (learning ? move->moveId!=0 : (state->moveMask&(1u<<i))) ? OriginalFrameRegion(subFrame,move->cropX,move->cropY,move->cropWidth,move->cropHeight) : nil;
        [moves addObject:image ?: NSNull.null];
    }
    pictures[@"moves"]=[moves copy];
    if (learning) pictures[@"learnAction"]=OriginalFrameRegion(subFrame,8,152,119,31) ?: NSNull.null;
    const HGNativePartyUIWindow *windows[]={&state->nickname,&state->levelText,&state->heldItemText,&state->backText,&state->pageTitle};
    NSArray *keys=@[@"nickname",@"level",@"item",@"back",@"title"];
    for (NSUInteger i=0;i<5;i++) {
        UIImage *image=OriginalWindowImage(windows[i],_env->memory->data,_env->memory->size);
        if (image) pictures[keys[i]]=image;
    }
    pictures[@"revision"]=@(state->revision); pictures[@"phase"]=@(state->phase); pictures[@"page"]=@(state->page); pictures[@"mode"]=@(state->mode);
    _summaryOwnerPictures=[pictures copy]; return _summaryOwnerPictures;
}
- (void)queueSummaryInput:(uint32_t)kind index:(uint32_t)index {
    const HGNativeSummaryUIState *state=_displayedSummaryUIState.bytes; HGNativeSummaryUIInput input;
    if (_displayedSummaryUIState.length!=sizeof(*state) || !HGNativeSummaryUIResolveInput(state,state->revision,state->phase,kind,index,&input)) return;
    [_condition lock];
    if (!_paused && !_stopping && !_summaryChoicePending && !_summaryInputActive) {
        _summaryChoiceRevision=state->revision; _summaryChoicePhase=state->phase; _summaryChoiceKind=kind; _summaryChoiceIndex=index; _summaryChoicePending=YES;
    }
    [_condition unlock];
}
- (void)summaryMovePressed:(UIButton *)button { [self queueSummaryInput:HG_SUMMARY_UI_INPUT_MOVE index:(uint32_t)button.tag]; }
- (void)summaryNavigate:(UIButton *)button {
    const HGNativeSummaryUIState *state=_displayedSummaryUIState.bytes;
    if (_displayedSummaryUIState.length==sizeof(*state) && state->active && HGNativeSummaryUIIsLearning(state)) {
        if (button.tag==0) [self queueSummaryInput:HG_SUMMARY_UI_INPUT_BACK index:0];
        else if (button.tag==1) [self queueSummaryInput:HG_SUMMARY_UI_INPUT_CONFIRM index:0];
        else if (button.tag==5) [self queueSummaryInput:HG_SUMMARY_UI_INPUT_MOVE index:4];
        return;
    }
    const uint32_t kinds[]={HG_SUMMARY_UI_INPUT_BACK,HG_SUMMARY_UI_INPUT_PREVIOUS_PAGE,HG_SUMMARY_UI_INPUT_NEXT_PAGE,
        HG_SUMMARY_UI_INPUT_PREVIOUS_MON,HG_SUMMARY_UI_INPUT_NEXT_MON,HG_SUMMARY_UI_INPUT_RIBBON_DETAILS};
    if (button.tag>=0 && button.tag<6) [self queueSummaryInput:kinds[button.tag] index:0];
}
- (void)takeSummaryInputLocked:(const HGNativeSummaryUIState *)state reads:(uint32_t)reads keys:(uint32_t *)keys x:(uint32_t *)x y:(uint32_t *)y pressed:(BOOL *)pressed {
    BOOL learning=state->active && HGNativeSummaryUIIsLearning(state);
    if (learning) { _keys=_pendingKeys=_submittedKeys=0; _touchPressed=NO; *keys=0; *pressed=NO; }
    BOOL panel=state->active && (learning ? state->phase==HG_SUMMARY_UI_FORGET_MOVE || state->phase==HG_SUMMARY_UI_FORGET_CONFIRM
        : HGNativeSummaryUIIsOrdinary(state) && state->page<=2
            && (state->phase==HG_SUMMARY_UI_INPUT || (state->phase==HG_SUMMARY_UI_MOVE_SELECT && state->page==1)));
    if (!panel) {
        if (_summaryChoicePending || _summaryInputActive) { _keys=_pendingKeys=_submittedKeys=0; _touchPressed=NO; *keys=0; *pressed=NO; }
        _summaryChoicePending=_summaryInputActive=_summaryTouchPressed=NO; _summaryInputKeys=0; return;
    }
    *keys=0; *pressed=NO;
    if (_summaryChoicePending) {
        if (state->revision!=_summaryChoiceRevision || state->phase!=_summaryChoicePhase) _summaryChoicePending=NO;
        else if (state->ready) {
            HGNativeSummaryUIInput input;
            if (HGNativeSummaryUIResolveInput(state,_summaryChoiceRevision,_summaryChoicePhase,_summaryChoiceKind,_summaryChoiceIndex,&input)) {
                _summaryInputActive=YES; _summaryInputRevision=state->revision; _summaryInputPhase=state->phase; _summaryInputReads=reads;
                _summaryInputKeys=input.keys; _summaryTouchPressed=input.touchPressed!=0; _summaryTouchX=input.touchX; _summaryTouchY=input.touchY;
            }
            _summaryChoicePending=NO;
        }
    }
    if (_summaryInputActive) {
        BOOL release=state->revision!=_summaryInputRevision || state->phase!=_summaryInputPhase || reads-_summaryInputReads>=4;
        if (release) {
            _summaryInputActive=_summaryTouchPressed=NO; _summaryInputKeys=0;
            _keys=_pendingKeys=_submittedKeys=0; _touchPressed=NO;
        } else { *keys=_summaryInputKeys; *x=_summaryTouchX; *y=_summaryTouchY; *pressed=_summaryTouchPressed; }
    }
}
- (void)updateSummaryControls:(NSData *)data pictures:(NSDictionary *)pictures paused:(BOOL)paused {
    const HGNativeSummaryUIState *state=data.bytes;
    BOOL learning=data.length==sizeof(*state) && state->active && HGNativeSummaryUIIsLearning(state);
    BOOL scene=data.length==sizeof(*state) && state->active && (learning || HGNativeSummaryUIIsOrdinary(state));
    BOOL panel=scene && (learning ? state->phase==HG_SUMMARY_UI_FORGET_MOVE || state->phase==HG_SUMMARY_UI_FORGET_CONFIRM
        : state->page<=2 && (state->phase==HG_SUMMARY_UI_INPUT || (state->phase==HG_SUMMARY_UI_MOVE_SELECT && state->page==1)));
    BOOL changed=_summaryScene!=scene || !_displayedSummaryUIState || data.length!=_displayedSummaryUIState.length
        || (scene && memcmp(data.bytes,_displayedSummaryUIState.bytes,offsetof(HGNativeSummaryUIState,moves)));
    if (scene && !_summaryScene) { [_condition lock]; _keys=_pendingKeys=_submittedKeys=0; _touchPressed=NO; [_condition unlock]; [self resetJoystick]; }
    _summaryScene=scene; _summaryMovePanel=panel && state->page==1; _summaryMoveDetail=panel && (learning || state->phase==HG_SUMMARY_UI_MOVE_SELECT);
    _displayedSummaryUIState=data; _summaryPanel.hidden=!panel;
    for (UIButton *button in _summaryNavigation) button.hidden=!panel;
    if (!learning && (_summaryNavigation[1].configuration || _summaryNavigation[5].configuration)) [UIView performWithoutAnimation:^{
        self->_summaryNavigation[1].configuration=nil; self->_summaryNavigation[5].configuration=nil;
        [self->_summaryNavigation[1] setTitle:@"上一页" forState:UIControlStateNormal]; [self->_summaryNavigation[5] setTitle:@"查看缎带" forState:UIControlStateNormal];
        self->_summaryNavigation[5].backgroundColor=self->_partyBack.backgroundColor; self->_summaryNavigation[5].layer.borderWidth=0; self->_summaryNavigation[5].layer.cornerRadius=12;
        [self->_summaryNavigation[1] layoutIfNeeded]; [self->_summaryNavigation[5] layoutIfNeeded];
    }];
    if (changed) [self.view setNeedsLayout];
    if (!panel) return;
    [_condition lock]; BOOL waiting=_summaryChoicePending || _summaryInputActive; [_condition unlock];
    BOOL ready=state->ready && !paused && !waiting;
    _summaryPreview.image=[pictures[@"preview"] isKindOfClass:UIImage.class] ? pictures[@"preview"] : nil;
    _summaryContent.image=[pictures[@"content"] isKindOfClass:UIImage.class] ? pictures[@"content"] : nil;
    _summaryContent.hidden=_summaryMovePanel && !_summaryMoveDetail; _summaryTitle.image=pictures[@"title"];
    NSArray *statusNames=@[@"宝可病毒",@"麻痹",@"冰冻",@"睡眠",@"中毒",@"灼伤",@"濒死",@""];
    NSString *species=state->species<_speciesNames.count ? _speciesNames[state->species] : @"宝可梦";
    NSString *status=state->statusIcon<statusNames.count ? statusNames[state->statusIcon] : @"";
    _summaryPreview.isAccessibilityElement=YES;
    _summaryPreview.accessibilityLabel=state->isEgg ? @"蛋"
        : [NSString stringWithFormat:@"%@，Lv.%u，HP %u/%u，%@%@",species,state->level,state->hp,state->maxHp,status,state->heldItem ? @"，携带道具" : @""];
    _summaryContent.isAccessibilityElement=YES;
    _summaryContent.accessibilityLabel=_summaryMoveDetail ? @"原版招式详情" : state->page==2 ? [NSString stringWithFormat:@"原版缎带页面，%u 个缎带",state->ribbonCount] : @"原版宝可梦资料";
    NSArray *moves=pictures[@"moves"];
    for (NSUInteger i=0;i<HG_SUMMARY_UI_MOVES;i++) {
        const HGNativeSummaryUIMove *move=&state->moves[i]; UIButton *button=_summaryMoves[i]; HGNativeSummaryUIInput input;
        button.hidden=!_summaryMovePanel || (learning ? !move->moveId : !(state->moveMask&(1u<<i)));
        button.enabled=ready && HGNativeSummaryUIResolveInput(state,state->revision,state->phase,HG_SUMMARY_UI_INPUT_MOVE,(uint32_t)i,&input);
        button.layer.borderColor=(i==state->moveSlot && _summaryMoveDetail ? [UIColor colorWithRed:0.91 green:0.49 blue:0.12 alpha:1] : [UIColor colorWithRed:0.45 green:0.62 blue:0.72 alpha:1]).CGColor;
        button.accessibilityTraits=UIAccessibilityTraitButton | (i==state->moveSlot && _summaryMoveDetail ? UIAccessibilityTraitSelected : 0);
        NSString *name=move->moveId<_moveNames.count ? _moveNames[move->moveId] : @"招式";
        button.accessibilityLabel=[NSString stringWithFormat:@"%@，PP %u/%u",name,move->pp,move->maxPP];
        button.accessibilityHint=@"查看原版招式详情";
        _summaryMoveImages[i].image=i<moves.count && [moves[i] isKindOfClass:UIImage.class] ? moves[i] : nil;
    }
    const uint32_t kinds[]={HG_SUMMARY_UI_INPUT_BACK,HG_SUMMARY_UI_INPUT_PREVIOUS_PAGE,HG_SUMMARY_UI_INPUT_NEXT_PAGE,
        HG_SUMMARY_UI_INPUT_PREVIOUS_MON,HG_SUMMARY_UI_INPUT_NEXT_MON,HG_SUMMARY_UI_INPUT_RIBBON_DETAILS};
    for (NSUInteger i=0;i<6;i++) {
        UIButton *button=_summaryNavigation[i]; HGNativeSummaryUIInput input;
        if (learning) {
            uint32_t kind=i==0 ? HG_SUMMARY_UI_INPUT_BACK : i==1 ? HG_SUMMARY_UI_INPUT_CONFIRM : HG_SUMMARY_UI_INPUT_MOVE;
            BOOL allowed=(i==0 || i==1 || i==5) && HGNativeSummaryUIResolveInput(state,state->revision,state->phase,kind,i==5 ? 4 : 0,&input);
            button.enabled=ready && allowed;
            button.hidden=i==1 ? !allowed : i==5 ? state->phase!=HG_SUMMARY_UI_FORGET_MOVE : i!=0;
        } else {
            button.enabled=ready && HGNativeSummaryUIResolveInput(state,state->revision,state->phase,kinds[i],0,&input);
            button.hidden=i ? _summaryMoveDetail || (i==5 && (state->page!=2 || !state->ribbonCount)) : NO;
            button.accessibilityLabel=(@[@"返回",@"上一页",@"下一页",@"上一只",@"下一只",@"查看缎带"])[i];
            button.accessibilityHint=nil; button.accessibilityTraits=UIAccessibilityTraitButton;
        }
    }
    UIButtonConfiguration *back=UIButtonConfiguration.filledButtonConfiguration;
    back.baseBackgroundColor=_partyBack.backgroundColor; back.baseForegroundColor=UIColor.whiteColor;
    OriginalSetButtonContent(_summaryNavigation[0],back,pictures[@"back"],@"返回");
    if (learning) {
        UIButtonConfiguration *confirm=[back copy],*candidate=[back copy];
        // The fifth card selects only; the existing generic confirmation caption sends typed A in phase 8.
        OriginalSetButtonContent(_summaryNavigation[1],confirm,state->phase==HG_SUMMARY_UI_FORGET_CONFIRM ? pictures[@"learnAction"] : nil,@"确认");
        NSString *name=state->moveToLearn<_moveNames.count ? _moveNames[state->moveToLearn] : @"招式";
        OriginalSetButtonContent(_summaryNavigation[5],candidate,pictures[@"learnAction"],name);
        _summaryNavigation[1].accessibilityLabel=@"确认"; _summaryNavigation[5].accessibilityLabel=name;
        _summaryNavigation[5].accessibilityHint=@"查看原版招式详情";
        _summaryNavigation[5].accessibilityTraits=UIAccessibilityTraitButton | (state->moveSlot==4 ? UIAccessibilityTraitSelected : 0);
        _summaryNavigation[5].layer.borderWidth=2; _summaryNavigation[5].layer.cornerRadius=8;
        _summaryNavigation[5].layer.borderColor=(state->moveSlot==4 ? [UIColor colorWithRed:0.91 green:0.49 blue:0.12 alpha:1] : [UIColor colorWithRed:0.45 green:0.62 blue:0.72 alpha:1]).CGColor;
    }
}
- (void)updateBattleControls:(NSData *)data {
    if (data.length != sizeof(HGNativeBattleUIState)) return;
    const HGNativeBattleUIState *state = data.bytes;
    if (state->mode > HG_BATTLE_UI_YES_NO) return;
    BOOL scene=state->active && (state->mode || !_displayedEngine);
    BOOL changed = !_displayedBattleUIState || memcmp(_displayedBattleUIState.bytes, data.bytes, offsetof(HGNativeBattleUIState, namePixels));
    _displayedBattleUIState = data;
    if (_battleMode != state->mode || _battleScene != scene) {
        _battleMode = state->mode;
        _battleScene = scene;
        if (_battleScene) {
            [_condition lock]; _keys = _pendingKeys = _submittedKeys = 0; _touchPressed = NO; [_condition unlock];
            [self resetJoystick];
        }
        _battleBar.hidden = !_battleScene;
        for (UIButton *button in _buttons.allValues) button.hidden = _battleMode != 0;
        _menuButton.hidden = _battleMode != 0;
        [self.view setNeedsLayout];
    }
    NSArray *commands = @[@"战斗", @"背包", @"宝可梦", state->cancelRun ? @"返回" : @"脱战"];
    NSArray *targetRoles = @[@"对方左侧",@"对方右侧",@"我方左侧",@"我方右侧"];
    NSArray *nextMonChoices = @[@"使用下一只宝可梦",@"逃跑"];
    uint32_t displayMode=state->mode ? state->mode : HG_BATTLE_UI_COMMAND;
    BOOL nextMon=displayMode==HG_BATTLE_UI_NEXT_MON;
    BOOL yesNo=displayMode==HG_BATTLE_UI_YES_NO;
    BOOL choice=OriginalBattleChoiceMode(displayMode);
    NSArray *choices=yesNo ? @[@"是",@"否"] : nextMon ? nextMonChoices : displayMode==HG_BATTLE_UI_KEEP_FORGET ? @[@"忘记一个招式",@"保留原有招式"] : @[@"放弃学习",@"不放弃学习"];
    _battleBar.accessibilityLabel=yesNo ? @"是或否" : nextMon ? @"是否使用下一只宝可梦" : displayMode==HG_BATTLE_UI_KEEP_FORGET ? @"是否遗忘招式以学习新招式"
        : displayMode==HG_BATTLE_UI_GIVE_UP ? @"是否放弃学习新招式" : displayMode==HG_BATTLE_UI_TARGETS ? @"选择招式目标" : @"战斗操作";
    for (NSUInteger i = 0; i < 6; i++) {
        UIButton *button = i < 4 ? _battleButtons[i] : i == 4 ? _battleBack : _battleBall;
        button.hidden = choice ? i >= 2 : (i == 4 && displayMode == HG_BATTLE_UI_COMMAND) || (i == 5 && displayMode != HG_BATTLE_UI_COMMAND);
        button.enabled = state->ready && (state->enabledMask & (1u << i));
        button.alpha = button.enabled ? 1 : 0.5;
        if (!changed) continue;
        UIButtonConfiguration *config = UIButtonConfiguration.filledButtonConfiguration;
        config.contentInsets = NSDirectionalEdgeInsetsMake(4, 8, 4, 8);
        config.baseBackgroundColor = [UIColor colorWithRed:0.22 green:0.29 blue:0.31 alpha:1];
        config.baseForegroundColor = UIColor.whiteColor;
        config.titleAlignment = UIButtonConfigurationTitleAlignmentCenter;
        button.accessibilityHint=nil;
        if (i == 5) { config.title = @"精灵球"; config.cornerStyle = UIButtonConfigurationCornerStyleCapsule; button.accessibilityHint = @"打开精灵球口袋"; }
        else if (i == 4) { config.title = @"返回"; if (displayMode==HG_BATTLE_UI_TARGETS) button.accessibilityHint=@"返回招式选择"; }
        else if (displayMode == HG_BATTLE_UI_COMMAND) config.title = commands[i];
        else if (choice ? i<2 : (displayMode==HG_BATTLE_UI_TARGETS ? state->targetSpecies[i] : state->move[i])) {
            NSString *name;
            if (choice) {
                name=choices[i]; config.title=name; button.accessibilityLabel=name;
                button.accessibilityHint=yesNo ? (i ? @"选择否" : @"选择是") : nextMon ? (i ? @"按原版规则尝试逃跑" : @"选择可继续战斗的宝可梦")
                    : displayMode==HG_BATTLE_UI_KEEP_FORGET ? (i ? @"保留原有招式，进入原版放弃学习确认" : @"选择要遗忘的招式")
                    : (i ? @"返回原版招式学习流程" : @"按原版流程放弃本次学习");
            } else if (displayMode==HG_BATTLE_UI_TARGETS) {
                uint32_t species=state->targetSpecies[i];
                name=species<_speciesNames.count ? _speciesNames[species] : [NSString stringWithFormat:@"宝可梦 %u",species];
                config.title=[NSString stringWithFormat:@"HP %u/%u",state->targetHp[i],state->targetMaxHp[i]];
                button.accessibilityLabel=[NSString stringWithFormat:@"%@，%@，%@",targetRoles[i],name,config.title];
                unsigned targets=__builtin_popcount(state->targetMask[i]);
                button.accessibilityHint=targets>1 ? [NSString stringWithFormat:@"本招式影响 %u 个目标",targets] : @"选择此目标";
            } else {
                name=state->move[i]<_moveNames.count ? _moveNames[state->move[i]] : [NSString stringWithFormat:@"招式 %u",state->move[i]];
                config.title=[NSString stringWithFormat:@"PP %u/%u",state->pp[i],state->maxPP[i]];
                button.accessibilityLabel=[NSString stringWithFormat:@"%@，%@",name,config.title];
            }
            uint32_t width = state->nameWidth[i];
            if (width && width <= HG_BATTLE_NAME_WIDTH) {
                NSData *pixels = [NSData dataWithBytes:state->namePixels[i] length:sizeof(state->namePixels[i])];
                CGDataProviderRef provider = CGDataProviderCreateWithCFData((__bridge CFDataRef)pixels);
                CGColorSpaceRef color = CGColorSpaceCreateDeviceRGB();
                CGImageRef image = CGImageCreate(width, HG_BATTLE_NAME_HEIGHT, 8, 32, HG_BATTLE_NAME_WIDTH * 4, color, kCGBitmapByteOrder32Big | kCGImageAlphaLast, provider, NULL, NO, kCGRenderingIntentDefault);
                config.image = [UIImage imageWithCGImage:image scale:0.75 orientation:UIImageOrientationUp];
                config.imagePlacement = NSDirectionalRectEdgeTop; config.imagePadding = 2;
                if (choice) config.title=nil;
                CGImageRelease(image); CGColorSpaceRelease(color); CGDataProviderRelease(provider);
            } else if (!choice) config.title = [NSString stringWithFormat:@"%@ · %@", name, config.title];
        } else config.title = @"—";
        if (i >= 4 || displayMode == HG_BATTLE_UI_COMMAND || (!choice && (displayMode==HG_BATTLE_UI_TARGETS ? !state->targetSpecies[i] : !state->move[i]))) button.accessibilityLabel = config.title;
        button.configuration = config;
    }
}
- (void)clearStartMenuInputLocked {
    if (_startMenuInputActive) _startMenuReleasePending=YES;
    _startMenuChoicePending=_startMenuInputActive=NO;
    _startMenuChoiceKeys=_startMenuInputKeys=0;
}
- (void)queueStartMenuInputKeys:(uint32_t)keys touch:(BOOL)touch point:(CGPoint)point {
    const HGNativeFieldMenuUIState *shown=_displayedStartMenuUIState.length==sizeof(*shown) ? _displayedStartMenuUIState.bytes : NULL;
    if (!_startMenuScene || !_startMenuBlocksInput || !shown || !isfinite(point.x) || !isfinite(point.y)
        || point.x<0 || point.x>=256 || point.y<0 || point.y>=192) return;
    uint32_t resolved=0;
    if (touch ? !HGNativeFieldMenuUITouchAllowed(shown,shown->ownerGeneration,shown->revision,point.x,point.y)
        : !HGNativeFieldMenuUIResolveKey(shown,shown->ownerGeneration,shown->revision,keys,&resolved)) return;
    [_condition lock];
    const HGNativeFieldMenuUIState *live=_startMenuUIState.length==sizeof(*live) ? _startMenuUIState.bytes : NULL;
    if (!_paused && !_stopping && !_failure && !_startMenuChoicePending && !_startMenuInputActive && !_startMenuInputCooling
        && !_startMenuReleasePending && _touchEngine==_displayedStartMenuEngine
        && (touch ? HGNativeFieldMenuUITouchAllowed(live,shown->ownerGeneration,shown->revision,point.x,point.y)
            : HGNativeFieldMenuUIResolveKey(live,shown->ownerGeneration,shown->revision,keys,&resolved))) {
        _startMenuChoice=*shown;_startMenuChoiceKeys=resolved;_startMenuChoiceTouch=touch;
        _startMenuChoiceX=point.x;_startMenuChoiceY=point.y;_startMenuChoiceEngine=_touchEngine;
        _startMenuChoicePending=YES;_inputGeneration++;
    }
    [_condition unlock];
}
- (void)takeStartMenuInputLocked:(const HGNativeFieldMenuUIState *)state blocked:(BOOL)blocked reads:(uint32_t)reads
    keys:(uint32_t *)keys x:(uint32_t *)x y:(uint32_t *)y pressed:(BOOL *)pressed {
    *keys=*x=*y=0;*pressed=NO;
    if (_paused || _stopping || _failure || blocked) [self clearStartMenuInputLocked];
    if (_startMenuReleasePending) {
        _startMenuReleasePending=NO;_startMenuInputCooling=YES;_startMenuReleaseReads=reads;return;
    }
    if (_startMenuInputCooling && reads-_startMenuReleaseReads>=2) _startMenuInputCooling=NO;
    if (_startMenuInputActive && (reads-_startMenuInputReads>=4 || _touchEngine!=_startMenuInputEngine
        || !HGNativeFieldMenuUIInputAllowed(state,_startMenuInput.ownerGeneration,_startMenuInput.revision))) {
        _startMenuInputActive=NO;_startMenuInputCooling=YES;_startMenuReleaseReads=reads;
    }
    if (_startMenuChoicePending) {
        uint32_t resolved=0;
        if (!_startMenuInputCooling && !blocked && _touchEngine==_startMenuChoiceEngine
            && (_startMenuChoiceTouch ? HGNativeFieldMenuUITouchAllowed(state,_startMenuChoice.ownerGeneration,_startMenuChoice.revision,_startMenuChoiceX,_startMenuChoiceY)
                : HGNativeFieldMenuUIResolveKey(state,_startMenuChoice.ownerGeneration,_startMenuChoice.revision,_startMenuChoiceKeys,&resolved))) {
            _startMenuInput=_startMenuChoice;_startMenuInputKeys=resolved;_startMenuInputTouch=_startMenuChoiceTouch;
            _startMenuInputX=_startMenuChoiceX;_startMenuInputY=_startMenuChoiceY;_startMenuInputEngine=_startMenuChoiceEngine;
            _startMenuInputReads=reads;_startMenuInputActive=YES;
        }
        _startMenuChoicePending=NO;
    }
    if (_startMenuInputActive) {*keys=_startMenuInputKeys;*x=_startMenuInputX;*y=_startMenuInputY;*pressed=_startMenuInputTouch;}
}
- (void)updateStartMenuControls:(NSData *)data main:(NSData *)main sub:(NSData *)sub engine:(uint32_t)engine blocked:(BOOL)blocked paused:(BOOL)paused {
    const HGNativeFieldMenuUIState *state=data.length==sizeof(*state) ? data.bytes : NULL;
    BOOL scene=state && state->active==1 && !state->childActive && !blocked,was=_startMenuScene;
    BOOL paired=main.length==256*192*4 && sub.length==256*192*4 && engine<2;
    const HGNativeFieldMenuUIState *old=_presentedStartMenuUIState.length==sizeof(*old) ? _presentedStartMenuUIState.bytes : NULL;
    BOOL changed=_presentedStartMenuUIState.length!=data.length || (data.length && memcmp(_presentedStartMenuUIState.bytes,data.bytes,data.length));
    if (!scene || changed) {
        [_condition lock];[self clearStartMenuInputLocked];[_condition unlock];
    }
    if (was!=scene || (scene && engine!=_displayedStartMenuEngine)
        || OriginalStartMenuHandoff(old,state,_startMenuBlocksInput,scene && OriginalStartMenuBlocksInput(state),changed)) {
        for (UIGestureRecognizer *gesture in _fieldMenu.gestureRecognizers) {gesture.enabled=NO;gesture.enabled=YES;}
        for (UIGestureRecognizer *gesture in _canvas.gestureRecognizers) {gesture.enabled=NO;gesture.enabled=YES;}
    }
    _presentedStartMenuUIState=data;
    [_condition lock];_startMenuScene=scene;_startMenuBlocksInput=scene && OriginalStartMenuBlocksInput(state);
    _displayedStartMenuUIState=scene && paired ? data : nil;_displayedStartMenuEngine=engine;[_condition unlock];
    if (was!=scene) [self.view setNeedsLayout];
    if (!scene) {
        if (was) {_fieldMenuShade.userInteractionEnabled=YES;_menuButton.enabled=YES;}
        if (was) for (NSNumber *key in @[@64,@128,@32,@16,@1,@2,@8,@1024]) {
            UIButton *button=_buttons[key];button.configuration=nil;button.enabled=YES;button.alpha=1;button.accessibilityHint=nil;
            [button setTitle:[key isEqual:@1] ? @"确认" : [key isEqual:@2] ? @"返回" : [key isEqual:@8] ? @"开始" : button.currentTitle forState:UIControlStateNormal];
            button.accessibilityLabel=button.currentTitle;
        }
        return;
    }
    BOOL view=OriginalStartMenuHasView(state),fallback=OriginalStartMenuFallback(state);
    if (view && paired) {_canvas.image=OriginalPixelImage(engine ? main : sub,256,192);_canvas.hidden=NO;_displayedEngine=1-engine;}
    _fieldMenu.image=paired ? OriginalPixelImage(engine ? sub : main,256,192) : nil;
    _fieldMenu.hidden=_fieldMenuShade.hidden=!view || !_fieldMenu.image;
    _fieldMenu.userInteractionEnabled=!paused && paired && (fallback || HGNativeFieldMenuUIInputAllowed(state,state->ownerGeneration,state->revision));
    _canvas.userInteractionEnabled=NO;_joystick.hidden=YES;
    [_condition lock];BOOL idle=!_stopping && !_failure && !_startMenuChoicePending && !_startMenuInputActive && !_startMenuInputCooling && !_startMenuReleasePending;
    BOOL resume=_interrupted && _interruptionEnded && !_inactive;[_condition unlock];
    for (NSNumber *key in @[@64,@128,@32,@16,@1,@2,@1024]) {
        UIButton *button=_buttons[key];uint32_t resolved=0;
        BOOL allowed=HGNativeFieldMenuUIResolveKey(state,state->ownerGeneration,state->revision,key.unsignedIntValue,&resolved);
        button.hidden=!view || (fallback ? [key isEqual:@1024] : !allowed);
        button.enabled=!paused && paired && idle && (fallback || allowed);button.alpha=button.enabled ? 1 : 0.5;
        if ([key isEqual:@1] || [key isEqual:@2]) [button setTitle:[key isEqual:@1] ? @"确认" : @"返回" forState:UIControlStateNormal];
        button.accessibilityLabel=button.currentTitle;
        if (!button.hidden) [self.view bringSubviewToFront:button];
    }
    _menuButton.hidden=!view || fallback;_menuButton.enabled=!paused && paired && idle && state->ready;
    _fieldMenuShade.userInteractionEnabled=_menuButton.enabled;
    OriginalSetButtonContent(_menuButton,nil,nil,@"关闭菜单");
    _menuButton.accessibilityLabel=@"关闭菜单";
    if (!_menuButton.hidden) [self.view bringSubviewToFront:_menuButton];
    _buttons[@8].hidden=!resume;_buttons[@8].enabled=resume;
    if (resume) {[_buttons[@8] setTitle:@"继续" forState:UIControlStateNormal];_buttons[@8].accessibilityLabel=@"继续";[self.view bringSubviewToFront:_buttons[@8]];}
}
- (void)setupTrainerCardControls {
    _trainerCardPanel=[UIView new]; _trainerCardPanel.hidden=YES; _trainerCardPanel.clipsToBounds=YES;
    [self.view addSubview:_trainerCardPanel];
    _trainerCardBody=[UIImageView new]; _trainerCardFooter=[UIImageView new]; _trainerCardBadges=[UIImageView new];
    for (UIImageView *image in @[_trainerCardBody,_trainerCardFooter,_trainerCardBadges]) {
        image.contentMode=UIViewContentModeScaleToFill; image.layer.magnificationFilter=kCAFilterNearest;
        image.isAccessibilityElement=YES; [_trainerCardPanel addSubview:image];
    }
    _trainerCardBody.accessibilityLabel=@"训练家卡片"; _trainerCardFooter.accessibilityLabel=@"卡片操作";
    _trainerCardBadges.accessibilityLabel=@"徽章盒";
    _trainerCardSurface=[OriginalJoystick new]; _trainerCardSurface.backgroundColor=UIColor.clearColor;
    [_trainerCardSurface addTarget:self action:@selector(trainerCardSurfaceChanged:) forControlEvents:UIControlEventValueChanged];
    [_trainerCardPanel addSubview:_trainerCardSurface];
    _trainerCardDone=[OriginalKeyButton new]; _trainerCardDone.backgroundColor=UIColor.clearColor;
    _trainerCardDone.accessibilityLabel=@"完成签名"; _trainerCardDone.accessibilityTraits=UIAccessibilityTraitButton;
    [_trainerCardDone addTarget:self action:@selector(trainerCardDoneDown:) forControlEvents:UIControlEventTouchDown];
    [_trainerCardDone addTarget:self action:@selector(trainerCardDoneUp:) forControlEvents:UIControlEventTouchUpInside|UIControlEventTouchUpOutside|UIControlEventTouchCancel];
    [_trainerCardPanel addSubview:_trainerCardDone];
    _trainerCardTouches=[NSMutableArray new];
}
- (void)layoutTrainerCardControls:(CGRect)area {
    if (!_trainerCardScene) return;
    const HGNativeTrainerCardUIState *state=_displayedTrainerCardUIState.length==sizeof(*state) ? _displayedTrainerCardUIState.bytes : NULL;
    BOOL full=state && (state->view==HG_TRAINER_CARD_SIGNATURE || state->phase==4);
    CGFloat rail=MIN(160,MAX(104,area.size.width*0.18)),gap=12,pw=MAX(1,area.size.width-rail-gap),ph=MAX(1,area.size.height);
    _trainerCardPanel.frame=CGRectMake(area.origin.x,area.origin.y,pw,ph);
    CGFloat cardWidth=MIN(full ? pw : pw*0.6,ph*256/192),cardHeight=cardWidth*192/256;
    CGFloat left=full ? (pw-cardWidth)/2 : 0,top=(ph-cardHeight)/2;
    _trainerCardBody.frame=CGRectMake(left,top,cardWidth,cardWidth*(full ? 192 : 168)/256);
    _trainerCardFooter.frame=CGRectMake(left,CGRectGetMaxY(_trainerCardBody.frame),cardWidth,cardWidth*24/256);
    _trainerCardSurface.frame=CGRectMake(left,top,cardWidth,cardHeight);
    CGFloat bx=cardWidth+gap,bw=MAX(1,pw-bx),bh=MIN(ph,bw*192/256);
    _trainerCardBadges.frame=CGRectMake(bx,(ph-bh)/2,bw,bh);
    CGFloat dw=MAX(44,cardWidth*52/256),dh=MAX(44,cardHeight*26/192);
    _trainerCardDone.frame=CGRectMake(MIN(pw-dw,left+cardWidth*229/256-dw/2),MIN(ph-dh,top+cardHeight*179/192-dh/2),dw,dh);
    CGFloat x=CGRectGetMaxX(area)-rail;
    NSArray *keys=@[@64,@128,@16,@32,@1,@2];
    for (NSUInteger i=0;i<keys.count;i++) _buttons[keys[i]].frame=CGRectMake(x,area.origin.y+i*56,rail,48);
    if (state && state->view==HG_TRAINER_CARD_VIEWER && state->phase==1) {
        _buttons[@32].frame=CGRectMake(x,area.origin.y,(rail-8)/2,48);
        _buttons[@16].frame=CGRectMake(x+(rail+8)/2,area.origin.y,(rail-8)/2,48);
        _buttons[@1].frame=CGRectMake(x,area.origin.y+60,rail,48);
        _buttons[@2].frame=CGRectMake(x,area.origin.y+120,rail,48);
    } else {
        for (NSUInteger i=0;i<4;i++) _buttons[(@[@64,@128,@1,@2])[i]].frame=CGRectMake(x,area.origin.y+i*56,rail,48);
    }
    _status.frame=CGRectInset(_trainerCardPanel.frame,8,8);
}
- (void)clearTrainerCardInputLocked {
    if (_trainerCardRawDown || _trainerCardTouchLease.down) _trainerCardReleasePending=YES;
    _trainerCardChoicePending=_trainerCardInputActive=_trainerCardInputCooling=_trainerCardTracking=NO;
    _trainerCardChoiceKeys=_trainerCardInputKeys=_trainerCardInputReads=_trainerCardReleaseReads=0;
    _trainerCardTouchLease=(HGNativeTrainerCardUITouchLease){0};
    _trainerCardTouchSource=nil;
    // Keep physical surface tracking until UP/cancel so a fenced MOVE cannot become a new DOWN.
    [_trainerCardTouches removeAllObjects];
}
- (void)queueTrainerCardKey:(OriginalKeyButton *)button {
    const HGNativeTrainerCardUIState *shown=_displayedTrainerCardUIState.length==sizeof(*shown) ? _displayedTrainerCardUIState.bytes : NULL;
    uint32_t key=(uint32_t)button.tag,resolved;
    if (!_trainerCardScene || button.hidden || !button.enabled || !shown
        || !HGNativeTrainerCardUIResolveKey(shown,shown->ownerGeneration,shown->revision,key,&resolved)) return;
    [_condition lock];
    const HGNativeTrainerCardUIState *live=_trainerCardUIState.length==sizeof(*live) ? _trainerCardUIState.bytes : NULL;
    if (!_paused && !_stopping && !_failure && !_trainerCardChoicePending && !_trainerCardInputActive && !_trainerCardInputCooling
        && !_trainerCardTracking && !_trainerCardRawDown && !_trainerCardReleasePending && !_trainerCardTouchCooling && !_trainerCardTouches.count
        && HGNativeTrainerCardUIResolveKey(live,shown->ownerGeneration,shown->revision,key,&resolved)) {
        _trainerCardChoice=*shown;_trainerCardChoiceKeys=resolved;_trainerCardChoicePending=YES;_inputGeneration++;
    }
    [_condition unlock];
}
- (void)queueTrainerCardTouch:(uint32_t)event point:(CGPoint)point source:(id)source {
    if (!source || !isfinite(point.x) || !isfinite(point.y) || point.x<0 || point.x>=256 || point.y<0 || point.y>=192) return;
    const HGNativeTrainerCardUIState *shown=_displayedTrainerCardUIState.length==sizeof(*shown) ? _displayedTrainerCardUIState.bytes : NULL;
    [_condition lock];
    const HGNativeTrainerCardUIState *live=_trainerCardUIState.length==sizeof(*live) ? _trainerCardUIState.bytes : NULL;
    if (event==HG_TRAINER_CARD_TOUCH_DOWN) {
        if (!_trainerCardScene || _paused || _stopping || _failure || !shown || _trainerCardTracking || _trainerCardRawDown
            || _trainerCardReleasePending || _trainerCardTouchCooling || _trainerCardTouches.count
            || _trainerCardChoicePending || _trainerCardInputActive || _trainerCardInputCooling
            || !HGNativeTrainerCardUITouchStartAllowed(shown,shown->ownerGeneration,shown->revision,(uint32_t)point.x,(uint32_t)point.y)
            || !HGNativeTrainerCardUITouchStartAllowed(live,shown->ownerGeneration,shown->revision,(uint32_t)point.x,(uint32_t)point.y)) {
            [_condition unlock];return;
        }
        _trainerCardGesture=*shown;_trainerCardTracking=YES;_trainerCardTouchSource=source;
    } else if (!_trainerCardTracking || _trainerCardTouchSource!=source) {[_condition unlock];return;}
    OriginalTrainerTouchEvent value={_trainerCardGesture.ownerGeneration,_trainerCardGesture.revision,event,(uint32_t)point.x,(uint32_t)point.y};
    NSData *data=[NSData dataWithBytes:&value length:sizeof(value)];
    if (!_trainerCardTouches) _trainerCardTouches=[NSMutableArray new];
    OriginalTrainerTouchEvent previous={0}; if (_trainerCardTouches.lastObject.length==sizeof(previous)) memcpy(&previous,_trainerCardTouches.lastObject.bytes,sizeof(previous));
    // The original sampler reads the latest physical position; never coalesce a down or up boundary.
    if (event==HG_TRAINER_CARD_TOUCH_MOVE && previous.event==HG_TRAINER_CARD_TOUCH_MOVE) [_trainerCardTouches removeLastObject];
    [_trainerCardTouches addObject:data];
    if (event==HG_TRAINER_CARD_TOUCH_UP) {_trainerCardTracking=NO;_trainerCardTouchSource=nil;}
    [_condition unlock];
}
- (void)trainerCardSurfaceChanged:(OriginalJoystick *)surface {
    BOOL wasDown=_trainerCardSurfaceDown;_trainerCardSurfaceDown=surface.pressed;
    if (!surface.pressed) {[self queueTrainerCardTouch:HG_TRAINER_CARD_TOUCH_UP point:CGPointZero source:surface];return;}
    CGPoint p=surface.location;CGRect b=surface.bounds;
    if (b.size.width<=0 || b.size.height<=0 || !CGRectContainsPoint(b,p)) return;
    CGPoint original=CGPointMake((p.x-b.origin.x)*256/b.size.width,(p.y-b.origin.y)*192/b.size.height);
    [self queueTrainerCardTouch:wasDown ? HG_TRAINER_CARD_TOUCH_MOVE : HG_TRAINER_CARD_TOUCH_DOWN point:original source:surface];
}
- (void)trainerCardDoneDown:(OriginalKeyButton *)button { [self queueTrainerCardTouch:HG_TRAINER_CARD_TOUCH_DOWN point:CGPointMake(229,179) source:button]; }
- (void)trainerCardDoneUp:(OriginalKeyButton *)button { [self queueTrainerCardTouch:HG_TRAINER_CARD_TOUCH_UP point:CGPointZero source:button]; }
- (void)takeTrainerCardInputLocked:(const HGNativeTrainerCardUIState *)state reads:(uint32_t)reads
    keys:(uint32_t *)keys x:(uint32_t *)x y:(uint32_t *)y pressed:(BOOL *)pressed {
    *keys=*x=*y=0;*pressed=NO;
    BOOL wasDown=_trainerCardTouchLease.down || _trainerCardRawDown;
    HGNativeTrainerCardUIFenceTouch(&_trainerCardTouchLease,state);
    if (_paused || _stopping || _failure) [self clearTrainerCardInputLocked];
    if (_trainerCardReleasePending || (wasDown && !_trainerCardTouchLease.down)) {
        _trainerCardReleasePending=NO;_trainerCardRawDown=_trainerCardTracking=NO;_trainerCardTouchSource=nil;
        _trainerCardTouchLease=(HGNativeTrainerCardUITouchLease){0};[_trainerCardTouches removeAllObjects];
        _trainerCardTouchCooling=YES;_trainerCardTouchReleaseReads=reads;return;
    }
    if (_trainerCardTouchCooling) {
        if (reads-_trainerCardTouchReleaseReads<2) return;
        _trainerCardTouchCooling=NO;
    }
    if (_trainerCardInputCooling && reads-_trainerCardReleaseReads>=2) _trainerCardInputCooling=NO;
    if (_trainerCardInputActive && (reads-_trainerCardInputReads>=4
        || !HGNativeTrainerCardUIInputAllowed(state,_trainerCardInput.ownerGeneration,_trainerCardInput.revision))) {
        _trainerCardInputActive=NO;_trainerCardInputCooling=YES;_trainerCardReleaseReads=reads;
    }
    if (_trainerCardChoicePending) {
        uint32_t resolved=0;
        if (!_trainerCardInputCooling && HGNativeTrainerCardUIResolveKey(state,_trainerCardChoice.ownerGeneration,_trainerCardChoice.revision,_trainerCardChoiceKeys,&resolved)) {
            _trainerCardInput=_trainerCardChoice;_trainerCardInputKeys=resolved;_trainerCardInputReads=reads;_trainerCardInputActive=YES;
        }
        _trainerCardChoicePending=NO;
    }
    if (_trainerCardInputActive) {*keys=_trainerCardInputKeys;return;}
    if (_trainerCardInputCooling) return;
    if (_trainerCardTouches.count) {
        NSData *data=_trainerCardTouches.firstObject;
        OriginalTrainerTouchEvent event={0};if (data.length!=sizeof(event)) {[self clearTrainerCardInputLocked];return;}
        memcpy(&event,data.bytes,sizeof(event));
        // Display callbacks may repeat before PAD_Read. Retain short taps for the original sampler; moves stay continuous.
        if (event.event==HG_TRAINER_CARD_TOUCH_UP && _trainerCardTouchLease.down
            && event.generation==_trainerCardTouchLease.ownerGeneration && event.revision==_trainerCardTouchLease.revision
            && reads-_trainerCardTouchStartReads<4) {
            *x=_trainerCardTouchX;*y=_trainerCardTouchY;*pressed=YES;return;
        }
        [_trainerCardTouches removeObjectAtIndex:0];
        if (HGNativeTrainerCardUIResolveTouch(&_trainerCardTouchLease,state,event.generation,event.revision,event.event,event.x,event.y)) {
            if (event.event==HG_TRAINER_CARD_TOUCH_DOWN) _trainerCardTouchStartReads=reads;
            _trainerCardRawDown=_trainerCardTouchLease.down;
            _trainerCardTouchX=event.x;_trainerCardTouchY=event.y;
            if (!_trainerCardRawDown) {_trainerCardTouchCooling=YES;_trainerCardTouchReleaseReads=reads;}
        }
    }
    if (_trainerCardRawDown) {*x=_trainerCardTouchX;*y=_trainerCardTouchY;*pressed=YES;}
}
- (NSDictionary *)copyTrainerCardPictures:(const HGNativeTrainerCardUIState *)state main:(NSData *)main sub:(NSData *)sub {
    if (!state || state->active!=1 || state->viewActive!=1 || main.length!=256*192*4 || sub.length!=256*192*4) return nil;
    UIImage *body=OriginalFrameRegion(sub,0,0,256,168),*footer=OriginalFrameRegion(sub,0,168,256,24);
    UIImage *original=OriginalFrameRegion(main,0,0,256,192);
    UIImage *prompt=OriginalFrameRegion(state->view==HG_TRAINER_CARD_SIGNATURE ? main : sub,0,0,256,192);
    return body && footer && original && prompt ? @{@"body":body,@"footer":footer,@"badges":original,@"signature":original,@"prompt":prompt} : nil;
}
- (void)updateTrainerCardControls:(NSData *)data pictures:(NSDictionary *)pictures paused:(BOOL)paused {
    const HGNativeTrainerCardUIState *state=data.length==sizeof(*state) ? data.bytes : NULL;
    BOOL active=state && state->active==1,was=_trainerCardScene;
    const HGNativeTrainerCardUIState *old=_displayedTrainerCardUIState.length==sizeof(*old) ? _displayedTrainerCardUIState.bytes : NULL;
    if (!active || (old && old->ownerGeneration!=state->ownerGeneration)) {[_condition lock];[self clearTrainerCardInputLocked];[_condition unlock];}
    _trainerCardScene=active;_displayedTrainerCardUIState=data;_trainerCardPanel.hidden=!active;
    if (was!=active) [self.view setNeedsLayout];
    if (!active) {
        if (was) for (NSNumber *key in @[@64,@128,@32,@16,@1,@2,@8]) {
            UIButton *button=_buttons[key];button.configuration=nil;button.enabled=YES;button.alpha=1;button.accessibilityHint=nil;
            [button setTitle:[key isEqual:@1] ? @"确认" : [key isEqual:@2] ? @"返回" : [key isEqual:@8] ? @"开始" : button.currentTitle forState:UIControlStateNormal];
            button.accessibilityLabel=button.currentTitle;
        }
        return;
    }
    BOOL full=state->view==HG_TRAINER_CARD_SIGNATURE || state->phase==4;
    _trainerCardBody.image=pictures[full ? (state->view==HG_TRAINER_CARD_SIGNATURE ? @"signature" : @"prompt") : @"body"];
    _trainerCardFooter.image=pictures[@"footer"];_trainerCardBadges.image=pictures[@"badges"];
    _trainerCardFooter.hidden=_trainerCardBadges.hidden=full || state->viewActive!=1;
    _trainerCardBody.hidden=state->viewActive!=1;
    _canvas.hidden=YES;_canvas.userInteractionEnabled=NO;
    _battleBar.hidden=_partyPanel.hidden=_partyBack.hidden=_bagPanel.hidden=_bagPage.hidden=_summaryPanel.hidden=_battleMenuPanel.hidden=_pokedexPanel.hidden=_pcBoxPanel.hidden=YES;
    _fieldMenu.hidden=_fieldMenuShade.hidden=_joystick.hidden=_menuButton.hidden=_fieldChoicePreview.hidden=_optionsPreview.hidden=YES;
    for (UIButton *button in _bagNavigation) button.hidden=YES;
    for (UIButton *button in _summaryNavigation) button.hidden=YES;
    for (UIButton *button in _buttons.allValues) button.hidden=YES;
    [_condition lock];BOOL idle=!_trainerCardChoicePending && !_trainerCardInputActive && !_trainerCardInputCooling
        && !_trainerCardTracking && !_trainerCardRawDown && !_trainerCardTouchCooling && !_trainerCardReleasePending;
    [_condition unlock];
    BOOL enabled=!paused && pictures.count && idle && HGNativeTrainerCardUIInputAllowed(state,state->ownerGeneration,state->revision);
    [_condition lock];BOOL resume=_interrupted && _interruptionEnded && !_inactive;[_condition unlock];
    NSArray *keys=@[@64,@128,@32,@16,@1,@2],*titles=@[@"↑",@"↓",@"←",@"→",state->view==HG_TRAINER_CARD_VIEWER && state->phase==1 ? @"签名" : @"确认",@"返回"];
    for (NSUInteger i=0;i<keys.count;i++) {
        UIButton *button=_buttons[keys[i]];uint32_t resolved;
        BOOL allowed=HGNativeTrainerCardUIResolveKey(state,state->ownerGeneration,state->revision,[keys[i] unsignedIntValue],&resolved);
        button.hidden=!allowed;button.enabled=enabled && allowed;button.alpha=button.enabled ? 1 : 0.5;
        button.configuration=nil;[button setTitle:titles[i] forState:UIControlStateNormal];button.accessibilityLabel=titles[i];
        if (allowed) [self.view bringSubviewToFront:button];
    }
    _trainerCardSurface.userInteractionEnabled=!paused && pictures.count && HGNativeTrainerCardUIInputAllowed(state,state->ownerGeneration,state->revision);
    _trainerCardDone.hidden=!(state->view==HG_TRAINER_CARD_SIGNATURE && state->phase==1 && state->subState==1);
    _trainerCardDone.enabled=enabled;
    _buttons[@8].hidden=!resume;_buttons[@8].enabled=resume;[_buttons[@8] setTitle:@"继续" forState:UIControlStateNormal];_buttons[@8].accessibilityLabel=@"继续";
    if (resume) [self.view bringSubviewToFront:_buttons[@8]];
    [self layoutTrainerCardControls:UIEdgeInsetsInsetRect(self.view.bounds,self.view.safeAreaInsets)];
}
- (void)setupPCBoxControls {
    _pcBoxPanel=[UIView new]; _pcBoxPanel.hidden=YES; _pcBoxPanel.backgroundColor=_partyPanel.backgroundColor;
    _pcBoxPanel.layer.cornerRadius=12; _pcBoxPanel.clipsToBounds=YES; [self.view addSubview:_pcBoxPanel];
    _pcBoxWorkspace=[UIImageView new]; _pcBoxFooter=[UIImageView new]; _pcBoxInfo=[UIImageView new];
    NSMutableArray *actions=[NSMutableArray new];
    for (NSUInteger i=0;i<5;i++) [actions addObject:[UIImageView new]];
    _pcBoxActions=actions;
    NSMutableArray *images=[NSMutableArray arrayWithObjects:_pcBoxWorkspace,_pcBoxFooter,_pcBoxInfo,nil];
    [images addObjectsFromArray:actions];
    for (UIImageView *image in images) {
        image.contentMode=UIViewContentModeScaleAspectFit; image.clipsToBounds=YES; image.layer.magnificationFilter=kCAFilterNearest;
        image.isAccessibilityElement=YES; image.accessibilityTraits=UIAccessibilityTraitImage; [_pcBoxPanel addSubview:image];
    }
    _pcBoxWorkspace.accessibilityHint=@"使用方向键移动原版光标，再按A；按Y切换快捷操作";
    _pcBoxFooter.accessibilityLabel=@"原版电脑操作提示";
    _pcBoxInfo.accessibilityLabel=@"原版宝可梦资料、画像、招式和道具说明";
    for (NSUInteger i=0;i<actions.count;i++) {
        _pcBoxActions[i].accessibilityLabel=[NSString stringWithFormat:@"原版电脑操作第%lu项",(unsigned long)i+1];
        _pcBoxActions[i].accessibilityHint=@"只读原版操作栏；使用方向键选择，按A执行";
    }
    UIButton *shortcut=[self button:@"快捷" key:0x800]; shortcut.hidden=YES;
}
- (void)layoutPCBoxControls:(CGRect)area {
    if (!_pcBoxScene) return;
    CGFloat gap=8,controls=52,ph=MAX(1,area.size.height-controls-8),pw=area.size.width;
    _pcBoxPanel.frame=CGRectMake(area.origin.x,area.origin.y,pw,ph);
    CGFloat footerHeight=MIN(48,ph*0.17),bodyHeight=MAX(1,ph-footerHeight-gap),detailWidth=pw*0.40;
    CGFloat railWidth=MIN(132,pw*0.15),workspaceWidth=MAX(1,pw-detailWidth-railWidth-4*gap);
    BOOL railVisible=_pcBoxActions.count && !_pcBoxActions[0].hidden;
    _pcBoxWorkspace.frame=CGRectMake(gap,0,workspaceWidth+(!railVisible ? railWidth+gap : 0),bodyHeight);
    CGFloat railX=workspaceWidth+2*gap,rowHeight=MIN(44,bodyHeight/5);
    for (NSUInteger i=0;i<5;i++) _pcBoxActions[i].frame=CGRectMake(railX,i*rowHeight,railWidth,rowHeight);
    _pcBoxInfo.frame=CGRectMake(pw-detailWidth,0,detailWidth-gap,bodyHeight);
    _pcBoxFooter.frame=CGRectMake(gap,ph-footerHeight,pw-2*gap,footerHeight);
    NSArray *keys=@[@0x40,@0x80,@0x20,@0x10,@1,@2,@0x800];
    CGFloat keyWidth=(pw-6*gap)/7,y=CGRectGetMaxY(area)-controls;
    for (NSUInteger i=0;i<keys.count;i++) _buttons[keys[i]].frame=CGRectMake(area.origin.x+i*(keyWidth+gap),y,keyWidth,controls);
    _buttons[@8].frame=CGRectMake(CGRectGetMaxX(area)-96,area.origin.y+4,88,44);
    _status.frame=CGRectInset(_pcBoxPanel.frame,10,10);
}
- (void)clearPCBoxInputLocked {
    _pcBoxChoicePending=_pcBoxInputActive=_pcBoxInputCooling=NO;
    memset(&_pcBoxChoice,0,sizeof(_pcBoxChoice)); memset(&_pcBoxInput,0,sizeof(_pcBoxInput));
    _pcBoxChoiceKeys=_pcBoxInputKeys=_pcBoxInputReads=_pcBoxReleaseReads=0;
}
- (void)queuePCBoxKey:(OriginalKeyButton *)button {
    const HGNativePCBoxUIState *shown=_displayedPCBoxUIState.bytes; uint32_t keys;
    if (!_pcBoxScene || button.hidden || !button.enabled || _displayedPCBoxUIState.length!=sizeof(*shown)
        || !HGNativePCBoxUIResolveInput(shown,shown->ownerGeneration,shown->revision,(uint32_t)button.tag,&keys)) return;
    [_condition lock];
    const HGNativePCBoxUIState *live=_pcBoxUIState.length==sizeof(*live) ? _pcBoxUIState.bytes : NULL;
    if (!_paused && !_stopping && !_failure && !_pcBoxChoicePending && !_pcBoxInputActive && !_pcBoxInputCooling
        && HGNativePCBoxUIResolveInput(live,shown->ownerGeneration,shown->revision,keys,&keys)) {
        _pcBoxChoice=*shown; _pcBoxChoiceKeys=keys; _pcBoxChoicePending=YES;
    }
    [_condition unlock];
}
- (void)takePCBoxInputLocked:(const HGNativePCBoxUIState *)state reads:(uint32_t)reads keys:(uint32_t *)keys {
    *keys=0; uint32_t resolved;
    if (_pcBoxInputCooling && reads-_pcBoxReleaseReads>=2) [self clearPCBoxInputLocked];
    if (_pcBoxInputActive && (reads-_pcBoxInputReads>=4
        || !HGNativePCBoxUIResolveInput(state,_pcBoxInput.ownerGeneration,_pcBoxInput.revision,_pcBoxInputKeys,&resolved))) {
        [self clearPCBoxInputLocked]; _pcBoxInputCooling=YES; _pcBoxReleaseReads=reads;
    }
    if (_pcBoxChoicePending) {
        if (!_pcBoxInputActive && !_pcBoxInputCooling
            && HGNativePCBoxUIResolveInput(state,_pcBoxChoice.ownerGeneration,_pcBoxChoice.revision,_pcBoxChoiceKeys,&resolved)) {
            _pcBoxInput=_pcBoxChoice; _pcBoxInputKeys=resolved; _pcBoxInputReads=reads; _pcBoxInputActive=YES;
        }
        _pcBoxChoicePending=NO; _pcBoxChoiceKeys=0; memset(&_pcBoxChoice,0,sizeof(_pcBoxChoice));
    }
    if (_pcBoxInputActive) *keys=_pcBoxInputKeys;
}
- (NSDictionary *)copyPCBoxPictures:(const HGNativePCBoxUIState *)state main:(NSData *)main sub:(NSData *)sub {
    if (state->active!=1) { _pcBoxOwnerPictures=nil; return nil; }
    if (state->viewActive!=1) return _pcBoxOwnerPictures;
    if (main.length!=256*192*4 || sub.length!=256*192*4) return nil;
    BOOL full=state->partyPanelOpen || state->trayOpen || !(state->phase==HG_PC_BOX_UI_MOVE_MODE_INPUT
        || state->phase==HG_PC_BOX_UI_WITHDRAW_INPUT || state->phase==HG_PC_BOX_UI_DEPOSIT_INPUT || state->phase==HG_PC_BOX_UI_ITEM_MODE_INPUT);
    NSMutableArray *actions=[NSMutableArray new];
    for (NSUInteger i=0;i<5;i++) [actions addObject:OriginalFrameRegion(main,168,40+24*(uint32_t)i,88,24) ?: NSNull.null];
    _pcBoxOwnerPictures=@{@"workspace":OriginalFrameRegion(main,0,0,full ? 256 : 168,full ? 160 : 168) ?: NSNull.null,
        @"footer":OriginalFrameRegion(main,0,160,256,32) ?: NSNull.null,@"actions":actions,
        @"info":OriginalFrameRegion(sub,0,0,256,192) ?: NSNull.null,
        @"generation":@(state->ownerGeneration),@"revision":@(state->revision),@"full":@(full)};
    return _pcBoxOwnerPictures;
}
- (void)updatePCBoxControls:(NSData *)data pictures:(NSDictionary *)pictures blocked:(BOOL)blocked paused:(BOOL)paused {
    const HGNativePCBoxUIState *state=data.bytes;
    BOOL scene=data.length==sizeof(*state) && state->active==1 && !blocked,was=_pcBoxScene;
    BOOL changed=was!=scene || (scene && (_displayedPCBoxUIState.length!=sizeof(*state) || memcmp(_displayedPCBoxUIState.bytes,state,sizeof(*state))));
    _pcBoxScene=scene; _displayedPCBoxUIState=data; _pcBoxPanel.hidden=!scene; _buttons[@0x800].hidden=!scene;
    if (changed && (was || scene)) {
        [_condition lock]; _pcBoxChoicePending=NO; _pcBoxChoiceKeys=0; memset(&_pcBoxChoice,0,sizeof(_pcBoxChoice)); _inputGeneration++;
        _keys=_pendingKeys=_submittedKeys=_joystickKeys=_pendingJoystickKeys=_submittedJoystickKeys=0; _touchPressed=NO; _touchX=_touchY=0;
        if (!scene) {
            [self clearPCBoxInputLocked];
            if (data.length==sizeof(*state) && state->active==1) {_pcBoxInputCooling=YES;_pcBoxReleaseReads=_lastPadReadCount;}
        }
        [_condition unlock];
        for (OriginalKeyButton *button in _buttons.allValues) button.inputGeneration++;
        [self resetJoystick]; [self.view setNeedsLayout];
    }
    if (!scene) {
        if (data.length!=sizeof(*state) || !state->active) {
            _pcBoxWorkspace.image=_pcBoxFooter.image=_pcBoxInfo.image=nil;
            for (UIImageView *image in _pcBoxActions) image.image=nil;
        }
        if (was) for (NSNumber *key in @[@0x40,@0x80,@0x20,@0x10,@1,@2,@8]) {
            UIButton *button=_buttons[key]; button.configuration=nil; button.enabled=YES; button.alpha=1; button.accessibilityHint=nil;
            [button setTitle:[key isEqual:@1] ? @"确认" : [key isEqual:@2] ? @"返回" : [key isEqual:@8] ? @"开始" : button.currentTitle forState:UIControlStateNormal];
            button.accessibilityLabel=button.currentTitle;
        }
        return;
    }
    _battleBar.hidden=_partyPanel.hidden=_partyBack.hidden=_bagPanel.hidden=_bagPage.hidden=_summaryPanel.hidden=_battleMenuPanel.hidden=_pokedexPanel.hidden=YES;
    _fieldMenu.hidden=_fieldMenuShade.hidden=_joystick.hidden=_menuButton.hidden=_canvas.hidden=_optionsPreview.hidden=YES;
    _canvas.userInteractionEnabled=NO;
    for (UIButton *button in _bagNavigation) button.hidden=YES;
    for (UIButton *button in _summaryNavigation) button.hidden=YES;
    _pcBoxPanel.alpha=state->viewActive==1 && !paused ? 1 : 0.55;
    _pcBoxWorkspace.image=[pictures[@"workspace"] isKindOfClass:UIImage.class] ? pictures[@"workspace"] : nil;
    _pcBoxFooter.image=[pictures[@"footer"] isKindOfClass:UIImage.class] ? pictures[@"footer"] : nil;
    NSString *cursor=state->selectedSlot<36 ? [NSString stringWithFormat:@"光标位置%u",state->selectedSlot+1] : @"盒子标题或操作栏";
    _pcBoxWorkspace.accessibilityLabel=[NSString stringWithFormat:@"原版电脑盒子%u，%@，%@%@",state->activeBox+1,
        (@[@"寄放宝可梦",@"取出宝可梦",@"移动宝可梦",@"移动道具"])[MIN(state->mode,3u)],cursor,state->cursorHolding ? @"，正在持有" : @""];
    NSArray *actions=pictures[@"actions"];
    _pcBoxInfo.image=[pictures[@"info"] isKindOfClass:UIImage.class] ? pictures[@"info"] : nil;
    for (NSUInteger i=0;i<5;i++) {
        _pcBoxActions[i].image=i<actions.count && [actions[i] isKindOfClass:UIImage.class] ? actions[i] : nil;
        _pcBoxActions[i].hidden=[pictures[@"full"] boolValue] || !state->actionMenuOpen;
    }
    [_condition lock]; BOOL idle=!_stopping && !_failure && !_pcBoxChoicePending && !_pcBoxInputActive && !_pcBoxInputCooling;
    BOOL resume=_interrupted && _interruptionEnded && !_inactive; [_condition unlock];
    NSArray *keys=@[@0x40,@0x80,@0x20,@0x10,@1,@2,@0x800],*titles=@[@"↑",@"↓",@"←",@"→",@"选择",@"返回",@"快捷"];
    NSArray *labels=@[@"向上移动原版光标",@"向下移动原版光标",@"向左移动原版光标",@"向右移动原版光标",@"A：执行原版当前项目",@"B：原版返回或取消",@"Y：切换原版快捷操作"];
    for (NSUInteger i=0;i<keys.count;i++) {
        UIButton *button=_buttons[keys[i]]; uint32_t resolved; button.hidden=NO; button.configuration=nil;
        button.enabled=!paused && idle && HGNativePCBoxUIResolveInput(state,state->ownerGeneration,state->revision,[keys[i] unsignedIntValue],&resolved);
        button.alpha=button.enabled ? 1 : 0.5; [button setTitle:titles[i] forState:UIControlStateNormal]; button.accessibilityLabel=labels[i];
        button.accessibilityHint=@"按键交给原版电脑处理；操作栏用方向键和A选择"; [self.view bringSubviewToFront:button];
    }
    _buttons[@0x400].hidden=YES; _buttons[@8].hidden=!resume; _buttons[@8].enabled=resume;
    [_buttons[@8] setTitle:resume ? @"继续" : @"开始" forState:UIControlStateNormal]; _buttons[@8].accessibilityLabel=resume ? @"恢复应用" : @"开始";
    if (resume) [self.view bringSubviewToFront:_buttons[@8]];
}
- (void)layoutFieldChoiceControls:(CGRect)area {
    if (!_fieldChoiceScene) return;
    CGFloat side=MIN(150,area.size.width*0.19),gap=8;
    CGFloat width=MIN(area.size.width-2*side,MAX(1,area.size.height-gap)*256.0/240.0);
    CGFloat menuHeight=width*192/256,previewHeight=width*48/256;
    CGFloat x=CGRectGetMidX(area)-width/2,y=CGRectGetMidY(area)-(menuHeight+gap+previewHeight)/2;
    _canvas.frame=CGRectMake(x,y,width,menuHeight);
    _fieldChoicePreview.frame=CGRectMake(x,y+menuHeight+gap,width,previewHeight);
    CGFloat size=MAX(44,MIN(56,(side-12)/2)),left=area.origin.x+6,bottom=CGRectGetMaxY(area)-size-12;
    _buttons[@0x40].frame=CGRectMake(left+size/2,bottom-size*2-8,size,size);
    _buttons[@0x80].frame=CGRectMake(left+size/2,bottom,size,size);
    _buttons[@0x20].frame=CGRectMake(left,bottom-size-4,size,size);
    _buttons[@0x10].frame=CGRectMake(left+size+4,bottom-size-4,size,size);
    CGFloat right=CGRectGetMaxX(area)-side+6;
    _buttons[@1].frame=CGRectMake(right,bottom-size-8,side-12,size);
    _buttons[@2].frame=CGRectMake(right,bottom,side-12,size);
    _status.frame=CGRectInset(_canvas.frame,10,10);
}
- (void)clearFieldChoiceInputLocked {
    _fieldChoiceChoicePending=_fieldChoiceInputActive=_fieldChoiceInputCooling=NO;
    memset(&_fieldChoiceChoice,0,sizeof(_fieldChoiceChoice)); memset(&_fieldChoiceInput,0,sizeof(_fieldChoiceInput));
    _fieldChoiceChoiceKeys=_fieldChoiceInputKeys=_fieldChoiceInputReads=_fieldChoiceReleaseReads=0;
}
- (void)queueFieldChoiceKey:(OriginalKeyButton *)button {
    const HGNativeFieldChoiceUIState *shown=_displayedFieldChoiceUIState.bytes; uint32_t keys;
    if (!_fieldChoiceScene || button.hidden || !button.enabled || _displayedFieldChoiceUIState.length!=sizeof(*shown)
        || !HGNativeFieldChoiceUIResolveInput(shown,shown->ownerGeneration,shown->revision,(uint32_t)button.tag,&keys)) return;
    [_condition lock];
    const HGNativeFieldChoiceUIState *live=_fieldChoiceUIState.length==sizeof(*live) ? _fieldChoiceUIState.bytes : NULL;
    if (!_paused && !_stopping && !_failure && !_fieldChoiceChoicePending && !_fieldChoiceInputActive && !_fieldChoiceInputCooling
        && HGNativeFieldChoiceUIResolveInput(live,shown->ownerGeneration,shown->revision,keys,&keys)) {
        _fieldChoiceChoice=*shown; _fieldChoiceChoiceKeys=keys; _fieldChoiceChoicePending=YES;
    }
    [_condition unlock];
}
- (void)takeFieldChoiceInputLocked:(const HGNativeFieldChoiceUIState *)state reads:(uint32_t)reads keys:(uint32_t *)keys {
    *keys=0; uint32_t resolved;
    if (_fieldChoiceInputCooling && reads-_fieldChoiceReleaseReads>=2) [self clearFieldChoiceInputLocked];
    if (_fieldChoiceInputActive && (reads-_fieldChoiceInputReads>=4
        || !HGNativeFieldChoiceUIResolveInput(state,_fieldChoiceInput.ownerGeneration,_fieldChoiceInput.revision,_fieldChoiceInputKeys,&resolved))) {
        [self clearFieldChoiceInputLocked]; _fieldChoiceInputCooling=YES; _fieldChoiceReleaseReads=reads;
    }
    if (_fieldChoiceChoicePending) {
        if (!_fieldChoiceInputActive && !_fieldChoiceInputCooling
            && HGNativeFieldChoiceUIResolveInput(state,_fieldChoiceChoice.ownerGeneration,_fieldChoiceChoice.revision,_fieldChoiceChoiceKeys,&resolved)) {
            _fieldChoiceInput=_fieldChoiceChoice; _fieldChoiceInputKeys=resolved; _fieldChoiceInputReads=reads; _fieldChoiceInputActive=YES;
        }
        _fieldChoiceChoicePending=NO; _fieldChoiceChoiceKeys=0; memset(&_fieldChoiceChoice,0,sizeof(_fieldChoiceChoice));
    }
    if (_fieldChoiceInputActive) *keys=_fieldChoiceInputKeys;
}
- (void)updateFieldChoiceControls:(NSData *)data preview:(UIImage *)preview blocked:(BOOL)blocked paused:(BOOL)paused {
    const HGNativeFieldChoiceUIState *state=data.bytes;
    BOOL scene=data.length==sizeof(*state) && state->active==1 && HGNativeFieldChoiceUIIsViewPhase(state->phase) && !blocked;
    BOOL was=_fieldChoiceScene,changed=was!=scene || (scene && (_displayedFieldChoiceUIState.length!=sizeof(*state)
        || memcmp(_displayedFieldChoiceUIState.bytes,state,sizeof(*state))));
    _fieldChoiceScene=scene; _displayedFieldChoiceUIState=data;
    if (changed && (was || scene)) {
        [_condition lock]; _fieldChoiceChoicePending=NO; _fieldChoiceChoiceKeys=0; memset(&_fieldChoiceChoice,0,sizeof(_fieldChoiceChoice));
        _inputGeneration++; _keys=_pendingKeys=_submittedKeys=_joystickKeys=_pendingJoystickKeys=_submittedJoystickKeys=0;
        _touchPressed=NO; _touchX=_touchY=0;
        if (!scene) [self clearFieldChoiceInputLocked]; [_condition unlock];
        for (UIGestureRecognizer *gesture in _canvas.gestureRecognizers) {gesture.enabled=NO;gesture.enabled=YES;}
        for (OriginalKeyButton *button in _buttons.allValues) button.inputGeneration++;
        [self resetJoystick]; [self.view setNeedsLayout];
    }
    _fieldChoicePreview.hidden=!scene || !preview; _fieldChoicePreview.image=scene ? preview : nil;
    if (!scene) {
        if (was) {
            _canvas.alpha=1;
            if (!_evolutionScene && !_captureScene) {_canvas.accessibilityLabel=@"原版游戏画面";_canvas.accessibilityValue=nil;}
            for (NSNumber *key in @[@0x40,@0x80,@0x20,@0x10,@1,@2,@8]) {
                if ((_evolutionScene || _captureScene) && (key.unsignedIntValue==1 || key.unsignedIntValue==2)) continue;
                UIButton *button=_buttons[key]; button.configuration=nil; button.enabled=YES; button.alpha=1; button.accessibilityHint=nil;
                [button setTitle:[key isEqual:@1] ? @"确认" : [key isEqual:@2] ? @"返回" : [key isEqual:@8] ? @"开始" : button.currentTitle forState:UIControlStateNormal];
                button.accessibilityLabel=button.currentTitle;
            }
        }
        return;
    }
    _battleBar.hidden=_partyPanel.hidden=_partyBack.hidden=_bagPanel.hidden=_bagPage.hidden=_summaryPanel.hidden=_battleMenuPanel.hidden=_pokedexPanel.hidden=_pcBoxPanel.hidden=YES;
    _fieldMenu.hidden=_fieldMenuShade.hidden=_joystick.hidden=_menuButton.hidden=_optionsPreview.hidden=YES;
    for (UIButton *button in _bagNavigation) button.hidden=YES;
    for (UIButton *button in _summaryNavigation) button.hidden=YES;
    [_condition lock]; BOOL idle=!_stopping && !_failure && !_fieldChoiceChoicePending && !_fieldChoiceInputActive && !_fieldChoiceInputCooling;
    BOOL resume=_interrupted && _interruptionEnded && !_inactive; [_condition unlock];
    BOOL yesNo=!HGNativeFieldChoiceUIIsListPhase(state->phase);
    NSArray *keys=@[@0x40,@0x80,@0x20,@0x10,@1,@2],*titles=@[@"↑",@"↓",@"←",@"→",@"选择",yesNo ? @"否" : @"返回"];
    NSArray *labels=@[@"向上选择",@"向下选择",@"向左选择",@"向右选择",@"A：选择当前项目",yesNo ? @"B：选择否" : @"B：返回"];
    for (NSUInteger i=0;i<keys.count;i++) {
        UIButton *button=_buttons[keys[i]]; uint32_t resolved; button.hidden=NO; button.configuration=nil;
        button.enabled=!paused && idle && HGNativeFieldChoiceUIResolveInput(state,state->ownerGeneration,state->revision,[keys[i] unsignedIntValue],&resolved);
        button.alpha=button.enabled ? 1 : 0.5; [button setTitle:titles[i] forState:UIControlStateNormal]; button.accessibilityLabel=labels[i];
        button.accessibilityHint=i==5 && yesNo ? @"选择否并确认" : i==5 && !state->cancelEnabled ? @"当前选项不能取消" : @"使用原版方向键和A选择";
    }
    _buttons[@0x400].hidden=_buttons[@0x800].hidden=YES;
    _buttons[@8].hidden=!resume; _buttons[@8].enabled=resume;
    [_buttons[@8] setTitle:resume ? @"继续" : @"开始" forState:UIControlStateNormal]; _buttons[@8].accessibilityLabel=resume ? @"恢复应用" : @"开始";
    _canvas.hidden=NO; _canvas.userInteractionEnabled=NO; _canvas.isAccessibilityElement=YES;
    _canvas.alpha=_fieldChoicePreview.alpha=state->viewActive==1 && !paused ? 1 : 0.55;
    _canvas.accessibilityLabel=@"原版选项菜单";
    _canvas.accessibilityValue=state->rowCount ? [NSString stringWithFormat:@"第%u项，共%u项",state->selectedIndex+1,state->rowCount] : @"正在准备选项";
}
- (void)clearOptionsInputLocked {
    _optionsChoicePending=_optionsInputActive=_optionsInputCooling=NO;
    memset(&_optionsChoice,0,sizeof(_optionsChoice)); memset(&_optionsInput,0,sizeof(_optionsInput));
    _optionsChoiceKeys=_optionsInputKeys=_optionsInputReads=_optionsReleaseReads=0;
}
- (void)queueOptionsKey:(OriginalKeyButton *)button {
    const HGNativeOptionsUIState *shown=_displayedOptionsUIState.bytes;
    uint32_t keys=(uint32_t)button.tag;
    if (!_optionsScene || button.hidden || !button.enabled || !keys || (keys&~0xF3u) || (keys&(keys-1))
        || _displayedOptionsUIState.length!=sizeof(*shown) || !HGNativeOptionsUIInputAllowed(shown,shown->ownerGeneration,shown->revision)) return;
    [_condition lock];
    const HGNativeOptionsUIState *live=_optionsUIState.length==sizeof(*live) ? _optionsUIState.bytes : NULL;
    if (!_paused && !_stopping && !_failure && !_optionsChoicePending && !_optionsInputActive && !_optionsInputCooling
        && HGNativeOptionsUIInputAllowed(live,shown->ownerGeneration,shown->revision)) {
        _optionsChoice=*shown; _optionsChoiceKeys=keys; _optionsChoicePending=YES;
    }
    [_condition unlock];
}
- (void)takeOptionsInputLocked:(const HGNativeOptionsUIState *)state reads:(uint32_t)reads keys:(uint32_t *)keys {
    *keys=0;
    if (_optionsInputCooling && reads-_optionsReleaseReads>=2) [self clearOptionsInputLocked];
    if (_optionsInputActive && (reads-_optionsInputReads>=4 || !HGNativeOptionsUIInputAllowed(state,_optionsInput.ownerGeneration,_optionsInput.revision))) {
        _optionsInputActive=NO; _optionsInputCooling=YES; _optionsReleaseReads=reads;
    }
    if (_optionsChoicePending) {
        if (!_optionsInputActive && !_optionsInputCooling && HGNativeOptionsUIInputAllowed(state,_optionsChoice.ownerGeneration,_optionsChoice.revision)) {
            _optionsInput=_optionsChoice; _optionsInputKeys=_optionsChoiceKeys; _optionsInputReads=reads; _optionsInputActive=YES;
        }
        _optionsChoicePending=NO;
    }
    if (_optionsInputActive) *keys=_optionsInputKeys;
}
- (void)updateOptionsControls:(NSData *)data preview:(UIImage *)preview paused:(BOOL)paused {
    const HGNativeOptionsUIState *state=data.bytes;
    BOOL scene=data.length==sizeof(*state) && state->active==1;
    BOOL was=_optionsScene,changed=was!=scene || (scene && (_displayedOptionsUIState.length!=sizeof(*state) || memcmp(_displayedOptionsUIState.bytes,state,sizeof(*state))));
    _optionsScene=scene; _displayedOptionsUIState=data;
    if (changed && (was || scene)) {
        [_condition lock]; _optionsChoicePending=NO; _inputGeneration++;
        _keys=_pendingKeys=_submittedKeys=_joystickKeys=_pendingJoystickKeys=_submittedJoystickKeys=0; _touchPressed=NO;
        if (!scene) [self clearOptionsInputLocked]; [_condition unlock];
        for (UIGestureRecognizer *gesture in _canvas.gestureRecognizers) {gesture.enabled=NO;gesture.enabled=YES;}
        for (OriginalKeyButton *button in _buttons.allValues) button.inputGeneration++;
        [self resetJoystick]; [self.view setNeedsLayout];
    }
    _optionsPreview.image=scene ? preview : nil; _optionsPreview.hidden=!scene || !preview;
    if (!scene) {
        if (was) {
            _canvas.accessibilityLabel=@"原版游戏画面";
            [_buttons[@8] setTitle:@"开始" forState:UIControlStateNormal]; _buttons[@8].accessibilityLabel=@"开始"; _buttons[@8].enabled=YES;
            for (NSNumber *key in @[@0x40,@0x80,@0x20,@0x10,@1,@2]) {
                UIButton *button=_buttons[key]; button.configuration=nil; button.enabled=YES; button.alpha=1; button.accessibilityHint=nil;
                [button setTitle:[key isEqual:@1] ? @"确认" : [key isEqual:@2] ? @"返回" : button.currentTitle forState:UIControlStateNormal]; button.accessibilityLabel=button.currentTitle;
            }
        }
        return;
    }
    _battleBar.hidden=_partyPanel.hidden=_partyBack.hidden=_bagPanel.hidden=_bagPage.hidden=_summaryPanel.hidden=_battleMenuPanel.hidden=_pokedexPanel.hidden=YES;
    _fieldMenu.hidden=_fieldMenuShade.hidden=_joystick.hidden=_menuButton.hidden=YES;
    for (UIButton *button in _bagNavigation) button.hidden=YES;
    for (UIButton *button in _summaryNavigation) button.hidden=YES;
    [_condition lock]; BOOL idle=!_stopping && !_failure && !_optionsChoicePending && !_optionsInputActive && !_optionsInputCooling;
    BOOL resume=_interrupted && _interruptionEnded && !_inactive; [_condition unlock];
    BOOL enabled=!paused && idle && HGNativeOptionsUIInputAllowed(state,state->ownerGeneration,state->revision);
    NSArray *keys=@[@0x40,@0x80,@0x20,@0x10,@1,@2],*titles=@[@"↑",@"↓",@"←",@"→",@"选择",@"取消"];
    NSArray *labels=@[@"向上选择设置",@"向下选择设置",@"向左调整原版选项",@"向右调整原版选项",@"A：操作原版当前项目",@"B：按原版取消并恢复设置"];
    for (NSUInteger i=0;i<keys.count;i++) {
        UIButton *button=_buttons[keys[i]]; button.hidden=NO; button.configuration=nil; button.enabled=enabled; button.alpha=enabled ? 1 : 0.5;
        [button setTitle:titles[i] forState:UIControlStateNormal]; button.accessibilityLabel=labels[i];
        button.accessibilityHint=i==4 ? @"在原版底部选中确认或取消后按A" : nil;
    }
    _buttons[@0x400].hidden=YES; _buttons[@8].hidden=!resume; _buttons[@8].enabled=resume;
    [_buttons[@8] setTitle:resume ? @"继续" : @"开始" forState:UIControlStateNormal];
    _buttons[@8].accessibilityLabel=resume ? @"恢复应用" : @"开始";
    _canvas.hidden=NO; _canvas.userInteractionEnabled=NO; _canvas.isAccessibilityElement=YES; _canvas.accessibilityLabel=@"原版设置菜单";
}
- (void)clearNamingInputLocked {
    _namingChoicePending=_namingChoiceTouch=_namingInputActive=_namingInputCooling=_namingInputTouch=NO;
    memset(&_namingChoice,0,sizeof(_namingChoice)); memset(&_namingInput,0,sizeof(_namingInput));
    _namingChoiceKeys=_namingChoiceX=_namingChoiceY=_namingInputKeys=_namingInputX=_namingInputY=_namingInputReads=_namingReleaseReads=0;
}
- (void)queueNamingInputKeys:(uint32_t)keys touch:(BOOL)touch point:(CGPoint)point {
    const HGNativeNamingUIState *shown=_displayedNamingUIState.bytes;
    if (!_namingScene || _displayedNamingUIState.length!=sizeof(*shown)
        || !HGNativeNamingUIInputAllowed(shown,shown->ownerGeneration,shown->revision)) return;
    uint32_t x=0,y=0;
    if (touch) {
        if (keys || _canvas.hidden || !_canvas.userInteractionEnabled || _displayedWidth!=256
            || _canvas.image.size.width!=256 || _canvas.image.size.height!=192 || !isfinite(point.x) || !isfinite(point.y)
            || _canvas.bounds.size.width<=0 || _canvas.bounds.size.height<=0
            || fabs(_canvas.bounds.size.width*3-_canvas.bounds.size.height*4)>1 || !CGRectContainsPoint(_canvas.bounds,point)) return;
        x=(uint32_t)MIN(255,(point.x-CGRectGetMinX(_canvas.bounds))*256/_canvas.bounds.size.width);
        y=(uint32_t)MIN(191,(point.y-CGRectGetMinY(_canvas.bounds))*192/_canvas.bounds.size.height);
    } else if (!keys || (keys&~255u) || (keys&(keys-1))) return;
    [_condition lock];
    const HGNativeNamingUIState *live=_namingUIState.length==sizeof(*live) ? _namingUIState.bytes : NULL;
    if (!_paused && !_stopping && !_failure && !_namingChoicePending && !_namingInputActive && !_namingInputCooling
        && HGNativeNamingUIInputAllowed(live,shown->ownerGeneration,shown->revision)) {
        _namingChoice=*shown; _namingChoiceKeys=keys; _namingChoiceTouch=touch; _namingChoiceX=x; _namingChoiceY=y; _namingChoicePending=YES;
        _inputGeneration++;
    }
    [_condition unlock];
}
- (void)takeNamingInputLocked:(const HGNativeNamingUIState *)state reads:(uint32_t)reads
    keys:(uint32_t *)keys x:(uint32_t *)x y:(uint32_t *)y pressed:(BOOL *)pressed {
    *keys=*x=*y=0; *pressed=NO;
    if (_namingInputCooling && reads-_namingReleaseReads>=2) [self clearNamingInputLocked];
    if (_namingInputActive && (reads-_namingInputReads>=4
        || !HGNativeNamingUIInputAllowed(state,_namingInput.ownerGeneration,_namingInput.revision))) {
        _namingInputActive=NO; _namingInputCooling=YES; _namingReleaseReads=reads;
    }
    if (_namingChoicePending) {
        if (!_namingInputActive && !_namingInputCooling
            && HGNativeNamingUIInputAllowed(state,_namingChoice.ownerGeneration,_namingChoice.revision)) {
            _namingInput=_namingChoice; _namingInputKeys=_namingChoiceKeys; _namingInputTouch=_namingChoiceTouch;
            _namingInputX=_namingChoiceX; _namingInputY=_namingChoiceY; _namingInputReads=reads; _namingInputActive=YES;
        }
        _namingChoicePending=NO;
    }
    if (_namingInputActive) {*keys=_namingInputKeys;*x=_namingInputX;*y=_namingInputY;*pressed=_namingInputTouch;}
}
- (void)updateNamingControls:(NSData *)data paused:(BOOL)paused {
    const HGNativeNamingUIState *state=data.bytes;
    BOOL scene=data.length==sizeof(*state) && state->active==1;
    BOOL was=_namingScene,changed=was!=scene || (scene && (_displayedNamingUIState.length!=sizeof(*state)
        || memcmp(_displayedNamingUIState.bytes,state,sizeof(*state))));
    _namingScene=scene; _displayedNamingUIState=data;
    if (changed && (was || scene)) {
        [_condition lock];
        _namingChoicePending=NO; _inputGeneration++;
        _keys=_pendingKeys=_submittedKeys=_joystickKeys=_pendingJoystickKeys=_submittedJoystickKeys=0;
        _touchPressed=NO; _touchX=_touchY=0;
        if (!scene) [self clearNamingInputLocked];
        [_condition unlock];
        // Cancel touches begun against the previous owner/readiness before showing the next packet.
        for (UIGestureRecognizer *gesture in _canvas.gestureRecognizers) {gesture.enabled=NO;gesture.enabled=YES;}
        for (OriginalKeyButton *button in _buttons.allValues) button.inputGeneration++;
        [self resetJoystick]; [self.view setNeedsLayout];
    }
    if (!scene) {
        if (was) {
            _canvas.accessibilityLabel=@"原版游戏画面"; _canvas.accessibilityValue=nil;
            for (NSNumber *key in @[@1,@2,@8,@0x400]) {
                UIButton *button=_buttons[key]; button.configuration=nil; button.enabled=YES; button.alpha=1;
                [button setTitle:[key isEqual:@1] ? @"确认" : [key isEqual:@2] ? @"返回" : [key isEqual:@8] ? @"开始" : @"X" forState:UIControlStateNormal];
                button.accessibilityLabel=button.currentTitle; button.accessibilityHint=nil;
            }
            for (NSNumber *key in @[@0x40,@0x80,@0x20,@0x10]) {_buttons[key].accessibilityLabel=_buttons[key].currentTitle;_buttons[key].accessibilityHint=nil;}
        }
        return;
    }
    _battleBar.hidden=_partyPanel.hidden=_partyBack.hidden=_bagPanel.hidden=_bagPage.hidden=_summaryPanel.hidden=_battleMenuPanel.hidden=_pokedexPanel.hidden=YES;
    _fieldMenu.hidden=_fieldMenuShade.hidden=_joystick.hidden=_menuButton.hidden=YES;
    for (UIButton *button in _bagNavigation) button.hidden=YES;
    for (UIButton *button in _summaryNavigation) button.hidden=YES;
    [_condition lock];
    BOOL idle=!_stopping && !_failure && !_namingChoicePending && !_namingInputActive && !_namingInputCooling;
    BOOL resume=_interrupted && _interruptionEnded && !_inactive;
    [_condition unlock];
    BOOL enabled=!paused && idle && HGNativeNamingUIInputAllowed(state,state->ownerGeneration,state->revision);
    NSArray *keys=@[@0x40,@0x80,@0x20,@0x10,@1,@2,@0x400,@8];
    NSArray *titles=@[@"↑",@"↓",@"←",@"→",@"选择",@"删除",@"换页",resume ? @"继续" : @"定位OK"];
    NSArray *labels=@[@"向上移动光标",@"向下移动光标",@"向左移动光标",@"向右移动光标",@"A：选择当前字符或项目",@"B：删除前一个字符",@"Select：切换原版键盘页",resume ? @"恢复应用" : @"Start：仅将光标移到OK"];
    for (NSUInteger i=0;i<keys.count;i++) {
        UIButton *button=_buttons[keys[i]]; button.hidden=NO; button.configuration=nil;
        button.enabled=enabled || (i==7 && resume); button.alpha=button.enabled ? 1 : 0.5;
        [button setTitle:titles[i] forState:UIControlStateNormal]; button.accessibilityLabel=labels[i];
        button.accessibilityHint=i==7 ? (resume ? @"恢复应用，不发送命名按键" : @"仍需按A或点击原版OK才能确认") : nil;
    }
    _canvas.hidden=NO; _canvas.userInteractionEnabled=enabled; _canvas.isAccessibilityElement=YES;
    _canvas.accessibilityLabel=@"原版命名键盘"; _canvas.accessibilityValue=state->ready ? @"按原版键盘输入" : @"请等待原版键盘";
}
- (void)clearCaptureInputLocked {
    _captureChoicePending=_captureInputActive=_captureInputCooling=NO;
    _captureChoiceGeneration=_captureChoiceRevision=_captureInputGeneration=_captureInputRevision=_captureInputReads=_captureReleaseReads=0;
}
- (void)queueCaptureButton:(UIButton *)button {
    const HGNativeCaptureUIState *state=_displayedCaptureUIState.bytes;
    if (!_captureScene || _displayedCaptureUIState.length!=sizeof(*state) || button.tag!=1 || button.hidden || !button.enabled) return;
    uint32_t keys;
    if (!HGNativeCaptureUIResolveInput(state,state->ownerGeneration,state->revision,&keys)) return;
    [_condition lock];
    if (!_paused && !_stopping && !_failure && !_captureChoicePending && !_captureInputActive && !_captureInputCooling) {
        _captureChoiceGeneration=state->ownerGeneration; _captureChoiceRevision=state->revision; _captureChoicePending=YES;
    }
    [_condition unlock];
}
- (void)takeCaptureInputLocked:(const HGNativeCaptureUIState *)state reads:(uint32_t)reads keys:(uint32_t *)keys {
    if (!state->active) { [self clearCaptureInputLocked]; return; }
    *keys=0;
    if (_captureInputCooling && reads-_captureReleaseReads>=2) _captureInputCooling=NO;
    if (_captureChoicePending) {
        uint32_t resolved;
        if (!_captureInputCooling && HGNativeCaptureUIResolveInput(state,_captureChoiceGeneration,_captureChoiceRevision,&resolved)) {
            _captureInputActive=YES; _captureInputGeneration=state->ownerGeneration; _captureInputRevision=state->revision; _captureInputReads=reads;
        }
        _captureChoicePending=NO;
    }
    if (_captureInputActive) {
        uint32_t resolved;
        if (reads-_captureInputReads>=4 || !HGNativeCaptureUIResolveInput(state,_captureInputGeneration,_captureInputRevision,&resolved)) {
            _captureInputActive=NO; _captureInputCooling=YES; _captureReleaseReads=reads;
        } else *keys=resolved;
    }
}
- (void)updateCaptureControls:(NSData *)data paused:(BOOL)paused {
    const HGNativeCaptureUIState *state=data.bytes;
    BOOL scene=data.length==sizeof(*state) && state->active==1;
    BOOL changed=_captureScene!=scene || _displayedCaptureUIState.length!=data.length
        || (scene && memcmp(data.bytes,_displayedCaptureUIState.bytes,sizeof(*state)));
    if (_captureScene && !scene) {
        [UIView performWithoutAnimation:^{ for (NSNumber *key in @[@1,@2]) { self->_buttons[key].configuration=nil; self->_buttons[key].enabled=YES; self->_buttons[key].alpha=1; [self->_buttons[key] setTitle:nil forState:UIControlStateNormal]; } }];
        [_condition lock]; [self clearCaptureInputLocked]; [_condition unlock];
    }
    _captureScene=scene; _displayedCaptureUIState=data;
    if (changed) { if (scene) [self resetJoystick]; [self.view setNeedsLayout]; }
    if (!scene) return;
    _battleBar.hidden=_partyPanel.hidden=_partyBack.hidden=_bagPanel.hidden=_bagPage.hidden=_summaryPanel.hidden=_battleMenuPanel.hidden=_pokedexPanel.hidden=YES;
    for (UIButton *button in _bagNavigation) button.hidden=YES;
    for (UIButton *button in _summaryNavigation) button.hidden=YES;
    _canvas.isAccessibilityElement=YES; _canvas.accessibilityLabel=@"原版捕获登记画面";
    _canvas.accessibilityValue=state->species<_speciesNames.count ? _speciesNames[state->species] : nil;
    [_condition lock]; BOOL idle=!_stopping && !_failure && !_captureChoicePending && !_captureInputActive && !_captureInputCooling; [_condition unlock];
    uint32_t keys; UIButton *button=_buttons[@1];
    button.hidden=NO; button.enabled=!paused && idle && HGNativeCaptureUIResolveInput(state,state->ownerGeneration,state->revision,&keys); button.alpha=button.enabled ? 1 : 0.5;
    UIButtonConfiguration *config=UIButtonConfiguration.filledButtonConfiguration;
    config.contentInsets=NSDirectionalEdgeInsetsMake(8,12,8,12); config.baseForegroundColor=UIColor.whiteColor;
    config.baseBackgroundColor=[UIColor colorWithWhite:0.13 alpha:0.94]; OriginalSetButtonContent(button,config,nil,@"继续"); button.accessibilityLabel=@"继续";
    _buttons[@2].hidden=YES;
}
- (void)clearEvolutionInputLocked {
    _evolutionChoicePending=_evolutionInputActive=_evolutionInputCooling=NO;
    _evolutionChoiceRevision=_evolutionChoicePhase=_evolutionChoiceKind=_evolutionChoiceIndex=0;
    _evolutionInputRevision=_evolutionInputPhase=_evolutionInputKind=_evolutionInputIndex=_evolutionInputReads=_evolutionReleaseReads=_evolutionInputKeys=_evolutionTouchX=_evolutionTouchY=0;
}
- (NSDictionary *)copyEvolutionPictures:(const HGNativeEvolutionUIState *)state {
    if (!state->active || state->summaryActive) return nil;
    NSMutableArray *choices=[NSMutableArray new];
    for (NSUInteger i=0;i<2;i++) {
        UIImage *image=OriginalWindowImage(&state->choices[i],_env->memory->data,_env->memory->size);
        [choices addObject:image && image.size.width==208 && image.size.height==16 ? image : NSNull.null];
    }
    return @{@"choices":choices};
}
- (void)queueEvolutionButton:(UIButton *)button {
    const HGNativeEvolutionUIState *state=_displayedEvolutionUIState.bytes;
    if (!_evolutionScene || _displayedEvolutionUIState.length!=sizeof(*state) || button.hidden || !button.enabled || (button.tag!=1 && button.tag!=2)) return;
    BOOL choice=(state->actionMask&(HG_EVOLUTION_UI_ACTION_TOP|HG_EVOLUTION_UI_ACTION_BOTTOM))!=0;
    uint32_t kind=choice ? HG_EVOLUTION_UI_INPUT_CHOICE : button.tag==1 ? HG_EVOLUTION_UI_INPUT_ACK : HG_EVOLUTION_UI_INPUT_CANCEL;
    uint32_t index=choice && button.tag==2 ? 1 : 0,keys,x,y;
    if (!HGNativeEvolutionUIResolveInput(state,state->revision,state->phase,kind,index,&keys,&x,&y)) return;
    [_condition lock];
    if (!_paused && !_stopping && !_evolutionChoicePending && !_evolutionInputActive && !_evolutionInputCooling) {
        _evolutionChoiceRevision=state->revision; _evolutionChoicePhase=state->phase;
        _evolutionChoiceKind=kind; _evolutionChoiceIndex=index; _evolutionChoicePending=YES;
    }
    [_condition unlock];
}
- (void)takeEvolutionInputLocked:(const HGNativeEvolutionUIState *)state summary:(BOOL)summary reads:(uint32_t)reads
    keys:(uint32_t *)keys x:(uint32_t *)x y:(uint32_t *)y pressed:(BOOL *)pressed {
    if (!state->active) { [self clearEvolutionInputLocked]; return; }
    _keys=_pendingKeys=_submittedKeys=0; _touchPressed=NO; *keys=0; *pressed=NO;
    if (summary || state->summaryActive) { [self clearEvolutionInputLocked]; return; }
    if (_evolutionInputCooling && reads-_evolutionReleaseReads>=2) _evolutionInputCooling=NO;
    if (_evolutionChoicePending) {
        if (state->revision!=_evolutionChoiceRevision || state->phase!=_evolutionChoicePhase) _evolutionChoicePending=NO;
        else if (state->ready) {
            uint32_t k,tx,ty;
            if (HGNativeEvolutionUIResolveInput(state,_evolutionChoiceRevision,_evolutionChoicePhase,_evolutionChoiceKind,_evolutionChoiceIndex,&k,&tx,&ty)) {
                _evolutionInputActive=YES; _evolutionInputRevision=state->revision; _evolutionInputPhase=state->phase;
                _evolutionInputKind=_evolutionChoiceKind; _evolutionInputIndex=_evolutionChoiceIndex; _evolutionInputReads=reads;
                _evolutionInputKeys=k; _evolutionTouchX=tx; _evolutionTouchY=ty;
            }
            _evolutionChoicePending=NO;
        }
    }
    if (_evolutionInputActive) {
        uint32_t k,tx,ty;
        BOOL release=state->revision!=_evolutionInputRevision || state->phase!=_evolutionInputPhase || reads-_evolutionInputReads>=4;
        // Begin temporarily retires Window leases without advancing this semantic revision.
        if (!release && state->ready) release=!HGNativeEvolutionUIResolveInput(state,_evolutionInputRevision,_evolutionInputPhase,
            _evolutionInputKind,_evolutionInputIndex,&k,&tx,&ty);
        if (release) {
            _evolutionInputActive=NO; _evolutionInputCooling=YES; _evolutionReleaseReads=reads;
            _evolutionInputKeys=_evolutionTouchX=_evolutionTouchY=0;
        } else { *keys=_evolutionInputKeys; *x=_evolutionTouchX; *y=_evolutionTouchY; *pressed=_evolutionInputKind==HG_EVOLUTION_UI_INPUT_CHOICE; }
    }
}
- (void)updateEvolutionControls:(NSData *)data pictures:(NSDictionary *)pictures summary:(BOOL)summary paused:(BOOL)paused {
    const HGNativeEvolutionUIState *state=data.bytes;
    BOOL scene=data.length==sizeof(*state) && state->active && !summary;
    BOOL changed=_evolutionScene!=scene || !_displayedEvolutionUIState || data.length!=_displayedEvolutionUIState.length
        || (scene && memcmp(data.bytes,_displayedEvolutionUIState.bytes,offsetof(HGNativeEvolutionUIState,message)));
    if (_evolutionScene && !scene) {
        [UIView performWithoutAnimation:^{ for (NSNumber *key in @[@1,@2]) { self->_buttons[key].configuration=nil; self->_buttons[key].enabled=YES; self->_buttons[key].alpha=1; [self->_buttons[key] setTitle:nil forState:UIControlStateNormal]; } }];
    }
    _evolutionScene=scene; _displayedEvolutionUIState=data;
    _canvas.isAccessibilityElement=scene; _canvas.accessibilityLabel=scene ? @"原版进化画面" : @"原版游戏画面";
    _canvas.accessibilityValue=scene ? [NSString stringWithFormat:@"%@ → %@",
        state->species<_speciesNames.count ? _speciesNames[state->species] : @"宝可梦",
        state->newSpecies<_speciesNames.count ? _speciesNames[state->newSpecies] : @"宝可梦"] : nil;
    if (changed) { if (scene) [self resetJoystick]; [self.view setNeedsLayout]; }
    if (!scene) return;
    _battleBar.hidden=_partyPanel.hidden=_partyBack.hidden=_bagPanel.hidden=_bagPage.hidden=_summaryPanel.hidden=_battleMenuPanel.hidden=YES;
    for (UIButton *button in _bagNavigation) button.hidden=YES;
    for (UIButton *button in _summaryNavigation) button.hidden=YES;
    BOOL choice=(state->actionMask&(HG_EVOLUTION_UI_ACTION_TOP|HG_EVOLUTION_UI_ACTION_BOTTOM))!=0;
    NSArray *captions=state->phase==HG_EVOLUTION_UI_STOP_LEARNING_MENU ? @[@"放弃学习",@"继续学习"] : @[@"遗忘招式",@"保留原有招式"];
    NSArray *glyphs=pictures[@"choices"];
    [_condition lock]; BOOL idle=!_evolutionChoicePending && !_evolutionInputActive && !_evolutionInputCooling; [_condition unlock];
    for (NSUInteger i=0;i<2;i++) {
        UIButton *button=_buttons[@(i+1)]; uint32_t kind=choice ? HG_EVOLUTION_UI_INPUT_CHOICE : i ? HG_EVOLUTION_UI_INPUT_CANCEL : HG_EVOLUTION_UI_INPUT_ACK;
        uint32_t k,x,y; UIImage *image=choice && i<glyphs.count && [glyphs[i] isKindOfClass:UIImage.class] ? glyphs[i] : nil;
        uint32_t mask=choice ? (i ? HG_EVOLUTION_UI_ACTION_BOTTOM : HG_EVOLUTION_UI_ACTION_TOP) : i ? HG_EVOLUTION_UI_ACTION_CANCEL : HG_EVOLUTION_UI_ACTION_ACK;
        button.hidden=!(state->actionMask&mask) || (choice && !image);
        button.enabled=!paused && idle && !button.hidden && HGNativeEvolutionUIResolveInput(state,state->revision,state->phase,kind,choice ? (uint32_t)i : 0,&k,&x,&y);
        button.alpha=button.enabled ? 1 : 0.5;
        UIButtonConfiguration *config=UIButtonConfiguration.filledButtonConfiguration;
        config.contentInsets=NSDirectionalEdgeInsetsMake(8,12,8,12); config.baseForegroundColor=UIColor.whiteColor;
        config.baseBackgroundColor=[UIColor colorWithWhite:0.13 alpha:0.94]; button.imageView.layer.magnificationFilter=kCAFilterNearest;
        NSString *caption=choice ? captions[i] : i ? @"停止进化" : @"继续";
        OriginalSetButtonContent(button,config,image,caption); button.accessibilityLabel=caption;
    }
}
- (void)keyDown:(OriginalKeyButton *)sender {
    sender.inputGeneration++;
    if (_trainerCardScene) {
        [_condition lock];BOOL resume=sender.tag==8 && _interrupted && _interruptionEnded && !_inactive;
        if (resume) {_interrupted=_interruptionEnded=NO;_paused=NO;[_condition signal];}[_condition unlock];
        if (!resume) [self queueTrainerCardKey:sender];return;
    }
    if (_pcBoxScene) {
        if (sender.hidden || !sender.enabled) return;
        [_condition lock]; BOOL resume=sender.tag==8 && _interrupted && _interruptionEnded && !_inactive;
        if (resume) {_interrupted=_interruptionEnded=NO;_paused=NO;[_condition signal];} [_condition unlock];
        if (!resume) [self queuePCBoxKey:sender]; return;
    }
    if (_optionsScene) {
        [_condition lock]; BOOL resume=sender.tag==8 && _interrupted && _interruptionEnded && !_inactive;
        if (resume) {_interrupted=_interruptionEnded=NO;_paused=NO;[_condition signal];} [_condition unlock];
        if (!resume) [self queueOptionsKey:sender]; return;
    }
    if (_namingScene) {
        if (sender.hidden || !sender.enabled) return;
        [_condition lock]; BOOL resume=sender.tag==8 && _interrupted && _interruptionEnded && !_inactive;
        if (resume) {_interrupted=_interruptionEnded=NO;_paused=NO;[_condition signal];} [_condition unlock];
        if (!resume) [self queueNamingInputKeys:sender.tag==0x400 ? 4 : (uint32_t)sender.tag touch:NO point:CGPointZero];
        return;
    }
    if (_fieldChoiceScene) {
        if (sender.hidden || !sender.enabled) return;
        [_condition lock]; BOOL resume=sender.tag==8 && _interrupted && _interruptionEnded && !_inactive;
        if (resume) {_interrupted=_interruptionEnded=NO;_paused=NO;[_condition signal];} [_condition unlock];
        if (!resume) [self queueFieldChoiceKey:sender]; return;
    }
    if (_captureScene && sender.tag!=8) { if (sender.tag==1) [self queueCaptureButton:sender]; return; }
    if (_pokedexScene && sender.tag!=8) {
        uint32_t kind=sender.tag==1 ? HG_POKEDEX_UI_INPUT_CONFIRM : sender.tag==2 ? HG_POKEDEX_UI_INPUT_BACK : HG_POKEDEX_UI_INPUT_DIRECTION;
        uint32_t index=sender.tag==64 ? 0 : sender.tag==128 ? 1 : sender.tag==32 ? 2 : 3;
        if (sender.tag==1 || sender.tag==2 || sender.tag==16 || sender.tag==32 || sender.tag==64 || sender.tag==128)
            [self queuePokedexInput:kind index:kind==HG_POKEDEX_UI_INPUT_DIRECTION ? index : 0];
        return;
    }
    if (_evolutionScene && (sender.tag==1 || sender.tag==2)) { [self queueEvolutionButton:sender]; return; }
    if (_startMenuScene && _startMenuBlocksInput) {
        [_condition lock];BOOL resume=sender.tag==8 && _interrupted && _interruptionEnded && !_inactive;
        if (resume) {_interrupted=_interruptionEnded=NO;_paused=NO;[_condition signal];}[_condition unlock];
        if (!resume && !sender.hidden && sender.enabled) [self queueStartMenuInputKeys:sender.tag touch:NO point:CGPointZero];
        return;
    }
    [_condition lock];
    if (_interrupted && _interruptionEnded && !_inactive) {
        _interrupted = _interruptionEnded = NO; _paused = NO; [_condition signal];
    } else if (!_paused && !_captureScene) { _keys |= sender.tag; _pendingKeys |= sender.tag; }
    [_condition unlock];
}
- (void)keyUp:(UIButton *)sender { if ((_startMenuScene && _startMenuBlocksInput) || _trainerCardScene || _optionsScene || _namingScene || _pcBoxScene || _fieldChoiceScene) return; [_condition lock]; _keys &= ~sender.tag; [_condition unlock]; }
- (void)keyCancel:(UIButton *)sender { if ((_startMenuScene && _startMenuBlocksInput) || _trainerCardScene || _optionsScene || _namingScene || _pcBoxScene || _fieldChoiceScene) return; [_condition lock]; _keys &= ~sender.tag; _pendingKeys &= ~sender.tag; _submittedKeys &= ~sender.tag; [_condition unlock]; }
- (void)acknowledgeKeysLocked:(uint32_t)reads {
    // Display callbacks can outpace original PAD_Read; keep submitted taps until it samples them.
    if (reads==_lastPadReadCount) return;
    _pendingKeys &= ~_submittedKeys; _pendingJoystickKeys &= ~_submittedJoystickKeys;
    _submittedKeys = _submittedJoystickKeys = 0; _lastPadReadCount=reads;
}
- (uint32_t)takeKeysLocked {
    uint32_t keys = _keys | _pendingKeys;
    if (_fieldMapId && !_selectedEngine && !_fieldMenuOpen && !_startMenuScene) keys |= _joystickKeys | _pendingJoystickKeys;
    _submittedKeys=_pendingKeys;
    _submittedJoystickKeys=_fieldMapId && !_selectedEngine && !_fieldMenuOpen && !_startMenuScene ? _pendingJoystickKeys : 0;
    return keys;
}
- (void)resetJoystick {
    [_condition lock]; _joystickKeys = _pendingJoystickKeys = _submittedJoystickKeys = 0; _joystickNeedsReset = NO; [_condition unlock];
    _joystickThumb.center=CGPointMake(CGRectGetMidX(_joystick.bounds),CGRectGetMidY(_joystick.bounds));
    _joystick.accessibilityValue=@"停止";
}
- (void)joystickAt:(CGPoint)point pressed:(BOOL)pressed {
    CGPoint center=CGPointMake(CGRectGetMidX(_joystick.bounds),CGRectGetMidY(_joystick.bounds));
    CGFloat dx=point.x-center.x,dy=point.y-center.y,travel=MAX(1,MIN(_joystick.bounds.size.width,_joystick.bounds.size.height)/2-24);
    CGFloat length=hypot(dx,dy);
    if (!isfinite(length)) { [self resetJoystick]; return; }
    uint32_t direction=0;
    if (pressed && length>=travel*0.2) direction=fabs(dy)>=fabs(dx) ? (dy<0 ? 0x40 : 0x80) : (dx<0 ? 0x20 : 0x10);
    [_condition lock];
    BOOL allowed=_fieldMapId && !_selectedEngine && !_fieldMenuOpen && !_startMenuScene && !_paused && !_stopping && !_battleMode && !_evolutionScene && !_captureScene && !_namingScene && !_optionsScene && !_pcBoxOwnerGeneration && !_fieldChoiceOwnerGeneration;
    if (!allowed) { direction=0; _pendingJoystickKeys=0; }
    _joystickKeys=direction;
    if (pressed && allowed) { _pendingJoystickKeys=direction; _joystickNeedsReset=NO; }
    [_condition unlock];
    CGFloat scale=pressed && allowed ? MIN(1,travel/MAX(1,length)) : 0;
    _joystickThumb.center=CGPointMake(center.x+dx*scale,center.y+dy*scale);
    _joystick.accessibilityValue=direction==0x40 ? @"向上" : direction==0x80 ? @"向下" : direction==0x20 ? @"向左" : direction==0x10 ? @"向右" : @"停止";
}
- (void)joystickChanged:(OriginalJoystick *)stick {
    if (stick.cancelled) [self resetJoystick];
    else [self joystickAt:stick.location pressed:stick.pressed];
}
- (void)toggleMenu {
    if (_optionsScene || _namingScene || _captureScene || _evolutionScene || _pokedexScene || _pcBoxOwnerGeneration || _fieldChoiceScene) return;
    if (_startMenuScene) {if (_startMenuBlocksInput) [self queueStartMenuInputKeys:2 touch:NO point:CGPointZero];return;}
    [_condition lock];
    if (_fieldMapId) {
        _fieldMenuOpen=!_fieldMenuOpen; _automaticMenuEngine=_fieldMenuOpen; _selectedEngine=0; _fieldMenuFrame=nil;
        _keys=_pendingKeys=_submittedKeys=0;
        if (!_fieldMenuOpen) _pendingKeys=2;
        _fieldMenu.image=nil;
    } else { _automaticMenuEngine=NO; _selectedEngine ^= 1; }
    _touchPressed = NO; uint32_t engine = _selectedEngine;
    NSString *menuTitle=_fieldMapId ? (_fieldMenuOpen ? @"关闭菜单" : @"菜单") : (engine ? @"返回场景" : @"触摸菜单"); [_condition unlock];
    [self resetJoystick];
    OriginalSetButtonContent(_menuButton,nil,nil,menuTitle);
    _menuButton.accessibilityLabel=menuTitle;
}
- (void)touchAt:(CGPoint)point pressed:(BOOL)pressed {
    if (_optionsScene) return;
    if (_namingScene) {if (pressed) [self queueNamingInputKeys:0 touch:YES point:point];return;}
    if (_pcBoxOwnerGeneration || _fieldChoiceScene) return;
    if (_startMenuScene && _startMenuBlocksInput) {
        if (pressed && !_fieldMenu.hidden && _fieldMenu.userInteractionEnabled && _fieldMenu.bounds.size.width>0 && _fieldMenu.bounds.size.height>0
            && CGRectContainsPoint(_fieldMenu.bounds,point))
            [self queueStartMenuInputKeys:0 touch:YES point:CGPointMake(point.x*256/_fieldMenu.bounds.size.width,point.y*192/_fieldMenu.bounds.size.height)];
        return;
    }
    [_condition lock];
    _inputGeneration++;
    BOOL fieldMenu=(_fieldMenuOpen || _startMenuScene) && _fieldMapId;
    UIView *surface=fieldMenu ? _fieldMenu : _canvas;
    _touchX = (uint32_t)MAX(0, MIN(255, point.x * 256 / MAX(1, surface.bounds.size.width)));
    _touchY = (uint32_t)MAX(0, MIN(191, point.y * 192 / MAX(1, surface.bounds.size.height)));
    _touchPressed = pressed && !_paused && !_partyScene && !(_bagScene && !_bagPanel.hidden) && !(_summaryScene && !_summaryPanel.hidden)
        && !_captureScene && !_evolutionScene && !_pokedexScene && (fieldMenu || _selectedEngine == _touchEngine);
    [_condition unlock];
}
- (void)canvasTouch:(UIPanGestureRecognizer *)gesture {
    if (_optionsScene) return;
    if (_namingScene) {if (gesture.state==UIGestureRecognizerStateBegan) [self tapAt:[gesture locationInView:gesture.view]];return;}
    if (_pcBoxOwnerGeneration || _fieldChoiceScene) return;
    if (_startMenuScene && _startMenuBlocksInput) {if (gesture.state==UIGestureRecognizerStateBegan) [self touchAt:[gesture locationInView:gesture.view] pressed:YES];return;}
    [self touchAt:[gesture locationInView:gesture.view] pressed:gesture.state == UIGestureRecognizerStateBegan || gesture.state == UIGestureRecognizerStateChanged];
}
- (void)canvasTap:(UITapGestureRecognizer *)gesture {
    [self tapAt:[gesture locationInView:gesture.view]];
}
- (void)tapAt:(CGPoint)point {
    [self touchAt:point pressed:YES];
    NSUInteger generation=_inputGeneration;
    dispatch_block_t release=^{ if (generation==self->_inputGeneration) [self touchAt:CGPointZero pressed:NO]; };
#ifdef HEARTGOLD_ORIGINAL_APP_CHECK
    if (_checkInputCompletions) [_checkInputCompletions addObject:release]; else
#endif
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW,120*NSEC_PER_MSEC),dispatch_get_main_queue(),release);
}
- (void)updatePauseLocked {
    _paused = _inactive || _interrupted;
    if (_paused) { _keys = _pendingKeys = _submittedKeys = _joystickKeys = _pendingJoystickKeys = _submittedJoystickKeys = 0; _joystickNeedsReset = YES; _touchPressed = NO; _battleChoicePending = _partyChoicePending = _partyTouchActive = _bagChoicePending = _bagTouchActive = _summaryChoicePending = _summaryInputActive = _summaryTouchPressed = NO; _summaryInputKeys=0; } else [_condition signal];
    if (_paused) {
        [self clearStartMenuInputLocked];
        [self clearTrainerCardInputLocked];
#ifdef HEARTGOLD_ORIGINAL_PC_BOX_CHECK
        [self clearPCBoxCheckTransientLocked:YES];
#endif
        [self clearPCBoxInputLocked];
        if (_pcBoxOwnerGeneration) {_pcBoxInputCooling=YES;_pcBoxReleaseReads=_lastPadReadCount;}
        [self clearFieldChoiceInputLocked];
        if (_fieldChoiceOwnerGeneration) {_fieldChoiceInputCooling=YES;_fieldChoiceReleaseReads=_lastPadReadCount;}
        [self clearOptionsInputLocked];
        if (_optionsOwnerGeneration) {_optionsInputCooling=YES;_optionsReleaseReads=_lastPadReadCount;}
        [self clearNamingInputLocked];
        if (_namingOwnerGeneration) {_namingInputCooling=YES;_namingReleaseReads=_lastPadReadCount;}
        [self clearCaptureInputLocked];
        [self clearEvolutionInputLocked];
        [self clearPokedexInputLocked];
        _battleMenuChoicePending=_battleMenuTouchActive=_battleMenuTouchCooling=NO;
#ifdef HEARTGOLD_ORIGINAL_APP_CHECK
        _checkBattleMenusRequestPending=NO;
#ifdef HEARTGOLD_ORIGINAL_CAPTURE_REGISTRATION_CHECK
        _captureQARequestPending=_captureQARequestInFlight=NO;
        _captureQARequestRevision=_captureQARequestVariant=0;
#endif
#ifdef HEARTGOLD_ORIGINAL_SUMMARY_TM_CHECK
        [self clearSummaryTMTransientLocked];
#endif
#endif
    }
}
- (void)pause { [_condition lock]; _inactive = YES; [self updatePauseLocked]; [_condition unlock]; }
- (void)resume { [_condition lock]; _inactive = NO; [self updatePauseLocked]; [_condition unlock]; }
- (void)audioInterruption:(NSNotification *)notification {
    AVAudioSessionInterruptionType type = [notification.userInfo[AVAudioSessionInterruptionTypeKey] unsignedIntegerValue];
    [_condition lock];
    if (type == AVAudioSessionInterruptionTypeBegan) { _interrupted = YES; _interruptionEnded = NO; }
    else if (type == AVAudioSessionInterruptionTypeEnded) {
        _interruptionEnded = YES;
        if ([notification.userInfo[AVAudioSessionInterruptionOptionKey] unsignedIntegerValue] & AVAudioSessionInterruptionOptionShouldResume) _interrupted = NO;
    }
    [self updatePauseLocked]; [_condition unlock];
}
- (void)audioRouteChanged:(NSNotification *)notification {
    if ([notification.userInfo[AVAudioSessionRouteChangeReasonKey] unsignedIntegerValue] != AVAudioSessionRouteChangeReasonOldDeviceUnavailable) return;
    [_condition lock]; _interrupted = _interruptionEnded = YES; [self updatePauseLocked]; [_condition unlock];
}
- (void)stop {
    [_condition lock];
    [self clearStartMenuInputLocked];
#ifdef HEARTGOLD_ORIGINAL_PC_BOX_CHECK
    [self clearPCBoxCheckTransientLocked:NO];
#endif
    [self clearTrainerCardInputLocked]; [self clearFieldChoiceInputLocked]; [self clearPCBoxInputLocked]; [self clearOptionsInputLocked]; [self clearNamingInputLocked]; [self clearCaptureInputLocked]; [self clearEvolutionInputLocked]; [self clearPokedexInputLocked]; _stopping = YES; _partyChoicePending = _partyTouchActive = _bagChoicePending = _bagTouchActive = _summaryChoicePending = _summaryInputActive = _summaryTouchPressed = NO; _summaryInputKeys=0; _inputGeneration++;
    _battleMenuChoicePending=_battleMenuTouchActive=_battleMenuTouchCooling=NO;
#ifdef HEARTGOLD_ORIGINAL_APP_CHECK
    _checkBattleMenusRequestPending=NO;
#ifdef HEARTGOLD_ORIGINAL_CAPTURE_REGISTRATION_CHECK
        _captureQARequestPending=_captureQARequestInFlight=NO;
        _captureQARequestRevision=_captureQARequestVariant=0;
#endif
#ifdef HEARTGOLD_ORIGINAL_SUMMARY_TM_CHECK
        [self clearSummaryTMTransientLocked];
#endif
#endif
    [_condition signal]; [_condition unlock];
    for (OriginalKeyButton *button in _buttons.allValues) button.inputGeneration++;
    [_displayLink invalidate]; _displayLink = nil;
    [NSNotificationCenter.defaultCenter removeObserver:self];
}
- (void)displayFrame {
    [_condition lock]; NSData *frame = _frame; uint32_t frameWidth=_frameWidth,frameEngine=_frameEngine; _frame = nil; NSString *failure = _failure; NSData *battleUI = _battleUIState;
    BOOL interrupted = _interrupted, ended = _interruptionEnded,paused = _paused,resetStick = _joystickNeedsReset;
    uint32_t fieldMap=_fieldMapId,icons=_fieldMenuIcons;
    BOOL menuOpen=_fieldMenuOpen; NSData *menuFrame=_fieldMenuFrame; _fieldMenuFrame=nil;
    NSData *partyUI=_partyUIState,*bagUI=_bagUIState,*summaryUI=_summaryUIState,*evolutionUI=_evolutionUIState,*pokedexUI=_pokedexUIState,*optionsUI=_optionsUIState,*namingUI=_namingUIState,*captureUI=_captureUIState,*pcBoxUI=_pcBoxUIState;
    NSData *fieldChoiceUI=_fieldChoiceUIState; UIImage *fieldChoicePreview=_fieldChoicePreviewImage;
    NSData *trainerUI=_trainerCardUIState,*trainerRaw=_trainerCardRawUIState;NSDictionary *trainerPictures=_trainerCardPictures;
    NSData *startMenuUI=_startMenuUIState,*startMenuRaw=_startMenuRawUIState,*startMenuMain=_startMenuMainFrame,*startMenuSub=_startMenuSubFrame;
    uint32_t startMenuFrameEngine=_startMenuFrameEngine,startMenuRawEngine=_startMenuRawEngine;
#ifdef HEARTGOLD_ORIGINAL_PC_BOX_CHECK
    NSData *pcQAAtRead=_pcBoxQAState;
    uint32_t pcPadAtRead=_pcBoxQAPadReads,pcOwnerFrameAtRead=_pcBoxQAOwnerFrame,pcPublishedAtRead=_pcBoxQAPublishedFrame;
#endif
#ifdef HEARTGOLD_ORIGINAL_APP_CHECK
    uint32_t checkFrameAtRead=_publishedFrames;
#ifdef HEARTGOLD_ORIGINAL_DEFAULT_CHECK
    NSData *battleMenusQAAtRead=_battleMenusQAState; BOOL paletteFadeFinishedAtRead=_paletteFadeFinished;
#endif
#ifdef HEARTGOLD_ORIGINAL_CAPTURE_REGISTRATION_CHECK
    NSData *captureQAAtRead=_captureQAState; uint32_t capturePadAtRead=_captureQAPadReads;
#endif
#endif
    NSData *battleBagUI=_battleBagUIState,*battlePartyUI=_battlePartyUIState;
    UIImage *optionsPreview=_optionsPreviewImage;
    NSDictionary *partyPictures=_partyPictures,*bagPictures=_bagPictures,*summaryPictures=_summaryPictures,*battleMenuPictures=_battleMenuPictures,*evolutionPictures=_evolutionPictures,*pokedexPictures=_pokedexPictures,*pcBoxPictures=_pcBoxPictures; [_condition unlock];
    if (frame) _displayedEngine=frameEngine;
    uint32_t engine=_displayedEngine;
    BOOL trainerContext=trainerRaw.length==sizeof(HGNativeTrainerCardUIState) && ((const HGNativeTrainerCardUIState *)trainerRaw.bytes)->active==1;
    BOOL startMenuBlocked=trainerContext
        || (optionsUI.length==sizeof(HGNativeOptionsUIState) && ((const HGNativeOptionsUIState *)optionsUI.bytes)->active)
        || (namingUI.length==sizeof(HGNativeNamingUIState) && ((const HGNativeNamingUIState *)namingUI.bytes)->active)
        || (captureUI.length==sizeof(HGNativeCaptureUIState) && ((const HGNativeCaptureUIState *)captureUI.bytes)->active)
        || (partyUI.length==sizeof(HGNativePartyUIState) && ((const HGNativePartyUIState *)partyUI.bytes)->active)
        || (bagUI.length==sizeof(HGNativeBagUIState) && ((const HGNativeBagUIState *)bagUI.bytes)->active)
        || (summaryUI.length==sizeof(HGNativeSummaryUIState) && ((const HGNativeSummaryUIState *)summaryUI.bytes)->active)
        || (pcBoxUI.length==sizeof(HGNativePCBoxUIState) && ((const HGNativePCBoxUIState *)pcBoxUI.bytes)->active)
        || (evolutionUI.length==sizeof(HGNativeEvolutionUIState) && ((const HGNativeEvolutionUIState *)evolutionUI.bytes)->active)
        || (pokedexUI.length==sizeof(HGNativePokedexUIState) && ((const HGNativePokedexUIState *)pokedexUI.bytes)->active)
        || (battleUI.length==sizeof(HGNativeBattleUIState) && ((const HGNativeBattleUIState *)battleUI.bytes)->active)
        || (battleBagUI.length==sizeof(HGNativeBattleBagUIState) && ((const HGNativeBattleBagUIState *)battleBagUI.bytes)->active)
        || (battlePartyUI.length==sizeof(HGNativeBattlePartyUIState) && ((const HGNativeBattlePartyUIState *)battlePartyUI.bytes)->active)
        || (fieldChoiceUI.length==sizeof(HGNativeFieldChoiceUIState) && ((const HGNativeFieldChoiceUIState *)fieldChoiceUI.bytes)->active && HGNativeFieldChoiceUIIsViewPhase(((const HGNativeFieldChoiceUIState *)fieldChoiceUI.bytes)->phase));
    const HGNativeFieldMenuUIState *startMenu=startMenuRaw.length==sizeof(*startMenu) ? startMenuRaw.bytes : NULL;
    BOOL startMenuContext=startMenu && startMenu->active==1 && !startMenu->childActive && !startMenuBlocked;
    if (!startMenuContext) [self updateStartMenuControls:startMenuRaw main:nil sub:nil engine:startMenuRawEngine blocked:YES paused:paused];
    if (!trainerContext) [self updateTrainerCardControls:trainerRaw pictures:nil paused:paused];
    BOOL optionsContext=!trainerContext && optionsUI.length==sizeof(HGNativeOptionsUIState) && ((const HGNativeOptionsUIState *)optionsUI.bytes)->active==1;
    if (!optionsContext) [self updateOptionsControls:optionsUI preview:nil paused:paused];
    BOOL namingContext=!trainerContext && !optionsContext && namingUI.length==sizeof(HGNativeNamingUIState) && ((const HGNativeNamingUIState *)namingUI.bytes)->active==1;
    BOOL captureContext=!trainerContext && !optionsContext && !namingContext && captureUI.length==sizeof(HGNativeCaptureUIState) && ((const HGNativeCaptureUIState *)captureUI.bytes)->active==1;
    if (!namingContext) [self updateNamingControls:namingUI paused:paused];
    if (!captureContext) [self updateCaptureControls:captureUI paused:paused];
    [self updateBattleControls:battleUI];
    [self updatePartyControls:partyUI pictures:partyPictures paused:paused];
    [self updateBagControls:bagUI pictures:bagPictures paused:paused];
    [self updateSummaryControls:summaryUI pictures:summaryPictures paused:paused];
    const HGNativeSummaryUIState *summary=summaryUI.length==sizeof(HGNativeSummaryUIState) ? summaryUI.bytes : NULL;
    BOOL summaryLearning=summary && summary->active && HGNativeSummaryUIIsLearning(summary);
    [self updateBattleMenuControls:battleBagUI party:battlePartyUI pictures:battleMenuPictures paused:paused engine:engine];
    BOOL summaryContext=!trainerContext && !optionsContext && !namingContext && summary && summary->active && (summaryLearning || HGNativeSummaryUIIsOrdinary(summary));
    [self updateEvolutionControls:evolutionUI pictures:evolutionPictures summary:summaryContext || captureContext || namingContext || optionsContext paused:paused];
    BOOL pcBoxActive=pcBoxUI.length==sizeof(HGNativePCBoxUIState) && ((const HGNativePCBoxUIState *)pcBoxUI.bytes)->active==1;
    BOOL bagActive=bagUI.length==sizeof(HGNativeBagUIState) && ((const HGNativeBagUIState *)bagUI.bytes)->active;
    BOOL pcBoxBlocked=trainerContext || optionsContext || namingContext || captureContext || (summary && summary->active) || bagActive || _partyScene || _evolutionScene || _battleScene || _battleMenuScene;
    BOOL pcBoxContext=pcBoxActive && !pcBoxBlocked;
    if (!pcBoxContext) [self updatePCBoxControls:pcBoxUI pictures:pcBoxPictures blocked:pcBoxBlocked paused:paused];
    const HGNativeFieldChoiceUIState *fieldChoice=fieldChoiceUI.length==sizeof(HGNativeFieldChoiceUIState) ? fieldChoiceUI.bytes : NULL;
    BOOL fieldChoiceBlocked=trainerContext || optionsContext || namingContext || captureContext || summaryContext || pcBoxActive || _evolutionScene || _battleScene || _battleMenuScene || _partyScene || _bagScene
        || (pokedexUI.length==sizeof(HGNativePokedexUIState) && ((const HGNativePokedexUIState *)pokedexUI.bytes)->active);
    BOOL fieldChoiceContext=fieldChoice && fieldChoice->active==1 && HGNativeFieldChoiceUIIsViewPhase(fieldChoice->phase) && !fieldChoiceBlocked;
    if (!fieldChoiceContext) [self updateFieldChoiceControls:fieldChoiceUI preview:nil blocked:fieldChoiceBlocked paused:paused];
    BOOL pokedexBlocked=trainerContext || optionsContext || namingContext || captureContext || summaryContext || _evolutionScene || _battleScene || _battleMenuScene || _partyScene || _bagScene || pcBoxActive || fieldChoiceContext;
    [self updatePokedexControls:pokedexUI pictures:pokedexPictures blocked:pokedexBlocked paused:paused];
    if (captureContext) [self updateCaptureControls:captureUI paused:paused];
    if (frame) {
        if (_displayedWidth!=frameWidth) { _displayedWidth=frameWidth; [self.view setNeedsLayout]; }
        _canvas.image = OriginalPixelImage(frame,frameWidth,192);
        _status.hidden = YES;
#ifdef HEARTGOLD_ORIGINAL_DEFAULT_CHECK
        _displayedBattleMenusQAState=battleMenusQAAtRead; _displayedPaletteFadeFinished=paletteFadeFinishedAtRead;
#endif
    }
    BOOL field=fieldMap && !engine && !_battleMode && !_battleScene && !_partyScene && !_bagScene && !_summaryScene && !_battleMenuScene && !_evolutionScene && !_pokedexScene && !_captureScene && !namingContext && !optionsContext && !pcBoxActive && !fieldChoiceContext;
    if (!field) _fieldMenu.image=nil;
    else if (menuFrame) _fieldMenu.image=OriginalPixelImage(menuFrame,256,192);
    BOOL panel=field && menuOpen && _fieldMenu.image;
    _fieldMenu.hidden=_fieldMenuShade.hidden=!panel; _fieldMenu.userInteractionEnabled=!paused;
    _canvas.userInteractionEnabled=!(field && menuOpen) && !_partyScene && !(_bagScene && !_bagPanel.hidden) && !(_summaryScene && !_summaryPanel.hidden) && !summaryLearning && !_battleMenuKind && !_evolutionScene && !_pokedexScene && !_captureScene && !namingContext && !optionsContext && !pcBoxActive && !fieldChoiceContext;
    NSMutableArray *visible=[NSMutableArray new];
    NSArray *auxNames=@[@"已登记道具 1",@"已登记道具 2",@"跑鞋开关",@"互动"];
    const unsigned auxBits[]={29,30,28,31};
    for (NSUInteger i=0;i<_fieldMenuElements.count;i++) {
        unsigned icon=i<7 ? (icons>>(i*4))&15 : 13;
        BOOL enabled=i<7 ? icon<_fieldMenuNames.count : (icons&(1u<<auxBits[i-7]))!=0;
        if (enabled) {
            _fieldMenuElements[i].accessibilityLabel=i<7 ? (icon==4 ? @"训练家卡片" : _fieldMenuNames[icon]) : auxNames[i-7];
            [visible addObject:_fieldMenuElements[i]];
        }
    }
    _fieldMenu.accessibilityElements=visible;
    BOOL stick=field && !menuOpen;
    if (_joystick.hidden==stick || resetStick) [self resetJoystick];
    _joystick.hidden=!stick; _joystick.userInteractionEnabled=!paused;
    BOOL nativeMenu=(_partyScene && !_partyPanel.hidden) || (_bagScene && !_bagPanel.hidden) || (_summaryScene && !_summaryPanel.hidden) || (_battleMenuKind && !_battleMenuPanel.hidden);
    _canvas.hidden=!_captureScene && (nativeMenu || _pokedexScene);
    for (NSNumber *key in @[@0x40,@0x80,@0x20,@0x10]) _buttons[key].hidden=_battleMode || field || nativeMenu || summaryLearning || _evolutionScene || _captureScene;
    if (!_evolutionScene && !_captureScene) {
        _buttons[@1].hidden=_battleMode || (field && menuOpen) || nativeMenu || summaryLearning;
        _buttons[@2].hidden=_battleMode || (field && menuOpen) || nativeMenu || summaryLearning;
    }
    _buttons[@0x400].hidden=_battleMode || field || _partyScene || _bagScene || _summaryScene || _battleMenuScene || _evolutionScene || _pokedexScene || _captureScene;
    _buttons[@8].hidden=(_captureScene || _evolutionScene) ? !(interrupted && ended) : _battleMode || ((field || _partyScene || _bagScene || _summaryScene || _battleMenuScene) && !(interrupted && ended));
    _menuButton.hidden=_battleMode || _partyScene || _bagScene || _summaryScene || _battleMenuScene || _evolutionScene || _pokedexScene || _captureScene;
    if (!_evolutionScene && !_captureScene) {
        [_buttons[@1] setTitle:field ? @"互动" : @"确认" forState:UIControlStateNormal];
        [_buttons[@2] setTitle:field ? @"奔跑" : @"返回" forState:UIControlStateNormal];
        _buttons[@1].accessibilityLabel=_buttons[@1].currentTitle;
        _buttons[@2].accessibilityLabel=_buttons[@2].currentTitle;
    }
    OriginalSetButtonContent(_menuButton,nil,nil,field ? (menuOpen ? @"关闭菜单" : @"菜单") : (engine ? @"返回场景" : @"触摸菜单"));
    _menuButton.accessibilityLabel=_menuButton.currentTitle;
    if (_pokedexScene) [self layoutPokedexControls:UIEdgeInsetsInsetRect(self.view.bounds,self.view.safeAreaInsets)];
    if (pcBoxContext) [self updatePCBoxControls:pcBoxUI pictures:pcBoxPictures blocked:NO paused:paused];
    if (fieldChoiceContext) [self updateFieldChoiceControls:fieldChoiceUI preview:fieldChoicePreview blocked:NO paused:paused];
    if (namingContext) [self updateNamingControls:namingUI paused:paused];
    if (optionsContext) [self updateOptionsControls:optionsUI preview:optionsPreview paused:paused];
    if (trainerContext) {
        BOOL trainerPaired=trainerUI.length==trainerRaw.length && trainerUI.length==sizeof(HGNativeTrainerCardUIState) && !memcmp(trainerUI.bytes,trainerRaw.bytes,trainerUI.length);
        [self updateTrainerCardControls:trainerPaired ? trainerUI : trainerRaw pictures:trainerPaired ? trainerPictures : nil paused:paused];
    }
    if (startMenuContext) {
        BOOL paired=startMenuUI.length==sizeof(*startMenu) && !memcmp(startMenuUI.bytes,startMenuRaw.bytes,sizeof(*startMenu)) && startMenuFrameEngine==startMenuRawEngine;
        [self updateStartMenuControls:paired ? startMenuUI : startMenuRaw main:paired ? startMenuMain : nil sub:paired ? startMenuSub : nil engine:startMenuRawEngine blocked:NO paused:paused];
    }
    if (failure) { _status.text = failure; _status.hidden = NO; }
    else if (interrupted) { _status.text = ended ? @"已暂停，点开始继续" : @"声音中断，已暂停"; _status.hidden = NO; }
#ifdef HEARTGOLD_ORIGINAL_APP_CHECK
#if defined(HEARTGOLD_ORIGINAL_PC_BOX_CHECK)
    _displayedPCBoxQAState=pcQAAtRead; _displayedPCBoxQAPadReads=pcPadAtRead;
    _displayedPCBoxQAOwnerFrame=pcOwnerFrameAtRead; _pcBoxQADisplayedPublishedFrame=pcPublishedAtRead;
    [self checkPCBoxUIKitDisplay];
#elif defined(HEARTGOLD_ORIGINAL_CAPTURE_REGISTRATION_CHECK)
    _displayedCaptureQAState=captureQAAtRead; _displayedCaptureQAPadReads=capturePadAtRead;
    [self checkCaptureRegistrationDisplay:checkFrameAtRead];
#else
    [self checkPrivateCaseDisplay:checkFrameAtRead];
#endif
#endif
}
#ifdef HEARTGOLD_ORIGINAL_APP_CHECK
- (void)publishAudioProducerTrace {
    uint64_t firstShortTimeNs=[_audio firstShortTimeNs];
    HGNativeAudioCheckSnapshot snapshot;
    if (!HGNativeAudioCheckGetSnapshot(&snapshot)) return;
    uint64_t capturedAtNs=HGNativeAudioCheckNow();
    NSData *data=[NSData dataWithBytes:&snapshot length:sizeof(snapshot)];
    [_condition lock]; _audioProducerTrace=data; _audioProducerCapturedAtNs=capturedAtNs;
    // Include a completed audio service after the shortage so the retained history covers recovery.
    if (firstShortTimeNs && snapshot.lastServiceEndNs>=firstShortTimeNs && !_audioFirstShortTrace) {
        _audioFirstShortTrace=data; _audioFirstShortTimeNs=firstShortTimeNs; _audioFirstShortCapturedAtNs=capturedAtNs;
    }
    [_condition unlock];
}
- (NSDictionary *)checkAudioProducerSnapshot {
    [_condition lock]; NSData *data=_audioProducerTrace, *firstShort=_audioFirstShortTrace;
    uint64_t capturedAtNs=_audioProducerCapturedAtNs, firstShortTimeNs=_audioFirstShortTimeNs, firstShortCapturedAtNs=_audioFirstShortCapturedAtNs;
    [_condition unlock];
    NSMutableDictionary *result=[CheckAudioTrace(data,capturedAtNs) mutableCopy];
    result[@"firstShortCapture"]=@{@"timeNs":@(firstShortTimeNs),@"producer":CheckAudioTrace(firstShort,firstShortCapturedAtNs)};
    return result;
}
#endif
#ifdef HEARTGOLD_ORIGINAL_SUMMARY_TM_CHECK
- (const void *)summaryTMGuest:(uint32_t)address size:(uint64_t)size {
    if (!_env || !_env->memory || !address || address>_env->memory->size || size>_env->memory->size-address) wasm_rt_trap(WASM_RT_TRAP_OOB);
    return _env->memory->data+address;
}
- (BOOL)readSummaryTMQA:(HGSummaryLearnQAState *)state {
    uint32_t address=w2c_hgengine_HGNativeSummaryLearnQAGetState(_game);
    memcpy(state,[self summaryTMGuest:address size:sizeof(*state)],sizeof(*state));
    if (!state->version && !state->byteSize && !_summaryTMObserverSeen) return NO;
    if (state->version!=HG_SUMMARY_LEARN_QA_VERSION || state->byteSize!=sizeof(*state)) wasm_rt_trap(WASM_RT_TRAP_OOB);
    _summaryTMObserverSeen=YES; return YES;
}
- (BOOL)summaryTMInputIdleLocked {
    return !_keys && !_pendingKeys && !_submittedKeys && !_joystickKeys && !_pendingJoystickKeys && !_submittedJoystickKeys && !_touchPressed
        && !_battleChoicePending && !_partyChoicePending && !_partyTouchActive && !_bagChoicePending && !_bagTouchActive
        && !_summaryChoicePending && !_summaryInputActive && !_summaryTouchPressed && !_summaryInputKeys
        && !_evolutionChoicePending && !_evolutionInputActive && !_evolutionInputCooling
        && !_captureChoicePending && !_captureInputActive && !_captureInputCooling
        && !_pokedexChoicePending && !_pokedexInputActive && !_pokedexInputCooling && !_pokedexGestureLive && !_pokedexGestureAdmitted
        && !_battleMenuChoicePending && !_battleMenuTouchActive && !_battleMenuTouchCooling
        && !_summaryTMRawPending && !_summaryTMRawActive && !_summaryTMRawCooling;
}
- (BOOL)queueSummaryTMFixture:(uint32_t)revision {
    [_condition lock]; const HGSummaryLearnQAState *q=_summaryTMQAState.length==sizeof(*q) ? _summaryTMQAState.bytes : NULL;
    BOOL valid=_summaryTMEnabled && !_paused && !_stopping && !_summaryTMRequestPending && !_summaryTMRequestAccepted && !_summaryTMFixturePreparing
        && q && q->revision==revision && q->fieldReady && !q->owner && q->fadeFinished && _fieldMapId==MAP_ROUTE_29 && !_fieldMenuOpen
        && _summaryTMBootInputStep==3 && !_summaryTMBootInputFrame && [self summaryTMInputIdleLocked];
    if (valid) { _summaryTMRequestPending=YES; _summaryTMRequestRevision=revision; }
    [_condition unlock]; return valid;
}
- (BOOL)summaryTMRawAllowed:(const HGSummaryLearnQAState *)q bag:(const HGNativeBagUIState *)state keys:(uint32_t)keys x:(uint32_t)x y:(uint32_t)y pressed:(BOOL)pressed {
    if (!q || q->version!=HG_SUMMARY_LEARN_QA_VERSION || q->byteSize!=sizeof(*q) || q->execState!=2 || !q->graphicsReady || !q->fadeFinished || x>=256 || y>=192) return NO;
    if (!keys && pressed && q->owner==HG_SUMMARY_LEARN_BAG && state && state->active && state->ready && !state->mode && q->phase==state->phase
        && (state->phase==HG_BAG_UI_INPUT || state->phase==HG_BAG_UI_ITEM_MENU)) {
        const uint32_t counts[]={HG_BAG_UI_POCKETS,HG_BAG_UI_ITEMS,HG_BAG_UI_ACTIONS,1,1,1};
        for (uint32_t kind=HG_BAG_UI_TOUCH_POCKET;kind<=HG_BAG_UI_TOUCH_CANCEL;kind++) for (uint32_t index=0;index<counts[kind];index++) {
            uint32_t hitX,hitY; if (HGNativeBagUIResolveTouch(state,state->revision,state->phase,kind,index,&hitX,&hitY) && x==hitX && y==hitY) return YES;
        }
        return NO;
    }
    BOOL bag=q->owner==HG_SUMMARY_LEARN_BAG && q->phase==13 && q->tmTaskMatches && q->itemId==ITEM_TM17;
    BOOL party=q->owner==HG_SUMMARY_LEARN_PARTY && (q->context==6 || q->context==7) && q->itemId==ITEM_TM17 && q->moveId==MOVE_PROTECT;
    if (keys==1 && !pressed) {
        if (bag && q->taskState==1 && q->printerId<8 && !q->printerActive) return YES;
        if (party && q->context==6 && q->phase==21 && !q->partySlot && !q->partyBusy) return YES;
        return ((bag && q->taskState==2) || (party && q->phase==24)) && q->printerActive && q->printerMatches && q->printerState==2 && q->printerWaiting;
    }
    if (keys || !pressed || !((bag && q->taskState==3) || (party && q->phase==27)) || !q->yesNoReady || q->yesNoActive!=1 || q->yesNoResult!=3 || q->yesNoTimer) return NO;
    if (q->yesNoX!=25 || q->yesNoY!=(bag ? 6u : 10u) || q->yesNoWidth!=6 || q->yesNoHeight!=4) return NO;
    for (NSUInteger i=0;i<2;i++) if (x==q->yesHitX[i] && y==q->yesHitY[i] && x==224 && y==q->yesNoY*8+16+i*32) return YES;
    return NO;
}
- (BOOL)queueSummaryTMRaw:(const HGSummaryLearnQAState *)expected keys:(uint32_t)keys x:(uint32_t)x y:(uint32_t)y pressed:(BOOL)pressed {
    [_condition lock]; const HGSummaryLearnQAState *q=_summaryTMQAState.length==sizeof(*q) ? _summaryTMQAState.bytes : NULL;
    const HGNativeBagUIState *bag=_bagUIState.length==sizeof(*bag) ? _bagUIState.bytes : NULL;
    BOOL valid=_summaryTMEnabled && _summaryTMRequestAccepted && !_paused && !_stopping && q && expected
        && q->revision==expected->revision && q->generation==expected->generation && q->owner==expected->owner && q->data==expected->data
        && _summaryTMRawIdle && [self summaryTMInputIdleLocked] && [self summaryTMRawAllowed:q bag:bag keys:keys x:x y:y pressed:pressed];
    if (valid) { _summaryTMRawExpected=*expected; _summaryTMRawKeys=keys; _summaryTMRawX=x; _summaryTMRawY=y; _summaryTMRawPressed=pressed;
        _summaryTMRawBagRevision=q->owner==HG_SUMMARY_LEARN_BAG && (q->phase==HG_BAG_UI_INPUT || q->phase==HG_BAG_UI_ITEM_MENU) ? bag->revision : 0;
        _summaryTMRawPending=YES; _summaryTMRawIdle=NO; _summaryTMRawAccepted=NO; }
    [_condition unlock]; return valid;
}
- (void)clearSummaryTMTransientLocked {
    if (_summaryTMRawPending || _summaryTMRawActive || _summaryTMRawCooling) _summaryTMRawRejected++;
    _summaryTMRequestPending=_summaryTMRawPending=_summaryTMRawActive=_summaryTMRawCooling=_summaryTMRawAccepted=NO; _summaryTMRawIdle=YES;
    _summaryTMRawKeys=_summaryTMRawBagRevision=0; _summaryTMRawPressed=NO;
}
- (void)takeSummaryTMInputLocked:(const HGSummaryLearnQAState *)q bag:(const HGNativeBagUIState *)bag reads:(uint32_t)reads keys:(uint32_t *)keys x:(uint32_t *)x y:(uint32_t *)y pressed:(BOOL *)pressed {
    if (!_summaryTMEnabled) return;
    if (!_summaryTMRequestAccepted) {
#ifdef HEARTGOLD_ORIGINAL_POKEDEX_CHECK
        // The original no-Dex route opens Bag before its fixture can be admitted.
        if (_summaryTMBootStep==3 && !_summaryTMBootFrame && _fieldMapId==MAP_ROUTE_29 && _fieldMenuOpen
            && !_summaryTMFixturePrepared && !_summaryTMRequestPending && !_summaryTMFixturePreparing
            && q && q->version==HG_SUMMARY_LEARN_QA_VERSION && q->byteSize==sizeof(*q)
            && q->fieldReady && !q->owner && !q->manager && q->fadeFinished && bag && !bag->active
            && *pressed && *x<256 && *y<192 && !*keys && !_keys && !_pendingKeys && !_submittedKeys
            && !_joystickKeys && !_pendingJoystickKeys && !_submittedJoystickKeys) {
            const HGNativePokedexUIState *dex=[self summaryTMGuest:w2c_hgengine_HGNativePokedexUIGetState(_game) size:sizeof(HGNativePokedexUIState)];
            uint32_t icons=w2c_hgengine_HGNativeFieldMenuIcons(_game); BOOL noDex=YES,bagHit=NO;
            for (NSUInteger i=0;i<MIN((NSUInteger)7,_fieldMenuHitboxes.count);i++) {
                uint32_t role=(icons>>(4*i))&15; noDex&=role!=0;
                NSArray *box=_fieldMenuHitboxes[i];
                if (role==2 && box.count==4) bagHit|=CGRectContainsPoint(CGRectMake([box[0] doubleValue],[box[1] doubleValue],[box[2] doubleValue],[box[3] doubleValue]),CGPointMake(*x,*y));
            }
            if (!dex->active && noDex && bagHit) { _keys=_pendingKeys=_submittedKeys=0; *keys=0; return; }
        }
#endif
        _keys=_pendingKeys=_submittedKeys=0; _touchPressed=NO; *keys=0; *pressed=NO;
        if (!_env->resetOccurred && _summaryTMBootStep<3) {
            const uint32_t frames[]={450,780,1320}; uint32_t now=OS_GetVBlankCount();
            if (!_summaryTMBootFrame && now>=frames[_summaryTMBootStep]) _summaryTMBootFrame=now;
            if (_summaryTMBootFrame && now-_summaryTMBootFrame>=8) { _summaryTMBootFrame=0; _summaryTMBootStep++; }
            else if (_summaryTMBootFrame) *keys=1;
            if (now>=3600 && _summaryTMBootStep<3) _summaryTMFixtureError=@"original_boot_input_deadline";
        }
        return;
    }
    BOOL peripheral=q && (q->owner==HG_SUMMARY_LEARN_BAG || q->owner==HG_SUMMARY_LEARN_PARTY);
    if (peripheral || _summaryTMRawPending || _summaryTMRawActive || _summaryTMRawCooling) { _keys=_pendingKeys=_submittedKeys=0; _touchPressed=NO; *keys=0; *pressed=NO; }
    BOOL nativeBag=_summaryTMRawExpected.owner==HG_SUMMARY_LEARN_BAG && (_summaryTMRawExpected.phase==HG_BAG_UI_INPUT || _summaryTMRawExpected.phase==HG_BAG_UI_ITEM_MENU);
    BOOL sameBag=!nativeBag || (bag && bag->active && bag->ready && !bag->mode && bag->phase==_summaryTMRawExpected.phase && bag->revision==_summaryTMRawBagRevision);
    if (_summaryTMRawPending) {
        BOOL same=q && q->revision==_summaryTMRawExpected.revision && q->generation==_summaryTMRawExpected.generation
            && q->owner==_summaryTMRawExpected.owner && q->manager==_summaryTMRawExpected.manager && q->data==_summaryTMRawExpected.data && q->phase==_summaryTMRawExpected.phase && sameBag;
        _summaryTMRawPending=NO; _summaryTMRawReads=reads;
        if (same && [self summaryTMRawAllowed:q bag:bag keys:_summaryTMRawKeys x:_summaryTMRawX y:_summaryTMRawY pressed:_summaryTMRawPressed]) {
            _summaryTMRawActive=_summaryTMRawAccepted=YES; _summaryTMRawPresses++;
        } else { _summaryTMRawRejected++; _summaryTMRawCooling=YES; }
    }
    if (_summaryTMRawActive) {
        BOOL changed=!q || q->revision!=_summaryTMRawExpected.revision || q->generation!=_summaryTMRawExpected.generation || q->owner!=_summaryTMRawExpected.owner || !sameBag;
        if (changed || reads-_summaryTMRawReads>=4) { _summaryTMRawActive=NO; _summaryTMRawCooling=YES; _summaryTMRawReads=reads; _summaryTMRawReleases++; }
        else { *keys=_summaryTMRawKeys; *x=_summaryTMRawX; *y=_summaryTMRawY; *pressed=_summaryTMRawPressed; }
    }
    if (_summaryTMRawCooling && reads-_summaryTMRawReads>=2) _summaryTMRawCooling=NO;
    _summaryTMRawIdle=!_summaryTMRawPending && !_summaryTMRawActive && !_summaryTMRawCooling;
}
- (BOOL)prepareSummaryTMFixture:(const HGSummaryLearnQAState *)q {
#if HEARTGOLD_ORIGINAL_POKEDEX_CHECK
    return [self preparePokedexFixture:q];
#else
#ifdef HEARTGOLD_ORIGINAL_EVOLUTION_LEARN_CHECK
    return [self prepareEvolutionLearningFixture:q];
#endif
#ifdef HEARTGOLD_ORIGINAL_EVOLUTION_CHECK
    return [self prepareEvolutionFixture:q];
#endif
#define TM_REQUIRE(value,message) do { if (!(value)) { _summaryTMFixtureError=(message); return NO; } } while (0)
    BOOL candy=[[NSBundle.mainBundle objectForInfoDictionaryKey:@"HGPrivateCheckCase"] isEqual:@"summary-candy"];
    uint32_t item=candy ? ITEM_RARE_CANDY : ITEM_TM17,pocket=candy ? POCKET_MEDICINE : POCKET_TMHMS;
    uint32_t save=w2c_hgengine_SaveData_Get(_game),party=w2c_hgengine_SaveArray_Party_Get(_game,save);
    _summaryTMFlags=w2c_hgengine_Save_VarsFlags_Get(_game,save); _summaryTMBag=w2c_hgengine_Save_Bag_Get(_game,save);
    _summaryTMProfile=w2c_hgengine_Save_PlayerData_GetProfile(_game,save); _summaryTMDex=w2c_hgengine_Save_Pokedex_Get(_game,save);
    TM_REQUIRE(w2c_hgengine_CheckGotStarter(_game,_summaryTMFlags) && w2c_hgengine_Party_GetCount(_game,party)==1,@"original_party_prerequisite");
    _summaryTMMon=w2c_hgengine_Party_GetMonByIndex(_game,party,0);
    TM_REQUIRE(w2c_hgengine_GetMonData(_game,_summaryTMMon,MON_DATA_SPECIES,0)==152 && w2c_hgengine_GetMonData(_game,_summaryTMMon,MON_DATA_LEVEL,0)==5
        && w2c_hgengine_GetMonData(_game,_summaryTMMon,MON_DATA_HP,0)==20 && w2c_hgengine_GetMonData(_game,_summaryTMMon,MON_DATA_MAX_HP,0)==20
        && !w2c_hgengine_GetMonData(_game,_summaryTMMon,MON_DATA_HELD_ITEM,0)
        && w2c_hgengine_GetMonData(_game,_summaryTMMon,MON_DATA_OT_ID,0)==w2c_hgengine_PlayerProfile_GetTrainerID(_game,_summaryTMProfile)
        && !w2c_hgengine_Bag_GetQuantity(_game,_summaryTMBag,item,HEAP_ID_FIELD2),candy ? @"original_starter_or_candy_prerequisite" : @"original_starter_or_tm_prerequisite");
    _summaryTMOriginalPersonality=w2c_hgengine_GetMonData(_game,_summaryTMMon,MON_DATA_PERSONALITY,0);
    _summaryTMOriginalOtId=w2c_hgengine_GetMonData(_game,_summaryTMMon,MON_DATA_OT_ID,0);
    uint16_t name[PLAYER_NAME_LENGTH+1]; memcpy(name,[self summaryTMGuest:_summaryTMProfile+offsetof(PlayerProfile,name) size:sizeof(name)],sizeof(name));
    _summaryTMNameLength=0; while (_summaryTMNameLength<PLAYER_NAME_LENGTH+1 && name[_summaryTMNameLength]!=EOS) _summaryTMNameLength++;
    TM_REQUIRE(_summaryTMNameLength && _summaryTMNameLength<=PLAYER_NAME_LENGTH && q->mapSection,@"bounded_original_trainer_name");
    uint32_t scratch=w2c_hgengine_HGNativeFrameBuffer(_game); [self summaryTMGuest:scratch size:sizeof(name)];
    w2c_hgengine_GetMonData(_game,_summaryTMMon,MON_DATA_OT_NAME,scratch);
    TM_REQUIRE(!memcmp(name,[self summaryTMGuest:scratch size:sizeof(name)],(_summaryTMNameLength+1)*sizeof(uint16_t)),@"original_trainer_name_mismatch");
    const uint16_t moves[]={MOVE_TACKLE,MOVE_GROWL,candy ? MOVE_REFLECT : MOVE_RAZOR_LEAF,MOVE_CUT};
    for (NSUInteger i=0;i<4;i++) w2c_hgengine_MonSetMoveInSlot_ResetPpUp(_game,_summaryTMMon,moves[i],i);
    TM_REQUIRE(w2c_hgengine_Bag_AddItem(_game,_summaryTMBag,item,2,HEAP_ID_FIELD2),candy ? @"private_candy_add_failed" : @"private_tm_add_failed");
    TM_REQUIRE(w2c_hgengine_GetMonData(_game,_summaryTMMon,MON_DATA_FRIENDSHIP,0)==70 && !w2c_hgengine_GetMonData(_game,_summaryTMMon,MON_DATA_MOOD,0)
        && !w2c_hgengine_GetMonData(_game,_summaryTMMon,MON_DATA_EGG_LOCATION,0) && !w2c_hgengine_GetMonData(_game,_summaryTMMon,MON_DATA_HELD_ITEM,0)
        && w2c_hgengine_GetMonData(_game,_summaryTMMon,MON_DATA_DP_POKEBALL,0)!=BALL_LUXURY,@"original_starter_metadata");
    for (NSUInteger i=0;i<26;i++) _summaryTMMonBaseline[i]=w2c_hgengine_GetMonData(_game,_summaryTMMon,SummaryTMAttributes[i],0);
    TM_REQUIRE(_summaryTMMonBaseline[0]==_summaryTMOriginalPersonality && _summaryTMMonBaseline[1]==_summaryTMOriginalOtId,@"original_starter_identity_changed");
    if (candy) {
        const uint32_t ids[]={MON_DATA_MAX_HP,MON_DATA_ATK,MON_DATA_DEF,MON_DATA_SP_ATK,MON_DATA_SP_DEF,MON_DATA_SPEED};
        const uint32_t ivs[]={MON_DATA_HP_IV,MON_DATA_ATK_IV,MON_DATA_DEF_IV,MON_DATA_SPATK_IV,MON_DATA_SPDEF_IV,MON_DATA_SPEED_IV};
        const uint32_t evs[]={MON_DATA_HP_EV,MON_DATA_ATK_EV,MON_DATA_DEF_EV,MON_DATA_SPATK_EV,MON_DATA_SPDEF_EV,MON_DATA_SPEED_EV};
        const uint32_t base[]={45,49,65,49,65,45},natureIndex[]={0,0,1,3,4,2};
        uint32_t nature=_summaryTMOriginalPersonality%25;
        for (NSUInteger i=0;i<6;i++) {
            uint32_t iv=w2c_hgengine_GetMonData(_game,_summaryTMMon,ivs[i],0),ev=w2c_hgengine_GetMonData(_game,_summaryTMMon,evs[i],0);
            TM_REQUIRE(iv<=31 && ev<=255,@"original_candy_iv_ev_bounds");
            uint32_t expected=(2*base[i]+iv+ev/4)*5/100+(i ? 5 : 15);
            if (i && nature/5!=nature%5) {
                if (natureIndex[i]==nature/5) expected=(uint16_t)(expected*110)/100;
                else if (natureIndex[i]==nature%5) expected=(uint16_t)(expected*90)/100;
            }
            _summaryTMMonBaseline[26+i]=w2c_hgengine_GetMonData(_game,_summaryTMMon,ids[i],0);
            TM_REQUIRE(_summaryTMMonBaseline[26+i]==expected,@"original_candy_stats_or_data_mismatch");
        }
        _summaryTMMonBaseline[32]=w2c_hgengine_GetMonData(_game,_summaryTMMon,MON_DATA_EXPERIENCE,0);
        TM_REQUIRE(_summaryTMMonBaseline[32]>=135 && _summaryTMMonBaseline[32]<179,@"original_level5_experience");
    }
    uint32_t bagSize=w2c_hgengine_Save_Bag_sizeof(_game),flagsSize=w2c_hgengine_Save_VarsFlags_sizeof(_game);
    TM_REQUIRE(bagSize && bagSize<65536 && flagsSize && flagsSize<65536,@"private_baseline_sizes");
    _summaryTMBagBaseline=[NSData dataWithBytes:[self summaryTMGuest:_summaryTMBag size:bagSize] length:bagSize];
    _summaryTMFlagsBaseline=[NSData dataWithBytes:[self summaryTMGuest:_summaryTMFlags size:flagsSize] length:flagsSize];
    _summaryTMTmOffset=bagSize;
    for (NSUInteger i=0;i<(candy ? NUM_BAG_MEDICINE : NUM_BAG_TMS_HMS);i++) { uint32_t slot=w2c_hgengine_Bag_GetPocketSlotN(_game,_summaryTMBag,pocket,i); uint16_t value[2];
        memcpy(value,[self summaryTMGuest:slot size:sizeof(value)],sizeof(value));
        if (value[0]==item) { TM_REQUIRE(value[1]==2 && slot>=_summaryTMBag && slot-_summaryTMBag<=bagSize-4,candy ? @"private_candy_slot" : @"private_tm_slot"); _summaryTMTmOffset=slot-_summaryTMBag; break; } }
    TM_REQUIRE(_summaryTMTmOffset<bagSize,candy ? @"private_candy_slot_missing" : @"private_tm_slot_missing");
    _summaryTMMoney=w2c_hgengine_PlayerProfile_GetMoney(_game,_summaryTMProfile); _summaryTMDexEnabled=w2c_hgengine_Pokedex_IsEnabled(_game,_summaryTMDex);
    _summaryTMDexSeen=w2c_hgengine_Pokedex_CountNationalDexSeen(_game,_summaryTMDex); _summaryTMDexOwned=w2c_hgengine_Pokedex_CountNationalDexOwned(_game,_summaryTMDex);
    _summaryTMMapSection=q->mapSection; _summaryTMFixturePrepared=YES; return YES;
#undef TM_REQUIRE
#endif
}
- (NSDictionary *)summaryTMCurrentFacts {
#if HEARTGOLD_ORIGINAL_POKEDEX_CHECK
    return [self pokedexCurrentFacts];
#else
#ifdef HEARTGOLD_ORIGINAL_EVOLUTION_LEARN_CHECK
    return [self evolutionLearningCurrentFacts];
#endif
#ifdef HEARTGOLD_ORIGINAL_EVOLUTION_CHECK
    return [self evolutionCurrentFacts];
#endif
    NSMutableDictionary *facts=[@{@"fixturePrepared":@(_summaryTMFixturePrepared),@"fixtureError":_summaryTMFixtureError ?: @"",
        @"rawPresses":@(_summaryTMRawPresses),@"rawReleases":@(_summaryTMRawReleases),@"rawRejected":@(_summaryTMRawRejected)} mutableCopy];
    if (!_summaryTMFixturePrepared) return facts;
    if ([[NSBundle.mainBundle objectForInfoDictionaryKey:@"HGPrivateCheckCase"] isEqual:@"summary-candy"]) {
        uint32_t current[26],expectedStats[6]; BOOL unchanged=YES,upgraded=YES,learned=YES,statsMatch=YES;
        NSMutableArray *moves=[NSMutableArray new],*pp=[NSMutableArray new],*maxPP=[NSMutableArray new],*ups=[NSMutableArray new];
        NSMutableArray *initialStats=[NSMutableArray new],*stats=[NSMutableArray new],*expected=[NSMutableArray new];
        const uint32_t ids[]={MON_DATA_MAX_HP,MON_DATA_ATK,MON_DATA_DEF,MON_DATA_SP_ATK,MON_DATA_SP_DEF,MON_DATA_SPEED};
        const uint32_t ivs[]={MON_DATA_HP_IV,MON_DATA_ATK_IV,MON_DATA_DEF_IV,MON_DATA_SPATK_IV,MON_DATA_SPDEF_IV,MON_DATA_SPEED_IV};
        const uint32_t evs[]={MON_DATA_HP_EV,MON_DATA_ATK_EV,MON_DATA_DEF_EV,MON_DATA_SPATK_EV,MON_DATA_SPDEF_EV,MON_DATA_SPEED_EV};
        const uint32_t base[]={45,49,65,49,65,45},natureIndex[]={0,0,1,3,4,2};
        uint32_t nature=_summaryTMOriginalPersonality%25;
        // Original CalcMonStats/ModifyStatByNature at level6, using the valid starter's actual IVs/EVs/PID.
        for (NSUInteger i=0;i<6;i++) {
            uint32_t iv=w2c_hgengine_GetMonData(_game,_summaryTMMon,ivs[i],0),ev=w2c_hgengine_GetMonData(_game,_summaryTMMon,evs[i],0);
            expectedStats[i]=(2*base[i]+iv+ev/4)*6/100+(i ? 5 : 16);
            if (i && nature/5!=nature%5) {
                if (natureIndex[i]==nature/5) expectedStats[i]=(uint16_t)(expectedStats[i]*110)/100;
                else if (natureIndex[i]==nature%5) expectedStats[i]=(uint16_t)(expectedStats[i]*90)/100;
            }
            uint32_t value=w2c_hgengine_GetMonData(_game,_summaryTMMon,ids[i],0);
            [initialStats addObject:@(_summaryTMMonBaseline[26+i])];[stats addObject:@(value)];[expected addObject:@(expectedStats[i])];
            statsMatch&=iv<=31 && ev<=255 && value==expectedStats[i];
        }
        uint32_t ppMax=w2c_hgengine_GetMoveMaxPP(_game,MOVE_RAZOR_LEAF,0);
        for (NSUInteger i=0;i<26;i++) {
            current[i]=w2c_hgengine_GetMonData(_game,_summaryTMMon,SummaryTMAttributes[i],0);unchanged&=current[i]==_summaryTMMonBaseline[i];
            uint32_t value=i==6 || i==7 ? expectedStats[0] : i==8 ? 6 : _summaryTMMonBaseline[i];
            upgraded&=current[i]==value;
            if (i==10) value=MOVE_RAZOR_LEAF;else if (i==14 || i==22) value=ppMax;else if (i==18) value=0;
            learned&=current[i]==value;
        }
        for (NSUInteger i=0;i<4;i++) {[moves addObject:@(current[10+i])];[pp addObject:@(current[14+i])];[ups addObject:@(current[18+i])];[maxPP addObject:@(current[22+i])];}
        uint32_t experience=w2c_hgengine_GetMonData(_game,_summaryTMMon,MON_DATA_EXPERIENCE,0),friendship=w2c_hgengine_GetMonData(_game,_summaryTMMon,MON_DATA_FRIENDSHIP,0),mood=w2c_hgengine_GetMonData(_game,_summaryTMMon,MON_DATA_MOOD,0);
        uint32_t quantity=w2c_hgengine_Bag_GetQuantity(_game,_summaryTMBag,ITEM_RARE_CANDY,HEAP_ID_FIELD2);
        BOOL bagSame=!memcmp(_summaryTMBagBaseline.bytes,[self summaryTMGuest:_summaryTMBag size:_summaryTMBagBaseline.length],_summaryTMBagBaseline.length);
        NSMutableData *oneConsumed=[_summaryTMBagBaseline mutableCopy];uint16_t one=1;memcpy((uint8_t *)oneConsumed.mutableBytes+_summaryTMTmOffset+2,&one,sizeof(one));
        BOOL bagConsumed=quantity==1 && !memcmp(oneConsumed.bytes,[self summaryTMGuest:_summaryTMBag size:oneConsumed.length],oneConsumed.length);
        BOOL preserved=!memcmp(_summaryTMFlagsBaseline.bytes,[self summaryTMGuest:_summaryTMFlags size:_summaryTMFlagsBaseline.length],_summaryTMFlagsBaseline.length)
            && w2c_hgengine_PlayerProfile_GetMoney(_game,_summaryTMProfile)==_summaryTMMoney && w2c_hgengine_Pokedex_IsEnabled(_game,_summaryTMDex)==_summaryTMDexEnabled
            && w2c_hgengine_Pokedex_CountNationalDexSeen(_game,_summaryTMDex)==_summaryTMDexSeen && w2c_hgengine_Pokedex_CountNationalDexOwned(_game,_summaryTMDex)==_summaryTMDexOwned;
        BOOL effect=statsMatch && experience==179 && friendship==75 && mood==40;
        [facts addEntriesFromDictionary:@{@"fixtureCase":@"summary-candy",@"species":@(current[2]),@"partyCount":@(w2c_hgengine_Party_GetCount(_game,w2c_hgengine_SaveArray_Party_Get(_game,w2c_hgengine_SaveData_Get(_game)))),
            @"moves":moves,@"pp":pp,@"maxPP":maxPP,@"ppUps":ups,@"candyQuantity":@(quantity),@"friendship":@(friendship),@"mood":@(mood),
            @"level":@(current[8]),@"experience":@(experience),@"initialExperience":@(_summaryTMMonBaseline[32]),@"hp":@(current[6]),@"maxHp":@(current[7]),
            @"initialStats":initialStats,@"stats":stats,@"expectedStats":expected,@"statsMatchOriginalFormula":@(statsMatch),
            @"baseline26Preserved":@(unchanged && experience==_summaryTMMonBaseline[32] && friendship==70 && !mood),
            @"expectedCandyDelta":@(upgraded && effect),@"expectedLearningDelta":@(learned && effect),@"bagUnchanged":@(bagSame),@"bagOnlyOneCandyConsumed":@(bagConsumed),@"flagsMoneyDexUnchanged":@(preserved),
            @"otNameProfileMatched":@YES,@"otNameLength":@(_summaryTMNameLength),@"fixtureUsesExistingStarter":@YES,
            @"originalPersonality":@(_summaryTMOriginalPersonality),@"originalOtId":@(_summaryTMOriginalOtId),
            @"originalIdentityPreserved":@(current[0]==_summaryTMOriginalPersonality && current[1]==_summaryTMOriginalOtId),@"mapSection":@(_summaryTMMapSection)}];
        return facts;
    }
    uint32_t current[26]; BOOL unchanged=YES,learned=YES;
    NSMutableArray *moves=[NSMutableArray new],*pp=[NSMutableArray new],*maxPP=[NSMutableArray new],*ups=[NSMutableArray new];
    for (NSUInteger i=0;i<26;i++) { current[i]=w2c_hgengine_GetMonData(_game,_summaryTMMon,SummaryTMAttributes[i],0); unchanged&=current[i]==_summaryTMMonBaseline[i];
        uint32_t expected=i==10 ? MOVE_PROTECT : i==14 || i==22 ? 10 : i==18 ? 0 : _summaryTMMonBaseline[i]; learned&=current[i]==expected; }
    for (NSUInteger i=0;i<4;i++) { [moves addObject:@(current[10+i])]; [pp addObject:@(current[14+i])]; [ups addObject:@(current[18+i])]; [maxPP addObject:@(current[22+i])]; }
    uint32_t friendship=w2c_hgengine_GetMonData(_game,_summaryTMMon,MON_DATA_FRIENDSHIP,0),mood=w2c_hgengine_GetMonData(_game,_summaryTMMon,MON_DATA_MOOD,0);
    uint32_t quantity=w2c_hgengine_Bag_GetQuantity(_game,_summaryTMBag,ITEM_TM17,HEAP_ID_FIELD2);
    BOOL bagSame=!memcmp(_summaryTMBagBaseline.bytes,[self summaryTMGuest:_summaryTMBag size:_summaryTMBagBaseline.length],_summaryTMBagBaseline.length);
    NSMutableData *oneConsumed=[_summaryTMBagBaseline mutableCopy]; uint16_t one=1; memcpy((uint8_t *)oneConsumed.mutableBytes+_summaryTMTmOffset+2,&one,sizeof(one));
    BOOL bagConsumed=quantity==1 && !memcmp(oneConsumed.bytes,[self summaryTMGuest:_summaryTMBag size:oneConsumed.length],oneConsumed.length);
    BOOL preserved=!memcmp(_summaryTMFlagsBaseline.bytes,[self summaryTMGuest:_summaryTMFlags size:_summaryTMFlagsBaseline.length],_summaryTMFlagsBaseline.length)
        && w2c_hgengine_PlayerProfile_GetMoney(_game,_summaryTMProfile)==_summaryTMMoney && w2c_hgengine_Pokedex_IsEnabled(_game,_summaryTMDex)==_summaryTMDexEnabled
        && w2c_hgengine_Pokedex_CountNationalDexSeen(_game,_summaryTMDex)==_summaryTMDexSeen && w2c_hgengine_Pokedex_CountNationalDexOwned(_game,_summaryTMDex)==_summaryTMDexOwned;
    [facts addEntriesFromDictionary:@{@"species":@(current[2]),@"partyCount":@(w2c_hgengine_Party_GetCount(_game,w2c_hgengine_SaveArray_Party_Get(_game,w2c_hgengine_SaveData_Get(_game)))),
        @"moves":moves,@"pp":pp,@"maxPP":maxPP,@"ppUps":ups,@"tmQuantity":@(quantity),@"friendship":@(friendship),@"mood":@(mood),
        @"baseline26Preserved":@(unchanged && friendship==70 && !mood),@"expectedLearningDelta":@(learned && friendship==71 && mood==40),
        @"bagUnchanged":@(bagSame),@"bagOnlyOneTMConsumed":@(bagConsumed),@"flagsMoneyDexUnchanged":@(preserved),
        @"otNameProfileMatched":@YES,@"otNameLength":@(_summaryTMNameLength),@"fixtureUsesExistingStarter":@YES,
        @"originalPersonality":@(_summaryTMOriginalPersonality),@"originalOtId":@(_summaryTMOriginalOtId),
        @"originalIdentityPreserved":@(current[0]==_summaryTMOriginalPersonality && current[1]==_summaryTMOriginalOtId),@"mapSection":@(_summaryTMMapSection)}];
    return facts;
#endif
}
- (UIImage *)summaryTMWarningImage:(const HGNativePartyUIWindow *)window {
    if (!window || window->tileWidth!=15 || window->tileHeight!=10 || window->engine!=1 || window->clearColor || window->pixelBuffer>UINT32_MAX-2400) return nil;
    HGNativePartyUIWindow half=*window; half.tileHeight=5;
    UIImage *top=OriginalWindowImage(&half,_env->memory->data,_env->memory->size); half.pixelBuffer+=2400;
    UIImage *bottom=OriginalWindowImage(&half,_env->memory->data,_env->memory->size);
    if (!top || !bottom || CGImageGetBytesPerRow(top.CGImage)!=480 || CGImageGetBytesPerRow(bottom.CGImage)!=480) return nil;
    NSData *first=CFBridgingRelease(CGDataProviderCopyData(CGImageGetDataProvider(top.CGImage)));
    NSData *second=CFBridgingRelease(CGDataProviderCopyData(CGImageGetDataProvider(bottom.CGImage)));
    if (first.length!=19200 || second.length!=19200) return nil;
    NSMutableData *rgba=[first mutableCopy]; [rgba appendData:second]; return OriginalPixelImage(rgba,120,80);
}
#endif
- (void)engineFrame {
    [_condition lock]; BOOL paused = _paused;
#ifdef HEARTGOLD_ORIGINAL_APP_CHECK
    if (paused) _audioChanging=YES;
#endif
    [_condition unlock];
    if (paused) {
        if (!HGNativeSoundRealtime(_env, NO)) wasm_rt_trap(WASM_RT_TRAP_OOB);
        [_audio pause];
#ifdef HEARTGOLD_ORIGINAL_APP_CHECK
        [self publishAudioProducerTrace];
#endif
        HGNativeStopDisplayClock(_env);
        [_condition lock];
#ifdef HEARTGOLD_ORIGINAL_APP_CHECK
        _audioChanging=NO; _suspended = YES;
#endif
        while (_paused && !_stopping) [_condition wait];
#ifdef HEARTGOLD_ORIGINAL_APP_CHECK
        _suspended = NO;
        _audioChanging=!_stopping;
#endif
        BOOL stopping = _stopping; [_condition unlock];
        if (!stopping) {
            NSError *error;
            BOOL ready = [_audio resume:&error] && HGNativeSoundRealtime(_env, YES) && HGNativeStartDisplayClock(_env, _displayStatus) && HGNativeSetDisplayObserver(_env, present, (__bridge void *)self);
#ifdef HEARTGOLD_ORIGINAL_APP_CHECK
            [_condition lock]; _audioChanging=NO; [_condition unlock];
#endif
            if (!ready) { [_condition lock]; _failure = error.localizedDescription ?: @"无法恢复游戏与声音输出。"; _stopping = YES;
#ifdef HEARTGOLD_ORIGINAL_PC_BOX_CHECK
                [self clearPCBoxCheckTransientLocked:YES];
#endif
                [self clearFieldChoiceInputLocked]; [self clearPCBoxInputLocked]; [self clearCaptureInputLocked]; [_condition unlock]; }
        }
    }
    uint32_t battleAddress=w2c_hgengine_HGNativeBattleUIGetState(_game);
    if (!battleAddress || battleAddress>_env->memory->size || sizeof(HGNativeBattleUIState)>_env->memory->size-battleAddress) wasm_rt_trap(WASM_RT_TRAP_OOB);
    const HGNativeBattleUIState *currentBattle=(const HGNativeBattleUIState *)(_env->memory->data+battleAddress);
    uint32_t battleBagAddress=w2c_hgengine_HGNativeBattleBagUIGetState(_game),battlePartyAddress=w2c_hgengine_HGNativeBattlePartyUIGetState(_game);
    if (!battleBagAddress || battleBagAddress>_env->memory->size || sizeof(HGNativeBattleBagUIState)>_env->memory->size-battleBagAddress
        || !battlePartyAddress || battlePartyAddress>_env->memory->size || sizeof(HGNativeBattlePartyUIState)>_env->memory->size-battlePartyAddress) wasm_rt_trap(WASM_RT_TRAP_OOB);
    const HGNativeBattleBagUIState *currentBattleBag=(const HGNativeBattleBagUIState *)(_env->memory->data+battleBagAddress);
    const HGNativeBattlePartyUIState *currentBattleParty=(const HGNativeBattlePartyUIState *)(_env->memory->data+battlePartyAddress);
    uint32_t partyAddress=w2c_hgengine_HGNativePartyUIGetState(_game);
    if (!partyAddress || partyAddress>_env->memory->size || sizeof(HGNativePartyUIState)>_env->memory->size-partyAddress) wasm_rt_trap(WASM_RT_TRAP_OOB);
    const HGNativePartyUIState *currentParty=(const HGNativePartyUIState *)(_env->memory->data+partyAddress);
    uint32_t bagAddress=w2c_hgengine_HGNativeBagUIGetState(_game);
    if (!bagAddress || bagAddress>_env->memory->size || sizeof(HGNativeBagUIState)>_env->memory->size-bagAddress) wasm_rt_trap(WASM_RT_TRAP_OOB);
    const HGNativeBagUIState *currentBag=(const HGNativeBagUIState *)(_env->memory->data+bagAddress);
    uint32_t summaryAddress=w2c_hgengine_HGNativeSummaryUIGetState(_game);
    if (!summaryAddress || summaryAddress>_env->memory->size || sizeof(HGNativeSummaryUIState)>_env->memory->size-summaryAddress) wasm_rt_trap(WASM_RT_TRAP_OOB);
    const HGNativeSummaryUIState *currentSummary=(const HGNativeSummaryUIState *)(_env->memory->data+summaryAddress);
    uint32_t pcBoxAddress=w2c_hgengine_HGNativePCBoxUIGetState(_game);
    if (!pcBoxAddress || pcBoxAddress>_env->memory->size || sizeof(HGNativePCBoxUIState)>_env->memory->size-pcBoxAddress) wasm_rt_trap(WASM_RT_TRAP_OOB);
    const HGNativePCBoxUIState *currentPCBox=(const HGNativePCBoxUIState *)(_env->memory->data+pcBoxAddress);
    uint32_t fieldChoiceAddress=w2c_hgengine_HGNativeFieldChoiceUIGetState(_game);
    if (!fieldChoiceAddress || fieldChoiceAddress>_env->memory->size || sizeof(HGNativeFieldChoiceUIState)>_env->memory->size-fieldChoiceAddress) wasm_rt_trap(WASM_RT_TRAP_OOB);
    const HGNativeFieldChoiceUIState *currentFieldChoice=(const HGNativeFieldChoiceUIState *)(_env->memory->data+fieldChoiceAddress);
    uint32_t evolutionAddress=w2c_hgengine_HGNativeEvolutionUIGetState(_game);
    if (!evolutionAddress || evolutionAddress>_env->memory->size || sizeof(HGNativeEvolutionUIState)>_env->memory->size-evolutionAddress) wasm_rt_trap(WASM_RT_TRAP_OOB);
    const HGNativeEvolutionUIState *currentEvolution=(const HGNativeEvolutionUIState *)(_env->memory->data+evolutionAddress);
    uint32_t optionsAddress=w2c_hgengine_HGNativeOptionsUIGetState(_game);
    if (!optionsAddress || optionsAddress>_env->memory->size || sizeof(HGNativeOptionsUIState)>_env->memory->size-optionsAddress) wasm_rt_trap(WASM_RT_TRAP_OOB);
    const HGNativeOptionsUIState *currentOptions=(const HGNativeOptionsUIState *)(_env->memory->data+optionsAddress);
    uint32_t trainerAddress=w2c_hgengine_HGNativeTrainerCardUIGetState(_game);
    if (!trainerAddress || trainerAddress>_env->memory->size || sizeof(HGNativeTrainerCardUIState)>_env->memory->size-trainerAddress) wasm_rt_trap(WASM_RT_TRAP_OOB);
    const HGNativeTrainerCardUIState *currentTrainer=(const HGNativeTrainerCardUIState *)(_env->memory->data+trainerAddress);
    uint32_t startMenuAddress=w2c_hgengine_HGNativeFieldMenuUIGetState(_game);
    if (!startMenuAddress || startMenuAddress>_env->memory->size || sizeof(HGNativeFieldMenuUIState)>_env->memory->size-startMenuAddress) wasm_rt_trap(WASM_RT_TRAP_OOB);
    const HGNativeFieldMenuUIState *currentStartMenu=(const HGNativeFieldMenuUIState *)(_env->memory->data+startMenuAddress);
    BOOL trainerContext=currentTrainer->active==1;
    BOOL optionsContext=!trainerContext && currentOptions->active==1;
    uint32_t namingAddress=w2c_hgengine_HGNativeNamingUIGetState(_game);
    if (!namingAddress || namingAddress>_env->memory->size || sizeof(HGNativeNamingUIState)>_env->memory->size-namingAddress) wasm_rt_trap(WASM_RT_TRAP_OOB);
    const HGNativeNamingUIState *currentNaming=(const HGNativeNamingUIState *)(_env->memory->data+namingAddress);
    BOOL namingContext=!trainerContext && !optionsContext && currentNaming->active==1;
    uint32_t captureAddress=w2c_hgengine_HGNativeCaptureUIGetState(_game);
    if (!captureAddress || captureAddress>_env->memory->size || sizeof(HGNativeCaptureUIState)>_env->memory->size-captureAddress) wasm_rt_trap(WASM_RT_TRAP_OOB);
    const HGNativeCaptureUIState *currentCapture=(const HGNativeCaptureUIState *)(_env->memory->data+captureAddress);
    BOOL captureContext=!trainerContext && !optionsContext && !namingContext && currentCapture->active==1;
    uint32_t pokedexAddress=w2c_hgengine_HGNativePokedexUIGetState(_game);
    if (!pokedexAddress || pokedexAddress>_env->memory->size || sizeof(HGNativePokedexUIState)>_env->memory->size-pokedexAddress) wasm_rt_trap(WASM_RT_TRAP_OOB);
    const HGNativePokedexUIState *currentPokedex=(const HGNativePokedexUIState *)(_env->memory->data+pokedexAddress);
    if (currentPokedex->active && (currentPokedex->version!=HG_POKEDEX_UI_VERSION || currentPokedex->byteSize!=sizeof(*currentPokedex))) wasm_rt_trap(WASM_RT_TRAP_OOB);
    uint32_t battleMenuKind=OriginalBattleMenuKind(currentBattleBag,currentBattleParty);
    BOOL battleChild=currentBattleBag->active || currentBattleParty->active;
    BOOL summaryLearning=currentSummary->active && HGNativeSummaryUIIsLearning(currentSummary);
    BOOL summaryContext=!trainerContext && !optionsContext && !namingContext && currentSummary->active && (summaryLearning || HGNativeSummaryUIIsOrdinary(currentSummary));
    BOOL pcBoxActive=currentPCBox->active==1;
    BOOL pcBoxContext=!trainerContext && pcBoxActive && !optionsContext && !namingContext && !captureContext && !currentSummary->active
        && !currentBag->active && !currentParty->active && !currentEvolution->active && !battleChild && !currentBattle->active;
    BOOL fieldChoiceView=currentFieldChoice->active==1 && HGNativeFieldChoiceUIIsViewPhase(currentFieldChoice->phase);
    BOOL fieldChoiceContext=!trainerContext && fieldChoiceView && !optionsContext && !namingContext && !captureContext && !summaryContext && !pcBoxActive
        && !currentBag->active && !currentParty->active && !currentEvolution->active && !battleChild && !currentBattle->active && !currentPokedex->active;
    BOOL pokedexBlocked=trainerContext || optionsContext || namingContext || captureContext || summaryContext || currentEvolution->active || battleChild || currentBattle->active || currentParty->active || currentBag->active || pcBoxActive || fieldChoiceContext;
    BOOL pokedexContext=currentPokedex->active && !pokedexBlocked;
    BOOL startMenuBlocked=trainerContext || optionsContext || namingContext || captureContext || currentSummary->active || pcBoxActive
        || currentParty->active || currentBag->active || currentEvolution->active || battleChild || currentBattle->active || currentPokedex->active || fieldChoiceContext;
    BOOL startMenuContext=currentStartMenu->active==1 && !currentStartMenu->childActive && !startMenuBlocked;
    BOOL startMenuView=startMenuContext && OriginalStartMenuHasView(currentStartMenu);
    BOOL startMenuBlocksInput=startMenuContext && OriginalStartMenuBlocksInput(currentStartMenu);
    uint32_t fieldMap=w2c_hgengine_HGNativeFieldMap(_game);
#ifdef HEARTGOLD_ORIGINAL_PC_BOX_CHECK
    HGPCBoxQAState pcOwnerQA={0};
    if (![self readPCBoxQAOwner:&pcOwnerQA]) {
        [_condition lock]; _failure=[NSString stringWithFormat:@"Private PC observer failed at %u",pcOwnerQA.error]; _stopping=YES; [_condition unlock];
    }
#endif
#if defined(HEARTGOLD_ORIGINAL_CAPTURE_REGISTRATION_CHECK)
    HGNativeCaptureQAState captureQA={0}; [self readCaptureRegistrationQA:&captureQA];
#elif defined(HEARTGOLD_ORIGINAL_SUMMARY_TM_CHECK)
    HGSummaryLearnQAState summaryTM={0}; BOOL summaryTMObserved=[self readSummaryTMQA:&summaryTM];
#endif
    [_condition lock];
    [self acknowledgeKeysLocked:HGNativeInputPadReads(_env)];
    const HGNativeFieldMenuUIState *oldStartMenu=_startMenuRawUIState.length==sizeof(*oldStartMenu) ? _startMenuRawUIState.bytes : NULL;
    uint32_t startMenuRawEngine=(*_displayControl>>15)&1;
    BOOL startMenuChanged=!oldStartMenu || memcmp(oldStartMenu,currentStartMenu,sizeof(*currentStartMenu)) || _startMenuRawEngine!=startMenuRawEngine;
    if (OriginalStartMenuHandoff(oldStartMenu,currentStartMenu,_startMenuOwnedInputs,startMenuBlocksInput,startMenuChanged)) {
        [self clearStartMenuInputLocked];
        _keys=_pendingKeys=_submittedKeys=_joystickKeys=_pendingJoystickKeys=_submittedJoystickKeys=0;
        _touchPressed=NO;_joystickNeedsReset=YES;_inputGeneration++;
        if (!currentStartMenu->active || currentStartMenu->childActive || !currentStartMenu->viewActive) {_fieldMenuOpen=NO;_fieldMenuFrame=nil;}
    }
    _startMenuOwnedInputs=startMenuBlocksInput;
    _startMenuRawUIState=[NSData dataWithBytes:currentStartMenu length:sizeof(*currentStartMenu)];_startMenuRawEngine=startMenuRawEngine;
    _trainerCardRawUIState=[NSData dataWithBytes:currentTrainer length:sizeof(*currentTrainer)];
    if (trainerContext || _trainerCardOwnerGeneration) {
        _keys=_pendingKeys=_submittedKeys=_joystickKeys=_pendingJoystickKeys=_submittedJoystickKeys=0;
        _touchPressed=NO;_joystickNeedsReset=YES;_fieldMenuOpen=NO;_fieldMenuFrame=nil;_battleChoicePending=NO;
        _partyChoicePending=_partyTouchActive=_bagChoicePending=_bagTouchActive=_summaryChoicePending=_summaryInputActive=_summaryTouchPressed=NO;
        _summaryInputKeys=0;_battleMenuChoicePending=_battleMenuTouchActive=_battleMenuTouchCooling=NO;
        [self clearOptionsInputLocked];[self clearNamingInputLocked];[self clearPCBoxInputLocked];[self clearFieldChoiceInputLocked];
        [self clearCaptureInputLocked];[self clearEvolutionInputLocked];[self clearPokedexInputLocked];
    }
    _trainerCardOwnerGeneration=trainerContext ? currentTrainer->ownerGeneration : 0;
#ifdef HEARTGOLD_ORIGINAL_PC_BOX_CHECK
    [self processPCBoxQARequestOwnerLocked:&pcOwnerQA];
#endif
    if (pcBoxActive || _pcBoxOwnerGeneration) {
        _joystickKeys=_pendingJoystickKeys=_submittedJoystickKeys=0; _joystickNeedsReset=YES;
        _touchX=_touchY=0; _touchPressed=NO; _fieldMenuOpen=NO; _fieldMenuFrame=nil;
        if (!pcBoxActive || currentPCBox->ownerGeneration!=_pcBoxOwnerGeneration) {
            BOOL retiring=_pcBoxOwnerGeneration!=0; [self clearPCBoxInputLocked];
            if (retiring && pcBoxActive) {_pcBoxInputCooling=YES;_pcBoxReleaseReads=HGNativeInputPadReads(_env);}
        }
        if (!pcBoxContext && (_pcBoxChoicePending || _pcBoxInputActive)) {
            [self clearPCBoxInputLocked]; _pcBoxInputCooling=YES; _pcBoxReleaseReads=HGNativeInputPadReads(_env);
        }
        if (pcBoxContext || !_pcBoxOwnerGeneration || !pcBoxActive) {
            _keys=_pendingKeys=_submittedKeys=0; _battleChoicePending=NO;
            _partyChoicePending=_partyTouchActive=_bagChoicePending=_bagTouchActive=NO;
            _summaryChoicePending=_summaryInputActive=_summaryTouchPressed=NO; _summaryInputKeys=0;
            _battleMenuChoicePending=_battleMenuTouchActive=_battleMenuTouchCooling=NO;
            [self clearCaptureInputLocked]; [self clearEvolutionInputLocked]; [self clearPokedexInputLocked];
        }
    }
    _pcBoxOwnerGeneration=pcBoxActive ? currentPCBox->ownerGeneration : 0;
    if (fieldChoiceContext || _fieldChoiceOwnerGeneration) {
        _keys=_pendingKeys=_submittedKeys=_joystickKeys=_pendingJoystickKeys=_submittedJoystickKeys=0;
        _touchX=_touchY=0; _touchPressed=NO; _joystickNeedsReset=YES; _fieldMenuOpen=NO; _fieldMenuFrame=nil;
        if (!fieldChoiceContext || currentFieldChoice->ownerGeneration!=_fieldChoiceOwnerGeneration) {
            BOOL retiring=_fieldChoiceOwnerGeneration!=0; [self clearFieldChoiceInputLocked];
            if (retiring && fieldChoiceContext) {_fieldChoiceInputCooling=YES;_fieldChoiceReleaseReads=HGNativeInputPadReads(_env);}
        }
        if (fieldChoiceContext) {
            _battleChoicePending=NO; _partyChoicePending=_partyTouchActive=_bagChoicePending=_bagTouchActive=NO;
            _summaryChoicePending=_summaryInputActive=_summaryTouchPressed=NO; _summaryInputKeys=0;
            _battleMenuChoicePending=_battleMenuTouchActive=_battleMenuTouchCooling=NO;
            [self clearPCBoxInputLocked]; [self clearCaptureInputLocked]; [self clearEvolutionInputLocked]; [self clearPokedexInputLocked];
        }
    }
    _fieldChoiceOwnerGeneration=fieldChoiceContext ? currentFieldChoice->ownerGeneration : 0;
#ifdef HEARTGOLD_ORIGINAL_CAPTURE_REGISTRATION_CHECK
    BOOL captureLeaseWasActive=_captureInputActive; uint32_t captureLeaseStart=_captureInputReads;
#endif
#ifdef HEARTGOLD_ORIGINAL_NAMING_CHECK
    BOOL namingLeaseWasActive=_namingInputActive; uint32_t namingLeaseStart=_namingInputReads;
#endif
    if (optionsContext || _optionsOwnerGeneration) {
        _keys=_pendingKeys=_submittedKeys=_joystickKeys=_pendingJoystickKeys=_submittedJoystickKeys=0;
        _touchX=_touchY=0; _touchPressed=NO; _joystickNeedsReset=YES;
        _battleChoicePending=NO; _battleChoiceRevision=_battleChoiceIndex=0;
        _partyChoicePending=_partyTouchActive=_bagChoicePending=_bagTouchActive=NO;
        _summaryChoicePending=_summaryInputActive=_summaryTouchPressed=NO; _summaryInputKeys=0;
        _battleMenuChoicePending=_battleMenuTouchActive=_battleMenuTouchCooling=NO;
        [self clearNamingInputLocked]; [self clearCaptureInputLocked]; [self clearEvolutionInputLocked]; [self clearPokedexInputLocked];
        if (!optionsContext || !_optionsOwnerGeneration) [self clearOptionsInputLocked];
    }
    if (!optionsContext) [self clearOptionsInputLocked];
    _optionsOwnerGeneration=optionsContext ? currentOptions->ownerGeneration : 0;
    if (namingContext || _namingOwnerGeneration) {
        _keys=_pendingKeys=_submittedKeys=_joystickKeys=_pendingJoystickKeys=_submittedJoystickKeys=0;
        _touchX=_touchY=0; _touchPressed=NO; _joystickNeedsReset=YES;
        _battleChoicePending=NO; _battleChoiceRevision=_battleChoiceIndex=0;
        _partyChoicePending=_partyTouchActive=_bagChoicePending=_bagTouchActive=NO;
        _summaryChoicePending=_summaryInputActive=_summaryTouchPressed=NO; _summaryInputKeys=0;
        _battleMenuChoicePending=_battleMenuTouchActive=_battleMenuTouchCooling=NO;
        [self clearCaptureInputLocked]; [self clearEvolutionInputLocked]; [self clearPokedexInputLocked];
        if (!namingContext || !_namingOwnerGeneration) [self clearNamingInputLocked];
    }
    if (!namingContext) [self clearNamingInputLocked];
    _namingOwnerGeneration=namingContext ? currentNaming->ownerGeneration : 0;
    _fieldMapId=fieldMap;
    // Original LCD routing: SUB_MAIN=0 touches main; MAIN_SUB=1 touches sub.
    _touchEngine=(*_displayControl>>15)&1;
    if (startMenuContext) _selectedEngine=0;
    else if (optionsContext) _selectedEngine=0;
    else if (namingContext) _selectedEngine=0;
    else if (captureContext) _selectedEngine=0;
    else if (summaryContext) _selectedEngine=(summaryLearning ? currentSummary->phase==HG_SUMMARY_UI_FORGET_MOVE || currentSummary->phase==HG_SUMMARY_UI_FORGET_CONFIRM
        : currentSummary->page<=2 && (currentSummary->phase==HG_SUMMARY_UI_INPUT || (currentSummary->phase==HG_SUMMARY_UI_MOVE_SELECT && currentSummary->page==1))) ? 0 : _touchEngine;
    else if (pcBoxContext) _selectedEngine=0;
    else if (fieldChoiceContext) _selectedEngine=1;
    else if (currentEvolution->active) _selectedEngine=0;
    else if (pokedexContext) _selectedEngine=0;
    else if (battleChild) _selectedEngine=battleMenuKind ? 0 : _touchEngine;
    else if (fieldMap || (currentParty->active && !currentParty->panelLayout && (!currentParty->context || HGNativePartyUIIsItemProgress(currentParty) || HGNativePartyUIIsPotionUse(currentParty) || HGNativePartyUIIsTMHM(currentParty) || HGNativePartyUIIsRareCandy(currentParty)))) _selectedEngine=0;
    else if (currentBag->active && (!currentBag->mode || HGNativeBagUIIsList(currentBag))) _selectedEngine=HGNativeBagUIIsList(currentBag) || HGNativeBagUIIsTMHM(currentBag) || currentBag->phase==HG_BAG_UI_ITEM_MENU ? 0 : _touchEngine;
    else if (currentBattle->active && (currentBattle->mode || _automaticMenuEngine)) _selectedEngine=0;
    else if (_automaticMenuEngine) _selectedEngine=_touchEngine;
    if ((!fieldMap || battleChild || currentEvolution->active || currentPokedex->active || captureContext) && (_joystickKeys || _pendingJoystickKeys)) { _joystickKeys=_pendingJoystickKeys=_submittedJoystickKeys=0; _joystickNeedsReset=YES; }
    if (captureContext) {
        _keys=_pendingKeys=_submittedKeys=_joystickKeys=_pendingJoystickKeys=_submittedJoystickKeys=0;
        _touchX=_touchY=0; _touchPressed=NO; _joystickNeedsReset=YES; _battleChoicePending=NO; _battleChoiceRevision=_battleChoiceIndex=0;
        _partyChoicePending=_partyTouchActive=_bagChoicePending=_bagTouchActive=NO;
        _summaryChoicePending=_summaryInputActive=_summaryTouchPressed=NO; _summaryInputKeys=0;
        _battleMenuChoicePending=_battleMenuTouchActive=_battleMenuTouchCooling=NO;
        [self clearEvolutionInputLocked]; [self clearPokedexInputLocked];
#ifdef HEARTGOLD_ORIGINAL_APP_CHECK
        _checkBattleMenusRequestPending=NO;
#ifdef HEARTGOLD_ORIGINAL_SUMMARY_TM_CHECK
        [self clearSummaryTMTransientLocked];
#endif
#endif
    } else [self clearCaptureInputLocked];
    if (pokedexContext) {
        _keys=_pendingKeys=_submittedKeys=0; _touchPressed=NO; _battleChoicePending=NO;
        _partyChoicePending=_partyTouchActive=_bagChoicePending=_bagTouchActive=NO;
        _summaryChoicePending=_summaryInputActive=_summaryTouchPressed=NO; _summaryInputKeys=0;
        _battleMenuChoicePending=_battleMenuTouchActive=_battleMenuTouchCooling=NO;
    }
    // The owner can reach this choice before UIKit hides the previous message's A/B controls.
    if (currentBattle->active && OriginalBattleChoiceMode(currentBattle->mode)) { _keys&=~3u; _pendingKeys&=~3u; _submittedKeys&=~3u; _touchPressed=NO; }
    if (currentEvolution->active) {
        _keys=_pendingKeys=_submittedKeys=0; _touchPressed=NO; _battleChoicePending=NO;
        if (!summaryContext) {
            _partyChoicePending=_partyTouchActive=_bagChoicePending=_bagTouchActive=NO;
            _summaryChoicePending=_summaryInputActive=_summaryTouchPressed=NO; _summaryInputKeys=0;
            _battleMenuChoicePending=_battleMenuTouchActive=_battleMenuTouchCooling=NO;
        }
    }
    if (_audio.failure) { _failure = _audio.failure; _stopping = YES;
        [self clearStartMenuInputLocked];
#ifdef HEARTGOLD_ORIGINAL_PC_BOX_CHECK
        [self clearPCBoxCheckTransientLocked:YES];
#endif
        [self clearTrainerCardInputLocked]; [self clearFieldChoiceInputLocked]; [self clearPCBoxInputLocked]; [self clearOptionsInputLocked]; [self clearNamingInputLocked]; [self clearCaptureInputLocked]; }
    BOOL stopping = _stopping;
    uint32_t keys = startMenuBlocksInput || trainerContext || optionsContext || namingContext || captureContext || pcBoxContext || fieldChoiceContext ? 0 : [self takeKeysLocked], x = _touchX, y = _touchY, engine = _selectedEngine,preferredWidth=_preferredViewWidth,preferredBattleWidth=_preferredBattleWidth;
    BOOL battleChoice = !trainerContext && !optionsContext && !namingContext && !captureContext && !pcBoxActive && !fieldChoiceContext && _battleChoicePending;
    uint32_t battleRevision = _battleChoiceRevision, battleIndex = _battleChoiceIndex;
    _battleChoicePending = NO;
    BOOL pressed = _touchPressed;
    uint32_t startKeys=0,startX=0,startY=0;BOOL startPressed=NO;
    [self takeStartMenuInputLocked:currentStartMenu blocked:startMenuBlocked reads:HGNativeInputPadReads(_env) keys:&startKeys x:&startX y:&startY pressed:&startPressed];
    uint32_t trainerKeys=0,trainerX=0,trainerY=0;BOOL trainerPressed=NO;
    [self takeTrainerCardInputLocked:currentTrainer reads:HGNativeInputPadReads(_env) keys:&trainerKeys x:&trainerX y:&trainerY pressed:&trainerPressed];
    if (_startMenuInputCooling || _startMenuReleasePending) {keys=x=y=0;pressed=NO;battleChoice=NO;}
    else if (trainerContext || _trainerCardTouchCooling || _trainerCardReleasePending || _trainerCardInputActive || _trainerCardInputCooling) {
        keys=trainerKeys;x=trainerX;y=trainerY;pressed=trainerPressed;engine=0;battleChoice=NO;
    } else if (optionsContext) {
        x=y=0; pressed=NO;
        if (!_paused && !stopping) [self takeOptionsInputLocked:currentOptions reads:HGNativeInputPadReads(_env) keys:&keys];
        else keys=0;
    } else if (namingContext) {
        if (!_paused && !stopping) [self takeNamingInputLocked:currentNaming reads:HGNativeInputPadReads(_env) keys:&keys x:&x y:&y pressed:&pressed];
        else {keys=x=y=0;pressed=NO;}
    } else if (captureContext) {
        if (!_paused && !stopping) [self takeCaptureInputLocked:currentCapture reads:HGNativeInputPadReads(_env) keys:&keys];
    } else if (pcBoxContext) {
        x=y=0; pressed=NO;
        if (!_paused && !stopping) [self takePCBoxInputLocked:currentPCBox reads:HGNativeInputPadReads(_env) keys:&keys];
        else keys=0;
    } else if (fieldChoiceContext) {
        x=y=0; pressed=NO;
        if (!_paused && !stopping) [self takeFieldChoiceInputLocked:currentFieldChoice reads:HGNativeInputPadReads(_env) keys:&keys];
        else keys=0;
    } else if (startMenuBlocksInput) {
        keys=startKeys;x=startX;y=startY;pressed=startPressed;engine=0;battleChoice=NO;
    } else {
#ifdef HEARTGOLD_ORIGINAL_CAPTURE_REGISTRATION_CHECK
    if (!_paused && !stopping) [self takeCaptureRegistrationBootLocked:&captureQA keys:&keys x:&x y:&y pressed:&pressed];
#endif
#ifdef HEARTGOLD_ORIGINAL_SUMMARY_TM_CHECK
    if (!_paused && !stopping) [self takeSummaryTMInputLocked:summaryTMObserved ? &summaryTM : NULL bag:currentBag reads:HGNativeInputPadReads(_env) keys:&keys x:&x y:&y pressed:&pressed];
#endif
    if (!_paused && !stopping) [self takePartyTouchLocked:currentParty reads:HGNativeInputPadReads(_env) keys:&keys x:&x y:&y pressed:&pressed];
    if (!_paused && !stopping && HGNativeBagUIIsTMHM(currentBag)) { _keys&=~3u; _pendingKeys&=~3u; _submittedKeys&=~3u; keys&=~3u; _touchPressed=NO; pressed=NO; }
    if (!_paused && !stopping) [self takeBagTouchLocked:currentBag reads:HGNativeInputPadReads(_env) x:&x y:&y pressed:&pressed];
    if (!_paused && !stopping) [self takeBattleMenuTouchLocked:currentBattleBag party:currentBattleParty reads:HGNativeInputPadReads(_env) x:&x y:&y pressed:&pressed];
    if (!_paused && !stopping) [self takeEvolutionInputLocked:currentEvolution summary:summaryContext reads:HGNativeInputPadReads(_env) keys:&keys x:&x y:&y pressed:&pressed];
    if (!_paused && !stopping) [self takeSummaryInputLocked:currentSummary reads:HGNativeInputPadReads(_env) keys:&keys x:&x y:&y pressed:&pressed];
    if (!_paused && !stopping) [self takePokedexInputLocked:currentPokedex blocked:pokedexBlocked reads:HGNativeInputPadReads(_env) keys:&keys x:&x y:&y pressed:&pressed];
    }
#ifdef HEARTGOLD_ORIGINAL_CAPTURE_REGISTRATION_CHECK
    uint32_t captureLeaseReads=HGNativeInputPadReads(_env);
    if (!captureLeaseWasActive && _captureInputActive) _captureQAInputPresses++;
    if (captureLeaseWasActive && !_captureInputActive) {_captureQAInputReleases++;_captureQALastReleaseReads=captureLeaseReads;}
    if (captureLeaseWasActive) _captureQAMaxHeldReads=MAX(_captureQAMaxHeldReads,captureLeaseReads-captureLeaseStart);
#endif
#ifdef HEARTGOLD_ORIGINAL_NAMING_CHECK
    uint32_t namingLeaseReads=HGNativeInputPadReads(_env);
    if (!namingLeaseWasActive && _namingInputActive) {
        _namingQAInputPresses++;
        if (!_namingQAInputs) _namingQAInputs=[NSMutableArray new];
        if (_namingQAInputs.count<64) [_namingQAInputs addObject:@{@"generation":@(_namingInput.ownerGeneration),@"revision":@(_namingInput.revision),
            @"keys":@(_namingInputKeys),@"touch":@(_namingInputTouch),@"x":@(_namingInputX),@"y":@(_namingInputY),@"padReads":@(namingLeaseReads)}];
        else _namingQAOverflow=YES;
    }
    if (namingLeaseWasActive && !_namingInputActive) {_namingQAInputReleases++;_namingQALastReleaseReads=namingLeaseReads;}
    if (namingLeaseWasActive) _namingQAMaxHeldReads=MAX(_namingQAMaxHeldReads,namingLeaseReads-namingLeaseStart);
#endif
    BOOL menuOpen=_fieldMenuOpen;
#ifdef HEARTGOLD_ORIGINAL_PC_BOX_CHECK
    [self observePCBoxCheckPulseOwnerLocked:&pcOwnerQA reads:HGNativeInputPadReads(_env) keys:&keys pressed:pressed];
#endif
#ifdef HEARTGOLD_ORIGINAL_APP_CHECK
    BOOL checkReset=_checkResetPending; _checkResetPending=NO;
#if defined(HEARTGOLD_ORIGINAL_CAPTURE_REGISTRATION_CHECK)
    BOOL captureRequest=_captureQARequestPending;
    uint32_t captureRevision=_captureQARequestRevision,captureVariant=_captureQARequestVariant;
    BOOL captureIdle=!captureContext && [self captureRegistrationInputIdleLocked] && !keys && !pressed;
    _captureQARequestPending=NO; _captureQARequestInFlight=captureRequest;
#elif defined(HEARTGOLD_ORIGINAL_SUMMARY_TM_CHECK)
    BOOL summaryTMRequest=_summaryTMRequestPending,summaryTMIdle=!captureContext && [self summaryTMInputIdleLocked] && !keys && !pressed;
    uint32_t summaryTMRevision=_summaryTMRequestRevision; _summaryTMRequestPending=NO; _summaryTMFixturePreparing=summaryTMRequest;
#elif !defined(HEARTGOLD_ORIGINAL_PC_BOX_CHECK)
    BOOL checkBattleMenusRequest=_checkBattleMenusRequestPending; uint32_t checkBattleMenusRevision=_checkBattleMenusRequestRevision; _checkBattleMenusRequestPending=NO;
#endif
#endif
    [_condition unlock];
    if (stopping) w2c_env_HGNativeResetControl(_env, HG_RESET_REQUEST, 0);
#ifdef HEARTGOLD_ORIGINAL_APP_CHECK
    if (checkReset) w2c_env_HGNativeResetControl(_env, HG_RESET_REQUEST, 1);
#if defined(HEARTGOLD_ORIGINAL_CAPTURE_REGISTRATION_CHECK)
    if (captureRequest && !stopping) [self processCaptureRegistrationRequest:captureRevision variant:captureVariant idle:captureIdle];
    else if (captureRequest) {[_condition lock];_captureQARequestInFlight=NO;[_condition unlock];}
#elif defined(HEARTGOLD_ORIGINAL_SUMMARY_TM_CHECK)
    if (summaryTMRequest && !stopping) {
        HGSummaryLearnQAState live={0};
#ifdef HEARTGOLD_ORIGINAL_POKEDEX_CHECK
        BOOL valid=[self readSummaryTMQA:&live] && live.revision==summaryTMRevision && summaryTMIdle
            && _summaryTMBootStep==3 && !_summaryTMBootFrame && !_paused
            && [self pokedexFixtureAdmission:&live bag:currentBag dex:currentPokedex map:fieldMap idle:summaryTMIdle];
#else
        BOOL valid=[self readSummaryTMQA:&live] && live.revision==summaryTMRevision && live.fieldReady && !live.owner
            && live.fadeFinished && fieldMap==MAP_ROUTE_29 && !menuOpen && summaryTMIdle && _summaryTMBootStep==3 && !_summaryTMBootFrame && !_paused;
#endif
        BOOL accepted=valid && [self prepareSummaryTMFixture:&live];
        if (!valid) _summaryTMFixtureError=@"fixture_request_no_longer_ready";
        [_condition lock]; _summaryTMRequestAccepted=accepted; _summaryTMFixturePreparing=NO; [_condition unlock];
    }
#elif !defined(HEARTGOLD_ORIGINAL_PC_BOX_CHECK)
    if (checkBattleMenusRequest && !stopping && !captureContext) {
        BOOL accepted=w2c_hgengine_HGNativeBattleMenusQARequest(_game,checkBattleMenusRevision)!=0;
        [_condition lock]; _checkBattleMenusRequestAccepted=accepted; [_condition unlock];
    }
#endif
#endif
    // Original sound queries can be 30 Hz; service PCM at every display event, before and after drawing.
    if (!HGNativeSoundRealtime(_env, YES)) wasm_rt_trap(WASM_RT_TRAP_OOB);
    if (battleChoice) w2c_hgengine_HGNativeBattleUIChoose(_game, battleRevision, battleIndex);
    if (!HGNativeSetInput(_env, keys, NO, x, y, pressed)) wasm_rt_trap(WASM_RT_TRAP_OOB);
    uint32_t tick = OS_GetVBlankCount();
    if (tick - _lastFrame < 2) return;
#ifdef HEARTGOLD_ORIGINAL_APP_CHECK
    CheckAudioAppSpan trace={HG_AUDIO_CHECK_APP_COPY,HGNativeAudioCheckNow(),
        (fieldMap ? 1u : 0u)|((currentBattle->active || battleChild) ? 2u : 0u)|(currentParty->active ? 4u : 0u)
            |(currentBag->active ? 8u : 0u)|(currentSummary->active ? 16u : 0u)|(engine ? 32u : 0u)
            |(currentBattleBag->active ? 64u : 0u)|(currentBattleParty->active ? 128u : 0u),tick};
    BOOL tracePublished=NO;
    // Complete the diagnostic span on every early render return.
    @try {
#endif
    _lastFrame = tick; HGNativeDrainDMA();
    NSData *battleUI = [NSData dataWithBytes:_env->memory->data + battleAddress length:sizeof(HGNativeBattleUIState)];
    NSData *partyUI = [NSData dataWithBytes:currentParty length:sizeof(*currentParty)];
    NSData *bagUI = [NSData dataWithBytes:currentBag length:sizeof(*currentBag)];
    NSData *summaryUI = [NSData dataWithBytes:currentSummary length:sizeof(*currentSummary)];
    NSData *pcBoxUI = [NSData dataWithBytes:currentPCBox length:sizeof(*currentPCBox)];
    NSData *fieldChoiceUI = [NSData dataWithBytes:currentFieldChoice length:sizeof(*currentFieldChoice)];
    NSData *evolutionUI = [NSData dataWithBytes:currentEvolution length:sizeof(*currentEvolution)];
    NSData *optionsUI = [NSData dataWithBytes:currentOptions length:sizeof(*currentOptions)];
    NSData *trainerUI=[NSData dataWithBytes:currentTrainer length:sizeof(*currentTrainer)];
    NSData *startMenuUI=[NSData dataWithBytes:currentStartMenu length:sizeof(*currentStartMenu)];
    NSData *namingUI = [NSData dataWithBytes:currentNaming length:sizeof(*currentNaming)];
    NSData *captureUI = [NSData dataWithBytes:currentCapture length:sizeof(*currentCapture)];
    NSData *pokedexUI = [NSData dataWithBytes:currentPokedex length:sizeof(*currentPokedex)];
    NSData *battleBagUI=[NSData dataWithBytes:currentBattleBag length:sizeof(*currentBattleBag)],*battlePartyUI=[NSData dataWithBytes:currentBattleParty length:sizeof(*currentBattleParty)];
#ifdef HEARTGOLD_ORIGINAL_APP_CHECK
#if defined(HEARTGOLD_ORIGINAL_CAPTURE_REGISTRATION_CHECK)
    HGNativeCaptureQAState publishedCapture={0}; [self readCaptureRegistrationQA:&publishedCapture];
    NSData *captureQAData=[NSData dataWithBytes:&publishedCapture length:sizeof(publishedCapture)];
#elif defined(HEARTGOLD_ORIGINAL_SUMMARY_TM_CHECK)
    HGSummaryLearnQAState publishedTM={0}; BOOL haveTM=[self readSummaryTMQA:&publishedTM];
    NSData *summaryTMData=haveTM ? [NSData dataWithBytes:&publishedTM length:sizeof(publishedTM)] : nil;
    NSDictionary *summaryTMFacts=[self summaryTMCurrentFacts];
    UIImage *summaryTMWarning=haveTM && publishedTM.owner==HG_SUMMARY_LEARN_SUMMARY && publishedTM.graphicsReady && currentSummary->ready
        ? [self summaryTMWarningImage:&publishedTM.hmWarning] : nil;
#elif !defined(HEARTGOLD_ORIGINAL_PC_BOX_CHECK)
    uint32_t qaAddress=w2c_hgengine_HGNativeBattleMenusQAGetState(_game);
    if (!qaAddress || qaAddress>_env->memory->size || sizeof(HGNativeBattleMenusQAState)>_env->memory->size-qaAddress) wasm_rt_trap(WASM_RT_TRAP_OOB);
    const HGNativeBattleMenusQAState *qa=(const HGNativeBattleMenusQAState *)(_env->memory->data+qaAddress);
    if (qa->version!=HG_BATTLE_MENUS_QA_VERSION || qa->byteSize!=sizeof(*qa)) wasm_rt_trap(WASM_RT_TRAP_OOB);
    NSData *battleMenusQA=[NSData dataWithBytes:qa length:sizeof(*qa)];
#endif
#endif
    const HGNativeBattleUIState *battleState = battleUI.bytes;
    BOOL originalPage=startMenuContext || trainerContext || optionsContext || namingContext || captureContext || summaryContext || pcBoxActive || fieldChoiceContext || currentEvolution->active || pokedexContext || (currentParty->active && !currentParty->panelLayout && (!currentParty->context || HGNativePartyUIIsItemProgress(currentParty) || HGNativePartyUIIsPotionUse(currentParty) || HGNativePartyUIIsTMHM(currentParty) || HGNativePartyUIIsRareCandy(currentParty)))
        || (currentBag->active && (!currentBag->mode || HGNativeBagUIIsList(currentBag)));
    BOOL battleScene=(battleState->active || battleChild) && !engine && !originalPage;
    uint32_t sceneWidth=originalPage ? 256 : fieldMap ? MAX(256,preferredWidth) : 256;
    uint32_t width=battleScene ? MAX(256,preferredBattleWidth) : engine ? 256 : sceneWidth;
    uint32_t geometryWidth=battleScene ? MIN(512,width) : sceneWidth;
    if (!w2c_hgengine_HGNativeSetViewWidth(_game,geometryWidth)) wasm_rt_trap(WASM_RT_TRAP_OOB);
    if (!_frameBuffer) _frameBuffer = w2c_hgengine_HGNativeFrameBuffer(_game);
    uint32_t size = width * 192 * 4;
    if (!_frameBuffer || _frameBuffer > _env->memory->size || size > _env->memory->size - _frameBuffer) wasm_rt_trap(WASM_RT_TRAP_OOB);
#ifdef HEARTGOLD_ORIGINAL_APP_CHECK
    CheckAudioAppStage(&trace,HG_AUDIO_CHECK_APP_RENDER,1);
#endif
    if (pcBoxActive && memcmp(pcBoxUI.bytes,currentPCBox,sizeof(*currentPCBox))) return;
    if (memcmp(fieldChoiceUI.bytes,currentFieldChoice,sizeof(*currentFieldChoice))) return;
    if (!fieldChoiceView || _fieldChoiceFrameGeneration!=currentFieldChoice->ownerGeneration) {
        _fieldChoiceOwnerFrame=nil; _fieldChoiceOwnerPreview=nil; _fieldChoiceFrameGeneration=0;
    }
    BOOL frozenChoice=fieldChoiceContext && !currentFieldChoice->viewActive && _fieldChoiceOwnerFrame.length==256*192*4;
    BOOL rendered=frozenChoice || (battleScene ? w2c_hgengine_HGNativeRenderBattleView(_game,width,_frameBuffer,size) : w2c_hgengine_HGNativeRender2DView(_game,engine,width,_frameBuffer,size));
    if (!rendered) {
        if (width==256) return;
        width=256; size=width*192*4;
        if (!w2c_hgengine_HGNativeSetViewWidth(_game,width) || !w2c_hgengine_HGNativeRender2D(_game,engine,_frameBuffer,size)) return;
    }
#ifdef HEARTGOLD_ORIGINAL_APP_CHECK
    CheckAudioAppStage(&trace,HG_AUDIO_CHECK_APP_COPY,1);
#endif
    NSData *frame = frozenChoice ? _fieldChoiceOwnerFrame : [NSData dataWithBytes:_env->memory->data + _frameBuffer length:size];
    UIImage *fieldChoicePreview=nil;
    if (fieldChoiceContext) {
        if (memcmp(fieldChoiceUI.bytes,currentFieldChoice,sizeof(*currentFieldChoice))) return;
        if (currentFieldChoice->viewActive==1) {
#ifdef HEARTGOLD_ORIGINAL_APP_CHECK
            CheckAudioAppStage(&trace,HG_AUDIO_CHECK_APP_RENDER,1);
#endif
            if (!w2c_hgengine_HGNativeRender2D(_game,0,_frameBuffer,256*192*4)) return;
#ifdef HEARTGOLD_ORIGINAL_APP_CHECK
            CheckAudioAppStage(&trace,HG_AUDIO_CHECK_APP_COPY,1);
#endif
            NSData *mainFrame=[NSData dataWithBytes:_env->memory->data+_frameBuffer length:256*192*4];
            fieldChoicePreview=OriginalFrameRegion(mainFrame,0,144,256,48);
            if (!fieldChoicePreview || memcmp(fieldChoiceUI.bytes,currentFieldChoice,sizeof(*currentFieldChoice))) return;
            _fieldChoiceOwnerFrame=frame; _fieldChoiceOwnerPreview=fieldChoicePreview; _fieldChoiceFrameGeneration=currentFieldChoice->ownerGeneration;
        } else fieldChoicePreview=_fieldChoiceOwnerPreview;
    }
    NSData *startMenuMain=nil,*startMenuSub=nil;uint32_t startMenuFrameEngine=(*_displayControl>>15)&1;
    if (startMenuView) {
        if (memcmp(startMenuUI.bytes,currentStartMenu,sizeof(*currentStartMenu))) return;
        if (!w2c_hgengine_HGNativeRender2D(_game,0,_frameBuffer,256*192*4)) return;
        startMenuMain=[NSData dataWithBytes:_env->memory->data+_frameBuffer length:256*192*4];
        if (!w2c_hgengine_HGNativeRender2D(_game,1,_frameBuffer,256*192*4)) return;
        startMenuSub=[NSData dataWithBytes:_env->memory->data+_frameBuffer length:256*192*4];
        if (memcmp(startMenuUI.bytes,currentStartMenu,sizeof(*currentStartMenu)) || startMenuFrameEngine!=((*_displayControl>>15)&1)) return;
    }
    NSDictionary *trainerPictures=nil;
    if (trainerContext && currentTrainer->viewActive==1) {
        if (memcmp(trainerUI.bytes,currentTrainer,sizeof(*currentTrainer))) return;
        if (!w2c_hgengine_HGNativeRender2D(_game,0,_frameBuffer,256*192*4)) return;
        NSData *trainerMain=[NSData dataWithBytes:_env->memory->data+_frameBuffer length:256*192*4];
        if (!w2c_hgengine_HGNativeRender2D(_game,1,_frameBuffer,256*192*4)) return;
        NSData *trainerSub=[NSData dataWithBytes:_env->memory->data+_frameBuffer length:256*192*4];
        if (memcmp(trainerUI.bytes,currentTrainer,sizeof(*currentTrainer))) return;
        trainerPictures=[self copyTrainerCardPictures:trainerUI.bytes main:trainerMain sub:trainerSub];
        if (!trainerPictures) return;
    }
    NSDictionary *pcBoxPictures=nil;
    if (pcBoxActive) {
        if (memcmp(pcBoxUI.bytes,currentPCBox,sizeof(*currentPCBox))) return;
        if (pcBoxContext && currentPCBox->viewActive==1) {
#ifdef HEARTGOLD_ORIGINAL_APP_CHECK
            CheckAudioAppStage(&trace,HG_AUDIO_CHECK_APP_RENDER,1);
#endif
            if (!w2c_hgengine_HGNativeRender2D(_game,1,_frameBuffer,256*192*4)) return;
#ifdef HEARTGOLD_ORIGINAL_APP_CHECK
            CheckAudioAppStage(&trace,HG_AUDIO_CHECK_APP_COPY,1);
#endif
            NSData *subFrame=[NSData dataWithBytes:_env->memory->data+_frameBuffer length:256*192*4];
            if (memcmp(pcBoxUI.bytes,currentPCBox,sizeof(*currentPCBox))) return;
            pcBoxPictures=[self copyPCBoxPictures:pcBoxUI.bytes main:frame sub:subFrame];
            if (!pcBoxPictures) return;
        } else pcBoxPictures=_pcBoxOwnerPictures;
    } else _pcBoxOwnerPictures=nil;
    UIImage *optionsPreview=nil;
    if (optionsContext) {
        if (memcmp(optionsUI.bytes,currentOptions,sizeof(*currentOptions)) || !w2c_hgengine_HGNativeRender2D(_game,1,_frameBuffer,256*192*4)) return;
        NSData *subFrame=[NSData dataWithBytes:_env->memory->data+_frameBuffer length:256*192*4];
        // Preserve frame2 around the internal(16,152,216,32) text window.
        optionsPreview=OriginalFrameRegion(subFrame,0,144,256,48);
        if (!optionsPreview || memcmp(optionsUI.bytes,currentOptions,sizeof(*currentOptions))) return;
    }
    NSDictionary *evolutionPictures=(trainerContext || optionsContext || namingContext || captureContext) ? nil : [self copyEvolutionPictures:evolutionUI.bytes];
    NSDictionary *partyPictures=(trainerContext || optionsContext || namingContext || captureContext) ? nil : [self copyPartyPictures:partyUI.bytes frame:frame];
    NSData *bagSubFrame=nil;
    if (!trainerContext && !optionsContext && !namingContext && !captureContext && currentBag->active && (!currentBag->mode || HGNativeBagUIIsList(currentBag))) {
#ifdef HEARTGOLD_ORIGINAL_APP_CHECK
        CheckAudioAppStage(&trace,HG_AUDIO_CHECK_APP_RENDER,1);
#endif
        if (!w2c_hgengine_HGNativeRender2D(_game,1,_frameBuffer,256*192*4)) return;
#ifdef HEARTGOLD_ORIGINAL_APP_CHECK
        CheckAudioAppStage(&trace,HG_AUDIO_CHECK_APP_COPY,1);
#endif
        bagSubFrame=[NSData dataWithBytes:_env->memory->data+_frameBuffer length:256*192*4];
    }
    NSDictionary *bagPictures=(trainerContext || optionsContext || namingContext || captureContext) ? nil : [self copyBagPictures:bagUI.bytes frame:frame subFrame:bagSubFrame];
    NSData *summarySubFrame=nil;
    if (!trainerContext && !optionsContext && !namingContext && !captureContext && summaryContext) {
#ifdef HEARTGOLD_ORIGINAL_APP_CHECK
        CheckAudioAppStage(&trace,HG_AUDIO_CHECK_APP_RENDER,1);
#endif
        if (!w2c_hgengine_HGNativeRender2D(_game,1,_frameBuffer,256*192*4)) return;
#ifdef HEARTGOLD_ORIGINAL_APP_CHECK
        CheckAudioAppStage(&trace,HG_AUDIO_CHECK_APP_COPY,1);
#endif
        summarySubFrame=[NSData dataWithBytes:_env->memory->data+_frameBuffer length:256*192*4];
    }
    NSDictionary *summaryPictures=(trainerContext || optionsContext || namingContext || captureContext) ? nil : [self copySummaryPictures:summaryUI.bytes frame:frame subFrame:summarySubFrame];
    NSData *battleMenuSubFrame=nil;
    if (!trainerContext && !optionsContext && !namingContext && !captureContext && battleMenuKind) {
#ifdef HEARTGOLD_ORIGINAL_APP_CHECK
        CheckAudioAppStage(&trace,HG_AUDIO_CHECK_APP_RENDER,1);
#endif
        if (!w2c_hgengine_HGNativeRender2D(_game,1,_frameBuffer,256*192*4)) return;
#ifdef HEARTGOLD_ORIGINAL_APP_CHECK
        CheckAudioAppStage(&trace,HG_AUDIO_CHECK_APP_COPY,1);
#endif
        battleMenuSubFrame=[NSData dataWithBytes:_env->memory->data+_frameBuffer length:256*192*4];
    }
    NSDictionary *battleMenuPictures=(trainerContext || optionsContext || namingContext || captureContext) ? nil : [self copyBattleMenuPictures:battleBagUI.bytes party:battlePartyUI.bytes frame:frame subFrame:battleMenuSubFrame];
    if (!trainerContext && !optionsContext && !namingContext && !captureContext && battleMenuKind && !battleMenuPictures) return;
    NSData *pokedexSubFrame=nil;
    BOOL pokedexPassiveMessage=pokedexContext && currentPokedex->phase==23 && currentPokedex->family==HG_POKEDEX_UI_FAMILY_NONE
        && currentPokedex->busyReason==HG_POKEDEX_UI_BUSY_TRANSITION && !currentPokedex->ready && !currentPokedex->keyEnabledMask
        && !currentPokedex->controlCount && !currentPokedex->controlEnabledMask && !currentPokedex->visualCount;
    if (pokedexContext && (currentPokedex->visualCount || pokedexPassiveMessage)) {
#ifdef HEARTGOLD_ORIGINAL_APP_CHECK
        CheckAudioAppStage(&trace,HG_AUDIO_CHECK_APP_RENDER,1);
#endif
        if (!w2c_hgengine_HGNativeRender2D(_game,1,_frameBuffer,256*192*4)) return;
#ifdef HEARTGOLD_ORIGINAL_APP_CHECK
        CheckAudioAppStage(&trace,HG_AUDIO_CHECK_APP_COPY,1);
#endif
        pokedexSubFrame=[NSData dataWithBytes:_env->memory->data+_frameBuffer length:256*192*4];
        if (memcmp(pokedexUI.bytes,currentPokedex,sizeof(*currentPokedex))) return;
    }
    NSDictionary *pokedexPictures=pokedexContext ? [self copyPokedexPictures:pokedexUI.bytes main:frame sub:pokedexSubFrame] : nil;
    if (pokedexContext && (currentPokedex->visualCount || pokedexPassiveMessage) && !pokedexPictures) return;
    if (pokedexContext && memcmp(pokedexUI.bytes,currentPokedex,sizeof(*currentPokedex))) return;
    if (!pokedexContext) _pokedexOwnerPictures=nil;
    uint32_t icons=w2c_hgengine_HGNativeFieldMenuIcons(_game);
    NSData *menuFrame=nil;
    if (!startMenuContext && !trainerContext && !optionsContext && !namingContext && !captureContext && !pcBoxActive && !fieldChoiceContext && fieldMap && menuOpen) {
#ifdef HEARTGOLD_ORIGINAL_APP_CHECK
        CheckAudioAppStage(&trace,HG_AUDIO_CHECK_APP_RENDER,1);
#endif
        BOOL menuRendered=w2c_hgengine_HGNativeRender2D(_game,1,_frameBuffer,256*192*4);
#ifdef HEARTGOLD_ORIGINAL_APP_CHECK
        CheckAudioAppStage(&trace,HG_AUDIO_CHECK_APP_COPY,menuRendered);
#endif
        if (menuRendered) menuFrame=[NSData dataWithBytes:_env->memory->data+_frameBuffer length:256*192*4];
    }
#ifdef HEARTGOLD_ORIGINAL_APP_CHECK
    // Read the published geometry on its owner thread, after copying the displayed pixels.
    NSData *geometry = w2c_hgengine_HGNativeGeometryFrame(_game, 1, _frameBuffer) ? [NSData dataWithBytes:_env->memory->data + _frameBuffer length:8 * sizeof(uint32_t)] : nil;
#endif
    if (!HGNativeSoundRealtime(_env, YES)) wasm_rt_trap(WASM_RT_TRAP_OOB);
    if (optionsContext && memcmp(optionsUI.bytes,currentOptions,sizeof(*currentOptions))) return;
    if (memcmp(trainerUI.bytes,currentTrainer,sizeof(*currentTrainer))) return;
    if (memcmp(startMenuUI.bytes,currentStartMenu,sizeof(*currentStartMenu)) || (startMenuView && startMenuFrameEngine!=((*_displayControl>>15)&1))) return;
    if (memcmp(pcBoxUI.bytes,currentPCBox,sizeof(*currentPCBox))) return;
    if (memcmp(fieldChoiceUI.bytes,currentFieldChoice,sizeof(*currentFieldChoice))) return;
#ifdef HEARTGOLD_ORIGINAL_PC_BOX_CHECK
    HGPCBoxQAState pcPublishedQA={0}; BOOL pcQACoherent=[self readPCBoxQAOwner:&pcPublishedQA]
        && !memcmp(&pcPublishedQA.pc,currentPCBox,sizeof(*currentPCBox))
        && !memcmp(&pcPublishedQA.summary,currentSummary,sizeof(*currentSummary))
        && !memcmp(&pcPublishedQA.naming,currentNaming,sizeof(*currentNaming));
    NSData *pcQAData=pcQACoherent ? [NSData dataWithBytes:&pcPublishedQA length:sizeof(pcPublishedQA)] : nil;
#endif
#ifdef HEARTGOLD_ORIGINAL_DEFAULT_CHECK
    BOOL paletteFadeFinished=w2c_hgengine_IsPaletteFadeFinished(_game)!=0;
#endif
    [_condition lock]; _frame = frame; _frameWidth=width; _frameEngine=engine; _battleUIState = battleUI;
    _partyUIState=partyUI; _partyPictures=partyPictures;
    _bagUIState=bagUI; _bagPictures=bagPictures;
    _summaryUIState=summaryUI; _summaryPictures=summaryPictures;
    _pcBoxUIState=pcBoxUI; _pcBoxPictures=pcBoxPictures;
    _trainerCardUIState=trainerUI;_trainerCardPictures=trainerPictures;
    _startMenuUIState=startMenuUI;_startMenuMainFrame=startMenuMain;_startMenuSubFrame=startMenuSub;_startMenuFrameEngine=startMenuFrameEngine;
    _fieldChoiceUIState=fieldChoiceUI; _fieldChoicePreviewImage=fieldChoicePreview;
#ifdef HEARTGOLD_ORIGINAL_PC_BOX_CHECK
    _pcBoxQAState=pcQAData; _pcBoxQAPadReads=HGNativeInputPadReads(_env);
    _pcBoxQAOwnerFrame=OS_GetVBlankCount(); _pcBoxQAPublishedFrame=_publishedFrames+1;
#endif
    _evolutionUIState=evolutionUI; _evolutionPictures=evolutionPictures;
    _optionsUIState=optionsUI; _optionsPreviewImage=optionsPreview;
    _namingUIState=namingUI;
    _captureUIState=captureUI;
    _pokedexUIState=pokedexUI; _pokedexPictures=pokedexPictures;
    _battleBagUIState=battleBagUI; _battlePartyUIState=battlePartyUI; _battleMenuPictures=battleMenuPictures;
    _fieldMenuFrame=menuFrame; _fieldMenuIcons=icons;
#ifdef HEARTGOLD_ORIGINAL_APP_CHECK
    _geometryState = geometry;
#if defined(HEARTGOLD_ORIGINAL_CAPTURE_REGISTRATION_CHECK)
    _captureQAState=captureQAData; _captureQAPadReads=HGNativeInputPadReads(_env);
#elif defined(HEARTGOLD_ORIGINAL_SUMMARY_TM_CHECK)
#ifdef HEARTGOLD_ORIGINAL_POKEDEX_CHECK
    NSMutableDictionary *pokedexFacts=[summaryTMFacts mutableCopy];
    pokedexFacts[@"lastOwnerStartupState"]=@{@"bootEpoch":@(_bootCount),@"publishedFrame":@(_publishedFrames+1),
        @"bootStep":@(_summaryTMBootStep),@"bootHeldFrame":@(_summaryTMBootFrame),@"padReads":@(HGNativeInputPadReads(_env)),
        @"qaObserved":@(haveTM),@"qaVersion":@(publishedTM.version),@"qaBytes":@(publishedTM.byteSize),
        @"qaFrame":@(publishedTM.frame),@"qaRevision":@(publishedTM.revision),@"qaFieldReady":@(publishedTM.fieldReady),
        @"qaOwner":@(publishedTM.owner),@"qaManager":@(publishedTM.manager),@"qaPhase":@(publishedTM.phase),
        @"qaExecState":@(publishedTM.execState),@"qaFadeFinished":@(publishedTM.fadeFinished),
        @"requestPending":@(_summaryTMRequestPending),@"requestAccepted":@(_summaryTMRequestAccepted),@"fixturePreparing":@(_summaryTMFixturePreparing),
        @"map":@(fieldMap),@"fieldMenuOpen":@(_fieldMenuOpen),@"fieldMenuIcons":@(icons),
        @"bagActive":@(currentBag->active),@"bagReady":@(currentBag->ready),@"bagPhase":@(currentBag->phase),@"dexActive":@(currentPokedex->active)};
    summaryTMFacts=pokedexFacts;
#endif
    _summaryTMQAState=summaryTMData; _summaryTMFacts=summaryTMFacts; _summaryTMHMWarningImage=summaryTMWarning;
    _summaryTMPadReads=HGNativeInputPadReads(_env); _summaryTMBootInputStep=_summaryTMBootStep; _summaryTMBootInputFrame=_summaryTMBootFrame;
#elif !defined(HEARTGOLD_ORIGINAL_PC_BOX_CHECK)
    _battleMenusQAState=battleMenusQA;
#ifdef HEARTGOLD_ORIGINAL_DEFAULT_CHECK
    _paletteFadeFinished=paletteFadeFinished;
#endif
#endif
    _publishedFrames++;
#endif
    [_condition unlock];
#ifdef HEARTGOLD_ORIGINAL_APP_CHECK
#if !defined(HEARTGOLD_ORIGINAL_CAPTURE_REGISTRATION_CHECK) && !defined(HEARTGOLD_ORIGINAL_PC_BOX_CHECK)
    [self checkPrivatePartyPublished:partyUI pictures:partyPictures frame:_publishedFrames];
#endif
    tracePublished=YES;
    } @finally {
        CheckAudioAppStage(&trace,0,tracePublished);
        [self publishAudioProducerTrace];
    }
#endif
}
- (void)runEngine {
    @autoreleasepool {
#ifdef HEARTGOLD_ORIGINAL_APP_CHECK
        qos_class_t requestedQos=QOS_CLASS_UNSPECIFIED; int relativePriority=0;
        int qosResult=pthread_get_qos_class_np(pthread_self(),&requestedQos,&relativePriority);
        fprintf(stderr,"Original owner thread: isMain=%d nsRequestedQos=%ld pthreadRequestedQos=%u relativePriority=%d queryResult=%d effectiveQos=unmeasured\n",
            NSThread.isMainThread,(long)NSThread.currentThread.qualityOfService,(unsigned)requestedQos,relativePriority,qosResult);
#endif
        NSString *root = [NSBundle.mainBundle.resourcePath stringByAppendingPathComponent:@"OriginalContent"];
        NSData *header = [NSData dataWithContentsOfFile:[root stringByAppendingPathComponent:@"boot-header.bin"]];
        NSURL *support = [NSFileManager.defaultManager URLsForDirectory:NSApplicationSupportDirectory inDomains:NSUserDomainMask].firstObject;
        support = [support URLByAppendingPathComponent:@"HeartGoldOriginal" isDirectory:YES];
        NSError *error;
        BOOL ready = header.length == 0x160 && [NSFileManager.defaultManager createDirectoryAtURL:support withIntermediateDirectories:YES attributes:nil error:&error];
        ready = ready && HGNativeFSSetRoot([root stringByAppendingPathComponent:@"files"].fileSystemRepresentation) && HGNativeBackupSetPath([[support URLByAppendingPathComponent:@"heartgold.sav"] path].fileSystemRepresentation);
        if (!ready) { [_condition lock]; _failure = @"无法打开原版资源或存档；已有存档未覆盖。"; [_condition unlock]; return; }
        wasm_rt_init();
        uint32_t bootParameter = 0;
        BOOL rebooted = NO;
        for (;;) {
            @autoreleasepool {
                [_condition lock]; BOOL stopping = _stopping;
#ifdef HEARTGOLD_ORIGINAL_APP_CHECK
                // AVAudio control calls run outside this lock; CHECK readers skip their mutable lifecycle.
                _audioChanging=!stopping;
#endif
                [_condition unlock];
                if (stopping) break;
                error = nil;
                if (![_audio open:&error]) {
                    [_audio close]; HGNativeBackupSetPath(NULL);
                    [_condition lock]; _failure = error.localizedDescription ?: @"无法初始化声音输出。";
#ifdef HEARTGOLD_ORIGINAL_APP_CHECK
                    _audioChanging=NO;
#endif
                    [_condition unlock]; return;
                }
                _game = calloc(1, sizeof(*_game)); _env = calloc(1, sizeof(*_env)); _wasi = calloc(1, sizeof(*_wasi));
                if (_wasi) for (unsigned i = 0; i < 3; i++) _wasi->fds[i] = dup(i);
                if (!_game || !_env || !_wasi || _wasi->fds[0] < 0 || _wasi->fds[1] < 0 || _wasi->fds[2] < 0) {
                    free(_game); free(_env); _game = NULL; _env = NULL;
                    if (_wasi) for (unsigned i = 0; i < 3; i++) if (_wasi->fds[i] >= 0) close(_wasi->fds[i]);
                    free(_wasi); _wasi = NULL;
                    HGNativeBackupSetPath(NULL);
                    [_audio close];
                    [_condition lock]; _failure = @"无法分配原版引擎运行资源。";
#ifdef HEARTGOLD_ORIGINAL_APP_CHECK
                    _audioChanging=NO;
#endif
                    [_condition unlock]; return;
                }
                _env->bootParameter = bootParameter; _env->resetOccurred = rebooted;
                wasm_rt_trap_t trap = wasm_rt_impl_try();
                if (!trap) {
#ifdef HEARTGOLD_ORIGINAL_APP_CHECK
                    if (!HGNativeAudioCheckReset(_bootCount+1,HGNativeAudioCheckNow())) wasm_rt_trap(WASM_RT_TRAP_OOB);
                    [_condition lock]; _audioProducerTrace=_audioFirstShortTrace=nil;
                    _audioProducerCapturedAtNs=_audioFirstShortTimeNs=_audioFirstShortCapturedAtNs=0; [_condition unlock];
#endif
                    wasm2c_hgengine_instantiate(_game, _env, _wasi);
                    _env->memory = _wasi->memory = w2c_hgengine_memory(_game);
#ifdef HEARTGOLD_ORIGINAL_POKEDEX_CHECK
                    [self resetPokedexCheckOwner];
#endif
                    _env->irqTable = &_game->w2c_T0; _env->stackPointer = &_game->w2c_0x5F_stack_pointer;
                    _env->rootStackHigh = *_env->stackPointer; *_env->stackPointer -= 16;
                    _env->irqType0 = wasm2c_hgengine_get_func_type(0, 0);
                    _env->irqType1 = wasm2c_hgengine_get_func_type(1, 0, WASM_RT_I32);
                    _env->rtcType = wasm2c_hgengine_get_func_type(2, 0, WASM_RT_I32, WASM_RT_I32);
                    _env->irqGuardType = wasm2c_hgengine_get_func_type(4, 0, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32);
                    uint32_t work = w2c_hgengine_HGNativeSystemWorkAddress(_game);
                    if (!work || work > _env->memory->size || 0x360 > _env->memory->size - work) wasm_rt_trap(WASM_RT_TRAP_OOB);
                    memcpy(_env->memory->data + work + 0x200, header.bytes, header.length);
                    if (!HGNativeSoundSetOutput(_env, outputPCM, (__bridge void *)_audio) || !HGNativeSoundRealtime(_env, YES)) wasm_rt_trap(WASM_RT_TRAP_OOB);
                    w2c_hgengine_0x5Finitialize(_game);
                    uint32_t status = w2c_hgengine_HGNativeRegisterAddress(_game, 0x04000004, 2);
                    if (!status || status % 2 || status > _env->memory->size || 2 > _env->memory->size - status) wasm_rt_trap(WASM_RT_TRAP_OOB);
                    _displayStatus = (const vu16 *)(_env->memory->data + status);
                    uint32_t control=w2c_hgengine_HGNativeRegisterAddress(_game,0x04000304,2);
                    if (!control || control%2 || control>_env->memory->size || 2>_env->memory->size-control) wasm_rt_trap(WASM_RT_TRAP_OOB);
                    _displayControl=(const vu16 *)(_env->memory->data+control);
#ifdef HEARTGOLD_ORIGINAL_APP_CHECK
                    [_condition lock]; _audioChanging=NO; _bootCount++; _bootFrameBase=_publishedFrames; _checkedBootParameter=_env->bootParameter; _checkedResetOccurred=_env->resetOccurred; [_condition unlock];
#endif
                    if (!HGNativeStartDisplayClock(_env, _displayStatus) || !HGNativeSetDisplayObserver(_env, present, (__bridge void *)self)) wasm_rt_trap(WASM_RT_TRAP_OOB);
                    w2c_hgengine_NitroMain(_game);
                }
                // shortcut: unexpected IRQ/fiber faults retain memory; only a completed normal reset is reusable.
                BOOL reset = _env->resetRequested && trap == WASM_RT_TRAP_UNREACHABLE;
                _env->soundOutput = NULL; _env->soundOutputContext = NULL;
                if (reset) HGNativeFinishReset(_env);
                else HGNativeStopDisplayClock(_env);
#ifdef HEARTGOLD_ORIGINAL_APP_CHECK
                [_condition lock]; _audioChanging=YES; [_condition unlock];
#endif
                [_audio close];
                if (!reset) {
                    [_condition lock];
                    [self clearStartMenuInputLocked];
#ifdef HEARTGOLD_ORIGINAL_PC_BOX_CHECK
                    [self clearPCBoxCheckTransientLocked:NO]; _pcBoxQAState=nil;
#endif
                    [self clearTrainerCardInputLocked]; [self clearFieldChoiceInputLocked]; [self clearPCBoxInputLocked]; [self clearOptionsInputLocked]; [self clearNamingInputLocked]; [self clearCaptureInputLocked];
#ifdef HEARTGOLD_ORIGINAL_APP_CHECK
                    _audioChanging=NO; _finished = YES;
#endif
                    if (!_failure) _failure = trap ? [NSString stringWithFormat:@"原版引擎停止：%s", wasm_rt_strerror(trap)] : @"原版引擎意外返回。";
                    [_condition unlock];
                    return;
                }
                bootParameter = _env->resetParameter;
#ifdef HEARTGOLD_ORIGINAL_APP_CHECK
                [_condition lock];
                _resetQuiesced = _env->resetFinished && !_env->threads && !_env->irqTable && !_env->sound &&
                    !_env->pendingRTC && !_env->pendingCARD && !_env->pendingDMA && !_env->pendingMIC && !_env->pendingPOWER && !_env->pendingINPUT;
                [_condition unlock];
#endif
                _displayStatus = _displayControl = NULL; _frameBuffer = _lastFrame = 0;
                wasm2c_hgengine_free(_game);
                free(_game); free(_env); _game = NULL; _env = NULL;
                for (unsigned i = 0; i < 3; i++) if (_wasi->fds[i] >= 0) close(_wasi->fds[i]);
                free(_wasi); _wasi = NULL;
                [_condition lock]; BOOL restart = !_stopping && !_failure; [_condition unlock];
#ifdef HEARTGOLD_ORIGINAL_PC_BOX_CHECK
                if (restart) {
                    [_condition lock]; [self clearPCBoxCheckTransientLocked:YES];
                    _failure=@"Private PC case reset before its normal stop"; _stopping=YES; [_condition unlock]; restart=NO;
                }
#endif
                if (!restart) break;
                rebooted = YES;
                // Publish fresh UI/audio ownership on the main queue after all old callbacks have been fenced.
                dispatch_sync(dispatch_get_main_queue(), ^{
                    HGNativeBattleUIState battle = {0};
                    HGNativePartyUIState party = {0};
                    HGNativeBagUIState bag = {0};
                    HGNativeSummaryUIState summary = {0}; HGNativeEvolutionUIState evolution={0};
                    HGNativeOptionsUIState options={0}; NSData *optionsUI=[NSData dataWithBytes:&options length:sizeof(options)];
                    HGNativeNamingUIState naming={0}; NSData *namingUI=[NSData dataWithBytes:&naming length:sizeof(naming)];
                    HGNativePCBoxUIState pcBox={0}; NSData *pcBoxUI=[NSData dataWithBytes:&pcBox length:sizeof(pcBox)];
                    HGNativeFieldChoiceUIState fieldChoice={0}; NSData *fieldChoiceUI=[NSData dataWithBytes:&fieldChoice length:sizeof(fieldChoice)];
                    HGNativeCaptureUIState capture={0}; NSData *captureUI=[NSData dataWithBytes:&capture length:sizeof(capture)];
                    HGNativePokedexUIState pokedex={0};
                    HGNativeBattleBagUIState battleBag={0}; HGNativeBattlePartyUIState battleParty={0};
                    [self updateOptionsControls:optionsUI preview:nil paused:NO];
                    [self updateNamingControls:namingUI paused:NO];
                    [self updatePCBoxControls:pcBoxUI pictures:nil blocked:NO paused:NO];
                    [self updateFieldChoiceControls:fieldChoiceUI preview:nil blocked:NO paused:NO];
                    [self updateCaptureControls:captureUI paused:NO];
                    [self->_condition lock];
                    self->_audio = [OriginalAudio new];
                    self->_keys = self->_pendingKeys = self->_submittedKeys = 0;
                    self->_joystickKeys = self->_pendingJoystickKeys = self->_submittedJoystickKeys = 0;
                    self->_lastPadReadCount = self->_touchX = self->_touchY = 0;
                    self->_inputGeneration++;
                    self->_touchPressed = self->_battleChoicePending = NO;
                    self->_battleChoiceRevision = self->_battleChoiceIndex = 0;
                    self->_partyChoicePending = self->_partyTouchActive = NO;
                    self->_partyChoiceRevision = self->_partyChoicePhase = self->_partyChoiceKind = self->_partyChoiceIndex = 0;
                    self->_partyTouchRevision = self->_partyTouchReads = self->_partyTouchX = self->_partyTouchY = 0;
                    self->_bagChoicePending = self->_bagTouchActive = NO;
                    self->_bagChoiceRevision = self->_bagChoicePhase = self->_bagChoiceKind = self->_bagChoiceIndex = 0;
                    self->_bagTouchRevision = self->_bagTouchReads = self->_bagTouchX = self->_bagTouchY = 0;
                    [self clearCaptureInputLocked];
                    [self clearEvolutionInputLocked];
                    [self clearPokedexInputLocked];
                    self->_evolutionUIState=[NSData dataWithBytes:&evolution length:sizeof(evolution)]; self->_evolutionPictures=nil;
                    self->_optionsUIState=self->_displayedOptionsUIState=optionsUI; self->_optionsPreviewImage=nil; self->_optionsScene=NO;
                    [self clearOptionsInputLocked]; self->_optionsOwnerGeneration=0;
                    self->_namingUIState=self->_displayedNamingUIState=namingUI; self->_namingScene=NO;
                    [self clearNamingInputLocked]; self->_namingOwnerGeneration=0;
                    self->_pcBoxUIState=self->_displayedPCBoxUIState=pcBoxUI; self->_pcBoxPictures=self->_pcBoxOwnerPictures=nil;
                    [self clearPCBoxInputLocked]; self->_pcBoxOwnerGeneration=0; self->_pcBoxScene=NO;
                    [self clearTrainerCardInputLocked];self->_trainerCardRawDown=self->_trainerCardReleasePending=self->_trainerCardTouchCooling=NO;
                    [self clearStartMenuInputLocked];self->_startMenuInputCooling=self->_startMenuReleasePending=NO;
                    self->_startMenuOwnedInputs=NO;
                    self->_startMenuUIState=self->_startMenuRawUIState=self->_displayedStartMenuUIState=nil;
                    self->_startMenuMainFrame=self->_startMenuSubFrame=nil;self->_startMenuFrameEngine=self->_startMenuRawEngine=self->_displayedStartMenuEngine=1;
                    self->_trainerCardOwnerGeneration=0;self->_trainerCardUIState=self->_trainerCardRawUIState=self->_displayedTrainerCardUIState=nil;self->_trainerCardPictures=nil;
                    [self clearFieldChoiceInputLocked]; self->_fieldChoiceOwnerGeneration=self->_fieldChoiceFrameGeneration=0; self->_fieldChoiceScene=NO;
                    self->_fieldChoiceUIState=self->_displayedFieldChoiceUIState=fieldChoiceUI;
                    self->_fieldChoiceOwnerFrame=nil; self->_fieldChoicePreviewImage=self->_fieldChoiceOwnerPreview=nil;
#ifdef HEARTGOLD_ORIGINAL_PC_BOX_CHECK
                    [self clearPCBoxCheckTransientLocked:YES];
                    self->_pcBoxQAState=self->_displayedPCBoxQAState=nil;
                    self->_pcBoxQAPadReads=self->_displayedPCBoxQAPadReads=self->_pcBoxQAOwnerFrame=self->_displayedPCBoxQAOwnerFrame=0;
                    self->_pcBoxQAPublishedFrame=self->_pcBoxQADisplayedPublishedFrame=0;
                    self->_pcBoxQARequestSerial=self->_pcBoxQARequestCompleted=0;
#endif
                    self->_captureUIState=captureUI;
                    self->_pokedexUIState=[NSData dataWithBytes:&pokedex length:sizeof(pokedex)]; self->_pokedexPictures=self->_pokedexOwnerPictures=self->_pokedexDisplayPictures=nil;
                    self->_summaryChoicePending = self->_summaryInputActive = self->_summaryTouchPressed = NO;
                    self->_summaryChoiceRevision = self->_summaryChoicePhase = self->_summaryChoiceKind = self->_summaryChoiceIndex = 0;
                    self->_summaryInputRevision = self->_summaryInputPhase = self->_summaryInputReads = self->_summaryInputKeys = self->_summaryTouchX = self->_summaryTouchY = 0;
                    self->_battleMenuChoicePending=self->_battleMenuTouchActive=self->_battleMenuTouchCooling=NO;
                    self->_battleMenuChoiceKind=self->_battleMenuChoiceRevision=self->_battleMenuChoicePhase=self->_battleMenuChoiceIndex=0;
                    self->_battleMenuTouchKind=self->_battleMenuTouchRevision=self->_battleMenuTouchReads=self->_battleMenuTouchX=self->_battleMenuTouchY=0;
                    self->_fieldMapId = self->_selectedEngine = 0; self->_touchEngine = 1;
                    self->_fieldMenuOpen = self->_automaticMenuEngine = NO; self->_fieldMenuIcons = 0x0DDDDDDD;
                    self->_frame = self->_fieldMenuFrame = nil; self->_frameWidth = self->_frameEngine = self->_displayedEngine = 0;
                    self->_battleUIState = [NSData dataWithBytes:&battle length:sizeof(battle)];
                    self->_partyUIState = [NSData dataWithBytes:&party length:sizeof(party)];
                    self->_partyPictures = self->_partyOwnerPictures = nil;
                    self->_bagUIState = [NSData dataWithBytes:&bag length:sizeof(bag)];
                    self->_bagPictures = self->_bagOwnerPictures = nil;
                    self->_summaryUIState = [NSData dataWithBytes:&summary length:sizeof(summary)];
                    self->_summaryPictures = self->_summaryOwnerPictures = nil;
                    self->_battleBagUIState=[NSData dataWithBytes:&battleBag length:sizeof(battleBag)];
                    self->_battlePartyUIState=[NSData dataWithBytes:&battleParty length:sizeof(battleParty)];
                    self->_battleMenuPictures=self->_battleMenuOwnerPictures=nil;
                    self->_battleMenuOwnerKind=self->_battleMenuOwnerRevision=self->_battleMenuOwnerPhase=0;
                    self->_joystickNeedsReset = YES;
#ifdef HEARTGOLD_ORIGINAL_APP_CHECK
                    self->_geometryState = nil; self->_checkResetPending = NO;
                    self->_battleMenusQAState=nil; self->_checkBattleMenusRequestPending=self->_checkBattleMenusRequestAccepted=NO; self->_checkBattleMenusRequestRevision=0;
#ifdef HEARTGOLD_ORIGINAL_DEFAULT_CHECK
                    self->_displayedBattleMenusQAState=nil; self->_paletteFadeFinished=self->_displayedPaletteFadeFinished=NO;
#endif
#ifdef HEARTGOLD_ORIGINAL_CAPTURE_REGISTRATION_CHECK
                    self->_captureQAState=self->_displayedCaptureQAState=nil;
                    self->_captureQARequestPending=self->_captureQARequestInFlight=self->_captureQARequestAccepted=NO;
                    self->_captureQARequestRevision=self->_captureQARequestVariant=self->_captureQABootStep=self->_captureQABootFrame=0;
                    self->_captureQAPadReads=self->_displayedCaptureQAPadReads=self->_captureQAOwnerRequests=0;
                    self->_captureQAInputPresses=self->_captureQAInputReleases=self->_captureQALastReleaseReads=self->_captureQAMaxHeldReads=0;
#ifdef HEARTGOLD_ORIGINAL_NAMING_CHECK
                    self->_namingQAInputPresses=self->_namingQAInputReleases=self->_namingQALastReleaseReads=self->_namingQAMaxHeldReads=0;
                    self->_namingQAInputs=nil; self->_namingQAOverflow=NO;
#endif
#endif
#ifdef HEARTGOLD_ORIGINAL_SUMMARY_TM_CHECK
                    [self clearSummaryTMTransientLocked];
                    self->_summaryTMQAState=self->_summaryTMBagBaseline=self->_summaryTMFlagsBaseline=nil;
                    self->_summaryTMFacts=nil; self->_summaryTMHMWarningImage=nil; self->_summaryTMFixtureError=nil;
                    self->_summaryTMObserverSeen=self->_summaryTMFixturePreparing=self->_summaryTMFixturePrepared=self->_summaryTMRequestAccepted=NO;
                    self->_summaryTMRequestRevision=self->_summaryTMPadReads=self->_summaryTMBootStep=self->_summaryTMBootFrame=self->_summaryTMBootInputStep=self->_summaryTMBootInputFrame=0;
                    self->_summaryTMRawReads=self->_summaryTMRawPresses=self->_summaryTMRawReleases=self->_summaryTMRawRejected=0;
                    self->_summaryTMMon=self->_summaryTMBag=self->_summaryTMFlags=self->_summaryTMProfile=self->_summaryTMDex=self->_summaryTMTmOffset=0;
                    self->_summaryTMMoney=self->_summaryTMDexEnabled=self->_summaryTMDexSeen=self->_summaryTMDexOwned=self->_summaryTMNameLength=self->_summaryTMMapSection=self->_summaryTMOriginalPersonality=self->_summaryTMOriginalOtId=0;
                    memset(self->_summaryTMMonBaseline,0,sizeof(self->_summaryTMMonBaseline)); memset(&self->_summaryTMRawExpected,0,sizeof(self->_summaryTMRawExpected));
#endif
                    self->_restartClean = !self->_keys && !self->_pendingKeys && !self->_submittedKeys &&
                        !self->_joystickKeys && !self->_pendingJoystickKeys && !self->_submittedJoystickKeys &&
                        !self->_touchPressed && !self->_battleChoicePending && !self->_partyChoicePending && !self->_partyTouchActive && !self->_partyOwnerPictures &&
                        !self->_bagChoicePending && !self->_bagTouchActive && !self->_bagOwnerPictures &&
                        !self->_summaryChoicePending && !self->_summaryInputActive && !self->_summaryTouchPressed && !self->_summaryInputKeys && !self->_summaryOwnerPictures && !self->_fieldMapId &&
                        !self->_evolutionChoicePending && !self->_evolutionInputActive && !self->_evolutionInputCooling && !self->_evolutionInputKeys && !self->_evolutionTouchX && !self->_evolutionTouchY && !self->_evolutionPictures &&
                        !self->_captureScene && !self->_captureChoicePending && !self->_captureInputActive && !self->_captureInputCooling &&
                        !self->_captureChoiceGeneration && !self->_captureChoiceRevision && !self->_captureInputGeneration && !self->_captureInputRevision && !self->_captureInputReads && !self->_captureReleaseReads &&
                        [self->_captureUIState isEqualToData:captureUI] && [self->_displayedCaptureUIState isEqualToData:captureUI] &&
                        !self->_optionsScene && !self->_optionsChoicePending && !self->_optionsInputActive && !self->_optionsInputCooling && !self->_optionsOwnerGeneration &&
                        !self->_optionsChoiceKeys && !self->_optionsInputKeys && !self->_optionsInputReads && !self->_optionsReleaseReads && !self->_optionsPreviewImage &&
                        !self->_optionsChoice.active && !self->_optionsChoice.ownerGeneration && !self->_optionsChoice.revision && !self->_optionsChoice.ready &&
                        !self->_optionsInput.active && !self->_optionsInput.ownerGeneration && !self->_optionsInput.revision && !self->_optionsInput.ready &&
                        [self->_optionsUIState isEqualToData:optionsUI] && [self->_displayedOptionsUIState isEqualToData:optionsUI] &&
                        !self->_namingScene && !self->_namingChoicePending && !self->_namingChoiceTouch && !self->_namingInputActive && !self->_namingInputCooling && !self->_namingInputTouch &&
                        !self->_namingOwnerGeneration && !self->_namingChoiceKeys && !self->_namingChoiceX && !self->_namingChoiceY &&
                        !self->_namingInputKeys && !self->_namingInputX && !self->_namingInputY && !self->_namingInputReads && !self->_namingReleaseReads &&
                        !self->_namingChoice.active && !self->_namingChoice.ownerGeneration && !self->_namingChoice.revision && !self->_namingChoice.ready &&
                        !self->_namingInput.active && !self->_namingInput.ownerGeneration && !self->_namingInput.revision && !self->_namingInput.ready &&
                        [self->_namingUIState isEqualToData:namingUI] && [self->_displayedNamingUIState isEqualToData:namingUI] &&
                        !self->_pcBoxScene && !self->_pcBoxChoicePending && !self->_pcBoxInputActive && !self->_pcBoxInputCooling && !self->_pcBoxOwnerGeneration &&
                        !self->_pcBoxChoiceKeys && !self->_pcBoxInputKeys && !self->_pcBoxInputReads && !self->_pcBoxReleaseReads && !self->_pcBoxPictures && !self->_pcBoxOwnerPictures &&
                        !self->_pcBoxChoice.active && !self->_pcBoxInput.active && [self->_pcBoxUIState isEqualToData:pcBoxUI] && [self->_displayedPCBoxUIState isEqualToData:pcBoxUI] &&
                        !self->_fieldChoiceScene && !self->_fieldChoiceOwnerGeneration && !self->_fieldChoiceFrameGeneration && !self->_fieldChoiceChoicePending && !self->_fieldChoiceInputActive && !self->_fieldChoiceInputCooling &&
                        !self->_fieldChoiceChoiceKeys && !self->_fieldChoiceInputKeys && !self->_fieldChoiceInputReads && !self->_fieldChoiceReleaseReads && !self->_fieldChoiceOwnerFrame && !self->_fieldChoicePreviewImage && !self->_fieldChoiceOwnerPreview &&
                        !self->_fieldChoiceChoice.active && !self->_fieldChoiceInput.active && [self->_fieldChoiceUIState isEqualToData:fieldChoiceUI] && [self->_displayedFieldChoiceUIState isEqualToData:fieldChoiceUI] &&
                        !self->_pokedexChoicePending && !self->_pokedexInputActive && !self->_pokedexInputCooling && !self->_pokedexGestureLive && !self->_pokedexGestureAdmitted && !self->_pokedexPictures && !self->_pokedexOwnerPictures &&
                        !self->_pokedexGestureContinued && !self->_pokedexInput.keys && !self->_pokedexInput.touchPressed && !self->_pokedexInput.touchX && !self->_pokedexInput.touchY && !self->_pokedexGestureTrack.kind && !self->_pokedexDisplayPictures &&
                        !self->_battleMenuChoicePending && !self->_battleMenuTouchActive && !self->_battleMenuTouchCooling && !self->_battleMenuOwnerPictures && !self->_checkBattleMenusRequestPending &&
                        !self->_fieldMenuOpen && !self->_frameBuffer && !self->_displayControl && !self->_displayStatus && !self->_frameEngine && !self->_displayedEngine;
#ifdef HEARTGOLD_ORIGINAL_DEFAULT_CHECK
                    self->_restartClean&=!self->_battleMenusQAState && !self->_displayedBattleMenusQAState && !self->_paletteFadeFinished && !self->_displayedPaletteFadeFinished;
#endif
#ifdef HEARTGOLD_ORIGINAL_CAPTURE_REGISTRATION_CHECK
                    self->_restartClean&=!self->_captureQAState && !self->_displayedCaptureQAState
                        && !self->_captureQARequestPending && !self->_captureQARequestInFlight && !self->_captureQARequestAccepted
                        && !self->_captureQARequestRevision && !self->_captureQARequestVariant && !self->_captureQABootStep && !self->_captureQABootFrame
                        && !self->_captureQAPadReads && !self->_displayedCaptureQAPadReads && !self->_captureQAOwnerRequests
                        && !self->_captureQAInputPresses && !self->_captureQAInputReleases && !self->_captureQALastReleaseReads && !self->_captureQAMaxHeldReads;
#ifdef HEARTGOLD_ORIGINAL_NAMING_CHECK
                    self->_restartClean&=!self->_namingQAInputPresses && !self->_namingQAInputReleases && !self->_namingQALastReleaseReads
                        && !self->_namingQAMaxHeldReads && !self->_namingQAInputs && !self->_namingQAOverflow;
#endif
#endif
#ifdef HEARTGOLD_ORIGINAL_SUMMARY_TM_CHECK
                    self->_restartClean&=!self->_summaryTMRequestPending && !self->_summaryTMRawPending && !self->_summaryTMRawActive && !self->_summaryTMRawCooling
                        && !self->_summaryTMQAState && !self->_summaryTMFacts && !self->_summaryTMHMWarningImage && !self->_summaryTMBagBaseline && !self->_summaryTMFlagsBaseline;
#endif
#endif
                    [self->_condition unlock];
                    for (OriginalKeyButton *button in self->_buttons.allValues) button.inputGeneration++;
                    self->_canvas.image = self->_fieldMenu.image = nil; self->_displayedWidth = 0;
                    self->_fieldMenu.hidden = self->_fieldMenuShade.hidden = YES;
                    self->_canvas.userInteractionEnabled = YES; self->_displayedBattleUIState = nil;
                    [self updateStartMenuControls:nil main:nil sub:nil engine:1 blocked:YES paused:NO];
                    [self updateBattleControls:self->_battleUIState]; [self resetJoystick];
                    [self updatePartyControls:self->_partyUIState pictures:nil paused:NO];
                    [self updateBagControls:self->_bagUIState pictures:nil paused:NO];
                    [self updateSummaryControls:self->_summaryUIState pictures:nil paused:NO];
                    [self updateEvolutionControls:self->_evolutionUIState pictures:nil summary:NO paused:NO];
                    [self updatePokedexControls:self->_pokedexUIState pictures:nil blocked:NO paused:NO];
                    [self updateBattleMenuControls:self->_battleBagUIState party:self->_battlePartyUIState pictures:nil paused:NO engine:0];
                    self->_status.text = @"正在启动原版引擎…"; self->_status.hidden = NO;
                    [self.view setNeedsLayout];
                });
            }
        }
        HGNativeBackupSetPath(NULL);
        [_condition lock];
        [self clearStartMenuInputLocked];
#ifdef HEARTGOLD_ORIGINAL_PC_BOX_CHECK
        [self clearPCBoxCheckTransientLocked:NO]; _pcBoxQAState=nil;
#endif
        [self clearTrainerCardInputLocked]; [self clearFieldChoiceInputLocked]; [self clearPCBoxInputLocked]; [self clearOptionsInputLocked]; [self clearNamingInputLocked]; [self clearCaptureInputLocked];
#ifdef HEARTGOLD_ORIGINAL_APP_CHECK
        _audioChanging=NO; _finished = YES;
#endif
        if (!_failure) _failure = @"游戏已停止";
        [_condition unlock];
    }
}
@end

#include "pokedex_ui.inc"

#ifdef HEARTGOLD_ORIGINAL_APP_CHECK
#ifdef HEARTGOLD_ORIGINAL_PC_BOX_CHECK
#include "../tests/pc_box_uikit_check.inc"
#else
#ifdef HEARTGOLD_ORIGINAL_DEFAULT_CHECK
#include "../tests/pc_box_check.inc"
#include "../tests/trainer_card_uikit_check.inc"
#include "../tests/start_menu_uikit_check.inc"
#endif
#include "../tests/check_original_app.inc"
#ifdef HEARTGOLD_ORIGINAL_EVOLUTION_CHECK
#include "../tests/evolution_check.inc"
#endif
#ifdef HEARTGOLD_ORIGINAL_EVOLUTION_LEARN_CHECK
#include "../tests/evolution_learning_check.inc"
#endif
#ifdef HEARTGOLD_ORIGINAL_POKEDEX_CHECK
#include "../tests/pokedex_check.inc"
#endif
#endif
#endif

@interface OriginalAppDelegate : UIResponder <UIApplicationDelegate>
@end
@interface OriginalSceneDelegate : UIResponder <UIWindowSceneDelegate>
@property(nonatomic, strong) UIWindow *window;
@end
@implementation OriginalAppDelegate
- (UISceneConfiguration *)application:(UIApplication *)application configurationForConnectingSceneSession:(UISceneSession *)session options:(UISceneConnectionOptions *)options {
    UISceneConfiguration *configuration = [[UISceneConfiguration alloc] initWithName:@"Original Game" sessionRole:session.role];
    configuration.sceneClass = UIWindowScene.class;
    configuration.delegateClass = OriginalSceneDelegate.class;
    return configuration;
}
@end
@implementation OriginalSceneDelegate
- (void)scene:(UIScene *)scene willConnectToSession:(UISceneSession *)session options:(UISceneConnectionOptions *)options {
    if (![scene isKindOfClass:UIWindowScene.class]) return;
    self.window = [[UIWindow alloc] initWithWindowScene:(UIWindowScene *)scene];
    self.window.rootViewController = [OriginalController new];
    [self.window makeKeyAndVisible];
}
- (void)sceneDidDisconnect:(UIScene *)scene { [(OriginalController *)self.window.rootViewController stop]; }
@end
int main(int argc, char **argv) { @autoreleasepool { return UIApplicationMain(argc, argv, nil, NSStringFromClass(OriginalAppDelegate.class)); } }
