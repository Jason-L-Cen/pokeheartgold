// SPDX-License-Identifier: GPL-3.0-or-later
#include "../NativeFloatRuntime.h"
#include "hg_float_checks.h"
#ifdef HEARTGOLD_ABI32
extern int errno;
#else
#include <errno.h>
#endif
#define CHECK(v) do { if (!(v)) return __LINE__; } while (0)
static u32 bits32(float value) { union { float value; u32 bits; } v={.value=value}; return v.bits; }
static float value32(u32 bits) { union { float value; u32 bits; } v={.bits=bits}; return v.value; }
u32 CheckOriginalFloatRuntime(void) {
    for (u32 i=0;i<sizeof(floatCases)/sizeof(floatCases[0]);i++) {
        CHECK(_fadd(floatCases[i].a,floatCases[i].b)==floatCases[i].add);
        CHECK(bits32(_fdiv(value32(floatCases[i].a),value32(floatCases[i].b)))==floatCases[i].divide);
        CHECK((u32)_ffix(floatCases[i].a)==floatCases[i].fix);
    }
    for (u32 i=0;i<sizeof(integerFloatCases)/sizeof(integerFloatCases[0]);i++) {
        CHECK(bits32(_f_lltof((s64)integerFloatCases[i].value))==integerFloatCases[i].signed64);
        CHECK(bits32(_f_ulltof(integerFloatCases[i].value))==integerFloatCases[i].unsigned64);
        CHECK(_fflt((s32)integerFloatCases[i].value)==integerFloatCases[i].signed32);
    }
    for (u32 i=0;i<sizeof(squareCases)/sizeof(squareCases[0]);i++) {
        union { double value; u64 bits; } input={.bits=squareCases[i].value},result;
        errno=123; result.value=_dsqrt(input.value);
        CHECK(result.bits==squareCases[i].result && errno==squareCases[i].error);
    }
    return 0;
}
