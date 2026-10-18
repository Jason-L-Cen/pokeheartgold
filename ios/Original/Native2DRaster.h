// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
#include <nitro/types.h>
#include <nitro/hw/io_reg.h>
#include <stdio.h>
#include <string.h>

#define SIM_NDS_SCREEN_WIDTH 256
#define SIM_NDS_SCREEN_HEIGHT 192
static u8 HGNativeBG[2][4][256 * 192 * 4];
static u8 HGNativeBGExt[2][32768];
static u8 bgMemory[2][0x80000], objMemory[2][0x40000], paletteMemory[2][0x400], oamMemory[2][0x400];
static u32 OBJLine[2][256];
static u8 OBJWindow[2][256];
static const u8 s_SIM_DBG_BGenable[4] = {1, 1, 1, 1}, s_SIM_DBG_BGSenable[4] = {1, 1, 1, 1};
static const u8 s_SIM_DBG_OAMenable = 1, s_SIM_DBG_OAMSenable = 1;
static const u8 s_mosaicTable[16][256] = {}; // BG mosaic is applied after sampling by the native compositor.

#define bg0tex HGNativeBG[0][0]
#define bg1tex HGNativeBG[0][1]
#define bg2tex HGNativeBG[0][2]
#define bg3tex HGNativeBG[0][3]
#define bg0stex HGNativeBG[1][0]
#define bg1stex HGNativeBG[1][1]
#define bg2stex HGNativeBG[1][2]
#define bg3stex HGNativeBG[1][3]
#define s_HW_BG_VRAM bgMemory[0]
#define s_HW_DB_BG_VRAM bgMemory[1]
#define s_HW_OBJ_VRAM objMemory[0]
#define s_HW_DB_OBJ_VRAM objMemory[1]
#define s_HW_BG_PLTT paletteMemory[0]
#define s_HW_DB_BG_PLTT paletteMemory[1]
#define s_HW_OAM oamMemory[0]
#define s_HW_DB_OAM oamMemory[1]

static inline void SIM_u16ToRGB(u16 value, u8 *r, u8 *g, u8 *b) {
    *r = ((value & 31) * 527 + 23) >> 6;
    *g = (((value >> 5) & 31) * 527 + 23) >> 6;
    *b = (((value >> 10) & 31) * 527 + 23) >> 6;
}
