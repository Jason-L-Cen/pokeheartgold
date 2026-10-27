"""Private original-NPC YES/NO draft; requires a matching future Full and explicit fixture."""
import argparse
from concurrent.futures import ThreadPoolExecutor
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
import tempfile

sys.dont_write_bytecode = True
os.environ['HEARTGOLD_TEST_MUTE'] = os.environ['SIMCTL_CHILD_HEARTGOLD_TEST_MUTE'] = '1'
ROOT = Path(__file__).resolve().parents[2]
HERE = ROOT / 'ios/tests'
HOST = ROOT / 'ios/Original/abi32/npc_yesno_scenario_check.c'
sys.path.insert(0, str(ROOT / 'ios/tools'))
sys.path.insert(0, str(HERE))
from check_native_pc_box import (Runner, validate_full, assert_unchanged, file_sha, wasm_exports, require_hashes)
from prepare_overlay_state import compile_unit
from npc_yesno_boot_sources import build_candidates

QA_EXPORTS = ('HGNativeNPCYesNoQAGetState', 'HGNativeNPCYesNoQARequest')
OWNED = (HERE/'npc_yesno_qa.h', HERE/'npc_yesno_guest.inc', HERE/'npc_yesno_boot.inc', HOST,
    HERE/'npc_yesno_contract_check.c', ROOT/'ios/tools/npc_yesno_boot_sources.py', Path(__file__).resolve())
ORIGINAL = ('src/field_system.c', 'src/title_screen.c', 'src/application/main_menu/main_menu.c')

def require(value, message):
    if not value:
        raise RuntimeError(message)

def sha(path):
    return file_sha(Path(path))

def write(path, value):
    Path(path).write_text(json.dumps(value, indent=2, sort_keys=True) + '\n')

def field_candidate(source):
    anchor = 'BOOL finished = FieldSystem_Main(fieldSystem);'
    require(source.count(anchor) == 1, 'Original resident field observation anchor changed')
    return '#include "' + str(HERE / 'npc_yesno_guest.inc') + '"\n#line 1 "src/field_system.c"\n' \
        + source.replace(anchor, anchor + '\n    NPCYNFieldTick(fieldSystem);')

