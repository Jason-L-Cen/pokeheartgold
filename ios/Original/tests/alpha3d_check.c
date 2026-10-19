// SPDX-License-Identifier: GPL-3.0-or-later
#include <nitro.h>
#include <nitro/gx/g3b.h>
#include <nitro/gx/native_geometry.h>
#include <nitro/hw/native_graphics.h>
#include <string.h>
#define CHECK(v) do { if (!(v)) return __LINE__; } while (0)
static _Alignas(4) u32 commands[64];
static u8 image[256*192*4];
static BOOL triangle(u16 color,s16 depth,u32 id,u32 alpha,u32 misc,s16 bottom) {
    GXDLInfo list; G3_BeginMakeDL(&list,commands,sizeof(commands));
    G3B_PolygonAttr(&list,0,GX_POLYGONMODE_MODULATE,GX_CULL_NONE,(int)id,(int)alpha,(int)misc);
    G3B_Color(&list,color); G3B_Begin(&list,GX_BEGIN_TRIANGLES);
    G3B_Vtx(&list,-2048,bottom,depth); G3B_Vtx(&list,2048,bottom,depth); G3B_Vtx(&list,0,2048,depth); G3B_End(&list);
    return HGNativeGeometryFifo(commands,G3_EndMakeDL(&list)/4);
}
static BOOL center(u8 r,u8 g,u8 b,u8 a) {
    const u8 *pixel=image+(96*256+128)*4;
    return pixel[0]==r && pixel[1]==g && pixel[2]==b && pixel[3]==a;
}
static void reset(void) {
    HGNativeGeometryReset(); G3X_SetClearColor(0,0,32767,0,FALSE);
    G3X_AlphaBlend(TRUE); G3X_AlphaTest(FALSE,0);
}
static BOOL render(u32 manual) {
    G3_SwapBuffers((GXSortMode)manual,GX_BUFFERMODE_Z); return HGNativeRender3D(1,image,sizeof(image));
}
u32 CheckOriginalAlpha3D(void) {
    reset(); CHECK(triangle(0x3E0,0,1,15,0,-2048)); CHECK(render(1));
    CHECK(center(0,255,0,123));
    reset(); CHECK(triangle(0x3E0,0,1,15,0,-2048));
    CHECK(triangle(0x7C00,-512,1,15,0,-2048)); CHECK(render(1));
    CHECK(center(0,255,0,123));
    reset(); CHECK(triangle(0x3E0,0,1,15,0,-2048));
    CHECK(triangle(0x7C00,-512,2,15,0,-2048)); CHECK(render(1));
    CHECK(center(0,123,123,123));
    reset(); G3X_AlphaBlend(FALSE);
    CHECK(triangle(0x3E0,0,1,15,0,-2048) && triangle(0x7C00,-512,2,15,0,-2048)); CHECK(render(1));
    CHECK(center(0,0,255,123));
    G3X_AlphaBlend(TRUE); CHECK(HGNativeRender3D(1,image,sizeof(image)) && center(0,0,255,123));

    reset(); CHECK(triangle(0x3E0,-512,1,15,0,-2048) && triangle(0x7C00,512,2,15,0,-2048)); CHECK(render(1));
    CHECK(center(0,123,123,123));
    reset(); CHECK(triangle(0x3E0,-512,1,15,0x800,-2048) && triangle(0x7C00,512,2,15,0,-2048)); CHECK(render(1));
    CHECK(center(0,255,0,123));
    reset(); G3X_SetClearColor(31,31,32767,0,FALSE);
    CHECK(triangle(0x3E0,0,1,15,0,-2048)); CHECK(render(1));
    CHECK(center(123,123,0,255));
    reset(); G3X_AlphaTest(TRUE,15); CHECK(triangle(31,0,1,15,0,-2048)); CHECK(render(1));
    CHECK(center(0,0,0,0)); G3X_AlphaTest(FALSE,0);
    CHECK(HGNativeRender3D(1,image,sizeof(image)) && center(0,0,0,0));
    reset(); G3X_AlphaTest(TRUE,14); CHECK(triangle(31,0,1,15,0,-2048)); CHECK(render(1));
    CHECK(center(255,0,0,123));

    reset(); CHECK(triangle(0x7C00,0,1,7,0,-3072) && triangle(31,0,2,15,0,-2048)); CHECK(render(0));
    CHECK(center(189,0,58,123));
    reset(); CHECK(triangle(0x7C00,0,1,7,0,-3072) && triangle(31,0,2,15,0,-2048)); CHECK(render(1));
    CHECK(center(123,0,123,123));
    reset(); CHECK(triangle(31,-512,1,15,0,-2048) && triangle(0x7C00,512,2,31,0,-2048)); CHECK(render(1));
    CHECK(center(123,0,123,255));
#ifndef HEARTGOLD_ABI32
    reset(); CHECK(triangle(31,0,1,15,0,-2048)); CHECK(render(1));
    const u16 backdrop=0x3E0; GX_LoadBGPltt(&backdrop,0,2);
    reg_GX_DISPCNT=0x10108; reg_G2_BG0CNT=0; reg_G2_BG0OFS=0;
    reg_G2_BLDCNT=1u<<13; reg_GX_MASTER_BRIGHT=0;
    CHECK(HGNativeRender2D(0,image,sizeof(image)) && center(123,123,0,255));
    reg_G2_BLDCNT=0;
    CHECK(HGNativeRender2D(0,image,sizeof(image)) && center(255,0,0,255));
#endif
    reset(); return 0;
}
