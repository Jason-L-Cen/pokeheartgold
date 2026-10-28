// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef HG_PRIVATE_POKEGEAR_MAP_CARD_QA_H
#define HG_PRIVATE_POKEGEAR_MAP_CARD_QA_H
#include "pokegear_qa.h"
#include "native_battle_ui.h"
enum { HG_PG_MAP_VERSION=1,HG_PG_MAP_START=0x200,HG_PG_MAP_FINISH=0x201,HG_PG_MAP_RUN_ATTEMPT_CAP=8 };
typedef struct HGPokegearMapCardQAState {
    HGNativePokegearQAState phone;
    uint32_t version,byteSize,revision,error,started,done;
    uint32_t mapId,x,z,facing,betweenSteps,moveState,playerMoveState,scene,cards,lastApp;
    uint32_t scriptActive,scriptId,scriptWait,scriptSerial;
    uint32_t parentEntries,parentExits,mapReadySeen;
    uint32_t battleLaunches,battleReturns,battleType,battleSpecial,battleOutcome,battleLive;
} HGPokegearMapCardQAState;
typedef struct HGPokegearMapRouteSegment {uint32_t key,x0,z0,x1,z1;} HGPokegearMapRouteSegment;
static const HGPokegearMapRouteSegment hgMapGuideRoute[]={
    {32,668,397,663,397},{128,663,397,663,398},{32,663,398,652,398},
    {128,652,398,652,408},{32,652,408,649,408},{128,649,408,649,409},
    {32,649,409,644,409},{128,644,409,644,411},{32,644,411,633,411},
    {64,633,411,633,402},{16,633,402,638,402},{64,638,402,638,398},
    {32,638,398,618,398},{64,618,398,618,396},{32,618,396,614,396},
    {64,614,396,614,394},{32,614,394,612,394},{64,612,394,612,393},
    {32,612,393,611,393},{64,611,393,611,392},{32,611,392,610,392},
    {64,610,392,610,391},{32,610,391,604,391},{128,604,391,604,400},
    {32,604,400,566,400}
};
static const HGPokegearMapRouteSegment hgMapCardRoute[]={
    {32,557,404,551,404},{64,551,404,551,396},{32,551,396,550,396},{64,550,396,550,385}
};
// Endpoint and segment membership use actual world coordinates, including the original westward ledge.
static inline uint32_t HGMapRouteKey(const HGPokegearMapRouteSegment *s,uint32_t x,uint32_t z) {
    if(!s || (x==s->x1 && z==s->z1))return 0;
    if(s->key==32 && z==s->z0 && z==s->z1 && x<=s->x0 && x>s->x1)return 32;
    if(s->key==16 && z==s->z0 && z==s->z1 && x>=s->x0 && x<s->x1)return 16;
    if(s->key==64 && x==s->x0 && x==s->x1 && z<=s->z0 && z>s->z1)return 64;
    if(s->key==128 && x==s->x0 && x==s->x1 && z>=s->z0 && z<s->z1)return 128;
    return 0;
}
static inline uint32_t HGMapStepValid(uint32_t key,uint32_t x,uint32_t z,uint32_t nextX,uint32_t nextZ) {
    if(x==nextX && z==nextZ)return 1; // Original turn or blocked step; host bounds retries.
    if(key==32 && z==400 && x==589 && nextX==587 && nextZ==400)return 1;
    return (key==32 && x && nextX==x-1 && nextZ==z) || (key==16 && x<UINT32_MAX && nextX==x+1 && nextZ==z)
        || (key==64 && z && nextZ==z-1 && nextX==x) || (key==128 && z<UINT32_MAX && nextZ==z+1 && nextX==x);
}
static inline uint32_t HGMapStationary(const HGPokegearMapCardQAState *s) {
    return s && s->phone.fieldReady==1 && s->betweenSteps==1 && s->moveState==0 && (s->playerMoveState==0 || s->playerMoveState==3);
}
static inline uint32_t HGMapTextAllowed(const HGPokegearMapCardQAState *s,uint32_t id,uint32_t serial) {
    return s && s->started==1 && !s->error && s->mapId==67 && s->scriptActive==1 && s->scriptId==id
        && ((id==2 && s->scene==0) || (id==3 && s->scene==1)) && (s->scriptWait==2 || s->scriptWait==3)
        && serial && s->scriptSerial==serial && !s->battleLive && !s->phone.ui.active;
}
static inline uint32_t HGMapRunReady(const HGNativeBattleUIState *ui,uint32_t attempts,uint32_t lastRevision) {
    return ui && ui->active==1 && ui->ready==1 && ui->mode==HG_BATTLE_UI_COMMAND && (ui->enabledMask&8)
        && attempts<HG_PG_MAP_RUN_ATTEMPT_CAP && (!attempts || ui->revision!=lastRevision);
}
#ifdef HG_PG_MR_EXTENSION
enum {HG_PG_MR_VERSION=1,HG_PG_MR_START=0x202,HG_PG_MR_FINISH=0x203};
typedef struct HGPokegearMrPokemonQAState {
    uint32_t version,byteSize,revision,error,started,done;
    uint32_t routeScene,houseScene,embeddedTower,apricornBox,mysteryEgg,pokedexEnabled,oakRegistered,partyHealed;
    uint32_t apricornScriptSeen,apricornCompletedSeen,houseScriptSeen,doorAnchorSeen;
} HGPokegearMrPokemonQAState;
static const HGPokegearMapRouteSegment hgMrApricornRoute[]={
    {64,550,385,550,374},{16,550,374,558,374},{64,558,374,558,354}
};
static const HGPokegearMapRouteSegment hgMrHouseRoute[]={
    {16,556,356,559,356},{64,559,356,559,333},{16,559,333,565,333},
    {64,565,333,565,323},{32,565,323,559,323},{64,559,323,559,307},
    {16,559,307,561,307},{64,561,307,561,299},{16,561,299,562,299},
    {64,562,299,562,295},{16,562,295,568,295}
};
static const HGPokegearMapRouteSegment hgMrDoorRoute[]={{64,568,295,568,294}};
static inline uint32_t HGMrMapAllowed(const HGPokegearMapCardQAState *s,const HGPokegearMrPokemonQAState *q) {
    if(!q->started)return (s->mapId==33 || s->mapId==67) && s->cards<=1 && s->scene<=2;
    return s->done && s->cards==1 && s->scene>=2 && s->scene<=3 && (s->mapId==67 || s->mapId==34 || s->mapId==143);
}
static inline uint32_t HGMrTextQualified(const HGPokegearMapCardQAState *s,const HGPokegearMrPokemonQAState *q,uint32_t id,uint32_t serial) {
    if(HGMapTextAllowed(s,id,serial))return 1;
    return q && q->started && !q->done && !q->error && s && s->started && s->done && !s->error
        && s->scriptActive && s->scriptId==id && (s->scriptWait==2 || s->scriptWait==3)
        && serial && s->scriptSerial==serial && !s->battleLive && !s->phone.ui.active && q->embeddedTower!=1
        && ((s->mapId==34 && id==3 && q->routeScene==0 && q->houseScene==0)
            || (s->mapId==143 && id==2 && q->houseScene==0 && q->apricornCompletedSeen && q->doorAnchorSeen));
}
static inline uint32_t HGMrEndpoint(const HGPokegearMapCardQAState *s,const HGPokegearMrPokemonQAState *q) {
    return s && q && q->started && !q->error && !s->error && s->started && s->done && HGMapStationary(s) && !s->scriptActive
        && !s->battleLive && !s->phone.ui.active && !s->phone.ringActive && s->mapId==143 && s->x==5 && s->z==6 && s->facing==2
        && s->cards==1 && s->scene==3 && q->routeScene==2 && q->houseScene==1 && q->embeddedTower!=1
        && q->apricornBox==1 && q->mysteryEgg==1 && q->pokedexEnabled==1 && q->oakRegistered==1 && q->partyHealed==1
        && q->apricornScriptSeen && q->apricornCompletedSeen && q->houseScriptSeen && q->doorAnchorSeen;
}
const HGPokegearMrPokemonQAState *HGNativeMrPokemonQAGetState(void);
#endif
#ifdef HG_POKEGEAR_QA_GUEST
struct FieldSystem;
void PokegearQAFieldObserved(struct FieldSystem *field);
void PokegearQAParentObserved(const struct PokegearAppData *owner,int phase);
void PokegearQAParentExitObserved(const struct PokegearAppData *owner);
#endif
// The original private two export names/signatures are retained, with the Phone QA prefix.
const HGNativePokegearQAState *HGNativePokegearQAGetState(void);
uint32_t HGNativePokegearQARequest(uint32_t revision,uint32_t request);
#endif
