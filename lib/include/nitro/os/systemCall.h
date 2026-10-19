#ifndef NITRO_OS_SYSTEMCALL_H_
#define NITRO_OS_SYSTEMCALL_H_

#include <nitro/types.h>

void SVC_Halt(void);
void SVC_CpuSet(const void *source, void *destination, u32 control);
u16 SVC_GetCRC16(u16 seed, const void *data, u32 size);
void SVC_WaitByLoop(s32 count);

#endif //NITRO_OS_SYSTEMCALL_H_
