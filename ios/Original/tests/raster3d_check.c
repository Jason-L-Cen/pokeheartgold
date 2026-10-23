// SPDX-License-Identifier: GPL-3.0-or-later
#include <nitro.h>
#include <nitro/gx/g3b.h>
#include <nitro/gx/native_geometry.h>
#include <nitro/hw/native_graphics.h>
#include <string.h>
#include <stdio.h>
// Keep the original static weather fade helpers in this QA TU; unused scene machinery is GC-pruned.
#include "../../../src/field/weather_manager.c"
#include "../../../src/field/field_fog.c"
#define CHECK(v) do { if (!(v)) return __LINE__; } while (0)
void G3_LoadMtx44(const MtxFx44 *);
static struct { u32 before; u8 pixels[256*192*4]; u32 after; } image;
static struct { u32 before; u8 pixels[512*192*4]; u32 after; } wideImage;
static u32 checkWideView(void);
static void setFog(u32 enabled,u32 mode,u32 shift,u32 offset,u16 color,u32 alpha,const u8 table[32]);
u32 CheckOriginalFog3D(void);
static _Alignas(4) u32 commands[128];
static BOOL triangle(u16 color,s16 z0,s16 z1,s16 z2,u32 misc,u32 cull) {
    GXDLInfo list; G3_BeginMakeDL(&list,commands,sizeof(commands));
    G3B_PolygonAttr(&list,0,GX_POLYGONMODE_MODULATE,(GXCull)cull,0,31,(int)misc);
    G3B_Color(&list,color); G3B_Begin(&list,GX_BEGIN_TRIANGLES);
    G3B_Vtx(&list,-2048,-2048,z0); G3B_Vtx(&list,2048,-2048,z1); G3B_Vtx(&list,0,2048,z2); G3B_End(&list);
    u32 bytes=G3_EndMakeDL(&list); return HGNativeGeometryFifo(commands,bytes/4);
}
static BOOL pixel(u32 x,u32 y,u8 r,u8 g,u8 b,u8 a) {
    const u8 *p=image.pixels+(y*256+x)*4;
    return p[0]==r && p[1]==g && p[2]==b && p[3]==a;
}
static void reset(void) {
    HGNativeGeometryReset(); G3X_SetClearColor(0,0,32767,0,FALSE); G3_ViewPort(0,0,255,191);
    reg_G3X_DISP3DCNT&=~0x80;
}
u32 CheckOriginalRaster3D(void) {
    image.before=0x12345678; image.after=0x87654321;
    reset(); CHECK(triangle(31,0,0,0,0,GX_CULL_NONE));
    G3_SwapBuffers(GX_SORTMODE_AUTO,GX_BUFFERMODE_Z);
    CHECK(HGNativeRender3D(1,image.pixels,sizeof(image.pixels)));
    CHECK(pixel(128,96,255,0,0,255) && pixel(0,0,0,0,0,0));
    u32 count=0; for (u32 i=0;i<256*192;i++) count+=image.pixels[i*4+3]!=0;
    CHECK(count==6144 && image.before==0x12345678 && image.after==0x87654321);
    for (u32 small=0;small<2;small++) {
        reset(); G3_ViewPort(0,small ? 94 : 80,255,small ? 96 : 95);
        CHECK(triangle(31,0,0,0,0,GX_CULL_NONE)); G3_SwapBuffers(0,0);
        CHECK(HGNativeRender3D(1,image.pixels,sizeof(image.pixels)));
        count=0; for (u32 i=0;i<256*192;i++) count+=image.pixels[i*4+3]!=0;
        CHECK(count==(small ? 64 : 512));
        u32 top=small ? 96 : 100,bottom=small ? 97 : 108;
        CHECK(pixel(128,top,255,0,0,255) && pixel(128,bottom-1,255,0,0,255));
        CHECK(pixel(128,top-1,0,0,0,0) && pixel(128,bottom,0,0,0,0));
    }
    // Shared edges in opposite directions, including a shorter neighbor ending partway along an edge.
    for (u32 variant=0;variant<4;variant++) {
        reset(); G3_ViewPort(0,0,255,127);
        const s16 vertices[2][8][2]={
            {{-4096,3456},{-3776,3456},{-2336,3136},{-4096,3136},
                {-3776,3456},{-2048,3456},{-2048,3136},{-2336,3136}},
            {{-768,3456},{1190,3456},{1278,640},{-768,640},
                {1190,3456},{1888,3456},{2080,-2496},{1376,-2496}}};
        GXDLInfo adjacent; G3_BeginMakeDL(&adjacent,commands,sizeof(commands));
        G3B_PolygonAttr(&adjacent,0,GX_POLYGONMODE_MODULATE,GX_CULL_NONE,0,31,0);
        G3B_Color(&adjacent,31); G3B_Begin(&adjacent,GX_BEGIN_QUADS);
        for (u32 polygon=0;polygon<2;polygon++) for (u32 v=0;v<4;v++) {
            u32 index=polygon*4+((variant&1) ? 3-v : v);
            G3B_Vtx(&adjacent,vertices[variant/2][index][0],vertices[variant/2][index][1],0);
        }
        G3B_End(&adjacent); CHECK(HGNativeGeometryFifo(commands,G3_EndMakeDL(&adjacent)/4));
        G3_SwapBuffers(0,0); CHECK(HGNativeRender3D(1,image.pixels,sizeof(image.pixels)));
        for (u32 y=74;y<(variant<2 ? 79 : 118);y++)
            for (u32 x=variant<2 ? 0 : 104;x<(variant<2 ? 64 : 187);x++) CHECK(pixel(x,y,255,0,0,255));
        if (variant<2) CHECK(pixel(23,73,0,0,0,0) && pixel(23,79,0,0,0,0) && pixel(64,75,0,0,0,0));
        else CHECK(pixel(104,73,0,0,0,0) && pixel(103,83,0,0,0,0));
    }
    reset(); CHECK(triangle(31,0,0,0,0,GX_CULL_NONE)); G3_SwapBuffers(0,0);
    G3X_SetClearColor(0x7C00,31,32767,0,FALSE);
    CHECK(HGNativeRender3D(1,image.pixels,sizeof(image.pixels)) && pixel(0,0,0,0,0,0));
    CHECK(HGNativeRender3D(0,image.pixels,sizeof(image.pixels)) && pixel(0,0,0,0,255,255));

    reset(); CHECK(triangle(31,1024,1024,1024,0,GX_CULL_NONE));
    CHECK(triangle(0x3E0,-1024,-1024,-1024,0,GX_CULL_NONE));
    CHECK(triangle(0x7C00,2048,2048,2048,0,GX_CULL_NONE));
    G3_SwapBuffers(GX_SORTMODE_AUTO,GX_BUFFERMODE_Z);
    CHECK(HGNativeRender3D(1,image.pixels,sizeof(image.pixels)) && pixel(128,96,0,255,0,255));
    reset(); CHECK(triangle(31,0,0,0,0,GX_CULL_FRONT)); G3_SwapBuffers(0,0);
    CHECK(HGNativeRender3D(1,image.pixels,sizeof(image.pixels)) && pixel(128,96,0,0,0,0));
    reset(); CHECK(triangle(31,0,0,0,0,GX_CULL_BACK)); G3_SwapBuffers(0,0);
    CHECK(HGNativeRender3D(1,image.pixels,sizeof(image.pixels)) && pixel(128,96,255,0,0,255));

    reset(); CHECK(triangle(31,0,0,8192,0,GX_CULL_NONE)); G3_SwapBuffers(0,0);
    CHECK(HGNativeRender3D(1,image.pixels,sizeof(image.pixels)) && pixel(128,112,0,0,0,0));
    reset(); CHECK(triangle(31,0,0,8192,0x1000,GX_CULL_NONE)); G3_SwapBuffers(0,0);
    CHECK(HGNativeRender3D(1,image.pixels,sizeof(image.pixels)) && pixel(128,112,255,0,0,255));
    CHECK(pixel(128,80,0,0,0,0));
    reset(); CHECK(triangle(31,0,0,-8192,0,GX_CULL_NONE)); G3_SwapBuffers(0,0);
    CHECK(HGNativeRender3D(1,image.pixels,sizeof(image.pixels)) && pixel(128,112,255,0,0,255));
    CHECK(pixel(128,80,0,0,0,0));
    reset(); G3_ViewPort(32,16,223,175); CHECK(triangle(31,0,0,0,0,GX_CULL_NONE)); G3_SwapBuffers(0,0);
    CHECK(HGNativeRender3D(1,image.pixels,sizeof(image.pixels)) && pixel(128,96,255,0,0,255));
    CHECK(pixel(70,140,0,0,0,0));

    reset(); GXDLInfo list; G3_BeginMakeDL(&list,commands,sizeof(commands));
    G3B_PolygonAttr(&list,0,GX_POLYGONMODE_MODULATE,GX_CULL_NONE,0,31,0); G3B_Begin(&list,GX_BEGIN_TRIANGLES);
    G3B_Color(&list,31); G3B_Vtx(&list,-2048,-2048,0);
    G3B_Color(&list,0x3E0); G3B_Vtx(&list,2048,-2048,0);
    G3B_Color(&list,0x7C00); G3B_Vtx(&list,0,2048,0); G3B_End(&list);
    CHECK(HGNativeGeometryFifo(commands,G3_EndMakeDL(&list)/4)); G3_SwapBuffers(0,0);
    CHECK(HGNativeRender3D(1,image.pixels,sizeof(image.pixels)));
    const u8 *center=image.pixels+(96*256+128)*4;
    CHECK(center[0]>0 && center[1]>0 && center[2]>center[0] && center[2]>center[1] && center[3]==255);
    reset();
    MtxFx44 perspective={.a={4096,0,0,0,0,4096,0,0,0,0,0,-4096,0,0,0,0}};
    G3_MtxMode(GX_MTXMODE_PROJECTION); G3_LoadMtx44(&perspective);
    G3_BeginMakeDL(&list,commands,sizeof(commands));
    G3B_PolygonAttr(&list,0,GX_POLYGONMODE_MODULATE,GX_CULL_NONE,0,31,0); G3B_Begin(&list,GX_BEGIN_TRIANGLES);
    G3B_Color(&list,31); G3B_Vtx(&list,-2048,-2048,-4096);
    G3B_Color(&list,0x3E0); G3B_Vtx(&list,2048,-2048,-4096);
    G3B_Color(&list,0x7C00); G3B_Vtx(&list,0,2048,-8192); G3B_End(&list);
    CHECK(HGNativeGeometryFifo(commands,G3_EndMakeDL(&list)/4)); G3_SwapBuffers(0,0);
    CHECK(HGNativeRender3D(1,image.pixels,sizeof(image.pixels)) && pixel(128,108,82,90,82,255));
    reset();
    MtxFx44 enlarged={.a={16384,0,0,0,0,16384,0,0,0,0,4096,0,0,0,0,4096}};
    G3_MtxMode(GX_MTXMODE_PROJECTION); G3_LoadMtx44(&enlarged);
    CHECK(triangle(31,0,0,0,0,GX_CULL_NONE)); G3_SwapBuffers(0,0);
    CHECK(HGNativeRender3D(1,image.pixels,sizeof(image.pixels)) && pixel(128,96,255,0,0,255));
    reset();
    memset(&enlarged,0,sizeof(enlarged)); G3_MtxMode(GX_MTXMODE_PROJECTION); G3_LoadMtx44(&enlarged);
    CHECK(triangle(31,0,0,0,0,GX_CULL_NONE)); G3_SwapBuffers(0,0);
    CHECK(HGNativeRender3D(1,image.pixels,sizeof(image.pixels)) && pixel(128,96,0,0,0,0));

    reset(); CHECK(triangle(31,0,0,0,0,GX_CULL_NONE)); G3_SwapBuffers(0,0);
    // Disabled fog ignores per-polygon and rear-plane fog flags.
    reset(); G3X_SetClearColor(0,0,32767,0,TRUE);
    CHECK(triangle(31,0,0,0,0x8000,GX_CULL_NONE)); G3_SwapBuffers(0,0);
    CHECK(HGNativeRender3D(1,image.pixels,sizeof(image.pixels)) && pixel(128,96,255,0,0,255));
    u32 fogLine=CheckOriginalFog3D(); if (fogLine) return fogLine;
    CHECK(!HGNativeRender3D(1,image.pixels,sizeof(image.pixels)-1) && !HGNativeRender3D(1,NULL,sizeof(image.pixels)));
    reset(); CHECK(triangle(31,0,0,0,0,GX_CULL_NONE)); G3_SwapBuffers(0,GX_BUFFERMODE_W);
    CHECK(HGNativeRender3D(1,image.pixels,sizeof(image.pixels)) && pixel(128,96,255,0,0,255));
    for (u32 wbuffer=0;wbuffer<2;wbuffer++) {
        reset(); G3_MtxMode(GX_MTXMODE_PROJECTION); G3_LoadMtx44(&perspective);
        CHECK(triangle(31,-8192,-8192,-8192,0,GX_CULL_NONE));
        CHECK(triangle(0x3E0,-4096,-4096,-4096,0,GX_CULL_NONE));
        G3_SwapBuffers(0,(GXBufferMode)wbuffer);
        CHECK(HGNativeRender3D(1,image.pixels,sizeof(image.pixels)));
        CHECK(wbuffer ? pixel(128,96,0,255,0,255) : pixel(128,96,255,0,0,255));
    }
    CHECK(image.before==0x12345678 && image.after==0x87654321);
    reset();
#ifndef HEARTGOLD_ABI32
    CHECK(triangle(31,0,0,0,0,GX_CULL_NONE)); G3_SwapBuffers(0,0);
    const u16 backdrop=0x3E0; GX_LoadBGPltt(&backdrop,0,2);
    reg_GX_DISPCNT=0x10108; reg_G2_BG0CNT=0; reg_G2_BG0OFS=0; reg_G2_BLDCNT=0; reg_GX_MASTER_BRIGHT=0;
    CHECK(HGNativeRender2D(0,image.pixels,sizeof(image.pixels)));
    CHECK(pixel(128,96,255,0,0,255) && pixel(0,0,0,255,0,255));
    reg_G2_BG0OFS=32;
    CHECK(HGNativeRender2D(0,image.pixels,sizeof(image.pixels)) && pixel(96,96,255,0,0,255));
#endif
    reset(); return checkWideView();
}

