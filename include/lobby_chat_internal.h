#ifndef POKEHEARTGOLD_LOBBY_CHAT_INTERNAL_H
#define POKEHEARTGOLD_LOBBY_CHAT_INTERNAL_H

// Lobby chat: the chat manager, its string, map and request types, and the functions shared
// between the chat source files.

#include "global.h"
#include "lobby_chat_callbacks.h"

#include "dwc/net_common_internal.h"

#include "lobby_chat_manager.h"
#include "lobby_mem.h"
#include "lobby_vector.h"
#include "ppw_lobby_state.h"

#include "../lib/NitroDWC/src/tree-iterator4.h"


static inline void StrInit(Str *s) {
    u32 i;
    u32 *p;
    for (i = 0, p = (u32 *)s; i < 3; i++) {
        p[i] = 0;
    }
}

static inline void PairInit(LobbyUserMapEntry *p, int *k, Str *v) {
    p->userId = *k;
    LobbyString_InitCopy(&p->nick, v);
}

void LobbyRequestMap_DeleteAll(LobbyRequestMap *map);

typedef Iterator Iter;

void LobbyUserIdSet_Find(Iter *out, LobbyUserIdSet *set, int *key);

static inline BOOL StrLess(Str *a, Str *b) {
    return LobbyString_Compare(a, b) < 0;
}

u32 current_time(void);
extern char gLobbyFmt_N_UnderscoreIntUnderscoreUint[];
extern char gLobbyStr___2[];
void LobbyChat_SplitString(LobbyStringVector *out, const Str *s, Str *sep, int n);
long atol(const char *s);
int LobbyChat_GetUserIdFromNick(const Str *s);

LobbyUserMapEntry *LobbyUserMap_GetOrInsert(LobbyUserMap *map, int *key);

Str *LobbyChat_GetFullChannelName(Str *out, LobbyChatManager *manager, Str *name);

void *LobbyChatManager_Destroy(LobbyChatManager *manager);

// Commands need a session that is logged in and has not failed (the commands return PPWLobbyResult codes).
#define CTX_CHECK(c)                                           \
    if ((c) == NULL) {                                         \
        return PPW_LOBBY_RESULT_ERROR_STATE;                   \
    }                                                          \
    if ((c) != NULL && (c)->state == LOBBY_CHAT_STATE_ERROR) { \
        return PPW_LOBBY_RESULT_ERROR_CONDITION;               \
    }                                                          \
    if ((c) != NULL && CtxState() != LOBBY_CHAT_STATE_READY) { \
        return PPW_LOBBY_RESULT_ERROR_STATE;                   \
    }

static inline void OpDelete(void *p) {
    if (p != NULL) {
        DWC_Free(0, p, 0);
    }
}

static inline void TagTemp(void) {
    LobbyTag t;
    t = (LobbyTag) { 0 };
}

void LobbyU32Vector_PushBackSlow(LobbyU32Vector *v, u32 *val, LobbyTag t);
void *LobbyU32Vector_Data(LobbyU32Vector *vector);

static inline BOOL StrEqC(Str *a, const char *b) {
    return LobbyString_CompareCStr(a, b) == 0;
}

// The session pointer, read from memory again at every call.
static inline LobbyChatManager *CurrentManager(void) {
    return *(LobbyChatManager *volatile *)&gLobbyChatManager;
}

static inline int CtxState(void) {
    return CurrentManager()->state;
}

typedef void (*VoidFunc)(void);

extern struct {
    LobbyChatManager *manager;
    void *buf;
} gLobbyMemReserve;

void LobbyChat_StripChannelPrefix(Str *out, LobbyChatManager *manager, Str *name);
void LobbyChat_StripChannelPrefixes(LobbyU32Vector *out, LobbyChatManager *manager, int n, u32 *arr);
void LobbyChat_FormatNick(Str *out, int userId);

extern VoidFunc gLobbyMemOutOfMemoryHandler;
extern u8 gLobbyRequestMapInsertFlags[];
extern char gLobbyStr_Empty_17[];
extern char gLobbyStr_normal[];
extern char gLobbyStr_voice[];
extern char gLobbyStr_ops[];
extern char gLobbyStr_VoiceOps[];
extern char gLobbyStr_invalid[];

