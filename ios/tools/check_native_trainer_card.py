#!/usr/bin/env python3
"""Private original Trainer Card scenario; pure checks/syntax precede any future matching Full execution."""
import argparse
import hashlib
import json
import os
from pathlib import Path
import re
import shlex
import shutil
import struct
import subprocess
import sys

sys.dont_write_bytecode=True
os.environ['HEARTGOLD_TEST_MUTE']=os.environ['SIMCTL_CHILD_HEARTGOLD_TEST_MUTE']='1'
DEFAULT_ROOT=Path(__file__).resolve().parents[2]
HERE=DEFAULT_ROOT/'ios/tests'
HOST=DEFAULT_ROOT/'ios/Original/abi32/trainer_card_scenario_check.c'
QA_EXPORTS=('HGNativeTrainerCardQAGetState','HGNativeTrainerCardQARequest')
ORIGINAL=('src/field_system.c','src/title_screen.c','src/application/main_menu/main_menu.c',
    'src/application/trainer_card/overlay_trainer_card_main.c','src/application/trainer_card/overlay_trainer_card_signature.c')
OWNED=(HERE/'trainer_card_qa.h',HERE/'trainer_card_ledger.h',HERE/'trainer_card_guest.inc',
    HERE/'trainer_card_contract_check.c',HOST,Path(__file__).resolve())
def require(value,message):
    if not value:raise RuntimeError(message)
def sha(path):return hashlib.sha256(Path(path).read_bytes()).hexdigest()
def write(path,value):Path(path).write_text(json.dumps(value,indent=2,sort_keys=True)+'\n')
def function(source,name):
    match=re.findall(r'^(?:static (?:inline )?)?[A-Za-z_][\w *]*\b'+re.escape(name)+r'\([^;]*?\)\s*\{.*?^\}',source,re.M|re.S)
    require(len(match)==1,'Expected one original C body: '+name);return match[0]
def replace_once(source,anchor,new):
    require(source.count(anchor)==1,'Original source anchor changed: '+anchor);return source.replace(anchor,new,1)
def hook(call):return '/* TCQA_HOOK_BEGIN */\n'+call+'\n/* TCQA_HOOK_END */'
def private_candidates(root,destination):
    from npc_yesno_boot_sources import build_candidates
    destination.mkdir(parents=True,exist_ok=False);title,menu=build_candidates(root,destination)
    field=(root/ORIGINAL[0]).read_text();anchor='BOOL finished = FieldSystem_Main(fieldSystem);'
    field=replace_once(field,anchor,anchor+'\n'+hook('    TrainerCardQAFieldTick(fieldSystem);'))
    field='#include "trainer_card_guest.inc"\n#line 1 "'+ORIGINAL[0]+'"\n'+field
    viewer=(root/ORIGINAL[3]).read_text();anchor='    TrainerCardMain_PublishNativeUI(cardMain, *state);'
    viewer=replace_once(viewer,anchor,anchor+'\n'+hook('    TrainerCardQAViewerObserved(cardMain);'))
    signature=(root/ORIGINAL[4]).read_text()
    for anchor,call in (
        ('    TrainerCardSignature_PublishNativeUI(signature, *state);','    TrainerCardQACanvasObserved(&signature->canvasWindow, *state, signature->state);'),
        ('        GameStats_Inc(signature->gameStats, GAME_STAT_TRAINER_CARDS_SIGNED);','        TrainerCardQAConfirmObserved();')):
        signature=replace_once(signature,anchor,anchor+'\n'+hook(call))
    anchor='    TrainerCardSignature_PackCanvas(signature->signature, signature->canvasWindow.pixelBuffer);'
    signature=replace_once(signature,anchor,hook('    TrainerCardQACanvasObserved(&signature->canvasWindow, *state, signature->state);')+'\n'+anchor+'\n'+hook('    TrainerCardQAEditorPacked();'))
    include='#define HG_TRAINER_CARD_QA_GUEST 1\n#include "trainer_card_qa.h"\n'
    for name,source in ((ORIGINAL[0],field),(ORIGINAL[3],include+viewer),(ORIGINAL[4],include+signature)):
        path=destination/name;path.parent.mkdir(parents=True,exist_ok=True);path.write_text(source)
    for name in ORIGINAL:
        candidate=(destination/name).read_text().removeprefix(include).removeprefix('#include "trainer_card_guest.inc"\n')
        candidate=re.sub(r'^#line 1 [^\n]*\n','',candidate)
        candidate=re.sub(r'/\* TCQA_HOOK_BEGIN \*/\n.*?\n/\* TCQA_HOOK_END \*/\n?','',candidate,flags=re.S)
        candidate=re.sub(r'/\* HGYN_BOOT_HOOK_BEGIN:[^*]+\*/.*?/\* HGYN_BOOT_HOOK_END:[^*]+\*/','',candidate,flags=re.S)
        lines=lambda text:[line.strip() for line in text.splitlines() if line.strip()]
        require(lines(candidate)==lines((root/name).read_text()),'Private observer changed original nonblank source: '+name)
    return [destination/name for name in ORIGINAL]
