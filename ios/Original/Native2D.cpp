// SPDX-License-Identifier: GPL-3.0-or-later
#include "Native2DRaster.h"
#include <nitro/hw/native_graphics.h>
#include <nitro/gx/native_geometry.h>
#include "../ThirdParty/libntr-g2/libraries/sim/src/sim_g2.cpp"

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
static unsigned windowMask(unsigned engine, u32 display, unsigned x, unsigned y) {
    if (!(display & 0xE000)) return 0x3F;
    u16 horizontal[2] = {engine ? reg_G2S_DB_WIN0H : reg_G2_WIN0H, engine ? reg_G2S_DB_WIN1H : reg_G2_WIN1H};
    u16 vertical[2] = {engine ? reg_G2S_DB_WIN0V : reg_G2_WIN0V, engine ? reg_G2S_DB_WIN1V : reg_G2_WIN1V};
    unsigned inside = engine ? reg_G2S_DB_WININ : reg_G2_WININ;
    for (unsigned window = 0; window < 2; window++)
        if ((display & (0x2000 << window)) && interval(x, horizontal[window]) && interval(y, vertical[window]))
            return (inside >> (window * 8)) & 63;
    unsigned outside = engine ? reg_G2S_DB_WINOUT : reg_G2_WINOUT;
    return (outside >> (((display & 0x8000) && OBJWindow[engine][x]) ? 8 : 0)) & 63;
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
struct Pixel { u16 color; unsigned layer, depth; u32 object; };
static void insert(Pixel pixel, Pixel &first, Pixel &second) {
    if (pixel.depth < first.depth) { second = first; first = pixel; }
    else if (pixel.depth < second.depth) second = pixel;
}

BOOL HGNativeRender2D(u32 engine, u8 *rgba, u32 size) {
    if (engine > 1 || !rgba || size < 256 * 192 * 4) return FALSE;
    u32 display = engine ? reg_GXS_DB_DISPCNT : reg_GX_DISPCNT;
    unsigned mode = (display >> 16) & (engine ? 1 : 3), bgMode = display & 7;
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
            // shortcut: OBJ mosaic needs per-object scanline sampling; reject until that driver is wired.
            if (attributes & 0x1000) return FALSE;
            if ((attributes & 0xC00) == 0xC00 && (display & 0x60) == 0x60) return FALSE;
        }
    }
    if ((display & 0x80) || !mode) { memset(rgba, 255, 256 * 192 * 4); return TRUE; }
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
    if (geometryLayer && ((controls[0]&0x40) || !HGNativeRender3D(1,HGNativeBG[0][0],sizeof(HGNativeBG[0][0])))) return FALSE;
    for (unsigned bg = 0; bg < 4; bg++) if ((display & (0x100 << bg)) && !(geometryLayer && !bg)) G2SIM_DrawBG(bg, bgMode, 0, engine);
    unsigned blend = engine ? reg_G2S_DB_BLDCNT : reg_G2_BLDCNT;
    unsigned coefficients = engine ? reg_G2S_DB_BLDALPHA : reg_G2_BLDALPHA;
    unsigned fade = engine ? reg_G2S_DB_BLDY : reg_G2_BLDY;
    unsigned mosaic = engine ? reg_G2S_DB_MOSAIC : reg_G2_MOSAIC;
    const u16 *palette = (const u16 *)paletteMemory[engine];
    // shortcut: a frame uses one register snapshot; raster-time changes require scanline-driven rendering.
    for (unsigned y = 0; y < 192; y++) {
        u32 objects[256] = {}; memset(OBJWindow[engine], 0, sizeof(OBJWindow[engine]));
        for (int priority = 3; priority >= 0; priority--) {
            memset(OBJLine[engine], 0, sizeof(OBJLine[engine])); G2SIM_DrawOBJ(y, engine, priority << 10);
            for (unsigned x = 0; x < 256; x++) if (OBJLine[engine][x] & 0xC0000) objects[x] = OBJLine[engine][x];
        }
        for (unsigned x = 0; x < 256; x++) {
            unsigned mask = windowMask(engine, display, x, y);
            Pixel first = {palette[0], 5, 1000, 0}, second = {0, 6, 1001, 0};
            for (unsigned bg = 0; bg < 4; bg++) {
                if (!(mask & (1 << bg))) continue;
                unsigned sampleX = x, sampleY = y;
                if (geometryLayer && !bg) { sampleX=(x+(reg_G2_BG0OFS&511))&511; if (sampleX>=256) continue; }
                if (controls[bg] & 0x40) { sampleX -= x % ((mosaic & 15) + 1); sampleY -= y % (((mosaic >> 4) & 15) + 1); }
                const u8 *pixel = HGNativeBG[engine][bg] + (sampleY * 256 + sampleX) * 4;
                if (pixel[3]) insert({(u16)((pixel[0] >> 3) | ((pixel[1] >> 3) << 5) | ((pixel[2] >> 3) << 10)),
                                     bg, (controls[bg] & 3) * 8 + bg + 1, 0}, first, second);
            }
            u32 object = objects[x];
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
                if ((translucent || (effect == 1 && (blend & (1 << first.layer)))) && target2) {
                    unsigned a = coefficients & 31, b = (coefficients >> 8) & 31;
                    if (first.object & 0x40000000) { a = (first.object >> 24) & 31; b = 16 - a; }
                    color = alphaBlend(first.color, second.color, a, b);
                } else if ((blend & (1 << first.layer)) && effect >= 2) color = brighten(color, effect - 1, fade & 31);
            }
            color = brighten(color, master >> 14, master & 31);
            u8 *pixel = rgba + (y * 256 + x) * 4;
            SIM_u16ToRGB(color, pixel, pixel + 1, pixel + 2); pixel[3] = 255;
        }
    }
    return TRUE;
}
