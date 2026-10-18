"""Verify rebuilt source archive members against the supplied ROM content pack."""
import hashlib
import json
from pathlib import Path
import sys

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0,str(ROOT / 'ios/tools'))
from build_original_resources import archive_paths
from import_original_content import narc

records = []
for name,path in archive_paths().items():
    original = ROOT / 'ios/OriginalContent/files' / path
    rebuilt = ROOT / 'ios/build-original-host/include' / (name+'.narc')
    expected,actual = narc(original),narc(rebuilt)
    assert len(expected)==len(actual),(name,len(expected),len(actual))
    assert all(left==right for left,right in zip(expected,actual)),f'Source/ROM member mismatch: {name}'
    records.append(dict(archive=name,path=path,members=len(actual),
                        payloadSha256=hashlib.sha256(b''.join(actual)).hexdigest()))
report = ROOT / 'ios/build-original-host/resource-build/identity.json'
report.write_text(json.dumps(dict(scope='member order and bytes; excludes NARC container padding/headers',archives=records),indent=2)+'\n')
print(f'PASS: {sum(record["members"] for record in records)} rebuilt source members match the supplied ROM byte for byte across {len(records)} archives.')
