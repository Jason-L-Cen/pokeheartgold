// SPDX-License-Identifier: GPL-3.0-or-later
// Included only in the explicit UIKit self-check build; all input uses the real control handlers.
@implementation OriginalController (Checks)
- (void)checkControls {
    NSMutableDictionary *report = [NSMutableDictionary new];
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
    [self pause];
    [_condition lock]; report[@"pauseClearsInput"] = @(!_keys && !_touchPressed && _paused); [_condition unlock];
    [self resume]; [self.view setNeedsLayout];
    [_buttons[@1] sendActionsForControlEvents:UIControlEventTouchDown];
    [_buttons[@1] sendActionsForControlEvents:UIControlEventTouchUpInside];
    [_condition lock]; report[@"shortTapDeliveredOnce"] = @(!_keys && [self takeKeysLocked] == 1 && ![self takeKeysLocked]); [_condition unlock];
    [_buttons[@1] sendActionsForControlEvents:UIControlEventTouchDown];
    [_buttons[@1] sendActionsForControlEvents:UIControlEventTouchCancel];
    [_condition lock]; report[@"cancelledTapClearsPending"] = @(![self takeKeysLocked]); [_condition unlock];
    NSMutableData *battleData = [NSMutableData dataWithLength:sizeof(HGNativeBattleUIState)];
    HGNativeBattleUIState *battle = battleData.mutableBytes;
    battle->active = 1; battle->revision = 7; battle->mode = HG_BATTLE_UI_COMMAND; battle->ready = 1; battle->enabledMask = 15;
    [_buttons[@0x20] sendActionsForControlEvents:UIControlEventTouchDown];
    [self updateBattleControls:[battleData copy]]; [self.view layoutIfNeeded];
    [_condition lock]; report[@"battleClearsHeldFieldInput"] = @(!_keys && !_pendingKeys && !_touchPressed); [_condition unlock];
    report[@"nativeCommandControls"] = @(!_battleBar.hidden && _battleBack.hidden && _battleButtons[0].enabled && _buttons[@1].hidden);
    report[@"nativeBattleMinimumTarget"] = @(_battleButtons[0].bounds.size.width >= 44 && _battleButtons[0].bounds.size.height >= 44);
    [_battleButtons[1] sendActionsForControlEvents:UIControlEventTouchUpInside];
    [_condition lock]; report[@"nativeBagQueued"] = @(_battleChoicePending && _battleChoiceRevision == 7 && _battleChoiceIndex == 1 && _selectedEngine == 1); [_condition unlock];
    [self pause];
    [_condition lock]; report[@"pauseClearsBattleRequest"] = @(!_battleChoicePending); [_condition unlock];
    [self resume];
    battle->mode = HG_BATTLE_UI_MOVES; battle->enabledMask = 17; battle->move[0] = 33; battle->pp[0] = 32; battle->maxPP[0] = 35;
    [self updateBattleControls:[battleData copy]];
    report[@"nativeMoveControls"] = @(!_battleBack.hidden && _battleButtons[0].enabled && !_battleButtons[1].enabled && [_battleButtons[0].accessibilityLabel containsString:@"Tackle"]);
    [_battleBack sendActionsForControlEvents:UIControlEventTouchUpInside];
    [_condition lock]; report[@"nativeMoveBackQueued"] = @(_battleChoicePending && _battleChoiceIndex == 4); _battleChoicePending = NO; _selectedEngine = 0; [_condition unlock];
    battle->mode = HG_BATTLE_UI_NONE; battle->ready = 0;
    [self updateBattleControls:[battleData copy]];
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
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, 18 * NSEC_PER_SEC), dispatch_get_main_queue(), ^{
        [self pause];
        dispatch_after(dispatch_time(DISPATCH_TIME_NOW, NSEC_PER_SEC), dispatch_get_main_queue(), ^{
            [self->_condition lock];
            uint32_t count = self->_publishedFrames;
            report[@"nativeFrames"] = @(count > 0);
            report[@"ownerSuspended"] = @(self->_suspended);
            [self->_condition unlock];
            NSDictionary *audio = [self->_audio statistics];
            report[@"audioSubmitted"] = @([audio[@"submitted"] unsignedLongLongValue] > 0);
            report[@"audioRendered"] = @([audio[@"rendered"] unsignedLongLongValue] > 0);
            report[@"originalAudioNonzero"] = @([audio[@"renderedNonzeroSamples"] unsignedLongLongValue] > 0);
            report[@"audioQueueBounded"] = @([audio[@"queued"] unsignedLongLongValue] <= 8192 && ![audio[@"dropped"] unsignedLongLongValue]);
            dispatch_after(dispatch_time(DISPATCH_TIME_NOW, NSEC_PER_SEC), dispatch_get_main_queue(), ^{
                [self->_condition lock]; report[@"pauseStopsFrames"] = @(self->_publishedFrames == count); [self->_condition unlock];
                NSDictionary *heldAudio = [self->_audio statistics];
                report[@"pauseStopsAudio"] = @([heldAudio[@"rendered"] isEqual:audio[@"rendered"]] && [heldAudio[@"submitted"] isEqual:audio[@"submitted"]]);
                [self resume];
                dispatch_after(dispatch_time(DISPATCH_TIME_NOW, 2 * NSEC_PER_SEC), dispatch_get_main_queue(), ^{
                    [self->_condition lock];
                    report[@"resumeFrames"] = @(self->_publishedFrames > count);
                    NSDictionary *resumedAudio = [self->_audio statistics];
                    report[@"resumeAudio"] = @([resumedAudio[@"rendered"] unsignedLongLongValue] > [heldAudio[@"rendered"] unsignedLongLongValue] && [resumedAudio[@"submitted"] unsignedLongLongValue] > [heldAudio[@"submitted"] unsignedLongLongValue]);
                    [self->_condition unlock]; [self stop];
                    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, 2 * NSEC_PER_SEC), dispatch_get_main_queue(), ^{
                        [self->_condition lock]; report[@"normalResetCleanup"] = @(self->_finished && !self->_env && !self->_game && !self->_wasi); [self->_condition unlock];
                        BOOL passed = YES;
                        for (NSNumber *value in report.allValues) if (!value.boolValue) passed = NO;
                        report[@"passed"] = @(passed);
                        NSURL *folder = [NSFileManager.defaultManager URLsForDirectory:NSDocumentDirectory inDomains:NSUserDomainMask].firstObject;
                        NSData *audioData=[NSJSONSerialization dataWithJSONObject:@{@"beforePause":audio,@"whilePaused":heldAudio,@"afterResume":resumedAudio} options:NSJSONWritingPrettyPrinted error:nil];
                        [audioData writeToURL:[folder URLByAppendingPathComponent:@"original-audio-metrics.json"] options:NSDataWritingAtomic error:nil];
                        NSData *data = [NSJSONSerialization dataWithJSONObject:report options:NSJSONWritingPrettyPrinted error:nil];
                        [data writeToURL:[folder URLByAppendingPathComponent:@"original-app-check.json"] options:NSDataWritingAtomic error:nil];
                        NSLog(@"Original app checks: %@", report);
                    });
                });
            });
        });
    });
}
@end
