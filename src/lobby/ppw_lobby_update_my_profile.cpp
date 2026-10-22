#include <nitro/types.h>

#include "lobby_cpp_types.h"
#include "ppw_lobby_state.h"

#include <nitro/code32.h>
extern "C" int memcmp(const void *, const void *, u32);

struct Tag {
#ifdef HEARTGOLD_ABI32
    u8 hgAbiCarrier;
#endif
};

struct Alloc {
#ifdef HEARTGOLD_ABI32
    u8 hgAbiCarrier;
#endif
    Alloc() {
#ifdef HEARTGOLD_ABI32
        hgAbiCarrier = 0;
#endif
    }
};

struct Vec;
extern "C" Vec *LobbyCharVector_Destroy(Vec *v);
extern "C" void LobbyCharVector_AssignIterRange(Vec *v, ByteIterator first, ByteIterator last, RangeTag);
extern "C" void LobbyCharVector_InitRange(Vec *v, char *first, char *last, Alloc);
extern "C" void LobbyCharVector_AssignRange(Vec *v, char *first, char *last, Tag);
extern "C" const char *LobbyByteVector_Data(const Vec *v);

struct Vec {
    char *ptr;
    u32 size, cap;

    void Assign(ByteIterator first, ByteIterator last) {
        Assign(first, last, AssignTag(), IteratorTag());
    }

    void Assign(ByteIterator first, ByteIterator last, const AssignTag &, const IteratorTag &) {
        LobbyCharVector_AssignIterRange(this, first, last, RangeTag());
    }

    void Destroy() {
        LobbyCharVector_Destroy(this);
    }

    char *begin() const {
        return ptr;
    }

    char *end() const {
        return ptr + size;
    }

    const char *Data() const {
        return LobbyByteVector_Data(this);
    }
};

// Member cleanup also runs if byte-range construction throws.
struct VecStorage : Vec {
    ~VecStorage() { Destroy(); }
};

// Byte buffer built from a [first,last) range; destroyed by LobbyCharVector_Destroy.
struct Bytes {
    VecStorage v;

    Bytes(const u8 *first, const u8 *last) {
        v.ptr = 0;
        v.size = 0;
        v.cap = 0;
        v.Assign(ByteIterator(first), ByteIterator(last));
    }

    char *begin() const {
        return v.ptr;
    }

    char *end() const {
        return v.ptr + v.size;
    }
};

inline int Compare(const Vec &a, const Vec &b, u32 n) {
    const char *pa = a.Data();
    return memcmp(pa, b.Data(), n);
}

inline bool operator==(const Vec &a, const Vec &b) {
    u32 n = a.size;
    bool eq = false;
    if (n == b.size) {
        if (n == 0 || Compare(a, b, n) == 0) {
            eq = true;
        }
    }
    return eq;
}

// A user profile and whether it changed since it was last uploaded.
struct Field {
    Vec v;
    int dirty;

    Field() {
    }

    Field &Init(char *b, char *e, Alloc a) {
        v.ptr = 0;
        v.size = 0;
        v.cap = 0;
        Field *self = (Field *)(void *)this;
        LobbyCharVector_InitRange(&self->v, b, e, a);
        self->dirty = 1;
        return *self;
    }
};

// The PPWLobbyWork fields of the own player's user profile.
struct Work {
    int state;
    int error;
    u8 pad[0x110 - 8];
    Field userProfile;
    u8 pad2[0x12c - 0x120];
    int userProfileReceived;
};

extern "C" Work *gPPWLobbyWork;
extern "C" int PPW_LobbyUpdateMyProfile(const u8 *data, u32 len, int flag);

extern "C" int PPW_LobbyUpdateMyProfile(const u8 *data, u32 len, int flag) {
    Work *w = gPPWLobbyWork;
    if (w == 0) {
        return PPW_LOBBY_RESULT_ERROR_STATE;
    }
    if (w != 0 && w->state == PPW_LOBBY_STATE_ERROR) {
        return PPW_LOBBY_RESULT_ERROR_CONDITION;
    }
    if (w != 0 && (*(Work *volatile *)&gPPWLobbyWork)->state != PPW_LOBBY_STATE_READY) {
        return PPW_LOBBY_RESULT_ERROR_STATE;
    }
    Field f;
    Bytes raw(data, data + len);
    f.Init(raw.begin(), raw.end(), Alloc());
    const Field &nf = f;
    if (flag) {
        w->userProfile.dirty = 1;
    }
    if (!(w->userProfile.v == nf.v)) {
        if (&w->userProfile != &nf) {
            LobbyCharVector_AssignRange(&w->userProfile.v, nf.v.begin(), nf.v.end(), Tag());
        }
        w->userProfile.dirty = 1;
    }
    f.v.Destroy();
    w->userProfileReceived = 1;
    return PPW_LOBBY_RESULT_SUCCESS;
}
