# Save identity investigation

Read-only static evidence for the 1.19.10 binary with SHA256
`fff49ac21720abfc824c2b4f68b862727630eb0db71cfe1f9ea8f685d0db10ae`.
Exports are currently in `reports/cpp-signal-ui/`.

## Located paths

- `.nimbyrails5` references lead to `0x49cb20` and `0x49f830`.
- `0x49d9a0` is a load path: it publishes `Parsing header`, seeks the input
  stream, calls `0x49b990`, checks corruption and save-version compatibility,
  and proceeds to deserialization (`0x4b6700` on one path).
- `0x49b990` reads the header. One branch accepts magic `0x59424d4e`
  (`NMBY` little-endian), reads an initial 20 bytes and, for header variant 2,
  a further `0x550` bytes. It copies numeric fields and bounded name/description
  buffers into the destination. Other formats use a deserializer fallback.
- `0x5e4f40` implements save-name/overwrite dialogs. Its editable file name
  is not evidence of a persistent identity for the currently loaded game.

## What is not established

No field has yet been demonstrated to be a persistent game identifier. The
header contains several unknown numeric fields; none should be treated as a
UUID based only on size or position. Searches for `uuid` find two API-name
strings but no corresponding named symbols or direct string references in
this analysis project. This does not establish absence of a game identifier.

Do not import settings based on the process ID, a current heap address, or the
editable save-name field. They do not establish continuity of one saved game.
The existing store's session token invalidates pending clicks, but its caller
still needs an actual lifecycle transition and a persistent storage identity.

## Next evidence needed

Trace header construction on save and game construction/deserialization to
assign meanings to the numeric fields. Compare read-only metadata from saves
of the same game and from independent games, then locate the matching live
field. Renaming, Save As, loading an older save and overwriting a path need
explicit persistence semantics; no automatic sidecar import is enabled yet.

`tools/FindNativeStrings.java` provides reusable case-insensitive string and
symbol discovery with incoming references. It runs in a read-only Ghidra
project and does not inspect or modify user save contents by itself.

## Writer and Versioning evidence

RTTI for `save_ostream` identifies `0x49f050`. This function serializes
`nimby::sync::saves::Game` through `0x4b5ea0`, then writes the header. The
leading metadata includes simulation date, bank balance, collection counts
and shell state; these are mutable values, not suitable identities.

`0x4b5ea0` explicitly serializes `nimby::model::Versioning` at Game `+0xb58`:
a 32-byte array followed by a vector of 32-byte arrays at `+0xb78`.
Deserializer `0x49b1d0` reads the same structure. The header writer copies
the first array to file offset `0x4d8` and the last vector entry (or zeros if
empty) to `0x4f8`. The header alone does not expose the full history.

A read-only comparison of 56 local `.nimbyrails5` files found 56 variant-2
headers, 11 distinct first arrays and one distinct last-entry array. Grouping
files provisionally by filename with ` Autosave N` removed yielded 12 groups
with multiple files: 11 groups had one first-array value, one group had two.
Filename grouping is a heuristic, not proof of shared game origin. No files
were written and no game save command was issued.

This supports investigating Versioning as a lineage key, but does not prove
which events change it or whether Save As should inherit settings. Next trace
Versioning generation/mutation and its transfer from live state into Game.
`tools/windows/read-save-versioning.ps1` reproduces bounded header extraction and
rejects unsupported/truncated/changing headers. It is a research utility,
not the SDK's production persistence reader.

The reusable tool reproduced all 56 headers successfully: all first values
were nonzero, and all last-history values were zero. Therefore this sample
cannot validate history traversal or branching behavior.

## Live value located

Game construction at `0x4aafd0` copies the 32-byte value from the object
pointed to by its source session `+0x400`, into Game `+0xb58`, and copies
the adjacent history vector. Caller `0x4e47c0` subsequently passes that Game
to `save_ostream`; the local-save path goes through `0x4abc50` as well.

