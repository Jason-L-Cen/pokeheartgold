// SPDX-License-Identifier: GPL-3.0-or-later
#include "NativeCompression.h"
#include <stdint.h>
#include <nitro/hw/native_graphics.h>
#include <nitro/os/cache.h>
#include "assert.h"

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
static u32 littleWord(const u8 *p) {
    return p[0]|((u32)p[1]<<8)|((u32)p[2]<<16)|((u32)p[3]<<24);
}
static BOOL backward(u8 *buffer,u32 packedSize,u32 capacity,BOOL write) {
    if (!buffer || packedSize<8 || packedSize>capacity || (uintptr_t)buffer>UINTPTR_MAX-capacity) return FALSE;
    u32 footer=littleWord(buffer+packedSize-8),extra=littleWord(buffer+packedSize-4);
    u32 encoded=footer&0xFFFFFF,header=footer>>24;
    if (header<8 || encoded<header || encoded>packedSize || extra>capacity-packedSize) return FALSE;
    u32 first=packedSize-encoded,read=packedSize-header,end=packedSize+extra,out=end;
    while (read>first) {
        u32 flags=buffer[--read];
        for (u32 bit=0x80;bit && read>first;bit>>=1) {
            u32 length=1,distance=0,value=buffer[--read];
            if (flags&bit) {
                if (read==first) return FALSE;
                length=(value>>4)+3; distance=((value&15)<<8)+buffer[--read]+3;
                if (distance>end-out) return FALSE;
            }
            // Original in-place streams must not overwrite unread tokens or the uncompressed prefix.
            if (length>out-first || out-length<read) return FALSE;
            if (write) {
                for (u32 i=0;i<length;i++) { --out; buffer[out]=distance ? buffer[out+distance] : (u8)value; }
            } else out-=length;
        }
    }
    return out==first;
}
BOOL HGNativeUncompressBackward(void *buffer,u32 packedSize,u32 capacity) {
    if (!backward(buffer,packedSize,capacity,FALSE)) return FALSE;
    return backward(buffer,packedSize,capacity,TRUE);
}
void MIi_UncompressBackward(void *bottom) {
    if (!bottom) return;
    uaddr end=(uaddr)bottom; u32 size=8;
    const u8 *footer=end>=8 ? HGNativeMemorySpan(end-8,&size) : NULL;
    GF_ASSERT(footer && size==8);
    u32 packed=littleWord(footer)&0xFFFFFF,extra=littleWord(footer+4);
    GF_ASSERT(packed>=8 && end>=packed && extra<=UINT32_MAX-packed && end<=UINTPTR_MAX-extra);
    u32 total=packed+extra; size=total;
    void *buffer=HGNativeMemorySpan(end-packed,&size);
    BOOL success=buffer && size==total && HGNativeUncompressBackward(buffer,packed,total);
    GF_ASSERT(success);
    DC_FlushRange(buffer,total); IC_InvalidateRange(buffer,total);
}