def prepare_guest(destination, runner, context):
    h = context['helpers']; audit = context['audit']; destination.mkdir()
    source_dir = destination / 'source'; source_dir.mkdir()
    title, menu = build_candidates(ROOT, source_dir)
    field = source_dir / ORIGINAL[0]; field.parent.mkdir(parents=True, exist_ok=True)
    field.write_text(field_candidate((ROOT / ORIGINAL[0]).read_text()))
    flags = ['--target=wasm32-wasip1', '-std=gnu11', '-O2', '-fwrapv', '-fno-strict-aliasing', '-fno-builtin-malloc', '-fno-builtin-free',
        '-fwasm-exceptions', '-fexceptions', '-Xclang', '-disable-red-zone', '-fms-extensions', '-Wno-microsoft', '-Wno-unknown-pragmas',
        '-DHEARTGOLD_NATIVE', '-DHEARTGOLD_ABI32', '-DSDK_ARM9', '-DSDK_CW', '-DSDK_CODE_ARM', '-DSDK_TS', '-D_NITRO',
        '-DPM_KEEP_ASSERTS', '-DPLATFORM_INTRINSIC_FUNCTION_BIT_CLZ32=__builtin_clz', '-I', str(HERE)]
    for folder in context['includes']: flags += ['-I', str(folder)]
    if context['link'].get('nativeContent'): flags.append('-DHG_NATIVE_ROM_TABLES')
    objects = list(context['objects']); units = {row['file']: row for row in context['audited']['units']}; replacements = []
    for name, source in zip(ORIGINAL, (field, title, menu)):
        target = destination / (Path(name).stem + '.o')
        expected = units[name]['overlayStates']; log=destination / (Path(name).stem + '-compile.log')
        if expected:
            code, diagnostics, state = compile_unit(str(h['SDK'] / 'bin/clang'), flags, source, target, name, ROOT)
            log.write_text(diagnostics)
        else:
            row, diagnostics = runner([h['SDK']/'bin/clang',*flags,'-c',source,'-o',target],log)
            code=row['terminalReturnCode']; state=dict(key=name)
        require(code == 0 and 'error' not in state, 'Private original source-copy compile failed: ' + name)
        require((state.get('getter') if state.get('getter') else None)
            == (expected[0]['getter'] if expected else None), 'Private source copy changed overlay ownership: ' + name)
        old = str(audit / 'objects' / Path(name).with_suffix('.o'))
        require(objects.count(old) == 1, 'Expected one audited original object: ' + name)
        objects[objects.index(old)] = str(target)
        replacements.append(dict(original=name, originalSha256=sha(ROOT / name), privateSource=str(source), privateSourceSha256=sha(source),
            privateObject=str(target), privateObjectSha256=sha(target), overlayState=state))
    response = destination / 'objects.rsp'; response.write_text('\n'.join(shlex.quote(name) for name in objects) + '\n')
    command = context['link']['command'].copy(); command[command.index(context['rsp'])] = '@' + str(response)
    module = destination / 'engine.wasm'; command[command.index('-o') + 1] = str(module)
    for name in QA_EXPORTS: command.insert(command.index('-o'), '-Wl,--export=' + name)
    for name in ('ScrCmd_NPCMsg', 'ScrCmd_WaitButton', 'ScrCmd_UnlockRandomTrendySaying'): command.insert(command.index('-o'), '-Wl,--wrap=' + name)
    runner(command, destination / 'strict-link.log')
    require(wasm_exports(module.read_bytes()) == sorted([*context['formal']['shipping']['exports'], *QA_EXPORTS]),
        'Private module must contain exactly shipping exports plus two NPC QA exports')
    write(destination / 'preparation.json', dict(passed=True, sourceCopies=replacements, privateModuleSha256=sha(module),
        shippingModuleSha256=context['link']['outputSha256'], privateExports=list(QA_EXPORTS), originalFieldHookCount=1,
        commandWrappers=['ScrCmd_NPCMsg', 'ScrCmd_WaitButton', 'ScrCmd_UnlockRandomTrendySaying'], syntheticSaveSetters=False, productionSourceChanged=False))
    return module

def evidence_passed(row):
    values = dict(qaVersion=2, qaBytes=276, completedNo=2, completedYes=1, completedDaily=1, conservationChecks=5, checkpoints=6,
        messageSerial=7, waitSerial=4, waitAckSerial=4, phase5Mask=7, phase6Mask=7, continueExit=1,
        yesBeforeResult=0, yesSerial=1, yesFlagSet=1, varsLedger=1, easyChatLedger=1)
    return isinstance(row, dict) and row.get('status') == 'passed_original_npc_yesno' \
        and all(type(row.get(name)) is int and row[name] == value for name, value in values.items()) \
        and type(row.get('newWordId')) is int and 0 <= row['newWordId'] < 32 \
        and type(row.get('staleRejected')) is int and row['staleRejected'] >= 6 \
        and type(row.get('snapshots')) is int and row['snapshots'] >= 10 \
        and row.get('privateSaveWrites') is False and row.get('resetQuiescent') is True

