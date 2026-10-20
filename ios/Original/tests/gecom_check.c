// SPDX-License-Identifier: GPL-3.0-or-later
#include <nitro.h>
#include <nitro/gx/g3b.h>
#include <nitro/gx/native_geometry.h>
#include <nitro/hw/native_graphics.h>
#include <nnsys/g3d/gecom.h>
#include <nnsys/g3d_util_internal.h>
#include <string.h>
#define CHECK(v) do { if (!(v)) return __LINE__; } while (0)
static _Alignas(4) u32 words[256];
static u8 pixels[256*192*4];
static struct { u32 before,words[G3D_GE_BUFFER_WORDS+1],after; } buffer;
static u32 triangle(void) {
    GXDLInfo list; memset(words,0,sizeof(words)); G3_BeginMakeDL(&list,words,sizeof(words));
    G3B_PolygonAttr(&list,0,GX_POLYGONMODE_MODULATE,GX_CULL_NONE,0,31,0);
    G3B_Begin(&list,GX_BEGIN_TRIANGLES);
    G3B_Vtx(&list,-2048,-2048,0); G3B_Vtx(&list,2048,-2048,0); G3B_Vtx(&list,0,2048,0); G3B_End(&list);
    return G3_EndMakeDL(&list);
}
u32 CheckOriginalGecom(void) {
#ifdef HEARTGOLD_ABI32
    extern void NNS_G3dInit(void);
    NNS_G3dInit();
    CHECK(!(HGNativeGeometryStatus(FALSE)&0x8000));
#endif
    u32 savedDma=GXi_DmaId; G3dGeBufferState saved=gGeBufferState;
    memset(&buffer,0,sizeof(buffer)); buffer.before=0x12345678; buffer.after=0x87654321;
    gGeBufferState=(G3dGeBufferState){.pBuffer=buffer.words};
    // Exercise small/large lists and both original DMA choices through the same native transport.
    for (u32 variant=0;variant<4;variant++) {
        HGNativeGeometryReset(); G3X_SetClearColor(0,0,32767,0,FALSE);
        G3_ViewPort(0,0,255,191); GXi_DmaId=variant&1 ? (u32)-1 : 0;
        gGeBufferState.useFastDma=variant>>1;
        u32 color=variant&1 ? 0x3E0 : 31;
        gGeBufferState.isLocked=TRUE;
        NNS_G3dGeBufferOP_N(G3OP_COLOR,&color,1);
        CHECK(buffer.words[0]==2);
        gGeBufferState.isLocked=FALSE;
        u32 bytes=triangle(); CHECK(bytes<256);
        NNS_G3dGeSendDL(NULL,0); CHECK(buffer.words[0]==2);
        NNS_G3dGeSendDL(words,variant&2 ? 256 : bytes);
        CHECK(!buffer.words[0] && !gGeBufferState.isLocked);
        HGNativeGeometryVertex vertex; HGNativeGeometryFrameInfo frame;
        CHECK(HGNativeGeometryReadVertex(0,0,&vertex) && vertex.color==color);
        CHECK(HGNativeGeometryFrame(0,&frame) && frame.vertices==3 && frame.polygons==1);
        G3_SwapBuffers(GX_SORTMODE_AUTO,GX_BUFFERMODE_Z);
        CHECK(HGNativeRender3D(1,pixels,sizeof(pixels)));
        u8 *p=pixels+(96*256+128)*4;
        CHECK(p[0]==(variant&1 ? 0 : 255) && p[1]==(variant&1 ? 255 : 0) && !p[2] && p[3]==255);
        NNS_G3dGeWaitSendDL();
    }
    HGNativeGeometryReset();
    const u32 material[]={31,0x3E0,0x1F0000};
    gGeBufferState.pBuffer=NULL;
    NNS_G3dGeBufferOP_N(G3OP_DIF_AMB|(G3OP_SPE_EMI<<8)|(G3OP_POLYGON_ATTR<<16),material,3);
    u32 begin=GX_BEGIN_TRIANGLES,position[]={0,0};
    NNS_G3dGeBufferOP_N(G3OP_BEGIN,&begin,1); NNS_G3dGeBufferOP_N(G3OP_VTX_16,position,2);
    NNS_G3dGeBufferOP_N(G3OP_END,NULL,0);
    HGNativeGeometryVertex vertex; CHECK(HGNativeGeometryReadVertex(0,0,&vertex));

    HGNativeGeometryReset(); gGeBufferState.pBuffer=buffer.words; gGeBufferState.isLocked=TRUE;
    memset(words,0,sizeof(words));
    NNS_G3dGeBufferOP_N(0,words,G3D_GE_BUFFER_WORDS-1);
    CHECK(buffer.words[0]==G3D_GE_BUFFER_WORDS);
    CHECK(buffer.before==0x12345678 && buffer.after==0x87654321);
    gGeBufferState.isLocked=FALSE; NNS_G3dGeFlushBuffer(); CHECK(!buffer.words[0]);
    for (u32 i=0;i<G3D_GE_BUFFER_WORDS;i++) buffer.words[i+1]=0;
    buffer.words[0]=G3D_GE_BUFFER_WORDS;
    NNS_G3dGeBufferOP_N(0,NULL,0); CHECK(!buffer.words[0]);

    HGNativeGeometryReset();
    MtxFx44 projection={.a={8192,0,0,0,0,12288,0,0,0,0,4096,0,0,0,0,4096}};
    MtxFx43 positionMtx={.a={4096,0,0,0,8192,0,0,0,12288,256,512,768}},actual;
    MtxFx33 direction;
    CHECK(HGNativeGeometryCommand(G3OP_MTX_MODE,(u32[]){GX_MTXMODE_PROJECTION},1));
    CHECK(HGNativeGeometryCommand(G3OP_MTX_LOAD_4x4,&projection,16));
    G3_MtxMode(GX_MTXMODE_POSITION_VECTOR); G3_LoadMtx43(&positionMtx);
    MtxFx44 before,after; CHECK(HGNativeGeometryMatrix(0,&before));
    NNS_G3dGetCurrentMtx(&actual,&direction);
    CHECK(!memcmp(&actual,&positionMtx,sizeof(actual)) && !memcmp(&direction,&positionMtx,sizeof(direction)));
    CHECK(HGNativeGeometryMatrix(0,&after) && !memcmp(&before,&after,sizeof(before)));
    NNS_G3dGetCurrentMtx(NULL,NULL);
    G3_Identity(); CHECK(HGNativeGeometryMatrix(0,&after) && !memcmp(&projection,&after,sizeof(after)));

    // A later invalid opcode must roll back earlier commands in the same packet.
    HGNativeGeometryReset(); G3_MtxMode(GX_MTXMODE_POSITION_VECTOR);
    u32 invalid[]={GX_MTXMODE_PROJECTION,0xFF};
    CHECK(!HGNativeGeometryPacket(G3OP_MTX_MODE,invalid,2));
    G3_Translate(4096,0,0); NNS_G3dGetCurrentMtx(&actual,NULL); CHECK(actual.a[9]==4096);
    CHECK(!HGNativeGeometryPacket(0,NULL,1) && !HGNativeGeometryPacket(0,words,0x40000000));
    CHECK(buffer.before==0x12345678 && buffer.after==0x87654321);
    gGeBufferState=saved; GXi_DmaId=savedDma; HGNativeGeometryReset(); return 0;
}