static u32 checkWideView(void) {
    CHECK(HGNativeSetViewWidth(256) && HGNativeViewWidth()==256);
    CHECK(!HGNativeSetViewWidth(0) && !HGNativeSetViewWidth(254) && !HGNativeSetViewWidth(257) && !HGNativeSetViewWidth(514));
    CHECK(HGNativeViewWidth()==256 && HGNativeSetViewWidth(384));
    reset(); CHECK(triangle(31,0,0,0,0,GX_CULL_NONE)); G3_SwapBuffers(0,0);
    CHECK(HGNativeRender3D(1,image.pixels,sizeof(image.pixels)));
    wideImage.before=0x12345678; wideImage.after=0x87654321; memset(wideImage.pixels,0xA5,sizeof(wideImage.pixels));
    CHECK(HGNativeRender3DView(1,384,wideImage.pixels,sizeof(wideImage.pixels)));
    u32 colored=0;
    for (u32 y=0;y<192;y++) for (u32 x=0;x<384;x++) {
        const u8 *p=wideImage.pixels+(y*384+x)*4; colored+=p[3]!=0;
        if (x>=64 && x<320) CHECK(!memcmp(p,image.pixels+(y*256+x-64)*4,4));
    }
    CHECK(colored==6144 && wideImage.before==0x12345678 && wideImage.after==0x87654321);
    for (u32 i=384*192*4;i<sizeof(wideImage.pixels);i++) CHECK(wideImage.pixels[i]==0xA5);
    const u32 invalidWidths[]={0,254,257,514};
    for (u32 i=0;i<sizeof(invalidWidths)/sizeof(invalidWidths[0]);i++) CHECK(!HGNativeRender3DView(1,invalidWidths[i],wideImage.pixels,sizeof(wideImage.pixels)));
    CHECK(!HGNativeRender3DView(1,384,wideImage.pixels,384*192*4-1));

    // This triangle and box are entirely outside the original view, inside the wider one.
    reset(); GXDLInfo list; G3_BeginMakeDL(&list,commands,sizeof(commands));
    G3B_PolygonAttr(&list,0,GX_POLYGONMODE_MODULATE,GX_CULL_NONE,0,31,0);
    G3B_Color(&list,0x3E0); G3B_Begin(&list,GX_BEGIN_TRIANGLES);
    G3B_Vtx(&list,4608,-1024,0); G3B_Vtx(&list,5632,-1024,0); G3B_Vtx(&list,5120,1024,0); G3B_End(&list);
    u32 outsideBytes=G3_EndMakeDL(&list);
    CHECK(HGNativeGeometryFifo(commands,outsideBytes/4)); G3_SwapBuffers(0,0);
    CHECK(HGNativeRender3DView(1,384,wideImage.pixels,sizeof(wideImage.pixels)));
    const u8 *extra=wideImage.pixels+(96*384+352)*4;
    CHECK(!extra[0] && extra[1]==255 && !extra[2] && extra[3]==255);
    CHECK(HGNativeRender3D(1,image.pixels,sizeof(image.pixels)));
    for (u32 i=0;i<256*192;i++) CHECK(!image.pixels[i*4+3]);
    u32 box[]={4608u|((u32)(u16)-1024<<16),1024u<<16,2048u|(1024u<<16)};
    CHECK(HGNativeGeometryCommand(0x70,box,3) && (HGNativeGeometryStatus(FALSE)&2));
    CHECK(HGNativeSetViewWidth(256));
    CHECK(HGNativeGeometryCommand(0x70,box,3) && !(HGNativeGeometryStatus(FALSE)&2));

    // A width change cannot change visibility or capacity halfway through a primitive.
    CHECK(HGNativeSetViewWidth(384)); reset(); G3_BeginMakeDL(&list,commands,sizeof(commands));
    G3B_PolygonAttr(&list,0,GX_POLYGONMODE_MODULATE,GX_CULL_NONE,0,31,0); G3B_Color(&list,31);
    G3B_Begin(&list,GX_BEGIN_TRIANGLES); G3B_Vtx(&list,4608,-1024,0);
    CHECK(HGNativeGeometryFifo(commands,G3_EndMakeDL(&list)/4) && HGNativeSetViewWidth(256));
    G3_BeginMakeDL(&list,commands,sizeof(commands)); G3B_Vtx(&list,5632,-1024,0); G3B_Vtx(&list,5120,1024,0); G3B_End(&list);
    CHECK(HGNativeGeometryFifo(commands,G3_EndMakeDL(&list)/4)); G3_SwapBuffers(0,0);
    HGNativeGeometryFrameInfo frame; CHECK(HGNativeGeometryFrame(1,&frame) && frame.polygons==1);
    G3_BeginMakeDL(&list,commands,sizeof(commands));
    G3B_PolygonAttr(&list,0,GX_POLYGONMODE_MODULATE,GX_CULL_NONE,0,31,0); G3B_Begin(&list,GX_BEGIN_TRIANGLES);
    G3B_Vtx(&list,4608,-1024,0); G3B_Vtx(&list,5632,-1024,0); G3B_Vtx(&list,5120,1024,0); G3B_End(&list);
    CHECK(HGNativeGeometryFifo(commands,G3_EndMakeDL(&list)/4)); G3_SwapBuffers(0,0);
    CHECK(HGNativeGeometryFrame(1,&frame) && !frame.polygons);

    // Changing the X clip plane of a shaded quad must only extend its wings.
    for (u32 fog=0;fog<3;fog++) {
        CHECK(HGNativeSetViewWidth(512)); reset();
        u8 table[32]; memset(table,fog==2 ? 64 : 0,sizeof(table));
        setFog(fog!=0,0,0,0,0,31,table);
        G3_BeginMakeDL(&list,commands,sizeof(commands));
        G3B_PolygonAttr(&list,0,GX_POLYGONMODE_MODULATE,GX_CULL_NONE,0,31,0x8000);
        G3B_Begin(&list,GX_BEGIN_QUADS); G3B_Color(&list,0); G3B_Vtx(&list,3072,1024,0);
        G3B_Color(&list,31); G3B_Vtx(&list,3072,-1024,0);
        G3B_Vtx(&list,6144,-1536,0); G3B_Vtx(&list,5120,1536,0); G3B_End(&list);
        CHECK(HGNativeGeometryFifo(commands,G3_EndMakeDL(&list)/4)); G3_SwapBuffers(0,0);
        CHECK(HGNativeGeometryFrame(1,&frame) && frame.polygons==1);
        CHECK(HGNativeRender3D(1,image.pixels,sizeof(image.pixels)));
        if (!fog) CHECK(pixel(253,67,123,0,0,255));
        static const u32 widths[]={384,512,384};
        for (u32 i=0;i<sizeof(widths)/sizeof(widths[0]);i++) {
            u32 width=widths[i],wings=0; HGNativeGeometryFrameInfo after;
            memset(wideImage.pixels,0xA5,sizeof(wideImage.pixels));
            CHECK(HGNativeRender3DView(1,width,wideImage.pixels,sizeof(wideImage.pixels)));
            for (u32 y=0;y<192;y++) {
                CHECK(!memcmp(wideImage.pixels+(y*width+(width-256)/2)*4,image.pixels+y*256*4,256*4));
                for (u32 x=0;x<width;x++) if (x<(width-256)/2 || x>=(width+256)/2) wings+=wideImage.pixels[(y*width+x)*4+3]!=0;
            }
            CHECK(wings && HGNativeViewWidth()==512 && HGNativeGeometryFrame(1,&after) && !memcmp(&frame,&after,sizeof(frame)));
            CHECK(wideImage.before==0x12345678 && wideImage.after==0x87654321);
            for (u32 n=width*192*4;n<sizeof(wideImage.pixels);n++) CHECK(wideImage.pixels[n]==0xA5);
        }
        // The wide viewport is unsupported even if its canonical prepass succeeds.
        reset(); G3_ViewPort(1,0,255,191); CHECK(triangle(31,0,0,0,0,GX_CULL_NONE)); G3_SwapBuffers(0,0);
        CHECK(HGNativeRender3D(1,image.pixels,sizeof(image.pixels)));
        memset(wideImage.pixels,0xA5,sizeof(wideImage.pixels));
        CHECK(!HGNativeRender3DView(1,512,wideImage.pixels,sizeof(wideImage.pixels)));
        for (u32 n=0;n<sizeof(wideImage.pixels);n++) CHECK(wideImage.pixels[n]==0xA5);
    }
#ifndef HEARTGOLD_ABI32
    CHECK(HGNativeSetViewWidth(384)); reset(); CHECK(triangle(31,0,0,0,0,GX_CULL_NONE)); G3_SwapBuffers(0,0);
    reg_GX_VRAMCNT_A=0x81; memset((void *)HGNativeGraphicsBank(0),0,0x20000);
    memset((void *)HGNativeGraphicsBank(0),0x11,32);
    const u16 palette[]={0x3E0,0x7C00}; GX_LoadBGPltt(palette,0,sizeof(palette));
    reg_GX_DISPCNT=0x10308; reg_G2_BG0CNT=1; reg_G2_BG1CNT=0x100;
    reg_G2_BG0OFS=reg_G2_BG1OFS=0; reg_G2_BLDCNT=0; reg_GX_MASTER_BRIGHT=0;
    CHECK(HGNativeRender2DView(0,384,wideImage.pixels,sizeof(wideImage.pixels)));
    for (u32 x=0;x<384;x++) {
        const u8 *p=wideImage.pixels+(96*384+x)*4;
        CHECK(!p[0] && p[1]==(x<64 || x>=320 ? 255 : 0) && p[2]==(x>=64 && x<320 ? 255 : 0) && p[3]==255);
    }
    memset(wideImage.pixels,0xA5,sizeof(wideImage.pixels));
    u8 *battleImage=(u8 *)HGNativeFrameBuffer();
    reg_G2_BG1CNT=0x102;
    CHECK(HGNativeRenderBattleView(700,battleImage,700*192*4));
    CHECK(HGNativeRender2D(0,wideImage.pixels,sizeof(wideImage.pixels)));
    u32 redPixels=0;
    for (u32 y=0;y<192;y++) {
        CHECK(!memcmp(battleImage+(y*700+(700-256)/2)*4,wideImage.pixels+y*256*4,256*4));
        for (u32 x=0;x<256;x++) { const u8 *p=wideImage.pixels+(y*256+x)*4; redPixels+=p[0]>0 && !p[1] && !p[2]; }
    }
    CHECK(redPixels); reg_G2_BG1CNT=0x100;
    memset(wideImage.pixels,0xA5,sizeof(wideImage.pixels));
    CHECK(!HGNativeRender2DView(1,384,wideImage.pixels,sizeof(wideImage.pixels)));
    reg_GX_DISPCNT|=0x2000;
    CHECK(!HGNativeRender2DView(0,384,wideImage.pixels,sizeof(wideImage.pixels)));
    for (u32 i=0;i<sizeof(wideImage.pixels);i++) CHECK(wideImage.pixels[i]==0xA5);
    reg_GX_DISPCNT=0x10108; reg_G2_BG0CNT=0;
#endif
    CHECK(HGNativeSetViewWidth(256)); reset(); return 0;
}

