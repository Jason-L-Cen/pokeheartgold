#include <nitro/os/interrupt.h>
#include <nitro/os/system.h>
#include <nitro/os/thread.h>
#include <nitro/os/irqTable.h>
#include <nitro/os/irqHandler.h>
#include <nitro/os/context.h>

extern OSThreadQueue OSi_IrqThreadQueue;

void OS_WaitIrq(BOOL clear, OSIrqMask irqFlags) {
    OSIntrMode enabled = OS_DisableInterrupts();
    if (clear) {
        OS_ClearIrqCheckFlag(irqFlags);
    }
    OS_RestoreInterrupts(enabled);
    while (!(OS_GetIrqCheckFlag() & irqFlags)) {
        OS_SleepThread(&OSi_IrqThreadQueue);
    }
}
