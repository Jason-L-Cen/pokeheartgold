// SPDX-License-Identifier: GPL-3.0-or-later
#import <UIKit/UIKit.h>
#import <UniformTypeIdentifiers/UniformTypeIdentifiers.h>
#import <AudioToolbox/AudioToolbox.h>
#import <CommonCrypto/CommonDigest.h>
#include "Host.h"
#include "Args.h"
#include "NDS.h"
#include "NDSCart.h"
#include "NDSCart/CartCommon.h"
#include <memory>
#include <stdexcept>

static UIColor* Ink() { return [UIColor colorWithRed:.055 green:.075 blue:.10 alpha:1]; }
static UIColor* Gold() { return [UIColor colorWithRed:1 green:.80 blue:.35 alpha:1]; }
static UIImage* FrameImage(const void* pixels) {
    NSData* bytes = [NSData dataWithBytes:pixels length:256 * 192 * 4];
    CGDataProviderRef provider = CGDataProviderCreateWithCFData((__bridge CFDataRef)bytes);
    CGColorSpaceRef color = CGColorSpaceCreateDeviceRGB();
    CGImageRef image = CGImageCreate(256, 192, 8, 32, 256 * 4, color,
        kCGBitmapByteOrder32Little | kCGImageAlphaNoneSkipFirst, provider, nullptr, false, kCGRenderingIntentDefault);
    UIImage* result = [UIImage imageWithCGImage:image];
    CGImageRelease(image); CGColorSpaceRelease(color); CGDataProviderRelease(provider);
    return result;
}
static UIImage* StarterImage(NSString* species) {
    UIImage* sheet = [UIImage imageNamed:species];
    if (!sheet) return nil;
    CGImageRef frame = CGImageCreateWithImageInRect(sheet.CGImage, CGRectMake(0, 0, 80, 80));
    uint8_t pixel[4]{};
    CGColorSpaceRef color = CGColorSpaceCreateDeviceRGB();
    CGContextRef sample = CGBitmapContextCreate(pixel, 1, 1, 8, 4, color, kCGImageAlphaPremultipliedLast | kCGBitmapByteOrder32Big);
    CGContextDrawImage(sample, CGRectMake(0, 0, 80, 80), frame);
    const CGFloat key[] = {CGFloat(pixel[0]), CGFloat(pixel[0]), CGFloat(pixel[1]), CGFloat(pixel[1]), CGFloat(pixel[2]), CGFloat(pixel[2])};
    CGImageRef masked = CGImageCreateWithMaskingColors(frame, key);
    UIImage* result = [UIImage imageWithCGImage:masked ?: frame];
    if (masked) CGImageRelease(masked);
    CGImageRelease(frame); CGContextRelease(sample); CGColorSpaceRelease(color);
    return result;
}

@interface HGSurface : UIView
@property(nonatomic, strong) UIImageView* scene;
@property(nonatomic, strong) UIImageView* commands;
@property(nonatomic, copy) void (^touchInput)(BOOL, uint16_t, uint16_t);
@end
@implementation HGSurface
- (instancetype)init {
    if ((self = [super init])) {
        self.multipleTouchEnabled = NO;
        self.backgroundColor = UIColor.blackColor;
        _scene = [UIImageView new]; _scene.contentMode = UIViewContentModeScaleAspectFit;
        _scene.layer.magnificationFilter = kCAFilterNearest;
        _commands = [UIImageView new]; _commands.hidden = YES;
        _commands.layer.magnificationFilter = kCAFilterNearest;
        _commands.layer.borderColor = Gold().CGColor; _commands.layer.borderWidth = 2;
        _commands.contentMode = UIViewContentModeScaleToFill;
        [self addSubview:_scene]; [self addSubview:_commands];
    }
    return self;
}
- (void)layoutSubviews {
    [super layoutSubviews]; _scene.frame = self.bounds;
    CGFloat height = MIN(self.bounds.size.height * .68, 240);
    _commands.frame = CGRectMake(self.bounds.size.width - height * 4 / 3 - 12,
                                self.bounds.size.height - height - 12, height * 4 / 3, height);
}
- (void)sendTouch:(UITouch*)touch down:(BOOL)down {
    uint16_t x = 0, y = 0; CGPoint point = [touch locationInView:self]; CGRect r = _commands.frame;
    BOOL inside = !_commands.hidden && HGTouchPoint(point.x, point.y, {r.origin.x, r.origin.y, r.size.width, r.size.height}, x, y);
    if (_touchInput) _touchInput(down && inside, x, y);
}
- (void)touchesBegan:(NSSet<UITouch*>*)touches withEvent:(UIEvent*)event { [self sendTouch:touches.anyObject down:YES]; }
- (void)touchesMoved:(NSSet<UITouch*>*)touches withEvent:(UIEvent*)event { [self sendTouch:touches.anyObject down:YES]; }
- (void)touchesEnded:(NSSet<UITouch*>*)touches withEvent:(UIEvent*)event { [self sendTouch:touches.anyObject down:NO]; }
- (void)touchesCancelled:(NSSet<UITouch*>*)touches withEvent:(UIEvent*)event { [self sendTouch:touches.anyObject down:NO]; }
@end

