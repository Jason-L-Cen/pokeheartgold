# SPDX-License-Identifier: GPL-3.0-or-later
"""Verify that shared lobby tags keep their parameter slot across the C/C++ boundary."""
from pathlib import Path
import re
import subprocess
import sys

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools'))
from check_abi32 import ROOT, SDK


def check(cpp, native):
    tags = ['LobbyTag', 'LobbyStringTypes::RangeDispatch', 'LobbyCharVectorTypes::ResizeDispatch',
            'AssignTag', 'IteratorTag', 'RangeTag', 'TrueTag'] if cpp else ['LobbyTag']
    source = '#include "lobby_vector.h"\n'
    if cpp:
        source += '#include "overlay45_format_buffer_private.h"\n#include "lobby_cpp_types.h"\n'
    for i, tag in enumerate(tags):
        source += f'typedef char TagLayout{i}[(sizeof({tag}) == 1) ? 1 : -1];\n'
        source += ('extern "C" ' if cpp else '') + f'int TagProbe{i}(int value, {tag} tag) {{ return value; }}\n'
    command = [str(SDK / ('bin/clang++' if cpp else 'bin/clang')), '--target=wasm32-wasip1',
               '-std=gnu++98' if cpp else '-std=gnu11', '-O2', '-DHEARTGOLD_NATIVE', '-DSDK_ARM9',
               '-Iinclude', '-Ilib/include', '-S', '-o', '-', '-x', 'c++' if cpp else 'c', '-']
    if native:
        command.insert(4, '-DHEARTGOLD_ABI32')
    result = subprocess.run(command, cwd=ROOT, input=source, capture_output=True, text=True)
    if result.returncode:
        raise RuntimeError(result.stderr)
    expected = 'i32, i32' if native or not cpp else 'i32'
    for i, tag in enumerate(tags):
        signatures = set(re.findall(r'\.functype\s+TagProbe' + str(i) + r'\s+\(([^)]*)\)', result.stdout))
        if signatures != {expected}:
            raise AssertionError((tag, cpp, native, signatures, expected))


def check_assignment(native):
    checks=[('LobbyCharVector_AssignIterRange',name) for name in ['lobby_char_vector_assign.cpp',
        'ppw_lobby_parse_message.cpp','ppw_lobby_update_my_profile.cpp','ppw_lobby_players_set_self.cpp']]
    checks += [('LobbyCharVector_InitRange',name) for name in ['lobby_char_vector_init_range.cpp',
        'ppw_lobby_update_my_profile.cpp','ppw_lobby_players_set_self.cpp']]
    checks += [('LobbyU32Vector_AssignFromSet','lobby_chat_remove_channel.cpp')]
    assignment_expected='i32, i32, i32, i32' if native else 'i32, i32, i32'
    assembly={}
    for symbol,name in checks:
        expected='i32, i32, i32' if symbol=='LobbyU32Vector_AssignFromSet' else assignment_expected
        command=[str(SDK/'bin/clang++'),'--target=wasm32-wasip1','-std=gnu++98','-O2',
                 '-fms-extensions','-Wno-microsoft','-Wno-unknown-pragmas','-DHEARTGOLD_NATIVE',
                 '-DSDK_ARM9','-Iinclude','-Ilib/include','-Ilib/include/cw',
                 '-S','-o','-',str(ROOT/'src/lobby'/name)]
        if native:command.insert(4,'-DHEARTGOLD_ABI32')
        if name not in assembly:
            result=subprocess.run(command,cwd=ROOT,capture_output=True,text=True)
            if result.returncode:raise RuntimeError(result.stderr)
            assembly[name]=result.stdout
        signatures=set(re.findall(r'\.functype\s+'+symbol+r'\s+\(([^)]*)\)',assembly[name]))
        if signatures!={expected}:raise AssertionError((name,symbol,native,signatures,expected))


