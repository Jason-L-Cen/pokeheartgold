#pragma force_active on
#include <string.h>

#include "global.h"

#include "dwc/match_internal.h"

// The code below is compiled as ARM (32-bit) code.
#include <nitro/code32.h>

#pragma warn_uninitializedvar off

// Matchmaking peer connections: raw packets for the query/reporting and NAT negotiation code, accepting and
// opening the peer connections, the "SBCM" command argument parser, aborting a match with an error, the
// match messages exchanged by the peers, dropping a peer, the aid lists and the server's custom keys.

// Address of a raw packet, 8 bytes.
typedef struct MatchPacketAddr {
    u8 unk_00;
    u8 family; // 0x01
    u16 port;  // 0x02, network byte order
    u32 address;
} MatchPacketAddr;

// Connection slot of ov00_021EE7A4, 8 bytes.
typedef struct MatchConnSlot {
    u8 index;
    u8 aid;     // 0x01
    u16 unk_02; // 0x02
    u32 unk_04; // 0x04
} MatchConnSlot;

extern const u8 ov00_02219D28[];   // NAT negotiation magic, 6 bytes
extern const char ov00_02217A60[]; // ""
extern const char ov00_02217A64[]; // "%u"
extern const char ov00_02217A70[]; // "VER"
extern const char ov00_02217A74[]; // "Init state"
extern const char ov00_02217A80[]; // "Server full"
extern const char ov00_02217A8C[]; // "Unknown connect attempt"
extern const char ov00_02217AA4[]; // "SCM"
extern const char ov00_02217AA8[]; // "SCN"
extern u8 ov00_0221B080[];

extern int memcmp(const void *, const void *, u32);
extern u32 strtoul(const char *, char **, int);
extern void ov00_021EC24C(int kind, int code);
extern void ov00_021EC2EC(int kind, void *p, int size);
extern void ov00_021ED470(const char *key, const char *value, void *buf, int size);
extern void ov00_021ED4A8(const char *key, const char *value, void *buf, int size);
extern int ov00_021EE568(void);
extern int ov00_021EE6D0(void);
extern void **ov00_021EE72C(int index);
extern MatchConnSlot *ov00_021EE7A4(int index);
extern int ov00_021F0404(int profile);
extern int ov00_021F04A0(int type, const void *message, int arg);
extern int DWCi_HandleGT2Error(unsigned error);
extern u32 DWCi_GetPeerAidBitmap(int flag);
extern int gt2Accept(void *connection, void *callbacks);
extern void gt2Reject(void *connection, const char *message, int size);
extern int gt2Connect(void *socket, int a, u32 remote, char *message, int size, int timeout, void *callbacks, int blocking);
extern void gt2CloseAllConnectionsHard(void *connection);
// Called with two and with three arguments.
extern void gt2SetConnectionData(void *connection, MatchConnSlot *slot, ...);
extern u32 gt2AddressToString(u32 addr, u16 port, int flag);
extern void ov00_0221019C(void *p);
extern void ov00_0221075C(u8 *data, u32 size, MatchPacketAddr *addr);
extern void qr2_send_statechanged(void *reporting);
extern void qr2_parse_queryA(void *reporting, u8 *data, u32 size, MatchPacketAddr *addr);

#pragma opt_optimizecontrolflow off

// Raw packet from address:port: query/reporting packets (0xFE 0xFD or '\') and NAT negotiation packets.
int DWCi_GT2UnrecognizedMessageCallback(void *unused, u32 address, int port, u8 *data, u32 size) {
    MatchPacketAddr addr;
    u8 tag;
    if (size == 0 || data == 0) {
        return 0;
    }
    MI_CpuClear32(&addr, sizeof(addr));
    addr.family = 2;
    addr.port = ((port >> 8) & 255) | ((port << 8) & 0xff00);
    addr.address = address;
    tag = data[0];
    if ((tag == 254 && data[1] == 253) || tag == 92) {
        if (ov00_021F955C()->reporting) {
            qr2_parse_queryA(ov00_021F955C()->reporting, data, size, &addr);
        }
    } else if (memcmp(data, ov00_02219D28, 6) == 0) {
        ov00_0221075C(data, size, &addr);
    } else {
        if (tag == 254) {
            return 0;
        } else {
            return 0;
        }
    }
    return 1;
}

