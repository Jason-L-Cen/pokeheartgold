#!/usr/bin/env python3
"""Authentic Map Card case: source checks by default; explicit Full+save opts into one runtime."""
import argparse
import hashlib
import importlib.util
import json
import os
from pathlib import Path
import shlex
import shutil
import struct
import subprocess
import sys
sys.dont_write_bytecode=True
ROOT=Path(__file__).resolve().parents[2]
QA_PATHS=('ios/tests/pokegear_map_card_qa.h','ios/tests/pokegear_map_card_guest.inc',
          'ios/Original/abi32/pokegear_map_card_scenario_check.c','ios/tools/check_native_pokegear_map_card.py')
LINK_WRAPS=('Battle_LaunchApp','BattleSetup_Delete')
C_CHECK=r'''
#include "global.h"
#include "pokegear_map_card_qa.h"
#include "native_field_menu_ui.h"
#include "NativeImports.h"
#include "battle/battle_setup.h"
#include "constants/battle.h"
#include "field_system.h"
#include "script.h"
#include <assert.h>
#include <stddef.h>
#include <stdio.h>
typedef struct {struct w2c_env *env;} Capture;
static Capture *pgCapture;
static HGPokegearMapCardQAState mapFinal;
static HGNativePokegearQAState pgFinal;
static HGNativeFieldMenuUIState pgMenu;
#ifdef HG_PG_MR_EXTENSION
static HGPokegearMrPokemonQAState mrFinal;
#define HGMapTextAllowed(s,i,r) HGMrTextQualified((s),&mrFinal,(i),(r))
#endif
static u32 mapLease,mapReads,mapReleaseReads,mapOwner,mapGen,mapRev,mapLCD,mapLeaseScript,mapLeaseSerial;
static u32 mapMovePending,mapMoveX,mapMoveZ;
static u32 inputReads,inputKeys,inputPressed,inputCalls,inputLCD;
#define PG_CHECK assert
static u32 pgTouchEngine(void){return inputLCD;}
static void mapTrace(const char *event,u32 key,u32 touch,u32 x,u32 y){(void)event;(void)key;(void)touch;(void)x;(void)y;}
int HGNativeSetInput(struct w2c_env *env,uint32_t keys,int folded,uint32_t x,uint32_t y,int pressed){assert(env==pgCapture->env && !folded && !x && !y);inputKeys=keys;inputPressed=pressed;++inputCalls;return 1;}
uint32_t HGNativeInputPadReads(struct w2c_env *env){assert(env==pgCapture->env);return inputReads;}
#include "actual_map_settle.inc"
static HGPokegearMapCardQAState sPGMap;
#ifdef HG_PG_MR_EXTENSION
static HGPokegearMrPokemonQAState sPGMr;
#include "actual_mr_running.inc"
#include "actual_mr_context.inc"
static BOOL mrWaitTag(ScriptContext *ctx){return ctx!=NULL;}
#endif
static u32 sPGMapPending;
#ifdef HG_PG_MR_EXTENSION
static u32 sPGMrPending;
#endif
#include "actual_map_request.inc"
static const BattleSetup *sPGMapBattle;
static FieldSystem field;
static FieldSystem *sPGQAField=&field;
static unsigned launchCalls,deleteCalls;
HGNativeBattleUIState gNativeBattleUIState;
static BOOL sNativeBattleReady;
static int sNativeBattleChoice=-1;
const HGNativeBattleUIState *HGNativeBattleUIGetState(void){return &gNativeBattleUIState;}
#include "actual_native_battle_choose.inc"
void __real_Battle_LaunchApp(FieldSystem *f,BattleSetup *setup){assert(f==&field && setup);++launchCalls;}
void __real_BattleSetup_Delete(BattleSetup *setup){assert(setup && setup!=sPGMapBattle);++deleteCalls;}
#include "actual_map_battle.inc"
static void route(const HGPokegearMapRouteSegment *r,u32 count,u32 grid,u32 physical){
 u32 distance=0,steps=0,x=r[0].x0,z=r[0].z0;
 for(u32 i=0;i<count;i++){
  assert(x==r[i].x0 && z==r[i].z0 && !HGMapRouteKey(&r[i],UINT32_MAX,UINT32_MAX));
  while(x!=r[i].x1 || z!=r[i].z1){u32 key=HGMapRouteKey(&r[i],x,z),nx=x,nz=z;assert(key);
   if(key==32)nx-=x==589 && z==400 ? 2 : 1;else if(key==16)nx++;else if(key==64)nz--;else if(key==128)nz++;else assert(0);
   assert(HGMapStepValid(key,x,z,nx,nz));distance+=(nx>x ? nx-x : x-nx)+(nz>z ? nz-z : z-nz);steps++;x=nx;z=nz;
  }
  assert(!HGMapRouteKey(&r[i],x,z));
 }
 assert(distance==grid && steps==physical);
}
int main(void){
 route(hgMapGuideRoute,sizeof(hgMapGuideRoute)/sizeof(*hgMapGuideRoute),155,154);
 route(hgMapCardRoute,sizeof(hgMapCardRoute)/sizeof(*hgMapCardRoute),26,26);
 assert(HGMapStepValid(32,589,400,587,400) && !HGMapStepValid(16,587,400,589,400));
 assert(!HGMapStepValid(32,588,400,586,400) && !HGMapStepValid(32,589,399,587,399));
 assert(!HGMapStepValid(32,668,397,660,397) && !HGMapStepValid(32,668,397,668,398));
 mapFinal=(HGPokegearMapCardQAState){.started=1,.mapId=67,.scene=0,.scriptActive=1,.scriptId=2,.scriptWait=2,.scriptSerial=7};
 assert(HGMapTextAllowed(&mapFinal,2,7) && !HGMapTextAllowed(&mapFinal,2,6) && !HGMapTextAllowed(&mapFinal,1,7));
 mapFinal.battleLive=1;assert(!HGMapTextAllowed(&mapFinal,2,7));mapFinal.battleLive=0;
 mapFinal.scriptWait=3;assert(HGMapTextAllowed(&mapFinal,2,7));mapFinal.scriptActive=0;assert(!HGMapTextAllowed(&mapFinal,2,7));
 mapFinal.scriptActive=1;mapFinal.scene=1;mapFinal.scriptId=3;assert(HGMapTextAllowed(&mapFinal,3,7));
 mapFinal.scene=2;assert(!HGMapTextAllowed(&mapFinal,3,7));
 struct w2c_env env={0};Capture capture={&env};pgCapture=&capture;
 pgFinal.fieldReady=1;mapFinal=(HGPokegearMapCardQAState){0};mapOwner=0;mapLease=1;mapReads=inputReads=100;
 inputReads=103;assert(!mapSettle() && !inputCalls);inputReads=104;assert(!mapSettle() && inputCalls==1 && !inputKeys && !inputPressed);
 inputReads=105;assert(!mapSettle());inputReads=106;assert(mapSettle() && !mapLease);
 pgFinal.ui=(HGNativePokegearUIState){.active=1,.ready=1,.viewActive=1,.ownerKind=1,.parentPhase=7,.viewKind=2,.phase=1,.app=2,.registeredCards=1,.ownerGeneration=5,.revision=9};
 mapOwner=1;mapGen=5;mapRev=9;mapLCD=0;mapLease=1;mapReads=inputReads=200;
 assert(!mapSettle() && inputCalls==1);pgFinal.ui.revision++;assert(!mapSettle() && mapLease==2 && inputCalls==2);
 mapOwner=3;mapLease=1;mapReads=inputReads=300;mapLeaseScript=2;mapLeaseSerial=7;
 mapFinal=(HGPokegearMapCardQAState){.started=1,.mapId=67,.scene=0,.scriptActive=1,.scriptId=2,.scriptWait=2,.scriptSerial=7};
 assert(!mapSettle() && inputCalls==2);mapFinal.scriptSerial++;assert(!mapSettle() && mapLease==2 && inputCalls==3);
 mapLease=1;mapOwner=1;mapRev=10;mapReads=inputReads=400;inputLCD=1;assert(!mapSettle() && mapLease==2 && inputCalls==4);
 BattleSetup setup={0},original;sPGMap.started=1;setup.winFlag=BATTLE_OUTCOME_PLAYER_FLED;original=setup;
 __wrap_Battle_LaunchApp(&field,&setup);assert(launchCalls==1 && sPGMap.battleLive && sPGMapBattle==&setup && !memcmp(&setup,&original,sizeof(setup)));
 __wrap_BattleSetup_Delete(&setup);assert(deleteCalls==1 && !sPGMap.battleLive && !sPGMapBattle && sPGMap.battleReturns==1 && !sPGMap.error && !memcmp(&setup,&original,sizeof(setup)));
 setup.battleType=1;original=setup;__wrap_Battle_LaunchApp(&field,&setup);assert(launchCalls==2 && sPGMap.error && !sPGMapBattle && !memcmp(&setup,&original,sizeof(setup)));
 __wrap_BattleSetup_Delete(&setup);assert(deleteCalls==2 && !memcmp(&setup,&original,sizeof(setup)));
 mapLease=1;mapOwner=0;mapReads=500;inputReads=501;mapMovePending=1;mapMoveX=668;mapMoveZ=397;
 mapFinal=(HGPokegearMapCardQAState){.x=667,.z=397};assert(!mapSettle() && mapLease==2 && inputCalls==5);
 u32 attempts=0,lastRevision=0;
 gNativeBattleUIState=(HGNativeBattleUIState){.active=1,.ready=1,.mode=HG_BATTLE_UI_COMMAND,.enabledMask=8,.revision=1};
 assert(HGMapRunReady(&gNativeBattleUIState,attempts,lastRevision));assert(!HGNativeBattleUIChoose(0,3));
 assert(HGNativeBattleUIChoose(1,3) && !HGNativeBattleUIChoose(1,3));attempts=1;lastRevision=1;
 assert(!HGMapRunReady(&gNativeBattleUIState,attempts,lastRevision));
 gNativeBattleUIState.ready=1;gNativeBattleUIState.revision=2;sNativeBattleReady=TRUE;sNativeBattleChoice=-1;
 assert(HGMapRunReady(&gNativeBattleUIState,attempts,lastRevision) && !HGNativeBattleUIChoose(1,3));
 assert(HGNativeBattleUIChoose(2,3) && !HGNativeBattleUIChoose(2,3));attempts=2;lastRevision=2;
 gNativeBattleUIState.ready=1;sNativeBattleReady=TRUE;sNativeBattleChoice=-1;
 assert(!HGMapRunReady(&gNativeBattleUIState,attempts,lastRevision));
 gNativeBattleUIState.revision=3;gNativeBattleUIState.mode=HG_BATTLE_UI_MOVES;
 assert(!HGMapRunReady(&gNativeBattleUIState,attempts,lastRevision) && sNativeBattleChoice==-1);
 gNativeBattleUIState.mode=HG_BATTLE_UI_COMMAND;assert(!HGMapRunReady(&gNativeBattleUIState,HG_PG_MAP_RUN_ATTEMPT_CAP,lastRevision));
#ifdef HG_PG_MR_EXTENSION
 route(hgMrApricornRoute,sizeof(hgMrApricornRoute)/sizeof(*hgMrApricornRoute),39,39);
 route(hgMrHouseRoute,sizeof(hgMrHouseRoute)/sizeof(*hgMrHouseRoute),85,85);
 _Static_assert(sizeof(HGPokegearMrPokemonQAState)==72,"Separate private Mr packet");
 Location location={.mapId=143};field.location=&location;
 ScriptContext parent={.id=0,.mode=SCRIPT_MODE_NATIVE,.native_ptr=mrWaitTag,.fieldSystem=&field,.taskman=field.taskman};
 ScriptContext child={.id=1,.mode=SCRIPT_MODE_NATIVE,.native_ptr=mrWaitTag,.fieldSystem=&field,.taskman=field.taskman};
 ScriptEnvironment context={.activeScriptContextCount=1,.activeScriptNumber=2,.scriptContexts={&parent,NULL,NULL}};
 assert(PGMrTextContext(&field,&context,&parent)==&parent);
 context.scriptContexts[1]=&child;assert(!PGMrTextContext(&field,&context,&parent));context.scriptContexts[1]=NULL;
 context.scriptContexts[2]=&child;assert(!PGMrTextContext(&field,&context,&parent));context.scriptContexts[2]=NULL;
 context.activeScriptContextCount=2;context.scriptContexts[1]=&child;context.stdScriptWaitFlags=1;sPGMr=(HGPokegearMrPokemonQAState){.started=1,.apricornCompletedSeen=1,.doorAnchorSeen=1};
 assert(PGMrTextContext(&field,&context,&parent)==&child);
 context.scriptContexts[2]=&child;assert(!PGMrTextContext(&field,&context,&parent));context.scriptContexts[2]=NULL;
 child.id=2;assert(!PGMrTextContext(&field,&context,&parent));child.id=1;
 child.fieldSystem=NULL;assert(!PGMrTextContext(&field,&context,&parent));child.fieldSystem=&field;
 context.stdScriptWaitFlags=0;assert(!PGMrTextContext(&field,&context,&parent));context.stdScriptWaitFlags=1;
 context.activeScriptContextCount=3;assert(!PGMrTextContext(&field,&context,&parent));context.activeScriptContextCount=2;
 context.activeScriptNumber=2033;assert(!PGMrTextContext(&field,&context,&parent));context.activeScriptNumber=2;
 child.mode=SCRIPT_MODE_STOPPED;assert(!PGMrTextContext(&field,&context,&parent));child.mode=SCRIPT_MODE_NATIVE;
 sPGMr=(HGPokegearMrPokemonQAState){0};
 HGPokegearMrPokemonQAState q={.started=1,.routeScene=0};
 HGPokegearMapCardQAState m={.started=1,.done=1,.mapId=34,.scene=2,.cards=1,.scriptActive=1,.scriptId=3,.scriptWait=2,.scriptSerial=10};
 assert(HGMrMapAllowed(&m,&q) && HGMrTextQualified(&m,&q,3,10));
 assert(!HGMrTextQualified(&m,&q,3,9));m.scriptWait=1;assert(!HGMrTextQualified(&m,&q,3,10));m.scriptWait=3;
 assert(HGMrTextQualified(&m,&q,3,10));q.routeScene=1;assert(!HGMrTextQualified(&m,&q,3,10));
 q.houseScene=0;q.apricornCompletedSeen=1;q.doorAnchorSeen=1;m.mapId=143;m.scriptId=2;
 assert(HGMrTextQualified(&m,&q,2,10));q.houseScene=1;assert(!HGMrTextQualified(&m,&q,2,10));
 m.scene=3;assert(HGMrMapAllowed(&m,&q));q.started=0;assert(!HGMrMapAllowed(&m,&q));q.started=1;
 q=(HGPokegearMrPokemonQAState){.started=1,.routeScene=2,.houseScene=1,.apricornBox=1,.mysteryEgg=1,.pokedexEnabled=1,.oakRegistered=1,.partyHealed=1,.apricornScriptSeen=1,.apricornCompletedSeen=1,.houseScriptSeen=1,.doorAnchorSeen=1};
 m=(HGPokegearMapCardQAState){.started=1,.done=1,.mapId=143,.x=5,.z=6,.facing=2,.scene=3,.cards=1,.betweenSteps=1,.phone.fieldReady=1};
 assert(HGMrEndpoint(&m,&q));q.routeScene=1;assert(!HGMrEndpoint(&m,&q));q.routeScene=2;
 q.houseScene=0;assert(!HGMrEndpoint(&m,&q));q.houseScene=1;q.mysteryEgg=0;assert(!HGMrEndpoint(&m,&q));q.mysteryEgg=1;
 m.scriptActive=1;assert(!HGMrEndpoint(&m,&q));m.scriptActive=0;m.mapId=34;assert(!HGMrEndpoint(&m,&q));m.mapId=143;
 sPGMap=m;sPGMap.revision=20;sPGMr=(HGPokegearMrPokemonQAState){0};
 assert(!HGNativePokegearQARequest(20,HG_PG_MR_START));sPGMap.mapId=67;sPGMap.x=550;sPGMap.z=385;sPGMap.scene=2;
 assert(!HGNativePokegearQARequest(19,HG_PG_MR_START));assert(HGNativePokegearQARequest(20,HG_PG_MR_START));
 assert(!HGNativePokegearQARequest(20,HG_PG_MR_START));sPGMrPending=0;
 setup=(BattleSetup){.winFlag=BATTLE_OUTCOME_PLAYER_FLED};original=setup;sPGMap.error=0;sPGMr.started=1;
 __wrap_Battle_LaunchApp(&field,&setup);assert(launchCalls==3 && sPGMap.battleLive && !memcmp(&setup,&original,sizeof(setup)));
 __wrap_BattleSetup_Delete(&setup);assert(deleteCalls==3 && !sPGMap.battleLive && !sPGMap.error);
 sPGMr.done=1;__wrap_Battle_LaunchApp(&field,&setup);assert(launchCalls==4 && !sPGMap.battleLive && !sPGMapBattle);
 mapFinal=(HGPokegearMapCardQAState){.started=1,.done=1,.mapId=34,.scene=2,.cards=1,.scriptActive=1,.scriptId=3,.scriptWait=2,.scriptSerial=40};
 mrFinal=(HGPokegearMrPokemonQAState){.started=1};mapOwner=3;mapLCD=inputLCD=0;mapLease=1;mapReads=inputReads=900;mapLeaseScript=3;mapLeaseSerial=40;
 u32 priorCalls=inputCalls;assert(!mapSettle() && inputCalls==priorCalls);inputReads=904;
 assert(!mapSettle() && inputCalls==priorCalls+1 && mapLease==2);inputReads=905;assert(!mapSettle());inputReads=906;assert(mapSettle());
 mapLease=1;mapReads=inputReads=910;mapFinal.scriptSerial=41;assert(!mapSettle() && mapLease==2);
 puts("PASS Mr 124-tile route, qualified waits/4+2 leases, scene transitions, gift endpoint and original RUN observer continuation");
#else
 sPGMap=(HGPokegearMapCardQAState){.revision=1,.started=1,.done=1,.mapId=67,.x=550,.z=385,.scene=2,.cards=1,.betweenSteps=1,.phone.fieldReady=1};
 assert(!HGNativePokegearQARequest(1,0x202));
#endif
 puts("PASS actual original Choose: COMMAND rev1 -> pending -> new COMMAND rev2 retries; stale/duplicate/unknown mode/cap rejected");
 printf("MAP_LAYOUT {\"stateBytes\":%zu,\"prefixBytes\":%zu,\"mapOffset\":%zu,\"uiOffset\":%zu}\n",sizeof(HGPokegearMapCardQAState),sizeof(HGNativePokegearQAState),offsetof(HGPokegearMapCardQAState,version),offsetof(HGNativePokegearQAState,ui));
 puts("PASS actual route/one-way ledge/qualified waits/stale owner+serial/LCD/4-read press+2-read release");return 0;
}
'''
def sha(path):return hashlib.sha256(Path(path).read_bytes()).hexdigest()
def require(value,message):
    if not value:raise RuntimeError(message)
