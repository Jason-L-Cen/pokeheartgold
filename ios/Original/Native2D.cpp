// SPDX-License-Identifier: GPL-3.0-or-later
#include "Native2DRaster.h"
#include <nitro/hw/native_graphics.h>
#include <nitro/gx/native_geometry.h>
#include "../ThirdParty/libntr-g2/libraries/sim/src/sim_g2.cpp"

uaddr HGNativeFrameBuffer(void) {
    alignas(16) static u8 pixels[1024*192*4];
    return (uaddr)pixels;
}

static unsigned limit16(unsigned value) { return value > 16 ? 16 : value; }
static void paletteBank(u8 *destination, unsigned bank, unsigned count) {
    const u8 *source = (const u8 *)HGNativeGraphicsBank(bank);
    for (unsigned i = 0; i < count; i++) destination[i] |= source[i];
}
static void extendedPalettes(unsigned engine, u8 *objectPalette) {
    memset(HGNativeBGExt[engine], 0, 32768); memset(objectPalette, 0, 8192);
    if (engine) {
        if ((reg_GX_VRAMCNT_H & 0x87) == 0x82) paletteBank(HGNativeBGExt[engine], 7, 32768);
        if ((reg_GX_VRAMCNT_I & 0x87) == 0x83) paletteBank(objectPalette, 8, 8192);
    } else {
        if ((reg_GX_VRAMCNT_E & 0x87) == 0x84) paletteBank(HGNativeBGExt[engine], 4, 32768);
        const u8 control[] = {reg_GX_VRAMCNT_F, reg_GX_VRAMCNT_G};
        for (unsigned i = 0; i < 2; i++) {
            if ((control[i] & 0x87) == 0x84)
                paletteBank(HGNativeBGExt[engine] + ((control[i] >> 3) & 1) * 16384, 5 + i, 16384);
            if ((control[i] & 0x87) == 0x85) paletteBank(objectPalette, 5 + i, 8192);
        }
    }
}
static BOOL interval(unsigned position, u16 edges) {
    unsigned first = edges >> 8, end = edges & 255;
    return first <= end ? position >= first && position < end : position >= first || position < end;
}
static unsigned windowMask(unsigned engine, u32 display, unsigned x, unsigned y, unsigned objectX) {
    if (!(display & 0xE000)) return 0x3F;
    u16 horizontal[2] = {engine ? reg_G2S_DB_WIN0H : reg_G2_WIN0H, engine ? reg_G2S_DB_WIN1H : reg_G2_WIN1H};
    u16 vertical[2] = {engine ? reg_G2S_DB_WIN0V : reg_G2_WIN0V, engine ? reg_G2S_DB_WIN1V : reg_G2_WIN1V};
    unsigned inside = engine ? reg_G2S_DB_WININ : reg_G2_WININ;
    for (unsigned window = 0; window < 2; window++)
        if ((display & (0x2000 << window)) && interval(x, horizontal[window]) && interval(y, vertical[window]))
            return (inside >> (window * 8)) & 63;
    unsigned outside = engine ? reg_G2S_DB_WINOUT : reg_G2_WINOUT;
    return (outside >> (((display & 0x8000) && OBJWindow[engine][objectX]) ? 8 : 0)) & 63;
}
static u16 brighten(u16 color, unsigned mode, unsigned amount) {
    u16 result = 0; amount = limit16(amount);
    for (unsigned shift = 0; shift < 15; shift += 5) {
        unsigned value = (color >> shift) & 31;
        if (mode == 1) value += (31 - value) * amount / 16;
        else if (mode == 2) value -= value * amount / 16;
        result |= value << shift;
    }
    return result;
}
static u16 alphaBlend(u16 first, u16 second, unsigned a, unsigned b) {
    u16 result = 0; a = limit16(a); b = limit16(b);
    for (unsigned shift = 0; shift < 15; shift += 5) {
        unsigned value = (((first >> shift) & 31) * a + ((second >> shift) & 31) * b) / 16;
        result |= (value > 31 ? 31 : value) << shift;
    }
    return result;
}
struct Pixel { u16 color; unsigned layer, depth; u32 object; unsigned alpha=31; };
static u16 geometryBlend(u16 first,u16 second,unsigned alpha) {
    u16 result=0;
    for (unsigned shift=0;shift<15;shift+=5) {
        unsigned value=(((first>>shift)&31)*(alpha+1)+((second>>shift)&31)*(31-alpha))>>5;
        result|=(u16)(value<<shift);
    }
    return result;
}
static void insert(Pixel pixel, Pixel &first, Pixel &second) {
    if (pixel.depth < first.depth) { second = first; first = pixel; }
    else if (pixel.depth < second.depth) second = pixel;
}

