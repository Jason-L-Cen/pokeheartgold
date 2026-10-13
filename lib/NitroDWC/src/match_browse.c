#pragma force_active on
#include "global.h"

#include "dwc/match_internal.h"

// The code below is compiled as ARM (32-bit) code.
#include <nitro/code32.h>

#pragma warn_unusedvar off
#pragma warn_uninitializedvar off

// Matchmaking over the server browser: the custom server keys, the filter applied to the server list,
// the list callback driving the connection states, the error code mapping, and the NAT negotiation result.

extern MatchKeyEntry ov00_0221A948[];
extern u8 ov00_0221A949[];
extern void *ov00_0221A950[];
extern char ov00_02217A64[];       // "%u"
extern char ov00_02217AAC[];       // "dwc_pid"
extern char ov00_02217AB4[];       // "dwc_mtype"
extern char ov00_02217AC0[];       // "dwc_mresv"
extern char ov00_02217ACC[];       // "dwc_mver"
extern const u8 ov00_02217AD8[];   // "dwc_eval"
extern const char ov00_02217B54[]; // "SBCM"
extern char ov00_02217B70[];       // "numplayers"
extern char ov00_02217B7C[];       // "maxplayers"
extern const int ov00_02216040[];

extern int memcmp(const void *, const void *, u32);
extern int strncmp(const char *, const char *, u32);
extern void ov00_021EC24C(int, int);
extern void ov00_021EC2EC(int, void *, int);
extern int ov00_021ED5E4(u32);
extern int ov00_021EE568(void);
extern void ov00_021EF3C0(int, int);
extern void ov00_021F0238(int, int);
extern int ov00_021F03BC(int);
extern void DWCi_StopMatching(int, int);
extern void DWCi_SendMatchCommand(int, int, u32, u16, int, int);
extern int gt2Connect(void *, int, u32, char *, int, int, void *, int);
extern u32 gt2AddressToString(u32, u16, int);
extern void qr2_keybuffer_add(void *, int);
extern void qr2_buffer_add_int(void *, int);
extern void qr2_buffer_addA(void *, void *);
extern void SBServerAddIntKeyValue(void *, const void *, int);
extern int SBServerGetIntValueA(void *, const void *, int);
extern u32 SBServerGetPublicInetAddress(void *);
extern u16 SBServerGetPublicQueryPort(void *);
extern void ServerBrowserRemoveServer(void *, void *);
extern void *ServerBrowserGetServer(void *, int);
extern int ServerBrowserCount(void *);
extern void ServerBrowserSortA(void *, int, const void *, int);

u8 DWCi_GetNewMatchKeyID(void);
void DWCi_ClearMatchKeys(void);
u8 DWCi_GetFreeAid(void);
int DWCi_IsFriendProfile(int profile);
int DWCi_CheckServerKeys(void *server);
u8 DWCi_GetAidFromProfile(u32 profile, int first);
u32 DWCi_GetPeerAidBitmap(int flag);
void ov00_021F7DB4(void);
void DWCi_ResetMinCompleteTimer(int flag);
void DWCi_ProcessMinComplete(void);
int DWCi_HandleGPError(unsigned error);
int DWCi_HandleSBError(unsigned error);
int DWCi_HandleQR2Error(int error);
int DWCi_HandleNNError(unsigned error);
int DWCi_HandleNNResult(int result);
int DWCi_HandleGT2Error(unsigned error);
void DWCi_SBCallback(void *list, int event, void *arg);
void DWCi_SBServerUpdated(void *arg);
int DWCi_EvaluateServers(int sort);
void DWCi_PickRandomServer(void);
void DWCi_QR2ServerKeyCallback(int id, void *out);
void DWCi_QR2PlayerKeyCallback(void);
void DWCi_QR2TeamKeyCallback(void);
void DWCi_QR2KeyListCallback(int state, void *p);
int DWCi_QR2CountCallback(void);
int DWCi_QR2AddErrorCallback(unsigned error);
void DWCi_QR2PublicAddressCallback(u32 addr, u16 port);
void DWCi_QR2NatNegCallback(int profile);
void DWCi_QR2ClientMessageCallback(void *src, u32 size);
void DWCi_NNProgressCallback(void);
void DWCi_NNCompletedCallback(int result, int unused, MatchAddr *addr, MatchRequest *req);

