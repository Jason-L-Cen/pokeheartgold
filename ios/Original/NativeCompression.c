// SPDX-License-Identifier: GPL-3.0-or-later
#include "NativeCompression.h"
#include <stdint.h>
#include <nitro/hw/native_graphics.h>

static u8 *byteAt(const void *base, u32 offset, BOOL mapped) {
    if (!mapped) return (u8 *)base + offset;
    u32 size = 1;
    return HGNativeMemorySpan((uaddr)base + offset, &size);
}
static BOOL take(const u8 *input, u32 size, u32 *offset, BOOL mapped, u32 *value) {
    if (*offset == size) return FALSE;
    const u8 *byte = byteAt(input, *offset, mapped);
    if (!byte) return FALSE;
    *value = *byte; ++*offset; return TRUE;
}
static BOOL decode(const void *source, u32 sourceSize, void *destination, u32 destinationSize,
                   BOOL mapped, BOOL write, u32 *consumed) {
    if (!source || sourceSize < 4 || (uintptr_t)source > UINTPTR_MAX - sourceSize) return FALSE;
    const u8 *input = source; u8 *output = destination;
    u32 read = 0, header[4];
    for (u32 i = 0; i < 4; i++) if (!take(input, sourceSize, &read, mapped, &header[i])) return FALSE;
    if (header[0] != 0x10 && header[0] != 0x11) return FALSE;
    u32 size = header[1] | (header[2] << 8) | (header[3] << 16);
    if (size > destinationSize || (size && !output) || (uintptr_t)output > UINTPTR_MAX - size) return FALSE;
    if (!mapped && size && (uintptr_t)input < (uintptr_t)output + size && (uintptr_t)output < (uintptr_t)input + sourceSize) return FALSE;
    u32 written = 0;
    while (written < size) {
        u32 flags;
        if (!take(input, sourceSize, &read, mapped, &flags)) return FALSE;
        for (unsigned bit = 0x80; bit && written < size; bit >>= 1) {
            u32 first;
            if (!take(input, sourceSize, &read, mapped, &first)) return FALSE;
            if (!(flags & bit)) {
                if (write) { u8 *byte = byteAt(output, written, mapped); if (!byte) return FALSE; *byte = (u8)first; }
                written++; continue;
            }
            u32 length, distanceHigh = first & 15;
            if (header[0] == 0x10) length = (first >> 4) + 3;
            else if ((first >> 4) >= 2) length = (first >> 4) + 1;
            else {
                u32 second;
                if (!take(input, sourceSize, &read, mapped, &second)) return FALSE;
                if (first & 0x10) {
                    u32 third;
                    if (!take(input, sourceSize, &read, mapped, &third)) return FALSE;
                    length = ((first & 15) << 12) + (second << 4) + (third >> 4) + 0x111;
                    distanceHigh = third & 15;
                } else {
                    length = ((first & 15) << 4) + (second >> 4) + 0x11;
                    distanceHigh = second & 15;
                }
            }
            u32 last;
            if (!take(input, sourceSize, &read, mapped, &last)) return FALSE;
            u32 distance = (distanceHigh << 8) + last + 1;
            if (distance > written || length > size - written) return FALSE;
            if (!write) { written += length; continue; }
            for (u32 i = 0; i < length; i++, written++) {
                const u8 *from = byteAt(output, written - distance, mapped);
                u8 *to = byteAt(output, written, mapped);
                if (!from || !to) return FALSE;
                *to = *from;
            }
        }
    }
    if (consumed) *consumed = read;
    return TRUE;
}
BOOL HGNativeUncompressLZ(const void *source, u32 sourceSize, void *destination, u32 destinationSize) {
    return decode(source, sourceSize, destination, destinationSize, FALSE, TRUE, NULL);
}
BOOL HGNativeUncompressLZMapped(const void *source, void *destination) {
    u32 header[4], read = 0;
    for (u32 i = 0; i < 4; i++) if (!take(source, 4, &read, TRUE, &header[i])) return FALSE;
    u32 size = header[1] | (header[2] << 8) | (header[3] << 16), consumed = 0;
    // The SDK has no source length: validate tokens before writing; NARC callers retain explicit member bounds.
    u32 maximum = 4 + size + (size + 7) / 8;
    if (!decode(source, maximum, destination, size, TRUE, FALSE, &consumed)) return FALSE;
    if (size && (uaddr)source < (uaddr)destination + size && (uaddr)destination < (uaddr)source + consumed) return FALSE;
    for (u32 offset = 0; offset < size;) {
        u32 count = size - offset;
        const u8 *to = HGNativeMemorySpan((uaddr)destination + offset, &count);
        if (!to || !count) return FALSE;
        for (u32 inputOffset = 0; inputOffset < consumed;) {
            u32 inputCount = consumed - inputOffset;
            const u8 *from = HGNativeMemorySpan((uaddr)source + inputOffset, &inputCount);
            if (!from || !inputCount || ((uaddr)from < (uaddr)to + count && (uaddr)to < (uaddr)from + inputCount)) return FALSE;
            inputOffset += inputCount;
        }
        offset += count;
    }
    return decode(source, consumed, destination, size, TRUE, TRUE, NULL);
}
