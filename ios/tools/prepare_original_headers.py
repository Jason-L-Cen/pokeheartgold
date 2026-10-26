# SPDX-License-Identifier: GPL-3.0-or-later
"""Build original data headers with the repository's existing native tools."""
import argparse
import hashlib
import json
from concurrent.futures import ThreadPoolExecutor
from pathlib import Path
import re
import shutil
import subprocess
import tempfile
from prepare_overlay_state import overlay_objects

ROOT = Path(__file__).resolve().parents[2]

def sine_table(output):
    source=(ROOT/'lib/NitroSDK/asm/fx_sincos.s').read_text().split('FX_SinCosTable_:\n',1)[1]
    rows=[]
    for line in source.splitlines():
        if not line.strip(): continue
        match=re.fullmatch(r'\s*\.short (0x[0-9A-Fa-f]{4}), (0x[0-9A-Fa-f]{4})',line)
        if not match: raise ValueError(f'Unexpected original sine table data: {line}')
        rows.append(', '.join('(s16)'+value for value in match.groups()))
    if len(rows)!=4096: raise ValueError('Original sine/cosine table must contain 4096 pairs')
    (output/'hg_native_sincos.c').write_text('#include <nitro/types.h>\nconst s16 FX_SinCosTable_[8192] = {\n'+',\n'.join(rows)+'\n};\n')


def member_indexes(source):
    """Recover only explicit member order: numbered files or ordered data objects."""
    if source.is_dir():
        order = source / '.narcorder'
        if order.is_file():
            members = {}
            for index, line in enumerate(order.read_text().splitlines()):
                path = Path(line)
                if not line or path.is_absolute() or '..' in path.parts:
                    raise ValueError(f'Unsafe explicit archive member: {line}')
                name = re.sub(r'[^A-Za-z0-9_]', '_', line)
                if name in members:
                    raise ValueError(f'Duplicate explicit archive member: {line}')
                members[name] = index
            if not members: raise ValueError(f'Empty archive member order: {order}')
            return members
        members = {}
        for path in source.iterdir():
            if path.is_dir() and not path.name.startswith('.'):
                raise ValueError(f'Archive needs an explicit recursive member order: {source}')
            if not path.is_file() or path.name.startswith('.') or path.suffix in ('.mk', '.txt', '.h'):
                continue
            stem = path.name.split('.')[0]
            match = (re.match(re.escape(source.name) + r'_(\d+)(?:_|$)', stem) or re.match(r'^(\d+)_', stem)
                     or re.search(r'(?:^|_)(\d{8})(?:_|$)', stem))
            if not match:
                raise ValueError(f'No explicit archive member number: {path}')
            members[stem] = int(match[1])
        if not members or sorted(set(members.values())) != list(range(max(members.values()) + 1)):
            raise ValueError(f'Non-contiguous archive members: {source}')
        if len(set(members.values())) != len(members):
            raise ValueError(f'Ambiguous archive member names: {source}')
        if [members[name] for name in sorted(members)] != list(range(len(members))):
            raise ValueError(f'Member numbers disagree with filename order: {source}')
        return members
    template = source.with_suffix('.json.txt')
    if not template.is_file():
        raise ValueError(f'Archive source not found: {source}')
    text = template.read_text()
    if not re.search(r'^\s*\.type\b', text, re.MULTILINE):
        raise ValueError(f'Archive requires its C data builder: {template}')
    data = source.with_suffix('.json')
    if data.is_file():
        text = run([ROOT / 'tools/jsonproc/jsonproc', data, template, '/dev/stdout'])
        text = run(['xcrun', 'clang', '-E', '-P', '-x', 'assembler-with-cpp', '-DHEARTGOLD', '-DENGLISH',
                    '-DPM_ASM', '-I', ROOT / 'include', '-'], input=text)
    names = re.findall(r'^\s*\.type\s+(\w+),\s*@object', text, re.MULTILINE)
    if not names or len(set(names)) != len(names) or any(name in ('__size', '__data') for name in names):
        raise ValueError(f'Ambiguous archive data objects: {template}')
    members = {name: index for index, name in enumerate(names)}
    for index in range(len(names)):
        alias = f'{source.name}_{index:08d}'
        if alias in members and members[alias] != index:
            raise ValueError(f'Archive data object conflicts with numeric index: {source}')
        members[alias] = index
    return members