def flags(root):
    values=['--target=wasm32-wasip1','-std=gnu11','-O2','-fwrapv','-fno-strict-aliasing','-fno-builtin-malloc','-fno-builtin-free',
        '-fwasm-exceptions','-fexceptions','-Xclang','-disable-red-zone','-fms-extensions','-Wno-microsoft','-Wno-unknown-pragmas',
        '-DHEARTGOLD_NATIVE','-DHEARTGOLD_ABI32','-DSDK_ARM9','-DSDK_CW','-DSDK_CODE_ARM','-DSDK_TS','-D_NITRO',
        '-DPM_KEEP_ASSERTS','-DPLATFORM_INTRINSIC_FUNCTION_BIT_CLZ32=__builtin_clz','-I',str(HERE),'-I',str(root/'ios/tests')]
    for folder in ('ios/build-original-host/include','include','files','src','lib/include','lib/include/cw','lib/include/nitro',
        'lib/include/nnsys','lib/include/nitro/os','include/library','lib/NitroSDK/include','lib/NitroSDK/include-pokediamond'):
        values+=['-I',str(root/folder)]
    return values
def pure_check(root,output,run):
    math=(root/'src/math_util.c').read_text()
    names=('Math_CalcArraySum','_MonEncryptSegment','_MonDecryptSegment','MonEncryptionLCRNG')
    (output/'actual_math.inc').write_text('static u16 MonEncryptionLCRNG(u32 *seed);\n'+'\n'.join(function(math,name) for name in names))
    stats=(root/'src/game_stats.c').read_text();stats=re.sub(r'^#include .*\n','',stats,flags=re.M)
    for name in ('Save_GameStats_Get','GameStats_IncSpeciesCaught'):stats=stats.replace(function(stats,name),'')
    (output/'actual_game_stats.inc').write_text(stats)
    signature=(root/ORIGINAL[4]).read_text();save=(root/'src/save_trainer_card.c').read_text()
    struct=re.search(r'struct SaveTrainerCard \{.*?\};',(root/'include/save_trainer_card.h').read_text(),re.S).group()
    (output/'actual_card.inc').write_text(struct+'\n'+function(signature,'TrainerCardSignature_PackCanvas')+'\n'+function(save,'TrainerCard_SignatureExists'))
    host=HOST.read_text()
    regions=re.search(r'^static const u32 tcStrokeRegions\[4\]\[4\]=.*?;',host,re.M)[0]
    (output/'actual_stroke_effect.inc').write_text(regions+'\n'+function(host,'tcStrokeNewInk')+'\n'+function(host,'tcStrokeEffectValid'))
    enum=re.search(r'^enum \{\s*TC_BOOT[^;]+;',host,re.M)[0]
    steps=host[host.index('typedef struct TCStep'):host.index('static Capture *tcCapture;')]
    callers='''#include "NativeImports.h"
#include "native_field_menu_ui.h"
typedef struct {struct w2c_env *env;} Capture;
static Capture *tcCapture;
static HGNativeTrainerCardQAState tcFinal;
static HGNativeFieldMenuUIState tcMenu,tcMenuBefore;
static u32 tcMenuCloseStage;
static u32 tcStep,tcLease,tcPressReads,tcReleaseReads,tcKey,tcX,tcY,tcTouch,tcPoint,tcStroke;
static u32 tcLeaseGeneration,tcLeaseRevision,tcStrokeTraceCount;
static HGNativeTrainerCardUITouchLease tcTouchLease;
static TCQATouchPoint tcStrokeTrace[6];
static u32 inputReads,inputKeys,inputX,inputY,inputFolded,inputPressed;
#define TC_CHECK assert
static void tcTrace(const char *event){(void)event;}
static void tcMenuTrace(const char *event){(void)event;}
int HGNativeSetInput(struct w2c_env *env,uint32_t keys,int folded,uint32_t x,uint32_t y,int pressed){
    assert(env==tcCapture->env);inputKeys=keys;inputFolded=folded;inputX=x;inputY=y;inputPressed=pressed;return 1;
}
uint32_t HGNativeInputPadReads(struct w2c_env *env){assert(env==tcCapture->env);return inputReads;}
'''
    callers+=enum+'\n'+steps+'\n'+function(host,'tcRawUp')+'\n'+function(host,'tcPulse')+'\n'+function(host,'tcSettle')
    callers+='''
static void check_actual_input_callers(void){
    struct w2c_env env={0};Capture capture={&env};tcCapture=&capture;
    tcFinal.ui=(HGNativeTrainerCardUIState){.active=1,.ownerGeneration=7,.revision=9,.ready=1,.viewActive=1,.view=2,.phase=1,.subState=1};
    tcStep=0;tcPulse(1,0,0,FALSE,FALSE);assert(inputKeys==1&&!inputFolded&&!inputPressed);tcLease=0;
    tcStep=16;tcStroke=0;inputReads=10;tcPulse(0,40,80,TRUE,TRUE);
    assert(!inputKeys&&!inputFolded&&inputPressed&&inputX==40&&inputY==80);
    for(u32 point=1;point<5;++point){inputReads+=2;assert(!tcSettle());assert(!inputFolded&&inputPressed&&inputX==tcPoints[0][point][0]&&inputY==tcPoints[0][point][1]);}
    inputReads+=2;assert(!tcSettle());assert(!inputFolded&&!inputPressed);inputReads+=2;assert(tcSettle());
    tcStep=44;tcMenuCloseStage=1;tcMenu=tcMenuBefore=(HGNativeFieldMenuUIState){.active=1,.ownerGeneration=7,.revision=9,.ready=1,.phase=3,.viewActive=1,.controlActive=1};
    tcPulse(2,0,0,FALSE,FALSE);assert(inputKeys==2&&!inputFolded&&!inputPressed);
    assert(!tcSettle()&&inputKeys==2);++tcMenu.ownerGeneration;
    assert(!tcSettle()&&!inputKeys&&!inputPressed&&tcLease==2);inputReads+=2;assert(tcSettle());
    tcMenuCloseStage=0;
    puts("PASS actual tcPulse/tcSettle/tcRawUp callers against real NativeImports.h prototype: lid stays open, touch down/moves/up are pressed correctly");
}
'''
    (output/'actual_input_callers.inc').write_text(callers)
    binary=output/'ledger-check'
    run(['xcrun','clang','-std=c11','-O1','-g','-fsanitize=address,undefined','-fno-sanitize-recover=all',
        '-iquote',HERE,'-iquote',root/'ios/tests','-iquote',root/'include','-iquote',output,'-iquote',root/'ios/Original/abi32',
        '-I',Path(os.environ.get('HG_ABI32_TOOLS',root/'ios/build-original-abi32/tools'))/'wabt-1.0.42/include',
        HERE/'trainer_card_contract_check.c','-o',binary],'ledger-compile.log')
    text=run([binary],'ledger-run.log')
    layout=[json.loads(line[len('TCQA_LAYOUT '):]) for line in text.splitlines() if line.startswith('TCQA_LAYOUT ')]
    require(len(layout)==1,'Actual C packet layout witness missing')
    # Actual host pure contract only; generated w2c types/functions are deliberately excluded.
    host=HOST.read_text();begin=host.index('static BOOL tcBootEligible(');end=host.index('static void TCQAPresent(',begin)
    check=function(host,'tcInputContractSelfCheck')
    fixture='#include <stdint.h>\n#include "trainer_card_qa.h"\n#include "native_field_menu_ui.h"\ntypedef uint32_t u32;typedef int BOOL;enum{FALSE=0,TRUE=1};\n'+function(host,'tcMenuCanClose')+'\n'+function(host,'tcMenuRetired')+'\n'+host[begin:end]+check+'\nint main(void){return tcInputContractSelfCheck()!=0;}\n'
    (output/'host-contract.c').write_text(fixture)
    run(['xcrun','clang','-std=c11','-O1','-fsanitize=address,undefined','-fno-sanitize-recover=all',
        '-iquote',HERE,'-iquote',root/'ios/tests','-iquote',root/'include',output/'host-contract.c','-o',output/'host-contract'],'host-contract-compile.log')
    run([output/'host-contract'],'host-contract-run.log')
    # The sole original direct-signature script uses these exact fades around the application wait and restoration.
    script=(root/'files/fielddata/script/scr_seq/scr_seq_0004.s').read_text()
    require('_0EF6:\n\tCloseMsg\n\tFadeScreen 6, 1, 0, RGB_BLACK\n\tWaitFade\n\tOpenTrainerCardSignature\n\tRestoreOverworld\n\tFadeScreen 6, 1, 1, RGB_BLACK\n\tWaitFade\n' in script,'Original direct-signature script sequence changed')
    bodies=[]
    for name,names in (
        ('src/task.c',('Task_New','TaskManager_Call','FieldSystem_RunTaskFrame','FieldSystem_TaskIsRunning','FieldSystem_ApplicationIsRunning','FieldSystem_LoadFieldOverlay','FieldSystem_IsOverworldRestored','TaskManager_GetFieldSystem','TaskManager_GetStatePtr')),
        ('src/field_system.c',('FieldSystem_IsFieldMapLoaded','FieldSystem_IsFieldMapRunning','FieldSystem_IsChildAppRunning')),
        ('src/field_overworld_transition.c',('Task_RestoreOverworld','CallTask_RestoreOverworld','Task_WaitPaletteFade','PaletteFadeUntilFinished','CallTask_FadeFromBlack'))):
        source=(root/name).read_text();bodies.extend(function(source,name) for name in names)
    (output/'actual_direct_task_bodies.inc').write_text('\n'.join([*bodies,function((HERE/'trainer_card_guest.inc').read_text(),'TCQADirectTask')])+'\n')
    run(['xcrun','clang','-std=gnu11','-O1','-g','-fsanitize=address,undefined','-fno-sanitize-recover=all',
        '-DHEARTGOLD_NATIVE','-DSDK_ARM9','-DPM_KEEP_ASSERTS','-DHG_TC_DIRECT_TASK_CHECK','-iquote',root/'include',
        '-iquote',root/'include/library','-iquote',output,'-idirafter',root/'lib/include','-idirafter',root/'ios/build-original-host/include',
        HERE/'trainer_card_contract_check.c','-o',output/'direct-task-check'],'direct-task-compile.log')
    run([output/'direct-task-check'],'direct-task-run.log')
    return layout[0]
