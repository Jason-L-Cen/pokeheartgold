#include <nitro/types.h>

#include <nitro/code32.h>

#include "lobby_mem.h"

#include "lobby_chat_manager.h"
#include "ppw_lobby_state.h"

extern "C" unsigned long strlen(const char *);
extern "C" void MI_CpuFill8(void *, u8, u32);

extern "C" u32 gLobbyChatNextRequestId;


#ifdef HEARTGOLD_NATIVE
extern "C" char gLobbyStr_Empty_17[];
extern "C" Str *LobbyChat_GetFullChannelName(Str *out, LobbyChatManager *m, Str *s);
#else
extern "C" Str LobbyChat_GetFullChannelName(LobbyChatManager *m, const Str &s);
#endif

extern "C" int LobbyChat_AddChannel(LobbyChatManager *m, const Str &s, LobbyChannelCallbacks *callbacks, u32 limit);

struct Callbacks {
    void (*message)();
    void (*kicked)();
    void (*userJoined)();
    void (*userLeft)();
    void (*nickChanged)();
    void (*topicChanged)();
    void (*modeChanged)();
    void (*userModeChanged)();
    void (*usersChanged)();
    void (*unk_24)();
    void (*broadcastKeyChanged)();
    void *param;
};

extern "C" void chatEnterChannelA(void *chat, const char *channel, u32 password, Callbacks *callbacks, void (*callback)(), u32 param, u32 blocking);
#ifndef HEARTGOLD_NATIVE
extern "C" void LobbyChat_OnChannelMessage();
extern "C" void LobbyChat_OnKicked();
extern "C" void LobbyChat_OnUserJoined();
extern "C" void LobbyChat_OnUserParted();
extern "C" void LobbyChat_OnUserChangedNick();
extern "C" void LobbyChat_OnUserModeChanged();
extern "C" void LobbyChat_OnTopicChanged();
extern "C" void LobbyChat_OnChannelModeChanged();
extern "C" void LobbyChat_OnUserListUpdated();
extern "C" void LobbyChat_OnEnterChannel();
extern "C" void LobbyChat_OnBroadcastKeyChanged();
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
        p->vtable = &gLobbyEnterChannelRequestVtable;
        p->callback = callback;
    }
    return p;
}

struct IdTag {};

inline u32 GetId(LobbyCallbackRequest *h, IdTag, IdTag) {
    return h->vtable->getId(h);
}

extern "C" int LobbyChat_EnterChannelAsync(const char *name, u32 password, u32 limit, LobbyChannelCallbacks *channelCallbacks, u32 callback, u32 param);
#pragma opt_common_subs off

extern "C" int LobbyChat_EnterChannelAsync(const char *name, u32 password, u32 limit, LobbyChannelCallbacks *channelCallbacks, u32 callback, u32 param) {
    Callbacks cb;
    LobbyChatManager *m = gLobbyChatManager;
    if (m == NULL) {
        return PPW_LOBBY_RESULT_ERROR_STATE;
    }
    if (m != NULL && m->state == 5) {
        return PPW_LOBBY_RESULT_ERROR_CONDITION;
    }
    if (m != NULL && GetMgr()->state != 3) {
        return PPW_LOBBY_RESULT_ERROR_STATE;
    }
    strlen(name);
    LobbyRequestMapInsertResult res;
    LobbyCallbackRequest *h = NewHandler(callback, param);
    LobbyRequestMap_Insert(&res, &m->requestMap, &h);
    u32 id = GetId(h, IdTag(), IdTag());
    {
        Str s;
        LobbyString_InitFromCStr(&s, name);
#ifdef HEARTGOLD_NATIVE
        Str t;
        LobbyString_InitFromCStr(&t, gLobbyStr_Empty_17);
        LobbyChat_GetFullChannelName(&t, GetMgr(), &s);
#else
        Str t = LobbyChat_GetFullChannelName(GetMgr(), s);
#endif
        Str u;
        LobbyString_InitFromCStr(&u, t.c_str());
        LobbyChat_AddChannel(GetMgr(), u, channelCallbacks, limit);
        LobbyString_Destroy(&u);
        LobbyString_Destroy(&t);
        LobbyString_Destroy(&s);
    }
    MI_CpuFill8(&cb, 0, sizeof(cb));
    // The SDK copies these raw slots and invokes each with its registered callback signature.
    cb.message = reinterpret_cast<void (*)()>(LobbyChat_OnChannelMessage);
    cb.modeChanged = reinterpret_cast<void (*)()>(LobbyChat_OnChannelModeChanged);
    cb.kicked = reinterpret_cast<void (*)()>(LobbyChat_OnKicked);
    cb.topicChanged = reinterpret_cast<void (*)()>(LobbyChat_OnTopicChanged);
    cb.userLeft = reinterpret_cast<void (*)()>(LobbyChat_OnUserParted);
    cb.userJoined = reinterpret_cast<void (*)()>(LobbyChat_OnUserJoined);
    cb.usersChanged = reinterpret_cast<void (*)()>(LobbyChat_OnUserListUpdated);
    cb.userModeChanged = reinterpret_cast<void (*)()>(LobbyChat_OnUserModeChanged);
    cb.nickChanged = reinterpret_cast<void (*)()>(LobbyChat_OnUserChangedNick);
    cb.broadcastKeyChanged = reinterpret_cast<void (*)()>(LobbyChat_OnBroadcastKeyChanged);
    cb.param = channelCallbacks->param;
    Str s2;
    LobbyString_InitFromCStr(&s2, name);
#ifdef HEARTGOLD_NATIVE
    Str t2;
    LobbyString_InitFromCStr(&t2, gLobbyStr_Empty_17);
    LobbyChat_GetFullChannelName(&t2, GetMgr(), &s2);
#else
    Str t2 = LobbyChat_GetFullChannelName(GetMgr(), s2);
#endif
    chatEnterChannelA(GetMgr()->connection, t2.c_str(), password, &cb, reinterpret_cast<void (*)()>(LobbyChat_OnEnterChannel), id, 0);
    LobbyString_Destroy(&t2);
    LobbyString_Destroy(&s2);
    return PPW_LOBBY_RESULT_SUCCESS;
}

#pragma opt_common_subs reset