def input_trace_passed(rows):
    if not isinstance(rows, list) or not rows or len(rows) % 3:
        return False
    presses = []
    previous=None
    for index in range(0, len(rows), 3):
        press, release, settled = rows[index:index+3]
        if [row.get('event') for row in (press, release, settled)] != ['press', 'released', 'settled']:
            return False
        if any(type(row.get(name)) is not int or not 0<=row[name]<=0xFFFFFFFF
                for row in (press, release, settled) for name in ('key', 'padReads', 'frame', 'cycle')):
            return False
        if press['owner'] != release['owner'] or press['owner'] != settled['owner'] or release['key'] or settled['key'] \
                or release['padReads'] - press['padReads'] < 2 or settled['padReads'] - release['padReads'] < 2:
            return False
        for row in (press,release,settled):
            if previous and (row['frame']<previous['frame'] or row['padReads']<previous['padReads']): return False
            previous=row
        presses.append(press)
    gameplay = [(row['owner'], row['key'], row['cycle']) for row in presses if row['owner'] in ('field', 'choice', 'message')]
    if gameplay != [('field', 1, 0), ('choice', 128, 0), ('choice', 64, 0), ('choice', 2, 0), ('message', 1, 0),
            ('field', 1, 1), ('choice', 128, 1), ('choice', 1, 1), ('message', 1, 1),
            ('field', 1, 2), ('choice', 1, 2), ('message', 1, 2), ('field', 1, 3), ('message', 1, 3)]:
        return False
    printer = [row for row in presses if row['owner']=='printer']
    if len(printer)>8 or any(row['cycle']!=2 or row['key']!=1 or row.get('messageId')!=2 or row.get('printerReady')!=1
            or type(row.get('printerSerial')) is not int or row['printerSerial']!=i+1 for i,row in enumerate(printer)):
        return False
    yes_choice = next(i for i,row in enumerate(presses) if row['owner']=='choice' and row['cycle']==2)
    yes_message = next(i for i,row in enumerate(presses) if row['owner']=='message' and row['cycle']==2)
    if any(not yes_choice<i<yes_message for i,row in enumerate(presses) if row['owner']=='printer'):
        return False
    boot = [row for row in presses if row['owner'] not in ('field', 'choice', 'message', 'printer')]
    roles = [row['owner'] for row in boot]
    if roles not in (['title', 'continue'], ['title', 'mode_switch', 'continue'], ['continue'], ['mode_switch', 'continue']):
        return False
    for row in boot:
        if any(type(row.get(name)) is not int or not 0<=row[name]<=0xFFFFFFFF for name in ('bootKind','bootReady','bootActive','bootGeneration','bootRevision','bootPhase','bootMode','bootOption','bootNotice','bootExit','bootProgress')):
            return False
        if row['key']!=1 or row['bootReady']!=1 or row['bootActive']!=1 or not row['bootGeneration'] or not row['bootRevision']:
            return False
        if row['owner']=='title':
            if row['bootKind']!=1 or row['bootPhase']!=2 or row['bootMode'] or row['bootOption'] or row['bootNotice'] or row['bootExit'] or row['bootProgress']&15!=15: return False
        elif row['bootKind']!=2 or row['bootPhase']!=5 or row['bootMode']!=(1 if row['owner']=='mode_switch' else 0) or row['bootOption']>=9 or row['bootNotice']!=15 or row['bootExit'] or row['bootProgress']&255!=255:
            return False
    if 'mode_switch' in roles:
        switch=boot[roles.index('mode_switch')]; confirm=boot[-1]
        if switch['bootGeneration']!=confirm['bootGeneration'] or switch['bootRevision']==confirm['bootRevision']: return False
    for row in presses:
        if row['owner']!='message': continue
        if any(type(row.get(name)) is not int for name in ('messageId','result','waitReady','waitSerial')): return False
        if row['messageId']!=(3 if row['cycle']<2 else 2 if row['cycle']==2 else 4) or row['waitReady']!=1 or row['waitSerial']!=row['cycle']+1: return False
        if row['cycle']<2 and row['result']!=1: return False
        if row['cycle']>=2 and (type(row.get('yesBeforeResult')) is not int or row['yesBeforeResult']!=0
                or row.get('yesSerial')!=1 or type(row.get('newWordId')) is not int or not 0<=row['newWordId']<32): return False
        if row['cycle']==2 and row['result']!=row['newWordId']: return False
    return True

