// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef HEARTGOLD_NATIVE_INTEGER_RUNTIME_H
#define HEARTGOLD_NATIVE_INTEGER_RUNTIME_H
#include <nitro/types.h>
#ifdef __cplusplus
extern "C" {
#endif
s64 _ll_sdiv(s64 dividend,s64 divisor);
s64 _ll_div(s64 dividend,s64 divisor);
s64 _ll_mod(s64 dividend,s64 divisor);
u64 _ll_udiv(u64 dividend,u64 divisor);
u64 _ull_div(u64 dividend,u64 divisor);
u64 _ull_mod(u64 dividend,u64 divisor);
s64 _ll_mul(s64 a,s64 b);
u64 _ull_mul(u64 a,u64 b);
s64 _ll_shl(s64 value,u32 shift);
s64 _ll_sll(s64 value,u32 shift);
u64 _ull_sll(u64 value,u32 shift);
s32 _s32_div_f(s32 dividend,s32 divisor);
u32 _u32_div_f(u32 dividend,u32 divisor);
u32 _u32_div_not_0_f(u32 dividend,u32 divisor);
#ifdef __cplusplus
}
#endif
#endif
