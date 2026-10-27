// SPDX-License-Identifier: GPL-3.0-or-later
// Private attached-control replay; the native report approval remains separately pinned.
#ifdef HEARTGOLD_ORIGINAL_PC_BOX_CHECK
#include "pc_box_qa.h"
#import <CommonCrypto/CommonDigest.h>
_Static_assert(sizeof(HGPCBoxQAState)==1372 && offsetof(HGPCBoxQAState,pc)==212
    && offsetof(HGPCBoxQAState,summary)==264 && offsetof(HGPCBoxQAState,naming)==1068
    && offsetof(HGPCBoxQAState,messageAwaitInput)==1368,"Private PC QA v2 ABI");

static NSString *PCBoxCheckHash(NSData *data) {
    unsigned char hash[CC_SHA256_DIGEST_LENGTH]; CC_SHA256(data.bytes,(CC_LONG)data.length,hash);
    NSMutableString *result=[NSMutableString new]; for (NSUInteger i=0;i<sizeof(hash);i++) [result appendFormat:@"%02x",hash[i]];
    return result;
}
static BOOL PCBoxCheckPixels(UIImage *image) {
    if (!image.CGImage) return NO;
    NSData *data=CFBridgingRelease(CGDataProviderCopyData(CGImageGetDataProvider(image.CGImage)));
    const uint8_t *pixels=data.bytes;
    for (NSUInteger i=4;i+4<=data.length;i+=4) if (memcmp(pixels,pixels+i,4)) return YES;
    return NO;
}
static NSDictionary *PCBoxCheckPrivateFiles(void) {
    NSURL *support=[NSFileManager.defaultManager URLsForDirectory:NSApplicationSupportDirectory inDomains:NSUserDomainMask].firstObject;
    support=[support URLByAppendingPathComponent:@"HeartGoldOriginal" isDirectory:YES];
    NSMutableDictionary *hashes=[NSMutableDictionary new];
    for (NSString *suffix in @[@"",@".console",@".wifi",@".lock"]) {
        NSString *name=[@"heartgold.sav" stringByAppendingString:suffix]; NSURL *url=[support URLByAppendingPathComponent:name];
        NSDictionary *attributes=[NSFileManager.defaultManager attributesOfItemAtPath:url.path error:nil];
        NSData *bytes=[attributes[NSFileType] isEqual:NSFileTypeRegular] ? [NSData dataWithContentsOfURL:url] : nil;
        if (!bytes) return nil; hashes[name]=PCBoxCheckHash(bytes);
    }
    return hashes;
}
static BOOL PCBoxCheckTarget(UIControl *control) {
    if (!control.window || control.hidden || !control.enabled || control.alpha<0.01 || control.bounds.size.width<44 || control.bounds.size.height<44) return NO;
    for (UIView *parent=control.superview;parent;parent=parent.superview) if (parent.hidden || parent.alpha<0.01) return NO;
    return YES;
}
static BOOL PCBoxCheckSemanticOwner(const HGPCBoxQAState *a,const HGPCBoxQAState *b) {
    // QA revision advances every tick; production scene tokens identify the admitted displayed owner.
    return a->stage==b->stage && a->mapId==b->mapId && a->baselinePrepared==b->baselinePrepared
        && a->checkpointSerial==b->checkpointSerial && a->fieldReady==b->fieldReady
        && !memcmp(&a->pc,&b->pc,sizeof(a->pc)) && !memcmp(&a->summary,&b->summary,sizeof(a->summary))
        && !memcmp(&a->naming,&b->naming,sizeof(a->naming));
}