#pragma opt_optimizecontrolflow reset

// Incoming connection: accepted if it comes from the expected peer (address and port, or its profile id in
// option), refused otherwise.
void DWCi_GT2ConnectAttemptCallback(void *unused, void *connection, u32 address, int port, int maxSize, u8 *option) {
    int index;
    MatchWork *work;
    MatchConnSlot *slot;
    void **entry;
    if (ov00_021F955C() == 0 || ov00_021F955C()->state != 7 || ov00_021F955C()->unk_1A9 != 0) {
        gt2Reject(connection, ov00_02217A74, -1);
        return;
    }
    index = ov00_021EE6D0();
    if (index == -1) {
        gt2Reject(connection, ov00_02217A80, -1);
        DWCi_StopMatching(6, -87100);
        return;
    }
    work = ov00_021F955C();
    if (address != work->addr2[ov00_021F955C()->players]) {
        goto different;
    }
    work = ov00_021F955C();
    if (port == work->port2[ov00_021F955C()->players]) {
        goto accepted;
    }
different:
    if (option[0]) {
        u32 profile = strtoul((char *)option, 0, 10);
        MatchWork *dest = ov00_021F955C();
        if (profile == dest->profiles[ov00_021F955C()->players]) {
            dest = ov00_021F955C();
            dest->addr2[ov00_021F955C()->players] = address;
            dest = ov00_021F955C();
            dest->port2[ov00_021F955C()->players] = port;
            goto accepted;
        }
    }
    gt2Reject(connection, ov00_02217A8C, -1);
    return;
accepted:
    work = ov00_021F955C();
    work->waitTick = 0;
    if (gt2Accept(connection, ov00_021F955C()->callback) == 0) {
        DWCi_StopMatching(6, -80410);
        return;
    }
    DWCi_ClearSentCommand();
    if (ov00_021F955C()->players == 0) {
        int half = maxSize >> 1;
        if (half >= 65535) {
            half = 65535;
        }
        ov00_021F955C()->maxSize = half;
    }
    entry = ov00_021EE72C(index);
    slot = ov00_021EE7A4(index);
    *entry = connection;
    work = ov00_021F955C();
    work->players++;
    slot->index = index;
    work = ov00_021F955C();
    slot->aid = work->aids[ov00_021F955C()->players - 1];
    slot->unk_02 = 0;
    slot->unk_04 = 0;
    gt2SetConnectionData(connection, slot, 0);
    DWCi_AdvanceJoin(2);
}