static void FillAudio(void* data, AudioQueueRef queue, AudioQueueBufferRef buffer) {
    auto* ring = static_cast<HGAudioRing*>(data);
    ring->pop(static_cast<int16_t*>(buffer->mAudioData), buffer->mAudioDataBytesCapacity / sizeof(int16_t));
    buffer->mAudioDataByteSize = buffer->mAudioDataBytesCapacity;
    AudioQueueEnqueueBuffer(queue, buffer, 0, nullptr);
}

@interface HGController : UIViewController <UIDocumentPickerDelegate> {
    std::unique_ptr<melonDS::NDS> _console;
    std::unique_ptr<HGSaveContext> _save;
    dispatch_queue_t _coreQueue;
    std::atomic<uint32_t> _keys, _touch;
    std::atomic<bool> _busy;
    HGAudioRing _audio;
    AudioQueueRef _audioQueue;
    CADisplayLink* _clock;
    HGSurface* _surface;
    UIScrollView* _welcome;
    UIScrollView* _battlePreview;
    UIImageView* _previewAlly;
    UIImageView* _previewFoe;
    UILabel* _allyName;
    UILabel* _foeName;
    UILabel* _previewCaption;
    UIStackView* _previewCommands;
    UIButton* _previewButton;
    UIButton* _backButton;
    UILabel* _headline;
    UILabel* _intro;
    UILabel* _status;
    UIStackView* _starters;
    UIButton* _importButton;
    UIButton* _touchButton;
    UIButton* _pauseButton;
    NSMutableArray<UIButton*>* _keyButtons;
    BOOL _playing;
    BOOL _paused;
    NSString* _storage;
    uint64_t _lastFlush;
    double _nextFrame;
}
@end

