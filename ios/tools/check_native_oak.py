#!/usr/bin/env python3
"""Private original Oak NewGame source contracts, genuine ABI preparation, and separately authorized runtime."""
from pathlib import Path
import argparse,hashlib,json,os,re,shlex,subprocess,sys
HERE=Path(__file__).resolve().parents[1]/'tests'
HOST=Path(__file__).resolve().parents[1]/'Original/abi32/oak_scenario_check.c'
QA_EXPORTS=('HGNativeOakQAGetState',)
ORIGINAL=('src/field_system.c','src/title_screen.c','src/application/main_menu/main_menu.c','src/oaks_speech.c','src/naming_screen.c')
def require(v,message):
    if not v:raise RuntimeError(message)
def sha(path):return hashlib.sha256(Path(path).read_bytes()).hexdigest()
def write(path,value):path.write_text(json.dumps(value,indent=2,sort_keys=True)+'\n')
def private_candidates(root,destination):
    from npc_yesno_boot_sources import build_candidates
    from check_native_trainer_card import replace_once
    destination.mkdir(parents=True,exist_ok=False);build_candidates(root,destination)
    field=(root/ORIGINAL[0]).read_text();anchor='BOOL finished = FieldSystem_Main(fieldSystem);'
    field=replace_once(field,anchor,anchor+'\n/* OAK_QA_BEGIN */\n    OakQAFieldObserved(fieldSystem);\n/* OAK_QA_END */')
    field='#include "oak_guest.inc"\n'+field
    menu=(destination/ORIGINAL[2]).read_text();menu='#include "oak_qa.h"\n'+menu
    anchor='            data->selectedApp = APPOPTION_NEW_GAME;'
    menu=replace_once(menu,anchor,anchor+'\n/* OAK_QA_BEGIN */\n            OakQAMainMenuObserved(3,data->dontHaveSavedata,data->selectedApp,HG_OAK_QA_MAIN_SELECTED);\n/* OAK_QA_END */')
    anchor='    MainMenuUtil_UpdateSprites();';menu=replace_once(menu,anchor,anchor+'\n/* OAK_QA_BEGIN */\n    OakQAMainMenuObserved(*state,data->dontHaveSavedata,data->selectedApp,HG_OAK_QA_MAIN_ORDINARY);\n/* OAK_QA_END */')
    anchor='        MainMenu_FreeGraphics(manager);';menu=replace_once(menu,anchor,'/* OAK_QA_BEGIN */\n        OakQAMainMenuObserved(7,data->dontHaveSavedata,data->selectedApp,HG_OAK_QA_MAIN_TERMINAL);\n/* OAK_QA_END */\n'+anchor)
    oak=(root/ORIGINAL[3]).read_text();anchor='    OakSpeech_PublishNativeUI(data,*pState);'
    oak=replace_once(oak,anchor,anchor+'\n/* OAK_QA_BEGIN */\n    OakQAObserved(data,*pState);\n/* OAK_QA_END */');oak='#include "oak_qa.h"\n'+oak
    naming=(root/ORIGINAL[4]).read_text();naming='#include "oak_qa.h"\n'+naming
    anchor='static void NativeNamingUIPublish(const NamingScreenAppData *owner, int phase) {'
    naming=replace_once(naming,anchor,'/* OAK_QA_BEGIN */\nstatic void OakQAObserveNamingData(const NamingScreenAppData *data,int phase);\n/* OAK_QA_END */\n'+anchor)
    anchor='    if (owner != sNativeNamingOwner || !sNativeNamingUI.active) return;'
    naming=replace_once(naming,anchor,anchor+'\n/* OAK_QA_BEGIN */\n    OakQAObserveNamingData(owner,phase);\n/* OAK_QA_END */')
    helper=r'''static void OakQAObserveNamingData(const NamingScreenAppData *data,int phase) {
        HGOakNamingFacts f={.phase=phase,.pageSwitchState=data->pageSwitchState,.pageNum=data->pageNum,.type=data->type,.maxLen=data->maxLen,.gender=data->playerGenderOrMonSpecies,.cursorX=data->kbCursor.x,.cursorY=data->kbCursor.y};
        f.keyboardReady=phase==NS_MAIN_STATE_INPUT_LOOP && data->pageSwitchState==NS_PAGESWITCH_STATE_IDLE && !data->kbCursor.ignoreInput;
        f.confirmReady=f.keyboardReady && data->kbCursor.x==12 && !data->kbCursor.y && data->keyboard[0][12]==NAME_SCREEN_BUTTON_OK && Sprite_GetDrawFlag(data->uiSprites[8]);
        for(unsigned i=0;i<NELEMS(sTouchHitboxDef) && f.glyphCount<2;i++) {
            const NamingScreenTouchHitboxDef *h=&sTouchHitboxDef[i];if(h->sizeParam!=2 || !h->cursorY || h->cursorY>=6 || h->cursorX>=13)continue;
            u16 code=data->keyboard[h->cursorY][h->cursorX];if(!code || code>=NAME_SCREEN_CONTROL_DAKU || code==CHAR_SPACE || code==EOS)continue;
            if(f.glyphCount && f.glyph[0]==code)continue;unsigned n=f.glyphCount++;f.glyph[n]=code;f.glyphX[n]=h->x+8;f.glyphY[n]=h->y+9;
        }
        f.length=data->textCursorPos;for(unsigned i=0;i<8;i++)f.name[i]=data->entryBuf[i];OakQANamingObserved(data,&f);
    }
'''
    anchor='static BOOL NamingScreen_HandleTouchInput(NamingScreenAppData *data) {'
    naming=replace_once(naming,anchor,'/* OAK_QA_BEGIN */\n'+helper+'/* OAK_QA_END */\n'+anchor)
    anchor='    String_Delete(data->groupLabel);';naming=replace_once(naming,anchor,'/* OAK_QA_BEGIN */\n    OakQANamingResultObserved(data,args->nameInputFlat,String_GetLength(args->nameInputString),args->noInput);\n/* OAK_QA_END */\n'+anchor)
    for name,text in ((ORIGINAL[0],field),(ORIGINAL[2],menu),(ORIGINAL[3],oak),(ORIGINAL[4],naming)):
        path=destination/name;path.parent.mkdir(parents=True,exist_ok=True);path.write_text(text)
    for name in ORIGINAL:
        text=(destination/name).read_text().removeprefix('#include "oak_guest.inc"\n').removeprefix('#include "oak_qa.h"\n')
        text=re.sub(r'^#line 1 [^\n]*\n','',text);text=re.sub(r'/\* OAK_QA_BEGIN \*/\n.*?/\* OAK_QA_END \*/\n?','',text,flags=re.S)
        text=re.sub(r'/\* HGYN_BOOT_HOOK_BEGIN:[^*]+\*/.*?/\* HGYN_BOOT_HOOK_END:[^*]+\*/','',text,flags=re.S)
        lines=lambda s:[line.strip() for line in s.splitlines() if line.strip()];require(lines(text)==lines((root/name).read_text()),'Original body changed:'+name)
    return [destination/name for name in ORIGINAL]
