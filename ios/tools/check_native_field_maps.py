# SPDX-License-Identifier: GPL-3.0-or-later
"""Run continuous512 original field-map round trips from private Route29 fixture copies."""
import argparse
from collections import Counter
from concurrent.futures import ThreadPoolExecutor
import hashlib
import json
from pathlib import Path
import re
import shlex
import shutil
import struct
import subprocess

from check_abi32 import ROOT,BUILD,SDK,WABT,native_build_settings
from audit_original import digest
from check_native_engine import generate_engine
from check_native_rainy_field import INCLUDES,sha
from import_original_content import read_boot_header,narc
from prepare_native_content import verify,source_table
from prepare_overlay_state import compile_unit


def catalogue(audit,pack):
    profile=json.loads((audit/'native-content/report.json').read_text())
    table=source_table(audit/'native-content/native-map.o')
    if len(table)!=540*24 or hashlib.sha256(table).hexdigest()!=profile['mapSha256']:
        raise RuntimeError('Verified 540-record ROM map table required')
    names={int(n):name for name,n in re.findall(r'^#define\s+(MAP_\w+)\s+(\d+)\b',(ROOT/'include/constants/maps.h').read_text(),re.M)}
    paths=[pack/'files'/p for p in ('a/0/3/2','a/0/4/1','a/0/1/2','a/0/6/5')]
    events,matrices,scripts,land=map(narc,paths)
    literals={}; script_sources=[]
    # Literal Warp operands are cross-checked against actual ROM script bytes,
    # using the original opcode176 layout (asm/macros/script.inc; ScrCmd_Warp).
    directions={'DIR_NORTH':0,'DIR_SOUTH':1,'DIR_WEST':2,'DIR_EAST':3}
    ids={name:map_id for map_id,name in names.items()}
    pattern=r'^\s*Warp\s+(MAP_\w+),\s*(0x[\da-fA-F]+|\d+),\s*(0x[\da-fA-F]+|\d+),\s*(0x[\da-fA-F]+|\d+),\s*(DIR_\w+)\s*$'
    for path in sorted((ROOT/'files/fielddata/script/scr_seq').glob('*.s')):
        for line_number,line in enumerate(path.read_text().splitlines(),1):
            match=re.match(pattern,line)
            if not match or match[1] not in ids or match[5] not in directions: continue
            map_id=ids[match[1]]; unused,x,z=(int(match[i],0) for i in (2,3,4)); direction=directions[match[5]]
            if max(unused,x,z)>65535: continue
            encoded=struct.pack('<6H',176,map_id,unused,x,z,direction)
            found=next(((i,blob.find(encoded)) for i,blob in enumerate(scripts) if blob.find(encoded)>=0 and blob.find(encoded)%2==0),None)
            if not found: continue
            literals.setdefault(map_id,[]).append(dict(warpId=0xFFFFFFFF,x=x,z=z,direction=direction,
                source=str(path.relative_to(ROOT)),sourceLine=line_number,romScript=found[0],romOffset=found[1],instructionHex=encoded.hex()))
            script_sources.append(path)
    rows=[]
    for map_id in range(540):
        record=struct.unpack_from('<BB9HI',table,map_id*24)
        matrix_id,event_id,script_id,flags=record[3],record[9],record[5],record[11]
        data=matrices[matrix_id]; width,height,has_headers,has_altitudes,length=struct.unpack_from('<5B',data)
        count=width*height; offset=5+length
        headers=struct.unpack_from('<'+str(count)+'H',data,offset) if has_headers else [map_id]*count
        offset+=(count*2 if has_headers else 0)+(count if has_altitudes else 0)
        models=struct.unpack_from('<'+str(count)+'H',data,offset)
        cells=[i for i in range(count) if headers[i]==map_id and models[i]!=0xFFFF]
        data=events[event_id]; offset=4+struct.unpack_from('<I',data)[0]*20
        offset+=4+struct.unpack_from('<I',data,offset)[0]*32
        warp_count=struct.unpack_from('<I',data,offset)[0]; offset+=4
        anchors=[]
        for warp_id in range(warp_count):
            x,z,destination,dest_warp,altitude=struct.unpack_from('<HHHHI',data,offset+warp_id*12)
            if dest_warp==0x100: continue
            anchors.append(dict(warpId=warp_id,x=x,z=z,direction=1,destination=destination,destinationWarp=dest_warp,altitude=altitude))
        anchors+=literals.get(map_id,[])
        valid=[]
        for anchor in anchors:
            x,z=anchor['x'],anchor['z']; index=(z//32)*width+x//32
            if x//32>=width or z//32>=height or index not in cells: continue
            model=models[index]; blob=land[model]
            if len(blob)<16 or sum(struct.unpack_from('<4I',blob))+16>len(blob): raise RuntimeError('Invalid real land member '+str(model))
            valid.append(dict(anchor,matrixIndex=index,model=model,landSha256=hashlib.sha256(blob).hexdigest()))
        name=names.get(map_id,'MAP_'+str(map_id))
        special=name in ('MAP_UNION','MAP_WIFI_SINGLE_BATTLE_AREA','MAP_WIFI_MULTI_BATTLE_AREA') or any(p in name for p in ('WIFI_PLAZA','BATTLE_TOWER','SAFARI_ZONE','COLOSSEUM'))
        classification='placeholder_or_no_model_cell' if not cells else 'special_mode_or_session_prerequisite' if special else 'validated_anchor_candidate' if valid else 'needs_verified_spawn_anchor'
        rows.append(dict(mapId=map_id,name=name,mapType=(flags>>8)&15,weather=(flags>>1)&127,matrixId=matrix_id,
            matrixWidth=width,matrixHeight=height,eventsBank=event_id,scriptHeader=script_id,
            initHeaderSha256=hashlib.sha256(scripts[script_id]).hexdigest(),initHeaderEmpty=scripts[script_id]==bytes(4),
            warpCount=warp_count,modelCells=len(cells),classification=classification,anchors=valid))
    return dict(mapTableSha256=profile['mapSha256'],romSha256=json.loads((pack/'manifest.json').read_text())['image']['sha256'],
        archives={str(p.relative_to(pack)):sha(p) for p in paths},counts=dict(Counter(row['classification'] for row in rows)),maps=rows,
        scriptSources={str(p.relative_to(ROOT)):sha(p) for p in sorted(set(script_sources))})


def main(map_ids=None):
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--save',required=True,help='Private Route29 fixture; the game opens only independent copies')
    args=parser.parse_args()
    audit=BUILD/'audit'; destination=BUILD/'field-maps-wide-scenario'; destination.mkdir(exist_ok=True)
    report_file=destination/'report.json'; report_file.unlink(missing_ok=True)
    source_save=Path(args.save).resolve()
    allowed=(ROOT.parent/'native-ios-route29-save/heartgold.sav').resolve()
    if source_save!=allowed or not source_save.is_file(): raise ValueError('Use only the designated private Route29 fixture')
    protected={str(p):sha(p) for p in sorted(source_save.parent.iterdir()) if p.is_file()}
    link=json.loads((audit/'engine-link.json').read_text()); audit_report=json.loads((audit/'report.json').read_text())
    if link['exitCode'] or sha(audit/'engine.wasm')!=link['outputSha256']: raise RuntimeError('Strict full engine required')
    include_paths=[ROOT/p for p in INCLUDES]
    if link.get('nativeContent'): include_paths.insert(0,ROOT/link['nativeContent']['include'])
    headers={p for folder in include_paths for pattern in ('**/*.h','**/*.naix') for p in folder.glob(pattern)}
    headers|=set((ROOT/'lib/MSL_C/src').glob('*.h'))|set((ROOT/'lib/MSL_C/src').glob('*.hpp'))
    if digest(headers)!=audit_report['headersSha256']: raise RuntimeError('Audited headers changed')
    units={row['file']:row for row in audit_report['units']}
    for name,unit in units.items():
        if not unit['exitCode'] and (ROOT/name).is_file() and sha(ROOT/name)!=unit['sha256']: raise RuntimeError('Unaudited source: '+name)
    pack=verify(link.get('nativeContent')); catalog=catalogue(audit,pack)
    (destination/'catalogue.json').write_text(json.dumps(catalog,indent=2)+'\n')
    candidates=[row for row in catalog['maps'] if row['classification']=='validated_anchor_candidate']
    selected=candidates if map_ids is None else [next(row for row in candidates if row['mapId']==n) for n in map_ids]
    command=link['command'].copy(); original_rsp=next(value for value in command if value.startswith('@'))
    objects=shlex.split(Path(original_rsp[1:]).read_text()); cached=[Path(p) for p in objects]
    seal=cached+[audit/'engine.wasm',Path(original_rsp[1:]),audit/'engine-link.json',audit/'report.json',audit/'native-content/report.json',audit/'native-content/native-map.o']
    cache_hashes={str(p):sha(p) for p in seal}
    host_flags,runtime=native_build_settings(destination)
    host_headers=set((ROOT/'ios/Original').rglob('*.h'))|set(runtime[0].parent.glob('*.h'))|set((WABT/'include').rglob('*.h'))
    owned=['ios/Original/abi32/field_maps_scenario_check.inc','ios/Original/tests/field_maps_scenario_host_check.c','ios/tools/check_native_field_maps.py']
    names=['src/field_system.c','src/field/map_load_manager.c']
    tracked=[ROOT/p for p,u in units.items() if not u['exitCode'] and (ROOT/p).is_file()]+runtime+[ROOT/p for p in owned]+[
        ROOT/'ios/Original/abi32/engine_check.c',ROOT/'ios/tools/check_native_rainy_field.py',ROOT/'ios/tools/check_native_engine.py',ROOT/'ios/tools/prepare_overlay_state.py',
        ROOT/'asm/macros/script.inc']+[ROOT/p for p in catalog['scriptSources']]+sorted(host_headers)
    source_hashes={str(p):sha(p) for p in tracked}
    commands=[]
    def run(argv,log):
        argv=list(map(str,argv)); commands.append(argv)
        result=subprocess.run(argv,cwd=ROOT,capture_output=True,text=True,timeout=240)
        (destination/log).write_text(result.stdout+result.stderr)
        if result.returncode: raise RuntimeError('Field-map build failed: '+str(destination/log))
    flags=['--target=wasm32-wasip1','-std=gnu11','-O2','-fwrapv','-fno-strict-aliasing','-fno-builtin-malloc','-fno-builtin-free',
        '-fwasm-exceptions','-fexceptions','-Xclang','-disable-red-zone','-fms-extensions','-Wno-microsoft','-Wno-unknown-pragmas']
    flags+=['-D'+n for n in ('HEARTGOLD_NATIVE','HEARTGOLD_ABI32','SDK_ARM9','SDK_CW','SDK_CODE_ARM','SDK_TS','_NITRO','PM_KEEP_ASSERTS','PLATFORM_INTRINSIC_FUNCTION_BIT_CLZ32=__builtin_clz')]
    for folder in INCLUDES: flags+=['-I',ROOT/folder]
    if link.get('nativeContent'): flags=['-I',ROOT/link['nativeContent']['include'],'-DHG_NATIVE_ROM_TABLES',*flags]
    field_code=(ROOT/names[0]).read_text(); anchor='BOOL finished = FieldSystem_Main(fieldSystem);'
    if field_code.count(anchor)!=1: raise RuntimeError('Field owner anchor changed')
    field=destination/'field-scenario.c'
    field.write_text('#include "'+str(ROOT/owned[0])+'"\n'+field_code.replace(anchor,anchor+'\n    FieldMapsQAFieldTick(fieldSystem);'))
    field_object=destination/'field-scenario.o'; run([SDK/'bin/clang',*flags,'-c',field,'-o',field_object],'field-compile.log')
    model_code=(ROOT/names[1]).read_text(); anchor='        GF3dRender_DrawModel(&mapLoadManager->maps[mapSlot]->renderObj, &pos, &rotation, &scale);'
    if model_code.count(anchor)!=1: raise RuntimeError('Real model draw anchor changed')
    model=destination/'map-scenario.c'
    model.write_text('unsigned FieldMapsQABuildingPolygons(void);\nvoid FieldMapsQAObserveMapModel(void *,int,const void *,unsigned,unsigned);\n'+model_code.replace(anchor,
        '        unsigned before=FieldMapsQABuildingPolygons();\n'+anchor+'\n        FieldMapsQAObserveMapModel(mapLoadManager,mapLoadManager->maps[mapSlot]->matrixIndex,mapLoadManager->maps[mapSlot]->modelFile,GetMapModelNo(mapLoadManager->maps[mapSlot]->matrixIndex,mapLoadManager->mapMatrix),before);'))
    model_object=destination/'map-scenario.o'
    code,diagnostics,state=compile_unit(SDK/'bin/clang',flags,model,model_object,names[1],ROOT)
    (destination/'map-compile.log').write_text(diagnostics); (destination/'map-overlay-state.json').write_text(json.dumps(state,indent=2)+'\n')
    if code or 'error' in state or state['getter']!=units[names[1]]['overlayStates'][0]['getter']: raise RuntimeError('QA changed overlay ownership')
    for name,new in ((names[0],field_object),(names[1],model_object)):
        old=str(audit/'objects'/Path(name).with_suffix('.o'))
        if objects.count(old)!=1: raise RuntimeError('Expected one audited object: '+old)
        objects[objects.index(old)]=str(new)
    listing=destination/'objects.rsp'; listing.write_text('\n'.join('"'+p+'"' for p in objects)+'\n')
    command[command.index(original_rsp)]='@'+str(listing); module=destination/'engine.wasm'; command[command.index('-o')+1]=str(module)
    for export in ('HGNativeFieldMapsQAGetState','HGNativeFieldMapsQARequest','HGNativeFieldMapsQAReturn','HGNativeFieldMapsQARender'): command.insert(command.index('-o'),'-Wl,--export='+export)
    for name in ('MapLoadManager_End','MapLoadManager_HasEnded','MapLoadManager_FreeNARCAndLoadedMapBuffers'): command.insert(command.index('-o'),'-Wl,--wrap='+name)
    run(command,'strict-link.log'); generated,writes,restores=generate_engine(destination,module,run)
    host_flags+=['-g','-fsanitize=address,undefined','-fno-sanitize-recover=all']
    def compile_one(item):
        index,path=item; output=destination/f'host-{index}.o'; extra=['-DWASM_RT_TRAP_HANDLER=HGEngineTrap'] if path.name=='wasm-rt-impl.c' else []
        run(['xcrun','clang',*host_flags,*extra,'-c',path,'-o',output],f'compile-{index}.log'); return output
    with ThreadPoolExecutor(max_workers=4) as pool: native=list(pool.map(compile_one,enumerate([*generated,*runtime,ROOT/owned[1]])))
    binary=destination/'field-maps-check'; run(['xcrun','clang','-fsanitize=address,undefined',*native,'-o',binary],'native-link.log')
    boot=destination/'boot-header.bin'; boot.write_bytes(read_boot_header(pack)); results=[]
    for row in selected:
        case=destination/('map-'+str(row['mapId'])); case.mkdir(exist_ok=True); frames=case/'frames'; frames.mkdir(exist_ok=True)
        for path in case.glob('scenario.sav*'): path.unlink()
        for path in frames.glob('*.ppm'): path.unlink()
        save=case/'scenario.sav'
        for path in source_save.parent.glob(source_save.name+'*'):
            if path.is_file(): shutil.copyfile(path,case/('scenario.sav'+path.name[len(source_save.name):]))
        anchor=row['anchors'][0]
        argv=list(map(str,[binary,pack/'files',boot,save,frames,row['mapId'],anchor['warpId'],anchor['x'],anchor['z'],anchor['direction'],anchor['matrixIndex'],anchor['model']]))
        commands.append(argv)
        try:
            result=subprocess.run(argv,cwd=ROOT,capture_output=True,text=True,timeout=120); exit_code=result.returncode; output=result.stdout+result.stderr
        except subprocess.TimeoutExpired as error:
            exit_code=None; output=(error.stdout or b'').decode()+(error.stderr or b'').decode()
        (case/'runtime.log').write_text(output)
        evidence=[json.loads(line.split('Field maps evidence: ',1)[1]) for line in output.splitlines() if line.startswith('Field maps evidence: ')]
        passed=exit_code==0 and len(evidence)==1 and evidence[0]['status']=='passed_continuous_512' and 'PASS: real field ScriptWarp' in output
        result_row=dict(mapId=row['mapId'],name=row['name'],mapType=row['mapType'],anchor=anchor,passed=passed,exitCode=exit_code,evidence=evidence,
            status='passed' if passed else evidence[0]['status'] if evidence else 'wall_clock_timeout' if exit_code is None else 'native_runtime_failure',
            frames=[dict(file=str(p.relative_to(destination)),sha256=sha(p)) for p in sorted(frames.glob('*.ppm'))])
        results.append(result_row); (case/'report.json').write_text(json.dumps(result_row,indent=2)+'\n')
        print(('PASS' if passed else 'UNVERIFIED')+': '+row['name'],flush=True)
        unchanged=all(sha(Path(p))==h for p,h in protected.items()) and all(sha(Path(p))==h for p,h in cache_hashes.items()) and all(sha(Path(p))==h for p,h in source_hashes.items()) and digest(headers)==audit_report['headersSha256']
        complete=len(results)==len(selected)
        report=dict(scope='continuous512 retained before original submission across real epochs, main512/sub256/main512 and canonical center, owner ScriptWarp/model draw/return teardown; explicit fallback separately classified; not story/all-cell/full-game validation',
            complete=complete,passed=complete and all(r['passed'] for r in results) and unchanged,productionModuleSha256=link['outputSha256'],scenarioModuleSha256=sha(module),
            sourceSaveSha256=protected[str(source_save)],protectedFixtureHashes=protected,sharedCacheAndSourcesUnchanged=unchanged,
            catalogueCounts=catalog['counts'],requestedMaps=[r['mapId'] for r in selected],completedCases=results,
            remainingCandidateMaps=[r['mapId'] for r in candidates if r['mapId'] not in {x['mapId'] for x in results}],
            unverifiedCandidateMaps=[r['mapId'] for r in candidates if r['mapId'] not in {x['mapId'] for x in results if x['passed']}],
            sourceHashes=source_hashes,cacheHashes=cache_hashes,commands=commands,checkedStackWrites=writes,exceptionDepthRestores=restores)
        report_file.write_text(json.dumps(report,indent=2)+'\n')
        if not unchanged: raise RuntimeError('Protected fixture/cache/source/header changed')
    return 0 if all(r['passed'] for r in results) else 1


if __name__=='__main__': raise SystemExit(main())
