// SPDX-License-Identifier: GPL-3.0-or-later
// Included after pc_box_check.inc; these fixtures never create a game or environment.
#ifdef HEARTGOLD_ORIGINAL_APP_CHECK
@interface OriginalTrainerCardHostFixture : OriginalController
@end
@implementation OriginalTrainerCardHostFixture
- (void)viewDidLoad {}
@end

@interface OriginalTrainerCardTouchFixture : OriginalJoystick
@property(nonatomic) CGPoint fixtureLocation;
@property(nonatomic) BOOL fixturePressed;
@end
@implementation OriginalTrainerCardTouchFixture
- (CGPoint)location {return _fixtureLocation;}
- (BOOL)pressed {return _fixturePressed;}
@end

@implementation OriginalController (TrainerCardHostChecks)
- (BOOL)checkTrainerCardCallbackSources {
    OriginalController *fixture=[OriginalController new];fixture->_condition=[NSCondition new];fixture->_trainerCardScene=YES;
    fixture->_trainerCardTouches=[NSMutableArray new];fixture->_trainerCardDone=[OriginalKeyButton new];
    OriginalTrainerCardTouchFixture *surface=[OriginalTrainerCardTouchFixture new];surface.bounds=CGRectMake(0,0,256,192);
    fixture->_trainerCardSurface=surface;
    HGNativeTrainerCardUIState state={.active=1,.ownerGeneration=17,.revision=29,.ready=1,.viewActive=1,
        .view=HG_TRAINER_CARD_SIGNATURE,.phase=1,.subState=1};
    NSData *packet=[NSData dataWithBytes:&state length:sizeof(state)];fixture->_trainerCardUIState=fixture->_displayedTrainerCardUIState=packet;
    uint32_t keys=0,x=0,y=0;BOOL pressed=NO;
    surface.fixturePressed=YES;surface.fixtureLocation=CGPointMake(64,80);[fixture trainerCardSurfaceChanged:surface];
    [fixture takeTrainerCardInputLocked:&state reads:100 keys:&keys x:&x y:&y pressed:&pressed];
    BOOL valid=pressed && x==64 && y==80 && fixture->_trainerCardTouchSource==surface;
    [fixture trainerCardDoneDown:fixture->_trainerCardDone];[fixture trainerCardDoneUp:fixture->_trainerCardDone];
    surface.fixtureLocation=CGPointMake(80,90);[fixture trainerCardSurfaceChanged:surface];
    [fixture takeTrainerCardInputLocked:&state reads:101 keys:&keys x:&x y:&y pressed:&pressed];
    valid&=pressed && x==80 && y==90 && fixture->_trainerCardTouchSource==surface;
    surface.fixturePressed=NO;[fixture trainerCardSurfaceChanged:surface];
    [fixture takeTrainerCardInputLocked:&state reads:104 keys:&keys x:&x y:&y pressed:&pressed];valid&=!pressed;
    [fixture takeTrainerCardInputLocked:&state reads:106 keys:&keys x:&x y:&y pressed:&pressed];
    [fixture trainerCardDoneDown:fixture->_trainerCardDone];
    [fixture takeTrainerCardInputLocked:&state reads:200 keys:&keys x:&x y:&y pressed:&pressed];
    valid&=pressed && x==229 && y==179 && fixture->_trainerCardTouchSource==fixture->_trainerCardDone;
    surface.fixturePressed=YES;surface.fixtureLocation=CGPointMake(64,80);[fixture trainerCardSurfaceChanged:surface];
    surface.fixtureLocation=CGPointMake(90,96);[fixture trainerCardSurfaceChanged:surface];
    surface.fixturePressed=NO;[fixture trainerCardSurfaceChanged:surface]; // The same callback handles surface cancellation.
    [fixture takeTrainerCardInputLocked:&state reads:201 keys:&keys x:&x y:&y pressed:&pressed];
    valid&=pressed && x==229 && y==179 && fixture->_trainerCardTouchSource==fixture->_trainerCardDone;
    surface.fixturePressed=YES;[fixture trainerCardSurfaceChanged:surface];
    [fixture trainerCardDoneUp:fixture->_trainerCardDone];
    [fixture takeTrainerCardInputLocked:&state reads:204 keys:&keys x:&x y:&y pressed:&pressed];valid&=!pressed;
    [fixture takeTrainerCardInputLocked:&state reads:206 keys:&keys x:&x y:&y pressed:&pressed];
    surface.fixtureLocation=CGPointMake(100,100);[fixture trainerCardSurfaceChanged:surface];
    valid&=!fixture->_trainerCardTracking && !fixture->_trainerCardTouches.count; // A rejected DOWN cannot restart on MOVE.
    surface.fixturePressed=NO;[fixture trainerCardSurfaceChanged:surface];
    surface.fixturePressed=YES;surface.fixtureLocation=CGPointMake(70,90);[fixture trainerCardSurfaceChanged:surface];
    [fixture takeTrainerCardInputLocked:&state reads:300 keys:&keys x:&x y:&y pressed:&pressed];valid&=pressed;
    [fixture clearTrainerCardInputLocked];HGNativeTrainerCardUIState next=state;next.ownerGeneration++;
    [fixture takeTrainerCardInputLocked:&next reads:301 keys:&keys x:&x y:&y pressed:&pressed];valid&=!pressed;
    NSData *nextPacket=[NSData dataWithBytes:&next length:sizeof(next)];fixture->_trainerCardUIState=fixture->_displayedTrainerCardUIState=nextPacket;
    [fixture takeTrainerCardInputLocked:&next reads:303 keys:&keys x:&x y:&y pressed:&pressed];
    surface.fixtureLocation=CGPointMake(80,100);[fixture trainerCardSurfaceChanged:surface];
    valid&=fixture->_trainerCardSurfaceDown && !fixture->_trainerCardTracking && !fixture->_trainerCardTouches.count;
    surface.fixturePressed=NO;[fixture trainerCardSurfaceChanged:surface];
    surface.fixturePressed=YES;[fixture trainerCardSurfaceChanged:surface];
    [fixture takeTrainerCardInputLocked:&next reads:304 keys:&keys x:&x y:&y pressed:&pressed];
    valid&=pressed && x==80 && y==100 && fixture->_trainerCardTouchLease.ownerGeneration==next.ownerGeneration;
    surface.fixturePressed=NO;[fixture trainerCardSurfaceChanged:surface];
    [fixture takeTrainerCardInputLocked:&next reads:308 keys:&keys x:&x y:&y pressed:&pressed];valid&=!pressed;
    return valid && !fixture->_trainerCardSurfaceDown && !fixture->_trainerCardTouchSource;
}
- (NSDictionary *)checkTrainerCardHostPrimitives {
    OriginalController *fixture=[OriginalController new]; fixture->_condition=[NSCondition new]; fixture->_trainerCardScene=YES;
    fixture->_trainerCardTouches=[NSMutableArray new];
    fixture->_trainerCardSurface=[OriginalJoystick new];fixture->_trainerCardDone=[OriginalKeyButton new];
    OriginalKeyButton *button=[OriginalKeyButton new]; button.enabled=YES; button.hidden=NO; button.tag=1;
    HGNativeTrainerCardUIState viewer={.active=1,.ownerGeneration=17,.revision=29,.ready=1,.viewActive=1,
        .view=HG_TRAINER_CARD_VIEWER,.phase=1,.showingBack=1};
    HGNativeTrainerCardUIState signature=viewer; signature.view=HG_TRAINER_CARD_SIGNATURE; signature.subState=1; signature.showingBack=0;
    NSData *viewerPacket=[NSData dataWithBytes:&viewer length:sizeof(viewer)];
    NSData *signaturePacket=[NSData dataWithBytes:&signature length:sizeof(signature)];
    fixture->_trainerCardUIState=fixture->_displayedTrainerCardUIState=viewerPacket;
    NSMutableData *main=[NSMutableData dataWithLength:256*192*4],*sub=[NSMutableData dataWithLength:256*192*4];
    for (NSUInteger engine=0;engine<2;engine++) {
        uint8_t *pixels=(engine ? sub : main).mutableBytes;
        for (uint32_t y=0;y<192;y++) for (uint32_t x=0;x<256;x++) {
            uint8_t *pixel=pixels+(y*256+x)*4; pixel[0]=x; pixel[1]=y; pixel[2]=engine ? 197 : 43; pixel[3]=255;
        }
    }
    NSDictionary *pictures=[fixture copyTrainerCardPictures:&viewer main:main sub:sub];
    BOOL crops=CheckPCBoxCrop(pictures[@"body"],0,0,256,168,197)
        && CheckPCBoxCrop(pictures[@"footer"],0,168,256,24,197)
        && CheckPCBoxCrop(pictures[@"badges"],0,0,256,192,43)
        && CheckPCBoxCrop(pictures[@"signature"],0,0,256,192,43)
        && CheckPCBoxCrop(pictures[@"prompt"],0,0,256,192,197);
    NSDictionary *signaturePictures=[fixture copyTrainerCardPictures:&signature main:main sub:sub];
    crops&=CheckPCBoxCrop(signaturePictures[@"signature"],0,0,256,192,43);
    HGNativeTrainerCardUIState prompt=viewer; prompt.phase=4;prompt.subState=2;
    prompt.promptLeft=208;prompt.promptTop=48;prompt.promptWidth=48;prompt.promptButtonHeight=32;
    NSDictionary *promptPictures=[fixture copyTrainerCardPictures:&prompt main:main sub:sub];
    // Original viewer window 11 and Yes/No use SUB0; editor prompts use MAIN0.
    crops&=CheckPCBoxCrop(promptPictures[@"prompt"],0,0,256,192,197);
    HGNativeTrainerCardUIState editorPrompt=signature;editorPrompt.subState=3;
    NSDictionary *editorPromptPictures=[fixture copyTrainerCardPictures:&editorPrompt main:main sub:sub];
    crops&=CheckPCBoxCrop(editorPromptPictures[@"prompt"],0,0,256,192,43);
    BOOL invalidFrames=[fixture copyTrainerCardPictures:&viewer main:[NSMutableData dataWithLength:4] sub:sub]==nil
        && [fixture copyTrainerCardPictures:&viewer main:main sub:nil]==nil;

    BOOL whitelist=YES;
    for (NSNumber *key in @[@1,@2,@16,@32]) {
        [fixture clearTrainerCardInputLocked];button.tag=key.unsignedIntValue;[fixture queueTrainerCardKey:button];
        whitelist&=fixture->_trainerCardChoicePending && fixture->_trainerCardChoiceKeys==key.unsignedIntValue;
    }
    for (NSNumber *key in @[@0,@3,@4,@8,@64,@128,@256,@512,@1024,@2048,@4096,@(UINT32_MAX)]) {
        [fixture clearTrainerCardInputLocked];button.tag=key.unsignedIntValue;[fixture queueTrainerCardKey:button];
        whitelist&=!fixture->_trainerCardChoicePending && !fixture->_trainerCardChoiceKeys;
    }
    button.tag=1;
    for (NSUInteger i=0;i<2;i++) {
        HGNativeTrainerCardUIState restricted=viewer;if (i) restricted.isConnected=1;else restricted.showingBack=0;
        NSData *packet=[NSData dataWithBytes:&restricted length:sizeof(restricted)];
        fixture->_trainerCardUIState=fixture->_displayedTrainerCardUIState=packet;[fixture clearTrainerCardInputLocked];
        [fixture queueTrainerCardKey:button];whitelist&=!fixture->_trainerCardChoicePending;
    }
    fixture->_trainerCardUIState=fixture->_displayedTrainerCardUIState=signaturePacket;
    for (NSNumber *key in @[@1,@2,@16,@32,@64,@128]) {
        [fixture clearTrainerCardInputLocked];button.tag=key.unsignedIntValue;[fixture queueTrainerCardKey:button];
        whitelist&=!fixture->_trainerCardChoicePending;
    }
    NSData *promptPacket=[NSData dataWithBytes:&prompt length:sizeof(prompt)];
    fixture->_trainerCardUIState=fixture->_displayedTrainerCardUIState=promptPacket;
    for (NSNumber *key in @[@1,@2,@64,@128]) {
        [fixture clearTrainerCardInputLocked];button.tag=key.unsignedIntValue;[fixture queueTrainerCardKey:button];
        whitelist&=fixture->_trainerCardChoicePending && fixture->_trainerCardChoiceKeys==key.unsignedIntValue;
    }
    button.tag=1;fixture->_trainerCardUIState=fixture->_displayedTrainerCardUIState=viewerPacket;
    BOOL admission=YES;
    for (NSUInteger i=0;i<2;i++) {
        [fixture clearTrainerCardInputLocked];HGNativeTrainerCardUIState stale=viewer;
        if (i) stale.revision++;else stale.ownerGeneration++;
        fixture->_trainerCardUIState=[NSData dataWithBytes:&stale length:sizeof(stale)];[fixture queueTrainerCardKey:button];
        admission&=!fixture->_trainerCardChoicePending;
    }
    BOOL gated=YES;
    for (NSUInteger i=0;i<10;i++) {
        [fixture clearTrainerCardInputLocked];fixture->_paused=fixture->_stopping=NO;fixture->_failure=nil;fixture->_trainerCardScene=YES;
        button.hidden=NO;button.enabled=YES;HGNativeTrainerCardUIState busy=viewer;
        switch (i) {
        case 0:fixture->_paused=YES;break;case 1:fixture->_stopping=YES;break;case 2:fixture->_failure=@"synthetic";break;
        case 3:fixture->_trainerCardScene=NO;break;case 4:button.hidden=YES;break;case 5:button.enabled=NO;break;
        case 6:fixture->_trainerCardInputCooling=YES;break;case 7:fixture->_trainerCardInputActive=YES;break;
        case 8:busy.ready=0;break;case 9:busy.viewActive=0;break;
        }
        fixture->_trainerCardUIState=[NSData dataWithBytes:&busy length:sizeof(busy)];[fixture queueTrainerCardKey:button];
        gated&=!fixture->_trainerCardChoicePending;
    }
    fixture->_paused=fixture->_stopping=NO;fixture->_failure=nil;fixture->_trainerCardScene=YES;
    button.hidden=NO;button.enabled=YES;fixture->_trainerCardUIState=fixture->_displayedTrainerCardUIState=viewerPacket;
    [fixture clearTrainerCardInputLocked];[fixture queueTrainerCardKey:button];uint32_t keys=0,x=0,y=0;BOOL pressed=NO;
    [fixture takeTrainerCardInputLocked:&viewer reads:100 keys:&keys x:&x y:&y pressed:&pressed];
    BOOL keyLease=keys==1 && !pressed && fixture->_trainerCardInputActive && !fixture->_trainerCardChoicePending;
    [fixture takeTrainerCardInputLocked:&viewer reads:103 keys:&keys x:&x y:&y pressed:&pressed];keyLease&=keys==1;
    [fixture takeTrainerCardInputLocked:&viewer reads:104 keys:&keys x:&x y:&y pressed:&pressed];
    keyLease&=!keys && !fixture->_trainerCardInputActive && fixture->_trainerCardInputCooling;
    [fixture queueTrainerCardKey:button];keyLease&=!fixture->_trainerCardChoicePending;
    [fixture takeTrainerCardInputLocked:&viewer reads:105 keys:&keys x:&x y:&y pressed:&pressed];keyLease&=!keys && fixture->_trainerCardInputCooling;
    [fixture takeTrainerCardInputLocked:&viewer reads:106 keys:&keys x:&x y:&y pressed:&pressed];keyLease&=!keys && !fixture->_trainerCardInputCooling;
    BOOL keyRelease=YES;
    for (NSUInteger i=0;i<5;i++) {
        [fixture clearTrainerCardInputLocked];[fixture queueTrainerCardKey:button];
        [fixture takeTrainerCardInputLocked:&viewer reads:200 keys:&keys x:&x y:&y pressed:&pressed];keyRelease&=keys==1;
        HGNativeTrainerCardUIState changed=viewer;
        if (!i) changed.ownerGeneration++;else if (i==1) changed.revision++;else if (i==2) changed.ready=0;else if (i==3) changed.viewActive=0;else changed.phase=0;
        [fixture takeTrainerCardInputLocked:&changed reads:201 keys:&keys x:&x y:&y pressed:&pressed];
        keyRelease&=!keys && !pressed && !fixture->_trainerCardInputActive && fixture->_trainerCardInputCooling;
    }

    [fixture clearTrainerCardInputLocked];fixture->_trainerCardUIState=fixture->_displayedTrainerCardUIState=signaturePacket;
    [fixture queueTrainerCardTouch:HG_TRAINER_CARD_TOUCH_DOWN point:CGPointMake(64,80) source:fixture->_trainerCardSurface];
    [fixture queueTrainerCardTouch:HG_TRAINER_CARD_TOUCH_MOVE point:CGPointMake(80,90) source:fixture->_trainerCardSurface];
    [fixture queueTrainerCardTouch:HG_TRAINER_CARD_TOUCH_MOVE point:CGPointMake(95,95) source:fixture->_trainerCardSurface];
    [fixture queueTrainerCardTouch:HG_TRAINER_CARD_TOUCH_UP point:CGPointMake(95,95) source:fixture->_trainerCardSurface];
    BOOL queueOrder=fixture->_trainerCardTouches.count==3;
    const uint32_t expectedEvents[]={HG_TRAINER_CARD_TOUCH_DOWN,HG_TRAINER_CARD_TOUCH_MOVE,HG_TRAINER_CARD_TOUCH_UP};
    for (NSUInteger i=0;i<fixture->_trainerCardTouches.count;i++) {
        NSData *data=fixture->_trainerCardTouches[i];const uint32_t *event=data.bytes;
        queueOrder&=data.length==5*sizeof(uint32_t) && i<3 && event[0]==signature.ownerGeneration && event[1]==signature.revision && event[2]==expectedEvents[i];
        if (i==1) queueOrder&=event[3]==95 && event[4]==95;
    }
    [fixture takeTrainerCardInputLocked:&signature reads:300 keys:&keys x:&x y:&y pressed:&pressed];queueOrder&=pressed && x==64 && y==80;
    [fixture takeTrainerCardInputLocked:&signature reads:301 keys:&keys x:&x y:&y pressed:&pressed];queueOrder&=pressed && x==95 && y==95;
    [fixture takeTrainerCardInputLocked:&signature reads:302 keys:&keys x:&x y:&y pressed:&pressed];queueOrder&=pressed && fixture->_trainerCardTouchLease.down;
    [fixture takeTrainerCardInputLocked:&signature reads:304 keys:&keys x:&x y:&y pressed:&pressed];queueOrder&=!pressed && !fixture->_trainerCardTouchLease.down;
    [fixture takeTrainerCardInputLocked:&signature reads:306 keys:&keys x:&x y:&y pressed:&pressed];
    [fixture clearTrainerCardInputLocked];
    [fixture queueTrainerCardTouch:HG_TRAINER_CARD_TOUCH_DOWN point:CGPointMake(229,179) source:fixture->_trainerCardDone];
    [fixture queueTrainerCardTouch:HG_TRAINER_CARD_TOUCH_UP point:CGPointZero source:fixture->_trainerCardDone];
    [fixture takeTrainerCardInputLocked:&signature reads:320 keys:&keys x:&x y:&y pressed:&pressed];
    BOOL quickTap=pressed && x==229 && y==179;
    for (uint32_t reads=320;reads<324;reads++) {
        [fixture takeTrainerCardInputLocked:&signature reads:reads keys:&keys x:&x y:&y pressed:&pressed];
        quickTap&=pressed && x==229 && y==179 && fixture->_trainerCardTouchLease.down;
    }
    [fixture takeTrainerCardInputLocked:&signature reads:324 keys:&keys x:&x y:&y pressed:&pressed];
    quickTap&=!pressed && !fixture->_trainerCardTouchLease.down;
    [fixture takeTrainerCardInputLocked:&signature reads:326 keys:&keys x:&x y:&y pressed:&pressed];
    [fixture clearTrainerCardInputLocked];
    [fixture queueTrainerCardTouch:HG_TRAINER_CARD_TOUCH_DOWN point:CGPointMake(64,80) source:fixture->_trainerCardSurface];
    [fixture takeTrainerCardInputLocked:&signature reads:400 keys:&keys x:&x y:&y pressed:&pressed];
    BOOL stroke=pressed && x==64 && y==80 && fixture->_trainerCardTouchLease.down;
    [fixture takeTrainerCardInputLocked:&signature reads:401 keys:&keys x:&x y:&y pressed:&pressed];stroke&=pressed && x==64 && y==80;
    [fixture takeTrainerCardInputLocked:&signature reads:1000 keys:&keys x:&x y:&y pressed:&pressed];stroke&=pressed && x==64 && y==80;
    [fixture queueTrainerCardTouch:HG_TRAINER_CARD_TOUCH_MOVE point:CGPointMake(20,60) source:fixture->_trainerCardSurface];
    [fixture takeTrainerCardInputLocked:&signature reads:1001 keys:&keys x:&x y:&y pressed:&pressed];
    BOOL clipping=pressed && x==20 && y==60;
    NSUInteger pending=fixture->_trainerCardTouches.count;[fixture queueTrainerCardTouch:HG_TRAINER_CARD_TOUCH_MOVE point:CGPointMake(256,80) source:fixture->_trainerCardSurface];
    clipping&=fixture->_trainerCardTouches.count==pending;
    [fixture queueTrainerCardTouch:HG_TRAINER_CARD_TOUCH_UP point:CGPointMake(20,60) source:fixture->_trainerCardSurface];
    pressed=YES;[fixture takeTrainerCardInputLocked:&signature reads:1002 keys:&keys x:&x y:&y pressed:&pressed];
    BOOL touchRelease=!pressed && !fixture->_trainerCardTouchLease.down && fixture->_trainerCardTouchCooling;
    [fixture queueTrainerCardTouch:HG_TRAINER_CARD_TOUCH_DOWN point:CGPointMake(64,80) source:fixture->_trainerCardSurface];touchRelease&=!fixture->_trainerCardTouches.count;
    [fixture takeTrainerCardInputLocked:&signature reads:1003 keys:&keys x:&x y:&y pressed:&pressed];touchRelease&=!pressed && fixture->_trainerCardTouchCooling;
    [fixture takeTrainerCardInputLocked:&signature reads:1004 keys:&keys x:&x y:&y pressed:&pressed];touchRelease&=!pressed && !fixture->_trainerCardTouchCooling;

    [fixture clearTrainerCardInputLocked];[fixture queueTrainerCardTouch:HG_TRAINER_CARD_TOUCH_DOWN point:CGPointMake(64,80) source:fixture->_trainerCardSurface];
    [fixture takeTrainerCardInputLocked:&signature reads:1100 keys:&keys x:&x y:&y pressed:&pressed];
    HGNativeTrainerCardUIState next=signature;next.ownerGeneration++;
    pressed=YES;[fixture takeTrainerCardInputLocked:&next reads:1101 keys:&keys x:&x y:&y pressed:&pressed];
    BOOL fence=!pressed && !fixture->_trainerCardRawDown && !fixture->_trainerCardTouchLease.down && fixture->_trainerCardTouchCooling;
    [fixture takeTrainerCardInputLocked:&next reads:1103 keys:&keys x:&x y:&y pressed:&pressed];
    NSData *nextPacket=[NSData dataWithBytes:&next length:sizeof(next)];fixture->_trainerCardUIState=fixture->_displayedTrainerCardUIState=nextPacket;
    [fixture queueTrainerCardTouch:HG_TRAINER_CARD_TOUCH_DOWN point:CGPointMake(70,90) source:fixture->_trainerCardSurface];
    [fixture takeTrainerCardInputLocked:&next reads:1104 keys:&keys x:&x y:&y pressed:&pressed];
    const uint32_t oldUp[]={signature.ownerGeneration,signature.revision,HG_TRAINER_CARD_TOUCH_UP,64,80};
    [fixture->_trainerCardTouches addObject:[NSData dataWithBytes:oldUp length:sizeof(oldUp)]];
    [fixture takeTrainerCardInputLocked:&next reads:1105 keys:&keys x:&x y:&y pressed:&pressed];
    BOOL staleUp=pressed && x==70 && y==90 && fixture->_trainerCardTouchLease.ownerGeneration==next.ownerGeneration;
    HGNativeTrainerCardUIState invalid=next;invalid.ready=0;
    [fixture takeTrainerCardInputLocked:&invalid reads:1106 keys:&keys x:&x y:&y pressed:&pressed];
    fence&=!pressed && !fixture->_trainerCardTouchLease.down && fixture->_trainerCardTouchCooling;
    [fixture takeTrainerCardInputLocked:&invalid reads:1108 keys:&keys x:&x y:&y pressed:&pressed];[fixture clearTrainerCardInputLocked];
    fixture->_trainerCardUIState=nextPacket;fixture->_displayedTrainerCardUIState=signaturePacket;
    [fixture queueTrainerCardTouch:HG_TRAINER_CARD_TOUCH_DOWN point:CGPointMake(64,80) source:fixture->_trainerCardSurface];admission&=!fixture->_trainerCardTouches.count;

    OriginalController *layout=[OriginalTrainerCardHostFixture new];layout->_condition=[NSCondition new];
    layout.view=[[UIView alloc] initWithFrame:CGRectMake(0,0,844,390)];layout->_trainerCardPanel=[UIView new];
    layout->_trainerCardBody=[UIImageView new];layout->_trainerCardFooter=[UIImageView new];layout->_trainerCardBadges=[UIImageView new];
    layout->_trainerCardSurface=[OriginalJoystick new];layout->_trainerCardDone=[OriginalKeyButton new];layout->_buttons=[NSMutableDictionary new];
    for (NSNumber *key in @[@64,@128,@32,@16,@1,@2,@8]) {
        OriginalKeyButton *control=[OriginalKeyButton new];control.tag=key.unsignedIntValue;layout->_buttons[key]=control;
        [layout.view addSubview:control];
    }
    layout->_buttons[@8].frame=CGRectMake(6,8,88,44);
    [layout.view addSubview:layout->_trainerCardPanel];
    for (UIView *view in @[layout->_trainerCardBody,layout->_trainerCardFooter,layout->_trainerCardBadges,layout->_trainerCardSurface,layout->_trainerCardDone])
        [layout->_trainerCardPanel addSubview:view];
    [layout updateTrainerCardControls:viewerPacket pictures:pictures paused:NO];[layout layoutTrainerCardControls:layout.view.bounds];
    CGRect body=layout->_trainerCardBody.frame,footer=layout->_trainerCardFooter.frame,badges=layout->_trainerCardBadges.frame;
    BOOL geometry=layout->_trainerCardScene && !layout->_trainerCardPanel.hidden
        && CGRectContainsRect(layout.view.bounds,layout->_trainerCardPanel.frame)
        && body.size.width>0 && body.size.height>0 && footer.size.width==body.size.width
        && fabs(CGRectGetMinY(footer)-CGRectGetMaxY(body))<0.5
        && fabs(body.size.width*168-body.size.height*256)<256 && fabs(footer.size.width*24-footer.size.height*256)<256
        && badges.size.width>0 && fabs(badges.size.width*192-badges.size.height*256)<256
        && layout->_trainerCardBody.image==pictures[@"body"] && layout->_trainerCardFooter.image==pictures[@"footer"]
        && layout->_trainerCardBadges.image==pictures[@"badges"];
    [layout updateTrainerCardControls:promptPacket pictures:promptPictures paused:NO];[layout layoutTrainerCardControls:layout.view.bounds];
    geometry&=layout->_trainerCardBody.image==promptPictures[@"prompt"] && layout->_trainerCardFooter.hidden && layout->_trainerCardBadges.hidden;
    [layout updateTrainerCardControls:signaturePacket pictures:signaturePictures paused:NO];[layout layoutTrainerCardControls:layout.view.bounds];
    geometry&=layout->_trainerCardBody.image==signaturePictures[@"signature"] && layout->_trainerCardFooter.hidden && layout->_trainerCardBadges.hidden;
    HGNativeTrainerCardUIState starting=signature;starting.ready=0;NSData *startingPacket=[NSData dataWithBytes:&starting length:sizeof(starting)];
    [layout updateTrainerCardControls:startingPacket pictures:signaturePictures paused:NO];
    geometry&=layout->_trainerCardScene && !layout->_trainerCardSurface.userInteractionEnabled && !layout->_trainerCardDone.enabled;
    HGNativeTrainerCardUIState inactive={0};NSData *inactivePacket=[NSData dataWithBytes:&inactive length:sizeof(inactive)];
    [layout updateTrainerCardControls:inactivePacket pictures:nil paused:NO];
    BOOL retirement=!layout->_trainerCardScene && layout->_trainerCardPanel.hidden;
    for (NSNumber *key in @[@64,@128,@32,@16,@1,@2,@8]) retirement&=layout->_buttons[key].enabled && layout->_buttons[key].alpha==1;
    retirement&=[layout->_buttons[@1].currentTitle isEqual:@"确认"] && [layout->_buttons[@2].currentTitle isEqual:@"返回"]
        && [layout->_buttons[@8].currentTitle isEqual:@"开始"];
    layout->_interrupted=layout->_interruptionEnded=YES;
    [layout updateTrainerCardControls:viewerPacket pictures:pictures paused:YES];
    UIButton *resume=layout->_buttons[@8];
    BOOL resumeFront=!resume.hidden && resume.enabled && [resume.accessibilityLabel isEqual:@"继续"]
        && [layout.view hitTest:CGPointMake(CGRectGetMidX(resume.frame),CGRectGetMidY(resume.frame)) withEvent:nil]==resume;
    [fixture clearTrainerCardInputLocked];
    BOOL cleanup=!fixture->_trainerCardChoicePending && !fixture->_trainerCardInputActive && !fixture->_trainerCardInputCooling
        && !fixture->_trainerCardTouches.count && !fixture->_trainerCardTouchLease.down && !fixture->_trainerCardTracking
        && !fixture->_trainerCardChoiceKeys && !fixture->_trainerCardInputKeys;
    BOOL isolated=!fixture.isViewLoaded && !fixture->_game && !fixture->_env && !layout->_game && !layout->_env && !layout->_displayLink && !layout->_bootCount;
    return @{@"trainerCardHostCoherentSourcePixels":@(crops),@"trainerCardHostInvalidFrames":@(invalidFrames),
        @"trainerCardHostKeyWhitelist":@(whitelist),@"trainerCardHostTokenAdmission":@(admission),@"trainerCardHostBusyAndSuspend":@(gated),
        @"trainerCardHostFourReadKeyLease":@(keyLease),@"trainerCardHostStaleKeyRelease":@(keyRelease),
        @"trainerCardHostTouchEventOrder":@(queueOrder && quickTap),@"trainerCardHostContinuousStroke":@(stroke),
        @"trainerCardHostOriginalClipping":@(clipping),@"trainerCardHostTouchReleaseCooldown":@(touchRelease),
        @"trainerCardHostFenceWithoutEvents":@(fence),@"trainerCardHostOldUpCannotEndNewStroke":@(staleUp),
        @"trainerCardHostCallbackSources":@([self checkTrainerCardCallbackSources]),
        @"trainerCardHostRetirementDefaults":@(retirement),@"trainerCardHostResumeHitTarget":@(resumeFront),
        @"trainerCardHostGeometryAndStartup":@(geometry),@"trainerCardHostCleanupAndIsolation":@(cleanup && isolated)};
}
@end
#endif
