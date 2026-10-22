# SPDX-License-Identifier: GPL-3.0-or-later
"""Verify native data selection using real assemblers and existing code objects, outside the audit cache."""
import argparse
import ast
import copy
import hashlib
import json
import os
from pathlib import Path
import re
import sys

ROOT=Path(__file__).resolve().parents[2]
parser=argparse.ArgumentParser(description=__doc__)
parser.add_argument('--output',type=Path,required=True)
args=parser.parse_args(); OUT=args.output.resolve(); cache=ROOT/'ios/build-original-abi32/audit'
assert OUT!=cache and cache not in OUT.parents,'Do not overwrite the real audit cache'
OUT.mkdir(parents=True,exist_ok=True)
sys.path.insert(0,str(ROOT/'ios/tools'))
import build_original_data as data
SDK=Path(os.environ.get('HG_ABI32_TOOLS',ROOT/'ios/build-original-abi32/tools'))/'wasi-sdk-34.0-arm64-macos'
tree=ast.parse((ROOT/'ios/tools/check_abi32.py').read_text())
main=next(n for n in tree.body if isinstance(n,ast.FunctionDef) and n.name=='main')
values={}
for name in ('includes','flags'):
    node=next(n for n in main.body if isinstance(n,ast.Assign) and any(isinstance(t,ast.Name) and t.id==name for t in n.targets))
    values[name]=eval(compile(ast.Expression(node.value),'<actual audit flags>','eval'),{'ROOT':ROOT})
flags=values['flags']
for include in values['includes']:flags+=['-I',include]
link=json.loads((cache/'engine-link.json').read_text())
assert link['exitCode']==0 and not link['undefinedSymbols']
engine_sha=hashlib.sha256((cache/'engine.wasm').read_bytes()).hexdigest()
assert engine_sha==link['outputSha256']
if link.get('nativeContent'):flags=['-I',ROOT/link['nativeContent']['include'],'-DHG_NATIVE_ROM_TABLES',*flags]
objects=[Path(json.loads(line)) for line in (cache/'native-objects.rsp').read_text().splitlines() if line.strip()]
objects=[p for p in objects if cache/'objects' in p.parents]
assert objects
specification=(ROOT/'main.lsf').read_text()
sources=[(ROOT/name).with_suffix('.s') for name in dict.fromkeys(re.findall(r'^\s*Object\s+([^\s]+\.o)',specification,re.M)) if (ROOT/name).with_suffix('.s').is_file()]
baseline=json.loads((cache/'original-data/report.json').read_text())
assert len(sources)==baseline['summary']['total']==150

verify=data.verify; proof_checks=[]
mixed_targets={OUT/'data'/Path(name).with_suffix('.o'):name for name,pin in data._NATIVE_ARM_EXCEPTION_DATA.items() if pin[2]}
def checked_verify(path,expected):
    result=verify(path,expected)
    if Path(path) in mixed_targets:
        for kind in ('bytes','relocations','symbols'):
            damaged=copy.deepcopy(expected)
            segment=next(s for s in damaged.values() if s['bytes'] and (kind!='symbols' or s['exports']))
            if kind=='bytes':segment['bytes']=bytes([segment['bytes'][0]^1])+segment['bytes'][1:]
            elif kind=='relocations':
                if segment['relocations']:next(iter(segment['relocations'].values()))['addend']+=4
                else:segment['relocations'][0]=dict(name='UnexpectedRelocation',kind=5,addend=0)
            else:
                name=next(iter(segment['exports']));offset,size,weak=segment['exports'][name]
                segment['exports'][name]=(offset+1,size,weak)
            try:verify(path,damaged)
            except ValueError as error:assert 'differ' in str(error)
            else:raise AssertionError('Changed ordinary '+kind+' accepted')
            proof_checks.append(dict(file=mixed_targets[Path(path)],rejected=kind))
    return result
data.verify=checked_verify
report=data.build(objects,SDK,flags,OUT/'data',sources)
assert report['summary']==baseline['summary'],report['summary']
assert report['functionTypesSha256']==baseline['functionTypesSha256'],'Code registry differs from the strict engine inputs'
assert len(proof_checks)==33
raw_fingerprint=hashlib.sha256(json.dumps(report,sort_keys=True).encode()).hexdigest()
selection=data.native_data_selection(report,flags,SDK)
assert not selection['errors'] and selection['enabled'],selection
assert selection['summary']==dict(total=136,verified=136,blocked=0,excludedMetadataOnly=14),selection['summary']
assert len(selection['exceptionMetadataReplacements'])==25
assert sum(u['bytes'] for u in report['units'] if 'constantsObject' in u)==2202
assert raw_fingerprint==hashlib.sha256(json.dumps(report,sort_keys=True).encode()).hexdigest(),'Selection changed raw ARM facts'
audit=next(n for n in tree.body if isinstance(n,ast.FunctionDef) and n.name=='audit')
mapping=next(n for n in ast.walk(audit) if isinstance(n,ast.Expr) and isinstance(n.value,ast.Call)
    and isinstance(n.value.func,ast.Attribute) and n.value.func.attr=='update'
    and isinstance(n.value.func.value,ast.Name) and n.value.func.value.id=='native_by_name')
