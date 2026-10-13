#pragma force_active on
#include "global.h"

#include "dwc/match_internal.h"

// The code below is compiled as ARM (32-bit) code.
#include <nitro/code32.h>

#pragma warn_uninitializedvar off

// Finishing and closing a match: the join state machine after a peer was added, cancelling the matchmaking,
// restarting it, closing the peer connections, the aid messages of the "all peers ready" exchange and the
// close exchange of the server (commands 13 to 15).

extern const char ov00_02217A60[]; // ""

extern void ov00_021EE31C(int index);
extern int ov00_021EE568(void);
extern void **ov00_021EE73C(int profile, int aid);
extern int ov00_021F0404(int profile);
extern int ov00_021F04A0(int type, const void *message, int arg);
extern int ov00_021F4304(int command, u32 profile, u32 addr, u32 port, u32 *data, int count);
extern int ov00_021F7BA0(int profile);
extern int ov00_021F7CEC(int profile, int first);
extern u32 ov00_021F7D50(int flag);
extern void ov00_021F7DB4(void);
extern int ov00_021F9630(int flag, int aid, const void *p, int n);
extern void gt2CloseConnectionHard(void *connection);
extern void gt2CloseAllConnectionsHard(void *connection);
extern void ov00_0221019C(void *p);
extern void ov00_02210CBC(void *reporting);
extern void ov00_022136E4(void *list);
extern void ov00_0221396C(void *list);

// The connection to the server (profile) was refused: a client looks for another match.
int ov00_021F631C(int profile) {
    int result = 1;
    MatchWork *w;
    if (ov00_021F955C()->mode == 3) {
        if (ov00_021F955C()->players) {
            ov00_021F7128();
        }
        ov00_021F2EE4(6, -80430);
        return 0;
    }
    w = ov00_021F955C();
    w->connections = ov00_021F955C()->players;
    ov00_021F955C()->unk_208 = 0;
    if (ov00_021F955C()->request.active) {
        ov00_0221019C((void *)ov00_021F955C()->request.active);
        ov00_021F955C()->request.active = 0;
    }
    if (ov00_021F955C()->players) {
        ov00_021F6FC4();
    } else {
        ov00_021F956C(4);
        result = ov00_021F5FD4(0);
    }
    return result;
}

// Sends the profiles of the server's peers (command 10) to every peer and closes the connection to the server.
int ov00_021F63DC(void) {
    int i = 1, result;
    while (i <= ov00_021F955C()->players) {
        MatchWork *a = ov00_021F955C(), *b = ov00_021F955C(), *c = ov00_021F955C(), *d = ov00_021F955C(), *e = ov00_021F955C();
        result = ov00_021F4304(10, a->profiles[i], b->addresses[i], c->ports[i], &d->unk_348, e->unk_348 + 1);
        if (result) {
            return result;
        }
        i++;
    }
    ov00_021F955C()->unk_17 = 0;
    ov00_021F955C()->unk_20 = 0;
    ov00_021F955C()->closeState = 1;
    gt2CloseAllConnectionsHard(*ov00_021F955C()->peer);
    ov00_021F955C()->closeState = 0;
    return 0;
}

