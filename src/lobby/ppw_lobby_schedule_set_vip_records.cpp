#include <nitro/types.h>

#include <nitro/code32.h>

#include "ppw_lobby_work.h"

// The allocator tag the range assignment takes by value.
struct RangeTag {};

extern "C" void LobbyVipVector_AssignRange(PPWLobbyVipVector *, const PPWLobbyVipRecord *, const PPWLobbyVipRecord *, RangeTag);

extern "C" BOOL PPWLobbySchedule_SetVipRecords(PPWLobbySchedule *schedule, const void *data, u32 size) {
    PPWLobbySchedule_ValidateVipRecords(schedule, data, size);
    const PPWLobbyVipRecord *first = (const PPWLobbyVipRecord *)data;
    LobbyVipVector_AssignRange(&schedule->vips, first, first + size / sizeof(PPWLobbyVipRecord), RangeTag());
    return TRUE;
}
