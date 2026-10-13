#pragma force_active on
#include "global.h"

#include "dwc/ap_connect_internal.h"
// The code below is compiled as ARM (32-bit) code.
#include <nitro/code32.h>

typedef void (*RecvCb)(int, void *, int);

typedef struct RecvEntry {
    u32 unk0;
    u8 *buf;
    int size;
    u32 unkC;
    int pos;
    u32 unk14;
    int total;
    u8 unk1C;
    u8 state;
    u8 saved;
    u8 unk1F;
    u16 unk20;
    u16 type;
    u32 tickLo;
    u32 tickHi;
    int tickOn;
} RecvEntry;

typedef struct RecvWork {
    RecvEntry entries[32];
    u32 unk600;
    RecvCb cb;
    int tickOn;
} RecvWork;

extern RecvWork *ov00_0221B0A0;
typedef void (*HttpDoneCb)(void *, int, int, int);
typedef void (*HttpProgCb)(int, void *, int, int, int, int);

typedef struct HttpInit {
    int param;
    HttpDoneCb cb;
    HttpProgCb prog;
    int flag;
} HttpInit;

typedef struct HttpReq {
    HttpInit init;
    void *buf;
    int handle;
    struct HttpReq *next;
} HttpReq;

typedef struct HttpList {
    HttpReq *head;
    int count;
} HttpList;

extern HttpList ov00_0221B0A4;

typedef struct Ctx {
    void *(*alloc)(int, int);
    void (*free)(int, void *, int);
    u8 flags;
    u8 state;
    u8 prevState;
    u8 unkB;
    int unkC;
    int status;
    u8 unk14;
    u8 unk15;
    u8 maxState;
    u8 unk17;
    char ssid[10];
    u8 unk22;
    u8 pad23;
} Ctx;

typedef struct Globals {
    void *p0;
    void *p4;
    void *p8;
    Ctx *ctx;
    ApConnectWork *work;
} Globals;

extern Globals ov00_0221B0AC;
#define RSSI_CALC(v) ((u8)(((v) & 2) ? (u8)((v) >> 2) : (u8)(((v) >> 2) + 25)))

typedef struct InitParam {
    void *(*alloc)(int, int);
    void (*free)(int, void *, int);
    u8 u8_;
    u8 u9;
    u8 uA;
    u8 uB;
} InitParam;

typedef struct WcmEvent {
    s16 notify;
    s16 result;
    void *ptr;
    int reason;
} WcmEvent;

extern char ov00_02216060[];
extern char ov00_02217B8C[];
extern char ov00_02216058[];
extern char ov00_02216068[];
extern int ov00_021EE6B0(int);
extern u8 ov00_021F9CF0(int, void *, int);
extern int ov00_021F992C(void *);
extern RecvEntry *ov00_021F9CBC(int);
extern void DWCi_ProcessMatchSynPacket(int, int, int);
extern void ov00_021EC24C(int, int);
extern int ov00_021EE64C(void);
extern int gt2GetOutgoingBufferFreeSpace(int);
extern void ov38_0221D7B4(void);
extern void ov38_0221D808(void);
extern void ov38_0221DAEC(void);
extern int ov00_021EC230(void);
extern void ov00_021EC2EC(int, void *, int);
extern void *ov00_021EC2A8(int, int);
extern int ov38_0221D864(const char *, int, void *, HttpReq *);
extern int ghttpGetExA(const char *, int, void *, int, int, int, int, void *, void *, void *);
extern void DWCi_BM_GetApInfo(void *);
extern int WCM_Init(void *, int);
extern u8 ov00_021FCBD8(void);
extern u8 ov00_021FC408(void);
extern u8 ov00_021FC32C(void);
extern u8 ov00_021FCC60(void);
extern void *WCM_GetApEssid(int);
extern void DC_InvalidateRange(void *, u32);
extern void ov00_021FD17C(void *, char *);
extern int strncmp(const char *, const char *, u32);
extern void ov00_021FC4CC(u16);
extern int ov00_021FD1C8(u8 *);
extern int ov00_021FD11C(u8 *);
extern int WCM_GetPhase(void);
extern void WCM_ConnectAsync(void *, void *, u32);
extern void ov00_021FD1EC(u8 *, u8 *);
extern void ov00_021FD154(u8 *, u8 *);
extern void ov00_021EA518(void);
extern void ov00_021EA420(void);
extern void WCM_Finish(void);
extern void WCM_CleanupAsync(void);
extern void WCM_EndSearchAsync(void);
extern void WCM_DisconnectAsync(void);
extern void WCM_TerminateAsync(void);
extern int SOCL_CalmDown(void);
extern int SOC_Cleanup(void);
extern u8 ov00_021FC150(ApConnectWork *);
extern u8 ov00_021FC258(ApConnectWork *);
extern u8 ov00_021FC2E8(ApConnectWork *);

