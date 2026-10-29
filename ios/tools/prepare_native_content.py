# SPDX-License-Identifier: GPL-3.0-or-later
"""Keep ROM script IDs intact while binding source UI message IDs to compatible banks."""
import collections
import hashlib
import json
import os
from pathlib import Path
import re
import shutil
import struct
import subprocess

from build_original_data import wasm_object
from import_original_content import narc,read_boot_header
from prepare_original_headers import run

ROOT=Path(__file__).resolve().parents[2]
SOURCE_BASE=1024
MESSAGE_PATH=Path('a/0/2/7')
POKEDEX_GRAPHICS_PATH=Path('a/0/6/8')
# Original ARM9/overlay source proof: message-bank-level-up-source-proof-r1.json (314df16b6a8b5e4b8643018e825e99d95f2350a9cb78fb333ae881aed11556dd).
ENTRY_PROJECTION_ROM_SHA256='3bd743e8ab963a6fe56669fbf49c4bfebd22418498d0f9f2ae177f33ee68d101'
ENTRY_PROJECTIONS={
    40:(38,'5c71338a091b23e622ce653d5626e1ee93c0f9e82cb5598d2fb16c5694d32bc2','f91f8093e6ed5349c70eefc4cd42a4d5a0809198bfa47c9db74d1c5be3f66bec',((15,15),(21,21),(81,80),(82,81))),
    197:(195,'cd45ea9f6ad249508c562c7c8967ee3929bec616a6aaa2034d329131180780bf','fe1bbf650b0618262966def2c746e6970155b18db263aca933a446709f27b7ab',((867,867),(1226,1226),(1227,1227),(921,921),(3,3))),
    423:(416,'91adc9e75d88b21436e056f0bf2f8a1598f92f7d9966da754c7f986e94f081b4','d4a12dd5685c420e449a8689d84619235e85db5150144bcd35380e6b801dd156',((5,5),(6,6),(8,8))),
    664:(655,'ab6f7dcca6c372b4a9e16216fff55a475f0ac3eecb4aadd5b036d64b6f46dfa7','490a5f6db29643b66079d343338edebedd4e5d7e57414dc2419648d6bbdca98c',((0,0),)),
}

def sha(data): return hashlib.sha256(data).hexdigest()

def decode_code(data):
    if len(data)<8: raise ValueError('Short BLZ footer')
    extra=struct.unpack_from('<I',data,len(data)-4)[0]
    if not extra: return data
    packed=int.from_bytes(data[-8:-5],'little'); header=data[-5]
    if not 8<=header<=packed<=len(data) or extra>16*1024*1024: raise ValueError('Invalid BLZ bounds')
    out=bytearray(data)+bytearray(extra); src=len(data)-header; dst=len(out); stop=len(data)-packed
    while src>stop:
        src-=1; flags=out[src]
        for mask in (128,64,32,16,8,4,2,1):
            if src<=stop: break
            if flags&mask:
                if src-stop<2: raise ValueError('Short BLZ reference')
                src-=1; high=out[src]; src-=1; low=out[src]
                count=(high>>4)+3; distance=((high&15)<<8|low)+3
                if count>dst-stop or dst-1+distance>=len(out): raise ValueError('Invalid BLZ reference')
                for _ in range(count): dst-=1; out[dst]=out[dst+distance]
            else:
                if dst<=stop: raise ValueError('BLZ output underflow')
                src-=1; dst-=1; out[dst]=out[src]
    if dst!=stop: raise ValueError('Incomplete BLZ output')
    return bytes(out)

