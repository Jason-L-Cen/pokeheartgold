// SPDX-License-Identifier: GPL-3.0-or-later
#include <nitro.h>
#include <nitro/gx/native_geometry.h>
#include <string.h>
#define CHECK(v) do { if (!(v)) return __LINE__; } while (0)
void G3_LoadMtx44(const MtxFx44 *);
void G3_LoadMtx43(const MtxFx43 *);
void G3_MultMtx33(const MtxFx33 *);
void GX_SendFifo48B(const void *,void *);
void GX_SendFifo64B(const void *,void *);
void G3X_InitMtxStack(void);
void G3X_ResetMtxStack(void);
void G3X_ClearFifo(void);
u32 G3X_GetClipMtx(void *);
u32 G3X_GetVectorMtx(void *);
u32 G3X_GetMtxStackLevelPV(u32 *);
u32 G3X_GetMtxStackLevelPJ(u32 *);

u32 CheckOriginalGeometry(void) {
    struct { u32 before; MtxFx44 matrix; u32 after; } output={.before=0x12345678,.after=0x87654321};
    const fx32 unit44[16]={4096,0,0,0,0,4096,0,0,0,0,4096,0,0,0,0,4096};
    const fx32 unit33[9]={4096,0,0,0,4096,0,0,0,4096};
    HGNativeGeometryReset(); G3X_InitMtxStack(); G3X_ClearFifo();
    CHECK(!G3X_GetClipMtx(&output.matrix) && !memcmp(&output.matrix,unit44,64));
    MtxFx33 vector; CHECK(!G3X_GetVectorMtx(&vector) && !memcmp(&vector,unit33,36));
    MtxFx44 projection={.a={8192,0,0,0,0,12288,0,0,0,0,16384,0,0,0,0,4096}};
    MtxFx43 position={.a={4096,0,0,0,4096,0,0,0,4096,5*4096,6*4096,7*4096}};
    G3_MtxMode(GX_MTXMODE_PROJECTION); G3_LoadMtx44(&projection);
    G3_MtxMode(GX_MTXMODE_POSITION_VECTOR); G3_LoadMtx43(&position);
    CHECK(!G3X_GetClipMtx(&output.matrix));
    const fx32 first[16]={8192,0,0,0,0,12288,0,0,0,0,16384,0,10*4096,18*4096,28*4096,4096};
    CHECK(!memcmp(&output.matrix,first,64));
    const u32 scale[]={8192,16384,24576};
    CHECK(HGNativeGeometryCommand(G3OP_MTX_SCALE,scale,3));
    G3_Translate(4096,8192,12288);
    const fx32 transformed[16]={16384,0,0,0,0,49152,0,0,0,0,98304,0,14*4096,42*4096,100*4096,4096};
    CHECK(!G3X_GetClipMtx(&output.matrix) && !memcmp(&output.matrix,transformed,64));
    CHECK(!G3X_GetVectorMtx(&vector) && !memcmp(&vector,unit33,36));
    MtxFx33 rotation={.a={0,4096,0,-4096,0,0,0,0,4096}};
    G3_MultMtx33(&rotation);
    CHECK(!G3X_GetVectorMtx(&vector) && !memcmp(&vector,&rotation,36));
    MtxFx44 saved; CHECK(!G3X_GetClipMtx(&saved));
    G3_PushMtx(); u32 level;
    CHECK(!G3X_GetMtxStackLevelPV(&level) && level==1);
    G3_Identity(); G3_PopMtx(1);
    CHECK(!G3X_GetClipMtx(&output.matrix) && !memcmp(&output.matrix,&saved,64));
    CHECK(!G3X_GetVectorMtx(&vector) && !memcmp(&vector,&rotation,36));
    G3_MtxMode(GX_MTXMODE_PROJECTION); G3_PushMtx(); G3_Identity();
    CHECK(!G3X_GetMtxStackLevelPJ(&level) && level==1); G3_PopMtx(17);
    CHECK(!G3X_GetMtxStackLevelPJ(&level) && !level);
    CHECK(!G3X_GetClipMtx(&output.matrix) && !memcmp(&output.matrix,&saved,64));
    G3_MtxMode(GX_MTXMODE_TEXTURE); G3_LoadMtx44(&projection);
    CHECK(HGNativeGeometryMatrix(2,&output.matrix) && !memcmp(&output.matrix,&projection,64));
    G3X_ResetMtxStack();
    CHECK(HGNativeGeometryMatrix(2,&output.matrix) && !memcmp(&output.matrix,unit44,64));
    CHECK(!G3X_GetClipMtx(&output.matrix) && !memcmp(&output.matrix,&projection,64));
    G3_MtxMode(GX_MTXMODE_PROJECTION); G3_PushMtx(); G3_Identity(); G3X_ResetMtxStack();
    CHECK(!G3X_GetMtxStackLevelPJ(&level) && !level);
    CHECK(!G3X_GetClipMtx(&output.matrix) && !memcmp(&output.matrix,unit44,64));

    HGNativeGeometryReset();
    const u32 header[]={0x00161510,0};
    CHECK(HGNativeGeometryFifo(header,2) && (HGNativeGeometryStatus(FALSE)&(1u<<27)));
    memset(&output.matrix,0xA5,64); CHECK(G3X_GetClipMtx(&output.matrix)==(u32)-1);
    for (u32 i=0;i<16;i++) CHECK((u32)output.matrix.a[i]==0xA5A5A5A5);
    GX_SendFifo64B(&projection,(void *)0x04000400);
    CHECK(!G3X_GetClipMtx(&output.matrix) && !memcmp(&output.matrix,&projection,64));
    G3_MtxMode(GX_MTXMODE_POSITION_VECTOR);
    const u32 load43=G3OP_MTX_LOAD_4x3;
    CHECK(HGNativeGeometryFifo(&load43,1)); GX_SendFifo48B(&position,(void *)0x0400040C);
    CHECK(!G3X_GetClipMtx(&output.matrix) && !memcmp(&output.matrix,first,64));
    const u32 invalid[]={0x00FF1510,0};
    CHECK(!HGNativeGeometryFifo(invalid,2));
    CHECK(!G3X_GetClipMtx(&output.matrix) && !memcmp(&output.matrix,first,64));
    CHECK(!HGNativeGeometryCommand(0x23,header,2));
    CHECK(!HGNativeGeometryCommand(G3OP_MTX_LOAD_4x4,&projection,15));
    CHECK(!HGNativeGeometryCommand(G3OP_MTX_LOAD_4x4,NULL,16));
    CHECK(!HGNativeGeometryCommand(G3OP_MTX_MODE,(const u8 *)header+1,1));
    CHECK(!HGNativeGeometryFifo(NULL,1));
    CHECK(!HGNativeGeometryMatrix(0,NULL) && !HGNativeGeometryMatrix(3,&output.matrix));
    CHECK(output.before==0x12345678 && output.after==0x87654321);
    HGNativeGeometryReset(); G3_MtxMode(GX_MTXMODE_POSITION_VECTOR);
    for (u32 i=0;i<31;i++) G3_PushMtx();
    CHECK(!(HGNativeGeometryStatus(FALSE)&0x8000)); G3_PushMtx();
    CHECK(HGNativeGeometryStatus(FALSE)&0x8000);
    for (u32 i=0;i<64;i++) G3_PushMtx();
    CHECK(HGNativeGeometryStatus(TRUE)&0x8000);
    CHECK(!(HGNativeGeometryStatus(FALSE)&0x8000));
    HGNativeGeometryReset();
    return 0;
}
