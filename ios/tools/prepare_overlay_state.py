# SPDX-License-Identifier: GPL-3.0-or-later
"""Describe compiled overlay state without loading or executing ARM images."""
import hashlib
import re
import subprocess
from pathlib import Path

IDENT = r'(?:"[^"\n]+"|[-a-zA-Z$._0-9]+)'
NATIVE_OVERLAY_REPLACEMENTS = {'ds_protect':['ios/Original/NativeDSProtect.o']}


def overlay_objects(specification):
    overlays = []
    owners = {}
    for match in re.finditer(r'(?m)^Overlay\s+(\w+)\s*\n\{', specification):
        end = specification.find('\n}', match.end())
        if end < 0:
            raise ValueError('Unclosed overlay block')
        name = match[1]
        objects = re.findall(r'(?m)^\s*Object\s+(\S+\.o)\s*$', specification[match.end():end])
        if name in {row['name'] for row in overlays}:
            raise ValueError('Duplicate overlay name: ' + name)
        if name.startswith('OVY_') and int(name[4:]) != len(overlays):
            raise ValueError('Overlay ID differs from original manifest order: ' + name)
        for obj in objects:
            if obj in owners:
                raise ValueError('Overlay object has multiple owners: ' + obj)
            owners[obj] = name
        overlays.append(dict(id=len(overlays), name=name, objects=objects))
    if not overlays:
        raise ValueError('No original overlays')
    return overlays


def first_value(text):
    """Split a top-level LLVM initializer from its trailing attributes."""
    stack = []
    quoted = False
    for index, char in enumerate(text):
        if char == '"':
            quoted = not quoted
        if quoted:
            continue
        if char in '[{<(':
            stack.append(char)
        elif char in ']}>)':
            if not stack or '[{<('.index(stack.pop()) != ']}>)'.index(char):
                raise ValueError('Unbalanced LLVM value')
        elif char == ',' and not stack:
            return text[:index].strip()
    if stack or quoted:
        raise ValueError('Unterminated LLVM value')
    return text.strip()


def llvm_type(text):
    if text[0] in '[{<':
        stack = []
        quoted = False
        for index, char in enumerate(text):
            if char == '"':
                quoted = not quoted
            if quoted:
                continue
            if char in '[{<':
                stack.append(char)
            elif char in ']}>':
                if not stack or '[{<'.index(stack.pop()) != ']}>'.index(char):
                    raise ValueError('Unbalanced LLVM type')
                if not stack:
                    return text[:index + 1], text[index + 1:].strip()
        raise ValueError('Unterminated LLVM type')
    match = re.match(r'(%' + IDENT + r'|i\d+|half|float|double|fp128|x86_fp80|ppc_fp128|ptr)\s+(.+)$', text)
    if not match:
        raise ValueError('Unsupported LLVM global type: ' + text[:120])
    if match[2].startswith('addrspace('):
        raise ValueError('Overlay globals require address space zero')
    return match[1], match[2]


def getter_name(key):
    return 'HGOverlayUnit_' + hashlib.sha256(key.encode()).hexdigest()[:24]


