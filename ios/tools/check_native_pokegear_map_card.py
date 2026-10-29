#!/usr/bin/env python3
"""Authentic Map Card case: source checks by default; explicit Full+save opts into one runtime."""
import argparse
import hashlib
import importlib.util
import json
import os
import re
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
#ifdef HG_PG_ELM_EXTENSION
#include "application/pokegear/phone/phone_internal.h"
static HGNativePokegearQAState sPGQA;
static u32 sPGElmPending,elmLeaseSerial,elmLeaseHangup;
#include "actual_elm_running.inc"
#endif
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
#ifdef HG_PG_RIVAL_EXTENSION
static HGPokegearFirstRivalQAState rivalFinal;
static u32 rivalLeaseOwner,rivalLeaseState,rivalLeaseKind,rivalLeaseSerial;
#undef HGMapTextAllowed
#define HGMapTextAllowed(s,i,r) HGRivalTextQualified((s),&mrFinal,&rivalFinal,(i),(r))
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
#ifdef HG_PG_RIVAL_EXTENSION
#include "party.h"
#include "pokemon.h"
#include "constants/trainers.h"
#include "constants/trainer_class.h"
static HGPokegearFirstRivalQAState sPGRival;
static const BattleSetup *sPGRivalBattle;
static u32 sPGRivalPending;
#include "actual_rival_running.inc"
#include "actual_rival_next.inc"
#define PGMAP_REQUIRE(value) do{if(!(value)){sPGMap.error=__LINE__;return;}}while(0)
#include "actual_rival_stat_wait.inc"
#include "actual_rival_exp_enum.inc"
int Party_GetCount(const Party *party){assert(party==(Party *)1 || party==(Party *)2);return 1;}
Pokemon *Party_GetMonByIndex(Party *party,int slot){assert(!slot);return (Pokemon *)party;}
u32 GetMonData(Pokemon *mon,int attr,void *ptr){assert(!ptr);return attr==MON_DATA_SPECIES ? (mon==(Pokemon *)1 ? 152 : 155) : attr==MON_DATA_LEVEL ? 5 : 0;}
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
#ifdef HG_PG_ELM_EXTENSION
static const PokegearAppData *sPGMapParent;
#include "actual_elm_owner.inc"
#endif
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
#ifdef HG_PG_ELM_EXTENSION
 PokegearAppData owner={0};PokegearPhoneAppData phone={0};PokegearPhoneCallContext call={0};PhoneCallScriptDef definition={0};
 field.saveData=(SaveData *)1;owner.saveData=field.saveData;owner.childAppdata=&phone;phone.pokegear=&owner;
 sPGMapParent=&owner;call.phoneApp=&phone;call.saveData=field.saveData;call.state.callerID=1;call.state.isIncomingCall=1;
 call.state.isScriptedCall=2;call.state.scriptID=PHONE_SCRIPT_002;call.state.scriptDef=&definition;
 assert(PGElmCallRootIdentity(&call));call.state.predefinedScriptID=1;assert(!PGElmCallRootIdentity(&call));call.state.predefinedScriptID=0;
 owner.args=NULL;assert(PGElmCallRootIdentity(&call)); // Public scriptedLaunch is not the retained call identity.
 call.state.isScriptedCall=0;assert(!PGElmCallRootIdentity(&call));call.state.isScriptedCall=2;
 phone.pokegear=NULL;assert(!PGElmCallRootIdentity(&call));phone.pokegear=&owner;
 HGNativePokegearQAState elm={.coverage=63,.callSelections=1,.callTarget=1,.callIncoming=1,.callScripted=2,.callScriptID=2,
  .callMainState=1,.callScriptState=3,.callTextWait=2,.callTextSerial=1,.viewFactsGeneration=9,
  .ui={.active=1,.ready=1,.viewActive=1,.ownerGeneration=9,.revision=3,.ownerKind=1,.app=3,.viewKind=3,.parentPhase=11,.phase=6,.registeredCards=1}};
 assert(HGElmCallLease(&elm,9,3,1,0) && !HGElmCallLease(&elm,8,3,1,0) && !HGElmCallLease(&elm,9,2,1,0) && !HGElmCallLease(&elm,9,3,2,0));
 elm.ui.scriptedLaunch=0;assert(HGElmCallLease(&elm,9,3,1,0));elm.callTextWait=0;assert(!HGElmCallLease(&elm,9,3,1,0));
 elm.callMainState=2;elm.callAwaitHangup=1;assert(HGElmCallLease(&elm,9,3,1,1));elm.ui.phase=1;assert(!HGElmCallLease(&elm,9,3,1,1));
 assert(HGElmPhoneReady(&elm,1));elm.ui.parentPhase=7;assert(!HGElmPhoneReady(&elm,1));
 elm.ui.parentPhase=11;elm.ui.phase=6;elm.callMainState=1;elm.callAwaitHangup=0;elm.callTextWait=3;pgFinal=elm;
 struct w2c_env elmEnv={0};Capture elmCapture={&elmEnv};pgCapture=&elmCapture;
 mapOwner=4;mapGen=9;mapRev=3;mapLCD=0;elmLeaseSerial=1;elmLeaseHangup=0;mapLease=1;mapReads=inputReads=2000;
 assert(!mapSettle() && !inputCalls);inputReads=2004;assert(!mapSettle() && mapLease==2 && inputCalls==1);
 inputReads=2006;assert(mapSettle() && !mapLease);
 mapLease=1;mapReads=inputReads=2010;pgFinal.callTextSerial=2;assert(!mapSettle() && mapLease==2 && inputCalls==2);
 inputReads=2012;assert(mapSettle());inputCalls=inputKeys=inputPressed=inputReads=mapLease=0;
 HGPokegearMapCardQAState terminal={.mapId=34,.x=568,.z=295,.facing=1,.cards=1,.scene=3,.betweenSteps=1,.lastApp=3,.parentEntries=2,.parentExits=2,
  .phone={.fieldReady=1,.panicFlag=1,.coverage=255,.callSelections=1,.callCompletions=1}};
 HGPokegearMrPokemonQAState mr={.done=1,.routeScene=3,.houseScene=1,.mysteryEgg=1,.pokedexEnabled=1,.oakRegistered=1};
 assert(HGElmEndpoint(&terminal,&mr));terminal.phone.ui.active=1;assert(!HGElmEndpoint(&terminal,&mr));terminal.phone.ui.active=0;
 mr.routeScene=2;assert(!HGElmEndpoint(&terminal,&mr));mr.routeScene=3;terminal.scriptActive=1;assert(!HGElmEndpoint(&terminal,&mr));
