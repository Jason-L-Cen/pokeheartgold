#ifndef POKEHEARTGOLD_LOBBY_CPP_TYPES_H
#define POKEHEARTGOLD_LOBBY_CPP_TYPES_H

// Lobby C++: the small value types the vector and string code passes around.

#include <nitro/types.h>

// The empty tags the byte vector's range assignment passes on.
// ABI32 carriers retain the original argument slots across C/C++ calls.
struct AssignTag {
#ifdef HEARTGOLD_ABI32
    u8 hgAbiCarrier;
#endif
};

struct IteratorTag {
#ifdef HEARTGOLD_ABI32
    u8 hgAbiCarrier;
#endif
};

struct RangeTag {
#ifdef HEARTGOLD_ABI32
    u8 hgAbiCarrier;
#endif
};

struct TrueTag {
#ifdef HEARTGOLD_ABI32
    u8 hgAbiCarrier;
#endif
};

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
