#include <nitro/fx/fx.h>

void MTX_ScaleApply22(const MtxFx22 *mtx, MtxFx22 *dst, fx32 x, fx32 y);
void MTX_Identity22_(MtxFx22 *mtx);
void MTX_Rot22_(MtxFx22 *mtx, fx32 sinphi, fx32 cosphi);

#include <nitro/code32.h>

asm void MTX_Identity22_(MtxFx22 *mtx) {
    mov r1, #0x0
    mov r2, #0x1000
    mov r3, #0x0
    stmia r0!, {r2-r3}
    stmia r0!, {r1-r2}
    bx lr
}

#include <nitro/code16.h>

asm void MTX_Rot22_(MtxFx22 *mtx, fx32 sinphi, fx32 cosphi) {
    str r2, [r0, #0x0]
    str r1, [r0, #0x4]
    neg r1, r1
    str r1, [r0, #0x8]
    str r2, [r0, #0xc]
    bx lr
}

#include <nitro/code32.h>

void MTX_ScaleApply22(const MtxFx22 *mtx, MtxFx22 *dst, fx32 x, fx32 y) {
    dst->a[0] = (fx32)(((fx64)x * mtx->a[0]) >> FX32_SHIFT);
    dst->a[1] = (fx32)(((fx64)x * mtx->a[1]) >> FX32_SHIFT);
    dst->a[2] = (fx32)(((fx64)y * mtx->a[2]) >> FX32_SHIFT);
    dst->a[3] = (fx32)(((fx64)y * mtx->a[3]) >> FX32_SHIFT);
}
