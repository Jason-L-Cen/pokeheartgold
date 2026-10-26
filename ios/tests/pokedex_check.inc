// SPDX-License-Identifier: GPL-3.0-or-later
// UNAPPLIED private CHECK draft. A passed matching native Pokédex case is a build prerequisite.
#ifdef HEARTGOLD_ORIGINAL_POKEDEX_CHECK
#include "pokedex.h"
#include "pokemon.h"

// Only the already-existing production pan callback is exercised; this is not recognizer/physical-touch proof.
@interface PokedexCheckPan : UIPanGestureRecognizer
@property(nonatomic) UIGestureRecognizerState checkState;
@property(nonatomic, weak) UIView *checkView;
@property(nonatomic) CGPoint checkTranslation;
@end
@implementation PokedexCheckPan
- (UIGestureRecognizerState)state { return _checkState; }
- (UIView *)view { return _checkView; }
- (CGPoint)translationInView:(UIView *)view { (void)view; return _checkTranslation; }
@end

static struct {
    uint32_t save,party,flags,bag,profile,dex,money,gift,variant,requestedVariant;
    BOOL restoreRequested,prepared;
} sPokedexCheckOwner;
static NSData *sPokedexPartyBefore,*sPokedexBagBefore,*sPokedexFlagsBefore,*sPokedexFlagsPrepared,*sPokedexDexBefore,*sPokedexDexPrepared;
static NSArray *sPokedexRegionalSeen;
static void (^sPokedexCheckDisplay)(uint32_t);
static BOOL PokedexCheckFamily(const HGNativePokedexUIState *s,uint32_t family) {
    return s && s->active && s->ready && s->family==family && s->phase==HGNativePokedexUIFamilyLimit(family,0);
}
static uint32_t PokedexCheckControl(const HGNativePokedexUIState *s,uint32_t role,uint32_t index,BOOL enabled) {
    if (s) for (uint32_t i=0;i<MIN(s->controlCount,HG_POKEDEX_UI_CONTROLS);i++)
        if (s->controls[i].role==role && s->controls[i].index==index && (!enabled || (s->controls[i].flags&HG_POKEDEX_UI_CONTROL_ENABLED))) return i;
    return UINT32_MAX;
}

