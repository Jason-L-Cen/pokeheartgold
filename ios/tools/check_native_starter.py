#!/usr/bin/env python3
"""Original starter input/grant case; real execution requires the new matching Full."""
import argparse
import hashlib
import json
import os
from pathlib import Path
import re
import shlex
import shutil
import sys
sys.dont_write_bytecode=True
os.environ['HEARTGOLD_TEST_MUTE']=os.environ['SIMCTL_CHILD_HEARTGOLD_TEST_MUTE']='1'
ROOT=Path(__file__).resolve().parents[2]
HERE=ROOT/'ios/tests'
HOST=ROOT/'ios/Original/abi32/starter_scenario_check.c'
ORIGINAL=('src/field_system.c','src/title_screen.c','src/application/main_menu/main_menu.c','src/choose_starter_app.c','src/scrcmd_c.c')
QA_EXPORTS=('HGNativeStarterQAGetState',)
def require(value,message):
    if not value:raise RuntimeError(message)
def sha(path):return hashlib.sha256(Path(path).read_bytes()).hexdigest()
def write(path,value):path.write_text(json.dumps(value,indent=2,sort_keys=True)+'\n')
def private_candidates(root,destination):
    from check_native_trainer_card import function,replace_once
    from npc_yesno_boot_sources import build_candidates
    destination.mkdir(parents=True,exist_ok=False);build_candidates(root,destination)
    field=(root/ORIGINAL[0]).read_text();anchor='BOOL finished = FieldSystem_Main(fieldSystem);'
    field=replace_once(field,anchor,anchor+'\n/* STARTER_QA_BEGIN */\n    StarterQAFieldObserved(fieldSystem);\n/* STARTER_QA_END */')
    field='#include "starter_guest.inc"\n'+field
    choose=(root/ORIGINAL[3]).read_text();body=function(choose,'ChooseStarter_Main')
    probe='''    if(StarterQAProbeWanted(work)) {
        // Original ray queries may populate math caches; work and source input must stay byte-identical.
        struct ChooseStarterAppWork before=*work;
        u32 held=gSystem.heldKeys,newKeys=gSystem.newKeys,touchX=gSystem.touchX,touchY=gSystem.touchY;
        u32 touchHeld=System_GetTouchHeld(),touchNew=System_GetTouchNew(),queries=0,found=0,hitX=0,hitY=0;
        for(u32 y=8;y<192 && !found;y+=8)for(u32 x=8;x<256 && !found;x+=8) {
            VecFx32 near,far;++queries;
            if(NNS_G3dScrPosToWorldLine(x,y,&near,&far)==0 && getTappedBallId(work->positions,&near,&far,13*FX32_ONE)==work->curSelection)
                {found=1;hitX=x;hitY=y;}
        }
        u32 unchanged=!memcmp(&before,work,sizeof(before)) && held==gSystem.heldKeys && newKeys==gSystem.newKeys
            && touchX==gSystem.touchX && touchY==gSystem.touchY && touchHeld==System_GetTouchHeld() && touchNew==System_GetTouchNew();
        StarterQAProbeObserved(work,queries,hitX,hitY,found,unchanged);
    }'''
    changed=replace_once(body,'    return FALSE;','/* STARTER_QA_BEGIN */\n'+probe+'\n/* STARTER_QA_END */\n    return FALSE;')
    choose='#include "starter_qa.h"\n'+replace_once(choose,body,changed)
    script=(root/ORIGINAL[4]).read_text()
    for name,anchor,call in [('ScrCmd_WaitButton','    SetupNativeScript(ctx, NativeScript_WaitButton);','    StarterQAWaitButtonObserved(ctx);'),
                            ('NativeScript_WaitTouchscreenMenuChoice','        *p_ret = ctx->data[1];','        StarterQAChoiceObserved(ctx,*p_ret);')]:
        body=function(script,name);changed=replace_once(body,anchor,anchor+'\n/* STARTER_QA_BEGIN */\n'+call+'\n/* STARTER_QA_END */')
        script=replace_once(script,body,changed)
    script='#include "starter_qa.h"\n'+script
    for name,text in ((ORIGINAL[0],field),(ORIGINAL[3],choose),(ORIGINAL[4],script)):
        path=destination/name;path.parent.mkdir(parents=True,exist_ok=True);path.write_text(text)
    for name in ORIGINAL:
        text=(destination/name).read_text().removeprefix('#include "starter_guest.inc"\n').removeprefix('#include "starter_qa.h"\n')
        text=re.sub(r'^#line 1 [^\n]*\n','',text)
        text=re.sub(r'/\* STARTER_QA_BEGIN \*/\n.*?/\* STARTER_QA_END \*/\n?','',text,flags=re.S)
        text=re.sub(r'/\* HGYN_BOOT_HOOK_BEGIN:[^*]+\*/.*?/\* HGYN_BOOT_HOOK_END:[^*]+\*/','',text,flags=re.S)
        lines=lambda value:[line.strip() for line in value.splitlines() if line.strip()]
        require(lines(text)==lines((root/name).read_text()),'Original body changed: '+name)
    return [destination/name for name in ORIGINAL]
