#include "global.h"

#include "ppw_lobby_state.h"

#include <nitro/code32.h>

#include "lobby_timer.h"
#include "overlay45_format_buffer_private.h"

#include "../../lib/NitroDWC/src/tree-iterator4.h"

typedef struct {
    u32 words[4];

    struct {
        u8 bytes[32];
    } a;

    struct {
        u8 bytes[20];
    } b;
} Packet;

// The PPWLobbyWork fields the recruiting functions use, through a volatile pointer: every step reloads it.
typedef struct {
    int state;
    u8 padding_04[0x4C - 0x04];
    u32 userId;
    u8 padding_50[0x68 - 0x50];
    int recruiting;
    u8 padding_6C[0x74 - 0x6C];
    Packet recruitInfo;
    u8 padding_B8[0x134 - 0xB8];
    LobbyTimerMap timers;
} Work;

extern Work *volatile gPPWLobbyWork;

static inline int release(Work *w) {
    if (w->recruiting) {
        w->recruiting = 0;
        return 1;
    }
    return 0;
}

extern int PPWLobby_SendMessage(int, int, int, int, int, const void *, u32);
void LobbyTimerMap_Erase(Iterator *, LobbyTimerMap *, Iterator);
char *LobbyCharVector_Data(LobbyCharVector *);
int PPW_LobbyUpdateRecruitInfo(Packet *);
int PPW_LobbyStopRecruit(void);

char *LobbyCharVector_Data(LobbyCharVector *p) {
    if (p->size == 0) {
        return 0;
    }
    return p->data;
}

int PPW_LobbyUpdateRecruitInfo(Packet *p) {
    Work *w = gPPWLobbyWork;
    int result;
    if (!w) {
        return PPW_LOBBY_RESULT_ERROR_STATE;
    }
    if (w && w->state == PPW_LOBBY_STATE_ERROR) {
        return PPW_LOBBY_RESULT_ERROR_CONDITION;
    }
    if (w && gPPWLobbyWork->state != PPW_LOBBY_STATE_READY) {
        return PPW_LOBBY_RESULT_ERROR_STATE;
    }
    w = gPPWLobbyWork;
    if (!w->recruiting) {
        result = 0;
    } else {
        w->recruitInfo.words[0] = p->words[0];
        w->recruitInfo.words[1] = p->words[1];
        w->recruitInfo.words[2] = p->words[2];
        w->recruitInfo.words[3] = p->words[3];
        w->recruitInfo.a = p->a;
        w->recruitInfo.b = p->b;
        result = 1;
    }
    if (!result) {
        return PPW_LOBBY_RESULT_ERROR_STATE;
    }
    return PPW_LOBBY_RESULT_SUCCESS;
}

int PPW_LobbyStopRecruit(void) {
    u32 timerId;
    Work *w = gPPWLobbyWork;
    int result;
    if (!w) {
        return PPW_LOBBY_RESULT_ERROR_STATE;
    }
    if (w && w->state == PPW_LOBBY_STATE_ERROR) {
        return PPW_LOBBY_RESULT_ERROR_CONDITION;
    }
    if (w && gPPWLobbyWork->state != PPW_LOBBY_STATE_READY) {
        return PPW_LOBBY_RESULT_ERROR_STATE;
    }
    w = gPPWLobbyWork;
    result = release(w);
    if (!result) {
        return PPW_LOBBY_RESULT_ERROR_STATE;
    }
    w = gPPWLobbyWork;
    PPWLobby_SendMessage(w->userId, 0, 1, 0, 2, &w->recruitInfo, sizeof(Packet));
    timerId = 3;
    LobbyTimerMap_EraseKey(&gPPWLobbyWork->timers, &timerId);
    return PPW_LOBBY_RESULT_SUCCESS;
}

BOOL LobbyTimerMap_EraseKey(LobbyTimerMap *map, u32 *id) {
    struct {
        Iterator argument, out;
        LobbyTimerMapNode *found;
    } local;

    LobbyTimerMap_Find(&local.found, map, id);
    if (local.found == (LobbyTimerMapNode *)&map->root) {
        return FALSE;
    }
    local.argument.current = local.found;
    LobbyTimerMap_Erase(&local.out, map, local.argument);
    return TRUE;
}
