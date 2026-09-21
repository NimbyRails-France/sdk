# Native signal settings panel — static evidence

The BAL mod declares its panel through `Mod::signalSettings`. The SDK owns
rendering and settings. No NimbyScript is required by this integration.

## Binary and scope

Read-only Ghidra exports for NIMBY Rails 1.19.10, SHA256
`fff49ac21720abfc824c2b4f68b862727630eb0db71cfe1f9ea8f685d0db10ae`.
Evidence: `reports/cpp-signal-ui/`. Addresses below are RVAs for this binary,
not validated hooks. No native UI call or patch was executed during this study.

## Candidate insertion points

- `0x7c5020` constructs the `##signal_editor` window.
- `0x7a0a40` declares the signal editor's controls. Its first argument is a
  captured context; its second is the native UI declaration interface.
- The built-in `Check beyond stops` control in that function calls virtual
  slot `+0xf0` with the declaration object, localized label and a pointer to
  a 32-bit local value. It compares that value with the signal's byte at
  `+0x6d`, then marks the working copy dirty. This is evidence for a checkbox
  primitive, not a complete ABI specification.
- That same editor calls `0x5a1f30` with captured editor object `+0x21b8`,
  two context arguments and the declaration interface. The callee draws the
  built-in Extensions heading and script instance tabs. It is shared native
  UI infrastructure; calling its script machinery is not part of our design.
- `0x55b8e0` merges style properties into the declaration object only where
  the destination property is unset. It also appends property collections.
  It must not be treated as a style reset or a harmless pure function.

## Requirements before a live hook

Verify the concrete declaration vtable and checkbox implementation, signal
identity in the captured editor context, UI thread and callback lifetimes.
The checkbox's value pointer may be retained by the UI implementation; the
decompiled call site alone does not establish ownership or lifetime.
Establish balanced layout scopes and stable widget identities as well.

The store is implemented independently in `nimby/signal_settings_store.hpp`.
The adapter registers the schema and the runtime reads it using
`nimby::readSignalSettings(id)`. The native bridge must supply an actual save
identity and a complete coherent signal catalogue before making it available.
Rendering, save-file persistence and live checkbox validation remain pending.

## Concrete implementations identified

`InspectNativeUi.java` exports the RTTI-named vtables into
`reports/cpp-signal-ui/native-ui-symbols.txt`. Two concrete implementations
share the checkbox slot `+0xf0`:

| Implementation | Vtable RVA | Checkbox RVA |
| --- | --- | --- |
| `NuklearEmit` | `0xa83470` | `0x560870` |
| `LayoutEmit` | `0xa83818` | `0x55cd20` |

The layout implementation calls the size/layout helpers `0x55c420` and
`0x55c4b0`. It does not use the value pointer in the recovered code. The
interactive implementation dereferences a 32-bit value, passes the value
itself to `0x50cc40`, and writes the returned value back synchronously. The
original caller's value pointer is not passed to that helper. Label lifetime
still requires checking the lower-level draw implementation.

On `NuklearEmit`, slot `+0x08` (`0x55d2c0`) pushes a style/group entry and
slot `+0x18` (`0x55d430`) pops one. Insertion must preserve that balance and
the matching layout structure; calling only the interactive implementation
would bypass layout construction.

The SDK now provides an owned `SignalSettingsStore::Frame` and internal
`drawSignalSettings` presenter. Its UI adapter is still to be supplied by the
native hook. Frame acquisition is atomic with session/selection validation;
no store lock spans a UI call. Only an interactive pass can commit changes,
and each commit revalidates the editor token. Tests cover layout-only calls,
stale selections and schema invalidation during rendering. These tests do
not establish the native hook ABI or prove the panel works in game.

## Editor invocation and adapter

`0x7d6060` calls the editor body `0x7a0a40` twice: call sites
`0x7d63f7` (layout) and `0x7d6a3f` (interactive). Both pass `param_18[3]`
as the captured context. The outer layout group remains open across the
body call, providing a candidate place to append the SDK controls after
the original body returns. The interactive call is conditional on the
window being drawn; a layout pass does not guarantee an interactive pass.

The capture constructed in `0x79f4d0` stores the original Signal pointer at
offset `+0x38` (`puStack_e0` relative to `local_118`). Its first 64-bit word
is the full signal ID, also passed to `0x57c820` in the original editor.

`engine/signal_ui.h` implements the native checkbox adapter and guarded
capture identity reader. Binding recognizes only the two concrete vtables
and verifies their checkbox slot. It requires prior executable identity
verification and must only live inside a synchronous UI callback. The
adapter is not installed as a hook yet. Its tests use simulated memory and
make no native calls. Descriptions/tooltips are not rendered yet.

`runtime/signal_settings_panel.h` now connects this adapter to the store via
`SignalSettingsPresentation`. A layout callback replaces the pending frame;
the matching interactive callback consumes it exactly once. Skipped interactive
passes are supported. If the session or selected signal changes between passes,
the original control count is still drawn but edits are rejected. An unknown
UI implementation is never called. The wrapper must be invoked after the
original body returns, inside its caller's outer layout group.