def flags(root):
    from check_native_trainer_card import flags as original_flags
    return ['-I',str(HERE),'-I',str(root/'include'),'-I',str(ROOT/'include'),*original_flags(root)]
def pure_check(root,output,runner):
    from check_native_trainer_card import function
    host=HOST.read_text()
    fixture='''#include "starter_qa.h"
#include <assert.h>
#include <stdio.h>
typedef uint32_t u32;typedef int BOOL;
#define TRUE 1
#define FALSE 0
#define SQ_CHECK assert
static HGNativeStarterQAState sqFinal;
static u32 sqLease,sqReads,sqReleaseReads,sqOwner,sqGen,sqRev,sqLCD,sqScript,sqSerial,sqMovePending,sqMoveX,sqMoveZ;
static u32 reads,lcd,releases;
typedef struct {void *env;} Capture;static Capture capture,*sqCapture=&capture;
static u32 HGNativeInputPadReads(void *env){(void)env;return reads;}
static BOOL HGNativeSetInput(void *env,u32 key,BOOL folded,u32 x,u32 y,BOOL touch){(void)env;assert(!key && !folded && !x && !y && !touch);++releases;return 1;}
static u32 sqTouchEngine(void){return lcd;}
static void sqTrace(const char *event){(void)event;}
'''+function(host,'sqBootReady')+'\n'+function(host,'sqSettle')+'''
int main(void){
    HGNativeStarterQAState good={.fieldReady=1,.mapId=61,.x=4,.z=7,.betweenSteps=1};assert(HGStarterBaseline(&good));
    good.z=14;assert(HGStarterBaseline(&good));good.z=8;assert(!HGStarterBaseline(&good));good.z=7;
    good.partyCount=1;assert(!HGStarterBaseline(&good));good.partyCount=0;good.gotStarter=1;assert(!HGStarterBaseline(&good));good.gotStarter=0;
    assert(HGStarterStepValid(64,4,7,4,6) && !HGStarterStepValid(64,4,7,4,5) && !HGStarterStepValid(64,4,0,4,UINT32_MAX));
    assert(!HGStarterStepValid(16,UINT32_MAX,7,0,7));
    assert(!HGStarterStepValid(0,4,7,4,7));
    sqFinal.ui=(HGNativeStarterUIState){.active=1,.ownerGeneration=7,.revision=9,.ready=1,.phase=5};
    sqLease=1;sqOwner=2;sqGen=7;sqRev=9;reads=3;assert(!sqSettle() && !releases && sqLease==1);
    reads=4;assert(!sqSettle() && releases==1 && sqLease==2);reads=5;assert(!sqSettle());reads=6;assert(sqSettle() && !sqLease);
    sqLease=1;sqReads=reads;sqRev=8;assert(!sqSettle() && releases==2 && sqLease==2);
    sqLease=1;sqRev=9;lcd=1;assert(!sqSettle() && releases==3);lcd=0;
    sqLease=1;sqOwner=0;sqMovePending=1;sqMoveX=4;sqMoveZ=7;sqFinal=good;sqFinal.x=5;assert(!sqSettle() && releases==4);
    sqFinal=(HGNativeStarterQAState){.mapId=61,.scriptActive=1,.scriptId=13,.scriptWait=1,.scriptSerial=4};
    assert(HGStarterTextAllowed(&sqFinal,13,4) && !HGStarterTextAllowed(&sqFinal,13,3) && !HGStarterTextAllowed(&sqFinal,12,4));
    sqLease=1;sqOwner=4;sqScript=13;sqSerial=3;assert(!sqSettle() && releases==5);
    sqFinal.ui=(HGNativeStarterUIState){.active=1,.ownerGeneration=7,.revision=9,.ready=1,.phase=5};
    assert(HGNativeStarterUITouchAllowed(&sqFinal.ui,7,9,0,128,96));assert(!HGNativeStarterUITouchAllowed(&sqFinal.ui,7,8,0,128,96));
    assert(!HGNativeStarterUITouchAllowed(&sqFinal.ui,7,9,1,128,96) && !HGNativeStarterUITouchAllowed(&sqFinal.ui,7,9,0,256,96));
    sqFinal.ui.selectionState=2;assert(!HGNativeStarterUIKeyAllowed(&sqFinal.ui,7,9,32) && HGNativeStarterUIKeyAllowed(&sqFinal.ui,7,9,2));
    printf("STARTER layout: {\\"bytes\\":%zu,\\"bootBytes\\":%zu,\\"starterBytes\\":%zu}\\n",sizeof(HGNativeStarterQAState),sizeof(HGYNBootFacts),sizeof(HGNativeStarterUIState));return 0;
}
'''
    source=output/'contract.c';source.write_text(fixture);binary=output/'contract-check'
    runner(['xcrun','clang','-std=c11','-O1','-fsanitize=address,undefined','-fno-sanitize-recover=all','-I',HERE,'-I',root/'ios/tests','-iquote',root/'include','-iquote',ROOT/'include',source,'-o',binary],output/'contract-compile.log')
    runner([binary],output/'contract-run.log')