@implementation OriginalController (PokedexChecks)
- (BOOL)pokedexFixtureAdmission:(const HGSummaryLearnQAState *)q bag:(const HGNativeBagUIState *)bag
    dex:(const HGNativePokedexUIState *)dex map:(uint32_t)map idle:(BOOL)idle {
    if (!q || q->version!=HG_SUMMARY_LEARN_QA_VERSION || q->byteSize!=sizeof(*q)
        || !dex || dex->active || !idle || !q->fadeFinished) return NO;
    if (!q->owner) return map==MAP_ROUTE_29 && q->fieldReady && !q->manager && (!bag || !bag->active);
    return q->owner==HG_SUMMARY_LEARN_BAG && q->execState==2 && q->graphicsReady && bag && bag->active
        && bag->ready && !bag->mode && bag->phase==HG_BAG_UI_INPUT && q->phase==bag->phase;
}
- (BOOL)queuePokedexFixture:(uint32_t)variant restoring:(BOOL)restoring revision:(uint32_t)revision {
    [_condition lock];
    const HGSummaryLearnQAState *q=_summaryTMQAState.length==sizeof(*q) ? _summaryTMQAState.bytes : NULL;
    const HGNativeBagUIState *bag=_bagUIState.length==sizeof(*bag) ? _bagUIState.bytes : NULL;
    const HGNativePokedexUIState *dex=_pokedexUIState.length==sizeof(*dex) ? _pokedexUIState.bytes : NULL;
    BOOL valid=_summaryTMEnabled && !_paused && !_stopping && !_summaryTMRequestPending && !_summaryTMFixturePreparing
        && q && q->revision==revision && variant>=1 && variant<=3 && _summaryTMBootInputStep==3 && !_summaryTMBootInputFrame
        && [self pokedexFixtureAdmission:q bag:bag dex:dex map:_fieldMapId idle:[self summaryTMInputIdleLocked]];
    if (valid) {sPokedexCheckOwner.requestedVariant=variant;sPokedexCheckOwner.restoreRequested=restoring;
        _summaryTMRequestAccepted=NO;_summaryTMRequestPending=YES;_summaryTMRequestRevision=revision;}
    [_condition unlock];return valid;
}
- (void)resetPokedexCheckOwner {
    memset(&sPokedexCheckOwner,0,sizeof(sPokedexCheckOwner));
    sPokedexPartyBefore=sPokedexBagBefore=sPokedexFlagsBefore=sPokedexFlagsPrepared=sPokedexDexBefore=sPokedexDexPrepared=nil;sPokedexRegionalSeen=nil;
}
- (BOOL)preparePokedexFixture:(const HGSummaryLearnQAState *)q {
#define PD_REQUIRE(value,message) do {if (!(value)) {_summaryTMFixtureError=(message);return NO;}} while (0)
    [_condition lock];uint32_t variant=sPokedexCheckOwner.requestedVariant;BOOL restore=sPokedexCheckOwner.restoreRequested;[_condition unlock];
    PD_REQUIRE(q && variant>=1 && variant<=3,@"pokedex_fixture_request");
    uint32_t scratch=w2c_hgengine_HGNativeFrameBuffer(_game);
    if (!sPokedexPartyBefore) {
        sPokedexCheckOwner.save=w2c_hgengine_SaveData_Get(_game);
        sPokedexCheckOwner.party=w2c_hgengine_SaveArray_Party_Get(_game,sPokedexCheckOwner.save);
        sPokedexCheckOwner.flags=w2c_hgengine_Save_VarsFlags_Get(_game,sPokedexCheckOwner.save);
        sPokedexCheckOwner.bag=w2c_hgengine_Save_Bag_Get(_game,sPokedexCheckOwner.save);
        sPokedexCheckOwner.profile=w2c_hgengine_Save_PlayerData_GetProfile(_game,sPokedexCheckOwner.save);
        sPokedexCheckOwner.dex=w2c_hgengine_Save_Pokedex_Get(_game,sPokedexCheckOwner.save);
        uint32_t ps=w2c_hgengine_SaveArray_Party_sizeof(_game),bs=w2c_hgengine_Save_Bag_sizeof(_game),fs=w2c_hgengine_Save_VarsFlags_sizeof(_game);
        PD_REQUIRE(ps && ps<65536 && bs && bs<65536 && fs && fs<65536 && w2c_hgengine_Save_Pokedex_sizeof(_game)==sizeof(Pokedex),@"pokedex_fixture_sizes");
        PD_REQUIRE(w2c_hgengine_Party_GetCount(_game,sPokedexCheckOwner.party)>0,@"pokedex_original_party");
        uint32_t mon=w2c_hgengine_Party_GetMonByIndex(_game,sPokedexCheckOwner.party,0);
        PD_REQUIRE(w2c_hgengine_GetMonData(_game,mon,MON_DATA_SPECIES,0)==152 && w2c_hgengine_GetMonData(_game,mon,MON_DATA_LEVEL,0)==5,@"pokedex_original_starter");
        PD_REQUIRE(!w2c_hgengine_Pokedex_GetNatDexFlag(_game,sPokedexCheckOwner.dex)
            && !w2c_hgengine_Pokedex_CheckMonSeenFlag(_game,sPokedexCheckOwner.dex,201)
            && w2c_hgengine_Pokedex_CountNationalDexOwned(_game,sPokedexCheckOwner.dex)==1
            && w2c_hgengine_Pokedex_CheckMonCaughtFlag(_game,sPokedexCheckOwner.dex,152),@"pokedex_existing_history_precondition");
        sPokedexPartyBefore=[NSData dataWithBytes:[self summaryTMGuest:sPokedexCheckOwner.party size:ps] length:ps];
        sPokedexBagBefore=[NSData dataWithBytes:[self summaryTMGuest:sPokedexCheckOwner.bag size:bs] length:bs];
        sPokedexFlagsBefore=[NSData dataWithBytes:[self summaryTMGuest:sPokedexCheckOwner.flags size:fs] length:fs];
        sPokedexDexBefore=[NSData dataWithBytes:[self summaryTMGuest:sPokedexCheckOwner.dex size:sizeof(Pokedex)] length:sizeof(Pokedex)];
        sPokedexCheckOwner.money=w2c_hgengine_PlayerProfile_GetMoney(_game,sPokedexCheckOwner.profile);
        sPokedexCheckOwner.gift=w2c_hgengine_Save_VarsFlags_CheckFlagInArray(_game,sPokedexCheckOwner.flags,0x6B);
    }
    PD_REQUIRE(!memcmp(sPokedexPartyBefore.bytes,[self summaryTMGuest:sPokedexCheckOwner.party size:sPokedexPartyBefore.length],sPokedexPartyBefore.length)
        && !memcmp(sPokedexBagBefore.bytes,[self summaryTMGuest:sPokedexCheckOwner.bag size:sPokedexBagBefore.length],sPokedexBagBefore.length),@"pokedex_party_bag_preservation");
    memcpy((void *)[self summaryTMGuest:scratch size:sizeof(Pokedex)],sPokedexDexBefore.bytes,sizeof(Pokedex));
    w2c_hgengine_Pokedex_Copy(_game,scratch,sPokedexCheckOwner.dex);
    if (!sPokedexCheckOwner.gift) w2c_hgengine_Save_VarsFlags_ClearFlagInArray(_game,sPokedexCheckOwner.flags,0x6B);
    PD_REQUIRE(!memcmp(sPokedexFlagsBefore.bytes,[self summaryTMGuest:sPokedexCheckOwner.flags size:sPokedexFlagsBefore.length],sPokedexFlagsBefore.length),@"pokedex_original_flags");
    sPokedexCheckOwner.variant=variant;
    if (restore) {sPokedexCheckOwner.prepared=NO;_summaryTMFixturePrepared=NO;return YES;}
    w2c_hgengine_Pokedex_Enable(_game,sPokedexCheckOwner.dex);
    w2c_hgengine_Save_VarsFlags_SetFlagInArray(_game,sPokedexCheckOwner.flags,0x6B);
    if (variant==3) w2c_hgengine_Pokedex_SetNatDexFlag(_game,sPokedexCheckOwner.dex);
    void (^see)(uint32_t,uint32_t)=^(uint32_t species,uint32_t form) {
        uint32_t mon=w2c_hgengine_Party_GetMonByIndex(self->_game,sPokedexCheckOwner.party,0),copy=scratch+1024,value=scratch+2048;
        [self summaryTMGuest:copy size:sizeof(Pokemon)];w2c_hgengine_CopyPokemonToPokemon(self->_game,mon,copy);
        uint16_t sp=(uint16_t)species;uint8_t fm=(uint8_t)form;
        memcpy((void *)[self summaryTMGuest:value size:sizeof(sp)],&sp,sizeof(sp));w2c_hgengine_SetMonData(self->_game,copy,MON_DATA_SPECIES,value);
        memcpy((void *)[self summaryTMGuest:value size:sizeof(fm)],&fm,sizeof(fm));w2c_hgengine_SetMonData(self->_game,copy,MON_DATA_FORM,value);
        w2c_hgengine_Pokedex_SetMonSeenFlag(self->_game,sPokedexCheckOwner.dex,copy);
    };
    for (uint32_t form=0;form<variant;form++) see(201,form);
    const uint32_t capacity[]={16,17,18,19,20,21,22,23,24,161,162,163,164,165,166};
    for (NSUInteger i=0;i<sizeof(capacity)/sizeof(*capacity) && w2c_hgengine_Pokedex_CountJohtoDexSeen(_game,sPokedexCheckOwner.dex)<16;i++)
        if (!w2c_hgengine_Pokedex_CheckMonSeenFlag(_game,sPokedexCheckOwner.dex,capacity[i])) see(capacity[i],0);
    PD_REQUIRE(w2c_hgengine_Pokedex_CountJohtoDexSeen(_game,sPokedexCheckOwner.dex)>=16
        && w2c_hgengine_Pokedex_CountNationalDexOwned(_game,sPokedexCheckOwner.dex)==1
        && w2c_hgengine_Pokedex_GetSeenFormNum(_game,sPokedexCheckOwner.dex,201)==variant,@"pokedex_original_seen_capacity");
    for (uint32_t form=0;form<variant;form++) PD_REQUIRE(w2c_hgengine_Pokedex_GetSeenFormByIdx(_game,sPokedexCheckOwner.dex,201,form)==form,@"pokedex_original_forms");
    for (uint32_t species=1;species<=493;species++) if (w2c_hgengine_Pokedex_CheckMonSeenFlag(_game,sPokedexCheckOwner.dex,species))
        PD_REQUIRE(w2c_hgengine_GetMonBaseStat_HandleAlternateForm(_game,species,0,BASE_TYPE1)!=16
            && w2c_hgengine_GetMonBaseStat_HandleAlternateForm(_game,species,0,BASE_TYPE2)!=16,@"pokedex_existing_dragon_history");
    NSMutableArray *regionalSeen=[NSMutableArray new];
    for (uint32_t species=1;species<=493;species++) if (w2c_hgengine_Pokedex_CheckMonSeenFlag(_game,sPokedexCheckOwner.dex,species)
        && w2c_hgengine_Pokedex_ConvertToCurrentDexNo(_game,0,species)) [regionalSeen addObject:@(species)];
    sPokedexRegionalSeen=[regionalSeen copy];
    sPokedexFlagsPrepared=[NSData dataWithBytes:[self summaryTMGuest:sPokedexCheckOwner.flags size:sPokedexFlagsBefore.length] length:sPokedexFlagsBefore.length];
    sPokedexDexPrepared=[NSData dataWithBytes:[self summaryTMGuest:sPokedexCheckOwner.dex size:sizeof(Pokedex)] length:sizeof(Pokedex)];
    sPokedexCheckOwner.prepared=YES;_summaryTMFixturePrepared=YES;return YES;
#undef PD_REQUIRE
}
- (NSDictionary *)pokedexCurrentFacts {
    if (!sPokedexPartyBefore) return @{@"fixturePrepared":@NO,@"fixtureError":_summaryTMFixtureError ?: @""};
    NSData *flags=sPokedexCheckOwner.prepared ? sPokedexFlagsPrepared : sPokedexFlagsBefore;
    NSData *dex=sPokedexCheckOwner.prepared ? sPokedexDexPrepared : sPokedexDexBefore;
    BOOL party=!memcmp(sPokedexPartyBefore.bytes,[self summaryTMGuest:sPokedexCheckOwner.party size:sPokedexPartyBefore.length],sPokedexPartyBefore.length);
    BOOL bag=!memcmp(sPokedexBagBefore.bytes,[self summaryTMGuest:sPokedexCheckOwner.bag size:sPokedexBagBefore.length],sPokedexBagBefore.length);
    BOOL flagBytes=!memcmp(flags.bytes,[self summaryTMGuest:sPokedexCheckOwner.flags size:flags.length],flags.length);
    BOOL dexBytes=!memcmp(dex.bytes,[self summaryTMGuest:sPokedexCheckOwner.dex size:dex.length],dex.length);
    return @{@"fixturePrepared":@(sPokedexCheckOwner.prepared),@"fixtureError":_summaryTMFixtureError ?: @"",@"variant":@(sPokedexCheckOwner.variant),
        @"fullPartyPreserved":@(party),@"bagPreserved":@(bag),@"flagsPreserved":@(flagBytes),@"dexPreserved":@(dexBytes),
        @"moneyPreserved":@(w2c_hgengine_PlayerProfile_GetMoney(_game,sPokedexCheckOwner.profile)==sPokedexCheckOwner.money),
        @"national":@(w2c_hgengine_Pokedex_GetNatDexFlag(_game,sPokedexCheckOwner.dex)),@"forms":@(w2c_hgengine_Pokedex_GetSeenFormNum(_game,sPokedexCheckOwner.dex,201)),
        @"caught":@(w2c_hgengine_Pokedex_CountNationalDexOwned(_game,sPokedexCheckOwner.dex)),@"regionalSeen":sPokedexRegionalSeen ?: @[]};
}
- (void)checkPokedexDisplay:(uint32_t)frame { if (sPokedexCheckDisplay) sPokedexCheckDisplay(frame); }
- (NSString *)pokedexLayoutFailure:(const HGNativePokedexUIState *)state evidence:(NSMutableDictionary *)evidence {
    NSData *metadata=_pokedexDisplayPictures[@"metadata"];
    const HGNativePokedexUIState *visuals=metadata.length==sizeof(*visuals) ? metadata.bytes : NULL;
    if (!visuals || visuals->ownerGeneration!=state->ownerGeneration || visuals->family!=state->family) return @"layout_metadata_mismatch";
    BOOL list=state->family==HG_POKEDEX_UI_FAMILY_LIST || state->family==HG_POKEDEX_UI_FAMILY_SEARCH_RESULTS;
    BOOL regular=_pokedexPanel.bounds.size.width>=520 && _pokedexPanel.bounds.size.height>=320;
    if (state->family==HG_POKEDEX_UI_FAMILY_LIST) for (uint32_t i=0;i<MIN(visuals->visualCount,HG_POKEDEX_UI_VISUALS);i++)
        if (visuals->visuals[i].role==HG_POKEDEX_UI_IMAGE_FOOTER && visuals->visuals[i].index==10 && ![_pokedexImages[i] isHidden]) return @"list_start_gutter_exposed";
    if (list && regular) {
        if (fabs(_pokedexDetails.frame.size.height-_pokedexPanel.bounds.size.height)>0.5
            || !CGRectContainsRect(_pokedexPanel.bounds,_pokedexDetails.frame)) return @"list_detail_viewport_clipped";
        NSUInteger criteria=0;
        for (uint32_t i=0;i<MIN(visuals->visualCount,HG_POKEDEX_UI_VISUALS);i++) {
            UIImageView *image=_pokedexImages[i];
            if (image.superview==_pokedexDetails && !image.hidden
                && !CGRectContainsRect(_pokedexDetails.bounds,image.frame)) return @"list_detail_visual_clipped";
            criteria+=visuals->visuals[i].role==HG_POKEDEX_UI_IMAGE_CRITERION_LABEL;
        }
        for (uint32_t c=0;c<MIN(state->controlCount,HG_POKEDEX_UI_CONTROLS);c++) {
            UIButton *button=_pokedexButtons[c];
            if (!button.hidden && button.superview==_pokedexPanel
                && (button.frame.size.width<44 || button.frame.size.height<44
                    || !CGRectContainsRect(_pokedexPanel.bounds,button.frame)
                    || CGRectIntersectsRect(button.frame,_pokedexDetails.frame))) return @"list_footer_geometry_invalid";
        }
        if (state->family==HG_POKEDEX_UI_FAMILY_SEARCH_RESULTS && !state->page && !state->cursor && criteria!=7) return @"results_criterion_geometry_missing";
        evidence[@(state->family).stringValue]=@{@"panelHeight":@(_pokedexPanel.bounds.size.height),@"detailHeight":@(_pokedexDetails.frame.size.height),@"criteria":@(criteria)};
    }
    if (state->family==HG_POKEDEX_UI_FAMILY_AREA) {
        NSMutableArray *frames=[NSMutableArray arrayWithArray:@[NSNull.null,NSNull.null,NSNull.null]];
        for (uint32_t i=0;i<MIN(visuals->visualCount,HG_POKEDEX_UI_VISUALS);i++) {
            const HGNativePokedexUIVisual *v=&visuals->visuals[i];
            if (v->role!=HG_POKEDEX_UI_IMAGE_OPTION || v->kind!=HG_POKEDEX_UI_VISUAL_RASTER || v->index>=3) continue;
            UIView *view=_pokedexImages[i];
            for (uint32_t c=0;c<MIN(state->controlCount,HG_POKEDEX_UI_CONTROLS);c++)
                if (state->controls[c].visualId==i && ![_pokedexButtons[c] isHidden]) {view=_pokedexButtons[c];break;}
            CGRect rect=view.frame;
            if (view.hidden || view.superview!=_pokedexContent || !CGRectContainsRect(_pokedexContent.bounds,rect)
                || rect.size.width<44 || rect.size.height<44) return @"area_time_view_missing";
            frames[v->index]=@[@(rect.origin.x),@(rect.origin.y),@(rect.size.width),@(rect.size.height)];
        }
        for (NSUInteger i=0;i<3;i++) {
            if (![frames[i] isKindOfClass:NSArray.class]) return @"area_time_view_missing";
            if (i && (fabs([frames[i][1] doubleValue]-[frames[0][1] doubleValue])>0.5
                || [frames[i-1][0] doubleValue]+[frames[i-1][2] doubleValue]>[frames[i][0] doubleValue])) return @"area_time_alignment_invalid";
        }
        NSDictionary *ready=evidence[@"areaReady"];
        if (state->phase==73) {
            if ([ready[@"generation"] unsignedIntValue]!=state->ownerGeneration) return @"area_time_ready_baseline_missing";
            for (NSUInteger i=0;i<3;i++) for (NSUInteger j=0;j<4;j++)
                if (fabs([ready[@"frames"][i][j] doubleValue]-[frames[i][j] doubleValue])>0.5) return @"area_time_drag_geometry_changed";
        }
        evidence[state->ready ? @"areaReady" : @"areaDrag73"]=@{@"generation":@(state->ownerGeneration),@"frames":frames};
    }
    return nil;
}
- (void)checkPokedexCase {
    NSArray *requiredKeys=@[@"caseBinding",@"muted",@"fixtureHistory",@"all16Families",@"all8DragPhases",@"nativeButtons",@"originalGlyphs",@"rawInputBlocked",@"staleGenerationRejected",@"pauseClearsLease",@"threeVariants",@"returnedField",@"fieldControlsRestored",@"partyBagHistoryPreserved",@"privateSave4Preserved",@"resetQuiescent",@"capturesComplete"];
    NSMutableDictionary *checks=[NSMutableDictionary new];for (NSString *key in requiredKeys) checks[key]=@NO;
    NSString *nativeReportSha256=[NSBundle.mainBundle objectForInfoDictionaryKey:@"HGPrivatePokedexNativeReportSha256"];
    NSString *nativeModuleSha256=[NSBundle.mainBundle objectForInfoDictionaryKey:@"HGPrivatePokedexNativeModuleSha256"];
    NSMutableDictionary *noResultsEvidence=[NSMutableDictionary new],*layoutEvidence=[NSMutableDictionary new];
    NSMutableArray *events=[NSMutableArray new],*dragEvidence=[NSMutableArray new],*requiredPNGs=[NSMutableArray new];
    NSMutableSet *captured=[NSMutableSet new];NSMutableIndexSet *families=[NSMutableIndexSet new],*phases=[NSMutableIndexSet new];
    for (uint32_t f=1;f<=16;f++) [requiredPNGs addObject:[NSString stringWithFormat:@"private-pokedex-family-%02u.png",f]];
    const uint32_t dragPhases[]={15,60,73,84,40,41,47,48};
    for (uint32_t p=0;p<8;p++) for (NSString *end in @[@"min",@"max"]) [requiredPNGs addObject:[NSString stringWithFormat:@"private-pokedex-drag-%02u-%@.png",dragPhases[p],end]];
    for (NSString *name in @[@"u2-forms",@"u2-compare",@"u3-forms",@"u3-compare",@"no-results",@"field-return"])
        [requiredPNGs addObject:[NSString stringWithFormat:@"private-pokedex-%@.png",name]];
    NSURL *support=[[NSFileManager.defaultManager URLsForDirectory:NSApplicationSupportDirectory inDomains:NSUserDomainMask].firstObject URLByAppendingPathComponent:@"HeartGoldOriginal" isDirectory:YES];
    NSMutableDictionary *privateBefore=[NSMutableDictionary new];for (NSString *suffix in @[@"",@".console",@".wifi",@".lock"]) {
        NSString *name=[@"heartgold.sav" stringByAppendingString:suffix];NSData *bytes=[NSData dataWithContentsOfURL:[support URLByAppendingPathComponent:name]];if (bytes) privateBefore[name]=bytes;
    }
    [_condition lock];sPrivateUICase=@"pokedex";sPrivateCaptures=[NSMutableArray new];sPrivateObserving=NO;[_condition unlock];
    __block uint32_t variant=1,stage=0,bootstrap=0,substep=0,polls=0,stageFrame=0,lastStage=UINT32_MAX,lastRevision=0,oldGeneration=0;
    __block uint32_t buttons=0,stale=0,areaProbe=16,areaTime=0,fieldFrame=0,dragPhase=0,dragSerial=0,dragStage=0,dragBeginReads=0,panGeneration=0,panFamily=0;
    __block BOOL finishing=NO,pauseChecked=NO,release=NO,releaseStarted=NO;__block uint32_t releaseReads=0;
    __block NSData *noResultsInitial=nil,*noResultsCandidate=nil;__block uint32_t noResultsFrame=0,noResultsCandidateFrame=0;
    __block PokedexCheckPan *pan=nil;__block HGNativePokedexUITrack track={0};__block uint32_t panStart=0,panTarget=0;
    void (^capture)(NSString *)=^(NSString *name) {if (![captured containsObject:name]) {[self checkPrivateCapture:name kind:@"pokedex" image:nil];[captured addObject:name];}};
    __block void (^finish)(NSString *);
    finish=^(NSString *status) {
        if (finishing) return;finishing=YES;sPokedexCheckDisplay=nil;
        [self->_condition lock];NSDictionary *facts=[self->_summaryTMFacts copy] ?: @{};uint32_t boots=self->_bootCount;self->_checkResetPending=YES;[self->_condition unlock];
        __block NSUInteger waits=0;__block void (^reset)(void);reset=^{
            [self->_condition lock];BOOL restarted=self->_bootCount==boots+1 && self->_fieldMapId==MAP_ROUTE_29 && self->_publishedFrames>self->_bootFrameBase;
            BOOL clean=self->_resetQuiesced && self->_restartClean,failed=self->_failure!=nil || self->_finished;[self->_condition unlock];
            const HGNativePokedexUIState *d=self->_displayedPokedexUIState.length==sizeof(*d) ? self->_displayedPokedexUIState.bytes : NULL;
            if (!restarted && !failed && ++waits<200) {dispatch_after(dispatch_time(DISPATCH_TIME_NOW,NSEC_PER_SEC/10),dispatch_get_main_queue(),reset);return;}
            checks[@"resetQuiescent"]=@(restarted && clean && d && !d->active && !self->_pokedexScene && self->_pokedexPanel.hidden
                && self->_buttons[@1].enabled && self->_buttons[@2].enabled && self->_buttons[@1].alpha==1 && self->_buttons[@2].alpha==1);
            reset=nil;[self stop];__block NSUInteger stops=0;__block void (^stopped)(void);stopped=^{
                [self->_condition lock];BOOL done=self->_finished && !self->_env && !self->_game && !self->_wasi;[self->_condition unlock];
                if (!done && ++stops<100) {dispatch_after(dispatch_time(DISPATCH_TIME_NOW,NSEC_PER_SEC/10),dispatch_get_main_queue(),stopped);return;}
                checks[@"resetQuiescent"]=@([checks[@"resetQuiescent"] boolValue] && done);BOOL same=privateBefore.count==4;
                for (NSString *name in privateBefore) same&=[privateBefore[name] isEqualToData:[NSData dataWithContentsOfURL:[support URLByAppendingPathComponent:name]]];
                checks[@"privateSave4Preserved"]=@(same);BOOL images=sPrivateCaptures.count==requiredPNGs.count;
                for (NSDictionary *row in sPrivateCaptures) images&=[row[@"drawn"] boolValue] && [row[@"written"] boolValue];
                checks[@"capturesComplete"]=@(images && [captured isEqualToSet:[NSSet setWithArray:requiredPNGs]]);
                checks[@"all16Families"]=@(families.count==16);checks[@"all8DragPhases"]=@(phases.count==8);
                checks[@"nativeButtons"]=@(buttons>0);checks[@"staleGenerationRejected"]=@(stale==2);BOOL passed=done && [status isEqual:@"completed"];
                for (NSString *key in requiredKeys) passed&=[checks[key] boolValue];
                NSDictionary *report=@{@"version":@1,@"case":@"pokedex",@"passed":@(passed),@"complete":@(done),@"status":status,@"variant":@(variant),@"stage":@(stage),
                    @"requiredKeys":requiredKeys,@"checks":checks,@"requiredPNGs":requiredPNGs,@"captures":[sPrivateCaptures copy],@"events":events,@"dragEvidence":dragEvidence,
                    @"nativeButtonActions":@(buttons),@"factsBeforeReset":facts,@"captureRegistrationCovered":@NO,@"productionApiAdded":@NO,@"physicalGestureRecognitionVerified":@NO,
                    @"nativePrerequisiteReportSha256":nativeReportSha256 ?: @"",@"nativePrerequisiteModuleSha256":nativeModuleSha256 ?: @"",@"nativePrerequisiteScope":@"sealed compiled-game normal-flow proof; current UIKit is independently checked here",
                    @"noResultsEvidence":[noResultsEvidence copy],@"noResultsSemanticTextVerified":@NO,@"layoutEvidence":[layoutEvidence copy],
                    @"rootVisualReviewPending":@YES,@"productionAllowed":@NO,@"scope":@"Actual original normal Pokédex, three private history variants, sixteen page families and eight production pan-callback leases. No popup, physical recognizer, full-story or audible-audio claim."};
                NSURL *documents=[NSFileManager.defaultManager URLsForDirectory:NSDocumentDirectory inDomains:NSUserDomainMask].firstObject;
                [[NSJSONSerialization dataWithJSONObject:report options:NSJSONWritingPrettyPrinted error:nil] writeToURL:[documents URLByAppendingPathComponent:@"private-pokedex-check.json"] options:NSDataWritingAtomic error:nil];
                stopped=nil;finish=nil;
            };stopped();
        };reset();
    };
    checks[@"caseBinding"]=@([[NSBundle.mainBundle objectForInfoDictionaryKey:@"HGPrivateCheckCase"] isEqual:@"pokedex"]
        && [NSProcessInfo.processInfo.environment[@"HEARTGOLD_CHECK_CASE"] isEqual:@"pokedex"] && [NSProcessInfo.processInfo.environment[@"HEARTGOLD_TEST_MUTE"] isEqual:@"1"]
        && [nativeReportSha256 isEqual:@HG_POKEDEX_NATIVE_REPORT_SHA256]
        && [nativeModuleSha256 isEqual:@HG_POKEDEX_NATIVE_MODULE_SHA256]);
    if (![checks[@"caseBinding"] boolValue] || privateBefore.count!=4) {finish(@"case_or_save_binding_failed");return;}
    sPokedexCheckDisplay=^(uint32_t frame) {
        [self->_condition lock];NSData *owner=[self->_pokedexUIState copy],*qaData=[self->_summaryTMQAState copy],*ownerBag=[self->_bagUIState copy];
        NSDictionary *facts=[self->_summaryTMFacts copy] ?: @{},*audio=self->_audioChanging ? nil : [self->_audio statistics];
        uint32_t map=self->_fieldMapId,reads=self->_summaryTMPadReads,boot=self->_summaryTMBootInputStep,bootFrame=self->_summaryTMBootInputFrame;
        BOOL idle=[self summaryTMInputIdleLocked],accepted=self->_summaryTMRequestAccepted,failed=self->_failure!=nil || self->_finished;
        BOOL raw=!self->_keys && !self->_pendingKeys && !self->_submittedKeys && !self->_joystickKeys && !self->_pendingJoystickKeys && !self->_submittedJoystickKeys && !self->_touchPressed;
        [self->_condition unlock];
        const HGSummaryLearnQAState *q=qaData.length==sizeof(*q) ? qaData.bytes : NULL;
        const HGNativePokedexUIState *s=self->_displayedPokedexUIState.length==sizeof(*s) ? self->_displayedPokedexUIState.bytes : NULL;
        const HGNativeBagUIState *b=self->_displayedBagUIState.length==sizeof(*b) ? self->_displayedBagUIState.bytes : NULL;
        BOOL paired=s && owner.length==sizeof(*s) && !memcmp(s,owner.bytes,sizeof(*s));
        if (!stageFrame) stageFrame=frame;
        if (failed || ++polls>36000 || frame-stageFrame>3600 || events.count>=2048 || [facts[@"fixtureError"] length]) {finish(failed ? @"engine_failed" : [facts[@"fixtureError"] length] ? @"fixture_failed" : @"flow_timeout");return;}
        if (stage!=lastStage || (paired && s->revision!=lastRevision)) {
            [events addObject:@{@"variant":@(variant),@"stage":@(stage),@"frame":@(frame),@"padReads":@(reads),@"phase":@(paired ? s->phase : 0),
                @"generation":@(paired ? s->ownerGeneration : 0),@"revision":@(paired ? s->revision : 0),@"ready":@(paired ? s->ready : 0),@"busyReason":@(paired ? s->busyReason : 0),
                @"qaVersion":@(q ? q->version : 0),@"qaBytes":@(q ? q->byteSize : 0)}];
            lastStage=stage;lastRevision=paired ? s->revision : 0;
        }
        if (paired && s->active) {
            if (!raw || !self->_canvas.hidden || self->_canvas.userInteractionEnabled || !self->_fieldMenu.hidden || self->_pokedexPanel.hidden) {finish(@"raw_input_or_original_screen_exposed");return;}
            checks[@"rawInputBlocked"]=@YES;
            if (s->ready) {
                NSDictionary *pictures=self->_pokedexDisplayPictures;NSData *meta=pictures[@"metadata"];NSArray *images=pictures[@"images"];
                if (meta.length!=sizeof(*s) || memcmp(meta.bytes,s,sizeof(*s)) || images.count!=s->visualCount) {return;}
                BOOL pixels=NO,help=NO,overviewLogo=NO,overviewBall=NO,flavor=NO;uint32_t summaryWindows=0,areaHeaders=0;
                BOOL preview=s->family==HG_POKEDEX_UI_FAMILY_LIST || s->family==HG_POKEDEX_UI_FAMILY_SEARCH_RESULTS;
                for (NSUInteger i=0;i<images.count;i++) {
                    const HGNativePokedexUIVisual *v=&s->visuals[i];CGSize size=v->kind==HG_POKEDEX_UI_VISUAL_WINDOW ? CGSizeMake(v->source.window.tileWidth*8,v->source.window.tileHeight*8) : CGSizeMake(v->source.raster.width,v->source.raster.height);
                    flavor|=v->role==HG_POKEDEX_UI_IMAGE_FLAVOR;
                    if (preview && ((v->role==HG_POKEDEX_UI_IMAGE_FLAVOR && (!s->species || s->caughtState!=2))
                        || (v->role==HG_POKEDEX_UI_IMAGE_SELECTED_NAME && !s->species)
                        || (v->role==HG_POKEDEX_UI_IMAGE_MEASURE_VALUE && !s->species)
                        || (s->family==HG_POKEDEX_UI_FAMILY_SEARCH_RESULTS && (v->role==HG_POKEDEX_UI_IMAGE_COUNT_LABEL || v->role==HG_POKEDEX_UI_IMAGE_COUNT_NUMBER)))) {finish(@"hidden_original_window_exported");return;}
                    if (v->kind==HG_POKEDEX_UI_VISUAL_WINDOW) {
                        help|=v->role==HG_POKEDEX_UI_IMAGE_HELP && !v->index;
                        if (v->index>=1 && v->index<=16) summaryWindows|=1u<<(v->index-1);
                        if (s->family==HG_POKEDEX_UI_FAMILY_AREA && v->role==HG_POKEDEX_UI_IMAGE_HEADER && v->index<8) areaHeaders|=1u<<v->index;
                    }
                    if (s->family==HG_POKEDEX_UI_FAMILY_OVERVIEW && v->kind==HG_POKEDEX_UI_VISUAL_RASTER && !v->index) {
                        overviewLogo|=v->role==HG_POKEDEX_UI_IMAGE_HEADER && !v->source.raster.engine && v->source.raster.x==40 && v->source.raster.y==24 && v->source.raster.width==176 && v->source.raster.height==32;
                        overviewBall|=v->role==HG_POKEDEX_UI_IMAGE_PORTRAIT && v->source.raster.engine==1 && !v->source.raster.x && !v->source.raster.y && v->source.raster.width==256 && v->source.raster.height==160;
                    }
                    if (![images[i] isKindOfClass:UIImage.class] || !CGSizeEqualToSize([images[i] size],size)) {finish(@"original_visual_size_mismatch");return;}
                    pixels|=[self checkPrivatePixels:images[i]];
                }
                if (s->family==HG_POKEDEX_UI_FAMILY_OVERVIEW && (!overviewLogo || !overviewBall)) {finish(@"overview_original_art_missing");return;}
                if (preview && s->species && s->caughtState==2 && !flavor) {finish(@"caught_original_flavor_missing");return;}
                if (s->family==HG_POKEDEX_UI_FAMILY_AREA && areaHeaders!=(s->nationalDex ? 191u : 95u)) {finish(@"area_region_windows_mismatch");return;}
                if ((s->family>=HG_POKEDEX_UI_FAMILY_SEARCH_MENU && s->family<=HG_POKEDEX_UI_FAMILY_SEARCH_BODY && (!help || summaryWindows))
                    || (s->family==HG_POKEDEX_UI_FAMILY_SEARCH_RESULTS && !s->page && !s->cursor && summaryWindows!=65535u)) {finish(@"search_summary_visibility_mismatch");return;}
                if (!pixels) {finish(@"original_visual_pixels_missing");return;}checks[@"originalGlyphs"]=@YES;
                NSString *layoutFailure=[self pokedexLayoutFailure:s evidence:layoutEvidence];if (layoutFailure) {finish(layoutFailure);return;}
                if (s->family && ![families containsIndex:s->family]) {capture([NSString stringWithFormat:@"private-pokedex-family-%02u.png",s->family]);[families addIndex:s->family];}
            }
        }
        if (release) {if (!idle) releaseStarted=NO;else if (!releaseStarted) {releaseStarted=YES;releaseReads=reads;}else if (reads-releaseReads>=2) {release=NO;releaseStarted=NO;}}
        if (dragStage) {
            const HGNativePokedexUIState *panOwner=owner.length==sizeof(*s) ? owner.bytes : NULL;
            if (!s || !s->active || s->ownerGeneration!=panGeneration || s->family!=panFamily || HGNativePokedexUIDragKind(s->family)!=track.kind
                || !panOwner || !panOwner->active || panOwner->ownerGeneration!=panGeneration || panOwner->family!=panFamily || HGNativePokedexUIDragKind(panOwner->family)!=track.kind) {
                [events addObject:@{@"event":@"drag_identity_mismatch",@"variant":@(variant),@"stage":@(stage),@"frame":@(frame),@"paired":@(paired),
                    @"panIdentity":@[@(panGeneration),@(panFamily),@(track.kind)],
                    @"displayPacket":s ? @[@(s->active),@(s->ownerGeneration),@(s->family),@(s->phase),@(s->revision),@(s->frameSerial),@(s->busyReason)] : @[],
                    @"ownerPacket":panOwner ? @[@(panOwner->active),@(panOwner->ownerGeneration),@(panOwner->family),@(panOwner->phase),@(panOwner->revision),@(panOwner->frameSerial),@(panOwner->busyReason)] : @[]}];
                finish(@"drag_owner_or_family_changed");return;
            }
            // A later frame from the same owner does not invalidate the displayed drag.
            if (!paired) return;
            CGFloat extent=track.axis==HG_POKEDEX_UI_AXIS_X ? pan.checkView.bounds.size.width : pan.checkView.bounds.size.height;
            CGFloat delta=((double)panTarget-panStart)*extent/65535.0;pan.checkTranslation=track.axis==HG_POKEDEX_UI_AXIS_X ? CGPointMake(delta,0) : CGPointMake(0,delta);
            pan.checkState=UIGestureRecognizerStateChanged;[self pokedexGesture:pan];
            if (s->phase==dragPhase && s->busyReason==HG_POKEDEX_UI_BUSY_DRAG && s->continuation.kind==track.kind && s->continuation.handle==track.handle) {
                uint32_t target=dragStage==1 ? s->continuation.minimum : s->continuation.maximum;
                if (s->continuation.position==target && (dragStage==1 || s->frameSerial!=dragSerial)) {
                    if (s->family==HG_POKEDEX_UI_FAMILY_AREA || s->family==HG_POKEDEX_UI_FAMILY_LIST) {NSString *layoutFailure=[self pokedexLayoutFailure:s evidence:layoutEvidence];if (layoutFailure) {finish(layoutFailure);return;}}
                    capture([NSString stringWithFormat:@"private-pokedex-drag-%02u-%@.png",dragPhase,dragStage==1 ? @"min" : @"max"]);
                    [dragEvidence addObject:@{@"phase":@(dragPhase),@"generation":@(s->ownerGeneration),@"revision":@(s->revision),@"frameSerial":@(s->frameSerial),@"position":@(s->continuation.position),@"minimum":@(s->continuation.minimum),@"maximum":@(s->continuation.maximum),@"padReads":@(reads),@"beginPadReads":@(dragBeginReads),@"callback":@"pokedexGesture:"}];
                    if (dragStage==1) {dragStage=2;dragSerial=s->frameSerial;panTarget=65535;}
                    else {pan.checkState=UIGestureRecognizerStateEnded;[self pokedexGesture:pan];[phases addIndex:dragPhase];dragStage=0;pan=nil;stage++;substep=0;stageFrame=frame;release=YES;}
                }
            }
            return;
        }
        if (variant==1 && stage==24) {
            if (!paired) {noResultsCandidate=nil;noResultsCandidateFrame=0;return;}
            if (s->active && s->phase==23) {
                // The original pre-draw packet precedes the committed passive-message raster.
                if (s->family==HG_POKEDEX_UI_FAMILY_NONE && s->busyReason==HG_POKEDEX_UI_BUSY_BUILD && !s->ready
                    && !s->keyEnabledMask && !s->controlCount && !s->controlEnabledMask && !s->visualCount
                    && (!noResultsInitial || s->ownerGeneration==[noResultsEvidence[@"generation"] unsignedIntValue])) {
                    noResultsCandidate=nil;noResultsCandidateFrame=0;return;
                }
                NSDictionary *pictures=self->_pokedexDisplayPictures;NSData *metadata=pictures[@"metadata"];
                UIImage *message=pictures[@"passiveMessage"];UIImageView *image=self->_pokedexImages[0];
                BOOL passive=s->family==HG_POKEDEX_UI_FAMILY_NONE && s->busyReason==HG_POKEDEX_UI_BUSY_TRANSITION && !s->ready
                    && !s->keyEnabledMask && !s->controlCount && !s->controlEnabledMask && !s->visualCount
                    && (!noResultsInitial || s->ownerGeneration==[noResultsEvidence[@"generation"] unsignedIntValue])
                    && metadata.length==sizeof(*s) && !memcmp(metadata.bytes,s,sizeof(*s)) && [pictures[@"images"] isKindOfClass:NSArray.class] && ![pictures[@"images"] count]
                    && [message isKindOfClass:UIImage.class] && message.CGImage && CGSizeEqualToSize(message.size,CGSizeMake(224,40))
                    && CGImageGetWidth(message.CGImage)==224 && CGImageGetHeight(message.CGImage)==40
                    && !image.hidden && image.image==message && image.superview==self->_pokedexPanel && !image.userInteractionEnabled
                    && CGRectGetWidth(image.frame)>0 && CGRectGetHeight(image.frame)>0;
                for (UIButton *button in self->_pokedexButtons) passive&=button.hidden && !button.enabled;
                for (UIPanGestureRecognizer *gesture in self->_pokedexPans) passive&=!gesture.enabled;
                for (NSNumber *tag in @[@64,@128,@32,@16,@1,@2]) passive&=!self->_buttons[tag].enabled;
                passive&=self->_buttons[@8].hidden;
                if (!passive) {
                    NSMutableArray *buttons=[NSMutableArray new],*pans=[NSMutableArray new],*keys=[NSMutableArray new];
                    for (UIButton *button in self->_pokedexButtons) [buttons addObject:@[@(button.hidden),@(button.enabled)]];
                    for (UIPanGestureRecognizer *gesture in self->_pokedexPans) [pans addObject:@(gesture.enabled)];
                    for (NSNumber *tag in @[@64,@128,@32,@16,@1,@2,@8]) [keys addObject:@[@(self->_buttons[tag].hidden),@(self->_buttons[tag].enabled)]];
                    [noResultsEvidence addEntriesFromDictionary:@{@"failedDisplayFrame":@(frame),@"failedPacket":@[@(s->phase),@(s->family),@(s->busyReason),@(s->ready),@(s->keyEnabledMask),@(s->controlCount),@(s->controlEnabledMask),@(s->visualCount),@(s->ownerGeneration),@(s->revision),@(s->frameSerial)],
                        @"metadataBytes":@(metadata.length),@"metadataMatches":@(metadata.length==sizeof(*s) && !memcmp(metadata.bytes,s,sizeof(*s))),
                        @"pictureImageCount":@([pictures[@"images"] isKindOfClass:NSArray.class] ? [pictures[@"images"] count] : NSUIntegerMax),
                        @"messageSize":NSStringFromCGSize([message isKindOfClass:UIImage.class] ? message.size : CGSizeZero),
                        @"imageState":@[@(image.hidden),@(image.image==message),@(image.superview==self->_pokedexPanel),@(image.userInteractionEnabled)],
                        @"imageFrame":NSStringFromCGRect(image.frame),@"buttonStates":buttons,@"panStates":pans,@"keyStates":keys}];
                    finish(@"passive_message_display_mismatch");return;
                }
                NSData *bytes=CFBridgingRelease(CGDataProviderCopyData(CGImageGetDataProvider(message.CGImage)));
                if (bytes.length!=224*40*4) {finish(@"passive_message_crop_bytes_mismatch");return;}
                if (!noResultsInitial) {
                    noResultsInitial=bytes;noResultsFrame=frame;
                    [noResultsEvidence addEntriesFromDictionary:@{@"initialDisplayFrame":@(frame),@"initialPacketFrame":@(s->frameSerial),@"generation":@(s->ownerGeneration),@"crop":@[@16,@152,@224,@40]}];
                } else if (frame>noResultsFrame && ![captured containsObject:@"private-pokedex-no-results.png"]) {
                    if ([bytes isEqualToData:noResultsInitial]) {noResultsCandidate=nil;noResultsCandidateFrame=0;}
                    else if (noResultsCandidate && frame>noResultsCandidateFrame && [bytes isEqualToData:noResultsCandidate]) {
                        capture(@"private-pokedex-no-results.png");
                        [noResultsEvidence addEntriesFromDictionary:@{@"candidateDisplayFrame":@(noResultsCandidateFrame),@"capturedDisplayFrame":@(frame),@"capturedPacketFrame":@(s->frameSerial),@"revision":@(s->revision),@"changedFromInitial":@YES,@"consecutiveDisplayPixelsEqual":@YES,@"displayedOriginalCrop":@YES,@"inputDisabled":@YES,@"semanticTextVerified":@NO}];
                    } else {noResultsCandidate=bytes;noResultsCandidateFrame=frame;}
                }
            } else if (noResultsInitial) {
                if (![captured containsObject:@"private-pokedex-no-results.png"]) {finish(@"passive_message_candidate_missing");return;}
                if (q && idle && !release && PokedexCheckFamily(s,3)) {stage++;substep=0;stageFrame=frame;}
            }
            return;
        }
        if (!q || q->version!=HG_SUMMARY_LEARN_QA_VERSION || q->byteSize!=sizeof(*q) || !s || !paired || !idle || release) return;
        BOOL bagReady=b && b->active && b->ready && !b->mode && b->phase==HG_BAG_UI_INPUT && q->owner==HG_SUMMARY_LEARN_BAG && q->phase==b->phase
            && q->execState==2 && q->graphicsReady && q->fadeFinished && ownerBag.length==sizeof(*b) && !memcmp(b,ownerBag.bytes,sizeof(*b));
        BOOL (^fieldIcon)(uint32_t)=^BOOL(uint32_t icon) {
            if (self->_fieldMenu.hidden) {[self toggleMenu];return NO;}
            for (NSUInteger i=0;i<MIN(7,self->_fieldMenuElements.count);i++) if (((self->_fieldMenuIcons>>(i*4))&15)==icon)
                return [self->_fieldMenuElements[i] accessibilityActivate];
            return NO;
        };
        BOOL (^tap)(uint32_t,uint32_t)=^BOOL(uint32_t role,uint32_t index) {
            uint32_t id=PokedexCheckControl(s,role,index,YES);if (id==UINT32_MAX || id>=self->_pokedexButtons.count) return NO;
            UIButton *button=self->_pokedexButtons[id];HGNativePokedexUIInput out;
            if (button.hidden || !button.enabled || CGRectGetWidth(button.frame)<44 || CGRectGetHeight(button.frame)<44 || !button.accessibilityLabel.length
                || !HGNativePokedexUIResolveInput(s,s->ownerGeneration,s->revision,s->phase,HG_POKEDEX_UI_INPUT_CONTROL,id,0,&out)) return NO;
            [button sendActionsForControlEvents:UIControlEventTouchUpInside];buttons++;release=YES;return YES;
        };
        BOOL (^key)(uint32_t)=^BOOL(uint32_t tag) {OriginalKeyButton *button=self->_buttons[@(tag)];if (!button || button.hidden || !button.enabled) return NO;
            BOOL sent=[button accessibilityActivate];if (sent) {buttons++;release=YES;}return sent;};
        BOOL (^goSpecies)(uint32_t)=^BOOL(uint32_t species) {
            if (s->species==species && s->cursor<15 && s->entries[s->cursor].species==species) return YES;
            for (uint32_t i=0;i<15;i++) if (s->entries[i].species==species) {tap(HG_POKEDEX_UI_ROLE_GRID_CELL,i);return NO;}
            if (!substep && s->page) {tap(HG_POKEDEX_UI_ROLE_PAGE,0);return NO;}substep=1;
            if (s->page>=35 || !tap(HG_POKEDEX_UI_ROLE_PAGE,1)) finish(@"species_not_reachable");return NO;
        };
        BOOL (^beginDrag)(uint32_t,uint32_t)=^BOOL(uint32_t role,uint32_t handle) {
            uint32_t id=PokedexCheckControl(s,role,handle,YES);if (id==UINT32_MAX || id>=self->_pokedexButtons.count) return NO;
            HGNativePokedexUIInput out;UIButton *button=self->_pokedexButtons[id];
            if (button.hidden || !button.enabled || !HGNativePokedexUIResolveInput(s,s->ownerGeneration,s->revision,s->phase,HG_POKEDEX_UI_INPUT_GESTURE_BEGIN,id,0,&out)) return NO;
            track=s->controls[id].transport.track;dragPhase=HGNativePokedexUITrackPhase(&track);dragBeginReads=reads;panGeneration=s->ownerGeneration;panFamily=s->family;
            panStart=(track.position-track.minimum)*65535u/(track.maximum-track.minimum);panTarget=0;pan=[PokedexCheckPan new];pan.checkView=button;pan.checkState=UIGestureRecognizerStateBegan;pan.checkTranslation=CGPointZero;
            [self pokedexGesture:pan];dragStage=1;return YES;
        };
        BOOL advance=NO;
        if (!stage) {
            if (boot!=3 || bootFrame || s->active) return;
            checks[@"muted"]=@([audio[@"muted"] boolValue]);BOOL visibleDex=NO;
            for (uint32_t i=0;i<7;i++) visibleDex|=((self->_fieldMenuIcons>>(4*i))&15)==0;
            if (!bootstrap && map==MAP_ROUTE_29 && q->fieldReady && !q->owner) {
                if (visibleDex) {if ([self queuePokedexFixture:variant restoring:NO revision:q->revision]) bootstrap=2;}
                else if (fieldIcon(2)) {bootstrap=1;release=YES;}
            } else if (bootstrap==1 && bagReady) {if ([self queuePokedexFixture:variant restoring:NO revision:q->revision]) bootstrap=2;}
            else if (bootstrap==2 && accepted && [facts[@"fixturePrepared"] boolValue] && [facts[@"variant"] unsignedIntValue]==variant) {
                checks[@"fixtureHistory"]=@([facts[@"fullPartyPreserved"] boolValue] && [facts[@"bagPreserved"] boolValue] && [facts[@"caught"] unsignedIntValue]==1 && [facts[@"forms"] unsignedIntValue]==variant && [facts[@"national"] boolValue]==(variant==3));
                if (![checks[@"fixtureHistory"] boolValue]) {finish(@"fixture_mismatch");return;}
                uint32_t x,y;if (bagReady && HGNativeBagUIResolveTouch(b,b->revision,b->phase,HG_BAG_UI_TOUCH_CANCEL,0,&x,&y)) {UIButton *cancel=self->_bagNavigation[0];if (!cancel.hidden && cancel.enabled) {[cancel sendActionsForControlEvents:UIControlEventTouchUpInside];bootstrap=3;release=YES;}}
                else if (q->fieldReady && !q->owner) bootstrap=3;
            } else if (bootstrap==3 && q->fieldReady && !q->owner && (!b || !b->active) && !s->active && map==MAP_ROUTE_29 && fieldIcon(0)) {stage=1;release=YES;stageFrame=frame;}
            return;
        }
        if (stage==1 && PokedexCheckFamily(s,1)) {
            if (variant>1 && !substep) {HGNativePokedexUIInput out={1,1,1,1};uint32_t id=PokedexCheckControl(s,HG_POKEDEX_UI_ROLE_OPEN_LIST,0,YES);
                if (!oldGeneration || s->ownerGeneration==oldGeneration || id==UINT32_MAX || HGNativePokedexUIResolveInput(s,oldGeneration,s->revision,s->phase,0,id,0,&out) || out.keys || out.touchPressed || out.touchX || out.touchY) {finish(@"old_generation_accepted");return;}stale++;substep=1;}
            if (variant==3 && substep<4) {if (!substep) substep=1;if (tap(HG_POKEDEX_UI_ROLE_DEX_MODE,substep==2 ? 1 : 0)) substep++;return;}
            advance=tap(HG_POKEDEX_UI_ROLE_OPEN_LIST,0);
        } else if (variant==1) {
            switch (stage) {
            case 2: if (PokedexCheckFamily(s,2)) {
                if (!pauseChecked) {
                    uint32_t id=PokedexCheckControl(s,HG_POKEDEX_UI_ROLE_SCROLLBAR,0,YES);if (id==UINT32_MAX) return;
                    PokedexCheckPan *probe=[PokedexCheckPan new];probe.checkView=self->_pokedexButtons[id];probe.checkState=UIGestureRecognizerStateBegan;[self pokedexGesture:probe];
                    [self->_condition lock];BOOL queued=self->_pokedexChoicePending || self->_pokedexInputActive || self->_pokedexGestureAdmitted;[self->_condition unlock];[self pause];
                    [self->_condition lock];checks[@"pauseClearsLease"]=@(queued && !self->_pokedexChoicePending && !self->_pokedexInputActive && !self->_pokedexInputCooling && !self->_pokedexGestureAdmitted && !self->_pokedexGestureLive);[self->_condition unlock];
                    [self resume];pauseChecked=YES;release=YES;return;
                }
                beginDrag(HG_POKEDEX_UI_ROLE_SCROLLBAR,0);
            }break;
            case 3: if (PokedexCheckFamily(s,2)) advance=goSpecies(152);break;
            case 4: if (PokedexCheckFamily(s,2)) advance=tap(HG_POKEDEX_UI_ROLE_ENTRY,0);break;
            case 5: if (PokedexCheckFamily(s,12)) advance=tap(HG_POKEDEX_UI_ROLE_TAB,1);break;
            case 6: if (PokedexCheckFamily(s,13)) advance=tap(HG_POKEDEX_UI_ROLE_REPLAY,0);break;
            case 7: if (PokedexCheckFamily(s,13)) advance=tap(HG_POKEDEX_UI_ROLE_TAB,0);break;
            case 8: if (PokedexCheckFamily(s,12)) advance=tap(HG_POKEDEX_UI_ROLE_ENTRY,0);break;
            case 9: if (PokedexCheckFamily(s,16)) advance=tap(HG_POKEDEX_UI_ROLE_ENTRY_BACK,0);break;
            case 10: if (PokedexCheckFamily(s,2)) {while (areaProbe<=493 && ![facts[@"regionalSeen"] containsObject:@(areaProbe)]) areaProbe++;if (areaProbe>493) {finish(@"area_scroll_capacity_unavailable");return;}if (goSpecies(areaProbe)) {areaTime=0;advance=tap(HG_POKEDEX_UI_ROLE_ENTRY,0);}}break;
            case 11: if (PokedexCheckFamily(s,12)) {if (PokedexCheckControl(s,HG_POKEDEX_UI_ROLE_SCROLLBAR,0,YES)!=UINT32_MAX) beginDrag(HG_POKEDEX_UI_ROLE_SCROLLBAR,0);
                else if (areaTime<3) {uint32_t time=areaTime++;if (s->data.area.time!=time) tap(HG_POKEDEX_UI_ROLE_TIME,time);}
                else if (tap(HG_POKEDEX_UI_ROLE_TAB,3)) {stage=10;substep=0;areaProbe++;stageFrame=frame;}}break;
            case 12: if (PokedexCheckFamily(s,12)) advance=tap(HG_POKEDEX_UI_ROLE_TAB,3);break;
            case 13: if (PokedexCheckFamily(s,2)) advance=tap(HG_POKEDEX_UI_ROLE_OPEN_SEARCH,0);break;
            case 14: if (PokedexCheckFamily(s,3)) advance=tap(HG_POKEDEX_UI_ROLE_CRITERION,0);break;
            case 15: if (PokedexCheckFamily(s,4)) advance=tap(HG_POKEDEX_UI_ROLE_OPTION,1);break;
            case 16: if (PokedexCheckFamily(s,4)) advance=tap(HG_POKEDEX_UI_ROLE_CANCEL,0);break;
            case 17: if (PokedexCheckFamily(s,3)) advance=tap(HG_POKEDEX_UI_ROLE_CRITERION,1);break;
            case 18: if (PokedexCheckFamily(s,5)) advance=tap(HG_POKEDEX_UI_ROLE_OPTION,0);break;
            case 19: if (PokedexCheckFamily(s,5)) advance=tap(HG_POKEDEX_UI_ROLE_CANCEL,0);break;
            case 20: if (PokedexCheckFamily(s,3)) advance=tap(HG_POKEDEX_UI_ROLE_CRITERION,2);break;
            case 21: if (PokedexCheckFamily(s,6)) advance=tap(HG_POKEDEX_UI_ROLE_OPTION,16);break;
            case 22: if (PokedexCheckFamily(s,6)) advance=tap(HG_POKEDEX_UI_ROLE_ACCEPT,0);break;
            case 23: if (PokedexCheckFamily(s,3)) advance=tap(HG_POKEDEX_UI_ROLE_SEARCH,0);break;
            case 25: if (PokedexCheckFamily(s,3)) advance=tap(HG_POKEDEX_UI_ROLE_RESET,0);break;
            case 26: if (PokedexCheckFamily(s,3)) advance=tap(HG_POKEDEX_UI_ROLE_CRITERION,3);break;
            case 27: if (PokedexCheckFamily(s,7)) beginDrag(HG_POKEDEX_UI_ROLE_RANGE_HANDLE,0);break;
            case 28: if (PokedexCheckFamily(s,7)) beginDrag(HG_POKEDEX_UI_ROLE_RANGE_HANDLE,1);break;
            case 29: if (PokedexCheckFamily(s,7)) advance=tap(HG_POKEDEX_UI_ROLE_CANCEL,0);break;
            case 30: if (PokedexCheckFamily(s,3)) advance=tap(HG_POKEDEX_UI_ROLE_CRITERION,4);break;
            case 31: if (PokedexCheckFamily(s,8)) beginDrag(HG_POKEDEX_UI_ROLE_RANGE_HANDLE,0);break;
            case 32: if (PokedexCheckFamily(s,8)) beginDrag(HG_POKEDEX_UI_ROLE_RANGE_HANDLE,1);break;
            case 33: if (PokedexCheckFamily(s,8)) advance=tap(HG_POKEDEX_UI_ROLE_CANCEL,0);break;
            case 34: if (PokedexCheckFamily(s,3)) advance=tap(HG_POKEDEX_UI_ROLE_CRITERION,5);break;
            case 35: if (PokedexCheckFamily(s,9)) advance=tap(HG_POKEDEX_UI_ROLE_OPTION,2);break;
            case 36: if (PokedexCheckFamily(s,9)) advance=tap(HG_POKEDEX_UI_ROLE_CANCEL,0);break;
            case 37: if (PokedexCheckFamily(s,3)) advance=tap(HG_POKEDEX_UI_ROLE_CRITERION,6);break;
            case 38: if (PokedexCheckFamily(s,10)) advance=tap(HG_POKEDEX_UI_ROLE_OPTION,0);break;
            case 39: if (PokedexCheckFamily(s,10)) advance=tap(HG_POKEDEX_UI_ROLE_CANCEL,0);break;
            case 40: if (PokedexCheckFamily(s,3)) advance=tap(HG_POKEDEX_UI_ROLE_SEARCH,0);break;
            case 41: if (PokedexCheckFamily(s,11)) beginDrag(HG_POKEDEX_UI_ROLE_SCROLLBAR,0);break;
            case 42: if (PokedexCheckFamily(s,11)) advance=goSpecies(152);break;
            case 43: if (PokedexCheckFamily(s,11)) advance=tap(HG_POKEDEX_UI_ROLE_ENTRY,0);break;
            case 44: if (PokedexCheckFamily(s,12) && s->entryFromSearch) advance=tap(HG_POKEDEX_UI_ROLE_TAB,3);break;
            case 45: if (PokedexCheckFamily(s,11)) advance=tap(HG_POKEDEX_UI_ROLE_OPEN_SEARCH,0);break;
            case 46: if (PokedexCheckFamily(s,3)) advance=key(2);break;
            case 47: if (PokedexCheckFamily(s,2)) advance=goSpecies(201);break;
            case 48: if (PokedexCheckFamily(s,2)) advance=tap(HG_POKEDEX_UI_ROLE_ENTRY,0);break;
            case 49: if (PokedexCheckFamily(s,12)) advance=tap(HG_POKEDEX_UI_ROLE_TAB,2);break;
            case 50: if (PokedexCheckFamily(s,14) && s->data.forms.count==1) advance=tap(HG_POKEDEX_UI_ROLE_TAB,3);break;
            case 51: if (PokedexCheckFamily(s,2)) {oldGeneration=s->ownerGeneration;advance=tap(HG_POKEDEX_UI_ROLE_EXIT,0);}break;
            }
        } else {
            switch (stage) {
            case 2: if (PokedexCheckFamily(s,2)) advance=goSpecies(201);break;
            case 3: if (PokedexCheckFamily(s,2)) advance=tap(HG_POKEDEX_UI_ROLE_ENTRY,0);break;
            case 4: if (PokedexCheckFamily(s,12)) advance=tap(HG_POKEDEX_UI_ROLE_TAB,2);break;
            case 5: if (PokedexCheckFamily(s,14) && s->data.forms.count==variant) {capture([NSString stringWithFormat:@"private-pokedex-u%u-forms.png",variant]);if (variant==3) beginDrag(HG_POKEDEX_UI_ROLE_SCROLLBAR,0);else advance=YES;}break;
            case 6: if (PokedexCheckFamily(s,14)) advance=tap(HG_POKEDEX_UI_ROLE_COMPARE,0);break;
            case 7: if (PokedexCheckFamily(s,15) && s->data.forms.left!=s->data.forms.right) {capture([NSString stringWithFormat:@"private-pokedex-u%u-compare.png",variant]);advance=tap(HG_POKEDEX_UI_ROLE_FACING,0);}break;
            case 8: if (PokedexCheckFamily(s,15)) advance=variant==3 ? tap(HG_POKEDEX_UI_ROLE_SIDE,1) : YES;break;
            case 9: if (PokedexCheckFamily(s,15)) advance=variant==3 ? tap(HG_POKEDEX_UI_ROLE_FORM_STEP,2) : YES;break;
            case 10: if (PokedexCheckFamily(s,15) && s->data.forms.left!=s->data.forms.right) advance=tap(HG_POKEDEX_UI_ROLE_CANCEL,0);break;
            case 11: if (PokedexCheckFamily(s,14)) advance=tap(HG_POKEDEX_UI_ROLE_TAB,3);break;
            case 12: if (PokedexCheckFamily(s,2)) {oldGeneration=s->ownerGeneration;advance=tap(HG_POKEDEX_UI_ROLE_EXIT,0);}break;
            }
        }
        if (advance) {stage++;substep=0;stageFrame=frame;}
        BOOL returned=stage==(variant==1 ? 52u : 13u);
        if (returned && !s->active && (!b || !b->active) && q->fieldReady && !q->owner && !q->manager && map==MAP_ROUTE_29) {
            BOOL preserved=[facts[@"fullPartyPreserved"] boolValue] && [facts[@"bagPreserved"] boolValue] && [facts[@"flagsPreserved"] boolValue] && [facts[@"dexPreserved"] boolValue] && [facts[@"moneyPreserved"] boolValue];
            if (!preserved) {finish(@"field_return_preservation_failed");return;}
            if (![facts[@"fixturePrepared"] boolValue]) {
                if (variant<3) {variant++;stage=0;bootstrap=0;substep=0;stageFrame=frame;return;}
                if (self->_fieldMenuOpen) {[self toggleMenu];fieldFrame=frame;return;}
                if (!fieldFrame) {fieldFrame=frame;return;}
                BOOL field=!self->_canvas.hidden && self->_canvas.userInteractionEnabled && self->_canvas.image.size.width==self->_frameWidth
                    && self->_canvas.image.size.height==192 && [self checkPrivatePixels:self->_canvas.image] && frame>fieldFrame && self->_pokedexPanel.hidden && self->_joystick.userInteractionEnabled;
                if (!field) return;checks[@"returnedField"]=@YES;checks[@"fieldControlsRestored"]=@(self->_buttons[@1].enabled && self->_buttons[@2].enabled && self->_buttons[@1].alpha==1 && self->_buttons[@2].alpha==1);
                checks[@"partyBagHistoryPreserved"]=@YES;checks[@"threeVariants"]=@YES;capture(@"private-pokedex-field-return.png");finish(@"completed");return;
            }
            [self queuePokedexFixture:variant restoring:YES revision:q->revision];
        }
    };
}
@end
#endif
