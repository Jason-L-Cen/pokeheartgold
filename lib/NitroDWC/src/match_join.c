#pragma force_active on
#include "global.h"

#include "dwc/dwc_account_internal.h"
#include "dwc/match_internal.h"
#include "dwc/net_common_internal.h"

// The code below is compiled as ARM (32-bit) code.
#include <nitro/code32.h>

#pragma warn_uninitializedvar off

// Joining a match: the handler of the commands received from the peers, answering a connect request, adding the new peer and telling the others about it,
// requesting the connection to a server or to the next friend that hosts a match, and dropping a peer.

// Buddy status read by ov00_021F5D0C, 0x210 bytes.
typedef struct MatchBuddyStatus {
    int unk_00;
    int status;     // 0x04, 4 = playing
    char data[520]; // 0x08, "\"-separated key/value pairs
} MatchBuddyStatus;

extern const char ov00_02217A68[]; // "FME"
extern const char ov00_02217A6C[]; // "MDF"
extern const char ov00_02217A70[]; // "VER"
extern const char ov00_02217AAC[]; // "dwc_pid"

extern u32 strtoul(const char *s, char **end, int base);
extern void ov00_021EE31C(int index);
extern int ov00_021EE568(void);
extern DwcUserData *ov00_021EF3A4(void);
extern int ov00_021F0404(int profile);
extern int ov00_021F4304(int command, u32 profile, u32 addr, u32 port, u32 *data, int count);
extern u8 ov00_021F7B30(void);
extern int ov00_021F7BA0(int profile);
extern int ov00_021F7CEC(int profile, int first);
extern void ov00_021F7DB4(void);
extern void ov00_021F7DDC(int flag);
extern int ov00_021F8468(unsigned error);
extern int ov00_02200400(void *connection, int buddy, MatchBuddyStatus *status);
extern int ov00_02200540(void *connection, int profile, int *buddy);
extern int ov00_022005B0(void *connection, int profile);
extern void gt2CloseAllConnectionsHard(void *connection);
extern void *gt2GetSocketSOCKET(void *connection);
extern void ov00_0221019C(void *p);
extern void ov00_02210CBC(void *reporting);
extern u32 ov00_02212DB4(void *server, const char *key, int fallback);
extern u32 ov00_02212E78(void *server);
extern u32 ov00_02212E80(void *server);
extern void *ov00_022139B8(void *list, int index);

