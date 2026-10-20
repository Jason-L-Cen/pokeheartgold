#include <nitro/types.h>

#include <nitro/code32.h>

#include "lobby_mem.h"

#include "lobby_chat_manager.h"
#include "ppw_lobby_state.h"


extern "C" const u32 gLobbyChatNextRequestId;


extern "C" Str LobbyChat_GetFullChannelName(LobbyChatManager *m, const Str &s);
extern "C" void chatGetChannelKeysA(void *chat, const char *channel, const char *nick, int count, const char **keys, void (*callback)(), u32 param, u32 blocking);
extern "C" void chatEnumChannelsA(void *chat, const char *filter, void (*callbackEach)(), void (*callbackAll)(), u32 param, u32 blocking);
extern "C" void chatEnumUsersA(void *chat, const char *channel, void (*callback)(), u32 param, u32 blocking);
extern "C" int chatInChannelA(void *chat, const char *channel);
extern "C" void LobbyChat_OnGetChannelKeys();
extern "C" void LobbyChat_OnEnumChannelsEach();
extern "C" void LobbyChat_OnEnumChannels();
extern "C" void LobbyChat_OnEnumUsers();
extern "C" unsigned long strlen(const char *);
extern "C" const char gLobbyStr_Empty_17[];
extern "C" Str LobbyChat_GetUserNick(LobbyChatManager *m, int idx);

inline bool StrEquals(const Str *s, const char *cstr) {
    return LobbyString_CompareCStr(s, cstr) == 0;
}

inline LobbyChatManager *GetMgr() {
    return gLobbyChatManager;
}

inline void BaseInit(LobbyRequest *h, u32 param) {
    h->vtable = &gLobbyRequestVtable;
    h->id = 0;
    h->param = param;
}

// The id counter is only changed here; it is declared const so that stores through the new handler cannot alias it.
inline u32 NextHandlerId() {
    u32 n = gLobbyChatNextRequestId;
    *(u32 *)&gLobbyChatNextRequestId = n + 1;
    return n;
}

inline LobbyCallbackRequest *NewHandler(LobbyRequestVtable *vtable, u32 callback, u32 param) {
    LobbyCallbackRequest *p = (LobbyCallbackRequest *)LobbyMem_Alloc(sizeof(LobbyCallbackRequest));
    if (p) {
        BaseInit(p, param);
        p->id = NextHandlerId();
        p->vtable = vtable;
        p->callback = callback;
    }
    return p;
}

extern "C" int LobbyChat_GetChannelKeysAsync(const char *name, int userId, const char *key, u32 callback, u32 param);
extern "C" int LobbyChat_EnumChannelsAsync(const char *name, u32 callback, u32 param);
extern "C" int LobbyChat_EnumUsersAsync(const char *name, u32 callback, u32 param);

struct IdTag {};

inline u32 GetId(LobbyCallbackRequest *h, IdTag, IdTag) {
    return h->vtable->getId(h);
}

#pragma opt_common_subs off

extern "C" int LobbyChat_GetChannelKeysAsync(const char *name, int userId, const char *key, u32 callback, u32 param) {
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
    if (strlen(key) + 1 > 20) {
        return PPW_LOBBY_RESULT_ERROR_PARAM;
    }
    Str nick;
    LobbyString_InitFromCStr(&nick, gLobbyStr_Empty_17);
    if (userId == -1) {
        LobbyString_AssignCStr(&nick, gLobbyStr_Empty_17);
    } else {
        Str tmp = LobbyChat_GetUserNick(GetMgr(), userId);
        LobbyString_Assign(&nick, &tmp);
        LobbyString_Destroy(&tmp);
        if (StrEquals(&nick, gLobbyStr_Empty_17)) {
            LobbyString_Destroy(&nick);
            return PPW_LOBBY_RESULT_ERROR_PARAM;
        }
    }
    m = GetMgr();
    LobbyRequestMapInsertResult res;
    LobbyCallbackRequest *h = NewHandler(&gLobbyGetChannelKeysRequestVtable, callback, param);
    LobbyRequestMap_Insert(&res, &m->requestMap, &h);
    u32 id = GetId(h, IdTag(), IdTag());
    Str s;
    LobbyString_InitFromCStr(&s, name);
    Str t = LobbyChat_GetFullChannelName(GetMgr(), s);
    chatGetChannelKeysA(GetMgr()->connection, t.c_str(), nick.c_str(), 1, &key, LobbyChat_OnGetChannelKeys, id, 0);
    LobbyString_Destroy(&t);
    LobbyString_Destroy(&s);
    LobbyString_Destroy(&nick);
    return PPW_LOBBY_RESULT_SUCCESS;
}

extern "C" int LobbyChat_EnumChannelsAsync(const char *name, u32 callback, u32 param) {
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
    LobbyCallbackRequest *h = NewHandler(&gLobbyEnumChannelsRequestVtable, callback, param);
    LobbyRequestMap_Insert(&res, &m->requestMap, &h);
    u32 id = GetId(h, IdTag(), IdTag());
    Str s;
    LobbyString_InitFromCStr(&s, name);
    Str t = LobbyChat_GetFullChannelName(GetMgr(), s);
    chatEnumChannelsA(GetMgr()->connection, t.c_str(), LobbyChat_OnEnumChannelsEach, LobbyChat_OnEnumChannels, id, 0);
    LobbyString_Destroy(&t);
    LobbyString_Destroy(&s);
    return PPW_LOBBY_RESULT_SUCCESS;
}

extern "C" int LobbyChat_EnumUsersAsync(const char *name, u32 callback, u32 param) {
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
    {
        Str s;
        LobbyString_InitFromCStr(&s, name);
        Str t = LobbyChat_GetFullChannelName(GetMgr(), s);
        BOOL exists = chatInChannelA(GetMgr()->connection, t.c_str()) == 0;
        LobbyString_Destroy(&t);
        LobbyString_Destroy(&s);
        if (exists) {
            return PPW_LOBBY_RESULT_ERROR_CHANNEL;
        }
    }
    m = GetMgr();
    LobbyRequestMapInsertResult res;
    LobbyCallbackRequest *h = NewHandler(&gLobbyEnumUsersRequestVtable, callback, param);
    LobbyRequestMap_Insert(&res, &m->requestMap, &h);
    u32 id = GetId(h, IdTag(), IdTag());
    Str s;
    LobbyString_InitFromCStr(&s, name);
    Str t = LobbyChat_GetFullChannelName(GetMgr(), s);
    chatEnumUsersA(GetMgr()->connection, t.c_str(), LobbyChat_OnEnumUsers, id, 0);
    LobbyString_Destroy(&t);
    LobbyString_Destroy(&s);
    return PPW_LOBBY_RESULT_SUCCESS;
}

#pragma opt_common_subs reset