def check_native_calls():
    flags=['--target=wasm32-wasip1','-O2','-fwrapv','-fno-strict-aliasing','-fms-extensions','-Wno-microsoft','-Wno-unknown-pragmas']
    flags+=['-D'+d for d in ['HEARTGOLD_NATIVE','HEARTGOLD_ABI32','SDK_ARM9','SDK_CW','SDK_CODE_ARM','SDK_TS','_NITRO','PM_KEEP_ASSERTS']]
    for p in [ROOT/'ios/build-original-host/include','include','files','src','lib/include','lib/include/cw','lib/include/nitro','lib/include/nnsys','lib/include/nitro/os','include/library','lib/NitroSDK/include','lib/NitroSDK/include-pokediamond']:flags+=['-I',str(p)]
    from prepare_lobby_parts import prepare
    prepared=prepare(ROOT/'ios/build-original-abi32/lobby-call-abi')[0]
    checks=[(ROOT/'src/lobby/lobby_chat_channel_callbacks.cpp','LobbyRequestMap_Find',('i32, i32, i32','')),
            (ROOT/'src/lobby/lobby_request_map_find.c','LobbyRequestMap_Find',('i32, i32, i32','')),
            (ROOT/'src/lobby/ppw_lobby_player_set_destroy_nodes.c','LobbyStringVector_RecommendSize',('i32, i32','i32')),
            (prepared,'LobbyStringVector_RecommendSize',('i32, i32','i32')),
            (ROOT/'src/lobby/ppw_lobby_start_recruit.cpp','LobbyString_FormatN',('i32, i32, i32, i32','')),
            (ROOT/'src/lobby/lobby_string_format.c','LobbyString_FormatN',('i32, i32, i32, i32',''))]
    checks += [(prepared,name,('i32','i32')) for name in ['PPWLobby_TimerUploadUserProfile',
        'PPWLobby_TimerUploadSystemProfile','PPWLobby_TimerSendRecruitInfo','PPWLobby_TimerPollChannelMode',
        'PPWLobby_TimerRequestChannelTime','PPWLobby_TimerRequestLobbyData']]
    checks += [(source,name,('i32, i32','i32')) for name in ['PPWLobbyPlayers_Add','PPWLobbyPlayers_Remove']
               for source in [prepared,ROOT/'src/lobby/ppw_lobby_players.c']]
    checks += [(ROOT/'src/lobby'/name,'PPWLobbyChannel_GetName',('i32, i32','')) for name in
               ['ppw_lobby_leave_sub_channel.cpp','ppw_lobby_get_time_info.c']]
    checks += [(ROOT/'src/lobby'/name,'LobbyU32Vector_AssignFromSet',('i32, i32, i32','')) for name in
               ['lobby_chat_remove_channel.cpp','lobby_u32_vector_assign_from_set.c']]
    assembly={}
    for source,symbol,expected in checks:
        if source not in assembly:
            cpp=source.suffix=='.cpp'
            command=[str(SDK/('bin/clang++' if cpp else 'bin/clang')),'-std=gnu++98' if cpp else '-std=gnu11',*flags,'-S','-o','-',str(source)]
            result=subprocess.run(command,cwd=ROOT,capture_output=True,text=True)
            if result.returncode:raise RuntimeError(result.stderr)
            assembly[source]=result.stdout
        signatures=set(re.findall(r'\.functype\s+'+symbol+r'\s+\(([^)]*)\)\s*->\s*\(([^)]*)\)',assembly[source]))
        if signatures!={expected}:raise AssertionError((source,symbol,signatures,expected))
    result=subprocess.run([str(SDK/'bin/clang++'),'-std=gnu++98',*flags,'-Xclang','-disable-llvm-passes',
                           '-S','-emit-llvm','-o','-',str(prepared)],cwd=ROOT,capture_output=True,text=True)
    if result.returncode:raise RuntimeError(result.stderr)
    definitions=re.findall(r'(?m)^define [^\n]* @LobbyTimerMap_InsertNode\(([^)\n]*)\)',result.stdout)
    calls=re.findall(r'\bcall ptr @LobbyTimerMap_InsertNode\(([^)\n]*)\)',result.stdout)
    if len(definitions)!=1 or not calls:raise AssertionError('Actual timer insertion definition/call is missing')
    for parameters in definitions+calls:
        types=[parameter.strip().split()[0] for parameter in parameters.split(',')]
        if types!=['ptr','ptr','i32','i32','ptr']:raise AssertionError(('LobbyTimerMap_InsertNode LLVM types',types))


if __name__ == '__main__':
    for cpp in [False, True]:
        for native in [False, True]:
            check(cpp, native)
    for native in [False,True]:check_assignment(native)
    check_native_calls()
    print('PASS: native lobby tags, assignment caller/callee slots, C output-iterator ABI and actual timer insertion LLVM types; ordinary empty tags retained.')