def load(path):
    spec=importlib.util.spec_from_file_location('map_original_pokegear_qa',path)
    module=importlib.util.module_from_spec(spec);spec.loader.exec_module(module);return module
def source_copies(root,here,destination,base):
    originals=base.source_copies(root,root,destination)
    field=destination/base.COPIES[0]
    previous=field.read_text()
    text=base.replace_once(previous,'#include "pokegear_qa_guest.inc"','#include "pokegear_map_card_guest.inc"')
    field.write_text(text)
    require(text.replace('#include "pokegear_map_card_guest.inc"','#include "pokegear_qa_guest.inc"',1)==
            previous,'Field include replacement drift')
    return originals
def evidence_passed(row,layout):
    fixed=dict(status='passed_original_map_card',stage=11,done=1,qaBytes=layout['stateBytes'],cards=1,scene=2,mapReadySeen=1,runAttemptCap=8,
               parentEntries=1,parentExits=1,lastApp=2,menuCloses=1,fieldReady=1,menuActive=0,privateSaveWrites=False,resetQuiescent=True)
    if not isinstance(row,dict) or any(type(row.get(k)) is not type(v) or row[k]!=v for k,v in fixed.items()):return False
    n=row.get('battleLaunches');attempts=row.get('runAttempts')
    return type(n) is int and 0<=n<=32 and type(attempts) is int and all(type(row.get(k)) is int for k in ('runs','battleReturns','outcome')) and row['battleReturns']==n and row['runs']==attempts and ((not n and not attempts) or (n>0 and n<=attempts<=8*n and row['outcome']==5)) and type(row.get('steps')) is int and 178<=row['steps']<=180
