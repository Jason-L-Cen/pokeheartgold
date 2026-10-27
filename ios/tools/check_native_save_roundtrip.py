#!/usr/bin/env python3
"""Private two-process original Save-menu roundtrip; source checks are safe before a matching Full."""
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
ROOT=Path(__file__).resolve().parents[2]
HERE=ROOT/'ios/tests'
HOST=ROOT/'ios/Original/abi32/save_roundtrip_scenario_check.c'
sys.path.insert(0,str(ROOT/'ios/tools'))
import check_native_trainer_card as trainer
QA_EXPORTS=trainer.QA_EXPORTS
ORIGINAL=(*trainer.ORIGINAL,'src/touch_save_app.c','src/save.c')
def require(value,message):
    if not value:raise RuntimeError(message)
def sha(path):return hashlib.sha256(Path(path).read_bytes()).hexdigest()
def write(path,value):Path(path).write_text(json.dumps(value,indent=2,sort_keys=True)+'\n')
def hook(call):return '/* SRQA_BEGIN */\n'+call+'\n/* SRQA_END */'
def private_candidates(root,destination):
    sources=trainer.private_candidates(root,destination)
    field=destination/trainer.ORIGINAL[0]
    field.write_text(trainer.replace_once(trainer.replace_once(field.read_text(),'#include "trainer_card_guest.inc"','#include "save_roundtrip_guest.inc"'),'    TrainerCardQAFieldTick(fieldSystem);','    SaveRoundtripQAFieldTick(fieldSystem);'))
    touch=(root/ORIGINAL[-2]).read_text()
    task=trainer.function(touch,'TouchSaveApp_Task')
    observed='''    YesNoPrompt *prompt=data->yesNoPrompt;
    BOOL input=(data->state==TOUCHSAVEAPP_STATE_HANDLE_SAVE_CONFIRMATION || data->state==TOUCHSAVEAPP_STATE_HANDLE_OVERWRITE_CONFIRMATION);
    BOOL ready=input && data->fieldSystem && FieldSystem_TaskIsRunning(data->fieldSystem) && data->fieldSystem->startMenuState==2
        && prompt && prompt->buttonsAreInit==1 && prompt->result==3 && !prompt->confirmSelectionTimer && prompt->touchHitboxController
        && !TextPrinterCheckActive(data->textPrinter) && data->bgConfig
        && !((data->bgConfig->bufferTransferScheduled|data->bgConfig->scrollScheduled)&0xF0);
    SaveRoundtripQAUI(data,data->state,ready,ready ? prompt->cursorPos : 0,ready ? prompt->inTouchMode : 0);'''
    touch=trainer.replace_once(touch,task,task[:-1]+hook(observed)+'\n}')
    create=trainer.function(touch,'TouchSaveApp_CreateYesNoPrompt');touch=trainer.replace_once(touch,create,create[:-1]+hook('    SaveRoundtripQAPromptQueued(data);')+'\n}')
    touch=trainer.replace_once(touch,'    DestroySysTaskAndEnvironment(task);',hook('    SaveRoundtripQAUIRetire(data);')+'\n    DestroySysTaskAndEnvironment(task);')
    save=(root/ORIGINAL[-1]).read_text()
    load=trainer.function(save,'SaveData_TryLoadOnContinue')
    load2=trainer.replace_once(load,'        Save_ResetPCBoxModifiedFlags(saveData);','        Save_ResetPCBoxModifiedFlags(saveData);\n'+hook('        SaveRoundtripQALoaded(saveData);'))
    save=trainer.replace_once(save,load,load2)
    for name,real,wrapper in (
        ('SaveGameNormal','SRSaveGameNormalOriginal','''int SaveGameNormal(SaveData *saveData) {
    SaveRoundtripQANormalEntry(saveData);
    int result=SRSaveGameNormalOriginal(saveData);SaveRoundtripQANormalResult(saveData,result);return result;
}'''),
        ('WaitFlashWrite','SRWaitFlashWriteOriginal','''static BOOL WaitFlashWrite(s32 lockId,BOOL checkResult,BOOL *resultSuccess) {
    BOOL done=SRWaitFlashWriteOriginal(lockId,checkResult,resultSuccess);
    SaveRoundtripQAFlash(lockId,checkResult,done,done && checkResult && resultSuccess ? *resultSuccess : 0);return done;
}''')):
        body=trainer.function(save,name);renamed=body.replace(name+'(',real+'(',1)
        save=trainer.replace_once(save,body,renamed+'\n'+hook(wrapper))
    save=trainer.replace_once(save,'    Save_WriteManInit(saveData, &writeManager, 2);','    Save_WriteManInit(saveData, &writeManager, 2);\n'+hook('    SaveRoundtripQAManager(saveData,&writeManager,1);'))
    save=trainer.replace_once(save,'    do {\n        if (writeManager.curSector != 1) {','    do {\n'+hook('        SaveRoundtripQAManager(saveData,&writeManager,1);')+'\n        if (writeManager.curSector != 1) {')
    save=trainer.replace_once(save,'    Save_WriteManFinish(saveData, &writeManager, ret);','    Save_WriteManFinish(saveData, &writeManager, ret);\n'+hook('    SaveRoundtripQAManager(saveData,&writeManager,0);'))
    include='#define HG_SAVE_ROUNDTRIP_GUEST 1\n#include "save_roundtrip_qa.h"\n'
    for name,text in ((ORIGINAL[-2],touch),(ORIGINAL[-1],save)):
        p=destination/name;p.parent.mkdir(parents=True,exist_ok=True);p.write_text(include+text);sources.append(p)
    # Restore hooks and the two wrapper names to verify every original nonblank body once.
    for name in ORIGINAL[-2:]:
        text=(destination/name).read_text().removeprefix(include)
        text=re.sub(r'/\* SRQA_BEGIN \*/\n.*?\n/\* SRQA_END \*/\n?','',text,flags=re.S)
        text=text.replace('SRSaveGameNormalOriginal(','SaveGameNormal(').replace('SRWaitFlashWriteOriginal(','WaitFlashWrite(')
        normalized=lambda value:[line.strip() for line in value.splitlines() if line.strip()]
        require(normalized(text)==normalized((root/name).read_text()),'Original save observer changed a body: '+name)
    return sources
