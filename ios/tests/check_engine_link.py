#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-or-later
"""Exercise the real final linker: exports, explicit native imports, failure and stale output removal."""
import json
from pathlib import Path
import subprocess
import sys
import tempfile

ROOT=Path(__file__).resolve().parents[2]
sys.path.insert(0,str(ROOT/'ios/tools'))
from check_abi32 import SDK,WABT,link_engine

with tempfile.TemporaryDirectory(prefix='heartgold-engine-link-') as temporary:
    folder=Path(temporary); source=folder/'input.c'; obj=folder/'input.o'; listing=folder/'objects.rsp'
    allowed=folder/'imports.txt'; allowed.write_text('HGNativeGetProcMode\n')
    listing.write_text(json.dumps(str(obj))+'\n')
    link=[SDK/'bin/clang','--target=wasm32-wasip1','-mexec-model=reactor','-Wl,--export-memory',
          '-Wl,--gc-sections','-Wl,--fatal-warnings','-Wl,--allow-undefined-file='+str(allowed)]
    for code,missing in [('void NitroMain(void) {}',[]),
                         ('extern unsigned HGNativeGetProcMode(void); void NitroMain(void) { HGNativeGetProcMode(); }',[]),
                         ('extern void engine_missing_for_check(void); void NitroMain(void) { engine_missing_for_check(); }',['engine_missing_for_check'])]:
        source.write_text(code+'\n')
        subprocess.run([str(SDK/'bin/clang'),'--target=wasm32-wasip1','-c',str(source),'-o',str(obj)],check=True)
        result=link_engine(link,listing,folder)
        assert bool(result['exitCode'])==bool(missing) and result['undefinedSymbols']==missing,result
        assert json.loads((folder/'engine-link.json').read_text())==result
        target=folder/'engine.wasm'
        if missing:
            assert not target.exists() and result['outputSha256'] is None
        else:
            assert target.read_bytes().startswith(b'\0asm') and result['outputSha256']
            exports=subprocess.check_output([str(WABT/'bin/wasm-objdump'),'-x',str(target)],text=True)
            assert '-> "NitroMain"' in exports
    source.write_text('extern void engine_type_collision(void); void NitroMain(void) { engine_type_collision(); }\n')
    subprocess.run([str(SDK/'bin/clang'),'--target=wasm32-wasip1','-c',str(source),'-o',str(obj)],check=True)
    data=folder/'data.c'; data.write_text('int engine_type_collision;\n'); data_obj=folder/'data.o'
    subprocess.run([str(SDK/'bin/clang'),'--target=wasm32-wasip1','-c',str(data),'-o',str(data_obj)],check=True)
    listing.write_text(json.dumps(str(obj))+'\n'+json.dumps(str(data_obj))+'\n')
    result=link_engine(link,listing,folder)
    assert result['exitCode'] and not result['undefinedSymbols'] and result['outputSha256'] is None
    assert 'symbol type mismatch:' in (folder/'engine-link.log').read_text() and not target.exists()
    print('PASS: strict engine link exports NitroMain, allows explicit imports, rejects missing code/type collisions and removes stale output.')