// Handles a command from profile (addr:port); returns 0 when the matchmaking was aborted.
int ov00_021F4740(int command, int profile, u32 addr, int port, int *data, int count) {
    int n = 0;
    int newProfile;
    MatchAddr peerAddr;
    u32 reply[65];
    if (!ov00_021F955C() || ov00_021F955C()->state == 0) {
        return 1;
    }
    if (ov00_021EE568() == 5 && ov00_021F955C()->state == 1 && profile == ov00_021F955C()->profiles[0]) {
        ov00_021F7DB4();
    }
    switch (command) {
    case 1:
    case 11: {
        int response;
        if (ov00_021F955C()->mode != 0) {
            addr = data[1];
            port = (u16)data[2];
        }
        response = ov00_021F5624(profile, addr, port, data[0], command == 11);
        if (response == 2) {
            ov00_021F955C()->unk_1DC = 0;
            if (ov00_021F5B30(ov00_021F5920(profile, addr, port))) {
                return 0;
            }
            if (ov00_021F955C()->mode == 2 && ov00_021F955C()->unk_46C) {
                MatchWork *w = ov00_021F955C();
                int friendIndex = ov00_021F0404(profile);
                w->unk_46C(friendIndex, ov00_021F955C()->unk_470);
            }
            reply[0] = ov00_021F955C()->connections;
            for (n = 1; n <= ov00_021F955C()->connections; n++) {
                reply[n] = ov00_021F955C()->profiles[n];
            }
            reply[n++] = ov00_021F955C()->addr;
            reply[n++] = ov00_021F955C()->port;
            ov00_021F956C(11);
        } else if (response == 3 && ov00_021F955C()->limit != 0) {
            MatchWork *w = ov00_021F955C();
            if (w->connections == ov00_021F955C()->limit && ov00_021F955C()->mode == 2) {
                reply[0] = 16;
                n = 1;
            }
        }
        if (response != 255) {
            if (ov00_021F5B30(ov00_021F4304(response, profile, addr, port, reply, n))) {
                return 0;
            }
        }
        break;
    }
    case 2:
        if (ov00_021F955C()->state != 4 || profile != ov00_021F955C()->target) {
            break;
        }
        ov00_021F955C()->unk_208 = 0;
        ov00_021F955C()->unk_1A7 = 0;
        ov00_021F955C()->delay = 0;
        ov00_021F955C()->active = 0;
        ov00_021F955C()->addresses[0] = (data + data[0])[1];
        ov00_021F955C()->ports[0] = (data + data[0])[2];
        ov00_021F955C()->remoteAddr = (data + data[0])[1];
        ov00_021F955C()->remotePort = (data + data[0])[2];
        if (ov00_021F955C()->mode == 1) {
            if (ov00_021F6B20(data + 1, data[0])) {
                if (ov00_021F955C()->players) {
                    ov00_021F5AE0(profile, data);
                }
            } else {
                if (ov00_021F5B30(ov00_021F60AC(profile))) {
                    return 0;
                }
                if (ov00_021F5B30(ov00_021F5D0C(0, 0, profile))) {
                    return 0;
                }
                break;
            }
        }
        if (ov00_021F955C()->mode == 0) {
            if (ov00_021F955C()->players) {
                ov00_021F5AE0(profile, data);
                if (ov00_021F5B30(ov00_021F63DC())) {
                    return 0;
                }
            }
            ov00_021F956C(6);
            if (ov00_021F8468(ov00_021F3F5C(0, 0, ov00_022139B8(ov00_021F955C()->list, 0)))) {
                return 0;
            }
        } else {
            ov00_021F956C(5);
            if (ov00_021F82EC(ov00_021F3CF8(profile))) {
                return 0;
            }
        }
        break;
    case 3:
        if (ov00_021F955C()->state != 4 || profile != ov00_021F955C()->target) {
            break;
        }
        if (count > 0 && data[0] == 16) {
            ov00_021F2EE4(13, 0);
            return 0;
        }
        return ov00_021F5FD4(ov00_021F955C()->target);
    case 4: {
        MatchWork *w;
        if (ov00_021F955C()->state != 4 || profile != ov00_021F955C()->target) {
            break;
        }
        w = ov00_021F955C();
        w->time = OS_GetTick();
        if ((ov00_021F955C()->unk_208 && ov00_021F955C()->unk_1A7 < 16) || ov00_021F955C()->mode == 3) {
            ov00_021F955C()->active = 1;
            w = ov00_021F955C();
            w->activeTick = OS_GetTick();
            if (ov00_021F955C()->mode != 3) {
                ov00_021F955C()->unk_1A7++;
            }
        } else {
            ov00_021F955C()->unk_208 = 0;
            ov00_021F955C()->unk_1A7 = 0;
            if (ov00_021F955C()->mode == 0) {
                ov00_021F956C(3);
                ov00_021F955C()->unk_E8 = 1;
                w = ov00_021F955C();
                w->unk_EC = OS_GetTick();
            } else if (ov00_021F955C()->mode == 1) {
                ov00_021F5D0C(1, 0, 0);
            }
        }
        break;
    }
    case 5:
        if (!ov00_021F955C()->unk_17 || profile != ov00_021F955C()->unk_20) {
            break;
        }
        if (ov00_021F955C()->mode == 2 && ov00_021F955C()->players == 1 && profile == ov00_021F955C()->profiles[1]) {
            gt2CloseAllConnectionsHard(*ov00_021F955C()->peer);
        }
        if (!ov00_021F6100(profile)) {
            return 0;
        }
        break;
    case 6: {
        int newPort;
        int newAddr;
        newAddr = data[0];
        newPort = (u16)data[1];
        void *socket;
        MatchWork *w;
        if (ov00_021F955C()->state == 1) {
            ov00_021F956C(6);
        } else {
            if (ov00_021F955C()->state != 6 && ov00_021F955C()->state != 11) {
                break;
            }
            if (profile != ov00_021F955C()->unk_20) {
                break;
            }
        }
        ov00_021F955C()->unk_3CC = 255;
        w = ov00_021F955C();
        if (w->profiles[ov00_021F955C()->players + 1] != profile) {
            w = ov00_021F955C();
            w->profiles[ov00_021F955C()->players + 1] = profile;
        }
        peerAddr.address = newAddr;
        peerAddr.port = ((newPort >> 8) & 0xFF) | ((newPort << 8) & 0xFF00);
        ov00_021F955C()->request.flag = 1;
        socket = gt2GetSocketSOCKET(*ov00_021F955C()->peer);
        ov00_021F9104(0, (int)socket, &peerAddr, &ov00_021F955C()->request);
        w = ov00_021F955C();
        w->waitTick = 0;
        break;
    }
    case 7: {
        int flag;
        MatchWork *w;
        if (ov00_021F955C()->state != 1 || profile != ov00_021F955C()->profiles[0]) {
            break;
        }
        newProfile = data[0];
        flag = data[1] & 0xFF;
        if (ov00_021F955C()->mode == 1 && !ov00_021F955C()->unk_1A6) {
            reply[0] = ov00_021F7BA0(newProfile);
            if (ov00_021F5B30(ov00_021F4304(32, profile, addr, port, reply, 1))) {
                return 0;
            }
        }
        w = ov00_021F955C();
        w->profiles[ov00_021F955C()->connections + 1] = newProfile;
        w = ov00_021F955C();
        w->aids[ov00_021F955C()->connections + 1] = flag;
        ov00_02210CBC(ov00_021F955C()->reporting);
        if (ov00_021F955C()->unk_46C) {
            int friendIndex;
            w = ov00_021F955C();
            friendIndex = ov00_021F0404(newProfile);
            w->unk_46C(friendIndex, ov00_021F955C()->unk_470);
        }
        break;
    }
    case 8: {
        int index;
        MatchWork *w;
        if (ov00_021F955C()->state != 1 || profile != ov00_021F955C()->profiles[0]) {
            break;
        }
        newProfile = data[0];
        if (!newProfile) {
            index = data[1];
            n = data[2];
            ((MatchWork *)(index + (u32)ov00_021F955C()))->aids[0] = n;
            w = ov00_021F955C();
            w->profiles[index] = ov00_021F955C()->identity;
            ov00_021F64C0(3);
            break;
        }
        n = data[2] & 0xFF;
        index = data[1];
        if (newProfile == ov00_021F955C()->profiles[index] && index == ov00_021F955C()->players - 1) {
            w = ov00_021F955C();
            if (ov00_021F5B30(ov00_021F4304(9, profile, w->addresses[0], ov00_021F955C()->ports[0], (u32 *)&newProfile, 1))) {
                return 0;
            }
            break;
        }
        ov00_021F955C()->profiles[index] = newProfile;
        ((MatchWork *)(index + (u32)ov00_021F955C()))->aids[0] = n;
        ov00_021F955C()->addresses[index] = data[3];
        ov00_021F955C()->ports[index] = data[4];
        ov00_021F955C()->remoteAddr = data[3];
        ov00_021F955C()->remotePort = data[4];
        ov00_021F956C(5);
        if (ov00_021F82EC(ov00_021F3CF8(newProfile))) {
            return 0;
        }
        ov00_021F955C()->delay = 0;
        ov00_021F955C()->active = 0;
        break;
    }
    case 9: {
        MatchWork *w;
        if (ov00_021F955C()->state != 13) {
            break;
        }
        w = ov00_021F955C();
        if (data[0] != w->profiles[ov00_021F955C()->initial + 1]) {
            break;
        }
        ov00_021F955C()->initial++;
        ov00_021F64C0(0);
        break;
    }
    case 10:
        if (ov00_021F955C()->state != 1 && ov00_021F955C()->state != 18) {
            break;
        }
        if (ov00_021F955C()->mode == 0 || ov00_021F6B20(data + 1, data[0])) {
            ov00_021F955C()->unk_208 = data[1];
            ov00_021F955C()->unk_1A7 = 0;
        } else {
            ov00_021F955C()->unk_208 = 0;
        }
        if (ov00_021F955C()->players) {
            gt2CloseAllConnectionsHard(*ov00_021F955C()->peer);
        } else if (ov00_021F6FF8()) {
            return 0;
        }
        break;
    case 12:
        if (profile != ov00_021F955C()->profiles[0]) {
            break;
        }
        if (ov00_021F955C()->mode == 0 || ov00_021F955C()->mode == 1) {
            if (!ov00_021F631C(profile)) {
                return 0;
            }
        } else if (ov00_021F955C()->mode == 3) {
            ov00_021F955C()->unk_20C = profile;
            ov00_021F7128();
            ov00_021F6EC4(0);
        }
        break;
    case 13:
    case 14:
    case 15:
        if (!ov00_021F7674(profile, command, data[0])) {
            return 0;
        }
        break;
    case 16: {
        int i;
        if (profile != ov00_021F955C()->profiles[0]) {
            return 1;
        }
        for (i = 0; i < count; i++) {
            int index = ov00_021F7CEC(data[i], 0);
            if (index != 255) {
                ov00_021EE31C(index);
            }
        }
        break;
    }
    case 17: {
        MatchTimer *timer = ov00_0221A928.timer;
        if (timer && timer->active && TICKS_TO_MS(OS_GetTick() - timer->start) >= timer->timeout) {
            reply[0] = 1;
        } else {
            reply[0] = 0;
        }
        if (ov00_021F5B30(ov00_021F4304(18, profile, addr, port, reply, 1))) {
            return 0;
        }
        break;
    }
    case 18: {
        int index;
        if (ov00_021F955C()->state != 19) {
            break;
        }
        index = ov00_021F7CEC(profile, 0);
        if (index == 255) {
            break;
        }
        ov00_0221A928.timer->mask |= 1 << index;
        if (data[0]) {
            ov00_0221A928.timer->all |= 1 << index;
        }
        break;
    }
    case 19:
        ov00_021F2EE4(12, 0);
        return 0;
    case 32: {
        int i;
        if (ov00_021F955C()->mode != 1 || ov00_021F955C()->unk_1A6) {
            break;
        }
        for (i = 1; i <= ov00_021F955C()->players; i++) {
            if (profile == ov00_021F955C()->profiles[i]) {
                if (!data[0]) {
                    if (ov00_021F955C()->state == 11 || ov00_021F955C()->state == 12) {
                        if (ov00_021F5B30(ov00_021F6DF0())) {
                            return 0;
                        }
                    }
                    if (ov00_021F955C()->request.active) {
                        ov00_0221019C((void *)ov00_021F955C()->request.active);
                        ov00_021F955C()->request.active = 0;
                    }
                    ov00_021F6FC4();
                } else {
                    MatchWork *w = ov00_021F955C();
                    w->unk_1DC |= 1 << ov00_021F7CEC(profile, 0);
                }
                break;
            }
        }
        break;
    }
    case 64: {
        int i;
        for (i = 1; i <= ov00_021F955C()->players; i++) {
            if (profile == ov00_021F955C()->profiles[i]) {
                if (ov00_021F5B30(ov00_021F4304(65, profile, addr, port, NULL, 0))) {
                    return 0;
                }
                break;
            }
        }
        break;
    }
    case 65:
        break;
    }
    return 1;
}

