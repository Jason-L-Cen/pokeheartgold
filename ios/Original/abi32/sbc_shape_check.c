// SPDX-License-Identifier: GPL-3.0-or-later
#include <nnsys/g3d_render_internal.h>
#include <string.h>
#include <nitro/gx/g3imm.h>
#include <nitro/gx/native_geometry.h>
void G3_LoadMtx44(const MtxFx44 *);
G3dEvpMtxCache gEvpMtxCache[64];
static void (*volatile shape)(G3dRS *,u32,const G3dResShpData *,u32)=NNSi_G3dFuncSbc_SHP_InternalDefault;
static u32 calls;
static void callback(G3dRS *state) { calls++; state->flag|=G3D_RS_FLAG_SKIP_CMD; }
u32 CheckNativeShapeCallback(void) {
    G3dRS state; memset(&state,0,sizeof(state)); state.flag=G3D_RS_FLAG_NO_GE_OUTPUT;
    shape(&state,0,NULL,42);
    if (state.flag!=G3D_RS_FLAG_NO_GE_OUTPUT) return __LINE__;
    calls=0; state.cbFunc[NNS_G3D_SBC_SHP]=callback; state.cbTiming[NNS_G3D_SBC_SHP]=G3D_SBC_CALLBACK_TIMING_A;
    shape(&state,0,NULL,99);
    return calls==1 && (state.flag&G3D_RS_FLAG_SKIP_CMD) ? 0 : __LINE__;
}
u32 CheckNativeNodeMix(void) {
    struct { NNSG3dResMdl model; G3dResEvpMtx inverse[2]; } resource;
    memset(&resource,0,sizeof(resource)); resource.model.ofsEvpMtx=(u32)((u8 *)resource.inverse-(u8 *)&resource);
    for (u32 i=0;i<2;i++) {
        for (u32 d=0;d<3;d++) { resource.inverse[i].invM.a[d*3+d]=4096; resource.inverse[i].invN.a[d*3+d]=4096; }
    }
    NNSG3dRenderObj object; memset(&object,0,sizeof(object)); object.resMdl=&resource.model;
    const u8 command[]={NNS_G3D_SBC_NODEMIX,3,2,0,0,128,1,1,128};
    for (u32 cached=0;cached<2;cached++) {
        HGNativeGeometryReset(); memset(gEvpMtxCache,0,sizeof(gEvpMtxCache));
        MtxFx44 projection={0}; projection.a[0]=projection.a[5]=projection.a[10]=8192; projection.a[15]=4096;
        G3_MtxMode(GX_MTXMODE_PROJECTION); G3_LoadMtx44(&projection); G3_MtxMode(GX_MTXMODE_POSITION_VECTOR);
        for (u32 i=0;i<2;i++) {
            MtxFx43 matrix={0}; matrix.a[0]=matrix.a[4]=matrix.a[8]=(s32)(4096*(i+1)); matrix.a[9]=(s32)((i ? 6 : 2)*4096);
            G3_LoadMtx43(&matrix); G3_StoreMtx(i);
            for (u32 r=0;r<4;r++) for (u32 c=0;c<3;c++) gEvpMtxCache[i].posMtx.a[r*4+c]=matrix.a[r*3+c];
            gEvpMtxCache[i].posMtx.a[15]=4096;
            memcpy(&gEvpMtxCache[i].dirMtx,&matrix,sizeof(MtxFx33));
        }
        G3dRS state; memset(&state,0,sizeof(state)); state.pRenderObj=&object; state.c=command;
        state.evpCached[0]=cached ? 3 : 0; NNSi_G3dFuncSbc_NODEMIX(&state,0);
        if (state.c!=command+sizeof(command) || state.evpCached[0]!=3) return __LINE__;
        MtxFx44 clip; MtxFx33 normal;
        if (!HGNativeGeometryMatrix(0,&clip) || !HGNativeGeometryMatrix(1,&normal)) return __LINE__;
        if (clip.a[0]!=12288 || clip.a[5]!=12288 || clip.a[10]!=12288 || clip.a[12]!=32768 || normal.a[0]!=6144 || normal.a[4]!=6144 || normal.a[8]!=6144) return __LINE__;
        G3_Identity(); G3_RestoreMtx(3);
        if (!HGNativeGeometryMatrix(0,&clip) || clip.a[0]!=12288 || clip.a[12]!=32768 || (HGNativeGeometryStatus(FALSE)&0x8000)) return __LINE__;
    }
    return 0;
}
