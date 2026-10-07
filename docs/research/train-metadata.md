# Native train metadata, Windows 1.19

This profile is restricted to executable SHA-256
`fff49ac21720abfc824c2b4f68b862727630eb0db71cfe1f9ea8f685d0db10ae`.
Addresses below are research addresses at image base `0x140000000`, never
functions called by the reader. Ghidra 12.1.3 exports, disassembly and invocation
provenance are retained in `.validation/sdk-isolation/train-data-research` in
the development workspace. The independent `probe-train-data.py` checks these
fields through read-only process reads. These additions do not qualify Linux
layouts.

## Tags and line relationships

The Train pool is `DB+0x200`, stride `0x178`. Its declared tag vector is at
`Train+0x80/+0x88/+0x90`. The Line pool is `DB+0x180`, stride `0x280`; parent
ID is at `+0x10`, name at `+0x78` and kind at `+0xfc`. A root line declares
tags at `+0xc8`; a descendant declares its own tags at `+0xe0`. The native
line/tag serializer and consumer exports are preserved with the research
artifacts. Parent IDs retain index and generation; a missing, self-referential
or mismatched parent is unavailable. Readers expose declared tags and the
parent relationship separately; they do not flatten inheritance into native
records.

`0x1404ba780` serializes tag catalogue entries. The catalogue pointer is at
`DB+0x420`, its bucket table at `+0x10` and bucket count at `+0x18`. The table
contains `count+1` heads, with the final entry acting as the sentinel. A node
is `0x88` bytes: opaque `uint64` key at `+0`, native string at `+0x18`, next
pointer at `+0x80`. Tag IDs are not entity IDs; no generation-domain test is
applied to them. A stable ID reference can remain available when its catalogue
name cannot be read.

Limits are 16,384 catalogue tags, 65,536 hash buckets, 4,096 declared tags per
object and 262,144 tag references per capture. Cycles, duplicate nodes/keys,
duplicate declared tags, malformed vectors and changed content are rejected.
Headers, IDs, pool selection, block tables, strings and referenced values are
checked again; a pool replacement cannot make a still-readable old address a
current object. These checks detect observed changes, not atomicity across the
game's simulation thread.

## Predicted arrival delay

`0x14043e0c0` implements the native estimate. The assembly performs `SUBSS`,
`DIVSS`, `MAXSS`, then `CVTTSS2SI`; preserving float arithmetic and truncation
matters. For Drive at `Motion+0x290`, the sample count is `Drive+0x1e0`,
distance `+0x1dc`, progress `+0x1e4` and estimated speed `+0x1e8`.

Eligibility requires Drive present (`Motion+0x4b0=1`), no TimedStop
(`+0x4d0=0`), at least 10 samples, distance at least 10, speed at least 1,
a valid positive arrival deadline (`Motion+0x488`) and a qualified simulation
clock. The current track (`Motion+0x3a8`) must differ from the Drive endpoints
at `+0x30` and `+0x48`, unless it is zero. Relevant fields must agree in the
two existing Motion copies.

```
seconds = trunc(max(float((distance - progress) / speed), 0))
predicted_delay_us = seconds * 1,000,000 - arrival_deadline_us + game_time_us
```

Finite values and integer-overflow checks precede conversion. Negative is
predicted early, zero is a valid on-time estimate, positive is predicted late.
This differs from the signed difference between the planned arrival and now.
An ineligible estimate is unavailable, never a fabricated zero.

## Configured and current characteristics

`0x1403157f0` serializes Dynamics. The native train UI `0x1407f3600` selects
configured Dynamics at `Train+0xc0` or current Dynamics at `Motion+8`.
`0x14043df60`, `0x14043e030` and `0x140378600` provide consumer/integrator
evidence for physical quantities. Within Dynamics, the implemented fields are:

| Offset | Meaning | Unit |
| --- | --- | --- |
| `+0/+8/+0x10` | Cars vector, stride 32 | count |
| `+0x1c` | Maximum speed | m/s |
| `+0x20` | Maximum acceleration | m/s² |
| `+0x2c` | Tractive force | N |
| `+0x30` | Power | W |
| `+0x34` | Empty mass | kg |
| `+0x38` | Length | m |
| `+0x40` | Passenger capacity | count |

The two Dynamics blocks must match; each scalar has a separate validity flag.
Values must be finite, nonnegative and bounded. Configured data requires a
fresh model Dynamics read and stable full train identity. Current data reuses
the two Motion copies and is never replaced with configured values. Fields
whose semantics are outside this contract remain unexposed.

## Vehicle composition and referenced models

`0x14031d180` traverses `vector<CarSetup>` in 32-byte steps and dispatches
`0x1403156b0` for each element. The first `uint64` is its resource/model hash.
Configured and current compositions come from their respective Dynamics
vectors. A missing or changed current composition is never replaced by the
configured one. Order and repeated car models are preserved.

The native UI `0x1407f3600` resolves model keys through Rules at `DB+0xa80`:
bucket pointer `Rules+0x38`, count `Rules+0x40`, bucket `key % count`, final
sentinel at index `count`. Nodes are `0x2a8` bytes, with key at `+0` and next
at `+0x2a0`. The exposed native strings are resource code `+8`, English model
name `+0x50` and source mod name `+0x70`. The localization key at `+0x30` is
not passed off as a translated display name. Live inspection and the loaded
workshop asset `2430447984/mod.txt` corroborate the four-car B 82500 resource
codes and labels. No passenger/freight classification is inferred from names.

Composition reads revalidate the full owner ID, pool selection, Dynamics
headers and both copies of all CarSetup bytes. Limits are 4,096 cars per
composition and 262,144 cars per capture. Model resolution groups the distinct
referenced hashes by bucket and traverses each requested bucket once; it does
not scan unrelated models. At most 16,384 distinct models/nodes are processed.
Nodes, strings, bucket heads, sentinel and table descriptor are rechecked.
A missing or unreadable model description does not erase a known composition.

## Focused validation

`train_metadata` exercises the production reader with independently mutable
remote-memory fixtures: signed/zero ETA, float truncation and thresholds,
configured/current differences, malformed values, full-ID reuse, pool
replacement, in-place edits, catalogue cycles and both published tag limits.
It also checks failure isolation and that retained results own their bytes.

Each request flag is tested independently. Service, characteristics,
timetables, tags, passengers, locations, line metadata and composition do not read unrelated
catalogues. Timetables include service; tags include line relationships.
Path vectors can be explicitly excluded. Existing callers keep their prior
default behavior. Presence-only captures perform exactly the same 27 reads /
5,880 bytes in the one-train fixture with or without a metadata output.
Composition tests cover independent configured/current failures, known empty
versus unavailable, missing model descriptions, cycles, changed headers and
the published budgets. A 10,000-car fixture referencing two models resolves
only those two models, with two complete reads of each node.