def execute(root,args,output,copies,report,pins,runner):
    from check_native_pc_box import validate_full,assert_unchanged,require_hashes,wasm_exports
    from check_native_save_roundtrip import static_delta
    from prepare_overlay_state import compile_unit
    require((root/'include/native_starter_ui.h').is_file(),'Integrated shipping Starter header required')
    context=validate_full(args.full_receipt);require(context['fullSourceBankCurrent'],'Fresh Full must bind these QA files')
    require('HGNativeStarterUIGetState' in context['formal']['shipping']['exports'],'New shipping Starter getter absent')
    original=args.save.resolve();fixture=[Path(str(original)+suffix) for suffix in ('','.console','.wifi','.lock')]
    require(not args.save.is_symlink() and all(path.is_file() and not path.is_symlink() for path in fixture) and set(fixture)==set(original.parent.glob(original.name+'*')),'Exactly four original regular fixture files required')
    originals={str(path):sha(path) for path in fixture};private=[];h=context['helpers'];guest=output/'guest';guest.mkdir()
    try:
        objects=list(context['objects']);units={row['file']:row for row in context['audited']['units']};private_objects=[];original_objects=[];ownership=[]
        guestflags=flags(root);guestflags[:0]=[part for folder in context['includes'] for part in ('-I',str(folder))]
        if context['link'].get('nativeContent'):guestflags.append('-DHG_NATIVE_ROM_TABLES')
        for name,source in zip(ORIGINAL,copies):
            target=guest/(Path(name).stem+'.o');expected=units[name]['overlayStates'];log=guest/(Path(name).stem+'.log')
            if expected:code,text,state=compile_unit(str(h['SDK']/'bin/clang'),guestflags,source,target,name,root);log.write_text(text)
            else:row,text=runner([h['SDK']/'bin/clang',*guestflags,'-c',source,'-o',target],log);code=row['terminalReturnCode'];state={}
            require(code==0 and 'error' not in state and state.get('getter')==(expected[0]['getter'] if expected else None),'Private original unit/overlay failed: '+name)
            old=str(context['audit']/'objects'/Path(name).with_suffix('.o'));require(objects.count(old)==1,'Original object missing: '+name);objects[objects.index(old)]=str(target)
            private_objects.append(target);original_objects.append(Path(old));ownership.append(dict(source=name,originalSha256=sha(root/name),copySha256=sha(source),overlayState=state))
        report['sourceCopies']=ownership;memory=static_delta(private_objects,original_objects,runner,guest);report['privateStaticData']=memory
        response=guest/'objects.rsp';response.write_text('\n'.join(shlex.quote(path) for path in objects)+'\n')
        command=context['link']['command'].copy();command[command.index(context['rsp'])]='@'+str(response);module=guest/'engine.wasm';command[command.index('-o')+1]=str(module)
        for kind in ('initial','max'):
            indices=[i for i,value in enumerate(command) if value.startswith('-Wl,--'+kind+'-memory=')];require(len(indices)==1,'One fixed memory flag required');i=indices[0]
            command[i]=command[i].split('=')[0]+'='+str(int(command[i].split('=')[1])+memory['extraFixedMemoryBytes'])
        command.insert(command.index('-o'),'-Wl,--export='+QA_EXPORTS[0])
        runner(command,guest/'strict-link.log');require(wasm_exports(module.read_bytes())==sorted([*context['formal']['shipping']['exports'],*QA_EXPORTS]),'Unexpected private/public exports')
        report.update(moduleBuilt=True,privateModuleSha256=sha(module));native=output/'native';native.mkdir()
        generated,writes,restores=h['generate_engine'](native,module,lambda argv,log:runner(argv,native/log))
        cflags,runtime=h['native_build_settings'](native);cflags+=['-I',str(HERE),'-I',str(root/'ios/tests'),'-iquote',str(root/'include'),'-I',str(root/'ios/Original/abi32'),'-g','-fsanitize=address,undefined','-fno-sanitize-recover=all']
        native_objects=[]
        for index,source in enumerate([*generated,*runtime,HOST]):
            obj=native/f'host-{index}.o';trap=['-DWASM_RT_TRAP_HANDLER=HGEngineTrap'] if source.name=='wasm-rt-impl.c' else []
            runner(['xcrun','clang',*cflags,*trap,'-c',source,'-o',obj],native/f'compile-{index}.log');native_objects.append(obj)
        binary=native/'starter-check';runner(['xcrun','clang','-fsanitize=address,undefined',*native_objects,'-o',binary],native/'link.log')
        boot=output/'boot-header.bin';boot.write_bytes(h['read_boot_header'](context['pack']))
        for path in fixture:
            copy=output/('scenario.sav'+path.name[len(original.name):]);shutil.copy2(path,copy);private.append(copy)
        before={str(path):sha(path) for path in private};frames=output/'frames';frames.mkdir()
        assert_unchanged(context);require_hashes(pins,'Starter QA sources');require_hashes(originals,'Original protected fixture')
        report['gameRuntimeExecuted']=True;row,text=runner([binary,context['pack']/'files',boot,output/'scenario.sav',frames,str(args.starter_index)],output/'runtime.log',timeout=300,check=False)
        evidence=[json.loads(line[len('STARTER evidence: '):]) for line in text.splitlines() if line.startswith('STARTER evidence: ')]
        snapshots=[json.loads(line[len('STARTER snapshot: '):]) for line in text.splitlines() if line.startswith('STARTER snapshot: ')]
        report.update(evidence=evidence,snapshots=snapshots)
        require(row['terminalReturnCode']==0 and not row['timedOut'] and row['allChildrenWaited'] and len(evidence)==1 and evidence[0].get('status')=='passed_original_starter' and evidence[0].get('resetQuiescent') is True,'Original Starter game flow failed')
        expected=evidence[0];require(expected['starterIndex']==args.starter_index and expected['partyCount']==1 and expected['species']==[152,155,158][args.starter_index] and expected['level']==5
            and all(expected[key]==1 for key in ('dexSeen','dexCaught','gotStarter','scene','entries','exits','grants','nicknameNo','choiceReturns','touchInspect','left','right','back','finalConfirm','fieldReady','probeUnchanged'))
            and expected['openSnapshots']==3 and 0<expected['probeQueries']<=713,'Incomplete real grant/input evidence')
        opened=[snapshot for snapshot in snapshots if snapshot['label']=='ball-open'];require(len(opened)==3,'Three original open/fade frames required')
        require(len({sha(frames/f"starter-{snapshot['sequence']:03d}-engine-0.ppm") for snapshot in opened})>=2,'Original ball/camera frames did not change')
        require({str(path):sha(path) for path in private}==before,'Starter case must not save private files')
        report.update(passed=True,complete=True,status='passed_original_starter',privateSaveFilesUnchanged=True,originalFixtureUnchanged=True,checkedStackWrites=writes,exceptionDepthRestores=restores,
            allThreeStartersVerified=False,scope='One original starter grant at selected index; UIKit/device/audio not executed.')
    finally:
        assert_unchanged(context);require_hashes(pins,'Starter QA sources');require_hashes(originals,'Original protected fixture')
