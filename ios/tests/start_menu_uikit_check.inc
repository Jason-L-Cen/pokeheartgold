// SPDX-License-Identifier: GPL-3.0-or-later
// Standalone source fixtures; none creates an engine or audio object.
#ifdef HEARTGOLD_ORIGINAL_APP_CHECK
@interface OriginalStartMenuHostFixture : OriginalController @end
@implementation OriginalStartMenuHostFixture
- (void)viewDidLoad {}
@end
@implementation OriginalController (StartMenuHostChecks)
- (NSDictionary *)checkStartMenuInputPrimitives {
    OriginalController *fixture=[OriginalController new];fixture->_condition=[NSCondition new];
    fixture->_startMenuScene=fixture->_startMenuBlocksInput=YES;fixture->_touchEngine=fixture->_displayedStartMenuEngine=1;
    HGNativeFieldMenuUIState state={.active=1,.ownerGeneration=7,.revision=11,.ready=1,.phase=3,.viewActive=1,.controlActive=1};
    NSData *packet=[NSData dataWithBytes:&state length:sizeof(state)];fixture->_startMenuUIState=fixture->_displayedStartMenuUIState=packet;
    OriginalKeyButton *button=[OriginalKeyButton new];button.enabled=YES;button.tag=2;
    [fixture keyDown:button];BOOL callback=fixture->_startMenuChoicePending && fixture->_startMenuChoiceKeys==2 && !fixture->_fieldMenuOpen;
    uint32_t keys=0,x=0,y=0;BOOL pressed=NO;
    [fixture takeStartMenuInputLocked:&state blocked:NO reads:100 keys:&keys x:&x y:&y pressed:&pressed];
    BOOL pulse=keys==2 && !pressed;
    [fixture takeStartMenuInputLocked:&state blocked:NO reads:103 keys:&keys x:&x y:&y pressed:&pressed];pulse&=keys==2;
    [fixture takeStartMenuInputLocked:&state blocked:NO reads:104 keys:&keys x:&x y:&y pressed:&pressed];pulse&=!keys && !pressed && fixture->_startMenuInputCooling;
    [fixture queueStartMenuInputKeys:1 touch:NO point:CGPointZero];pulse&=!fixture->_startMenuChoicePending;
    [fixture takeStartMenuInputLocked:&state blocked:NO reads:106 keys:&keys x:&x y:&y pressed:&pressed];pulse&=!fixture->_startMenuInputCooling;
    for (NSNumber *key in @[@1,@2,@16,@32,@64,@128,@1024]) {
        [fixture clearStartMenuInputLocked];button.tag=key.unsignedIntValue;[fixture keyDown:button];
        callback&=fixture->_startMenuChoicePending && fixture->_startMenuChoiceKeys==key.unsignedIntValue;
    }
    for (NSNumber *key in @[@0,@3,@8,@2048,@(UINT32_MAX)]) {
        [fixture clearStartMenuInputLocked];button.tag=key.unsignedIntValue;[fixture keyDown:button];callback&=!fixture->_startMenuChoicePending;
    }
    BOOL admission=YES;
    for (NSUInteger index=0;index<3;index++) {
        [fixture clearStartMenuInputLocked];HGNativeFieldMenuUIState live=state;
        if (!index)live.ownerGeneration++;else if(index==1)live.revision++;else fixture->_touchEngine=0;
        fixture->_startMenuUIState=[NSData dataWithBytes:&live length:sizeof(live)];
        [fixture queueStartMenuInputKeys:2 touch:NO point:CGPointZero];admission&=!fixture->_startMenuChoicePending;
        fixture->_touchEngine=1;
    }
    fixture->_startMenuUIState=packet;BOOL fence=YES;
    for (NSUInteger index=0;index<9;index++) {
        [fixture clearStartMenuInputLocked];fixture->_startMenuInputCooling=fixture->_startMenuReleasePending=NO;
        [fixture queueStartMenuInputKeys:0 touch:YES point:CGPointMake(128,96)];
        [fixture takeStartMenuInputLocked:&state blocked:NO reads:200 keys:&keys x:&x y:&y pressed:&pressed];fence&=pressed && x==128 && y==96;
        HGNativeFieldMenuUIState changed=state;
        switch(index){case 0:changed.ownerGeneration++;break;case 1:changed.revision++;break;case 2:changed.ready=0;break;
        case 3:changed.viewActive=0;break;case 4:changed.childActive=1;break;case 5:changed.controlActive=0;break;
        case 6:fixture->_paused=YES;break;case 7:fixture->_touchEngine=0;break;}
        [fixture takeStartMenuInputLocked:&changed blocked:index==8 reads:201 keys:&keys x:&x y:&y pressed:&pressed];
        fence&=!keys && !pressed && !fixture->_startMenuInputActive && fixture->_startMenuInputCooling;
        if(index==8){[fixture takeStartMenuInputLocked:&changed blocked:YES reads:203 keys:&keys x:&x y:&y pressed:&pressed];fence&=!fixture->_startMenuInputCooling;}
        fixture->_paused=NO;fixture->_touchEngine=1;
    }
    [fixture clearStartMenuInputLocked];fixture->_startMenuInputCooling=fixture->_startMenuReleasePending=NO;
    [fixture queueStartMenuInputKeys:0 touch:YES point:CGPointMake(256,96)];admission&=!fixture->_startMenuChoicePending;
    [fixture queueStartMenuInputKeys:0 touch:YES point:CGPointMake(NAN,96)];admission&=!fixture->_startMenuChoicePending;
    BOOL phases=YES,fallback=YES;
    for (NSNumber *phase in @[@4,@15,@16,@17]) {
        HGNativeFieldMenuUIState waiting=state;waiting.phase=phase.unsignedIntValue;waiting.controlActive=waiting.ready=0;
        phases&=OriginalStartMenuHasView(&waiting) && OriginalStartMenuBlocksInput(&waiting) && !OriginalStartMenuFallback(&waiting);
    }
    for (NSNumber *phase in @[@6,@7,@19,@20]) {
        HGNativeFieldMenuUIState bottom=state;bottom.phase=phase.unsignedIntValue;bottom.controlActive=bottom.ready=0;
        phases&=OriginalStartMenuFallback(&bottom) && !OriginalStartMenuBlocksInput(&bottom);
        fixture->_startMenuBlocksInput=NO;fixture->_keys=fixture->_pendingKeys=0;button.tag=1;[fixture keyDown:button];
        fallback&=fixture->_keys==1 && fixture->_pendingKeys==1 && !fixture->_startMenuChoicePending;
    }
    HGNativeFieldMenuUIState child=state;child.phase=5;child.childActive=1;child.viewActive=child.controlActive=child.ready=0;
    phases&=!OriginalStartMenuHasView(&child) && !OriginalStartMenuBlocksInput(&child);
    fixture->_startMenuScene=fixture->_startMenuBlocksInput=NO;fixture->_keys=fixture->_pendingKeys=0;[fixture keyDown:button];
    fallback&=fixture->_keys==1 && !fixture->_startMenuChoicePending;
    HGNativeFieldMenuUIState hint=child;hint.revision++;hint.inputMode=1;
    fallback&=!OriginalStartMenuHandoff(&child,&hint,NO,NO,YES);
    HGNativeFieldMenuUIState returning=state;returning.phase=15;returning.ready=returning.controlActive=returning.viewActive=0;
    phases&=OriginalStartMenuBlocksInput(&returning);
    fallback&=OriginalStartMenuHandoff(&child,&returning,NO,YES,YES);
    fixture->_startMenuScene=fixture->_startMenuBlocksInput=YES;fixture->_stopping=YES;
    [fixture queueStartMenuInputKeys:2 touch:NO point:CGPointZero];BOOL stop=!fixture->_startMenuChoicePending;
    return @{@"startMenuHostActualKeyCallback":@(callback),@"startMenuHostTokenAndLCDAdmission":@(admission),
        @"startMenuHostPulseAndRelease":@(pulse),@"startMenuHostOwnerPauseRetireFence":@(fence && stop),
        @"startMenuHostOriginalPhaseBarriers":@(phases),@"startMenuHostChildFallback":@(fallback)};
}
- (NSDictionary *)checkStartMenuHostPrimitives {
    NSMutableDictionary *gates=[[self checkStartMenuInputPrimitives] mutableCopy];
    OriginalController *layout=[OriginalStartMenuHostFixture new];layout->_condition=[NSCondition new];
    layout.view=[[UIView alloc] initWithFrame:CGRectMake(0,0,844,390)];layout->_buttons=[NSMutableDictionary new];
    layout->_canvas=[UIImageView new];layout->_fieldMenu=[UIImageView new];layout->_fieldMenuShade=[UIView new];
    layout->_joystick=[OriginalJoystick new];layout->_menuButton=[UIButton new];
    for (UIView *view in @[layout->_canvas,layout->_joystick,layout->_fieldMenuShade,layout->_fieldMenu,layout->_menuButton]) [layout.view addSubview:view];
    for (NSNumber *key in @[@1,@2,@16,@32,@64,@128,@8,@1024]) {
        OriginalKeyButton *button=[OriginalKeyButton new];button.tag=key.unsignedIntValue;layout->_buttons[key]=button;[layout.view addSubview:button];
    }
    layout->_fieldMenu.bounds=CGRectMake(0,0,256,192);layout->_touchEngine=1;
    NSMutableData *main=[NSMutableData dataWithLength:256*192*4],*sub=[NSMutableData dataWithLength:256*192*4];
    for (NSUInteger engine=0;engine<2;engine++) {
        uint8_t *pixels=(engine ? sub : main).mutableBytes;
        for(uint32_t y=0;y<192;y++)for(uint32_t x=0;x<256;x++) {uint8_t *p=pixels+(y*256+x)*4;p[0]=x;p[1]=y;p[2]=engine ? 197 : 43;p[3]=255;}
    }
    HGNativeFieldMenuUIState state={.active=1,.ownerGeneration=7,.revision=11,.ready=1,.phase=3,.viewActive=1,.controlActive=1};
    NSData *packet=[NSData dataWithBytes:&state length:sizeof(state)];layout->_startMenuUIState=packet;
    [layout updateStartMenuControls:packet main:main sub:sub engine:1 blocked:NO paused:NO];
    [layout.view layoutIfNeeded];
    BOOL pictures=layout->_startMenuScene && !layout->_fieldMenuOpen && !layout->_fieldMenu.hidden && !layout->_fieldMenuShade.hidden
        && layout->_joystick.hidden && !layout->_canvas.userInteractionEnabled
        && CheckPCBoxCrop(layout->_fieldMenu.image,0,0,256,192,197) && CheckPCBoxCrop(layout->_canvas.image,0,0,256,192,43);
    for(NSNumber *key in @[@1,@2,@1024])pictures&=!CGRectIntersectsRect(layout->_fieldMenu.frame,layout->_buttons[key].frame);
    [layout touchAt:CGPointMake(CGRectGetMidX(layout->_fieldMenu.bounds),CGRectGetMidY(layout->_fieldMenu.bounds)) pressed:YES];
    pictures&=layout->_startMenuChoicePending && layout->_startMenuChoiceX==128 && layout->_startMenuChoiceY==96;
    [layout clearStartMenuInputLocked];[layout toggleMenu];BOOL close=layout->_startMenuChoicePending && layout->_startMenuChoiceKeys==2 && !layout->_fieldMenuOpen;
    [layout clearStartMenuInputLocked];
    [layout updateStartMenuControls:packet main:nil sub:nil engine:1 blocked:NO paused:NO];
    [layout toggleMenu];close&=!layout->_startMenuChoicePending && !layout->_fieldMenu.userInteractionEnabled;
    layout->_touchEngine=0;[layout updateStartMenuControls:packet main:main sub:sub engine:0 blocked:NO paused:NO];
    pictures&=CheckPCBoxCrop(layout->_fieldMenu.image,0,0,256,192,43) && CheckPCBoxCrop(layout->_canvas.image,0,0,256,192,197);
    HGNativeFieldMenuUIState inactive={0};NSData *retired=[NSData dataWithBytes:&inactive length:sizeof(inactive)];
    [layout updateStartMenuControls:retired main:nil sub:nil engine:1 blocked:NO paused:NO];
    [layout.view layoutIfNeeded];
    close&=!layout->_startMenuScene && layout->_menuButton.enabled && layout->_fieldMenuShade.userInteractionEnabled;
    close&=fabs(CGRectGetMaxX(layout->_fieldMenu.frame)-(CGRectGetMaxX(layout.view.bounds)-12))<0.01;
    for(NSNumber *key in @[@1,@2,@16,@32,@64,@128])close&=layout->_buttons[key].enabled;
    layout->_fieldMapId=33;layout->_fieldMenuOpen=NO;[layout toggleMenu];close&=layout->_fieldMenuOpen;
    [layout toggleMenu];close&=!layout->_fieldMenuOpen && layout->_pendingKeys==2;
    gates[@"startMenuHostPairedPlanesAndCoordinates"]=@(pictures);gates[@"startMenuHostOriginalBackAndLocalHUD"]=@(close);
    return gates;
}
@end
#endif
