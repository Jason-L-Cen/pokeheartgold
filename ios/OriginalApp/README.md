# Original engine UIKit entry

This target runs the compiled original C/C++ engine in UIKit. It is separate from the earlier Swift gameplay prototype in `App/`. The executable contains native arm64 code generated at build time; it does not load a DS CPU interpreter, JIT or WebAssembly module at runtime.

Build after the strict ABI32 audit and native content preparation:

```sh
HG_ABI32_TOOLS=/absolute/path/to/abi32-tools python3 ios/tools/build_original_app.py
HG_ABI32_TOOLS=/absolute/path/to/abi32-tools python3 ios/tools/build_original_app.py --sdk iphoneos
```

The outputs are `ios/build-original-abi32/app-<sdk>/HeartGoldOriginal.app`. Device builds are ad-hoc signed and require a development signing profile before installation on a physical device. The private verified content pack is copied into the bundle; do not commit or redistribute it. The existing Swift target and its resources are untouched.

The game owns one native thread. UIKit queues input under a condition lock, and the owner applies it between display events. Frames are copied into immutable `NSData`; the main thread displays the latest frame without building a callback backlog. Backgrounding clears held input and suspends the owner and display clock. Foregrounding restarts the clock and observer. Scene disconnection requests the existing original reset/drain path before freeing the module. The original 512 KiB save is stored separately at `Application Support/HeartGoldOriginal/heartgold.sav`; prototype JSON saves are not migrated or overwritten.

For a runnable UIKit check, build with `--check`, install and launch on an iOS Simulator. After about twelve seconds, read `Documents/original-app-check.json` from the app data container. It checks actual button event wiring, combined keys and independent release, bounded logical touch coordinates, clearing input on pause, original-engine frames, suspended frame production, resumed frames, and normal reset cleanup. This check intentionally stops the engine after testing.

Verified on the iOS 27 iPhone 18 Pro Simulator: original Game Freak opening graphics, all eleven runtime checks passing. This does not verify battles, the complete story, performance on physical devices or a complete port.

Remaining limitations: the canvas currently switches between the original scene and touch menu; the intended contextual landscape interface and custom battle action bar are not connected yet. Original audio is advanced but discarded, with no device audio sink. Original soft reset stops this entry instead of automatically rebooting it. Unexpected IRQ/fiber traps still fail fast; other unexpected traps quarantine live module memory to avoid use-after-free of native transfers. Automatic recovery and complete flow validation remain required.
