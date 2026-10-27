// SPDX-License-Identifier: GPL-3.0-or-later
#ifdef HEARTGOLD_ORIGINAL_APP_CHECK
@interface OriginalPCBoxPriorityFixture : OriginalController
@end
@implementation OriginalPCBoxPriorityFixture
- (void)viewDidLoad {}
@end

static BOOL CheckPCBoxCrop(id value,uint32_t x,uint32_t y,uint32_t width,uint32_t height,uint8_t engine) {
    if (![value isKindOfClass:UIImage.class]) return NO;
    CGImageRef image=((UIImage *)value).CGImage;
    if (!image || CGImageGetWidth(image)!=width || CGImageGetHeight(image)!=height) return NO;
    CFDataRef data=CGDataProviderCopyData(CGImageGetDataProvider(image));
    if (!data) return NO;
    size_t stride=CGImageGetBytesPerRow(image),last=(height-1)*stride+(width-1)*4;
    BOOL valid=CFDataGetLength(data)>=(CFIndex)(last+4);
    if (valid) {
        const uint8_t *bytes=CFDataGetBytePtr(data);
        valid=bytes[0]==x && bytes[1]==y && bytes[2]==engine && bytes[3]==255
            && bytes[last]==x+width-1 && bytes[last+1]==y+height-1 && bytes[last+2]==engine && bytes[last+3]==255;
        for (uint32_t row=0;valid && row<height;row++) for (uint32_t column=0;valid && column<width;column++) {
            const uint8_t *pixel=bytes+row*stride+column*4;
            valid=pixel[0]==x+column && pixel[1]==y+row && pixel[2]==engine && pixel[3]==255;
        }
    }
    CFRelease(data); return valid;
}

