#pragma force_active on
#include "global.h"

#include "dwc/match_internal.h"
#include "dwc/transport_internal.h"

// The code below is compiled as ARM (32-bit) code.
#include <nitro/code32.h>

#pragma warn_unusedvar off
#pragma warn_uninitializedvar off

// Data transport between the matched peers: reliable sends split into chunks behind an 8-byte "DT"
// header, unreliable sends, per-aid receive buffers and timeouts. The file starts with the end of the
// matchmaking code (retry check and the matchmaking work accessors).

// Header in front of every reliable send, 8 bytes.
typedef struct TransportHeader {
    u32 size;
    u16 type;      // 0x04
    char magic[2]; // 0x06, "DT"
} TransportHeader;

extern Transport *ov00_0221B0A0;
extern char ov00_02217B88[]; // "DT"

extern int memcmp(const void *, const void *, u32);
extern char *strncpy(char *, const char *, u32);
extern int ov00_021EC230(void);
extern void ov00_021EC24C(int, int);
extern int ov00_021EE490(void);
extern int ov00_021EE4AC(u8 **);
extern int ov00_021EE530(int);
extern int ov00_021EE64C(int);
extern int ov00_021EE6B0(int);
extern int ov00_021EE7B4(int);
extern void ov00_021F2EE4(int, int);
extern void ov00_021F9E28(int, const void *, int, int);
extern void ov00_021F9EA8(int, const void *, int);
extern void ov00_021F9F70(int, const void *, int);
extern void ov00_021FA054(int, const void *, int);
extern int ov00_021FA090(int);
extern void gt2Send(int, const void *, int, int);

int ov00_021F9580(int aid);
int ov00_021F9590(int aid, int flag);
int DWC_SendReliable(int aid, const void *p, int n);
int ov00_021F9630(int flag, int aid, const void *p, int n);
u32 ov00_021F9750(u32 bits, const void *p, int n);
int DWC_SendUnreliable(int aid, const void *p, int n);
int ov00_021F9850(int aid, u8 *p, int n);
int ov00_021F989C(void (*cb)(int, int));
int ov00_021F98BC(void (*cb)(int, const void *, int));
int ov00_021F98DC(void (*cb)(int));
void ov00_021F98FC(void *p, int flag, u32 n);
unsigned ov00_021F992C(const void *p);
int ov00_021F9968(int n);
int ov00_021F9988(int aid, u32 timeout);
void ov00_021F99D4(Transport *p);
void ov00_021F9A10(int aid, const void *p, int n, int flag);
void ov00_021F9A4C(int id, int n);
void ov00_021F9A8C(void);
void ov00_021F9C20(int aid);
void ov00_021F9CA8(void);
TransportChannel *ov00_021F9CBC(int aid);
int ov00_021F9CD4(int aid);
unsigned ov00_021F9CF0(int aid);
void ov00_021F9D0C(int aid, const void *p, int n, int flag);
void ov00_021F9D34(int id, const void *p, int n);

int ov00_021F94DC(int flag) {
    if (flag) {
        return 1;
    }
    if (ov00_021F955C()->mode != 3) {
        ov00_021F955C()->unk_17D++;
    }
    if (ov00_021F955C()->mode == 3 || ov00_021F955C()->unk_17D >= 5) {
        ov00_021F2EE4(6, -86420);
        return 0;
    }
    return 1;
}

void ov00_021F954C(MatchWork *work) {
    ov00_0221A928.work = work;
}

MatchWork *ov00_021F955C(void) {
    return ov00_0221A928.work;
}

void ov00_021F956C(int state) {
    ov00_0221A928.work->state = state;
}

int ov00_021F9580(int aid) {
    return ov00_021F9590(aid, 1);
}

int ov00_021F9590(int aid, int flag) {
    int n;
    if (ov00_021EC230() || (flag == 1 && !ov00_021EE530(aid)) || !ov00_021EE7B4(aid)) {
        return 0;
    }
    if (ov00_021F9CD4(aid) == 1) {
        return 0;
    }
    n = ov00_021FA090(aid);
    return n >= ov00_021F9968(flag);
}