#endif
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
#ifdef HG_PG_RIVAL_EXTENSION
 _Static_assert(sizeof(HGPokegearFirstRivalQAState)==120,"Separate rival stat-wait packet");
 _Static_assert(STATE_GET_EXP_LEVEL_UP_SUMMARY_PRINT_DIFF_WAIT==HG_PG_RIVAL_STAT_DIFF && STATE_GET_EXP_LEVEL_UP_SUMMARY_PRINT_TRUE_WAIT==HG_PG_RIVAL_STAT_TRUE,"Original stat-wait states");
 route(hgRivalSouthRoute,sizeof(hgRivalSouthRoute)/sizeof(*hgRivalSouthRoute),120,120);
 route(hgRivalCityRoute,sizeof(hgRivalCityRoute)/sizeof(*hgRivalCityRoute),90,90);
 HGPokegearFirstRivalQAState rival={.started=1,.scriptSeen=1,.trainerLaunches=1,.trainerReturns=1,.battleType=0x801,.trainerId=496,
  .trainerClass=119,.opponentSpecies=155,.opponentLevel=5,.playerSpecies=152,.playerLevel=5,.outcome=1,.battleResult=1,.metPasserby=1,.rivalHidden=1,
  .statWaits=2,.statRetirements=2,.statSerial=2,.statRetiredSerial=2};
 HGPokegearMapCardQAState end={.started=1,.done=1,.mapId=67,.x=575,.z=399,.facing=2,.cards=1,.scene=4,.lastApp=3,
  .parentEntries=2,.parentExits=2,.betweenSteps=1,.phone={.fieldReady=1,.coverage=255,.callCompletions=1}};
 HGPokegearMrPokemonQAState gifts={.done=1,.routeScene=3,.houseScene=1,.mysteryEgg=1,.pokedexEnabled=1,.oakRegistered=1};
 assert(HGRivalEndpoint(&end,&gifts,&rival));rival.outcome=rival.battleResult=2;rival.statWaits=rival.statRetirements=rival.statSerial=rival.statRetiredSerial=0;assert(HGRivalEndpoint(&end,&gifts,&rival));
 rival.battleResult=1;assert(!HGRivalEndpoint(&end,&gifts,&rival));rival.battleResult=2;
 end.scriptActive=1;end.scriptId=4;end.scene=3;end.scriptWait=2;end.scriptSerial=70;
 assert(HGRivalTextQualified(&end,&gifts,&rival,4,70) && !HGRivalTextQualified(&end,&gifts,&rival,4,69));
 rival.battleLive=1;assert(!HGRivalTextQualified(&end,&gifts,&rival,4,70));rival.battleLive=0;
 rivalFinal=rival;mrFinal=gifts;mapFinal=end;mapOwner=3;mapLCD=inputLCD=0;mapLease=1;mapReads=inputReads=3000;mapLeaseScript=4;mapLeaseSerial=70;
 unsigned rivalInputCalls=inputCalls;assert(!mapSettle() && inputCalls==rivalInputCalls);mapFinal.scriptSerial++;
 assert(!mapSettle() && mapLease==2 && inputCalls==rivalInputCalls+1);
 for(u32 outcome=1;outcome<=2;outcome++){
  sPGMap=(HGPokegearMapCardQAState){.started=1,.done=1,.mapId=67,.x=575,.z=400,.scene=3};sPGRival=(HGPokegearFirstRivalQAState){.started=1,.scriptSeen=1};
  setup=(BattleSetup){.battleType=0x801,.trainerId={[1]=496},.party={(Party *)1,(Party *)2},.trainer={[1]={.data={.trainerClass=119}}},.winFlag=outcome};original=setup;
  __wrap_Battle_LaunchApp(&field,&setup);assert(sPGRivalBattle==&setup && sPGRival.battleLive && sPGRival.trainerLaunches==1 && !sPGMap.battleLaunches && !sPGMap.error && !memcmp(&setup,&original,sizeof(setup)));
  __wrap_BattleSetup_Delete(&setup);assert(!sPGRivalBattle && !sPGRival.battleLive && sPGRival.outcome==outcome && sPGRival.trainerReturns==1 && !sPGMap.battleReturns && !sPGMap.error && !memcmp(&setup,&original,sizeof(setup)));
 }
 gNativeBattleUIState=(HGNativeBattleUIState){.active=1,.ready=1,.mode=HG_BATTLE_UI_COMMAND,.enabledMask=1,.revision=30};sNativeBattleChoice=-1;
 assert(HGRivalChoiceReady(&gNativeBattleUIState,0,0) && !HGNativeBattleUIChoose(29,0));
 assert(HGNativeBattleUIChoose(30,0) && !HGNativeBattleUIChoose(30,0));
 gNativeBattleUIState=(HGNativeBattleUIState){.active=1,.ready=1,.mode=HG_BATTLE_UI_MOVES,.enabledMask=1,.revision=31,.move={33},.pp={35}};sNativeBattleChoice=-1;
 assert(HGRivalChoiceReady(&gNativeBattleUIState,1,30) && HGNativeBattleUIChoose(31,0) && !HGNativeBattleUIChoose(31,0));
 gNativeBattleUIState.ready=1;assert(!HGRivalChoiceReady(&gNativeBattleUIState,2,31));gNativeBattleUIState.revision=32;gNativeBattleUIState.pp[0]=0;
 assert(!HGRivalChoiceReady(&gNativeBattleUIState,2,31));gNativeBattleUIState.pp[0]=34;assert(!HGRivalChoiceReady(&gNativeBattleUIState,64,31));
 sPGMap.error=0;sPGRival=(HGPokegearFirstRivalQAState){.started=1,.battleLive=1};
 HGNativeFirstRivalQAStatWaitObserved((void *)0x1234,HG_PG_RIVAL_STAT_DIFF,1,1);
 gNativeBattleUIState=(HGNativeBattleUIState){.mode=HG_BATTLE_UI_NONE};
 assert(sPGRival.statWaits==1 && sPGRival.statSerial==1 && HGRivalStatReady(&sPGRival,&gNativeBattleUIState,0));
 assert(!HGRivalStatReady(&sPGRival,&gNativeBattleUIState,1) && !HGRivalStatLease(&sPGRival,0x1235,11,1,1) && !HGRivalStatLease(&sPGRival,0x1234,13,1,1));
 gNativeBattleUIState.ready=1;assert(!HGRivalStatReady(&sPGRival,&gNativeBattleUIState,0));gNativeBattleUIState.ready=0;
 rivalFinal=sPGRival;rivalLeaseOwner=0x1234;rivalLeaseState=11;rivalLeaseKind=1;rivalLeaseSerial=1;
 mapOwner=5;mapLCD=inputLCD=0;mapLease=1;mapReads=inputReads=4000;assert(!mapSettle() && mapLease==1);
 HGNativeFirstRivalQAStatWaitObserved((void *)0x1234,11,1,0);rivalFinal=sPGRival;assert(!mapSettle() && mapLease==2);inputReads+=2;assert(mapSettle());
 HGNativeFirstRivalQAStatWaitObserved((void *)0x1234,13,2,1);assert(sPGRival.statWaits==2 && sPGRival.statSerial==2 && HGRivalStatReady(&sPGRival,&gNativeBattleUIState,1));
 HGNativeFirstRivalQAStatWaitObserved((void *)0x1234,13,2,0);HGNativeFirstRivalQAStatWaitObserved((void *)0x1234,38,0,0);
 assert(!sPGRival.statOwner && !sPGRival.statReady && sPGRival.statRetirements==2 && sPGRival.statRetiredSerial==2 && !sPGMap.error);
 puts("PASS separate read-only trainer setup/outcome1-or2, FIGHT/Tackle original Choose and independent wild ledger");
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
def source_copies(root,here,destination,base,first_rival=False):
    originals=base.source_copies(root,root,destination)
    field=destination/base.COPIES[0]
    previous=field.read_text()
    text=base.replace_once(previous,'#include "pokegear_qa_guest.inc"','#include "pokegear_map_card_guest.inc"')
    field.write_text(text)
    require(text.replace('#include "pokegear_map_card_guest.inc"','#include "pokegear_qa_guest.inc"',1)==
            previous,'Field include replacement drift')
    if first_rival:
        name='src/battle/battle_command.c';original=(root/name).read_text();task=base.function(original,'Task_GetExp')
        wait='''    case STATE_GET_EXP_LEVEL_UP_SUMMARY_PRINT_DIFF_WAIT:
    case STATE_GET_EXP_LEVEL_UP_SUMMARY_PRINT_TRUE_WAIT:
        if ((gSystem.newKeys & (PAD_BUTTON_A | PAD_BUTTON_B | PAD_BUTTON_X | PAD_BUTTON_Y)) || System_GetTouchNew()) {
            PlaySE(SEQ_SE_DP_SELECT);
            data->state++;
        }
        break;'''
        observed=wait.replace('        if (',base.hook('''#ifdef HG_PG_RIVAL_EXTENSION
        HGNativeFirstRivalQAStatWaitObserved(data,(u32)data->state,data->state==STATE_GET_EXP_LEVEL_UP_SUMMARY_PRINT_DIFF_WAIT ? 1 : 2,1);
#endif''')+'        if (',1)
        observed=base.replace_once(observed,'            data->state++;',base.hook('''#ifdef HG_PG_RIVAL_EXTENSION
            HGNativeFirstRivalQAStatWaitObserved(data,(u32)data->state,data->state==STATE_GET_EXP_LEVEL_UP_SUMMARY_PRINT_DIFF_WAIT ? 1 : 2,0);
#endif''')+'            data->state++;')
        changed=base.replace_once(task,wait,observed)
        changed=base.replace_once(changed,'        Heap_Free(inData);',base.hook('''#ifdef HG_PG_RIVAL_EXTENSION
        HGNativeFirstRivalQAStatWaitObserved(data,(u32)data->state,0,0);
#endif''')+'        Heap_Free(inData);')
        declaration=base.hook('''#ifdef HG_PG_RIVAL_EXTENSION
extern void HGNativeFirstRivalQAStatWaitObserved(const void *owner,u32 state,u32 kind,u32 ready);
#endif''')
        copy=base.replace_once(original,task,declaration+changed)
        restored=re.sub(r'/\* PGQA_HOOK_BEGIN \*/\n.*?\n/\* PGQA_HOOK_END \*/\n?','',copy,flags=re.S)
        require([line.strip() for line in restored.splitlines() if line.strip()]==[line.strip() for line in original.splitlines() if line.strip()], 'GetExp original body changed')
        path=destination/name;path.parent.mkdir(parents=True,exist_ok=True);path.write_text(copy);originals[name]=original
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
def elm_evidence_passed(row,checkpoint,elm,layout):
    common=dict(status='passed_original_elm_panic_call',stage=25,done=1,qaBytes=layout['stateBytes'],cards=1,scene=3,mapReadySeen=1,
        runAttemptCap=8,parentEntries=2,parentExits=2,lastApp=3,menuCloses=1,fieldReady=1,menuActive=0,privateSaveWrites=False,resetQuiescent=True)
    indoor=dict(stage=19,map=143,x=5,z=6,facing=2,routeScene=2,houseScene=1,mysteryEgg=1,pokedex=1,oakContact=1,healed=1,
        mrDone=1,manualTiles=124,parentEntries=1,parentExits=1)
    endpoint=dict(status='passed_original_elm_panic_call',stage=25,map=34,x=568,z=295,facing=1,routeScene=3,houseScene=1,
        panicFlag=1,coverage=255,mysteryEgg=1,pokedex=1,oakContact=1,callTarget=1,incoming=1,scripted=2,scriptID=2,effectType=0,
        selections=1,completions=1,hangupPresses=1)
    if any(not isinstance(actual,dict) or any(type(actual.get(k)) is not type(v) or actual[k]!=v for k,v in fixed.items())
           for actual,fixed in ((row,common),(checkpoint,indoor),(elm,endpoint))):return False
    n=row.get('battleLaunches');a=row.get('runAttempts');text=elm.get('textContinues');steps=elm.get('indoorSteps')
    return type(text) is int and 1<=text<=64 and elm.get('textSerial')==text and type(elm.get('closePresses')) is int and 2<=elm['closePresses']<=3 \
        and type(steps) is int and 302<=steps<=304 and type(row.get('steps')) is int and row['steps']>=steps \
        and type(n) is int and 0<=n<=32 and type(a) is int and all(type(row.get(k)) is int for k in ('runs','battleReturns','outcome')) \
        and row['battleReturns']==n and row['runs']==a and ((not n and not a) or n>0 and n<=a<=8*n and row['outcome']==5)
