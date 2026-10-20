#include <nitro/code32.h>
#include <nitro/hw/io_reg.h>
#include <nitro/mi/dma.h>
#include <nitro/mi/memory.h>
#include <nitro/types.h>
#ifdef HEARTGOLD_NATIVE
#include <nitro/gx/g3imm.h>
#include "assert.h"
static void nativeDefaults(void) {
    const u32 polygon=0x1F0080,zero=0;
    GF_ASSERT(HGNativeGeometryCommand(0x29,&polygon,1));
    GF_ASSERT(HGNativeGeometryCommand(0x2A,&zero,1));
    GF_ASSERT(HGNativeGeometryCommand(0x2B,&zero,1));
}
static void nativeMtxStack(BOOL identityProjection) {
    HGNativeGeometryStatus(TRUE);
    u32 state=HGNativeGeometryStatus(FALSE);
    G3_MtxMode(GX_MTXMODE_TEXTURE); G3_Identity();
    G3_MtxMode(GX_MTXMODE_PROJECTION);
    if (state&0x2000) G3_PopMtx(1);
    if (identityProjection) G3_Identity();
    G3_MtxMode(GX_MTXMODE_POSITION_VECTOR); G3_PopMtx((state>>8)&31); G3_Identity();
}
#endif

void MI_Copy32B(const void *src, void *dst);
void GXi_NopClearFifo128_(void *reg);
void G3X_Init(void);
void G3X_Reset(void);
void G3X_ClearFifo(void);
void G3X_InitMtxStack(void);
void G3X_ResetMtxStack(void);
void G3X_SetFog(u32 enable, u32 alphamode, u32 depth, s32 offset);
u32 G3X_GetClipMtx(void *dst);
u32 G3X_GetVectorMtx(void *dst);
void G3X_SetEdgeColorTable(void *tbl_ptr);
void G3X_SetFogTable(void *tbl_ptr);
void G3X_SetClearColor(u32 col, u32 alpha, u32 depth, u32 polygon_id, u32 enable_fog);
void G3X_InitTable(void);
u32 G3X_GetMtxStackLevelPV(u32 *level);
u32 G3X_GetMtxStackLevelPJ(u32 *level);
u32 G3X_GetBoxTestResult(u32 *result);
void G3X_SetHOffset(u32 offset);

extern u32 GXi_DmaId;

void G3X_Init() {
#ifdef HEARTGOLD_NATIVE
    HGNativeGeometryReset();
    reg_G3X_DISP3DCNT=REG_G3X_DISP3DCNT_AAE_MASK;
    reg_G2_BG0OFS=0;
    G3X_InitMtxStack(); HGNativeGeometryClear(0,0x7FFF);
    reg_G3X_CLRIMAGE_OFFSET=0; reg_G3X_FOG_COLOR=0; reg_G3X_FOG_OFFSET=0;
    reg_G2_BG0CNT&=~REG_G2_BG0CNT_PRIORITY_MASK;
    G3X_InitTable(); nativeDefaults();
#else
    G3X_ClearFifo();
    reg_G3_END_VTXS = 0x0;
    while (reg_G3X_GXSTAT & REG_G3X_GXSTAT_B_MASK) {} // wait for geometry engine to not be busy
    reg_G3X_DISP3DCNT = 0x0;
    reg_G3X_GXSTAT = 0x0;
    reg_G2_BG0OFS = 0x0;
    reg_G3X_DISP3DCNT |= REG_G3X_DISP3DCNT_GO_MASK;
    reg_G3X_DISP3DCNT |= REG_G3X_DISP3DCNT_RO_MASK;
    reg_G3X_DISP3DCNT &= ~(REG_G3X_DISP3DCNT_GO_MASK | REG_G3X_DISP3DCNT_RO_MASK | REG_G3X_DISP3DCNT_THS_MASK);
    reg_G3X_DISP3DCNT = (u16)((reg_G3X_DISP3DCNT & ~(REG_G3X_DISP3DCNT_GO_MASK | REG_G3X_DISP3DCNT_RO_MASK)) | REG_G3X_DISP3DCNT_AAE_MASK);
    reg_G3X_DISP3DCNT = (u16)(reg_G3X_DISP3DCNT & (u16)~(REG_G3X_DISP3DCNT_GO_MASK | REG_G3X_DISP3DCNT_RO_MASK | REG_G3X_DISP3DCNT_ATE_MASK));
    reg_G3X_GXSTAT |= REG_G3X_GXSTAT_SE_MASK;
    reg_G3X_GXSTAT = (reg_G3X_GXSTAT & ~REG_G3X_GXSTAT_FI_MASK) | 0x80000000;
    G3X_InitMtxStack();
    reg_G3X_CLEAR_COLOR = 0x0;
    reg_G3X_CLEAR_DEPTH = 0x7FFF;
    reg_G3X_CLRIMAGE_OFFSET = 0x0;
    reg_G3X_FOG_COLOR = 0x0;
    reg_G3X_FOG_OFFSET = 0x0;
    reg_G2_BG0CNT &= ~REG_G2_BG0CNT_PRIORITY_MASK;
    G3X_InitTable();
    reg_G3_POLYGON_ATTR = 0x1F0080;
    reg_G3_TEXIMAGE_PARAM = 0x0;
    reg_G3_TEXPLTT_BASE = 0x0;
#endif
}

