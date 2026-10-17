# SPDX-License-Identifier: GPL-3.0-or-later
"""Export original HGSS battle graphics using the repository's nitrogfx tool."""
import argparse
import hashlib
from pathlib import Path
import struct
import subprocess
from PIL import Image
from export_assets import ROOT, narc, write_json


def screen(chars, palette, tilemap, transparent=False):
    assert chars[:4] == b'RGCN' and palette[:4] == b'RLCN' and tilemap[:4] == b'RCSN'
    assert chars[28] in (3, 4)
    bpp = 4 if chars[28] == 3 else 8
    width, height = struct.unpack_from('<HH', tilemap, 24)
    size = struct.unpack_from('<I', tilemap, 32)[0]
    assert width in (256, 512) and height in (256, 512) and size == width * height // 32
    entries = struct.unpack_from(f'<{size // 2}H', tilemap, 36)
    colors = []
    for (value,) in struct.iter_unpack('<H', palette[40:40 + struct.unpack_from('<I', palette, 32)[0]]):
        colors.append(tuple(((value >> shift) & 31) * 255 // 31 for shift in (0, 5, 10)))
    pixels = chars[48:48 + struct.unpack_from('<I', chars, 40)[0]]
    image = Image.new('RGBA', (width, height))
    for y in range(height):
        for x in range(width):
            # NSCR's 32x32 screen blocks are in hardware order, not wide scanlines.
            block = (y // 256 * (width // 256) + x // 256) * 1024
            entry = entries[block + (y % 256 // 8) * 32 + x % 256 // 8]
            tx, ty = x % 8, y % 8
            if entry & 1024:
                tx = 7 - tx
            if entry & 2048:
                ty = 7 - ty
            at = (entry & 1023) * (bpp * 8) + ty * bpp + tx * bpp // 8
            index = pixels[at] >> (4 * (tx % 2)) & 15 if bpp == 4 else pixels[at]
            color = colors[(entry >> 12) * 16 + index if bpp == 4 else index]
            image.putpixel((x, y), (*color, 0 if transparent and index == 0 else 255))
    return image


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--work', type=Path, required=True)
    parser.add_argument('--output', type=Path, default=ROOT / 'ios/Resources')
    args = parser.parse_args()
    args.work.mkdir(parents=True, exist_ok=True)
    args.output.mkdir(parents=True, exist_ok=True)
    tool = ROOT / 'tools/nitrogfx/nitrogfx'
    if not tool.exists():
        subprocess.run(['make', '-C', str(tool.parent)], check=True)
    archives = {n: narc(ROOT / f'files/a/0/0/{n}') for n in (7, 8)}
    used = {}

    def member(archive, index, extension):
        name = f'{archive}-{index}.{extension}'
        raw = archives[archive][index]
        path = args.work / name
        used[f'a/0/0/{archive}:{index}'] = hashlib.sha256(raw).hexdigest()
        if raw[0] == 16:
            compressed = path.with_suffix(path.suffix + '.lz')
            compressed.write_bytes(raw)
            subprocess.run([str(tool), str(compressed), str(path)], check=True)
        else:
            path.write_bytes(raw)
        return path

    for time, index in [('day', 176), ('evening', 177), ('night', 178)]:
        image = screen(member(7, 3, 'NCGR').read_bytes(), member(7, index, 'NCLR').read_bytes(), member(7, 2, 'NSCR').read_bytes())
        assert image.size == (512, 256)
        image.save(args.work / f'background-{time}.png')
        image.crop((0, 0, 256, 144)).save(args.output / f'battle-background-{time}.png')
        for side, char, cell in [('player', 127, 128), ('enemy', 130, 131)]:
            char = member(8, char, 'NCGR')
            cell = member(8, cell, 'NCER')
            palette = member(8, index - 175, 'NCLR')
            subprocess.run([str(tool), str(char), str(args.output / f'battle-ground-{side}-{time}.png'),
                            '-palette', str(palette), '-cell', str(cell), '-object'], check=True)
    for side, char, cell in [('player', 191, 190), ('enemy', 188, 187)]:
        subprocess.run([str(tool), str(member(8, char, 'NCGR')), str(args.output / f'battle-health-{side}.png'),
                        '-palette', str(member(8, 71, 'NCLR')), '-cell', str(member(8, cell, 'NCER')), '-object'], check=True)
    for path in args.output.glob('battle-*.png'):
        if 'ground' in path.name or 'health' in path.name:
            with Image.open(path) as source:
                image = source.convert('RGBA')
                image.crop(image.getbbox()).save(path)
    chars, palette = member(7, 28, 'NCGR').read_bytes(), member(7, 246, 'NCLR').read_bytes()
    for name, index in [('commands', 36), ('moves', 37), ('base', 41)]:
        screen(chars, palette, member(7, index, 'NSCR').read_bytes(), transparent=True).save(args.work / f'{name}.png')
    with Image.open(args.work / 'commands.png') as commands, Image.open(args.work / 'moves.png') as moves:
        for name, source, box in [('fight', commands, (24, 40, 232, 128)),
                                  ('bag', commands, (1, 145, 79, 191)),
                                  ('party', moves, (130, 25, 254, 79)),
                                  ('escape', commands, (177, 145, 255, 191)),
                                  ('move', moves, (2, 25, 126, 79))]:
            source.crop(box).save(args.output / f'battle-button-{name}.png')
    item = ROOT / 'files/itemtool/itemdata/item_icon'
    icon, palette = item / 'item_icon_008.NCGR', item / 'item_icon_009.NCLR'
    subprocess.run([str(tool), str(icon), str(args.output / 'battle-ball.png'), '-palette', str(palette), '-width', '4', '-object'], check=True)
    with Image.open(args.output / 'battle-ball.png') as source:
        image = source.convert('RGBA')
        image.crop(image.getbbox()).save(args.output / 'battle-ball.png')
    for path in (icon, palette):
        used[str(path.relative_to(ROOT))] = hashlib.sha256(path.read_bytes()).hexdigest()
    bottoms = {}
    for number, side in ((number, side) for number in (16, 19, 152, 155, 158, 161, 163) for side in ('front', 'back')):
        path = ROOT / f'files/poketool/pokegra/pokegra/{number:04}/male/{side}.png'
        with Image.open(path) as source:
            assert source.mode == 'P' and source.size == (160, 80)
            frame = source.crop((0, 0, 80, 80))
            background = frame.getpixel((0, 0))
            bounds = frame.point(lambda index: 0 if index == background else 255).getbbox()
            assert bounds is not None and 0 < bounds[3] <= 80
            bottoms[f'pokemon-{number}-{side}'] = bounds[3] / 80
        used[str(path.relative_to(ROOT))] = hashlib.sha256(path.read_bytes()).hexdigest()
    assert len(bottoms) == 14
    write_json(args.output / 'battle-sprite-bottoms.json', bottoms)
    write_json(args.output / 'battle-sources.json', used)
    print(f'Exported original battle backgrounds, grass platforms and health boxes ({len(used)} source members).')


if __name__ == '__main__':
    main()
