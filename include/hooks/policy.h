#pragma once
#include <nimby/detail/sdk.h>
namespace nimby::hooks {
// MinHook is linked privately, but no validated game target or activation path exists.
// Recognition of a research file must never be sufficient authorization.
constexpr uint32_t request_activation() noexcept { return NIMBY_HOOKS_UNAVAILABLE; }
}