@implementation OriginalController (PCBoxHostChecks)
- (NSDictionary *)checkPCBoxHostPrimitives {
    OriginalController *fixture=[OriginalController new]; fixture->_condition=[NSCondition new]; fixture->_pcBoxScene=YES;
    OriginalKeyButton *button=[OriginalKeyButton new]; button.hidden=NO; button.enabled=YES; button.tag=1;
    HGNativePCBoxUIState state={.active=1,.ownerGeneration=17,.revision=29,.ready=1,.phase=HG_PC_BOX_UI_MOVE_MODE_INPUT,
        .mode=2,.activeBox=4,.selectedSlot=17,.viewActive=1};
    NSData *packet=[NSData dataWithBytes:&state length:sizeof(state)];
    fixture->_pcBoxUIState=fixture->_displayedPCBoxUIState=packet;
    NSMutableData *main=[NSMutableData dataWithLength:256*192*4],*sub=[NSMutableData dataWithLength:256*192*4];
    for (NSUInteger engine=0;engine<2;engine++) {
        uint8_t *pixels=(engine ? sub : main).mutableBytes;
        for (uint32_t y=0;y<192;y++) for (uint32_t x=0;x<256;x++) {
            uint8_t *pixel=pixels+(y*256+x)*4; pixel[0]=x; pixel[1]=y; pixel[2]=engine ? 197 : 43; pixel[3]=255;
        }
    }
    NSDictionary *pictures=[fixture copyPCBoxPictures:&state main:main sub:sub];
    NSArray *actions=pictures[@"actions"];
    BOOL crops=CheckPCBoxCrop(pictures[@"workspace"],0,0,168,168,43)
        && CheckPCBoxCrop(pictures[@"footer"],0,160,256,32,43) && actions.count==5
        && CheckPCBoxCrop(pictures[@"info"],0,0,256,192,197);
    for (NSUInteger i=0;i<actions.count;i++) crops&=CheckPCBoxCrop(actions[i],168,40+24*(uint32_t)i,88,24,43);
    BOOL pair=[pictures[@"generation"] unsignedIntValue]==state.ownerGeneration && [pictures[@"revision"] unsignedIntValue]==state.revision
        && ![pictures[@"full"] boolValue] && fixture->_pcBoxOwnerPictures==pictures;
    BOOL overlays=YES;
    for (NSUInteger i=0;i<3;i++) {
        HGNativePCBoxUIState overlay=state;
        if (!i) overlay.partyPanelOpen=1; else if (i==1) overlay.trayOpen=1; else overlay.phase=HG_PC_BOX_UI_HANDLE_YES_NO;
        NSDictionary *full=[fixture copyPCBoxPictures:&overlay main:main sub:sub];
        overlays&=[full[@"full"] boolValue] && CheckPCBoxCrop(full[@"workspace"],0,0,256,160,43)
            && CheckPCBoxCrop(full[@"footer"],0,160,256,32,43);
    }
    NSDictionary *cached=fixture->_pcBoxOwnerPictures;
    HGNativePCBoxUIState inactiveView=state; inactiveView.viewActive=inactiveView.ready=0; inactiveView.revision++;
    BOOL freeze=[fixture copyPCBoxPictures:&inactiveView main:nil sub:nil]==cached;
    HGNativePCBoxUIState retired={0};
    BOOL retire=[fixture copyPCBoxPictures:&retired main:main sub:sub]==nil && fixture->_pcBoxOwnerPictures==nil;
    BOOL invalidFrames=[fixture copyPCBoxPictures:&state main:[NSMutableData dataWithLength:4] sub:sub]==nil
        && [fixture copyPCBoxPictures:&state main:main sub:nil]==nil && !fixture->_pcBoxOwnerPictures;

    BOOL whitelist=YES;
    const uint32_t allowed[]={1,2,16,32,64,128,2048},rejected[]={0,3,4,8,256,512,1024,2049,4096,UINT32_MAX};
    for (NSUInteger i=0;i<sizeof(allowed)/sizeof(*allowed);i++) {
        [fixture clearPCBoxInputLocked]; button.tag=allowed[i]; [fixture queuePCBoxKey:button];
        whitelist&=fixture->_pcBoxChoicePending && fixture->_pcBoxChoiceKeys==allowed[i];
    }
    for (NSUInteger i=0;i<sizeof(rejected)/sizeof(*rejected);i++) {
        [fixture clearPCBoxInputLocked]; button.tag=rejected[i]; [fixture queuePCBoxKey:button];
        whitelist&=!fixture->_pcBoxChoicePending && !fixture->_pcBoxChoiceKeys;
    }
    button.tag=1; BOOL tokens=YES;
    for (NSUInteger i=0;i<2;i++) {
        [fixture clearPCBoxInputLocked]; HGNativePCBoxUIState stale=state;
        if (i) stale.revision++; else stale.ownerGeneration++;
        fixture->_pcBoxUIState=[NSData dataWithBytes:&stale length:sizeof(stale)]; [fixture queuePCBoxKey:button];
        tokens&=!fixture->_pcBoxChoicePending && !fixture->_pcBoxChoiceKeys;
    }
    fixture->_pcBoxUIState=packet; BOOL gated=YES;
    for (NSUInteger i=0;i<10;i++) {
        [fixture clearPCBoxInputLocked]; fixture->_paused=fixture->_stopping=NO; fixture->_failure=nil; fixture->_pcBoxScene=YES;
        button.hidden=NO; button.enabled=YES; HGNativePCBoxUIState busy=state;
        switch (i) {
        case 0: fixture->_paused=YES; break;
        case 1: fixture->_stopping=YES; break;
        case 2: fixture->_failure=@"synthetic"; break;
        case 3: fixture->_pcBoxScene=NO; break;
        case 4: button.hidden=YES; break;
        case 5: button.enabled=NO; break;
        case 6: fixture->_pcBoxInputCooling=YES; break;
        case 7: fixture->_pcBoxInputActive=YES; break;
        case 8: busy.ready=0; break;
        case 9: busy.viewActive=0; break;
        }
        fixture->_pcBoxUIState=[NSData dataWithBytes:&busy length:sizeof(busy)]; [fixture queuePCBoxKey:button];
        gated&=!fixture->_pcBoxChoicePending && !fixture->_pcBoxChoiceKeys;
    }
    fixture->_paused=fixture->_stopping=NO; fixture->_failure=nil; fixture->_pcBoxScene=YES;
    button.hidden=NO; button.enabled=YES; fixture->_pcBoxUIState=packet;
    [fixture clearPCBoxInputLocked]; fixture->_pcBoxChoicePending=YES; fixture->_pcBoxChoiceKeys=2;
    [fixture queuePCBoxKey:button]; gated&=fixture->_pcBoxChoicePending && fixture->_pcBoxChoiceKeys==2;
    [fixture clearPCBoxInputLocked]; [fixture queuePCBoxKey:button]; uint32_t keys=0;
    [fixture takePCBoxInputLocked:&state reads:100 keys:&keys];
    BOOL lease=keys==1 && fixture->_pcBoxInputActive && !fixture->_pcBoxChoicePending;
    [fixture takePCBoxInputLocked:&state reads:100 keys:&keys]; lease&=keys==1;
    [fixture takePCBoxInputLocked:&state reads:103 keys:&keys]; lease&=keys==1;
    [fixture takePCBoxInputLocked:&state reads:104 keys:&keys]; lease&=!keys && !fixture->_pcBoxInputActive && fixture->_pcBoxInputCooling && !fixture->_pcBoxInputKeys;
    [fixture queuePCBoxKey:button]; lease&=!fixture->_pcBoxChoicePending;
    [fixture takePCBoxInputLocked:&state reads:105 keys:&keys]; lease&=!keys && fixture->_pcBoxInputCooling;
    [fixture takePCBoxInputLocked:&state reads:106 keys:&keys]; lease&=!keys && !fixture->_pcBoxInputCooling;
    BOOL staleLease=YES;
    for (NSUInteger i=0;i<5;i++) {
        [fixture clearPCBoxInputLocked]; [fixture queuePCBoxKey:button]; [fixture takePCBoxInputLocked:&state reads:200 keys:&keys];
        staleLease&=keys==1; HGNativePCBoxUIState changed=state;
        switch (i) {
        case 0: changed.ownerGeneration++; break;
        case 1: changed.revision++; break;
        case 2: changed.phase=0; break;
        case 3: changed.viewActive=0; break;
        case 4: changed.ready=0; break;
        }
        [fixture takePCBoxInputLocked:&changed reads:201 keys:&keys];
        staleLease&=!keys && !fixture->_pcBoxInputActive && !fixture->_pcBoxInputKeys && fixture->_pcBoxInputCooling;
    }
    fixture->_pcBoxChoice=fixture->_pcBoxInput=state;
    fixture->_pcBoxChoicePending=fixture->_pcBoxInputActive=fixture->_pcBoxInputCooling=YES;
    fixture->_pcBoxChoiceKeys=fixture->_pcBoxInputKeys=fixture->_pcBoxInputReads=fixture->_pcBoxReleaseReads=2048;
    [fixture clearPCBoxInputLocked]; HGNativePCBoxUIState zero={0};
    BOOL cleanup=!fixture->_pcBoxChoicePending && !fixture->_pcBoxInputActive && !fixture->_pcBoxInputCooling
        && !fixture->_pcBoxChoiceKeys && !fixture->_pcBoxInputKeys && !fixture->_pcBoxInputReads && !fixture->_pcBoxReleaseReads
        && !memcmp(&fixture->_pcBoxChoice,&zero,sizeof(zero)) && !memcmp(&fixture->_pcBoxInput,&zero,sizeof(zero))
        && !fixture->_keys && !fixture->_pendingKeys && !fixture->_submittedKeys && !fixture->_touchPressed && !fixture->_touchX && !fixture->_touchY;
    OriginalController *priority=[OriginalPCBoxPriorityFixture new]; priority->_condition=[NSCondition new];
    priority.view=[UIView new]; priority->_pcBoxPanel=[UIView new]; priority->_summaryPanel=[UIView new]; priority->_bagPanel=[UIView new];
    priority->_buttons=[NSMutableDictionary new]; priority->_pcBoxUIState=packet; priority->_pcBoxPictures=pictures;
    HGNativeSummaryUIState startingSummary={.active=1}; HGNativeBagUIState startingBag={.active=1,.mode=1};
    BOOL childPriority=YES;
    for (NSUInteger i=0;i<2;i++) {
        priority->_summaryUIState=i ? nil : [NSData dataWithBytes:&startingSummary length:sizeof(startingSummary)];
        priority->_bagUIState=i ? [NSData dataWithBytes:&startingBag length:sizeof(startingBag)] : nil;
        priority->_pcBoxScene=YES; priority->_pcBoxPanel.hidden=NO;
        priority->_pcBoxChoicePending=priority->_pcBoxInputActive=YES; priority->_keys=1;
        [priority displayFrame];
        childPriority&=!priority->_pcBoxScene && priority->_pcBoxPanel.hidden
            && !priority->_pcBoxChoicePending && !priority->_pcBoxInputActive && !priority->_keys
            && !priority->_summaryScene && priority->_summaryPanel.hidden && !priority->_bagScene && priority->_bagPanel.hidden;
    }
    childPriority&=!priority->_game && !priority->_env && !priority->_displayLink && !priority->_bootCount;
    return @{@"pcBoxHostSourceCrops":@(crops),@"pcBoxHostPixelPairMetadata":@(pair),@"pcBoxHostOverlayFallback":@(overlays),
        @"pcBoxHostViewFreeze":@(freeze),@"pcBoxHostRetireClearsCache":@(retire),@"pcBoxHostRejectsInvalidFrames":@(invalidFrames),
        @"pcBoxHostKeyWhitelist":@(whitelist),@"pcBoxHostTokenAdmission":@(tokens),@"pcBoxHostBusyAndSuspend":@(gated && childPriority),
        @"pcBoxHostPadLease":@(lease),@"pcBoxHostStaleLeaseRelease":@(staleLease),@"pcBoxHostCleanupAndIsolation":@(cleanup && !fixture.isViewLoaded && !fixture->_game && !fixture->_env)};
}
@end
#endif
