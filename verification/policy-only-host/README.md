# Policy-only adapter verification

This Windows harness loads the packaged BC adapter and Kotlin library unchanged.
A synthetic SDK facade reports the test process as an isolated host, supplies an
in-memory saved preference and real Win32 action events, and records length
ownership. Unmodified SDK imports forward to a renamed runtime in the isolated
build directory. Process opening and session capture are counted and rejected.
No game is discovered, opened, launched or modified. The verifier is assigned
to a Job Object with a 192 MiB process commit limit before loading the mod. It
reports `PrivateUsage` and `PeakPagefileUsage`, not only resident working set.

Configure this separate CMake project with `POLICY_MOD` pointing at the packaged
`bc-train-super-long-mod.dll` and `POLICY_SDK` pointing at the matching runtime
`NimbyRailsFranceSDK.dll`. Build it and run CTest in that build directory.

The test covers the actual adapter's saved preference before registration,
remote and local option wakes, unchanged values, bounded contention retry,
revision retention, idle behavior, stop/restart and failed-start cleanup.
This memory result applies to the small policy-only BC mod in this synthetic
host; it does not qualify large maps or signalling mods under the same quota.
It does not qualify the broker transport, disk persistence, native train hooks,
game option rendering or composition behavior. Those need their own tests.