def evidence_self_check(layout):
    good=dict(status='passed_original_map_card',stage=11,done=1,qaBytes=layout['stateBytes'],cards=1,scene=2,mapReadySeen=1,parentEntries=1,parentExits=1,lastApp=2,menuCloses=1,fieldReady=1,menuActive=0,privateSaveWrites=False,resetQuiescent=True,runs=2,runAttempts=2,runAttemptCap=8,battleLaunches=1,battleReturns=1,outcome=5,steps=180)
    require(evidence_passed(good,layout),'Genuine Map evidence contract rejected')
    for k,v in [('cards',3),('scene',1),('done',0),('outcome',2),('battleReturns',0),('fieldReady',0),('privateSaveWrites',True),('resetQuiescent',False),('runAttempts',9),('runAttemptCap',0)]:require(not evidence_passed({**good,k:v},layout),'False Map success accepted: '+k)
def mr_evidence_passed(row,mr,layout):
    fixed=dict(status='passed_original_mr_pokemon',stage=18,done=1,qaBytes=layout['stateBytes'],cards=1,scene=3,mapReadySeen=1,runAttemptCap=8,
               parentEntries=1,parentExits=1,lastApp=2,menuCloses=1,fieldReady=1,menuActive=0,privateSaveWrites=False,resetQuiescent=True)
    endpoint=dict(status='passed_original_mr_pokemon',stage=18,done=1,qaBytes=72,map=143,x=5,z=6,facing=2,manualTiles=124,
        routeScene=2,houseScene=1,apricornBox=1,mysteryEgg=1,pokedex=1,oakContact=1,partyHealed=1,
        apricornScriptSeen=1,apricornCompletedSeen=1,houseScriptSeen=1,doorAnchorSeen=1)
    if not isinstance(row,dict) or not isinstance(mr,dict) or any(type(row.get(k)) is not type(v) or row[k]!=v for k,v in fixed.items()) or any(type(mr.get(k)) is not type(v) or mr[k]!=v for k,v in endpoint.items()):return False
    n=row.get('battleLaunches');a=row.get('runAttempts');before=mr.get('mapCardSteps')
    return type(before) is int and 178<=before<=180 and all(type(row.get(k)) is int for k in ('steps','battleReturns','runs','outcome')) and row['steps']==before+124 and type(n) is int and 0<=n<=32 and type(a) is int and row['battleReturns']==n and row['runs']==a and ((not n and not a) or n>0 and n<=a<=8*n and row['outcome']==5)