// Next step of the join: 0 = tell the next peer about the new one (command 8) or finish, 1-3 = the new peer
// is connected, 4 = the match is complete.
void ov00_021F64C0(unsigned action) {
    int count = 3, notify = 0;
    u32 message[5];
    int i;
    switch (action) {
    case 0:
        if (ov00_021F955C()->initial < ov00_021F955C()->players - 1) {
            ov00_021F956C(13);
            message[0] = ov00_021F955C()->profiles[ov00_021F955C()->initial + 1];
            message[1] = ov00_021F955C()->initial + 1;
            message[2] = ov00_021F955C()->aids[ov00_021F955C()->initial + 1];
            message[3] = ov00_021F955C()->addresses[ov00_021F955C()->initial + 1];
            message[4] = ov00_021F955C()->ports[ov00_021F955C()->initial + 1];
            count = 5;
        } else {
            ov00_021F955C()->unk_17 = 0;
            ov00_021F955C()->unk_20 = 0;
            ov00_02210CBC(ov00_021F955C()->reporting);
            if (ov00_021F955C()->mode == 0) {
                ov00_021F956C(count);
            } else if (ov00_021F955C()->mode == 1) {
                ov00_021F956C(4);
            } else {
                ov00_021F956C(10);
            }
            ov00_021F955C()->initial = 0;
            if (ov00_021F955C()->mode == 2 || ov00_021F955C()->players == ov00_021F955C()->limit) {
                if (ov00_021F955C()->mode == 2) {
                    MatchWork *w = ov00_021F955C();
                    w->unk_20C = ov00_021F955C()->profiles[ov00_021F955C()->players];
                } else {
                    ov00_021F955C()->unk_20C = 0;
                    ov00_021F955C()->profiles[0] = ov00_021F955C()->identity;
                }
                ov00_021F956C(16);
                ov00_021F955C()->readyMask = 0;
                for (i = 1; i <= ov00_021F955C()->players; i++) {
                    ov00_021F7294(ov00_021F955C()->aids[i], 2);
                }
            } else {
                message[0] = 0;
                message[1] = ov00_021F955C()->players;
                message[2] = ov00_021F955C()->aids[ov00_021F955C()->players];
                if (ov00_021F955C()->mode == 0) {
                    ov00_021F955C()->unk_E8 = 2;
                    ov00_021F955C()->unk_EC = OS_GetTick();
                } else if (ov00_021F955C()->mode == 1) {
                    ov00_021F5D0C(1, 0, 0);
                    if (!ov00_021F955C()->unk_1A6 && ov00_021F955C()->players >= 2) {
                        {
                            MatchWork *w = ov00_021F955C();
                            u32 mask = ov00_021F7D50(0);
                            MatchWork *v = ov00_021F955C();
                            if (w->unk_1DC != (mask & ~(1 << v->aids[ov00_021F955C()->players]))) {
                                ov00_021F6FC4();
                                goto done;
                            }
                        }
                    }
                }
            }
            if (ov00_021F955C()->mode != 2) {
                notify = 1;
            }
        }
        if (ov00_021F955C()->state == 16) {
            break;
        }
        if (ov00_021F5B30(ov00_021F4304(8, ov00_021F955C()->profiles[ov00_021F955C()->players], ov00_021F955C()->addresses[ov00_021F955C()->players], ov00_021F955C()->ports[ov00_021F955C()->players], message, count))) {
            return;
        }
        ov00_021F955C()->commandRetries = 0;
        break;
    case 1:
        ov00_021F956C(1);
        if (ov00_021F955C()->mode == 3) {
            ov00_021F955C()->unk_20C = ov00_021F955C()->profiles[ov00_021F955C()->players];
        }
        notify = 1;
        break;
    case 2:
        ov00_021F956C(1);
        if (ov00_021F955C()->mode == 0 || ov00_021F955C()->mode == 1) {
            ov00_021F955C()->unk_17 = 1;
            ov00_021F955C()->unk_20 = ov00_021F955C()->identity;
        }
        ov00_021F7DB4();
        if (ov00_021F955C()->players > 1) {
            if (ov00_021F5B30(ov00_021F4304(9, ov00_021F955C()->profiles[0], ov00_021F955C()->addresses[0], ov00_021F955C()->ports[0], (u32 *)&ov00_021F955C()->profiles[ov00_021F955C()->players - 1], 1))) {
                return;
            }
        }
        break;
    case 3:
        ov00_021F956C(1);
        ov00_021F7DB4();
        ov00_021F955C()->unk_20C = 0;
        notify = 1;
        break;
    case 4: {
        int noProfile, friendIndex;
        MatchWork *w;
        if (ov00_021F955C()->mode != 2) {
            ov00_021F04A0(2, ov00_02217A60, notify);
        }
        noProfile = ov00_021F955C()->unk_20C == 0;
        w = ov00_021F955C();
        friendIndex = ov00_021F0404(ov00_021F955C()->unk_20C);
        w->errorCallback(0, 0, noProfile, 0, friendIndex, ov00_021F955C()->errorParam);
        if (ov00_021F955C()->mode == 0 || ov00_021F955C()->mode == 1) {
            ov00_021F3C5C();
        } else {
            if (ov00_021F955C()->list) {
                ov00_022136E4(ov00_021F955C()->list);
                ov00_021F955C()->list = NULL;
            }
            ov00_021F125C();
            if (ov00_021F955C()->mode == 2) {
                if (ov00_021F8278(ov00_021F36BC())) {
                    return;
                }
                if (ov00_0221A928.unk_1C == 1) {
                    ov00_0221A928.unk_1D = 1;
                }
                ov00_021F956C(10);
            } else {
                ov00_021F956C(1);
            }
            ov00_021F955C()->unk_20C = 0;
        }
        ov00_021F955C()->unk_1A9 = 0;
        break;
    }
    }
done:
    if (notify && ov00_021F955C()->mode != 3) {
        ov00_0221396C(ov00_021F955C()->list);
    }
}

