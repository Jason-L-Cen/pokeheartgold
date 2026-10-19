# SPDX-License-Identifier: GPL-3.0-or-later
"""Extract named NitroFS files for the native port without loading or executing ARM code."""
import argparse
import hashlib
import json
import os
from pathlib import Path, PurePosixPath
import struct
import tempfile

ROOT = Path(__file__).resolve().parents[2]


def narc(path):
    data = path.read_bytes()
    if data[:4] != b'NARC' or len(data) < 28:
        raise ValueError(f'Invalid NARC: {path}')
    fat = struct.unpack_from('<H', data, 12)[0]
    fat_size, count = struct.unpack_from('<IH', data, fat + 4)
    names = fat + fat_size
    image = names + struct.unpack_from('<I', data, names + 4)[0]
    if data[fat:fat + 4] != b'BTAF' or data[image:image + 4] != b'GMIF':
        raise ValueError(f'Invalid NARC sections: {path}')
    result = []
    for i in range(count):
        start, end = struct.unpack_from('<II', data, fat + 12 + 8 * i)
        if start > end or image + 8 + end > len(data):
            raise ValueError(f'Invalid NARC member: {path}:{i}')
        result.append(data[image + 8 + start:image + 8 + end])
    return result


def crc16(data):
    crc = 0xFFFF
    for value in data:
        crc ^= value
        for _ in range(8):
            crc = (crc >> 1) ^ (0xA001 if crc & 1 else 0)
    return crc


