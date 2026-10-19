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
from prepare_msl_native import legacy_flags
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


flags=['--target=wasm32-wasip1','-O2','-fwasm-exceptions','-fexceptions','-fno-builtin-malloc','-fno-builtin-free']
original=ROOT/'src/to91_msl_scalar_new.cpp'
run([SDK/'bin/clang++',*flags,'-std=gnu++98','-DHEARTGOLD_NATIVE','-DHEARTGOLD_ABI32',
     '-I',ROOT/'include','-I',ROOT/'lib/include','-Dmalloc=CppExceptionCheckMalloc','-c',original,'-o',BUILD/'original-new.o'],'original-new.log')
run([SDK/'bin/clang',*flags,'-std=c11','-c',ROOT/'ios/Original/abi32/cpp_exception_frame.c','-o',BUILD/'frame.o'],'frame.log')
allocator=ROOT/'lib/MSL_C/src/msl_alloc.c'
run([SDK/'bin/clang',*flags,'-DHEARTGOLD_NATIVE','-DHEARTGOLD_ABI32','-I',ROOT/'lib/include',
     '-c',allocator,'-o',BUILD/'allocator.o'],'allocator.log')
run([SDK/'bin/clang++',*flags,'-std=c++17','-c',ROOT/'ios/Original/abi32/cpp_exception_check.cpp','-o',BUILD/'check.o'],'check-compile.log')
legacy_sources=['lib/MSL_C/src/msl_shared_ptr_create.c','lib/MSL_C/src/msl_shared_ptr_destroy.c','lib/MSL_C/src/msl_shared_ptr_release_weak.c',
    'lib/MSL_C/src/msl_shared_ptr_release.cpp','src/to91_msl_shared_ptr_char_d1.cpp',
    'ios/Original/abi32/cpp_legacy_message_check.cpp','ios/Original/abi32/cpp_legacy_vtables.c']
legacy_objects=[]
observer=['-D_Znwm=CppLegacyControlNew','-D_ZdlPv=CppLegacyControlDelete','-D_ZdaPv=CppLegacyArrayDelete']
for source in legacy_sources:
    target=BUILD/(Path(source).stem+'.o'); legacy_objects.append(target)
    cpp=source.endswith('.cpp')
    run([SDK/('bin/clang++' if cpp else 'bin/clang'),*flags,'-std=gnu++98' if cpp else '-std=gnu11',
        '-DHEARTGOLD_NATIVE','-DHEARTGOLD_ABI32','-I',ROOT/'include','-I',ROOT/'lib/include',
        *legacy_flags(source),*observer,'-c',ROOT/source,'-o',target],Path(source).stem+'.log')
run([SDK/'bin/clang++',*flags,'-mexec-model=reactor','-Wl,--export=CheckCppExceptionRuntime','-Wl,--export=ExitCppExceptionRuntime',
     '-Wl,--wrap=malloc','-Wl,--wrap=free',
     '-Wl,--export=RecurseCppExceptionRuntime',
     '-Wl,--export-memory','-Wl,--stack-first','-Wl,-z,stack-size=65536','-Wl,--initial-memory=16777216','-Wl,--max-memory=16777216',
     BUILD/'original-new.o',BUILD/'frame.o',BUILD/'allocator.o',BUILD/'check.o',*legacy_objects,'-lunwind','-o',BUILD/'CppExceptionRuntime.wasm'],'link.log')
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
tracked=[original,allocator,ROOT/'ios/Original/abi32/cpp_exception_check.cpp',ROOT/'ios/Original/abi32/cpp_exception_frame.c',
    ROOT/'ios/Original/abi32/cpp_exception_host.c',ROOT/'ios/Original/abi32/NativeWASIImports.c',
    ROOT/'ios/Original/abi32/NativeWASIImports.h',ROOT/'ios/tools/prepare_exception_unwind.py',Path(__file__)]
tracked += [ROOT/source for source in legacy_sources]+[ROOT/'lib/MSL_C/src/msl_native_error_views.hpp',
    ROOT/'lib/MSL_C/src/msl_shared_ptr_internal.h',ROOT/'lib/MSL_C/src/msl_vector_length_error.cpp',ROOT/'ios/tools/prepare_msl_native.py']
(BUILD/'report.json').write_text(json.dumps(dict(scope='SDK EH runtime, original allocator and original 8/12-byte message ownership; full gameplay not validated',
    sources={str(p.relative_to(ROOT)):hashlib.sha256(p.read_bytes()).hexdigest() for p in tracked},
    runtimeLibraries={str(p.relative_to(SDK)):hashlib.sha256(p.read_bytes()).hexdigest() for p in
        [SDK/'share/wasi-sysroot/lib/wasm32-wasip1/eh/libc++abi.a',SDK/'share/wasi-sysroot/lib/wasm32-wasip1/eh/libunwind.a']},
    nativeCatchDepthRestores=depth_restores,
    limitations=['Encoded legacy RTTI is isolated from native type views; checked exception specifications, SDK fibre exception state and gameplay remain unvalidated.'],
    nativeArchiveSha256=hashlib.sha256(archive.read_bytes()).hexdigest()),indent=2)+'\n')
print('iPhoneOS native exception probe: '+str(archive))
