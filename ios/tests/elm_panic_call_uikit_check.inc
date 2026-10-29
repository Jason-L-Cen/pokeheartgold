// SPDX-License-Identifier: GPL-3.0-or-later
// Opt-in attached-control story check; private requests only mark or assert observed stages.
#ifdef HEARTGOLD_ORIGINAL_ELM_PANIC_CALL_CHECK
#import <CommonCrypto/CommonDigest.h>
_Static_assert(sizeof(HGPokegearMapCardQAState)==5620 && offsetof(HGPokegearMapCardQAState,version)==5504
    && offsetof(HGNativePokegearQAState,ui)==268 && sizeof(HGPokegearMrPokemonQAState)==72,"Sealed Elm36 observer ABI");

static NSString *ElmCheckHash(NSData *data) {
    unsigned char digest[CC_SHA256_DIGEST_LENGTH]; CC_SHA256(data.bytes,(CC_LONG)data.length,digest);
    NSMutableString *value=[NSMutableString new]; for (NSUInteger i=0;i<sizeof(digest);i++) [value appendFormat:@"%02x",digest[i]];
    return value;
}
static NSDictionary *ElmCheckPrivateFiles(void) {
    NSURL *folder=[[NSFileManager.defaultManager URLsForDirectory:NSApplicationSupportDirectory inDomains:NSUserDomainMask].firstObject URLByAppendingPathComponent:@"HeartGoldOriginal" isDirectory:YES];
    NSMutableDictionary *files=[NSMutableDictionary new];
    for (NSString *suffix in @[@"",@".console",@".wifi",@".lock"]) {
        NSString *name=[@"heartgold.sav" stringByAppendingString:suffix]; NSURL *url=[folder URLByAppendingPathComponent:name];
        NSDictionary *attributes=[NSFileManager.defaultManager attributesOfItemAtPath:url.path error:nil];
        NSData *data=[attributes[NSFileType] isEqual:NSFileTypeRegular] ? [NSData dataWithContentsOfURL:url] : nil;
        if (!data) return nil; files[name]=ElmCheckHash(data);
    }
    return files;
}
static BOOL ElmCheckTarget(UIControl *control) {
    if (!control.window || control.hidden || !control.enabled || !control.userInteractionEnabled || control.alpha<0.01
        || control.bounds.size.width<44 || control.bounds.size.height<44) return NO;
    for (UIView *view=control.superview;view;view=view.superview) if (view.hidden || view.alpha<0.01 || !view.userInteractionEnabled) return NO;
    return YES;
}
static BOOL ElmCheckPixels(UIImage *image) {
    if (!image.CGImage) return NO;
    NSData *data=CFBridgingRelease(CGDataProviderCopyData(CGImageGetDataProvider(image.CGImage)));
    const uint8_t *bytes=data.bytes;
    for (NSUInteger i=4;i+4<=data.length;i+=4) if (memcmp(bytes,bytes+i,4)) return YES;
    return NO;
}
static BOOL ElmCheckSameOwner(const HGPokegearMapCardQAState *a,const HGPokegearMapCardQAState *b,
    const HGPokegearMrPokemonQAState *ma,const HGPokegearMrPokemonQAState *mb) {
    HGPokegearMapCardQAState x=*a,y=*b; HGPokegearMrPokemonQAState mx=*ma,my=*mb;
    // Observer tick counters are not original UI owner tokens; all semantic facts must still match.
    x.revision=y.revision=x.phone.revision=y.phone.revision=x.phone.frame=y.phone.frame=x.phone.snapshotSerial=y.phone.snapshotSerial=0;
    mx.revision=my.revision=0;
    return !memcmp(&x,&y,sizeof(x)) && !memcmp(&mx,&my,sizeof(mx));
}