def trace_passed(rows,runs,run_count,launch_count=0,mr=False):
    if not rows or len(rows)%3 or len(runs)!=run_count:return False
    serials=set();guide=gift=apricorn=house=False;previous=0
    for i in range(0,len(rows),3):
        a,b,c=rows[i:i+3]
        if [r.get('event') for r in (a,b,c)]!=['press','release','settled']:return False
        if any(type(r.get(k)) is not int for r in (a,b,c) for k in ('padReads','owner','generation','revision','stage','key','touch')):return False
        if not previous<=a['padReads']<=b['padReads'] or c['padReads']-b['padReads']<2:return False
        if any(any(r[k]!=a[k] for k in ('owner','generation','revision','stage')) for r in (b,c)):return False
        if b['key'] or b['touch'] or c['key'] or c['touch']:return False
        if a['owner'] not in (0,1,2,3) or a['owner'] in (1,2) and (not a['generation'] or not a['revision']):return False
        if a['owner']==0 and (a['touch'] or a['key'] not in (16,32,64,128,1024)):return False
        if a['owner']==3:
            token=(a.get('script'),a.get('serial'))
            if a['key']!=1 or a['touch'] or a.get('wait') not in (2,3) or type(token[1]) is not int or token[1]<=0 or token in serials:return False
            if token[0]==2 and a['stage']==2:guide=True
            elif token[0]==3 and a['stage']==4:gift=True
            elif mr and token[0]==3 and a['stage']==14 and a.get('map')==34:apricorn=True
            elif mr and token[0]==2 and a['stage']==17 and a.get('map')==143:house=True
            else:return False
            serials.add(token)
        previous=c['padReads']
    launch=attempt=0;last_revision=None
    for i,r in enumerate(runs):
        if any(type(r.get(k)) is not int for k in ('launch','attempt','totalAttempts','revision','battleType','battleSpecial','index')):return False
        if r['launch']==launch+1:launch+=1;attempt=0;last_revision=None
        if r['launch']!=launch or r['attempt']!=attempt+1 or r['attempt']>8 or r['totalAttempts']!=i+1 or r['revision']==last_revision or r['index']!=3 or r['battleType'] or r['battleSpecial']:return False
        attempt+=1;last_revision=r['revision']
    return guide and gift and launch==launch_count and (not mr or apricorn and house)
def trace_self_check():
    rows=[]
    for script,stage,start in ((2,2,10),(3,4,20)):
        a=dict(event='press',padReads=start,owner=3,generation=0,revision=0,stage=stage,key=1,touch=0,script=script,serial=script,wait=2)
        rows.extend((a,{**a,'event':'release','padReads':start+4,'key':0},{**a,'event':'settled','padReads':start+6,'key':0}))
    require(trace_passed(rows,[],0),'Qualified two-script trace rejected')
    runs=[dict(launch=1,attempt=1,totalAttempts=1,revision=1,battleType=0,battleSpecial=0,index=3),dict(launch=1,attempt=2,totalAttempts=2,revision=2,battleType=0,battleSpecial=0,index=3),dict(launch=2,attempt=1,totalAttempts=3,revision=1,battleType=0,battleSpecial=0,index=3)]
    require(trace_passed(rows,runs,3,2),'Same battle fresh-revision retry/new launch trace rejected')
    for index,key,value in ((1,'revision',1),(1,'attempt',3),(2,'launch',3),(2,'totalAttempts',2)):
        bad=[dict(r) for r in runs];bad[index][key]=value;require(not trace_passed(rows,bad,3,2),'Invalid RUN retry trace accepted')
    for index,key,value in ((0,'script',1),(0,'wait',0),(2,'padReads',15),(3,'key',2),(3,'serial',0)):
        bad=[dict(r) for r in rows];bad[index][key]=value;require(not trace_passed(bad,[],0),'Malformed script/release trace accepted')