#pragma opt_lifetimes off

// Answers a connect request (command 1, or 11 when resent) from profile: 2 = accept, 3 = refuse,
// 4 = not ready yet, 19 = closed, 255 = ignore.
int ov00_021F5624(int profile, u32 addr, int port, int mode, BOOL resend) {
    int result;
    switch (ov00_021F955C()->mode) {
    case 1:
        if (!ov00_022005B0(ov00_021F955C()->unk_00, profile)) {
            result = 255;
            break;
        }
        if (!ov00_021F7BA0(profile)) {
            result = 3;
            break;
        }
    case 0:
        if (mode != ov00_021F955C()->mode || ov00_021F955C()->unk_1A9 || ov00_021F955C()->connections == ov00_021F955C()->limit || (ov00_021F955C()->unk_17 && ov00_021F955C()->unk_20 == ov00_021F955C()->identity)) {
            result = 3;
            if (ov00_021F955C()->mode == 0 && *(int *)((u8 *)ov00_021F955C()->reporting + 0xB4) == 0 && ov00_021F955C()->unk_17 && ov00_021F955C()->unk_20 == ov00_021F955C()->identity) {
                ov00_02210CBC(ov00_021F955C()->reporting);
            }
            break;
        }
        if (ov00_021F955C()->state == 3 || ov00_021F955C()->state == 4) {
            if (ov00_021F955C()->addr || ov00_021F955C()->port) {
                if (addr || port) {
                    goto accept;
                }
            }
        }
        result = 4;
        break;
accept: {
    if (ov00_021F955C()->target) {
        if (profile == ov00_021F955C()->target) {
            if (resend || ((int)ov00_021F955C()->identity < profile && ov00_021F955C()->unk_208 != profile)) {
                result = 2;
            } else {
                result = 255;
            }
        } else {
            if (resend || ((int)ov00_021F955C()->identity < profile && ov00_021F955C()->unk_208 == 0)) {
                if (ov00_021F5B30(ov00_021F60AC(ov00_021F955C()->target))) {
                    return 255;
                }
                result = 2;
            } else {
                result = 3;
            }
        }
    } else {
        result = 2;
    }
} break;
    case 2:
        if (!ov00_022005B0(ov00_021F955C()->unk_00, profile)) {
            result = 255;
            break;
        }
        if (mode != 3 || ov00_021F955C()->connections == ov00_021F955C()->limit) {
            result = 3;
            break;
        }
        if (ov00_0221A928.unk_1C == 1 && ov00_0221A928.unk_1D == 1) {
            result = 19;
            break;
        }
        if (ov00_021F955C()->state == 10 && (ov00_021F955C()->addr || ov00_021F955C()->port)) {
            if (addr || port) {
                goto server;
            }
        }
        result = 4;
        break;
server:
        result = 2;
        break;
    }
    return result;
}

