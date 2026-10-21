// SPDX-License-Identifier: GPL-3.0-or-later
#include <nitro.h>
#include <nitro/gx/g3b.h>
#include <nitro/gx/native_geometry.h>
#include <nitro/hw/native_graphics.h>
#include <string.h>
#define CHECK(v) do { if (!(v)) return __LINE__; } while (0)
void G3_LoadMtx44(const MtxFx44 *);
static struct { u32 before; u8 pixels[256*192*4]; u32 after; } image;
static struct { u32 before; u8 pixels[512*192*4]; u32 after; } wideImage;
static u32 checkWideView(void);
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
    // Enabled fog remains unsupported and must preserve the output transactionally.
    reg_G3X_DISP3DCNT|=0x80; G3_SwapBuffers(0,0);
    memset(image.pixels,0xA5,sizeof(image.pixels)); CHECK(!HGNativeRender3D(1,image.pixels,sizeof(image.pixels)));
    for (u32 i=0;i<sizeof(image.pixels);i++) CHECK(image.pixels[i]==0xA5);
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
