# SPDX-License-Identifier: GPL-3.0-or-later
"""Reassemble original data, retaining ARM bytes and converting symbolic relocations."""
from concurrent.futures import ThreadPoolExecutor
import hashlib
import json
from pathlib import Path
import re
import struct
import subprocess

ROOT = Path(__file__).resolve().parents[2]


def region(data, start, size):
    if start < 0 or size < 0 or start > len(data) or size > len(data)-start:
        raise ValueError('Object range outside file')
    return data[start:start+size]


class Reader:
    def __init__(self, data): self.data, self.pos = data, 0
    def take(self, size):
        value = region(self.data,self.pos,size); self.pos += size; return value
    def byte(self): return self.take(1)[0]
    def leb(self, signed=False):
        value = shift = 0
        for _ in range(5):
            byte = self.byte(); value |= (byte & 127) << shift; shift += 7
            if not byte & 128:
                if signed and byte & 64: value -= 1 << shift
                if not -(1 << 31) <= value < (1 << 32): raise ValueError('Invalid 32-bit LEB')
                return value
        raise ValueError('Overlong LEB')
    def name(self): return self.take(self.leb()).decode('utf-8')
    def limits(self):
        flags = self.leb()
        if flags & ~3: raise ValueError('Unsupported non-32-bit limits')
        self.leb()
        if flags & 1: self.leb()


def wasm_object(data, contents=False):
    r = Reader(data)
    if r.take(8) != b'\0asm\x01\0\0\0': raise ValueError('Expected Wasm32 object')
    types, imports, function_types, symbols, segments, segment_names, relocations = [], {0:[],2:[],4:[],5:[]}, [], [], [], [], []
    alignments = []
    linking = False
    while r.pos < len(data):
        kind = r.byte(); section = Reader(r.take(r.leb()))
        if kind == 1:
            for _ in range(section.leb()):
                if section.byte() != 0x60: raise ValueError('Unsupported function type')
                params = tuple(section.byte() for _ in range(section.leb()))
                results = tuple(section.byte() for _ in range(section.leb()))
                types.append((params,results))
        elif kind == 2:
            for _ in range(section.leb()):
                section.name(); name = section.name(); tag = section.byte()
                if tag == 0:
                    imports[0].append(name); function_types.append(section.leb())
                elif tag == 1:
                    imports[5].append(name); section.byte(); section.limits()
                elif tag == 2: section.limits()
                elif tag == 3:
                    imports[2].append(name); section.take(2)
                elif tag == 4:
                    imports[4].append(name); section.byte(); section.leb()
                else: raise ValueError('Unsupported import kind')
        elif kind == 3:
            function_types.extend(section.leb() for _ in range(section.leb()))
        elif kind == 11 and contents:
            for _ in range(section.leb()):
                flags = section.leb()
                if flags not in (0,1,2): raise ValueError('Unsupported data segment')
                if flags == 2 and section.leb() != 0: raise ValueError('Multiple memories')
                if flags != 1:
                    if section.byte() != 0x41: raise ValueError('Nonconstant data offset')
                    section.leb(True)
                    if section.byte() != 0x0B: raise ValueError('Invalid data initializer')
                size = section.leb(); offset = section.pos
                segments.append(dict(offset=offset,bytes=section.take(size)))
        elif kind == 0:
            name = section.name()
            if name == 'linking':
                linking = True
                if section.leb() != 2: raise ValueError('Unsupported linking metadata')
                while section.pos < len(section.data):
                    tag = section.byte(); sub = Reader(section.take(section.leb()))
                    if tag == 5:
                        for _ in range(sub.leb()):
                            segment_names.append(sub.name()); alignment = sub.leb()
                            if alignment>31: raise ValueError('Excessive segment alignment')
                            alignments.append(1<<alignment); sub.leb()
                    elif tag == 8:
                        for _ in range(sub.leb()):
                            symbol_kind, flags = sub.byte(), sub.leb()
                            symbol = dict(kind=symbol_kind,flags=flags)
                            if symbol_kind == 1:
                                symbol['name'] = sub.name()
                                if not flags & 0x10: symbol.update(segment=sub.leb(),offset=sub.leb(),size=sub.leb())
                            elif symbol_kind == 3: symbol['section'] = sub.leb()
                            elif symbol_kind in (0,2,4,5):
                                index = sub.leb(); symbol['index'] = index
                                symbol['name'] = sub.name() if not flags & 0x10 or flags & 0x40 else imports[symbol_kind][index]
                                if symbol_kind == 0: symbol['signature'] = types[function_types[index]]
                            else: raise ValueError('Unknown object symbol kind')
                            symbols.append(symbol)
                    if tag in (5,8) and sub.pos != len(sub.data): raise ValueError('Trailing linking metadata')
            elif name == 'reloc.DATA' and contents:
                section.leb()
                for _ in range(section.leb()):
                    tag, offset, index = section.byte(), section.leb(), section.leb()
                    if tag not in (2,5): raise ValueError('Unsupported data relocation')
                    relocations.append(dict(kind=tag,offset=offset,symbol=index,addend=section.leb(True) if tag==5 else 0))
    if not linking: raise ValueError('Object has no linking metadata')
    return dict(symbols=symbols,segments=segments,segmentNames=segment_names,alignments=alignments,relocations=relocations)