void ov00_021F9E28(int a, void *p, int len);
void ov00_021F9EA8(int idx, void *p, int len);
void ov00_021F9F70(int idx, void *p, int len);
void ov00_021FA054(int a, int b);
int ov00_021FA090(void);
int ov00_021FA0B4(void);
int ov00_021FA0D8(void);
int ov00_021FA118(void);
int ov00_021FA138(void *r, int res, void *buf, int len, void *param);
void ov00_021FA1D4(void *r, int a, void *b, int c, int d, int e, HttpReq *req);
int ov00_021FA21C(const char *url, HttpDoneCb cb, int param);
int ov00_021FA2E4(const char *url, int len, int flag, int *post, HttpProgCb prog, HttpDoneCb cb, int param);
int ov00_021FA488(const char *url, int len, int flag, HttpProgCb prog, HttpDoneCb cb, int param);
int ov00_021FA4B0(int err);
HttpReq *ov00_021FA60C(HttpInit *init);
void ov00_021FA668(HttpReq *req);
void ov00_021FA6EC(void);
int ov00_021FA75C(InitParam *param);
int ov00_021FA904(void);
int ov00_021FA9D4(void);
u8 ov00_021FAA40(void);
int ov00_021FAA70(void *dst);
int ov00_021FAAC8(void);
void ov00_021FAB04(const u8 *ssid, const void *key, int type, const void *name, int x);
void ov00_021FAB68(const u8 *ssid, const void *key, int type);
void ov00_021FAC20(int idx, const void *src);
void *ov00_021FAC4C(int kind, int size);
void ov00_021FAC8C(int kind, void *p, int size);
void ov00_021FACF0(void);
void *ov00_021FAE14(int kind);
void ov00_021FAE6C(u8 st);
u8 ov00_021FAECC(void);
void ov00_021FAEE8(int v);
int ov00_021FAF0C(void);
void ov00_021FAF20(u8 x);
u8 ov00_021FAFC8(u8 x);
void ov00_021FAFD8(void);
int ov00_021FB010(void);
void ov00_021FB0B0(ApBss *bss);
int ov00_021FB21C(ApBss *ap);
int ov00_021FB2B0(ApBss *ap, int n, ApSsidEntry *s);
int ov00_021FB32C(ApBss *ap, ApConnectWork *w);
int ov00_021FB3BC(int x, ApBss *bss, u8 y, ApConnectWork *w);
void ov00_021FB484(u8 x, ApBss *bss, u8 y, ApConnectWork *w);
void ov00_021FB4D0(int idx, ApBss *bss, u8 y, ApConnectWork *w);
void ov00_021FB53C(int i, ApConnectWork *w);
void ov00_021FB62C(WcmEvent *ev);
u8 ov00_021FB6B8(void);
u8 ov00_021FB6FC(ApConnectWork *w);
u8 ov00_021FB7F4(ApConnectWork *w);
u8 ov00_021FB930(ApConnectWork *w);
u32 ov00_021FBACC(ApConnectWork *w);
u32 ov00_021FBAE8(ApConnectWork *w);
BOOL ov00_021FBB04(ApConnectWork *w, int kind, u8 *out);
int ov00_021FBBF0(u8 *st);
int ov00_021FBC78(void);
int ov00_021FBD08(void);
u8 ov00_021FBD38(void);
int ov00_021FBD5C(void);
int ov00_021FBDAC(int v);
int ov00_021FBDF0(void);
int ov00_021FBDF8(Ctx *c);
static inline int c800(void);
static inline int defaultError(int reason);
int ov00_021FBE14(Ctx *p);
u8 ov00_021FBF24(int kind);
int ov00_021FBFB8(ApBss *ap);
u8 ov00_021FC004(ApConnectWork *w);
u8 ov00_021FC064(ApConnectWork *w);

void ov00_021F9E28(int a, void *p, int len) {
    int idx = ov00_021EE6B0(a);
    RecvWork *g = ov00_0221B0A0;
    RecvEntry *e = &g->entries[idx];
    if (e->buf == NULL) {
        return;
    }
    if (e->size < len) {
        return;
    }
    if (g->cb != NULL) {
        g->cb(idx, p, len);
    }
    if (ov00_0221B0A0->tickOn && e->tickOn) {
        *(u64 *)&e->tickLo = OS_GetTick();
    }
}

void ov00_021F9EA8(int idx, void *p, int len) {
    u32 hdr[2];
    RecvEntry *e = &ov00_0221B0A0->entries[idx];
    int type;
    e->saved = ov00_021F9CF0(idx, p, len);
    type = ov00_021F992C(p);
    switch (type) {
    case 1:
        if (len != 8) {
            return;
        }
        MI_CpuCopy8(p, hdr, 8);
        e->total = hdr[0];
        e->pos = 0;
        if (e->buf != NULL && e->size >= e->total) {
            e->state = 2;
        } else {
            e->state = 4;
        }
        break;
    case 2:
    case 3:
    case 4:
        e->state = 3;
        break;
    }
    e->type = type;
}

void ov00_021F9F70(int idx, void *p, int len) {
    RecvEntry *e = &ov00_0221B0A0->entries[idx];
    if (ov00_021F9CF0(idx, p, len) == 2) {
        int pos = e->pos;
        if (pos + len > e->size) {
            ov00_021EC24C(6, -97620);
            return;
        }
        MI_CpuCopy8(p, e->buf + pos, len);
    }
    e->pos += len;
    if (e->pos == e->total) {
        int total = e->total;
        e->state = 1;
        e->pos = 0;
        e->total = 0;
        if (ov00_0221B0A0->cb != NULL) {
            ov00_0221B0A0->cb(idx, e->buf, total);
        }
    }
    if (ov00_0221B0A0->tickOn && e->tickOn) {
        *(u64 *)&e->tickLo = OS_GetTick();
    }
}

void ov00_021FA054(int a, int b) {
    RecvEntry *e = ov00_021F9CBC(a);
    e->state = e->saved;
    switch (e->type) {
    case 2:
    case 3:
    case 4:
        DWCi_ProcessMatchSynPacket(a, e->type, b);
    }
}

int ov00_021FA090(void) {
    int r = gt2GetOutgoingBufferFreeSpace(ov00_021EE64C()) - 519;
    if (r <= 0) {
        r = 0;
    }
    return r;
}

int ov00_021FA0B4(void) {
    ov38_0221D7B4();
    ov00_0221B0A4.count++;
    return 1;
}

