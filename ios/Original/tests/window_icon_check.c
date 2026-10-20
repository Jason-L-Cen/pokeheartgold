// SPDX-License-Identifier: GPL-3.0-or-later
// Include the original routine so this probe uses its private ABI views unchanged.
#include "../../../src/render_window_frame_icon.c"
#include <stdio.h>
#include <stdlib.h>
#define CHECK(v) do { if (!(v)) { fprintf(stderr,"Window icon line %d: %s\n",__LINE__,#v); abort(); } } while (0)
static u8 first[32],second[32],icon[128];
static u32 copies,loads,allocations,expectedCount;
static CharDataView character;
int BgConfig_GetHeapId(void *config) { CHECK(config==(void *)16); return 3; }
u8 GetWindowBgId(WindowView *window) { CHECK(window->bgConfig==(void *)16); return 0; }
void *Heap_Alloc(int heap,u32 size) { CHECK(heap==3); void *p=malloc(size); CHECK(p); allocations++; return p; }
void Heap_Free(void *p) { CHECK(p && allocations); allocations--; free(p); }
void *BgGetCharPtr(int bg) { CHECK(!bg); return (void *)0x0601C000; }
void MI_CpuCopy8(const void *source,void *destination,u32 size) {
    CHECK(size==32); uaddr address=(uaddr)source;
    CHECK(address==0x0601FFE0 || address==0x06020000);
    memcpy(destination,address==0x0601FFE0 ? first : second,32); copies++;
}
void *GfGfxLoader_GetCharData(int narc,int member,int compressed,CharDataView **output,int heap) {
    CHECK(narc==38 && member==22 && !compressed); character.pRawData=icon; *output=&character; return Heap_Alloc(heap,1);
}
static void pattern(const u8 *data) {
    for (u32 i=0;i<expectedCount*128;i++) CHECK(data[i]==((i/32)&1 ? second : first)[i%32]);
}
void BlitPixelRect4Bit(void *source,u8 sx,u8 sy,u8 sw,u8 sh,void *dest,u8 dw,u8 dh,u8 dx,u8 dy,u8 width,u8 height) {
    CHECK(source==icon && sx==3 && !sy && sw==13 && sh==expectedCount*16 && dw==13 && dh==sh && !dx && !dy && width==sw && height==sh);
    pattern(dest);
}
void BG_LoadCharTilesData(void *config,int bg,void *data,u32 size,u32 tile) {
    CHECK(config==(void *)16 && !bg && size==expectedCount*128 && tile==519); pattern(data); loads++;
}
int main(void) {
    for (u32 i=0;i<32;i++) { first[i]=(u8)(i+1); second[i]=(u8)(200-i); }
    WindowView window={(void *)16};
    for (expectedCount=1;expectedCount<=4;expectedCount++) {
        copies=loads=0; LoadWindowFrameIconTiles(&window,22,501,(int)expectedCount,3,0);
        CHECK(copies==4*expectedCount && loads==1 && !allocations);
    }
    puts("PASS: original dialogue icon tiles read both sides of a mapped graphics-bank boundary and preserve repeated tile layout.");
    return 0;
}
