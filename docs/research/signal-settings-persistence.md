# C++ signal settings persistence

`nimby/detail/signal_settings_file.hpp` implements the SDK sidecar codec and
file operations for `SignalSettingsStore::SavedSettings`. No game save is
modified. Automatic restoration and writes are now wired through SignalSettingsClient.

The format starts with `NIMBY-SIGNAL-SETTINGS 1`, a CRC32 of the payload, then
quoted session/panel IDs and each full signal ID with named boolean values.
The checksum detects accidental corruption; it is not authentication. Reads
are limited to 16 MiB, 16384 signals and 64 fields per signal. Duplicate IDs,
duplicate field names, non-boolean values and trailing data are rejected.
A missing file returns absence; a corrupt existing file raises an error.

Writes validate before touching disk, flush a sibling temporary file, then
replace the destination with Windows `MoveFileExW` using replacement and
write-through flags. Failed replacement cleans up the temporary file and
retains the original destination. This does not claim transactionality with
the native game save or protection against every power-loss/filesystem failure.

The future persistence worker must serialize writes, choose an SDK-owned path
under its settings directory, and pair it with validated save/revision identity.
It must report write failures and must not reinterpret a corrupt file as default
settings. `beginSession` independently rejects import for a different panel or
session. Save As and rollback semantics remain pending native lifecycle work.

`cpp_signal_settings_file` passes roundtrip, truncation/corruption, invalid
duplicate identity, blocked replacement and successful subsequent replacement
tests. Only temporary files are used by these tests; no native saves were changed.

## Resident bridge transport (implemented)

The optional C exports `NimbyUi_ExportV1` and `NimbyUi_BeginSavedV1` now move
bounded codec payloads between the resident panel and the SDK worker. No STL
objects or native pointers cross the DLL boundary. Export checks the expected
session under the store lock; stale-session exports are rejected. A short
buffer is untouched and reports the required size. The client retries bounded
size changes caused by concurrent checkbox edits.

Import validates checksum, world/revision identity supplied by the caller and
panel identity before replacing any values. It creates a new session token and
requires a new complete catalog before values or clicks become available.
Failed import retains the existing session and values. Export/import do not
read or write files, guess a save filename, or enable automatic restoration.
The native save/revision coordinator is still required.

Tests cover edited values across the resident C ABI and DLL client, malformed
payloads, wrong identity, short buffers, old tokens, removal, and re-observation.

## World profiles (2026-09-20)

The SDK worker now restores and checkpoints edits in
`%LOCALAPPDATA%/NimbyRailsFrance/signal-settings/<world-id>/<hex-panel-id>.settings`.
It polls the resident export at most every 250 ms and writes only changed data;
normal adapter stop performs a final checkpoint. Forced process termination
within that interval may lose the last click. Import errors suspend the panel
and preserve the corrupt file rather than overwriting it with defaults.

This is explicitly a **world profile**, not a native save-revision sidecar.
Save As and older saves of the same world share the latest checkbox edits.
The native world identifier, full signal ID (including generation), panel ID,
and complete matching texture catalog isolate profiles and prune absent signals.
Independently edited branches with identical world/signal IDs share settings;
native save/revision transactions remain unimplemented. No native save is modified.

The real-DLL client test covers restoring edited values after reconnect, separate
worlds, absent signals, unchanged-file timestamps and preserving corrupt files.