def flags(root):
    from check_native_starter import flags as base
    return ['-I',str(HERE),*base(root)]
def pure_check(root,output,runner):
    from check_native_trainer_card import function
    host=HOST.read_text();guest=(HERE/'oak_guest.inc').read_text();macro=next(line for line in guest.splitlines() if line.startswith('#define OAK_REQUIRE('));source=output/'contract.c'
    text=r'''#include "oak_qa.h"
#include <assert.h>
typedef uint32_t u32;typedef int BOOL;
#define TRUE 1
#define FALSE 0
#define OQ_CHECK assert
static HGNativeOakQAState oqFinal;
static u32 oqLease,oqReads,oqReleaseReads,oqOwner,oqGen,oqRev,oqLCD,oqKey,oqX,oqY;
static BOOL oqTouch;static u32 reads,lcd,releases;
static struct {void *env;} capture,*oqCapture=&capture;
static u32 HGNativeInputPadReads(void *env){(void)env;return reads;}
static BOOL HGNativeSetInput(void *env,u32 key,BOOL folded,u32 x,u32 y,BOOL touch){(void)env;assert(!key && !folded && !x && !y && !touch);++releases;return 1;}
static u32 oqTouchEngine(void){return lcd;}
'''+macro+'\nstatic HGNativeOakQAState sOakQA;\n'+function(guest,'OakQAMainMenuObserved')+'\n'+function(host,'oqOwnerValid')+'\n'+function(host,'oqSettle')+r'''
int main(void) {
    _Static_assert(sizeof(HGNativeOakQAState)==396,"OakQA396");
    sOakQA=(HGNativeOakQAState){0};OakQAMainMenuObserved(3,1,0,HG_OAK_QA_MAIN_ORDINARY);
    assert(!sOakQA.error && !sOakQA.mainSelectedNewGame && sOakQA.mainRouteMask==8 && !sOakQA.mainTerminal);
    OakQAMainMenuObserved(3,1,2,HG_OAK_QA_MAIN_SELECTED);assert(!sOakQA.error && sOakQA.mainSelectedNewGame);
    OakQAMainMenuObserved(8,1,2,HG_OAK_QA_MAIN_ORDINARY);OakQAMainMenuObserved(7,1,2,HG_OAK_QA_MAIN_TERMINAL);
    assert(!sOakQA.error && sOakQA.mainTerminal && sOakQA.mainSelectedNewGame && sOakQA.mainRouteMask==392);
    sOakQA=(HGNativeOakQAState){0};OakQAMainMenuObserved(3,1,0,HG_OAK_QA_MAIN_SELECTED);assert(sOakQA.error);
    sOakQA=(HGNativeOakQAState){0};OakQAMainMenuObserved(7,1,0,HG_OAK_QA_MAIN_TERMINAL);assert(sOakQA.error);

    oqFinal.ui=(HGNativeOakUIState){.active=1,.ownerGeneration=7,.revision=9,.ready=1,.parentPhase=1,.phase=3,.mode=HG_OAK_MENU,.keyMask=195,.touchMode=1,.count=3};
    oqOwner=2;oqGen=7;oqRev=9;oqKey=1;oqLCD=lcd=1;oqLease=1;oqReads=10;reads=13;assert(!oqSettle() && !releases);reads=14;assert(!oqSettle() && releases==1);reads=15;assert(!oqSettle());reads=16;assert(oqSettle());
    oqLease=1;oqReads=reads=20;oqFinal.ui.revision++;assert(!oqSettle() && releases==2);oqLease=0;
    oqFinal.ui.parentPhase=4;oqFinal.naming=(HGNativeNamingUIState){.active=1,.ownerGeneration=11,.revision=12,.ready=1};oqFinal.edit.keyboardReady=1;
    oqOwner=3;oqGen=11;oqRev=12;oqLCD=lcd=0;oqLease=1;oqReads=UINT32_MAX-1;reads=1;assert(!oqSettle());reads=2;assert(!oqSettle() && releases==3);reads=4;assert(oqSettle());
    oqLease=1;oqReads=reads=30;oqFinal.naming.ownerGeneration++;assert(!oqSettle() && releases==4);oqLease=0;
    assert(!HGOakNamingReady(&oqFinal,11,12));return 0;
}
'''
    source.write_text(text);binary=output/'contract-check';runner(['xcrun','clang','-std=c11','-O1','-fsanitize=address,undefined','-fno-sanitize-recover=all','-iquote',HERE,'-iquote',root/'ios/tests','-iquote',root/'include',source,'-o',binary],output/'contract-compile.log');runner([binary],output/'contract-run.log')
