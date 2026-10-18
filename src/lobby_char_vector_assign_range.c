#include <stdarg.h>

#include "global.h"

// The code below is compiled as ARM (32-bit) code.
#include <nitro/code32.h>

typedef struct {
    u8 *data;
    u32 len, cap;
} Vec;

typedef struct {
    u32 a, b;
} Pair;

typedef struct {
    Pair first, second;
    u32 e, f;
} Six;

typedef struct {
    u32 type;
    Six value;
} Record;

typedef struct { u8 byte; } Allocator;
extern u32 LobbyCharVector_RecommendSize(Vec *, u32);
extern void LobbyCharVector_Reallocate(Vec *, Allocator, u32);

typedef struct {
    u8 *p;
    u32 extra;
} Iterator;

void LobbyCharVector_AssignRange(Vec *, u8 *, u8 *, Allocator);

void LobbyCharVector_AssignRange(Vec *v, u8 *first, u8 *last, Allocator unused) {
    u32 len = last - first;
    if (len > v->cap) {
        volatile u8 tag = 0;
        Allocator allocator = { 0 };
        LobbyCharVector_Reallocate(v, allocator, LobbyCharVector_RecommendSize(v, len - v->cap));
    }
    memmove(v->data, first, (u32)last - (u32)first);
    v->len = len;
}
