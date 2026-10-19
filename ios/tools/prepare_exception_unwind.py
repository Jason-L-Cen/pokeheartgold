# SPDX-License-Identifier: GPL-3.0-or-later
"""Restore wasm2c's bounded-call counter after native exception longjmp."""
import re


def restore_exception_depth(source):
    output=[]; active=[]; declarations=tries=restored=0
    for line in source.splitlines():
        output.append(line)
        declaration=re.fullmatch(r'(\s*)WASM_RT_UNWIND_TARGET (\w+);',line)
        if declaration:
            indent,target=declaration.groups(); declarations+=1
            output.extend(['#if WASM_RT_STACK_DEPTH_COUNT',indent+'const uint32_t '+target+'_depth = wasm_rt_call_stack_depth;','#endif'])
        begin=re.fullmatch(r'(\s*)if \(!wasm_rt_try\((\w+)\)\) \{',line)
        if begin:
            active.append(begin.groups()); tries+=1
        elif active and line==active[-1][0]+'} else {':
            indent,target=active.pop(); restored+=1
            output.extend(['#if WASM_RT_STACK_DEPTH_COUNT',indent+'  wasm_rt_call_stack_depth = '+target+'_depth;','#endif'])
    if active or declarations!=tries or tries!=restored:
        raise ValueError('Pinned wasm2c exception target layout changed')
    return '\n'.join(output)+'\n',restored
