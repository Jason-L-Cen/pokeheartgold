// SPDX-License-Identifier: GPL-3.0-or-later
#import <Foundation/Foundation.h>
#include <stdint.h>

// The game owner supplies borrowed PCM. Apple's player owns scheduled copies; it never calls the game.
@interface OriginalAudio : NSObject
@property(nonatomic, readonly) NSString *failure;
- (BOOL)open:(NSError **)error;
- (BOOL)resume:(NSError **)error;
- (void)pause;
- (void)close;
- (void)enqueue:(const int16_t *)stereo frames:(size_t)frames;
- (NSDictionary<NSString *, NSNumber *> *)statistics;
@end
