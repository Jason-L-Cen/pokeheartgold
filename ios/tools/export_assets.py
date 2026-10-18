#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-or-later
"""Convert the existing HGSS assets for the native iOS slice; never read a ROM."""
import argparse
import hashlib
import json
import math
from pathlib import Path
import shutil
import struct
import subprocess
from PIL import Image
from import_original_content import narc

ROOT = Path(__file__).resolve().parents[2]
ACTORS = [('hero', 69), ('doctor', 54), ('follower-16', 313), ('follower-19', 316),
          ('follower-152', 450), ('follower-155', 454), ('follower-158', 457),
          ('follower-161', 460), ('follower-163', 462)]


def write_json(path, value):
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(value, separators=(',', ':'), ensure_ascii=False) + '\n')


def extract(work):
    land = narc(ROOT / 'files/a/0/6/5')
    buildings = narc(ROOT / 'files/a/0/4/0')
    textures = narc(ROOT / 'files/a/0/4/4')
    prop_textures = narc(ROOT / 'files/a/0/7/0')
    matrix = (ROOT / 'files/fielddata/mapmatrix/map_matrix/map_matrix_0000_EVERYWHERE.bin').read_bytes()
    width, height, has_headers, has_heights, name_length = matrix[:5]
    count = width * height
    offset = 5 + name_length
    headers = struct.unpack_from(f'<{count}H', matrix, offset)
    offset += count * 2
    altitudes = matrix[offset:offset + count]
    offset += count
    models = struct.unpack_from(f'<{count}H', matrix, offset)
    assert has_headers and has_heights
    # Original global tiles: New Bark (672..703,384..415), Route 29 west of it.
    world = dict(width=160, height=96, originX=576, originZ=352, cells=[], permissions=[32768] * (160 * 96))
    used_models, used_props = set(), set()
    for z in range(11, 14):
        for x in range(18, 23):
            index = z * width + x
            model = models[index]
            blob = land[model]
            permissions_size, props_size, model_size, collision_size, sound = struct.unpack_from('<IIIII', blob)
            offset = 20 + (sound >> 16)
            assert permissions_size == 2048
            permissions = struct.unpack_from('<1024H', blob, offset)
            offset += permissions_size
            props = []
            for at in range(offset, offset + props_size, 48):
                record = struct.unpack_from('<10i', blob, at)
                props.append(dict(model=f'prop-{record[0]}', position=[v / 4096 for v in record[1:4]],
                                  rotation=[v / 4096 for v in record[4:7]], scale=[v / 4096 for v in record[7:10]]))
                used_props.add(record[0])
            offset += props_size
            model_data = blob[offset:offset + model_size]
            assert model_data[:4] == b'BMD0' and offset + model_size + collision_size <= len(blob)
            used_models.add(model)
            (work / f'land-{model}.nsbmd').write_bytes(model_data)
            cell_x, cell_z = (x - 18) * 32, (z - 11) * 32
            world['cells'].append(dict(model=f'land-{model}', x=cell_x, z=cell_z, altitude=int(altitudes[index]), props=props))
            if headers[index] in (33, 60):
                for row in range(32):
                    start = (cell_z + row) * world['width'] + cell_x
                    world['permissions'][start:start + 32] = permissions[row * 32:(row + 1) * 32]
    for model in used_props:
        (work / f'prop-{model}.nsbmd').write_bytes(buildings[model])
    (work / 'ground.nsbtx').write_bytes(textures[2])
    (work / 'buildings.nsbtx').write_bytes(prop_textures[0])
    for name, model in ACTORS:
        data = (ROOT / f'files/data/mmodel/mmodel/mmodel_{model:08}.NSBTX').read_bytes()
        if name.startswith('follower-'):
            # Select only palette 0 (normal) in the temporary conversion copy.
            data = bytearray(data)
            base = struct.unpack_from('<I', data, 16)[0]
            at = base + struct.unpack_from('<I', data, base + 52)[0]
            count = data[at + 1]
            assert count == 2
            entry = at + 16 + count * 4
            names_at = entry + count * 4
            selected = data[at:at + 16] + data[entry - 4:entry + 4] + data[names_at:names_at + 16]
            selected[1] = 1
            data[at:at + len(selected)] = selected
        (work / f'{name}.nsbtx').write_bytes(data)
    write_json(work / 'world-source.json', world)
    print(f'Extracted {len(used_models)} terrain models, {len(used_props)} prop models, original collision grid.')