static FogData fogData;
static WeatherFogTableFade weatherFade;
static HGNativeGeometryFrameInfo fogFrame;
static _Alignas(4) u32 fogAffinePackets[192];
void MI_HBlankDmaCopy32(u32,const void *,void *,u32);

static void setFog(u32 enabled,u32 mode,u32 shift,u32 offset,u16 color,u32 alpha,const u8 table[32]) {
    G3X_AlphaBlend(TRUE); G3X_AlphaTest(FALSE,0);
    Fog_Set(&fogData,-1,enabled,(GXFogBlend)mode,(GXFogSlope)shift,(int)offset);
    Fog_SetColor(&fogData,-1,color,alpha); Fog_SetTable(&fogData,table);
}

static BOOL fogTriangle(u16 color,s16 z,u32 alpha,u32 flags,u32 id,BOOL back) {
    GXDLInfo list; G3_BeginMakeDL(&list,commands,sizeof(commands));
    G3B_PolygonAttr(&list,0,GX_POLYGONMODE_MODULATE,GX_CULL_NONE,(int)id,(int)alpha,(int)flags);
    G3B_Color(&list,color); G3B_Begin(&list,GX_BEGIN_TRIANGLES);
    G3B_Vtx(&list,-2048,-2048,z);
    G3B_Vtx(&list,back ? 0 : 2048,back ? 2048 : -2048,z);
    G3B_Vtx(&list,back ? 2048 : 0,back ? -2048 : 2048,z); G3B_End(&list);
    return HGNativeGeometryFifo(commands,G3_EndMakeDL(&list)/4);
}