def execute(root,args,output,copies,report,pins,runner):
    from check_native_pc_box import validate_full,assert_unchanged,require_hashes,wasm_exports
    from check_native_save_roundtrip import static_delta
    from prepare_overlay_state import compile_unit
    context=validate_full(args.full_receipt);require(context['fullSourceBankCurrent'],'Current production Full required')
    require('HGNativeOakUIGetState' in context['formal']['shipping']['exports'],'Oak shipping interface absent')
    h=context['helpers'];guest=output/'guest';guest.mkdir();objects=list(context['objects']);units={row['file']:row for row in context['audited']['units']};private=[];original=[];ownership=[]
    try:
        gf=[part for folder in context['includes'] for part in ('-I',str(folder))]+flags(root)
        if context['link'].get('nativeContent'):gf.append('-DHG_NATIVE_ROM_TABLES')
        for name,source in zip(ORIGINAL,copies):
            target=guest/(Path(name).stem+'.o');expected=units[name]['overlayStates'];log=guest/(Path(name).stem+'.log')
            if expected:code,text,state=compile_unit(str(h['SDK']/'bin/clang'),gf,source,target,name,root);log.write_text(text)
            else:row,text=runner([h['SDK']/'bin/clang',*gf,'-c',source,'-o',target],log);code=row['terminalReturnCode'];state={}
            require(code==0 and 'error' not in state and state.get('getter')==(expected[0]['getter'] if expected else None),'Original unit failed:'+name)
            if expected:require(state.get('globals')==expected[0]['globals'],'Overlay globals changed:'+name)
            old=str(context['audit']/'objects'/Path(name).with_suffix('.o'));require(objects.count(old)==1,'One original object required:'+name);objects[objects.index(old)]=str(target);private.append(target);original.append(Path(old));ownership.append(dict(source=name,originalSha256=sha(root/name),copySha256=sha(source),overlayState=state))
        memory=static_delta(private,original,runner,guest);rsp=guest/'objects.rsp';rsp.write_text('\n'.join(shlex.quote(p) for p in objects)+'\n');command=context['link']['command'].copy();command[command.index(context['rsp'])]='@'+str(rsp);module=guest/'engine.wasm';command[command.index('-o')+1]=str(module)
        for kind in ('initial','max'):
            values=[i for i,v in enumerate(command) if v.startswith('-Wl,--'+kind+'-memory=')];require(len(values)==1,'One fixed memory bound');i=values[0];command[i]=command[i].split('=')[0]+'='+str(int(command[i].split('=')[1])+memory['extraFixedMemoryBytes'])
        command.insert(command.index('-o'),'-Wl,--export='+QA_EXPORTS[0]);runner(command,guest/'strict-link.log');exports=wasm_exports(module.read_bytes());require(exports==sorted([*context['formal']['shipping']['exports'],*QA_EXPORTS]),'Exact shipping plus readonly QA getter required')
        report.update(moduleBuilt=True,privateModuleSha256=sha(module),actualExports=exports,sourceCopies=ownership,privateStaticData=memory,fullReceipt=str(args.full_receipt),fullReceiptSha256=sha(args.full_receipt),externalPrivateSourceBank=pins)
        native=output/'native';native.mkdir();generated,writes,restores=h['generate_engine'](native,module,lambda argv,log:runner(argv,native/log));cflags,runtime=h['native_build_settings'](native);cflags+=['-iquote',str(HERE),'-iquote',str(root/'ios/tests'),'-iquote',str(root/'include'),'-iquote',str(root/'ios/Original/abi32'),'-g','-fsanitize=address,undefined','-fno-sanitize-recover=all']
        runner(['xcrun','clang',*cflags,'-fsyntax-only',HOST],native/'host-syntax.log');report.update(genuineHeaderSha256=sha(native/'hgengine.h'),wholeHostSyntaxPassed=True)
        if args.prepare_only:report.update(passed=True,complete=False,status='prepared_genuine_private_oak_abi',gameRuntimeExecuted=False);return
        native_objects=[]
        for i,source in enumerate([*generated,*runtime,HOST]):
            obj=native/f'host-{i}.o';trap=['-DWASM_RT_TRAP_HANDLER=HGEngineTrap'] if source.name=='wasm-rt-impl.c' else [];runner(['xcrun','clang',*cflags,*trap,'-c',source,'-o',obj],native/f'compile-{i}.log');native_objects.append(obj)
        binary=native/'oak-check';runner(['xcrun','clang','-fsanitize=address,undefined',*native_objects,'-o',binary],native/'link.log');boot=output/'boot-header.bin';boot.write_bytes(h['read_boot_header'](context['pack']));frames=output/'frames';frames.mkdir();baseline=output/'platform-baseline';baseline.mkdir();save=output/'newgame.sav';require(not save.exists(),'NewGame path must not preexist')
        assert_unchanged(context);require_hashes(pins,'Reviewed external QA sources')
        row,text=runner([binary,context['pack']/'files',boot,save,frames,baseline],output/'runtime.log',timeout=600,check=False)
        evidence=[json.loads(line[len('OAK evidence: '):]) for line in text.splitlines() if line.startswith('OAK evidence: ')];snapshots=[json.loads(line[len('OAK snapshot: '):]) for line in text.splitlines() if line.startswith('OAK snapshot: ')];report.update(evidence=evidence,snapshots=snapshots,gameRuntimeExecuted='OAK game-started: true' in text)
        require(row['terminalReturnCode']==0 and not row['timedOut'] and row['allChildrenWaited'] and len(evidence)==1 and evidence[0].get('status')=='passed_original_oak_newgame' and evidence[0].get('resetQuiescent') is True,'Original Oak NewGame failed')
        e=evidence[0];require(all(e[k]==1 for k in ('controls','adventure','genderNo','nameNo','oakEntries','oakExits','fieldReady')) and e['namingEntries']==e['namingExits']==2 and e['stage']==19 and e['finalName']==e['expectedName'] and not e['gender'] and not e['party'] and not e['gotStarter'] and e['map']==64 and e['x']==e['z']==6 and e['ballSnapshots']==3,'Incomplete original NewGame evidence')
        require((baseline/'before').read_bytes()==save.read_bytes()==b'\xFF'*524288,'Story save bytes changed');files=sorted(output.glob('newgame.sav*'));require(set(p.name[len(save.name):] for p in files)<=set(('','.console','.wifi','.lock')),'Unexpected backup companion')
        for suffix in ('.console','.lock'):
            before=baseline/('before'+suffix);after=Path(str(save)+suffix)
            if before.exists():require(after.is_file() and before.read_bytes()==after.read_bytes(),'Existing platform companion changed:'+suffix)
        wifi=Path(str(save)+'.wifi')
        if wifi.exists():require(wifi.stat().st_size==1024,'Original platform WiFi companion has wrong size')
        ball=[s for s in snapshots if s['phase'] in (52,53,54,55)];require({s['phase'] for s in ball}>={52,53,54,55},'Original ball/Marill phases missing');require(len({sha(frames/f"oak-{s['sequence']:03d}-engine-0.ppm") for s in ball})>=2,'Original ball/Marill pixels did not change')
        report.update(passed=True,complete=True,status='passed_original_oak_newgame',platformFilesAfter={str(p):{'bytes':p.stat().st_size,'sha256':sha(p)} for p in files},platformBaseline={str(p):{'bytes':p.stat().st_size,'sha256':sha(p)} for p in baseline.iterdir()},storySaveWrites=False,checkedStackWrites=writes,exceptionDepthRestores=restores,allBranchesVerified=False)
    finally:assert_unchanged(context);require_hashes(pins,'Reviewed external QA sources')
