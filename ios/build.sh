#!/bin/bash
set -euo pipefail
cd "$(dirname "$0")"
cmake_bin="${CMAKE_BIN:-cmake}"
command -v "$cmake_bin" >/dev/null || { echo 'Install CMake or set CMAKE_BIN to its executable.' >&2; exit 1; }
if [[ ! -f Vendor/melonDS/src/NDS.h ]]; then
    git submodule update --init --recursive -- Vendor/melonDS
fi
if [[ "${1:-}" == check ]]; then
    "$cmake_bin" -S . -B build-host -DCMAKE_BUILD_TYPE=Release
    "$cmake_bin" --build build-host --target heartgold-check -j 6
    (cd build-host && "$cmake_bin" -E env CTEST_OUTPUT_ON_FAILURE=1 "$(dirname "$(command -v "$cmake_bin")")/ctest")
else
    "$cmake_bin" -S . -B build -G Xcode \
        -DCMAKE_SYSTEM_NAME=iOS -DCMAKE_OSX_SYSROOT=iphonesimulator \
        -DCMAKE_OSX_DEPLOYMENT_TARGET=16.0
    "$cmake_bin" --build build --config Release --target HeartGoldIOS -- CODE_SIGNING_ALLOWED=NO -quiet
    echo "Built: $(pwd)/build/Release-iphonesimulator/HeartGoldIOS.app"
fi
