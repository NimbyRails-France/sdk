# Train details and line plans (0.6.4)

Read-only profile for game SHA-256
`fff49ac21720abfc824c2b4f68b862727630eb0db71cfe1f9ea8f685d0db10ae`.

Passenger-count correction (0.6.6): Motion+0x48 is capacity, not onboard
passengers. The earlier inference from `140804000.c` was wrong. Use the native
Basics UI query described in [train-passengers.md](train-passengers.md).
Assignment evidence: `reports/train-timetable/14043f150.c`
reads Schedule optional +0x5f0, schedule ID +0x5d8, shift key +0x5e0;
+0x5e8 is the current order index. IDs describe assignment, not resolved names.

Line pool DB+0x180, stride 0x280, tag 4. Stops vector +0x118/+0x120/+0x128,
stride 0x158; track +0x78, optional station cache +0x110. A zero station cache
is resolved through the validated snapshot track. Nonzero conflicting IDs reject
the plan. Full-generation IDs are joined to tracks/stations from the snapshot.
`14035a1f0.c` uses signed offsets +0xb8/+0xbc for arrivals/departures.
`140355740.c` shows partial runs require a start-index correction; packed loop
adjustments also apply. Therefore these offsets are NOT exported as absolute
train times. The complete line plan may include stops outside a partial run.

Assignment fields must match two Motion reads. Passenger counts come from a separately validated query map. Line header and
stop vector must match repeated reads; reads are bounded to 16,384 stops per line
and 1,048,576 copied stops overall. Invalid timings retain the stop with its time
flag clear. Unknown values are not zero-valued observations. Snapshots remain
non-atomic across records; no writes or game hooks are introduced.

Manual platform labels are confirmed at Track+0xa8 by `14038af60.c`; automatic
labels also depend on geometry/direction suffixes. Platform labels and absolute
future timetable times are not included in this version.

Live probe on TER X 72011 / line L-184.2: indices 0/1/3 point to stations;
index 2 has track 0x10003a6580001, station cache zero, arrival=departure=276.
Its snapshot track is outside a station. Such waypoints are retained with
station_id=0; consumers must not attempt to resolve zero as a station ID.
