#ifndef NITRO_OS_RESET_H
#define NITRO_OS_RESET_H

#include <nitro/types.h>
#ifdef HEARTGOLD_ABI32
#include <nitro/os/native_reset.h>
#endif
#include <nitro/os/common/reset_shared.h>
#include <nitro/pxi/fifo.h>

void OS_InitReset(void);
BOOL OS_IsResetOccurred(void);
void OSi_CommonCallback(PXIFifoTag tag, u32 data, BOOL err);
void OSi_SendToPxi(u16 data);

#ifdef SDK_ARM9
void OS_ResetSystem(u32 parameter);

static inline u32 OS_GetResetParameter(void) {
#ifdef HEARTGOLD_ABI32
    return HGNativeResetControl(HG_RESET_PARAMETER,0);
#else
    return *(u32 *)HW_RESET_PARAMETER_BUF;
#endif
}
#else
void OS_ResetSystem(void);
#endif

#endif //NITRO_OS_RESET_H
