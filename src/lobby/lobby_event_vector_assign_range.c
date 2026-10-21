#include "global.h"

#include <nitro/code32.h>

#include "ppw_lobby_work.h"

#define W(p, n) (*(u32 *)((u8 *)(p) + (n)))
#define P(p, n) (*(void **)((u8 *)(p) + (n)))

typedef struct {
    int a, b;
} Pair;

typedef struct {
    Pair *data;
    u32 size, capacity;
} Vec;

extern u32 gLobbySortEmptyBuffer[];
extern char _ZSt7nothrow[];
void *_ZnamRKSt9nothrow_t(u32, void *);
void LobbySort_GetTemporaryBuffer(void *, int);
u32 LobbyEventVector_RecommendSize(Vec *, u32);
void LobbyEventVector_AssignRange(Vec *, Pair *, Pair *);
void LobbyEventVector_Reallocate(Vec *, u32, u32);

void LobbySort_GetTemporaryBuffer(void *p, int n) {
    P(p, 0) = 0;
    W(p, 4) = gLobbySortEmptyBuffer[1];
    while (n > 0) {
        P(p, 0) = _ZnamRKSt9nothrow_t(n * 8, _ZSt7nothrow);
        if (P(p, 0)) {
            W(p, 4) = n;
            return;
        }
        n /= 2;
    }
}

void LobbyEventVector_AssignRange(Vec *p, Pair *first, Pair *last) {
    u32 n = last - first, growth;
    Pair *end, *dest;
    volatile u8 a, c, d, b;
    if (p->capacity < n) {
        a = 0;
        growth = LobbyEventVector_RecommendSize(p, n - p->capacity);
    } else {
        end = first;
        end += *(p->size < n ? &p->size : &n);
        dest = p->data;
        while (first < end) {
            dest->a = first->a;
            dest->b = first->b;
            ++first;
            ++dest;
        }
        if (n < p->size) {
            u32 old = p->size;
            c = 0;
            p->size = *(volatile u32 *)&p->size - (old - n);
            return;
        }
        if (p->size >= n) {
            return;
        }
        d = 0;
        dest = *(Pair *volatile *)&p->data + *(volatile u32 *)&p->size;
        while (end != last) {
            if (dest) {
                *dest = *end;
            }
            ++p->size;
            ++end;
            ++dest;
        }
        return;
    }
    LobbyEventVector_Reallocate(p, n, growth);
    b = 0;
    dest = p->data + *(volatile u32 *)&p->size;
    for (; first != last; ++first, ++dest) {
        if (dest) {
            *dest = *first;
        }
        ++p->size;
    }
}

// A schedule record is at least its header and holds eventCount events.
BOOL PPWLobbySchedule_IsSizeValid(PPWLobbySchedule *schedule, const void *data, u32 size) {
    const PPWLobbyScheduleRecord *record = data;
    if (size < offsetof(PPWLobbyScheduleRecord, events)) {
        return FALSE;
    }
    return size >= (record->eventCount - 1) * sizeof(LobbyEvent) + sizeof(PPWLobbyScheduleRecord);
}
