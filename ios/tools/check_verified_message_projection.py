# SPDX-License-Identifier: GPL-3.0-or-later
"""Assert raw entry projection and read-only proof binding; emit only into --output."""
import argparse
import hashlib
import importlib.util
import json
from pathlib import Path
import struct
import sys

PROOF_SHA256='95829965d520fecfa0b583eb234827413b8ecf127a49f39a063d21ae044b5c55'

def sha(data): return hashlib.sha256(data).hexdigest()

def raw_row(data,row):
    count,key=struct.unpack_from('<HH',data)
    assert 0<=row<count
    mask=(key*765*(row+1))&0xFFFF; mask*=0x10001
    start,size=(value^mask for value in struct.unpack_from('<II',data,4+8*row))
    assert 4+8*count<=start<=len(data) and size<=(len(data)-start)//2
    words=struct.unpack_from('<'+'H'*size,data,start)
    return tuple(value^((596947*(row+1)+18749*position)&0xFFFF) for position,value in enumerate(words)),data[start:start+size*2]

def bank(rows,key=123):
    data=bytearray(struct.pack('<HH',len(rows),key)+bytes(len(rows)*8))
    for row,words in enumerate(rows):
        mask=(key*765*(row+1)&0xFFFF)*0x10001
        struct.pack_into('<II',data,4+row*8,len(data)^mask,len(words)^mask)
        data.extend(struct.pack('<'+'H'*len(words),*(value^((596947*(row+1)+18749*i)&0xFFFF) for i,value in enumerate(words))))
    return bytes(data)

def rejects(callback):
    try: callback()
    except ValueError: return
    raise AssertionError('Malformed or mismatched input was accepted')

def synthetic(module):
    duplicate=(0xFFFE,0x0103,1,0,0xFFFE,0x0103,1,0,0xFFFF)
    packed=(0xF100,0x0123|(0x1FF<<9)&0x7FFF,0x7FFF,0xFFFF)
    source=bank([(0x137,0xFFFF)]*80+[(0x138,0xFFFF),(0x139,0xFFFF),duplicate,packed])
    original=bank([(0x140,0xFFFF)]*80+[(0x04FE,0x0D8A,0xFFFF,0xFFFF),duplicate,packed],key=456)
    result=module.project_message_entries(source,original,((81,80),(82,81),(83,82)))
    assert result[:4]==source[:4]
    for row in range(84):
        expected=raw_row(original,{81:80,82:81,83:82}[row])[0] if row in (81,82,83) else raw_row(source,row)[0]
        assert raw_row(result,row)[0]==expected
        if row not in (81,82,83):
            assert result[4+row*8:12+row*8]==source[4+row*8:12+row*8]
            assert raw_row(result,row)[1]==raw_row(source,row)[1]
    assert raw_row(result,81)[1]!=raw_row(original,80)[1]
    assert module.message_row(result,82)[1]==(0,0)
    assert module.message_row(result,83)[0]==packed
    assert module.message_identity(result)[2][82]==(0,0)
    rejects(lambda:module.project_message_entries(source,bank([(0xFFFE,0x0103,1,1,0xFFFF)]),((81,0),)))
    rejects(lambda:module.project_message_entries(source,bank([duplicate]),((81,0),)))
    rejects(lambda:module.project_message_entries(b'',original,((0,0),)))
    rejects(lambda:module.message_row(b'\x00',0))
    rejects(lambda:module.message_row(bank([(1,0xFFFF)]),1))
    rejects(lambda:module.message_row(bank([(1,)]),0))
    rejects(lambda:module.message_row(bank([(0xFFFE,0x0103,0,0xFFFF)]),0))
    rejects(lambda:module.message_row(bank([(0xFFFE,0x0103,2,0,0xFFFF)]),0))
    rejects(lambda:module.message_row(bank([(0xF100,1)]),0))
    malformed=bytearray(bank([(1,0xFFFF)])); malformed[4:12]=bytes(8)
    rejects(lambda:module.message_row(malformed,0))
    malformed=bytearray(bank([(1,0xFFFF)])); malformed[2:4]=struct.pack('<H',65535)
    rejects(lambda:module.message_row(malformed,0))

