#!/usr/bin/env python3
"""Focused Pokegear packet/resident check and optional real producer syntax; never builds or runs a game module."""
import argparse,ast,hashlib,json,os,re,subprocess
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2]
PRODUCERS=('src/launch_application.c','src/application/pokegear/main/pokegear_main.c','src/application/pokegear/main/pokegear_app_ui.c',
    'src/application/pokegear/configure/pokegear_configure.c','src/application/pokegear/map/pokegear_map.c',
    'src/application/pokegear/map/fly_map.c','src/application/pokegear/phone/pokegear_phone.c','src/application/pokegear/radio/pokegear_radio.c')
def require(value,message):
    if not value:raise RuntimeError(message)
def sha(path):return hashlib.sha256(Path(path).read_bytes()).hexdigest()
def main():
    parser=argparse.ArgumentParser(description=__doc__);parser.add_argument('--source-root',type=Path,default=ROOT)
    parser.add_argument('--candidate-root',type=Path,default=ROOT);parser.add_argument('--wasi-sdk',type=Path)
    parser.add_argument('--output',type=Path,required=True);args=parser.parse_args();root=args.source_root.resolve();candidate=args.candidate_root.resolve();out=args.output.resolve()
    require(not out.exists(),'Choose a new output attempt');out.mkdir(parents=True)
    owned=[candidate/name for name in (*PRODUCERS,'include/native_pokegear_ui.h','include/application/pokegear/pokegear_internal.h',
        'ios/Original/tests/pokegear_ui_check.c','ios/tools/check_abi32.py','ios/tools/check_native_pc_box.py',
        'ios/tools/check_pc_box_uikit_binding.py','ios/tools/check_native_trainer_card.py','ios/tools/check_native_field_menu_ui.py','ios/tools/build_original_app.py','ios/Original/CMakeLists.txt','ios/Original/abi32/check.c')]
    owned.append(candidate/'ios/tools/check_native_pokegear_ui.py')
    originals=[root/name for name in (*PRODUCERS,'src/sys_task.c','include/sys_task.h','src/touchscreen_list_menu.c',
        'src/save_pokegear.c','include/save_pokegear.h','src/start_menu.c')]
    before={str(path):sha(path) for path in {*owned,*originals}}
    report=dict(passed=False,commands=[],sourceHashesBefore=before,gameRuntimeExecuted=False,moduleBuilt=False,fixtureCardsObserved=False)
    env={**os.environ,'HEARTGOLD_TEST_MUTE':'1','SIMCTL_CHILD_HEARTGOLD_TEST_MUTE':'1','PYTHONDONTWRITEBYTECODE':'1'}
    def run(command,log):
        result=subprocess.run(['nice','-n','10',*map(str,command)],cwd=root,env=env,capture_output=True,text=True,timeout=60)
        (out/log).write_text(result.stdout+result.stderr);report['commands'].append(dict(command=list(map(str,command)),returnCode=result.returncode,log=str(out/log)))
        require(result.returncode==0,'Failed '+str(out/log))
    try:
        launch=(candidate/PRODUCERS[0]).read_text();start=launch.index('// Native Pokegear resident state begin.');end=launch.index('// Native Pokegear resident state end.')
        (out/'actual_resident.inc').write_text(launch[start:end])
        (out/'actual_sys_task.h').write_text((root/'include/sys_task.h').read_text().replace('#include "global.h"',''))
        (out/'actual_sys_task.inc').write_text(re.sub(r'^#include .*\n','',(root/'src/sys_task.c').read_text(),flags=re.M))
        flags=['xcrun','clang','-std=c11','-O1','-g','-fsanitize=address,undefined','-fno-sanitize-recover=all',
            '-iquote',candidate/'include','-iquote',out,'-DHG_NATIVE_POKEGEAR_UI_CHECK_MAIN']
        check=candidate/'ios/Original/tests/pokegear_ui_check.c'
        for mode,extra in (('normal',[]),('resident',['-DHEARTGOLD_NATIVE','-DHG_NATIVE_POKEGEAR_UI_STATE_CHECK'])):
            binary=out/(mode+'-check');run([*flags,*extra,check,'-o',binary],mode+'-compile.log');run([binary],mode+'-run.log')
        # Execute the changed pure validator functions, without importing App/game preparation.
        tree=ast.parse((candidate/'ios/tools/check_native_pc_box.py').read_text());names={'shipping_exports_passed','normal_checks_passed'}
        functions=[node for node in tree.body if isinstance(node,ast.FunctionDef) and node.name in names];require(len(functions)==2,'Validator functions missing')
        scope={};exec(compile(ast.Module(body=functions,type_ignores=[]),'candidate-export-contract','exec'),scope)
        exports=['HGNativePCBoxUIGetState','HGNativeFieldChoiceUIGetState','HGNativeTrainerCardUIGetState','HGNativeFieldMenuUIGetState','HGNativePokegearUIGetState','HGNativeStarterUIGetState',*('public'+str(i) for i in range(26))]
        require(scope['shipping_exports_passed'](exports,sorted(exports)),'Shipping32 fixture rejected')
        require(not scope['shipping_exports_passed']([name.replace('HGNativePokegearUIGetState','missing') for name in exports],sorted(exports)),'Missing Pokegear getter accepted')
        require(not scope['shipping_exports_passed']([name.replace('HGNativeStarterUIGetState','missing') for name in exports],sorted(name.replace('HGNativeStarterUIGetState','missing') for name in exports)),'Missing Starter getter accepted')
        checks=['CheckNativePCBoxUI','CheckOriginalSummaryUI','CheckNativeFieldChoiceUI','CheckNativeTrainerCardUI','CheckNativeFieldMenuUI','CheckNativePokegearUI','CheckNativeStarterUI']
        require(scope['normal_checks_passed'](checks,7) and not scope['normal_checks_passed'](checks[:-1],6),'Normal Pokegear check gate')
        link=(candidate/'ios/tools/check_abi32.py').read_text();section=link[link.index('def link_engine('):link.index('    result=subprocess.run',link.index('def link_engine('))]
        symbols=re.findall(r"'-Wl,--export=([^']+)'",section);require(len(symbols)==30 and len(set(symbols))==30 and symbols.count('HGNativePokegearUIGetState')==1 and symbols.count('HGNativeStarterUIGetState')==1,'30 declared functions plus reactor memory/_initialize must be shipping32')
        for path in (candidate/'ios/tools').glob('*.py'):compile(path.read_text(),str(path),'exec')
        if args.wasi_sdk:
            wasi=['--target=wasm32-wasip1','-std=gnu11','-O2','-fwrapv','-fno-strict-aliasing','-fno-builtin-malloc','-fno-builtin-free','-fwasm-exceptions','-fexceptions','-Xclang','-disable-red-zone',
                '-fms-extensions','-Wno-microsoft','-Wno-unknown-pragmas','-DHEARTGOLD_NATIVE','-DHEARTGOLD_ABI32','-DSDK_ARM9','-DSDK_CW','-DSDK_CODE_ARM','-DSDK_TS','-D_NITRO','-DPM_KEEP_ASSERTS',
                '-DPLATFORM_INTRINSIC_FUNCTION_BIT_CLZ32=__builtin_clz','-I',str(candidate/'include')]
            for folder in ('ios/build-original-host/include','include','files','src','lib/include','lib/include/cw','lib/include/nitro','lib/include/nnsys','lib/include/nitro/os','include/library','lib/NitroSDK/include','lib/NitroSDK/include-pokediamond'):wasi+=['-I',str(root/folder)]
            for index,name in enumerate((*PRODUCERS,'ios/Original/tests/pokegear_ui_check.c')):
                run([args.wasi_sdk/'bin/clang',*wasi,'-fsyntax-only',candidate/name],f'wasi-syntax-{index}.log')
        for path in owned:require(not re.search(r'[ \t]+$',path.read_text(),re.M),'Trailing whitespace '+str(path))
        report.update(passed=True,packetBytes=48,proposedShippingExports=32,proposedNormalChecks=103,proposedPrivateObservers=34,
            actualOriginalSchedulerExecuted=True,scope='packet/header/resident scheduler execution; optional real source syntax; no module/game/App/AV',
            fixtureCards='not read by this packet check; source caches the actual registered-card mask')
    except BaseException as error:report['firstFailure']=dict(type=type(error).__name__,message=str(error))
    finally:
        after={str(path):sha(path) for path in {*owned,*originals}};report['sourceHashesAfter']=after;report['sourceInputsUnchanged']=before==after;report['passed'] &= before==after
        (out/'report.json').write_text(json.dumps(report,indent=2,sort_keys=True)+'\n')
    print(('PASS' if report['passed'] else 'FAIL')+': '+str(out/'report.json'));return 0 if report['passed'] else 1
if __name__=='__main__':raise SystemExit(main())
