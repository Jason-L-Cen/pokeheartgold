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
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, 6 * NSEC_PER_SEC), dispatch_get_main_queue(), ^{
        [self pause];
        dispatch_after(dispatch_time(DISPATCH_TIME_NOW, NSEC_PER_SEC), dispatch_get_main_queue(), ^{
            [self->_condition lock];
            uint32_t count = self->_publishedFrames;
            report[@"nativeFrames"] = @(count > 0);
            report[@"ownerSuspended"] = @(self->_suspended);
            [self->_condition unlock];
            dispatch_after(dispatch_time(DISPATCH_TIME_NOW, NSEC_PER_SEC), dispatch_get_main_queue(), ^{
                [self->_condition lock]; report[@"pauseStopsFrames"] = @(self->_publishedFrames == count); [self->_condition unlock];
                [self resume];
                dispatch_after(dispatch_time(DISPATCH_TIME_NOW, 2 * NSEC_PER_SEC), dispatch_get_main_queue(), ^{
                    [self->_condition lock];
                    report[@"resumeFrames"] = @(self->_publishedFrames > count);
                    [self->_condition unlock]; [self stop];
                    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, 2 * NSEC_PER_SEC), dispatch_get_main_queue(), ^{
                        [self->_condition lock]; report[@"normalResetCleanup"] = @(self->_finished && !self->_env && !self->_game && !self->_wasi); [self->_condition unlock];
                        BOOL passed = YES;
                        for (NSNumber *value in report.allValues) if (!value.boolValue) passed = NO;
                        report[@"passed"] = @(passed);
                        NSURL *folder = [NSFileManager.defaultManager URLsForDirectory:NSDocumentDirectory inDomains:NSUserDomainMask].firstObject;
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