def message_identity(data):
    if len(data)<4: raise ValueError('Short message header')
    count,key=struct.unpack_from('<HH',data)
    if 4+count*8>len(data): raise ValueError('Short message allocation table')
    placeholders=[]
    for row in range(count):
        mask=key*765*(row+1)&65535; mask|=mask<<16
        start,length=struct.unpack_from('<II',data,4+row*8); start^=mask; length^=mask
        if start<4+count*8 or start>len(data) or length>(len(data)-start)//2: raise ValueError('Invalid message extent')
        words=[value^(((row+1)*596947+index*18749)&65535) for index,value in enumerate(struct.unpack_from('<'+'H'*length,data,start))]
        if not words or 65535 not in words: raise ValueError('Unterminated message')
        # Packed 9-bit literal strings contain no extended commands.
        if words[0]==0xF100: placeholders.append(()); continue
        slots=[]; position=0
        while words[position]!=65535:
            if words[position]==65534:
                if position+3>len(words): raise ValueError('Short message control')
                code,arguments=words[position+1:position+3]; end=position+3+arguments
                if end>len(words): raise ValueError('Short message control arguments')
                if code&0xFF00 in (0x100,0x300,0x400,0x3400):
                    slots.append(words[position+3] if arguments else None)
                position=end
            else: position+=1
            if position>=len(words): raise ValueError('Missing message terminator')
        # StringExpandPlaceholders reads only field 0 for all four STRVAR classes.
        # Translation can reorder those reads; retain their multiplicity and every row.
        placeholders.append(tuple(sorted(slots,key=lambda slot:-1 if slot is None else slot)))
    return count,key,tuple(placeholders)

def message_row(data,row):
    if len(data)<4: raise ValueError('Short message header')
    count,key=struct.unpack_from('<HH',data)
    if not 0<=row<count or 4+count*8>len(data): raise ValueError('Invalid message row/table')
    mask=key*765*(row+1)&65535; mask|=mask<<16
    start,length=struct.unpack_from('<II',data,4+row*8); start^=mask; length^=mask
    if start%2 or start<4+count*8 or start>len(data) or length>(len(data)-start)//2: raise ValueError('Invalid message extent')
    words=tuple(value^(((row+1)*596947+index*18749)&65535) for index,value in enumerate(struct.unpack_from('<'+'H'*length,data,start)))
    if not words or words[-1]!=65535: raise ValueError('Unterminated message row')
    if words[0]==0xF100:
        bits=0; packed=0
        for word in words[1:]:
            packed|=(word&0x7FFF)<<bits; bits+=15
            while bits>=9:
                value=packed&0x1FF; packed>>=9; bits-=9
                if value==0x1FF: return words,()
        raise ValueError('Unterminated packed message')
    slots=[]; position=0
    while position<len(words) and words[position]!=65535:
        if words[position]==0xF100: raise ValueError('Embedded packed message')
        if words[position]==65534:
            if position+3>len(words): raise ValueError('Short message control')
            code,arguments=words[position+1:position+3]; end=position+3+arguments
            if end>=len(words): raise ValueError('Short message control arguments/terminator')
            if code&0xFF00 in (0x100,0x300,0x400,0x3400):
                if not arguments: raise ValueError('STRVAR has no input slot')
                slots.append(words[position+3])
            position=end
        else: position+=1
    if position>=len(words): raise ValueError('Missing message terminator')
    return words,tuple(sorted(slots))

def project_message_entries(source,original,entries):
    if len(source)<4 or len(source)%2: raise ValueError('Invalid projection source bank')
    result=bytearray(source); key=struct.unpack_from('<H',source,2)[0]
    for source_id,rom_id in entries:
        _,source_slots=message_row(source,source_id); words,rom_slots=message_row(original,rom_id)
        if source_slots!=rom_slots: raise ValueError('Entry projection changes required input slots')
        start=len(result)
        # Re-key raw words for their source ID; unknown rows retain their original table and ciphertext.
        result.extend(struct.pack('<'+'H'*len(words),*(value^(((source_id+1)*596947+index*18749)&65535) for index,value in enumerate(words))))
        mask=key*765*(source_id+1)&65535; mask|=mask<<16
        struct.pack_into('<II',result,4+source_id*8,start^mask,len(words)^mask)
    return bytes(result)

