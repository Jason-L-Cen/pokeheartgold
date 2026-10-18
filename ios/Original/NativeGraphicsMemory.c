// SPDX-License-Identifier: GPL-3.0-or-later
#include <nitro/hw/io_reg.h>
#include "assert.h"
#include <stdint.h>
#include <string.h>

static u32 registers[0x1070 / 4];
static u32 palettes[0x800 / 4], objects[0x800 / 4], vram[0xA4000 / 4];
static const u32 bankOffset[9] = {0, 0x20000, 0x40000, 0x60000, 0x80000, 0x90000, 0x94000, 0x98000, 0xA0000};
static const u32 bankSize[9] = {0x20000, 0x20000, 0x20000, 0x20000, 0x10000, 0x4000, 0x4000, 0x8000, 0x4000};
static const u32 bankControl[9] = {0x240, 0x241, 0x242, 0x243, 0x244, 0x245, 0x246, 0x248, 0x249};

uaddr HGNativeRegisterAddress(u32 address, u32 width) {
    GF_ASSERT(width == 1 || width == 2 || width == 4);
    GF_ASSERT(address % width == 0);
    u32 offset = address - 0x04000000u;
    // These are graphics state registers. FIFO, capture and other hardware need active backends.
    BOOL supported = (offset < 0x64 && width <= 0x64 - offset) ||
        (offset >= 0x6C && offset < 0x70 && width <= 0x70 - offset) ||
        (offset >= 0x1000 && offset < 0x1070 && width <= 0x1070 - offset) ||
        (offset >= 0x240 && offset < 0x247 && width <= 0x247 - offset) ||
        (offset >= 0x248 && offset < 0x24A && width <= 0x24A - offset) ||
        (offset == 0x304 && width <= 2);
    GF_ASSERT(supported);
    return (uaddr)((u8 *)registers + offset);
}

static BOOL bankAddress(unsigned bank, u8 control, u32 *address) {
    if (!(control & 0x80)) return FALSE;
    u32 mode = control & 7, slot = (control >> 3) & 3;
    if (!mode) { *address = 0x06800000u + bankOffset[bank]; return TRUE; }
    if (bank < 4) {
        if (mode == 1) *address = 0x06000000u + slot * 0x20000;
        else if (mode == 2 && bank < 2 && slot < 2) *address = 0x06400000u + slot * 0x20000;
        else if (mode == 4 && bank == 2) *address = 0x06200000u;
        else if (mode == 4 && bank == 3) *address = 0x06600000u;
        else return FALSE; // Texture/ARM7 mappings are not CPU-visible here.
    } else if (bank < 7 && (mode == 1 || mode == 2)) {
        u32 offset = bank == 4 ? 0 : ((slot & 1) * 0x4000 + (slot & 2) * 0x8000);
        *address = (mode == 1 ? 0x06000000u : 0x06400000u) + offset;
    } else if (bank == 7 && mode == 1) *address = 0x06200000u;
    else if (bank == 8 && mode == 1) *address = 0x06208000u;
    else if (bank == 8 && mode == 2) *address = 0x06600000u;
    else return FALSE;
    return TRUE;
}

void *HGNativeMemorySpan(uaddr address, u32 *size) {
    if (!size || !*size || address > UINTPTR_MAX - *size) return NULL;
    if (address > UINT32_MAX) return (void *)address;
#ifdef HEARTGOLD_ABI32
    // Generated C uses bounded offsets below the original DS main-RAM address range.
    if (address && address < 0x02000000u && *size <= 0x02000000u - address &&
        (u64)address + *size <= (u64)__builtin_wasm_memory_size(0) * 65536) return (void *)address;
#endif
    if (address >= 0x05000000u && address < 0x05000800u) {
        if (*size > 0x05000800u - address) *size = 0x05000800u - address;
        return (u8 *)palettes + address - 0x05000000u;
    }
    if (address >= 0x07000000u && address < 0x07000800u) {
        if (*size > 0x07000800u - address) *size = 0x07000800u - address;
        return (u8 *)objects + address - 0x07000000u;
    }
    void *result = NULL;
    u32 count = *size;
    for (unsigned bank = 0; bank < 9; bank++) {
        u32 base;
        if (!bankAddress(bank, ((u8 *)registers)[bankControl[bank]], &base) ||
            address < base || address - base >= bankSize[bank]) continue;
        // shortcut: overlapping bank mappings need bus OR/broadcast semantics; reject until used.
        if (result) return NULL;
        u32 offset = (u32)(address - base);
        count = bankSize[bank] - offset;
        result = (u8 *)vram + bankOffset[bank] + offset;
    }
    if (result && *size > count) *size = count;
    return result;
}

const void *HGNativeGraphicsBank(u32 bank) {
    GF_ASSERT(bank < 9); return (const u8 *)vram + bankOffset[bank];
}

void HGNativeSnapshotGraphics(u32 address, void *destination, u32 size) {
    GF_ASSERT(destination && size && address <= UINT32_MAX - size);
    const u32 base[] = {0x05000000, 0x06000000, 0x06200000, 0x06400000, 0x06600000, 0x07000000};
    const u32 length[] = {0x800, 0x80000, 0x20000, 0x40000, 0x20000, 0x800};
    BOOL valid = FALSE;
    for (unsigned i = 0; i < 6; i++)
        if (address >= base[i] && address - base[i] < length[i] && size <= length[i] - (address - base[i])) valid = TRUE;
    GF_ASSERT(valid);
    if (address >= 0x05000000 && address + size <= 0x05000800) {
        memcpy(destination, (u8 *)palettes + address - 0x05000000, size); return;
    }
    if (address >= 0x07000000 && address + size <= 0x07000800) {
        memcpy(destination, (u8 *)objects + address - 0x07000000, size); return;
    }
    memset(destination, 0, size); // Unmapped graphics reads are zero, overlapping banks are ORed.
    for (unsigned bank = 0; bank < 9; bank++) {
        u32 start;
        if (!bankAddress(bank, ((u8 *)registers)[bankControl[bank]], &start)) continue;
        u32 first = address > start ? address : start;
        u32 end = address + size < start + bankSize[bank] ? address + size : start + bankSize[bank];
        for (u32 position = first; position < end; position++)
            ((u8 *)destination)[position - address] |= ((u8 *)vram)[bankOffset[bank] + position - start];
    }
}