def arm_object(data):
    header = struct.unpack('<16sHHIIIIIHHHHHH',region(data,0,52))
    if header[0][:7] != b'\x7fELF\x01\x01\x01' or header[1:3] != (1,40) or header[11] != 40 or not header[12]:
        raise ValueError('Expected relocatable ARM ELF32')
    sections = [dict(zip(('nameOffset','type','flags','address','offset','size','link','info','alignment','entrySize'),
                        struct.unpack('<10I',region(data,header[6]+40*i,40)))) for i in range(header[12])]
    def section_bytes(index):
        entry = sections[index]
        if entry['size'] > 64*1024*1024: raise ValueError('Excessive data section')
        return bytes(entry['size']) if entry['type']==8 else region(data,entry['offset'],entry['size'])
    def string(blob, offset):
        if offset >= len(blob): raise ValueError('Invalid ELF string offset')
        end = blob.find(b'\0',offset)
        if end < 0: raise ValueError('Unterminated ELF string')
        return blob[offset:end].decode('utf-8')
    names = section_bytes(header[13])
    for section in sections: section['name'] = string(names,section['nameOffset'])
    tables = [i for i,section in enumerate(sections) if section['type']==2]
    if len(tables)!=1: raise ValueError('Ambiguous ELF symbol table')
    table = sections[tables[0]]; strings = section_bytes(table['link'])
    if table['entrySize']!=16 or table['size']%16: raise ValueError('Invalid ELF symbols')
    symbols = []
    for offset in range(table['offset'],table['offset']+table['size'],16):
        name,value,size,info,other,index = struct.unpack('<IIIBBH',region(data,offset,16))
        symbols.append(dict(name=string(strings,name),offset=value,size=size,type=info&15,binding=info>>4,section=index))
    allocations = {i:section for i,section in enumerate(sections) if section['flags']&2 and section['size']}
    for index,section in allocations.items():
        if section['type'] not in (1,8): raise ValueError('Non-data ARM section')
        section['bytes'] = section_bytes(index); section['relocations'] = []
    if any(symbol['type']==2 and symbol['section'] for symbol in symbols): raise ValueError('Executable ARM symbol in data input')
    for section in sections:
        if section['type'] not in (4,9): continue
        if section['info'] not in allocations: continue
        if section['type']!=9 or section['entrySize']!=8 or section['size']%8 or section['link']!=tables[0]:
            raise ValueError('Unsupported ARM relocation table')
        target = allocations[section['info']]
        for offset in range(section['offset'],section['offset']+section['size'],8):
            address,info = struct.unpack('<II',region(data,offset,8))
            if info&255 not in (2,38): raise ValueError(f'Unsupported ARM relocation {info&255}')
            addend = struct.unpack('<i',region(target['bytes'],address,4))[0]
            target['relocations'].append(dict(offset=address,symbol=symbols[info>>8],addend=addend))
    return dict(sections=allocations,symbols=symbols)


