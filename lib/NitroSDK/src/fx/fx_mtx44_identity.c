#pragma force_active on
#include <nitro/fx/fx.h>
void MTX_Identity44_(register MtxFx44 *pDst);
void MTX_Copy44To43_(register const MtxFx44 *pSrc, register MtxFx43 *pDst);

// The code below is compiled as ARM (32-bit) code.
#include <nitro/code32.h>

// clang-format off
#ifdef HEARTGOLD_NATIVE
void MTX_Identity44_(MtxFx44 *pDst) {
    *pDst = (MtxFx44){.a = {1 << FX32_SHIFT, 0, 0, 0, 0, 1 << FX32_SHIFT, 0, 0, 0, 0, 1 << FX32_SHIFT, 0, 0, 0, 0, 1 << FX32_SHIFT}};
}
void MTX_Copy44To43_(const MtxFx44 *pSrc, MtxFx43 *pDst) {
    for (int row = 0; row < 4; row++) {
        fx32 x = pSrc->m[row][0], y = pSrc->m[row][1], z = pSrc->m[row][2];
        pDst->m[row][0] = x; pDst->m[row][1] = y; pDst->m[row][2] = z;
    }
}
#else
asm void MTX_Identity44_(register MtxFx44 *pDst) {
    mov r2, #0x1000
    mov r3, #0
    stmia r0!, {r2, r3}
    mov r1, #0
    stmia r0!, {r1, r3}
    stmia r0!, {r1, r2, r3}
    stmia r0!, {r1, r3}
    stmia r0!, {r1, r2, r3}
    stmia r0!, {r1, r3}
    stmia r0!, {r1, r2}
    bx lr
}
// clang-format on

// clang-format off
asm void MTX_Copy44To43_(register const MtxFx44 *pSrc, register MtxFx43 *pDst) {
    ldmia r0!, {r2, r3, r12}
    add r0, r0, #4
    stmia r1!, {r2, r3, r12}
    ldmia r0!, {r2, r3, r12}
    add r0, r0, #4
    stmia r1!, {r2, r3, r12}
    ldmia r0!, {r2, r3, r12}
    add r0, r0, #4
    stmia r1!, {r2, r3, r12}
    ldmia r0!, {r2, r3, r12}
    add r0, r0, #4
    stmia r1!, {r2, r3, r12}
    bx lr
}
#endif
// clang-format on
