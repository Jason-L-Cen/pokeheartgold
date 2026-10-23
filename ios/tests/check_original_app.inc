// SPDX-License-Identifier: GPL-3.0-or-later
// Included only in the explicit UIKit self-check build; all input uses the real control handlers.
#include "constants/maps.h"
@implementation OriginalController (Checks)
- (BOOL)checkGlyph:(UIImage *)glyph inCard:(UIImage *)card x:(NSInteger)x y:(NSInteger)y {
    CGImageRef g=glyph.CGImage,c=card.CGImage;
    if (!g || !c || x<0 || y<0 || CGImageGetBitsPerPixel(g)!=32 || CGImageGetBitsPerPixel(c)!=32
        || CGImageGetBitsPerComponent(g)!=8 || CGImageGetBitsPerComponent(c)!=8) return NO;
    size_t gw=CGImageGetWidth(g),gh=CGImageGetHeight(g),cw=CGImageGetWidth(c),ch=CGImageGetHeight(c);
    if (!gw || !gh || !cw || !ch || cw>256 || ch>192 || gw>cw || gh>ch || (NSUInteger)x>cw-gw || (NSUInteger)y>ch-gh) return NO;
    size_t gs=CGImageGetBytesPerRow(g),cs=CGImageGetBytesPerRow(c);
    NSData *gp=CFBridgingRelease(CGDataProviderCopyData(CGImageGetDataProvider(g)));
    NSData *cp=CFBridgingRelease(CGDataProviderCopyData(CGImageGetDataProvider(c)));
    if (gs<gw*4 || cs<cw*4 || gs>gp.length/gh || cs>cp.length/ch) return NO;
    const uint8_t *gb=gp.bytes,*cb=cp.bytes; NSUInteger opaque=0;
    for (size_t row=0;row<gh;row++) for (size_t col=0;col<gw;col++) {
        const uint8_t *a=gb+row*gs+col*4,*b=cb+((NSUInteger)y+row)*cs+((NSUInteger)x+col)*4;
        if (!a[3]) continue;
        if (a[3]!=255 || b[3]!=255 || memcmp(a,b,3)) return NO;
        opaque++;
    }
    return opaque>0;
}
- (BOOL)checkReturnTitle:(UIButton *)button inScreenshot:(UIImage *)screenshot {
    UILabel *title=button.titleLabel;
    if (!screenshot.CGImage || screenshot.imageOrientation!=UIImageOrientationUp || title.hidden || title.alpha!=1 || !title.font) return NO;
    CGFloat scale=screenshot.scale;
    if (!isfinite(scale) || scale<=0) return NO;
    CGRect frame=[title convertRect:title.bounds toView:self.view];
    CGRect pixels=CGRectIntegral(CGRectMake(frame.origin.x*scale,frame.origin.y*scale,frame.size.width*scale,frame.size.height*scale));
    if (!isfinite(pixels.origin.x) || !isfinite(pixels.origin.y) || !isfinite(pixels.size.width) || !isfinite(pixels.size.height)
        || pixels.origin.x<0 || pixels.origin.y<0 || pixels.size.width<=0 || pixels.size.height<=0 || pixels.size.width>256 || pixels.size.height>192
        || CGRectGetMaxX(pixels)>CGImageGetWidth(screenshot.CGImage) || CGRectGetMaxY(pixels)>CGImageGetHeight(screenshot.CGImage)) return NO;
    UILabel *literal=[[UILabel alloc] initWithFrame:CGRectMake(frame.origin.x-pixels.origin.x/scale,frame.origin.y-pixels.origin.y/scale,frame.size.width,frame.size.height)];
    literal.text=@"返回"; literal.font=title.font; literal.textAlignment=title.textAlignment; literal.textColor=UIColor.whiteColor;
    UIGraphicsBeginImageContextWithOptions(CGSizeMake(pixels.size.width/scale,pixels.size.height/scale),NO,scale);
    CGContextTranslateCTM(UIGraphicsGetCurrentContext(),literal.frame.origin.x,literal.frame.origin.y);
    [literal drawTextInRect:literal.bounds];
    UIImage *reference=UIGraphicsGetImageFromCurrentImageContext(); UIGraphicsEndImageContext();
    CGImageRef crop=CGImageCreateWithImageInRect(screenshot.CGImage,pixels);
    if (!crop || !reference.CGImage) { if (crop) CGImageRelease(crop); return NO; }
    NSMutableData *(^rgba)(CGImageRef)=^NSMutableData *(CGImageRef image) {
        size_t width=CGImageGetWidth(image),height=CGImageGetHeight(image);
        NSMutableData *data=[NSMutableData dataWithLength:width*height*4];
        CGColorSpaceRef colors=CGColorSpaceCreateDeviceRGB();
        CGContextRef context=CGBitmapContextCreate(data.mutableBytes,width,height,8,width*4,colors,kCGBitmapByteOrder32Big|kCGImageAlphaPremultipliedLast);
        CGColorSpaceRelease(colors); if (!context) return nil;
        CGContextDrawImage(context,CGRectMake(0,0,width,height),image); CGContextRelease(context); return data;
    };
    NSMutableData *mask=rgba(reference.CGImage),*actual=rgba(crop); CGImageRelease(crop);
    if (!mask || !actual) return NO;
    uint8_t *bytes=mask.mutableBytes;
    for (NSUInteger i=3;i<mask.length;i+=4) if (bytes[i]!=255) bytes[i]=0;
    return [self checkGlyph:OriginalPixelImage(mask,(uint32_t)pixels.size.width,(uint32_t)pixels.size.height)
        inCard:OriginalPixelImage(actual,(uint32_t)pixels.size.width,(uint32_t)pixels.size.height) x:0 y:0];
}
- (BOOL)checkSummaryRasterBack {
    UIButton *back=_summaryNavigation[0];
    return back.configuration.image && !back.configuration.title.length && !back.configuration.attributedTitle.length
        && !back.currentTitle.length && !back.currentAttributedTitle.length;
}
- (BOOL)checkBagItem:(NSUInteger)index state:(const HGNativeBagUIState *)state {
    if (!state || !state->active || !state->ready || !HGNativeBagUIIsList(state) || index>=HG_BAG_UI_ITEMS
        || index>=state->visibleCount || !(state->itemMask&(1u<<index)) || index>=_displayedBagItemGlyphs.count) return NO;
    id glyph=_displayedBagItemGlyphs[index]; const HGNativeBagUIItem *item=&state->items[index];
    return [glyph isKindOfClass:UIImage.class] && ((UIImage *)glyph).size.width==88 && ((UIImage *)glyph).size.height==32
        && [self checkGlyph:glyph inCard:_bagItemImages[index].image
        x:32+128*(NSInteger)(index%2)-(NSInteger)item->cropX y:40+40*(NSInteger)(index/2)-(NSInteger)item->cropY];
}
- (void)checkBattleMenusFlow:(NSMutableDictionary *)report completion:(dispatch_block_t)completion {
    NSArray *keys=@[@"actualBattleMenusRequested",@"actualBattleOriginalWildPrerequisite",@"actualBattleBagSingleScreen",@"actualBattleBagCategories",
        @"actualBattleBagPotionList",@"actualBattleBagUseDetails",@"actualBattleBagBackToList",@"actualBattleBagAllPockets",@"actualBattleBagReturnsNewCommands",
        @"actualBattleBagReopenRejectsStale",@"actualBattleItemTargetCancel",@"actualBattlePartyReopenRejectsStale",@"actualBattlePotionNoEffect",
        @"actualBattlePartySingleScreen",@"actualBattleShiftUsesOriginalRejection",@"actualBattlePartyCancelReturnsCommands",@"actualBattleGrowlUsesOriginalTurns",
        @"actualBattleNaturalDamage",@"actualBattlePotionTargetsInjuredMon",@"actualBattlePotionHealsAndConsumes",@"actualBattleRunSettlesOriginalField",
        @"actualBattleMenusGuestLifecycle",@"actualBattleMenusSemantics",@"actualBattlePartySummaryOriginalBody",@"actualBattlePartyMovesOriginalCards",
        @"actualBattlePartyDetailsOriginalBody",@"actualBattlePartyInfoRejectsStale",@"actualBattlePartyInfoReturnsMonMenu",@"actualBattlePartyInfoPreservesBattle",@"actualBattlePartyInfoSingleScreen"];
    for (NSString *key in keys) report[key]=@NO;
    __block NSUInteger stage=0,polls=0,lastStage=NSNotFound,nextPocket=1,growls=0,runs=0,fleeFailures=0;
    __block uint32_t bagRevision=0,partyRevision=0,commandRevision=0,visited=0,turnBefore=0,initialPP=0,preTargetHp=0,replyBefore=0,runRevision=0,requestRevision=0;
    __block NSUInteger infoStage=0,lastInfoStage=NSNotFound; __block uint32_t detailRevision=0;
    __block NSData *infoBefore=nil,*infoAfter=nil; __block BOOL infoNative=YES,infoDetails=YES;
    __block NSString *note=@"running";
    NSMutableArray *events=[NSMutableArray new];
    NSURL *folder=[NSFileManager.defaultManager URLsForDirectory:NSDocumentDirectory inDomains:NSUserDomainMask].firstObject;
    __block void (^step)(void);
    void (^finish)(NSString *)=^(NSString *status) {
        [self->_condition lock]; NSData *bytes=self->_battleMenusQAState; NSString *failure=self->_failure; BOOL menuOpen=self->_fieldMenuOpen; [self->_condition unlock];
        NSMutableArray *words=[NSMutableArray new]; const uint32_t *p=bytes.bytes;
        if (bytes.length==sizeof(HGNativeBattleMenusQAState)) for (NSUInteger i=0;i<bytes.length/sizeof(*p);i++) [words addObject:@(p[i])];
        NSMutableArray *beforeWords=[NSMutableArray new],*afterWords=[NSMutableArray new];
        if (infoBefore.length==sizeof(HGNativeBattleMenusQAState)) for (NSUInteger i=0;i<infoBefore.length/sizeof(uint32_t);i++) [beforeWords addObject:@(((const uint32_t *)infoBefore.bytes)[i])];
        if (infoAfter.length==sizeof(HGNativeBattleMenusQAState)) for (NSUInteger i=0;i<infoAfter.length/sizeof(uint32_t);i++) [afterWords addObject:@(((const uint32_t *)infoAfter.bytes)[i])];
        NSDictionary *evidence=@{@"version":@1,@"status":status,@"note":note,@"stage":@(stage),@"polls":@(polls),@"growlCount":@(growls),
            @"runAttempts":@(runs),@"fleeFailures":@(fleeFailures),@"preTargetHp":@(preTargetHp),@"fieldMenuOpen":@(menuOpen),@"fieldMenuHidden":@(self->_fieldMenu.hidden),
            @"infoStage":@(infoStage),@"infoBeforeWords":beforeWords,@"infoAfterWords":afterWords,@"events":events,@"qaWords":words,@"engineFailure":failure ?: @""};
        [[NSJSONSerialization dataWithJSONObject:evidence options:NSJSONWritingPrettyPrinted error:nil] writeToURL:[folder URLByAppendingPathComponent:@"original-battle-menus-qa.json"] options:NSDataWritingAtomic error:nil];
        step=nil; completion();
    };
    step=^{
        [self->_condition lock]; NSData *qaData=self->_battleMenusQAState; uint32_t map=self->_fieldMapId,frame=self->_publishedFrames;
        BOOL failed=self->_failure!=nil || self->_finished,accepted=self->_checkBattleMenusRequestAccepted,menuOpen=self->_fieldMenuOpen; [self->_condition unlock];
        const HGNativeBattleMenusQAState *q=qaData.length==sizeof(*q) ? qaData.bytes : NULL;
        const HGNativeBattleUIState *battle=self->_displayedBattleUIState.length==sizeof(*battle) ? self->_displayedBattleUIState.bytes : NULL;
        const HGNativeBattleBagUIState *bag=self->_displayedBattleBagUIState.length==sizeof(*bag) ? self->_displayedBattleBagUIState.bytes : NULL;
        const HGNativeBattlePartyUIState *party=self->_displayedBattlePartyUIState.length==sizeof(*party) ? self->_displayedBattlePartyUIState.bytes : NULL;
        BOOL command=battle && battle->active && battle->ready && battle->mode==HG_BATTLE_UI_COMMAND;
        BOOL moves=battle && battle->active && battle->ready && battle->mode==HG_BATTLE_UI_MOVES;
        BOOL bagReady=bag && bag->active && bag->ready && self->_battleMenuKind==OriginalBattleMenuBag && !self->_battleMenuPanel.hidden;
        BOOL partyReady=party && party->active && party->ready && self->_battleMenuKind==OriginalBattleMenuParty && !self->_battleMenuPanel.hidden;
        if (failed || ++polls>3600 || (q && q->errorLine)) { finish(failed ? @"engine_failed" : q && q->errorLine ? @"guest_failed" : @"flow_timeout"); return; }
        if (q && q->returned && q->outcome!=5) { finish(@"original_battle_ended_otherwise"); return; }
        if (stage==22 && infoBefore && !infoAfter) {
            if (!q || infoBefore.length!=sizeof(*q)) { finish(@"info_observation_unavailable"); return; }
            HGNativeBattleMenusQAState before=*(const HGNativeBattleMenusQAState *)infoBefore.bytes,after=*q;
            before.revision=after.revision=before.fieldFrame=after.fieldFrame=0;
            if (memcmp(&before,&after,sizeof(before))) { finish(@"info_changed_battle_state"); return; }
        }
        if (lastStage!=stage || lastInfoStage!=infoStage) {
            [events addObject:@{@"stage":@(stage),@"infoStage":@(infoStage),@"publishedFrame":@(frame),@"turns":@(q ? q->totalTurns : 0),@"commandRevision":@(battle ? battle->revision : 0),@"bagRevision":@(bag ? bag->revision : 0),@"partyRevision":@(party ? party->revision : 0),
                @"partyPhase":@(party ? party->phase : 0),@"selectedMove":@(party ? party->selectedMove : 0),@"fieldReady":@(q ? q->fieldReady : 0),@"fieldMenuOpen":@(menuOpen),@"fieldMenuHidden":@(self->_fieldMenu.hidden)}]; lastStage=stage; lastInfoStage=infoStage;
        }
        NSString *capture=nil; dispatch_block_t action=nil;
        if (!stage && map==MAP_ROUTE_29 && q && q->fieldReady) {
            if (menuOpen) [self toggleMenu]; stage=1;
        } else if (stage==1 && q && q->fieldReady && !q->requested && !menuOpen && self->_fieldMenu.hidden) {
            [self->_condition lock]; self->_checkBattleMenusRequestRevision=q->revision; self->_checkBattleMenusRequestPending=YES; [self->_condition unlock]; requestRevision=q->revision; stage=2;
        } else if (stage==2 && q && !q->requested && q->fieldReady && q->revision!=requestRevision) stage=1;
        else if (stage==2 && q && q->started && q->battleActive && command) {
            report[@"actualBattleMenusRequested"]=@(accepted);
            // QA playerStatus is the original MON status bitset; roster status is an icon ID.
            BOOL valid=q->partyCount==1 && q->playerHp==20 && q->playerMaxHp==20 && !q->playerStatus && q->activePartySlot==0
                && q->enemySpecies==10 && q->enemyLevel==1 && q->moves[1]==45 && q->pp[1]>=6 && q->potionQuantity==5;
            report[@"actualBattleOriginalWildPrerequisite"]=@(valid);
            if (!valid) { finish(@"prerequisite_failed"); return; }
            initialPP=q->pp[1]; commandRevision=battle->revision; action=^{ [self->_battleButtons[1] sendActionsForControlEvents:UIControlEventTouchUpInside]; }; stage=3;
        } else if (stage==3 && bagReady && bag->phase==HG_BATTLE_BAG_UI_POCKETS && self->_battleMenuButtons[0].enabled) {
            report[@"actualBattleBagSingleScreen"]=@(self->_canvas.hidden && self->_battleBar.hidden && self->_battleMenuPreview.image!=nil && self->_buttons[@1].hidden);
            BOOL categories=bag->buttonCount==6;
            for (NSUInteger i=0;i<4;i++) if (!self->_battleMenuButtons[i].enabled || !self->_battleMenuImages[i].image) categories=NO;
            report[@"actualBattleBagCategories"]=@(categories); bagRevision=bag->revision; capture=@"original-battle-bag-pockets-check.png";
            action=^{ [self->_battleMenuButtons[0] sendActionsForControlEvents:UIControlEventTouchUpInside]; }; stage=4;
        } else if ((stage==4 || stage==11 || stage==15 || stage==30) && bagReady && bag->phase==HG_BATTLE_BAG_UI_ITEMS && bag->pocket==0) {
            NSUInteger slot=NSNotFound; for (NSUInteger i=0;i<6;i++) if (bag->itemId[i]==17 && bag->quantity[i]==5 && self->_battleMenuButtons[i].enabled) { slot=i; break; }
            if (slot!=NSNotFound) {
                if (stage==4) { visited|=1; report[@"actualBattleBagPotionList"]=@(self->_battleMenuImages[slot].image.size.width==bag->buttons[slot].cropWidth); capture=@"original-battle-bag-items-check.png"; }
                action=^{ [self->_battleMenuButtons[slot] sendActionsForControlEvents:UIControlEventTouchUpInside]; }; stage=stage==4 ? 5 : stage==11 ? 12 : stage==15 ? 16 : 31;
            }
        } else if (stage==5 && bagReady && bag->phase==HG_BATTLE_BAG_UI_USE && self->_battleMenuButtons[1].enabled) {
            report[@"actualBattleBagUseDetails"]=@(bag->selectedItemId==17 && bag->selectedQuantity==5 && self->_battleMenuSelected.image && self->_battleMenuQuantity.image && self->_battleMenuDescription.image);
            capture=@"original-battle-bag-use-check.png"; action=^{ [self->_battleMenuButtons[1] sendActionsForControlEvents:UIControlEventTouchUpInside]; }; stage=6;
        } else if (stage==6 && bagReady && bag->phase==HG_BATTLE_BAG_UI_ITEMS && self->_battleMenuButtons[6].enabled) {
            report[@"actualBattleBagBackToList"]=@YES; action=^{ [self->_battleMenuButtons[6] sendActionsForControlEvents:UIControlEventTouchUpInside]; }; stage=7;
        } else if (stage==7 && bagReady && bag->phase==HG_BATTLE_BAG_UI_POCKETS) {
            if (nextPocket<4 && self->_battleMenuButtons[nextPocket].enabled) { action=^{ [self->_battleMenuButtons[nextPocket] sendActionsForControlEvents:UIControlEventTouchUpInside]; }; stage=8; }
            else if (nextPocket==4 && self->_battleMenuButtons[5].enabled) { report[@"actualBattleBagAllPockets"]=@(visited==15); action=^{ [self->_battleMenuButtons[5] sendActionsForControlEvents:UIControlEventTouchUpInside]; }; stage=9; }
        } else if (stage==8 && bagReady && bag->phase==HG_BATTLE_BAG_UI_ITEMS && bag->pocket==nextPocket && self->_battleMenuButtons[6].enabled) {
            visited|=1u<<nextPocket; nextPocket++; action=^{ [self->_battleMenuButtons[6] sendActionsForControlEvents:UIControlEventTouchUpInside]; }; stage=7;
        } else if (stage==9 && command && self->_battleButtons[1].enabled) {
            report[@"actualBattleBagReturnsNewCommands"]=@(battle->revision!=commandRevision); action=^{ [self->_battleButtons[1] sendActionsForControlEvents:UIControlEventTouchUpInside]; }; stage=10;
        } else if (stage==10 && bagReady && bag->phase==HG_BATTLE_BAG_UI_POCKETS && self->_battleMenuButtons[0].enabled) {
            uint32_t x,y; report[@"actualBattleBagReopenRejectsStale"]=@(bag->revision!=bagRevision && !HGNativeBattleBagUIResolveTouch(bag,bagRevision,HG_BATTLE_BAG_UI_POCKETS,0,&x,&y));
            action=^{ [self->_battleMenuButtons[0] sendActionsForControlEvents:UIControlEventTouchUpInside]; }; stage=11;
        } else if ((stage==12 || stage==16 || stage==31) && bagReady && bag->phase==HG_BATTLE_BAG_UI_USE && self->_battleMenuButtons[0].enabled) {
            action=^{ [self->_battleMenuButtons[0] sendActionsForControlEvents:UIControlEventTouchUpInside]; }; stage=stage==12 ? 13 : stage==16 ? 17 : 32;
        } else if (stage==13 && partyReady && party->phase==HG_BATTLE_PARTY_UI_ROSTER && party->mode==HG_BATTLE_PARTY_UI_USE_ITEM && self->_battleMenuButtons[6].enabled) {
            partyRevision=party->revision; capture=@"original-battle-item-target-check.png"; action=^{ [self->_battleMenuButtons[6] sendActionsForControlEvents:UIControlEventTouchUpInside]; }; stage=14;
        } else if (stage==14 && bagReady && bag->phase==HG_BATTLE_BAG_UI_POCKETS && self->_battleMenuButtons[0].enabled) {
            report[@"actualBattleItemTargetCancel"]=@(q && q->potionQuantity==5); action=^{ [self->_battleMenuButtons[0] sendActionsForControlEvents:UIControlEventTouchUpInside]; }; stage=15;
        } else if (stage==17 && partyReady && party->phase==HG_BATTLE_PARTY_UI_ROSTER && self->_battleMenuButtons[0].enabled) {
            uint32_t x,y; report[@"actualBattlePartyReopenRejectsStale"]=@(party->revision!=partyRevision && !HGNativeBattlePartyUIResolveTouch(party,partyRevision,HG_BATTLE_PARTY_UI_ROSTER,0,&x,&y));
            action=^{ [self->_battleMenuButtons[0] sendActionsForControlEvents:UIControlEventTouchUpInside]; }; stage=18;
        } else if (stage==18 && partyReady && party->phase==HG_BATTLE_PARTY_UI_ACK && self->_battleMenuButtons[0].enabled) {
            capture=@"original-battle-party-noeffect-check.png"; action=^{ [self->_battleMenuButtons[0] sendActionsForControlEvents:UIControlEventTouchUpInside]; }; stage=19;
        } else if (stage==19 && bagReady && bag->phase==HG_BATTLE_BAG_UI_POCKETS && self->_battleMenuButtons[5].enabled) {
            report[@"actualBattlePotionNoEffect"]=@(q && q->playerHp==20 && q->potionQuantity==5 && !q->totalTurns); action=^{ [self->_battleMenuButtons[5] sendActionsForControlEvents:UIControlEventTouchUpInside]; }; stage=20;
        } else if (stage==20 && command && self->_battleButtons[2].enabled) {
            action=^{ [self->_battleButtons[2] sendActionsForControlEvents:UIControlEventTouchUpInside]; }; stage=21;
        } else if (stage==21 && partyReady && party->phase==HG_BATTLE_PARTY_UI_ROSTER && party->mode==HG_BATTLE_PARTY_UI_SWITCH && self->_battleMenuButtons[0].enabled) {
            report[@"actualBattlePartySingleScreen"]=@(self->_canvas.hidden && self->_battleMenuImages[0].image.size.width==128 && self->_battleMenuImages[0].image.size.height==48);
            capture=@"original-battle-party-roster-check.png"; action=^{ [self->_battleMenuButtons[0] sendActionsForControlEvents:UIControlEventTouchUpInside]; }; stage=22;
        } else if (stage==22 && partyReady) {
            BOOL native=self->_canvas.hidden && self->_battleBar.hidden && !self->_battleMenuPanel.hidden && self->_buttons[@1].hidden && self->_buttons[@2].hidden;
            if (!infoStage && party->phase==HG_BATTLE_PARTY_UI_MON_MENU && self->_battleMenuButtons[1].enabled) {
                if (!q || !q->moves[0] || !q->moves[1] || q->moves[2] || q->moves[3]) { finish(@"info_fixture_unavailable"); return; }
                infoBefore=[qaData copy]; capture=@"original-battle-party-menu-check.png";
                action=^{ [self->_battleMenuButtons[1] sendActionsForControlEvents:UIControlEventTouchUpInside]; }; infoStage=1;
            } else if (infoStage==1 && party->phase==HG_BATTLE_PARTY_UI_SUMMARY && self->_battleMenuButtons[2].enabled) {
                infoNative&=native && self->_battleMenuPreview.hidden && !self->_battleMenuPreview.image;
                report[@"actualBattlePartySummaryOriginalBody"]=@(self->_battleMenuDescription.image.size.width==256 && self->_battleMenuDescription.image.size.height==152
                    && self->_battleMenuSelected.hidden && self->_battleMenuQuantity.hidden && !self->_battleMenuButtons[0].enabled && !self->_battleMenuButtons[1].enabled);
                capture=@"original-battle-party-summary-check.png"; action=^{ [self->_battleMenuButtons[2] sendActionsForControlEvents:UIControlEventTouchUpInside]; }; infoStage=2;
            } else if (infoStage==2 && party->phase==HG_BATTLE_PARTY_UI_CHECK_MOVES && self->_battleMenuButtons[0].enabled) {
                infoNative&=native && self->_battleMenuPreview.hidden && !self->_battleMenuPreview.image;
                BOOL cards=self->_battleMenuSelected.image.size.width==72 && self->_battleMenuSelected.image.size.height==16 && self->_battleMenuDescription.hidden;
                for (NSUInteger i=0;i<4;i++) {
                    cards&=party->moves[i].moveId==q->moves[i] && party->moves[i].pp==q->pp[i]
                        && self->_battleMenuImages[i].image.size.width==party->buttons[i].cropWidth && self->_battleMenuImages[i].image.size.height==party->buttons[i].cropHeight;
                    if (i<2) cards&=self->_battleMenuButtons[i].enabled && party->moves[i].moveId<self->_moveNames.count
                        && [self->_battleMenuButtons[i].accessibilityLabel containsString:self->_moveNames[party->moves[i].moveId]]
                        && [self->_battleMenuButtons[i].accessibilityLabel containsString:[NSString stringWithFormat:@"PP %u／%u",party->moves[i].pp,party->moves[i].maxPP]];
                    else cards&=!self->_battleMenuButtons[i].enabled;
                }
                report[@"actualBattlePartyMovesOriginalCards"]=@(cards); capture=@"original-battle-party-moves-check.png";
                action=^{ [self->_battleMenuButtons[0] sendActionsForControlEvents:UIControlEventTouchUpInside]; }; infoStage=3;
            } else if ((infoStage==3 || infoStage==4 || infoStage==5) && party->phase==HG_BATTLE_PARTY_UI_MOVE_DETAIL
                && party->selectedMove==(infoStage==4 ? 1 : 0) && self->_battleMenuButtons[4].enabled && self->_battleMenuButtons[infoStage==3 ? 1 : 0].enabled) {
                infoNative&=native && self->_battleMenuPreview.hidden && !self->_battleMenuPreview.image;
                infoDetails&=self->_battleMenuDescription.image.size.width==256 && self->_battleMenuDescription.image.size.height==152
                    && self->_battleMenuSelected.hidden && self->_battleMenuQuantity.hidden && party->moves[party->selectedMove].moveId==q->moves[party->selectedMove]
                    && party->moves[party->selectedMove].pp==q->pp[party->selectedMove];
                for (NSUInteger i=0;i<4;i++) infoDetails&=party->moves[i].moveId ? party->moves[i].moveId<self->_moveNames.count && [self->_battleMenuButtons[i].currentTitle isEqualToString:self->_moveNames[party->moves[i].moveId]] : self->_battleMenuButtons[i].currentTitle.length==0;
                if (infoStage==3) { detailRevision=party->revision; capture=@"original-battle-party-move-detail-check.png"; action=^{ [self->_battleMenuButtons[1] sendActionsForControlEvents:UIControlEventTouchUpInside]; }; infoStage=4; }
                else if (infoStage==4) {
                    uint32_t x,y; report[@"actualBattlePartyInfoRejectsStale"]=@(party->revision!=detailRevision && !HGNativeBattlePartyUIResolveTouch(party,detailRevision,HG_BATTLE_PARTY_UI_MOVE_DETAIL,0,&x,&y));
                    capture=@"original-battle-party-move-detail-1-check.png"; action=^{ [self->_battleMenuButtons[0] sendActionsForControlEvents:UIControlEventTouchUpInside]; }; infoStage=5;
                } else { report[@"actualBattlePartyDetailsOriginalBody"]=@(infoDetails); action=^{ [self->_battleMenuButtons[4] sendActionsForControlEvents:UIControlEventTouchUpInside]; }; infoStage=6; }
            } else if (infoStage==6 && party->phase==HG_BATTLE_PARTY_UI_CHECK_MOVES && self->_battleMenuButtons[6].enabled) {
                infoNative&=native && self->_battleMenuPreview.hidden && !self->_battleMenuPreview.image;
                action=^{ [self->_battleMenuButtons[6] sendActionsForControlEvents:UIControlEventTouchUpInside]; }; infoStage=7;
            } else if (infoStage==7 && party->phase==HG_BATTLE_PARTY_UI_SUMMARY && self->_battleMenuButtons[3].enabled) {
                infoNative&=native && self->_battleMenuPreview.hidden && !self->_battleMenuPreview.image;
                action=^{ [self->_battleMenuButtons[3] sendActionsForControlEvents:UIControlEventTouchUpInside]; }; infoStage=8;
            } else if (infoStage==8 && party->phase==HG_BATTLE_PARTY_UI_MON_MENU && self->_battleMenuButtons[0].enabled) {
                BOOL titlesCleared=YES; for (UIButton *button in self->_battleMenuButtons) titlesCleared&=button.currentTitle.length==0;
                infoAfter=[qaData copy]; report[@"actualBattlePartyInfoReturnsMonMenu"]=@(!self->_battleMenuPreview.hidden && self->_battleMenuPreview.image && !self->_battleMenuDescription.isAccessibilityElement && titlesCleared);
                report[@"actualBattlePartyInfoPreservesBattle"]=@YES; report[@"actualBattlePartyInfoSingleScreen"]=@(infoNative);
                action=^{ [self->_battleMenuButtons[0] sendActionsForControlEvents:UIControlEventTouchUpInside]; }; stage=23;
            }
        } else if (stage==23 && partyReady && party->phase==HG_BATTLE_PARTY_UI_ACK && self->_battleMenuButtons[0].enabled) {
            report[@"actualBattleShiftUsesOriginalRejection"]=@(q && q->activePartySlot==0 && q->partyCount==1); action=^{ [self->_battleMenuButtons[0] sendActionsForControlEvents:UIControlEventTouchUpInside]; }; stage=24;
        } else if (stage==24 && partyReady && party->phase==HG_BATTLE_PARTY_UI_MON_MENU && self->_battleMenuButtons[3].enabled) {
            action=^{ [self->_battleMenuButtons[3] sendActionsForControlEvents:UIControlEventTouchUpInside]; }; stage=25;
        } else if (stage==25 && partyReady && party->phase==HG_BATTLE_PARTY_UI_ROSTER && self->_battleMenuButtons[6].enabled) {
            action=^{ [self->_battleMenuButtons[6] sendActionsForControlEvents:UIControlEventTouchUpInside]; }; stage=26;
        } else if (stage==26 && command && q && self->_battleButtons[0].enabled) {
            report[@"actualBattlePartyCancelReturnsCommands"]=@YES; turnBefore=q->totalTurns;
            action=^{ [self->_battleButtons[0] sendActionsForControlEvents:UIControlEventTouchUpInside]; }; stage=27;
        } else if (stage==27 && moves && battle->move[1]==45 && battle->pp[1] && self->_battleButtons[1].enabled) {
            action=^{ [self->_battleButtons[1] sendActionsForControlEvents:UIControlEventTouchUpInside]; }; stage=28;
        } else if (stage==28 && command && q && q->totalTurns>turnBefore) {
            growls++; report[@"actualBattleGrowlUsesOriginalTurns"]=@(q->pp[1]+growls==initialPP && (growls==1 || [report[@"actualBattleGrowlUsesOriginalTurns"] boolValue]));
            if (q->playerHp && q->playerHp<q->playerMaxHp && self->_battleButtons[1].enabled) {
                preTargetHp=q->playerHp; report[@"actualBattleNaturalDamage"]=@YES; action=^{ [self->_battleButtons[1] sendActionsForControlEvents:UIControlEventTouchUpInside]; }; stage=29;
            } else if (q->playerHp==q->playerMaxHp && growls<6) stage=26;
            else { note=@"prerequisite_damage_not_observed"; stage=34; }
        } else if (stage==29 && bagReady && bag->phase==HG_BATTLE_BAG_UI_POCKETS && self->_battleMenuButtons[0].enabled) {
            action=^{ [self->_battleMenuButtons[0] sendActionsForControlEvents:UIControlEventTouchUpInside]; }; stage=30;
        } else if (stage==32 && partyReady && party->phase==HG_BATTLE_PARTY_UI_ROSTER && self->_battleMenuButtons[0].enabled && q) {
            report[@"actualBattlePotionTargetsInjuredMon"]=@(party->mons[0].hp==preTargetHp && preTargetHp && preTargetHp<party->mons[0].maxHp);
            replyBefore=q->replyCount; turnBefore=q->totalTurns; capture=@"original-battle-healing-target-check.png";
            action=^{ [self->_battleMenuButtons[0] sendActionsForControlEvents:UIControlEventTouchUpInside]; }; stage=33;
        } else if (stage==33 && q) {
            if (q->replyCount>replyBefore) report[@"actualBattlePotionHealsAndConsumes"]=@(q->replyItem==17 && q->replyPocket==0 && q->replyTarget==1 && q->replyHp==20 && q->replyMaxHp==20 && q->replyPotionQuantity==4 && preTargetHp && preTargetHp<20);
            if (command && q->totalTurns>turnBefore && q->replyCount>replyBefore) stage=34;
        } else if (stage==34 && command && battle->revision!=runRevision && self->_battleButtons[3].enabled) {
            if (runs>=6) { finish(@"escape_not_observed"); return; }
            runRevision=battle->revision; runs++; action=^{ [self->_battleButtons[3] sendActionsForControlEvents:UIControlEventTouchUpInside]; }; stage=35;
        } else if (stage==35 && q && q->returned && q->fieldReady && map==MAP_ROUTE_29) {
            report[@"actualBattleRunSettlesOriginalField"]=@(q->outcome==5 && q->battleResult==5);
            report[@"actualBattleMenusGuestLifecycle"]=@(q->started && q->lifecycleCleared && !q->battleActive && (!bag || !bag->active) && (!party || !party->active));
            report[@"actualBattleMenusSemantics"]=@(q->semanticsMask==63);
            note=[note isEqual:@"running"] ? @"completed" : note; capture=@"original-battle-field-return-check.png"; stage=36;
        } else if (stage==35 && command && battle->revision!=runRevision) { fleeFailures++; stage=34; }
        if (capture) {
            UIGraphicsBeginImageContextWithOptions(self.view.bounds.size,NO,0); BOOL complete=[self.view drawViewHierarchyInRect:self.view.bounds afterScreenUpdates:YES];
            UIImage *image=UIGraphicsGetImageFromCurrentImageContext(); UIGraphicsEndImageContext();
            report[@"actualCapturesComplete"]=@([report[@"actualCapturesComplete"] boolValue] && complete && image!=nil);
            [UIImagePNGRepresentation(image) writeToURL:[folder URLByAppendingPathComponent:capture] options:NSDataWritingAtomic error:nil];
        }
        if (action) action();
        if (stage==36) { finish([note isEqual:@"completed"] ? @"completed" : note); return; }
        if (q && q->started && !q->returned && !(polls%3) && !self->_battleMenuScene && battle && battle->mode==HG_BATTLE_UI_NONE && !self->_buttons[@1].hidden) [self->_buttons[@1] accessibilityActivate];
        dispatch_after(dispatch_time(DISPATCH_TIME_NOW,NSEC_PER_SEC/10),dispatch_get_main_queue(),step);
    };
    step();
}
- (void)checkBattleMenuControls:(NSMutableDictionary *)report {
    HGNativeBattleBagUIState bag={.active=1,.revision=17,.phase=HG_BATTLE_BAG_UI_POCKETS,.ready=1,.buttonCount=6,.enabledMask=47};
    HGNativeBattleBagUIState emptyBag={0}; HGNativeBattlePartyUIState emptyParty={0};
    for (NSUInteger i=0;i<6;i++) bag.buttons[i]=(HGNativeBattleBagUIButton){(uint32_t)i*24+7,7,(uint32_t)i*24,0,16,16};
    NSData *zeroBag=[NSData dataWithBytes:&emptyBag length:sizeof(emptyBag)],*zeroParty=[NSData dataWithBytes:&emptyParty length:sizeof(emptyParty)];
    UIImage *tile=OriginalPixelImage([NSMutableData dataWithLength:16*16*4],16,16);
    NSMutableArray *images=[NSMutableArray new]; for (NSUInteger i=0;i<9;i++) [images addObject:tile];
    NSDictionary *pictures=@{@"buttons":images,@"preview":tile,@"message":tile};
    NSData *oldBattle=_battleUIState,*oldBattleBag=_battleBagUIState,*oldBattleParty=_battlePartyUIState; NSDictionary *oldPictures=_battleMenuPictures;
    NSDictionary *oldOwnerPictures=_battleMenuOwnerPictures;
    uint32_t oldOwnerKind=_battleMenuOwnerKind,oldOwnerRevision=_battleMenuOwnerRevision,oldOwnerPhase=_battleMenuOwnerPhase;
    _battleBagUIState=[NSData dataWithBytes:&bag length:sizeof(bag)]; _battlePartyUIState=zeroParty; _battleMenuPictures=pictures;
    [self displayFrame]; [self viewDidLayoutSubviews];
    report[@"battleMenuUsesPublishedImages"]=@(_battleMenuKind==OriginalBattleMenuBag && _battleMenuImages[0].image==tile && !_battleMenuPanel.hidden);
    report[@"battleMenuHidesCanvasAndFallbackButtons"]=@(_canvas.hidden && !_canvas.userInteractionEnabled && _battleBar.hidden && _buttons[@1].hidden && _buttons[@2].hidden);
    BOOL targets=YES;
    for (NSUInteger i=0;i<6;i++) if (_battleMenuButtons[i].bounds.size.width<44 || _battleMenuButtons[i].bounds.size.height<44 || !_battleMenuButtons[i].accessibilityLabel.length) targets=NO;
    report[@"battleMenuAccessibleTouchTargets"]=@(targets && !_battleMenuButtons[4].enabled && _battleMenuButtons[5].enabled);
    [_battleMenuButtons[0] sendActionsForControlEvents:UIControlEventTouchUpInside];
    uint32_t x=0,y=0; BOOL pressed=NO;
    [_condition lock]; BOOL queued=_battleMenuChoicePending;
    [self takeBattleMenuTouchLocked:&bag party:&emptyParty reads:100 x:&x y:&y pressed:&pressed];
    BOOL started=pressed && x==7 && y==7 && _battleMenuTouchActive;
    pressed=NO; [self takeBattleMenuTouchLocked:&bag party:&emptyParty reads:103 x:&x y:&y pressed:&pressed]; BOOL held=pressed;
    [self takeBattleMenuTouchLocked:&bag party:&emptyParty reads:104 x:&x y:&y pressed:&pressed]; BOOL released=!pressed && _battleMenuTouchCooling;
    [self takeBattleMenuTouchLocked:&bag party:&emptyParty reads:105 x:&x y:&y pressed:&pressed]; BOOL cooling=!pressed && _battleMenuTouchCooling;
    [self takeBattleMenuTouchLocked:&bag party:&emptyParty reads:106 x:&x y:&y pressed:&pressed];
    report[@"battleMenuOriginalTouchLease"]=@(queued && started && held && released && cooling && !_battleMenuTouchCooling);
    _battleMenuChoicePending=YES; _battleMenuChoiceKind=OriginalBattleMenuBag; _battleMenuChoiceRevision=17; _battleMenuChoicePhase=bag.phase; _battleMenuChoiceIndex=0;
    bag.revision=18; pressed=NO; [self takeBattleMenuTouchLocked:&bag party:&emptyParty reads:110 x:&x y:&y pressed:&pressed];
    report[@"battleMenuDropsStaleQueuedTouch"]=@(!pressed && !_battleMenuChoicePending && !_battleMenuTouchActive);
    _battleMenuChoicePending=_battleMenuTouchActive=_battleMenuTouchCooling=YES; [_condition unlock];
    [self pause];
    [_condition lock]; report[@"battleMenuPauseClearsInput"]=@(!_battleMenuChoicePending && !_battleMenuTouchActive && !_battleMenuTouchCooling); [_condition unlock]; [self resume];
    HGNativeBattlePartyUIState party={.active=1,.revision=31,.phase=HG_BATTLE_PARTY_UI_ROSTER,.ready=1,.mode=HG_BATTLE_PARTY_UI_USE_ITEM,.partyCount=1,.itemId=HG_BATTLE_PARTY_UI_POTION,.buttonCount=7,.enabledMask=65};
    party.mons[0]=(HGNativeBattlePartyUIMon){.species=152,.level=5,.hp=20,.maxHp=20,.status=7,.slotType=1};
    for (NSUInteger i=0;i<7;i++) party.buttons[i]=(HGNativeBattlePartyUIButton){(uint32_t)i*24+7,7,(uint32_t)i*24,0,16,16};
    _battleBagUIState=zeroBag; _battlePartyUIState=[NSData dataWithBytes:&party length:sizeof(party)]; [self displayFrame]; [self viewDidLayoutSubviews];
    report[@"battleMenuHealthyStatus"]=@(!OriginalPartyStatusName(7).length && ![_battleMenuButtons[0].accessibilityLabel containsString:@"异常"]);
    report[@"battleMenuUnsetStatus"]=@(!OriginalPartyStatusName(0).length);
    party.mons[0].isEgg=1; _battlePartyUIState=[NSData dataWithBytes:&party length:sizeof(party)]; [self displayFrame];
    report[@"battlePartyEggConcealsSpecies"]=@([_battleMenuButtons[0].accessibilityLabel isEqual:@"蛋"]);
    party.mons[0].isEgg=0;
    party.mons[0].status=4; _battlePartyUIState=[NSData dataWithBytes:&party length:sizeof(party)]; [self displayFrame];
    report[@"battleMenuPoisonStatus"]=@([_battleMenuButtons[0].accessibilityLabel containsString:@"中毒"]);
    report[@"battleMenuKeepsOriginalTargetEligibility"]=@(_battleMenuButtons[0].enabled && _battleMenuButtons[1].hidden && _battleMenuButtons[6].enabled);
    party.phase=HG_BATTLE_PARTY_UI_MESSAGE; party.ready=party.buttonCount=party.enabledMask=0;
    _battlePartyUIState=[NSData dataWithBytes:&party length:sizeof(party)]; [self displayFrame];
    BOOL readOnly=!_battleMenuMessage.hidden && !_battleMenuPanel.hidden;
    for (UIButton *button in _battleMenuButtons) if (!button.hidden || button.enabled) readOnly=NO;
    report[@"battleMenuReadOnlyOriginalMessage"]=@(readOnly);
    party.phase=HG_BATTLE_PARTY_UI_ACK; party.ready=party.buttonCount=party.enabledMask=1;
    party.buttons[0]=(HGNativeBattlePartyUIButton){124,168,16,152,216,32};
    _battlePartyUIState=[NSData dataWithBytes:&party length:sizeof(party)]; [self displayFrame]; [self viewDidLayoutSubviews];
    report[@"battleMenuOriginalMessageAcknowledgement"]=@(_battleMenuButtons[0].enabled && _battleMenuMessage.hidden && _battleMenuImages[0].image==tile);
    NSData *originalSub=[[NSMutableData dataWithLength:256*192*4] copy];
    party.mode=HG_BATTLE_PARTY_UI_SWITCH; party.phase=HG_BATTLE_PARTY_UI_SUMMARY; party.page=2; party.revision=41;
    party.buttonCount=4; party.enabledMask=12; party.mons[0].status=7;
    party.moves[0]=(HGNativeBattlePartyUIMove){33,35,35}; party.moves[1]=(HGNativeBattlePartyUIMove){45,40,40};
    for (NSUInteger i=0;i<HG_BATTLE_PARTY_UI_BUTTONS;i++) party.buttons[i]=(HGNativeBattlePartyUIButton){(uint32_t)i*24+7,7,(uint32_t)i*24,0,16,16};
    _battleMenuPictures=[self copyBattleMenuPictures:&emptyBag party:&party frame:originalSub subFrame:originalSub];
    _battlePartyUIState=[NSData dataWithBytes:&party length:sizeof(party)]; [self displayFrame]; [self viewDidLayoutSubviews];
    BOOL summaryBody=_battleMenuDescription.image.size.width==256 && _battleMenuDescription.image.size.height==152
        && _battleMenuDescription.isAccessibilityElement && _battleMenuPreview.hidden && !_battleMenuPreview.image && _battleMenuSelected.hidden && _battleMenuQuantity.hidden;
    for (NSUInteger i=0;i<4;i++) summaryBody&=!_battleMenuButtons[i].hidden && _battleMenuButtons[i].bounds.size.height>=44
        && CGRectGetMaxY(_battleMenuDescription.frame)<=CGRectGetMinY(_battleMenuButtons[i].frame);
    report[@"battlePartyInfoSummaryBodyAndFooter"]=@(summaryBody && !_battleMenuButtons[0].enabled && !_battleMenuButtons[1].enabled && _battleMenuButtons[2].enabled && _battleMenuButtons[3].enabled);
    party.phase=HG_BATTLE_PARTY_UI_CHECK_MOVES; party.page=3; party.revision=42; party.buttonCount=8; party.enabledMask=195;
    _battleMenuPictures=[self copyBattleMenuPictures:&emptyBag party:&party frame:originalSub subFrame:originalSub];
    _battlePartyUIState=[NSData dataWithBytes:&party length:sizeof(party)]; [self displayFrame]; [self viewDidLayoutSubviews];
    BOOL moveCards=_battleMenuSelected.image.size.width==72 && _battleMenuSelected.image.size.height==16 && _battleMenuSelected.isAccessibilityElement
        && _battleMenuDescription.hidden && _battleMenuPreview.hidden && !_battleMenuPreview.image && _battleMenuButtons[8].hidden;
    for (NSUInteger i=0;i<8;i++) moveCards&=!_battleMenuButtons[i].hidden && _battleMenuButtons[i].bounds.size.width>=44 && _battleMenuButtons[i].bounds.size.height>=44 && _battleMenuImages[i].image!=nil;
    report[@"battlePartyInfoMovesCardsAndNavigation"]=@(moveCards && _battleMenuButtons[0].enabled && _battleMenuButtons[1].enabled && !_battleMenuButtons[2].enabled && !_battleMenuButtons[3].enabled
        && !_battleMenuButtons[4].enabled && !_battleMenuButtons[5].enabled && _battleMenuButtons[6].enabled && _battleMenuButtons[7].enabled && [_battleMenuButtons[0].accessibilityLabel containsString:@"35／35"]);
    party.phase=HG_BATTLE_PARTY_UI_MOVE_DETAIL; party.page=4; party.revision=43; party.selectedMove=1; party.buttonCount=5; party.enabledMask=19;
    _battleMenuPictures=[self copyBattleMenuPictures:&emptyBag party:&party frame:originalSub subFrame:originalSub];
    _battlePartyUIState=[NSData dataWithBytes:&party length:sizeof(party)]; [self displayFrame]; [self viewDidLayoutSubviews];
    BOOL detailBody=_battleMenuDescription.image.size.width==256 && _battleMenuDescription.image.size.height==152 && !_battleMenuDescription.hidden
        && _battleMenuSelected.hidden && _battleMenuQuantity.hidden && _battleMenuPreview.hidden && !_battleMenuPreview.image;
    for (NSUInteger i=0;i<5;i++) detailBody&=!_battleMenuButtons[i].hidden && _battleMenuButtons[i].bounds.size.width>=44 && _battleMenuButtons[i].bounds.size.height>=44
        && CGRectGetMaxY(_battleMenuDescription.frame)<=CGRectGetMinY(_battleMenuButtons[i].frame);
    for (NSUInteger i=0;i<4;i++) detailBody&=party.moves[i].moveId ? party.moves[i].moveId<_moveNames.count && [_battleMenuButtons[i].currentTitle isEqualToString:_moveNames[party.moves[i].moveId]] : _battleMenuButtons[i].currentTitle.length==0;
    report[@"battlePartyInfoDetailBodyAndTabs"]=@(detailBody && _battleMenuButtons[4].enabled && (_battleMenuButtons[1].accessibilityTraits&UIAccessibilityTraitSelected)
        && [_battleMenuButtons[1].accessibilityLabel containsString:@"40／40"]);
    [_battleMenuButtons[0] sendActionsForControlEvents:UIControlEventTouchUpInside];
    party.revision=44; party.selectedMove=0; pressed=NO;
    [_condition lock]; [self takeBattleMenuTouchLocked:&emptyBag party:&party reads:200 x:&x y:&y pressed:&pressed];
    report[@"battlePartyInfoRejectsStaleQueuedTouch"]=@(!pressed && !_battleMenuChoicePending && !_battleMenuTouchActive); [_condition unlock];
    party.ready=0;
    NSDictionary *infoSeed=@{@"content":tile,@"buttons":@[tile]}; _battleMenuOwnerPictures=infoSeed;
    _battleMenuOwnerKind=OriginalBattleMenuParty; _battleMenuOwnerRevision=party.revision; _battleMenuOwnerPhase=party.phase;
    NSDictionary *infoGap=[self copyBattleMenuPictures:&emptyBag party:&party frame:originalSub subFrame:originalSub];
    BOOL infoCache=infoGap[@"content"]==tile && infoGap[@"buttons"][0]==tile;
    for (NSUInteger i=0;i<3;i++) {
        HGNativeBattlePartyUIState changed=party;
        if (!i) changed.revision++; else if (i==1) { changed.phase=HG_BATTLE_PARTY_UI_CHECK_MOVES; changed.page=3; } else changed.active=0;
        _battleMenuOwnerPictures=infoSeed; _battleMenuOwnerKind=OriginalBattleMenuParty; _battleMenuOwnerRevision=party.revision; _battleMenuOwnerPhase=party.phase;
        NSDictionary *cleared=[self copyBattleMenuPictures:&emptyBag party:&changed frame:originalSub subFrame:originalSub];
        infoCache&=!cleared[@"content"] && (!cleared || cleared[@"buttons"][0]!=tile);
    }
    report[@"battlePartyRawInfoCacheScope"]=@(infoCache); party.ready=1;
    party.phase=HG_BATTLE_PARTY_UI_SUMMARY; party.page=2; party.mode=HG_BATTLE_PARTY_UI_USE_ITEM;
    BOOL contextFallback=OriginalBattleMenuKind(&emptyBag,&party)==0;
    party.mode=HG_BATTLE_PARTY_UI_SWITCH; party.mons[0].isEgg=1; contextFallback&=OriginalBattleMenuKind(&emptyBag,&party)==0;
    party.mons[0].isEgg=0; party.page=3; contextFallback&=OriginalBattleMenuKind(&emptyBag,&party)==0;
    party.page=2; party.partyCount=HG_BATTLE_PARTY_UI_MONS+1; party.selectedSlot=HG_BATTLE_PARTY_UI_MONS;
    contextFallback&=OriginalBattleMenuKind(&emptyBag,&party)==0; party.partyCount=1; party.selectedSlot=0;
    report[@"battlePartyInfoKeepsContextFallback"]=@(contextFallback);
    party.mode=1; _battlePartyUIState=[NSData dataWithBytes:&party length:sizeof(party)];
    [_condition lock]; _selectedEngine=1; [_condition unlock]; [self displayFrame];
    report[@"battleMenuUnsupportedModeFallback"]=@(!_battleMenuKind && _battleMenuPanel.hidden && !_canvas.hidden && _canvas.userInteractionEnabled && !_buttons[@1].hidden);
    party.mode=HG_BATTLE_PARTY_UI_USE_ITEM; party.itemId=18;
    report[@"battleMenuOtherItemContextFallback"]=@(OriginalBattleMenuKind(&emptyBag,&party)==0);
    _battleUIState=oldBattle; _battleBagUIState=zeroBag; _battlePartyUIState=zeroParty; _battleMenuPictures=nil;
    [_condition lock]; _selectedEngine=0; _battleMenuChoicePending=_battleMenuTouchActive=_battleMenuTouchCooling=NO; [_condition unlock];
    HGNativeBattleUIState emptyBattle={0}; [self updateBattleControls:[NSData dataWithBytes:&emptyBattle length:sizeof(emptyBattle)]]; [self displayFrame];
    _battleBagUIState=oldBattleBag; _battlePartyUIState=oldBattleParty; _battleMenuPictures=oldPictures;
    _battleMenuOwnerPictures=oldOwnerPictures; _battleMenuOwnerKind=oldOwnerKind; _battleMenuOwnerRevision=oldOwnerRevision; _battleMenuOwnerPhase=oldOwnerPhase;
}
- (void)checkSummaryFlow:(NSMutableDictionary *)report completion:(dispatch_block_t)completion {
    __block NSUInteger stage=0,attempts=0,moveIndex=0; __block uint32_t originalPartyRevision=0,pages=0;
    __block void (^step)(void);
    for (NSString *key in @[@"actualSummarySingleScreenFlow",@"actualSummaryOriginalInfo",@"actualSummaryOriginalMoves",@"actualSummaryOriginalRibbons",
        @"actualSummaryOriginalMoveDetails",@"actualSummaryMoveBackPreservesSummary",@"actualSummaryReturnsNewParty",@"actualSummaryNoSwapControl",
        @"actualSummaryAllNativePages",@"actualSummaryRibbonEntryPreserved"]) report[key]=@NO;
    step=^{
        const HGNativeSummaryUIState *summary=self->_displayedSummaryUIState.bytes;
        const HGNativePartyUIState *party=self->_displayedPartyUIState.bytes;
        BOOL valid=self->_displayedSummaryUIState.length==sizeof(*summary);
        BOOL ready=valid && summary->active && !summary->mode && summary->monDataType==1 && !summary->moveToLearn && summary->ready;
        BOOL partyReady=self->_displayedPartyUIState.length==sizeof(*party) && party->active && party->ready;
        [self->_condition lock]; BOOL failed=self->_failure!=nil || self->_finished; uint32_t map=self->_fieldMapId; [self->_condition unlock];
        if (failed || ++attempts>600) { step=nil; completion(); return; }
        BOOL native=ready && !self->_summaryPanel.hidden && self->_canvas.hidden && !self->_canvas.userInteractionEnabled && self->_buttons[@1].hidden;
        if (native && summary->phase==HG_SUMMARY_UI_INPUT && summary->page<=2) pages|=1u<<summary->page;
        NSString *capture=nil; dispatch_block_t action=nil;
        if (!stage && map==MAP_ROUTE_29 && !self->_fieldMenu.hidden && self->_fieldMenu.userInteractionEnabled) {
            for (NSUInteger i=0;i<7;i++) if (((self->_fieldMenuIcons>>(i*4))&15)==1 && [self->_fieldMenuElements[i] accessibilityActivate]) { stage=1; break; }
        } else if (stage==1 && partyReady && party->phase==HG_PARTY_UI_ROSTER) {
            for (NSUInteger i=0;i<HG_PARTY_UI_SLOTS;i++) if (party->mons[i].active && !party->mons[i].isEgg && self->_partyMons[i].enabled) {
                originalPartyRevision=party->revision; action=^{ [self->_partyMons[i] sendActionsForControlEvents:UIControlEventTouchUpInside]; }; stage=2; break;
            }
        } else if (stage==2 && partyReady && party->phase==HG_PARTY_UI_CONTEXT) {
            for (NSUInteger i=0;i<MIN(party->actionCount,HG_PARTY_UI_ACTIONS);i++) if (party->actions[i].textId==2 && self->_partyActions[i].enabled) {
                action=^{ [self->_partyActions[i] sendActionsForControlEvents:UIControlEventTouchUpInside]; }; stage=3; break;
            }
        } else if (stage==3 && ready && summary->phase==HG_SUMMARY_UI_INPUT && summary->page==0 && self->_summaryNavigation[2].enabled) {
            report[@"actualSummaryOriginalInfo"]=@(native && self->_summaryPreview.image.size.width==256 && self->_summaryPreview.image.size.height==192
                && self->_summaryContent.image.size.width==144 && self->_summaryContent.image.size.height==160
                && self->_summaryPictures[@"nickname"]!=nil && self->_summaryTitle.image!=nil);
            capture=@"original-summary-info-check.png"; action=^{ [self->_summaryNavigation[2] sendActionsForControlEvents:UIControlEventTouchUpInside]; }; stage=4;
        } else if (stage==4 && ready && summary->phase==HG_SUMMARY_UI_INPUT && summary->page==1 && self->_summaryNavigation[2].enabled) {
            for (NSUInteger i=0;i<HG_SUMMARY_UI_MOVES;i++) if ((summary->moveMask&(1u<<i)) && self->_summaryMoves[i].enabled) {
                moveIndex=i; const HGNativeSummaryUIMove *move=&summary->moves[i];
                report[@"actualSummaryOriginalMoves"]=@(native && self->_summaryContent.hidden && self->_summaryMoveImages[i].image.size.width==move->cropWidth
                    && self->_summaryMoveImages[i].image.size.height==move->cropHeight && self->_summaryMoves[i].bounds.size.height>=44);
                capture=@"original-summary-moves-check.png"; action=^{ [self->_summaryNavigation[2] sendActionsForControlEvents:UIControlEventTouchUpInside]; }; stage=5; break;
            }
        } else if (stage==5 && ready && summary->phase==HG_SUMMARY_UI_INPUT && summary->page==2 && self->_summaryNavigation[1].enabled) {
            report[@"actualSummaryOriginalRibbons"]=@(native && self->_summaryContent.image.size.width==144 && self->_summaryContent.image.size.height==160
                && self->_summaryPictures[@"back"] && [self checkSummaryRasterBack]);
            report[@"actualSummaryRibbonEntryPreserved"]=@(summary->ribbonCount ? !self->_summaryNavigation[5].hidden && self->_summaryNavigation[5].enabled : self->_summaryNavigation[5].hidden);
            capture=@"original-summary-ribbons-check.png"; action=^{ [self->_summaryNavigation[1] sendActionsForControlEvents:UIControlEventTouchUpInside]; }; stage=6;
        } else if (stage==6 && ready && summary->phase==HG_SUMMARY_UI_INPUT && summary->page==1 && self->_summaryMoves[moveIndex].enabled) {
            action=^{ [self->_summaryMoves[moveIndex] sendActionsForControlEvents:UIControlEventTouchUpInside]; }; stage=7;
        } else if (stage==7 && ready && summary->phase==HG_SUMMARY_UI_MOVE_SELECT && summary->page==1 && self->_summaryNavigation[0].enabled) {
            BOOL backOnly=YES; for (NSUInteger i=1;i<6;i++) if (!self->_summaryNavigation[i].hidden) backOnly=NO;
            report[@"actualSummaryOriginalMoveDetails"]=@(native && self->_summaryContent.image.size.width==120 && self->_summaryContent.image.size.height==160
                && self->_summaryMoveImages[moveIndex].image.size.width==summary->moves[moveIndex].cropWidth);
            report[@"actualSummaryNoSwapControl"]=@(backOnly && self->_buttons[@1].hidden && self->_buttons[@2].hidden);
            capture=@"original-summary-move-detail-check.png"; action=^{ [self->_summaryNavigation[0] sendActionsForControlEvents:UIControlEventTouchUpInside]; }; stage=8;
        } else if (stage==8 && ready && summary->phase==HG_SUMMARY_UI_INPUT && summary->page==1 && self->_summaryNavigation[0].enabled) {
            report[@"actualSummaryMoveBackPreservesSummary"]=@(native);
            report[@"actualSummaryAllNativePages"]=@(pages==7);
            action=^{ [self->_summaryNavigation[0] sendActionsForControlEvents:UIControlEventTouchUpInside]; }; stage=9;
        } else if (stage==9 && valid && !summary->active && partyReady && party->phase==HG_PARTY_UI_ROSTER && self->_partyBack.enabled) {
            report[@"actualSummaryReturnsNewParty"]=@(party->revision>originalPartyRevision && !self->_partyPanel.hidden && self->_summaryPanel.hidden);
            action=^{ [self->_partyBack sendActionsForControlEvents:UIControlEventTouchUpInside]; }; stage=10;
        } else if (stage==10 && valid && !summary->active && self->_displayedPartyUIState.length==sizeof(*party) && !party->active
            && map==MAP_ROUTE_29 && self->_summaryPanel.hidden && self->_partyPanel.hidden) {
            report[@"actualSummarySingleScreenFlow"]=@YES; step=nil; completion(); return;
        }
        if (capture) {
            UIGraphicsBeginImageContextWithOptions(self.view.bounds.size,NO,0); BOOL complete=[self.view drawViewHierarchyInRect:self.view.bounds afterScreenUpdates:YES];
            UIImage *image=UIGraphicsGetImageFromCurrentImageContext(); UIGraphicsEndImageContext();
            report[@"actualCapturesComplete"]=@([report[@"actualCapturesComplete"] boolValue] && complete && image!=nil);
            NSURL *folder=[NSFileManager.defaultManager URLsForDirectory:NSDocumentDirectory inDomains:NSUserDomainMask].firstObject;
            [UIImagePNGRepresentation(image) writeToURL:[folder URLByAppendingPathComponent:capture] options:NSDataWritingAtomic error:nil];
        }
        if (action) action();
        dispatch_after(dispatch_time(DISPATCH_TIME_NOW,NSEC_PER_SEC/10),dispatch_get_main_queue(),step);
    };
    step();
}
- (void)checkBagFlow:(NSMutableDictionary *)report completion:(dispatch_block_t)completion {
    __block NSUInteger stage=0,attempts=0,nextPocket=0;
    __block uint32_t target=UINT32_MAX,firstItemPocket=UINT32_MAX,visited=0;
    __block void (^step)(void);
    for (NSString *key in @[@"actualBagSingleScreenFlow",@"actualBagPocketsVisited",@"actualBagOriginalItemCard",@"actualBagOriginalActionGlyph",@"actualBagClosedPromptHidden"]) report[key]=@NO;
    step=^{
        const HGNativeBagUIState *bag=self->_displayedBagUIState.bytes;
        BOOL valid=self->_displayedBagUIState.length==sizeof(*bag),ready=valid && bag->active && !bag->mode && bag->ready;
        [self->_condition lock]; BOOL failed=self->_failure!=nil || self->_finished; uint32_t map=self->_fieldMapId; [self->_condition unlock];
        if (failed || ++attempts>480) { step=nil; completion(); return; }
        if (ready && bag->phase==HG_BAG_UI_INPUT && bag->pocketIndex<HG_BAG_UI_POCKETS) {
            visited|=1u<<bag->pocketIndex;
            if (firstItemPocket==UINT32_MAX && bag->visibleCount && bag->itemMask) firstItemPocket=bag->pocketIndex;
        }
        NSString *capture=nil; dispatch_block_t action=nil;
        if (!stage && map==MAP_ROUTE_29 && !self->_fieldMenu.hidden && self->_fieldMenu.userInteractionEnabled) {
            for (NSUInteger i=0;i<7;i++) if (((self->_fieldMenuIcons>>(i*4))&15)==2 && [self->_fieldMenuElements[i] accessibilityActivate]) { stage=1; break; }
        } else if (stage==1 && ready && bag->phase==HG_BAG_UI_INPUT) {
            capture=@"original-bag-pockets-check.png"; stage=2;
        } else if (stage==2 && ready && bag->phase==HG_BAG_UI_INPUT) {
            if (target!=UINT32_MAX && bag->pocketIndex==target) { target=UINT32_MAX; nextPocket++; }
            if (target==UINT32_MAX) {
                while (nextPocket<HG_BAG_UI_POCKETS && (!(bag->pocketMask&(1u<<nextPocket)) || bag->pocketIndex==nextPocket)) nextPocket++;
                if (nextPocket<HG_BAG_UI_POCKETS && self->_bagPockets[nextPocket].enabled) {
                    target=(uint32_t)nextPocket; action=^{ [self->_bagPockets[nextPocket] sendActionsForControlEvents:UIControlEventTouchUpInside]; };
                } else if (nextPocket==HG_BAG_UI_POCKETS) {
                    report[@"actualBagPocketsVisited"]=@((visited&bag->pocketMask)==bag->pocketMask && bag->pocketMask!=0);
                    stage=3;
                }
            }
        } else if (stage==3 && ready && bag->phase==HG_BAG_UI_INPUT) {
            if (firstItemPocket==UINT32_MAX) stage=6;
            else if (bag->pocketIndex==firstItemPocket) { target=firstItemPocket; stage=4; }
            else if (self->_bagPockets[firstItemPocket].enabled) {
                target=firstItemPocket; action=^{ [self->_bagPockets[firstItemPocket] sendActionsForControlEvents:UIControlEventTouchUpInside]; }; stage=4;
            }
        } else if (stage==4 && ready && bag->phase==HG_BAG_UI_INPUT && bag->pocketIndex==target) {
            for (NSUInteger i=0;i<MIN(bag->visibleCount,HG_BAG_UI_ITEMS);i++) if ((bag->itemMask&(1u<<i)) && self->_bagItems[i].enabled) {
                const HGNativeBagUIItem *item=&bag->items[i]; UIImage *image=self->_bagItemImages[i].image;
                report[@"actualBagOriginalItemCard"]=@(!self->_bagPanel.hidden && self->_menuButton.hidden && self->_canvas.hidden && image.size.width==item->cropWidth && image.size.height==item->cropHeight
                    && self->_bagPreview.image.size.width==256 && self->_bagPreview.image.size.height==192 && [self checkBagItem:i state:bag]);
                capture=@"original-bag-list-check.png"; action=^{ [self->_bagItems[i] sendActionsForControlEvents:UIControlEventTouchUpInside]; }; stage=5; break;
            }
        } else if (stage==5 && ready && bag->phase==HG_BAG_UI_ITEM_MENU && self->_bagNavigation[0].enabled) {
            BOOL glyph=NO;
            for (NSUInteger i=0;i<HG_BAG_UI_ACTIONS;i++) if ((bag->actionMask&(1u<<i)) && self->_bagActions[i].configuration.image) glyph=YES;
            report[@"actualBagOriginalActionGlyph"]=@(glyph && self->_bagSelectedEntry.image!=nil);
            capture=@"original-bag-item-check.png"; action=^{ [self->_bagNavigation[0] sendActionsForControlEvents:UIControlEventTouchUpInside]; }; stage=6;
        } else if (stage==6 && ready && bag->phase==HG_BAG_UI_INPUT && self->_bagNavigation[0].enabled) {
            report[@"actualBagClosedPromptHidden"]=@(firstItemPocket!=UINT32_MAX && self->_bagPrompt.hidden && !self->_bagPrompt.image);
            action=^{ [self->_bagNavigation[0] sendActionsForControlEvents:UIControlEventTouchUpInside]; }; stage=7;
        } else if (stage==7 && valid && !bag->active && map==MAP_ROUTE_29 && self->_bagPanel.hidden) {
            report[@"actualBagSingleScreenFlow"]=@YES; step=nil; completion(); return;
        }
        if (capture) {
            UIGraphicsBeginImageContextWithOptions(self.view.bounds.size,NO,0); BOOL complete=[self.view drawViewHierarchyInRect:self.view.bounds afterScreenUpdates:YES];
            UIImage *image=UIGraphicsGetImageFromCurrentImageContext(); UIGraphicsEndImageContext();
            report[@"actualCapturesComplete"]=@([report[@"actualCapturesComplete"] boolValue] && complete && image!=nil);
            NSURL *folder=[NSFileManager.defaultManager URLsForDirectory:NSDocumentDirectory inDomains:NSUserDomainMask].firstObject;
            [UIImagePNGRepresentation(image) writeToURL:[folder URLByAppendingPathComponent:capture] options:NSDataWritingAtomic error:nil];
        }
        if (action) action();
        dispatch_after(dispatch_time(DISPATCH_TIME_NOW,NSEC_PER_SEC/10),dispatch_get_main_queue(),step);
    };
    step();
}
- (void)checkPartyFlow:(NSMutableDictionary *)report completion:(dispatch_block_t)completion {
    __block NSUInteger stage=0,attempts=0,slot=0,waitPolls=0; __block uint32_t pickerRevision=0,continued=0; __block void (^step)(void);
    for (NSString *key in @[@"actualPartySingleScreenFlow",@"actualPartyGivePicker",@"actualPartyGiveCancelPreservesHeldItem",
        @"actualPartyGiveOriginalMessage",@"actualPartyGiveHeldPotion",@"actualPartyGiveBagQuantity",@"actualPartyTakeOriginalMessage",
        @"actualPartyTakeRestoresHeldItem",@"actualPartyTakeBagQuantity",@"actualPartyGiveTakeFlow",@"actualPartyGiveOriginalItemGlyph"]) report[key]=@NO;
    step=^{
        const HGNativePartyUIState *party=self->_displayedPartyUIState.bytes;
        const HGNativeBagUIState *bag=self->_displayedBagUIState.bytes;
        BOOL valid=self->_displayedPartyUIState.length==sizeof(*party);
        BOOL ready=valid && party->active && !party->context && party->ready;
        BOOL bagReady=self->_displayedBagUIState.length==sizeof(*bag) && bag->active && HGNativeBagUIIsList(bag) && bag->ready;
        [self->_condition lock]; BOOL failed=self->_failure!=nil || self->_finished; uint32_t map=self->_fieldMapId; [self->_condition unlock];
        if (failed || ++attempts>900) { step=nil; completion(); return; }
        NSString *capture=nil; dispatch_block_t action=nil;
        uint32_t continueBit=stage==12 ? 1 : 2;
        if ((stage==12 || stage==19) && !(continued&continueBit) && valid && HGNativePartyUIIsItemProgress(party)
            && party->ready && party->cancelEnabled && self->_partyBack.enabled && self->_partyMessage.image && !self->_partyMessage.hidden) {
            BOOL readOnly=!self->_partyPanel.hidden && self->_canvas.hidden && !self->_canvas.userInteractionEnabled
                && self->_buttons[@1].hidden && self->_buttons[@2].hidden
                && [self->_partyBack.currentTitle isEqual:@"继续"]
                && self->_partyMessage.image.size.width==216 && self->_partyMessage.image.size.height==32;
            for (UIButton *button in self->_partyMons) if (button.enabled) readOnly=NO;
            for (UIButton *button in self->_partyActions) if (!button.hidden || button.enabled) readOnly=NO;
            NSString *key=stage==12 ? @"actualPartyGiveOriginalMessage" : @"actualPartyTakeOriginalMessage";
            if (++waitPolls>=2) {
                capture=stage==12 ? @"original-party-give-result-check.png" : @"original-party-take-result-check.png";
                report[key]=@(readOnly); continued|=continueBit; waitPolls=0;
                action=^{ [self->_partyBack sendActionsForControlEvents:UIControlEventTouchUpInside]; };
            }
        }
        if (!stage && map==MAP_ROUTE_29 && !self->_fieldMenu.hidden && self->_fieldMenu.userInteractionEnabled) {
            for (NSUInteger i=0;i<7;i++) if (((self->_fieldMenuIcons>>(i*4))&15)==1 && [self->_fieldMenuElements[i] accessibilityActivate]) { stage=1; break; }
        } else if (stage==1 && ready && party->phase==HG_PARTY_UI_ROSTER) {
            BOOL native=!self->_partyPanel.hidden && self->_menuButton.hidden && self->_canvas.hidden && !self->_canvas.userInteractionEnabled;
            for (NSUInteger i=0;i<HG_PARTY_UI_SLOTS;i++) if (party->mons[i].active && !party->mons[i].isEgg && !party->mons[i].heldItem) {
                report[@"actualPartyOriginalCard"] = @(native && self->_partyCards[i].image.size.width==128 && self->_partyCards[i].image.size.height==48);
                capture=@"original-party-roster-check.png";
                slot=i; action=^{ [self->_partyMons[i] sendActionsForControlEvents:UIControlEventTouchUpInside]; }; stage=2; break;
            }
        } else if ((stage==2 || stage==3) && ready && party->phase==(stage==2 ? HG_PARTY_UI_CONTEXT : HG_PARTY_UI_SUBCONTEXT)) {
            for (NSUInteger i=0;i<MIN(party->actionCount,HG_PARTY_UI_ACTIONS);i++) if (party->actions[i].textId==(stage==2 ? 3 : 10)) {
                UIButton *button=self->_partyActions[i];
                if (!button.enabled) break;
                if (stage==2) {
                    report[@"actualPartyOriginalActionGlyph"] = @(button.configuration.image!=nil && self->_partySelectedName.image!=nil && self->_partySelectedItem.image!=nil);
                    capture=@"original-party-context-check.png";
                } else { report[@"actualPartyOriginalItemMenu"]=@(party->actionCount==3); capture=@"original-party-item-check.png"; }
                action=^{ [button sendActionsForControlEvents:UIControlEventTouchUpInside]; }; stage++; break;
            }
        } else if ((stage==4 || stage==8 || stage==16) && ready && party->phase==HG_PARTY_UI_ROSTER && self->_partyMons[slot].enabled) {
            if (stage==8) report[@"actualPartyGiveCancelPreservesHeldItem"]=@(party->mons[slot].heldItem==0 && party->revision>pickerRevision && self->_bagPanel.hidden);
            action=^{ [self->_partyMons[slot] sendActionsForControlEvents:UIControlEventTouchUpInside]; }; stage++;
        } else if ((stage==5 || stage==6 || stage==9 || stage==10 || stage==17 || stage==18) && ready
            && party->phase==((stage==5 || stage==9 || stage==17) ? HG_PARTY_UI_CONTEXT : HG_PARTY_UI_SUBCONTEXT)) {
            uint32_t textId=(stage==5 || stage==9 || stage==17) ? 3 : stage==18 ? 5 : 4;
            for (NSUInteger i=0;i<MIN(party->actionCount,HG_PARTY_UI_ACTIONS);i++) if (party->actions[i].textId==textId && self->_partyActions[i].enabled) {
                action=^{ [self->_partyActions[i] sendActionsForControlEvents:UIControlEventTouchUpInside]; }; stage++; break;
            }
        } else if ((stage==7 || stage==11) && bagReady && bag->mode==HG_BAG_UI_GIVE_MODE && bag->phase==HG_BAG_UI_GIVE_INPUT) {
            BOOL native=!self->_bagPanel.hidden && self->_partyPanel.hidden && self->_canvas.hidden && !self->_canvas.userInteractionEnabled
                && self->_bagPreview.image.size.width==256 && self->_bagPreview.image.size.height==192 && self->_bagPrompt.hidden;
            report[@"actualPartyGivePicker"]=@(native && (stage==7 || [report[@"actualPartyGivePicker"] boolValue]));
            if (stage==7 && self->_bagNavigation[0].enabled) {
                pickerRevision=party->revision; capture=@"original-party-give-picker-check.png";
                action=^{ [self->_bagNavigation[0] sendActionsForControlEvents:UIControlEventTouchUpInside]; }; stage=8;
            } else if (stage==11 && bag->pocketId!=1) {
                for (NSUInteger i=0;i<HG_BAG_UI_POCKETS;i++) if (bag->pockets[i].pocketId==1 && self->_bagPockets[i].enabled) {
                    action=^{ [self->_bagPockets[i] sendActionsForControlEvents:UIControlEventTouchUpInside]; }; break;
                }
            } else if (stage==11) {
                for (NSUInteger i=0;i<MIN(bag->visibleCount,HG_BAG_UI_ITEMS);i++) if (bag->items[i].itemId==17 && bag->items[i].quantity==5 && self->_bagItems[i].enabled) {
                    report[@"actualPartyGiveOriginalItemGlyph"]=@([self checkBagItem:i state:bag]);
                    capture=@"original-party-give-potion-check.png";
                    action=^{ [self->_bagItems[i] sendActionsForControlEvents:UIControlEventTouchUpInside]; }; stage=12; break;
                }
            }
        } else if ((stage==12 || stage==19) && ready && party->phase==HG_PARTY_UI_ROSTER && self->_partyBack.enabled) {
            BOOL held=party->mons[slot].heldItem==(stage==12 ? 17 : 0);
            CGPoint center=[self->_partyBack convertPoint:CGPointMake(CGRectGetMidX(self->_partyBack.bounds),CGRectGetMidY(self->_partyBack.bounds)) toView:self.view];
            UIView *hit=[self.view hitTest:center withEvent:nil];
            BOOL back=!self->_partyBack.hidden && [self->_partyBack.currentTitle isEqual:@"返回"] && self->_buttons[@2].hidden
                && (hit==self->_partyBack || [hit isDescendantOfView:self->_partyBack]);
            report[stage==12 ? @"actualPartyGiveHeldPotion" : @"actualPartyTakeRestoresHeldItem"]=@(held && back && (continued&(stage==12 ? 1 : 2)) && self->_partyMessage.hidden && !self->_partyMessage.image);
            capture=stage==12 ? @"original-party-held-potion-check.png" : @"original-party-taken-potion-check.png";
            action=^{ [self->_partyBack sendActionsForControlEvents:UIControlEventTouchUpInside]; }; stage++;
        } else if ((stage==13 || stage==15 || stage==20) && valid && !party->active && map==MAP_ROUTE_29
            && !self->_fieldMenu.hidden && self->_fieldMenu.userInteractionEnabled) {
            uint32_t icon=stage==15 ? 1 : 2;
            for (NSUInteger i=0;i<7;i++) if (((self->_fieldMenuIcons>>(i*4))&15)==icon && [self->_fieldMenuElements[i] accessibilityActivate]) { stage++; break; }
        } else if ((stage==14 || stage==21) && bagReady && !bag->mode && bag->phase==HG_BAG_UI_INPUT) {
            if (bag->pocketId!=1) {
                for (NSUInteger i=0;i<HG_BAG_UI_POCKETS;i++) if (bag->pockets[i].pocketId==1 && self->_bagPockets[i].enabled) {
                    action=^{ [self->_bagPockets[i] sendActionsForControlEvents:UIControlEventTouchUpInside]; }; break;
                }
            } else if (self->_bagNavigation[0].enabled) {
                for (NSUInteger i=0;i<MIN(bag->visibleCount,HG_BAG_UI_ITEMS);i++) if (bag->items[i].itemId==17) {
                    report[stage==14 ? @"actualPartyGiveBagQuantity" : @"actualPartyTakeBagQuantity"]=@(bag->items[i].quantity==(stage==14 ? 4 : 5) && [self checkBagItem:i state:bag]);
                    capture=stage==14 ? @"original-party-give-bag-quantity-check.png" : @"original-party-take-bag-quantity-check.png";
                    action=^{ [self->_bagNavigation[0] sendActionsForControlEvents:UIControlEventTouchUpInside]; }; stage++; break;
                }
            }
        } else if (stage==22 && valid && !party->active && self->_displayedBagUIState.length==sizeof(*bag) && !bag->active
            && map==MAP_ROUTE_29 && self->_partyPanel.hidden && self->_bagPanel.hidden) {
            report[@"actualPartySingleScreenFlow"]=@YES; report[@"actualPartyGiveTakeFlow"]=@YES; step=nil; completion(); return;
        }
        if (capture) {
            UIGraphicsBeginImageContextWithOptions(self.view.bounds.size,NO,0);
            BOOL complete=[self.view drawViewHierarchyInRect:self.view.bounds afterScreenUpdates:YES];
            UIImage *image=UIGraphicsGetImageFromCurrentImageContext(); UIGraphicsEndImageContext();
            report[@"actualCapturesComplete"]=@([report[@"actualCapturesComplete"] boolValue] && complete && image!=nil);
            if ([capture isEqual:@"original-party-held-potion-check.png"] || [capture isEqual:@"original-party-taken-potion-check.png"]) {
                NSString *key=[capture isEqual:@"original-party-held-potion-check.png"] ? @"actualPartyGiveHeldPotion" : @"actualPartyTakeRestoresHeldItem";
                report[key]=@([report[key] boolValue] && complete && [self checkReturnTitle:self->_partyBack inScreenshot:image]);
            }
            NSURL *folder=[NSFileManager.defaultManager URLsForDirectory:NSDocumentDirectory inDomains:NSUserDomainMask].firstObject;
            [UIImagePNGRepresentation(image) writeToURL:[folder URLByAppendingPathComponent:capture] options:NSDataWritingAtomic error:nil];
        }
        if (action) action();
        dispatch_after(dispatch_time(DISPATCH_TIME_NOW,NSEC_PER_SEC/10),dispatch_get_main_queue(),step);
    };
    step();
}
- (void)checkControls {
    NSMutableDictionary *report = [NSMutableDictionary new];
    report[@"actualCapturesComplete"]=@YES;
    uint8_t tiles[65]={0}; tiles[1]=0x21; tiles[33]=0x03;
    HGNativePartyUIWindow window={.pixelBuffer=1,.tileWidth=2,.tileHeight=1,.clearColor=0};
    window.palette[1]=31; window.palette[2]=31<<5; window.palette[3]=31<<10;
    UIImage *glyph=OriginalWindowImage(&window,tiles,sizeof(tiles));
    NSData *glyphPixels=CFBridgingRelease(CGDataProviderCopyData(CGImageGetDataProvider(glyph.CGImage)));
    const uint8_t *glyphBytes=glyphPixels.bytes;
    report[@"partyOriginalTileAndPalette"] = @(glyphPixels.length==16*8*4 && glyphBytes[0]==255 && !glyphBytes[1] && glyphBytes[3]==255
        && glyphBytes[5]==255 && glyphBytes[7]==255 && glyphBytes[34]==255 && glyphBytes[35]==255 && !glyphBytes[39]);
    tiles[1]=0; report[@"partyWindowCopiesBeforeAdvance"] = @(glyphBytes[0]==255);
    NSMutableData *wrongGlyph=[glyphPixels mutableCopy]; ((uint8_t *)wrongGlyph.mutableBytes)[0]^=1;
    UIImage *wrongCard=OriginalPixelImage(wrongGlyph,16,8),*emptyGlyph=OriginalPixelImage([NSMutableData dataWithLength:16*8*4],16,8);
    report[@"bagOriginalGlyphCoherence"]=@([self checkGlyph:glyph inCard:glyph x:0 y:0]
        && ![self checkGlyph:glyph inCard:wrongCard x:0 y:0] && ![self checkGlyph:glyph inCard:glyph x:1 y:0]
        && ![self checkGlyph:emptyGlyph inCard:glyph x:0 y:0]);
    NSMutableData *returnMask=[glyphPixels mutableCopy]; uint8_t *returnBytes=returnMask.mutableBytes;
    for (NSUInteger i=0;i<returnMask.length;i+=4) if (returnBytes[i+3]==255) memset(returnBytes+i,255,3);
    UIImage *returnGlyph=OriginalPixelImage(returnMask,16,8);
    NSMutableData *wrongReturn=[returnMask mutableCopy]; ((uint8_t *)wrongReturn.mutableBytes)[0]^=1;
    report[@"partyReturnGlyphMatcher"]=@([self checkGlyph:returnGlyph inCard:returnGlyph x:0 y:0]
        && ![self checkGlyph:returnGlyph inCard:emptyGlyph x:0 y:0]
        && ![self checkGlyph:returnGlyph inCard:OriginalPixelImage(wrongReturn,16,8) x:0 y:0]
        && ![self checkGlyph:returnGlyph inCard:returnGlyph x:1 y:0]);
    window.pixelBuffer=64;
    BOOL invalidWindow=!OriginalWindowImage(&window,tiles,sizeof(tiles));
    window.pixelBuffer=1; window.tileWidth=UINT32_MAX;
    report[@"partyRejectsInvalidWindow"] = @(invalidWindow && !OriginalWindowImage(&window,tiles,sizeof(tiles)));
    NSMutableData *iconFrame=[NSMutableData dataWithLength:256*192*4]; ((uint8_t *)iconFrame.mutableBytes)[0]=123;
    HGNativePartyUIMon iconMon={.hitX=64,.hitY=24};
    UIImage *icon=OriginalPartyCard(iconFrame,&iconMon);
    NSData *iconPixels=CFBridgingRelease(CGDataProviderCopyData(CGImageGetDataProvider(icon.CGImage)));
    iconMon.hitX=63;
    report[@"partyOriginalWholeCard"] = @(iconPixels.length==128*48*4 && ((const uint8_t *)iconPixels.bytes)[0]==123 && !OriginalPartyCard(iconFrame,&iconMon));
    report[@"partyOriginalActionMetadata"] = @(_partyActionNames.count==33 && [_partyActionNames[3] isEqual:@"ITEM"] && [_partyActionNames[10] isEqual:@"QUIT"]);
    NSMutableData *partyData=[NSMutableData dataWithLength:sizeof(HGNativePartyUIState)]; HGNativePartyUIState *party=partyData.mutableBytes;
    party->active=party->ready=party->count=party->slotMask=party->cancelEnabled=1; party->revision=10; party->phase=HG_PARTY_UI_ROSTER;
    party->mons[0].active=1; party->mons[0].species=152; party->mons[0].hp=19; party->mons[0].maxHp=20; party->mons[0].level=5;
    party->mons[0].hitX=64; party->mons[0].hitY=32; party->cancelX=228; party->cancelY=172;
    [self updatePartyControls:[partyData copy] pictures:nil paused:NO]; [self.view layoutIfNeeded];
    report[@"partyNativeRoster"] = @(!_partyPanel.hidden && !_partyMons[0].hidden && _partyMons[0].enabled && _partyMons[1].hidden
        && _partyMons[0].bounds.size.height>=44 && _partyMons[0].bounds.size.width>=44 && [_partyMons[0].accessibilityLabel containsString:@"19/20"]);
    party->mons[0].isEgg=1; [self updatePartyControls:[partyData copy] pictures:nil paused:NO];
    report[@"partyEggConcealsSpecies"]=@([_partyMons[0].accessibilityLabel isEqual:@"蛋"]);
    party->mons[0].isEgg=0; [self updatePartyControls:[partyData copy] pictures:nil paused:NO];
    [_partyMons[0] sendActionsForControlEvents:UIControlEventTouchUpInside];
    uint32_t partyX=0,partyY=0,partyKeys=0; BOOL partyPressed=NO;
    [_condition lock];
    BOOL queued=_partyChoicePending && _partyChoiceRevision==10 && _partyChoiceKind==HG_PARTY_UI_TOUCH_MON;
    party->ready=0; [self takePartyTouchLocked:party reads:0 keys:&partyKeys x:&partyX y:&partyY pressed:&partyPressed];
    BOOL waitsForPublish=_partyChoicePending && !partyPressed;
    party->ready=1; [self takePartyTouchLocked:party reads:0 keys:&partyKeys x:&partyX y:&partyY pressed:&partyPressed];
    BOOL press=partyPressed && partyX==64 && partyY==32 && _partyTouchActive;
    [self takePartyTouchLocked:party reads:3 keys:&partyKeys x:&partyX y:&partyY pressed:&partyPressed]; BOOL sampled=partyPressed;
    [self takePartyTouchLocked:party reads:4 keys:&partyKeys x:&partyX y:&partyY pressed:&partyPressed];
    report[@"partyOriginalTouchSampling"] = @(queued && waitsForPublish && press && sampled && !partyPressed && !_partyTouchActive);
    _partyChoicePending=YES; _partyChoiceRevision=9;
    [self takePartyTouchLocked:party reads:5 keys:&partyKeys x:&partyX y:&partyY pressed:&partyPressed];
    report[@"partyRejectsStaleRequest"] = @(!_partyChoicePending && !_partyTouchActive && !partyPressed);
    [_condition unlock];
    party->phase=HG_PARTY_UI_SUBCONTEXT; party->actionCount=3; party->actionEnabledMask=7;
    party->actions[0].enabled=1; party->actions[0].textId=3; party->actions[0].hitX=192; party->actions[0].hitY=32;
    [self updatePartyControls:[partyData copy] pictures:nil paused:NO]; [self.view layoutIfNeeded];
    report[@"partyNativeOriginalActions"] = @(!_partyActions[0].hidden && _partyActions[0].enabled && [_partyActions[0].accessibilityLabel isEqual:@"ITEM"] && _partyActions[0].bounds.size.height>=44);
    [_partyActions[0] sendActionsForControlEvents:UIControlEventTouchUpInside]; [self pause];
    [_condition lock]; report[@"partyPauseClearsRequest"] = @(!_partyChoicePending && !_partyTouchActive); [_condition unlock]; [self resume];
    party->context=HG_PARTY_UI_GIVE_CONTEXT; party->phase=HG_PARTY_UI_ITEM_RESULT; party->ready=party->cancelEnabled=0;
    party->message=(HGNativePartyUIWindow){.pixelBuffer=1,.tileWidth=27,.tileHeight=4,.clearColor=15};
    struct w2c_env *savedEnv=_env; NSDictionary *savedPartyPictures=_partyOwnerPictures;
    wasm_rt_memory_t cacheMemory={.data=tiles,.size=sizeof(tiles)}; struct w2c_env cacheEnv={.memory=&cacheMemory};
    _env=&cacheEnv;
    HGNativePartyUIState borrowedGap=*party; borrowedGap.message=(HGNativePartyUIWindow){0};
    NSDictionary *messageSeed=@{@"message":glyph,@"selected":glyph,@"item":glyph,@"cards":@[glyph],@"messageRevision":@(party->revision),@"messageContext":@(party->context),@"messagePhase":@(party->phase)};
    _partyOwnerPictures=messageSeed;
    NSDictionary *retainedMessage=[self copyPartyPictures:&borrowedGap frame:nil];
    BOOL sameProgressMessage=retainedMessage[@"message"]==glyph && retainedMessage[@"selected"]==NSNull.null && retainedMessage[@"item"]==NSNull.null;
    BOOL changedMessageCleared=YES,cardCache=retainedMessage[@"cards"][0]==glyph;
    for (NSUInteger i=0;i<4;i++) {
        HGNativePartyUIState changedMessage=borrowedGap;
        if (!i) changedMessage.revision++;
        else if (i==1) { changedMessage.context=0; changedMessage.phase=HG_PARTY_UI_TAKE_RESULT; }
        else changedMessage.phase=i==2 ? HG_PARTY_UI_GIVE_ITEM : HG_PARTY_UI_ROSTER;
        _partyOwnerPictures=messageSeed;
        NSDictionary *changedPictures=[self copyPartyPictures:&changedMessage frame:nil];
        changedMessageCleared&=changedPictures[@"message"]==NSNull.null || !changedPictures;
        cardCache&=changedPictures[@"cards"][0]!=glyph;
    }
    borrowedGap.active=0; [self copyPartyPictures:&borrowedGap frame:nil];
    report[@"partyProgressMessageCacheScope"]=@(sameProgressMessage && changedMessageCleared && !_partyOwnerPictures);
    report[@"partyRawCardCacheScope"]=@(cardCache && !_partyOwnerPictures);
    _env=savedEnv; _partyOwnerPictures=savedPartyPictures;
    NSData *savedPartyState=_partyUIState; NSDictionary *savedPartyDisplayPictures=_partyPictures;
    uint32_t savedFieldMap=_fieldMapId,savedEngine=_selectedEngine; BOOL savedMenuOpen=_fieldMenuOpen;
    _partyUIState=[partyData copy]; _partyPictures=@{@"message":glyph}; [self displayFrame]; [self.view layoutIfNeeded];
    BOOL progressHidesAB=_buttons[@1].hidden && _buttons[@2].hidden;
    BOOL blocked=!_partyPanel.hidden && !_partyBack.enabled && !_partyMessage.hidden && [_partyBack.currentTitle isEqual:@"继续"] && progressHidesAB;
    for (UIButton *button in _partyMons) if (button.enabled) blocked=NO;
    for (UIButton *button in _partyActions) if (!button.hidden || button.enabled) blocked=NO;
    [_partyBack sendActionsForControlEvents:UIControlEventTouchUpInside];
    [_condition lock];
    report[@"partyItemPrintingBlocksInput"]=@(blocked && !_partyChoicePending);
    _keys=_pendingKeys=_submittedKeys=partyKeys=0x43; _touchPressed=partyPressed=YES;
    [self takePartyTouchLocked:party reads:6 keys:&partyKeys x:&partyX y:&partyY pressed:&partyPressed];
    BOOL printingClearsAB=partyKeys==0x40 && _keys==0x40 && _pendingKeys==0x40 && _submittedKeys==0x40 && !partyPressed && !_touchPressed;
    _keys=_pendingKeys=_submittedKeys=partyKeys=0; [_condition unlock];
    party->ready=party->cancelEnabled=1; party->cancelX=124; party->cancelY=168; party->actionCount=party->actionEnabledMask=0;
    _partyUIState=[partyData copy]; [self displayFrame];
    progressHidesAB&=_buttons[@1].hidden && _buttons[@2].hidden;
    [_partyBack sendActionsForControlEvents:UIControlEventTouchUpInside];
    [_condition lock];
    BOOL continueQueued=_partyChoicePending && _partyChoiceKind==HG_PARTY_UI_TOUCH_CANCEL;
    _keys=_pendingKeys=_submittedKeys=partyKeys=3; _touchPressed=partyPressed=YES;
    [self takePartyTouchLocked:party reads:10 keys:&partyKeys x:&partyX y:&partyY pressed:&partyPressed];
    BOOL waitingClearsAB=!partyKeys && !_keys && !_pendingKeys && !_submittedKeys && !_touchPressed;
    BOOL continuePressed=partyPressed && partyX==124 && partyY==168;
    [self takePartyTouchLocked:party reads:14 keys:&partyKeys x:&partyX y:&partyY pressed:&partyPressed];
    [_condition unlock];
    party->context=0; party->phase=HG_PARTY_UI_TAKE_RESULT;
    _partyUIState=[partyData copy]; [self displayFrame];
    progressHidesAB&=_buttons[@1].hidden && _buttons[@2].hidden;
    uint32_t takeX=0,takeY=0;
    report[@"partyItemOriginalContinue"]=@(continueQueued && continuePressed && !partyPressed && _partyBack.enabled
        && HGNativePartyUIResolveTouch(party,party->revision,party->phase,HG_PARTY_UI_TOUCH_CANCEL,0,&takeX,&takeY) && takeX==124 && takeY==168);
    report[@"partyProgressRejectsRawAB"]=@(printingClearsAB && waitingClearsAB);
    party->context=HG_PARTY_UI_GIVE_CONTEXT; party->phase=HG_PARTY_UI_GIVE_ITEM; party->ready=party->cancelEnabled=0;
    _partyUIState=[partyData copy]; _partyPictures=nil; [self displayFrame];
    progressHidesAB&=_buttons[@1].hidden && _buttons[@2].hidden;
    report[@"partyProgressHidesFallbackButtons"]=@(progressHidesAB);
    BOOL giveProgress=!_partyPanel.hidden && !_partyBack.enabled && _partyMessage.hidden;
    party->phase=HG_PARTY_UI_TAKE_RESULT; _partyUIState=[partyData copy]; [self displayFrame];
    BOOL invalidContext=!_partyScene && _partyPanel.hidden;
    BOOL fallback=!_buttons[@1].hidden && !_buttons[@2].hidden;
    [_condition lock]; _keys=_pendingKeys=_submittedKeys=partyKeys=3;
    [self takePartyTouchLocked:party reads:15 keys:&partyKeys x:&partyX y:&partyY pressed:&partyPressed];
    fallback&=partyKeys==3 && _keys==3 && _pendingKeys==3 && _submittedKeys==3;
    _keys=_pendingKeys=_submittedKeys=partyKeys=0; [_condition unlock];
    party->context=0; party->phase=HG_PARTY_UI_ROSTER; party->ready=party->cancelEnabled=1; party->cancelX=228; party->cancelY=172;
    _partyUIState=[partyData copy]; [self displayFrame]; [self.view layoutIfNeeded];
    CGPoint backCenter=[_partyBack convertPoint:CGPointMake(CGRectGetMidX(_partyBack.bounds),CGRectGetMidY(_partyBack.bounds)) toView:self.view];
    UIView *backHit=[self.view hitTest:backCenter withEvent:nil];
    report[@"partyReturnButtonIsReachable"]=@(!_partyBack.hidden && _partyBack.enabled && [_partyBack.currentTitle isEqual:@"返回"]
        && _buttons[@2].hidden && (backHit==_partyBack || [backHit isDescendantOfView:_partyBack]));
    report[@"partyItemProgressKeepsContext"]=@(giveProgress && invalidContext && !_partyPanel.hidden && _partyMessage.hidden && !_partyMessage.image
        && [_partyBack.currentTitle isEqual:@"返回"]);
    party->active=0; _partyUIState=[partyData copy]; _fieldMapId=MAP_ROUTE_29; _selectedEngine=0; _fieldMenuOpen=NO; [self displayFrame];
    report[@"partyFallbackRestoresAB"]=@(fallback && !_buttons[@1].hidden && !_buttons[@2].hidden && !_partyScene
        && [_buttons[@1].currentTitle isEqual:@"互动"] && [_buttons[@2].currentTitle isEqual:@"奔跑"]);
    report[@"partyExitHidesPanel"] = @(!_partyScene && _partyPanel.hidden && _partyBack.hidden);
    _partyUIState=savedPartyState; _partyPictures=savedPartyDisplayPictures; _fieldMapId=savedFieldMap; _selectedEngine=savedEngine; _fieldMenuOpen=savedMenuOpen; [self displayFrame];
    NSDictionary *savedBagOwner=_bagOwnerPictures;
    HGNativeBagUIState cachedBag={.active=1,.revision=71,.phase=HG_BAG_UI_INPUT};
    NSDictionary *bagSeed=@{@"items":@[glyph],@"itemGlyphs":@[glyph],@"revision":@71,@"mode":@0,@"phase":@(HG_BAG_UI_INPUT)};
    _bagOwnerPictures=bagSeed;
    BOOL bagCache=[self copyBagPictures:&cachedBag frame:nil subFrame:nil]==bagSeed;
    for (NSUInteger i=0;i<4;i++) {
        HGNativeBagUIState changed=cachedBag;
        if (!i) changed.revision++; else if (i==1) changed.phase=HG_BAG_UI_ITEM_MENU;
        else if (i==2) { changed.mode=HG_BAG_UI_GIVE_MODE; changed.phase=HG_BAG_UI_GIVE_INPUT; } else changed.active=0;
        _bagOwnerPictures=bagSeed; bagCache&=![self copyBagPictures:&changed frame:nil subFrame:nil];
    }
    report[@"bagRawCardCacheScope"]=@(bagCache); _bagOwnerPictures=savedBagOwner;
    NSDictionary *savedSummaryOwner=_summaryOwnerPictures;
    HGNativeSummaryUIState cachedSummary={.active=1,.revision=81,.phase=HG_SUMMARY_UI_INPUT,.monDataType=1};
    NSDictionary *summarySeed=@{@"content":glyph,@"revision":@81,@"phase":@(HG_SUMMARY_UI_INPUT),@"page":@0};
    _summaryOwnerPictures=summarySeed;
    BOOL summaryCache=[self copySummaryPictures:&cachedSummary frame:nil subFrame:nil]==summarySeed;
    for (NSUInteger i=0;i<4;i++) {
        HGNativeSummaryUIState changed=cachedSummary;
        if (!i) changed.revision++; else if (i==1) { changed.phase=HG_SUMMARY_UI_MOVE_SELECT; changed.page=1; }
        else if (i==2) changed.page=2; else changed.active=0;
        _summaryOwnerPictures=summarySeed; summaryCache&=![self copySummaryPictures:&changed frame:nil subFrame:nil];
    }
    report[@"summaryRawImageCacheScope"]=@(summaryCache); _summaryOwnerPictures=savedSummaryOwner;
    NSMutableData *bagData=[NSMutableData dataWithLength:sizeof(HGNativeBagUIState)]; HGNativeBagUIState *bag=bagData.mutableBytes;
    bag->active=bag->ready=bag->cancelEnabled=1; bag->revision=21; bag->phase=HG_BAG_UI_INPUT;
    bag->pocketCount=8; bag->pocketMask=255; bag->visibleCount=bag->itemMask=bag->itemCount=1; bag->pageCount=2; bag->cursorButton=8;
    for (NSUInteger i=0;i<HG_BAG_UI_POCKETS;i++) {
        HGNativeBagUIPocket *pocket=&bag->pockets[i]; pocket->pocketId=(uint32_t)i; pocket->enabled=1;
        pocket->cropX=(uint32_t)i*32; pocket->cropWidth=pocket->cropHeight=31; pocket->hitX=pocket->cropX+15; pocket->hitY=15;
    }
    bag->items[0]=(HGNativeBagUIItem){.itemId=17,.quantity=3,.enabled=1,.hitX=63,.hitY=52,.cropY=32,.cropWidth=127,.cropHeight=41};
    bag->cancelX=223; bag->cancelY=179;
    [self updateBagControls:[bagData copy] pictures:nil paused:NO]; [self.view layoutIfNeeded];
    report[@"bagOriginalResourceMetadata"]=@(_pocketNames.count==8 && _itemNames.count==537 && _bagActionNames.count==16 && [_bagActionNames[0] isEqual:@"USE"]);
    report[@"bagNativeListTargets"]=@(!_bagPanel.hidden && _bagPockets[7].enabled && _bagPockets[7].bounds.size.width>=44
        && !_bagItems[0].hidden && _bagItems[0].enabled && _bagItems[0].bounds.size.height>=44 && _bagItems[1].hidden);
    report[@"bagPageImageStaysInBounds"]=@(_bagPage.clipsToBounds && CGRectContainsRect(self.view.bounds,_bagPage.frame));
    [_bagItems[0] sendActionsForControlEvents:UIControlEventTouchUpInside];
    uint32_t bagX=0,bagY=0; BOOL bagPressed=NO;
    [_condition lock]; BOOL bagQueued=_bagChoicePending && _bagChoiceKind==HG_BAG_UI_TOUCH_ITEM;
    bag->ready=0; [self takeBagTouchLocked:bag reads:0 x:&bagX y:&bagY pressed:&bagPressed]; BOOL bagWaits=_bagChoicePending && !bagPressed;
    bag->ready=1; [self takeBagTouchLocked:bag reads:0 x:&bagX y:&bagY pressed:&bagPressed]; BOOL bagSample=bagPressed && bagX==63 && bagY==52;
    [self takeBagTouchLocked:bag reads:4 x:&bagX y:&bagY pressed:&bagPressed];
    report[@"bagOriginalTouchSampling"]=@(bagQueued && bagWaits && bagSample && !bagPressed && !_bagTouchActive);
    _bagChoicePending=YES; _bagChoiceRevision=20;
    [self takeBagTouchLocked:bag reads:5 x:&bagX y:&bagY pressed:&bagPressed];
    report[@"bagRejectsStaleRequest"]=@(!_bagChoicePending && !_bagTouchActive);
    [_condition unlock];
    bag->phase=HG_BAG_UI_ITEM_MENU; bag->actionCount=4; bag->actionMask=1;
    bag->actions[0]=(HGNativeBagUIAction){.actionId=0,.enabled=1,.hitX=45,.hitY=143};
    [self updateBagControls:[bagData copy] pictures:@{@"prompt":glyph} paused:NO]; [self.view layoutIfNeeded];
    report[@"bagNativeActionAndCancel"]=@(_bagItems[0].hidden && !_bagActions[0].hidden && _bagActions[0].enabled && _bagActions[1].hidden
        && _bagNavigation[0].enabled && _bagActions[0].bounds.size.height>=44);
    [_bagActions[0] sendActionsForControlEvents:UIControlEventTouchUpInside]; [self pause];
    [_condition lock]; report[@"bagPauseClearsRequest"]=@(!_bagChoicePending && !_bagTouchActive); [_condition unlock]; [self resume];
    bag->phase=HG_BAG_UI_INPUT; [self updateBagControls:[bagData copy] pictures:@{@"prompt":glyph} paused:NO];
    report[@"bagClosedPromptDoesNotPersist"]=@(_bagPrompt.hidden && !_bagPrompt.image);
    bag->mode=HG_BAG_UI_GIVE_MODE; bag->phase=HG_BAG_UI_GIVE_INPUT;
    [self updateBagControls:[bagData copy] pictures:nil paused:NO]; [self.view layoutIfNeeded];
    BOOL giveList=!_bagPanel.hidden && !_bagItems[0].hidden && _bagItems[0].enabled && _bagActions[0].hidden && _bagNavigation[0].enabled && _bagPrompt.hidden;
    bag->phase=HG_BAG_UI_ITEM_MENU; [self updateBagControls:[bagData copy] pictures:nil paused:NO];
    BOOL giveNoSubmenu=!_bagScene && _bagPanel.hidden;
    bag->phase=HG_BAG_UI_GIVE_INPUT; bag->mode=2; [self updateBagControls:[bagData copy] pictures:nil paused:NO];
    report[@"bagGivePickerKeepsExactContext"]=@(giveList && giveNoSubmenu && !_bagScene && _bagPanel.hidden);
    bag->active=0; [self updateBagControls:[bagData copy] pictures:nil paused:NO];
    report[@"bagExitHidesPanel"]=@(!_bagScene && _bagPanel.hidden && _bagNavigation[0].hidden && _bagPage.hidden);
    NSMutableData *descriptionTiles=[NSMutableData dataWithLength:6145];
    HGNativePartyUIWindow description={.pixelBuffer=1,.tileWidth=32,.tileHeight=6};
    UIImage *descriptionImage=OriginalWindowImage(&description,descriptionTiles.bytes,descriptionTiles.length);
    report[@"bagOriginalDescriptionWindow"]=@(descriptionImage.size.width==256 && descriptionImage.size.height==48
        && !OriginalFrameRegion(iconFrame,UINT32_MAX,0,1,1));
    NSMutableData *summaryData=[NSMutableData dataWithLength:sizeof(HGNativeSummaryUIState)]; HGNativeSummaryUIState *summary=summaryData.mutableBytes;
    summary->active=summary->ready=summary->monDataType=summary->partyCount=summary->backEnabled=1;
    summary->revision=30; summary->phase=HG_SUMMARY_UI_INPUT; summary->pageMask=7;
    summary->species=152; summary->level=5; summary->hp=19; summary->maxHp=20; summary->statusIcon=7;
    summary->previousPageEnabled=summary->nextPageEnabled=1; summary->previousPage=2; summary->nextPage=1;
    NSDictionary *summaryImages=@{@"preview":OriginalPixelImage(iconFrame,256,192),@"content":OriginalFrameRegion(iconFrame,0,0,144,160),@"title":glyph,@"back":glyph};
    [self updateSummaryControls:[summaryData copy] pictures:summaryImages paused:NO]; [self.view layoutIfNeeded];
    BOOL rasterBack=[self checkSummaryRasterBack];
    report[@"summaryRasterBackBeforeFallback"]=@(rasterBack);
    NSMutableDictionary *fallbackImages=[summaryImages mutableCopy]; [fallbackImages removeObjectForKey:@"back"];
    [self updateSummaryControls:[summaryData copy] pictures:fallbackImages paused:NO]; [self.view layoutIfNeeded];
    // UIKit can mirror the same configured caption into attributedTitle without rendering a second caption.
    BOOL fallbackBack=!_summaryNavigation[0].configuration.image && [_summaryNavigation[0].configuration.title isEqual:@"返回"]
        && (!_summaryNavigation[0].configuration.attributedTitle.length || [_summaryNavigation[0].configuration.attributedTitle.string isEqual:@"返回"]);
    report[@"summaryNativeFallbackBack"]=@(fallbackBack);
    [self updateSummaryControls:[summaryData copy] pictures:summaryImages paused:NO]; [self.view layoutIfNeeded];
    BOOL rasterAfter=[self checkSummaryRasterBack]; report[@"summaryRasterBackAfterFallback"]=@(rasterAfter);
    report[@"summaryNativeOriginalInfo"]=@(!_summaryPanel.hidden && _summaryPreview.image.size.width==256 && _summaryContent.image.size.width==144
        && _summaryNavigation[0].enabled && _summaryNavigation[0].bounds.size.height>=44 && [_summaryPreview.accessibilityLabel containsString:@"19/20"]
        && rasterBack && fallbackBack && rasterAfter);
    summary->page=1; summary->moveMask=3;
    for (NSUInteger i=0;i<2;i++) summary->moves[i]=(HGNativeSummaryUIMove){.moveId=33+(uint32_t)i,.pp=0,.maxPP=35,.enabled=1,.hitX=67,.hitY=23+32*(uint32_t)i,
        .cropX=8,.cropY=8+32*(uint32_t)i,.cropWidth=119,.cropHeight=31};
    NSDictionary *moveImages=@{@"preview":summaryImages[@"preview"],@"moves":@[OriginalFrameRegion(iconFrame,8,8,119,31),OriginalFrameRegion(iconFrame,8,40,119,31)],@"title":glyph};
    [self updateSummaryControls:[summaryData copy] pictures:moveImages paused:NO]; [self.view layoutIfNeeded];
    report[@"summaryZeroPPMovesRemainViewable"]=@(_summaryMoves[0].enabled && _summaryMoves[1].enabled && _summaryMoves[2].hidden
        && [_summaryMoves[0].accessibilityLabel containsString:@"PP 0/35"] && _summaryMoves[0].bounds.size.height>=44);
    [_summaryMoves[0] sendActionsForControlEvents:UIControlEventTouchUpInside];
    uint32_t summaryKeys=0,summaryX=0,summaryY=0; BOOL summaryPressed=NO;
    [_condition lock]; BOOL summaryQueued=_summaryChoicePending && _summaryChoiceKind==HG_SUMMARY_UI_INPUT_MOVE;
    summary->ready=0; [self takeSummaryInputLocked:summary reads:0 keys:&summaryKeys x:&summaryX y:&summaryY pressed:&summaryPressed];
    BOOL summaryWaits=_summaryChoicePending && !summaryPressed && !summaryKeys;
    summary->ready=1; [self takeSummaryInputLocked:summary reads:0 keys:&summaryKeys x:&summaryX y:&summaryY pressed:&summaryPressed];
    BOOL summarySample=summaryPressed && summaryX==67 && summaryY==23 && !summaryKeys;
    [self takeSummaryInputLocked:summary reads:3 keys:&summaryKeys x:&summaryX y:&summaryY pressed:&summaryPressed]; BOOL summaryHeld=summaryPressed;
    [self takeSummaryInputLocked:summary reads:4 keys:&summaryKeys x:&summaryX y:&summaryY pressed:&summaryPressed];
    report[@"summaryOriginalTouchSampling"]=@(summaryQueued && summaryWaits && summarySample && summaryHeld && !summaryPressed && !_summaryInputActive);
    _summaryChoicePending=YES; _summaryChoiceRevision=29; _summaryChoicePhase=summary->phase;
    [self takeSummaryInputLocked:summary reads:5 keys:&summaryKeys x:&summaryX y:&summaryY pressed:&summaryPressed];
    report[@"summaryRejectsStaleRequest"]=@(!_summaryChoicePending && !_summaryInputActive && !summaryKeys && !summaryPressed);
    [_condition unlock];
    summary->phase=HG_SUMMARY_UI_MOVE_SELECT;
    [self updateSummaryControls:[summaryData copy] pictures:moveImages paused:NO]; [self.view layoutIfNeeded];
    BOOL onlyBack=YES; for (NSUInteger i=1;i<6;i++) if (!_summaryNavigation[i].hidden) onlyBack=NO;
    [_summaryNavigation[0] sendActionsForControlEvents:UIControlEventTouchUpInside];
    [_condition lock];
    [self takeSummaryInputLocked:summary reads:6 keys:&summaryKeys x:&summaryX y:&summaryY pressed:&summaryPressed]; BOOL summaryBack=summaryKeys==2 && !summaryPressed;
    summary->phase=HG_SUMMARY_UI_INPUT; summary->revision++;
    [self takeSummaryInputLocked:summary reads:7 keys:&summaryKeys x:&summaryX y:&summaryY pressed:&summaryPressed];
    report[@"summaryBackLeaseStopsAtPageReturn"]=@(summaryBack && !summaryKeys && !summaryPressed && !_summaryInputActive && !_summaryInputKeys);
    [_condition unlock];
    report[@"summaryDetailsExposeNoSwap"]=@(onlyBack && _summaryMoves[0].enabled);
    [self updateSummaryControls:[summaryData copy] pictures:moveImages paused:NO];
    [_summaryMoves[0] sendActionsForControlEvents:UIControlEventTouchUpInside]; [self pause];
    [_condition lock]; report[@"summaryPauseClearsRequest"]=@(!_summaryChoicePending && !_summaryInputActive && !_summaryTouchPressed && !_summaryInputKeys); [_condition unlock]; [self resume];
    summary->page=2; summary->ribbonCount=1;
    [self updateSummaryControls:[summaryData copy] pictures:summaryImages paused:NO];
    [_summaryNavigation[5] sendActionsForControlEvents:UIControlEventTouchUpInside];
    [_condition lock];
    [self takeSummaryInputLocked:summary reads:8 keys:&summaryKeys x:&summaryX y:&summaryY pressed:&summaryPressed]; BOOL ribbonInput=summaryKeys==1;
    summary->phase=10;
    [self takeSummaryInputLocked:summary reads:9 keys:&summaryKeys x:&summaryX y:&summaryY pressed:&summaryPressed];
    report[@"summaryRibbonFallbackReleasesInput"]=@(ribbonInput && !summaryKeys && !summaryPressed && !_summaryChoicePending && !_summaryInputActive);
    [_condition unlock];
    [self updateSummaryControls:[summaryData copy] pictures:nil paused:NO];
    report[@"summaryUnsupportedPhaseUsesCanvas"]=@(_summaryScene && _summaryPanel.hidden && _summaryNavigation[0].hidden);
    summary->phase=HG_SUMMARY_UI_INPUT; summary->isEgg=1; summary->page=0; summary->pageMask=1;
    [self updateSummaryControls:[summaryData copy] pictures:summaryImages paused:NO];
    report[@"summaryEggHasOnlyInfo"]=@(!_summaryPanel.hidden && !_summaryNavigation[1].enabled && !_summaryNavigation[2].enabled && _summaryMoves[0].hidden
        && [_summaryPreview.accessibilityLabel isEqual:@"蛋"]);
    summary->isEgg=0; summary->mode=1;
    [self updateSummaryControls:[summaryData copy] pictures:nil paused:NO];
    report[@"summaryOtherModeUsesOriginalControls"]=@(!_summaryScene && _summaryPanel.hidden);
    summary->mode=summary->active=0; [self updateSummaryControls:[summaryData copy] pictures:nil paused:NO];
    report[@"summaryExitHidesPanel"]=@(!_summaryScene && _summaryPanel.hidden && _summaryNavigation[0].hidden);
    uint32_t simulatedReads=0;
    [_buttons[@1] sendActionsForControlEvents:UIControlEventTouchDown];
    [_buttons[@0x20] sendActionsForControlEvents:UIControlEventTouchDown];
    [_condition lock]; report[@"combinedKeys"] = @(_keys == 0x21); [_condition unlock];
    [_buttons[@1] sendActionsForControlEvents:UIControlEventTouchUpInside];
    [_condition lock]; report[@"independentRelease"] = @(_keys == 0x20); [_condition unlock];
    [self toggleMenu];
    _canvas.frame = CGRectMake(0, 0, 512, 384);
    [self touchAt:CGPointMake(256, 192) pressed:YES];
    [_condition lock]; report[@"touchCoordinates"] = @(_touchPressed && _touchX == 128 && _touchY == 96); [_condition unlock];
    [self touchAt:CGPointMake(-20, 400) pressed:YES];
    [_condition lock]; report[@"touchBounds"] = @(_touchX == 0 && _touchY == 191); [_condition unlock];
    [self toggleMenu];
    [self touchAt:CGPointMake(256, 192) pressed:YES];
    [_condition lock]; report[@"sceneDoesNotTouchMenu"] = @(!_touchPressed && !_selectedEngine); [_condition unlock];
    [_condition lock]; _touchEngine=0; [_condition unlock];
    [self touchAt:CGPointMake(256,192) pressed:YES];
    [_condition lock]; report[@"originalMainTouchRouting"] = @(_touchPressed && !_selectedEngine); _touchEngine=1; [_condition unlock];
    [self pause];
    [_condition lock]; report[@"pauseClearsInput"] = @(!_keys && !_touchPressed && _paused); [_condition unlock];
    [self resume]; [self.view setNeedsLayout];
    [_buttons[@1] sendActionsForControlEvents:UIControlEventTouchDown];
    [_buttons[@1] sendActionsForControlEvents:UIControlEventTouchUpInside];
    [_condition lock];
    BOOL retainedTap=!_keys && [self takeKeysLocked]==1 && [self takeKeysLocked]==1;
    [self acknowledgeKeysLocked:++simulatedReads];
    report[@"shortTapDeliveredOnce"] = @(retainedTap && ![self takeKeysLocked]);
    [_condition unlock];
    [_buttons[@1] sendActionsForControlEvents:UIControlEventTouchDown]; [_buttons[@1] sendActionsForControlEvents:UIControlEventTouchUpInside];
    [_condition lock]; BOOL firstTap=[self takeKeysLocked]==1; [_condition unlock];
    [_buttons[@0x400] sendActionsForControlEvents:UIControlEventTouchDown]; [_buttons[@0x400] sendActionsForControlEvents:UIControlEventTouchUpInside];
    [_condition lock];
    [self acknowledgeKeysLocked:++simulatedReads]; BOOL nextTap=[self takeKeysLocked]==0x400;
    [self acknowledgeKeysLocked:++simulatedReads]; report[@"newKeySurvivesEarlierRead"] = @(firstTap && nextTap && ![self takeKeysLocked]);
    [_condition unlock];
    [_buttons[@1] sendActionsForControlEvents:UIControlEventTouchDown];
    [_buttons[@1] sendActionsForControlEvents:UIControlEventTouchCancel];
    [_condition lock]; report[@"cancelledTapClearsPending"] = @(![self takeKeysLocked]); [_condition unlock];
    _joystick.frame=CGRectMake(0,0,128,128);
    [_condition lock]; _fieldMapId=MAP_ROUTE_29; [_condition unlock];
    [self joystickAt:CGPointMake(68,64) pressed:YES];
    [_condition lock]; report[@"joystickDeadzone"] = @(![self takeKeysLocked]); [_condition unlock];
    const CGPoint directions[]={{64,0},{64,128},{0,64},{128,64}};
    const uint32_t directionKeys[]={0x40,0x80,0x20,0x10}; BOOL correctDirections=YES;
    for (NSUInteger i=0;i<4;i++) {
        [self joystickAt:directions[i] pressed:YES];
        [_condition lock]; correctDirections &= [self takeKeysLocked]==directionKeys[i]; [_condition unlock];
        [self resetJoystick];
    }
    report[@"joystickDirections"] = @(correctDirections);
    [self joystickAt:CGPointMake(400,-200) pressed:YES];
    CGFloat dx=_joystickThumb.center.x-64,dy=_joystickThumb.center.y-64;
    report[@"joystickThumbBounded"] = @(fabs(hypot(dx,dy)-40)<0.001);
    [_buttons[@2] sendActionsForControlEvents:UIControlEventTouchDown];
    [_condition lock]; report[@"joystickCombinesButtons"] = @([self takeKeysLocked]==0x12); [self acknowledgeKeysLocked:++simulatedReads]; [_condition unlock];
    [_buttons[@2] sendActionsForControlEvents:UIControlEventTouchUpInside];
    [_condition lock]; report[@"joystickIndependentRelease"] = @([self takeKeysLocked]==0x10); [_condition unlock];
    [self joystickAt:CGPointZero pressed:NO];
    [_condition lock]; report[@"joystickReleaseStops"] = @(![self takeKeysLocked]); [_condition unlock];
    [self joystickAt:CGPointMake(64,0) pressed:YES]; [self joystickAt:CGPointZero pressed:NO];
    [_condition lock];
    BOOL retainedFlick=[self takeKeysLocked]==0x40 && [self takeKeysLocked]==0x40;
    [self acknowledgeKeysLocked:++simulatedReads]; report[@"joystickFlickDeliveredOnce"] = @(retainedFlick && ![self takeKeysLocked]);
    [_condition unlock];
    [self joystickAt:CGPointMake(64,0) pressed:YES];
    [_condition lock]; BOOL previousDirection=[self takeKeysLocked]==0x40; [_condition unlock];
    [self joystickAt:CGPointMake(128,64) pressed:YES]; [self joystickAt:CGPointZero pressed:NO];
    [_condition lock];
    [self acknowledgeKeysLocked:++simulatedReads]; BOOL nextDirection=[self takeKeysLocked]==0x10;
    [self acknowledgeKeysLocked:++simulatedReads]; report[@"newJoystickDirectionSurvivesEarlierRead"] = @(previousDirection && nextDirection && ![self takeKeysLocked]);
    [_condition unlock];
    [self joystickAt:CGPointMake(0,64) pressed:YES]; [_joystick cancelTrackingWithEvent:nil];
    [_condition lock]; report[@"joystickCancelClearsPending"] = @(![self takeKeysLocked]); [_condition unlock];
    [self joystickAt:CGPointMake(64,128) pressed:YES]; [self joystickAt:CGPointMake(NAN,0) pressed:YES];
    [_condition lock]; report[@"joystickRejectsInvalidPoint"] = @(![self takeKeysLocked]); [_condition unlock];
    [self joystickAt:CGPointMake(128,64) pressed:YES]; [self toggleMenu];
    [_condition lock]; report[@"joystickMenuClearsInput"] = @(![self takeKeysLocked] && !_joystickKeys && !_pendingJoystickKeys); [_condition unlock];
    report[@"fieldMenuMetadata"] = @(_fieldMenuHitboxes.count==11 && _fieldMenuNames.count==13);
    [_condition lock];
    _fieldMenuIcons=(0x0DDDDDDDu & ~(15u<<4)) | (1u<<4);
    _fieldMenuFrame=[NSMutableData dataWithLength:256*192*4];
    [_condition unlock];
    [self.view layoutIfNeeded]; [self displayFrame];
    report[@"fieldMenuOverlay"] = @(_fieldMenuOpen && !_selectedEngine && !_fieldMenu.hidden && !_fieldMenuShade.hidden && !_canvas.userInteractionEnabled && _joystick.hidden);
    report[@"fieldMenuHidesLockedIcons"] = @(_fieldMenu.accessibilityElements.count==1 && [_fieldMenuElements[1].accessibilityLabel isEqual:_fieldMenuNames[1]]);
    BOOL menuTargets=YES;
    for (OriginalMenuElement *element in _fieldMenuElements) {
        CGRect rect=element.accessibilityFrameInContainerSpace;
        menuTargets &= rect.size.width>=44 && rect.size.height>=44;
    }
    report[@"fieldMenuMinimumTargets"] = @(menuTargets && CGRectContainsRect(self.view.bounds,_fieldMenu.frame));
    [self touchAt:CGPointMake(_fieldMenu.bounds.size.width/2,_fieldMenu.bounds.size.height/2) pressed:YES];
    [_condition lock]; report[@"fieldMenuTouchCoordinates"] = @(_touchPressed && _touchX==128 && _touchY==96 && !_selectedEngine); [_condition unlock];
    [self touchAt:CGPointZero pressed:NO];
    [self joystickAt:CGPointMake(128,64) pressed:YES];
    [_condition lock]; report[@"joystickIgnoresMenuInput"] = @(![self takeKeysLocked]); [_condition unlock];
    [self toggleMenu];
    [_condition lock];
    report[@"fieldMenuCloseUsesOriginalBack"] = @(!_fieldMenuOpen && [self takeKeysLocked]==2);
    [self acknowledgeKeysLocked:++simulatedReads]; _fieldMenuIcons=0x0DDDDDDD;
    [_condition unlock]; [self displayFrame];
    report[@"fieldMenuRestoresWorldControls"] = @(_fieldMenu.hidden && _fieldMenuShade.hidden && _canvas.userInteractionEnabled && !_joystick.hidden);
    [self joystickAt:CGPointMake(64,128) pressed:YES]; [self pause];
    [self joystickAt:CGPointMake(64,0) pressed:YES];
    [_condition lock]; report[@"joystickPauseClearsInput"] = @(!_joystickKeys && !_pendingJoystickKeys && ![self takeKeysLocked]); [_condition unlock];
    [self resume];
    [_condition lock]; report[@"joystickResumeDoesNotMove"] = @(![self takeKeysLocked]); _fieldMapId=_lastPadReadCount=0; [_condition unlock];
    NSMutableData *battleData = [NSMutableData dataWithLength:sizeof(HGNativeBattleUIState)];
    HGNativeBattleUIState *battle = battleData.mutableBytes;
    battle->active = 1; battle->revision = 7; battle->mode = HG_BATTLE_UI_COMMAND; battle->ready = 1; battle->enabledMask = 15 | 32;
    [_buttons[@0x20] sendActionsForControlEvents:UIControlEventTouchDown];
    [_condition lock]; _fieldMapId=MAP_ROUTE_29; [_condition unlock];
    [self joystickAt:CGPointMake(64,0) pressed:YES];
    [self updateBattleControls:[battleData copy]]; [self.view layoutIfNeeded];
    [_condition lock]; report[@"battleClearsHeldFieldInput"] = @(!_keys && !_pendingKeys && !_submittedKeys && !_joystickKeys && !_pendingJoystickKeys && !_submittedJoystickKeys && !_touchPressed); _fieldMapId=0; [_condition unlock];
    report[@"nativeCommandControls"] = @(!_battleBar.hidden && _battleBack.hidden && _battleButtons[0].enabled && _buttons[@1].hidden);
    report[@"nativeBattleMinimumTarget"] = @(_battleButtons[0].bounds.size.width >= 44 && _battleButtons[0].bounds.size.height >= 44);
    CGRect battleArea=UIEdgeInsetsInsetRect(self.view.bounds,self.view.safeAreaInsets);
    report[@"nativeBattleWideCanvas"] = @(_battleScene && _canvas.frame.origin.x==battleArea.origin.x && _canvas.frame.size.width==battleArea.size.width && _preferredBattleWidth>256 && _preferredBattleWidth<=1024);
    [_battleButtons[1] sendActionsForControlEvents:UIControlEventTouchUpInside];
    [_condition lock]; report[@"nativeBagQueued"] = @(_battleChoicePending && _battleChoiceRevision == 7 && _battleChoiceIndex == 1 && _selectedEngine == 1 && [_menuButton.currentTitle isEqualToString:@"返回场景"]); [_condition unlock];
    [self pause];
    [_condition lock]; report[@"pauseClearsBattleRequest"] = @(!_battleChoicePending); [_condition unlock];
    [self resume];
    [self updateBattleControls:[battleData copy]];
    report[@"nativeBallControl"] = @(!_battleBall.hidden && _battleBall.enabled && _battleBall.bounds.size.width >= 44 && _battleBall.bounds.size.height >= 44);
    [_battleBall sendActionsForControlEvents:UIControlEventTouchUpInside];
    [_condition lock]; report[@"nativeBallQueued"] = @(_battleChoicePending && _battleChoiceIndex == 5 && _selectedEngine == 1 && [_menuButton.currentTitle isEqualToString:@"返回场景"]); _battleChoicePending = NO; [_condition unlock];
    battle->mode = HG_BATTLE_UI_MOVES; battle->enabledMask = 17; battle->move[0] = 33; battle->pp[0] = 32; battle->maxPP[0] = 35;
    [self updateBattleControls:[battleData copy]];
    report[@"nativeMoveControls"] = @(!_battleBack.hidden && _battleBall.hidden && _battleButtons[0].enabled && !_battleButtons[1].enabled && [_battleButtons[0].accessibilityLabel containsString:@"Tackle"]);
    [_battleBack sendActionsForControlEvents:UIControlEventTouchUpInside];
    [_condition lock]; report[@"nativeMoveBackQueued"] = @(_battleChoicePending && _battleChoiceIndex == 4 && [_menuButton.currentTitle isEqualToString:@"触摸菜单"]); _battleChoicePending = NO; _selectedEngine = 0; [_condition unlock];
    battle->mode=HG_BATTLE_UI_TARGETS; battle->enabledMask=23;
    battle->targetSpecies[0]=163; battle->targetSpecies[1]=161; battle->targetSpecies[2]=152;
    battle->targetHp[0]=13; battle->targetMaxHp[0]=20; battle->targetMask[0]=3;
    [self updateBattleControls:[battleData copy]]; [self.view layoutIfNeeded];
    report[@"nativeTargetControls"] = @(!_battleBack.hidden && _battleBall.hidden && _battleButtons[0].enabled && !_battleButtons[3].enabled && _battleButtons[0].frame.origin.x>=CGRectGetMaxX(_battleBack.frame));
    report[@"nativeTargetAccessibility"] = @([_battleButtons[0].accessibilityLabel containsString:@"对方左侧"] && [_battleButtons[0].accessibilityLabel localizedCaseInsensitiveContainsString:@"Hoothoot"] && [_battleButtons[0].accessibilityLabel containsString:@"13/20"] && [_battleButtons[0].accessibilityHint isEqualToString:@"本招式影响 2 个目标"]);
    [_battleButtons[0] sendActionsForControlEvents:UIControlEventTouchUpInside];
    [_condition lock]; report[@"nativeTargetQueued"] = @(_battleChoicePending && _battleChoiceIndex==0 && !_selectedEngine); _battleChoicePending=NO; [_condition unlock];
    [self updateBattleControls:[battleData copy]]; [_battleBack sendActionsForControlEvents:UIControlEventTouchUpInside];
    [_condition lock]; report[@"nativeTargetCancelQueued"] = @(_battleChoicePending && _battleChoiceIndex==4); _battleChoicePending=NO; [_condition unlock];
    battle->mode = HG_BATTLE_UI_NONE; battle->ready = 0;
    [self updateBattleControls:[battleData copy]];
    report[@"nativeBattleKeepsSceneDuringAnimation"] = @(_battleScene && !_battleBar.hidden && !_battleButtons[0].enabled);
    [_condition lock]; _selectedEngine=1; [_condition unlock];
    [self updateBattleControls:[battleData copy]];
    report[@"nativeBattleSubscreenFallback"] = @(!_battleScene && _battleBar.hidden);
    [_condition lock]; _selectedEngine=0; _automaticMenuEngine=NO; [_condition unlock];
    battle->active=0; [self updateBattleControls:[battleData copy]];
    report[@"nativeBattleRestoresFieldControls"] = @(_battleBar.hidden && !_buttons[@1].hidden);
    [self checkBattleMenuControls:report];
    NSNotification *begin = [NSNotification notificationWithName:AVAudioSessionInterruptionNotification object:nil userInfo:@{AVAudioSessionInterruptionTypeKey:@(AVAudioSessionInterruptionTypeBegan)}];
    [self audioInterruption:begin]; [self resume];
    [_condition lock]; report[@"activationDoesNotOverrideInterruption"] = @(_paused && _interrupted); [_condition unlock];
    NSNotification *end = [NSNotification notificationWithName:AVAudioSessionInterruptionNotification object:nil userInfo:@{AVAudioSessionInterruptionTypeKey:@(AVAudioSessionInterruptionTypeEnded),AVAudioSessionInterruptionOptionKey:@0}];
    [self audioInterruption:end]; [self resume];
    [_condition lock]; report[@"interruptionWaitsForUser"] = @(_paused && _interruptionEnded); [_condition unlock];
    [_buttons[@8] sendActionsForControlEvents:UIControlEventTouchDown];
    [_condition lock]; report[@"userResumesWithoutSendingStart"] = @(!_paused && !_keys); [_condition unlock];
    BOOL activated=[_buttons[@1] accessibilityActivate];
    [_condition lock]; report[@"accessibilityPress"] = @(activated && (_keys&1)); [_condition unlock];
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW,200*NSEC_PER_MSEC),dispatch_get_main_queue(), ^{
        [self->_condition lock]; report[@"accessibilityRelease"] = @(!(self->_keys&1)); [self->_condition unlock];
    });
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, 8 * NSEC_PER_SEC), dispatch_get_main_queue(), ^{
        [self->_buttons[@8] sendActionsForControlEvents:UIControlEventTouchDown];
        dispatch_after(dispatch_time(DISPATCH_TIME_NOW, 150 * NSEC_PER_MSEC), dispatch_get_main_queue(), ^{ [self->_buttons[@8] sendActionsForControlEvents:UIControlEventTouchUpInside]; });
    });
    // Continue a private test save through the real controls, then verify its original 3D scene.
    for (NSUInteger delay = 12; delay <= 32; delay += 2) dispatch_after(dispatch_time(DISPATCH_TIME_NOW, delay * NSEC_PER_SEC), dispatch_get_main_queue(), ^{
        [self->_condition lock]; BOOL loaded = self->_fieldMapId != 0; [self->_condition unlock];
        if (loaded) return;
        [self->_buttons[@1] sendActionsForControlEvents:UIControlEventTouchDown];
        dispatch_after(dispatch_time(DISPATCH_TIME_NOW, 150 * NSEC_PER_MSEC), dispatch_get_main_queue(), ^{ [self->_buttons[@1] sendActionsForControlEvents:UIControlEventTouchUpInside]; });
    });
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, 36 * NSEC_PER_SEC), dispatch_get_main_queue(), ^{
        [self pause];
        dispatch_after(dispatch_time(DISPATCH_TIME_NOW, NSEC_PER_SEC), dispatch_get_main_queue(), ^{
            [self->_condition lock];
            uint32_t count = self->_publishedFrames;
            report[@"nativeFrames"] = @(count > 0);
            report[@"ownerSuspended"] = @(self->_suspended);
            NSData *geometry = self->_geometryState;
            uint32_t mapId = self->_fieldMapId;
            [self->_condition unlock];
            CGImageRef image = self->_canvas.image.CGImage;
            NSData *pixels = image ? CFBridgingRelease(CGDataProviderCopyData(CGImageGetDataProvider(image))) : nil;
            const uint8_t *bytes = pixels.bytes;
            BOOL varied = NO;
            for (NSUInteger i = 4; i + 4 <= pixels.length; i += 4) if (memcmp(bytes, bytes + i, 4)) { varied = YES; break; }
            const uint32_t *scene = geometry.bytes;
            report[@"originalWorldRendered"] = @(mapId == MAP_ROUTE_29 && varied && geometry.length == 8 * sizeof(uint32_t) && scene[0] && scene[1]);
            report[@"nativeWorldWide"] = @(mapId == MAP_ROUTE_29 && image && CGImageGetWidth(image)>256 && self->_displayedWidth==CGImageGetWidth(image));
            report[@"nativeWorldFillsView"] = @(CGRectEqualToRect(self->_canvas.frame,UIEdgeInsetsInsetRect(self.view.bounds,self.view.safeAreaInsets)));
            report[@"nativeJoystickVisible"] = @(!self->_joystick.hidden && self->_buttons[@0x40].hidden && self->_joystick.bounds.size.width>=44 && self->_joystick.accessibilityCustomActions.count==4);
            report[@"originalFieldMenuIconsPublished"] = @((self->_fieldMenuIcons&0x0FFFFFFF)!=0x0DDDDDDD);
            report[@"originalFieldTouchRouting"] = @(self->_touchEngine==1);
            NSURL *documents = [NSFileManager.defaultManager URLsForDirectory:NSDocumentDirectory inDomains:NSUserDomainMask].firstObject;
            [UIImagePNGRepresentation(self->_canvas.image) writeToURL:[documents URLByAppendingPathComponent:@"original-world-check.png"] options:NSDataWritingAtomic error:nil];
            NSMutableArray *sceneWords = [NSMutableArray new];
            for (NSUInteger i = 0; i < geometry.length / sizeof(uint32_t); i++) [sceneWords addObject:@(scene[i])];
            NSData *sceneJSON = [NSJSONSerialization dataWithJSONObject:@{@"mapId":@(mapId), @"width":@(image ? CGImageGetWidth(image) : 0), @"geometry":sceneWords, @"varied":@(varied), @"frames":@(count)} options:NSJSONWritingPrettyPrinted error:nil];
            [sceneJSON writeToURL:[documents URLByAppendingPathComponent:@"original-world-state.json"] options:NSDataWritingAtomic error:nil];
            [self->_condition lock]; NSDictionary *audio = self->_audioChanging ? nil : [self->_audio statistics]; [self->_condition unlock];
            NSDictionary *producerBeforePause=[self checkAudioProducerSnapshot];
            report[@"testAudioMuted"] = @([audio[@"muted"] boolValue]);
            report[@"audioSubmitted"] = @([audio[@"submitted"] unsignedLongLongValue] > 0);
            report[@"audioRendered"] = @([audio[@"rendered"] unsignedLongLongValue] > 0);
            report[@"originalAudioNonzero"] = @([audio[@"renderedNonzeroSamples"] unsignedLongLongValue] > 0);
            report[@"audioQueueBounded"] = @(audio && [audio[@"queued"] unsignedLongLongValue] <= 8192 && ![audio[@"dropped"] unsignedLongLongValue]);
            dispatch_after(dispatch_time(DISPATCH_TIME_NOW, NSEC_PER_SEC), dispatch_get_main_queue(), ^{
                [self->_condition lock]; report[@"pauseStopsFrames"] = @(self->_publishedFrames == count); [self->_condition unlock];
                [self->_condition lock]; NSDictionary *heldAudio = self->_audioChanging ? nil : [self->_audio statistics]; [self->_condition unlock];
                NSDictionary *producerWhilePaused=[self checkAudioProducerSnapshot];
                report[@"pauseStopsAudio"] = @([heldAudio[@"rendered"] isEqual:audio[@"rendered"]] && [heldAudio[@"submitted"] isEqual:audio[@"submitted"]]);
                [self resume]; [self toggleMenu];
                dispatch_after(dispatch_time(DISPATCH_TIME_NOW, 2 * NSEC_PER_SEC), dispatch_get_main_queue(), ^{
                    [self->_condition lock];
                    report[@"resumeFrames"] = @(self->_publishedFrames > count);
                    NSDictionary *resumedAudio = self->_audioChanging ? nil : [self->_audio statistics];
                    report[@"audioSnapshotsAvailable"] = @(audio && heldAudio && resumedAudio);
                    report[@"resumeAudio"] = @(resumedAudio && heldAudio && [resumedAudio[@"rendered"] unsignedLongLongValue] > [heldAudio[@"rendered"] unsignedLongLongValue] && [resumedAudio[@"submitted"] unsignedLongLongValue] > [heldAudio[@"submitted"] unsignedLongLongValue]);
                    report[@"resumeAudioQueueBounded"] = @([resumedAudio[@"queued"] unsignedLongLongValue] <= 8192 && [resumedAudio[@"dropped"] isEqual:heldAudio[@"dropped"]]);
                    report[@"resumeAudioWithoutUnderrun"] = @([resumedAudio[@"underrunFrames"] isEqual:heldAudio[@"underrunFrames"]]);
                    report[@"originalFieldMenuSameScreen"] = @(!self->_fieldMenu.hidden && self->_fieldMenu.image.size.width==256 && self->_fieldMenu.image.size.height==192 && self->_canvas.image.size.width>256 && !self->_selectedEngine && self->_fieldMenu.accessibilityElements.count>0);
                    uint32_t boots=self->_bootCount, resetFrames=self->_publishedFrames;
                    [self->_condition unlock];
                    NSDictionary *producerAfterResume=[self checkAudioProducerSnapshot];
                    NSURL *saveURL=[NSFileManager.defaultManager URLsForDirectory:NSApplicationSupportDirectory inDomains:NSUserDomainMask].firstObject;
                    saveURL=[saveURL URLByAppendingPathComponent:@"HeartGoldOriginal/heartgold.sav"];
                    NSData *saveBeforeReset=[NSData dataWithContentsOfURL:saveURL];
                    OriginalAudio *audioBeforeReset=self->_audio;
                    NSMutableArray *heldInputCompletions=[NSMutableArray new];
                    OriginalKeyButton *resetButton=(OriginalKeyButton *)self->_buttons[@2];
                    resetButton.heldInputCompletions=heldInputCompletions; self->_checkInputCompletions=heldInputCompletions;
                    [resetButton accessibilityActivate]; [self tapAt:CGPointZero];
                    resetButton.heldInputCompletions=nil; self->_checkInputCompletions=nil;
                    NSUInteger oldKeyGeneration=resetButton.inputGeneration, oldTouchGeneration=self->_inputGeneration;
                    [self->_condition lock]; self->_checkResetPending=YES; [self->_condition unlock];
                    __block NSUInteger attempts=30;
                    __block void (^verifyReset)(void);
                    verifyReset=^{
                        [self->_condition lock];
                        BOOL restarted=self->_bootCount==boots+1 && self->_bootFrameBase>=resetFrames && self->_publishedFrames>self->_bootFrameBase && !self->_finished && !self->_failure;
                        BOOL field=self->_fieldMapId==MAP_ROUTE_29, menu=self->_fieldMenuOpen;
                        BOOL wait=!self->_finished && !self->_failure && self->_bootCount<=boots+1;
                        NSDictionary *resetAudio=self->_audioChanging ? nil : [self->_audio statistics];
                        BOOL audioReplaced=self->_audio!=audioBeforeReset;
                        [self->_condition unlock];
                        BOOL audioReady=audioReplaced && [resetAudio[@"muted"] boolValue] &&
                            [resetAudio[@"submitted"] unsignedLongLongValue]>0 && [resetAudio[@"rendered"] unsignedLongLongValue]>0;
                        if ((!restarted || !audioReady || !field) && wait && --attempts) {
                            dispatch_after(dispatch_time(DISPATCH_TIME_NOW,NSEC_PER_SEC/2),dispatch_get_main_queue(),verifyReset);
                            return;
                        }
                        NSDictionary *producerAfterReset=[self checkAudioProducerSnapshot];
                        verifyReset=nil;
                        [self->_condition lock];
                        report[@"normalResetReboots"] = @(restarted);
                        report[@"normalResetBootParameter"] = @(self->_checkedBootParameter==1 && self->_checkedResetOccurred);
                        report[@"normalResetClearsInputAndSnapshots"] = @(self->_restartClean);
                        report[@"normalResetQuiescesCallbacks"] = @(self->_resetQuiesced);
                        report[@"normalResetInvalidatesQueuedInput"] = @(self->_inputGeneration!=oldTouchGeneration && resetButton.inputGeneration!=oldKeyGeneration && heldInputCompletions.count==2);
                        [self->_condition unlock];
                        report[@"normalResetContinuesSavedField"] = @(field);
                        if (field) {
                            if (!menu) [self toggleMenu];
                            [resetButton sendActionsForControlEvents:UIControlEventTouchDown];
                            [self touchAt:CGPointZero pressed:YES];
                            for (dispatch_block_t completion in heldInputCompletions) completion();
                            [self->_condition lock];
                            report[@"resetRejectsOldTouchRelease"] = @(self->_touchPressed);
                            report[@"resetRejectsOldKeyRelease"] = @((self->_keys&2)!=0);
                            [self->_condition unlock];
                            [self touchAt:CGPointZero pressed:NO];
                            [resetButton sendActionsForControlEvents:UIControlEventTouchUpInside];
                        } else {
                            report[@"resetRejectsOldTouchRelease"]=@NO; report[@"resetRejectsOldKeyRelease"]=@NO;
                        }
                        report[@"normalResetPreservesSave"] = @(saveBeforeReset && [saveBeforeReset isEqualToData:[NSData dataWithContentsOfURL:saveURL]]);
                        report[@"normalResetReopensMutedAudio"] = @(audioReady);
                        report[@"normalResetAudioStatisticsAvailable"] = @(resetAudio!=nil);
                        [self checkPartyFlow:report completion:^{
                         [self checkBagFlow:report completion:^{
                         [self checkSummaryFlow:report completion:^{
                         [self checkBattleMenusFlow:report completion:^{
                          report[@"actualBattleMenusPreserveSave"]=@(saveBeforeReset && [saveBeforeReset isEqualToData:[NSData dataWithContentsOfURL:saveURL]]);
                          [self->_condition lock]; NSDictionary *menusAudio=self->_audioChanging ? nil : [self->_audio statistics]; [self->_condition unlock];
                          NSDictionary *producerAfterMenus=[self checkAudioProducerSnapshot];
                          [self stop];
                          dispatch_after(dispatch_time(DISPATCH_TIME_NOW,2*NSEC_PER_SEC),dispatch_get_main_queue(), ^{
                            [self->_condition lock];
                            report[@"normalResetCleanup"] = @(self->_finished && !self->_env && !self->_game && !self->_wasi);
                            report[@"explicitStopDoesNotReboot"] = @(self->_finished && self->_bootCount==boots+1);
                            [self->_condition unlock];
                            NSArray *audioEpochs=@[audio ?: @{},heldAudio ?: @{},resumedAudio ?: @{},resetAudio ?: @{},menusAudio ?: @{}];
                            NSArray *producerEpochs=@[producerBeforePause,producerWhilePaused,producerAfterResume,producerAfterReset,producerAfterMenus];
                            NSArray *epochNames=@[@"beforePause",@"whilePaused",@"afterResume",@"afterReset",@"afterMenus"];
                            NSMutableArray *pendingFirstShort=[NSMutableArray new];
                            BOOL firstShortCaptureValid=YES, firstShortObserved=NO;
                            for (NSUInteger i=0;i<audioEpochs.count;i++) {
                                NSDictionary *consumer=audioEpochs[i], *producer=producerEpochs[i], *capture=producer[@"firstShortCapture"], *saved=capture[@"producer"];
                                uint64_t eventNs=[consumer[@"firstShortTimeNs"] unsignedLongLongValue];
                                if (!consumer[@"firstShortTimeNs"] || !capture) firstShortCaptureValid=NO;
                                uint64_t retainedNs=[capture[@"timeNs"] unsignedLongLongValue];
                                if (eventNs || retainedNs || [consumer[@"firstShortFrames"] unsignedLongLongValue]) firstShortObserved=YES;
                                if (!retainedNs) { if ([saved[@"available"] boolValue]) firstShortCaptureValid=NO; }
                                else if (![saved[@"available"] boolValue] || ![saved[@"bootEpoch"] isEqual:producer[@"bootEpoch"]]
                                    || [saved[@"bootStartNs"] unsignedLongLongValue]>retainedNs || [saved[@"lastServiceEndNs"] unsignedLongLongValue]<retainedNs
                                    || [saved[@"capturedAtNs"] unsignedLongLongValue]<retainedNs || [saved[@"capturedAtNs"] unsignedLongLongValue]>[producer[@"capturedAtNs"] unsignedLongLongValue]) firstShortCaptureValid=NO;
                                BOOL matched=NO;
                                for (NSDictionary *other in producerEpochs) if ([producer[@"bootEpoch"] isEqual:other[@"bootEpoch"]]) {
                                    NSDictionary *otherCapture=other[@"firstShortCapture"];
                                    uint64_t otherNs=[otherCapture[@"timeNs"] unsignedLongLongValue];
                                    if (eventNs && otherNs==eventNs) matched=YES;
                                    if (retainedNs && otherNs==retainedNs && ![capture isEqual:otherCapture]) firstShortCaptureValid=NO;
                                }
                                if ((eventNs || [consumer[@"firstShortFrames"] unsignedLongLongValue]) && !matched) [pendingFirstShort addObject:epochNames[i]];
                            }
                            report[@"audioFirstShortCaptureConsistent"]=@(firstShortCaptureValid);
                            BOOL passed=YES;
                            for (NSNumber *value in report.allValues) if (!value.boolValue) passed=NO;
                            report[@"passed"]=@(passed);
                            NSURL *folder=[NSFileManager.defaultManager URLsForDirectory:NSDocumentDirectory inDomains:NSUserDomainMask].firstObject;
                            NSData *audioData=[NSJSONSerialization dataWithJSONObject:@{@"beforePause":audio ?: @{},@"whilePaused":heldAudio ?: @{},@"afterResume":resumedAudio ?: @{},@"afterReset":resetAudio ?: @{},@"afterMenus":menusAudio ?: @{}} options:NSJSONWritingPrettyPrinted error:nil];
                            [audioData writeToURL:[folder URLByAppendingPathComponent:@"original-audio-metrics.json"] options:NSDataWritingAtomic error:nil];
                            NSData *producerData=[NSJSONSerialization dataWithJSONObject:@{@"version":@(HG_AUDIO_CHECK_VERSION),@"clock":@"CLOCK_MONOTONIC",
                                @"threadCpuClock":@"CLOCK_THREAD_CPUTIME_ID",
                                @"synthOutputCMeaning":@"Version 2+ SYNTH/OUTPUT record c is elapsed owner-thread CPU nanoseconds. Zero means unavailable or below resolution, not evidence of zero CPU work. Wall time includes scheduling and blocking.",
                                @"serviceGapCpuMeaning":@"Version 3 SERVICE_GAP CPU covers cpuWallStartNs (previous completed service end) through record endNs (current service entry), not the full start-to-start interval. Zero CPU or zero/invalid idle wall is unknown; non-CPU wall may include scheduling or blocking.",
                                @"costsMayOverlap":@YES,@"completedSpansOnly":@YES,
                                @"firstShortCaptureMeaning":@"First owner snapshot after observing the callback's first shortage and a completed audio service at or after it; short or overwritten spans can still be absent.",
                                @"firstShortCaptureObserved":@(firstShortObserved),@"firstShortCaptureComplete":@(pendingFirstShort.count==0),@"firstShortCapturePendingEpochs":pendingFirstShort,
                                @"appOtherMeaning":@"App setup, copies and publication; includes nested audio/SDK/FS work. A missing event does not exclude unmeasured work.",
                                @"sourceNames":@[@"none",@"hostRealtime",@"sdkPump",@"setOutput",@"detachOutput",@"modeChange"],
                                @"appSceneBits":@{@"field":@1,@"battle":@2,@"party":@4,@"bag":@8,@"summary":@16,@"subEngine":@32,@"battleBag":@64,@"battleParty":@128},
                                @"epochs":@{@"beforePause":producerBeforePause,@"whilePaused":producerWhilePaused,@"afterResume":producerAfterResume,
                                    @"afterReset":producerAfterReset,@"afterMenus":producerAfterMenus}} options:NSJSONWritingPrettyPrinted error:nil];
                            [producerData writeToURL:[folder URLByAppendingPathComponent:@"original-audio-producer-trace.json"] options:NSDataWritingAtomic error:nil];
                            NSData *data=[NSJSONSerialization dataWithJSONObject:report options:NSJSONWritingPrettyPrinted error:nil];
                            [data writeToURL:[folder URLByAppendingPathComponent:@"original-app-check.json"] options:NSDataWritingAtomic error:nil];
                            NSLog(@"Original app checks: %@",report);
                          });
                         }];
                         }];
                         }];
                        }];
                    };
                    dispatch_after(dispatch_time(DISPATCH_TIME_NOW,NSEC_PER_SEC/2),dispatch_get_main_queue(),verifyReset);
                });
            });
        });
    });
}
@end
