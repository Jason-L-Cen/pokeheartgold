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
python3 ios/tests/check_engine_link.py
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

After the relocatable merge, the audit attempts a strict final intermediate link
rooted at `NitroMain`, using the same reactor runtime and explicit native-import
list as the component build. `audit/engine-link.json` records the exact command,
object-list hash, exit code and linker diagnostic names. Failed links remove stale
module output. `check_engine_link.py` exercises the actual linker with an exported
entry point, an allowed native import, a rejected missing function and a symbol-type
collision. A successful
intermediate link still does not prove native C/App linking or gameplay.

The component and full-source builds reuse `NativeMemory.c` for native assertions.
A subprocess check requires the real condition/file/line diagnostic and termination;
the probe no longer substitutes a silent trap for `HGNativeAssertFail`. The original
legacy `GF_AssertFail` reset/error-screen implementation retains its separate owner
in the full game. ABI32 diagnostics use byte writes and the existing formatter to
avoid the incompatible MSL/WASI `__stdio_exit` ownership; the fatal stop uses a
compiler trap rather than the MSL abort/semihosting path.

`NativeWASIImports.c` supplies the six current clock/standard-descriptor bindings
in the iPhoneOS archive itself. The check executable links the same implementation.
`NativeWASIImports.h` describes each instance's stable memory binding and owned
descriptor copies; only slots 0–2 are exposed. Existing checks cover vector/range
validation before I/O, clock IDs, seek/status/close results, unchanged file position
after rejected writes and unbound contexts. This does not implement arbitrary
WASI filesystem access or the remaining full-engine imports.

MSL debugger-console entry points now use native byte I/O. Console reads preserve
CR/LF stopping and the original success/error/EOF results; writes report partial
lengths and retry interrupted calls. Read/write vectors share pre-I/O bounds checks.
Checks cover newline handling, NUL/high-byte round trips, EOF, invalid descriptors
and rejected reads that leave the file position unchanged. `sys_exit` emits a
diagnostic and traps rather than issuing the ARM debugger SWI; a subprocess check
verifies that fatal stop. The empty original `OSi_ReferSymbol` anchor has a native
definition with the same lack of runtime effects. These console paths do not
implement the game's soft-reset lifecycle.

The native exception-message shared pointer bridges the frontend's `char[]`
constructor to the existing MSL `char[0]` constructor. Its control-block vtable now
declares each callback's real return/argument types, including the deleting
destructor's returned pointer. Native lifetime checks use the original constructor,
destructor, strong release and weak release code with named allocator/vtable
fixtures. They cover an alias, final strong/weak releases, an external weak owner
and a null payload with no remaining allocations. These fixture checks do not
validate RTTI, allocation-failure unwinding or the original exception-table format.

`NativeException.c` owns the original 44-byte SDK exception work record and a
per-instance main exception-vector slot. The ABI32 `OS_InitException` keeps its
original debugger-range decisions and DTCM-vector writes, but its main vector is
relocated from fixed DS RAM into bounded instance memory. Checks pin member
offsets, both debugger-range boundaries, preserved user arguments/hook-stack
bytes, repeated initialization and independent instance state. Entering the legacy
exception vector now uses the existing native assertion diagnostic and fatal trap;
a subprocess check verifies termination. Native faults remain the responsibility
of the host runtime/debugger. This does not capture ARM exception registers, run a
DS debugger, install host signal handlers or implement the game's error screen.

