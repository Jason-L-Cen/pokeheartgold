// SPDX-License-Identifier: GPL-3.0-or-later
#ifdef HEARTGOLD_ORIGINAL_DEFAULT_CHECK
@interface OriginalPokegearSurfaceFixture:OriginalJoystick
@property(nonatomic) BOOL checkPressed;
@property(nonatomic) BOOL checkCancelled;
@property(nonatomic) CGPoint checkLocation;
@end
@implementation OriginalPokegearSurfaceFixture
- (BOOL)pressed{return self.checkPressed;}
- (BOOL)cancelled{return self.checkCancelled;}
- (CGPoint)location{return self.checkLocation;}
@end
@interface OriginalPokegearHostFixture:OriginalController @end
@implementation OriginalPokegearHostFixture
- (void)viewDidLoad {}
@end
@implementation OriginalController (PokegearHostCheck)
- (NSDictionary *)checkPokegearInputPrimitives {
    OriginalController *f=[OriginalController new];f->_condition=[NSCondition new];f->_pokegearScene=YES;
    OriginalPokegearSurfaceFixture *surface=[OriginalPokegearSurfaceFixture new];surface.bounds=CGRectMake(0,0,256,192);f->_pokegearSurface=surface;
    HGNativePokegearUIState state={.active=1,.ownerGeneration=7,.revision=9,.ready=1,.ownerKind=HG_POKEGEAR_OWNER_GEAR,
        .parentPhase=3,.viewActive=1,.viewKind=HG_POKEGEAR_CONFIGURE,.phase=1,.app=0,.registeredCards=0};
    void (^publish)(HGNativePokegearUIState)=^(HGNativePokegearUIState s){NSData *d=[NSData dataWithBytes:&s length:sizeof(s)];f->_pokegearRawUIState=f->_displayedPokegearUIState=d;};
    void (^reset)(void)=^{[f clearPokegearInputLocked];f->_pokegearRawDown=f->_pokegearReleasePending=f->_pokegearCooling=f->_pokegearSurfaceDown=NO;
        f->_paused=f->_stopping=NO;f->_touchEngine=0;surface.checkPressed=surface.checkCancelled=NO;publish(state);};
    reset();OriginalKeyButton *button=[OriginalKeyButton new];button.enabled=YES;
    uint32_t keys=0,x=0,y=0;BOOL pressed=NO,callback=YES,pulse=YES,admission=YES;
    for(NSNumber *key in @[@1,@2,@16,@32,@64,@128,@1024,@2048]) {
        reset();button.tag=key.unsignedIntValue;[f keyDown:button];callback&=f->_pokegearChoicePending && f->_pokegearChoiceKeys==button.tag && !f->_keys;
        [f keyUp:button];[f takePokegearInputLocked:&state reads:100 keys:&keys x:&x y:&y pressed:&pressed];pulse&=keys==button.tag && !pressed;
        [f takePokegearInputLocked:&state reads:103 keys:&keys x:&x y:&y pressed:&pressed];pulse&=keys==button.tag;
        [f takePokegearInputLocked:&state reads:104 keys:&keys x:&x y:&y pressed:&pressed];pulse&=!keys && f->_pokegearCooling;
        [f takePokegearInputLocked:&state reads:106 keys:&keys x:&x y:&y pressed:&pressed];pulse&=!f->_pokegearCooling;
    }
    for(NSNumber *key in @[@0,@4,@8,@3,@4096]){reset();button.tag=key.unsignedIntValue;[f keyDown:button];callback&=!f->_pokegearChoicePending;}
    for(NSUInteger i=0;i<3;i++){
        reset();HGNativePokegearUIState live=state;if(!i)live.ownerGeneration++;else if(i==1)live.revision++;else f->_touchEngine=1;
        f->_pokegearRawUIState=[NSData dataWithBytes:&live length:sizeof(live)];button.tag=2;[f keyDown:button];admission&=!f->_pokegearChoicePending;
    }
    reset();surface.checkPressed=YES;surface.checkLocation=CGPointMake(128,96);[f pokegearSurfaceChanged:surface];
    [f takePokegearInputLocked:&state reads:200 keys:&keys x:&x y:&y pressed:&pressed];BOOL stroke=pressed && x==128 && y==96;
    surface.checkLocation=CGPointMake(80,176);[f pokegearSurfaceChanged:surface];
    [f takePokegearInputLocked:&state reads:201 keys:&keys x:&x y:&y pressed:&pressed];stroke&=pressed && x==80 && y==176;
    surface.checkLocation=CGPointMake(300,-30);[f pokegearSurfaceChanged:surface];
    [f takePokegearInputLocked:&state reads:202 keys:&keys x:&x y:&y pressed:&pressed];stroke&=pressed && x==255 && y==0;
    button.tag=1024;[f keyDown:button];[f keyUp:button];stroke&=f->_pokegearRawDown && !f->_pokegearChoicePending;
    surface.checkPressed=NO;[f pokegearSurfaceChanged:surface];
    [f takePokegearInputLocked:&state reads:203 keys:&keys x:&x y:&y pressed:&pressed];stroke&=pressed;
    [f takePokegearInputLocked:&state reads:204 keys:&keys x:&x y:&y pressed:&pressed];stroke&=!pressed && f->_pokegearCooling;
    reset();surface.checkPressed=YES;surface.checkLocation=CGPointMake(80,176);[f pokegearSurfaceChanged:surface];
    BOOL rejected=!f->_pokegearTracking && !f->_pokegearTouches.count;
    surface.checkLocation=CGPointMake(128,96);[f pokegearSurfaceChanged:surface];rejected&=!f->_pokegearTouches.count;
    surface.checkPressed=NO;[f pokegearSurfaceChanged:surface];
    button.tag=1;[f keyDown:button];[f takePokegearInputLocked:&state reads:300 keys:&keys x:&x y:&y pressed:&pressed];rejected&=keys==1;
    surface.checkPressed=YES;[f pokegearSurfaceChanged:surface];surface.checkLocation=CGPointMake(80,176);[f pokegearSurfaceChanged:surface];
    surface.checkPressed=NO;[f pokegearSurfaceChanged:surface];
    [f takePokegearInputLocked:&state reads:301 keys:&keys x:&x y:&y pressed:&pressed];rejected&=keys==1 && !pressed && !f->_pokegearTouches.count;
    BOOL fence=YES;
    for(NSUInteger i=0;i<9;i++){
        reset();surface.checkPressed=YES;surface.checkLocation=CGPointMake(128,96);[f pokegearSurfaceChanged:surface];
        [f takePokegearInputLocked:&state reads:400 keys:&keys x:&x y:&y pressed:&pressed];fence&=pressed;
        HGNativePokegearUIState changed=state;
        switch(i){case 0:changed.ownerGeneration++;break;case 1:changed.revision++;break;case 2:changed.ready=0;break;case 3:changed.viewActive=0;break;
            case 4:changed.active=0;break;case 5:f->_paused=YES;break;case 6:f->_stopping=YES;break;case 7:f->_touchEngine=1;break;case 8:[f clearPokegearInputLocked];changed.ownerGeneration++;break;}
        [f takePokegearInputLocked:&changed reads:401 keys:&keys x:&x y:&y pressed:&pressed];fence&=!pressed && !keys && f->_pokegearSurfaceDown;
        f->_paused=f->_stopping=NO;f->_touchEngine=0;publish(state);
        [f takePokegearInputLocked:&state reads:403 keys:&keys x:&x y:&y pressed:&pressed];
        surface.checkLocation=CGPointMake(60,90);[f pokegearSurfaceChanged:surface];fence&=!f->_pokegearTouches.count;
        surface.checkPressed=NO;[f pokegearSurfaceChanged:surface];surface.checkPressed=YES;[f pokegearSurfaceChanged:surface];
        [f takePokegearInputLocked:&state reads:405 keys:&keys x:&x y:&y pressed:&pressed];fence&=pressed && x==60 && y==90;
    }
    reset();surface.checkPressed=YES;surface.checkLocation=CGPointMake(128,96);[f pokegearSurfaceChanged:surface];
    [f takePokegearInputLocked:&state reads:500 keys:&keys x:&x y:&y pressed:&pressed];fence&=pressed;
    surface.checkPressed=NO;surface.checkCancelled=YES;[f pokegearSurfaceChanged:surface];
    [f takePokegearInputLocked:&state reads:501 keys:&keys x:&x y:&y pressed:&pressed];fence&=!pressed && !f->_pokegearSurfaceDown;
    reset();HGNativePokegearUIState child=state;child.viewActive=child.ready=0;child.viewKind=HG_POKEGEAR_EASY_CHAT;child.parentPhase=8;
    BOOL fallback=!OriginalPokegearOwnsInput(&child);child.parentPhase=9;fallback&=!OriginalPokegearOwnsInput(&child);
    for(NSNumber *phase in @[@0,@1,@2,@4,@6,@10,@12]){child.parentPhase=phase.unsignedIntValue;child.viewKind=HG_POKEGEAR_NO_VIEW;fallback&=OriginalPokegearOwnsInput(&child);}
    f->_pokegearScene=NO;button.tag=1;[f keyDown:button];fallback&=f->_keys==1 && f->_pendingKeys==1 && !f->_pokegearChoicePending;
    return @{@"pokegearHostActualKeyCallbacks":@(callback),@"pokegearHostFourReadRelease":@(pulse),@"pokegearHostTokenLCDAdmission":@(admission),
        @"pokegearHostContinuousTouchAndClipping":@(stroke),@"pokegearHostRejectedSourceIsolation":@(rejected),
        @"pokegearHostOwnerPauseHeldFence":@(fence),@"pokegearHostEasyChatFallback":@(fallback)};
}
- (NSDictionary *)checkPokegearHostPrimitives {
    NSMutableDictionary *gates=[[self checkPokegearInputPrimitives] mutableCopy];
    OriginalController *f=[OriginalPokegearHostFixture new];f->_condition=[NSCondition new];f->_buttons=[NSMutableDictionary new];
    f.view=[[UIView alloc] initWithFrame:CGRectMake(0,0,844,390)];
    f->_canvas=[UIImageView new];f->_fieldMenu=[UIImageView new];f->_fieldMenuShade=[UIView new];f->_menuButton=[UIButton new];f->_joystick=[OriginalJoystick new];
    for(NSNumber *key in @[@1,@2,@16,@32,@64,@128,@8,@1024,@2048]){OriginalKeyButton *b=[OriginalKeyButton new];b.tag=key.unsignedIntValue;f->_buttons[key]=b;[f.view addSubview:b];}
    [f setupPokegearControls];
    NSMutableData *main=[NSMutableData dataWithLength:256*192*4],*sub=[NSMutableData dataWithLength:256*192*4];
    for(NSUInteger engine=0;engine<2;engine++){uint8_t *pixels=(engine ? sub : main).mutableBytes;for(uint32_t y=0;y<192;y++)for(uint32_t x=0;x<256;x++){
        uint8_t *p=pixels+(y*256+x)*4;p[0]=x;p[1]=y;p[2]=engine ? 197 : 43;p[3]=255;}}
    HGNativePokegearUIState state={.active=1,.ownerGeneration=7,.revision=9,.ready=1,.ownerKind=1,.parentPhase=3,.viewActive=1,.viewKind=0,.phase=1};
    NSData *packet=[NSData dataWithBytes:&state length:sizeof(state)];f->_pokegearRawUIState=packet;
    NSDictionary *pictures=[f copyPokegearPictures:&state main:main sub:sub];[f updatePokegearControls:packet pictures:pictures paused:NO];
    BOOL pixels=CheckPCBoxCrop(f->_pokegearMain.image,0,0,256,192,43) && CheckPCBoxCrop(f->_pokegearSub.image,0,0,256,192,197);
    for(NSValue *size in @[[NSValue valueWithCGRect:CGRectMake(0,0,844,390)],[NSValue valueWithCGRect:CGRectMake(0,0,568,320)],
        [NSValue valueWithCGRect:CGRectMake(44,0,756,369)]]){
        [f layoutPokegearControls:size.CGRectValue];pixels&=CGRectContainsRect(f->_pokegearPanel.bounds,f->_pokegearMain.frame)
            && CGRectContainsRect(f->_pokegearPanel.bounds,f->_pokegearSub.frame) && !CGRectIntersectsRect(f->_pokegearMain.frame,f->_pokegearSub.frame);
        for(NSNumber *key in @[@1,@2,@16,@32,@64,@128,@1024,@2048])pixels&=!CGRectIntersectsRect(f->_pokegearPanel.frame,f->_buttons[key].frame);
    }
    for (NSNumber *phase in @[@6,@1]) {
        state.viewKind=HG_POKEGEAR_PHONE;state.parentPhase=11;state.phase=phase.unsignedIntValue;
        packet=[NSData dataWithBytes:&state length:sizeof(state)];f->_pokegearRawUIState=packet;
        pictures=[f copyPokegearPictures:&state main:main sub:sub];[f updatePokegearControls:packet pictures:pictures paused:NO];
        BOOL call=state.phase==6;
        pixels&=CheckPCBoxCrop(f->_pokegearMain.image,0,0,256,192,43)
            && CheckPCBoxCrop(f->_pokegearSub.image,0,call ? 119 : 0,256,call ? 73 : 192,197)
            && !f->_pokegearSub.userInteractionEnabled;
        HGNativePokegearUIState live=state;live.phase=call ? 1 : 6;f->_pokegearRawUIState=[NSData dataWithBytes:&live length:sizeof(live)];
        for (NSValue *size in @[[NSValue valueWithCGRect:CGRectMake(59,0,726,369)],[NSValue valueWithCGRect:CGRectMake(59,0,814,409)],
            [NSValue valueWithCGRect:CGRectMake(0,0,568,320)],[NSValue valueWithCGRect:CGRectMake(0,0,568,300)]]) {
            [f layoutPokegearControls:size.CGRectValue];
            CGRect mainRect=f->_pokegearMain.frame,subRect=f->_pokegearSub.frame;
            pixels&=CGRectContainsRect(f->_pokegearPanel.bounds,mainRect) && CGRectContainsRect(f->_pokegearPanel.bounds,subRect)
                && !CGRectIntersectsRect(mainRect,subRect) && CGRectEqualToRect(f->_pokegearSurface.frame,mainRect)
                && fabs(mainRect.size.width*0.75-mainRect.size.height)<0.001
                && fabs(subRect.size.width*(call ? 73.0/256 : 0.75)-subRect.size.height)<0.001;
            if (call) pixels&=subRect.size.width==f->_pokegearPanel.bounds.size.width && 8*subRect.size.height/73>=14
                && CGRectGetMaxY(subRect)==f->_pokegearPanel.bounds.size.height && CGRectGetMaxY(mainRect)<subRect.origin.y;
            else pixels&=subRect.origin.x>CGRectGetMaxX(mainRect);
            for (NSNumber *key in @[@1,@2,@16,@32,@64,@128,@1024,@2048])
                pixels&=!CGRectIntersectsRect(f->_pokegearPanel.frame,f->_buttons[key].frame);
        }
        f->_pokegearRawUIState=packet;
    }
    f->_interrupted=f->_interruptionEnded=YES;[f updatePokegearControls:packet pictures:pictures paused:YES];
    BOOL retire=!f->_buttons[@8].hidden && f->_buttons[@8].enabled && [f->_buttons[@8].accessibilityLabel isEqual:@"继续"]
        && f.view.subviews.lastObject==f->_buttons[@8] && !f->_pokegearSurface.userInteractionEnabled;
    HGNativePokegearUIState child=state;child.parentPhase=8;child.viewKind=HG_POKEGEAR_EASY_CHAT;child.viewActive=child.ready=0;
    NSData *childData=[NSData dataWithBytes:&child length:sizeof(child)];[f updatePokegearControls:childData pictures:nil paused:NO];
    retire&=!f->_pokegearScene && f->_pokegearPanel.hidden && f->_buttons[@1].enabled && [f->_buttons[@8].accessibilityLabel isEqual:@"开始"] && f->_buttons[@2048].hidden;
    gates[@"pokegearHostOriginalPlanesAndLayout"]=@(pixels);gates[@"pokegearHostRetirementAndResume"]=@(retire);return gates;
}
@end
#endif
