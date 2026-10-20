# SPDX-License-Identifier: GPL-3.0-or-later
"""Verify native synthesis and original sound resources without a DS CPU core."""
import hashlib
import json
from pathlib import Path
import re
import struct
import subprocess
from check_native_waves import sound_files
ROOT=Path(__file__).resolve().parents[2]
BUILD=ROOT/'ios/build-native-audio'
def integers(text,name):
    match=re.search(re.escape(name)+r'\[[^]]+\]\s*=\s*\{(.*?)\};',text,re.S); assert match,name
    return [int(n,0) if 'x' in n.lower() else int(n) for n in re.findall(r'-?0x[\dA-Fa-f]+|-?\d+',match[1].replace('//',''))]
def entries(data,kind):
    info=struct.unpack_from('<I',data,0x18)[0]; table=info+struct.unpack_from('<I',data,info+8+kind*4)[0]
    count=struct.unpack_from('<I',data,table)[0]
    for index in range(count):
        relative=struct.unpack_from('<I',data,table+4+index*4)[0]
        yield memoryview(data)[info+relative:] if relative else None
def main():
    BUILD.mkdir(exist_ok=True)
    report=BUILD/'synth-report.json'; report.unlink(missing_ok=True)
    original=(ROOT/'lib/NitroSDK/src/snd/SND_util.c').read_text(); native=(ROOT/'ios/Original/NativeSoundTables.inc').read_text()
    for a,b in [('sPitchTable','pitchTable'),('sVolumeTable','volumeTable'),('SNDi_DecibelSquareTable','squareTable')]:
        assert integers(original,a)==integers(native,b),(a,b)
    decibels=integers(original,'SNDi_DecibelTable'); pitch_table=integers(original,'sPitchTable')
    fixture=BUILD/'synth.bin'; records=0; images={}
    with fixture.open('wb') as output:
        output.write(b'\0'*4)
        for path in sorted((ROOT/'ios/OriginalContent/files').rglob('*.sdat')):
            raw=path.read_bytes(); images[str(path.relative_to(ROOT))]=hashlib.sha256(raw).hexdigest(); files=list(sound_files(path))
            banks=list(entries(raw,2)); waves=list(entries(raw,3))
            for seq in entries(raw,0):
                if seq is None: continue
                file_id,bank_id=struct.unpack_from('<IH',seq); sequence=files[file_id]
                if sequence[:4]!=b'SSEQ': continue
                info=banks[bank_id]; assert info is not None
                bank=files[struct.unpack_from('<I',info)[0]]; memory=bytearray(64)
                def append(blob):
                    memory.extend(b'\0'*((-len(memory))%4)); position=len(memory); memory.extend(blob); return position
                b=append(bank); table={}
                for slot,number in enumerate(struct.unpack_from('<4H',info,4)):
                    if number==65535: continue
                    if number not in table:
                        wave_info=waves[number]; assert wave_info is not None
                        block=files[struct.unpack_from('<I',wave_info)[0]]; assert block[:4]==b'SWAR'
                        table[number]=append(block)
                    struct.pack_into('<I',memory,b+24+slot*8,table[number])
                offset=struct.unpack_from('<I',sequence,24)[0]; commands=sequence[offset:]; start=append(commands)
                output.write(struct.pack('<6I',len(memory),start,len(commands),b,decibels[seq[6]]&0xFFFFFFFF,seq[7])); output.write(memory); records+=1
        cases=[]
        for base in [0,1,16,256,512,8006,65535]:
            for pitch in sorted(set(range(-4096,4097,17))|{0,-2**31,2**31-1}):
                octave,index=divmod(-pitch,768); shift=octave-16
                value=(base or 65536)*(pitch_table[index]+65536)
                value=65535 if shift>=0 else 0 if -shift>=64 else value>>-shift
                want=base if pitch==0 else max(16,min(65535,value)); cases.append((base,pitch,want))
        output.write(struct.pack('<I',len(cases)))
        for row in cases: output.write(struct.pack('<IiI',*row))
        output.seek(0); output.write(struct.pack('<I',records))
    common=['-std=c11','-O2','-Wall','-Wextra','-Werror','-DHEARTGOLD_NATIVE','-I',ROOT/'lib/include']
    sources=[ROOT/'ios/Original'/name for name in ['NativeWave.c','NativeMixer.c','NativeInstrument.c','NativeSequence.c','NativeSynth.c']]
    binary=BUILD/'synth-check'; test=ROOT/'ios/Original/tests/synth_check.c'
    subprocess.run(['xcrun','clang',*common,'-fsanitize=address,undefined','-fno-sanitize-recover=all',*sources,test,'-o',binary],check=True)
    subprocess.run([binary,fixture],check=True)
    objects=[]; archive=BUILD/'libHeartGoldNativeSynth.a'; archive.unlink(missing_ok=True)
    for source in sources:
        target=BUILD/(source.stem+'-synth.o'); objects.append(target)
        subprocess.run(['xcrun','--sdk','iphoneos','clang','-target','arm64-apple-ios16.0',*common,'-c',source,'-o',target],check=True)
    subprocess.run(['xcrun','ar','rcs',archive,*objects],check=True)
    inputs=[*sources,*[ROOT/'ios/Original'/name for name in ['NativeWave.h','NativeMixer.h','NativeInstrument.h','NativeSequence.h','NativeSynth.h','NativeSoundTables.inc']],ROOT/'lib/include/nitro/snd/native_sound.h']
    report.write_text(json.dumps(dict(scope='native original-resource synthesis; full music fidelity and device output unverified',
        sourceImages=images,resourceSets=records,framesPerSet=6000,fixtureSha256=hashlib.sha256(fixture.read_bytes()).hexdigest(),
        nativeSources={str(p.relative_to(ROOT)):hashlib.sha256(p.read_bytes()).hexdigest() for p in inputs},
        deviceArchiveSha256=hashlib.sha256(archive.read_bytes()).hexdigest()),indent=2)+'\n')
if __name__=='__main__': main()