void G3X_Reset() {
#ifdef HEARTGOLD_NATIVE
    HGNativeGeometryStatus(TRUE);
    reg_G3X_DISP3DCNT&=~(REG_G3X_DISP3DCNT_GO_MASK|REG_G3X_DISP3DCNT_RO_MASK);
    G3X_ResetMtxStack(); nativeDefaults();
#else
    while (reg_G3X_GXSTAT & REG_G3X_GXSTAT_B_MASK) {}
    reg_G3X_GXSTAT |= REG_G3X_GXSTAT_SE_MASK;
    reg_G3X_DISP3DCNT |= REG_G3X_DISP3DCNT_GO_MASK;
    reg_G3X_DISP3DCNT |= REG_G3X_DISP3DCNT_RO_MASK;
    G3X_ResetMtxStack();
    reg_G3_POLYGON_ATTR = 0x1F0080;
    reg_G3_TEXIMAGE_PARAM = 0x0;
    reg_G3_TEXPLTT_BASE = 0x0;
#endif
}

void G3X_ClearFifo() {
#ifdef HEARTGOLD_NATIVE
    GXi_NopClearFifo128_((void *)0x04000400);
#else
    GXi_NopClearFifo128_((void *)&reg_G3X_GXFIFO);
    while (reg_G3X_GXSTAT & REG_G3X_GXSTAT_B_MASK) {}
#endif
}

void G3X_InitMtxStack() {
#ifdef HEARTGOLD_NATIVE
    nativeMtxStack(TRUE);
#else
    u32 PV_level, PJ_level;
    reg_G3X_GXSTAT |= REG_G3X_GXSTAT_SE_MASK;
    while (G3X_GetMtxStackLevelPV(&PV_level)) {}
    while (G3X_GetMtxStackLevelPJ(&PJ_level)) {}
    reg_G3_MTX_MODE = 0x3;
    reg_G3_MTX_IDENTITY = 0x0;
    reg_G3_MTX_MODE = 0x0;
    if (PJ_level) {
        reg_G3_MTX_POP = PJ_level;
    }
    reg_G3_MTX_IDENTITY = 0x0;
    reg_G3_MTX_MODE = 0x2;
    reg_G3_MTX_POP = PV_level;
    reg_G3_MTX_IDENTITY = 0x0;
#endif
}

void G3X_ResetMtxStack() {
#ifdef HEARTGOLD_NATIVE
    nativeMtxStack(FALSE);
#else
    u32 PV_level, PJ_level;
    reg_G3X_GXSTAT |= REG_G3X_GXSTAT_SE_MASK;
    while (G3X_GetMtxStackLevelPV(&PV_level)) {}
    while (G3X_GetMtxStackLevelPJ(&PJ_level)) {}
    reg_G3_MTX_MODE = 0x3;
    reg_G3_MTX_IDENTITY = 0x0;
    reg_G3_MTX_MODE = 0x0;
    if (PJ_level) {
        reg_G3_MTX_POP = PJ_level;
    }

    reg_G3_MTX_MODE = 0x2;
    reg_G3_MTX_POP = PV_level;
    reg_G3_MTX_IDENTITY = 0x0;
#endif
}

void G3X_SetFog(u32 enable, u32 alphamode, u32 depth, s32 offset) {
    if (enable) {
        reg_G3X_FOG_OFFSET = (u16)offset;
        reg_G3X_DISP3DCNT = (u16)((reg_G3X_DISP3DCNT & ~0x3f40) | ((depth << REG_G3X_DISP3DCNT_FOG_SHIFT_SHIFT) | (alphamode << REG_G3X_DISP3DCNT_FMOD_SHIFT) | 0x80));

    } else {
        reg_G3X_DISP3DCNT = (u16)(reg_G3X_DISP3DCNT & (u16)~(REG_G3X_DISP3DCNT_GO_MASK | REG_G3X_DISP3DCNT_RO_MASK | REG_G3X_DISP3DCNT_FME_MASK));
    }
}

