// SPDX-License-Identifier: GPL-3.0-or-later
#include <nitro.h>
#include <nitro/gx/g3b.h>
#include <nitro/gx/native_geometry.h>
#include <nitro/gx/native_texture.h>
#include <nitro/hw/native_graphics.h>
#include <string.h>
#define CHECK(v) do { if (!(v)) return __LINE__; } while (0)
void GX_InitGXState(void);
void GX_BeginLoadTex(void); void GX_LoadTex(void *,u32,u32); void GX_EndLoadTex(void);
void GX_BeginLoadTexPltt(void); void GX_LoadTexPltt(void *,u32,u32); void GX_EndLoadTexPltt(void);
extern u32 GXi_DmaId;
static _Alignas(4) u8 data[256],pixels[256*192*4];
static _Alignas(4) u16 palette[256];
static _Alignas(4) u32 displayList[64];
static void upload(void *bytes,u32 offset,u32 size) { GX_BeginLoadTex(); GX_LoadTex(bytes,offset,size); GX_EndLoadTex(); }
static void uploadPalette(void *bytes,u32 offset,u32 size) { GX_BeginLoadTexPltt(); GX_LoadTexPltt(bytes,offset,size); GX_EndLoadTexPltt(); }
#ifndef HEARTGOLD_ABI32
#include "../NativeEvents.h"
void GX_BeginLoadClearImage(void); void GX_EndLoadClearImage(void);
void GX_BeginLoadBGExtPltt(void); void GX_EndLoadBGExtPltt(void);
void GX_BeginLoadOBJExtPltt(void); void GX_EndLoadOBJExtPltt(void);
void GXS_BeginLoadBGExtPltt(void); void GXS_EndLoadBGExtPltt(void);
void GXS_BeginLoadOBJExtPltt(void); void GXS_EndLoadOBJExtPltt(void);
static u32 interruptUploads;
static _Alignas(4) u8 interruptTexture[64];
static void uploadOnVBlank(void) { interruptUploads++; upload(interruptTexture,64,sizeof(interruptTexture)); }
static u32 checkUploadInterrupts(void) {
    void (*const begins[])(void)={GX_BeginLoadTex,GX_BeginLoadTexPltt,GX_BeginLoadClearImage,GX_BeginLoadBGExtPltt,GX_BeginLoadOBJExtPltt,GXS_BeginLoadBGExtPltt,GXS_BeginLoadOBJExtPltt};
    void (*const ends[])(void)={GX_EndLoadTex,GX_EndLoadTexPltt,GX_EndLoadClearImage,GX_EndLoadBGExtPltt,GX_EndLoadOBJExtPltt,GXS_EndLoadBGExtPltt,GXS_EndLoadOBJExtPltt};
    OS_InitIrqTable(); OS_SetIrqFunction(OS_IE_VBLANK,uploadOnVBlank); OS_EnableIrqMask(OS_IE_VBLANK);
    OS_EnableInterrupts(); OS_EnableIrq(); GXi_DmaId=3;
    GX_SetBankForTex(GX_VRAM_TEX_0_A); memset(data,0x11,64); memset(interruptTexture,0x22,64);
    for (u32 i=0;i<sizeof(begins)/sizeof(begins[0]);i++) {
        u32 before=interruptUploads; begins[i]();
        CHECK(OS_GetCpsrIrq()==OS_INTRMODE_IRQ_DISABLE);
        HGNativeSignalVBlank(); CHECK(interruptUploads==before);
        if (!i) GX_LoadTex(data,0,64);
        ends[i](); CHECK(interruptUploads==before+1 && OS_GetCpsrIrq()==OS_INTRMODE_IRQ_ENABLE);
        OSIntrMode previous=OS_DisableInterrupts(); begins[i](); ends[i]();
        CHECK(OS_GetCpsrIrq()==OS_INTRMODE_IRQ_DISABLE); OS_RestoreInterrupts(previous);
    }
    u8 actual[128]; CHECK(HGNativeReadTextureMemory(FALSE,0,actual,sizeof(actual)));
    for (u32 i=0;i<sizeof(actual);i++) CHECK(actual[i]==(i<64 ? 0x11 : 0x22));
    OS_DisableIrq(); OS_SetIrqFunction(OS_IE_VBLANK,NULL); GXi_DmaId=(u32)-1;
    return 0;
}
#endif
static BOOL sample(u32 texture,u32 base,s32 s,s32 t,u16 color,u16 alpha) {
    u16 actual=0x1234,opacity=0x5678;
    return HGNativeSampleTexture(texture,base,s,t,&actual,&opacity) && actual==color && opacity==alpha;
}
u32 CheckOriginalTextures(void) {
    GX_InitGXState(); GXi_DmaId=(u32)-1;
#ifndef HEARTGOLD_ABI32
    u32 line=checkUploadInterrupts(); if (line) return line;
#endif
    GX_SetBankForTex(GX_VRAM_TEX_01_AB); GX_SetBankForTexPltt(GX_VRAM_TEXPLTT_0123_E);
    palette[0]=31; palette[1]=0x3E0; palette[2]=0x7C00; palette[3]=0x7FFF;
    uploadPalette(palette,0,sizeof(palette));
    memset(data,0,sizeof(data)); data[0]=0xE4; upload(data,0,64);
    CHECK(sample(2u<<26,0,0,0,31,31) && sample(2u<<26,0,16,0,0x3E0,31));
    CHECK(sample(2u<<26,0,32,0,0x7C00,31) && sample(2u<<26,0,48,0,0x7FFF,31));
    CHECK(sample((2u<<26)|(1u<<29),0,0,0,31,0));
    data[0]=0x21; upload(data,0,64);
    CHECK(sample(3u<<26,0,0,0,0x3E0,31) && sample(3u<<26,0,16,0,0x7C00,31));
    for (u32 i=0;i<8;i++) data[i]=(u8)((i<<5)|1);
    upload(data,0,64);
    for (u32 i=0;i<8;i++) CHECK(sample(1u<<26,0,(s32)i*16,0,0x3E0,(u16)((i<<2)+(i>>1))));
    for (u32 i=0;i<32;i++) data[i]=(u8)((i<<3)|1);
    upload(data,0,64);
    for (u32 i=0;i<32;i++) CHECK(sample(6u<<26,0,(s32)(i%8)*16,(s32)(i/8)*16,0x3E0,(u16)i));
    const u16 direct[]={0x801F,0x03E0,0xFC00,0xFFFF};
    memset(data,0,sizeof(data)); memcpy(data,direct,sizeof(direct)); upload(data,0,128);
    CHECK(sample(7u<<26,0,0,0,31,31) && sample(7u<<26,0,16,0,0x3E0,0));
    CHECK(sample(7u<<26,0,32,0,0x7C00,31));
    for (u32 i=0;i<64;i++) { data[i]=(u8)i; palette[i]=(u16)i; }
    upload(data,0,64); uploadPalette(palette,0,sizeof(palette));
    CHECK(sample(4u<<26,0,-1,0,0,31) && sample(4u<<26,0,128,0,7,31));
    CHECK(sample((4u<<26)|(1u<<16),0,-1,0,7,31));
    CHECK(sample((4u<<26)|(1u<<16),0,128,0,0,31));
    CHECK(sample((4u<<26)|(1u<<16)|(1u<<18),0,128,0,7,31));
    CHECK(sample((4u<<26)|(1u<<16)|(1u<<18),0,-1,0,0,31));
    CHECK(sample((4u<<26)|(1u<<17),0,0,-1,56,31));
    CHECK(sample((4u<<26)|(1u<<17)|(1u<<19),0,0,128,56,31));

    palette[0]=31; palette[1]=0x3E0; palette[2]=0x7C00; palette[3]=0x7FFF;
    uploadPalette(palette,0,sizeof(palette)); memset(data,0xE4,16); upload(data,0,16);
    for (u32 mode=0;mode<4;mode++) {
        u32 info=mode<<14; upload(&info,0x20000,4);
        CHECK(sample(5u<<26,0,0,0,31,31) && sample(5u<<26,0,16,0,0x3E0,31));
        const u16 third[]={0x7C00,0x1EF,0x7C00,0x173};
        const u16 fourth[]={0,0,0x7FFF,0x26B};
        CHECK(sample(5u<<26,0,32,0,third[mode],31));
        CHECK(sample(5u<<26,0,48,0,fourth[mode],mode<2 ? 0 : 31));
    }
    GX_SetBankForTex(GX_VRAM_TEX_012_ABC);
    upload(data,0x40000,16); u32 descriptor=3u<<14; upload(&descriptor,0x30000,4);
    CHECK(sample((5u<<26)|0x8000,0,32,0,0x173,31));
    CHECK(sample((5u<<26)|0x4000,0,48,0,31,31));

    // The original CPU/DMA upload paths both restore GPU bank mappings before sampling.
    for (u32 pass=0;pass<2;pass++) {
        GXi_DmaId=pass ? 3 : (u32)-1;
        GX_SetBankForTex(GX_VRAM_TEX_01_AC);
        memset(data,1,64); upload(data,0x1FFE0,64);
        u8 cross[64]; CHECK(HGNativeReadTextureMemory(FALSE,0x1FFE0,cross,64));
        for (u32 i=0;i<64;i++) CHECK(cross[i]==1);
    }
    GXi_DmaId=(u32)-1; GX_SetBankForTex(GX_VRAM_TEX_01_AB);
    GX_BeginLoadTex(); MI_CpuFill8((void *)0x06800000,1,4); MI_CpuFill8((void *)0x06820000,2,4); GX_EndLoadTex();
    reg_GX_VRAMCNT_B=0x83;
    u8 overlap[4]; CHECK(HGNativeReadTextureMemory(FALSE,0,overlap,4));
    for (u32 i=0;i<4;i++) CHECK(overlap[i]==3);
    GX_SetBankForTex(GX_VRAM_TEX_NONE); memset(overlap,0xA5,4);
    CHECK(HGNativeReadTextureMemory(FALSE,0,overlap,4));
    for (u32 i=0;i<4;i++) CHECK(!overlap[i]);
    CHECK(!HGNativeReadTextureMemory(FALSE,0x7FFFF,overlap,2));
    CHECK(!HGNativeReadTextureMemory(TRUE,0x1FFFF,overlap,2));
    CHECK(!HGNativeSampleTexture(0,0,0,0,NULL,&palette[0]));

    GX_SetBankForTex(GX_VRAM_TEX_0_A); memset(data,0x11,64); upload(data,0,64);
    palette[1]=0x3E0; uploadPalette(palette,0,sizeof(palette));
    HGNativeGeometryReset(); G3X_SetClearColor(0,0,32767,0,FALSE);
    u32 texture=3u<<26; CHECK(HGNativeGeometryCommand(G3OP_TEXIMAGE_PARAM,&texture,1));
    GXDLInfo list; G3_BeginMakeDL(&list,displayList,sizeof(displayList));
    G3B_PolygonAttr(&list,0,GX_POLYGONMODE_MODULATE,GX_CULL_NONE,0,31,0);
    G3B_Color(&list,0x7FFF); G3B_Begin(&list,GX_BEGIN_TRIANGLES);
    G3B_Vtx(&list,-2048,-2048,0); G3B_Vtx(&list,2048,-2048,0); G3B_Vtx(&list,0,2048,0); G3B_End(&list);
    CHECK(HGNativeGeometryFifo(displayList,G3_EndMakeDL(&list)/4)); G3_SwapBuffers(0,0);
    CHECK(HGNativeRender3D(1,pixels,sizeof(pixels)));
    u32 center=(96*256+128)*4; CHECK(!pixels[center] && pixels[center+1]==255 && !pixels[center+2] && pixels[center+3]==255);
    memset(data,0,64); upload(data,0,64); texture|=1u<<29;
    HGNativeGeometryReset(); CHECK(HGNativeGeometryCommand(G3OP_TEXIMAGE_PARAM,&texture,1));
    CHECK(HGNativeGeometryFifo(displayList,G3_EndMakeDL(&list)/4)); G3_SwapBuffers(0,0);
    CHECK(HGNativeRender3D(1,pixels,sizeof(pixels)) && !pixels[center+3]);
    memset(data,0x61,64); upload(data,0,64); texture=1u<<26;
    HGNativeGeometryReset(); CHECK(HGNativeGeometryCommand(G3OP_TEXIMAGE_PARAM,&texture,1));
    CHECK(HGNativeGeometryFifo(displayList,G3_EndMakeDL(&list)/4)); G3_SwapBuffers(0,0);
    CHECK(HGNativeRender3D(1,pixels,sizeof(pixels)));
    CHECK(!pixels[center] && pixels[center+1]==255 && !pixels[center+2] && pixels[center+3]==107);
    displayList[1]=(displayList[1]&~0x1F0000u)|(15u<<16);
    HGNativeGeometryReset(); CHECK(HGNativeGeometryCommand(G3OP_TEXIMAGE_PARAM,&texture,1));
    CHECK(HGNativeGeometryFifo(displayList,G3_EndMakeDL(&list)/4)); G3_SwapBuffers(0,0);
    CHECK(HGNativeRender3D(1,pixels,sizeof(pixels)) && pixels[center+1]==255 && pixels[center+3]==49);
    displayList[1]=(displayList[1]&~0x1F0030u)|(31u<<16)|0x10;
    HGNativeGeometryReset(); CHECK(HGNativeGeometryCommand(G3OP_TEXIMAGE_PARAM,&texture,1));
    CHECK(HGNativeGeometryFifo(displayList,G3_EndMakeDL(&list)/4)); G3_SwapBuffers(0,0);
    CHECK(HGNativeRender3D(1,pixels,sizeof(pixels)));
    CHECK(pixels[center]==140 && pixels[center+1]==247 && pixels[center+2]==140 && pixels[center+3]==255);
    HGNativeGeometryReset(); GX_SetBankForTex(GX_VRAM_TEX_NONE); return 0;
}
