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

// Buddy status read by DWCi_SendReservationToNextFriend, 0x210 bytes.
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
extern int DWCi_SendMatchCommand(int command, u32 profile, u32 addr, u32 port, u32 *data, int count);
extern u8 DWCi_GetFreeAid(void);
extern int DWCi_IsFriendProfile(int profile);
extern int DWCi_GetAidFromProfile(int profile, int first);
extern void ov00_021F7DB4(void);
extern void DWCi_ResetMinCompleteTimer(int flag);
extern int DWCi_HandleNNError(unsigned error);
extern int gpGetBuddyStatus(void *connection, int buddy, MatchBuddyStatus *status);
extern int gpGetBuddyIndex(void *connection, int profile, int *buddy);
extern int gpIsBuddy(void *connection, int profile);
extern void gt2CloseAllConnectionsHard(void *connection);
extern void *gt2GetSocketSOCKET(void *connection);
extern void ov00_0221019C(void *p);
extern void qr2_send_statechanged(void *reporting);
extern u32 SBServerGetIntValueA(void *server, const char *key, int fallback);
extern u32 SBServerGetPublicInetAddress(void *server);
extern u32 SBServerGetPublicQueryPort(void *server);
extern void *ServerBrowserGetServer(void *list, int index);

// Handles a command from profile (addr:port); returns 0 when the matchmaking was aborted.
int DWCi_ProcessMatchCommand(int command, int profile, u32 addr, int port, int *data, int count) {
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
        response = DWCi_ProcessReservationRequest(profile, addr, port, data[0], command == 11);
        if (response == 2) {
            ov00_021F955C()->unk_1DC = 0;
            if (DWCi_HandleMatchCommandError(DWCi_AddNewPeer(profile, addr, port))) {
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
            if (DWCi_HandleMatchCommandError(DWCi_SendMatchCommand(response, profile, addr, port, reply, n))) {
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
            if (DWCi_CheckFriendProfiles(data + 1, data[0])) {
                if (ov00_021F955C()->players) {
                    DWCi_SetServerPeerProfiles(profile, data);
                }
            } else {
                if (DWCi_HandleMatchCommandError(DWCi_CancelReservation(profile))) {
                    return 0;
                }
                if (DWCi_HandleMatchCommandError(DWCi_SendReservationToNextFriend(0, 0, profile))) {
                    return 0;
                }
                break;
            }
        }
        if (ov00_021F955C()->mode == 0) {
            if (ov00_021F955C()->players) {
                DWCi_SetServerPeerProfiles(profile, data);
                if (DWCi_HandleMatchCommandError(DWCi_SendServerPeerProfiles())) {
                    return 0;
                }
            }
            ov00_021F956C(6);
            if (DWCi_HandleNNError(DWCi_StartNatNegotiation(0, 0, ServerBrowserGetServer(ov00_021F955C()->list, 0)))) {
                return 0;
            }
        } else {
            ov00_021F956C(5);
            if (DWCi_HandleSBError(DWCi_UpdateServerListAsync(profile))) {
                return 0;
            }
        }
        break;
    case 3:
        if (ov00_021F955C()->state != 4 || profile != ov00_021F955C()->target) {
            break;
        }
        if (count > 0 && data[0] == 16) {
            DWCi_StopMatching(13, 0);
            return 0;
        }
        return DWCi_RestartMatchSearch(ov00_021F955C()->target);
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
                DWCi_SendReservationToNextFriend(1, 0, 0);
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
        if (!DWCi_DropConnectingPeer(profile)) {
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
        DWCi_NNCompletedCallback(0, (int)socket, &peerAddr, &ov00_021F955C()->request);
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
            reply[0] = DWCi_IsFriendProfile(newProfile);
            if (DWCi_HandleMatchCommandError(DWCi_SendMatchCommand(32, profile, addr, port, reply, 1))) {
                return 0;
            }
        }
        w = ov00_021F955C();
        w->profiles[ov00_021F955C()->connections + 1] = newProfile;
        w = ov00_021F955C();
        w->aids[ov00_021F955C()->connections + 1] = flag;
        qr2_send_statechanged(ov00_021F955C()->reporting);
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
            DWCi_AdvanceJoin(3);
            break;
        }
        n = data[2] & 0xFF;
        index = data[1];
        if (newProfile == ov00_021F955C()->profiles[index] && index == ov00_021F955C()->players - 1) {
            w = ov00_021F955C();
            if (DWCi_HandleMatchCommandError(DWCi_SendMatchCommand(9, profile, w->addresses[0], ov00_021F955C()->ports[0], (u32 *)&newProfile, 1))) {
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
        if (DWCi_HandleSBError(DWCi_UpdateServerListAsync(newProfile))) {
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
        DWCi_AdvanceJoin(0);
        break;
    }
    case 10:
        if (ov00_021F955C()->state != 1 && ov00_021F955C()->state != 18) {
            break;
        }
        if (ov00_021F955C()->mode == 0 || DWCi_CheckFriendProfiles(data + 1, data[0])) {
            ov00_021F955C()->unk_208 = data[1];
            ov00_021F955C()->unk_1A7 = 0;
        } else {
            ov00_021F955C()->unk_208 = 0;
        }
        if (ov00_021F955C()->players) {
            gt2CloseAllConnectionsHard(*ov00_021F955C()->peer);
        } else if (DWCi_RestartWithPendingServer()) {
            return 0;
        }
        break;
    case 12:
        if (profile != ov00_021F955C()->profiles[0]) {
            break;
        }
        if (ov00_021F955C()->mode == 0 || ov00_021F955C()->mode == 1) {
            if (!DWCi_HandleReservationRefused(profile)) {
                return 0;
            }
        } else if (ov00_021F955C()->mode == 3) {
            ov00_021F955C()->unk_20C = profile;
            DWCi_CloseAllConnectionsHard();
            DWCi_RestartMatching(0);
        }
        break;
    case 13:
    case 14:
    case 15:
        if (!DWCi_ProcessCloseMessage(profile, command, data[0])) {
            return 0;
        }
        break;
    case 16: {
        int i;
        if (profile != ov00_021F955C()->profiles[0]) {
            return 1;
        }
        for (i = 0; i < count; i++) {
            int index = DWCi_GetAidFromProfile(data[i], 0);
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
        if (DWCi_HandleMatchCommandError(DWCi_SendMatchCommand(18, profile, addr, port, reply, 1))) {
            return 0;
        }
        break;
    }
    case 18: {
        int index;
        if (ov00_021F955C()->state != 19) {
            break;
        }
        index = DWCi_GetAidFromProfile(profile, 0);
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
        DWCi_StopMatching(12, 0);
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
                        if (DWCi_HandleMatchCommandError(DWCi_SendLeaveToServer())) {
                            return 0;
                        }
                    }
                    if (ov00_021F955C()->request.active) {
                        ov00_0221019C((void *)ov00_021F955C()->request.active);
                        ov00_021F955C()->request.active = 0;
                    }
                    DWCi_ClientRestartMatching();
                } else {
                    MatchWork *w = ov00_021F955C();
                    w->unk_1DC |= 1 << DWCi_GetAidFromProfile(profile, 0);
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
                if (DWCi_HandleMatchCommandError(DWCi_SendMatchCommand(65, profile, addr, port, NULL, 0))) {
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
int DWCi_ProcessReservationRequest(int profile, u32 addr, int port, int mode, BOOL resend) {
    int result;
    switch (ov00_021F955C()->mode) {
    case 1:
        if (!gpIsBuddy(ov00_021F955C()->unk_00, profile)) {
            result = 255;
            break;
        }
        if (!DWCi_IsFriendProfile(profile)) {
            result = 3;
            break;
        }
    case 0:
        if (mode != ov00_021F955C()->mode || ov00_021F955C()->unk_1A9 || ov00_021F955C()->connections == ov00_021F955C()->limit || (ov00_021F955C()->unk_17 && ov00_021F955C()->unk_20 == ov00_021F955C()->identity)) {
            result = 3;
            if (ov00_021F955C()->mode == 0 && *(int *)((u8 *)ov00_021F955C()->reporting + 0xB4) == 0 && ov00_021F955C()->unk_17 && ov00_021F955C()->unk_20 == ov00_021F955C()->identity) {
                qr2_send_statechanged(ov00_021F955C()->reporting);
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
                if (DWCi_HandleMatchCommandError(DWCi_CancelReservation(ov00_021F955C()->target))) {
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
        if (!gpIsBuddy(ov00_021F955C()->unk_00, profile)) {
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
int DWCi_AddNewPeer(int profile, u32 addr, int port) {
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
    qr2_send_statechanged(ov00_021F955C()->reporting);
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
    result = DWCi_GetFreeAid();
    w->aids[v->connections + 1] = result;
    data[0] = profile;
    w = ov00_021F955C();
    data[1] = w->aids[ov00_021F955C()->connections + 1];
    for (i = 1; i <= ov00_021F955C()->connections; i++) {
        w = ov00_021F955C();
        v = ov00_021F955C();
        u = ov00_021F955C();
        result = DWCi_SendMatchCommand(7, w->profiles[i], v->addresses[i], u->ports[i], (u32 *)data, 2);
        if (result) {
            return result;
        }
    }
    DWCi_ResetMinCompleteTimer(1);
    return 0;
}

// Keeps the profiles of the server's peers (count, profiles...) with profile in front.
void DWCi_SetServerPeerProfiles(int profile, int *list) {
    u32 n = list[0] + 2;
    if (n > 2) {
        MIi_CpuCopy32(list + 1, &ov00_021F955C()->unk_34C[1], (n - 2) * 4);
    }
    ov00_021F955C()->unk_348 = n - 1;
    ov00_021F955C()->unk_34C[0] = profile;
}

int DWCi_HandleMatchCommandError(unsigned error) {
    if (ov00_021F955C()->mode == 0) {
        return DWCi_HandleSBError(error);
    } else {
        return DWCi_HandleGPError(error);
    }
}

// Requests the connection to profile (command 1, or 11 when retrying); with no server address yet, or when
// direct, only waits for it.
int DWCi_SendReservationRequest(int profile, BOOL direct) {
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
        void *server = ServerBrowserGetServer(ov00_021F955C()->list, 0);
        w = ov00_021F955C();
        w->profiles[0] = SBServerGetIntValueA(server, ov00_02217AAC, 0);
        w = ov00_021F955C();
        w->addresses[0] = SBServerGetPublicInetAddress(server);
        w = ov00_021F955C();
        w->ports[0] = SBServerGetPublicQueryPort(server);
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
    return DWCi_SendMatchCommand(command, profile, w->addresses[0], ov00_021F955C()->ports[0], data, n);
}

// Looks for the next friend (after the current one unless same) that hosts a match of our kind and requests
// the connection to it; waits 3 seconds when there is none.
int DWCi_SendReservationToNextFriend(int mode, BOOL same, int previous) {
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
        ret = gpGetBuddyIndex(ov00_021F955C()->unk_00, profile, &buf.buddy);
        ret |= gpGetBuddyStatus(ov00_021F955C()->unk_00, buf.buddy, &buf.status);
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
        return DWCi_SendReservationRequest(profile, mode);
    }
}

int DWCi_RestartMatchSearch(int profile) {
    MatchWork *w;
    ov00_021F955C()->unk_208 = 0;
    ov00_021F955C()->target = 0;
    ov00_021F955C()->unk_1A7 = 0;
    w = ov00_021F955C();
    w->time = OS_GetTick();
    if (!ov00_021F955C()->mode) {
        ov00_021F956C(3);
        if (DWCi_HandleSBError(DWCi_UpdateServerListAsync(0))) {
            return 0;
        }
    } else if (ov00_021F955C()->mode == 1) {
        if (DWCi_HandleMatchCommandError(DWCi_SendReservationToNextFriend(0, 0, profile))) {
            return 0;
        }
    } else if (ov00_021F955C()->mode == 3) {
        DWCi_StopMatching(6, -80410);
        return 0;
    }
    return 1;
}

// Cancels the connection to profile (command 5).
int DWCi_CancelReservation(int profile) {
    MatchWork *w = ov00_021F955C();
    int result = DWCi_SendMatchCommand(5, profile, w->addresses[0], ov00_021F955C()->ports[0], NULL, 0);
    ov00_021F955C()->target = 0;
    return result;
}

static inline BOOL IsOwnServer(void) {
    MatchWork *a = ov00_021F955C();
    MatchWork *b = ov00_021F955C();
    return a->unk_20 == b->identity;
}

// Drops the peer being connected and goes back to looking for a match.
int DWCi_DropConnectingPeer(int profile) {
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
        qr2_send_statechanged(ov00_021F955C()->reporting);
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
            DWCi_ClientRestartMatching();
        }
    } else if (ov00_021F955C()->mode == 0) {
        ov00_021F956C(3);
        ov00_021F955C()->unk_E8 = 2;
        w = ov00_021F955C();
        w->unk_EC = OS_GetTick();
    } else if (ov00_021F955C()->mode == 1) {
        ov00_021F956C(4);
        DWCi_SendReservationToNextFriend(1, 0, 0);
    } else if (ov00_021F955C()->mode == 2) {
        int i;
        ov00_021F956C(14);
        ov00_021F955C()->closeMask = 0;
        ov00_021F955C()->closeDelay = 0;
        DWCi_CloseConnectionToProfile(profile);
        for (i = 1; i <= ov00_021F955C()->players; i++) {
            if (DWCi_SendCloseMessage(ov00_021F955C()->profiles[i], 13) == 0) {
                return 0;
            }
        }
        if (ov00_021F955C()->players == 0) {
            DWCi_RestartMatching(2);
        }
    }
    return 1;
}
