#include <nitro/code32.h>
#include <nitro/fx/fx.h>

void MTX_ScaleApply33(MtxFx33 *mtx, MtxFx33 *dst, fx32 x, fx32 y, fx32 z);

void MTX_ScaleApply33(MtxFx33 *mtx, MtxFx33 *dst, fx32 x, fx32 y, fx32 z) {
    dst->a[0] = (fx32)(((fx64)x * mtx->a[0]) >> FX32_SHIFT);
    dst->a[1] = (fx32)(((fx64)x * mtx->a[1]) >> FX32_SHIFT);
    dst->a[2] = (fx32)(((fx64)x * mtx->a[2]) >> FX32_SHIFT);
    dst->a[3] = (fx32)(((fx64)y * mtx->a[3]) >> FX32_SHIFT);
    dst->a[4] = (fx32)(((fx64)y * mtx->a[4]) >> FX32_SHIFT);
    dst->a[5] = (fx32)(((fx64)y * mtx->a[5]) >> FX32_SHIFT);
    dst->a[6] = (fx32)(((fx64)z * mtx->a[6]) >> FX32_SHIFT);
    dst->a[7] = (fx32)(((fx64)z * mtx->a[7]) >> FX32_SHIFT);
    dst->a[8] = (fx32)(((fx64)z * mtx->a[8]) >> FX32_SHIFT);
}
