# SPDX-License-Identifier: GPL-3.0-or-later
"""Check original FontData against supplied appended glyphs and independent 2bpp goldens."""
import hashlib
import json
from pathlib import Path
import struct
import subprocess
import sys

ROOT=Path(__file__).resolve().parents[2]
sys.path.insert(0,str(ROOT/'ios/tools'))
from import_original_content import narc
BUILD=ROOT/'ios/build-original-font'

def golden(font,index):
    header,offset,count,width,height,columns,rows=struct.unpack_from('<IIIBBBB',font)
    result=bytearray([0xA5]*128)
    for tile in range(columns*rows):
        source=header+(index*columns*rows+tile)*16
        destination=(tile//columns)*64+(tile%columns)*32
        for y in range(8):
            word=struct.unpack_from('<H',font,source+y*2)[0]
            pixels=[(0,1,2,0)[(word>>(14-2*x))&3] for x in range(8)]
            for x in range(4): result[destination+y*4+x]=pixels[x*2]|pixels[x*2+1]<<4
    return result+bytes([font[offset+(index if index<count else 427)],height])

def synthetic(columns,rows):
    count=428; glyphs=430; size=16*columns*rows; end=16+size*glyphs
    return struct.pack('<IIIBBBB',16,end,count,columns*8,rows*8,columns,rows)+bytes((i*17+i//7)&255 for i in range(size*glyphs))+bytes(5+i%7 for i in range(count))

def main():
    BUILD.mkdir(exist_ok=True); source=ROOT/'ios/OriginalContent/files/a/0/1/6'
    records=[(index,data) for index,data in enumerate(narc(source)[:5])]
    records += [(100+columns*2+rows,synthetic(columns,rows)) for columns in (1,2) for rows in (1,2)]
    fixture=BUILD/'fonts.bin'
    with fixture.open('wb') as output:
        output.write(struct.pack('<II',0x54464748,len(records)))
        for index,data in records:
            header,end,count,_,_,columns,rows=struct.unpack_from('<IIIBBBB',data)
            assert end>=header and (end-header)%(16*columns*rows)==0
            glyphs=(end-header)//(16*columns*rows)
            assert 428<=count<=glyphs<65535 and count<=len(data)-end
            output.write(struct.pack('<IIII',index,len(data),glyphs,count)); output.write(data)
            for glyph in range(glyphs): output.write(golden(data,glyph))
    includes=['ios/build-original-host/include','include','files','src','lib/include','lib/include/cw','lib/include/nitro','lib/include/nnsys','lib/include/nitro/os','include/library']
    flags=['-std=c11','-O1','-g','-fms-extensions','-Wno-unknown-pragmas','-Wno-microsoft','-DHEARTGOLD_NATIVE','-DSDK_ARM9','-DSDK_CW','-DSDK_CODE_ARM','-DSDK_TS','-D_NITRO','-DPM_KEEP_ASSERTS','-DPLATFORM_INTRINSIC_FUNCTION_BIT_CLZ32=__builtin_clz']
    for include in includes: flags += ['-I',str(ROOT/include)]
    binary=BUILD/'font-check'; sources=[ROOT/name for name in ['src/font_data.c','src/text.c','src/string_control_code.c','ios/Original/tests/font_check.c']]
    result=subprocess.run(['xcrun','clang',*flags,'-fsanitize=address,undefined','-fno-sanitize-recover=all','-Wl,-dead_strip',*sources,'-o',binary],cwd=ROOT,capture_output=True,text=True)
    (BUILD/'compile.log').write_text(result.stdout+result.stderr)
    if result.returncode: raise RuntimeError(f'Font build failed: {BUILD/"compile.log"}')
    subprocess.run([binary,fixture],check=True)
    bad=BUILD/'invalid-font.bin'; original=fixture.read_bytes(); offset=24
    invalid=[(0,'I',len(records[0][1])+1),(14,'B',0),(15,'B',3),(8,'I',427),
             (4,'I',len(records[0][1])+1),(4,'I',16+509*64+1),(8,'I',4510),(4,'I',0)]
    for field,kind,value in invalid:
        data=bytearray(original); struct.pack_into('<'+kind,data,offset+field,value); bad.write_bytes(data)
        result=subprocess.run([binary,bad],capture_output=True,text=True)
        if result.returncode!=86 or 'Rejected font ' not in result.stderr: raise AssertionError(f'Font header accepted or failed outside its guard: {field}/{value}: {result.stderr[-600:]}')
    bad.unlink()
    print(f'PASS: {len(invalid)} malformed font headers rejected before glyph access.')
    (BUILD/'font-report.json').write_text(json.dumps(dict(scope='original bitmap/width decoding; no message-ID or game fidelity verification',fonts=len(records),malformedHeaders=len(invalid),sourceSha256=hashlib.sha256(source.read_bytes()).hexdigest(),fixtureSha256=hashlib.sha256(fixture.read_bytes()).hexdigest()),indent=2)+'\n')

if __name__=='__main__': main()
