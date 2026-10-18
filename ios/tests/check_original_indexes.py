"""Check that native archive IDs require explicit, unambiguous source ordering."""
from pathlib import Path
import sys
import subprocess
import tempfile

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools'))
from prepare_original_headers import member_indexes
import prepare_original_headers as headers

with tempfile.TemporaryDirectory() as temporary:
    root = Path(temporary)
    numbered = root / 'numbered'; numbered.mkdir()
    for name in ('resource_00000000.NCGR.png', 'resource_00000000.NCGR', 'resource_00000001.NCLR', '.narcignore'):
        (numbered / name).touch()
    assert member_indexes(numbered) == {'resource_00000000': 0, 'resource_00000001': 1}
    (numbered / 'resource_00000003.NCER').touch()
    try:
        member_indexes(numbered)
        raise AssertionError('Accepted a gap in archive member IDs')
    except ValueError:
        pass
    named = root / 'named'; named.mkdir()
    for name in ('00_table.NSBMD', '01_ball.NSBCA'):
        (named / name).touch()
    assert member_indexes(named) == {'00_table': 0, '01_ball': 1}
    matrix = root / 'matrix'; matrix.mkdir()
    for name in ('matrix_0000_EVERYWHERE.bin', 'matrix_0001_ROOM.bin'):
        (matrix / name).touch()
    assert member_indexes(matrix) == {'matrix_0000_EVERYWHERE': 0, 'matrix_0001_ROOM': 1}
    (matrix / 'subdirectory').mkdir()
    try:
        member_indexes(matrix)
        raise AssertionError('Accepted a recursive archive without explicit member ordering')
    except ValueError:
        pass
    (named / '01_other.NSBMD').touch()
    try:
        member_indexes(named)
        raise AssertionError('Accepted two different members with the same ID')
    except ValueError:
        pass
    unpadded = root / 'unpadded'; unpadded.mkdir()
    for index in range(12):
        (unpadded / f'unpadded_{index}_asset.bin').touch()
    try:
        member_indexes(unpadded)
        raise AssertionError('Accepted numeric IDs that disagree with packed filename order')
    except ValueError:
        pass
    objects = root / 'stats'
    objects.with_suffix('.json.txt').write_text('.type height, @object\n.type weight, @object\n')
    assert member_indexes(objects) == {'height': 0, 'weight': 1, 'stats_00000000': 0, 'stats_00000001': 1}
    project = root / 'project'; (project / 'src').mkdir(parents=True)
    source = project / 'files/letters/letters.json.txt'; source.parent.mkdir(parents=True)
    source.write_text('.type entry_A, @object\n.type entry_B, @object\n')
    consumer = project / 'src/consumer.c'
    consumer.write_text('int value; /* multiline\ncomment */ # include "letters/letters.naix"\n#define MEMBER(letter) NARC_letters_entry_##letter\n'
                        '// NARC_letters_undefined_*\nconst char *note = "NARC_letters_example";\n')
    previous_root = headers.ROOT; headers.ROOT = project
    output = project / 'build/include'; headers.archive_headers(output)
    result = output / 'letters/letters.naix'
    assert '#define NARC_letters_entry_B 1\n' in result.read_text()
    source.write_text('.byte 0\n'); headers.archive_headers(output)
    assert not result.exists() # A changed/unsupported source cannot leave stale successful IDs.
    consumer.write_text('#include "../../escape.naix"\n')
    try:
        headers.archive_headers(output)
        raise AssertionError('Accepted an archive header outside the build directory')
    except ValueError:
        pass
    version_header = 'application/zukanlist/zkn_data/zukan_hw_data.naix'
    version_source = project / 'files' / Path(version_header).with_suffix(''); version_source.mkdir(parents=True)
    for name, payload in [('0',b'base-content'),('1_gold',b'gold-content'),('1_silver',b'silver-content')]:
        (version_source / f'zukan_hw_data_{name}.bin').write_bytes(payload)
    tool = project / 'tools/nitroarc/nitroarc'; tool.parent.mkdir(parents=True)
    native_tool = previous_root / 'tools/nitroarc/nitroarc'
    if not native_tool.is_file(): subprocess.run(['make','-s','-C',str(native_tool.parent)],check=True)
    tool.symlink_to(native_tool)
    consumer.write_text(f'#include "{version_header}"\nint id=NARC_zukan_hw_data_zukan_hw_data_1_bin;\n')
    headers.archive_headers(output)
    assert '#define NARC_zukan_hw_data_zukan_hw_data_1_bin 1\n' in (output / version_header).read_text()
    archive = (output / Path(version_header).with_suffix('.narc')).read_bytes()
    fat = int.from_bytes(archive[12:14],'little')
    assert int.from_bytes(archive[fat+8:fat+10],'little') == 2
    assert b'gold-content' in archive and b'silver-content' not in archive
    headers.ROOT = previous_root
print('PASS: original archive source IDs, conversion sidecars, gaps, duplicate IDs and data-object order.')