def elm_trace_passed(rows,maprows,runs,run_count,launch_count):
    prefix=[r for r in maprows if r.get('stage',99)<19]
    if not trace_passed(prefix,runs,run_count,launch_count,True) or not rows or len(rows)%3:return False
    tail=[r for r in maprows if r.get('stage',0)>=19]
    if len(tail)!=len(rows):return False
    serials=set();hangups=0;closes=[];previous=prefix[-1]['padReads']
    for i in range(0,len(rows),3):
        a,b,c=rows[i:i+3]
        if [r.get('event') for r in (a,b,c)]!=['press','release','settled']:return False
        if any(type(r.get(k)) is not int for r in (a,b,c) for k in ('stage','key','touch','owner','generation','revision','padReads')):return False
        if not previous<=a['padReads']<=b['padReads'] or c['padReads']-b['padReads']<2:return False
        if any(any(r[k]!=a[k] for k in ('stage','owner','generation','revision')) for r in (b,c)):return False
        if any(r['key'] or r['touch'] for r in (b,c)) or a['touch']:return False
        if any(any(tail[i+j].get(k)!=r[k] for k in ('event','stage','key','touch','owner','generation','revision','padReads')) for j,r in enumerate((a,b,c))):return False
        if a['owner']==0:
            if a['stage'] not in (20,21) or a['key'] not in (32,128):return False
        elif a['owner']==4:
            if a['stage']!=22 or a['key']!=1 or not a['generation'] or not a['revision'] or a.get('phase')!=6 \
                or (a.get('target'),a.get('scriptID'),a.get('incoming'),a.get('scripted'))!=(1,2,1,2) or a.get('coverage',0)&63!=63:return False
            if a.get('main')==2:hangups+=1
            elif a.get('main')==1 and a.get('scriptState')==3 and a.get('wait') in (2,3):
                serial=a.get('serial')
                if type(serial) is not int or serial!=len(serials)+1:return False
                serials.add(serial)
            else:return False
        elif a['owner']==1:
            if a['stage']!=23 or a['key']!=2 or a.get('phase')!=1 or a.get('completed')!=1 or not a['generation'] or not a['revision']:return False
            closes.append(a.get('footer'))
        else:return False
        previous=c['padReads']
    return bool(serials) and hangups==1 and 2<=len(closes)<=3 and closes[0]==0 and closes[-1]==1
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
def rival_evidence_passed(row,checkpoint,elm_checkpoint,elm,rival,layout):
    if not isinstance(row,dict) or not isinstance(elm_checkpoint,dict):return False
    if not elm_evidence_passed({**elm_checkpoint,'resetQuiescent':row.get('resetQuiescent')},checkpoint,elm,layout):return False
    common=dict(status='passed_original_first_rival',stage=31,done=1,qaBytes=layout['stateBytes'],cards=1,scene=4,mapReadySeen=1,
        runAttemptCap=8,parentEntries=2,parentExits=2,lastApp=3,menuCloses=1,fieldReady=1,menuActive=0,privateSaveWrites=False,resetQuiescent=True)
    endpoint=dict(status='passed_original_first_rival',stage=31,qaBytes=120,done=1,map=67,x=575,z=399,facing=2,routeScene=3,houseScene=1,
        scriptSeen=1,trainerLaunches=1,trainerReturns=1,battleType=0x801,battleSpecial=0,trainer=496,**{'class':119},
        opponentSpecies=155,opponentLevel=5,playerSpecies=152,playerLevel=5,metPasserby=1,rivalHidden=1,battleLive=0,mysteryEgg=1,pokedex=1,oakContact=1)
    if not isinstance(rival,dict) or any(any(type(actual.get(k)) is not type(v) or actual[k]!=v for k,v in fixed.items()) for actual,fixed in ((row,common),(rival,endpoint))):return False
    n=row.get('battleLaunches');a=row.get('runAttempts');outcome=rival.get('outcome');choices=rival.get('choices');steps=rival.get('elmSteps')
    return type(outcome) is int and outcome in (1,2) and type(rival.get('battleResult')) is int and rival['battleResult']==outcome \
        and type(choices) is int and 2<=choices<=64 and choices%2==0 and type(steps) is int and steps==elm_checkpoint.get('steps') \
        and type(row.get('steps')) is int and row['steps']==steps+210 and type(n) is int and elm_checkpoint['battleLaunches']<=n<=32 \
        and type(a) is int and elm_checkpoint['runAttempts']<=a and all(type(row.get(k)) is int for k in ('runs','battleReturns','outcome')) \
        and row['battleReturns']==n and row['runs']==a and ((not n and not a) or n>0 and n<=a<=8*n and row['outcome']==5) \
        and all(type(rival.get(k)) is int for k in ('statOwner','statKind','statReady','statSerial','statRetiredSerial','statWaits','statRetirements','statPresses','statSettles')) \
        and not rival['statOwner'] and not rival['statKind'] and not rival['statReady'] \
        and all(rival[k]==(2 if outcome==1 else 0) for k in ('statSerial','statRetiredSerial','statWaits','statRetirements','statPresses','statSettles'))