`build_original_data.py` converts original game/library assembly data files without
executing ARM instructions. LLVM first assembles a data-only ARM ELF reference;
the builder retains its section bytes, symbols, aliases and alignment, translates
symbolic data/function relocations, and checks the resulting Wasm object against
that reference. Overlay IDs come from the existing generated ID registry. Missing
types, unsupported relocations and function-address addends fail conversion.
The object metadata follows the [WebAssembly linking convention](https://github.com/WebAssembly/tool-conventions/blob/main/Linking.md).

The all-object link includes 1,120 selected game units, successful library units and
verified data in `main.lsf` order. The original 187 game data files remain verified;
library data conversion rejects unsupported ARM functions. Mixed CodeWarrior files
can supply byte/relocation-verified ordinary sections as `constantsObject` while
their unit `exitCode` remains 1. Excluded preprocessed exception statements are
retained in `excludedArmExceptionMetadata`; no native unwind support is inferred.
The complete-file failure count still controls the overall audit result. Constants
after an exception section are retained too, and invalid input removes stale outputs.
The ARM assembler's `%function`/`%object` spelling is used for input `.type` directives;
defined ARM functions are still rejected, including after this syntax conversion.
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

Request-map lookups now receive an explicit iterator destination in all six C++
response handlers. Key deletion declarations keep the original Boolean result.
A generated-native check runs the original insert/find/delete, iterator and rebalance
code for empty/missing keys, unsigned high-bit IDs, duplicate insertion, root/endpoint
removal, destructor routing and balanced node/request release. It uses probe vtable/
Boolean table inputs and a request-destruction observer, not an active chat session.

Timer/channel/user map erase declarations keep their Boolean results. Timer
callers pass two arguments, and the erase helper passes the shared iterator by
value instead of passing its node as an untyped argument. Generated-native checks
run empty/missing/success/repeated deletion for leaf map views, verify root/first
updates and release of user/channel strings and a nested channel-user node.
These checks use owned test records rather than a live session.

Byte/word-buffer/VIP allocation APIs retain their one-byte allocator arguments.
The standalone byte-range constructors/assigners use matching ABI carriers, and
buffer/VIP cleanup declarations retain their actual pointer results. Generated-native
checks execute the original range initialization/assignment, growth, word-buffer
allocation/destruction and VIP reallocation/destruction for valid capacities,
including empty/binary/overlapping byte ranges and balanced release. Oversize
requests remain fatal in this isolated probe; the original C++ length-exception
implementation and unwinding dependencies are not validated by these checks.

String-vector/deque-map capacity helpers use their actual two arguments, and the
deque-map advance helper keeps its void result while C++ `operator+=` returns
the iterator itself. Slow string/word pushes retain their one-byte allocator tags.
Generated-native checks preserve the original growth-formula breakpoints, lazy
map-limit initialization and cursor wrapping. Repeated word/string pushes verify
previous values, long-string deep copies and balanced release. Map-limit words
start with the original data files' zero values in this isolated probe.

The schedule loader forwards the received byte count to its original size validator.
Its vector/deque calls retain their actual allocator/comparator carriers and erase
arguments. Sort insertion/merge comparators preserve their ABI words, and rotate
uses an explicit iterator destination. Generated-native checks run the original
size validator at header/count boundaries and the complete original stable-sort
helper chain for 0–161 entries, equal keys, signed extremes, guard records and
full/partial/failed temporary allocation. Probe-only allocation bindings inject
these failures; original allocation retry, sorting and rotation code still runs.

User-set lookup now receives an explicit iterator destination in the C++ channel
scan. A generated-native check runs that scan against owned maps: a referenced
user is retained, an unreferenced user's long nickname/node is released, and
channel/member cleanup stays balanced. Event-vector/player-set cleanup declarations
also preserve their pointer results, checked against the original destructors.
These are isolated record/algorithm checks, not live lobby or complete schedule
loading tests.

Lobby command/helper declarations retain their actual result codes and destructor
pointer results. Nickname lookup uses three explicit arguments, including the
C++ channel-key request builder. Generated-native checks run the original lookup
for absent/present/deleted users and verify that a copied long nickname survives
user removal. They also check missing/existing channel enumeration flags and the
original questionnaire set/get result, bytes and untouched record boundaries.
These paths use owned records and do not validate an active network session.

All six PPW timer handlers and both registration views use the dispatcher's shared
`int (void *)` contract. The native check includes the original private `Timer_Update`
implementation and calls a runtime-selected observer through its stored function
pointer, checking argument delivery, keep/remove results, strict interval boundaries,
zero intervals, backward time and timestamps beyond 32 bits. It does not exercise
the outer timer map's deletion loop or the handlers' networking side effects.
The whole-source inventory separately requires the six compiled handler signatures;
that contract gate catches indirect-call defects even when no direct symbol conflicts.

Fifteen PPW channel/global/query handlers now share explicit declarations. The
channel/global callback records are shared with the original chat consumers;
main/sub-channel and query registrations retain their complete signatures. Handlers
keep sender-supplied context arguments even when unused. Channel-mode data remains
a pointer, and the source's string views/declarations preserve read-only channel names.
The audit requires all fifteen compiled callback shapes in addition to the SDK and
timer contracts; matching direct symbols alone does not validate indirect calls.

Generated-native checks run the original topic, broadcast-key, channel-message,
private-message/flood and user-parted handlers against owned channel/user records.
They verify prefix stripping, ID/nick validation, message-type/error-state gates,
payload/context delivery, last-member removal, end markers and balanced release.
The original PPW topic handler is also called indirectly through the chat handler.
Other application callbacks use observers in these checks; the PPW state machines
and live network sessions are not exercised. User-set fixtures retain their actual
12-byte header, distinct from the 16-byte map views.

Wi-Fi setup version requests return the packet length consumed by the send worker.
The native setup file loader retains its compressed byte length and uses the shared
bounded LZ10/LZ11 decoder. It checks the signed read-size limit, short/error reads,
close results, headers and both allocations. Failure closes an opened file, releases
both buffers and then follows the original fatal-error policy; it does not pass a
partial resource to callers. Uncompressed bytes, allocation sides/alignment and
optional output lengths retain their original behavior. A file-specific allocation
helper restores IRQ masks before returning failure, allowing that cleanup before
the general UI allocator would terminate.
The generated check exercises 21 raw/compressed/empty/error cases with file/heap
observers, allocation guards and resource counters, including both allocation
failures and a missing output-size pointer. Its fatal observer returns solely to
inspect cleanup; this is not a real filesystem, heap-capacity or Wi-Fi service test.
The `.l` suffix fixture matches the four original bytes in
`lib/NitroDWC/asm/wifi_setup_share_data.s`. ASan/UBSan and TSan pass.
The read-only archive write callback accepts the SDK's four arguments and keeps its
failure result. Child-state notifications pass their received AID to the original
state getter. Setup command declarations retain their actual results/arguments.

The inventory finds 74 named task targets in Wi-Fi setup queue creation/function
replacement sites and requires each compiled target to accept task/argument pointers.
Their declarations and unused parameters agree with the original two-argument
dispatcher. The native check uses owned queue/pool records with the original insertion,
dispatch and cleanup code: priorities/equal-priority order, queue disabling, callback
replacement, deferred removal, argument delivery and owned-argument release are checked.
Both heap-free helpers now check for an empty pointer before masking VBlank IRQs;
the regression check verifies that empty cleanup preserves the IRQ mask.
Heap-free/IRQ bindings here are probe observers; scheduler initialization, DS BSS
address routing, actual interrupt delivery and screen rendering are not validated.

The same check runs the original child-state getter for AIDs 1/3/15, all seven
states and balanced interrupt-state restoration. Original packet construction matches
independently assembled 48/72-byte normal/extended requests, including checksums,
padding and output guards. Original AES-128 key wrap/unwrap matches an independent
`cryptography` golden and rejects a corrupted wrapped key. The version bytes and
eleven AES tables are typed views of original assembly data, byte-checked after
compilation. These checks do not connect to an access point or validate a live session.

Plaza initialization wrappers now return the original result classifier's Boolean
instead of discarding it. Own-ID, one-shot flag, group-join and talk-counter wrappers
likewise preserve their underlying results. Singleton getters use their no-argument
interfaces; command registration and its footprint-app wrapper retain void results.
The PPW result declarations used by plaza callers agree with their implementations.
Join/action messages initialize their unused payload word to zero; the original
receivers use sender IDs and do not read those payloads.

The generated-native plaza check uses owned records with the real PPW ID getter,
client/work getters, command-table setters and group/talk helpers. It checks missing/
high-bit IDs, one-shot clearing without adjacent-field changes, ordinary result-code
classification, table registration/clearing, player counts and signed timeout values.
It fills all 27 group seats, rejects duplicate/full joins without mutation, reuses a
freed seat and checks ended-group/marker rules. Initialization, live sends and
matchmaking are not exercised; no lobby-service success stand-in is used here.

The final three direct-call repairs are grounded in retained original assembly:

- The [Hall dispatcher at `ov80_02231844`](https://github.com/Jason-L-Cen/pokeheartgold/blob/170d7558434f9b390ea8a03e4cde40e313a4b045/asm/overlay_80.s#L16070)
  returns the selected send helper's result, truncates selector 3's value to a byte,
  and leaves the data address in r0 for unhandled selectors. The C dispatcher keeps
  those results, including the otherwise unusual no-op return.
- The [text palette call](https://github.com/Jason-L-Cen/pokeheartgold/blob/7af322fc349504d2afb76ea6dede57e37bef3338/asm/overlay_93_thumb_2.s#L4594)
  reuses the eighth parameter in r1: the 0x88-byte frame puts it at sp+0x94.
  Wobbuffet text creation passes that palette value through the shared font declaration.
- The [Disc Catch actor initializer](https://github.com/Jason-L-Cen/pokeheartgold/blob/763c166a7501e5e336e21064266f891c401bde4b/asm/overlay_96.s#L85162)
  leaves r1 at 2 after loading the Y halfword, then calls the animation-state setter.
  The C call supplies state 2 explicitly.

The native scene-interface check executes the original Hall dispatcher, font palette
loop, mon animation-state selection and mon-gfx sprite getter. Send/output observers
check selected helpers, 0/1 result propagation, byte truncation, no-op address results,
palette forwarding and animation sequence requests. These observers do not provide
live transport, glyph rendering or sprite animation playback. The complete scene
constructors and their GPU/resource dependencies remain unvalidated.

The original CPS big-integer unit now compiles in both native paths. Three ARM
helpers that assemble high 48/64-bit windows from 16-bit digits have equivalent C
definitions; the original arithmetic algorithms remain in use. Two digit products
use explicit unsigned multiplication to retain ARM's 32-bit results without signed
overflow. The modular-inverse workspace documentation reflects its six arrays plus
the three-array division workspace (nine arrays, initially zeroed).

Deterministic Python-integer goldens cover 26 arithmetic/division cases and eight
modular-power/inverse cases through 1024-bit operands. Checks include carry/borrow,
supported aliasing, zero values, output/workspace guards, ordinary and Montgomery
power, allocator failure and balanced release. Plain power uses both buffer widths
found in SSL callers. The checks pass in the generated ABI32 native-C probe and the
direct 64-bit ASan/UBSan core; the iPhoneOS arm64 archive contains the real CPS
definitions. These are arithmetic checks, not a TLS session or certificate audit.
After changing the direct core's source list, configure its Xcode project explicitly
before building; automatic project regeneration can leave the current build using
the previous source graph. Archive symbols must be checked after that rebuild.

The WCM control unit has native population-count and leading-zero helpers, including
the ARM result of 32 for zero input. Configuration retains complete host addresses
and the native list header's alignment; ABI32 keeps the original four-byte alignment.
Null/undersized list buffers are disabled without a write. `WCM_UpdateOption` checks
for an absent work record before reading its previous options and restores IRQ state.

The native WCM check includes the original control/API sources with platform-request
observers. It checks bit helpers over all 16-bit inputs and shifted/mixed 32-bit words,
all 13-bit channel masks/start channels, buffer offsets/sizes and guards, option fields,
scan parameters, original all-FF wildcard bytes, notice payloads/reentry and phase/
keep-alive requests. Original fatal-phase latching/alarm-request behavior is retained.
The checks pass in ABI32 and the direct 64-bit sanitizer core, and the real WCM
definitions are present in the iPhoneOS arm64 archive. IRQ, alarm, scan-period and
null-packet observers here do not implement radio hardware, real timer delivery or
a network connection; WCM startup/connect/disconnect remain unvalidated.

`MI_SwapWord` uses a real native atomic exchange. A local tool-chain experiment
confirmed that wasm2c 1.0.42 lowers atomic exchange on unshared memory to memcpy
operations, while its shared-memory conversion path aborts as unimplemented.
The ABI32 implementation therefore imports the explicitly named
`HGNativeAtomicSwapWord`; `NativeImports.c` checks memory bounds/word alignment and
calls the native SDK primitive. The probe links only explicitly whitelisted native imports.
Each module owns an environment pointing to its fixed memory buffer; the owner
must keep that buffer alive and fixed during concurrent calls.

Direct/native-generated checks verify old-value results and adjacent guards, then
exchange 20,000 unique values through two host threads and require the complete
initial/returned/final-value permutation. ABI32 also checks independent module
memories and traps for bad bounds, alignment and missing environments. ASan/UBSan
and a separate ThreadSanitizer build pass. Both iPhoneOS archives contain the native
primitive, and the probe archive also contains its import binding. These checks
cover the exchange leaf only; SDK lock release, thread/context switching and
concurrent execution of the whole game remain unvalidated.

SDK lock-ID allocation/release now imports the existing mutex-protected native
48-ID provider. Its implementation lives in `NativeLockIDs.c` so the direct core
and generated-code bindings share one device-wide pool. Original compatibility
unlock names forward to their canonical SDK functions in native builds.
Checks cover ordered allocation, exhaustion (-3), release/reuse and two module
instances sharing the pool. Two host threads perform 20,000 allocate/release cycles
with duplicate-claim checks; ASan/UBSan and ThreadSanitizer pass. The existing direct
backup/core checks also pass after the provider move. This does not validate the
SDK's hardware card/cartridge lock words or its complete thread scheduler.

The nine SDK data/instruction-cache entries now order coherent native CPU buffers
with a sequentially consistent fence. Generated code calls the whitelisted
`HGNativeMemoryBarrier` import, whose binding uses the same native SDK provider as
the direct core. Address ranges are not dereferenced or copied; all translated
code is compiled into the native binary before execution. This does not implement
GPU uploads/completion, separate device-memory coherence or runtime code patching.
Direct and generated checks preserve bytes across aligned, unaligned, zero-length
and null/zero calls. Each path also runs 18,432 two-thread store/load rounds over
all nine entries, rejecting the forbidden result where both reads see old data;
the competing payload accesses are atomic so the check is valid under TSan.
The direct core passes ASan/UBSan, and generated C also passes TSan. Both iPhoneOS
archives define all nine entries; their arm64 objects contain a DMB ISH instruction
in each entry. These are CPU synchronization checks, not GPU or full-game validation.

`NativeIntegerRuntime.c` supplies the fourteen typed C integer entry points packed
inside the original `msl_fp_runtime.c` assembly owner: signed/unsigned 64-bit
multiply, divide, remainder and left-shift aliases, plus the three 32-bit division
entries. Multiplication wraps at 64 bits and shift counts are masked to six bits.
Division/remainder retain the original zero-divisor returns and MIN/-1 results
without invoking C undefined behavior. The unchecked unsigned 32-bit entry has a
different zero-divisor result (zero) from the guarded entry (the dividend), as in
the original 32-step ARM loop. Register flags and secondary register results are
not C-call outputs and are not exposed by this backend.
The direct/generated checks use Python-integer goldens for seventeen 64-bit pairs,
ten 32-bit pairs and eleven shift counts, including signed limits, cross-word
operands and over-wide counts. These integer checks do not cover floating-point
behavior; the referenced float entries are described below. The original packed
assembly owner and its remaining aliases still remain blocked in the audit.
The direct core passes ASan/UBSan and generated C passes both ASan/UBSan and TSan.

The MSL narrow/u16 printf formatters use compiler `va_arg` instead of manually
advancing ARM argument slots. Reads use canonical `long long` types: SDK `s64/u64`
aliases deliberately have four-byte storage alignment, which is not the native
variadic calling convention. Dynamic width/precision use the same compiler reader,
and variadic wrappers pair `va_start` with `va_end`. The original CodeWarrior header
provides its original slot-stepping `va_arg` for the non-native build.
Generated-C checks execute both original formatters, original decimal/multibyte
conversion routines and ARM-byte-verified constant/locale sections. They cover mixed
integer/64-bit/double/long-double/string arguments, packed SDK typedef values,
dynamic width/precision, `%n`/`%lln`, truncated buffers, returned counts and guards.
The old narrow formatter aborted inside `strlen` on the mixed-argument regression.
Probe stdio wrappers are renamed to avoid replacing the probe's WASI libc. It shares
the full build's constant partitioner; verified ordinary sections can participate
in the all-object link while the mixed files' ARM exception records remain blocked.
These checks do not validate arbitrary floating-point rounding or complete game text.
Generated C passes ASan/UBSan and TSan; both format-check exports are present in the
iPhoneOS arm64 probe archive. Direct native-64 MSL stdio is not exercised here.

The native GameSpy availability query has one typed 0x5C-byte state object shared
by its sender, response parser, poller and five availability-gated SDK clients.
The old views depended on adjacent globals and interior linker aliases across
translation units; native object placement does not preserve that arrangement.
Its socket address, request packet, send time and retry count now have explicit
offsets in one owner. The non-native state/alias layout remains in place.
Generated-native checks execute the original request/address/parser/polling code
with clock/socket observers. They verify original request bytes/constants, DNS and
socket failure, response address/port/magic/length rejection, high-bit result words,
result precedence, one resend, the strict 2,000-ms boundary and unsigned deadline
wrap. Original constants are checked against ARM data and the compiled C views.
ASan/UBSan and TSan pass, and the check export is in the iPhoneOS arm64 archive.
No real socket, live service or full networking session is exercised here.

Eight CPU-control/wait entries in `os_system.c` now import the existing native
interrupt-mask/execution-mode provider. Its state lives in `NativeCPU.c`, shared
with the direct core, instead of duplicating the arena implementation. The imports
reject missing environments and restore values containing non-mask bits.
The native ARM9 wait converts the original four-cycle subtract/branch loop to a
minimum delay at twice `HW_SYSTEM_CLOCK`. A relative pthread condition wait uses
the existing IRQ queue, dispatching enabled owner-thread events while waiting and
deferring them while IRQs are masked. Short-wait overhead and physical-device frame
timing still need calibration; this does not reproduce ARM instruction timing.
Direct/generated checks cover nested IRQ/FIQ masks and prior-state returns. Two
host threads perform 20,000 protected increments in generated memory with guards.
Further checks exercise native callback reentry into generated mode/mask getters,
pending dispatch on restore, enabled/masked waits, a 20-ms minimum delay and invalid
import arguments. The Wi-Fi setup check now uses these real CPU masks rather than
its former CPU-mask observers; its IRQ-mask/heap fixtures remain isolated.
These CPU leaf/context checks do not prove original thread/context switching or
execution of the complete game loop.
The direct core passes ASan/UBSan and generated C also passes TSan.

Translated IRQ registration now reaches the native event queue through explicit
imports. Each environment binds its fixed function table and the generated void()
and void(i32) signature IDs. Registration and invocation validate the table entry
before a native call; legacy casts from no-argument timer handlers are invoked with
their actual signature. Non-void handlers and invalid table/channel indices trap.
The native dispatcher snapshots the tagged closure before unlocking, so callbacks
can replace or rearm their registration. DMA callbacks preserve prior mask-enable
state; timer callbacks remain enabled after their one-shot function is cleared.
`HGNativeReleaseIrqCallbacks` removes an environment's registrations and pending
bits on the idle owner thread (or before an owner exists) before its table/memory may be freed. Bindings must
keep those buffers fixed and alive until detachment. A foreign-context handler
cannot be returned as an index in another module's table.
Checks cover VBlank/HBlank priority, masks/check/request flags, master/CPU disabling,
wait/wake, DMA completion, timer rearm, user data, no-argument handler casts, invalid
registrations and detachment. Seven game handlers use `OS_SetIrqCheckFlag` instead
of writing a DS DTCM offset relative to `OS_IRQTable`. The IRQ stack checker and
ARM context/thread-switch implementation remain unported; no full scene is proved.
Generated C passes ASan/UBSan and TSan. Both iPhoneOS arm64 archives contain the
native bridge; the probe also exports its import bindings, cleanup and IRQ check.

The seven public RTC read/init entries use the existing native local-clock backend
through `HGNativeRtcRequest`. The bridge checks the original 16-byte date/12-byte
time layout, output spans/alignment and void(i32,i32) completion signatures. Sync
and async paths preserve busy/error results and write outputs only on success.
Accepted async requests retain native reply storage until the FIFO owner-thread
completion writes guest buffers and invokes the original callback. An atomic
pending count prevents detachment while requests remain active, including requests
from another host thread. Embedders must stop new calls and drain requests before
detaching/freeing the module; async callers must keep their output buffers alive.
Checks cover pre-IRQ send failure, device date/time and weekday, null pointers,
untouched busy buffers, masked delivery, null/reentrant/worker callbacks, opaque
user data, rejected teardown, bounds/alignment and callback-type errors. Original
`gf_rtc.c` also runs through this bridge: all 24 morning/day/evening/night encounter
periods, frozen-time restoration and its eleven-frame refresh threshold are checked.
This is local RTC/GF clock logic, not validation of complete maps or encounter scenes.
Generated C passes ASan/UBSan and TSan; separate UTC, America/New_York and
Asia/Kathmandu processes pass the device-time comparisons. The iPhoneOS arm64
probe archive defines the native RTC provider, binding and original-GF check.

The translated SDK tick API now uses the existing native monotonic clock at
`HW_SYSTEM_CLOCK / 64`. Its initialization/availability state and timer-0 reservation
remain module-local; reads and sets update the original upper-counter view under
the same native CPU critical section used by the rest of the core. Native clock
initialization remains idempotent, and setting the tick retains 64-bit wrap behavior.
The SDK entropy entry uses the direct core's `OS_GetLowEntropyData` provider
(`arc4random_buf`) through a bounded/aligned 32-byte guest-memory view, replacing
reads from DS MAC/microphone/display/controller addresses. Original RNG/crypto
algorithms are not replaced.
Generated checks cover initialization, high tick values, elapsed-rate bounds using
measured monotonic intervals, low-word/counter views, two-thread monotonic reads,
set/reset/wrap, entropy output guards and invalid native import arguments. These
checks do not establish full-game animation pacing or hardware-timer scheduling.
ASan/UBSan and TSan pass; the iPhoneOS arm64 probe archive contains the native
clock/entropy providers, import bindings and generated check.

The original alarm queue prefix/handler/suffix now runs with native one-shot wake
notifications in place of timer-1 register writes. The queue still selects the
original reload count (including its 16-bit maximum interval), installs its real
generated IRQ handler and performs insertion, cancellation and periodic rearming.
A private serial dispatch timer posts the native timer-1 IRQ; guest callbacks run
only on the event owner thread. Stale/early dispatch events are rechecked against
the current deadline. Detachment synchronously cancels the owner's wake source and
clears pending timer requests before the module may be freed.
The native prefix also restores the saved CPU mask after `OS_SetPeriodicAlarm`;
the reconstructed source omitted that restoration. Its check rejects a mask left
disabled after periodic setup. Other checks cover head/middle/tail cancellation,
equal-deadline FIFO, periodic self-cancellation, masked overdue alarms and teardown
with a future wake pending. This is the SDK alarm service, not general timer-register
or cascade emulation, full thread scheduling or physical-device timing validation.
ASan/UBSan and TSan pass. The iPhoneOS arm64 probe archive contains the wake provider,
context cancellation, native import and generated original-queue check.

`NativeFloatRuntime.c` supplies the seven floating entries currently referenced by
compiled game/library code: raw-bit `_fadd`/`_fflt`/`_ffix`, typed `_fdiv`, signed and
unsigned 64-bit-to-float conversion, and `_dsqrt`. NaN results of single add/divide
use the original 0x7FFFFFFF pattern; float-to-int conversion saturates according to
the input sign, including NaNs. Square root preserves signed zero/infinity and
the original NaN payload/sign OR rule and errno 0x21 for negative/NaN inputs.
Finite arithmetic uses native compiler operations. A standard-library-only Python
oracle rounds exact fractions for float operations/conversions and uses integer
square-root midpoint comparisons for binary64 results. It generates 589 operand
pairs, 109 integer-conversion cases and 211 square-root/errno cases, including
subnormals, limits, ties, NaNs and infinities. Both direct and generated paths pass.
This does not port all aliases, processor-flag side effects or every entry of the
packed ARM runtime; its SDK unit remains incomplete. Full-game numerical pacing
and physical-device floating-environment behavior still require verification.
The direct core passes ASan/UBSan; generated C also passes TSan. The direct iPhoneOS
arm64 archive defines all seven entries and the probe exports the generated check.

`NativeSystemCalls.c` implements three BIOS entries used by the original SDK/game.
`SVC_CpuSet` reuses the bounded MI copy/fill functions, preserving the 21-bit
element count, 16/32-bit width and fixed-source flags, forward overlap behavior
and one-time fill-source read. `SVC_GetCRC16` reuses the original reflected 0xA001
table/update code with the caller's 16-bit seed and mapped-memory span checks.
Its supported BIOS contract requires an even address and byte length. Zero length
returns the seed without reading memory. `SVC_WaitByLoop` accepts positive signed
counts and reuses the interruptible native wait; chunking avoids overflow of its
32-bit cycle argument. This provides the cached ARM9 reference delay, not exact
BIOS overhead or cache-disabled timing. Interface contracts are cross-checked
against the [libnds BIOS declarations](https://github.com/devkitPro/libnds/blob/master/include/nds/bios.h)
and [GBATEK BIOS descriptions](https://www.akkit.org/info/gbatek.htm).
The shared check covers both widths, zero counts, ignored bits, overlap/fill
aliasing, independent CRC vectors with four seeds, split updates and buffer guards.
The generated-path CPU check also measures a positive loop wait against the host
monotonic clock; maximum-count waits and physical-device calibration remain untested.
The direct core passes ASan/UBSan and the generated path passes ASan/UBSan and TSan.
The iPhoneOS arm64 core archive defines all three entries; the probe archive exports
the generated shared check. Neither archive is a complete playable App.

The frontier module requests now use `FS_OVERLAY_ID`/`FS_EXTERN_OVERLAY` instead
of pointer casts of three linker symbols. Native IDs are 80, 81 and 42 in their
original load/unload order, with the asynchronous load mode retained; the legacy
target still uses the standard SDK linker-symbol macros. Both sound player paths
reference the existing `SNDi_DecibelTable[128]` definition rather than the empty
`_0210D750` anchor. `tools/nitro-section-renames/nitro_link_order.json` places that
anchor immediately before the original decibel section. No replacement audio
table or module-loader success stub is added to the production code.
An isolated shared check observes repeated frontier requests, verifies all 128
decibel entries against logarithmic rounding, checks all 724 channel-volume steps
for bounded quantization and monotonicity, and runs the original fader through
zero-duration, rising, falling and interrupted fades. The hardware divisor mapping
is documented by [devkitPro Calico](https://github.com/devkitPro/calico/blob/master/include/calico/nds/sound.h);
the 1/128 gain scale is described in [GBATEK](https://oafc.anod.cc/man).
The request observer does not load overlays, and these control/data checks do not
exercise either complete sound-player main loop, PCM mixing or physical audio.
Direct ASan/UBSan and generated ASan/UBSan/TSan checks pass. The generated check
is also defined in the iPhoneOS arm64 probe archive.

The profile-update byte buffer now uses its implicit C++ destructor and gives its
storage member a destructor that calls the existing `LobbyCharVector_Destroy`.
This releases the temporary range on normal scope exit and leaves compiler-driven
member cleanup available for a future exception-runtime implementation, without
changing the profile algorithm or adding a second allocator. The generated check
calls the actual `PPW_LobbyUpdateMyProfile`, byte-range assignment, profile copying,
comparison and cleanup functions. It covers a null work pointer, all rejected
states 0..11, empty/one-byte/32-byte/257-byte binary data, unchanged and changed
profiles, force-dirty behavior, surrounding byte guards and live-allocation counts.
Only the stored profile allocation survives a successful call; explicit cleanup
restores the baseline count. ASan/UBSan and TSan pass.
The iPhoneOS arm64 probe archive also defines the generated profile check.
The check's `__cxa_begin_catch` observer aborts if reached; it is not linked into the
whole-game audit and does not implement native unwinding. Throwing constructors,
out-of-memory unwinding, remaining C++ runtime gaps and live lobby sessions
remain unverified/incomplete.

The event deque capacity helper now defines its ownership cleanup rather than
leaving the local `Deque` destructor unresolved. Its existing storage base owns
the index array, including during a failed copy constructor; the complete deque
cleanup reuses `ov45_0223CA14` and clears the member owner before automatic base
cleanup, avoiding a second release of the same array. The private C++ type views
are scoped to `LobbyDequeCapacity`; C entry points and original 16/16/24-byte
storage layouts are retained and asserted. No new allocation algorithm is added.
The generated check includes this actual implementation and links the original
reserve/push/copy/insert/cleanup helpers. It runs 84 cases of empty/nonempty maps,
wrapped indexes, front-slack reuse and 0/1/127/128/129/257-element capacity requests,
checks retained blocks/data/order/sentinel and exact allocation counts, and tests
wrapped index-array copying with independent array ownership. Every scope returns
to its starting allocation count; ASan/UBSan and TSan pass. These are valid-input
checks with allocator hooks, not proof of failed-constructor unwinding, the full
lobby runtime or live network sessions; the production exception runtime is still
unresolved.
The generated owned-deque check is defined in the iPhoneOS arm64 probe archive.

The ABI32 memory-DMA SDK entry points now call `HGNativeDmaRequest` rather than
writing/polling DS DMA registers. The client resolves source/destination spans
through its own graphics-memory mapper before submission. The native import
checks the 12-byte descriptor format, offsets, lengths, alignment, consistent
overlapping mappings, control mode and complete coverage, then retains an owned
descriptor snapshot. Workers reuse `NativeDMA.c` for copying/filling and post
completion IRQs; translated callbacks run only on the event owner, including
callback reentry through `MI_WaitDma`. Zero-byte callbacks retain synchronous
system-mode behavior. Both 16/32-bit transfers, fixed/reverse addresses, physical
aliases and mappings crossing VRAM A/B are checked through the actual SDK client.
The native backend separately checks fragmented ranges, valid/invalid overlaps,
guard bytes, cancellation and resource disposal before IRQ delivery.

DMA requires fixed-size wasm32 memory so native worker pointers cannot move.
The pinned WABT runtime returns immediately for `memory.grow(0)`; nonzero growth
is disallowed by the instance maximum. A per-environment atomic reference count
prevents detachment during a retained transfer. Detachment also fences the queues
without guest dispatch before clearing IRQ bridges, covering completion posts
that can follow the final memory-reference release. The retained-reference guard
is tested with an explicit occupied-counter fixture; transfer/wait/stop tests
separately check real reference balancing and absence of stale completion IRQs.
Backing memory and descriptors must remain alive until completion/stop. Timing,
repeat, card and graphics-FIFO DMA, register readback, full-frame bus behavior
and physical-device timing remain unsupported/unverified; they are not replaced
by a successful no-op.

Named register lvalues now use the native mapper in ABI32 builds while `_ADDR`
constants retain bus addresses for original data and SDK contracts. Generated
views resolve numeric offsets up front for standalone legacy headers, and the
317 typed legacy register macros are reapplied after their original definitions.
This fixes out-of-bounds VRAM-control reads in the end-to-end DMA path. Direct
encoded-pointer accesses and register side effects still need individual ports;
this routing change does not implement every IO device. Direct ASan/UBSan and
generated ASan/UBSan/TSan checks pass; the iPhoneOS probe includes the native DMA
provider, import and generated SDK-client check. Its runtime keeps system
`<assert.h>` while platform sources use the quoted game assertion header with
`PM_KEEP_ASSERTS` enabled.

The original arena API now uses the shared `NativeOSMemory.c` implementation in
ABI32 as well as the direct core. Each translated instance owns aligned Main,
ITCM, DTCM and shared data pools; ARM7-owned regions remain unavailable, the
standard MainEx arena is disabled, and main WRAM retains its empty range.
Their capacities derive from the SDK memory constants. Native executable code
and stacks are outside these data pools, so complete-engine reservations/peaks
still need calibration. This replaces DS shared-memory arena pointers and the
arena initializer's CP15/linker dependencies; it does not implement ARM contexts,
TCM execution modes, arbitrary encoded RAM accesses or every memory service.
Initialization is idempotent and bound changes use the existing interrupt
control bridge. Allocation checks include invalid alignments, overflow, exact
exhaustion, zero sizes, cleared bounds and unaligned high endpoints. Low-end
allocation now rejects a rounded end beyond the limit; high-end allocation
retains the original two-stage downward alignment. The shared direct-core path
passes the same boundary check.

The probe now initializes the actual `src/heap.c` plus original NitroSystem heap,
list and allocator code instead of the command table's fixed storage hook.
A checked two-MiB root fixture and communication child heap bootstrap both
instances. Checks create/destroy front/back child heaps, allocate/free through
both ends, shrink allocations and restore the parent's free space. They also
verify that one instance's arena state/data cannot change the other's. Native
heap guards reject overflowing block-header/rounding operations, impossible
region sizes and invalid signed alignment; failed growth preserves block bytes.
The command-dispatch check now verifies real heap-space recovery after cleanup.
The DMA client check also allocates source/destination buffers from opposite ends
of the actual game heap, performs synchronous/asynchronous copies, waits before
freeing them and verifies that free space returns to its baseline.
Wi-Fi's existing local allocation observers have distinct names, so they cannot
replace the SDK allocator used by this check. Communication is inactive in this
fixture and its reset-UI observer traps; live communication/OOM reset and failed
constructor unwinding are not validated. Direct ASan/UBSan and translated
ASan/UBSan/TSan checks pass. The probe's fixed memory has increased to 16 MiB to
hold these real data pools; it remains an isolated check, not a complete App.

The legacy SDK `OS_interrupt.h` and `nitro/OS_interrupt_shared.h` now select the
canonical native IRQ API/types under `HEARTGOLD_NATIVE`. They retain their legacy
register/memory dependencies and the non-native definitions. Native translation
units no longer get a second set of inline master-enable/mask readers backed by
the IO register array rather than the event queue. The shared direct/generated
check includes the legacy shared/type header and then its API header, verifies mask read/write and saved-enable
results, and leaves CPU mode/masks unchanged. The generated cross-unit check also
reads canonical mask state through the legacy view and enables queued original
VBlank/HBlank callbacks through it. Direct ASan/UBSan and generated ASan/UBSan/TSan
checks pass. Native IRQ declarations avoid the unrelated hardware system-work
types; implementation files explicitly include thread types where they use them,
so the legacy and canonical type families can coexist without redefinitions.
This does not port the original TCM IRQ-stack checker, ARM IRQ handler/context
switcher or the complete cartridge path that uses the legacy header.

Generated IRQ callbacks now use a separate 64 KiB guest temporary stack with
end guards. The embedder binds the module's stack-pointer slot and the typed
four-argument guard callback before guest IRQ initialization. Plain, argument,
timer/DMA and RTC reply callbacks enter the same guard; it preserves/restores
the original SDK coprocessor operand/mode record around each callback. The caller's
stack pointer, outer runtime jump buffer and saved call depth survive normal IRQ
completion. Initialization rejects invalid configuration spans, callback types,
alignment and stack sizes before replacing the active binding.

The pinned C-generation step checks every generated stack-pointer assignment
while an IRQ is active. Source compilation disables leaf red zones, and checked
frames reserve the prebuilt WASI libc's 128-byte leaf red zone. Unknown generated
assignment/header layouts fail the build. A 1 KiB caller stack stays untouched
while a callback uses 8 KiB of locals and replaces division/root operands. An
isolated oversized callback must fail with stack exhaustion in its prologue;
entering its body produces a different failure and fails the check. IRQ traps
remain fatal so error jumps cannot strand native event locks. This is temporary
stack/context protection, not a native replacement for thread call-stack switching.

The translated SDK file interface now uses `HGNativeFileRequest` and the existing
read-only `NativeFile.c` backend. Guest `FSFile` remains 0x48 bytes: native streams
live in host records keyed by environment and guest-object address, with checked
status/position/length mirrors rather than a truncated host pointer. Instance-local
path IDs support convert/open-fast and bounded file ranges; they are opaque native
tokens, not original ROM FAT IDs. Checked canonical directories provide relative
paths, while the backend confines files/symlinks to its configured content root.
The original SDK archive-name registration remains in use. The native host index
requires no guest FAT/FNT cache, so `FS_TryLoadTable` reports zero cached bytes.
Range-relative seeking now honors the image top in the shared backend.

The check creates temporary files plus a copy of the supplied ROM's `a/0/0/0`.
Actual original NARC code reads its first/middle/last members through the SDK client
and game heap; host comparisons verify every returned byte. Other cases cover
sync/async-style reads, EOF, signed seek clamping, subranges, guards, duplicate
close, bad pointers/unterminated paths, traversal/symlink escape, independent
instance handles/cwd, open-object reinitialization rejection and context cleanup.
The Wi-Fi file observers have distinct symbols and cannot mask these real APIs.
After callbacks are detached, context cleanup closes native streams and releases
cached path IDs before the environment can be freed. Root changes remain blocked
while streams or cached directory snapshots are open. The file check requires the privately imported ROM pack;
it does not publish its bytes. ASan/UBSan and TSan pass.
The iPhoneOS arm64 probe defines the native file service/context cleanup and the
generated file/NARC checks; the direct core's file backend also builds for iPhoneOS.

The original SDK's internal seek/read/find/get-path commands also route to this
backend. Directory zero is the content root; checked positions preserve a cursor
within an instance-owned, sorted snapshot. File/directory entries, skip-name reads,
position save/restore, empty/end-of-directory, parent traversal, path-size queries
and short output buffers are checked through the original interfaces. Escaping
symlinks are omitted, unrepresentable names fail the snapshot, and context cleanup
releases all snapshots before allowing a root change. Names are sorted by byte
value and path lookup follows host filesystem case rules. These are native opaque
IDs/positions, not the ROM's original FNT ordering, IDs or case-insensitive lookup.

Original FNT positions/order/case rules, custom archive volumes, raw ROM file IDs,
ROM-header/card bootstrap and complete sound/filesystem lifecycle still need
ports. Async-style reads currently finish synchronously, matching the existing
direct-backend shortcut; background streaming and frame pacing are unverified.
These file-interface checks do not establish complete resource/version coverage
or a playable App.

The SDK `MI_UncompressLZ8` entry now calls the existing LZ10/LZ11 decoder through
mapped native memory, preserving original background callers rather than leaving
their ARM assembly unresolved. The same decoder serves bounded NARC loads and
legacy SDK calls. SDK calls validate token distances/lengths, preflight destination
spans and reject input/output aliases before writing, including aliases between a
bank pointer and its physical address. Source and output can cross mapped VRAM
bank boundaries. The legacy interface has no allocation lengths: it checks mapped
address bounds, but cannot prove a compressed heap object's true length. External
NARC resources therefore retain explicit member-size bounds. Reverse overlay
decompression (`MIi_UncompressBackward`) and complete resource coverage remain
unported/unverified.

Shared direct/generated checks exercise the SDK entry and original
`BG_LoadScreenTilemapData` caller, LZ10/LZ11 reference-length forms, 4 KiB distance,
guards, cross-bank source/output, alias rejection and failed-output preservation.
The existing bounded decoder/resource checks remain in place.

Native arithmetic register views now provide signed 32/32, 64/32 and 64/64
division, remainder and exact integer square roots. Result/status reads compute
the native result; control modes, full-width zero flags and signed overflow cases
are retained. The register adapter also handles 64-bit views and contiguous
operand pointers used by the original vector and projection code. Other SDK
`_ADDR` constants keep their existing DMA bus semantics. Native `CP_SaveContext`
and `CPi_RestoreContext` preserve the original 28-byte operand/mode record.
Unsigned shifts retain ARM wrap behavior without signed-left-shift undefined
behavior in the original division and projection functions.

The shared check runs division boundaries, 512 integer-root properties, context
guards/restore and original fixed-point division/root, vector length/distance/
normalization and perspective/orthographic matrix functions. The Wi-Fi check's
old modulo stand-in is removed in favor of the real SDK implementation. Arithmetic
edge rules were cross-checked against the [upstream arithmetic device source](https://github.com/melonDS-emu/melonDS/blob/master/src/NDS.cpp#L1693).
This adds native arithmetic operations; ARM instruction execution is not involved.
Hardware busy-cycle latency remains uncalibrated. These arithmetic checks alone
do not establish thread scheduling or full-scene behavior.

The native thread path now retains the SDK's priority/list/queue, mutex,
join/destructor and sleep-alarm code. Its scheduler transfers between live native
C stacks instead of saving ARM registers. Apple arm64 only supplies the initial
stack bootstrap; libc saves continuations while their C frames remain suspended.
Each worker owns a non-executable 512 KiB native stack with protected end pages.
ASan/TSan fiber hooks and native floating environments follow each transfer.

The binding also saves/restores the module temporary-stack pointer, runtime trap
buffer/unwind target/call depth and native interrupt mask. Generated stack writes
check the current thread's supplied guest stack bounds, including the libc leaf
red zone. Word-aligned supplied stacks are accepted; the running stack pointer is
aligned down to 16 bytes. A non-16-byte-aligned stack has a switch/destruction
check too. Native guest thread stacks currently require at least 512 bytes.
The embedder reserves the top 16 bytes before guest startup and retains
the initial stack high-water address in `rootStackHigh`; the link explicitly uses
a first, 64 KiB launcher stack. SDK launcher guards therefore cover real module
stack storage, without using the original TCM/linker addresses. IRQ-exit scheduling
invokes the original scheduler after callback mode/stack restoration. The native
idle thread waits for enabled IRQ events rather than spinning.

The shared check runs two sequential independent game instances through preserved
local arrays and coprocessor state, 24 equal-priority yields, queue sleep/wakeup,
recursive mutex blocking, join/normal exit, forced exit/destructors, destruction of
a suspended thread and alarm-driven sleep through the idle thread. Teardown
discards suspended native frames, releases stacks and detaches the scheduling hook
before freeing the instance. Invalid commands/contexts and stack limits have
runnable rejection checks. The socket timeout observer has its own sleep symbol,
so it cannot mask the real SDK sleep path.

Only one kernel is active on its owning native thread at a time; sequential
instances are checked. Launcher termination, arbitrary standalone
`OS_SaveContext`/unregistered `OS_LoadContext`, shared destructor-stack relocation,
full-scene stack budgets and physical-device scheduling remain unsupported or
unverified. Thread traps are fatal. These checks are not a complete engine/App
startup or gameplay result.

The native geometry service implements matrix modes, fixed-point load/multiply/
scale/translation, paired position/vector slots, projection/texture slots and
clip/vector readback. It reuses the original `MTX_Concat44` implementation.
The existing SDK matrix wrappers and readback/reset routines call this service;
the ARM 48/64-byte FIFO helpers and NOP drain have native implementations.
Packed FIFO matrix commands retain partial parameter state between batches and
reject invalid batches without committing their changes. Stack index wrapping
and error state follow the [arithmetic/geometry device reference](https://github.com/melonDS-emu/melonDS/blob/master/src/GPU3D.cpp#L1628).

Shared direct/generated checks exercise original SDK matrix calls, hand-specified
non-unit clip matrices, vector behavior during scaling/translation/rotation,
paired push/pop restoration, projection reset, texture reset, split FIFO load
packets, guards, unknown commands and stack boundaries.

Unlit geometry commands also record color/UV state, every vertex encoding,
triangles/quads/strips, polygon/texture/palette/viewport attributes and buffer swaps.
Vertices retain signed fixed-point homogeneous clip coordinates after the original
matrix multiplication. Two bounded banks (6,144 submitted vertices / 2,048 polygons
each) keep the published frame stable while the next frame is built. Typed copy-out
APIs expose vertices and polygon indices to the future native raster/Metal consumer.
Calls are confined to the game owner; concurrent readers require an integration
handoff. FIFO inputs stay immutable during a synchronous call. A validation pass
checks capacity, commands and swaps before the commit pass writes either bank.

The check uses the original SDK display-list builder to construct and submit a
triangle, checks transformed clip/depth/UV/color values, quad/strip index ordering,
all vertex encodings, publication stability, rollback across a swap, output guards
and exact capacity limits. SDK viewport/polygon/texture/palette/swap wrappers use
this service.

Normal/material/light commands now provide four fixed-point lights, captured
direction transforms, diffuse/ambient/specular/emission terms and the 128-byte
shininess lookup table. Signed intermediate widths and truncation are explicit.
The original SDK material and display-list functions feed these commands.
Texture coordinates also support direct, UV-matrix, normal-generated and
vertex-generated modes at their command-specific update times.
Shared checks cover emission with no lights, single-light diffuse/ambient/specular,
four-light saturation, table enable/disable and split table packets, diffuse color
replacement, direction capture across matrix changes and all three generated UV
modes. These fixtures do not establish complete original model lighting fidelity.

The native software pixel path consumes published geometry.
It clips homogeneous coordinates, applies the SDK viewport convention, culls faces,
scan-converts convex polygons, compares Z depth and interpolates vertex colors
with reciprocal-W correction. Clear color/depth are captured per published frame.
Main-engine 3D BG0 pixels enter the existing 2D priority/window/blend/brightness
compositor, including horizontal offset. Caller output remains untouched for
unsupported frames. Shared checks use original display lists for opaque pixel
coverage, depth order, culling, near/far/side clipping, nonuniform-W color samples,
zero-W safety, viewport limits, publication state and unsupported output guards.
The direct core also checks 3D BG0 over a 2D backdrop and horizontal offset.

This is not a pixel-identical DS renderer. Fixed-function edge/interpolation
precision and hardware frame comparisons remain unverified.

GPU texture views now read the existing VRAM banks in texture/palette modes,
including slot assignment, unmapped zero reads, overlapping-bank OR reads and
address wrapping. The sampler decodes A3I5, palette4/16/256, compressed 4x4 (all
four palette modes), A5I3 and direct RGB555. It handles signed fractional UVs,
clamping, repetition/flips, palette bases and format-specific transparency.
Texels enter the pixel path with perspective-correct UVs and modulate/decal color
and alpha combination, including fractional texel transparency.

Checks run the original CPU and DMA texture upload APIs through bank restoration,
noncontiguous/cross-bank copies, compressed descriptor slots, every sampler format,
negative/repeated/flipped coordinates, transparent color and textured pixels.
These are fixture/API checks, not complete original model/asset coverage.

NitroSystem's original `g3d_gecom.c` now submits packed display lists and buffered
commands to the native geometry service, preserving flush order and current-matrix
queries. Whole command packets validate before committing state. The shared check
uses the original G3B list builder, small/large lists, both original DMA choices,
the exact 192-word buffer boundary, matrix restoration and rendered pixels in both
direct and generated native builds. Native submission is synchronous; original
asynchronous GX DMA timing is not reproduced.

The original orthographic-camera, look-at and axis-rotation wrappers now select and
multiply native geometry matrices through the existing SDK functions. Shared checks
compare quarter turns, repeated rotations, preserved translations, orthographic
projection and two camera directions with explicit fixed-point golden matrices;
calculation-only calls leave geometry state untouched.

Original software-sprite camera setup and `NNS_G2dDrawSpriteFast` submit their
stored projection, translation, scale, texture coordinates and four vertices to
native geometry. The original UV conversion and quad order remain in use. Shared
checks upload texture data through the original SDK, render the original quad,
and verify pixel bounds, both-axis flipping, UV cropping, screen clipping,
negative positions/UVs, zero width and projection-slot restoration. These checks
cover the shared sprite path, not complete Pokepic/battle animation playback.

The generated native check also runs the original `pokepic.c`, G2D resource
unpacking/image upload and palette code against the supplied ROM. It creates real
Pokepic managers, loads Chikorita/Hoothoot front/back artwork and palettes, and
renders both original frames through `PokepicManager_DrawAll`. Full-frame
RGB555/opacity hashes are independently pinned from the repository's original PNGs.
Front and back use the shared normal palette from the front PNG, matching the game.
A controlled animation script checks frame changes, positive/negative offsets,
completion and deletion; original heap space is recovered after each manager.
The original sine/cosine table is assembled and byte/relocation-verified by the
existing data converter. This is an isolated original-sprite integration check;
it does not establish full battle scripts, move effects, audio or App integration.

`MIi_UncompressBackward` now uses a native translation of the original `crt0.c`
backward-LZ decoder. Its bounded entry point validates the complete stream before
in-place writes, preserves uncompressed prefixes, and rejects tokens that overwrite
unread input. Shared checks cover overlapping references, footer padding, unaligned
buffers, truncated headers, invalid distances/lengths and insufficient capacity;
failure leaves the input untouched. The SDK wrapper validates the mapped contiguous
range and uses the native cache APIs. This supplies decompression for existing
overlay callers; native overlay lifecycle/startup still needs integration.
Decompressed ARM bytes are not executable native code.

The original `OS_Halt` API and SDK idle thread now use the native event condition
wait. An enabled pending IRQ wakes the wait even when CPSR.I masks callback
delivery; callbacks remain pending until interrupts are restored. This follows
the [ARM946E-S manual, section 2.3.10](https://documentation-service.arm.com/static/5e8e3ee588295d1e18d3aa82).
Direct and generated native checks cover delayed and already-pending requests,
masked wake-up without callback delivery, enabled wake-up with delivery, and
invalid/IRQ-context wait rejection. Original scheduler sleep/idle checks continue
through the public SDK API. This does not add a display or ARM7 event producer.

After SDK thread initialization, original `OS_WaitIrq` now sleeps on the original
IRQ thread queue instead of blocking the native owner directly. The native IRQ
exit path runs `OS_IrqHandler_ThreadSwitch`, waking that queue and preserving
original priority scheduling; pre-scheduler waits retain the bootstrap path.
Only a dispatched IRQ triggers the exit hook. Tests cover two different IRQ flags,
unrelated wake-ups, low-priority work while the root waits, masked root waits,
restored math/CPU state, an empty queue after completion and the public dispatcher.
These are SDK scheduling checks, not verification of the game's full startup loop.

`OS_SetIrqStackChecker` initializes the existing native IRQ stack's two 16-byte
guard regions, using the same helper as IRQ setup. It no longer computes a stack
address from the DS `SDK_IRQ_STACKSIZE` linker token. Checks verify both guard
regions, unchanged stack interior bytes and unchanged native DTCM storage.
Initialization from an active IRQ callback fails before rewriting the guards;
a subprocess check requires that rejection. Overflow-prologue checks remain active.

Native VCOUNT reads now use an independent monotonic display clock. The cadence
uses 355×6 system clocks per line and 263 lines per frame, also documented in the
[melonDS GPU timing constants](https://github.com/melonDS-emu/melonDS/blob/master/src/GPU.cpp),
with this repository's nominal `HW_SYSTEM_CLOCK`. Integer conversion handles the
full 64-bit nanosecond range without overflow. Original `OSi_WaitVCount0` preserves
master/CPU interrupt state and waits until scanline zero is actually observed.
It sleeps for most of the interval, then polls the final millisecond; integration
with the renderer's frame-boundary event and real-device timing calibration remain.
Checks cover all line boundaries, frame wrap, large timestamps, refreshed MMIO
reads and wait-state preservation. The original empty autoload-completion callback
also retains an empty native implementation. These changes do not supply VBlank,
HBlank or VCount-match IRQ production, capture, or scanline register effects.

`NativeMicSampler.c` supplies the native microphone sample-format/buffer stage,
following the ARM7 source's 8/12-bit quantization, signed conversion, bounded DC-bias
updates and timer-period quantization. Eight-bit samples publish as halfword pairs;
the last address identifies the last published halfword. Finite and looping buffers
report completed rounds, and calibration persists across starts. Missing input
repeats the prior sample or uses the original filter-off sentinel. Direct native
sanitizer checks pin every format, publication, wrap/stop, calibration thresholds,
invalid parameters and unchanged state after rejected starts. Input must already
be mono PCM at the configured rate; live iOS capture and permission/session handling
are not yet connected.

`NativeMicResampler.m` provides stateful planar-Float32 downmix/resampling through
Apple `AVAudioConverter`, then feeds the original-format sampler through a C sink.
It uses normal priming and the mastering/max-quality converter configuration;
checks verify DC gain, whole/split-stream agreement at 48/44.1 kHz to 2/8/32 kHz,
ramp continuity, stereo cancellation, end-of-stream handling and the 1,984-byte
Chatot recording buffer. The Objective-C object target has clean platform include
paths to avoid legacy SDK header collisions. These are offline conversion checks;
live capture, permission/session handling and buffer pooling remain to connect.
Calls on a converter must be serialized, and sink buffers last only
for the duration of their synchronous callback.

The ABI32 MIC path retains original `mic.c` request locking, result mapping,
synchronous waits and user callbacks. Its ARM7 transport is replaced by
`NativeMicImports.c`, which maps fixed instance memory into the native sampler and
posts original start/stop/full replies through the owner thread's FIFO IRQ queue.
Native synchronous wrappers preserve results even when a reply arrives before the
asynchronous wrapper returns; failed sends release the original request lock.
`HGNativeFeedMic` accepts already resampled mono PCM on that owner thread, splits
input at buffer boundaries, and stops consuming old input when a callback stops or
restarts sampling. Queued replies prevent instance release; final cleanup stops
sampling and frees the instance's native context. Shared public MIC declarations
also correct the voice-chat caller's return type. The bridge check covers all six
sample formats, paired-byte publication, finite/loop completion, masked IRQs,
reentrant stop, rejected sends before IRQ initialization, invalid ranges/signatures
and separate instance buffers. It uses
injected PCM and does not validate device recording or the full audio subsystem.

The native Pokewalker request, transfer and status views now share one original
0x134-byte work area. Internal labels address fields within that area; message
queue storage overlaps the reserved transfer prefix as in the original layout.
The ABI32 work view fixes its 64-bit tick field at offset 0x3C with four-byte
packing. Compile-time checks pin view sizes/offsets, and native checks run original
upload/receive/gift request functions, transfer/progress/error accessors, compression
buffer setup and an SDK message round trip against the same state. Original IR
packet/link BSS bytes use the existing data verifier. This validates storage and
request logic; it does not supply the infrared/card-SPI transport.

The pixel path also handles partial polygon alpha, alpha-test thresholds, blending
enable state, translucent polygon-ID rejection and optional depth writes. Opaque
polygons precede translucent ones; automatic ordering uses viewport Y bounds,
while manual mode preserves translucent submission order. Original `MATH_QSort`
sorts these records with a deterministic submission-order tie break. Control and
alpha-reference state are captured per published frame. 3D output alpha uses the
source/destination maximum, and 3D BG0 uses its 5-bit alpha over selected 2D second
targets. The check covers equal/different IDs, blending on/off, depth-write on/off,
opaque-prefix ordering, auto/manual differences, threshold boundaries, preserved
frame state and 3D/2D composition. Five-bit color quantization and exact hardware
edge/interpolation behavior still need original-frame comparisons.

Toon/shadow, W buffering, fog, depth-equality,
one-dot/wireframe modes, wrapped viewports and 3D mosaic remain unsupported.
This does not implement GX FIFO DMA timing,
arbitrary direct command-register writes, complete `G3X_Init` or full 3D scenes.
The App still uses its prototype renderer; original-engine 3D integration remains incomplete.

Current audit checkpoint: all 1,121 game sources compile (1,120 are selected by
`main.lsf`); 504 of 514 library sources and thirteen native platform units compile;
232 of 257 data sources verify completely (including three declaration-only files).
Eleven additional mixed files provide verified constant objects while remaining
incomplete; 243 data objects enter the relocatable merge and 25 files still fail
their whole-file data audit because ARM exception metadata is unsupported.
The independent inventory finds no incompatible defined/external function signatures.
The relocatable object merge succeeds, while 57 symbols remain undefined at the
intermediate stage. Twenty-three are intentional imports with native bindings for
atomic/lock-ID/cache/CPU/IRQ/RTC/clock/alarm/DMA/file/thread operations described above; other SDK/data,
source and linker-generated symbols still require resolution.
The ABI32 arena now owns aligned 16 KiB `SDK_AUTOLOAD_DTCM_START` storage, and
`OS_GetDTCMAddress` returns that actual base even as allocations advance.
`NativeCtrdgData.c` owns the documented 28-byte `gCtrdgInitWork` state instead of
the original empty data anchor. Native checks verify field offsets, original PHI
acknowledgment callback writes and per-instance isolation. This supplies state
storage, not cartridge/ARM7 communication services.
The overall audit remains unsuccessful because of missing library/
data objects and unresolved symbols. This is not a fully linked executable or App;
these counts measure build gaps, not features or gameplay completion.
The strict final intermediate engine link currently fails on 21 diagnostic names
reachable from `NitroMain`; this differs from the all-object inventory above.
Reset, microphone, startup/exception and remaining runtime/data definitions still
need implementation. No final engine module or native App is produced by that gate.

Remaining work includes complete resource dependencies and SDK/platform bindings,
graphics memory/register routing, callbacks and task scheduling, full engine linking,
single-screen scene integration and runtime comparison. The current probe has 16 MiB
of memory and exercises isolated interfaces rather than complete scenes. Physical iOS execution is unverified;
a command-line simulator attempt stalled inside dyld before reaching the test entry point.
Other reconstructed callback views, including PPW web requests, still use untyped
function pointers and are not covered by these callback contracts.

Tool provenance: [WABT wasm2c](https://github.com/WebAssembly/wabt/tree/1.0.42/wasm2c),
[WASI SDK 34](https://github.com/WebAssembly/wasi-sdk/releases/tag/wasi-sdk-34).
WABT's C support is supplied by its downloaded release under its included license;
it must retain the applicable notices when incorporated into the shipping App.
