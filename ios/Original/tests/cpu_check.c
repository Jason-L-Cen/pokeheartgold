// SPDX-License-Identifier: GPL-3.0-or-later
#include <nitro/os/system.h>
#define CHECK(v) do { if (!(v)) return __LINE__; } while (0)

static struct { u32 before,value,after; } counter={0x12345678,0,0x87654321};
u32 HGCPUAction(u32 operation,u32 value) {
    switch (operation) {
    case 0: return OS_EnableInterrupts();
    case 1: return OS_DisableInterrupts();
    case 2: return OS_RestoreInterrupts((OSIntrMode)value);
    case 3: return OS_DisableInterrupts_IrqAndFiq();
    case 4: return OS_RestoreInterrupts_IrqAndFiq((OSIntrMode)value);
    case 5: return OS_GetCpsrIrq();
    case 6: return OS_GetProcMode();
    case 7: OS_SpinWait(value); return 0;
    }
    return ~0u;
}
u32 HGCPUIncrement(u32 count) {
    for (u32 i=0;i<count;i++) {
        OSIntrMode previous=OS_DisableInterrupts(); counter.value++; OS_RestoreInterrupts(previous);
    }
    return count;
}
u32 HGCPUCounter(u32 field) { return field==0 ? counter.value : counter.before==0x12345678 && counter.after==0x87654321; }
u32 CheckOriginalCPU(void) {
    CHECK(OS_GetCpsrIrq()==0 && OS_GetProcMode()==OS_PROCMODE_SYS);
    OSIntrMode outer=OS_DisableInterrupts(); CHECK(outer==0);
    OSIntrMode inner=OS_DisableInterrupts(); CHECK(inner==OS_INTRMODE_DISABLE_IRQ);
    CHECK(OS_RestoreInterrupts(inner)==OS_INTRMODE_DISABLE_IRQ && OS_GetCpsrIrq()==OS_INTRMODE_IRQ_DISABLE);
    CHECK(OS_RestoreInterrupts(outer)==OS_INTRMODE_DISABLE_IRQ && OS_GetCpsrIrq()==0);
    CHECK(OS_DisableInterrupts_IrqAndFiq()==0);
    CHECK(OS_RestoreInterrupts(OS_INTRMODE_ENABLE)==OS_INTRMODE_DISABLE_IRQ && OS_GetCpsrIrq()==0);
    CHECK(OS_DisableInterrupts_IrqAndFiq()==OS_INTRMODE_DISABLE_FIQ);
    CHECK(OS_RestoreInterrupts_IrqAndFiq(OS_INTRMODE_DISABLE_IRQ)==(OS_INTRMODE_DISABLE_IRQ|OS_INTRMODE_DISABLE_FIQ));
    CHECK(OS_RestoreInterrupts_IrqAndFiq(OS_INTRMODE_ENABLE)==OS_INTRMODE_DISABLE_IRQ);
    CHECK(OS_EnableInterrupts()==0 && OS_GetCpsrIrq()==0);
    OS_SpinWait(0); OS_SpinWait(3); OS_SpinWait(4);
    counter.value=0;
    return 0;
}