int DWC_SendReliable(int aid, const void *p, int n) {
    return ov00_021F9630(1, aid, p, n);
}

int ov00_021F9630(int flag, int aid, const void *p, int n) {
    TransportChannel *c = ov00_021F9CBC(aid);
    u8 header[8];
    int amount;
    if (!ov00_021F9590(aid, flag)) {
        return 0;
    }
    c->sending = 1;
    c->sendBuf = (u8 *)p;
    c->sent = 0;
    c->total = n;
    ov00_021F98FC(header, flag, n);
    ov00_021F9D0C(aid, header, 8, 1);
    amount = ov00_0221B0A0->maxSize;
    if (n <= amount) {
        amount = n;
    }
    if (amount > ov00_021FA090(aid)) {
        return 1;
    }
    ov00_021F9D0C(aid, p, amount, 1);
    c->sent += amount;
    {
        int total = c->total;
        if (c->sent == total) {
            c->sending = 0;
            c->sendBuf = 0;
            c->sent = 0;
            c->total = 0;
            if (ov00_0221B0A0->sendCallback && flag == 1) {
                ov00_0221B0A0->sendCallback(total, aid);
            }
        }
    }
    return 1;
}

u32 ov00_021F9750(u32 bits, const void *p, int n) {
    u8 i;
    for (i = 0; i < 32; i++) {
        u32 bit = i ? 1 << i : 1;
        if (bits & bit) {
            if (i == ov00_021EE490()) {
                continue;
            }
            if (!DWC_SendReliable(i, p, n)) {
                bits &= ~bit;
            }
        }
    }
    return bits;
}

int DWC_SendUnreliable(int aid, const void *p, int n) {
    if (ov00_021EC230()) {
        return 0;
    }
    if (!ov00_021EE530(aid)) {
        return 0;
    }
    if (n > ov00_0221B0A0->maxSize) {
        return 0;
    }
    ov00_021F9D0C(aid, p, n, 0);
    if (ov00_0221B0A0->sendCallback) {
        ov00_0221B0A0->sendCallback(n, aid);
    }
    return 1;
}

int ov00_021F9850(int aid, u8 *p, int n) {
    TransportChannel *c = ov00_021F9CBC(aid);
    if (ov00_021F9CF0(aid) == 2) {
        return 0;
    }
    c->recvBuf = p;
    c->size = n;
    c->receiving = 1;
    c->received = 0;
    c->pending = 0;
    return 1;
}

int ov00_021F989C(void (*cb)(int, int)) {
    if (!ov00_0221B0A0) {
        return 0;
    }
    ov00_0221B0A0->sendCallback = cb;
    return 1;
}

int ov00_021F98BC(void (*cb)(int, const void *, int)) {
    if (!ov00_0221B0A0) {
        return 0;
    }
    ov00_0221B0A0->recvCallback = cb;
    return 1;
}

int ov00_021F98DC(void (*cb)(int)) {
    if (!ov00_0221B0A0) {
        return 0;
    }
    ov00_0221B0A0->timeoutCallback = cb;
    return 1;
}

void ov00_021F98FC(void *p, int flag, u32 n) {
    strncpy(((TransportHeader *)p)->magic, ov00_02217B88, 2);
    ((TransportHeader *)p)->type = flag;
    ((TransportHeader *)p)->size = n;
}

unsigned ov00_021F992C(const void *p) {
    u8 buf[8];
    MI_CpuCopy8(p, buf, 8);
    return memcmp(buf + 6, ov00_02217B88, 2) == 0 ? ((TransportHeader *)buf)->type : 0;
}

int ov00_021F9968(int n) {
    switch (n) {
    case 2:
    case 3:
    case 4:
        return 12;
    default:
        return 8;
    }
}

int ov00_021F9988(int aid, u32 timeout) {
    if (!ov00_0221B0A0) {
        return 0;
    }
    ov00_0221B0A0->channel[aid].timeout = timeout;
    ov00_0221B0A0->channel[aid].tick = OS_GetTick();
    return 1;
}

void ov00_021F99D4(Transport *p) {
    ov00_0221B0A0 = p;
    MI_CpuFill8(p, 0, 1556);
    ov00_0221B0A0->maxSize = 1465;
}

