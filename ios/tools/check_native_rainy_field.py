# SPDX-License-Identifier: GPL-3.0-or-later
"""Run one original Route33 rainy-field round trip using an isolated save copy."""
import argparse
from concurrent.futures import ThreadPoolExecutor
import hashlib
import json
from pathlib import Path
import shlex
import shutil
import struct
import subprocess

from check_abi32 import ROOT,BUILD,SDK,native_build_settings
from audit_original import digest
from check_native_engine import generate_engine
from import_original_content import read_boot_header,narc
from prepare_native_content import verify,source_table
from prepare_overlay_state import compile_unit


def sha(path): return hashlib.sha256(path.read_bytes()).hexdigest()

INCLUDES=['ios/build-original-host/include','include','files','src','lib/include','lib/include/cw',
          'lib/include/nitro','lib/include/nnsys','lib/include/nitro/os','include/library',
          'lib/NitroSDK/include','lib/NitroSDK/include-pokediamond']


def validate_anchor(audit,pack):
    profile=json.loads((audit/'native-content/report.json').read_text())
    table=source_table(audit/'native-content/native-map.o')
    if hashlib.sha256(table).hexdigest()!=profile['mapSha256']: raise RuntimeError('Native ROM map table proof changed')
    row=struct.unpack_from('<BB9HI',table,37*24)
    matrix,scripts,header,event,flags=row[3],row[4],row[5],row[9],row[11]
    if (matrix,scripts,header,event,(flags>>1)&127)!=(0,236,481,34,1): raise RuntimeError('Route33 ROM record changed')
    paths=[pack/'files'/name for name in ['a/0/3/2','a/0/4/1','a/0/1/2','a/0/6/5']]
    events,matrices,scripts,land=map(narc,paths)
    data=events[event]; offset=4+struct.unpack_from('<I',data)[0]*20
    offset+=4+struct.unpack_from('<I',data,offset)[0]*32
    count=struct.unpack_from('<I',data,offset)[0]; offset+=4
    if count!=1 or struct.unpack_from('<HHHHI',data,offset)!=(467,461,99,2,0): raise RuntimeError('Route33 warp0 changed')
    data=events[96]; offset=4+struct.unpack_from('<I',data)[0]*20
    offset+=4+struct.unpack_from('<I',data,offset)[0]*32
    count=struct.unpack_from('<I',data,offset)[0]; offset+=4
    if count<=2 or struct.unpack_from('<HHHHI',data,offset+24)!=(27,93,37,0,0): raise RuntimeError('Reciprocal cave anchor changed')
    data=matrices[0]; width,height,headers,altitude,length=struct.unpack_from('<5B',data)
    if (width,height,headers)!=(47,17,1): raise RuntimeError('Route33 matrix changed')
    offset=5+length; index=14*width+14
    map_id=struct.unpack_from('<H',data,offset+index*2)[0]
    offset+=width*height*2+(width*height if altitude else 0)
    model=struct.unpack_from('<H',data,offset+index*2)[0]
    if (map_id,model)!=(37,32) or scripts[header]!=bytes(4): raise RuntimeError('Route33 cell/init script changed')
    return dict(mapId=37,warpId=0,x=467,z=461,matrixIndex=index,model=model,weather=1,
                mapTableSha256=profile['mapSha256'],archives={str(p.relative_to(pack)):sha(p) for p in paths},
                landMemberSha256=hashlib.sha256(land[model]).hexdigest())


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--save',required=True,type=str,help='Existing private Route29 fixture; only its copy is opened by the game')
    args=parser.parse_args()
    audit=BUILD/'audit'; destination=BUILD/'rainy-field-scenario'; destination.mkdir(exist_ok=True)
    source_save=Path(args.save).resolve(); save=destination/'scenario.sav'
    if not source_save.is_file() or source_save==save.resolve(): raise ValueError('Supply a separate existing private Route29 save')
    save_hash=sha(source_save)
    report=destination/'report.json'; report.unlink(missing_ok=True)
    link=json.loads((audit/'engine-link.json').read_text())
    if link['exitCode'] or sha(audit/'engine.wasm')!=link['outputSha256']: raise RuntimeError('Verified full engine audit required')
    audit_report=json.loads((audit/'report.json').read_text())
    include_paths=[ROOT/p for p in INCLUDES]
    if link.get('nativeContent'): include_paths.insert(0,ROOT/link['nativeContent']['include'])
    headers={p for folder in include_paths for pattern in ('**/*.h','**/*.naix') for p in folder.glob(pattern)}
    headers|=set((ROOT/'lib/MSL_C/src').glob('*.h'))|set((ROOT/'lib/MSL_C/src').glob('*.hpp'))
    if digest(headers)!=audit_report['headersSha256']: raise RuntimeError('Headers differ from the full audit')
    units={row['file']:row for row in audit_report['units']}
    names=['src/field_system.c','src/field/map_load_manager.c','ios/Original/NativeGeometry.c',
           'ios/Original/Native3D.c','ios/Original/Native2D.cpp','ios/Original/abi32/NativeIRQClient.c']
    for name in names:
        if units[name]['exitCode'] or sha(ROOT/name)!=units[name]['sha256']: raise RuntimeError('Unaudited source: '+name)
    pack=verify(link.get('nativeContent')); anchor=validate_anchor(audit,pack)
    command=link['command'].copy(); original_rsp=next(value for value in command if value.startswith('@'))
    original_objects=shlex.split(Path(original_rsp[1:]).read_text())
    cached=[Path(p) for p in original_objects]
    cache_hashes={str(p):sha(p) for p in cached}
    host_flags,runtime=native_build_settings(destination)
    owned=['ios/Original/abi32/rainy_field_scenario_check.inc','ios/Original/tests/rainy_field_scenario_host_check.c',
           'ios/tools/check_native_rainy_field.py']
    tracked=[ROOT/n for n in names+owned]+runtime+[ROOT/'ios/Original/abi32/engine_check.c']
    source_hashes={str(p.relative_to(ROOT)) if p.is_relative_to(ROOT) else str(p):sha(p) for p in tracked}
    commands=[]
    def run(argv,log):
        argv=list(map(str,argv)); commands.append(argv)
        result=subprocess.run(argv,cwd=ROOT,capture_output=True,text=True)
        (destination/log).write_text(result.stdout+result.stderr)
        if result.returncode: raise RuntimeError('Rainy field build failed: '+str(destination/log))
    flags=['--target=wasm32-wasip1','-std=gnu11','-O2','-fwrapv','-fno-strict-aliasing',
           '-fno-builtin-malloc','-fno-builtin-free','-fwasm-exceptions','-fexceptions','-Xclang','-disable-red-zone',
           '-fms-extensions','-Wno-microsoft','-Wno-unknown-pragmas']
    flags+=['-D'+n for n in ['HEARTGOLD_NATIVE','HEARTGOLD_ABI32','SDK_ARM9','SDK_CW','SDK_CODE_ARM','SDK_TS','_NITRO',
                             'PM_KEEP_ASSERTS','PLATFORM_INTRINSIC_FUNCTION_BIT_CLZ32=__builtin_clz']]
    for path in INCLUDES:
        flags+=['-I',ROOT/path]
    native_content=link.get('nativeContent')
    if native_content: flags=['-I',ROOT/native_content['include'],'-DHG_NATIVE_ROM_TABLES',*flags]
    field_code=(ROOT/names[0]).read_text(); field_anchor='BOOL finished = FieldSystem_Main(fieldSystem);'
    if field_code.count(field_anchor)!=1: raise RuntimeError('Field owner anchor changed')
    field=destination/'field-scenario.c'
    field.write_text('#include "'+str(ROOT/owned[0])+'"\n'+field_code.replace(field_anchor,field_anchor+'\n    RainyFieldQAFieldTick(fieldSystem);'))
    field_object=destination/'field-scenario.o'
    run([SDK/'bin/clang',*flags,'-c',field,'-o',field_object],'field-compile.log')
    model_code=(ROOT/names[1]).read_text()
    model_anchor='        GF3dRender_DrawModel(&mapLoadManager->maps[mapSlot]->renderObj, &pos, &rotation, &scale);'
    if model_code.count(model_anchor)!=1: raise RuntimeError('Loaded model draw anchor changed')
    model=destination/'map-scenario.c'
    model.write_text('void RainyFieldQAObserveMapModel(void *manager,int matrixIndex,const void *modelFile);\n'+model_code.replace(model_anchor,
        '        RainyFieldQAObserveMapModel(mapLoadManager,mapLoadManager->maps[mapSlot]->matrixIndex,mapLoadManager->maps[mapSlot]->modelFile);\n'+model_anchor))
    model_object=destination/'map-scenario.o'
    code,diagnostics,state=compile_unit(SDK/'bin/clang',flags,model,model_object,names[1],ROOT)
    (destination/'map-compile.log').write_text(diagnostics)
    (destination/'map-overlay-state.json').write_text(json.dumps(state,indent=2)+'\n')
    if code or 'error' in state or state['getter']!=units[names[1]]['overlayStates'][0]['getter']:
        raise RuntimeError('Map QA copy changed audited overlay ownership')
    objects=original_objects.copy()
    for name,new in [(names[0],field_object),(names[1],model_object)]:
        old=str(audit/'objects'/Path(name).with_suffix('.o'))
        if objects.count(old)!=1: raise RuntimeError('Expected one audited object: '+old)
        objects[objects.index(old)]=str(new)
    listing=destination/'objects.rsp'; listing.write_text('\n'.join('"'+p+'"' for p in objects)+'\n')
    command[command.index(original_rsp)]='@'+str(listing)
    module=destination/'engine.wasm'; command[command.index('-o')+1]=str(module)
    for export in ['HGNativeRainyFieldQAGetState','HGNativeRainyFieldQARequest','HGNativeRainyFieldQACompare']:
        command.insert(command.index('-o'),'-Wl,--export='+export)
    for wrapped in ['HGNativeGeometryFog','MapLoadManager_End','MapLoadManager_HasEnded','MapLoadManager_FreeNARCAndLoadedMapBuffers']:
        command.insert(command.index('-o'),'-Wl,--wrap='+wrapped)
    run(command,'strict-link.log')
    generated,writes,restores=generate_engine(destination,module,run)
    host_flags+=['-g','-fsanitize=address,undefined','-fno-sanitize-recover=all']
    def compile_one(item):
        index,path=item; output=destination/f'host-{index}.o'
        extra=['-DWASM_RT_TRAP_HANDLER=HGEngineTrap'] if path.name=='wasm-rt-impl.c' else []
        run(['xcrun','clang',*host_flags,*extra,'-c',path,'-o',output],f'compile-{index}.log'); return output
    with ThreadPoolExecutor(max_workers=4) as pool:
        native_objects=list(pool.map(compile_one,enumerate([*generated,*runtime,ROOT/owned[1]])))
    binary=destination/'rainy-field-check'
    run(['xcrun','clang','-fsanitize=address,undefined',*native_objects,'-o',binary],'native-link.log')
    boot=destination/'boot-header.bin'; boot.write_bytes(read_boot_header(pack)); shutil.copyfile(source_save,save)
    frames=destination/'frames'; frames.mkdir(exist_ok=True)
    for path in frames.glob('*.ppm'): path.unlink()
    argv=list(map(str,[binary,pack/'files',boot,save,frames])); commands.append(argv)
    try:
        result=subprocess.run(argv,cwd=ROOT,capture_output=True,text=True,timeout=180)
        exit_code=result.returncode; output=result.stdout+result.stderr
    except subprocess.TimeoutExpired as error:
        exit_code=None; output=(error.stdout or b'').decode()+(error.stderr or b'').decode()
    (destination/'runtime.log').write_text(output)
    evidence=[json.loads(line.split('Rainy field evidence: ',1)[1]) for line in output.splitlines() if line.startswith('Rainy field evidence: ')]
    unchanged=sha(source_save)==save_hash and sha(audit/'engine.wasm')==link['outputSha256']
    cache_unchanged=all(sha(p)==cache_hashes[str(p)] for p in cached)
    sources_unchanged=all(sha(p)==source_hashes[str(p.relative_to(ROOT)) if p.is_relative_to(ROOT) else str(p)] for p in tracked)
    passed=exit_code==0 and len(evidence)==1 and 'PASS: real rainy Route33' in output and unchanged and cache_unchanged and sources_unchanged and digest(headers)==audit_report['headersSha256']
    report.write_text(json.dumps(dict(passed=passed,scope='native-host original Route33 rain/fog, owner async warp, same-epoch density-zero comparison, return and cleanup; no App/full-game claim',
        exitCode=exit_code,productionModuleSha256=link['outputSha256'],scenarioModuleSha256=sha(module),sourceSaveSha256=save_hash,
        originalSaveAndModuleUnchanged=unchanged,sharedObjectsUnchanged=cache_unchanged,sourcesUnchanged=sources_unchanged,
        validatedAnchor=anchor,evidence=evidence,sourceHashes=source_hashes,commands=commands,
        frames=[dict(file=p.name,sha256=sha(p)) for p in sorted(frames.glob('*.ppm'))],
        checkedStackWrites=writes,exceptionDepthRestores=restores),indent=2)+'\n')
    print(('PASS' if passed else 'FAIL')+': '+str(report)); return 0 if passed else 1


if __name__=='__main__': raise SystemExit(main())
