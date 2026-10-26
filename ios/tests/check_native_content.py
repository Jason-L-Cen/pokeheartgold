# SPDX-License-Identifier: GPL-3.0-or-later
"""Small checks for native region binding, archive preservation and map identity gates."""
from pathlib import Path
import hashlib
import struct
import subprocess
import sys
import tempfile

ROOT=Path(__file__).resolve().parents[2]
sys.path.insert(0,str(ROOT/'ios/tools'))
from prepare_native_content import decode_code,locate_maps,locate_script_banks,map_header,script_header,message_identity,select_messages,write_narc,pokedex_sprite_members,pokedex_logo_screen,SOURCE_BASE
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

def check_pokedex_sprites():
    original_path=ROOT/'ios/OriginalContent/files/a/0/6/8'
    source=ROOT/'files/graphic/zukan_gra'; tool=ROOT/'tools/nitrogfx/nitrogfx'
    paths={12:source/'zukan_gra_00000012.png',13:source/'zukan_gra_00000013.NCER',14:source/'zukan_gra_00000014.NANR'}
    palette=source/'zukan_gra_00000015.NCLR'
    protected={path:path.read_bytes() for path in (*paths.values(),palette)}
    if original_path.is_file(): protected[original_path]=original_path.read_bytes()
    original=[bytes([i]) for i in range(123)]; original[15]=protected[palette]+bytes(2)
    assert len(original)==123 and len(protected[palette])==226
    assert original[15]==protected[palette]+bytes(2)
    assert struct.unpack_from('>IIB',protected[paths[12]],16)==(8,3376,4)
    with tempfile.TemporaryDirectory(prefix='native-pokedex-sprites-') as folder:
        destination=Path(folder)
        def packed_member(name,data):
            raw=destination/name; packed=raw.with_suffix(raw.suffix+'.lz'); raw.write_bytes(data)
            subprocess.run([str(tool),str(raw),str(packed),'-nopad'],check=True,capture_output=True,timeout=30)
            return packed.read_bytes()
        tiles=tuple(0x9000|((index%4)<<10)|index for index in range(1,31))
        small=struct.pack('<4sHHIHH4sIHHII',b'RCSN',0xFEFF,0x100,96,16,1,b'NRCS',80,80,24,0,60)+struct.pack('<30H',*tiles)
        normal=pokedex_logo_screen(small)
        assert struct.unpack_from('<IHH',normal,8)==(212,16,1) and normal[16:20]==b'NRCS'
        assert struct.unpack_from('<IHHII',normal,20)==(196,176,32,0,176)
        normalized_tiles=struct.unpack_from('<88H',normal,36)
        assert normalized_tiles==tuple(tiles[row*10+column-6] if row<3 and 6<=column<16 else 0x9001 for row in range(4) for column in range(22))
        assert pokedex_logo_screen(normal)==normal
        rejected(pokedex_logo_screen,small[:-1]); rejected(pokedex_logo_screen,small+bytes(1))
        for offset,fmt,value in ((0,'I',0),(8,'I',95),(12,'H',20),(14,'H',2),(16,'I',0),(20,'I',79),(24,'H',88),(26,'H',32),(28,'I',1),(32,'I',58)):
            invalid=bytearray(small); struct.pack_into('<'+fmt,invalid,offset,value); rejected(pokedex_logo_screen,bytes(invalid))
        character=struct.pack('<4sHHIHH4sIHHIIIII',b'RGCN',0xFEFF,0x101,128,16,2,b'RAHC',96,1,2,3,0,0,64,24)+bytes(32)+b'\x77'*32+b'SOPC'+struct.pack('<III',16,0,0)
        assert len(character)==128
        original[1]=packed_member('blank.NCGR',character)
        original[7]=packed_member('logo7.NSCR',small); original[8]=packed_member('logo8.NSCR',small)
        before=list(original)
        rejected(pokedex_sprite_members,destination,original[:-1])
        selected,proof=pokedex_sprite_members(destination,original)
        assert original==before and len(selected)==123
        assert [i for i in range(123) if selected[i]!=original[i]]==[7,8,12,13,14]
        assert selected[15]==original[15]
        archive=destination/'pokedex.narc'; archive.write_bytes(write_narc(selected))
        assert narc(archive)==selected
        assert proof['path']=='a/0/6/8' and proof['tool']==str(tool.relative_to(ROOT))
        assert proof['toolSha256']==hashlib.sha256(tool.read_bytes()).hexdigest()
        assert [row['member'] for row in proof['members']]==[12,13,14]
        decoded={}
        for row in proof['members']:
            index=row['member']; packed=destination/f'{index}.lz'; raw=destination/f'{index}.bin'
            assert selected[index][0]==0x10
            packed.write_bytes(selected[index])
            subprocess.run([str(tool),str(packed),str(raw)],check=True,capture_output=True,timeout=30)
            decoded[index]=raw.read_bytes()
            assert int.from_bytes(selected[index][1:4],'little')==len(decoded[index])
            assert row['source']==str(paths[index].relative_to(ROOT))
            assert row['sourceSha256']==hashlib.sha256(protected[paths[index]]).hexdigest()
            assert row['decodedSha256']==hashlib.sha256(decoded[index]).hexdigest()
            assert row['originalSha256']==hashlib.sha256(original[index]).hexdigest()
            assert row['nativeSha256']==hashlib.sha256(selected[index]).hexdigest()
        assert [row['member'] for row in proof['logoMembers']]==[7,8]
        for row in proof['logoMembers']:
            index=row['member']; packed=destination/f'logo-{index}.lz'; raw=destination/f'logo-{index}.NSCR'; packed.write_bytes(selected[index])
            subprocess.run([str(tool),str(packed),str(raw)],check=True,capture_output=True,timeout=30)
            assert raw.read_bytes()==normal
            assert row['source']=='rom' and row['sourceSizeTiles']==[10,3] and row['nativeSizeTiles']==[22,4] and row['paddingOffsetTiles']==[6,0]
            assert row['originalSha256']==hashlib.sha256(original[index]).hexdigest() and row['originalDecodedSha256']==hashlib.sha256(small).hexdigest()
            assert row['decodedSha256']==hashlib.sha256(normal).hexdigest() and row['nativeSha256']==hashlib.sha256(selected[index]).hexdigest()
            assert row['paddingTile']==0x9001 and row['blankCharacterMember']==1 and row['blankCharacterSha256']==hashlib.sha256(original[1]).hexdigest()
            assert row['blankCharacterDecodedSha256']==hashlib.sha256(character).hexdigest()
        unchanged=list(original); unchanged[7]=packed_member('normal7.NSCR',normal); unchanged[8]=packed_member('normal8.NSCR',normal)
        identity,identity_proof=pokedex_sprite_members(destination,unchanged)
        assert identity[7:9]==unchanged[7:9] and all(row['paddingOffsetTiles']==[0,0] and row['originalSha256']==row['nativeSha256'] for row in identity_proof['logoMembers'])
        for offset,value in ((80,0),(20,95),(28,4),(40,0xFFFFFFFF)):
            invalid=bytearray(character); struct.pack_into('<I',invalid,offset,value)
            bad=list(original); bad[1]=packed_member('invalid.NCGR',bytes(invalid)); rejected(pokedex_sprite_members,destination,bad)
        assert original==before
        # The real original converter must keep the source's tiled 4bpp/1D64/v1.1 recipe.
        character=decoded[12]
        assert len(character)==13552 and character[:8]==b'RGCN\xff\xfe\x01\x01'
        assert struct.unpack_from('<IHH',character,8)==(len(character),16,1)
        assert character[16:20]==b'RAHC' and struct.unpack_from('<I',character,20)[0]==len(character)-16
        assert struct.unpack_from('<HHIIIII',character,24)==(65535,65535,3,0x100010,0,422*32,24)
        assert decoded[13]==protected[paths[13]] and decoded[14]==protected[paths[14]]
        assert decoded[13][:4]==b'RECN' and struct.unpack_from('<H',decoded[13],24)[0]==65
        animation=decoded[14]; assert animation[:4]==b'RNAN'
        count,frames,sequences,frame_array,contents=struct.unpack_from('<HHIII',animation,24)
        assert count==69
        for index,cell in ((67,63),(68,64)):
            num,loop,kind,play,frame=struct.unpack_from('<HHIII',animation,24+sequences+16*index)
            content,duration,padding=struct.unpack_from('<IHH',animation,24+frame_array+frame)
            assert num==1 and kind==0x10000 and duration==4
            assert struct.unpack_from('<H',animation,24+contents+content)[0]==cell<65
    assert all(path.read_bytes()==data for path,data in protected.items())

