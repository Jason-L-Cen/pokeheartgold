// SPDX-License-Identifier: GPL-3.0-or-later
// Normal Pokédex presentation; original controls and read-only packet remain authoritative.
@implementation OriginalController (PokedexUI)
- (void)setupPokedexControls {
    _pokedexLabels=[NSDictionary dictionaryWithContentsOfFile:[NSBundle.mainBundle.resourcePath stringByAppendingPathComponent:@"pokedex-ui-labels.plist"]];
    _pokedexPanel=[UIView new]; _pokedexPanel.hidden=YES; _pokedexPanel.backgroundColor=[UIColor colorWithRed:0.07 green:0.12 blue:0.16 alpha:1];
    [self.view addSubview:_pokedexPanel];
    _pokedexContent=[UIScrollView new]; _pokedexDetails=[UIScrollView new];
    for (UIScrollView *scroll in @[_pokedexContent,_pokedexDetails]) { scroll.clipsToBounds=YES; [_pokedexPanel addSubview:scroll]; }
    NSMutableArray *images=[NSMutableArray new],*buttons=[NSMutableArray new],*pans=[NSMutableArray new];
    for (NSUInteger i=0;i<HG_POKEDEX_UI_VISUALS;i++) {
        UIImageView *image=[UIImageView new]; image.contentMode=UIViewContentModeScaleAspectFit; image.hidden=YES;
        image.layer.magnificationFilter=kCAFilterNearest; image.clipsToBounds=YES; [_pokedexContent addSubview:image]; [images addObject:image];
    }
    for (NSUInteger i=0;i<HG_POKEDEX_UI_CONTROLS;i++) {
        UIButton *button=[UIButton buttonWithType:UIButtonTypeCustom]; button.tag=i; button.hidden=YES;
        button.layer.cornerRadius=8; button.layer.borderWidth=2; button.clipsToBounds=YES;
        button.imageView.layer.magnificationFilter=kCAFilterNearest;
        [button addTarget:self action:@selector(pokedexControlPressed:) forControlEvents:UIControlEventTouchUpInside];
        UIPanGestureRecognizer *pan=[[UIPanGestureRecognizer alloc] initWithTarget:self action:@selector(pokedexGesture:)];
        pan.minimumNumberOfTouches=pan.maximumNumberOfTouches=1; [button addGestureRecognizer:pan];
        [_pokedexContent.panGestureRecognizer requireGestureRecognizerToFail:pan]; [_pokedexDetails.panGestureRecognizer requireGestureRecognizerToFail:pan];
        [_pokedexContent addSubview:button]; [buttons addObject:button]; [pans addObject:pan];
    }
    _pokedexImages=images; _pokedexButtons=buttons; _pokedexPans=pans; _pokedexGestureControl=-1;
}
- (void)clearPokedexInputLocked {
    _pokedexChoicePending=_pokedexInputActive=_pokedexInputCooling=_pokedexGestureLive=_pokedexGestureAdmitted=_pokedexGestureContinued=NO;
    _pokedexChoiceGeneration=_pokedexChoiceRevision=_pokedexChoicePhase=_pokedexChoiceKind=_pokedexChoiceIndex=0;
    _pokedexInputGeneration=_pokedexInputRevision=_pokedexInputPhase=_pokedexInputFamily=_pokedexInputKind=_pokedexInputIndex=_pokedexInputReads=_pokedexReleaseReads=0;
    _pokedexGestureValue=_pokedexGestureStartValue=_pokedexGesturePulse=_pokedexSampledGesturePulse=_pokedexGestureCommitReads=0; _pokedexGestureControl=-1;
    memset(&_pokedexInput,0,sizeof(_pokedexInput)); memset(&_pokedexGestureTrack,0,sizeof(_pokedexGestureTrack));
}
- (NSDictionary *)copyPokedexPictures:(const HGNativePokedexUIState *)state main:(NSData *)main sub:(NSData *)sub {
    if (!state->active) { _pokedexOwnerPictures=nil; return nil; }
    if (!state->visualCount && sub) {
        UIImage *message=OriginalFrameRegion(sub,16,152,224,40);
        _pokedexOwnerPictures=message ? @{@"passiveMessage":message,@"images":@[],@"metadata":[NSData dataWithBytes:state length:sizeof(*state)],@"generation":@(state->ownerGeneration),@"revision":@(state->revision),@"family":@(state->family),@"phase":@(state->phase),@"frame":@(state->frameSerial)} : nil;
        return _pokedexOwnerPictures;
    }
    if (_pokedexOwnerPictures[@"passiveMessage"]) _pokedexOwnerPictures=nil;
    BOOL same=[_pokedexOwnerPictures[@"generation"] unsignedIntValue]==state->ownerGeneration
        && [_pokedexOwnerPictures[@"revision"] unsignedIntValue]==state->revision
        && [_pokedexOwnerPictures[@"family"] unsignedIntValue]==state->family && [_pokedexOwnerPictures[@"phase"] unsignedIntValue]==state->phase;
    if (!state->visualCount) { if (!same) _pokedexOwnerPictures=nil; return _pokedexOwnerPictures; }
    // BUILD visuals are the producer's owned committed snapshot; input still uses the live masks/resolver.
    if (state->visualCount>HG_POKEDEX_UI_VISUALS || (!state->ready && state->busyReason!=HG_POKEDEX_UI_BUSY_DRAG
        && state->busyReason!=HG_POKEDEX_UI_BUSY_BUILD)) { _pokedexOwnerPictures=nil; return nil; }
    NSMutableArray *images=[NSMutableArray new];
    for (uint32_t i=0;i<state->visualCount;i++) {
        const HGNativePokedexUIVisual *visual=&state->visuals[i];
        if (!HGNativePokedexUIVisualIsValid(visual)) { _pokedexOwnerPictures=nil; return nil; }
        UIImage *image=visual->kind==HG_POKEDEX_UI_VISUAL_WINDOW ? OriginalWindowImage(&visual->source.window,_env->memory->data,_env->memory->size)
            : OriginalFrameRegion(visual->source.raster.engine ? sub : main,visual->source.raster.x,visual->source.raster.y,visual->source.raster.width,visual->source.raster.height);
        if (!image) { _pokedexOwnerPictures=nil; return nil; } [images addObject:image];
    }
    _pokedexOwnerPictures=@{@"images":[images copy],@"metadata":[NSData dataWithBytes:state length:sizeof(*state)],@"generation":@(state->ownerGeneration),@"revision":@(state->revision),@"family":@(state->family),@"phase":@(state->phase),@"frame":@(state->frameSerial)};
    return _pokedexOwnerPictures;
}
- (void)queuePokedexInput:(uint32_t)kind index:(uint32_t)index {
    const HGNativePokedexUIState *state=_displayedPokedexUIState.bytes; HGNativePokedexUIInput input;
    if (!_pokedexScene || _displayedPokedexUIState.length!=sizeof(*state)
        || !HGNativePokedexUIResolveInput(state,state->ownerGeneration,state->revision,state->phase,kind,index,0,&input)) return;
    [_condition lock];
    if (!_paused && !_stopping && !_pokedexChoicePending && !_pokedexInputActive && !_pokedexInputCooling && !_pokedexGestureAdmitted) {
        _pokedexChoiceGeneration=state->ownerGeneration; _pokedexChoiceRevision=state->revision; _pokedexChoicePhase=state->phase;
        _pokedexChoiceKind=kind; _pokedexChoiceIndex=index; _pokedexChoicePending=YES;
    }
    [_condition unlock];
}
- (void)pokedexControlPressed:(UIButton *)button {
    const HGNativePokedexUIState *state=_displayedPokedexUIState.bytes;
    if (_displayedPokedexUIState.length!=sizeof(*state) || button.tag<0 || button.tag>=state->controlCount || button.hidden || !button.enabled) return;
    uint32_t kind=state->controls[button.tag].transportKind==HG_POKEDEX_UI_TRANSPORT_TRACK ? HG_POKEDEX_UI_INPUT_GESTURE_BEGIN : HG_POKEDEX_UI_INPUT_CONTROL;
    [self queuePokedexInput:kind index:(uint32_t)button.tag];
}
- (void)pokedexGesture:(UIPanGestureRecognizer *)gesture {
    UIButton *button=(UIButton *)gesture.view; const HGNativePokedexUIState *state=_displayedPokedexUIState.bytes;
    BOOL began=gesture.state==UIGestureRecognizerStateBegan,live=began || gesture.state==UIGestureRecognizerStateChanged;
    if (began) {
        HGNativePokedexUIInput input;
        if (!_pokedexScene || _displayedPokedexUIState.length!=sizeof(*state) || button.tag<0 || button.tag>=state->controlCount
            || !HGNativePokedexUIResolveInput(state,state->ownerGeneration,state->revision,state->phase,HG_POKEDEX_UI_INPUT_GESTURE_BEGIN,(uint32_t)button.tag,0,&input)) return;
        [_condition lock]; BOOL idle=!_paused && !_stopping && !_pokedexChoicePending && !_pokedexInputActive && !_pokedexInputCooling && !_pokedexGestureAdmitted;
        if (idle) {
            const HGNativePokedexUITrack *track=&state->controls[button.tag].transport.track;
            _pokedexGestureControl=button.tag; _pokedexGestureLive=YES; _pokedexGesturePulse++;
            _pokedexGestureStartValue=(track->position-track->minimum)*65535u/(track->maximum-track->minimum); _pokedexGestureValue=_pokedexGestureStartValue;
        } [_condition unlock];
        if (!idle) return; [self queuePokedexInput:HG_POKEDEX_UI_INPUT_GESTURE_BEGIN index:(uint32_t)button.tag];
        [_condition lock];
        if (!_pokedexChoicePending && !_pokedexGestureAdmitted && !(_pokedexInputActive && _pokedexInputKind==HG_POKEDEX_UI_INPUT_GESTURE_BEGIN)) { _pokedexGestureLive=NO; _pokedexGestureControl=-1; }
        [_condition unlock];
    }
    [_condition lock];
    if (_pokedexGestureControl==button.tag) {
        _pokedexGestureLive=live; _pokedexGesturePulse++;
        if (live && (_pokedexGestureAdmitted || (_displayedPokedexUIState.length==sizeof(*state) && button.tag<state->controlCount))) {
            uint32_t axis=_pokedexGestureAdmitted ? _pokedexGestureTrack.axis : state->controls[button.tag].transport.track.axis;
            CGPoint delta=[gesture translationInView:button]; CGFloat extent=axis==HG_POKEDEX_UI_AXIS_X ? button.bounds.size.width : button.bounds.size.height;
            CGFloat position=axis==HG_POKEDEX_UI_AXIS_X ? delta.x : delta.y;
            if (!isfinite(position) || !isfinite(extent)) _pokedexGestureLive=NO;
            else _pokedexGestureValue=(uint32_t)llround(MAX(0,MIN(65535,_pokedexGestureStartValue+position*65535/MAX(1,extent))));
        }
        if (!live && !_pokedexGestureAdmitted && _pokedexChoiceKind==HG_POKEDEX_UI_INPUT_GESTURE_BEGIN) _pokedexChoicePending=NO;
    }
    [_condition unlock];
}
- (void)takePokedexInputLocked:(const HGNativePokedexUIState *)state blocked:(BOOL)blocked reads:(uint32_t)reads
    keys:(uint32_t *)keys x:(uint32_t *)x y:(uint32_t *)y pressed:(BOOL *)pressed {
    if (!state->active || blocked) { [self clearPokedexInputLocked]; return; }
    _keys=_pendingKeys=_submittedKeys=0; _touchPressed=NO; _touchX=_touchY=0; *keys=*x=*y=0; *pressed=NO;
    if (_pokedexInputCooling && reads-_pokedexReleaseReads>=2) _pokedexInputCooling=NO;
    if (_pokedexChoicePending) {
        BOOL same=state->ownerGeneration==_pokedexChoiceGeneration && state->revision==_pokedexChoiceRevision && state->phase==_pokedexChoicePhase;
        if (!same) { _pokedexChoicePending=NO; _pokedexGestureLive=NO; }
        else if (state->ready) {
            HGNativePokedexUIInput input;
            if (HGNativePokedexUIResolveInput(state,_pokedexChoiceGeneration,_pokedexChoiceRevision,_pokedexChoicePhase,_pokedexChoiceKind,_pokedexChoiceIndex,0,&input)) {
                _pokedexInput=input; _pokedexInputGeneration=state->ownerGeneration; _pokedexInputRevision=state->revision; _pokedexInputPhase=state->phase;
                _pokedexInputFamily=state->family; _pokedexInputKind=_pokedexChoiceKind; _pokedexInputIndex=_pokedexChoiceIndex; _pokedexInputReads=reads; _pokedexInputActive=YES;
                if (_pokedexChoiceKind==HG_POKEDEX_UI_INPUT_GESTURE_BEGIN && _pokedexGestureLive) {
                    // Admission is recorded only as this resolved BEGIN is actually sampled.
                    _pokedexGestureAdmitted=YES; _pokedexGestureContinued=NO; _pokedexGestureTrack=state->controls[_pokedexChoiceIndex].transport.track;
                    _pokedexGestureCommitReads=reads; _pokedexSampledGesturePulse=_pokedexGesturePulse;
                }
            } else _pokedexGestureLive=NO;
            _pokedexChoicePending=NO;
        }
    }
    BOOL release=NO;
    if (_pokedexGestureAdmitted) {
        if (_pokedexSampledGesturePulse!=_pokedexGesturePulse) { _pokedexSampledGesturePulse=_pokedexGesturePulse; _pokedexInputReads=reads; }
        release=!_pokedexGestureLive || state->ownerGeneration!=_pokedexInputGeneration || state->family!=_pokedexInputFamily || reads-_pokedexInputReads>=4;
        if (!release && state->continuation.kind==_pokedexGestureTrack.kind && state->continuation.handle==_pokedexGestureTrack.handle) {
            HGNativePokedexUIInput input;
            release=!HGNativePokedexUIResolveInput(state,state->ownerGeneration,state->revision,state->phase,HG_POKEDEX_UI_INPUT_GESTURE_UPDATE,_pokedexGestureTrack.handle,_pokedexGestureValue,&input);
            if (!release) { _pokedexGestureContinued=YES; _pokedexInput=input; _pokedexGestureCommitReads=reads; *keys=input.keys; *x=input.touchX; *y=input.touchY; *pressed=input.touchPressed!=0; return; }
        } else if (!release) {
            BOOL originalBeginGap=(!_pokedexGestureContinued && state->phase==_pokedexInputPhase && state->revision==_pokedexInputRevision)
                || (state->phase==HGNativePokedexUITrackPhase(&_pokedexGestureTrack)
                    && (state->busyReason==HG_POKEDEX_UI_BUSY_BUILD || state->busyReason==HG_POKEDEX_UI_BUSY_GRAPHICS));
            // No new UPDATE is admitted here: retain the last resolved sample for one bounded commit gap.
            release=!originalBeginGap || reads-_pokedexGestureCommitReads>=4;
        }
        if (!release) { *keys=_pokedexInput.keys; *x=_pokedexInput.touchX; *y=_pokedexInput.touchY; *pressed=_pokedexInput.touchPressed!=0; return; }
    } else if (_pokedexInputActive) {
        release=state->ownerGeneration!=_pokedexInputGeneration || state->revision!=_pokedexInputRevision || state->phase!=_pokedexInputPhase || reads-_pokedexInputReads>=4;
        HGNativePokedexUIInput input;
        if (!release && state->ready) release=!HGNativePokedexUIResolveInput(state,_pokedexInputGeneration,_pokedexInputRevision,_pokedexInputPhase,_pokedexInputKind,_pokedexInputIndex,0,&input);
        if (!release) { *keys=_pokedexInput.keys; *x=_pokedexInput.touchX; *y=_pokedexInput.touchY; *pressed=_pokedexInput.touchPressed!=0; }
    }
    if (release) {
        _pokedexInputActive=_pokedexGestureLive=_pokedexGestureAdmitted=_pokedexGestureContinued=NO; _pokedexGestureControl=-1;
        _pokedexInputCooling=YES; _pokedexReleaseReads=reads; memset(&_pokedexInput,0,sizeof(_pokedexInput)); memset(&_pokedexGestureTrack,0,sizeof(_pokedexGestureTrack));
    }
}
- (NSString *)pokedexLabelForRole:(uint32_t)role index:(uint32_t)index state:(const HGNativePokedexUIState *)state visual:(BOOL)visual {
    if (visual) {
        if (state->family!=HG_POKEDEX_UI_FAMILY_OVERVIEW && (role==HG_POKEDEX_UI_IMAGE_SELECTED_NAME || role==HG_POKEDEX_UI_IMAGE_PORTRAIT) && state->caughtState && state->species<_speciesNames.count) return _speciesNames[state->species];
        return nil;
    }
    if (role==HG_POKEDEX_UI_ROLE_GRID_CELL && index<HG_POKEDEX_UI_ENTRIES) {
        const HGNativePokedexUIEntry *entry=&state->entries[index];
        if (entry->caughtState && entry->species<_speciesNames.count) return _speciesNames[entry->species];
    }
    NSString *key=[NSString stringWithFormat:@"%u/%u/%u",state->family,role,index];
    id controls=_pokedexLabels[@"controls"];
    NSString *label=[controls isKindOfClass:NSDictionary.class] ? controls[key] : nil;
    if ([label isKindOfClass:NSString.class] && label.length) return label;
    if (role==HG_POKEDEX_UI_ROLE_EXIT || role==HG_POKEDEX_UI_ROLE_CANCEL || role==HG_POKEDEX_UI_ROLE_ENTRY_BACK) return @"返回";
    if (role==HG_POKEDEX_UI_ROLE_ACCEPT) return @"确认";
    return @"—";
}
- (void)updatePokedexControls:(NSData *)data pictures:(NSDictionary *)pictures blocked:(BOOL)blocked paused:(BOOL)paused {
    const HGNativePokedexUIState *state=data.bytes,*old=_displayedPokedexUIState.bytes;
    BOOL scene=data.length==sizeof(*state) && state->active==1 && !blocked;
    BOOL ownerChanged=_displayedPokedexUIState.length!=sizeof(*state) || (scene && (old->ownerGeneration!=state->ownerGeneration || old->family!=state->family));
    BOOL changed=_pokedexScene!=scene || _displayedPokedexUIState.length!=data.length
        || (scene && (old->ownerGeneration!=state->ownerGeneration || old->revision!=state->revision || old->phase!=state->phase || old->family!=state->family));
    if (changed) { [_condition lock]; if (!scene || ownerChanged) [self clearPokedexInputLocked]; [_condition unlock]; }
    _pokedexScene=scene; _displayedPokedexUIState=data; _pokedexPanel.hidden=!scene; _pokedexPanel.userInteractionEnabled=!paused;
    id families=_pokedexLabels[@"families"];
    NSString *familyLabel=scene && [families isKindOfClass:NSArray.class] && state->family<[families count] ? families[state->family] : nil;
    _pokedexPanel.accessibilityLabel=[familyLabel isKindOfClass:NSString.class] ? familyLabel : nil;
    if (!scene) {
        _pokedexDisplayPictures=nil;
        for (NSUInteger i=0;i<_pokedexImages.count;i++) { _pokedexImages[i].image=nil; _pokedexImages[i].hidden=YES; }
        for (NSUInteger i=0;i<_pokedexButtons.count;i++) { _pokedexButtons[i].hidden=YES; _pokedexPans[i].enabled=NO; }
        if (changed) { if (!blocked) for (NSNumber *key in @[@64,@128,@32,@16,@1,@2]) { _buttons[key].enabled=YES; _buttons[key].alpha=1; } [self.view setNeedsLayout]; }
        return;
    }
    if (changed && ownerChanged) { _pokedexContent.contentOffset=CGPointZero; _pokedexDetails.contentOffset=CGPointZero; }
    BOOL matching=[pictures[@"generation"] unsignedIntValue]==state->ownerGeneration && [pictures[@"revision"] unsignedIntValue]==state->revision
        && [pictures[@"family"] unsignedIntValue]==state->family && [pictures[@"phase"] unsignedIntValue]==state->phase;
    _pokedexDisplayPictures=matching ? pictures : nil;
    NSArray *images=_pokedexDisplayPictures[@"images"]; NSData *metadata=_pokedexDisplayPictures[@"metadata"];
    UIImage *passiveMessage=_pokedexDisplayPictures[@"passiveMessage"];
    const HGNativePokedexUIState *visualState=metadata.length==sizeof(*visualState) ? metadata.bytes : NULL;
    for (NSUInteger i=0;i<_pokedexImages.count;i++) {
        UIImageView *image=_pokedexImages[i]; BOOL present=visualState && i<MIN(visualState->visualCount,HG_POKEDEX_UI_VISUALS) && i<images.count && [images[i] isKindOfClass:UIImage.class];
        BOOL passive=visualState && i==0 && [passiveMessage isKindOfClass:UIImage.class];
        BOOL startGutter=present && visualState->family==HG_POKEDEX_UI_FAMILY_LIST && visualState->visuals[i].role==HG_POKEDEX_UI_IMAGE_FOOTER && visualState->visuals[i].index==10;
        image.image=passive ? passiveMessage : present ? images[i] : nil; image.hidden=!(present || passive) || startGutter;
        image.accessibilityLabel=passive ? @"图鉴搜索提示" : present ? [self pokedexLabelForRole:visualState->visuals[i].role index:visualState->visuals[i].index state:state visual:YES] : nil;
        image.isAccessibilityElement=image.accessibilityLabel.length!=0;
    }
    [_condition lock];
    BOOL waiting=_pokedexChoicePending || _pokedexInputActive || _pokedexInputCooling || _pokedexGestureAdmitted;
    NSInteger gestureControl=_pokedexGestureControl;
    BOOL continuing=_pokedexGestureLive && _pokedexGestureAdmitted && state->ownerGeneration==_pokedexInputGeneration && state->family==_pokedexInputFamily
        && state->continuation.kind==_pokedexGestureTrack.kind && state->continuation.handle==_pokedexGestureTrack.handle;
    uint32_t gestureHandle=_pokedexGestureTrack.handle;
    BOOL originalBeginGap=(!_pokedexGestureContinued && state->phase==_pokedexInputPhase && state->revision==_pokedexInputRevision)
        || (state->phase==HGNativePokedexUITrackPhase(&_pokedexGestureTrack) && (state->busyReason==HG_POKEDEX_UI_BUSY_BUILD || state->busyReason==HG_POKEDEX_UI_BUSY_GRAPHICS));
    BOOL gestureAlive=continuing || (_pokedexGestureLive && _pokedexGestureAdmitted
        && state->ownerGeneration==_pokedexInputGeneration && state->family==_pokedexInputFamily && originalBeginGap);
    if (gestureControl>=0 && gestureControl<_pokedexPans.count && _pokedexGestureLive) {
        UIGestureRecognizerState phase=_pokedexPans[gestureControl].state;
        if (phase==UIGestureRecognizerStateBegan || phase==UIGestureRecognizerStateChanged) _pokedexGesturePulse++;
    }
    [_condition unlock];
    for (NSUInteger i=0;i<_pokedexButtons.count;i++) {
        UIButton *button=_pokedexButtons[i]; UIPanGestureRecognizer *pan=_pokedexPans[i];
        if (i>=MIN(state->controlCount,HG_POKEDEX_UI_CONTROLS) || !visualState) {
            // Keep only the already admitted UIKit drag alive; this does not authorize a new control.
            button.hidden=!(gestureAlive && gestureControl==(NSInteger)i); button.enabled=!button.hidden && !paused;
            pan.enabled=!button.hidden; button.accessibilityTraits=UIAccessibilityTraitButton|UIAccessibilityTraitNotEnabled;
            if (!button.hidden && visualState) for (uint32_t v=0;v<MIN(visualState->visualCount,HG_POKEDEX_UI_VISUALS);v++) {
                if (visualState->visuals[v].role==HG_POKEDEX_UI_IMAGE_TRACK && visualState->visuals[v].index==gestureHandle && v<images.count && [images[v] isKindOfClass:UIImage.class]) {
                    UIButtonConfiguration *config=UIButtonConfiguration.filledButtonConfiguration; config.contentInsets=NSDirectionalEdgeInsetsMake(3,4,3,4);
                    config.baseBackgroundColor=[UIColor colorWithRed:0.16 green:0.22 blue:0.26 alpha:1]; OriginalSetButtonContent(button,config,images[v],nil); _pokedexImages[v].hidden=YES; break;
                }
            }
            continue;
        }
        const HGNativePokedexUIControl *control=&state->controls[i];
        UIImage *image=control->visualId<images.count && [images[control->visualId] isKindOfClass:UIImage.class] ? images[control->visualId] : nil;
        HGNativePokedexUIInput input; BOOL track=control->transportKind==HG_POKEDEX_UI_TRANSPORT_TRACK;
        BOOL enabled=!paused && !waiting && HGNativePokedexUIResolveInput(state,state->ownerGeneration,state->revision,state->phase,
            track ? HG_POKEDEX_UI_INPUT_GESTURE_BEGIN : HG_POKEDEX_UI_INPUT_CONTROL,(uint32_t)i,0,&input);
        BOOL keepGesture=gestureAlive && gestureControl==(NSInteger)i && !paused;
        button.hidden=!image; button.enabled=enabled || keepGesture; button.alpha=button.enabled ? 1 : 0.5; pan.enabled=track && button.enabled;
        UIButtonConfiguration *config=UIButtonConfiguration.filledButtonConfiguration; config.contentInsets=NSDirectionalEdgeInsetsMake(3,4,3,4);
        config.baseBackgroundColor=[UIColor colorWithRed:0.16 green:0.22 blue:0.26 alpha:1]; config.baseForegroundColor=UIColor.whiteColor;
        BOOL keyArrow=control->role==HG_POKEDEX_UI_ROLE_DEX_MODE || (control->role==HG_POKEDEX_UI_ROLE_ENTRY && (state->family==HG_POKEDEX_UI_FAMILY_AREA || state->family==HG_POKEDEX_UI_FAMILY_FORMS));
        BOOL overviewKey=state->family==HG_POKEDEX_UI_FAMILY_LIST && control->role==HG_POKEDEX_UI_ROLE_OPEN_LIST;
        NSString *arrow=control->role==HG_POKEDEX_UI_ROLE_DEX_MODE ? (control->index ? @"→" : @"←") : state->family==HG_POKEDEX_UI_FAMILY_FORMS ? @"→" : @"←";
        OriginalSetButtonContent(button,config,keyArrow || overviewKey ? nil : image,keyArrow ? arrow : overviewKey ? [self pokedexLabelForRole:control->role index:control->index state:state visual:NO] : nil);
        button.layer.borderColor=((control->flags&HG_POKEDEX_UI_CONTROL_SELECTED) ? UIColor.systemOrangeColor : UIColor.clearColor).CGColor;
        button.accessibilityLabel=[self pokedexLabelForRole:control->role index:control->index state:state visual:NO];
        button.accessibilityTraits=UIAccessibilityTraitButton|((control->flags&HG_POKEDEX_UI_CONTROL_SELECTED) ? UIAccessibilityTraitSelected : 0)|(!enabled ? UIAccessibilityTraitNotEnabled : 0);
        if (!keyArrow && control->visualId<_pokedexImages.count) _pokedexImages[control->visualId].hidden=YES;
    }
    if (changed) [self.view setNeedsLayout];
}
- (void)layoutPokedexControls:(CGRect)area {
    if (!_pokedexScene) return;
    const HGNativePokedexUIState *state=_displayedPokedexUIState.bytes;
    if (_displayedPokedexUIState.length!=sizeof(*state)) return;
    CGFloat keyWidth=104,gap=8,pw=MAX(1,area.size.width-keyWidth-gap),ph=area.size.height;
    _pokedexPanel.frame=CGRectMake(area.origin.x,area.origin.y,pw,ph);
    [_condition lock]; BOOL idle=!_pokedexChoicePending && !_pokedexInputActive && !_pokedexInputCooling && !_pokedexGestureAdmitted,canResume=_interrupted && _interruptionEnded; [_condition unlock];
    const uint32_t keyBits[]={64,128,32,16,1,2}; NSArray *keyNames=@[@"↑",@"↓",@"←",@"→",@"确认",@"返回"];
    for (NSUInteger i=0;i<6;i++) {
        UIButton *button=_buttons[@(keyBits[i])]; HGNativePokedexUIInput input;
        uint32_t kind=i<4 ? HG_POKEDEX_UI_INPUT_DIRECTION : i==4 ? HG_POKEDEX_UI_INPUT_CONFIRM : HG_POKEDEX_UI_INPUT_BACK;
        button.hidden=!(state->keyEnabledMask&(1u<<i));
        button.enabled=_pokedexPanel.userInteractionEnabled && idle && HGNativePokedexUIResolveInput(state,state->ownerGeneration,state->revision,state->phase,kind,i<4 ? (uint32_t)i : 0,0,&input);
        button.alpha=button.enabled ? 1 : 0.5;
        CGFloat x=CGRectGetMaxX(area)-keyWidth+(i<4 ? (i%2)*52 : 0),y=area.origin.y+MAX(0,(ph-200)/2)+(i<4 ? (i/2)*52 : 104+(i-4)*48);
        button.frame=CGRectMake(x,y,i<4 ? 48 : keyWidth,44);
        OriginalSetButtonContent(button,nil,nil,keyNames[i]);
        button.accessibilityLabel=keyNames[i];
    }
    _buttons[@8].hidden=!canResume; _buttons[@0x400].hidden=_menuButton.hidden=YES;
    if (canResume) { _buttons[@8].frame=CGRectMake(CGRectGetMaxX(area)-keyWidth,area.origin.y,keyWidth,44); _buttons[@8].enabled=YES; _buttons[@8].alpha=1; }
    NSData *metadata=_pokedexDisplayPictures[@"metadata"]; const HGNativePokedexUIState *visualState=metadata.length==sizeof(*visualState) ? metadata.bytes : NULL;
    if (!visualState) return;
    if ([_pokedexDisplayPictures[@"passiveMessage"] isKindOfClass:UIImage.class]) {
        UIImageView *image=_pokedexImages[0]; CGFloat scale=MIN(2,MIN(pw/224,ph/40));
        if (image.superview!=_pokedexPanel) [_pokedexPanel addSubview:image];
        image.frame=CGRectMake((pw-224*scale)/2,(ph-40*scale)/2,224*scale,40*scale);
        _pokedexDetails.hidden=YES;
        return;
    }
    uint32_t family=state->family; BOOL list=family==HG_POKEDEX_UI_FAMILY_LIST || family==HG_POKEDEX_UI_FAMILY_SEARCH_RESULTS;
    BOOL filters=family>=HG_POKEDEX_UI_FAMILY_SEARCH_MENU && family<=HG_POKEDEX_UI_FAMILY_SEARCH_BODY;
    BOOL forms=family==HG_POKEDEX_UI_FAMILY_FORMS,areaPage=family==HG_POKEDEX_UI_FAMILY_AREA;
    BOOL size=family==HG_POKEDEX_UI_FAMILY_SIZE,compare=family==HG_POKEDEX_UI_FAMILY_COMPARE,entry=family==HG_POKEDEX_UI_FAMILY_ENTRY;
    CGFloat detailW=(list || forms) ? pw*0.43 : filters ? MIN(220,pw*0.33) : areaPage ? pw*0.39 : 0;
    CGFloat contentW=MAX(1,pw-detailW-(detailW ? gap : 0)),footerWidth=list ? contentW : pw;
    NSUInteger footerCount=0,footerColumns=MAX(1,(NSUInteger)floor((footerWidth+gap)/104));
    for (uint32_t i=0;i<MIN(state->controlCount,HG_POKEDEX_UI_CONTROLS);i++) {
        uint32_t role=state->controls[i].role;
        if (role==HG_POKEDEX_UI_ROLE_OPEN_LIST || role==HG_POKEDEX_UI_ROLE_OPEN_SEARCH || role==HG_POKEDEX_UI_ROLE_EXIT
            || role==HG_POKEDEX_UI_ROLE_CRY || role==HG_POKEDEX_UI_ROLE_LANGUAGE || role==HG_POKEDEX_UI_ROLE_ENTRY
            || role==HG_POKEDEX_UI_ROLE_RESET || role==HG_POKEDEX_UI_ROLE_SEARCH || role==HG_POKEDEX_UI_ROLE_ACCEPT || role==HG_POKEDEX_UI_ROLE_CANCEL
            || role==HG_POKEDEX_UI_ROLE_TAB || role==HG_POKEDEX_UI_ROLE_FACING || role==HG_POKEDEX_UI_ROLE_DEX_MODE
            || (compare && role==HG_POKEDEX_UI_ROLE_ENTRY_BACK) || (list && role==HG_POKEDEX_UI_ROLE_PAGE)) footerCount++;
    }
    // Keep the same track geometry when the original drag continuation removes normal controls.
    if (list) footerCount=MAX(footerCount,9u);
    else if (filters) footerCount=MAX(footerCount,family==HG_POKEDEX_UI_FAMILY_SEARCH_MENU ? 3u : 2u);
    else if (areaPage || forms) footerCount=MAX(footerCount,5u);
    CGFloat footer=MAX(56,((footerCount+footerColumns-1)/footerColumns)*48+8),bodyH=MAX(44,ph-footer),detailH=list ? ph : bodyH;
    _pokedexContent.frame=CGRectMake(filters ? detailW+gap : 0,0,contentW,bodyH);
    _pokedexDetails.frame=CGRectMake(filters ? 0 : contentW+gap,0,detailW,detailH); _pokedexDetails.hidden=detailW==0;
    CGFloat detailY=8,mainY=8,contentBottom=bodyH,detailBottom=detailH,contentRight=contentW,listDetailsY=220,listLanguageY=280;
    CGFloat summaryRow=MAX(24,MIN(44,(detailH-96)/7)); NSUInteger footerIndex=0;
    NSMutableArray<NSNumber *> *order=[NSMutableArray new];
    BOOL overviewMode=NO;
    for (uint32_t i=0;i<MIN(visualState->visualCount,HG_POKEDEX_UI_VISUALS);i++) {
        [order addObject:@(i)];overviewMode|=family==HG_POKEDEX_UI_FAMILY_OVERVIEW && visualState->visuals[i].role==HG_POKEDEX_UI_IMAGE_FOOTER && visualState->visuals[i].index==10;
    }
    [order sortUsingComparator:^NSComparisonResult(NSNumber *a,NSNumber *b) {
        const HGNativePokedexUIVisual *x=&visualState->visuals[a.unsignedIntValue],*y=&visualState->visuals[b.unsignedIntValue];
        uint32_t (^rank)(uint32_t)=^uint32_t(uint32_t role) { switch (role) {
            case HG_POKEDEX_UI_IMAGE_SELECTED_NAME:return 0; case HG_POKEDEX_UI_IMAGE_HEADER:return 1;
            case HG_POKEDEX_UI_IMAGE_PORTRAIT:return 2; case HG_POKEDEX_UI_IMAGE_TYPE:return 3;
            case HG_POKEDEX_UI_IMAGE_CATEGORY:return 4; case HG_POKEDEX_UI_IMAGE_FLAVOR:return 5; default:return 6+role; } };
        uint32_t rx=rank(x->role),ry=rank(y->role); return rx==ry ? (x->index>y->index ? NSOrderedDescending : x->index<y->index ? NSOrderedAscending : NSOrderedSame) : rx>ry ? NSOrderedDescending : NSOrderedAscending;
    }];
    for (NSNumber *number in order) {
        NSUInteger i=number.unsignedIntegerValue; const HGNativePokedexUIVisual *visual=&visualState->visuals[i]; UIImageView *image=_pokedexImages[i];
        uint32_t role=visual->role,index=visual->index,cRole=0,cIndex=0;
        for (uint32_t c=0;c<MIN(state->controlCount,HG_POKEDEX_UI_CONTROLS);c++) if (state->controls[c].visualId==i) { cRole=state->controls[c].role; cIndex=state->controls[c].index; break; }
        UIView *parent=detailW ? _pokedexDetails : _pokedexContent; CGFloat width=detailW ? detailW-16 : contentW-16;
        CGFloat natural=image.image.size.width ? image.image.size.height*width/image.image.size.width : 20;
        CGFloat height=MAX(20,MIN(role==HG_POKEDEX_UI_IMAGE_PORTRAIT ? 112 : 64,natural));
        CGRect rect=CGRectMake(8,detailW ? detailY : mainY,width,height); BOOL manual=NO;
        BOOL footerControl=cRole==HG_POKEDEX_UI_ROLE_OPEN_LIST || cRole==HG_POKEDEX_UI_ROLE_OPEN_SEARCH || cRole==HG_POKEDEX_UI_ROLE_EXIT
            || cRole==HG_POKEDEX_UI_ROLE_CRY || cRole==HG_POKEDEX_UI_ROLE_LANGUAGE || (cRole==HG_POKEDEX_UI_ROLE_ENTRY && !areaPage && !forms)
            || cRole==HG_POKEDEX_UI_ROLE_RESET || cRole==HG_POKEDEX_UI_ROLE_SEARCH || cRole==HG_POKEDEX_UI_ROLE_ACCEPT || cRole==HG_POKEDEX_UI_ROLE_CANCEL
            || cRole==HG_POKEDEX_UI_ROLE_TAB || cRole==HG_POKEDEX_UI_ROLE_FACING
            || (compare && cRole==HG_POKEDEX_UI_ROLE_ENTRY_BACK);
        BOOL footerVisual=role==HG_POKEDEX_UI_IMAGE_TAB || (role==HG_POKEDEX_UI_IMAGE_FOOTER && family!=HG_POKEDEX_UI_FAMILY_OVERVIEW)
            || (list && ((role==HG_POKEDEX_UI_IMAGE_LANGUAGE && index>=2) || role==HG_POKEDEX_UI_IMAGE_ARROW));
        if (footerControl || footerVisual || (list && cRole==HG_POKEDEX_UI_ROLE_PAGE)) {
            NSUInteger columns=footerColumns; CGFloat cw=(footerWidth-(columns+1)*gap)/columns;
            rect=CGRectMake(gap+(footerIndex%columns)*(cw+gap),bodyH+4+(footerIndex/columns)*48,cw,44); footerIndex++; parent=_pokedexPanel; manual=YES;
        } else if (family==HG_POKEDEX_UI_FAMILY_OVERVIEW && role==HG_POKEDEX_UI_IMAGE_HEADER) {
            rect=CGRectMake(8,4,MAX(0,contentW-(overviewMode ? 152 : 16)),32);manual=YES;
        } else if (family==HG_POKEDEX_UI_FAMILY_OVERVIEW && role==HG_POKEDEX_UI_IMAGE_PORTRAIT) {
            CGFloat width=MAX(0,MIN(contentW-16,MAX(0,bodyH-112)*256/160));
            rect=CGRectMake((contentW-width)/2,108,width,width*160/256);manual=YES;
        } else if (family==HG_POKEDEX_UI_FAMILY_OVERVIEW && (role==HG_POKEDEX_UI_IMAGE_COUNT_LABEL || role==HG_POKEDEX_UI_IMAGE_COUNT_NUMBER)) {
            uint32_t column=role==HG_POKEDEX_UI_IMAGE_COUNT_LABEL ? index : index>=4;
            CGFloat cw=(contentW-3*gap)/2; rect=CGRectMake(gap+column*(cw+gap),role==HG_POKEDEX_UI_IMAGE_COUNT_LABEL ? 40 : 76,cw,28); manual=YES;
        } else if (family==HG_POKEDEX_UI_FAMILY_OVERVIEW && role==HG_POKEDEX_UI_IMAGE_FOOTER) {
            rect=CGRectMake(MAX(8,contentW-136),4,MAX(0,MIN(128,contentW-16)),28); manual=YES;
        } else if (role==HG_POKEDEX_UI_IMAGE_GRID_CELL && list) {
            CGFloat top=family==HG_POKEDEX_UI_FAMILY_SEARCH_RESULTS ? 36 : gap;
            CGFloat cw=MAX(44,(contentW-52-4*gap)/5),ch=MAX(44,(bodyH-top-3*gap)/3);
            rect=CGRectMake(gap+(index%5)*(cw+gap),top+(index/5)*(ch+gap),cw,ch); parent=_pokedexContent; manual=YES;
        } else if (list) {
            parent=_pokedexDetails; CGFloat cw=detailW-16;
            if (role==HG_POKEDEX_UI_IMAGE_TRACK) { parent=_pokedexContent; rect=CGRectMake(MAX(0,contentW-44),8,44,MAX(132,bodyH-16)); manual=YES; }
            else if (role==HG_POKEDEX_UI_IMAGE_SELECTED_NAME) { rect=CGRectMake(8,4,cw,24); manual=YES; }
            else if (role==HG_POKEDEX_UI_IMAGE_HEADER && visual->kind==HG_POKEDEX_UI_VISUAL_WINDOW) {
                uint32_t part=index>=81 ? index-81 : index>=7 ? index-7 : 0;
                if (part==2) image.hidden=YES; // The selected-name glyph above already carries this same original name.
                rect=CGRectMake(8+(part%2)*cw/2,part==3 ? 56 : 32,cw/2,20); manual=YES;
            } else if (role==HG_POKEDEX_UI_IMAGE_PORTRAIT) { rect=CGRectMake(8,56,cw*0.46,108); manual=YES; }
            else if (role==HG_POKEDEX_UI_IMAGE_TYPE) { rect=CGRectMake(12+cw*0.46,88+index*28,cw*0.5,24); manual=YES; }
            else if (role==HG_POKEDEX_UI_IMAGE_CATEGORY) { rect=CGRectMake(12+cw*0.46,56,cw*0.5,24); manual=YES; }
            else if (role==HG_POKEDEX_UI_IMAGE_FOOTPRINT || role==HG_POKEDEX_UI_IMAGE_CAUGHT) { rect=CGRectMake(12+cw*0.46+(role==HG_POKEDEX_UI_IMAGE_CAUGHT ? 40 : 0),144,32,24); manual=YES; }
            else if (role==HG_POKEDEX_UI_IMAGE_FLAVOR) { rect=CGRectMake(8,172,cw,MAX(44,natural)); listDetailsY=MAX(220,CGRectGetMaxY(rect)+8); manual=YES; }
            else if (role==HG_POKEDEX_UI_IMAGE_MEASURE_VALUE && visual->kind==HG_POKEDEX_UI_VISUAL_WINDOW) {
                uint32_t first=(family==HG_POKEDEX_UI_FAMILY_SEARCH_RESULTS ? 81 : 7)+(state->language==2 ? 5 : 9),part=index-first;
                if (state->language==2) rect=CGRectMake(8+(part&1 ? cw*0.35 : 0),listDetailsY+(part/2)*24,cw*(part&1 ? 0.65 : 0.35),24);
                else { // Original foreign branch: dex label, dex number/name, then category; not height/weight.
                    rect=CGRectMake(8+(part==2 ? cw*0.25 : 0),listDetailsY+(part==0 ? 0 : part==3 ? 48 : 24),part==1 ? cw*0.25 : part==2 ? cw*0.75 : cw,24);
                }
                listLanguageY=MAX(listLanguageY,CGRectGetMaxY(rect)+12); manual=YES;
            } else if (role==HG_POKEDEX_UI_IMAGE_LANGUAGE) { rect=CGRectMake(index ? 48 : 8,listLanguageY,index ? MAX(32,cw-40) : 32,24); manual=YES; }
            else if (role==HG_POKEDEX_UI_IMAGE_HELP) { rect=CGRectMake(8,32,cw,48); manual=YES; }
            else if (role==HG_POKEDEX_UI_IMAGE_CRITERION_LABEL) { rect=CGRectMake(8,88+(index-1)*summaryRow,cw*0.42,summaryRow); manual=YES; }
            else if (role==HG_POKEDEX_UI_IMAGE_CRITERION_VALUE || role==HG_POKEDEX_UI_IMAGE_BODY_SHAPE) {
                uint32_t criterion=role==HG_POKEDEX_UI_IMAGE_BODY_SHAPE ? 6 : index<=9 ? index-8 : index<=11 ? 2 : index<=13 ? 3 : index<=15 ? 4 : 5;
                BOOL second=index==11 || index==13 || index==15;
                rect=CGRectMake(8+cw*0.44,88+criterion*summaryRow+(second ? summaryRow/2 : 0),cw*0.56,role==HG_POKEDEX_UI_IMAGE_BODY_SHAPE ? summaryRow : summaryRow/2); manual=YES;
            }
            else { rect=CGRectMake(8,276+detailY,cw,height); detailY+=height+4; manual=YES; }
        } else if (role==HG_POKEDEX_UI_IMAGE_TRACK && forms) {
            rect=CGRectMake(MAX(0,contentW-44),60,44,MAX(132,bodyH-120)); parent=_pokedexContent; manual=YES;
        } else if (filters) {
            BOOL sub=visual->kind==HG_POKEDEX_UI_VISUAL_WINDOW ? visual->source.window.engine!=0 : visual->source.raster.engine!=0;
            if (sub) {
                if (family==HG_POKEDEX_UI_FAMILY_SEARCH_MENU && (role==HG_POKEDEX_UI_IMAGE_CRITERION_LABEL || role==HG_POKEDEX_UI_IMAGE_CRITERION_VALUE || role==HG_POKEDEX_UI_IMAGE_BODY_SHAPE)) image.hidden=YES;
                CGFloat cw=detailW-16;
                if (role==HG_POKEDEX_UI_IMAGE_HELP) rect=CGRectMake(8,4,cw,48);
                else if (role==HG_POKEDEX_UI_IMAGE_CRITERION_LABEL) rect=CGRectMake(8,60+(index-1)*52,cw*0.42,44);
                else {
                    uint32_t criterion=role==HG_POKEDEX_UI_IMAGE_BODY_SHAPE ? 6 : index<=9 ? index-8 : index<=11 ? 2 : index<=13 ? 3 : index<=15 ? 4 : 5;
                    BOOL second=index==11 || index==13 || index==15;
                    rect=CGRectMake(8+cw*0.44,60+criterion*52+(second ? 22 : 0),cw*0.56,role==HG_POKEDEX_UI_IMAGE_BODY_SHAPE ? 44 : 22);
                }
                manual=YES;
            } else {
                parent=_pokedexContent; width=contentW-16;
                if (cRole==HG_POKEDEX_UI_ROLE_OPTION || (role==HG_POKEDEX_UI_IMAGE_OPTION && visual->kind==HG_POKEDEX_UI_VISUAL_RASTER)) {
                    NSUInteger columns=family==HG_POKEDEX_UI_FAMILY_SEARCH_NAME ? 7 : family==HG_POKEDEX_UI_FAMILY_SEARCH_TYPE ? 4 : family==HG_POKEDEX_UI_FAMILY_SEARCH_BODY ? 5 : 2;
                    uint32_t option=cRole ? cIndex : index; CGFloat cw=MAX(44,(contentW-(columns+1)*gap)/columns);
                    rect=CGRectMake(gap+(option%columns)*(cw+gap),64+(option/columns)*52,cw,44); manual=YES;
                } else if (family==HG_POKEDEX_UI_FAMILY_SEARCH_MENU && visual->kind==HG_POKEDEX_UI_VISUAL_WINDOW && index>=20 && index<=35) {
                    uint32_t criterion=index<=26 ? index-20 : index==27 ? 0 : index==28 ? 1 : index<=30 ? 2 : index<=32 ? 3 : index<=34 ? 4 : 5;
                    CGFloat cw=MAX(128,(contentW-3*gap)/2),x=gap+(criterion%2)*(cw+gap),y=8+(criterion/2)*60;
                    BOOL label=index<=26; rect=CGRectMake(x+(label ? 0 : cw*0.44),y+(label ? 0 : (index==30 || index==32 || index==34 ? 22 : 0)),label ? cw*0.42 : cw*0.56,label ? 44 : 22); manual=YES;
                } else if (family==HG_POKEDEX_UI_FAMILY_SEARCH_MENU && role==HG_POKEDEX_UI_IMAGE_BODY_SHAPE) {
                    CGFloat cw=MAX(128,(contentW-3*gap)/2); rect=CGRectMake(gap+cw*0.44,188,cw*0.56,44); manual=YES;
                } else if (role==HG_POKEDEX_UI_IMAGE_TRACK || cRole==HG_POKEDEX_UI_ROLE_RANGE_HANDLE) {
                    rect=CGRectMake(52,64+index*80,MAX(132,contentW-104),44); manual=YES;
                } else if (cRole==HG_POKEDEX_UI_ROLE_RANGE_STEP || (role==HG_POKEDEX_UI_IMAGE_ARROW
                    && (family==HG_POKEDEX_UI_FAMILY_SEARCH_HEIGHT || family==HG_POKEDEX_UI_FAMILY_SEARCH_WEIGHT))) {
                    uint32_t step=cRole ? cIndex : index;
                    rect=CGRectMake((step&1) ? 4 : MAX(56,contentW-48),64+(step/2)*80,44,44); manual=YES;
                } else if (visual->kind==HG_POKEDEX_UI_VISUAL_WINDOW) {
                    uint32_t first=family==HG_POKEDEX_UI_FAMILY_SEARCH_ORDER ? 36 : family==HG_POKEDEX_UI_FAMILY_SEARCH_NAME ? 65 : family==HG_POKEDEX_UI_FAMILY_SEARCH_TYPE ? 44
                        : family==HG_POKEDEX_UI_FAMILY_SEARCH_HEIGHT ? 68 : family==HG_POKEDEX_UI_FAMILY_SEARCH_WEIGHT ? 71 : family==HG_POKEDEX_UI_FAMILY_SEARCH_AREA ? 74 : 80;
                    NSUInteger columns=family==HG_POKEDEX_UI_FAMILY_SEARCH_TYPE || family==HG_POKEDEX_UI_FAMILY_SEARCH_HEIGHT || family==HG_POKEDEX_UI_FAMILY_SEARCH_WEIGHT ? 3 : 2;
                    CGFloat cw=MAX(44,(contentW-(columns+1)*gap)/columns); rect=CGRectMake(gap+(index-first)%columns*(cw+gap),8,cw,44); manual=YES;
                } else { rect=CGRectMake(8,mainY,width,MAX(24,height)); mainY=CGRectGetMaxY(rect)+8; manual=YES; }
            }
        } else if (areaPage) {
            if (role==HG_POKEDEX_UI_IMAGE_AREA_MAP) { parent=_pokedexContent; rect=CGRectMake(8,56,contentW-16,MAX(96,bodyH-64)); manual=YES; }
            else if (visual->kind==HG_POKEDEX_UI_VISUAL_WINDOW && visual->source.window.engine) { image.hidden=YES; manual=YES; }
            else if (cRole==HG_POKEDEX_UI_ROLE_TIME || (role==HG_POKEDEX_UI_IMAGE_OPTION && visual->kind==HG_POKEDEX_UI_VISUAL_RASTER && index<3)) { parent=_pokedexContent; CGFloat cw=MAX(44,(contentW-4*gap)/3); uint32_t time=cRole==HG_POKEDEX_UI_ROLE_TIME ? cIndex : index; rect=CGRectMake(gap+time*(cw+gap),4,cw,44); manual=YES; }
            else if (role==HG_POKEDEX_UI_IMAGE_AREA_ROW) { parent=_pokedexDetails; uint32_t row=visual->kind==HG_POKEDEX_UI_VISUAL_WINDOW && index>=8 ? index-8 : index; rect=CGRectMake(8,64+row*52,MAX(44,detailW-60),44); manual=YES; }
            else if (role==HG_POKEDEX_UI_IMAGE_TRACK) { parent=_pokedexDetails; rect=CGRectMake(MAX(0,detailW-44),112,44,MAX(132,bodyH-168)); manual=YES; }
            else if ((cRole==HG_POKEDEX_UI_ROLE_FORM_STEP && cIndex>=4) || (role==HG_POKEDEX_UI_IMAGE_COMPARE_CONTROL && index>=4 && index<=5)) { parent=_pokedexDetails; rect=CGRectMake(MAX(0,detailW-44),(cRole ? cIndex : index)==4 ? 64 : MAX(248,bodyH-52),44,44); manual=YES; }
            else if (cRole==HG_POKEDEX_UI_ROLE_REGION || (visual->kind==HG_POKEDEX_UI_VISUAL_WINDOW && (index==5 || index==7))) { parent=_pokedexDetails; CGFloat cw=MAX(44,(detailW-3*gap)/2); uint32_t region=cRole ? cIndex : index==7; rect=CGRectMake(gap+region*(cw+gap),4,cw,44); manual=YES; }
            else if (visual->kind==HG_POKEDEX_UI_VISUAL_WINDOW && index==6) { parent=_pokedexDetails; rect=CGRectMake(8,4,detailW-16,44); manual=YES; }
            else if (visual->kind==HG_POKEDEX_UI_VISUAL_WINDOW && index==4) { parent=_pokedexDetails; rect=CGRectMake(8,384,detailW-16,24); manual=YES; }
        } else if (size) {
            parent=_pokedexContent; CGFloat cw=MAX(128,(contentW-3*gap)/2); uint32_t side=visual->kind==HG_POKEDEX_UI_VISUAL_WINDOW ? (visual->source.window.engine ? 1 : 0) : index;
            if (role==HG_POKEDEX_UI_IMAGE_SIZE_SCENE) { rect=CGRectMake(gap+side*(cw+gap),36,cw,MAX(112,bodyH-116)); manual=YES; }
            else if (visual->kind==HG_POKEDEX_UI_VISUAL_WINDOW) {
                if (!side || index<4) image.hidden=YES; // These original captions are already inside the paired scene crops.
                uint32_t row=side ? index : index>=8 ? index-8 : index;
                rect=CGRectMake(gap+side*(cw+gap),row<2 ? row*24 : MAX(160,bodyH-64)+(row-2)*22,cw,20); manual=YES;
            }
        } else if (forms) {
            if (role==HG_POKEDEX_UI_IMAGE_FORM_ROW || role==HG_POKEDEX_UI_IMAGE_FORM_ICON) {
                uint32_t row=role==HG_POKEDEX_UI_IMAGE_FORM_ROW && visual->kind==HG_POKEDEX_UI_VISUAL_WINDOW && index>=10 ? index-10 : index;
                parent=_pokedexContent; rect=CGRectMake(role==HG_POKEDEX_UI_IMAGE_FORM_ICON ? 4 : 48,8+row*52,role==HG_POKEDEX_UI_IMAGE_FORM_ICON ? 40 : MAX(44,contentW-100),44); manual=YES;
            } else if ((cRole==HG_POKEDEX_UI_ROLE_FORM_STEP && cIndex>=4) || (role==HG_POKEDEX_UI_IMAGE_COMPARE_CONTROL && index>=4 && index<=5)) { parent=_pokedexContent; rect=CGRectMake(MAX(0,contentW-44),(cRole ? cIndex : index)==4 ? 8 : MAX(196,bodyH-52),44,44); manual=YES;
            } else if (role==HG_POKEDEX_UI_IMAGE_FORM_PICTURE) { parent=_pokedexDetails; CGFloat cw=(detailW-3*gap)/2; rect=CGRectMake(gap+index*(cw+gap),72,cw,112); manual=YES; }
            else if (cRole==HG_POKEDEX_UI_ROLE_COMPARE || (visual->kind==HG_POKEDEX_UI_VISUAL_WINDOW && index==4)) { parent=_pokedexDetails; rect=CGRectMake(8,192,MAX(44,detailW-16),44); manual=YES; }
            else if (visual->kind==HG_POKEDEX_UI_VISUAL_WINDOW && index<=2) { parent=_pokedexDetails; rect=CGRectMake(8,index ? 32 : 4,MAX(44,detailW-16),24); if (index==2) rect=CGRectMake(8+detailW*0.4,32,MAX(44,detailW*0.6-16),24); else if (index==1) rect.size.width=detailW*0.4-8; manual=YES; }
            else if (visual->kind==HG_POKEDEX_UI_VISUAL_WINDOW) { image.hidden=YES; manual=YES; }
        } else if (compare) {
            parent=_pokedexContent; CGFloat cw=MAX(128,(contentW-3*gap)/2);
            if (role==HG_POKEDEX_UI_IMAGE_FORM_PICTURE) { rect=CGRectMake(gap+index*(cw+gap),64,cw,128); manual=YES; }
            else if (role==HG_POKEDEX_UI_IMAGE_FORM_ICON) { rect=CGRectMake(gap+index*(cw+gap),32,cw,28); manual=YES; }
            else if (role==HG_POKEDEX_UI_IMAGE_COMPARE_CONTROL) { uint32_t side=index<4 ? index/2 : index-6; rect=CGRectMake(gap+side*(cw+gap)+(index<4 ? (index%2)*(cw-44) : 0),index<4 ? 224 : 276,index<4 ? 44 : cw,44); manual=YES; }
            else if (visual->kind==HG_POKEDEX_UI_VISUAL_WINDOW && (index==6 || index==7)) { rect=CGRectMake(gap+(index-6)*(cw+gap),196,cw,20); manual=YES; }
            else if (visual->kind==HG_POKEDEX_UI_VISUAL_WINDOW) {
                rect=CGRectMake(index==3 ? contentW*0.3 : 8,4,index==3 ? contentW*0.7-8 : contentW*0.3-16,24);
                if (index==5) image.hidden=YES; // Window0 already shows the same original FORMS title.
                manual=YES;
            }
        } else if (entry) {
            parent=_pokedexContent; rect=CGRectMake(24,role==HG_POKEDEX_UI_IMAGE_ENTRY_BODY ? 80 : 24,MAX(44,contentW-48),role==HG_POKEDEX_UI_IMAGE_ENTRY_BODY ? 64 : 28); manual=YES;
            if (visual->kind==HG_POKEDEX_UI_VISUAL_WINDOW && index==1) image.hidden=YES;
        }
        if (!manual) { if (detailW) detailY=CGRectGetMaxY(rect)+8; else mainY=CGRectGetMaxY(rect)+8; }
        if (image.superview!=parent) [parent addSubview:image]; image.frame=rect;
        if (parent==_pokedexContent) { contentBottom=MAX(contentBottom,CGRectGetMaxY(rect)+8); contentRight=MAX(contentRight,CGRectGetMaxX(rect)+8); }
        if (parent==_pokedexDetails) detailBottom=MAX(detailBottom,CGRectGetMaxY(rect)+8);
    }
    _pokedexContent.contentSize=CGSizeMake(contentRight,contentBottom);
    _pokedexDetails.contentSize=CGSizeMake(detailW,detailBottom);
    NSUInteger keyArrowIndex=footerIndex;
    for (uint32_t i=0;i<MIN(state->controlCount,HG_POKEDEX_UI_CONTROLS);i++) {
        UIButton *button=_pokedexButtons[i]; uint32_t visual=state->controls[i].visualId;
        if (visual>=MIN(visualState->visualCount,HG_POKEDEX_UI_VISUALS)) { button.hidden=YES; continue; }
        uint32_t role=state->controls[i].role;
        if (role==HG_POKEDEX_UI_ROLE_DEX_MODE || (role==HG_POKEDEX_UI_ROLE_ENTRY && (areaPage || forms))) {
            NSUInteger columns=footerColumns; CGFloat cw=(footerWidth-(columns+1)*gap)/columns;
            if (button.superview!=_pokedexPanel) [_pokedexPanel addSubview:button];
            button.frame=CGRectMake(gap+(keyArrowIndex%columns)*(cw+gap),bodyH+4+(keyArrowIndex/columns)*48,cw,44); keyArrowIndex++; continue;
        }
        UIImageView *image=_pokedexImages[visual]; if (button.superview!=image.superview) [image.superview addSubview:button];
        CGRect rect=image.frame; CGFloat dw=MAX(0,44-rect.size.width),dh=MAX(0,44-rect.size.height);
        button.frame=CGRectInset(rect,-dw/2,-dh/2);
    }
    [_condition lock]; NSInteger dragging=_pokedexGestureAdmitted ? _pokedexGestureControl : -1; uint32_t handle=_pokedexGestureTrack.handle; [_condition unlock];
    if (dragging>=0 && dragging<_pokedexButtons.count && !state->controlCount) for (uint32_t i=0;i<MIN(visualState->visualCount,HG_POKEDEX_UI_VISUALS);i++) {
        if (visualState->visuals[i].role==HG_POKEDEX_UI_IMAGE_TRACK && visualState->visuals[i].index==handle) {
            UIImageView *image=_pokedexImages[i]; UIButton *button=_pokedexButtons[dragging];
            if (button.superview!=image.superview) [image.superview addSubview:button]; button.frame=image.frame; break;
        }
    }
}
@end
