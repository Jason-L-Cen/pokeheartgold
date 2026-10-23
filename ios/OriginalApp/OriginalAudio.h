// SPDX-License-Identifier: GPL-3.0-or-later
#import <Foundation/Foundation.h>
#include <stdint.h>

// The game supplies borrowed PCM; the native render thread consumes owned copies and never calls the game.
@interface OriginalAudio : NSObject
@property(nonatomic, readonly) NSString *failure;
- (BOOL)open:(NSError **)error;
- (BOOL)resume:(NSError **)error;
- (void)pause;
- (void)close;
- (void)enqueue:(const int16_t *)stereo frames:(size_t)frames;
- (NSDictionary<NSString *, NSNumber *> *)statistics;
#ifdef HEARTGOLD_ORIGINAL_APP_CHECK
// Zero means no timestamp is available; first-short counters must not imply a successful clock reading.
- (uint64_t)firstShortTimeNs;
#endif
@end
