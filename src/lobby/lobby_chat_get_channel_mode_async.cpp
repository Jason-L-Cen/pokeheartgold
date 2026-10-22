#include <nitro/types.h>

#include <nitro/code32.h>

#include "lobby_mem.h"

#include "lobby_chat_manager.h"
#include "ppw_lobby_state.h"


extern "C" u32 gLobbyChatNextRequestId;


#ifdef HEARTGOLD_NATIVE
extern "C" char gLobbyStr_Empty_17[];
extern "C" Str *LobbyChat_GetFullChannelName(Str *out, LobbyChatManager *m, Str *s);
extern "C" void chatGetChannelModeA(void *chat, const char *channel, void (*callback)(void *, int, const char *, u32, u32), u32 param, u32 blocking);
#else
extern "C" Str LobbyChat_GetFullChannelName(LobbyChatManager *m, const Str &s);
extern "C" void chatGetChannelModeA(void *chat, const char *channel, void (*callback)(), u32 param, u32 blocking);
extern "C" void LobbyChat_OnGetChannelMode();
#endif

inline LobbyChatManager *GetMgr() {
    return gLobbyChatManager;
}

inline void BaseInit(LobbyRequest *h, u32 param) {
    h->vtable = &gLobbyRequestVtable;
    h->id = 0;
    h->param = param;
}

inline u32 NextHandlerId() {
    u32 n = gLobbyChatNextRequestId;
    gLobbyChatNextRequestId = n + 1;
    return n;
}

inline LobbyCallbackRequest *NewHandler(u32 callback, u32 param) {
    LobbyCallbackRequest *p = (LobbyCallbackRequest *)LobbyMem_Alloc(sizeof(LobbyCallbackRequest));
    if (p) {
        BaseInit(p, param);
        p->id = NextHandlerId();
        p->vtable = &gLobbyGetChannelModeRequestVtable;
        p->callback = callback;
    }
    return p;
}

extern "C" int LobbyChat_GetChannelModeAsync(const char *name, u32 callback, u32 param);

struct IdTag {};

inline u32 GetId(LobbyCallbackRequest *h, IdTag, IdTag) {
    return h->vtable->getId(h);
}

#pragma opt_common_subs off

extern "C" int LobbyChat_GetChannelModeAsync(const char *name, u32 callback, u32 param) {
    LobbyChatManager *m = gLobbyChatManager;
    if (m == NULL) {
        return PPW_LOBBY_RESULT_ERROR_STATE;
    }
    if (m != NULL && m->state == LOBBY_CHAT_STATE_ERROR) {
        return PPW_LOBBY_RESULT_ERROR_CONDITION;
    }
    if (m != NULL && GetMgr()->state != LOBBY_CHAT_STATE_READY) {
        return PPW_LOBBY_RESULT_ERROR_STATE;
    }
    LobbyRequestMapInsertResult res;
    LobbyCallbackRequest *h;
    h = NewHandler(callback, param);
    LobbyRequestMap_Insert(&res, &m->requestMap, &h);
    u32 id = GetId(h, IdTag(), IdTag());
    Str s;
    LobbyString_InitFromCStr(&s, name);
#ifdef HEARTGOLD_NATIVE
    Str t;
    LobbyString_InitFromCStr(&t, gLobbyStr_Empty_17);
    LobbyChat_GetFullChannelName(&t, GetMgr(), &s);
#else
    Str t = LobbyChat_GetFullChannelName(GetMgr(), s);
#endif
    chatGetChannelModeA(GetMgr()->connection, t.c_str(), LobbyChat_OnGetChannelMode, id, 0);
    LobbyString_Destroy(&t);
    LobbyString_Destroy(&s);
    return PPW_LOBBY_RESULT_SUCCESS;
}

#pragma opt_common_subs reset