def mr_contract_self_check(layout):
    row=dict(status='passed_original_mr_pokemon',stage=18,done=1,qaBytes=layout['stateBytes'],cards=1,scene=3,mapReadySeen=1,
        parentEntries=1,parentExits=1,lastApp=2,menuCloses=1,fieldReady=1,menuActive=0,privateSaveWrites=False,resetQuiescent=True,
        runs=0,runAttempts=0,runAttemptCap=8,battleLaunches=0,battleReturns=0,outcome=0,steps=304)
    mr=dict(status='passed_original_mr_pokemon',stage=18,done=1,qaBytes=72,map=143,x=5,z=6,facing=2,mapCardSteps=180,manualTiles=124,
        routeScene=2,houseScene=1,apricornBox=1,mysteryEgg=1,pokedex=1,oakContact=1,partyHealed=1,
        apricornScriptSeen=1,apricornCompletedSeen=1,houseScriptSeen=1,doorAnchorSeen=1)
    require(mr_evidence_passed(row,mr,layout) and not evidence_passed(row,layout),'Mr/legacy endpoints were conflated')
    for key,value in [('scene',2),('resetQuiescent',False),('steps',303),('privateSaveWrites',True),('runAttemptCap',9)]:
        require(not mr_evidence_passed({**row,key:value},mr,layout),'Bad Mr common evidence admitted: '+key)
    for key,value in [('routeScene',1),('houseScene',0),('map',34),('doorAnchorSeen',0),('mysteryEgg',0),('oakContact',0),('mapCardSteps',181)]:
        require(not mr_evidence_passed(row,{**mr,key:value},layout),'Bad Mr endpoint admitted: '+key)
    rows=[]
    for script,stage,start,map_id in ((2,2,10,67),(3,4,20,67),(3,14,30,34),(2,17,40,143)):
        a=dict(event='press',padReads=start,owner=3,generation=0,revision=0,stage=stage,key=1,touch=0,script=script,serial=start,wait=2,map=map_id)
        rows.extend((a,{**a,'event':'release','padReads':start+4,'key':0},{**a,'event':'settled','padReads':start+6,'key':0}))
    require(trace_passed(rows,[],0,0,True) and not trace_passed(rows,[],0),'Mr input modes were conflated')
    bad=[dict(r) for r in rows];bad[6]['map']=67;require(not trace_passed(bad,[],0,0,True),'Wrong-map Mr script admitted')
def snapshots_passed(folder,rows,maprows,layout,mrrows=None):
    if not rows or not maprows:return False
    expected=set();mapseen=set()
    for sequence,row in enumerate(rows):
        if row.get('sequence')!=sequence:return False
        state=folder/f'pokegear-{sequence:03d}-state.bin';menu=folder/f'pokegear-{sequence:03d}-menu.bin';expected|={state,menu}
        if any(not p.is_file() or p.is_symlink() for p in (state,menu)):return False
        raw=state.read_bytes();m=menu.read_bytes()
        if len(raw)!=layout['prefixBytes'] or len(m)!=36:return False
        ui=struct.unpack_from('<12I',raw,layout['uiOffset']);required=3 if ui[0] or struct.unpack_from('<I',m)[0] else 1
        if row.get('requiredMask')!=required or row.get('renderedMask',0)&row.get('variedMask',0)&required!=required:return False
        for engine in range(2):
            if not row['renderedMask']&(1<<engine):continue
            image=folder/f'pokegear-{sequence:03d}-engine-{engine}.ppm';expected.add(image)
            if not image.is_file() or image.is_symlink():return False
            data=image.read_bytes();header=b'P6\n256 192\n255\n'
            if not data.startswith(header) or len(data)!=len(header)+256*192*3:return False
            if required&(1<<engine) and len(set(data[len(header):][i:i+3] for i in range(0,256*192*3,3)))<2:return False
    for row in maprows:
        n=row.get('sequence')
        if type(n) is not int or n in mapseen or not 0<=n<len(rows):return False
        path=folder/f'map-{n:03d}-state.bin'
        if not path.is_file() or path.is_symlink():return False
        raw=path.read_bytes()
        if len(raw)!=layout['stateBytes'] or struct.unpack_from('<II',raw,layout['mapOffset'])!=(1,layout['stateBytes']):return False
        if raw[:layout['prefixBytes']]!=(folder/f'pokegear-{n:03d}-state.bin').read_bytes():return False
        mapseen.add(n);expected.add(path)
    mrseen=set();mrcomplete=False
    for row in mrrows or []:
        n=row.get('sequence');path=folder/f'mr-{n:03d}-state.bin' if type(n) is int else None
        if n in mrseen or n not in mapseen or not path.is_file() or path.is_symlink():return False
        raw=path.read_bytes()
        if len(raw)!=72:return False
        packet=struct.unpack('<18I',raw)
        if packet[:2]!=(1,72) or packet[3] or packet[4]!=1 or packet[6]!=row.get('routeScene') or packet[7]!=row.get('houseScene'):return False
        mrcomplete|=row.get('stage')==18 and packet[5]==1;mrseen.add(n);expected.add(path)
    return expected==set(folder.iterdir()) and any(r.get('stage')==8 and r.get('cards')==1 and r.get('scene')==2 for r in maprows) and (mrrows is None or mrcomplete)
