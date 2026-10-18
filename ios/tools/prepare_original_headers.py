# SPDX-License-Identifier: GPL-3.0-or-later
"""Build original data headers with the repository's existing native tools."""
import argparse
from concurrent.futures import ThreadPoolExecutor
from pathlib import Path
import re
import subprocess

ROOT = Path(__file__).resolve().parents[2]


def run(args, cwd=ROOT):
    result = subprocess.run([str(arg) for arg in args], cwd=cwd, capture_output=True, text=True)
    if result.returncode:
        raise RuntimeError(f'{args[0]} failed: {result.stderr[-4000:]}')


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--output', type=Path, default=ROOT / 'ios/build-original-host/include')
    args = parser.parse_args()
    output = args.output.resolve(); output.mkdir(parents=True, exist_ok=True)
    overlays = re.findall(r'^Overlay\s+(\w+)\s*\n\{', (ROOT / 'main.lsf').read_text(), re.MULTILINE)
    if len(overlays) != len(set(overlays)) or not overlays:
        raise ValueError('Invalid original overlay registry')
    for index, name in enumerate(overlays):
        if name.startswith('OVY_') and int(name[4:]) != index:
            raise ValueError(f'Unexpected overlay order: {name}')
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
    run([ROOT / 'tools/nitroarc/nitroarc', '-cPf', output / 'msgdata/msg.narc', '-T', file_list, messages])
    items = output / 'itemtool/itemdata'; items.mkdir(parents=True, exist_ok=True)
    csv = ROOT / 'files/itemtool/itemdata/item_data.csv'
    manifest = csv.with_suffix('.txt')
    run([ROOT / 'tools/csv2bin/csv2bin', 'compile', csv, items / 'item_data.narc', manifest, '-i', ROOT / 'include', '--naix', '--pad', '0xFF'])
    icons = ROOT / 'files/itemtool/itemdata/item_icon'
    run([ROOT / 'tools/nitroarc/nitroarc', '-cPf', items / 'item_icon.narc', icons])
    print(f'Prepared original headers: {len(sources)} message banks, item records and item icons.')


if __name__ == '__main__':
    main()
