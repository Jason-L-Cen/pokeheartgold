#include <nitro.h>
#include <nitro/code32.h>

void GX_SendFifo48B(const void *src, void *dst);
void GX_SendFifo64B(const void *src, void *dst);
void G3_LoadMtx44(const MtxFx44 *mtx);
void G3_LoadMtx43(const MtxFx43 *mtx);
void G3_MultMtx43(const MtxFx43 *mtx);
void G3_MultMtx33(const MtxFx33 *mtx);

void G3_LoadMtx44(const MtxFx44 *mtx) {
#ifdef HEARTGOLD_NATIVE
    HGNativeG3Immediate(G3OP_MTX_LOAD_4x4,mtx,16);
#else
    reg_G3X_GXFIFO = G3OP_MTX_LOAD_4x4;
    GX_SendFifo64B(mtx, (void *)&reg_G3X_GXFIFO);
#endif
}

void G3_LoadMtx43(const MtxFx43 *mtx) {
#ifdef HEARTGOLD_NATIVE
    HGNativeG3Immediate(G3OP_MTX_LOAD_4x3,mtx,12);
#else
    reg_G3X_GXFIFO = G3OP_MTX_LOAD_4x3;
    GX_SendFifo48B(mtx, (void *)&reg_G3X_GXFIFO);
#endif
}

void G3_MultMtx43(const MtxFx43 *mtx) {
#ifdef HEARTGOLD_NATIVE
    HGNativeG3Immediate(G3OP_MTX_MULT_4x3,mtx,12);
#else
    reg_G3X_GXFIFO = G3OP_MTX_MULT_4x3;
    GX_SendFifo48B(mtx, (void *)&reg_G3X_GXFIFO);
#endif
}

void G3_MultMtx33(const MtxFx33 *mtx) {
#ifdef HEARTGOLD_NATIVE
    HGNativeG3Immediate(G3OP_MTX_MULT_3x3,mtx,9);
#else
    reg_G3X_GXFIFO = G3OP_MTX_MULT_3x3;
    MI_Copy36B(mtx, (void *)&reg_G3X_GXFIFO);
#endif
}