@implementation HGController
- (UIInterfaceOrientationMask)supportedInterfaceOrientations { return UIInterfaceOrientationMaskLandscape; }
- (UIInterfaceOrientation)preferredInterfaceOrientationForPresentation { return UIInterfaceOrientationLandscapeRight; }
- (BOOL)prefersStatusBarHidden { return YES; }
- (UIButton*)button:(NSString*)title action:(SEL)action {
    UIButtonConfiguration* c = [UIButtonConfiguration tintedButtonConfiguration];
    c.title = title; c.baseForegroundColor = Gold();
    c.baseBackgroundColor = [Gold() colorWithAlphaComponent:.12];
    c.contentInsets = NSDirectionalEdgeInsetsMake(8, 14, 8, 14);
    UIButton* b = [UIButton buttonWithConfiguration:c primaryAction:nil];
    if (action) [b addTarget:self action:action forControlEvents:UIControlEventTouchUpInside];
    b.accessibilityLabel = title; return b;
}
- (void)viewDidLoad {
    [super viewDidLoad]; self.view.backgroundColor = Ink();
    _coreQueue = dispatch_queue_create("com.jasoncen.heartgold.core", DISPATCH_QUEUE_SERIAL);
    _storage = [NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory, NSUserDomainMask, YES).firstObject stringByAppendingPathComponent:@"HeartGold"];
    NSError* folderError = nil;
    [[NSFileManager defaultManager] createDirectoryAtPath:_storage withIntermediateDirectories:YES attributes:nil error:&folderError];
    HGSetStorageRoot(_storage.UTF8String);
    _surface = [HGSurface new]; _surface.hidden = YES; [self.view addSubview:_surface];
    __weak HGController* weakSelf = self;
    _surface.touchInput = ^(BOOL down, uint16_t x, uint16_t y) {
        HGController* owner = weakSelf;
        if (owner) owner->_touch.store(down ? 0x10000u | (uint32_t(y) << 8) | x : 0);
    };
    _welcome = [UIScrollView new]; [self.view addSubview:_welcome];
    _headline = [UILabel new]; _headline.text = @"HEARTGOLD";
    _headline.font = [UIFont monospacedSystemFontOfSize:36 weight:UIFontWeightBold]; _headline.textColor = Gold();
    _headline.adjustsFontSizeToFitWidth = YES; _headline.minimumScaleFactor = .6;
    [_welcome addSubview:_headline];
    _intro = [UILabel new]; _intro.text = @"重返城都，以一块屏幕。";
    _intro.numberOfLines = 0;
    _intro.font = [UIFont preferredFontForTextStyle:UIFontTextStyleTitle3]; _intro.adjustsFontForContentSizeCategory = YES;
    _intro.textColor = UIColor.whiteColor; [_welcome addSubview:_intro];
    _starters = [[UIStackView alloc] init]; _starters.axis = UILayoutConstraintAxisHorizontal;
    _starters.distribution = UIStackViewDistributionFillEqually; [_welcome addSubview:_starters];
    NSArray* ids = @[@"0152", @"0155", @"0158"];
    NSArray* names = @[@"菊草叶", @"火球鼠", @"小锯鳄"];
    for (NSUInteger i = 0; i < ids.count; ++i) {
        UIStackView* column = [UIStackView new]; column.axis = UILayoutConstraintAxisVertical;
        column.alignment = UIStackViewAlignmentCenter; column.spacing = 4;
        UIImageView* sprite = [[UIImageView alloc] initWithImage:StarterImage(ids[i])];
        sprite.contentMode = UIViewContentModeScaleAspectFit; sprite.layer.magnificationFilter = kCAFilterNearest;
        [sprite.heightAnchor constraintEqualToConstant:96].active = YES;
        [sprite.widthAnchor constraintEqualToConstant:112].active = YES;
        sprite.isAccessibilityElement = YES; sprite.accessibilityLabel = names[i];
        UILabel* name = [UILabel new]; name.text = names[i]; name.textColor = UIColor.whiteColor;
        name.font = [UIFont preferredFontForTextStyle:UIFontTextStyleSubheadline]; name.adjustsFontForContentSizeCategory = YES;
        [column addArrangedSubview:sprite]; [column addArrangedSubview:name]; [_starters addArrangedSubview:column];
    }
    _importButton = [self button:@"导入游戏" action:@selector(importGame)]; _importButton.accessibilityIdentifier = @"import-game";
    [self.view addSubview:_importButton];
    _previewButton = [self button:@"查看单屏布局" action:@selector(showBattlePreview)];
    _previewButton.accessibilityIdentifier = @"battle-layout-preview"; [self.view addSubview:_previewButton];
    _backButton = [self button:@"返回" action:@selector(closeBattlePreview)]; _backButton.hidden = YES;
    [self.view addSubview:_backButton];
    _battlePreview = [UIScrollView new]; _battlePreview.hidden = YES;
    _battlePreview.backgroundColor = [UIColor colorWithRed:.10 green:.15 blue:.16 alpha:1];
    _battlePreview.layer.cornerRadius = 16; [self.view insertSubview:_battlePreview belowSubview:_backButton];
    _previewAlly = [[UIImageView alloc] initWithImage:StarterImage(@"0155-back")];
    _previewFoe = [[UIImageView alloc] initWithImage:StarterImage(@"0158")];
    for (UIImageView* sprite in @[_previewAlly, _previewFoe]) {
        sprite.contentMode = UIViewContentModeScaleAspectFit; sprite.layer.magnificationFilter = kCAFilterNearest;
        [_battlePreview addSubview:sprite];
    }
    _allyName = [UILabel new]; _allyName.text = @"火球鼠";
    _foeName = [UILabel new]; _foeName.text = @"小锯鳄";
    for (UILabel* label in @[_allyName, _foeName]) {
        label.textColor = UIColor.whiteColor; label.font = [UIFont preferredFontForTextStyle:UIFontTextStyleHeadline];
        label.adjustsFontForContentSizeCategory = YES; [_battlePreview addSubview:label];
    }
    _previewCaption = [UILabel new]; _previewCaption.text = @"单屏战斗预览";
    _previewCaption.textColor = Gold(); _previewCaption.font = [UIFont preferredFontForTextStyle:UIFontTextStyleHeadline];
    _previewCaption.adjustsFontForContentSizeCategory = YES; _previewCaption.numberOfLines = 2;
    [_battlePreview addSubview:_previewCaption];
    _previewCommands = [UIStackView new]; _previewCommands.axis = UILayoutConstraintAxisVertical;
    _previewCommands.spacing = 8; _previewCommands.distribution = UIStackViewDistributionFillEqually;
    for (NSString* title in @[@"战斗", @"背包", @"队伍", @"逃跑"]) {
        UIButton* button = [self button:title action:nullptr]; button.enabled = NO;
        UIButtonConfiguration* configuration = button.configuration;
        configuration.titleTextAttributesTransformer = ^NSDictionary*(NSDictionary* attributes) {
            NSMutableDictionary* result = [attributes mutableCopy];
            result[NSForegroundColorAttributeName] = [UIColor colorWithWhite:.7 alpha:1]; return result;
        };
        button.configuration = configuration;
        [_previewCommands addArrangedSubview:button];
    }
    [_battlePreview addSubview:_previewCommands];
    _touchButton = [self button:@"触摸菜单" action:@selector(toggleTouch)]; _touchButton.hidden = YES;
    [self.view addSubview:_touchButton];
    _pauseButton = [self button:@"暂停" action:@selector(togglePause)]; _pauseButton.hidden = YES;
    [self.view addSubview:_pauseButton];
    _status = [UILabel new]; _status.text = @"横屏界面预览 · 尚未导入游戏";
    _status.font = [UIFont preferredFontForTextStyle:UIFontTextStyleCaption1]; _status.adjustsFontForContentSizeCategory = YES;
    _status.textColor = [UIColor colorWithWhite:.78 alpha:1]; _status.numberOfLines = 2;
    _status.accessibilityIdentifier = @"game-status"; [self.view addSubview:_status];
    _keyButtons = [NSMutableArray new];
    NSArray* keyNames = @[@"上", @"左", @"下", @"右", @"A", @"B", @"X", @"Y", @"START", @"SELECT", @"L", @"R"];
    const int bits[] = {6, 5, 7, 4, 0, 1, 10, 11, 3, 2, 9, 8};
    for (NSUInteger i = 0; i < keyNames.count; ++i) {
        UIButton* key = [self button:keyNames[i] action:nullptr]; key.tag = 1 << bits[i];
        [key addTarget:self action:@selector(keyDown:) forControlEvents:UIControlEventTouchDown];
        [key addTarget:self action:@selector(keyUp:) forControlEvents:UIControlEventTouchUpInside | UIControlEventTouchUpOutside | UIControlEventTouchCancel | UIControlEventTouchDragExit];
        [self.view addSubview:key]; [_keyButtons addObject:key]; key.hidden = YES;
    }
    _clock = [CADisplayLink displayLinkWithTarget:self selector:@selector(frame)];
    _clock.preferredFramesPerSecond = 60; _clock.paused = YES;
    [_clock addToRunLoop:NSRunLoop.mainRunLoop forMode:NSRunLoopCommonModes];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(suspend) name:UIApplicationWillResignActiveNotification object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(resume) name:UIApplicationDidBecomeActiveNotification object:nil];
    if (folderError) [self showError:folderError.localizedDescription];
}
- (void)viewDidLayoutSubviews {
    [super viewDidLayoutSubviews]; CGRect safe = UIEdgeInsetsInsetRect(self.view.bounds, self.view.safeAreaInsets);
    CGFloat w = safe.size.width, h = safe.size.height, x = safe.origin.x + 16, y = safe.origin.y;
    CGFloat statusHeight = MAX(36, ceil(_status.font.lineHeight) * 2);
    _importButton.frame = CGRectMake(safe.origin.x + w - 128, y + 4, 112, 44);
    _previewButton.frame = CGRectMake(safe.origin.x + w - 284, y + 4, 148, 44);
    _backButton.frame = CGRectMake(x, y + 4, 84, 44);
    _battlePreview.frame = CGRectMake(x, y + 56, w - 32, h - 64 - statusHeight);
    CGFloat fieldWidth = _battlePreview.bounds.size.width - 184;
    CGFloat commandMinimum = 4 * MAX(44, [UIFont preferredFontForTextStyle:UIFontTextStyleBody].lineHeight + 16) + 24;
    CGFloat fieldHeight = MAX(_battlePreview.bounds.size.height, commandMinimum + 32);
    CGFloat captionHeight = ceil(_previewCaption.font.lineHeight) + 8, nameHeight = ceil(_allyName.font.lineHeight) + 8;
    _battlePreview.contentSize = CGSizeMake(_battlePreview.bounds.size.width, fieldHeight);
    _previewCaption.frame = CGRectMake(20, 12, MAX(180, fieldWidth - 28), captionHeight);
    _previewFoe.frame = CGRectMake(MAX(180, fieldWidth - 200), captionHeight + nameHeight + 20, 160, 140);
    _previewAlly.frame = CGRectMake(24, fieldHeight - nameHeight - 152, 180, 144);
    _allyName.frame = CGRectMake(24, fieldHeight - nameHeight - 8, 180, nameHeight);
    _foeName.frame = CGRectMake(MAX(180, fieldWidth - 200), captionHeight + 20, 180, nameHeight);
    _previewCommands.frame = CGRectMake(_battlePreview.bounds.size.width - 164, 16, 148, fieldHeight - 32);
    _pauseButton.frame = CGRectMake(x, y + 4, 84, 44);
    _touchButton.frame = CGRectMake(x + 92, y + 4, 120, 44);
    _status.frame = CGRectMake(x, y + h - statusHeight, w - 32, statusHeight);
    _welcome.frame = CGRectMake(x + 24, y + 52, w - 80, h - 60 - statusHeight);
    _headline.frame = CGRectMake(0, 0, _welcome.bounds.size.width - 80, 46);
    CGFloat introHeight = ceil([_intro sizeThatFits:CGSizeMake(_welcome.bounds.size.width, CGFLOAT_MAX)].height);
    _intro.frame = CGRectMake(0, 52, _welcome.bounds.size.width, introHeight);
    CGFloat artY = 52 + introHeight + 16;
    CGFloat artHeight = MAX(150, [UIFont preferredFontForTextStyle:UIFontTextStyleSubheadline].lineHeight + 104);
    _starters.frame = CGRectMake(0, artY, _welcome.bounds.size.width, artHeight);
    _welcome.contentSize = CGSizeMake(_welcome.bounds.size.width, artY + artHeight + 16);
    _surface.frame = CGRectMake(x + 64, y + 52, w - 160, h - 60 - statusHeight);
    CGFloat bottom = y + h - 100;
    const CGPoint positions[] = {
        {x + 44, bottom - 96}, {x, bottom - 48}, {x + 44, bottom}, {x + 88, bottom - 48},
        {safe.origin.x + w - 64, bottom - 48}, {safe.origin.x + w - 112, bottom},
        {safe.origin.x + w - 112, bottom - 96}, {safe.origin.x + w - 160, bottom - 48},
        {safe.origin.x + w / 2 + 4, y + h - 92}, {safe.origin.x + w / 2 - 104, y + h - 92},
        {x, y + 56}, {safe.origin.x + w - 64, y + 56}
    };
    for (NSUInteger i = 0; i < _keyButtons.count; ++i) {
        UIButton* key = _keyButtons[i];
        key.hidden = !_playing;
        key.frame = CGRectMake(positions[i].x, positions[i].y, i == 8 || i == 9 ? 100 : 44, 44);
    }
}
- (void)showError:(NSString*)message {
    _status.text = message;
    if (self.presentedViewController) return;
    UIAlertController* alert = [UIAlertController alertControllerWithTitle:@"无法完成操作" message:message preferredStyle:UIAlertControllerStyleAlert];
    [alert addAction:[UIAlertAction actionWithTitle:@"好" style:UIAlertActionStyleDefault handler:nil]];
    [self presentViewController:alert animated:YES completion:nil];
}
- (void)importGame {
    [self suspend];
    UIDocumentPickerViewController* picker = [[UIDocumentPickerViewController alloc] initForOpeningContentTypes:@[UTTypeData] asCopy:YES];
    picker.delegate = self; picker.allowsMultipleSelection = NO;
    [self presentViewController:picker animated:YES completion:nil];
}
- (void)showBattlePreview {
    _welcome.hidden = YES; _previewButton.hidden = YES; _battlePreview.hidden = NO; _backButton.hidden = NO;
    _status.text = @"单屏战斗布局预览 · 按钮尚未接入游戏逻辑";
}
- (void)closeBattlePreview {
    _welcome.hidden = NO; _previewButton.hidden = NO; _battlePreview.hidden = YES; _backButton.hidden = YES;
    _status.text = @"横屏界面预览 · 尚未导入游戏";
}
- (void)documentPickerWasCancelled:(UIDocumentPickerViewController*)controller { [self resume]; }
- (void)documentPicker:(UIDocumentPickerViewController*)controller didPickDocumentsAtURLs:(NSArray<NSURL*>*)urls {
    NSURL* url = urls.firstObject; if (!url) { [self resume]; return; }
    _importButton.enabled = NO; _status.text = @"正在读取游戏…";
    dispatch_async(_coreQueue, ^{
        NSError* error = nil;
        BOOL scoped = [url startAccessingSecurityScopedResource];
        NSNumber* size = nil; [url getResourceValue:&size forKey:NSURLFileSizeKey error:&error];
        NSData* bytes = nil;
        if (!error && size.unsignedLongLongValue >= 512 && size.unsignedLongLongValue <= 512ull * 1024 * 1024)
            bytes = [NSData dataWithContentsOfURL:url options:NSDataReadingMappedIfSafe error:&error];
        NSString* failure = nil;
        if (!bytes || !HGIsSupportedROM(static_cast<const uint8_t*>(bytes.bytes), bytes.length))
            failure = error.localizedDescription ?: @"请选择完整的美版《心金》或《魂银》.nds 文件。";
        if (!failure) {
            unsigned char hash[CC_SHA256_DIGEST_LENGTH]; CC_SHA256(bytes.bytes, (CC_LONG)bytes.length, hash);
            NSMutableString* name = [NSMutableString new];
            for (unsigned char c : hash) [name appendFormat:@"%02x", c];
            NSString* savePath = [self->_storage stringByAppendingPathComponent:[name stringByAppendingString:@".sav"]];
            try {
                auto context = std::make_unique<HGSaveContext>(); context->path = savePath.UTF8String;
                melonDS::NDSCart::NDSCartArgs cartArgs;
                if ([[NSFileManager defaultManager] fileExistsAtPath:savePath]) {
                    NSData* saved = [NSData dataWithContentsOfFile:savePath options:0 error:&error];
                    if (!saved || !saved.length || saved.length > 16 * 1024 * 1024)
                        throw std::runtime_error("Existing save could not be read; it has not been replaced.");
                    cartArgs.SRAMLength = (uint32_t)saved.length;
                    cartArgs.SRAM = std::make_unique<uint8_t[]>(saved.length);
                    memcpy(cartArgs.SRAM.get(), saved.bytes, saved.length);
                }
                auto rom = std::make_unique<uint8_t[]>(bytes.length); memcpy(rom.get(), bytes.bytes, bytes.length);
                auto cart = melonDS::NDSCart::ParseROM(std::move(rom), (uint32_t)bytes.length, context.get(), std::move(cartArgs));
                if (!cart) throw std::runtime_error("The game header or executable sections are invalid.");
                melonDS::NDSArgs args; args.JIT = std::nullopt;
                auto console = std::make_unique<melonDS::NDS>(std::move(args), context.get());
                console->SetNDSCart(std::move(cart)); console->Reset();
                console->SetupDirectBoot(url.lastPathComponent.UTF8String); console->Start();
                NSDateComponents* date = [NSCalendar.currentCalendar components:NSCalendarUnitYear | NSCalendarUnitMonth | NSCalendarUnitDay | NSCalendarUnitHour | NSCalendarUnitMinute | NSCalendarUnitSecond fromDate:NSDate.date];
                console->RTC.SetDateTime((int)date.year, (int)date.month, (int)date.day, (int)date.hour, (int)date.minute, (int)date.second);
                if (self->_save && !HGFlushSave(*self->_save)) throw std::runtime_error(self->_save->error);
                self->_console = std::move(console); self->_save = std::move(context);
            } catch (const std::exception& ex) { failure = [NSString stringWithUTF8String:ex.what()]; }
        }
        if (scoped) [url stopAccessingSecurityScopedResource];
        dispatch_async(dispatch_get_main_queue(), ^{
            self->_importButton.enabled = YES;
            if (failure) { [self showError:failure]; [self resume]; return; }
            self->_playing = YES; self->_paused = NO; self->_welcome.hidden = YES;
            UIButtonConfiguration* pause = self->_pauseButton.configuration; pause.title = @"暂停"; self->_pauseButton.configuration = pause;
            self->_previewButton.hidden = YES; self->_battlePreview.hidden = YES; self->_backButton.hidden = YES;
            self->_surface.hidden = NO; self->_touchButton.hidden = NO; self->_pauseButton.hidden = NO;
            self->_status.text = @"运行适配验证 · 触摸菜单为临时合成界面";
            [self.view setNeedsLayout]; [self startAudio]; [self resume];
        });
    });
}
- (void)startAudio {
    if (_audioQueue) return;
    AudioStreamBasicDescription format{}; format.mSampleRate = 48000;
    format.mFormatID = kAudioFormatLinearPCM; format.mFormatFlags = kLinearPCMFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked;
    format.mBytesPerPacket = format.mBytesPerFrame = 4; format.mFramesPerPacket = 1;
    format.mChannelsPerFrame = 2; format.mBitsPerChannel = 16;
    if (AudioQueueNewOutput(&format, FillAudio, &_audio, nullptr, nullptr, 0, &_audioQueue)) {
        _audioQueue = nullptr; _status.text = @"音频设备初始化失败，游戏可继续。"; return;
    }
    for (int i = 0; i < 3; ++i) {
        AudioQueueBufferRef buffer;
        if (!AudioQueueAllocateBuffer(_audioQueue, 4096, &buffer)) FillAudio(&_audio, _audioQueue, buffer);
    }
    AudioQueueStart(_audioQueue, nullptr);
}
- (void)frame {
    if (_paused || _busy.load() || !HGFrameDue(_clock.timestamp, _nextFrame)) return;
    _busy.store(true);
    dispatch_async(_coreQueue, ^{
        @autoreleasepool {
            UIImage* top = nil; UIImage* bottom = nil; NSString* failure = nil;
            if (self->_console) {
                self->_console->SetKeyMask(~self->_keys.load() & 0xfff);
                auto touch = self->_touch.load();
                if (touch & 0x10000) self->_console->TouchScreen(touch & 0xff, (touch >> 8) & 0xff);
                else self->_console->ReleaseScreen();
                self->_console->RunFrame();
                void *a = nullptr, *b = nullptr;
                if (self->_console->GPU.GetFramebuffers(&a, &b)) { top = FrameImage(a); bottom = FrameImage(b); }
                int16_t samples[4096]; int count = self->_console->SPU.ReadOutput(samples, 2048);
                if (count > 0) self->_audio.push(samples, count * 2);
                auto now = melonDS::Platform::GetMSCount();
                if (now - self->_lastFlush >= 1000 && self->_save) {
                    self->_lastFlush = now;
                    if (!HGFlushSave(*self->_save)) failure = [NSString stringWithUTF8String:self->_save->error.c_str()];
                }
                if (self->_save->stopped) failure = @"游戏已停止。请重新导入，现有存档已保留。";
            }
            dispatch_async(dispatch_get_main_queue(), ^{
                self->_surface.scene.image = top; self->_surface.commands.image = bottom;
                self->_busy.store(false);
                if (failure) { [self suspend]; [self showError:failure]; }
            });
        }
    });
}
- (void)keyDown:(UIButton*)key { _keys.fetch_or((uint32_t)key.tag); }
- (void)keyUp:(UIButton*)key { _keys.fetch_and(~(uint32_t)key.tag); }
- (void)toggleTouch {
    _touch.store(0); _surface.commands.hidden = !_surface.commands.hidden;
    _touchButton.accessibilityValue = _surface.commands.hidden ? @"已关闭" : @"已展开";
}
- (void)togglePause {
    _paused = !_paused; _clock.paused = _paused;
    UIButtonConfiguration* configuration = _pauseButton.configuration;
    configuration.title = _paused ? @"继续" : @"暂停"; _pauseButton.configuration = configuration;
    if (_paused) [self suspend]; else [self resume];
}
- (void)suspend {
    _clock.paused = YES; _keys.store(0); _touch.store(0);
    if (_audioQueue) AudioQueuePause(_audioQueue);
    UIBackgroundTaskIdentifier task = [UIApplication.sharedApplication beginBackgroundTaskWithExpirationHandler:nil];
    dispatch_async(_coreQueue, ^{
        NSString* error = nil;
        if (self->_save && !HGFlushSave(*self->_save)) error = [NSString stringWithUTF8String:self->_save->error.c_str()];
        dispatch_async(dispatch_get_main_queue(), ^{
            if (error) self->_status.text = error;
            if (task != UIBackgroundTaskInvalid) [UIApplication.sharedApplication endBackgroundTask:task];
        });
    });
}
- (void)resume {
    if (_playing && !_paused && UIApplication.sharedApplication.applicationState == UIApplicationStateActive) {
        _nextFrame = 0;
        _clock.paused = NO;
        if (_audioQueue) AudioQueueStart(_audioQueue, nullptr);
    }
}
@end

