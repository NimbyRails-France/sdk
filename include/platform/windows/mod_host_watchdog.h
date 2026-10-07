#pragma once
#include <windows.h>
#include <cstdint>

namespace nimby::mod_host {
// Watchdog deadlines measure awake time, not uptime: a sleeping PC gives no
// callback an opportunity to finish. Parent and child use the same system-wide
// clock, which excludes sleep/hibernation and is available since Windows 7.
// https://learn.microsoft.com/windows/win32/api/realtimeapiset/nf-realtimeapiset-queryunbiasedinterrupttime
inline uint64_t watchdogStamp(uint64_t awake100ns) noexcept {
    // Reserve zero for inactive work/stop markers, including at system boot.
    return awake100ns/10000+1;
}
inline uint64_t watchdogNow() noexcept {
    ULONGLONG awake{};
    // With a non-null output this API cannot fail on the supported Windows
    // targets. Never fall back to a clock with a different epoch or sleep bias.
    QueryUnbiasedInterruptTime(&awake);
    return watchdogStamp(awake);
}
inline bool watchdogExpired(uint64_t since,uint64_t now,uint32_t limitMs) noexcept {
    // A child can publish after the parent sampled now. A future marker is not
    // expired; unsigned subtraction alone would quarantine healthy startup.
    return since&&now>=since&&now-since>limitMs;
}
}
