// SPDX-License-Identifier: GPL-3.0-or-later
#include <nitro.h>
#include <nitro/gx/native_geometry.h>
#include <nitro/hw/native_graphics.h>
#include <nnsys/g2d/g2d_Softsprite.h>
#include <nnsys/g2d/g2d_Sprite.h>
#include <string.h>
#define CHECK(v) do { if (!(v)) return __LINE__; } while (0)
void GX_InitGXState(void);
void GX_BeginLoadTex(void); void GX_LoadTex(void *,u32,u32); void GX_EndLoadTex(void);
extern u32 GXi_DmaId;
static _Alignas(4) u16 texture[64];
static struct { u32 before; u8 pixels[256*192*4]; u32 after; } image;
static BOOL pixel(u32 x,u32 y,u16 color) {
    const u8 *p=image.pixels+(y*256+x)*4;
    return p[0]>>3==(color&31) && p[1]>>3==((color>>5)&31) && p[2]>>3==((color>>10)&31) && p[3]==255;
}
static BOOL draw(s16 x,s16 y,int width,int height,int u0,int v0,int u1,int v1) {
    HGNativeGeometryReset(); reg_G3X_DISP3DCNT=1;
    G3X_SetClearColor(0,0,32767,0,FALSE); NNS_G2dSetupSoftwareSpriteCamera();
    G3_MaterialColorDiffAmb(0x7FFF,0,TRUE);
    G3_PolygonAttr(0,GX_POLYGONMODE_MODULATE,GX_CULL_NONE,1,31,0);
    G3_TexImageParam(GX_TEXFMT_DIRECT,GX_TEXGEN_TEXCOORD,GX_TEXSIZE_S8,GX_TEXSIZE_T8,GX_TEXREPEAT_NONE,GX_TEXFLIP_NONE,GX_TEXPLTTCOLOR0_USE,0);
    NNS_G2dDrawSpriteFast(x,y,0,width,height,u0,v0,u1,v1);
    G3_SwapBuffers(GX_SORTMODE_AUTO,GX_BUFFERMODE_Z);
    return HGNativeRender3D(1,image.pixels,sizeof(image.pixels));
}
u32 CheckOriginalSoftsprite(void) {
    u32 savedDma=GXi_DmaId;
    image.before=0x12345678; image.after=0x87654321;
    GX_InitGXState(); GXi_DmaId=(u32)-1; GX_SetBankForTex(GX_VRAM_TEX_0_A);
    const u16 colors[]={31,0x3E0,0x7C00,0x7FFF};
    for (u32 y=0;y<8;y++) for (u32 x=0;x<8;x++) texture[y*8+x]=0x8000|colors[(y>=4)*2+(x>=4)];
    GX_BeginLoadTex(); GX_LoadTex(texture,0,sizeof(texture)); GX_EndLoadTex();
    CHECK(draw(32,24,64,32,0,0,8,8));
    CHECK(pixel(40,28,31) && pixel(88,28,0x3E0) && pixel(40,52,0x7C00) && pixel(88,52,0x7FFF));
    u32 count=0; for (u32 i=0;i<256*192;i++) count+=image.pixels[i*4+3]!=0;
    CHECK(count==64*32 && !image.pixels[(24*256+31)*4+3] && !image.pixels[(56*256+32)*4+3]);
    HGNativeGeometryFrameInfo frame; HGNativeGeometryPolygon polygon; HGNativeGeometryVertex vertex;
    CHECK(HGNativeGeometryFrame(1,&frame) && frame.vertices==4 && frame.polygons==1);
    CHECK(HGNativeGeometryReadPolygon(1,0,&polygon) && polygon.count==4);
    const s16 uv[4][2]={{0,128},{128,128},{128,0},{0,0}};
    for (u32 i=0;i<4;i++) {
        CHECK(HGNativeGeometryReadVertex(1,i,&vertex) && !memcmp(vertex.uv,uv[i],sizeof(uv[i])));
        CHECK(vertex.clip[3]==1024*4096);
    }
    CHECK(draw(32,24,64,32,8,8,0,0));
    CHECK(pixel(40,28,0x7FFF) && pixel(88,28,0x7C00) && pixel(40,52,0x3E0) && pixel(88,52,31));
    CHECK(draw(32,24,64,32,0,0,4,8));
    CHECK(pixel(40,28,31) && pixel(88,28,31) && pixel(40,52,0x7C00) && pixel(88,52,0x7C00));
    CHECK(draw(-8,-8,16,16,0,0,8,8) && pixel(4,4,0x7FFF));
    count=0; for (u32 i=0;i<256*192;i++) count+=image.pixels[i*4+3]!=0; CHECK(count==64);
    CHECK(draw(32,24,64,32,-8,-8,0,0) && pixel(40,28,31));
    CHECK(HGNativeGeometryReadVertex(1,0,&vertex) && vertex.uv[0]==-128 && vertex.uv[1]==0);
    CHECK(draw(32,24,0,32,0,0,8,8));
    for (u32 i=0;i<256*192;i++) CHECK(!image.pixels[i*4+3]);
    // The camera stores its projection in slot zero before switching to position mode.
    HGNativeGeometryReset(); NNS_G2dSetupSoftwareSpriteCamera(); MtxFx44 projection,restored;
    CHECK(HGNativeGeometryMatrix(0,&projection));
    G3_MtxMode(GX_MTXMODE_PROJECTION); G3_Identity(); u32 index=0;
    CHECK(HGNativeGeometryCommand(G3OP_MTX_RESTORE,&index,1));
    CHECK(HGNativeGeometryMatrix(0,&restored) && !memcmp(&projection,&restored,sizeof(projection)));
    CHECK(image.before==0x12345678 && image.after==0x87654321);
    GXi_DmaId=savedDma; HGNativeGeometryReset(); return 0;
}
