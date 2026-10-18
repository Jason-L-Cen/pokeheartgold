#!/bin/bash
set -euo pipefail
cd "$(dirname "$0")/../.."
make -s -C tools/gen_fx_consts
tools/gen_fx_consts/gen_fx_consts lib/include/nitro/fx/fx_const.h
mkdir -p ios/build-original-host
python3 ios/tools/prepare_original_headers.py
python3 ios/tools/prepare_original_test_content.py
# Match the original ARM code's wrapping signed arithmetic and buffer aliasing.
if ! xcrun clang -std=gnu11 -fwrapv -fno-strict-aliasing -fms-extensions -Wno-microsoft -Wno-unknown-pragmas \
    -Wno-incompatible-library-redeclaration \
    -DHEARTGOLD_NATIVE -DSDK_ARM9 -DSDK_CW -DSDK_CODE_ARM -DSDK_TS -D_NITRO \
    -DPLATFORM_INTRINSIC_FUNCTION_BIT_CLZ32=__builtin_clz -DPM_KEEP_ASSERTS \
    -Iios/build-original-host/include -Iinclude -Ifiles -Ilib/include -Ilib/include/cw -Ilib/include/nitro \
    -Ilib/include/nnsys -Ilib/include/nitro/os -Iinclude/library \
    -ffunction-sections -fdata-sections -Wl,-dead_strip \
    -fsanitize=address,undefined -fno-sanitize-recover=all -fno-omit-frame-pointer -g \
    src/pokemon.c src/math_util.c src/filesystem.c src/heap.c ios/Original/NativeMemory.c ios/Original/NativeFile.c ios/Original/NativeOSMemory.c \
    src/seal_case.c src/pm_string.c src/item.c src/move.c src/msgdata.c \
    src/map_section.c src/mail.c src/mail_message.c src/string_util.c src/pm_version.c src/orbit_cursor.c \
    lib/NitroSystem/src/fnd_expheap.c lib/NitroSystem/src/fnd_heapcommon.c \
    lib/NitroSystem/src/fnd_list.c lib/NitroSystem/src/fnd_allocator.c ios/Original/NativeFndData.c \
    ios/Original/tests/abi_check.c ios/Original/tests/environment.c -o ios/build-original-host/abi-check 2>ios/build-original-host/compiler.log; then
    tail -80 ios/build-original-host/compiler.log >&2
    exit 1
fi
ios/build-original-host/abi-check "${1:-$(pwd)/ios/build-original-host/reference-content}"