def wire(data,expected_lengths):
    require(len(expected_lengths)==6 and all(type(n) is int and 0<n<=0xFFFFFFFF for n in expected_lengths),'Invalid actual C record lengths')
    require(len(data)>=36,'Truncated record header');header=struct.unpack_from('<9I',data)
    require(header[:7]==(2,*expected_lengths),'Snapshot version/actual C lengths changed')
    require(len(data)==36+sum(expected_lengths),'Truncated/trailing snapshot records')
    offset=36;records=[]
    for length in expected_lengths:records.append(data[offset:offset+length]);offset+=length
    return records
def parser_self_check():
    lengths=[1,2,3,4,5,6];good=struct.pack('<9I',2,*lengths,3000,12345)+bytes(range(21));require(len(wire(good,lengths))==6,'Valid wire rejected')
    for bad in (good[:-1],good+b'x',struct.pack('<I',3)+good[4:],good[:4]+struct.pack('<I',0xFFFFFFFF)+good[8:]):
        try:wire(bad,lengths)
        except RuntimeError:pass
        else:raise RuntimeError('Malformed snapshot accepted')
def prepare_host_support(root,output):
    text=(root/'ios/Original/abi32/trainer_card_scenario_check.c').read_text();anchor='int main(int argc,char **argv)'
    require(text.count(anchor)==1,'Trainer host entry changed');(output/'trainer_roundtrip_support.inc').write_text(text.replace(anchor,'static int SRUnusedTrainerMain(int argc,char **argv)',1))
def flags(root):return ['-I',str(HERE),*trainer.flags(root)]
def data_extent(path,run,log):
    from check_abi32 import WABT
    _,text=run([WABT/'bin/wasm-objdump','-x',path],log)
    section=re.search(r'^Data\[(\d+)\]:',text,re.M)
    entries=re.findall(r'^ - segment\[\d+\](?: <[^>]+>)? memory=\d+ size=(\d+) - init i32=(\d+)',text,re.M)
    require(len(entries)==(int(section[1]) if section else 0),'Unrecognised relocatable WASI data layout')
    return max((int(size)+int(offset) for size,offset in entries),default=0)
