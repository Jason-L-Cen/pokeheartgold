# SPDX-License-Identifier: GPL-3.0-or-later
"""Run original doubles targets and a full animated turn in an isolated native save copy."""
import argparse
from concurrent.futures import ThreadPoolExecutor
import hashlib
import json
from pathlib import Path
import shlex
import shutil
import struct
import subprocess

from check_abi32 import ROOT, BUILD, SDK, WABT, native_build_settings
from check_native_engine import generate_engine
from import_original_content import read_boot_header,narc
from prepare_native_content import verify
from prepare_overlay_state import compile_unit
from audit_original import digest


def sha(path): return hashlib.sha256(path.read_bytes()).hexdigest()


def elite_resources(pack):
    manifest=json.loads((pack/'manifest.json').read_text()); archives={}; loaded={}
    for name in ('a/1/0/9','a/0/5/5','a/0/5/6'):
        entry=next(e for e in manifest['files'] if e['path']==name); path=pack/'files'/name
        if sha(path)!=entry['sha256']: raise RuntimeError('Elite archive differs from extracted ROM: '+name)
        archives[name]=entry; loaded[name]=narc(path)
    sprites=loaded['a/1/0/9']; members={}
    required=[16,47,48,49,50,51,52,53,54,59,60,61,62,151,152,*range(131,151),*range(207,215)]
    for member in required:
        data=sprites[member]
        if member in (151,152):
            if data[:8]!=b' APS12_1' or struct.unpack_from('<HH',data,8)!=(member-148,2): raise RuntimeError('Original SPL resources changed')
        elif data[:4] not in (b'RLCN',b'RGCN',b'RECN',b'RNAN'): raise RuntimeError('Original intro sprite resource changed')
        members[str(member)]=dict(size=len(data),sha256=hashlib.sha256(data).hexdigest(),magic=data[:8].hex())
    trainers=[]
    for trainer,cls in ((245,87),(247,89),(418,112),(246,88),(244,86)):
        data=loaded['a/0/5/5'][trainer]; party=loaded['a/0/5/6'][trainer]
        if len(data)!=20 or data[1]!=cls or not data[3] or struct.unpack_from('<I',data,16)[0]: raise RuntimeError('Elite trainer must be actual single-battle class')
        trainers.append(dict(id=trainer,trainerClass=cls,partyCount=data[3],dataSha256=hashlib.sha256(data).hexdigest(),partySha256=hashlib.sha256(party).hexdigest()))
    return dict(imageSha256=manifest['image']['sha256'],archiveSourceId=109,archives=archives,members=members,trainers=trainers)


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--save',required=True,type=str,help='Private Route29 save; copied, never opened by the game')
    parser.add_argument('--elite-four',action='store_true',help='Run the bounded five original single-trainer Elite Four/Champion intro cases')
    args=parser.parse_args()
    audit=BUILD/'audit'; destination=BUILD/('elite-four-scenario' if args.elite_four else 'battle-scenario'); destination.mkdir(exist_ok=True)
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
    resource_proof=elite_resources(pack) if args.elite_four else None
    audit_report=json.loads((audit/'report.json').read_text())
    audited_sources=[ROOT/row['file'] for row in audit_report['units'] if not row['exitCode'] and (ROOT/row['file']).is_file()]
    for row in audit_report['units']:
        if not row['exitCode'] and (ROOT/row['file']).is_file() and sha(ROOT/row['file'])!=row['sha256']:
            raise RuntimeError('Source differs from the full audit: '+row['file'])
    original_rsp=next(value for value in link['command'] if value.startswith('@'))
    cached=[Path(p) for p in shlex.split(Path(original_rsp[1:]).read_text())]
    cached+=[Path(p) for p in link['command'] if p.endswith(('.o','.a'))]
    cached+=[audit/'engine.wasm',Path(original_rsp[1:]),audit/'engine-link.json',audit/'report.json',pack/'manifest.json']
    if link.get('nativeContent'):
        cached.append(ROOT/link['nativeContent']['report'])
    if args.elite_four: cached+=[pack/'files'/name for name in resource_proof['archives']]
    cache_hashes={str(p):sha(p) for p in cached}
    commands=[]

    def run(command,log):
        command=list(map(str,command)); commands.append(command)
        result=subprocess.run(command,cwd=ROOT,capture_output=True,text=True)
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
    if native_content: flags=['-I',ROOT/native_content['include'],'-DHG_NATIVE_ROM_TABLES',*flags]
    include_paths=[ROOT/p for p in includes]
    if native_content: include_paths.insert(0,ROOT/native_content['include'])
    headers={p for folder in include_paths for pattern in ('**/*.h','**/*.naix') for p in folder.glob(pattern)}
    headers|=set((ROOT/'lib/MSL_C/src').glob('*.h'))|set((ROOT/'lib/MSL_C/src').glob('*.hpp'))
    if digest(headers)!=audit_report['headersSha256']: raise RuntimeError('Headers differ from the full audit')
    field_object=destination/'field-scenario.o'
    run([SDK/'bin/clang',*flags,'-c',field,'-o',field_object],'field-compile.log')
    battle_unit=next(row for row in json.loads((audit/'report.json').read_text())['units'] if row['file']=='src/battle/battle_main.c')
    battle_source=ROOT/battle_unit['file']; battle_code=battle_source.read_text()
    if battle_unit['exitCode'] or hashlib.sha256(battle_source.read_bytes()).hexdigest()!=battle_unit['sha256']:
        raise RuntimeError('Battle source differs from its audited object')
    anchor='    return battleSystem->battleFinished;'
    if battle_code.count(anchor)!=1: raise RuntimeError('Original battle owner entry changed')
    battle=destination/'battle-scenario.c'
    battle.write_text('#include "battle/battle_system.h"\nvoid BattleQAObserve(BattleSystem *battleSystem);\n'+
                      battle_code.replace(anchor,'    BattleQAObserve(battleSystem);\n'+anchor))
    battle_object=destination/'battle-scenario.o'
    code,diagnostics,battle_state=compile_unit(SDK/'bin/clang',flags,battle,battle_object,battle_unit['file'],ROOT)
    (destination/'battle-compile.log').write_text(diagnostics)
    (destination/'battle-overlay-state.json').write_text(json.dumps(battle_state,indent=2)+'\n')
    if code or 'error' in battle_state or battle_state['getter']!=battle_unit['overlayStates'][0]['getter']:
        raise RuntimeError('Battle QA copy must retain its audited overlay state ownership')
    leaders_unit=None; leaders_object=None
    if args.elite_four:
        leaders_unit=next(row for row in audit_report['units'] if row['file']=='src/field/encounter_effect_leaders.c')
        leaders_source=ROOT/leaders_unit['file']; leaders_code=leaders_source.read_text()
        if leaders_unit['exitCode'] or sha(leaders_source)!=leaders_unit['sha256']: raise RuntimeError('Leaders source differs from its audited object')
        signature='static BOOL LeadersEffect_RunEliteFourIntro(EncounterEffectTask *effect, enum HeapID heapID, const EliteFourIntroParams *params) {'
        if leaders_code.count(signature)!=1: raise RuntimeError('Original elite owner signature changed')
        start=leaders_code.index(signature); position=leaders_code.index('    switch (effect->state) {',start)
        leaders=destination/'leaders-scenario.c'
        leaders.write_text('#include "field/encounter_effect.h"\nvoid BattleQAEliteObserve(EncounterEffectTask *,Sprite *const *);\n'+
            leaders_code[:position]+'    BattleQAEliteObserve(effect,effect->state>=2 ? work->sprites : NULL);\n'+leaders_code[position:])
        leaders_object=destination/'leaders-scenario.o'
        code,diagnostics,leaders_state=compile_unit(SDK/'bin/clang',flags,leaders,leaders_object,leaders_unit['file'],ROOT)
        (destination/'leaders-compile.log').write_text(diagnostics)
        (destination/'leaders-overlay-state.json').write_text(json.dumps(leaders_state,indent=2)+'\n')
        if code or 'error' in leaders_state or leaders_state['getter']!=leaders_unit['overlayStates'][0]['getter']:
            raise RuntimeError('Elite QA copy must retain its audited overlay state ownership')
    command=link['command'].copy(); old_rsp=next(value for value in command if value.startswith('@'))
    objects=shlex.split(Path(old_rsp[1:]).read_text())
    original_object=str(audit/'objects/src/field_system.o')
    if objects.count(original_object)!=1: raise RuntimeError('Expected one field object')
    objects[objects.index(original_object)]=str(field_object)
    original_object=str(audit/'objects/src/battle/battle_main.o')
    if objects.count(original_object)!=1: raise RuntimeError('Expected one battle owner object')
    objects[objects.index(original_object)]=str(battle_object)
    if leaders_object:
        original_object=str(audit/'objects/src/field/encounter_effect_leaders.o')
        if objects.count(original_object)!=1: raise RuntimeError('Expected one original leaders object')
        objects[objects.index(original_object)]=str(leaders_object)
    listing=destination/'objects.rsp'; listing.write_text('\n'.join('"'+value+'"' for value in objects)+'\n')
    command[command.index(old_rsp)]='@'+str(listing)
    module=destination/'engine.wasm'; command[command.index('-o')+1]=str(module)
    command.insert(command.index('-o'),'-Wl,--export=HGNativeBattleQARequest')
    command.insert(command.index('-o'),'-Wl,--export=HGNativeBattleQAGetState')
    command.insert(command.index('-o'),'-Wl,--export=HGNativeBattleQAArm')
    for reply in ('Command','Move','Target'):
        command.insert(command.index('-o'),'-Wl,--wrap=BattleController_Reply'+reply+'Selection')
    if args.elite_four:
        for wrapped in ('BattleSetup_GetWildTransitionEffect','BattleSetup_GetWildBattleMusic','Battle_LaunchApp',
            'EncounterEffect_Start','EncounterEffect_UnloadOverlay','EncounterEffect_RequestParticleSetup',
            'EncounterEffect_LoadSpriteResources','EncounterEffect_FreeSpriteResources','EncounterEffect_FreeSprites',
            'EncounterEffect_LoadParticles','EncounterEffect_FreeParticles','EncounterEffect_EndParticleMode','EncounterEffect_Finish',
            'ParticleSystem_CreateEmitter','ParticleSystem_DrawAll','ParticleSystem_CalcAll','RemoveWindow'):
            command.insert(command.index('-o'),'-Wl,--wrap='+wrapped)
    run(command,'strict-link.log')
    generated,writes,restores=generate_engine(destination,module,run)
    host_flags,sources=native_build_settings(destination)
    host_headers=set((ROOT/'ios/Original').rglob('*.h'))|set(sources[0].parent.glob('*.h'))|set((WABT/'include').rglob('*.h'))
    tracked=[*audited_sources,*sorted(host_headers),source,battle_source,ROOT/'ios/Original/abi32/engine_check.c',*sources,
        ROOT/'ios/tools/check_native_battle_scenario.py',ROOT/'ios/Original/abi32/battle_scenario_field.inc',ROOT/'ios/Original/abi32/battle_scenario_check.c']
    if leaders_unit: tracked.append(ROOT/leaders_unit['file'])
    source_hashes={str(p):sha(p) for p in tracked}
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
    artifacts=[module,binary,*generated,destination/'hgengine.h',destination/'hgengine-impl.h',*objects]
    artifact_hashes={str(p):sha(p) for p in artifacts}
    boot=destination/'boot-header.bin'; boot.write_bytes(read_boot_header(pack))
    if args.elite_four:
        companions=[save_source,*[save_source.with_name(save_source.name+suffix) for suffix in ('.console','.lock','.wifi')]]
        companion_hashes={str(p):sha(p) if p.exists() else None for p in companions}
        cases=[]
        for case,name in enumerate(('doubles','will','koga','bruno','karen','lance')):
            folder=destination/name; folder.mkdir(exist_ok=True); case_save=folder/'scenario.sav'; shutil.copyfile(save_source,case_save)
            frames=folder/'frames'; frames.mkdir(exist_ok=True)
            for path in frames.glob('*.ppm'): path.unlink()
            argv=list(map(str,[binary,pack/'files',boot,case_save,frames,'--continue',*([case] if case else [])])); commands.append(argv)
            try:
                result=subprocess.run(argv,cwd=ROOT,capture_output=True,text=True,timeout=180)
                status=result.returncode; output=result.stdout+result.stderr
            except subprocess.TimeoutExpired as error:
                status=None; output=(error.stdout or b'').decode()+(error.stderr or b'').decode()
            (folder/'runtime.log').write_text(output)
            prefix='Elite QA evidence: ' if case else 'Battle QA turn evidence: '
            evidence=[json.loads(line.split(prefix,1)[1]) for line in output.splitlines() if line.startswith(prefix)]
            frame_evidence=[json.loads(line.split('Elite frame evidence: ',1)[1]) for line in output.splitlines() if line.startswith('Elite frame evidence: ')]
            images=[dict(file=p.name,sha256=sha(p)) for p in sorted(frames.glob('elite-*.ppm' if case else 'battle-action-*.ppm'))]
            passed=status==0 and len(evidence)==1 and (len(frame_evidence)==4 and len(images)==4 and 'PASS: full original Elite Four/Champion intro' in output if case else len(images)==2 and 'PASS: real original doubles animated turn' in output)
            cases.append(dict(case=case,name=name,passed=passed,exitCode=status,evidence=evidence,frameEvidence=frame_evidence,frames=images))
            if not passed: break
        unchanged=all((sha(Path(p)) if Path(p).exists() else None)==v for p,v in companion_hashes.items())
        production_unchanged=sha(audit/'engine.wasm')==link['outputSha256']
        cache_unchanged=all(sha(p)==cache_hashes[str(p)] for p in cached)
        sources_unchanged=all(sha(p)==source_hashes[str(p)] for p in tracked)
        artifacts_unchanged=all(sha(p)==artifact_hashes[str(p)] for p in artifacts)
        passed=len(cases)==6 and all(c['passed'] for c in cases) and unchanged and production_unchanged and cache_unchanged and sources_unchanged and artifacts_unchanged and digest(headers)==audit_report['headersSha256']
        report.write_text(json.dumps(dict(passed=passed,scope='unchanged native doubles turn plus five original Elite Four/Champion intros and actual command handoff; Route29 background, no league story/settlement/UIKit/device claim',
            cases=cases,resources=resource_proof,sourceSaveUnchanged=unchanged,saveCompanionHashes=companion_hashes,
            productionModuleUnchanged=production_unchanged,productionModuleSha256=link['outputSha256'],scenarioModuleSha256=sha(module),
            sharedObjectsUnchanged=cache_unchanged,sharedInputHashes=cache_hashes,sourcesUnchanged=sources_unchanged,sourceHashes=source_hashes,
            hostHeaderHashes={str(p):source_hashes[str(p)] for p in sorted(host_headers)},generatedArtifactsUnchanged=artifacts_unchanged,artifactHashes=artifact_hashes,commands=commands,
            checkedStackWrites=writes,exceptionDepthRestores=restores),indent=2)+'\n')
        print(('PASS' if passed else 'FAIL')+': '+str(report)); return 0 if passed else 1
    shutil.copyfile(save_source,save)
    frames=destination/'frames'; frames.mkdir(exist_ok=True)
    for path in frames.glob('battle-action-*.ppm'): path.unlink()
    try:
        result=subprocess.run([str(binary),str(pack/'files'),str(boot),str(save),str(frames),'--continue'],cwd=ROOT,capture_output=True,text=True,timeout=180)
        status=result.returncode; output=result.stdout+result.stderr
    except subprocess.TimeoutExpired as error:
        status=None; output=(error.stdout or b'').decode()+(error.stderr or b'').decode()
    (destination/'runtime.log').write_text(output)
    unchanged=hashlib.sha256(save_source.read_bytes()).hexdigest()==original_save_hash
    evidence=[json.loads(line.split('Battle QA turn evidence: ',1)[1]) for line in output.splitlines() if line.startswith('Battle QA turn evidence: ')]
    production_unchanged=hashlib.sha256((audit/'engine.wasm').read_bytes()).hexdigest()==link['outputSha256']
    passed=status==0 and 'PASS: real original doubles animated turn' in output and unchanged and production_unchanged and len(evidence)==1
    report.write_text(json.dumps(dict(scope='native host original doubles targets and animated turn; UIKit, settlement and 1:1 fidelity unverified',
        passed=passed,exitCode=status,sourceSaveUnchanged=unchanged,sourceSaveSha256=original_save_hash,
        productionModuleUnchanged=production_unchanged,turnEvidence=evidence,
        renderedBattleFrames=[dict(path=str(p.relative_to(ROOT)),sha256=hashlib.sha256(p.read_bytes()).hexdigest()) for p in sorted(frames.glob('battle-action-*.ppm'))],
        productionModuleSha256=link['outputSha256'],scenarioModuleSha256=hashlib.sha256(module.read_bytes()).hexdigest(),
        fixtureSha256={name:hashlib.sha256((ROOT/name).read_bytes()).hexdigest() for name in (
            'ios/tools/check_native_battle_scenario.py','ios/Original/abi32/battle_scenario_field.inc','ios/Original/abi32/battle_scenario_check.c')},
        checkedStackWrites=writes,exceptionDepthRestores=restores),indent=2)+'\n')
    print(('PASS' if passed else 'FAIL')+': '+str(report))
    return 0 if passed else 1


if __name__=='__main__': raise SystemExit(main())
