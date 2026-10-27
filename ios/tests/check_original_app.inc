// SPDX-License-Identifier: GPL-3.0-or-later
// Included only in the explicit UIKit self-check build; all input uses the real control handlers.
#include "constants/maps.h"
// ponytail: One CHECK controller/case per process; move this storage onto the controller for multiwindow checks.
static NSString *sPrivateUICase;
static NSMutableArray *sPrivatePublished, *sPrivateDisplayed, *sPrivateCaptures;
static NSMutableSet *sPrivateVisibleHP;
static BOOL sPrivateObserving, sPrivateOverflow;
static uint32_t sPrivateDisplayFrame;
static const NSUInteger sPrivateSampleLimit=1024;
#ifdef HEARTGOLD_ORIGINAL_CAPTURE_REGISTRATION_CHECK
static void (^sCaptureRegistrationDisplay)(uint32_t);
#endif
@implementation OriginalController (Checks)
#ifndef HEARTGOLD_ORIGINAL_CAPTURE_REGISTRATION_CHECK
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
    uint32_t oldFrameEngine=_frameEngine,oldDisplayedEngine=_displayedEngine;
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
    HGNativeBattlePartyUIState beforeForcedFixture=party;
    party.mode=HG_BATTLE_PARTY_UI_SWITCH_NO_CANCEL; party.phase=HG_BATTLE_PARTY_UI_ROSTER; party.page=0;
    party.revision=35; party.buttonCount=7; party.enabledMask=1; party.itemId=party.pocket=0;
    for (NSUInteger i=0;i<HG_BATTLE_PARTY_UI_BUTTONS;i++) party.buttons[i]=(HGNativeBattlePartyUIButton){(uint32_t)i*24+7,7,(uint32_t)i*24,0,16,16};
    _battlePartyUIState=[NSData dataWithBytes:&party length:sizeof(party)]; [self displayFrame]; [self viewDidLayoutSubviews];
    report[@"battleForcedRosterDisablesCancel"]=@(_battleMenuKind==OriginalBattleMenuParty && !_battleMenuPanel.hidden
        && _battleMenuButtons[0].enabled && !_battleMenuButtons[6].enabled && _buttons[@1].hidden && _buttons[@2].hidden);
    HGNativeBattlePartyUIState forgedCancel=party; forgedCancel.enabledMask|=1u<<6;
    BOOL forcedCancelRejected=!HGNativeBattlePartyUIResolveTouch(&forgedCancel,party.revision,party.phase,6,&x,&y);
    [_condition lock];
    _battleMenuChoicePending=YES; _battleMenuChoiceKind=OriginalBattleMenuParty; _battleMenuChoiceRevision=party.revision;
    _battleMenuChoicePhase=party.phase; _battleMenuChoiceIndex=6; pressed=NO;
    [self takeBattleMenuTouchLocked:&emptyBag party:&forgedCancel reads:140 x:&x y:&y pressed:&pressed];
    report[@"battleForcedRejectsForgedCancel"]=@(forcedCancelRejected && !pressed && !_battleMenuChoicePending && !_battleMenuTouchActive);
    [_condition unlock];
    party.phase=HG_BATTLE_PARTY_UI_MON_MENU; party.page=1; party.revision++; party.buttonCount=4; party.enabledMask=15;
    _battlePartyUIState=[NSData dataWithBytes:&party length:sizeof(party)]; [self displayFrame]; [self viewDidLayoutSubviews];
    BOOL forcedMonBack=_battleMenuKind==OriginalBattleMenuParty && _battleMenuButtons[3].enabled
        && HGNativeBattlePartyUIResolveTouch(&party,party.revision,party.phase,3,&x,&y);
    [_battleMenuButtons[3] sendActionsForControlEvents:UIControlEventTouchUpInside];
    [_condition lock]; pressed=NO;
    [self takeBattleMenuTouchLocked:&emptyBag party:&party reads:150 x:&x y:&y pressed:&pressed];
    forcedMonBack&=pressed && x==party.buttons[3].hitX && y==party.buttons[3].hitY;
    [self takeBattleMenuTouchLocked:&emptyBag party:&party reads:154 x:&x y:&y pressed:&pressed];
    [self takeBattleMenuTouchLocked:&emptyBag party:&party reads:156 x:&x y:&y pressed:&pressed];
    report[@"battleForcedMonMenuBackIsValid"]=@(forcedMonBack && !pressed && !_battleMenuChoicePending && !_battleMenuTouchActive && !_battleMenuTouchCooling);
    [_condition unlock];
    party.phase=HG_BATTLE_PARTY_UI_SUMMARY; party.page=2; party.revision++; party.enabledMask=12;
    _battlePartyUIState=[NSData dataWithBytes:&party length:sizeof(party)]; [self displayFrame]; [self viewDidLayoutSubviews];
    report[@"battleForcedInfoBackIsValid"]=@(_battleMenuKind==OriginalBattleMenuParty && _battleMenuButtons[3].enabled
        && HGNativeBattlePartyUIResolveTouch(&party,party.revision,party.phase,3,&x,&y));
    party=beforeForcedFixture;
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
    party.mode=4; // Modes0..3 are supported; an unknown mode keeps the original canvas fallback.
    _battlePartyUIState=[NSData dataWithBytes:&party length:sizeof(party)];
    [_condition lock]; _selectedEngine=_frameEngine=_displayedEngine=1; [_condition unlock]; [self displayFrame];
    report[@"battleMenuUnsupportedModeFallback"]=@(!_battleMenuKind && _battleMenuPanel.hidden && !_canvas.hidden && _canvas.userInteractionEnabled && !_buttons[@1].hidden);
    party.mode=HG_BATTLE_PARTY_UI_USE_ITEM; party.itemId=18;
    report[@"battleMenuOtherItemContextFallback"]=@(OriginalBattleMenuKind(&emptyBag,&party)==0);
    _battleUIState=oldBattle; _battleBagUIState=zeroBag; _battlePartyUIState=zeroParty; _battleMenuPictures=nil;
    [_condition lock]; _selectedEngine=_frameEngine=_displayedEngine=0; _battleMenuChoicePending=_battleMenuTouchActive=_battleMenuTouchCooling=NO; [_condition unlock];
    HGNativeBattleUIState emptyBattle={0}; [self updateBattleControls:[NSData dataWithBytes:&emptyBattle length:sizeof(emptyBattle)]]; [self displayFrame];
    _battleBagUIState=oldBattleBag; _battlePartyUIState=oldBattleParty; _battleMenuPictures=oldPictures;
    _battleMenuOwnerPictures=oldOwnerPictures; _battleMenuOwnerKind=oldOwnerKind; _battleMenuOwnerRevision=oldOwnerRevision; _battleMenuOwnerPhase=oldOwnerPhase;
    _frameEngine=oldFrameEngine; _displayedEngine=oldDisplayedEngine;
}
- (void)checkOptionsFlow:(NSMutableDictionary *)report completion:(dispatch_block_t)completion {
    NSArray *keys=@[@"actualOptionsTwoLifetimes",@"actualOptionsSingleScreenCanvas",@"actualOptionsBusyBlocksButtons",@"actualOptionsRealButtonEvents",
        @"actualOptionsTextPreviewChanged",@"actualOptionsFramePreviewChanged",@"actualOptionsCancelRestoresFrame",@"actualOptionsCancelReturnsField",
        @"actualOptionsConfirmReturnsField",@"actualOptionsCapturesComplete",@"actualOptionsPreservesSave",@"actualOptionsCompleted"];
    for (NSString *key in keys) report[key]=@NO;
    NSArray *pngs=@[@"original-options-first-ready-check.png",@"original-options-text-speed-check.png",@"original-options-frame-preview-check.png",
        @"original-options-first-cancel-field-check.png",@"original-options-reopen-check.png",@"original-options-confirm-check.png",@"original-options-confirm-field-check.png",
        @"original-options-preview-baseline.png",@"original-options-preview-changed.png",@"original-options-preview-restored.png"];
    NSMutableSet *captured=[NSMutableSet new]; NSMutableArray *events=[NSMutableArray new];
    NSURL *folder=[NSFileManager.defaultManager URLsForDirectory:NSDocumentDirectory inDomains:NSUserDomainMask].firstObject;
    NSURL *save=[NSFileManager.defaultManager URLsForDirectory:NSApplicationSupportDirectory inDomains:NSUserDomainMask].firstObject;
    save=[[save URLByAppendingPathComponent:@"HeartGoldOriginal"] URLByAppendingPathComponent:@"heartgold.sav"];
    NSData *saveBefore=[NSData dataWithContentsOfURL:save];
    __block NSUInteger stage=0,polls=0,lastStage=NSNotFound,move=0,presses=0,busySamples=0,stableFrames=0;
    __block uint32_t firstGeneration=0,secondGeneration=0,steadyGeneration=0,steadyRevision=0;
    __block BOOL canvasOK=YES,busyOK=YES,buttonsOK=YES;
    __block NSData *baselineBorder=nil,*baselineMain=nil,*lastBorder=nil;
    __block UIImage *lastCanvas=nil;
    __block CFAbsoluteTime steadySince=0;
    CFAbsoluteTime deadline=CFAbsoluteTimeGetCurrent()+90;
    NSData *(^border)(UIImage *)=^NSData *(UIImage *image) {
        CGImageRef cg=image.CGImage;
        if (!cg || CGImageGetWidth(cg)!=256 || CGImageGetHeight(cg)!=48 || CGImageGetBitsPerPixel(cg)!=32 || CGImageGetBitsPerComponent(cg)!=8) return nil;
        size_t stride=CGImageGetBytesPerRow(cg); NSData *pixels=CFBridgingRelease(CGDataProviderCopyData(CGImageGetDataProvider(cg)));
        if (stride<256*4 || stride>pixels.length/48) return nil;
        NSMutableData *result=[NSMutableData new]; const uint8_t *p=pixels.bytes;
        // Full original frame2; exclude only its printer's inner window (16,8,216,32).
        for (NSUInteger y=0;y<48;y++) for (NSUInteger x=0;x<256;x++) if (x<16 || x>=232 || y<8 || y>=40) [result appendBytes:p+y*stride+x*4 length:4];
        return result;
    };
    BOOL (^shot)(NSString *,UIImage *)=^BOOL(NSString *name,UIImage *image) {
        BOOL ok=[self checkPrivateCapture:name kind:@"actual-options" image:image];
        report[@"actualCapturesComplete"]=@([report[@"actualCapturesComplete"] boolValue] && ok);
        if (ok) [captured addObject:name]; return ok;
    };
    __block void (^step)(void);
    void (^finish)(NSString *)=^(NSString *status) {
        BOOL files=[captured isEqualToSet:[NSSet setWithArray:pngs]];
        for (NSString *name in pngs) {UIImage *image=[UIImage imageWithContentsOfFile:[folder URLByAppendingPathComponent:name].path];files&=image.CGImage!=nil;}
        report[@"actualOptionsTwoLifetimes"]=@(firstGeneration && secondGeneration==(firstGeneration==UINT32_MAX ? 1u : firstGeneration+1));
        report[@"actualOptionsSingleScreenCanvas"]=@(firstGeneration && canvasOK);
        report[@"actualOptionsBusyBlocksButtons"]=@(busySamples && busyOK);
        report[@"actualOptionsRealButtonEvents"]=@(buttonsOK && presses==11);
        report[@"actualOptionsCapturesComplete"]=@(files);
        report[@"actualOptionsPreservesSave"]=@(saveBefore && [saveBefore isEqualToData:[NSData dataWithContentsOfURL:save]]);
        report[@"actualOptionsCompleted"]=@([status isEqual:@"completed"]);
        NSDictionary *evidence=@{@"status":status,@"stage":@(stage),@"polls":@(polls),@"firstGeneration":@(firstGeneration),@"secondGeneration":@(secondGeneration),
            @"buttonEvents":@(presses),@"busySamples":@(busySamples),@"captures":[captured allObjects],@"events":events,
            @"diskPersistenceValidated":@NO,@"audibleAudioValidated":@NO,@"physicalGesturesValidated":@NO};
        [[NSJSONSerialization dataWithJSONObject:evidence options:NSJSONWritingPrettyPrinted error:nil] writeToURL:[folder URLByAppendingPathComponent:@"original-options-flow-qa.json"] options:NSDataWritingAtomic error:nil];
        // The existing completion chain takes afterMenus snapshots, stops, waits and writes its report on every outcome.
        step=nil; completion();
    };
    BOOL (^press)(uint32_t)=^BOOL(uint32_t key) {
        OriginalKeyButton *button=(OriginalKeyButton *)self->_buttons[@(key)];
        NSData *shownData=self->_displayedOptionsUIState; const HGNativeOptionsUIState *shown=shownData.bytes;
        if (![button isKindOfClass:OriginalKeyButton.class] || button.hidden || !button.enabled || button.window!=self.view.window
            || !button.window || ![button isDescendantOfView:self.view] || shownData.length!=sizeof(*shown) || !HGNativeOptionsUIInputAllowed(shown,shown->ownerGeneration,shown->revision)) return NO;
        BOOL activated=[button accessibilityActivate];
        [self->_condition lock];
        BOOL queued=self->_optionsChoicePending && self->_optionsChoice.ownerGeneration==shown->ownerGeneration && self->_optionsChoice.revision==shown->revision && self->_optionsChoiceKeys==key;
        BOOL admitted=self->_optionsInputActive && self->_optionsInput.ownerGeneration==shown->ownerGeneration && self->_optionsInput.revision==shown->revision && self->_optionsInputKeys==key;
        [self->_condition unlock];
        BOOL ok=activated && (queued || admitted); buttonsOK&=ok; if (ok) presses++; return ok;
    };
    void (^resetSteady)(void)=^{steadySince=0;stableFrames=0;lastBorder=nil;lastCanvas=nil;};
    step=^{
        [self.view layoutIfNeeded];
        NSData *data=self->_displayedOptionsUIState; const HGNativeOptionsUIState *p=data.bytes;
        BOOL valid=data.length==sizeof(*p),active=valid && p->active==1;
#ifdef HEARTGOLD_ORIGINAL_DEFAULT_CHECK
        NSData *displayedQA=self->_displayedBattleMenusQAState;
        const HGNativeBattleMenusQAState *q=displayedQA.length==sizeof(*q) ? displayedQA.bytes : NULL;
#endif
        [self->_condition lock]; BOOL failed=self->_failure!=nil || self->_finished;
        uint32_t map=self->_fieldMapId; BOOL idle=!self->_optionsChoicePending && !self->_optionsInputActive && !self->_optionsInputCooling;
#ifdef HEARTGOLD_ORIGINAL_DEFAULT_CHECK
        NSData *geometry=self->_geometryState;
#endif
        BOOL menu=self->_fieldMenuOpen,paused=self->_paused; [self->_condition unlock];
        if (failed || ++polls>900 || CFAbsoluteTimeGetCurrent()>deadline) {finish(failed ? @"engine_stopped" : @"options_flow_timeout");return;}
        if (lastStage!=stage) {
            if (events.count<128) [events addObject:@{@"stage":@(stage),@"generation":@(active ? p->ownerGeneration : 0),@"revision":@(active ? p->revision : 0),@"ready":@(active ? p->ready : 0),@"map":@(map)}];
            lastStage=stage;
        }
        BOOL native=active && self->_optionsScene && !self->_displayedEngine && self->_displayedWidth==256
            && self->_canvas.image.size.width==256 && self->_canvas.image.size.height==192 && !self->_canvas.hidden && !self->_canvas.userInteractionEnabled
            && self->_optionsPreview.image.size.width==256 && self->_optionsPreview.image.size.height==48 && !self->_optionsPreview.hidden && !self->_optionsPreview.userInteractionEnabled
            && fabs(self->_canvas.bounds.size.width*3-self->_canvas.bounds.size.height*4)<1 && CGRectGetMaxY(self->_canvas.frame)<=CGRectGetMinY(self->_optionsPreview.frame)
            && self->_menuButton.hidden && self->_fieldMenu.hidden && self->_joystick.hidden && self->_partyPanel.hidden && self->_bagPanel.hidden && self->_battleBar.hidden;
        if (active) {
            canvasOK&=native;
            if (!p->ready) {busySamples++;for (NSNumber *key in @[@0x40,@0x80,@0x20,@0x10,@1,@2]) busyOK&=!self->_buttons[key].enabled;}
        }
        NSData *edge=native ? border(self->_optionsPreview.image) : nil;
        BOOL ready=native && !paused && p->ready==1 && idle && edge && [self checkPrivatePixels:self->_canvas.image] && [self checkPrivatePixels:self->_optionsPreview.image];
        if (!ready || steadyGeneration!=p->ownerGeneration || steadyRevision!=p->revision || ![lastBorder isEqualToData:edge]) {
            steadySince=ready ? CFAbsoluteTimeGetCurrent() : 0; stableFrames=0; steadyGeneration=active ? p->ownerGeneration : 0; steadyRevision=active ? p->revision : 0;
        }
        if (ready && lastCanvas!=self->_canvas.image) stableFrames++;
        lastCanvas=self->_canvas.image; lastBorder=edge;
        BOOL steady=ready && stableFrames>=3 && CFAbsoluteTimeGetCurrent()-steadySince>=0.5;
        BOOL field=valid && !active && map==MAP_ROUTE_29 && !self->_optionsScene && self->_optionsPreview.hidden && !self->_displayedEngine
            && !self->_battleMode && !self->_battleScene && !self->_battleMenuScene && !self->_partyScene && !self->_bagScene && !self->_summaryScene
            && !self->_canvas.hidden && [self checkPrivatePixels:self->_canvas.image] && !self->_menuButton.hidden && self->_menuButton.enabled && self->_menuButton.window
            && (menu ? !self->_fieldMenu.hidden && self->_fieldMenu.image!=nil : !self->_joystick.hidden);
        if ((stage==0 || stage==6) && field) {
            if (!menu) {
                UIButton *button=self->_menuButton;
                if (!button.hidden && button.enabled && button.window && [button isDescendantOfView:self.view]) [button sendActionsForControlEvents:UIControlEventTouchUpInside];
            } else if (!self->_fieldMenu.hidden && self->_fieldMenu.window && self->_fieldMenu.userInteractionEnabled) {
                for (NSUInteger i=0;i<MIN((NSUInteger)7,self->_fieldMenuElements.count);i++) if (((self->_fieldMenuIcons>>(i*4))&15)==6
                    && [self->_fieldMenu.accessibilityElements containsObject:self->_fieldMenuElements[i]]) {
                    if ([self->_fieldMenuElements[i] accessibilityActivate]) {stage=stage==0 ? 1 : 7;resetSteady();} break;
                }
            }
        } else if (stage==1 && steady) {
            firstGeneration=p->ownerGeneration;baselineBorder=edge;baselineMain=CFBridgingRelease(CGDataProviderCopyData(CGImageGetDataProvider(self->_canvas.image.CGImage)));
            if (!shot(@"original-options-first-ready-check.png",nil) || !shot(@"original-options-preview-baseline.png",self->_optionsPreview.image)) {finish(@"first_capture_failed");return;}
            if (!press(0x10)) {finish(@"text_right_not_admitted");return;}stage=2;resetSteady();
        } else if (stage==2 && steady) {
            NSData *main=CFBridgingRelease(CGDataProviderCopyData(CGImageGetDataProvider(self->_canvas.image.CGImage)));
            report[@"actualOptionsTextPreviewChanged"]=@(baselineMain && ![baselineMain isEqualToData:main]);
            if (!shot(@"original-options-text-speed-check.png",nil)) {finish(@"text_capture_failed");return;}stage=3;move=0;
        } else if (stage==3 && steady) {
            if (move<5) {if (!press(0x80)) {finish(@"down_not_admitted");return;}move++;resetSteady();}
            else {if (!press(0x10)) {finish(@"frame_right_not_admitted");return;}stage=4;resetSteady();}
        } else if (stage==4 && steady) {
            report[@"actualOptionsFramePreviewChanged"]=@(baselineBorder && ![baselineBorder isEqualToData:edge]);
            if (![report[@"actualOptionsFramePreviewChanged"] boolValue]) {finish(@"frame_preview_did_not_change");return;}
            if (!shot(@"original-options-frame-preview-check.png",nil) || !shot(@"original-options-preview-changed.png",self->_optionsPreview.image)) {finish(@"frame_capture_failed");return;}
            if (!press(2)) {finish(@"cancel_not_admitted");return;}stage=5;resetSteady();
        } else if ((stage==5 || stage==10) && field) {
#ifdef HEARTGOLD_ORIGINAL_DEFAULT_CHECK
            const uint32_t *scene=geometry.bytes;
            if (menu) {
                UIButton *button=self->_menuButton;
                if (self->_displayedPaletteFadeFinished && !button.hidden && button.enabled && button.userInteractionEnabled
                    && button.window && button.window==self.view.window && [button isDescendantOfView:self.view])
                    [button sendActionsForControlEvents:UIControlEventTouchUpInside];
            } else if (self->_displayedPaletteFadeFinished && q && q->version==HG_BATTLE_MENUS_QA_VERSION && q->byteSize==sizeof(*q) && q->fieldReady==1
                && self->_fieldMenu.hidden && !self->_joystick.hidden && geometry.length==8*sizeof(uint32_t) && scene[0] && scene[1]) {
#else
            {
#endif
                BOOL cancelled=stage==5;
                NSString *name=cancelled ? @"original-options-first-cancel-field-check.png" : @"original-options-confirm-field-check.png";
                report[cancelled ? @"actualOptionsCancelReturnsField" : @"actualOptionsConfirmReturnsField"]=@YES;
                if (!shot(name,nil)) {finish(cancelled ? @"cancel_field_capture_failed" : @"confirm_field_capture_failed");return;}
#ifdef HEARTGOLD_ORIGINAL_DEFAULT_CHECK
                [events addObject:@{@"event":@"field-captured",@"stage":@(stage),@"capture":name,@"fieldFrame":@(q->fieldFrame),@"revision":@(q->revision)}];
#endif
                if (cancelled) stage=6; else {finish(@"completed");return;}
            }
        } else if (stage==7 && steady) {
            secondGeneration=p->ownerGeneration;
            report[@"actualOptionsCancelRestoresFrame"]=@(baselineBorder && [baselineBorder isEqualToData:edge]);
            if (![report[@"actualOptionsCancelRestoresFrame"] boolValue]) {finish(@"cancel_did_not_restore_frame");return;}
            if (!shot(@"original-options-reopen-check.png",nil) || !shot(@"original-options-preview-restored.png",self->_optionsPreview.image)) {finish(@"reopen_capture_failed");return;}
            // Original new owner starts on row0: Up wraps to footer6; Left selects Confirm1.
            if (!press(0x40)) {finish(@"footer_up_not_admitted");return;}stage=8;resetSteady();
        } else if (stage==8 && steady) {
            if (!press(0x20)) {finish(@"footer_confirm_left_not_admitted");return;}stage=9;resetSteady();
        } else if (stage==9 && steady) {
            if (!shot(@"original-options-confirm-check.png",nil)) {finish(@"confirm_capture_failed");return;}
            if (!press(1)) {finish(@"confirm_a_not_admitted");return;}stage=10;resetSteady();
        }
        dispatch_after(dispatch_time(DISPATCH_TIME_NOW,NSEC_PER_SEC/10),dispatch_get_main_queue(),step);
    };
    step();
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
#endif
- (BOOL)checkPrivatePixels:(UIImage *)image {
    if (!image.CGImage) return NO;
    NSData *bytes=CFBridgingRelease(CGDataProviderCopyData(CGImageGetDataProvider(image.CGImage)));
    if (bytes.length<8) return NO;
    const uint8_t *p=bytes.bytes;
    for (NSUInteger i=4;i+4<=bytes.length;i+=4) if (memcmp(p,p+i,4)) return YES;
    return NO;
}
- (BOOL)checkPrivateCapture:(NSString *)name kind:(NSString *)kind image:(UIImage *)source {
    UIImage *image=source; BOOL drawn=source!=nil;
    if (!source) {
        [self.view layoutIfNeeded]; UIGraphicsBeginImageContextWithOptions(self.view.bounds.size,NO,0);
        drawn=[self.view drawViewHierarchyInRect:self.view.bounds afterScreenUpdates:YES];
        image=UIGraphicsGetImageFromCurrentImageContext(); UIGraphicsEndImageContext();
    }
    NSURL *folder=[NSFileManager.defaultManager URLsForDirectory:NSDocumentDirectory inDomains:NSUserDomainMask].firstObject;
    NSData *png=image ? UIImagePNGRepresentation(image) : nil;
    BOOL written=png && [png writeToURL:[folder URLByAppendingPathComponent:name] options:NSDataWritingAtomic error:nil];
    [sPrivateCaptures addObject:@{@"file":name,@"kind":kind,@"drawn":@(drawn),@"written":@(written),@"displayFrame":@(sPrivateDisplayFrame)}];
    return drawn && written;
}
#ifndef HEARTGOLD_ORIGINAL_CAPTURE_REGISTRATION_CHECK
- (void)checkPrivatePartyPublished:(NSData *)data pictures:(NSDictionary *)pictures frame:(uint32_t)frame {
    [_condition lock]; BOOL observing=sPrivateObserving && [sPrivateUICase isEqual:@"field-potion"]; [_condition unlock];
    if (!observing || data.length!=sizeof(HGNativePartyUIState)) return;
    const HGNativePartyUIState *state=data.bytes;
    if (!HGNativePartyUIIsPotionUse(state)) return;
    NSArray *cards=pictures[@"cards"];
    id card=state->mons[0].nickname.pixelBuffer && cards.count && [cards[0] isKindOfClass:UIImage.class] ? cards[0] : NSNull.null;
    id message=state->message.pixelBuffer && [pictures[@"message"] isKindOfClass:UIImage.class] ? pictures[@"message"] : NSNull.null;
    NSDictionary *meta=@{@"frame":@(frame),@"revision":@(state->revision),@"phase":@(state->phase),@"hp":@(state->mons[0].hp),
        @"selected":@(state->selected),@"ready":@(state->ready),@"cardLease":@(state->mons[0].nickname.pixelBuffer!=0),
        @"messageWidth":@(state->message.tileWidth),@"messageHeight":@(state->message.tileHeight)};
    [_condition lock];
    if (sPrivateObserving) {
        if (sPrivatePublished.count<sPrivateSampleLimit) [sPrivatePublished addObject:@{@"meta":meta,@"card":card,@"message":message}];
        else sPrivateOverflow=YES;
    }
    [_condition unlock];
}
- (void)checkPrivateCaseDisplay:(uint32_t)frame {
    if (!sPrivateUICase) return;
    sPrivateDisplayFrame=frame;
#ifdef HEARTGOLD_ORIGINAL_POKEDEX_CHECK
    [self checkPokedexDisplay:frame];
#endif
    const HGNativePartyUIState *state=_displayedPartyUIState.length==sizeof(*state) ? _displayedPartyUIState.bytes : NULL;
    if (![sPrivateUICase isEqual:@"field-potion"] || !HGNativePartyUIIsPotionUse(state)) return;
    NSUInteger mons=0,actions=0;
    for (UIButton *button in _partyMons) if (button.enabled) mons++;
    for (UIButton *button in _partyActions) if (!button.hidden || button.enabled) actions++;
    NSDictionary *sample=@{@"frame":@(frame),@"revision":@(state->revision),@"phase":@(state->phase),@"hp":@(state->mons[0].hp),
        @"selected":@(state->selected),@"ready":@(state->ready),@"monButtonsEnabled":@(mons),@"actionsVisibleOrEnabled":@(actions),
        @"backEnabled":@(_partyBack.enabled),@"rawABHidden":@(_buttons[@1].hidden && _buttons[@2].hidden),
        @"canvasBlocked":@(_canvas.hidden && !_canvas.userInteractionEnabled)};
    [_condition lock]; BOOL observing=sPrivateObserving;
    if (observing) { if (sPrivateDisplayed.count<sPrivateSampleLimit) [sPrivateDisplayed addObject:sample]; else sPrivateOverflow=YES; }
    [_condition unlock];
    if (observing && state->phase==HG_PARTY_UI_ITEM_USE && state->mons[0].nickname.pixelBuffer && _partyCards[0].image
        && ![sPrivateVisibleHP containsObject:@(state->mons[0].hp)]) {
        [sPrivateVisibleHP addObject:@(state->mons[0].hp)];
        [self checkPrivateCapture:[NSString stringWithFormat:@"private-field-visible-hp-%u.png",state->mons[0].hp] kind:@"UIKit-displayed-HP" image:nil];
    }
}
- (void)checkPrivateNativeCase:(NSString *)kind {
    BOOL forced=[kind isEqual:@"forced-switch"];
    BOOL learn=[kind isEqual:@"learn-moves"];
    [_condition lock];
    sPrivateUICase=[kind copy]; sPrivatePublished=[NSMutableArray new]; sPrivateDisplayed=[NSMutableArray new];
    sPrivateCaptures=[NSMutableArray new]; sPrivateVisibleHP=[NSMutableSet new]; sPrivateObserving=YES; sPrivateOverflow=NO;
    [_condition unlock];
    NSArray *requiredKeys=learn
        ? @[@"caseBinding",@"muted",@"originalPrerequisite",@"originalVineWhip",@"originalStatAdvances",@"originalExpLevelUp",@"autoStockpile",
            @"promptRawInputBlocked",@"keepContinue",@"nativeLearnList",@"infoPreservesBattle",@"hmRejected",@"fifthCancelled",@"continueAfterFifth",
            @"replacementOriginalPP",@"finalRefusal",@"savedPartyPreserved",@"learnRejectsStale",@"resetBeforeSettlement",@"resetQuiescent",@"capturesComplete",@"samplesWithinBound"]
        : forced
        ? @[@"caseBinding",@"muted",@"originalPrerequisite",@"nextMonPrompt",@"naturalKO",@"nativeRoster",@"typedCancelRejected",@"originalCancelRejected",
            @"faintedShiftAck",@"eggInfoDisabled",@"eggShiftAck",@"reserveActive",@"resetBeforeSettlement",@"resetQuiescent",@"capturesComplete",@"samplesWithinBound"]
        : @[@"caseBinding",@"muted",@"originalPrerequisite",@"naturalDamage",@"runSettled",@"targetPrompt",@"targetCancelPreserves",@"reopenRejectsStale",
            @"hpRestored",@"hpPublicationObserved",@"phase5ReadOnly",@"successOriginalMessage",@"consumedOnePotion",@"fullHpNoEffect",
            @"noEffectOriginalMessage",@"noEffectConsumesNothing",@"continueExactlyOnceEach",@"returnedField",@"resetQuiescent",@"capturesComplete",@"samplesWithinBound"];
    NSArray *requiredPNGs=learn
        ? @[@"private-learn-stats-difference.png",@"private-learn-stats-total.png",@"private-learn-spit-up-keep.png",@"private-learn-spit-up-give-up.png",@"private-learn-spit-up-list.png",@"private-learn-cut-detail.png",
            @"private-learn-hm-rejection.png",@"private-learn-new-move-detail.png",@"private-learn-fifth-give-up.png",@"private-learn-spit-up-retry-list.png",
            @"private-learn-replace-vine-whip.png",@"private-learn-swallow-keep.png",@"private-learn-swallow-give-up.png",@"private-learn-complete.png"]
        : forced
        ? @[@"private-forced-next-mon-prompt.png",@"private-forced-switch-roster.png",@"private-forced-switch-cancel.png",@"private-forced-switch-fainted-menu.png",@"private-forced-switch-fainted-ack.png",
            @"private-forced-switch-egg-menu.png",@"private-forced-switch-egg-ack.png",@"private-forced-switch-reserve-menu.png",@"private-forced-switch-active.png"]
        : @[@"private-field-potion-bag.png",@"private-field-potion-target-cancel.png",@"private-field-potion-cancel-bag.png",@"private-field-potion-target-heal.png",
            @"private-field-potion-healed-result.png",@"private-field-potion-consumed-bag.png",@"private-field-potion-fullhp-target.png",
            @"private-field-potion-noeffect-result.png",@"private-field-potion-final-bag.png",@"private-field-potion-return.png"];
    NSMutableDictionary *checks=[NSMutableDictionary new]; for (NSString *key in requiredKeys) checks[key]=@NO;
    void (^record)(NSString *,BOOL)=^(NSString *key,BOOL value) { checks[key]=@(value); };
    NSMutableArray *events=[NSMutableArray new];
    __block NSUInteger stage=0,polls=0,growls=0,runs=0,waits=0,continues=0,lastStage=NSNotFound;
    __block uint32_t turnBefore=0,requestRevision=0,runRevision=0,bagRevision=0,targetRevision=0,beforeHP=0,rawCancelRevision=0,lastWaitFrame=0;
    __block BOOL finishing=NO; __block NSData *successMessage=nil;
    __block NSUInteger learnStatPulses=0,learnStale=0;
    __block uint32_t learnReleaseReads=0,learnRevision=0,learnRawPromptMask=0;
    __block BOOL learnReleaseStarted=NO;
    __block NSData *learnBefore=nil,*learnHMBody=nil;
    __block void (^step)(void);
    void (^finish)(NSString *)=^(NSString *status) {
        if (finishing) return; finishing=YES; step=nil;
        [self->_condition lock];
        sPrivateObserving=NO; NSArray *published=[sPrivatePublished copy],*displayed=[sPrivateDisplayed copy]; BOOL overflow=sPrivateOverflow;
        NSData *finalQA=[self->_battleMenusQAState copy]; uint32_t boots=self->_bootCount;
        self->_checkResetPending=YES; [self->_condition unlock];
        NSMutableArray *publicationMetadata=[NSMutableArray new]; NSMutableSet *publishedHP=[NSMutableSet new],*displayedHP=[NSMutableSet new];
        BOOL readonly=YES; NSUInteger phase5Displays=0;
        for (NSDictionary *row in published) { NSDictionary *meta=row[@"meta"]; [publicationMetadata addObject:meta];
            if ([meta[@"phase"] unsignedIntValue]==HG_PARTY_UI_ITEM_USE) [publishedHP addObject:meta[@"hp"]]; }
        for (NSDictionary *row in displayed) if ([row[@"phase"] unsignedIntValue]==HG_PARTY_UI_ITEM_USE) {
            phase5Displays++; [displayedHP addObject:row[@"hp"]];
            readonly&=![row[@"monButtonsEnabled"] unsignedIntegerValue] && ![row[@"actionsVisibleOrEnabled"] unsignedIntegerValue]
                && [row[@"rawABHidden"] boolValue] && [row[@"canvasBlocked"] boolValue]
                && (![row[@"backEnabled"] boolValue] || [row[@"ready"] boolValue]);
        }
        if (!forced && !learn) {
            record(@"hpPublicationObserved",publishedHP.count && [publishedHP containsObject:@20] && beforeHP>0 && beforeHP<20);
            record(@"phase5ReadOnly",phase5Displays && readonly);
        }
        record(@"samplesWithinBound",!overflow);
        __block NSUInteger resetPolls=0;
        __block void (^resetStep)(void);
        resetStep=^{
            [self->_condition lock];
            BOOL field=self->_bootCount==boots+1 && self->_fieldMapId==MAP_ROUTE_29 && self->_publishedFrames>self->_bootFrameBase;
            BOOL clean=self->_resetQuiesced && self->_restartClean;
            BOOL failed=self->_failure!=nil || self->_finished;
            NSDictionary *audio=self->_audioChanging ? nil : [self->_audio statistics];
            [self->_condition unlock];
            if (!field && !failed && ++resetPolls<200) { dispatch_after(dispatch_time(DISPATCH_TIME_NOW,NSEC_PER_SEC/10),dispatch_get_main_queue(),resetStep); return; }
            record(@"resetQuiescent",field && clean); record(@"muted",[checks[@"muted"] boolValue] && [audio[@"muted"] boolValue]);
            resetStep=nil; [self stop];
            __block NSUInteger stopPolls=0; __block void (^stopStep)(void);
            stopStep=^{
                [self->_condition lock]; BOOL stopped=self->_finished && !self->_env && !self->_game && !self->_wasi; [self->_condition unlock];
                if (!stopped && ++stopPolls<100) { dispatch_after(dispatch_time(DISPATCH_TIME_NOW,NSEC_PER_SEC/10),dispatch_get_main_queue(),stopStep); return; }
                record(@"resetQuiescent",[checks[@"resetQuiescent"] boolValue] && stopped);
                NSMutableSet *files=[NSMutableSet new]; BOOL captures=YES;
                for (NSDictionary *capture in sPrivateCaptures) { captures&=[capture[@"drawn"] boolValue] && [capture[@"written"] boolValue]; [files addObject:capture[@"file"]]; }
                for (NSString *file in requiredPNGs) captures&=[files containsObject:file];
                record(@"capturesComplete",captures);
                BOOL passed=[status isEqual:@"completed"]; for (NSString *key in requiredKeys) passed&=[checks[key] boolValue];
                NSMutableArray *words=[NSMutableArray new]; if (finalQA.length==sizeof(HGNativeBattleMenusQAState))
                    for (NSUInteger i=0;i<finalQA.length/4;i++) [words addObject:@(((const uint32_t *)finalQA.bytes)[i])];
                NSMutableDictionary *result=[@{@"version":@1,@"case":kind,@"complete":@(stopped),@"passed":@(passed),@"status":status,@"checks":checks,
                    @"requiredKeys":requiredKeys,@"requiredPNGs":requiredPNGs,@"captures":[sPrivateCaptures copy],@"events":events,@"qaWordsBeforeReset":words,
                    @"growlChoices":@(growls),@"runAttempts":@(runs),@"continues":@(continues),@"sampleLimit":@(sPrivateSampleLimit),@"sampleOverflow":@(overflow),
                    @"publishedSamples":publicationMetadata,@"displayedSamples":displayed,@"phase5PublishedHP":[publishedHP allObjects],@"phase5DisplayedHP":[displayedHP allObjects],
                    @"animationScope":@"Owner publications and actual 30Hz displays are separate. One HP means one observed update; missing intermediate display values are not a full animation proof.",
                    @"fixtureScope":learn ? @"Battle-owned Carnivine/Magikarp fixture; original EXP/learning and HM rejection. Reset before results copy; original saved party/EXP/Bag remain private-guest checks."
                        : forced ? @"Test-owned copied battle party and original-created stronger opponent; reset before results copy. Provided save still has one starter." : @"Original one-starter save; natural battle damage and flee, then original field Potion. Native CQA owns full-attribute/Bag proof.",
                    @"businessStatePatched":@NO,@"privateRawCancelDiagnostic":@(forced),@"privateNextMonRawInputDiagnostic":@(forced),@"newProductionAPI":@NO} mutableCopy];
                if (learn) [result addEntriesFromDictionary:@{@"learningStatPulses":@(learnStatPulses),@"learningRawPromptMask":@(learnRawPromptMask),
                    @"learningStaleRejects":@(learnStale),@"privateLearnRawInputDiagnostic":@YES}];
                NSURL *folder=[NSFileManager.defaultManager URLsForDirectory:NSDocumentDirectory inDomains:NSUserDomainMask].firstObject;
                NSString *file=learn ? @"private-learn-moves-check.json" : forced ? @"private-forced-switch-check.json" : @"private-field-potion-check.json";
                [[NSJSONSerialization dataWithJSONObject:result options:NSJSONWritingPrettyPrinted error:nil] writeToURL:[folder URLByAppendingPathComponent:file] options:NSDataWritingAtomic error:nil];
                stopStep=nil;
            }; stopStep();
        }; resetStep();
    };
    NSString *built=[NSBundle.mainBundle objectForInfoDictionaryKey:@"HGPrivateCheckCase"] ?: @"default";
    record(@"caseBinding",[built isEqual:kind] && [NSProcessInfo.processInfo.environment[@"HEARTGOLD_TEST_MUTE"] isEqual:@"1"]);
    if (![checks[@"caseBinding"] boolValue]) { finish(@"case_binding_mismatch"); return; }
    step=^{
        [self->_condition lock]; NSData *qaData=self->_battleMenusQAState; uint32_t map=self->_fieldMapId;
        BOOL failed=self->_failure!=nil || self->_finished,menuOpen=self->_fieldMenuOpen,overflow=sPrivateOverflow;
        NSDictionary *audio=self->_audioChanging ? nil : [self->_audio statistics]; [self->_condition unlock];
        const HGNativeBattleMenusQAState *q=qaData.length==sizeof(*q) ? qaData.bytes : NULL;
        const HGNativeBattleUIState *battle=self->_displayedBattleUIState.length==sizeof(*battle) ? self->_displayedBattleUIState.bytes : NULL;
        const HGNativeBattlePartyUIState *bp=self->_displayedBattlePartyUIState.length==sizeof(*bp) ? self->_displayedBattlePartyUIState.bytes : NULL;
        const HGNativePartyUIState *party=self->_displayedPartyUIState.length==sizeof(*party) ? self->_displayedPartyUIState.bytes : NULL;
        const HGNativeBagUIState *bag=self->_displayedBagUIState.length==sizeof(*bag) ? self->_displayedBagUIState.bytes : NULL;
        BOOL command=battle && battle->active && battle->ready && battle->mode==HG_BATTLE_UI_COMMAND;
        BOOL moves=battle && battle->active && battle->ready && battle->mode==HG_BATTLE_UI_MOVES;
        BOOL bpReady=bp && bp->active && bp->ready && self->_battleMenuKind==OriginalBattleMenuParty && !self->_battleMenuPanel.hidden;
        BOOL target=HGNativePartyUIIsPotionUse(party) && party->phase==HG_PARTY_UI_ITEM_TARGET && party->ready;
        BOOL bagReady=bag && bag->active && !bag->mode && bag->ready;
        if (failed || ++polls>3600 || (q && q->errorLine) || overflow) { finish(failed ? @"engine_failed" : q && q->errorLine ? @"guest_failed" : overflow ? @"sample_overflow" : @"flow_timeout"); return; }
        if (q && q->returned && (forced || learn || q->outcome!=5)) { finish(@"battle_ended_otherwise"); return; }
        if (learn && learnBefore.length==sizeof(*q) && q && ((stage>=302 && stage<=308) || stage==311 || stage==312)) {
            HGNativeBattleMenusQAState before=*(const HGNativeBattleMenusQAState *)learnBefore.bytes,after=*q;
            before.revision=after.revision=before.fieldFrame=after.fieldFrame=0;
            if (memcmp(&before,&after,sizeof(before))) { finish(@"learning_info_changed_battle_state"); return; }
        }
        if (lastStage!=stage) { [events addObject:@{@"stage":@(stage),@"displayFrame":@(sPrivateDisplayFrame),@"turns":@(q ? q->totalTurns : 0),
            @"fieldPhase":@(party ? party->phase : 0),@"battlePartyPhase":@(bp ? bp->phase : 0),@"battleSelectedSlot":@(bp ? bp->selectedSlot : 0),
            @"fieldHP":@(party ? party->mons[0].hp : 0)}];
            if (learn && q) [events addObject:@{@"stage":@(stage),@"displayFrame":@(sPrivateDisplayFrame),@"candidate":@(q->replyItem),@"exp":@(q->replyPocket),
                @"semanticsMask":@(q->semanticsMask),@"commandMode":@(battle ? battle->mode : 0),@"commandRevision":@(battle ? battle->revision : 0),
                @"partyPhase":@(bp ? bp->phase : 0),@"partyRevision":@(bp ? bp->revision : 0),@"selectedMove":@(bp ? bp->selectedMove : 0),
                @"replyCount":@(q->replyCount),@"replyTarget":@(q->replyTarget),@"hp":@(q->playerHp),@"enemyHp":@(q->enemyHp),
                @"moves":@[@(q->moves[0]),@(q->moves[1]),@(q->moves[2]),@(q->moves[3])],@"pp":@[@(q->pp[0]),@(q->pp[1]),@(q->pp[2]),@(q->pp[3])]}];
            lastStage=stage; }
        NSString *capture=nil; dispatch_block_t action=nil;
        if (stage==0 && q && q->fieldReady && map==MAP_ROUTE_29) {
            record(@"muted",[audio[@"muted"] boolValue]);
            if (menuOpen) [self toggleMenu]; stage=1;
        } else if (stage==0 && polls>=120 && !(polls%20) && !self->_buttons[@1].hidden) [self->_buttons[@1] accessibilityActivate];
        else if (stage==1 && q && q->fieldReady && !q->requested && !menuOpen && self->_fieldMenu.hidden) {
            [self->_condition lock]; self->_checkBattleMenusRequestRevision=q->revision; self->_checkBattleMenusRequestPending=YES; [self->_condition unlock];
            requestRevision=q->revision; stage=2;
        } else if (stage==2 && q && !q->requested && q->fieldReady && q->revision!=requestRevision) stage=1;
        else if (stage==2 && command && q && q->battleActive) {
            BOOL valid=learn ? q->semanticsMask==2063 && q->partyCount==1 && !q->activePartySlot && q->playerHp && q->playerHp==q->playerMaxHp
                && !q->playerStatus && q->potionQuantity==5 && q->enemySpecies==129 && q->enemyLevel==1 && q->enemyHp
                && q->moves[0]==22 && q->moves[1]==15 && q->moves[2]==74 && !q->moves[3] && q->pp[0]==24 && q->replyPocket==37237 && !q->replyCount
                : q->playerHp==20 && q->playerMaxHp==20 && q->potionQuantity==5 && q->moves[1]==45
                    && q->enemySpecies==(forced ? 19 : 10) && q->enemyLevel==(forced ? 20 : 1) && q->partyCount==(forced ? 3 : 1);
            record(@"originalPrerequisite",valid); if (!valid) { finish(@"original_prerequisite_failed"); return; } stage=3;
        } else if (stage==3 && command && self->_battleButtons[0].enabled) {
            turnBefore=q->totalTurns; action=^{ [self->_battleButtons[0] sendActionsForControlEvents:UIControlEventTouchUpInside]; }; stage=learn ? 300 : 4;
        } else if (stage==4 && moves && battle->move[1]==45 && battle->pp[1] && self->_battleButtons[1].enabled) {
            growls++; action=^{ [self->_battleButtons[1] sendActionsForControlEvents:UIControlEventTouchUpInside]; }; stage=5;
        } else if (stage==5 && forced && battle && battle->active && battle->ready && battle->mode==HG_BATTLE_UI_NEXT_MON) {
            if (++waits==1) {
                [self->_condition lock]; self->_keys|=1; self->_pendingKeys|=1; self->_touchX=128; self->_touchY=68; self->_touchPressed=YES; [self->_condition unlock];
            } else {
                [self->_condition lock]; BOOL rawCleared=!(self->_keys&3) && !(self->_pendingKeys&3) && !(self->_submittedKeys&3) && !self->_touchPressed; [self->_condition unlock];
                BOOL valid=q && !q->playerHp && battle->enabledMask==3 && battle->nameWidth[0] && battle->nameWidth[1]
                    && !self->_battleBar.hidden && !self->_battleButtons[0].hidden && !self->_battleButtons[1].hidden
                    && self->_battleButtons[0].enabled && self->_battleButtons[1].enabled && self->_battleButtons[2].hidden && self->_battleButtons[3].hidden
                    && self->_battleBack.hidden && self->_battleBall.hidden && self->_battleButtons[0].configuration.image && self->_battleButtons[1].configuration.image
                    && !self->_battleButtons[0].configuration.title.length && !self->_battleButtons[1].configuration.title.length && rawCleared;
                record(@"nextMonPrompt",valid); if (!valid) { finish(@"next_mon_prompt_failed"); return; }
                capture=@"private-forced-next-mon-prompt.png"; action=^{ [self->_battleButtons[0] sendActionsForControlEvents:UIControlEventTouchUpInside]; }; waits=0; stage=19;
            }
        } else if (stage==19 && forced && bpReady && bp->mode==HG_BATTLE_PARTY_UI_SWITCH_NO_CANCEL && bp->phase==HG_BATTLE_PARTY_UI_ROSTER) {
            record(@"naturalKO",q && !q->playerHp && growls>0 && growls<=12); stage=20;
        } else if (stage==5 && command && q && q->totalTurns>turnBefore) {
            if (!forced && q->playerHp && q->playerHp<20) { record(@"naturalDamage",YES); stage=6; }
            else if (q->playerHp && growls<12) stage=3;
            else { finish(forced ? @"natural_ko_not_observed" : @"natural_damage_not_observed"); return; }
        }
        if (learn) {
            [self->_condition lock];
            BOOL inputIdle=!self->_battleChoicePending && !self->_battleMenuChoicePending && !self->_battleMenuTouchActive && !self->_battleMenuTouchCooling;
            [self->_condition unlock];
            BOOL (^promptValid)(uint32_t,uint32_t)=^BOOL(uint32_t mode,uint32_t candidate) {
                if (!q || !battle || !battle->active || !battle->ready || battle->mode!=mode || q->replyItem!=candidate
                    || battle->enabledMask!=3 || battle->cancelRun || self->_battleBar.hidden || self->_battleMenuScene
                    || !self->_battleButtons[0].enabled || !self->_battleButtons[1].enabled || !self->_battleButtons[2].hidden
                    || !self->_battleButtons[3].hidden || !self->_battleBack.hidden || !self->_battleBall.hidden) return NO;
                for (NSUInteger i=0;i<4;i++) if (battle->move[i] || battle->pp[i] || battle->maxPP[i]) return NO;
                for (NSUInteger i=0;i<2;i++) if (!battle->nameWidth[i] || battle->nameWidth[i]>HG_BATTLE_NAME_WIDTH
                    || !self->_battleButtons[i].configuration.image || self->_battleButtons[i].configuration.title.length
                    || self->_battleButtons[i].hidden || ![self checkPrivatePixels:self->_battleButtons[i].configuration.image]) return NO;
                return YES;
            };
            BOOL (^partyValid)(uint32_t,uint32_t)=^BOOL(uint32_t phase,uint32_t selectedMove) {
                BOOL list=phase==HG_BATTLE_PARTY_UI_LEARN_LIST; uint32_t x=0,y=0;
                if (!q || !bpReady || bp->mode!=HG_BATTLE_PARTY_UI_LEARN_MOVE || bp->phase!=phase || bp->page!=(list ? 6 : 7)
                    || bp->partyCount!=1 || bp->selectedSlot || bp->mons[0].species!=455 || bp->mons[0].level!=31 || bp->mons[0].partyIndex
                    || bp->mons[0].isEgg || bp->itemId || bp->pocket || bp->moveToLearn!=255 || q->replyItem!=255 || q->replyPocket!=37239
                    || bp->buttonCount!=(list ? 7 : 3) || bp->enabledMask!=(list ? 0x5F : 5)
                    || !self->_canvas.hidden || !self->_battleMenuPreview.hidden || !self->_buttons[@1].hidden || !self->_buttons[@2].hidden
                    || !self->_battleMenuMessage.hidden || HGNativeBattlePartyUIResolveTouch(bp,bp->revision,phase,list ? 5 : 1,&x,&y)) return NO;
                for (NSUInteger i=0;i<4;i++) if (bp->moves[i].moveId!=q->moves[i] || bp->moves[i].pp!=q->pp[i]) return NO;
                if (list) {
                    if (bp->selectedMove!=selectedMove || !self->_battleMenuButtons[5].hidden || self->_battleMenuButtons[5].enabled || !self->_battleMenuButtons[6].enabled
                        || !self->_battleMenuDescription.hidden || !self->_battleMenuSelected.hidden) return NO;
                    for (NSUInteger i=0;i<5;i++) {
                        uint32_t move=i<4 ? bp->moves[i].moveId : bp->moveToLearn;
                        NSString *name=move<self->_moveNames.count ? self->_moveNames[move] : nil;
                        if (!self->_battleMenuButtons[i].enabled || self->_battleMenuButtons[i].hidden || !name.length
                            || ![self->_battleMenuButtons[i].accessibilityLabel containsString:name] || ![self checkPrivatePixels:self->_battleMenuImages[i].image]) return NO;
                    }
                } else if (bp->selectedMove!=selectedMove || !bp->message.pixelBuffer || bp->message.tileWidth!=15 || bp->message.tileHeight!=10
                    || bp->message.engine!=1 || self->_battleMenuDescription.hidden || !self->_battleMenuButtons[1].hidden
                    || self->_battleMenuButtons[1].enabled || !self->_battleMenuButtons[0].enabled || !self->_battleMenuButtons[2].enabled
                    || self->_battleMenuDescription.image.size.width!=256 || self->_battleMenuDescription.image.size.height!=152
                    || ![self checkPrivatePixels:self->_battleMenuDescription.image]) return NO;
                return YES;
            };
            BOOL prompt=battle && battle->active && battle->ready && (battle->mode==HG_BATTLE_UI_KEEP_FORGET || battle->mode==HG_BATTLE_UI_GIVE_UP);
            if (stage==300 && moves && battle->move[0]==22 && battle->pp[0]==24 && self->_battleButtons[0].enabled) {
                record(@"originalVineWhip",YES); action=^{ [self->_battleButtons[0] sendActionsForControlEvents:UIControlEventTouchUpInside]; }; stage=301;
            } else if (stage==301 && q && (q->semanticsMask&0x100) && !(q->semanticsMask&16)
                && battle && battle->mode==HG_BATTLE_UI_NONE && !self->_battleMenuScene && !q->enemyHp && !self->_buttons[@1].hidden) {
                [self->_condition lock]; BOOL idle=!(self->_keys&3) && !(self->_pendingKeys&3) && !(self->_submittedKeys&3);
                uint32_t reads=self->_lastPadReadCount; [self->_condition unlock];
                if (learnStatPulses==1 && idle && !learnReleaseStarted) { learnReleaseReads=reads; learnReleaseStarted=YES; }
                if (idle && learnStatPulses<2 && (!learnStatPulses || (learnReleaseStarted && reads-learnReleaseReads>=2))) {
                    learnStatPulses++;
                    [events addObject:@{@"statPulse":@(learnStatPulses),@"displayFrame":@(sPrivateDisplayFrame),@"semanticsMask":@(q->semanticsMask),
                        @"commandMode":@(battle->mode),@"commandReady":@(battle->ready),@"rawAVisible":@(!self->_buttons[@1].hidden),
                        @"padReads":@(reads),@"releasePadReads":@(learnStatPulses==2 ? reads-learnReleaseReads : 0)}];
                    capture=learnStatPulses==1 ? @"private-learn-stats-difference.png" : @"private-learn-stats-total.png";
                    action=^{ [self->_buttons[@1] accessibilityActivate]; };
                }
            } else if ((stage==301 || stage==302 || stage==303 || stage==309 || stage==310 || stage==313 || stage==314) && prompt && inputIdle
                && self->_battleButtons[0].enabled && self->_battleButtons[1].enabled) {
                uint32_t mode=(stage==302 || stage==309 || stage==314) ? HG_BATTLE_UI_GIVE_UP : HG_BATTLE_UI_KEEP_FORGET;
                uint32_t candidate=stage>=313 ? 256 : 255;
                if (battle->mode==mode && q && q->replyItem==candidate) {
                    if (!promptValid(mode,candidate)) { finish(@"learning_prompt_contract_failed"); return; }
                    if ((stage==301 && (learnStatPulses!=2 || q->semanticsMask!=2207 || q->replyPocket!=37239 || q->moves[3]!=254 || q->pp[3]!=20 || q->enemyHp || q->replyCount))
                        || (stage==309 && (q->replyCount!=1 || q->replyTarget!=255))
                        || (stage==313 && (q->semanticsMask!=2239 || q->replyCount!=2 || q->replyTarget!=1 || q->moves[0]!=255 || q->pp[0]!=10 || q->moves[3]!=254 || q->pp[3]!=20))) {
                        finish(@"learning_original_state_mismatch"); return;
                    }
                    if ((stage==301 || stage==302) && !waits) {
                        [self->_condition lock]; self->_keys|=1; self->_pendingKeys|=1; self->_touchX=128; self->_touchY=68; self->_touchPressed=YES; [self->_condition unlock]; waits=1;
                    } else {
                        [self->_condition lock]; BOOL cleared=!(self->_keys&3) && !(self->_pendingKeys&3) && !(self->_submittedKeys&3) && !self->_touchPressed;
                        [self->_condition unlock];
                        if ((stage==301 || stage==302) && !cleared) { finish(@"learning_prompt_raw_input_not_cleared"); return; }
                        NSUInteger choice=(stage==303 || stage==310 || stage==314) ? 0 : 1;
                        if (stage==301) {
                            learnRawPromptMask|=1; learnBefore=[qaData copy]; record(@"originalStatAdvances",learnStatPulses==2);
                            record(@"originalExpLevelUp",q->replyPocket==37239 && (q->semanticsMask&128)); record(@"autoStockpile",q->moves[3]==254 && q->pp[3]==20 && (q->semanticsMask&16));
                            capture=@"private-learn-spit-up-keep.png";
                        } else if (stage==302) { learnRawPromptMask|=2; record(@"promptRawInputBlocked",learnRawPromptMask==3); capture=@"private-learn-spit-up-give-up.png"; }
                        else if (stage==303) record(@"keepContinue",YES);
                        else if (stage==309) { record(@"fifthCancelled",YES); capture=@"private-learn-fifth-give-up.png"; }
                        else if (stage==310) learnBefore=[qaData copy];
                        else if (stage==313) { record(@"replacementOriginalPP",(q->semanticsMask&32) && q->pp[0]==10 && q->pp[1]==30 && q->pp[2]==40 && q->pp[3]==20); capture=@"private-learn-swallow-keep.png"; }
                        else capture=@"private-learn-swallow-give-up.png";
                        action=^{ [self->_battleButtons[choice] sendActionsForControlEvents:UIControlEventTouchUpInside]; }; waits=0; stage++;
                    }
                }
            } else if ((stage==304 || stage==307 || stage==311) && bpReady && bp->phase==HG_BATTLE_PARTY_UI_LEARN_LIST && inputIdle
                && self->_battleMenuButtons[stage==304 ? 1 : stage==307 ? 4 : 0].enabled) {
                if (!partyValid(HG_BATTLE_PARTY_UI_LEARN_LIST,stage==307 ? 1 : 0)) { finish(@"learning_list_contract_failed"); return; }
                if (stage==304) { record(@"nativeLearnList",YES); capture=@"private-learn-spit-up-list.png"; }
                else if (stage==311) { record(@"continueAfterFifth",q && q->replyCount==1 && q->replyTarget==255); capture=@"private-learn-spit-up-retry-list.png"; }
                NSUInteger index=stage==304 ? 1 : stage==307 ? 4 : 0; learnRevision=bp->revision;
                action=^{ [self->_battleMenuButtons[index] sendActionsForControlEvents:UIControlEventTouchUpInside]; }; stage++;
            } else if ((stage==305 || stage==306 || stage==308 || stage==312) && bpReady && bp->phase==HG_BATTLE_PARTY_UI_LEARN_DETAIL && bp->revision!=learnRevision
                && inputIdle && self->_battleMenuButtons[stage==306 ? 2 : 0].enabled) {
                uint32_t selected=stage==305 || stage==306 ? 1 : stage==308 ? 4 : 0,x=0,y=0;
                if (!partyValid(HG_BATTLE_PARTY_UI_LEARN_DETAIL,selected)
                    || HGNativeBattlePartyUIResolveTouch(bp,learnRevision,bp->phase,0,&x,&y)) { finish(@"learning_detail_or_stale_contract_failed"); return; }
                learnStale++;
                if (stage==305) {
                    learnHMBody=CFBridgingRelease(CGDataProviderCopyData(CGImageGetDataProvider(self->_battleMenuDescription.image.CGImage)));
                    learnRevision=bp->revision; capture=@"private-learn-cut-detail.png";
                } else if (stage==306) {
                    NSData *body=CFBridgingRelease(CGDataProviderCopyData(CGImageGetDataProvider(self->_battleMenuDescription.image.CGImage)));
                    if ([body isEqualToData:learnHMBody] || !q || q->replyCount || q->moves[1]!=15) { finish(@"original_hm_rejection_not_observed"); return; }
                    record(@"hmRejected",YES); capture=@"private-learn-hm-rejection.png";
                } else if (stage==308) { record(@"infoPreservesBattle",YES); capture=@"private-learn-new-move-detail.png"; }
                else { if (!q || q->replyCount!=1 || q->replyTarget!=255 || bp->moves[0].maxPP!=24) { finish(@"learning_replacement_prerequisite_failed"); return; }
                    capture=@"private-learn-replace-vine-whip.png"; }
                NSUInteger index=stage==306 ? 2 : 0;
                action=^{ [self->_battleMenuButtons[index] sendActionsForControlEvents:UIControlEventTouchUpInside]; }; stage++;
            } else if (stage==315 && q && q->semanticsMask==2303) {
                BOOL final=q->replyItem==256 && q->replyPocket==37239 && q->replyCount==2 && q->replyTarget==1
                    && q->moves[0]==255 && q->moves[1]==15 && q->moves[2]==74 && q->moves[3]==254
                    && q->pp[0]==10 && q->pp[1]==30 && q->pp[2]==40 && q->pp[3]==20;
                record(@"finalRefusal",final && (q->semanticsMask&64)); record(@"savedPartyPreserved",(q->semanticsMask&2)!=0);
                record(@"learnRejectsStale",learnStale>=2); record(@"resetBeforeSettlement",q->battleActive && !q->returned && !q->lifecycleCleared);
                if (!final) { finish(@"learning_final_state_mismatch"); return; } capture=@"private-learn-complete.png"; stage=99;
            }
        }
        if (!forced && !learn) {
            if (stage==6 && command && self->_battleButtons[3].enabled) {
                if (runs>=6) { finish(@"escape_not_observed"); return; } runRevision=battle->revision; runs++;
                action=^{ [self->_battleButtons[3] sendActionsForControlEvents:UIControlEventTouchUpInside]; }; stage=7;
            } else if (stage==7 && q && q->returned && q->fieldReady && map==MAP_ROUTE_29) {
                record(@"runSettled",q->outcome==5 && q->battleResult==5 && q->lifecycleCleared && q->semanticsMask==63); stage=8;
            } else if (stage==7 && command && battle->revision!=runRevision) stage=6;
            if (stage==8) {
                if (!menuOpen) action=^{ [self->_menuButton sendActionsForControlEvents:UIControlEventTouchUpInside]; };
                else if (!self->_fieldMenu.hidden && self->_fieldMenu.userInteractionEnabled) {
                    for (NSUInteger i=0;i<7;i++) if (((self->_fieldMenuIcons>>(i*4))&15)==2 && [self->_fieldMenuElements[i] accessibilityActivate]) { stage=9; break; }
                }
            }
            if ((stage==9 || stage==12 || stage==16 || stage==20) && bagReady && bag->phase==HG_BAG_UI_INPUT) {
                if (bag->pocketId!=1) {
                    for (NSUInteger i=0;i<HG_BAG_UI_POCKETS;i++) if (bag->pockets[i].pocketId==1 && self->_bagPockets[i].enabled) { action=^{ [self->_bagPockets[i] sendActionsForControlEvents:UIControlEventTouchUpInside]; }; break; }
                } else for (NSUInteger i=0;i<MIN(bag->visibleCount,HG_BAG_UI_ITEMS);i++) if (bag->items[i].itemId==17 && self->_bagItems[i].enabled) {
                    uint32_t expected=stage==9 || stage==12 ? 5 : 4;
                    BOOL glyph=[self checkBagItem:i state:bag];
                    if (bag->items[i].quantity!=expected || !glyph) { finish(@"potion_quantity_or_original_glyph_mismatch"); return; }
                    if (stage==9) { bagRevision=bag->revision; capture=@"private-field-potion-bag.png"; stage=10; }
                    else if (stage==12) { record(@"targetCancelPreserves",bag->revision>bagRevision); capture=@"private-field-potion-cancel-bag.png"; stage=13; }
                    else if (stage==16) { record(@"consumedOnePotion",YES); capture=@"private-field-potion-consumed-bag.png"; stage=17; }
                    else { record(@"noEffectConsumesNothing",YES); capture=@"private-field-potion-final-bag.png"; stage=21; }
                    if (stage==21) action=^{ [self->_bagNavigation[0] sendActionsForControlEvents:UIControlEventTouchUpInside]; };
                    else action=^{ [self->_bagItems[i] sendActionsForControlEvents:UIControlEventTouchUpInside]; };
                    break;
                }
            } else if ((stage==10 || stage==13 || stage==17) && bagReady && bag->phase==HG_BAG_UI_ITEM_MENU) {
                for (NSUInteger i=0;i<HG_BAG_UI_ACTIONS;i++) if (i<bag->actionCount && bag->actions[i].actionId==0 && self->_bagActions[i].enabled) {
                    action=^{ [self->_bagActions[i] sendActionsForControlEvents:UIControlEventTouchUpInside]; }; stage=stage==10 ? 11 : stage==13 ? 14 : 18; break;
                }
            } else if (stage==11 && target && self->_partyBack.enabled) {
                beforeHP=party->mons[0].hp;
                BOOL baseline=party->count==1 && party->mons[0].species==152 && party->mons[0].maxHp==20 && beforeHP>0 && beforeHP<20;
                if (!baseline) { finish(@"post_flee_field_prerequisite_failed"); return; }
                BOOL prompt=party->message.tileWidth==20 && party->message.tileHeight==2
                    && self->_partyMessage.image.size.width==160 && [self checkPrivatePixels:self->_partyMessage.image] && [self checkPrivatePixels:self->_partyCards[0].image];
                record(@"targetPrompt",prompt); targetRevision=party->revision; capture=@"private-field-potion-target-cancel.png";
                action=^{ [self->_partyBack sendActionsForControlEvents:UIControlEventTouchUpInside]; }; stage=12;
            } else if (stage==14 && target && self->_partyMons[0].enabled) {
                uint32_t x=0,y=0,keys=0; BOOL pressed=NO;
                [self->_condition lock]; self->_partyChoicePending=YES; self->_partyChoiceRevision=targetRevision; self->_partyChoicePhase=HG_PARTY_UI_ITEM_TARGET;
                self->_partyChoiceKind=HG_PARTY_UI_TOUCH_MON; self->_partyChoiceIndex=0;
                [self takePartyTouchLocked:party reads:self->_lastPadReadCount keys:&keys x:&x y:&y pressed:&pressed];
                BOOL rejected=!self->_partyChoicePending && !self->_partyTouchActive; [self->_condition unlock];
                record(@"reopenRejectsStale",party->revision>targetRevision && rejected && party->mons[0].hp==beforeHP);
                record(@"targetCancelPreserves",[checks[@"targetCancelPreserves"] boolValue] && party->mons[0].hp==beforeHP);
                capture=@"private-field-potion-target-heal.png"; action=^{ [self->_partyMons[0] sendActionsForControlEvents:UIControlEventTouchUpInside]; }; stage=15;
            } else if ((stage==15 || stage==19) && HGNativePartyUIIsItemProgress(party) && party->phase==HG_PARTY_UI_ITEM_USE
                && party->ready && self->_partyBack.enabled && self->_partyMessage.image) {
                if (lastWaitFrame!=sPrivateDisplayFrame) { waits++; lastWaitFrame=sPrivateDisplayFrame; }
                if (waits>=2) {
                    BOOL glyph=party->message.tileWidth==27 && party->message.tileHeight==4 && self->_partyMessage.image.size.width==216
                        && self->_partyMessage.image.size.height==32 && [self checkPrivatePixels:self->_partyMessage.image];
                    NSData *pixels=CFBridgingRelease(CGDataProviderCopyData(CGImageGetDataProvider(self->_partyMessage.image.CGImage)));
                    if (stage==15) { successMessage=pixels; record(@"hpRestored",party->mons[0].hp==20 && party->mons[0].maxHp==20);
                        record(@"successOriginalMessage",glyph); capture=@"private-field-potion-healed-result.png"; stage=16; }
                    else { record(@"fullHpNoEffect",party->selected==HG_PARTY_UI_CONFIRM && party->mons[0].hp==20);
                        record(@"noEffectOriginalMessage",glyph && successMessage && ![successMessage isEqualToData:pixels]);
                        capture=@"private-field-potion-noeffect-result.png"; stage=20; }
                    waits=0; lastWaitFrame=0; continues++; action=^{ [self->_partyBack sendActionsForControlEvents:UIControlEventTouchUpInside]; };
                }
            } else if (stage==18 && target && self->_partyMons[0].enabled) {
                capture=@"private-field-potion-fullhp-target.png"; action=^{ [self->_partyMons[0] sendActionsForControlEvents:UIControlEventTouchUpInside]; }; stage=19;
            } else if (stage==21 && party && !party->active && bag && !bag->active && map==MAP_ROUTE_29) {
                if (menuOpen) [self toggleMenu];
                lastWaitFrame=sPrivateDisplayFrame; stage=22;
            } else if (stage==22 && party && !party->active && bag && !bag->active && map==MAP_ROUTE_29 && !menuOpen
                && sPrivateDisplayFrame>lastWaitFrame && !self->_canvas.hidden && self->_canvas.userInteractionEnabled
                && self->_partyPanel.hidden && self->_bagPanel.hidden && self->_summaryPanel.hidden && self->_battleMenuPanel.hidden
                && self->_fieldMenu.hidden && self->_fieldMenuShade.hidden && !self->_joystick.hidden && self->_joystick.userInteractionEnabled
                && !self->_menuButton.hidden && !self->_buttons[@1].hidden && !self->_buttons[@2].hidden
                && [self->_menuButton.currentTitle isEqualToString:@"菜单"] && !self->_menuButton.titleLabel.hidden && self->_menuButton.titleLabel.alpha==1) {
                [self->_condition lock]; NSData *geometry=self->_geometryState; [self->_condition unlock];
                const uint32_t *scene=geometry.bytes; CGImageRef image=self->_canvas.image.CGImage;
                BOOL varied=[self checkPrivatePixels:self->_canvas.image];
                if (geometry.length==8*sizeof(uint32_t) && scene[0] && scene[1] && varied && image
                    && CGImageGetWidth(image)>256 && self->_displayedWidth==CGImageGetWidth(image)) {
                    record(@"returnedField",YES); record(@"continueExactlyOnceEach",continues==2);
                    [events addObject:@{@"stage":@22,@"displayFrame":@(sPrivateDisplayFrame),@"afterMenuCloseFrame":@(lastWaitFrame),
                        @"canvasWidth":@(CGImageGetWidth(image)),@"canvasHeight":@(CGImageGetHeight(image)),
                        @"vertices":@(scene[0]),@"polygons":@(scene[1]),@"canvasVaried":@(varied)}];
                    capture=@"private-field-potion-return.png"; stage=99;
                }
            }
        } else if (forced && stage>=20) {
            if (stage==20 && bpReady && bp->phase==HG_BATTLE_PARTY_UI_ROSTER) {
                uint32_t x=0,y=0; BOOL reject=!HGNativeBattlePartyUIResolveTouch(bp,bp->revision,bp->phase,6,&x,&y);
                [self->_battleMenuButtons[6] sendActionsForControlEvents:UIControlEventTouchUpInside];
                [self->_condition lock]; BOOL unqueued=!self->_battleMenuChoicePending && !self->_battleMenuTouchActive; [self->_condition unlock];
                record(@"nativeRoster",bp->partyCount==3 && !bp->mons[0].hp && bp->mons[1].isEgg && bp->mons[2].species==155
                    && self->_canvas.hidden && !self->_battleMenuButtons[6].enabled && self->_battleMenuImages[0].image);
                record(@"typedCancelRejected",reject && unqueued && !q->replyCount); capture=@"private-forced-switch-roster.png"; stage=21;
            } else if (stage==21 && bpReady && bp->phase==HG_BATTLE_PARTY_UI_ROSTER && self->_battleMenuButtons[0].enabled) {
                [self->_condition lock];
                if (!self->_battleMenuChoicePending && !self->_battleMenuTouchActive && !self->_battleMenuTouchCooling) {
                    rawCancelRevision=bp->revision; self->_battleMenuTouchKind=OriginalBattleMenuParty; self->_battleMenuTouchRevision=bp->revision;
                    self->_battleMenuTouchX=bp->buttons[6].hitX; self->_battleMenuTouchY=bp->buttons[6].hitY;
                    self->_battleMenuTouchReads=self->_lastPadReadCount; self->_battleMenuTouchActive=YES; stage=22;
                }
                [self->_condition unlock];
            } else if (stage==22 && bpReady && bp->phase==HG_BATTLE_PARTY_UI_ROSTER && bp->selectedSlot==6
                && bp->revision>rawCancelRevision && self->_battleMenuButtons[0].enabled) {
                [self->_condition lock];
                BOOL inputIdle=!self->_battleMenuChoicePending && !self->_battleMenuTouchActive && !self->_battleMenuTouchCooling;
                [self->_condition unlock];
                if (inputIdle) {
                    record(@"originalCancelRejected",!q->replyCount && q->battleActive && !q->returned);
                    capture=@"private-forced-switch-cancel.png"; action=^{ [self->_battleMenuButtons[0] sendActionsForControlEvents:UIControlEventTouchUpInside]; }; stage=23;
                }
            } else if ((stage==23 || stage==27 || stage==31) && bpReady && bp->phase==HG_BATTLE_PARTY_UI_MON_MENU && self->_battleMenuButtons[0].enabled) {
                if (stage==23) capture=@"private-forced-switch-fainted-menu.png";
                else if (stage==27) { record(@"eggInfoDisabled",bp->mons[bp->selectedSlot].isEgg && !(bp->enabledMask&6)
                    && !self->_battleMenuButtons[1].enabled && !self->_battleMenuButtons[2].enabled); capture=@"private-forced-switch-egg-menu.png"; }
                else capture=@"private-forced-switch-reserve-menu.png";
                action=^{ [self->_battleMenuButtons[0] sendActionsForControlEvents:UIControlEventTouchUpInside]; }; stage++;
            } else if ((stage==24 || stage==28) && bpReady && bp->phase==HG_BATTLE_PARTY_UI_ACK && self->_battleMenuButtons[0].enabled) {
                record(stage==24 ? @"faintedShiftAck" : @"eggShiftAck",!q->replyCount && [self checkPrivatePixels:self->_battleMenuImages[0].image]);
                capture=stage==24 ? @"private-forced-switch-fainted-ack.png" : @"private-forced-switch-egg-ack.png";
                action=^{ [self->_battleMenuButtons[0] sendActionsForControlEvents:UIControlEventTouchUpInside]; }; stage++;
            } else if ((stage==25 || stage==29) && bpReady && bp->phase==HG_BATTLE_PARTY_UI_MON_MENU && self->_battleMenuButtons[3].enabled) {
                action=^{ [self->_battleMenuButtons[3] sendActionsForControlEvents:UIControlEventTouchUpInside]; }; stage++;
            } else if ((stage==26 || stage==30) && bpReady && bp->phase==HG_BATTLE_PARTY_UI_ROSTER && self->_battleMenuButtons[stage==26 ? 1 : 2].enabled) {
                NSUInteger index=stage==26 ? 1 : 2; action=^{ [self->_battleMenuButtons[index] sendActionsForControlEvents:UIControlEventTouchUpInside]; }; stage++;
            } else if (stage==32 && command && q->activePartySlot==2 && q->replyCount==1 && q->replyTarget==3 && q->semanticsMask==575) {
                record(@"reserveActive",q->playerHp>0); record(@"resetBeforeSettlement",q->battleActive && !q->returned && !q->lifecycleCleared);
                capture=@"private-forced-switch-active.png"; stage=99;
            }
        }
        if (capture) [self checkPrivateCapture:capture kind:@"UIKit" image:nil];
        if (action) action();
        if (stage==99) { finish(@"completed"); return; }
        if (q && q->started && !q->returned && !self->_battleMenuScene && battle && battle->mode==HG_BATTLE_UI_NONE
            && (!learn || (!(q->semanticsMask&0x100) && (q->enemyHp || (q->semanticsMask&16)))) && !self->_buttons[@1].hidden && !(polls%3))
            [self->_buttons[@1] accessibilityActivate];
        dispatch_after(dispatch_time(DISPATCH_TIME_NOW,NSEC_PER_SEC/10),dispatch_get_main_queue(),step);
    }; step();
}
#ifdef HEARTGOLD_ORIGINAL_SUMMARY_TM_CHECK
- (void)checkSummaryTMCase {
    enum { TM_OPEN_BAG=1, TM_POCKET, TM_ITEM, TM_USE, TM_BOOT_ACK, TM_BAG_YES, TM_TARGET, TM_FORGET_YES,
        TM_FIRST_SUMMARY, TM_FIFTH_CONFIRM, TM_STOP_NO, TM_RETRY_YES, TM_SECOND_SUMMARY, TM_HM_RESULT,
        TM_OLD_MOVE, TM_CONFIRM_BACK, TM_OLD_AGAIN, TM_CONFIRM_REPLACE, TM_RETURN_BAG, TM_CLOSE_BAG, TM_FIELD };
    NSArray *requiredKeys=@[@"caseBinding",@"muted",@"completeFixture",@"otNameMatchesProfile",@"nativeFiveMoveList",
        @"fifthCancelled",@"continuedTrying",@"secondSummaryLifetime",@"originalHMWarning",@"hmRejected",@"originalConfirm",
        @"confirmBack",@"replacementProtect",@"replacementPP",@"consumedOneTM",@"otherStatePreserved",@"rawLeaseReleased",
        @"staleRejected",@"nativeSummaryButtons",@"returnedField",@"privateSave4Preserved",@"resetQuiescent",@"capturesComplete",@"nativeBagButtons",@"nativePartyButtons"];
    NSArray *requiredPNGs=@[@"private-summary-tm-first-list.png",@"private-summary-tm-fifth-cancel.png",
        @"private-summary-tm-retry-list.png",@"private-summary-tm-hm-warning.png",@"private-summary-tm-confirm.png",
        @"private-summary-tm-back-to-list.png",@"private-summary-tm-final-bag.png",@"private-summary-tm-bag-continue.png",
        @"private-summary-tm-bag-question.png",@"private-summary-tm-party-target.png",@"private-summary-tm-party-continue.png",@"private-summary-tm-party-question.png"];
    NSMutableDictionary *checks=[NSMutableDictionary new]; for (NSString *key in requiredKeys) checks[key]=@NO;
    NSMutableArray *events=[NSMutableArray new];
    NSURL *support=[NSFileManager.defaultManager URLsForDirectory:NSApplicationSupportDirectory inDomains:NSUserDomainMask].firstObject;
    support=[support URLByAppendingPathComponent:@"HeartGoldOriginal" isDirectory:YES];
    NSMutableDictionary *privateBefore=[NSMutableDictionary new];
    for (NSString *suffix in @[@"",@".console",@".wifi",@".lock"]) {
        NSString *name=[@"heartgold.sav" stringByAppendingString:suffix]; NSData *data=[NSData dataWithContentsOfURL:[support URLByAppendingPathComponent:name]];
        if (data) privateBefore[name]=data;
    }
    [_condition lock]; sPrivateUICase=@"summary-tm"; sPrivateCaptures=[NSMutableArray new]; sPrivateObserving=NO; [_condition unlock];
    __block NSUInteger stage=0,polls=0,lastStage=NSNotFound,lifetimes=0,stale=0,summaryClicks=0;
    __block uint32_t stageFrame=0,lastRevision=0,firstGeneration=0,firstRevision=0,oldRevision=0,hmCalls=0,releaseReads=0;
    __block BOOL finishing=NO,typedRelease=NO,releaseStarted=NO;
    __block NSUInteger bagClicks=0,partyClicks=0; __block uint32_t bagKinds=0,bagRoles=0,partyKinds=0,partyRoles=0;
    NSMutableSet *capturedNames=[NSMutableSet new];
    __block void (^step)(void);
    BOOL (^unchanged)(NSDictionary *)=^BOOL(NSDictionary *facts) {
        return [facts[@"fixturePrepared"] boolValue] && ![facts[@"fixtureError"] length]
            && [facts[@"baseline26Preserved"] boolValue] && [facts[@"originalIdentityPreserved"] boolValue] && [facts[@"bagUnchanged"] boolValue] && [facts[@"flagsMoneyDexUnchanged"] boolValue]
            && [facts[@"tmQuantity"] unsignedIntValue]==2 && [facts[@"friendship"] unsignedIntValue]==70 && ![facts[@"mood"] unsignedIntValue]
            && [facts[@"moves"] isEqual:@[@33,@45,@75,@15]];
    };
    void (^finish)(NSString *)=^(NSString *status) {
        if (finishing) return; finishing=YES; step=nil;
        [self->_condition lock]; NSData *finalQA=[self->_summaryTMQAState copy]; NSDictionary *finalFacts=[self->_summaryTMFacts copy] ?: @{};
        uint32_t boots=self->_bootCount; self->_checkResetPending=YES; [self->_condition unlock];
        __block NSUInteger resetPolls=0; __block void (^resetStep)(void);
        resetStep=^{
            [self->_condition lock]; BOOL field=self->_bootCount==boots+1 && self->_fieldMapId==MAP_ROUTE_29 && self->_publishedFrames>self->_bootFrameBase;
            BOOL clean=self->_resetQuiesced && self->_restartClean,failed=self->_failure!=nil || self->_finished;
            NSDictionary *audio=self->_audioChanging ? nil : [self->_audio statistics]; [self->_condition unlock];
            if (!field && !failed && ++resetPolls<200) { dispatch_after(dispatch_time(DISPATCH_TIME_NOW,NSEC_PER_SEC/10),dispatch_get_main_queue(),resetStep); return; }
            checks[@"resetQuiescent"]=@(field && clean); checks[@"muted"]=@([checks[@"muted"] boolValue] && [audio[@"muted"] boolValue]);
            resetStep=nil; [self stop];
            __block NSUInteger stopPolls=0; __block void (^stopStep)(void);
            stopStep=^{
                [self->_condition lock]; BOOL stopped=self->_finished && !self->_env && !self->_game && !self->_wasi; [self->_condition unlock];
                if (!stopped && ++stopPolls<100) { dispatch_after(dispatch_time(DISPATCH_TIME_NOW,NSEC_PER_SEC/10),dispatch_get_main_queue(),stopStep); return; }
                checks[@"resetQuiescent"]=@([checks[@"resetQuiescent"] boolValue] && stopped);
                BOOL preserved=privateBefore.count==4;
                for (NSString *name in privateBefore) preserved&=[privateBefore[name] isEqualToData:[NSData dataWithContentsOfURL:[support URLByAppendingPathComponent:name]]];
                checks[@"privateSave4Preserved"]=@(preserved);
                NSMutableSet *files=[NSMutableSet new]; BOOL captures=sPrivateCaptures.count==requiredPNGs.count;
                for (NSDictionary *capture in sPrivateCaptures) { captures&=[capture[@"drawn"] boolValue] && [capture[@"written"] boolValue]; [files addObject:capture[@"file"]]; }
                captures&=[files isEqualToSet:[NSSet setWithArray:requiredPNGs]]; checks[@"capturesComplete"]=@(captures);
                BOOL passed=[status isEqual:@"completed"]; for (NSString *key in requiredKeys) passed&=[checks[key] boolValue];
                NSMutableArray *words=[NSMutableArray new]; if (finalQA.length==sizeof(HGSummaryLearnQAState))
                    for (NSUInteger i=0;i<finalQA.length/4;i++) [words addObject:@(((const uint32_t *)finalQA.bytes)[i])];
                NSDictionary *result=@{@"version":@1,@"case":@"summary-tm",@"complete":@(stopped),@"passed":@(passed),@"status":status,
                    @"stage":@(stage),@"polls":@(polls),@"checks":checks,@"requiredKeys":requiredKeys,@"requiredPNGs":requiredPNGs,
                    @"captures":[sPrivateCaptures copy],@"events":events,@"qaWordsBeforeReset":words,@"factsBeforeReset":finalFacts,
                    @"summaryLifetimes":@(lifetimes),@"staleRejected":@(stale),@"summaryButtonActions":@(summaryClicks),
                    @"privateSaveFileCount":@(privateBefore.count),@"privateRawPeripheralInputDiagnostic":@NO,@"newProductionAPI":@NO,
                    @"peripheralInputMode":@"native-UIKit-controls",@"bagButtonActions":@(bagClicks),@"partyButtonActions":@(partyClicks),
                    @"bagInputKinds":@(bagKinds),@"bagTMRoles":@(bagRoles),@"partyInputKinds":@(partyKinds),@"partyTMRoles":@(partyRoles),
                    @"scope":@"Actual native Bag/Party and Summary controls on the private TM17 route. Original bounded boot input is unchanged. Twelve UIKit captures; no full-game, all TM/HM, full-animation, audible-audio or physical-device claim."};
                NSURL *folder=[NSFileManager.defaultManager URLsForDirectory:NSDocumentDirectory inDomains:NSUserDomainMask].firstObject;
                [[NSJSONSerialization dataWithJSONObject:result options:NSJSONWritingPrettyPrinted error:nil]
                    writeToURL:[folder URLByAppendingPathComponent:@"private-summary-tm-check.json"] options:NSDataWritingAtomic error:nil];
                stopStep=nil;
            }; stopStep();
        }; resetStep();
    };
    NSString *built=[NSBundle.mainBundle objectForInfoDictionaryKey:@"HGPrivateCheckCase"] ?: @"default";
    checks[@"caseBinding"]=@([built isEqual:@"summary-tm"] && [NSProcessInfo.processInfo.environment[@"HEARTGOLD_TEST_MUTE"] isEqual:@"1"]);
    if (![checks[@"caseBinding"] boolValue] || privateBefore.count!=4) { finish(@"case_or_private_save_binding_failed"); return; }
    step=^{
        [self->_condition lock]; NSData *qaData=[self->_summaryTMQAState copy],*ownerSummary=[self->_summaryUIState copy],*ownerBag=[self->_bagUIState copy],*ownerParty=[self->_partyUIState copy];
        NSDictionary *facts=[self->_summaryTMFacts copy] ?: @{}; UIImage *hmImage=self->_summaryTMHMWarningImage;
        uint32_t map=self->_fieldMapId,reads=self->_summaryTMPadReads,bootStep=self->_summaryTMBootInputStep,bootFrame=self->_summaryTMBootInputFrame;
        BOOL failed=self->_failure!=nil || self->_finished,accepted=self->_summaryTMRequestAccepted,rawIdle=self->_summaryTMRawIdle;
        BOOL inputIdle=rawIdle && !self->_summaryChoicePending && !self->_summaryInputActive && !self->_bagChoicePending && !self->_bagTouchActive
            && !self->_partyChoicePending && !self->_partyTouchActive && !self->_keys && !self->_pendingKeys && !self->_submittedKeys && !self->_touchPressed;
        NSDictionary *audio=self->_audioChanging ? nil : [self->_audio statistics]; [self->_condition unlock];
        const HGSummaryLearnQAState *q=qaData.length==sizeof(*q) ? qaData.bytes : NULL;
        if (q && (q->version!=HG_SUMMARY_LEARN_QA_VERSION || q->byteSize!=sizeof(*q))) { finish(@"observer_abi_mismatch"); return; }
        if (failed || ++polls>3600 || events.count>=1024 || [facts[@"fixtureError"] length]) {
            finish(failed ? @"engine_failed" : [facts[@"fixtureError"] length] ? @"fixture_failed" : events.count>=1024 ? @"evidence_bound_exceeded" : @"flow_timeout"); return;
        }
        if (lastStage!=stage || (q && lastRevision!=q->revision)) {
            if (lastStage!=stage) { stageFrame=q ? q->frame : 0; lastStage=stage; }
            [events addObject:@{@"event":@"state",@"stage":@(stage),@"qaFrame":@(q ? q->frame : 0),@"revision":@(q ? q->revision : 0),
                @"owner":@(q ? q->owner : 0),@"generation":@(q ? q->generation : 0),@"phase":@(q ? q->phase : 0),
                @"context":@(q ? q->context : 0),@"taskState":@(q ? q->taskState : 0),@"padReads":@(reads),
                @"bootStep":@(bootStep),@"bootInputFrame":@(bootFrame)}]; lastRevision=q ? q->revision : 0;
        }
        if (q && stageFrame && q->frame-stageFrame>3600) { finish(@"original_scene_or_input_handshake_timeout"); return; }
        if (!q) { dispatch_after(dispatch_time(DISPATCH_TIME_NOW,NSEC_PER_SEC/10),dispatch_get_main_queue(),step); return; }
        if (typedRelease) {
            if (!inputIdle) releaseStarted=NO;
            else if (!releaseStarted) { releaseStarted=YES; releaseReads=reads; }
            else if (reads-releaseReads>=2) { typedRelease=NO; releaseStarted=NO; }
        }
        if (!inputIdle || typedRelease) { dispatch_after(dispatch_time(DISPATCH_TIME_NOW,NSEC_PER_SEC/10),dispatch_get_main_queue(),step); return; }
        const HGNativeSummaryUIState *summary=self->_displayedSummaryUIState.length==sizeof(*summary) ? self->_displayedSummaryUIState.bytes : NULL;
        const HGNativePartyUIState *party=self->_displayedPartyUIState.length==sizeof(*party) ? self->_displayedPartyUIState.bytes : NULL;
        const HGNativeBagUIState *bag=self->_displayedBagUIState.length==sizeof(*bag) ? self->_displayedBagUIState.bytes : NULL;
        BOOL summaryReady=summary && summary->active && summary->ready && HGNativeSummaryUIIsLearning(summary)
            && ownerSummary.length==sizeof(*summary) && !memcmp(ownerSummary.bytes,summary,sizeof(*summary))
            && q->owner==HG_SUMMARY_LEARN_SUMMARY && q->execState==2 && q->graphicsReady && q->fadeFinished;
        BOOL bagReady=bag && bag->active && bag->ready && !bag->mode && q->owner==HG_SUMMARY_LEARN_BAG && q->phase==bag->phase
            && q->execState==2 && q->graphicsReady && q->fadeFinished && ownerBag.length==sizeof(*bag) && !memcmp(ownerBag.bytes,bag,sizeof(*bag));
        BOOL partyReady=party && party->active && party->ready && HGNativePartyUIIsTMHM(party) && q->owner==HG_SUMMARY_LEARN_PARTY
            && q->phase==party->phase && q->context==party->context && q->execState==2 && q->graphicsReady && q->fadeFinished
            && ownerParty.length==sizeof(*party) && !memcmp(ownerParty.bytes,party,sizeof(*party));
        BOOL (^nativeSummary)(void)=^BOOL {
            if (!summaryReady || summary->mode!=2 || summary->monDataType!=1 || summary->partyCount!=1 || summary->partySlot
                || summary->moveToLearn!=182 || summary->page!=1 || self->_summaryPanel.hidden || !self->_canvas.hidden
                || self->_canvas.userInteractionEnabled || !self->_bagPanel.hidden || !self->_partyPanel.hidden
                || !self->_buttons[@1].hidden || !self->_buttons[@2].hidden || self->_summaryPreview.image.size.width!=256
                || self->_summaryPreview.image.size.height!=192 || self->_summaryContent.image.size.width!=120 || self->_summaryContent.image.size.height!=160
                || ![self checkPrivatePixels:self->_summaryPreview.image] || ![self checkPrivatePixels:self->_summaryContent.image]
                || self->_summaryNavigation[0].hidden || !self->_summaryNavigation[0].enabled || ![self checkSummaryRasterBack]) return NO;
            for (NSUInteger i=2;i<5;i++) if (!self->_summaryNavigation[i].hidden) return NO;
            NSArray *moves=facts[@"moves"],*pp=facts[@"pp"],*max=facts[@"maxPP"];
            if (moves.count!=4 || pp.count!=4 || max.count!=4) return NO;
            for (NSUInteger i=0;i<4;i++) {
                const HGNativeSummaryUIMove *move=&summary->moves[i];
                if (move->moveId!=[moves[i] unsignedIntValue] || move->pp!=[pp[i] unsignedIntValue] || move->maxPP!=[max[i] unsignedIntValue]
                    || !move->moveId || self->_summaryMoves[i].hidden || self->_summaryMoves[i].bounds.size.height<44
                    || self->_summaryMoveImages[i].image.size.width!=move->cropWidth || self->_summaryMoveImages[i].image.size.height!=move->cropHeight
                    || ![self checkPrivatePixels:self->_summaryMoveImages[i].image]) return NO;
                if (summary->phase==HG_SUMMARY_UI_FORGET_MOVE && !self->_summaryMoves[i].enabled) return NO;
            }
            return YES;
        };
        NSString *capture=nil; dispatch_block_t action=nil; HGNativeSummaryUIInput ignored;
        UIButton *peripheralButton=nil; BOOL peripheralBag=NO; uint32_t peripheralKind=0,peripheralIndex=0,touchX=0,touchY=0; NSUInteger originStage=stage;
        BOOL bagTM=bagReady && HGNativeBagUIIsTMHM(bag),partyTM=partyReady && HGNativePartyUIIsTMHM(party);
        BOOL printer=(bagTM && q->phase==HG_BAG_UI_TM_MESSAGE && q->taskState==2 && q->tmTaskMatches)
            || (partyTM && q->phase==HG_PARTY_UI_TM_MESSAGE && (q->context==6 || q->context==7));
        if (printer && q->printerId<8 && q->printerActive && q->printerMatches && q->printerState==2 && q->printerWaiting) {
            if (bagTM && HGNativeBagUIResolveTouch(bag,bag->revision,bag->phase,HG_BAG_UI_TOUCH_ACTION,0,&touchX,&touchY)) {
                peripheralButton=self->_bagActions[0];peripheralBag=YES;peripheralKind=HG_BAG_UI_TOUCH_ACTION;
                if (![capturedNames containsObject:@"private-summary-tm-bag-continue.png"]) capture=@"private-summary-tm-bag-continue.png";
            } else if (partyTM && HGNativePartyUIResolveTouch(party,party->revision,party->phase,HG_PARTY_UI_TOUCH_ACTION,0,&touchX,&touchY)) {
                peripheralButton=self->_partyActions[0];peripheralKind=HG_PARTY_UI_TOUCH_ACTION;
                if (![capturedNames containsObject:@"private-summary-tm-party-continue.png"]) capture=@"private-summary-tm-party-continue.png";
            }
        } else if (!stage && q->fieldReady && !q->owner && map==MAP_ROUTE_29 && bootStep==3 && !bootFrame) {
            checks[@"muted"]=@([audio[@"muted"] boolValue]);
            if ([self queueSummaryTMFixture:q->revision]) stage=TM_OPEN_BAG;
        } else if (stage==TM_OPEN_BAG && q->fieldReady && !q->owner && map==MAP_ROUTE_29 && accepted && [facts[@"fixturePrepared"] boolValue]) {
            BOOL fixture=[facts[@"species"] unsignedIntValue]==152 && [facts[@"partyCount"] unsignedIntValue]==1 && unchanged(facts);
            NSArray *pp=facts[@"pp"],*max=facts[@"maxPP"],*ups=facts[@"ppUps"];
            fixture&=pp.count==4 && max.count==4 && ups.count==4;
            for (NSUInteger i=0;i<MIN(pp.count,MIN(max.count,ups.count));i++) fixture&=[pp[i] unsignedIntValue]>0 && [pp[i] isEqual:max[i]] && ![ups[i] unsignedIntValue];
            BOOL ot=[facts[@"otNameProfileMatched"] boolValue] && [facts[@"otNameLength"] unsignedIntValue]>=1
                && [facts[@"otNameLength"] unsignedIntValue]<=7 && [facts[@"fixtureUsesExistingStarter"] boolValue] && [facts[@"originalIdentityPreserved"] boolValue]
                && [facts[@"originalPersonality"] isKindOfClass:NSNumber.class] && [facts[@"originalOtId"] isKindOfClass:NSNumber.class] && [facts[@"mapSection"] unsignedIntValue];
            checks[@"completeFixture"]=@(fixture); checks[@"otNameMatchesProfile"]=@(ot);
            if (!fixture || !ot) { finish(@"original_fixture_mismatch"); return; }
            if (!self->_fieldMenuOpen) action=^{ [self->_menuButton sendActionsForControlEvents:UIControlEventTouchUpInside]; };
            else if (!self->_fieldMenu.hidden && self->_fieldMenu.userInteractionEnabled) {
                for (NSUInteger i=0;i<7;i++) if (((self->_fieldMenuIcons>>(i*4))&15)==2 && [self->_fieldMenuElements[i] accessibilityActivate]) { stage=TM_POCKET; break; }
            }
        } else if (stage==TM_POCKET && bagReady && HGNativeBagUIIsList(bag)) {
            for (uint32_t i=0;i<HG_BAG_UI_POCKETS;i++) if (bag->pockets[i].enabled && bag->pockets[i].pocketId==3
                && HGNativeBagUIResolveTouch(bag,bag->revision,bag->phase,HG_BAG_UI_TOUCH_POCKET,i,&touchX,&touchY)) {
                peripheralButton=self->_bagPockets[i];peripheralBag=YES;peripheralKind=HG_BAG_UI_TOUCH_POCKET;peripheralIndex=i;stage=TM_ITEM;break;
            }
        } else if (stage==TM_ITEM && bagReady && HGNativeBagUIIsList(bag) && bag->pocketId==3) {
            for (uint32_t i=0;i<MIN(bag->visibleCount,HG_BAG_UI_ITEMS);i++) if (bag->items[i].enabled && bag->items[i].itemId==344 && bag->items[i].quantity==2
                && HGNativeBagUIResolveTouch(bag,bag->revision,bag->phase,HG_BAG_UI_TOUCH_ITEM,i,&touchX,&touchY)) {
                peripheralButton=self->_bagItems[i];peripheralBag=YES;peripheralKind=HG_BAG_UI_TOUCH_ITEM;peripheralIndex=i;stage=TM_USE;break;
            }
        } else if (stage==TM_USE && bagReady && bag->phase==HG_BAG_UI_ITEM_MENU && bag->selectedItemId==344) {
            for (uint32_t i=0;i<MIN(bag->actionCount,HG_BAG_UI_ACTIONS);i++) if (bag->actions[i].enabled && !bag->actions[i].actionId
                && HGNativeBagUIResolveTouch(bag,bag->revision,bag->phase,HG_BAG_UI_TOUCH_ACTION,i,&touchX,&touchY)) {
                peripheralButton=self->_bagActions[i];peripheralBag=YES;peripheralKind=HG_BAG_UI_TOUCH_ACTION;peripheralIndex=i;stage=TM_BOOT_ACK;break;
            }
        } else if (stage==TM_BOOT_ACK && bagTM && q->taskState==1 && q->printerId<8 && !q->printerActive
            && HGNativeBagUIResolveTouch(bag,bag->revision,bag->phase,HG_BAG_UI_TOUCH_ACTION,0,&touchX,&touchY)) {
            peripheralButton=self->_bagActions[0];peripheralBag=YES;peripheralKind=HG_BAG_UI_TOUCH_ACTION;stage=TM_BAG_YES;
            if (![capturedNames containsObject:@"private-summary-tm-bag-continue.png"]) capture=@"private-summary-tm-bag-continue.png";
        } else if ((stage==TM_BAG_YES || stage==TM_FORGET_YES || stage==TM_STOP_NO || stage==TM_RETRY_YES)
            && q->yesNoReady && q->yesNoActive==1 && q->yesNoResult==3 && !q->yesNoTimer) {
            BOOL bagPrompt=stage==TM_BAG_YES && bagTM && q->phase==HG_BAG_UI_TM_MESSAGE && q->tmTaskMatches && q->taskState==3;
            BOOL partyPrompt=stage!=TM_BAG_YES && partyTM && q->phase==HG_PARTY_UI_TM_YES_NO
                && q->context==(stage==TM_FORGET_YES ? 6u : 7u) && q->itemId==344 && q->moveId==182;
            uint32_t row=stage==TM_STOP_NO ? 1 : 0;
            if ((bagPrompt || partyPrompt) && q->yesNoX==25 && q->yesNoY==(bagPrompt ? 6u : 10u)
                && q->yesNoWidth==6 && q->yesNoHeight==4 && q->yesHitX[row]==224 && q->yesHitY[row]==q->yesNoY*8+16+row*32) {
                if (stage==TM_STOP_NO) {
                    checks[@"fifthCancelled"]=@(q->selectedMoveSlot==4 && unchanged(facts));
                    if (![checks[@"fifthCancelled"] boolValue]) { finish(@"fifth_did_not_cancel"); return; }
                }
                if (bagPrompt && HGNativeBagUIResolveTouch(bag,bag->revision,bag->phase,HG_BAG_UI_TOUCH_ACTION,row,&touchX,&touchY)) {
                    peripheralButton=self->_bagActions[row];peripheralBag=YES;peripheralKind=HG_BAG_UI_TOUCH_ACTION;peripheralIndex=row;
                    if (![capturedNames containsObject:@"private-summary-tm-bag-question.png"]) capture=@"private-summary-tm-bag-question.png";
                } else if (partyPrompt && HGNativePartyUIResolveTouch(party,party->revision,party->phase,HG_PARTY_UI_TOUCH_ACTION,row,&touchX,&touchY)) {
                    peripheralButton=self->_partyActions[row];peripheralKind=HG_PARTY_UI_TOUCH_ACTION;peripheralIndex=row;
                    if (![capturedNames containsObject:@"private-summary-tm-party-question.png"]) capture=@"private-summary-tm-party-question.png";
                }
                if (peripheralButton) stage++;
            }
        } else if (stage==TM_TARGET && partyReady && q->context==6 && q->phase==HG_PARTY_UI_TM_TARGET
            && !q->partyBusy && !q->partySlot && q->itemId==344 && q->moveId==182
            && HGNativePartyUIResolveTouch(party,party->revision,party->phase,HG_PARTY_UI_TOUCH_MON,0,&touchX,&touchY)) {
            peripheralButton=self->_partyMons[0];peripheralKind=HG_PARTY_UI_TOUCH_MON;stage=TM_FORGET_YES;
            capture=@"private-summary-tm-party-target.png";
        } else if ((stage==TM_FIRST_SUMMARY || stage==TM_SECOND_SUMMARY) && summaryReady && summary->phase==HG_SUMMARY_UI_FORGET_MOVE) {
            BOOL native=nativeSummary() && summary->moveMask==31 && !self->_summaryNavigation[5].hidden
                && self->_summaryNavigation[5].enabled && [self checkPrivatePixels:self->_summaryNavigation[5].configuration.image];
            if (!native || !unchanged(facts)) { finish(@"native_five_move_list_mismatch"); return; }
            checks[@"nativeFiveMoveList"]=@(native && (!lifetimes || [checks[@"nativeFiveMoveList"] boolValue])); lifetimes++;
            if (stage==TM_FIRST_SUMMARY) {
                firstGeneration=q->generation; firstRevision=summary->revision; capture=@"private-summary-tm-first-list.png";
                if (HGNativeSummaryUIResolveInput(summary,summary->revision,summary->phase,HG_SUMMARY_UI_INPUT_MOVE,5,&ignored)) { finish(@"invalid_fifth_index_accepted"); return; } stale++;
                action=^{ [self->_summaryNavigation[5] sendActionsForControlEvents:UIControlEventTouchUpInside]; }; stage=TM_FIFTH_CONFIRM;
            } else {
                if (q->generation<=firstGeneration || lifetimes!=2 || HGNativeSummaryUIResolveInput(summary,firstRevision,HG_SUMMARY_UI_FORGET_MOVE,HG_SUMMARY_UI_INPUT_MOVE,4,&ignored)) {
                    finish(@"old_summary_lifetime_accepted"); return;
                }
                stale++; checks[@"continuedTrying"]=@YES; checks[@"secondSummaryLifetime"]=@YES; hmCalls=q->hmCalls;
                capture=@"private-summary-tm-retry-list.png"; action=^{ [self->_summaryMoves[3] sendActionsForControlEvents:UIControlEventTouchUpInside]; }; stage=TM_HM_RESULT;
            }
        } else if (stage==TM_FIFTH_CONFIRM && summaryReady && summary->phase==HG_SUMMARY_UI_FORGET_MOVE && summary->moveSlot==4 && !self->_summaryNavigation[1].hidden && self->_summaryNavigation[1].enabled) {
            if (!nativeSummary() || !unchanged(facts) || HGNativeSummaryUIResolveInput(summary,firstRevision,HG_SUMMARY_UI_FORGET_MOVE,HG_SUMMARY_UI_INPUT_MOVE,4,&ignored)) { finish(@"fifth_selection_or_stale_guard_failed"); return; }
            stale++; capture=@"private-summary-tm-fifth-cancel.png"; action=^{ [self->_summaryNavigation[1] sendActionsForControlEvents:UIControlEventTouchUpInside]; }; stage=TM_STOP_NO;
        } else if (stage==TM_HM_RESULT && summaryReady && summary->phase==HG_SUMMARY_UI_FORGET_MOVE && summary->moveSlot==3 && q->hmCalls==hmCalls+1) {
            BOOL warning=nativeSummary() && q->hmOwner==q->data && q->hmMoveSlot==3 && summary->moves[3].moveId==15
                && q->hmWarning.pixelBuffer && q->hmWarning.tileWidth==15 && q->hmWarning.tileHeight==10 && q->hmWarning.engine==1 && !q->hmWarning.clearColor
                && hmImage.size.width==120 && hmImage.size.height==80 && [self checkPrivatePixels:hmImage]
                && [self checkGlyph:hmImage inCard:self->_summaryContent.image x:0 y:80];
            checks[@"originalHMWarning"]=@(warning); checks[@"hmRejected"]=@(warning && unchanged(facts));
            if (![checks[@"hmRejected"] boolValue]) { finish(@"original_hm_warning_mismatch"); return; }
            capture=@"private-summary-tm-hm-warning.png"; stage=TM_OLD_MOVE;
        } else if ((stage==TM_OLD_MOVE || stage==TM_OLD_AGAIN) && summaryReady && summary->phase==HG_SUMMARY_UI_FORGET_MOVE && self->_summaryMoves[0].enabled) {
            if (!nativeSummary() || !unchanged(facts)) { finish(@"old_move_before_confirmation_mismatch"); return; }
            if (stage==TM_OLD_AGAIN) { checks[@"confirmBack"]=@YES; capture=@"private-summary-tm-back-to-list.png"; }
            oldRevision=summary->revision; action=^{ [self->_summaryMoves[0] sendActionsForControlEvents:UIControlEventTouchUpInside]; }; stage++;
        } else if ((stage==TM_CONFIRM_BACK || stage==TM_CONFIRM_REPLACE) && summaryReady && summary->phase==HG_SUMMARY_UI_FORGET_CONFIRM && summary->moveSlot==0) {
            BOOL confirm=nativeSummary() && self->_summaryNavigation[1].enabled && !self->_summaryNavigation[1].hidden
                && self->_summaryNavigation[5].hidden && [self checkPrivatePixels:self->_summaryNavigation[1].configuration.image];
            if (!confirm || !unchanged(facts) || HGNativeSummaryUIResolveInput(summary,oldRevision,HG_SUMMARY_UI_FORGET_MOVE,HG_SUMMARY_UI_INPUT_MOVE,0,&ignored)) { finish(@"original_confirm_or_stale_guard_failed"); return; }
            stale++; checks[@"originalConfirm"]=@YES;
            if (stage==TM_CONFIRM_BACK) { capture=@"private-summary-tm-confirm.png"; action=^{ [self->_summaryNavigation[0] sendActionsForControlEvents:UIControlEventTouchUpInside]; }; }
            else action=^{ [self->_summaryNavigation[1] sendActionsForControlEvents:UIControlEventTouchUpInside]; };
            stage++;
        } else if (stage==TM_RETURN_BAG && bagReady && HGNativeBagUIIsList(bag)) {
            NSArray *pp=facts[@"pp"],*max=facts[@"maxPP"],*ups=facts[@"ppUps"];
            BOOL learned=[facts[@"expectedLearningDelta"] boolValue] && [facts[@"moves"] isEqual:@[@182,@45,@75,@15]]
                && pp.count==4 && max.count==4 && ups.count==4 && [pp[0] unsignedIntValue]==10 && [max[0] unsignedIntValue]==10 && ![ups[0] unsignedIntValue];
            BOOL others=[facts[@"flagsMoneyDexUnchanged"] boolValue] && [facts[@"originalIdentityPreserved"] boolValue] && [facts[@"bagOnlyOneTMConsumed"] boolValue]
                && [facts[@"friendship"] unsignedIntValue]==71 && [facts[@"mood"] unsignedIntValue]==40;
            BOOL item=NO; for (NSUInteger i=0;i<MIN(bag->visibleCount,HG_BAG_UI_ITEMS);i++) if (bag->items[i].itemId==344 && bag->items[i].quantity==1
                && self->_bagItems[i].enabled && [self checkBagItem:i state:bag]) item=YES;
            checks[@"replacementProtect"]=@(learned); checks[@"replacementPP"]=@(learned); checks[@"otherStatePreserved"]=@(others);
            checks[@"consumedOneTM"]=@(item && [facts[@"tmQuantity"] unsignedIntValue]==1);
            if (!learned || !others || ![checks[@"consumedOneTM"] boolValue] || (summary && summary->active) || (party && party->active)) { finish(@"original_learning_delta_mismatch"); return; }
            capture=@"private-summary-tm-final-bag.png"; stage=TM_CLOSE_BAG;
        } else if (stage==TM_CLOSE_BAG && bagReady && HGNativeBagUIIsList(bag)) {
            if (HGNativeBagUIResolveTouch(bag,bag->revision,bag->phase,HG_BAG_UI_TOUCH_CANCEL,0,&touchX,&touchY)) {
                peripheralButton=self->_bagNavigation[0];peripheralBag=YES;peripheralKind=HG_BAG_UI_TOUCH_CANCEL;stage=TM_FIELD;
            }
        } else if (stage==TM_FIELD && q->fieldReady && !q->owner && map==MAP_ROUTE_29
            && (!bag || !bag->active) && (!party || !party->active) && (!summary || !summary->active)) {
            uint32_t presses=[facts[@"rawPresses"] unsignedIntValue],releases=[facts[@"rawReleases"] unsignedIntValue];
            checks[@"rawLeaseReleased"]=@(rawIdle && inputIdle && !typedRelease && !presses && !releases && ![facts[@"rawRejected"] unsignedIntValue]);
            checks[@"nativeBagButtons"]=@(bagClicks>=6 && (bagKinds&0x27)==0x27 && (bagRoles&3)==3);
            checks[@"nativePartyButtons"]=@(partyClicks>=4 && (partyKinds&3)==3 && (partyRoles&7)==7);
            checks[@"staleRejected"]=@(stale>=4); checks[@"nativeSummaryButtons"]=@(summaryClicks==7);
            checks[@"returnedField"]=@(self->_summaryPanel.hidden && self->_bagPanel.hidden && self->_partyPanel.hidden && lifetimes==2);
            finish(@"completed"); return;
        }
        if (peripheralButton) {
            NSString *selector=peripheralBag ? (peripheralKind==HG_BAG_UI_TOUCH_CANCEL ? @"bagNavigate:" : @"bagPressed:")
                : peripheralKind==HG_PARTY_UI_TOUCH_MON ? @"partyMonPressed:" : @"partyActionPressed:";
            CGPoint point=[peripheralButton convertPoint:CGPointMake(CGRectGetMidX(peripheralButton.bounds),CGRectGetMidY(peripheralButton.bounds)) toView:self.view];
            UIView *hit=[self.view hitTest:point withEvent:nil];
            if (peripheralButton.hidden || !peripheralButton.enabled || !peripheralButton.window
                || ![[peripheralButton actionsForTarget:self forControlEvent:UIControlEventTouchUpInside] containsObject:selector]
                || (hit!=peripheralButton && ![hit isDescendantOfView:peripheralButton])) { finish(@"native_peripheral_button_not_reachable"); return; }
            if ((peripheralBag && HGNativeBagUIIsTMHM(bag) && bag->actionCount==2)
                || (!peripheralBag && party->phase==HG_PARTY_UI_TM_YES_NO)) {
                UIImage *glyph=peripheralButton.configuration.image;
                if (glyph.size.width!=48 || glyph.size.height!=32 || ![self checkPrivatePixels:glyph]) { finish(@"native_original_yes_no_glyph_missing"); return; }
            }
        }
        if (capture) {
            if (![self checkPrivateCapture:capture kind:@"UIKit-summary-tm" image:nil]) { finish(@"capture_failed"); return; }
            [capturedNames addObject:capture];
        }
        if (peripheralButton) {
            uint32_t role=peripheralBag ? (peripheralKind==HG_BAG_UI_TOUCH_ACTION ? bag->actions[peripheralIndex].actionId : 0)
                : peripheralKind==HG_PARTY_UI_TOUCH_ACTION ? party->actions[peripheralIndex].textId : 0;
            NSMutableArray *words=[NSMutableArray new];for (NSUInteger i=0;i<sizeof(*q)/4;i++) [words addObject:@(((const uint32_t *)q)[i])];
            [events addObject:@{@"event":@"native-peripheral-button-sent",@"stage":@(originStage),@"nextStage":@(stage),@"owner":@(q->owner),@"generation":@(q->generation),
                @"observerRevision":@(q->revision),@"nativeRevision":@(peripheralBag ? bag->revision : party->revision),@"phase":@(q->phase),
                @"kind":@(peripheralKind),@"index":@(peripheralIndex),@"role":@(role),@"ready":@(peripheralBag ? bag->ready : party->ready),
                @"padReads":@(reads),@"hit":@[@(touchX),@(touchY)],@"qaWords":words}];
            [peripheralButton sendActionsForControlEvents:UIControlEventTouchUpInside];
            if (peripheralBag) { bagClicks++;bagKinds|=1u<<peripheralKind;if (role>=HG_BAG_UI_ACTION_TM_ACK && role<=HG_BAG_UI_ACTION_TM_NO) bagRoles|=1u<<(role-HG_BAG_UI_ACTION_TM_ACK); }
            else { partyClicks++;partyKinds|=1u<<peripheralKind;if (role>=HG_PARTY_UI_TEXT_TM_ACK && role<=HG_PARTY_UI_TEXT_TM_NO) partyRoles|=1u<<(role-HG_PARTY_UI_TEXT_TM_ACK); }
            typedRelease=YES;releaseStarted=NO;
        }
        if (action) {
            BOOL typed=summaryReady && (stage==TM_FIFTH_CONFIRM || stage==TM_STOP_NO || stage==TM_HM_RESULT
                || stage==TM_CONFIRM_BACK || stage==TM_OLD_AGAIN || stage==TM_CONFIRM_REPLACE || stage==TM_RETURN_BAG);
            action(); if (typed) { summaryClicks++; typedRelease=YES; releaseStarted=NO; }
        }
        dispatch_after(dispatch_time(DISPATCH_TIME_NOW,NSEC_PER_SEC/10),dispatch_get_main_queue(),step);
    }; step();
}
- (void)checkSummaryCandyCase {
    enum { C_OPEN_BAG=1,C_POCKET,C_ITEM,C_USE,C_CANCEL_TARGET,C_CANCEL_BAG,C_USE_AGAIN,C_TARGET,C_LEVEL_MESSAGE,
        C_STATS_DIFF,C_STATS_TOTAL,C_FORGET_YES,C_FIRST_SUMMARY,C_FIFTH_CONFIRM,C_STOP_NO,C_RETRY_YES,C_SECOND_SUMMARY,
        C_HM_RESULT,C_OLD_MOVE,C_CONFIRM_BACK,C_OLD_AGAIN,C_CONFIRM_REPLACE,C_RETURN_BAG,C_CLOSE_BAG,C_FIELD,C_FIELD_VISIBLE };
    NSArray *requiredKeys=@[@"caseBinding",@"muted",@"completeFixture",@"otNameMatchesProfile",@"targetCancelPreservesAll",@"targetReopenRejectsStale",@"originalLevelUp",@"originalStatsDifference",@"originalStatsTotal",@"statsDisplayCoherence",@"busyInputBlocked",@"nativeFiveMoveList",@"fifthCancelled",@"continuedTrying",@"secondSummaryLifetime",@"originalHMWarning",@"hmRejected",@"originalConfirm",@"confirmBack",@"replacementRazorLeaf",@"replacementPP",@"consumedOneCandy",@"ctx8ContinuationPreservesUpgrade",@"otherStatePreserved",@"rawLeaseReleased",@"staleRejected",@"nativeSummaryButtons",@"returnedField",@"privateSave4Preserved",@"resetQuiescent",@"capturesComplete",@"nativeBagButtons",@"nativePartyButtons"];
    NSArray *requiredPNGs=@[@"private-candy-target-cancel.png",@"private-candy-cancel-bag.png",@"private-candy-target-use.png",@"private-candy-level-up-message.png",@"private-candy-stats-difference.png",@"private-candy-stats-total.png",@"private-candy-forget-question.png",@"private-candy-first-list.png",@"private-candy-fifth-cancel.png",@"private-candy-stop-question.png",@"private-candy-retry-list.png",@"private-candy-hm-warning.png",@"private-candy-confirm.png",@"private-candy-back-to-list.png",@"private-candy-return-message.png",@"private-candy-final-bag.png",@"private-candy-field-return.png"];
    NSMutableDictionary *checks=[NSMutableDictionary new]; for (NSString *key in requiredKeys) checks[key]=@NO;
    NSMutableArray *events=[NSMutableArray new];
    NSURL *support=[NSFileManager.defaultManager URLsForDirectory:NSApplicationSupportDirectory inDomains:NSUserDomainMask].firstObject;
    support=[support URLByAppendingPathComponent:@"HeartGoldOriginal" isDirectory:YES];
    NSMutableDictionary *privateBefore=[NSMutableDictionary new];
    for (NSString *suffix in @[@"",@".console",@".wifi",@".lock"]) {
        NSString *name=[@"heartgold.sav" stringByAppendingString:suffix]; NSData *data=[NSData dataWithContentsOfURL:[support URLByAppendingPathComponent:name]];
        if (data) privateBefore[name]=data;
    }
    [_condition lock]; sPrivateUICase=@"summary-candy"; sPrivateCaptures=[NSMutableArray new]; sPrivateObserving=NO; [_condition unlock];
    __block NSUInteger stage=0,polls=0,lastStage=NSNotFound,lifetimes=0,stale=0,summaryClicks=0;
    __block uint32_t stageFrame=0,lastRevision=0,firstGeneration=0,firstRevision=0,oldRevision=0,hmCalls=0,releaseReads=0;
    __block BOOL finishing=NO,typedRelease=NO,releaseStarted=NO,busyOK=YES;
    __block NSUInteger busySamples=0,statsChangedPixels=0; __block uint32_t targetGeneration=0,targetRevision=0,fieldFrame=0;
    __block NSData *differencePixels=nil; NSMutableArray *statSamples=[NSMutableArray new];
    __block NSUInteger bagClicks=0,partyClicks=0; __block uint32_t bagKinds=0,bagRoles=0,partyKinds=0,partyRoles=0;
    NSMutableSet *capturedNames=[NSMutableSet new];
    __block void (^step)(void);
    BOOL (^initialUntouched)(NSDictionary *)=^BOOL(NSDictionary *facts) {
        return [facts[@"fixturePrepared"] boolValue] && ![facts[@"fixtureError"] length] && [facts[@"baseline26Preserved"] boolValue]
            && [facts[@"originalIdentityPreserved"] boolValue] && [facts[@"bagUnchanged"] boolValue] && [facts[@"flagsMoneyDexUnchanged"] boolValue]
            && [facts[@"candyQuantity"] unsignedIntValue]==2 && [facts[@"level"] unsignedIntValue]==5
            && [facts[@"experience"] isEqual:facts[@"initialExperience"]] && [facts[@"stats"] isEqual:facts[@"initialStats"]]
            && [facts[@"friendship"] unsignedIntValue]==70 && ![facts[@"mood"] unsignedIntValue] && [facts[@"moves"] isEqual:@[@33,@45,@115,@15]];
    };
    BOOL (^upgraded)(NSDictionary *)=^BOOL(NSDictionary *facts) {
        return [facts[@"fixturePrepared"] boolValue] && ![facts[@"fixtureError"] length] && [facts[@"expectedCandyDelta"] boolValue]
            && [facts[@"originalIdentityPreserved"] boolValue] && [facts[@"bagOnlyOneCandyConsumed"] boolValue] && [facts[@"flagsMoneyDexUnchanged"] boolValue]
            && [facts[@"candyQuantity"] unsignedIntValue]==1 && [facts[@"level"] unsignedIntValue]==6 && [facts[@"experience"] unsignedIntValue]==179
            && [facts[@"friendship"] unsignedIntValue]==75 && [facts[@"mood"] unsignedIntValue]==40
            && [facts[@"stats"] isEqual:facts[@"expectedStats"]] && [facts[@"moves"] isEqual:@[@33,@45,@115,@15]];
    };
    void (^finish)(NSString *)=^(NSString *status) {
        if (finishing) return; finishing=YES; step=nil;
        [self->_condition lock]; NSData *finalQA=[self->_summaryTMQAState copy]; NSDictionary *finalFacts=[self->_summaryTMFacts copy] ?: @{};
        uint32_t boots=self->_bootCount; self->_checkResetPending=YES; [self->_condition unlock];
        __block NSUInteger resetPolls=0; __block void (^resetStep)(void);
        resetStep=^{
            [self->_condition lock]; BOOL field=self->_bootCount==boots+1 && self->_fieldMapId==MAP_ROUTE_29 && self->_publishedFrames>self->_bootFrameBase;
            BOOL clean=self->_resetQuiesced && self->_restartClean,failed=self->_failure!=nil || self->_finished;
            NSDictionary *audio=self->_audioChanging ? nil : [self->_audio statistics]; [self->_condition unlock];
            if (!field && !failed && ++resetPolls<200) { dispatch_after(dispatch_time(DISPATCH_TIME_NOW,NSEC_PER_SEC/10),dispatch_get_main_queue(),resetStep); return; }
            checks[@"resetQuiescent"]=@(field && clean); checks[@"muted"]=@([checks[@"muted"] boolValue] && [audio[@"muted"] boolValue]);
            resetStep=nil; [self stop];
            __block NSUInteger stopPolls=0; __block void (^stopStep)(void);
            stopStep=^{
                [self->_condition lock]; BOOL stopped=self->_finished && !self->_env && !self->_game && !self->_wasi; [self->_condition unlock];
                if (!stopped && ++stopPolls<100) { dispatch_after(dispatch_time(DISPATCH_TIME_NOW,NSEC_PER_SEC/10),dispatch_get_main_queue(),stopStep); return; }
                checks[@"resetQuiescent"]=@([checks[@"resetQuiescent"] boolValue] && stopped);
                BOOL preserved=privateBefore.count==4;
                for (NSString *name in privateBefore) preserved&=[privateBefore[name] isEqualToData:[NSData dataWithContentsOfURL:[support URLByAppendingPathComponent:name]]];
                checks[@"privateSave4Preserved"]=@(preserved);
                NSMutableSet *files=[NSMutableSet new]; BOOL captures=sPrivateCaptures.count==requiredPNGs.count;
                for (NSDictionary *capture in sPrivateCaptures) { captures&=[capture[@"drawn"] boolValue] && [capture[@"written"] boolValue]; [files addObject:capture[@"file"]]; }
                captures&=[files isEqualToSet:[NSSet setWithArray:requiredPNGs]]; checks[@"capturesComplete"]=@(captures);
                BOOL passed=[status isEqual:@"completed"]; for (NSString *key in requiredKeys) passed&=[checks[key] boolValue];
                NSMutableArray *words=[NSMutableArray new]; if (finalQA.length==sizeof(HGSummaryLearnQAState))
                    for (NSUInteger i=0;i<finalQA.length/4;i++) [words addObject:@(((const uint32_t *)finalQA.bytes)[i])];
                NSDictionary *result=@{@"version":@1,@"case":@"summary-candy",@"complete":@(stopped),@"passed":@(passed),@"status":status,
                    @"stage":@(stage),@"polls":@(polls),@"checks":checks,@"requiredKeys":requiredKeys,@"requiredPNGs":requiredPNGs,
                    @"captures":[sPrivateCaptures copy],@"events":events,@"qaWordsBeforeReset":words,@"factsBeforeReset":finalFacts,
                    @"summaryLifetimes":@(lifetimes),@"staleRejected":@(stale),@"summaryButtonActions":@(summaryClicks),
                    @"privateSaveFileCount":@(privateBefore.count),@"privateRawPeripheralInputDiagnostic":@NO,@"newProductionAPI":@YES,
                    @"peripheralInputMode":@"native-UIKit-controls",@"bagButtonActions":@(bagClicks),@"partyButtonActions":@(partyClicks),
                    @"bagInputKinds":@(bagKinds),@"bagInputRoles":@(bagRoles),@"partyInputKinds":@(partyKinds),@"partyCandyRoles":@(partyRoles),
                    @"statSamples":statSamples,@"statsChangedPixels":@(statsChangedPixels),@"busySamples":@(busySamples),
                    @"scope":@"Actual native Candy target/cancel, original level5-to6 statistics, Summary cancel/HM/retry/replace and ctx8 continuation on one valid private starter. Uses the one read-only shipping HGNativePartyUIGetStats(revision) getter; Party1480/QA232 unchanged. UIKit button-sent events are not CQA accepted records. Seventeen captures; no evolution, other species/items, full-game, audible-audio or physical-device claim."};
                NSURL *folder=[NSFileManager.defaultManager URLsForDirectory:NSDocumentDirectory inDomains:NSUserDomainMask].firstObject;
                [[NSJSONSerialization dataWithJSONObject:result options:NSJSONWritingPrettyPrinted error:nil]
                    writeToURL:[folder URLByAppendingPathComponent:@"private-summary-candy-check.json"] options:NSDataWritingAtomic error:nil];
                stopStep=nil;
            }; stopStep();
        }; resetStep();
    };
    NSString *built=[NSBundle.mainBundle objectForInfoDictionaryKey:@"HGPrivateCheckCase"] ?: @"default";
    checks[@"caseBinding"]=@([built isEqual:@"summary-candy"] && [NSProcessInfo.processInfo.environment[@"HEARTGOLD_TEST_MUTE"] isEqual:@"1"]);
    if (![checks[@"caseBinding"] boolValue] || privateBefore.count!=4) { finish(@"case_or_private_save_binding_failed"); return; }
    step=^{
        [self->_condition lock]; NSData *qaData=[self->_summaryTMQAState copy],*ownerSummary=[self->_summaryUIState copy],*ownerBag=[self->_bagUIState copy],*ownerParty=[self->_partyUIState copy];
        NSDictionary *facts=[self->_summaryTMFacts copy] ?: @{}; UIImage *hmImage=self->_summaryTMHMWarningImage; NSData *mainFrame=[self->_frame copy]; BOOL menuOpen=self->_fieldMenuOpen;
        uint32_t map=self->_fieldMapId,reads=self->_summaryTMPadReads,bootStep=self->_summaryTMBootInputStep,bootFrame=self->_summaryTMBootInputFrame;
        BOOL failed=self->_failure!=nil || self->_finished,accepted=self->_summaryTMRequestAccepted,rawIdle=self->_summaryTMRawIdle;
        BOOL inputIdle=rawIdle && !self->_summaryChoicePending && !self->_summaryInputActive && !self->_bagChoicePending && !self->_bagTouchActive
            && !self->_partyChoicePending && !self->_partyTouchActive && !self->_keys && !self->_pendingKeys && !self->_submittedKeys && !self->_touchPressed;
        NSDictionary *audio=self->_audioChanging ? nil : [self->_audio statistics]; [self->_condition unlock];
        const HGSummaryLearnQAState *q=qaData.length==sizeof(*q) ? qaData.bytes : NULL;
        if (q && (q->version!=HG_SUMMARY_LEARN_QA_VERSION || q->byteSize!=sizeof(*q))) { finish(@"observer_abi_mismatch"); return; }
        if (failed || ++polls>3600 || events.count>=1024 || [facts[@"fixtureError"] length]) {
            finish(failed ? @"engine_failed" : [facts[@"fixtureError"] length] ? @"fixture_failed" : events.count>=1024 ? @"evidence_bound_exceeded" : @"flow_timeout"); return;
        }
        if (lastStage!=stage || (q && lastRevision!=q->revision)) {
            if (lastStage!=stage) { stageFrame=q ? q->frame : 0; lastStage=stage; }
            [events addObject:@{@"event":@"state",@"stage":@(stage),@"qaFrame":@(q ? q->frame : 0),@"revision":@(q ? q->revision : 0),
                @"owner":@(q ? q->owner : 0),@"generation":@(q ? q->generation : 0),@"phase":@(q ? q->phase : 0),
                @"context":@(q ? q->context : 0),@"taskState":@(q ? q->taskState : 0),@"padReads":@(reads),
                @"bootStep":@(bootStep),@"bootInputFrame":@(bootFrame)}]; lastRevision=q ? q->revision : 0;
        }
        if (q && stageFrame && q->frame-stageFrame>3600) { finish(@"original_scene_or_input_handshake_timeout"); return; }
        if (!q) { dispatch_after(dispatch_time(DISPATCH_TIME_NOW,NSEC_PER_SEC/10),dispatch_get_main_queue(),step); return; }
        if (typedRelease) {
            if (!inputIdle) releaseStarted=NO;
            else if (!releaseStarted) { releaseStarted=YES; releaseReads=reads; }
            else if (reads-releaseReads>=2) { typedRelease=NO; releaseStarted=NO; }
        }
        if (!inputIdle || typedRelease) { dispatch_after(dispatch_time(DISPATCH_TIME_NOW,NSEC_PER_SEC/10),dispatch_get_main_queue(),step); return; }
        const HGNativeSummaryUIState *summary=self->_displayedSummaryUIState.length==sizeof(*summary) ? self->_displayedSummaryUIState.bytes : NULL;
        const HGNativePartyUIState *party=self->_displayedPartyUIState.length==sizeof(*party) ? self->_displayedPartyUIState.bytes : NULL;
        const HGNativeBagUIState *bag=self->_displayedBagUIState.length==sizeof(*bag) ? self->_displayedBagUIState.bytes : NULL;
        BOOL summaryReady=summary && summary->active && summary->ready && HGNativeSummaryUIIsLearning(summary)
            && ownerSummary.length==sizeof(*summary) && !memcmp(ownerSummary.bytes,summary,sizeof(*summary))
            && q->owner==HG_SUMMARY_LEARN_SUMMARY && q->execState==2 && q->graphicsReady && q->fadeFinished;
        BOOL bagReady=bag && bag->active && bag->ready && !bag->mode && q->owner==HG_SUMMARY_LEARN_BAG && q->phase==bag->phase
            && q->execState==2 && q->graphicsReady && q->fadeFinished && ownerBag.length==sizeof(*bag) && !memcmp(ownerBag.bytes,bag,sizeof(*bag));
        BOOL partyReady=party && party->active && party->ready && HGNativePartyUIIsRareCandy(party) && q->owner==HG_SUMMARY_LEARN_PARTY
            && q->phase==party->phase && q->context==party->context && q->execState==2 && q->graphicsReady && q->fadeFinished
            && ownerParty.length==sizeof(*party) && !memcmp(ownerParty.bytes,party,sizeof(*party));
        BOOL candyScene=party && HGNativePartyUIIsRareCandy(party) && ownerParty.length==sizeof(*party) && !memcmp(ownerParty.bytes,party,sizeof(*party))
            && q->owner==HG_SUMMARY_LEARN_PARTY && q->phase==party->phase && q->context==party->context;
        if (candyScene && !party->ready) {
            BOOL blocked=!self->_partyPanel.hidden && self->_canvas.hidden && !self->_canvas.userInteractionEnabled
                && self->_buttons[@1].hidden && self->_buttons[@2].hidden && !self->_partyBack.enabled;
            for (UIButton *button in self->_partyMons) blocked&=!button.enabled;
            for (UIButton *button in self->_partyActions) blocked&=!button.enabled;
            busySamples++;busyOK&=blocked;
            if (!blocked) { finish(@"busy_candy_input_exposed");return; }
        }
        BOOL (^nativeStats)(uint32_t,uint32_t)=^BOOL(uint32_t role,uint32_t loop) {
            if (!partyReady || party->phase!=HG_PARTY_UI_ITEM_USE || party->context!=5 || q->itemId!=50 || q->taskState!=loop
                || party->actionCount!=1 || party->actions[0].textId!=role || self->_partyPanel.hidden || self->_partyBody.hidden
                || self->_partyBody.image.size.width!=128 || self->_partyBody.image.size.height!=112 || ![self checkPrivatePixels:self->_partyBody.image]
                || !self->_partyBack.hidden || !self->_canvas.hidden || self->_canvas.userInteractionEnabled
                || !self->_buttons[@1].hidden || !self->_buttons[@2].hidden || self->_partyActions[0].hidden || !self->_partyActions[0].enabled) return NO;
            for (UIButton *button in self->_partyMons) if (!button.hidden || button.enabled) return NO;
            uint32_t x,y;if (!HGNativePartyUIResolveTouch(party,party->revision,party->phase,HG_PARTY_UI_TOUCH_ACTION,0,&x,&y) || x!=124 || y!=168) return NO;
            NSArray *initial=facts[@"initialStats"],*expected=facts[@"expectedStats"],*statNames=@[@"HP",@"攻击",@"防御",@"特攻",@"特防",@"速度"];
            if (initial.count!=6 || expected.count!=6 || !self->_partyBody.isAccessibilityElement) return NO;
            BOOL difference=role==HG_PARTY_UI_TEXT_CANDY_STATS_DIFF;NSMutableArray *parts=[NSMutableArray new];
            for (NSUInteger i=0;i<6;i++) {
                uint32_t old=[initial[i] unsignedIntValue],now=[expected[i] unsignedIntValue];if (now<old) return NO;
                [parts addObject:[NSString stringWithFormat:@"%@ %@%u",statNames[i],difference ? @"+" : @"",difference ? now-old : now]];
            }
            if (![self->_partyBody.accessibilityLabel isEqual:difference ? @"能力变化" : @"当前能力值"]
                || ![self->_partyBody.accessibilityValue isEqual:[parts componentsJoinedByString:@"，"]]) return NO;
            UIImage *original=OriginalFrameRegion(mainFrame,0,0,128,112);
            return original && [self checkGlyph:original inCard:self->_partyBody.image x:0 y:0];
        };
        BOOL (^nativeSummary)(void)=^BOOL {
            if (!summaryReady || summary->mode!=2 || summary->monDataType!=1 || summary->partyCount!=1 || summary->partySlot
                || summary->moveToLearn!=75 || summary->page!=1 || self->_summaryPanel.hidden || !self->_canvas.hidden
                || self->_canvas.userInteractionEnabled || !self->_bagPanel.hidden || !self->_partyPanel.hidden
                || !self->_buttons[@1].hidden || !self->_buttons[@2].hidden || self->_summaryPreview.image.size.width!=256
                || self->_summaryPreview.image.size.height!=192 || self->_summaryContent.image.size.width!=120 || self->_summaryContent.image.size.height!=160
                || ![self checkPrivatePixels:self->_summaryPreview.image] || ![self checkPrivatePixels:self->_summaryContent.image]
                || self->_summaryNavigation[0].hidden || !self->_summaryNavigation[0].enabled || ![self checkSummaryRasterBack]) return NO;
            for (NSUInteger i=2;i<5;i++) if (!self->_summaryNavigation[i].hidden) return NO;
            NSArray *moves=facts[@"moves"],*pp=facts[@"pp"],*max=facts[@"maxPP"];
            if (moves.count!=4 || pp.count!=4 || max.count!=4) return NO;
            for (NSUInteger i=0;i<4;i++) {
                const HGNativeSummaryUIMove *move=&summary->moves[i];
                if (move->moveId!=[moves[i] unsignedIntValue] || move->pp!=[pp[i] unsignedIntValue] || move->maxPP!=[max[i] unsignedIntValue]
                    || !move->moveId || self->_summaryMoves[i].hidden || self->_summaryMoves[i].bounds.size.height<44
                    || self->_summaryMoveImages[i].image.size.width!=move->cropWidth || self->_summaryMoveImages[i].image.size.height!=move->cropHeight
                    || ![self checkPrivatePixels:self->_summaryMoveImages[i].image]) return NO;
                if (summary->phase==HG_SUMMARY_UI_FORGET_MOVE && !self->_summaryMoves[i].enabled) return NO;
            }
            return YES;
        };
        NSString *capture=nil; dispatch_block_t action=nil; HGNativeSummaryUIInput ignored;
        UIButton *peripheralButton=nil; BOOL peripheralBag=NO; uint32_t peripheralKind=0,peripheralIndex=0,touchX=0,touchY=0; NSUInteger originStage=stage;
        BOOL candy=partyReady && HGNativePartyUIIsRareCandy(party),target=candy && party->phase==HG_PARTY_UI_ITEM_TARGET;
        BOOL printer=candy && party->actions[0].textId==HG_PARTY_UI_TEXT_CANDY_ACK
            && (q->phase==HG_PARTY_UI_TM_MESSAGE || (q->phase==HG_PARTY_UI_ITEM_USE && (q->taskState==0 || q->taskState==4)))
            && q->printerId<8 && q->printerActive && q->printerMatches && q->printerState==2 && q->printerWaiting;
        if (printer && stage!=C_LEVEL_MESSAGE && HGNativePartyUIResolveTouch(party,party->revision,party->phase,HG_PARTY_UI_TOUCH_ACTION,0,&touchX,&touchY)) {
            peripheralButton=self->_partyActions[0];peripheralKind=HG_PARTY_UI_TOUCH_ACTION;
            if (stage==C_RETURN_BAG && q->context==8 && !q->itemId && [facts[@"expectedLearningDelta"] boolValue]) {
                checks[@"ctx8ContinuationPreservesUpgrade"]=@([facts[@"bagOnlyOneCandyConsumed"] boolValue] && [facts[@"candyQuantity"] unsignedIntValue]==1);
                if (![capturedNames containsObject:@"private-candy-return-message.png"]) capture=@"private-candy-return-message.png";
            }
        } else if (!stage && q->fieldReady && !q->owner && map==MAP_ROUTE_29 && bootStep==3 && !bootFrame) {
            checks[@"muted"]=@([audio[@"muted"] boolValue]);
            if ([self queueSummaryTMFixture:q->revision]) stage=C_OPEN_BAG;
        } else if (stage==C_OPEN_BAG && q->fieldReady && !q->owner && map==MAP_ROUTE_29 && accepted && [facts[@"fixturePrepared"] boolValue]) {
            BOOL fixture=[facts[@"fixtureCase"] isEqual:@"summary-candy"] && [facts[@"species"] unsignedIntValue]==152
                && [facts[@"partyCount"] unsignedIntValue]==1 && initialUntouched(facts);
            NSArray *pp=facts[@"pp"],*max=facts[@"maxPP"],*ups=facts[@"ppUps"];
            fixture&=pp.count==4 && max.count==4 && ups.count==4;
            for (NSUInteger i=0;i<MIN(pp.count,MIN(max.count,ups.count));i++) fixture&=[pp[i] unsignedIntValue]>0 && [pp[i] isEqual:max[i]] && ![ups[i] unsignedIntValue];
            BOOL ot=[facts[@"otNameProfileMatched"] boolValue] && [facts[@"otNameLength"] unsignedIntValue]>=1 && [facts[@"otNameLength"] unsignedIntValue]<=7
                && [facts[@"fixtureUsesExistingStarter"] boolValue] && [facts[@"originalIdentityPreserved"] boolValue] && [facts[@"mapSection"] unsignedIntValue];
            checks[@"completeFixture"]=@(fixture);checks[@"otNameMatchesProfile"]=@(ot);
            if (!fixture || !ot) { finish(@"original_candy_fixture_mismatch");return; }
            if (!menuOpen) action=^{ [self->_menuButton sendActionsForControlEvents:UIControlEventTouchUpInside]; };
            else if (!self->_fieldMenu.hidden && self->_fieldMenu.userInteractionEnabled) {
                for (NSUInteger i=0;i<7;i++) if (((self->_fieldMenuIcons>>(i*4))&15)==2 && [self->_fieldMenuElements[i] accessibilityActivate]) {stage=C_POCKET;break;}
            }
        } else if (stage==C_POCKET && bagReady && HGNativeBagUIIsList(bag)) {
            for (uint32_t i=0;i<HG_BAG_UI_POCKETS;i++) if (bag->pockets[i].enabled && bag->pockets[i].pocketId==POCKET_MEDICINE
                && HGNativeBagUIResolveTouch(bag,bag->revision,bag->phase,HG_BAG_UI_TOUCH_POCKET,i,&touchX,&touchY)) {
                peripheralButton=self->_bagPockets[i];peripheralBag=YES;peripheralKind=HG_BAG_UI_TOUCH_POCKET;peripheralIndex=i;stage=C_ITEM;break;
            }
        } else if ((stage==C_ITEM || stage==C_CANCEL_BAG) && bagReady && HGNativeBagUIIsList(bag) && bag->pocketId==POCKET_MEDICINE) {
            if (stage==C_CANCEL_BAG) {
                checks[@"targetCancelPreservesAll"]=@(initialUntouched(facts) && (!party || !party->active));
                if (![checks[@"targetCancelPreservesAll"] boolValue]) {finish(@"candy_cancel_changed_state");return;}
                capture=@"private-candy-cancel-bag.png";
            }
            for (uint32_t i=0;i<MIN(bag->visibleCount,HG_BAG_UI_ITEMS);i++) if (bag->items[i].enabled && bag->items[i].itemId==50 && bag->items[i].quantity==2
                && HGNativeBagUIResolveTouch(bag,bag->revision,bag->phase,HG_BAG_UI_TOUCH_ITEM,i,&touchX,&touchY)) {
                peripheralButton=self->_bagItems[i];peripheralBag=YES;peripheralKind=HG_BAG_UI_TOUCH_ITEM;peripheralIndex=i;stage=stage==C_ITEM ? C_USE : C_USE_AGAIN;break;
            }
        } else if ((stage==C_USE || stage==C_USE_AGAIN) && bagReady && bag->phase==HG_BAG_UI_ITEM_MENU && bag->selectedItemId==50) {
            for (uint32_t i=0;i<MIN(bag->actionCount,HG_BAG_UI_ACTIONS);i++) if (bag->actions[i].enabled && !bag->actions[i].actionId
                && HGNativeBagUIResolveTouch(bag,bag->revision,bag->phase,HG_BAG_UI_TOUCH_ACTION,i,&touchX,&touchY)) {
                peripheralButton=self->_bagActions[i];peripheralBag=YES;peripheralKind=HG_BAG_UI_TOUCH_ACTION;peripheralIndex=i;stage=stage==C_USE ? C_CANCEL_TARGET : C_TARGET;break;
            }
        } else if ((stage==C_CANCEL_TARGET || stage==C_TARGET) && target && q->context==5 && q->itemId==50 && !q->partyBusy) {
            if (!initialUntouched(facts)) {finish(@"candy_target_baseline_changed");return;}
            if (stage==C_CANCEL_TARGET && HGNativePartyUIResolveTouch(party,party->revision,party->phase,HG_PARTY_UI_TOUCH_CANCEL,0,&touchX,&touchY)) {
                targetGeneration=q->generation;targetRevision=party->revision;capture=@"private-candy-target-cancel.png";
                peripheralButton=self->_partyBack;peripheralKind=HG_PARTY_UI_TOUCH_CANCEL;stage=C_CANCEL_BAG;
            } else if (stage==C_TARGET && HGNativePartyUIResolveTouch(party,party->revision,party->phase,HG_PARTY_UI_TOUCH_MON,0,&touchX,&touchY)) {
                uint32_t ignoredX,ignoredY;
                BOOL fresh=q->generation>targetGeneration && party->revision>targetRevision
                    && !HGNativePartyUIResolveTouch(party,targetRevision,HG_PARTY_UI_ITEM_TARGET,HG_PARTY_UI_TOUCH_MON,0,&ignoredX,&ignoredY);
                checks[@"targetReopenRejectsStale"]=@(fresh);if (!fresh) {finish(@"old_candy_target_accepted");return;}stale++;
                capture=@"private-candy-target-use.png";peripheralButton=self->_partyMons[0];peripheralKind=HG_PARTY_UI_TOUCH_MON;stage=C_LEVEL_MESSAGE;
            }
        } else if (stage==C_LEVEL_MESSAGE && printer && q->context==5 && q->itemId==50 && q->phase==HG_PARTY_UI_ITEM_USE && q->taskState==0
            && HGNativePartyUIResolveTouch(party,party->revision,party->phase,HG_PARTY_UI_TOUCH_ACTION,0,&touchX,&touchY)) {
            checks[@"originalLevelUp"]=@(upgraded(facts));
            if (![checks[@"originalLevelUp"] boolValue]) {finish(@"original_candy_level_delta_mismatch");return;}
            capture=@"private-candy-level-up-message.png";peripheralButton=self->_partyActions[0];peripheralKind=HG_PARTY_UI_TOUCH_ACTION;stage=C_STATS_DIFF;
        } else if (stage==C_STATS_DIFF && nativeStats(HG_PARTY_UI_TEXT_CANDY_STATS_DIFF,1)) {
            if (!upgraded(facts)) {finish(@"candy_stats_difference_delta_mismatch");return;}
            CGImageRef body=self->_partyBody.image.CGImage;
            if (CGImageGetBytesPerRow(body)!=512) {finish(@"stats_pixel_stride_mismatch");return;}
            differencePixels=CFBridgingRelease(CGDataProviderCopyData(CGImageGetDataProvider(body)));
            checks[@"originalStatsDifference"]=@(differencePixels.length==128*112*4);
            if (![checks[@"originalStatsDifference"] boolValue]) {finish(@"stats_pixel_size_mismatch");return;}
            [statSamples addObject:@{@"role":@(party->actions[0].textId),@"revision":@(party->revision),@"phase":@(party->phase),
                @"loop":@(q->taskState),@"displayFrame":@(sPrivateDisplayFrame),@"qaFrame":@(q->frame),@"stats":facts[@"stats"],@"expectedStats":facts[@"expectedStats"]}];
            capture=@"private-candy-stats-difference.png";peripheralButton=self->_partyActions[0];peripheralKind=HG_PARTY_UI_TOUCH_ACTION;stage=C_STATS_TOTAL;
            HGNativePartyUIResolveTouch(party,party->revision,party->phase,peripheralKind,0,&touchX,&touchY);
        } else if (stage==C_STATS_TOTAL && nativeStats(HG_PARTY_UI_TEXT_CANDY_STATS_TOTAL,2)) {
            if (!upgraded(facts)) {finish(@"candy_stats_total_delta_mismatch");return;}
            CGImageRef body=self->_partyBody.image.CGImage;NSData *total=CFBridgingRelease(CGDataProviderCopyData(CGImageGetDataProvider(body)));
            BOOL geometry=CGImageGetBytesPerRow(body)==512 && total.length==differencePixels.length && total.length==128*112*4;
            if (geometry) for (NSUInteger y=8;y<104;y++) for (NSUInteger x=88;x<120;x++)
                if (memcmp((const uint8_t *)differencePixels.bytes+(y*128+x)*4,(const uint8_t *)total.bytes+(y*128+x)*4,4)) statsChangedPixels++;
            checks[@"originalStatsTotal"]=@(geometry);checks[@"statsDisplayCoherence"]=@(geometry && statsChangedPixels>=8);
            if (![checks[@"statsDisplayCoherence"] boolValue]) {finish(@"original_stats_pages_not_distinct");return;}
            [statSamples addObject:@{@"role":@(party->actions[0].textId),@"revision":@(party->revision),@"phase":@(party->phase),
                @"loop":@(q->taskState),@"displayFrame":@(sPrivateDisplayFrame),@"qaFrame":@(q->frame),@"stats":facts[@"stats"],@"expectedStats":facts[@"expectedStats"],@"changedNumberPixels":@(statsChangedPixels)}];
            capture=@"private-candy-stats-total.png";peripheralButton=self->_partyActions[0];peripheralKind=HG_PARTY_UI_TOUCH_ACTION;stage=C_FORGET_YES;
            HGNativePartyUIResolveTouch(party,party->revision,party->phase,peripheralKind,0,&touchX,&touchY);
        } else if ((stage==C_FORGET_YES || stage==C_STOP_NO || stage==C_RETRY_YES) && candy && q->phase==HG_PARTY_UI_TM_YES_NO
            && q->context==(stage==C_FORGET_YES ? 5u : 8u) && q->itemId==(stage==C_FORGET_YES ? 50u : 0u) && q->moveId==75
            && q->yesNoReady && q->yesNoActive==1 && q->yesNoResult==3 && !q->yesNoTimer
            && q->yesNoX==25 && q->yesNoY==10 && q->yesNoWidth==6 && q->yesNoHeight==4) {
            uint32_t row=stage==C_STOP_NO ? 1 : 0;
            if (!upgraded(facts) || (stage==C_STOP_NO && q->selectedMoveSlot!=4)) {finish(@"candy_cancel_upgrade_changed");return;}
            if (HGNativePartyUIResolveTouch(party,party->revision,party->phase,HG_PARTY_UI_TOUCH_ACTION,row,&touchX,&touchY)) {
                if (touchX!=q->yesHitX[row] || touchY!=q->yesHitY[row] || touchX!=224 || touchY!=96+32*row) {finish(@"candy_yes_no_geometry_mismatch");return;}
                if (stage==C_STOP_NO) {checks[@"fifthCancelled"]=@YES;capture=@"private-candy-stop-question.png";}
                else if (stage==C_FORGET_YES) capture=@"private-candy-forget-question.png";
                peripheralButton=self->_partyActions[row];peripheralKind=HG_PARTY_UI_TOUCH_ACTION;peripheralIndex=row;stage++;
            }
        } else if ((stage==C_FIRST_SUMMARY || stage==C_SECOND_SUMMARY) && summaryReady && summary->phase==HG_SUMMARY_UI_FORGET_MOVE) {
            BOOL native=nativeSummary() && summary->moveMask==31 && !self->_summaryNavigation[5].hidden && self->_summaryNavigation[5].enabled
                && [self checkPrivatePixels:self->_summaryNavigation[5].configuration.image] && self->_partyBody.hidden && !self->_partyBody.accessibilityValue.length;
            if (!native || !upgraded(facts)) {finish(@"native_candy_five_move_list_mismatch");return;}
            checks[@"nativeFiveMoveList"]=@(native && (!lifetimes || [checks[@"nativeFiveMoveList"] boolValue]));lifetimes++;
            if (stage==C_FIRST_SUMMARY) {
                firstGeneration=q->generation;firstRevision=summary->revision;capture=@"private-candy-first-list.png";
                if (HGNativeSummaryUIResolveInput(summary,summary->revision,summary->phase,HG_SUMMARY_UI_INPUT_MOVE,5,&ignored)) {finish(@"invalid_fifth_index_accepted");return;}stale++;
                action=^{[self->_summaryNavigation[5] sendActionsForControlEvents:UIControlEventTouchUpInside];};stage=C_FIFTH_CONFIRM;
            } else {
                if (q->generation<=firstGeneration || lifetimes!=2 || HGNativeSummaryUIResolveInput(summary,firstRevision,HG_SUMMARY_UI_FORGET_MOVE,HG_SUMMARY_UI_INPUT_MOVE,4,&ignored)) {finish(@"old_candy_summary_owner_accepted");return;}
                stale++;checks[@"continuedTrying"]=@YES;checks[@"secondSummaryLifetime"]=@YES;hmCalls=q->hmCalls;
                capture=@"private-candy-retry-list.png";action=^{[self->_summaryMoves[3] sendActionsForControlEvents:UIControlEventTouchUpInside];};stage=C_HM_RESULT;
            }
        } else if (stage==C_FIFTH_CONFIRM && summaryReady && summary->phase==HG_SUMMARY_UI_FORGET_MOVE && summary->moveSlot==4
            && !self->_summaryNavigation[1].hidden && self->_summaryNavigation[1].enabled) {
            if (!nativeSummary() || !upgraded(facts) || HGNativeSummaryUIResolveInput(summary,firstRevision,HG_SUMMARY_UI_FORGET_MOVE,HG_SUMMARY_UI_INPUT_MOVE,4,&ignored)) {finish(@"candy_fifth_or_stale_failed");return;}
            stale++;capture=@"private-candy-fifth-cancel.png";action=^{[self->_summaryNavigation[1] sendActionsForControlEvents:UIControlEventTouchUpInside];};stage=C_STOP_NO;
        } else if (stage==C_HM_RESULT && summaryReady && summary->phase==HG_SUMMARY_UI_FORGET_MOVE && summary->moveSlot==3 && q->hmCalls==hmCalls+1) {
            BOOL warning=nativeSummary() && q->hmOwner==q->data && q->hmMoveSlot==3 && summary->moves[3].moveId==15
                && q->hmWarning.pixelBuffer && q->hmWarning.tileWidth==15 && q->hmWarning.tileHeight==10 && q->hmWarning.engine==1 && !q->hmWarning.clearColor
                && hmImage.size.width==120 && hmImage.size.height==80 && [self checkPrivatePixels:hmImage] && [self checkGlyph:hmImage inCard:self->_summaryContent.image x:0 y:80];
            checks[@"originalHMWarning"]=@(warning);checks[@"hmRejected"]=@(warning && upgraded(facts));
            if (![checks[@"hmRejected"] boolValue]) {finish(@"original_candy_hm_warning_mismatch");return;}
            capture=@"private-candy-hm-warning.png";stage=C_OLD_MOVE;
        } else if ((stage==C_OLD_MOVE || stage==C_OLD_AGAIN) && summaryReady && summary->phase==HG_SUMMARY_UI_FORGET_MOVE && self->_summaryMoves[0].enabled) {
            if (!nativeSummary() || !upgraded(facts)) {finish(@"candy_move_before_confirmation_mismatch");return;}
            if (stage==C_OLD_AGAIN) {checks[@"confirmBack"]=@YES;capture=@"private-candy-back-to-list.png";}
            oldRevision=summary->revision;action=^{[self->_summaryMoves[0] sendActionsForControlEvents:UIControlEventTouchUpInside];};stage++;
        } else if ((stage==C_CONFIRM_BACK || stage==C_CONFIRM_REPLACE) && summaryReady && summary->phase==HG_SUMMARY_UI_FORGET_CONFIRM && summary->moveSlot==0) {
            BOOL confirm=nativeSummary() && !self->_summaryNavigation[1].hidden && self->_summaryNavigation[1].enabled && self->_summaryNavigation[5].hidden
                && [self checkPrivatePixels:self->_summaryNavigation[1].configuration.image];
            if (!confirm || !upgraded(facts) || HGNativeSummaryUIResolveInput(summary,oldRevision,HG_SUMMARY_UI_FORGET_MOVE,HG_SUMMARY_UI_INPUT_MOVE,0,&ignored)) {finish(@"candy_confirm_or_stale_mismatch");return;}
            stale++;checks[@"originalConfirm"]=@YES;
            if (stage==C_CONFIRM_BACK) {capture=@"private-candy-confirm.png";action=^{[self->_summaryNavigation[0] sendActionsForControlEvents:UIControlEventTouchUpInside];};}
            else action=^{[self->_summaryNavigation[1] sendActionsForControlEvents:UIControlEventTouchUpInside];};
            stage++;
        } else if (stage==C_RETURN_BAG && bagReady && HGNativeBagUIIsList(bag)) {
            NSArray *pp=facts[@"pp"],*max=facts[@"maxPP"],*ups=facts[@"ppUps"];
            BOOL learned=[facts[@"expectedLearningDelta"] boolValue] && [facts[@"moves"] isEqual:@[@75,@45,@115,@15]] && pp.count==4 && max.count==4 && ups.count==4
                && [pp[0] unsignedIntValue]==25 && [max[0] unsignedIntValue]==25 && ![ups[0] unsignedIntValue];
            BOOL others=[facts[@"flagsMoneyDexUnchanged"] boolValue] && [facts[@"originalIdentityPreserved"] boolValue] && [facts[@"bagOnlyOneCandyConsumed"] boolValue]
                && [facts[@"level"] unsignedIntValue]==6 && [facts[@"experience"] unsignedIntValue]==179 && [facts[@"friendship"] unsignedIntValue]==75 && [facts[@"mood"] unsignedIntValue]==40;
            BOOL item=NO;for (NSUInteger i=0;i<MIN(bag->visibleCount,HG_BAG_UI_ITEMS);i++) if (bag->items[i].itemId==50 && bag->items[i].quantity==1
                && self->_bagItems[i].enabled && [self checkBagItem:i state:bag]) item=YES;
            checks[@"replacementRazorLeaf"]=@(learned);checks[@"replacementPP"]=@(learned);checks[@"otherStatePreserved"]=@(others);
            checks[@"consumedOneCandy"]=@(item && [facts[@"candyQuantity"] unsignedIntValue]==1);
            if (!learned || !others || !item || ![checks[@"ctx8ContinuationPreservesUpgrade"] boolValue] || (summary && summary->active) || (party && party->active)) {finish(@"original_candy_learning_delta_mismatch");return;}
            capture=@"private-candy-final-bag.png";stage=C_CLOSE_BAG;
        } else if (stage==C_CLOSE_BAG && bagReady && HGNativeBagUIIsList(bag)) {
            if (HGNativeBagUIResolveTouch(bag,bag->revision,bag->phase,HG_BAG_UI_TOUCH_CANCEL,0,&touchX,&touchY)) {
                peripheralButton=self->_bagNavigation[0];peripheralBag=YES;peripheralKind=HG_BAG_UI_TOUCH_CANCEL;stage=C_FIELD;
            }
        } else if (stage==C_FIELD && q->fieldReady && !q->owner && map==MAP_ROUTE_29 && (!bag || !bag->active) && (!party || !party->active) && (!summary || !summary->active)) {
            if (menuOpen) [self toggleMenu];fieldFrame=sPrivateDisplayFrame;stage=C_FIELD_VISIBLE;
        } else if (stage==C_FIELD_VISIBLE && q->fieldReady && !q->owner && map==MAP_ROUTE_29 && !menuOpen && sPrivateDisplayFrame>fieldFrame
            && !self->_canvas.hidden && self->_canvas.userInteractionEnabled && self->_partyPanel.hidden && self->_bagPanel.hidden && self->_summaryPanel.hidden
            && self->_battleMenuPanel.hidden && self->_fieldMenu.hidden && self->_fieldMenuShade.hidden && !self->_joystick.hidden && self->_joystick.userInteractionEnabled
            && !self->_menuButton.hidden && !self->_buttons[@1].hidden && !self->_buttons[@2].hidden && [self->_menuButton.currentTitle isEqual:@"菜单"]
            && !self->_menuButton.titleLabel.hidden && self->_menuButton.titleLabel.alpha==1) {
            [self->_condition lock];NSData *geometry=[self->_geometryState copy];[self->_condition unlock];const uint32_t *scene=geometry.bytes;CGImageRef image=self->_canvas.image.CGImage;
            if (geometry.length==8*sizeof(uint32_t) && scene[0] && scene[1] && image && [self checkPrivatePixels:self->_canvas.image]
                && CGImageGetWidth(image)>256 && self->_displayedWidth==CGImageGetWidth(image)) {
                checks[@"rawLeaseReleased"]=@(rawIdle && inputIdle && !typedRelease && ![facts[@"rawPresses"] unsignedIntValue] && ![facts[@"rawReleases"] unsignedIntValue] && ![facts[@"rawRejected"] unsignedIntValue]);
                checks[@"nativeBagButtons"]=@(bagClicks>=6 && (bagKinds&0x27)==0x27);checks[@"nativePartyButtons"]=@(partyClicks>=8 && (partyKinds&7)==7 && (partyRoles&31)==31);
                checks[@"busyInputBlocked"]=@(busySamples>0 && busyOK);checks[@"staleRejected"]=@(stale>=5);checks[@"nativeSummaryButtons"]=@(summaryClicks==7);checks[@"returnedField"]=@YES;
                [events addObject:@{@"event":@"field-visible",@"displayFrame":@(sPrivateDisplayFrame),@"afterMenuCloseFrame":@(fieldFrame),
                    @"canvasWidth":@(CGImageGetWidth(image)),@"vertices":@(scene[0]),@"polygons":@(scene[1])}];
                capture=@"private-candy-field-return.png";stage=99;
            }
        }
        if (peripheralButton) {
            NSString *selector=peripheralBag ? (peripheralKind==HG_BAG_UI_TOUCH_CANCEL ? @"bagNavigate:" : @"bagPressed:")
                : peripheralKind==HG_PARTY_UI_TOUCH_MON ? @"partyMonPressed:" : peripheralKind==HG_PARTY_UI_TOUCH_CANCEL ? @"partyBackPressed:" : @"partyActionPressed:";
            CGPoint point=[peripheralButton convertPoint:CGPointMake(CGRectGetMidX(peripheralButton.bounds),CGRectGetMidY(peripheralButton.bounds)) toView:self.view];
            UIView *hit=[self.view hitTest:point withEvent:nil];
            if (peripheralButton.hidden || !peripheralButton.enabled || !peripheralButton.window
                || ![[peripheralButton actionsForTarget:self forControlEvent:UIControlEventTouchUpInside] containsObject:selector]
                || (hit!=peripheralButton && ![hit isDescendantOfView:peripheralButton])) { finish(@"native_peripheral_button_not_reachable"); return; }
            if (!peripheralBag && party->phase==HG_PARTY_UI_TM_YES_NO) {
                UIImage *glyph=peripheralButton.configuration.image;
                if (glyph.size.width!=48 || glyph.size.height!=32 || ![self checkPrivatePixels:glyph]) { finish(@"native_original_yes_no_glyph_missing"); return; }
            }
        }
        if (capture) {
            if (![self checkPrivateCapture:capture kind:@"UIKit-summary-candy" image:nil]) { finish(@"capture_failed"); return; }
            [capturedNames addObject:capture];
        }
        if (peripheralButton) {
            uint32_t role=peripheralBag ? (peripheralKind==HG_BAG_UI_TOUCH_ACTION ? bag->actions[peripheralIndex].actionId : 0)
                : peripheralKind==HG_PARTY_UI_TOUCH_ACTION ? party->actions[peripheralIndex].textId : 0;
            NSMutableArray *words=[NSMutableArray new];for (NSUInteger i=0;i<sizeof(*q)/4;i++) [words addObject:@(((const uint32_t *)q)[i])];
            [events addObject:@{@"event":@"native-peripheral-button-sent",@"stage":@(originStage),@"nextStage":@(stage),@"owner":@(q->owner),@"generation":@(q->generation),
                @"observerRevision":@(q->revision),@"nativeRevision":@(peripheralBag ? bag->revision : party->revision),@"phase":@(q->phase),
                @"kind":@(peripheralKind),@"index":@(peripheralIndex),@"role":@(role),@"ready":@(peripheralBag ? bag->ready : party->ready),
                @"padReads":@(reads),@"hit":@[@(touchX),@(touchY)],@"qaWords":words}];
            [peripheralButton sendActionsForControlEvents:UIControlEventTouchUpInside];
            if (peripheralBag) { bagClicks++;bagKinds|=1u<<peripheralKind;if (role>=HG_BAG_UI_ACTION_TM_ACK && role<=HG_BAG_UI_ACTION_TM_NO) bagRoles|=1u<<(role-HG_BAG_UI_ACTION_TM_ACK); }
            else { partyClicks++;partyKinds|=1u<<peripheralKind;if (role>=HG_PARTY_UI_TEXT_CANDY_ACK && role<=HG_PARTY_UI_TEXT_CANDY_NO) partyRoles|=1u<<(role-HG_PARTY_UI_TEXT_CANDY_ACK); }
            typedRelease=YES;releaseStarted=NO;
        }
        if (action) {
            BOOL typed=summaryReady && (stage==C_FIFTH_CONFIRM || stage==C_STOP_NO || stage==C_HM_RESULT
                || stage==C_CONFIRM_BACK || stage==C_OLD_AGAIN || stage==C_CONFIRM_REPLACE || stage==C_RETURN_BAG);
            action(); if (typed) { summaryClicks++; typedRelease=YES; releaseStarted=NO; }
        }
        if (stage==99) {finish(@"completed");return;}
        dispatch_after(dispatch_time(DISPATCH_TIME_NOW,NSEC_PER_SEC/10),dispatch_get_main_queue(),step);
    }; step();
}
#endif
- (void)checkOptionsControls:(NSMutableDictionary *)report {
    // Synthetic geometry/input only. These RGBA bytes are not original-asset or gameplay evidence.
    report[@"optionsSyntheticBeforeOwner"]=@(NSThread.isMainThread && !_game && !_env);
    if (_game || _env) return;
    NSData *oldFrame=_frame,*oldMenuFrame=_fieldMenuFrame,*oldOptions=_optionsUIState,*oldDisplayed=_displayedOptionsUIState;
#ifdef HEARTGOLD_ORIGINAL_DEFAULT_CHECK
    NSData *oldQA=_battleMenusQAState,*oldDisplayedQA=_displayedBattleMenusQAState;
    BOOL oldFade=_paletteFadeFinished,oldDisplayedFade=_displayedPaletteFadeFinished;
#endif
    UIImage *oldCanvas=_canvas.image,*oldPreview=_optionsPreviewImage,*oldPreviewShown=_optionsPreview.image;
    NSString *oldCanvasLabel=_canvas.accessibilityLabel,*oldCanvasValue=_canvas.accessibilityValue,*oldStickValue=_joystick.accessibilityValue,*oldStatus=_status.text;
    NSArray *oldMenuElements=_fieldMenu.accessibilityElements; NSMutableArray *oldCompletions=_checkInputCompletions;
    CGPoint oldThumb=_joystickThumb.center; BOOL oldCanvasElement=_canvas.isAccessibilityElement,oldStatusHidden=_status.hidden;
    struct {
        uint32_t keys,pending,submitted,stick,pendingStick,submittedStick,x,y,selected,frameEngine,displayedEngine,width,displayedWidth,map,icons,touchEngine,reads,viewWidth,battleWidth;
        NSUInteger inputGeneration;
        BOOL paused,inactive,interrupted,ended,stopping,touch,resetStick,menu,automatic;
    } old={_keys,_pendingKeys,_submittedKeys,_joystickKeys,_pendingJoystickKeys,_submittedJoystickKeys,_touchX,_touchY,_selectedEngine,_frameEngine,_displayedEngine,_frameWidth,_displayedWidth,_fieldMapId,_fieldMenuIcons,_touchEngine,_lastPadReadCount,_preferredViewWidth,_preferredBattleWidth,
        _inputGeneration,_paused,_inactive,_interrupted,_interruptionEnded,_stopping,_touchPressed,_joystickNeedsReset,_fieldMenuOpen,_automaticMenuEngine};
    HGNativeOptionsUIState oldChoice=_optionsChoice,oldInput=_optionsInput;
    struct {uint32_t owner,choiceKeys,inputKeys,inputReads,releaseReads;BOOL scene,pending,active,cooling;} oldLease={_optionsOwnerGeneration,_optionsChoiceKeys,_optionsInputKeys,_optionsInputReads,_optionsReleaseReads,_optionsScene,_optionsChoicePending,_optionsInputActive,_optionsInputCooling};
    NSArray *views=@[_canvas,_optionsPreview,_status,_battleBar,_partyPanel,_partyBack,_bagPanel,_bagPage,_summaryPanel,_battleMenuPanel,_pokedexPanel,_fieldMenu,_fieldMenuShade,_joystick,_menuButton];
    NSMutableArray *viewState=[NSMutableArray new],*buttonState=[NSMutableArray new],*gestureState=[NSMutableArray new];
    for (UIGestureRecognizer *gesture in _canvas.gestureRecognizers) [gestureState addObject:@(gesture.enabled)];
    for (UIView *view in views) [viewState addObject:@{@"hidden":@(view.hidden),@"interaction":@(view.userInteractionEnabled),@"frame":[NSValue valueWithCGRect:view.frame]}];
    NSArray *buttons=[[_buttons.allValues arrayByAddingObject:_menuButton] arrayByAddingObjectsFromArray:[_bagNavigation arrayByAddingObjectsFromArray:_summaryNavigation]];
    for (UIButton *button in buttons) [buttonState addObject:@{@"title":[button titleForState:UIControlStateNormal] ?: NSNull.null,@"configuration":button.configuration ?: NSNull.null,
        @"label":button.accessibilityLabel ?: NSNull.null,@"hint":button.accessibilityHint ?: NSNull.null,@"hidden":@(button.hidden),@"enabled":@(button.enabled),@"alpha":@(button.alpha),@"frame":[NSValue valueWithCGRect:button.frame],
        @"generation":@([button isKindOfClass:OriginalKeyButton.class] ? ((OriginalKeyButton *)button).inputGeneration : 0)}];
    HGNativeOptionsUIState ready={1,7,11,1},zero={0};
    NSData *readyData=[NSData dataWithBytes:&ready length:sizeof(ready)],*zeroData=[NSData dataWithBytes:&zero length:sizeof(zero)];
    NSData *pixels=[NSMutableData dataWithLength:256*192*4]; UIImage *preview=OriginalFrameRegion(pixels,0,144,256,48);
    [_condition lock]; [self clearOptionsInputLocked]; _paused=_inactive=_interrupted=_interruptionEnded=_stopping=NO;
    _optionsOwnerGeneration=ready.ownerGeneration; _optionsUIState=readyData; _optionsPreviewImage=preview;
    _frame=pixels; _frameWidth=256; _frameEngine=0; _selectedEngine=_displayedEngine=1;
#ifdef HEARTGOLD_ORIGINAL_DEFAULT_CHECK
    HGNativeBattleMenusQAState qa={.version=HG_BATTLE_MENUS_QA_VERSION,.byteSize=sizeof(qa),.revision=17,.fieldReady=1,.fieldFrame=31};
    NSData *qaData=[NSData dataWithBytes:&qa length:sizeof(qa)]; _battleMenusQAState=qaData; _paletteFadeFinished=YES;
#endif
    _fieldMapId=MAP_ROUTE_29; _fieldMenuOpen=YES; _checkInputCompletions=[NSMutableArray new]; [_condition unlock];
    [self displayFrame]; [self viewDidLayoutSubviews];
#ifdef HEARTGOLD_ORIGINAL_DEFAULT_CHECK
    BOOL paired=_displayedBattleMenusQAState==qaData && _displayedPaletteFadeFinished;
    [_condition lock]; _battleMenusQAState=nil; _paletteFadeFinished=NO; [_condition unlock]; [self displayFrame];
    paired&=_displayedBattleMenusQAState==qaData && _displayedPaletteFadeFinished;
    [_condition lock]; _frame=pixels; [_condition unlock]; [self displayFrame];
    paired&=!_displayedBattleMenusQAState && !_displayedPaletteFadeFinished;
#endif
    report[@"optionsSyntheticPublishedCanvas"]=@(_selectedEngine==1 && !_displayedEngine && !_frameEngine && _displayedWidth==256
        && [_displayedOptionsUIState isEqualToData:readyData] && _canvas.image.size.width==256 && _canvas.image.size.height==192
        && _optionsPreview.image==preview && preview.size.width==256 && preview.size.height==48 && !_canvas.hidden && !_optionsPreview.hidden
        && fabs(_canvas.bounds.size.width*3-_canvas.bounds.size.height*4)<1 && fabs(_optionsPreview.bounds.size.width*48-_optionsPreview.bounds.size.height*256)<1
        && fabs(_canvas.frame.origin.x-_optionsPreview.frame.origin.x)<1 && fabs(_canvas.bounds.size.width-_optionsPreview.bounds.size.width)<1
        && CGRectGetMaxY(_canvas.frame)<=CGRectGetMinY(_optionsPreview.frame) && !_canvas.userInteractionEnabled && !_optionsPreview.userInteractionEnabled);
#ifdef HEARTGOLD_ORIGINAL_DEFAULT_CHECK
    report[@"optionsSyntheticPublishedCanvas"]=@([report[@"optionsSyntheticPublishedCanvas"] boolValue] && paired);
#endif
    BOOL visible=_optionsScene && _battleBar.hidden && _partyPanel.hidden && _partyBack.hidden && _bagPanel.hidden && _bagPage.hidden && _summaryPanel.hidden && _battleMenuPanel.hidden && _pokedexPanel.hidden
        && _fieldMenu.hidden && _fieldMenuShade.hidden && _joystick.hidden && _menuButton.hidden && _buttons[@8].hidden && _buttons[@0x400].hidden;
    for (UIButton *button in [_bagNavigation arrayByAddingObjectsFromArray:_summaryNavigation]) visible&=button.hidden;
    for (NSNumber *key in @[@0x40,@0x80,@0x20,@0x10,@1,@2]) visible&=!_buttons[key].hidden && _buttons[key].enabled;
    report[@"optionsSyntheticReadyControls"]=@(visible);
    uint32_t keys=0; BOOL stale=YES;
    for (NSUInteger i=0;i<2;i++) {
        HGNativeOptionsUIState changed=ready; if (i) changed.revision++; else changed.ownerGeneration++;
        _optionsUIState=[NSData dataWithBytes:&changed length:sizeof(changed)]; [self keyDown:(OriginalKeyButton *)_buttons[@1]];
        [_condition lock]; stale&=!_optionsChoicePending && !_optionsInputActive;
        _optionsUIState=readyData; [_condition unlock]; [self keyDown:(OriginalKeyButton *)_buttons[@1]];
        [_condition lock]; stale&=_optionsChoicePending; [self takeOptionsInputLocked:&changed reads:50+(uint32_t)i keys:&keys];
        stale&=!keys && !_optionsChoicePending && !_optionsInputActive; [self clearOptionsInputLocked]; [_condition unlock];
    }
    report[@"optionsSyntheticRejectsStaleTuple"]=@(stale);
    BOOL leases=YES;
    for (NSNumber *key in @[@1,@2]) {
        uint32_t begin=key.unsignedIntValue==1 ? 100 : 120;
        [self updateOptionsControls:readyData preview:preview paused:NO]; [self keyDown:(OriginalKeyButton *)_buttons[key]];
        [_condition lock]; BOOL queued=_optionsChoicePending;
        [self takeOptionsInputLocked:&ready reads:begin keys:&keys]; BOOL started=keys==key.unsignedIntValue && _optionsInputActive;
        [self takeOptionsInputLocked:&ready reads:begin+3 keys:&keys]; BOOL held=keys==key.unsignedIntValue;
        [self takeOptionsInputLocked:&ready reads:begin+4 keys:&keys]; BOOL released=!keys && !_optionsInputActive && _optionsInputCooling;
        [self takeOptionsInputLocked:&ready reads:begin+5 keys:&keys]; BOOL cooling=!keys && _optionsInputCooling;
        [self takeOptionsInputLocked:&ready reads:begin+6 keys:&keys]; leases&=queued && started && held && released && cooling && !keys && !_optionsInputCooling && !_optionsChoicePending;
        [_condition unlock];
    }
    report[@"optionsSyntheticABFourPadLease"]=@(leases);
    HGNativeOptionsUIState busy=ready; busy.ready=0; busy.revision++;
    NSData *busyData=[NSData dataWithBytes:&busy length:sizeof(busy)]; _optionsUIState=busyData;
    [self updateOptionsControls:busyData preview:preview paused:NO]; [self keyDown:(OriginalKeyButton *)_buttons[@1]];
    BOOL blocked=_optionsScene && !_canvas.hidden && !_optionsPreview.hidden;
    for (NSNumber *key in @[@0x40,@0x80,@0x20,@0x10,@1,@2]) blocked&=!_buttons[key].hidden && !_buttons[key].enabled;
    [_condition lock]; [self takeOptionsInputLocked:&busy reads:140 keys:&keys]; blocked&=!keys && !_optionsChoicePending && !_optionsInputActive; [_condition unlock];
    report[@"optionsSyntheticBusyBlocksInput"]=@(blocked);
    _optionsUIState=readyData; [self updateOptionsControls:readyData preview:preview paused:NO];
    [_condition lock]; _keys=_pendingKeys=_submittedKeys=_joystickKeys=_pendingJoystickKeys=_submittedJoystickKeys=_touchX=_touchY=0; _touchPressed=NO;
    NSUInteger generation=_inputGeneration; [_condition unlock];
    [self toggleMenu]; [self tapAt:CGPointMake(CGRectGetMidX(_canvas.bounds),CGRectGetMidY(_canvas.bounds))];
    for (dispatch_block_t completion in [_checkInputCompletions copy]) completion();
    report[@"optionsSyntheticExclusiveInput"]=@(_selectedEngine==1 && !_frameEngine && !_displayedEngine && _fieldMenuOpen && _inputGeneration==generation
        && !_keys && !_pendingKeys && !_submittedKeys && !_touchPressed && !_touchX && !_touchY && !_optionsChoicePending && _checkInputCompletions.count==1);
    [self keyDown:(OriginalKeyButton *)_buttons[@1]]; [_condition lock]; _lastPadReadCount=200; BOOL beforePause=_optionsChoicePending; [_condition unlock];
    [self pause]; [self keyDown:(OriginalKeyButton *)_buttons[@2]];
    [_condition lock]; BOOL paused=beforePause && _paused && !_optionsChoicePending && !_optionsInputActive && _optionsInputCooling; [_condition unlock]; [self resume];
    [_condition lock]; [self takeOptionsInputLocked:&ready reads:201 keys:&keys]; paused&=!keys && _optionsInputCooling;
    [self takeOptionsInputLocked:&ready reads:202 keys:&keys]; paused&=!keys && !_optionsInputCooling; [_condition unlock];
    [self updateOptionsControls:readyData preview:preview paused:NO]; [self keyDown:(OriginalKeyButton *)_buttons[@2]];
    _optionsUIState=zeroData; _optionsPreviewImage=nil; _optionsOwnerGeneration=0; [self updateOptionsControls:zeroData preview:nil paused:NO];
    [_condition lock]; [self takeOptionsInputLocked:&zero reads:210 keys:&keys];
    BOOL retired=!keys && !_optionsScene && !_optionsChoicePending && !_optionsInputActive && !_optionsInputCooling && !_optionsOwnerGeneration
        && !_optionsChoiceKeys && !_optionsInputKeys && !_optionsInputReads && !_optionsReleaseReads
        && !memcmp(&_optionsChoice,&zero,sizeof(zero)) && !memcmp(&_optionsInput,&zero,sizeof(zero))
        && [_optionsUIState isEqualToData:zeroData] && [_displayedOptionsUIState isEqualToData:zeroData] && !_optionsPreviewImage;
    [_condition unlock]; report[@"optionsSyntheticRetireAndPauseClear"]=@(paused && retired && _optionsPreview.hidden && !_optionsPreview.image);
    [_condition lock];
#ifdef HEARTGOLD_ORIGINAL_DEFAULT_CHECK
    _battleMenusQAState=oldQA; _displayedBattleMenusQAState=oldDisplayedQA; _paletteFadeFinished=oldFade; _displayedPaletteFadeFinished=oldDisplayedFade;
#endif
    _optionsUIState=oldOptions; _displayedOptionsUIState=oldDisplayed; _optionsPreviewImage=oldPreview; _optionsChoice=oldChoice; _optionsInput=oldInput;
    _optionsOwnerGeneration=oldLease.owner; _optionsChoiceKeys=oldLease.choiceKeys; _optionsInputKeys=oldLease.inputKeys; _optionsInputReads=oldLease.inputReads; _optionsReleaseReads=oldLease.releaseReads;
    _optionsScene=oldLease.scene; _optionsChoicePending=oldLease.pending; _optionsInputActive=oldLease.active; _optionsInputCooling=oldLease.cooling;
    _frame=oldFrame; _fieldMenuFrame=oldMenuFrame; _keys=old.keys; _pendingKeys=old.pending; _submittedKeys=old.submitted;
    _joystickKeys=old.stick; _pendingJoystickKeys=old.pendingStick; _submittedJoystickKeys=old.submittedStick; _touchX=old.x; _touchY=old.y;
    _selectedEngine=old.selected; _frameEngine=old.frameEngine; _displayedEngine=old.displayedEngine; _frameWidth=old.width; _displayedWidth=old.displayedWidth;
    _fieldMapId=old.map; _fieldMenuIcons=old.icons; _touchEngine=old.touchEngine; _lastPadReadCount=old.reads; _preferredViewWidth=old.viewWidth; _preferredBattleWidth=old.battleWidth;
    _inputGeneration=old.inputGeneration; _paused=old.paused; _inactive=old.inactive; _interrupted=old.interrupted; _interruptionEnded=old.ended; _stopping=old.stopping;
    _touchPressed=old.touch; _joystickNeedsReset=old.resetStick; _fieldMenuOpen=old.menu; _automaticMenuEngine=old.automatic; _checkInputCompletions=oldCompletions; [_condition unlock];
    _canvas.image=oldCanvas; _canvas.accessibilityLabel=oldCanvasLabel; _canvas.accessibilityValue=oldCanvasValue; _canvas.isAccessibilityElement=oldCanvasElement;
    _optionsPreview.image=oldPreviewShown; _joystick.accessibilityValue=oldStickValue; _joystickThumb.center=oldThumb; _status.text=oldStatus; _status.hidden=oldStatusHidden; _fieldMenu.accessibilityElements=oldMenuElements;
    for (NSUInteger i=0;i<gestureState.count;i++) _canvas.gestureRecognizers[i].enabled=[gestureState[i] boolValue];
    for (NSUInteger i=0;i<views.count;i++) {UIView *view=views[i]; NSDictionary *value=viewState[i]; view.hidden=[value[@"hidden"] boolValue]; view.userInteractionEnabled=[value[@"interaction"] boolValue]; view.frame=[value[@"frame"] CGRectValue];}
    for (NSUInteger i=0;i<buttons.count;i++) {UIButton *button=buttons[i]; NSDictionary *value=buttonState[i];
        button.configuration=value[@"configuration"]==NSNull.null ? nil : value[@"configuration"]; [button setTitle:value[@"title"]==NSNull.null ? nil : value[@"title"] forState:UIControlStateNormal];
        button.accessibilityLabel=value[@"label"]==NSNull.null ? nil : value[@"label"]; button.accessibilityHint=value[@"hint"]==NSNull.null ? nil : value[@"hint"];
        button.hidden=[value[@"hidden"] boolValue]; button.enabled=[value[@"enabled"] boolValue]; button.alpha=[value[@"alpha"] doubleValue]; button.frame=[value[@"frame"] CGRectValue];
        if ([button isKindOfClass:OriginalKeyButton.class]) ((OriginalKeyButton *)button).inputGeneration=[value[@"generation"] unsignedIntegerValue];
    }
    report[@"optionsSyntheticRestoresTransientState"]=@(_frame==oldFrame && _fieldMenuFrame==oldMenuFrame && _optionsUIState==oldOptions && _displayedOptionsUIState==oldDisplayed && _optionsPreviewImage==oldPreview
        && _selectedEngine==old.selected && _frameEngine==old.frameEngine && _displayedEngine==old.displayedEngine && _inputGeneration==old.inputGeneration && _lastPadReadCount==old.reads
        && _optionsOwnerGeneration==oldLease.owner && _optionsScene==oldLease.scene && _optionsChoicePending==oldLease.pending && _optionsInputActive==oldLease.active && _optionsInputCooling==oldLease.cooling
        && !memcmp(&_optionsChoice,&oldChoice,sizeof(oldChoice)) && !memcmp(&_optionsInput,&oldInput,sizeof(oldInput)) && _checkInputCompletions==oldCompletions);
#ifdef HEARTGOLD_ORIGINAL_DEFAULT_CHECK
    report[@"optionsSyntheticRestoresTransientState"]=@([report[@"optionsSyntheticRestoresTransientState"] boolValue]
        && _battleMenusQAState==oldQA && _displayedBattleMenusQAState==oldDisplayedQA && _paletteFadeFinished==oldFade && _displayedPaletteFadeFinished==oldDisplayedFade);
#endif
}
- (void)checkControls {
    NSString *privateCase=NSProcessInfo.processInfo.environment[@"HEARTGOLD_CHECK_CASE"];
#ifdef HEARTGOLD_ORIGINAL_SUMMARY_TM_CHECK
    if ([privateCase isEqual:@"summary-tm"]) { [self checkSummaryTMCase]; return; }
    if ([privateCase isEqual:@"summary-candy"]) { [self checkSummaryCandyCase]; return; }
#endif
    if ([privateCase isEqual:@"field-potion"] || [privateCase isEqual:@"forced-switch"] || [privateCase isEqual:@"learn-moves"]) { [self checkPrivateNativeCase:privateCase]; return; }
    NSMutableDictionary *report = [NSMutableDictionary new];
    [self checkOptionsControls:report];
#ifdef HEARTGOLD_ORIGINAL_DEFAULT_CHECK
    [report addEntriesFromDictionary:[self checkPCBoxHostPrimitives]];
#endif
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
    uint32_t savedFieldMap=_fieldMapId,savedEngine=_selectedEngine,savedFrameEngine=_frameEngine,savedDisplayedEngine=_displayedEngine; BOOL savedMenuOpen=_fieldMenuOpen;
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
    party->active=0; _partyUIState=[partyData copy]; _fieldMapId=MAP_ROUTE_29; _selectedEngine=_frameEngine=_displayedEngine=0; _fieldMenuOpen=NO; [self displayFrame];
    report[@"partyFallbackRestoresAB"]=@(fallback && !_buttons[@1].hidden && !_buttons[@2].hidden && !_partyScene
        && [_buttons[@1].currentTitle isEqual:@"互动"] && [_buttons[@2].currentTitle isEqual:@"奔跑"]);
    report[@"partyExitHidesPanel"] = @(!_partyScene && _partyPanel.hidden && _partyBack.hidden);
    HGNativePartyUIState potion={.active=1,.revision=90,.context=HG_PARTY_UI_USE_CONTEXT,.phase=HG_PARTY_UI_ITEM_TARGET,
        .ready=1,.count=2,.slotMask=3,.cancelEnabled=1,.cancelX=228,.cancelY=172};
    potion.mons[0]=(HGNativePartyUIMon){.species=152,.hp=19,.maxHp=20,.level=5,.heldItem=17,.status=4,.gender=1,.active=1,
        .hitX=64,.hitY=24,.iconX=16,.iconY=0,.iconWidth=32,.iconHeight=32,.nickname={.pixelBuffer=1,.tileWidth=9,.tileHeight=2}};
    potion.mons[1]=(HGNativePartyUIMon){.species=155,.hp=20,.maxHp=20,.level=5,.isEgg=1,.active=1,.hitX=192,.hitY=32,
        .iconX=144,.iconY=8,.iconWidth=32,.iconHeight=32,.nickname={.pixelBuffer=1,.tileWidth=9,.tileHeight=2}};
    potion.message=(HGNativePartyUIWindow){.pixelBuffer=1,.tileWidth=20,.tileHeight=2,.clearColor=15};
    UIImage *potionPrompt=OriginalFrameRegion(iconFrame,0,0,160,16),*potionResult=OriginalFrameRegion(iconFrame,0,0,216,32);
    NSDictionary *potionPictures=@{@"cards":@[icon,icon],@"message":potionPrompt};
    _partyUIState=[NSData dataWithBytes:&potion length:sizeof(potion)]; _partyPictures=potionPictures; [self displayFrame]; [self.view layoutIfNeeded];
    report[@"partyPotionTargetMetadata"]=@([_displayedPartyUIState isEqualToData:_partyUIState] && [_partyMons[0].accessibilityLabel containsString:@"19/20"]
        && [_partyMons[0].accessibilityLabel containsString:@"中毒"] && [_partyMons[0].accessibilityLabel containsString:@"携带道具"]
        && [_partyMons[1].accessibilityLabel isEqual:@"蛋"] && _partyCards[0].image==icon && _partyCards[1].image==icon);
    BOOL potionTarget=!_partyPanel.hidden && _partyScene && _canvas.hidden && _buttons[@1].hidden && _buttons[@2].hidden
        && _partyMons[0].enabled && _partyMons[1].enabled && _partyBack.enabled && [_partyBack.currentTitle isEqual:@"返回"]
        && _partyMessage.image==potionPrompt && !_partyMessage.hidden && potionPrompt.size.width==160 && potionPrompt.size.height==16
        && _partySelectedName.hidden && _partySelectedItem.hidden;
    for (UIButton *button in _partyActions) potionTarget&=button.hidden && !button.enabled;
    report[@"partyPotionOriginalTargetPrompt"]=@(potionTarget);
    [_partyMons[0] sendActionsForControlEvents:UIControlEventTouchUpInside];
    [_condition lock];
    BOOL potionQueued=_partyChoicePending && _partyChoiceKind==HG_PARTY_UI_TOUCH_MON && _partyChoiceRevision==potion.revision && _partyChoicePhase==potion.phase;
    _keys=_pendingKeys=_submittedKeys=partyKeys=0x43; _touchPressed=partyPressed=YES;
    [self takePartyTouchLocked:&potion reads:100 keys:&partyKeys x:&partyX y:&partyY pressed:&partyPressed];
    BOOL potionPress=partyPressed && partyX==64 && partyY==24 && !_touchPressed && partyKeys==0x40
        && _keys==0x40 && _pendingKeys==0x40 && _submittedKeys==0x40;
    [self takePartyTouchLocked:&potion reads:103 keys:&partyKeys x:&partyX y:&partyY pressed:&partyPressed]; BOOL potionHeld=partyPressed;
    [self takePartyTouchLocked:&potion reads:104 keys:&partyKeys x:&partyX y:&partyY pressed:&partyPressed];
    report[@"partyPotionOriginalTargetSampling"]=@(potionQueued && potionPress && potionHeld && !partyPressed && !_partyTouchActive && !_partyChoicePending);
    _keys=_pendingKeys=_submittedKeys=partyKeys=0; [_condition unlock];
    [self displayFrame]; [_partyBack sendActionsForControlEvents:UIControlEventTouchUpInside];
    [_condition lock]; BOOL potionCancelQueued=_partyChoicePending && _partyChoiceKind==HG_PARTY_UI_TOUCH_CANCEL;
    [self takePartyTouchLocked:&potion reads:110 keys:&partyKeys x:&partyX y:&partyY pressed:&partyPressed];
    BOOL potionCancel=partyPressed && partyX==228 && partyY==172;
    [self takePartyTouchLocked:&potion reads:113 keys:&partyKeys x:&partyX y:&partyY pressed:&partyPressed]; potionCancel&=partyPressed;
    [self takePartyTouchLocked:&potion reads:114 keys:&partyKeys x:&partyX y:&partyY pressed:&partyPressed];
    report[@"partyPotionOriginalCancelSampling"]=@(potionCancelQueued && potionCancel && !partyPressed && !_partyTouchActive && !_partyChoicePending);
    [_condition unlock];
    HGNativePartyUIState potionGap=potion; potionGap.ready=0; potionGap.message=(HGNativePartyUIWindow){0};
    for (NSUInteger i=0;i<HG_PARTY_UI_SLOTS;i++) potionGap.mons[i].nickname=(HGNativePartyUIWindow){0};
    struct w2c_env *potionSavedEnv=_env; NSDictionary *potionSavedOwner=_partyOwnerPictures; _env=&cacheEnv;
    _partyOwnerPictures=@{@"cards":@[icon,icon],@"message":potionPrompt,@"messageRevision":@(potion.revision),
        @"messageContext":@(potion.context),@"messagePhase":@(potion.phase)};
    NSDictionary *potionGapPictures=[self copyPartyPictures:&potionGap frame:nil]; _env=potionSavedEnv; _partyOwnerPictures=potionSavedOwner;
    _partyUIState=[NSData dataWithBytes:&potionGap length:sizeof(potionGap)]; _partyPictures=potionGapPictures; [self displayFrame];
    report[@"partyPotionBorrowedGapKeepsScene"]=@(HGNativePartyUIIsPotionUse(&potionGap) && _partyScene && !_partyPanel.hidden
        && !_partyBack.enabled && !_partyMons[0].enabled && _partyCards[0].image==icon && _partyMessage.image==potionPrompt
        && _buttons[@1].hidden && _buttons[@2].hidden);
    _partyUIState=[NSData dataWithBytes:&potion length:sizeof(potion)]; _partyPictures=potionPictures; [self displayFrame];
    [_partyMons[0] sendActionsForControlEvents:UIControlEventTouchUpInside];
    HGNativePartyUIState potionProgress=potion; potionProgress.revision++; potionProgress.phase=HG_PARTY_UI_ITEM_USE;
    potionProgress.selected=HG_PARTY_UI_CONFIRM; potionProgress.ready=potionProgress.cancelEnabled=0;
    potionProgress.message=(HGNativePartyUIWindow){.pixelBuffer=1,.tileWidth=27,.tileHeight=4,.clearColor=15};
    [_condition lock]; BOOL potionStaleQueued=_partyChoicePending && _partyChoiceRevision==potion.revision && _partyChoicePhase==potion.phase; partyPressed=NO;
    [self takePartyTouchLocked:&potionProgress reads:120 keys:&partyKeys x:&partyX y:&partyY pressed:&partyPressed];
    report[@"partyPotionRejectsStaleTarget"]=@(potionStaleQueued && !_partyChoicePending && !_partyTouchActive && !partyPressed); [_condition unlock];
    _partyUIState=[NSData dataWithBytes:&potionProgress length:sizeof(potionProgress)]; _partyPictures=@{@"cards":@[icon,icon],@"message":potionResult}; [self displayFrame];
    BOOL potionPrinting=_partyScene && !_partyPanel.hidden && !_partyBack.enabled && [_partyBack.currentTitle isEqual:@"继续"]
        && !_partyMessage.hidden && _partyMessage.image==potionResult && _buttons[@1].hidden && _buttons[@2].hidden;
    [_partyBack sendActionsForControlEvents:UIControlEventTouchUpInside];
    [_condition lock];
    _keys=_pendingKeys=_submittedKeys=partyKeys=0x43; _touchPressed=partyPressed=YES;
    [self takePartyTouchLocked:&potionProgress reads:130 keys:&partyKeys x:&partyX y:&partyY pressed:&partyPressed];
    report[@"partyPotionPrintingBlocksRawInput"]=@(potionPrinting && !_partyChoicePending && !partyPressed && !_touchPressed
        && partyKeys==0x40 && _keys==0x40 && _pendingKeys==0x40 && _submittedKeys==0x40);
    _keys=_pendingKeys=_submittedKeys=partyKeys=0; [_condition unlock];
    potionProgress.ready=potionProgress.cancelEnabled=1; potionProgress.cancelX=124; potionProgress.cancelY=168;
    _partyUIState=[NSData dataWithBytes:&potionProgress length:sizeof(potionProgress)]; [self displayFrame];
    BOOL potionContinueOnly=_partyScene && _partyBack.enabled && [_partyBack.currentTitle isEqual:@"继续"] && _buttons[@1].hidden && _buttons[@2].hidden
        && !HGNativePartyUIResolveTouch(&potionProgress,potionProgress.revision,potionProgress.phase,HG_PARTY_UI_TOUCH_MON,0,&takeX,&takeY)
        && !HGNativePartyUIResolveTouch(&potionProgress,potionProgress.revision,potionProgress.phase,HG_PARTY_UI_TOUCH_ACTION,0,&takeX,&takeY);
    for (UIButton *button in _partyMons) potionContinueOnly&=!button.enabled;
    for (UIButton *button in _partyActions) potionContinueOnly&=button.hidden && !button.enabled;
    report[@"partyPotionWaitHasOnlyContinue"]=@(potionContinueOnly);
    [_partyBack sendActionsForControlEvents:UIControlEventTouchUpInside];
    [_condition lock]; BOOL potionContinueQueued=_partyChoicePending && _partyChoiceKind==HG_PARTY_UI_TOUCH_CANCEL;
    _keys=_pendingKeys=_submittedKeys=partyKeys=0x43; _touchPressed=partyPressed=YES;
    [self takePartyTouchLocked:&potionProgress reads:140 keys:&partyKeys x:&partyX y:&partyY pressed:&partyPressed];
    BOOL potionContinue=partyPressed && partyX==124 && partyY==168 && !_touchPressed && partyKeys==0x40
        && _keys==0x40 && _pendingKeys==0x40 && _submittedKeys==0x40;
    [self takePartyTouchLocked:&potionProgress reads:143 keys:&partyKeys x:&partyX y:&partyY pressed:&partyPressed]; potionContinue&=partyPressed;
    [self takePartyTouchLocked:&potionProgress reads:144 keys:&partyKeys x:&partyX y:&partyY pressed:&partyPressed];
    report[@"partyPotionOriginalContinueSampling"]=@(potionContinueQueued && potionContinue && !partyPressed && !_partyTouchActive && !_partyChoicePending);
    _keys=_pendingKeys=_submittedKeys=partyKeys=0; [_condition unlock];
    HGNativePartyUIState potionCancelBusy=potion; potionCancelBusy.selected=HG_PARTY_UI_CONFIRM; potionCancelBusy.ready=0;
    _partyUIState=[NSData dataWithBytes:&potionCancelBusy length:sizeof(potionCancelBusy)]; _partyPictures=potionPictures; [self displayFrame];
    report[@"partyPotionCancelBusyKeepsScene"]=@(HGNativePartyUIIsPotionUse(&potionCancelBusy) && _partyScene && !_partyPanel.hidden
        && !_partyBack.enabled && !_partyMons[0].enabled && _buttons[@1].hidden && _buttons[@2].hidden);
    HGNativePartyUIState potionUnsupported=potion; potionUnsupported.phase=HG_PARTY_UI_CONTEXT;
    _partyUIState=[NSData dataWithBytes:&potionUnsupported length:sizeof(potionUnsupported)]; [self displayFrame];
    [_condition lock]; _keys=_pendingKeys=_submittedKeys=partyKeys=3;
    [self takePartyTouchLocked:&potionUnsupported reads:150 keys:&partyKeys x:&partyX y:&partyY pressed:&partyPressed];
    report[@"partyPotionUnsupportedPhaseUsesCanvas"]=@(!_partyScene && _partyPanel.hidden && _partyBack.hidden && !_canvas.hidden
        && !_buttons[@1].hidden && !_buttons[@2].hidden && partyKeys==3 && _keys==3 && _pendingKeys==3 && _submittedKeys==3);
    _keys=_pendingKeys=_submittedKeys=partyKeys=0; _touchPressed=NO; _partyChoicePending=_partyTouchActive=NO; [_condition unlock];
    _partyUIState=savedPartyState; _partyPictures=savedPartyDisplayPictures; _fieldMapId=savedFieldMap; _selectedEngine=savedEngine; _frameEngine=savedFrameEngine; _displayedEngine=savedDisplayedEngine; _fieldMenuOpen=savedMenuOpen; [self displayFrame];
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
    HGNativeSummaryUIState cachedSummary={.active=1,.revision=81,.phase=HG_SUMMARY_UI_INPUT,.monDataType=1,.partyCount=1};
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
    _frame=[NSMutableData dataWithLength:256*192*4]; _frameWidth=256; _frameEngine=0;
    _selectedEngine=_displayedEngine=1;
    [_condition unlock];
    [self.view layoutIfNeeded]; [self displayFrame];
    report[@"fieldMenuUsesPublishedRoute"] = @(_selectedEngine==1 && !_displayedEngine && _fieldMenuOpen && !_fieldMenu.hidden && !_fieldMenuShade.hidden && !_canvas.userInteractionEnabled);
    [_condition lock]; _selectedEngine=0; [_condition unlock];
    report[@"fieldMenuOverlay"] = @(_fieldMenuOpen && !_displayedEngine && !_fieldMenu.hidden && !_fieldMenuShade.hidden && !_canvas.userInteractionEnabled && _joystick.hidden);
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
    [_condition lock]; report[@"joystickResumeDoesNotMove"] = @(![self takeKeysLocked]); _fieldMapId=_lastPadReadCount=_frameEngine=_displayedEngine=0; [_condition unlock];
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
    battle->mode=HG_BATTLE_UI_NEXT_MON; battle->enabledMask=3; battle->nameWidth[0]=battle->nameWidth[1]=1;
    for (NSUInteger i=0;i<2;i++) memset(battle->namePixels[i],255,4);
    [self updateBattleControls:[battleData copy]]; [self.view layoutIfNeeded];
    report[@"nativeNextMonControls"] = @(!_battleBar.hidden && !_battleButtons[0].hidden && !_battleButtons[1].hidden && _battleButtons[0].enabled && _battleButtons[1].enabled
        && _battleButtons[2].hidden && _battleButtons[3].hidden && _battleBack.hidden && _battleBall.hidden && _buttons[@1].hidden
        && _battleButtons[0].configuration.image && _battleButtons[1].configuration.image && !_battleButtons[0].configuration.title.length && !_battleButtons[1].configuration.title.length
        && [_battleButtons[0].accessibilityLabel isEqualToString:@"使用下一只宝可梦"] && [_battleButtons[1].accessibilityLabel isEqualToString:@"逃跑"]
        && _battleButtons[0].bounds.size.width>=44 && _battleButtons[0].bounds.size.height>=44);
    [_battleButtons[0] sendActionsForControlEvents:UIControlEventTouchUpInside];
    [_condition lock]; report[@"nativeNextMonQueued"] = @(_battleChoicePending && _battleChoiceRevision==battle->revision && _battleChoiceIndex==0 && !_selectedEngine); _battleChoicePending=NO; [_condition unlock];
    [_battleButtons[1] sendActionsForControlEvents:UIControlEventTouchUpInside];
    [_condition lock]; report[@"nativeNextMonFleeQueued"] = @(_battleChoicePending && _battleChoiceIndex==1 && !_selectedEngine); _battleChoicePending=NO; [_condition unlock];
    battle->mode = HG_BATTLE_UI_NONE; battle->ready = 0;
    [self updateBattleControls:[battleData copy]];
    report[@"nativeBattleKeepsSceneDuringAnimation"] = @(_battleScene && !_battleBar.hidden && !_battleButtons[0].enabled);
    [_condition lock]; _selectedEngine=1; _frameEngine=_displayedEngine=0; [_condition unlock];
    [self updateBattleControls:[battleData copy]];
    report[@"nativeBattleUsesDisplayedRoute"] = @(_selectedEngine==1 && !_displayedEngine && _battleScene && !_battleBar.hidden);
    [_condition lock]; _frameEngine=_displayedEngine=1; [_condition unlock];
    [self updateBattleControls:[battleData copy]];
    report[@"nativeBattleSubscreenFallback"] = @(!_battleScene && _battleBar.hidden);
    [_condition lock]; _selectedEngine=_frameEngine=_displayedEngine=0; _automaticMenuEngine=NO; [_condition unlock];
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
                    report[@"originalFieldMenuSameScreen"] = @(!self->_fieldMenu.hidden && self->_fieldMenu.image.size.width==256 && self->_fieldMenu.image.size.height==192 && self->_canvas.image.size.width>256 && !self->_displayedEngine && self->_fieldMenu.accessibilityElements.count>0);
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
                          [self checkOptionsFlow:report completion:^{
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
                        }];
                    };
                    dispatch_after(dispatch_time(DISPATCH_TIME_NOW,NSEC_PER_SEC/2),dispatch_get_main_queue(),verifyReset);
                });
            });
        });
    });
}
#endif
#ifdef HEARTGOLD_ORIGINAL_CAPTURE_REGISTRATION_CHECK
#ifdef HEARTGOLD_ORIGINAL_NAMING_CHECK
enum { CaptureCheckFirstVariant=HG_CAPTURE_QA_NAMED_PARTY, CaptureCheckLastVariant=HG_CAPTURE_QA_EMPTY_PC };
#else
enum { CaptureCheckFirstVariant=HG_CAPTURE_QA_PARTY, CaptureCheckLastVariant=HG_CAPTURE_QA_PC };
#endif
- (BOOL)captureRegistrationInputIdleLocked {
    return !_keys && !_pendingKeys && !_submittedKeys && !_joystickKeys && !_pendingJoystickKeys && !_submittedJoystickKeys && !_touchPressed
        && !_battleChoicePending && !_partyChoicePending && !_partyTouchActive && !_bagChoicePending && !_bagTouchActive
        && !_summaryChoicePending && !_summaryInputActive && !_summaryTouchPressed && !_summaryInputKeys
        && !_evolutionChoicePending && !_evolutionInputActive && !_evolutionInputCooling
        && !_pokedexChoicePending && !_pokedexInputActive && !_pokedexInputCooling && !_pokedexGestureLive && !_pokedexGestureAdmitted
        && !_battleMenuChoicePending && !_battleMenuTouchActive && !_battleMenuTouchCooling
        && !_captureChoicePending && !_captureInputActive && !_captureInputCooling;
}
// Owner-thread only. UI methods consume _captureQAState's copied NSData instead.
- (BOOL)readCaptureRegistrationQA:(HGNativeCaptureQAState *)state {
    if (NSThread.isMainThread) wasm_rt_trap(WASM_RT_TRAP_OOB);
    uint32_t address=w2c_hgengine_HGNativeCaptureQAGetState(_game);
    if (!address || address>_env->memory->size || sizeof(*state)>_env->memory->size-address) wasm_rt_trap(WASM_RT_TRAP_OOB);
    memcpy(state,_env->memory->data+address,sizeof(*state));
    if (state->version!=HG_CAPTURE_QA_VERSION || state->byteSize!=HG_CAPTURE_QA_BYTES) wasm_rt_trap(WASM_RT_TRAP_OOB);
    return YES;
}
- (BOOL)queueCaptureQARequest:(uint32_t)revision variant:(uint32_t)variant {
    [_condition lock]; const HGNativeCaptureQAState *q=_captureQAState.length==sizeof(*q) ? _captureQAState.bytes : NULL;
    BOOL valid=!_paused && !_stopping && !_failure && !_captureQARequestPending && !_captureQARequestInFlight && !_captureQARequestAccepted
        && q && q->version==HG_CAPTURE_QA_VERSION && q->byteSize==HG_CAPTURE_QA_BYTES && q->revision==revision && q->fieldReady && q->map==MAP_ROUTE_29
        && !q->requested && !q->started && !q->seeded && !q->errorLine && (variant==CaptureCheckFirstVariant || variant==CaptureCheckLastVariant)
        && _captureQABootStep==3 && !_captureQABootFrame && [self captureRegistrationInputIdleLocked];
    if (valid) {_captureQARequestPending=YES;_captureQARequestRevision=revision;_captureQARequestVariant=variant;}
    [_condition unlock]; return valid;
}
// Owner-thread only, outside _condition: the actual guest request may enter engine code.
- (void)processCaptureRegistrationRequest:(uint32_t)revision variant:(uint32_t)variant idle:(BOOL)idle {
    [_condition lock]; BOOL running=!_paused && !_stopping && !_failure && _captureQARequestInFlight; [_condition unlock];
    HGNativeCaptureQAState live={0}; BOOL valid=[self readCaptureRegistrationQA:&live] && live.revision==revision && live.fieldReady
        && live.map==MAP_ROUTE_29 && !live.requested && !live.started && !live.seeded && !live.errorLine && idle && running
        && (variant==CaptureCheckFirstVariant || variant==CaptureCheckLastVariant);
    BOOL accepted=NO; if (valid) {accepted=w2c_hgengine_HGNativeCaptureQARequest(_game,revision,variant)!=0;}
    [_condition lock]; _captureQAOwnerRequests+=valid; _captureQARequestAccepted=accepted; _captureQARequestInFlight=NO; [_condition unlock];
}
- (void)takeCaptureRegistrationBootLocked:(const HGNativeCaptureQAState *)q keys:(uint32_t *)keys x:(uint32_t *)x y:(uint32_t *)y pressed:(BOOL *)pressed {
    if (_captureQARequestAccepted || (q && (q->requested || q->started))) return;
    _keys=_pendingKeys=_submittedKeys=0; _touchPressed=NO; *keys=*x=*y=0; *pressed=NO;
    if (q && q->fieldReady && q->map==MAP_ROUTE_29) {_captureQABootStep=3;_captureQABootFrame=0;return;}
    if (!_env->resetOccurred && _captureQABootStep<3) {
        const uint32_t frames[]={450,780,1320}; uint32_t now=OS_GetVBlankCount();
        if (!_captureQABootFrame && now>=frames[_captureQABootStep]) _captureQABootFrame=now;
        if (_captureQABootFrame && now-_captureQABootFrame>=8) {_captureQABootFrame=0;_captureQABootStep++;}
        else if (_captureQABootFrame) *keys=1;
    }
}
- (void)checkCaptureRegistrationDisplay:(uint32_t)frame {
    sPrivateDisplayFrame=frame;
    if (sCaptureRegistrationDisplay) sCaptureRegistrationDisplay(frame);
}
- (BOOL)captureRegistrationPrivateFilesEqual:(NSDictionary *)before {
    NSURL *support=[[NSFileManager.defaultManager URLsForDirectory:NSApplicationSupportDirectory inDomains:NSUserDomainMask].firstObject URLByAppendingPathComponent:@"HeartGoldOriginal" isDirectory:YES];
    if (before.count!=4) return NO;
    for (NSString *name in before) if (![before[name] isEqualToData:[NSData dataWithContentsOfURL:[support URLByAppendingPathComponent:name]]]) return NO;
    return YES;
}
- (BOOL)captureRegistrationSettlement:(const HGNativeCaptureQAState *)q variant:(uint32_t)variant {
    if (variant!=CaptureCheckFirstVariant && variant!=CaptureCheckLastVariant) return NO;
    uint32_t location=variant==CaptureCheckFirstVariant ? HG_CAPTURE_QA_PARTY : HG_CAPTURE_QA_PC;
    if (!q || q->version!=HG_CAPTURE_QA_VERSION || q->byteSize!=HG_CAPTURE_QA_BYTES || q->errorLine || q->variant!=variant
        || !q->seeded || !q->requested || !q->started || !q->returned || !q->fieldReturned || !q->fieldReady || q->map!=MAP_ROUTE_29
        || q->outcome!=4 || q->battleResult!=4 || q->ballBefore!=1 || q->ballAfter || q->ownedBefore || q->ownedDuringPopup || q->ownedAfter!=1
        || !q->targetKnown || q->targetSpecies!=10 || q->targetLevel!=1 || q->storedSpecies!=q->targetSpecies || q->storedForm!=q->targetForm
        || q->storedPersonality!=q->targetPersonality || q->storedExp!=q->targetExp || q->storedBall!=q->expectedBall || q->storedOtId!=q->trainerId
        || !q->oldPartyPreserved || !q->oldPcPreserved || !q->bagExact || !q->flagsPreserved || !q->moneyPreserved || !q->statsExact
        || q->wildAfter!=q->wildBefore+1 || q->caughtAfter!=q->caughtBefore+1 || q->seenAfter<q->seenBefore || q->seenAfter>q->seenBefore+1
        || !q->popupWaitFrames || !q->popupReadyFrames || !q->popupFadeFrames || q->popupPrematureOwned || !q->nicknameReadyFrames) return NO;
    if (location==HG_CAPTURE_QA_PARTY) return q->storedLocation==location && q->partyBefore==1 && q->partyAfter==2
        && q->pcAfter==q->pcBefore && q->storedBox==UINT32_MAX && q->storedSlot==q->partyBefore;
    return location==HG_CAPTURE_QA_PC && q->storedLocation==location && q->partyBefore==6 && q->partyAfter==6
        && q->pcAfter==q->pcBefore+1 && q->storedBox<18 && q->storedSlot<30;
}
- (void)checkCaptureRegistrationCase {
#ifdef HEARTGOLD_ORIGINAL_NAMING_CHECK
    NSString *caseName=@"naming",*prefix=@"private-naming",*choiceRole=@"nickname-yes",*reportFile=@"private-naming-check.json";
    NSString *nativeReportSha=@HG_NAMING_NATIVE_REPORT_SHA256,*nativeModuleSha=@HG_NAMING_NATIVE_MODULE_SHA256;
    NSString *reportInfoKey=@"HGPrivateNamingNativeReportSha256",*moduleInfoKey=@"HGPrivateNamingNativeModuleSha256";
    uint32_t nicknameIndex=0; NSString *nicknameCheck=@"nativeNicknameYes";
    NSString *caseScope=@"Original Yes-nickname capture and keyboard via visible UIKit controls/canvas in two clean guest epochs. Final naming assertions are carried by approved guest returned/errorLine0; intermediate strings and physical gestures need manual/physical validation.";
#else
    NSString *caseName=@"capture-registration",*prefix=@"private-capture",*choiceRole=@"nickname-no",*reportFile=@"private-capture-registration-check.json";
    NSString *nativeReportSha=@HG_CAPTURE_NATIVE_REPORT_SHA256,*nativeModuleSha=@HG_CAPTURE_NATIVE_MODULE_SHA256;
    NSString *reportInfoKey=@"HGPrivateCaptureNativeReportSha256",*moduleInfoKey=@"HGPrivateCaptureNativeModuleSha256";
    uint32_t nicknameIndex=1; NSString *nicknameCheck=@"nativeNicknameNo";
    NSString *caseScope=@"Actual native capture registration and original No-nickname settlement in two clean guest-memory epochs of one owned iOS CHECK process. Snapshot writes do not prove text, portrait, footprint, types or animation fidelity; Root visual review remains required.";
#endif
    NSArray *requiredKeys=@[@"caseBinding",@"muted",@"qa244",@"ownerOnlyRequest",@"nativeBagButtons",@"waitBlocksInput",@"captureCanvas",
        @"staleCaptureRejected",@"pauseCancelsPending",@"typedContinue",@"activeFadeBlocksInput",
#ifdef HEARTGOLD_ORIGINAL_NAMING_CHECK
        @"nativeNicknameYes",
#else
        @"nativeNicknameNo",
#endif
        @"partyAndPcSettlement",
        @"fieldControlsRestored",@"popupLifetimeWitness",@"twoGuestEpochs",@"privateSave4Preserved",@"resetQuiescent",@"capturesComplete"];
    NSArray *roles=@[@"field-before",@"master-ball-use",@"cry-wait",@"registration-ready",@"registration-original",@"original-fade",choiceRole,@"field-return"];
#ifdef HEARTGOLD_ORIGINAL_NAMING_CHECK
    requiredKeys=[requiredKeys arrayByAddingObjectsFromArray:@[@"naming16",@"namingCanvas256",@"namingBusyBlocksInput",
        @"namingStaleRejected",@"namingPauseClearsPending",@"namingPages",@"namingDelete",@"namingTenPlusExtra",
        @"startOnlyLocatesOK",@"originalOKTouch",@"guestNamingStorageAssertions",@"namingLeaseReleased"]];
#endif
    NSMutableDictionary *checks=[NSMutableDictionary new]; for (NSString *key in requiredKeys) checks[key]=@NO;
    NSMutableArray *requiredPNGs=[NSMutableArray new],*events=[NSMutableArray new],*variants=[NSMutableArray new];
    for (uint32_t variant=CaptureCheckFirstVariant;variant<=CaptureCheckLastVariant;variant++) for (NSString *role in roles) [requiredPNGs addObject:[NSString stringWithFormat:@"%@-v%u-%@.png",prefix,variant,role]];
#ifdef HEARTGOLD_ORIGINAL_NAMING_CHECK
    for (uint32_t v=CaptureCheckFirstVariant;v<=CaptureCheckLastVariant;v++)
        for (NSString *role in @[@"naming-init-busy",@"naming-ready",@"naming-original",@"naming-post-OK-busy",@"naming-inactive"])
            [requiredPNGs addObject:[NSString stringWithFormat:@"%@-v%u-%@.png",prefix,v,role]];
    for (NSString *role in @[@"naming-page-busy",@"naming-upper-restored",@"naming-B",@"naming-backspaced",@"naming-ten-A",@"naming-max-busy",@"naming-eleventh-attempt",@"naming-start-positioned"])
        [requiredPNGs addObject:[NSString stringWithFormat:@"%@-v%u-%@.png",prefix,CaptureCheckFirstVariant,role]];
    [requiredPNGs addObject:[NSString stringWithFormat:@"%@-v%u-naming-empty.png",prefix,CaptureCheckLastVariant]];
#endif
    NSMutableSet *captured=[NSMutableSet new];
    BOOL (^visible)(UIView *)=^BOOL(UIView *view) {
        if (!view || !view.window || view.window.hidden || view.window.alpha<=0) return NO;
        BOOL attached=NO; for (UIView *parent=view;parent;parent=parent.superview) {if (parent.hidden || parent.alpha<=0) return NO;attached|=parent==self.view;}
        return attached;
    };
    NSURL *support=[[NSFileManager.defaultManager URLsForDirectory:NSApplicationSupportDirectory inDomains:NSUserDomainMask].firstObject URLByAppendingPathComponent:@"HeartGoldOriginal" isDirectory:YES];
    NSMutableDictionary *privateBefore=[NSMutableDictionary new];
    for (NSString *suffix in @[@"",@".console",@".wifi",@".lock"]) {
        NSString *name=[@"heartgold.sav" stringByAppendingString:suffix]; NSData *data=[NSData dataWithContentsOfURL:[support URLByAppendingPathComponent:name]];
        if (data) privateBefore[name]=data;
    }
    [_condition lock]; sPrivateUICase=caseName; sPrivateCaptures=[NSMutableArray new]; sPrivateObserving=NO; [_condition unlock];
    __block uint32_t variant=CaptureCheckFirstVariant,stage=0,stageFrame=0,polls=0,lastStage=UINT32_MAX,epochBoot=0,resetBoot=0,baselineReads=0,readyGeneration=0,readyRevision=0;
    __block BOOL finishing=NO,waitSeen=NO,fadeSeen=NO,pausePendingCleared=NO;
#ifdef HEARTGOLD_ORIGINAL_NAMING_CHECK
    __block uint32_t pageTurns=0,aCount=0,namingGeneration=0,namingRevision=0,namingPressBase=0;
    __block BOOL namingInitSeen=NO,namingPageBusy=NO,namingMaxBusy=NO,namingPostOK=NO,namingSubmitted=NO,namingPauseCleared=NO;
#endif
    __block void (^finish)(NSString *);
    BOOL (^shot)(NSString *,BOOL)=^BOOL(NSString *role,BOOL original) {
        NSString *name=[NSString stringWithFormat:@"%@-v%u-%@.png",prefix,variant,role];
        if ([captured containsObject:name]) return YES;
        if (![self checkPrivateCapture:name kind:original ? @"original-MAIN256" : @"UIKit-capture-registration" image:original ? self->_canvas.image : nil]) return NO;
        [captured addObject:name]; return YES;
    };
#ifdef HEARTGOLD_ORIGINAL_NAMING_CHECK
    BOOL (^tapNaming)(uint32_t,uint32_t)=^BOOL(uint32_t x,uint32_t y) {
        if (!self->_namingScene || !visible(self->_canvas) || !self->_canvas.userInteractionEnabled || x>=256 || y>=192) return NO;
        CGRect bounds=self->_canvas.bounds;
        CGPoint point=CGPointMake(CGRectGetMinX(bounds)+(x+0.5)*bounds.size.width/256.0,CGRectGetMinY(bounds)+(y+0.5)*bounds.size.height/192.0);
        [self->_condition lock]; uint32_t before=self->_namingQAInputPresses; [self->_condition unlock];
        [self tapAt:point];
        [self->_condition lock]; NSDictionary *last=self->_namingQAInputs.lastObject;
        BOOL queued=self->_namingChoicePending && self->_namingChoiceTouch && !self->_namingChoiceKeys && self->_namingChoiceX==x && self->_namingChoiceY==y;
        BOOL admitted=self->_namingQAInputPresses==before+1 && [last[@"touch"] boolValue] && ![last[@"keys"] unsignedIntValue]
            && [last[@"x"] unsignedIntValue]==x && [last[@"y"] unsignedIntValue]==y;
        [self->_condition unlock]; return queued || admitted;
    };
    BOOL (^pressNaming)(uint32_t)=^BOOL(uint32_t tag) {
        UIButton *button=self->_buttons[@(tag)]; if (!self->_namingScene || !visible(button) || !button.enabled) return NO;
        uint32_t key=tag==0x400 ? 4 : tag;
        [self->_condition lock]; uint32_t before=self->_namingQAInputPresses; [self->_condition unlock];
        [button sendActionsForControlEvents:UIControlEventTouchDown]; [button sendActionsForControlEvents:UIControlEventTouchUpInside];
        [self->_condition lock]; NSDictionary *last=self->_namingQAInputs.lastObject;
        BOOL queued=self->_namingChoicePending && !self->_namingChoiceTouch && self->_namingChoiceKeys==key;
        BOOL admitted=self->_namingQAInputPresses==before+1 && ![last[@"touch"] boolValue] && [last[@"keys"] unsignedIntValue]==key;
        [self->_condition unlock]; return queued || admitted;
    };
#endif
    finish=^(NSString *status) {
        if (finishing) return; finishing=YES; sCaptureRegistrationDisplay=nil; [self stop];
        __block NSUInteger waits=0; __block void (^stopped)(void); stopped=^{
            [self->_condition lock]; BOOL done=self->_finished && !self->_env && !self->_game && !self->_wasi,quiesced=self->_resetQuiesced; [self->_condition unlock];
            if (!done && ++waits<100) {dispatch_after(dispatch_time(DISPATCH_TIME_NOW,NSEC_PER_SEC/10),dispatch_get_main_queue(),stopped);return;}
            checks[@"privateSave4Preserved"]=@([self captureRegistrationPrivateFilesEqual:privateBefore]);
            checks[@"resetQuiescent"]=@([checks[@"resetQuiescent"] boolValue] && done && quiesced);
            BOOL images=sPrivateCaptures.count==requiredPNGs.count && [captured isEqualToSet:[NSSet setWithArray:requiredPNGs]];
            for (NSDictionary *row in sPrivateCaptures) images&=[row[@"drawn"] boolValue] && [row[@"written"] boolValue];
            checks[@"capturesComplete"]=@(images); checks[@"twoGuestEpochs"]=@(variants.count==2 && [variants[1][@"boot"] unsignedIntValue]==[variants[0][@"boot"] unsignedIntValue]+1);
            BOOL passed=done && [status isEqual:@"completed"]; for (NSString *key in requiredKeys) passed&=[checks[key] boolValue];
            NSDictionary *report=@{@"version":@1,@"case":caseName,@"complete":@(done),@"passed":@(passed),@"status":status,
                @"variant":@(variant),@"stage":@(stage),@"requiredKeys":requiredKeys,@"checks":checks,@"requiredPNGs":requiredPNGs,
                @"captures":[sPrivateCaptures copy],@"events":events,@"variants":variants,@"guestEpochCount":@(variants.count),@"osProcessCount":@1,
                @"nativePrerequisiteReportSha256":nativeReportSha,@"nativePrerequisiteModuleSha256":nativeModuleSha,
                @"qaBytes":@(HG_CAPTURE_QA_BYTES),@"lifecycleObservers":@[@"PokedexEntryPopup_Create",@"PokedexEntryPopup_Delete"],@"gameplayWrappers":@[],
                @"manualVisualReviewPending":@YES,@"manualVisualPassed":@NO,@"physicalGestureRecognitionVerified":@NO,
                #ifdef HEARTGOLD_ORIGINAL_NAMING_CHECK
                @"namingYesCovered":@([checks[@"nativeNicknameYes"] boolValue] && [checks[@"naming16"] boolValue]),@"intermediateStringsAutomaticallyVerified":@NO,@"independentNamingFreeVerified":@NO,
#else
                @"namingYesCovered":@NO,
#endif
                @"fullGameComplete":@NO,@"audioStabilityEstablished":@NO,@"audibleAudioValidated":@NO,
                @"scope":caseScope};
            NSURL *documents=[NSFileManager.defaultManager URLsForDirectory:NSDocumentDirectory inDomains:NSUserDomainMask].firstObject;
            [[NSJSONSerialization dataWithJSONObject:report options:NSJSONWritingPrettyPrinted error:nil] writeToURL:[documents URLByAppendingPathComponent:reportFile] options:NSDataWritingAtomic error:nil];
            stopped=nil; finish=nil;
        }; stopped();
    };
    checks[@"caseBinding"]=@([[NSBundle.mainBundle objectForInfoDictionaryKey:@"HGPrivateCheckCase"] isEqual:caseName]
        && [NSProcessInfo.processInfo.environment[@"HEARTGOLD_CHECK_CASE"] isEqual:caseName]
        && [NSProcessInfo.processInfo.environment[@"HEARTGOLD_TEST_MUTE"] isEqual:@"1"]
        && [[NSBundle.mainBundle objectForInfoDictionaryKey:reportInfoKey] isEqual:nativeReportSha]
        && [[NSBundle.mainBundle objectForInfoDictionaryKey:moduleInfoKey] isEqual:nativeModuleSha]);
    if (![checks[@"caseBinding"] boolValue] || privateBefore.count!=4) {finish(@"case_or_private_save_binding_failed");return;}
    sCaptureRegistrationDisplay=^(uint32_t frame) {
        [self->_condition lock]; NSData *qaData=[self->_displayedCaptureQAState copy],*ownerQA=[self->_captureQAState copy],*owner=[self->_captureUIState copy];
        uint32_t displayedReads=self->_displayedCaptureQAPadReads;
        uint32_t reads=self->_lastPadReadCount,boot=self->_bootCount,bootStep=self->_captureQABootStep,bootHeld=self->_captureQABootFrame;
        uint32_t presses=self->_captureQAInputPresses,releases=self->_captureQAInputReleases,lastRelease=self->_captureQALastReleaseReads,maxHeld=self->_captureQAMaxHeldReads;
        BOOL failed=self->_failure!=nil || self->_finished,paused=self->_paused,suspended=self->_suspended,requestPending=self->_captureQARequestPending,requestInFlight=self->_captureQARequestInFlight,requestAccepted=self->_captureQARequestAccepted;
        uint32_t ownerRequests=self->_captureQAOwnerRequests;
#ifdef HEARTGOLD_ORIGINAL_NAMING_CHECK
        NSData *namingData=[self->_displayedNamingUIState copy],*namingOwner=[self->_namingUIState copy];
        uint32_t namingPresses=self->_namingQAInputPresses,namingReleases=self->_namingQAInputReleases,namingLastRelease=self->_namingQALastReleaseReads,namingMaxHeld=self->_namingQAMaxHeldReads;
        NSArray *namingInputs=[self->_namingQAInputs copy]; BOOL namingOverflow=self->_namingQAOverflow;
        BOOL namingIdle=!self->_namingChoicePending && !self->_namingInputActive && !self->_namingInputCooling;
        BOOL namingZero=namingIdle && !self->_namingChoiceTouch && !self->_namingInputTouch && !self->_namingChoice.active && !self->_namingInput.active
            && !self->_namingChoiceKeys && !self->_namingInputKeys && !self->_namingInputReads && !self->_namingReleaseReads;
#endif
        BOOL idle=!self->_captureChoicePending && !self->_captureInputActive && !self->_captureInputCooling;
        BOOL zeroLease=idle && !self->_captureChoiceGeneration && !self->_captureChoiceRevision && !self->_captureInputGeneration && !self->_captureInputRevision && !self->_captureInputReads && !self->_captureReleaseReads;
        BOOL raw=!self->_keys && !self->_pendingKeys && !self->_submittedKeys && !self->_joystickKeys && !self->_pendingJoystickKeys && !self->_submittedJoystickKeys && !self->_touchPressed;
        NSDictionary *audio=self->_audioChanging ? nil : [self->_audio statistics]; BOOL resetClean=self->_restartClean,resetQuiesced=self->_resetQuiesced; [self->_condition unlock];
        const HGNativeCaptureQAState *q=qaData.length==sizeof(*q) ? qaData.bytes : NULL;
        const HGNativeCaptureQAState *liveQA=ownerQA.length==sizeof(*liveQA) ? ownerQA.bytes : NULL;
        const HGNativeCaptureUIState *p=self->_displayedCaptureUIState.length==sizeof(*p) ? self->_displayedCaptureUIState.bytes : NULL;
        const HGNativeBattleUIState *b=self->_displayedBattleUIState.length==sizeof(*b) ? self->_displayedBattleUIState.bytes : NULL;
        const HGNativeBattleBagUIState *bag=self->_displayedBattleBagUIState.length==sizeof(*bag) ? self->_displayedBattleBagUIState.bytes : NULL;
        if (stage!=lastStage) { [events addObject:@{@"variant":@(variant),@"stage":@(stage),@"frame":@(frame),@"boot":@(boot),@"padReads":@(reads),@"displayedPadReads":@(displayedReads),@"qaRevision":@(q ? q->revision : 0),@"capturePhase":@(p ? p->callerPhase : 0)}]; lastStage=stage; stageFrame=frame; }
        if (failed || ++polls>36000 || frame-stageFrame>7200 || events.count>=2048 || (q && q->errorLine)) {finish(failed ? @"engine_failed" : q && q->errorLine ? @"native_guest_failed" : @"flow_timeout");return;}
        if (!q || q->version!=HG_CAPTURE_QA_VERSION || q->byteSize!=HG_CAPTURE_QA_BYTES) return;
        checks[@"qa244"]=@YES;
        if (audio && ![audio[@"muted"] boolValue]) {finish(@"audio_not_muted");return;} if (audio) checks[@"muted"]=@YES;
        BOOL paired=p && owner.length==sizeof(*p) && !memcmp(p,owner.bytes,sizeof(*p));
#ifdef HEARTGOLD_ORIGINAL_NAMING_CHECK
        const HGNativeNamingUIState *n=namingData.length==sizeof(*n) ? namingData.bytes : NULL;
        BOOL namingPaired=n && namingOwner.length==sizeof(*n) && !memcmp(n,namingOwner.bytes,sizeof(*n));
        BOOL namingDone=namingIdle && namingPresses==namingReleases && (!namingPresses || reads-namingLastRelease>=2);
        if (namingOverflow || namingMaxHeld>4) {finish(@"naming_input_observer_overflow_or_hold");return;}
        if (n && n->active) {
            if (!namingPaired) return;
            BOOL geometry=self->_namingScene && visible(self->_canvas) && !self->_displayedEngine && self->_displayedWidth==256
                && self->_canvas.image.size.width==256 && self->_canvas.image.size.height==192
                && fabs(self->_canvas.bounds.size.width*3-self->_canvas.bounds.size.height*4)<=1
                && self->_battleBar.hidden && self->_partyPanel.hidden && self->_bagPanel.hidden && self->_summaryPanel.hidden
                && self->_battleMenuPanel.hidden && self->_pokedexPanel.hidden && self->_joystick.hidden && self->_menuButton.hidden;
            if (!geometry || !raw) {finish(@"naming_canvas_or_priority_failed");return;}
            checks[@"naming16"]=@YES; checks[@"namingCanvas256"]=@YES;
            if (!n->ready) {
                if (HGNativeNamingUIInputAllowed(n,n->ownerGeneration,n->revision) || self->_canvas.userInteractionEnabled
                    || self->_buttons[@1].enabled || self->_buttons[@2].enabled || self->_buttons[@0x400].enabled || self->_buttons[@8].enabled) {finish(@"naming_busy_input_enabled");return;}
                checks[@"namingBusyBlocksInput"]=@YES;
                if (stage==20 && !namingInitSeen) {if (!shot(@"naming-init-busy",NO)) {finish(@"capture_failed");return;}namingInitSeen=YES;}
                if (stage==25 && !namingPageBusy) {if (!shot(@"naming-page-busy",NO)) {finish(@"capture_failed");return;}namingPageBusy=YES;}
                if (stage==28 && aCount==10 && !namingMaxBusy) {if (!shot(@"naming-max-busy",NO)) {finish(@"capture_failed");return;}namingMaxBusy=YES;}
                if (namingSubmitted && !namingPostOK) {if (!shot(@"naming-post-OK-busy",NO)) {finish(@"capture_failed");return;}namingPostOK=YES;}
            }
        }
#endif
        if (p && p->active) {
            if (!paired) return;
            uint32_t keys=0xA5A5A5A5; BOOL allowed=HGNativeCaptureUIResolveInput(p,p->ownerGeneration,p->revision,&keys);
            BOOL canvas=self->_captureScene && !self->_canvas.hidden && !self->_canvas.userInteractionEnabled && !self->_displayedEngine && self->_displayedWidth==256
                && self->_canvas.image.size.width==256 && self->_canvas.image.size.height==192 && self->_battleBar.hidden && self->_partyPanel.hidden && self->_bagPanel.hidden
                && self->_summaryPanel.hidden && self->_battleMenuPanel.hidden && self->_pokedexPanel.hidden && self->_joystick.hidden && self->_menuButton.hidden && self->_buttons[@2].hidden;
            if (!canvas || !raw || (!allowed && self->_buttons[@1].enabled) || q->ownedDuringPopup || q->popupPrematureOwned) {finish(@"capture_priority_or_input_failed");return;}
            checks[@"captureCanvas"]=@YES;
            if (p->callerPhase==13 && !p->done && !p->ready && q->popupWaitFrames>=2 && [self checkPrivatePixels:self->_canvas.image]) {
                if (!shot(@"cry-wait",NO)) {finish(@"capture_failed");return;} waitSeen=YES; checks[@"waitBlocksInput"]=@YES;
            }
            if (p->callerPhase==14 && !p->ready) {if (!shot(@"original-fade",NO)) {finish(@"capture_failed");return;} fadeSeen=YES; checks[@"activeFadeBlocksInput"]=@YES;}
        }
        BOOL bagReady=bag && bag->active && bag->ready && self->_battleMenuKind==OriginalBattleMenuBag && !self->_battleMenuPanel.hidden;
        BOOL command=b && b->active && b->ready && b->mode==HG_BATTLE_UI_COMMAND;
        switch (stage) {
        case 0:
            if (!q->fieldReady || q->map!=MAP_ROUTE_29 || bootStep!=3 || bootHeld || q->requested || q->started || q->seeded
                || !p || p->active || p->ready || p->done || !zeroLease || !liveQA || liveQA->revision!=q->revision || self->_fieldMapId!=MAP_ROUTE_29) break;
            epochBoot=boot; if (!shot(@"field-before",NO)) {finish(@"capture_failed");return;}
#ifdef HEARTGOLD_ORIGINAL_NAMING_CHECK
            if ([self queueCaptureQARequest:q->revision variant:HG_CAPTURE_QA_PARTY]) {finish(@"non_naming_variant_admitted");return;}
#endif
            if ([self queueCaptureQARequest:q->revision+1 variant:variant]) {finish(@"stale_request_admitted");return;}
            if (![self queueCaptureQARequest:q->revision variant:variant]) break;
            stage=1; break;
        case 1:
            if (!requestPending && !requestInFlight && !requestAccepted && !q->requested) {finish(@"owner_request_rejected");return;}
            if (!q->started || !q->targetKnown || !command || !visible(self->_battleBar) || !visible(self->_battleButtons[1]) || !self->_battleButtons[1].enabled) break;
            if (!requestAccepted || ownerRequests!=1 || q->variant!=variant || !q->seeded) {finish(@"owner_request_evidence_failed");return;}
            checks[@"ownerOnlyRequest"]=@YES; [self->_battleButtons[1] sendActionsForControlEvents:UIControlEventTouchUpInside]; stage=2; break;
        case 2:
            if (!bagReady || bag->phase!=HG_BATTLE_BAG_UI_POCKETS || !visible(self->_battleMenuButtons[2]) || !self->_battleMenuButtons[2].enabled) break;
            [self->_battleMenuButtons[2] sendActionsForControlEvents:UIControlEventTouchUpInside]; stage=3; break;
        case 3:
            if (!bagReady || bag->phase!=HG_BATTLE_BAG_UI_ITEMS || bag->pocket!=2) break;
            { NSUInteger row=0; while (row<HG_BATTLE_BAG_UI_ITEMS_PER_PAGE && bag->itemId[row]!=1) row++;
              if (row==HG_BATTLE_BAG_UI_ITEMS_PER_PAGE) {if (bag->pageIndex+1<bag->pageCount && visible(self->_battleMenuButtons[8]) && self->_battleMenuButtons[8].enabled) [self->_battleMenuButtons[8] sendActionsForControlEvents:UIControlEventTouchUpInside]; else {finish(@"master_ball_missing");return;} break;}
              if (bag->quantity[row]!=1 || !visible(self->_battleMenuButtons[row]) || !self->_battleMenuButtons[row].enabled) {finish(@"master_ball_item_invalid");return;}
              [self->_battleMenuButtons[row] sendActionsForControlEvents:UIControlEventTouchUpInside]; stage=4; } break;
        case 4:
            if (!bagReady || bag->phase!=HG_BATTLE_BAG_UI_USE || !visible(self->_battleMenuButtons[0]) || !self->_battleMenuButtons[0].enabled) break;
            if (bag->selectedItemId!=1 || bag->selectedQuantity!=1 || !shot(@"master-ball-use",NO)) {finish(@"master_ball_use_invalid");return;}
            checks[@"nativeBagButtons"]=@YES; [self->_battleMenuButtons[0] sendActionsForControlEvents:UIControlEventTouchUpInside]; stage=5; break;
        case 5:
            if (!waitSeen || !paired || !p->active || !p->ready || !p->done || p->callerPhase!=13 || !idle) break;
            readyGeneration=p->ownerGeneration; readyRevision=p->revision;
            [self->_condition lock]; self->_captureChoicePending=YES; self->_captureChoiceGeneration=readyGeneration+1; self->_captureChoiceRevision=readyRevision; [self->_condition unlock]; baselineReads=reads; stage=6; break;
        case 6:
        case 7:
            if (reads-baselineReads<1 || !idle) break;
            if (!paired || !p->active || !p->ready || p->ownerGeneration!=readyGeneration || p->revision!=readyRevision) {finish(@"stale_capture_advanced_original");return;}
            if (stage==6) { [self->_condition lock]; self->_captureChoicePending=YES; self->_captureChoiceGeneration=readyGeneration; self->_captureChoiceRevision=readyRevision+1; [self->_condition unlock]; baselineReads=reads; stage=7; }
            else { checks[@"staleCaptureRejected"]=@YES;
                [self->_condition lock]; self->_captureChoicePending=YES; self->_captureChoiceGeneration=readyGeneration+1; self->_captureChoiceRevision=readyRevision;
                self->_inactive=YES; [self updatePauseLocked]; pausePendingCleared=!self->_captureChoicePending && !self->_captureInputActive && !self->_captureInputCooling; [self->_condition unlock]; stage=8; }
            break;
        case 8:
            if (!paused || !suspended) break;
            if (!pausePendingCleared || !zeroLease || self->_buttons[@1].enabled || !paired || !p->ready || p->revision!=readyRevision) {finish(@"pause_pending_cleanup_failed");return;}
            checks[@"pauseCancelsPending"]=@YES; baselineReads=reads; [self resume]; stage=9; break;
        case 9:
            if (paused || reads-baselineReads<2 || !idle || !paired || !p->active || !p->ready || !visible(self->_buttons[@1]) || !self->_buttons[@1].enabled) break;
            if (!shot(@"registration-ready",NO) || !shot(@"registration-original",YES)) {finish(@"capture_failed");return;}
            [self->_buttons[@1] sendActionsForControlEvents:UIControlEventTouchDown]; [self->_buttons[@1] sendActionsForControlEvents:UIControlEventTouchUpInside];
            checks[@"typedContinue"]=@YES; baselineReads=reads; stage=10; break;
        case 10:
            if ((p && p->active) || !fadeSeen || !q->nicknameReadyFrames || !b || !b->active || !b->ready || b->mode!=HG_BATTLE_UI_YES_NO) break;
            if (!zeroLease || presses!=1 || releases!=1 || reads-lastRelease<2 || !(b->enabledMask&(1u<<nicknameIndex)) || !visible(self->_battleBar) || !visible(self->_battleButtons[0]) || !visible(self->_battleButtons[1]) || !self->_battleButtons[nicknameIndex].enabled) break;
            if (maxHeld>4) {finish(@"capture_lease_exceeded_four_pad_reads");return;}
            for (NSUInteger i=0;i<2;i++) if (!b->nameWidth[i] || !self->_battleButtons[i].configuration.image || self->_battleButtons[i].configuration.title.length) {finish(@"original_yes_no_glyph_missing");return;}
            if (![self->_battleBar.accessibilityLabel isEqual:@"是或否"] || !shot(choiceRole,NO)) {finish(@"nickname_prompt_invalid");return;}
            [self->_battleButtons[nicknameIndex] sendActionsForControlEvents:UIControlEventTouchUpInside]; checks[nicknameCheck]=@YES;
#ifdef HEARTGOLD_ORIGINAL_NAMING_CHECK
            stage=20;
#else
            stage=11;
#endif
            break;
        case 11:
            if (!q->returned || !q->fieldReady || !q->fieldReturned || q->map!=MAP_ROUTE_29 || (p && p->active) || (b && b->active) || (bag && bag->active)) break;
#ifdef HEARTGOLD_ORIGINAL_NAMING_CHECK
            if (!n || n->active || self->_namingScene || !namingZero || !namingDone || namingPresses!=(variant==CaptureCheckFirstVariant ? 18u : 1u)
                || namingInputs.count!=namingPresses) {finish(@"naming_exit_or_lease_failed");return;}
            checks[@"namingLeaseReleased"]=@YES; checks[@"guestNamingStorageAssertions"]=@(!q->errorLine && q->returned && q->fieldReturned);
#endif
            if (![self captureRegistrationSettlement:q variant:variant] || !requestAccepted || ownerRequests!=1) {finish(@"original_settlement_failed");return;}
            if (self->_captureScene || self->_canvas.hidden || !self->_canvas.userInteractionEnabled || self->_joystick.hidden || self->_menuButton.hidden
                || !self->_buttons[@1].enabled || !self->_buttons[@2].enabled || !zeroLease || !raw || !shot(@"field-return",NO)) {finish(@"field_controls_not_restored");return;}
            { NSMutableArray *words=[NSMutableArray new]; const uint32_t *values=(const uint32_t *)q; for (NSUInteger i=0;i<sizeof(*q)/sizeof(*values);i++) [words addObject:@(values[i])];
              [variants addObject:@{@"variant":@(variant),@"boot":@(epochBoot),@"qaWords":words,@"ownerRequests":@(ownerRequests),@"settled":@YES,@"popupLifecycleChecked":@(!q->errorLine && q->returned),
                  @"captureInputPresses":@(presses),@"captureInputReleases":@(releases),@"maxHeldPadReads":@(maxHeld),@"lastReleasePadRead":@(lastRelease)
#ifdef HEARTGOLD_ORIGINAL_NAMING_CHECK
                  ,@"namingInputPresses":@(namingPresses),@"namingInputReleases":@(namingReleases),@"namingMaxHeldPadReads":@(namingMaxHeld),
                  @"namingInputs":namingInputs ?: @[],@"guestNamingAssertionsCarried":@(!q->errorLine && q->returned),@"nameBufferDirectlyReadByUI":@NO
#endif
              }]; }
            if (![self captureRegistrationPrivateFilesEqual:privateBefore]) {finish(@"private_save_changed_before_reset");return;}
            if (variant==CaptureCheckLastVariant) {checks[@"partyAndPcSettlement"]=@(variants.count==2);checks[@"fieldControlsRestored"]=@YES;checks[@"popupLifetimeWitness"]=@(variants.count==2 && [variants[0][@"popupLifecycleChecked"] boolValue] && [variants[1][@"popupLifecycleChecked"] boolValue]);finish(@"completed");return;}
            resetBoot=boot; [self->_condition lock]; self->_checkResetPending=YES; [self->_condition unlock]; stage=12; break;
        case 12:
            if (boot!=resetBoot+1 || !resetQuiesced || !resetClean || !q->fieldReady || q->map!=MAP_ROUTE_29 || bootStep!=3 || bootHeld) break;
            if (q->started || q->requested || q->seeded || q->returned || q->fieldReturned || q->variant || q->battleGeneration || q->targetKnown || q->errorLine
                || !p || p->active || p->ready || p->done || !zeroLease || requestPending || requestInFlight || requestAccepted || ownerRequests || presses || releases || lastRelease || maxHeld
                || ![self captureRegistrationPrivateFilesEqual:privateBefore]) {finish(@"fresh_second_epoch_invalid");return;}
#ifdef HEARTGOLD_ORIGINAL_NAMING_CHECK
            if (namingPresses || namingReleases || namingLastRelease || namingMaxHeld || namingInputs.count || namingOverflow || !namingZero) {finish(@"naming_observers_not_reset");return;}
            pageTurns=aCount=namingGeneration=namingRevision=namingPressBase=0;
            namingInitSeen=namingPageBusy=namingMaxBusy=namingPostOK=namingSubmitted=namingPauseCleared=NO;
#endif
            checks[@"resetQuiescent"]=@YES; variant=CaptureCheckLastVariant; stage=0; waitSeen=fadeSeen=pausePendingCleared=NO; readyGeneration=readyRevision=0; break;
#ifdef HEARTGOLD_ORIGINAL_NAMING_CHECK
        case 20:
            if (!namingInitSeen || !n || !n->active || !n->ready || !namingPaired || !namingDone) break;
            namingGeneration=n->ownerGeneration; namingRevision=n->revision; namingPressBase=namingPresses;
            if (!shot(@"naming-ready",NO) || !shot(@"naming-original",YES)) {finish(@"capture_failed");return;}
            [self tapAt:CGPointMake(CGRectGetMaxX(self->_canvas.bounds)+1,CGRectGetMidY(self->_canvas.bounds))];
            [self->_condition lock]; BOOL outsideRejected=!self->_namingChoicePending && !self->_namingInputActive;
            self->_namingChoice=*n; self->_namingChoice.ownerGeneration++; self->_namingChoiceKeys=1; self->_namingChoiceTouch=NO; self->_namingChoicePending=YES; [self->_condition unlock];
            if (!outsideRejected) {finish(@"naming_outside_canvas_admitted");return;} baselineReads=reads; stage=21; break;
        case 21:
        case 22:
            if (reads-baselineReads<1 || !namingIdle) break;
            if (!namingPaired || !n || !n->active || !n->ready || n->ownerGeneration!=namingGeneration || n->revision!=namingRevision || namingPresses!=namingPressBase) {finish(@"stale_naming_input_advanced");return;}
            if (stage==21) {
                [self->_condition lock]; self->_namingChoice=*n; self->_namingChoice.revision++; self->_namingChoiceKeys=1; self->_namingChoiceTouch=NO; self->_namingChoicePending=YES; [self->_condition unlock];
                baselineReads=reads; stage=22;
            } else {
                checks[@"namingStaleRejected"]=@YES;
                [self->_condition lock]; self->_namingChoice=*n; self->_namingChoice.ownerGeneration++; self->_namingChoiceKeys=1; self->_namingChoicePending=YES;
                self->_inactive=YES; [self updatePauseLocked];
                namingPauseCleared=!self->_namingChoicePending && !self->_namingInputActive; [self->_condition unlock]; stage=23;
            } break;
        case 23:
            if (!paused || !suspended) break;
            if (!namingPauseCleared || !namingPaired || !n || !n->active || !n->ready || n->revision!=namingRevision
                || self->_canvas.userInteractionEnabled || self->_buttons[@1].enabled || self->_buttons[@2].enabled || namingPresses!=namingPressBase) {finish(@"naming_pause_pending_cleanup_failed");return;}
            checks[@"namingPauseClearsPending"]=@YES; baselineReads=reads; [self resume]; stage=24; break;
        case 24:
            if (paused || reads-baselineReads<2 || !namingDone || !namingPaired || !n || !n->active || !n->ready) break;
            if (variant==CaptureCheckLastVariant) {
                if (!shot(@"naming-empty",NO) || !tapNaming(213,71)) {finish(@"naming_empty_OK_not_queued");return;}
                namingSubmitted=YES; checks[@"originalOKTouch"]=@YES; stage=31;
            } else {
                if (!pressNaming(0x400)) {finish(@"naming_Select_not_queued");return;}
                pageTurns=1; stage=25;
            } break;
        case 25:
            if (!namingPaired || !n || !n->active || !n->ready || !namingDone || namingPresses!=pageTurns) break;
            if (pageTurns<3) {if (!pressNaming(0x400)) {finish(@"naming_Select_not_queued");return;}pageTurns++;break;}
            if (!namingPageBusy || !shot(@"naming-upper-restored",NO) || !tapNaming(52,97)) {finish(@"naming_upper_or_B_touch_failed");return;}
            checks[@"namingPages"]=@YES; stage=26; break;
        case 26:
            if (!namingPaired || !n || !n->active || !n->ready || !namingDone || namingPresses!=4) break;
            if (!shot(@"naming-B",NO) || !pressNaming(2)) {finish(@"naming_B_delete_not_queued");return;} stage=27; break;
        case 27:
            if (!namingPaired || !n || !n->active || !n->ready || !namingDone || namingPresses!=5) break;
            if (!shot(@"naming-backspaced",NO) || !tapNaming(36,97)) {finish(@"naming_first_A_not_queued");return;}
            checks[@"namingDelete"]=@YES; aCount=1; stage=28; break;
        case 28:
            if (!namingPaired || !n || !n->active || !n->ready || !namingDone || namingPresses!=5+aCount) break;
            if (aCount<10) {if (!tapNaming(36,97)) {finish(@"naming_A_not_queued");return;}aCount++;break;}
            if (!namingMaxBusy || !shot(@"naming-ten-A",NO) || !tapNaming(36,97)) {finish(@"naming_ten_A_or_extra_failed");return;}
            stage=29; break;
        case 29:
            if (!namingPaired || !n || !n->active || !n->ready || !namingDone || namingPresses!=16) break;
            if (!shot(@"naming-eleventh-attempt",NO) || !pressNaming(8)) {finish(@"naming_Start_not_queued");return;}
            checks[@"namingTenPlusExtra"]=@YES; stage=30; break;
        case 30:
            if (!namingPaired || !n || !n->active || !n->ready || !namingDone || namingPresses!=17) break;
            if (q->returned || q->fieldReturned || !self->_namingScene || !shot(@"naming-start-positioned",NO)) {finish(@"Start_submitted_or_left_naming");return;}
            checks[@"startOnlyLocatesOK"]=@YES;
            if (!tapNaming(213,71)) {finish(@"naming_original_OK_not_queued");return;}
            namingSubmitted=YES; checks[@"originalOKTouch"]=@YES; stage=31; break;
        case 31:
            if (!n || n->active || !namingPostOK || self->_namingScene || !namingDone) break;
            if (!shot(@"naming-inactive",NO)) {finish(@"capture_failed");return;} stage=11; break;
#endif
        default: finish(@"invalid_stage");return;
        }
    };
}
#endif
@end