int ov00_021FA0D8(void) {
    if (ov00_0221B0A4.count <= 0) {
        return 1;
    }
    ov38_0221D808();
    if (--ov00_0221B0A4.count == 0) {
        ov00_021FA6EC();
    }
    return 1;
}

int ov00_021FA118(void) {
    if (ov00_021EC230()) {
        return 0;
    }
    ov38_0221DAEC();
    return 1;
}

int ov00_021FA138(void *r, int res, void *buf, int len, void *param) {
    HttpReq *req = param;
    HttpDoneCb cb = req->init.cb;
    int flag = req->init.flag;
    if (cb != NULL) {
        if (res == 0) {
            cb(buf, len, res, req->init.param);
        } else {
            ov00_021FA4B0(res);
            cb(NULL, 0, res, req->init.param);
        }
    }
    if (res != 0 || flag == 1) {
        if (req->buf == NULL) {
            flag = 1;
        } else {
            ov00_021EC2EC(4, req->buf, 0);
        }
    }
    ov00_021FA668(req);
    return flag ? 1 : 0;
}

void ov00_021FA1D4(void *r, int a, void *b, int c, int d, int e, HttpReq *req) {
    if (req->init.prog == NULL) {
        return;
    }
    req->init.prog(a, b, c, d, e, req->init.param);
}

int ov00_021FA21C(const char *url, HttpDoneCb cb, int param) {
    HttpInit init;
    int r;
    HttpReq *req;
    if (ov00_021EC230()) {
        return -8;
    }
    init.param = param;
    init.cb = cb;
    init.prog = NULL;
    init.flag = 1;
    req = ov00_021FA60C(&init);
    if (req == NULL) {
        ov00_021FA4B0(-5);
        cb(NULL, 0, -5, param);
        return -5;
    }
    r = ov38_0221D864(url, 0, ov00_021FA138, req);
    if (r < 0) {
        ov00_021FA4B0(r);
        cb(NULL, 0, r, param);
        ov00_021FA668(req);
    }
    req->handle = r;
    return r;
}

int ov00_021FA2E4(const char *url, int len, int flag, int *post, HttpProgCb prog, HttpDoneCb cb, int param) {
    HttpInit init;
    void *buf = NULL;
    HttpReq *req;
    int r;
    if (ov00_021EC230()) {
        return -8;
    }
    init.param = param;
    init.cb = cb;
    init.prog = prog;
    init.flag = flag;
    req = ov00_021FA60C(&init);
    if (req == NULL) {
        ov00_021FA4B0(-5);
        cb(buf, 0, -5, param);
        return -5;
    }
    if (len > 0) {
        buf = ov00_021EC2A8(4, len);
        if (buf == NULL) {
            ov00_021FA4B0(-5);
            cb(NULL, 0, -5, param);
            ov00_021FA668(req);
            return -5;
        }
        req->buf = buf;
    }
    if (post != NULL) {
        r = ghttpGetExA(url, 0, buf, len, *post, 0, 0, ov00_021FA1D4, ov00_021FA138, req);
    } else {
        r = ghttpGetExA(url, 0, buf, len, 0, 0, 0, ov00_021FA1D4, ov00_021FA138, req);
    }
    if (r < 0) {
        ov00_021FA4B0(r);
        cb(NULL, 0, r, param);
        if (req->buf != NULL) {
            ov00_021EC2EC(4, req->buf, 0);
        }
        ov00_021FA668(req);
    }
    req->handle = r;
    return r;
}

int ov00_021FA488(const char *url, int len, int flag, HttpProgCb prog, HttpDoneCb cb, int param) {
    return ov00_021FA2E4(url, len, flag, NULL, prog, cb, param);
}

int ov00_021FA4B0(int err) {
    int type = 7;
    int code = -98000;
    if (err == 0) {
        return 0;
    }
    switch (err) {
    case -7:
        code += -800;
        break;
    case -6:
        code += -810;
        break;
    case -5:
        code += -840;
        break;
    case -4:
    case -3:
    case -2:
        code += -820;
        break;
    case -1:
        code += -830;
        break;
    case 1:
    case 19:
        code += -1;
        type = 9;
        break;
    case 2:
        code += -840;
        break;
    case 3:
        code += -850;
        break;
    case 4:
        code += -30;
        break;
    case 5:
        code += -50;
        break;
    case 6:
    case 11:
    case 12:
        code += -20;
        break;
    case 7:
        code += -860;
        break;
    case 8:
    case 9:
    case 10:
        code += -870;
        break;
    case 13:
    case 14:
        code += -880;
        break;
    case 15:
        code += -890;
        break;
    case 16:
        code += -900;
        break;
    case 17:
        code += -910;
        break;
    }
    ov00_021EC24C(type, code);
    return err;
}

HttpReq *ov00_021FA60C(HttpInit *init) {
    HttpReq *req = ov00_021EC2A8(4, sizeof(HttpReq));
    if (req == NULL) {
        return NULL;
    }
    req->init = *init;
    req->next = NULL;
    req->buf = NULL;
    if (ov00_0221B0A4.head == NULL) {
        ov00_0221B0A4.head = req;
        return req;
    } else {
        req->next = ov00_0221B0A4.head;
        ov00_0221B0A4.head = req;
        return req;
    }
}

void ov00_021FA668(HttpReq *req) {
    HttpReq *p = ov00_0221B0A4.head;
    HttpReq *q;
    if (p == NULL) {
        return;
    }
    if (p == req) {
        HttpReq *next = p->next;
        ov00_021EC2EC(4, p, 0);
        ov00_0221B0A4.head = next;
        return;
    }
    for (q = p->next; q != NULL; q = q->next) {
        if (q != req) {
            p = q;
        } else {
            HttpReq *t = p->next;
            p->next = t->next;
            ov00_021EC2EC(4, t, 0);
            return;
        }
    }
}

