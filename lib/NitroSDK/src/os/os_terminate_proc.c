#include <nitro.h>

#ifdef SDK_ARM9
#ifdef HEARTGOLD_ABI32
#include <nitro/os/native_irq.h>
void OS_Halt(void) { HGNativeIrqControl(HG_IRQ_HALT,0); }
#elif defined(HEARTGOLD_NATIVE)
#include "../../../../ios/Original/NativeEvents.h"
void OS_Halt(void) { HGNativeHalt(); }
#else
asm void OS_Halt(void) {
    mov r0, #0
    mcr p15, 0, r0, c7, c0, 4
    bx lr
}
#endif
#endif