@implementation OriginalController (PCBoxUIKitCheck)
- (void)clearPCBoxCheckTransientLocked:(BOOL)interrupted {
    _pcBoxQARequestPending=_pcBoxQARequestAccepted=NO; _pcBoxQARequestExpected=nil; _pcBoxQARequestKind=0;
    _pcBoxQAPulseStage=_pcBoxQAPulseKey=_pcBoxQAPulseReads=_pcBoxQAPulseReleaseReads=_pcBoxQAPulseHeld=0;
    _pcBoxQAPulseGeneric=_pcBoxQAPulseMessage=NO;
    if (interrupted) _pcBoxQAPulseError=4;
}
- (BOOL)readPCBoxQAOwner:(HGPCBoxQAState *)qa {
    uint32_t address=w2c_hgengine_HGNativePCBoxQAGetState(_game);
    if (!address || address>_env->memory->size || sizeof(*qa)>_env->memory->size-address) wasm_rt_trap(WASM_RT_TRAP_OOB);
    memcpy(qa,_env->memory->data+address,sizeof(*qa));
    if (qa->version!=HG_PC_QA_VERSION || qa->byteSize!=sizeof(*qa) || qa->messageAwaitInput>1
        || w2c_hgengine_HGNativePCBoxQAGetState(_game)!=address
        || memcmp(qa,_env->memory->data+address,sizeof(*qa))) wasm_rt_trap(WASM_RT_TRAP_OOB);
    return !qa->error;
}
- (BOOL)queuePCBoxQARequest:(uint32_t)kind {
    if (_displayedPCBoxQAState.length!=sizeof(HGPCBoxQAState)) return NO;
    [_condition lock];
    BOOL idle=!_paused && !_stopping && !_failure && !_pcBoxQARequestPending && !_pcBoxQAPulseStage
        && !_pcBoxChoicePending && !_pcBoxInputActive && !_pcBoxInputCooling
        && !_fieldChoiceChoicePending && !_fieldChoiceInputActive && !_fieldChoiceInputCooling
        && !_summaryChoicePending && !_summaryInputActive && !_namingChoicePending && !_namingInputActive && !_namingInputCooling;
    if (idle) {
        _pcBoxQARequestExpected=_displayedPCBoxQAState; _pcBoxQARequestKind=kind;
        _pcBoxQARequestPending=YES; _pcBoxQARequestSerial++;
    }
    [_condition unlock]; return idle;
}
- (void)processPCBoxQARequestOwnerLocked:(const HGPCBoxQAState *)qa {
    if (!_pcBoxQARequestPending) return;
    const HGPCBoxQAState *expected=_pcBoxQARequestExpected.bytes; uint32_t kind=_pcBoxQARequestKind;
    BOOL allowed=!_paused && !_stopping && !_failure && !_pcBoxQAPulseStage
        && _pcBoxQARequestExpected.length==sizeof(*expected) && PCBoxCheckSemanticOwner(expected,qa);
    if (kind==HG_PC_QA_START) allowed&=qa->stage==0 && qa->fieldReady && qa->mapId==33;
    else if (kind==HG_PC_QA_CHECKPOINT) allowed&=qa->baselinePrepared && !qa->summary.active && !qa->naming.active
        && ((qa->fieldReady && !qa->pc.active) || (qa->pc.ready && !qa->pc.cursorHolding));
    else if (kind==HG_PC_QA_RETURN) allowed&=qa->stage==3 && qa->fieldReady && qa->mapId==69 && !qa->pc.active;
    else allowed=NO;
    // Only this owner-thread call can touch the private guest request queue.
    _pcBoxQARequestAccepted=allowed && w2c_hgengine_HGNativePCBoxQARequest(_game,qa->revision,kind);
    _pcBoxQARequestCompleted=_pcBoxQARequestSerial; _pcBoxQARequestPending=NO;
    _pcBoxQARequestExpected=nil; _pcBoxQARequestKind=0;
}
- (void)observePCBoxCheckPulseOwnerLocked:(const HGPCBoxQAState *)qa reads:(uint32_t)reads keys:(uint32_t *)keys pressed:(BOOL)pressed {
    if (!_pcBoxQAPulseStage) return;
    if (qa->error || _paused || _stopping || _failure || pressed || (*keys && *keys!=_pcBoxQAPulseKey)) {_pcBoxQAPulseError=1;return;}
    if (_pcBoxQAPulseMessage && _pcBoxQAPulseStage==1 && (!qa->messageAwaitInput || _fieldChoiceOwnerGeneration)) {
        _keys=_pendingKeys=_submittedKeys=0; *keys=0; _pcBoxQAPulseError=5; return;
    }
    if (_pcBoxQAPulseStage==1 && *keys==_pcBoxQAPulseKey) {
        _pcBoxQAPulseStage=2; _pcBoxQAPulseReads=reads;
    }
    if (_pcBoxQAPulseStage==2) {
        uint32_t held=reads-_pcBoxQAPulseReads;
        if (_pcBoxQAPulseGeneric && (held>=4 || (_pcBoxQAPulseMessage && (!qa->messageAwaitInput || _fieldChoiceOwnerGeneration)))) {
            // Release only the admitted real generic control pulse; this hook never creates input.
            _keys=_pendingKeys=_submittedKeys=_joystickKeys=_pendingJoystickKeys=_submittedJoystickKeys=0; *keys=0;
        }
        if (!*keys) {
            _pcBoxQAPulseHeld=held; _pcBoxQAPulseReleaseReads=reads; _pcBoxQAPulseStage=3;
            if (!held || held>4) _pcBoxQAPulseError=2;
        } else if (held>4) _pcBoxQAPulseError=3;
    }
    if (_pcBoxQAPulseStage==3 && reads-_pcBoxQAPulseReleaseReads>=2) _pcBoxQAPulseStage=4;
}
- (BOOL)pcBoxCheckPacket:(HGPCBoxQAState *)qa {
    if (_displayedPCBoxQAState.length!=sizeof(*qa)) return NO;
    memcpy(qa,_displayedPCBoxQAState.bytes,sizeof(*qa));
    if (qa->version!=2 || qa->byteSize!=sizeof(*qa) || qa->error || qa->messageAwaitInput>1) return NO;
    return _displayedPCBoxUIState.length==sizeof(qa->pc) && !memcmp(_displayedPCBoxUIState.bytes,&qa->pc,sizeof(qa->pc))
        && _displayedSummaryUIState.length==sizeof(qa->summary) && !memcmp(_displayedSummaryUIState.bytes,&qa->summary,sizeof(qa->summary))
        && _displayedNamingUIState.length==sizeof(qa->naming) && !memcmp(_displayedNamingUIState.bytes,&qa->naming,sizeof(qa->naming));
}
- (BOOL)pcBoxCheckExpected:(NSDictionary *)event qa:(const HGPCBoxQAState *)qa {
    NSString *owner=event[@"owner"];
    NSDictionary *pc=event[@"pc"];
    if (pc && (qa->pc.phase!=[pc[@"phase"] unsignedIntValue] || qa->pc.mode!=[pc[@"mode"] unsignedIntValue]
        || qa->pc.selectedSlot!=[pc[@"selectedSlot"] unsignedIntValue])) return NO;
    if ([owner isEqual:@"pc"]) {
        return qa->pc.active && qa->pc.ready && qa->pc.viewActive && !qa->summary.active && !qa->naming.active
            && qa->pc.phase==[pc[@"phase"] unsignedIntValue] && qa->pc.mode==[pc[@"mode"] unsignedIntValue]
            && qa->pc.selectedSlot==[pc[@"selectedSlot"] unsignedIntValue];
    }
    if ([owner isEqual:@"summary"]) {
        NSDictionary *summary=event[@"summary"];
        return qa->summary.active && qa->summary.ready && !qa->naming.active && qa->summary.monDataType==2
            && qa->summary.phase==[summary[@"phase"] unsignedIntValue] && qa->summary.partySlot==[summary[@"partySlot"] unsignedIntValue];
    }
    if ([owner isEqual:@"naming"]) return qa->naming.active && qa->naming.ready;
    const HGNativeFieldChoiceUIState *choice=_displayedFieldChoiceUIState.length==sizeof(*choice) ? _displayedFieldChoiceUIState.bytes : NULL;
    if ([owner isEqual:@"field_script"]) return !qa->pc.active && !qa->summary.active && !qa->naming.active && qa->scriptMenuReady && choice
        && HGNativeFieldChoiceUIInputAllowed(choice,choice->ownerGeneration,choice->revision) && choice->rowCount==qa->scriptMenuCount;
    if ([owner isEqual:@"field_message"]) return [event[@"keys"] unsignedIntValue]==1 && qa->messageAwaitInput==1 && !qa->scriptMenuReady
        && !qa->pc.active && !qa->summary.active && !qa->naming.active && choice
        && !(choice->active && (choice->ready || choice->viewActive || HGNativeFieldChoiceUIIsListPhase(choice->phase)));
    return [owner isEqual:@"boot"] ? qa->stage==0 && !qa->baselinePrepared && !qa->pc.active
        : [owner isEqual:@"field"] && !qa->pc.active && !qa->summary.active && !qa->naming.active && qa->fieldReady;
}
- (BOOL)pcBoxCheckSend:(NSDictionary *)event qa:(const HGPCBoxQAState *)qa {
    NSString *owner=event[@"owner"]; uint32_t key=[event[@"keys"] unsignedIntValue]; UIControl *control=nil;
    if ([owner isEqual:@"field_script"]) {
        BOOL visibleSub=_fieldChoiceScene && _displayedEngine==1 && !_canvas.hidden && PCBoxCheckPixels(_canvas.image)
            && !_fieldChoicePreview.hidden && _fieldChoicePreview.image.size.width==256 && _fieldChoicePreview.image.size.height==48;
        if (!visibleSub) return NO;
        NSString *capture=[NSString stringWithFormat:@"pc-uikit-script-menu-%u.png",qa->scriptMenuSerial];
        if (!_pcBoxCheckRuntime[capture]) {
            if (![self pcBoxCheckCapture:capture]) return NO; _pcBoxCheckRuntime[capture]=@YES;
        }
    }
    BOOL generic=[owner isEqual:@"field"] || [owner isEqual:@"field_message"] || [owner isEqual:@"boot"];
    BOOL direction=generic && (key==16 || key==32 || key==64 || key==128);
    if ([owner isEqual:@"summary"]) {
        NSUInteger index=key==64 ? 3 : key==128 ? 4 : key==2 ? 0 : NSNotFound;
        if (index==NSNotFound) return NO; control=_summaryNavigation[index];
    } else control=direction ? _joystick : _buttons[@(key)];
    if (!PCBoxCheckTarget(control) || ![self pcBoxCheckExpected:event qa:qa]) return NO;
    [_condition lock];
    BOOL idle=!_paused && !_stopping && !_failure && !_pcBoxQAPulseStage && !_pcBoxQARequestPending
        && !_pcBoxChoicePending && !_pcBoxInputActive && (![owner isEqual:@"pc"] || !_pcBoxInputCooling) && !_summaryChoicePending && !_summaryInputActive
        && !_fieldChoiceChoicePending && !_fieldChoiceInputActive && !_fieldChoiceInputCooling
        && !_namingChoicePending && !_namingInputActive && !_namingInputCooling;
    if (idle) {_pcBoxQAPulseStage=1;_pcBoxQAPulseKey=key;_pcBoxQAPulseGeneric=generic;_pcBoxQAPulseMessage=[owner isEqual:@"field_message"];_pcBoxQAPulseError=0;_pcBoxQAPulseHeld=0;}
    [_condition unlock]; if (!idle) return NO;
    if (direction) {
        CGPoint center=CGPointMake(CGRectGetMidX(_joystick.bounds),CGRectGetMidY(_joystick.bounds));
        CGFloat travel=MAX(1,MIN(_joystick.bounds.size.width,_joystick.bounds.size.height)/2-24);
        CGPoint point=CGPointMake(center.x+(key==16 ? travel : key==32 ? -travel : 0),center.y+(key==128 ? travel : key==64 ? -travel : 0));
        [_joystick checkPCBoxEventAt:point pressed:YES];
    } else if ([owner isEqual:@"summary"]) [control sendActionsForControlEvents:UIControlEventTouchUpInside];
    else [control sendActionsForControlEvents:UIControlEventTouchDown];
    _pcBoxCheckRuntime[@"pendingControl"]=control; _pcBoxCheckRuntime[@"pendingDirection"]=@(direction);
    _pcBoxCheckRuntime[@"pendingEvent"]=event;
    return YES;
}
- (BOOL)pcBoxCheckCapture:(NSString *)name {
    [self.view layoutIfNeeded]; UIGraphicsBeginImageContextWithOptions(self.view.bounds.size,NO,0);
    BOOL drawn=[self.view drawViewHierarchyInRect:self.view.bounds afterScreenUpdates:YES];
    UIImage *image=UIGraphicsGetImageFromCurrentImageContext(); UIGraphicsEndImageContext();
    NSData *png=image ? UIImagePNGRepresentation(image) : nil;
    NSURL *folder=[NSFileManager.defaultManager URLsForDirectory:NSDocumentDirectory inDomains:NSUserDomainMask].firstObject;
    BOOL written=png && [png writeToURL:[folder URLByAppendingPathComponent:name] options:NSDataWritingAtomic error:nil];
    [_pcBoxCheckRuntime[@"captures"] addObject:@{@"file":name,@"drawn":@(drawn),@"written":@(written),@"displayedFrame":@(_pcBoxQADisplayedPublishedFrame)}];
    return drawn && written;
}
- (BOOL)pcBoxCheckConservation:(const HGPCBoxQAState *)qa expected:(NSDictionary *)expected {
    if (!qa->baselinePrepared || qa->partyCount!=[expected[@"partyCount"] unsignedIntValue]
        || qa->boxedCount!=[expected[@"boxedCount"] unsignedIntValue] || !qa->bagUnchanged || !qa->flagsUnchanged
        || !qa->moneyDexUnchanged || !qa->boxNameUnchanged || qa->mons[0].location!=HG_PC_QA_PARTY_LOCATION) return NO;
    NSArray *mons=expected[@"mons"]; if (mons.count!=4) return NO;
    for (NSUInteger i=0;i<4;i++) {
        const uint32_t *actual=(const uint32_t *)&qa->mons[i]; NSArray *wanted=mons[i]; if (wanted.count!=5) return NO;
        for (NSUInteger j=0;j<5;j++) if (actual[j]!=[wanted[j] unsignedIntValue]) return NO;
    }
    return YES;
}
- (BOOL)checkFieldChoiceHostPrimitives {
    OriginalController *fixture=[OriginalController new]; fixture->_condition=[NSCondition new]; fixture->_fieldChoiceScene=YES;
    OriginalKeyButton *button=[OriginalKeyButton new]; button.tag=1; button.enabled=YES;
    HGNativeFieldChoiceUIState choice={.active=1,.ownerGeneration=7,.revision=11,.ready=1,.phase=HG_FIELD_CHOICE_UI_LIST_INPUT,
        .rowCount=3,.selectedIndex=1,.cancelEnabled=0,.viewActive=1};
    NSData *packet=[NSData dataWithBytes:&choice length:sizeof(choice)];
    fixture->_fieldChoiceUIState=fixture->_displayedFieldChoiceUIState=packet;
    [fixture queueFieldChoiceKey:button]; uint32_t keys=0;
    [fixture takeFieldChoiceInputLocked:&choice reads:20 keys:&keys]; BOOL valid=keys==1 && fixture->_fieldChoiceInputActive;
    [fixture takeFieldChoiceInputLocked:&choice reads:23 keys:&keys]; valid&=keys==1;
    [fixture takeFieldChoiceInputLocked:&choice reads:24 keys:&keys]; valid&=!keys && fixture->_fieldChoiceInputCooling;
    [fixture queueFieldChoiceKey:button]; valid&=!fixture->_fieldChoiceChoicePending;
    [fixture takeFieldChoiceInputLocked:&choice reads:25 keys:&keys]; valid&=fixture->_fieldChoiceInputCooling && !keys;
    [fixture takeFieldChoiceInputLocked:&choice reads:26 keys:&keys]; valid&=!fixture->_fieldChoiceInputCooling && !keys;
    for (NSUInteger i=0;i<4;i++) {
        [fixture clearFieldChoiceInputLocked]; HGNativeFieldChoiceUIState changed=choice;
        if (!i) changed.revision++; else if (i==1) changed.ownerGeneration++; else if (i==2) changed.phase=1; else changed.viewActive=0;
        fixture->_fieldChoiceUIState=[NSData dataWithBytes:&changed length:sizeof(changed)];
        [fixture queueFieldChoiceKey:button]; valid&=!fixture->_fieldChoiceChoicePending;
    }
    fixture->_fieldChoiceUIState=packet;
    for (NSNumber *key in @[@0,@2,@3,@8,@0x400,@0x800]) {
        [fixture clearFieldChoiceInputLocked]; button.tag=key.unsignedIntValue; [fixture queueFieldChoiceKey:button]; valid&=!fixture->_fieldChoiceChoicePending;
    }
    button.tag=1; fixture->_paused=YES; [fixture queueFieldChoiceKey:button]; valid&=!fixture->_fieldChoiceChoicePending;
    fixture->_paused=NO; [fixture queueFieldChoiceKey:button]; [fixture takeFieldChoiceInputLocked:&choice reads:30 keys:&keys];
    choice.viewActive=0; [fixture takeFieldChoiceInputLocked:&choice reads:31 keys:&keys]; valid&=!keys && !fixture->_fieldChoiceInputActive;
    [fixture clearFieldChoiceInputLocked];
    return valid && !fixture->_fieldChoiceChoicePending && !fixture->_fieldChoiceInputActive && !fixture->_fieldChoiceInputCooling
        && !fixture->_fieldChoiceChoiceKeys && !fixture->_fieldChoiceInputKeys && !fixture.isViewLoaded && !fixture->_game && !fixture->_env;
}
- (void)beginPCBoxUIKitCheck {
    NSURL *url=[NSBundle.mainBundle URLForResource:@"pc-box-approved-trace" withExtension:@"json"];
    NSData *bytes=url ? [NSData dataWithContentsOfURL:url] : nil;
    id decoded=bytes ? [NSJSONSerialization JSONObjectWithData:bytes options:0 error:nil] : nil;
    NSDictionary *plan=[decoded isKindOfClass:NSDictionary.class] ? decoded : nil;
    NSArray *names=@[@"caseBinding",@"nativeReceipt",@"singleRuntime",@"physicalEntry",@"pairedPackets",@"attachedTargets",
        @"pcSingleLandscape",@"originalSourcePixels",@"pcKeysOnly",@"releaseObserved",@"approvedTraceOnce",@"depositWithdraw",
        @"moveSwap",@"refusalAndCancel",@"itemConservation",@"boxedSummary",@"summarySparseNext",@"namingStartThenA",
        @"childReturnsNewOwner",@"checkpointConservation",@"baselineSetterFence",@"route29Return",@"normalReset",@"capturesComplete"];
    NSMutableDictionary *gates=[NSMutableDictionary new]; for (NSString *name in names) gates[name]=@NO;
    NSTimeInterval now=NSDate.timeIntervalSinceReferenceDate;
    _pcBoxCheckRuntime=[@{@"plan":plan ?: @{},@"gates":gates,@"phase":@"boot",@"index":@0,@"checkpoint":@0,
        @"boot":@0,@"coverage":@0,@"actions":[NSMutableArray new],@"captures":[NSMutableArray new],
        @"started":@(now),@"advanced":@(now),@"initialBoots":@1} mutableCopy];
    NSDictionary *privateFiles=PCBoxCheckPrivateFiles();
    if (privateFiles) _pcBoxCheckRuntime[@"privateFilesBefore"]=privateFiles;
    gates[@"caseBinding"]=@([[NSBundle.mainBundle objectForInfoDictionaryKey:@"HGPrivateCheckCase"] isEqual:@"pc-box"]
        && [NSProcessInfo.processInfo.environment[@"HEARTGOLD_CHECK_CASE"] isEqual:@"pc-box"]
        && [NSProcessInfo.processInfo.environment[@"HEARTGOLD_TEST_MUTE"] isEqual:@"1"] && [self checkFieldChoiceHostPrimitives]);
    gates[@"nativeReceipt"]=@(bytes && [PCBoxCheckHash(bytes) isEqual:@HG_PC_BOX_REPLAY_SHA256]
        && [plan[@"sourceReportSha256"] isEqual:@HG_PC_BOX_NATIVE_REPORT_SHA256]
        && [plan[@"privateModuleSha256"] isEqual:@HG_PC_BOX_NATIVE_MODULE_SHA256]
        && [plan[@"qaVersion"] unsignedIntValue]==2 && [plan[@"qaBytes"] unsignedIntValue]==1372 && [plan[@"actions"] count] && [plan[@"checkpoints"] count]>=10);
    if (![gates[@"caseBinding"] boolValue] || ![gates[@"nativeReceipt"] boolValue]) [self finishPCBoxUIKitCheck:@"binding_failed"];
    else if (privateFiles.count!=4) [self finishPCBoxUIKitCheck:@"private_fixture_missing"];
}
- (void)finishPCBoxUIKitCheck:(NSString *)status {
    NSMutableDictionary *run=_pcBoxCheckRuntime; if (!run || run[@"status"]) return;
    run[@"status"]=status; run[@"phase"]=@"stopping"; run[@"advanced"]=@(NSDate.timeIntervalSinceReferenceDate);
    [_condition lock]; [self clearPCBoxCheckTransientLocked:NO];
    _keys=_pendingKeys=_submittedKeys=_joystickKeys=_pendingJoystickKeys=_submittedJoystickKeys=0; _touchPressed=NO;
    [self clearFieldChoiceInputLocked]; [self clearPCBoxInputLocked]; [self clearNamingInputLocked]; _summaryChoicePending=_summaryInputActive=_summaryTouchPressed=NO; _summaryInputKeys=0;
    [_condition unlock]; [self stop];
    // stop invalidates displayLink; finish the existing normal owner reset with a bounded main-queue poll.
    __block NSUInteger polls=0; __block void (^wait)(void);
    wait=^{
        [self->_condition lock]; BOOL finished=self->_finished,quiescent=self->_resetQuiesced; [self->_condition unlock];
        if (!finished && ++polls<400) {dispatch_after(dispatch_time(DISPATCH_TIME_NOW,50*NSEC_PER_MSEC),dispatch_get_main_queue(),wait);return;}
        NSMutableDictionary *gates=run[@"gates"]; gates[@"normalReset"]=@(finished && quiescent);
        NSArray *captures=run[@"captures"]; BOOL complete=captures.count>=8;
        for (NSDictionary *capture in captures) complete&=[capture[@"drawn"] boolValue] && [capture[@"written"] boolValue];
        gates[@"capturesComplete"]=@(complete); BOOL passed=[status isEqual:@"trace_complete"];
        for (NSNumber *value in gates.allValues) passed&=value.boolValue;
        NSDictionary *before=run[@"privateFilesBefore"],*after=PCBoxCheckPrivateFiles();
        BOOL privateUnchanged=before.count==4 && after.count==4 && [before isEqualToDictionary:after]; passed&=privateUnchanged;
        NSDictionary *report=@{@"case":@"pc-box",@"passed":@(passed),@"complete":@(passed),@"status":status,@"checks":gates,
            @"actions":run[@"actions"],@"captures":captures,@"physicalGestures":@NO,@"productionApiAdded":@NO,
            @"sourceNativeReportSha256":@HG_PC_BOX_NATIVE_REPORT_SHA256,@"privateModuleSha256":@HG_PC_BOX_NATIVE_MODULE_SHA256,
            @"firstFailure":run[@"firstFailure"] ?: NSNull.null,
            @"privateFourFilesBefore":before ?: @{},@"privateFourFilesAfter":after ?: @{},@"privateFourFilesUnchanged":@(privateUnchanged),
            @"savePreservationScope":@"private App four-file copy checked here; external runner also verifies original private fixture and protected user/production artifacts"};
        NSURL *folder=[NSFileManager.defaultManager URLsForDirectory:NSDocumentDirectory inDomains:NSUserDomainMask].firstObject;
        NSData *json=[NSJSONSerialization dataWithJSONObject:report options:NSJSONWritingPrettyPrinted error:nil];
        [json writeToURL:[folder URLByAppendingPathComponent:@"pc-box-uikit-report.json"] options:NSDataWritingAtomic error:nil]; wait=nil;
    }; wait();
}
- (void)checkPCBoxUIKitDisplay {
    NSMutableDictionary *run=_pcBoxCheckRuntime; if (!run || run[@"status"]) return;
    NSTimeInterval now=NSDate.timeIntervalSinceReferenceDate;
    if (now-[run[@"started"] doubleValue]>480 || now-[run[@"advanced"] doubleValue]>40) {[self finishPCBoxUIKitCheck:@"bounded_timeout"];return;}
    [_condition lock]; NSString *failure=_failure; [_condition unlock];
    if (failure) {run[@"firstFailure"]=failure;[self finishPCBoxUIKitCheck:@"engine_failure"];return;}
    HGPCBoxQAState qa; if (![self pcBoxCheckPacket:&qa]) return;
    NSMutableDictionary *gates=run[@"gates"]; NSDictionary *plan=run[@"plan"]; gates[@"pairedPackets"]=@YES;
    [_condition lock]; uint32_t pulse=_pcBoxQAPulseStage,error=_pcBoxQAPulseError,held=_pcBoxQAPulseHeld;
    uint32_t completed=_pcBoxQARequestCompleted; BOOL accepted=_pcBoxQARequestAccepted; [_condition unlock];
    if (error || qa.error) {[self finishPCBoxUIKitCheck:@"owner_or_input_error"];return;}
    if (run[@"pendingEvent"]) {
        if (pulse!=4) return;
        UIControl *control=run[@"pendingControl"];
        if ([run[@"pendingDirection"] boolValue]) [_joystick checkPCBoxEventAt:CGPointZero pressed:NO];
        else if (![run[@"pendingEvent"][@"owner"] isEqual:@"summary"]) [control sendActionsForControlEvents:UIControlEventTouchUpInside];
        NSMutableDictionary *done=[run[@"pendingEvent"] mutableCopy]; done[@"hostHeldPadReads"]=@(held); done[@"releasePadReads"]=@2;
        [run[@"actions"] addObject:done]; gates[@"releaseObserved"]=@YES;
        if ([done[@"owner"] isEqual:@"naming"]) {
            NSUInteger count=[run[@"namingCount"] unsignedIntegerValue];
            if ([done[@"keys"] unsignedIntValue]!=(count ? 1u : 8u) || count>=2) {[self finishPCBoxUIKitCheck:@"naming_trace_order"];return;}
            run[@"namingCount"]=@(count+1); gates[@"namingStartThenA"]=@(count==1);
        }
        if ([done[@"owner"] isEqual:@"boot"]) run[@"boot"]=@([run[@"boot"] unsignedIntegerValue]+1);
        else run[@"index"]=@([run[@"index"] unsignedIntegerValue]+1);
        [run removeObjectForKey:@"pendingEvent"]; [run removeObjectForKey:@"pendingControl"]; [run removeObjectForKey:@"pendingDirection"];
        [_condition lock]; _pcBoxQAPulseStage=0; [_condition unlock]; run[@"advanced"]=@(now); return;
    }
    if (qa.baselinePrepared) {
        if (!run[@"setters"]) run[@"setters"]=@(qa.preparationSetterCalls);
        if ([run[@"setters"] unsignedIntValue]!=qa.preparationSetterCalls) {[self finishPCBoxUIKitCheck:@"setter_after_baseline"];return;}
        gates[@"baselineSetterFence"]=@YES;
    }
    if (qa.summary.active || qa.naming.active) {
        if (_pcBoxScene || !_pcBoxPanel.hidden) {[self finishPCBoxUIKitCheck:@"parent_covered_child"];return;}
        if (!run[@"childGeneration"]) run[@"childGeneration"]=run[@"lastPCGeneration"] ?: @0;
        if (qa.summary.ready) {
            gates[@"boxedSummary"]=@(_summaryScene && !_summaryPanel.hidden && qa.summary.monDataType==2 && qa.summary.partyCount==30);
            if (qa.summary.partySlot==29) gates[@"summarySparseNext"]=@(qa.summary.previousMonEnabled && qa.summary.previousMonSlot==14 && !qa.summary.nextMonEnabled);
            NSString *name=[NSString stringWithFormat:@"pc-uikit-boxed-summary-%u.png",qa.summary.partySlot];
            if (!run[name]) {if (![self pcBoxCheckCapture:name]) {[self finishPCBoxUIKitCheck:@"capture_failed"];return;}run[name]=@YES;}
        }
        if (qa.naming.ready && !run[@"namingCapture"]) {
            if (![self pcBoxCheckCapture:@"pc-uikit-original-naming-keyboard.png"]) {[self finishPCBoxUIKitCheck:@"capture_failed"];return;}
            run[@"namingCapture"]=@YES;
        }
    } else if (qa.pc.ready && qa.pc.viewActive) {
        NSNumber *old=run[@"childGeneration"];
        if (old) {
            if (!old.unsignedIntValue || old.unsignedIntValue==qa.pc.ownerGeneration) {[self finishPCBoxUIKitCheck:@"child_return_generation"];return;}
            NSUInteger returns=[run[@"childReturnCount"] unsignedIntegerValue]+1; run[@"childReturnCount"]=@(returns);
            gates[@"childReturnsNewOwner"]=@(returns>=2); [run removeObjectForKey:@"childGeneration"];
            if (![self pcBoxCheckCapture:[NSString stringWithFormat:@"pc-uikit-return-%u.png",qa.pc.ownerGeneration]]) {[self finishPCBoxUIKitCheck:@"capture_failed"];return;}
        }
        run[@"lastPCGeneration"]=@(qa.pc.ownerGeneration);
        gates[@"pcSingleLandscape"]=@(_pcBoxScene && !_pcBoxPanel.hidden && _canvas.hidden && !_canvas.userInteractionEnabled
            && self.view.bounds.size.width>self.view.bounds.size.height && CGRectContainsRect(self.view.bounds,_pcBoxPanel.frame));
        gates[@"originalSourcePixels"]=@(PCBoxCheckPixels(_pcBoxWorkspace.image) && PCBoxCheckPixels(_pcBoxFooter.image)
            && PCBoxCheckPixels(_pcBoxInfo.image) && _pcBoxInfo.image.size.width==256 && _pcBoxInfo.image.size.height==192);
        [_condition lock]; BOOL touchBlocked=!_touchPressed; [_condition unlock];
        gates[@"pcKeysOnly"]=@(touchBlocked && _canvas.gestureRecognizers.count && !_canvas.userInteractionEnabled && _joystick.hidden);
    }
    NSString *phase=run[@"phase"];
    if ([phase isEqual:@"boot"]) {
        NSArray *boot=plan[@"boot"]; NSUInteger index=[run[@"boot"] unsignedIntegerValue];
        if (index<boot.count) {
            NSDictionary *row=boot[index]; if (_displayedPCBoxQAOwnerFrame<[row[@"frame"] unsignedIntValue]) return;
            NSMutableDictionary *event=[row mutableCopy]; event[@"owner"]=@"boot";
            if ([self pcBoxCheckSend:event qa:&qa]) gates[@"attachedTargets"]=@YES; return;
        }
        if (qa.fieldReady && qa.mapId==33 && [self queuePCBoxQARequest:HG_PC_QA_START]) {
            run[@"request"]=@(_pcBoxQARequestSerial); run[@"phase"]=@"prepare"; run[@"advanced"]=@(now);
        }
        return;
    }
    if ([phase isEqual:@"prepare"]) {
        if (completed<[run[@"request"] unsignedIntValue]) return;
        if (!accepted) {[self finishPCBoxUIKitCheck:@"start_request_rejected"];return;}
        if (qa.stage!=3 || !qa.fieldReady) return;
        NSDictionary *entry=plan[@"entry"];
        gates[@"physicalEntry"]=@(qa.baselinePrepared && qa.mapId==[entry[@"mapId"] unsignedIntValue]
            && qa.x==[entry[@"x"] unsignedIntValue] && qa.z==[entry[@"z"] unsignedIntValue]
            && qa.facing==[entry[@"facing"] unsignedIntValue] && qa.pcTileBehavior==[entry[@"pcTileBehavior"] unsignedIntValue]);
        if (![gates[@"physicalEntry"] boolValue]) {[self finishPCBoxUIKitCheck:@"physical_prerequisite_mismatch"];return;}
        if (_canvas.hidden || _displayedEngine || !PCBoxCheckPixels(_canvas.image)) return;
        if (![self pcBoxCheckCapture:@"pc-uikit-original-terminal-entry.png"]) {[self finishPCBoxUIKitCheck:@"capture_failed"];return;}
        run[@"phase"]=@"replay"; run[@"advanced"]=@(now); return;
    }
    NSArray *actions=plan[@"actions"],*checkpoints=plan[@"checkpoints"]; NSUInteger index=[run[@"index"] unsignedIntegerValue],ci=[run[@"checkpoint"] unsignedIntegerValue];
    NSDictionary *next=index<actions.count ? actions[index] : nil,*checkpoint=ci<checkpoints.count ? checkpoints[ci] : nil;
    if (checkpoint && (!next || [checkpoint[@"nativeStep"] unsignedIntValue]<[next[@"nativeStep"] unsignedIntValue])) {
        if ([checkpoint[@"mapId"] unsignedIntValue]==33 && qa.mapId!=33) {
            if (![phase isEqual:@"return"] && qa.fieldReady && !qa.pc.active && qa.mapId==69 && [self queuePCBoxQARequest:HG_PC_QA_RETURN]) {
                run[@"request"]=@(_pcBoxQARequestSerial); run[@"phase"]=@"return"; run[@"advanced"]=@(now);
            } else if ([phase isEqual:@"return"] && completed>=[run[@"request"] unsignedIntValue] && !accepted) [self finishPCBoxUIKitCheck:@"return_request_rejected"];
            return;
        }
        if (![phase isEqual:@"checkpoint"]) {
            BOOL ready=qa.fieldReady && !qa.pc.active && !_canvas.hidden && !_displayedEngine && PCBoxCheckPixels(_canvas.image);
            if (qa.pc.active) ready=qa.pc.ready && !qa.pc.cursorHolding && qa.pc.phase==[checkpoint[@"pc"][@"phase"] unsignedIntValue];
            if (ready && [self queuePCBoxQARequest:HG_PC_QA_CHECKPOINT]) {
                run[@"request"]=@(_pcBoxQARequestSerial); run[@"checkpointBefore"]=@(qa.checkpointSerial);
                run[@"phase"]=@"checkpoint"; run[@"advanced"]=@(now);
            }
            return;
        }
        if (completed<[run[@"request"] unsignedIntValue]) return;
        if (!accepted) {[self finishPCBoxUIKitCheck:@"checkpoint_request_rejected"];return;}
        if (qa.checkpointSerial==[run[@"checkpointBefore"] unsignedIntValue]) return;
        if (![self pcBoxCheckConservation:&qa expected:checkpoint]) {[self finishPCBoxUIKitCheck:@"conservation_mismatch"];return;}
        uint32_t coverage=[run[@"coverage"] unsignedIntValue]|[checkpoint[@"coverage"] unsignedIntValue]; run[@"coverage"]=@(coverage);
        gates[@"checkpointConservation"]=@YES; gates[@"depositWithdraw"]=@((coverage&(HG_PC_QA_DEPOSIT|HG_PC_QA_WITHDRAW))==(HG_PC_QA_DEPOSIT|HG_PC_QA_WITHDRAW));
        gates[@"moveSwap"]=@((coverage&(HG_PC_QA_MOVE|HG_PC_QA_SWAP))==(HG_PC_QA_MOVE|HG_PC_QA_SWAP));
        gates[@"refusalAndCancel"]=@((coverage&(HG_PC_QA_LAST|HG_PC_QA_RELEASE_CANCEL))==(HG_PC_QA_LAST|HG_PC_QA_RELEASE_CANCEL));
        gates[@"itemConservation"]=@((coverage&HG_PC_QA_ITEMS)!=0); gates[@"route29Return"]=@(qa.stage==5 && qa.mapId==33 && qa.fieldReady && !qa.pc.active);
        if (![self pcBoxCheckCapture:[NSString stringWithFormat:@"pc-uikit-checkpoint-%02lu.png",(unsigned long)ci]]) {[self finishPCBoxUIKitCheck:@"capture_failed"];return;}
        run[@"checkpoint"]=@(ci+1); run[@"phase"]=@"replay"; run[@"advanced"]=@(now); return;
    }
    if (next) {if ([self pcBoxCheckSend:next qa:&qa]) {gates[@"attachedTargets"]=@YES;run[@"advanced"]=@(now);}return;}
    gates[@"approvedTraceOnce"]=@(index==actions.count && [run[@"actions"] count]==actions.count+[plan[@"boot"] count]
        && [run[@"coverage"] unsignedIntValue]==HG_PC_QA_REQUIRED && ci==checkpoints.count);
    [_condition lock]; uint32_t boots=_bootCount; [_condition unlock];
    gates[@"singleRuntime"]=@(boots==[run[@"initialBoots"] unsignedIntValue]);
    [self finishPCBoxUIKitCheck:@"trace_complete"];
}
@end
#endif
