#include <nitro/types.h>

#include <nitro/code32.h>

#include "ppw_lobby_work.h"

struct VipAllocator {
    void destroy(PPWLobbyVipRecord *, u32) {
    }
};

// A VIP list answer is a whole number of records; an empty one clears the list.
extern "C" BOOL PPWLobbySchedule_ValidateVipRecords(PPWLobbySchedule *schedule, const void *data, u32 size) {
    if (size % sizeof(PPWLobbyVipRecord)) {
        return FALSE;
    }
    if (size != 0) {
        return TRUE;
    }
    u32 count = schedule->vips.size;
    VipAllocator allocator = VipAllocator();
    allocator.destroy(schedule->vips.data, count);
    schedule->vips.size -= count;
    return TRUE;
}
