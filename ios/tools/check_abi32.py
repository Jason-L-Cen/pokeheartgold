# SPDX-License-Identifier: GPL-3.0-or-later
"""Check original 32-bit layouts through build-time C generation, never a runtime VM."""
import os
import argparse
from concurrent.futures import ThreadPoolExecutor
import hashlib
import json
from pathlib import Path
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[2]
BUILD = ROOT / 'ios/build-original-abi32'
TOOLS = Path(os.environ.get('HG_ABI32_TOOLS', ROOT / 'ios/build-original-abi32/tools'))
SDK = TOOLS / 'wasi-sdk-34.0-arm64-macos'
WABT = TOOLS / 'wabt-1.0.42'
BUILD.mkdir(parents=True, exist_ok=True)


def prepare_tools():
    # shortcut: bootstrap packages target Apple Silicon macOS; add pinned host packages for other build hosts.
    packages = [
        ('wabt-1.0.42-macos-arm64.tar.gz', '3f654779b436c628db3ca4323c51538815f5fbc887a6d5f3bb1c18d36281e240',
         'https://github.com/WebAssembly/wabt/releases/download/1.0.42/', WABT / 'bin/wasm2c'),
        ('wasi-sdk-34.0-arm64-macos.tar.gz', '9c59398106b417f8f14913380fdf0097a8cc0ff4af9eb3ce0065a859e88d49e9',
         'https://github.com/WebAssembly/wasi-sdk/releases/download/wasi-sdk-34/', SDK / 'bin/clang'),
    ]
    TOOLS.mkdir(parents=True, exist_ok=True)
    for name, expected, url, binary in packages:
        archive = TOOLS / name
        if not archive.is_file():
            temporary = archive.with_suffix('.download')
            run(['curl','--fail','--location','--silent','--show-error',url+name,'-o',temporary],name+'.download.log')
            if hashlib.sha256(temporary.read_bytes()).hexdigest() != expected:
                raise RuntimeError(f'Tool download checksum mismatch: {name}')
            temporary.replace(archive)
        if hashlib.sha256(archive.read_bytes()).hexdigest() != expected:
            raise RuntimeError(f'Tool archive checksum mismatch: {archive}')
        if not binary.is_file(): run(['tar','-xzf',archive,'-C',TOOLS],name+'.extract.log')


def audit(flags, includes):
    destination = BUILD / 'audit'; destination.mkdir(exist_ok=True)
    headers = sorted({p for root in includes for pattern in ('**/*.h','**/*.naix') for p in root.glob(pattern)})
    def header_hash():
        value = hashlib.sha256()
        for path in headers: value.update(str(path.relative_to(ROOT)).encode()); value.update(path.read_bytes())
        return value.hexdigest()
    before = header_hash()
    sources = sorted([*ROOT.glob('src/**/*.c'), *ROOT.glob('src/**/*.cpp')])
    def compile_source(source):
        relative = source.relative_to(ROOT)
        target = destination / 'objects' / relative.with_suffix('.o'); target.parent.mkdir(parents=True,exist_ok=True)
        log = destination / 'logs' / relative.with_suffix('.log'); log.parent.mkdir(parents=True,exist_ok=True)
        cpp = source.suffix == '.cpp'
        options = ['-std=gnu++98' if option == '-std=gnu11' and cpp else option for option in flags]
        result = subprocess.run([str(SDK / ('bin/clang++' if cpp else 'bin/clang')), *map(str,options), '-c', str(source), '-o', str(target)],
                                cwd=ROOT,capture_output=True,text=True)
        log.write_text(result.stdout+result.stderr)
        if result.returncode: target.unlink(missing_ok=True)
        return dict(file=str(relative),sha256=hashlib.sha256(source.read_bytes()).hexdigest(),exitCode=result.returncode,
                    log=str(log.relative_to(ROOT)))
    with ThreadPoolExecutor(max_workers=4) as pool: units=list(pool.map(compile_source,sources))
    if header_hash() != before: raise RuntimeError('Headers changed during ABI32 compilation')
    compiled = sum(unit['exitCode']==0 for unit in units)
    report = dict(scope='ABI32 intermediate objects only; not full iOS linking or gameplay', target='wasm32-wasip1',
                  headersSha256=before, summary=dict(total=len(units),compiled=compiled,blocked=len(units)-compiled),units=units)
    (destination / 'report.json').write_text(json.dumps(report,indent=2)+'\n')
    print(f'ABI32 intermediate object audit: {compiled} compiled, {len(units)-compiled} blocked, {len(units)} total.')
    return 1 if compiled != len(units) else 0


