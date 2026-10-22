// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef HEARTGOLD_NATIVE_GRAPHICS_H
#define HEARTGOLD_NATIVE_GRAPHICS_H

#include <nitro/types.h>

#ifdef __cplusplus
extern "C" {
#endif
uaddr HGNativeRegisterAddress(u32 address, u32 width);
enum { HG_NATIVE_RASTER_LINE, HG_NATIVE_RASTER_HBLANK, HG_NATIVE_RASTER_FINISH,
       HG_NATIVE_RASTER_AFFINE_SHIFT=8 };
// Owner-only display transaction; the two-argument type matches the original IRQ guard.
void HGNativeRasterStep(u32 line, void *phase);
// No driver: row zero freezes one static view until End. A partial driver frame is rejected.
BOOL HGNativeSetRasterRow(u32 line);
void HGNativeEndRasterReplay(void);
u32 HGNativeRasterAffineWrites(u32 engine,u32 line);
void HGNativeMarkAffineWrite(uaddr address,u32 size);
void HGNativeInvalidateRasterFrame(void);
typedef struct HGNativeDisplayCaptureInfo {
    u32 control, displayControl, epoch;
} HGNativeDisplayCaptureInfo;
// Owner-only completed epoch. Canceled, replaced and consumed requests return FALSE without mutation.
// Frame-end capture uses row-zero physical memory; midline CPU VRAM feedback/writes are unsupported.
BOOL HGNativeBeginDisplayCapture(HGNativeDisplayCaptureInfo *output);
// One complete source-B row in RGB555A1; the frozen physical source never aliases the destination.
BOOL HGNativeReadDisplayCapture(const HGNativeDisplayCaptureInfo *capture,u32 line,u16 *output,u32 pixels);
// Commit final packed pixels once to live LCDC memory, preserving the frozen display view.
BOOL HGNativeCompleteDisplayCapture(const HGNativeDisplayCaptureInfo *capture,const u16 *source,u32 pixels);
// Hardware owner calls once at SYS FINISH; ordinary rendering never advances capture.
void HGNativeFinishDisplayCapture(void);
// Normalize a native register alias before sending bounded DS-address DMA ranges.
uaddr HGNativeRegisterDSAddress(uaddr address);
BOOL HGNativeIsGraphicsRegisterSpan(uaddr address,u32 size);
// CPU palette alias used by original direct HW_PLTT writes.
uaddr HGNativePaletteAddress(void);
// Returns the contiguous native span, shortening size at a graphics bank boundary.
// Unmapped/unsupported DS addresses return NULL; native pointers pass through.
void *HGNativeMemorySpan(uaddr address, u32 *size);
// Fill CPU-visible VRAM banks; holes discard writes and overlapping mappings broadcast them.
BOOL HGNativeFillVRAM(uaddr address, u32 value, u32 size, u32 width);
void HGNativeSnapshotGraphics(u32 address, void *destination, u32 size);
const void *HGNativeGraphicsBank(u32 bank);
// GPU texture/palette views; unmapped bytes read zero, overlapping banks are ORed.
BOOL HGNativeReadTextureMemory(BOOL palette,u32 address,void *output,u32 size);
// Game-thread renderer. FALSE leaves output untouched for invalid/unsupported modes.
BOOL HGNativeRender2D(u32 engine, u8 *rgba, u32 size);
BOOL HGNativeRender2DView(u32 engine, u32 width, u8 *rgba, u32 size);
// Original battle layers with a wider text backdrop and uncropped OBJ/3D edges.
BOOL HGNativeRenderBattleView(u32 width, u8 *rgba, u32 size);
// Reusable owner-thread RGBA8 output span, up to 1024x192 pixels.
uaddr HGNativeFrameBuffer(void);
#ifdef __cplusplus
}
#endif
#endif