// Checks that every profile is a friend (or a friend of a friend) we may play with.
int ov00_021F6B20(int *profiles, u32 count) {
    u32 i;
    if (ov00_021F955C()->unk_1A6 && ov00_021F955C()->state == 4) {
        return 1;
    }
    for (i = 0; i < count; i++) {
        if (!ov00_021F7BA0(profiles[i])) {
            return 0;
        }
        if (ov00_021F955C()->unk_1A6 && ov00_021F955C()->state == 1) {
            return 1;
        }
    }
    return 1;
}

// Cancels the matchmaking.
void ov00_021F6BA8(void) {
    ov00_021F955C()->unk_20C = 0;
    if (ov00_021F955C()->state == 2) {
        ov00_021F6D30();
        return;
    }
    ov00_021F955C()->unk_1A9 = 1;
    if (ov00_021F955C()->mode == 3) {
        ov00_021F955C()->unk_1A9 = 1;
        if (ov00_021F955C()->players) {
            gt2CloseAllConnectionsHard(*ov00_021F955C()->peer);
        } else if (ov00_021F5B30(ov00_021F60AC(ov00_021F955C()->profiles[0]))) {
            return;
        }
        ov00_021F6D30();
        return;
    }
    if (ov00_021F955C()->state == 4 || ov00_021F955C()->state == 5 || ov00_021F955C()->state == 6 || ov00_021F955C()->state == 7 || ov00_021F955C()->state == 11 || ov00_021F955C()->state == 12) {
        if (ov00_021F955C()->target && ov00_021F5B30(ov00_021F60AC(ov00_021F955C()->profiles[0]))) {
            return;
        }
        if (ov00_021F5B30(ov00_021F6DF0())) {
            return;
        }
    }
    if (ov00_021F955C()->request.active) {
        ov00_0221019C((void *)ov00_021F955C()->request.active);
        ov00_021F955C()->request.active = 0;
    }
    if (ov00_021F955C()->players) {
        ov00_021F955C()->unk_1A9 = 1;
        gt2CloseAllConnectionsHard(*ov00_021F955C()->peer);
    }
    ov00_021F6D30();
}

void ov00_021F6D30(void) {
    int isServer, noProfile, friendIndex;
    MatchWork *w;
    if (ov00_021F8278(ov00_021F04A0(1, ov00_02217A60, 0))) {
        return;
    }
    ov00_021F3C5C();
    if (ov00_021F955C()->unk_20C) {
        isServer = 1;
    } else {
        isServer = ov00_021F955C()->mode == 2;
    }
    noProfile = ov00_021F955C()->unk_20C == 0;
    w = ov00_021F955C();
    friendIndex = ov00_021F0404(ov00_021F955C()->unk_20C);
    {
        MatchWork *v = ov00_021F955C();
        w->errorCallback(0, 1, noProfile, isServer, friendIndex, v->errorParam);
    }
    ov00_021F955C()->unk_1A9 = 0;
}