def expand(path, stack=()):
    path = Path(path).resolve()
    if path in stack: raise ValueError(f'Recursive assembler include: {path}')
    if path == ROOT/'asm/macros.inc' or ROOT/'asm/macros' in path.parents: return ''
    output = []
    for line in path.read_text().splitlines():
        line = re.sub(r'"(?:\\.|[^"\\])*"|;.*',lambda m:'' if m[0].startswith(';') else m[0],line)
        include = re.fullmatch(r'\s*\.include\s+"([^"]+)"\s*',line)
        if include:
            candidates = [ROOT/include[1],ROOT/'asm/include'/include[1],path.parent/include[1],ROOT/'lib/NitroDWC/asm/include'/include[1]]
            found = next((p for p in candidates if p.is_file()),None)
            if found is None: raise ValueError(f'Missing assembler include: {include[1]}')
            output.append(expand(found,stack+(path,)))
        elif line.strip() != '#pragma once': output.append(line+'\n')
    return ''.join(output)


def prepare_arm(source, output, sdk, flags):
    expanded = output.with_suffix('.expanded.S'); expanded.write_text(expand(source))
    # Reconstruct preprocessing flags without compiler-only options.
    options = []
    for i,value in enumerate(flags):
        value = str(value)
        if value.startswith('-D') or value.startswith('-I') and value!='-I': options.append(value)
        elif value=='-I': options += ['-I',str(flags[i+1])]
    result = subprocess.run([str(sdk/'bin/clang'),'-E','-P','-x','assembler-with-cpp','-DSDK_ASM','-DPM_ASM',*options,str(expanded)],
                            cwd=ROOT,capture_output=True,text=True)
    if result.returncode: raise ValueError(result.stderr[-1500:])
    text = result.stdout; macros = set(re.findall(r'^\s*\.macro\s+(\w+)',text,re.M))
    allowed = {'.public','.global','.globl','.extern','.text','.rodata','.data','.bss','.section','.word','.byte','.short',
               '.space','.balign','.ascii','.asciz','.set','.float','.size','.macro','.endm'}
    lines = []
    for line in text.splitlines():
        line = line.strip()
        if not line or line.startswith('#'): continue
        token = line.split()[0]
        if not re.match(r'^[\w.$?]+:',line) and token not in allowed and token not in macros:
            raise ValueError(f'Non-data assembler statement: {line[:120]}')
        if token=='.public': line = line.replace('.public','.global',1)
        if token in ('.byte','.short','.word','.float') and line.endswith(','): line = line[:-1]
        if token=='.rodata': line = '.section .rodata,"a",%progbits'
        if token=='.section':
            if line!='.section .version,4': raise ValueError(f'Unsupported section declaration: {line}')
            line = '.section .version,"a",%progbits'
        lines.append(line)
    prepared = output.with_suffix('.arm.s'); prepared.write_text('\n'.join(lines)+'\n')
    result = subprocess.run(['xcrun','clang','--target=armv5te-none-eabi','-x','assembler','-c',str(prepared),'-o',str(output)],
                            cwd=ROOT,capture_output=True,text=True)
    if result.returncode: raise ValueError(result.stderr[-1500:])
    return arm_object(output.read_bytes())


def function_registry(objects):
    defined, referenced, variables = {}, {}, set()
    for path in objects:
        for symbol in wasm_object(Path(path).read_bytes())['symbols']:
            if symbol['flags']&2: continue
            if symbol['kind']==1: variables.add(symbol['name'])
            if symbol['kind']!=0: continue
            target = referenced if symbol['flags']&0x10 else defined
            target.setdefault(symbol['name'],set()).add(symbol['signature'])
    canonical = {}
    for name in set(defined)|set(referenced):
        signatures = defined.get(name,referenced.get(name))
        if len(signatures)==1: canonical[name] = next(iter(signatures))
    variables.difference_update(set(defined)|set(referenced))
    return canonical,variables