u8 DWCi_GetNewMatchKeyID(void) {
    u8 i;
    for (i = 0; i < 154; i++) {
        if (!ov00_0221A948[i].id) {
            return i + 100;
        }
    }
    return 0;
}

void DWCi_ClearMatchKeys(void) {
    int i;
    MatchKeyEntry *p = ov00_0221A948;
    for (i = 0; i < 154; i++, p++) {
        if (p->buffer) {
            ov00_021EC2EC(4, p->buffer, 0);
        }
    }
    MIi_CpuClear32(0, (u32 *)ov00_0221A948, 1848);
}

u8 DWCi_GetFreeAid(void) {
    u8 aid = 0;
    int i;
    do {
        for (i = 0; i <= ov00_021F955C()->connections; i++) {
            if (aid == ov00_021F955C()->aids[i]) {
                break;
            }
        }
        if (i > ov00_021F955C()->connections) {
            break;
        }
        aid++;
    } while (aid < 32);
    return aid;
}

int DWCi_IsFriendProfile(int profile) {
    int i;
    if (!ov00_021F955C()->friends) {
        return 0;
    }
    for (i = 0; i < (int)ov00_021F955C()->friendCount; i++) {
        int x = ov00_021F03BC(ov00_021F955C()->friendIds[i]);
        if (x > 0 && x == profile) {
            return 1;
        }
    }
    return 0;
}

int DWCi_CheckServerKeys(void *p) {
    if (SBServerGetIntValueA(p, ov00_02217B70, -1) == -1) {
        return 0;
    }
    if (SBServerGetIntValueA(p, ov00_02217B7C, -1) == -1) {
        return 0;
    }
    if (SBServerGetIntValueA(p, ov00_02217AB4, -1) == -1) {
        return 0;
    }
    if (SBServerGetIntValueA(p, ov00_02217AC0, -1) == -1) {
        if (SBServerGetIntValueA(p, ov00_02217AC0, 0) == 0) {
            return 0;
        }
    }
    if (SBServerGetIntValueA(p, ov00_02217ACC, -1) == -1) {
        return 0;
    }
    return SBServerGetIntValueA(p, ov00_02217AAC, 0);
}

u8 DWCi_GetAidFromProfile(u32 profile, int first) {
    int i = first == 0 ? 1 : 0;
    for (; i <= ov00_021F955C()->players; i++) {
        if (profile == ov00_021F955C()->profiles[i]) {
            return ov00_021F955C()->aids[i];
        }
    }
    return 255;
}

u32 DWCi_GetPeerAidBitmap(int flag) {
    u32 mask = 0;
    int i;
    if (flag) {
        return ov00_021F955C()->aidMask & ~1;
    }
    for (i = 1; i <= ov00_021F955C()->players; i++) {
        mask |= 1 << ov00_021F955C()->aids[i];
    }
    return mask;
}

void ov00_021F7DB4(void) {
    ov00_021F955C()->unk_1AD = 0;
    ov00_021F955C()->unk_1F8 = OS_GetTick();
}

void DWCi_ResetMinCompleteTimer(int flag) {
    if (!ov00_0221A928.timer || !ov00_0221A928.timer->active) {
        return;
    }
    ov00_0221A928.timer->mask = 0;
    ov00_0221A928.timer->all = 0;
    ov00_0221A928.timer->retries = 0;
    ov00_0221A928.timer->last = OS_GetTick();
    if (flag) {
        return;
    }
    ov00_0221A928.timer->start = OS_GetTick();
}

