// SPDX-License-Identifier: GPL-3.0-or-later
// C-call entries currently referenced from the packed original msl_fp_runtime.c assembly.
#include "NativeFloatRuntime.h"
#ifdef HEARTGOLD_ABI32
extern int errno;
#else
#include <errno.h>
#endif
static float floatValue(u32 bits) { union { u32 bits; float value; } v={.bits=bits}; return v.value; }
static u32 floatBits(float value) { union { u32 bits; float value; } v={.value=value}; return v.bits; }
static BOOL nan32(u32 value) { return (value&0x7FFFFFFF)>0x7F800000; }
u32 _fadd(u32 a,u32 b) {
    if (nan32(a) || nan32(b)) return 0x7FFFFFFF;
    u32 result=floatBits(floatValue(a)+floatValue(b));
    return nan32(result) ? 0x7FFFFFFF : result;
}
float _fdiv(float a,float b) {
    if (nan32(floatBits(a)) || nan32(floatBits(b))) return floatValue(0x7FFFFFFF);
    float result=a/b;
    return nan32(floatBits(result)) ? floatValue(0x7FFFFFFF) : result;
}
u32 _fflt(s32 value) { return floatBits((float)value); }
s32 _ffix(u32 value) {
    if ((value&0x7FFFFFFF)>=0x4F000000) return value&0x80000000 ? (-2147483647-1) : 2147483647;
    return (s32)floatValue(value);
}
float _f_lltof(s64 value) { return (float)value; }
float _f_ulltof(u64 value) { return (float)value; }
double _dsqrt(double value) {
    union { u64 bits; double value; } v={.value=value};
    u64 magnitude=v.bits&0x7FFFFFFFFFFFFFFFULL;
    if (((v.bits>>63) && magnitude) || magnitude>0x7FF0000000000000ULL) {
        v.bits|=0x7FF8000000000000ULL; errno=0x21; return v.value;
    }
    if (!magnitude || magnitude==0x7FF0000000000000ULL) return value;
    return __builtin_sqrt(value);
}