void ov00_021FA6EC(void) {
    HttpReq *p = ov00_0221B0A4.head;
    while (p != NULL) {
        HttpReq *cur = p;
        p = p->next;
        if (cur->buf != NULL) {
            ov00_021EC2EC(4, cur->buf, 0);
        }
        ov00_021EC2EC(4, cur, 0);
    }
    ov00_0221B0A4.head = NULL;
}

int ov00_021FA75C(InitParam *param) {
    Ctx *ctx;
    ApConnectWork *w;
    u32 *p8;
    int r;
    ov00_0221B0AC.ctx = param->alloc(1, sizeof(Ctx));
    MIi_CpuClear32(0, (u32 *)ov00_0221B0AC.ctx, sizeof(Ctx));
    ctx = ov00_0221B0AC.ctx;
    ctx->alloc = param->alloc;
    ctx->free = param->free;
    ctx->state = 1;
    ctx->maxState = 1;
    ctx->flags = 1;
    ov00_0221B0AC.work = ov00_021FAC4C(0x10, sizeof(ApConnectWork));
    ov00_0221B0AC.p0 = ov00_021FAC4C(2, 0x2300);
    ov00_0221B0AC.p4 = ov00_021FAC4C(4, 0x58);
    ov00_0221B0AC.p8 = ov00_021FAC4C(8, 0xc);
    MIi_CpuClear32(0, (u32 *)ov00_0221B0AC.work, sizeof(ApConnectWork));
    MIi_CpuClear32(0, (u32 *)ov00_0221B0AC.p0, 0x2300);
    MIi_CpuClear32(0, (u32 *)ov00_0221B0AC.p4, 0x58);
    MIi_CpuClear32(0, (u32 *)ov00_0221B0AC.p8, 0xc);
    w = ov00_0221B0AC.work;
    w->unk_D0A = param->u8_;
    w->unk_D0B_0 = param->u9;
    p8 = ov00_0221B0AC.p8;
    p8[0] = (u32)param->alloc;
    p8[1] = (u32)param->free;
    p8[2] = 0;
    w->profileSel = param->uA;
    w->unk_D0C_4 = param->uB;
    DWCi_BM_GetApInfo(w);
    r = WCM_Init(ov00_0221B0AC.p0, 0x2300);
    if (r == 1 || r > 4) {
        ov00_021FACF0();
        return 0;
    }
    return 1;
}

int ov00_021FA904(void) {
    u8 st = ov00_021FAECC();
    int r;
    if (st == 1) {
        st = ov00_021FCBD8();
    } else if (st < 7) {
        OSIntrMode en = OS_DisableInterrupts();
        st = ov00_021FC408();
        ov00_021FAE6C(st);
        OS_RestoreInterrupts(en);
    } else if (st < 9) {
        st = ov00_021FB6B8();
    } else if (st < 10) {
        st = ov00_021FC32C();
    } else if (st < 16) {
        st = ov00_021FCC60();
    } else if (st == 17) {
        st = ov00_021FBD38();
    }
    ov00_021FAE6C(st);
    if (st == 16) {
        r = ov00_021FB010();
        ov00_021FAFD8();
        return r;
    }
    if (st != 18) {
        return 0;
    }
    ov00_021FAFD8();
    return -1;
}

int ov00_021FA9D4(void) {
    u8 st = ov00_021FAECC();
    if (st <= 1) {
        return 0;
    }
    if (st < 7) {
        return 1;
    }
    if (st == 9) {
        return 4;
    }
    if (st < 10) {
        return 2;
    }
    if (st == 11) {
        return 4;
    }
    if (st < 16) {
        return 3;
    }
    if (st == 16) {
        return 5;
    }
    if (st == 17) {
        return 4;
    }
    return ov00_021FBD5C();
}

u8 ov00_021FAA40(void) {
    u8 r = 0xff;
    u8 st = ov00_021FAECC();
    if (st >= 10 && st <= 16) {
        r = ov00_0221B0AC.ctx->unk17;
    }
    return r;
}

int ov00_021FAA70(void *dst) {
    int r = 0;
    u8 st = ov00_021FAECC();
    if (st >= 10 && st <= 16) {
        Ctx *ctx = ov00_0221B0AC.ctx;
        if (ctx->unk17 == 4 || ctx->unk17 == 8) {
            MI_CpuCopy8(ctx->ssid, dst, 10);
            r = 1;
        }
    }
    return r;
}

int ov00_021FAAC8(void) {
    u8 st = ov00_021FAECC();
    if (st == 0 || st == 18) {
        ov00_021FACF0();
        return 1;
    }
    ov00_021FBBF0(&st);
    ov00_021FAE6C(st);
    return 0;
}

void ov00_021FAB04(const u8 *ssid, const void *key, int type, const void *name, int x) {
    Ctx *ctx = ov00_021FAE14(1);
    if (name != NULL) {
        MI_CpuCopy8(name, ctx->ssid, 10);
    } else {
        MI_CpuFill8(ctx->ssid, 0, 10);
    }
    ctx->unk22 = x;
    ov00_021FAB68(ssid, key, type);
}

void ov00_021FAB68(const u8 *ssid, const void *key, int type) {
    ApConnectWork *w = ov00_021FAE14(0x10);
    int i;
    int len;
    w->profileSel = 1;
    MI_CpuFill8(w, 0, 0xf0);
    for (i = 0; i < 0x20; i++) {
        u8 c = ssid[i];
        if (c == 0) {
            break;
        }
        w->profiles[0].ssid[i] = c;
    }
    if (key == NULL || type == 0) {
        w->profiles[0].wepKeyType = 0;
        return;
    }
    if (type == 1) {
        len = 5;
    } else if (type == 2) {
        len = 13;
    } else {
        len = 16;
    }
    MI_CpuCopy8(key, w->profiles[0].wepKey, len);
    w->profiles[0].wepKeyType = (u8)type;
}

