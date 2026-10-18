# SPDX-License-Identifier: GPL-3.0-or-later
"""Assemble the data-core check corpus; this is not a production JP/US compatibility map."""
import hashlib
import json
import os
from pathlib import Path
import shutil

ROOT = Path(__file__).resolve().parents[2]
base = ROOT / 'ios/OriginalContent'
build = ROOT / 'ios/build-original-host'
manifest = json.loads((base / 'manifest.json').read_text())
destination = build / 'reference-content'
for entry in manifest['files']:
    # US C code refers to US message bank IDs. Keep a source-matched message archive
    # for data-core tests until the production JP/US bank mapping is verified.
    source = build / 'include/msgdata/msg.narc' if entry['path'] == 'a/0/2/7' else base / 'files' / entry['path']
    target = destination / entry['path']
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
print('Prepared data-core check corpus with source-matched US message IDs; original JP pack is unchanged.')
