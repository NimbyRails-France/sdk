# Native mod options and keyboard assignments (Windows 1.19.10)

Status: the corrected layout is confirmed in the running game. After cleanup
and a new launch, Options > NRF Hub directly shows Shortcuts with BB Timechange's
Ctrl+Shift+R assignment preserved. The user also reports BB Timechange working.
Native-solver tests cover the geometry; the remaining conflict, capture and
lifecycle scenarios are listed below and are not claimed as verified.
This is internal research, not the mod authoring contract.

Qualified executable SHA-256:
`fff49ac21720abfc824c2b4f68b862727630eb0db71cfe1f9ea8f685d0db10ae`.
All addresses below are RVAs for this executable. Resident Bootstrap checks
full identity; each new hook checks its first 16 bytes before creation.
Ghidra 12.1.3 exports used the repository's `InspectConstruction.java` and
`FindNativeStrings.java` on the read-only `build/sdk-data-ghidra` project.
Evidence is retained in `.validation/sdk-native-options-20261008` in the workspace.

## Dedicated native NRF Hub tab

`0x5dc920` dispatches native Options. Owner `+0x10c` selects one of seven tabs
(0–6): Graphics, Interface, Performance, Keybinds, Autosave, Alerts, Uploader.
Window ID is `##mm_options`. Assigning 7 cannot add a tab: the dispatcher draws
no window for an index outside 0–6. The SDK never stores or uses index 7.
The dispatcher initializes Close at owner `+0x110`, returns its value, and
handles the uploader's file-dialog request at `+0x111` after rendering.

The Interface body is `0x5d0d70(capture, Declare*)`. Caller `0x5ef790` opens
the outer/right groups, calls it at `0x5efe59`, then closes those groups.
`0x604270` calls the containing body for layout (`0x6045a2`) and interaction
(`0x604b87`). The Interface body closes its own groups and ends in flexible
space; it does not provide scrolling for arbitrary additional settings.

The seven sidebar wrappers are `0x5ef790`, `0x5f0190`, `0x5f0b90`, `0x5f1590`,
`0x5f1f90`, `0x5f2a80`, `0x5f3480`. Each receives four pointers: width, owner,
pending tab, body capture. The buttons write the temporary pending tab, not the
owner's tab. Their enclosing builder (Interface: `0x5ef490`) commits that value
only after the two passes. `native-tab-returns/return-addresses.json` records
all 49 exact button call/return addresses extracted from the qualified executable;
the Interface/Keybinds series also match the independent Ghidra exports.

Two native button detours (`0x55c9c0`, `0x55ef00`) insert `NRF Hub` immediately
after Uploader. They are active only under a thread-local Options dispatcher
scope and at those exact callsites, on the root emitter identified by Graphics
separately for each pass. Other emitters, modal/nested dispatchers and ordinary
buttons are delegated unchanged. Return addresses are captured in the detours
themselves, before entering a shared helper. No vtable is patched. Native button
flag 2 supplies selection highlighting; row geometry matches the native sidebar.

NRF selection lives in SDK memory. When selected, the SDK temporarily routes
the native tab to Interface (1) for the duration of the ORIGINAL dispatcher and
replaces only its body. This preserves Close initialization, return and uploader
tail without reproducing that logic. The native Interface highlight is suppressed
only in this scope, while NRF receives the native selected style. Observed native
button clicks are committed at dispatch exit; otherwise the previous valid
native tab is restored, including on exception. No owner pointer is dereferenced
outside its native callback. Closing, changing owner/native tab or a failed UI
validation leaves NRF selection. The ordinary Interface body is unchanged.

The NRF body shows Interface (boolean/number/choice options) and Raccourcis /
Shortcuts only when the loaded mods declare matching fields. A single category
appears directly, without category navigation buttons; both categories enable
navigation. The classification and selection are frozen with the displayed
snapshot for BOTH passes and committed afterward. If a category disappears,
selection is normalized to the remaining one before validating capture. Fields
are grouped by mod in each category. Each category has
its own scroll key; a vertical content root has automatic height so its full
contents drive the native scrollbar. At layout, the presentation owns a frozen
copy of rows, translations, errors and capture state. Interaction consumes the
same rows; edits affect the next layout. No mod callback or storage IO runs
during rendering. Integer drafts remain SDK-owned until applied.

