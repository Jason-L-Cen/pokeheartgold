#pragma force_active on
#include <nitro/fx/fx.h>

// The code below is compiled as ARM (32-bit) code.
#include <nitro/code32.h>

// The code below is compiled as Thumb code.
#include <nitro/code16.h>

void MTX_Rot22_(MtxFx22 *mtx, fx16 sinphi, fx16 cosphi);

void MTX_Rot22_(MtxFx22 *mtx, fx16 sinphi, fx16 cosphi) {
    mtx->a[0] = cosphi;
    mtx->a[1] = sinphi;
    mtx->a[2] = (fx32)(0u - (u32)sinphi);
    mtx->a[3] = cosphi;
}

// The code below is compiled as ARM (32-bit) code.
#include <nitro/code32.h>

void MTX_ScaleApply22(const MtxFx22 *mtx, MtxFx22 *dst, fx32 x, fx32 y);

void MTX_ScaleApply22(const MtxFx22 *mtx, MtxFx22 *dst, fx32 x, fx32 y) {
    dst->a[0] = (fx32)(((fx64)x * mtx->a[0]) >> FX32_SHIFT);
    dst->a[1] = (fx32)(((fx64)x * mtx->a[1]) >> FX32_SHIFT);
    dst->a[2] = (fx32)(((fx64)y * mtx->a[2]) >> FX32_SHIFT);
    dst->a[3] = (fx32)(((fx64)y * mtx->a[3]) >> FX32_SHIFT);
}
