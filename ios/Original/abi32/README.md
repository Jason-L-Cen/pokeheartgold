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
The audit also writes `audit/link-inventory.json`, combining selected game/library
definitions, verified data and external references. Linker-generated symbols, missing SDK/data objects and remaining
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

The probe also checks field callbacks through a runtime-selected function pointer,
the movement gate's original byte result, texture/palette pairs returned as the
original two-byte structure, and the unchanged trainer-task/map-system record offsets.
The direct native core separately reads the ROM's grass/dust sequence members through
the original NARC API and runs the same original C decoder at frame boundaries.
The basic effect-template layout remains 20 bytes. The direct native core also runs
the original effect pool using real original heap and task-queue implementations,
checking init failure, capacity, update/draw callbacks, self-destruction and reuse.
This does not validate the full set of effect renderers or their GPU bindings.

The merge follows `main.lsf`, rather than globbing obsolete source copies. It also
audits the 514 available library C/C++ sources. Repeated library object paths in the
manifest select ITCM/DTCM sections of the same object and are compiled once.
The existing shared NitroSDK headers supply the legacy SDK include fallback.
Standard C functions use `<string.h>` declarations instead of private prototypes.
Its `audit/game-link.json` records selected sources, pending non-game objects and
signature conflicts. A nonzero merge result remains a failure even when every
individual source compiles. These checks still do not provide the SDK/data imports
or final App integration.

`audit/signature-inventory.json` independently compares every nonlocal function
definition and reference in the selected game/library objects, including unresolved SDK
names. It records every ABI variant and its referring objects, and retains raw
names for C++ symbols displayed by the tool. All linker-reported signature conflicts
must appear in this inventory. Any incompatible variant fails the audit; the current
linker's diagnostic count is not a count of all remaining interface problems.
Counts distinguish definitions owned by the game from those owned by libraries.

