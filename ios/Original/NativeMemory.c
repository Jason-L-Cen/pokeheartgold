// SPDX-License-Identifier: GPL-3.0-or-later
#include <nitro/mi/memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void GF_AssertFail(void) {
    fputs("Original game assertion failed.\n", stderr);
    abort();
}
void HGNativeAssertFail(const char *condition, const char *file, int line) {
    fprintf(stderr, "%s:%d: original game assertion: %s\n", file, line, condition);
    abort();
}

void MIi_CpuClear16(u16 value, u16 *dst, u32 size) {
    if ((uintptr_t)dst % 2 || size % 2) abort();
    for (u32 i = 0; i < size / 2; i++) dst[i] = value;
}
void MIi_CpuClear32(u32 value, u32 *dst, u32 size) {
    if ((uintptr_t)dst % 4 || size % 4) abort();
    for (u32 i = 0; i < size / 4; i++) dst[i] = value;
}
void MIi_CpuClearFast(u32 value, u32 *dst, u32 size) { MIi_CpuClear32(value, dst, size); }
void MIi_CpuCopy16(const u16 *src, u16 *dst, u32 size) { memcpy(dst, src, size); }
void MIi_CpuCopy32(const u32 *src, u32 *dst, u32 size) { memcpy(dst, src, size); }
void MIi_CpuCopyFast(const u32 *src, u32 *dst, u32 size) { memcpy(dst, src, size); }
void MI_CpuFill8(void *dst, u8 value, u32 size) { memset(dst, value, size); }
void MI_CpuCopy8(const void *src, void *dst, u32 size) { memcpy(dst, src, size); }
void MI_Copy36B(const void *src, void *dst) { memcpy(dst, src, 36); }
void MI_Copy64B(const void *src, void *dst) { memcpy(dst, src, 64); }