The first live candidate tested was DB `+0xa48`: a 0x38-byte Versioning
would fit immediately before the already verified Rules object at `+0xa80`.
`nimby_versioning_probe` was built and run against PID 10236. It verified
the executable identity, resolved the known live roots, read the 0x38 bytes
twice, and rechecked the roots. It made no writes or target function calls.

Observed first 32 bytes:
`af75d0a8a5c5b0a828ed2ccb318c4ef541f949741b6ae5a3e243efd44219174d`.
That exact value matched 11 of the 56 local save headers. This is direct
live/save correlation, not yet proof of lifecycle semantics. The probe
labels the offset as a candidate and is not used by automatic settings
restoration. Next verify the source-session pointer construction and observe
load/new-game transitions; retain revision-specific behavior for rollback.

## Shared SDK reader

`engine/versioning.h` now owns the bounded read used by the research probe.
It requires a recognized binary and freshly resolved roots, rejects an all-zero
value, validates the history vector (maximum 4096 entries), and rereads both
header and history before checking the roots again. Failure clears the output.
External repeated reads detect some races; they do not provide atomicity or
prove that a load/unload cycle did not reuse the same addresses.

The `versioning_read_guards` test passes for valid empty/nonempty histories,
unrecognized builds, zero values, malformed/oversized vectors, unreadable
history, and changing roots/header/history. The rebuilt probe was run on
PID 10236 and returned the same 32-byte value with `history_count=0`.
Automatic settings session creation/import remains disabled until lifecycle
semantics are established; this reader is an observation primitive only.

## Controlled save-copy observation, 2026-09-20

The user saved the currently open game as
`SFR Etude BAL 2026-09-20 A.nimbyrails5` (11,604,494 bytes, local timestamp
14:16:11). Its header and the live PID 10236 both expose
`af75d0a8a5c5b0a828ed2ccb318c4ef541f949741b6ae5a3e243efd44219174d`,
with an empty live history. This is also the value observed before Save As.
Therefore this value alone cannot distinguish separately named save copies.

After returning to the main menu, the versioning probe returned exit code 6
(observation unavailable). It also returned 6 during the copy's loading phase,
including native station validation. These observations support invalidating
the active settings session on an observed unload; they do not guarantee a
polling worker will observe every short transition.

Loading that exact named copy completed successfully. The UI returned to the
same test tracks and signals; the probe again returned exit code 0, the same
32-byte value and `history_count=0`. Thus this controlled Save As + unload +
reload cycle preserves the value. New-game uniqueness and revision/rollback
semantics remain unproven. The game was left running the test copy.

## Snapshot session integration

Snapshots now expose `GameSession { generation, worldId }`. Versioning is read
before and after the native capture, with matching roots/value/history required
for this optional metadata. An observed metadata loss, different roots or world,
history change, or backwards simulation ticks changes the generation shared by
simultaneously open readers of the same live process in this SDK registry.
A later reader joins that detector, so cooperating mods compare the same epoch.
Closing every reader discards it; reconnections must invalidate old state.
The targeted driving observation counter remains connection-local.
Missing clock observations preserve the previous tick guard.
Pointers are not exposed or used as persistent keys.

The mod adapter uses this generation to begin the resident panel session and
feeds the complete texture-set catalog before invoking the mod's observation
callback. New sessions currently start with defaults; disk restoration is not
enabled. This polling detector does not prove detection of an unload/reload
that reuses all roots between samples with nondecreasing ticks. An exact native
load notification and revision-aware persistence remain outstanding.

Live read-only check on PID 10236 using `nimby_signal_settings_probe`: the saved
test copy reported the expected world value, generation 1, 25,972 signals,
25,972 texture rows, zero unresolved rows, two SFR signals and a complete
settings catalog. This validates real observation/projection, not checkbox
rendering or automatic BAL actuation.