def rival_choices_passed(rows,launches,returns,rival):
    identity=dict(trainer=496,**{'class':119},battleType=0x801,battleSpecial=0,opponentSpecies=155,opponentLevel=5,playerSpecies=152,playerLevel=5)
    returned=dict(launches=1,returns=1,outcome=rival.get('outcome'),battleResult=rival.get('battleResult'))
    if len(launches)!=1 or len(returns)!=1 or any(not isinstance(actual,dict) or any(type(actual.get(k)) is not type(v) or actual[k]!=v for k,v in expected.items()) for actual,expected in ((launches[0],identity),(returns[0],returned))):return False
    if len(rows)!=rival.get('choices') or not 2<=len(rows)<=64 or len(rows)%2:return False
    revisions=set()
    for i,row in enumerate(rows):
        if not isinstance(row,dict) or any(type(row.get(k)) is not int for k in ('sequence','revision','mode','enabledMask','index','move','pp')):return False
        if row['sequence']!=i+1 or row['mode']!=1+i%2 or row['index'] or not row['enabledMask']&1 or not row['revision'] or row['revision'] in revisions:return False
        if row['mode']==2 and (row['move']!=33 or row['pp']<=0):return False
        revisions.add(row['revision'])
    return True
def rival_trace_passed(rows,elmrows,runs,elm_checkpoint):
    prefix=[r for r in rows if r.get('stage',99)<=25];tail=[r for r in rows if r.get('stage',0)>=26]
    if not elm_trace_passed(elmrows,prefix,runs[:elm_checkpoint['runAttempts']],elm_checkpoint['runAttempts'],elm_checkpoint['battleLaunches']) or not tail or len(tail)%3:return False
    previous=prefix[-1]['padReads'];serials=set();south=city=text=False
    for i in range(0,len(tail),3):
        a,b,c=tail[i:i+3]
        if [r.get('event') for r in (a,b,c)]!=['press','release','settled'] or any(type(r.get(k)) is not int for r in (a,b,c) for k in ('stage','key','touch','owner','generation','revision','padReads')):return False
        if not previous<=a['padReads']<=b['padReads'] or c['padReads']-b['padReads']<2 or any(r['key'] or r['touch'] for r in (b,c)):return False
        if any(any(r[k]!=a[k] for k in ('stage','owner','generation','revision')) for r in (b,c)) or a['touch'] or a['generation'] or a['revision']:return False
        if a['owner']==0:
            if a['stage'] not in (27,28) or a['key'] not in (16,32,64,128):return False
            south|=a['stage']==27;city|=a['stage']==28
        elif a['owner']==5:
            if a['stage']!=29 or a['key']!=1:return False
        elif a['owner']==3:
            if a['stage']!=29 or a['key']!=1 or a.get('map')!=67 or a.get('scene')!=3 or a.get('script')!=4 or a.get('wait') not in (2,3):return False
            serial=a.get('serial')
            if type(serial) is not int or serial<=0 or serial in serials:return False
            serials.add(serial);text=True
        else:return False
        previous=c['padReads']
    return south and city and text
def rival_ack_passed(rows,diagnostics,rival):
    if type(rival.get('outcome')) is not int or rival['outcome'] not in (1,2):return False
    count=2 if rival.get('outcome')==1 else 0
    planes=[r for r in diagnostics if r.get('label') in ('stat-1','stat-2')]
    if len(rows)!=count*3 or len(planes)!=count:return False
    owner=None
    for i,plane in enumerate(planes):
        kind=i+1;state=11+2*i;serial=i+1
        fixed=dict(label='stat-'+str(kind),kind=kind,state=state,serial=serial,ready=1,uiMode=0,uiReady=0,frameAttempt=1,frameCaptured=1)
        if any(type(plane.get(k)) is not type(v) or plane[k]!=v for k,v in fixed.items()) or type(plane.get('owner')) is not int or not plane['owner']:return False
        if owner is not None and plane['owner']!=owner:return False
        owner=plane['owner'];a,b,c=rows[i*3:i*3+3]
        if [r.get('event') for r in (a,b,c)]!=['press','release','settled']:return False
        for row in (a,b,c):
            if any(type(row.get(k)) is not int for k in ('stage','ack','key','owner','state','kind','serial','retiredSerial','padReads')):return False
            if any(row[k]!=v for k,v in dict(stage=29,ack=kind,owner=owner,state=state,kind=kind,serial=serial).items()):return False
        if a['key']!=1 or b['key'] or c['key'] or a['retiredSerial']!=i or b['retiredSerial']!=serial or c['retiredSerial']!=serial:return False
        if not a['padReads']<=b['padReads'] or c['padReads']-b['padReads']<2:return False
    return True
def rival_stat_files_passed(folder,diagnostics):
    finish=False
    for row in diagnostics:
        label=row.get('label')
        if label not in ('stat-1','stat-2','finish'):return False
        state=folder/('rival-'+label+'-state.bin');ui=folder/('rival-'+label+'-ui.bin')
        if any(not p.is_file() or p.is_symlink() for p in (state,ui)) or state.stat().st_size!=120 or ui.stat().st_size!=row.get('uiBytes'):return False
        packet=struct.unpack('<30I',state.read_bytes())
        if packet[:2]!=(2,120) or packet[3] or packet[22:27]!=(row.get('owner'),row.get('state'),row.get('kind'),row.get('ready'),row.get('serial')):return False
        if row.get('frameAttempt')!=1 or row.get('frameCaptured')!=1:return False
        finish|=label=='finish'
    return finish

