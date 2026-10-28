#!/usr/bin/env python3
"""Source-copy/real-C proof for the Pokegear QA draft; never links or runs a game."""
import argparse,copy,hashlib,importlib.util,json,os,re,shlex,shutil,struct,subprocess,sys,tempfile,types
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2]
sys.dont_write_bytecode=True
QA_PATHS=('ios/tests/pokegear_qa.h','ios/tests/pokegear_qa_guest.inc','ios/Original/abi32/pokegear_scenario_check.c','ios/tools/check_native_pokegear_qa.py')
C_LEDGER_CHECK=r'''
// SPDX-License-Identifier: GPL-3.0-or-later
#include "application/pokegear/phone/phone_internal.h"
#include "constants/items.h"
#include "constants/maps.h"
#include "math_util.h"
#include "sys_flags.h"
#include "sys_vars.h"
#include "msgdata/msg/msg_0716.h"
#include "pokegear_qa.h"
#include "field_system.h"
#include "gear_phone_ring_manager.h"
#include "save_local_field_data.h"
#include "text.h"
#include "render_text.h"
#include "font.h"
#include "system.h"
#include "sound_playback.h"
#include "constants/sndseq.h"
#include "game_stats.h"
#include "actual_expected_setters.inc"
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#define assert(value) do{if(!(value)){fprintf(stderr,"Pokegear draft check line %u\n",__LINE__);abort();}}while(0)
static SaveVarsFlags liveFlags;
static PlayerProfile profile;
static unsigned rngCalls,writeCalls;
static BOOL panicFlag;
static BOOL ringActive,gotGear=TRUE;
static SaveData save;
static SavePokegear seedGear;
static GearPhoneRingManager ring;
static Location location={.mapId=MAP_ROUTE_29};
SavePokegear *SaveData_Pokegear_Get(SaveData *value){assert(value==&save);return &seedGear;}
SaveVarsFlags *Save_VarsFlags_Get(SaveData *value){assert(value==&save);return &liveFlags;}
BOOL CheckGotPokegear(SaveVarsFlags *value){assert(value==&liveFlags);return gotGear;}
BOOL GearPhoneRingManager_IsRinging(GearPhoneRingManager *value){assert(value==&ring);return ringActive;}
LocalFieldData *Save_LocalFieldData_Get(SaveData *value){assert(value==&save);return NULL;}
Location *LocalFieldData_GetCurrentPosition(LocalFieldData *value){assert(!value);return &location;}
s32 PlayerProfile_CountBadges(PlayerProfile *value){assert(value==&profile);return 0;}
BOOL Save_VarsFlags_CheckFlagInArray(SaveVarsFlags *value,u16 id){assert(value==&liveFlags);return id==FLAG_GOT_ELMS_PANIC_CALL && panicFlag;}
BOOL Save_VarsFlags_IsInRocketTakeover(SaveVarsFlags *value){assert(value==&liveFlags);return FALSE;}
BOOL CheckGameClearFlag(SaveVarsFlags *value){assert(value==&liveFlags);return FALSE;}
u16 LCRandom(void){++rngCalls;return 0;}
u32 MTRandom(void){++rngCalls;return 0;}
void Save_VarsFlags_SetFlagInArray(SaveVarsFlags *value,u16 id){++writeCalls;}
void Save_VarsFlags_ClearFlagInArray(SaveVarsFlags *value,u16 id){++writeCalls;}
void PhoneCallPersistentState_PhoneRematches_SetSeeking(PhoneCallPersistentState *value,u8 id,BOOL enabled){++writeCalls;}
void PhoneCallPersistentState_PhoneRematches_GiftItemIdSet(PhoneCallPersistentState *value,u8 id,u16 item){++writeCalls;}
void ReadMsgDataIntoString(MsgData *data,s32 number,String *dest){++writeCalls;}
void BufferString(MessageFormat *format,u32 field,const String *value,s32 a,s32 b,s32 c){++writeCalls;}
#include "actual_elm_selector.inc"
#include "actual_generic_effect.inc"
#include "actual_gear_setters.inc"
#include "actual_elm_script.inc"
#include "actual_stats_type.inc"
static volatile GameStats statsLayoutProbe;
#include "actual_pg_baseline.inc"
static SysTask *sTextPrinterTasks[MAX_TEXT_PRINTERS];
static TextFlags sTextFlags;
static TouchscreenHitbox sTouchScreenHitbox;
struct System gSystem;
static unsigned pageSounds,pageClears,pageFills;
BOOL TouchscreenHitbox_TouchNewIsIn(const TouchscreenHitbox *hitbox){assert(hitbox==&sTouchScreenHitbox);return FALSE;}
BOOL PlaySE(u16 id){assert(id==SEQ_SE_DP_SELECT);++pageSounds;return TRUE;}
u8 GetFontAttribute(FontID font,int attribute){assert(attribute==1);return 12;}
void FillWindowPixelBuffer(Window *window,u8 value){assert(window);++pageFills;}
static void TextPrinter_DrawDownArrow(TextPrinter *printer){assert(printer->template.window);}
static void TextPrinter_ClearDownArrow(TextPrinter *printer){assert(printer->template.window);++pageClears;}
#include "actual_pg_render_wait.inc"
#include "actual_pg_printer_wait.inc"
static void CheckPageWait(SysTaskQueue *queue,TextPrinter *printer,Window *window) {
    const u8 states[]={2,3,7,8};
    for(unsigned i=0;i<4;i++)for(unsigned key=1;key<=2;key++) {
        printer->state=states[i];printer->callbackResult=printer->callbackParam=0;printer->active=1;
        printer->template.window=window;printer->template.x=3;printer->template.y=4;printer->template.lineSpacing=2;
        TextFlags_SetAutoScrollParam(AUTO_SCROLL_OFF);gSystem.newKeys=0;
        TextPrinter before=*printer;assert(PGQAPrinterWait(queue,1,window)==2 && !memcmp(&before,printer,sizeof(before)));
        printer->callbackResult=1;assert(!TextPrinterIsWaitingForInput(1,window));printer->callbackResult=0;
        printer->callbackParam=1;assert(!TextPrinterIsWaitingForInput(1,window));printer->callbackParam=0;
        assert(PGRenderTextWait(printer)==RENDER_UPDATE && printer->state==states[i]);
        unsigned sounds=pageSounds,clears=pageClears,fills=pageFills;gSystem.newKeys=key;
        assert(PGRenderTextWait(printer)==RENDER_UPDATE);
        assert(printer->state==((states[i]==3 || states[i]==8) ? 4 : 0));
        assert(printer->template.currentX==3 && pageSounds==sounds+1 && pageClears==clears+1);
        if(states[i]==3 || states[i]==8)assert(printer->scrollDistance==14 && pageFills==fills);
        else assert(printer->template.currentY==4 && pageFills==fills+1);
        assert(!TextPrinterIsWaitingForInput(1,window));
        printer->state=states[i];TextFlags_SetAutoScrollParam(AUTO_SCROLL_ENABLE);
        TextPrinterSubStruct *sub=(TextPrinterSubStruct *)&printer->subStructFields;sub->autoScrollDelay=0;sounds=pageSounds;
        assert(!TextPrinterIsWaitingForInput(1,window));PGRenderTextWait(printer);
        assert(printer->state==states[i] && sub->autoScrollDelay==1 && pageSounds==sounds);
        sub->autoScrollDelay=100;PGRenderTextWait(printer);assert(printer->state==((states[i]==3 || states[i]==8) ? 4 : 0));
        printer->state=states[i];sub->autoScrollDelay=0;TextFlags_SetAutoScrollParam(AUTO_SCROLL_ENABLE|AUTO_SCROLL_SPEEDUP);
        assert(TextPrinterIsWaitingForInput(1,window));PGRenderTextWait(printer);assert(printer->state==((states[i]==3 || states[i]==8) ? 4 : 0));
    }
    TextFlags_SetAutoScrollParam(AUTO_SCROLL_OFF);gSystem.newKeys=0;
    for(unsigned state=0;state<10;state++)if(state!=2 && state!=3 && state!=7 && state!=8) {
        printer->state=state;assert(!TextPrinterIsWaitingForInput(1,window));
    }
    printer->state=3;assert(PGQAPrinterWait(queue,1,window)==2);
    puts("PASS: actual renderer page waits 2/3/7/8, A/B transitions, auto-scroll policy and busy/callback guards.");
}
int main(void) {
    assert(!statsLayoutProbe.encryptionKey);
    Window window={0},otherWindow={0};TextPrinter printer={.id=1,.active=1,.state=2};printer.template.window=&window;
    SysTaskQueue queue={.limit=4,.activeCount=1};SysTask task={.queue=&queue,.data=&printer,.next=&queue.headSentinel};queue.headSentinel.next=&task;sTextPrinterTasks[1]=&task;
    assert(PGQAPrinterWait(&queue,1,&window)==2);printer.state=3;assert(PGQAPrinterWait(&queue,1,&window)==2);
    CheckPageWait(&queue,&printer,&window);
    printer.callbackResult=1;assert(!PGQAPrinterWait(&queue,1,&window));printer.callbackResult=0;printer.callbackParam=1;assert(!PGQAPrinterWait(&queue,1,&window));printer.callbackParam=0;
    assert(!PGQAPrinterWait(&queue,2,&window) && !PGQAPrinterWait(&queue,1,&otherWindow));printer.active=0;assert(!PGQAPrinterWait(&queue,1,&window));printer.active=1;
    task.next=&task;assert(!PGQAPrinterWait(&queue,2,&window));task.next=&queue.headSentinel;
    FieldSystem field={.saveData=&save,.phoneRingManager=&ring};
    memset(&seedGear,0,sizeof(seedGear));memset(seedGear.phoneContacts,PHONE_CONTACT_NONE,sizeof(seedGear.phoneContacts));
    seedGear.phoneContacts[0].id=PHONE_CONTACT_MOTHER;seedGear.phoneContacts[1].id=PHONE_CONTACT_PROF__ELM;seedGear.lastUsedApp=3;
    assert(PGQABaseline(&field));ringActive=TRUE;assert(!PGQABaseline(&field));ringActive=FALSE;
    panicFlag=TRUE;assert(!PGQABaseline(&field));panicFlag=FALSE;gotGear=FALSE;assert(!PGQABaseline(&field));gotGear=TRUE;
    location.mapId=MAP_ROUTE_30;assert(!PGQABaseline(&field));location.mapId=MAP_ROUTE_29;
    seedGear.registeredCards=1;assert(!PGQABaseline(&field));seedGear.registeredCards=0;
    seedGear.skin=1;assert(!PGQABaseline(&field));seedGear.skin=0;seedGear.lastUsedApp=0;assert(!PGQABaseline(&field));seedGear.lastUsedApp=3;
    seedGear.phoneContacts[2].id=2;assert(!PGQABaseline(&field));seedGear.phoneContacts[2].id=PHONE_CONTACT_NONE;assert(PGQABaseline(&field));
    PhoneBookEntry entry={.id=PHONE_CONTACT_PROF__ELM,.type=PHONECALLTYPE_PROF_ELM,.mapId=MAP_NEW_BARK_ELMS_LAB_1F};
    PokegearPhoneCallContext ctx={.playerProfile=&profile,.saveVarsFlags=&liveFlags,.playerMapSec=MAP_ROUTE_29};
    PokegearPhoneCallState state={.phoneBookEntry=&entry};
    assert(PhoneCall_GetScriptId_ProfElm(&ctx,&state)==PHONE_SCRIPT_008 && state.scriptType==0 && !rngCalls && !writeCalls);
    // A story-gate drift must be visible rather than silently accepted as the same conversation.
    panicFlag=TRUE;assert(PhoneCall_GetScriptId_ProfElm(&ctx,&state)==PHONE_SCRIPT_009);panicFlag=FALSE;
    PhoneCall_ApplyGenericNPCcallSideEffect(&ctx,&actualElmScript);assert(!rngCalls && !writeCalls);
    SavePokegear live,expected,before;memset(&live,0xA5,sizeof(live));live.lastUsedApp=3;live.skin=0;live.registeredCards=0;
    before=expected=live;
    assert(!PokegearQAExpectConfigureExit(&live,&live,1) && !PokegearQAExpectParentExit(&live,&live,0));
    assert(!memcmp(&live,&before,sizeof(live)));
    assert(PokegearQAExpectConfigureExit(&expected,&live,1) && expected.skin==1 && !expected.registeredCards);
    assert(PokegearQAExpectParentExit(&expected,&live,0) && expected.lastUsedApp==0 && !memcmp(&live,&before,sizeof(live)));
    SavePokegear allowed=before;allowed.skin=1;allowed.lastUsedApp=0;assert(!memcmp(&expected,&allowed,sizeof(expected)));
    assert(!PokegearQAExpectConfigureExit(&expected,&live,6) && !PokegearQAExpectParentExit(&expected,&live,1));
    assert(PokegearQAExpectParentExit(&expected,&live,3));allowed.lastUsedApp=3;assert(!memcmp(&expected,&allowed,sizeof(expected)));
    puts("PASS: actual no-ring/story/card baseline, Elm early selector/script-none effects and original clone-only Gear setters; live block unchanged.");
    static const struct {const char *name;size_t offset;} fields[]={
        {"ui",offsetof(HGNativePokegearQAState,ui)},
        {"boot",offsetof(HGNativePokegearQAState,boot)},
        {"started",offsetof(HGNativePokegearQAState,started)},
        {"gearBytes",offsetof(HGNativePokegearQAState,gearBytes)},
        {"statsBytes",offsetof(HGNativePokegearQAState,statsBytes)},
        {"ledgerFlags",offsetof(HGNativePokegearQAState,partyUnchanged)},
        {"gearLive",offsetof(HGNativePokegearQAState,gearLive)},
        {"gearExpected",offsetof(HGNativePokegearQAState,gearExpected)},
        {"statsLive",offsetof(HGNativePokegearQAState,statsLive)},
        {"statsExpected",offsetof(HGNativePokegearQAState,statsExpected)},
        {"parentEntries",offsetof(HGNativePokegearQAState,parentEntries)},
        {"parentExits",offsetof(HGNativePokegearQAState,parentExits)},
        {"phoneEntries",offsetof(HGNativePokegearQAState,phoneEntries)},
        {"configureEntries",offsetof(HGNativePokegearQAState,configureEntries)},
        {"callSelections",offsetof(HGNativePokegearQAState,callSelections)},
        {"callCompletions",offsetof(HGNativePokegearQAState,callCompletions)},
        {"callTarget",offsetof(HGNativePokegearQAState,callTarget)},
        {"callScriptID",offsetof(HGNativePokegearQAState,callScriptID)},
        {"callEffectType",offsetof(HGNativePokegearQAState,callEffectType)},
        {"callTextWait",offsetof(HGNativePokegearQAState,callTextWait)},
        {"callTextSerial",offsetof(HGNativePokegearQAState,callTextSerial)},
        {"savedApp",offsetof(HGNativePokegearQAState,savedApp)},
        {"savedSkin",offsetof(HGNativePokegearQAState,savedSkin)},
        {"registeredCards",offsetof(HGNativePokegearQAState,registeredCards)},
        {"baselineSkin",offsetof(HGNativePokegearQAState,baselineSkin)},
        {"expectedSkin",offsetof(HGNativePokegearQAState,expectedSkin)},
        {"currentMap",offsetof(HGNativePokegearQAState,currentMap)},
        {"baselineMoney",offsetof(HGNativePokegearQAState,baselineMoney)},
        {"currentMoney",offsetof(HGNativePokegearQAState,currentMoney)},
    };
    printf("PGQA layout: {\"bytes\":%zu,\"actualGearBytes\":%zu,\"actualStatsBytes\":%zu",sizeof(HGNativePokegearQAState),sizeof(SavePokegear),sizeof(GameStats));
    for(size_t i=0;i<sizeof(fields)/sizeof(fields[0]);++i)printf(",\"%s\":%zu",fields[i].name,fields[i].offset);
    puts("}");return 0;
}
'''
C_STATE_CHECK=r'''
// SPDX-License-Identifier: GPL-3.0-or-later
#include "pokegear_qa.h"
#include "native_field_menu_ui.h"
#include "NativeImports.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
typedef uint32_t u32;typedef int BOOL;
enum {FALSE=0,TRUE=1};
static HGNativePokegearQAState sPGQA,pgFinal;
static const void *sPGQABootOwner;
static u32 sPGQARequest;
#include "actual_pg_request.inc"
static struct w2c_env environment;
static struct {struct w2c_env *env;} capture={&environment},*pgCapture=&capture;
static HGNativeFieldMenuUIState pgMenu;
static u32 pgStep;
#include "actual_pg_steps.inc"
static u32 pgLease,pgPressReads,pgReleaseReads,pgKey,pgX,pgY,pgTouch,pgLeaseOwner,pgLeaseGeneration,pgLeaseRevision,pgLeaseEngine,pgMenuCloseStage,touchEngine;
static u32 reads,writes,traceEvents,menuEvents,seenKeys,seenX,seenY,seenPressed;
int HGNativeSetInput(struct w2c_env *env,uint32_t keys,int folded,uint32_t x,uint32_t y,int pressed) {
    assert(env==&environment && !folded);++writes;seenKeys=keys;seenX=x;seenY=y;seenPressed=pressed;return 1;
}
uint32_t HGNativeInputPadReads(struct w2c_env *env){assert(env==&environment);return reads;}
#define PG_CHECK assert
static void pgTrace(const char *event){assert(!strcmp(event,"press") || !strcmp(event,"released") || !strcmp(event,"settled"));++traceEvents;}
static void pgMenuTrace(const char *event){++menuEvents;}
static void pgTextTrace(const char *event){}
static u32 pgTouchEngine(void){return touchEngine;}
#include "actual_pg_host_input.inc"
#include "actual_pg_host_predicates.inc"
int main(void) {
    int title,other;HGYNBootFacts facts={.kind=HG_YN_BOOT_TITLE,.ready=1,.phase=2,.progressFlags=15};
    sPGQA.revision=sPGQA.boot.revision=sPGQA.boot.ownerGeneration=UINT32_MAX;
    NPCYNBootPublish(&title,&facts);assert(sPGQA.revision==1 && sPGQA.boot.revision==1 && sPGQA.boot.ownerGeneration==1);
    assert(pgBootEligible(&sPGQA.boot));NPCYNBootRetire(&other);assert(sPGQA.boot.active);NPCYNBootRetire(&title);assert(!sPGQA.boot.active && !sPGQA.boot.ready);
    sPGQA=(HGNativePokegearQAState){.revision=7,.fieldReady=1,.compatibleSave=1};sPGQARequest=0;
    assert(!HGNativePokegearQARequest(6,HG_PG_QA_START) && !HGNativePokegearQARequest(0,HG_PG_QA_START) && !HGNativePokegearQARequest(7,99));
    assert(HGNativePokegearQARequest(7,HG_PG_QA_START) && !HGNativePokegearQARequest(7,HG_PG_QA_START));sPGQARequest=0;
    sPGQA.started=1;sPGQA.stage=HG_PG_QA_RUNNING;assert(!HGNativePokegearQARequest(7,HG_PG_QA_FINISH));sPGQA.fieldReady=0;
    assert(!HGNativePokegearQARequest(7,HG_PG_QA_CHECKPOINT));sPGQA.fieldReady=1;sPGQA.parentEntries=sPGQA.parentExits=sPGQA.phoneEntries=sPGQA.checkpoints=2;
    sPGQA.ui.active=1;assert(!HGNativePokegearQARequest(7,HG_PG_QA_CHECKPOINT));sPGQA.ui.active=0;
    sPGQA.configureEntries=3;sPGQA.callSelections=sPGQA.callCompletions=sPGQA.savedSkin=1;sPGQA.callTextSerial=2;
    assert(HGNativePokegearQARequest(7,HG_PG_QA_FINISH));sPGQARequest=0;sPGQA.error=1;assert(!HGNativePokegearQARequest(7,HG_PG_QA_CHECKPOINT));
    pgFinal.ui=(HGNativePokegearUIState){.active=1,.ownerGeneration=7,.revision=9,.ready=1,.ownerKind=1,.parentPhase=3,.viewActive=1,.viewKind=0,.phase=1};
    pgFinal.viewFactsGeneration=7;assert(pgView(&pgFinal,0,1));reads=10;pgPulse(1,0,0,FALSE,1);assert(writes==1 && seenKeys==1 && !seenPressed);
    reads=13;assert(!pgSettle() && writes==1);reads=14;assert(!pgSettle() && writes==2 && !seenKeys && !seenPressed);reads=15;assert(!pgSettle());reads=16;assert(pgSettle() && traceEvents==3);
    pgPulse(0,32,176,TRUE,1);assert(seenPressed && seenX==32 && seenY==176);pgFinal.ui.ready=0;++reads;assert(!pgSettle() && !seenPressed);reads+=2;assert(pgSettle());
    pgFinal.ui.ready=1;pgPulse(1,0,0,FALSE,1);++pgFinal.ui.ownerGeneration;++reads;assert(!pgSettle() && !seenKeys);reads+=2;assert(pgSettle());--pgFinal.ui.ownerGeneration;
    pgPulse(1,0,0,FALSE,1);touchEngine=1;++reads;assert(!pgSettle() && !seenKeys);reads+=2;assert(pgSettle());touchEngine=0;
    pgMenu=(HGNativeFieldMenuUIState){.active=1,.ownerGeneration=11,.revision=13,.ready=1,.phase=3,.viewActive=1,.controlActive=1};pgMenuCloseStage=1;
    pgPulse(2,0,0,FALSE,2);pgMenu.ready=0;++pgMenu.revision;++reads;assert(!pgSettle() && !seenKeys);reads+=2;assert(pgSettle() && menuEvents==2);pgMenuCloseStage=0;
    reads=UINT32_MAX-2;pgPulse(1,0,0,FALSE,1);reads=1;assert(!pgSettle());reads=3;assert(pgSettle());
    pgMenu=(HGNativeFieldMenuUIState){.ownerGeneration=11,.revision=14,.phase=UINT32_MAX};assert(pgMenuRetired(&pgMenu,11,13) && !pgMenuRetired(&pgMenu,12,13));
    pgFinal.ui=(HGNativePokegearUIState){.active=1,.ownerGeneration=7,.revision=9,.ready=1,.ownerKind=1,.parentPhase=11,.viewActive=1,.viewKind=3,.app=3,.phase=6};pgFinal.viewFactsGeneration=7;
    pgFinal.callMainState=1;pgFinal.callScriptState=3;pgFinal.callTextWait=2;pgFinal.callTextSerial=1;
    assert(pgTextCanContinue(&pgFinal,0,0) && !pgTextCanContinue(&pgFinal,1,0));pgFinal.callTextWait=3;pgFinal.callTextSerial=2;
    assert(pgTextCanContinue(&pgFinal,1,1) && !pgTextCanContinue(&pgFinal,1,0));pgFinal.callAwaitHangup=1;assert(!pgTextCanContinue(&pgFinal,1,1));
    assert(sizeof(pgSteps)/sizeof(pgSteps[0])==33 && pgSteps[20].op==PG_CALL_WAIT && pgSteps[21].op==PG_KEY && pgSteps[21].a==1);
    puts("PASS: actual resident request/boot helpers, real-prototype host4/2 input, stale/owner/menu/release/wrap guards.");return 0;
}
'''
os.environ['HEARTGOLD_TEST_MUTE']=os.environ['SIMCTL_CHILD_HEARTGOLD_TEST_MUTE']='1'
COPIES=('src/field_system.c','src/title_screen.c','src/application/main_menu/main_menu.c',
    'src/application/pokegear/main/pokegear_main.c','src/application/pokegear/phone/pokegear_phone.c',
    'src/application/pokegear/configure/pokegear_configure.c','src/application/pokegear/phone/phone_call.c')
