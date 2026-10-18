// SPDX-License-Identifier: GPL-3.0-or-later
#include "pokemon.h"
#include "math_util.h"
#include "../NativeFile.h"
#include "orbit_cursor.h"
#include <nnsys/fnd_internal.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#define CHECK(value) do { if (!(value)) { fprintf(stderr, "CHECK failed at line %d: %s\n", __LINE__, #value); abort(); } } while (0)
PokemonDataBlock *GetSubstruct(BoxPokemon *boxMon, u32 pid, u8 which);
void CheckNativeRuntime(void);
void CheckNativeDMA(void);

static u64 nativeVarargs(const char *label, ...) {
    va_list args; va_start(args, label);
    u64 first = va_arg(args, u64);
    int second = va_arg(args, int);
    va_end(args);
    return first + second;
}

_Static_assert(sizeof(u32) == 4 && sizeof(s32) == 4, "DS integers must remain 32-bit on iOS");
_Static_assert(sizeof(u64) == 8 && sizeof(s64) == 8, "DS integers must remain 64-bit on iOS");
_Static_assert(_Alignof(u64) == 4, "DS persistent data aligns 64-bit fields to 4 bytes");
_Static_assert(sizeof(BASE_STATS) == 0x2C, "personal table entry layout");
_Static_assert(sizeof(PokemonDataBlockA) == 32, "Pokemon block A layout");
_Static_assert(sizeof(PokemonDataBlockB) == 32, "Pokemon block B layout");
_Static_assert(sizeof(PokemonDataBlockC) == 32, "Pokemon block C layout");
_Static_assert(sizeof(PokemonDataBlockD) == 32, "Pokemon block D layout");
_Static_assert(offsetof(BoxPokemon, checksum) == 6 && offsetof(BoxPokemon, dataBlocks) == 8, "encrypted box header layout");
_Static_assert(sizeof(BoxPokemon) == 0x88, "boxed Pokemon binary layout");
_Static_assert(offsetof(Pokemon, party) == 0x88 && sizeof(Pokemon) == 0xEC, "party Pokemon binary layout");
_Static_assert(FS_OVERLAY_ID(OVY_36) == 36 && FS_OVERLAY_ID(field) == 1, "original overlay IDs are IDs, not native pointer casts");

