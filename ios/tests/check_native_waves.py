# SPDX-License-Identifier: GPL-3.0-or-later
"""Compare native wave decoding with independent complete SDAT waveform fixtures."""
import hashlib
import json
from pathlib import Path
import struct
import subprocess
import array
import sys

ROOT=Path(__file__).resolve().parents[2]
BUILD=ROOT/'ios/build-native-audio'
STEPS=(7,8,9,10,11,12,13,14,16,17,19,21,23,25,28,31,34,37,41,45,50,55,60,66,73,80,88,97,107,118,
       130,143,157,173,190,209,230,253,279,307,337,371,408,449,494,544,598,658,724,796,876,963,
       1060,1166,1282,1411,1552,1707,1878,2066,2272,2499,2749,3024,3327,3660,4026,4428,4871,
       5358,5894,6484,7132,7845,8630,9493,10442,11487,12635,13899,15289,16818,18500,20350,
       22385,24623,27086,29794,32767)

def reference(blob):
    kind,loop,rate,timer,start,tail=struct.unpack_from('<BBHHHI',blob)
    length=(start+tail)*4; data=blob[12:12+length]; assert len(data)==length
    if kind==0:
        samples=array.array('h',((n if n<128 else n-256)*256 for n in data)); loop_frame=start*4
    elif kind==1:
        samples=array.array('h'); samples.frombytes(data)
        if sys.byteorder!='little': samples.byteswap()
        loop_frame=start*2
    else:
        assert kind==2 and len(data)>=4
        predictor=struct.unpack_from('<h',data)[0]; index=data[2]&127; assert index<=88
        samples=array.array('h'); loop_frame=(start-1)*8 if start else 0
        for byte in data[4:]:
            for nibble in (byte&15,byte>>4):
                magnitude=((nibble%8)*2+1)*STEPS[index]//8
                predictor=max(-32768,min(32767,predictor+(-magnitude if nibble>=8 else magnitude)))
                index=max(0,min(88,index+(-1 if nibble%8<4 else (2,4,6,8)[nibble%8-4])))
                samples.append(predictor)
    if sys.byteorder!='little': samples.byteswap()
    return samples.tobytes(),loop_frame

def waves(path):
    data=path.read_bytes(); assert data[:4]==b'SDAT'
    fat=struct.unpack_from('<I',data,0x20)[0]; assert data[fat:fat+4]==b'FAT '
    count=struct.unpack_from('<I',data,fat+8)[0]; assert count<=100000 and fat+12+count*16<=len(data)
    for i in range(count):
        start,size=struct.unpack_from('<II',data,fat+12+i*16); assert start+size<=len(data)
        block=data[start:start+size]
        if block[:4]!=b'SWAR': continue
        n=struct.unpack_from('<I',block,0x38)[0]; assert 0x3C+n*4<=len(block)
        for j in range(n):
            offset=struct.unpack_from('<I',block,0x3C+j*4)[0]
            if not offset: continue
            assert offset+12<=len(block)
            begin,tail=struct.unpack_from('<HI',block,offset+6); length=12+(begin+tail)*4
            assert offset+length<=len(block)
            yield block[offset:offset+length]

def main():
    BUILD.mkdir(exist_ok=True); fixture=BUILD/'waves.bin'; records=0; digests={}
    paths=sorted((ROOT/'ios/OriginalContent/files').rglob('*.sdat')); assert paths
    with fixture.open('wb') as output:
        output.write(b'\0'*4)
        for path in paths:
            digests[str(path.relative_to(ROOT))]=hashlib.sha256(path.read_bytes()).hexdigest()
            for blob in waves(path):
                pcm,loop=reference(blob); output.write(struct.pack('<III',len(blob),len(pcm)//2,loop)); output.write(blob); output.write(pcm); records+=1
        output.seek(0); output.write(struct.pack('<I',records))
    common=['-std=c11','-O2','-Wall','-Wextra','-Werror']
    sources=[ROOT/'ios/Original/NativeWave.c',ROOT/'ios/Original/NativeMixer.c']; test=ROOT/'ios/Original/tests/wave_check.c'; binary=BUILD/'wave-check'
    subprocess.run(['xcrun','clang',*common,'-fsanitize=address,undefined','-fno-sanitize-recover=all',*sources,test,'-o',binary],check=True)
    subprocess.run([binary,fixture],check=True)
    objects=[]; archive=BUILD/'libHeartGoldNativeWave.a'; archive.unlink(missing_ok=True)
    for source in sources:
        device=BUILD/(source.stem+'.o'); objects.append(device)
        subprocess.run(['xcrun','--sdk','iphoneos','clang','-target','arm64-apple-ios16.0',*common,'-c',source,'-o',device],check=True)
    subprocess.run(['xcrun','ar','rcs',archive,*objects],check=True)
    (BUILD/'wave-report.json').write_text(json.dumps(dict(scope='offline native wave decoding and PCM mixer; sequence/output not yet connected',
        sourceImages=digests,waveforms=records,fixtureSha256=hashlib.sha256(fixture.read_bytes()).hexdigest(),
        nativeSources={str(p.relative_to(ROOT)):hashlib.sha256(p.read_bytes()).hexdigest() for p in sources},
        deviceArchiveSha256=hashlib.sha256(archive.read_bytes()).hexdigest()),indent=2)+'\n')
    print('iPhoneOS native wave decoder:',archive)

if __name__=='__main__': main()
