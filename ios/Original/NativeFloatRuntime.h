// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef HEARTGOLD_NATIVE_FLOAT_RUNTIME_H
#define HEARTGOLD_NATIVE_FLOAT_RUNTIME_H
#include <nitro/types.h>
#ifdef __cplusplus
extern "C" {
#endif
u32 _fadd(u32 a,u32 b);
float _fdiv(float a,float b);
u32 _fflt(s32 value);
s32 _ffix(u32 value);
float _f_lltof(s64 value);
float _f_ulltof(u64 value);
double _dsqrt(double value);
#ifdef __cplusplus
}
#endif
#endif
