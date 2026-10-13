#pragma force_active on
#include <string.h>

#include "global.h"

#include "dwc/friend_internal.h"
#include "dwc/match_internal.h"

// The code below is compiled as ARM (32-bit) code.
#include <nitro/code32.h>

#pragma warn_uninitializedvar off

// Matchmaking set-up: custom server keys, the "all peers ready" timer option, initialising the matchmaking
// work, starting the query/reporting record and the server browser for the three ways of starting a
// match (ov00_021F1938, ov00_021F1B24, ov00_021F1BCC), and the per-frame update with its timeouts.

// Query/reporting record created by ov00_022108BC.
typedef struct MatchReporting {
    u8 unk_00[0xB4];
    int unk_B4;
} MatchReporting;

extern FriendGlobal ov00_0221A918;

extern const char ov00_02217A58[]; // " and ()"
extern const char ov00_02217A60[]; // ""

extern void *ov00_021EC2A8(int kind, u32 size);
extern void ov00_021EC2EC(int kind, void *p, int size);
extern int ov00_021EC230(void);
extern int ov00_021EE568(void);
extern int ov00_021F04A0(int type, const char *message, int arg);
extern void ov00_021F2EC4(void);
extern void ov00_021F2EE4(int kind, int code);
extern int ov00_021F36BC(void);
extern void ov00_021F4304(int command, u32 profile, u32 addr, u16 port, void *data, u32 size);
extern void gt2Think(void *peer);
extern int gt2GetLocalPort(void *peer);
extern int gt2GetSocketSOCKET(void *peer);
extern void ov00_0220FB40(void);
extern void ov00_022103C4(void);
extern int ov00_022108BC(void **reporting, int a, int b, const char *gameName, const char *secretKey, int c, int d, void *cb0, void *cb1, void *cb2, void *cb3, void *cb4, void *cb5, int e);
extern void qr2_register_natneg_callback(void *reporting, void *callback);
extern void qr2_register_clientmessage_callback(void *reporting, void *callback);
extern void qr2_register_publicaddress_callback(void *reporting, void *callback);
extern void qr2_think(void *reporting);
extern void qr2_send_statechanged(void *reporting);
extern void qr2_shutdown(void *reporting);
extern void qr2_register_keyA(u32 id, const char *name);
extern void *ServerBrowserNewA(const char *gameName, const char *fromGameName, const char *secretKey, int a, int b, int c, int d, void (*callback)(void *list, int event, void *arg), int e);
extern void ServerBrowserFree(void *list);
extern void ServerBrowserThink(void *list);
extern int ServerBrowserState(void *list);

u32 ov00_021F1248(void) {
    return ov00_0221A918.work->unk_20;
}

void ov00_021F125C(void) {
    if (ov00_0221A928.updating) {
        ov00_0221A928.deferred = 1;
        return;
    }
    ov00_0220FB40();
}

int ov00_021F1284(void) {
    if (!ov00_021F12A4()) {
        return 0;
    }
    ov00_021F6BA8();
    return 1;
}

int ov00_021F12A4(void) {
    if (ov00_021EC230()) {
        return 0;
    }
    if (!ov00_021F955C() || ov00_021EE568() != 5 || ov00_021F955C()->unk_1A9) {
        return 0;
    }
    return 1;
}

// Stops accepting players: right away in state 10, otherwise once ov00_021F1DE4 sees state 10.
int ov00_021F12F0(void (*callback)(void *param), void *param) {
    if (ov00_021EC230() || !ov00_021F955C() || ov00_021F955C()->mode != 2 || (ov00_021EE568() != 5 && ov00_021EE568() != 6)) {
        return 0;
    }
    if (ov00_021F955C()->state == 10) {
        ov00_021F955C()->limit = ov00_021F955C()->players;
        ov00_021F36BC();
        if (callback) {
            callback(param);
        }
    } else {
        ov00_021F955C()->stopCallback = callback;
        ov00_021F955C()->stopParam = param;
        ov00_021F955C()->stopPending = 1;
    }
    return 1;
}