The internal `OptionsBodyLayout` helper freezes the body implementation too:
validation/preparation failure before any SDK declaration selects the native
body for both passes. Once SDK layout emission starts, a later failed bind or
emitter/capture mismatch skips the body and leaves NRF selection; it never emits
native Interface widgets against SDK layout nodes. Preparation can fail before
emission without forcing an incompatible partial tree. The helper is independent
of game memory and exercised by the keybinding regression target.

Leaving NRF or its Shortcuts category cancels capture before the frame's SDK
input handling; already captured events cannot become ordinary dispatch events.
Current registry identity/type is rechecked before the event pump and dispatch,
so removing a mod/shortcut cancels capture even when other mods remain loaded.
Ownership of already swallowed keydowns is kept until their releases arrive.
`navigation-entry-bytes.json` contains the three additional validated prologues.

## Body geometry correction

The user's first dedicated-tab screenshot confirms the NRF Hub sidebar entry,
but shows the category buttons too low, an empty right-hand area and a narrow
scrollbar. This is a failed layout acceptance, not a successful complete UI test.

Three declaration errors were confirmed against the qualified binary. Native
label layout (`0x55ca70`) measures text width only when declared width is negative;
the reset declaration is zero, so the heading outside an automatic filled row
received no width. The scroll viewport previously declared height alone: its
child's minimum width does not set the parent viewport width or clip. Finally,
column flow 3 centers children when spare space exists; flag `0x8` selects start
justification (`0xb`). Horizontal fill (`0xa0`) alone does not anchor a column at
the top (`0x40`). Declaration reset and scaling are visible in `0x55ba40` and
`0x55c4b0`; the solver applies these rules in `0x81be40` and `0x81c160`.

The NRF body now declares a 400-by-700 logical-unit column, using the Interface
shell's body width and native outer-box height, with flow `0xb` and alignment
`0xe0`. The title explicitly fills its row. `SignalUi::scroll` explicitly fills
the viewport horizontally before opening its native group. The 560-unit viewport
uses the available inner width, without a 400-pixel child minimum that would
otherwise ignore the native scrollbar gutter and cause horizontal overflow.
When only one category is present, its two navigation rows are omitted and the
viewport grows to 628 logical units; the column height stays 700. This choice is
frozen with the category snapshot so both native passes emit the same tree.

`native_mod_options_geometry` maps the recognized executable with
`DONT_RESOLVE_DLL_REFERENCES` and calls only layout solver `0x55c690`; it does not
run the game entry point, create a window or send input. The regression models
the native 620-by-700 outer row, 200-unit sidebar, 20-unit margin, filled right
body and nested NRF controls. It reproduces the old zero-width heading/viewport,
then checks the corrected dimensions, vertical order, bounds and start
justification at scales 1, 1.25 and 1.5. At scale 1 the NRF column is
`[220,0,400,700]`, the title is `[230,8,380,24]`, and the viewport is
`[220,104,400,560]`, ending at 664 within the 700-unit body.

An independent probe confirms the same nested geometry at all three scales:
`native-layout-probe.py` and `native-layout-probe-full.json`; the earlier baseline
is retained in `native-layout-probe.json`. These checks establish the corrected
layout contract, but do not reproduce the screenshot's large vertical offset
pixel for pixel. A later user-operated screenshot confirms that the corrected
body renders its controls. This is visual evidence for that configuration, not
qualification of every window size, language or input transition.

## Current game assignments and labels

`0x7255f0` initializes default configuration. `0x2ed700` returns the active tree
at `0xb5d940`; reload flag `0xb5b0f8` first makes it copy defaults and read
`config.txt`. `0x2edcf0` replaces the active tree, marks reload and writes the
file. Initialization byte `0xb819d0` must be true. Reading the file alone is
not authoritative for a live game.

The 40-byte tree header contains extremal nodes at `+0/+8`, root at `+0x10`
and count at `+0x20`. Each 96-byte node contains children at `+0/+8`, parent
at `+0x10`, key MSVC string at `+0x20`, value MSVC string at `+0x40`.
Strings have 16-byte inline storage, size `+0x10`, capacity `+0x18`.
`0x2ee480/0x2ee560` prove allocation/parent links; no foreign C++ container
ownership crosses the SDK boundary.

Keys are `kb1_<action>` and `kb2_<action>`, values decimal SDL_Keycodes; zero
is unassigned. The reader bounds nodes/strings, rejects cycles, invalid parents,
replacement and pending reload. Failure means unknown, never an empty conflict
table. It runs between native callbacks on the UI thread: replacement checks
do not promise atomic arbitrary cross-thread observation.

