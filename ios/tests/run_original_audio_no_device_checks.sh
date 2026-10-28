#!/bin/bash
set -euo pipefail
if [ "$#" -ne 2 ]; then
    echo "usage: bash $0 ORIGINAL_AUDIO_HEADER_DIRECTORY OUTPUT_DIRECTORY" >&2
    exit 2
fi
test_source_root="$(cd "$(dirname "$0")/../.." && pwd)"
header_dir="$1"
build_dir="$2"
mkdir -p "$build_dir"
flags=(-std=c11 -O1 -g -fobjc-arc -fblocks -Werror -fsanitize=address,undefined -fno-sanitize-recover=all)
xcrun clang "${flags[@]}" -DHEARTGOLD_ORIGINAL_APP_CHECK -DHEARTGOLD_ORIGINAL_APP_CHECK_NO_AUDIO_DEVICE \
    -I "$header_dir" "$test_source_root/ios/OriginalApp/OriginalAudio.m" \
    "$test_source_root/ios/tests/check_original_audio_no_device.m" -framework Foundation -o "$build_dir/no-audio-device-check"
nm -u "$build_dir/no-audio-device-check" > "$build_dir/undefined-symbols.txt"
otool -L "$build_dir/no-audio-device-check" > "$build_dir/dependencies.txt"
if rg -n 'AVFAudio|AVFoundation|AVAudio|AudioToolbox|CoreAudio' "$build_dir/undefined-symbols.txt" "$build_dir/dependencies.txt"; then
    echo "Audio framework dependency is forbidden in no-device checks" >&2
    exit 1
fi
if xcrun clang "${flags[@]}" -DHEARTGOLD_ORIGINAL_APP_CHECK_NO_AUDIO_DEVICE -I "$header_dir" \
    -fsyntax-only "$test_source_root/ios/OriginalApp/OriginalAudio.m" > "$build_dir/non-check.log" 2>&1; then
    echo "No-device output unexpectedly compiled without CHECK" >&2
    exit 1
fi
rg -q 'No-audio-device output is available only in CHECK builds' "$build_dir/non-check.log"
"$build_dir/no-audio-device-check"
