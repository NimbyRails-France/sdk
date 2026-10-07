#pragma once
#include <platform/windows/mod_host_protocol.h>

namespace nimby::mod_host {
// Parent configuration comes from the supervisor, never from a worker packet.
// Call before launching workers; a zero value uses the current game process.
void configureEpochTarget(uint32_t targetPid);
// Child attachment installs the process-wide authority before any mod loads.
void installChildEpochAuthority();
// Operations 2 (begin) and 3 (validate). The parent reads world/clock itself.
uint32_t dispatchEpoch(const Request&,Reply&,Owners&);
}
