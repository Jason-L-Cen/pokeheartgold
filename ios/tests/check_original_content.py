# SPDX-License-Identifier: GPL-3.0-or-later
import struct
import sys
import tempfile
import json
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools'))
from import_original_content import crc16, index_rom, store_boot_header, read_boot_header


def image(root_name=b'root.bin', child=0xF001):
    rom = bytearray(1024)
    rom[:12] = b'POKEMON HG\0\0'; rom[12:16] = b'IPKJ'
    root_names = bytes([len(root_name)]) + root_name + b'\x81a' + struct.pack('<H', child) + b'\0'
    fnt = struct.pack('<IHHIHH', 16, 0, 2, 16 + len(root_names), 1, 0xF000) + root_names + b'\x05entry\0'
    struct.pack_into('<4I', rom, 64, 512, len(fnt), 640, 16)
    rom[512:512 + len(fnt)] = fnt
    struct.pack_into('<4I', rom, 640, 768, 774, 784, 788)
    rom[768:774] = b'native'; rom[784:788] = b'data'
    struct.pack_into('<H', rom, 0x15E, crc16(rom[:0x15E]))
    return rom


metadata, files = index_rom(image())
assert metadata['gameCode'] == 'IPKJ' and metadata['directoryCount'] == 2
assert [(f['id'], f['path'], f['size']) for f in files] == [(0, 'root.bin', 6), (1, 'a/entry', 4)]
bad_extent = image(); struct.pack_into('<I', bad_extent, 644, 2000)
bad_directory = image(child=0xF000)
bad_header = image(); bad_header[0] ^= 1
bad_parent = image(); struct.pack_into('<H', bad_parent, 526, 0xF002)
for invalid in (b'', image(b'..'), image(b'/etc'), image(b'a\\b'), bad_extent, bad_directory, bad_header, bad_parent):
    try:
        index_rom(invalid)
    except (ValueError, UnicodeError):
        continue
    raise AssertionError('Malformed/unsafe input accepted')
with tempfile.TemporaryDirectory() as temporary:
    folder=Path(temporary); data=image()
    (folder/'manifest.json').write_text(json.dumps({'image':index_rom(data)[0]}))
    store_boot_header(folder,data); store_boot_header(folder,data)
    assert read_boot_header(folder)==data[:0x160]
    assert (folder/'boot-header.bin').read_bytes()==data[:0x160]
    before=(folder/'boot-header.bin').read_bytes()
    other=image(); other[30]=1; struct.pack_into('<H',other,0x15E,crc16(other[:0x15E]))
    for invalid in (bad_header,other):
        try: store_boot_header(folder,invalid)
        except ValueError: pass
        else: raise AssertionError('Changed/invalid boot header accepted')
        assert (folder/'boot-header.bin').read_bytes()==before
    assert sorted(p.name for p in folder.iterdir())==['boot-header.bin','manifest.json']
    out_of_range=bytearray(before); struct.pack_into('<I',out_of_range,0x40,2000)
    struct.pack_into('<H',out_of_range,0x15E,crc16(out_of_range[:0x15E]))
    for invalid in (before[:-1],bad_header[:0x160],other[:0x160],out_of_range):
        (folder/'boot-header.bin').write_bytes(invalid)
        try: read_boot_header(folder)
        except ValueError: pass
        else: raise AssertionError('Mismatched/unsafe runtime boot metadata accepted')
print('PASS: original content filenames, nested directories, IDs, extents, header checksum and malformed-input rejection.')
print('PASS: atomic native boot metadata extraction, idempotent import and rejection without overwriting changed data.')
