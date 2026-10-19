/*
 * NOTE:
 * This file is shared between ARM9 and ARM7
 * DO NOT PUT PROC SPECIFIC CODE IN HERE
 * Thank You!
 */

/*
 * DO NOT INCLUDE THIS FILE DIRECTLY
 * Include OS_interrupt.h from the specific proc's lib
 */

#ifndef POKEDIAMOND_OS_INTERRUPT_SHARED_H
#define POKEDIAMOND_OS_INTERRUPT_SHARED_H

#ifdef HEARTGOLD_NATIVE
#include <nitro/os/common/interrupt_shared.h>
#else
#include "nitro/types.h"

typedef u32 OSIrqMask;

typedef void (*OSIrqFunction) (void);

typedef struct
{
    void (*func) (void *);
    u32 enable;
    void* arg;
} OSIrqCallbackInfo;

#endif // HEARTGOLD_NATIVE
#endif //POKEDIAMOND_OS_INTERRUPT_SHARED_H