void ov00_021FAC20(int idx, const void *src) {
    ApConnectWork *w = ov00_021FAE14(0x10);
    MIi_CpuCopy32((const u32 *)src, (u32 *)&w->profiles[idx], 0xf0);
}

void *ov00_021FAC4C(int kind, int size) {
    Ctx *ctx = ov00_021FAE14(1);
    if (ctx->flags & kind) {
        return NULL;
    }
    ctx->flags |= kind;
    return ctx->alloc(kind, size);
}

void ov00_021FAC8C(int kind, void *p, int size) {
    Ctx *ctx = ov00_021FAE14(1);
    if (ctx == NULL) {
        return;
    }
    if (!(ctx->flags & kind)) {
        return;
    }
    ctx->flags &= ~kind;
    ctx->free(kind, p, size);
    if (kind == 1) {
        ov00_0221B0AC.ctx = NULL;
    }
}

void ov00_021FACF0(void) {
    Ctx *ctx = ov00_021FAE14(1);
    if (ctx == NULL) {
        return;
    }
    if (ctx->flags & 0x10) {
        void *p = ov00_021FAE14(0x10);
        ctx->flags &= ~0x10;
        ctx->free(0x10, p, sizeof(ApConnectWork));
    }
    if (ctx->flags & 8) {
        void *p = ov00_021FAE14(8);
        ctx->flags &= ~8;
        ctx->free(8, p, 0xc);
    }
    if (ctx->flags & 4) {
        void *p = ov00_021FAE14(4);
        ctx->flags &= ~4;
        ctx->free(4, p, 0x58);
    }
    if (ctx->flags & 2) {
        void *p = ov00_021FAE14(2);
        ctx->flags &= ~2;
        ctx->free(2, p, 0x2300);
    }
    if (ctx->flags & 1) {
        ctx->flags &= ~1;
        ctx->free(1, ctx, sizeof(Ctx));
        ov00_0221B0AC.ctx = NULL;
    }
}

void *ov00_021FAE14(int kind) {
    if (kind & 1) {
        return ov00_0221B0AC.ctx;
    }
    if (kind & 2) {
        return ov00_0221B0AC.p0;
    }
    if (kind & 4) {
        return ov00_0221B0AC.p4;
    }
    if (kind & 8) {
        return ov00_0221B0AC.p8;
    }
    if (kind & 0x10) {
        return ov00_0221B0AC.work;
    }
    return NULL;
}

void ov00_021FAE6C(u8 st) {
    Ctx *ctx = ov00_021FAE14(1);
    ApConnectWork *w = ov00_021FAE14(0x10);
    ctx->state = st;
    if (st >= 16) {
        return;
    }
    if (st > ctx->maxState) {
        ctx->maxState = st;
        if (st > 7) {
            ctx->unk15 = ov00_021FAFC8(w->profileIdx);
            ctx->unk14 = w->ap[w->selAp].status;
        }
    }
}

u8 ov00_021FAECC(void) {
    Ctx *ctx = ov00_0221B0AC.ctx;
    if (ctx != NULL) {
        return ctx->state;
    }
    return 0;
}

void ov00_021FAEE8(int v) {
    Ctx *ctx = ov00_021FAE14(1);
    ctx->unkC = v;
    ctx->prevState = ov00_021FAECC();
}

int ov00_021FAF0C(void) {
    return ov00_0221B0AC.ctx->unkC;
}

void ov00_021FAF20(u8 x) {
    u8 v = ov00_0221B0AC.ctx->unk22;
    void *p;
    int i;
    if (v == 0) {
        v = ov00_021FAFC8(x);
    }
    ov00_0221B0AC.ctx->unk17 = v;
    p = WCM_GetApEssid(0);
    if (p != NULL) {
        DC_InvalidateRange(p, 0x20);
        ov00_021FD17C(p, ov00_0221B0AC.ctx->ssid);
    }
    for (i = 0; i < 10; i++) {
        if (ov00_0221B0AC.ctx->ssid[i] < 0x20 || ov00_0221B0AC.ctx->ssid[i] > 0x7e) {
            MI_CpuFill8(ov00_0221B0AC.ctx->ssid, 0, 10);
            return;
        }
    }
}

u8 ov00_021FAFC8(u8 x) {
    if (x > 2) {
        x -= 3;
    }
    return x;
}

void ov00_021FAFD8(void) {
    ov00_021FAC8C(8, ov00_0221B0AC.p8, 0xc);
    ov00_021FAC8C(0x10, ov00_0221B0AC.work, sizeof(ApConnectWork));
}

int ov00_021FB010(void) {
    u8 i;
    u8 cur;
    ApConnectWork *w;
    u8 n;
    w = ov00_0221B0AC.work;
    cur = w->selAp;
    u8 st = w->profileIdx;
    char *cb = (char *)w->bss[cur].bssid;
    if (st >= 6) {
        return 1;
    }
    n = w->numAps;
    for (i = 0; i < n; i++) {
        if (i != cur && w->ap[i].index < 6) {
            if (strncmp(cb, (char *)w->bss[i].bssid, w->bss[i].ssidLength) == 0) {
                return 2;
            }
        }
    }
    return 1;
}

