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
The audit also writes `audit/link-inventory.json`, combining game-object definitions
and external references. Linker-generated symbols, SDK/data objects and remaining
interface conflicts must be resolved before treating it as an engine link result.

`prepare_lobby_parts.py` preserves the lobby source's existing pragma scopes as
separate compiler inputs and emits shared file data once. The namespace views keep
their own reconstructed types. Standard-library declarations come from the real
headers; one-byte carrier values remain present in the ABI. A strict relocatable
link checks signatures across all lobby parts, and the native probe runs original
short/long string searches and a C-to-C++ deque-copy call.

The intermediate uses SDK reactor startup, and the native check initializes each
instance once. Original billboard frame stepping is executed through both wrapper
layers, checking completion, end-frame clamping, looping and reset. Probe-only SDK
stand-ins test failure/success and host-count return propagation in the original
Wi-Fi wrappers; assertion failures trap. These stand-ins are not App bindings and
do not validate networking or the full battle-animation system.

The all-game merge follows `main.lsf`, rather than globbing obsolete source copies.
Its `audit/game-link.json` records selected sources, pending non-game objects and
signature conflicts. A nonzero merge result remains a failure even when every
individual source compiles. These checks still do not provide the SDK/data imports
or final App integration.

`audit/signature-inventory.json` independently compares every nonlocal function
definition and reference in the selected game objects, including unresolved SDK
names. It records every ABI variant and its referring objects, and retains raw
names for C++ symbols displayed by the tool. All linker-reported signature conflicts
must appear in this inventory. Any incompatible variant fails the audit; the current
linker's diagnostic count is not a count of all remaining interface problems.

Remaining work includes complete resource dependencies and SDK/platform bindings,
graphics memory/register routing, callbacks and task scheduling, full engine linking,
single-screen scene integration and runtime comparison. The current probe has only 2 MiB
of memory and does not exercise these systems. Physical iOS execution is unverified;
a command-line simulator attempt stalled inside dyld before reaching the test entry point.

Tool provenance: [WABT wasm2c](https://github.com/WebAssembly/wabt/tree/1.0.42/wasm2c),
[WASI SDK 34](https://github.com/WebAssembly/wasi-sdk/releases/tag/wasi-sdk-34).
WABT's C support is supplied by its downloaded release under its included license;
it must retain the applicable notices when incorporated into the shipping App.