def evidence_passed(row):
    return isinstance(row,dict) and row.get('status')=='passed_original_trainer_card' and row.get('confirms')==3 \
        and row.get('qaVersion')==1 and row.get('qaBytes')==12092 and row.get('totalSteps')==55 \
        and row.get('editorEntries')==row.get('editorExits')==3 and row.get('directComplete')==1 \
        and row.get('step',-2)+1==row.get('totalSteps') and row.get('checkpoints',0)>=3 \
        and row.get('staleRejected',0)>=9 and row.get('snapshots',0)>=30 \
        and row.get('strokeEffects')==4 and row.get('menuCloseRequests')==row.get('menuCloses')==2 and row.get('directFieldWitnesses')==1 \
        and row.get('terminalFieldReady')==1 and all(row.get(key)==0 for key in ('menuActive','menuReady','menuViewActive','menuControlActive','menuChildActive')) \
        and all(type(row.get(key)) is int for key in ('menuCloseRequests','menuCloses','directFieldWitnesses','terminalFieldReady','menuActive','menuReady','menuViewActive','menuControlActive','menuChildActive','menuGeneration','menuRevision','menuPhase')) \
        and row['menuGeneration']>0 and row['menuRevision']>0 and row['menuPhase']==0xFFFFFFFF \
        and row.get('privateSaveWrites') is False and row.get('resetQuiescent') is True
