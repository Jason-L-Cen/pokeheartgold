#include "global.h"

// The code below is compiled as ARM (32-bit) code.
#include <nitro/code32.h>
#pragma force_active on
void ov13_02242478(int);
void ov13_02241B70(void);
#pragma force_active off

static inline volatile u32 *FlagBase(void) {
    return (volatile u32 *)0x027E0000;
}

static inline void SetFlag(volatile u32 *p) {
    u32 old = *p;
    *p = old | 1;
}

#pragma force_active on

void ov13_02241B70(void) {
    ov13_02242478(1);
    {
        volatile u32 *base = FlagBase();
        base += 0x3000 / 4;
        {
            volatile u32 *flags = base + 0xff8 / 4;
            *flags |= 1;
        }
    }
}