def execute(root,here,args,out,copies,report,pins,base):
    require(here==root,'Runtime requires installed Map QA4 and matching fresh Full')
    sys.path.insert(0,str(root/'ios/tools'))
    from check_native_pc_box import Runner,validate_full,assert_unchanged,require_hashes,wasm_exports
    from check_native_save_roundtrip import static_delta
    from prepare_overlay_state import compile_unit
    context=validate_full(args.full_receipt);require(context['fullSourceBankCurrent'],'Fresh Full must include the exact installed Map QA4')
    shipping=context['formal']['shipping']['exports'];require(len(shipping)==33 and context['formal']['actualNormalCheckKinds']==104,'Shipping33/normal104 required')
    save=args.save.resolve();fixture=[Path(str(save)+s) for s in ('','.console','.wifi','.lock')]
    require(not args.save.is_symlink() and all(p.is_file() and not p.is_symlink() for p in fixture) and set(fixture)==set(save.parent.glob(save.name+'*')),'Exactly four original regular fixture files required')
    fixture_pins={str(p):sha(p) for p in fixture};h=context['helpers'];runner=Runner(report);guest=out/'guest';guest.mkdir();units={r['file']:r for r in context['audited']['units']};objects=list(context['objects'])
    flags=['--target=wasm32-wasip1','-std=gnu11','-O2','-fwrapv','-fno-strict-aliasing','-fno-builtin-malloc','-fno-builtin-free','-fwasm-exceptions','-fexceptions','-Xclang','-disable-red-zone','-fms-extensions','-Wno-microsoft','-Wno-unknown-pragmas','-DHEARTGOLD_NATIVE','-DHEARTGOLD_ABI32','-DSDK_ARM9','-DSDK_CW','-DSDK_CODE_ARM','-DSDK_TS','-D_NITRO','-DPM_KEEP_ASSERTS','-DPLATFORM_INTRINSIC_FUNCTION_BIT_CLZ32=__builtin_clz']
    for p in (*context['includes'],here/'ios/tests',root/'ios/tests',root/'ios/build-original-host/include',root/'include',root/'files',root/'src',root/'lib/include',root/'lib/include/cw',root/'lib/include/nitro',root/'lib/include/nnsys',root/'lib/include/nitro/os',root/'include/library',root/'lib/NitroSDK/include',root/'lib/NitroSDK/include-pokediamond'):flags+=['-I',str(p)]
    if context['link'].get('nativeContent'):flags.append('-DHG_NATIVE_ROM_TABLES')
    if args.mr_pokemon:flags.append('-DHG_PG_MR_EXTENSION')
    private=[];private_pins={};replacements=[];new_objects=[];old_objects=[]
    try:
        require_hashes(pins,'Map source inputs')
        for name in base.COPIES:
            source=copies/name;target=guest/(Path(name).stem+'.o');expected=units[name]['overlayStates'];log=guest/(Path(name).stem+'.log')
            if expected:code,text,state=compile_unit(str(args.wasi_sdk/'bin/clang'),flags,source,target,name,root);log.write_text(text)
            else:row,text=runner([args.wasi_sdk/'bin/clang',*flags,'-c',source,'-o',target],log);code=row['terminalReturnCode'];state={}
            require(code==0 and 'error' not in state and state.get('getter')==(expected[0]['getter'] if expected else None),'Original overlay ownership changed: '+name)
            if expected:require(state.get('globals')==expected[0]['globals'],'Original writable globals changed: '+name)
            old=str(context['audit']/'objects'/Path(name).with_suffix('.o'));require(objects.count(old)==1,'One original object required: '+name);objects[objects.index(old)]=str(target)
            new_objects.append(target);old_objects.append(Path(old));replacements.append(dict(source=name,originalSha256=sha(root/name),candidateSha256=sha(source),overlayState=state))
        memory=static_delta(new_objects,old_objects,runner,guest);report['privateStaticData']=memory
        rsp=guest/'objects.rsp';rsp.write_text('\n'.join(shlex.quote(p) for p in objects)+'\n');command=context['link']['command'].copy();command[command.index(context['rsp'])]='@'+str(rsp)
        for kind in ('initial','max'):
            indexes=[i for i,p in enumerate(command) if p.startswith('-Wl,--'+kind+'-memory=')];require(len(indexes)==1,'One private fixed memory flag required');i=indexes[0];command[i]=command[i].split('=')[0]+'='+str(int(command[i].split('=')[1])+memory['extraFixedMemoryBytes'])
        module=guest/'engine.wasm';command[command.index('-o')+1]=str(module)
        for name in LINK_WRAPS:require('-Wl,--wrap='+name not in command,'Duplicate wrap');command.insert(command.index('-o'),'-Wl,--wrap='+name)
        for name in report['privateExports']:command.insert(command.index('-o'),'-Wl,--export='+name)
        runner(command,guest/'strict-link.log');report['moduleBuilt']=True
        require(wasm_exports(module.read_bytes())==sorted([*shipping,*report['privateExports']]),'Exactly current public plus mode-qualified private exports required');report['observerDefinitionsLinked']=True
        native=out/'native';native.mkdir();support=(root/'ios/Original/abi32/pokegear_scenario_check.c').read_text();require(support.count('int main(int argc,char **argv)')==1,'Phone main identity changed')
        (native/'pokegear_map_phone_support.inc').write_text(support.replace('int main(int argc,char **argv)','static int PGMapUnusedPhoneMain(int argc,char **argv)',1))
        generated,writes,restores=h['generate_engine'](native,module,lambda argv,log:runner(argv,native/log));report['genuineGeneratedHeaderSha256']=sha(native/'hgengine.h');cflags,runtime=h['native_build_settings'](native)
        cflags+=['-I',str(here/'ios/tests'),'-I',str(root/'ios/tests'),'-I',str(root/'ios/Original/abi32'),'-g','-fsanitize=address,undefined','-fno-sanitize-recover=all'];native_objects=[]
        if args.mr_pokemon:cflags.append('-DHG_PG_MR_EXTENSION')
        for i,source in enumerate([*generated,*runtime,here/QA_PATHS[2]]):
            target=native/f'host-{i}.o';extra=['-DWASM_RT_TRAP_HANDLER=HGEngineTrap'] if source.name=='wasm-rt-impl.c' else [];runner(['xcrun','clang',*cflags,*extra,'-c',source,'-o',target],native/f'compile-{i}.log');native_objects.append(target)
        binary=native/'map-card-check';runner(['xcrun','clang','-fsanitize=address,undefined',*native_objects,'-o',binary],native/'link.log');report['hostABICompiled']=True
        boot=out/'boot-header.bin';boot.write_bytes(h['read_boot_header'](context['pack']));frames=out/'frames';frames.mkdir()
        for p in fixture:copy=out/('scenario.sav'+p.name[len(save.name):]);shutil.copy2(p,copy);private.append(copy)
        private_pins={str(p):sha(p) for p in private};assert_unchanged(context);require_hashes(pins,'Map source inputs');require_hashes(fixture_pins,'Original fixture')
        report.update(gameExecuted=True,gameRuntimeExecuted=True,sourceCopiesCompiled=replacements,privateModuleSha256=sha(module),checkedStackWrites=writes,exceptionDepthRestores=restores)
        row,text=runner([binary,context['pack']/'files',boot,out/'scenario.sav',frames],out/'runtime.log',timeout=600 if args.mr_pokemon else 300,check=False)
        rows={p:[] for p in ('MAP evidence: ','MAP baseline: ','MAP input: ','MAP run: ','MAP snapshot: ','POKEGEAR snapshot: ','MR evidence: ','MR snapshot: ')}
        for line in text.splitlines():
            for prefix,destination in rows.items():
                if line.startswith(prefix):destination.append(json.loads(line[len(prefix):]));break
        report['runtimeEvidence']=rows
        accepted=len(rows['MAP evidence: '])==1 and (len(rows['MR evidence: '])==1 and mr_evidence_passed(rows['MAP evidence: '][0],rows['MR evidence: '][0],report['nativeLayout']) if args.mr_pokemon else evidence_passed(rows['MAP evidence: '][0],report['nativeLayout']))
        require(row['terminalReturnCode']==0 and not row['timedOut'] and row['allChildrenWaited'] and accepted,'Original Map/Mr flow failed; no automatic retry')
        require(rows['MAP baseline: ']==[dict(map=33,x=668,z=397,scene=0,cards=0)],'Fresh actual Map baseline mismatch')
        require(trace_passed(rows['MAP input: '],rows['MAP run: '],rows['MAP evidence: '][0]['runAttempts'],rows['MAP evidence: '][0]['battleLaunches'],args.mr_pokemon),'Qualified script/RUN/release trace mismatch')
        require(snapshots_passed(frames,rows['POKEGEAR snapshot: '],rows['MAP snapshot: '],report['nativeLayout'],rows['MR snapshot: '] if args.mr_pokemon else None),'Original paired snapshot files mismatch')
        require(private_pins=={str(p):sha(p) for p in private},'Unexpected private4 disk change')
        report.update(passed=True,complete=True,status='passed_original_mr_pokemon' if args.mr_pokemon else 'passed_original_map_card',privateSaveFilesUnchanged=True,originalFixtureUnchanged=True,allCardsComplete=False,
                      limits=['Radio/Fly/Town/EasyChat positive flows remain excluded.','Original story and encounter effects are RAM changes; no disk persistence is claimed.','Original PPM visual review remains a separate post-run step.'])
    finally:
        assert_unchanged(context);require_hashes(pins,'Map source inputs');require_hashes(fixture_pins,'Original fixture')
        if private_pins:
            report['privateFileHashesBefore']=private_pins;report['privateFileHashesAfter']={str(p):sha(p) for p in private}
            require(report['privateFileHashesBefore']==report['privateFileHashesAfter'],'Private four files changed during attempted flow')
