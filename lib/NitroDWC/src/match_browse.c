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
extern void ov00_021F2EE4(int, int);
extern void ov00_021F4304(int, int, u32, u16, int, int);
extern int ov00_0220D018(void *, int, u32, char *, int, int, void *, int);
extern u32 ov00_0220F724(u32, u16, int);
extern void ov00_02210D9C(void *, int);
extern void ov00_02210DC8(void *, int);
extern void ov00_02210DFC(void *, void *);
extern void ov00_02212D10(void *, const void *, int);
extern int ov00_02212DB4(void *, const void *, int);
extern u32 ov00_02212E78(void *);
extern u16 ov00_02212E80(void *);
extern void ov00_0221390C(void *, void *);
extern void *ov00_022139B8(void *, int);
extern int ov00_022139C8(void *);
extern void ov00_022139D8(void *, int, const void *, int);

u8 ov00_021F7A98(void);
void ov00_021F7AD8(void);
u8 ov00_021F7B30(void);
int ov00_021F7BA0(int profile);
int ov00_021F7C10(void *server);
u8 ov00_021F7CEC(u32 profile, int first);
u32 ov00_021F7D50(int flag);
void ov00_021F7DB4(void);
void ov00_021F7DDC(int flag);
void ov00_021F7E4C(void);
int ov00_021F8278(unsigned error);
int ov00_021F82EC(unsigned error);
int ov00_021F8380(int error);
int ov00_021F8468(unsigned error);
int ov00_021F84C4(int result);
int ov00_021F8514(unsigned error);
void ov00_021F85B8(void *list, int event, void *arg);
void ov00_021F89CC(void *arg);
int ov00_021F89D0(int sort);
void ov00_021F8BA8(void);
void ov00_021F8D18(int id, void *out);
void ov00_021F8E60(void);
void ov00_021F8E64(void);
void ov00_021F8E68(int state, void *p);
int ov00_021F8F0C(void);
int ov00_021F8F14(unsigned error);
void ov00_021F8F20(u32 addr, u16 port);
void ov00_021F8F40(int profile);
void ov00_021F8FF4(void *src, u32 size);
void ov00_021F9100(void);
void ov00_021F9104(int result, int unused, MatchAddr *addr, MatchRequest *req);

u8 ov00_021F7A98(void) {
    u8 i;
    for (i = 0; i < 154; i++) {
        if (!ov00_0221A948[i].id) {
            return i + 100;
        }
    }
    return 0;
}

void ov00_021F7AD8(void) {
    int i;
    MatchKeyEntry *p = ov00_0221A948;
    for (i = 0; i < 154; i++, p++) {
        if (p->buffer) {
            ov00_021EC2EC(4, p->buffer, 0);
        }
    }
    MIi_CpuClear32(0, (u32 *)ov00_0221A948, 1848);
}