void ov00_021FB0B0(ApBss *bss) {
    int idx = -1;
    u8 y;
    ApConnectWork *w = ov00_021FAE14(0x10);
    ((Ctx *)ov00_021FAE14(1))->unkB = 1;
    switch (ov00_021FAECC()) {
    case 3:
        y = w->channel;
        if (bss->ssidLength == 0 || bss->ssid[0] == 0) {
            ov00_021FC4CC(bss->channel);
        } else if (bss->ssidLength == 1 && bss->ssid[0] == 0x20) {
            ov00_021FC4CC(bss->channel);
            idx = ov00_021FB2B0(bss, w->numEntries, w->entries);
        } else {
            idx = ov00_021FB2B0(bss, w->numEntries, w->entries);
        }
        break;
    case 4:
        y = w->bss[w->cur].channel - 1;
        idx = ov00_021FB32C(bss, w);
        if (idx >= 0) {
            w->ap[w->cur].done = 1;
        }
        break;
    case 5:
        y = w->channel;
        idx = ov00_021FB2B0(bss, 1, &w->entries[w->cur]);
        if (idx >= 0) {
            w->entries[w->cur].flags = 1;
        }
        break;
    default:
        return;
    }
    if (idx < 0) {
        return;
    }
    ov00_021FB53C(ov00_021FB3BC(idx, bss, y, w), w);
}

int ov00_021FB21C(ApBss *ap) {
    ApConnectWork *w = ov00_021FAE14(0x10);
    if ((w->profileSel == 0 || w->profileSel == 4) && ((ap->capability >> 4) & 1) == 1 && ov00_021FD1C8(ap->ssid) == 1) {
        return 6;
    }
    if ((w->profileSel == 0 || w->profileSel == 5) && ((ap->capability >> 4) & 1) == 1 && ov00_021FD11C(ap->ssid) == 1) {
        return 7;
    }
    return -1;
}

int ov00_021FB2B0(ApBss *ap, int n, ApSsidEntry *s) {
    int i;
    u16 len;
    if (ap->ssidLength == 0x20) {
        int r = ov00_021FB21C(ap);
        if (r > 0) {
            return r;
        }
    }
    i = 0;
    if (i < n) {
        len = ap->ssidLength;
        do {
            if ((u8)len == s->len && strncmp((char *)ap->ssid, s->ssid, len) == 0) {
                return s->index;
            }
            i++;
            s++;
        } while (i < n);
    }
    return -1;
}

int ov00_021FB32C(ApBss *ap, ApConnectWork *w) {
    int i;
    u8 n;
    u16 len;
    if (ap->ssidLength == 0x20) {
        int r = ov00_021FB21C(ap);
        if (r > 0) {
            return r;
        }
    }
    n = w->numAps;
    i = 0;
    if (i < n) {
        len = ap->ssidLength;
        do {
            if (len == w->bss[i].ssidLength && strncmp((char *)ap->ssid, (char *)w->bss[i].ssid, len) == 0) {
                return w->ap[i].index;
            }
            i++;
        } while (i < n);
    }
    return -1;
}

int ov00_021FB3BC(int x, ApBss *bss, u8 y, ApConnectWork *w) {
    u8 n;
    int idx;
    int i;
    n = w->numAps;
    idx = -1;
    for (i = 0; i < n; i++) {
        u8 *b = w->bss[i].bssid;
        if (bss->bssid[0] == b[0] && bss->bssid[1] == b[1] && bss->bssid[2] == b[2] && bss->bssid[3] == b[3] && bss->bssid[4] == b[4] && bss->bssid[5] == b[5]) {
            idx = i;
            break;
        }
    }
    if (idx == -1) {
        ov00_021FB484((u8)x, bss, y, w);
        idx = 10;
        if (w->numAps < 10) {
            w->numAps++;
        }
    } else {
        ov00_021FB4D0(idx, bss, y, w);
    }
    return idx;
}

void ov00_021FB484(u8 x, ApBss *bss, u8 y, ApConnectWork *w) {
    ApStatus *st = &w->ap[10];
    st->index = x;
    st->rssi = RSSI_CALC(bss->rssi);
    st->channel = y;
    MIi_CpuCopy32((const u32 *)bss, (u32 *)&w->bss[10], sizeof(ApBss));
}

void ov00_021FB4D0(int idx, ApBss *bss, u8 y, ApConnectWork *w) {
    ApStatus *stats = w->ap;
    u8 *aps = (u8 *)w->bss;
    ApStatus *st = &stats[idx];
    int off = idx * sizeof(ApBss);
    int r = RSSI_CALC(bss->rssi);
    if ((u8)r > st->rssi) {
        st->rssi = r;
        st->channel = y;
    }
    MIi_CpuCopy32((const u32 *)bss, (u32 *)(aps + off), sizeof(ApBss));
}

void ov00_021FB53C(int i, ApConnectWork *w) {
    ApStatus tmp;
    ApBss tmpAp;
    int j;
    ApStatus *st = w->ap;
    ApBss *ap = w->bss;
    for (j = i - 1; j >= 0; j--) {
        if (st[i].rssi < st[j].rssi) {
            break;
        }
        MIi_CpuCopy32((const u32 *)&st[j], (u32 *)&tmp, 4);
        MIi_CpuCopy32((const u32 *)&st[i], (u32 *)&st[j], 4);
        MIi_CpuCopy32((const u32 *)&tmp, (u32 *)&st[i], 4);
        MIi_CpuCopy32((const u32 *)&ap[j], (u32 *)&tmpAp, sizeof(ApBss));
        MIi_CpuCopy32((const u32 *)&ap[i], (u32 *)&ap[j], sizeof(ApBss));
        MIi_CpuCopy32((const u32 *)&tmpAp, (u32 *)&ap[i], sizeof(ApBss));
        i = j;
    }
    MIi_CpuClear32(0, (u32 *)&st[10], 4);
    MIi_CpuClear32(0, (u32 *)&ap[10], sizeof(ApBss));
}