void DWCi_ProcessMinComplete(void) {
    MatchTimer *s;
    int retries, i;
    u32 mask;
    u64 elapsed;
    if (!ov00_0221A928.timer || !ov00_0221A928.timer->active) {
        return;
    }
    if (ov00_021F955C()->mode == 2) {
        return;
    }
    if (ov00_021F955C()->mode == 3) {
        return;
    }
    if (ov00_021F955C()->state == 19) {
        mask = DWCi_GetPeerAidBitmap(0);
        s = ov00_0221A928.timer;
        if (s->mask == mask) {
            if (s->all == mask) {
                ov00_021F955C()->limit = ov00_021F955C()->players;
                ov00_021F955C()->initial = ov00_021F955C()->players - 1;
                DWCi_AdvanceJoin(0);
                return;
            }
            s->last = OS_GetTick();
            s->mask = 0;
            if (ov00_021F955C()->mode == 0) {
                ov00_021F956C(3);
                ov00_021F955C()->unk_E8 = 2;
                {
                    MatchWork *w = ov00_021F955C();
                    w->unk_EC = OS_GetTick();
                }
                return;
            }
            ov00_021F956C(4);
            DWCi_SendReservationToNextFriend(1, 0, 0);
            return;
        }
        retries = s->retries;
        elapsed = ((OS_GetTick() - s->last) << 6) / 0x82ea;
        if (elapsed < (u64)(retries * 6000)) {
            return;
        }
        if ((u32)retries > 5) {
            DWCi_ResetMinCompleteTimer(1);
            DWCi_CloseAllConnectionsHard();
            DWCi_RestartMatching(1);
            return;
        }
        for (i = 1; i <= ov00_021F955C()->players; i++) {
            if (!(ov00_0221A928.timer->mask & (1 << ov00_021F955C()->aids[i]))) {
                MatchWork *a = ov00_021F955C(), *b = ov00_021F955C(), *c = ov00_021F955C();
                DWCi_SendMatchCommand(17, a->profiles[i], b->addresses[i], c->ports[i], 0, 0);
                if (DWCi_HandleMatchCommandError()) {
                    return;
                }
            }
        }
        ov00_0221A928.timer->retries++;
        return;
    }
    if (ov00_021F955C()->state != 4 && ov00_021F955C()->state != 3) {
        return;
    }
    s = ov00_0221A928.timer;
    if (ov00_021F955C()->players < s->wanted - 1) {
        return;
    }
    if (s->retries == 0) {
        s = ov00_0221A928.timer;
        elapsed = ((OS_GetTick() - s->start) << 6) / 0x82ea;
        if (elapsed >= s->timeout) {
            goto send;
        }
    }
    if (!s->retries) {
        return;
    }
    s = ov00_0221A928.timer;
    elapsed = ((OS_GetTick() - s->last) << 6) / 0x82ea;
    if (elapsed < (s->timeout >> 2)) {
        return;
    }
send:
    if (ov00_021F955C()->target) {
        DWCi_CancelReservation(ov00_021F955C()->target);
        if (DWCi_HandleMatchCommandError()) {
            return;
        }
    }
    ov00_021F956C(19);
    for (i = 1; i <= ov00_021F955C()->players; i++) {
        MatchWork *a = ov00_021F955C(), *b = ov00_021F955C(), *c = ov00_021F955C();
        DWCi_SendMatchCommand(17, a->profiles[i], b->addresses[i], c->ports[i], 0, 0);
        if (DWCi_HandleMatchCommandError()) {
            return;
        }
    }
    s = ov00_0221A928.timer;
    s->last = OS_GetTick();
    s->retries = 1;
}

int DWCi_HandleGPError(unsigned n) {
    int cat, off;
    if (!n) {
        return 0;
    }
    switch (n) {
    case 1:
        cat = 9;
        off = -1;
        break;
    case 2:
        cat = 9;
        off = -2;
        break;
    case 3:
        cat = 6;
        off = -10;
        break;
    case 4:
        cat = 6;
        off = -20;
        break;
    }
    DWCi_StopMatching(cat, off - 81000);
    return n;
}

int DWCi_HandleSBError(unsigned n) {
    int cat, off;
    if (!n) {
        return 0;
    }
    switch (n) {
    case 1:
        cat = 6;
        off = -50;
        break;
    case 2:
        cat = 6;
        off = -30;
        break;
    case 3:
        cat = 6;
        off = -20;
        break;
    case 4:
        cat = 6;
        off = -40;
        break;
    case 5:
        cat = 9;
        off = -1;
        break;
    case 6:
        cat = 9;
        off = -2;
        break;
    }
    DWCi_StopMatching(cat, off - 85000);
    return n;
}

