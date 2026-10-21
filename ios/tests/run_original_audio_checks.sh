#!/bin/bash
set -euo pipefail
cd "$(dirname "$0")/../.."
mkdir -p ios/build-original-host
xcrun clang -std=c11 -O1 -g -fobjc-arc -Werror -fsanitize=address,undefined -fno-sanitize-recover=all \
    -DHEARTGOLD_ORIGINAL_APP_CHECK ios/tests/check_original_audio.m -framework Foundation -framework AVFAudio -o ios/build-original-host/original-audio-check
ios/build-original-host/original-audio-check
xcrun clang -std=c11 -O1 -g -fobjc-arc -Werror -fsanitize=thread \
    -DHEARTGOLD_ORIGINAL_APP_CHECK ios/tests/check_original_audio.m -framework Foundation -framework AVFAudio -o ios/build-original-host/original-audio-thread-check
ios/build-original-host/original-audio-thread-check