def main():
    parser=argparse.ArgumentParser(description=__doc__);parser.add_argument('--source-root',type=Path,required=True);parser.add_argument('--output',type=Path,required=True)
    parser.add_argument('--self-check',action='store_true');parser.add_argument('--syntax-check',action='store_true');parser.add_argument('--wasi-sdk',type=Path)
    parser.add_argument('--full-receipt',type=Path);parser.add_argument('--save',type=Path);parser.add_argument('--starter-index',type=int,choices=range(3),default=0);args=parser.parse_args()
    root=args.source_root.resolve();output=args.output.resolve();require(not output.exists(),'Fresh output required');output.mkdir(parents=True)
    sys.path.insert(0,str(root/'ios/tools'));from check_native_pc_box import Runner
    report=dict(passed=False,complete=False,moduleBuilt=False,gameRuntimeExecuted=False,noAutomaticRetry=True,commands=[],privateExports=list(QA_EXPORTS),publicABIChanges=False)
    runner=Runner(report);owned=[HERE/'starter_qa.h',HERE/'starter_guest.inc',HOST,Path(__file__).resolve(),*(root/name for name in ORIGINAL),root/'ios/tools/npc_yesno_boot_sources.py',root/'ios/tests/npc_yesno_boot.inc']
    before={str(path):sha(path) for path in owned}
    try:
        pure_check(root,output,runner);copies=private_candidates(root,output/'source');report['originalBodiesPreserved']=True
        if args.syntax_check:
            require(args.wasi_sdk,'Explicit installed WASI SDK required')
            for index,source in enumerate(copies):runner([args.wasi_sdk/'bin/clang',*flags(root),'-fsyntax-only',source],output/f'wasi-{index}.log')
        if args.self_check or args.syntax_check:report.update(passed=True,status='passed_pure_starter_contracts',remainingGaps=['No new Full/private generated ABI/host build or original game/grant execution yet.'])
        else:
            require(args.full_receipt and args.save,'Runtime requires matching new Full and immutable before-starter fixture');execute(root,args,output,copies,report,before,runner)
    except BaseException as error:report.update(passed=False,firstFailure=dict(type=type(error).__name__,message=str(error)))
    finally:
        report['sourceInputsUnchanged']=before=={str(path):sha(path) for path in owned};report['passed'] &= report['sourceInputsUnchanged'];report['sourceHashesBefore']=before;write(output/'report.json',report)
    print(('PASS' if report['passed'] else 'FAIL')+': '+str(output/'report.json'));return 0 if report['passed'] else 1
if __name__=='__main__':raise SystemExit(main())
