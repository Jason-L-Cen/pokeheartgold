# SPDX-License-Identifier: GPL-3.0-or-later
"""Run original doubles/target generation in an isolated native process using a save copy."""
import argparse
from concurrent.futures import ThreadPoolExecutor
import hashlib
import json
from pathlib import Path
import shlex
import shutil
import subprocess

from check_abi32 import ROOT, BUILD, SDK, native_build_settings
from check_native_engine import generate_engine
from import_original_content import read_boot_header
from prepare_native_content import verify


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--save',required=True,type=str,help='Private Route29 save; copied, never opened by the game')
    args=parser.parse_args()
    audit=BUILD/'audit'; destination=BUILD/'battle-scenario'; destination.mkdir(exist_ok=True)
    save_source=Path(args.save).resolve()
    save=destination/'scenario.sav'
    if save_source==save.resolve() or not save_source.is_file(): raise ValueError('Supply a separate existing save')
    original_save_hash=hashlib.sha256(save_source.read_bytes()).hexdigest()
    report=destination/'report.json'; report.unlink(missing_ok=True)
    link=json.loads((audit/'engine-link.json').read_text())
    if link['exitCode'] or hashlib.sha256((audit/'engine.wasm').read_bytes()).hexdigest()!=link['outputSha256']:
        raise RuntimeError('A verified strict engine audit is required')
    unit=next(row for row in json.loads((audit/'report.json').read_text())['units'] if row['file']=='src/field_system.c')
    source=ROOT/unit['file']
    if unit['exitCode'] or hashlib.sha256(source.read_bytes()).hexdigest()!=unit['sha256']:
        raise RuntimeError('Field source differs from its audited object')
    pack=verify(link.get('nativeContent'))

    def run(command,log):
        result=subprocess.run(list(map(str,command)),cwd=ROOT,capture_output=True,text=True)
        (destination/log).write_text(result.stdout+result.stderr)
        if result.returncode: raise RuntimeError(f'Battle scenario build failed: {destination/log}')

    # Keep QA initialization in the field task flow, not in an IRQ/display observer.
    anchor='BOOL finished = FieldSystem_Main(fieldSystem);'
    code=source.read_text()
    if code.count(anchor)!=1: raise RuntimeError('Original field entry changed')
    field=destination/'field-scenario.c'
    field.write_text('#include "'+str(ROOT/'ios/Original/abi32/battle_scenario_field.inc')+'"\n'+code.replace(anchor,anchor+'\n    BattleQAFieldTick(fieldSystem);'))
    includes=['ios/build-original-host/include','include','files','src','lib/include','lib/include/cw',
              'lib/include/nitro','lib/include/nnsys','lib/include/nitro/os','include/library',
              'lib/NitroSDK/include','lib/NitroSDK/include-pokediamond']
    flags=['--target=wasm32-wasip1','-std=gnu11','-O2','-fwrapv','-fno-strict-aliasing',
           '-fno-builtin-malloc','-fno-builtin-free','-fwasm-exceptions','-fexceptions',
           '-Xclang','-disable-red-zone','-fms-extensions','-Wno-microsoft','-Wno-unknown-pragmas',
           '-DHEARTGOLD_NATIVE','-DHEARTGOLD_ABI32','-DSDK_ARM9','-DSDK_CW','-DSDK_CODE_ARM','-DSDK_TS','-D_NITRO','-DPM_KEEP_ASSERTS',
           '-DPLATFORM_INTRINSIC_FUNCTION_BIT_CLZ32=__builtin_clz']
    for include in includes: flags+=['-I',ROOT/include]
    native_content=link.get('nativeContent')
    if native_content: flags+=['-I',ROOT/native_content['include'],'-DHG_NATIVE_ROM_TABLES']
    field_object=destination/'field-scenario.o'
    run([SDK/'bin/clang',*flags,'-c',field,'-o',field_object],'field-compile.log')
    command=link['command'].copy(); old_rsp=next(value for value in command if value.startswith('@'))
    objects=shlex.split(Path(old_rsp[1:]).read_text())
    original_object=str(audit/'objects/src/field_system.o')
    if objects.count(original_object)!=1: raise RuntimeError('Expected one field object')
    objects[objects.index(original_object)]=str(field_object)
    listing=destination/'objects.rsp'; listing.write_text('\n'.join('"'+value+'"' for value in objects)+'\n')
    command[command.index(old_rsp)]='@'+str(listing)
    module=destination/'engine.wasm'; command[command.index('-o')+1]=str(module)
    command.insert(command.index('-o'),'-Wl,--export=HGNativeBattleQARequest')
    run(command,'strict-link.log')
    generated,writes,restores=generate_engine(destination,module,run)
    host_flags,sources=native_build_settings(destination)
    host_flags+=['-g','-fsanitize=address,undefined','-fno-sanitize-recover=all']
    def compile_one(item):
        index,path=item; output=destination/f'host-{index}.o'
        trap_flags=['-DWASM_RT_TRAP_HANDLER=HGEngineTrap'] if path.name=='wasm-rt-impl.c' else []
        run(['xcrun','clang',*host_flags,*trap_flags,'-c',path,'-o',output],f'compile-{index}.log')
        return output
    with ThreadPoolExecutor(max_workers=4) as pool:
        objects=list(pool.map(compile_one,enumerate([*generated,*sources,ROOT/'ios/Original/abi32/battle_scenario_check.c'])))
    binary=destination/'battle-check'
    run(['xcrun','clang','-fsanitize=address,undefined',*objects,'-o',binary],'native-link.log')
    boot=destination/'boot-header.bin'; boot.write_bytes(read_boot_header(pack))
    shutil.copyfile(save_source,save)
    frames=destination/'frames'; frames.mkdir(exist_ok=True)
    try:
        result=subprocess.run([str(binary),str(pack/'files'),str(boot),str(save),str(frames),'--continue'],cwd=ROOT,capture_output=True,text=True,timeout=120)
        status=result.returncode; output=result.stdout+result.stderr
    except subprocess.TimeoutExpired as error:
        status=None; output=(error.stdout or b'').decode()+(error.stderr or b'').decode()
    (destination/'runtime.log').write_text(output)
    unchanged=hashlib.sha256(save_source.read_bytes()).hexdigest()==original_save_hash
    passed=status==0 and 'PASS: real original doubles' in output and unchanged
    report.write_text(json.dumps(dict(scope='native host original doubles target flow; UIKit, turn execution and settlement unverified',
        passed=passed,exitCode=status,sourceSaveUnchanged=unchanged,sourceSaveSha256=original_save_hash,
        productionModuleSha256=link['outputSha256'],scenarioModuleSha256=hashlib.sha256(module.read_bytes()).hexdigest(),
        checkedStackWrites=writes,exceptionDepthRestores=restores),indent=2)+'\n')
    print(('PASS' if passed else 'FAIL')+': '+str(report))
    return 0 if passed else 1


if __name__=='__main__': raise SystemExit(main())