def select_messages(source,original,map_banks=None,image_sha256=None):
    candidates=collections.defaultdict(list)
    identities=[message_identity(data) for data in original]
    for index,identity in enumerate(identities): candidates[identity].append(index)
    output=[]; choices=[]
    for index,data in enumerate(source):
        identity=message_identity(data); matches=candidates[identity]
        selected=matches[0] if len(matches)==1 else None
        method='identity' if selected is not None else 'source-original'
        bindings=map_banks.get(index,set()) if map_banks is not None else set()
        if len(bindings)>1 or selected is not None and bindings and selected not in bindings:
            selected=None; method='source-original'
        elif selected is None and len(bindings)==1:
            bound=next(iter(bindings))
            if not 0<=bound<len(original): raise ValueError('Map message binding is outside its archive')
            # The verified source/ROM map pair establishes bank identity even if its key changed.
            if identities[bound][0]==identity[0] and identities[bound][2]==identity[2]:
                selected=bound; method='map'
        selected_data=original[selected] if selected is not None else data
        choice=dict(source=index,native=SOURCE_BASE+index,rom=selected,kind='rom' if selected is not None else 'source-original',method=method)
        if selected is None and image_sha256==ENTRY_PROJECTION_ROM_SHA256 and index in ENTRY_PROJECTIONS:
            bound,source_sha,rom_sha,entries=ENTRY_PROJECTIONS[index]
            if bound>=len(original) or sha(data)!=source_sha or sha(original[bound])!=rom_sha:
                raise ValueError(f'Verified entry projection bank changed: {index}')
            selected_data=project_message_entries(data,original[bound],entries)
            choice.update(kind='entry-projection',method='verified-entry-projection',entryRomBank=bound,
                sourceSha256=source_sha,romBankSha256=rom_sha,entries=[dict(sourceId=a,romId=b) for a,b in entries])
        output.append(selected_data); choices.append(choice)
    return output,choices

def write_narc(members):
    if len(members)>65535: raise ValueError('Too many NARC members')
    image=bytearray(); entries=bytearray()
    for member in members:
        start=len(image); image.extend(member); end=len(image)
        image.extend(bytes((-len(image))%4)); entries.extend(struct.pack('<II',start,end))
    fat=b'BTAF'+struct.pack('<IHH',12+len(entries),len(members),0)+entries
    names=b'BTNF'+struct.pack('<I',16)+bytes.fromhex('0400000000000100')
    payload=b'GMIF'+struct.pack('<I',8+len(image))+image
    return struct.pack('<4sHHIHH',b'NARC',0xFFFE,0x100,16+len(fat)+len(names)+len(payload),16,3)+fat+names+payload

def pokedex_logo_screen(data):
    if (len(data)<36 or data[:8]!=b'RCSN\xff\xfe\x00\x01'
        or struct.unpack_from('<IHH',data,8)!=(len(data),16,1) or data[16:20]!=b'NRCS'
        or struct.unpack_from('<I',data,20)[0]!=len(data)-16 or struct.unpack_from('<I',data,28)[0]):
        raise ValueError('Invalid original Pokédex logo screen')
    width,height=struct.unpack_from('<HH',data,24); size=struct.unpack_from('<I',data,32)[0]
    if (width,height) not in ((80,24),(176,32)) or size!=width*height//32 or len(data)!=36+size:
        raise ValueError('Unsupported original Pokédex logo dimensions')
    if (width,height)==(176,32): return data
    # Preserve the original opaque BG0 backing so the list underneath cannot show through.
    result=bytearray(data[:36]+struct.pack('<H',0x9001)*88)
    struct.pack_into('<I',result,8,len(result)); struct.pack_into('<I',result,20,len(result)-16)
    struct.pack_into('<HH',result,24,176,32); struct.pack_into('<I',result,32,176)
    for row in range(3): result[36+(row*22+6)*2:36+(row*22+16)*2]=data[36+row*20:36+(row+1)*20]
    return bytes(result)

