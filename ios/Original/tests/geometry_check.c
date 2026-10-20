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
void G3i_OrthoW_(fx32,fx32,fx32,fx32,fx32,fx32,fx32,BOOL,MtxFx44 *);
void G3i_LookAt_(const VecFx32 *,const VecFx32 *,const VecFx32 *,BOOL,MtxFx43 *);
u32 G3X_GetClipMtx(void *);
u32 G3X_GetVectorMtx(void *);
u32 G3X_GetMtxStackLevelPV(u32 *);
u32 G3X_GetMtxStackLevelPJ(u32 *);
void G3X_Init(void);
void G3X_Reset(void);
u32 G3X_GetBoxTestResult(u32 *);

static BOOL runBox(s16 x,s16 y,s16 z,s16 width,s16 height,s16 depth) {
    u32 words[]={(u16)x|((u32)(u16)y<<16),(u16)z|((u32)(u16)width<<16),(u16)height|((u32)(u16)depth<<16)};
    return HGNativeGeometryCommand(G3OP_BOX_TEST,words,3);
}
static BOOL latchBoxAttributes(u32 attributes) {
    const u32 primitive=0;
    return HGNativeGeometryCommand(G3OP_POLYGON_ATTR,&attributes,1) && HGNativeGeometryCommand(G3OP_BEGIN,&primitive,1) && HGNativeGeometryCommand(G3OP_END,NULL,0);
}
static u32 checkBoxes(void) {
    HGNativeGeometryReset(); u32 result=99,attributes=0x1000;
    CHECK(!G3X_GetBoxTestResult(&result) && !result && G3X_GetBoxTestResult(NULL)==(u32)-1);
    CHECK(latchBoxAttributes(attributes));
    CHECK(runBox(-1024,-1024,-1024,2048,2048,2048));
    CHECK(!G3X_GetBoxTestResult(&result) && result==2 && (HGNativeGeometryStatus(FALSE)&2));
    MtxFx44 matrix; CHECK(!G3X_GetClipMtx(&matrix));
    for (u32 i=0;i<16;i++) CHECK(matrix.a[i]==(i%5==0 ? 4096 : 0));
    HGNativeGeometryFrameInfo frame; CHECK(HGNativeGeometryFrame(0,&frame) && !frame.vertices && !frame.polygons);
    for (u32 axis=0;axis<3;axis++) for (u32 side=0;side<2;side++) {
        s16 origin[3]={0,0,0}; origin[axis]=(s16)(side ? 8192 : -8192);
        CHECK(runBox(origin[0],origin[1],origin[2],1024,1024,1024));
        CHECK(!G3X_GetBoxTestResult(&result) && !result);
    }
    CHECK(runBox(4096,0,0,1024,1024,1024) && !G3X_GetBoxTestResult(&result) && result==2);
    CHECK(runBox(3072,-1024,-1024,2048,2048,2048) && !G3X_GetBoxTestResult(&result) && result==2);
    CHECK(runBox(1024,1024,1024,-2048,-2048,-2048) && !G3X_GetBoxTestResult(&result) && result==2);
    CHECK(runBox(32760,0,0,16,1024,1024) && !G3X_GetBoxTestResult(&result) && result==2);
    // A box enclosing the whole frustum has no face within it.
    CHECK(runBox(-8192,-8192,-8192,16384,16384,16384) && !G3X_GetBoxTestResult(&result) && !result);
    G3_MtxMode(GX_MTXMODE_POSITION_VECTOR); G3_Translate(8192,0,0);
    CHECK(runBox(-512,-512,-512,1024,1024,1024) && !G3X_GetBoxTestResult(&result) && !result);
    MtxFx43 shear={.a={4096,0,0,0,4096,0,16384,0,4096,-20480,0,0}};
    G3_LoadMtx43(&shear); attributes=0;
    CHECK(latchBoxAttributes(attributes));
    CHECK(runBox(-8192,-512,0,16384,1024,8192) && !G3X_GetBoxTestResult(&result) && !result);
    attributes=0x1000; CHECK(HGNativeGeometryCommand(G3OP_POLYGON_ATTR,&attributes,1));
    CHECK(runBox(-8192,-512,0,16384,1024,8192) && !G3X_GetBoxTestResult(&result) && !result);
    CHECK(latchBoxAttributes(attributes));
    CHECK(runBox(-8192,-512,0,16384,1024,8192) && !G3X_GetBoxTestResult(&result) && result==2);
    HGNativeGeometryReset();
    const u32 box[]={0xFC00FC00,0x0800FC00,0x08000800},command=G3OP_BOX_TEST;
    CHECK(HGNativeGeometryFifo(&command,1)); result=0xA5A5A5A5;
    CHECK((HGNativeGeometryStatus(FALSE)&1) && G3X_GetBoxTestResult(&result)==(u32)-1 && result==0xA5A5A5A5);
    CHECK(HGNativeGeometryFifo(box,2) && G3X_GetBoxTestResult(&result)==(u32)-1);
    CHECK(HGNativeGeometryFifo(box+2,1) && !G3X_GetBoxTestResult(&result) && result==2 && !(HGNativeGeometryStatus(FALSE)&1));
    CHECK(!HGNativeGeometryCommand(G3OP_BOX_TEST,box,2));
    CHECK(!HGNativeGeometryPacket(0xFF70,box,3) && !G3X_GetBoxTestResult(&result) && result==2);
    HGNativeGeometryReset(); CHECK(!G3X_GetBoxTestResult(&result) && !result);
    return 0;
}

