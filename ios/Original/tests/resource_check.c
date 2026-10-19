// SPDX-License-Identifier: GPL-3.0-or-later
#include "filesystem.h"
#include "wild_encounter.h"
#include "catching_show.h"
#include "billboard_internal.h"
#include "gf_gfx_loader.h"
// Exercise the actual private sprite-resource loading route, without resource-manager/GPU setup.
#include "../../../src/sprite_resource.c"
#include "../NativeCompression.h"
#include "../NativeFile.h"
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(v) do { if (!(v)) { fprintf(stderr,"Resource check line %d: %s\n",__LINE__,#v); abort(); } } while (0)
static void put16(u8 *data,u32 offset,u16 value) { data[offset]=value; data[offset+1]=value>>8; }
static void put32(u8 *data,u32 offset,u32 value) { for (u32 i=0;i<4;i++) data[offset+i]=value>>(8*i); }
void CheckNativeGfxLoaderMalformed(const char *contentRoot) {
    const u8 members[][12]={
        {0x10,1}, {0x10,3,0,0,0x80,0,0}, {0x11,0x12,1,0,0x40,'A',0x10,0,0}, {0x10,255,255,255},
        {0x10,9,0,0,0x10,'A','B','C',0x30,2}, {0x11,4,0,0,0x40,'A',0x20,0}, {0x10,9,0,0,0x10,'A','B','C',0x30}
    };
    const u32 sizes[]={2,7,9,4,10,8,9};
    enum { count=sizeof(sizes)/sizeof(sizes[0]), fatSize=12+count*8, dataStart=16+fatSize+16+8 };
    u8 archive[512]={0};
    memcpy(archive,"NARC",4); put16(archive,4,0xFFFE); put16(archive,6,0x0100); put16(archive,12,16); put16(archive,14,3);
    memcpy(archive+16,"BTAF",4); put32(archive,20,fatSize); put16(archive,24,count);
    memcpy(archive+16+fatSize,"BTNF",4); put32(archive,20+fatSize,16); put32(archive,24+fatSize,4); put16(archive,30+fatSize,1);
    memcpy(archive+dataStart-8,"GMIF",4);
    u32 length=0;
    for (u32 i=0;i<count;i++) {
        put32(archive,28+8*i,length); put32(archive,32+8*i,length+sizes[i]);
        memcpy(archive+dataStart+length,members[i],sizes[i]); length=(length+sizes[i]+3)&~3u;
    }
    put32(archive,8,dataStart+length); put32(archive,dataStart-4,8+length);
    char folder[]="/tmp/heartgold-gfx-XXXXXX"; CHECK(mkdtemp(folder));
    char path[256]; snprintf(path,sizeof(path),"%s/a",folder); CHECK(!mkdir(path,0700));
    snprintf(path,sizeof(path),"%s/a/1",folder); CHECK(!mkdir(path,0700));
    snprintf(path,sizeof(path),"%s/a/1/6",folder); CHECK(!mkdir(path,0700));
    snprintf(path,sizeof(path),"%s/a/1/6/5",folder);
    FILE *file=fopen(path,"wb"); CHECK(file && fwrite(archive,1,dataStart+length,file)==dataStart+length && !fclose(file));
    CHECK(HGNativeFSSetRoot(folder));
    NARC *narc=NARC_New(NARC_data_sbox_gra,HEAP_ID_DEFAULT); CHECK(narc && NARC_GetFileCount(narc)==count);
    for (u32 i=0;i<count;i++) for (BOOL atEnd=FALSE;atEnd<=TRUE;atEnd++) {
        u32 freeBefore=HeapExp_FndGetTotalFreeSize(HEAP_ID_3);
        for (u32 route=0;route<3;route++) {
            u32 size=123;
            void *data;
            if (route==2) data=loadResourceFromNarc(narc,i,TRUE,HEAP_ID_3,atEnd);
            else data=route ? GfGfxLoader_LoadFromOpenNarc_GetSizeOut(narc,i,TRUE,HEAP_ID_3,atEnd,&size) :
                GfGfxLoader_LoadFromNarc_GetSizeOut(NARC_data_sbox_gra,i,TRUE,HEAP_ID_3,atEnd,&size);
            if (i==4 || i==5) {
                u32 expectedSize=i==4 ? 9 : 4;
                CHECK(data && (route==2 || size==expectedSize) && !memcmp(data,i==4 ? "ABCABCABC" : "AAAA",expectedSize)); Heap_Free(data);
            } else CHECK(!data && (route==2 || !size));
        }
        CHECK(HeapExp_FndGetTotalFreeSize(HEAP_ID_3)==freeBefore);
    }
    NARC_Delete(narc); CHECK(HGNativeFSSetRoot(contentRoot)); CHECK(!unlink(path));
    snprintf(path,sizeof(path),"%s/a/1/6",folder); CHECK(!rmdir(path));
    snprintf(path,sizeof(path),"%s/a/1",folder); CHECK(!rmdir(path));
    snprintf(path,sizeof(path),"%s/a",folder); CHECK(!rmdir(path)); CHECK(!rmdir(folder));
    puts("PASS: original graphics/sprite loaders reject short/truncated/invalid-distance/oversized compressed members without leaking heaps.");
}
static void signature(NarcId id, u32 member, const char *expected) {
    u32 size = GetNarcMemberSizeByIdPair(id,member); CHECK(size >= 4);
    u8 *data = malloc(size); CHECK(data); ReadWholeNarcMemberByIdPair(data,id,member);
    BOOL compressed=data[0] == 0x10 || data[0] == 0x11;
    if (compressed) {
        u32 length = data[1] | (data[2]<<8) | (data[3]<<16); CHECK(length >= 4 && length < 1048576);
        u8 *decoded = malloc(length); CHECK(decoded && HGNativeUncompressLZ(data,size,decoded,length));
        free(data); data = decoded; size=length;
    }
    CHECK(memcmp(data,expected,4)==0);
    NARC *narc=NARC_New(id,HEAP_ID_DEFAULT); CHECK(narc);
    for (BOOL atEnd=FALSE;atEnd<=TRUE;atEnd++) {
        u32 freeBefore=HeapExp_FndGetTotalFreeSize(HEAP_ID_3), loadedSize=0;
        void *loaded=GfGfxLoader_LoadFromNarc_GetSizeOut(id,member,compressed,HEAP_ID_3,atEnd,&loadedSize);
        CHECK(loaded && loadedSize==size && !memcmp(loaded,data,size)); Heap_Free(loaded);
        loaded=GfGfxLoader_LoadFromNarc(id,member,compressed,HEAP_ID_3,atEnd);
        CHECK(loaded && !memcmp(loaded,data,size)); Heap_Free(loaded);
        loaded=GfGfxLoader_LoadFromOpenNarc_GetSizeOut(narc,member,compressed,HEAP_ID_3,atEnd,&loadedSize);
        CHECK(loaded && loadedSize==size && !memcmp(loaded,data,size)); Heap_Free(loaded);
        loaded=GfGfxLoader_LoadFromOpenNarc(narc,member,compressed,HEAP_ID_3,atEnd);
        CHECK(loaded && !memcmp(loaded,data,size)); Heap_Free(loaded);
        loaded=loadResourceFromNarc(narc,member,compressed,HEAP_ID_3,atEnd);
        CHECK(loaded && !memcmp(loaded,data,size)); Heap_Free(loaded);
        if (!compressed) {
            loadedSize=123;
            CHECK(!GfGfxLoader_LoadFromNarc_GetSizeOut(id,member,TRUE,HEAP_ID_3,atEnd,&loadedSize) && !loadedSize);
            loadedSize=123;
            CHECK(!GfGfxLoader_LoadFromOpenNarc_GetSizeOut(narc,member,TRUE,HEAP_ID_3,atEnd,&loadedSize) && !loadedSize);
            CHECK(!loadResourceFromNarc(narc,member,TRUE,HEAP_ID_3,atEnd));
        }
        CHECK(HeapExp_FndGetTotalFreeSize(HEAP_ID_3)==freeBefore);
    }
    NARC_Delete(narc); free(data);
}
void CheckNativeSourceResources(void) {
    NARC *gold=NARC_New(NARC_fielddata_encountdata_g_enc_data,HEAP_ID_DEFAULT);
    NARC *silver=NARC_New(NARC_fielddata_encountdata_s_enc_data,HEAP_ID_DEFAULT);
    CHECK(gold && silver && NARC_GetFileCount(gold)==142 && NARC_GetFileCount(silver)==142);
    BOOL different=FALSE;
    for (unsigned i=0;i<142;i++) {
        EncounterData a,b;
        CHECK(NARC_GetMemberSize(gold,i)==sizeof(a) && NARC_GetMemberSize(silver,i)==sizeof(b));
        NARC_ReadWholeMember(gold,i,&a); NARC_ReadWholeMember(silver,i,&b);
        if (memcmp(&a,&b,sizeof(a))) different=TRUE;
    }
    CHECK(different); NARC_Delete(gold); NARC_Delete(silver);
    NARC *park=NARC_New(NARC_arc_ppark,HEAP_ID_DEFAULT);
    CHECK(park && NARC_GetFileCount(park)==1 && NARC_GetMemberSize(park,0)==493*sizeof(struct PalParkMonsData));
    NARC_Delete(park);
    const NarcId archives[]={NARC_data_sbox_gra,NARC_graphic_camera_viewfinder,NARC_a_1_5_0,NARC_poketool_pokegra_otherpoke};
    const u32 counts[]={2,3,228,261};
    for (unsigned i=0;i<4;i++) {
        NARC *narc=NARC_New(archives[i],HEAP_ID_DEFAULT); CHECK(narc && NARC_GetFileCount(narc)==counts[i]); NARC_Delete(narc);
    }
    signature(NARC_data_sbox_gra,0,"RLCN"); signature(NARC_data_sbox_gra,1,"RGCN");
    signature(NARC_graphic_camera_viewfinder,1,"RGCN"); signature(NARC_graphic_camera_viewfinder,2,"RCSN");
    for (unsigned i=0;i<228;i++) signature(NARC_a_1_5_0,i,i%3==0 ? "RLCN" : i%3==1 ? "RGCN" : "RCSN");
    signature(NARC_poketool_pokegra_otherpoke,259,"RGCN"); signature(NARC_poketool_pokegra_otherpoke,260,"RLCN");
    // Members selected by field_tall_grass.c and field_ground_dust.c, retained from the supplied ROM.
    const struct {u32 member;u16 frames[4];u8 texture[4],palette[4];} sequences[]={
        {0x8C,{3,4,11,12},{0,1,2,3},{0,0,0,0}},
        {0x91,{2,3,8,9},{0,1,2,3},{0,1,2,3}}
    };
    NARC *effects=NARC_New(NARC_a_1_0_3,HEAP_ID_DEFAULT); CHECK(effects);
    for (unsigned i=0;i<2;i++) {
        u32 raw[5]; CHECK(NARC_GetMemberSize(effects,sequences[i].member)==sizeof(raw));
        NARC_ReadWholeMember(effects,sequences[i].member,raw);
        CHECK(raw[0]==4);
        BillboardGfxSequence sequence; BillboardGfxSequence_Init(raw,&sequence); CHECK(sequence.seqCount==4);
        for (unsigned j=0;j<4;j++) {
            BillboardTexPlttIndex entry=BillboardGfxSequence_GetTexPlttIndex(&sequence,sequences[i].frames[j]);
            CHECK(entry.textureIdx==sequences[i].texture[j] && entry.plttIdx==sequences[i].palette[j]);
        }
    }
    NARC_Delete(effects);
    puts("PASS: original grass/dust texture and palette sequences through original NARC reads and C animation decoding.");
    puts("PASS: generated original encounter versions, Pal Park records, ordered photo/preview graphics and alternate sprites through original NARC reads.");
    puts("PASS: original graphics loaders, compressed/raw member bytes, heap-end allocation, size results and failed-load cleanup.");
}