def archive_headers(output, prepared=()):
    text = '\n'.join(path.read_text() for folder in ('src', 'include')
                     for path in sorted((ROOT / folder).rglob('*')) if path.suffix in ('.c', '.cpp', '.h'))
    quoted = r'"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\''
    text = re.sub(quoted + r'|//[^\n]*|/\*.*?\*/',
                  lambda match: ' ' + '\n' * match[0].count('\n') if match[0].startswith('/') else match[0], text, flags=re.DOTALL)
    headers = sorted(set(re.findall(r'^[ \t]*#[ \t]*include[ \t]+["<]([^">]+\.naix)[">]', text, re.MULTILINE)))
    text = re.sub(quoted, ' ', text)
    generated, pending = 0, []
    for header in headers:
        if Path(header).is_absolute() or '..' in Path(header).parts:
            raise ValueError(f'Unsafe archive header path: {header}')
        if header in prepared or header in ('msgdata/msg.naix', 'itemtool/itemdata/item_data.naix', 'itemtool/itemdata/item_icon.naix'):
            continue # These are built with the original packagers below.
        if any((ROOT / folder / header).is_file() for folder in ('include', 'files')):
            continue
        source = ROOT / 'files' / Path(header).with_suffix('')
        target = output / header
        prefix = 'NARC_' + source.name + '_'
        symbols = sorted(set(re.findall(r'\b' + re.escape(prefix) + r'\w+', text)))
        try:
            if header == 'application/zukanlist/zkn_data/zukan_hw_data.naix':
                # Match zukan_hw_data.mk: select Gold, rename member 1, exclude the other version.
                inputs = [source / 'zukan_hw_data_0.bin', source / 'zukan_hw_data_1_gold.bin']
                if not all(path.is_file() for path in inputs):
                    raise ValueError(f'HeartGold archive source not found: {source}')
                target.parent.mkdir(parents=True, exist_ok=True)
                with tempfile.TemporaryDirectory(prefix='hg-index-', dir=output) as temporary:
                    staging = Path(temporary)
                    for index, path in enumerate(inputs): shutil.copyfile(path, staging / f'zukan_hw_data_{index}.bin')
                    run([ROOT / 'tools/nitroarc/nitroarc', '-cPf', target.with_suffix('.narc'), staging])
                target.write_text('// Generated from explicit original source member order.\n' + target.read_text())
                generated += 1
                continue
            members = member_indexes(source)
            definitions = {prefix + name: index for name, index in members.items()} if not source.is_dir() else {}
            for symbol in symbols:
                suffix = symbol[len(prefix):]
                indexes = {members[suffix]} if suffix in members else {
                    index for name, index in members.items() if suffix.startswith(name + '_')}
                if len(indexes) != 1:
                    if (not source.is_dir() and re.search(r'\b' + re.escape(symbol) + r'\s*##', text)
                            and any(name.startswith(suffix) for name in members)):
                        continue
                    raise ValueError(f'Archive symbol does not identify one source member: {symbol}')
                definitions[symbol] = indexes.pop()
            if not definitions:
                raise ValueError(f'No referenced archive symbols: {header}')
        except ValueError as error:
            if target.is_file() and target.read_text().startswith('// Generated from explicit original source member order.\n'):
                target.unlink()
            pending.append(str(error)); continue
        target.parent.mkdir(parents=True, exist_ok=True)
        target.write_text('// Generated from explicit original source member order.\n#pragma once\n'
                          + ''.join(f'#define {name} {index}\n' for name, index in sorted(definitions.items())))
        generated += 1
    print(f'Prepared source archive indexes: {generated}; unresolved source orders: {len(pending)}.')
    for reason in pending:
        print('Pending index:', reason.removeprefix(str(ROOT) + '/'))