// Adds custom server key id (100..253, or the next free one) with the given name; returns the key id or 0.
u32 ov00_021F13A4(u32 id, const char *name, void *value) {
    u32 idx;
    if (name == NULL || value == NULL) {
        return 0;
    }
    if (id >= 100 && ov00_0221A948[idx = id - 100].id != 0) {
        if (ov00_0221A948[idx].buffer != NULL && strcmp(ov00_0221A948[idx].buffer, name) != 0) {
            return 0;
        }
    } else {
        id = ov00_021F7A98();
        if (id == 0) {
            return 0;
        }
    }
    idx = id - 100;
    ov00_0221A948[idx].id = id;
    ov00_0221A948[idx].isString = 1;
    ov00_0221A948[idx].unk_02 = 0;
    if (ov00_0221A948[idx].buffer != NULL) {
        ov00_021EC2EC(4, ov00_0221A948[idx].buffer, 0);
    }
    ov00_0221A948[idx].buffer = ov00_021EC2A8(4, strlen(name) + 1);
    if (ov00_0221A948[idx].buffer == NULL) {
        ov00_021F2EE4(9, -80001);
        return 0;
    }
    strcpy(ov00_0221A948[idx].buffer, name);
    ov00_0221A948[idx].value = value;
    qr2_register_keyA(id, ov00_0221A948[idx].buffer);
    return id;
}

// Option 0: set up the "all peers ready" timer from param; option 1: *(u32 *)param sets ov00_0221A928.unk_1C.
int ov00_021F14D4(int option, MatchTimer *param) {
    if (ov00_021F955C() == NULL) {
        return 1;
    }
    if (param == NULL) {
        return 3;
    }
    switch (option) {
    case 0:
        if (ov00_021F955C()->state == 19) {
            return 1;
        }
        if (param->active && param->wanted <= 1) {
            return 3;
        }
        if (ov00_0221A928.timer == NULL) {
            ov00_0221A928.timer = ov00_021EC2A8(4, sizeof(MatchTimer));
            if (ov00_0221A928.timer == NULL) {
                return 4;
            }
        }
        ov00_0221A928.timer->active = param->active;
        ov00_0221A928.timer->wanted = param->wanted;
        ov00_0221A928.timer->retries = 0;
        ov00_0221A928.timer->unk_03 = 0;
        ov00_0221A928.timer->timeout = param->timeout;
        ov00_0221A928.timer->mask = 0;
        ov00_0221A928.timer->all = 0;
        ov00_0221A928.timer->start = OS_GetTick();
        ov00_0221A928.timer->last = OS_GetTick();
        return 0;
    case 1:
        if (*(u32 *)param) {
            ov00_0221A928.unk_1C = 1;
        } else {
            ov00_0221A928.unk_1C = 0;
        }
        ov00_0221A928.unk_1D = 0;
        return 0;
    default:
        return 2;
    }
}

static inline void ClearGlobalFlags(int zero, MatchGlobal *global) {
    global->unk_1C = zero;
    global->unk_1D = zero;
    global->unk_1E = zero;
}

void ov00_021F1620(MatchWork *work, void *unk_00, void **peer, void *callback, const char *gameName, const char *secretKey, void *friends, u32 unk_300) {
    ov00_021F954C(work);
    ov00_021F955C()->unk_00 = unk_00;
    ov00_021F955C()->peer = peer;
    ov00_021F955C()->callback = callback;
    ov00_021F955C()->reporting = NULL;
    ov00_021F955C()->addr = 0;
    ov00_021F955C()->port = 0;
    ov00_021F955C()->list = NULL;
    ov00_021F956C(0);
    ov00_021F955C()->unk_0F = 0;
    ov00_021F955C()->unk_19 = 0;
    ov00_021F955C()->unk_1AF = 0;
    ov00_021F955C()->unk_1B6 = 0;
    ov00_021F955C()->unk_1DC = 0;
    ov00_021F955C()->identity = 0;
    ov00_021F955C()->gameName = gameName;
    ov00_021F955C()->secretKey = secretKey;
    ov00_021F955C()->friends = friends;
    ov00_021F955C()->unk_300 = unk_300;
    MI_CpuFill8(ov00_021F955C()->friendIds, 0, sizeof(ov00_021F955C()->friendIds));
    ov00_021F955C()->friendCount = 0;
    ov00_021F955C()->errorCallback = 0;
    ov00_021F955C()->errorParam = 0;
    ov00_021F955C()->evalCallback = NULL;
    ov00_021F955C()->evalParam = NULL;
    ov00_021F955C()->stopCallback = NULL;
    ov00_021F955C()->stopParam = NULL;
    ov00_021F955C()->pending = 0;
    ov00_021F7AD8();
    ClearGlobalFlags(0, &ov00_0221A928);
    ov00_021F381C(0);
}

