#!/bin/bash
set -euo pipefail
cd "$(dirname "$0")/../.."
make -s -C tools/gen_fx_consts
tools/gen_fx_consts/gen_fx_consts lib/include/nitro/fx/fx_const.h
mkdir -p ios/build-original-host
python3 ios/tools/prepare_original_headers.py
python3 ios/tools/prepare_bignum_checks.py
python3 ios/tools/prepare_float_checks.py
python3 ios/tests/check_original_indexes.py
python3 tools/o2narc/test_native.py
python3 ios/tools/prepare_original_test_content.py
python3 ios/tests/check_source_resource_identity.py
# Share the device library's source list and compiler settings.
native_cmake="$(command -v cmake || true)"
if [[ -z "$native_cmake" ]]; then native_cmake="../../work/build-tools/bin/cmake"; fi
if [[ ! -x "$native_cmake" ]]; then echo "CMake is required; add it to PATH." >&2; exit 1; fi
if ! "$native_cmake" -S ios/Original -B ios/build-original-host/cmake -G 'Unix Makefiles' -DCMAKE_BUILD_TYPE=Debug >ios/build-original-host/cmake-configure.log 2>&1; then
    tail -80 ios/build-original-host/cmake-configure.log >&2
    exit 1
fi
if ! "$native_cmake" --build ios/build-original-host/cmake --target OriginalDataCheck OriginalWindowIconCheck OriginalModelBindingCheck OriginalAreaAnimationCheck OriginalPokepicMotionCheck --parallel 4 >ios/build-original-host/compiler.log 2>&1; then
    tail -80 ios/build-original-host/compiler.log >&2
    exit 1
fi
cp ios/build-original-host/cmake/OriginalDataCheck ios/build-original-host/abi-check
ios/build-original-host/abi-check "${1:-$(pwd)/ios/build-original-host/reference-content}"
ios/build-original-host/cmake/OriginalWindowIconCheck
ios/build-original-host/cmake/OriginalModelBindingCheck
ios/build-original-host/cmake/OriginalAreaAnimationCheck
ios/build-original-host/cmake/OriginalPokepicMotionCheck
