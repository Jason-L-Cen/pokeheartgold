# SPDX-License-Identifier: GPL-3.0-or-later
"""Check native instrument selection against every original SBNK program/key."""
import hashlib
import json
from pathlib import Path
import struct
import subprocess
from check_native_waves import sound_files

ROOT=Path(__file__).resolve().parents[2]
BUILD=ROOT/'ios/build-native-audio'

def program_notes(bank,program):
    entry=struct.unpack_from('<I',bank,60+program*4)[0]; kind=entry&255; offset=entry>>8
    notes=[None]*128
    if not kind: return notes
    assert offset>=60+struct.unpack_from('<I',bank,56)[0]*4
    if kind<=5:
        assert offset+10<=len(bank)
        notes=[bytes((kind,0))+bank[offset:offset+10]]*128
    elif kind==16:
        low,high=bank[offset:offset+2]; assert low<=high<=127
        for key in range(low,high+1):
            begin=offset+2+(key-low)*12
            notes[key]=bank[begin:begin+12]
    elif kind==17:
        low=0
        for i,high in enumerate(bank[offset:offset+8]):
            if not high: break
            assert low<=high<=127
            record=bank[offset+8+i*12:offset+20+i*12]
            for key in range(low,high+1): notes[key]=record
            low=high+1
            if high==127: break
    else: raise AssertionError(f'Unexpected instrument {kind}')
    for key,record in enumerate(notes):
        if record is not None:
            assert len(record)==12 and record[0]<=5
            if not record[0]: notes[key]=None
    return notes

def main():
    BUILD.mkdir(exist_ok=True); fixture=BUILD/'instruments.bin'; banks=comparisons=0; sources={}
    paths=sorted((ROOT/'ios/OriginalContent/files').rglob('*.sdat')); assert paths
    with fixture.open('wb') as output:
        output.write(b'\0'*4)
        for path in paths:
            sources[str(path.relative_to(ROOT))]=hashlib.sha256(path.read_bytes()).hexdigest()
            for bank in sound_files(path):
                if bank[:4]!=b'SBNK': continue
                count=struct.unpack_from('<I',bank,56)[0]
                assert 60+count*4<=len(bank) and struct.unpack_from('<I',bank,8)[0]==len(bank)
                output.write(struct.pack('<II',len(bank),count)); output.write(bank)
                for program in range(count):
                    for record in program_notes(bank,program):
                        output.write(b'\x01'+record if record is not None else b'\0'*13); comparisons+=1
                banks+=1
        output.seek(0); output.write(struct.pack('<I',banks))
    source=ROOT/'ios/Original/NativeInstrument.c'; test=ROOT/'ios/Original/tests/instrument_check.c'
    common=['-std=c11','-O2','-Wall','-Wextra','-Werror']; binary=BUILD/'instrument-check'
    subprocess.run(['xcrun','clang',*common,'-fsanitize=address,undefined','-fno-sanitize-recover=all',source,test,'-o',binary],check=True)
    subprocess.run([binary,fixture],check=True)
    device=BUILD/'NativeInstrument.o'; archive=BUILD/'libHeartGoldNativeInstrument.a'; archive.unlink(missing_ok=True)
    subprocess.run(['xcrun','--sdk','iphoneos','clang','-target','arm64-apple-ios16.0',*common,'-c',source,'-o',device],check=True)
    subprocess.run(['xcrun','ar','rcs',archive,device],check=True)
    (BUILD/'instrument-report.json').write_text(json.dumps(dict(scope='original bank instrument lookup; sequence playback not connected',
        sourceImages=sources,banks=banks,programKeyComparisons=comparisons,fixtureSha256=hashlib.sha256(fixture.read_bytes()).hexdigest(),
        nativeSourceSha256=hashlib.sha256(source.read_bytes()).hexdigest(),deviceArchiveSha256=hashlib.sha256(archive.read_bytes()).hexdigest()),indent=2)+'\n')

if __name__=='__main__': main()