MENU_FIELDS=('active','generation','revision','ready','phase','viewActive','controlActive','childActive','inputMode')
def menu_packet(row):
    return isinstance(row,dict) and set(row)==set(MENU_FIELDS) and all(type(row[k]) is int and 0<=row[k]<=0xFFFFFFFF for k in MENU_FIELDS) \
        and all(row[k]<=1 for k in ('active','ready','viewActive','controlActive','childActive','inputMode'))
def menu_evidence_passed(rows,trace):
    if not isinstance(rows,list) or len(rows)!=9:return False
    for index,row in enumerate(rows):
        if not isinstance(row,dict) or not menu_packet(row.get('menu')):return False
        if any(type(row.get(k)) is not int or not 0<=row[k]<=0xFFFFFFFF for k in ('step','padReads','frame','snapshot','fieldReady','trainerActive','closeCount','admittedGeneration','admittedRevision')):return False
        if row['trainerActive'] or (index and row['padReads']<rows[index-1]['padReads']):return False
    for number,step in enumerate((44,48)):
        group=rows[number*4:number*4+4];before=group[0];after=group[-1];menu=before['menu']
        if [r.get('event') for r in group]!=['admitted','released','settled','closed_field_ready'] or any(r['step']!=step for r in group):return False
        if not menu['generation'] or not menu['revision'] or menu['active']!=1 or menu['ready']!=1 or menu['phase']!=3 \
            or menu['viewActive']!=1 or menu['controlActive']!=1 or menu['childActive'] or menu['inputMode'] or before['fieldReady']:return False
        if any(r['admittedGeneration']!=menu['generation'] or r['admittedRevision']!=menu['revision'] or r['menu']['generation']!=menu['generation'] for r in group):return False
        if [r['closeCount'] for r in group]!=[number,number,number,number+1] or group[2]['padReads']-group[1]['padReads']<2:return False
        retired=after['menu']
        if not after['fieldReady'] or after['snapshot']<=before['snapshot'] or not retired['revision'] or retired['revision']==menu['revision'] \
            or retired['phase']!=0xFFFFFFFF or any(retired[k] for k in ('active','ready','viewActive','controlActive','childActive')):return False
        inputs=[r for r in trace if r.get('step')==step]
        if [r.get('event') for r in inputs]!=['press','released','settled'] or any(r.get('op')!=8 or r.get('touch') for r in inputs):return False
        if [r.get('key') for r in inputs]!=[2,0,0] or [r.get('padReads') for r in inputs]!=[r['padReads'] for r in group[:3]]:return False
    direct=rows[-1]
    return direct.get('event')=='direct_field_ready' and direct['step']==53 and direct['fieldReady']==1 and direct['closeCount']==2 \
        and direct['admittedGeneration']==direct['admittedRevision']==0 and direct['menu']['phase']==0xFFFFFFFF \
        and not any(direct['menu'][k] for k in ('active','ready','viewActive','controlActive','childActive')) \
        and not any(r.get('op')==8 and r.get('step') not in (44,48) for r in trace)
def menu_files_passed(folder,rows,snapshots,layout):
    for row in rows:
        if row['event'] not in ('admitted','closed_field_ready','direct_field_ready'):continue
        sequence=row['snapshot']
        if sequence>=len(snapshots) or snapshots[sequence]['step']!=row['step']:return False
        menu=(folder/f'trainer-{sequence:03d}-menu.bin').read_bytes()
        if len(menu)!=36 or dict(zip(MENU_FIELDS,struct.unpack('<9I',menu)))!=row['menu']:return False
        state=(folder/f'trainer-{sequence:03d}-state.bin').read_bytes()
        if struct.unpack_from('<I',state,layout['fieldReady'])[0]!=row['fieldReady'] or struct.unpack_from('<I',state,layout['ui'])[0]!=row['trainerActive']:return False
        snap=snapshots[sequence]
        if snap['requiredMask']!=(3 if row['menu']['active'] else 1):return False
    return True
def trace_passed(rows):
    if not rows:return False
    strokes=[];active=None;released=False;previousReads=0
    for row in rows:
        if not isinstance(row,dict):return False
        if type(row.get('padReads')) is not int or row['padReads']<previousReads:return False
        previousReads=row['padReads'];event=row.get('event')
        if event=='press':
            if active is not None:return False
            active=[row];released=False
        elif event=='move':
            if active is None or released or row.get('op')!=6:return False
            active.append(row)
        elif event=='released':
            if active is None or released or row.get('key') or row.get('touch'):return False
            active.append(row);released=True
        elif event=='settled':
            if active is None or not released or row['padReads']-active[-1]['padReads']<2:return False
            if active[0].get('op')==6:
                if len(active)!=6 or not active[0].get('generation') or not active[0].get('revision') \
                    or any(point.get('generation')!=active[0]['generation'] or point.get('revision')!=active[0]['revision']
                        or type(point.get('x')) is not int or not 0<=point['x']<256 or type(point.get('y')) is not int or not 0<=point['y']<192
                        or point.get('touch')!=(index<5) or (index and point['padReads']-active[index-1]['padReads']<2)
                        for index,point in enumerate(active)):return False
                if active[0].get('stroke')!=len(strokes) or any(point.get('stroke')!=len(strokes) for point in active):return False
                strokes.append(active)
            active=None
        else:return False
    return active is None and len(strokes)==4
