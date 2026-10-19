// SPDX-License-Identifier: GPL-3.0-or-later
#include <nitro.h>

#define CHECK(value) do { if (!(value)) return __LINE__; } while (0)
s32 HGGetLockID(void) { return OS_GetLockID(); }
void HGReleaseLockID(u32 id) { OS_ReleaseLockID((u16)id); }
u32 CheckOriginalLockIDs(void) {
    s32 ids[48];
    for (u32 i=0;i<48;i++) { ids[i]=OS_GetLockID(); CHECK(ids[i]==OS_MAINP_LOCK_ID_START+(s32)i); }
    CHECK(OS_GetLockID()==OS_LOCK_ID_ERROR);
    OS_ReleaseLockID((u16)ids[7]); CHECK(OS_GetLockID()==ids[7]);
    for (u32 i=0;i<48;i++) OS_ReleaseLockID((u16)ids[i]);
    s32 first=OS_GetLockID(); CHECK(first==OS_MAINP_LOCK_ID_START); OS_ReleaseLockID((u16)first);
    return 0;
}