// Result of connecting to the next peer: 5 pending, 6 retry (up to 5 times), 0 connected.
void DWCi_GT2ConnectedCallback(void *connection, int result) {
    char name[12];
    int ret;
    if (ov00_021F955C() == NULL) {
        return;
    }
    if (ov00_021F955C()->state != 7 && ov00_021F955C()->state != 12) {
        return;
    }
    switch (result) {
    case 5:
        return;
    case 6:
        ov00_021F955C()->flag++;
        if (ov00_021F955C()->flag > 5) {
            ov00_021F955C()->flag = 0;
            DWCi_DropConnectingPeer(ov00_021F955C()->profiles[ov00_021F955C()->connections]);
            return;
        }
        OS_SNPrintf(name, sizeof(name), ov00_02217A64, ov00_021F955C()->identity);
        ret = gt2Connect(*ov00_021F955C()->peer, 0, gt2AddressToString(ov00_021F955C()->addr2[ov00_021F955C()->connections], ov00_021F955C()->port2[ov00_021F955C()->connections], 0), name, -1, 5000, ov00_021F955C()->callback, 0);
        if (ret == 1) {
            DWCi_HandleGT2Error(ret);
            return;
        }
        if (ret == 0) {
            return;
        }
        if (DWCi_DropConnectingPeer(ov00_021F955C()->profiles[ov00_021F955C()->connections])) {
            return;
        }
        break;
    default:
        if (DWCi_DropConnectingPeer(ov00_021F955C()->profiles[ov00_021F955C()->players + 1])) {
            return;
        }
        break;
    case 0:
        int index;
        void **entry;
        MatchConnSlot *slot;
        if ((index = ov00_021EE6D0()) == -1) {
            DWCi_StopMatching(6, -87100);
            return;
        }
        entry = ov00_021EE72C(index);
        slot = ov00_021EE7A4(index);
        *entry = connection;
        ov00_021F955C()->players++;
        slot->index = index;
        slot->unk_02 = 0;
        slot->unk_04 = 0;
        slot->aid = ov00_021F955C()->aids[ov00_021F955C()->players];
        gt2SetConnectionData(connection, slot);
        if (ov00_021F955C()->state == 12) {
            DWCi_AdvanceJoin(0);
        } else {
            DWCi_AdvanceJoin(1);
        }
        break;
    }
}

// Parses the comma separated numbers after the command byte of s and runs the command.
int DWCi_ProcessGPMatchCommand(int unused, void *arg, u8 *s) {
    char number[16];
    int values[128];
    int i = 0;
    while (i < 128) {
        if (DWCi_GetCommandField(number, (char *)s + 1, i) == -1) {
            break;
        }
        values[i] = strtoul(number, 0, 10);
        i++;
    }
    return DWCi_ProcessMatchCommand(*s, (int)arg, 0, 0, values, i);
}

void DWCi_ClearSentCommand(void) {
    ov00_021F955C()->unk_3CC = 255;
    ov00_021F955C()->commandRetries = 0;
}

// Ends the match with an error (kind 0: no error) and reports it to the error callback.
void DWCi_StopMatching(int kind, int code) {
    BOOL isServer, noFriend;
    int friendIndex;
    MatchWork *work;
    if (!ov00_021F955C() || !kind) {
        return;
    }
    DWCi_CloseAllConnectionsHard();
    ov00_021EC24C(kind, code);
    ov00_021F04A0(1, ov00_02217A60, 0);
    isServer = ov00_021F955C()->mode == 2;
    noFriend = ov00_021F955C()->unk_20C == 0;
    work = ov00_021F955C();
    friendIndex = ov00_021F0404(ov00_021F955C()->unk_20C);
    work->errorCallback(kind, 0, noFriend, isServer, friendIndex, ov00_021F955C()->errorParam);
    DWCi_CloseMatching();
}

void DWCi_ResetReportedPlayerCount(void) {
    if (ov00_021F955C()->mode == 2) {
        return;
    }
    ov00_021F955C()->connections = 0;
    ov00_021F955C()->limit = 0;
    qr2_send_statechanged(ov00_021F955C()->reporting);
}

