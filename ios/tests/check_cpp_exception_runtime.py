# SPDX-License-Identifier: GPL-3.0-or-later
"""Run SDK exception handling as generated native C before mixing legacy MSL RTTI."""
from pathlib import Path
import hashlib
import json
import os
import subprocess
import sys

ROOT=Path(__file__).resolve().parents[2]
sys.path.insert(0,str(ROOT/'ios/tools'))
from prepare_exception_unwind import restore_exception_depth
TOOLS=Path(os.environ.get('HG_ABI32_TOOLS',ROOT/'ios/build-original-abi32/tools'))
SDK=TOOLS/'wasi-sdk-34.0-arm64-macos'
WABT=TOOLS/'wabt-1.0.42'
BUILD=ROOT/'ios/build-original-abi32/cpp-exceptions'
BUILD.mkdir(parents=True,exist_ok=True)
for stale in [BUILD/'report.json',BUILD/'cpp-exception-check',BUILD/'iphoneos/libHeartGoldCppExceptionProbe.a']:
    stale.unlink(missing_ok=True)


def run(command,log):
    result=subprocess.run(list(map(str,command)),cwd=ROOT,capture_output=True,text=True)
    (BUILD/log).write_text(result.stdout+result.stderr)
    if result.returncode: raise RuntimeError(str(command[0])+' failed; see '+str(BUILD/log)+'\n'+result.stderr[-2000:])
    return result.stdout


flags=['--target=wasm32-wasip1','-O2','-fwasm-exceptions','-fexceptions']
original=ROOT/'src/to91_msl_scalar_new.cpp'
run([SDK/'bin/clang++',*flags,'-std=gnu++98','-DHEARTGOLD_NATIVE','-DHEARTGOLD_ABI32',
     '-I',ROOT/'include','-I',ROOT/'lib/include','-Dmalloc=CppExceptionCheckMalloc','-c',original,'-o',BUILD/'original-new.o'],'original-new.log')
run([SDK/'bin/clang',*flags,'-std=c11','-c',ROOT/'ios/Original/abi32/cpp_exception_frame.c','-o',BUILD/'frame.o'],'frame.log')
run([SDK/'bin/clang++',*flags,'-std=c++17','-c',ROOT/'ios/Original/abi32/cpp_exception_check.cpp','-o',BUILD/'check.o'],'check-compile.log')
run([SDK/'bin/clang++',*flags,'-mexec-model=reactor','-Wl,--export=CheckCppExceptionRuntime','-Wl,--export=ExitCppExceptionRuntime',
     '-Wl,--export=RecurseCppExceptionRuntime',
     '-Wl,--export-memory','-Wl,--stack-first','-Wl,-z,stack-size=65536','-Wl,--initial-memory=16777216','-Wl,--max-memory=16777216',
     BUILD/'original-new.o',BUILD/'frame.o',BUILD/'check.o','-lunwind','-o',BUILD/'CppExceptionRuntime.wasm'],'link.log')
run([WABT/'bin/wasm2c',BUILD/'CppExceptionRuntime.wasm','-n','cxxexceptions','-o',BUILD/'CppExceptionRuntime.c'],'wasm2c.log')
generated,depth_restores=restore_exception_depth((BUILD/'CppExceptionRuntime.c').read_text())
if not depth_restores: raise RuntimeError('Exception profile produced no native catch targets')
(BUILD/'CppExceptionRuntime.c').write_text(generated)
runtime=WABT/'share/wabt/wasm2c'
native=['-std=c11','-O2','-DWASM_RT_USE_MMAP=0','-DWASM_RT_MEMCHECK_GUARD_PAGES=0','-DWASM_RT_MEMCHECK_BOUNDS_CHECK=1',
        '-I',BUILD,'-I',ROOT/'ios/Original/abi32','-I',WABT/'include','-I',runtime]
sources=[BUILD/'CppExceptionRuntime.c',ROOT/'ios/Original/abi32/NativeWASIImports.c',
         *[runtime/name for name in ['wasm-rt-impl.c','wasm-rt-mem-impl.c','wasm-rt-exceptions-impl.c']]]
run(['xcrun','clang',*native,'-fsanitize=address,undefined','-fno-sanitize-recover=all',
     ROOT/'ios/Original/abi32/cpp_exception_host.c',*sources,'-o',BUILD/'cpp-exception-check'],'native.log')
print(run([BUILD/'cpp-exception-check'],'check.log').strip())
device=BUILD/'iphoneos'; device.mkdir(exist_ok=True); objects=[]
for source in sources:
    target=device/(source.stem+'.o'); objects.append(target)
    run(['xcrun','--sdk','iphoneos','clang','-target','arm64-apple-ios16.0',*native,'-c',source,'-o',target],source.stem+'-iphoneos.log')
archive=device/'libHeartGoldCppExceptionProbe.a'; archive.unlink(missing_ok=True)
run(['xcrun','ar','rcs',archive,*objects],'archive.log')
tracked=[original,ROOT/'ios/Original/abi32/cpp_exception_check.cpp',ROOT/'ios/Original/abi32/cpp_exception_frame.c',
    ROOT/'ios/Original/abi32/cpp_exception_host.c',ROOT/'ios/Original/abi32/NativeWASIImports.c',
    ROOT/'ios/Original/abi32/NativeWASIImports.h',ROOT/'ios/tools/prepare_exception_unwind.py',Path(__file__)]
(BUILD/'report.json').write_text(json.dumps(dict(scope='isolated SDK EH runtime and original allocator; legacy MSL RTTI/game integration not validated',
    sources={str(p.relative_to(ROOT)):hashlib.sha256(p.read_bytes()).hexdigest() for p in tracked},
    runtimeLibraries={str(p.relative_to(SDK)):hashlib.sha256(p.read_bytes()).hexdigest() for p in
        [SDK/'share/wasi-sysroot/lib/wasm32-wasip1/eh/libc++abi.a',SDK/'share/wasi-sysroot/lib/wasm32-wasip1/eh/libunwind.a']},
    nativeCatchDepthRestores=depth_restores,
    limitations=['Legacy MSL RTTI/layout interoperation, checked exception specifications and SDK fibre exception state remain unvalidated.'],
    nativeArchiveSha256=hashlib.sha256(archive.read_bytes()).hexdigest()),indent=2)+'\n')
print('iPhoneOS native exception probe: '+str(archive))