static u32 fogAcceptanceStage;
u32 HGFogAcceptanceStage(void) { return fogAcceptanceStage; }
u32 CheckOriginalFog3D(void) {
    fogAcceptanceStage=0;
    CHECK(sizeof(HGNativeGeometryFrameInfo)==32 && sizeof(HGNativeGeometryFogInfo)==40);
    _Alignas(4) u8 guard[48]; memset(guard,0xA5,sizeof(guard));
    HGNativeGeometryFogInfo a,b;
    reset();
    CHECK(!HGNativeGeometryFog(1,(HGNativeGeometryFogInfo *)(guard+4)));
    CHECK(!HGNativeGeometryFog(2,(HGNativeGeometryFogInfo *)(guard+4)) && !HGNativeGeometryFog(0,NULL));
    CHECK(!HGNativeGeometryFog(0,(HGNativeGeometryFogInfo *)(guard+5)));
    for (u32 i=0;i<sizeof(guard);i++) CHECK(guard[i]==0xA5);
    u8 table[32]; memset(table,0xFF,32);
    setFog(TRUE,0,10,0,0,31,table);
    reg_G3X_FOG_COLOR=0xFFFFFFFF; reg_G3X_FOG_OFFSET=0xFFFF;
    CHECK(HGNativeGeometryFog(0,&a) && a.color==0x001F7FFF && a.offset==32767);
    for (u32 i=0;i<32;i++) CHECK(a.density[i]==127);
    CHECK(fogTriangle(31,0,31,0x8000,0,FALSE)); G3_SwapBuffers(0,0);
    CHECK(HGNativeGeometryFog(1,&a) && HGNativeGeometryFrame(1,&fogFrame));
    memset(table,0,32); setFog(TRUE,1,3,17,0x1234,9,table);
    CHECK(HGNativeGeometryFog(0,&b) && b.offset==17 && b.color==(0x1234|(9u<<16)) && (b.control&0x40));
    HGNativeGeometryFogInfo saved; CHECK(HGNativeGeometryFog(1,&saved) && !memcmp(&a,&saved,sizeof(a)));
    HGNativeGeometryCaptureFog();
    HGNativeGeometryFrameInfo frame;
    CHECK(HGNativeGeometryFog(1,&saved) && !memcmp(&b,&saved,sizeof(b)));
    CHECK(HGNativeGeometryFrame(1,&frame) && !memcmp(&frame,&fogFrame,sizeof(frame)));
    reset(); CHECK(!HGNativeGeometryFog(1,(HGNativeGeometryFogInfo *)(guard+4)));
    for (u32 i=0;i<sizeof(guard);i++) CHECK(guard[i]==0xA5);

    // Real gym-style 0xFF density is 127 -> 128, with per-material eligibility.
    for (u32 enabled=0;enabled<2;enabled++) for (u32 eligible=0;eligible<2;eligible++) {
        reset(); memset(table,0xFF,32); setFog(enabled,0,10,0,0,31,table);
        CHECK(fogTriangle(31,0,31,eligible ? 0x8000 : 0,0,FALSE)); G3_SwapBuffers(0,0);
        if (enabled && eligible) fogAcceptanceStage=1;
        CHECK(HGNativeRender3D(1,image.pixels,sizeof(image.pixels)));
        CHECK(pixel(128,96,enabled && eligible ? 0 : 255,0,0,255));
    }
    fogAcceptanceStage=2;
    // Non-endpoint source colors: master-off keeps its legacy golden; fog mode uses six-bit RGB.
    for (u32 test=0;test<4;test++) {
        reset(); memset(table,test==3 ? 64 : 0,32); setFog(test!=0,0,0,0,0,0,table);
        CHECK(fogTriangle(GX_RGB(7,12,19),0,31,test==2 ? 0 : 0x8000,0,FALSE)); G3_SwapBuffers(0,0);
        CHECK(HGNativeRender3D(1,image.pixels,sizeof(image.pixels)));
        CHECK(test==0 ? pixel(128,96,58,99,156,255) : test==3 ? pixel(128,96,28,48,77,123) : pixel(128,96,60,101,158,255));
    }
    // Independent primary-derived clear-depth vectors: padding, interpolation, saturation and u32 wrap.
    static const struct { u16 depth,offset; u8 shift,r,g,b,a; } vectors[]={
        {40,100,0,56,101,150,132},{100,100,0,56,101,150,132},
        {1636,100,0,56,105,146,132},{32767,0,1,40,158,48,58},
        {1636,100,15,40,158,48,58},{8192,0,15,40,154,48,58}};
    for (u32 i=0;i<32;i++) table[i]=7+3*i;
    for (u32 i=0;i<sizeof(vectors)/sizeof(vectors[0]);i++) {
        reset(); setFog(TRUE,0,vectors[i].shift,vectors[i].offset,GX_RGB(4,21,2),5,table);
        G3X_SetClearColor(GX_RGB(7,12,19),17,vectors[i].depth,0,TRUE); G3_SwapBuffers(0,0);
        CHECK(HGNativeRender3D(1,image.pixels,sizeof(image.pixels)));
        CHECK(pixel(0,0,vectors[i].r,vectors[i].g,vectors[i].b,vectors[i].a));
    }
    for (u32 alphaOnly=0;alphaOnly<2;alphaOnly++) for (u32 clearEligible=0;clearEligible<2;clearEligible++) {
        reset(); memset(table,0xFF,32); setFog(TRUE,alphaOnly,0,0,0,31,table);
        G3X_SetClearColor(GX_RGB(7,12,19),0,32767,0,clearEligible); G3_SwapBuffers(0,0);
        CHECK(HGNativeRender3D(1,image.pixels,sizeof(image.pixels)));
        CHECK(pixel(0,0,clearEligible && !alphaOnly ? 0 : 60,clearEligible && !alphaOnly ? 0 : 101,
            clearEligible && !alphaOnly ? 0 : 158,clearEligible ? 255 : 0));
    }
    for (u32 density=0;density<3;density++) {
        reset(); memset(table,density==2 ? 255 : density*64,32); setFog(TRUE,1,0,0,0,0,table);
        G3X_SetClearColor(0,0,32767,0,TRUE);
        CHECK(fogTriangle(GX_RGB(7,12,19),0,17,0x8000,1,FALSE)); G3_SwapBuffers(1,0);
        CHECK(HGNativeRender3D(1,image.pixels,sizeof(image.pixels)) && pixel(128,96,60,101,158,density==0 ? 140 : density==1 ? 66 : 0));
    }
    for (u32 oldFog=0;oldFog<2;oldFog++) for (u32 newFog=0;newFog<2;newFog++) {
        reset(); memset(table,0xFF,32); setFog(TRUE,0,0,0,0,31,table);
        CHECK(fogTriangle(31,0,31,oldFog ? 0x8000 : 0,0,FALSE));
        CHECK(fogTriangle(0x3E0,-512,15,newFog ? 0x8000 : 0,1,FALSE)); G3_SwapBuffers(1,0);
        CHECK(HGNativeRender3D(1,image.pixels,sizeof(image.pixels)));
        CHECK(pixel(128,96,oldFog && newFog ? 0 : 125,oldFog && newFog ? 0 : 125,0,255));
        reset(); setFog(TRUE,0,0,0,0,31,table); G3X_SetClearColor(0,0,32767,0,oldFog);
        CHECK(fogTriangle(0x3E0,0,15,newFog ? 0x8000 : 0,1,FALSE)); G3_SwapBuffers(1,0);
        CHECK(HGNativeRender3D(1,image.pixels,sizeof(image.pixels)));
        CHECK(pixel(128,96,0,oldFog && newFog ? 0 : 255,0,oldFog && newFog ? 255 : 123));
    }
    for (u32 writeDepth=0;writeDepth<2;writeDepth++) for (u32 w=0;w<2;w++) {
        reset(); for (u32 i=0;i<32;i++) table[i]=7+3*i;
        setFog(TRUE,0,8,0,GX_RGB(4,21,2),5,table);
        CHECK(fogTriangle(GX_RGB(7,12,19),0,17,0x8000|(writeDepth ? 0x800 : 0),1,FALSE));
        // Transparent clear must also be fog-eligible for a translucent fragment's AND contract.
        G3X_SetClearColor(0,0,32767,0,TRUE); G3_SwapBuffers(0,(GXBufferMode)w);
        CHECK(HGNativeRender3D(1,image.pixels,sizeof(image.pixels)));
        CHECK(w && writeDepth ? pixel(128,96,56,105,146,132) : pixel(128,96,40,158,48,58));
    }
    // Fog's spare bit cannot become an opaque-backface depth exception.
    reset(); CHECK(fogTriangle(31,0,31,0x8000,0,FALSE)); CHECK(fogTriangle(0x3E0,0,31,0,1,FALSE));
    G3_SwapBuffers(1,0); CHECK(HGNativeRender3D(1,image.pixels,sizeof(image.pixels)) && pixel(128,96,255,0,0,255));
    reset(); CHECK(fogTriangle(31,0,31,0x8000,0,TRUE)); CHECK(fogTriangle(0x3E0,-512,15,0x8000,1,FALSE));
    CHECK(fogTriangle(0x7C00,0,7,0,2,FALSE)); G3_SwapBuffers(1,0);
    CHECK(HGNativeRender3D(1,image.pixels,sizeof(image.pixels)) && pixel(128,96,123,123,0,255));
    // Still-unsupported attributes must leave all caller bytes untouched.
    const u32 unsupported[]={0x2000,0x4000,0x20,0x30,0};
    for (u32 i=0;i<sizeof(unsupported)/sizeof(unsupported[0]);i++) {
        reset(); CHECK(fogTriangle(31,0,i==4 ? 0 : 31,unsupported[i],0,FALSE)); G3_SwapBuffers(0,0);
        memset(image.pixels,0xA5,sizeof(image.pixels)); CHECK(!HGNativeRender3D(1,image.pixels,sizeof(image.pixels)));
        for (u32 i=0;i<sizeof(image.pixels);i++) CHECK(image.pixels[i]==0xA5);
    }
    reset(); return 0;
}

