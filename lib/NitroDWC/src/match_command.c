#pragma force_active on
#include <string.h>

#include "global.h"

#include "dwc/match_internal.h"
#include "dwc/net_common_internal.h"

// The code below is compiled as ARM (32-bit) code.
#include <nitro/code32.h>

// Matchmaking commands: resetting the matchmaking state, the server search filter, exchanging the NAT
// negotiation addresses and sending a command to a peer (as a "GPCM" buddy message or a raw "SBCM" packet).

// "SBCM" command packet sent by ov00_021F44C8.
typedef struct MatchCommandPacket {
    MatchCommandHeader header;
    u32 data[32]; // 0x14
} MatchCommandPacket;

extern const char ov00_02217A64[]; // "%u"
extern const char ov00_02217AAC[]; // "dwc_pid"
extern const char ov00_02217AB4[]; // "dwc_mtype"
extern const char ov00_02217AC0[]; // "dwc_mresv"
extern const char ov00_02217ACC[]; // "dwc_mver"
extern const char ov00_02217AD8[]; // "dwc_eval"
extern const char ov00_02217AE4[]; // " and (%s)"
extern const char ov00_02217AF0[]; // "%s = %u"
extern const char ov00_02217AF8[]; // "%s = %d and %s != %u and maxplayers = %d and numplayers < %d and %s = %d and %s != %s"
extern const char ov00_02217B50[]; // "/%u"
extern const char ov00_02217B54[]; // "SBCM"
extern const char ov00_02217B5C[]; // "%s%dv%s"
extern const char ov00_02217B64[]; // "GPCM"
extern const char ov00_02217B6C[]; // "MAT"

extern u32 SOC_GetHostID(void);
extern char *strchr(const char *s, int c);
extern void ov00_021F9100(void);
extern int ov00_02200880(void *connection, u32 profile, const char *message);
extern u32 ov00_0220D27C(void *connection);
extern void *ov00_0220D294(void *connection);
extern u32 ov00_0220F724(u32 addr, u16 port, int flag);
extern int ov00_022100D4(void *socket, void *data, int flag, void (*callback)(void), void (*resultCallback)(int, int, MatchAddr *, MatchRequest *), MatchRequest *req);
extern void ov00_02212008(int key, const char *name);
extern u32 ov00_02212E78(void *server);
extern u16 ov00_02212E80(void *server);
extern int ov00_02212EAC(void *server);
extern u32 ov00_02212EC4(void *server);
extern u32 ov00_02212ECC(void *server);
extern void ov00_022136E4(void *list);
extern int ov00_02213838(void *list, int a, int b, u8 *keys, int count, char *filter, int c);
extern int ov00_0221386C(void *list, u32 addr, u32 port, MatchCommandPacket *packet, int size);
extern int ov00_022138C4(void *list, u32 addr, u32 port, void *data);
extern void ov00_0221396C(void *list);
extern u32 ov00_022139E8(void *list);

int ov00_021F4304(u8 command, u32 profile, u32 addr, u16 port, u32 *data, int count);

