# SPDX-License-Identifier: GPL-3.0-or-later
"""Build remaining source archives with original converters and explicit member orders."""
import hashlib
import json
from pathlib import Path
import re
import shlex
import shutil
import subprocess
import tempfile

from prepare_original_headers import ROOT, run

ARCHIVES = ('arc/ppark', 'fielddata/encountdata/g_enc_data', 'fielddata/encountdata/s_enc_data',
            'data/sbox_gra', 'graphic/camera_viewfinder', 'fielddata/graphic/preview_graphic/preview_graphic',
            'poketool/pokegra/otherpoke')


def archive_paths():
    registry = (ROOT / 'include/filesystem_files_def.h').read_text()
    identifiers = {name:int(value) for name,value in re.findall(r'\b(NARC_\w+)\s*=\s*(\d+)',registry)}
    paths = re.findall(r'"([^"\n]+)"',registry[registry.index('sNarcFileList[] = {'):].split('};',1)[0])
    result = {}
    for name in ARCHIVES:
        identifier = 'NARC_a_1_5_0' if name == 'fielddata/graphic/preview_graphic/preview_graphic' else 'NARC_'+name.replace('/','_')
        result[name] = paths[identifiers[identifier]]
    if len(set(result.values())) != len(result): raise ValueError('Duplicate source archive paths')
    return result