#pragma opt_lifetimes reset

// Adds profile as the next peer and sends it to the connected peers (command 7).
int ov00_021F5920(int profile, u32 addr, int port) {
    MatchWork *w, *v, *u;
    int data[2];
    int i, result;
    if (ov00_021F955C()->unk_17 && profile == ov00_021F955C()->unk_20) {
        return 0;
    }
    ov00_021F955C()->unk_17 = 1;
    ov00_021F955C()->unk_20 = profile;
    ov00_021F955C()->active = 0;
    ov00_021F955C()->delay = 0;
    ov00_02210CBC(ov00_021F955C()->reporting);
    ov00_021F955C()->target = 0;
    w = ov00_021F955C();
    w->profiles[ov00_021F955C()->connections + 1] = profile;
    w = ov00_021F955C();
    w->addresses[ov00_021F955C()->connections + 1] = addr;
    w = ov00_021F955C();
    w->ports[ov00_021F955C()->connections + 1] = port;
    ov00_021F955C()->remoteAddr = addr;
    ov00_021F955C()->remotePort = port;
    w = ov00_021F955C();
    v = ov00_021F955C();
    result = ov00_021F7B30();
    w->aids[v->connections + 1] = result;
    data[0] = profile;
    w = ov00_021F955C();
    data[1] = w->aids[ov00_021F955C()->connections + 1];
    for (i = 1; i <= ov00_021F955C()->connections; i++) {
        w = ov00_021F955C();
        v = ov00_021F955C();
        u = ov00_021F955C();
        result = ov00_021F4304(7, w->profiles[i], v->addresses[i], u->ports[i], (u32 *)data, 2);
        if (result) {
            return result;
        }
    }
    ov00_021F7DDC(1);
    return 0;
}