def run(args, log):
    result = subprocess.run([str(arg) for arg in args], cwd=ROOT, capture_output=True, text=True)
    (BUILD / log).write_text(result.stdout + result.stderr)
    if result.returncode:
        errors = [line for line in result.stderr.splitlines() if any(word in line for word in ['error:','undefined','check line','Layout field'])]
        raise RuntimeError(f'{args[0]} failed; see {BUILD / log}\n' + '\n'.join(errors[:8]))
    return result.stdout


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--audit',action='store_true',help='Also compile all game units for the 32-bit intermediate target')
    args = parser.parse_args()
    if args.audit: (BUILD / 'audit/report.json').unlink(missing_ok=True)
    run(['make','-s','-C',ROOT / 'tools/gen_fx_consts'], 'constants-tool.log')
    # The generator derives its include guard from this argument; match the existing build path.
    run([ROOT / 'tools/gen_fx_consts/gen_fx_consts','lib/include/nitro/fx/fx_const.h'], 'constants.log')
    run([sys.executable,ROOT / 'ios/tools/prepare_original_headers.py'], 'headers.log')
    prepare_tools()
    includes = [ROOT / 'ios/build-original-host/include', ROOT / 'include', ROOT / 'files', ROOT / 'src',
                ROOT / 'lib/include', ROOT / 'lib/include/cw', ROOT / 'lib/include/nitro',
                ROOT / 'lib/include/nnsys', ROOT / 'lib/include/nitro/os', ROOT / 'include/library', ROOT / 'lib/NitroSDK/include']
    flags = ['--target=wasm32-wasip1', '-std=gnu11', '-O2', '-fwrapv', '-fno-strict-aliasing',
             '-fms-extensions', '-Wno-microsoft', '-Wno-unknown-pragmas', '-ferror-limit=5',
             '-DHEARTGOLD_NATIVE', '-DHEARTGOLD_ABI32', '-DSDK_ARM9', '-DSDK_CW', '-DSDK_CODE_ARM', '-DSDK_TS', '-D_NITRO',
             '-DPM_KEEP_ASSERTS', '-DPLATFORM_INTRINSIC_FUNCTION_BIT_CLZ32=__builtin_clz']
    for include in includes: flags += ['-I', include]
    sources = ['src/math_util.c', 'src/pokemon.c', 'ios/Original/abi32/probe.c']
    objects = []
    for source in sources:
        target = BUILD / (Path(source).stem + '.o'); objects.append(target)
        run([SDK / 'bin/clang', *flags, '-c', ROOT / source, '-o', target], Path(source).stem + '.log')
    exports = ['HGAotLayout','HGAotPointerCheck','HGAotString','HGAotIterator','HGAotData','HGAotEncrypt','HGAotDecrypt','HGAotChecksum',
               'HGAotSeed','HGAotRandom','HGAotMTSeed','HGAotMTRandom']
    link = [SDK / 'bin/clang', '--target=wasm32-wasip1', '-nostartfiles', '-Wl,--no-entry', '-Wl,--export-memory',
            '-Wl,--gc-sections', '-Wl,--initial-memory=2097152', '-Wl,--max-memory=2097152']
    run([*link, *['-Wl,--export=' + name for name in exports], *objects, '-o', BUILD / 'hg32.wasm'], 'link.log')
    run([WABT / 'bin/wasm2c', BUILD / 'hg32.wasm', '-n', 'hg32', '-o', BUILD / 'hg32.c'], 'wasm2c.log')
    runtime = WABT / 'share/wabt/wasm2c'
    native = ['-std=c11', '-O2', '-fno-optimize-sibling-calls', '-frounding-math',
              '-DWASM_RT_USE_MMAP=0', '-DWASM_RT_MEMCHECK_GUARD_PAGES=0', '-DWASM_RT_MEMCHECK_BOUNDS_CHECK=1',
              '-I', BUILD, '-I', WABT / 'include', '-I', runtime]
    native_sources = [BUILD / 'hg32.c', runtime / 'wasm-rt-impl.c', runtime / 'wasm-rt-mem-impl.c', runtime / 'wasm-rt-exceptions-impl.c']
    run(['xcrun', 'clang', *native, '-fsanitize=address,undefined', '-fno-sanitize-recover=all',
         ROOT / 'ios/Original/abi32/check.c', *native_sources, '-o', BUILD / 'abi32-check'], 'native.log')
    print(run([BUILD / 'abi32-check'], 'check.log').strip())
    device = BUILD / 'iphoneos'; device.mkdir(exist_ok=True)
    device_objects = []
    for source in native_sources:
        target = device / (source.stem + '.o'); device_objects.append(target)
        run(['xcrun', '--sdk', 'iphoneos', 'clang', '-target', 'arm64-apple-ios16.0', *native,
             '-c', source, '-o', target], source.stem + '-iphoneos.log')
    (device / 'libHeartGoldABI32Probe.a').unlink(missing_ok=True)
    run(['xcrun', 'ar', 'rcs', device / 'libHeartGoldABI32Probe.a', *device_objects], 'archive.log')
    print(f'iPhoneOS native C archive: {device / "libHeartGoldABI32Probe.a"}')
    if args.audit: return audit(flags, includes)


if __name__ == '__main__':
    raise SystemExit(main())