def emit(arm, functions, variables, overlay_ids, identity):
    def quote(name):
        if not re.fullmatch(r'[A-Za-z_.$][\w.$]*',name): raise ValueError(f'Unsupported assembler symbol: {name}')
        return name
    sections = arm['sections']; symbols = arm['symbols']
    anchors = {index:f'.Lhg_data_{identity}_{index}' for index in sections}
    defined = {s['name'] for s in symbols if s['section'] in sections and s['binding'] and s['name'] and s['type']!=3}
    if defined & functions.keys(): raise ValueError(f'Data input defines functions: {sorted(defined & functions.keys())}')
    imports, functions_used, expected, output = set(), set(), {}, []
    value_types = {0x7F:'i32',0x7E:'i64',0x7D:'f32',0x7C:'f64'}
    for index,section in sections.items():
        blob = bytearray(section['bytes']); relocations = {}
        for item in section['relocations']:
            symbol, addend = item['symbol'],item['addend']; name = symbol['name']
            if symbol['section'] in anchors and (symbol['type']==3 or symbol['binding']==0):
                if symbol['section'] not in anchors: raise ValueError('Reference to non-data section')
                name = anchors[symbol['section']]
                addend += symbol['offset']
            if symbol['section']==0xFFF1:
                blob[item['offset']:item['offset']+4] = struct.pack('<I',(symbol['offset']+addend)&0xFFFFFFFF)
                continue
            if name in overlay_ids:
                blob[item['offset']:item['offset']+4] = struct.pack('<I',(overlay_ids[name]+addend)&0xFFFFFFFF)
                continue
            kind = 2 if name in functions else 5
            if kind==2:
                if addend: raise ValueError(f'Function-pointer addend requires review: {name}+{addend}')
                functions_used.add(name)
            elif name not in defined and name not in variables and name not in anchors.values():
                raise ValueError(f'Unknown relocation symbol: {name}')
            if not name: raise ValueError('Unnamed relocation')
            if any(abs(item['offset']-offset)<4 for offset in relocations): raise ValueError('Overlapping relocation')
            relocations[item['offset']] = dict(name=name,addend=addend,kind=kind)
            if symbol['section']==0: imports.add(name)
        native_name = f'.hg_data.{identity}.{index}'
        output.append(f'.section {quote(native_name)},"",@')
        alignment = section['alignment'] or 1
        if alignment & (alignment-1): raise ValueError('Invalid ARM section alignment')
        output.append(f'.p2align {alignment.bit_length()-1}, 0')
        labels = {0:[dict(name=anchors[index],binding=0,size=len(blob))]}
        for symbol in symbols:
            if symbol['section']!=index or not symbol['binding'] or not symbol['name'] or symbol['type']==3: continue
            if symbol['offset'] > len(blob) or symbol['size'] > len(blob)-symbol['offset']: raise ValueError('Symbol outside data section')
            labels.setdefault(symbol['offset'],[]).append(symbol)
        position = 0
        while position <= len(blob):
            for symbol in labels.get(position,[]):
                if symbol['binding'] not in (0,1,2): raise ValueError('Unsupported ELF symbol binding')
                name = quote(symbol['name'])
                if symbol['binding']: output.append(('.weak ' if symbol['binding']==2 else '.globl ')+name)
                output += [f'.type {name},@object',f'.size {name},{symbol["size"]}',name+':']
            if position==len(blob): break
            if position in relocations:
                item = relocations[position]
                if any(position < p < position+4 for p in labels): raise ValueError('Symbol inside relocated word requires review')
                output.append(f'.int32 {quote(item["name"])}'+(f'{item["addend"]:+d}' if item['addend'] else ''))
                blob[position:position+4] = bytes(4); position += 4
            else:
                end = min([len(blob),position+32,*[p for p in labels if p>position],*[p for p in relocations if p>position]])
                output.append('.byte '+','.join(str(b) for b in blob[position:end])); position = end
        expected[native_name] = dict(bytes=bytes(blob),relocations=relocations,alignment=alignment,
                                    exports={s['name']:(offset,s['size'],s['binding']==2) for offset,items in labels.items() for s in items if s['binding']})
    for name in sorted(imports-functions_used): output += [f'.type {quote(name)},@object',f'.size {quote(name)},0']
    for name in sorted(functions_used):
        args,result = functions[name]
        output.append(f'.functype {quote(name)} ({", ".join(value_types[x] for x in args)}) -> ({", ".join(value_types[x] for x in result)})')
    if functions_used: output.append('.no_dead_strip __indirect_function_table')
    return '\n'.join(output)+'\n',expected,defined