def main():
    check_blz()
    check_pokedex_sprites()
    source=message([[65,65534,0x103,2,0,0,65535]])
    translated=message([[0x758,65534,0x103,1,0,65534,0x200,1,0,65535]])
    assert message_identity(source)==message_identity(translated)
    selected,choices=select_messages([source],[translated])
    assert selected==[translated] and choices[0]['native']==SOURCE_BASE and choices[0]['rom']==0
    wrong_slot=message([[65534,0x103,1,1,65535]])
    assert select_messages([source],[wrong_slot])[0]==[source]
    assert select_messages([source],[translated,translated])[1][0]['rom'] is None
    assert select_messages([source],[message([[65,65535]],42)])[0]==[source]
    # The actual formatter consumes field 0 identically for all four STRVAR classes.
    ordered=message([[65534,0x100,1,0,65534,0x3400,2,1,42,65535]])
    reordered=message([[65534,0x300,1,1,65534,0x400,3,0,17,18,65535]])
    assert select_messages([ordered],[reordered])[0]==[reordered]
    for wrong in [message([[65534,0x100,1,0,65534,0x100,1,2,65535]]),
                  message([[65534,0x100,1,0,65534,0x100,1,1,65534,0x100,1,1,65535]]),
                  message([[65534,0x100,1,0,65534,0x100,1,1,65535],[65535]]),
                  message([[65534,0x100,0,65534,0x100,1,1,65535]])]:
        assert select_messages([ordered],[wrong])[0]==[ordered]
        assert select_messages([ordered],[wrong],{0:{0}})[0]==[ordered]
    assert select_messages([ordered],[reordered,reordered])[0]==[ordered]
    assert select_messages([ordered],[reordered,reordered],{0:{1}})[1][0]['rom']==1
    assert select_messages([ordered],[reordered,reordered],{0:{0,1}})[0]==[ordered]
    different_key=message([[65534,0x300,1,1,65534,0x400,1,0,65535]],42)
    assert select_messages([ordered],[different_key])[0]==[ordered]
    selected,choice=select_messages([ordered],[different_key],{0:{0}})
    assert selected==[different_key] and choice[0]['method']=='map'
    assert select_messages([ordered],[reordered,different_key],{0:{0,1}})[0]==[ordered]
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
    print('PASS: all-row STRVAR read multisets, map bindings, NARC/BLZ identity, and original Pokédex sprite-group conversion with other members preserved.')

if __name__=='__main__': main()
