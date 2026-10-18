# SPDX-License-Identifier: GPL-3.0-or-later
"""Partition the lobby's existing typed namespace views while keeping shared data once."""
import json
from pathlib import Path
import re
import subprocess

ROOT = Path(__file__).resolve().parents[2]


def prepare(output):
    source = ROOT / 'src/ov45_02232e60.cpp'
    text = source.read_text()
    starts, depth = [], 0
    for match in re.finditer(r'^#pragma (push|pop)\s*$',text,re.MULTILINE):
        if match[1] == 'push':
            if depth == 0: starts.append(match.start())
            depth += 1
        else:
            depth -= 1
            if depth < 0: raise ValueError('Unbalanced lobby pragma scopes')
    if not starts or depth: raise ValueError('Lobby scope registry is missing or unbalanced')
    prefix = text[:starts[0]]
    # Each namespace already carries its own reconstructed wire types and C declarations.
    prefix = prefix.replace('#include "overlay45_format_buffer_private.h"','').replace('#include "overlay45_string_search.h"','')
    prefix = re.sub(r'#include "(\.\./[^"\n]+)"',lambda match:'#include '+json.dumps(str((source.parent/match[1]).resolve())),prefix)
    declarations = re.sub(r'^(extern "C" [^\n;]+?)\s*=\s*[^\n;]+;',r'\1;',prefix,flags=re.MULTILINE)
    output = Path(output); output.mkdir(parents=True,exist_ok=True)
    data = output / 'lobby-data.cpp'; data.write_text(prefix)
    result = [data]
    used = set()
    for index,start in enumerate(starts):
        end = starts[index+1] if index+1 < len(starts) else len(text)
        body = text[start:end]
        # Keep the one-byte C carrier arguments present; the Wasm C++ ABI omits empty value arguments.
        body = re.sub(r'\b((?:class|struct)\s+\w+\s*)\{\s*\}',r'\1{u8 hgAbiCarrier;}',body)
        namespace = re.search(r'^namespace (F\d+) \{',body,re.MULTILINE)
        name = namespace[1] if namespace else f'global-{index:02d}'
        if name in used: raise ValueError(f'Duplicate lobby scope: {name}')
        used.add(name)
        path = output / (name+'.cpp')
        line = text.count('\n',0,start)+1
        # Use the actual C library's declarations instead of duplicated 32-bit spellings.
        body = re.sub(r'extern "C" (?:unsigned|int|void\s*\*)\s*(?:strlen|memcmp|memset|memcpy|memmove)\s*\([^;{}]*\)\s*;', '', body)
        shared = '#include "overlay45_format_buffer_private.h"\n'
        if name in ('F05','F08','F18'):
            shared = '#define HG_LOBBY_NO_RANGE_DECL\n'+shared+'#undef HG_LOBBY_NO_RANGE_DECL\n'
        elif namespace:
            shared = '#define HG_LOBBY_TYPES_ONLY\n'+shared+'#undef HG_LOBBY_TYPES_ONLY\n'
        if re.search(r'\bStringSearch(?:Range)?\b',body): shared += '#include "overlay45_string_search.h"\n'
        path.write_text(declarations+'\n'+shared+'\n#line '+str(line)+' '+json.dumps(str(source))+'\n'+body)
        result.append(path)
    return result


def build(output, sdk, flags, target):
    output = Path(output); target = Path(target)
    objects, diagnostics = [], []
    for source in prepare(output):
        obj = source.with_suffix('.o')
        result = subprocess.run([str(sdk / 'bin/clang++'),*map(str,flags),'-c',str(source),'-o',str(obj)],
                                cwd=ROOT,capture_output=True,text=True)
        source.with_suffix('.log').write_text(result.stdout+result.stderr)
        diagnostics.append(result.stdout+result.stderr)
        if result.returncode:
            obj.unlink(missing_ok=True); target.unlink(missing_ok=True)
            return result.returncode,''.join(diagnostics)
        objects.append(obj)
    result = subprocess.run([str(sdk / 'bin/wasm-ld'),'--relocatable','--fatal-warnings',*map(str,objects),'-o',str(target)],
                            cwd=ROOT,capture_output=True,text=True)
    diagnostics.append(result.stdout+result.stderr)
    (output / 'link.log').write_text(result.stdout+result.stderr)
    if result.returncode: target.unlink(missing_ok=True)
    else:
        archive = target.with_suffix('.a'); archive.unlink(missing_ok=True)
        result = subprocess.run([str(sdk / 'bin/llvm-ar'),'rcs',str(archive),*map(str,objects)],
                                cwd=ROOT,capture_output=True,text=True)
        diagnostics.append(result.stdout+result.stderr)
    return result.returncode,''.join(diagnostics)


if __name__ == '__main__':
    print(f'Prepared {len(prepare(ROOT / "ios/build-original-abi32/lobby-parts"))} lobby data/namespace source files.')