// Match message from peer aid: 2 = server accepted us, 3 = peer ready (data: its max size), 4 = all ready.
void DWCi_ProcessMatchSynPacket(int aid, int message, u8 *data) {
    int i;
    MatchWork *work;
    switch (message) {
    case 2:
        if (ov00_021F955C()->state == 1) {
            if (data[0] == 1) {
                ov00_021F955C()->unk_20C = 0;
            }
            i = data[1];
            ov00_021F955C()->aids[i] = data[2];
            work = ov00_021F955C();
            work->profiles[i] = ov00_021F955C()->identity;
            if (ov00_021F955C()->mode == 0 || ov00_021F955C()->mode == 1) {
                work = ov00_021F955C();
                work->limit = ov00_021F955C()->players;
            }
            ov00_021F955C()->unk_1AD = 0;
            ov00_021F956C(9);
        }
        DWCi_SendMatchSynPacket(aid, 3);
        break;
    case 3:
        if (ov00_021F955C()->state == 16) {
            int value;
            work = ov00_021F955C();
            work->readyMask |= 1 << aid;
            value = data[0] | data[1] << 8;
            if (value > ov00_021F955C()->maxSize) {
                ov00_021F955C()->maxSize = value;
            }
            value = DWCi_GetPeerAidBitmap(0);
            if (value != ov00_021F955C()->readyMask) {
                return;
            }
            for (i = 1; i <= ov00_021F955C()->players; i++) {
                DWCi_SendMatchSynPacket(ov00_021F955C()->aids[i], 4);
            }
            ov00_021F956C(17);
        } else {
            DWCi_SendMatchSynPacket(aid, 4);
        }
        break;
    case 4:
        if (ov00_021F955C()->state == 9) {
            DWCi_AdvanceJoin(4);
        }
        break;
    }
}

// The connection to the server was closed (error: kind of the error, 0 if none).
int DWCi_HandleServerConnectionClosed(int kind, int code) {
    if (ov00_021EE568() != 5) {
        return 0;
    }
    if (ov00_021F955C()->mode == 2) {
        return 1;
    }
    if (kind != 0) {
        DWCi_StopMatching(kind, code - 80000);
        return 1;
    }
    ov00_021F955C()->aids[0] = 0;
    if (ov00_021F955C()->unk_1A9 == 1 || ov00_021F955C()->closeState == 1 || ov00_021F955C()->closeState == 2) {
        return 1;
    }
    if (ov00_021F955C()->request.active) {
        ov00_0221019C((void *)ov00_021F955C()->request.active);
        ov00_021F955C()->request.active = 0;
    }
    if (ov00_021F955C()->players) {
        if (ov00_021F955C()->closeState == 0) {
            ov00_021F955C()->closeState = 3;
            gt2CloseAllConnectionsHard(*ov00_021F955C()->peer);
        }
    } else if (ov00_021F955C()->mode == 3) {
        DWCi_StopMatching(6, -80430);
    } else if (ov00_021F955C()->unk_208) {
        DWCi_RestartWithPendingServer();
    } else if (ov00_021F955C()->state == 1) {
        MatchWork *work;
        u64 tick;
        ov00_021F956C(18);
        work = ov00_021F955C();
        tick = OS_GetTick();
        work->unk_1F0 = tick;
    } else {
        DWCi_RestartMatching(1);
    }
    return 1;
}

void DWCi_HandlePeerConnectionClosed(int profile) {
    if (ov00_021F955C()->closeState == 2) {
        return;
    }
    DWCi_DropConnectingPeer(profile);
}

// Drops the peer with the given profile id from the first count entries.
int DWCi_DeleteHostByProfileID(int profile, int count) {
    int i;
    if (!ov00_021F955C()) {
        return 0;
    }
    for (i = 0; i < count; i++) {
        if (profile == ov00_021F955C()->profiles[i]) {
            DWCi_DeleteHostByIndex(i, count);
            return 1;
        }
    }
    return 0;
}

// Removes entry index of the peer lists (count entries), returns its profile id.
int DWCi_DeleteHostByIndex(int index, int count) {
    int profile, i, n;
    MatchWork *work;
    if (!ov00_021F955C()) {
        return 0;
    }
    profile = ov00_021F955C()->profiles[index];
    work = ov00_021F955C();
    work->aidMask &= ~(1 << ov00_021F955C()->aids[index]);
    DWCi_UpdateValidAidCount();
    if (index < count - 1) {
        n = count - index - 1;
        for (i = 0; i < n; i++) {
            ov00_021F955C()->addresses[index + i] = ov00_021F955C()->addresses[index + i + 1];
            ov00_021F955C()->ports[index + i] = ov00_021F955C()->ports[index + i + 1];
            ov00_021F955C()->profiles[index + i] = ov00_021F955C()->profiles[index + i + 1];
            ov00_021F955C()->addr2[index + i] = ov00_021F955C()->addr2[index + i + 1];
            ov00_021F955C()->port2[index + i] = ov00_021F955C()->port2[index + i + 1];
            ov00_021F955C()->aids[index + i] = ov00_021F955C()->aids[index + i + 1];
        }
    }
    if (count > 0) {
        i = count - 1;
        ov00_021F955C()->addresses[i] = 0;
        ov00_021F955C()->ports[i] = 0;
        ov00_021F955C()->profiles[i] = 0;
        ov00_021F955C()->addr2[i] = 0;
        ov00_021F955C()->port2[i] = 0;
        ov00_021F955C()->aids[i] = 0;
    }
    return profile;
}

