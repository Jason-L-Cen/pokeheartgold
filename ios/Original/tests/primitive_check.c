// SPDX-License-Identifier: GPL-3.0-or-later
#include <nitro.h>
#include <nitro/gx/g3b.h>
#include <nitro/gx/native_geometry.h>
#include <stddef.h>
#include <string.h>
#define CHECK(v) do { if (!(v)) return __LINE__; } while (0)
static BOOL command(u32 opcode,u32 value) { return HGNativeGeometryCommand(opcode,&value,1); }
static BOOL vertex(s16 x,s16 y,s16 z) {
    const u32 words[]={(u16)x|((u32)(u16)y<<16),(u16)z};
    return HGNativeGeometryCommand(G3OP_VTX_16,words,2);
}
u32 CheckOriginalPrimitives(void) {
    _Static_assert(sizeof(HGNativeGeometryVertex)==24 && sizeof(HGNativeGeometryPolygon)==28,"Native geometry output layout");
    static _Alignas(4) u32 displayList[128];
    GXDLInfo info; HGNativeGeometryReset();
    G3_MtxMode(GX_MTXMODE_POSITION_VECTOR); G3_Translate(4096,8192,-4096);
    G3_ViewPort(0,0,255,191);
    CHECK(command(G3OP_TEXCOORD,16u|((u32)(u16)-32<<16)));
    G3_BeginMakeDL(&info,displayList,sizeof(displayList));
    G3B_PolygonAttr(&info,0,GX_POLYGONMODE_MODULATE,GX_CULL_NONE,7,31,0);
    G3B_Color(&info,31); G3B_Begin(&info,GX_BEGIN_TRIANGLES);
    G3B_Vtx(&info,-4096,-4096,0); G3B_Vtx(&info,4096,-4096,0); G3B_Vtx(&info,0,4096,0); G3B_End(&info);
    u32 bytes=G3_EndMakeDL(&info); CHECK(bytes && bytes%4==0 && bytes<=sizeof(displayList));
    CHECK(HGNativeGeometryFifo(displayList,bytes/4));
    HGNativeGeometryFrameInfo frame; CHECK(!HGNativeGeometryFrame(1,&frame));
    CHECK(HGNativeGeometryFrame(0,&frame) && frame.vertices==3 && frame.polygons==1);
    struct { u32 before; HGNativeGeometryVertex value; u32 after; } output={.before=0x12345678,.after=0x87654321};
    CHECK(HGNativeGeometryReadVertex(0,0,&output.value));
    const s32 expected[]={0,4096,-4096,4096};
    CHECK(!memcmp(output.value.clip,expected,sizeof(expected)) && output.value.color==31);
    CHECK(output.value.uv[0]==16 && output.value.uv[1]==-32 && !output.value.reserved);
    HGNativeGeometryPolygon polygon;
    CHECK(HGNativeGeometryReadPolygon(0,0,&polygon) && polygon.count==3);
    CHECK(polygon.indices[0]==0 && polygon.indices[1]==1 && polygon.indices[2]==2);
    CHECK(polygon.attributes==((7u<<24)|(31u<<16)|0xC0) && polygon.viewport==0xBFFF0000);
    G3_SwapBuffers(GX_SORTMODE_AUTO,GX_BUFFERMODE_Z);
    CHECK(HGNativeGeometryFrame(1,&frame) && frame.serial==1 && frame.vertices==3 && frame.polygons==1);
    CHECK(HGNativeGeometryFifo(displayList,bytes/4));
    const u32 invalid[]={0x0000FF50,0};
    CHECK(!HGNativeGeometryFifo(invalid,2));
    CHECK(HGNativeGeometryFrame(1,&frame) && frame.serial==1);
    CHECK(HGNativeGeometryFrame(0,&frame) && frame.vertices==3 && frame.polygons==1);
    G3_SwapBuffers(GX_SORTMODE_MANUAL,GX_BUFFERMODE_W);
    CHECK(HGNativeGeometryFrame(1,&frame) && frame.serial==2 && frame.swapMode==3);
    CHECK(HGNativeGeometryFrame(0,&frame) && !frame.vertices && !frame.polygons);
    CHECK(command(G3OP_BEGIN,0) && command(G3OP_COLOR,0x7C00) && vertex(1,2,3));
    CHECK(HGNativeGeometryReadVertex(1,0,&output.value) && output.value.color==31);
    CHECK(!HGNativeGeometryReadVertex(1,3,&output.value) && !HGNativeGeometryReadPolygon(1,1,&polygon));
    CHECK(output.before==0x12345678 && output.after==0x87654321);

    HGNativeGeometryReset(); CHECK(command(G3OP_BEGIN,GX_BEGIN_TRIANGLE_STRIP));
    for (u32 i=0;i<4;i++) CHECK(vertex((s16)i,0,0));
    CHECK(HGNativeGeometryReadPolygon(0,1,&polygon));
    CHECK(polygon.indices[0]==2 && polygon.indices[1]==1 && polygon.indices[2]==3);
    HGNativeGeometryReset(); CHECK(command(G3OP_BEGIN,GX_BEGIN_QUAD_STRIP));
    for (u32 i=0;i<6;i++) CHECK(vertex((s16)i,0,0));
    const u16 firstQuad[]={0,1,3,2},secondQuad[]={2,3,5,4};
    CHECK(HGNativeGeometryReadPolygon(0,0,&polygon) && polygon.count==4 && !memcmp(polygon.indices,firstQuad,8));
    CHECK(HGNativeGeometryReadPolygon(0,1,&polygon) && !memcmp(polygon.indices,secondQuad,8));
    HGNativeGeometryReset(); CHECK(command(G3OP_BEGIN,GX_BEGIN_QUADS));
    for (u32 i=0;i<4;i++) CHECK(vertex((s16)i,0,0));
    CHECK(HGNativeGeometryReadPolygon(0,0,&polygon) && polygon.count==4 && polygon.indices[2]==2 && polygon.indices[3]==3);

    HGNativeGeometryReset(); CHECK(command(G3OP_BEGIN,0) && vertex(-1,2,-3));
    CHECK(command(G3OP_VTX_XY,5u|((u32)(u16)-6<<16)));
    CHECK(command(G3OP_VTX_XZ,(u16)-7|((u32)8<<16)));
    CHECK(HGNativeGeometryReadVertex(0,2,&output.value));
    CHECK(output.value.clip[0]==-7 && output.value.clip[1]==-6 && output.value.clip[2]==8);
    CHECK(command(G3OP_VTX_YZ,9u|((u32)(u16)-10<<16)));
    CHECK(command(G3OP_VTX_10,1023u|(2u<<10)|(1021u<<20)));
    CHECK(command(G3OP_VTX_DIFF,1u|(1022u<<10)|(3u<<20)));
    CHECK(HGNativeGeometryReadVertex(0,5,&output.value));
    CHECK(output.value.clip[0]==-63 && output.value.clip[1]==126 && output.value.clip[2]==-189);
    u32 texture=3u<<30; CHECK(HGNativeGeometryCommand(G3OP_TEXIMAGE_PARAM,&texture,1));
    CHECK(!HGNativeGeometryCommand(G3OP_VEC_TEST,&texture,1));

    HGNativeGeometryReset(); CHECK(command(G3OP_BEGIN,0));
    for (u32 i=0;i<6144;i++) CHECK(vertex(0,0,0));
    CHECK(!vertex(0,0,0));
    CHECK(HGNativeGeometryFrame(0,&frame) && frame.vertices==6144 && frame.polygons==2048);
    CHECK(!HGNativeGeometryFrame(2,&frame) && !HGNativeGeometryFrame(0,NULL));
    HGNativeGeometryReset(); return 0;
}
