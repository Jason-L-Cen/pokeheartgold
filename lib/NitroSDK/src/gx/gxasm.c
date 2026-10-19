#pragma force_active on
#include <nitro/types.h>
#ifdef HEARTGOLD_NATIVE
#include <nitro/gx/native_geometry.h>
#include "assert.h"
static void send(const void *source,void *destination,u32 count) {
    uaddr address=(uaddr)destination;
    GF_ASSERT(address>=0x04000400 && address<0x04000440 && address%4==0);
    BOOL success=HGNativeGeometryFifo(source,count); GF_ASSERT(success);
}
void GX_SendFifo48B(const void *source,void *destination) { send(source,destination,12); }
void GX_SendFifo64B(const void *source,void *destination) { send(source,destination,16); }
#else
void GX_SendFifo48B(register const void *pSrc, register void *pDest);
void GX_SendFifo64B(register const void *pSrc, register void *pDest);

// The code below is compiled as ARM (32-bit) code.
#include <nitro/code32.h>

// clang-format off
asm void GX_SendFifo48B(register const void *pSrc, register void *pDest) {
    ldmia r0!, {r2, r3, r12}
    stmia r1, {r2, r3, r12}
    ldmia r0!, {r2, r3, r12}
    stmia r1, {r2, r3, r12}
    ldmia r0!, {r2, r3, r12}
    stmia r1, {r2, r3, r12}
    ldmia r0!, {r2, r3, r12}
    stmia r1, {r2, r3, r12}
    bx lr
}
// clang-format on

// clang-format off
asm void GX_SendFifo64B(register const void *pSrc, register void *pDest) {
    stmdb sp!, {r4, r5, r6, r7, r8}
    ldmia r0!, {r2, r3, r4, r5, r6, r7, r8, r12}
    stmia r1, {r2, r3, r4, r5, r6, r7, r8, r12}
    ldmia r0!, {r2, r3, r4, r5, r6, r7, r8, r12}
    stmia r1, {r2, r3, r4, r5, r6, r7, r8, r12}
    ldmia sp!, {r4, r5, r6, r7, r8}
    bx lr
}
// clang-format on
#endif