@interface HGSceneDelegate : UIResponder <UIWindowSceneDelegate>
@property(nonatomic, strong) UIWindow* window;
@end
@implementation HGSceneDelegate
- (void)scene:(UIScene*)scene willConnectToSession:(UISceneSession*)session options:(UISceneConnectionOptions*)options {
    if (![scene isKindOfClass:UIWindowScene.class]) return;
    self.window = [[UIWindow alloc] initWithWindowScene:(UIWindowScene*)scene];
    self.window.rootViewController = [HGController new]; [self.window makeKeyAndVisible];
}
@end
@interface HGAppDelegate : UIResponder <UIApplicationDelegate>
@end
@implementation HGAppDelegate
- (BOOL)application:(UIApplication*)application didFinishLaunchingWithOptions:(NSDictionary*)options { return YES; }
- (UISceneConfiguration*)application:(UIApplication*)application configurationForConnectingSceneSession:(UISceneSession*)session options:(UISceneConnectionOptions*)options {
    UISceneConfiguration* configuration = [[UISceneConfiguration alloc] initWithName:@"Game" sessionRole:session.role];
    configuration.sceneClass = UIWindowScene.class; configuration.delegateClass = HGSceneDelegate.class;
    return configuration;
}
@end
int main(int argc, char** argv) {
    @autoreleasepool { return UIApplicationMain(argc, argv, nil, NSStringFromClass(HGAppDelegate.class)); }
}