def glb(path):
    blob = path.read_bytes()
    if blob[:4] != b'glTF':
        raise ValueError(f'Invalid glTF: {path}')
    size = struct.unpack_from('<I', blob, 12)[0]
    doc = json.loads(blob[20:20 + size])
    binary = blob[28 + size:]

    def read(index):
        item = doc['accessors'][index]
        view = doc['bufferViews'][item['bufferView']]
        count = {'SCALAR': 1, 'VEC2': 2, 'VEC3': 3, 'VEC4': 4, 'MAT4': 16}[item['type']]
        code = {5121: 'B', 5123: 'H', 5125: 'I', 5126: 'f'}[item['componentType']]
        at = view.get('byteOffset', 0) + item.get('byteOffset', 0)
        stride = view.get('byteStride', struct.calcsize(code) * count)
        result = [struct.unpack_from('<' + code * count, binary, at + i * stride) for i in range(item['count'])]
        if item.get('normalized'):
            divisor = 255 if code == 'B' else 65535
            result = [tuple(v / divisor for v in row) for row in result]
        return result
    return doc, read


def package_meshes(work, output):
    assets = []
    textures = {}
    terrain = {}
    for path in sorted(work.glob('*/*.glb')):
        model = path.parent.name
        doc, read = glb(path)
        for mesh in doc['meshes']:
            for primitive in mesh['primitives']:
                assert primitive.get('mode', 4) == 4
                attributes = primitive['attributes']
                positions = read(attributes['POSITION'])
                uv = read(attributes['TEXCOORD_0'])
                colors = read(attributes['COLOR_0'])
                material = doc['materials'][primitive['material']]
                pbr = material.get('pbrMetallicRoughness', {})
                tint = pbr.get('baseColorFactor', [1, 1, 1, 1])
                texture = ''
                if 'baseColorTexture' in pbr:
                    tex = doc['textures'][pbr['baseColorTexture']['index']]
                    source = path.parent / doc['images'][tex['source']]['uri']
                    texture = hashlib.sha256(source.read_bytes()).hexdigest()[:16] + '.png'
                    textures[texture] = source
                vertices = bytearray()
                indices = [i[0] for i in read(primitive['indices'])]
                if model.startswith('land-'):
                    terrain.setdefault(model, []).extend([positions[i] for i in indices[j:j + 3]] for j in range(0, len(indices), 3))
                for index in indices:
                    color = colors[index]
                    values = (*positions[index], *uv[index], *(color[i] * tint[i] for i in range(3)), tint[3])
                    assert all(math.isfinite(v) for v in values)
                    vertices.extend(struct.pack('<9f', *values))
                name = f'mesh-{len(assets)}.bin'
                (output / name).write_bytes(vertices)
                assets.append(dict(model=model, file=name, texture=texture, count=len(vertices) // 36,
                                   transparent=material.get('alphaMode') == 'BLEND'))
    for name, source in textures.items():
        shutil.copyfile(source, output / name)
    for name, _ in ACTORS:
        prefix = 'tsure_poke' if name.startswith('follower-') else name
        images = sorted((work / name).glob(f'{prefix}_*.png'), key=lambda p: int(p.stem.rsplit('_', 1)[-1]))
        assert images, name
        for i, image in enumerate(images):
            shutil.copyfile(image, output / f'{name}-{i}.png')
    write_json(output / 'meshes.json', assets)
    world = json.loads((work / 'world-source.json').read_text())
    # shortcut: sample the lowest terrain surface; replace with BDHC queries for bridges and stacked floors.
    heights = [None] * (world['width'] * world['height'])
    for cell in world['cells']:
        for triangle in terrain[cell['model']]:
            points = [(v[0] / 16 + 16, v[1] / 16 + cell['altitude'] / 2, v[2] / 16 + 16) for v in triangle]
            a, b, c = points
            denominator = (b[2] - c[2]) * (a[0] - c[0]) + (c[0] - b[0]) * (a[2] - c[2])
            if abs(denominator) < 0.00001:
                continue
            for z in range(max(0, math.floor(min(v[2] for v in points))), min(32, math.ceil(max(v[2] for v in points)))):
                for x in range(max(0, math.floor(min(v[0] for v in points))), min(32, math.ceil(max(v[0] for v in points)))):
                    u = ((b[2] - c[2]) * (x + .5 - c[0]) + (c[0] - b[0]) * (z + .5 - c[2])) / denominator
                    v = ((c[2] - a[2]) * (x + .5 - c[0]) + (a[0] - c[0]) * (z + .5 - c[2])) / denominator
                    if u >= -0.00001 and v >= -0.00001 and u + v <= 1.00001:
                        y = u * a[1] + v * b[1] + (1 - u - v) * c[1]
                        index = (cell['z'] + z) * world['width'] + cell['x'] + x
                        heights[index] = min(heights[index], y) if heights[index] is not None else y
    world['heights'] = [round(y or 0, 5) for y in heights]
    write_json(output / 'world.json', world)
    assert len(world['permissions']) == world['width'] * world['height']
    assert world['permissions'][45 * world['width'] + 119] & 32768 == 0
    assert {a['model'] for a in assets} >= {c['model'] for c in world['cells']}
    print(f'Packaged {len(assets)} native triangle batches and {len(textures)} original textures.')


