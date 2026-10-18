# SPDX-License-Identifier: GPL-3.0-or-later
"""Compile every original game C/C++ unit for iPhoneOS; report gaps, not game completion."""
import argparse
from collections import Counter
from concurrent.futures import ThreadPoolExecutor, as_completed
import hashlib
import json
from pathlib import Path
import re
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[2]
DIAGNOSTIC = re.compile(r'^(.*?):(\d+):(\d+): (fatal error|error|warning): (.*)$', re.MULTILINE)


def digest(paths):
    result = hashlib.sha256()
    for path in sorted(paths):
        result.update(str(path.relative_to(ROOT)).encode())
        result.update(path.read_bytes())
    return result.hexdigest()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--jobs', type=int, default=4)
    args = parser.parse_args()
    if not 1 <= args.jobs <= 16:
        parser.error('--jobs must be between 1 and 16')
    build = ROOT / 'ios/build-original-audit'
    build.mkdir(parents=True, exist_ok=True)
    generated = ROOT / 'ios/build-original-host/include'
    if not (generated / 'msgdata/msg.naix').exists():
        parser.error('Run prepare_original_headers.py first')
    sdk = subprocess.check_output(['xcrun', '--sdk', 'iphoneos', '--show-sdk-path'], text=True).strip()
    version = subprocess.check_output(['xcrun', 'clang', '--version'], text=True).splitlines()[0]
    headers = sorted([*ROOT.glob('include/**/*.h'), *ROOT.glob('lib/include/**/*.h'), *generated.rglob('*.h'), *generated.rglob('*.naix')])
    header_hash = digest(headers)
    sources = sorted([*ROOT.glob('src/**/*.c'), *ROOT.glob('src/**/*.cpp')])
    common = ['-target', 'arm64-apple-ios16.0', '-isysroot', sdk, '-O0', '-c', '-ferror-limit=3',
              '-fwrapv', '-fno-strict-aliasing', '-fms-extensions', '-Wno-microsoft', '-Wno-unknown-pragmas',
              '-DHEARTGOLD_NATIVE', '-DSDK_ARM9', '-DSDK_CW', '-DSDK_CODE_ARM', '-DSDK_TS', '-D_NITRO',
              '-DPLATFORM_INTRINSIC_FUNCTION_BIT_CLZ32=__builtin_clz', '-DPM_KEEP_ASSERTS']
    for directory in [generated, ROOT / 'include', ROOT / 'files', ROOT / 'lib/include', ROOT / 'lib/include/cw',
                      ROOT / 'lib/include/nitro', ROOT / 'lib/include/nnsys', ROOT / 'lib/include/nitro/os', ROOT / 'include/library', ROOT / 'lib/NitroSDK/include']:
        common += ['-I', str(directory)]

    def compile_unit(source):
        relative = source.relative_to(ROOT)
        output = build / 'objects' / relative.with_suffix('.o')
        log = build / 'logs' / relative.with_suffix('.log')
        output.parent.mkdir(parents=True, exist_ok=True); log.parent.mkdir(parents=True, exist_ok=True)
        cpp = source.suffix == '.cpp'
        command = ['xcrun', 'clang++' if cpp else 'clang', '-std=gnu++98' if cpp else '-std=gnu11', *common, str(source), '-o', str(output)]
        result = subprocess.run(command, cwd=ROOT, capture_output=True, text=True)
        log.write_text(result.stdout + result.stderr)
        if result.returncode:
            output.unlink(missing_ok=True)
        diagnostics = [dict(file=path, line=int(line), column=int(column), severity=severity, message=message)
                       for path, line, column, severity, message in DIAGNOSTIC.findall(result.stderr)]
        return dict(file=str(relative), sha256=hashlib.sha256(source.read_bytes()).hexdigest(),
                    status='compiled' if result.returncode == 0 else 'blocked', exitCode=result.returncode,
                    diagnostics=diagnostics, log=str(log.relative_to(ROOT)))

    results = []
    with ThreadPoolExecutor(max_workers=args.jobs) as pool:
        pending = {pool.submit(compile_unit, source): source for source in sources}
        for future in as_completed(pending):
            results.append(future.result())
            if len(results) % 100 == 0:
                print(f'Native compilation checked {len(results)} / {len(sources)} units.', flush=True)
    if digest(headers) != header_hash:
        raise RuntimeError('Headers changed during the audit; report is not authoritative')
    counts = Counter(result['status'] for result in results)
    report = dict(scope='original game translation units; this does not prove linking, rendering or playability',
                  target='arm64-apple-ios16.0', compiler=version, sdk=sdk, headersSha256=header_hash,
                  summary=dict(total=len(sources), compiled=counts['compiled'], blocked=counts['blocked']),
                  assemblySources=[str(path.relative_to(ROOT)) for path in sorted(ROOT.glob('asm/*.s'))],
                  units=sorted(results, key=lambda result: result['file']))
    destination = build / 'report.json'
    destination.write_text(json.dumps(report, ensure_ascii=False, indent=2) + '\n')
    print(f'iPhoneOS compile audit: {counts["compiled"]} compiled, {counts["blocked"]} blocked, {len(sources)} total.')
    print(destination)
    return 1 if counts['blocked'] else 0


if __name__ == '__main__':
    sys.exit(main())