`0x5db050` supplies native action labels, using `kb_<action>` localization
keys. `0x2d82e0(key, fallback)` resolves the active game language. Its borrowed
bytes are immediately copied in bounded chunks. Bindings and language/owner
identity are compared with the previous snapshot, so unchanged values do not
rebuild labels or registry conflicts. Assignments are read fresh before each
assignment and dispatch batch. Display-only checks are limited to four per
second, with a fresh check on entering the page. Inter-mod conflicts retain
owner/field identities so labels are translated through the conflicting mod's
catalogue rather than concatenating raw translation references.

`0x726750` compares only event `+0x38` to the two configured values; it does
not test Ctrl/Alt/Shift. Dispatcher `0x6830d0` calls it directly for speed and
mode actions. Each native key therefore reserves all eight modifier combinations.
The same dispatcher directly handles Escape and F11/G on release. F11 toggles
the Debug panel (`0x671d60`); G selects a conditional speed/status panel
(`0x677b40`). These are reserved too. Editor dispatcher `0x786320` separately
tests the Ctrl flag at its frame `+0xfa` and key codes Z/C/X/V/B. It invokes
undo/copy/cut/paste/duplicate; additional Shift/Alt do not disable those branches.
Their Ctrl combinations are reserved too. Native tips in `0x7af120` provide
localized labels (`editor_tip_undo/copy/paste/dupe`); cut has an SDK FR/EN label.
This is not an exhaustive claim about every context-specific editor command.
In particular Ctrl-S is not inferred from convention without dispatch evidence.

## Keyboard and focus

Pump `0x2d4040` drains SDL_PollEvent (IAT `0x9ab8f0`). Its 112-byte native
events use kinds 8/9 for key down/up, keycode `+0x38`, repeat `+0x3c`.
Frame modifiers `+0x61..0x63` are sampled by `0x2d3df0` before the pump and
may be one frame old. The adapter observes SDL events only within a thread-local
scope of that pump, copying window ID, keycode, modifiers and repeat into a
fixed 256-entry buffer. Overflow discards the SDK batch. No OS hotkey is registered.