// Keeps the profiles of the server's peers (count, profiles...) with profile in front.
void ov00_021F5AE0(int profile, int *list) {
    u32 n = list[0] + 2;
    if (n > 2) {
        MIi_CpuCopy32(list + 1, &ov00_021F955C()->unk_34C[1], (n - 2) * 4);
    }
    ov00_021F955C()->unk_348 = n - 1;
    ov00_021F955C()->unk_34C[0] = profile;
}

int ov00_021F5B30(unsigned error) {
    if (ov00_021F955C()->mode == 0) {
        return ov00_021F82EC(error);
    } else {
        return ov00_021F8278(error);
    }
}

// Requests the connection to profile (command 1, or 11 when retrying); with no server address yet, or when
// direct, only waits for it.
int ov00_021F5B5C(int profile, BOOL direct) {
    MatchWork *w;
    int n, command;
    u32 data[3];
    if (direct || (!ov00_021F955C()->addr && !ov00_021F955C()->port)) {
        ov00_021F955C()->active = 1;
        w = ov00_021F955C();
        w->activeTick = OS_GetTick();
        ov00_021F955C()->profiles[0] = profile;
        return 0;
    }
    if (!ov00_021F955C()->mode) {
        void *server = ov00_022139B8(ov00_021F955C()->list, 0);
        w = ov00_021F955C();
        w->profiles[0] = ov00_02212DB4(server, ov00_02217AAC, 0);
        w = ov00_021F955C();
        w->addresses[0] = ov00_02212E78(server);
        w = ov00_021F955C();
        w->ports[0] = ov00_02212E80(server);
        w = ov00_021F955C();
        w->target = ov00_021F955C()->profiles[0];
        n = 1;
    } else {
        if (ov00_021F955C()->mode == 1) {
            ov00_021F955C()->profiles[0] = profile;
        }
        ov00_021F955C()->target = profile;
        data[1] = ov00_021F955C()->addr;
        data[2] = ov00_021F955C()->port;
        n = 3;
    }
    ov00_021F955C()->delay = 6000;
    w = ov00_021F955C();
    w->time = OS_GetTick();
    ov00_021F955C()->active = 0;
    command = ov00_021F955C()->unk_208 ? 11 : 1;
    data[0] = ov00_021F955C()->mode;
    w = ov00_021F955C();
    return ov00_021F4304(command, profile, w->addresses[0], ov00_021F955C()->ports[0], data, n);
}

