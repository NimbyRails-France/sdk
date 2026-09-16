# Onboard passenger count: correction in 0.6.6

Profile SHA-256: fff49ac21720abfc824c2b4f68b862727630eb0db71cfe1f9ea8f685d0db10ae.

SDK 0.6.4/0.6.5 incorrectly interpreted Motion+0x48 as onboard passengers.
The live Z58000-030K test proved that it held capacity (1564) while the native
Basics panel showed 0 / 1564. This source is no longer used for passenger_count.

Native evidence in `reports/train-passengers/`:
- `1407fd860`: Sim+0x2208 points to the passenger query. Buckets at query+0x90,
  bucket count at +0x98. Lookup uses the full train ID modulo bucket count.
  Node +0 is train ID, +8 contains the 32-bit passenger count, +12 another
  field (not part of the count), +16 the next pointer. Buckets[count] is sentinel.
  The function initializes local_480 to zero when the key is absent.
- The closure carries &local_480 through 14080bea0 / 14080e3d0 / 140814bd0 /
  1407fe960 to parameter 15 of 1407f3600.
- `reports/trains/1407f3600.c`: the Basics `Pax / max` display reads the first
  32 bits of parameter 15 for onboard passengers and a different source for capacity.

SDK reads and validates the map with bounded buckets/nodes, full IDs, correct
hash bucket, nonnegative counts, unique nodes/IDs, cycle rejection, repeated
node/header/bucket/root-pointer reads and up to three attempts. Missing keys
become zero only when the whole table was successfully observed. Unavailable
or changing query data leaves NIMBY_TRAIN_PASSENGERS_VALID clear. The existing
Presence/state validity requirement is retained; old structures and ABI stay unchanged.

Synthetic regressions cover 123 occupants versus 1564 capacity, zero occupants,
failed reads, negative values, cycles, different generations and changing nodes.
Live paused UI comparison on 2033-10-14 at 22:16:56 game time: Z58000-030K
(id 1407374892007427) shows 0 / 1564 in game and passenger_count=0 in SDK.
Other observed query values: RER z55544=28 and RC 72013=228; these were also
returned by the public API. Those two were not visually cross-checked.