def require(value,message):
    if not value:raise RuntimeError(message)
def sha(path):return hashlib.sha256(Path(path).read_bytes()).hexdigest()
def function(source,name):
    match=re.search(r'^(?:static (?:inline )?)?[\w *]+\b'+re.escape(name)+r'\([^;]*?\)\s*\{',source,re.M)
    require(match is not None,'Missing function '+name);start=source.index('{',match.start());end=start+1;level=1
    while level:level+=(source[end]=='{')-(source[end]=='}');end+=1
    return source[match.start():end]
def replace_once(source,anchor,value):
    require(source.count(anchor)==1,'Anchor drift: '+anchor);return source.replace(anchor,value,1)
def hook(call):return '/* PGQA_HOOK_BEGIN */\n'+call+'\n/* PGQA_HOOK_END */'
def private_append_delta(historical,current,approved):
    require(isinstance(historical,dict) and historical and isinstance(current,dict),'Actual historical/current banks required')
    require(set(historical)<=set(current) and all(current[n]==v for n,v in historical.items()),'Every historical Full input must remain byte-identical')
    added=set(current)-set(historical);require(not added or added==set(approved),'Only the exact four new private QA paths may be appended')
    require(all(isinstance(v,str) and re.fullmatch('[0-9a-f]{64}',v) for v in current.values()),'Concrete input hashes required')
    return {name:dict(before=None,after=current[name]) for name in sorted(added)}
