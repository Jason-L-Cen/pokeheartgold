#include <nitro.h>

#ifdef HEARTGOLD_ABI32
u32 OS_GetDTCMAddress(void) {
    return (u32)(uaddr)OS_GetInitArenaLo(OS_ARENA_DTCM);
}
#else
asm u32 OS_GetDTCMAddress(void) {
    mrc p15, 0, r0, c9, c1, 0
    ldr r1, =0xFFFFF000
    and r0, r0, r1
    bx lr
}
#endif