def pokedex_sprite_members(destination,original):
    if len(original)!=123: raise ValueError('Unsupported Pokédex graphics archive revision')
    source=ROOT/'files/graphic/zukan_gra'; tool=ROOT/'tools/nitrogfx/nitrogfx'
    scratch=destination/'pokedex-sprites'; scratch.mkdir(parents=True,exist_ok=True)
    selected=list(original)
    provenance=dict(path=str(POKEDEX_GRAPHICS_PATH),tool=str(tool.relative_to(ROOT)),toolSha256=sha(tool.read_bytes()),members=[])
    provenance['logoMembers']=[]; blank_character=None
    for index in (7,8):
        name=f'zukan_gra_{index:08d}'; raw=scratch/(name+'.NSCR'); packed=raw.with_suffix(raw.suffix+'.lz'); commands=[]
        if len(original[index])<4 or original[index][0]!=0x10: raise ValueError('Invalid original Pokédex logo LZ10 member')
        packed.write_bytes(original[index]); command=[tool,packed,raw]; run(command); commands.append(list(map(str,command)))
        previous=raw.read_bytes(); decoded=pokedex_logo_screen(previous)
        if decoded!=previous:
            if blank_character is None:
                character_packed=scratch/'zukan_gra_00000001.NCGR.lz'; character_raw=character_packed.with_suffix('')
                if len(original[1])<4 or original[1][0]!=0x10: raise ValueError('Invalid original Pokédex background LZ10 member')
                character_packed.write_bytes(original[1]); command=[tool,character_packed,character_raw]; run(command); commands.append(list(map(str,command)))
                blank_character=character_raw.read_bytes()
                if (len(blank_character)<48 or blank_character[:6]!=b'RGCN\xff\xfe'
                    or struct.unpack_from('<H',blank_character,6)[0] not in (0x100,0x101)
                    or struct.unpack_from('<IHH',blank_character,8)[:2]!=(len(blank_character),16)
                    or not struct.unpack_from('<H',blank_character,14)[0] or blank_character[16:20]!=b'RAHC'
                    or struct.unpack_from('<I',blank_character,28)[0]!=3 or struct.unpack_from('<I',blank_character,36)[0]):
                    raise ValueError('Invalid original Pokédex background characters')
                block=struct.unpack_from('<I',blank_character,20)[0]; size,offset=struct.unpack_from('<II',blank_character,40); start=24+offset
                if block<32 or block>len(blank_character)-16 or size<64 or start<48 or start>16+block-size or blank_character[start+32:start+64]!=b'\x77'*32:
                    raise ValueError('Original Pokédex logo padding tile does not match its background')
            raw.write_bytes(decoded); command=[tool,raw,packed,'-nopad']; run(command); commands.append(list(map(str,command)))
            selected[index]=packed.read_bytes()
            if len(selected[index])<4 or selected[index][0]!=0x10 or int.from_bytes(selected[index][1:4],'little')!=len(decoded):
                raise ValueError('Invalid normalized Pokédex logo LZ10 member')
        row=dict(member=index,source='rom',originalSha256=sha(original[index]),originalDecodedSha256=sha(previous),decodedSha256=sha(decoded),
            nativeSha256=sha(selected[index]),sourceSizeTiles=[value//8 for value in struct.unpack_from('<HH',previous,24)],nativeSizeTiles=[22,4],
            paddingOffsetTiles=[6,0] if decoded!=previous else [0,0],commands=commands)
        if decoded!=previous: row.update(paddingTile=0x9001,blankCharacterMember=1,blankCharacterSha256=sha(original[1]),blankCharacterDecodedSha256=sha(blank_character))
        provenance['logoMembers'].append(row)
    # The source search sliders use animations 67/68 and cells 63/64 as one resource group.
    for index,suffix in ((12,'NCGR'),(13,'NCER'),(14,'NANR')):
        name=f'zukan_gra_{index:08d}'; source_file=source/(name+('.png' if index==12 else '.'+suffix))
        raw=scratch/(name+'.'+suffix); packed=raw.with_suffix(raw.suffix+'.lz'); commands=[]
        if index==12:
            command=[tool,source_file,raw,'-clobbersize','-version101','-mappingtype','64']
            run(command); commands.append(list(map(str,command)))
        else: raw.write_bytes(source_file.read_bytes())
        command=[tool,raw,packed,'-nopad']; run(command); commands.append(list(map(str,command)))
        decoded=raw.read_bytes(); data=packed.read_bytes()
        if len(data)<4 or data[0]!=0x10 or int.from_bytes(data[1:4],'little')!=len(decoded):
            raise ValueError(f'Invalid original Pokédex LZ10 member: {index}')
        selected[index]=data
        provenance['members'].append(dict(member=index,source=str(source_file.relative_to(ROOT)),sourceSha256=sha(source_file.read_bytes()),
            decodedSha256=sha(decoded),commands=commands,originalSha256=sha(original[index]),nativeSha256=sha(data)))
    return selected,provenance

def source_table(path,name='sMapHeaders'):
    obj=wasm_object(path.read_bytes(),True)
    symbol=next(s for s in obj['symbols'] if s.get('name')==name)
    return obj['segments'][symbol['segment']]['bytes'][symbol['offset']:symbol['offset']+symbol['size']]

def locate_maps(code,baseline,message_count):
    if not baseline or len(baseline)%24: raise ValueError('Invalid source map table')
    # These fields identify maps across regions; script, message, event IDs and final flags may differ.
    stable=tuple(range(6))+tuple(range(12,16))+tuple(range(18,22))
    candidates=[]; offset=code.find(baseline[:6])
    while offset>=0:
        if offset%4==0 and offset+len(baseline)<=len(code) and all(
            code[offset+row+column]==baseline[row+column] for row in range(0,len(baseline),24) for column in stable):
            candidates.append(offset)
        offset=code.find(baseline[:6],offset+1)
    if len(candidates)!=1: raise ValueError(f'ROM map table is not unique: {len(candidates)} candidates')
    offset=candidates[0]; table=code[offset:offset+len(baseline)]
    if any(struct.unpack_from('<H',table,row+10)[0]>=message_count for row in range(0,len(table),24)): raise ValueError('ROM map message ID is outside its archive')
    return offset,table

def locate_script_banks(code,baseline,script_count,message_count):
    if not baseline or len(baseline)%6: raise ValueError('Invalid standard-script table')
    candidates=[]; offset=code.find(baseline[:2])
    while offset>=0:
        if offset%2==0 and offset+len(baseline)<=len(code) and all(code[offset+row:offset+row+2]==baseline[row:row+2] for row in range(0,len(baseline),6)):
            candidates.append(offset)
        offset=code.find(baseline[:2],offset+1)
    if len(candidates)!=1: raise ValueError('ROM standard-script table is not unique')
    offset=candidates[0]; table=code[offset:offset+len(baseline)]
    if any(script>=script_count or message>=message_count for _,script,message in struct.iter_unpack('<HHH',table)): raise ValueError('Standard-script resource ID is outside its archive')
    return offset,table

def script_header(table):
    rows=[f'    {{{start},{script},{message}}},' for start,script,message in struct.iter_unpack('<HHH',table)]
    return '\n'.join(['#pragma once','_Static_assert(sizeof(struct ScriptBankMapping)==6,"Original script-bank layout");',
        f'const struct ScriptBankMapping sScriptBankMapping[{len(rows)}] = {{',*rows,'};',''])

def map_header(table):
    lines=['#pragma once','_Static_assert(sizeof(MapHeader)==24,"Original map record layout");','static const MapHeader sMapHeaders[] = {']
    for offset in range(0,len(table),24):
        wild,area,coords,matrix,scripts,script_header,message,day,night,events,section,flags=struct.unpack_from('<BB9HI',table,offset)
        fields=dict(wildEncounterBank=wild,areaDataBank=area,moveModelBank=coords&15,worldMapX=(coords>>4)&63,worldMapY=coords>>10,
            matrixId=matrix,scriptsBank=scripts,scriptHeaderBank=script_header,msgBank=message,dayMusicId=day,nightMusicId=night,
            eventsBank=events,mapsec=section&255,areaIcon=(section>>8)&15,momCallIntroParam=section>>12,regionNo=flags&1,
            weather=(flags>>1)&127,mapType=(flags>>8)&15,cameraType=(flags>>12)&63,followMode=(flags>>18)&3,battleBg=(flags>>20)&31,
            bikeAllowed=(flags>>25)&1,runningAllowed_Unused=(flags>>26)&1,escapeRopeAllowed=(flags>>27)&1,flyAllowed=(flags>>28)&1,
            outgoingCalls=(flags>>29)&1,incomingCalls=(flags>>30)&1,radioSignal=flags>>31)
        lines.append('    {'+','.join(f'.{name}={value}' for name,value in fields.items())+'},')
    return '\n'.join([*lines,'};',''])

def rom_image(manifest):
    configured=os.environ.get('HG_ORIGINAL_ROM')
    paths=[Path(configured)] if configured else [p for folder in (ROOT.parent/'roms',ROOT.parents[1]/'work/rom-reference') if folder.is_dir() for p in folder.rglob('*') if p.suffix.lower()=='.nds']
    for path in paths:
        data=path.read_bytes()
        if sha(data)==manifest['image']['sha256']: return data
    raise ValueError('The verified original ROM is required for native map metadata; set HG_ORIGINAL_ROM to its path')

def prepare(destination,compiler,flags):
    original=ROOT/'ios/OriginalContent'
    if not (original/'manifest.json').exists() and not (original/'boot-header.bin').exists(): return None
    read_boot_header(original)
    manifest=json.loads((original/'manifest.json').read_text())
    if manifest['image']['gameCode'][-1]!='J': return None
    destination.mkdir(parents=True,exist_ok=True)
    generated=ROOT/'ios/build-original-host/include'; original_messages=narc(original/'files'/MESSAGE_PATH)
    if len(original_messages)!=814: raise ValueError('Unsupported JP message archive revision')
    bins=sorted((generated/'msgdata/msg').glob('*.bin'))
    if [int(p.name[4:8]) for p in bins]!=list(range(829)): raise ValueError('Source message bank IDs are incomplete')
    source=[p.read_bytes() for p in bins]
    image=rom_image(manifest); start,_,base,size=struct.unpack_from('<IIII',image,0x20)
    if start<512 or size>len(image)-start: raise ValueError('Invalid ARM9 extent')
    code=decode_code(image[start:start+size])
    baseline=destination/'baseline-map.o'; log=destination/'baseline-map.log'
    result=subprocess.run([compiler,*flags,'-c',ROOT/'src/map_header.c','-o',baseline],cwd=ROOT,capture_output=True,text=True)
    log.write_text(result.stdout+result.stderr)
    if result.returncode: raise RuntimeError(f'Baseline map compile failed: {log}')
    baseline_maps=source_table(baseline)
    offset,maps=locate_maps(code,baseline_maps,len(original_messages))
    map_banks=collections.defaultdict(set)
    for row in range(0,len(maps),24):
        source_bank=struct.unpack_from('<H',baseline_maps,row+10)[0]
        if source_bank>=len(source): raise ValueError('Source map message ID is outside its archive')
        map_banks[source_bank].add(struct.unpack_from('<H',maps,row+10)[0])
    selected,choices=select_messages(source,original_messages,map_banks,sha(image))
    members=[*original_messages,*([b'']*(SOURCE_BASE-len(original_messages))),*selected]
    script_baseline=destination/'baseline-script.o'
    result=subprocess.run([compiler,*flags,'-c',ROOT/'src/script_manager.c','-o',script_baseline],cwd=ROOT,capture_output=True,text=True)
    (destination/'baseline-script.log').write_text(result.stdout+result.stderr)
    if result.returncode: raise RuntimeError('Baseline standard-script compile failed')
    scripts=narc(original/'files/a/0/1/2')
    script_offset,script_table=locate_script_banks(code,source_table(script_baseline,'sScriptBankMapping'),len(scripts),len(original_messages))
    zero_message=choices[184]['rom']
    if zero_message is None or zero_message>255: raise ValueError('Default script message binding is not verified')
    references=[p for p in range(0,len(code)-3,4) if code[p:p+4]==struct.pack('<I',base+script_offset)]
    zero_pair=struct.pack('<HH',0x2200|140,0x2300|zero_message)
    if not any(zero_pair in code[max(0,p-256):p] for p in references): raise ValueError('Default ROM script/message instructions do not match source bindings')
    include=destination/'include'; (include/'msgdata').mkdir(parents=True,exist_ok=True)
    naix=(generated/'msgdata/msg.naix').read_text()
    naix,count=re.subn(r'(#define NARC_msg_msg_\w+_bin )([0-9]+)',lambda m:m[1]+str(SOURCE_BASE+int(m[2])),naix)
    if count!=829: raise ValueError('Incomplete source message header')
    (include/'msgdata/msg.naix').write_text(naix); (include/'hg_native_map_headers.h').write_text(map_header(maps))
    (include/'hg_native_script_banks.h').write_text(script_header(script_table))
    verify=destination/'native-map.o'
    result=subprocess.run([compiler,'-I',include,'-DHG_NATIVE_ROM_TABLES',*flags,'-c',ROOT/'src/map_header.c','-o',verify],cwd=ROOT,capture_output=True,text=True)
    (destination/'native-map.log').write_text(result.stdout+result.stderr)
    if result.returncode or source_table(verify)!=maps: raise RuntimeError('Native map records do not exactly match the verified ROM table')
    script_verify=destination/'native-script.o'
    result=subprocess.run([compiler,'-I',include,'-DHG_NATIVE_ROM_TABLES',*flags,'-c',ROOT/'src/script_manager.c','-o',script_verify],cwd=ROOT,capture_output=True,text=True)
    (destination/'native-script.log').write_text(result.stdout+result.stderr)
    if result.returncode or source_table(script_verify,'sScriptBankMapping')!=script_table: raise RuntimeError('Native standard-script records do not match the ROM table')
    pack=destination/'pack'; (pack/'files').mkdir(parents=True,exist_ok=True); pokedex_graphics=None
    for record in manifest['files']:
        if Path(record['path']).is_absolute() or '..' in Path(record['path']).parts: raise ValueError('Unsafe content path')
        source_file=original/'files'/record['path']; target=pack/'files'/record['path']; target.parent.mkdir(parents=True,exist_ok=True)
        if sha(source_file.read_bytes())!=record['sha256']: raise ValueError(f'Original content changed: {record["path"]}')
        target.unlink(missing_ok=True)
        if Path(record['path'])==MESSAGE_PATH:
            data=write_narc(members); target.write_bytes(data); record.update(size=len(data),sha256=sha(data))
        elif Path(record['path'])==POKEDEX_GRAPHICS_PATH:
            selected_graphics,pokedex_graphics=pokedex_sprite_members(destination,narc(source_file))
            pokedex_graphics['originalArchiveSha256']=record['sha256']
            data=write_narc(selected_graphics); target.write_bytes(data); record.update(size=len(data),sha256=sha(data))
            if narc(target)!=selected_graphics: raise RuntimeError('Native Pokédex graphics archive round trip failed')
            pokedex_graphics['archiveSha256']=sha(data)
        else: os.link(source_file,target)
    shutil.copyfile(original/'boot-header.bin',pack/'boot-header.bin')
    (pack/'manifest.json').write_text(json.dumps(manifest,indent=2)+'\n'); read_boot_header(pack)
    if narc(pack/'files'/MESSAGE_PATH)!=members: raise RuntimeError('Native message archive round trip failed')
    if pokedex_graphics is None: raise ValueError('Missing original Pokédex graphics archive')
    report=dict(scope='region data binding; complete dialogue and gameplay fidelity unverified',imageSha256=manifest['image']['sha256'],sourceBase=SOURCE_BASE,
        romMessages=len(original_messages),sourceMessages=len(source),reusedRomBanks=sum(c['rom'] is not None for c in choices),
        sourceFallbackBanks=[c['source'] for c in choices if c['kind']=='source-original'],
        entryProjectionBanks=[c['source'] for c in choices if c['kind']=='entry-projection'],choices=choices,mapCount=len(maps)//24,pokedexGraphics=pokedex_graphics,
        sourceMapInitializerSha256=sha((ROOT/'src/data/map_headers.h').read_bytes()),
        sourceMapObjectSha256=sha(baseline.read_bytes()),sourceMapTableSha256=sha(baseline_maps),
        romMapAddress=base+offset,mapSha256=sha(maps),messageArchiveSha256=sha((pack/'files'/MESSAGE_PATH).read_bytes()),
        romScriptTableAddress=base+script_offset,scriptTableSha256=sha(script_table),standardScripts=len(script_table)//6,
        scriptHeaderSha256=sha((include/'hg_native_script_banks.h').read_bytes()),defaultScriptPair=[140,zero_message],
        mapHeaderSha256=sha((include/'hg_native_map_headers.h').read_bytes()),messageHeaderSha256=sha((include/'msgdata/msg.naix').read_bytes()))
    report_file=destination/'report.json'; report_file.write_text(json.dumps(report,indent=2)+'\n')
    print(f'Native content: {report["reusedRomBanks"]}/829 source banks reuse ROM bytes; {len(report["entryProjectionBanks"])} project verified entries; {len(report["sourceFallbackBanks"])} use source originals; {report["mapCount"]} ROM maps verified.')
    return dict(root=str(pack.relative_to(ROOT)),include=str(include.relative_to(ROOT)),report=str(report_file.relative_to(ROOT)),reportSha256=sha(report_file.read_bytes()))

def verify(profile):
    if not profile: return ROOT/'ios/OriginalContent'
    report_file=ROOT/profile['report']
    if sha(report_file.read_bytes())!=profile['reportSha256']: raise ValueError('Native content report changed since engine linking')
    report=json.loads(report_file.read_text()); pack=ROOT/profile['root']; include=ROOT/profile['include']
    checks=[(pack/'files'/MESSAGE_PATH,'messageArchiveSha256'),(include/'hg_native_map_headers.h','mapHeaderSha256'),
        (include/'hg_native_script_banks.h','scriptHeaderSha256'),(include/'msgdata/msg.naix','messageHeaderSha256')]
    if any(sha(path.read_bytes())!=report[key] for path,key in checks): raise ValueError('Native content changed since engine linking')
    if 'pokedexGraphics' in report and sha((pack/'files'/POKEDEX_GRAPHICS_PATH).read_bytes())!=report['pokedexGraphics']['archiveSha256']:
        raise ValueError('Native Pokédex graphics changed since engine linking')
    manifest=json.loads((pack/'manifest.json').read_text())
    for record in manifest['files']:
        if Path(record['path']).is_absolute() or '..' in Path(record['path']).parts or sha((pack/'files'/record['path']).read_bytes())!=record['sha256']:
            raise ValueError('Native content file does not match its manifest')
    read_boot_header(pack)
    return pack
