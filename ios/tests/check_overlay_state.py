# SPDX-License-Identifier: GPL-3.0-or-later
"""Small parser check; check_abi32.py runs the generated state records as native C."""
from pathlib import Path
import sys
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'tools'))
from prepare_overlay_state import overlay_objects,instrument_ir,llvm_type,first_value,getter_name


def rejects(function,*args):
    try:
        function(*args)
    except ValueError:
        return
    raise AssertionError('Malformed overlay metadata was accepted')


root=Path(__file__).resolve().parents[2]
registry=overlay_objects((root/'main.lsf').read_text())
assert len(registry)==129 and registry[0]['name']=='OVY_0'
assert registry[1]['name']=='field' and registry[123]['name']=='ds_protect' and registry[-1]['name']=='OVY_128'
assert 'src/wifi_connection.o' in registry[0]['objects']
rejects(overlay_objects,'Overlay OVY_1\n{\n Object a.o\n}')
rejects(overlay_objects,'Overlay A\n{\n Object a.o\n}\nOverlay B\n{\n Object a.o\n}')
assert llvm_type('<{ i32, [2 x ptr] }> zeroinitializer, align 4')[0]=='<{ i32, [2 x ptr] }>'
assert first_value('{ i32 7, ptr @x }, align 4')=='{ i32 7, ptr @x }'
prefix='target triple = "wasm32-wasip1"\n'
source=prefix+'@x = internal global i32 5, align 4\n@y = internal global [7 x i8] zeroinitializer, align 1\n@z = external global i32\ndeclare ptr @HGOverlayUnit_external()\n'
output,report=instrument_ir(source,'unit')
assert report['globals']==['@x','@y'] and report['constructors']==0
assert '_image_0 = private constant i32 5' in output and '_image_1' not in output
assert source.splitlines()[1] in output
own_getter=getter_name('unit')
with_declaration,_=instrument_ir(source+'declare ptr @'+own_getter+'()\n','unit')
assert 'declare ptr @'+own_getter not in with_declaration
rejects(instrument_ir,source+'declare i32 @'+own_getter+'()\n','unit')
ties=prefix+'@llvm.global_dtors = appending global [2 x { i32, ptr, ptr }] [{ i32, ptr, ptr } { i32 65535, ptr @first, ptr null }, { i32, ptr, ptr } { i32 65535, ptr @second, ptr null }]\n'
result,record=instrument_ir(ties,'dtors')
assert record['destructors']==2 and result.index('ptr @second')<result.index('ptr @first')
rejects(instrument_ir,output,'unit')
for declaration in ['thread_local global i32 0','weak global i32 0','global i32 poison','global ptr addrspace(1) null']:
    rejects(instrument_ir,prefix+'@x = '+declaration+'\n','bad')
rejects(instrument_ir,'target triple = "arm64-apple-ios16.0"\n','wrong')
print('PASS: 129 original overlay ownership records, LLVM types/initializers, deterministic images and rejected ambiguous state.')
