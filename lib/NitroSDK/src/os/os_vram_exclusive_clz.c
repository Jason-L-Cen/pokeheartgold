#pragma force_active on
#include <nitro/types.h>
u32 OsCountZeroBits(register u32 bitmap);

// The code below is compiled as ARM (32-bit) code.
#include <nitro/code32.h>

// clang-format off
#ifdef HEARTGOLD_NATIVE
u32 OsCountZeroBits(register u32 bitmap) {
    return bitmap ? __builtin_clz(bitmap) : 32;
}
#else
asm u32 OsCountZeroBits(register u32 bitmap) {
    clz r0, r0
    bx lr
}
#endif
// clang-format on
