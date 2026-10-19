#include <nitro/os/context.h>
#include <nitro/os/interrupt.h>
#include <nitro/os/irqHandler.h>
#include <nitro/os/irqTable.h>
#include <nitro/os/system.h>
#include <nitro/os/thread.h>
#ifdef HEARTGOLD_ABI32
#include <nitro/os/native_irq.h>
#endif

extern OSThreadQueue OSi_IrqThreadQueue;

void OS_WaitIrq(BOOL clear, OSIrqMask irqFlags) {
#ifdef HEARTGOLD_ABI32
    HGNativeIrqControl(clear ? HG_IRQ_WAIT_CLEAR : HG_IRQ_WAIT,irqFlags);
#else
    OSIntrMode enabled = OS_DisableInterrupts();
    if (clear) {
        OS_ClearIrqCheckFlag(irqFlags);
    }
    OS_RestoreInterrupts(enabled);
    while (!(OS_GetIrqCheckFlag() & irqFlags)) {
        OS_SleepThread(&OSi_IrqThreadQueue);
    }
#endif
}