// Creates the query/reporting record (5 tries) and sets its callbacks.
int ov00_021F179C(u32 identity) {
    int i, result;
    MatchWork *work, *names;
    int ip, port;
    if (ov00_021F955C()->reporting) {
        return 0;
    }
    ov00_021F955C()->identity = identity;
    for (i = 0; i < 5; i++) {
        work = ov00_021F955C();
        ip = gt2GetSocketSOCKET(*ov00_021F955C()->peer);
        port = gt2GetLocalPort(*ov00_021F955C()->peer);
        names = ov00_021F955C();
        result = ov00_022108BC(&work->reporting, ip, port, names->gameName, ov00_021F955C()->secretKey, 1, 1, ov00_021F8D18, ov00_021F8E60, ov00_021F8E64, ov00_021F8E68, ov00_021F8F0C, ov00_021F8F14, 0);
        if (!result) {
            break;
        }
        qr2_shutdown(ov00_021F955C()->reporting);
        ov00_021F955C()->reporting = NULL;
        if (result != 3 || i == 4) {
            ov00_021F8380(result);
            return result;
        }
    }
    ov00_021F955C()->addr = 0;
    ov00_021F955C()->port = 0;
    qr2_register_publicaddress_callback(ov00_021F955C()->reporting, ov00_021F8F20);
    qr2_register_natneg_callback(ov00_021F955C()->reporting, ov00_021F8F40);
    qr2_register_clientmessage_callback(ov00_021F955C()->reporting, ov00_021F8FF4);
    qr2_send_statechanged(ov00_021F955C()->reporting);
    return result;
}

// Starts matchmaking among anybody (type 0); filter is added to the server list filter.
void ov00_021F1938(int count, const char *filter, void *arg2, void *arg3, int (*evalCallback)(int index, void *param), void *evalParam) {
    int size;
    char buf[256];
    MatchGlobal *global = &ov00_0221A928;
    MatchWork *first, *second, *dest, *third;
    char *old = global->filter;
    if (old) {
        ov00_021EC2EC(4, old, 0);
        ov00_0221A928.filter = NULL;
    }
    if (filter) {
        size = ov00_021F3EF4(buf, -1, 32, 3);
        size = 256 - size - strlen(ov00_02217A58);
        ov00_0221A928.filter = ov00_021EC2A8(4, size);
        if (ov00_0221A928.filter == NULL) {
            ov00_021F2EE4(9, -80001);
            return;
        }
        MI_CpuCopy8(filter, ov00_0221A928.filter, size);
        *(ov00_0221A928.filter + size - 1) = 0;
    }
    ov00_021F3BB4(0, count, arg2, arg3);
    ov00_021F955C()->evalCallback = evalCallback;
    ov00_021F955C()->evalParam = evalParam;
    ov00_021F956C(2);
    if (ov00_021F955C()->list == NULL) {
        dest = ov00_021F955C();
        first = ov00_021F955C();
        second = ov00_021F955C();
        third = ov00_021F955C();
        dest->list = ServerBrowserNewA(first->gameName, second->gameName, third->secretKey, 0, 20, 1, 0, ov00_021F85B8, 0);
    }
    if (ov00_021F955C()->list == NULL) {
        if (ov00_021F82EC(5) != 0) {
            return;
        }
    }
    ov00_021F04A0(3, ov00_02217A60, 0);
    if (ov00_021F82EC(ov00_021F3CF8(ov00_021F955C()->identity)) != 0) {
        return;
    }
    if (ov00_021F955C()->reporting == NULL) {
        ov00_021F179C(ov00_021F955C()->identity);
    }
    ov00_021F7DDC(0);
}

