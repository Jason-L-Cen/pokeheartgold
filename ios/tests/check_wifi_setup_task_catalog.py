# SPDX-License-Identifier: GPL-3.0-or-later
"""Run the actual audit registration/ABI blocks against existing canonical task objects."""
import argparse
import ast
from concurrent.futures import ThreadPoolExecutor
import copy
import hashlib
import json
import os
from pathlib import Path
import re
import subprocess

ROOT=Path(__file__).resolve().parents[2]
parser=argparse.ArgumentParser(description=__doc__)
parser.add_argument('--report',type=Path)
args=parser.parse_args()
destination=ROOT/'ios/build-original-abi32/audit'
tools=Path(os.environ.get('HG_ABI32_TOOLS',ROOT/'ios/build-original-abi32/tools'))
tree=ast.parse((ROOT/'ios/tools/check_abi32.py').read_text())
def assignment(node,name):
    return isinstance(node,ast.Assign) and any(isinstance(t,ast.Name) and t.id==name for t in node.targets)
body=next(node.body for node in ast.walk(tree) if hasattr(node,'body') and isinstance(node.body,list)
          and any(assignment(n,'setup_task_sources') for n in node.body))
start=next(i for i,n in enumerate(body) if assignment(n,'setup_task_sources'))
signature_index=next(i for i,n in enumerate(body) if isinstance(n,ast.FunctionDef) and n.name=='signatures')
scanner=body[start:signature_index]
inventory_start=next(i for i,n in enumerate(body) if assignment(n,'function_types'))
inventory_end=next(i for i,n in enumerate(body[inventory_start:],inventory_start) if isinstance(n,ast.With))
inventory=body[inventory_start:inventory_end+1]
contract_start=next(i for i,n in enumerate(body) if assignment(n,'setup_task_contracts'))
contracts=body[contract_start:contract_start+3]
def execute(nodes,namespace):
    exec(compile(ast.fix_missing_locations(ast.Module(body=copy.deepcopy(nodes),type_ignores=[])),str(ROOT/'ios/tools/check_abi32.py'),'exec'),namespace)

units={u['file']:u for u in json.loads((destination/'report.json').read_text())['units']}
sources=sorted((ROOT/'src').rglob('wifi_setup*.c'))
assert sources
selected=[]
for source in sources:
    relative=str(source.relative_to(ROOT)); unit=units[relative]
    assert unit['exitCode']==0 and unit['sha256']==hashlib.sha256(source.read_bytes()).hexdigest(),relative
    obj=destination/'objects'/Path(relative).with_suffix('.o')
    assert obj.is_file(),obj
    selected.append(obj)
preserved_definitions={'catalog-sentinel'}
namespace=dict(ROOT=ROOT,WABT=tools/'wabt-1.0.42',re=re,subprocess=subprocess,destination=destination,
               ThreadPoolExecutor=ThreadPoolExecutor,selected=selected,definitions=preserved_definitions)
execute(scanner+[body[signature_index]],namespace)
execute(inventory,namespace)
execute(contracts,namespace)
assert namespace['definitions'] is preserved_definitions and preserved_definitions=={'catalog-sentinel'},'Task validation replaced the outer definition set'
assert not namespace['setup_task_errors'],namespace['setup_task_errors']
names=namespace['setup_task_names']; definitions=namespace['setup_task_definitions']
assert len(names)>=74
local_names={name for name,rows in definitions.items() if any(row['local'] for row in rows)}
assert local_names
assert not local_names.intersection(name for name,types in namespace['function_types'].items() if any(sites['definitions'] for sites in types.values()))

# The old flat path must fail the unchanged completeness threshold.
old_scanner=copy.deepcopy(scanner)
for statement in old_scanner:
    for node in ast.walk(statement):
        if isinstance(node,ast.Attribute) and node.attr=='rglob':node.attr='glob'
try:execute(old_scanner,dict(namespace))
except RuntimeError as error:assert 'registrations are missing' in str(error)
else:raise AssertionError('Flat source scan unexpectedly accepted')

# Omitting local definitions, or borrowing another TU's local, must fail ABI validation.
for mode in ('nonlocal-only','wrong-local-owner'):
    rejected=dict(namespace)
    rejected['setup_task_definitions']={name:[dict(row,object='other/translation-unit.o') if mode=='wrong-local-owner' and row['local'] else dict(row)
        for row in rows if mode!='nonlocal-only' or not row['local']] for name,rows in definitions.items()}
    execute(contracts,rejected)
    assert local_names<=set(rejected['setup_task_errors']),mode

report=dict(scope='Current source registrations and actual cached ABI definitions; callback behavior/gameplay separate',
    sourceObjects=len(selected),registeredCallbacks=len(names),localCallbacks=len(local_names),
    callbackContracts=namespace['setup_task_contracts'],registrationObjects={n:sorted(s) for n,s in namespace['setup_task_sources'].items()},
    definitions=definitions,objectSHA256={str(p.relative_to(destination)):hashlib.sha256(p.read_bytes()).hexdigest() for p in selected},
    rejected=['old flat source scan','old nonlocal-only filter','other TU local substitution'],globalDefinitionSetPreserved=True,passed=True)
if args.report:
    args.report.parent.mkdir(parents=True,exist_ok=True)
    args.report.write_text(json.dumps(report,indent=2)+'\n')
print(json.dumps({k:report[k] for k in ('sourceObjects','registeredCallbacks','localCallbacks','rejected','passed')}))