def index_rom(data):
    if len(data) < 512 or data[12:15] not in (b'IPK', b'IPG'):
        raise ValueError('Expected a HeartGold or SoulSilver DS image')
    if crc16(data[:0x15E]) != struct.unpack_from('<H', data, 0x15E)[0]:
        raise ValueError('Invalid DS header checksum')
    fnt_start, fnt_size, fat_start, fat_size = struct.unpack_from('<4I', data, 64)
    if not (8 <= fnt_size <= 4 * 1024 * 1024 and fat_size > 0 and fat_size % 8 == 0):
        raise ValueError('Invalid filename/allocation table sizes')
    for start, size in ((fnt_start, fnt_size), (fat_start, fat_size)):
        if start < 512 or start + size > len(data):
            raise ValueError('Filename/allocation table is outside the image')
    fnt = data[fnt_start:fnt_start + fnt_size]
    directory_count = struct.unpack_from('<H', fnt, 6)[0]
    if not 0 < directory_count <= 4096 or directory_count * 8 > len(fnt):
        raise ValueError('Invalid root directory')
    records, directories, names, file_ids = [], set(), set(), set()
    pending = [(0xF000, PurePosixPath(), None)]
    while pending:
        directory, path, expected_parent = pending.pop()
        index = directory - 0xF000
        if not 0 <= index < directory_count or directory in directories:
            raise ValueError('Invalid/cyclic directory reference')
        directories.add(directory)
        offset, file_id, parent = struct.unpack_from('<IHH', fnt, index * 8)
        if expected_parent is not None and parent != expected_parent:
            raise ValueError('Directory parent mismatch')
        if not directory_count * 8 <= offset < len(fnt):
            raise ValueError('Directory name table is out of bounds')
        while True:
            if offset >= len(fnt):
                raise ValueError('Unterminated directory')
            marker = fnt[offset]; offset += 1
            if marker == 0:
                break
            length = marker & 127
            if not length or offset + length > len(fnt):
                raise ValueError('Invalid filename length')
            name = fnt[offset:offset + length].decode('ascii'); offset += length
            if name in ('.', '..') or any(ord(c) < 32 or c in '/\\:' for c in name):
                raise ValueError('Unsafe filename')
            target = path / name
            if str(target).casefold() in names:
                raise ValueError('Duplicate filename')
            names.add(str(target).casefold())
            if marker & 128:
                if offset + 2 > len(fnt):
                    raise ValueError('Truncated child directory')
                child = struct.unpack_from('<H', fnt, offset)[0]; offset += 2
                pending.append((child, target, directory))
            else:
                if file_id >= fat_size // 8 or file_id in file_ids:
                    raise ValueError('Invalid/duplicate file ID')
                start, end = struct.unpack_from('<II', data, fat_start + file_id * 8)
                if start > end or start < 512 or end > len(data):
                    raise ValueError('File extent is outside the image')
                file_ids.add(file_id)
                records.append(dict(id=file_id, path=str(target), offset=start, size=end - start,
                                    sha256=hashlib.sha256(data[start:end]).hexdigest()))
                file_id += 1
    if len(directories) != directory_count:
        raise ValueError('Unreachable directories')
    metadata = dict(gameCode=data[12:16].decode('ascii'), revision=data[30],
                    title=data[:12].rstrip(b'\0').decode('ascii'), imageSize=len(data),
                    sha256=hashlib.sha256(data).hexdigest(), directoryCount=directory_count,
                    namedFiles=len(records), unnamedFileIds=fat_size // 8 - len(records))
    return metadata, sorted(records, key=lambda record: record['id'])


def store_boot_header(folder, data):
    header=data[:0x160]
    if len(header)!=0x160 or crc16(header[:0x15E])!=struct.unpack_from('<H',header,0x15E)[0]:
        raise ValueError('Invalid native boot header')
    target=folder/'boot-header.bin'
    with tempfile.TemporaryDirectory(prefix='boot-header-',dir=folder) as temporary:
        staged=Path(temporary)/'header'; staged.write_bytes(header)
        try:
            os.link(staged,target)
        except FileExistsError:
            if target.read_bytes()!=header: raise ValueError('Existing native boot header changed')


def read_boot_header(folder):
    header=(folder/'boot-header.bin').read_bytes()
    image=json.loads((folder/'manifest.json').read_text())['image']
    if (len(header)!=0x160 or header[12:15] not in (b'IPK',b'IPG') or
        crc16(header[:0x15E])!=struct.unpack_from('<H',header,0x15E)[0] or
        header[12:16].decode('ascii')!=image['gameCode'] or header[30]!=image['revision'] or
        header[:12].rstrip(b'\0').decode('ascii')!=image['title']):
        raise ValueError('Native boot header does not match the content pack')
    fnt,size,fat,fat_size=struct.unpack_from('<4I',header,0x40)
    if not (8<=size<=4*1024*1024 and fat_size>0 and fat_size%8==0):
        raise ValueError('Invalid boot filename/allocation table sizes')
    if any(offset<512 or offset+length>image['imageSize'] for offset,length in ((fnt,size),(fat,fat_size))):
        raise ValueError('Boot filename/allocation table is outside the original image')
    return header


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--rom', type=Path, required=True)
    parser.add_argument('--output', type=Path, default=ROOT / 'ios/OriginalContent')
    args = parser.parse_args()
    if not 512 <= args.rom.stat().st_size <= 512 * 1024 * 1024:
        raise ValueError('Unexpected DS image size')
    data = args.rom.read_bytes()
    metadata, records = index_rom(data)
    manifest = dict(format=1, image=metadata, files=records)
    if args.output.exists():
        previous = json.loads((args.output / 'manifest.json').read_text())
        if previous != manifest:
            raise ValueError('Output contains a different content pack; choose a new directory')
        for record in records:
            path = args.output / 'files' / record['path']
            if not path.is_file() or hashlib.sha256(path.read_bytes()).hexdigest() != record['sha256']:
                raise ValueError(f'Existing content changed: {record["path"]}')
        store_boot_header(args.output,data)
        print(f'Verified {len(records)} existing native content files ({metadata["gameCode"]}).')
        return
    args.output.parent.mkdir(parents=True, exist_ok=True)
    with tempfile.TemporaryDirectory(prefix='original-content-', dir=args.output.parent) as staging:
        folder = Path(staging) / 'pack'
        for record in records:
            path = folder / 'files' / record['path']; path.parent.mkdir(parents=True, exist_ok=True)
            path.write_bytes(data[record['offset']:record['offset'] + record['size']])
        (folder / 'manifest.json').write_text(json.dumps(manifest, ensure_ascii=False, indent=2) + '\n')
        store_boot_header(folder,data)
        folder.rename(args.output)
    print(f'Imported {len(records)} named NitroFS files ({metadata["gameCode"]}); ARM9/ARM7 images and unnamed overlay files were not extracted.')


if __name__ == '__main__':
    main()