def validate_private_full(root,receipt,out):
    import check_native_pc_box as common
    folder=Path(receipt).resolve().parent;historical=json.loads((folder/'inputs-before.json').read_text())
    actual_paths=common.current_input_paths();actual={name:sha(Path(name)) for name in sorted(actual_paths)}
    approved={str(root/name) for name in QA_PATHS};delta=private_append_delta(historical,actual,approved)
    require(all(Path(name).is_file() and not Path(name).is_symlink() for name in approved),'Four regular installed private QA files required')
    if delta:
        runtime_seals=[]
        def historical_seals(seals):
            # Preserve the recorded Full; separately prove that only these four new status entries were added.
            status=subprocess.check_output(['git','status','--porcelain=v1','-z','-uall'],cwd=root,env={**os.environ,'GIT_OPTIONAL_LOCKS':'0'},timeout=15)
            relative={str(Path(name).relative_to(root)).encode() for name in delta};kept=[];seen=set()
            for record in status.split(b'\0'):
                if not record:continue
                if len(record)>=4 and record[3:] in relative:
                    require(record[:2] in (b'??',b'A ',b' A'),'New QA file has an unexpected working-state status');seen.add(record[3:])
                else:kept.append(record+b'\0')
            require(seen==relative and hashlib.sha256(b''.join(kept)).hexdigest()==seals['workingState']['wtStatus'],'Full working-state differs beyond the exact QA4 append')
            current=copy.deepcopy(seals);current['workingState']['wtStatus']=hashlib.sha256(status).hexdigest();common.validate_seal_metadata(current);runtime_seals.append(current)
        # Reuse the actual validator code against its unchanged historical subset, without changing its module or files.
        scope=dict(common.validate_full.__globals__);scope['current_input_paths']=lambda:set(historical);scope['validate_seal_metadata']=historical_seals
        context=types.FunctionType(common.validate_full.__code__,scope)(receipt)
        context['historicalFullSeals']=context['seals'];context['seals']=runtime_seals[-1]
    else:context=common.validate_full(receipt)
    bank=out/'inputs-actual-native.json';bank.write_text(json.dumps(actual,indent=2,sort_keys=True)+'\n')
    context.update(sourceBank=actual,fullSourceBankCurrent=not delta,historicalFullBankUnchanged=True,qaAppendDelta=delta,
        historicalFullInputCount=len(historical),actualNativeInputCount=len(actual),actualRuntimeInputBankSha256=sha(bank))
    context['runtimeInputBankBytes']=bank.read_bytes()
    context['protected']={**context['protected'],**{name:actual[name] for name in approved}}
    return context
