// SPDX-License-Identifier: GPL-3.0-or-later
#include <nitro.h>
#include <nitro/gx/g3b.h>
#include <nitro/gx/native_geometry.h>
#include <string.h>
#define CHECK(v) do { if (!(v)) return __LINE__; } while (0)
void G3_LoadMtx44(const MtxFx44 *);
static _Alignas(4) u32 words[256],shine[32];
static BOOL command(u32 op,u32 value) { return HGNativeGeometryCommand(op,&value,1); }
static BOOL vertex(HGNativeGeometryVertex *out) {
    const u32 position[]={4096u|((u32)(u16)-4096<<16),0};
    return HGNativeGeometryCommand(G3OP_VTX_16,position,2) && HGNativeGeometryReadVertex(0,0,out);
}
static BOOL lightSetup(u32 mask,u16 diffuse,u16 ambient,u16 specular,u16 emission,BOOL table) {
    GXDLInfo list; G3_BeginMakeDL(&list,words,sizeof(words));
    G3B_MaterialColorDiffAmb(&list,diffuse,ambient,FALSE);
    G3B_MaterialColorSpecEmi(&list,specular,emission,table);
    for (u32 i=0;i<4;i++) { G3B_LightVector(&list,(GXLightId)i,0,0,-4096); G3B_LightColor(&list,(GXLightId)i,0x7FFF); }
    G3B_PolygonAttr(&list,(int)mask,GX_POLYGONMODE_MODULATE,GX_CULL_NONE,0,31,0);
    G3B_Begin(&list,GX_BEGIN_TRIANGLES); G3B_Normal(&list,0,0,4088); G3B_Vtx(&list,0,0,0); G3B_End(&list);
    return HGNativeGeometryFifo(words,G3_EndMakeDL(&list)/4);
}
u32 CheckOriginalLighting(void) {
    HGNativeGeometryVertex out;
    HGNativeGeometryReset(); CHECK(lightSetup(0,0,0,0,0x3E0,FALSE));
    CHECK(HGNativeGeometryReadVertex(0,0,&out) && out.color==0x3E0);
    HGNativeGeometryReset(); CHECK(lightSetup(1,31,0,0,0,FALSE));
    CHECK(HGNativeGeometryReadVertex(0,0,&out) && out.color==29);
    HGNativeGeometryReset(); CHECK(lightSetup(1,0,0x3E0,0,0,FALSE));
    CHECK(HGNativeGeometryReadVertex(0,0,&out) && out.color==0x3C0);
    HGNativeGeometryReset(); CHECK(lightSetup(15,0,0x7FFF,0,0,FALSE));
    CHECK(HGNativeGeometryReadVertex(0,0,&out) && out.color==0x7FFF);
    HGNativeGeometryReset(); CHECK(lightSetup(1,0,0,31,0,FALSE));
    CHECK(HGNativeGeometryReadVertex(0,0,&out) && out.color==29);
    HGNativeGeometryReset(); memset(shine,0x40,sizeof(shine));
    CHECK(HGNativeGeometryCommand(G3OP_SHININESS,shine,32));
    CHECK(lightSetup(1,0,0,31,0,TRUE));
    CHECK(HGNativeGeometryReadVertex(0,0,&out) && out.color==7);
    HGNativeGeometryReset(); memset(shine,0,sizeof(shine));
    u32 opcode=G3OP_SHININESS; CHECK(HGNativeGeometryFifo(&opcode,1));
    CHECK(HGNativeGeometryFifo(shine,16) && (HGNativeGeometryStatus(FALSE)&(1u<<27)));
    CHECK(HGNativeGeometryFifo(shine+16,16));
    CHECK(lightSetup(1,0,0,31,0,TRUE));
    CHECK(HGNativeGeometryReadVertex(0,0,&out) && !out.color);
    HGNativeGeometryReset(); G3_MaterialColorDiffAmb(0x7C00,0,TRUE);
    CHECK(command(G3OP_BEGIN,0) && vertex(&out) && out.color==0x7C00);

    HGNativeGeometryReset(); CHECK(command(G3OP_LIGHT_VECTOR,512u<<20));
    CHECK(command(G3OP_LIGHT_COLOR,0x7FFF)); G3_MaterialColorDiffAmb(31,0,FALSE);
    MtxFx44 flipped={.a={-4096,0,0,0,0,4096,0,0,0,0,-4096,0,0,0,0,4096}};
    G3_MtxMode(GX_MTXMODE_POSITION_VECTOR); G3_LoadMtx44(&flipped);
    G3_PolygonAttr(1,GX_POLYGONMODE_MODULATE,GX_CULL_NONE,0,31,0);
    CHECK(command(G3OP_BEGIN,0) && command(G3OP_NORMAL,511u<<20) && vertex(&out) && !out.color);

    HGNativeGeometryReset();
    MtxFx44 texture={.a={8192,0,0,0,0,12288,0,0,4096,8192,4096,0,8192,12288,0,4096}};
    G3_MtxMode(GX_MTXMODE_TEXTURE); G3_LoadMtx44(&texture);
    CHECK(command(G3OP_TEXIMAGE_PARAM,1u<<30));
    CHECK(command(G3OP_TEXCOORD,16u|((u32)(u16)-32<<16)) && command(G3OP_BEGIN,0) && vertex(&out));
    CHECK(out.uv[0]==35 && out.uv[1]==-91);
    HGNativeGeometryReset(); texture.a[0]=131072; texture.a[5]=262144;
    G3_MtxMode(GX_MTXMODE_TEXTURE); G3_LoadMtx44(&texture);
    CHECK(command(G3OP_TEXIMAGE_PARAM,2u<<30));
    CHECK(command(G3OP_TEXCOORD,16u|((u32)(u16)-32<<16)));
    CHECK(command(G3OP_NORMAL,256u|(768u<<10)) && command(G3OP_BEGIN,0) && vertex(&out));
    CHECK(out.uv[0]==32 && out.uv[1]==-64);
    HGNativeGeometryReset(); texture.a[0]=16384; texture.a[5]=32768;
    G3_MtxMode(GX_MTXMODE_TEXTURE); G3_LoadMtx44(&texture);
    CHECK(command(G3OP_TEXIMAGE_PARAM,3u<<30));
    CHECK(command(G3OP_TEXCOORD,16u|((u32)(u16)-32<<16)) && command(G3OP_BEGIN,0) && vertex(&out));
    CHECK(out.uv[0]==20 && out.uv[1]==-40);
    CHECK(!HGNativeGeometryCommand(G3OP_SHININESS,shine,31));
    HGNativeGeometryReset(); return 0;
}