def package_rules(output):
    personal = json.loads((ROOT / 'files/poketool/personal/personal.json').read_text())['baseStats']
    ids = {p['species']: i for i, p in enumerate(personal)}
    tables = narc(ROOT / 'files/poketool/waza/waza_tbl.narc')
    learnsets = narc(ROOT / 'files/poketool/personal/wotbl.narc')
    encounters = next(e for e in json.loads((ROOT / 'files/fielddata/encountdata/gs_enc_data.json').read_text())['encounters'] if e['map'] == 'R29')['land']
    names = {16: '波波', 19: '小拉达', 152: '菊草叶', 155: '火球鼠', 158: '小锯鳄', 161: '尾立', 163: '咕咕'}
    species, moves = [], {}
    move_names = {10: '抓', 33: '撞击', 39: '摇尾巴', 43: '瞪眼', 45: '叫声', 52: '火花', 55: '水枪', 64: '啄', 75: '飞叶快刀', 77: '毒粉', 81: '吐丝', 95: '催眠术', 98: '电光一闪', 99: '愤怒', 111: '变圆', 116: '聚气', 165: '挣扎', 193: '识破', 202: '超级吸取', 204: '撒娇', 235: '光合作用', 281: '哈欠'}
    for number, name in names.items():
        entry = personal[number]
        levels = []
        for (word,) in struct.iter_unpack('<H', learnsets[number]):
            if word == 65535:
                break
            move, level = word & 511, word >> 9
            if level > 10:
                continue
            levels.append(dict(move=move, level=level))
            effect, category, power, kind, accuracy, pp = struct.unpack_from('<H5B', tables[move])
            moves[str(move)] = dict(id=move, name=move_names.get(move, f'招式 {move}'), effect=effect,
                                    category=category, power=power, type=kind, accuracy=accuracy, pp=pp,
                                    priority=struct.unpack_from('<b', tables[move], 10)[0])
        species.append(dict(id=number, name=name, hp=entry['hp'], attack=entry['atk'], defense=entry['def'],
                            speed=entry['speed'], specialAttack=entry['spatk'], specialDefense=entry['spdef'],
                            types=entry['types'], expYield=entry['expYield'], catchRate=entry['catchRate'],
                            growth=entry['growthRate'], learnset=levels))
        for side in ['front', 'back']:
            source = ROOT / f'files/poketool/pokegra/pokegra/{number:04}/male/{side}.png'
            if side == 'back':
                # Like the existing hd-artwork source_image tool, use the front's normal palette for the back indices.
                with Image.open(source) as back, Image.open(source.with_name('front.png')) as front:
                    assert back.mode == front.mode == 'P'
                    back.putpalette(front.getpalette())
                    back.save(output / f'pokemon-{number}-{side}.png')
            else:
                shutil.copyfile(source, output / f'pokemon-{number}-{side}.png')
    slots = []
    def version(value):
        return value.get('HEARTGOLD') if isinstance(value, dict) else value
    for slot, weight in zip(encounters['mons'], [20, 20, 10, 10, 10, 10, 5, 5, 4, 4, 1, 1]):
        slots.append(dict(level=version(slot['level']), weight=weight,
                          species={time: ids[version(name).removeprefix('SPECIES_')] for time, name in slot['species'].items()}))
    assert sum(s['weight'] for s in slots) == 100
    effect, category, power, kind, accuracy, pp = struct.unpack_from('<H5B', tables[165])
    moves['165'] = dict(id=165, name='挣扎', effect=effect, category=category, power=power, type=kind, accuracy=accuracy, pp=pp, priority=0)
    write_json(output / 'rules.json', dict(species=species, moves=moves, encounterRate=encounters['rate'], encounters=slots))
    print(f'Copied {len(species)} species and {len(moves)} moves from original tables.')


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--work', type=Path, required=True)
    parser.add_argument('--apicula', type=Path, required=True)
    parser.add_argument('--output', type=Path, default=ROOT / 'ios/Resources')
    args = parser.parse_args()
    args.work.mkdir(parents=True, exist_ok=True)
    extract(args.work)
    for model in sorted(args.work.glob('*.nsbmd')):
        subprocess.run([str(args.apicula), 'convert', '-f=glb', str(model),
                        str(args.work / 'ground.nsbtx'), str(args.work / 'buildings.nsbtx'),
                        '-o', str(args.work / model.stem), '--overwrite'], check=True, stdout=subprocess.DEVNULL)
    for name, _ in ACTORS:
        subprocess.run([str(args.apicula), 'convert', str(args.work / f'{name}.nsbtx'), '--more-textures',
                        '-o', str(args.work / name), '--overwrite'], check=True, stdout=subprocess.DEVNULL)
    args.output.mkdir(parents=True, exist_ok=True)
    package_meshes(args.work, args.output)
    package_rules(args.output)
    sources = [ROOT / f'files/a/{p}' for p in ['0/6/5', '0/4/0', '0/4/4', '0/7/0']]
    sources += [ROOT / f'files/data/mmodel/mmodel/mmodel_{number:08}.NSBTX' for _, number in ACTORS]
    sources += [ROOT / f'files/{p}' for p in ['fielddata/mapmatrix/map_matrix/map_matrix_0000_EVERYWHERE.bin',
                'poketool/personal/personal.json', 'poketool/personal/wotbl.narc', 'poketool/waza/waza_tbl.narc', 'fielddata/encountdata/gs_enc_data.json']]
    sources += [ROOT / f'files/poketool/pokegra/pokegra/{number:04}/male/{side}.png'
                for number in [16, 19, 152, 155, 158, 161, 163] for side in ['front', 'back']]
    write_json(args.output / 'sources.json', {str(path.relative_to(ROOT)): hashlib.sha256(path.read_bytes()).hexdigest() for path in sources})


if __name__ == '__main__':
    main()