u8 ov00_021F7B30(void) {
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

int ov00_021F7BA0(int profile) {
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

int ov00_021F7C10(void *p) {
    if (ov00_02212DB4(p, ov00_02217B70, -1) == -1) {
        return 0;
    }
    if (ov00_02212DB4(p, ov00_02217B7C, -1) == -1) {
        return 0;
    }
    if (ov00_02212DB4(p, ov00_02217AB4, -1) == -1) {
        return 0;
    }
    if (ov00_02212DB4(p, ov00_02217AC0, -1) == -1) {
        if (ov00_02212DB4(p, ov00_02217AC0, 0) == 0) {
            return 0;
        }
    }
    if (ov00_02212DB4(p, ov00_02217ACC, -1) == -1) {
        return 0;
    }
    return ov00_02212DB4(p, ov00_02217AAC, 0);
}

u8 ov00_021F7CEC(u32 profile, int first) {
    int i = first == 0 ? 1 : 0;
    for (; i <= ov00_021F955C()->players; i++) {
        if (profile == ov00_021F955C()->profiles[i]) {
            return ov00_021F955C()->aids[i];
        }
    }
    return 255;
}

u32 ov00_021F7D50(int flag) {
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

void ov00_021F7DDC(int flag) {
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

void ov00_021F7E4C(void) {
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
        mask = ov00_021F7D50(0);
        s = ov00_0221A928.timer;
        if (s->mask == mask) {
            if (s->all == mask) {
                ov00_021F955C()->limit = ov00_021F955C()->players;
                ov00_021F955C()->initial = ov00_021F955C()->players - 1;
                ov00_021F64C0(0);
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
            ov00_021F5D0C(1, 0, 0);
            return;
        }
        retries = s->retries;
        elapsed = ((OS_GetTick() - s->last) << 6) / 0x82ea;
        if (elapsed < (u64)(retries * 6000)) {
            return;
        }
        if ((u32)retries > 5) {
            ov00_021F7DDC(1);
            ov00_021F7128();
            ov00_021F6EC4(1);
            return;
        }
        for (i = 1; i <= ov00_021F955C()->players; i++) {
            if (!(ov00_0221A928.timer->mask & (1 << ov00_021F955C()->aids[i]))) {
                MatchWork *a = ov00_021F955C(), *b = ov00_021F955C(), *c = ov00_021F955C();
                ov00_021F4304(17, a->profiles[i], b->addresses[i], c->ports[i], 0, 0);
                if (ov00_021F5B30()) {
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
        ov00_021F60AC(ov00_021F955C()->target);
        if (ov00_021F5B30()) {
            return;
        }
    }
    ov00_021F956C(19);
    for (i = 1; i <= ov00_021F955C()->players; i++) {
        MatchWork *a = ov00_021F955C(), *b = ov00_021F955C(), *c = ov00_021F955C();
        ov00_021F4304(17, a->profiles[i], b->addresses[i], c->ports[i], 0, 0);
        if (ov00_021F5B30()) {
            return;
        }
    }
    s = ov00_0221A928.timer;
    s->last = OS_GetTick();
    s->retries = 1;
}

int ov00_021F8278(unsigned n) {
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
    ov00_021F2EE4(cat, off - 81000);
    return n;
}

int ov00_021F82EC(unsigned n) {
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
    ov00_021F2EE4(cat, off - 85000);
    return n;
}

int ov00_021F8380(int error) {
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
        ov00_021F2EE4(kind, code - 84000);
        break;
    default:
        ov00_021EC24C(kind, code - 94000);
        break;
    }
    return error;
}

int ov00_021F8468(unsigned n) {
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
    ov00_021F2EE4(cat, off - 86000);
    return n;
}

static inline void fail(int level) {
    register int code = level;
    if (code != 0) {
        ov00_021F2EE4(code, -86009);
    }
}

#pragma opt_propagation off

int ov00_021F84C4(int a) {
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

int ov00_021F8514(unsigned n) {
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
        ov00_021F2EE4(cat, off - 87000);
    }
    return n;
}

void ov00_021F85B8(void *list, int event, void *arg) {
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
        ov00_021F89CC(arg);
        break;
    case 4:
        for (i = 0; i < ov00_022139C8(list); i++) {
            item = ov00_022139B8(list, i);
            if (!ov00_021F7C10(item)) {
                ov00_0221390C(list, item);
                i--;
            }
        }
        switch (ov00_021F955C()->state) {
        case 2:
            for (i = 0; i < ov00_022139C8(list); i++) {
                item = ov00_022139B8(list, i);
                if (ov00_021F955C()->addr && ov00_021F955C()->addr == ov00_02212E78(item) && ov00_021F955C()->port && ov00_021F955C()->port == ov00_02212E80(item)) {
                    break;
                }
            }
            if (i < ov00_022139C8(list)) {
                ov00_021F956C(3);
                ov00_021F955C()->target = 0;
                ov00_021F82EC(ov00_021F3CF8(ov00_021F955C()->target));
            } else {
                ov00_021F955C()->unk_E8 = 2;
                w = ov00_021F955C();
                w->unk_EC = OS_GetTick();
            }
            break;
        case 3:
            ov00_021F89D0(1);
            ov00_021F8BA8();
            if (ov00_022139C8(list)) {
                ov00_021F5B5C(0, 0);
                if (ov00_021F5B30()) {
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
            if (ov00_022139C8(list)) {
                i = 0;
                do {
                    item = ov00_022139B8(list, i);
                    if (ov00_02212E78(item) == ov00_021F955C()->remoteAddr && ov00_02212E80(item) == ov00_021F955C()->remotePort) {
                        break;
                    }
                    ov00_0221390C(list, item);
                } while (ov00_022139C8(list));
            }
            if (ov00_022139C8(list)) {
                profile = ov00_02212DB4(ov00_022139B8(list, 0), ov00_02217AAC, 0);
                if (ov00_021F955C()->mode == 1 && profile == ov00_021F955C()->profiles[0]) {
                    if (ov00_021F89D0(0)) {
                        if (ov00_021F955C()->players) {
                            ov00_021F63DC();
                            if (ov00_021F5B30()) {
                                goto end;
                            }
                        }
                    } else {
                        ov00_021F60AC(ov00_021F955C()->profiles[0]);
                        if (ov00_021F5B30()) {
                            goto end;
                        }
                        ov00_021F956C(4);
                        ov00_021F5D0C(0, 0, ov00_021F955C()->profiles[0]);
                        ov00_021F5B30();
                        goto end;
                    }
                }
                ov00_021F956C(6);
                ov00_021F8468(ov00_021F3F5C(0, 0, ov00_022139B8(list, 0)));
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

void ov00_021F89CC(void *arg) {
}

int ov00_021F89D0(int sort) {
    int removed = 0, matched, value, weight;
    MatchWork *w;
    void *arg;
    int i = removed;
    if (ov00_022139C8(ov00_021F955C()->list) > 0) {
        do {
            int j;
            void *item = ov00_022139B8(ov00_021F955C()->list, i);
            if (ov00_021F955C()->mode == 0) {
                value = ov00_02212DB4(item, ov00_02217AAC, 0);
                matched = 0;
                for (j = 1; j <= ov00_021F955C()->players; j++) {
                    if (value == ov00_021F955C()->profiles[j]) {
                        ov00_0221390C(ov00_021F955C()->list, item);
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
                    ov00_02212D10(item, ov00_02217AD8, ov00_021ED5E4(256) | (weight << 8));
                } else {
                    ov00_0221390C(ov00_021F955C()->list, item);
                    i--;
                    removed = 1;
                }
            } else {
                ov00_02212D10(item, ov00_02217AD8, ov00_021ED5E4(128));
            }
advance:
            i++;
        } while (i < ov00_022139C8(ov00_021F955C()->list));
    }
    if (sort && ov00_022139C8(ov00_021F955C()->list) != 0) {
        ov00_022139D8(ov00_021F955C()->list, 0, ov00_02217AD8, 0);
    }
    if (removed && ov00_022139C8(ov00_021F955C()->list) == 0) {
        return 0;
    }
    return 1;
}

static inline u32 combine(u32 q, u32 p) {
    return q + p;
}

void ov00_021F8BA8(void) {
    u32 random;
    int cumulative[6];
    int largest = 0, sum = 0, i;
    u32 prior;
    if (ov00_022139C8(ov00_021F955C()->list) <= 1) {
        return;
    }
    for (i = 0; i < ov00_022139C8(ov00_021F955C()->list); i++) {
        int value = ov00_02212DB4(ov00_022139B8(ov00_021F955C()->list, i), ov00_02217AD8, -1);
        if (value > largest) {
            largest = value;
        }
        sum += ov00_02216040[i];
    }
    random = ov00_021ED5E4(100);
    for (i = 0; i < ov00_022139C8(ov00_021F955C()->list); i++) {
        if (i == ov00_022139C8(ov00_021F955C()->list) - 1) {
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
    ov00_02212D10(ov00_022139B8(ov00_021F955C()->list, i), ov00_02217AD8, largest);
    ov00_022139D8(ov00_021F955C()->list, 0, ov00_02217AD8, 0);
}

void ov00_021F8D18(int id, void *out) {
    int index;
    switch (id) {
    case 8:
        ov00_02210DC8(out, ov00_021F955C()->connections);
        return;
    case 10:
        ov00_02210DC8(out, ov00_021F955C()->limit);
        return;
    case 50:
        ov00_02210DC8(out, ov00_021F955C()->identity);
        return;
    case 51:
        ov00_02210DC8(out, ov00_021F955C()->mode);
        return;
    case 52:
        ov00_02210DC8(out, ov00_021F955C()->unk_20);
        return;
    case 53:
        ov00_02210DC8(out, 3);
        return;
    case 54:
        ov00_02210DC8(out, 1);
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
        ov00_02210DFC(out, *(void **)((u8 *)ov00_0221A950 + index));
    } else {
        ov00_02210DC8(out, *(int *)*(void **)((u8 *)ov00_0221A950 + index));
    }
}

void ov00_021F8E60(void) {
}

void ov00_021F8E64(void) {
}

void ov00_021F8E68(int state, void *p) {
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
    ov00_02210D9C(p, 8);
    ov00_02210D9C(p, 10);
    ov00_02210D9C(p, 50);
    ov00_02210D9C(p, 51);
    ov00_02210D9C(p, 52);
    ov00_02210D9C(p, 53);
    ov00_02210D9C(p, 54);
    e = ov00_0221A948;
    for (i = 0; i < 154; i++, e++) {
        if (e->id) {
            ov00_02210D9C(p, e->id);
        }
    }
}

int ov00_021F8F0C(void) {
    return 0;
}

int ov00_021F8F14(unsigned n) {
    return ov00_021F8380(n);
}

void ov00_021F8F20(u32 a, u16 b) {
    ov00_021F955C()->addr = a;
    ov00_021F955C()->port = b;
}

void ov00_021F8F40(int profile) {
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
    if (ov00_021F8468(ov00_021F3F5C(1, profile, 0))) {
        return;
    }
    ov00_021F955C()->unk_3CC = 255;
}

void ov00_021F8FF4(void *src, u32 size) {
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
        if (!ov00_021F4740(pkt.type, pkt.arg1, pkt.arg0, pkt.value, data, pkt.size >> 2)) {
            return;
        }
        used += pkt.size + 20;
    } while (used + 20 <= size);
}

void ov00_021F9100(void) {
}

void ov00_021F9104(int result, int unused, MatchAddr *addr, MatchRequest *req) {
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
            remote = ov00_0220F724(a->addr2[index], b->port2[index], 0);
            a = ov00_021F955C();
            ret = ov00_0220D018(*w->peer, 0, remote, buffer, -1, 5000, a->callback, 0);
            if (ret == 1) {
                ov00_021F8514(ret);
                return;
            }
            if (!ret) {
                return;
            }
            if (ov00_021F6100(ov00_021F955C()->profiles[index])) {
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
    type = ov00_021F84C4(result);
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
            if (ov00_021F631C(w->profiles[ov00_021F955C()->players])) {
                return;
            }
            return;
        }
        req->retries++;
        if (ov00_021F8468(ov00_021F4248(req))) {
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
    if (ov00_021F6100(w->profiles[ov00_021F955C()->players + 1])) {
        return;
    }
}
