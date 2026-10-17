# SPDX-License-Identifier: GPL-3.0-or-later
"""Run with python3 ios/tests/check_battle_assets.py after exporting battle art."""
import hashlib
import json
from pathlib import Path
import struct
import sys
from PIL import Image

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools'))
from export_battle import ROOT, narc, screen

# Two hardware screen blocks, asymmetric tiles, both flips and a second palette bank.
palette = bytearray(40 + 64)
palette[:4] = b'RLCN'
struct.pack_into('<I', palette, 32, 64)
for index in range(32):
    struct.pack_into('<H', palette, 40 + 2 * index, index)
tilemap = bytearray(36 + 4096)
tilemap[:4] = b'RCSN'
struct.pack_into('<HH', tilemap, 24, 512, 256)
struct.pack_into('<I', tilemap, 32, 4096)
struct.pack_into('<H', tilemap, 38, 0x1C00)
struct.pack_into('<H', tilemap, 36 + 1024 * 2, 1)
for bpp in (4, 8):
    chars = bytearray(48)
    chars[:4] = b'RGCN'; chars[28] = 3 if bpp == 4 else 4
    values = [(x + y) % 16 for y in range(8) for x in range(8)] + [2] * 64
    pixels = bytes(values) if bpp == 8 else bytes(values[i] | values[i + 1] << 4 for i in range(0, 128, 2))
    struct.pack_into('<I', chars, 40, len(pixels)); chars.extend(pixels)
    image = screen(chars, palette, tilemap, transparent=True)
    assert image.getpixel((0, 0)) == (0, 0, 0, 0)
    assert image.getpixel((7, 7)) == (14 * 255 // 31, 0, 0, 255)
    assert image.getpixel((8, 0)) == ((30 if bpp == 4 else 14) * 255 // 31, 0, 0, 255)
    assert image.getpixel((256, 0)) == (2 * 255 // 31, 0, 0, 255)

resources = ROOT / 'ios/Resources'
expected = {'health-player': (128, 47), 'health-enemy': (128, 35)}
for time in ('day', 'evening', 'night'):
    expected[f'background-{time}'] = (256, 144)
    expected[f'ground-player-{time}'] = (204, 30)
    expected[f'ground-enemy-{time}'] = (126, 32)
for name, size in expected.items():
    with Image.open(resources / f'battle-{name}.png') as image:
        assert image.size == size, (name, image.size)
        assert image.getchannel('A').getextrema()[1] == 255
        if name.startswith('background'):
            assert image.getchannel('A').getextrema()[0] == 255
        else:
            assert image.getchannel('A').getextrema()[0] == 0
bottoms = json.loads((resources / 'battle-sprite-bottoms.json').read_text())
assert len(bottoms) == 14 and all(0 < value <= 1 for value in bottoms.values())
assert bottoms['pokemon-163-front'] == 62 / 80
archives = {number: narc(ROOT / f'files/a/0/0/{number}') for number in (7, 8)}
for source, digest in json.loads((resources / 'battle-sources.json').read_text()).items():
    if ':' in source:
        archive, member = source.split(':')
        data = archives[int(archive.rsplit('/', 1)[1])][int(member)]
    else:
        data = (ROOT / source).read_bytes()
    assert hashlib.sha256(data).hexdigest() == digest, source
print('Battle assets OK: 4/8bpp, flips, palette banks, hardware blocks, alpha, sprite baselines, source hashes.')