def stroke_evidence_passed(rows,trace):
    presses=[r for r in trace if r.get('event')=='press' and r.get('op')==6]
    if len(rows)!=len(presses) or len(rows)!=4:return False
    for stroke,(row,press) in enumerate(zip(rows,presses)):
        if not isinstance(row,dict) or any(type(row.get(k)) is not int or not 0<=row[k]<=0xFFFFFFFF
            for k in ('stroke','step','generation','beforeSerial','afterSerial','newInkPixels','nonblank','differentFromPrevious','snapshot')):return False
        if row['stroke']!=stroke or row['step']!=press.get('step') or row['generation']!=press.get('generation') \
            or not row['generation'] or row['afterSerial']<=row['beforeSerial'] or not row['newInkPixels'] \
            or row['nonblank']!=1 or row['differentFromPrevious']!=1:return False
    return True
def pack_canvas(canvas):
    packed=bytearray(1536)
    for index,value in enumerate(canvas):packed[index//4]|=((value&15)==1)<<((index%4)*2)|((value>>4)==1)<<((index%4)*2+1)
    return bytes(packed)
def stroke_files_passed(folder,rows,snapshots,layout):
    regions=json.loads(re.search(r'tcStrokeRegions\[4\]\[4\]=(\{.*?\});',HOST.read_text())[1].replace('{','[').replace('}',']'))
    for row in rows:
        matches=[s for s in snapshots if s['step']==row['step'] and s['sequence']<=row['snapshot']]
        if len(matches)!=2 or matches[-1]['sequence']!=row['snapshot']:return False
        before,after=[(folder/f"trainer-{s['sequence']:03d}-state.bin").read_bytes() for s in matches]
        old=before[layout['canvas']:layout['canvas']+6144];new=after[layout['canvas']:layout['canvas']+6144]
        word=lambda data,key:struct.unpack_from('<I',data,layout[key])[0]
        if old==new or word(before,'canvasSerial')!=row['beforeSerial'] or word(after,'canvasSerial')!=row['afterSerial'] \
            or word(after,'canvasGeneration')!=row['generation']:return False
        x0,y0,width,height=regions[row['stroke']];ink=0
        for y in range(y0,y0+height):
            for x in range(x0,x0+width):
                offset=((y//8)*24+x//8)*32+(y%8)*4+(x%8)//2;shift=(x&1)*4
                ink+=((new[offset]>>shift)&15)==1 and ((old[offset]>>shift)&15)!=1
        packed=pack_canvas(new);previous=before[layout['saveCard']+32:layout['saveCard']+0x620]
        if ink!=row['newInkPixels'] or not (sum(struct.unpack('<384I',packed))&0xFFFFFFFF) or packed==previous:return False
    return True
def parser_self_check():
    valid=dict(status='passed_original_trainer_card',confirms=3,editorEntries=3,editorExits=3,directComplete=1,
        step=54,totalSteps=55,qaVersion=1,qaBytes=12092,checkpoints=3,staleRejected=12,snapshots=40,strokeEffects=4,privateSaveWrites=False,resetQuiescent=True,
        menuCloseRequests=2,menuCloses=2,directFieldWitnesses=1,terminalFieldReady=1,menuActive=0,menuReady=0,menuViewActive=0,menuControlActive=0,menuChildActive=0,menuGeneration=8,menuRevision=12,menuPhase=0xFFFFFFFF)
    require(evidence_passed(valid),'Valid complete evidence predicate failed')
    for key,value in (('confirms',2),('editorEntries',2),('directComplete',0),('totalSteps',54),('strokeEffects',3),('privateSaveWrites',True),('resetQuiescent',False),('menuCloses',1),('menuCloseRequests',3),('directFieldWitnesses',0),('terminalFieldReady',0),('menuActive',1)):
        require(not evidence_passed({**valid,key:value}),'Incomplete evidence accepted: '+key)
    trace=[]
    for stroke in range(4):
        for point in range(7):
            trace.append(dict(event='press' if point==0 else 'move' if point<5 else 'released' if point==5 else 'settled',
                op=6,step=(16,17,27,39)[stroke],stroke=stroke,padReads=stroke*20+point*2,generation=7,revision=9,key=0,touch=point<5,x=40+point,y=80+point))
    require(trace_passed(trace),'Finite continuous strokes rejected')
    menu_rows=[];menu_trace=[]
    for number,step in enumerate((44,48)):
        for index,event in enumerate(('admitted','released','settled','closed_field_ready')):
            retired=index==3;menu=dict(zip(MENU_FIELDS,(not retired,7+number,9+index,not retired,0xFFFFFFFF if retired else 3,not retired,not retired,0,0)))
            menu={k:int(v) for k,v in menu.items()}
            row=dict(event=event,step=step,padReads=100+number*20+index*2,frame=200+number*20+index*2,snapshot=number*2+retired,
                fieldReady=int(retired),trainerActive=0,closeCount=number+retired,admittedGeneration=7+number,admittedRevision=9,menu=menu)
            menu_rows.append(row)
            if index<3:menu_trace.append(dict(event=('press','released','settled')[index],step=step,op=8,key=2 if index==0 else 0,touch=0,padReads=row['padReads']))
    menu_rows.append(dict(event='direct_field_ready',step=53,padReads=150,frame=250,snapshot=4,fieldReady=1,trainerActive=0,closeCount=2,admittedGeneration=0,admittedRevision=0,menu=menu_rows[-1]['menu'].copy()))
    require(menu_evidence_passed(menu_rows,menu_trace),'Qualified original B closure trace rejected')
    for index,key,value in ((0,'ready',0),(0,'inputMode',1),(0,'childActive',1),(1,'generation',99),(3,'active',1),(3,'revision',9),(3,'phase',3)):
        broken=json.loads(json.dumps(menu_rows));broken[index]['menu'][key]=value
        require(not menu_evidence_passed(broken,menu_trace),'Invalid/recreated menu owner accepted: '+key)
    for index,key,value in ((2,'padReads',103),(3,'fieldReady',0),(3,'snapshot',0),(8,'closeCount',3)):
        broken=json.loads(json.dumps(menu_rows));broken[index][key]=value
        require(not menu_evidence_passed(broken,menu_trace),'Missing release/field/pixel witness accepted: '+key)
    require(not menu_evidence_passed(menu_rows,menu_trace+[dict(event='press',step=53,op=8,key=2)]),'Invented direct-exit B accepted')
    for index,change in ((1,{'revision':8}),(2,{'x':256}),(5,{'event':'move'}),(6,{'padReads':10})):
        bad=[dict(row) for row in trace];bad[index].update(change);require(not trace_passed(bad),'Broken stroke trace accepted')
    effects=[dict(stroke=s,step=(16,17,27,39)[s],generation=7,beforeSerial=s+1,afterSerial=s+2,newInkPixels=1,nonblank=1,differentFromPrevious=1,snapshot=2*s+1) for s in range(4)]
    require(stroke_evidence_passed(effects,trace),'Observed stroke witness rejected')
    for stroke in range(4):
        require(not trace_passed([r for r in trace if r['stroke']!=stroke]),'Dropping an entire sent stroke accepted')
        require(not stroke_evidence_passed([r for r in effects if r['stroke']!=stroke],trace),'Dropping an observed stroke accepted')
        for key,value in (('newInkPixels',0),('afterSerial',effects[stroke]['beforeSerial']),('nonblank',0),('differentFromPrevious',0)):
            broken=[{**r,key:value} if i==stroke else r for i,r in enumerate(effects)]
            require(not stroke_evidence_passed(broken,trace),'Dropped/no-effect/previous stroke witness accepted')
def snapshot_files_passed(folder,rows,layout):
    if not rows:return False
    expected=set();prefix=None;payload=None;exits=0
    for sequence,row in enumerate(rows):
        if not isinstance(row,dict):return False
        if row.get('sequence')!=sequence or type(row.get('step')) is not int:return False
        state=folder/f'trainer-{sequence:03d}-state.bin';expected.add(state)
        if not state.is_file() or state.is_symlink():return False
        raw=state.read_bytes()
        if len(raw)!=layout['bytes']:return False
        menu=folder/f'trainer-{sequence:03d}-menu.bin';expected.add(menu)
        if not menu.is_file() or menu.is_symlink() or len(menu.read_bytes())!=36 or not menu_packet(dict(zip(MENU_FIELDS,struct.unpack('<9I',menu.read_bytes())))):return False
        word=lambda name:struct.unpack_from('<I',raw,layout[name])[0]
        card=raw[layout['saveCard']:layout['saveCard']+0x620]
        if word('started'):
            if prefix is None:prefix=card[:32];payload=card[32:]
            if prefix!=card[:32] or any(struct.unpack_from('<I',raw,layout['ledgerFlags']+index*4)[0]!=1 for index in range(9)):return False
            count=word('statsBytes')
            if count!=layout['statsTypeBytes'] or raw[layout['statsLive']:layout['statsLive']+count]!=raw[layout['statsExpected']:layout['statsExpected']+count]:return False
        if word('canvasBytes'):
            if word('canvasBytes')!=6144:return False
            canvas=raw[layout['canvas']:layout['canvas']+6144];packed=pack_canvas(canvas)
            if bytes(packed)!=raw[layout['expectedPayload']:layout['expectedPayload']+1536]:return False
        observed=word('editorExits')
        if observed!=exits:
            if observed!=exits+1 or card[32:]!=raw[layout['expectedPayload']:layout['expectedPayload']+1536]:return False
            payload=card[32:];exits=observed
        elif word('started') and card[32:]!=payload:return False
        required=row.get('requiredMask');rendered=row.get('renderedMask');varied=row.get('variedMask')
        if type(required) is not int or required not in (1,3) or rendered&varied&required!=required:return False
        for engine in range(2):
            if not rendered&(1<<engine):continue
            image=folder/f'trainer-{sequence:03d}-engine-{engine}.ppm';expected.add(image)
            if not image.is_file() or image.is_symlink():return False
            data=image.read_bytes();header=b'P6\n256 192\n255\n'
            if not data.startswith(header) or len(data)!=len(header)+256*192*3:return False
            if required&(1<<engine) and len(set(data[len(header):][index:index+3] for index in range(0,256*192*3,3)))<2:return False
    return exits==3 and expected==set(folder.iterdir()) and payload is not None and not any(payload)
def main():
    parser=argparse.ArgumentParser(description=__doc__);parser.add_argument('--source-root',type=Path,default=DEFAULT_ROOT)
    parser.add_argument('--self-check',action='store_true');parser.add_argument('--syntax-check',action='store_true')
    parser.add_argument('--wasi-sdk',type=Path);parser.add_argument('--full-receipt',type=Path);parser.add_argument('--save',type=Path)
    parser.add_argument('--output',type=Path,required=True);args=parser.parse_args();root=args.source_root.resolve();args.output=args.output.resolve()
    sys.path.insert(0,str(root/'ios/tools'));sys.path.insert(0,str(root/'ios/tests'))
    require(not args.output.exists(),'Select a new output attempt; prior evidence is retained');args.output.mkdir(parents=True)
    report=dict(passed=False,complete=False,gameRuntimeExecuted=False,moduleBuilt=False,commands=[],remainingGaps=[],
        noAutomaticRetry=True,syntheticSaveSetters=False,privateExports=list(QA_EXPORTS),expectedPrivateExportCount=35,expectedSteps=55)
    def run(command,log,timeout=60):
        command=['nice','-n','10',*map(str,command)];result=subprocess.run(command,cwd=root,capture_output=True,text=True,timeout=timeout)
        (args.output/log).write_text(result.stdout+result.stderr);report['commands'].append(dict(command=command,returnCode=result.returncode,log=log,allChildrenWaited=True,timedOut=False))
        require(result.returncode==0,'Command failed; inspect '+str(args.output/log));return result.stdout+result.stderr
    source_paths=[*OWNED,*(root/name for name in ORIGINAL),root/'src/game_stats.c',root/'src/math_util.c',root/'src/save_trainer_card.c',
        root/'include/native_trainer_card_ui.h',root/'include/native_field_menu_ui.h',root/'ios/tools/npc_yesno_boot_sources.py',root/'ios/tests/npc_yesno_boot.inc',
        root/'ios/tests/npc_yesno_qa.h',root/'ios/Original/abi32/engine_check.c',root/'include/game_stats.h',root/'include/constants/game_stats.h',
        root/'include/save_trainer_card.h',root/'include/trainer_card_data.h',root/'include/application/trainer_card/overlay_trainer_card_main_internal.h',
        root/'include/application/trainer_card/overlay_trainer_card_signature_internal.h',root/'src/task.c',root/'src/field_overworld_transition.c',
        root/'src/scrcmd_c.c',root/'files/fielddata/script/scr_seq/scr_seq_0004.s']
    before={str(path):sha(path) for path in source_paths}
    try:
        report['packetLayout']=pure_check(root,args.output,run);parser_self_check()
        copies=private_candidates(root,args.output/'source')
        report['originalNonblankSourcePreserved']=True
        if args.syntax_check:
            require(args.wasi_sdk is not None,'Syntax check requires explicit installed --wasi-sdk')
            for index,source in enumerate(copies):run([args.wasi_sdk/'bin/clang',*flags(root),'-fsyntax-only',source],f'wasi-syntax-{index}.log')
        if args.self_check or args.syntax_check:
            report.update(passed=True,status='passed_pure_contracts_and_source_copies',complete=False,
                remainingGaps=['No future Full/module/generated hgengine.h yet; entire host C ABI compilation remains unrun.',
                    'All viewer/editor/overwrite/erase/direct runtime branches, coherent pixels, sampler calibration, reset and disk invariance remain unexecuted.'])
        else:
            require(args.full_receipt and args.save,'Execution requires explicit matching future Full and private4 fixture')
            execute(root,args,args.output,copies,report,before)
    except BaseException as error:report.update(passed=False,status='failed',firstFailure=dict(type=type(error).__name__,message=str(error)))
    finally:
        after={str(path):sha(path) for path in source_paths};report['sourceHashesBefore']=before;report['sourceHashesAfter']=after
        report['sourceInputsUnchanged']=before==after;report['passed'] &= before==after;write(args.output/'report.json',report)
    print(('PASS' if report['passed'] else 'FAIL')+': '+str(args.output/'report.json'));return 0 if report['passed'] else 1

def execute(root,args,output,copies,report,source_pins):
    from check_native_pc_box import Runner,validate_full,assert_unchanged,require_hashes,wasm_exports
    from prepare_overlay_state import compile_unit
    context=validate_full(args.full_receipt);require(context['fullSourceBankCurrent'],'Fresh Full must bind exact current source bank')
    guest_flags=flags(root)
    guest_flags[:0]=[part for folder in context['includes'] for part in ('-I',str(folder))]
    if context['link'].get('nativeContent'):guest_flags.append('-DHG_NATIVE_ROM_TABLES')
    save=args.save.resolve();require(not args.save.is_symlink(),'Fixture symlink forbidden')
    fixture=[Path(str(save)+suffix) for suffix in ('','.console','.wifi','.lock')]
    require(all(path.is_file() and not path.is_symlink() for path in fixture) and set(fixture)==set(save.parent.glob(save.name+'*')),'Require exactly private4 regular fixture files')
    original_hashes={str(path):sha(path) for path in fixture};runner=Runner(report);h=context['helpers'];guest=output/'guest';guest.mkdir()
    objects=list(context['objects']);units={row['file']:row for row in context['audited']['units']};replacement=[]
    try:
        require_hashes(source_pins,'Trainer private scenario sources')
        for name,source in zip(ORIGINAL,copies):
            target=guest/(Path(name).stem+'.o');expected=units[name]['overlayStates'];log=guest/(Path(name).stem+'.log')
            if expected:
                code,diagnostics,state=compile_unit(str(h['SDK']/'bin/clang'),guest_flags,source,target,name,root);log.write_text(diagnostics)
            else:
                row,diagnostics=runner([h['SDK']/'bin/clang',*guest_flags,'-c',source,'-o',target],log);code=row['terminalReturnCode'];state={}
            require(code==0 and 'error' not in state,'Private source compilation failed: '+name)
            require(state.get('getter')==(expected[0]['getter'] if expected else None),'Original overlay ownership changed: '+name)
            old=str(context['audit']/'objects'/Path(name).with_suffix('.o'));require(objects.count(old)==1,'One original object required: '+name)
            objects[objects.index(old)]=str(target);replacement.append(dict(source=name,originalSha256=sha(root/name),
                candidateSha256=sha(source),objectSha256=sha(target),overlayState=state))
        rsp=guest/'objects.rsp';rsp.write_text('\n'.join(shlex.quote(path) for path in objects)+'\n')
        command=context['link']['command'].copy();command[command.index(context['rsp'])]='@'+str(rsp);module=guest/'engine.wasm';command[command.index('-o')+1]=str(module)
        for name in QA_EXPORTS:command.insert(command.index('-o'),'-Wl,--export='+name)
        runner(command,guest/'strict-link.log');report['moduleBuilt']=True
        exports=wasm_exports(module.read_bytes())
        require(exports==sorted([*context['formal']['shipping']['exports'],*QA_EXPORTS]),'Exactly current shipping exports plus private2 required')
        native=output/'native';native.mkdir();generated,writes,restores=h['generate_engine'](native,module,lambda argv,log:runner(argv,native/log))
        cflags,runtime=h['native_build_settings'](native);cflags+=['-I',str(HERE),'-I',str(root/'ios/tests'),'-I',str(root/'ios/Original/abi32'),'-g','-fsanitize=address,undefined','-fno-sanitize-recover=all']
        native_objects=[]
        for index,source in enumerate([*generated,*runtime,HOST]):
            target=native/f'host-{index}.o';extra=['-DWASM_RT_TRAP_HANDLER=HGEngineTrap'] if source.name=='wasm-rt-impl.c' else []
            runner(['xcrun','clang',*cflags,*extra,'-c',source,'-o',target],native/f'compile-{index}.log');native_objects.append(target)
        binary=native/'trainer-card-check';runner(['xcrun','clang','-fsanitize=address,undefined',*native_objects,'-o',binary],native/'link.log')
        boot=output/'boot-header.bin';boot.write_bytes(h['read_boot_header'](context['pack']));frames=output/'frames';frames.mkdir();private=[]
        for path in fixture:
            copy=output/('scenario.sav'+path.name[len(save.name):]);shutil.copy2(path,copy);private.append(copy)
        private_before={str(path):sha(path) for path in private};assert_unchanged(context)
        require_hashes(source_pins,'Trainer private scenario sources');require_hashes(original_hashes,'Original private4 fixture')
        report.update(gameRuntimeExecuted=True,sourceCopies=replacement,privateModuleSha256=sha(module),checkedStackWrites=writes,exceptionDepthRestores=restores)
        row,text=runner([binary,context['pack']/'files',boot,output/'scenario.sav',frames],output/'runtime.log',timeout=300,check=False)
        evidence=[];trace=[];snapshots=[];strokes=[];menu=[]
        for line in text.splitlines():
            for prefix,target in (('TRAINER evidence: ',evidence),('TRAINER input: ',trace),('TRAINER snapshot: ',snapshots),('TRAINER stroke: ',strokes),('TRAINER menu: ',menu)):
                if line.startswith(prefix):target.append(json.loads(line[len(prefix):]));break
        report.update(evidence=evidence,inputs=trace,snapshots=snapshots,strokeEvidence=strokes,menuEvidence=menu)
        require(row['terminalReturnCode']==0 and not row['timedOut'] and row['allChildrenWaited'] and len(evidence)==1 and evidence_passed(evidence[0])
            and trace_passed(trace) and stroke_evidence_passed(strokes,trace) and snapshot_files_passed(frames,snapshots,report['packetLayout'])
            and stroke_files_passed(frames,strokes,snapshots,report['packetLayout']) and menu_evidence_passed(menu,trace)
            and menu_files_passed(frames,menu,snapshots,report['packetLayout']),'Original Trainer scenario evidence failed')
        require(private_before=={str(path):sha(path) for path in private},'Unexpected private4 disk change')
        report.update(passed=True,complete=True,status='passed_original_trainer_card',evidence=evidence,inputs=trace,snapshots=snapshots,strokeEvidence=strokes,privateSaveFilesUnchanged=True)
    finally:
        assert_unchanged(context);require_hashes(source_pins,'Trainer private scenario sources');require_hashes(original_hashes,'Original private4 fixture')

if __name__=='__main__':raise SystemExit(main())