int DWCi_GetNumConnectedPeers(void) {
    if (!ov00_021F955C()) {
        return 0;
    }
    return ov00_021F955C()->players;
}

int DWCi_GetNumValidAids(void) {
    if (!ov00_021F955C()) {
        return 0;
    }
    return ov00_021F955C()->aidCount;
}

void DWCi_UpdateValidAidCount(void) {
    int n = -1, i;
    for (i = 0; i < 32; i++) {
        if (ov00_021F955C()->aidMask & (1 << i)) {
            n++;
        }
    }
    if (n == -1) {
        ov00_021F955C()->aidCount = 0;
    } else {
        ov00_021F955C()->aidCount = n;
    }
}

int DWCi_GetAIDList(u8 **aids) {
    if (!ov00_021F955C()) {
        return 0;
    }
    *aids = ov00_021F955C()->aids;
    return ov00_021F955C()->players + 1;
}

// Copies the aids of the connected peers (up to the first one not in aidMask) to ov00_0221B080.
int DWCi_GetValidAIDList(u8 **aids) {
    int i;
    if (!ov00_021F955C()) {
        return 0;
    }
    MI_CpuFill8(ov00_0221B080, 0, 32);
    for (i = 0; i <= ov00_021F955C()->aidCount; i++) {
        MatchWork *work = ov00_021F955C();
        if (!(work->aidMask & (1 << ov00_021F955C()->aids[i]))) {
            break;
        }
        ov00_0221B080[i] = ov00_021F955C()->aids[i];
    }
    *aids = ov00_0221B080;
    return ov00_021F955C()->aidCount + 1;
}

// Server only: updates the "SCM" (limit + 1), "SCN" (players + 1) and "VER" (3) keys.
int DWCi_UpdateGameServerStatus(void) {
    char value[12];
    u8 buffer[32];
    if (ov00_021F955C()->mode != 2) {
        return 0;
    }
    OS_SNPrintf(value, 12, ov00_02217A64, ov00_021F955C()->limit + 1);
    ov00_021ED470(ov00_02217AA4, value, buffer, 47);
    OS_SNPrintf(value, 12, ov00_02217A64, ov00_021F955C()->players + 1);
    ov00_021ED4A8(ov00_02217AA8, value, buffer, 47);
    OS_SNPrintf(value, 12, ov00_02217A64, 3);
    ov00_021ED4A8(ov00_02217A70, value, buffer, 47);
    return ov00_021F04A0(6, buffer, 0);
}

// Frees the matchmaking state.
void DWCi_ShutdownMatch(void) {
    ov00_021F954C(NULL);
    if (ov00_0221A928.filter) {
        ov00_021EC2EC(4, ov00_0221A928.filter, 0);
        ov00_0221A928.filter = NULL;
    }
    DWCi_ClearMatchKeys();
    if (ov00_0221A928.timer) {
        ov00_021EC2EC(4, ov00_0221A928.timer, 0);
        ov00_0221A928.timer = NULL;
    }
    ov00_0221A928.unk_1C = 0;
    ov00_0221A928.unk_1D = 0;
}

BOOL DWCi_IsShutdownMatch(void) {
    return ov00_021F955C() == NULL;
}