def rival_contract_self_check(layout):
    row=dict(status='passed_original_first_rival',stage=31,done=1,qaBytes=layout['stateBytes'],cards=1,scene=4,mapReadySeen=1,runAttemptCap=8,
        parentEntries=2,parentExits=2,lastApp=3,menuCloses=1,fieldReady=1,menuActive=0,privateSaveWrites=False,resetQuiescent=True,
        battleLaunches=0,battleReturns=0,runs=0,runAttempts=0,outcome=0,steps=518)
    checkpoint=dict(stage=19,map=143,x=5,z=6,facing=2,routeScene=2,houseScene=1,mysteryEgg=1,pokedex=1,oakContact=1,healed=1,mrDone=1,manualTiles=124,parentEntries=1,parentExits=1)
    before={**row,'status':'passed_original_elm_panic_call','stage':25,'scene':3,'steps':308}
    elm=dict(status='passed_original_elm_panic_call',stage=25,map=34,x=568,z=295,facing=1,routeScene=3,houseScene=1,panicFlag=1,coverage=255,
        mysteryEgg=1,pokedex=1,oakContact=1,callTarget=1,incoming=1,scripted=2,scriptID=2,effectType=0,selections=1,completions=1,
        hangupPresses=1,textContinues=2,textSerial=2,closePresses=3,indoorSteps=304)
    rival=dict(status='passed_original_first_rival',stage=31,qaBytes=120,done=1,map=67,x=575,z=399,facing=2,routeScene=3,houseScene=1,scriptSeen=1,
        trainerLaunches=1,trainerReturns=1,battleType=0x801,battleSpecial=0,trainer=496,**{'class':119},opponentSpecies=155,opponentLevel=5,
        playerSpecies=152,playerLevel=5,outcome=1,battleResult=1,metPasserby=1,rivalHidden=1,battleLive=0,mysteryEgg=1,pokedex=1,oakContact=1,choices=2,elmSteps=308,
        statOwner=0,statKind=0,statReady=0,statSerial=2,statRetiredSerial=2,statWaits=2,statRetirements=2,statPresses=2,statSettles=2)
    require(rival_evidence_passed(row,checkpoint,before,elm,rival,layout),'Original rival win contract rejected')
    loss={**rival,'outcome':2,'battleResult':2,**dict.fromkeys(('statSerial','statRetiredSerial','statWaits','statRetirements','statPresses','statSettles'),0)}
    require(rival_evidence_passed(row,checkpoint,before,elm,loss,layout),'Original rival loss contract rejected')
    for key,value in (('battleType',1),('battleSpecial',1),('trainer',495),('outcome',5),('battleResult',2),('metPasserby',0),('rivalHidden',0),('trainerReturns',0),('battleLive',1),('x',574),('choices',65)):
        require(not rival_evidence_passed(row,checkpoint,before,elm,{**rival,key:value},layout),'False rival evidence accepted: '+key)
    require(not rival_evidence_passed(row,checkpoint,before,{**elm,'coverage':127},rival,layout),'Lost original Elm endpoint admitted')
    launches=[dict(trainer=496,**{'class':119},battleType=0x801,battleSpecial=0,opponentSpecies=155,opponentLevel=5,playerSpecies=152,playerLevel=5)]
    returns=[dict(launches=1,returns=1,outcome=1,battleResult=1)]
    choices=[dict(sequence=1,revision=10,mode=1,enabledMask=1,index=0,move=0,pp=0),dict(sequence=2,revision=11,mode=2,enabledMask=1,index=0,move=33,pp=35)]
    require(rival_choices_passed(choices,launches,returns,rival),'Original FIGHT/Tackle revision sequence rejected')
    for key,value in (('revision',10),('index',3),('mode',1),('move',45),('pp',0)):
        require(not rival_choices_passed([choices[0],{**choices[1],key:value}],launches,returns,rival),'False rival choice admitted: '+key)
    require(not rival_choices_passed(choices,launches,[],rival),'Missing trainer deletion witness admitted')
    acks=[];planes=[]
    for i,state in enumerate((11,13)):
        kind=serial=i+1;start=100+i*10
        planes.append(dict(label='stat-'+str(kind),owner=0x1234,state=state,kind=kind,serial=serial,ready=1,uiMode=0,uiReady=0,frameAttempt=1,frameCaptured=1))
        first=dict(event='press',stage=29,ack=kind,key=1,owner=0x1234,state=state,kind=kind,serial=serial,retiredSerial=i,padReads=start)
        acks.extend([first,{**first,'event':'release','key':0,'retiredSerial':serial,'padReads':start+1},{**first,'event':'settled','key':0,'retiredSerial':serial,'padReads':start+3}])
    require(rival_ack_passed(acks,planes,rival) and rival_ack_passed([],[],loss),'Original stat acknowledgement/loss contracts rejected')
    for key,value in (('owner',0),('state',13),('kind',2),('serial',0),('key',2)):
        bad=[dict(r) for r in acks];bad[0][key]=value;require(not rival_ack_passed(bad,planes,rival),'False stat acknowledgement admitted: '+key)
    bad=[dict(r) for r in acks];bad[1]['retiredSerial']=0;require(not rival_ack_passed(bad,planes,rival),'Unretired stat wait admitted')
    bad=[dict(r) for r in acks];bad[2]['padReads']=bad[1]['padReads']+1;require(not rival_ack_passed(bad,planes,rival),'Missing released reads admitted')
    require(not rival_ack_passed(acks,[{**planes[0],'uiMode':1},planes[1]],rival),'Battle command A spam admitted')
    require(not rival_ack_passed([],[],rival),'Missing original stat-wait witness admitted')
    rows=[];elmrows=[]
    def pulse(stage,owner,key,**facts):
        start=len(rows)*10+10
        a=dict(event='press',stage=stage,owner=owner,key=key,touch=0,generation=9 if owner else 0,revision=3 if owner else 0,padReads=start,**facts)
        if owner==3:a.update(generation=0,revision=0)
        triple=[a,{**a,'event':'release','key':0,'padReads':start+4},{**a,'event':'settled','key':0,'padReads':start+6}]
        rows.extend(triple)
        if 19<=stage<=25:elmrows.extend(triple)
    for stage,script,map_id in ((2,2,67),(4,3,67),(14,3,34),(17,2,143)):
        pulse(stage,3,1,script=script,serial=stage,wait=2,map=map_id)
    pulse(20,0,128)
    pulse(22,4,1,phase=6,main=1,scriptState=3,wait=2,serial=1,target=1,scriptID=2,incoming=1,scripted=2,coverage=63)
    pulse(22,4,1,phase=6,main=2,target=1,scriptID=2,incoming=1,scripted=2,coverage=63)
    pulse(23,1,2,phase=1,completed=1,footer=0)
    pulse(23,1,2,phase=1,completed=1,footer=1)
    pulse(27,0,32);pulse(28,0,16)
    pulse(29,3,1,map=67,scene=3,script=4,wait=2,serial=70)
    require(rival_trace_passed(rows,elmrows,[],before),'Original Elm prefix and rival text/release trace rejected')
    for key,value in (('script',2),('scene',4),('map',34),('serial',0),('owner',4)):
        bad=[dict(r) for r in rows];bad[-3][key]=value
        require(not rival_trace_passed(bad,elmrows,[],before),'False rival text lease trace admitted: '+key)

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
    if args.elm_panic_call:flags.append('-DHG_PG_ELM_EXTENSION')
    if args.first_rival:flags.append('-DHG_PG_RIVAL_EXTENSION')
    private=[];private_pins={};replacements=[];new_objects=[];old_objects=[]
    try:
        require_hashes(pins,'Map source inputs')
        for name in ((*base.COPIES,'src/battle/battle_command.c') if args.first_rival else base.COPIES):
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
        if args.elm_panic_call:cflags.append('-DHG_PG_ELM_EXTENSION')
        if args.first_rival:cflags.append('-DHG_PG_RIVAL_EXTENSION')
        for i,source in enumerate([*generated,*runtime,here/QA_PATHS[2]]):
            target=native/f'host-{i}.o';extra=['-DWASM_RT_TRAP_HANDLER=HGEngineTrap'] if source.name=='wasm-rt-impl.c' else [];runner(['xcrun','clang',*cflags,*extra,'-c',source,'-o',target],native/f'compile-{i}.log');native_objects.append(target)
        binary=native/'map-card-check';runner(['xcrun','clang','-fsanitize=address,undefined',*native_objects,'-o',binary],native/'link.log');report['hostABICompiled']=True
        boot=out/'boot-header.bin';boot.write_bytes(h['read_boot_header'](context['pack']));frames=out/'frames';frames.mkdir()
        for p in fixture:copy=out/('scenario.sav'+p.name[len(save.name):]);shutil.copy2(p,copy);private.append(copy)
        private_pins={str(p):sha(p) for p in private};assert_unchanged(context);require_hashes(pins,'Map source inputs');require_hashes(fixture_pins,'Original fixture')
        report.update(gameExecuted=True,gameRuntimeExecuted=True,sourceCopiesCompiled=replacements,privateModuleSha256=sha(module),checkedStackWrites=writes,exceptionDepthRestores=restores)
        row,text=runner([binary,context['pack']/'files',boot,out/'scenario.sav',frames],out/'runtime.log',timeout=900 if args.first_rival else 600 if args.mr_pokemon else 300,check=False)
        rows={p:[] for p in ('MAP evidence: ','MAP baseline: ','MAP input: ','MAP run: ','MAP snapshot: ','POKEGEAR snapshot: ','MR evidence: ','MR snapshot: ')}
        if args.elm_panic_call:rows.update({p:[] for p in ('ELM checkpoint: ','ELM evidence: ','ELM input: ')})
        if args.first_rival:rows.update({p:[] for p in ('RIVAL elm checkpoint: ','RIVAL evidence: ','RIVAL launch: ','RIVAL return: ','RIVAL choice: ','RIVAL ack input: ','RIVAL diagnostic: ')})
        for line in text.splitlines():
            for prefix,destination in rows.items():
                if line.startswith(prefix):destination.append(json.loads(line[len(prefix):]));break
        report['runtimeEvidence']=rows
        if args.first_rival:
            accepted=all(len(rows[p])==1 for p in ('MAP evidence: ','ELM checkpoint: ','RIVAL elm checkpoint: ','ELM evidence: ','RIVAL evidence: ')) and rival_evidence_passed(rows['MAP evidence: '][0],rows['ELM checkpoint: '][0],rows['RIVAL elm checkpoint: '][0],rows['ELM evidence: '][0],rows['RIVAL evidence: '][0],report['nativeLayout'])
        elif args.elm_panic_call:
            accepted=len(rows['MAP evidence: '])==len(rows['ELM checkpoint: '])==len(rows['ELM evidence: '])==1 and elm_evidence_passed(rows['MAP evidence: '][0],rows['ELM checkpoint: '][0],rows['ELM evidence: '][0],report['nativeLayout'])
        else:
            accepted=len(rows['MAP evidence: '])==1 and (len(rows['MR evidence: '])==1 and mr_evidence_passed(rows['MAP evidence: '][0],rows['MR evidence: '][0],report['nativeLayout']) if args.mr_pokemon else evidence_passed(rows['MAP evidence: '][0],report['nativeLayout']))
        require(row['terminalReturnCode']==0 and not row['timedOut'] and row['allChildrenWaited'] and accepted,'Original Map/Mr flow failed; no automatic retry')
        require(rows['MAP baseline: ']==[dict(map=33,x=668,z=397,scene=0,cards=0)],'Fresh actual Map baseline mismatch')
        if args.first_rival:
            require(rival_trace_passed(rows['MAP input: '],rows['ELM input: '],rows['MAP run: '],rows['RIVAL elm checkpoint: '][0]),'Qualified rival story/release trace mismatch')
            require(trace_passed([r for r in rows['MAP input: '] if r.get('stage',99)<19],rows['MAP run: '],rows['MAP evidence: '][0]['runAttempts'],rows['MAP evidence: '][0]['battleLaunches'],True),'Original wild RUN ledger mismatch')
            require(rival_choices_passed(rows['RIVAL choice: '],rows['RIVAL launch: '],rows['RIVAL return: '],rows['RIVAL evidence: '][0]),'Trainer launch/FIGHT/Tackle/deletion evidence mismatch')
            require(rival_ack_passed(rows['RIVAL ack input: '],rows['RIVAL diagnostic: '],rows['RIVAL evidence: '][0]),'Original stat-wait A acknowledgement evidence mismatch')
            require(rival_stat_files_passed(out,rows['RIVAL diagnostic: ']),'Actual stat/final UI and private packets missing')
            require(any(r.get('stage')==31 and r.get('scene')==4 and not r.get('script') for r in rows['MAP snapshot: ']),'Rival final field snapshot missing')
        elif args.elm_panic_call:require(elm_trace_passed(rows['ELM input: '],rows['MAP input: '],rows['MAP run: '],rows['MAP evidence: '][0]['runAttempts'],rows['MAP evidence: '][0]['battleLaunches']),'Qualified Elm phone/release trace mismatch')
        else:require(trace_passed(rows['MAP input: '],rows['MAP run: '],rows['MAP evidence: '][0]['runAttempts'],rows['MAP evidence: '][0]['battleLaunches'],args.mr_pokemon),'Qualified script/RUN/release trace mismatch')
        require(snapshots_passed(frames,rows['POKEGEAR snapshot: '],rows['MAP snapshot: '],report['nativeLayout'],rows['MR snapshot: '] if args.mr_pokemon else None),'Original paired snapshot files mismatch')
        require(private_pins=={str(p):sha(p) for p in private},'Unexpected private4 disk change')
        report.update(passed=True,complete=True,status='passed_original_mr_pokemon' if args.mr_pokemon else 'passed_original_map_card',privateSaveFilesUnchanged=True,originalFixtureUnchanged=True,allCardsComplete=False,
                      limits=['Radio/Fly/Town/EasyChat positive flows remain excluded.','Original story and encounter effects are RAM changes; no disk persistence is claimed.','Original PPM visual review remains a separate post-run step.'])
        if args.elm_panic_call:report.update(status='passed_original_elm_panic_call',indoorCheckpointObserved=True,elmTerminalObserved=True)
        if args.first_rival:report.update(status='passed_original_first_rival',rivalTerminalObserved=True,originalTrainerOutcome=rows['RIVAL evidence: '][0]['outcome'])
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
    parser.add_argument('--elm-panic-call',action='store_true',help='Continue the original MrPokemon endpoint through the natural exit and Elm panic call')
    parser.add_argument('--first-rival',action='store_true',help='Continue Elm through the natural south route and original first-rival trainer battle')
    args=parser.parse_args();root=args.source_root.resolve();here=ROOT;out=args.output.resolve()
    if args.first_rival:args.elm_panic_call=True
    if args.elm_panic_call:args.mr_pokemon=True
    require(bool(args.full_receipt)==bool(args.save),'Runtime requires both explicit --full-receipt and --save')
    require(not out.exists(),'Fresh output required');out.mkdir(parents=True)
    base=load(root/'ios/tools/check_native_pokegear_qa.py')
    source_names=(*base.COPIES,'ios/tools/check_native_pokegear_qa.py','ios/Original/abi32/pokegear_scenario_check.c',
        'ios/tests/pokegear_qa.h','ios/tests/pokegear_qa_guest.inc','ios/tools/check_native_save_roundtrip.py','src/player_avatar_movement.c','src/field/field_control.c',
        'src/field/scrcmd_message.c','src/script_manager.c','src/encounter.c','src/battle/battle_input.c','src/battle/battle_util.c','src/battle/battle_command.c','files/fielddata/encountdata/gs_enc_data.json',
        'files/fielddata/script/scr_seq/scr_seq_0850_T21.s','files/fielddata/eventdata/zone_event/064_T21.json',
        'files/battledata/script/subscript/subscript_0003_RunAway.s','files/battledata/script/subscript/subscript_0008_RunAwayFail.s','include/native_pokegear_ui.h','include/native_battle_ui.h')
    if args.mr_pokemon:source_names+=('files/fielddata/script/scr_seq/scr_seq_0227_R30.s','files/fielddata/script/scr_seq/scr_seq_0229_R30R0201.s','include/constants/vars.h','include/constants/items.h','include/constants/phone_contacts.h','include/save_pokegear.h','include/party.h','include/pokedex.h','include/bag.h')
    if args.elm_panic_call:source_names+=('src/application/pokegear/phone/phone_call.c','src/application/pokegear/phone/pokegear_phone.c','src/application/pokegear/phone/phone_script_defs.c','src/application/pokegear/phone/scripts/phone_scripts_prof_elm.c','include/application/pokegear/phone/phone_internal.h')
    if args.first_rival:source_names+=('include/constants/trainers.h','include/constants/trainer_class.h','include/battle/battle_setup.h','src/battle/battle_setup.c','files/poketool/trainer/trainers.json','asm/macros/script.inc')
    pins={str(p):sha(p) for p in [*(here/p for p in QA_PATHS),*(root/p for p in source_names)]}
    report=dict(passed=False,complete=False,commands=[],sourceHashesBefore=pins,moduleBuilt=False,gameExecuted=False,gameRuntimeExecuted=False,observerDefinitionsLinked=False,hostABICompiled=False,
                audioOperations=False,privateExports=['HGNativePokegearQAGetState','HGNativePokegearQARequest'],futureExports=35,shippingABIChanged=False,
                requiredPrivateLinkWraps=list(LINK_WRAPS),Phone33DefaultCaseUnchanged=True,allCardsComplete=False)
    if args.mr_pokemon:report.update(privateExports=[*report['privateExports'],'HGNativeMrPokemonQAGetState'],futureExports=36,mrPokemonMode=True,legacyMapPacketUnchanged=True)
    if args.elm_panic_call:report.update(elmPanicCallMode=True,legacyMrPacketUnchanged=True,existingPhonePrefixReused=True)
    if args.first_rival:report.update(privateExports=[*report['privateExports'],'HGNativeFirstRivalQAGetState'],futureExports=37,firstRivalMode=True,rivalPrivateStateBytes=120,rivalPrivateVersion=2,legacyElmEndpointRetained=True,runtimeBudgetSeconds=900)
    env={**os.environ,'HEARTGOLD_TEST_MUTE':'1','SIMCTL_CHILD_HEARTGOLD_TEST_MUTE':'1','PYTHONDONTWRITEBYTECODE':'1'}
    def run(argv,label):
        result=subprocess.run(['nice','-n','10',*map(str,argv)],cwd=root,env=env,capture_output=True,text=True,timeout=60)
        (out/label).write_text(result.stdout+result.stderr)
        report['commands'].append(dict(argv=list(map(str,argv)),terminalReturnCode=result.returncode,log=label,allChildrenWaited=True))
        require(result.returncode==0,'Failed '+str(out/label));return result.stdout+result.stderr
    try:
        copies=out/'source';copies.mkdir();originals=source_copies(root,here,copies,base,args.first_rival)
        host=(here/QA_PATHS[2]).read_text();(out/'actual_map_settle.inc').write_text(base.function(host,'mapSettle')+'\n')
        guest=(here/QA_PATHS[1]).read_text();(out/'actual_map_battle.inc').write_text('\n'.join(base.function(guest,name) for name in ('__wrap_Battle_LaunchApp','__wrap_BattleSetup_Delete'))+'\n')
        (out/'actual_map_request.inc').write_text(base.function(guest,'HGNativePokegearQARequest')+'\n')
        if args.mr_pokemon:
            (out/'actual_mr_running.inc').write_text(base.function(guest,'PGMrRunning')+'\n')
            (out/'actual_mr_context.inc').write_text(base.function(guest,'PGMrTextContext')+'\n')
        if args.elm_panic_call:
            (out/'actual_elm_running.inc').write_text(base.function(guest,'PGElmRunning')+'\n')
            (out/'actual_elm_owner.inc').write_text('\n'.join(base.function(guest,name) for name in ('PGElmCallOwner','PGElmCallRootIdentity'))+'\n')
        if args.first_rival:
            (out/'actual_rival_running.inc').write_text(base.function(guest,'PGRivalRunning')+'\n')
            (out/'actual_rival_next.inc').write_text(base.function((root/'ios/tests/pokegear_qa_guest.inc').read_text(),'PGQANext')+'\n')
            (out/'actual_rival_stat_wait.inc').write_text(base.function(guest,'HGNativeFirstRivalQAStatWaitObserved')+'\n')
            exp_enum=re.search(r'enum \{\n    STATE_GET_EXP_START = 0,.*?\n\};',(root/'src/battle/battle_command.c').read_text(),re.S)
            require(exp_enum is not None,'Original GetExp enum missing');(out/'actual_rival_exp_enum.inc').write_text(exp_enum.group()+'\n')
        (out/'actual_native_battle_choose.inc').write_text(base.function((root/'src/battle/battle_input.c').read_text(),'HGNativeBattleUIChoose')+'\n')
        check=out/'actual_map_check.c';check.write_text(C_CHECK)
        includes=[here/'ios/tests',root/'ios/tests',root/'include',root/'ios/Original/abi32',root/'ios/build-original-host/include',out]
        tools=Path(os.environ.get('HG_ABI32_TOOLS',root/'ios/build-original-abi32/tools'))
        native=['xcrun','clang','-std=c11','-O1','-g','-fms-extensions','-Wno-microsoft','-Wno-unknown-pragmas','-fsanitize=address,undefined','-fno-sanitize-recover=all','-DSDK_ARM9','-DHEARTGOLD_NATIVE']
        if args.mr_pokemon:native.append('-DHG_PG_MR_EXTENSION')
        if args.elm_panic_call:native.append('-DHG_PG_ELM_EXTENSION')
        if args.first_rival:native.append('-DHG_PG_RIVAL_EXTENSION')
        for p in includes:native+=['-iquote',p]
        native+=['-I',tools/'wabt-1.0.42/include','-idirafter',root/'ios/build-original-host/include']
        for p in ('lib/include','include/library','lib/include/cw','lib/include/nitro','lib/include/nnsys','lib/include/nitro/os','lib/NitroSDK/include','lib/NitroSDK/include-pokediamond','files'):native+=['-idirafter',root/p]
        run([*native,check,'-o',out/'actual-map-check'],'native-compile.log');log=run([out/'actual-map-check'],'native-run.log')
        layouts=[json.loads(line[11:]) for line in log.splitlines() if line.startswith('MAP_LAYOUT ')];require(len(layouts)==1,'Real native layout missing');report['nativeLayout']=layouts[0];evidence_self_check(layouts[0]);trace_self_check()
        require(report['futureExports']==33+len(report['privateExports']) and len(set(report['privateExports']))==len(report['privateExports']),'Mode-specific private ABI changed')
        if args.mr_pokemon:mr_contract_self_check(layouts[0])
        if args.first_rival:rival_contract_self_check(layouts[0])
        flags=['--target=wasm32-wasip1','-std=gnu11','-O2','-fwrapv','-fno-strict-aliasing','-fno-builtin-malloc','-fno-builtin-free','-fwasm-exceptions','-fexceptions','-Xclang','-disable-red-zone','-fms-extensions','-Wno-microsoft','-Wno-unknown-pragmas','-DHEARTGOLD_NATIVE','-DHEARTGOLD_ABI32','-DSDK_ARM9','-DSDK_CW','-DSDK_CODE_ARM','-DSDK_TS','-D_NITRO','-DPM_KEEP_ASSERTS','-DPLATFORM_INTRINSIC_FUNCTION_BIT_CLZ32=__builtin_clz']
        if args.mr_pokemon:flags.append('-DHG_PG_MR_EXTENSION')
        if args.elm_panic_call:flags.append('-DHG_PG_ELM_EXTENSION')
        if args.first_rival:flags.append('-DHG_PG_RIVAL_EXTENSION')
        for p in includes:flags+=['-I',p]
        for p in ('files','src','lib/include','lib/include/cw','lib/include/nitro','lib/include/nnsys','lib/include/nitro/os','include/library','lib/NitroSDK/include','lib/NitroSDK/include-pokediamond'):flags+=['-I',root/p]
        for i,name in enumerate((*base.COPIES,'src/battle/battle_command.c') if args.first_rival else base.COPIES):run([args.wasi_sdk/'bin/clang',*flags,'-fsyntax-only',copies/name],f'wasi-{i}.log')
        layout=out/'layout.c';layout.write_text('#include "global.h"\n#include "pokegear_map_card_qa.h"\n_Static_assert(sizeof(HGPokegearMapCardQAState)=='+str(layouts[0]['stateBytes'])+',"Real WASI private packet/native host layout");\n_Static_assert(__builtin_offsetof(HGPokegearMapCardQAState,version)=='+str(layouts[0]['prefixBytes'])+',"Original prefix ABI");\n')
        if args.mr_pokemon:layout.write_text(layout.read_text()+'_Static_assert(sizeof(HGPokegearMrPokemonQAState)==72,"Separate Mr packet WASI/native layout");\n')
        if args.first_rival:layout.write_text(layout.read_text()+'_Static_assert(sizeof(HGPokegearFirstRivalQAState)==120,"Separate rival stat-wait packet WASI/native layout");\n')
        run([args.wasi_sdk/'bin/clang',*flags,'-Xclang','-fdump-record-layouts','-fsyntax-only',layout],'wasi-layout.log')
        report['actualWASI32LayoutVerified']=True
        support=(root/'ios/Original/abi32/pokegear_scenario_check.c').read_text();anchor='int main(int argc,char **argv)'
        require(support.count(anchor)==1,'Phone33 main source identity changed')
        (out/'pokegear_map_phone_support.inc').write_text(support.replace(anchor,'static int PGMapUnusedPhoneMain(int argc,char **argv)',1))
        if args.host_generated:
            require('w2c_hgengine_HGNativePokegearQAGetState' in (args.host_generated/'hgengine.h').read_text(),'Genuine private header lacks original private getters')
            if args.mr_pokemon:require('w2c_hgengine_HGNativeMrPokemonQAGetState(' in (args.host_generated/'hgengine.h').read_text(),'Mr mode requires its genuine private36 header, not the prior Map header')
            if args.first_rival:require('w2c_hgengine_HGNativeFirstRivalQAGetState(' in (args.host_generated/'hgengine.h').read_text(),'Rival mode requires its genuine private37 header')
            hostflags=['xcrun','clang','-std=c11','-fsyntax-only','-fms-extensions','-Wno-microsoft','-Wno-unknown-pragmas','-DSDK_ARM9','-DHEARTGOLD_NATIVE','-DWASM_RT_USE_MMAP=0','-DWASM_RT_MEMCHECK_GUARD_PAGES=0','-DWASM_RT_MEMCHECK_BOUNDS_CHECK=1']
            if args.mr_pokemon:hostflags.append('-DHG_PG_MR_EXTENSION')
            if args.elm_panic_call:hostflags.append('-DHG_PG_ELM_EXTENSION')
            if args.first_rival:hostflags.append('-DHG_PG_RIVAL_EXTENSION')
            for p in [args.host_generated,*includes,tools/'wabt-1.0.42/include',tools/'wabt-1.0.42/share/wabt/wasm2c']:hostflags+=['-I',p]
            for p in ('lib/include','include/library'):hostflags+=['-idirafter',root/p]
            run([*hostflags,here/QA_PATHS[2]],'genuine-prior-header-host-syntax.log')
            report['hostSyntaxWithGenuineMr36Header' if args.mr_pokemon else 'hostSyntaxWithGenuinePrior33Header']=True;report['priorGeneratedHeaderSha256']=sha(args.host_generated/'hgengine.h')
        report.update(passed=True,status='passed_map_source_contracts',originalBodiesPreserved=True,originalSourceCopies=list((*base.COPIES,'src/battle/battle_command.c') if args.first_rival else base.COPIES),
                      sourceCopyHashes={name:sha(copies/name) for name in ((*base.COPIES,'src/battle/battle_command.c') if args.first_rival else base.COPIES)},actualNativeRouteAndLeaseCheck=True,actualReadonlyBattleWrappersCheck=True,actualOriginalChooseRetrySequenceCheck=True,runAttemptCap=8,
                      limits=['No Map private module linked, so linker wrappers and generated Map ABI remain unproved.','No game, route/grass/guide/Map images or encounter runtime acceptance.','Original wild damage/RNG/seen/stats changes are allowed; Phone33 immutable RAM ledger is deliberately not reused.','Original and private disk nonmutation must be checked in a future authorized runtime driver.'])
        if args.mr_pokemon:report.update(status='passed_mr_source_contracts',mrPrivateStateBytes=72,actualMrPredicateLeaseAndRunContinuationCheck=True,
            staticManualTiles=124,staticGrassTiles=39,indoorEndpoint=dict(map=143,x=5,z=6,facing=2),
            limits=[*report['limits'],'Mr route/BDHC, random encounter survival, automatic scripts and indoor gifts remain runtime-unproved.','Party HP/status is observed; full PP restoration and the Elm call after exiting are outside this endpoint.'])
        if args.elm_panic_call:report.update(status='passed_elm_source_contracts',actualElmContextLeaseExitCheck=True,
            limits=['Source/predicate checks only; no natural exit/Phone call/runtime/pixel/current Full acceptance is claimed.','Existing Mr72 and Phone prefix/getters are reused; a genuine fresh private36/native host build remains required.'])
        if args.first_rival:report.update(status='passed_rival_source_contracts',actualRivalPredicateChoiceAndReadonlyWitnessCheck=True,
            limits=['Source/predicate checks only; natural south route, trainer battle, original outcome and departure remain runtime-unproved.','Only the opt-in rival case adds a separate private getter; public33, Map5620, Mr72 and existing private36 cases are unchanged.'])
        if args.full_receipt:execute(root,here,args,out,copies,report,pins,base)
    except BaseException as error:report.update(passed=False,firstFailure=dict(type=type(error).__name__,message=str(error)))
    finally:
        report['sourceInputsUnchanged']=pins=={p:sha(p) for p in pins};report['passed'] &= report['sourceInputsUnchanged']
        (out/'report.json').write_text(json.dumps(report,indent=2,sort_keys=True)+'\n')
    print(('PASS' if report['passed'] else 'FAIL')+': '+str(out/'report.json'));return 0 if report['passed'] else 1
if __name__=='__main__':raise SystemExit(main())
