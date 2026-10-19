# SPDX-License-Identifier: GPL-3.0-or-later
"""Keep legacy MSL support symbols separate from the SDK's native exception ABI."""
import hashlib

_functions=[
    '_ZNSt9exceptionD0Ev','_ZNSt9exceptionD1Ev','_ZNSt9exceptionD2Ev',
    '_ZNSt11logic_errorD0Ev','_ZNSt11logic_errorD1Ev',
    '_ZNSt12length_errorD0Ev','_ZNSt12length_errorD1Ev',
    '_ZNSt9bad_allocD0Ev','_ZNSt9bad_allocD1Ev','_ZNKSt9bad_alloc4whatEv',
    '_ZNSt13bad_exceptionD0Ev','_ZNSt13bad_exceptionD1Ev','_ZNKSt13bad_exception4whatEv',
    '_ZNSt9type_infoD2Ev','_ZNKSt9type_infoeqERKS_',
    '_ZN10__cxxabiv117__class_type_infoD0Ev','_ZN10__cxxabiv117__class_type_infoD1Ev',
    '_ZN10__cxxabiv117__class_type_infoD2Ev','_ZN10__cxxabiv120__si_class_type_infoD0Ev',
    '_ZN10__cxxabiv120__si_class_type_infoD1Ev',
    '_ZSt17__throw_bad_allocv','_ZSt9terminatev','_ZSt10unexpectedv']
_data=['_ZTVSt11logic_error','_ZTVSt12length_error','_ZTVSt9bad_alloc','__stdio_exit','_ZSt7nothrow']
MSL_NATIVE_SYMBOLS={name:'HGNativeLegacy_'+hashlib.sha256(name.encode()).hexdigest()[:24] for name in _functions+_data}


def legacy_flags(source):
    return ['-D'+name+'='+alias for name,alias in MSL_NATIVE_SYMBOLS.items()] if str(source).startswith('lib/MSL_C/src/') and str(source).endswith('.c') else []


def remap_legacy_data(arm):
    renamed={}
    for symbol in arm['symbols']:
        name=symbol['name']
        if name in MSL_NATIVE_SYMBOLS:
            symbol['name']=MSL_NATIVE_SYMBOLS[name]; renamed[name]=symbol['name']
    return renamed