def main():
    parser=argparse.ArgumentParser(description=__doc__);parser.add_argument('--source-root',type=Path,required=True);parser.add_argument('--output',type=Path,required=True);parser.add_argument('--self-check',action='store_true');parser.add_argument('--syntax-check',action='store_true');parser.add_argument('--wasi-sdk',type=Path);parser.add_argument('--full-receipt',type=Path);parser.add_argument('--prepare-only',action='store_true');args=parser.parse_args();root=args.source_root.resolve();out=args.output.resolve();require(not out.exists(),'Fresh output required');out.mkdir(parents=True);sys.path.insert(0,str(root/'ios/tools'));from check_native_pc_box import Runner
    report=dict(passed=False,complete=False,moduleBuilt=False,gameRuntimeExecuted=False,commands=[],noAutomaticRetry=True,privateExports=list(QA_EXPORTS),publicABIChanges=False);runner=Runner(report);owned=[HERE/'oak_qa.h',HERE/'oak_guest.inc',HOST,Path(__file__).resolve(),*(root/n for n in ORIGINAL),root/'ios/tools/npc_yesno_boot_sources.py',root/'ios/tests/npc_yesno_boot.inc'];pins={str(p):sha(p) for p in owned}
    try:
        pure_check(root,out,runner);copies=private_candidates(root,out/'source');report['originalBodiesPreserved']=True
        if args.syntax_check:
            require(args.wasi_sdk,'Explicit WASI SDK required')
            for i,source in enumerate(copies):runner([args.wasi_sdk/'bin/clang',*flags(root),'-fsyntax-only',source],out/f'wasi-{i}.log')
        if args.self_check or args.syntax_check:report.update(passed=True,status='passed_pure_oak_contracts',remainingGaps=['No genuine private ABI, host build, original game or NewGame execution.'])
        else:require(args.full_receipt,'Explicit current Full required');execute(root,args,out,copies,report,pins,runner)
    except BaseException as error:report.update(passed=False,firstFailure={'type':type(error).__name__,'message':str(error)})
    finally:
        log=out/'runtime.log'
        if log.exists():report['gameRuntimeExecuted']='OAK game-started: true' in log.read_text(errors='replace')
        report['sourceInputsUnchanged']=pins=={str(p):sha(p) for p in owned};report['passed'] &= report['sourceInputsUnchanged'];report['sourceHashesBefore']=pins;write(out/'report.json',report)
    print(json.dumps({'passed':report['passed'],'report':str(out/'report.json'),'firstFailure':report.get('firstFailure')}));return 0 if report['passed'] else 1
if __name__=='__main__':raise SystemExit(main())
