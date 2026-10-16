# Read-only native track admission conflicts

Qualified target: Windows 1.19 research profile, executable SHA-256
`fff49ac21720abfc824c2b4f68b862727630eb0db71cfe1f9ea8f685d0db10ae`.
This document concerns internal SDK observation. It is not a mod API or a
replacement for native admission, reservations, occupation or script gates.

The native admission callback at RVA `0x458560` iterates the vector at
`Track+0xF8`/`+0x100`, in records of `0x30` bytes. The serialization routine at
RVA `0x30FC40` independently confirms the record fields. The other neighbouring
lists at `+0xE0`, `+0x110` and `+0x128` have different roles; the decoder does not
combine them or infer permission from them.

| Record offset | Qualified interpretation |
| --- | --- |
| `+0x00` | Full other Track ID, including incarnation; zero is explicitly skipped |
| `+0x18`, `+0x1C` | Current track's canonical conflict interval, float fractions |
| `+0x20`, `+0x24` | Other track's conflict interval, float fractions |

The callback checks overlap of the current range, resolves the other full ID
through lookup RVA `0x32C420`, and queries its occupation at `0x4586A8` and
reservation at `0x4586BB` using both interval endpoints. Both other-track
predicates normalize direction. The decoder therefore requires an ordered
current interval, normalizes the other interval, and validates finite fractions
in `[0,1]`. Other fields remain unexposed because their semantics are unnecessary
and have not been fully qualified.

`engine/track_conflict_decode.h` performs no native call, pointer dereference or
target write. Callers supply safe memory reads, a qualified source address and a
full-ID validator. It compares two vector copies and rereads the source header.
The process wrapper additionally verifies the bounded pool descriptor/table,
source/target incarnations and live roots; the ABI verifies world/versioning
before and after capture. These are bounded non-atomic observations, not a
transaction with the game. Central admission must use a valid current route and
recheck relevant live conditions at its own controlled decision point.

Limits are 4096 requested tracks, 4096 stored entries per source vector and
131072 stored entries per batch. Entries consume budget even if tombstones or
later invalid. No whole-map traversal is performed. The target pool's metadata
is shared across the batch and referenced IDs are cached only for that capture.

A complete empty vector is a known empty conflict list. Missing IDs, unreadable
storage, stale references, malformed fractions, concurrent changes and exceeded
budgets produce an unavailable list with a specific reason. An individual fault
does not invalidate unrelated readable lists. A changed pool/root or unqualified
profile makes the whole capture unavailable. Linux is intentionally unavailable
until its downstream conflict layout is independently qualified.

Synthetic tests cover directed intervals, unknown versus empty, isolation,
recycled IDs, unstable storage, descriptor/content/pool changes, local and global
budgets, ABI buffers, session generations, immutable views and retained-snapshot
quotas. Qualification artifacts were generated read-only with Ghidra in
`.validation/signal-interlocking-20261009/native-permission`; no game launch or
native table mutation was used for this work.

The pure `engine/route_resources.h` builder consumes qualified travelled
portions and complete conflict lists. It protects normalized own intervals and
the full other intervals of directly overlapping F8 records, with inclusive
endpoint contact. It requires every travelled track's full identity and list;
it does not recursively expand referenced tracks or infer crossings from shared
coordinates. Own intervals are merged before lookup, each relevant list is read
once, and intersection uses binary search rather than global route pairs.

The uniform interval-alias model can conservatively exclude two independent
routes that alias overlapping ranges of the same third track. A partial own
overlap also claims the complete other range, because projecting a smaller range
would require additional qualified geometric evidence. These exclusions are
explicitly tested. Conversely, a crossing absent from the qualified F8 list is
not proved by this builder; native admission remains separate until the full
central-control replacement has been qualified.

## Positive crossing refusal observation — local addition, 11 October

The bridge copies a specific refusal from the pinned calls inside `0x458560`:
occupation return `0x4586AD` and reservation return `0x4586C0`. The longitudinal
calls at `0x4585EF` and `0x45860C` remain distinct. A generic refusal from
`0x451B60` cannot identify a crossing, because it may concern a longitudinal
block where BAL permits restricted movement after a stop.

The observation requires the selected train and source signal, a qualified
source marker and matching native context, complete track identities and
bounded valid fractions. It records the original refusal without changing
any permission. Timestamp, route and source qualification still apply when
the consumer reads it. A positive crossing reason can remain restrictive
when downstream block geometry is unavailable; it does not turn that geometry
into a known route or a clear block.

`Observation.routeConflict` exposes `CrossingOccupied` or `CrossingReserved`
to Kotlin rules. `null` means no qualified reason was supplied, never clear.
The adapter negotiates the additive restrictions format with the loaded mod
DLL; an older mod without that capability receives its original Boolean field.
No native reservation is removed and no central release certificate is added.
