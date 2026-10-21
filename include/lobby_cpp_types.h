#ifndef POKEHEARTGOLD_LOBBY_CPP_TYPES_H
#define POKEHEARTGOLD_LOBBY_CPP_TYPES_H

// Lobby C++: the small value types the vector and string code passes around.

#include <nitro/types.h>

// The empty tags the byte vector's range assignment passes on.
struct AssignTag {};

struct IteratorTag {};

struct RangeTag {};

struct TrueTag {};

// An iterator over a byte range.
struct ByteIterator {
    const u8 *p;
    u32 extra;

    ByteIterator(const u8 *q)
        : p(q) {
    }
};

// The allocator the C-style vector functions take by value (one unused byte).
struct ByteAllocator {
    u8 unused;
};

#endif // POKEHEARTGOLD_LOBBY_CPP_TYPES_H
