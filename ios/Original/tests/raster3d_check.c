// SPDX-License-Identifier: GPL-3.0-or-later
#include <nitro.h>
#include <nitro/gx/g3b.h>
#include <nitro/gx/native_geometry.h>
#include <nitro/hw/native_graphics.h>
#include <string.h>
#define CHECK(v) do { if (!(v)) return __LINE__; } while (0)
void G3_LoadMtx44(const MtxFx44 *);
static struct { u32 before; u8 pixels[256*192*4]; u32 after; } image;
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
}
u32 CheckOriginalRaster3D(void) {
    image.before=0x12345678; image.after=0x87654321;
    reset(); CHECK(triangle(31,0,0,0,0,GX_CULL_NONE));
    G3_SwapBuffers(GX_SORTMODE_AUTO,GX_BUFFERMODE_Z);
    CHECK(HGNativeRender3D(1,image.pixels,sizeof(image.pixels)));
    CHECK(pixel(128,96,255,0,0,255) && pixel(0,0,0,0,0,0));
    u32 count=0; for (u32 i=0;i<256*192;i++) count+=image.pixels[i*4+3]!=0;
    CHECK(count==6144 && image.before==0x12345678 && image.after==0x87654321);
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
    // Opaque geometry is supported; fractional clear alpha remains unsupported.
    G3X_SetClearColor(0,15,32767,0,FALSE); G3_SwapBuffers(0,0);
    memset(image.pixels,0xA5,sizeof(image.pixels)); CHECK(!HGNativeRender3D(1,image.pixels,sizeof(image.pixels)));
    for (u32 i=0;i<sizeof(image.pixels);i++) CHECK(image.pixels[i]==0xA5);
    CHECK(!HGNativeRender3D(1,image.pixels,sizeof(image.pixels)-1) && !HGNativeRender3D(1,NULL,sizeof(image.pixels)));
    reset(); CHECK(triangle(31,0,0,0,0,GX_CULL_NONE)); G3_SwapBuffers(0,GX_BUFFERMODE_W);
    CHECK(!HGNativeRender3D(1,image.pixels,sizeof(image.pixels)));
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
    reset(); return 0;
}
