#include <nitro/types.h>

#include <nitro/code32.h>

#include "ppw_lobby_work.h"

struct EventIterator {
    LobbyEvent *p;

    EventIterator(LobbyEvent *q)
        : p(q) {
    }
};

class Tag {};

class Less {};

extern "C" void LobbyEventVector_AssignRange(LobbyEventVector *, const LobbyEvent *, const LobbyEvent *, Tag);
extern "C" void LobbySort_StableSort(EventIterator, EventIterator, Less);
extern "C" void LobbyEventDeque_Assign(LobbyEventDeque *, EventIterator, const EventIterator &, Tag);

inline EventIterator Begin(LobbyEventVector *events) {
    return EventIterator(events->data);
}

inline EventIterator End(LobbyEventVector *events) {
    return EventIterator(events->data + events->size);
}

inline void Sort(EventIterator first, EventIterator last, Less c) {
    LobbySort_StableSort(first, last, c);
}

inline void Assign(LobbyEventDeque *deque, EventIterator first, EventIterator last) {
    Tag t;
    LobbyEventDeque_Assign(deque, first, last, t);
}

// Takes over a schedule record from the server: its header, and its events sorted by time.
extern "C" BOOL PPWLobbySchedule_Load(PPWLobbySchedule *schedule, const void *data, u32 size) {
    const PPWLobbyScheduleRecord *record = (const PPWLobbyScheduleRecord *)data;
    Less c;
    if (!PPWLobbySchedule_IsSizeValid(schedule, data, size)) {
        return FALSE;
    }
    schedule->roomType = record->roomType;
    schedule->season = record->season;
    schedule->lockTime = record->lockTime;
    schedule->unk_20 = record->unk_04;
    schedule->roomFlag = record->roomFlag;
    {
        u32 n = record->eventCount;
        LobbyEventVector_AssignRange(&schedule->events, record->events, record->events + n, Tag());
    }
    Sort(Begin(&schedule->events), End(&schedule->events), c);
    Assign(&schedule->pendingEvents, Begin(&schedule->events), End(&schedule->events));
    return TRUE;
}
