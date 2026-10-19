// SPDX-License-Identifier: GPL-3.0-or-later
// Typed C-call results of the integer entries in lib/MSL_C/src/msl_fp_runtime.c.
#include "NativeIntegerRuntime.h"

s64 _ll_sdiv(s64 dividend,s64 divisor) {
    // Original ARM returns the dividend on zero and wraps MIN / -1.
    if (!divisor || (dividend==(-9223372036854775807LL-1) && divisor==-1)) return dividend;
    return dividend/divisor;
}
s64 _ll_div(s64 dividend,s64 divisor) { return _ll_sdiv(dividend,divisor); }
s64 _ll_mod(s64 dividend,s64 divisor) {
    if (!divisor) return dividend;
    if (dividend==(-9223372036854775807LL-1) && divisor==-1) return 0;
    return dividend%divisor;
}
u64 _ll_udiv(u64 dividend,u64 divisor) { return divisor ? dividend/divisor : dividend; }
u64 _ull_div(u64 dividend,u64 divisor) { return _ll_udiv(dividend,divisor); }
u64 _ull_mod(u64 dividend,u64 divisor) { return divisor ? dividend%divisor : dividend; }
s64 _ll_mul(s64 a,s64 b) { return (s64)((u64)a*(u64)b); }
u64 _ull_mul(u64 a,u64 b) { return a*b; }
s64 _ll_shl(s64 value,u32 shift) { return (s64)((u64)value<<(shift&63)); }
s64 _ll_sll(s64 value,u32 shift) { return _ll_shl(value,shift); }
u64 _ull_sll(u64 value,u32 shift) { return value<<(shift&63); }
s32 _s32_div_f(s32 dividend,s32 divisor) {
    if (!divisor || (dividend==(-2147483647-1) && divisor==-1)) return dividend;
    return dividend/divisor;
}
u32 _u32_div_f(u32 dividend,u32 divisor) { return divisor ? dividend/divisor : dividend; }
u32 _u32_div_not_0_f(u32 dividend,u32 divisor) {
    // Bypassing the ARM zero-divisor guard runs all 32 steps and leaves a zero quotient.
    return divisor ? dividend/divisor : 0;
}
