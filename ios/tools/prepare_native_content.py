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

ROOT=Path(__file__).resolve().parents[2]
SOURCE_BASE=1024
MESSAGE_PATH=Path('a/0/2/7')

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
                    slots.append((code&0xFF00,words[position+3] if arguments else None))
                position=end
            else: position+=1
            if position>=len(words): raise ValueError('Missing message terminator')
        placeholders.append(tuple(slots))
    return count,key,tuple(placeholders)

def select_messages(source,original):
    candidates=collections.defaultdict(list)
    for index,data in enumerate(original): candidates[message_identity(data)].append(index)
    output=[]; choices=[]
    for index,data in enumerate(source):
        matches=candidates[message_identity(data)]
        selected=matches[0] if len(matches)==1 else None
        output.append(original[selected] if selected is not None else data)
        choices.append(dict(source=index,native=SOURCE_BASE+index,rom=selected,kind='rom' if selected is not None else 'source-original'))
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
    source=[p.read_bytes() for p in bins]; selected,choices=select_messages(source,original_messages)
    members=[*original_messages,*([b'']*(SOURCE_BASE-len(original_messages))),*selected]
    image=rom_image(manifest); start,_,base,size=struct.unpack_from('<IIII',image,0x20)
    if start<512 or size>len(image)-start: raise ValueError('Invalid ARM9 extent')
    code=decode_code(image[start:start+size])
    baseline=destination/'baseline-map.o'; log=destination/'baseline-map.log'
    result=subprocess.run([compiler,*flags,'-c',ROOT/'src/map_header.c','-o',baseline],cwd=ROOT,capture_output=True,text=True)
    log.write_text(result.stdout+result.stderr)
    if result.returncode: raise RuntimeError(f'Baseline map compile failed: {log}')
    offset,maps=locate_maps(code,source_table(baseline),len(original_messages))
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
    pack=destination/'pack'; (pack/'files').mkdir(parents=True,exist_ok=True)
    for record in manifest['files']:
        if Path(record['path']).is_absolute() or '..' in Path(record['path']).parts: raise ValueError('Unsafe content path')
        source_file=original/'files'/record['path']; target=pack/'files'/record['path']; target.parent.mkdir(parents=True,exist_ok=True)
        if sha(source_file.read_bytes())!=record['sha256']: raise ValueError(f'Original content changed: {record["path"]}')
        target.unlink(missing_ok=True)
        if Path(record['path'])==MESSAGE_PATH:
            data=write_narc(members); target.write_bytes(data); record.update(size=len(data),sha256=sha(data))
        else: os.link(source_file,target)
    shutil.copyfile(original/'boot-header.bin',pack/'boot-header.bin')
    (pack/'manifest.json').write_text(json.dumps(manifest,indent=2)+'\n'); read_boot_header(pack)
    if narc(pack/'files'/MESSAGE_PATH)!=members: raise RuntimeError('Native message archive round trip failed')
    report=dict(scope='region data binding; complete dialogue and gameplay fidelity unverified',imageSha256=manifest['image']['sha256'],sourceBase=SOURCE_BASE,
        romMessages=len(original_messages),sourceMessages=len(source),reusedRomBanks=sum(c['rom'] is not None for c in choices),
        sourceFallbackBanks=[c['source'] for c in choices if c['rom'] is None],choices=choices,mapCount=len(maps)//24,
        romMapAddress=base+offset,mapSha256=sha(maps),messageArchiveSha256=sha((pack/'files'/MESSAGE_PATH).read_bytes()),
        romScriptTableAddress=base+script_offset,scriptTableSha256=sha(script_table),standardScripts=len(script_table)//6,
        scriptHeaderSha256=sha((include/'hg_native_script_banks.h').read_bytes()),defaultScriptPair=[140,zero_message],
        mapHeaderSha256=sha((include/'hg_native_map_headers.h').read_bytes()),messageHeaderSha256=sha((include/'msgdata/msg.naix').read_bytes()))
    report_file=destination/'report.json'; report_file.write_text(json.dumps(report,indent=2)+'\n')
    print(f'Native content: {report["reusedRomBanks"]}/829 source banks reuse ROM bytes; {len(report["sourceFallbackBanks"])} use source originals; {report["mapCount"]} ROM maps verified.')
    return dict(root=str(pack.relative_to(ROOT)),include=str(include.relative_to(ROOT)),report=str(report_file.relative_to(ROOT)),reportSha256=sha(report_file.read_bytes()))

def verify(profile):
    if not profile: return ROOT/'ios/OriginalContent'
    report_file=ROOT/profile['report']
    if sha(report_file.read_bytes())!=profile['reportSha256']: raise ValueError('Native content report changed since engine linking')
    report=json.loads(report_file.read_text()); pack=ROOT/profile['root']; include=ROOT/profile['include']
    checks=[(pack/'files'/MESSAGE_PATH,'messageArchiveSha256'),(include/'hg_native_map_headers.h','mapHeaderSha256'),
        (include/'hg_native_script_banks.h','scriptHeaderSha256'),(include/'msgdata/msg.naix','messageHeaderSha256')]
    if any(sha(path.read_bytes())!=report[key] for path,key in checks): raise ValueError('Native content changed since engine linking')
    manifest=json.loads((pack/'manifest.json').read_text())
    for record in manifest['files']:
        if Path(record['path']).is_absolute() or '..' in Path(record['path']).parts or sha((pack/'files'/record['path']).read_bytes())!=record['sha256']:
            raise ValueError('Native content file does not match its manifest')
    read_boot_header(pack)
    return pack