def static_delta(private,original,run,output):
    rows=[]
    for i,(new,old) in enumerate(zip(private,original)):
        before=data_extent(old,run,output/f'data-original-{i}.log');after=data_extent(new,run,output/f'data-private-{i}.log')
        rows.append(dict(original=str(old),private=str(new),originalExtent=before,privateExtent=after,addedExtent=max(0,after-before)))
    added=sum(row['addedExtent'] for row in rows)
    return dict(units=rows,addedStaticDataBytes=added,extraFixedMemoryBytes=((added+65535)//65536)*65536)
def pure_check(root,output,run):
    fixture='''#include "global.h"
#include "save_roundtrip_qa.h"
#include <assert.h>
#include <stddef.h>
#include <stdio.h>
int main(void){
    _Static_assert(offsetof(HGSaveRoundtripQAState,trainer)==0,"Trainer prefix");
    HGSaveRoundtripSnapshot expected={.version=HG_SR_VERSION,.lengths={sizeof(Party),sizeof(PCStorage),sizeof(Bag),sizeof(SaveVarsFlags),sizeof(struct SaveTrainerCard),sizeof(SavePokegear)}},actual;
    assert(HGSRWireValid(&expected,sizeof(expected)));actual=expected;
    for(unsigned i=0;i<6;i++){actual=expected;actual.lengths[i]++;assert(!HGSRWireValid(&actual,sizeof(actual)));}
    actual=expected;actual.version=3;assert(!HGSRWireValid(&actual,sizeof(actual)));
    assert(!HGSRWireValid(NULL,sizeof(expected)) && !HGSRWireValid(&expected,sizeof(expected)-1));
    for(unsigned i=0;i<6;i++){unsigned offset=0;for(unsigned j=0;j<i;j++)offset+=expected.lengths[j];actual=expected;actual.records[offset]^=1;assert(HGSRWireValid(&actual,sizeof(actual)) && memcmp(&actual,&expected,sizeof(actual)));}
    actual=expected;actual.money++;assert(memcmp(&actual,&expected,sizeof(actual)));actual=expected;actual.trainerID++;assert(memcmp(&actual,&expected,sizeof(actual)));
    printf("SR_LAYOUT {\\"stateBytes\\":%zu,\\"wireBytes\\":%zu,\\"snapshotOffset\\":%zu,\\"lengths\\":[%zu,%zu,%zu,%zu,%zu,%zu]}\\n",sizeof(HGSaveRoundtripQAState),sizeof(HGSaveRoundtripSnapshot),offsetof(HGSaveRoundtripQAState,snapshot),sizeof(Party),sizeof(PCStorage),sizeof(Bag),sizeof(SaveVarsFlags),sizeof(struct SaveTrainerCard),sizeof(SavePokegear));
    return 0;
}'''
    (output/'record-check.c').write_text(fixture)
    run(['xcrun','clang','-std=gnu11','-O1','-fsanitize=address,undefined','-fno-sanitize-recover=all','-DHEARTGOLD_NATIVE','-DSDK_ARM9',
        '-iquote',HERE,'-iquote',root/'ios/tests','-iquote',root/'include','-iquote',root/'include/library','-idirafter',root/'lib/include','-idirafter',root/'ios/build-original-host/include',
        output/'record-check.c','-o',output/'record-check'],output/'record-compile.log')
    _,text=run([output/'record-check'],output/'record-run.log');rows=[json.loads(x[len('SR_LAYOUT '):]) for x in text.splitlines() if x.startswith('SR_LAYOUT ')]
    require(len(rows)==1,'Actual C wire layout missing');parser_self_check();return rows[0]
def evidence_passed(value,role):
    common=isinstance(value,dict) and value.get('role')==role and type(value.get('processId')) is int and value['processId']>0 and value.get('done')==value.get('fieldReady')==value.get('snapshotWritten')==1 and value.get('resetQuiescent') is True
    return common and (value.get('status')=='passed_save_roundtrip_writer' and value.get('trainerConfirms')==1 and value.get('saveCalls')==value.get('saveSuccess')==value.get('writerFrozen')==1
        and value.get('beforeFrozen')==value.get('beforeWritten')==value.get('preservationChecked')==value.get('preservationPassed')==1
        and value.get('lastSaveStatus')==2 and value.get('promptMask')==3 and value.get('managerIsStack')==1 and value.get('managerLive')==0 and value.get('flashCompleted',0)>0 if role==1
        else value.get('status')=='passed_save_roundtrip_reader' and value.get('readerFrozen')==value.get('loadSuccess')==value.get('readerCompared')==1)
def evidence_self_check():
    writer=dict(role=1,processId=101,done=1,fieldReady=1,snapshotWritten=1,resetQuiescent=True,status='passed_save_roundtrip_writer',trainerConfirms=1,saveCalls=1,saveSuccess=1,writerFrozen=1,beforeFrozen=1,beforeWritten=1,preservationChecked=1,preservationPassed=1,lastSaveStatus=2,promptMask=3,managerIsStack=1,managerLive=0,flashCompleted=1)
    require(evidence_passed(writer,1),'Real successful writer evidence rejected')
    for key,value in (('status','saved_message_only'),('lastSaveStatus',3),('saveCalls',2),('promptMask',1),('managerLive',1),('fieldReady',0),('resetQuiescent',False),('beforeFrozen',0),('preservationPassed',0)):
        require(not evidence_passed({**writer,key:value},1),'False writer success accepted: '+key)
    reader=dict(role=2,processId=102,done=1,fieldReady=1,snapshotWritten=1,resetQuiescent=True,status='passed_save_roundtrip_reader',readerFrozen=1,loadSuccess=1,readerCompared=1)
    require(evidence_passed(reader,2),'Real successful reader evidence rejected')
    for key,value in (('readerCompared',0),('loadSuccess',2),('readerFrozen',0),('snapshotWritten',0),('role',1),('processId',0)):
        require(not evidence_passed({**reader,key:value},2),'False reader success accepted: '+key)
def execute(root,args,output,copies,layout,report,pins,runner):
    from check_native_pc_box import validate_full,assert_unchanged,require_hashes,wasm_exports
    from prepare_overlay_state import compile_unit
    context=validate_full(args.full_receipt);require(context['fullSourceBankCurrent'],'A fresh Full must bind this integrated four-file source bank')
    original=args.save.resolve();fixture=[Path(str(original)+s) for s in ('','.console','.wifi','.lock')]
    require(not args.save.is_symlink() and all(p.is_file() and not p.is_symlink() for p in fixture) and set(fixture)==set(original.parent.glob(original.name+'*')),'Exactly four original regular fixture files required')
    original_hashes={str(p):sha(p) for p in fixture};h=context['helpers'];guest=output/'guest';guest.mkdir();objects=list(context['objects']);units={r['file']:r for r in context['audited']['units']}
    guestflags=flags(root);guestflags[:0]=[part for folder in context['includes'] for part in ('-I',str(folder))]
    if context['link'].get('nativeContent'):guestflags.append('-DHG_NATIVE_ROM_TABLES')
    try:
        require_hashes(pins,'Save QA sources');replacements=[];private_objects=[];original_objects=[]
        for name,source in zip(ORIGINAL,copies):
            target=guest/(Path(name).stem+'.o');expected=units[name]['overlayStates'];log=guest/(Path(name).stem+'.log')
            if expected:
                code,text,state=compile_unit(str(h['SDK']/'bin/clang'),guestflags,source,target,name,root);log.write_text(text)
            else:
                row,text=runner([h['SDK']/'bin/clang',*guestflags,'-c',source,'-o',target],log);code=row['terminalReturnCode'];state={}
            require(code==0 and 'error' not in state and state.get('getter')==(expected[0]['getter'] if expected else None),'Original unit/overlay ownership failed: '+name)
            old=str(context['audit']/'objects'/Path(name).with_suffix('.o'));require(objects.count(old)==1,'Original object identity missing: '+name);objects[objects.index(old)]=str(target)
            private_objects.append(target);original_objects.append(Path(old))
            replacements.append(dict(source=name,originalSha256=sha(root/name),candidateSha256=sha(source),objectSha256=sha(target),overlayState=state))
        rsp=guest/'objects.rsp';rsp.write_text('\n'.join(shlex.quote(p) for p in objects)+'\n');command=context['link']['command'].copy();command[command.index(context['rsp'])]='@'+str(rsp)
        # Measure all seven actual WASI objects, including embedded Trainer buffers, before fixing private memory.
        memory=static_delta(private_objects,original_objects,runner,guest);extra=memory['extraFixedMemoryBytes'];report['privateStaticData']=memory
        for kind in ('initial','max'):
            matches=[i for i,p in enumerate(command) if p.startswith('-Wl,--'+kind+'-memory=')];require(len(matches)==1,'One fixed memory flag required');i=matches[0];command[i]=command[i].split('=')[0]+'='+str(int(command[i].split('=')[1])+extra)
        module=guest/'engine.wasm';command[command.index('-o')+1]=str(module)
        for name in QA_EXPORTS:command.insert(command.index('-o'),'-Wl,--export='+name)
        runner(command,guest/'strict-link.log');require(wasm_exports(module.read_bytes())==sorted([*context['formal']['shipping']['exports'],*QA_EXPORTS]),'Only original public exports plus Trainer private two permitted')
        report.update(moduleBuilt=True,privateModuleSha256=sha(module),privateExtraMemoryBytes=extra,sourceCopies=replacements)
        native=output/'native';native.mkdir();prepare_host_support(root,native);generated,writes,restores=h['generate_engine'](native,module,lambda argv,log:runner(argv,native/log))
        cflags,runtime=h['native_build_settings'](native);cflags+=['-I',str(HERE),'-I',str(root/'ios/tests'),'-I',str(root/'ios/Original/abi32'),'-g','-fsanitize=address,undefined','-fno-sanitize-recover=all']
        native_objects=[]
        for i,source in enumerate([*generated,*runtime,HOST]):
            obj=native/f'host-{i}.o';trap=['-DWASM_RT_TRAP_HANDLER=HGEngineTrap'] if source.name=='wasm-rt-impl.c' else []
            runner(['xcrun','clang',*cflags,*trap,'-c',source,'-o',obj],native/f'compile-{i}.log');native_objects.append(obj)
        binary=native/'save-roundtrip-check';runner(['xcrun','clang','-fsanitize=address,undefined',*native_objects,'-o',binary],native/'link.log')
        boot=output/'boot-header.bin';boot.write_bytes(h['read_boot_header'](context['pack']))
        private=[]
        # Copy exactly once, before writer; reader below opens these SAME four files without another copy.
        for p in fixture:
            copy=output/('scenario.sav'+p.name[len(original.name):]);shutil.copy2(p,copy);private.append(copy)
        report['fixtureCopyCount']=1;before_writer={str(p):sha(p) for p in private};phase_evidence=[]
        for role in ('writer','reader'):
            assert_unchanged(context);require_hashes(pins,'Save QA sources');require_hashes(original_hashes,'Original protected fixture')
            frames=output/(role+'-frames');frames.mkdir();snapshot=output/('expected.bin' if role=='writer' else 'actual.bin')
            argv=[binary,context['pack']/'files',boot,output/'scenario.sav',frames,role,snapshot,output/'expected.bin' if role=='reader' else '-']
            report['gameRuntimeExecuted']=True;row,text=runner(argv,output/(role+'.log'),timeout=300,check=False)
            evidence=[json.loads(x[len('SAVE evidence: '):]) for x in text.splitlines() if x.startswith('SAVE evidence: ')]
            phase_evidence.append(dict(role=role,evidence=evidence,terminal=row));report['phases']=phase_evidence
            require(row['terminalReturnCode']==0 and not row['timedOut'] and row['allChildrenWaited'] and len(evidence)==1 and evidence_passed(evidence[0],1 if role=='writer' else 2),'Original '+role+' failed; reader is never started after writer failure')
            records=wire(snapshot.read_bytes(),layout['lengths']);require(any(records[4][32:]),'Saved signature must be nonempty')
            if role=='writer':
                wire((output/'before.bin').read_bytes(),layout['lengths'])
                after_writer={str(p):sha(p) for p in private};require(after_writer[str(output/'scenario.sav')]!=before_writer[str(output/'scenario.sav')],'Real successful writer must change private save bytes');report['privateFilesAfterWriter']=after_writer
            else:require({str(p):sha(p) for p in private}==after_writer,'Reader must not write the private save set')
        require((output/'expected.bin').read_bytes()==(output/'actual.bin').read_bytes(),'Six complete reloaded records/Money/TrainerID differ')
        require(phase_evidence[0]['evidence'][0]['processId']!=phase_evidence[1]['evidence'][0]['processId'],'Writer and reader must be distinct new processes')
        report.update(passed=True,complete=True,status='passed_original_two_process_save_roundtrip',allSixRecordsMoneyTrainerIDByteIdentical=True,readerReusedWriterPrivateFiles=True,expectedSha256=sha(output/'expected.bin'),actualSha256=sha(output/'actual.bin'),originalFixtureUnchanged=True,checkedStackWrites=writes,exceptionDepthRestores=restores)
    finally:
        assert_unchanged(context);require_hashes(pins,'Save QA sources');require_hashes(original_hashes,'Original protected fixture')
def main():
    parser=argparse.ArgumentParser(description=__doc__);parser.add_argument('--source-root',type=Path,default=ROOT);parser.add_argument('--self-check',action='store_true');parser.add_argument('--syntax-check',action='store_true');parser.add_argument('--wasi-sdk',type=Path)
    parser.add_argument('--full-receipt',type=Path);parser.add_argument('--save',type=Path);parser.add_argument('--output',type=Path,required=True);args=parser.parse_args();root=args.source_root.resolve();output=args.output.resolve();require(not output.exists(),'Fresh output required');output.mkdir(parents=True)
    sys.path.insert(0,str(root/'ios/tools'));from check_native_pc_box import Runner
    report=dict(passed=False,complete=False,moduleBuilt=False,gameRuntimeExecuted=False,noAutomaticRetry=True,commands=[],privateExports=list(QA_EXPORTS),publicABIChanges=False)
    runner=Runner(report);owned=[HERE/'save_roundtrip_qa.h',HERE/'save_roundtrip_guest.inc',HOST,Path(__file__).resolve(),*(root/p for p in ORIGINAL),root/'ios/Original/abi32/trainer_card_scenario_check.c',root/'ios/tests/trainer_card_guest.inc']
    before={str(p):sha(p) for p in owned}
    try:
        layout=pure_check(root,output,runner);evidence_self_check();report['layout']=layout;copies=private_candidates(root,output/'source');report['originalBodiesPreserved']=True
        if args.syntax_check:
            require(args.wasi_sdk,'Explicit installed WASI SDK required')
            for i,source in enumerate(copies):runner([args.wasi_sdk/'bin/clang',*flags(root),'-fsyntax-only',source],output/f'wasi-{i}.log')
            from prepare_overlay_state import compile_unit
            audited=json.loads((root/'ios/build-original-abi32/audit/report.json').read_text());units={row['file']:row for row in audited['units']}
            object_flags=flags(root);link_meta=json.loads((root/'ios/build-original-abi32/audit/engine-link.json').read_text())
            if link_meta.get('nativeContent'):object_flags=['-I',str(root/link_meta['nativeContent']['include']),*object_flags,'-DHG_NATIVE_ROM_TABLES']
            private_objects=[];original_objects=[];objects=output/'static-data-objects';objects.mkdir()
            for i,(name,source) in enumerate(zip(ORIGINAL,copies)):
                require(sha(root/name)==units[name]['sha256'],'Original audited body changed: '+name);target=objects/f'unit-{i}.o';expected=units[name]['overlayStates']
                if expected:
                    code,text,state=compile_unit(str(args.wasi_sdk/'bin/clang'),object_flags,source,target,name,root);(objects/f'unit-{i}.log').write_text(text)
                else:
                    row,text=runner([args.wasi_sdk/'bin/clang',*object_flags,'-c',source,'-o',target],objects/f'unit-{i}.log');code=row['terminalReturnCode'];state={}
                require(code==0 and 'error' not in state and state.get('getter')==(expected[0]['getter'] if expected else None),'Private source ownership failed: '+name)
                private_objects.append(target);original_objects.append(root/'ios/build-original-abi32/audit/objects'/Path(name).with_suffix('.o'))
            report['privateStaticData']=static_delta(private_objects,original_objects,runner,objects)
        if args.self_check or args.syntax_check:report.update(passed=True,status='passed_pure_save_contracts',remainingGaps=['No genuine private generated header/whole-host ABI yet.','No module/game/disk save/second-process runtime has been executed.'])
        else:
            require(args.full_receipt and args.save,'Runtime requires matching Full and original protected fixture');execute(root,args,output,copies,layout,report,before,runner)
    except BaseException as error:report.update(passed=False,firstFailure=dict(type=type(error).__name__,message=str(error)))
    finally:
        report['sourceInputsUnchanged']=before=={str(p):sha(p) for p in owned};report['passed'] &= report['sourceInputsUnchanged'];report['sourceHashesBefore']=before;write(output/'report.json',report)
    print(('PASS' if report['passed'] else 'FAIL')+': '+str(output/'report.json'));return 0 if report['passed'] else 1
if __name__=='__main__':raise SystemExit(main())
