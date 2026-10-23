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

enum { OriginalBattleMenuBag=1, OriginalBattleMenuParty=2 };
static NSString *OriginalPartyStatusName(uint32_t status) {
    static NSString *const names[]={@"",@"麻痹",@"冰冻",@"睡眠",@"中毒",@"灼伤",@"濒死",@""};
    return status<sizeof(names)/sizeof(*names) ? names[status] : @"";
}
_Static_assert(sizeof(HGNativeBattleBagUIState)==536, "Battle Bag UI ABI");
_Static_assert(sizeof(HGNativeBattlePartyUIState)==684, "Battle Party UI ABI");
_Static_assert(offsetof(HGNativeBattlePartyUIState,selectedMove)==528 && offsetof(HGNativeBattlePartyUIState,moves)==532
    && offsetof(HGNativeBattlePartyUIState,prompt)==580 && offsetof(HGNativeBattlePartyUIState,message)==632, "Battle Party info ABI");
static uint32_t OriginalBattleMenuKind(const HGNativeBattleBagUIState *bag, const HGNativeBattlePartyUIState *party) {
    if (bag && bag->active && ((bag->phase==HG_BATTLE_BAG_UI_POCKETS && bag->page==0)
        || (bag->phase==HG_BATTLE_BAG_UI_ITEMS && bag->page==1)
        || ((bag->phase==HG_BATTLE_BAG_UI_USE || bag->phase==HG_BATTLE_BAG_UI_ACK) && bag->page==2))) return OriginalBattleMenuBag;
    if (party && party->active && party->partyCount<=HG_BATTLE_PARTY_UI_MONS
        && (party->mode==HG_BATTLE_PARTY_UI_SWITCH || (party->mode==HG_BATTLE_PARTY_UI_USE_ITEM && party->itemId==HG_BATTLE_PARTY_UI_POTION))
        && ((party->phase==HG_BATTLE_PARTY_UI_ROSTER && party->page==0)
            || (party->phase==HG_BATTLE_PARTY_UI_MON_MENU && party->page==1 && party->mode==HG_BATTLE_PARTY_UI_SWITCH)
            || (party->mode==HG_BATTLE_PARTY_UI_SWITCH && party->selectedSlot<HG_BATTLE_PARTY_UI_MONS && party->selectedSlot<party->partyCount
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
#include "../tests/battle_menus_qa.h"
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

@interface OriginalJoystick : UIControl
@property(nonatomic,readonly) CGPoint location;
@property(nonatomic,readonly) BOOL pressed;
@property(nonatomic,readonly) BOOL cancelled;
@end
@implementation OriginalJoystick
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
    UIImageView *_partySelectedName, *_partySelectedItem, *_partyMessage;
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
    w2c_hgengine *_game;
    struct w2c_env *_env;
    struct w2c_wasi__snapshot__preview1 *_wasi;
    const vu16 *_displayStatus, *_displayControl;
    uint32_t _frameBuffer, _lastFrame, _preferredViewWidth, _preferredBattleWidth, _frameWidth, _displayedWidth, _fieldMapId;
#ifdef HEARTGOLD_ORIGINAL_APP_CHECK
    uint32_t _publishedFrames, _bootCount, _bootFrameBase, _checkedBootParameter;
    BOOL _suspended, _finished, _checkResetPending, _checkedResetOccurred, _resetQuiesced, _restartClean, _audioChanging;
    NSData *_geometryState;
    NSData *_audioProducerTrace;
    uint64_t _audioProducerCapturedAtNs;
    NSData *_audioFirstShortTrace;
    uint64_t _audioFirstShortTimeNs, _audioFirstShortCapturedAtNs;
    NSMutableArray *_checkInputCompletions;
    NSData *_battleMenusQAState;
    BOOL _checkBattleMenusRequestPending, _checkBattleMenusRequestAccepted;
    uint32_t _checkBattleMenusRequestRevision;
#endif
}
- (void)engineFrame;
- (void)stop;
- (void)tapAt:(CGPoint)point;
@end
#ifdef HEARTGOLD_ORIGINAL_APP_CHECK
@interface OriginalController (Checks)
- (void)checkControls;
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
    self.view.backgroundColor = [UIColor colorWithRed:0.035 green:0.06 blue:0.07 alpha:1];
    _condition = [NSCondition new]; _buttons = [NSMutableDictionary new]; _audio = [OriginalAudio new];
    _touchEngine=1;
    _canvas = [UIImageView new]; _canvas.contentMode = UIViewContentModeScaleAspectFit;
    _canvas.accessibilityLabel = @"原版游戏画面";
    _canvas.layer.magnificationFilter = kCAFilterNearest;
    _canvas.userInteractionEnabled = YES; [self.view addSubview:_canvas];
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
    [self checkControls];
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
    _fieldMenu.frame=CGRectMake(CGRectGetMaxX(area)-menuWidth-12,area.origin.y+56,menuWidth,menuWidth*0.75);
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
        CGFloat ballSpace = selection ? 0 : 96;
        CGFloat gap = 10, buttonWidth = (_battleBar.bounds.size.width - inset - 12 - gap - ballSpace) / 2;
        CGFloat buttonHeight = (barHeight - 24 - gap) / 2;
        for (NSUInteger i = 0; i < 4; i++) {
            _battleButtons[i].frame = CGRectMake(inset + (i % 2) * (buttonWidth + gap), 12 + (i / 2) * (buttonHeight + gap), buttonWidth, buttonHeight);
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
        _partyMessage.frame=CGRectMake(rx,ph-36,rw,32);
        _partyActionScroll.frame=CGRectMake(rx,72,rw,MAX(44,ph-116));
        const HGNativePartyUIState *state=_displayedPartyUIState.bytes;
        NSUInteger count=_displayedPartyUIState.length==sizeof(*state) ? MIN(HG_PARTY_UI_ACTIONS,state->actionCount) : 0;
        for (NSUInteger i=0;i<HG_PARTY_UI_ACTIONS;i++) _partyActions[i].frame=CGRectMake((i%2)*(rw+gap)/2,(i/2)*52,(rw-gap)/2,44);
        _partyActionScroll.contentSize=CGSizeMake(rw,((count+1)/2)*52);
        _partyBack.frame=CGRectMake(area.origin.x,CGRectGetMaxY(area)-52,120,44);
        _buttons[@2].frame=_partyBack.frame;
        _buttons[@1].frame=CGRectMake(CGRectGetMaxX(area)-120,CGRectGetMaxY(area)-52,120,44);
        _canvas.frame=CGRectMake(area.origin.x,area.origin.y,area.size.width,MAX(1,h-60));
        _status.frame=CGRectInset(_canvas.frame,10,10);
    }
    if (_bagScene && !_bagPanel.hidden) {
        _bagPanel.frame=CGRectMake(area.origin.x,area.origin.y,area.size.width,MAX(1,h-60));
        CGFloat pw=_bagPanel.bounds.size.width,ph=_bagPanel.bounds.size.height,gap=8,leftWidth=MIN(280,pw*0.38),rx=leftWidth+gap,rw=pw-rx-gap;
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
        } else if (_battleMenuKind==OriginalBattleMenuParty && phase>=HG_BATTLE_PARTY_UI_SUMMARY && phase<=HG_BATTLE_PARTY_UI_MOVE_DETAIL) {
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
    [_menuButton setTitle:engine ? @"返回场景" : @"触摸菜单" forState:UIControlStateNormal];
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
    BOOL info=kind==OriginalBattleMenuParty && phase>=HG_BATTLE_PARTY_UI_SUMMARY && phase<=HG_BATTLE_PARTY_UI_MOVE_DETAIL;
    BOOL ready=kind==OriginalBattleMenuBag ? bag->ready : party->ready;
    BOOL keep=_battleMenuOwnerPictures && _battleMenuOwnerKind==kind && _battleMenuOwnerRevision==revision && _battleMenuOwnerPhase==phase;
    NSMutableDictionary *pictures=keep ? [_battleMenuOwnerPictures mutableCopy] : [NSMutableDictionary new];
    if (info) {
        [pictures removeObjectForKey:@"preview"];
        if (phase==HG_BATTLE_PARTY_UI_CHECK_MOVES) {
            UIImage *name=OriginalFrameRegion(subFrame,40,8,72,16); if (!name) return nil;
            pictures[@"selected"]=name;
        } else {
            UIImage *content=OriginalFrameRegion(subFrame,0,0,256,152); if (!content) return nil;
            pictures[@"content"]=content;
        }
    } else {
        UIImage *preview=OriginalPixelImage(frame,(uint32_t)(frame.length/(192*4)),192);
        if (!preview) return nil;
        pictures[@"preview"]=preview;
    }
    NSArray *old=pictures[@"buttons"]; NSMutableArray *images=[NSMutableArray new];
    uint32_t count=kind==OriginalBattleMenuBag ? MIN(bag->buttonCount,HG_BATTLE_BAG_UI_BUTTONS) : MIN(party->buttonCount,HG_BATTLE_PARTY_UI_BUTTONS);
    for (NSUInteger i=0;i<HG_BATTLE_BAG_UI_BUTTONS;i++) {
        UIImage *image=nil;
        if (i<count && kind==OriginalBattleMenuBag) {
            const HGNativeBattleBagUIButton *button=&bag->buttons[i];
            image=OriginalFrameRegion(subFrame,button->cropX,button->cropY,button->cropWidth,button->cropHeight);
        } else if (i<count) {
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
    BOOL info=kind==OriginalBattleMenuParty && phase>=HG_BATTLE_PARTY_UI_SUMMARY && phase<=HG_BATTLE_PARTY_UI_MOVE_DETAIL;
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
                label=mon->isEgg ? [name stringByAppendingString:@"，蛋"] : [NSString stringWithFormat:@"%@，Lv.%u，HP %u／%u%@%@",name,mon->level,mon->hp,mon->maxHp,status.length ? [@"，" stringByAppendingString:status] : @"",mon->heldItem ? @"，携带道具" : @""];
                highlight=i==party->selectedSlot;
            } else if (phase==HG_BATTLE_PARTY_UI_ROSTER && i==6) label=@"返回";
            else if (phase==HG_BATTLE_PARTY_UI_MON_MENU && i<monActions.count) label=monActions[i];
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
    _battleMenuTitle.text=kind==OriginalBattleMenuBag ? @"战斗背包" : kind==OriginalBattleMenuParty ? (party->mode==HG_BATTLE_PARTY_UI_USE_ITEM ? @"使用道具" : @"宝可梦") : nil;
    _battleMenuPreview.image=pictures[@"preview"]; _battleMenuSelected.image=pictures[@"selected"]; _battleMenuQuantity.image=pictures[@"quantity"];
    _battleMenuDescription.image=info ? pictures[@"content"] : pictures[@"description"] ?: pictures[@"prompt"]; _battleMenuMessage.image=pictures[@"message"];
    _battleMenuTitle.hidden=_battleMenuPreview.hidden=_battleMenuQuantity.hidden=info;
    _battleMenuSelected.hidden=info && phase!=HG_BATTLE_PARTY_UI_CHECK_MOVES;
    _battleMenuDescription.hidden=info && phase==HG_BATTLE_PARTY_UI_CHECK_MOVES;
    _battleMenuDescription.isAccessibilityElement=info && phase!=HG_BATTLE_PARTY_UI_CHECK_MOVES;
    _battleMenuSelected.isAccessibilityElement=info && phase==HG_BATTLE_PARTY_UI_CHECK_MOVES;
    NSString *selectedName=info && party->mons[party->selectedSlot].species<_speciesNames.count ? _speciesNames[party->mons[party->selectedSlot].species] : @"宝可梦";
    _battleMenuSelected.accessibilityLabel=selectedName;
    _battleMenuDescription.accessibilityLabel=phase==HG_BATTLE_PARTY_UI_MOVE_DETAIL ? [NSString stringWithFormat:@"%@，%@",selectedName,_battleMenuButtons[party->selectedMove].accessibilityLabel] : [selectedName stringByAppendingString:@"，属性"];
    _battleMenuMessage.hidden=!(kind==OriginalBattleMenuParty && phase==HG_BATTLE_PARTY_UI_MESSAGE);
}
- (NSDictionary *)copyPartyPictures:(const HGNativePartyUIState *)state frame:(NSData *)frame {
    if (!state->active || state->context || state->panelLayout) { _partyOwnerPictures=nil; return nil; }
    NSMutableDictionary *pictures=_partyOwnerPictures ? [_partyOwnerPictures mutableCopy] : [NSMutableDictionary new];
    for (NSString *key in @[@"cards",@"actions"]) {
        NSUInteger count=[key isEqual:@"actions"] ? HG_PARTY_UI_ACTIONS : HG_PARTY_UI_SLOTS;
        NSArray *old=pictures[key]; NSMutableArray *images=[NSMutableArray new];
        for (NSUInteger i=0;i<count;i++) {
            UIImage *image=nil;
            if ([key isEqual:@"actions"]) image=OriginalWindowImage(&state->actions[i].text,_env->memory->data,_env->memory->size);
            else if (state->phase==HG_PARTY_UI_ROSTER && state->ready && state->mons[i].active) image=OriginalPartyCard(frame,&state->mons[i]);
            [images addObject:image ?: (i<old.count ? old[i] : NSNull.null)];
        }
        pictures[key]=[images copy];
    }
    const HGNativePartyUIWindow *windows[]={&state->selectedName,&state->selectedItem,&state->message};
    NSArray *keys=@[@"selected",@"item",@"message"];
    for (NSUInteger i=0;i<3;i++) {
        UIImage *image=OriginalWindowImage(windows[i],_env->memory->data,_env->memory->size);
        if (image) pictures[keys[i]]=image;
    }
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
- (void)takePartyTouchLocked:(const HGNativePartyUIState *)state reads:(uint32_t)reads x:(uint32_t *)x y:(uint32_t *)y pressed:(BOOL *)pressed {
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
    BOOL scene=data.length==sizeof(*state) && state->active && !state->context && !state->panelLayout;
    BOOL panel=scene && (state->phase==HG_PARTY_UI_ROSTER || state->phase==HG_PARTY_UI_CONTEXT || state->phase==HG_PARTY_UI_SUBCONTEXT);
    BOOL changed=_partyScene!=scene || !_displayedPartyUIState || data.length!=_displayedPartyUIState.length
        || (scene && memcmp(data.bytes,_displayedPartyUIState.bytes,offsetof(HGNativePartyUIState,mons)));
    if (scene && !_partyScene) {
        [_condition lock]; _keys=_pendingKeys=_submittedKeys=0; _touchPressed=NO; [_condition unlock]; [self resetJoystick];
    }
    _partyScene=scene; _displayedPartyUIState=data;
    _partyPanel.hidden=!panel; _partyBack.hidden=!panel;
    if (changed) [self.view setNeedsLayout];
    if (!panel) return;
    [_condition lock]; BOOL waiting=_partyChoicePending || _partyTouchActive; [_condition unlock];
    BOOL ready=state->ready && !paused && !waiting;
    _partyBack.enabled=ready && state->cancelEnabled;
    NSArray *cards=pictures[@"cards"],*actions=pictures[@"actions"];
    for (NSUInteger i=0;i<HG_PARTY_UI_SLOTS;i++) {
        const HGNativePartyUIMon *mon=&state->mons[i]; UIButton *button=_partyMons[i];
        button.hidden=!(state->slotMask&(1u<<i)) || !mon->active;
        button.enabled=ready && state->phase==HG_PARTY_UI_ROSTER;
        button.layer.borderColor=(i==state->selected ? [UIColor colorWithRed:0.91 green:0.49 blue:0.12 alpha:1] : [UIColor colorWithRed:0.45 green:0.62 blue:0.72 alpha:1]).CGColor;
        _partyCards[i].image=i<cards.count && [cards[i] isKindOfClass:UIImage.class] ? cards[i] : nil;
        NSString *name=mon->species<_speciesNames.count ? _speciesNames[mon->species] : @"宝可梦";
        NSString *stats=mon->isEgg ? @"蛋" : [NSString stringWithFormat:@"Lv.%u  %u/%u",mon->level,mon->hp,mon->maxHp];
        NSString *status=OriginalPartyStatusName(mon->status);
        button.accessibilityLabel=[NSString stringWithFormat:@"%@，%@，%@%@%@",name,stats,status,mon->heldItem ? @"，携带道具" : @"",i==state->selected ? @"，已选择" : @""];
        button.accessibilityHint=state->phase==HG_PARTY_UI_ROSTER ? @"打开原版宝可梦操作菜单" : nil;
        button.accessibilityTraits=UIAccessibilityTraitButton | (i==state->selected ? UIAccessibilityTraitSelected : 0);
    }
    for (NSUInteger i=0;i<HG_PARTY_UI_ACTIONS;i++) {
        UIButton *button=_partyActions[i]; button.hidden=i>=state->actionCount;
        button.enabled=ready && i<state->actionCount && (state->actionEnabledMask&(1u<<i));
        uint32_t textId=state->actions[i].textId;
        NSString *name=textId<_partyActionNames.count && [_partyActionNames[textId] length] ? _partyActionNames[textId] : @"操作";
        UIButtonConfiguration *config=UIButtonConfiguration.filledButtonConfiguration;
        config.contentInsets=NSDirectionalEdgeInsetsMake(2,4,2,4);
        config.baseBackgroundColor=i==state->actionSelected ? [UIColor colorWithRed:1 green:0.81 blue:0.48 alpha:1] : UIColor.whiteColor;
        config.baseForegroundColor=_partyTitle.textColor;
        UIImage *image=i<actions.count && [actions[i] isKindOfClass:UIImage.class] ? actions[i] : nil;
        if (image) config.image=image; else config.title=name;
        button.configuration=config; button.accessibilityLabel=name;
    }
    _partySelectedName.image=pictures[@"selected"]; _partySelectedItem.image=pictures[@"item"]; _partyMessage.image=pictures[@"message"];
    _partySelectedName.hidden=state->phase==HG_PARTY_UI_ROSTER || !_partySelectedName.image;
    _partySelectedItem.hidden=state->phase==HG_PARTY_UI_ROSTER || !_partySelectedItem.image;
}
- (NSDictionary *)copyBagPictures:(const HGNativeBagUIState *)state frame:(NSData *)frame subFrame:(NSData *)subFrame {
    if (!state->active || state->mode) { _bagOwnerPictures=nil; return nil; }
    NSMutableDictionary *pictures=_bagOwnerPictures ? [_bagOwnerPictures mutableCopy] : [NSMutableDictionary new];
    pictures[@"preview"]=OriginalPixelImage(frame,256,192) ?: NSNull.null;
    for (NSString *key in @[@"pockets",@"items",@"actions"]) {
        NSUInteger count=[key isEqual:@"pockets"] ? HG_BAG_UI_POCKETS : [key isEqual:@"items"] ? HG_BAG_UI_ITEMS : HG_BAG_UI_ACTIONS;
        NSArray *old=pictures[key]; NSMutableArray *images=[NSMutableArray new];
        for (NSUInteger i=0;i<count;i++) {
            UIImage *image=nil;
            if ([key isEqual:@"actions"]) image=OriginalWindowImage(&state->actions[i].text,_env->memory->data,_env->memory->size);
            else if (state->phase==HG_BAG_UI_INPUT && state->ready) {
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
    const HGNativePartyUIWindow *windows[]={&state->prompt,&state->message,&state->page,&state->selectedEntry};
    NSArray *keys=@[@"prompt",@"message",@"page",@"selected"];
    for (NSUInteger i=0;i<4;i++) {
        UIImage *image=OriginalWindowImage(windows[i],_env->memory->data,_env->memory->size);
        if (image) pictures[keys[i]]=image;
    }
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
    BOOL scene=data.length==sizeof(*state) && state->active && !state->mode;
    BOOL panel=scene && (state->phase==HG_BAG_UI_INPUT || state->phase==HG_BAG_UI_ITEM_MENU);
    BOOL changed=_bagScene!=scene || !_displayedBagUIState || data.length!=_displayedBagUIState.length
        || (scene && memcmp(data.bytes,_displayedBagUIState.bytes,offsetof(HGNativeBagUIState,pockets)));
    if (scene && !_bagScene) { [_condition lock]; _keys=_pendingKeys=_submittedKeys=0; _touchPressed=NO; [_condition unlock]; [self resetJoystick]; }
    _bagScene=scene; _displayedBagUIState=data; _bagPanel.hidden=!panel;
    for (UIButton *button in _bagNavigation) button.hidden=!panel;
    _bagPage.hidden=!panel || state->phase!=HG_BAG_UI_INPUT;
    if (changed) [self.view setNeedsLayout];
    if (!panel) return;
    [_condition lock]; BOOL waiting=_bagChoicePending || _bagTouchActive; [_condition unlock];
    BOOL ready=state->ready && !paused && !waiting,list=state->phase==HG_BAG_UI_INPUT;
    UIColor *selected=[UIColor colorWithRed:0.91 green:0.49 blue:0.12 alpha:1],*border=[UIColor colorWithRed:0.45 green:0.62 blue:0.72 alpha:1];
    NSArray *pockets=pictures[@"pockets"],*items=pictures[@"items"],*actions=pictures[@"actions"];
    for (NSUInteger i=0;i<HG_BAG_UI_POCKETS;i++) {
        UIButton *button=_bagPockets[i]; const HGNativeBagUIPocket *pocket=&state->pockets[i];
        button.hidden=!(state->pocketMask&(1u<<i)); button.enabled=ready && list && pocket->enabled;
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
        button.hidden=list || i>=state->actionCount || !(state->actionMask&(1u<<i)); button.enabled=ready && action->enabled;
        NSString *name=action->actionId<_bagActionNames.count && [_bagActionNames[action->actionId] length] ? _bagActionNames[action->actionId] : @"操作";
        UIButtonConfiguration *config=UIButtonConfiguration.filledButtonConfiguration;
        config.baseBackgroundColor=i==state->menuCursor ? [UIColor colorWithRed:1 green:0.81 blue:0.48 alpha:1] : UIColor.whiteColor;
        config.baseForegroundColor=_partyTitle.textColor; config.contentInsets=NSDirectionalEdgeInsetsMake(4,4,4,4);
        UIImage *image=i<actions.count && [actions[i] isKindOfClass:UIImage.class] ? actions[i] : nil;
        if (image) config.image=image; else config.title=name;
        button.configuration=config; button.accessibilityLabel=name;
    }
    _bagNavigation[0].enabled=ready && state->cancelEnabled;
    _bagNavigation[1].hidden=_bagNavigation[2].hidden=!list;
    _bagNavigation[1].enabled=ready && state->previousEnabled; _bagNavigation[2].enabled=ready && state->nextEnabled;
    _bagPreview.image=[pictures[@"preview"] isKindOfClass:UIImage.class] ? pictures[@"preview"] : nil;
    _bagSelectedEntry.hidden=list; _bagSelectedEntry.image=pictures[@"selected"];
    _bagPrompt.hidden=list; _bagPrompt.image=list ? nil : (pictures[@"prompt"] ?: pictures[@"message"]); _bagPage.image=pictures[@"page"];
}
- (NSDictionary *)copySummaryPictures:(const HGNativeSummaryUIState *)state frame:(NSData *)frame subFrame:(NSData *)subFrame {
    if (!state->active || state->mode || state->monDataType!=1 || state->moveToLearn) { _summaryOwnerPictures=nil; return nil; }
    if (!state->ready || state->page>2 || (state->phase!=HG_SUMMARY_UI_INPUT && (state->phase!=HG_SUMMARY_UI_MOVE_SELECT || state->page!=1))) return _summaryOwnerPictures;
    NSMutableDictionary *pictures=[NSMutableDictionary new];
    pictures[@"preview"]=OriginalPixelImage(frame,256,192) ?: NSNull.null;
    BOOL detail=state->phase==HG_SUMMARY_UI_MOVE_SELECT;
    if (detail) pictures[@"content"]=OriginalFrameRegion(subFrame,136,0,120,160) ?: NSNull.null;
    else if (state->page!=1) pictures[@"content"]=OriginalFrameRegion(subFrame,0,0,144,160) ?: NSNull.null;
    NSMutableArray *moves=[NSMutableArray new];
    for (NSUInteger i=0;i<HG_SUMMARY_UI_MOVES;i++) {
        const HGNativeSummaryUIMove *move=&state->moves[i];
        UIImage *image=state->page==1 && (state->moveMask&(1u<<i)) ? OriginalFrameRegion(subFrame,move->cropX,move->cropY,move->cropWidth,move->cropHeight) : nil;
        [moves addObject:image ?: NSNull.null];
    }
    pictures[@"moves"]=[moves copy];
    const HGNativePartyUIWindow *windows[]={&state->nickname,&state->levelText,&state->heldItemText,&state->backText,&state->pageTitle};
    NSArray *keys=@[@"nickname",@"level",@"item",@"back",@"title"];
    for (NSUInteger i=0;i<5;i++) {
        UIImage *image=OriginalWindowImage(windows[i],_env->memory->data,_env->memory->size);
        if (image) pictures[keys[i]]=image;
    }
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
    const uint32_t kinds[]={HG_SUMMARY_UI_INPUT_BACK,HG_SUMMARY_UI_INPUT_PREVIOUS_PAGE,HG_SUMMARY_UI_INPUT_NEXT_PAGE,
        HG_SUMMARY_UI_INPUT_PREVIOUS_MON,HG_SUMMARY_UI_INPUT_NEXT_MON,HG_SUMMARY_UI_INPUT_RIBBON_DETAILS};
    if (button.tag>=0 && button.tag<6) [self queueSummaryInput:kinds[button.tag] index:0];
}
- (void)takeSummaryInputLocked:(const HGNativeSummaryUIState *)state reads:(uint32_t)reads keys:(uint32_t *)keys x:(uint32_t *)x y:(uint32_t *)y pressed:(BOOL *)pressed {
    BOOL panel=state->active && !state->mode && state->monDataType==1 && !state->moveToLearn && state->page<=2
        && (state->phase==HG_SUMMARY_UI_INPUT || (state->phase==HG_SUMMARY_UI_MOVE_SELECT && state->page==1));
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
    BOOL scene=data.length==sizeof(*state) && state->active && !state->mode && state->monDataType==1 && !state->moveToLearn;
    BOOL panel=scene && state->page<=2 && (state->phase==HG_SUMMARY_UI_INPUT || (state->phase==HG_SUMMARY_UI_MOVE_SELECT && state->page==1));
    BOOL changed=_summaryScene!=scene || !_displayedSummaryUIState || data.length!=_displayedSummaryUIState.length
        || (scene && memcmp(data.bytes,_displayedSummaryUIState.bytes,offsetof(HGNativeSummaryUIState,moves)));
    if (scene && !_summaryScene) { [_condition lock]; _keys=_pendingKeys=_submittedKeys=0; _touchPressed=NO; [_condition unlock]; [self resetJoystick]; }
    _summaryScene=scene; _summaryMovePanel=panel && state->page==1; _summaryMoveDetail=panel && state->phase==HG_SUMMARY_UI_MOVE_SELECT;
    _displayedSummaryUIState=data; _summaryPanel.hidden=!panel;
    for (UIButton *button in _summaryNavigation) button.hidden=!panel;
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
    _summaryPreview.accessibilityLabel=state->isEgg ? [NSString stringWithFormat:@"%@，蛋",species]
        : [NSString stringWithFormat:@"%@，Lv.%u，HP %u/%u，%@%@",species,state->level,state->hp,state->maxHp,status,state->heldItem ? @"，携带道具" : @""];
    _summaryContent.isAccessibilityElement=YES;
    _summaryContent.accessibilityLabel=_summaryMoveDetail ? @"原版招式详情" : state->page==2 ? [NSString stringWithFormat:@"原版缎带页面，%u 个缎带",state->ribbonCount] : @"原版宝可梦资料";
    NSArray *moves=pictures[@"moves"];
    for (NSUInteger i=0;i<HG_SUMMARY_UI_MOVES;i++) {
        const HGNativeSummaryUIMove *move=&state->moves[i]; UIButton *button=_summaryMoves[i]; HGNativeSummaryUIInput input;
        button.hidden=!_summaryMovePanel || !(state->moveMask&(1u<<i));
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
        button.enabled=ready && HGNativeSummaryUIResolveInput(state,state->revision,state->phase,kinds[i],0,&input);
        button.hidden=i ? _summaryMoveDetail || (i==5 && (state->page!=2 || !state->ribbonCount)) : NO;
    }
    UIButtonConfiguration *back=UIButtonConfiguration.filledButtonConfiguration;
    back.baseBackgroundColor=_partyBack.backgroundColor; back.baseForegroundColor=UIColor.whiteColor;
    if (pictures[@"back"]) back.image=pictures[@"back"]; else back.title=@"返回";
    _summaryNavigation[0].configuration=back;
}
- (void)updateBattleControls:(NSData *)data {
    if (data.length != sizeof(HGNativeBattleUIState)) return;
    const HGNativeBattleUIState *state = data.bytes;
    if (state->mode > HG_BATTLE_UI_TARGETS) return;
    [_condition lock]; BOOL scene=state->active && (state->mode || !_selectedEngine); [_condition unlock];
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
    uint32_t displayMode=state->mode ? state->mode : HG_BATTLE_UI_COMMAND;
    _battleBar.accessibilityLabel=displayMode==HG_BATTLE_UI_TARGETS ? @"选择招式目标" : @"战斗操作";
    for (NSUInteger i = 0; i < 6; i++) {
        UIButton *button = i < 4 ? _battleButtons[i] : i == 4 ? _battleBack : _battleBall;
        button.hidden = (i == 4 && displayMode == HG_BATTLE_UI_COMMAND) || (i == 5 && displayMode != HG_BATTLE_UI_COMMAND);
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
        else if (displayMode==HG_BATTLE_UI_TARGETS ? state->targetSpecies[i] : state->move[i]) {
            NSString *name;
            if (displayMode==HG_BATTLE_UI_TARGETS) {
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
                CGImageRelease(image); CGColorSpaceRelease(color); CGDataProviderRelease(provider);
            } else config.title = [NSString stringWithFormat:@"%@ · %@", name, config.title];
        } else config.title = @"—";
        if (i >= 4 || displayMode == HG_BATTLE_UI_COMMAND || (displayMode==HG_BATTLE_UI_TARGETS ? !state->targetSpecies[i] : !state->move[i])) button.accessibilityLabel = config.title;
        button.configuration = config;
    }
}
- (void)keyDown:(OriginalKeyButton *)sender {
    sender.inputGeneration++;
    [_condition lock];
    if (_interrupted && _interruptionEnded && !_inactive) {
        _interrupted = _interruptionEnded = NO; _paused = NO; [_condition signal];
    } else if (!_paused) { _keys |= sender.tag; _pendingKeys |= sender.tag; }
    [_condition unlock];
}
- (void)keyUp:(UIButton *)sender { [_condition lock]; _keys &= ~sender.tag; [_condition unlock]; }
- (void)keyCancel:(UIButton *)sender { [_condition lock]; _keys &= ~sender.tag; _pendingKeys &= ~sender.tag; _submittedKeys &= ~sender.tag; [_condition unlock]; }
- (void)acknowledgeKeysLocked:(uint32_t)reads {
    // Display callbacks can outpace original PAD_Read; keep submitted taps until it samples them.
    if (reads==_lastPadReadCount) return;
    _pendingKeys &= ~_submittedKeys; _pendingJoystickKeys &= ~_submittedJoystickKeys;
    _submittedKeys = _submittedJoystickKeys = 0; _lastPadReadCount=reads;
}
- (uint32_t)takeKeysLocked {
    uint32_t keys = _keys | _pendingKeys;
    if (_fieldMapId && !_selectedEngine && !_fieldMenuOpen) keys |= _joystickKeys | _pendingJoystickKeys;
    _submittedKeys=_pendingKeys;
    _submittedJoystickKeys=_fieldMapId && !_selectedEngine && !_fieldMenuOpen ? _pendingJoystickKeys : 0;
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
    BOOL allowed=_fieldMapId && !_selectedEngine && !_fieldMenuOpen && !_paused && !_stopping && !_battleMode;
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
    [_condition lock];
    if (_fieldMapId) {
        _fieldMenuOpen=!_fieldMenuOpen; _automaticMenuEngine=_fieldMenuOpen; _selectedEngine=0; _fieldMenuFrame=nil;
        _keys=_pendingKeys=_submittedKeys=0;
        if (!_fieldMenuOpen) _pendingKeys=2;
        _fieldMenu.image=nil;
    } else { _automaticMenuEngine=NO; _selectedEngine ^= 1; }
    _touchPressed = NO; uint32_t engine = _selectedEngine; [_condition unlock];
    [self resetJoystick];
    [_menuButton setTitle:engine ? @"返回场景" : @"触摸菜单" forState:UIControlStateNormal];
}
- (void)touchAt:(CGPoint)point pressed:(BOOL)pressed {
    [_condition lock];
    _inputGeneration++;
    BOOL fieldMenu=_fieldMenuOpen && _fieldMapId;
    UIView *surface=fieldMenu ? _fieldMenu : _canvas;
    _touchX = (uint32_t)MAX(0, MIN(255, point.x * 256 / MAX(1, surface.bounds.size.width)));
    _touchY = (uint32_t)MAX(0, MIN(191, point.y * 192 / MAX(1, surface.bounds.size.height)));
    _touchPressed = pressed && !_paused && !_partyScene && !(_bagScene && !_bagPanel.hidden) && !(_summaryScene && !_summaryPanel.hidden)
        && (fieldMenu || _selectedEngine == _touchEngine);
    [_condition unlock];
}
- (void)canvasTouch:(UIPanGestureRecognizer *)gesture {
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
        _battleMenuChoicePending=_battleMenuTouchActive=_battleMenuTouchCooling=NO;
#ifdef HEARTGOLD_ORIGINAL_APP_CHECK
        _checkBattleMenusRequestPending=NO;
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
    [_condition lock]; _stopping = YES; _partyChoicePending = _partyTouchActive = _bagChoicePending = _bagTouchActive = _summaryChoicePending = _summaryInputActive = _summaryTouchPressed = NO; _summaryInputKeys=0; _inputGeneration++;
    _battleMenuChoicePending=_battleMenuTouchActive=_battleMenuTouchCooling=NO;
#ifdef HEARTGOLD_ORIGINAL_APP_CHECK
    _checkBattleMenusRequestPending=NO;
#endif
    [_condition signal]; [_condition unlock];
    for (OriginalKeyButton *button in _buttons.allValues) button.inputGeneration++;
    [_displayLink invalidate]; _displayLink = nil;
    [NSNotificationCenter.defaultCenter removeObserver:self];
}
- (void)displayFrame {
    [_condition lock]; NSData *frame = _frame; uint32_t frameWidth=_frameWidth; _frame = nil; NSString *failure = _failure; NSData *battleUI = _battleUIState;
    BOOL interrupted = _interrupted, ended = _interruptionEnded,paused = _paused,resetStick = _joystickNeedsReset;
    uint32_t fieldMap=_fieldMapId,engine=_selectedEngine,icons=_fieldMenuIcons;
    BOOL menuOpen=_fieldMenuOpen; NSData *menuFrame=_fieldMenuFrame; _fieldMenuFrame=nil;
    NSData *partyUI=_partyUIState,*bagUI=_bagUIState,*summaryUI=_summaryUIState;
    NSData *battleBagUI=_battleBagUIState,*battlePartyUI=_battlePartyUIState;
    NSDictionary *partyPictures=_partyPictures,*bagPictures=_bagPictures,*summaryPictures=_summaryPictures,*battleMenuPictures=_battleMenuPictures; [_condition unlock];
    [self updateBattleControls:battleUI];
    [self updatePartyControls:partyUI pictures:partyPictures paused:paused];
    [self updateBagControls:bagUI pictures:bagPictures paused:paused];
    [self updateSummaryControls:summaryUI pictures:summaryPictures paused:paused];
    [self updateBattleMenuControls:battleBagUI party:battlePartyUI pictures:battleMenuPictures paused:paused engine:engine];
    if (frame) {
        if (_displayedWidth!=frameWidth) { _displayedWidth=frameWidth; [self.view setNeedsLayout]; }
        _canvas.image = OriginalPixelImage(frame,frameWidth,192);
        _status.hidden = YES;
    }
    BOOL field=fieldMap && !engine && !_battleMode && !_battleScene && !_partyScene && !_bagScene && !_summaryScene && !_battleMenuScene;
    if (!field) _fieldMenu.image=nil;
    else if (menuFrame) _fieldMenu.image=OriginalPixelImage(menuFrame,256,192);
    BOOL panel=field && menuOpen && _fieldMenu.image;
    _fieldMenu.hidden=_fieldMenuShade.hidden=!panel; _fieldMenu.userInteractionEnabled=!paused;
    _canvas.userInteractionEnabled=!(field && menuOpen) && !_partyScene && !(_bagScene && !_bagPanel.hidden) && !(_summaryScene && !_summaryPanel.hidden) && !_battleMenuKind;
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
    _canvas.hidden=nativeMenu;
    for (NSNumber *key in @[@0x40,@0x80,@0x20,@0x10]) _buttons[key].hidden=_battleMode || field || nativeMenu;
    _buttons[@1].hidden=_battleMode || (field && menuOpen) || (_summaryScene && !_summaryPanel.hidden) || _battleMenuKind;
    _buttons[@2].hidden=_battleMode || (field && menuOpen) || nativeMenu;
    _buttons[@0x400].hidden=_battleMode || field || _partyScene || _bagScene || _summaryScene || _battleMenuScene;
    _buttons[@8].hidden=_battleMode || ((field || _partyScene || _bagScene || _summaryScene || _battleMenuScene) && !(interrupted && ended));
    _menuButton.hidden=_battleMode || _partyScene || _bagScene || _summaryScene || _battleMenuScene;
    [_buttons[@1] setTitle:field ? @"互动" : @"确认" forState:UIControlStateNormal];
    [_buttons[@2] setTitle:field ? @"奔跑" : @"返回" forState:UIControlStateNormal];
    _buttons[@1].accessibilityLabel=_buttons[@1].currentTitle;
    _buttons[@2].accessibilityLabel=_buttons[@2].currentTitle;
    [_menuButton setTitle:field ? (menuOpen ? @"关闭菜单" : @"菜单") : (engine ? @"返回场景" : @"触摸菜单") forState:UIControlStateNormal];
    if (failure) { _status.text = failure; _status.hidden = NO; }
    else if (interrupted) { _status.text = ended ? @"已暂停，点开始继续" : @"声音中断，已暂停"; _status.hidden = NO; }
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
            if (!ready) { [_condition lock]; _failure = error.localizedDescription ?: @"无法恢复游戏与声音输出。"; _stopping = YES; [_condition unlock]; }
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
    uint32_t battleMenuKind=OriginalBattleMenuKind(currentBattleBag,currentBattleParty);
    BOOL battleChild=currentBattleBag->active || currentBattleParty->active;
    BOOL summaryContext=currentSummary->active && !currentSummary->mode && currentSummary->monDataType==1 && !currentSummary->moveToLearn;
    uint32_t fieldMap=w2c_hgengine_HGNativeFieldMap(_game);
    [_condition lock];
    [self acknowledgeKeysLocked:HGNativeInputPadReads(_env)];
    _fieldMapId=fieldMap;
    // Original LCD routing: SUB_MAIN=0 touches main; MAIN_SUB=1 touches sub.
    _touchEngine=(*_displayControl>>15)&1;
    if (summaryContext) _selectedEngine=currentSummary->page<=2 && (currentSummary->phase==HG_SUMMARY_UI_INPUT || (currentSummary->phase==HG_SUMMARY_UI_MOVE_SELECT && currentSummary->page==1)) ? 0 : _touchEngine;
    else if (battleChild) _selectedEngine=battleMenuKind ? 0 : _touchEngine;
    else if (fieldMap || (currentParty->active && !currentParty->context && !currentParty->panelLayout)) _selectedEngine=0;
    else if (currentBag->active && !currentBag->mode) _selectedEngine=currentBag->phase==HG_BAG_UI_INPUT || currentBag->phase==HG_BAG_UI_ITEM_MENU ? 0 : _touchEngine;
    else if (currentBattle->active && (currentBattle->mode || _automaticMenuEngine)) _selectedEngine=0;
    else if (_automaticMenuEngine) _selectedEngine=_touchEngine;
    if ((!fieldMap || battleChild) && (_joystickKeys || _pendingJoystickKeys)) { _joystickKeys=_pendingJoystickKeys=_submittedJoystickKeys=0; _joystickNeedsReset=YES; }
    if (_audio.failure) { _failure = _audio.failure; _stopping = YES; }
    BOOL stopping = _stopping;
    uint32_t keys = [self takeKeysLocked], x = _touchX, y = _touchY, engine = _selectedEngine,preferredWidth=_preferredViewWidth,preferredBattleWidth=_preferredBattleWidth;
    BOOL battleChoice = _battleChoicePending;
    uint32_t battleRevision = _battleChoiceRevision, battleIndex = _battleChoiceIndex;
    _battleChoicePending = NO;
    BOOL pressed = _touchPressed;
    if (!_paused && !stopping) [self takePartyTouchLocked:currentParty reads:HGNativeInputPadReads(_env) x:&x y:&y pressed:&pressed];
    if (!_paused && !stopping) [self takeBagTouchLocked:currentBag reads:HGNativeInputPadReads(_env) x:&x y:&y pressed:&pressed];
    if (!_paused && !stopping) [self takeBattleMenuTouchLocked:currentBattleBag party:currentBattleParty reads:HGNativeInputPadReads(_env) x:&x y:&y pressed:&pressed];
    if (!_paused && !stopping) [self takeSummaryInputLocked:currentSummary reads:HGNativeInputPadReads(_env) keys:&keys x:&x y:&y pressed:&pressed];
    BOOL menuOpen=_fieldMenuOpen;
#ifdef HEARTGOLD_ORIGINAL_APP_CHECK
    BOOL checkReset=_checkResetPending; _checkResetPending=NO;
    BOOL checkBattleMenusRequest=_checkBattleMenusRequestPending; uint32_t checkBattleMenusRevision=_checkBattleMenusRequestRevision; _checkBattleMenusRequestPending=NO;
#endif
    [_condition unlock];
    if (stopping) w2c_env_HGNativeResetControl(_env, HG_RESET_REQUEST, 0);
#ifdef HEARTGOLD_ORIGINAL_APP_CHECK
    if (checkReset) w2c_env_HGNativeResetControl(_env, HG_RESET_REQUEST, 1);
    if (checkBattleMenusRequest && !stopping) {
        BOOL accepted=w2c_hgengine_HGNativeBattleMenusQARequest(_game,checkBattleMenusRevision)!=0;
        [_condition lock]; _checkBattleMenusRequestAccepted=accepted; [_condition unlock];
    }
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
    NSData *battleBagUI=[NSData dataWithBytes:currentBattleBag length:sizeof(*currentBattleBag)],*battlePartyUI=[NSData dataWithBytes:currentBattleParty length:sizeof(*currentBattleParty)];
#ifdef HEARTGOLD_ORIGINAL_APP_CHECK
    uint32_t qaAddress=w2c_hgengine_HGNativeBattleMenusQAGetState(_game);
    if (!qaAddress || qaAddress>_env->memory->size || sizeof(HGNativeBattleMenusQAState)>_env->memory->size-qaAddress) wasm_rt_trap(WASM_RT_TRAP_OOB);
    const HGNativeBattleMenusQAState *qa=(const HGNativeBattleMenusQAState *)(_env->memory->data+qaAddress);
    if (qa->version!=HG_BATTLE_MENUS_QA_VERSION || qa->byteSize!=sizeof(*qa)) wasm_rt_trap(WASM_RT_TRAP_OOB);
    NSData *battleMenusQA=[NSData dataWithBytes:qa length:sizeof(*qa)];
#endif
    const HGNativeBattleUIState *battleState = battleUI.bytes;
    BOOL originalPage=summaryContext || (currentParty->active && !currentParty->context && !currentParty->panelLayout) || (currentBag->active && !currentBag->mode);
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
    BOOL rendered=battleScene ? w2c_hgengine_HGNativeRenderBattleView(_game,width,_frameBuffer,size) : w2c_hgengine_HGNativeRender2DView(_game,engine,width,_frameBuffer,size);
    if (!rendered) {
        if (width==256) return;
        width=256; size=width*192*4;
        if (!w2c_hgengine_HGNativeSetViewWidth(_game,width) || !w2c_hgengine_HGNativeRender2D(_game,engine,_frameBuffer,size)) return;
    }
#ifdef HEARTGOLD_ORIGINAL_APP_CHECK
    CheckAudioAppStage(&trace,HG_AUDIO_CHECK_APP_COPY,1);
#endif
    NSData *frame = [NSData dataWithBytes:_env->memory->data + _frameBuffer length:size];
    NSDictionary *partyPictures=[self copyPartyPictures:partyUI.bytes frame:frame];
    NSData *bagSubFrame=nil;
    if (currentBag->active && !currentBag->mode) {
#ifdef HEARTGOLD_ORIGINAL_APP_CHECK
        CheckAudioAppStage(&trace,HG_AUDIO_CHECK_APP_RENDER,1);
#endif
        if (!w2c_hgengine_HGNativeRender2D(_game,1,_frameBuffer,256*192*4)) return;
#ifdef HEARTGOLD_ORIGINAL_APP_CHECK
        CheckAudioAppStage(&trace,HG_AUDIO_CHECK_APP_COPY,1);
#endif
        bagSubFrame=[NSData dataWithBytes:_env->memory->data+_frameBuffer length:256*192*4];
    }
    NSDictionary *bagPictures=[self copyBagPictures:bagUI.bytes frame:frame subFrame:bagSubFrame];
    NSData *summarySubFrame=nil;
    if (currentSummary->active && !currentSummary->mode && currentSummary->monDataType==1 && !currentSummary->moveToLearn) {
#ifdef HEARTGOLD_ORIGINAL_APP_CHECK
        CheckAudioAppStage(&trace,HG_AUDIO_CHECK_APP_RENDER,1);
#endif
        if (!w2c_hgengine_HGNativeRender2D(_game,1,_frameBuffer,256*192*4)) return;
#ifdef HEARTGOLD_ORIGINAL_APP_CHECK
        CheckAudioAppStage(&trace,HG_AUDIO_CHECK_APP_COPY,1);
#endif
        summarySubFrame=[NSData dataWithBytes:_env->memory->data+_frameBuffer length:256*192*4];
    }
    NSDictionary *summaryPictures=[self copySummaryPictures:summaryUI.bytes frame:frame subFrame:summarySubFrame];
    NSData *battleMenuSubFrame=nil;
    if (battleMenuKind) {
#ifdef HEARTGOLD_ORIGINAL_APP_CHECK
        CheckAudioAppStage(&trace,HG_AUDIO_CHECK_APP_RENDER,1);
#endif
        if (!w2c_hgengine_HGNativeRender2D(_game,1,_frameBuffer,256*192*4)) return;
#ifdef HEARTGOLD_ORIGINAL_APP_CHECK
        CheckAudioAppStage(&trace,HG_AUDIO_CHECK_APP_COPY,1);
#endif
        battleMenuSubFrame=[NSData dataWithBytes:_env->memory->data+_frameBuffer length:256*192*4];
    }
    NSDictionary *battleMenuPictures=[self copyBattleMenuPictures:battleBagUI.bytes party:battlePartyUI.bytes frame:frame subFrame:battleMenuSubFrame];
    if (battleMenuKind && !battleMenuPictures) return;
    uint32_t icons=w2c_hgengine_HGNativeFieldMenuIcons(_game);
    NSData *menuFrame=nil;
    if (fieldMap && menuOpen) {
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
    [_condition lock]; _frame = frame; _frameWidth=width; _battleUIState = battleUI;
    _partyUIState=partyUI; _partyPictures=partyPictures;
    _bagUIState=bagUI; _bagPictures=bagPictures;
    _summaryUIState=summaryUI; _summaryPictures=summaryPictures;
    _battleBagUIState=battleBagUI; _battlePartyUIState=battlePartyUI; _battleMenuPictures=battleMenuPictures;
    _fieldMenuFrame=menuFrame; _fieldMenuIcons=icons;
#ifdef HEARTGOLD_ORIGINAL_APP_CHECK
    _geometryState = geometry;
    _battleMenusQAState=battleMenusQA;
    _publishedFrames++;
#endif
    [_condition unlock];
#ifdef HEARTGOLD_ORIGINAL_APP_CHECK
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
                if (!restart) break;
                rebooted = YES;
                // Publish fresh UI/audio ownership on the main queue after all old callbacks have been fenced.
                dispatch_sync(dispatch_get_main_queue(), ^{
                    HGNativeBattleUIState battle = {0};
                    HGNativePartyUIState party = {0};
                    HGNativeBagUIState bag = {0};
                    HGNativeSummaryUIState summary = {0};
                    HGNativeBattleBagUIState battleBag={0}; HGNativeBattlePartyUIState battleParty={0};
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
                    self->_summaryChoicePending = self->_summaryInputActive = self->_summaryTouchPressed = NO;
                    self->_summaryChoiceRevision = self->_summaryChoicePhase = self->_summaryChoiceKind = self->_summaryChoiceIndex = 0;
                    self->_summaryInputRevision = self->_summaryInputPhase = self->_summaryInputReads = self->_summaryInputKeys = self->_summaryTouchX = self->_summaryTouchY = 0;
                    self->_battleMenuChoicePending=self->_battleMenuTouchActive=self->_battleMenuTouchCooling=NO;
                    self->_battleMenuChoiceKind=self->_battleMenuChoiceRevision=self->_battleMenuChoicePhase=self->_battleMenuChoiceIndex=0;
                    self->_battleMenuTouchKind=self->_battleMenuTouchRevision=self->_battleMenuTouchReads=self->_battleMenuTouchX=self->_battleMenuTouchY=0;
                    self->_fieldMapId = self->_selectedEngine = 0; self->_touchEngine = 1;
                    self->_fieldMenuOpen = self->_automaticMenuEngine = NO; self->_fieldMenuIcons = 0x0DDDDDDD;
                    self->_frame = self->_fieldMenuFrame = nil; self->_frameWidth = 0;
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
                    self->_restartClean = !self->_keys && !self->_pendingKeys && !self->_submittedKeys &&
                        !self->_joystickKeys && !self->_pendingJoystickKeys && !self->_submittedJoystickKeys &&
                        !self->_touchPressed && !self->_battleChoicePending && !self->_partyChoicePending && !self->_partyTouchActive && !self->_partyOwnerPictures &&
                        !self->_bagChoicePending && !self->_bagTouchActive && !self->_bagOwnerPictures &&
                        !self->_summaryChoicePending && !self->_summaryInputActive && !self->_summaryTouchPressed && !self->_summaryInputKeys && !self->_summaryOwnerPictures && !self->_fieldMapId &&
                        !self->_battleMenuChoicePending && !self->_battleMenuTouchActive && !self->_battleMenuTouchCooling && !self->_battleMenuOwnerPictures && !self->_checkBattleMenusRequestPending &&
                        !self->_fieldMenuOpen && !self->_frameBuffer && !self->_displayControl && !self->_displayStatus;
#endif
                    [self->_condition unlock];
                    for (OriginalKeyButton *button in self->_buttons.allValues) button.inputGeneration++;
                    self->_canvas.image = self->_fieldMenu.image = nil; self->_displayedWidth = 0;
                    self->_fieldMenu.hidden = self->_fieldMenuShade.hidden = YES;
                    self->_canvas.userInteractionEnabled = YES; self->_displayedBattleUIState = nil;
                    [self updateBattleControls:self->_battleUIState]; [self resetJoystick];
                    [self updatePartyControls:self->_partyUIState pictures:nil paused:NO];
                    [self updateBagControls:self->_bagUIState pictures:nil paused:NO];
                    [self updateSummaryControls:self->_summaryUIState pictures:nil paused:NO];
                    [self updateBattleMenuControls:self->_battleBagUIState party:self->_battlePartyUIState pictures:nil paused:NO engine:0];
                    self->_status.text = @"正在启动原版引擎…"; self->_status.hidden = NO;
                    [self.view setNeedsLayout];
                });
            }
        }
        HGNativeBackupSetPath(NULL);
        [_condition lock];
#ifdef HEARTGOLD_ORIGINAL_APP_CHECK
        _audioChanging=NO; _finished = YES;
#endif
        if (!_failure) _failure = @"游戏已停止";
        [_condition unlock];
    }
}
@end

#ifdef HEARTGOLD_ORIGINAL_APP_CHECK
#include "../tests/check_original_app.inc"
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
