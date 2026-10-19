// SPDX-License-Identifier: GPL-3.0-or-later
#include <nitro/types.h>
#include <nitro/os/cache.h>

void HGCacheOperation(u32 operation,void *address,u32 size) {
    switch (operation) {
    case 0: DC_InvalidateAll(); break;
    case 1: DC_StoreAll(); break;
    case 2: DC_FlushAll(); break;
    case 3: DC_InvalidateRange(address,size); break;
    case 4: DC_StoreRange(address,size); break;
    case 5: DC_FlushRange(address,size); break;
    case 6: DC_WaitWriteBufferEmpty(); break;
    case 7: IC_InvalidateAll(); break;
    case 8: IC_InvalidateRange(address,size); break;
    }
}
#ifndef HEARTGOLD_ABI32
#include "cache_threads.h"
static void NativeCacheFence(void *context,unsigned operation) {
    (void)context; HGCacheOperation(operation,NULL,0);
}
#endif
u32 CheckOriginalCache(void) {
    u8 bytes[96];
    for (u32 i=0;i<sizeof(bytes);i++) bytes[i]=(u8)(i*13);
    for (u32 operation=0;operation<9;operation++) {
        for (u32 offset=0;offset<32;offset++) {
            HGCacheOperation(operation,bytes+offset,0);
            HGCacheOperation(operation,bytes+offset,1);
            HGCacheOperation(operation,bytes+offset,64);
        }
        HGCacheOperation(operation,NULL,0);
    }
    for (u32 i=0;i<sizeof(bytes);i++) if (bytes[i]!=(u8)(i*13)) return __LINE__;
#ifndef HEARTGOLD_ABI32
    CheckCacheOrdering(NativeCacheFence,NULL);
#endif
    return 0;
}
