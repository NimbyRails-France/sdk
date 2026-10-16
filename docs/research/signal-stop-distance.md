# Configurable signal stopping distance — 2026-10-09

This is an internal Windows qualification, not a mod API or an in-game test.
The shared SDK preference is 3 m by default and accepts integer values from
0 to 100 m. Portable helpers accept finite values in the same interval and
fall back to 3 m for invalid input. The separate on-sight obstacle margin
remains 5 m. Mods continue to supply numeric signal instructions.

## Provenance

Qualified executable SHA-256:
`fff49ac21720abfc824c2b4f68b862727630eb0db71cfe1f9ea8f685d0db10ae`.
Image base: `0x140000000`. Read-only Ghidra exports, without native calls or a
running game, are in the local validation directory
`.validation/signal-interlocking-20261009/signal-stop-native/exports/`.
Relevant files are `140377dd0.c/.asm`, `140378600.c/.asm`,
`140448710.c/.asm`, `140448450.c/.asm`, and `constants.txt`.
The projector's first 16 bytes were also checked directly in the PE image:
`4c 89 4c 24 20 4c 89 44 24 18 48 89 54 24 10 48`.
Bootstrap validates the recognized hash and all hook entry bytes.

| RVA | Qualified role |
| --- | --- |
| `0x377dd0` | Project a distance along the selected native Path; update Presence and return Range. |
| `0x378600` | Integrate motion; projector calls at `0x3786f7` and `0x378e08`. |
| `0x448710` | Motion caller; subtract returned elapsed ticks once from its remaining budget. |
| `0x448450` | Account travelled distance and elapsed seconds, including stationary time costs. |

## Private layouts and stopping bound

The copied Presence prefix is 40 bytes: path index at `+0`, full track ID at
`+8`, fraction at `+0x10`, direction/orientation bytes at `+0x18/+0x19`, and
route head metres at `+0x20`. Real Presence additionally contains speed in m/s
at `+0x28` and pending reversal/overlap metres at `+0x30`.
Range is 48 bytes: track pointer, from/to fractions, length in metres, kind
at `+0x20`, and signal pointer at `+0x28`. The integrator's result appends
elapsed simulation ticks at `+0x30`.

The integrator's first projection uses a private stack copy of the Presence
prefix. Its requested distance is positive pending overlap or the finite
constant `99,999,999,999.0` at VA `0x140aac350`. If integrated movement reaches
that projected boundary, native code copies the projected prefix to real
Presence. Otherwise its second projection advances real Presence by the
integrated distance. Consequently, an input velocity ceiling alone is not a
physical travel bound.

For a qualified SDK zero-speed signal target, the wrapper records the absolute
stop head (`signal position - preference`, clamped to the current head) and
bounds both existing projector requests by the remaining route metres. It
adds no native query. Native Range classification and branch handling remain
intact: kind 8 is a partial section; kind 0 is invalid, kind 1 destination,
kinds 2–5 transitions, kind 6 signal, and kind 7 another event. At zero distance
inside a section, native kind 8 has zero length and an unchanged route head.
A real event at that exact point retains its native kind and timing.

## Idle timing and isolation

Native integration enforces a 0.01 m/s floor (VA `0x140aabb00`) per 0.01 s
tick. A zero-speed target without a distance bound can therefore creep.
At a zero-length bound it instead returns after one tick. The caller would
repeat that work for every remaining tick.

The wrapper consumes the caller's entire remaining budget in the result only
after native integration has actually produced a clipped kind-8 idle result:
zero length, equal fractions, unchanged full train/track/index/direction/head,
no signal event, no pending overlap, speed at the qualified idle floor, and
positive elapsed ticks within the original budget. It changes only the
caller-owned elapsed result field. No native Motion position or velocity is
written by the SDK. A moving, invalid, reversing, or event result receives no
extension. Caller `0x448710` subtracts elapsed once and accounts
`elapsed * 0.01` seconds with zero travelled distance, preserving stationary
time and its costs rather than multiplying simulation time.

The fence uses thread-local scope, exact pool/Path identity, and the captured
Presence cursor. Nested integrators hide an outer fence even on early native
fallbacks. No fence survives its integration. Unknown cursors retain native
projection and the conservative numeric target; this qualification does not
make unknown geometry trustworthy. Busy/missing controller states and the
separate obstacle handling do not acquire a fabricated signal fence.

The true signal position remains the stop-proof, passage, release and
restricted-entry boundary. A measured standstill 3 m before it may satisfy
`STOP_THEN_PROCEED`; permission alone never counts as passage. Reopening
removes the zero-speed fence. The proof is bound to the exact instruction and
signal position and survives forward approach across successive integrations.
A changed rule, displaced signal, backward head or stale physical admission
invalidates an admitted proof. A native refusal also removes its admission.
An initial measured but not yet admitted standstill can remain while waiting
for its first valid physical permission; it authorizes no displacement.
New stationary measurements after refusal likewise remain unadmitted.

Before the real source is crossed, the captured native scan supplies the exact
head-to-source sections through `SignalRoute.copyPrefix`, including a qualified
zero-distance source point. A missing source is unknown, even if the game reports
the train waiting: that callback can measure standstill but cannot manufacture
the source's position. Path IDs, attachments, topology, source incarnation and
head are reread around physical occupation queries. No inferred gap is added to
free distance. Occupation uses the current callback's live native context and
private output byte; no native context pointer is retained between calls.
Ordinary native ranges before the source retain their original checks; only
the first qualified longitudinal range after the marker substitutes restricted
occupation/reservation admission. The marker does not count the prefix twice.

Private `entryPhysicalRoute` holds admission geometry separately from the
scanner's unfinished `physicalRoute`. This accepts the game's native lookahead
permission callback without creating passage/boundary evidence, or having the
next scanner range append a second copy of its downstream geometry. Each
pre-entry integration uses `SignalRoute.withPrefix` to reread its topology
around current occupation via the live
simulation worker before using on-sight speed and the separate 5 m obstacle
margin. Subsequent scans rebuild physical geometry for an admitted approach.
Only measured crossing of the real source starts retained on-sight mode and
can transfer admission geometry to it. Source retirement, route revision,
recycled motion identity and refusal discard the admission geometry.
Private `DrivingPreferences` shares the accepted
option between resident bridge DLLs using lock-free primitive fields, with no
file access, new lock, or option lookup syscall during motion integration.

## Validation and limits

Portable planner/controller fixtures cover margins, invalid values, stale
instructions, retained/announced targets, stop proof and the independent
obstacle margin. `automatic_driving_permission_bridge` exercises production
wrappers with controlled callbacks: 0/3/12/100 m, large attempted movement,
kind-8 idle budget consumption, both directions, exact native events,
overlap, reopening, stop permission at 3 m, progressive departure across
3/100 m without an intermediate stop, scans with/without a new permission,
first lookahead admission, ordinary pre-marker ranges, a 40 m pre-source
obstacle (35 m target), loss of admission, changed source/topology before or
during a physical query, qualified
zero-gap contact, missing source, unrelated contexts, another thread and nested
integrations. These are offline fixtures with explicit
qualified layouts; they do not validate native addresses by executing the
game. No game launch or live map validation was performed in this work.
