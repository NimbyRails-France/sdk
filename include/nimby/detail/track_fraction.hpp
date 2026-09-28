#pragma once
#include <limits>

namespace nimby::detail {
// Native head positions, signal positions and footprint endpoints can differ
// by a few double rounding units at the same physical point. Approach and
// occupation must agree about that contact. This is a normalized-fraction
// numerical tolerance, never a distance margin or a permission to move.
inline constexpr double trackFractionRounding = 8 * std::numeric_limits<double>::epsilon();
}