// Looks for the next friend (after the current one unless same) that hosts a match of our kind and requests
// the connection to it; waits 3 seconds when there is none.
int ov00_021F5D0C(int mode, BOOL same, int previous) {
    struct {
        char mdf[4];
        int buddy;
        char fme[4], ver[12];
        MatchBuddyStatus status;
    } buf;

    int profile, seen, advance, start, i, ret, a, b, d;
    MatchWork *w, *x, *y;
    DwcUserData *user;
    if (same) {
        start = ov00_021F955C()->current;
    } else if (ov00_021F955C()->current >= ov00_021F955C()->friendCount - 1) {
        start = 0;
    } else {
        start = ov00_021F955C()->current + 1;
    }
    seen = 0;
    advance = same == 0 ? 1 : seen;
    for (;;) {
        if (advance || seen) {
            ov00_021F955C()->current++;
            if (ov00_021F955C()->current >= ov00_021F955C()->friendCount) {
                ov00_021F955C()->current = 0;
            }
        }
        if (seen && start == ov00_021F955C()->current) {
            ov00_021F955C()->delay = 3000;
            w = ov00_021F955C();
            w->time = OS_GetTick();
            ov00_021F955C()->active = 0;
            return 0;
        }
        seen = 1;
        user = ov00_021EF3A4();
        w = ov00_021F955C();
        x = ov00_021F955C();
        y = ov00_021F955C();
        profile = DWC_GetGsProfileId(user, &w->friends[x->friendIds[y->current]]);
        if (!profile || profile == -1) {
            continue;
        }
        w = ov00_021F955C();
        x = ov00_021F955C();
        y = ov00_021F955C();
        if (!DWCi_Acc_IsValidFriendData(&w->friends[x->friendIds[y->current]])) {
            continue;
        }
        for (i = 1; i <= ov00_021F955C()->players; i++) {
            if (profile == ov00_021F955C()->profiles[i]) {
                break;
            }
        }
        if (i <= ov00_021F955C()->players) {
            continue;
        }
        ret = ov00_02200540(ov00_021F955C()->unk_00, profile, &buf.buddy);
        ret |= ov00_02200400(ov00_021F955C()->unk_00, buf.buddy, &buf.status);
        if (ret || buf.status.status != 4) {
            continue;
        }
        a = ov00_021ED4E8((char *)ov00_02217A70, buf.ver, buf.status.data, '/');
        b = ov00_021ED4E8((char *)ov00_02217A68, buf.fme, buf.status.data, '/');
        d = ov00_021ED4E8((char *)ov00_02217A6C, buf.mdf, buf.status.data, '/');
        if (a <= 0 || b <= 0 || d <= 0) {
            continue;
        }
        if (strtoul(buf.ver, NULL, 10) != 3) {
            continue;
        }
        a = strtoul(buf.fme, NULL, 10);
        if (a != ov00_021F955C()->limit) {
            continue;
        }
        if (profile == previous) {
            mode = 1;
        }
        return ov00_021F5B5C(profile, mode);
    }
}