def snapshot_files_passed(folder, rows, inputs):
    if not isinstance(rows,list) or len(rows)<10: return False
    for index,row in enumerate(rows):
        if not isinstance(row,dict) or row.get('sequence')!=index or not isinstance(row.get('label'),str) \
                or not re.fullmatch(r'[a-z0-9-]+',row['label']): return False
        if any(type(row.get(name)) is not int or not 0<=row[name]<=0xFFFFFFFF
                for name in ('sequence','frame','phase','ownerGeneration','revision','requiredMask','renderedMask','variedMask')): return False
        required,rendered,varied=(row[name] for name in ('requiredMask','renderedMask','variedMask'))
        if not 1<=required<=3 or rendered&~3 or varied&~rendered or rendered&varied&required!=required: return False
        base=folder/f"snapshot-{index:03d}-{row['label']}"
        state=Path(str(base)+'-state.bin').read_bytes(); choice=Path(str(base)+'-choice.bin').read_bytes()
        if len(state)!=276 or struct.unpack_from('<II',state)!=(2,276) or len(choice)!=36: return False
        if struct.unpack_from('<II',choice,4)!=(row['ownerGeneration'],row['revision']): return False
        if struct.unpack_from('<I',choice,16)[0]!=row['phase']: return False
        boot=struct.unpack_from('<11I',state,176)
        if row['label'] not in ('title','mode_switch','continue') and (boot[0]!=2 or boot[1] or not boot[2] or not boot[3] or boot[4] or boot[5]!=8 or boot[9]!=1): return False
        for engine in range(2):
            if not rendered&(1<<engine): continue
            data=Path(str(base)+f'-engine-{engine}.ppm').read_bytes(); header=b'P6\n256 192\n255\n'
            if not data.startswith(header) or len(data)!=len(header)+256*192*3: return False
            pixels=data[len(header):]; measured=any(pixels[i:i+3]!=pixels[:3] for i in range(3,len(pixels),3))
            if measured!=bool(varied&(1<<engine)): return False
    for press in (row for row in inputs if row['event']=='press' and row['owner'] in ('title','mode_switch','continue')):
        matches=[row for row in rows if row['label']==press['owner'] and row['frame']==press['frame']]
        if len(matches)!=1 or not matches[0]['requiredMask']&1: return False
        row=matches[0]; state=(folder/f"snapshot-{row['sequence']:03d}-{row['label']}-state.bin").read_bytes()
        if struct.unpack_from('<11I',state,176)!=tuple(press['boot'+name] for name in
                ('Kind','Active','Generation','Revision','Ready','Phase','Mode','Option','Notice','Exit','Progress')): return False
    return True