def instrument_ir(source, key):
    if not re.search(r'^target triple = "wasm32-', source, re.M):
        raise ValueError('Overlay state requires the original 32-bit intermediate ABI')
    if re.search(r'^(?:@HGOverlayUnit_\w+_unit =|define[^\n]* @HGOverlayUnit_)',source,re.M):
        raise ValueError('Overlay IR was already instrumented')
    name = getter_name(key)
    kept, added, spans = [], [], []
    functions = {'ctors': [], 'dtors': []}
    symbols = []
    for line in source.splitlines():
        if line.startswith('declare ') and '@'+name+'(' in line:
            if not re.match(r'^declare (?:noundef )?ptr @'+name+r'\(\)(?:\s|$)',line):
                raise ValueError('Overlay unit getter has an incompatible declaration')
            continue
        match = re.match(r'^(@' + IDENT + r') = (.*?)\b(global|constant)\s+(.+)$', line)
        if not match:
            kept.append(line)
            continue
        symbol, attributes, kind, value = match.groups()
        if symbol in ('@llvm.global_ctors', '@llvm.global_dtors'):
            typ, initializer = llvm_type(value)
            count = re.match(r'^\[(\d+) x ', typ)
            entries = re.findall(r'\{\s*i32\s+(\d+),\s*ptr\s+(@' + IDENT + r'|null),\s*ptr\s+([^}]+)\}', first_value(initializer))
            if not count or len(entries) != int(count[1]) or any(data.strip() != 'null' for _, _, data in entries):
                raise ValueError('Unsupported associated constructor/destructor records')
            functions[symbol.rsplit('_', 1)[1]] = [(int(priority), fn) for priority, fn, _ in entries if fn != 'null']
            continue
        kept.append(line)
        if kind != 'global' or symbol.startswith('@llvm.') or re.search(r'\b(external|extern_weak|available_externally)\b', attributes):
            continue
        typ, value = llvm_type(value)
        initial = first_value(value)
        if re.search(r'\b(thread_local|addrspace|comdat|weak|weak_odr|linkonce|linkonce_odr|common)\b', attributes) or re.search(r',\s*comdat\b', value[len(initial):]):
            raise ValueError('Overlay global has shared/TLS storage: ' + symbol)
        if initial in ('undef', 'poison'):
            raise ValueError('Overlay global has no deterministic initial image: ' + symbol)
        image = 'null'
        if initial not in ('zeroinitializer', '0', 'null', 'false', '0.000000e+00'):
            image = '@' + name + '_image_' + str(len(spans))
            added.append(f'{image} = private constant {typ} {initial}')
        spans.append('{ ptr, ptr, i32 } { ptr ' + symbol + ', ptr ' + image +
                     ', i32 ptrtoint (ptr getelementptr (' + typ + ', ptr null, i32 1) to i32) }')
        symbols.append(symbol)
    added.append(f'@{name}_spans = private constant [{len(spans)} x {{ ptr, ptr, i32 }}] [' + ', '.join(spans) + ']')
    for which, entries in functions.items():
        entries.sort(key=lambda entry: entry[0])
        if which=='dtors':
            entries.reverse()
        records = ', '.join(f'{{ i32, ptr }} {{ i32 {priority}, ptr {fn} }}' for priority, fn in entries)
        added.append(f'@{name}_{which} = private constant [{len(entries)} x {{ i32, ptr }}] [{records}]')
    owned_functions=[fn for attributes,fn in re.findall(r'^define\b([^\n]*?)(@'+IDENT+r')\(',source,re.M)
                     if not re.search(r'\bavailable_externally\b',attributes)]
    added.append(f'@{name}_functions = private constant [{len(owned_functions)} x ptr] ['+', '.join('ptr '+fn for fn in owned_functions)+']')
    added.append(f'@{name}_unit = private constant {{ i32, ptr, i32, ptr, i32, ptr, i32, ptr }} '
                 f'{{ i32 {len(spans)}, ptr @{name}_spans, i32 {len(functions["ctors"])}, ptr @{name}_ctors, '
                 f'i32 {len(functions["dtors"])}, ptr @{name}_dtors, i32 {len(owned_functions)}, ptr @{name}_functions }}')
    added.append(f'define hidden ptr @{name}() {{\n  ret ptr @{name}_unit\n}}')
    return '\n'.join(kept + added) + '\n', dict(key=key, getter=name, globals=symbols,
        constructors=len(functions['ctors']), destructors=len(functions['dtors']),functions=len(owned_functions))


def compile_unit(compiler,flags,source,target,key,cwd):
    target=Path(target); target.unlink(missing_ok=True); intermediate=target.with_suffix('.ll')
    base=[str(compiler),*map(str,flags)]
    result=subprocess.run([*base,'-Xclang','-disable-llvm-passes','-S','-emit-llvm',str(source),'-o',str(intermediate)],cwd=cwd,capture_output=True,text=True)
    diagnostics=result.stdout+result.stderr
    if result.returncode: return result.returncode,diagnostics,dict(key=key,error='IR compilation failed')
    try:
        transformed,report=instrument_ir(intermediate.read_text(),key)
        intermediate.write_text(transformed)
        command=[*base,'-c',str(intermediate),'-o',str(target)]
    except ValueError as error:
        report=dict(key=key,error=str(error))
        command=[*base,'-c',str(source),'-o',str(target)]
    result=subprocess.run(command,cwd=cwd,capture_output=True,text=True)
    diagnostics+=result.stdout+result.stderr
    if result.returncode: target.unlink(missing_ok=True)
    return result.returncode,diagnostics,report


def catalog_source(overlays,objects,replacements=None):
    replacements=replacements or {}
    if set(replacements)-{row['name'] for row in overlays}:
        raise ValueError('Native replacement names an unknown original overlay')
    occupied={obj for row in overlays if row['name'] not in replacements for obj in row['objects']}
    for native_objects in replacements.values():
        if not native_objects or len(set(native_objects))!=len(native_objects) or occupied.intersection(native_objects):
            raise ValueError('Native replacement has empty or ambiguous object ownership')
        occupied.update(native_objects)
    lines=['#include <nitro/fs/native_overlay.h>']; rows=[]
    for overlay in overlays:
        getters=[]; missing=[]
        native_objects=replacements.get(overlay['name'],overlay['objects'])
        for obj in native_objects:
            state=objects.get(obj)
            if not state or not state.get('getters'): missing.append(obj)
            else: getters.extend(state['getters'])
        ready=not missing
        rows.append(dict(id=overlay['id'],name=overlay['name'],ready=ready,missing=missing,units=len(getters)))
        if overlay['name'] in replacements:
            rows[-1].update(originalObjects=overlay['objects'],nativeObjects=native_objects)
        if ready:
            lines.extend('extern const HGNativeOverlayUnit *'+name+'(void);' for name in getters)
            lines.append(f'static const HGNativeOverlayGetter units_{overlay["id"]}[]={{'+','.join(getters)+'};')
    lines.append('const HGNativeOverlayEntry HGNativeOverlays[]={')
    lines.extend(('{'+str(row['units'])+',units_'+str(row['id'])+',TRUE},') if row['ready'] else '{0,0,FALSE},' for row in rows)
    lines.extend(['};',f'const u32 HGNativeOverlayCount={len(rows)};'])
    return '\n'.join(lines)+'\n',rows