void ov00_021F381C(int mode) {
    MatchWork *w;
    ov00_021F955C()->flag = 0;
    ov00_021F955C()->retryCount = 0;
    w = ov00_021F955C();
    w->unk_17E = ov00_021ED5E4(0x10000);
    ov00_021F955C()->retryProfile = 0;
    w = ov00_021F955C();
    w->retryTick = 0;
    w = ov00_021F955C();
    w->waitTick = 0;
    ov00_021F955C()->initial = 0;
    ov00_021F955C()->unk_1A9 = 0;
    ov00_021F955C()->unk_1AA = 0;
    ov00_021F955C()->readyRetries = 0;
    ov00_021F955C()->closeRetries = 0;
    ov00_021F955C()->unk_1A7 = 0;
    ov00_021F955C()->closeState = 0;
    ov00_021F955C()->closeDelay = 0;
    ov00_021F955C()->remotePort = 0;
    ov00_021F955C()->remoteAddr = 0;
    w = ov00_021F955C();
    w->readyTick = 0;
    w = ov00_021F955C();
    w->unk_1F0 = 0;
    MIi_CpuClear32(0, (u32 *)&ov00_021F955C()->unk_3CC, 0x98);
    if (mode == 2) {
        w = ov00_021F955C();
        w->connections = ov00_021F955C()->players;
        if (ov00_021F955C()->mode == 3) {
            ov00_021F956C(1);
        } else if (ov00_021F955C()->mode == 2) {
            ov00_021F956C(10);
        }
        return;
    }
    ov00_021F955C()->players = 0;
    ov00_021F955C()->aidCount = 0;
    ov00_021F955C()->connections = 0;
    ov00_021F955C()->unk_17 = 0;
    ov00_021F955C()->unk_20 = 0;
    ov00_021F955C()->unk_E8 = 0;
    ov00_021F955C()->current = 0;
    ov00_021F955C()->maxSize = 0;
    ov00_021F955C()->active = 0;
    w = ov00_021F955C();
    w->activeTick = 0;
    ov00_021F955C()->delay = 0;
    w = ov00_021F955C();
    w->time = 0;
    ov00_021F955C()->readyMask = 0;
    ov00_021F955C()->target = 0;
    ov00_021F955C()->unk_208 = 0;
    ov00_021F955C()->aidMask = 0;
    MIi_CpuClear32(0, ov00_021F955C()->addresses, 128);
    MIi_CpuClear16(0, ov00_021F955C()->ports, 64);
    MIi_CpuClear32(0, (u32 *)ov00_021F955C()->profiles, 128);
    MIi_CpuClear32(0, (u32 *)&ov00_021F955C()->request, 12);
    MIi_CpuClear32(0, ov00_021F955C()->addr2, 128);
    MIi_CpuClear16(0, ov00_021F955C()->port2, 64);
    MI_CpuFill8(ov00_021F955C()->aids, 0, 32);
    MIi_CpuClear32(0, &ov00_021F955C()->unk_348, 0x84);
    if (mode == 1) {
        if (ov00_021F955C()->mode == 0) {
            ov00_021F956C(3);
        } else if (ov00_021F955C()->mode == 1) {
            ov00_021F956C(4);
        }
        return;
    }
    ov00_021F955C()->mode = 0;
    ov00_021F955C()->limit = 0;
    ov00_021F955C()->unk_18 = 0;
    ov00_021F955C()->unk_20C = 0;
    ov00_021F955C()->unk_1A6 = 0;
    ov00_021F955C()->stopPending = 0;
    ov00_021F955C()->unk_1AD = 0;
    w = ov00_021F955C();
    w->unk_1F8 = 0;
    ov00_021F955C()->unk_46C = NULL;
    ov00_021F955C()->unk_470 = NULL;
}

void ov00_021F3BB4(int mode, int limit, MatchErrorCallback callback, void *param) {
    ov00_021F381C(0);
    ov00_021F955C()->mode = mode;
    ov00_021F955C()->limit = limit;
    ov00_021F955C()->errorCallback = callback;
    ov00_021F955C()->errorParam = param;
    ov00_021F955C()->unk_17D = 0;
    ov00_021F955C()->aids[0] = 0;
    ov00_02212008(50, ov00_02217AAC);
    ov00_02212008(51, ov00_02217AB4);
    ov00_02212008(52, ov00_02217AC0);
    ov00_02212008(53, ov00_02217ACC);
    ov00_02212008(54, ov00_02217AD8);
}

void ov00_021F3C5C(void) {
    if (!ov00_021F955C()) {
        return;
    }
    if (ov00_021F955C()->list) {
        if (ov00_0221A928.busy) {
            ov00_0221A928.unk_14 = 1;
        } else {
            ov00_022136E4(ov00_021F955C()->list);
            ov00_021F955C()->list = NULL;
        }
    }
    ov00_021F125C();
    ov00_021F956C(0);
    if (ov00_0221A928.filter) {
        ov00_021EC2EC(4, ov00_0221A928.filter, 0);
        ov00_0221A928.filter = NULL;
    }
    ov00_021F7AD8();
    ov00_021F955C()->unk_18 = 1;
}