int DWCi_HandleQR2Error(int error) {
    int code, kind;
    if (!error) {
        return 0;
    }
    switch (error) {
    case 1:
        kind = 6;
        code = kind - 0x38;
        break;
    case 2:
        kind = 6;
        code = kind - 0x42;
        break;
    case 3:
        kind = 6;
        code = kind - 0x24;
        break;
    case 4:
        kind = 6;
        code = kind - 0x56;
        break;
    case 5:
        kind = 6;
        code = kind - 0x1a;
        break;
    }
    switch (ov00_021EE568()) {
    case 2:
        ov00_021EF3C0(kind, code - 64000);
        break;
    case 4:
        ov00_021F0238(kind, code - 74000);
        break;
    case 5:
        DWCi_StopMatching(kind, code - 84000);
        break;
    default:
        ov00_021EC24C(kind, code - 94000);
        break;
    }
    return error;
}

int DWCi_HandleNNError(unsigned n) {
    int cat, off;
    if (!n) {
        return 0;
    }
    switch (n) {
    case 1:
        cat = 9;
        off = -1;
        break;
    case 2:
        cat = 6;
        off = -50;
        break;
    case 3:
        cat = 6;
        off = -30;
        break;
    }
    DWCi_StopMatching(cat, off - 86000);
    return n;
}

static inline void fail(int level) {
    register int code = level;
    if (code != 0) {
        DWCi_StopMatching(code, -86009);
    }
}

#pragma opt_propagation off

int DWCi_HandleNNResult(int a) {
    if (a == 0) {
        return 0;
    }
    switch (a) {
    case 1:
        return 1;
    case 2:
        return 2;
    default:
        fail(6);
        return a;
    }
}

#pragma opt_propagation reset

int DWCi_HandleGT2Error(unsigned n) {
    int cat, off;
    if (!n) {
        return 0;
    }
    switch (n) {
    case 1:
        cat = 9;
        off = -1;
        break;
    case 2:
    case 5:
        cat = 0;
        off = 0;
        n = 0;
        break;
    case 3:
        cat = 6;
        off = -10;
        break;
    case 4:
        cat = 6;
        off = -30;
        break;
    case 6:
        cat = 6;
        off = -70;
        break;
    case 7:
        cat = 6;
        off = -80;
        break;
    }
    if (cat) {
        DWCi_StopMatching(cat, off - 87000);
    }
    return n;
}

void DWCi_SBCallback(void *list, int event, void *arg) {
    int i, profile;
    void *item;
    MatchWork *w;
    ov00_0221A928.busy++;
    if (ov00_021F955C()->pending) {
        if ((u32)(event - 4) <= 1) {
            w = ov00_021F955C();
            w->pending = 0;
        } else {
            w = ov00_021F955C();
            w->pending = OS_GetTick() + 0xefb5f7;
        }
    }
    switch (event) {
    case 0:
        DWCi_SBServerUpdated(arg);
        break;
    case 4:
        for (i = 0; i < ServerBrowserCount(list); i++) {
            item = ServerBrowserGetServer(list, i);
            if (!DWCi_CheckServerKeys(item)) {
                ServerBrowserRemoveServer(list, item);
                i--;
            }
        }
        switch (ov00_021F955C()->state) {
        case 2:
            for (i = 0; i < ServerBrowserCount(list); i++) {
                item = ServerBrowserGetServer(list, i);
                if (ov00_021F955C()->addr && ov00_021F955C()->addr == SBServerGetPublicInetAddress(item) && ov00_021F955C()->port && ov00_021F955C()->port == SBServerGetPublicQueryPort(item)) {
                    break;
                }
            }
            if (i < ServerBrowserCount(list)) {
                ov00_021F956C(3);
                ov00_021F955C()->target = 0;
                DWCi_HandleSBError(DWCi_UpdateServerListAsync(ov00_021F955C()->target));
            } else {
                ov00_021F955C()->unk_E8 = 2;
                w = ov00_021F955C();
                w->unk_EC = OS_GetTick();
            }
            break;
        case 3:
            DWCi_EvaluateServers(1);
            DWCi_PickRandomServer();
            if (ServerBrowserCount(list)) {
                DWCi_SendReservationRequest(0, 0);
                if (DWCi_HandleMatchCommandError()) {
                    goto end;
                }
                ov00_021F956C(4);
                ov00_021F955C()->unk_E8 = 0;
            } else {
                ov00_021F955C()->unk_E8 = 2;
                w = ov00_021F955C();
                w->unk_EC = OS_GetTick();
            }
            break;
        case 5:
            if (ServerBrowserCount(list)) {
                i = 0;
                do {
                    item = ServerBrowserGetServer(list, i);
                    if (SBServerGetPublicInetAddress(item) == ov00_021F955C()->remoteAddr && SBServerGetPublicQueryPort(item) == ov00_021F955C()->remotePort) {
                        break;
                    }
                    ServerBrowserRemoveServer(list, item);
                } while (ServerBrowserCount(list));
            }
            if (ServerBrowserCount(list)) {
                profile = SBServerGetIntValueA(ServerBrowserGetServer(list, 0), ov00_02217AAC, 0);
                if (ov00_021F955C()->mode == 1 && profile == ov00_021F955C()->profiles[0]) {
                    if (DWCi_EvaluateServers(0)) {
                        if (ov00_021F955C()->players) {
                            DWCi_SendServerPeerProfiles();
                            if (DWCi_HandleMatchCommandError()) {
                                goto end;
                            }
                        }
                    } else {
                        DWCi_CancelReservation(ov00_021F955C()->profiles[0]);
                        if (DWCi_HandleMatchCommandError()) {
                            goto end;
                        }
                        ov00_021F956C(4);
                        DWCi_SendReservationToNextFriend(0, 0, ov00_021F955C()->profiles[0]);
                        DWCi_HandleMatchCommandError();
                        goto end;
                    }
                }
                ov00_021F956C(6);
                DWCi_HandleNNError(DWCi_StartNatNegotiation(0, 0, ServerBrowserGetServer(list, 0)));
            } else {
                ov00_021F955C()->unk_E8 = 2;
                w = ov00_021F955C();
                w->unk_EC = OS_GetTick();
            }
            break;
        }
        break;
    case 5:
        break;
    }
end:
    ov00_0221A928.busy--;
}

