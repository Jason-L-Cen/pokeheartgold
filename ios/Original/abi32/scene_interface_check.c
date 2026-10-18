// SPDX-License-Identifier: GPL-3.0-or-later
// Keep the original font object's private cell records visible to this isolated check.
#include "../../../src/font_sprite.c"
#include "frontier/frontier_link_messages.h"
#include "pokeathlon/pokeathlon_course_internal.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(value) do { if (!(value)) return __LINE__; } while (0)
_Static_assert(sizeof(FontOAMSprite)==12,"Original font cell record");
_Static_assert(offsetof(PokeathlonCourseMonObject,animState)==0x34,"Original mon animation state offset");

// Probe-only output observers: no transport, glyph renderer or sprite animation backend is supplied.
static u32 sends,sendKind,sendValue;
static BOOL sendResult;
static BattleHallData *sentData;
static BOOL ObserveSend(BattleHallData *data,u32 kind,u32 value) {
    sends++; sentData=data; sendKind=kind; sendValue=value; return sendResult;
}
BOOL BattleHallLink_SendEmptyRecord(BattleHallData *data) { return ObserveSend(data,0,0); }
BOOL BattleHallLink_SendTrainerIds(BattleHallData *data) { return ObserveSend(data,1,0); }
BOOL BattleHallLink_SendSetIds(BattleHallData *data) { return ObserveSend(data,2,0); }
BOOL BattleHallLink_SendValue(BattleHallData *data,u16 value) { return ObserveSend(data,3,value); }
BOOL BattleHallLink_SendPartyMon(BattleHallData *data) { return ObserveSend(data,7,0); }
static Sprite *palSprites[4],*animatedSprite;
static u32 palCalls,animCalls;
static int palette,sequence;
void Sprite_SetPalOffset(Sprite *sprite,int value) {
    if (palCalls>=4) abort();
    palSprites[palCalls++]=sprite; palette=value;
}
void Sprite_TryChangeAnimSeq(Sprite *sprite,int seq) { animCalls++; animatedSprite=sprite; sequence=seq; }

u32 CheckOriginalSceneInterfaces(void) {
    static BattleHallData data;
    const u32 active[]={0,1,2,3,7},unused[]={4,5,6,8,0xFFFFFFFF};
    for (u32 i=0;i<sizeof(active)/sizeof(active[0]);i++) for (u32 status=0;status<2;status++) {
        sends=0; sendResult=status;
        CHECK(BattleHallData_SendLinkMessage(&data,active[i],0xABCD0123)==status);
        CHECK(sends==1 && sentData==&data && sendKind==active[i] && sendValue==(active[i]==3 ? 0x23 : 0));
    }
    for (u32 i=0;i<sizeof(unused)/sizeof(unused[0]);i++) {
        sends=0;
        CHECK(BattleHallData_SendLinkMessage(&data,unused[i],0xFFFFFFFF)==(u32)(uaddr)&data && sends==0);
        CHECK(BattleHallData_SendLinkMessage(NULL,unused[i],0)==0 && sends==0);
    }

    Sprite sprites[3]={{0}}; FontOAMSprite cells[3]={{0}};
    for (u32 i=0;i<3;i++) cells[i].sprite=&sprites[i];
    for (int count=0;count<=3;count++) {
        TextOBJ object={0}; object.sprites=cells; object.spriteCount=count;
        TextOBJ before=object; palCalls=0; palette=-1;
        TextOBJ_SetPalOffset(&object,13);
        CHECK(palCalls==(u32)count && !memcmp(&before,&object,sizeof(object)));
        for (int i=0;i<count;i++) CHECK(palSprites[i]==&sprites[i]);
        CHECK(!count || palette==13);
    }

    PokeathlonCourseMonGfx gfx={0}; gfx.sprite=&sprites[0];
    PokeathlonCourseMonObject object={0}; object.gfx=&gfx;
    const int sequences[]={-1,0,1,2,3,4,8};
    for (u32 state=0;state<7;state++) {
        PokeathlonCourseMonObject before=object; before.animState=state;
        animCalls=0; sequence=-1; animatedSprite=NULL;
        PokeathlonCourseMonObject_SetAnimState(&object,state);
        CHECK(object.animState==state && !memcmp(&before,&object,sizeof(object)));
        CHECK(animCalls==(state!=0));
        CHECK(state==0 || (sequence==sequences[state] && animatedSprite==&sprites[0]));
    }
    return 0;
}