// Starts the server list update with the matchmaking keys and the filter for the current state.
int ov00_021F3CF8(int profile) {
    struct {
        u8 keys[161];
        char filter[256];
    } search;

    int count = 7, i, n, result;
    u8 *dst;
    search.keys[0] = 8;
    search.keys[1] = 10;
    search.keys[2] = 50;
    search.keys[3] = 51;
    search.keys[4] = 52;
    search.keys[5] = 53;
    search.keys[6] = 54;
    if (ov00_021F955C()->mode == 0 || ov00_021F955C()->mode == 1) {
        MatchKeyEntry *entry = ov00_0221A948;
        dst = search.keys + 7;
        for (i = 0; i < 154; i++, entry++) {
            u8 id = entry->id;
            if (id) {
                *dst++ = id;
                count++;
            }
        }
    }
    switch (ov00_021F955C()->state) {
    case 0:
    case 1:
    default:
        break;
    case 3:
        if (ov00_021F955C()->unk_208 == 0) {
            volatile MatchWork *a = ov00_021F955C();
            volatile MatchWork *b = ov00_021F955C();
            volatile MatchWork *c = ov00_021F955C();
            u32 identity = a->identity;
            u8 limit = b->limit;
            u8 mode = c->mode;
            n = ov00_021F3EF4(search.filter, identity, limit, mode);
            if (ov00_0221A928.filter) {
                OS_SNPrintf(search.filter + n, 256 - n, ov00_02217AE4, ov00_0221A928.filter);
            }
            break;
        }
        profile = ov00_021F955C()->unk_208;
    case 2:
    case 4:
    case 5:
        OS_SNPrintf(search.filter, 256, ov00_02217AF0, ov00_02217AAC, profile);
        ov00_021F955C()->target = profile;
        break;
    }
    ov00_0221396C(ov00_021F955C()->list);
    i = 0;
    do {
        result = ov00_02213838(ov00_021F955C()->list, 1, 0, search.keys, count, search.filter, 6);
        if (result == 0 || result != 2) {
            break;
        }
        i++;
    } while (i < 5);
    if (result == 0) {
        MatchWork *w = ov00_021F955C();
        w->pending = OS_GetTick() + 0xEFB5F7;
    }
    return result;
}

int ov00_021F3EF4(char *out, int identity, int limit, int mode) {
    return OS_SNPrintf(out, 256, ov00_02217AF8, ov00_02217ACC, 3, ov00_02217AAC, identity, limit, limit, ov00_02217AB4, mode, ov00_02217AC0, ov00_02217AAC);
}

// Mode 0: the server was found, send it our address (command 6) unless it is behind the same NAT; otherwise
// start the NAT negotiation for value.
int ov00_021F3F5C(int mode, u32 value, void *server) {
    MatchWork *w = ov00_021F955C();
    int index = w->connections, retry;
    unsigned result = 0;
    u32 local[2];
    if (mode == 0) {
        w = ov00_021F955C();
        value = (w->identity & 0xFFFF) | (ov00_021F955C()->unk_17E << 16);
        if (ov00_02212EAC(server)) {
            u32 ip = ov00_02212E78(server);
            if (ip != ov00_022139E8(ov00_021F955C()->list)) {
                retry = 1;
            } else {
                w = ov00_021F955C();
                w->addr2[index] = ov00_02212EC4(server);
                w = ov00_021F955C();
                w->port2[index] = ov00_02212ECC(server);
                retry = result;
            }
        } else {
            u32 ip = (u16)SOC_GetHostID();
            // 192.168.x.x, 172.16-31.x.x, 10.x.x.x
            if ((u16)ip == 0xA8C0 || ((ip & 0xFF) == 0xAC && (ip & 0xFF00) >= 0x1000 && (ip & 0xFF00) <= 0x1F00) || (ip & 0xFF) == 10) {
                retry = 1;
            } else {
                w = ov00_021F955C();
                w->addr2[index] = ov00_02212E78(server);
                w = ov00_021F955C();
                w->port2[index] = ov00_02212E80(server);
                retry = 0;
            }
        }
        if (retry) {
            w = ov00_021F955C();
            w->unk_17E = ov00_021ED5E4(0x10000);
            ov00_021F955C()->request.active = value;
        } else {
            local[0] = SOC_GetHostID();
            local[1] = ov00_0220D27C(*ov00_021F955C()->peer);
            w = ov00_021F955C();
            value = ov00_02212E78(server);
            {
                int error = ov00_021F4304(6, w->profiles[index], value, ov00_02212E80(server), local, 2);
                ov00_021F955C()->commandRetries = 0;
                if (error) {
                    return 2;
                }
            }
            ov00_021F955C()->request.active = 0;
        }
        ov00_021F955C()->request.flag = 0;
        ov00_021F955C()->request.retries = 0;
        w = ov00_021F955C();
        w->request.port = ov00_02212E80(server);
        w = ov00_021F955C();
        w->request.address = ov00_02212E78(server);
    } else {
        ov00_021F955C()->request.flag = 1;
        ov00_021F955C()->request.retries = result;
        ov00_021F955C()->request.port = result;
        ov00_021F955C()->request.address = result;
        ov00_021F955C()->request.active = value;
        retry = 1;
    }
    if (retry) {
        result = ov00_021F4248(&ov00_021F955C()->request);
    } else {
        u32 socket = (u32)ov00_0220D294(*ov00_021F955C()->peer);
        ov00_021F9104(0, socket, NULL, &ov00_021F955C()->request);
        w = ov00_021F955C();
        w->waitTick = 0;
    }
    return result;
}