u32 G3X_GetClipMtx(void *dst) {
#ifdef HEARTGOLD_NATIVE
    return HGNativeGeometryMatrix(0,dst) ? 0 : (u32)-1;
#else
    if (reg_G3X_GXSTAT & REG_G3X_GXSTAT_B_MASK) {
        return (u32)-1;
    } else {
        MI_Copy64B((void *)&reg_G3X_CLIPMTX_RESULT_0, dst);
        return 0;
    }
#endif
}

u32 G3X_GetVectorMtx(void *dst) {
#ifdef HEARTGOLD_NATIVE
    return HGNativeGeometryMatrix(1,dst) ? 0 : (u32)-1;
#else
    if (reg_G3X_GXSTAT & REG_G3X_GXSTAT_B_MASK) {
        return (u32)-1;
    } else {
        MI_Copy36B((void *)&reg_G3X_VECMTX_RESULT_0, dst);
        return 0;
    }
#endif
}

void G3X_SetEdgeColorTable(void *tbl_ptr) {
    MIi_CpuCopy16(tbl_ptr, (void *)&reg_G3X_EDGE_COLOR_0, 0x10);
}

void G3X_SetFogTable(void *tbl_ptr) {
    MI_Copy32B(tbl_ptr, (void *)&reg_G3X_FOG_TABLE_0);
}

void G3X_SetClearColor(u32 col, u32 alpha, u32 depth, u32 polygon_id, u32 enable_fog) {
    u32 temp = col | (alpha << 0x10) | (polygon_id << 0x18);
    if (enable_fog) {
        temp |= 0x8000;
    }
#ifdef HEARTGOLD_NATIVE
    HGNativeGeometryClear(temp,depth);
#else
    reg_G3X_CLEAR_COLOR = temp;
    reg_G3X_CLEAR_DEPTH = (u16)depth;
#endif
}

void G3X_InitTable() {
    if (GXi_DmaId != -1) {
        MI_DmaFill32Async(GXi_DmaId, (void *)&reg_G3X_EDGE_COLOR_0, 0x0, 0x10, 0x0, 0x0);
        MI_DmaFill32(GXi_DmaId, (void *)&reg_G3X_FOG_TABLE_0, 0x0, 0x60);
    } else {
        MIi_CpuClear32(0x0, (void *)&reg_G3X_EDGE_COLOR_0, 0x10);
        MIi_CpuClear32(0x0, (void *)&reg_G3X_FOG_TABLE_0, 0x60);
    }
#ifdef HEARTGOLD_NATIVE
    const u32 shine[32]={0};
    GF_ASSERT(HGNativeGeometryCommand(0x34,shine,32));
#else
    for (int i = 0; i < 0x20; i++) {
        reg_G3_SHININESS = 0x0;
    }
#endif
}

u32 G3X_GetMtxStackLevelPV(u32 *level) {
#ifdef HEARTGOLD_NATIVE
    if (!level) return (u32)-1;
    *level=(HGNativeGeometryStatus(FALSE)>>8)&31; return 0;
#else
    if (reg_G3X_GXSTAT & REG_G3X_GXSTAT_SB_MASK) {
        return (u32)-1;
    } else {
        *level = (reg_G3X_GXSTAT & REG_G3X_GXSTAT_PV_MASK) >> REG_G3X_GXSTAT_PV_SHIFT;
        return 0;
    }
#endif
}

u32 G3X_GetMtxStackLevelPJ(u32 *level) {
#ifdef HEARTGOLD_NATIVE
    if (!level) return (u32)-1;
    *level=(HGNativeGeometryStatus(FALSE)>>13)&1; return 0;
#else
    if (reg_G3X_GXSTAT & REG_G3X_GXSTAT_SB_MASK) {
        return (u32)-1;
    } else {
        *level = (reg_G3X_GXSTAT & REG_G3X_GXSTAT_PJ_MASK) >> REG_G3X_GXSTAT_PJ_SHIFT;
        return 0;
    }
#endif
}

u32 G3X_GetBoxTestResult(u32 *result) {
    if (reg_G3X_GXSTAT & REG_G3X_GXSTAT_TB_MASK) {
        return (u32)-1;
    } else {
        *result = (reg_G3X_GXSTAT & REG_G3X_GXSTAT_TR_MASK);
        return 0;
    }
}

void G3X_SetHOffset(u32 offset) {
    reg_G2_BG0OFS = offset;
}