// Tells the server we are connected to that we leave (command 12).
int ov00_021F6DF0(void) {
    int result = 0, zero = 0;
    if (ov00_021F955C()->unk_17 && ov00_021F955C()->unk_20) {
        MatchWork *w = ov00_021F955C();
        if (w->unk_20 != ov00_021F955C()->identity) {
            MatchWork *a = ov00_021F955C(), *b = ov00_021F955C(), *c = ov00_021F955C(), *d = ov00_021F955C();
            result = ov00_021F4304(12, a->unk_20, b->addresses[c->players + 1], d->ports[ov00_021F955C()->players + 1], (u32 *)&zero, 1);
            ov00_021F955C()->unk_17 = 0;
            ov00_021F955C()->unk_20 = 0;
        }
    }
    return result;
}

// Restarts the matchmaking (mode 0: report the end to the error callback instead).
void ov00_021F6EC4(int mode) {
    if (!mode) {
        ov00_021F6D30();
        return;
    }
    ov00_021F381C(mode);
    if (ov00_021F955C()->mode == 2 || ov00_021F955C()->mode == 3) {
        int noProfile = ov00_021F955C()->unk_20C == 0;
        MatchWork *w = ov00_021F955C();
        int friendIndex = ov00_021F0404(ov00_021F955C()->unk_20C);
        w->errorCallback(0, 1, noProfile, 0, friendIndex, ov00_021F955C()->errorParam);
        return;
    }
    if (ov00_021F955C()->mode == 0) {
        if (mode == 1) {
            if (ov00_021F82EC(ov00_021F3CF8(0))) {
                return;
            }
        }
    } else if (ov00_021F955C()->mode == 1 && mode == 1) {
        ov00_021F5D0C(0, 0, 0);
    }
}

void ov00_021F6FC4(void) {
    if (ov00_021F955C()->mode == 2) {
        return;
    }
    if (ov00_021F955C()->mode == 3) {
        return;
    }
    ov00_021F7128();
    ov00_021F6EC4(1);
}

int ov00_021F6FF8(void) {
    int result;
    ov00_021F955C()->unk_17 = 0;
    ov00_021F955C()->unk_20 = 0;
    ov00_021F955C()->closeState = 0;
    if (ov00_021F955C()->unk_208) {
        if (!ov00_021F955C()->mode) {
            ov00_021F956C(3);
            result = ov00_021F3CF8(0);
            if (ov00_021F82EC(result)) {
                return result;
            }
        } else if (ov00_021F955C()->mode == 1) {
            ov00_021F956C(4);
            result = ov00_021F5B5C(ov00_021F955C()->unk_208, 0);
            if (ov00_021F5B30(result)) {
                return result;
            }
        }
    } else {
        ov00_021F6EC4(1);
    }
    return 0;
}

// Closes the connection to profile if it is open, otherwise drops it from the peers.
int ov00_021F70B8(int profile) {
    void **connection;
    ov00_021F955C()->unk_20C = profile;
    connection = ov00_021EE73C(profile, ov00_021F955C()->players + 1);
    if (connection) {
        ov00_021F955C()->closeState = 2;
        gt2CloseConnectionHard(*connection);
        ov00_021F955C()->closeState = 0;
        return 1;
    }
    ov00_021F3334(profile, ov00_021F955C()->players + 1);
    return 0;
}

void ov00_021F7128(void) {
    ov00_021F955C()->closeState = 2;
    gt2CloseAllConnectionsHard(*ov00_021F955C()->peer);
    ov00_021F955C()->closeState = 0;
}

