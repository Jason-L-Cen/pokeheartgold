#!/bin/bash
set -euo pipefail
cd "$(dirname "$0")"
if [[ "${1:-}" == check ]]; then
    mkdir -p build-native-host
    xcrun swiftc -Onone -parse-as-library App/Game.swift tests/check.swift -o build-native-host/native-check
    build-native-host/native-check "$(pwd)/Resources"
else
    cmake_bin="${CMAKE_BIN:-cmake}"
    command -v "$cmake_bin" >/dev/null || { echo 'Install CMake or set CMAKE_BIN to its executable.' >&2; exit 1; }
    "$cmake_bin" -S . -B build-native -G Xcode \
        -DCMAKE_SYSTEM_NAME=iOS -DCMAKE_OSX_SYSROOT=iphonesimulator \
        -DCMAKE_OSX_DEPLOYMENT_TARGET=16.0
    "$cmake_bin" --build build-native --config Release --target HeartGoldNative -- CODE_SIGNING_ALLOWED=NO -quiet
    echo "Built: $(pwd)/build-native/Release-iphonesimulator/HeartGoldNative.app"
fi
