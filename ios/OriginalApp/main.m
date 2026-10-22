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

WASM_RT_THREAD_LOCAL wasm_rt_jmp_buf g_wasm_rt_jmp_buf;
void HGAppTrap(wasm_rt_trap_t code) {
    void *frames[48];
    fprintf(stderr,"Original iOS engine trap: %s\n",wasm_rt_strerror(code));
    backtrace_symbols_fd(frames,backtrace(frames,48),STDERR_FILENO);
    WASM_RT_LONGJMP(g_wasm_rt_jmp_buf,code);
}

@interface OriginalKeyButton : UIButton
@end
@implementation OriginalKeyButton
- (BOOL)accessibilityActivate {
    [self sendActionsForControlEvents:UIControlEventTouchDown];
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW,120*NSEC_PER_MSEC),dispatch_get_main_queue(), ^{ [self sendActionsForControlEvents:UIControlEventTouchUpInside]; });
    return YES;
}
@end

@interface OriginalMenuElement : UIAccessibilityElement
@property(nonatomic,copy) BOOL (^activate)(void);
@end
@implementation OriginalMenuElement
- (BOOL)accessibilityActivate { return self.activate ? self.activate() : NO; }
@end

static UIImage *OriginalFrameImage(NSData *frame,uint32_t width) {
    CGDataProviderRef provider=CGDataProviderCreateWithCFData((__bridge CFDataRef)frame);
    CGColorSpaceRef color=CGColorSpaceCreateDeviceRGB();
    CGImageRef image=CGImageCreate(width,192,8,32,width*4,color,kCGBitmapByteOrder32Big | kCGImageAlphaLast,provider,NULL,NO,kCGRenderingIntentDefault);
    UIImage *result=image ? [UIImage imageWithCGImage:image] : nil;
    if (image) CGImageRelease(image);
    CGColorSpaceRelease(color); CGDataProviderRelease(provider); return result;
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
    w2c_hgengine *_game;
    struct w2c_env *_env;
    struct w2c_wasi__snapshot__preview1 *_wasi;
    const vu16 *_displayStatus, *_displayControl;
    uint32_t _frameBuffer, _lastFrame, _preferredViewWidth, _preferredBattleWidth, _frameWidth, _displayedWidth, _fieldMapId;
#ifdef HEARTGOLD_ORIGINAL_APP_CHECK
    uint32_t _publishedFrames;
    BOOL _suspended, _finished;
    NSData *_geometryState;
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
    [NSThread detachNewThreadSelector:@selector(runEngine) toTarget:self withObject:nil];
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
- (void)keyDown:(UIButton *)sender {
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
    BOOL fieldMenu=_fieldMenuOpen && _fieldMapId;
    UIView *surface=fieldMenu ? _fieldMenu : _canvas;
    _touchX = (uint32_t)MAX(0, MIN(255, point.x * 256 / MAX(1, surface.bounds.size.width)));
    _touchY = (uint32_t)MAX(0, MIN(191, point.y * 192 / MAX(1, surface.bounds.size.height)));
    _touchPressed = pressed && !_paused && (fieldMenu || _selectedEngine == _touchEngine);
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
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, 120 * NSEC_PER_MSEC), dispatch_get_main_queue(), ^{ [self touchAt:CGPointZero pressed:NO]; });
}
- (void)updatePauseLocked {
    _paused = _inactive || _interrupted;
    if (_paused) { _keys = _pendingKeys = _submittedKeys = _joystickKeys = _pendingJoystickKeys = _submittedJoystickKeys = 0; _joystickNeedsReset = YES; _touchPressed = NO; _battleChoicePending = NO; } else [_condition signal];
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
    [_condition lock]; _stopping = YES; [_condition signal]; [_condition unlock];
    [_displayLink invalidate]; _displayLink = nil;
    [NSNotificationCenter.defaultCenter removeObserver:self];
}
- (void)displayFrame {
    [_condition lock]; NSData *frame = _frame; uint32_t frameWidth=_frameWidth; _frame = nil; NSString *failure = _failure; NSData *battleUI = _battleUIState;
    BOOL interrupted = _interrupted, ended = _interruptionEnded,paused = _paused,resetStick = _joystickNeedsReset;
    uint32_t fieldMap=_fieldMapId,engine=_selectedEngine,icons=_fieldMenuIcons;
    BOOL menuOpen=_fieldMenuOpen; NSData *menuFrame=_fieldMenuFrame; _fieldMenuFrame=nil; [_condition unlock];
    [self updateBattleControls:battleUI];
    if (frame) {
        if (_displayedWidth!=frameWidth) { _displayedWidth=frameWidth; [self.view setNeedsLayout]; }
        _canvas.image = OriginalFrameImage(frame,frameWidth);
        _status.hidden = YES;
    }
    BOOL field=fieldMap && !engine && !_battleMode && !_battleScene;
    if (!field) _fieldMenu.image=nil;
    else if (menuFrame) _fieldMenu.image=OriginalFrameImage(menuFrame,256);
    BOOL panel=field && menuOpen && _fieldMenu.image;
    _fieldMenu.hidden=_fieldMenuShade.hidden=!panel; _fieldMenu.userInteractionEnabled=!paused;
    _canvas.userInteractionEnabled=!(field && menuOpen);
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
    for (NSNumber *key in @[@0x40,@0x80,@0x20,@0x10]) _buttons[key].hidden=_battleMode || field;
    _buttons[@1].hidden=_buttons[@2].hidden=_battleMode || (field && menuOpen);
    _buttons[@0x400].hidden=_battleMode || field;
    _buttons[@8].hidden=_battleMode || (field && !(interrupted && ended));
    [_buttons[@1] setTitle:field ? @"互动" : @"确认" forState:UIControlStateNormal];
    [_buttons[@2] setTitle:field ? @"奔跑" : @"返回" forState:UIControlStateNormal];
    _buttons[@1].accessibilityLabel=_buttons[@1].currentTitle;
    _buttons[@2].accessibilityLabel=_buttons[@2].currentTitle;
    [_menuButton setTitle:field ? (menuOpen ? @"关闭菜单" : @"菜单") : (engine ? @"返回场景" : @"触摸菜单") forState:UIControlStateNormal];
    if (failure) { _status.text = failure; _status.hidden = NO; }
    else if (interrupted) { _status.text = ended ? @"已暂停，点开始继续" : @"声音中断，已暂停"; _status.hidden = NO; }
}
- (void)engineFrame {
    [_condition lock]; BOOL paused = _paused; [_condition unlock];
    if (paused) {
        if (!HGNativeSoundRealtime(_env, NO)) wasm_rt_trap(WASM_RT_TRAP_OOB);
        [_audio pause];
        HGNativeStopDisplayClock(_env);
        [_condition lock];
#ifdef HEARTGOLD_ORIGINAL_APP_CHECK
        _suspended = YES;
#endif
        while (_paused && !_stopping) [_condition wait];
#ifdef HEARTGOLD_ORIGINAL_APP_CHECK
        _suspended = NO;
#endif
        BOOL stopping = _stopping; [_condition unlock];
        if (!stopping) {
            NSError *error;
            BOOL ready = [_audio resume:&error] && HGNativeSoundRealtime(_env, YES) && HGNativeStartDisplayClock(_env, _displayStatus) && HGNativeSetDisplayObserver(_env, present, (__bridge void *)self);
            if (!ready) { [_condition lock]; _failure = error.localizedDescription ?: @"无法恢复游戏与声音输出。"; _stopping = YES; [_condition unlock]; }
        }
    }
    uint32_t battleAddress=w2c_hgengine_HGNativeBattleUIGetState(_game);
    if (!battleAddress || battleAddress>_env->memory->size || sizeof(HGNativeBattleUIState)>_env->memory->size-battleAddress) wasm_rt_trap(WASM_RT_TRAP_OOB);
    const HGNativeBattleUIState *currentBattle=(const HGNativeBattleUIState *)(_env->memory->data+battleAddress);
    uint32_t fieldMap=w2c_hgengine_HGNativeFieldMap(_game);
    [_condition lock];
    [self acknowledgeKeysLocked:HGNativeInputPadReads(_env)];
    _fieldMapId=fieldMap;
    // Original LCD routing: SUB_MAIN=0 touches main; MAIN_SUB=1 touches sub.
    _touchEngine=(*_displayControl>>15)&1;
    if (fieldMap) _selectedEngine=0;
    else if (currentBattle->active && (currentBattle->mode || _automaticMenuEngine)) _selectedEngine=0;
    else if (_automaticMenuEngine) _selectedEngine=_touchEngine;
    if (!fieldMap && (_joystickKeys || _pendingJoystickKeys)) { _joystickKeys=_pendingJoystickKeys=_submittedJoystickKeys=0; _joystickNeedsReset=YES; }
    if (_audio.failure) { _failure = _audio.failure; _stopping = YES; }
    BOOL stopping = _stopping;
    uint32_t keys = [self takeKeysLocked], x = _touchX, y = _touchY, engine = _selectedEngine,preferredWidth=_preferredViewWidth,preferredBattleWidth=_preferredBattleWidth;
    BOOL battleChoice = _battleChoicePending;
    uint32_t battleRevision = _battleChoiceRevision, battleIndex = _battleChoiceIndex;
    _battleChoicePending = NO;
    BOOL pressed = _touchPressed;
    BOOL menuOpen=_fieldMenuOpen;
    [_condition unlock];
    if (stopping) w2c_env_HGNativeResetControl(_env, HG_RESET_REQUEST, 0);
    // Original sound queries can be 30 Hz; service PCM at every display event, before and after drawing.
    if (!HGNativeSoundRealtime(_env, YES)) wasm_rt_trap(WASM_RT_TRAP_OOB);
    if (battleChoice) w2c_hgengine_HGNativeBattleUIChoose(_game, battleRevision, battleIndex);
    if (!HGNativeSetInput(_env, keys, NO, x, y, pressed)) wasm_rt_trap(WASM_RT_TRAP_OOB);
    uint32_t tick = OS_GetVBlankCount();
    if (tick - _lastFrame < 2) return;
    _lastFrame = tick; HGNativeDrainDMA();
    NSData *battleUI = [NSData dataWithBytes:_env->memory->data + battleAddress length:sizeof(HGNativeBattleUIState)];
    const HGNativeBattleUIState *battleState = battleUI.bytes;
    BOOL battleScene=battleState->active && !engine;
    uint32_t sceneWidth=fieldMap ? MAX(256,preferredWidth) : 256;
    uint32_t width=battleScene ? MAX(256,preferredBattleWidth) : engine ? 256 : sceneWidth;
    uint32_t geometryWidth=battleScene ? MIN(512,width) : sceneWidth;
    if (!w2c_hgengine_HGNativeSetViewWidth(_game,geometryWidth)) wasm_rt_trap(WASM_RT_TRAP_OOB);
    if (!_frameBuffer) _frameBuffer = w2c_hgengine_HGNativeFrameBuffer(_game);
    uint32_t size = width * 192 * 4;
    if (!_frameBuffer || _frameBuffer > _env->memory->size || size > _env->memory->size - _frameBuffer) wasm_rt_trap(WASM_RT_TRAP_OOB);
    BOOL rendered=battleScene ? w2c_hgengine_HGNativeRenderBattleView(_game,width,_frameBuffer,size) : w2c_hgengine_HGNativeRender2DView(_game,engine,width,_frameBuffer,size);
    if (!rendered) {
        if (width==256) return;
        width=256; size=width*192*4;
        if (!w2c_hgengine_HGNativeSetViewWidth(_game,width) || !w2c_hgengine_HGNativeRender2D(_game,engine,_frameBuffer,size)) return;
    }
    NSData *frame = [NSData dataWithBytes:_env->memory->data + _frameBuffer length:size];
    uint32_t icons=w2c_hgengine_HGNativeFieldMenuIcons(_game);
    NSData *menuFrame=nil;
    if (fieldMap && menuOpen && w2c_hgengine_HGNativeRender2D(_game,1,_frameBuffer,256*192*4))
        menuFrame=[NSData dataWithBytes:_env->memory->data+_frameBuffer length:256*192*4];
#ifdef HEARTGOLD_ORIGINAL_APP_CHECK
    // Read the published geometry on its owner thread, after copying the displayed pixels.
    NSData *geometry = w2c_hgengine_HGNativeGeometryFrame(_game, 1, _frameBuffer) ? [NSData dataWithBytes:_env->memory->data + _frameBuffer length:8 * sizeof(uint32_t)] : nil;
#endif
    if (!HGNativeSoundRealtime(_env, YES)) wasm_rt_trap(WASM_RT_TRAP_OOB);
    [_condition lock]; _frame = frame; _frameWidth=width; _battleUIState = battleUI;
    _fieldMenuFrame=menuFrame; _fieldMenuIcons=icons;
#ifdef HEARTGOLD_ORIGINAL_APP_CHECK
    _geometryState = geometry;
    _publishedFrames++;
#endif
    [_condition unlock];
}
- (void)runEngine {
    @autoreleasepool {
        NSString *root = [NSBundle.mainBundle.resourcePath stringByAppendingPathComponent:@"OriginalContent"];
        NSData *header = [NSData dataWithContentsOfFile:[root stringByAppendingPathComponent:@"boot-header.bin"]];
        NSURL *support = [NSFileManager.defaultManager URLsForDirectory:NSApplicationSupportDirectory inDomains:NSUserDomainMask].firstObject;
        support = [support URLByAppendingPathComponent:@"HeartGoldOriginal" isDirectory:YES];
        NSError *error;
        BOOL ready = header.length == 0x160 && [NSFileManager.defaultManager createDirectoryAtURL:support withIntermediateDirectories:YES attributes:nil error:&error];
        ready = ready && HGNativeFSSetRoot([root stringByAppendingPathComponent:@"files"].fileSystemRepresentation) && HGNativeBackupSetPath([[support URLByAppendingPathComponent:@"heartgold.sav"] path].fileSystemRepresentation);
        if (!ready) { [_condition lock]; _failure = @"无法打开原版资源或存档；已有存档未覆盖。"; [_condition unlock]; return; }
        if (![_audio open:&error]) {
            [_audio close]; HGNativeBackupSetPath(NULL);
            [_condition lock]; _failure = error.localizedDescription ?: @"无法初始化声音输出。"; [_condition unlock]; return;
        }
        wasm_rt_init();
        _game = calloc(1, sizeof(*_game)); _env = calloc(1, sizeof(*_env)); _wasi = calloc(1, sizeof(*_wasi));
        if (_wasi) for (unsigned i = 0; i < 3; i++) _wasi->fds[i] = dup(i);
        if (!_game || !_env || !_wasi || _wasi->fds[0] < 0 || _wasi->fds[1] < 0 || _wasi->fds[2] < 0) {
            free(_game); free(_env); _game = NULL; _env = NULL;
            if (_wasi) for (unsigned i = 0; i < 3; i++) if (_wasi->fds[i] >= 0) close(_wasi->fds[i]);
            free(_wasi); _wasi = NULL;
            HGNativeBackupSetPath(NULL);
            [_audio close];
            [_condition lock]; _failure = @"无法分配原版引擎运行资源。"; [_condition unlock]; return;
        }
        wasm_rt_trap_t trap = wasm_rt_impl_try();
        if (!trap) {
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
            if (!HGNativeStartDisplayClock(_env, _displayStatus) || !HGNativeSetDisplayObserver(_env, present, (__bridge void *)self)) wasm_rt_trap(WASM_RT_TRAP_OOB);
            w2c_hgengine_NitroMain(_game);
        }
        // shortcut: unexpected IRQ/fiber faults still use the engine's fail-fast guards; recover only normal reset here.
        BOOL reset = _env->resetRequested != 0;
        _env->soundOutput = NULL; _env->soundOutputContext = NULL;
        [_audio close];
        if (reset) {
            HGNativeFinishReset(_env);
            wasm2c_hgengine_free(_game);
            free(_game); free(_env); _game = NULL; _env = NULL;
            for (unsigned i = 0; i < 3; i++) if (_wasi->fds[i] >= 0) close(_wasi->fds[i]);
            free(_wasi); _wasi = NULL;
            HGNativeBackupSetPath(NULL);
        } else {
            HGNativeStopDisplayClock(_env);
            // Keep memory alive for any outstanding native transfers after an unexpected failure.
        }
        [_condition lock];
#ifdef HEARTGOLD_ORIGINAL_APP_CHECK
        _finished = YES;
#endif
        if (!_failure) _failure = reset ? @"游戏已停止" : trap ? [NSString stringWithFormat:@"原版引擎停止：%s", wasm_rt_strerror(trap)] : @"原版引擎意外返回。";
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