def check_c_contracts():
    from check_native_rainy_field import INCLUDES
    from prepare_original_headers import register_header, overlay_objects
    with tempfile.TemporaryDirectory(prefix='npc-yn-contract-') as directory:
        temp=Path(directory); (temp/'hg_native_registers.h').write_text(register_header())
        overlays=overlay_objects((ROOT/'main.lsf').read_text())
        (temp/'hg_native_overlay_ids.h').write_text('#pragma once\n'+''.join(
            f"#define HG_OVERLAY_ID_{row['name']} {index}\n" for index,row in enumerate(overlays))+f'#define HG_OVERLAY_COUNT {len(overlays)}\n')
        flags=['-std=gnu11','-O2','-fwrapv','-fno-strict-aliasing','-fms-extensions','-Wno-microsoft','-Wno-unknown-pragmas',
            '-Wno-pointer-to-int-cast','-Wno-int-to-pointer-cast','-ffunction-sections','-fdata-sections','-Wl,-dead_strip',
            '-fsanitize=address,undefined','-fno-sanitize-recover=all']
        defines=['HEARTGOLD_NATIVE','SDK_ARM9','SDK_CW','SDK_CODE_ARM','SDK_TS','_NITRO','PM_KEEP_ASSERTS',
            'PLATFORM_INTRINSIC_FUNCTION_BIT_CLZ32=__builtin_clz']
        includes=[temp,HERE,*[ROOT/name for name in INCLUDES if name!='ios/build-original-host/include']]
        flags += ['-D'+name for name in defines]
        for folder in includes: flags += ['-I',str(folder)]
        for mode in ('boot','ledger'):
            binary=temp/mode
            mode_flags=[*flags,'-DNPCYN_CHECK_LEDGER'] if mode=='ledger' else ['-std=c11','-O2',
                '-fsanitize=address,undefined','-fno-sanitize-recover=all','-iquote',str(HERE),'-iquote',str(ROOT/'include')]
            subprocess.run(['xcrun','clang',*mode_flags,
                str(HERE/'npc_yesno_contract_check.c'),'-o',str(binary)],check=True,timeout=60)
            subprocess.run([str(binary)],check=True,timeout=20)
        import npc_yesno_boot_sources as hooks
        title,menu=hooks.build_candidates(ROOT,temp/'copies')
        for path,kind,name in ((title,'title',hooks.TITLE_NAME),(menu,'main-menu',hooks.MAIN_MENU_NAME)):
            blocks=[hooks.include_block(kind)]
            if kind=='title': blocks += [hooks.call_block(kind,'terminal','            ','NPCYNBootTitleTerminal(data, *state);'),
                hooks.call_block(kind,'normal','    ','NPCYNBootPublishTitle(data, *state);')]
            else: blocks += [hooks.call_block(kind,'notice','        ','NPCYNBootPublishMainMenu(data, *state);'),
                hooks.call_block(kind,'normal','    ','NPCYNBootPublishMainMenu(data, *state);'),
                hooks.call_block(kind,'terminal','        ','NPCYNBootMainMenuTerminal(data, *state);')]
            body=path.read_text().removeprefix(f'#line 1 "{name}"\n')
            for block in blocks:
                require(body.count(block)==1,'Source-copy hook count drift: '+name); body=body.replace(block,'',1)
            require(body==(ROOT/name).read_text(),'Original source changed outside explicit read-only hooks: '+name)

