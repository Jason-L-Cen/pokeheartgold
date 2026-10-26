// SPDX-License-Identifier: GPL-3.0-or-later
#include "../NativeIntegerRuntime.h"
#define CHECK(v) do { if (!(v)) return __LINE__; } while (0)

// Python arbitrary-precision goldens, truncated to ARM's word width after each result.
static const struct { u64 a,b,mul,sdiv,smod,udiv,umod; } cases64[]={
    {0,0,0,0,0,0,0},
    {1,0,0,1,1,1,1},
    {0xFFFFFFFFFFFFFFFFULL,0,0,0xFFFFFFFFFFFFFFFFULL,0xFFFFFFFFFFFFFFFFULL,0xFFFFFFFFFFFFFFFFULL,0xFFFFFFFFFFFFFFFFULL},
    {0x8000000000000000ULL,0,0,0x8000000000000000ULL,0x8000000000000000ULL,0x8000000000000000ULL,0x8000000000000000ULL},
    {0,1,0,0,0,0,0},
    {1,1,1,1,0,1,0},
    {0xFFFFFFFFFFFFFFFFULL,1,0xFFFFFFFFFFFFFFFFULL,0xFFFFFFFFFFFFFFFFULL,0,0xFFFFFFFFFFFFFFFFULL,0},
    {0x8000000000000000ULL,0xFFFFFFFFFFFFFFFFULL,0x8000000000000000ULL,0x8000000000000000ULL,0,0,0x8000000000000000ULL},
    {0x8000000000000000ULL,1,0x8000000000000000ULL,0x8000000000000000ULL,0,0x8000000000000000ULL,0},
    {0x8000000000000000ULL,2,0,0xC000000000000000ULL,0,0x4000000000000000ULL,0},
    {0x7FFFFFFFFFFFFFFFULL,3,0x7FFFFFFFFFFFFFFDULL,0x2AAAAAAAAAAAAAAAULL,1,0x2AAAAAAAAAAAAAAAULL,1},
    {0xFFFFFFFFFFFFFFFFULL,2,0xFFFFFFFFFFFFFFFEULL,0,0xFFFFFFFFFFFFFFFFULL,0x7FFFFFFFFFFFFFFFULL,1},
    {0xFEDCBA9876543210ULL,0x12345678,0x3D70A3D70B88D780ULL,0xFFFFFFFFF0000000ULL,0xFFFFFFFFF6543210ULL,0xE00000077ULL,0x48},
    {0x123456789ABCDEF0ULL,0x100000001ULL,0xACF135689ABCDEF0ULL,0x12345678,0x88888878,0x12345678,0x88888878},
    {0x8000000000000001ULL,0x8000000000000000ULL,0x8000000000000000ULL,0,0x8000000000000001ULL,1,1},
    {0xFFFFFFFFFFFFFFFFULL,0x8000000000000000ULL,0x8000000000000000ULL,0,0xFFFFFFFFFFFFFFFFULL,1,0x7FFFFFFFFFFFFFFFULL},
    {0x7FFFFFFFFFFFFFFFULL,0xFFFFFFFFFFFFFFFFULL,0x8000000000000001ULL,0x8000000000000001ULL,0,0,0x7FFFFFFFFFFFFFFFULL},
};
static const struct { u32 a,b,sdiv,udiv,unchecked; } cases32[]={
    {0,0,0,0,0}, {1,0,1,1,0}, {0xFFFFFFFF,0,0xFFFFFFFF,0xFFFFFFFF,0},
    {0x80000000,0,0x80000000,0x80000000,0}, {0x80000000,0xFFFFFFFF,0x80000000,0,0},
    {0x80000000,1,0x80000000,0x80000000,0x80000000}, {0xFFFFFFFF,2,0,0x7FFFFFFF,0x7FFFFFFF},
    {0x7FFFFFFF,3,0x2AAAAAAA,0x2AAAAAAA,0x2AAAAAAA}, {0xFFFFFFFF,0x80000000,0,1,1},
    {0x87654321,12345,0xFFFD7FC0,0x2CEC6,0x2CEC6},
};
static const struct { u32 count; u64 result; } shifts[]={
    {0,0xFEDCBA9876543210ULL}, {1,0xFDB97530ECA86420ULL}, {31,0x3B2A190800000000ULL},
    {32,0x7654321000000000ULL}, {63,0}, {64,0xFEDCBA9876543210ULL},
    {65,0xFDB97530ECA86420ULL}, {127,0}, {128,0xFEDCBA9876543210ULL}, {255,0}, {0xFFFFFFFF,0},
};
u32 CheckOriginalIntegerRuntime(void) {
    for (u32 i=0;i<sizeof(cases64)/sizeof(cases64[0]);i++) {
        u64 a=cases64[i].a,b=cases64[i].b;
        CHECK((u64)_ll_mul((s64)a,(s64)b)==cases64[i].mul && _ull_mul(a,b)==cases64[i].mul);
        CHECK((u64)_ll_sdiv((s64)a,(s64)b)==cases64[i].sdiv && (u64)_ll_div((s64)a,(s64)b)==cases64[i].sdiv);
        CHECK((u64)_ll_mod((s64)a,(s64)b)==cases64[i].smod);
        CHECK(_ll_udiv(a,b)==cases64[i].udiv && _ull_div(a,b)==cases64[i].udiv);
        CHECK(_ull_mod(a,b)==cases64[i].umod);
#ifdef HEARTGOLD_ABI32
        volatile s64 signedA=(s64)a,signedB=(s64)b;
        volatile u64 unsignedA=a,unsignedB=b;
        CHECK((u64)(signedA/signedB)==cases64[i].sdiv);
        CHECK(unsignedA/unsignedB==cases64[i].udiv);
#endif
    }
    for (u32 i=0;i<sizeof(cases32)/sizeof(cases32[0]);i++) {
        CHECK((u32)_s32_div_f((s32)cases32[i].a,(s32)cases32[i].b)==cases32[i].sdiv);
        CHECK(_u32_div_f(cases32[i].a,cases32[i].b)==cases32[i].udiv);
        CHECK(_u32_div_not_0_f(cases32[i].a,cases32[i].b)==cases32[i].unchecked);
#ifdef HEARTGOLD_ABI32
        volatile s32 signedA=(s32)cases32[i].a,signedB=(s32)cases32[i].b;
        volatile u32 unsignedA=cases32[i].a,unsignedB=cases32[i].b;
        CHECK((u32)(signedA/signedB)==cases32[i].sdiv);
        CHECK(unsignedA/unsignedB==cases32[i].udiv);
#endif
    }
    for (u32 i=0;i<sizeof(shifts)/sizeof(shifts[0]);i++) {
        CHECK((u64)_ll_shl((s64)0xFEDCBA9876543210ULL,shifts[i].count)==shifts[i].result);
        CHECK((u64)_ll_sll((s64)0xFEDCBA9876543210ULL,shifts[i].count)==shifts[i].result);
        CHECK(_ull_sll(0xFEDCBA9876543210ULL,shifts[i].count)==shifts[i].result);
    }
    return 0;
}
