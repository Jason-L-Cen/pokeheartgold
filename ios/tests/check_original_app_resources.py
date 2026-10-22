# SPDX-License-Identifier: GPL-3.0-or-later
"""Run real app packaging with original menu data; native compilation/signing are mocked."""
import hashlib
import json
from pathlib import Path
import plistlib
import shutil
import subprocess
import sys
import tempfile
from unittest.mock import patch

ROOT=Path(__file__).resolve().parents[2]
sys.path.insert(0,str(ROOT/'ios/tools'))
import build_original_app as builder
from import_original_content import narc


def skipped_native_command(command,**kwargs):
    if '-o' in command:
        Path(command[command.index('-o')+1]).write_bytes(b'packaging check only; not executable')
    return subprocess.CompletedProcess(command,0,'','')


with tempfile.TemporaryDirectory(prefix='heartgold-app-resources-') as temporary:
    folder=Path(temporary); audit=folder/'audit'; audit.mkdir()
    module=b'packaging check only; not a native engine'
    (audit/'engine.wasm').write_bytes(module)
    (audit/'engine-link.json').write_text(json.dumps(dict(exitCode=0,outputSha256=hashlib.sha256(module).hexdigest())))
    pack=folder/'pack'; pixels=pack/'files/a/0/1/4'; pixels.parent.mkdir(parents=True)
    shutil.copyfile(ROOT/'files/a/0/1/4',pixels)
    original=pixels.read_bytes()
    assert len(narc(pixels))==78
    header=b'packaging check only; not runtime boot metadata'
    with patch.object(builder,'BUILD',folder), patch.object(builder,'verify',return_value=pack), \
         patch.object(builder,'read_boot_header',return_value=header), \
         patch.object(builder,'generate_engine',return_value=([],1,1)), \
         patch.object(builder,'native_build_settings',return_value=([],[])), \
         patch.object(builder.subprocess,'run',side_effect=skipped_native_command), \
         patch.object(sys,'argv',['build_original_app.py','--check']):
        builder.main()
    app=folder/'app-iphonesimulator/HeartGoldOriginal.app'
    menu=plistlib.loads((app/'field-menu.plist').read_bytes())
    assert menu['names']==['POKéDEX','POKéMON','BAG','POKéGEAR','{STRVAR_1 3, 0, 0}',
                           'SAVE','OPTIONS','RETIRE','BALL','BALL','BALL','CHAT','LOG'],menu
    assert menu['hitboxes']==[[16,22,60,32],[16,62,60,32],[16,102,60,32],[16,142,60,32],
                              [96,22,60,32],[96,62,60,32],[96,102,60,32],
                              [203,8,52,31],[203,46,52,31],[168,144,87,44],[184,86,68,48]],menu
    assert (app/'OriginalContent/files/a/0/1/4').read_bytes()==original
    assert (app/'OriginalContent/boot-header.bin').read_bytes()==header
    assert len(plistlib.loads((app/'battle-move-names.plist').read_bytes()))==468
    assert len(plistlib.loads((app/'battle-species-names.plist').read_bytes()))==496
    assert 'sTouchscreenMenu' not in Path(builder.__file__).read_text()
print('PASS: actual app packaging preserves original menu pixels, 13 labels, 11 touch rectangles and battle name tables; native build/signing not exercised.')