int ov00_021F5FD4(int profile) {
    MatchWork *w;
    ov00_021F955C()->unk_208 = 0;
    ov00_021F955C()->target = 0;
    ov00_021F955C()->unk_1A7 = 0;
    w = ov00_021F955C();
    w->time = OS_GetTick();
    if (!ov00_021F955C()->mode) {
        ov00_021F956C(3);
        if (ov00_021F82EC(ov00_021F3CF8(0))) {
            return 0;
        }
    } else if (ov00_021F955C()->mode == 1) {
        if (ov00_021F5B30(ov00_021F5D0C(0, 0, profile))) {
            return 0;
        }
    } else if (ov00_021F955C()->mode == 3) {
        ov00_021F2EE4(6, -80410);
        return 0;
    }
    return 1;
}

// Cancels the connection to profile (command 5).
int ov00_021F60AC(int profile) {
    MatchWork *w = ov00_021F955C();
    int result = ov00_021F4304(5, profile, w->addresses[0], ov00_021F955C()->ports[0], NULL, 0);
    ov00_021F955C()->target = 0;
    return result;
}

static inline BOOL IsOwnServer(void) {
    MatchWork *a = ov00_021F955C();
    MatchWork *b = ov00_021F955C();
    return a->unk_20 == b->identity;
}

// Drops the peer being connected and goes back to looking for a match.
int ov00_021F6100(int profile) {
    int clear;
    MatchWork *w;
    if (ov00_021F955C()->unk_17 != 0 && IsOwnServer()) {
        clear = 0;
    } else {
        clear = 1;
    }
    if (clear) {
        ov00_021F955C()->unk_17 = 0;
        ov00_021F955C()->unk_20 = 0;
        ov00_02210CBC(ov00_021F955C()->reporting);
    }
    if (ov00_021F955C()->players < 31) {
        w = ov00_021F955C();
        w->profiles[ov00_021F955C()->players + 1] = 0;
    }
    ov00_021F955C()->unk_3CC = 255;
    if (ov00_021F955C()->request.active != 0) {
        ov00_0221019C((void *)ov00_021F955C()->request.active);
        ov00_021F955C()->request.active = 0;
    }
    w = ov00_021F955C();
    w->connections = ov00_021F955C()->players;
    ov00_021F955C()->target = 0;
    if (!clear) {
        if (ov00_021F955C()->mode != 3) {
            ov00_021F6FC4();
        }
    } else if (ov00_021F955C()->mode == 0) {
        ov00_021F956C(3);
        ov00_021F955C()->unk_E8 = 2;
        w = ov00_021F955C();
        w->unk_EC = OS_GetTick();
    } else if (ov00_021F955C()->mode == 1) {
        ov00_021F956C(4);
        ov00_021F5D0C(1, 0, 0);
    } else if (ov00_021F955C()->mode == 2) {
        int i;
        ov00_021F956C(14);
        ov00_021F955C()->closeMask = 0;
        ov00_021F955C()->closeDelay = 0;
        ov00_021F70B8(profile);
        for (i = 1; i <= ov00_021F955C()->players; i++) {
            if (ov00_021F75F4(ov00_021F955C()->profiles[i], 13) == 0) {
                return 0;
            }
        }
        if (ov00_021F955C()->players == 0) {
            ov00_021F6EC4(2);
        }
    }
    return 1;
}