def build_archives(output):
    output = Path(output)
    work = output.parent / 'resource-build'; work.mkdir(parents=True, exist_ok=True)
    for tool in ('o2narc', 'nitrogfx'):
        run(['make', '-s', '-C', ROOT / 'tools' / tool])
    gfx, pack = ROOT / 'tools/nitrogfx/nitrogfx', ROOT / 'tools/nitroarc/nitroarc'
    image_roots = [ROOT / 'files' / name for name in ARCHIVES[3:]]
    inputs = {Path(__file__), ROOT / 'graphics_files_rules.mk', ROOT / 'files/poketool/pokegra/otherpoke.txt'}
    for folder in image_roots: inputs.update(path for path in folder.rglob('*') if path.is_file())
    for name in ('files/arc/ppark', 'files/fielddata/encountdata/gs_enc_data'):
        inputs.update(ROOT / (name + suffix) for suffix in ('.json', '.json.txt', '.mk'))
    inputs.update(ROOT / 'tools' / tool / tool for tool in ('jsonproc','nitroarc','nitrogfx','o2narc'))
    for folder in ('include','lib/include'):
        inputs.update((ROOT / folder).rglob('*.h'))
    def digest(paths):
        value = hashlib.sha256()
        for path in sorted(paths): value.update(str(path.relative_to(ROOT)).encode()); value.update(path.read_bytes())
        return value.hexdigest()
    fingerprint = digest(inputs) + run(['xcrun','clang','--version']).splitlines()[0]
    products = [output / (name + suffix) for name in ARCHIVES for suffix in ('.narc','.naix')]
    stamp = work / 'manifest.json'
    if stamp.is_file() and all(path.is_file() for path in products):
        prior = json.loads(stamp.read_text())
        if prior == dict(inputs=fingerprint, outputs=digest(products)):
            return {name + '.naix' for name in ARCHIVES}
    # shortcut: cache the batch together; split per archive if source-art iteration makes this costly.
    for name, source, defines in [('arc/ppark','arc/ppark',()),
            ('fielddata/encountdata/g_enc_data','fielddata/encountdata/gs_enc_data',('ENC_HEARTGOLD',)),
            ('fielddata/encountdata/s_enc_data','fielddata/encountdata/gs_enc_data',('ENC_SOULSILVER',))]:
        target = output / (name + '.narc'); target.parent.mkdir(parents=True, exist_ok=True)
        generated = work / (target.stem + '.c'); obj = generated.with_suffix('.o')
        run([ROOT / 'tools/jsonproc/jsonproc', ROOT / ('files/' + source + '.json'),
             ROOT / ('files/' + source + '.json.txt'), generated])
        flags = ['--target=armv5te-none-eabi','-std=gnu11','-ffreestanding','-fms-extensions',
                 '-Wno-microsoft','-Wno-unknown-pragmas','-DSDK_ARM9','-DSDK_CW','-DSDK_CODE_ARM',
                 '-DSDK_TS','-D_NITRO','-DHEARTGOLD','-DENGLISH', *['-D'+name for name in defines]]
        for directory in ('include','files','lib/include','lib/include/cw','lib/include/nitro',
                          'lib/include/nnsys','lib/include/nitro/os','include/library'):
            flags += ['-I', ROOT / directory]
        run(['xcrun','clang',*flags,'-c',generated,'-o',obj])
        run([ROOT / 'tools/o2narc/o2narc',obj,target,'-n'])

    for name, palette_flags, char_flags in [
        ('data/sbox_gra',('-bitdepth','4','-nopad','-pcmp'),('-version101','-sopc')),
        ('graphic/camera_viewfinder',(),('-version101','-sopc')),
        ('fielddata/graphic/preview_graphic/preview_graphic',('-bitdepth','8','-nopad'),('-version101','-sopc','-bitdepth','8'))]:
        source = ROOT / 'files' / name
        target = output / (name + '.narc'); target.parent.mkdir(parents=True, exist_ok=True)
        names = (source / '.narcorder').read_text().splitlines()
        with tempfile.TemporaryDirectory(dir=work) as temporary:
            staging = Path(temporary) / 'members'; staging.mkdir()
            raw = Path(temporary) / 'raw'; raw.mkdir()
            for member in names:
                path = Path(member)
                if path.is_absolute() or '..' in path.parts or len(path.parts) != 1:
                    raise ValueError(f'Unsafe source archive member: {member}')
                destination = staging / member
                uncompressed = path.with_suffix('') if path.suffix == '.lz' else path
                intermediate = raw / uncompressed
                if not intermediate.exists():
                    if uncompressed.suffix == '.NCLR':
                        run([gfx,source / uncompressed.with_suffix('.png'),intermediate,*palette_flags])
                    elif uncompressed.suffix == '.NCGR':
                        run([gfx,source / uncompressed.with_suffix('.png'),intermediate,*char_flags])
                    else: shutil.copyfile(source / uncompressed, intermediate)
                if path.suffix == '.lz': run([gfx,intermediate,destination,'-nopad'])
                else: shutil.copyfile(intermediate,destination)
            run([pack,'-cPf',target,'-T',source / '.narcorder',staging])

    target = output / 'poketool/pokegra/otherpoke.narc'; target.parent.mkdir(parents=True, exist_ok=True)
    with tempfile.TemporaryDirectory(dir=work) as temporary:
        staging = Path(temporary)
        names = []
        for line in (ROOT / 'files/poketool/pokegra/otherpoke.txt').read_text().splitlines():
            args = shlex.split(line)
            if not args: continue
            source, destination = Path(args[0]), Path(args[1])
            if source.is_absolute() or '..' in source.parts or not str(source).startswith('files/poketool/pokegra/otherpoke/'):
                raise ValueError(f'Unsafe alternate sprite source: {source}')
            if destination.parent != Path('files/poketool/pokegra/build-otherpoke') or destination.name in names:
                raise ValueError(f'Invalid alternate sprite output: {destination}')
            names.append(destination.name)
            run([gfx,ROOT / source,staging / destination.name,*args[2:]])
        if names != sorted(names) or len(names) != 261:
            raise ValueError('Alternate sprite order changed; check original archive recipe')
        run([pack,'-cPf',target,staging])
    temporary = stamp.with_suffix('.new')
    temporary.write_text(json.dumps(dict(inputs=fingerprint,outputs=digest(products)),indent=2)+'\n'); temporary.replace(stamp)
    print(f'Built {len(ARCHIVES)} original source archives with native tools.')
    return {name + '.naix' for name in ARCHIVES}
