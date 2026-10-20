// SPDX-License-Identifier: GPL-3.0-or-later
#include "font_data.h"
#include "text.h"
#include "assert.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(v) do { if (!(v)) { fprintf(stderr,"Font check line %d: %s\n",__LINE__,#v); abort(); } } while (0)
static const u8 *member;
static u32 memberSize,memberId,liveAllocations,largestAllocation,openNarc;
static NARC archive;
void HGNativeAssertFail(const char *condition,const char *file,int line) { fprintf(stderr,"Rejected font %s:%d: %s\n",file,line,condition); exit(86); }
void *Heap_Alloc(enum HeapID heap,u32 size) {
    CHECK(heap==HEAP_ID_3); uint64_t *allocation=malloc((size_t)size+16); CHECK(allocation);
    allocation[0]=size; liveAllocations++; if (size>largestAllocation) largestAllocation=size;
    return allocation+2;
}
void Heap_Free(void *pointer) { CHECK(pointer && liveAllocations); liveAllocations--; free((uint64_t *)pointer-2); }
NARC *NARC_New(NarcId id,enum HeapID heap) { CHECK(id==NARC_graphic_font && heap==HEAP_ID_3 && !openNarc); openNarc=1; return &archive; }
void NARC_Delete(NARC *narc) { CHECK(narc==&archive && openNarc); openNarc=0; }
u32 NARC_GetMemberSize(NARC *narc,u32 id) { CHECK(narc==&archive && id==memberId); return memberSize; }
void NARC_GetMemberImageStartOffset(NARC *narc,u32 id,u32 *result) { CHECK(narc==&archive && id==memberId); *result=0; }
void NARC_ReadFromAbsolutePos(NARC *narc,u32 offset,u32 size,void *output) {
    CHECK(narc==&archive && openNarc && offset<=memberSize && size<=memberSize-offset);
    memcpy(output,member+offset,size);
}
void NARC_ReadFromMember(NARC *narc,u32 id,u32 offset,u32 size,void *output) { CHECK(id==memberId); NARC_ReadFromAbsolutePos(narc,offset,size,output); }
static u32 read32(FILE *input) { u32 result; CHECK(fread(&result,4,1,input)==1); return result; }
static void compare(struct FontData *font,u32 id,const u8 *expected) {
    GlyphInfo glyph; memset(&glyph,0xA5,sizeof(glyph)); TryLoadGlyph(font,(u16)id,&glyph);
    CHECK(!memcmp(&glyph,expected,sizeof(glyph)));
    CHECK(GetGlyphWidth(font,(u16)(id ? id : 428))==glyph.width);
}
int main(int argc,char **argv) {
    CHECK(argc==2); FILE *input=fopen(argv[1],"rb"); CHECK(input); CHECK(read32(input)==0x54464748);
    u32 records=read32(input),comparisons=0; GenerateFontHalfRowLookupTable(1,0,2);
    for (u32 record=0;record<records;record++) {
        memberId=read32(input); memberSize=read32(input); u32 glyphs=read32(input),base=read32(input);
        u8 *data=malloc(memberSize),*goldens=malloc((size_t)glyphs*sizeof(GlyphInfo)); CHECK(data && goldens);
        CHECK(fread(data,1,memberSize,input)==memberSize && fread(goldens,sizeof(GlyphInfo),glyphs,input)==glyphs); member=data;
        for (u32 initial=0;initial<2;initial++) {
            largestAllocation=0;
            struct FontData *font=FontData_New(NARC_graphic_font,memberId,(int)initial,FALSE,HEAP_ID_3); CHECK(font);
            for (u32 change=0;change<3;change++) {
                if (change) FontData_ModeSwitch(font,(int)((initial+change)&1),HEAP_ID_3);
                for (u32 glyph=1;glyph<=glyphs;glyph++) { compare(font,glyph,goldens+(glyph-1)*sizeof(GlyphInfo)); comparisons++; }
                compare(font,0,goldens+427*sizeof(GlyphInfo)); compare(font,glyphs+1,goldens+427*sizeof(GlyphInfo));
                u16 text[]={428,(u16)glyphs,0xFFFF};
                u32 first=goldens[427*sizeof(GlyphInfo)+128],last=goldens[(glyphs-1)*sizeof(GlyphInfo)+128];
                CHECK(GetStringWidth(font,text,1)==first+last+1);
            }
            CHECK(largestAllocation<=base*16u*data[14]*data[15]);
            FontData_Delete(font); CHECK(!liveAllocations && !openNarc);
        }
        free(data); free(goldens);
    }
    CHECK(fgetc(input)==EOF && !ferror(input)); fclose(input);
    printf("PASS: %u fonts, %u original glyph bitmap/width comparisons, direct/lazy switching, bounded fallback and unchanged preload ceilings.\n",records,comparisons);
    return 0;
}