def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--root',type=Path,required=True,help='Read-only source checkout')
    parser.add_argument('--proof',type=Path,required=True)
    parser.add_argument('--output',type=Path,required=True)
    args=parser.parse_args(); root=args.root.resolve(); output=args.output.resolve()
    assert root!=output and root not in output.parents,'Checks must not write inside the source checkout'
    sys.dont_write_bytecode=True; sys.path.insert(0,str(root/'ios/tools'))
    candidate=Path(__file__).with_name('prepare_native_content.py')
    spec=importlib.util.spec_from_file_location('projection_candidate',candidate)
    module=importlib.util.module_from_spec(spec); spec.loader.exec_module(module)
    from import_original_content import index_rom,narc
    synthetic(module)
    proof_bytes=args.proof.read_bytes(); assert sha(proof_bytes)==PROOF_SHA256
    proof=json.loads(proof_bytes); image=Path(proof['provenance']['romPath']).read_bytes()
    image_sha=sha(image); assert image_sha==proof['provenance']['romSha256']==module.ENTRY_PROJECTION_ROM_SHA256
    archive=root/'ios/OriginalContent/files'/module.MESSAGE_PATH
    rom_record=next(record for record in index_rom(image)[1] if record['path']==str(module.MESSAGE_PATH))
    assert sha(archive.read_bytes())==rom_record['sha256']
    original=narc(archive)
    paths=sorted((root/'ios/build-original-host/include/msgdata/msg').glob('*.bin'))
    assert [int(path.name[4:8]) for path in paths]==list(range(829))
    source=[path.read_bytes() for path in paths]
    baseline,baseline_choices=module.select_messages(source,original)
    selected,choices=module.select_messages(source,original,image_sha256=image_sha)
    assert module.select_messages(source,original,image_sha256='0'*64)==(baseline,baseline_choices)
    proved=proof['provenEntryCorrespondences']
    expected={(row['sourceBank'],row['sourceId'],row['romBank'],row['romId']) for row in proved}
    actual={(source_id,a,record[0],b) for source_id,record in module.ENTRY_PROJECTIONS.items() for a,b in record[3]}
    assert actual==expected and len(actual)==8
    report=dict(scope='Read-only raw-code-unit entry projection checks; no build or runtime',imageSha256=image_sha,
        proofSha256=sha(proof_bytes),candidateSha256=sha(candidate.read_bytes()),banks=[],entries=[],
        pending=['Fresh content generation/audit/link','Full gameplay/dialogue and regional compatibility','Glyph/font rendering and layout'])
    output.mkdir(parents=True,exist_ok=True)
    for source_id,(rom_id,source_sha,rom_sha,entries) in module.ENTRY_PROJECTIONS.items():
        binding=proof['bankProofs'][str(source_id)]
        assert source_sha==binding['sourceBankSha256']==sha(source[source_id])
        assert rom_sha==binding['romBankSha256']==sha(original[rom_id])
        assert baseline[source_id]==source[source_id] and baseline_choices[source_id]['rom'] is None
        assert selected[source_id][:4]==source[source_id][:4]
        assert choices[source_id]['rom'] is None and choices[source_id]['kind']=='entry-projection'
        replacements=dict(entries); count=struct.unpack_from('<H',source[source_id])[0]
        for row in range(count):
            words,ciphertext=raw_row(selected[source_id],row)
            if row in replacements:
                assert words==raw_row(original[rom_id],replacements[row])[0]
                assert module.message_row(source[source_id],row)[1]==module.message_row(original[rom_id],replacements[row])[1]
                report['entries'].append(dict(sourceBank=source_id,sourceId=row,romBank=rom_id,romId=replacements[row],
                    wordCount=len(words),rawU16Sha256=sha(struct.pack('<'+'H'*len(words),*words)),
                    originalGlyphCodeUnits=[value for value in words if 0x0200<=value<0xE000]))
            else:
                assert words==raw_row(source[source_id],row)[0] and ciphertext==raw_row(source[source_id],row)[1]
                assert selected[source_id][4+row*8:12+row*8]==source[source_id][4+row*8:12+row*8]
        target=output/f'source-{source_id:04d}-entry-projected.bin'; target.write_bytes(selected[source_id])
        report['banks'].append(dict(source=source_id,rom=rom_id,sourceCount=count,romCount=struct.unpack_from('<H',original[rom_id])[0],
            projectedEntries=len(entries),fallbackEntries=count-len(entries),sourceSha256=source_sha,romBankSha256=rom_sha,
            entryProjectedSha256=sha(selected[source_id]),path=str(target)))
        changed=list(source); changed[source_id]=bank([(1,0xFFFF)]*count,key=struct.unpack_from('<H',source[source_id],2)[0])
        rejects(lambda:module.select_messages(changed,original,image_sha256=image_sha))
        changed=list(original); changed[rom_id]=bank([(1,0xFFFF)]*struct.unpack_from('<H',original[rom_id])[0],key=struct.unpack_from('<H',original[rom_id],2)[0])
        rejects(lambda:module.select_messages(source,changed,image_sha256=image_sha))
    for source_id in range(len(source)):
        if source_id not in module.ENTRY_PROJECTIONS:
            assert selected[source_id]==baseline[source_id] and choices[source_id]==baseline_choices[source_id]
    members=[*original,*([b'']*(module.SOURCE_BASE-len(original))),*selected]
    projected_archive=output/'entry-projected-messages.narc'; projected_archive.write_bytes(module.write_narc(members))
    assert narc(projected_archive)==members
    report.update(archiveSha256=sha(projected_archive.read_bytes()),archivePath=str(projected_archive),
        originalRomMembersUnchanged=len(original),sourceFallbackRowsUnchanged=156)
    report.update(status='pass',synthetic='raw/packed/rekey/duplicate-slot preservation; malformed and changed-slot rejection',
        actual='8 exact original-ROM raw-u16 rows; 156 other rows retain source words/table/ciphertext; all 826 other banks match prior selector')
    (output/'check-report.json').write_text(json.dumps(report,indent=2)+'\n')
    print(json.dumps(dict(status='pass',entries=len(actual),projectedBanks=len(report['banks']),report=str(output/'check-report.json'))))

if __name__=='__main__': main()
