# Original engine UIKit entry

This target runs the compiled original C/C++ engine in UIKit. It is separate from the earlier Swift gameplay prototype in `App/`. The executable contains native arm64 code generated at build time; it does not load a DS CPU interpreter, JIT or WebAssembly module at runtime.

Build after the strict ABI32 audit and native content preparation:

```sh
HG_ABI32_TOOLS=/absolute/path/to/abi32-tools python3 ios/tools/build_original_app.py
HG_ABI32_TOOLS=/absolute/path/to/abi32-tools python3 ios/tools/build_original_app.py --sdk iphoneos
```

The outputs are `ios/build-original-abi32/app-<sdk>/HeartGoldOriginal.app`. Device builds are ad-hoc signed and require a development signing profile before installation on a physical device. The private verified content pack is copied into the bundle; do not commit or redistribute it. The existing Swift target and its resources are untouched.

The game owns one native thread. UIKit queues input under a condition lock, and the owner applies it between display events. Frames are copied into immutable `NSData`; the main thread displays the latest frame without building a callback backlog. Backgrounding clears held input and suspends the owner and display clock. Foregrounding restarts the clock and observer. Scene disconnection requests the existing original reset/drain path before freeing the module. The original 512 KiB save is stored separately at `Application Support/HeartGoldOriginal/heartgold.sav`; prototype JSON saves are not migrated or overwritten.

Original 48 kHz stereo synthesis now feeds `AVAudioEngine` through `AVAudioPlayerNode`. PCM is copied into separate float buffers before scheduling; audio callbacks update atomic counters and never call the game or read module memory. The output starts after 2048 queued frames and caps pending data at 8192 frames. Ambient audio respects the system's audio policy. Backgrounding and interruptions pause the native sound clock and the player, so resuming does not synthesize the background interval. An interruption without permission to resume, or headphone disconnection, waits for a user button press before resuming. Output failures are reported. New error-returning APIs are used on iOS 27, with iOS 16–26 compatibility paths.

For a runnable UIKit check, build with `--check`, install and launch on an iOS Simulator. After about twenty-four seconds, read `Documents/original-app-check.json` from the app data container. Its twenty checks cover control events, touch bounds, frames, original nonzero PCM reaching the mixer output, bounded audio queues without drops in the opening, pause/resume of both clocks and output, interruption state handling, and normal reset cleanup. This check intentionally stops the engine after testing. Interruption state checks inject notifications; they do not simulate a physical phone call or headset.

`bash ios/tests/run_original_audio_checks.sh` separately checks PCM limits, signed extrema, independent stereo copies and exact 48 kHz offline AVAudioEngine output under AddressSanitizer/UndefinedBehaviorSanitizer. The ABI32 checks exercise native output callbacks, detachment, explicit/realtime exclusivity and pausing the sound clock.

Verified on the iOS 27 iPhone 18 Pro Simulator: original opening graphics and nonzero original audio in the output graph, all twenty runtime checks passing. This does not verify battles, all music/effects, the complete story, long sessions or physical-device performance and audibility.

Remaining limitations: the canvas currently switches between the original scene and touch menu; the intended contextual landscape interface and custom battle action bar are not connected yet. Long-session clock drift, actual calls/headsets and route recovery need physical-device calibration; the bounded queue records dropped frames rather than allowing unbounded latency. Original soft reset stops this entry instead of automatically rebooting it. Unexpected IRQ/fiber traps still fail fast; other unexpected traps quarantine live module memory to avoid use-after-free of native transfers. Automatic recovery and complete flow validation remain required.