void ov00_021FB62C(WcmEvent *ev) {
    ApConnectWork *w = ov00_021FAE14(0x10);
    if (ev->notify == 5) {
        if (ev->result == 0) {
            return;
        }
        switch (ev->reason) {
        case 0xd:
            w->unk_D14 = 1;
            return;
        case 0xf:
            w->unk_D14 = 2;
            return;
        case 0x11:
            w->unk_D14 = 3;
            return;
        default:
            w->unk_D14 = 4;
            return;
        }
    } else if (ev->notify == 7) {
        ov00_021FB0B0(ev->ptr);
    }
}

u8 ov00_021FB6B8(void) {
    u8 st = ov00_021FAECC();
    ApConnectWork *w = ov00_021FAE14(0x10);
    switch (st) {
    case 7:
        st = ov00_021FB6FC(w);
        break;
    case 8:
        st = ov00_021FB7F4(w);
        break;
    }
    return st;
}

u8 ov00_021FB6FC(ApConnectWork *w) {
    ApBss *ap = &w->bss[w->selAp];
    w->profileIdx = ov00_021FB930(w);
    MI_CpuFill8(w->connectConfig, 0, 0x52);
    if (ov00_021FBB04(w, w->profileIdx, w->connectConfig)) {
        w->unk_D0B_2 = 1;
        if (!((ap->capability >> 4) & 1)) {
            w->ap[w->selAp].status = 3;
            return 9;
        }
        if (w->profileIdx == 6 && ((u8 *)ap)[0x15] == 0) {
            w->ap[w->selAp].status = 3;
            return 9;
        }
    } else {
        w->unk_D0B_2 = 0;
        if (((ap->capability >> 4) & 1) == 1) {
            w->ap[w->selAp].status = 3;
            return 9;
        }
    }
    w->chIter = 0;
    w->unk_D14 = 0;
    return 8;
}

u8 ov00_021FB7F4(ApConnectWork *w) {
    int ph = WCM_GetPhase();
    int off;
    u8 *aps;
    aps = (u8 *)w->bss;
    off = w->selAp * sizeof(ApBss);
    if (ph == 3) {
        u32 a = ov00_021FBACC(w);
        u8 c = ++w->chIter;
        if (c > 3) {
            w->chIter = 0;
            w->ap[w->selAp].status = 1;
            return 9;
        }
        if (c != 1) {
            if (w->unk_D14 == 1) {
                w->unk_D0B_2 = 0;
            } else if (w->unk_D14 == 2) {
                w->chIter = 0;
                w->ap[w->selAp].status = 3;
                return 9;
            } else if (w->unk_D14 == 3) {
                w->chIter = 0;
                w->ap[w->selAp].status = 4;
                return 9;
            } else if (c == 3) {
                w->unk_D0B_2 = 0;
            }
        }
        WCM_ConnectAsync(aps + off, w->connectConfig, a | ov00_021FBAE8(w));
    } else if (ph == 9) {
        w->chIter = 0;
        w->tick = OS_GetTick();
        return 10;
    }
    return 8;
}

u8 ov00_021FB930(ApConnectWork *w) {
    ApBss *ap = &w->bss[w->selAp];
    int res = 0;
    int i;
    int cnt;
    if (w->mode == 0) {
        cnt = res;
        if (ap->ssidLength == 0x20) {
            res = ov00_021FB21C(ap);
            if (res > 0) {
                cnt++;
            } else {
                res = 0;
            }
        } else if (ap->ssidLength == 8) {
            res = ov00_021FBFB8(ap);
            if (res != 0) {
                cnt++;
            } else {
                res = 0;
            }
        }
        for (i = 0; i < w->numEntries; i++) {
            if (ap->ssidLength == w->entries[i].len && strncmp((char *)ap->ssid, w->entries[i].ssid, ap->ssidLength) == 0) {
                if (cnt == 0) {
                    res = w->entries[i].index;
                } else {
                    w->entries[i].flagsHi = 1;
                    w->mode = 1;
                }
                cnt++;
            }
        }
    } else {
        i = res;
        cnt = res;
        for (; i < w->numEntries; i++) {
            if (w->entries[i].flagsHi == 1) {
                if (cnt == 0) {
                    w->entries[i].flagsHi = 0;
                    res = w->entries[i].index;
                }
                cnt++;
            }
        }
        if (cnt == 1) {
            w->mode = 0;
        }
    }
    return res;
}

u32 ov00_021FBACC(ApConnectWork *w) {
    return w->unk_D0B_0 == 1 ? 0x30000 : 0x20000;
}

u32 ov00_021FBAE8(ApConnectWork *w) {
    return w->unk_D0B_2 == 1 ? 0xc0000 : 0x80000;
}

BOOL ov00_021FBB04(ApConnectWork *w, int kind, u8 *out) {
    u8 *p = (u8 *)w;
    switch (kind) {
    case 2:
        p += 0x100;
    case 1:
        p += 0x100;
    case 0:
        out[0] = ((ApProfile *)p)->wepKeyType;
        MI_CpuCopy8(((ApProfile *)p)->wepKey, out + 2, 16);
        break;
    case 5:
        p += 0x100;
    case 4:
        p += 0x100;
    case 3:
        out[0] = 1;
        MI_CpuCopy8(((ApProfile *)p)->wepKey + 0x51, out + 2, 5);
        break;
    case 6:
        out[0] = 2;
        ov00_021FD1EC(w->bss[w->selAp].ssid, out + 2);
        break;
    case 7:
        out[0] = 2;
        ov00_021FD154(w->bss[w->selAp].ssid, out + 2);
        break;
    case 8:
    case 9:
        break;
    }
    return out[0] != 0 ? TRUE : FALSE;
}

