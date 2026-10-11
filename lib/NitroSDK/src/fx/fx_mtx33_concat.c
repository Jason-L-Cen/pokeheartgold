// The code below is compiled as ARM (32-bit) code.
#include <nitro/code32.h>
#include <nitro/fx/fx.h>
#include <nitro/fx/fx_mtx33.h>

void MTX_Concat33(const MtxFx33 *a, const MtxFx33 *b, MtxFx33 *c) {
    MtxFx33 temp;
    MtxFx33 *dst;
    fx32 a0, a1, a2;
    fx32 b0, b1, b2;

    if (c == b) {
        dst = &temp;
    } else {
        dst = c;
    }

    a0 = a->a[0];
    a1 = a->a[1];
    a2 = a->a[2];
    dst->a[0] = (fx32)(((fx64)a0 * b->a[0] + (fx64)a1 * b->a[3] + (fx64)a2 * b->a[6]) >> FX32_SHIFT);
    dst->a[1] = (fx32)(((fx64)a0 * b->a[1] + (fx64)a1 * b->a[4] + (fx64)a2 * b->a[7]) >> FX32_SHIFT);
    b0 = b->a[2];
    b1 = b->a[5];
    b2 = b->a[8];
    dst->a[2] = (fx32)(((fx64)a0 * b0 + (fx64)a1 * b1 + (fx64)a2 * b2) >> FX32_SHIFT);
    a0 = a->a[3];
    a1 = a->a[4];
    a2 = a->a[5];
    dst->a[5] = (fx32)(((fx64)a0 * b0 + (fx64)a1 * b1 + (fx64)a2 * b2) >> FX32_SHIFT);
    dst->a[4] = (fx32)(((fx64)a0 * b->a[1] + (fx64)a1 * b->a[4] + (fx64)a2 * b->a[7]) >> FX32_SHIFT);
    b0 = b->a[0];
    b1 = b->a[3];
    b2 = b->a[6];
    dst->a[3] = (fx32)(((fx64)a0 * b0 + (fx64)a1 * b1 + (fx64)a2 * b2) >> FX32_SHIFT);
    a0 = a->a[6];
    a1 = a->a[7];
    a2 = a->a[8];
    dst->a[6] = (fx32)(((fx64)a0 * b0 + (fx64)a1 * b1 + (fx64)a2 * b2) >> FX32_SHIFT);
    dst->a[7] = (fx32)(((fx64)a0 * b->a[1] + (fx64)a1 * b->a[4] + (fx64)a2 * b->a[7]) >> FX32_SHIFT);
    b0 = b->a[2];
    b1 = b->a[5];
    b2 = b->a[8];
    dst->a[8] = (fx32)(((fx64)a0 * b0 + (fx64)a1 * b1 + (fx64)a2 * b2) >> FX32_SHIFT);

    if (dst == &temp) {
        *c = temp;
    }
}

void MTX_MultVec33(const VecFx32 *vec, const MtxFx33 *mtx, VecFx32 *dst) {
    fx32 x, y, z;
    x = vec->x;
    y = vec->y;
    z = vec->z;
    dst->x = (fx32)(((fx64)x * mtx->a[0] + (fx64)y * mtx->a[3] + (fx64)z * mtx->a[6]) >> FX32_SHIFT);
    dst->y = (fx32)(((fx64)x * mtx->a[1] + (fx64)y * mtx->a[4] + (fx64)z * mtx->a[7]) >> FX32_SHIFT);
    dst->z = (fx32)(((fx64)x * mtx->a[2] + (fx64)y * mtx->a[5] + (fx64)z * mtx->a[8]) >> FX32_SHIFT);
}
