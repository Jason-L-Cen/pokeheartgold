// SPDX-License-Identifier: GPL-3.0-or-later
#include <nitro/mi/memory.h>
#include <nitro/hw/native_graphics.h>
#include "assert.h"
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

static void clear(u32 value, void *dst, u32 size, u32 width) {
    uaddr address = (uaddr)dst;
    GF_ASSERT(address % width == 0 && size % width == 0);
    while (size) {
        u32 count = size;
        u8 *native = HGNativeMemorySpan(address, &count); GF_ASSERT(native && count % width == 0);
        if (width == 1) memset(native, (u8)value, count);
        else for (u32 i = 0; i < count; i += width) memcpy(native + i, &value, width);
        address += count; size -= count;
    }
}
static void copy(const void *src, void *dst, u32 size, u32 width) {
    uaddr source = (uaddr)src, destination = (uaddr)dst;
    GF_ASSERT(source % width == 0 && destination % width == 0 && size % width == 0);
    while (size) {
        u32 count = size;
        const u8 *from = HGNativeMemorySpan(source, &count);
        u8 *to = HGNativeMemorySpan(destination, &count); GF_ASSERT(from && to && count % width == 0);
        if ((uaddr)from + count <= (uaddr)to || (uaddr)to + count <= (uaddr)from) memcpy(to, from, count);
        else for (u32 i = 0; i < count; i++) to[i] = from[i];
        source += count; destination += count; size -= count;
    }
}
void MIi_CpuClear16(u16 value, u16 *dst, u32 size) { clear(value, dst, size, 2); }
void MIi_CpuClear32(u32 value, u32 *dst, u32 size) { clear(value, dst, size, 4); }
void MIi_CpuClearFast(u32 value, u32 *dst, u32 size) { MIi_CpuClear32(value, dst, size); }
void MIi_CpuCopy16(const u16 *src, u16 *dst, u32 size) { copy(src, dst, size, 2); }
void MIi_CpuCopy32(const u32 *src, u32 *dst, u32 size) { copy(src, dst, size, 4); }
void MIi_CpuCopyFast(const u32 *src, u32 *dst, u32 size) { copy(src, dst, size, 4); }
void MI_CpuFill8(void *dst, u8 value, u32 size) { clear(value, dst, size, 1); }
void MI_CpuCopy8(const void *src, void *dst, u32 size) { copy(src, dst, size, 1); }
void MI_Copy36B(const void *src, void *dst) { copy(src, dst, 36, 4); }
void MI_Copy64B(const void *src, void *dst) { copy(src, dst, 64, 4); }
