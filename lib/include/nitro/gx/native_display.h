// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef HEARTGOLD_NATIVE_DISPLAY_H
#define HEARTGOLD_NATIVE_DISPLAY_H
#include <nitro/types.h>
#ifdef __cplusplus
extern "C" {
#endif
enum { HG_DISPLAY_LINE_CLOCKS=355*6,HG_DISPLAY_LINES=263,HG_DISPLAY_FRAME_CLOCKS=HG_DISPLAY_LINE_CLOCKS*HG_DISPLAY_LINES };
u32 HGNativeDisplayLineAt(u64 nanoseconds);
u64 HGNativeDisplayVBlanksAt(u64 nanoseconds);
u32 HGNativeDisplayLine(void);
// Returns the monotonic timestamp actually observed within scanline zero.
u64 HGNativeWaitDisplayLine0(void);
#ifdef __cplusplus
}
#endif
#endif
