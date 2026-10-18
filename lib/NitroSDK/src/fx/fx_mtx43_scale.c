#pragma force_active on
#include <nitro/fx/fx.h>
void MTX_Scale43_(register MtxFx43 *pDst, register fx32 x, register fx32 y, register fx32 z);

// The code below is compiled as Thumb code.
#include <nitro/code16.h>

// clang-format off
#ifdef HEARTGOLD_NATIVE
void MTX_Scale43_(MtxFx43 *pDst, fx32 x, fx32 y, fx32 z) {
    *pDst = (MtxFx43){.a = {x, 0, 0, 0, y, 0, 0, 0, z, 0, 0, 0}};
}
#else
asm void MTX_Scale43_(register MtxFx43 *pDst, register fx32 x, register fx32 y, register fx32 z) {
    stmia r0!, {r1}
    mov r1, #0
    str r3, [r0, #0x1c]
    mov r3, #0
    stmia r0!, {r1, r3}
    stmia r0!, {r1, r2, r3}
    mov r2, #0
    stmia r0!, {r1, r3}
    add r0, #4
    stmia r0!, {r1, r2, r3}
    bx lr
}
#endif
// clang-format on
