# SPDX-License-Identifier: GPL-3.0-or-later
"""Run native alias/shared-state and dispatch-tag regression checks."""
from pathlib import Path
import subprocess
import sys

ROOT=Path(__file__).resolve().parents[2]
sys.path.insert(0,str(ROOT/'ios/tools'))
from check_abi32 import SDK,WABT
from prepare_lobby_parts import native_views as prepare
OUT=ROOT/'ios/build-original-abi32/lobby-views-check'

def self_check(sdk,wabt):
    source='''typedef unsigned char u8;
struct Real { int value; };
extern "C" {
Real shared={40};
int step(Real *p) { return ++p->value; }
struct CTag {u8 value;};
int tag(int value,CTag) {return value+1;}
}
namespace Alpha {
struct View {int value;};
struct Tag {};
extern "C" View shared;
extern "C" int step(View *);
extern "C" int tag(int,Tag);
extern "C" int Alpha() {return tag(step(&shared),Tag());}
static const char protocol[]="step shared Alpha::Alpha struct Empty {}";
}
namespace Beta {
extern "C" {
struct View {int value;};
extern View shared;
int step(View *);
int Beta() {return step(&shared);}
}
}
extern "C" int CheckViews() {return Alpha::Alpha()==42 && Beta::Beta()==42 && shared.value==42;}
'''
    code,_=prepare(source);assert '"step shared Alpha::Alpha struct Empty {}"' in code
    try:prepare('namespace Bad {\nextern "C" void first(), second();\n}')
    except ValueError:pass
    else:raise AssertionError('Combined declarations must be rejected')
    def run(command,name,input=None):
        r=subprocess.run(list(map(str,command)),cwd=ROOT,input=input,capture_output=True,text=True)
        (OUT/name).write_text(r.stdout+r.stderr)
        if r.returncode:raise RuntimeError(str(OUT/name))
    run([sdk/'bin/clang++','--target=wasm32-wasip1','-std=gnu++98','-nostdlib','-Wl,--no-entry','-Wl,--export=CheckViews','-Wl,--export-memory','-x','c++','-','-o',OUT/'views.wasm'],'self-wasm.log',code)
    run([wabt/'bin/wasm2c',OUT/'views.wasm','-n','views','-o',OUT/'views.c'],'self-generate.log')
    from check_abi32 import native_build_settings
    flags,runtime=native_build_settings(OUT)
    flags=['xcrun','clang',*flags,'-O1','-g','-fsanitize=address,undefined','-fno-sanitize-recover=all']
    main='#include "views.h"\n#include <assert.h>\nint main(void){wasm_rt_init();w2c_views v;wasm2c_views_instantiate(&v);assert(w2c_views_CheckViews(&v)==1);wasm2c_views_free(&v);wasm_rt_free();}\n'
    (OUT/'self-main.c').write_text(main)
    run([*flags,OUT/'views.c',OUT/'self-main.c',*[p for p in runtime if p.name in ('wasm-rt-impl.c','wasm-rt-mem-impl.c')],'-o',OUT/'self-check'],'self-native.log')
    run([OUT/'self-check'],'self-runtime.log')
    print('PASS: native shared-state/function aliases, byte dispatch tag, C linkage blocks and unchanged protocol text')

if __name__=='__main__':
    OUT.mkdir(parents=True,exist_ok=True)
    self_check(SDK,WABT)
