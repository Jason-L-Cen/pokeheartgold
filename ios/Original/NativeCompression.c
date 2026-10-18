// SPDX-License-Identifier: GPL-3.0-or-later
#include "NativeCompression.h"
#include <stdint.h>

BOOL HGNativeUncompressLZ(const void *source, u32 sourceSize, void *destination, u32 destinationSize) {
    if (!source || sourceSize < 4 || (uintptr_t)source > UINTPTR_MAX - sourceSize) return FALSE;
    const u8 *input = source; u8 *output = destination;
    if (input[0] != 0x10 && input[0] != 0x11) return FALSE;
    u32 size = input[1] | (input[2] << 8) | (input[3] << 16);
    if (size > destinationSize || (size && !output) || (uintptr_t)output > UINTPTR_MAX - size) return FALSE;
    if (size && (uintptr_t)input < (uintptr_t)output + size && (uintptr_t)output < (uintptr_t)input + sourceSize) return FALSE;
    u32 read = 4, written = 0;
    while (written < size) {
        if (read == sourceSize) return FALSE;
        u8 flags = input[read++];
        for (unsigned bit = 0x80; bit && written < size; bit >>= 1) {
            if (read == sourceSize) return FALSE;
            u32 first = input[read++];
            if (!(flags & bit)) { output[written++] = first; continue; }
            u32 length, distanceHigh = first & 15;
            if (input[0] == 0x10) length = (first >> 4) + 3;
            else if ((first >> 4) >= 2) length = (first >> 4) + 1;
            else {
                if (read == sourceSize) return FALSE;
                u32 second = input[read++];
                if (first & 0x10) {
                    if (read == sourceSize) return FALSE;
                    u32 third = input[read++];
                    length = ((first & 15) << 12) + (second << 4) + (third >> 4) + 0x111;
                    distanceHigh = third & 15;
                } else {
                    length = ((first & 15) << 4) + (second >> 4) + 0x11;
                    distanceHigh = second & 15;
                }
            }
            if (read == sourceSize) return FALSE;
            u32 distance = (distanceHigh << 8) + input[read++] + 1;
            if (distance > written || length > size - written) return FALSE;
            for (u32 i = 0; i < length; i++, written++) output[written] = output[written - distance];
        }
    }
    return TRUE;
}
