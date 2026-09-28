# Windows: restart after a permissive stop

Local correction, 2026-09-28; no release or changelog publication. Qualified game
SHA-256: `fff49ac21720abfc824c2b4f68b862727630eb0db71cfe1f9ea8f685d0db10ae`.

## Evidence and scope

Live permission samples contained state 45: the required stop was recorded,
the mod instruction was eligible, and physical geometry was available, but
the native permission was false. These samples did not identify the individual
native refusal. The original reported train is no longer available to reproduce.

The adapter previously replaced only longitudinal physical-occupation refusal.
A native exclusive reservation could therefore still block the same on-sight
entry. A deterministic native-wrapper fixture reproduces this second gate:
stopped train, explicit permission, leader farther ahead, clear entry prefix,
and a conflicting longitudinal reservation. It must permit restricted departure.
This establishes a covered defect, not proof that every live refusal has that cause.

## Permission ownership

`ON_SIGHT` is explicit mod authority to enter using physical clearance on the
followed route. `STOP_THEN_PROCEED` additionally requires measured standstill at
this signal. The mod chooses the entry policy and both speeds; the SDK does not
infer a railway indication from colour, select a national speed, or clear a
reservation table. A plain `STOP` remains absolute.

On each followed range, Windows queries occupation and reservation through the
native predicates with a private result byte. Longitudinal reservation refusal
is replaced for this request only. Physical occupation still bounds the clear
prefix and braking; an obstacle inside the stopping margin denies departure.
Fresh rules, verified geometry and the final native permission are still required.

Crossing-track occupation/reservations and native controller ownership retain
their refusal. The exception consumes exactly the first matching longitudinal
call, after the physical query; repeated identical calls, other contexts, other
Motions, missing observations and unscoped calls receive no exception. This does
not classify reservation owners by direction or resolve opposing-route deadlocks:
the mod must only authorize the intended restricted movements.

Approval alone neither counts as passage nor activates retained restricted mode.
Measured head passage does that. Movement then uses fresh occupation queried
before each integration; the mode ends at the next signal, without a time limit.

## Qualified native calls

- `0x458560`: followed-range occupation call at `0x4585ea`, reservation call at
  `0x458607`, followed by crossing checks (`0x4586a8`, `0x4586bb`) and controller
  ownership (`0x4586cd` onwards). Context offsets `+24` and `+32` point to the two
  predicate contexts. Their layout is map, pointer to Motion pointer, result byte.
- `0x458460`: reservation predicate, four arguments (context, track, from, to),
  byte result. It retains its own native lookup/locking. The wrapper never reads
  or reconstructs reservation-map internals. Its first 16 bytes are checked at
  bootstrap, in addition to the executable hash, before installing the hook.
- `0x451b60`: permission returns its combined result byte at `0x451c7a`. It is
  never globally forced to true; unrelated native denials survive.

Telemetry V4 permission-state bit 128 records a replaced longitudinal reservation
refusal, independently of final approval. Existing record layout is unchanged.

## Verification

`automatic_driving_permission_bridge` executes the actual wrappers with controlled
native callbacks: loaded waiting train, stop proof, longitudinal conflict,
crossing occupation and reservations, controller refusal, repeated queries,
wrong Motion, missing physical query, close obstacle, invalid geometry, stale
publication, absolute stop and unscoped native behaviour. Physical-route tests
separately cover fresh obstacles during movement and both travel directions.

These fixtures do not replace validation of the rebuilt DLL in a new game process.
The running process cannot acquire the replacement by rebuilding files on disk.