SDL3 defines per-event modifier/repeat state in
[SDL_KeyboardEvent](https://wiki.libsdl.org/SDL3/SDL_KeyboardEvent) and
[SDL_Keymod](https://wiki.libsdl.org/SDL3/SDL_Keymod). The adapter checks the
imported poll address against its owner's SDL_PollEvent export and resolves
window/text queries from the same module. The installed proxy forwards these exports.

Frame `0x7272d0(shell, rawFrame)` runs after the pump. Primary SDL window is
rawFrame `+0x78`. `0x731d80` manages native text input via shell `+0x64e0`
(lease) and `+0x64f0` (active flag); the adapter also checks
[SDL_TextInputActive](https://wiki.libsdl.org/SDL3/SDL_TextInputActive).
Foreign-window/process input, repeats and unsupported GUI/AltGr chords do not dispatch.

Text focus is sampled before and after the original frame. Either observation
blocks dispatch: Enter used to submit a text field cannot become a mod action
merely because the field loses focus during that frame. Fresh assignments after
the original frame include changes made by native controls during that frame.

Capture begins only after Assign is clicked, ignoring earlier queued keys. The
poll adapter withholds assignable keyboard down events only in the native pump's
thread-local scope, while NRF Hub > Shortcuts was visible on the preceding frame, capture
is active, the primary window is foreground and text input is inactive. Mouse,
window, modifier-only and other SDL events keep their native route. Escape
cancels capture and Backspace clears the setting, even if native bindings are
temporarily unavailable. Leaving this category or losing focus cancels capture.

The SDK owns each withheld key until its release, including repeats and releases
after capture completes. A fixed table uses physical scancodes (with a bounded
logical-key fallback for unknown scancodes), so a modifier/layout change does
not leak the release to a native action such as F11. After the pump drains,
[SDL_GetKeyboardState](https://wiki.libsdl.org/SDL3/SDL_GetKeyboardState) retires
released physical keys; loss of focus clears ownership. At most 256 events are
withheld per pump: further events remain queued and ownership is retained until
the remaining events drain. Events withheld for capture never later dispatch a
mod action if native navigation cancels that capture during the frame.

The pure `CapturedKeys` helper is covered in `native_game_keybindings`: success
followed by repeat/up, foreign window, already-held key, modifiers, changed keycode,
focus loss, physical-state cleanup, unknown scancodes and primary-window replacement.

## Lifecycle and live acceptance

Validation on 2026-10-08: the Windows bridge builds successfully and 17 targeted
C++ regression tests pass, including capture routing, native assignments,
options persistence, isolated RPC, client revision handling and the existing
signal UI contracts. Kotlin contract/export checks and the bilingual wiki checks
also pass. After the dedicated-tab change, the bridge/keybinding targets rebuilt
without warnings and all 17 targeted C++ tests passed again (`nrf-tab-build.log`,
`nrf-tab-tests.log`). The 49 navigation return addresses additionally passed an
independent static comparison against the qualified PE's real CALL instructions
(`native-navigation-static-audit.json`). These checks do not establish live UI
geometry, native input behavior or a successful manual acceptance session.
The earlier isolated test package was loaded successfully at the game main
menu. That process predates the final capture correction and dedicated-tab
integration. A later user-operated screenshot confirms the dedicated tab but
reveals the body geometry failure described above. After the correction, the
bridge builds and all 18 targeted C++ tests pass, including the new geometry test
against the recognized executable at all three scales (`nrf-layout-build.log`,
`nrf-layout-tests.log`). A later user-operated screenshot confirms the corrected
visible layout, and the user reports BB Timechange working. These observations
do not establish the pending conflict, capture or persistence scenarios below.
Validation-only mods have been removed from the user's profile as requested;
they are fixtures, not default SDK preferences. The user subsequently authorized
agent-operated retesting and PC control ("re teste et prend main sur pc"),
superseding the earlier request for manual-only checks. The current cleanup and
installation through the existing Hub are authorized and complete.

After the conditional-category simplification, the bridge builds and all 18
targeted tests pass, including native geometry with the qualified executable
(`nrf-shortcuts-clean-build.log`, `nrf-shortcuts-clean-tests.log`). Bridge SHA-256:
`9b6637cb8b17d5f766bcfce1bf87bc62776c53e8750e4e11ea48fab733192979`.
These results do not mark the remaining live conflict scenarios as verified.

Final live verification on 2026-10-09: Hub activation is COMMITTED, with no
remaining activation journal, and the installed bridge matches the SHA-256 above.
The active development profile contains SDK, AB, BA, BB and TCO, with no validation
fixture; both fixture junctions are absent. Inspection of the native window state
after the user opened Options > NRF Hub confirms the direct Raccourcis heading,
without category-navigation buttons. The only displayed mod is BB Timechange,
with Date et heure, Ctrl+Shift+R, Effacer and Rétablir controls. The panel is
positioned at the top with the corrected geometry. After the new launch, the
Timechange preference file retains its exact pre-launch hash and the same
Ctrl+Shift+R assignment is displayed. This verifies persistence of that setting
in this configuration; no additional live conflict test was performed.

The active-tree reader was also exercised read-only against that live process:
37 nonzero assignments, 20/20 identical successful reads, with no failed memory
read. External reads averaged 0.197 ms (307 memory reads / 18,265 bytes each).
This is a bounded diagnostic sample, not a whole-SDK latency measurement. `T`
was assigned to `track_station`; the SDK's conservative key reservation therefore
rejects `Ctrl+Shift+T` too. This does not assert that every native editor mode
dispatches that modified combination. F9 was absent from the active table.

The initial launch delay was Steam waiting for a Cloud synchronization warning,
before it created the game process. This was not evidence of a SDK startup crash.

`installUi` creates and queues seven hooks, never enabling a subset. Bootstrap
applies its complete set and pins the bridge. `cleanupHooks` removes this adapter's
hooks on failure before activation, or after callbacks are disabled/drained.

Required live checks before completion:

- The NRF Hub entry appears in each of the seven native Options pages; Close,
  Uploader and returning to native Interface retain their normal behavior.
- Direct Shortcuts or Interface display when only that category is populated;
  category selection/highlighting when both are populated, and independent
  scrolling at different window sizes/scales; typed numbers, choices, many mods
  and removal between passes.
- French/English and live language changes; persisted settings after restart and
  visible storage-error handling.
- User-edited first/second native bindings and cleared assignments; conflict labels;
  native reassignment after registration; unknown/stale data rejects assignment.
- F2/F11/Escape during capture do not trigger an unwanted game action before the
  conflict diagnostic; their releases/repeats remain withheld after completion.
- No mod action while typing in native fields, during repeat or after losing focus;
  capture cancellation on navigation and no code from a slow/failed mod on UI/input.