// Keeps the peers whose aid is in mask: they get the profiles of the others (command 16), which are dropped.
int ov00_021F7158(u32 mask) {
    u32 dropped[32], kept[32];
    int numDropped = 0, numKept = numDropped, i = 1;
    for (; i <= ov00_021F955C()->players; i++) {
        if (mask & (1 << ov00_021F955C()->aids[i])) {
            kept[numKept] = ov00_021F955C()->profiles[i];
            numKept++;
        } else {
            dropped[numDropped] = ov00_021F955C()->profiles[i];
            numDropped++;
        }
    }
    for (i = 0; i < numKept; i++) {
        if (ov00_021F5B30(ov00_021F4304(16, kept[i], 0, 0, dropped, numDropped))) {
            return 0;
        }
    }
    ov00_021F955C()->closeState = 2;
    for (i = 0; i < numDropped; i++) {
        int index = ov00_021F7CEC(dropped[i], 0);
        if (index != 255) {
            ov00_021EE31C(index);
        }
    }
    ov00_021F955C()->closeState = 0;
    return 1;
}

// Sends an aid message to aid: 2 = its place in the peer list, 3 = our largest receive size.
void ov00_021F7294(int aid, int message) {
    u8 data[4];
    switch (message) {
    case 2: {
        MatchWork *w = ov00_021F955C();
        u8 i;
        if (aid == w->aids[ov00_021F955C()->players]) {
            data[0] = 1;
        } else {
            data[0] = 0;
        }
        for (i = 1; i <= ov00_021F955C()->players; i++) {
            if (aid == ov00_021F955C()->aids[i]) {
                data[1] = i;
                data[2] = aid;
                break;
            }
        }
        break;
    }
    case 3: {
        data[0] = ov00_021F955C()->maxSize;
        data[1] = ov00_021F955C()->maxSize >> 8;
        break;
    }
    default:
        break;
    }
    ov00_021F9630(message, aid, data, 4);
    {
        MatchWork *w = ov00_021F955C();
        w->readyTick = OS_GetTick();
    }
}

// Resends the aid messages of the "all peers ready" exchange every 6 seconds; returns 0 when the matchmaking
// was aborted.
int ov00_021F7384(void) {
    u64 elapsed;
    if (ov00_021F955C()->state == 9 || ov00_021F955C()->state == 16 || ov00_021F955C()->state == 17) {
        u64 now = OS_GetTick();
        elapsed = TICKS_TO_MS(now - ov00_021F955C()->readyTick);
    } else {
        return 1;
    }
    switch (ov00_021F955C()->state) {
    case 9:
        if (elapsed > 6000) {
            if (ov00_021EE568() == 5 && ov00_021F955C()->unk_1AD >= 5) {
                if (!ov00_021F631C(ov00_021F955C()->profiles[0])) {
                    return 0;
                }
            } else {
                ov00_021F955C()->unk_1AD++;
                ov00_021F7294(ov00_021F955C()->aids[0], 3);
            }
        }
        break;
    case 16:
        if (elapsed > 6000) {
            ov00_021F955C()->readyRetries++;
            if (ov00_021F955C()->readyRetries > 5) {
                if (ov00_021F955C()->mode == 0 || ov00_021F955C()->mode == 1) {
                    ov00_021F7128();
                    ov00_021F6EC4(1);
                } else {
                    if (!ov00_021F7158(ov00_021F955C()->readyMask)) {
                        return 0;
                    }
                    if (ov00_021F955C()->players) {
                        MatchWork *w;
                        ov00_021F955C()->readyRetries = 0;
                        w = ov00_021F955C();
                        w->readyTick = OS_GetTick();
                    } else if (!ov00_021F6100(ov00_021F955C()->unk_20C)) {
                        return 0;
                    }
                }
            } else {
                int i;
                for (i = 1; i <= ov00_021F955C()->players; i++) {
                    MatchWork *w = ov00_021F955C();
                    if (!(w->readyMask & (1 << ov00_021F955C()->aids[i]))) {
                        ov00_021F7294(ov00_021F955C()->aids[i], 2);
                    }
                }
            }
        }
        break;
    case 17:
        if (elapsed > ov00_021F955C()->maxSize) {
            ov00_021F64C0(4);
        }
        break;
    }
    return 1;
}