def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--source-root',type=Path,default=ROOT);parser.add_argument('--wasi-sdk',type=Path,required=True)
    parser.add_argument('--output',type=Path,required=True);parser.add_argument('--host-generated',type=Path)
    parser.add_argument('--full-receipt',type=Path);parser.add_argument('--save',type=Path)
    parser.add_argument('--mr-pokemon',action='store_true',help='Opt-in natural Route30/Apricorn/MrPokemon/Oak continuation; stops indoors')
    args=parser.parse_args();root=args.source_root.resolve();here=ROOT;out=args.output.resolve()
    require(bool(args.full_receipt)==bool(args.save),'Runtime requires both explicit --full-receipt and --save')
    require(not out.exists(),'Fresh output required');out.mkdir(parents=True)
    base=load(root/'ios/tools/check_native_pokegear_qa.py')
    source_names=(*base.COPIES,'ios/tools/check_native_pokegear_qa.py','ios/Original/abi32/pokegear_scenario_check.c',
        'ios/tests/pokegear_qa.h','ios/tests/pokegear_qa_guest.inc','ios/tools/check_native_save_roundtrip.py','src/player_avatar_movement.c','src/field/field_control.c',
        'src/field/scrcmd_message.c','src/script_manager.c','src/encounter.c','src/battle/battle_input.c','src/battle/battle_util.c','src/battle/battle_command.c','files/fielddata/encountdata/gs_enc_data.json',
        'files/fielddata/script/scr_seq/scr_seq_0850_T21.s','files/fielddata/eventdata/zone_event/064_T21.json',
        'files/battledata/script/subscript/subscript_0003_RunAway.s','files/battledata/script/subscript/subscript_0008_RunAwayFail.s','include/native_pokegear_ui.h','include/native_battle_ui.h')
    if args.mr_pokemon:source_names+=('files/fielddata/script/scr_seq/scr_seq_0227_R30.s','files/fielddata/script/scr_seq/scr_seq_0229_R30R0201.s','include/constants/vars.h','include/constants/items.h','include/constants/phone_contacts.h','include/save_pokegear.h','include/party.h','include/pokedex.h','include/bag.h')
    pins={str(p):sha(p) for p in [*(here/p for p in QA_PATHS),*(root/p for p in source_names)]}
    report=dict(passed=False,complete=False,commands=[],sourceHashesBefore=pins,moduleBuilt=False,gameExecuted=False,gameRuntimeExecuted=False,observerDefinitionsLinked=False,hostABICompiled=False,
                audioOperations=False,privateExports=['HGNativePokegearQAGetState','HGNativePokegearQARequest'],futureExports=35,shippingABIChanged=False,
                requiredPrivateLinkWraps=list(LINK_WRAPS),Phone33DefaultCaseUnchanged=True,allCardsComplete=False)
    if args.mr_pokemon:report.update(privateExports=[*report['privateExports'],'HGNativeMrPokemonQAGetState'],futureExports=36,mrPokemonMode=True,legacyMapPacketUnchanged=True)
    env={**os.environ,'HEARTGOLD_TEST_MUTE':'1','SIMCTL_CHILD_HEARTGOLD_TEST_MUTE':'1','PYTHONDONTWRITEBYTECODE':'1'}
    def run(argv,label):
        result=subprocess.run(['nice','-n','10',*map(str,argv)],cwd=root,env=env,capture_output=True,text=True,timeout=60)
        (out/label).write_text(result.stdout+result.stderr)
        report['commands'].append(dict(argv=list(map(str,argv)),terminalReturnCode=result.returncode,log=label,allChildrenWaited=True))
        require(result.returncode==0,'Failed '+str(out/label));return result.stdout+result.stderr
    try:
        copies=out/'source';copies.mkdir();originals=source_copies(root,here,copies,base)
        host=(here/QA_PATHS[2]).read_text();(out/'actual_map_settle.inc').write_text(base.function(host,'mapSettle')+'\n')
        guest=(here/QA_PATHS[1]).read_text();(out/'actual_map_battle.inc').write_text('\n'.join(base.function(guest,name) for name in ('__wrap_Battle_LaunchApp','__wrap_BattleSetup_Delete'))+'\n')
        (out/'actual_map_request.inc').write_text(base.function(guest,'HGNativePokegearQARequest')+'\n')
        if args.mr_pokemon:
            (out/'actual_mr_running.inc').write_text(base.function(guest,'PGMrRunning')+'\n')
            (out/'actual_mr_context.inc').write_text(base.function(guest,'PGMrTextContext')+'\n')
        (out/'actual_native_battle_choose.inc').write_text(base.function((root/'src/battle/battle_input.c').read_text(),'HGNativeBattleUIChoose')+'\n')
        check=out/'actual_map_check.c';check.write_text(C_CHECK)
        includes=[here/'ios/tests',root/'ios/tests',root/'include',root/'ios/Original/abi32',root/'ios/build-original-host/include',out]
        tools=Path(os.environ.get('HG_ABI32_TOOLS',root/'ios/build-original-abi32/tools'))
        native=['xcrun','clang','-std=c11','-O1','-g','-fms-extensions','-Wno-microsoft','-Wno-unknown-pragmas','-fsanitize=address,undefined','-fno-sanitize-recover=all','-DSDK_ARM9','-DHEARTGOLD_NATIVE']
        if args.mr_pokemon:native.append('-DHG_PG_MR_EXTENSION')
        for p in includes:native+=['-iquote',p]
        native+=['-I',tools/'wabt-1.0.42/include','-idirafter',root/'ios/build-original-host/include']
        for p in ('lib/include','include/library','lib/include/cw','lib/include/nitro','lib/include/nnsys','lib/include/nitro/os','lib/NitroSDK/include','lib/NitroSDK/include-pokediamond','files'):native+=['-idirafter',root/p]
        run([*native,check,'-o',out/'actual-map-check'],'native-compile.log');log=run([out/'actual-map-check'],'native-run.log')
        layouts=[json.loads(line[11:]) for line in log.splitlines() if line.startswith('MAP_LAYOUT ')];require(len(layouts)==1,'Real native layout missing');report['nativeLayout']=layouts[0];evidence_self_check(layouts[0]);trace_self_check()
        require(report['futureExports']==33+len(report['privateExports']) and len(set(report['privateExports']))==len(report['privateExports']),'Mode-specific private ABI changed')
        if args.mr_pokemon:mr_contract_self_check(layouts[0])
        flags=['--target=wasm32-wasip1','-std=gnu11','-O2','-fwrapv','-fno-strict-aliasing','-fno-builtin-malloc','-fno-builtin-free','-fwasm-exceptions','-fexceptions','-Xclang','-disable-red-zone','-fms-extensions','-Wno-microsoft','-Wno-unknown-pragmas','-DHEARTGOLD_NATIVE','-DHEARTGOLD_ABI32','-DSDK_ARM9','-DSDK_CW','-DSDK_CODE_ARM','-DSDK_TS','-D_NITRO','-DPM_KEEP_ASSERTS','-DPLATFORM_INTRINSIC_FUNCTION_BIT_CLZ32=__builtin_clz']
        if args.mr_pokemon:flags.append('-DHG_PG_MR_EXTENSION')
        for p in includes:flags+=['-I',p]
        for p in ('files','src','lib/include','lib/include/cw','lib/include/nitro','lib/include/nnsys','lib/include/nitro/os','include/library','lib/NitroSDK/include','lib/NitroSDK/include-pokediamond'):flags+=['-I',root/p]
        for i,name in enumerate(base.COPIES):run([args.wasi_sdk/'bin/clang',*flags,'-fsyntax-only',copies/name],f'wasi-{i}.log')
        layout=out/'layout.c';layout.write_text('#include "global.h"\n#include "pokegear_map_card_qa.h"\n_Static_assert(sizeof(HGPokegearMapCardQAState)=='+str(layouts[0]['stateBytes'])+',"Real WASI private packet/native host layout");\n_Static_assert(__builtin_offsetof(HGPokegearMapCardQAState,version)=='+str(layouts[0]['prefixBytes'])+',"Original prefix ABI");\n')
        if args.mr_pokemon:layout.write_text(layout.read_text()+'_Static_assert(sizeof(HGPokegearMrPokemonQAState)==72,"Separate Mr packet WASI/native layout");\n')
        run([args.wasi_sdk/'bin/clang',*flags,'-Xclang','-fdump-record-layouts','-fsyntax-only',layout],'wasi-layout.log')
        report['actualWASI32LayoutVerified']=True
        support=(root/'ios/Original/abi32/pokegear_scenario_check.c').read_text();anchor='int main(int argc,char **argv)'
        require(support.count(anchor)==1,'Phone33 main source identity changed')
        (out/'pokegear_map_phone_support.inc').write_text(support.replace(anchor,'static int PGMapUnusedPhoneMain(int argc,char **argv)',1))
        if args.host_generated:
            require('w2c_hgengine_HGNativePokegearQAGetState' in (args.host_generated/'hgengine.h').read_text(),'Genuine private header lacks original private getters')
            if args.mr_pokemon:require('w2c_hgengine_HGNativeMrPokemonQAGetState(' in (args.host_generated/'hgengine.h').read_text(),'Mr mode requires its genuine private36 header, not the prior Map header')
            hostflags=['xcrun','clang','-std=c11','-fsyntax-only','-fms-extensions','-Wno-microsoft','-Wno-unknown-pragmas','-DSDK_ARM9','-DHEARTGOLD_NATIVE','-DWASM_RT_USE_MMAP=0','-DWASM_RT_MEMCHECK_GUARD_PAGES=0','-DWASM_RT_MEMCHECK_BOUNDS_CHECK=1']
            if args.mr_pokemon:hostflags.append('-DHG_PG_MR_EXTENSION')
            for p in [args.host_generated,*includes,tools/'wabt-1.0.42/include',tools/'wabt-1.0.42/share/wabt/wasm2c']:hostflags+=['-I',p]
            for p in ('lib/include','include/library'):hostflags+=['-idirafter',root/p]
            run([*hostflags,here/QA_PATHS[2]],'genuine-prior-header-host-syntax.log')
            report['hostSyntaxWithGenuineMr36Header' if args.mr_pokemon else 'hostSyntaxWithGenuinePrior33Header']=True;report['priorGeneratedHeaderSha256']=sha(args.host_generated/'hgengine.h')
        report.update(passed=True,status='passed_map_source_contracts',originalBodiesPreserved=True,originalSourceCopies=list(base.COPIES),
                      sourceCopyHashes={name:sha(copies/name) for name in base.COPIES},actualNativeRouteAndLeaseCheck=True,actualReadonlyBattleWrappersCheck=True,actualOriginalChooseRetrySequenceCheck=True,runAttemptCap=8,
                      limits=['No Map private module linked, so linker wrappers and generated Map ABI remain unproved.','No game, route/grass/guide/Map images or encounter runtime acceptance.','Original wild damage/RNG/seen/stats changes are allowed; Phone33 immutable RAM ledger is deliberately not reused.','Original and private disk nonmutation must be checked in a future authorized runtime driver.'])
        if args.mr_pokemon:report.update(status='passed_mr_source_contracts',mrPrivateStateBytes=72,actualMrPredicateLeaseAndRunContinuationCheck=True,
            staticManualTiles=124,staticGrassTiles=39,indoorEndpoint=dict(map=143,x=5,z=6,facing=2),
            limits=[*report['limits'],'Mr route/BDHC, random encounter survival, automatic scripts and indoor gifts remain runtime-unproved.','Party HP/status is observed; full PP restoration and the Elm call after exiting are outside this endpoint.'])
        if args.full_receipt:execute(root,here,args,out,copies,report,pins,base)
    except BaseException as error:report.update(passed=False,firstFailure=dict(type=type(error).__name__,message=str(error)))
    finally:
        report['sourceInputsUnchanged']=pins=={p:sha(p) for p in pins};report['passed'] &= report['sourceInputsUnchanged']
        (out/'report.json').write_text(json.dumps(report,indent=2,sort_keys=True)+'\n')
    print(('PASS' if report['passed'] else 'FAIL')+': '+str(out/'report.json'));return 0 if report['passed'] else 1
if __name__=='__main__':raise SystemExit(main())