int main(int argc, char **argv) {
    _Alignas(16) u8 arena[65536];
    NNSFndHeapHandle heap = NNS_FndCreateExpHeapEx(arena, sizeof(arena), 1);
    CHECK(heap);
    u32 available = NNS_FndGetTotalFreeSizeForExpHeap(heap);
    void *head = NNS_FndAllocFromExpHeapEx(heap, 13, 4), *tail = NNS_FndAllocFromExpHeapEx(heap, 27, -32);
    CHECK(head && tail && (uintptr_t)head % 8 == 0 && (uintptr_t)tail % 32 == 0 && head < tail);
    CHECK(NNS_FndGetSizeForMBlockExpHeap(head) >= 13);
    CHECK(NNS_FndResizeForMBlockExpHeap(heap, head, 256) >= 256);
    CHECK(NNS_FndResizeForMBlockExpHeap(heap, head, 16) >= 16);
    NNS_FndFreeToExpHeap(heap, head); NNS_FndFreeToExpHeap(heap, tail);
    CHECK(NNS_FndGetTotalFreeSizeForExpHeap(heap) == available);
    NNSFndAllocator allocator; NNS_FndInitAllocatorForExpHeap(&allocator, heap, 16);
    void *childStorage = NNS_FndAllocFromAllocator(&allocator, 4096);
    CHECK(childStorage && (uintptr_t)childStorage % 16 == 0);
    NNSFndHeapHandle child = NNS_FndCreateExpHeapEx(childStorage, 4096, 0);
    CHECK(child && heap->childList.numObjects == 1);
    NNS_FndDestroyExpHeap(child); NNS_FndFreeToAllocator(&allocator, childStorage);
    CHECK(heap->childList.numObjects == 0 && NNS_FndGetTotalFreeSizeForExpHeap(heap) == available);
    NNS_FndDestroyExpHeap(heap);
    OS_InitArena();
    void *arenaLow = OS_GetArenaLo(OS_ARENA_DTCM), *arenaHigh = OS_GetArenaHi(OS_ARENA_DTCM);
    void *lowBlock = OS_AllocFromArenaLo(OS_ARENA_DTCM, 13, 32), *highBlock = OS_AllocFromArenaHi(OS_ARENA_DTCM, 29, 64);
    CHECK(lowBlock && highBlock && lowBlock < highBlock);
    CHECK((uintptr_t)lowBlock % 32 == 0 && (uintptr_t)highBlock % 64 == 0);
    CHECK(OS_AllocFromArenaLo(OS_ARENA_DTCM, UINT32_MAX, 4) == NULL);
    CHECK(OS_AllocFromArenaHi(OS_ARENA_DTCM, 32, 3) == NULL);
    OS_SetArenaLo(OS_ARENA_DTCM, arenaLow); OS_SetArenaHi(OS_ARENA_DTCM, arenaHigh);
    OSIntrMode outer = OS_DisableInterrupts(), inner = OS_DisableInterrupts();
    CHECK(outer == OS_INTRMODE_ENABLE && inner == OS_INTRMODE_DISABLE_IRQ);
    OS_RestoreInterrupts(inner); CHECK(OS_GetCpsrIrq() == OS_INTRMODE_IRQ_DISABLE);
    OS_RestoreInterrupts(outer); CHECK(OS_GetCpsrIrq() == OS_INTRMODE_IRQ_ENABLE);
    static const HeapParam templates[] = {
        {0xD200, OS_ARENA_MAIN}, {0x23600, OS_ARENA_MAIN}, {0x10, OS_ARENA_MAIN}, {0x11D000, OS_ARENA_MAIN}
    };
    Heap_InitSystem(templates, 4, HEAP_ID_MAX, 0);
    u32 parentFree = HeapExp_FndGetTotalFreeSize(HEAP_ID_3);
    CHECK(Heap_Create(HEAP_ID_3, HEAP_ID_FIELD1, 65536));
    void *blockHead = Heap_Alloc(HEAP_ID_FIELD1, 256), *blockTail = Heap_AllocAtEnd(HEAP_ID_FIELD1, 256);
    CHECK(blockHead && blockTail && blockHead < blockTail);
    memset(blockHead, 0xA5, 256); Heap_Realloc(blockHead, 64);
    CHECK(((u8 *)blockHead)[63] == 0xA5);
    Heap_Free(blockHead); Heap_FreeExplicit(HEAP_ID_FIELD1, blockTail);
    Heap_Destroy(HEAP_ID_FIELD1);
    CHECK(HeapExp_FndGetTotalFreeSize(HEAP_ID_3) == parentFree);
    CheckNativeRuntime();
    CheckNativeDMA();
    CHECK(nativeVarargs("native", (u64)0x1234567887654321ULL, 9) == 0x123456788765432AULL);
    CHECK(sizeof(strlen("native")) == sizeof(size_t));
    const u16 expected[] = {0x0000, 0xE97E, 0x5271, 0x31B0, 0x8E42, 0xE2CC, 0xAFC5, 0x67DB};
    SetLCRNGSeed(0);
    for (unsigned i = 0; i < sizeof(expected) / sizeof(*expected); i++) CHECK(LCRandom() == expected[i]);
    SetMTRNGSeed(5489);
    CHECK(MTRandom() == 3499211612u && MTRandom() == 581869302u && MTRandom() == 3890346734u);
    u16 encrypted[8] = {0};
    _MonEncryptSegment(encrypted, sizeof(encrypted), 0);
    CHECK(memcmp(encrypted, expected, sizeof(expected)) == 0);
    _MonDecryptSegment(encrypted, sizeof(encrypted), 0);
    for (unsigned i = 0; i < 8; i++) CHECK(encrypted[i] == 0);
    BoxPokemon box;
    ZeroBoxMonData(&box);
    CHECK(box.personality == 0 && box.checksum == 0);
    MonDecryptSegment(box.dataBlocks, sizeof(box.dataBlocks), box.checksum);
    CHECK(CalcMonChecksum(box.dataBlocks, sizeof(box.dataBlocks)) == 0);
    for (u32 permutation = 0; permutation < 32; permutation++) {
        unsigned seen = 0;
        for (u8 block = 0; block < 4; block++) {
            intptr_t offset = (char *)GetSubstruct(&box, permutation << 13, block) - (char *)box.dataBlocks;
            CHECK(offset >= 0 && offset <= 96 && offset % 32 == 0);
            seen |= 1u << (offset / 32);
        }
        CHECK(seen == 15);
    }
    CHECK(ModifyStatByNature(0, 100, STAT_ATK) == 100);
    CHECK(ModifyStatByNature(3, 100, STAT_ATK) == 110);
    CHECK(ModifyStatByNature(3, 100, STAT_SPATK) == 90);
    CHECK(argc == 2 && HGNativeFSSetRoot(argv[1]));
    FSFile file; FS_InitFile(&file);
    CHECK(!FS_OpenFile(&file, "../manifest.json") && !FS_OpenFile(&file, "/etc/passwd"));
    CHECK(FS_OpenFile(&file, "rom:/a/0/0/2") && FS_GetLength(&file) > 0);
    CHECK(!HGNativeFSSetRoot(argv[1]));
    CHECK(FS_SeekFile(&file, -99, FS_SEEK_SET) && file.prop.file.pos == 0);
    char signature[4]; CHECK(FS_ReadFile(&file, signature, 4) == 4 && memcmp(signature, "NARC", 4) == 0);
    CHECK(FS_SeekFile(&file, INT32_MAX, FS_SEEK_SET) && file.prop.file.pos == FS_GetLength(&file));
    CHECK(FS_ReadFileAsync(&file, signature, 4) == 0 && FS_WaitAsync(&file));
    CHECK(FS_CloseFile(&file));
    BASE_STATS personal;
    LoadMonPersonal(SPECIES_CHIKORITA, &personal);
    CHECK(personal.hp == 45 && personal.atk == 49 && personal.def == 65);
    for (unsigned i = 0; i < 1000; i++) CHECK(GetNarcMemberSizeByIdPair(NARC_poketool_personal_personal, SPECIES_CHIKORITA) == sizeof(personal));
    CHECK(HGNativeFSSetRoot(argv[1]));
    NARC *narc = NARC_New(NARC_poketool_personal_personal, HEAP_ID_DEFAULT);
    CHECK(narc && NARC_GetFileCount(narc) >= 494);
    NARC_ReadWholeMember(narc, SPECIES_ARCEUS, &personal); CHECK(personal.hp == 120 && personal.atk == 120);
    NARC_Delete(narc); CHECK(HGNativeFSSetRoot(argv[1]));
    Pokemon mon;
    CreateMon(&mon, SPECIES_CHIKORITA, 5, 31, TRUE, 0, 1, 12345);
    CHECK(GetMonData(&mon, MON_DATA_SPECIES, NULL) == SPECIES_CHIKORITA);
    CHECK(GetMonData(&mon, MON_DATA_LEVEL, NULL) == 5);
    CHECK(GetMonData(&mon, MON_DATA_MAX_HP, NULL) == 21);
    for (u32 species = 1; species <= SPECIES_ARCEUS; species++) {
        CreateMon(&mon, species, 100, 31, TRUE, species * 101, 1, 12345);
        Pokemon serialized = mon;
        CHECK(GetMonData(&mon, MON_DATA_SPECIES, NULL) == species);
        CHECK(GetMonData(&mon, MON_DATA_LEVEL, NULL) == 100);
        CHECK(GetMonData(&mon, MON_DATA_HP, NULL) > 0);
        CHECK(GetMonData(&mon, MON_DATA_HP_IV, NULL) == 31);
        CHECK(memcmp(&serialized, &mon, sizeof(mon)) == 0);
    }
    CreateMon(&mon, SPECIES_CHIKORITA, 50, 31, TRUE, 3, 1, 0);
    u32 oldAttack = GetMonData(&mon, MON_DATA_ATK, NULL), effort = 252;
    SetMonData(&mon, MON_DATA_ATK_EV, &effort); CalcMonStats(&mon);
    CHECK(GetMonData(&mon, MON_DATA_ATK, NULL) > oldAttack && GetMonData(&mon, MON_DATA_ATK_EV, NULL) == 252);
    CreateMon(&mon, SPECIES_PIKACHU, 5, 32, FALSE, 0, 0, 0);
    CHECK(GetMonData(&mon, MON_DATA_SPECIES, NULL) == SPECIES_PIKACHU);
    OrbitCursor *cursor = OrbitCursor_New(12, HEAP_ID_DEFAULT, 0, 0, 0, NULL, NULL, NULL);
    CHECK(cursor && cursor->count == 12 && cursor->state == 0 && cursor->task == NULL);
    CHECK(cursor->radiusX == 15 && cursor->radiusY == 15); Heap_Free(cursor);
    printf("PASS: original C runs natively: original heap manager/SDK heaps, native arenas/critical sections, layouts, RNG, crypto, all 493 species at Lv100, encrypted round trips, EVs/natures, NARC and file lifetime.\n");
    return 0;
}
