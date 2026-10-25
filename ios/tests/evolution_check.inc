// SPDX-License-Identifier: GPL-3.0-or-later
// Separate private UIKit cases; the existing default/TM/Candy check bodies stay unchanged.
#ifdef HEARTGOLD_ORIGINAL_EVOLUTION_CHECK
@implementation OriginalController (EvolutionChecks)
- (BOOL)prepareEvolutionFixture:(const HGSummaryLearnQAState *)q {
#define EV_REQUIRE(value,message) do { if (!(value)) { _summaryTMFixtureError=(message); return NO; } } while (0)
    uint32_t save=w2c_hgengine_SaveData_Get(_game),party=w2c_hgengine_SaveArray_Party_Get(_game,save);
    _summaryTMFlags=w2c_hgengine_Save_VarsFlags_Get(_game,save);_summaryTMBag=w2c_hgengine_Save_Bag_Get(_game,save);
    _summaryTMProfile=w2c_hgengine_Save_PlayerData_GetProfile(_game,save);_summaryTMDex=w2c_hgengine_Save_Pokedex_Get(_game,save);
    EV_REQUIRE(q && q->fieldReady && !q->owner && q->mapSection && w2c_hgengine_CheckGotStarter(_game,_summaryTMFlags)
        && w2c_hgengine_Party_GetCount(_game,party)==1,@"evolution_original_field_party");
    _summaryTMMon=w2c_hgengine_Party_GetMonByIndex(_game,party,0);
    EV_REQUIRE(w2c_hgengine_GetMonData(_game,_summaryTMMon,MON_DATA_SPECIES,0)==152
        && w2c_hgengine_GetMonData(_game,_summaryTMMon,MON_DATA_LEVEL,0)==5
        && w2c_hgengine_GetMonData(_game,_summaryTMMon,MON_DATA_HP,0)==20 && w2c_hgengine_GetMonData(_game,_summaryTMMon,MON_DATA_MAX_HP,0)==20
        && !w2c_hgengine_GetMonData(_game,_summaryTMMon,MON_DATA_HELD_ITEM,0)
        && !w2c_hgengine_Bag_GetQuantity(_game,_summaryTMBag,ITEM_RARE_CANDY,HEAP_ID_FIELD2),@"evolution_original_starter");
    _summaryTMOriginalPersonality=w2c_hgengine_GetMonData(_game,_summaryTMMon,MON_DATA_PERSONALITY,0);
    _summaryTMOriginalOtId=w2c_hgengine_GetMonData(_game,_summaryTMMon,MON_DATA_OT_ID,0);
    EV_REQUIRE(_summaryTMOriginalOtId==w2c_hgengine_PlayerProfile_GetTrainerID(_game,_summaryTMProfile),@"evolution_original_ot");
    uint16_t name[PLAYER_NAME_LENGTH+1];memcpy(name,[self summaryTMGuest:_summaryTMProfile+offsetof(PlayerProfile,name) size:sizeof(name)],sizeof(name));
    _summaryTMNameLength=0;while (_summaryTMNameLength<PLAYER_NAME_LENGTH+1 && name[_summaryTMNameLength]!=EOS) _summaryTMNameLength++;
    EV_REQUIRE(_summaryTMNameLength && _summaryTMNameLength<=PLAYER_NAME_LENGTH,@"evolution_bounded_ot_name");
    uint32_t scratch=w2c_hgengine_HGNativeFrameBuffer(_game);[self summaryTMGuest:scratch size:sizeof(name)];
    w2c_hgengine_GetMonData(_game,_summaryTMMon,MON_DATA_OT_NAME,scratch);
    EV_REQUIRE(!memcmp(name,[self summaryTMGuest:scratch size:sizeof(name)],(_summaryTMNameLength+1)*sizeof(uint16_t)),@"evolution_ot_profile_match");
    EV_REQUIRE(w2c_hgengine_GetMonData(_game,_summaryTMMon,MON_DATA_FRIENDSHIP,0)==70 && !w2c_hgengine_GetMonData(_game,_summaryTMMon,MON_DATA_MOOD,0)
        && !w2c_hgengine_Pokedex_CheckMonSeenFlag(_game,_summaryTMDex,153) && !w2c_hgengine_Pokedex_CheckMonCaughtFlag(_game,_summaryTMDex,153),@"evolution_baseline_metadata");
    uint32_t experience=2035;memcpy((void *)[self summaryTMGuest:scratch size:sizeof(experience)],&experience,sizeof(experience));
    w2c_hgengine_SetMonData(_game,_summaryTMMon,MON_DATA_EXPERIENCE,scratch);w2c_hgengine_CalcMonLevelAndStats(_game,_summaryTMMon);
    EV_REQUIRE(w2c_hgengine_GetMonData(_game,_summaryTMMon,MON_DATA_LEVEL,0)==15 && w2c_hgengine_GetMonData(_game,_summaryTMMon,MON_DATA_EXPERIENCE,0)==2035
        && w2c_hgengine_GetMonData(_game,_summaryTMMon,MON_DATA_HP,0)==w2c_hgengine_GetMonData(_game,_summaryTMMon,MON_DATA_MAX_HP,0),@"evolution_original_level15_calculation");
    EV_REQUIRE(w2c_hgengine_Bag_AddItem(_game,_summaryTMBag,ITEM_RARE_CANDY,2,HEAP_ID_FIELD2),@"evolution_private_candy");
    for (NSUInteger i=0;i<26;i++) _summaryTMMonBaseline[i]=w2c_hgengine_GetMonData(_game,_summaryTMMon,SummaryTMAttributes[i],0);
    EV_REQUIRE(_summaryTMMonBaseline[0]==_summaryTMOriginalPersonality && _summaryTMMonBaseline[1]==_summaryTMOriginalOtId
        && _summaryTMMonBaseline[10]==33 && _summaryTMMonBaseline[11]==45 && !_summaryTMMonBaseline[12] && !_summaryTMMonBaseline[13],@"evolution_existing_identity_moves");
    uint32_t bagSize=w2c_hgengine_Save_Bag_sizeof(_game),flagsSize=w2c_hgengine_Save_VarsFlags_sizeof(_game);
    EV_REQUIRE(bagSize && bagSize<65536 && flagsSize && flagsSize<65536,@"evolution_baseline_sizes");
    _summaryTMBagBaseline=[NSData dataWithBytes:[self summaryTMGuest:_summaryTMBag size:bagSize] length:bagSize];
    _summaryTMFlagsBaseline=[NSData dataWithBytes:[self summaryTMGuest:_summaryTMFlags size:flagsSize] length:flagsSize];_summaryTMTmOffset=bagSize;
    for (NSUInteger i=0;i<NUM_BAG_MEDICINE;i++) { uint32_t slot=w2c_hgengine_Bag_GetPocketSlotN(_game,_summaryTMBag,POCKET_MEDICINE,i);uint16_t item[2];
        memcpy(item,[self summaryTMGuest:slot size:sizeof(item)],sizeof(item));if (item[0]==ITEM_RARE_CANDY) {
            EV_REQUIRE(item[1]==2 && slot>=_summaryTMBag && slot-_summaryTMBag<=bagSize-4,@"evolution_candy_slot");_summaryTMTmOffset=slot-_summaryTMBag;break; } }
    EV_REQUIRE(_summaryTMTmOffset<bagSize,@"evolution_candy_slot_missing");
    _summaryTMMoney=w2c_hgengine_PlayerProfile_GetMoney(_game,_summaryTMProfile);_summaryTMDexEnabled=w2c_hgengine_Pokedex_IsEnabled(_game,_summaryTMDex);
    _summaryTMDexSeen=w2c_hgengine_Pokedex_CountNationalDexSeen(_game,_summaryTMDex);_summaryTMDexOwned=w2c_hgengine_Pokedex_CountNationalDexOwned(_game,_summaryTMDex);
    _summaryTMMapSection=q->mapSection;_summaryTMFixturePrepared=YES;return YES;
#undef EV_REQUIRE
}
- (NSDictionary *)evolutionCurrentFacts {
    NSMutableDictionary *facts=[@{@"fixturePrepared":@(_summaryTMFixturePrepared),@"fixtureError":_summaryTMFixtureError ?: @"",@"rawPresses":@(_summaryTMRawPresses),@"rawReleases":@(_summaryTMRawReleases),@"rawRejected":@(_summaryTMRawRejected)} mutableCopy];
    if (!_summaryTMFixturePrepared) return facts;
    uint32_t current[26];BOOL initial=YES,moves=YES;for (NSUInteger i=0;i<26;i++) {
        current[i]=w2c_hgengine_GetMonData(_game,_summaryTMMon,SummaryTMAttributes[i],0);initial&=current[i]==_summaryTMMonBaseline[i];if (i>=10) moves&=current[i]==_summaryTMMonBaseline[i]; }
    uint32_t experience=w2c_hgengine_GetMonData(_game,_summaryTMMon,MON_DATA_EXPERIENCE,0),quantity=w2c_hgengine_Bag_GetQuantity(_game,_summaryTMBag,ITEM_RARE_CANDY,HEAP_ID_FIELD2);
    BOOL bagSame=!memcmp(_summaryTMBagBaseline.bytes,[self summaryTMGuest:_summaryTMBag size:_summaryTMBagBaseline.length],_summaryTMBagBaseline.length);
    NSMutableData *consumed=[_summaryTMBagBaseline mutableCopy];uint16_t one=1;memcpy((uint8_t *)consumed.mutableBytes+_summaryTMTmOffset+2,&one,2);
    BOOL bagConsumed=quantity==1 && !memcmp(consumed.bytes,[self summaryTMGuest:_summaryTMBag size:consumed.length],consumed.length);
    BOOL flags=!memcmp(_summaryTMFlagsBaseline.bytes,[self summaryTMGuest:_summaryTMFlags size:_summaryTMFlagsBaseline.length],_summaryTMFlagsBaseline.length)
        && w2c_hgengine_PlayerProfile_GetMoney(_game,_summaryTMProfile)==_summaryTMMoney && w2c_hgengine_Pokedex_IsEnabled(_game,_summaryTMDex)==_summaryTMDexEnabled;
    [facts addEntriesFromDictionary:@{@"species":@(current[2]),@"level":@(current[8]),@"experience":@(experience),@"hp":@(current[6]),@"maxHp":@(current[7]),@"candyQuantity":@(quantity),
        @"originalIdentityPreserved":@(current[0]==_summaryTMOriginalPersonality && current[1]==_summaryTMOriginalOtId),@"movesAndPPPreserved":@(moves),
        @"originalPersonality":@(_summaryTMOriginalPersonality),@"originalOtId":@(_summaryTMOriginalOtId),@"otNameLength":@(_summaryTMNameLength),
        @"initialUnchanged":@(initial && experience==2035 && quantity==2 && bagSame),@"bagOnlyOneCandyConsumed":@(bagConsumed),@"flagsMoneyPreserved":@(flags),
        @"bayleefSeen":@(w2c_hgengine_Pokedex_CheckMonSeenFlag(_game,_summaryTMDex,153)),@"bayleefCaught":@(w2c_hgengine_Pokedex_CheckMonCaughtFlag(_game,_summaryTMDex,153)),
        @"dexSeenDelta":@(w2c_hgengine_Pokedex_CountNationalDexSeen(_game,_summaryTMDex)-_summaryTMDexSeen),@"dexCaughtDelta":@(w2c_hgengine_Pokedex_CountNationalDexOwned(_game,_summaryTMDex)-_summaryTMDexOwned)}];return facts;
}
- (void)checkEvolutionCase {
    NSString *kind=[NSBundle.mainBundle objectForInfoDictionaryKey:@"HGPrivateCheckCase"] ?: @"";BOOL cancel=[kind isEqual:@"evolution-cancel"];
    NSString *prefix=cancel ? @"private-evolution-cancel-" : @"private-evolution-";
    NSArray *requiredKeys=@[@"caseBinding",@"muted",@"fixtureIdentity",@"fixtureLevel15",@"originalLevel16",@"statsDifference",@"statsTotal",@"originalEvolutionScene",@"canonicalWidth",@"typedCancelAvailable",@"originalOutcome",@"nativeEvolutionButtons",@"rawInputZero",@"staleRejected",@"inputLeaseReleased",@"returnedBag",@"returnedField",@"fieldRunButtonRouting",@"privateSave4Preserved",@"resetQuiescent",@"capturesComplete"];
    NSArray *suffixes=@[@"target.png",@"level-message.png",@"stats-difference.png",@"stats-total.png",@"oscillate.png",@"oscillate-later.png",@"result.png",@"final-bag.png",@"field-return.png"];
    NSMutableArray *requiredPNGs=[NSMutableArray new];for (NSString *name in suffixes) [requiredPNGs addObject:[prefix stringByAppendingString:name]];
    NSMutableDictionary *checks=[NSMutableDictionary new];for (NSString *key in requiredKeys) checks[key]=@NO;
    NSMutableArray *events=[NSMutableArray new];NSMutableSet *captured=[NSMutableSet new];
    NSURL *support=[NSFileManager.defaultManager URLsForDirectory:NSApplicationSupportDirectory inDomains:NSUserDomainMask].firstObject;
    support=[support URLByAppendingPathComponent:@"HeartGoldOriginal" isDirectory:YES];NSMutableDictionary *privateBefore=[NSMutableDictionary new];
    for (NSString *suffix in @[@"",@".console",@".wifi",@".lock"]) {NSString *name=[@"heartgold.sav" stringByAppendingString:suffix];NSData *d=[NSData dataWithContentsOfURL:[support URLByAppendingPathComponent:name]];if (d) privateBefore[name]=d;}
    [_condition lock];sPrivateUICase=kind;sPrivateCaptures=[NSMutableArray new];sPrivateObserving=NO;[_condition unlock];
    __block NSUInteger stage=0,polls=0,lastStage=NSNotFound,evolutionActions=0,ackActions=0,cancelActions=0,busySamples=0;__block uint32_t lastRevision=0,releaseReads=0,oscillateFrame=0,fieldFrame=0,resultFrame=0;
    __block BOOL finishing=NO,release=NO,releaseStarted=NO,rawZero=YES;__block NSData *firstPixels=nil;__block UIImage *resultImage=nil;
    __block void (^step)(void);
    void (^finish)(NSString *)=^(NSString *status) {
        if (finishing) return;finishing=YES;step=nil;[self->_condition lock];NSDictionary *facts=[self->_summaryTMFacts copy] ?: @{};
        uint32_t boots=self->_bootCount;self->_checkResetPending=YES;[self->_condition unlock];
        __block NSUInteger waits=0;__block void (^resetStep)(void);resetStep=^{
            [self->_condition lock];BOOL restarted=self->_bootCount==boots+1 && self->_fieldMapId==MAP_ROUTE_29 && self->_publishedFrames>self->_bootFrameBase;
            BOOL clean=self->_resetQuiesced && self->_restartClean,failed=self->_failure!=nil || self->_finished;[_condition unlock];
            const HGNativeEvolutionUIState *resetEvolution=self->_displayedEvolutionUIState.length==sizeof(HGNativeEvolutionUIState) ? self->_displayedEvolutionUIState.bytes : NULL;
            restarted&=!self->_evolutionScene && resetEvolution && !resetEvolution->active;
            clean&=self->_buttons[@1].enabled && self->_buttons[@2].enabled && self->_buttons[@1].alpha==1 && self->_buttons[@2].alpha==1;
            if (!restarted && !failed && ++waits<200) {dispatch_after(dispatch_time(DISPATCH_TIME_NOW,NSEC_PER_SEC/10),dispatch_get_main_queue(),resetStep);return;}
            checks[@"resetQuiescent"]=@(restarted && clean);resetStep=nil;[self stop];__block NSUInteger stops=0;__block void (^stopStep)(void);stopStep=^{
                [self->_condition lock];BOOL stopped=self->_finished && !self->_env && !self->_game && !self->_wasi;[self->_condition unlock];
                if (!stopped && ++stops<100) {dispatch_after(dispatch_time(DISPATCH_TIME_NOW,NSEC_PER_SEC/10),dispatch_get_main_queue(),stopStep);return;}
                checks[@"resetQuiescent"]=@([checks[@"resetQuiescent"] boolValue] && stopped);BOOL preserved=privateBefore.count==4;
                for (NSString *name in privateBefore) preserved&=[privateBefore[name] isEqualToData:[NSData dataWithContentsOfURL:[support URLByAppendingPathComponent:name]]];checks[@"privateSave4Preserved"]=@(preserved);
                NSMutableSet *names=[NSMutableSet new];BOOL complete=sPrivateCaptures.count==requiredPNGs.count;
                for (NSDictionary *capture in sPrivateCaptures) {complete&=[capture[@"drawn"] boolValue] && [capture[@"written"] boolValue];[names addObject:capture[@"file"]];}
                checks[@"capturesComplete"]=@(complete && [names isEqualToSet:[NSSet setWithArray:requiredPNGs]]);BOOL passed=[status isEqual:@"completed"];
                for (NSString *key in requiredKeys) passed&=[checks[key] boolValue];NSDictionary *report=@{@"version":@1,@"case":kind,@"complete":@(stopped),@"passed":@(passed),@"status":status,
                    @"stage":@(stage),@"polls":@(polls),@"checks":checks,@"requiredKeys":requiredKeys,@"requiredPNGs":requiredPNGs,@"captures":[sPrivateCaptures copy],@"events":events,@"factsBeforeReset":facts,
                    @"cancelRequested":@(cancel),@"evolutionButtonActions":@(evolutionActions),@"ackButtonActions":@(ackActions),@"cancelButtonActions":@(cancelActions),@"busySamples":@(busySamples),@"resultSourceDisplayFrame":@(resultFrame),@"particleContributionVerified":@NO,@"learningMenusCovered":@NO,@"summaryChildCovered":@NO,
                    @"productionAllowed":@NO,@"rootVisualReviewPending":@YES,@"scope":@"Actual private Lv15-to16 original Candy/Evolution UIKit route. No Evolution-learning menus, SPL contribution, full-game, audible-audio or physical-device claim."};
                NSURL *folder=[NSFileManager.defaultManager URLsForDirectory:NSDocumentDirectory inDomains:NSUserDomainMask].firstObject;
                [[NSJSONSerialization dataWithJSONObject:report options:NSJSONWritingPrettyPrinted error:nil] writeToURL:[folder URLByAppendingPathComponent:[@"private-" stringByAppendingFormat:@"%@-check.json",kind]] options:NSDataWritingAtomic error:nil];stopStep=nil;
            };stopStep();
        };resetStep();
    };
    checks[@"caseBinding"]=@(([kind isEqual:@"evolution"] || cancel) && [NSProcessInfo.processInfo.environment[@"HEARTGOLD_CHECK_CASE"] isEqual:kind] && [NSProcessInfo.processInfo.environment[@"HEARTGOLD_TEST_MUTE"] isEqual:@"1"]);
    if (![checks[@"caseBinding"] boolValue] || privateBefore.count!=4) {finish(@"case_or_save_binding_failed");return;}
    step=^{
        [self->_condition lock];NSData *qaData=[self->_summaryTMQAState copy],*ownerBag=[self->_bagUIState copy],*ownerParty=[self->_partyUIState copy],*ownerEvo=[self->_evolutionUIState copy],*geometry=[self->_geometryState copy];
        NSDictionary *facts=[self->_summaryTMFacts copy] ?: @{},*audio=self->_audioChanging ? nil : [self->_audio statistics];
        uint32_t map=self->_fieldMapId,reads=self->_summaryTMPadReads,boot=self->_summaryTMBootInputStep,bootInput=self->_summaryTMBootInputFrame,ownerWidth=self->_frameWidth;
        BOOL menuOpen=self->_fieldMenuOpen,failed=self->_failure!=nil || self->_finished,accepted=self->_summaryTMRequestAccepted;
        BOOL raw=!self->_keys && !self->_pendingKeys && !self->_submittedKeys && !self->_joystickKeys && !self->_pendingJoystickKeys && !self->_submittedJoystickKeys && !self->_touchPressed;
        BOOL idle=[self summaryTMInputIdleLocked];[self->_condition unlock];
        const HGSummaryLearnQAState *q=qaData.length==sizeof(*q) ? qaData.bytes : NULL;
        const HGNativeBagUIState *bag=self->_displayedBagUIState.length==sizeof(*bag) ? self->_displayedBagUIState.bytes : NULL;
        const HGNativePartyUIState *party=self->_displayedPartyUIState.length==sizeof(*party) ? self->_displayedPartyUIState.bytes : NULL;
        const HGNativeEvolutionUIState *evo=self->_displayedEvolutionUIState.length==sizeof(*evo) ? self->_displayedEvolutionUIState.bytes : NULL;
        BOOL ev=evo && evo->active && ownerEvo.length==sizeof(*evo) && !memcmp(evo,ownerEvo.bytes,offsetof(HGNativeEvolutionUIState,message));
        if (failed || ++polls>3600 || events.count>=1024 || [facts[@"fixtureError"] length]) {finish(failed ? @"engine_failed" : [facts[@"fixtureError"] length] ? @"fixture_failed" : @"flow_timeout");return;}
        if (q && (q->version!=HG_SUMMARY_LEARN_QA_VERSION || q->byteSize!=sizeof(*q))) {finish(@"observer_abi_mismatch");return;}
        if (lastStage!=stage || (ev && evo->revision!=lastRevision)) { [events addObject:@{@"event":@"state",@"stage":@(stage),@"displayFrame":@(sPrivateDisplayFrame),@"phase":@(ev ? evo->phase : 0),@"revision":@(ev ? evo->revision : 0),@"mask":@(ev ? evo->actionMask : 0),@"padReads":@(reads)}];lastStage=stage;lastRevision=ev ? evo->revision : 0; }
        if (ev) {busySamples++;rawZero&=raw;if (!raw || self->_canvas.userInteractionEnabled) {finish(@"evolution_raw_input_exposed");return;}}
        if (release) {if (!idle) releaseStarted=NO;else if (!releaseStarted) {releaseStarted=YES;releaseReads=reads;}else if (reads-releaseReads>=2) {release=NO;releaseStarted=NO;}}
        if (!q || !idle || release) {dispatch_after(dispatch_time(DISPATCH_TIME_NOW,NSEC_PER_SEC/10),dispatch_get_main_queue(),step);return;}
        BOOL bagReady=bag && bag->active && bag->ready && !bag->mode && q->owner==HG_SUMMARY_LEARN_BAG && q->phase==bag->phase && q->execState==2 && q->graphicsReady && q->fadeFinished
            && ownerBag.length==sizeof(*bag) && !memcmp(bag,ownerBag.bytes,sizeof(*bag));
        BOOL partyReady=party && party->ready && HGNativePartyUIIsRareCandy(party) && q->owner==HG_SUMMARY_LEARN_PARTY && q->phase==party->phase && q->graphicsReady && q->fadeFinished
            && ownerParty.length==sizeof(*party) && !memcmp(party,ownerParty.bytes,sizeof(*party));
        NSString *capture=nil;UIImage *captureImage=nil;UIButton *button=nil;BOOL evoButton=NO;uint32_t x,y,k;
        if (!stage && q->fieldReady && !q->owner && map==MAP_ROUTE_29 && boot==3 && !bootInput) {checks[@"muted"]=@([audio[@"muted"] boolValue]);if ([self queueSummaryTMFixture:q->revision]) stage=1;}
        else if (stage==1 && accepted && [facts[@"fixturePrepared"] boolValue] && q->fieldReady && !q->owner) {
            checks[@"fixtureIdentity"]=@([facts[@"originalIdentityPreserved"] boolValue] && [facts[@"otNameLength"] unsignedIntValue]>=1 && [facts[@"otNameLength"] unsignedIntValue]<=7);
            checks[@"fixtureLevel15"]=@([facts[@"initialUnchanged"] boolValue] && [facts[@"species"] unsignedIntValue]==152 && [facts[@"level"] unsignedIntValue]==15 && [facts[@"experience"] unsignedIntValue]==2035);
            if (![checks[@"fixtureIdentity"] boolValue] || ![checks[@"fixtureLevel15"] boolValue]) {finish(@"fixture_mismatch");return;}
            if (!menuOpen) [self toggleMenu];else if (!self->_fieldMenu.hidden) for (NSUInteger i=0;i<7;i++) if (((self->_fieldMenuIcons>>(4*i))&15)==2 && [self->_fieldMenuElements[i] accessibilityActivate]) {stage=2;break;}
        } else if (stage==2 && bagReady && HGNativeBagUIIsList(bag)) {
            for (uint32_t i=0;i<HG_BAG_UI_POCKETS;i++) if (bag->pockets[i].enabled && bag->pockets[i].pocketId==POCKET_MEDICINE && HGNativeBagUIResolveTouch(bag,bag->revision,bag->phase,HG_BAG_UI_TOUCH_POCKET,i,&x,&y)) {button=self->_bagPockets[i];stage=3;break;}
        } else if (stage==3 && bagReady && HGNativeBagUIIsList(bag) && bag->pocketId==POCKET_MEDICINE) {
            for (uint32_t i=0;i<MIN(bag->visibleCount,HG_BAG_UI_ITEMS);i++) if (bag->items[i].enabled && bag->items[i].itemId==50 && bag->items[i].quantity==2 && HGNativeBagUIResolveTouch(bag,bag->revision,bag->phase,HG_BAG_UI_TOUCH_ITEM,i,&x,&y)) {button=self->_bagItems[i];stage=4;break;}
        } else if (stage==4 && bagReady && bag->phase==HG_BAG_UI_ITEM_MENU && bag->selectedItemId==50) {
            for (uint32_t i=0;i<MIN(bag->actionCount,HG_BAG_UI_ACTIONS);i++) if (bag->actions[i].enabled && !bag->actions[i].actionId && HGNativeBagUIResolveTouch(bag,bag->revision,bag->phase,HG_BAG_UI_TOUCH_ACTION,i,&x,&y)) {button=self->_bagActions[i];stage=5;break;}
        } else if (stage==5 && partyReady && party->phase==4 && HGNativePartyUIResolveTouch(party,party->revision,4,HG_PARTY_UI_TOUCH_MON,0,&x,&y)) {capture=@"target.png";button=self->_partyMons[0];stage=6;}
        else if (stage>=6 && stage<=8 && partyReady && party->phase==5 && party->actions[0].textId==(stage==6 ? HG_PARTY_UI_TEXT_CANDY_ACK : stage==7 ? HG_PARTY_UI_TEXT_CANDY_STATS_DIFF : HG_PARTY_UI_TEXT_CANDY_STATS_TOTAL)
            && HGNativePartyUIResolveTouch(party,party->revision,5,HG_PARTY_UI_TOUCH_ACTION,0,&x,&y)) {
            checks[@"originalLevel16"]=@([facts[@"level"] unsignedIntValue]==16 && [facts[@"experience"] unsignedIntValue]==2535 && [facts[@"candyQuantity"] unsignedIntValue]==1);
            if (![checks[@"originalLevel16"] boolValue]) {finish(@"level16_mismatch");return;}
            if (stage>6) {BOOL stats=!self->_partyBody.hidden && self->_partyBody.image.size.width==128 && self->_partyBody.image.size.height==112 && [self checkPrivatePixels:self->_partyBody.image];checks[stage==7 ? @"statsDifference" : @"statsTotal"]=@(stats);if (!stats) {finish(@"stats_image_missing");return;}}
            capture=stage==6 ? @"level-message.png" : stage==7 ? @"stats-difference.png" : @"stats-total.png";button=self->_partyActions[0];stage++;
        } else if (stage==9 && ev && evo->phase==8) {
            CGImageRef image=self->_canvas.image.CGImage;const uint32_t *g=geometry.bytes;
            BOOL scene=self->_evolutionScene && !self->_canvas.hidden && image && CGImageGetWidth(image)==256 && CGImageGetHeight(image)==192 && ownerWidth==256 && self->_displayedWidth==256
                && geometry.length==32 && g[0] && g[1] && [self checkPrivatePixels:self->_canvas.image];
            checks[@"canonicalWidth"]=@(scene);checks[@"typedCancelAvailable"]=@(HGNativeEvolutionUIResolveInput(evo,evo->revision,8,HG_EVOLUTION_UI_INPUT_CANCEL,0,&k,&x,&y) && k==2);
            if (!scene || ![checks[@"typedCancelAvailable"] boolValue] || self->_buttons[@2].hidden || !self->_buttons[@2].enabled) {finish(@"evolution_scene_or_cancel_missing");return;}
            firstPixels=CFBridgingRelease(CGDataProviderCopyData(CGImageGetDataProvider(image)));oscillateFrame=sPrivateDisplayFrame;capture=@"oscillate.png";stage=10;
        } else if (stage==10 && ev && evo->phase==8 && sPrivateDisplayFrame>oscillateFrame) {
            CGImageRef current=self->_canvas.image.CGImage;if (!current) {finish(@"evolution_image_missing");return;}NSData *pixels=CFBridgingRelease(CGDataProviderCopyData(CGImageGetDataProvider(current)));
            if (pixels.length==firstPixels.length && ![pixels isEqual:firstPixels]) {
                checks[@"originalEvolutionScene"]=@YES;checks[@"staleRejected"]=@(!HGNativeEvolutionUIResolveInput(evo,evo->revision-1,8,HG_EVOLUTION_UI_INPUT_CANCEL,0,&k,&x,&y));
                capture=@"oscillate-later.png";if (cancel) {button=self->_buttons[@2];evoButton=YES;}stage=11;
            }
        } else if (stage==11 && ev && evo->phase==(cancel ? 44u : 12u)) {
            [self.view layoutIfNeeded];UIGraphicsBeginImageContextWithOptions(self.view.bounds.size,NO,0);BOOL drawn=[self.view drawViewHierarchyInRect:self.view.bounds afterScreenUpdates:YES];
            UIImage *image=UIGraphicsGetImageFromCurrentImageContext();UIGraphicsEndImageContext();if (drawn && image) {resultImage=image;resultFrame=sPrivateDisplayFrame;}
        } else if (stage==11 && resultImage && (!ev || evo->phase!=(cancel ? 44u : 12u))) {capture=@"result.png";captureImage=resultImage;stage=12;}
        else if (stage==12 && bagReady && HGNativeBagUIIsList(bag) && (!evo || !evo->active)) {
            BOOL outcome=[facts[@"species"] unsignedIntValue]==(cancel ? 152u : 153u) && [facts[@"level"] unsignedIntValue]==16 && [facts[@"experience"] unsignedIntValue]==2535
                && [facts[@"bagOnlyOneCandyConsumed"] boolValue] && [facts[@"originalIdentityPreserved"] boolValue] && [facts[@"movesAndPPPreserved"] boolValue] && [facts[@"flagsMoneyPreserved"] boolValue]
                && [facts[@"bayleefSeen"] boolValue]==!cancel && [facts[@"bayleefCaught"] boolValue]==!cancel && [facts[@"dexSeenDelta"] unsignedIntValue]==(cancel ? 0u : 1u) && [facts[@"dexCaughtDelta"] unsignedIntValue]==(cancel ? 0u : 1u);
            BOOL item=NO;for (NSUInteger i=0;i<MIN(bag->visibleCount,HG_BAG_UI_ITEMS);i++) if (bag->items[i].itemId==50 && bag->items[i].quantity==1 && [self checkBagItem:i state:bag]) item=YES;
            checks[@"originalOutcome"]=@(outcome);checks[@"returnedBag"]=@(item);if (!outcome || !item) {finish(@"evolution_outcome_mismatch");return;}capture=@"final-bag.png";stage=13;
        } else if (stage==13 && bagReady && HGNativeBagUIIsList(bag) && HGNativeBagUIResolveTouch(bag,bag->revision,bag->phase,HG_BAG_UI_TOUCH_CANCEL,0,&x,&y)) {button=self->_bagNavigation[0];stage=14;}
        else if (stage==14 && q->fieldReady && !q->owner && map==MAP_ROUTE_29 && (!bag || !bag->active) && (!party || !party->active) && (!evo || !evo->active)) {if (menuOpen) [self toggleMenu];fieldFrame=sPrivateDisplayFrame;stage=15;}
        else if (stage==15 && q->fieldReady && !q->owner && map==MAP_ROUTE_29 && !menuOpen && sPrivateDisplayFrame>fieldFrame && !self->_canvas.hidden && self->_canvas.userInteractionEnabled
            && self->_partyPanel.hidden && self->_bagPanel.hidden && self->_summaryPanel.hidden && self->_fieldMenu.hidden && self->_fieldMenuShade.hidden && !self->_joystick.hidden && self->_joystick.userInteractionEnabled
            && !self->_menuButton.hidden && [self->_menuButton.currentTitle isEqual:@"菜单"] && !self->_buttons[@1].hidden && !self->_buttons[@2].hidden
            && self->_buttons[@1].enabled && self->_buttons[@2].enabled && self->_buttons[@1].alpha==1 && self->_buttons[@2].alpha==1
            && [self->_buttons[@1].currentTitle isEqual:@"互动"] && [self->_buttons[@2].currentTitle isEqual:@"奔跑"]) {
            const uint32_t *g=geometry.bytes;CGImageRef image=self->_canvas.image.CGImage;
            if (geometry.length==32 && g[0] && g[1] && image && CGImageGetWidth(image)>256 && [self checkPrivatePixels:self->_canvas.image]) {
                checks[@"inputLeaseReleased"]=@(idle && ![facts[@"rawPresses"] unsignedIntValue] && ![facts[@"rawReleases"] unsignedIntValue] && ![facts[@"rawRejected"] unsignedIntValue]);checks[@"rawInputZero"]=@(rawZero && busySamples>0);checks[@"returnedField"]=@YES;capture=@"field-return.png";stage=16;
            }
        }
        if (stage==16 && !capture) {
            UIButton *run=self->_buttons[@2];CGPoint point=[run convertPoint:CGPointMake(CGRectGetMidX(run.bounds),CGRectGetMidY(run.bounds)) toView:self.view];UIView *hit=[self.view hitTest:point withEvent:nil];
            if (run.hidden || !run.enabled || run.alpha!=1 || (hit!=run && ![hit isDescendantOfView:run]) || ![run accessibilityActivate]) {finish(@"field_run_unreachable");return;}
            [self->_condition lock];BOOL queued=(self->_keys&2) || (self->_pendingKeys&2);releaseReads=self->_summaryTMPadReads;[self->_condition unlock];
            [events addObject:@{@"event":@"regular-field-run-routing-diagnostic",@"queuedBit2":@(queued),@"padReads":@(releaseReads)}];
            if (!queued) {finish(@"field_run_did_not_use_raw_handler");return;}stage=17;
        } else if (stage==17 && idle && reads-releaseReads>=2) {checks[@"fieldRunButtonRouting"]=@YES;stage=99;}
        if (!button && ev && evo->actionMask==HG_EVOLUTION_UI_ACTION_ACK && HGNativeEvolutionUIResolveInput(evo,evo->revision,evo->phase,HG_EVOLUTION_UI_INPUT_ACK,0,&k,&x,&y)) {button=self->_buttons[@1];evoButton=YES;}
        if (button) {CGPoint point=[button convertPoint:CGPointMake(CGRectGetMidX(button.bounds),CGRectGetMidY(button.bounds)) toView:self.view];UIView *hit=[self.view hitTest:point withEvent:nil];
            if (button.hidden || !button.enabled || !button.window || (hit!=button && ![hit isDescendantOfView:button])) {finish(@"typed_button_unreachable");return;}}
        if (capture && ![captured containsObject:capture]) {if (![self checkPrivateCapture:[prefix stringByAppendingString:capture] kind:@"UIKit-Evolution" image:captureImage]) {finish(@"capture_failed");return;}[captured addObject:capture];}
        if (button) {if (evoButton) {if (![button accessibilityActivate]) {finish(@"evolution_accessibility_activation_failed");return;}evolutionActions++;if (button.tag==1) ackActions++;else cancelActions++;}
            else [button sendActionsForControlEvents:UIControlEventTouchUpInside];release=YES;releaseStarted=NO;}
        if (stage==99) {checks[@"nativeEvolutionButtons"]=@(ackActions==1 && cancelActions==(cancel ? 1u : 0u));finish(@"completed");return;}
        dispatch_after(dispatch_time(DISPATCH_TIME_NOW,NSEC_PER_SEC/10),dispatch_get_main_queue(),step);
    };step();
}
@end
#endif