int ov00_021FBBF0(u8 *st) {
    if (*st <= 10) {
        int r = ov00_021FBC78();
        if (r == 1) {
            *st = 0;
            return 1;
        }
        if (r == -1) {
            *st = 18;
            return 1;
        }
    } else if (*st == 14) {
        ov00_021EA518();
        ov00_021EA420();
        *st = 12;
    } else if (*st < 18) {
        if (ov00_021FBD08() == 1) {
            *st = 10;
        }
    }
    return 0;
}

int ov00_021FBC78(void) {
    switch (WCM_GetPhase()) {
    case 0:
        return 1;
    case 1:
        WCM_Finish();
        break;
    case 3:
        WCM_CleanupAsync();
        break;
    case 6:
        WCM_EndSearchAsync();
        break;
    case 9:
        WCM_DisconnectAsync();
        break;
    case 12:
        WCM_TerminateAsync();
        break;
    case 11:
        ov00_021FAEE8(0);
        return -1;
    }
    return 0;
}

int ov00_021FBD08(void) {
    int r;
    if (SOCL_CalmDown()) {
        return 0;
    }
    r = SOC_Cleanup();
    return (r == 0 || r == -39) ? 1 : 0;
}

u8 ov00_021FBD38(void) {
    Ctx *ctx = ov00_021FAE14(1);
    if (ov00_021FBBF0(&ctx->prevState) == 1) {
        return 18;
    }
    return 17;
}

int ov00_021FBD5C(void) {
    Ctx *ctx = ov00_021FAE14(1);
    int v = ov00_021FAF0C();
    if (v < 4) {
        return ov00_021FBDAC(v);
    }
    if (v < 5) {
        return ov00_021FBDF0();
    }
    if (v == 5) {
        return ov00_021FBDF8(ctx);
    }
    return ov00_021FBE14(ctx);
}

int ov00_021FBDAC(int v) {
    switch (v) {
    case 1:
        return -9;
    case 0:
        return -10;
    case 2:
        return -8;
    case 3:
        return -7;
    }
    return 0;
}

int ov00_021FBDF0(void) {
    return -6;
}

int ov00_021FBDF8(Ctx *c) {
    return c->unkB == 0 ? -50099 : -51099;
}

static inline int c800(void) {
    return 51200;
}

static inline int defaultError(int reason) {
    return -c800() - reason;
}

#pragma opt_rebuildconditionals off

int ov00_021FBE14(Ctx *p) {
    int reason = p->unk22;
    unsigned int phase = p->maxState;
    if (reason == 0) {
        reason = p->unk15;
    }
    if (phase < 10) {
        if (p->unk14 == 3) {
            return -51100 - reason;
        }
        if (p->unk14 != 4) {
            return -51300 - reason;
        }
        return defaultError(reason);
    }
    if (phase < 13) {
        return -52000 - reason;
    }
    if (p->status == 0) {
        return -50000 - reason;
    }
    if (p->status == -1) {
        return -52100 - reason;
    }
    if (p->status == -2) {
        return -52200 - reason;
    }
    if (p->status == -3) {
        return -52300 - reason;
    }
    if (p->status == -4) {
        return -53000 - reason;
    }
    if (p->status == -5) {
        return -53100 - reason;
    }
    if (p->status == -6) {
        return -53200 - reason;
    }
    return p->status;
}

#pragma opt_rebuildconditionals reset

u8 ov00_021FBF24(int kind) {
    ApConnectWork *w = ov00_021FAE14(0x10);
    switch (kind) {
    case 0:
        MIi_CpuClear32(0, (u32 *)w->entries, sizeof(w->entries));
        w->numEntries = ov00_021FC004(w);
        break;
    case 1:
        w->numEntries = ov00_021FC258(w);
        w->cur = ov00_021FC2E8(w);
        break;
    case 2:
        MIi_CpuClear32(0, (u32 *)w->entries, sizeof(w->entries));
        w->cur = 0;
        w->numEntries = ov00_021FC064(w);
        break;
    }
    return w->numEntries;
}

int ov00_021FBFB8(ApBss *ap) {
    ApConnectWork *w = ov00_021FAE14(0x10);
    if ((w->profileSel == 0 || w->profileSel == 6) && strncmp((char *)ap->ssid, ov00_02216060, 8) == 0) {
        return 8;
    }
    return 0;
}

u8 ov00_021FC004(ApConnectWork *w) {
    ApSsidEntry *s = w->entries;
    u8 n = ov00_021FC150(w);
    s += n;
    if (w->profileSel == 0 || w->profileSel == 6) {
        MI_CpuCopy8(ov00_02216060, s->ssid, 8);
        s->len = 8;
        s->index = 8;
        n++;
    }
    return n;
}

u8 ov00_021FC064(ApConnectWork *w) {
    u8 n;
    ApSsidEntry *s = w->entries;
    n = ov00_021FC150(w);
    s += n;
    if (w->profileSel == 0 || w->profileSel == 4) {
        MI_CpuCopy8(ov00_02217B8C, s->ssid, 8);
        s->len = 8;
        s->index = 6;
        n++;
        s++;
    }
    if (w->profileSel == 0 || w->profileSel == 7) {
        MI_CpuCopy8(ov00_02216058, s->ssid, 8);
        s->len = 8;
        s->index = 9;
        n++;
        s++;
    }
    if (w->profileSel == 0 || w->profileSel == 8) {
        MI_CpuCopy8(ov00_02216068, s->ssid, 11);
        s->len = 11;
        s->index = 10;
        n++;
    }
    return n;
}
