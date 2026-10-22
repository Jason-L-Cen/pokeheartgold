#include "global.h"

#include <nitro/code32.h>

#include "ppw_lobby_work.h"

u32 LobbyVipVector_RecommendSize(PPWLobbyVipVector *, u32);
void LobbyVipVector_AssignRange(PPWLobbyVipVector *, PPWLobbyVipRecord *, PPWLobbyVipRecord *);
void LobbyVipVector_Reallocate(PPWLobbyVipVector *, u32, u32);

void LobbyVipVector_AssignRange(PPWLobbyVipVector *p, PPWLobbyVipRecord *first, PPWLobbyVipRecord *last) {
    u32 n = last - first, growth;
    PPWLobbyVipRecord *end, *dest;
    volatile u8 a, c, d, b;
    if (p->capacity < n) {
        a = 0;
        growth = LobbyVipVector_RecommendSize(p, n - p->capacity);
    } else {
        end = first;
        end += *(p->size < n ? &p->size : &n);
        dest = p->data;
        while (first < end) {
            dest->id = first->id;
            dest->value = first->value;
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
        dest = *(PPWLobbyVipRecord *volatile *)&p->data + *(volatile u32 *)&p->size;
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
    LobbyVipVector_Reallocate(p, n, growth);
    b = 0;
    dest = p->data + *(volatile u32 *)&p->size;
    for (; first != last; ++first, ++dest) {
        if (dest) {
            *dest = *first;
        }
        ++p->size;
    }
}