`build_original_data.py` converts original game/library assembly data files without
executing ARM instructions. LLVM first assembles a data-only ARM ELF reference;
the builder retains its section bytes, symbols, aliases and alignment, translates
symbolic data/function relocations, and checks the resulting Wasm object against
that reference. Overlay IDs come from the existing generated ID registry. Missing
types, unsupported relocations and function-address addends fail conversion.
The object metadata follows the [WebAssembly linking convention](https://github.com/WebAssembly/tool-conventions/blob/main/Linking.md).

The all-object link includes 1,120 selected game units, successful library units and
verified data in `main.lsf` order. The original 187 game data files remain verified;
library data conversion rejects unsupported ARM functions and CodeWarrior exception
metadata instead of treating them as native exception handling.
`audit/original-data/report.json` records source/tool
hashes, exports and RAM-shaped literal words requiring semantic review. Those
literal values are preserved; matching bytes does not prove their runtime meaning.
`native-objects.rsp` includes the data while `game-objects.rsp` remains source-only.
The native probe checks data pointers, aliases, BSS, an indirect call to original
RNG code and all 30 original spawn/fly/special-spawn records through the original C
lookup functions. These checks do not prove full engine linking or game execution.
The probe also executes the original battle-factory flag getter/setter with golden
byte values, checking bit queries, set/clear operations and unrelated-field behavior.

Native SDK branches replace the ARM-only 2×2 identity and 3×3/4×3/4×4 identity,
axis rotation, scale and matrix-copy primitives. The native sanitizer check compares
complete matrix outputs with golden vectors, including wrapping negation and an
in-place 4×4-to-4×3 copy. The original character-index code runs against table bytes
extracted from the SDK assembly; checks cover a golden 5×3 layout and every tile
in all 1–16 tile width/height combinations, including the zero-input CLZ case.
These are numerical checks, not validation of the complete 3D renderer.

Particle consumers now use the library's public declarations. Emitter creation
passes a position vector to `SPL_Create`; initialization callbacks use the separate
`SPL_CreateWithInitialize` API. The probe executes the original game wrapper,
manager, emitter initializer and list operations, checking position offsets, pool
exhaustion, recycling, callback invocation and self-destroying emitter ownership.
No particle initializer or list operation is replaced by a test stand-in.

The alarm prefix/suffix sources follow the ownership in `tools/alarm-partition`:
the prefix owns queue storage and ordinary operations, the suffix owns timer
arrangement, and the native IRQ wrapper calls it directly. The audit checks their
exact exported-symbol ownership. Hardware timer scheduling is still unverified.

`lib/NitroSDK/src/mi/mi_memory.c` owns the native memory primitives for both the
direct native core and generated-C path; the original ARM branch is retained.
Fixed-size copies preserve the original LDM/STM groups, and fast copies read each
32-byte group before writing, then copy remaining words in order. This matters
for overlapping buffers. The shared native check covers exact sizes, untouched
guard bytes, word/halfword fills, golden overlap results, fixed-destination sends,
and a copy crossing logically adjacent, physically separate graphics banks.
The native graphics backing is also included in the object audit as platform code.
For the ABI32 path, native offsets are validated against the module's memory size
and kept below the original DS main-RAM range. This does not map DS main RAM or
provide an active geometry FIFO backend; unsupported accesses still fail.

The direct native core also runs the original `gf_rtc.c` and SDK calendar
converters. `NativeRTC.c` reads the device's local wall clock into the original
2000–2099 date format. The six synchronous/asynchronous read APIs preserve busy
results, invalid-parameter errors and single-request ownership. Async results
are delivered on the IRQ owner thread through the native event queue, obeying
both CPU and peripheral IRQ masks. Queued replies and ordinary hardware IRQs
retain separate pending state and do not replace the installed IRQ handler.
Reinitializing IRQ state with queued replies fails rather than silently losing them.
Checks cover startup send-error recovery, queued ordering, worker-thread requests,
callback reentry, the original 11-frame refresh gate, all 24 day/night/wild-encounter
periods, leap day and the last representable date. This clock backend is currently
connected to the direct native core; its ABI32 host binding and App integration
remain work to do.
The native checks also pass in separate UTC, America/New_York and Asia/Kathmandu
processes. Pokewalker and Wi-Fi callers use the canonical RTC declarations instead
of private integer/void returns or an untyped three-word time buffer.

The direct core's backup backend uses a real 512 KiB save image and the original
CARD read/write/verify interfaces, with pointer-width-safe buffers. A sidecar lock
prevents competing opens; wrong-size images and symlinks are rejected. Each write
builds and syncs a temporary image, checks the destination's file identity/version,
then publishes it atomically and syncs its parent directory. Replaced files are
preserved on conflict. Native callbacks run on the serial backup worker in system
mode, matching the original CARD task model; waits synchronize callback completion
for callers outside callbacks. Cancellation is checked before publishing, and the
original save callback flag is atomic in native builds. The original 48 lock IDs
are allocated/released with native synchronization. Checks exercise actual file
persistence/reopen, pointer widths, end-of-chip bounds, callback reentry, queued
cancellation and existing-file protection. ABI32 backup binding and App integration
are still pending; these backend checks do not prove power-loss behavior.
The same isolated backend check also passes under ThreadSanitizer.

The direct native library now includes the original save registry and its record
modules. The native check runs `SaveData_New`, `SaveGameNormal` and the original
continue/load path. Trainer identity, money, badges, items, party Pokémon and PC
Pokémon survive writes and reopening. Corrupting the newest normal-data footer
recovers the previous slot; corrupting both normal-data slots rejects the save.
This complete native check also passes under ThreadSanitizer.
All 42 main and six extra record sizes match golden values derived from the
original ABI32 size callbacks, and the `SaveData` layout remains 0x2330C bytes.
Two table-bound checks now test the index before reading the next record.
Native shared save mirrors and tower message templates preserve original layouts.

New profiles persist an unregistered 43-bit pseudo-console ID in a `.console`
sidecar. Original DWC account initialization/CRC code uses it with OS-provided
entropy; it does not report successful Nintendo authentication. Malformed identity
files are rejected and existing save data remains intact. Fatal native read/write
error paths currently stop the core; App error presentation remains unfinished.
`check.sh` uses the same CMake source list as the device library and requires CMake
on PATH or the existing workspace build-tools cache.

Original `GfGfxLoader` archive/member loading now runs in the native core. All four
load APIs share the decompression/cleanup path and report zero output size on
failure. Native decompression receives the actual member size and destination
capacity; invalid headers, distances, truncated streams and oversized outputs are
rejected, and temporary heap blocks are released. Tests compare real graphics
member bytes through archive-ID and open-archive routes, both heap directions and
the size/no-size APIs, plus malformed NARC members. The bounded decoder is also
compiled into the ABI32 platform objects and exercised through generated native C.
Packed graphics-descriptor relocation, rendering all effect types and App scene
integration remain separate work; loading bytes does not prove those systems.

Pokeathlon state callbacks now receive the actual one-byte substate, and Goal Roll
and Ring Drop tables use their real boolean two-argument function type. Result-size
queries use a void parameter list, CPU updates preserve their integer return and
time-up paths pass an explicit pause flag. The generated native check executes the
original course state dispatcher with runtime-selected callbacks, verifies boolean
returns and byte writes, and checks adjacent state fields. It does not validate
complete minigame execution, graphics or link play.

SDK-facing login/connection, WCM, WM/WVR, power and microphone declarations now
preserve their actual results and parameters. Wi-Fi Club passes the user-data
pointer returned by the roster lookup into account/console checks. Transport
registration preserves integer results and the timeout callback accepts its aid.
The native generated-C check runs the original transport setters with no work and
with real channel state, invokes the registered callbacks, and checks receive-buffer
rejection/reset behavior. These are ABI/data-flow checks, not networking or microphone
backend validation; they do not report successful access to Nintendo services.

Socket callers pass address lengths explicitly and HTTP polling uses the SDK's
signed 64-bit timeout. Platform socket errors keep the socket parameter while
reading the existing error state; close/shutdown/send and buffer helpers retain
their actual return types. The generated native check executes original SOC
adapters with probe-only SOCL/status/sleep fixtures, covering byte order, accept
failure/success, socket-name fields, zero/finite/infinite waits and a timeout above
32 bits. These fixtures are not App networking backends.

Container append/remove, base64, HTTP formatting/shutdown, account/AP helpers,
friend callbacks and cartridge-access declarations now match their actual return
types. Ignored results remain ignored at those callers; side effects and error
results are not replaced with fabricated service success. The independent inventory
continues to fail the full merge while incompatible interfaces remain.

Match setup/browse paths now pass each send/reservation operation's real result
into `DWCi_HandleMatchCommandError`, instead of relying on an undeclared argument.
The header fixes the unsigned result contract. The native generated-C check runs
the original router and match-work accessors with observer error handlers, checking
both matchmaking modes and high-bit error values. It validates routing/data flow,
not communication with external matching services.

Connection buffer-space queries now carry their aid into the lookup, and subtract
protocol overhead only after checking the available count. A generated native
check executes that original helper with observer lower layers, verifying aid
propagation, boundary counts and negative results without signed overflow.
Transport, ping/listen, local-IP, multiboot and exit declarations use their real
argument/result contracts; the unused variadic tail on callback insertion is gone.
Compiled SDK definitions currently have no incompatible signatures in the independent
inventory. Uncompiled SDK files and unresolved imports are still failures and are
not covered by that statement.

Game communication callers now pass the current link mode into Wi-Fi predicates
and the current save into alternate-search initialization. Command numbers and
Wonder Card lengths remain integers through the send path, and send/registration
declarations keep their real result types. The generated native check executes
the original mode predicates for every known mode and invalid boundary values;
it does not establish a wireless connection.

Field command tables now share the dispatcher's typed receive/size/buffer entries.
Receivers accept all four arguments; buffer callbacks accept all three and return
their real pointers. The shared command lookup rejects negative and one-past-end
indices and unavailable tables before any table access. A generated native check
executes the original register/free/dispatch functions with observer callbacks and
probe-only table/heap fixtures, checking argument propagation, both table boundaries,
empty entries, invalid indices and dispatch after free. It does not execute complete
field or Colosseum scenes.

Local-wireless and Wi-Fi receive registrations now use typed three-argument
callbacks, including peer-frame receivers. Wi-Fi users share the existing public
declarations instead of overlay/private copies with conflicting argument/results.
The club disconnect handler accepts the dispatcher's two arguments; the generated
native check invokes that original handler through its callback type for all four
club slots and verifies that unrelated state bytes remain unchanged. This does not
test a live wireless connection.

Lobby character-set search now returns the original search result through its C++
wrapper instead of discarding it. The generated native check covers both string
storage formats, start/end/not-found results, empty character sets and embedded
zero bytes using the original search implementation. Complete lobby protocol and
service behavior remain unverified.

Both original NitroCrypto RC4 state formats now have native C encryption paths;
the DS assembly paths and original key setup remain available. The word-state
beacon caller uses the existing context type to guarantee its required alignment.
The same check runs directly under host sanitizers and through generated native C:
it compares with the independent existing CPS implementation for key lengths 1–16
and the original 16-byte truncation rule, checks split/in-place/zero-length behavior
and state bytes, and matches numerical vectors from
[RFC 6229](https://www.rfc-editor.org/rfc/rfc6229#section-2). This validates these
SDK primitives, not an end-to-end Mystery Gift or beacon exchange.

SDK MD5/SHA-1 contexts use fixed-width words so their original layouts also hold
in the direct 64-bit core. SHA-1 block processing reuses the existing CPS C
compression function, while the original DS assembly remains available. Typed
native digest-table views are checked against the original ARM data bytes.
The shared direct/generated-native check covers MD5, SHA-1 and HMAC-SHA1 using
[RFC 1321](https://www.rfc-editor.org/rfc/rfc1321.html),
[RFC 3174](https://www.rfc-editor.org/rfc/rfc3174.html),
[RFC 2202](https://www.rfc-editor.org/rfc/rfc2202.html) and independent hashlib results, including split/unaligned
input, padding boundaries, length carries and the SDK's empty-HMAC rejection.
These primitive checks do not validate a complete authentication session.

SDK quicksort now has a native path preserving the original pivot, partition and
equal-key ordering, with 32-bit work indices retaining the caller's original
workspace capacity in both pointer models. The SDK's high-bit count rejection is
also retained. Shared declarations remove the Pokewalker caller's incompatible
comparator cast. Direct/generated-native checks compare unique-record results
with libc qsort across widths, alignments and both workspace paths, check equal-key
goldens and canaries, and execute the original event-log comparator with unsigned
timestamps and associated slot indices. Complete Pokewalker/Pokeathlon scenes are
not exercised by these checks.

Lobby string copies and concatenations keep their actual pointer returns. The C++
chat constructor passes explicit destination objects instead of relying on the DS
compiler's class-return convention; the standalone range dispatcher retains its
one-byte carrier argument. A generated-native check executes the original string
primitives and chat constructor for empty/short/long strings, deep copies, twelve
prefix selections, returned pointers and balanced allocation/release. It uses
libc-backed DWC memory hooks and synthetic prefix inputs, not a network backend or
a verification of the live service's channel data. Host-check stdio/clock imports
use owned POSIX descriptors and real clocks, with memory bounds, I/O/error and
closed-descriptor checks. App bindings for those imports remain separate work.

Case-insensitive comparison now returns the original substring comparison result
after lowercasing both inputs. Its generated-native check uses the original MSL
character-map bytes and covers prefix/full/zero-length comparisons, long strings,
mutation and unchanged non-ASCII bytes. Vector cleanup declarations retain their
real object-pointer results; byte/word/string vector checks verify returned objects
and release of nested allocations. OOM setup now receives the active chat manager.
A controlled allocation failure and the original two-stage reserve-release/error/
disconnect callbacks are checked with a disconnect observer, without connecting
to a service or exercising live transport teardown.

Global and channel chat registration use concrete function-pointer fields and a
shared C/C++ declaration header. Handlers retain the SDK's complete arguments,
including unused modes, leave reasons and user data. The audit independently checks twenty-three compiled handler signatures
against the SDK contracts, including handlers whose old declarations and definitions
were both wrong. A generated-native check executes the original callback dispatcher
and cleanup for twenty-one global/channel/query event types with argument observers and
owned payloads. It verifies argument order, high-bit words, user data, join notification
and release; it does not run the application handlers or establish a connection.
Query checks include empty/populated channel/user arrays and optional channel-key
value lists. Query registrations use the same shared callback declarations; the
per-channel enumeration handler retains all seven SDK arguments.
The channel-name helper now returns its destination pointer, and C++ callers pass
that destination explicitly. Its generated-native check verifies names and ownership
for all constructor-prefix fixtures. Timer/channel map cleanup declarations retain
their real pointer returns.

Current audit checkpoint: all 1,121 game sources compile (1,120 are selected by
`main.lsf`); 487 of 514 library sources and the native graphics/compression backends compile;
229 of 257 data sources verify.
The independent inventory still finds 82 defined-function conflicts (82 game,
0 library), no external names with incompatible signatures and 209 unresolved
symbols. The strict merge therefore fails. These counts measure build gaps, not
features or gameplay completion.

Remaining work includes complete resource dependencies and SDK/platform bindings,
graphics memory/register routing, callbacks and task scheduling, full engine linking,
single-screen scene integration and runtime comparison. The current probe has only 2 MiB
of memory and does not exercise these systems. Physical iOS execution is unverified;
a command-line simulator attempt stalled inside dyld before reaching the test entry point.

Tool provenance: [WABT wasm2c](https://github.com/WebAssembly/wabt/tree/1.0.42/wasm2c),
[WASI SDK 34](https://github.com/WebAssembly/wasi-sdk/releases/tag/wasi-sdk-34).
WABT's C support is supplied by its downloaded release under its included license;
it must retain the applicable notices when incorporated into the shipping App.
