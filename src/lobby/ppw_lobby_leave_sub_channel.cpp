#include <nitro/types.h>

#include "ppw_lobby_state.h"

#include <nitro/code32.h>
extern "C" int memcmp(const void *, const void *, unsigned);
class Str;
extern "C" Str *LobbyString_Destroy(Str *);
extern "C" Str *LobbyString_Assign(Str *, const Str *);

class Str {
public:
    union {
        u32 words[3];

        struct {
            u32 allocated : 1, capacity : 31;
            u32 length;
            char *pointer;
        } heap;

        char bytes[12];
    } s;

    Str() {
        u32 i;
        u32 *p = s.words;
        for (i = 0; i < 3; i++) {
            p[i] = 0;
        }
    }

    void Destroy() {
        LobbyString_Destroy(this);
    }

    Str &operator=(const Str &o) {
        LobbyString_Assign(this, &o);
        return *this;
    }

    const char *text() const {
        return s.heap.allocated == 0 ? s.bytes + 1 : s.heap.pointer;
    }
};

struct InfoHead {
    int id;
};

struct Info : InfoHead {
    u64 mid;
    int z;

    Info() {
    }

    Info(const Info &o)
        : InfoHead(o) {
        *this = o;
    }

    bool operator==(const Info &o) const {
        return memcmp(this, &o, sizeof(Info)) == 0;
    }
};

struct Entry {
    Info info;
    int valid;

    Entry(const Info &i)
        : info(i)
        , valid(1) {
    }

    void Assign(const Info &i) {
        info = i;
        valid = 1;
    }
};

struct Peer {
    int kind;
    Str name;
    int a;
    int b;

    BOOL IsValid() const {
        if (kind == 5) {
            return FALSE;
        }
        return TRUE;
    }

    void Reset() {
        Str empty;
        kind = 5;
        name = empty;
        a = 0;
        b = 0;
        empty.Destroy();
    }
};

struct Work {
    int state;
    int error;
    u8 pad8[0xd0 - 0x8];
    Peer peer;
    u8 pade8[0xfc - 0xe8];
    Entry entry;
    u8 pad110[0x128 - 0x110];
    int dirty;
};

extern "C" Work *gPPWLobbyWork;
extern "C" void PPWLobbyChannel_GetName(Str *, Peer *);
extern "C" int LobbyChat_LeaveChannel(const char *);
extern "C" int PPW_LobbyLeaveSubChannel(void);

inline Work *GetWork() {
    return gPPWLobbyWork;
}

inline bool IsError(Work *w) {
    return w != 0 && w->state == PPW_LOBBY_STATE_ERROR;
}

inline void Refresh(Work *w, Info &info) {
    Work *ww = w;
    ww->entry.valid = 0;
    info = ww->entry.info;
    info.id = 5;
    Entry e(info);
    const Info &ni = e.info;
    if (!(ww->entry.info == ni)) {
        ww->entry.Assign(ni);
    }
    ww->dirty = 1;
}

#pragma opt_common_subs off

extern "C" int PPW_LobbyLeaveSubChannel(void) {
    Info info;
    Work *w = GetWork();
    if (w == 0) {
        return PPW_LOBBY_RESULT_ERROR_STATE;
    }
    if (IsError(w)) {
        return PPW_LOBBY_RESULT_ERROR_CONDITION;
    }
    if (w != 0 && GetWork()->state != PPW_LOBBY_STATE_READY) {
        return PPW_LOBBY_RESULT_ERROR_STATE;
    }
    if (!(w != 0 && w->peer.IsValid())) {
        return PPW_LOBBY_RESULT_ERROR_STATE;
    }
    {
        Str t;
        PPWLobbyChannel_GetName(&t, &w->peer);
        int parsed;
        int error;
        parsed = LobbyChat_LeaveChannel(t.text());
        w = GetWork();
        error = PPWLobbyErrorMap().Map(parsed);
        if (error != PPW_LOBBY_ERROR_NONE) {
            w->error = error;
            w->state = PPW_LOBBY_STATE_ERROR;
        }
        t.Destroy();
    }
    w = GetWork();
    w->peer.Reset();
    w = GetWork();
    Refresh(w, info);
    return PPW_LOBBY_RESULT_SUCCESS;
}

#pragma opt_common_subs reset