void DWCi_SBServerUpdated(void *arg) {
}

int DWCi_EvaluateServers(int sort) {
    int removed = 0, matched, value, weight;
    MatchWork *w;
    void *arg;
    int i = removed;
    if (ServerBrowserCount(ov00_021F955C()->list) > 0) {
        do {
            int j;
            void *item = ServerBrowserGetServer(ov00_021F955C()->list, i);
            if (ov00_021F955C()->mode == 0) {
                value = SBServerGetIntValueA(item, ov00_02217AAC, 0);
                matched = 0;
                for (j = 1; j <= ov00_021F955C()->players; j++) {
                    if (value == ov00_021F955C()->profiles[j]) {
                        ServerBrowserRemoveServer(ov00_021F955C()->list, item);
                        i--;
                        matched = 1;
                        break;
                    }
                }
                if (matched) {
                    goto advance;
                }
            }
            if (ov00_021F955C()->evalCallback) {
                w = ov00_021F955C();
                arg = ov00_021F955C()->evalParam;
                weight = w->evalCallback(i, arg);
                if (weight > 0) {
                    if (weight > 0x7fffff) {
                        weight = 0x7fffff;
                    }
                    SBServerAddIntKeyValue(item, ov00_02217AD8, ov00_021ED5E4(256) | (weight << 8));
                } else {
                    ServerBrowserRemoveServer(ov00_021F955C()->list, item);
                    i--;
                    removed = 1;
                }
            } else {
                SBServerAddIntKeyValue(item, ov00_02217AD8, ov00_021ED5E4(128));
            }
advance:
            i++;
        } while (i < ServerBrowserCount(ov00_021F955C()->list));
    }
    if (sort && ServerBrowserCount(ov00_021F955C()->list) != 0) {
        ServerBrowserSortA(ov00_021F955C()->list, 0, ov00_02217AD8, 0);
    }
    if (removed && ServerBrowserCount(ov00_021F955C()->list) == 0) {
        return 0;
    }
    return 1;
}

static inline u32 combine(u32 q, u32 p) {
    return q + p;
}

