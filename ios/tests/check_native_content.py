# SPDX-License-Identifier: GPL-3.0-or-later
"""Small checks for native region binding, archive preservation and map identity gates."""
from pathlib import Path
import struct
import sys
import tempfile

ROOT=Path(__file__).resolve().parents[2]
sys.path.insert(0,str(ROOT/'ios/tools'))
from prepare_native_content import decode_code,locate_maps,locate_script_banks,map_header,script_header,message_identity,select_messages,write_narc,SOURCE_BASE
from import_original_content import narc

def rejected(function,*arguments):
    try: function(*arguments)
    except ValueError: return
    raise AssertionError('Invalid input was accepted')

def message(rows,key=41):
    offset=4+8*len(rows); table=bytearray(); payload=bytearray()
    for row,words in enumerate(rows):
        mask=key*765*(row+1)&65535; mask|=mask<<16
        table.extend(struct.pack('<II',offset^mask,len(words)^mask))
        payload.extend(struct.pack('<'+'H'*len(words),*[value^(((row+1)*596947+index*18749)&65535) for index,value in enumerate(words)]))
        offset+=len(words)*2
    return struct.pack('<HH',len(rows),key)+table+payload

def check_blz():
    remaining=1000; produced=0; stream=[]
    while remaining:
        tokens=[]; flags=0
        for bit in range(8):
            if not remaining: break
            count=min(18,remaining) if produced>=3 else 1
            if count>=3: flags|=128>>bit; tokens.extend([(count-3)<<4,0])
            else: count=1; tokens.append(0)
            remaining-=count; produced+=count
        stream.extend([flags,*tokens])
    packed=bytes(reversed(stream)); size=len(packed)+8
    fixture=packed+size.to_bytes(3,'little')+bytes([8])+struct.pack('<I',1000-size)
    assert decode_code(fixture)==bytes(1000)
    assert decode_code(bytes(8))==bytes(8)
    rejected(decode_code,bytes(7)); rejected(decode_code,b'\0'*7+b'\1')
    rejected(decode_code,bytes([0,0,128])+bytes([11,0,0,8])+struct.pack('<I',5))

def main():
    check_blz()
    source=message([[65,65534,0x103,2,0,0,65535]])
    translated=message([[0x758,65534,0x103,1,0,65534,0x200,1,0,65535]])
    assert message_identity(source)==message_identity(translated)
    selected,choices=select_messages([source],[translated])
    assert selected==[translated] and choices[0]['native']==SOURCE_BASE and choices[0]['rom']==0
    wrong_slot=message([[65534,0x103,1,1,65535]])
    assert select_messages([source],[wrong_slot])[0]==[source]
    assert select_messages([source],[translated,translated])[1][0]['rom'] is None
    assert select_messages([source],[message([[65,65535]],42)])[0]==[source]
    rejected(message_identity,bytes(3)); rejected(message_identity,source[:10])
    rejected(message_identity,message([[65]])); rejected(message_identity,message([[65534,0x103,8,0,65535]]))
    records=[struct.pack('<BB9HI',i+1,2,3,4,5,6,7,8,9,10,11,0xF0000000) for i in range(4)]
    baseline=b''.join(records); original=bytearray(baseline)
    for offset in range(0,len(original),24):
        struct.pack_into('<HHH',original,offset+6,15,16,4); struct.pack_into('<H',original,offset+16,20)
        original[offset+23]^=0x40
    code=bytes(32)+original+bytes(32)
    offset,table=locate_maps(code,baseline,10); assert offset==32 and table==original
    assert '.msgBank=4' in map_header(table) and '.scriptsBank=15' in map_header(table)
    rejected(locate_maps,code+original,baseline,10)
    wrong=bytearray(code); wrong[32]^=1; rejected(locate_maps,wrong,baseline,10)
    wrong=bytearray(code); struct.pack_into('<H',wrong,42,10); rejected(locate_maps,wrong,baseline,10)
    script_source=struct.pack('<6H',5000,8,9,4000,7,8); script_rom=struct.pack('<6H',5000,3,4,4000,2,3)
    assert locate_script_banks(bytes(16)+script_rom,script_source,10,10)==(16,script_rom)
    assert '{5000,3,4}' in script_header(script_rom)
    rejected(locate_script_banks,script_rom+script_rom,script_source,10,10)
    rejected(locate_script_banks,script_rom,script_source,3,10)
    with tempfile.TemporaryDirectory() as folder:
        path=Path(folder)/'messages.narc'; members=[source,translated,b'',b'abc']; path.write_bytes(write_narc(members)); assert narc(path)==members
        combined=[translated,*([b'']*(SOURCE_BASE-1)),source]; path.write_bytes(write_narc(combined))
        decoded=narc(path); assert decoded[0]==translated and decoded[SOURCE_BASE]==source and len(decoded)==SOURCE_BASE+1
    print('PASS: message identity/placeholder gates, ambiguous-source fallback, separate ID ranges, NARC bytes, BLZ references, and unique ROM map identity checks.')

if __name__=='__main__': main()
