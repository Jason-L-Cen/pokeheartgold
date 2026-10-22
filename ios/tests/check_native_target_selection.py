# SPDX-License-Identifier: GPL-3.0-or-later
"""Keep native SDK exclusions exact and reject retained hardware/MWCC entry points."""
import copy
import hashlib
from pathlib import Path
import subprocess
import sys
import tempfile
from unittest.mock import patch

ROOT=Path(__file__).resolve().parents[2]
sys.path.insert(0,str(ROOT/'ios/tools'))
from check_abi32 import native_target_selection,native_retained_entries

excluded=[
    'lib/NitroSDK/src/os/os_protectionUnit.c','lib/NitroSDK/src/os/os_protectionRegion.c',
    'lib/NitroSDK/src/os/os_reset_boot.c','lib/NitroSDK/src/os/os_reset_clear32.c',
    'lib/MSL_C/src/msl_fp_runtime.c','lib/MSL_C/src/msl_unexpected.cpp',
    'lib/MSL_C/src/msl_transfer_control.cpp','lib/MSL_C/src/msl_throw.c',
]
units=[dict(file=name,sha256=hashlib.sha256((ROOT/name).read_bytes()).hexdigest(),exitCode=1,log=name+'.log') for name in excluded]
units.append(dict(file='lib/NitroSDK/src/os/os_reset_core.c',sha256='selected-source',exitCode=0,log='selected.log'))
before=copy.deepcopy(units)
flags=['--target=wasm32-wasip1','-DHEARTGOLD_ABI32']
selection=native_target_selection(units,flags)
assert not selection['errors'] and units==before
assert {unit['file'] for unit in selection['excludedLibraryUnits']}==set(excluded)
assert all(unit['nativeSelected'] is False and unit['sha256']==unit['auditedSha256'] and unit['providers'] and unit['reason'] for unit in selection['excludedLibraryUnits'])
assert selection['selectedCompileSummary']['library']==dict(total=1,compiled=1,blocked=0)
with patch('check_abi32.hashlib.sha256') as digest:
    digest.return_value.hexdigest.return_value='changed-source-bytes'
    rejected=native_target_selection(units,flags)
assert len(rejected['errors'])==8 and not rejected['excludedLibraryUnits']
for options in [[],flags+['-UHEARTGOLD_ABI32'],flags+['-DHEARTGOLD_ABI32=0']]:
    rejected=native_target_selection(units,options)
    assert rejected['errors'] and not rejected['excludedLibraryUnits'] and rejected['selectedCompileSummary']['library']['blocked']==8
changed=copy.deepcopy(units); changed[0]['sha256']='changed-after-audit'
rejected=native_target_selection(changed,flags)
assert rejected['errors'] and len(rejected['excludedLibraryUnits'])==7 and rejected['selectedCompileSummary']['library']['blocked']==1
rejected=native_target_selection(units[1:],flags)
assert rejected['errors'] and len(rejected['excludedLibraryUnits'])==7
changed=copy.deepcopy(units); changed.append(dict(file='lib/new_failure.c',sha256='new',exitCode=1,log='new.log'))
rejected=native_target_selection(changed,flags)
assert not rejected['errors'] and rejected['selectedCompileSummary']['library']['blocked']==1

with tempfile.TemporaryDirectory(prefix='heartgold-native-selection-') as temporary:
    module=Path(temporary)/'module.wasm'; module.write_bytes(b'cached-metadata-parser-check')
    for name,forbidden in [('_ll_mul',False),('_fadd',False),('_dsqrt',False),('__cxa_throw',False),
                           ('_ZSt17__throw_bad_allocv',False),('std::__throw_bad_alloc()',False),
                           ('std::tr1::shared_ptr<char>::shared_ptr<char, std::detail::default_delete<char [0]>>(char*, std::detail::default_delete<char [0]>)',False),
                           ('OS_EnableProtectionUnit',True),('OS_SetProtectionRegion2',True),
                           ('OS_SetDPermissionsForProtectionRegion',True),('OSi_DoBoot',True),('OSi_CpuClear32',True),
                           ('__throw',True),('__rethrow',True),('__unexpected',True),('__TransferControl',True),
                           ('_Z17__TransferControlP12ThrowContextP13ExceptionInfoPc',True),('_Z14__ThrowHandlerP12ThrowContext',True),
                           ('__TransferControl(ThrowContext*, ExceptionInfo*, char*)',True),('__ThrowHandler(ThrowContext*)',True)]:
        dump=f'Import[1]:\n - func[0] sig=0 <env.{name}> <- env.raw_entry\nFunction[1]:\n - func[1] sig=0 <NitroMain>\n'
        with patch('check_abi32.subprocess.run',return_value=subprocess.CompletedProcess([],0,dump,'')):
            result=native_retained_entries(module)
        assert result['checked'] and bool(result['forbiddenSymbols'])==forbidden,(name,result)
        assert result['functions']==2 and result['importedFunctions']==1 and result['moduleSha256']==hashlib.sha256(module.read_bytes()).hexdigest()
    dump='Function[1]:\n - func[0] sig=0 <ordinary>\nExport[1]:\n - func[0] <ordinary> -> "__throw"\n'
    with patch('check_abi32.subprocess.run',return_value=subprocess.CompletedProcess([],0,dump,'')):
        assert native_retained_entries(module)['forbiddenSymbols']==['__throw']
    dump='Import[1]:\n - func[0] sig=0 <ordinary> <- env.__throw\nFunction[1]:\n - func[1] sig=0 <NitroMain>\n'
    with patch('check_abi32.subprocess.run',return_value=subprocess.CompletedProcess([],0,dump,'')):
        assert native_retained_entries(module)['forbiddenSymbols']==['env.__throw']
    for dump in ['', 'Function[1]:\n - func[0] sig=0\n',
                 'Function[2]:\n - func[0] sig=0 <NitroMain>\n',
                 'Function[1]:\n - func[3] sig=0 <NitroMain>\n']:
        with patch('check_abi32.subprocess.run',return_value=subprocess.CompletedProcess([],0,dump,'')):
            try: native_retained_entries(module)
            except ValueError: pass
            else: raise AssertionError('Unrecognized or incomplete retained metadata accepted')
print('PASS: eight source-hash/target-bound SDK exclusions preserve raw failures, reject changed/new failures, and block retained MPU/ARM boot/MWCC EH without rejecting native EH/numeric providers.')
