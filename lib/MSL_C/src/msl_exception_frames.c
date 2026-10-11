#include <nitro/types.h>

// The code below is compiled as ARM (32-bit) code.
#include <nitro/code32.h>

#pragma force_active on

// Exception handling runtime: catch type matching on the mangled type strings, popping a stack frame,
// frame information, the exception table bounds and skipping unwind information. (Written as C with the
// mangled names; the context / info views have only the fields these functions use.)

// ThrowContext as seen by __PopStackFrame.
typedef struct PopFrameContext {
    u8 unk_00[0x14];
    u8 *stack;    // 0x14
    u8 *frame;    // 0x18
    u32 regs[16]; // 0x1C
    u8 unk_5C[4]; // 0x5C
    u32 size;     // 0x60
    u8 unk_64[4]; // 0x64
    u16 mask;     // 0x68: saved registers
    u8 fp;        // 0x6A
} PopFrameContext;

// ThrowContext as seen by __SetupFrameInfo.
typedef struct FrameInfoContext {
    u8 unk_00[0x14];
    u32 current;     // 0x14
    u32 frame;       // 0x18
    u8 unk_1C[0x1C]; // 0x1C
    u32 stacked;     // 0x38
    u8 unk_3C[0xC];  // 0x3C
    u32 unstacked;   // 0x48
    u8 unk_4C[0x14]; // 0x4C
    u32 decoded;     // 0x60
    u32 extra;       // 0x64
    u16 size;        // 0x68
    u8 flag20;       // 0x6A
    u8 flag40;       // 0x6B
    u8 flag80;       // 0x6C
} FrameInfoContext;

// ExceptionInfo as seen by __SetupFrameInfo.
typedef struct FrameInfoException {
    u32 unk_00;
    u8 *info; // 0x04
} FrameInfoException;

// ExceptionInfo as seen by __FindExceptionTable.
typedef struct ExceptionTableInfo {
    u32 unk_00[3];
    const u8 *start; // 0x0C
    const u8 *end;   // 0x10
} ExceptionTableInfo;

extern const u8 __exception_table_start__[];
extern const u8 __exception_table_end__[];

char *_Z22__DecodeUnsignedNumberPcPm(char *p, u32 *out);

int __throw_catch_compare(const unsigned char *thrown, const unsigned char *caught, int *adjust);
u32 _Z15__PopStackFrameP12ThrowContextP13ExceptionInfo(PopFrameContext *ctx, void *unused);
void _Z16__SetupFrameInfoP12ThrowContextP13ExceptionInfo(FrameInfoContext *c, FrameInfoException *e);
BOOL _Z20__FindExceptionTableP13ExceptionInfoPc(ExceptionTableInfo *info, char *position);
char *_Z16__SkipUnwindInfoPc(char *p);

int __throw_catch_compare(const unsigned char *thrown, const unsigned char *caught, int *adjust) {
    const unsigned char *cursor = caught;
    int n;
    *adjust = 0;
    if (caught == 0) {
        return 1;
    }
    if (*caught == 'P') {
        cursor++;
        if (*cursor == 'V') {
            cursor++;
        }
        if (*cursor == 'K') {
            cursor++;
        }
        if (*cursor == 'v' && (*thrown == 'P' || *thrown == '*')) {
            return 1;
        }
        cursor = caught;
    }
    if (*thrown == '!' || *thrown == '*') {
        if (*thrown++ != *cursor++) {
            return 0;
        }
        for (;;) {
            if (*thrown == *cursor++) {
                if (*thrown++ == '!') {
                    n = 0;
                    while (*thrown != '!') {
                        n = n * 10 + *thrown++ - '0';
                    }
                    *adjust = n;
                    return 1;
                }
            } else {
                while (*thrown++ != '!') {}
                while (*thrown++ != '!') {}
                if (*thrown == 0) {
                    return 0;
                }
                cursor = caught + 1;
            }
        }
    }
    while ((*thrown == 'P' || *thrown == 'R') && *thrown == *cursor) {
        thrown++;
        cursor++;
        if (*cursor == 'K') {
            if (*thrown == 'K') {
                thrown++;
            }
            cursor++;
        }
        if (*thrown == 'K') {
            return 0;
        }
        if (*cursor == 'V') {
            if (*thrown == 'V') {
                thrown++;
            }
            cursor++;
        }
        if (*(const volatile unsigned char *)thrown == 'V') {
            return 0;
        }
    }
    while (*thrown == *cursor) {
        if (!*thrown) {
            return 1;
        }
        thrown++;
        cursor++;
    }
    return 0;
}

u32 _Z15__PopStackFrameP12ThrowContextP13ExceptionInfo(PopFrameContext *ctx, void *unused) {
    u32 size = ctx->size;
    u32 *cursor = (u32 *)(ctx->frame + size - (ctx->fp ? 16 : 0));
    int i;
    for (i = 15; i >= 0; i--) {
        if (ctx->mask & (1u << i)) {
            ctx->regs[i] = *--cursor;
        }
    }
    ctx->stack = ctx->frame + size;
    return ctx->regs[14];
}

void _Z16__SetupFrameInfoP12ThrowContextP13ExceptionInfo(FrameInfoContext *c, FrameInfoException *e) {
    u8 *src = e->info;
    u32 flags = src[0], forty = flags & 64, eighty;
    c->flag40 = forty != 0;
    c->flag20 = (flags & 32) != 0;
    eighty = flags & 128;
    c->flag80 = eighty != 0;
    c->size = src[1] << 4;
    c->size |= 0x4000;
    src = (u8 *)_Z22__DecodeUnsignedNumberPcPm((char *)(src + 2), &c->decoded);
    if (forty) {
        src = (u8 *)_Z22__DecodeUnsignedNumberPcPm((char *)src, &c->extra);
    }
    if (forty) {
        if (eighty) {
            c->frame = c->stacked;
        } else {
            c->frame = c->unstacked;
        }
    } else {
        c->frame = c->current;
    }
}

BOOL _Z20__FindExceptionTableP13ExceptionInfoPc(ExceptionTableInfo *info, char *position) {
    info->start = __exception_table_start__;
    info->end = __exception_table_end__;
    return TRUE;
}

char *_Z16__SkipUnwindInfoPc(char *p) {
    u32 value;
    u8 flag = *p;
    p = _Z22__DecodeUnsignedNumberPcPm(p + 2, &value);
    if (flag & 0x40) {
        p = _Z22__DecodeUnsignedNumberPcPm(p, &value);
    }
    return p;
}
