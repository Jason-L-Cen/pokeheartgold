// SPDX-License-Identifier: GPL-3.0-or-later
#include "application/pokewalker/pokewalker_ir_internal.h"
#include "heap.h"
#include <stddef.h>
#include <string.h>
#define CHECK(v) do { if (!(v)) return __LINE__; } while (0)
u32 CheckOriginalPokewalkerState(void) {
    PokewalkerIrNativeState saved; memcpy(&saved,&gNativePokewalkerIrState,sizeof(saved));
    memset(&gNativePokewalkerIrState,0,sizeof(saved));
    CHECK((u8 *)ov112_021FFAF0-(u8 *)&ov112_021FFAA4==0x4C);
    CHECK((u8 *)ov112_021FFAF8-(u8 *)&ov112_021FFAA4==0x54);
    CHECK((u8 *)&ov112_021FFB20-(u8 *)&ov112_021FFAA4==0x7C);
    CHECK((u8 *)&ov112_021FFB24-(u8 *)&ov112_021FFAA4==0x80);
    CHECK(ov112_021FFB64-(u8 *)&ov112_021FFAA4==0xC0);
    ov112_021FFAA4.transferStartTick=0x123456789ABCDEF0ULL;
    u64 tick; memcpy(&tick,(u8 *)&ov112_021FFAA4+0x3C,8); CHECK(tick==0x123456789ABCDEF0ULL);
    u8 a[8]={0},b[8]={0},c[8]={0},d[8]={0}; OSMessage received;
    OS_InitMessageQueue(&ov112_021FFB20,ov112_021FFAC0,1);
    CHECK(OS_SendMessage(&ov112_021FFB20,a,0));
    PokewalkerIr_RequestUpload(a,b,c);
    CHECK(ov112_021FFAA4.uploadImage==a && ov112_021FFAA4.strollData==b && ov112_021FFAA4.records==c);
    CHECK(ov112_021FFB24.command==0x32 && ov112_021FFB24.step==0x20 && ov112_021FFB24.totalSize==0xB732 && ov112_021FFB24.totalChunks==367);
    CHECK(OS_ReceiveMessage(&ov112_021FFB20,&received,0) && received==a);
    PokewalkerIr_RequestReceive(a,b,c,d);
    CHECK(ov112_021FFAA4.unk_08==a && (void *)ov112_021FFAA4.status==b && (void *)ov112_021FFAA4.blocks==c && ov112_021FFAA4.records==d);
    CHECK(ov112_021FFB24.command==0x40 && ov112_021FFB24.totalSize==0x2BCA);
    PokewalkerIrStatus status={0}; PokewalkerIr_RequestReceiveGifts(&status,d);
    CHECK(ov112_021FFAA4.status==&status && ov112_021FFB24.command==0x60 && ov112_021FFB24.totalSize==0x24D8 && ov112_021FFB24.totalChunks==74);
    PokewalkerIr_BeginTransfer(a,0x1234,320);
    CHECK(ov112_021FFB24.data==a && ov112_021FFB24.dataStart==a && ov112_021FFB24.addressStart==0x1234 && ov112_021FFB24.remaining==320 && ov112_021FFB24.numChunks==3);
    ov112_021FFB24.chunk=1; ov112_021FFB24.totalChunk=37;
    CHECK(PokewalkerIr_GetChunkProgress()==85 && PokewalkerIr_GetTotalProgress()==127);
    u8 *buffer=Heap_Alloc(HEAP_ID_DEFAULT,0x10000); CHECK(buffer);
    PokewalkerIr_SetCompressBuffer(buffer);
    CHECK(ov112_021FFAA4.compressedSizes==buffer+0xCD80 && ov112_021FFAA4.compressBuffer==buffer);
    ov112_021FFAA4.sizes0=a; ov112_021FFAA4.sizes1=b;
    CHECK(*ov112_021FFAF0==a && *ov112_021FFAF8==b);
    Heap_Free(buffer);
    u32 active=gPokewalkerIrLink.active; gPokewalkerIrLink.active=0; ov112_021FFAA4.error=47;
    CHECK(PokewalkerIr_GetAndClearError()==47 && ov112_021FFAA4.error==1);
    gPokewalkerIrLink.active=1; CHECK(PokewalkerIr_GetAndClearError()==1 && !ov112_021FFAA4.error);
    gPokewalkerIrLink.active=active; memcpy(&gNativePokewalkerIrState,&saved,sizeof(saved)); return 0;
}
