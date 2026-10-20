# SPDX-License-Identifier: GPL-3.0-or-later
"""Run bounded original SSEQ scheduling with native memory/UB sanitizers."""
import hashlib
import json
from pathlib import Path
import struct
import subprocess
from check_native_waves import sound_files
ROOT=Path(__file__).resolve().parents[2]
BUILD=ROOT/'ios/build-native-audio'
def main():
    BUILD.mkdir(exist_ok=True); fixture=BUILD/'sequences.bin'; records=0; sources={}
    with fixture.open('wb') as output:
        output.write(b'\0'*4)
        for path in sorted((ROOT/'ios/OriginalContent/files').rglob('*.sdat')):
            sources[str(path.relative_to(ROOT))]=hashlib.sha256(path.read_bytes()).hexdigest()
            for block in sound_files(path):
                if block[:4]!=b'SSEQ': continue
                assert len(block)>=28 and struct.unpack_from('<I',block,8)[0]==len(block)
                offset=struct.unpack_from('<I',block,24)[0]; assert 28<=offset<len(block)
                data=block[offset:]; output.write(struct.pack('<II',len(data),0)); output.write(data); records+=1
        output.seek(0); output.write(struct.pack('<I',records))
    assert records
    source=ROOT/'ios/Original/NativeSequence.c'; test=ROOT/'ios/Original/tests/sequence_check.c'; binary=BUILD/'sequence-check'
    common=['-std=c11','-O2','-Wall','-Wextra','-Werror']
    subprocess.run(['xcrun','clang',*common,'-fsanitize=address,undefined','-fno-sanitize-recover=all',source,test,'-o',binary],check=True)
    subprocess.run([binary,fixture],check=True)
    device=BUILD/'NativeSequence.o'; archive=BUILD/'libHeartGoldNativeSequence.a'; archive.unlink(missing_ok=True)
    subprocess.run(['xcrun','--sdk','iphoneos','clang','-target','arm64-apple-ios16.0',*common,'-c',source,'-o',device],check=True)
    subprocess.run(['xcrun','ar','rcs',archive,device],check=True)
    (BUILD/'sequence-report.json').write_text(json.dumps(dict(scope='bounded native SSEQ event scheduling; PCM/output and complete-song fidelity unverified',
        sourceImages=sources,sequences=records,maximumDriverClocks=32768,fixtureSha256=hashlib.sha256(fixture.read_bytes()).hexdigest(),
        nativeSourceSha256=hashlib.sha256(source.read_bytes()).hexdigest(),deviceArchiveSha256=hashlib.sha256(archive.read_bytes()).hexdigest()),indent=2)+'\n')
if __name__=='__main__': main()