@implementation OriginalController (ElmPanicCallUIKitCheck)
- (BOOL)readElmQAOwner:(HGPokegearMapCardQAState *)qa mr:(HGPokegearMrPokemonQAState *)mr {
    uint32_t address=w2c_hgengine_HGNativePokegearQAGetState(_game),other=w2c_hgengine_HGNativeMrPokemonQAGetState(_game);
    if (!address || address>_env->memory->size || sizeof(*qa)>_env->memory->size-address
        || !other || other>_env->memory->size || sizeof(*mr)>_env->memory->size-other) wasm_rt_trap(WASM_RT_TRAP_OOB);
    memcpy(qa,_env->memory->data+address,sizeof(*qa)); memcpy(mr,_env->memory->data+other,sizeof(*mr));
    if (qa->version!=HG_PG_MAP_VERSION || qa->byteSize!=sizeof(*qa) || qa->phone.version!=HG_PG_QA_VERSION
        || qa->phone.byteSize!=sizeof(qa->phone) || mr->version!=HG_PG_MR_VERSION || mr->byteSize!=sizeof(*mr)
        || w2c_hgengine_HGNativePokegearQAGetState(_game)!=address || w2c_hgengine_HGNativeMrPokemonQAGetState(_game)!=other
        || memcmp(qa,_env->memory->data+address,sizeof(*qa)) || memcmp(mr,_env->memory->data+other,sizeof(*mr))) wasm_rt_trap(WASM_RT_TRAP_OOB);
    return !qa->error && !qa->phone.error && !mr->error;
}
- (void)clearElmCheckTransientLocked:(BOOL)interrupted {
    _elmRequestPending=NO; _elmRequestExpected=_elmRequestMrExpected=nil;
    if (interrupted) _elmInputError=1;
    _elmInputPhase=0;
}
- (BOOL)queueElmQARequest:(uint32_t)kind {
    if (kind<HG_PG_MAP_START || kind>HG_PG_ELM_FINISH || _displayedElmQAState.length!=sizeof(HGPokegearMapCardQAState)
        || _displayedElmMrState.length!=sizeof(HGPokegearMrPokemonQAState)) return NO;
    [_condition lock];
    BOOL idle=!_paused && !_stopping && !_failure && !_elmRequestPending && !_elmInputPhase;
    if (idle) {_elmRequestExpected=_displayedElmQAState;_elmRequestMrExpected=_displayedElmMrState;
        _elmRequestKind=kind;_elmRequestPending=YES;_elmRequestSerial++;}
    [_condition unlock];
    if (idle) _elmCheckRuntime[@"requestSerial"]=@(_elmRequestSerial);
    return idle;
}
- (void)processElmQARequestOwnerLocked:(const HGPokegearMapCardQAState *)qa mr:(const HGPokegearMrPokemonQAState *)mr idle:(BOOL)idle {
    if (!_elmRequestPending) return;
    const HGPokegearMapCardQAState *expected=_elmRequestExpected.bytes;
    const HGPokegearMrPokemonQAState *expectedMr=_elmRequestMrExpected.bytes;
    BOOL allowed=idle && !_paused && !_stopping && !_failure && !_elmInputPhase
        && _elmRequestExpected.length==sizeof(*expected) && _elmRequestMrExpected.length==sizeof(*expectedMr)
        && ElmCheckSameOwner(expected,qa,expectedMr,mr) && _elmRequestKind>=HG_PG_MAP_START && _elmRequestKind<=HG_PG_ELM_FINISH;
    _elmRequestAccepted=allowed && w2c_hgengine_HGNativePokegearQARequest(_game,qa->revision,_elmRequestKind);
    _elmRequestCompleted=_elmRequestSerial;_elmRequestPending=NO;_elmRequestExpected=_elmRequestMrExpected=nil;
}
- (void)observeElmInputOwnerLocked:(uint32_t)reads keys:(uint32_t)keys x:(uint32_t)x y:(uint32_t)y pressed:(BOOL)pressed {
    if (!_elmInputPhase || _elmInputError) return;
    BOOL match=_elmInputTouch ? pressed && !keys : keys==_elmInputKey && !pressed;
    if (_elmInputPhase==1 && match) {_elmInputPhase=2;_elmInputReads=reads;_elmInputX=x;_elmInputY=y;}
    if (_elmInputPhase==2) {
        if (!keys && !pressed) {
            _elmInputHeld=reads-_elmInputReads;_elmInputReleaseReads=reads;_elmInputPhase=3;
            if (!_elmInputHeld || _elmInputHeld>(_elmInputTouch ? 12u : 4u)) _elmInputError=2;
        } else if (!match || reads-_elmInputReads>(_elmInputTouch ? 12u : 4u)) _elmInputError=3;
    }
    if (_elmInputPhase==3) {
        if (keys || pressed) _elmInputError=4;
        else if (reads-_elmInputReleaseReads>=2) {_elmInputSettledReads=reads;_elmInputPhase=4;}
    }
}
- (BOOL)elmCheckPacket:(HGPokegearMapCardQAState *)qa mr:(HGPokegearMrPokemonQAState *)mr {
    if (_displayedElmQAState.length!=sizeof(*qa) || _displayedElmMrState.length!=sizeof(*mr)) return NO;
    memcpy(qa,_displayedElmQAState.bytes,sizeof(*qa));memcpy(mr,_displayedElmMrState.bytes,sizeof(*mr));
    if (qa->error || qa->phone.error || mr->error || !_elmDisplayedPublishedFrame) return NO;
    return _pokegearRawUIState.length==sizeof(qa->phone.ui)
        && !memcmp(_pokegearRawUIState.bytes,&qa->phone.ui,sizeof(qa->phone.ui))
        && (!qa->phone.ui.active || (_displayedPokegearUIState.length==sizeof(qa->phone.ui)
            && !memcmp(_displayedPokegearUIState.bytes,&qa->phone.ui,sizeof(qa->phone.ui))));
}
- (BOOL)elmAdmitControl:(UIControl *)control qa:(const HGPokegearMapCardQAState *)qa mr:(const HGPokegearMrPokemonQAState *)mr key:(uint32_t)key touch:(BOOL)touch {
    if (!ElmCheckTarget(control)) return NO;
    [_condition lock];
    const HGPokegearMapCardQAState *live=_elmOwnerQAState.bytes;
    const HGPokegearMrPokemonQAState *liveMr=_elmOwnerMrState.bytes;
    BOOL allowed=!_paused && !_stopping && !_failure && !_elmInputPhase && !_elmRequestPending
        && _elmOwnerQAState.length==sizeof(*live) && _elmOwnerMrState.length==sizeof(*liveMr)
        && ElmCheckSameOwner(qa,live,mr,liveMr);
    if (allowed) {_elmInputPhase=1;_elmInputError=0;_elmInputKey=key;_elmInputTouch=touch;_elmInputHeld=0;}
    [_condition unlock];
    if (allowed) _elmCheckRuntime[@"pendingInput"]=[@{@"stage":_elmCheckRuntime[@"stage"],@"key":@(key),@"touch":@(touch),
        @"displayedFrame":@(_elmDisplayedPublishedFrame),@"ownerFrame":@(_elmDisplayedOwnerFrame),@"padReads":@(_elmDisplayedPadReads),
        @"generation":@(qa->phone.ui.ownerGeneration),@"revision":@(qa->phone.ui.revision),@"script":@(qa->scriptId),
        @"serial":@(qa->scriptSerial),@"callSerial":@(qa->phone.callTextSerial),@"phase":@(qa->phone.ui.phase),
        @"mapRevision":@(qa->revision),@"map":@(qa->mapId),@"worldX":@(qa->x),@"worldZ":@(qa->z),
        @"boot":@{@"kind":@(qa->phone.boot.kind),@"generation":@(qa->phone.boot.ownerGeneration),@"revision":@(qa->phone.boot.revision),
            @"phase":@(qa->phone.boot.phase),@"inputMode":@(qa->phone.boot.inputMode),@"exit":@(qa->phone.boot.exit)}} mutableCopy];
    if (allowed && _displayedStartMenuUIState.length==sizeof(HGNativeFieldMenuUIState)) {
        const HGNativeFieldMenuUIState *menu=_displayedStartMenuUIState.bytes;
        _elmCheckRuntime[@"pendingInput"][@"menu"]=@{@"generation":@(menu->ownerGeneration),@"revision":@(menu->revision),@"phase":@(menu->phase),@"active":@(menu->active)};
    }
    return allowed;
}
- (BOOL)elmSendKey:(uint32_t)key qa:(const HGPokegearMapCardQAState *)qa mr:(const HGPokegearMrPokemonQAState *)mr {
    OriginalKeyButton *button=_buttons[@(key)];
    if (![self elmAdmitControl:button qa:qa mr:mr key:key touch:NO]) return NO;
    [button sendActionsForControlEvents:UIControlEventTouchDown];[button sendActionsForControlEvents:UIControlEventTouchUpInside];return YES;
}
- (BOOL)elmSendDirection:(uint32_t)key qa:(const HGPokegearMapCardQAState *)qa mr:(const HGPokegearMrPokemonQAState *)mr {
    if (!HGMapStationary(qa) || qa->scriptActive || qa->battleLive || qa->phone.ui.active || _fieldMenuOpen
        || ![self elmAdmitControl:_joystick qa:qa mr:mr key:key touch:NO]) return NO;
    CGPoint center=CGPointMake(CGRectGetMidX(_joystick.bounds),CGRectGetMidY(_joystick.bounds));
    CGFloat travel=MAX(1,MIN(_joystick.bounds.size.width,_joystick.bounds.size.height)/2-24);
    CGPoint point=CGPointMake(center.x+(key==16 ? travel : key==32 ? -travel : 0),center.y+(key==128 ? travel : key==64 ? -travel : 0));
    _elmCheckRuntime[@"move"]=@{@"x":@(qa->x),@"z":@(qa->z),@"map":@(qa->mapId),@"key":@(key),@"stage":_elmCheckRuntime[@"stage"]};
    [_joystick checkPCBoxEventAt:point pressed:YES];[_joystick checkPCBoxEventAt:point pressed:NO];return YES;
}
- (BOOL)elmSendPhoneTouch:(CGPoint)point qa:(const HGPokegearMapCardQAState *)qa mr:(const HGPokegearMrPokemonQAState *)mr {
    if (!HGNativePokegearUITouchStartAllowed(&qa->phone.ui,qa->phone.ui.ownerGeneration,qa->phone.ui.revision,point.x,point.y)
        || ![self elmAdmitControl:_pokegearSurface qa:qa mr:mr key:0 touch:YES]) return NO;
    CGPoint local=CGPointMake(point.x*_pokegearSurface.bounds.size.width/256,point.y*_pokegearSurface.bounds.size.height/192);
    [_pokegearSurface checkPCBoxEventAt:local pressed:YES];[_pokegearSurface checkPCBoxEventAt:local pressed:NO];return YES;
}
- (BOOL)elmSendText:(const HGPokegearMapCardQAState *)qa mr:(const HGPokegearMrPokemonQAState *)mr {
    NSString *token=[NSString stringWithFormat:@"%u:%u:%u",qa->mapId,qa->scriptId,qa->scriptSerial];
    if ([_elmCheckRuntime[@"textToken"] isEqual:token] || !HGMrTextQualified(qa,mr,qa->scriptId,qa->scriptSerial)) return NO;
    if (![self elmSendKey:1 qa:qa mr:mr]) return NO;_elmCheckRuntime[@"textToken"]=token;return YES;
}
- (void)elmAdvance:(uint32_t)stage {
    _elmCheckRuntime[@"stage"]=@(stage);_elmCheckRuntime[@"segment"]=@0;
    _elmCheckRuntime[@"advanced"]=@(NSDate.timeIntervalSinceReferenceDate);[_elmCheckRuntime removeObjectForKey:@"requestSerial"];
}
- (BOOL)elmDriveRoute:(const HGPokegearMapRouteSegment *)route count:(NSUInteger)count qa:(const HGPokegearMapCardQAState *)qa mr:(const HGPokegearMrPokemonQAState *)mr {
    NSUInteger segment=[_elmCheckRuntime[@"segment"] unsignedIntegerValue];
    while (segment<count && qa->x==route[segment].x1 && qa->z==route[segment].z1) segment++;
    _elmCheckRuntime[@"segment"]=@(segment);if (segment==count) return YES;
    if (!HGMapStationary(qa) || qa->scriptActive || qa->battleLive) return NO;
    uint32_t key=HGMapRouteKey(&route[segment],qa->x,qa->z);
    if (!key) {[self finishElmUIKitCheck:@"route_membership_failed"];return NO;}
    [self elmSendDirection:key qa:qa mr:mr];return NO;
}
- (BOOL)elmCapture:(NSString *)name {
    if (_elmCheckRuntime[@"capturesByName"][name]) return YES;
    BOOL source=_pokegearScene ? ElmCheckPixels(_pokegearMain.image) && ElmCheckPixels(_pokegearSub.image) : ElmCheckPixels(_canvas.image);
    if (!source || self.view.bounds.size.width<=self.view.bounds.size.height) return NO;
    [self.view layoutIfNeeded];UIGraphicsBeginImageContextWithOptions(self.view.bounds.size,NO,0);
    BOOL drawn=[self.view drawViewHierarchyInRect:self.view.bounds afterScreenUpdates:YES];
    UIImage *image=UIGraphicsGetImageFromCurrentImageContext();UIGraphicsEndImageContext();NSData *png=image ? UIImagePNGRepresentation(image) : nil;
    NSURL *folder=[NSFileManager.defaultManager URLsForDirectory:NSDocumentDirectory inDomains:NSUserDomainMask].firstObject;
    NSString *file=[@"elm-uikit-" stringByAppendingString:[name stringByAppendingString:@".png"]];
    if (!drawn || !png || ![png writeToURL:[folder URLByAppendingPathComponent:file] options:NSDataWritingAtomic error:nil]) return NO;
    NSDictionary *capture=@{@"file":file,@"displayedFrame":@(_elmDisplayedPublishedFrame),@"ownerFrame":@(_elmDisplayedOwnerFrame),
        @"padReads":@(_elmDisplayedPadReads),@"sha256":ElmCheckHash(png),@"originalPixels":@YES};
    _elmCheckRuntime[@"capturesByName"][name]=capture;[_elmCheckRuntime[@"captures"] addObject:capture];return YES;
}
- (void)beginElmUIKitCheck {
    NSURL *url=[NSBundle.mainBundle URLForResource:@"elm-panic-call-binding" withExtension:@"json"];
    NSData *bytes=url ? [NSData dataWithContentsOfURL:url] : nil;
    id decoded=bytes ? [NSJSONSerialization JSONObjectWithData:bytes options:0 error:nil] : nil;
    NSDictionary *binding=[decoded isKindOfClass:NSDictionary.class] ? decoded : nil;
    NSTimeInterval now=NSDate.timeIntervalSinceReferenceDate;
    _elmCheckRuntime=[@{@"stage":@0,@"segment":@0,@"started":@(now),@"advanced":@(now),@"actions":[NSMutableArray new],
        @"captures":[NSMutableArray new],@"capturesByName":[NSMutableDictionary new],@"manualTiles":@0,@"binding":binding ?: @{}} mutableCopy];
    NSDictionary *files=ElmCheckPrivateFiles();if (files) _elmCheckRuntime[@"privateFilesBefore"]=files;
    BOOL bound=bytes && [ElmCheckHash(bytes) isEqual:@HG_ELM_BINDING_SHA256] && [binding[@"case"] isEqual:@"elm-panic-call"]
        && [binding[@"version"] unsignedIntValue]==1 && [binding[@"qaBytes"] unsignedIntValue]==5620 && [binding[@"mrBytes"] unsignedIntValue]==72
        && [binding[@"nativeReportSha256"] isEqual:@HG_ELM_NATIVE_REPORT_SHA256] && [binding[@"moduleSha256"] isEqual:@HG_ELM_NATIVE_MODULE_SHA256]
        && [binding[@"generatedHeaderSha256"] isEqual:@HG_ELM_NATIVE_HEADER_SHA256] && [binding[@"qaHeaderSha256"] isEqual:@HG_ELM_QA_HEADER_SHA256]
        && [[NSBundle.mainBundle objectForInfoDictionaryKey:@"HGPrivateCheckCase"] isEqual:@"elm-panic-call"]
        && [[NSBundle.mainBundle objectForInfoDictionaryKey:@"HGPrivateAudioValidation"] isEqual:@"skipped_no_audio_device"]
        && [NSProcessInfo.processInfo.environment[@"HEARTGOLD_CHECK_CASE"] isEqual:@"elm-panic-call"]
        && [NSProcessInfo.processInfo.environment[@"HEARTGOLD_TEST_MUTE"] isEqual:@"1"];
    if (!bound || files.count!=4) [self finishElmUIKitCheck:@"binding_or_private_fixture_failed"];
}
- (NSDictionary *)elmCaptureDiagnostic {
    HGPokegearMapCardQAState qa={0};HGNativeBattleUIState battle={0};
    BOOL haveQA=_displayedElmQAState.length==sizeof(qa),haveBattle=_displayedBattleUIState.length==sizeof(battle);
    if (haveQA) memcpy(&qa,_displayedElmQAState.bytes,sizeof(qa));
    if (haveBattle) memcpy(&battle,_displayedBattleUIState.bytes,sizeof(battle));
    NSDictionary *map=haveQA ? @{@"available":@YES,@"revision":@(qa.revision),@"error":@(qa.error),@"phoneError":@(qa.phone.error),
        @"mapId":@(qa.mapId),@"x":@(qa.x),@"z":@(qa.z),@"fieldReady":@(qa.phone.fieldReady),@"betweenSteps":@(qa.betweenSteps),
        @"moveState":@(qa.moveState),@"playerMoveState":@(qa.playerMoveState),@"scriptActive":@(qa.scriptActive),
        @"scriptId":@(qa.scriptId),@"scriptWait":@(qa.scriptWait),@"battleLive":@(qa.battleLive),
        @"battleLaunches":@(qa.battleLaunches),@"battleReturns":@(qa.battleReturns)} : @{@"available":@NO};
    NSDictionary *battleUI=haveBattle ? @{@"available":@YES,@"active":@(battle.active),@"mode":@(battle.mode),
        @"ready":@(battle.ready),@"revision":@(battle.revision),@"enabledMask":@(battle.enabledMask)} : @{@"available":@NO};
    UIImageView *view=_pokegearScene && _pokegearMain.image ? _pokegearMain : _canvas;
    UIImage *image=view.image;size_t width=image.CGImage ? CGImageGetWidth(image.CGImage) : 0,height=image.CGImage ? CGImageGetHeight(image.CGImage) : 0;
    BOOL bounded=width && height && width<=8192 && height<=8192 && (uint64_t)width*height<=16*1024*1024;
    NSData *png=bounded ? UIImagePNGRepresentation(image) : nil;
    NSURL *folder=[NSFileManager.defaultManager URLsForDirectory:NSDocumentDirectory inDomains:NSUserDomainMask].firstObject;
    NSString *file=@"elm-uikit-diagnostic.png";
    BOOL written=png.length && png.length<=32*1024*1024 && [png writeToURL:[folder URLByAppendingPathComponent:file] options:NSDataWritingAtomic error:nil];
    NSDictionary *picture=@{@"file":file,@"written":written ? @YES : @NO,@"source":view==_pokegearMain ? @"_pokegearMain.image" : @"_canvas.image",
        @"viewHidden":@(view.hidden),@"width":@(width),@"height":@(height),@"sha256":written ? ElmCheckHash(png) : (id)NSNull.null};
    return @{@"provenance":@"Copied last displayed QA/Battle UI packets and existing UIImage on main; no owner-thread getter or new render.",
        @"stage":_elmCheckRuntime[@"stage"] ?: @0,@"pendingInput":_elmCheckRuntime[@"pendingInput"] ?: (id)NSNull.null,
        @"pendingMove":_elmCheckRuntime[@"move"] ?: (id)NSNull.null,@"publishedFrame":@(_elmDisplayedPublishedFrame),
        @"ownerFrame":@(_elmDisplayedOwnerFrame),@"padReads":@(_elmDisplayedPadReads),@"map":map,@"battleUI":battleUI,
        @"runControl":@{@"attached":_battleButtons[3].window ? @YES : @NO,@"hidden":@(_battleButtons[3].hidden),
            @"enabled":@(_battleButtons[3].enabled),@"admitted":ElmCheckTarget(_battleButtons[3]) ? @YES : @NO},
        @"image":picture};
}
- (void)finishElmUIKitCheck:(NSString *)status {
    NSMutableDictionary *run=_elmCheckRuntime;if (!run || run[@"status"]) return;run[@"status"]=status;
    run[@"diagnostic"]=[self elmCaptureDiagnostic];
    [_condition lock];[self clearElmCheckTransientLocked:NO];[_condition unlock];[self stop];
    __block NSUInteger polls=0;__block void (^wait)(void);
    wait=^{
        [self->_condition lock];BOOL finished=self->_finished,quiescent=self->_resetQuiesced;[self->_condition unlock];
        if (!finished && ++polls<400) {dispatch_after(dispatch_time(DISPATCH_TIME_NOW,50*NSEC_PER_MSEC),dispatch_get_main_queue(),wait);return;}
        NSDictionary *before=run[@"privateFilesBefore"],*after=ElmCheckPrivateFiles();BOOL preserved=before.count==4 && [before isEqualToDictionary:after];
        BOOL captures=YES;for (NSString *name in @[@"seed",@"map-card",@"map-view",@"apricorn",@"mr-indoor",@"elm-call",@"hangup",@"field-return"]) captures&=run[@"capturesByName"][name]!=nil;
        BOOL passed=[status isEqual:@"passed_original_elm_panic_call_uikit"] && finished && quiescent && preserved && captures
            && [run[@"diagnostic"][@"image"][@"written"] boolValue];
        NSDictionary *report=@{@"case":@"elm-panic-call",@"passed":@(passed),@"complete":@(passed),@"status":status,@"binding":run[@"binding"],
            @"actions":run[@"actions"],@"captures":run[@"captures"],@"manualTiles":run[@"manualTiles"],@"terminal":run[@"terminal"] ?: @{},
            @"mrCheckpoint":run[@"mrCheckpoint"] ?: @{},@"callFacts":run[@"callFacts"] ?: @{},@"diagnostic":run[@"diagnostic"] ?: @{},
            @"normalReset":@(finished && quiescent),@"privateFourFilesUnchanged":@(preserved),@"privateFilesBefore":before ?: @{},@"privateFilesAfter":after ?: @{},
            @"physicalGestures":@NO,@"attachedUIKitCallbacks":@YES,@"productionApiAdded":@NO,@"audioDeviceSuppressed":@YES,@"audioValidation":@"untested",
            @"firstFailure":run[@"firstFailure"] ?: NSNull.null};
        NSURL *folder=[NSFileManager.defaultManager URLsForDirectory:NSDocumentDirectory inDomains:NSUserDomainMask].firstObject;
        NSData *json=[NSJSONSerialization dataWithJSONObject:report options:NSJSONWritingPrettyPrinted error:nil];
        [json writeToURL:[folder URLByAppendingPathComponent:@"elm-panic-call-uikit-report.json"] options:NSDataWritingAtomic error:nil];wait=nil;
    };wait();
}
- (void)checkElmUIKitDisplay {
    NSMutableDictionary *run=_elmCheckRuntime;if (!run || run[@"status"]) return;
    NSTimeInterval now=NSDate.timeIntervalSinceReferenceDate;
    if (now-[run[@"started"] doubleValue]>900 || now-[run[@"advanced"] doubleValue]>90) {[self finishElmUIKitCheck:@"bounded_timeout"];return;}
    [_condition lock];NSString *failure=_failure;uint32_t phase=_elmInputPhase,error=_elmInputError,held=_elmInputHeld;
    uint32_t pressReads=_elmInputReads,releaseReads=_elmInputReleaseReads,settledReads=_elmInputSettledReads,x=_elmInputX,y=_elmInputY;
    uint32_t completed=_elmRequestCompleted;BOOL accepted=_elmRequestAccepted;[_condition unlock];
    if (failure || error) {run[@"firstFailure"]=failure ?: [NSString stringWithFormat:@"input_observation_%u",error];[self finishElmUIKitCheck:@"engine_or_input_failed"];return;}
    if (run[@"pendingInput"]) {
        if (phase!=4) return;NSMutableDictionary *event=run[@"pendingInput"];event[@"pressPadReads"]=@(pressReads);event[@"releasePadReads"]=@(releaseReads);
        event[@"heldPadReads"]=@(held);event[@"settledAtPadReads"]=@(settledReads);event[@"settledPadReads"]=@(settledReads-releaseReads);event[@"x"]=@(x);event[@"y"]=@(y);[run[@"actions"] addObject:event];
        [run removeObjectForKey:@"pendingInput"];[_condition lock];_elmInputPhase=0;[_condition unlock];run[@"advanced"]=@(now);return;
    }
    if (run[@"requestSerial"] && completed==[run[@"requestSerial"] unsignedIntValue] && !accepted) {[self finishElmUIKitCheck:@"observer_marker_rejected"];return;}
    HGPokegearMapCardQAState qa;HGPokegearMrPokemonQAState mr;if (![self elmCheckPacket:&qa mr:&mr]) return;
    uint32_t frame=_elmDisplayedPublishedFrame;if ([run[@"lastFrame"] unsignedIntValue]==frame) return;run[@"lastFrame"]=@(frame);
    uint32_t stage=[run[@"stage"] unsignedIntValue];
    // A move-triggered encounter must return through the original battle before its pending step is settled.
    if (qa.battleLive) {
        const HGNativeBattleUIState *battle=_displayedBattleUIState.length==sizeof(*battle) ? _displayedBattleUIState.bytes : NULL;
        if ([run[@"battle"] unsignedIntValue]!=qa.battleLaunches) {run[@"battle"]=@(qa.battleLaunches);run[@"runs"]=@0;run[@"runRevision"]=@0;}
        uint32_t attempts=[run[@"runs"] unsignedIntValue];
        if (HGMapRunReady(battle,attempts,[run[@"runRevision"] unsignedIntValue]) && ElmCheckTarget(_battleButtons[3])) {
            [_battleButtons[3] sendActionsForControlEvents:UIControlEventTouchUpInside];run[@"runs"]=@(attempts+1);run[@"runRevision"]=@(battle->revision);run[@"advanced"]=@(now);
            [run[@"actions"] addObject:@{@"stage":@(stage),@"battle":@(qa.battleLaunches),@"runRevision":@(battle->revision),@"displayedFrame":@(frame)}];
        } else if (attempts>=HG_PG_MAP_RUN_ATTEMPT_CAP && battle && battle->ready && battle->revision!=[run[@"runRevision"] unsignedIntValue]) [self finishElmUIKitCheck:@"wild_run_limit"];
        return;
    }
    NSDictionary *move=run[@"move"];
    if (move && (HGMapStationary(&qa) || qa.scriptActive || qa.mapId!=[move[@"map"] unsignedIntValue])) {
        uint32_t px=[move[@"x"] unsignedIntValue],pz=[move[@"z"] unsignedIntValue],oldMap=[move[@"map"] unsignedIntValue];
        uint32_t moveStage=[move[@"stage"] unsignedIntValue];BOOL countedStage=moveStage==13 || moveStage==15;
        BOOL changed=qa.mapId!=oldMap || qa.x!=px || qa.z!=pz;
        if ((qa.mapId==oldMap || countedStage) && !HGMapStepValid([move[@"key"] unsignedIntValue],px,pz,qa.x,qa.z)) {[self finishElmUIKitCheck:@"original_step_failed"];return;}
        if (!changed && [run[@"blocked"] unsignedIntValue]>=4) {[self finishElmUIKitCheck:@"blocked_step_limit"];return;}
        run[@"blocked"]=changed ? @0 : @([run[@"blocked"] unsignedIntValue]+1);
        if ((qa.x!=px || qa.z!=pz) && countedStage) run[@"manualTiles"]=@([run[@"manualTiles"] unsignedIntValue]+1);
        [run removeObjectForKey:@"move"];
    } else if (move) return;
    switch (stage) {
    case 0: {
        const HGYNBootFacts *b=&qa.phone.boot;
        if (qa.phone.fieldReady) {
            if (![run[@"continueGeneration"] unsignedIntValue] || b->kind!=HG_YN_BOOT_MAIN_MENU || b->active || b->ready || b->phase!=8 || b->exit!=1
                || b->ownerGeneration!=[run[@"continueGeneration"] unsignedIntValue]) {[self finishElmUIKitCheck:@"normal_continue_exit_missing"];return;}
            if (![self elmCapture:@"seed"]) return;[self elmAdvance:1];return;
        }
        if (!run[@"bootStart"] && frame>=450 && [self elmSendKey:8 qa:&qa mr:&mr]) {run[@"bootStart"]=@YES;return;}
        BOOL title=b->kind==HG_YN_BOOT_TITLE;
        BOOL eligible=b->active==1 && b->ready==1 && b->ownerGeneration && b->revision && !b->exit
            && (title ? b->phase==2 && !b->inputMode && !b->option && !b->notice && (b->progressFlags&15)==15
                : b->kind==HG_YN_BOOT_MAIN_MENU && b->phase==5 && b->inputMode<=1 && b->option<9 && b->notice==15 && (b->progressFlags&255)==255);
        NSString *token=[NSString stringWithFormat:@"%u:%u:%u",b->kind,b->ownerGeneration,b->revision];
        if (eligible && ![run[@"bootToken"] isEqual:token] && [self elmSendKey:1 qa:&qa mr:&mr]) {
            run[@"bootToken"]=token;if (!title && !b->inputMode) run[@"continueGeneration"]=@(b->ownerGeneration);
        }return;
    }
    case 1: if (qa.started) {[self elmAdvance:2];return;}if (!run[@"requestSerial"]) [self queueElmQARequest:HG_PG_MAP_START];return;
    case 2: if (qa.scriptActive && qa.mapId==67 && qa.scriptId==2 && !qa.scene) {[self elmAdvance:3];return;}
        [self elmDriveRoute:hgMapGuideRoute count:sizeof(hgMapGuideRoute)/sizeof(*hgMapGuideRoute) qa:&qa mr:&mr];return;
    case 3: if (qa.mapId==67 && qa.x==557 && qa.z==404 && qa.scene==1 && !qa.cards && HGMapStationary(&qa) && !qa.scriptActive) {[self elmAdvance:4];return;}
        [self elmSendText:&qa mr:&mr];return;
    case 4: if (qa.scriptActive && qa.mapId==67 && qa.scriptId==3 && qa.scene==1) {[self elmAdvance:5];return;}
        [self elmDriveRoute:hgMapCardRoute count:sizeof(hgMapCardRoute)/sizeof(*hgMapCardRoute) qa:&qa mr:&mr];return;
    case 5: if (qa.mapId==67 && qa.x==550 && qa.z==385 && qa.scene==2 && qa.cards==1 && HGMapStationary(&qa) && !qa.scriptActive) {
            if (![self elmCapture:@"map-card"]) return;[self elmAdvance:6];return;}
        [self elmSendText:&qa mr:&mr];return;
    case 6: if (ElmCheckTarget(_menuButton) && HGMapStationary(&qa) && !qa.phone.ui.active) {[_menuButton sendActionsForControlEvents:UIControlEventTouchUpInside];[self elmAdvance:7];}return;
    case 7: {
        if (qa.phone.ui.active) {[self elmAdvance:8];return;}
        NSUInteger slot=qa.phone.pokegearSlot;
        if (_fieldMenu.hidden || !_fieldMenu.window || !_fieldMenu.userInteractionEnabled || slot>=7 || slot>=_fieldMenuElements.count
            || ((qa.phone.menuIcons>>(slot*4))&15)!=3 || ![_fieldMenu.accessibilityElements containsObject:_fieldMenuElements[slot]]) return;
        if ([self elmAdmitControl:_menuButton qa:&qa mr:&mr key:0 touch:YES]) [_fieldMenuElements[slot] accessibilityActivate];return;
    }
    case 8: if (HGElmPhoneReady(&qa.phone,1) && [self elmSendPhoneTouch:CGPointMake(128,176) qa:&qa mr:&mr]) [self elmAdvance:9];return;
    case 9: if (qa.phone.ui.app==2 && qa.phone.ui.viewKind==HG_POKEGEAR_MAP && qa.phone.ui.phase==1
            && HGNativePokegearUIInputAllowed(&qa.phone.ui,qa.phone.ui.ownerGeneration,qa.phone.ui.revision)) {
            if (![self elmCapture:@"map-view"]) return;[self elmAdvance:10];}return;
    case 10: if (!qa.phone.ui.active) {[self elmAdvance:11];return;}
        if (HGNativePokegearUIInputAllowed(&qa.phone.ui,qa.phone.ui.ownerGeneration,qa.phone.ui.revision)) {
            if ([run[@"mapCloses"] unsignedIntValue]>=3) {[self finishElmUIKitCheck:@"map_close_limit"];return;}
            if ([self elmSendKey:2 qa:&qa mr:&mr]) run[@"mapCloses"]=@([run[@"mapCloses"] unsignedIntValue]+1);
        }return;
    case 11: if (qa.phone.ui.active) return;
        if (_startMenuScene) {if ([run[@"menuClose"] boolValue]) return;if ([self elmSendKey:2 qa:&qa mr:&mr]) run[@"menuClose"]=@YES;return;}
        if (_fieldMenuOpen) {if (ElmCheckTarget(_menuButton)) [_menuButton sendActionsForControlEvents:UIControlEventTouchUpInside];return;}
        if (!HGMapStationary(&qa)) return;if (!qa.done) {if (!run[@"requestSerial"]) [self queueElmQARequest:HG_PG_MAP_FINISH];return;}[self elmAdvance:12];return;
    case 12: if (mr.started) {[self elmAdvance:13];return;}if (!run[@"requestSerial"]) [self queueElmQARequest:HG_PG_MR_START];return;
    case 13: if (qa.scriptActive && qa.mapId==34 && qa.scriptId==3 && !mr.routeScene) {[self elmAdvance:14];return;}
        [self elmDriveRoute:hgMrApricornRoute count:sizeof(hgMrApricornRoute)/sizeof(*hgMrApricornRoute) qa:&qa mr:&mr];return;
    case 14: if (qa.mapId==34 && qa.x==556 && qa.z==356 && HGMapStationary(&qa) && !qa.scriptActive && mr.routeScene==1
            && mr.apricornBox==1 && mr.apricornScriptSeen && mr.apricornCompletedSeen) {
            if (![self elmCapture:@"apricorn"]) return;[self elmAdvance:15];return;}[self elmSendText:&qa mr:&mr];return;
    case 15: if ([self elmDriveRoute:hgMrHouseRoute count:sizeof(hgMrHouseRoute)/sizeof(*hgMrHouseRoute) qa:&qa mr:&mr]) [self elmAdvance:16];return;
    case 16: if (qa.mapId==143 && mr.doorAnchorSeen) {[self elmAdvance:17];return;}
        [self elmDriveRoute:hgMrDoorRoute count:sizeof(hgMrDoorRoute)/sizeof(*hgMrDoorRoute) qa:&qa mr:&mr];return;
    case 17: if (HGMrEndpoint(&qa,&mr)) {
            if ([run[@"manualTiles"] unsignedIntValue]!=124) {[self finishElmUIKitCheck:@"mr_manual_tiles_failed"];return;}
            run[@"mrCheckpoint"]=@{@"map":@(qa.mapId),@"x":@(qa.x),@"z":@(qa.z),@"facing":@(qa.facing),@"routeScene":@(mr.routeScene),
                @"houseScene":@(mr.houseScene),@"mysteryEgg":@(mr.mysteryEgg),@"pokedex":@(mr.pokedexEnabled),@"oakContact":@(mr.oakRegistered),@"healed":@(mr.partyHealed),@"manualTiles":run[@"manualTiles"]};
            if (![self elmCapture:@"mr-indoor"]) return;[self elmAdvance:18];return;}[self elmSendText:&qa mr:&mr];return;
    case 18: if (mr.done) {[self elmAdvance:19];return;}if (!run[@"requestSerial"]) [self queueElmQARequest:HG_PG_MR_FINISH];return;
    case 19: if ((qa.phone.coverage&3)==3) {[self elmAdvance:20];return;}if (!run[@"requestSerial"]) [self queueElmQARequest:HG_PG_ELM_START];return;
    case 20: if (qa.mapId==34) {[self elmAdvance:22];return;}
        if ([self elmDriveRoute:hgElmExitRoute count:sizeof(hgElmExitRoute)/sizeof(*hgElmExitRoute) qa:&qa mr:&mr]) [self elmAdvance:21];return;
    case 21: if (qa.mapId==34) {[self elmAdvance:22];return;}if (qa.mapId==143 && qa.x==4 && qa.z==9) [self elmSendDirection:128 qa:&qa mr:&mr];return;
    case 22: {
        if (qa.phone.callCompletions==1 && HGElmPhoneReady(&qa.phone,1)) {[self elmAdvance:23];return;}
        BOOL hangup=qa.phone.callMainState==2 && qa.phone.callAwaitHangup;
        uint32_t serial=qa.phone.callTextSerial;
        if (HGElmCallLease(&qa.phone,qa.phone.ui.ownerGeneration,qa.phone.ui.revision,serial,hangup)
            && (hangup ? !run[@"hangup"] : serial && serial!=[run[@"callSerial"] unsignedIntValue])) {
            if (!hangup && (serial>64 || serial!=[run[@"callSerial"] unsignedIntValue]+1)) {[self finishElmUIKitCheck:@"elm_text_serial_failed"];return;}
            run[@"callFacts"]=@{@"target":@(qa.phone.callTarget),@"incoming":@(qa.phone.callIncoming),@"scripted":@(qa.phone.callScripted),
                @"scriptID":@(qa.phone.callScriptID),@"effectType":@(qa.phone.callEffectType),@"textSerial":@(serial),@"coverage":@(qa.phone.coverage),
                @"generation":@(qa.phone.ui.ownerGeneration),@"revision":@(qa.phone.ui.revision)};
            if (![self elmCapture:hangup ? @"hangup" : @"elm-call"]) return;
            if ([self elmSendKey:1 qa:&qa mr:&mr]) {if (hangup) run[@"hangup"]=@YES;else run[@"callSerial"]=@(serial);}
        }return;
    }
    case 23: if (!qa.phone.ui.active) {[self elmAdvance:24];return;}
        if (HGElmPhoneReady(&qa.phone,1) && qa.phone.callCompletions==1) {
            if ([run[@"callCloses"] unsignedIntValue]>=3) {[self finishElmUIKitCheck:@"elm_close_limit"];return;}
            if ([self elmSendKey:2 qa:&qa mr:&mr]) run[@"callCloses"]=@([run[@"callCloses"] unsignedIntValue]+1);
        }return;
    case 24: if (!HGElmEndpoint(&qa,&mr)) return;
        if ([run[@"callCloses"] unsignedIntValue]<2 || ![run[@"hangup"] boolValue]) {[self finishElmUIKitCheck:@"elm_call_sequence_failed"];return;}
        if (!run[@"requestSerial"]) [self queueElmQARequest:HG_PG_ELM_FINISH];if ((qa.phone.coverage&255)==255) [self elmAdvance:25];return;
    case 25: if (!HGElmEndpoint(&qa,&mr) || qa.phone.coverage!=255 || ![self elmCapture:@"field-return"]) return;
        run[@"terminal"]=@{@"map":@(qa.mapId),@"x":@(qa.x),@"z":@(qa.z),@"facing":@(qa.facing),@"coverage":@(qa.phone.coverage),
            @"parentEntries":@(qa.parentEntries),@"parentExits":@(qa.parentExits),@"selections":@(qa.phone.callSelections),
            @"completions":@(qa.phone.callCompletions),@"lastApp":@(qa.lastApp),@"battleLaunches":@(qa.battleLaunches),@"battleReturns":@(qa.battleReturns)};
        [self finishElmUIKitCheck:@"passed_original_elm_panic_call_uikit"];return;
    default:[self finishElmUIKitCheck:@"invalid_stage"];return;
    }
}
@end
#endif