LobbyChannelMapNode *LobbyChannelMap_LowerBound(LobbyChannelMap *map, Str *key, LobbyChannelMapNode *n, LobbyChannelMapNode *res);
void LobbyChannelMap_MakeEntry(LobbyChannelMapEntry *dst, Str *name, LobbyChannel *src);
void LobbyChatManager_GetGameKey(Str *out, LobbyChatManager *manager);
int LobbyChat_AreChannelUsersEnumerated(LobbyChatManager *manager, Str *channel);
int LobbyChat_SetChannelUsersEnumerated(LobbyChatManager *manager, Str *channel);
int LobbyChat_GetChannelUserLimit(LobbyChatManager *manager, Str *channel);
int LobbyChat_AddChannelUser(LobbyChatManager *manager, Str *nick, Str *channel);
LobbyChannelCallbacks *LobbyChat_GetChannelCallbacks(LobbyChatManager *manager, Str *key);
#ifdef HEARTGOLD_NATIVE
void LobbyChat_GetUserNick(Str *out, LobbyChatManager *manager, int userId);
#else
void LobbyChat_GetUserNick(Str *out, LobbyChatManager *manager, int userId, ...);
#endif
int LobbyChat_LeaveChannel(const char *channel);
int LobbyChat_RemoveChannel(LobbyChatManager *manager, Str *channel);
BOOL LobbyChat_RemoveChannelUser(LobbyChatManager *manager, Str *nick, Str *channel);
BOOL LobbyChat_RemoveUserIfUnused(LobbyChatManager *manager, int id);
int LobbyChat_SendChannelMessage(const char *channel, const char *message);
int LobbyChat_SendUserMessage(int userId, const char *message);
int LobbyChat_SetChannelKey(const char *channel, int userId, const char *key, const char *value);
int LobbyChat_SetChannelMode(const char *channel, const void *mode);
int LobbyChat_Shutdown(void);
LobbyRequest *LobbyEnterChannelRequest_Delete(LobbyRequest *request);
void LobbyEnterChannelRequest_Destroy(void);
LobbyRequest *LobbyEnumChannelsRequest_Delete(LobbyRequest *request);
void LobbyEnumChannelsRequest_Destroy(void);
LobbyRequest *LobbyEnumUsersRequest_Delete(LobbyRequest *request);
void LobbyEnumUsersRequest_Destroy(void);
LobbyRequest *LobbyGetChannelKeysRequest_Delete(LobbyRequest *request);
void LobbyGetChannelKeysRequest_Destroy(void);
LobbyRequest *LobbyGetChannelModeRequest_Delete(LobbyRequest *request);
void LobbyGetChannelModeRequest_Destroy(void);
BOOL LobbyMem_InstallOutOfMemoryHandler(LobbyChatManager *manager);
void LobbyMem_OnOutOfMemory(void);
void LobbyMem_OnOutOfMemoryDisconnect(void);
BOOL LobbyRequestMap_DeleteKey(LobbyRequestMap *map, u32 id);
void LobbyRequestMap_Erase(Iter *out, LobbyRequestMap *map, Iter pos);
void LobbyRequestMap_Find(Iter *out, LobbyRequestMap *map, LobbyRequest **key);
LobbyRequestMapNode *LobbyRequestMap_FindInsertPos(LobbyRequestMap *map, LobbyRequest **key, LobbyRequestMapNode **pos, u8 *insertLeft, u8 *isLeftmost);
void LobbyRequestMap_Insert(LobbyRequestMapInsertResult *out, LobbyRequestMap *map, LobbyRequest **val);
LobbyRequest *LobbyRequest_Delete(LobbyRequest *request);
void LobbyRequest_Destroy(LobbyRequest *request);
u32 LobbyRequest_GetParam(LobbyRequest *request);
u32 LobbyRequest_GetId(LobbyRequest *request);
void *LobbyTree_EraseRebalance(void *node, void *root);
LobbyUserIdSetNode *LobbyUserIdSet_LowerBound(LobbyUserIdSet *set, int *key, LobbyUserIdSetNode *node, LobbyUserIdSetNode *res);
BOOL LobbyUserMap_EraseKey(LobbyUserMap *map, int *key);
void LobbyUserMap_Find(Iter *out, LobbyUserMap *map, int *key);
LobbyUserMapNode *LobbyUserMap_FindInsertPos2(LobbyUserMap *map, int *key, LobbyUserMapNode **pos, u8 *insertLeft, u8 *isLeftmost);
LobbyUserMapNode *LobbyUserMap_LowerBound(LobbyUserMap *map, int *key, LobbyUserMapNode *node, LobbyUserMapNode *res);
void LobbyUserMap_MakeEntry(LobbyUserMapEntry *out, int id, const Str *nick);
void chatDisconnect(void *p);
int chatGetChannelNumUsersA(void *chat, const char *channel);
void chatRegisterUniqueNickA(void *chat, int accountSpace, const char *nick, const char *cdKey);
void chatRetryWithNickA(void *chat, const char *nick);
void chatLeaveChannelA(void *h, const char *name, int x);
void chatSendChannelMessageA(void *h, const char *name, const char *msg, int x);
void chatSendUserMessageA(void *h, const char *name, const char *msg, int x);
void chatSetChannelKeysA(void *h, const char *name, const char *who, int x, const char **key, const char **val);
void chatSetChannelModeA(void *h, const char *name, const void *mode);

void LobbyChatManager_GetGameName(Str *out, LobbyChatManager *manager);
void LobbyChatManager_GetNick(Str *out, LobbyChatManager *manager);
void LobbyChatManager_GetRealName(Str *out, LobbyChatManager *manager);
void LobbyChatManager_GetUser(Str *out, LobbyChatManager *manager);
int LobbyChat_FindUserIdByNick(LobbyChatManager *manager, const Str *nick);
int LobbyChat_GetLastError(void);
int LobbyChat_IsInChannel(char *channel);
int LobbyChat_RenameUser(LobbyChatManager *manager, const Str *oldNick, const Str *newNick);
int LobbyChat_TimerConnectTimeout(void *param);
int LobbyChat_ToErrorCode(int error);
BOOL chatInChannelA(void *chat, const char *channel);

#endif // POKEHEARTGOLD_LOBBY_CHAT_INTERNAL_H
