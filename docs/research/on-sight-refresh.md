# Windows restricted movement: physical refresh

Local correction, 2026-09-28. Qualified game SHA-256:
`fff49ac21720abfc824c2b4f68b862727630eb0db71cfe1f9ea8f685d0db10ae`.

## Symptom and contract

Live restricted trains repeatedly reached the native idle floor despite roughly
200 m of clear track and a higher ceiling supplied by the mod. Physical views
expired after 250 ms, while the native lookahead refreshed less frequently.
That freshness interval is not a maximum duration for traversing a block.
Restricted-mode memory remains effective until the measured exit-signal passage.

The Windows adapter now retains geometry from the native traversal and queries
physical occupation again before each restricted integration step. It never
refreshes a timestamp without an actual query. No new signal speed is introduced.

## Native scope

- `0x448710` is the native motion step. The call at `0x44b367` passes context,
  train, Motion, service, extra mass (double), and tick-budget pointer; result is
  a 32-bit native status. The wrapper forwards all six arguments and its result.
- The context constructor `0x442170` assigns Network at `+8`, simulation at
  `+0x18`, and the worker's physical-occupation map at `+0x68`.
- `0x448d6e` calls the existing integrator `0x378600` with Network, not this
  worker context. Reading Network+0x68 as an occupation map would be incorrect.
- A thread-local scope around the motion step exposes its current context to
  the integration hook. It is restored on return, including nested calls. No
  worker context or occupation-map pointer survives that scope in train memory.
- The integration verifies Motion, Path pointer, Network and simulation before
  calling the existing occupation predicate `0x4582d0` with its own result byte.
  Conflicting-track checks and movement remain native. Restricted longitudinal
  admission is described in [restricted entry](restricted-entry.md).

## Geometry lifetime

`platform/windows/runtime/physical_route.h` stores only ranges returned by the
native scan or a granted entry traversal. It validates the complete Motion ID,
Drive presence, Path header and full path-ID contents (including in-place edits),
track IDs, metrics, head position, direction and displacement. Changed data
invalidates the prefix; it does not establish another branch. Queries are bounded
by the existing simulated visibility and the captured geometry. A short known
route is never extended by an assumed clear continuation.

Occupation is always taken from the current native worker frame. The Path ID
vector is an identity check only; it is not interpreted as ordered geometry or
permission. Unavailable rules and unavailable physical coverage remain restrictive.

## Verification

`automatic_driving_permission_bridge` exercises the actual Windows wrappers with
controlled native callbacks: changing obstacles between scans, stale views,
missing/mismatched context, map replacement, scan recovery, stop proof, native
longitudinal admission, crossing/controller refusals and explicit train commands.

`automatic_driving_physical_route` covers a 5 km segment, arbitrarily delayed
refreshes, both directions, multiple ranges, bounded coverage, path edits,
identity reuse, geometry changes, unreadable data and route resets. These are
deterministic fixtures; they do not alone qualify a game ABI or establish live
performance. Static disassembly evidence is retained locally under
`.validation/on-sight-refresh/native.asm` in the development workspace.