void DWCi_PickRandomServer(void) {
    u32 random;
    int cumulative[6];
    int largest = 0, sum = 0, i;
    u32 prior;
    if (ServerBrowserCount(ov00_021F955C()->list) <= 1) {
        return;
    }
    for (i = 0; i < ServerBrowserCount(ov00_021F955C()->list); i++) {
        int value = SBServerGetIntValueA(ServerBrowserGetServer(ov00_021F955C()->list, i), ov00_02217AD8, -1);
        if (value > largest) {
            largest = value;
        }
        sum += ov00_02216040[i];
    }
    random = ov00_021ED5E4(100);
    for (i = 0; i < ServerBrowserCount(ov00_021F955C()->list); i++) {
        if (i == ServerBrowserCount(ov00_021F955C()->list) - 1) {
            break;
        }
        if (i > 0) {
            prior = cumulative[i - 1];
        } else {
            prior = 0;
        }
        cumulative[i] = combine(ov00_02216040[i] * 100 / sum, prior);
        if (random < (u32)cumulative[i]) {
            break;
        }
    }
    if (largest < 0x7fffffff) {
        largest++;
    }
    SBServerAddIntKeyValue(ServerBrowserGetServer(ov00_021F955C()->list, i), ov00_02217AD8, largest);
    ServerBrowserSortA(ov00_021F955C()->list, 0, ov00_02217AD8, 0);
}

void DWCi_QR2ServerKeyCallback(int id, void *out) {
    int index;
    switch (id) {
    case 8:
        qr2_buffer_add_int(out, ov00_021F955C()->connections);
        return;
    case 10:
        qr2_buffer_add_int(out, ov00_021F955C()->limit);
        return;
    case 50:
        qr2_buffer_add_int(out, ov00_021F955C()->identity);
        return;
    case 51:
        qr2_buffer_add_int(out, ov00_021F955C()->mode);
        return;
    case 52:
        qr2_buffer_add_int(out, ov00_021F955C()->unk_20);
        return;
    case 53:
        qr2_buffer_add_int(out, 3);
        return;
    case 54:
        qr2_buffer_add_int(out, 1);
        return;
    }
    index = id - 100;
    if (index < 0) {
        return;
    }
    if (index >= 154) {
        return;
    }
    index *= 12;
    if (!*((u8 *)ov00_0221A948 + index)) {
        return;
    }
    if (ov00_0221A949[index]) {
        qr2_buffer_addA(out, *(void **)((u8 *)ov00_0221A950 + index));
    } else {
        qr2_buffer_add_int(out, *(int *)*(void **)((u8 *)ov00_0221A950 + index));
    }
}

void DWCi_QR2PlayerKeyCallback(void) {
}

void DWCi_QR2TeamKeyCallback(void) {
}

void DWCi_QR2KeyListCallback(int state, void *p) {
    int i;
    MatchKeyEntry *e;
    switch (state) {
    case 0:
        break;
    case 1:
    case 2:
    default:
        return;
    }
    qr2_keybuffer_add(p, 8);
    qr2_keybuffer_add(p, 10);
    qr2_keybuffer_add(p, 50);
    qr2_keybuffer_add(p, 51);
    qr2_keybuffer_add(p, 52);
    qr2_keybuffer_add(p, 53);
    qr2_keybuffer_add(p, 54);
    e = ov00_0221A948;
    for (i = 0; i < 154; i++, e++) {
        if (e->id) {
            qr2_keybuffer_add(p, e->id);
        }
    }
}

int DWCi_QR2CountCallback(void) {
    return 0;
}

int DWCi_QR2AddErrorCallback(unsigned n) {
    return DWCi_HandleQR2Error(n);
}

void DWCi_QR2PublicAddressCallback(u32 a, u16 b) {
    ov00_021F955C()->addr = a;
    ov00_021F955C()->port = b;
}

void DWCi_QR2NatNegCallback(int profile) {
    if (ov00_021F955C()->state == 1) {
        ov00_021F956C(6);
    } else if (ov00_021F955C()->state != 6 && ov00_021F955C()->state != 11) {
        return;
    }
    if (profile == ov00_021F955C()->retryProfile) {
        ov00_021F955C()->retryCount++;
    } else {
        ov00_021F955C()->retryCount = 0;
        ov00_021F955C()->retryProfile = profile;
    }
    ov00_021F955C()->retryTick = 0;
    if (DWCi_HandleNNError(DWCi_StartNatNegotiation(1, profile, 0))) {
        return;
    }
    ov00_021F955C()->unk_3CC = 255;
}

