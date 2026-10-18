// SPDX-License-Identifier: GPL-3.0-or-later
#include "filesystem.h"
#include "wild_encounter.h"
#include "catching_show.h"
#include "../NativeCompression.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(v) do { if (!(v)) { fprintf(stderr,"Resource check line %d: %s\n",__LINE__,#v); abort(); } } while (0)
static void signature(NarcId id, u32 member, const char *expected) {
    u32 size = GetNarcMemberSizeByIdPair(id,member); CHECK(size >= 4);
    u8 *data = malloc(size); CHECK(data); ReadWholeNarcMemberByIdPair(data,id,member);
    if (data[0] == 0x10 || data[0] == 0x11) {
        u32 length = data[1] | (data[2]<<8) | (data[3]<<16); CHECK(length >= 4 && length < 1048576);
        u8 *decoded = malloc(length); CHECK(decoded && HGNativeUncompressLZ(data,size,decoded,length));
        free(data); data = decoded;
    }
    CHECK(memcmp(data,expected,4)==0); free(data);
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
    puts("PASS: generated original encounter versions, Pal Park records, ordered photo/preview graphics and alternate sprites through original NARC reads.");
}