def register_header():
    # Keep SDK bit fields and aliases; route accesses through the native platform.
    # Legacy registers.h can redefine views after io_reg.h; deliberately allow reapplication.
    result = '// Native register views; reapplied after legacy SDK definitions.\n'
    arithmetic = []
    for name in ('common', 'ARM9'):
        source = (ROOT / f'lib/include/nitro/hw/{name}/io_reg.h').read_text()
        widths = {address: int(width) // 8 for width, address in re.findall(
            r'#define reg_\w+ \(\*\((?:const )?REGType(\d+)v? \*\)(REG_\w+_ADDR)\)', source)}
        offsets = dict(re.findall(r'#define (REG_\w+_ADDR) \(HW_REG_BASE \+ (REG_\w+_OFFSET)\)', source))
        values = dict(re.findall(r'#define\s+(REG_\w+_OFFSET)\s+(0x[0-9A-Fa-f]+|\d+)\b', source))
        if any(offset not in values for offset in offsets.values()):
            raise ValueError('Register offsets must resolve before generating standalone SDK views')
        arithmetic += [(address, values[offset], widths[address]) for address, offset in offsets.items()
                       if 0x280 <= int(values[offset], 0) < 0x2C0]
        result += '#ifdef HEARTGOLD_ABI32\n'
        for register, ctype, bits, address in re.findall(
                r'#define (reg_\w+) \(\*\(((?:const )?REGType(\d+)v?) \*\)(REG_\w+_ADDR)\)', source):
            if address not in offsets:
                raise ValueError(f'Register offset not found: {address}')
            result += (f'#undef {register}\n#define {register} '
                       f'(*({ctype} *)HGNativeRegisterAddress(0x04000000u + {values[offsets[address]]}u, {int(bits) // 8}))\n')
        result += '#else\n'
        for address, offset in offsets.items():
            if address not in widths:
                raise ValueError(f'Register width not found: {address}')
            result += (f'#undef {address}\n#define {address} '
                       f'HGNativeRegisterAddress(0x04000000u + {values[offset]}u, {widths[address]})\n')
        result += '#endif\n'
    legacy = (ROOT / 'lib/NitroSDK/include/registers.h').read_text()
    views = re.findall(r'#define\s+(reg_\w+)\s+\(\*\(((?:const )?REGType(\d+)v?)\s*\*\)(0x[0-9A-Fa-f]+)\)', legacy)
    raw = set(re.findall(r'^#define\s+(reg_\w+)[^\n]*REGType[^\n]*0x[0-9A-Fa-f]+', legacy, re.MULTILINE))
    if not views or raw != {name for name, _, _, _ in views}:
        raise ValueError('Unrecognized legacy register view; native routing would be incomplete')
    result += '#ifdef NITRO_REGISTERS_H\n'
    for name, ctype, bits, address in views:
        result += (f'#undef {name}\n#define {name} '
                   f'(*({ctype} *)HGNativeRegisterAddress({address}u, {int(bits) // 8}))\n')
    result += '#endif\n'
    # These constants are contiguous register pointers, not DMA bus addresses.
    for address, offset, width in [*arithmetic, ('REG_BLDCNT_ADDR', '0x50', 4)]:
        result += (f'#undef {address}\n#define {address} '
                   f'HGNativeRegisterAddress(0x04000000u + {offset}u, {width})\n')
    return result


