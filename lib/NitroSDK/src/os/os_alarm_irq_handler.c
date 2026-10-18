#pragma force_active on
#include <nitro/types.h>

void OSi_ArrangeTimer(void);
void OSi_AlarmHandler(void *arg);

// The code below is compiled as ARM (32-bit) code.
#include <nitro/code32.h>

// clang-format off
#ifdef HEARTGOLD_NATIVE
void OSi_AlarmHandler(void *arg) {
    (void)arg;
    OSi_ArrangeTimer();
}
#else
asm void OSi_AlarmHandler(void *arg) {
    stmfd sp!, {r0, lr}
    bl OSi_ArrangeTimer
    ldmfd sp!, {r0, lr}
    bx lr
}
#endif
// clang-format on