void DWCi_QR2ClientMessageCallback(void *src, u32 size) {
    MatchCommandHeader pkt;
    int data[32];
    u32 used = 0;
    if (ov00_021EE568() != 5) {
        if (ov00_021EE568() != 6) {
            return;
        }
        if (ov00_021F955C()->mode != 2 && ov00_021F955C()->mode != 3) {
            return;
        }
    }
    if (size < 20) {
        return;
    }
    do {
        MI_CpuCopy8(src, &pkt, 20);
        if (strncmp(pkt.magic, ov00_02217B54, 4) != 0 || pkt.version != 3) {
            return;
        }
        MI_CpuCopy8((char *)src + 20, data, pkt.size);
        if (!DWCi_ProcessMatchCommand(pkt.type, pkt.arg1, pkt.arg0, pkt.value, data, pkt.size >> 2)) {
            return;
        }
        used += pkt.size + 20;
    } while (used + 20 <= size);
}

void DWCi_NNProgressCallback(void) {
}

void DWCi_NNCompletedCallback(int result, int unused, MatchAddr *addr, MatchRequest *req) {
    int index, type, ret;
    MatchWork *w, *a, *b;
    u32 remote;
    u16 port;
    char buffer[12];
    if (ov00_021F955C()->state != 6 && ov00_021F955C()->state != 11) {
        return;
    }
    if (!req) {
        return;
    }
    if (!result) {
        req->active = 0;
        ov00_021F955C()->connections++;
        index = ov00_021F955C()->connections;
        if (req->flag) {
            ov00_021F955C()->addr2[index] = addr->address;
            port = addr->port;
            ov00_021F955C()->port2[index] = ((port >> 8) & 255) | ((port << 8) & 0xff00);
            ov00_021F955C()->retryCount = 0;
            ov00_021F955C()->retryProfile = 0;
            w = ov00_021F955C();
            w->retryTick = 0;
            if (ov00_021F955C()->state == 11) {
                ov00_021F956C(12);
            } else {
                ov00_021F956C(7);
            }
            ov00_021F955C()->flag = 0;
            OS_SNPrintf(buffer, 12, ov00_02217A64, ov00_021F955C()->identity);
            w = ov00_021F955C();
            a = ov00_021F955C();
            b = ov00_021F955C();
            remote = gt2AddressToString(a->addr2[index], b->port2[index], 0);
            a = ov00_021F955C();
            ret = gt2Connect(*w->peer, 0, remote, buffer, -1, 5000, a->callback, 0);
            if (ret == 1) {
                DWCi_HandleGT2Error(ret);
                return;
            }
            if (!ret) {
                return;
            }
            if (DWCi_DropConnectingPeer(ov00_021F955C()->profiles[index])) {
                return;
            }
            return;
        }
        if (addr) {
            index--;
            ov00_021F955C()->addr2[index] = addr->address;
            port = addr->port;
            ov00_021F955C()->port2[index] = ((port >> 8) & 255) | ((port << 8) & 0xff00);
        }
        w = ov00_021F955C();
        w->waitTick = OS_GetTick();
        ov00_021F956C(7);
        return;
    }
    if (!req->active) {
        return;
    }
    type = DWCi_HandleNNResult(result);
    if (type != 2 && type != 1) {
        return;
    }
    if (!req->flag) {
        if (type == 1 || (type == 2 && req->retries >= 1)) {
            req->active = 0;
            if (!ov00_021F94DC(0)) {
                return;
            }
            w = ov00_021F955C();
            if (DWCi_HandleReservationRefused(w->profiles[ov00_021F955C()->players])) {
                return;
            }
            return;
        }
        req->retries++;
        if (DWCi_HandleNNError(DWCi_DoNatNegotiationAsync(req))) {
            return;
        }
        return;
    }
    w = ov00_021F955C();
    w->retryTick = OS_GetTick();
    switch (type) {
    case 1:
        break;
    case 2:
        if (ov00_021F955C()->retryCount < 1) {
            return;
        }
        break;
    default:
        return;
    }
    req->active = 0;
    if (ov00_021F955C()->mode == 3 || ov00_021F955C()->mode == 2) {
        if (!ov00_021F94DC(1)) {
            return;
        }
    } else if (!ov00_021F94DC(0)) {
        return;
    }
    ov00_021F955C()->retryCount = 0;
    ov00_021F955C()->retryProfile = 0;
    w = ov00_021F955C();
    w->retryTick = 0;
    w = ov00_021F955C();
    if (DWCi_DropConnectingPeer(w->profiles[ov00_021F955C()->players + 1])) {
        return;
    }
}
