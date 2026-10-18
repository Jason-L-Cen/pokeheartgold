#pragma force_active on
#include <nitro/fx/fx.h>
void MTX_Identity43_(register MtxFx43 *pDst);
void MTX_Copy43To44_(register const MtxFx43 *pSrc, register MtxFx44 *pDst);

// The code below is compiled as ARM (32-bit) code.
#include <nitro/code32.h>

// clang-format off
#ifdef HEARTGOLD_NATIVE
void MTX_Identity43_(MtxFx43 *pDst) {
    *pDst = (MtxFx43){.a = {1 << FX32_SHIFT, 0, 0, 0, 1 << FX32_SHIFT, 0, 0, 0, 1 << FX32_SHIFT, 0, 0, 0}};
}
void MTX_Copy43To44_(const MtxFx43 *pSrc, MtxFx44 *pDst) {
    for (int row = 0; row < 4; row++) {
        fx32 x = pSrc->m[row][0], y = pSrc->m[row][1], z = pSrc->m[row][2];
        pDst->m[row][0] = x; pDst->m[row][1] = y; pDst->m[row][2] = z;
        pDst->m[row][3] = row == 3 ? 1 << FX32_SHIFT : 0;
    }
}
#else
asm void MTX_Identity43_(register MtxFx43 *pDst) {
    mov r2, #0x1000
    mov r3, #0
    stmia r0!, {r2, r3}
    mov r1, #0
    stmia r0!, {r1, r3}
    stmia r0!, {r2, r3}
    stmia r0!, {r1, r3}
    stmia r0!, {r2, r3}
    stmia r0!, {r1, r3}
    bx lr
}
// clang-format on

// clang-format off
asm void MTX_Copy43To44_(register const MtxFx43 *pSrc, register MtxFx44 *pDst) {
    stmdb sp!, {r4}
    mov r12, #0
    ldmia r0!, {r2, r3, r4}
    stmia r1!, {r2, r3, r4, r12}
    ldmia r0!, {r2, r3, r4}
    stmia r1!, {r2, r3, r4, r12}
    ldmia r0!, {r2, r3, r4}
    stmia r1!, {r2, r3, r4, r12}
    mov r12, #0x1000
    ldmia r0!, {r2, r3, r4}
    stmia r1!, {r2, r3, r4, r12}
    ldmia sp!, {r4}
    bx lr
}
#endif
// clang-format on