mapping_env=dict(Path=Path,native_by_name={},data_selection=selection)
exec(compile(ast.Module(body=[mapping],type_ignores=[]),'<actual native data link selection>','exec'),mapping_env)
assert set(mapping_env['native_by_name'].values())=={Path(u.get('object',u.get('constantsObject'))) for u in report['units'] if u.get('object') or u.get('constantsObject')},'Ordinary data was omitted from the Native link selection'

def rejected(candidate,options=flags,sdk=SDK):
    result=data.native_data_selection(candidate,options,sdk)
    assert result['errors'] or result['summary']['blocked'],result
negative=[]
failed_index=next(i for i,u in enumerate(report['units']) if 'constantsObject' in u)
for kind in ('source-hash','metadata-records','nonmetadata-error','missing-constants','unknown-failure'):
    changed=copy.deepcopy(report);unit=changed['units'][failed_index]
    if kind=='source-hash':unit['sha256']='0'*64
    elif kind=='metadata-records':unit['excludedArmExceptionMetadata'].append('unexpected metadata')
    elif kind=='nonmetadata-error':unit['error']='ordinary data conversion failed'
    elif kind=='missing-constants':unit.pop('constantsObject')
    else:
        unit['file']='ios/Original/abi32/data_fixture.s'
        unit['sha256']=hashlib.sha256((ROOT/unit['file']).read_bytes()).hexdigest()
    rejected(changed);negative.append(kind)
unit=report['units'][failed_index];target=Path(unit['constantsObject']);assert OUT in target.parents
original=target.read_bytes()
try:
    target.write_bytes(original[:-1]+bytes([original[-1]^1]));rejected(report);negative.append('changed-verified-object')
finally:target.write_bytes(original)
for missing in ('-fwasm-exceptions','-fexceptions','-DHEARTGOLD_ABI32'):
    rejected(report,[f for f in flags if f!=missing]);negative.append('missing '+missing)
rejected(report,[*flags,'-UHEARTGOLD_ABI32']);negative.append('undefined ABI32 target')
rejected(report,flags,SDK/'missing');negative.append('missing native EH libraries')
(OUT/'native-selection.json').write_text(json.dumps(selection,indent=2)+'\n')

# Run the existing real EH suite with only its output directory redirected.
eh_source=ROOT/'ios/tests/check_cpp_exception_runtime.py'
eh_tree=ast.parse(eh_source.read_text())
for node in eh_tree.body:
    if isinstance(node,ast.Assign) and any(isinstance(t,ast.Name) and t.id=='BUILD' for t in node.targets):
        node.value=ast.parse('Path('+repr(str(OUT/'cpp-exceptions'))+')',mode='eval').body
exec(compile(ast.fix_missing_locations(eh_tree),str(eh_source),'exec'),{'__file__':str(eh_source),'__name__':'__main__'})
eh=json.loads((OUT/'cpp-exceptions/report.json').read_text())
assert eh['nativeCatchDepthRestores']>0
for relative,digest in eh['runtimeLibraries'].items():assert selection['providers'][str(SDK/relative)]==digest
assert hashlib.sha256((cache/'engine.wasm').read_bytes()).hexdigest()==engine_sha
(OUT/'report.json').write_text(json.dumps(dict(passed=True,scope='Native data selection and existing real EH component; full engine/App validation separate',
    rawSummary=report['summary'],rawFailures=sum(bool(u['exitCode']) for u in report['units']),nativeSummary=selection['summary'],
    mixedOrdinaryBytes=2202,ordinaryProofNegativeChecks=proof_checks,selectionNegativeChecks=negative,
    engineReadOnlySha256=engine_sha,exceptionRuntime=eh),indent=2)+'\n')
print('PASS: raw 150/25 retained; 136 ordinary objects verified; 14 exact ARM metadata-only inputs excluded; 33 real data-proof and 11 selection rejection checks; native EH runtime.')
