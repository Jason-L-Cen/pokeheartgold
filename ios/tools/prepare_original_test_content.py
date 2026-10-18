# SPDX-License-Identifier: GPL-3.0-or-later
"""Assemble the data-core check corpus; this is not a production JP/US compatibility map."""
import hashlib
import json
import os
from pathlib import Path
import shutil

from build_original_resources import archive_paths

ROOT = Path(__file__).resolve().parents[2]
base = ROOT / 'ios/OriginalContent'
build = ROOT / 'ios/build-original-host'
manifest = json.loads((base / 'manifest.json').read_text())
destination = build / 'reference-content'
replacements = {'a/0/2/7':build / 'include/msgdata/msg.narc'}
for name,path in archive_paths().items():
    if path in replacements: raise ValueError(f'Duplicate source archive mapping: {path}')
    replacements[path] = build / 'include' / (name+'.narc')
entries = {entry['path'] for entry in manifest['files']} | replacements.keys()
for path in sorted(entries):
    # This explicit source-matched corpus does not alter the original JP content pack.
    source = replacements.get(path,base / 'files' / path)
    target = destination / path
    if target.exists() and os.path.samefile(source, target):
        continue
    target.parent.mkdir(parents=True, exist_ok=True)
    temporary = target.with_name(target.name + '.new')
    if temporary.exists():
        raise ValueError(f'Unexpected build intermediate: {temporary}')
    try:
        os.link(source, temporary)
    except OSError:
        shutil.copyfile(source, temporary)
    os.replace(temporary, target)
assert hashlib.sha256((base / 'files/a/0/2/7').read_bytes()).hexdigest() == next(entry['sha256'] for entry in manifest['files'] if entry['path'] == 'a/0/2/7')
print(f'Prepared data-core check corpus with {len(replacements)} source-matched archives; original JP pack is unchanged.')
