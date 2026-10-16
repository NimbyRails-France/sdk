#pragma once
#include <cstdint>

namespace nimby {
// A positive native refusal on a crossing of the selected approaching train's
// route. No value represents Clear; absence means no qualified observation.
enum class RouteConflict : std::uint8_t { CrossingOccupied=1,CrossingReserved=2 };
}
