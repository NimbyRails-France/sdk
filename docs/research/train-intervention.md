# Native train intervention — 0.7.1 evidence

Profile SHA-256: `fff49ac21720abfc824c2b4f68b862727630eb0db71cfe1f9ea8f685d0db10ae`.
Addresses below are RVAs, not ASLR addresses. The associated exports are in
`reports/intervention`. Decompiled signatures remain inferred until runtime validation.

The UI's “All trains intervention” creates a `cmd::train::Intervention` using
`0x5bc270`, then writes mode **3** at command offset `0x20` (`0x6678f0`).
Mode 2 means stale trains; mode 0 takes an explicit train set.

Execution (`0x303620`) collects eligible trains with `0x44cc70` and applies
`0x44c9b0`. The collection excludes train order mode 2. Each intervention
uses `0x44c810`, charges the simulation balance, and updates accounting.
The command increments Sim `+0x2120` afterwards.

The core intervention also resets coupled train motions, relocates passengers,
clears passenger containers, resets dispatch/stop/route state with `0x43e300`,
and reloads the train configuration with `0x44bb80`. Clearing only dispatch or
timed-stop flags is therefore not equivalent. The game describes the result
as respawning at the next destination, not interpolating the theoretical
position at the requested time.

The scripting entry point `0x4415c0` queues a `SimCmd::TrainIntervention`:
game allocator, vtable `0xa744c8`, train ID at `+8`, controller mutex at `+0x58`,
enqueue `0x461330`. Its execution `0x440e00` calls the same core intervention.
The controller address and its lifetime have not yet been validated.

The simulation worker (`0x3489d0`) calls the update `0x478000`, the post-update
`0x478f40`, then snapshot production `0x47a5f0`. `0x478f40` receives the Sim,
a shared context, the worker's database, and a flag. The dedicated clock bridge
uses the return boundary of this function, on the simulation worker and within
its shared database lock. The request also checks the current root's Sim and DB.

Runtime validation: PID 32448, thread 10088, Sim 2288067749904 and DB
2288421941984 match independently read roots. CLion paused the external probe
at `clock_bridge_probe.cpp:17`, with 1215 callbacks and no mutation requested.
The controlled mutation then applied 345 interventions among 381 trains (36
mothballed). The installed API's 1940 test observed 34 subsequent departures,
the selected train relocating from Kaiserslautern to its Frankfurt depot, and
its new waiting time. See `../simulation-clock.md` for results and limitations.

The regular diagnostic SDK still enables no game hooks. The separate versioned
clock bridge installs one hook only on the explicit recalculation API call.