BOOL HGNativeRender2D(u32 engine, u8 *rgba, u32 size) {
    return HGNativeRender2DView(engine,256,rgba,size);
}
static BOOL renderView(u32 engine, u32 width, u8 *rgba, u32 size, BOOL battle);
BOOL HGNativeRender2DView(u32 engine, u32 width, u8 *rgba, u32 size) {
    return renderView(engine,width,rgba,size,FALSE);
}
BOOL HGNativeRenderBattleView(u32 width, u8 *rgba, u32 size) {
    return renderView(0,width,rgba,size,TRUE);
}
static BOOL renderView(u32 engine, u32 width, u8 *rgba, u32 size, BOOL battle) {
    if (engine > 1 || width<256 || width>(battle ? 1024 : 512) || (width&1) || (engine && width!=256) || !rgba || size < width * 192 * 4) return FALSE;
    u32 display = engine ? reg_GXS_DB_DISPCNT : reg_GX_DISPCNT;
    unsigned mode = (display >> 16) & (engine ? 1 : 3), bgMode = display & 7;
    if (width!=256 && !(display&0x80) && mode && (mode!=1 || (!battle && ((display&0x108)!=0x108 || (display&0xE000) || (reg_G2_BG0OFS&511))))) return FALSE;
    if (!(display & 0x80) && mode) {
        if (mode == 3) return FALSE; // Main-memory display FIFO needs an active producer.
        if (mode == 1 && (((display & 0x100) && ((engine && (display & 8)) || bgMode == 6)) ||
                         (bgMode == 6 && (display & 0x400)) || bgMode == 7)) return FALSE;
    }
    HGNativeSnapshotGraphics(0x07000000 + engine * 0x400, oamMemory[engine], 0x400);
    u16 *oam = (u16 *)oamMemory[engine];
    if (!(display & 0x80) && mode == 1 && (display & 0x1000)) {
        for (unsigned object = 0; object < 128; object++) {
            u16 attributes = oam[object * 4];
            if (!(attributes & 0x100) && (attributes & 0x200)) continue;
            if ((attributes & 0xC00) == 0xC00 && (display & 0x60) == 0x60) return FALSE;
        }
    }
    if ((display & 0x80) || !mode) { memset(rgba, 255, width * 192 * 4); return TRUE; }
    u16 master = engine ? reg_GXS_DB_MASTER_BRIGHT : reg_GX_MASTER_BRIGHT;
    if (mode == 2) {
        const u16 *bitmap = (const u16 *)HGNativeGraphicsBank((display >> 18) & 3);
        for (unsigned i = 0; i < 256 * 192; i++) {
            u16 color = brighten(bitmap[i], master >> 14, master & 31);
            SIM_u16ToRGB(color, rgba + i * 4, rgba + i * 4 + 1, rgba + i * 4 + 2); rgba[i * 4 + 3] = 255;
        }
        return TRUE;
    }
    HGNativeSnapshotGraphics(engine ? 0x06200000 : 0x06000000, bgMemory[engine], engine ? 0x20000 : 0x80000);
    HGNativeSnapshotGraphics(engine ? 0x06600000 : 0x06400000, objMemory[engine], engine ? 0x20000 : 0x40000);
    HGNativeSnapshotGraphics(0x05000000 + engine * 0x400, paletteMemory[engine], 0x400);
    alignas(4) u8 objectPalette[8192]; extendedPalettes(engine, objectPalette);
    memset(HGNativeBG[engine], 0, sizeof(HGNativeBG[engine]));
    u16 controls[4] = {engine ? reg_G2S_DB_BG0CNT : reg_G2_BG0CNT, engine ? reg_G2S_DB_BG1CNT : reg_G2_BG1CNT,
                       engine ? reg_G2S_DB_BG2CNT : reg_G2_BG2CNT, engine ? reg_G2S_DB_BG3CNT : reg_G2_BG3CNT};
    BOOL geometryLayer=!engine && (display&0x108)==0x108;
    // Main rendering owns these buffers; sub layers are rebuilt when that engine is rendered.
    unsigned geometryWidth=battle && width>512 ? 512 : width;
    u8 *geometryPixels=geometryWidth==256 ? HGNativeBG[0][0] : (u8 *)HGNativeBG[1];
    if (geometryLayer) {
        if (controls[0]&0x40) return FALSE;
        if (!HGNativeRender3DView(1,geometryWidth,geometryPixels,geometryWidth*192*4)) {
            if (!battle || geometryWidth==256 || !HGNativeRender3DView(1,256,geometryPixels,256*192*4)) return FALSE;
            geometryWidth=256;
        }
    }
    // shortcut: backdrop extension handles text BG3; add affine/bitmap sampling for those battle effects.
    BOOL backdrop=battle && width>256 && bgMode<=1 && (display&0x800) && !(controls[3]&0x40);
    for (unsigned bg = 0; bg < 4; bg++) if ((display & (0x100 << bg)) && !(geometryLayer && !bg) && !(backdrop && bg==3)) G2SIM_DrawBG(bg, bgMode, 0, engine);
    unsigned blend = engine ? reg_G2S_DB_BLDCNT : reg_G2_BLDCNT;
    unsigned coefficients = engine ? reg_G2S_DB_BLDALPHA : reg_G2_BLDALPHA;
    unsigned fade = engine ? reg_G2S_DB_BLDY : reg_G2_BLDY;
    unsigned mosaic = engine ? reg_G2S_DB_MOSAIC : reg_G2_MOSAIC;
    const u16 *palette = (const u16 *)paletteMemory[engine];
    // Keep the 9-bit OBJ camera aligned with the 512-pixel 3D raster; only the backdrop is wider.
    // shortcut: a frame uses one register snapshot; raster-time changes require scanline-driven rendering.
    for (unsigned y = 0; y < 192; y++) {
        unsigned objectWidth=battle && width>512 ? 512 : battle ? width : 256;
        u32 objects[512] = {}; memset(OBJWindow[engine], 0, objectWidth);
        memset(OBJLine[engine], 0, objectWidth*sizeof(u32));
        for (int priority = 3; priority >= 0; priority--) {
            G2SIM_DrawOBJ(y, engine, priority << 10, objectWidth, battle ? (objectWidth-256)/2 : 0);
        }
        // OBJ mosaic latches after sprite priority resolution, including transparent pixels.
        // Rules verified against melonDS GPU2D_Soft.cpp ApplySpriteMosaicX (GPL-3.0-or-later).
        unsigned block=((mosaic>>8)&15)+1,origin=battle ? (objectWidth-256)/2 : 0;
        u32 latch=0;
        for (unsigned x=0;x<objectWidth;x++) {
            u32 current=OBJLine[engine][x];
            if (((x+block-origin%block)%block)==0 || !(current&0x100000) || !(latch&0x100000) || (current&0x30000)<(latch&0x30000)) latch=current;
            if (latch&0x40000) objects[x]=latch;
        }
        for (unsigned x = 0; x < width; x++) {
            if (backdrop && !(x&255)) {
                memset(HGNativeBG[engine][3]+y*256*4,0,256*4);
                DrawBGLine_Text(y,3,0,engine,(s32)x-(s32)(width-256)/2);
            }
            unsigned canonicalX=x-(width-256)/2;
            BOOL inside=canonicalX<256;
            unsigned objectX=battle ? x-(width-objectWidth)/2 : canonicalX;
            unsigned outside=engine ? reg_G2S_DB_WINOUT : reg_G2_WINOUT;
            unsigned mask = inside ? windowMask(engine, display, canonicalX, y, objectX) :
                battle ? ((display&0xE000) ? ((outside>>(((display&0x8000) && objectX<objectWidth && OBJWindow[engine][objectX]) ? 8 : 0))&63) : 63) : ((display>>8)&1)|32;
            Pixel first = {palette[0], 5, 1000, 0}, second = {0, 6, 1001, 0};
            for (unsigned bg = 0; bg < 4; bg++) {
                if (!(mask & (1 << bg))) continue;
                unsigned sampleX = canonicalX, sampleY = y,stride=256;
                const u8 *layer=HGNativeBG[engine][bg];
                if (geometryLayer && !bg) {
                    stride=geometryWidth; layer=geometryPixels;
                    if (battle) {
                        if (reg_G2_BG0OFS&511) {
                            if (!inside) continue;
                            unsigned shifted=(canonicalX+(reg_G2_BG0OFS&511))&511;
                            if (shifted>=256) continue;
                            sampleX=shifted+(geometryWidth-256)/2;
                        } else sampleX=x-(width-geometryWidth)/2;
                    } else sampleX=width==256 ? (x+(reg_G2_BG0OFS&511))&511 : x;
                    if (sampleX>=geometryWidth) continue;
                } else if (backdrop && bg==3) {
                    sampleX=x&255;
                } else if (!inside) continue;
                if (controls[bg] & 0x40) { sampleX -= canonicalX % ((mosaic & 15) + 1); sampleY -= y % (((mosaic >> 4) & 15) + 1); }
                const u8 *pixel = layer + (sampleY * stride + sampleX) * 4;
                if (pixel[3]) insert({(u16)((pixel[0] >> 3) | ((pixel[1] >> 3) << 5) | ((pixel[2] >> 3) << 10)),
                                     bg, (controls[bg] & 3) * 8 + bg + 1, 0,
                                     geometryLayer && !bg ? (pixel[3]*31u+127)/255 : 31}, first, second);
            }
            u32 object = battle ? (objectX<objectWidth ? objects[objectX] : 0) : inside ? objects[canonicalX] : 0;
            if ((mask & 16) && object) {
                u16 color;
                if (object & 0x40000000) color = object & 0x7FFF;
                else if (object & 0x1000) color = palette[256 + (object & 255)];
                else color = ((const u16 *)objectPalette)[object & 4095];
                insert({color, 4, ((object >> 16) & 3) * 8, object}, first, second);
            }
            u16 color = first.color;
            if (mask & 32) {
                unsigned effect = (blend >> 6) & 3;
                BOOL translucent = first.layer == 4 && (first.object & 0x80000000);
                BOOL target2 = second.layer < 6 && (blend & (1 << (second.layer + 8)));
                if (geometryLayer && first.layer==0 && target2) color=geometryBlend(first.color,second.color,first.alpha);
                else if ((translucent || (effect == 1 && (blend & (1 << first.layer)) && !(geometryLayer && first.layer==0))) && target2) {
                    unsigned a = coefficients & 31, b = (coefficients >> 8) & 31;
                    if (first.object & 0x40000000) { a = (first.object >> 24) & 31; b = 16 - a; }
                    color = alphaBlend(first.color, second.color, a, b);
                } else if ((blend & (1 << first.layer)) && effect >= 2) color = brighten(color, effect - 1, fade & 31);
            }
            color = brighten(color, master >> 14, master & 31);
            u8 *pixel = rgba + (y * width + x) * 4;
            SIM_u16ToRGB(color, pixel, pixel + 1, pixel + 2); pixel[3] = 255;
        }
    }
    return TRUE;
}
