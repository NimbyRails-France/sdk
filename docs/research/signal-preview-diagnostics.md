# Diagnosing a refused signal preview

These records diagnose the resident SDK and its adapter. They do not change the
mod API, authorization checks, placement plan or retry behavior.

The Hub diagnostic ZIP already collects both relevant components:

- `logs/sdk/NimbySignalUiBridge-experimental-v1_dll.log`: resident preview validation.
- `logs/mods/<module>_dll.log`: SDK signal-settings synchronization and mod workflow.
- `logs/loader/<module>_dll.log`: broker `Signal UI ownership rejected` records,
  if the channel rejects a token before calling the resident endpoint.

Correlate UTC timestamps, `provider`, `panel`, `signal`, `origin` and `service` with
the placement mod's `Placement preview` and `Preview renderer failure` records.
A successfully calculated plan does not imply that its drawing remains authorized.

## Resident refusal

`Signal preview rejected` records the original status, exact first failed check,
provider identity, source signal, requested count and copied validation context.
Epoch and revision pairs are `expected/current`; `generation` is the request's
generation. A zero context field means that no corresponding snapshot was obtained.
For a busy retry, current fields describe the last admitted validation snapshot.

| Reason | Evidence |
| --- | --- |
| `panel_inactive` | The signal panel's observations/context were suspended. |
| `provider_epoch_changed` | The tool provider suspended, resumed or changed session during a pending request. |
| `observation_epoch_changed` | The signal catalogue changed after admission, even if the source remains present. |
| `signal_missing` | The source is absent from this panel's currently observed catalogue. |
| `request_cancelled` | A clear, edit or another publication superseded the pending request. |
| `*_world_changed`, `*_generation_changed` | The provider, panel or store no longer matches the admitted context. |
| `preview_expired`, `provider_expired` | The respective drawing or worker lease expired. |
| `registry_busy`, `store_busy` | Validation could not acquire the respective lock within its bounded attempt. |
| `provider_missing`, `panel_missing`, `store_retired` | An owner was removed. |
| `service_missing`, `action_missing` | The drawing request is not authorized by the declared service/action. |

Status `9` remains invalid handle/context. Status `12` remains temporary resource
contention. Only the first failing check is reported; a new request revalidates all
checks. Malformed requests and unavailable native world/hooks have explicit reasons too.

## Follow the causal synchronization event

`Signal settings synchronization unavailable` names the panel and records the
session, world, generation, catalogue/batch count, reason and raw transport status.
For local payload validation failures, status may be `0`: the `reason` and `detail`
identify the malformed field/row. A recovery record is emitted once on return to
successful synchronization. Look here before attributing `panel_inactive` to the
placement calculation.

`external_suspension` means that the observation coordinator suspended the client
without providing a new snapshot. Its generation/world fields are deliberately
empty/zero; inspect the preceding worker capture or exception record for the cause.

No successful preview tick produces a diagnostic. Resident failures use a bounded
512-provider table, suppress repeats and admit at most four reports per provider
per five-second window. Active windows cannot be evicted by provider-token churn.
`suppressed` counts repeats since the previous admitted report. The existing sink
also limits bursts, rotates logs and never waits for a contended diagnostic lock.
Formatting and writing happen after releasing the action/store/limiter locks.