int ov00_021F4248(MatchRequest *req) {
    int i, result;
    if (req->flag == 0) {
        MatchWork *w = ov00_021F955C();
        u32 addr = ov00_0220F724(req->address, 0, 0);
        if (ov00_021F82EC(ov00_022138C4(w->list, addr, req->port, (void *)req->active)) != 0) {
            return 2;
        }
    }
    for (i = 0; i < 5; i++) {
        void *socket = ov00_0220D294(*ov00_021F955C()->peer);
        result = ov00_022100D4(socket, (void *)req->active, req->flag, ov00_021F9100, ov00_021F9104, req);
        if (result == 0) {
            return result;
        }
        if (result != 3) {
            return result;
        }
    }
    return result;
}

// Sends command to profile, as a raw packet to addr:port while the server connection is used for the
// NAT negotiation, otherwise as a buddy message ("GPCM3vMAT", command, "/"-separated data). Commands 2, 6, 8
// and 9 are kept for resending.
int ov00_021F4304(u8 command, u32 profile, u32 addr, u16 port, u32 *data, int count) {
    char number[16];
    char args[512];
    int length = 0, result, i, n;
    if (ov00_021F955C()->mode == 0 || ((ov00_021F955C()->mode == 3 || ov00_021F955C()->unk_1A6 != 0) && command == 6)) {
        result = ov00_021F44C8(command, addr, port, data, count);
    } else {
        if (data && count) {
            length = OS_SNPrintf(args, 512, ov00_02217A64, data[0]);
            for (i = 1; i < count; i++) {
                n = OS_SNPrintf(number, 16, ov00_02217B50, data[i]);
                if ((u32)(length + n + 1) > 512) {
                    break;
                }
                MI_CpuCopy8(number, args + length, n);
                length += n;
            }
        }
        args[length] = 0;
        result = ov00_021F45E0(ov00_021F955C()->unk_00, command, profile, args);
    }
    if (command == 2 || command == 6 || command == 8 || command == 9) {
        ov00_021F955C()->unk_3CC = command;
        ov00_021F955C()->commandPort = port;
        ov00_021F955C()->commandAddr = addr;
        ov00_021F955C()->commandProfile = profile;
        ov00_021F955C()->commandSize = count;
        {
            MatchWork *w = ov00_021F955C();
            w->commandTick = OS_GetTick();
        }
        if (data && count) {
            MIi_CpuCopy32(data, (u32 *)ov00_021F955C()->commandData, count * 4);
        }
    }
    return result;
}

int ov00_021F44C8(int command, u32 addr, u32 port, const u32 *data, u32 count) {
    MatchCommandPacket packet;
    int i;
    int result;
    if (count > 32) {
        count = 32;
    }
    if (data == NULL || count == 0) {
        count = 0;
    } else {
        MIi_CpuCopy32(data, packet.data, count * 4);
    }
    memcpy(packet.header.magic, ov00_02217B54, 5);
    packet.header.version = 3;
    packet.header.type = command;
    packet.header.size = count * 4;
    packet.header.value = ov00_021F955C()->port;
    packet.header.arg0 = ov00_021F955C()->addr;
    packet.header.arg1 = ov00_021F955C()->identity;
    for (i = 0; i < 5; i++) {
        result = ov00_0221386C(ov00_021F955C()->list, ov00_0220F724(addr, 0, 0), port, &packet, packet.header.size + 20);
        if (result == 0) {
            return result;
        }
        if (result != 2) {
            return result;
        }
    }
    return result;
}

int ov00_021F45E0(void *connection, int command, u32 profile, const char *args) {
    char message[512];
    int n = OS_SNPrintf(message, 512, ov00_02217B5C, ov00_02217B64, 3, ov00_02217B6C);
    char *end;
    message[n] = command;
    (message + 1)[n] = 0;
    end = message + 1 + n;
    if (args) {
        u32 size = strlen(args);
        if (n + 2 + size > 512) {
            size = 511 - (n + 1);
        }
        MI_CpuCopy8(args, end, size);
        end[size] = 0;
    }
    return ov00_02200880(connection, profile, message);
}

// Copies the index-th "/"-separated field of s to out; returns its length, or -1.
int ov00_021F46A4(char *out, const char *s, int index) {
    char *end = strchr(s, 0), *stop;
    int i, n;
    for (i = 0; i < index; i++) {
        stop = strchr(s, '/');
        if (!stop) {
            return -1;
        }
        s = stop + 1;
    }
    stop = strchr(s, '/');
    if (!stop) {
        stop = end;
    }
    if (s == stop) {
        return -1;
    }
    n = stop - s;
    MI_CpuCopy8(s, out, n);
    out[n] = 0;
    return n;
}
