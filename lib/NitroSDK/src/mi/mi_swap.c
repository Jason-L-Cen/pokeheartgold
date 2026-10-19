#pragma force_active on
#include <nitro/types.h>
u32 MI_SwapWord(register u32 setData, register vu32 *destp);

// The code below is compiled as ARM (32-bit) code.
#include <nitro/code32.h>

#ifdef HEARTGOLD_ABI32
// The generated-C path imports the native atomic operation; unshared wasm2c memory is not atomic.
extern u32 HGNativeAtomicSwapWord(u32 setData, vu32 *destp)
    __attribute__((import_module("env"), import_name("HGNativeAtomicSwapWord")));
u32 MI_SwapWord(u32 setData, vu32 *destp) {
    return HGNativeAtomicSwapWord(setData, destp);
}
#elif defined(HEARTGOLD_NATIVE)
u32 MI_SwapWord(u32 setData, vu32 *destp) {
    return __atomic_exchange_n(destp, setData, __ATOMIC_SEQ_CST);
}
#else
// clang-format off
asm u32 MI_SwapWord(register u32 setData, register vu32 *destp) {
    swp r0, r0, [r1]
    bx lr
}
// clang-format on
#endif