def gear_layout(text,packet=None):
    match=re.search(r'\| struct SavePokegear\n(.*?)\[sizeof=(\d+), align=\d+\]',text,re.S);require(match is not None,'Actual WASI Gear layout absent')
    block=match.group(1);skin=re.search(r'^\s*(\d+):(\d+)-(\d+)\s*\|.*\bskin\s*$',block,re.M)
    app=re.search(r'^\s*(\d+)\s*\|.*\blastUsedApp\s*$',block,re.M);require(skin is not None and app is not None,'Actual WASI bitfield facts absent')
    byte,low,high=map(int,skin.groups());require(high-low==6,'Original skin width drift')
    result=dict(bytes=int(match.group(2)),skinBit=byte*8+low,skinWidth=7,lastAppByte=int(app.group(1)))
    if packet:
        record=re.search(r'\| struct HGNativePokegearQAState\n(.*?)\[sizeof=(\d+), align=\d+\]',text,re.S);require(record is not None and int(record.group(2))==packet['bytes'],'Real WASI/private packet size differs')
        for name,offset in packet.items():
            if name in ('bytes','actualGearBytes','actualStatsBytes'):continue
            field='partyUnchanged' if name=='ledgerFlags' else name
            found=re.search(r'^\s*(\d+)\s*\|.*\b'+field+r'\s*$',record.group(1),re.M);require(found is not None and int(found.group(1))==offset,'Real WASI packet offset differs: '+name)
        stats=re.search(r'\| struct GameStats\n.*?\[sizeof=(\d+), align=\d+\]',text,re.S);require(stats is not None and int(stats.group(1))==packet['actualStatsBytes'],'Actual complete WASI GameStats size differs')
    return result
def evidence_passed(row,layout):
    fixed=dict(qaVersion=1,qaBytes=layout['bytes'],step=32,totalSteps=33,parentEntries=2,parentExits=2,phoneEntries=2,configureEntries=3,
        callSelections=1,callCompletions=1,textContinues=2,callTextSerial=2,callTarget=1,callScriptID=8,callEffectType=0,savedApp=0,savedSkin=1,cards=0,checkpoints=2,
        lockedRejected=2,continueExit=1,menuCloseRequests=2,menuCloses=2,terminalFieldReady=1,menuActive=0,coverage=63)
    return isinstance(row,dict) and row.get('status')=='passed_original_pokegear_phone_configure' and all(type(row.get(k)) is int and row[k]==v for k,v in fixed.items()) \
        and type(row.get('snapshots')) is int and row['snapshots']>=26 and type(row.get('staleRejected')) is int and row['staleRejected']>=10 \
        and type(row.get('titleExit')) is int and row['titleExit'] in (0,1) and type(row.get('modeSwitchSeen')) is int and row['modeSwitchSeen'] in (0,1) \
        and row.get('privateSaveWrites') is False and row.get('resetQuiescent') is True
def trace_passed(rows):
    if not isinstance(rows,list) or not rows or len(rows)%3:return False
    previous=0;previousStep=0;boot=[]
    for i in range(0,len(rows),3):
        group=rows[i:i+3]
        if any(not isinstance(r,dict) for r in group) or [r.get('event') for r in group]!=['press','released','settled']:return False
        a,b,c=group;fields=('step','op','key','touch','x','y','owner','generation','revision')
        if any(any(type(r.get(k)) is not int for k in (*fields,'padReads','frame','liveActive','liveReady','liveGeneration','liveRevision','leaseEngine','touchEngine')) for r in group):return False
        if any(any(r[k]!=a[k] for k in fields) for r in group) or not previousStep<=a['step']<33 or a['owner'] not in (0,1,2):return False
        if a['key'] not in (0,1,2,16,1024) or a['touch'] not in (0,1) or a['x']>=256 or a['y']>=192 or (a['touch'] and a['key']):return False
        if not previous<=a['padReads']<=b['padReads']<c['padReads'] or c['padReads']-b['padReads']<2:return False
        if a['leaseEngine'] not in (0,1) or a['touchEngine']!=a['leaseEngine'] or any(r['touchEngine'] not in (0,1) or r['leaseEngine']!=a['leaseEngine'] for r in group) or a['owner']==1 and a['leaseEngine']!=0:return False
        if b['padReads']-a['padReads']<4 and (not a['owner'] or (b['liveActive']==b['liveReady']==1 and b['liveGeneration']==a['generation'] and b['liveRevision']==a['revision'] and b['touchEngine']==a['leaseEngine'])):return False
        if a['owner'] and (not a['generation'] or not a['revision'] or a['liveActive']!=1 or a['liveReady']!=1 or a['liveGeneration']!=a['generation'] or a['liveRevision']!=a['revision']):return False
        if a['owner']==0:
            if a['step']==0:
                if any(type(a.get(k)) is not int for k in ('bootKind','bootActive','bootReady','bootGeneration','bootRevision','bootPhase','bootMode','bootNotice','bootProgress')):return False
                if a['key']!=1 or a['touch'] or a.get('bootActive')!=1 or a.get('bootReady')!=1 or not a.get('bootGeneration') or not a.get('bootRevision'):return False
                if a.get('bootKind')==1:
                    if a.get('bootPhase')!=2 or a.get('bootMode') or a.get('bootProgress')&15!=15:return False
                elif a.get('bootKind')==2:
                    if a.get('bootPhase')!=5 or a.get('bootMode') not in (0,1) or a.get('bootNotice')!=15 or a.get('bootProgress')&255!=255:return False
                else:return False
                boot.append((a['bootKind'],a['bootMode'],a['bootGeneration']))
            elif a['op']!=2 or a['key']!=1024 or a['touch']:return False
        previous=c['padReads'];previousStep=a['step']
    return bool(boot) and sum(k==1 for k,m,g in boot)<=1 and sum(k==2 and m==1 for k,m,g in boot)<=1 and sum(k==2 and m==0 for k,m,g in boot)==1
def menu_evidence_passed(rows,trace):
    if not isinstance(rows,list) or len(rows)!=8:return False
    for index,step in enumerate((26,31)):
        group=rows[index*4:index*4+4]
        if [r.get('event') for r in group]!=['admitted','released','settled','closed_field_ready'] or any(r.get('step')!=step for r in group):return False
        a,b,c,d=group;before=a.get('menu',{});after=d.get('menu',{})
        if not isinstance(before,dict) or not isinstance(after,dict) or any(type(r.get(k)) is not int for r in group for k in ('step','padReads','frame','snapshot','fieldReady','pokegearActive','closeCount','admittedGeneration','admittedRevision')):return False
        if any(type(m.get(k)) is not int for m in (before,after) for k in ('active','generation','revision','ready','phase','viewActive','controlActive','childActive','inputMode')):return False
        if any(before.get(k)!=v for k,v in dict(active=1,ready=1,phase=3,viewActive=1,controlActive=1,childActive=0,inputMode=0).items()):return False
        gen=a.get('admittedGeneration');rev=a.get('admittedRevision')
        if not gen or not rev or before.get('generation')!=gen or before.get('revision')!=rev:return False
        if any(r.get('pokegearActive')!=0 or r.get('admittedGeneration')!=gen or r.get('admittedRevision')!=rev for r in group):return False
        if any(after.get(k)!=0 for k in ('active','ready','viewActive','controlActive','childActive')) or after.get('phase')!=0xFFFFFFFF or after.get('generation')!=gen or not after.get('revision') or after['revision']==rev:return False
        if d.get('fieldReady')!=1 or d.get('closeCount')!=index+1 or not a.get('padReads',-1)<=b.get('padReads',-2)<c.get('padReads',-3)<=d.get('padReads',-4):return False
        found=[r for r in trace if r.get('step')==step and r.get('event')=='press']
        if len(found)!=1 or any(found[0].get(k)!=v for k,v in dict(key=2,touch=0,owner=2,generation=gen,revision=rev).items()):return False
    return True
def text_evidence_passed(rows,trace):
    if not isinstance(rows,list) or len(rows)!=6:return False
    presses=[r for r in trace if r.get('step')==20 and r.get('event')=='press'];hangup=[r for r in trace if r.get('step')==21 and r.get('event')=='press']
    if len(presses)!=2 or len(hangup)!=1 or hangup[0].get('key')!=1 or hangup[0].get('owner')!=1:return False
    for index in range(2):
        group=rows[index*3:index*3+3]
        if [r.get('event') for r in group]!=['admitted','released','settled']:return False
        if any(any(type(r.get(k)) is not int for k in ('step','serial','wait','continues','callTarget','scriptID','effectType','padReads','generation','revision')) for r in group):return False
        a,b,c=group
        if any(r['step']!=20 or r['serial']!=index+1 or r['continues']!=index+1 or r['callTarget']!=1 or r['scriptID']!=8 or r['effectType'] for r in group):return False
        if a['wait']!=index+2 or not a['generation'] or not a['revision'] or not a['padReads']<=b['padReads']<c['padReads'] or c['padReads']-b['padReads']<2:return False
        if any(presses[index].get(k)!=v for k,v in dict(key=1,touch=0,owner=1,generation=a['generation'],revision=a['revision']).items()):return False
    return True
