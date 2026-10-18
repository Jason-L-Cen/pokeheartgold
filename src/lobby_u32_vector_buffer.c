#include "global.h"
#include "lobby_chat_callbacks.h"

// The code below is compiled as ARM (32-bit) code.
#include <nitro/code32.h>
#define W(p, n) (*(int *)((u8 *)(p) + (n)))
#define U(p, n) (*(unsigned *)((u8 *)(p) + (n)))
#define P(p, n) (*(void **)((u8 *)(p) + (n)))
typedef struct U32BufferAllocator { u8 byte; } U32BufferAllocator;

typedef union {
    struct {
        u32 flag : 1;
        u32 pad : 31;
        u32 len;
        char *data;
    } b;

    struct {
        char head;
        char inlinebuf[11];
    } s;
} Str;

extern void *gLobbyChatManager;
extern char sStr_normal[];
extern char sStr_voice[];
extern char sStr_ops[];
extern char sStr_VoiceOps[];
extern char sStr_invalid[];
Str * LobbyString_InitFromCStr(Str *, char *);
Str * LobbyString_Destroy(Str *);
void *LobbyString_InitCopy(void *, void *);
void *LobbyU32Vector_Destroy(void *);
BOOL LobbyTimerMap_EraseKey(void *, int *);
void *LobbyChatManager_Destroy(void *);
void DWC_Free(int, void *, int);
void LobbyChat_RenameUser(void *, Str *, Str *);
int LobbyChat_RemoveChannel(void *, Str *);
void LobbyString_AssignCStr(Str *, char *);
void *LobbyChat_GetChannelCallbacks(void *, Str *);
void LobbyChat_StripChannelPrefix(Str *, void *, Str *);
Str *LobbyChat_GetFullChannelName(Str *, void *, Str *);
int chatInChannelA(void *, char *);
void _ZNSt15__vector_commonILb1EE18throw_length_errorEv(void);
void *LobbyMem_Alloc(unsigned);
void *LobbyU32Vector_RecommendSize(void *, unsigned, char *);
void LobbyU32Vector_Reallocate(void *, unsigned, void *);
char *strncpy(char *, const char *, size_t);
int LobbyChat_FindUserIdByNick(void *, Str *);
void *LobbyU32VectorBuffer_Destroy(void *p);
void LobbyU32VectorBuffer_Allocate(void *p, unsigned n, U32BufferAllocator unused);
void *LobbyU32VectorBuffer_Deallocate(void *p);
void LobbyChatManager_GetUser(void *out, void *p);
void LobbyChatManager_GetNick(void *out, void *p);
void LobbyChatManager_GetGameName(void *out, void *p);
void LobbyChatManager_GetRealName(void *out, void *p);
int LobbyRequest_GetParam(void *p);
void *LobbyU32Vector_Data(void *p);
void LobbyRequest_Destroy(void);
void *LobbyRequestMap_LowerBound(void *c, void *key, void *node, void *best);
void LobbyRequestMap_Find(void **out, void *tree, void *key);
void LobbyU32Vector_AssignRange(void *p, u32 *first, u32 *last);
int LobbyChat_TimerConnectTimeout(void);
int LobbyChat_GetLastError(void);
void *LobbyChat_ToErrorCode(void *p);
int LobbyChat_IsInChannel(char *s);

void *LobbyU32VectorBuffer_Destroy(void *p) {
    W(p, 4) = 0;
    LobbyU32VectorBuffer_Deallocate(p);
    return p;
}

void LobbyU32VectorBuffer_Allocate(void *p, unsigned n, U32BufferAllocator unused) {
    if (n > 0x3fffffff) {
        _ZNSt15__vector_commonILb1EE18throw_length_errorEv();
    }
    P(p, 0) = LobbyMem_Alloc(n * 4);
    U(p, 8) = n;
}
