// SPDX-License-Identifier: GPL-3.0-or-later
#include "heap.h"
#include <nnsys/fnd_internal.h>
#include <string.h>
#define CHECK(v) do { if (!(v)) return __LINE__; } while (0)
u32 CheckNativeArenaBounds(void);

u32 HGArenaReference(u32 id,u32 field) {
    if (id>=OS_ARENA_MAX) return 0;
    if (!field) return (u32)OS_GetInitArenaLo(id);
    if (field==1) return (u32)OS_GetInitArenaHi(id);
    if (field==2) return (u32)OS_GetArenaLo(id);
    return (u32)OS_GetArenaHi(id);
}
u32 HGArenaTake(u32 size) { return (u32)OS_AllocFromArenaLo(OS_ARENA_MAIN,size,4); }
u32 InitializeOriginalGameHeap(void) {
    u32 line=CheckNativeArenaBounds(); if (line) return line;
    OS_InitArenaEx();
    CHECK(!OS_GetArenaLo(OS_ARENA_MAINEX) && !OS_GetArenaHi(OS_ARENA_MAINEX));
    CHECK(OS_GetInitArenaLo(OS_ARENA_WRAM_MAIN)==OS_GetInitArenaHi(OS_ARENA_WRAM_MAIN));
    CHECK(!OS_GetInitArenaLo(OS_ARENA_MAIN_SUBPRIV) && !OS_GetInitArenaLo(OS_ARENA_WRAM_SUB));
    const HeapParam template={2*1024*1024,OS_ARENA_MAIN};
    Heap_InitSystem(&template,1,128,0);
    CHECK(Heap_Create(HEAP_ID_DEFAULT,HEAP_ID_15,16*1024));
    return 0;
}
u32 CheckOriginalGameHeaps(void) {
    u32 before=HeapExp_FndGetTotalFreeSize(HEAP_ID_DEFAULT);
    CHECK(Heap_Create(HEAP_ID_DEFAULT,HEAP_ID_FIELD1,64*1024));
    CHECK(Heap_CreateAtEnd(HEAP_ID_DEFAULT,HEAP_ID_BATTLE,64*1024));
    u8 *head=Heap_Alloc(HEAP_ID_FIELD1,128),*tail=Heap_AllocAtEnd(HEAP_ID_FIELD1,129);
    CHECK(head && tail && (uaddr)head<(uaddr)tail);
    memset(head,0x5A,128); memset(tail,0xC3,129);
    Heap_Realloc(head,16);
    for (u32 i=0;i<16;i++) CHECK(head[i]==0x5A);
    for (u32 i=0;i<129;i++) CHECK(tail[i]==0xC3);
    Heap_Free(head); Heap_FreeExplicit(HEAP_ID_FIELD1,tail);
    void *last=Heap_AllocAtEnd(HEAP_ID_BATTLE,64); CHECK(last); Heap_Free(last);
    Heap_Destroy(HEAP_ID_FIELD1); Heap_Destroy(HEAP_ID_BATTLE);
    CHECK(HeapExp_FndGetTotalFreeSize(HEAP_ID_DEFAULT)==before);
    CHECK(!Heap_Alloc(HEAP_ID_DEFAULT,UINT32_MAX) && !Heap_AllocAtEnd(HEAP_ID_DEFAULT,UINT32_MAX));
    CHECK(!Heap_Alloc(HEAP_ID_DEFAULT,UINT32_MAX-16) && !Heap_Alloc(HEAP_ID_DEFAULT,UINT32_MAX-32));
    CHECK(HeapExp_FndGetTotalFreeSize(HEAP_ID_DEFAULT)==before);
    _Alignas(32) u8 storage[2048];
    CHECK(!NNS_FndCreateExpHeapEx(NULL,128,0));
    CHECK(!NNS_FndCreateExpHeapEx((void *)(UINTPTR_MAX-15),128,0));
    NNSFndHeapHandle heap=NNS_FndCreateExpHeapEx(storage,sizeof(storage),0); CHECK(heap);
    u32 available=NNS_FndGetTotalFreeSizeForExpHeap(heap);
    CHECK(!NNS_FndAllocFromExpHeapEx(heap,UINT32_MAX,4));
    CHECK(!NNS_FndAllocFromExpHeapEx(heap,UINT32_MAX-32,4));
    CHECK(!NNS_FndAllocFromExpHeapEx(heap,8,INT32_MIN));
    CHECK(!NNS_FndAllocFromExpHeapEx(heap,8,3));
    void *block=NNS_FndAllocFromExpHeapEx(heap,64,4); CHECK(block);
    memset(block,0xD7,64);
    CHECK(!NNS_FndResizeForMBlockExpHeap(heap,block,UINT32_MAX));
    for (u32 i=0;i<64;i++) CHECK(((u8 *)block)[i]==0xD7);
    NNS_FndFreeToExpHeap(heap,block);
    CHECK(NNS_FndGetTotalFreeSizeForExpHeap(heap)==available);
    NNS_FndDestroyExpHeap(heap);
    return 0;
}