// Sends a close message (13 to 15) to profile.
int ov00_021F75F4(int profile, int message) {
    int count, value;
    MatchWork *w;
    if (message == 13) {
        value = ov00_021F955C()->unk_20C;
        count = 1;
    } else {
        count = 0;
    }
    if (ov00_021F5B30(ov00_021F4304(message, profile, 0, 0, (u32 *)&value, count))) {
        return 0;
    }
    w = ov00_021F955C();
    w->closeTick = OS_GetTick();
    return 1;
}

// Close exchange: the server sends 13 (with the profile it closes with), the peers answer 14, and when all have
// answered the server sends 15 and waits for the longest half round trip.
int ov00_021F7674(int profile, int message, int arg) {
    int i;
    if (ov00_021EE568() != 6) {
        return 1;
    }
    switch (message) {
    case 13:
        if (ov00_021F955C()->state != 8) {
            ov00_021F956C(8);
            ov00_021F70B8(arg);
        }
        if (!ov00_021F75F4(profile, 14)) {
            return 0;
        }
        break;
    case 14:
        if (ov00_021F955C()->state == 14) {
            u64 now = OS_GetTick();
            if (TICKS_TO_MS(now - ov00_021F955C()->closeTick) / 2 > 300) {
                u64 delay = TICKS_TO_MS(now - ov00_021F955C()->closeTick) / 2 - 300;
                if (delay > (u64)(int)ov00_021F955C()->closeDelay) {
                    ov00_021F955C()->closeDelay = delay;
                }
            }
            i = ov00_021F7CEC(profile, 0);
            if (i != 255) {
                MatchWork *w = ov00_021F955C();
                w->closeMask |= 1 << i;
            }
            i = ov00_021F7D50(1);
            if (i == ov00_021F955C()->closeMask) {
                for (i = 1; i <= ov00_021F955C()->players; i++) {
                    if (!ov00_021F75F4(ov00_021F955C()->profiles[i], 15)) {
                        return 0;
                    }
                }
                ov00_021F956C(15);
            }
        } else if (!ov00_021F75F4(profile, 15)) {
            return 0;
        }
        break;
    case 15:
        if (ov00_021F955C()->state == 8) {
            ov00_021F6EC4(2);
        }
        break;
    }
    return 1;
}

// Resends the close messages every 6 seconds; returns 0 when the matchmaking was aborted.
int ov00_021F7898(void) {
    u64 elapsed;
    int i;
    if (ov00_021F955C()->state == 8 || ov00_021F955C()->state == 14 || ov00_021F955C()->state == 15) {
        u64 now = OS_GetTick();
        elapsed = TICKS_TO_MS(now - ov00_021F955C()->closeTick);
    } else {
        return 1;
    }
    switch (ov00_021F955C()->state) {
    case 8:
        if (elapsed > 6000) {
            if (!ov00_021F75F4(ov00_021F955C()->profiles[0], 14)) {
                return 0;
            }
        }
        break;
    case 14:
        if (elapsed > 6000) {
            ++ov00_021F955C()->closeRetries;
            if (ov00_021F955C()->closeRetries > 5) {
                if (!ov00_021F7158(ov00_021F955C()->closeMask)) {
                    return 0;
                }
                if (ov00_021F955C()->players) {
                    ov00_021F955C()->closeRetries = 0;
                    ov00_021F955C()->closeTick = OS_GetTick();
                } else {
                    ov00_021F6EC4(2);
                }
            } else {
                for (i = 1; i <= ov00_021F955C()->players; i++) {
                    MatchWork *w = ov00_021F955C();
                    if (!(w->closeMask & (1 << ov00_021F955C()->aids[i]))) {
                        if (!ov00_021F75F4(ov00_021F955C()->profiles[i], 13)) {
                            return 0;
                        }
                    }
                }
            }
        }
        break;
    case 15:
        if (elapsed > (u64)(int)ov00_021F955C()->closeDelay) {
            ov00_021F6EC4(2);
        }
        break;
    }
    return 1;
}