// Starts matchmaking as the server (type 2).
void ov00_021F1B24(int count, void *arg1, void *arg2, void *callback, void *param) {
    ov00_021F3BB4(2, count, arg1, arg2);
    ov00_021F955C()->unk_46C = callback;
    ov00_021F955C()->unk_470 = param;
    ov00_021F955C()->profiles[0] = ov00_021F955C()->identity;
    ov00_021F955C()->aidMask = 1;
    ov00_021F955C()->aidCount = 0;
    ov00_0221A928.unk_1D = 0;
    ov00_021F956C(10);
    if (ov00_021F8278(ov00_021F36BC())) {
        return;
    }
    if (ov00_021F955C()->reporting) {
        return;
    }
    ov00_021F179C(ov00_021F955C()->identity);
}

// Starts matchmaking as a client of the server with the given profile id (type 3).
void ov00_021F1BCC(int profile, void *arg1, void *arg2, void *callback, void *param) {
    MatchWork *work, *first, *second, *third;
    ov00_021F3BB4(3, 0, arg1, arg2);
    ov00_021F955C()->unk_46C = callback;
    ov00_021F955C()->unk_470 = param;
    ov00_021F955C()->unk_17 = 1;
    work = ov00_021F955C();
    work->unk_20 = ov00_021F955C()->identity;
    ov00_021F955C()->profiles[0] = profile;
    ov00_021F956C(4);
    if (ov00_021F955C()->list == NULL) {
        work = ov00_021F955C();
        first = ov00_021F955C();
        second = ov00_021F955C();
        third = ov00_021F955C();
        work->list = ServerBrowserNewA(first->gameName, second->gameName, third->secretKey, 0, 20, 1, 0, ov00_021F85B8, 0);
    }
    if (ov00_021F955C()->list == NULL) {
        if (ov00_021F82EC(5)) {
            return;
        }
    }
    if (ov00_021F8278(ov00_021F04A0(5, ov00_02217A60, 0))) {
        return;
    }
    if (ov00_021F955C()->reporting == NULL) {
        if (ov00_021F179C(ov00_021F955C()->identity)) {
            return;
        }
    }
    ov00_021F5B5C(ov00_021F955C()->profiles[0], 0);
    if (ov00_021F5B30()) {
        return;
    }
}

void ov00_021F1D34(void *reporting) {
    int mode;
    if (!reporting) {
        return;
    }
    qr2_think(reporting);
    if (((MatchReporting *)reporting)->unk_B4) {
        return;
    }
    mode = ov00_021F955C()->mode;
    switch (mode) {
    case 0:
    case 1:
        switch (ov00_021F955C()->state) {
        case 1:
        case 2:
        case 3:
        case 4:
        case 6:
        case 11:
            qr2_send_statechanged(ov00_021F955C()->reporting);
            break;
        }
        break;
    case 2:
        if (ov00_021F955C()->state == 11) {
            qr2_send_statechanged(ov00_021F955C()->reporting);
        }
        break;
    }
}