def self_check():
    source = (HERE / 'npc_yesno_guest.inc').read_text(); header = (HERE / 'npc_yesno_qa.h').read_text()
    require(not re.search(r'\b(?:CreateMon|SetMonData|Party_AddMon|PCStorage_PlaceMon\w*|Save_VarsFlags_SetFlagInArray)\s*\(', source), 'Synthetic save setters escaped into NPC observer')
    for name in ('ScrCmd_NPCMsg', 'ScrCmd_WaitButton', 'ScrCmd_UnlockRandomTrendySaying'):
        require(len(re.findall(r'\b__real_' + name + r'\s*\(ctx\)', source)) == 1, 'Original command must be called exactly once')
    for name in ('SaveArray_Party_sizeof', 'PCStorage_sizeof', 'Save_Bag_sizeof', 'Save_VarsFlags_sizeof', 'Save_EasyChat_sizeof', 'PlayerProfile_GetMoney'):
        require(name in source, 'Missing complete save conservation getter/size: ' + name)
    require(header.count('HGNativeNPCYesNoQAGetState') == header.count('HGNativeNPCYesNoQARequest') == 1, 'Unexpected private export declarations')
    fake = 'before\nBOOL finished = FieldSystem_Main(fieldSystem);\nafter\n'; copy = field_candidate(fake)
    require(copy.count('NPCYNFieldTick(fieldSystem);') == 1 and fake in copy.replace('\n    NPCYNFieldTick(fieldSystem);', ''), 'Original field body changed')
    valid = dict(status='passed_original_npc_yesno',qaVersion=2,qaBytes=276,completedNo=2,completedYes=1,completedDaily=1,conservationChecks=5,checkpoints=6,
        messageSerial=7,waitSerial=4,waitAckSerial=4,phase5Mask=7,phase6Mask=7,staleRejected=6,snapshots=24,continueExit=1,
        yesBeforeResult=0,newWordId=5,yesSerial=1,yesFlagSet=1,varsLedger=1,easyChatLedger=1,privateSaveWrites=False,resetQuiescent=True)
    require(evidence_passed(valid), 'Valid NPC evidence rejected')
    for name, value in (('completedNo',1),('qaBytes',1372),('phase6Mask',1),('waitAckSerial',1),('conservationChecks',True),('privateSaveWrites',True)):
        require(not evidence_passed({**valid,name:value}), 'Malformed NPC evidence accepted: ' + name)
    rows = []; read = 10
    examples=[('title',1,0),('continue',1,0),('field',1,0),('choice',128,0),('choice',64,0),('choice',2,0),('message',1,0),
        ('field',1,1),('choice',128,1),('choice',1,1),('message',1,1),('field',1,2),('choice',1,2),('printer',1,2),('message',1,2),('field',1,3),('message',1,3)]
    for ordinal,(owner,key,cycle) in enumerate(examples):
        common = dict(owner=owner,cycle=cycle,bootReady=1,bootActive=1,bootGeneration=1,bootRevision=1,
            bootKind=1 if owner=='title' else 2,bootPhase=2 if owner=='title' else 5,bootMode=0,bootOption=0,bootNotice=0 if owner=='title' else 15,bootExit=0,bootProgress=15 if owner=='title' else 255,
            messageId=3 if cycle<2 else 2 if cycle==2 else 4,result=1 if cycle<2 else 5,waitReady=1,waitSerial=cycle+1,
            yesBeforeResult=0,yesSerial=1,newWordId=5,printerReady=1,printerSerial=1)
        rows += [{**common,'event':'press','key':key,'padReads':read,'frame':read},
            {**common,'event':'released','key':0,'padReads':read+2,'frame':read+2}, {**common,'event':'settled','key':0,'padReads':read+4,'frame':read+4}]; read += 6
    require(input_trace_passed(rows) and not input_trace_passed(rows[:-1])
        and not input_trace_passed([{**rows[0],'key':True},*rows[1:]]) and not input_trace_passed(rows+rows), 'NPC input lease/route validation failed')
    require(not input_trace_passed([{**rows[0],'bootReady':True},*rows[1:]])
        and not input_trace_passed([*rows[:-3],{**rows[-3],'waitReady':0},*rows[-2:]]), 'Untyped boot or non-WaitButton message input accepted')
    for bit in range(4): require(not input_trace_passed([{**rows[0],'bootProgress':15^(1<<bit)},*rows[1:]]),'Incomplete Title readiness accepted')
    for bit in range(8): require(not input_trace_passed([*rows[:3],{**rows[3],'bootProgress':255^(1<<bit)},*rows[4:]]),'Incomplete Main Menu readiness accepted')
    with tempfile.TemporaryDirectory(prefix='npc-yn-pixels-') as directory:
        folder=Path(directory); snapshots=[]; state=bytearray(276); struct.pack_into('<II',state,0,2,276)
        struct.pack_into('<11I',state,176,2,0,1,1,0,8,0,0,15,1,255)
        for index in range(10):
            label='source-pixels'; base=folder/f'snapshot-{index:03d}-{label}'
            Path(str(base)+'-state.bin').write_bytes(state); Path(str(base)+'-choice.bin').write_bytes(bytes(36))
            Path(str(base)+'-engine-0.ppm').write_bytes(b'P6\n256 192\n255\n'+b'\0\1\2'+bytes(256*192*3-3))
            snapshots.append(dict(sequence=index,label=label,frame=index,phase=0,ownerGeneration=0,revision=0,requiredMask=1,renderedMask=1,variedMask=1))
        require(snapshot_files_passed(folder,snapshots,[]),'Actual PPM parser rejected valid original-source witness')
        require(not snapshot_files_passed(folder,[{**snapshots[0],'variedMask':0},*snapshots[1:]],[]),'Missing measured pixel variance accepted')
        Path(str(base)+'-engine-0.ppm').write_bytes(b'P6\n256 192\n255\n'+bytes(256*192*3))
        require(not snapshot_files_passed(folder,snapshots,[]),'Uniform source pixels accepted')
    check_c_contracts()
    host = HOST.read_text()
    contract = re.search(r'^static u32 ynInputContractSelfCheck\(void\) \{.*?^}', host, re.M | re.S)
    require(contract is not None, 'Missing actual host input contract check')
    boot_contracts=[re.search(r'^static BOOL '+name+r'\([^\n]+\) \{.*?^}',host,re.M|re.S)[0]
        for name in ('ynBootSameSwitchedOwner','ynBootExitWitness')]
    with tempfile.TemporaryDirectory(prefix='npc-yn-input-') as directory:
        temp = Path(directory); fixture = temp / 'check.c'
        fixture.write_text('#include <stdint.h>\n#include "npc_yesno_qa.h"\ntypedef uint32_t u32;\ntypedef int BOOL;\n'+'\n'.join(boot_contracts)+'\n'+contract[0]
            + '\nint main(void) {return ynInputContractSelfCheck()!=0;}\n')
        subprocess.run(['xcrun','clang','-std=c11','-O2','-fsanitize=address,undefined','-fno-sanitize-recover=all',
            '-iquote',str(HERE),'-iquote',str(ROOT/'include'),str(fixture),'-o',str(temp/'check')],check=True,timeout=30)
        subprocess.run([str(temp/'check')],check=True,timeout=10)
    print('PASS: actual host shipping-resolver C contract under ASan/UBSan; NPC observer/wrapper/conservation and finite trace checks; no module/game/AV run.')

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--self-check', action='store_true'); parser.add_argument('--full-receipt', type=Path)
    parser.add_argument('--save', type=Path); parser.add_argument('--output-name', default='native-npc-yesno')
    args = parser.parse_args()
    if args.self_check: self_check(); return 0
    if not args.full_receipt or not args.save: parser.error('Execution requires an explicit future matching --full-receipt and --save')
    require(re.fullmatch(r'[A-Za-z0-9][A-Za-z0-9_-]*',args.output_name), 'Choose a new archive basename')
    destination = ROOT / 'ios/build-original-abi32' / args.output_name; require(not destination.exists(), 'Preserve prior output; select a new attempt name')
    require(not args.save.is_symlink(), 'The explicit source save must not be a symlink')
    save = args.save.resolve()
    fixture = [Path(str(save)+suffix) for suffix in ('','.console','.wifi','.lock')]
    require(all(path.is_file() and not path.is_symlink() for path in fixture)
        and set(fixture) == set(save.parent.glob(save.name+'*')), 'The explicit fixture requires exactly four regular save/console/wifi/lock files')
    require('HG_FIELD_CHOICE_UI_YES_NO_INPUT' in (ROOT / 'include/native_field_choice_ui.h').read_text(), 'Integrate and validate production YES/NO before this private scenario')
    destination.mkdir(); frames = destination / 'frames'; frames.mkdir()
    report = dict(passed=False,complete=False,status='preparing',firstFailure=None,commands=[],noAutomaticRetry=True,gameRuntimeExecuted=False,
        scope='original NPC No/No/Yes/daily paths and exact expected mutation ledger; no synthetic mons/save setters, App/device/audio or full-game claim',inputs=[],snapshots=[],evidence=[])
    runner = Runner(report); context = None; owned = {str(path):sha(path) for path in OWNED}; originals = {str(path):sha(path) for path in fixture}
    try:
        context = validate_full(args.full_receipt)
        require(context['fullSourceBankCurrent'], 'Fresh Full must include the exact integrated production source bank')
        report.update(fullReceipt=str(context['receipt']),fullReceiptSha256=sha(context['receipt']),shippingModuleSha256=context['link']['outputSha256'],
            headersSha256=context['audited']['headersSha256'],sourceMapSha256=context['sourceMapSha256'],workInputsBefore=owned,fixtureHashes=originals)
        module = prepare_guest(destination / 'guest', runner, context); native = destination / 'native'; native.mkdir()
        h = context['helpers']; generated,writes,restores = h['generate_engine'](native,module,lambda argv,log:runner(argv,native/log))
        flags,runtime = h['native_build_settings'](native); flags += ['-I',str(HERE),'-g','-fsanitize=address,undefined','-fno-sanitize-recover=all']
        def compile_one(item):
            index,source = item; target = native / f'host-{index}.o'; extra = ['-DWASM_RT_TRAP_HANDLER=HGEngineTrap'] if source.name=='wasm-rt-impl.c' else []
            runner(['xcrun','clang',*flags,*extra,'-c',source,'-o',target],native/f'compile-{index}.log'); return target
        with ThreadPoolExecutor(max_workers=4) as pool: objects = list(pool.map(compile_one,enumerate([*generated,*runtime,HOST])))
        binary = native / 'npc-yesno-check'; runner(['xcrun','clang','-fsanitize=address,undefined',*objects,'-o',binary],native/'native-link.log')
        boot = destination/'boot-header.bin'; boot.write_bytes(h['read_boot_header'](context['pack']))
        copies=[]
        for path in fixture:
            copy=destination/('scenario.sav'+path.name[len(save.name):]); shutil.copy2(path,copy);copies.append(copy)
        report['privateSaveHashesBefore']={str(path):sha(path) for path in copies}
        assert_unchanged(context); require_hashes(owned,'WORK NPC sources');require_hashes(originals,'Original fixture')
        report.update(status='running',gameRuntimeExecuted=True,privateModuleSha256=sha(module),nativeBinarySha256=sha(binary),checkedStackWrites=writes,exceptionDepthRestores=restores)
        row,output=runner([binary,context['pack']/'files',boot,destination/'scenario.sav',frames],destination/'runtime.log',timeout=300,check=False)
        for line in output.splitlines():
            for prefix,key in (('NPC YESNO input: ','inputs'),('NPC YESNO snapshot: ','snapshots'),('NPC YESNO evidence: ','evidence')):
                if line.startswith(prefix): report[key].append(json.loads(line.removeprefix(prefix)));break
        report['privateSaveHashesAfter']={str(path):sha(path) for path in copies}
        require(row['terminalReturnCode']==0 and not row['timedOut'] and row['allChildrenWaited'] and len(report['evidence'])==1
            and evidence_passed(report['evidence'][0]) and input_trace_passed(report['inputs'])
            and snapshot_files_passed(frames,report['snapshots'],report['inputs']), 'Original NPC YES/NO scenario failed; retain all evidence')
        require(report['privateSaveHashesBefore']==report['privateSaveHashesAfter'], 'Private save files unexpectedly changed')
        report.update(passed=True,complete=True,status='passed_original_npc_yesno')
    except BaseException as error:
        report.update(passed=False,status='failed',firstFailure=dict(type=type(error).__name__,message=str(error)))
    finally:
        try:
            if context: assert_unchanged(context)
            require_hashes(owned,'WORK NPC sources');require_hashes(originals,'Original fixture');report['protectedInputsUnchanged']=True
        except BaseException as error:
            report['passed']=False;report['preservationFailure']=dict(type=type(error).__name__,message=str(error))
        report['allChildrenWaited']=bool(report['commands']) and all(row['allChildrenWaited'] for row in report['commands'])
        report['capturedFiles']={str(path.relative_to(destination)):sha(path) for path in frames.rglob('*') if path.is_file()}
        write(destination/'report.json',report)
    print(('PASS' if report['passed'] else 'FAIL')+': '+str(destination/'report.json'));return 0 if report['passed'] else 1

if __name__=='__main__': raise SystemExit(main())