def verify(path, expected):
    obj = wasm_object(Path(path).read_bytes(),True)
    if len(obj['segments'])!=len(expected): raise ValueError('Data segment count changed')
    for number,segment in enumerate(obj['segments']):
        name = obj['segmentNames'][number]; want = expected[name]; blob = bytearray(segment['bytes']); found = {}
        if obj['alignments'][number] < want['alignment']: raise ValueError('Data segment alignment weakened')
        for relocation in obj['relocations']:
            offset = relocation['offset']-segment['offset']
            if not 0 <= offset < len(blob): continue
            region(blob,offset,4)
            symbol = obj['symbols'][relocation['symbol']]
            found[offset] = dict(name=symbol['name'],kind=relocation['kind'],addend=relocation['addend'])
            blob[offset:offset+4] = bytes(4)
        if bytes(blob)!=want['bytes'] or found!=want['relocations']:
            raise ValueError(f'ARM/Wasm data or relocations differ: {name}')
        exports = {s['name']:(s['offset'],s['size'],bool(s['flags']&1)) for s in obj['symbols']
                   if s['kind']==1 and not s['flags']&0x12 and s['segment']==number}
        if exports!=want['exports']: raise ValueError(f'ARM/Wasm data symbols differ: {name}')
    exports = {s['name'] for s in obj['symbols'] if s['kind']==1 and not s['flags']&0x12}
    return exports


def build(objects, sdk, flags, destination):
    destination = Path(destination); destination.mkdir(parents=True,exist_ok=True)
    (destination/'report.json').unlink(missing_ok=True)
    functions,variables = function_registry(objects)
    overlay_text = (ROOT/'ios/build-original-host/include/hg_native_overlay_ids.h').read_text()
    overlays = {f'SDK_OVERLAY_{name}_ID':int(value) for name,value in re.findall(r'^#define HG_OVERLAY_ID_(\w+) (\d+)$',overlay_text,re.M)}
    sources = sorted((ROOT/'asm').glob('*.s'))
    # All source labels are known data candidates, including references across files.
    variables.update(name for source in sources for name in re.findall(r'^\s*([\w.$?]+):',source.read_text(),re.M))
    def convert(source):
        target = destination/(source.stem+'.o'); target.unlink(missing_ok=True)
        try:
            arm = prepare_arm(source,destination/(source.stem+'.arm.o'),sdk,flags)
            assembly,expected,defined = emit(arm,functions,variables,overlays,source.stem)
            native = destination/(source.stem+'.wasm.s'); native.write_text(assembly)
            result = subprocess.run([str(sdk/'bin/clang'),'-c','-x','assembler',str(native),'-o',str(target)],capture_output=True,text=True)
            if result.returncode: raise ValueError(result.stderr[-1500:])
            exports = verify(target,expected)
            review = []
            for section in arm['sections'].values():
                relocated = {r['offset'] for r in section['relocations']}
                for offset in range(0,len(section['bytes'])-3,4):
                    value = struct.unpack_from('<I',section['bytes'],offset)[0]
                    if offset not in relocated and 0x02000000 <= value < 0x02800000:
                        review.append(dict(section=section['name'],offset=offset,value=f'0x{value:08X}'))
            return dict(file=str(source.relative_to(ROOT)),sha256=hashlib.sha256(source.read_bytes()).hexdigest(),exitCode=0,
                        object=str(target),symbols=sorted(exports),bytes=sum(len(s['bytes']) for s in expected.values()),
                        preparedSha256=hashlib.sha256((destination/(source.stem+'.arm.arm.s')).read_bytes()).hexdigest(),
                        ramShapedWordsForRuntimeReview=review)
        except (ValueError,KeyError,IndexError,struct.error,UnicodeError) as error:
            target.unlink(missing_ok=True)
            return dict(file=str(source.relative_to(ROOT)),sha256=hashlib.sha256(source.read_bytes()).hexdigest(),exitCode=1,error=str(error))
    with ThreadPoolExecutor(max_workers=4) as pool: units = list(pool.map(convert,sources))
    report = dict(scope='data-only ARM-to-Wasm object conversion, constants and symbolic relocations checked; no ARM code execution',
                  functionTypesSha256=hashlib.sha256(json.dumps(functions,sort_keys=True).encode()).hexdigest(),
                  builderSha256=hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),
                  summary=dict(total=len(units),compiled=sum(not u['exitCode'] for u in units)),units=units)
    (destination/'report.json').write_text(json.dumps(report,indent=2)+'\n')
    return report