void ov00_021F9A10(int aid, const void *p, int n, int flag) {
    if (!ov00_0221B0A0) {
        return;
    }
    if (!p || !n) {
        return;
    }
    if (flag) {
        ov00_021F9D34(aid, p, n);
    } else {
        ov00_021F9E28(aid, p, n, flag);
    }
}

void ov00_021F9A4C(int id, int n) {
    if (!ov00_0221B0A0->unk_60C) {
        return;
    }
    id = ov00_021EE6B0(id);
    ov00_0221B0A0->unk_60C(n, id);
}

void ov00_021F9A8C(void) {
    u8 *clients;
    int count, i;
    if (!ov00_0221B0A0) {
        return;
    }
    count = ov00_021EE4AC(&clients);
    for (i = 0; i < count; i++) {
        int id = clients[i];
        if (ov00_021EE530(id)) {
            TransportChannel *channel = ov00_021F9CBC(id);
            if (ov00_0221B0A0->timeoutCallback && channel->timeout) {
                u64 now = OS_GetTick();
                if ((u32)(((now - channel->tick) << 6) / 0x82ea) > channel->timeout) {
                    ov00_0221B0A0->timeoutCallback(id);
                    channel->tick = now;
                }
            }
        }
        if (id != ov00_021EE490() && ov00_021F9CD4(id) == 1) {
            TransportChannel *channel = ov00_021F9CBC(id);
            int size = ov00_0221B0A0->maxSize;
            if (channel->total - channel->sent <= size) {
                size = channel->total - channel->sent;
            }
            if (ov00_021FA090(id) >= size) {
                int total;
                ov00_021F9D0C(id, channel->sendBuf + channel->sent, size, 1);
                channel->sent += size;
                total = channel->total;
                if (channel->sent == total) {
                    channel->sending = 0;
                    channel->sendBuf = 0;
                    channel->sent = 0;
                    channel->total = 0;
                    if (ov00_0221B0A0->sendCallback) {
                        ov00_0221B0A0->sendCallback(total, id);
                    }
                }
            }
        }
    }
}

void ov00_021F9C20(int aid) {
    if (!ov00_0221B0A0) {
        return;
    }
    ov00_0221B0A0->channel[aid].sent = 0;
    ov00_0221B0A0->channel[aid].received = 0;
    ov00_0221B0A0->channel[aid].total = 0;
    ov00_0221B0A0->channel[aid].pending = 0;
    ov00_0221B0A0->channel[aid].sending = 0;
    if (ov00_0221B0A0->channel[aid].receiving) {
        ov00_0221B0A0->channel[aid].receiving = 1;
    }
    ov00_0221B0A0->channel[aid].unk_22 = 0;
}

void ov00_021F9CA8(void) {
    ov00_0221B0A0 = 0;
}

TransportChannel *ov00_021F9CBC(int aid) {
    return &ov00_0221B0A0->channel[aid];
}

int ov00_021F9CD4(int aid) {
    return ov00_0221B0A0->channel[aid].sending;
}

unsigned ov00_021F9CF0(int aid) {
    return ov00_0221B0A0->channel[aid].receiving;
}

void ov00_021F9D0C(int aid, const void *p, int n, int flag) {
    int id = ov00_021EE64C(aid);
    gt2Send(id, p, n, flag);
}

void ov00_021F9D34(int id, const void *p, int n) {
    int aid = ov00_021EE6B0(id);
    switch (ov00_021F9CF0(aid)) {
    case 0: {
        unsigned flag = ov00_021F992C(p);
        if (flag < 2 || flag > 4) {
            return;
        }
        ov00_021F9EA8(aid, p, n);
        return;
    }
    case 1:
        ov00_021F9EA8(aid, p, n);
        return;
    case 2:
        ov00_021F9F70(aid, p, n);
        return;
    case 3:
        ov00_021FA054(aid, p, n);
        return;
    case 4:
        ov00_0221B0A0->channel[aid].receiving = 1;
        ov00_0221B0A0->channel[aid].received = 0;
        ov00_0221B0A0->channel[aid].pending = 0;
        return;
    default:
        ov00_021EC24C(6, -97610);
        return;
    }
}
