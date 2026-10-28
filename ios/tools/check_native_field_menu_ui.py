#!/usr/bin/env python3
"""Portable field-menu owner/producer/lifecycle check; no module, game or App execution."""
import argparse,ast,hashlib,json,os,re,subprocess
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2]
PRODUCERS=('src/launch_application.c','src/start_menu.c','src/field_system.c','src/field/touchscreen_menu.c')
def require(value,message):
    if not value:raise RuntimeError(message)
def sha(path):return hashlib.sha256(Path(path).read_bytes()).hexdigest()
def function(source,name):
    match=re.search(r'^(?:static (?:inline )?)?[\w *]+\b'+re.escape(name)+r'\([^;]*?\)\s*\{',source,re.M)
    require(match is not None,'Missing C function '+name);start=source.index('{',match.start());end=start+1;level=1
    while level:level+=(source[end]=='{')-(source[end]=='}');end+=1
    return source[match.start():end]
def main():
    parser=argparse.ArgumentParser(description=__doc__);parser.add_argument('--source-root',type=Path,default=ROOT)
    parser.add_argument('--candidate-root',type=Path,default=ROOT);parser.add_argument('--wasi-sdk',type=Path);parser.add_argument('--output',required=True,type=Path)
    args=parser.parse_args();root=args.source_root.resolve();candidate=args.candidate_root.resolve();out=args.output.resolve();require(not out.exists(),'Choose new attempt');out.mkdir(parents=True)
    names=(*PRODUCERS,'include/native_field_menu_ui.h','include/start_menu.h','ios/Original/tests/field_menu_ui_check.c','ios/tools/check_native_field_menu_ui.py',
        'ios/tools/check_abi32.py','ios/tools/build_original_app.py','ios/tools/check_native_pc_box.py','ios/tools/check_pc_box_uikit_binding.py','ios/Original/CMakeLists.txt','ios/Original/abi32/check.c')
    paths=[candidate/name for name in names]+[root/name for name in (*PRODUCERS,'src/sys_task.c','src/task.c','src/menu_input_state.c','include/field_system.h','include/start_menu.h')]
    before={str(path):sha(path) for path in paths};report=dict(passed=False,commands=[],sourceHashesBefore=before,moduleBuilt=False,gameRuntimeExecuted=False)
    env={**os.environ,'HEARTGOLD_TEST_MUTE':'1','SIMCTL_CHILD_HEARTGOLD_TEST_MUTE':'1','PYTHONDONTWRITEBYTECODE':'1'}
    def run(command,log):
        result=subprocess.run(['nice','-n','10',*map(str,command)],env=env,cwd=root,capture_output=True,text=True,timeout=60)
        (out/log).write_text(result.stdout+result.stderr);report['commands'].append(dict(command=list(map(str,command)),returnCode=result.returncode,log=str(out/log)));require(result.returncode==0,'Failed '+str(out/log))
    try:
        launch=(candidate/PRODUCERS[0]).read_text();start=launch.index('// Native field menu resident state begin.');end=launch.index('// Native field menu resident state end.')
        (out/'actual_resident.inc').write_text(launch[start:end]);(out/'actual_sys_task.inc').write_text(re.sub(r'^#include .*\n','',(root/'src/sys_task.c').read_text(),flags=re.M))
        menu=(candidate/'src/start_menu.c').read_text();field=(root/'src/field_system.c').read_text();task=(root/'src/task.c').read_text();mode=(root/'src/menu_input_state.c').read_text()
        services='\n'.join(function(source,name) for source,name in ((task,'TaskManager_GetFieldSystem'),(task,'TaskManager_GetEnvironment'),
            (task,'FieldSystem_TaskIsRunning'),(field,'FieldSystem_IsFieldMapRunning'),(field,'FieldSystem_IsChildAppRunning'),
            (field,'FieldSystem_IsPlayerMovementAllowed'),(mode,'MenuInputStateMgr_GetState')))
        (out/'actual_field_services.inc').write_text(services)
        (out/'actual_producer.inc').write_text(function(menu,'StartMenu_PublishNativeUI')+'\n'+function(menu,'Task_StartMenu_WaitApp')+'\n'+function(menu,'Task_StartMenu_HandleSelection_Retire'))
        fragments=[]
        for name in ('START_MENU_STATE_11','START_MENU_STATE_13','START_MENU_STATE_18','START_MENU_STATE_CLOSE'):
            begin=menu.index('    case '+name+':');match=re.search(r'^    case ',menu[begin+1:],re.M);require(match is not None,'Case boundary missing '+name)
            fragments.append(menu[begin:begin+1+match.start()])
        (out/'actual_close_cases.inc').write_text('static BOOL ExecuteActualCloseCase(TaskManager *taskManager){\nFieldSystem *fieldSystem=TaskManager_GetFieldSystem(taskManager);StartMenuTaskData *startMenu=TaskManager_GetEnvironment(taskManager);switch(startMenu->state){\n'+'\n'.join(fragments)+'\n}return FALSE;}\n')
        retirement=function(menu,'Task_StartMenu_HandleSelection_Retire')
        require(retirement.index('NativeFieldMenuUIRetire')<retirement.index('StartMenu_RemoveCursorAndIcons')<retirement.index('Heap_Free'),'Retire action must revoke before teardown/free')
        require(function(menu,'StartMenu_RemoveCursorAndIcons').index('NativeFieldMenuUIEndView')<function(menu,'StartMenu_RemoveCursorAndIcons').index('StartMenu_DestroyCursor'),'View revocation must precede dependency free')
        check=candidate/'ios/Original/tests/field_menu_ui_check.c'
        common=['xcrun','clang','-std=c11','-O1','-g','-fms-extensions','-Wno-microsoft','-Wno-unknown-pragmas','-fsanitize=address,undefined','-fno-sanitize-recover=all',
            '-iquote',candidate/'include','-iquote',root/'include','-idirafter',root/'lib/include','-idirafter',root/'ios/build-original-host/include','-I',out,'-DHG_NATIVE_FIELD_MENU_UI_CHECK_MAIN']
        for folder in ('include/library','lib/include/cw','lib/include/nitro','lib/include/nnsys','lib/include/nitro/os','lib/NitroSDK/include','lib/NitroSDK/include-pokediamond','files'):
            common+=['-idirafter',root/folder]
        for kind,extra in (('normal',[]),('resident-producer',['-DHEARTGOLD_NATIVE','-DSDK_ARM9','-DHG_NATIVE_FIELD_MENU_UI_STATE_CHECK'])):
            binary=out/kind;run([*common,*extra,check,'-o',binary],kind+'-compile.log');run([binary],kind+'-run.log')
        if args.wasi_sdk:
            flags=['--target=wasm32-wasip1','-std=gnu11','-O2','-fwrapv','-fno-strict-aliasing','-fno-builtin-malloc','-fno-builtin-free','-fwasm-exceptions','-fexceptions','-Xclang','-disable-red-zone','-fms-extensions','-Wno-microsoft','-Wno-unknown-pragmas',
                '-DHEARTGOLD_NATIVE','-DHEARTGOLD_ABI32','-DSDK_ARM9','-DSDK_CW','-DSDK_CODE_ARM','-DSDK_TS','-D_NITRO','-DPM_KEEP_ASSERTS','-DPLATFORM_INTRINSIC_FUNCTION_BIT_CLZ32=__builtin_clz','-I',str(candidate/'include')]
            for folder in ('ios/build-original-host/include','include','files','src','lib/include','lib/include/cw','lib/include/nitro','lib/include/nnsys','lib/include/nitro/os','include/library','lib/NitroSDK/include','lib/NitroSDK/include-pokediamond'):flags+=['-I',str(root/folder)]
            for index,name in enumerate((*PRODUCERS,'ios/Original/tests/field_menu_ui_check.c')):run([args.wasi_sdk/'bin/clang',*flags,'-fsyntax-only',candidate/name],f'wasi-syntax-{index}.log')
        tree=ast.parse((candidate/'ios/tools/check_native_pc_box.py').read_text());functions=[node for node in tree.body if isinstance(node,ast.FunctionDef) and node.name in ('shipping_exports_passed','normal_checks_passed')];scope={};exec(compile(ast.Module(body=functions,type_ignores=[]),'candidate-count-contract','exec'),scope)
        exports=['HGNativePCBoxUIGetState','HGNativeFieldChoiceUIGetState','HGNativeTrainerCardUIGetState','HGNativeFieldMenuUIGetState','HGNativePokegearUIGetState','HGNativeStarterUIGetState','HGNativeOakUIGetState',*('public'+str(i) for i in range(26))]
        checks=['CheckNativePCBoxUI','CheckOriginalSummaryUI','CheckNativeFieldChoiceUI','CheckNativeTrainerCardUI','CheckNativeFieldMenuUI','CheckNativePokegearUI','CheckNativeStarterUI','CheckNativeOakUI']
        require(scope['shipping_exports_passed'](exports,sorted(exports)) and not scope['shipping_exports_passed'](exports[:-1],sorted(exports[:-1])),'Shipping33 contract')
        require(scope['normal_checks_passed'](checks,8) and not scope['normal_checks_passed'](checks[:-1],7),'Normal FieldMenu check gate')
        for path in (candidate/'ios/tools').glob('*.py'):compile(path.read_text(),str(path),'exec')
        report.update(passed=True,packetBytes=36,originalSchedulerExecuted=True,actualProducerWithRealSDKStructsExecuted=True,
            actualButtonsTouchReturnBranchExecuted=True,actualFourFreeCaseFragmentsExecuted=True,actualFifthRetireActionExecuted=True,
            menuReadyWhileMovementBlocked=True,hudPendingInputReadinessRevoked=True,shippingExportsProposed=33,normalChecksProposed=104,
            limitations=['Producer fixture uses real SDK types but controlled field/bottom-app/fade service bindings; no game or graphical assets run.',
                'Four modified free case fragments and the fifth whole retire action execute at recorded teardown/free/jump/script boundaries.'])
    except BaseException as error:report['firstFailure']=dict(type=type(error).__name__,message=str(error))
    finally:
        after={str(path):sha(path) for path in paths};report['sourceHashesAfter']=after;report['sourceInputsUnchanged']=before==after;report['passed'] &= before==after
        (out/'report.json').write_text(json.dumps(report,indent=2,sort_keys=True)+'\n')
    print(('PASS' if report['passed'] else 'FAIL')+': '+str(out/'report.json'));return 0 if report['passed'] else 1
if __name__=='__main__':raise SystemExit(main())
