// SPDX-License-Identifier: GPL-3.0-or-later
#include <nitro.h>
#include <nitro/hw/native_graphics.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(value) do { if (!(value)) { fprintf(stderr, "Graphics check line %d: %s\n", __LINE__, #value); abort(); } } while (0)
extern u32 GXi_DmaId;
void GX_InitGXState(void);

static void equalAt(uaddr address, const void *expected, u32 size) {
    u8 *actual = malloc(size); CHECK(actual);
    MI_CpuCopy8((const void *)address, actual, size);
    CHECK(memcmp(actual, expected, size) == 0); free(actual);
}

void CheckNativeGraphics(void) {
    OSi_InitVramExclusive(); GX_InitGXState();
    CHECK(OSi_TryLockVram(GX_VRAM_A | GX_VRAM_B, 17));
    CHECK(OSi_TryLockVram(GX_VRAM_A, 17) && !OSi_TryLockVram(GX_VRAM_A, 18));
    OSi_UnlockVram(GX_VRAM_A, 18); CHECK(!OSi_TryLockVram(GX_VRAM_A, 18));
    OSi_UnlockVram(GX_VRAM_A | GX_VRAM_B, 17); CHECK(OSi_TryLockVram(GX_VRAM_A, 18));
    OSi_UnlockVram(GX_VRAM_A, 18);

    u32 source[128];
    for (u32 i = 0; i < 128; i++) source[i] = 0x13579B00 + i;
    void (*paletteLoad[])(const void *, u32, u32) = {GX_LoadBGPltt, GX_LoadOBJPltt, GXS_LoadBGPltt, GXS_LoadOBJPltt};
    void (*screenLoad[])(const void *, u32, u32) = {GX_LoadBG0Scr, GX_LoadBG1Scr, GX_LoadBG2Scr, GX_LoadBG3Scr};
    void (*charLoad[])(const void *, u32, u32) = {GX_LoadBG0Char, GX_LoadBG1Char, GX_LoadBG2Char, GX_LoadBG3Char};
    void *(*screenPtr[])(void) = {G2_GetBG0ScrPtr, G2_GetBG1ScrPtr, G2_GetBG2ScrPtr, G2_GetBG3ScrPtr};
    void *(*charPtr[])(void) = {G2_GetBG0CharPtr, G2_GetBG1CharPtr, G2_GetBG2CharPtr, G2_GetBG3CharPtr};
    GX_SetBankForBG(GX_VRAM_BG_512_ABCD); GX_SetBankForOBJ(GX_VRAM_OBJ_64_E);
    reg_GX_DISPCNT = 0;
    reg_G2_BG0CNT = 0x0100; reg_G2_BG1CNT = 0x0204;
    reg_G2_BG2CNT = 0x0308; reg_G2_BG3CNT = 0x040C;
    // Exercise the original SDK's CPU shortcut and DMA path through the same loaders.
    for (unsigned pass = 0; pass < 2; pass++) {
        GXi_DmaId = pass ? 3 : (u32)-1;
        for (unsigned i = 0; i < 4; i++) {
            paletteLoad[i](source, 0, sizeof(source)); equalAt(0x05000000u + i * 0x200, source, sizeof(source));
            screenLoad[i](source, 4, 16); equalAt((uaddr)screenPtr[i]() + 4, source, 16);
            charLoad[i](source, 0x100, 128); equalAt((uaddr)charPtr[i]() + 0x100, source, 128);
        }
        GX_LoadOBJ(source, 0x200, sizeof(source)); equalAt(0x06400200, source, sizeof(source));
        GX_LoadOAM(source, 0x200, sizeof(source)); equalAt(0x07000200, source, sizeof(source));
        GXS_LoadOAM(source, 0x200, sizeof(source)); equalAt(0x07000600, source, sizeof(source));
    }
    CHECK((uaddr)G2_GetBG1CharPtr() == 0x06004000);
    G2_SetBG0Offset(-4, 9); CHECK(reg_G2_BG0OFS == (508 | (9 << 16)));
    MtxFx22 identity = {FX32_ONE, 0, 0, FX32_ONE};
    G2_SetBG2Affine(&identity, 5, 6, 10, 12);
    CHECK(reg_G2_BG2PA == 256 && reg_G2_BG2PD == 256 && reg_G2_BG2X == 10 * 256 && reg_G2_BG2Y == 12 * 256);
    G2_SetBlendAlpha(GX_BLEND_PLANEMASK_BG0, GX_BLEND_PLANEMASK_OBJ, 8, 8);
    CHECK(reg_G2_BLDCNT == 0x1041 && reg_G2_BLDALPHA == 0x0808);
    G2x_SetBlendAlpha_(0x04000050, GX_BLEND_PLANEMASK_BG0, GX_BLEND_PLANEMASK_OBJ, 3, 13);
    CHECK(reg_G2_BLDALPHA == 0x0D03);
    u32 affineBuffer[4]; G2x_SetBGyAffine_((uaddr)affineBuffer, &identity, 5, 6, 10, 12);
    CHECK(affineBuffer[0] == 256 && affineBuffer[1] == 0x01000000 && affineBuffer[2] == 10 * 256 && affineBuffer[3] == 12 * 256);
    G2S_SetBlendBrightness(GX_BLEND_PLANEMASK_BG1, -6);
    CHECK(reg_G2S_DB_BLDCNT == 0xC2 && reg_G2S_DB_BLDY == 6);

    CHECK(GX_ResetBankForBG() == GX_VRAM_BG_512_ABCD);
    equalAt(0x06804100, source, 128); // A retains BG1 tiles in its LCDC view.
    GX_SetBankForBG(GX_VRAM_BG_256_BD);
    MI_DmaCopy32(0, source, (void *)0x0601FFF0, 64);
    u32 reverse[8];
    MIi_DmaSetParams_wait(0, 0x0602000C, (uaddr)&reverse[7],
        MI_DMA_ENABLE | MI_DMA_32BIT_BUS | MI_DMA_SRC_DEC | MI_DMA_DEST_DEC | 8);
    CHECK(memcmp(reverse, source, sizeof(reverse)) == 0);
    u32 fill[8]; for (unsigned i = 0; i < 8; i++) fill[i] = 0xDEADBEEF;
    MI_DmaFill32(0, (void *)0x07000000, fill[0], sizeof(fill)); equalAt(0x07000000, fill, sizeof(fill));
    CHECK(GX_ResetBankForBG() == GX_VRAM_BG_256_BD);
    equalAt(0x0683FFF0, source, 16); equalAt(0x06860000, (u8 *)source + 16, 48);
    GX_SetBankForBG(GX_VRAM_BG_256_BD);
    u32 aliasSize = sizeof(source); u32 *alias = HGNativeMemorySpan(0x06020000, &aliasSize);
    CHECK(alias && aliasSize == sizeof(source));
    u32 first = alias[0]; MI_DmaCopy32(0, alias, (void *)0x06020004, 20);
    for (unsigned i = 0; i < 6; i++) CHECK(alias[i] == first);
    GX_ResetBankForBG();
    GX_SetBankForOBJ(GX_VRAM_OBJ_NONE);
    GX_SetBankForBG(GX_VRAM_BG_96_EFG);
    MIi_CpuCopy32(source, (u32 *)0x0600FFF0, sizeof(source));
    MI_DmaCopy32(0, source, (void *)0x06013FF0, 64);
    CHECK(GX_ResetBankForBG() == GX_VRAM_BG_96_EFG);
    equalAt(0x0688FFF0, source, 16); equalAt(0x06890000, (u8 *)source + 16, sizeof(source) - 16);
    equalAt(0x06893FF0, source, 16); equalAt(0x06894000, (u8 *)source + 16, 48);

    GX_SetBankForSubBG(GX_VRAM_SUB_BG_128_C); GX_SetBankForSubOBJ(GX_VRAM_SUB_OBJ_128_D);
    reg_GXS_DB_DISPCNT = 0;
    reg_G2S_DB_BG0CNT = 0x0100; reg_G2S_DB_BG1CNT = 0x0204;
    reg_G2S_DB_BG2CNT = 0x0308; reg_G2S_DB_BG3CNT = 0x040C;
    void (*subScreen[])(const void *, u32, u32) = {GXS_LoadBG0Scr, GXS_LoadBG1Scr, GXS_LoadBG2Scr, GXS_LoadBG3Scr};
    void (*subChar[])(const void *, u32, u32) = {GXS_LoadBG0Char, GXS_LoadBG1Char, GXS_LoadBG2Char, GXS_LoadBG3Char};
    void *(*subScreenPtr[])(void) = {G2S_GetBG0ScrPtr, G2S_GetBG1ScrPtr, G2S_GetBG2ScrPtr, G2S_GetBG3ScrPtr};
    void *(*subCharPtr[])(void) = {G2S_GetBG0CharPtr, G2S_GetBG1CharPtr, G2S_GetBG2CharPtr, G2S_GetBG3CharPtr};
    for (unsigned i = 0; i < 4; i++) {
        subScreen[i](source, 4, 16); equalAt((uaddr)subScreenPtr[i]() + 4, source, 16);
        subChar[i](source, 0x100, 128); equalAt((uaddr)subCharPtr[i]() + 0x100, source, 128);
    }
    GXS_LoadOBJ(source, 0x200, sizeof(source)); equalAt(0x06600200, source, sizeof(source));
    reg_GXS_DB_DISPCNT = 5; reg_G2S_DB_BG2CNT = 0x0180;
    CHECK(G2S_GetBG2CharPtr() == NULL && (uaddr)G2S_GetBG2ScrPtr() == 0x06204000);
    GX_SetBankForSubBG(GX_VRAM_SUB_BG_NONE); GX_SetBankForSubOBJ(GX_VRAM_SUB_OBJ_NONE);
    GX_SetBankForSubBG(GX_VRAM_SUB_BG_48_HI);
    MI_DmaCopy32(0, source, (void *)0x06207FF0, 64);
    CHECK(GX_ResetBankForSubBG() == GX_VRAM_SUB_BG_48_HI);
    equalAt(0x0689FFF0, source, 16); equalAt(0x068A0000, (u8 *)source + 16, 48);

    const GXVRamBGExtPltt banks[] = {GX_VRAM_BGEXTPLTT_0123_E, GX_VRAM_BGEXTPLTT_0123_FG,
        GX_VRAM_BGEXTPLTT_01_F, GX_VRAM_BGEXTPLTT_23_G};
    for (unsigned i = 0; i < 4; i++) {
        GX_SetBankForBGExtPltt(banks[i]); GX_BeginLoadBGExtPltt();
        u32 offset = i == 2 ? 0 : 0x4000;
        GX_LoadBGExtPltt(source, offset, sizeof(source)); GX_EndLoadBGExtPltt();
        CHECK(reg_GX_DISPCNT & REG_GX_DISPCNT_BG_MASK);
        GX_ResetBankForBGExtPltt();
        equalAt(i == 0 ? 0x06884000 : (i == 2 ? 0x06890000 : 0x06894000), source, sizeof(source));
    }
    GX_SetBankForOBJExtPltt(GX_VRAM_OBJEXTPLTT_0_F); GX_BeginLoadOBJExtPltt();
    GX_LoadOBJExtPltt(source, 0, sizeof(source)); GX_EndLoadOBJExtPltt();
    GX_ResetBankForOBJExtPltt(); equalAt(0x06890000, source, sizeof(source));
    GXS_BeginLoadBGExtPltt(); // No bank is silently synthesized by the native layer.
    GXS_EndLoadBGExtPltt();
    GX_SetBankForSubBGExtPltt(GX_VRAM_SUB_BGEXTPLTT_0123_H); GXS_BeginLoadBGExtPltt();
    GXS_LoadBGExtPltt(source, 0x100, sizeof(source)); GXS_EndLoadBGExtPltt();
    GX_ResetBankForSubBGExtPltt(); equalAt(0x06898100, source, sizeof(source));
    GX_SetBankForSubOBJExtPltt(GX_VRAM_SUB_OBJEXTPLTT_0_I); GXS_BeginLoadOBJExtPltt();
    GXS_LoadOBJExtPltt(source, 0, sizeof(source)); GXS_EndLoadOBJExtPltt();
    GX_ResetBankForSubOBJExtPltt(); equalAt(0x068A0000, source, sizeof(source));

    u32 size = 16;
    CHECK(HGNativeMemorySpan(0x050007FC, &size) && size == 4);
    size = 16; CHECK(HGNativeMemorySpan(0x04000400, &size) == NULL);
    CHECK(HGNativeMemorySpan(UINTPTR_MAX - 8, &size) == NULL);
    GX_SetBankForBG(GX_VRAM_BG_256_AB);
    MI_DmaFill32(0,(void *)0x06000000,0x01010101,4); MI_DmaFill32(0,(void *)0x06020000,0x02020202,4);
    reg_GX_VRAMCNT_B = 0x81;
    CHECK(HGNativeMemorySpan(0x06000000, &size) == NULL);
    u32 combined = 0; HGNativeSnapshotGraphics(0x06000000,&combined,4); CHECK(combined == 0x03030303);
    reg_GX_VRAMCNT_B = 0x89; GX_DisableBankForBG();
    CHECK(HGNativeMemorySpan(0x06000000, &size) == NULL);
    GX_InitGXState(); GXi_DmaId = 3;
    printf("PASS: original GX 2D loaders, palettes/OAM, bank aliases and boundaries, extended palettes, affine/blend state and VRAM locks.\n");
}