This is the render callback implementation, not an installed hook. The bridge
still needs executable verification, native hook registration/draining, actual
session identity and complete catalogue observations. Native label lifetime,
panel styling and live rendering also remain to be validated. Tests simulate
the two-pass sequence, including missing/duplicate passes and session loss.

At the latest runtime inventory, NIMBY Rails PID 10236 is running, CLion has
no active debug session, and the available run configurations target the mod
and its tests, not the native game editor. No mouse input or debugger attachment
was performed during this implementation pass.

## Live editor probe (subsequent authorized UI pass)

The user explicitly authorized resuming game control. The native diagnostic
`NimbySignalUiProbe-v1.dll` was then built and loaded into PID 10236 through
the existing checked loader. Its hook at `0x7a0a40` verifies the binary hash
and the 16 entry bytes before installation, calls the original body once,
and records only the callback context, selected signal, pass and thread.
It draws no widgets and changes no signal/save data.

The first observation returned zero callbacks while the game was minimized.
After reopening the native signal editor, a two-second observation recorded
408 callbacks. The last 16 alternated pass 1 (LayoutEmit) and pass 2
(NuklearEmit), all on thread 57708, all with signal ID 2251799905894402 and
capture address 780114316656. The UI displayed `Path signal 1407.2`, consistent
with the full ID's index and generation. This validates the candidate body
signature, concrete vtable classification and capture signal offset for this
running binary. It does not yet validate drawing additional controls.

The client clears the observation lease on exit; a ten-second deadline also
limits collection if the client disappears. The diagnostic DLL and trampoline
remain pinned until game exit. The hook still forwards to the original body;
the collector is inactive. Restart before replacing this pinned DLL or using
a production hook that requires the original body-entry fingerprint. This
diagnostic target is not installed or packaged as the public SDK runtime.

## Resident renderer bridge implementation

`NimbySignalUiBridge-experimental-v1.dll` now implements the verified editor
hook and the resident multi-panel host. The bridge copies declarations through
the fixed-buffer C ABI in `nimby/detail/signal_ui_bridge.h`; it keeps no mod
callbacks or STL objects supplied across the DLL boundary. Register, remove,
begin-session, observe-catalog, suspend and read exports are implemented.
The ABI is internal and same-process; pointers must refer to valid caller buffers.

Layout freezes the complete ordered panel list. If an owner stops before the
interactive pass, its old slots are still consumed, while its session token
rejects edits. A storage exception also preserves consumption of subsequent
layout slots and emits a debugger diagnostic. The original game body is called
once, and the renderer is pinned before the native hook is enabled.

Validation includes a real C executable loading the DLL, resolving all exports,
registering a declaration, destroying the caller's declaration buffer and
reading its copied values. Bootstrap refuses this non-game executable.
Separate model tests exercise clicks, suspension, stale sessions, unregister
between passes and storage-capacity exhaustion. These are not visual game tests.

This bridge has **not been injected into the running game**. The existing
diagnostic hook must first be removed through a normal game restart. The bridge
is now included in SDK install/drop-in staging and the installer manifest.
The SDK's same-process loader resolves it beside the SDK DLL, validates the
game and resident bridge binary, and calls its checked bootstrap. An existing
diagnostic probe is rejected rather than installing a second hook at its entry.
The mod adapter requests initialization, connects to the resident
bridge, retries from its observation worker, reads
its values and unregisters its panel after stopping that worker. Diagnostic
hosts retain their local test store and never attach automatically. The client
owns a DLL reference and serializes export calls with close. A real-DLL test
checks this lifetime, suspension and stale-session rejection.

Snapshot world metadata and observed load generations now establish the panel
session; the adapter supplies the complete catalog before calling the mod.
Exact load notification, validation of automatic loading/rendering in the game,
and file persistence remain required. Registration alone does not enable a panel.

The adapter's `signalSettingsCatalog` helper now projects an existing SDK
snapshot into owned full signal IDs and resolved texture-set names. Missing,
duplicate, foreign or unresolved rows reject the whole catalog; an empty
successful snapshot remains distinguishable from a failed projection.
`SignalSettingsClient::observeSnapshot` suspends the panel on projection
failure rather than feeding a partial list to deletion/pruning logic. Tests
cover reordered rows, missing references, duplicate IDs and an empty catalog.
The adapter's `synchronize` establishes the session before this call. New epochs
start with defaults; see `save-identity.md` for the polling detector's limits.

## First actual rendered panel validation

After normal game exit and local installation, PID 49812 automatically loaded
the SDK, the SFR mod and the experimental UI bridge. The pinned diagnostic was
absent. On the reloaded test copy, signal 1406.10 displayed all eight declared
checkboxes. Clicking Active persisted across selecting the built-in 1407.2 and
returning; that built-in signal showed no SFR controls. The texture bridge
reported an active override at index 11, matching the mod's unknown indication.
Unchecking Active eventually produced index 0. An intermediate sample had no
override, so observation/render lease continuity still needs investigation.
No complete BAL sequence or persistence-across-load claim follows from this test.
