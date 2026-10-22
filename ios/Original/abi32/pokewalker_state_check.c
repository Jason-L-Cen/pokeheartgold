// SPDX-License-Identifier: GPL-3.0-or-later
#include "application/pokewalker/pokewalker_ir_internal.h"
#include "heap.h"
#include <stddef.h>
#include <string.h>
#define CHECK(v) do { if (!(v)) return __LINE__; } while (0)
u32 CheckOriginalPokewalkerState(void) {
    PokewalkerIrNativeState saved; memcpy(&saved,&gNativePokewalkerIrState,sizeof(saved));
    memset(&gNativePokewalkerIrState,0,sizeof(saved));
    CHECK((u8 *)gPokewalkerIrImageSizes-(u8 *)&gPokewalkerIrWork==0x4C);
    CHECK((u8 *)gPokewalkerIrStrollSizes-(u8 *)&gPokewalkerIrWork==0x54);
    CHECK((u8 *)&gPokewalkerIrMessageQueue-(u8 *)&gPokewalkerIrWork==0x7C);
    CHECK((u8 *)&gPokewalkerIrTransfer-(u8 *)&gPokewalkerIrWork==0x80);
    CHECK(gPokewalkerIrSendBuffer-(u8 *)&gPokewalkerIrWork==0xC0);
    gPokewalkerIrWork.transferStartTick=0x123456789ABCDEF0ULL;
    u64 tick; memcpy(&tick,(u8 *)&gPokewalkerIrWork+0x3C,8); CHECK(tick==0x123456789ABCDEF0ULL);
    u8 a[8]={0},b[8]={0},c[8]={0},d[8]={0}; OSMessage received;
    OS_InitMessageQueue(&gPokewalkerIrMessageQueue,gPokewalkerIrBuffers,1);
    CHECK(OS_SendMessage(&gPokewalkerIrMessageQueue,a,0));
    PokewalkerIr_RequestUpload(a,b,c);
    CHECK(gPokewalkerIrWork.uploadImage==a && gPokewalkerIrWork.strollData==b && gPokewalkerIrWork.records==c);
    CHECK(gPokewalkerIrTransfer.command==0x32 && gPokewalkerIrTransfer.step==0x20 && gPokewalkerIrTransfer.totalSize==0xB732 && gPokewalkerIrTransfer.totalChunks==367);
    CHECK(OS_ReceiveMessage(&gPokewalkerIrMessageQueue,&received,0) && received==a);
    PokewalkerIr_RequestReceive(a,b,c,d);
    CHECK(gPokewalkerIrWork.walkerMon==a && (void *)gPokewalkerIrWork.status==b && (void *)gPokewalkerIrWork.blocks==c && gPokewalkerIrWork.records==d);
    CHECK(gPokewalkerIrTransfer.command==0x40 && gPokewalkerIrTransfer.totalSize==0x2BCA);
    PokewalkerIrStatus status={0}; PokewalkerIr_RequestReceiveGifts(&status,d);
    CHECK(gPokewalkerIrWork.status==&status && gPokewalkerIrTransfer.command==0x60 && gPokewalkerIrTransfer.totalSize==0x24D8 && gPokewalkerIrTransfer.totalChunks==74);
    PokewalkerIr_BeginTransfer(a,0x1234,320);
    CHECK(gPokewalkerIrTransfer.data==a && gPokewalkerIrTransfer.dataStart==a && gPokewalkerIrTransfer.addressStart==0x1234 && gPokewalkerIrTransfer.remaining==320 && gPokewalkerIrTransfer.numChunks==3);
    gPokewalkerIrTransfer.chunk=1; gPokewalkerIrTransfer.totalChunk=37;
    CHECK(PokewalkerIr_GetChunkProgress()==85 && PokewalkerIr_GetTotalProgress()==127);
    u8 *buffer=Heap_Alloc(HEAP_ID_DEFAULT,0x10000); CHECK(buffer);
    PokewalkerIr_SetCompressBuffer(buffer);
    CHECK(gPokewalkerIrWork.compressedSizes==buffer+0xCD80 && gPokewalkerIrWork.compressBuffer==buffer);
    gPokewalkerIrWork.compressedImageSizes=a; gPokewalkerIrWork.compressedStrollSizes=b;
    CHECK(*gPokewalkerIrImageSizes==a && *gPokewalkerIrStrollSizes==b);
    Heap_Free(buffer);
    u32 active=gPokewalkerIrLink.active; gPokewalkerIrLink.active=0; gPokewalkerIrWork.error=47;
    CHECK(PokewalkerIr_GetAndClearError()==47 && gPokewalkerIrWork.error==1);
    gPokewalkerIrLink.active=1; CHECK(PokewalkerIr_GetAndClearError()==1 && !gPokewalkerIrWork.error);
    gPokewalkerIrLink.active=active; memcpy(&gNativePokewalkerIrState,&saved,sizeof(saved)); return 0;
}