def run(args, cwd=ROOT, input=None):
    result = subprocess.run([str(arg) for arg in args], cwd=cwd, capture_output=True, text=True, input=input)
    if result.returncode:
        raise RuntimeError(f'{args[0]} failed: {result.stderr[-4000:]}')
    return result.stdout


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--output', type=Path, default=ROOT / 'ios/build-original-host/include')
    args = parser.parse_args()
    output = args.output.resolve(); output.mkdir(parents=True, exist_ok=True)
    sine_table(output)
    (output / 'hg_native_registers.h').write_text(register_header())
    specification=(ROOT/'main.lsf').read_bytes()
    ownership=overlay_objects(specification.decode())
    overlays=[entry['name'] for entry in ownership]
    (output/'hg_native_overlay_ownership.json').write_text(json.dumps(dict(
        manifestSha256=hashlib.sha256(specification).hexdigest(),overlays=ownership),indent=2)+'\n')
    overlay_header = '#pragma once\n' + ''.join(f'#define HG_OVERLAY_ID_{name} {index}\n' for index, name in enumerate(overlays))
    (output / 'hg_native_overlay_ids.h').write_text(overlay_header + f'#define HG_OVERLAY_COUNT {len(overlays)}\n')
    for tool in ('msgenc', 'nitroarc', 'csv2bin', 'jsonproc'):
        run(['make', '-s', '-C', ROOT / 'tools' / tool])
    messages = output / 'msgdata/msg'; messages.mkdir(parents=True, exist_ok=True)
    trainer = messages / 'msg_0729.gmm'
    trainer_json = ROOT / 'files/poketool/trainer/trainers.json'
    template = ROOT / 'files/poketool/trainer/trname.json.txt'
    if not trainer.exists() or trainer.stat().st_mtime < max(trainer_json.stat().st_mtime, template.stat().st_mtime):
        run([ROOT / 'tools/jsonproc/jsonproc', trainer_json, template, trainer])
        trainer.write_text(trainer.read_text().replace('&', '&amp;'))
    sources = sorted([*ROOT.glob('files/msgdata/msg/*.gmm'), trainer], key=lambda path: path.name)
    keys = dict(re.findall(r'files/msgdata/msg/(\w+)\.bin: MSGENCFLAGS \+= -k (0x[\dA-Fa-f]+)', (ROOT / 'files/msgdata/msg.mk').read_text()))
    charmap = ROOT / 'charmap.txt'

    def encode(source):
        header, binary = messages / (source.stem + '.h'), messages / (source.stem + '.bin')
        if header.exists() and binary.exists() and min(header.stat().st_mtime, binary.stat().st_mtime) >= max(source.stat().st_mtime, charmap.stat().st_mtime):
            return
        command = [ROOT / 'tools/msgenc/msgenc', '-e', '-c', charmap, '--gmm', '-H', header]
        if source.stem in keys:
            command += ['-k', keys[source.stem]]
        run([*command, source, binary])

    with ThreadPoolExecutor(max_workers=4) as pool:
        list(pool.map(encode, sources))
    ids = [int(source.stem[4:8]) for source in sources]
    if ids != list(range(len(ids))):
        raise ValueError('Original message bank IDs are not contiguous')
    file_list = output / 'msgdata/messages.txt'
    file_list.write_text(''.join(source.stem + '.bin\n' for source in sources))
    # -T orders entries but does not exclude unlisted files; pack only the current binaries.
    with tempfile.TemporaryDirectory(prefix='hg-messages-', dir=output) as temporary:
        staging = Path(temporary)
        for source in sources: shutil.copyfile(messages / (source.stem + '.bin'), staging / (source.stem + '.bin'))
        run([ROOT / 'tools/nitroarc/nitroarc', '-cPf', output / 'msgdata/msg.narc', '-T', file_list, staging])
    items = output / 'itemtool/itemdata'; items.mkdir(parents=True, exist_ok=True)
    csv = ROOT / 'files/itemtool/itemdata/item_data.csv'
    manifest = csv.with_suffix('.txt')
    run([ROOT / 'tools/csv2bin/csv2bin', 'compile', csv, items / 'item_data.narc', manifest, '-i', ROOT / 'include', '--naix', '--pad', '0xFF'])
    icons = ROOT / 'files/itemtool/itemdata/item_icon'
    run([ROOT / 'tools/nitroarc/nitroarc', '-cPf', items / 'item_icon.narc', icons])
    from build_original_resources import build_archives
    archive_headers(output, build_archives(output))
    print(f'Prepared original headers: {len(sources)} message banks, item records and item icons.')


if __name__ == '__main__':
    main()
