// SPDX-License-Identifier: GPL-3.0-or-later
#include "cps/cps_crypto.h"
#include <stdlib.h>
#include <string.h>
#include "hg_bignum_checks.h"

#define CHECK(value) do { if (!(value)) return __LINE__; } while (0)
static u32 allocations,failAllocation;
static void *Allocate(u32 size) {
    if (failAllocation) { failAllocation--; return NULL; }
    void *data=malloc(size); if (data) allocations++; return data;
}
static void Release(void *data) {
    if (data) { if (!allocations) abort(); allocations--; free(data); }
}
// Test allocator bindings only; the original power routines own their work buffers.
void *(*gCpsAlloc)(u32)=Allocate;
void (*gCpsFree)(void *)=Release;

u32 CheckOriginalBignum(void) {
    const u16 words[]={0x0123,0x4567,0x89AB,0xCDEF},ones[]={0xFFFF,0xFFFF,0xFFFF,0xFFFF};
    CHECK(get48bits_1(words+3)==0x0000CDEF00000000ULL);
    CHECK(get48bits_2(words+3)==0x0000CDEF89AB0000ULL);
    CHECK(get48bits_3(words+3)==0x0000CDEF89AB4567ULL);
    CHECK(get64bits(words+3)==0xCDEF89AB45670123ULL);
    CHECK(get48bits_2(ones+3)==0x0000FFFFFFFF0000ULL && get48bits_3(ones+3)==0x0000FFFFFFFFFFFFULL && get64bits(ones+3)==~(u64)0);
    u16 guarded[HG_BIG_MAX_DIGITS+2],copy[HG_BIG_MAX_DIGITS],quotient[HG_BIG_MAX_DIGITS],remainder[HG_BIG_MAX_DIGITS];
    u16 scratchGuard[9*HG_BIG_MAX_DIGITS+2]; u16 *scratch=scratchGuard+1,*out=guarded+1;
    for (u32 i=0;i<sizeof(hgBigArithmetic)/sizeof(hgBigArithmetic[0]);i++) {
        const HGBigArithmetic *v=&hgBigArithmetic[i]; int n=v->digits; u32 bytes=n*2;
        memset(guarded,0xA5,sizeof(guarded));
        CPSi_big_add(out,v->a,v->b,n); CHECK(!memcmp(out,v->added,bytes));
        memcpy(copy,v->a,bytes); CPSi_big_add(copy,copy,v->b,n); CHECK(!memcmp(copy,v->added,bytes));
        memcpy(copy,v->b,bytes); CPSi_big_add(copy,v->a,copy,n); CHECK(!memcmp(copy,v->added,bytes));
        CPSi_big_sub(out,v->a,v->b,n); CHECK(!memcmp(out,v->subtracted,bytes));
        memcpy(copy,v->a,bytes); CPSi_big_sub(copy,copy,v->b,n); CHECK(!memcmp(copy,v->subtracted,bytes));
        CPSi_big_mult(out,v->a,v->b,n); CHECK(!memcmp(out,v->multiplied,bytes));
        CPSi_big_sqr(out,v->a,n); CHECK(!memcmp(out,v->squared,bytes));
        memcpy(copy,v->a,bytes); CPSi_big_negate(copy,n); CHECK(!memcmp(copy,v->negated,bytes));
        CPSi_big_mult_small(out,v->a,v->small,n); CHECK(!memcmp(out,v->scaled,bytes));
        memset(scratchGuard,0xA5,sizeof(scratchGuard));
        CPSi_big_div(quotient,v->a,v->b,remainder,n,scratch);
        CHECK(!memcmp(quotient,v->quotient,bytes) && !memcmp(remainder,v->remainder,bytes));
        memcpy(copy,v->a,bytes); CPSi_big_div(NULL,copy,v->b,copy,n,scratch); CHECK(!memcmp(copy,v->remainder,bytes));
        CHECK(scratchGuard[0]==0xA5A5 && scratchGuard[3*n+1]==0xA5A5);
        CHECK(guarded[0]==0xA5A5 && guarded[n+1]==0xA5A5);
    }
    for (u32 i=0;i<sizeof(hgBigPower)/sizeof(hgBigPower[0]);i++) {
        const HGBigPower *v=&hgBigPower[i]; int n=v->digits; u32 bytes=n*2;
        // Certificate verification uses a twice-modulus-width buffer; other SSL callers add a digit.
        for (int width=n-1;width<=n;width++) {
            memset(guarded,0xA5,sizeof(guarded));
            CPSi_big_power(out,v->base,v->exponent,width,v->modulus);
            CHECK(!memcmp(out,v->result,width*2) && !allocations);
            CHECK(guarded[0]==0xA5A5 && guarded[width+1]==0xA5A5);
        }
        memset(guarded,0xA5,sizeof(guarded));
        CPSi_big_montpower(out,v->base,v->exponent,n,v->modulus); CHECK(!memcmp(out,v->result,bytes) && !allocations);
        memset(scratchGuard,0xA5,sizeof(scratchGuard)); memset(scratch,0,9*bytes);
        CPSi_big_modinv(out,v->base,v->modulus,n,scratch);
        CHECK(!memcmp(out,v->inverse,bytes));
        CHECK(scratchGuard[0]==0xA5A5 && scratchGuard[9*n+1]==0xA5A5);
        CHECK(guarded[0]==0xA5A5 && guarded[n+1]==0xA5A5);
    }
    const HGBigPower *v=hgBigPower;
    memset(guarded,0xA5,sizeof(guarded)); failAllocation=1;
    CPSi_big_power(out,v->base,v->exponent,v->digits,v->modulus);
    for (u32 i=0;i<sizeof(guarded)/sizeof(guarded[0]);i++) CHECK(guarded[i]==0xA5A5);
    CHECK(!failAllocation && !allocations); failAllocation=1;
    CPSi_big_montpower(out,v->base,v->exponent,v->digits,v->modulus);
    for (u32 i=0;i<sizeof(guarded)/sizeof(guarded[0]);i++) CHECK(guarded[i]==0xA5A5);
    CHECK(!failAllocation && !allocations);
    return 0;
}