def snapshot_files_passed(folder,rows,layout,gear):
    if not rows:return False
    expected=set();baseline=stats=None;lastExits=lastCalls=0
    for seq,row in enumerate(rows):
        if row.get('sequence')!=seq or type(row.get('step')) is not int or not 0<=row['step']<33:return False
        state=folder/f'pokegear-{seq:03d}-state.bin';menu=folder/f'pokegear-{seq:03d}-menu.bin';expected|={state,menu}
        if any(not p.is_file() or p.is_symlink() for p in (state,menu)):return False
        raw=state.read_bytes();m=menu.read_bytes()
        if len(raw)!=layout['bytes'] or len(m)!=36:return False
        word=lambda name:struct.unpack_from('<I',raw,layout[name])[0]
        version,size,revision,error=struct.unpack_from('<4I',raw)
        if version!=1 or size!=layout['bytes'] or not revision or error:return False
        ui=struct.unpack_from('<12I',raw,layout['ui']);menuWords=struct.unpack('<9I',m);required=3 if ui[0] or menuWords[0] else 1
        if row.get('requiredMask')!=required or type(row.get('touchEngine')) is not int or row['touchEngine'] not in (0,1) or ui[0] and ui[6] and row['touchEngine']!=0 or type(row.get('renderedMask')) is not int or type(row.get('variedMask')) is not int or row['renderedMask']&row['variedMask']&required!=required:return False
        if word('started'):
            if any(struct.unpack_from('<I',raw,layout['ledgerFlags']+4*i)[0]!=1 for i in range(10)) or word('registeredCards') or word('currentMap')!=33 or word('baselineMoney')!=word('currentMoney'):return False
            count=word('gearBytes');n=word('statsBytes')
            if count!=gear['bytes'] or count!=layout['actualGearBytes'] or n!=layout['actualStatsBytes'] or not 0<n<=512:return False
            live=raw[layout['gearLive']:layout['gearLive']+count];want=raw[layout['gearExpected']:layout['gearExpected']+count]
            stat=raw[layout['statsLive']:layout['statsLive']+n];statWant=raw[layout['statsExpected']:layout['statsExpected']+n]
            if live!=want or stat!=statWant:return False
            if baseline is None:baseline=want;stats=statWant
            if statWant!=stats or word('baselineSkin')!=0 or word('expectedSkin') not in (0,1):return False
            exits=word('parentExits');calls=word('callCompletions')
            if not lastExits<=exits<=2 or not lastCalls<=calls<=1:return False
            derived=bytearray(baseline)
            for bit in range(gear['skinBit'],gear['skinBit']+gear['skinWidth']):derived[bit//8]&=~(1<<(bit%8))
            if word('expectedSkin'):derived[gear['skinBit']//8]|=1<<(gear['skinBit']%8)
            derived[gear['lastAppByte']]=0 if exits else 3
            if bytes(derived)!=want or word('savedSkin')!=word('expectedSkin') or word('savedApp')!=(0 if exits else 3):return False
            if word('callSelections') and (word('callTarget')!=1 or word('callScriptID')!=8 or word('callEffectType')):return False
            lastExits,lastCalls=exits,calls
        for engine in range(2):
            if not row['renderedMask']&(1<<engine):continue
            image=folder/f'pokegear-{seq:03d}-engine-{engine}.ppm';expected.add(image)
            if not image.is_file() or image.is_symlink():return False
            pixels=image.read_bytes();header=b'P6\n256 192\n255\n'
            if not pixels.startswith(header) or len(pixels)!=len(header)+256*192*3:return False
            if required&(1<<engine) and len(set(pixels[len(header):][i:i+3] for i in range(0,256*192*3,3)))<2:return False
    return baseline is not None and lastExits==2 and lastCalls==1 and expected==set(folder.iterdir())
def parser_self_check(layout,gear=None,output=None):
    historical={'/tmp/original':'a'*64};approved={'/tmp/qa'+str(i) for i in range(4)};actual={**historical,**{name:'b'*64 for name in approved}}
    require(len(private_append_delta(historical,actual,approved))==4,'Exact QA4 append rejected')
    for changed in ({**actual,'/tmp/extra':'c'*64},{**actual,'/tmp/original':'c'*64},{name:value for name,value in actual.items() if name!='/tmp/qa0'}):
        try:private_append_delta(historical,changed,approved)
        except RuntimeError:pass
        else:raise RuntimeError('Invalid appended source bank accepted')
    row=dict(status='passed_original_pokegear_phone_configure',qaVersion=1,qaBytes=layout['bytes'],step=32,totalSteps=33,parentEntries=2,parentExits=2,phoneEntries=2,configureEntries=3,
        callSelections=1,callCompletions=1,textContinues=2,callTextSerial=2,callTarget=1,callScriptID=8,callEffectType=0,savedApp=0,savedSkin=1,cards=0,checkpoints=2,lockedRejected=2,continueExit=1,
        menuCloseRequests=2,menuCloses=2,terminalFieldReady=1,menuActive=0,coverage=63,snapshots=30,staleRejected=12,titleExit=1,modeSwitchSeen=1,privateSaveWrites=False,resetQuiescent=True)
    require(evidence_passed(row,layout),'Positive evidence schema failed')
    for key in row:
        broken={k:v for k,v in row.items() if k!=key};require(not evidence_passed(broken,layout),'Missing evidence accepted '+key)
    for key,value in (('callScriptID',9),('callEffectType',1),('textContinues',1),('callTextSerial',3),('savedApp',3),('savedSkin',0),('cards',1),('menuCloses',1),('parentExits',1),('resetQuiescent',False),('qaBytes',layout['bytes']+4),('callSelections',True)):
        require(not evidence_passed({**row,key:value},layout),'Changed evidence accepted '+key)
    a=dict(step=10,op=7,key=1,touch=0,x=0,y=0,owner=1,generation=7,revision=9,frame=10,liveActive=1,liveReady=1,liveGeneration=7,liveRevision=9,leaseEngine=0,touchEngine=0)
    boot=dict(a,step=0,op=0,owner=0,generation=0,revision=0,bootKind=2,bootActive=1,bootReady=1,bootGeneration=1,bootRevision=1,bootPhase=5,bootMode=0,bootNotice=15,bootProgress=255)
    bootTrace=[dict(boot,event=e,padReads=t) for e,t in (('press',0),('released',4),('settled',6))]
    trace=[*bootTrace,*(dict(a,event=e,padReads=t) for e,t in (('press',10),('released',14),('settled',16)))];require(trace_passed(trace),'Positive raw input trace failed')
    require(not trace_passed(trace[:-1]) and not trace_passed([trace[0],trace[2],trace[1]]),'Incomplete/reordered trace accepted')
    bad=[*trace];bad[4]={**bad[4],'padReads':11};require(not trace_passed(bad),'Short admitted press accepted')
    bad[4].update(liveReady=0,liveRevision=10);require(trace_passed(bad),'Causal early owner release rejected')
    bad[4].update(liveReady=1,liveRevision=9,touchEngine=1);require(trace_passed(bad),'Causal LCD-change release rejected')
    menus=[];menuTrace=[*bootTrace]
    for i,step in enumerate((26,31)):
        gen=7+i;rev=11+i;start=20+i*20
        before=dict(active=1,generation=gen,revision=rev,ready=1,phase=3,viewActive=1,controlActive=1,childActive=0,inputMode=0)
        after=dict(before,active=0,ready=0,phase=0xFFFFFFFF,viewActive=0,controlActive=0,revision=rev+1)
        for j,event in enumerate(('admitted','released','settled','closed_field_ready')):
            menus.append(dict(event=event,step=step,padReads=start+(0,4,6,8)[j],frame=start+j,snapshot=i*2+(j==3),fieldReady=int(j==3),pokegearActive=0,closeCount=i+int(j==3),admittedGeneration=gen,admittedRevision=rev,menu=after if j==3 else before))
        menuTrace += [dict(a,event=event,step=step,key=2,owner=2,generation=gen,revision=rev,padReads=start+delta,liveGeneration=gen,liveRevision=rev) for event,delta in (('press',0),('released',4),('settled',6))]
    require(menu_evidence_passed(menus,menuTrace),'Source-qualified two-menu-close schema failed')
    for key,value in (('generation',99),('active',1),('revision',11),('phase',3)):
        changed=[*menus];changed[3]={**changed[3],'menu':{**changed[3]['menu'],key:value}};require(not menu_evidence_passed(changed,menuTrace),'Incorrect menu retirement accepted '+key)
    text=[];textTrace=[]
    for index in range(2):
        textTrace.append(dict(step=20,event='press',key=1,touch=0,owner=1,generation=7,revision=9))
        text += [dict(event=event,step=20,serial=index+1,wait=index+2 if j==0 else 0,continues=index+1,callTarget=1,scriptID=8,effectType=0,padReads=10+index*10+delta,generation=7,revision=9) for j,(event,delta) in enumerate((('admitted',0),('released',4),('settled',6)))]
    textTrace.append(dict(step=21,event='press',key=1,owner=1));require(text_evidence_passed(text,textTrace),'Two source-qualified text pages plus separate hangup rejected')
    changed=[*text];changed[3]={**changed[3],'wait':2};require(not text_evidence_passed(changed,textTrace),'Wrong scroll-page wait accepted')
    if not gear:return
    # Generated structural parser samples only. These are deleted and never labeled game frames.
    with tempfile.TemporaryDirectory(prefix='pgqa-parser-only-',dir=output) as directory:
        folder=Path(directory);samples=[];base=bytearray(gear['bytes']);base[gear['lastAppByte']]=3
        for seq,exits in enumerate((0,2)):
            raw=bytearray(layout['bytes']);struct.pack_into('<4I',raw,0,1,layout['bytes'],seq+1,0)
            values=dict(started=1,gearBytes=gear['bytes'],statsBytes=layout['actualStatsBytes'],parentExits=exits,expectedSkin=int(bool(exits)),savedSkin=int(bool(exits)),savedApp=0 if exits else 3,
                baselineSkin=0,currentMap=33,registeredCards=0,baselineMoney=100,currentMoney=100,callSelections=int(bool(exits)),callCompletions=int(bool(exits)),callTarget=1 if exits else 0,callScriptID=8 if exits else 0,callEffectType=0)
            for name,value in values.items():struct.pack_into('<I',raw,layout[name],value)
            for i in range(10):struct.pack_into('<I',raw,layout['ledgerFlags']+4*i,1)
            want=bytearray(base)
            if exits:want[gear['lastAppByte']]=0;want[gear['skinBit']//8]|=1<<(gear['skinBit']%8)
            for name in ('gearLive','gearExpected'):raw[layout[name]:layout[name]+gear['bytes']]=want
            (folder/f'pokegear-{seq:03d}-state.bin').write_bytes(raw);(folder/f'pokegear-{seq:03d}-menu.bin').write_bytes(bytes(36))
            pixels=bytearray(256*192*3);pixels[3]=1;(folder/f'pokegear-{seq:03d}-engine-0.ppm').write_bytes(b'P6\n256 192\n255\n'+pixels)
            samples.append(dict(sequence=seq,step=1+seq,requiredMask=1,renderedMask=1,variedMask=1,touchEngine=1))
        require(snapshot_files_passed(folder,samples,layout,gear),'Structural snapshot parser failed')
        path=folder/'pokegear-001-state.bin';raw=bytearray(path.read_bytes());raw[layout['gearLive']+20]^=1;path.write_bytes(raw)
        require(not snapshot_files_passed(folder,samples,layout,gear),'Extra live Gear mutation accepted')
        raw[layout['gearLive']+20]^=1;raw[layout['gearExpected']+20]^=1;raw[layout['gearLive']+20]^=1;path.write_bytes(raw)
        require(not snapshot_files_passed(folder,samples,layout,gear),'Extra matched-clone mutation accepted')
def source_copies(root,core,out):
    spec=importlib.util.spec_from_file_location('pgqa_original_boot_sources',root/'ios/tools/npc_yesno_boot_sources.py')
    boot=importlib.util.module_from_spec(spec);spec.loader.exec_module(boot);boot.build_candidates(root,out)
    prefix='#define HG_POKEGEAR_QA_GUEST 1\n#include "pokegear_qa.h"\n'
    original={}
    for name in COPIES:
        origin=core/name if name in COPIES[3:6] else root/name;original[name]=origin.read_text()
        if name in COPIES[1:3]:continue
        source=original[name]
        if name==COPIES[0]:
            anchor='BOOL finished = FieldSystem_Main(fieldSystem);';source=replace_once(source,anchor,anchor+'\n'+hook('    PokegearQAFieldObserved(fieldSystem);'))
        elif name==COPIES[3]:
            anchor='    NativePokegearUIParentState(pokegearApp,&facts);';source=replace_once(source,anchor,anchor+'\n'+hook('    PokegearQAParentObserved(pokegearApp,*state);'))
            anchor='    SavePokegear_SetLastUsedApp(pokegearApp->savePokegear, pokegearApp->app);';source=replace_once(source,anchor,anchor+'\n'+hook('    PokegearQAParentExitObserved(pokegearApp);'))
        elif name==COPIES[4]:
            anchor='    PokegearApp_PublishNativeUI(phoneApp->pokegear,phoneApp,*state,ready);';source=replace_once(source,anchor,anchor+'\n'+hook('    PokegearQAPhoneObserved(phoneApp,*state);'))
            anchor='    PhoneCall_Exit(phoneApp);';source=replace_once(source,anchor,anchor+'\n'+hook('    PokegearQACallCompleted(phoneApp);'))
        elif name==COPIES[5]:
            anchor='    PokegearApp_PublishNativeUI(configureApp->pokegear,configureApp,*state,ready);';source=replace_once(source,anchor,anchor+'\n'+hook('    PokegearQAConfigureObserved(configureApp,*state);'))
            anchor='    Pokegear_SetSkin(configureApp->pokegear->savePokegear, configureApp->skin);';source=replace_once(source,anchor,anchor+'\n'+hook('    PokegearQAConfigureExitObserved(configureApp);'))
        else:
            selected=function(source,'PhoneCall_GetCallScriptId');source=replace_once(source,selected,selected[:-1]+hook('    PokegearQACallSelected(ctx);')+'\n}')
            main=function(source,'PhoneCall_Main');anchor='    PokegearPhoneCallState *state = &ctx->state;';observed=replace_once(main,anchor,anchor+'\n'+hook('    PokegearQACallObserved(ctx);'));source=replace_once(source,main,observed)
        path=out/name;path.parent.mkdir(parents=True,exist_ok=True);path.write_text(('#include "pokegear_qa_guest.inc"\n' if name==COPIES[0] else prefix)+source)
    for name in COPIES:
        source=(out/name).read_text().removeprefix(prefix).removeprefix('#include "pokegear_qa_guest.inc"\n');source=re.sub(r'^#line 1 [^\n]*\n','',source)
        source=re.sub(r'/\* PGQA_HOOK_BEGIN \*/\n.*?\n/\* PGQA_HOOK_END \*/\n?','',source,flags=re.S)
        source=re.sub(r'/\* HGYN_BOOT_HOOK_BEGIN:[^*]+\*/.*?/\* HGYN_BOOT_HOOK_END:[^*]+\*/','',source,flags=re.S)
        lines=lambda value:[line.strip() for line in value.splitlines() if line.strip()]
        require(lines(source)==lines(original[name]),'Changed original body: '+name)
    return original
def main():
    parser=argparse.ArgumentParser(description=__doc__);parser.add_argument('--source-root',type=Path,default=ROOT)
    parser.add_argument('--core-root',type=Path);parser.add_argument('--wasi-sdk',type=Path);parser.add_argument('--output',type=Path,required=True)
    parser.add_argument('--self-check',action='store_true');parser.add_argument('--syntax-check',action='store_true')
    parser.add_argument('--full-receipt',type=Path);parser.add_argument('--save',type=Path)
    args=parser.parse_args();root=args.source_root.resolve();core=(args.core_root or root).resolve();here=Path(__file__).resolve().parents[2];out=args.output.resolve()
    require(bool(args.full_receipt)==bool(args.save),'Future execution requires both --full-receipt and --save')
    require(not args.syntax_check or args.wasi_sdk is not None,'--syntax-check requires the installed --wasi-sdk')
    require(not out.exists(),'Choose a new output attempt');out.mkdir(parents=True)
    host=here/'ios/Original/abi32/pokegear_scenario_check.c'
    owned=[here/'ios/tests'/name for name in ('pokegear_qa.h','pokegear_qa_guest.inc')]+[host,Path(__file__).resolve()]
    sources=[root/name for name in (*COPIES,'src/application/pokegear/phone/scripts/phone_scripts_prof_elm.c',
        'src/application/pokegear/phone/scripts/phone_scripts_mother.c','src/application/pokegear/phone/phone_script_defs.c',
        'src/gear_phone_ring_manager.c','src/save_pokegear.c','src/game_stats.c','src/text.c','src/render_text.c','include/render_text.h','src/sys_task.c','src/sys_task_api.c','src/system.c',
        'src/application/trade_sequence/trade_sequence.c','include/sys_task.h','include/font_types_def.h','files/msgdata/msg/msg_0716.gmm','src/application/pokegear/phone/pokegear_phone_states.c',
        'src/application/pokegear/phone/pokegear_phone_contacts.c','files/tel/pmtel_book.json','src/start_menu.c','include/start_menu.h','ios/tools/npc_yesno_boot_sources.py')]
    sources += [core/name for name in (*COPIES[3:6],'include/native_pokegear_ui.h','include/application/pokegear/pokegear_internal.h')]
    before={str(p):sha(p) for p in {*owned,*sources}};report=dict(passed=False,commands=[],sourceHashesBefore=before,noAutomaticRetry=True,syntheticSaveSetters=False,
        gameRuntimeExecuted=False,moduleBuilt=False,observerDefinitionsLinked=False,hostABICompiled=False)
    env={**os.environ,'HEARTGOLD_TEST_MUTE':'1','SIMCTL_CHILD_HEARTGOLD_TEST_MUTE':'1','PYTHONDONTWRITEBYTECODE':'1'}
    def run(command,log):
        result=subprocess.run(['nice','-n','10',*map(str,command)],env=env,cwd=root,capture_output=True,text=True,timeout=60)
        (out/log).write_text(result.stdout+result.stderr);report['commands'].append(dict(command=list(map(str,command)),returnCode=result.returncode,log=log));require(result.returncode==0,'Failed '+str(out/log))
    try:
        copies=out/'source';copies.mkdir();original=source_copies(root,core,copies)
        elm=(root/'src/application/pokegear/phone/scripts/phone_scripts_prof_elm.c').read_text();elm=re.sub(r'^#include .*\n','',elm,flags=re.M);(out/'actual_elm_selector.inc').write_text(elm)
        calls=(root/'src/application/pokegear/phone/phone_call.c').read_text();(out/'actual_generic_effect.inc').write_text(function(calls,'PhoneCall_ApplyGenericNPCcallSideEffect')+'\n')
        save=(root/'src/save_pokegear.c').read_text();(out/'actual_gear_setters.inc').write_text('\n'.join(function(save,name) for name in ('Pokegear_SetSkin','SavePokegear_SetLastUsedApp','Pokegear_GetSkin','SavePokegear_GetLastUsedApp','Pokegear_GetRegisteredCardsArray'))+'\n')
        guest=(here/'ios/tests/pokegear_qa_guest.inc').read_text();(out/'actual_pg_baseline.inc').write_text(function(guest,'PGQABaseline')+'\n')
        (out/'actual_expected_setters.inc').write_text('\n'.join(function(guest,name) for name in ('PokegearQAExpectConfigureExit','PokegearQAExpectParentExit'))+'\n')
        renderer=(root/'src/render_text.c').read_text();body=function(renderer,'RenderText')
        page_cases=body[body.index('    case 2:'):body.index('    case 4:')]+body[body.index('    case 7:'):body.rindex('    }')]
        render_wait='\n'.join(function(renderer,name) for name in ('TextPrinter_ContinueInputNew','TextPrinter_Continue','TextPrinter_WaitAutoMode','TextPrinter_WaitWithDownArrow','TextFlags_SetAutoScrollParam'))
        (out/'actual_pg_render_wait.inc').write_text(render_wait+'\nstatic RenderResult PGRenderTextWait(TextPrinter *printer) { switch (printer->state) {\n'+page_cases+'\n} return RENDER_UPDATE; }\n')
        (out/'actual_pg_printer_wait.inc').write_text(function((root/'src/sys_task.c').read_text(),'SysTask_GetData')+'\n'+function(renderer,'TextPrinterCanAcceptPageInput')+'\n'+function((root/'src/text.c').read_text(),'TextPrinterIsWaitingForInput')+'\n'+function(guest,'PGQAPrinterWait')+'\n')
        (out/'actual_pg_request.inc').write_text('\n'.join(function(guest,name) for name in ('PGQANext','NPCYNBootPublish','NPCYNBootRetire','HGNativePokegearQARequest'))+'\n')
        host_text=host.read_text();(out/'actual_pg_steps.inc').write_text(host_text[host_text.index('enum {PG_BOOT'):host_text.index('static Capture *pgCapture;')]);(out/'actual_pg_host_input.inc').write_text('\n'.join(function(host_text,name) for name in ('pgRawUp','pgPulse','pgSettle'))+'\n')
        (out/'actual_pg_host_predicates.inc').write_text('\n'.join(function(host_text,name) for name in ('pgBootEligible','pgBootSameSwitchedOwner','pgBootExitWitness','pgBootTitleExitWitness','pgMenuRetired','pgView','pgTextCanContinue'))+'\n')
        definitions=(root/'src/application/pokegear/phone/phone_script_defs.c').read_text();definition=definitions[definitions.index('// PHONE_SCRIPT_008'):definitions.index('// PHONE_SCRIPT_009')];definition=definition[definition.index('{'):definition.rindex('}')+1]
        require('PHONESCRIPTTYPE_NONE' in definition and '.param0 = 0' in definition and '.param1 = 0' in definition,'Elm008 side-effect definition drift')
        (out/'actual_elm_script.inc').write_text('static const PhoneCallScriptDef actualElmScript='+definition+';\n')
        stats=re.search(r'struct GameStats \{.*?\};',(root/'src/game_stats.c').read_text(),re.S);require(stats is not None,'Actual GameStats definition missing');(out/'actual_stats_type.inc').write_text(stats.group()+'\n')
        includes=[here/'ios/tests',core/'include',root/'include',root/'ios/tests',root/'ios/build-original-host/include',out]
        native=['xcrun','clang','-std=c11','-O1','-g','-fms-extensions','-Wno-microsoft','-Wno-unknown-pragmas','-fsanitize=address,undefined','-fno-sanitize-recover=all','-DSDK_ARM9','-DHEARTGOLD_NATIVE']
        for path in includes:native+=['-iquote',path]
        native+=['-idirafter',root/'ios/build-original-host/include']
        for folder in ('lib/include','include/library','lib/include/cw','lib/include/nitro','lib/include/nnsys','lib/include/nitro/os','lib/NitroSDK/include','lib/NitroSDK/include-pokediamond','files'):native+=['-idirafter',root/folder]
        check=out/'actual_contract_check.c';check.write_text(C_LEDGER_CHECK);binary=out/'actual-call-ledger';run([*native,check,'-o',binary],'c-compile.log');run([binary],'c-run.log')
        layouts=[json.loads(line[len('PGQA layout: '):]) for line in (out/'c-run.log').read_text().splitlines() if line.startswith('PGQA layout: ')];require(len(layouts)==1,'One real C packet layout required');report['packetLayout']=layouts[0]
        tools=Path(os.environ.get('HG_ABI32_TOOLS',root/'ios/build-original-abi32/tools'));state=out/'actual_state_check.c';state.write_text(C_STATE_CHECK);binary=out/'actual-state-input'
        run([*native,'-iquote',root/'ios/Original/abi32','-I',tools/'wabt-1.0.42/include',state,'-o',binary],'state-compile.log');run([binary],'state-run.log')
        if args.wasi_sdk:
            flags=['--target=wasm32-wasip1','-std=gnu11','-O2','-fwrapv','-fno-strict-aliasing','-fno-builtin-malloc','-fno-builtin-free','-fwasm-exceptions','-fexceptions','-Xclang','-disable-red-zone','-fms-extensions','-Wno-microsoft','-Wno-unknown-pragmas','-DHEARTGOLD_NATIVE','-DHEARTGOLD_ABI32','-DSDK_ARM9','-DSDK_CW','-DSDK_CODE_ARM','-DSDK_TS','-D_NITRO','-DPM_KEEP_ASSERTS','-DPLATFORM_INTRINSIC_FUNCTION_BIT_CLZ32=__builtin_clz']
            for path in includes:flags+=['-I',path]
            for folder in ('files','src','lib/include','lib/include/cw','lib/include/nitro','lib/include/nnsys','lib/include/nitro/os','include/library','lib/NitroSDK/include','lib/NitroSDK/include-pokediamond'):flags+=['-I',root/folder]
            for index,path in enumerate([*(copies/name for name in COPIES),check]):run([args.wasi_sdk/'bin/clang',*flags,'-fsyntax-only',path],f'wasi-syntax-{index}.log')
            run([args.wasi_sdk/'bin/clang',*flags,'-Xclang','-fdump-record-layouts-complete','-fsyntax-only',check],'actual-wasi-layouts.log')
            report['wasiGearLayout']=gear_layout((out/'actual-wasi-layouts.log').read_text(),report['packetLayout'])
        audit=root/'ios/build-original-abi32/audit/report.json';audited=json.loads(audit.read_text());rows={row['file']:row for row in audited['units']}
        ownership={name:rows[name]['overlayStates'] for name in COPIES};require(not ownership[COPIES[0]] and all(len(ownership[name])==1 for name in COPIES[1:]),'Original direct/overlay ownership drift')
        parser_self_check(report['packetLayout'],report.get('wasiGearLayout'),out)
        report.update(passed=True,complete=False,status='passed_source_contracts',originalNonblankBodiesPreserved=True,sourceCopies=list(COPIES),overlayOwnership=ownership,auditReferenceSha256=sha(audit),
            selectorAndCloneLedgerActualCExecuted=True,expectedElmScript=8,expectedScriptType=0,expectedElmRNGCalls=0,
            privateExportsProposed=['HGNativePokegearQAGetState','HGNativePokegearQARequest'],futureTotalExports=34,
            residentRequestAndHostInputActualCExecuted=True,residentObserverBodiesImplemented=True,hostDriverImplemented=True,
            wasiSyntaxUnits=8 if args.wasi_sdk else 0,actualWASILayoutVerified=bool(args.wasi_sdk),structuralSnapshotParserNegativesExecuted=bool(args.wasi_sdk),
            limits=['Seven source copies pass syntax; whole generated-header host ABI and game are not linked or run by source checks.',
                'Selector/effect service bindings are controlled typed C fixtures; no original phone conversation or graphical runtime has run.'])
        if args.full_receipt:
            require(not args.self_check and not args.syntax_check and args.save and core==root and here==root,'Execution requires installed QA4/integrated core, matching fresh Full and an explicit private4 fixture')
            execute(root,here,args,out,copies,report,before)
    except BaseException as error:report.update(passed=False,complete=False,status='failed',firstFailure=dict(type=type(error).__name__,message=str(error)))
    finally:
        after={str(p):sha(p) for p in {*owned,*sources}};report['sourceHashesAfter']=after;report['sourceInputsUnchanged']=before==after;report['passed'] &= before==after;(out/'report.json').write_text(json.dumps(report,indent=2,sort_keys=True)+'\n')
    print(('PASS' if report['passed'] else 'FAIL')+': '+str(out/'report.json'));return 0 if report['passed'] else 1
def execute(root,here,args,out,copies,report,source_pins):
    sys.path[:0]=[str(root/'ios/tools'),str(root/'ios/tests')]
    from check_native_pc_box import Runner,assert_unchanged,require_hashes,wasm_exports
    from prepare_overlay_state import compile_unit
    context=validate_private_full(root,args.full_receipt,out)
    report.update(historicalFullBankUnchanged=context['historicalFullBankUnchanged'],historicalFullInputCount=context['historicalFullInputCount'],actualNativeInputCount=context['actualNativeInputCount'],qaAppendDelta=context['qaAppendDelta'],actualRuntimeInputBankSha256=context['actualRuntimeInputBankSha256'],fullSourceBankCurrent=context['fullSourceBankCurrent'])
    shipping=context['formal']['shipping']['exports'];require(len(shipping)==32 and 'HGNativePokegearUIGetState' in shipping and context['formal']['actualNormalCheckKinds']==103,'Integrated Pokegear32/103 Full required')
    require('wasiGearLayout' in report,'Explicit --wasi-sdk required for the actual WASI byte-effect layout before execution')
    h=context['helpers'];sdk=args.wasi_sdk;guest_flags=['--target=wasm32-wasip1','-std=gnu11','-O2','-fwrapv','-fno-strict-aliasing','-fno-builtin-malloc','-fno-builtin-free','-fwasm-exceptions','-fexceptions','-Xclang','-disable-red-zone',
        '-fms-extensions','-Wno-microsoft','-Wno-unknown-pragmas','-DHEARTGOLD_NATIVE','-DHEARTGOLD_ABI32','-DSDK_ARM9','-DSDK_CW','-DSDK_CODE_ARM','-DSDK_TS','-D_NITRO','-DPM_KEEP_ASSERTS','-DPLATFORM_INTRINSIC_FUNCTION_BIT_CLZ32=__builtin_clz']
    for path in (*context['includes'],here/'ios/tests',root/'ios/tests',root/'include',root/'files',root/'src',root/'lib/include',root/'lib/include/cw',root/'lib/include/nitro',root/'lib/include/nnsys',root/'lib/include/nitro/os',root/'include/library',root/'lib/NitroSDK/include',root/'lib/NitroSDK/include-pokediamond'):guest_flags+=['-I',str(path)]
    if context['link'].get('nativeContent'):guest_flags.append('-DHG_NATIVE_ROM_TABLES')
    save=args.save.resolve();require(not args.save.is_symlink(),'Fixture symlink forbidden');fixture=[Path(str(save)+suffix) for suffix in ('','.console','.wifi','.lock')]
    require(all(p.is_file() and not p.is_symlink() for p in fixture) and set(fixture)==set(save.parent.glob(save.name+'*')),'Exactly four regular private fixture files required')
    fixture_pins={str(p):sha(p) for p in fixture};runner=Runner(report);guest=out/'guest';guest.mkdir();units={r['file']:r for r in context['audited']['units']};objects=list(context['objects']);replacements=[]
    try:
        require_hashes(source_pins,'Pokegear private sources')
        for name in COPIES:
            source=copies/name;target=guest/(Path(name).stem+'.o');expected=units[name]['overlayStates'];log=guest/(Path(name).stem+'.log')
            if expected:code,diagnostics,state=compile_unit(str(sdk/'bin/clang'),guest_flags,source,target,name,root);log.write_text(diagnostics)
            else:row,diagnostics=runner([sdk/'bin/clang',*guest_flags,'-c',source,'-o',target],log);code=row['terminalReturnCode'];state={}
            require(code==0 and 'error' not in state and state.get('getter')==(expected[0]['getter'] if expected else None),'Actual original overlay ownership changed: '+name)
            if expected:require(state.get('globals')==expected[0]['globals'],'Private copy changed original writable globals: '+name)
            old=str(context['audit']/'objects'/Path(name).with_suffix('.o'));require(objects.count(old)==1,'One actual original object required: '+name);objects[objects.index(old)]=str(target)
            replacements.append(dict(source=name,originalSha256=sha(root/name),candidateSha256=sha(source),objectSha256=sha(target),overlayState=state))
        rsp=guest/'objects.rsp';rsp.write_text('\n'.join(shlex.quote(p) for p in objects)+'\n');command=context['link']['command'].copy();command[command.index(context['rsp'])]='@'+str(rsp)
        module=guest/'engine.wasm';command[command.index('-o')+1]=str(module)
        qa_exports=('HGNativePokegearQAGetState','HGNativePokegearQARequest')
        for name in qa_exports:command.insert(command.index('-o'),'-Wl,--export='+name)
        runner(command,guest/'strict-link.log');report['moduleBuilt']=True;exports=wasm_exports(module.read_bytes());require(exports==sorted([*shipping,*qa_exports]),'Exactly current shipping exports plus private2 required')
        report['observerDefinitionsLinked']=True
        native=out/'native';native.mkdir();generated,writes,restores=h['generate_engine'](native,module,lambda argv,log:runner(argv,native/log));cflags,runtime=h['native_build_settings'](native)
        cflags+=['-I',str(here/'ios/tests'),'-I',str(root/'ios/tests'),'-I',str(root/'ios/Original/abi32'),'-g','-fsanitize=address,undefined','-fno-sanitize-recover=all'];native_objects=[]
        host=here/'ios/Original/abi32/pokegear_scenario_check.c'
        for index,source in enumerate([*generated,*runtime,host]):
            target=native/f'host-{index}.o';extra=['-DWASM_RT_TRAP_HANDLER=HGEngineTrap'] if source.name=='wasm-rt-impl.c' else []
            runner(['xcrun','clang',*cflags,*extra,'-c',source,'-o',target],native/f'compile-{index}.log');native_objects.append(target)
        binary=native/'pokegear-check';runner(['xcrun','clang','-fsanitize=address,undefined',*native_objects,'-o',binary],native/'link.log');report['hostABICompiled']=True
        boot=out/'boot-header.bin';boot.write_bytes(h['read_boot_header'](context['pack']));frames=out/'frames';frames.mkdir();private=[]
        for p in fixture:
            copy=out/('scenario.sav'+p.name[len(save.name):]);shutil.copy2(p,copy);private.append(copy)
        private_pins={str(p):sha(p) for p in private};assert_unchanged(context);require_hashes(source_pins,'Pokegear private sources');require_hashes(fixture_pins,'Original private4 fixture')
        report.update(gameRuntimeExecuted=True,sourceCopiesCompiled=replacements,privateModuleSha256=sha(module),checkedStackWrites=writes,exceptionDepthRestores=restores)
        row,text=runner([binary,context['pack']/'files',boot,out/'scenario.sav',frames],out/'runtime.log',timeout=300,check=False)
        evidence=[];trace=[];snapshots=[];menus=[];text_rows=[]
        for line in text.splitlines():
            for prefix,destination in (('POKEGEAR evidence: ',evidence),('POKEGEAR input: ',trace),('POKEGEAR snapshot: ',snapshots),('POKEGEAR menu: ',menus),('POKEGEAR text: ',text_rows)):
                if line.startswith(prefix):destination.append(json.loads(line[len(prefix):]));break
        report.update(evidence=evidence,inputs=trace,snapshots=snapshots,menuEvidence=menus,textEvidence=text_rows)
        require(row['terminalReturnCode']==0 and not row['timedOut'] and row['allChildrenWaited'] and len(evidence)==1 and evidence_passed(evidence[0],report['packetLayout'])
            and trace_passed(trace) and menu_evidence_passed(menus,trace) and text_evidence_passed(text_rows,trace) and snapshot_files_passed(frames,snapshots,report['packetLayout'],report['wasiGearLayout']),'Original Pokegear evidence failed')
        require(private_pins=={str(p):sha(p) for p in private},'Unexpected private4 disk change')
        report.update(passed=True,complete=True,status='passed_original_pokegear_phone_configure',privateSaveFilesUnchanged=True,allCardGoalComplete=False)
    finally:
        assert_unchanged(context);require_hashes(source_pins,'Pokegear private sources');require_hashes(fixture_pins,'Original private4 fixture')
if __name__=='__main__':raise SystemExit(main())