u32 CheckOriginalGeometry(void) {
    struct { u32 before; MtxFx44 matrix; u32 after; } output={.before=0x12345678,.after=0x87654321};
    const fx32 unit44[16]={4096,0,0,0,0,4096,0,0,0,0,4096,0,0,0,0,4096};
    const fx32 unit33[9]={4096,0,0,0,4096,0,0,0,4096};
    G3X_Init(); G3X_ClearFifo();
    CHECK(reg_G3X_DISP3DCNT==REG_G3X_DISP3DCNT_AAE_MASK && reg_G3X_CLEAR_COLOR==0 && reg_G3X_CLEAR_DEPTH==0x7FFF);
    CHECK(!(HGNativeGeometryStatus(FALSE)&0x8000));
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
    const fx32 quarterTurns[3][9]={
        {4096,0,0,0,0,4096,0,-4096,0},
        {0,0,-4096,0,4096,0,4096,0,0},
        {0,4096,0,-4096,0,0,0,0,4096}
    };
    void (*rotate[3])(fx32,fx32)={G3_RotX,G3_RotY,G3_RotZ};
    G3_MtxMode(GX_MTXMODE_POSITION_VECTOR);
    for (u32 axis=0;axis<3;axis++) {
        G3_Identity(); G3_Translate(4096,8192,12288); rotate[axis](4096,0);
        CHECK(!G3X_GetVectorMtx(&vector) && !memcmp(&vector,quarterTurns[axis],36));
        CHECK(!G3X_GetClipMtx(&output.matrix));
        CHECK(output.matrix.a[12]==4096 && output.matrix.a[13]==8192 && output.matrix.a[14]==12288);
        for (u32 turn=0;turn<3;turn++) rotate[axis](4096,0);
        CHECK(!G3X_GetVectorMtx(&vector) && !memcmp(&vector,unit33,36));
    }
    HGNativeGeometryReset(); G3_MtxMode(GX_MTXMODE_TEXTURE); G3_LoadMtx44(&projection);
    MtxFx44 ortho;
    const fx32 orthoGolden[16]={4096,0,0,0,0,4096,0,0,0,0,-4096,0,0,0,-8192,4096};
    G3i_OrthoW_(4096,-4096,-4096,4096,4096,12288,4096,FALSE,&ortho);
    CHECK(!memcmp(&ortho,orthoGolden,64));
    CHECK(!G3X_GetClipMtx(&output.matrix) && !memcmp(&output.matrix,unit44,64));
    CHECK(HGNativeGeometryMatrix(2,&output.matrix) && !memcmp(&output.matrix,&projection,64));
    G3i_OrthoW_(4096,-4096,-4096,4096,4096,12288,4096,TRUE,NULL);
    CHECK(!G3X_GetClipMtx(&output.matrix) && !memcmp(&output.matrix,orthoGolden,64));
    HGNativeGeometryReset();
    const VecFx32 eyes[2]={{0,0,16384},{16384,0,0}},up={0,4096,0},target={0,0,0};
    const fx32 cameraGolden[2][12]={
        {4096,0,0,0,4096,0,0,0,4096,0,0,-16384},
        {0,0,4096,0,4096,0,-4096,0,0,0,0,-16384}
    };
    MtxFx43 camera;
    for (u32 i=0;i<2;i++) {
        HGNativeGeometryReset();
        G3i_LookAt_(&eyes[i],&up,&target,FALSE,&camera);
        CHECK(!memcmp(&camera,cameraGolden[i],48));
        CHECK(!G3X_GetClipMtx(&output.matrix) && !memcmp(&output.matrix,unit44,64));
        G3i_LookAt_(&eyes[i],&up,&target,TRUE,i ? &camera : NULL);
        CHECK(!G3X_GetClipMtx(&output.matrix));
        for (u32 row=0;row<4;row++) for (u32 col=0;col<3;col++)
            CHECK(output.matrix.a[row*4+col]==cameraGolden[i][row*3+col]);
        CHECK(!G3X_GetVectorMtx(&vector) && !memcmp(&vector,cameraGolden[i],36));
    }
    CHECK(output.before==0x12345678 && output.after==0x87654321);
    HGNativeGeometryReset();
    u32 boxLine=checkBoxes(); if (boxLine) return boxLine;
    return 0;
}
