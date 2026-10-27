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
