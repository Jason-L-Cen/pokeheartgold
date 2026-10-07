#include <nitro/fx/fx.h>

void MTX_ScaleApply22(const MtxFx22 *mtx, MtxFx22 *dst, fx32 x, fx32 y);
void MTX_Identity22_(MtxFx22 *mtx);
void MTX_Rot22_(MtxFx22 *mtx, fx16 sinphi, fx16 cosphi);

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


#include <nitro/code32.h>