u32 HGFogClockSetup(void) {
    reset(); memset(&fogData,0,sizeof(fogData)); memset(&weatherFade,0,sizeof(weatherFade));
    weatherFade.fog=&fogData;
    WeatherFogTableFade_Clear(&weatherFade); WeatherFogTableFade_Start(&weatherFade,1,1);
    for (u32 i=0;i<64;i++) WeatherFogTableFade_Update(&weatherFade);
    CHECK(weatherFade.table[31]==60 && weatherFade.stepsLeft==63);
    Fog_Set(&fogData,-1,TRUE,GX_FOGBLEND_COLOR_ALPHA,GX_FOGSLOPE_0x1000,0);
    Fog_SetColor(&fogData,-1,GX_RGB(4,21,2),31);
    CHECK(fogTriangle(GX_RGB(7,12,19),0,31,0x8000,0,FALSE)); G3_SwapBuffers(0,0);
    CHECK(HGNativeGeometryFrame(1,&fogFrame));
    reg_GX_DISPCNT=0x10108; reg_G2_BG0CNT=0; reg_G2_BG0OFS=0;
    reg_G2_BLDCNT=reg_G2_MOSAIC=reg_GX_MASTER_BRIGHT=0; GX_SetVisibleWnd(GX_WNDMASK_NONE);
    reg_GXS_DB_DISPCNT=0x10000;
    GX_VBlankIntr(TRUE); OS_EnableIrqMask(OS_IE_VBLANK); OS_EnableIrq();
    MI_HBlankDmaCopy32(0,fogAffinePackets,(void *)REG_BG2X_ADDR,4);
    return 0;
}
u32 HGFogClockStatus(void) { return (u32)(uaddr)&reg_GX_DISPSTAT; }
u32 HGFogClockCheck(u32 frame) {
    HGNativeGeometryFrameInfo now; HGNativeGeometryFogInfo fog;
    CHECK(frame<2 && HGNativeGeometryFrame(1,&now) && !memcmp(&now,&fogFrame,sizeof(now)));
    CHECK(HGNativeGeometryFog(1,&fog) && fog.density[31]==(frame ? 64 : 60));
    for (u32 y=1;y<192;y++) CHECK(HGNativeRasterAffineWrites(0,y)&1);
    CHECK(HGNativeRender3D(1,image.pixels,sizeof(image.pixels)));
    if (!pixel(128,96,48,frame ? 138 : 134,frame ? 89 : 93,255)) {
        const u8 *p=image.pixels+(96*256+128)*4;
        fprintf(stderr,"Fog clock frame %u control=%x density31=%u actualRGBA=%u,%u,%u,%u\n",frame,fog.control,fog.density[31],p[0],p[1],p[2],p[3]);
    }
    CHECK(pixel(128,96,48,frame ? 138 : 134,frame ? 89 : 93,255));
    CHECK(HGNativeRender2D(0,image.pixels,sizeof(image.pixels)));
    const u8 *p=image.pixels+(96*256+128)*4;
    CHECK((p[0]>>3)==6 && (p[1]>>3)==(frame ? 17 : 16) && (p[2]>>3)==11 && p[3]==255);
    CHECK(HGNativeRender2D(1,image.pixels,sizeof(image.pixels)) && HGNativeRender2D(0,image.pixels,sizeof(image.pixels)));
    p=image.pixels+(96*256+128)*4;
    CHECK((p[0]>>3)==6 && (p[1]>>3)==(frame ? 17 : 16) && (p[2]>>3)==11);
    return 0;
}
u32 HGFogClockAdvance(void) {
    MI_StopDma(0); MI_WaitDma(0);
    WeatherFogTableFade_Update(&weatherFade); WeatherFogTableFade_Update(&weatherFade);
    CHECK(weatherFade.table[31]==64 && weatherFade.stepsLeft==61);
    // Live table changed, but repeated renders in the completed epoch must keep its published A snapshot.
    const u32 widths[]={256,384,512};
    for (u32 i=0;i<3;i++) {
        CHECK(HGNativeRender3DView(1,widths[i],wideImage.pixels,sizeof(wideImage.pixels)));
        const u8 *p=wideImage.pixels+(96*widths[i]+widths[i]/2)*4;
        CHECK(p[0]==48 && p[1]==134 && p[2]==93 && p[3]==255);
    }
    u32 line=HGFogClockCheck(0); if (line) return line;
    MI_HBlankDmaCopy32(0,fogAffinePackets,(void *)REG_BG2X_ADDR,4);
    return 0;
}
u32 HGFogClockStop(void) { MI_StopDma(0); MI_WaitDma(0); return 0; }