// Per-frame matchmaking update: state timeouts and command retries, the server list and the query/reporting
// record, then the deferred stop of ov00_021F12F0.
void ov00_021F1DE4(BOOL active) {
    if (!ov00_021F955C()) {
        return;
    }
    if (ov00_021EC230()) {
        return;
    }
    if (!active) {
        if (ov00_021F955C()->reporting) {
            qr2_think(ov00_021F955C()->reporting);
        }
        if (!ov00_021F955C()->peer) {
            return;
        }
        gt2Think(*ov00_021F955C()->peer);
        return;
    }
    if (!ov00_021F955C()->state) {
        return;
    }
    switch (ov00_021F955C()->state) {
    case 0:
    case 6:
    case 8:
    case 9:
    case 10:
    case 12:
        break;
    case 4:
        if (ov00_021F955C()->delay) {
            u64 now = OS_GetTick();
            MatchWork *stamp = ov00_021F955C();
            MatchWork *limit = ov00_021F955C();
            if (TICKS_TO_MS(now - stamp->time) > limit->delay) {
                ov00_021F955C()->delay = 0;
                if (ov00_021F955C()->mode == 3) {
                    ov00_021F955C()->unk_1AA++;
                    if (ov00_021F955C()->unk_1AA > 5) {
                        ov00_021F2EE4(6, -80430);
                        return;
                    }
                    ov00_021F5B5C(ov00_021F955C()->profiles[0], 0);
                    if (ov00_021F5B30()) {
                        return;
                    }
                } else if (!ov00_021F5FD4(0)) {
                    return;
                }
            }
        }
        if (ov00_021F955C()->active) {
            u32 limit = 3000 * ov00_021F955C()->players + 3000;
            u64 now = OS_GetTick();
            if (TICKS_TO_MS(now - ov00_021F955C()->activeTick) >= limit) {
                ov00_021F5B5C(ov00_021F955C()->profiles[0], 0);
                if (ov00_021F5B30()) {
                    return;
                }
            }
        }
        break;
    case 2:
    case 3:
    case 5:
        if (ov00_021F955C()->unk_E8 > 0) {
            u32 limit;
            u64 now;
            if (ov00_021F955C()->state == 3) {
                limit = 3000 * ov00_021F955C()->players + 3000;
            } else {
                limit = ov00_021F955C()->unk_E8 == 1 ? 1000 : 3000;
            }
            now = OS_GetTick();
            if (TICKS_TO_MS(now - ov00_021F955C()->unk_EC) > limit) {
                if (ov00_021F82EC(ov00_021F3CF8(ov00_021F955C()->target))) {
                    return;
                }
                ov00_021F955C()->unk_E8 = 0;
            }
        }
        break;
    case 7:
        if (ov00_021F955C()->waitTick) {
            u64 now = OS_GetTick();
            if (TICKS_TO_MS(now - ov00_021F955C()->waitTick) > 25000) {
                ov00_021F955C()->waitTick = 0;
                if (!ov00_021F631C(ov00_021F955C()->profiles[0])) {
                    return;
                }
            }
        } else if (ov00_021F955C()->unk_3CC == 6) {
            u64 now = OS_GetTick();
            if (TICKS_TO_MS(now - ov00_021F955C()->commandTick) > 6000) {
                ov00_021F955C()->commandRetries++;
                if (ov00_021F955C()->commandRetries > 5) {
                    ov00_021F2EC4();
                    if (!ov00_021F631C(ov00_021F955C()->profiles[0])) {
                        return;
                    }
                } else {
                    ov00_021F4304(6, ov00_021F955C()->commandProfile, ov00_021F955C()->commandAddr, ov00_021F955C()->commandPort, ov00_021F955C()->commandData, ov00_021F955C()->commandSize);
                    if (ov00_021F5B30()) {
                        return;
                    }
                }
            }
        }
        break;
    case 11:
        if (ov00_021F955C()->unk_3CC == 2) {
            if (!ov00_021F955C()->mode) {
                u64 now = OS_GetTick();
                if (TICKS_TO_MS(now - ov00_021F955C()->commandTick) > 6000) {
                    goto expired;
                }
            }
            if (ov00_021F955C()->mode) {
                u64 now = OS_GetTick();
                if (TICKS_TO_MS(now - ov00_021F955C()->commandTick) > 19000) {
expired:
                    ov00_021F2EC4();
                    if (!ov00_021F6100(ov00_021F955C()->profiles[ov00_021F955C()->players + 1])) {
                        return;
                    }
                }
            }
        }
        break;
    case 13:
        if (ov00_021F955C()->unk_3CC == 8) {
            u64 now = OS_GetTick();
            if (TICKS_TO_MS(now - ov00_021F955C()->commandTick) > 30000) {
                ov00_021F955C()->commandRetries++;
                if (ov00_021F955C()->commandRetries) {
                    ov00_021F2EC4();
                    if (ov00_021F955C()->mode == 2) {
                        if (!ov00_021F6100(ov00_021F955C()->profiles[ov00_021F955C()->players])) {
                            return;
                        }
                    } else {
                        ov00_021F6FC4();
                    }
                } else {
                    ov00_021F4304(8, ov00_021F955C()->commandProfile, ov00_021F955C()->commandAddr, ov00_021F955C()->commandPort, ov00_021F955C()->commandData, ov00_021F955C()->commandSize);
                    if (ov00_021F5B30()) {
                        return;
                    }
                }
            }
        }
        break;
    case 1:
        if (ov00_021EE568() == 5) {
            u64 now = OS_GetTick();
            if (TICKS_TO_MS(now - ov00_021F955C()->unk_1F8) > 30000) {
                if (ov00_021F955C()->unk_1AD >= 5) {
                    if (!ov00_021F631C(ov00_021F955C()->profiles[0])) {
                        return;
                    }
                } else {
                    ov00_021F4304(64, ov00_021F955C()->profiles[0], ov00_021F955C()->addresses[0], ov00_021F955C()->ports[0], NULL, 0);
                    if (ov00_021F5B30()) {
                        return;
                    }
                    ov00_021F955C()->unk_1AD++;
                    ov00_021F955C()->unk_1F8 = OS_GetTick() - 12567750; // 24 s back: the next try comes 6 s later
                }
            }
        }
        break;
    }
    if (ov00_021F955C()->state == 11 || ov00_021F955C()->state == 6) {
        if (ov00_021F955C()->retryTick) {
            u64 now = OS_GetTick();
            if (TICKS_TO_MS(now - ov00_021F955C()->retryTick) > 10000) {
                ov00_021F9104(1, 0, NULL, &ov00_021F955C()->request);
            }
        }
    }
    if (ov00_021F955C()->list) {
        ov00_0221A928.busy = 0;
        ov00_0221A928.unk_14 = 0;
        ServerBrowserThink(ov00_021F955C()->list);
        if (ov00_0221A928.unk_14) {
            ServerBrowserFree(ov00_021F955C()->list);
            ov00_021F955C()->list = NULL;
        }
        if (ov00_021F955C()->list && ServerBrowserState(ov00_021F955C()->list) && ov00_021F955C()->pending) {
            u64 now = OS_GetTick();
            if (now > ov00_021F955C()->pending) {
                ov00_021F2EE4(6, -85010);
            }
        }
    }
    ov00_021F1D34(ov00_021F955C()->reporting);
    ov00_0221A928.deferred = 0;
    ov00_0221A928.updating = 1;
    ov00_022103C4();
    ov00_0221A928.updating = 0;
    if (ov00_0221A928.deferred == 1) {
        ov00_0221A928.deferred = 0;
        ov00_0220FB40();
    }
    if (ov00_021F955C()->peer) {
        gt2Think(*ov00_021F955C()->peer);
    }
    if (ov00_021F955C()->state == 18) {
        u64 now = OS_GetTick();
        if (TICKS_TO_MS(now - ov00_021F955C()->unk_1F0) > 3000 && ov00_021F6FF8()) {
            return;
        }
    }
    if (!ov00_021F7384()) {
        return;
    }
    if (!ov00_021F7898()) {
        return;
    }
    ov00_021F7E4C();
    if (!ov00_021F955C()->stopPending) {
        return;
    }
    if (ov00_021F955C()->state != 10) {
        return;
    }
    ov00_021F955C()->limit = ov00_021F955C()->players;
    ov00_021F36BC();
    ov00_021F955C()->stopPending = 0;
    if (!ov00_021F955C()->stopCallback) {
        return;
    }
    ov00_021F955C()->stopCallback(ov00_021F955C()->stopParam);
}
