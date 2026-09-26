#pragma once
#include "platform/windows/runtime/texture_bridge.h"
#include "runtime/texture_commands.h"

namespace nimby::texture_bridge {
// Windows IPC adapter only. The caller owns the table lock and has acquired the
// pending request through Interlocked before invoking this function. Publish
// shared.result before releasing pending; volatile alone is not synchronization.
inline uint32_t dispatch(Shared& shared,Table& commands,uint64_t now) noexcept {
    Mailbox value;
    value.expected_database=shared.expected_database;
    value.expected_simulation=shared.expected_simulation;
    value.signal=shared.signal; value.expires=shared.expires;
    value.operation=shared.operation;
    value.active=shared.active; value.result_index=shared.result_index;
    value.active_count=shared.active_count; value.result_expiry=shared.result_expiry;
    value.request_signal=shared.request_signal; value.request_hash=shared.request_hash;
    value.request_expiry=shared.request_expiry; value.request_database=shared.request_database;
    value.request_simulation=shared.request_simulation; value.request_index=shared.request_index;
    value.request_alternate_index=shared.request_alternate_index;
    value.request_half_period_ms=shared.request_half_period_ms;
    const auto result=execute(value,commands,now);
    shared.expected_database=value.expected_database;
    shared.expected_simulation=value.expected_simulation;
    shared.signal=value.signal; shared.expires=value.expires;
    shared.active=value.active; shared.result_index=value.result_index;
    shared.active_count=value.active_count; shared.result_expiry=value.result_expiry;
    return result;
}
}
