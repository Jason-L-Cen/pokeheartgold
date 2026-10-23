// SPDX-License-Identifier: GPL-3.0-or-later
// Included only in the explicit UIKit self-check build; all input uses the real control handlers.
#include "constants/maps.h"
@implementation OriginalController (Checks)
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
        NSString *capture=nil;
        if (!stage && map==MAP_ROUTE_29 && !self->_fieldMenu.hidden && self->_fieldMenu.userInteractionEnabled) {
            for (NSUInteger i=0;i<7;i++) if (((self->_fieldMenuIcons>>(i*4))&15)==1 && [self->_fieldMenuElements[i] accessibilityActivate]) { stage=1; break; }
        } else if (stage==1 && partyReady && party->phase==HG_PARTY_UI_ROSTER) {
            for (NSUInteger i=0;i<HG_PARTY_UI_SLOTS;i++) if (party->mons[i].active && !party->mons[i].isEgg && self->_partyMons[i].enabled) {
                originalPartyRevision=party->revision; [self->_partyMons[i] sendActionsForControlEvents:UIControlEventTouchUpInside]; stage=2; break;
            }
        } else if (stage==2 && partyReady && party->phase==HG_PARTY_UI_CONTEXT) {
            for (NSUInteger i=0;i<MIN(party->actionCount,HG_PARTY_UI_ACTIONS);i++) if (party->actions[i].textId==2 && self->_partyActions[i].enabled) {
                [self->_partyActions[i] sendActionsForControlEvents:UIControlEventTouchUpInside]; stage=3; break;
            }
        } else if (stage==3 && ready && summary->phase==HG_SUMMARY_UI_INPUT && summary->page==0 && self->_summaryNavigation[2].enabled) {
            report[@"actualSummaryOriginalInfo"]=@(native && self->_summaryPreview.image.size.width==256 && self->_summaryPreview.image.size.height==192
                && self->_summaryContent.image.size.width==144 && self->_summaryContent.image.size.height==160
                && self->_summaryPictures[@"nickname"]!=nil && self->_summaryTitle.image!=nil);
            capture=@"original-summary-info-check.png"; [self->_summaryNavigation[2] sendActionsForControlEvents:UIControlEventTouchUpInside]; stage=4;
        } else if (stage==4 && ready && summary->phase==HG_SUMMARY_UI_INPUT && summary->page==1 && self->_summaryNavigation[2].enabled) {
            for (NSUInteger i=0;i<HG_SUMMARY_UI_MOVES;i++) if ((summary->moveMask&(1u<<i)) && self->_summaryMoves[i].enabled) {
                moveIndex=i; const HGNativeSummaryUIMove *move=&summary->moves[i];
                report[@"actualSummaryOriginalMoves"]=@(native && self->_summaryContent.hidden && self->_summaryMoveImages[i].image.size.width==move->cropWidth
                    && self->_summaryMoveImages[i].image.size.height==move->cropHeight && self->_summaryMoves[i].bounds.size.height>=44);
                capture=@"original-summary-moves-check.png"; [self->_summaryNavigation[2] sendActionsForControlEvents:UIControlEventTouchUpInside]; stage=5; break;
            }
        } else if (stage==5 && ready && summary->phase==HG_SUMMARY_UI_INPUT && summary->page==2 && self->_summaryNavigation[1].enabled) {
            report[@"actualSummaryOriginalRibbons"]=@(native && self->_summaryContent.image.size.width==144 && self->_summaryContent.image.size.height==160);
            report[@"actualSummaryRibbonEntryPreserved"]=@(summary->ribbonCount ? !self->_summaryNavigation[5].hidden && self->_summaryNavigation[5].enabled : self->_summaryNavigation[5].hidden);
            capture=@"original-summary-ribbons-check.png"; [self->_summaryNavigation[1] sendActionsForControlEvents:UIControlEventTouchUpInside]; stage=6;
        } else if (stage==6 && ready && summary->phase==HG_SUMMARY_UI_INPUT && summary->page==1 && self->_summaryMoves[moveIndex].enabled) {
            [self->_summaryMoves[moveIndex] sendActionsForControlEvents:UIControlEventTouchUpInside]; stage=7;
        } else if (stage==7 && ready && summary->phase==HG_SUMMARY_UI_MOVE_SELECT && summary->page==1 && self->_summaryNavigation[0].enabled) {
            BOOL backOnly=YES; for (NSUInteger i=1;i<6;i++) if (!self->_summaryNavigation[i].hidden) backOnly=NO;
            report[@"actualSummaryOriginalMoveDetails"]=@(native && self->_summaryContent.image.size.width==120 && self->_summaryContent.image.size.height==160
                && self->_summaryMoveImages[moveIndex].image.size.width==summary->moves[moveIndex].cropWidth);
            report[@"actualSummaryNoSwapControl"]=@(backOnly && self->_buttons[@1].hidden && self->_buttons[@2].hidden);
            capture=@"original-summary-move-detail-check.png"; [self->_summaryNavigation[0] sendActionsForControlEvents:UIControlEventTouchUpInside]; stage=8;
        } else if (stage==8 && ready && summary->phase==HG_SUMMARY_UI_INPUT && summary->page==1 && self->_summaryNavigation[0].enabled) {
            report[@"actualSummaryMoveBackPreservesSummary"]=@(native);
            report[@"actualSummaryAllNativePages"]=@(pages==7);
            [self->_summaryNavigation[0] sendActionsForControlEvents:UIControlEventTouchUpInside]; stage=9;
        } else if (stage==9 && valid && !summary->active && partyReady && party->phase==HG_PARTY_UI_ROSTER && self->_partyBack.enabled) {
            report[@"actualSummaryReturnsNewParty"]=@(party->revision>originalPartyRevision && !self->_partyPanel.hidden && self->_summaryPanel.hidden);
            [self->_partyBack sendActionsForControlEvents:UIControlEventTouchUpInside]; stage=10;
        } else if (stage==10 && valid && !summary->active && self->_displayedPartyUIState.length==sizeof(*party) && !party->active
            && map==MAP_ROUTE_29 && self->_summaryPanel.hidden && self->_partyPanel.hidden) {
            report[@"actualSummarySingleScreenFlow"]=@YES; step=nil; completion(); return;
        }
        if (capture) {
            UIGraphicsBeginImageContextWithOptions(self.view.bounds.size,NO,0); [self.view drawViewHierarchyInRect:self.view.bounds afterScreenUpdates:YES];
            UIImage *image=UIGraphicsGetImageFromCurrentImageContext(); UIGraphicsEndImageContext();
            NSURL *folder=[NSFileManager.defaultManager URLsForDirectory:NSDocumentDirectory inDomains:NSUserDomainMask].firstObject;
            [UIImagePNGRepresentation(image) writeToURL:[folder URLByAppendingPathComponent:capture] options:NSDataWritingAtomic error:nil];
        }
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
        NSString *capture=nil;
        if (!stage && map==MAP_ROUTE_29 && !self->_fieldMenu.hidden && self->_fieldMenu.userInteractionEnabled) {
            for (NSUInteger i=0;i<7;i++) if (((self->_fieldMenuIcons>>(i*4))&15)==2 && [self->_fieldMenuElements[i] accessibilityActivate]) { stage=1; break; }
        } else if (stage==1 && ready && bag->phase==HG_BAG_UI_INPUT) {
            capture=@"original-bag-pockets-check.png"; stage=2;
        } else if (stage==2 && ready && bag->phase==HG_BAG_UI_INPUT) {
            if (target!=UINT32_MAX && bag->pocketIndex==target) { target=UINT32_MAX; nextPocket++; }
            if (target==UINT32_MAX) {
                while (nextPocket<HG_BAG_UI_POCKETS && (!(bag->pocketMask&(1u<<nextPocket)) || bag->pocketIndex==nextPocket)) nextPocket++;
                if (nextPocket<HG_BAG_UI_POCKETS && self->_bagPockets[nextPocket].enabled) {
                    target=(uint32_t)nextPocket; [self->_bagPockets[nextPocket] sendActionsForControlEvents:UIControlEventTouchUpInside];
                } else if (nextPocket==HG_BAG_UI_POCKETS) {
                    report[@"actualBagPocketsVisited"]=@((visited&bag->pocketMask)==bag->pocketMask && bag->pocketMask!=0);
                    stage=3;
                }
            }
        } else if (stage==3 && ready && bag->phase==HG_BAG_UI_INPUT) {
            if (firstItemPocket==UINT32_MAX) stage=6;
            else if (bag->pocketIndex==firstItemPocket) { target=firstItemPocket; stage=4; }
            else if (self->_bagPockets[firstItemPocket].enabled) {
                target=firstItemPocket; [self->_bagPockets[firstItemPocket] sendActionsForControlEvents:UIControlEventTouchUpInside]; stage=4;
            }
        } else if (stage==4 && ready && bag->phase==HG_BAG_UI_INPUT && bag->pocketIndex==target) {
            for (NSUInteger i=0;i<MIN(bag->visibleCount,HG_BAG_UI_ITEMS);i++) if ((bag->itemMask&(1u<<i)) && self->_bagItems[i].enabled) {
                const HGNativeBagUIItem *item=&bag->items[i]; UIImage *image=self->_bagItemImages[i].image;
                report[@"actualBagOriginalItemCard"]=@(!self->_bagPanel.hidden && self->_menuButton.hidden && self->_canvas.hidden && image.size.width==item->cropWidth && image.size.height==item->cropHeight
                    && self->_bagPreview.image.size.width==256 && self->_bagPreview.image.size.height==192);
                capture=@"original-bag-list-check.png"; [self->_bagItems[i] sendActionsForControlEvents:UIControlEventTouchUpInside]; stage=5; break;
            }
        } else if (stage==5 && ready && bag->phase==HG_BAG_UI_ITEM_MENU && self->_bagNavigation[0].enabled) {
            BOOL glyph=NO;
            for (NSUInteger i=0;i<HG_BAG_UI_ACTIONS;i++) if ((bag->actionMask&(1u<<i)) && self->_bagActions[i].configuration.image) glyph=YES;
            report[@"actualBagOriginalActionGlyph"]=@(glyph && self->_bagSelectedEntry.image!=nil);
            capture=@"original-bag-item-check.png"; [self->_bagNavigation[0] sendActionsForControlEvents:UIControlEventTouchUpInside]; stage=6;
        } else if (stage==6 && ready && bag->phase==HG_BAG_UI_INPUT && self->_bagNavigation[0].enabled) {
            report[@"actualBagClosedPromptHidden"]=@(firstItemPocket!=UINT32_MAX && self->_bagPrompt.hidden && !self->_bagPrompt.image);
            [self->_bagNavigation[0] sendActionsForControlEvents:UIControlEventTouchUpInside]; stage=7;
        } else if (stage==7 && valid && !bag->active && map==MAP_ROUTE_29 && self->_bagPanel.hidden) {
            report[@"actualBagSingleScreenFlow"]=@YES; step=nil; completion(); return;
        }
        if (capture) {
            UIGraphicsBeginImageContextWithOptions(self.view.bounds.size,NO,0); [self.view drawViewHierarchyInRect:self.view.bounds afterScreenUpdates:YES];
            UIImage *image=UIGraphicsGetImageFromCurrentImageContext(); UIGraphicsEndImageContext();
            NSURL *folder=[NSFileManager.defaultManager URLsForDirectory:NSDocumentDirectory inDomains:NSUserDomainMask].firstObject;
            [UIImagePNGRepresentation(image) writeToURL:[folder URLByAppendingPathComponent:capture] options:NSDataWritingAtomic error:nil];
        }
        dispatch_after(dispatch_time(DISPATCH_TIME_NOW,NSEC_PER_SEC/10),dispatch_get_main_queue(),step);
    };
    step();
}
- (void)checkPartyFlow:(NSMutableDictionary *)report completion:(dispatch_block_t)completion {
    __block NSUInteger stage=0,attempts=0; __block void (^step)(void);
    report[@"actualPartySingleScreenFlow"]=@NO;
    step=^{
        const HGNativePartyUIState *party=self->_displayedPartyUIState.bytes;
        BOOL ready=self->_displayedPartyUIState.length==sizeof(*party) && party->active && party->ready;
        [self->_condition lock]; BOOL failed=self->_failure!=nil || self->_finished; uint32_t map=self->_fieldMapId; [self->_condition unlock];
        if (failed || ++attempts>240) { step=nil; completion(); return; }
        NSString *capture=nil;
        if (!stage && map==MAP_ROUTE_29 && !self->_fieldMenu.hidden && self->_fieldMenu.userInteractionEnabled) {
            for (NSUInteger i=0;i<7;i++) if (((self->_fieldMenuIcons>>(i*4))&15)==1 && [self->_fieldMenuElements[i] accessibilityActivate]) { stage=1; break; }
        } else if (stage==1 && ready && party->phase==HG_PARTY_UI_ROSTER) {
            BOOL native=!self->_partyPanel.hidden && self->_menuButton.hidden && self->_canvas.hidden && !self->_canvas.userInteractionEnabled;
            for (NSUInteger i=0;i<HG_PARTY_UI_SLOTS;i++) if (party->mons[i].active && !party->mons[i].isEgg && !party->mons[i].heldItem) {
                report[@"actualPartyOriginalCard"] = @(native && self->_partyCards[i].image.size.width==128 && self->_partyCards[i].image.size.height==48);
                capture=@"original-party-roster-check.png";
                [self->_partyMons[i] sendActionsForControlEvents:UIControlEventTouchUpInside]; stage=2; break;
            }
        } else if ((stage==2 || stage==3) && ready && party->phase==(stage==2 ? HG_PARTY_UI_CONTEXT : HG_PARTY_UI_SUBCONTEXT)) {
            for (NSUInteger i=0;i<MIN(party->actionCount,HG_PARTY_UI_ACTIONS);i++) if (party->actions[i].textId==(stage==2 ? 3 : 10)) {
                UIButton *button=self->_partyActions[i];
                if (!button.enabled) break;
                if (stage==2) {
                    report[@"actualPartyOriginalActionGlyph"] = @(button.configuration.image!=nil && self->_partySelectedName.image!=nil && self->_partySelectedItem.image!=nil);
                    capture=@"original-party-context-check.png";
                } else { report[@"actualPartyOriginalItemMenu"]=@(party->actionCount==3); capture=@"original-party-item-check.png"; }
                [button sendActionsForControlEvents:UIControlEventTouchUpInside]; stage++; break;
            }
        } else if (stage==4 && ready && party->phase==HG_PARTY_UI_ROSTER && self->_partyBack.enabled) {
            [self->_partyBack sendActionsForControlEvents:UIControlEventTouchUpInside]; stage=5;
        } else if (stage==5 && self->_displayedPartyUIState.length==sizeof(*party) && !party->active && map==MAP_ROUTE_29 && self->_partyPanel.hidden) {
            report[@"actualPartySingleScreenFlow"]=@YES; step=nil; completion(); return;
        }
        if (capture) {
            UIGraphicsBeginImageContextWithOptions(self.view.bounds.size,NO,0);
            [self.view drawViewHierarchyInRect:self.view.bounds afterScreenUpdates:YES];
            UIImage *image=UIGraphicsGetImageFromCurrentImageContext(); UIGraphicsEndImageContext();
            NSURL *folder=[NSFileManager.defaultManager URLsForDirectory:NSDocumentDirectory inDomains:NSUserDomainMask].firstObject;
            [UIImagePNGRepresentation(image) writeToURL:[folder URLByAppendingPathComponent:capture] options:NSDataWritingAtomic error:nil];
        }
        dispatch_after(dispatch_time(DISPATCH_TIME_NOW,NSEC_PER_SEC/10),dispatch_get_main_queue(),step);
    };
    step();
}
- (void)checkControls {
    NSMutableDictionary *report = [NSMutableDictionary new];
    uint8_t tiles[65]={0}; tiles[1]=0x21; tiles[33]=0x03;
    HGNativePartyUIWindow window={.pixelBuffer=1,.tileWidth=2,.tileHeight=1,.clearColor=0};
    window.palette[1]=31; window.palette[2]=31<<5; window.palette[3]=31<<10;
    UIImage *glyph=OriginalWindowImage(&window,tiles,sizeof(tiles));
    NSData *glyphPixels=CFBridgingRelease(CGDataProviderCopyData(CGImageGetDataProvider(glyph.CGImage)));
    const uint8_t *glyphBytes=glyphPixels.bytes;
    report[@"partyOriginalTileAndPalette"] = @(glyphPixels.length==16*8*4 && glyphBytes[0]==255 && !glyphBytes[1] && glyphBytes[3]==255
        && glyphBytes[5]==255 && glyphBytes[7]==255 && glyphBytes[34]==255 && glyphBytes[35]==255 && !glyphBytes[39]);
    tiles[1]=0; report[@"partyWindowCopiesBeforeAdvance"] = @(glyphBytes[0]==255);
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
    [_partyMons[0] sendActionsForControlEvents:UIControlEventTouchUpInside];
    uint32_t partyX=0,partyY=0; BOOL partyPressed=NO;
    [_condition lock];
    BOOL queued=_partyChoicePending && _partyChoiceRevision==10 && _partyChoiceKind==HG_PARTY_UI_TOUCH_MON;
    party->ready=0; [self takePartyTouchLocked:party reads:0 x:&partyX y:&partyY pressed:&partyPressed];
    BOOL waitsForPublish=_partyChoicePending && !partyPressed;
    party->ready=1; [self takePartyTouchLocked:party reads:0 x:&partyX y:&partyY pressed:&partyPressed];
    BOOL press=partyPressed && partyX==64 && partyY==32 && _partyTouchActive;
    [self takePartyTouchLocked:party reads:3 x:&partyX y:&partyY pressed:&partyPressed]; BOOL sampled=partyPressed;
    [self takePartyTouchLocked:party reads:4 x:&partyX y:&partyY pressed:&partyPressed];
    report[@"partyOriginalTouchSampling"] = @(queued && waitsForPublish && press && sampled && !partyPressed && !_partyTouchActive);
    _partyChoicePending=YES; _partyChoiceRevision=9;
    [self takePartyTouchLocked:party reads:5 x:&partyX y:&partyY pressed:&partyPressed];
    report[@"partyRejectsStaleRequest"] = @(!_partyChoicePending && !_partyTouchActive && !partyPressed);
    [_condition unlock];
    party->phase=HG_PARTY_UI_SUBCONTEXT; party->actionCount=3; party->actionEnabledMask=7;
    party->actions[0].enabled=1; party->actions[0].textId=3; party->actions[0].hitX=192; party->actions[0].hitY=32;
    [self updatePartyControls:[partyData copy] pictures:nil paused:NO]; [self.view layoutIfNeeded];
    report[@"partyNativeOriginalActions"] = @(!_partyActions[0].hidden && _partyActions[0].enabled && [_partyActions[0].accessibilityLabel isEqual:@"ITEM"] && _partyActions[0].bounds.size.height>=44);
    [_partyActions[0] sendActionsForControlEvents:UIControlEventTouchUpInside]; [self pause];
    [_condition lock]; report[@"partyPauseClearsRequest"] = @(!_partyChoicePending && !_partyTouchActive); [_condition unlock]; [self resume];
    party->active=0; [self updatePartyControls:[partyData copy] pictures:nil paused:NO];
    report[@"partyExitHidesPanel"] = @(!_partyScene && _partyPanel.hidden && _partyBack.hidden);
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
    report[@"summaryNativeOriginalInfo"]=@(!_summaryPanel.hidden && _summaryPreview.image.size.width==256 && _summaryContent.image.size.width==144
        && _summaryNavigation[0].enabled && _summaryNavigation[0].bounds.size.height>=44 && [_summaryPreview.accessibilityLabel containsString:@"19/20"]);
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
        && [_summaryPreview.accessibilityLabel containsString:@"蛋"]);
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
                          [self->_condition lock]; NSDictionary *menusAudio=self->_audioChanging ? nil : [self->_audio statistics]; [self->_condition unlock];
                          NSDictionary *producerAfterMenus=[self checkAudioProducerSnapshot];
                          [self stop];
                          dispatch_after(dispatch_time(DISPATCH_TIME_NOW,2*NSEC_PER_SEC),dispatch_get_main_queue(), ^{
                            [self->_condition lock];
                            report[@"normalResetCleanup"] = @(self->_finished && !self->_env && !self->_game && !self->_wasi);
                            report[@"explicitStopDoesNotReboot"] = @(self->_finished && self->_bootCount==boots+1);
                            [self->_condition unlock];
                            BOOL passed=YES;
                            for (NSNumber *value in report.allValues) if (!value.boolValue) passed=NO;
                            report[@"passed"]=@(passed);
                            NSURL *folder=[NSFileManager.defaultManager URLsForDirectory:NSDocumentDirectory inDomains:NSUserDomainMask].firstObject;
                            NSData *audioData=[NSJSONSerialization dataWithJSONObject:@{@"beforePause":audio ?: @{},@"whilePaused":heldAudio ?: @{},@"afterResume":resumedAudio ?: @{},@"afterReset":resetAudio ?: @{},@"afterMenus":menusAudio ?: @{}} options:NSJSONWritingPrettyPrinted error:nil];
                            [audioData writeToURL:[folder URLByAppendingPathComponent:@"original-audio-metrics.json"] options:NSDataWritingAtomic error:nil];
                            NSData *producerData=[NSJSONSerialization dataWithJSONObject:@{@"version":@1,@"clock":@"CLOCK_MONOTONIC",
                                @"costsMayOverlap":@YES,@"completedSpansOnly":@YES,
                                @"appOtherMeaning":@"App setup, copies and publication; includes nested audio/SDK/FS work. A missing event does not exclude unmeasured work.",
                                @"sourceNames":@[@"none",@"hostRealtime",@"sdkPump",@"setOutput",@"detachOutput",@"modeChange"],
                                @"appSceneBits":@{@"field":@1,@"battle":@2,@"party":@4,@"bag":@8,@"summary":@16,@"subEngine":@32},
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
                    };
                    dispatch_after(dispatch_time(DISPATCH_TIME_NOW,NSEC_PER_SEC/2),dispatch_get_main_queue(),verifyReset);
                });
            });
        });
    });
}
@end
