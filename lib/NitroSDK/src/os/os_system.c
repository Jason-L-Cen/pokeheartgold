#ifdef HEARTGOLD_ABI32
#include <nitro/os/system.h>
extern OSIntrMode HGNativeEnableInterrupts(void) __attribute__((import_module("env"),import_name("HGNativeEnableInterrupts")));
extern OSIntrMode HGNativeDisableInterrupts(void) __attribute__((import_module("env"),import_name("HGNativeDisableInterrupts")));
extern OSIntrMode HGNativeRestoreInterrupts(OSIntrMode) __attribute__((import_module("env"),import_name("HGNativeRestoreInterrupts")));
extern OSIntrMode HGNativeDisableInterrupts_IrqAndFiq(void) __attribute__((import_module("env"),import_name("HGNativeDisableInterrupts_IrqAndFiq")));
extern OSIntrMode HGNativeRestoreInterrupts_IrqAndFiq(OSIntrMode) __attribute__((import_module("env"),import_name("HGNativeRestoreInterrupts_IrqAndFiq")));
extern OSIntrMode_Irq HGNativeGetCpsrIrq(void) __attribute__((import_module("env"),import_name("HGNativeGetCpsrIrq")));
extern OSProcMode HGNativeGetProcMode(void) __attribute__((import_module("env"),import_name("HGNativeGetProcMode")));
extern void HGNativeSpinWait(u32) __attribute__((import_module("env"),import_name("HGNativeSpinWait")));
OSIntrMode OS_EnableInterrupts(void) { return HGNativeEnableInterrupts(); }
OSIntrMode OS_DisableInterrupts(void) { return HGNativeDisableInterrupts(); }
OSIntrMode OS_RestoreInterrupts(OSIntrMode state) { return HGNativeRestoreInterrupts(state); }
OSIntrMode OS_DisableInterrupts_IrqAndFiq(void) { return HGNativeDisableInterrupts_IrqAndFiq(); }
OSIntrMode OS_RestoreInterrupts_IrqAndFiq(OSIntrMode state) { return HGNativeRestoreInterrupts_IrqAndFiq(state); }
OSIntrMode_Irq OS_GetCpsrIrq(void) { return HGNativeGetCpsrIrq(); }
OSProcMode OS_GetProcMode(void) { return HGNativeGetProcMode(); }
void OS_SpinWait(u32 cycles) { HGNativeSpinWait(cycles); }
#else
#include <nitro.h>

asm OSIntrMode OS_EnableInterrupts(void) {
    mrs r0, cpsr
    bic r1, r0, #HW_PSR_IRQ_DISABLE
    msr cpsr_c, r1
    and r0, r0, #HW_PSR_IRQ_DISABLE
    bx lr
}

asm OSIntrMode OS_DisableInterrupts(void) {
    mrs r0, cpsr
    orr r1, r0, #HW_PSR_IRQ_DISABLE
    msr cpsr_c, r1
    and r0, r0, #HW_PSR_IRQ_DISABLE
    bx lr
}

asm OSIntrMode OS_RestoreInterrupts(register OSIntrMode prev) {
    mrs r1, cpsr
    bic r2, r1, #HW_PSR_IRQ_DISABLE
    orr r2, r2, r0
    msr cpsr_c, r2
    and r0, r1, #HW_PSR_IRQ_DISABLE
    bx lr
}

asm OSIntrMode OS_DisableInterrupts_IrqAndFiq(void) {
    mrs r0, cpsr
    orr r1, r0, #HW_PSR_IRQ_DISABLE | HW_PSR_FIQ_DISABLE
    msr cpsr_c, r1
    and r0, r0, #HW_PSR_IRQ_DISABLE | HW_PSR_FIQ_DISABLE
    bx lr
}

asm OSIntrMode OS_RestoreInterrupts_IrqAndFiq(register OSIntrMode prev) {
    mrs r1, cpsr
    bic r2, r1, #HW_PSR_IRQ_DISABLE | HW_PSR_FIQ_DISABLE
    orr r2, r2, r0
    msr cpsr_c, r2
    and r0, r1, #HW_PSR_IRQ_DISABLE | HW_PSR_FIQ_DISABLE
    bx lr
}

asm OSIntrMode_Irq OS_GetCpsrIrq(void) {
    mrs r0, cpsr
    and r0, r0, #HW_PSR_IRQ_DISABLE
    bx lr
}

asm OSProcMode OS_GetProcMode(void) {
    mrs r0, cpsr
    and r0, r0, #HW_PSR_CPU_MODE_MASK
    bx lr
}

#ifdef SDK_ARM9
#include <nitro/code32.h>
asm void OS_SpinWait(register u32 cycles) {
    @loop:
    subs r0, r0, #4
    bhs @loop
    bx lr
}
#include <nitro/codereset.h>
#else
void OS_SpinWait(u32 cycles) {
    SVC_WaitByLoop((s32)cycles / 4);
}
#endif
#endif
