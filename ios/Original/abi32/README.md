# Original ABI32 native C experiment

This build path preserves original 32-bit pointers and byte offsets while producing
ordinary arm64 C objects for iOS. It is an engine-porting experiment, not the App's entry point.

1. WASI SDK 34.0 compiles original game C/C++ to a 32-bit intermediate module.
2. WABT 1.0.42 `wasm2c` converts that intermediate to C.
3. Apple Clang compiles the C and memory/function-table support into native code.

The resulting executable runs native C functions. It has no DS CPU interpreter, JIT,
browser, or runtime `.wasm` loader. The support library uses ordinary allocated memory
with explicit bounds checks; mmap and guard-page signal recovery are disabled.

```sh
python3 ios/tools/check_abi32.py
python3 ios/tools/check_abi32.py --audit
```

The first run downloads checksum-pinned Apple Silicon macOS build tools into
`ios/build-original-abi32/tools/`. `HG_ABI32_TOOLS` can select an existing cache.
Generated C, intermediates, logs and the iPhoneOS archive remain in
`ios/build-original-abi32/`, outside Git.

The native ASan/UBSan check exercises original RNG/MT and encryption/checksum code,
136/236-byte Pokémon records, 12-byte strings, 4-byte iterators, original file/cursor
layouts, direct pointer offsets and independent module memories. Full-source auditing
only proves intermediate object compilation; it does not prove full linking or gameplay.

Remaining work includes complete resource dependencies and SDK/platform bindings,
graphics memory/register routing, callbacks and task scheduling, full engine linking,
single-screen scene integration and runtime comparison. The current probe has only 2 MiB
of memory and does not exercise these systems. Physical iOS execution is unverified;
a command-line simulator attempt stalled inside dyld before reaching the test entry point.

Tool provenance: [WABT wasm2c](https://github.com/WebAssembly/wabt/tree/1.0.42/wasm2c),
[WASI SDK 34](https://github.com/WebAssembly/wasi-sdk/releases/tag/wasi-sdk-34).
WABT's C support is supplied by its downloaded release under its included license;
it must retain the applicable notices when incorporated into the shipping App.
