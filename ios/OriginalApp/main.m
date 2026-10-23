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
#include "native_party_ui.h"

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

static UIImage *OriginalPartyWindow(const HGNativePartyUIWindow *window,const uint8_t *memory,uint64_t memorySize) {
    uint32_t width=window->tileWidth*8,height=window->tileHeight*8;
    uint64_t bytes=(uint64_t)window->tileWidth*window->tileHeight*32;
    if (!memory || !window->pixelBuffer || !window->tileWidth || window->tileWidth>32 || !window->tileHeight || window->tileHeight>4
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

static UIImage *OriginalPartyCard(NSData *frame,const HGNativePartyUIMon *mon) {
    // Layout 0's original 128x48 panels include the status and held-item sprites.
    if (frame.length!=256*192*4 || mon->hitX<64 || mon->hitX>192 || mon->hitY<24 || mon->hitY>168) return nil;
    uint32_t left=mon->hitX-64,top=mon->hitY-24;
    NSMutableData *pixels=[NSMutableData dataWithLength:128*48*4];
    for (uint32_t y=0;y<48;y++) memcpy((uint8_t *)pixels.mutableBytes+y*128*4,(const uint8_t *)frame.bytes+((top+y)*256+left)*4,128*4);
    return OriginalPixelImage(pixels,128,48);
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
    w2c_hgengine *_game;
    struct w2c_env *_env;
    struct w2c_wasi__snapshot__preview1 *_wasi;
    const vu16 *_displayStatus, *_displayControl;
    uint32_t _frameBuffer, _lastFrame, _preferredViewWidth, _preferredBattleWidth, _frameWidth, _displayedWidth, _fieldMapId;
#ifdef HEARTGOLD_ORIGINAL_APP_CHECK
    uint32_t _publishedFrames, _bootCount, _bootFrameBase, _checkedBootParameter;
    BOOL _suspended, _finished, _checkResetPending, _checkedResetOccurred, _resetQuiesced, _restartClean, _audioChanging;
    NSData *_geometryState;
    NSMutableArray *_checkInputCompletions;
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
- (NSDictionary *)copyPartyPictures:(const HGNativePartyUIState *)state frame:(NSData *)frame {
    if (!state->active || state->context || state->panelLayout) { _partyOwnerPictures=nil; return nil; }
    NSMutableDictionary *pictures=_partyOwnerPictures ? [_partyOwnerPictures mutableCopy] : [NSMutableDictionary new];
    for (NSString *key in @[@"cards",@"actions"]) {
        NSUInteger count=[key isEqual:@"actions"] ? HG_PARTY_UI_ACTIONS : HG_PARTY_UI_SLOTS;
        NSArray *old=pictures[key]; NSMutableArray *images=[NSMutableArray new];
        for (NSUInteger i=0;i<count;i++) {
            UIImage *image=nil;
            if ([key isEqual:@"actions"]) image=OriginalPartyWindow(&state->actions[i].text,_env->memory->data,_env->memory->size);
            else if (state->phase==HG_PARTY_UI_ROSTER && state->ready && state->mons[i].active) image=OriginalPartyCard(frame,&state->mons[i]);
            [images addObject:image ?: (i<old.count ? old[i] : NSNull.null)];
        }
        pictures[key]=[images copy];
    }
    const HGNativePartyUIWindow *windows[]={&state->selectedName,&state->selectedItem,&state->message};
    NSArray *keys=@[@"selected",@"item",@"message"];
    for (NSUInteger i=0;i<3;i++) {
        UIImage *image=OriginalPartyWindow(windows[i],_env->memory->data,_env->memory->size);
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
    NSArray *statusNames=@[@"",@"麻痹",@"冰冻",@"睡眠",@"中毒",@"灼伤",@"濒死",@""];
    for (NSUInteger i=0;i<HG_PARTY_UI_SLOTS;i++) {
        const HGNativePartyUIMon *mon=&state->mons[i]; UIButton *button=_partyMons[i];
        button.hidden=!(state->slotMask&(1u<<i)) || !mon->active;
        button.enabled=ready && state->phase==HG_PARTY_UI_ROSTER;
        button.layer.borderColor=(i==state->selected ? [UIColor colorWithRed:0.91 green:0.49 blue:0.12 alpha:1] : [UIColor colorWithRed:0.45 green:0.62 blue:0.72 alpha:1]).CGColor;
        _partyCards[i].image=i<cards.count && [cards[i] isKindOfClass:UIImage.class] ? cards[i] : nil;
        NSString *name=mon->species<_speciesNames.count ? _speciesNames[mon->species] : @"宝可梦";
        NSString *stats=mon->isEgg ? @"蛋" : [NSString stringWithFormat:@"Lv.%u  %u/%u",mon->level,mon->hp,mon->maxHp];
        NSString *status=mon->status<statusNames.count ? statusNames[mon->status] : @"";
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
    _touchPressed = pressed && !_paused && !_partyScene && (fieldMenu || _selectedEngine == _touchEngine);
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
    if (_paused) { _keys = _pendingKeys = _submittedKeys = _joystickKeys = _pendingJoystickKeys = _submittedJoystickKeys = 0; _joystickNeedsReset = YES; _touchPressed = NO; _battleChoicePending = _partyChoicePending = _partyTouchActive = NO; } else [_condition signal];
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
    [_condition lock]; _stopping = YES; _partyChoicePending = _partyTouchActive = NO; _inputGeneration++; [_condition signal]; [_condition unlock];
    for (OriginalKeyButton *button in _buttons.allValues) button.inputGeneration++;
    [_displayLink invalidate]; _displayLink = nil;
    [NSNotificationCenter.defaultCenter removeObserver:self];
}
- (void)displayFrame {
    [_condition lock]; NSData *frame = _frame; uint32_t frameWidth=_frameWidth; _frame = nil; NSString *failure = _failure; NSData *battleUI = _battleUIState;
    BOOL interrupted = _interrupted, ended = _interruptionEnded,paused = _paused,resetStick = _joystickNeedsReset;
    uint32_t fieldMap=_fieldMapId,engine=_selectedEngine,icons=_fieldMenuIcons;
    BOOL menuOpen=_fieldMenuOpen; NSData *menuFrame=_fieldMenuFrame; _fieldMenuFrame=nil;
    NSData *partyUI=_partyUIState; NSDictionary *partyPictures=_partyPictures; [_condition unlock];
    [self updateBattleControls:battleUI];
    [self updatePartyControls:partyUI pictures:partyPictures paused:paused];
    if (frame) {
        if (_displayedWidth!=frameWidth) { _displayedWidth=frameWidth; [self.view setNeedsLayout]; }
        _canvas.image = OriginalPixelImage(frame,frameWidth,192);
        _status.hidden = YES;
    }
    BOOL field=fieldMap && !engine && !_battleMode && !_battleScene && !_partyScene;
    if (!field) _fieldMenu.image=nil;
    else if (menuFrame) _fieldMenu.image=OriginalPixelImage(menuFrame,256,192);
    BOOL panel=field && menuOpen && _fieldMenu.image;
    _fieldMenu.hidden=_fieldMenuShade.hidden=!panel; _fieldMenu.userInteractionEnabled=!paused;
    _canvas.userInteractionEnabled=!(field && menuOpen) && !_partyScene;
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
    for (NSNumber *key in @[@0x40,@0x80,@0x20,@0x10]) _buttons[key].hidden=_battleMode || field || (_partyScene && !_partyPanel.hidden);
    _buttons[@1].hidden=_battleMode || (field && menuOpen);
    _buttons[@2].hidden=_battleMode || (field && menuOpen) || (_partyScene && !_partyPanel.hidden);
    _buttons[@0x400].hidden=_battleMode || field || _partyScene;
    _buttons[@8].hidden=_battleMode || ((field || _partyScene) && !(interrupted && ended));
    _menuButton.hidden=_battleMode || _partyScene;
    [_buttons[@1] setTitle:field ? @"互动" : @"确认" forState:UIControlStateNormal];
    [_buttons[@2] setTitle:field ? @"奔跑" : @"返回" forState:UIControlStateNormal];
    _buttons[@1].accessibilityLabel=_buttons[@1].currentTitle;
    _buttons[@2].accessibilityLabel=_buttons[@2].currentTitle;
    [_menuButton setTitle:field ? (menuOpen ? @"关闭菜单" : @"菜单") : (engine ? @"返回场景" : @"触摸菜单") forState:UIControlStateNormal];
    if (failure) { _status.text = failure; _status.hidden = NO; }
    else if (interrupted) { _status.text = ended ? @"已暂停，点开始继续" : @"声音中断，已暂停"; _status.hidden = NO; }
}
- (void)engineFrame {
    [_condition lock]; BOOL paused = _paused;
#ifdef HEARTGOLD_ORIGINAL_APP_CHECK
    if (paused) _audioChanging=YES;
#endif
    [_condition unlock];
    if (paused) {
        if (!HGNativeSoundRealtime(_env, NO)) wasm_rt_trap(WASM_RT_TRAP_OOB);
        [_audio pause];
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
    uint32_t partyAddress=w2c_hgengine_HGNativePartyUIGetState(_game);
    if (!partyAddress || partyAddress>_env->memory->size || sizeof(HGNativePartyUIState)>_env->memory->size-partyAddress) wasm_rt_trap(WASM_RT_TRAP_OOB);
    const HGNativePartyUIState *currentParty=(const HGNativePartyUIState *)(_env->memory->data+partyAddress);
    uint32_t fieldMap=w2c_hgengine_HGNativeFieldMap(_game);
    [_condition lock];
    [self acknowledgeKeysLocked:HGNativeInputPadReads(_env)];
    _fieldMapId=fieldMap;
    // Original LCD routing: SUB_MAIN=0 touches main; MAIN_SUB=1 touches sub.
    _touchEngine=(*_displayControl>>15)&1;
    if (fieldMap || (currentParty->active && !currentParty->context && !currentParty->panelLayout)) _selectedEngine=0;
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
    if (!_paused && !stopping) [self takePartyTouchLocked:currentParty reads:HGNativeInputPadReads(_env) x:&x y:&y pressed:&pressed];
    BOOL menuOpen=_fieldMenuOpen;
#ifdef HEARTGOLD_ORIGINAL_APP_CHECK
    BOOL checkReset=_checkResetPending; _checkResetPending=NO;
#endif
    [_condition unlock];
    if (stopping) w2c_env_HGNativeResetControl(_env, HG_RESET_REQUEST, 0);
#ifdef HEARTGOLD_ORIGINAL_APP_CHECK
    if (checkReset) w2c_env_HGNativeResetControl(_env, HG_RESET_REQUEST, 1);
#endif
    // Original sound queries can be 30 Hz; service PCM at every display event, before and after drawing.
    if (!HGNativeSoundRealtime(_env, YES)) wasm_rt_trap(WASM_RT_TRAP_OOB);
    if (battleChoice) w2c_hgengine_HGNativeBattleUIChoose(_game, battleRevision, battleIndex);
    if (!HGNativeSetInput(_env, keys, NO, x, y, pressed)) wasm_rt_trap(WASM_RT_TRAP_OOB);
    uint32_t tick = OS_GetVBlankCount();
    if (tick - _lastFrame < 2) return;
    _lastFrame = tick; HGNativeDrainDMA();
    NSData *battleUI = [NSData dataWithBytes:_env->memory->data + battleAddress length:sizeof(HGNativeBattleUIState)];
    NSData *partyUI = [NSData dataWithBytes:currentParty length:sizeof(*currentParty)];
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
    NSDictionary *partyPictures=[self copyPartyPictures:partyUI.bytes frame:frame];
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
    _partyUIState=partyUI; _partyPictures=partyPictures;
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
                    self->_fieldMapId = self->_selectedEngine = 0; self->_touchEngine = 1;
                    self->_fieldMenuOpen = self->_automaticMenuEngine = NO; self->_fieldMenuIcons = 0x0DDDDDDD;
                    self->_frame = self->_fieldMenuFrame = nil; self->_frameWidth = 0;
                    self->_battleUIState = [NSData dataWithBytes:&battle length:sizeof(battle)];
                    self->_partyUIState = [NSData dataWithBytes:&party length:sizeof(party)];
                    self->_partyPictures = self->_partyOwnerPictures = nil;
                    self->_joystickNeedsReset = YES;
#ifdef HEARTGOLD_ORIGINAL_APP_CHECK
                    self->_geometryState = nil; self->_checkResetPending = NO;
                    self->_restartClean = !self->_keys && !self->_pendingKeys && !self->_submittedKeys &&
                        !self->_joystickKeys && !self->_pendingJoystickKeys && !self->_submittedJoystickKeys &&
                        !self->_touchPressed && !self->_battleChoicePending && !self->_partyChoicePending && !self->_partyTouchActive && !self->_partyOwnerPictures && !self->_fieldMapId &&
                        !self->_fieldMenuOpen && !self->_frameBuffer && !self->_displayControl && !self->_displayStatus;
#endif
                    [self->_condition unlock];
                    for (OriginalKeyButton *button in self->_buttons.allValues) button.inputGeneration++;
                    self->_canvas.image = self->_fieldMenu.image = nil; self->_displayedWidth = 0;
                    self->_fieldMenu.hidden = self->_fieldMenuShade.hidden = YES;
                    self->_canvas.userInteractionEnabled = YES; self->_displayedBattleUIState = nil;
                    [self updateBattleControls:self->_battleUIState]; [self resetJoystick];
                    [self updatePartyControls:self->_partyUIState pictures:nil paused:NO];
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
