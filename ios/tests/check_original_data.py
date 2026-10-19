"""Check original data bytes, aliases and relocations with the real two assemblers."""
from pathlib import Path
import copy
import os
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0,str(ROOT/'ios/tools'))
from build_original_data import prepare_arm,emit,verify,wasm_object,arm_object,build

sdk = Path(os.environ.get('HG_ABI32_TOOLS',ROOT/'ios/build-original-abi32/tools'))/'wasi-sdk-34.0-arm64-macos'
folder = ROOT/'ios/build-original-abi32/data-check'; folder.mkdir(parents=True,exist_ok=True)
source = ROOT/'ios/Original/abi32/data_fixture.s'
reference = prepare_arm(source,folder/'fixture.arm.o',sdk,[])
functions = {'LCRandom':((),(0x7F,))}
overlays = {'SDK_OVERLAY_OVY_7_ID':7}
assembly,expected,_ = emit(reference,functions,set(),overlays,'fixture')
native = folder/'fixture.s'; native.write_text(assembly)
result = subprocess.run([str(sdk/'bin/clang'),'-c',str(native),'-o',str(folder/'fixture.o')],capture_output=True,text=True)
assert result.returncode==0,result.stderr
assert verify(folder/'fixture.o',expected)=={'HgAsmBytes','HgAsmTarget','HgAsmRng','HgAsmAlias','HgAsmZero','HgAsmOverlay','HgSdkData','HgSdkZero'}
damaged = copy.deepcopy(expected)
first = next(value for value in damaged.values() if value['bytes'][:4]==bytes([1,2,3,4]))
first['bytes']=bytes([9])+first['bytes'][1:]
try: verify(folder/'fixture.o',damaged)
except ValueError as error: assert 'differ' in str(error)
else: raise AssertionError('Changed data accepted')
parsed = wasm_object((folder/'fixture.o').read_bytes(),True)
assert any(s.get('name')=='LCRandom' and s.get('signature')==((),(0x7F,)) for s in parsed['symbols'])
try: emit(reference,{},set(),overlays,'missing')
except ValueError as error: assert 'Unknown relocation' in str(error)
else: raise AssertionError('Missing callback signature was accepted')
addend = copy.deepcopy(reference)
for section in addend['sections'].values():
    for relocation in section['relocations']:
        if relocation['symbol']['name']=='LCRandom': relocation['addend']=1
try: emit(addend,functions,set(),overlays,'addend')
except ValueError as error: assert 'Function-pointer addend' in str(error)
else: raise AssertionError('ARM function-address tag was silently retained')
for parser,payload in [(arm_object,b'\x7fELF'),(wasm_object,b'\0asm')]:
    try: parser(payload)
    except ValueError: pass
    else: raise AssertionError('Truncated object accepted')
for index,section in enumerate(['.section .sdk_fixture,16,9,3','.section .sdk_fixture,16,1,4']):
    invalid=folder/f'invalid-section-{index}.s'
    invalid.write_text(source.read_text().replace('.section .sdk_fixture,16,1,3',section))
    try: prepare_arm(invalid,folder/f'invalid-section-{index}.o',sdk,[])
    except ValueError as error: assert 'Unsupported section attributes' in str(error)
    else: raise AssertionError('Unsupported SDK section semantics accepted')
mixed=folder/'mixed-metadata.s'
mixed.write_text(source.read_text()+'''
.text
exception LCRandom, 0x31, HgArmUnwind
.section .exception,4,1,2
.public HgArmUnwind
HgArmUnwind:
.word 0xDEADBEEF
.section .after_unwind,8,1,3
.public HgAfterUnwind
.type HgAfterUnwind,@object
HgAfterUnwind:
.word HgAsmBytes+1
.word LCRandom
.size HgAfterUnwind,8
''')
try: prepare_arm(mixed,folder/'strict-mixed.o',sdk,[])
except ValueError as error: assert 'Non-data assembler statement: exception' in str(error)
else: raise AssertionError('Unported ARM exception metadata accepted by the strict converter')
excluded=[]
ordinary=prepare_arm(mixed,folder/'mixed.arm.o',sdk,[],excluded)
assert any('exception LCRandom' in line for line in excluded) and 'HgArmUnwind:' in excluded
assert all(s['name']!='.exception' for s in ordinary['sections'].values())
assembly,expected,_=emit(ordinary,functions,set(),overlays,'mixed_fixture')
native=folder/'mixed.s'; native.write_text(assembly)
result=subprocess.run([str(sdk/'bin/clang'),'-c',str(native),'-o',str(folder/'mixed.o')],capture_output=True,text=True)
assert result.returncode==0,result.stderr
assert 'HgAfterUnwind' in verify(folder/'mixed.o',expected)
assert 'HgArmUnwind' not in verify(folder/'mixed.o',expected)
report=build([folder/'fixture.o'],sdk,[],folder/'mixed-report',[mixed])
unit=report['units'][0]
assert report['summary']==dict(total=1,compiled=0,constantObjects=1,objects=1)
assert unit['exitCode']==1 and 'object' not in unit and Path(unit['constantsObject']).is_file()
assert 'HgAfterUnwind' in unit['symbols'] and unit['excludedArmExceptionMetadata']==excluded
mixed.write_text(mixed.read_text()+'\n.section .exception,4,1,2\nmov r0,r1\n')
report=build([folder/'fixture.o'],sdk,[],folder/'mixed-report',[mixed])
assert report['summary']['objects']==0 and report['units'][0]['exitCode']==1
assert not Path(unit['constantsObject']).exists()
invalid=folder/'invalid-exception.s'; invalid.write_text('.text\nexception LCRandom, 0x10000, 0\n')
try: prepare_arm(invalid,folder/'invalid-exception.o',sdk,[],[])
except ValueError as error: assert 'Invalid ARM exception record' in str(error)
else: raise AssertionError('Truncated ARM exception size accepted')
typed=folder/'external-function.s'; typed.write_text('.public ExternalFunction\n.type ExternalFunction,@function\n.text\n')
typed_arm=prepare_arm(typed,folder/'external-function.arm.o',sdk,[])
assert not typed_arm['sections']
typed.write_text(typed.read_text()+'ExternalFunction:\n.word 0xE12FFF1E\n')
try: prepare_arm(typed,folder/'defined-function.arm.o',sdk,[])
except ValueError as error: assert 'Executable ARM symbol' in str(error)
else: raise AssertionError('Defined ARM function accepted after type-dialect conversion')
print('PASS: ARM/Wasm bytes, aliases, BSS, relocations, partial constants with retained exception failures, stale-output removal and malformed-input rejection.')
